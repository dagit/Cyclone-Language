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
struct _fat_ptr Cyc_substring(struct _fat_ptr,int ofs,unsigned long n);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 289
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 302
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 448
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 455
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
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 693 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 852
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 881
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 885
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 890
void*Cyc_Absyn_compress_kb(void*);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 903
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 906
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 908
extern void*Cyc_Absyn_sint_type;
# 910
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 917
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 944
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 948
void*Cyc_Absyn_bounds_one();
# 950
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 972
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 975
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
# 983
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 986
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1024
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
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1050
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1055
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1066
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1069
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1072
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1076
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
# 1081
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1083
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1090
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1100
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1105
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1134
int Cyc_Absyn_fntype_att(void*);
# 1159
extern int Cyc_Absyn_porting_c_code;
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 45 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 94
void*Cyc_Tcutil_copy_type(void*);
# 107
void*Cyc_Tcutil_compress(void*);
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 149
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 154
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 156
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 158
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 163
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 298
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 308
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
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
({struct Cyc_List_List*_tmp76D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp76D;});
({struct _tuple11*_tmp76E=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp76E;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
({void*_tmpA=0U;({unsigned _tmp770=loc;struct _fat_ptr _tmp76F=({const char*_tmpB="qualifier on type is ignored";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmp770,_tmp76F,_tag_fat(_tmpA,sizeof(void*),0U));});});}
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
return({struct _tuple15 _tmp6CE;_tmp6CE.f1=nullable,_tmp6CE.f2=bound,_tmp6CE.f3=zeroterm,_tmp6CE.f4=rgn;_tmp6CE;});}
# 239
struct _tuple0*Cyc_Parse_gensym_enum(){
# 241
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp774=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp774;}),({
struct _fat_ptr*_tmp773=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp772=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6CF;_tmp6CF.tag=1U,_tmp6CF.f1=(unsigned long)enum_counter ++;_tmp6CF;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp771=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp771,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp772;});_tmp13;});_tmp14->f2=_tmp773;});_tmp14;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 251
struct _tuple18*_tmp15=field_info;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;struct _tuple0*_tmp17;unsigned _tmp16;_LL1: _tmp16=(_tmp15->f1)->f1;_tmp17=(_tmp15->f1)->f2;_tmp18=(_tmp15->f1)->f3;_tmp19=(_tmp15->f1)->f4;_tmp1A=(_tmp15->f1)->f5;_tmp1B=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1D=(_tmp15->f2)->f2;_LL2: {unsigned varloc=_tmp16;struct _tuple0*qid=_tmp17;struct Cyc_Absyn_Tqual tq=_tmp18;void*t=_tmp19;struct Cyc_List_List*tvs=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*widthopt=_tmp1C;struct Cyc_Absyn_Exp*reqopt=_tmp1D;
if(tvs != 0)
({void*_tmp1E=0U;({unsigned _tmp776=loc;struct _fat_ptr _tmp775=({const char*_tmp1F="bad type params in struct field";_tag_fat(_tmp1F,sizeof(char),32U);});Cyc_Warn_err(_tmp776,_tmp775,_tag_fat(_tmp1E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp20=0U;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp21="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp21,sizeof(char),59U);});Cyc_Warn_err(_tmp778,_tmp777,_tag_fat(_tmp20,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->name=(*qid).f2,_tmp22->tq=tq,_tmp22->type=t,_tmp22->width=widthopt,_tmp22->attributes=atts,_tmp22->requires_clause=reqopt;_tmp22;});}}
# 261
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp6D0;_tmp6D0.Signed_spec=0,_tmp6D0.Unsigned_spec=0,_tmp6D0.Short_spec=0,_tmp6D0.Long_spec=0,_tmp6D0.Long_Long_spec=0,_tmp6D0.Valid_type_spec=0,_tmp6D0.Type_spec=Cyc_Absyn_sint_type,_tmp6D0.loc=loc;_tmp6D0;});}
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
void*_tmp779=t;Cyc_Tcutil_promote_array(_tmp779,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 315 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;struct Cyc_List_List*res=_tmp28;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp2=_tmp29;struct _tuple8*_tmp2A=_stmttmp2;void**_tmp2C;struct _fat_ptr _tmp2B;void*_tmp2E;struct _fat_ptr*_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp2D=_tmp2A->f1;_tmp2E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp2D != 0){_LL2: {struct _fat_ptr*v=_tmp2D;void*i=_tmp2E;
# 320
{void*_tmp2F=i;void**_tmp30;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1U){_LL8: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {void**z=_tmp30;
# 324
struct _fat_ptr*nm=({struct _fat_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _fat_ptr _tmp77B=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp6D1;_tmp6D1.tag=0U,_tmp6D1.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp6D1;});void*_tmp33[1U];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp77A=({const char*_tmp34="`%s";_tag_fat(_tmp34,sizeof(char),4U);});Cyc_aprintf(_tmp77A,_tag_fat(_tmp33,sizeof(void*),1U));});});*_tmp36=_tmp77B;});_tmp36;});
({void*_tmp77D=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm,_tmp32->identity=- 1,({void*_tmp77C=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=0U,_tmp31->f1=& Cyc_Tcutil_ik;_tmp31;});_tmp32->kind=_tmp77C;});_tmp32;}));*z=_tmp77D;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 329
res=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _tuple19*_tmp77E=({struct _tuple19*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=v,_tmp37->f2=i;_tmp37;});_tmp38->hd=_tmp77E;}),_tmp38->tl=res;_tmp38;});goto _LL0;}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp2B=*_tmp2A->f1;_tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp2B;void**z=_tmp2C;
# 333
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp780=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp6D2;_tmp6D2.tag=0U,_tmp6D2.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp6D2;});void*_tmp3B[1U];_tmp3B[0]=& _tmp3D;({struct _fat_ptr _tmp77F=({const char*_tmp3C="`%s";_tag_fat(_tmp3C,sizeof(char),4U);});Cyc_aprintf(_tmp77F,_tag_fat(_tmp3B,sizeof(void*),1U));});});*_tmp3E=_tmp780;});_tmp3E;});
({void*_tmp782=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=nm,_tmp3A->identity=- 1,({void*_tmp781=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=& Cyc_Tcutil_rk;_tmp39;});_tmp3A->kind=_tmp781;});_tmp3A;}));*z=_tmp782;});
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
res=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _tuple19*_tmp783=({struct _tuple19*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp43->f2=i;_tmp43;});_tmp44->hd=_tmp783;}),_tmp44->tl=res;_tmp44;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
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
return({void*_tmp785=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp784=Cyc_Tcutil_copy_type(i);_tmp4D->f1=_tmp784;});_tmp4D;});Cyc_Absyn_new_exp(_tmp785,e->loc);});}}}
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
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D3;_tmp6D3.elt_type=et2,_tmp6D3.elt_tq=tq,(_tmp6D3.ptr_atts).rgn=r,(_tmp6D3.ptr_atts).nullable=n,(_tmp6D3.ptr_atts).bounds=b2,(_tmp6D3.ptr_atts).zero_term=zt,(_tmp6D3.ptr_atts).ptrloc=pl;_tmp6D3;}));
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
({void*_tmp786=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp786;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
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
if(({struct _fat_ptr _tmp787=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp787,({const char*_tmp64="`H";_tag_fat(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;else{
if(({struct _fat_ptr _tmp788=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp788,({const char*_tmp65="`U";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;else{
if(({struct _fat_ptr _tmp789=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp789,({const char*_tmp66="`RC";_tag_fat(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;else{
# 436
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _fat_ptr*_tmp78A=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp78A;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}}}}
# 443
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp69=t;struct Cyc_Absyn_Tvar*_tmp6A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->tag == 2U){_LL1: _tmp6A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp6A;
return pr;}}else{_LL3: _LL4:
({void*_tmp6B=0U;({unsigned _tmp78C=loc;struct _fat_ptr _tmp78B=({const char*_tmp6C="expecting a list of type variables, not types";_tag_fat(_tmp6C,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp78C,_tmp78B,_tag_fat(_tmp6B,sizeof(void*),0U));});});}_LL0:;}
# 451
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmp6D;void*_tmp6E=_stmttmp6;void**_tmp6F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->tag == 2U){_LL1: _tmp6F=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1)->kind;_LL2: {void**cptr=_tmp6F;
# 454
void*_tmp70=Cyc_Absyn_compress_kb(*cptr);void*_stmttmp7=_tmp70;void*_tmp71=_stmttmp7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp71)->tag == 1U){_LL6: _LL7:
# 456
 if(!leq)({void*_tmp78D=Cyc_Tcutil_kind_to_bound(k);*cptr=_tmp78D;});else{
({void*_tmp78E=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=2U,_tmp72->f1=0,_tmp72->f2=k;_tmp72;});*cptr=_tmp78E;});}
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
void*_tmp73=(void*)tms->hd;void*_stmttmp8=_tmp73;void*_tmp74=_stmttmp8;void*_tmp75;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->tag == 3U){_LL1: _tmp75=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->f1;_LL2: {void*args=_tmp75;
# 482
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 485
void*_tmp76=args;struct Cyc_List_List*_tmp77;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp76)->tag == 1U){_LL6: _LL7:
# 487
({void*_tmp78=0U;({unsigned _tmp790=loc;struct _fat_ptr _tmp78F=({const char*_tmp79="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp79,sizeof(char),93U);});Cyc_Warn_warn(_tmp790,_tmp78F,_tag_fat(_tmp78,sizeof(void*),0U));});});
# 489
return tms;}else{_LL8: _tmp77=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp76)->f1;_LL9: {struct Cyc_List_List*ids=_tmp77;
# 491
if(({int _tmp791=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ids);_tmp791 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp7A=0U;({unsigned _tmp793=loc;struct _fat_ptr _tmp792=({const char*_tmp7B="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp7B,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp793,_tmp792,_tag_fat(_tmp7A,sizeof(void*),0U));});});{
# 495
struct Cyc_List_List*rev_new_params=0;
for(0;ids != 0;ids=ids->tl){
struct Cyc_List_List*_tmp7C=tds;struct Cyc_List_List*tds2=_tmp7C;
for(0;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp7D=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp7D;
void*_tmp7E=x->r;void*_stmttmp9=_tmp7E;void*_tmp7F=_stmttmp9;struct Cyc_Absyn_Vardecl*_tmp80;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7F)->tag == 0U){_LLB: _tmp80=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7F)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp80;
# 502
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({void*_tmp81=0U;({unsigned _tmp795=x->loc;struct _fat_ptr _tmp794=({const char*_tmp82="initializer found in parameter declaration";_tag_fat(_tmp82,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp795,_tmp794,_tag_fat(_tmp81,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({void*_tmp83=0U;({unsigned _tmp797=x->loc;struct _fat_ptr _tmp796=({const char*_tmp84="namespaces forbidden in parameter declarations";_tag_fat(_tmp84,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp797,_tmp796,_tag_fat(_tmp83,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*_tmp86));
({struct _tuple8*_tmp798=({struct _tuple8*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=(*vd->name).f2,_tmp85->f2=vd->tq,_tmp85->f3=vd->type;_tmp85;});_tmp86->hd=_tmp798;}),_tmp86->tl=rev_new_params;_tmp86;});
# 511
goto L;}}else{_LLD: _LLE:
({void*_tmp87=0U;({unsigned _tmp79A=x->loc;struct _fat_ptr _tmp799=({const char*_tmp88="nonvariable declaration in parameter type";_tag_fat(_tmp88,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79A,_tmp799,_tag_fat(_tmp87,sizeof(void*),0U));});});}_LLA:;}
# 515
L: if(tds2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp6D4;_tmp6D4.tag=0U,_tmp6D4.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)ids->hd));_tmp6D4;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({unsigned _tmp79C=loc;struct _fat_ptr _tmp79B=({const char*_tmp8A="%s is not given a type";_tag_fat(_tmp8A,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79C,_tmp79B,_tag_fat(_tmp89,sizeof(void*),1U));});});}
# 518
return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));
({void*_tmp79F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8D=_region_malloc(yy,sizeof(*_tmp8D));_tmp8D->tag=3U,({void*_tmp79E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));_tmp8C->tag=1U,({struct Cyc_List_List*_tmp79D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp8C->f1=_tmp79D;}),_tmp8C->f2=0,_tmp8C->f3=0,_tmp8C->f4=0,_tmp8C->f5=0,_tmp8C->f6=0,_tmp8C->f7=0;_tmp8C;});_tmp8D->f1=_tmp79E;});_tmp8D;});_tmp8E->hd=_tmp79F;}),_tmp8E->tl=0;_tmp8E;});}}}_LL5:;}
# 525
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8F=_region_malloc(yy,sizeof(*_tmp8F));_tmp8F->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7A0=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8F->tl=_tmp7A0;});_tmp8F;});}_LL0:;}}
# 533
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 537
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6D5;_tmp6D5.id=d.id,_tmp6D5.varloc=d.varloc,({struct Cyc_List_List*_tmp7A1=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6D5.tms=_tmp7A1;});_tmp6D5;});{
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
enum Cyc_Parse_Storage_class _tmp90=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _stmttmpA=_tmp90;enum Cyc_Parse_Storage_class _tmp91=_stmttmpA;switch(_tmp91){case Cyc_Parse_Extern_sc: _LL1: _LL2:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL3: _LL4:
 sc=0U;goto _LL0;default: _LL5: _LL6:
({void*_tmp92=0U;({unsigned _tmp7A3=loc;struct _fat_ptr _tmp7A2=({const char*_tmp93="bad storage class on function";_tag_fat(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp7A3,_tmp7A2,_tag_fat(_tmp92,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 559
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp94;
struct _tuple14 _tmp95=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpB=_tmp95;struct _tuple14 _tmp96=_stmttmpB;struct Cyc_List_List*_tmp9A;struct Cyc_List_List*_tmp99;void*_tmp98;struct Cyc_Absyn_Tqual _tmp97;_LL8: _tmp97=_tmp96.f1;_tmp98=_tmp96.f2;_tmp99=_tmp96.f3;_tmp9A=_tmp96.f4;_LL9: {struct Cyc_Absyn_Tqual fn_tqual=_tmp97;void*fn_type=_tmp98;struct Cyc_List_List*x=_tmp99;struct Cyc_List_List*out_atts=_tmp9A;
# 564
if(x != 0)
# 567
({void*_tmp9B=0U;({unsigned _tmp7A5=loc;struct _fat_ptr _tmp7A4=({const char*_tmp9C="bad type params, ignoring";_tag_fat(_tmp9C,sizeof(char),26U);});Cyc_Warn_warn(_tmp7A5,_tmp7A4,_tag_fat(_tmp9B,sizeof(void*),0U));});});{
# 569
void*_tmp9D=fn_type;struct Cyc_Absyn_FnInfo _tmp9E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9D)->tag == 5U){_LLB: _tmp9E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9D)->f1;_LLC: {struct Cyc_Absyn_FnInfo i=_tmp9E;
# 571
{struct Cyc_List_List*_tmp9F=i.args;struct Cyc_List_List*args2=_tmp9F;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpA0=0U;({unsigned _tmp7A7=loc;struct _fat_ptr _tmp7A6=({const char*_tmpA1="missing argument variable in function prototype";_tag_fat(_tmpA1,sizeof(char),48U);});Cyc_Warn_err(_tmp7A7,_tmp7A6,_tag_fat(_tmpA0,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp7A9=({struct _fat_ptr*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct _fat_ptr _tmp7A8=({const char*_tmpA2="?";_tag_fat(_tmpA2,sizeof(char),2U);});*_tmpA3=_tmp7A8;});_tmpA3;});(*((struct _tuple8*)args2->hd)).f1=_tmp7A9;});}}}
# 578
({struct Cyc_List_List*_tmp7AA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp7AA;});
return({struct Cyc_Absyn_Fndecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->sc=sc,_tmpA4->is_inline=is_inline,_tmpA4->name=d.id,_tmpA4->body=body,_tmpA4->i=i,_tmpA4->cached_type=0,_tmpA4->param_vardecls=0,_tmpA4->fn_vardecl=0;_tmpA4;});}}else{_LLD: _LLE:
# 582
({void*_tmpA5=0U;({unsigned _tmp7AC=loc;struct _fat_ptr _tmp7AB=({const char*_tmpA6="declarator is not a function prototype";_tag_fat(_tmpA6,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7AC,_tmp7AB,_tag_fat(_tmpA5,sizeof(void*),0U));});});}_LLA:;}}}}}static char _tmpA7[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
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
({void*_tmpAB=0U;({unsigned _tmp7AE=loc;struct _fat_ptr _tmp7AD=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7AE,_tmp7AD,_tag_fat(_tmpAB,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpAC=0U;({unsigned _tmp7B0=loc;struct _fat_ptr _tmp7AF=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B0,_tmp7AF,_tag_fat(_tmpAC,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpAD=0U;({unsigned _tmp7B2=loc;struct _fat_ptr _tmp7B1=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B2,_tmp7B1,_tag_fat(_tmpAD,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpAE=0U;({unsigned _tmp7B4=loc;struct _fat_ptr _tmp7B3=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B4,_tmp7B3,_tag_fat(_tmpAE,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpAF=0U;({unsigned _tmp7B6=loc;struct _fat_ptr _tmp7B5=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7B6,_tmp7B5,_tag_fat(_tmpAF,sizeof(void*),0U));});});else{
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
({void*_tmpB0=0U;({unsigned _tmp7B8=loc;struct _fat_ptr _tmp7B7=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7B8,_tmp7B7,_tag_fat(_tmpB0,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpB1=0U;({unsigned _tmp7BA=loc;struct _fat_ptr _tmp7B9=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7BA,_tmp7B9,_tag_fat(_tmpB1,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 652
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpB2=0U;({unsigned _tmp7BC=loc;struct _fat_ptr _tmp7BB=({const char*_tmpB3="missing type within specifier";_tag_fat(_tmpB3,sizeof(char),30U);});Cyc_Warn_warn(_tmp7BC,_tmp7BB,_tag_fat(_tmpB2,sizeof(void*),0U));});});
t=Cyc_Absyn_int_type(sgn,sz);}else{
# 657
if(seen_sign){
void*_tmpB4=t;enum Cyc_Absyn_Size_of _tmpB6;enum Cyc_Absyn_Sign _tmpB5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->tag == 1U){_LL1: _tmpB5=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f1;_tmpB6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpB5;enum Cyc_Absyn_Size_of sz2=_tmpB6;
# 660
if((int)sgn2 != (int)sgn)
t=Cyc_Absyn_int_type(sgn,sz2);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpB7=0U;({unsigned _tmp7BE=loc;struct _fat_ptr _tmp7BD=({const char*_tmpB8="sign specification on non-integral type";_tag_fat(_tmpB8,sizeof(char),40U);});Cyc_Warn_err(_tmp7BE,_tmp7BD,_tag_fat(_tmpB7,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 665
if(seen_size){
void*_tmpB9=t;enum Cyc_Absyn_Size_of _tmpBB;enum Cyc_Absyn_Sign _tmpBA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)){case 1U: _LL6: _tmpBA=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f1;_tmpBB=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f2;_LL7: {enum Cyc_Absyn_Sign sgn2=_tmpBA;enum Cyc_Absyn_Size_of sz2=_tmpBB;
# 668
if((int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL5;}case 2U: _LL8: _LL9:
# 672
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpBC=0U;({unsigned _tmp7C0=loc;struct _fat_ptr _tmp7BF=({const char*_tmpBD="size qualifier on non-integral type";_tag_fat(_tmpBD,sizeof(char),36U);});Cyc_Warn_err(_tmp7C0,_tmp7BF,_tag_fat(_tmpBC,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 676
return t;}
# 679
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 683
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpBE=d.id;struct _tuple0*q=_tmpBE;
unsigned _tmpBF=d.varloc;unsigned varloc=_tmpBF;
struct _tuple14 _tmpC0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpC=_tmpC0;struct _tuple14 _tmpC1=_stmttmpC;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*_tmpC4;void*_tmpC3;struct Cyc_Absyn_Tqual _tmpC2;_LL1: _tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;_tmpC4=_tmpC1.f3;_tmpC5=_tmpC1.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpC2;void*new_typ=_tmpC3;struct Cyc_List_List*tvs=_tmpC4;struct Cyc_List_List*atts=_tmpC5;
# 690
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpC7=_region_malloc(r,sizeof(*_tmpC7));({struct _tuple16*_tmp7C1=({struct _tuple16*_tmpC6=_region_malloc(r,sizeof(*_tmpC6));_tmpC6->f1=varloc,_tmpC6->f2=q,_tmpC6->f3=tq2,_tmpC6->f4=new_typ,_tmpC6->f5=tvs,_tmpC6->f6=atts;_tmpC6;});_tmpC7->hd=_tmp7C1;}),_tmpC7->tl=0;_tmpC7;});else{
# 693
return({struct Cyc_List_List*_tmpC9=_region_malloc(r,sizeof(*_tmpC9));({struct _tuple16*_tmp7C7=({struct _tuple16*_tmpC8=_region_malloc(r,sizeof(*_tmpC8));_tmpC8->f1=varloc,_tmpC8->f2=q,_tmpC8->f3=tq2,_tmpC8->f4=new_typ,_tmpC8->f5=tvs,_tmpC8->f6=atts;_tmpC8;});_tmpC9->hd=_tmp7C7;}),({
struct Cyc_List_List*_tmp7C6=({struct _RegionHandle*_tmp7C5=r;struct Cyc_Absyn_Tqual _tmp7C4=tq;void*_tmp7C3=Cyc_Tcutil_copy_type(t);struct _tuple11*_tmp7C2=ds->tl;Cyc_Parse_apply_tmss(_tmp7C5,_tmp7C4,_tmp7C3,_tmp7C2,shared_atts);});_tmpC9->tl=_tmp7C6;});_tmpC9;});}}}}
# 697
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 700
if(tms == 0)return({struct _tuple14 _tmp6D6;_tmp6D6.f1=tq,_tmp6D6.f2=t,_tmp6D6.f3=0,_tmp6D6.f4=atts;_tmp6D6;});{
void*_tmpCA=(void*)tms->hd;void*_stmttmpD=_tmpCA;void*_tmpCB=_stmttmpD;struct Cyc_List_List*_tmpCD;unsigned _tmpCC;struct Cyc_Absyn_Tqual _tmpCF;struct Cyc_Absyn_PtrAtts _tmpCE;unsigned _tmpD1;struct Cyc_List_List*_tmpD0;void*_tmpD2;unsigned _tmpD5;void*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;unsigned _tmpD7;void*_tmpD6;switch(*((int*)_tmpCB)){case 0U: _LL1: _tmpD6=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpD7=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL2: {void*zeroterm=_tmpD6;unsigned ztloc=_tmpD7;
# 703
return({struct Cyc_Absyn_Tqual _tmp7CA=Cyc_Absyn_empty_tqual(0U);void*_tmp7C9=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 703
struct Cyc_List_List*_tmp7C8=atts;Cyc_Parse_apply_tms(_tmp7CA,_tmp7C9,_tmp7C8,tms->tl);});}case 1U: _LL3: _tmpD3=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpD4=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_tmpD5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpD3;void*zeroterm=_tmpD4;unsigned ztloc=_tmpD5;
# 706
return({struct Cyc_Absyn_Tqual _tmp7CD=Cyc_Absyn_empty_tqual(0U);void*_tmp7CC=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 706
struct Cyc_List_List*_tmp7CB=atts;Cyc_Parse_apply_tms(_tmp7CD,_tmp7CC,_tmp7CB,tms->tl);});}case 3U: _LL5: _tmpD2=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_LL6: {void*args=_tmpD2;
# 709
void*_tmpD8=args;unsigned _tmpD9;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_List_List*_tmpDE;void*_tmpDD;struct Cyc_Absyn_VarargInfo*_tmpDC;int _tmpDB;struct Cyc_List_List*_tmpDA;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->tag == 1U){_LLE: _tmpDA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f1;_tmpDB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f2;_tmpDC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f3;_tmpDD=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f4;_tmpDE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f5;_tmpDF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f6;_tmpE0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f7;_LLF: {struct Cyc_List_List*args2=_tmpDA;int c_vararg=_tmpDB;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpDC;void*eff=_tmpDD;struct Cyc_List_List*rgn_po=_tmpDE;struct Cyc_Absyn_Exp*req=_tmpDF;struct Cyc_Absyn_Exp*ens=_tmpE0;
# 711
struct Cyc_List_List*typvars=0;
# 713
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)as->hd,_tmpE1->tl=fn_atts;_tmpE1;});else{
# 718
new_atts=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=(void*)as->hd,_tmpE2->tl=new_atts;_tmpE2;});}}}
# 721
if(tms->tl != 0){
void*_tmpE3=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpE=_tmpE3;void*_tmpE4=_stmttmpE;struct Cyc_List_List*_tmpE5;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE4)->tag == 4U){_LL13: _tmpE5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE4)->f1;_LL14: {struct Cyc_List_List*ts=_tmpE5;
# 724
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
# 728
 goto _LL12;}_LL12:;}
# 732
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 737
args2=0;{
# 740
struct Cyc_List_List*_tmpE6=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpE6;
# 742
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 747
{struct Cyc_List_List*_tmpE7=args2;struct Cyc_List_List*a=_tmpE7;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpE8=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpF=_tmpE8;struct _tuple8*_tmpE9=_stmttmpF;void**_tmpEC;struct Cyc_Absyn_Tqual _tmpEB;struct _fat_ptr*_tmpEA;_LL18: _tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEC=(void**)& _tmpE9->f3;_LL19: {struct _fat_ptr*vopt=_tmpEA;struct Cyc_Absyn_Tqual tq=_tmpEB;void**t=_tmpEC;
if(tags != 0)
({void*_tmp7CE=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp7CE;});
({void*_tmp7CF=Cyc_Parse_array2ptr(*t,1);*t=_tmp7CF;});}}}
# 759
return({struct Cyc_Absyn_Tqual _tmp7D2=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7D1=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 759
struct Cyc_List_List*_tmp7D0=new_atts;Cyc_Parse_apply_tms(_tmp7D2,_tmp7D1,_tmp7D0,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpD9=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpD8)->f2;_LL11: {unsigned loc=_tmpD9;
# 766
({void*_tmpED=0U;({unsigned _tmp7D4=loc;struct _fat_ptr _tmp7D3=({const char*_tmpEE="function declaration without parameter types";_tag_fat(_tmpEE,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7D4,_tmp7D3,_tag_fat(_tmpED,sizeof(void*),0U));});});}}_LLD:;}case 4U: _LL7: _tmpD0=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL8: {struct Cyc_List_List*ts=_tmpD0;unsigned loc=_tmpD1;
# 773
if(tms->tl == 0)
return({struct _tuple14 _tmp6D7;_tmp6D7.f1=tq,_tmp6D7.f2=t,_tmp6D7.f3=ts,_tmp6D7.f4=atts;_tmp6D7;});
# 778
({void*_tmpEF=0U;({unsigned _tmp7D6=loc;struct _fat_ptr _tmp7D5=({const char*_tmpF0="type parameters must appear before function arguments in declarator";_tag_fat(_tmpF0,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7D6,_tmp7D5,_tag_fat(_tmpEF,sizeof(void*),0U));});});}case 2U: _LL9: _tmpCE=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpCF=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpCE;struct Cyc_Absyn_Tqual tq2=_tmpCF;
# 781
return({struct Cyc_Absyn_Tqual _tmp7D9=tq2;void*_tmp7D8=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D8;_tmp6D8.elt_type=t,_tmp6D8.elt_tq=tq,_tmp6D8.ptr_atts=ptratts;_tmp6D8;}));struct Cyc_List_List*_tmp7D7=atts;Cyc_Parse_apply_tms(_tmp7D9,_tmp7D8,_tmp7D7,tms->tl);});}default: _LLB: _tmpCC=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpCD=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLC: {unsigned loc=_tmpCC;struct Cyc_List_List*atts2=_tmpCD;
# 786
return({struct Cyc_Absyn_Tqual _tmp7DC=tq;void*_tmp7DB=t;struct Cyc_List_List*_tmp7DA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp7DC,_tmp7DB,_tmp7DA,tms->tl);});}}_LL0:;}}
# 792
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 801
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpF1=t;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF6;void*_tmpF5;struct Cyc_Absyn_Tqual _tmpF4;struct _tuple0*_tmpF3;unsigned _tmpF2;_LL1: _tmpF2=_tmpF1->f1;_tmpF3=_tmpF1->f2;_tmpF4=_tmpF1->f3;_tmpF5=_tmpF1->f4;_tmpF6=_tmpF1->f5;_tmpF7=_tmpF1->f6;_LL2: {unsigned varloc=_tmpF2;struct _tuple0*x=_tmpF3;struct Cyc_Absyn_Tqual tq=_tmpF4;void*typ=_tmpF5;struct Cyc_List_List*tvs=_tmpF6;struct Cyc_List_List*atts=_tmpF7;
# 804
Cyc_Lex_register_typedef(x);{
# 806
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpF8=typ;struct Cyc_Core_Opt*_tmpF9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF8)->tag == 1U){_LL4: _tmpF9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF8)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmpF9;
# 810
type=0;
if(kopt == 0)kind=& Cyc_Tcutil_bko;else{
kind=kopt;}
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 816
return({void*_tmp7DE=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7DD=({struct Cyc_Absyn_Typedefdecl*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->name=x,_tmpFA->tvs=tvs,_tmpFA->kind=kind,_tmpFA->defn=type,_tmpFA->atts=atts,_tmpFA->tq=tq,_tmpFA->extern_c=0;_tmpFA;});_tmpFB->f1=_tmp7DD;});_tmpFB;});Cyc_Absyn_new_decl(_tmp7DE,loc);});}}}
# 823
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7DF=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=12U,_tmpFC->f1=d,_tmpFC->f2=s;_tmpFC;});Cyc_Absyn_new_stmt(_tmp7DF,d->loc);});}
# 829
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 838
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned tqual_loc,unsigned loc){
# 842
struct _RegionHandle _tmpFD=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpFD;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpFE=ds;struct Cyc_List_List*_tmp101;struct Cyc_Parse_Type_specifier _tmp100;struct Cyc_Absyn_Tqual _tmpFF;_LL1: _tmpFF=_tmpFE.tq;_tmp100=_tmpFE.type_specs;_tmp101=_tmpFE.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmpFF;struct Cyc_Parse_Type_specifier tss=_tmp100;struct Cyc_List_List*atts=_tmp101;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp102=0U;({unsigned _tmp7E1=loc;struct _fat_ptr _tmp7E0=({const char*_tmp103="inline qualifier on non-function definition";_tag_fat(_tmp103,sizeof(char),44U);});Cyc_Warn_warn(_tmp7E1,_tmp7E0,_tag_fat(_tmp102,sizeof(void*),0U));});});{
# 848
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp104=*ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmp104;enum Cyc_Parse_Storage_class _tmp105=_stmttmp10;switch(_tmp105){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 if(Cyc_Parse_no_register)s=2U;else{s=5U;}goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 goto _LL13;default: _LL12: _LL13:
 s=1U;goto _LL3;}_LL3:;}{
# 865
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 869
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 877
void*_tmp106=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp106;
if(declarators == 0){
# 881
void*_tmp107=base_type;struct Cyc_List_List*_tmp108;struct _tuple0*_tmp109;struct Cyc_List_List*_tmp10C;int _tmp10B;struct _tuple0*_tmp10A;struct Cyc_Absyn_Datatypedecl**_tmp10D;struct Cyc_List_List*_tmp110;struct _tuple0*_tmp10F;enum Cyc_Absyn_AggrKind _tmp10E;struct Cyc_Absyn_Datatypedecl*_tmp111;struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;switch(*((int*)_tmp107)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)){case 0U: _LL15: _tmp113=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp113;
# 883
({struct Cyc_List_List*_tmp7E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp7E2;});
ad->sc=s;{
struct Cyc_List_List*_tmp116=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_Decl*_tmp7E4=({void*_tmp7E3=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->tag=5U,_tmp114->f1=ad;_tmp114;});Cyc_Absyn_new_decl(_tmp7E3,loc);});_tmp115->hd=_tmp7E4;}),_tmp115->tl=0;_tmp115;});_npop_handler(0U);return _tmp116;}}case 1U: _LL17: _tmp112=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp112;
# 887
if(atts != 0)({void*_tmp117=0U;({unsigned _tmp7E6=loc;struct _fat_ptr _tmp7E5=({const char*_tmp118="attributes on enum not supported";_tag_fat(_tmp118,sizeof(char),33U);});Cyc_Warn_err(_tmp7E6,_tmp7E5,_tag_fat(_tmp117,sizeof(void*),0U));});});
ed->sc=s;{
struct Cyc_List_List*_tmp11B=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct Cyc_Absyn_Decl*_tmp7E8=({void*_tmp7E7=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=7U,_tmp119->f1=ed;_tmp119;});Cyc_Absyn_new_decl(_tmp7E7,loc);});_tmp11A->hd=_tmp7E8;}),_tmp11A->tl=0;_tmp11A;});_npop_handler(0U);return _tmp11B;}}default: _LL19: _tmp111=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp111;
# 891
if(atts != 0)({void*_tmp11C=0U;({unsigned _tmp7EA=loc;struct _fat_ptr _tmp7E9=({const char*_tmp11D="attributes on datatypes not supported";_tag_fat(_tmp11D,sizeof(char),38U);});Cyc_Warn_err(_tmp7EA,_tmp7E9,_tag_fat(_tmp11C,sizeof(void*),0U));});});
dd->sc=s;{
struct Cyc_List_List*_tmp120=({struct Cyc_List_List*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct Cyc_Absyn_Decl*_tmp7EC=({void*_tmp7EB=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->tag=6U,_tmp11E->f1=dd;_tmp11E;});Cyc_Absyn_new_decl(_tmp7EB,loc);});_tmp11F->hd=_tmp7EC;}),_tmp11F->tl=0;_tmp11F;});_npop_handler(0U);return _tmp120;}}}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp10E=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).val).f1;_tmp10F=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).val).f2;_tmp110=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp10E;struct _tuple0*n=_tmp10F;struct Cyc_List_List*ts=_tmp110;
# 895
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp121;
struct Cyc_Absyn_Aggrdecl*_tmp122=({struct Cyc_Absyn_Aggrdecl*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->kind=k,_tmp128->sc=s,_tmp128->name=n,_tmp128->tvs=ts2,_tmp128->impl=0,_tmp128->attributes=0,_tmp128->expected_mem_kind=0;_tmp128;});struct Cyc_Absyn_Aggrdecl*ad=_tmp122;
if(atts != 0)({void*_tmp123=0U;({unsigned _tmp7EE=loc;struct _fat_ptr _tmp7ED=({const char*_tmp124="bad attributes on type declaration";_tag_fat(_tmp124,sizeof(char),35U);});Cyc_Warn_err(_tmp7EE,_tmp7ED,_tag_fat(_tmp123,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp127=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));({struct Cyc_Absyn_Decl*_tmp7F0=({void*_tmp7EF=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=5U,_tmp125->f1=ad;_tmp125;});Cyc_Absyn_new_decl(_tmp7EF,loc);});_tmp126->hd=_tmp7F0;}),_tmp126->tl=0;_tmp126;});_npop_handler(0U);return _tmp127;}}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp10D=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp10D;
# 900
if(atts != 0)({void*_tmp129=0U;({unsigned _tmp7F2=loc;struct _fat_ptr _tmp7F1=({const char*_tmp12A="bad attributes on datatype";_tag_fat(_tmp12A,sizeof(char),27U);});Cyc_Warn_err(_tmp7F2,_tmp7F1,_tag_fat(_tmp129,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12D=({struct Cyc_List_List*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct Cyc_Absyn_Decl*_tmp7F4=({void*_tmp7F3=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=6U,_tmp12B->f1=*tudp;_tmp12B;});Cyc_Absyn_new_decl(_tmp7F3,loc);});_tmp12C->hd=_tmp7F4;}),_tmp12C->tl=0;_tmp12C;});_npop_handler(0U);return _tmp12D;}}}else{_LL1F: _tmp10A=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownDatatype).val).name;_tmp10B=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp10C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f2;_LL20: {struct _tuple0*n=_tmp10A;int isx=_tmp10B;struct Cyc_List_List*ts=_tmp10C;
# 903
struct Cyc_List_List*_tmp12E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp12E;
struct Cyc_Absyn_Decl*_tmp12F=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp12F;
if(atts != 0)({void*_tmp130=0U;({unsigned _tmp7F6=loc;struct _fat_ptr _tmp7F5=({const char*_tmp131="bad attributes on datatype";_tag_fat(_tmp131,sizeof(char),27U);});Cyc_Warn_err(_tmp7F6,_tmp7F5,_tag_fat(_tmp130,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=tud,_tmp132->tl=0;_tmp132;});_npop_handler(0U);return _tmp133;}}}case 15U: _LL21: _tmp109=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1;_LL22: {struct _tuple0*n=_tmp109;
# 908
struct Cyc_Absyn_Enumdecl*_tmp134=({struct Cyc_Absyn_Enumdecl*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->sc=s,_tmp13B->name=n,_tmp13B->fields=0;_tmp13B;});struct Cyc_Absyn_Enumdecl*ed=_tmp134;
if(atts != 0)({void*_tmp135=0U;({unsigned _tmp7F8=loc;struct _fat_ptr _tmp7F7=({const char*_tmp136="bad attributes on enum";_tag_fat(_tmp136,sizeof(char),23U);});Cyc_Warn_err(_tmp7F8,_tmp7F7,_tag_fat(_tmp135,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));({struct Cyc_Absyn_Decl*_tmp7FA=({struct Cyc_Absyn_Decl*_tmp138=_cycalloc(sizeof(*_tmp138));({void*_tmp7F9=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->tag=7U,_tmp137->f1=ed;_tmp137;});_tmp138->r=_tmp7F9;}),_tmp138->loc=loc;_tmp138;});_tmp139->hd=_tmp7FA;}),_tmp139->tl=0;_tmp139;});_npop_handler(0U);return _tmp13A;}}case 16U: _LL23: _tmp108=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp108;
# 914
struct Cyc_Absyn_Enumdecl*_tmp13C=({struct Cyc_Absyn_Enumdecl*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->sc=s,({struct _tuple0*_tmp7FC=Cyc_Parse_gensym_enum();_tmp144->name=_tmp7FC;}),({struct Cyc_Core_Opt*_tmp7FB=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->v=fs;_tmp143;});_tmp144->fields=_tmp7FB;});_tmp144;});struct Cyc_Absyn_Enumdecl*ed=_tmp13C;
if(atts != 0)({void*_tmp13D=0U;({unsigned _tmp7FE=loc;struct _fat_ptr _tmp7FD=({const char*_tmp13E="bad attributes on enum";_tag_fat(_tmp13E,sizeof(char),23U);});Cyc_Warn_err(_tmp7FE,_tmp7FD,_tag_fat(_tmp13D,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_Absyn_Decl*_tmp800=({struct Cyc_Absyn_Decl*_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp7FF=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->tag=7U,_tmp13F->f1=ed;_tmp13F;});_tmp140->r=_tmp7FF;}),_tmp140->loc=loc;_tmp140;});_tmp141->hd=_tmp800;}),_tmp141->tl=0;_tmp141;});_npop_handler(0U);return _tmp142;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp145=0U;({unsigned _tmp802=loc;struct _fat_ptr _tmp801=({const char*_tmp146="missing declarator";_tag_fat(_tmp146,sizeof(char),19U);});Cyc_Warn_err(_tmp802,_tmp801,_tag_fat(_tmp145,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp147=0;_npop_handler(0U);return _tmp147;}}_LL14:;}else{
# 921
struct Cyc_List_List*_tmp148=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp148;
if(istypedef){
# 926
if(!exps_empty)
({void*_tmp149=0U;({unsigned _tmp804=loc;struct _fat_ptr _tmp803=({const char*_tmp14A="initializer in typedef declaration";_tag_fat(_tmp14A,sizeof(char),35U);});Cyc_Warn_err(_tmp804,_tmp803,_tag_fat(_tmp149,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple16*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp14B=decls;_npop_handler(0U);return _tmp14B;}}else{
# 932
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp14C=fields;struct Cyc_List_List*ds=_tmp14C;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp14D=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp14D;struct _tuple16*_tmp14E=_stmttmp11;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp153;void*_tmp152;struct Cyc_Absyn_Tqual _tmp151;struct _tuple0*_tmp150;unsigned _tmp14F;_LL28: _tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;_tmp151=_tmp14E->f3;_tmp152=_tmp14E->f4;_tmp153=_tmp14E->f5;_tmp154=_tmp14E->f6;_LL29: {unsigned varloc=_tmp14F;struct _tuple0*x=_tmp150;struct Cyc_Absyn_Tqual tq2=_tmp151;void*t2=_tmp152;struct Cyc_List_List*tvs2=_tmp153;struct Cyc_List_List*atts2=_tmp154;
if(tvs2 != 0)
({void*_tmp155=0U;({unsigned _tmp806=loc;struct _fat_ptr _tmp805=({const char*_tmp156="bad type params, ignoring";_tag_fat(_tmp156,sizeof(char),26U);});Cyc_Warn_warn(_tmp806,_tmp805,_tag_fat(_tmp155,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp157=0U;({unsigned _tmp808=loc;struct _fat_ptr _tmp807=({const char*_tmp158="unexpected NULL in parse!";_tag_fat(_tmp158,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp808,_tmp807,_tag_fat(_tmp157,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp159=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;struct Cyc_Absyn_Exp*eopt=_tmp159;
struct Cyc_Absyn_Vardecl*_tmp15A=Cyc_Absyn_new_vardecl(varloc,x,t2,eopt);struct Cyc_Absyn_Vardecl*vd=_tmp15A;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;{
struct Cyc_Absyn_Decl*_tmp15B=({struct Cyc_Absyn_Decl*_tmp15E=_cycalloc(sizeof(*_tmp15E));({void*_tmp809=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->tag=0U,_tmp15D->f1=vd;_tmp15D;});_tmp15E->r=_tmp809;}),_tmp15E->loc=loc;_tmp15E;});struct Cyc_Absyn_Decl*d=_tmp15B;
decls=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->hd=d,_tmp15C->tl=decls;_tmp15C;});}}}}}{
# 947
struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp15F;}}}}}}}}}
# 843
;_pop_region(mkrgn);}
# 953
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _fat_ptr s,unsigned loc){
if(Cyc_strlen((struct _fat_ptr)s)== (unsigned long)1 || Cyc_strlen((struct _fat_ptr)s)== (unsigned long)2){
char _tmp160=*((const char*)_check_fat_subscript(s,sizeof(char),0));char _stmttmp12=_tmp160;char _tmp161=_stmttmp12;switch(_tmp161){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 963
{char _tmp162=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _stmttmp13=_tmp162;char _tmp163=_stmttmp13;switch(_tmp163){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 970
goto _LL0;case 84U: _LLF: _LL10:
# 972
{char _tmp164=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _stmttmp14=_tmp164;char _tmp165=_stmttmp14;switch(_tmp165){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 979
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 982
({struct Cyc_String_pa_PrintArg_struct _tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp6DA;_tmp6DA.tag=0U,_tmp6DA.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp6DA;});struct Cyc_Int_pa_PrintArg_struct _tmp169=({struct Cyc_Int_pa_PrintArg_struct _tmp6D9;_tmp6D9.tag=1U,({unsigned long _tmp80A=(unsigned long)((int)Cyc_strlen((struct _fat_ptr)s));_tmp6D9.f1=_tmp80A;});_tmp6D9;});void*_tmp166[2U];_tmp166[0]=& _tmp168,_tmp166[1]=& _tmp169;({unsigned _tmp80C=loc;struct _fat_ptr _tmp80B=({const char*_tmp167="bad kind: %s; strlen=%d";_tag_fat(_tmp167,sizeof(char),24U);});Cyc_Warn_err(_tmp80C,_tmp80B,_tag_fat(_tmp166,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 987
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16A=e->r;void*_stmttmp15=_tmp16A;void*_tmp16B=_stmttmp15;int _tmp16C;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).tag == 5){_LL1: _tmp16C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).val).f2;_LL2: {int i=_tmp16C;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 991
({void*_tmp16D=0U;({unsigned _tmp80E=loc;struct _fat_ptr _tmp80D=({const char*_tmp16E="expecting integer constant";_tag_fat(_tmp16E,sizeof(char),27U);});Cyc_Warn_err(_tmp80E,_tmp80D,_tag_fat(_tmp16D,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 997
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16F=e->r;void*_stmttmp16=_tmp16F;void*_tmp170=_stmttmp16;struct _fat_ptr _tmp171;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).tag == 8){_LL1: _tmp171=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp171;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1001
({void*_tmp172=0U;({unsigned _tmp810=loc;struct _fat_ptr _tmp80F=({const char*_tmp173="expecting string constant";_tag_fat(_tmp173,sizeof(char),26U);});Cyc_Warn_err(_tmp810,_tmp80F,_tag_fat(_tmp172,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 1007
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp174=x;long long _tmp175;char _tmp176;int _tmp177;switch((_tmp174.LongLong_c).tag){case 5U: _LL1: _tmp177=((_tmp174.Int_c).val).f2;_LL2: {int i=_tmp177;
return(unsigned)i;}case 2U: _LL3: _tmp176=((_tmp174.Char_c).val).f2;_LL4: {char c=_tmp176;
return(unsigned)c;}case 6U: _LL5: _tmp175=((_tmp174.LongLong_c).val).f2;_LL6: {long long x=_tmp175;
# 1012
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp178=0U;({unsigned _tmp812=loc;struct _fat_ptr _tmp811=({const char*_tmp179="integer constant too large";_tag_fat(_tmp179,sizeof(char),27U);});Cyc_Warn_err(_tmp812,_tmp811,_tag_fat(_tmp178,sizeof(void*),0U));});});
return(unsigned)x;}default: _LL7: _LL8:
# 1017
({struct Cyc_String_pa_PrintArg_struct _tmp17C=({struct Cyc_String_pa_PrintArg_struct _tmp6DB;_tmp6DB.tag=0U,({struct _fat_ptr _tmp813=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp6DB.f1=_tmp813;});_tmp6DB;});void*_tmp17A[1U];_tmp17A[0]=& _tmp17C;({unsigned _tmp815=loc;struct _fat_ptr _tmp814=({const char*_tmp17B="expected integer constant but found %s";_tag_fat(_tmp17B,sizeof(char),39U);});Cyc_Warn_err(_tmp815,_tmp814,_tag_fat(_tmp17A,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1023
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17D=p->r;void*_stmttmp17=_tmp17D;void*_tmp17E=_stmttmp17;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_List_List*_tmp181;struct _tuple0*_tmp180;int _tmp183;struct _fat_ptr _tmp182;char _tmp184;int _tmp186;enum Cyc_Absyn_Sign _tmp185;struct Cyc_Absyn_Pat*_tmp187;struct Cyc_Absyn_Vardecl*_tmp188;struct _tuple0*_tmp189;switch(*((int*)_tmp17E)){case 15U: _LL1: _tmp189=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL2: {struct _tuple0*x=_tmp189;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f2)->r)->tag == 0U){_LL3: _tmp188=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp188;
# 1027
return({struct Cyc_Absyn_Exp*_tmp816=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp816,p->loc);});}}else{goto _LL13;}case 6U: _LL5: _tmp187=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp187;
return({struct Cyc_Absyn_Exp*_tmp817=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp817,p->loc);});}case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp185=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp186=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp185;int i=_tmp186;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11U: _LLB: _tmp184=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LLC: {char c=_tmp184;
return Cyc_Absyn_char_exp(c,p->loc);}case 12U: _LLD: _tmp182=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp183=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLE: {struct _fat_ptr s=_tmp182;int i=_tmp183;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f3 == 0){_LLF: _tmp180=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp181=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LL10: {struct _tuple0*x=_tmp180;struct Cyc_List_List*ps=_tmp181;
# 1034
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp17F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp17F;
return e;}default: _LL13: _LL14:
# 1039
({void*_tmp18A=0U;({unsigned _tmp819=p->loc;struct _fat_ptr _tmp818=({const char*_tmp18B="cannot mix patterns and expressions in case";_tag_fat(_tmp18B,sizeof(char),44U);});Cyc_Warn_err(_tmp819,_tmp818,_tag_fat(_tmp18A,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple21{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple21 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple29*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple31{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1124
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->tag=Cyc_Core_Failure,_tmp18C->f1=s;_tmp18C;}));}static char _tmp18F[7U]="cnst_t";
# 1096 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18F,_tmp18F,_tmp18F + 7U};
union Cyc_YYSTYPE*_tmp18D=yy1;union Cyc_Absyn_Cnst _tmp18E;if((((union Cyc_YYSTYPE*)_tmp18D)->Int_tok).tag == 1){_LL1: _tmp18E=(_tmp18D->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp18E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1103
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.Int_tok).tag=1U,(_tmp6DC.Int_tok).val=yy1;_tmp6DC;});}static char _tmp192[5U]="char";
# 1097 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp192,_tmp192,_tmp192 + 5U};
union Cyc_YYSTYPE*_tmp190=yy1;char _tmp191;if((((union Cyc_YYSTYPE*)_tmp190)->Char_tok).tag == 2){_LL1: _tmp191=(_tmp190->Char_tok).val;_LL2: {char yy=_tmp191;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1104
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.Char_tok).tag=2U,(_tmp6DD.Char_tok).val=yy1;_tmp6DD;});}static char _tmp195[13U]="string_t<`H>";
# 1098 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp195,_tmp195,_tmp195 + 13U};
union Cyc_YYSTYPE*_tmp193=yy1;struct _fat_ptr _tmp194;if((((union Cyc_YYSTYPE*)_tmp193)->String_tok).tag == 3){_LL1: _tmp194=(_tmp193->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp194;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.String_tok).tag=3U,(_tmp6DE.String_tok).val=yy1;_tmp6DE;});}static char _tmp198[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1101 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp198,_tmp198,_tmp198 + 45U};
union Cyc_YYSTYPE*_tmp196=yy1;struct _tuple22*_tmp197;if((((union Cyc_YYSTYPE*)_tmp196)->YY1).tag == 9){_LL1: _tmp197=(_tmp196->YY1).val;_LL2: {struct _tuple22*yy=_tmp197;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1108
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY1).tag=9U,(_tmp6DF.YY1).val=yy1;_tmp6DF;});}static char _tmp19B[11U]="ptrbound_t";
# 1102 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19B,_tmp19B,_tmp19B + 11U};
union Cyc_YYSTYPE*_tmp199=yy1;void*_tmp19A;if((((union Cyc_YYSTYPE*)_tmp199)->YY2).tag == 10){_LL1: _tmp19A=(_tmp199->YY2).val;_LL2: {void*yy=_tmp19A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1109
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY2).tag=10U,(_tmp6E0.YY2).val=yy1;_tmp6E0;});}static char _tmp19E[28U]="list_t<offsetof_field_t,`H>";
# 1103 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19E,_tmp19E,_tmp19E + 28U};
union Cyc_YYSTYPE*_tmp19C=yy1;struct Cyc_List_List*_tmp19D;if((((union Cyc_YYSTYPE*)_tmp19C)->YY3).tag == 11){_LL1: _tmp19D=(_tmp19C->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp19D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1110
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY3).tag=11U,(_tmp6E1.YY3).val=yy1;_tmp6E1;});}static char _tmp1A1[6U]="exp_t";
# 1104 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A1,_tmp1A1,_tmp1A1 + 6U};
union Cyc_YYSTYPE*_tmp19F=yy1;struct Cyc_Absyn_Exp*_tmp1A0;if((((union Cyc_YYSTYPE*)_tmp19F)->Exp_tok).tag == 7){_LL1: _tmp1A0=(_tmp19F->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp1A0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1111
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.Exp_tok).tag=7U,(_tmp6E2.Exp_tok).val=yy1;_tmp6E2;});}static char _tmp1A4[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A4,_tmp1A4,_tmp1A4 + 17U};
union Cyc_YYSTYPE*_tmp1A2=yy1;struct Cyc_List_List*_tmp1A3;if((((union Cyc_YYSTYPE*)_tmp1A2)->YY4).tag == 12){_LL1: _tmp1A3=(_tmp1A2->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp1A3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1119
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY4).tag=12U,(_tmp6E3.YY4).val=yy1;_tmp6E3;});}static char _tmp1A7[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1113 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A7,_tmp1A7,_tmp1A7 + 47U};
union Cyc_YYSTYPE*_tmp1A5=yy1;struct Cyc_List_List*_tmp1A6;if((((union Cyc_YYSTYPE*)_tmp1A5)->YY5).tag == 13){_LL1: _tmp1A6=(_tmp1A5->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp1A6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1120
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY5).tag=13U,(_tmp6E4.YY5).val=yy1;_tmp6E4;});}static char _tmp1AA[9U]="primop_t";
# 1114 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AA,_tmp1AA,_tmp1AA + 9U};
union Cyc_YYSTYPE*_tmp1A8=yy1;enum Cyc_Absyn_Primop _tmp1A9;if((((union Cyc_YYSTYPE*)_tmp1A8)->YY6).tag == 14){_LL1: _tmp1A9=(_tmp1A8->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp1A9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1121
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY6).tag=14U,(_tmp6E5.YY6).val=yy1;_tmp6E5;});}static char _tmp1AD[19U]="opt_t<primop_t,`H>";
# 1115 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AD,_tmp1AD,_tmp1AD + 19U};
union Cyc_YYSTYPE*_tmp1AB=yy1;struct Cyc_Core_Opt*_tmp1AC;if((((union Cyc_YYSTYPE*)_tmp1AB)->YY7).tag == 15){_LL1: _tmp1AC=(_tmp1AB->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp1AC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY7).tag=15U,(_tmp6E6.YY7).val=yy1;_tmp6E6;});}static char _tmp1B0[7U]="qvar_t";
# 1116 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B0,_tmp1B0,_tmp1B0 + 7U};
union Cyc_YYSTYPE*_tmp1AE=yy1;struct _tuple0*_tmp1AF;if((((union Cyc_YYSTYPE*)_tmp1AE)->QualId_tok).tag == 5){_LL1: _tmp1AF=(_tmp1AE->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp1AF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1123
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.QualId_tok).tag=5U,(_tmp6E7.QualId_tok).val=yy1;_tmp6E7;});}static char _tmp1B3[7U]="stmt_t";
# 1119 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B3,_tmp1B3,_tmp1B3 + 7U};
union Cyc_YYSTYPE*_tmp1B1=yy1;struct Cyc_Absyn_Stmt*_tmp1B2;if((((union Cyc_YYSTYPE*)_tmp1B1)->Stmt_tok).tag == 8){_LL1: _tmp1B2=(_tmp1B1->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp1B2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1126
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.Stmt_tok).tag=8U,(_tmp6E8.Stmt_tok).val=yy1;_tmp6E8;});}static char _tmp1B6[27U]="list_t<switch_clause_t,`H>";
# 1123 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B6,_tmp1B6,_tmp1B6 + 27U};
union Cyc_YYSTYPE*_tmp1B4=yy1;struct Cyc_List_List*_tmp1B5;if((((union Cyc_YYSTYPE*)_tmp1B4)->YY8).tag == 16){_LL1: _tmp1B5=(_tmp1B4->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp1B5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1130
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY8).tag=16U,(_tmp6E9.YY8).val=yy1;_tmp6E9;});}static char _tmp1B9[6U]="pat_t";
# 1124 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B9,_tmp1B9,_tmp1B9 + 6U};
union Cyc_YYSTYPE*_tmp1B7=yy1;struct Cyc_Absyn_Pat*_tmp1B8;if((((union Cyc_YYSTYPE*)_tmp1B7)->YY9).tag == 17){_LL1: _tmp1B8=(_tmp1B7->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1B8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY9).tag=17U,(_tmp6EA.YY9).val=yy1;_tmp6EA;});}static char _tmp1BC[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1129 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BC,_tmp1BC,_tmp1BC + 28U};
union Cyc_YYSTYPE*_tmp1BA=yy1;struct _tuple23*_tmp1BB;if((((union Cyc_YYSTYPE*)_tmp1BA)->YY10).tag == 18){_LL1: _tmp1BB=(_tmp1BA->YY10).val;_LL2: {struct _tuple23*yy=_tmp1BB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1136
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY10).tag=18U,(_tmp6EB.YY10).val=yy1;_tmp6EB;});}static char _tmp1BF[17U]="list_t<pat_t,`H>";
# 1130 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BF,_tmp1BF,_tmp1BF + 17U};
union Cyc_YYSTYPE*_tmp1BD=yy1;struct Cyc_List_List*_tmp1BE;if((((union Cyc_YYSTYPE*)_tmp1BD)->YY11).tag == 19){_LL1: _tmp1BE=(_tmp1BD->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1BE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY11).tag=19U,(_tmp6EC.YY11).val=yy1;_tmp6EC;});}static char _tmp1C2[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1131 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C2,_tmp1C2,_tmp1C2 + 36U};
union Cyc_YYSTYPE*_tmp1C0=yy1;struct _tuple24*_tmp1C1;if((((union Cyc_YYSTYPE*)_tmp1C0)->YY12).tag == 20){_LL1: _tmp1C1=(_tmp1C0->YY12).val;_LL2: {struct _tuple24*yy=_tmp1C1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1138
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY12).tag=20U,(_tmp6ED.YY12).val=yy1;_tmp6ED;});}static char _tmp1C5[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1132 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C5,_tmp1C5,_tmp1C5 + 47U};
union Cyc_YYSTYPE*_tmp1C3=yy1;struct Cyc_List_List*_tmp1C4;if((((union Cyc_YYSTYPE*)_tmp1C3)->YY13).tag == 21){_LL1: _tmp1C4=(_tmp1C3->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1C4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1139
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY13).tag=21U,(_tmp6EE.YY13).val=yy1;_tmp6EE;});}static char _tmp1C8[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1133 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C8,_tmp1C8,_tmp1C8 + 58U};
union Cyc_YYSTYPE*_tmp1C6=yy1;struct _tuple23*_tmp1C7;if((((union Cyc_YYSTYPE*)_tmp1C6)->YY14).tag == 22){_LL1: _tmp1C7=(_tmp1C6->YY14).val;_LL2: {struct _tuple23*yy=_tmp1C7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1140
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY14).tag=22U,(_tmp6EF.YY14).val=yy1;_tmp6EF;});}static char _tmp1CB[9U]="fndecl_t";
# 1134 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CB,_tmp1CB,_tmp1CB + 9U};
union Cyc_YYSTYPE*_tmp1C9=yy1;struct Cyc_Absyn_Fndecl*_tmp1CA;if((((union Cyc_YYSTYPE*)_tmp1C9)->YY15).tag == 23){_LL1: _tmp1CA=(_tmp1C9->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1CA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY15).tag=23U,(_tmp6F0.YY15).val=yy1;_tmp6F0;});}static char _tmp1CE[18U]="list_t<decl_t,`H>";
# 1135 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CE,_tmp1CE,_tmp1CE + 18U};
union Cyc_YYSTYPE*_tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;if((((union Cyc_YYSTYPE*)_tmp1CC)->YY16).tag == 24){_LL1: _tmp1CD=(_tmp1CC->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1CD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1142
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY16).tag=24U,(_tmp6F1.YY16).val=yy1;_tmp6F1;});}static char _tmp1D1[12U]="decl_spec_t";
# 1138 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D1,_tmp1D1,_tmp1D1 + 12U};
union Cyc_YYSTYPE*_tmp1CF=yy1;struct Cyc_Parse_Declaration_spec _tmp1D0;if((((union Cyc_YYSTYPE*)_tmp1CF)->YY17).tag == 25){_LL1: _tmp1D0=(_tmp1CF->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1D0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1145
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY17).tag=25U,(_tmp6F2.YY17).val=yy1;_tmp6F2;});}static char _tmp1D4[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1139 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D4,_tmp1D4,_tmp1D4 + 31U};
union Cyc_YYSTYPE*_tmp1D2=yy1;struct _tuple12 _tmp1D3;if((((union Cyc_YYSTYPE*)_tmp1D2)->YY18).tag == 26){_LL1: _tmp1D3=(_tmp1D2->YY18).val;_LL2: {struct _tuple12 yy=_tmp1D3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1146
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY18).tag=26U,(_tmp6F3.YY18).val=yy1;_tmp6F3;});}static char _tmp1D7[23U]="declarator_list_t<`yy>";
# 1140 "parse.y"
static struct _tuple13*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D7,_tmp1D7,_tmp1D7 + 23U};
union Cyc_YYSTYPE*_tmp1D5=yy1;struct _tuple13*_tmp1D6;if((((union Cyc_YYSTYPE*)_tmp1D5)->YY19).tag == 27){_LL1: _tmp1D6=(_tmp1D5->YY19).val;_LL2: {struct _tuple13*yy=_tmp1D6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1147
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple13*yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY19).tag=27U,(_tmp6F4.YY19).val=yy1;_tmp6F4;});}static char _tmp1DA[19U]="storage_class_t@`H";
# 1141 "parse.y"
static enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DA,_tmp1DA,_tmp1DA + 19U};
union Cyc_YYSTYPE*_tmp1D8=yy1;enum Cyc_Parse_Storage_class*_tmp1D9;if((((union Cyc_YYSTYPE*)_tmp1D8)->YY20).tag == 28){_LL1: _tmp1D9=(_tmp1D8->YY20).val;_LL2: {enum Cyc_Parse_Storage_class*yy=_tmp1D9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1148
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY20).tag=28U,(_tmp6F5.YY20).val=yy1;_tmp6F5;});}static char _tmp1DD[17U]="type_specifier_t";
# 1142 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DD,_tmp1DD,_tmp1DD + 17U};
union Cyc_YYSTYPE*_tmp1DB=yy1;struct Cyc_Parse_Type_specifier _tmp1DC;if((((union Cyc_YYSTYPE*)_tmp1DB)->YY21).tag == 29){_LL1: _tmp1DC=(_tmp1DB->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1DC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1149
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY21).tag=29U,(_tmp6F6.YY21).val=yy1;_tmp6F6;});}static char _tmp1E0[12U]="aggr_kind_t";
# 1144 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E0,_tmp1E0,_tmp1E0 + 12U};
union Cyc_YYSTYPE*_tmp1DE=yy1;enum Cyc_Absyn_AggrKind _tmp1DF;if((((union Cyc_YYSTYPE*)_tmp1DE)->YY22).tag == 30){_LL1: _tmp1DF=(_tmp1DE->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1DF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1151
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY22).tag=30U,(_tmp6F7.YY22).val=yy1;_tmp6F7;});}static char _tmp1E3[8U]="tqual_t";
# 1145 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E3,_tmp1E3,_tmp1E3 + 8U};
union Cyc_YYSTYPE*_tmp1E1=yy1;struct Cyc_Absyn_Tqual _tmp1E2;if((((union Cyc_YYSTYPE*)_tmp1E1)->YY23).tag == 31){_LL1: _tmp1E2=(_tmp1E1->YY23).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1E2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1152
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY23).tag=31U,(_tmp6F8.YY23).val=yy1;_tmp6F8;});}static char _tmp1E6[23U]="list_t<aggrfield_t,`H>";
# 1146 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E6,_tmp1E6,_tmp1E6 + 23U};
union Cyc_YYSTYPE*_tmp1E4=yy1;struct Cyc_List_List*_tmp1E5;if((((union Cyc_YYSTYPE*)_tmp1E4)->YY24).tag == 32){_LL1: _tmp1E5=(_tmp1E4->YY24).val;_LL2: {struct Cyc_List_List*yy=_tmp1E5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1153
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY24).tag=32U,(_tmp6F9.YY24).val=yy1;_tmp6F9;});}static char _tmp1E9[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1147 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E9,_tmp1E9,_tmp1E9 + 34U};
union Cyc_YYSTYPE*_tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;if((((union Cyc_YYSTYPE*)_tmp1E7)->YY25).tag == 33){_LL1: _tmp1E8=(_tmp1E7->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1E8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1154
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY25).tag=33U,(_tmp6FA.YY25).val=yy1;_tmp6FA;});}static char _tmp1EC[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1148 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EC,_tmp1EC,_tmp1EC + 33U};
union Cyc_YYSTYPE*_tmp1EA=yy1;struct Cyc_List_List*_tmp1EB;if((((union Cyc_YYSTYPE*)_tmp1EA)->YY26).tag == 34){_LL1: _tmp1EB=(_tmp1EA->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1EB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1155
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY26).tag=34U,(_tmp6FB.YY26).val=yy1;_tmp6FB;});}static char _tmp1EF[18U]="declarator_t<`yy>";
# 1149 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EF,_tmp1EF,_tmp1EF + 18U};
union Cyc_YYSTYPE*_tmp1ED=yy1;struct Cyc_Parse_Declarator _tmp1EE;if((((union Cyc_YYSTYPE*)_tmp1ED)->YY27).tag == 35){_LL1: _tmp1EE=(_tmp1ED->YY27).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1EE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1156
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY27).tag=35U,(_tmp6FC.YY27).val=yy1;_tmp6FC;});}static char _tmp1F2[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1150 "parse.y"
static struct _tuple25*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F2,_tmp1F2,_tmp1F2 + 45U};
union Cyc_YYSTYPE*_tmp1F0=yy1;struct _tuple25*_tmp1F1;if((((union Cyc_YYSTYPE*)_tmp1F0)->YY28).tag == 36){_LL1: _tmp1F1=(_tmp1F0->YY28).val;_LL2: {struct _tuple25*yy=_tmp1F1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1157
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple25*yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY28).tag=36U,(_tmp6FD.YY28).val=yy1;_tmp6FD;});}static char _tmp1F5[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1151 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F5,_tmp1F5,_tmp1F5 + 57U};
union Cyc_YYSTYPE*_tmp1F3=yy1;struct Cyc_List_List*_tmp1F4;if((((union Cyc_YYSTYPE*)_tmp1F3)->YY29).tag == 37){_LL1: _tmp1F4=(_tmp1F3->YY29).val;_LL2: {struct Cyc_List_List*yy=_tmp1F4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1158
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY29).tag=37U,(_tmp6FE.YY29).val=yy1;_tmp6FE;});}static char _tmp1F8[26U]="abstractdeclarator_t<`yy>";
# 1152 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F8,_tmp1F8,_tmp1F8 + 26U};
union Cyc_YYSTYPE*_tmp1F6=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F7;if((((union Cyc_YYSTYPE*)_tmp1F6)->YY30).tag == 38){_LL1: _tmp1F7=(_tmp1F6->YY30).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp1F7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1159
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY30).tag=38U,(_tmp6FF.YY30).val=yy1;_tmp6FF;});}static char _tmp1FB[5U]="bool";
# 1153 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FB,_tmp1FB,_tmp1FB + 5U};
union Cyc_YYSTYPE*_tmp1F9=yy1;int _tmp1FA;if((((union Cyc_YYSTYPE*)_tmp1F9)->YY31).tag == 39){_LL1: _tmp1FA=(_tmp1F9->YY31).val;_LL2: {int yy=_tmp1FA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1160
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY31).tag=39U,(_tmp700.YY31).val=yy1;_tmp700;});}static char _tmp1FE[8U]="scope_t";
# 1154 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FE,_tmp1FE,_tmp1FE + 8U};
union Cyc_YYSTYPE*_tmp1FC=yy1;enum Cyc_Absyn_Scope _tmp1FD;if((((union Cyc_YYSTYPE*)_tmp1FC)->YY32).tag == 40){_LL1: _tmp1FD=(_tmp1FC->YY32).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp1FD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1161
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY32).tag=40U,(_tmp701.YY32).val=yy1;_tmp701;});}static char _tmp201[16U]="datatypefield_t";
# 1155 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp201,_tmp201,_tmp201 + 16U};
union Cyc_YYSTYPE*_tmp1FF=yy1;struct Cyc_Absyn_Datatypefield*_tmp200;if((((union Cyc_YYSTYPE*)_tmp1FF)->YY33).tag == 41){_LL1: _tmp200=(_tmp1FF->YY33).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp200;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1162
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY33).tag=41U,(_tmp702.YY33).val=yy1;_tmp702;});}static char _tmp204[27U]="list_t<datatypefield_t,`H>";
# 1156 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp204,_tmp204,_tmp204 + 27U};
union Cyc_YYSTYPE*_tmp202=yy1;struct Cyc_List_List*_tmp203;if((((union Cyc_YYSTYPE*)_tmp202)->YY34).tag == 42){_LL1: _tmp203=(_tmp202->YY34).val;_LL2: {struct Cyc_List_List*yy=_tmp203;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1163
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY34).tag=42U,(_tmp703.YY34).val=yy1;_tmp703;});}static char _tmp207[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1157 "parse.y"
static struct _tuple26 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp207,_tmp207,_tmp207 + 41U};
union Cyc_YYSTYPE*_tmp205=yy1;struct _tuple26 _tmp206;if((((union Cyc_YYSTYPE*)_tmp205)->YY35).tag == 43){_LL1: _tmp206=(_tmp205->YY35).val;_LL2: {struct _tuple26 yy=_tmp206;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1164
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple26 yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY35).tag=43U,(_tmp704.YY35).val=yy1;_tmp704;});}static char _tmp20A[17U]="list_t<var_t,`H>";
# 1158 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20A,_tmp20A,_tmp20A + 17U};
union Cyc_YYSTYPE*_tmp208=yy1;struct Cyc_List_List*_tmp209;if((((union Cyc_YYSTYPE*)_tmp208)->YY36).tag == 44){_LL1: _tmp209=(_tmp208->YY36).val;_LL2: {struct Cyc_List_List*yy=_tmp209;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1165
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY36).tag=44U,(_tmp705.YY36).val=yy1;_tmp705;});}static char _tmp20D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1159 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20D,_tmp20D,_tmp20D + 31U};
union Cyc_YYSTYPE*_tmp20B=yy1;struct _tuple8*_tmp20C;if((((union Cyc_YYSTYPE*)_tmp20B)->YY37).tag == 45){_LL1: _tmp20C=(_tmp20B->YY37).val;_LL2: {struct _tuple8*yy=_tmp20C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1166
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY37).tag=45U,(_tmp706.YY37).val=yy1;_tmp706;});}static char _tmp210[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1160 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp210,_tmp210,_tmp210 + 42U};
union Cyc_YYSTYPE*_tmp20E=yy1;struct Cyc_List_List*_tmp20F;if((((union Cyc_YYSTYPE*)_tmp20E)->YY38).tag == 46){_LL1: _tmp20F=(_tmp20E->YY38).val;_LL2: {struct Cyc_List_List*yy=_tmp20F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1167
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY38).tag=46U,(_tmp707.YY38).val=yy1;_tmp707;});}static char _tmp213[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1161 "parse.y"
static struct _tuple27*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp213,_tmp213,_tmp213 + 115U};
union Cyc_YYSTYPE*_tmp211=yy1;struct _tuple27*_tmp212;if((((union Cyc_YYSTYPE*)_tmp211)->YY39).tag == 47){_LL1: _tmp212=(_tmp211->YY39).val;_LL2: {struct _tuple27*yy=_tmp212;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1168
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple27*yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY39).tag=47U,(_tmp708.YY39).val=yy1;_tmp708;});}static char _tmp216[8U]="types_t";
# 1162 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp216,_tmp216,_tmp216 + 8U};
union Cyc_YYSTYPE*_tmp214=yy1;struct Cyc_List_List*_tmp215;if((((union Cyc_YYSTYPE*)_tmp214)->YY40).tag == 48){_LL1: _tmp215=(_tmp214->YY40).val;_LL2: {struct Cyc_List_List*yy=_tmp215;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1169
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY40).tag=48U,(_tmp709.YY40).val=yy1;_tmp709;});}static char _tmp219[24U]="list_t<designator_t,`H>";
# 1164 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp219,_tmp219,_tmp219 + 24U};
union Cyc_YYSTYPE*_tmp217=yy1;struct Cyc_List_List*_tmp218;if((((union Cyc_YYSTYPE*)_tmp217)->YY41).tag == 49){_LL1: _tmp218=(_tmp217->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp218;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1171
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY41).tag=49U,(_tmp70A.YY41).val=yy1;_tmp70A;});}static char _tmp21C[13U]="designator_t";
# 1165 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21C,_tmp21C,_tmp21C + 13U};
union Cyc_YYSTYPE*_tmp21A=yy1;void*_tmp21B;if((((union Cyc_YYSTYPE*)_tmp21A)->YY42).tag == 50){_LL1: _tmp21B=(_tmp21A->YY42).val;_LL2: {void*yy=_tmp21B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1172
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY42).tag=50U,(_tmp70B.YY42).val=yy1;_tmp70B;});}static char _tmp21F[7U]="kind_t";
# 1166 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21F,_tmp21F,_tmp21F + 7U};
union Cyc_YYSTYPE*_tmp21D=yy1;struct Cyc_Absyn_Kind*_tmp21E;if((((union Cyc_YYSTYPE*)_tmp21D)->YY43).tag == 51){_LL1: _tmp21E=(_tmp21D->YY43).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp21E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1173
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY43).tag=51U,(_tmp70C.YY43).val=yy1;_tmp70C;});}static char _tmp222[7U]="type_t";
# 1167 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp222,_tmp222,_tmp222 + 7U};
union Cyc_YYSTYPE*_tmp220=yy1;void*_tmp221;if((((union Cyc_YYSTYPE*)_tmp220)->YY44).tag == 52){_LL1: _tmp221=(_tmp220->YY44).val;_LL2: {void*yy=_tmp221;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1174
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY44).tag=52U,(_tmp70D.YY44).val=yy1;_tmp70D;});}static char _tmp225[23U]="list_t<attribute_t,`H>";
# 1168 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp225,_tmp225,_tmp225 + 23U};
union Cyc_YYSTYPE*_tmp223=yy1;struct Cyc_List_List*_tmp224;if((((union Cyc_YYSTYPE*)_tmp223)->YY45).tag == 53){_LL1: _tmp224=(_tmp223->YY45).val;_LL2: {struct Cyc_List_List*yy=_tmp224;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1175
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY45).tag=53U,(_tmp70E.YY45).val=yy1;_tmp70E;});}static char _tmp228[12U]="attribute_t";
# 1169 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp228,_tmp228,_tmp228 + 12U};
union Cyc_YYSTYPE*_tmp226=yy1;void*_tmp227;if((((union Cyc_YYSTYPE*)_tmp226)->YY46).tag == 54){_LL1: _tmp227=(_tmp226->YY46).val;_LL2: {void*yy=_tmp227;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1176
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY46).tag=54U,(_tmp70F.YY46).val=yy1;_tmp70F;});}static char _tmp22B[12U]="enumfield_t";
# 1170 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22B,_tmp22B,_tmp22B + 12U};
union Cyc_YYSTYPE*_tmp229=yy1;struct Cyc_Absyn_Enumfield*_tmp22A;if((((union Cyc_YYSTYPE*)_tmp229)->YY47).tag == 55){_LL1: _tmp22A=(_tmp229->YY47).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp22A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1177
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY47).tag=55U,(_tmp710.YY47).val=yy1;_tmp710;});}static char _tmp22E[23U]="list_t<enumfield_t,`H>";
# 1171 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22E,_tmp22E,_tmp22E + 23U};
union Cyc_YYSTYPE*_tmp22C=yy1;struct Cyc_List_List*_tmp22D;if((((union Cyc_YYSTYPE*)_tmp22C)->YY48).tag == 56){_LL1: _tmp22D=(_tmp22C->YY48).val;_LL2: {struct Cyc_List_List*yy=_tmp22D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1178
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY48).tag=56U,(_tmp711.YY48).val=yy1;_tmp711;});}static char _tmp231[11U]="type_opt_t";
# 1172 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp231,_tmp231,_tmp231 + 11U};
union Cyc_YYSTYPE*_tmp22F=yy1;void*_tmp230;if((((union Cyc_YYSTYPE*)_tmp22F)->YY49).tag == 57){_LL1: _tmp230=(_tmp22F->YY49).val;_LL2: {void*yy=_tmp230;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1179
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY49).tag=57U,(_tmp712.YY49).val=yy1;_tmp712;});}static char _tmp234[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1173 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp234,_tmp234,_tmp234 + 31U};
union Cyc_YYSTYPE*_tmp232=yy1;struct Cyc_List_List*_tmp233;if((((union Cyc_YYSTYPE*)_tmp232)->YY50).tag == 58){_LL1: _tmp233=(_tmp232->YY50).val;_LL2: {struct Cyc_List_List*yy=_tmp233;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1180
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY50).tag=58U,(_tmp713.YY50).val=yy1;_tmp713;});}static char _tmp237[11U]="booltype_t";
# 1174 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp237,_tmp237,_tmp237 + 11U};
union Cyc_YYSTYPE*_tmp235=yy1;void*_tmp236;if((((union Cyc_YYSTYPE*)_tmp235)->YY51).tag == 59){_LL1: _tmp236=(_tmp235->YY51).val;_LL2: {void*yy=_tmp236;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1181
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY51).tag=59U,(_tmp714.YY51).val=yy1;_tmp714;});}static char _tmp23A[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1175 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23A,_tmp23A,_tmp23A + 45U};
union Cyc_YYSTYPE*_tmp238=yy1;struct Cyc_List_List*_tmp239;if((((union Cyc_YYSTYPE*)_tmp238)->YY52).tag == 60){_LL1: _tmp239=(_tmp238->YY52).val;_LL2: {struct Cyc_List_List*yy=_tmp239;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1182
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY52).tag=60U,(_tmp715.YY52).val=yy1;_tmp715;});}static char _tmp23D[58U]="$(list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1176 "parse.y"
static struct _tuple28*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23D,_tmp23D,_tmp23D + 58U};
union Cyc_YYSTYPE*_tmp23B=yy1;struct _tuple28*_tmp23C;if((((union Cyc_YYSTYPE*)_tmp23B)->YY53).tag == 61){_LL1: _tmp23C=(_tmp23B->YY53).val;_LL2: {struct _tuple28*yy=_tmp23C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1183
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp716;(_tmp716.YY53).tag=61U,(_tmp716.YY53).val=yy1;_tmp716;});}static char _tmp240[18U]="list_t<qvar_t,`H>";
# 1177 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp240,_tmp240,_tmp240 + 18U};
union Cyc_YYSTYPE*_tmp23E=yy1;struct Cyc_List_List*_tmp23F;if((((union Cyc_YYSTYPE*)_tmp23E)->YY54).tag == 62){_LL1: _tmp23F=(_tmp23E->YY54).val;_LL2: {struct Cyc_List_List*yy=_tmp23F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1184
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp717;(_tmp717.YY54).tag=62U,(_tmp717.YY54).val=yy1;_tmp717;});}static char _tmp243[20U]="pointer_qual_t<`yy>";
# 1178 "parse.y"
static void*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp243,_tmp243,_tmp243 + 20U};
union Cyc_YYSTYPE*_tmp241=yy1;void*_tmp242;if((((union Cyc_YYSTYPE*)_tmp241)->YY55).tag == 63){_LL1: _tmp242=(_tmp241->YY55).val;_LL2: {void*yy=_tmp242;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1185
static union Cyc_YYSTYPE Cyc_YY55(void*yy1){return({union Cyc_YYSTYPE _tmp718;(_tmp718.YY55).tag=63U,(_tmp718.YY55).val=yy1;_tmp718;});}static char _tmp246[21U]="pointer_quals_t<`yy>";
# 1179 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp246,_tmp246,_tmp246 + 21U};
union Cyc_YYSTYPE*_tmp244=yy1;struct Cyc_List_List*_tmp245;if((((union Cyc_YYSTYPE*)_tmp244)->YY56).tag == 64){_LL1: _tmp245=(_tmp244->YY56).val;_LL2: {struct Cyc_List_List*yy=_tmp245;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1186
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp719;(_tmp719.YY56).tag=64U,(_tmp719.YY56).val=yy1;_tmp719;});}static char _tmp249[10U]="exp_opt_t";
# 1180 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp249,_tmp249,_tmp249 + 10U};
union Cyc_YYSTYPE*_tmp247=yy1;struct Cyc_Absyn_Exp*_tmp248;if((((union Cyc_YYSTYPE*)_tmp247)->YY57).tag == 65){_LL1: _tmp248=(_tmp247->YY57).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp248;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1187
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp71A;(_tmp71A.YY57).tag=65U,(_tmp71A.YY57).val=yy1;_tmp71A;});}static char _tmp24C[10U]="raw_exp_t";
# 1181 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24C,_tmp24C,_tmp24C + 10U};
union Cyc_YYSTYPE*_tmp24A=yy1;void*_tmp24B;if((((union Cyc_YYSTYPE*)_tmp24A)->YY58).tag == 66){_LL1: _tmp24B=(_tmp24A->YY58).val;_LL2: {void*yy=_tmp24B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1188
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _tmp71B;(_tmp71B.YY58).tag=66U,(_tmp71B.YY58).val=yy1;_tmp71B;});}static char _tmp24F[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1183 "parse.y"
static struct _tuple29*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24F,_tmp24F,_tmp24F + 112U};
union Cyc_YYSTYPE*_tmp24D=yy1;struct _tuple29*_tmp24E;if((((union Cyc_YYSTYPE*)_tmp24D)->YY59).tag == 67){_LL1: _tmp24E=(_tmp24D->YY59).val;_LL2: {struct _tuple29*yy=_tmp24E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1190
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp71C;(_tmp71C.YY59).tag=67U,(_tmp71C.YY59).val=yy1;_tmp71C;});}static char _tmp252[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1184 "parse.y"
static struct _tuple30*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp252,_tmp252,_tmp252 + 73U};
union Cyc_YYSTYPE*_tmp250=yy1;struct _tuple30*_tmp251;if((((union Cyc_YYSTYPE*)_tmp250)->YY60).tag == 68){_LL1: _tmp251=(_tmp250->YY60).val;_LL2: {struct _tuple30*yy=_tmp251;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1191
static union Cyc_YYSTYPE Cyc_YY60(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp71D;(_tmp71D.YY60).tag=68U,(_tmp71D.YY60).val=yy1;_tmp71D;});}static char _tmp255[28U]="list_t<string_t<`H>@`H, `H>";
# 1185 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp255,_tmp255,_tmp255 + 28U};
union Cyc_YYSTYPE*_tmp253=yy1;struct Cyc_List_List*_tmp254;if((((union Cyc_YYSTYPE*)_tmp253)->YY61).tag == 69){_LL1: _tmp254=(_tmp253->YY61).val;_LL2: {struct Cyc_List_List*yy=_tmp254;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1192
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71E;(_tmp71E.YY61).tag=69U,(_tmp71E.YY61).val=yy1;_tmp71E;});}static char _tmp258[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1186 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp258,_tmp258,_tmp258 + 38U};
union Cyc_YYSTYPE*_tmp256=yy1;struct Cyc_List_List*_tmp257;if((((union Cyc_YYSTYPE*)_tmp256)->YY62).tag == 70){_LL1: _tmp257=(_tmp256->YY62).val;_LL2: {struct Cyc_List_List*yy=_tmp257;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1193
static union Cyc_YYSTYPE Cyc_YY62(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71F;(_tmp71F.YY62).tag=70U,(_tmp71F.YY62).val=yy1;_tmp71F;});}static char _tmp25B[26U]="$(string_t<`H>, exp_t)@`H";
# 1187 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp25B,_tmp25B,_tmp25B + 26U};
union Cyc_YYSTYPE*_tmp259=yy1;struct _tuple31*_tmp25A;if((((union Cyc_YYSTYPE*)_tmp259)->YY63).tag == 71){_LL1: _tmp25A=(_tmp259->YY63).val;_LL2: {struct _tuple31*yy=_tmp25A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1194
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp720;(_tmp720.YY63).tag=71U,(_tmp720.YY63).val=yy1;_tmp720;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1210
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp721;_tmp721.timestamp=0,_tmp721.first_line=0,_tmp721.first_column=0,_tmp721.last_line=0,_tmp721.last_column=0;_tmp721;});}
# 1213
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1224 "parse.y"
static short Cyc_yytranslate[379U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,150,2,2,135,148,145,2,132,133,128,142,127,146,137,147,2,2,2,2,2,2,2,2,2,2,136,124,130,129,131,141,140,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,138,2,139,144,134,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,125,143,126,149,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123};static char _tmp25C[2U]="$";static char _tmp25D[6U]="error";static char _tmp25E[12U]="$undefined.";static char _tmp25F[5U]="AUTO";static char _tmp260[9U]="REGISTER";static char _tmp261[7U]="STATIC";static char _tmp262[7U]="EXTERN";static char _tmp263[8U]="TYPEDEF";static char _tmp264[5U]="VOID";static char _tmp265[5U]="CHAR";static char _tmp266[6U]="SHORT";static char _tmp267[4U]="INT";static char _tmp268[5U]="LONG";static char _tmp269[6U]="FLOAT";static char _tmp26A[7U]="DOUBLE";static char _tmp26B[7U]="SIGNED";static char _tmp26C[9U]="UNSIGNED";static char _tmp26D[6U]="CONST";static char _tmp26E[9U]="VOLATILE";static char _tmp26F[9U]="RESTRICT";static char _tmp270[7U]="STRUCT";static char _tmp271[6U]="UNION";static char _tmp272[5U]="CASE";static char _tmp273[8U]="DEFAULT";static char _tmp274[7U]="INLINE";static char _tmp275[7U]="SIZEOF";static char _tmp276[9U]="OFFSETOF";static char _tmp277[3U]="IF";static char _tmp278[5U]="ELSE";static char _tmp279[7U]="SWITCH";static char _tmp27A[6U]="WHILE";static char _tmp27B[3U]="DO";static char _tmp27C[4U]="FOR";static char _tmp27D[5U]="GOTO";static char _tmp27E[9U]="CONTINUE";static char _tmp27F[6U]="BREAK";static char _tmp280[7U]="RETURN";static char _tmp281[5U]="ENUM";static char _tmp282[7U]="TYPEOF";static char _tmp283[16U]="BUILTIN_VA_LIST";static char _tmp284[10U]="EXTENSION";static char _tmp285[8U]="NULL_kw";static char _tmp286[4U]="LET";static char _tmp287[6U]="THROW";static char _tmp288[4U]="TRY";static char _tmp289[6U]="CATCH";static char _tmp28A[7U]="EXPORT";static char _tmp28B[9U]="OVERRIDE";static char _tmp28C[5U]="HIDE";static char _tmp28D[4U]="NEW";static char _tmp28E[9U]="ABSTRACT";static char _tmp28F[9U]="FALLTHRU";static char _tmp290[6U]="USING";static char _tmp291[10U]="NAMESPACE";static char _tmp292[9U]="DATATYPE";static char _tmp293[7U]="MALLOC";static char _tmp294[8U]="RMALLOC";static char _tmp295[15U]="RMALLOC_INLINE";static char _tmp296[7U]="CALLOC";static char _tmp297[8U]="RCALLOC";static char _tmp298[5U]="SWAP";static char _tmp299[9U]="REGION_T";static char _tmp29A[6U]="TAG_T";static char _tmp29B[7U]="REGION";static char _tmp29C[5U]="RNEW";static char _tmp29D[8U]="REGIONS";static char _tmp29E[7U]="PORTON";static char _tmp29F[8U]="PORTOFF";static char _tmp2A0[7U]="PRAGMA";static char _tmp2A1[10U]="TEMPESTON";static char _tmp2A2[11U]="TEMPESTOFF";static char _tmp2A3[8U]="NUMELTS";static char _tmp2A4[8U]="VALUEOF";static char _tmp2A5[10U]="VALUEOF_T";static char _tmp2A6[9U]="TAGCHECK";static char _tmp2A7[13U]="NUMELTS_QUAL";static char _tmp2A8[10U]="THIN_QUAL";static char _tmp2A9[9U]="FAT_QUAL";static char _tmp2AA[13U]="NOTNULL_QUAL";static char _tmp2AB[14U]="NULLABLE_QUAL";static char _tmp2AC[14U]="REQUIRES_QUAL";static char _tmp2AD[13U]="ENSURES_QUAL";static char _tmp2AE[12U]="REGION_QUAL";static char _tmp2AF[16U]="NOZEROTERM_QUAL";static char _tmp2B0[14U]="ZEROTERM_QUAL";static char _tmp2B1[12U]="TAGGED_QUAL";static char _tmp2B2[16U]="EXTENSIBLE_QUAL";static char _tmp2B3[7U]="PTR_OP";static char _tmp2B4[7U]="INC_OP";static char _tmp2B5[7U]="DEC_OP";static char _tmp2B6[8U]="LEFT_OP";static char _tmp2B7[9U]="RIGHT_OP";static char _tmp2B8[6U]="LE_OP";static char _tmp2B9[6U]="GE_OP";static char _tmp2BA[6U]="EQ_OP";static char _tmp2BB[6U]="NE_OP";static char _tmp2BC[7U]="AND_OP";static char _tmp2BD[6U]="OR_OP";static char _tmp2BE[11U]="MUL_ASSIGN";static char _tmp2BF[11U]="DIV_ASSIGN";static char _tmp2C0[11U]="MOD_ASSIGN";static char _tmp2C1[11U]="ADD_ASSIGN";static char _tmp2C2[11U]="SUB_ASSIGN";static char _tmp2C3[12U]="LEFT_ASSIGN";static char _tmp2C4[13U]="RIGHT_ASSIGN";static char _tmp2C5[11U]="AND_ASSIGN";static char _tmp2C6[11U]="XOR_ASSIGN";static char _tmp2C7[10U]="OR_ASSIGN";static char _tmp2C8[9U]="ELLIPSIS";static char _tmp2C9[11U]="LEFT_RIGHT";static char _tmp2CA[12U]="COLON_COLON";static char _tmp2CB[11U]="IDENTIFIER";static char _tmp2CC[17U]="INTEGER_CONSTANT";static char _tmp2CD[7U]="STRING";static char _tmp2CE[8U]="WSTRING";static char _tmp2CF[19U]="CHARACTER_CONSTANT";static char _tmp2D0[20U]="WCHARACTER_CONSTANT";static char _tmp2D1[18U]="FLOATING_CONSTANT";static char _tmp2D2[9U]="TYPE_VAR";static char _tmp2D3[13U]="TYPEDEF_NAME";static char _tmp2D4[16U]="QUAL_IDENTIFIER";static char _tmp2D5[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D6[10U]="ATTRIBUTE";static char _tmp2D7[8U]="ASM_TOK";static char _tmp2D8[4U]="';'";static char _tmp2D9[4U]="'{'";static char _tmp2DA[4U]="'}'";static char _tmp2DB[4U]="','";static char _tmp2DC[4U]="'*'";static char _tmp2DD[4U]="'='";static char _tmp2DE[4U]="'<'";static char _tmp2DF[4U]="'>'";static char _tmp2E0[4U]="'('";static char _tmp2E1[4U]="')'";static char _tmp2E2[4U]="'_'";static char _tmp2E3[4U]="'$'";static char _tmp2E4[4U]="':'";static char _tmp2E5[4U]="'.'";static char _tmp2E6[4U]="'['";static char _tmp2E7[4U]="']'";static char _tmp2E8[4U]="'@'";static char _tmp2E9[4U]="'?'";static char _tmp2EA[4U]="'+'";static char _tmp2EB[4U]="'|'";static char _tmp2EC[4U]="'^'";static char _tmp2ED[4U]="'&'";static char _tmp2EE[4U]="'-'";static char _tmp2EF[4U]="'/'";static char _tmp2F0[4U]="'%'";static char _tmp2F1[4U]="'~'";static char _tmp2F2[4U]="'!'";static char _tmp2F3[5U]="prog";static char _tmp2F4[17U]="translation_unit";static char _tmp2F5[18U]="tempest_on_action";static char _tmp2F6[19U]="tempest_off_action";static char _tmp2F7[16U]="extern_c_action";static char _tmp2F8[13U]="end_extern_c";static char _tmp2F9[14U]="hide_list_opt";static char _tmp2FA[17U]="hide_list_values";static char _tmp2FB[16U]="export_list_opt";static char _tmp2FC[12U]="export_list";static char _tmp2FD[19U]="export_list_values";static char _tmp2FE[13U]="override_opt";static char _tmp2FF[21U]="external_declaration";static char _tmp300[15U]="optional_comma";static char _tmp301[20U]="function_definition";static char _tmp302[21U]="function_definition2";static char _tmp303[13U]="using_action";static char _tmp304[15U]="unusing_action";static char _tmp305[17U]="namespace_action";static char _tmp306[19U]="unnamespace_action";static char _tmp307[12U]="declaration";static char _tmp308[17U]="declaration_list";static char _tmp309[23U]="declaration_specifiers";static char _tmp30A[24U]="storage_class_specifier";static char _tmp30B[15U]="attributes_opt";static char _tmp30C[11U]="attributes";static char _tmp30D[15U]="attribute_list";static char _tmp30E[10U]="attribute";static char _tmp30F[15U]="type_specifier";static char _tmp310[25U]="type_specifier_notypedef";static char _tmp311[5U]="kind";static char _tmp312[15U]="type_qualifier";static char _tmp313[15U]="enum_specifier";static char _tmp314[11U]="enum_field";static char _tmp315[22U]="enum_declaration_list";static char _tmp316[26U]="struct_or_union_specifier";static char _tmp317[16U]="type_params_opt";static char _tmp318[16U]="struct_or_union";static char _tmp319[24U]="struct_declaration_list";static char _tmp31A[25U]="struct_declaration_list0";static char _tmp31B[21U]="init_declarator_list";static char _tmp31C[22U]="init_declarator_list0";static char _tmp31D[16U]="init_declarator";static char _tmp31E[19U]="struct_declaration";static char _tmp31F[25U]="specifier_qualifier_list";static char _tmp320[35U]="notypedef_specifier_qualifier_list";static char _tmp321[23U]="struct_declarator_list";static char _tmp322[24U]="struct_declarator_list0";static char _tmp323[18U]="struct_declarator";static char _tmp324[20U]="requires_clause_opt";static char _tmp325[19U]="ensures_clause_opt";static char _tmp326[19U]="datatype_specifier";static char _tmp327[14U]="qual_datatype";static char _tmp328[19U]="datatypefield_list";static char _tmp329[20U]="datatypefield_scope";static char _tmp32A[14U]="datatypefield";static char _tmp32B[11U]="declarator";static char _tmp32C[23U]="declarator_withtypedef";static char _tmp32D[18U]="direct_declarator";static char _tmp32E[30U]="direct_declarator_withtypedef";static char _tmp32F[8U]="pointer";static char _tmp330[12U]="one_pointer";static char _tmp331[14U]="pointer_quals";static char _tmp332[13U]="pointer_qual";static char _tmp333[23U]="pointer_null_and_bound";static char _tmp334[14U]="pointer_bound";static char _tmp335[18U]="zeroterm_qual_opt";static char _tmp336[8U]="rgn_opt";static char _tmp337[11U]="tqual_list";static char _tmp338[20U]="parameter_type_list";static char _tmp339[9U]="type_var";static char _tmp33A[16U]="optional_effect";static char _tmp33B[19U]="optional_rgn_order";static char _tmp33C[10U]="rgn_order";static char _tmp33D[16U]="optional_inject";static char _tmp33E[11U]="effect_set";static char _tmp33F[14U]="atomic_effect";static char _tmp340[11U]="region_set";static char _tmp341[15U]="parameter_list";static char _tmp342[22U]="parameter_declaration";static char _tmp343[16U]="identifier_list";static char _tmp344[17U]="identifier_list0";static char _tmp345[12U]="initializer";static char _tmp346[18U]="array_initializer";static char _tmp347[17U]="initializer_list";static char _tmp348[12U]="designation";static char _tmp349[16U]="designator_list";static char _tmp34A[11U]="designator";static char _tmp34B[10U]="type_name";static char _tmp34C[14U]="any_type_name";static char _tmp34D[15U]="type_name_list";static char _tmp34E[20U]="abstract_declarator";static char _tmp34F[27U]="direct_abstract_declarator";static char _tmp350[10U]="statement";static char _tmp351[18U]="labeled_statement";static char _tmp352[21U]="expression_statement";static char _tmp353[19U]="compound_statement";static char _tmp354[16U]="block_item_list";static char _tmp355[20U]="selection_statement";static char _tmp356[15U]="switch_clauses";static char _tmp357[20U]="iteration_statement";static char _tmp358[15U]="jump_statement";static char _tmp359[12U]="exp_pattern";static char _tmp35A[20U]="conditional_pattern";static char _tmp35B[19U]="logical_or_pattern";static char _tmp35C[20U]="logical_and_pattern";static char _tmp35D[21U]="inclusive_or_pattern";static char _tmp35E[21U]="exclusive_or_pattern";static char _tmp35F[12U]="and_pattern";static char _tmp360[17U]="equality_pattern";static char _tmp361[19U]="relational_pattern";static char _tmp362[14U]="shift_pattern";static char _tmp363[17U]="additive_pattern";static char _tmp364[23U]="multiplicative_pattern";static char _tmp365[13U]="cast_pattern";static char _tmp366[14U]="unary_pattern";static char _tmp367[16U]="postfix_pattern";static char _tmp368[16U]="primary_pattern";static char _tmp369[8U]="pattern";static char _tmp36A[19U]="tuple_pattern_list";static char _tmp36B[20U]="tuple_pattern_list0";static char _tmp36C[14U]="field_pattern";static char _tmp36D[19U]="field_pattern_list";static char _tmp36E[20U]="field_pattern_list0";static char _tmp36F[11U]="expression";static char _tmp370[22U]="assignment_expression";static char _tmp371[20U]="assignment_operator";static char _tmp372[23U]="conditional_expression";static char _tmp373[20U]="constant_expression";static char _tmp374[22U]="logical_or_expression";static char _tmp375[23U]="logical_and_expression";static char _tmp376[24U]="inclusive_or_expression";static char _tmp377[24U]="exclusive_or_expression";static char _tmp378[15U]="and_expression";static char _tmp379[20U]="equality_expression";static char _tmp37A[22U]="relational_expression";static char _tmp37B[17U]="shift_expression";static char _tmp37C[20U]="additive_expression";static char _tmp37D[26U]="multiplicative_expression";static char _tmp37E[16U]="cast_expression";static char _tmp37F[17U]="unary_expression";static char _tmp380[9U]="asm_expr";static char _tmp381[13U]="volatile_opt";static char _tmp382[12U]="asm_out_opt";static char _tmp383[12U]="asm_outlist";static char _tmp384[11U]="asm_in_opt";static char _tmp385[11U]="asm_inlist";static char _tmp386[11U]="asm_io_elt";static char _tmp387[16U]="asm_clobber_opt";static char _tmp388[17U]="asm_clobber_list";static char _tmp389[15U]="unary_operator";static char _tmp38A[19U]="postfix_expression";static char _tmp38B[17U]="field_expression";static char _tmp38C[19U]="primary_expression";static char _tmp38D[25U]="argument_expression_list";static char _tmp38E[26U]="argument_expression_list0";static char _tmp38F[9U]="constant";static char _tmp390[20U]="qual_opt_identifier";static char _tmp391[17U]="qual_opt_typedef";static char _tmp392[18U]="struct_union_name";static char _tmp393[11U]="field_name";static char _tmp394[12U]="right_angle";
# 1593 "parse.y"
static struct _fat_ptr Cyc_yytname[313U]={{_tmp25C,_tmp25C,_tmp25C + 2U},{_tmp25D,_tmp25D,_tmp25D + 6U},{_tmp25E,_tmp25E,_tmp25E + 12U},{_tmp25F,_tmp25F,_tmp25F + 5U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 7U},{_tmp262,_tmp262,_tmp262 + 7U},{_tmp263,_tmp263,_tmp263 + 8U},{_tmp264,_tmp264,_tmp264 + 5U},{_tmp265,_tmp265,_tmp265 + 5U},{_tmp266,_tmp266,_tmp266 + 6U},{_tmp267,_tmp267,_tmp267 + 4U},{_tmp268,_tmp268,_tmp268 + 5U},{_tmp269,_tmp269,_tmp269 + 6U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 7U},{_tmp26C,_tmp26C,_tmp26C + 9U},{_tmp26D,_tmp26D,_tmp26D + 6U},{_tmp26E,_tmp26E,_tmp26E + 9U},{_tmp26F,_tmp26F,_tmp26F + 9U},{_tmp270,_tmp270,_tmp270 + 7U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 5U},{_tmp273,_tmp273,_tmp273 + 8U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 3U},{_tmp278,_tmp278,_tmp278 + 5U},{_tmp279,_tmp279,_tmp279 + 7U},{_tmp27A,_tmp27A,_tmp27A + 6U},{_tmp27B,_tmp27B,_tmp27B + 3U},{_tmp27C,_tmp27C,_tmp27C + 4U},{_tmp27D,_tmp27D,_tmp27D + 5U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 6U},{_tmp280,_tmp280,_tmp280 + 7U},{_tmp281,_tmp281,_tmp281 + 5U},{_tmp282,_tmp282,_tmp282 + 7U},{_tmp283,_tmp283,_tmp283 + 16U},{_tmp284,_tmp284,_tmp284 + 10U},{_tmp285,_tmp285,_tmp285 + 8U},{_tmp286,_tmp286,_tmp286 + 4U},{_tmp287,_tmp287,_tmp287 + 6U},{_tmp288,_tmp288,_tmp288 + 4U},{_tmp289,_tmp289,_tmp289 + 6U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 9U},{_tmp28C,_tmp28C,_tmp28C + 5U},{_tmp28D,_tmp28D,_tmp28D + 4U},{_tmp28E,_tmp28E,_tmp28E + 9U},{_tmp28F,_tmp28F,_tmp28F + 9U},{_tmp290,_tmp290,_tmp290 + 6U},{_tmp291,_tmp291,_tmp291 + 10U},{_tmp292,_tmp292,_tmp292 + 9U},{_tmp293,_tmp293,_tmp293 + 7U},{_tmp294,_tmp294,_tmp294 + 8U},{_tmp295,_tmp295,_tmp295 + 15U},{_tmp296,_tmp296,_tmp296 + 7U},{_tmp297,_tmp297,_tmp297 + 8U},{_tmp298,_tmp298,_tmp298 + 5U},{_tmp299,_tmp299,_tmp299 + 9U},{_tmp29A,_tmp29A,_tmp29A + 6U},{_tmp29B,_tmp29B,_tmp29B + 7U},{_tmp29C,_tmp29C,_tmp29C + 5U},{_tmp29D,_tmp29D,_tmp29D + 8U},{_tmp29E,_tmp29E,_tmp29E + 7U},{_tmp29F,_tmp29F,_tmp29F + 8U},{_tmp2A0,_tmp2A0,_tmp2A0 + 7U},{_tmp2A1,_tmp2A1,_tmp2A1 + 10U},{_tmp2A2,_tmp2A2,_tmp2A2 + 11U},{_tmp2A3,_tmp2A3,_tmp2A3 + 8U},{_tmp2A4,_tmp2A4,_tmp2A4 + 8U},{_tmp2A5,_tmp2A5,_tmp2A5 + 10U},{_tmp2A6,_tmp2A6,_tmp2A6 + 9U},{_tmp2A7,_tmp2A7,_tmp2A7 + 13U},{_tmp2A8,_tmp2A8,_tmp2A8 + 10U},{_tmp2A9,_tmp2A9,_tmp2A9 + 9U},{_tmp2AA,_tmp2AA,_tmp2AA + 13U},{_tmp2AB,_tmp2AB,_tmp2AB + 14U},{_tmp2AC,_tmp2AC,_tmp2AC + 14U},{_tmp2AD,_tmp2AD,_tmp2AD + 13U},{_tmp2AE,_tmp2AE,_tmp2AE + 12U},{_tmp2AF,_tmp2AF,_tmp2AF + 16U},{_tmp2B0,_tmp2B0,_tmp2B0 + 14U},{_tmp2B1,_tmp2B1,_tmp2B1 + 12U},{_tmp2B2,_tmp2B2,_tmp2B2 + 16U},{_tmp2B3,_tmp2B3,_tmp2B3 + 7U},{_tmp2B4,_tmp2B4,_tmp2B4 + 7U},{_tmp2B5,_tmp2B5,_tmp2B5 + 7U},{_tmp2B6,_tmp2B6,_tmp2B6 + 8U},{_tmp2B7,_tmp2B7,_tmp2B7 + 9U},{_tmp2B8,_tmp2B8,_tmp2B8 + 6U},{_tmp2B9,_tmp2B9,_tmp2B9 + 6U},{_tmp2BA,_tmp2BA,_tmp2BA + 6U},{_tmp2BB,_tmp2BB,_tmp2BB + 6U},{_tmp2BC,_tmp2BC,_tmp2BC + 7U},{_tmp2BD,_tmp2BD,_tmp2BD + 6U},{_tmp2BE,_tmp2BE,_tmp2BE + 11U},{_tmp2BF,_tmp2BF,_tmp2BF + 11U},{_tmp2C0,_tmp2C0,_tmp2C0 + 11U},{_tmp2C1,_tmp2C1,_tmp2C1 + 11U},{_tmp2C2,_tmp2C2,_tmp2C2 + 11U},{_tmp2C3,_tmp2C3,_tmp2C3 + 12U},{_tmp2C4,_tmp2C4,_tmp2C4 + 13U},{_tmp2C5,_tmp2C5,_tmp2C5 + 11U},{_tmp2C6,_tmp2C6,_tmp2C6 + 11U},{_tmp2C7,_tmp2C7,_tmp2C7 + 10U},{_tmp2C8,_tmp2C8,_tmp2C8 + 9U},{_tmp2C9,_tmp2C9,_tmp2C9 + 11U},{_tmp2CA,_tmp2CA,_tmp2CA + 12U},{_tmp2CB,_tmp2CB,_tmp2CB + 11U},{_tmp2CC,_tmp2CC,_tmp2CC + 17U},{_tmp2CD,_tmp2CD,_tmp2CD + 7U},{_tmp2CE,_tmp2CE,_tmp2CE + 8U},{_tmp2CF,_tmp2CF,_tmp2CF + 19U},{_tmp2D0,_tmp2D0,_tmp2D0 + 20U},{_tmp2D1,_tmp2D1,_tmp2D1 + 18U},{_tmp2D2,_tmp2D2,_tmp2D2 + 9U},{_tmp2D3,_tmp2D3,_tmp2D3 + 13U},{_tmp2D4,_tmp2D4,_tmp2D4 + 16U},{_tmp2D5,_tmp2D5,_tmp2D5 + 18U},{_tmp2D6,_tmp2D6,_tmp2D6 + 10U},{_tmp2D7,_tmp2D7,_tmp2D7 + 8U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 4U},{_tmp2F3,_tmp2F3,_tmp2F3 + 5U},{_tmp2F4,_tmp2F4,_tmp2F4 + 17U},{_tmp2F5,_tmp2F5,_tmp2F5 + 18U},{_tmp2F6,_tmp2F6,_tmp2F6 + 19U},{_tmp2F7,_tmp2F7,_tmp2F7 + 16U},{_tmp2F8,_tmp2F8,_tmp2F8 + 13U},{_tmp2F9,_tmp2F9,_tmp2F9 + 14U},{_tmp2FA,_tmp2FA,_tmp2FA + 17U},{_tmp2FB,_tmp2FB,_tmp2FB + 16U},{_tmp2FC,_tmp2FC,_tmp2FC + 12U},{_tmp2FD,_tmp2FD,_tmp2FD + 19U},{_tmp2FE,_tmp2FE,_tmp2FE + 13U},{_tmp2FF,_tmp2FF,_tmp2FF + 21U},{_tmp300,_tmp300,_tmp300 + 15U},{_tmp301,_tmp301,_tmp301 + 20U},{_tmp302,_tmp302,_tmp302 + 21U},{_tmp303,_tmp303,_tmp303 + 13U},{_tmp304,_tmp304,_tmp304 + 15U},{_tmp305,_tmp305,_tmp305 + 17U},{_tmp306,_tmp306,_tmp306 + 19U},{_tmp307,_tmp307,_tmp307 + 12U},{_tmp308,_tmp308,_tmp308 + 17U},{_tmp309,_tmp309,_tmp309 + 23U},{_tmp30A,_tmp30A,_tmp30A + 24U},{_tmp30B,_tmp30B,_tmp30B + 15U},{_tmp30C,_tmp30C,_tmp30C + 11U},{_tmp30D,_tmp30D,_tmp30D + 15U},{_tmp30E,_tmp30E,_tmp30E + 10U},{_tmp30F,_tmp30F,_tmp30F + 15U},{_tmp310,_tmp310,_tmp310 + 25U},{_tmp311,_tmp311,_tmp311 + 5U},{_tmp312,_tmp312,_tmp312 + 15U},{_tmp313,_tmp313,_tmp313 + 15U},{_tmp314,_tmp314,_tmp314 + 11U},{_tmp315,_tmp315,_tmp315 + 22U},{_tmp316,_tmp316,_tmp316 + 26U},{_tmp317,_tmp317,_tmp317 + 16U},{_tmp318,_tmp318,_tmp318 + 16U},{_tmp319,_tmp319,_tmp319 + 24U},{_tmp31A,_tmp31A,_tmp31A + 25U},{_tmp31B,_tmp31B,_tmp31B + 21U},{_tmp31C,_tmp31C,_tmp31C + 22U},{_tmp31D,_tmp31D,_tmp31D + 16U},{_tmp31E,_tmp31E,_tmp31E + 19U},{_tmp31F,_tmp31F,_tmp31F + 25U},{_tmp320,_tmp320,_tmp320 + 35U},{_tmp321,_tmp321,_tmp321 + 23U},{_tmp322,_tmp322,_tmp322 + 24U},{_tmp323,_tmp323,_tmp323 + 18U},{_tmp324,_tmp324,_tmp324 + 20U},{_tmp325,_tmp325,_tmp325 + 19U},{_tmp326,_tmp326,_tmp326 + 19U},{_tmp327,_tmp327,_tmp327 + 14U},{_tmp328,_tmp328,_tmp328 + 19U},{_tmp329,_tmp329,_tmp329 + 20U},{_tmp32A,_tmp32A,_tmp32A + 14U},{_tmp32B,_tmp32B,_tmp32B + 11U},{_tmp32C,_tmp32C,_tmp32C + 23U},{_tmp32D,_tmp32D,_tmp32D + 18U},{_tmp32E,_tmp32E,_tmp32E + 30U},{_tmp32F,_tmp32F,_tmp32F + 8U},{_tmp330,_tmp330,_tmp330 + 12U},{_tmp331,_tmp331,_tmp331 + 14U},{_tmp332,_tmp332,_tmp332 + 13U},{_tmp333,_tmp333,_tmp333 + 23U},{_tmp334,_tmp334,_tmp334 + 14U},{_tmp335,_tmp335,_tmp335 + 18U},{_tmp336,_tmp336,_tmp336 + 8U},{_tmp337,_tmp337,_tmp337 + 11U},{_tmp338,_tmp338,_tmp338 + 20U},{_tmp339,_tmp339,_tmp339 + 9U},{_tmp33A,_tmp33A,_tmp33A + 16U},{_tmp33B,_tmp33B,_tmp33B + 19U},{_tmp33C,_tmp33C,_tmp33C + 10U},{_tmp33D,_tmp33D,_tmp33D + 16U},{_tmp33E,_tmp33E,_tmp33E + 11U},{_tmp33F,_tmp33F,_tmp33F + 14U},{_tmp340,_tmp340,_tmp340 + 11U},{_tmp341,_tmp341,_tmp341 + 15U},{_tmp342,_tmp342,_tmp342 + 22U},{_tmp343,_tmp343,_tmp343 + 16U},{_tmp344,_tmp344,_tmp344 + 17U},{_tmp345,_tmp345,_tmp345 + 12U},{_tmp346,_tmp346,_tmp346 + 18U},{_tmp347,_tmp347,_tmp347 + 17U},{_tmp348,_tmp348,_tmp348 + 12U},{_tmp349,_tmp349,_tmp349 + 16U},{_tmp34A,_tmp34A,_tmp34A + 11U},{_tmp34B,_tmp34B,_tmp34B + 10U},{_tmp34C,_tmp34C,_tmp34C + 14U},{_tmp34D,_tmp34D,_tmp34D + 15U},{_tmp34E,_tmp34E,_tmp34E + 20U},{_tmp34F,_tmp34F,_tmp34F + 27U},{_tmp350,_tmp350,_tmp350 + 10U},{_tmp351,_tmp351,_tmp351 + 18U},{_tmp352,_tmp352,_tmp352 + 21U},{_tmp353,_tmp353,_tmp353 + 19U},{_tmp354,_tmp354,_tmp354 + 16U},{_tmp355,_tmp355,_tmp355 + 20U},{_tmp356,_tmp356,_tmp356 + 15U},{_tmp357,_tmp357,_tmp357 + 20U},{_tmp358,_tmp358,_tmp358 + 15U},{_tmp359,_tmp359,_tmp359 + 12U},{_tmp35A,_tmp35A,_tmp35A + 20U},{_tmp35B,_tmp35B,_tmp35B + 19U},{_tmp35C,_tmp35C,_tmp35C + 20U},{_tmp35D,_tmp35D,_tmp35D + 21U},{_tmp35E,_tmp35E,_tmp35E + 21U},{_tmp35F,_tmp35F,_tmp35F + 12U},{_tmp360,_tmp360,_tmp360 + 17U},{_tmp361,_tmp361,_tmp361 + 19U},{_tmp362,_tmp362,_tmp362 + 14U},{_tmp363,_tmp363,_tmp363 + 17U},{_tmp364,_tmp364,_tmp364 + 23U},{_tmp365,_tmp365,_tmp365 + 13U},{_tmp366,_tmp366,_tmp366 + 14U},{_tmp367,_tmp367,_tmp367 + 16U},{_tmp368,_tmp368,_tmp368 + 16U},{_tmp369,_tmp369,_tmp369 + 8U},{_tmp36A,_tmp36A,_tmp36A + 19U},{_tmp36B,_tmp36B,_tmp36B + 20U},{_tmp36C,_tmp36C,_tmp36C + 14U},{_tmp36D,_tmp36D,_tmp36D + 19U},{_tmp36E,_tmp36E,_tmp36E + 20U},{_tmp36F,_tmp36F,_tmp36F + 11U},{_tmp370,_tmp370,_tmp370 + 22U},{_tmp371,_tmp371,_tmp371 + 20U},{_tmp372,_tmp372,_tmp372 + 23U},{_tmp373,_tmp373,_tmp373 + 20U},{_tmp374,_tmp374,_tmp374 + 22U},{_tmp375,_tmp375,_tmp375 + 23U},{_tmp376,_tmp376,_tmp376 + 24U},{_tmp377,_tmp377,_tmp377 + 24U},{_tmp378,_tmp378,_tmp378 + 15U},{_tmp379,_tmp379,_tmp379 + 20U},{_tmp37A,_tmp37A,_tmp37A + 22U},{_tmp37B,_tmp37B,_tmp37B + 17U},{_tmp37C,_tmp37C,_tmp37C + 20U},{_tmp37D,_tmp37D,_tmp37D + 26U},{_tmp37E,_tmp37E,_tmp37E + 16U},{_tmp37F,_tmp37F,_tmp37F + 17U},{_tmp380,_tmp380,_tmp380 + 9U},{_tmp381,_tmp381,_tmp381 + 13U},{_tmp382,_tmp382,_tmp382 + 12U},{_tmp383,_tmp383,_tmp383 + 12U},{_tmp384,_tmp384,_tmp384 + 11U},{_tmp385,_tmp385,_tmp385 + 11U},{_tmp386,_tmp386,_tmp386 + 11U},{_tmp387,_tmp387,_tmp387 + 16U},{_tmp388,_tmp388,_tmp388 + 17U},{_tmp389,_tmp389,_tmp389 + 15U},{_tmp38A,_tmp38A,_tmp38A + 19U},{_tmp38B,_tmp38B,_tmp38B + 17U},{_tmp38C,_tmp38C,_tmp38C + 19U},{_tmp38D,_tmp38D,_tmp38D + 25U},{_tmp38E,_tmp38E,_tmp38E + 26U},{_tmp38F,_tmp38F,_tmp38F + 9U},{_tmp390,_tmp390,_tmp390 + 20U},{_tmp391,_tmp391,_tmp391 + 17U},{_tmp392,_tmp392,_tmp392 + 18U},{_tmp393,_tmp393,_tmp393 + 11U},{_tmp394,_tmp394,_tmp394 + 12U}};
# 1651
static short Cyc_yyr1[561U]={0,151,152,152,152,152,152,152,152,152,152,152,152,153,154,155,156,157,157,158,158,158,159,159,160,160,160,161,161,161,162,162,163,163,163,164,164,165,165,165,165,166,166,167,168,169,170,171,171,171,171,171,171,171,172,172,173,173,173,173,173,173,173,173,173,173,173,174,174,174,174,174,174,174,175,175,176,177,177,178,178,178,178,179,179,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,180,181,182,182,182,183,183,183,184,184,185,185,185,186,186,186,186,186,187,187,188,188,189,189,190,190,191,192,192,193,193,194,195,195,195,195,195,195,196,196,196,196,196,196,197,198,198,199,199,199,199,200,200,201,201,202,202,202,203,203,204,204,204,204,205,205,205,206,206,207,207,208,208,209,209,209,209,209,209,209,209,209,209,210,210,210,210,210,210,210,210,210,210,210,211,211,212,213,213,214,214,214,214,214,214,214,214,215,215,215,216,216,217,217,217,218,218,218,219,219,220,220,220,220,221,221,222,222,223,223,224,224,225,225,226,226,227,227,227,227,228,228,229,229,230,230,230,231,232,232,233,233,234,234,234,234,234,235,235,235,235,236,236,237,237,238,238,239,239,240,240,240,240,240,241,241,242,242,242,243,243,243,243,243,243,243,243,243,243,243,244,244,244,244,244,244,245,246,246,247,247,248,248,248,248,248,248,248,248,249,249,249,249,249,249,250,250,250,250,250,250,251,251,251,251,251,251,251,251,251,251,251,251,251,251,252,252,252,252,252,252,252,252,253,254,254,255,255,256,256,257,257,258,258,259,259,260,260,260,261,261,261,261,261,262,262,262,263,263,263,264,264,264,264,265,265,266,266,266,266,266,266,267,268,269,269,269,269,269,269,269,269,269,269,269,269,269,269,269,269,269,270,270,270,271,271,272,272,273,273,273,274,274,275,275,276,276,276,277,277,277,277,277,277,277,277,277,277,277,278,278,278,278,278,278,278,279,280,280,281,281,282,282,283,283,284,284,285,285,285,286,286,286,286,286,287,287,287,288,288,288,289,289,289,289,290,290,291,291,291,291,291,291,291,291,291,291,291,291,291,291,291,291,291,291,291,291,291,292,293,293,294,294,294,295,295,296,296,296,297,297,298,299,299,299,300,300,301,301,301,302,302,302,302,302,302,302,302,302,302,302,303,303,303,303,304,304,304,304,304,304,304,304,304,304,304,305,306,306,307,307,307,307,307,308,308,309,309,310,310,311,311,312,312};
# 1710
static short Cyc_yyr2[561U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1769
static short Cyc_yydefact[1133U]={0,34,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,127,128,63,0,0,97,0,0,73,0,0,165,104,106,0,0,0,13,14,0,0,0,551,231,553,552,554,0,217,0,100,0,217,216,1,0,0,0,0,32,0,0,33,0,56,65,59,83,61,94,95,0,98,0,0,176,0,201,204,99,180,125,71,70,64,0,113,0,58,550,0,551,546,547,548,549,0,125,0,0,391,0,0,0,254,0,393,394,43,45,0,0,0,0,0,0,0,0,166,0,0,0,214,0,0,0,0,215,0,0,0,2,0,0,0,0,47,0,133,134,136,57,66,60,62,129,555,556,125,125,0,54,0,0,36,0,233,0,189,177,202,0,208,209,212,213,0,211,210,222,204,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,535,536,496,0,0,0,0,0,516,514,515,0,420,422,436,444,446,448,450,452,454,457,462,465,468,472,0,474,517,534,532,551,403,0,0,0,0,404,0,0,402,50,0,0,125,0,0,0,143,139,141,274,276,0,0,52,0,0,8,9,0,125,557,558,232,108,0,0,0,181,101,252,0,249,10,11,0,3,0,5,0,48,0,0,0,36,0,130,131,156,124,0,163,0,0,0,0,0,0,0,0,0,0,0,0,551,304,306,0,314,308,0,312,297,298,299,0,300,301,302,0,55,36,136,35,37,281,0,239,255,0,0,235,233,0,219,0,0,0,224,74,223,205,0,118,114,0,0,0,482,0,0,494,438,472,0,439,440,0,0,0,0,0,0,0,0,0,0,475,476,497,493,0,478,0,0,0,0,479,477,0,96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,426,427,428,429,430,431,432,433,434,435,425,0,480,0,523,524,0,0,0,538,0,125,395,0,0,0,417,551,558,0,0,0,0,270,413,418,0,415,0,0,392,410,411,0,408,256,0,0,0,0,277,0,247,144,149,145,147,140,142,233,0,283,275,284,560,559,0,103,105,0,0,107,123,80,79,0,77,218,182,233,251,178,283,253,190,191,0,102,16,30,44,0,46,0,135,137,258,257,36,38,120,132,0,0,0,151,152,159,0,125,125,171,0,0,0,0,0,551,0,0,0,343,344,345,0,0,347,0,0,0,315,309,136,313,307,305,39,0,188,240,0,0,0,246,234,241,159,0,0,0,235,187,221,220,183,219,0,0,225,75,126,119,443,116,112,0,0,0,0,551,259,264,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,537,544,0,543,421,445,0,447,449,451,453,455,456,460,461,458,459,463,464,466,467,469,470,471,424,423,522,519,0,521,0,0,0,406,407,0,273,0,414,268,271,401,0,269,405,398,0,49,0,399,0,278,0,150,146,148,0,235,0,219,0,285,0,233,0,296,280,0,0,125,0,0,0,143,0,125,0,233,0,200,179,250,0,22,4,6,40,0,155,138,156,0,0,154,235,164,173,172,0,0,167,0,0,0,322,0,0,0,0,0,0,342,346,0,0,0,310,303,0,41,282,233,0,243,0,0,161,236,0,159,239,227,184,206,207,225,203,481,0,0,0,260,0,265,484,0,0,0,0,0,533,489,492,0,0,498,0,0,473,540,0,0,520,518,0,0,0,0,272,416,419,409,412,400,279,248,159,0,286,287,219,0,0,235,219,0,0,51,235,551,0,76,78,0,192,0,0,235,0,219,0,0,0,17,23,153,0,157,129,162,174,171,171,0,0,0,0,0,0,0,0,0,0,0,0,0,322,348,0,311,42,235,0,244,242,0,185,0,161,235,0,226,529,0,528,0,261,266,0,0,0,0,0,441,442,522,521,503,0,542,525,0,545,437,539,541,0,396,397,161,159,289,295,159,0,288,219,0,129,0,81,193,199,159,0,198,194,219,0,0,0,0,0,0,0,170,169,316,322,0,0,0,0,0,0,350,351,353,355,357,359,361,363,366,371,374,377,381,383,389,390,0,0,319,328,0,0,0,0,0,0,0,0,0,0,349,229,245,0,237,186,228,233,483,0,0,267,485,486,0,0,491,490,0,509,503,499,501,495,526,0,292,161,161,159,290,53,0,0,161,159,195,31,25,0,0,27,0,7,158,122,0,0,0,322,0,387,0,0,384,322,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,385,322,0,330,0,0,0,338,0,0,0,0,0,0,321,0,0,235,531,530,0,0,0,0,510,509,506,504,0,500,527,291,294,161,121,0,196,161,26,24,28,0,0,19,175,317,318,0,0,0,0,322,324,354,0,356,358,360,362,364,365,369,370,367,368,372,373,375,376,378,379,380,0,323,329,331,332,0,340,339,0,334,0,0,0,160,238,230,0,0,0,0,0,512,511,0,505,502,293,0,197,29,18,20,0,320,386,0,382,325,0,322,333,341,335,336,0,263,262,487,0,508,0,507,82,21,0,352,322,326,337,0,513,388,327,488,0,0,0};
# 1886
static short Cyc_yydefgoto[162U]={1130,53,54,55,56,485,874,1036,786,787,958,666,57,319,58,303,59,487,60,489,61,151,62,63,554,242,471,472,243,66,258,244,68,173,174,69,171,70,280,281,136,137,138,282,245,453,500,501,502,676,819,71,72,681,682,683,73,503,74,477,75,76,168,169,77,121,550,334,719,639,78,640,544,710,536,540,541,447,327,267,102,103,567,492,568,427,428,429,246,320,321,641,459,306,307,308,309,310,311,801,312,313,887,888,889,890,891,892,893,894,895,896,897,898,899,900,901,902,430,439,440,431,432,433,314,206,407,207,559,208,209,210,211,212,213,214,215,216,217,218,219,365,366,841,938,939,1019,940,1021,1088,220,221,826,222,585,586,223,224,80,959,434,463};
# 1906
static short Cyc_yypact[1133U]={2861,- -32768,- -32768,- -32768,- -32768,- 36,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3631,234,3,- -32768,3631,2701,- -32768,99,46,- -32768,41,51,96,85,134,- -32768,- -32768,170,715,263,- -32768,242,- -32768,- -32768,- -32768,206,260,601,278,324,260,- -32768,- -32768,274,286,343,2719,- -32768,600,647,- -32768,251,3631,3631,3631,- -32768,3631,- -32768,- -32768,704,- -32768,99,3542,630,211,109,793,- -32768,- -32768,348,388,360,- -32768,99,393,7042,- -32768,- -32768,2842,266,- -32768,- -32768,- -32768,- -32768,420,348,455,7042,- -32768,408,2842,452,457,505,- -32768,- 33,- -32768,- -32768,3845,3845,293,475,2719,2719,7042,309,- -32768,261,487,7042,- -32768,544,513,261,4320,- -32768,2719,2719,3002,- -32768,2719,3002,2719,3002,- -32768,527,529,- -32768,3409,- -32768,- -32768,- -32768,- -32768,4320,- -32768,- -32768,348,221,1833,- -32768,3542,251,534,3845,3717,5155,- -32768,630,- -32768,542,- -32768,- -32768,- -32768,- -32768,546,- -32768,- -32768,166,793,3845,- -32768,- -32768,564,589,619,99,7349,621,7399,7042,7203,633,655,657,659,677,679,681,688,698,706,7399,7399,- -32768,- -32768,825,7449,1509,714,7449,7449,- -32768,- -32768,- -32768,155,- -32768,- -32768,- 22,707,717,718,719,147,93,737,98,167,- -32768,752,7449,142,2,- -32768,757,162,- -32768,2842,173,766,1103,789,288,832,- -32768,- -32768,798,7042,348,832,772,4062,4320,4406,4320,354,- -32768,13,13,- -32768,802,788,- -32768,- -32768,364,348,- -32768,- -32768,- -32768,- -32768,68,795,790,- -32768,- -32768,246,412,- -32768,- -32768,- -32768,801,- -32768,811,- -32768,813,- -32768,544,5266,3542,534,824,4320,- -32768,899,828,99,829,823,198,826,4474,831,850,841,847,5377,2425,4474,369,839,- -32768,- -32768,846,1981,1981,251,1981,- -32768,- -32768,- -32768,854,- -32768,- -32768,- -32768,381,- -32768,534,852,- -32768,- -32768,842,10,872,- -32768,- 9,853,851,399,855,765,862,7042,3845,- -32768,867,- -32768,- -32768,10,99,- -32768,7042,864,1509,- -32768,4320,1509,- -32768,- -32768,- -32768,4585,- -32768,894,7042,7042,7042,7042,7042,7042,882,7042,4320,1059,- -32768,- -32768,- -32768,- -32768,871,- -32768,1981,861,456,7042,- -32768,- -32768,7042,- -32768,7449,7042,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7042,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7042,- -32768,261,- -32768,- -32768,5488,261,7042,- -32768,874,348,- -32768,877,878,880,- -32768,117,420,261,7042,2842,308,- -32768,- -32768,- -32768,879,886,885,2842,- -32768,- -32768,- -32768,883,888,- -32768,428,1103,889,3845,- -32768,881,896,- -32768,4406,4406,4406,- -32768,- -32768,3136,5599,460,- -32768,645,- -32768,- -32768,- 9,- -32768,- -32768,892,922,- -32768,909,- -32768,904,908,910,- -32768,- -32768,1326,- -32768,670,143,- -32768,- -32768,- -32768,4320,- -32768,- -32768,995,- -32768,2719,- -32768,2719,- -32768,- -32768,- -32768,- -32768,534,- -32768,- -32768,- -32768,976,7042,920,918,- -32768,- 21,342,348,348,884,7042,7042,914,923,7042,913,1021,2277,929,- -32768,- -32768,- -32768,464,1009,- -32768,5710,2129,2568,- -32768,- -32768,3409,- -32768,- -32768,- -32768,- -32768,3845,- -32768,- -32768,4320,924,4148,- -32768,- -32768,915,978,- 9,926,4234,851,- -32768,- -32768,- -32768,- -32768,765,927,- 80,817,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,928,936,951,974,950,- -32768,- -32768,770,5266,955,962,963,964,965,490,968,970,987,210,994,983,7253,- -32768,- -32768,989,996,- -32768,707,228,717,718,719,147,93,93,737,737,737,737,98,98,167,167,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,991,- -32768,205,3845,5041,4320,- -32768,4320,- -32768,986,- -32768,- -32768,- -32768,- -32768,1186,- -32768,- -32768,- -32768,1361,- -32768,1000,- -32768,272,- -32768,4320,- -32768,- -32768,- -32768,997,851,998,765,990,645,3845,3934,5821,- -32768,- -32768,7042,1004,348,7153,999,68,3270,1001,348,3845,3717,5932,- -32768,670,- -32768,1008,1089,- -32768,- -32768,- -32768,598,- -32768,- -32768,899,1007,7042,- -32768,851,- -32768,- -32768,- -32768,1010,99,578,499,504,7042,876,521,4474,1011,6043,6154,646,- -32768,- -32768,1015,1017,1012,1981,- -32768,3542,- -32768,842,1018,3845,- -32768,1022,- 9,1066,- -32768,1019,978,103,- -32768,- -32768,- -32768,- -32768,817,- -32768,1024,400,1024,1023,- -32768,4699,- -32768,- -32768,7042,7042,1127,7042,7203,- -32768,- -32768,- -32768,261,261,1020,1025,4813,- -32768,- -32768,7042,7042,- -32768,- -32768,10,776,1044,1046,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,978,1026,- -32768,- -32768,765,10,1027,851,765,1028,538,- -32768,851,1034,1031,- -32768,- -32768,1032,- -32768,10,1036,851,1045,765,1038,3002,1055,1133,- -32768,- -32768,7042,- -32768,4320,- -32768,1050,78,884,4474,1058,1051,3068,1052,1060,4474,7042,6265,661,6376,669,6487,876,- -32768,1065,- -32768,- -32768,851,307,- -32768,- -32768,1063,- -32768,1078,1066,851,4320,- -32768,- -32768,262,- -32768,7042,- -32768,- -32768,5266,1064,1068,1071,1077,- -32768,894,1074,1076,12,1080,- -32768,- -32768,799,- -32768,- -32768,- -32768,- -32768,5041,- -32768,- -32768,1066,978,- -32768,- -32768,978,1083,- -32768,765,1093,4320,1086,- -32768,- -32768,- -32768,978,1088,- -32768,- -32768,765,1098,568,1100,2719,1096,1106,4320,- -32768,- -32768,1205,876,1109,7499,1104,2568,7449,1108,- -32768,- 18,- -32768,1146,1102,785,837,129,845,429,193,- -32768,- -32768,- -32768,- -32768,1150,7449,1981,- -32768,- -32768,548,4474,550,6598,4474,571,6709,6820,671,1121,- -32768,- -32768,- -32768,7042,1122,- -32768,- -32768,1018,- -32768,422,328,- -32768,- -32768,- -32768,4320,1225,- -32768,- -32768,1119,77,333,- -32768,- -32768,- -32768,- -32768,4927,- -32768,1066,1066,978,- -32768,- -32768,1128,1126,1066,978,- -32768,- -32768,- -32768,1129,1130,683,309,- -32768,- -32768,- -32768,573,4474,1131,876,1509,- -32768,4320,1134,- -32768,1682,7449,7042,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7449,7042,- -32768,876,1135,- -32768,4474,4474,583,- -32768,4474,4474,595,4474,604,6931,- -32768,1136,- 9,851,- -32768,- -32768,2568,1137,1132,7042,1147,340,- -32768,- -32768,1148,- -32768,- -32768,- -32768,- -32768,1066,- -32768,1151,- -32768,1066,- -32768,- -32768,- -32768,309,1140,694,- -32768,- -32768,- -32768,1145,1139,1149,7449,876,- -32768,707,344,717,718,718,147,93,93,737,737,737,737,98,98,167,167,- -32768,- -32768,- -32768,410,- -32768,- -32768,- -32768,- -32768,4474,- -32768,- -32768,4474,- -32768,4474,4474,616,- -32768,- -32768,- -32768,1152,815,1141,4320,628,- -32768,1154,1148,- -32768,- -32768,- -32768,1142,- -32768,- -32768,- -32768,- -32768,309,- -32768,1024,400,- -32768,- -32768,7042,1682,- -32768,- -32768,- -32768,- -32768,4474,- -32768,- -32768,- -32768,1144,- -32768,1160,- -32768,- -32768,- -32768,431,- -32768,876,- -32768,- -32768,1153,- -32768,- -32768,- -32768,- -32768,1262,1279,- -32768};
# 2023
static short Cyc_yypgoto[162U]={- -32768,60,- -32768,- -32768,- -32768,- -32768,- -32768,185,- -32768,- -32768,249,- -32768,- -32768,- 211,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 58,- 117,57,- -32768,- -32768,0,634,- -32768,138,- 125,1161,33,- -32768,- -32768,- 140,- -32768,30,1249,- 759,- -32768,- -32768,- -32768,1016,1014,958,390,- -32768,- -32768,618,- 52,- 596,- -32768,- -32768,151,- -32768,- -32768,18,- 237,1218,- 392,124,- -32768,1138,- -32768,- -32768,1248,- 462,- -32768,582,- 150,- 149,- 144,- 119,298,596,602,- 448,- 499,- 123,- 442,- 135,- -32768,- 253,- 169,- 586,- 322,- -32768,887,- 192,- 75,- 115,- 158,- 281,116,- -32768,- -32768,- 63,- 260,- -32768,- 591,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,453,1073,- -32768,697,890,- -32768,402,643,- -32768,- 171,- 389,- 165,- 363,- 360,- 376,939,- 355,- 375,- 315,- 358,- 339,- 159,686,- -32768,- -32768,- -32768,- -32768,387,- -32768,- 877,311,- -32768,528,967,225,- -32768,- 367,- -32768,347,481,- 66,- 69,- 101,- 105};
# 2047
static short Cyc_yytable[7650U]={64,147,266,591,146,325,369,594,595,347,153,326,350,588,649,150,351,259,590,335,328,64,278,259,491,64,593,569,476,748,600,601,876,67,247,248,341,620,367,707,664,372,373,526,527,611,529,255,122,448,146,602,603,717,67,337,537,64,67,674,1020,408,462,64,64,64,123,64,495,596,597,598,599,64,157,376,279,81,83,974,139,150,87,679,680,469,663,458,316,715,67,304,238,315,704,711,67,67,67,239,67,460,950,664,460,532,67,479,581,42,671,415,663,64,64,675,538,130,451,377,140,141,142,975,143,936,230,64,64,64,152,64,64,64,64,86,758,533,65,64,262,461,416,464,461,1091,67,67,937,64,561,64,562,563,844,462,698,108,157,65,67,67,67,65,67,67,67,67,578,558,317,109,67,252,253,539,644,284,286,470,762,110,67,546,67,384,385,268,269,270,936,271,272,273,274,65,152,644,557,481,159,65,65,65,- 168,65,305,111,152,113,41,65,1117,1018,535,494,534,481,917,44,315,982,983,386,387,923,112,324,227,409,410,411,556,604,605,606,- 233,47,657,- 233,390,382,383,450,391,304,304,228,304,51,52,65,65,- 557,41,558,944,553,114,984,985,657,43,44,45,65,65,65,443,65,65,65,65,227,412,475,452,65,64,413,414,456,374,669,42,468,790,65,375,65,966,419,228,569,317,392,64,736,410,411,333,854,115,64,64,858,64,420,609,41,304,67,612,539,393,394,726,117,44,798,869,990,41,528,619,451,451,451,558,67,509,44,374,510,555,152,67,67,119,67,991,992,412,48,746,41,448,737,414,1025,1026,170,118,87,44,374,1030,41,285,84,305,305,41,305,744,43,44,45,64,457,633,44,256,105,47,135,1041,227,475,47,257,925,1046,48,456,120,51,52,124,478,- 255,51,52,- 255,539,926,228,948,127,927,875,67,266,831,1067,757,514,504,954,658,128,701,481,521,462,374,65,249,569,776,41,436,250,741,749,305,750,714,43,44,45,1092,481,776,65,1094,105,622,481,812,920,65,65,448,65,425,426,615,105,462,450,450,450,41,1103,374,125,692,703,648,1022,43,44,45,1014,702,699,1089,129,937,150,374,830,172,498,656,1018,662,170,104,1104,79,47,452,452,452,455,64,205,64,709,374,456,522,51,52,765,467,670,747,232,523,766,85,558,531,65,106,374,107,779,256,825,- 15,1123,64,780,254,176,257,67,760,67,324,64,781,545,831,64,79,764,1128,1009,256,1012,677,678,374,79,482,233,257,226,79,778,483,1105,667,67,668,229,145,630,148,234,374,79,67,791,539,814,67,711,836,1127,175,231,837,927,822,106,988,152,846,105,989,235,105,929,457,105,152,106,374,236,152,105,718,695,583,658,374,455,251,79,79,830,145,456,478,370,1050,1051,79,481,1053,1054,481,79,79,79,1047,79,79,79,79,1049,374,558,260,827,831,504,732,1052,65,374,65,1059,1060,815,374,796,79,237,838,839,797,813,442,700,304,847,315,648,995,263,857,374,1061,1062,275,861,65,802,41,276,175,318,855,821,867,65,662,44,374,65,1055,1056,1057,1058,860,47,865,331,374,48,374,332,41,418,996,771,998,51,52,438,43,44,45,830,338,438,971,956,919,957,520,374,64,482,64,794,924,1002,795,1038,852,106,41,374,106,41,1045,106,339,1071,43,44,45,106,44,374,46,131,132,47,972,1074,47,498,374,67,48,67,19,20,1076,51,52,1015,51,52,374,370,994,480,370,340,1110,558,718,46,344,964,374,305,79,152,575,154,1115,155,261,480,352,506,46,156,511,808,133,134,374,105,645,1042,646,1043,589,504,1011,105,647,64,911,79,353,374,354,105,355,46,914,504,1007,374,504,374,330,659,945,660,378,946,700,146,1034,661,356,1035,357,395,358,952,41,613,67,1097,175,359,1098,1082,43,44,45,1013,388,389,144,360,1063,1064,1065,16,17,18,65,361,65,636,637,638,364,871,1122,371,304,548,549,396,397,398,399,400,401,402,403,404,405,379,539,380,343,381,346,348,348,160,161,162,163,164,88,64,165,166,167,362,363,621,406,417,348,421,1102,348,348,627,679,680,1037,1081,1114,146,1027,724,725,799,800,435,1031,848,849,445,64,348,67,106,441,684,685,880,465,688,304,106,693,907,466,493,473,65,474,106,942,943,484,89,978,979,980,981,1121,961,986,987,486,67,488,437,1112,374,225,91,878,879,92,93,94,496,95,44,505,507,508,480,96,512,480,97,516,305,515,98,517,99,100,79,146,79,518,105,64,552,524,105,101,525,480,530,277,346,535,462,480,542,543,547,46,560,376,493,576,582,570,571,572,573,574,827,551,577,580,614,624,67,634,616,617,41,618,65,625,584,629,628,587,43,44,45,626,632,635,650,997,348,47,1001,1037,305,498,146,651,652,499,653,655,607,51,52,654,665,65,672,673,686,304,687,689,608,690,769,694,696,584,705,708,674,712,716,720,348,721,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,1039,755,265,722,723,- 557,41,727,728,729,730,731,805,807,43,44,45,46,643,88,733,283,734,47,64,106,738,498,739,106,65,348,265,1069,1070,51,52,1072,1073,735,1075,742,743,745,751,756,188,770,763,759,761,774,784,777,785,792,480,67,789,809,810,324,803,88,811,105,818,816,740,820,480,834,828,480,850,840,851,842,853,856,862,305,793,863,864,584,859,584,866,41,91,194,195,92,93,94,870,868,44,872,873,877,881,882,348,906,1106,905,918,1107,345,1108,1109,199,921,922,930,951,89,449,931,454,932,933,908,910,934,913,935,916,422,493,941,423,91,947,949,92,93,94,953,424,44,955,960,1124,88,96,962,928,97,963,965,967,98,970,99,100,283,425,426,976,65,973,977,993,1008,101,1010,1016,1017,903,1029,1028,1032,1033,1040,493,1068,1087,936,1131,1093,1085,79,1096,1044,348,1079,1084,1099,1100,1126,1113,1118,1101,1125,1111,1132,106,1116,89,1119,1095,264,1129,232,116,775,768,788,490,158,752,497,773,423,91,126,824,92,93,94,783,424,44,336,1080,823,817,96,444,1000,97,623,1004,1006,98,592,99,100,753,425,426,1023,1120,904,579,584,1090,101,0,631,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,145,0,79,0,0,0,0,0,348,0,22,23,24,0,0,493,0,370,832,833,0,835,0,0,1048,0,0,30,0,0,493,0,0,845,31,32,0,0,0,0,0,0,1066,0,0,0,38,0,0,88,0,0,0,0,0,0,1078,0,39,40,265,0,0,1083,0,348,1086,0,0,0,0,0,0,0,0,0,0,348,0,0,265,322,0,0,41,0,0,265,145,0,0,42,43,44,45,46,0,324,0,0,0,47,0,0,89,475,0,49,50,0,0,456,0,51,52,0,754,0,0,225,91,493,348,92,93,94,0,95,44,0,265,0,0,96,0,0,97,0,0,493,98,265,99,100,0,0,0,0,0,0,265,0,0,101,0,0,0,0,0,0,0,0,0,145,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,969,31,32,348,187,0,0,0,188,0,145,189,190,38,191,0,0,493,0,0,0,348,0,0,0,39,40,0,192,193,0,0,0,0,0,265,0,0,348,0,0,0,0,0,0,449,0,0,0,265,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,368,0,0,197,0,0,0,198,0,49,302,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204,348,0,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,799,800,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,348,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,283,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,265,0,0,0,0,0,0,0,0,348,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,197,0,0,0,198,0,49,302,0,283,0,0,0,0,200,0,0,201,202,0,0,203,204,0,0,265,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,301,0,197,0,0,0,198,0,49,302,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,197,0,0,0,198,0,49,302,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,513,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,197,0,0,0,198,0,49,302,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,296,88,26,180,0,0,0,0,0,181,27,0,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,691,0,0,0,197,0,0,0,198,0,49,302,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,296,88,0,0,0,0,0,0,0,0,27,0,0,0,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,197,0,0,0,345,0,49,302,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,197,0,0,0,198,0,49,302,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,88,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,89,0,0,0,0,0,0,39,40,0,0,0,0,0,0,90,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,97,41,0,0,98,0,99,100,42,43,44,45,46,0,0,0,- 12,101,47,0,0,0,48,0,49,50,0,0,0,0,51,52,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,89,0,0,0,0,0,0,0,39,40,0,0,0,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,97,0,41,0,98,0,99,100,0,42,43,44,45,46,0,0,0,101,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,883,884,0,0,0,0,0,0,0,0,0,0,0,0,0,0,88,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,89,0,0,0,0,0,0,0,0,22,23,24,0,0,0,225,91,0,0,92,93,94,0,95,44,0,30,0,0,96,0,0,97,31,32,0,885,0,99,100,0,0,0,0,0,38,886,0,0,101,202,0,0,203,204,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,324,0,0,0,47,0,0,0,455,0,49,50,0,0,456,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,498,0,49,50,0,0,0,0,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,277,0,0,0,0,49,50,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,323,0,0,0,0,0,0,42,43,0,45,46,0,324,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,240,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,241,22,23,24,0,0,0,0,0,49,50,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,322,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,324,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,446,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,706,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,713,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,177,178,287,0,288,289,290,291,292,293,294,295,0,0,0,179,88,0,180,297,0,0,0,0,181,42,298,0,0,46,182,183,184,185,186,0,0,0,0,187,0,49,50,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,513,91,194,195,92,93,94,0,0,44,0,0,196,300,149,0,0,197,0,0,0,198,0,0,199,177,178,0,0,0,0,200,564,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,565,91,194,195,92,93,94,0,257,44,0,0,196,0,349,566,0,197,0,0,0,198,0,0,199,0,425,426,177,178,0,200,0,0,201,202,0,0,203,204,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,565,91,194,195,92,93,94,0,257,44,0,0,196,0,349,829,0,197,0,0,0,198,0,0,199,0,425,426,177,178,0,200,0,0,201,202,0,0,203,204,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,565,91,194,195,92,93,94,0,257,44,0,0,196,0,349,843,0,197,0,0,0,198,0,0,199,0,425,426,177,178,0,200,0,0,201,202,0,0,203,204,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,565,91,194,195,92,93,94,0,257,44,0,0,196,0,349,1024,0,197,0,0,0,198,0,0,199,0,425,426,177,178,0,200,0,0,201,202,0,0,203,204,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,565,91,194,195,92,93,94,0,257,44,0,0,196,0,349,0,0,197,0,0,0,198,0,0,199,0,425,426,177,178,0,200,0,0,201,202,0,0,203,204,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,0,0,199,177,178,0,329,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,349,0,0,197,0,0,0,198,0,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,519,0,0,0,197,0,0,0,198,0,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,610,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,0,0,199,177,178,0,642,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,697,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,0,0,199,177,178,0,767,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,0,0,199,177,178,0,782,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,804,0,0,0,197,0,0,0,198,0,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,806,0,0,0,197,0,0,0,198,0,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,909,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,912,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,915,0,0,0,197,0,0,0,198,0,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,999,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,1003,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,1005,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,1077,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,198,0,0,199,177,178,0,0,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,177,178,0,0,0,0,0,0,0,0,0,0,0,192,193,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,772,91,194,195,92,93,94,188,0,44,189,190,196,191,177,178,0,197,0,0,0,198,0,0,199,0,0,192,193,179,88,200,0,0,201,202,0,0,203,204,0,0,0,0,182,183,184,185,186,0,41,91,194,195,92,93,94,188,0,44,189,190,196,191,349,0,0,197,0,0,0,198,0,0,199,0,0,192,193,0,0,200,0,0,201,202,0,0,203,204,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,177,178,196,0,740,0,0,197,0,0,0,198,0,0,199,179,88,0,0,0,0,200,0,0,201,202,0,0,203,204,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,177,178,0,0,0,0,0,0,0,0,0,0,0,192,193,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,41,91,194,195,92,93,94,188,0,44,189,190,196,191,177,178,0,197,0,0,0,342,0,0,199,0,0,192,193,179,88,200,0,0,201,202,0,0,203,204,0,0,0,0,182,183,184,185,186,0,41,91,194,195,92,93,94,188,0,44,189,190,196,191,177,178,0,197,0,0,0,345,0,0,199,0,0,192,193,179,88,200,0,0,201,202,0,0,203,204,0,0,0,0,182,183,184,185,186,0,41,91,194,195,92,93,94,188,0,44,189,190,196,191,0,0,0,197,0,0,0,198,0,0,199,0,0,192,193,0,0,200,0,0,201,202,0,0,203,204,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,197,0,0,0,968,0,0,199,0,0,0,0,0,0,200,0,0,201,202,0,0,203,204};
# 2815
static short Cyc_yycheck[7650U]={0,70,125,379,70,155,198,382,383,180,73,155,181,376,462,73,181,118,378,168,155,21,139,124,277,25,381,349,265,615,388,389,791,0,109,110,176,426,197,538,482,200,201,303,304,412,306,116,48,241,116,390,391,133,21,170,65,57,25,80,937,220,142,63,64,65,48,67,279,384,385,386,387,73,74,97,139,113,21,97,62,139,25,5,6,17,478,245,151,551,57,149,125,151,536,543,63,64,65,132,67,91,861,545,91,316,73,265,368,118,499,109,504,113,114,136,125,57,243,141,63,64,65,141,67,113,96,127,128,129,73,131,132,133,134,132,635,127,0,139,122,131,140,248,131,1022,113,114,136,149,342,151,344,345,740,142,523,111,158,21,127,128,129,25,131,132,133,134,360,340,152,130,139,113,114,324,457,147,148,111,642,130,149,327,151,92,93,127,128,129,113,131,132,133,134,57,139,478,338,265,76,63,64,65,126,67,149,111,151,124,111,73,1089,136,111,278,321,283,809,120,278,92,93,130,131,821,130,124,111,87,88,89,337,392,393,394,133,128,475,136,142,94,95,243,146,303,304,130,306,140,141,113,114,136,111,426,852,332,124,130,131,498,119,120,121,127,128,129,238,131,132,133,134,111,132,132,243,139,278,137,138,138,127,494,118,255,675,149,133,151,881,118,130,615,276,128,296,87,88,89,134,763,132,303,304,767,306,134,409,111,368,278,413,462,147,148,569,54,120,686,782,128,111,305,425,450,451,452,499,296,132,120,127,135,334,278,303,304,132,306,147,148,132,132,139,111,538,137,138,945,946,130,110,296,120,127,952,111,137,125,303,304,111,306,136,119,120,121,368,245,445,120,111,26,128,124,967,111,132,128,119,823,973,132,138,125,140,141,110,265,124,140,141,127,543,133,130,859,124,137,789,368,525,725,995,133,290,283,870,475,124,528,478,297,142,127,278,124,740,656,111,133,129,582,616,368,618,546,119,120,121,1027,498,670,296,1031,89,129,504,699,133,303,304,635,306,137,138,417,101,142,450,451,452,111,1045,127,132,515,533,459,127,119,120,121,136,528,524,127,125,136,528,127,725,113,132,475,136,477,130,26,136,0,128,450,451,452,132,487,86,489,542,127,138,124,140,141,646,133,498,614,98,132,646,22,675,124,368,26,127,28,660,111,112,125,1105,515,660,115,125,119,487,640,489,124,524,660,127,849,528,48,645,1122,921,111,112,505,506,127,57,127,132,119,89,62,659,133,136,487,515,489,130,70,124,72,101,127,75,524,677,708,704,528,1010,732,133,84,111,732,137,713,89,142,515,744,227,146,124,230,831,455,233,524,101,127,127,528,239,554,124,133,656,127,132,118,113,114,849,116,138,475,198,977,978,122,670,980,981,673,127,128,129,974,131,132,133,134,976,127,789,132,721,943,498,133,979,487,127,489,986,987,705,127,133,152,129,736,737,133,701,237,524,699,747,701,644,905,133,766,127,988,989,124,771,515,133,111,127,176,124,764,712,780,524,663,120,127,528,982,983,984,985,133,128,778,132,127,132,127,132,111,227,133,652,133,140,141,233,119,120,121,943,127,239,885,126,814,128,295,127,699,127,701,124,822,133,127,133,759,227,111,127,230,111,973,233,126,133,119,120,121,239,120,127,122,124,125,128,886,133,128,132,127,699,132,701,20,21,133,140,141,932,140,141,127,342,904,265,345,129,133,921,718,122,132,877,127,699,276,701,357,130,133,132,120,283,132,285,122,138,288,124,124,125,127,427,130,968,132,970,377,656,925,435,138,784,124,305,132,127,132,443,132,122,124,670,124,127,673,127,156,130,853,132,96,856,689,872,124,138,132,127,132,60,132,866,111,414,784,124,338,132,127,1014,119,120,121,927,90,91,125,132,990,991,992,17,18,19,699,132,701,450,451,452,18,784,1105,132,905,83,84,98,99,100,101,102,103,104,105,106,107,143,1010,144,177,145,179,180,181,75,76,77,78,79,41,874,82,83,84,192,193,427,129,125,197,118,1044,200,201,435,5,6,960,1011,1085,960,947,126,127,22,23,111,953,126,127,132,905,220,874,427,111,508,509,796,111,512,973,435,515,802,131,277,126,784,133,443,126,127,126,96,144,145,94,95,1104,874,90,91,126,905,126,108,126,127,111,112,794,795,115,116,117,126,119,120,125,125,132,475,125,132,478,128,111,905,132,132,124,134,135,487,1035,489,124,625,973,331,136,629,145,132,498,126,129,296,111,142,504,133,136,133,122,126,97,349,111,133,352,353,354,355,356,1101,139,359,132,130,126,973,126,131,131,111,131,874,127,371,127,133,374,119,120,121,136,133,127,132,909,340,128,912,1098,973,132,1098,111,125,136,132,127,395,140,141,133,47,905,124,127,132,1105,125,136,407,30,650,124,45,412,132,142,80,133,133,133,376,127,378,379,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,965,629,125,133,111,136,111,133,127,127,127,127,691,692,119,120,121,122,456,41,133,144,133,128,1105,625,113,132,126,629,973,426,155,998,999,140,141,1002,1003,133,1005,133,127,133,139,126,68,124,139,133,133,133,125,133,46,126,656,1105,132,125,124,124,132,41,133,799,81,126,125,131,670,25,130,673,111,136,111,133,133,133,127,1105,682,133,133,523,139,525,133,111,112,113,114,115,116,117,139,133,120,125,48,132,125,133,499,126,1071,136,124,1074,132,1076,1077,135,132,118,133,112,96,242,133,244,132,127,803,804,133,806,133,808,108,569,133,111,112,133,124,115,116,117,133,119,120,126,125,1110,41,125,133,828,128,126,28,125,132,132,134,135,281,137,138,96,1105,136,143,96,126,145,127,25,132,799,127,126,126,126,126,615,124,113,113,0,112,132,784,126,133,582,133,133,126,133,113,133,133,127,133,126,0,799,127,96,1098,1035,124,133,885,39,655,647,673,276,75,108,281,653,111,112,51,718,115,116,117,661,119,120,169,1010,713,708,125,239,911,128,428,914,915,132,380,134,135,625,137,138,938,1101,799,361,686,1019,145,- 1,443,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,872,- 1,874,- 1,- 1,- 1,- 1,- 1,675,- 1,37,38,39,- 1,- 1,725,- 1,968,728,729,- 1,731,- 1,- 1,975,- 1,- 1,54,- 1,- 1,740,- 1,- 1,743,61,62,- 1,- 1,- 1,- 1,- 1,- 1,993,- 1,- 1,- 1,73,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,1007,- 1,85,86,455,- 1,- 1,1014,- 1,732,1017,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,744,- 1,- 1,475,108,- 1,- 1,111,- 1,- 1,482,960,- 1,- 1,118,119,120,121,122,- 1,124,- 1,- 1,- 1,128,- 1,- 1,96,132,- 1,134,135,- 1,- 1,138,- 1,140,141,- 1,108,- 1,- 1,111,112,831,789,115,116,117,- 1,119,120,- 1,525,- 1,- 1,125,- 1,- 1,128,- 1,- 1,849,132,536,134,135,- 1,- 1,- 1,- 1,- 1,- 1,545,- 1,- 1,145,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1035,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,883,61,62,886,64,- 1,- 1,- 1,68,- 1,1098,71,72,73,74,- 1,- 1,943,- 1,- 1,- 1,904,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,646,- 1,- 1,921,- 1,- 1,- 1,- 1,- 1,- 1,656,- 1,- 1,- 1,660,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,974,- 1,976,977,978,979,980,981,982,983,984,985,986,987,988,989,990,991,992,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,1044,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,791,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,823,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1104,- 1,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,861,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,877,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,111,112,- 1,- 1,115,116,117,- 1,119,120,- 1,- 1,- 1,- 1,125,- 1,- 1,128,111,- 1,- 1,132,- 1,134,135,118,119,120,121,122,- 1,- 1,- 1,126,145,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,140,141,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,111,112,- 1,- 1,115,116,117,- 1,119,120,- 1,- 1,- 1,- 1,125,- 1,- 1,128,- 1,111,- 1,132,- 1,134,135,- 1,118,119,120,121,122,- 1,- 1,- 1,145,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,140,141,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,118,119,120,121,122,- 1,- 1,- 1,126,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,140,141,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,96,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,111,112,- 1,- 1,115,116,117,- 1,119,120,- 1,54,- 1,- 1,125,- 1,- 1,128,61,62,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,- 1,73,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,- 1,124,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,138,- 1,140,141,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,134,135,- 1,- 1,- 1,- 1,140,141,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,- 1,- 1,125,- 1,- 1,- 1,129,- 1,- 1,- 1,- 1,134,135,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,118,119,- 1,121,122,- 1,- 1,125,37,38,39,40,- 1,- 1,- 1,- 1,134,135,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,111,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,- 1,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,- 1,- 1,125,37,38,39,- 1,- 1,- 1,- 1,- 1,134,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,- 1,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,37,38,39,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,37,38,39,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,108,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,118,119,- 1,121,122,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,135,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,118,51,- 1,- 1,122,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,134,135,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,124,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,- 1,- 1,- 1,142,32,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,119,120,- 1,- 1,123,- 1,125,126,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,137,138,25,26,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,119,120,- 1,- 1,123,- 1,125,126,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,137,138,25,26,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,119,120,- 1,- 1,123,- 1,125,126,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,137,138,25,26,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,119,120,- 1,- 1,123,- 1,125,126,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,137,138,25,26,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,119,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,137,138,25,26,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,139,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,139,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,139,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,139,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,133,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,25,26,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,111,112,113,114,115,116,117,68,- 1,120,71,72,123,74,25,26,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,88,89,40,41,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,111,112,113,114,115,116,117,68,- 1,120,71,72,123,74,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,88,89,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,25,26,123,- 1,125,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,40,41,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,88,89,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,111,112,113,114,115,116,117,68,- 1,120,71,72,123,74,25,26,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,88,89,40,41,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,111,112,113,114,115,116,117,68,- 1,120,71,72,123,74,25,26,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,88,89,40,41,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,111,112,113,114,115,116,117,68,- 1,120,71,72,123,74,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,88,89,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,111,112,113,114,115,116,117,- 1,- 1,120,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,- 1,- 1,132,- 1,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,145,146,- 1,- 1,149,150};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple32{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple33{struct _fat_ptr f1;void*f2;};static char _tmp425[8U]="stdcall";static char _tmp426[6U]="cdecl";static char _tmp427[9U]="fastcall";static char _tmp428[9U]="noreturn";static char _tmp429[6U]="const";static char _tmp42A[8U]="aligned";static char _tmp42B[7U]="packed";static char _tmp42C[7U]="shared";static char _tmp42D[7U]="unused";static char _tmp42E[5U]="weak";static char _tmp42F[10U]="dllimport";static char _tmp430[10U]="dllexport";static char _tmp431[23U]="no_instrument_function";static char _tmp432[12U]="constructor";static char _tmp433[11U]="destructor";static char _tmp434[22U]="no_check_memory_usage";static char _tmp435[5U]="pure";static char _tmp436[14U]="always_inline";struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
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
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp766;(_tmp766.YYINITIALSVAL).tag=72U,(_tmp766.YYINITIALSVAL).val=0;_tmp766;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp6AF=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp6AF),sizeof(short),_tmp6AF);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp6AE=200U;struct Cyc_Yystacktype*_tmp6AD=({struct _RegionHandle*_tmp81A=yyregion;_region_malloc(_tmp81A,_check_times(_tmp6AE,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp765=200U;unsigned i;for(i=0;i < _tmp765;++ i){(_tmp6AD[i]).v=yylval,({struct Cyc_Yyltype _tmp81B=Cyc_yynewloc();(_tmp6AD[i]).l=_tmp81B;});}}0;});_tmp6AD;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp81D=({const char*_tmp396="parser stack overflow";_tag_fat(_tmp396,sizeof(char),22U);});int _tmp81C=yystate;Cyc_yyerror(_tmp81D,_tmp81C,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp39A=(unsigned)yystacksize;short*_tmp399=({struct _RegionHandle*_tmp81E=yyregion;_region_malloc(_tmp81E,_check_times(_tmp39A,sizeof(short)));});({{unsigned _tmp723=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp723;++ i){
i <= (unsigned)yyssp_offset?_tmp399[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp399[i]=0);}}0;});_tag_fat(_tmp399,sizeof(short),_tmp39A);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp398=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp397=({struct _RegionHandle*_tmp81F=yyregion;_region_malloc(_tmp81F,_check_times(_tmp398,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp722=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp722;++ i){
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
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1133U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 378?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,379U,sizeof(short),yychar)): 313;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 7649)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7650U,sizeof(short),yyn))!= yychar1)goto yydefault;
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
if(yyn == 1132){
int _tmp39B=0;_npop_handler(0U);return _tmp39B;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp820=({struct Cyc_Yystacktype _tmp724;_tmp724.v=yylval,_tmp724.l=yylloc;_tmp724;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp820;});
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
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,561U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=(yyyvsp[0]).v;
# 370 "cycbison.simple"
{int _tmp39C=yyn;switch(_tmp39C){case 1U: _LL1: _LL2:
# 1194 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1197
goto _LL0;case 2U: _LL3: _LL4:
# 1200 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp821=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp821,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1204 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39F=_cycalloc(sizeof(*_tmp39F));({struct Cyc_Absyn_Decl*_tmp825=({struct Cyc_Absyn_Decl*_tmp39E=_cycalloc(sizeof(*_tmp39E));({void*_tmp824=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->tag=10U,({struct _tuple0*_tmp823=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39D->f1=_tmp823;}),({struct Cyc_List_List*_tmp822=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39D->f2=_tmp822;});_tmp39D;});_tmp39E->r=_tmp824;}),_tmp39E->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39E;});_tmp39F->hd=_tmp825;}),_tmp39F->tl=0;_tmp39F;}));
Cyc_Lex_leave_using();
# 1207
goto _LL0;case 4U: _LL7: _LL8:
# 1208 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({struct Cyc_Absyn_Decl*_tmp82A=({struct Cyc_Absyn_Decl*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({void*_tmp829=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->tag=10U,({struct _tuple0*_tmp828=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3A0->f1=_tmp828;}),({struct Cyc_List_List*_tmp827=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A0->f2=_tmp827;});_tmp3A0;});_tmp3A1->r=_tmp829;}),_tmp3A1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A1;});_tmp3A2->hd=_tmp82A;}),({struct Cyc_List_List*_tmp826=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A2->tl=_tmp826;});_tmp3A2;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1211
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct Cyc_Absyn_Decl*_tmp82F=({struct Cyc_Absyn_Decl*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({void*_tmp82E=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));_tmp3A4->tag=9U,({struct _fat_ptr*_tmp82D=({struct _fat_ptr*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct _fat_ptr _tmp82C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A3=_tmp82C;});_tmp3A3;});_tmp3A4->f1=_tmp82D;}),({struct Cyc_List_List*_tmp82B=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A4->f2=_tmp82B;});_tmp3A4;});_tmp3A5->r=_tmp82E;}),_tmp3A5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A5;});_tmp3A6->hd=_tmp82F;}),_tmp3A6->tl=0;_tmp3A6;}));
Cyc_Lex_leave_namespace();
# 1214
goto _LL0;case 6U: _LLB: _LLC:
# 1215 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct Cyc_Absyn_Decl*_tmp835=({struct Cyc_Absyn_Decl*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({void*_tmp834=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->tag=9U,({struct _fat_ptr*_tmp833=({struct _fat_ptr*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct _fat_ptr _tmp832=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A7=_tmp832;});_tmp3A7;});_tmp3A8->f1=_tmp833;}),({struct Cyc_List_List*_tmp831=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A8->f2=_tmp831;});_tmp3A8;});_tmp3A9->r=_tmp834;}),_tmp3A9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A9;});_tmp3AA->hd=_tmp835;}),({struct Cyc_List_List*_tmp830=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3AA->tl=_tmp830;});_tmp3AA;}));
goto _LL0;case 7U: _LLD: _LLE: {
# 1217 "parse.y"
int _tmp3AB=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_c_include=_tmp3AB;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple28*_tmp3AC=Cyc_yyget_YY53(&(yyyvsp[5]).v);struct _tuple28*_stmttmp18=_tmp3AC;struct _tuple28*_tmp3AD=_stmttmp18;unsigned _tmp3AF;struct Cyc_List_List*_tmp3AE;_LL460: _tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;_LL461: {struct Cyc_List_List*exs=_tmp3AE;unsigned wc=_tmp3AF;
struct Cyc_List_List*_tmp3B0=Cyc_yyget_YY54(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3B0;
if(exs != 0 && hides != 0)
({void*_tmp3B1=0U;({unsigned _tmp837=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp836=({const char*_tmp3B2="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3B2,sizeof(char),65U);});Cyc_Warn_err(_tmp837,_tmp836,_tag_fat(_tmp3B1,sizeof(void*),0U));});});
# 1224
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
# 1227
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3B3=0U;({unsigned _tmp839=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp838=({const char*_tmp3B4="expecting \"C include\"";_tag_fat(_tmp3B4,sizeof(char),22U);});Cyc_Warn_err(_tmp839,_tmp838,_tag_fat(_tmp3B3,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct Cyc_Absyn_Decl*_tmp83E=({struct Cyc_Absyn_Decl*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({void*_tmp83D=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=12U,({struct Cyc_List_List*_tmp83C=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B6->f1=_tmp83C;}),_tmp3B6->f2=cycdecls,_tmp3B6->f3=exs,({struct _tuple10*_tmp83B=({struct _tuple10*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->f1=wc,_tmp3B5->f2=hides;_tmp3B5;});_tmp3B6->f4=_tmp83B;});_tmp3B6;});_tmp3B7->r=_tmp83D;}),_tmp3B7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B7;});_tmp3B8->hd=_tmp83E;}),({struct Cyc_List_List*_tmp83A=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B8->tl=_tmp83A;});_tmp3B8;}));}else{
# 1233
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct Cyc_Absyn_Decl*_tmp842=({struct Cyc_Absyn_Decl*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({void*_tmp841=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=11U,({struct Cyc_List_List*_tmp840=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B9->f1=_tmp840;});_tmp3B9;});_tmp3BA->r=_tmp841;}),_tmp3BA->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BA;});_tmp3BB->hd=_tmp842;}),({struct Cyc_List_List*_tmp83F=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BB->tl=_tmp83F;});_tmp3BB;}));}}else{
# 1237
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct Cyc_Absyn_Decl*_tmp847=({struct Cyc_Absyn_Decl*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({void*_tmp846=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=12U,({struct Cyc_List_List*_tmp845=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BD->f1=_tmp845;}),_tmp3BD->f2=cycdecls,_tmp3BD->f3=exs,({struct _tuple10*_tmp844=({struct _tuple10*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->f1=wc,_tmp3BC->f2=hides;_tmp3BC;});_tmp3BD->f4=_tmp844;});_tmp3BD;});_tmp3BE->r=_tmp846;}),_tmp3BE->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BE;});_tmp3BF->hd=_tmp847;}),({struct Cyc_List_List*_tmp843=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BF->tl=_tmp843;});_tmp3BF;}));}
# 1240
goto _LL0;}}case 8U: _LLF: _LL10:
# 1241 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));({struct Cyc_Absyn_Decl*_tmp849=({struct Cyc_Absyn_Decl*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C0;});_tmp3C1->hd=_tmp849;}),({struct Cyc_List_List*_tmp848=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C1->tl=_tmp848;});_tmp3C1;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1243 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct Cyc_Absyn_Decl*_tmp84B=({struct Cyc_Absyn_Decl*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3C2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C2;});_tmp3C3->hd=_tmp84B;}),({struct Cyc_List_List*_tmp84A=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C3->tl=_tmp84A;});_tmp3C3;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1245 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct Cyc_Absyn_Decl*_tmp84D=({struct Cyc_Absyn_Decl*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3C4->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C4;});_tmp3C5->hd=_tmp84D;}),({struct Cyc_List_List*_tmp84C=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C5->tl=_tmp84C;});_tmp3C5;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1247 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct Cyc_Absyn_Decl*_tmp84F=({struct Cyc_Absyn_Decl*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3C6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C6;});_tmp3C7->hd=_tmp84F;}),({struct Cyc_List_List*_tmp84E=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C7->tl=_tmp84E;});_tmp3C7;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1248 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1253 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1258 "parse.y"
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E: {
# 1263 "parse.y"
struct _fat_ptr _tmp3C8=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3C8;
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp850=(struct _fat_ptr)two;Cyc_strcmp(_tmp850,({const char*_tmp3C9="C";_tag_fat(_tmp3C9,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp851=(struct _fat_ptr)two;Cyc_strcmp(_tmp851,({const char*_tmp3CA="C include";_tag_fat(_tmp3CA,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1270
({void*_tmp3CB=0U;({unsigned _tmp853=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp852=({const char*_tmp3CC="expecting \"C\" or \"C include\"";_tag_fat(_tmp3CC,sizeof(char),29U);});Cyc_Warn_err(_tmp853,_tmp852,_tag_fat(_tmp3CB,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1274
goto _LL0;}case 16U: _LL1F: _LL20:
# 1277 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1281 "parse.y"
 yyval=Cyc_YY54(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1282 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1286 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));({struct _tuple0*_tmp854=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CD->hd=_tmp854;}),_tmp3CD->tl=0;_tmp3CD;}));
goto _LL0;case 20U: _LL27: _LL28:
# 1287 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct _tuple0*_tmp855=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CE->hd=_tmp855;}),_tmp3CE->tl=0;_tmp3CE;}));
goto _LL0;case 21U: _LL29: _LL2A:
# 1289 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));({struct _tuple0*_tmp857=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CF->hd=_tmp857;}),({struct Cyc_List_List*_tmp856=Cyc_yyget_YY54(&(yyyvsp[2]).v);_tmp3CF->tl=_tmp856;});_tmp3CF;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1293 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->f1=0,_tmp3D0->f2=0U;_tmp3D0;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1294 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24U: _LL2F: _LL30:
# 1298 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct Cyc_List_List*_tmp858=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D1->f1=_tmp858;}),_tmp3D1->f2=0U;_tmp3D1;}));
goto _LL0;case 25U: _LL31: _LL32:
# 1299 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->f1=0,_tmp3D2->f2=0U;_tmp3D2;}));
goto _LL0;case 26U: _LL33: _LL34:
# 1300 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=0,_tmp3D3->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3D3;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1304 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct _tuple32*_tmp85A=({struct _tuple32*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp859=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D4->f2=_tmp859;}),_tmp3D4->f3=0;_tmp3D4;});_tmp3D5->hd=_tmp85A;}),_tmp3D5->tl=0;_tmp3D5;}));
goto _LL0;case 28U: _LL37: _LL38:
# 1305 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct _tuple32*_tmp85C=({struct _tuple32*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D6->f2=_tmp85B;}),_tmp3D6->f3=0;_tmp3D6;});_tmp3D7->hd=_tmp85C;}),_tmp3D7->tl=0;_tmp3D7;}));
goto _LL0;case 29U: _LL39: _LL3A:
# 1307 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct _tuple32*_tmp85F=({struct _tuple32*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D8->f2=_tmp85E;}),_tmp3D8->f3=0;_tmp3D8;});_tmp3D9->hd=_tmp85F;}),({struct Cyc_List_List*_tmp85D=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D9->tl=_tmp85D;});_tmp3D9;}));
goto _LL0;case 30U: _LL3B: _LL3C:
# 1311 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL3D: _LL3E:
# 1312 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 32U: _LL3F: _LL40:
# 1316 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));({struct Cyc_Absyn_Decl*_tmp862=({void*_tmp861=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp860=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3DA->f1=_tmp860;});_tmp3DA;});Cyc_Absyn_new_decl(_tmp861,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3DB->hd=_tmp862;}),_tmp3DB->tl=0;_tmp3DB;}));
goto _LL0;case 33U: _LL41: _LL42:
# 1317 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 34U: _LL43: _LL44:
# 1318 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 37U: _LL45: _LL46:
# 1327 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp865=yyr;struct Cyc_Parse_Declarator _tmp864=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp863=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp865,0,_tmp864,0,_tmp863,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 38U: _LL47: _LL48: {
# 1329 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DC=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DC;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp868=yyr;struct Cyc_Parse_Declarator _tmp867=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp866=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp868,& d,_tmp867,0,_tmp866,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 39U: _LL49: _LL4A:
# 1342 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp86C=yyr;struct Cyc_Parse_Declarator _tmp86B=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp86A=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp869=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp86C,0,_tmp86B,_tmp86A,_tmp869,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 40U: _LL4B: _LL4C: {
# 1344 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DD=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DD;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp870=yyr;struct Cyc_Parse_Declarator _tmp86F=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp86E=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp86D=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp870,& d,_tmp86F,_tmp86E,_tmp86D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1352 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DE=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DE;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp873=yyr;struct Cyc_Parse_Declarator _tmp872=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp871=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp873,& d,_tmp872,0,_tmp871,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1355 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DF=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DF;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp877=yyr;struct Cyc_Parse_Declarator _tmp876=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp875=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp874=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp877,& d,_tmp876,_tmp875,_tmp874,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 43U: _LL51: _LL52:
# 1360 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 44U: _LL53: _LL54:
# 1363
 Cyc_Lex_leave_using();
goto _LL0;case 45U: _LL55: _LL56:
# 1366
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct _fat_ptr _tmp878=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3E0=_tmp878;});_tmp3E0;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 46U: _LL57: _LL58:
# 1369
 Cyc_Lex_leave_namespace();
goto _LL0;case 47U: _LL59: _LL5A: {
# 1375 "parse.y"
int _tmp3E1=((yyyvsp[0]).l).first_line;int location=_tmp3E1;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87A=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp879=(unsigned)location;Cyc_Parse_make_declarations(_tmp87A,0,_tmp879,(unsigned)location);}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1378 "parse.y"
int _tmp3E2=((yyyvsp[0]).l).first_line;int location=_tmp3E2;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87D=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple13*_tmp87C=Cyc_yyget_YY19(&(yyyvsp[1]).v);unsigned _tmp87B=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp87D,_tmp87C,_tmp87B,(unsigned)location);}));
goto _LL0;}case 49U: _LL5D: _LL5E:
# 1382
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));({struct Cyc_Absyn_Decl*_tmp880=({struct Cyc_Absyn_Pat*_tmp87F=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp87E=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp87F,_tmp87E,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E3->hd=_tmp880;}),_tmp3E3->tl=0;_tmp3E3;}));
goto _LL0;case 50U: _LL5F: _LL60: {
# 1384 "parse.y"
struct Cyc_List_List*_tmp3E4=0;struct Cyc_List_List*vds=_tmp3E4;
{struct Cyc_List_List*_tmp3E5=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3E5;for(0;ids != 0;ids=ids->tl){
struct _fat_ptr*_tmp3E6=(struct _fat_ptr*)ids->hd;struct _fat_ptr*id=_tmp3E6;
struct _tuple0*qv=({struct _tuple0*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({union Cyc_Absyn_Nmspace _tmp881=Cyc_Absyn_Rel_n(0);_tmp3E9->f1=_tmp881;}),_tmp3E9->f2=id;_tmp3E9;});
struct Cyc_Absyn_Vardecl*_tmp3E7=({struct _tuple0*_tmp882=qv;Cyc_Absyn_new_vardecl(0U,_tmp882,Cyc_Absyn_wildtyp(0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp3E7;
vds=({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8->hd=vd,_tmp3E8->tl=vds;_tmp3E8;});}}
# 1391
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({struct Cyc_Absyn_Decl*_tmp883=Cyc_Absyn_letv_decl(vds,(unsigned)((yyyvsp[0]).l).first_line);_tmp3EA->hd=_tmp883;}),_tmp3EA->tl=0;_tmp3EA;}));
# 1394
goto _LL0;}case 51U: _LL61: _LL62: {
# 1397 "parse.y"
struct _fat_ptr _tmp3EB=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp3EB;
# 1399
if(({struct _fat_ptr _tmp884=(struct _fat_ptr)three;Cyc_zstrcmp(_tmp884,({const char*_tmp3EC="`H";_tag_fat(_tmp3EC,sizeof(char),3U);}));})== 0)
({void*_tmp3ED=0U;({unsigned _tmp886=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp885=({const char*_tmp3EE="bad occurrence of heap region";_tag_fat(_tmp3EE,sizeof(char),30U);});Cyc_Warn_err(_tmp886,_tmp885,_tag_fat(_tmp3ED,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp887=(struct _fat_ptr)three;Cyc_zstrcmp(_tmp887,({const char*_tmp3EF="`U";_tag_fat(_tmp3EF,sizeof(char),3U);}));})== 0)
({void*_tmp3F0=0U;({unsigned _tmp889=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp888=({const char*_tmp3F1="bad occurrence of unique region";_tag_fat(_tmp3F1,sizeof(char),32U);});Cyc_Warn_err(_tmp889,_tmp888,_tag_fat(_tmp3F0,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));({struct _fat_ptr*_tmp88B=({struct _fat_ptr*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));*_tmp3F5=three;_tmp3F5;});_tmp3F6->name=_tmp88B;}),_tmp3F6->identity=- 1,({void*_tmp88A=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3F6->kind=_tmp88A;});_tmp3F6;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp88F=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp88E=({struct _tuple0*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp88D=({struct _fat_ptr*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));({struct _fat_ptr _tmp88C=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3F3=_tmp88C;});_tmp3F3;});_tmp3F4->f2=_tmp88D;});_tmp3F4;});Cyc_Absyn_new_vardecl(_tmp88F,_tmp88E,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));({struct Cyc_Absyn_Decl*_tmp890=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3F2->hd=_tmp890;}),_tmp3F2->tl=0;_tmp3F2;}));
# 1408
goto _LL0;}}case 52U: _LL63: _LL64: {
# 1410
struct _fat_ptr _tmp3F7=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3F7;
if(({struct _fat_ptr _tmp891=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp891,({const char*_tmp3F8="H";_tag_fat(_tmp3F8,sizeof(char),2U);}));})== 0)
({void*_tmp3F9=0U;({unsigned _tmp893=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp892=({const char*_tmp3FA="bad occurrence of heap region `H";_tag_fat(_tmp3FA,sizeof(char),33U);});Cyc_Warn_err(_tmp893,_tmp892,_tag_fat(_tmp3F9,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp894=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp894,({const char*_tmp3FB="U";_tag_fat(_tmp3FB,sizeof(char),2U);}));})== 0)
({void*_tmp3FC=0U;({unsigned _tmp896=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp895=({const char*_tmp3FD="bad occurrence of unique region `U";_tag_fat(_tmp3FD,sizeof(char),35U);});Cyc_Warn_err(_tmp896,_tmp895,_tag_fat(_tmp3FC,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp405=_cycalloc(sizeof(*_tmp405));({struct _fat_ptr*_tmp89A=({struct _fat_ptr*_tmp404=_cycalloc(sizeof(*_tmp404));({struct _fat_ptr _tmp899=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp403=({struct Cyc_String_pa_PrintArg_struct _tmp725;_tmp725.tag=0U,_tmp725.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp725;});void*_tmp401[1U];_tmp401[0]=& _tmp403;({struct _fat_ptr _tmp898=({const char*_tmp402="`%s";_tag_fat(_tmp402,sizeof(char),4U);});Cyc_aprintf(_tmp898,_tag_fat(_tmp401,sizeof(void*),1U));});});*_tmp404=_tmp899;});_tmp404;});_tmp405->name=_tmp89A;}),_tmp405->identity=- 1,({
void*_tmp897=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp405->kind=_tmp897;});_tmp405;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp89D=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp89C=({struct _tuple0*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp89B=({struct _fat_ptr*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));*_tmp3FF=two;_tmp3FF;});_tmp400->f2=_tmp89B;});_tmp400;});Cyc_Absyn_new_vardecl(_tmp89D,_tmp89C,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));({struct Cyc_Absyn_Decl*_tmp89E=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3FE->hd=_tmp89E;}),_tmp3FE->tl=0;_tmp3FE;}));
# 1421
goto _LL0;}}case 53U: _LL65: _LL66: {
# 1423
struct _fat_ptr _tmp406=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp406;
struct _fat_ptr _tmp407=Cyc_yyget_String_tok(&(yyyvsp[3]).v);struct _fat_ptr four=_tmp407;
struct Cyc_Absyn_Exp*_tmp408=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*six=_tmp408;
if(({struct _fat_ptr _tmp89F=(struct _fat_ptr)four;Cyc_strcmp(_tmp89F,({const char*_tmp409="open";_tag_fat(_tmp409,sizeof(char),5U);}));})!= 0)({void*_tmp40A=0U;({unsigned _tmp8A1=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp8A0=({const char*_tmp40B="expecting `open'";_tag_fat(_tmp40B,sizeof(char),17U);});Cyc_Warn_err(_tmp8A1,_tmp8A0,_tag_fat(_tmp40A,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp413=_cycalloc(sizeof(*_tmp413));({struct _fat_ptr*_tmp8A5=({struct _fat_ptr*_tmp412=_cycalloc(sizeof(*_tmp412));({struct _fat_ptr _tmp8A4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp411=({struct Cyc_String_pa_PrintArg_struct _tmp726;_tmp726.tag=0U,_tmp726.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp726;});void*_tmp40F[1U];_tmp40F[0]=& _tmp411;({struct _fat_ptr _tmp8A3=({const char*_tmp410="`%s";_tag_fat(_tmp410,sizeof(char),4U);});Cyc_aprintf(_tmp8A3,_tag_fat(_tmp40F,sizeof(void*),1U));});});*_tmp412=_tmp8A4;});_tmp412;});_tmp413->name=_tmp8A5;}),_tmp413->identity=- 1,({
void*_tmp8A2=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp413->kind=_tmp8A2;});_tmp413;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8A8=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp8A7=({struct _tuple0*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8A6=({struct _fat_ptr*_tmp40D=_cycalloc(sizeof(*_tmp40D));*_tmp40D=two;_tmp40D;});_tmp40E->f2=_tmp8A6;});_tmp40E;});Cyc_Absyn_new_vardecl(_tmp8A8,_tmp8A7,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp40C=_cycalloc(sizeof(*_tmp40C));({struct Cyc_Absyn_Decl*_tmp8A9=Cyc_Absyn_region_decl(tv,vd,six,(unsigned)((yyyvsp[0]).l).first_line);_tmp40C->hd=_tmp8A9;}),_tmp40C->tl=0;_tmp40C;}));
# 1433
goto _LL0;}}case 54U: _LL67: _LL68:
# 1437 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55U: _LL69: _LL6A:
# 1439 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp8AA=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8AA,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56U: _LL6B: _LL6C:
# 1445 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp727;({enum Cyc_Parse_Storage_class*_tmp8AD=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp727.sc=_tmp8AD;}),({struct Cyc_Absyn_Tqual _tmp8AC=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp727.tq=_tmp8AC;}),({
struct Cyc_Parse_Type_specifier _tmp8AB=Cyc_Parse_empty_spec(0U);_tmp727.type_specs=_tmp8AB;}),_tmp727.is_inline=0,_tmp727.attributes=0;_tmp727;}));
goto _LL0;case 57U: _LL6D: _LL6E: {
# 1448 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp414=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp414;
if(two.sc != 0)
({void*_tmp415=0U;({unsigned _tmp8AF=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8AE=({const char*_tmp416="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp416,sizeof(char),73U);});Cyc_Warn_warn(_tmp8AF,_tmp8AE,_tag_fat(_tmp415,sizeof(void*),0U));});});
# 1452
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp728;({enum Cyc_Parse_Storage_class*_tmp8B0=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp728.sc=_tmp8B0;}),_tmp728.tq=two.tq,_tmp728.type_specs=two.type_specs,_tmp728.is_inline=two.is_inline,_tmp728.attributes=two.attributes;_tmp728;}));
# 1456
goto _LL0;}case 58U: _LL6F: _LL70:
# 1457 "parse.y"
({void*_tmp417=0U;({unsigned _tmp8B2=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8B1=({const char*_tmp418="__extension__ keyword ignored in declaration";_tag_fat(_tmp418,sizeof(char),45U);});Cyc_Warn_warn(_tmp8B2,_tmp8B1,_tag_fat(_tmp417,sizeof(void*),0U));});});
yyval=(yyyvsp[1]).v;
# 1460
goto _LL0;case 59U: _LL71: _LL72:
# 1461 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp729;_tmp729.sc=0,({struct Cyc_Absyn_Tqual _tmp8B4=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp729.tq=_tmp8B4;}),({
struct Cyc_Parse_Type_specifier _tmp8B3=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp729.type_specs=_tmp8B3;}),_tmp729.is_inline=0,_tmp729.attributes=0;_tmp729;}));
goto _LL0;case 60U: _LL73: _LL74: {
# 1464 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp419=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp419;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72A;_tmp72A.sc=two.sc,_tmp72A.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp8B7=({unsigned _tmp8B6=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8B5=two.type_specs;Cyc_Parse_combine_specifiers(_tmp8B6,_tmp8B5,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp72A.type_specs=_tmp8B7;}),_tmp72A.is_inline=two.is_inline,_tmp72A.attributes=two.attributes;_tmp72A;}));
# 1470
goto _LL0;}case 61U: _LL75: _LL76:
# 1471 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72B;_tmp72B.sc=0,({struct Cyc_Absyn_Tqual _tmp8B9=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp72B.tq=_tmp8B9;}),({struct Cyc_Parse_Type_specifier _tmp8B8=Cyc_Parse_empty_spec(0U);_tmp72B.type_specs=_tmp8B8;}),_tmp72B.is_inline=0,_tmp72B.attributes=0;_tmp72B;}));
goto _LL0;case 62U: _LL77: _LL78: {
# 1473 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41A=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41A;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72C;_tmp72C.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp8BB=({struct Cyc_Absyn_Tqual _tmp8BA=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8BA,two.tq);});_tmp72C.tq=_tmp8BB;}),_tmp72C.type_specs=two.type_specs,_tmp72C.is_inline=two.is_inline,_tmp72C.attributes=two.attributes;_tmp72C;}));
# 1478
goto _LL0;}case 63U: _LL79: _LL7A:
# 1479 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72D;_tmp72D.sc=0,({struct Cyc_Absyn_Tqual _tmp8BD=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72D.tq=_tmp8BD;}),({
struct Cyc_Parse_Type_specifier _tmp8BC=Cyc_Parse_empty_spec(0U);_tmp72D.type_specs=_tmp8BC;}),_tmp72D.is_inline=1,_tmp72D.attributes=0;_tmp72D;}));
goto _LL0;case 64U: _LL7B: _LL7C: {
# 1482 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41B=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41B;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72E;_tmp72E.sc=two.sc,_tmp72E.tq=two.tq,_tmp72E.type_specs=two.type_specs,_tmp72E.is_inline=1,_tmp72E.attributes=two.attributes;_tmp72E;}));
# 1486
goto _LL0;}case 65U: _LL7D: _LL7E:
# 1487 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72F;_tmp72F.sc=0,({struct Cyc_Absyn_Tqual _tmp8C0=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72F.tq=_tmp8C0;}),({
struct Cyc_Parse_Type_specifier _tmp8BF=Cyc_Parse_empty_spec(0U);_tmp72F.type_specs=_tmp8BF;}),_tmp72F.is_inline=0,({struct Cyc_List_List*_tmp8BE=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp72F.attributes=_tmp8BE;});_tmp72F;}));
goto _LL0;case 66U: _LL7F: _LL80: {
# 1490 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41C=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41C;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp730;_tmp730.sc=two.sc,_tmp730.tq=two.tq,_tmp730.type_specs=two.type_specs,_tmp730.is_inline=two.is_inline,({
# 1493
struct Cyc_List_List*_tmp8C2=({struct Cyc_List_List*_tmp8C1=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8C1,two.attributes);});_tmp730.attributes=_tmp8C2;});_tmp730;}));
goto _LL0;}case 67U: _LL81: _LL82: {
# 1497 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL0;}case 68U: _LL83: _LL84: {
# 1499 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL0;}case 69U: _LL85: _LL86: {
# 1501 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL0;}case 70U: _LL87: _LL88: {
# 1503 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL0;}case 71U: _LL89: _LL8A: {
# 1506 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _fat_ptr _tmp8C3=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp8C3,({const char*_tmp41D="C";_tag_fat(_tmp41D,sizeof(char),2U);}));})!= 0)
({void*_tmp41E=0U;({unsigned _tmp8C5=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C4=({const char*_tmp41F="only extern or extern \"C\" is allowed";_tag_fat(_tmp41F,sizeof(char),37U);});Cyc_Warn_err(_tmp8C5,_tmp8C4,_tag_fat(_tmp41E,sizeof(void*),0U));});});
yyval=Cyc_YY20(& externC_sc);
# 1511
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1511 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1514 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL0;}case 74U: _LL8F: _LL90:
# 1520 "parse.y"
 yyval=Cyc_YY45(0);
goto _LL0;case 75U: _LL91: _LL92:
# 1521 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76U: _LL93: _LL94:
# 1526 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77U: _LL95: _LL96:
# 1530 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp420=_cycalloc(sizeof(*_tmp420));({void*_tmp8C6=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp420->hd=_tmp8C6;}),_tmp420->tl=0;_tmp420;}));
goto _LL0;case 78U: _LL97: _LL98:
# 1531 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));({void*_tmp8C8=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp421->hd=_tmp8C8;}),({struct Cyc_List_List*_tmp8C7=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp421->tl=_tmp8C7;});_tmp421;}));
goto _LL0;case 79U: _LL99: _LL9A: {
# 1536 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple33 att_map[18U]={{{_tmp425,_tmp425,_tmp425 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp426,_tmp426,_tmp426 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp427,_tmp427,_tmp427 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp428,_tmp428,_tmp428 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp429,_tmp429,_tmp429 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp42A,_tmp42A,_tmp42A + 8U},(void*)& att_aligned},{{_tmp42B,_tmp42B,_tmp42B + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp42C,_tmp42C,_tmp42C + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp42D,_tmp42D,_tmp42D + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp42E,_tmp42E,_tmp42E + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp42F,_tmp42F,_tmp42F + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp430,_tmp430,_tmp430 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp431,_tmp431,_tmp431 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp432,_tmp432,_tmp432 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp433,_tmp433,_tmp433 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp434,_tmp434,_tmp434 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp435,_tmp435,_tmp435 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp436,_tmp436,_tmp436 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1557
struct _fat_ptr _tmp422=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp422;
# 1559
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 1561
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 18U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple33*)_check_known_subscript_notnull(att_map,18U,sizeof(struct _tuple33),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1568
if((unsigned)i == 18U){
({void*_tmp423=0U;({unsigned _tmp8CA=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C9=({const char*_tmp424="unrecognized attribute";_tag_fat(_tmp424,sizeof(char),23U);});Cyc_Warn_err(_tmp8CA,_tmp8C9,_tag_fat(_tmp423,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1573
goto _LL0;}}case 80U: _LL9B: _LL9C:
# 1573 "parse.y"
 yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 81U: _LL9D: _LL9E: {
# 1575 "parse.y"
struct _fat_ptr _tmp437=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp437;
struct Cyc_Absyn_Exp*_tmp438=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*e=_tmp438;
void*a;
if(({struct _fat_ptr _tmp8CC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CC,({const char*_tmp439="aligned";_tag_fat(_tmp439,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8CB=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CB,({const char*_tmp43A="__aligned__";_tag_fat(_tmp43A,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->tag=6U,_tmp43B->f1=e;_tmp43B;});else{
if(({struct _fat_ptr _tmp8CE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CE,({const char*_tmp43C="section";_tag_fat(_tmp43C,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8CD=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CD,({const char*_tmp43D="__section__";_tag_fat(_tmp43D,sizeof(char),12U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->tag=8U,_tmp43E->f1=str;_tmp43E;});}else{
if(({struct _fat_ptr _tmp8CF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CF,({const char*_tmp43F="__mode__";_tag_fat(_tmp43F,sizeof(char),9U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->tag=24U,_tmp440->f1=str;_tmp440;});}else{
if(({struct _fat_ptr _tmp8D0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D0,({const char*_tmp441="alias";_tag_fat(_tmp441,sizeof(char),6U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=25U,_tmp442->f1=str;_tmp442;});}else{
# 1591
int n=Cyc_Parse_exp2int((unsigned)((yyyvsp[2]).l).first_line,e);
if(({struct _fat_ptr _tmp8D2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D2,({const char*_tmp443="regparm";_tag_fat(_tmp443,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D1,({const char*_tmp444="__regparm__";_tag_fat(_tmp444,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp445=0U;({unsigned _tmp8D4=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8D3=({const char*_tmp446="regparm requires value between 0 and 3";_tag_fat(_tmp446,sizeof(char),39U);});Cyc_Warn_err(_tmp8D4,_tmp8D3,_tag_fat(_tmp445,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=0U,_tmp447->f1=n;_tmp447;});}else{
if(({struct _fat_ptr _tmp8D6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D6,({const char*_tmp448="initializes";_tag_fat(_tmp448,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp8D5=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D5,({const char*_tmp449="__initializes__";_tag_fat(_tmp449,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->tag=20U,_tmp44A->f1=n;_tmp44A;});else{
if(({struct _fat_ptr _tmp8D8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D8,({const char*_tmp44B="noliveunique";_tag_fat(_tmp44B,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp8D7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D7,({const char*_tmp44C="__noliveunique__";_tag_fat(_tmp44C,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->tag=21U,_tmp44D->f1=n;_tmp44D;});else{
if(({struct _fat_ptr _tmp8DA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DA,({const char*_tmp44E="consume";_tag_fat(_tmp44E,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D9,({const char*_tmp44F="__consume__";_tag_fat(_tmp44F,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->tag=22U,_tmp450->f1=n;_tmp450;});else{
# 1603
({void*_tmp451=0U;({unsigned _tmp8DC=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8DB=({const char*_tmp452="unrecognized attribute";_tag_fat(_tmp452,sizeof(char),23U);});Cyc_Warn_err(_tmp8DC,_tmp8DB,_tag_fat(_tmp451,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1607
yyval=Cyc_YY46(a);
# 1609
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1610 "parse.y"
struct _fat_ptr _tmp453=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp453;
struct _fat_ptr _tmp454=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr t=_tmp454;
unsigned _tmp455=({unsigned _tmp8DD=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DD,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});unsigned n=_tmp455;
unsigned _tmp456=({unsigned _tmp8DE=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DE,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));});unsigned m=_tmp456;
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp8E0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E0,({const char*_tmp457="format";_tag_fat(_tmp457,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8DF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DF,({const char*_tmp458="__format__";_tag_fat(_tmp458,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp8E2=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E2,({const char*_tmp459="printf";_tag_fat(_tmp459,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8E1=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E1,({const char*_tmp45A="__printf__";_tag_fat(_tmp45A,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->tag=19U,_tmp45B->f1=Cyc_Absyn_Printf_ft,_tmp45B->f2=(int)n,_tmp45B->f3=(int)m;_tmp45B;});else{
if(({struct _fat_ptr _tmp8E4=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E4,({const char*_tmp45C="scanf";_tag_fat(_tmp45C,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp8E3=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E3,({const char*_tmp45D="__scanf__";_tag_fat(_tmp45D,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->tag=19U,_tmp45E->f1=Cyc_Absyn_Scanf_ft,_tmp45E->f2=(int)n,_tmp45E->f3=(int)m;_tmp45E;});else{
# 1621
({void*_tmp45F=0U;({unsigned _tmp8E6=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8E5=({const char*_tmp460="unrecognized format type";_tag_fat(_tmp460,sizeof(char),25U);});Cyc_Warn_err(_tmp8E6,_tmp8E5,_tag_fat(_tmp45F,sizeof(void*),0U));});});}}}else{
# 1623
({void*_tmp461=0U;({unsigned _tmp8E8=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8E7=({const char*_tmp462="unrecognized attribute";_tag_fat(_tmp462,sizeof(char),23U);});Cyc_Warn_err(_tmp8E8,_tmp8E7,_tag_fat(_tmp461,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1626
goto _LL0;}case 83U: _LLA1: _LLA2:
# 1635 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84U: _LLA3: _LLA4:
# 1637 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EA=({struct _tuple0*_tmp8E9=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp8E9,Cyc_yyget_YY40(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp8EA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85U: _LLA5: _LLA6:
# 1641 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1642 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1643 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1644 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89U: _LLAD: _LLAE:
# 1645 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90U: _LLAF: _LLB0:
# 1646 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1647 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1648 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93U: _LLB5: _LLB6:
# 1649 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1650 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95U: _LLB9: _LLBA:
# 1651 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96U: _LLBB: _LLBC:
# 1654
 yyval=Cyc_YY21(({void*_tmp8EB=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8EB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97U: _LLBD: _LLBE:
# 1656 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EC=Cyc_Absyn_builtin_type(({const char*_tmp463="__builtin_va_list";_tag_fat(_tmp463,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp8EC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1658 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99U: _LLC1: _LLC2:
# 1660 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8ED=Cyc_yyget_YY44(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp8ED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100U: _LLC3: _LLC4:
# 1662 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EE=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8EE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1664 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EF=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp8EF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1666 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F2=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->tag=6U,({struct Cyc_List_List*_tmp8F1=({unsigned _tmp8F0=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8F0,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[2]).v)));});
# 1666
_tmp464->f1=_tmp8F1;});_tmp464;});Cyc_Parse_type_spec(_tmp8F2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1669
goto _LL0;case 103U: _LLC9: _LLCA:
# 1670 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F3=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104U: _LLCB: _LLCC:
# 1672 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F4=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp8F4,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1674
goto _LL0;case 105U: _LLCD: _LLCE:
# 1675 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F5=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106U: _LLCF: _LLD0:
# 1677 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F6=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp8F6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107U: _LLD1: _LLD2:
# 1679 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F7=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1685 "parse.y"
 yyval=Cyc_YY43(({struct _fat_ptr _tmp8F8=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id_to_kind(_tmp8F8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6: {
# 1689 "parse.y"
unsigned loc=(unsigned)(Cyc_Absyn_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp731;_tmp731.print_const=1,_tmp731.q_volatile=0,_tmp731.q_restrict=0,_tmp731.real_const=1,_tmp731.loc=loc;_tmp731;}));
goto _LL0;}case 110U: _LLD7: _LLD8:
# 1691 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp732;_tmp732.print_const=0,_tmp732.q_volatile=1,_tmp732.q_restrict=0,_tmp732.real_const=0,_tmp732.loc=0U;_tmp732;}));
goto _LL0;case 111U: _LLD9: _LLDA:
# 1692 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp733;_tmp733.print_const=0,_tmp733.q_volatile=0,_tmp733.q_restrict=1,_tmp733.real_const=0,_tmp733.loc=0U;_tmp733;}));
goto _LL0;case 112U: _LLDB: _LLDC: {
# 1698 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp465=({struct Cyc_Absyn_TypeDecl*_tmp46A=_cycalloc(sizeof(*_tmp46A));({void*_tmp8FD=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8FC=({struct Cyc_Absyn_Enumdecl*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8FB=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp468->name=_tmp8FB;}),({struct Cyc_Core_Opt*_tmp8FA=({struct Cyc_Core_Opt*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_List_List*_tmp8F9=Cyc_yyget_YY48(&(yyyvsp[3]).v);_tmp467->v=_tmp8F9;});_tmp467;});_tmp468->fields=_tmp8FA;});_tmp468;});_tmp469->f1=_tmp8FC;});_tmp469;});_tmp46A->r=_tmp8FD;}),_tmp46A->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46A;});struct Cyc_Absyn_TypeDecl*ed=_tmp465;
# 1700
yyval=Cyc_YY21(({void*_tmp8FE=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->tag=10U,_tmp466->f1=ed,_tmp466->f2=0;_tmp466;});Cyc_Parse_type_spec(_tmp8FE,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1702
goto _LL0;}case 113U: _LLDD: _LLDE:
# 1703 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8FF=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp8FF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114U: _LLDF: _LLE0:
# 1705 "parse.y"
 yyval=Cyc_YY21(({void*_tmp900=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp900,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1711 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct _tuple0*_tmp901=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46B->name=_tmp901;}),_tmp46B->tag=0,_tmp46B->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46B;}));
goto _LL0;case 116U: _LLE3: _LLE4:
# 1713 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46C=_cycalloc(sizeof(*_tmp46C));({struct _tuple0*_tmp903=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46C->name=_tmp903;}),({struct Cyc_Absyn_Exp*_tmp902=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp46C->tag=_tmp902;}),_tmp46C->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46C;}));
goto _LL0;case 117U: _LLE5: _LLE6:
# 1717 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46D=_cycalloc(sizeof(*_tmp46D));({struct Cyc_Absyn_Enumfield*_tmp904=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46D->hd=_tmp904;}),_tmp46D->tl=0;_tmp46D;}));
goto _LL0;case 118U: _LLE7: _LLE8:
# 1718 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct Cyc_Absyn_Enumfield*_tmp905=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46E->hd=_tmp905;}),_tmp46E->tl=0;_tmp46E;}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1719 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*_tmp46F));({struct Cyc_Absyn_Enumfield*_tmp907=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46F->hd=_tmp907;}),({struct Cyc_List_List*_tmp906=Cyc_yyget_YY48(&(yyyvsp[2]).v);_tmp46F->tl=_tmp906;});_tmp46F;}));
goto _LL0;case 120U: _LLEB: _LLEC:
# 1725 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90A=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=7U,({enum Cyc_Absyn_AggrKind _tmp909=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp470->f1=_tmp909;}),({struct Cyc_List_List*_tmp908=Cyc_yyget_YY24(&(yyyvsp[2]).v);_tmp470->f2=_tmp908;});_tmp470;});Cyc_Parse_type_spec(_tmp90A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121U: _LLED: _LLEE: {
# 1731 "parse.y"
struct Cyc_List_List*_tmp471=({unsigned _tmp90B=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp90B,Cyc_yyget_YY40(&(yyyvsp[3]).v));});struct Cyc_List_List*ts=_tmp471;
struct Cyc_List_List*_tmp472=({unsigned _tmp90C=(unsigned)((yyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp90C,Cyc_yyget_YY40(&(yyyvsp[5]).v));});struct Cyc_List_List*exist_ts=_tmp472;
struct Cyc_Absyn_TypeDecl*_tmp473=({enum Cyc_Absyn_AggrKind _tmp912=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp911=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp910=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp90F=({
struct Cyc_List_List*_tmp90E=exist_ts;struct Cyc_List_List*_tmp90D=Cyc_yyget_YY50(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp90E,_tmp90D,Cyc_yyget_YY24(&(yyyvsp[7]).v),1);});
# 1733
Cyc_Absyn_aggr_tdecl(_tmp912,Cyc_Absyn_Public,_tmp911,_tmp910,_tmp90F,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp473;
# 1736
yyval=Cyc_YY21(({void*_tmp913=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->tag=10U,_tmp474->f1=td,_tmp474->f2=0;_tmp474;});Cyc_Parse_type_spec(_tmp913,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1738
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1742 "parse.y"
struct Cyc_List_List*_tmp475=({unsigned _tmp914=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp914,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp475;
struct Cyc_List_List*_tmp476=({unsigned _tmp915=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp915,Cyc_yyget_YY40(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp476;
struct Cyc_Absyn_TypeDecl*_tmp477=({enum Cyc_Absyn_AggrKind _tmp91B=Cyc_yyget_YY22(&(yyyvsp[0]).v);struct _tuple0*_tmp91A=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp919=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp918=({
struct Cyc_List_List*_tmp917=exist_ts;struct Cyc_List_List*_tmp916=Cyc_yyget_YY50(&(yyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp917,_tmp916,Cyc_yyget_YY24(&(yyyvsp[6]).v),0);});
# 1744
Cyc_Absyn_aggr_tdecl(_tmp91B,Cyc_Absyn_Public,_tmp91A,_tmp919,_tmp918,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp477;
# 1747
yyval=Cyc_YY21(({void*_tmp91C=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->tag=10U,_tmp478->f1=td,_tmp478->f2=0;_tmp478;});Cyc_Parse_type_spec(_tmp91C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1749
goto _LL0;}case 123U: _LLF1: _LLF2:
# 1750 "parse.y"
 yyval=Cyc_YY21(({void*_tmp920=({union Cyc_Absyn_AggrInfo _tmp91F=({enum Cyc_Absyn_AggrKind _tmp91E=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp91D=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp91E,_tmp91D,({struct Cyc_Core_Opt*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->v=(void*)1;_tmp479;}));});Cyc_Absyn_aggr_type(_tmp91F,Cyc_yyget_YY40(&(yyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp920,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1753
goto _LL0;case 124U: _LLF3: _LLF4:
# 1754 "parse.y"
 yyval=Cyc_YY21(({void*_tmp923=({union Cyc_Absyn_AggrInfo _tmp922=({enum Cyc_Absyn_AggrKind _tmp921=Cyc_yyget_YY22(&(yyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp921,Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp922,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp923,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 125U: _LLF5: _LLF6:
# 1759 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 126U: _LLF7: _LLF8:
# 1761 "parse.y"
 yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 127U: _LLF9: _LLFA:
# 1765 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 128U: _LLFB: _LLFC:
# 1766 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 129U: _LLFD: _LLFE:
# 1771 "parse.y"
 yyval=Cyc_YY24(0);
goto _LL0;case 130U: _LLFF: _LL100: {
# 1773 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp47A=Cyc_yyget_YY25(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp47A;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1777
struct Cyc_List_List*_tmp47B=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp47B;
if(tags != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY24(decls);
# 1782
goto _LL0;}}case 131U: _LL101: _LL102:
# 1787 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct Cyc_List_List*_tmp924=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp47C->hd=_tmp924;}),_tmp47C->tl=0;_tmp47C;}));
goto _LL0;case 132U: _LL103: _LL104:
# 1789 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct Cyc_List_List*_tmp926=Cyc_yyget_YY24(&(yyyvsp[1]).v);_tmp47D->hd=_tmp926;}),({struct Cyc_List_List*_tmp925=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp47D->tl=_tmp925;});_tmp47D;}));
goto _LL0;case 133U: _LL105: _LL106:
# 1793 "parse.y"
 yyval=Cyc_YY19(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[0]).v)));
goto _LL0;case 134U: _LL107: _LL108:
# 1799 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp47E=_region_malloc(yyr,sizeof(*_tmp47E));_tmp47E->tl=0,({struct _tuple12 _tmp927=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp47E->hd=_tmp927;});_tmp47E;}));
goto _LL0;case 135U: _LL109: _LL10A:
# 1801 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp47F=_region_malloc(yyr,sizeof(*_tmp47F));({struct _tuple13*_tmp929=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp47F->tl=_tmp929;}),({struct _tuple12 _tmp928=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp47F->hd=_tmp928;});_tmp47F;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1806 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp734;({struct Cyc_Parse_Declarator _tmp92A=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp734.f1=_tmp92A;}),_tmp734.f2=0;_tmp734;}));
goto _LL0;case 137U: _LL10D: _LL10E:
# 1808 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp735;({struct Cyc_Parse_Declarator _tmp92C=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp735.f1=_tmp92C;}),({struct Cyc_Absyn_Exp*_tmp92B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp735.f2=_tmp92B;});_tmp735;}));
goto _LL0;case 138U: _LL10F: _LL110: {
# 1814 "parse.y"
struct _RegionHandle _tmp480=_new_region("temp");struct _RegionHandle*temp=& _tmp480;_push_region(temp);
{struct _tuple26 _tmp481=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp19=_tmp481;struct _tuple26 _tmp482=_stmttmp19;struct Cyc_List_List*_tmp485;struct Cyc_Parse_Type_specifier _tmp484;struct Cyc_Absyn_Tqual _tmp483;_LL463: _tmp483=_tmp482.f1;_tmp484=_tmp482.f2;_tmp485=_tmp482.f3;_LL464: {struct Cyc_Absyn_Tqual tq=_tmp483;struct Cyc_Parse_Type_specifier tspecs=_tmp484;struct Cyc_List_List*atts=_tmp485;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple11*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp486=Cyc_yyget_YY29(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp486;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp487=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp1A=_tmp487;struct _tuple25*_tmp488=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Parse_Declarator _tmp489;_LL466: _tmp489=_tmp488->f1;_tmp48A=_tmp488->f2;_tmp48B=_tmp488->f3;_LL467: {struct Cyc_Parse_Declarator d=_tmp489;struct Cyc_Absyn_Exp*wd=_tmp48A;struct Cyc_Absyn_Exp*wh=_tmp48B;
decls=({struct _tuple11*_tmp48C=_region_malloc(temp,sizeof(*_tmp48C));_tmp48C->tl=decls,_tmp48C->hd=d;_tmp48C;});
widths_and_reqs=({struct Cyc_List_List*_tmp48E=_region_malloc(temp,sizeof(*_tmp48E));
({struct _tuple17*_tmp92D=({struct _tuple17*_tmp48D=_region_malloc(temp,sizeof(*_tmp48D));_tmp48D->f1=wd,_tmp48D->f2=wh;_tmp48D;});_tmp48E->hd=_tmp92D;}),_tmp48E->tl=widths_and_reqs;_tmp48E;});}}}
# 1825
decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp48F=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp48F;
struct Cyc_List_List*_tmp490=({struct _RegionHandle*_tmp930=temp;struct _RegionHandle*_tmp92F=temp;struct Cyc_List_List*_tmp92E=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1828
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp930,_tmp92F,_tmp92E,widths_and_reqs);});struct Cyc_List_List*info=_tmp490;
# 1831
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple18*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1833
_npop_handler(0U);goto _LL0;}}}}
# 1815
;_pop_region(temp);}case 139U: _LL111: _LL112:
# 1841 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp736;({struct Cyc_Absyn_Tqual _tmp932=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp736.f1=_tmp932;}),({struct Cyc_Parse_Type_specifier _tmp931=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp736.f2=_tmp931;}),_tmp736.f3=0;_tmp736;}));
goto _LL0;case 140U: _LL113: _LL114: {
# 1843 "parse.y"
struct _tuple26 _tmp491=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp491;yyval=Cyc_YY35(({struct _tuple26 _tmp737;_tmp737.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp935=({unsigned _tmp934=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp933=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp934,_tmp933,two.f2);});_tmp737.f2=_tmp935;}),_tmp737.f3=two.f3;_tmp737;}));
goto _LL0;}case 141U: _LL115: _LL116:
# 1845 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp738;({struct Cyc_Absyn_Tqual _tmp937=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp738.f1=_tmp937;}),({struct Cyc_Parse_Type_specifier _tmp936=Cyc_Parse_empty_spec(0U);_tmp738.f2=_tmp936;}),_tmp738.f3=0;_tmp738;}));
goto _LL0;case 142U: _LL117: _LL118: {
# 1847 "parse.y"
struct _tuple26 _tmp492=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp492;
yyval=Cyc_YY35(({struct _tuple26 _tmp739;({struct Cyc_Absyn_Tqual _tmp939=({struct Cyc_Absyn_Tqual _tmp938=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp938,two.f1);});_tmp739.f1=_tmp939;}),_tmp739.f2=two.f2,_tmp739.f3=two.f3;_tmp739;}));
goto _LL0;}case 143U: _LL119: _LL11A:
# 1850 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73A;({struct Cyc_Absyn_Tqual _tmp93C=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73A.f1=_tmp93C;}),({struct Cyc_Parse_Type_specifier _tmp93B=Cyc_Parse_empty_spec(0U);_tmp73A.f2=_tmp93B;}),({struct Cyc_List_List*_tmp93A=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp73A.f3=_tmp93A;});_tmp73A;}));
goto _LL0;case 144U: _LL11B: _LL11C: {
# 1852 "parse.y"
struct _tuple26 _tmp493=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp493;yyval=Cyc_YY35(({struct _tuple26 _tmp73B;_tmp73B.f1=two.f1,_tmp73B.f2=two.f2,({struct Cyc_List_List*_tmp93E=({struct Cyc_List_List*_tmp93D=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp93D,two.f3);});_tmp73B.f3=_tmp93E;});_tmp73B;}));
goto _LL0;}case 145U: _LL11D: _LL11E:
# 1858 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73C;({struct Cyc_Absyn_Tqual _tmp940=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73C.f1=_tmp940;}),({struct Cyc_Parse_Type_specifier _tmp93F=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp73C.f2=_tmp93F;}),_tmp73C.f3=0;_tmp73C;}));
goto _LL0;case 146U: _LL11F: _LL120: {
# 1860 "parse.y"
struct _tuple26 _tmp494=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp494;yyval=Cyc_YY35(({struct _tuple26 _tmp73D;_tmp73D.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp943=({unsigned _tmp942=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp941=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp942,_tmp941,two.f2);});_tmp73D.f2=_tmp943;}),_tmp73D.f3=two.f3;_tmp73D;}));
goto _LL0;}case 147U: _LL121: _LL122:
# 1862 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73E;({struct Cyc_Absyn_Tqual _tmp945=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp73E.f1=_tmp945;}),({struct Cyc_Parse_Type_specifier _tmp944=Cyc_Parse_empty_spec(0U);_tmp73E.f2=_tmp944;}),_tmp73E.f3=0;_tmp73E;}));
goto _LL0;case 148U: _LL123: _LL124: {
# 1864 "parse.y"
struct _tuple26 _tmp495=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp495;
yyval=Cyc_YY35(({struct _tuple26 _tmp73F;({struct Cyc_Absyn_Tqual _tmp947=({struct Cyc_Absyn_Tqual _tmp946=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp946,two.f1);});_tmp73F.f1=_tmp947;}),_tmp73F.f2=two.f2,_tmp73F.f3=two.f3;_tmp73F;}));
goto _LL0;}case 149U: _LL125: _LL126:
# 1867 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp740;({struct Cyc_Absyn_Tqual _tmp94A=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp740.f1=_tmp94A;}),({struct Cyc_Parse_Type_specifier _tmp949=Cyc_Parse_empty_spec(0U);_tmp740.f2=_tmp949;}),({struct Cyc_List_List*_tmp948=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp740.f3=_tmp948;});_tmp740;}));
goto _LL0;case 150U: _LL127: _LL128: {
# 1869 "parse.y"
struct _tuple26 _tmp496=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp496;yyval=Cyc_YY35(({struct _tuple26 _tmp741;_tmp741.f1=two.f1,_tmp741.f2=two.f2,({struct Cyc_List_List*_tmp94C=({struct Cyc_List_List*_tmp94B=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp94B,two.f3);});_tmp741.f3=_tmp94C;});_tmp741;}));
goto _LL0;}case 151U: _LL129: _LL12A:
# 1873 "parse.y"
 yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyvsp[0]).v)));
goto _LL0;case 152U: _LL12B: _LL12C:
# 1879 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp497=_region_malloc(yyr,sizeof(*_tmp497));({struct _tuple25*_tmp94D=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp497->hd=_tmp94D;}),_tmp497->tl=0;_tmp497;}));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1881 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp498=_region_malloc(yyr,sizeof(*_tmp498));({struct _tuple25*_tmp94F=Cyc_yyget_YY28(&(yyyvsp[2]).v);_tmp498->hd=_tmp94F;}),({struct Cyc_List_List*_tmp94E=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp498->tl=_tmp94E;});_tmp498;}));
goto _LL0;case 154U: _LL12F: _LL130:
# 1886 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp499=_region_malloc(yyr,sizeof(*_tmp499));({struct Cyc_Parse_Declarator _tmp951=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp499->f1=_tmp951;}),_tmp499->f2=0,({struct Cyc_Absyn_Exp*_tmp950=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp499->f3=_tmp950;});_tmp499;}));
goto _LL0;case 155U: _LL131: _LL132:
# 1890 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49D=_region_malloc(yyr,sizeof(*_tmp49D));({struct _tuple0*_tmp956=({struct _tuple0*_tmp49C=_cycalloc(sizeof(*_tmp49C));({union Cyc_Absyn_Nmspace _tmp955=Cyc_Absyn_Rel_n(0);_tmp49C->f1=_tmp955;}),({struct _fat_ptr*_tmp954=({struct _fat_ptr*_tmp49B=_cycalloc(sizeof(*_tmp49B));({struct _fat_ptr _tmp953=({const char*_tmp49A="";_tag_fat(_tmp49A,sizeof(char),1U);});*_tmp49B=_tmp953;});_tmp49B;});_tmp49C->f2=_tmp954;});_tmp49C;});(_tmp49D->f1).id=_tmp956;}),(_tmp49D->f1).varloc=0U,(_tmp49D->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp952=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp49D->f2=_tmp952;}),_tmp49D->f3=0;_tmp49D;}));
# 1892
goto _LL0;case 156U: _LL133: _LL134:
# 1895 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A1=_region_malloc(yyr,sizeof(*_tmp4A1));({struct _tuple0*_tmp95A=({struct _tuple0*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));({union Cyc_Absyn_Nmspace _tmp959=Cyc_Absyn_Rel_n(0);_tmp4A0->f1=_tmp959;}),({struct _fat_ptr*_tmp958=({struct _fat_ptr*_tmp49F=_cycalloc(sizeof(*_tmp49F));({struct _fat_ptr _tmp957=({const char*_tmp49E="";_tag_fat(_tmp49E,sizeof(char),1U);});*_tmp49F=_tmp957;});_tmp49F;});_tmp4A0->f2=_tmp958;});_tmp4A0;});(_tmp4A1->f1).id=_tmp95A;}),(_tmp4A1->f1).varloc=0U,(_tmp4A1->f1).tms=0,_tmp4A1->f2=0,_tmp4A1->f3=0;_tmp4A1;}));
# 1897
goto _LL0;case 157U: _LL135: _LL136:
# 1898 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A2=_region_malloc(yyr,sizeof(*_tmp4A2));({struct Cyc_Parse_Declarator _tmp95C=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp4A2->f1=_tmp95C;}),({struct Cyc_Absyn_Exp*_tmp95B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4A2->f2=_tmp95B;}),_tmp4A2->f3=0;_tmp4A2;}));
goto _LL0;case 158U: _LL137: _LL138:
# 1902 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 159U: _LL139: _LL13A:
# 1903 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 160U: _LL13B: _LL13C:
# 1907 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 161U: _LL13D: _LL13E:
# 1908 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 162U: _LL13F: _LL140: {
# 1914 "parse.y"
int _tmp4A3=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A3;
struct Cyc_List_List*_tmp4A4=({unsigned _tmp95D=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp95D,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp4A4;
struct Cyc_Absyn_TypeDecl*_tmp4A5=({struct _tuple0*_tmp962=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp961=ts;struct Cyc_Core_Opt*_tmp960=({struct Cyc_Core_Opt*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));({struct Cyc_List_List*_tmp95E=Cyc_yyget_YY34(&(yyyvsp[4]).v);_tmp4A7->v=_tmp95E;});_tmp4A7;});int _tmp95F=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp962,_tmp961,_tmp960,_tmp95F,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp4A5;
# 1918
yyval=Cyc_YY21(({void*_tmp963=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->tag=10U,_tmp4A6->f1=dd,_tmp4A6->f2=0;_tmp4A6;});Cyc_Parse_type_spec(_tmp963,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1920
goto _LL0;}case 163U: _LL141: _LL142: {
# 1921 "parse.y"
int _tmp4A8=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A8;
yyval=Cyc_YY21(({void*_tmp966=({union Cyc_Absyn_DatatypeInfo _tmp965=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp742;({struct _tuple0*_tmp964=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp742.name=_tmp964;}),_tmp742.is_extensible=is_extensible;_tmp742;}));Cyc_Absyn_datatype_type(_tmp965,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp966,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1924
goto _LL0;}case 164U: _LL143: _LL144: {
# 1925 "parse.y"
int _tmp4A9=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A9;
yyval=Cyc_YY21(({void*_tmp96A=({union Cyc_Absyn_DatatypeFieldInfo _tmp969=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp743;({struct _tuple0*_tmp968=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp743.datatype_name=_tmp968;}),({struct _tuple0*_tmp967=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp743.field_name=_tmp967;}),_tmp743.is_extensible=is_extensible;_tmp743;}));Cyc_Absyn_datatype_field_type(_tmp969,Cyc_yyget_YY40(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp96A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1928
goto _LL0;}case 165U: _LL145: _LL146:
# 1931 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 166U: _LL147: _LL148:
# 1932 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 167U: _LL149: _LL14A:
# 1936 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));({struct Cyc_Absyn_Datatypefield*_tmp96B=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AA->hd=_tmp96B;}),_tmp4AA->tl=0;_tmp4AA;}));
goto _LL0;case 168U: _LL14B: _LL14C:
# 1937 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct Cyc_Absyn_Datatypefield*_tmp96C=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AB->hd=_tmp96C;}),_tmp4AB->tl=0;_tmp4AB;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1938 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));({struct Cyc_Absyn_Datatypefield*_tmp96E=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AC->hd=_tmp96E;}),({struct Cyc_List_List*_tmp96D=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AC->tl=_tmp96D;});_tmp4AC;}));
goto _LL0;case 170U: _LL14F: _LL150:
# 1939 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct Cyc_Absyn_Datatypefield*_tmp970=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AD->hd=_tmp970;}),({struct Cyc_List_List*_tmp96F=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AD->tl=_tmp96F;});_tmp4AD;}));
goto _LL0;case 171U: _LL151: _LL152:
# 1943 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 172U: _LL153: _LL154:
# 1944 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 173U: _LL155: _LL156:
# 1945 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 174U: _LL157: _LL158:
# 1949 "parse.y"
 yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));({struct _tuple0*_tmp972=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4AE->name=_tmp972;}),_tmp4AE->typs=0,_tmp4AE->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp971=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4AE->sc=_tmp971;});_tmp4AE;}));
goto _LL0;case 175U: _LL159: _LL15A: {
# 1951 "parse.y"
struct Cyc_List_List*_tmp4AF=({unsigned _tmp973=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp973,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp4AF;
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));({struct _tuple0*_tmp975=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4B0->name=_tmp975;}),_tmp4B0->typs=typs,_tmp4B0->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp974=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4B0->sc=_tmp974;});_tmp4B0;}));
goto _LL0;}case 176U: _LL15B: _LL15C:
# 1957 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 177U: _LL15D: _LL15E: {
# 1959 "parse.y"
struct Cyc_Parse_Declarator _tmp4B1=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B1;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp744;_tmp744.id=two.id,_tmp744.varloc=two.varloc,({struct Cyc_List_List*_tmp977=({struct Cyc_List_List*_tmp976=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp976,two.tms);});_tmp744.tms=_tmp977;});_tmp744;}));
goto _LL0;}case 178U: _LL15F: _LL160:
# 1966 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 179U: _LL161: _LL162: {
# 1968 "parse.y"
struct Cyc_Parse_Declarator _tmp4B2=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B2;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp745;_tmp745.id=two.id,_tmp745.varloc=two.varloc,({struct Cyc_List_List*_tmp979=({struct Cyc_List_List*_tmp978=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp978,two.tms);});_tmp745.tms=_tmp979;});_tmp745;}));
goto _LL0;}case 180U: _LL163: _LL164:
# 1974 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;({struct _tuple0*_tmp97A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp746.id=_tmp97A;}),_tmp746.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp746.tms=0;_tmp746;}));
goto _LL0;case 181U: _LL165: _LL166:
# 1976 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 182U: _LL167: _LL168: {
# 1980 "parse.y"
struct Cyc_Parse_Declarator _tmp4B3=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4B3;
({struct Cyc_List_List*_tmp97D=({struct Cyc_List_List*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));({void*_tmp97C=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B4=_region_malloc(yyr,sizeof(*_tmp4B4));_tmp4B4->tag=5U,_tmp4B4->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp97B=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4B4->f2=_tmp97B;});_tmp4B4;});_tmp4B5->hd=_tmp97C;}),_tmp4B5->tl=d.tms;_tmp4B5;});d.tms=_tmp97D;});
yyval=(yyyvsp[2]).v;
# 1984
goto _LL0;}case 183U: _LL169: _LL16A:
# 1985 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;({struct _tuple0*_tmp983=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp747.id=_tmp983;}),({unsigned _tmp982=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp747.varloc=_tmp982;}),({struct Cyc_List_List*_tmp981=({struct Cyc_List_List*_tmp4B7=_region_malloc(yyr,sizeof(*_tmp4B7));({void*_tmp980=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4B6=_region_malloc(yyr,sizeof(*_tmp4B6));_tmp4B6->tag=0U,({void*_tmp97F=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4B6->f1=_tmp97F;}),_tmp4B6->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4B6;});_tmp4B7->hd=_tmp980;}),({struct Cyc_List_List*_tmp97E=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B7->tl=_tmp97E;});_tmp4B7;});_tmp747.tms=_tmp981;});_tmp747;}));
goto _LL0;case 184U: _LL16B: _LL16C:
# 1987 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;({struct _tuple0*_tmp98A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp748.id=_tmp98A;}),({unsigned _tmp989=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp748.varloc=_tmp989;}),({
struct Cyc_List_List*_tmp988=({struct Cyc_List_List*_tmp4B9=_region_malloc(yyr,sizeof(*_tmp4B9));({void*_tmp987=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4B8=_region_malloc(yyr,sizeof(*_tmp4B8));_tmp4B8->tag=1U,({struct Cyc_Absyn_Exp*_tmp986=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4B8->f1=_tmp986;}),({void*_tmp985=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4B8->f2=_tmp985;}),_tmp4B8->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4B8;});_tmp4B9->hd=_tmp987;}),({struct Cyc_List_List*_tmp984=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B9->tl=_tmp984;});_tmp4B9;});_tmp748.tms=_tmp988;});_tmp748;}));
goto _LL0;case 185U: _LL16D: _LL16E: {
# 1990 "parse.y"
struct _tuple27*_tmp4BA=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp1B=_tmp4BA;struct _tuple27*_tmp4BB=_stmttmp1B;struct Cyc_List_List*_tmp4C0;void*_tmp4BF;struct Cyc_Absyn_VarargInfo*_tmp4BE;int _tmp4BD;struct Cyc_List_List*_tmp4BC;_LL469: _tmp4BC=_tmp4BB->f1;_tmp4BD=_tmp4BB->f2;_tmp4BE=_tmp4BB->f3;_tmp4BF=_tmp4BB->f4;_tmp4C0=_tmp4BB->f5;_LL46A: {struct Cyc_List_List*lis=_tmp4BC;int b=_tmp4BD;struct Cyc_Absyn_VarargInfo*c=_tmp4BE;void*eff=_tmp4BF;struct Cyc_List_List*po=_tmp4C0;
struct Cyc_Absyn_Exp*_tmp4C1=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4C1;
struct Cyc_Absyn_Exp*_tmp4C2=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4C2;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;({struct _tuple0*_tmp990=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp749.id=_tmp990;}),({unsigned _tmp98F=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp749.varloc=_tmp98F;}),({struct Cyc_List_List*_tmp98E=({struct Cyc_List_List*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));({void*_tmp98D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));_tmp4C4->tag=3U,({void*_tmp98C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));_tmp4C3->tag=1U,_tmp4C3->f1=lis,_tmp4C3->f2=b,_tmp4C3->f3=c,_tmp4C3->f4=eff,_tmp4C3->f5=po,_tmp4C3->f6=req,_tmp4C3->f7=ens;_tmp4C3;});_tmp4C4->f1=_tmp98C;});_tmp4C4;});_tmp4C5->hd=_tmp98D;}),({struct Cyc_List_List*_tmp98B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C5->tl=_tmp98B;});_tmp4C5;});_tmp749.tms=_tmp98E;});_tmp749;}));
# 1995
goto _LL0;}}case 186U: _LL16F: _LL170:
# 1996 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;({struct _tuple0*_tmp99A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74A.id=_tmp99A;}),({unsigned _tmp999=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74A.varloc=_tmp999;}),({
struct Cyc_List_List*_tmp998=({struct Cyc_List_List*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));({void*_tmp997=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));_tmp4C7->tag=3U,({void*_tmp996=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));_tmp4C6->tag=1U,_tmp4C6->f1=0,_tmp4C6->f2=0,_tmp4C6->f3=0,({
# 1999
void*_tmp995=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4C6->f4=_tmp995;}),({struct Cyc_List_List*_tmp994=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4C6->f5=_tmp994;}),({struct Cyc_Absyn_Exp*_tmp993=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4C6->f6=_tmp993;}),({struct Cyc_Absyn_Exp*_tmp992=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4C6->f7=_tmp992;});_tmp4C6;});
# 1997
_tmp4C7->f1=_tmp996;});_tmp4C7;});_tmp4C8->hd=_tmp997;}),({
# 2000
struct Cyc_List_List*_tmp991=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C8->tl=_tmp991;});_tmp4C8;});
# 1997
_tmp74A.tms=_tmp998;});_tmp74A;}));
# 2002
goto _LL0;case 187U: _LL171: _LL172:
# 2003 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74B;({struct _tuple0*_tmp9A1=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74B.id=_tmp9A1;}),({unsigned _tmp9A0=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74B.varloc=_tmp9A0;}),({struct Cyc_List_List*_tmp99F=({struct Cyc_List_List*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));({void*_tmp99E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=3U,({void*_tmp99D=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));_tmp4C9->tag=0U,({struct Cyc_List_List*_tmp99C=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4C9->f1=_tmp99C;}),_tmp4C9->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4C9;});_tmp4CA->f1=_tmp99D;});_tmp4CA;});_tmp4CB->hd=_tmp99E;}),({struct Cyc_List_List*_tmp99B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CB->tl=_tmp99B;});_tmp4CB;});_tmp74B.tms=_tmp99F;});_tmp74B;}));
goto _LL0;case 188U: _LL173: _LL174: {
# 2006
struct Cyc_List_List*_tmp4CC=({unsigned _tmp9A2=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9A2,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4CC;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74C;({struct _tuple0*_tmp9A7=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74C.id=_tmp9A7;}),({unsigned _tmp9A6=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74C.varloc=_tmp9A6;}),({struct Cyc_List_List*_tmp9A5=({struct Cyc_List_List*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));({void*_tmp9A4=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));_tmp4CD->tag=4U,_tmp4CD->f1=ts,_tmp4CD->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4CD->f3=0;_tmp4CD;});_tmp4CE->hd=_tmp9A4;}),({struct Cyc_List_List*_tmp9A3=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CE->tl=_tmp9A3;});_tmp4CE;});_tmp74C.tms=_tmp9A5;});_tmp74C;}));
# 2009
goto _LL0;}case 189U: _LL175: _LL176:
# 2010 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74D;({struct _tuple0*_tmp9AD=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74D.id=_tmp9AD;}),({unsigned _tmp9AC=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74D.varloc=_tmp9AC;}),({struct Cyc_List_List*_tmp9AB=({struct Cyc_List_List*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));({void*_tmp9AA=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));_tmp4CF->tag=5U,_tmp4CF->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9A9=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4CF->f2=_tmp9A9;});_tmp4CF;});_tmp4D0->hd=_tmp9AA;}),({
struct Cyc_List_List*_tmp9A8=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4D0->tl=_tmp9A8;});_tmp4D0;});
# 2010
_tmp74D.tms=_tmp9AB;});_tmp74D;}));
# 2013
goto _LL0;case 190U: _LL177: _LL178:
# 2018 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74E;({struct _tuple0*_tmp9AE=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp74E.id=_tmp9AE;}),_tmp74E.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp74E.tms=0;_tmp74E;}));
goto _LL0;case 191U: _LL179: _LL17A:
# 2020 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74F;({struct _tuple0*_tmp9AF=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp74F.id=_tmp9AF;}),_tmp74F.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp74F.tms=0;_tmp74F;}));
goto _LL0;case 192U: _LL17B: _LL17C:
# 2022 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 193U: _LL17D: _LL17E: {
# 2026 "parse.y"
struct Cyc_Parse_Declarator _tmp4D1=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4D1;
({struct Cyc_List_List*_tmp9B2=({struct Cyc_List_List*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));({void*_tmp9B1=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D2=_region_malloc(yyr,sizeof(*_tmp4D2));_tmp4D2->tag=5U,_tmp4D2->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9B0=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D2->f2=_tmp9B0;});_tmp4D2;});_tmp4D3->hd=_tmp9B1;}),_tmp4D3->tl=d.tms;_tmp4D3;});d.tms=_tmp9B2;});
yyval=(yyyvsp[2]).v;
# 2030
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2031 "parse.y"
struct Cyc_Parse_Declarator _tmp4D4=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D4;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp750;_tmp750.id=one.id,_tmp750.varloc=one.varloc,({
struct Cyc_List_List*_tmp9B5=({struct Cyc_List_List*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));({void*_tmp9B4=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));_tmp4D5->tag=0U,({void*_tmp9B3=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4D5->f1=_tmp9B3;}),_tmp4D5->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4D5;});_tmp4D6->hd=_tmp9B4;}),_tmp4D6->tl=one.tms;_tmp4D6;});_tmp750.tms=_tmp9B5;});_tmp750;}));
goto _LL0;}case 195U: _LL181: _LL182: {
# 2035 "parse.y"
struct Cyc_Parse_Declarator _tmp4D7=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D7;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp751;_tmp751.id=one.id,_tmp751.varloc=one.varloc,({
struct Cyc_List_List*_tmp9B9=({struct Cyc_List_List*_tmp4D9=_region_malloc(yyr,sizeof(*_tmp4D9));({void*_tmp9B8=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D8=_region_malloc(yyr,sizeof(*_tmp4D8));_tmp4D8->tag=1U,({struct Cyc_Absyn_Exp*_tmp9B7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4D8->f1=_tmp9B7;}),({void*_tmp9B6=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4D8->f2=_tmp9B6;}),_tmp4D8->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4D8;});_tmp4D9->hd=_tmp9B8;}),_tmp4D9->tl=one.tms;_tmp4D9;});_tmp751.tms=_tmp9B9;});_tmp751;}));
# 2039
goto _LL0;}case 196U: _LL183: _LL184: {
# 2040 "parse.y"
struct _tuple27*_tmp4DA=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp1C=_tmp4DA;struct _tuple27*_tmp4DB=_stmttmp1C;struct Cyc_List_List*_tmp4E0;void*_tmp4DF;struct Cyc_Absyn_VarargInfo*_tmp4DE;int _tmp4DD;struct Cyc_List_List*_tmp4DC;_LL46C: _tmp4DC=_tmp4DB->f1;_tmp4DD=_tmp4DB->f2;_tmp4DE=_tmp4DB->f3;_tmp4DF=_tmp4DB->f4;_tmp4E0=_tmp4DB->f5;_LL46D: {struct Cyc_List_List*lis=_tmp4DC;int b=_tmp4DD;struct Cyc_Absyn_VarargInfo*c=_tmp4DE;void*eff=_tmp4DF;struct Cyc_List_List*po=_tmp4E0;
struct Cyc_Absyn_Exp*_tmp4E1=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4E1;
struct Cyc_Absyn_Exp*_tmp4E2=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4E2;
struct Cyc_Parse_Declarator _tmp4E3=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp752;_tmp752.id=one.id,_tmp752.varloc=one.varloc,({struct Cyc_List_List*_tmp9BC=({struct Cyc_List_List*_tmp4E6=_region_malloc(yyr,sizeof(*_tmp4E6));({void*_tmp9BB=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));_tmp4E5->tag=3U,({void*_tmp9BA=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));_tmp4E4->tag=1U,_tmp4E4->f1=lis,_tmp4E4->f2=b,_tmp4E4->f3=c,_tmp4E4->f4=eff,_tmp4E4->f5=po,_tmp4E4->f6=req,_tmp4E4->f7=ens;_tmp4E4;});_tmp4E5->f1=_tmp9BA;});_tmp4E5;});_tmp4E6->hd=_tmp9BB;}),_tmp4E6->tl=one.tms;_tmp4E6;});_tmp752.tms=_tmp9BC;});_tmp752;}));
# 2046
goto _LL0;}}case 197U: _LL185: _LL186: {
# 2047 "parse.y"
struct Cyc_Parse_Declarator _tmp4E7=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E7;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp753;_tmp753.id=one.id,_tmp753.varloc=one.varloc,({
struct Cyc_List_List*_tmp9C3=({struct Cyc_List_List*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));({void*_tmp9C2=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E9=_region_malloc(yyr,sizeof(*_tmp4E9));_tmp4E9->tag=3U,({void*_tmp9C1=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E8=_region_malloc(yyr,sizeof(*_tmp4E8));_tmp4E8->tag=1U,_tmp4E8->f1=0,_tmp4E8->f2=0,_tmp4E8->f3=0,({
# 2051
void*_tmp9C0=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4E8->f4=_tmp9C0;}),({struct Cyc_List_List*_tmp9BF=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4E8->f5=_tmp9BF;}),({struct Cyc_Absyn_Exp*_tmp9BE=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4E8->f6=_tmp9BE;}),({struct Cyc_Absyn_Exp*_tmp9BD=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4E8->f7=_tmp9BD;});_tmp4E8;});
# 2049
_tmp4E9->f1=_tmp9C1;});_tmp4E9;});_tmp4EA->hd=_tmp9C2;}),_tmp4EA->tl=one.tms;_tmp4EA;});_tmp753.tms=_tmp9C3;});_tmp753;}));
# 2054
goto _LL0;}case 198U: _LL187: _LL188: {
# 2055 "parse.y"
struct Cyc_Parse_Declarator _tmp4EB=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EB;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp754;_tmp754.id=one.id,_tmp754.varloc=one.varloc,({struct Cyc_List_List*_tmp9C7=({struct Cyc_List_List*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));({void*_tmp9C6=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));_tmp4ED->tag=3U,({void*_tmp9C5=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4EC=_region_malloc(yyr,sizeof(*_tmp4EC));_tmp4EC->tag=0U,({struct Cyc_List_List*_tmp9C4=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4EC->f1=_tmp9C4;}),_tmp4EC->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4EC;});_tmp4ED->f1=_tmp9C5;});_tmp4ED;});_tmp4EE->hd=_tmp9C6;}),_tmp4EE->tl=one.tms;_tmp4EE;});_tmp754.tms=_tmp9C7;});_tmp754;}));
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2059
struct Cyc_List_List*_tmp4EF=({unsigned _tmp9C8=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9C8,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4EF;
struct Cyc_Parse_Declarator _tmp4F0=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F0;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp755;_tmp755.id=one.id,_tmp755.varloc=one.varloc,({struct Cyc_List_List*_tmp9CA=({struct Cyc_List_List*_tmp4F2=_region_malloc(yyr,sizeof(*_tmp4F2));({void*_tmp9C9=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));_tmp4F1->tag=4U,_tmp4F1->f1=ts,_tmp4F1->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4F1->f3=0;_tmp4F1;});_tmp4F2->hd=_tmp9C9;}),_tmp4F2->tl=one.tms;_tmp4F2;});_tmp755.tms=_tmp9CA;});_tmp755;}));
# 2063
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2064 "parse.y"
struct Cyc_Parse_Declarator _tmp4F3=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp756;_tmp756.id=one.id,_tmp756.varloc=one.varloc,({struct Cyc_List_List*_tmp9CD=({struct Cyc_List_List*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));({void*_tmp9CC=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));_tmp4F4->tag=5U,_tmp4F4->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9CB=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4F4->f2=_tmp9CB;});_tmp4F4;});_tmp4F5->hd=_tmp9CC;}),_tmp4F5->tl=one.tms;_tmp4F5;});_tmp756.tms=_tmp9CD;});_tmp756;}));
# 2067
goto _LL0;}case 201U: _LL18D: _LL18E:
# 2071 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 202U: _LL18F: _LL190:
# 2072 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9CE=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9CE,Cyc_yyget_YY26(&(yyyvsp[1]).v));}));
goto _LL0;case 203U: _LL191: _LL192: {
# 2076 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4F7=_region_malloc(yyr,sizeof(*_tmp4F7));({void*_tmp9D0=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F6=_region_malloc(yyr,sizeof(*_tmp4F6));_tmp4F6->tag=5U,_tmp4F6->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp9CF=Cyc_yyget_YY45(&(yyyvsp[3]).v);_tmp4F6->f2=_tmp9CF;});_tmp4F6;});_tmp4F7->hd=_tmp9D0;}),_tmp4F7->tl=ans;_tmp4F7;});{
# 2080
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp4F8=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1D=_tmp4F8;struct _tuple22 _tmp4F9=_stmttmp1D;void*_tmp4FC;void*_tmp4FB;unsigned _tmp4FA;_LL46F: _tmp4FA=_tmp4F9.f1;_tmp4FB=_tmp4F9.f2;_tmp4FC=_tmp4F9.f3;_LL470: {unsigned ploc=_tmp4FA;void*nullable=_tmp4FB;void*bound=_tmp4FC;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));_tmp4FD->ptr_loc=ploc,_tmp4FD->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp4FD->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp4FD;});{
# 2085
struct _tuple15 _tmp4FE=({unsigned _tmp9D4=ploc;void*_tmp9D3=nullable;void*_tmp9D2=bound;void*_tmp9D1=Cyc_yyget_YY44(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp9D4,_tmp9D3,_tmp9D2,_tmp9D1,Cyc_yyget_YY56(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1E=_tmp4FE;struct _tuple15 _tmp4FF=_stmttmp1E;void*_tmp503;void*_tmp502;void*_tmp501;void*_tmp500;_LL472: _tmp500=_tmp4FF.f1;_tmp501=_tmp4FF.f2;_tmp502=_tmp4FF.f3;_tmp503=_tmp4FF.f4;_LL473: {void*nullable=_tmp500;void*bound=_tmp501;void*zeroterm=_tmp502;void*rgn_opt=_tmp503;
ans=({struct Cyc_List_List*_tmp505=_region_malloc(yyr,sizeof(*_tmp505));({void*_tmp9D6=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp504=_region_malloc(yyr,sizeof(*_tmp504));_tmp504->tag=2U,(_tmp504->f1).rgn=rgn_opt,(_tmp504->f1).nullable=nullable,(_tmp504->f1).bounds=bound,(_tmp504->f1).zero_term=zeroterm,(_tmp504->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp9D5=Cyc_yyget_YY23(&(yyyvsp[4]).v);_tmp504->f2=_tmp9D5;});_tmp504;});_tmp505->hd=_tmp9D6;}),_tmp505->tl=ans;_tmp505;});
yyval=Cyc_YY26(ans);
# 2089
goto _LL0;}}}}}case 204U: _LL193: _LL194:
# 2091
 yyval=Cyc_YY56(0);
goto _LL0;case 205U: _LL195: _LL196:
# 2092 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));({void*_tmp9D8=Cyc_yyget_YY55(&(yyyvsp[0]).v);_tmp506->hd=_tmp9D8;}),({struct Cyc_List_List*_tmp9D7=Cyc_yyget_YY56(&(yyyvsp[1]).v);_tmp506->tl=_tmp9D7;});_tmp506;}));
goto _LL0;case 206U: _LL197: _LL198:
# 2097 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));_tmp507->tag=0U,({struct Cyc_Absyn_Exp*_tmp9D9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp507->f1=_tmp9D9;});_tmp507;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2099 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp508=_region_malloc(yyr,sizeof(*_tmp508));_tmp508->tag=1U,({void*_tmp9DA=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp508->f1=_tmp9DA;});_tmp508;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2101 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));_tmp509->tag=2U;_tmp509;}));
goto _LL0;case 209U: _LL19D: _LL19E:
# 2103 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp50A=_region_malloc(yyr,sizeof(*_tmp50A));_tmp50A->tag=3U;_tmp50A;}));
goto _LL0;case 210U: _LL19F: _LL1A0:
# 2105 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50B=_region_malloc(yyr,sizeof(*_tmp50B));_tmp50B->tag=4U;_tmp50B;}));
goto _LL0;case 211U: _LL1A1: _LL1A2:
# 2107 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50C=_region_malloc(yyr,sizeof(*_tmp50C));_tmp50C->tag=5U;_tmp50C;}));
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2109 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp50D=_region_malloc(yyr,sizeof(*_tmp50D));_tmp50D->tag=6U;_tmp50D;}));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2111 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp50E=_region_malloc(yyr,sizeof(*_tmp50E));_tmp50E->tag=7U;_tmp50E;}));
goto _LL0;case 214U: _LL1A7: _LL1A8: {
# 2117 "parse.y"
unsigned loc=(unsigned)((yyyvsp[0]).l).first_line;
if(!Cyc_Parse_parsing_tempest)
yyval=Cyc_YY1(({struct _tuple22*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->f1=loc,_tmp50F->f2=Cyc_Absyn_true_type,({void*_tmp9DB=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp50F->f3=_tmp9DB;});_tmp50F;}));else{
# 2121
yyval=Cyc_YY1(({struct _tuple22*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->f1=loc,_tmp510->f2=Cyc_Absyn_true_type,_tmp510->f3=Cyc_Absyn_fat_bound_type;_tmp510;}));}
# 2123
goto _LL0;}case 215U: _LL1A9: _LL1AA: {
# 2124 "parse.y"
unsigned loc=(unsigned)((yyyvsp[0]).l).first_line;
yyval=Cyc_YY1(({struct _tuple22*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->f1=loc,_tmp511->f2=Cyc_Absyn_false_type,({void*_tmp9DC=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp511->f3=_tmp9DC;});_tmp511;}));
# 2127
goto _LL0;}case 216U: _LL1AB: _LL1AC: {
# 2128 "parse.y"
unsigned loc=(unsigned)((yyyvsp[0]).l).first_line;
yyval=Cyc_YY1(({struct _tuple22*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->f1=loc,_tmp512->f2=Cyc_Absyn_true_type,_tmp512->f3=Cyc_Absyn_fat_bound_type;_tmp512;}));
# 2131
goto _LL0;}case 217U: _LL1AD: _LL1AE:
# 2133
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2134 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2137
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2138 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2139 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2144 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2145 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Tcutil_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 224U: _LL1BB: _LL1BC:
# 2146 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 225U: _LL1BD: _LL1BE:
# 2150 "parse.y"
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2151 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9DD=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp9DD,Cyc_yyget_YY23(&(yyyvsp[1]).v));}));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2156 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp513=_cycalloc(sizeof(*_tmp513));({struct Cyc_List_List*_tmp9E0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp513->f1=_tmp9E0;}),_tmp513->f2=0,_tmp513->f3=0,({void*_tmp9DF=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp513->f4=_tmp9DF;}),({struct Cyc_List_List*_tmp9DE=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp513->f5=_tmp9DE;});_tmp513;}));
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2158 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp514=_cycalloc(sizeof(*_tmp514));({struct Cyc_List_List*_tmp9E3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp514->f1=_tmp9E3;}),_tmp514->f2=1,_tmp514->f3=0,({void*_tmp9E2=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp514->f4=_tmp9E2;}),({struct Cyc_List_List*_tmp9E1=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp514->f5=_tmp9E1;});_tmp514;}));
goto _LL0;case 229U: _LL1C5: _LL1C6: {
# 2161
struct _tuple8*_tmp515=Cyc_yyget_YY37(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1F=_tmp515;struct _tuple8*_tmp516=_stmttmp1F;void*_tmp519;struct Cyc_Absyn_Tqual _tmp518;struct _fat_ptr*_tmp517;_LL475: _tmp517=_tmp516->f1;_tmp518=_tmp516->f2;_tmp519=_tmp516->f3;_LL476: {struct _fat_ptr*n=_tmp517;struct Cyc_Absyn_Tqual tq=_tmp518;void*t=_tmp519;
struct Cyc_Absyn_VarargInfo*_tmp51A=({struct Cyc_Absyn_VarargInfo*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->name=n,_tmp51C->tq=tq,_tmp51C->type=t,({int _tmp9E4=Cyc_yyget_YY31(&(yyyvsp[1]).v);_tmp51C->inject=_tmp9E4;});_tmp51C;});struct Cyc_Absyn_VarargInfo*v=_tmp51A;
yyval=Cyc_YY39(({struct _tuple27*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->f1=0,_tmp51B->f2=0,_tmp51B->f3=v,({void*_tmp9E6=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp51B->f4=_tmp9E6;}),({struct Cyc_List_List*_tmp9E5=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp51B->f5=_tmp9E5;});_tmp51B;}));
# 2165
goto _LL0;}}case 230U: _LL1C7: _LL1C8: {
# 2167
struct _tuple8*_tmp51D=Cyc_yyget_YY37(&(yyyvsp[4]).v);struct _tuple8*_stmttmp20=_tmp51D;struct _tuple8*_tmp51E=_stmttmp20;void*_tmp521;struct Cyc_Absyn_Tqual _tmp520;struct _fat_ptr*_tmp51F;_LL478: _tmp51F=_tmp51E->f1;_tmp520=_tmp51E->f2;_tmp521=_tmp51E->f3;_LL479: {struct _fat_ptr*n=_tmp51F;struct Cyc_Absyn_Tqual tq=_tmp520;void*t=_tmp521;
struct Cyc_Absyn_VarargInfo*_tmp522=({struct Cyc_Absyn_VarargInfo*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->name=n,_tmp524->tq=tq,_tmp524->type=t,({int _tmp9E7=Cyc_yyget_YY31(&(yyyvsp[3]).v);_tmp524->inject=_tmp9E7;});_tmp524;});struct Cyc_Absyn_VarargInfo*v=_tmp522;
yyval=Cyc_YY39(({struct _tuple27*_tmp523=_cycalloc(sizeof(*_tmp523));({struct Cyc_List_List*_tmp9EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp523->f1=_tmp9EA;}),_tmp523->f2=0,_tmp523->f3=v,({void*_tmp9E9=Cyc_yyget_YY49(&(yyyvsp[5]).v);_tmp523->f4=_tmp9E9;}),({struct Cyc_List_List*_tmp9E8=Cyc_yyget_YY50(&(yyyvsp[6]).v);_tmp523->f5=_tmp9E8;});_tmp523;}));
# 2171
goto _LL0;}}case 231U: _LL1C9: _LL1CA:
# 2175 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9EB=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9EB,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525->tag=1U,_tmp525->f1=0;_tmp525;}));}));
goto _LL0;case 232U: _LL1CB: _LL1CC:
# 2176 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9EC=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9EC,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(&(yyyvsp[2]).v)));}));
goto _LL0;case 233U: _LL1CD: _LL1CE:
# 2179
 yyval=Cyc_YY49(0);
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2180 "parse.y"
 yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2184 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2185 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 237U: _LL1D5: _LL1D6: {
# 2193 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp526=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->tag=2U,_tmp52B->f1=0,_tmp52B->f2=& Cyc_Tcutil_trk;_tmp52B;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp526;
struct _fat_ptr _tmp527=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr id=_tmp527;
void*_tmp528=Cyc_Parse_id2type(id,(void*)kb);void*t=_tmp528;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp52A=_cycalloc(sizeof(*_tmp52A));({struct _tuple34*_tmp9EE=({struct _tuple34*_tmp529=_cycalloc(sizeof(*_tmp529));({void*_tmp9ED=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp529->f1=_tmp9ED;}),_tmp529->f2=t;_tmp529;});_tmp52A->hd=_tmp9EE;}),_tmp52A->tl=0;_tmp52A;}));
# 2198
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2200 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52C=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp531=_cycalloc(sizeof(*_tmp531));_tmp531->tag=2U,_tmp531->f1=0,_tmp531->f2=& Cyc_Tcutil_trk;_tmp531;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp52C;
struct _fat_ptr _tmp52D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr id=_tmp52D;
void*_tmp52E=Cyc_Parse_id2type(id,(void*)kb);void*t=_tmp52E;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp530=_cycalloc(sizeof(*_tmp530));({struct _tuple34*_tmp9F1=({struct _tuple34*_tmp52F=_cycalloc(sizeof(*_tmp52F));({void*_tmp9F0=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp52F->f1=_tmp9F0;}),_tmp52F->f2=t;_tmp52F;});_tmp530->hd=_tmp9F1;}),({struct Cyc_List_List*_tmp9EF=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp530->tl=_tmp9EF;});_tmp530;}));
# 2205
goto _LL0;}case 239U: _LL1D9: _LL1DA:
# 2209 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 240U: _LL1DB: _LL1DC:
# 2211 "parse.y"
 if(({struct _fat_ptr _tmp9F2=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp9F2,({const char*_tmp532="inject";_tag_fat(_tmp532,sizeof(char),7U);}));})!= 0)
({void*_tmp533=0U;({unsigned _tmp9F4=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp9F3=({const char*_tmp534="missing type in function declaration";_tag_fat(_tmp534,sizeof(char),37U);});Cyc_Warn_err(_tmp9F4,_tmp9F3,_tag_fat(_tmp533,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);
# 2215
goto _LL0;case 241U: _LL1DD: _LL1DE:
# 2218 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2219 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp9F5=Cyc_yyget_YY40(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F5,Cyc_yyget_YY40(&(yyyvsp[2]).v));}));
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2223 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2224 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2226 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp535=_cycalloc(sizeof(*_tmp535));({void*_tmp9F6=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v));_tmp535->hd=_tmp9F6;}),_tmp535->tl=0;_tmp535;}));
goto _LL0;case 246U: _LL1E7: _LL1E8:
# 2228 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));({void*_tmp9F7=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp536->hd=_tmp9F7;}),_tmp536->tl=0;_tmp536;}));
# 2231
goto _LL0;case 247U: _LL1E9: _LL1EA:
# 2248 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));({void*_tmp9F9=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9F8=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9F8,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp537->hd=_tmp9F9;}),_tmp537->tl=0;_tmp537;}));
goto _LL0;case 248U: _LL1EB: _LL1EC:
# 2250 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp538=_cycalloc(sizeof(*_tmp538));({void*_tmp9FC=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9FB=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9FB,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp538->hd=_tmp9FC;}),({struct Cyc_List_List*_tmp9FA=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp538->tl=_tmp9FA;});_tmp538;}));
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2256 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp539=_cycalloc(sizeof(*_tmp539));({struct _tuple8*_tmp9FD=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp539->hd=_tmp9FD;}),_tmp539->tl=0;_tmp539;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2258 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp53A=_cycalloc(sizeof(*_tmp53A));({struct _tuple8*_tmp9FF=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp53A->hd=_tmp9FF;}),({struct Cyc_List_List*_tmp9FE=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp53A->tl=_tmp9FE;});_tmp53A;}));
goto _LL0;case 251U: _LL1F1: _LL1F2: {
# 2264 "parse.y"
struct _tuple26 _tmp53B=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp21=_tmp53B;struct _tuple26 _tmp53C=_stmttmp21;struct Cyc_List_List*_tmp53F;struct Cyc_Parse_Type_specifier _tmp53E;struct Cyc_Absyn_Tqual _tmp53D;_LL47B: _tmp53D=_tmp53C.f1;_tmp53E=_tmp53C.f2;_tmp53F=_tmp53C.f3;_LL47C: {struct Cyc_Absyn_Tqual tq=_tmp53D;struct Cyc_Parse_Type_specifier tspecs=_tmp53E;struct Cyc_List_List*atts=_tmp53F;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp540=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp22=_tmp540;struct Cyc_Parse_Declarator _tmp541=_stmttmp22;struct Cyc_List_List*_tmp544;unsigned _tmp543;struct _tuple0*_tmp542;_LL47E: _tmp542=_tmp541.id;_tmp543=_tmp541.varloc;_tmp544=_tmp541.tms;_LL47F: {struct _tuple0*qv=_tmp542;unsigned varloc=_tmp543;struct Cyc_List_List*tms=_tmp544;
void*_tmp545=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp545;
struct _tuple14 _tmp546=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp546;struct _tuple14 _tmp547=_stmttmp23;struct Cyc_List_List*_tmp54B;struct Cyc_List_List*_tmp54A;void*_tmp549;struct Cyc_Absyn_Tqual _tmp548;_LL481: _tmp548=_tmp547.f1;_tmp549=_tmp547.f2;_tmp54A=_tmp547.f3;_tmp54B=_tmp547.f4;_LL482: {struct Cyc_Absyn_Tqual tq2=_tmp548;void*t2=_tmp549;struct Cyc_List_List*tvs=_tmp54A;struct Cyc_List_List*atts2=_tmp54B;
if(tvs != 0)
({void*_tmp54C=0U;({unsigned _tmpA01=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA00=({const char*_tmp54D="parameter with bad type params";_tag_fat(_tmp54D,sizeof(char),31U);});Cyc_Warn_err(_tmpA01,_tmpA00,_tag_fat(_tmp54C,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({void*_tmp54E=0U;({unsigned _tmpA03=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA02=({const char*_tmp54F="parameter cannot be qualified with a namespace";_tag_fat(_tmp54F,sizeof(char),47U);});Cyc_Warn_err(_tmpA03,_tmpA02,_tag_fat(_tmp54E,sizeof(void*),0U));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({void*_tmp550=0U;({unsigned _tmpA05=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA04=({const char*_tmp551="extra attributes on parameter, ignoring";_tag_fat(_tmp551,sizeof(char),40U);});Cyc_Warn_warn(_tmpA05,_tmpA04,_tag_fat(_tmp550,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->f1=idopt,_tmp552->f2=tq2,_tmp552->f3=t2;_tmp552;}));
# 2278
goto _LL0;}}}}}}case 252U: _LL1F3: _LL1F4: {
# 2279 "parse.y"
struct _tuple26 _tmp553=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp24=_tmp553;struct _tuple26 _tmp554=_stmttmp24;struct Cyc_List_List*_tmp557;struct Cyc_Parse_Type_specifier _tmp556;struct Cyc_Absyn_Tqual _tmp555;_LL484: _tmp555=_tmp554.f1;_tmp556=_tmp554.f2;_tmp557=_tmp554.f3;_LL485: {struct Cyc_Absyn_Tqual tq=_tmp555;struct Cyc_Parse_Type_specifier tspecs=_tmp556;struct Cyc_List_List*atts=_tmp557;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp558=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp558;
if(atts != 0)
({void*_tmp559=0U;({unsigned _tmpA07=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA06=({const char*_tmp55A="bad attributes on parameter, ignoring";_tag_fat(_tmp55A,sizeof(char),38U);});Cyc_Warn_warn(_tmpA07,_tmpA06,_tag_fat(_tmp559,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->f1=0,_tmp55B->f2=tq,_tmp55B->f3=t;_tmp55B;}));
# 2286
goto _LL0;}}}case 253U: _LL1F5: _LL1F6: {
# 2287 "parse.y"
struct _tuple26 _tmp55C=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp25=_tmp55C;struct _tuple26 _tmp55D=_stmttmp25;struct Cyc_List_List*_tmp560;struct Cyc_Parse_Type_specifier _tmp55F;struct Cyc_Absyn_Tqual _tmp55E;_LL487: _tmp55E=_tmp55D.f1;_tmp55F=_tmp55D.f2;_tmp560=_tmp55D.f3;_LL488: {struct Cyc_Absyn_Tqual tq=_tmp55E;struct Cyc_Parse_Type_specifier tspecs=_tmp55F;struct Cyc_List_List*atts=_tmp560;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp561=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp561;
struct Cyc_List_List*_tmp562=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp562;
struct _tuple14 _tmp563=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp26=_tmp563;struct _tuple14 _tmp564=_stmttmp26;struct Cyc_List_List*_tmp568;struct Cyc_List_List*_tmp567;void*_tmp566;struct Cyc_Absyn_Tqual _tmp565;_LL48A: _tmp565=_tmp564.f1;_tmp566=_tmp564.f2;_tmp567=_tmp564.f3;_tmp568=_tmp564.f4;_LL48B: {struct Cyc_Absyn_Tqual tq2=_tmp565;void*t2=_tmp566;struct Cyc_List_List*tvs=_tmp567;struct Cyc_List_List*atts2=_tmp568;
if(tvs != 0)
({void*_tmp569=0U;({unsigned _tmpA09=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA08=({const char*_tmp56A="bad type parameters on formal argument, ignoring";_tag_fat(_tmp56A,sizeof(char),49U);});Cyc_Warn_warn(_tmpA09,_tmpA08,_tag_fat(_tmp569,sizeof(void*),0U));});});
# 2295
if(atts2 != 0)
({void*_tmp56B=0U;({unsigned _tmpA0B=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA0A=({const char*_tmp56C="bad attributes on parameter, ignoring";_tag_fat(_tmp56C,sizeof(char),38U);});Cyc_Warn_warn(_tmpA0B,_tmpA0A,_tag_fat(_tmp56B,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->f1=0,_tmp56D->f2=tq2,_tmp56D->f3=t2;_tmp56D;}));
# 2299
goto _LL0;}}}}case 254U: _LL1F7: _LL1F8:
# 2303 "parse.y"
 yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyvsp[0]).v)));
goto _LL0;case 255U: _LL1F9: _LL1FA:
# 2307 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp56F=_cycalloc(sizeof(*_tmp56F));({struct _fat_ptr*_tmpA0D=({struct _fat_ptr*_tmp56E=_cycalloc(sizeof(*_tmp56E));({struct _fat_ptr _tmpA0C=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp56E=_tmpA0C;});_tmp56E;});_tmp56F->hd=_tmpA0D;}),_tmp56F->tl=0;_tmp56F;}));
goto _LL0;case 256U: _LL1FB: _LL1FC:
# 2309 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));({struct _fat_ptr*_tmpA10=({struct _fat_ptr*_tmp570=_cycalloc(sizeof(*_tmp570));({struct _fat_ptr _tmpA0F=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp570=_tmpA0F;});_tmp570;});_tmp571->hd=_tmpA10;}),({struct Cyc_List_List*_tmpA0E=Cyc_yyget_YY36(&(yyyvsp[0]).v);_tmp571->tl=_tmpA0E;});_tmp571;}));
goto _LL0;case 257U: _LL1FD: _LL1FE:
# 2313 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 258U: _LL1FF: _LL200:
# 2314 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 259U: _LL201: _LL202:
# 2319 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA11=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp572=_cycalloc(sizeof(*_tmp572));_tmp572->tag=36U,_tmp572->f1=0,_tmp572->f2=0;_tmp572;});Cyc_Absyn_new_exp(_tmpA11,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 260U: _LL203: _LL204:
# 2321 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA13=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=36U,_tmp573->f1=0,({struct Cyc_List_List*_tmpA12=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp573->f2=_tmpA12;});_tmp573;});Cyc_Absyn_new_exp(_tmpA13,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 261U: _LL205: _LL206:
# 2323 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA15=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->tag=36U,_tmp574->f1=0,({struct Cyc_List_List*_tmpA14=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp574->f2=_tmpA14;});_tmp574;});Cyc_Absyn_new_exp(_tmpA15,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 262U: _LL207: _LL208: {
# 2325 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp575=({unsigned _tmpA1A=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmpA19=({struct _tuple0*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA17=({struct _fat_ptr*_tmp577=_cycalloc(sizeof(*_tmp577));({struct _fat_ptr _tmpA16=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp577=_tmpA16;});_tmp577;});_tmp578->f2=_tmpA17;});_tmp578;});void*_tmpA18=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpA1A,_tmpA19,_tmpA18,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2325
struct Cyc_Absyn_Vardecl*vd=_tmp575;
# 2328
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmpA1D=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp576=_cycalloc(sizeof(*_tmp576));_tmp576->tag=27U,_tmp576->f1=vd,({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp576->f2=_tmpA1C;}),({struct Cyc_Absyn_Exp*_tmpA1B=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp576->f3=_tmpA1B;}),_tmp576->f4=0;_tmp576;});Cyc_Absyn_new_exp(_tmpA1D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2331
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2333 "parse.y"
void*_tmp579=({struct _tuple8*_tmpA1E=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpA1E,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp579;
yyval=Cyc_Exp_tok(({void*_tmpA20=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->tag=28U,({struct Cyc_Absyn_Exp*_tmpA1F=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp57A->f1=_tmpA1F;}),_tmp57A->f2=t,_tmp57A->f3=0;_tmp57A;});Cyc_Absyn_new_exp(_tmpA20,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2336
goto _LL0;}case 264U: _LL20B: _LL20C:
# 2341 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));({struct _tuple35*_tmpA22=({struct _tuple35*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->f1=0,({struct Cyc_Absyn_Exp*_tmpA21=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp57B->f2=_tmpA21;});_tmp57B;});_tmp57C->hd=_tmpA22;}),_tmp57C->tl=0;_tmp57C;}));
goto _LL0;case 265U: _LL20D: _LL20E:
# 2343 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct _tuple35*_tmpA25=({struct _tuple35*_tmp57D=_cycalloc(sizeof(*_tmp57D));({struct Cyc_List_List*_tmpA24=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp57D->f1=_tmpA24;}),({struct Cyc_Absyn_Exp*_tmpA23=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp57D->f2=_tmpA23;});_tmp57D;});_tmp57E->hd=_tmpA25;}),_tmp57E->tl=0;_tmp57E;}));
goto _LL0;case 266U: _LL20F: _LL210:
# 2345 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp580=_cycalloc(sizeof(*_tmp580));({struct _tuple35*_tmpA28=({struct _tuple35*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->f1=0,({struct Cyc_Absyn_Exp*_tmpA27=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp57F->f2=_tmpA27;});_tmp57F;});_tmp580->hd=_tmpA28;}),({struct Cyc_List_List*_tmpA26=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp580->tl=_tmpA26;});_tmp580;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2347 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp582=_cycalloc(sizeof(*_tmp582));({struct _tuple35*_tmpA2C=({struct _tuple35*_tmp581=_cycalloc(sizeof(*_tmp581));({struct Cyc_List_List*_tmpA2B=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp581->f1=_tmpA2B;}),({struct Cyc_Absyn_Exp*_tmpA2A=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp581->f2=_tmpA2A;});_tmp581;});_tmp582->hd=_tmpA2C;}),({struct Cyc_List_List*_tmpA29=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp582->tl=_tmpA29;});_tmp582;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2351 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[0]).v)));
goto _LL0;case 269U: _LL215: _LL216:
# 2352 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp585=_cycalloc(sizeof(*_tmp585));({void*_tmpA2F=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->tag=1U,({struct _fat_ptr*_tmpA2E=({struct _fat_ptr*_tmp583=_cycalloc(sizeof(*_tmp583));({struct _fat_ptr _tmpA2D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp583=_tmpA2D;});_tmp583;});_tmp584->f1=_tmpA2E;});_tmp584;});_tmp585->hd=_tmpA2F;}),_tmp585->tl=0;_tmp585;}));
goto _LL0;case 270U: _LL217: _LL218:
# 2357 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp586=_cycalloc(sizeof(*_tmp586));({void*_tmpA30=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp586->hd=_tmpA30;}),_tmp586->tl=0;_tmp586;}));
goto _LL0;case 271U: _LL219: _LL21A:
# 2358 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));({void*_tmpA32=Cyc_yyget_YY42(&(yyyvsp[1]).v);_tmp587->hd=_tmpA32;}),({struct Cyc_List_List*_tmpA31=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp587->tl=_tmpA31;});_tmp587;}));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2362 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->tag=0U,({struct Cyc_Absyn_Exp*_tmpA33=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp588->f1=_tmpA33;});_tmp588;}));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2363 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->tag=1U,({struct _fat_ptr*_tmpA35=({struct _fat_ptr*_tmp589=_cycalloc(sizeof(*_tmp589));({struct _fat_ptr _tmpA34=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp589=_tmpA34;});_tmp589;});_tmp58A->f1=_tmpA35;});_tmp58A;}));
goto _LL0;case 274U: _LL21F: _LL220: {
# 2368 "parse.y"
struct _tuple26 _tmp58B=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp27=_tmp58B;struct _tuple26 _tmp58C=_stmttmp27;struct Cyc_List_List*_tmp58F;struct Cyc_Parse_Type_specifier _tmp58E;struct Cyc_Absyn_Tqual _tmp58D;_LL48D: _tmp58D=_tmp58C.f1;_tmp58E=_tmp58C.f2;_tmp58F=_tmp58C.f3;_LL48E: {struct Cyc_Absyn_Tqual tq=_tmp58D;struct Cyc_Parse_Type_specifier tss=_tmp58E;struct Cyc_List_List*atts=_tmp58F;
void*_tmp590=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp590;
if(atts != 0)
({void*_tmp591=0U;({unsigned _tmpA37=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA36=({const char*_tmp592="ignoring attributes in type";_tag_fat(_tmp592,sizeof(char),28U);});Cyc_Warn_warn(_tmpA37,_tmpA36,_tag_fat(_tmp591,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->f1=0,_tmp593->f2=tq,_tmp593->f3=t;_tmp593;}));
# 2374
goto _LL0;}}case 275U: _LL221: _LL222: {
# 2375 "parse.y"
struct _tuple26 _tmp594=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp28=_tmp594;struct _tuple26 _tmp595=_stmttmp28;struct Cyc_List_List*_tmp598;struct Cyc_Parse_Type_specifier _tmp597;struct Cyc_Absyn_Tqual _tmp596;_LL490: _tmp596=_tmp595.f1;_tmp597=_tmp595.f2;_tmp598=_tmp595.f3;_LL491: {struct Cyc_Absyn_Tqual tq=_tmp596;struct Cyc_Parse_Type_specifier tss=_tmp597;struct Cyc_List_List*atts=_tmp598;
void*_tmp599=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp599;
struct Cyc_List_List*_tmp59A=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp59A;
struct _tuple14 _tmp59B=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp59B;
if(t_info.f3 != 0)
# 2381
({void*_tmp59C=0U;({unsigned _tmpA39=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA38=({const char*_tmp59D="bad type params, ignoring";_tag_fat(_tmp59D,sizeof(char),26U);});Cyc_Warn_warn(_tmpA39,_tmpA38,_tag_fat(_tmp59C,sizeof(void*),0U));});});
if(t_info.f4 != 0)
({void*_tmp59E=0U;({unsigned _tmpA3B=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA3A=({const char*_tmp59F="bad specifiers, ignoring";_tag_fat(_tmp59F,sizeof(char),25U);});Cyc_Warn_warn(_tmpA3B,_tmpA3A,_tag_fat(_tmp59E,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->f1=0,_tmp5A0->f2=t_info.f1,_tmp5A0->f3=t_info.f2;_tmp5A0;}));
# 2386
goto _LL0;}}case 276U: _LL223: _LL224:
# 2389 "parse.y"
 yyval=Cyc_YY44(({struct _tuple8*_tmpA3C=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA3C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 277U: _LL225: _LL226:
# 2390 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 278U: _LL227: _LL228:
# 2391 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 279U: _LL229: _LL22A:
# 2392 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v)));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2393 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));({void*_tmpA3E=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A1->hd=_tmpA3E;}),({struct Cyc_List_List*_tmpA3D=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp5A1->tl=_tmpA3D;});_tmp5A1;})));
goto _LL0;case 281U: _LL22D: _LL22E:
# 2399 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));({void*_tmpA3F=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A2->hd=_tmpA3F;}),_tmp5A2->tl=0;_tmp5A2;}));
goto _LL0;case 282U: _LL22F: _LL230:
# 2400 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));({void*_tmpA41=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp5A3->hd=_tmpA41;}),({struct Cyc_List_List*_tmpA40=Cyc_yyget_YY40(&(yyyvsp[0]).v);_tmp5A3->tl=_tmpA40;});_tmp5A3;}));
goto _LL0;case 283U: _LL231: _LL232:
# 2405 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp757;({struct Cyc_List_List*_tmpA42=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp757.tms=_tmpA42;});_tmp757;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2407 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 285U: _LL235: _LL236:
# 2409 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp758;({struct Cyc_List_List*_tmpA44=({struct Cyc_List_List*_tmpA43=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA43,(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms);});_tmp758.tms=_tmpA44;});_tmp758;}));
goto _LL0;case 286U: _LL237: _LL238:
# 2414 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 287U: _LL239: _LL23A:
# 2416 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp759;({struct Cyc_List_List*_tmpA47=({struct Cyc_List_List*_tmp5A5=_region_malloc(yyr,sizeof(*_tmp5A5));({void*_tmpA46=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A4=_region_malloc(yyr,sizeof(*_tmp5A4));_tmp5A4->tag=0U,({void*_tmpA45=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp5A4->f1=_tmpA45;}),_tmp5A4->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp5A4;});_tmp5A5->hd=_tmpA46;}),_tmp5A5->tl=0;_tmp5A5;});_tmp759.tms=_tmpA47;});_tmp759;}));
goto _LL0;case 288U: _LL23B: _LL23C:
# 2418 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75A;({struct Cyc_List_List*_tmpA4B=({struct Cyc_List_List*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));({void*_tmpA4A=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A6=_region_malloc(yyr,sizeof(*_tmp5A6));_tmp5A6->tag=0U,({void*_tmpA49=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A6->f1=_tmpA49;}),_tmp5A6->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A6;});_tmp5A7->hd=_tmpA4A;}),({struct Cyc_List_List*_tmpA48=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5A7->tl=_tmpA48;});_tmp5A7;});_tmp75A.tms=_tmpA4B;});_tmp75A;}));
goto _LL0;case 289U: _LL23D: _LL23E:
# 2420 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75B;({struct Cyc_List_List*_tmpA4F=({struct Cyc_List_List*_tmp5A9=_region_malloc(yyr,sizeof(*_tmp5A9));({void*_tmpA4E=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));_tmp5A8->tag=1U,({struct Cyc_Absyn_Exp*_tmpA4D=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp5A8->f1=_tmpA4D;}),({void*_tmpA4C=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A8->f2=_tmpA4C;}),_tmp5A8->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A8;});_tmp5A9->hd=_tmpA4E;}),_tmp5A9->tl=0;_tmp5A9;});_tmp75B.tms=_tmpA4F;});_tmp75B;}));
goto _LL0;case 290U: _LL23F: _LL240:
# 2422 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75C;({struct Cyc_List_List*_tmpA54=({struct Cyc_List_List*_tmp5AB=_region_malloc(yyr,sizeof(*_tmp5AB));({void*_tmpA53=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5AA=_region_malloc(yyr,sizeof(*_tmp5AA));_tmp5AA->tag=1U,({struct Cyc_Absyn_Exp*_tmpA52=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5AA->f1=_tmpA52;}),({void*_tmpA51=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp5AA->f2=_tmpA51;}),_tmp5AA->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp5AA;});_tmp5AB->hd=_tmpA53;}),({
struct Cyc_List_List*_tmpA50=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5AB->tl=_tmpA50;});_tmp5AB;});
# 2422
_tmp75C.tms=_tmpA54;});_tmp75C;}));
# 2425
goto _LL0;case 291U: _LL241: _LL242:
# 2426 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75D;({struct Cyc_List_List*_tmpA5B=({struct Cyc_List_List*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));({void*_tmpA5A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5AD=_region_malloc(yyr,sizeof(*_tmp5AD));_tmp5AD->tag=3U,({void*_tmpA59=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5AC=_region_malloc(yyr,sizeof(*_tmp5AC));_tmp5AC->tag=1U,_tmp5AC->f1=0,_tmp5AC->f2=0,_tmp5AC->f3=0,({void*_tmpA58=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp5AC->f4=_tmpA58;}),({struct Cyc_List_List*_tmpA57=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp5AC->f5=_tmpA57;}),({struct Cyc_Absyn_Exp*_tmpA56=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5AC->f6=_tmpA56;}),({struct Cyc_Absyn_Exp*_tmpA55=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5AC->f7=_tmpA55;});_tmp5AC;});_tmp5AD->f1=_tmpA59;});_tmp5AD;});_tmp5AE->hd=_tmpA5A;}),_tmp5AE->tl=0;_tmp5AE;});_tmp75D.tms=_tmpA5B;});_tmp75D;}));
# 2428
goto _LL0;case 292U: _LL243: _LL244: {
# 2429 "parse.y"
struct _tuple27*_tmp5AF=Cyc_yyget_YY39(&(yyyvsp[1]).v);struct _tuple27*_stmttmp29=_tmp5AF;struct _tuple27*_tmp5B0=_stmttmp29;struct Cyc_List_List*_tmp5B5;void*_tmp5B4;struct Cyc_Absyn_VarargInfo*_tmp5B3;int _tmp5B2;struct Cyc_List_List*_tmp5B1;_LL493: _tmp5B1=_tmp5B0->f1;_tmp5B2=_tmp5B0->f2;_tmp5B3=_tmp5B0->f3;_tmp5B4=_tmp5B0->f4;_tmp5B5=_tmp5B0->f5;_LL494: {struct Cyc_List_List*lis=_tmp5B1;int b=_tmp5B2;struct Cyc_Absyn_VarargInfo*c=_tmp5B3;void*eff=_tmp5B4;struct Cyc_List_List*po=_tmp5B5;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75E;({struct Cyc_List_List*_tmpA60=({struct Cyc_List_List*_tmp5B8=_region_malloc(yyr,sizeof(*_tmp5B8));({void*_tmpA5F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B7=_region_malloc(yyr,sizeof(*_tmp5B7));_tmp5B7->tag=3U,({void*_tmpA5E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B6=_region_malloc(yyr,sizeof(*_tmp5B6));_tmp5B6->tag=1U,_tmp5B6->f1=lis,_tmp5B6->f2=b,_tmp5B6->f3=c,_tmp5B6->f4=eff,_tmp5B6->f5=po,({struct Cyc_Absyn_Exp*_tmpA5D=Cyc_yyget_YY57(&(yyyvsp[3]).v);_tmp5B6->f6=_tmpA5D;}),({struct Cyc_Absyn_Exp*_tmpA5C=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5B6->f7=_tmpA5C;});_tmp5B6;});_tmp5B7->f1=_tmpA5E;});_tmp5B7;});_tmp5B8->hd=_tmpA5F;}),_tmp5B8->tl=0;_tmp5B8;});_tmp75E.tms=_tmpA60;});_tmp75E;}));
# 2432
goto _LL0;}}case 293U: _LL245: _LL246:
# 2433 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75F;({struct Cyc_List_List*_tmpA68=({struct Cyc_List_List*_tmp5BB=_region_malloc(yyr,sizeof(*_tmp5BB));({void*_tmpA67=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5BA=_region_malloc(yyr,sizeof(*_tmp5BA));_tmp5BA->tag=3U,({void*_tmpA66=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B9=_region_malloc(yyr,sizeof(*_tmp5B9));_tmp5B9->tag=1U,_tmp5B9->f1=0,_tmp5B9->f2=0,_tmp5B9->f3=0,({void*_tmpA65=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp5B9->f4=_tmpA65;}),({struct Cyc_List_List*_tmpA64=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp5B9->f5=_tmpA64;}),({struct Cyc_Absyn_Exp*_tmpA63=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5B9->f6=_tmpA63;}),({struct Cyc_Absyn_Exp*_tmpA62=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp5B9->f7=_tmpA62;});_tmp5B9;});_tmp5BA->f1=_tmpA66;});_tmp5BA;});_tmp5BB->hd=_tmpA67;}),({
struct Cyc_List_List*_tmpA61=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5BB->tl=_tmpA61;});_tmp5BB;});
# 2433
_tmp75F.tms=_tmpA68;});_tmp75F;}));
# 2436
goto _LL0;case 294U: _LL247: _LL248: {
# 2437 "parse.y"
struct _tuple27*_tmp5BC=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp2A=_tmp5BC;struct _tuple27*_tmp5BD=_stmttmp2A;struct Cyc_List_List*_tmp5C2;void*_tmp5C1;struct Cyc_Absyn_VarargInfo*_tmp5C0;int _tmp5BF;struct Cyc_List_List*_tmp5BE;_LL496: _tmp5BE=_tmp5BD->f1;_tmp5BF=_tmp5BD->f2;_tmp5C0=_tmp5BD->f3;_tmp5C1=_tmp5BD->f4;_tmp5C2=_tmp5BD->f5;_LL497: {struct Cyc_List_List*lis=_tmp5BE;int b=_tmp5BF;struct Cyc_Absyn_VarargInfo*c=_tmp5C0;void*eff=_tmp5C1;struct Cyc_List_List*po=_tmp5C2;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp760;({struct Cyc_List_List*_tmpA6E=({struct Cyc_List_List*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));({void*_tmpA6D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4->tag=3U,({void*_tmpA6C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));_tmp5C3->tag=1U,_tmp5C3->f1=lis,_tmp5C3->f2=b,_tmp5C3->f3=c,_tmp5C3->f4=eff,_tmp5C3->f5=po,({
struct Cyc_Absyn_Exp*_tmpA6B=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5C3->f6=_tmpA6B;}),({struct Cyc_Absyn_Exp*_tmpA6A=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5C3->f7=_tmpA6A;});_tmp5C3;});
# 2438
_tmp5C4->f1=_tmpA6C;});_tmp5C4;});_tmp5C5->hd=_tmpA6D;}),({
struct Cyc_List_List*_tmpA69=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C5->tl=_tmpA69;});_tmp5C5;});
# 2438
_tmp760.tms=_tmpA6E;});_tmp760;}));
# 2441
goto _LL0;}}case 295U: _LL249: _LL24A: {
# 2443
struct Cyc_List_List*_tmp5C6=({unsigned _tmpA6F=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA6F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp5C6;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp761;({struct Cyc_List_List*_tmpA72=({struct Cyc_List_List*_tmp5C8=_region_malloc(yyr,sizeof(*_tmp5C8));({void*_tmpA71=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5C7=_region_malloc(yyr,sizeof(*_tmp5C7));_tmp5C7->tag=4U,_tmp5C7->f1=ts,_tmp5C7->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp5C7->f3=0;_tmp5C7;});_tmp5C8->hd=_tmpA71;}),({
struct Cyc_List_List*_tmpA70=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C8->tl=_tmpA70;});_tmp5C8;});
# 2444
_tmp761.tms=_tmpA72;});_tmp761;}));
# 2447
goto _LL0;}case 296U: _LL24B: _LL24C:
# 2448 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp762;({struct Cyc_List_List*_tmpA76=({struct Cyc_List_List*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));({void*_tmpA75=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5C9=_region_malloc(yyr,sizeof(*_tmp5C9));_tmp5C9->tag=5U,_tmp5C9->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA74=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp5C9->f2=_tmpA74;});_tmp5C9;});_tmp5CA->hd=_tmpA75;}),({struct Cyc_List_List*_tmpA73=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5CA->tl=_tmpA73;});_tmp5CA;});_tmp762.tms=_tmpA76;});_tmp762;}));
# 2450
goto _LL0;case 297U: _LL24D: _LL24E:
# 2454 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 298U: _LL24F: _LL250:
# 2455 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299U: _LL251: _LL252:
# 2456 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300U: _LL253: _LL254:
# 2457 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301U: _LL255: _LL256:
# 2458 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2459 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 303U: _LL259: _LL25A:
# 2465 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA7A=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->tag=13U,({struct _fat_ptr*_tmpA79=({struct _fat_ptr*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));({struct _fat_ptr _tmpA78=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CB=_tmpA78;});_tmp5CB;});_tmp5CC->f1=_tmpA79;}),({struct Cyc_Absyn_Stmt*_tmpA77=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5CC->f2=_tmpA77;});_tmp5CC;});Cyc_Absyn_new_stmt(_tmpA7A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304U: _LL25B: _LL25C:
# 2469 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305U: _LL25D: _LL25E:
# 2470 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA7B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 306U: _LL25F: _LL260:
# 2475 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 307U: _LL261: _LL262:
# 2476 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 308U: _LL263: _LL264:
# 2481 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA7C=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA7C,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 309U: _LL265: _LL266:
# 2482 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA7D=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA7D,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 310U: _LL267: _LL268:
# 2483 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA82=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->tag=13U,({struct _fat_ptr*_tmpA81=({struct _fat_ptr*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));({struct _fat_ptr _tmpA80=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CD=_tmpA80;});_tmp5CD;});_tmp5CE->f1=_tmpA81;}),({struct Cyc_Absyn_Stmt*_tmpA7F=({struct Cyc_List_List*_tmpA7E=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA7E,Cyc_Absyn_skip_stmt(0U));});_tmp5CE->f2=_tmpA7F;});_tmp5CE;});Cyc_Absyn_new_stmt(_tmpA82,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2485
goto _LL0;case 311U: _LL269: _LL26A:
# 2485 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA87=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0->tag=13U,({struct _fat_ptr*_tmpA86=({struct _fat_ptr*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));({struct _fat_ptr _tmpA85=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CF=_tmpA85;});_tmp5CF;});_tmp5D0->f1=_tmpA86;}),({struct Cyc_Absyn_Stmt*_tmpA84=({struct Cyc_List_List*_tmpA83=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA83,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp5D0->f2=_tmpA84;});_tmp5D0;});Cyc_Absyn_new_stmt(_tmpA87,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2487
goto _LL0;case 312U: _LL26B: _LL26C:
# 2487 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 313U: _LL26D: _LL26E:
# 2488 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA89=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpA88=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpA89,_tmpA88,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 314U: _LL26F: _LL270:
# 2489 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA8C=({void*_tmpA8B=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA8A=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D1->f1=_tmpA8A;});_tmp5D1;});Cyc_Absyn_new_decl(_tmpA8B,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA8C,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 315U: _LL271: _LL272:
# 2492 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA8F=({void*_tmpA8E=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA8D=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D2->f1=_tmpA8D;});_tmp5D2;});Cyc_Absyn_new_decl(_tmpA8E,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA8F,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 316U: _LL273: _LL274:
# 2497 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA92=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA91=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA90=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA92,_tmpA91,_tmpA90,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317U: _LL275: _LL276:
# 2499 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA95=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA94=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA93=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpA95,_tmpA94,_tmpA93,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318U: _LL277: _LL278:
# 2505 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA97=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmpA96=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpA97,_tmpA96,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319U: _LL279: _LL27A: {
# 2508
struct Cyc_Absyn_Exp*_tmp5D3=({struct _tuple0*_tmpA98=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpA98,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D3;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9A=e;struct Cyc_List_List*_tmpA99=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpA9A,_tmpA99,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2512
struct Cyc_Absyn_Exp*_tmp5D4=({struct Cyc_List_List*_tmpA9B=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpA9B,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D4;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9D=e;struct Cyc_List_List*_tmpA9C=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpA9D,_tmpA9C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2515
goto _LL0;}case 321U: _LL27D: _LL27E:
# 2518 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA9F=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmpA9E=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpA9F,_tmpA9E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 322U: _LL27F: _LL280:
# 2532 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 323U: _LL281: _LL282:
# 2536 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));({struct Cyc_Absyn_Switch_clause*_tmpAA3=({struct Cyc_Absyn_Switch_clause*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));({struct Cyc_Absyn_Pat*_tmpAA2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp5D5->pattern=_tmpAA2;}),_tmp5D5->pat_vars=0,_tmp5D5->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA1=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5D5->body=_tmpAA1;}),_tmp5D5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D5;});
# 2536
_tmp5D6->hd=_tmpAA3;}),({
# 2538
struct Cyc_List_List*_tmpAA0=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D6->tl=_tmpAA0;});_tmp5D6;}));
goto _LL0;case 324U: _LL283: _LL284:
# 2540 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));({struct Cyc_Absyn_Switch_clause*_tmpAA7=({struct Cyc_Absyn_Switch_clause*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));({struct Cyc_Absyn_Pat*_tmpAA6=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D7->pattern=_tmpAA6;}),_tmp5D7->pat_vars=0,_tmp5D7->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA5=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp5D7->body=_tmpAA5;}),_tmp5D7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D7;});
# 2540
_tmp5D8->hd=_tmpAA7;}),({
# 2542
struct Cyc_List_List*_tmpAA4=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D8->tl=_tmpAA4;});_tmp5D8;}));
goto _LL0;case 325U: _LL285: _LL286:
# 2544 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));({struct Cyc_Absyn_Switch_clause*_tmpAAB=({struct Cyc_Absyn_Switch_clause*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct Cyc_Absyn_Pat*_tmpAAA=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D9->pattern=_tmpAAA;}),_tmp5D9->pat_vars=0,_tmp5D9->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpAA9=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp5D9->body=_tmpAA9;}),_tmp5D9->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D9;});_tmp5DA->hd=_tmpAAB;}),({struct Cyc_List_List*_tmpAA8=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp5DA->tl=_tmpAA8;});_tmp5DA;}));
goto _LL0;case 326U: _LL287: _LL288:
# 2546 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct Cyc_Absyn_Switch_clause*_tmpAB0=({struct Cyc_Absyn_Switch_clause*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));({struct Cyc_Absyn_Pat*_tmpAAF=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DB->pattern=_tmpAAF;}),_tmp5DB->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAAE=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5DB->where_clause=_tmpAAE;}),({
struct Cyc_Absyn_Stmt*_tmpAAD=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp5DB->body=_tmpAAD;}),_tmp5DB->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DB;});
# 2546
_tmp5DC->hd=_tmpAB0;}),({
# 2548
struct Cyc_List_List*_tmpAAC=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp5DC->tl=_tmpAAC;});_tmp5DC;}));
goto _LL0;case 327U: _LL289: _LL28A:
# 2550 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));({struct Cyc_Absyn_Switch_clause*_tmpAB5=({struct Cyc_Absyn_Switch_clause*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));({struct Cyc_Absyn_Pat*_tmpAB4=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DD->pattern=_tmpAB4;}),_tmp5DD->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5DD->where_clause=_tmpAB3;}),({struct Cyc_Absyn_Stmt*_tmpAB2=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp5DD->body=_tmpAB2;}),_tmp5DD->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DD;});_tmp5DE->hd=_tmpAB5;}),({struct Cyc_List_List*_tmpAB1=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp5DE->tl=_tmpAB1;});_tmp5DE;}));
goto _LL0;case 328U: _LL28B: _LL28C:
# 2557 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAB6=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpAB7,_tmpAB6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329U: _LL28D: _LL28E:
# 2561 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAB9=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAB8=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpAB9,_tmpAB8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 330U: _LL28F: _LL290:
# 2565 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpABD=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpABC=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpABB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpABA=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2565
Cyc_Absyn_for_stmt(_tmpABD,_tmpABC,_tmpABB,_tmpABA,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2567
goto _LL0;case 331U: _LL291: _LL292:
# 2568 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC0=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpABF=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpABE=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2568
Cyc_Absyn_for_stmt(_tmpAC1,_tmpAC0,_tmpABF,_tmpABE,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2570
goto _LL0;case 332U: _LL293: _LL294:
# 2571 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC5=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC4=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAC3=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAC2=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2571
Cyc_Absyn_for_stmt(_tmpAC5,_tmpAC4,_tmpAC3,_tmpAC2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2573
goto _LL0;case 333U: _LL295: _LL296:
# 2574 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC8=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAC6=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2574
Cyc_Absyn_for_stmt(_tmpAC9,_tmpAC8,_tmpAC7,_tmpAC6,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2576
goto _LL0;case 334U: _LL297: _LL298:
# 2577 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpACD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpACC=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpACA=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2577
Cyc_Absyn_for_stmt(_tmpACD,_tmpACC,_tmpACB,_tmpACA,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2579
goto _LL0;case 335U: _LL299: _LL29A:
# 2580 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD0=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACF=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpACE=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2580
Cyc_Absyn_for_stmt(_tmpAD1,_tmpAD0,_tmpACF,_tmpACE,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2582
goto _LL0;case 336U: _LL29B: _LL29C:
# 2583 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAD3=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD2=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2583
Cyc_Absyn_for_stmt(_tmpAD5,_tmpAD4,_tmpAD3,_tmpAD2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2585
goto _LL0;case 337U: _LL29D: _LL29E:
# 2586 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpAD6=
Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);
# 2586
Cyc_Absyn_for_stmt(_tmpAD9,_tmpAD8,_tmpAD7,_tmpAD6,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2588
goto _LL0;case 338U: _LL29F: _LL2A0: {
# 2589 "parse.y"
struct Cyc_List_List*_tmp5DF=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5DF;
struct Cyc_Absyn_Stmt*_tmp5E0=({struct Cyc_Absyn_Exp*_tmpADD=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpADC=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpADB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADA=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2590
Cyc_Absyn_for_stmt(_tmpADD,_tmpADC,_tmpADB,_tmpADA,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E0;
# 2592
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2594
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2595 "parse.y"
struct Cyc_List_List*_tmp5E1=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5E1;
struct Cyc_Absyn_Stmt*_tmp5E2=({struct Cyc_Absyn_Exp*_tmpAE1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE0=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpADF=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADE=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2596
Cyc_Absyn_for_stmt(_tmpAE1,_tmpAE0,_tmpADF,_tmpADE,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E2;
# 2598
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2600
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2601 "parse.y"
struct Cyc_List_List*_tmp5E3=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5E3;
struct Cyc_Absyn_Stmt*_tmp5E4=({struct Cyc_Absyn_Exp*_tmpAE5=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE4=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAE3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAE2=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2602
Cyc_Absyn_for_stmt(_tmpAE5,_tmpAE4,_tmpAE3,_tmpAE2,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E4;
# 2604
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2606
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2607 "parse.y"
struct Cyc_List_List*_tmp5E5=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5E5;
struct Cyc_Absyn_Stmt*_tmp5E6=({struct Cyc_Absyn_Exp*_tmpAE9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE8=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAE7=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAE6=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2608
Cyc_Absyn_for_stmt(_tmpAE9,_tmpAE8,_tmpAE7,_tmpAE6,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E6;
# 2610
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2612
goto _LL0;}case 342U: _LL2A7: _LL2A8:
# 2617 "parse.y"
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpAEB=({struct _fat_ptr*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));({struct _fat_ptr _tmpAEA=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5E7=_tmpAEA;});_tmp5E7;});Cyc_Absyn_goto_stmt(_tmpAEB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 343U: _LL2A9: _LL2AA:
# 2618 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 344U: _LL2AB: _LL2AC:
# 2619 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2620 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2621 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAEC=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpAEC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2623 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2624 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2626 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAED=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpAED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2635 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2638
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2640 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF0=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAEF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAEE=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpAF0,_tmpAEF,_tmpAEE,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2643
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2645 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpAF2,_tmpAF1,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2648
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2650 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF4=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpAF4,_tmpAF3,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2653
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2655 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF6=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpAF6,_tmpAF5,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2658
 yyval=(yyyvsp[0]).v;
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2660 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpAF8,_tmpAF7,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2663
 yyval=(yyyvsp[0]).v;
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2665 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpAFA,_tmpAF9,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2668
 yyval=(yyyvsp[0]).v;
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2670 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFC=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpAFC,_tmpAFB,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2672 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFE=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpAFE,_tmpAFD,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2675
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2677 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB00=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB00,_tmpAFF,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2679 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB02=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB01=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB02,_tmpB01,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2681 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB04=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB03=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB04,_tmpB03,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2683 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB06=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB05=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB06,_tmpB05,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2686
 yyval=(yyyvsp[0]).v;
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2688 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB08=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB07=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB08,_tmpB07,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2690 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0A=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB09=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB0A,_tmpB09,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2693
 yyval=(yyyvsp[0]).v;
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2695 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB0C,_tmpB0B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2697 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB0E,_tmpB0D,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 377U: _LL2ED: _LL2EE:
# 2700
 yyval=(yyyvsp[0]).v;
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2702 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB10=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB10,_tmpB0F,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2704 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB12=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB11=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB12,_tmpB11,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2706 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB14=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB13=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB14,_tmpB13,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 381U: _LL2F5: _LL2F6:
# 2709
 yyval=(yyyvsp[0]).v;
goto _LL0;case 382U: _LL2F7: _LL2F8: {
# 2711 "parse.y"
void*_tmp5E8=({struct _tuple8*_tmpB15=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB15,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5E8;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB17=t;struct Cyc_Absyn_Exp*_tmpB16=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB17,_tmpB16,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2714
goto _LL0;}case 383U: _LL2F9: _LL2FA:
# 2717 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 384U: _LL2FB: _LL2FC:
# 2720
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 385U: _LL2FD: _LL2FE:
# 2722 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpB19=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB18=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB19,_tmpB18,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 386U: _LL2FF: _LL300: {
# 2724 "parse.y"
void*_tmp5E9=({struct _tuple8*_tmpB1A=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB1A,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5E9;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
# 2727
goto _LL0;}case 387U: _LL301: _LL302:
# 2728 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1B=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB1B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 388U: _LL303: _LL304:
# 2730 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB1D=(*Cyc_yyget_YY37(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpB1C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB1D,_tmpB1C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 389U: _LL305: _LL306:
# 2735 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 390U: _LL307: _LL308:
# 2743 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 391U: _LL309: _LL30A:
# 2748 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 392U: _LL30B: _LL30C:
# 2750 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 393U: _LL30D: _LL30E: {
# 2752 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp5EA=e->r;void*_stmttmp2B=_tmp5EA;void*_tmp5EB=_stmttmp2B;int _tmp5ED;struct _fat_ptr _tmp5EC;int _tmp5EF;enum Cyc_Absyn_Sign _tmp5EE;short _tmp5F1;enum Cyc_Absyn_Sign _tmp5F0;char _tmp5F3;enum Cyc_Absyn_Sign _tmp5F2;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).LongLong_c).tag){case 2U: _LL499: _tmp5F2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Char_c).val).f1;_tmp5F3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Char_c).val).f2;_LL49A: {enum Cyc_Absyn_Sign s=_tmp5F2;char i=_tmp5F3;
# 2756
yyval=Cyc_YY9(({void*_tmpB1E=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->tag=11U,_tmp5F4->f1=i;_tmp5F4;});Cyc_Absyn_new_pat(_tmpB1E,e->loc);}));goto _LL498;}case 4U: _LL49B: _tmp5F0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Short_c).val).f1;_tmp5F1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Short_c).val).f2;_LL49C: {enum Cyc_Absyn_Sign s=_tmp5F0;short i=_tmp5F1;
# 2758
yyval=Cyc_YY9(({void*_tmpB1F=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=10U,_tmp5F5->f1=s,_tmp5F5->f2=(int)i;_tmp5F5;});Cyc_Absyn_new_pat(_tmpB1F,e->loc);}));goto _LL498;}case 5U: _LL49D: _tmp5EE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Int_c).val).f1;_tmp5EF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Int_c).val).f2;_LL49E: {enum Cyc_Absyn_Sign s=_tmp5EE;int i=_tmp5EF;
# 2760
yyval=Cyc_YY9(({void*_tmpB20=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=10U,_tmp5F6->f1=s,_tmp5F6->f2=i;_tmp5F6;});Cyc_Absyn_new_pat(_tmpB20,e->loc);}));goto _LL498;}case 7U: _LL49F: _tmp5EC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Float_c).val).f1;_tmp5ED=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EB)->f1).Float_c).val).f2;_LL4A0: {struct _fat_ptr s=_tmp5EC;int i=_tmp5ED;
# 2762
yyval=Cyc_YY9(({void*_tmpB21=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->tag=12U,_tmp5F7->f1=s,_tmp5F7->f2=i;_tmp5F7;});Cyc_Absyn_new_pat(_tmpB21,e->loc);}));goto _LL498;}case 1U: _LL4A1: _LL4A2:
# 2764
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL498;case 8U: _LL4A3: _LL4A4:
# 2766
({void*_tmp5F8=0U;({unsigned _tmpB23=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB22=({const char*_tmp5F9="strings cannot occur within patterns";_tag_fat(_tmp5F9,sizeof(char),37U);});Cyc_Warn_err(_tmpB23,_tmpB22,_tag_fat(_tmp5F8,sizeof(void*),0U));});});goto _LL498;case 9U: _LL4A5: _LL4A6:
# 2768
({void*_tmp5FA=0U;({unsigned _tmpB25=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB24=({const char*_tmp5FB="strings cannot occur within patterns";_tag_fat(_tmp5FB,sizeof(char),37U);});Cyc_Warn_err(_tmpB25,_tmpB24,_tag_fat(_tmp5FA,sizeof(void*),0U));});});goto _LL498;case 6U: _LL4A7: _LL4A8:
# 2770
({void*_tmp5FC=0U;({unsigned _tmpB27=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB26=({const char*_tmp5FD="long long's in patterns not yet implemented";_tag_fat(_tmp5FD,sizeof(char),44U);});Cyc_Warn_err(_tmpB27,_tmpB26,_tag_fat(_tmp5FC,sizeof(void*),0U));});});goto _LL498;default: goto _LL4A9;}else{_LL4A9: _LL4AA:
# 2772
({void*_tmp5FE=0U;({unsigned _tmpB29=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB28=({const char*_tmp5FF="bad constant in case";_tag_fat(_tmp5FF,sizeof(char),21U);});Cyc_Warn_err(_tmpB29,_tmpB28,_tag_fat(_tmp5FE,sizeof(void*),0U));});});}_LL498:;}
# 2775
goto _LL0;}case 394U: _LL30F: _LL310:
# 2776 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB2B=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=15U,({struct _tuple0*_tmpB2A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp600->f1=_tmpB2A;});_tmp600;});Cyc_Absyn_new_pat(_tmpB2B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 395U: _LL311: _LL312:
# 2778 "parse.y"
 if(({struct _fat_ptr _tmpB2C=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpB2C,({const char*_tmp601="as";_tag_fat(_tmp601,sizeof(char),3U);}));})!= 0)
({void*_tmp602=0U;({unsigned _tmpB2E=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB2D=({const char*_tmp603="expecting `as'";_tag_fat(_tmp603,sizeof(char),15U);});Cyc_Warn_err(_tmpB2E,_tmpB2D,_tag_fat(_tmp602,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB35=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB34=({unsigned _tmpB33=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB32=({struct _tuple0*_tmp605=_cycalloc(sizeof(*_tmp605));_tmp605->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB31=({struct _fat_ptr*_tmp604=_cycalloc(sizeof(*_tmp604));({struct _fat_ptr _tmpB30=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp604=_tmpB30;});_tmp604;});_tmp605->f2=_tmpB31;});_tmp605;});Cyc_Absyn_new_vardecl(_tmpB33,_tmpB32,Cyc_Absyn_void_type,0);});_tmp606->f1=_tmpB34;}),({
struct Cyc_Absyn_Pat*_tmpB2F=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp606->f2=_tmpB2F;});_tmp606;});
# 2780
Cyc_Absyn_new_pat(_tmpB35,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2783
goto _LL0;case 396U: _LL313: _LL314:
# 2784 "parse.y"
 if(({struct _fat_ptr _tmpB36=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB36,({const char*_tmp607="alias";_tag_fat(_tmp607,sizeof(char),6U);}));})!= 0)
({void*_tmp608=0U;({unsigned _tmpB38=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB37=({const char*_tmp609="expecting `alias'";_tag_fat(_tmp609,sizeof(char),18U);});Cyc_Warn_err(_tmpB38,_tmpB37,_tag_fat(_tmp608,sizeof(void*),0U));});});{
int _tmp60A=((yyyvsp[0]).l).first_line;int location=_tmp60A;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp610=_cycalloc(sizeof(*_tmp610));({struct _fat_ptr*_tmpB3B=({struct _fat_ptr*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct _fat_ptr _tmpB3A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60E=_tmpB3A;});_tmp60E;});_tmp610->name=_tmpB3B;}),_tmp610->identity=- 1,({void*_tmpB39=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->tag=0U,_tmp60F->f1=& Cyc_Tcutil_rk;_tmp60F;});_tmp610->kind=_tmpB39;});_tmp610;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB40=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB3F=({struct _tuple0*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB3D=({struct _fat_ptr*_tmp60C=_cycalloc(sizeof(*_tmp60C));({struct _fat_ptr _tmpB3C=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp60C=_tmpB3C;});_tmp60C;});_tmp60D->f2=_tmpB3D;});_tmp60D;});Cyc_Absyn_new_vardecl(_tmpB40,_tmpB3F,({
struct _tuple8*_tmpB3E=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB3E,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB41=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->tag=2U,_tmp60B->f1=tv,_tmp60B->f2=vd;_tmp60B;});Cyc_Absyn_new_pat(_tmpB41,(unsigned)location);}));
# 2792
goto _LL0;}case 397U: _LL315: _LL316:
# 2793 "parse.y"
 if(({struct _fat_ptr _tmpB42=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB42,({const char*_tmp611="alias";_tag_fat(_tmp611,sizeof(char),6U);}));})!= 0)
({void*_tmp612=0U;({unsigned _tmpB44=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB43=({const char*_tmp613="expecting `alias'";_tag_fat(_tmp613,sizeof(char),18U);});Cyc_Warn_err(_tmpB44,_tmpB43,_tag_fat(_tmp612,sizeof(void*),0U));});});{
int _tmp614=((yyyvsp[0]).l).first_line;int location=_tmp614;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp61A=_cycalloc(sizeof(*_tmp61A));({struct _fat_ptr*_tmpB47=({struct _fat_ptr*_tmp618=_cycalloc(sizeof(*_tmp618));({struct _fat_ptr _tmpB46=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp618=_tmpB46;});_tmp618;});_tmp61A->name=_tmpB47;}),_tmp61A->identity=- 1,({void*_tmpB45=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=0U,_tmp619->f1=& Cyc_Tcutil_rk;_tmp619;});_tmp61A->kind=_tmpB45;});_tmp61A;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB4C=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB4B=({struct _tuple0*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB49=({struct _fat_ptr*_tmp616=_cycalloc(sizeof(*_tmp616));({struct _fat_ptr _tmpB48=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp616=_tmpB48;});_tmp616;});_tmp617->f2=_tmpB49;});_tmp617;});Cyc_Absyn_new_vardecl(_tmpB4C,_tmpB4B,({
struct _tuple8*_tmpB4A=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB4A,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB4D=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=2U,_tmp615->f1=tv,_tmp615->f2=vd;_tmp615;});Cyc_Absyn_new_pat(_tmpB4D,(unsigned)location);}));
# 2801
goto _LL0;}case 398U: _LL317: _LL318: {
# 2802 "parse.y"
struct _tuple23 _tmp61B=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp61B;struct _tuple23 _tmp61C=_stmttmp2C;int _tmp61E;struct Cyc_List_List*_tmp61D;_LL4AC: _tmp61D=_tmp61C.f1;_tmp61E=_tmp61C.f2;_LL4AD: {struct Cyc_List_List*ps=_tmp61D;int dots=_tmp61E;
yyval=Cyc_YY9(({void*_tmpB4E=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->tag=5U,_tmp61F->f1=ps,_tmp61F->f2=dots;_tmp61F;});Cyc_Absyn_new_pat(_tmpB4E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2805
goto _LL0;}}case 399U: _LL319: _LL31A: {
# 2806 "parse.y"
struct _tuple23 _tmp620=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2D=_tmp620;struct _tuple23 _tmp621=_stmttmp2D;int _tmp623;struct Cyc_List_List*_tmp622;_LL4AF: _tmp622=_tmp621.f1;_tmp623=_tmp621.f2;_LL4B0: {struct Cyc_List_List*ps=_tmp622;int dots=_tmp623;
yyval=Cyc_YY9(({void*_tmpB50=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=16U,({struct _tuple0*_tmpB4F=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp624->f1=_tmpB4F;}),_tmp624->f2=ps,_tmp624->f3=dots;_tmp624;});Cyc_Absyn_new_pat(_tmpB50,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2809
goto _LL0;}}case 400U: _LL31B: _LL31C: {
# 2810 "parse.y"
struct _tuple23 _tmp625=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2E=_tmp625;struct _tuple23 _tmp626=_stmttmp2E;int _tmp628;struct Cyc_List_List*_tmp627;_LL4B2: _tmp627=_tmp626.f1;_tmp628=_tmp626.f2;_LL4B3: {struct Cyc_List_List*fps=_tmp627;int dots=_tmp628;
struct Cyc_List_List*_tmp629=({unsigned _tmpB51=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB51,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp629;
yyval=Cyc_YY9(({void*_tmpB54=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB53=({union Cyc_Absyn_AggrInfo*_tmp62A=_cycalloc(sizeof(*_tmp62A));({union Cyc_Absyn_AggrInfo _tmpB52=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp62A=_tmpB52;});_tmp62A;});_tmp62B->f1=_tmpB53;}),_tmp62B->f2=exist_ts,_tmp62B->f3=fps,_tmp62B->f4=dots;_tmp62B;});Cyc_Absyn_new_pat(_tmpB54,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2815
goto _LL0;}}case 401U: _LL31D: _LL31E: {
# 2816 "parse.y"
struct _tuple23 _tmp62C=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2F=_tmp62C;struct _tuple23 _tmp62D=_stmttmp2F;int _tmp62F;struct Cyc_List_List*_tmp62E;_LL4B5: _tmp62E=_tmp62D.f1;_tmp62F=_tmp62D.f2;_LL4B6: {struct Cyc_List_List*fps=_tmp62E;int dots=_tmp62F;
struct Cyc_List_List*_tmp630=({unsigned _tmpB55=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB55,Cyc_yyget_YY40(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp630;
yyval=Cyc_YY9(({void*_tmpB56=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=7U,_tmp631->f1=0,_tmp631->f2=exist_ts,_tmp631->f3=fps,_tmp631->f4=dots;_tmp631;});Cyc_Absyn_new_pat(_tmpB56,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2820
goto _LL0;}}case 402U: _LL31F: _LL320:
# 2821 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB58=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=6U,({struct Cyc_Absyn_Pat*_tmpB57=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp632->f1=_tmpB57;});_tmp632;});Cyc_Absyn_new_pat(_tmpB58,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 403U: _LL321: _LL322:
# 2823 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB5C=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=6U,({struct Cyc_Absyn_Pat*_tmpB5B=({void*_tmpB5A=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->tag=6U,({struct Cyc_Absyn_Pat*_tmpB59=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp633->f1=_tmpB59;});_tmp633;});Cyc_Absyn_new_pat(_tmpB5A,(unsigned)((yyyvsp[0]).l).first_line);});_tmp634->f1=_tmpB5B;});_tmp634;});Cyc_Absyn_new_pat(_tmpB5C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404U: _LL323: _LL324:
# 2825 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB63=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB62=({unsigned _tmpB61=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB60=({struct _tuple0*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB5F=({struct _fat_ptr*_tmp635=_cycalloc(sizeof(*_tmp635));({struct _fat_ptr _tmpB5E=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp635=_tmpB5E;});_tmp635;});_tmp636->f2=_tmpB5F;});_tmp636;});Cyc_Absyn_new_vardecl(_tmpB61,_tmpB60,Cyc_Absyn_void_type,0);});_tmp637->f1=_tmpB62;}),({
# 2827
struct Cyc_Absyn_Pat*_tmpB5D=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp637->f2=_tmpB5D;});_tmp637;});
# 2825
Cyc_Absyn_new_pat(_tmpB63,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2829
goto _LL0;case 405U: _LL325: _LL326:
# 2830 "parse.y"
 if(({struct _fat_ptr _tmpB64=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpB64,({const char*_tmp638="as";_tag_fat(_tmp638,sizeof(char),3U);}));})!= 0)
({void*_tmp639=0U;({unsigned _tmpB66=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpB65=({const char*_tmp63A="expecting `as'";_tag_fat(_tmp63A,sizeof(char),15U);});Cyc_Warn_err(_tmpB66,_tmpB65,_tag_fat(_tmp639,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB6D=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB6C=({unsigned _tmpB6B=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB6A=({struct _tuple0*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB69=({struct _fat_ptr*_tmp63B=_cycalloc(sizeof(*_tmp63B));({struct _fat_ptr _tmpB68=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp63B=_tmpB68;});_tmp63B;});_tmp63C->f2=_tmpB69;});_tmp63C;});Cyc_Absyn_new_vardecl(_tmpB6B,_tmpB6A,Cyc_Absyn_void_type,0);});_tmp63D->f1=_tmpB6C;}),({
# 2834
struct Cyc_Absyn_Pat*_tmpB67=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp63D->f2=_tmpB67;});_tmp63D;});
# 2832
Cyc_Absyn_new_pat(_tmpB6D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2836
goto _LL0;case 406U: _LL327: _LL328: {
# 2837 "parse.y"
void*_tmp63E=({struct _fat_ptr _tmpB6E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpB6E,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});void*tag=_tmp63E;
yyval=Cyc_YY9(({void*_tmpB75=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB74=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp641->f1=_tmpB74;}),({
struct Cyc_Absyn_Vardecl*_tmpB73=({unsigned _tmpB72=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB71=({struct _tuple0*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB70=({struct _fat_ptr*_tmp63F=_cycalloc(sizeof(*_tmp63F));({struct _fat_ptr _tmpB6F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp63F=_tmpB6F;});_tmp63F;});_tmp640->f2=_tmpB70;});_tmp640;});Cyc_Absyn_new_vardecl(_tmpB72,_tmpB71,
Cyc_Absyn_tag_type(tag),0);});
# 2839
_tmp641->f2=_tmpB73;});_tmp641;});
# 2838
Cyc_Absyn_new_pat(_tmpB75,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2842
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2843 "parse.y"
struct Cyc_Absyn_Tvar*_tmp642=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));struct Cyc_Absyn_Tvar*tv=_tmp642;
yyval=Cyc_YY9(({void*_tmpB7B=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->tag=4U,_tmp645->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpB7A=({unsigned _tmpB79=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB78=({struct _tuple0*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB77=({struct _fat_ptr*_tmp643=_cycalloc(sizeof(*_tmp643));({struct _fat_ptr _tmpB76=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp643=_tmpB76;});_tmp643;});_tmp644->f2=_tmpB77;});_tmp644;});Cyc_Absyn_new_vardecl(_tmpB79,_tmpB78,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2845
_tmp645->f2=_tmpB7A;});_tmp645;});
# 2844
Cyc_Absyn_new_pat(_tmpB7B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2848
goto _LL0;}case 408U: _LL32B: _LL32C:
# 2851 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp646=_cycalloc(sizeof(*_tmp646));({struct Cyc_List_List*_tmpB7C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp646->f1=_tmpB7C;}),_tmp646->f2=0;_tmp646;}));
goto _LL0;case 409U: _LL32D: _LL32E:
# 2852 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp647=_cycalloc(sizeof(*_tmp647));({struct Cyc_List_List*_tmpB7D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp647->f1=_tmpB7D;}),_tmp647->f2=1;_tmp647;}));
goto _LL0;case 410U: _LL32F: _LL330:
# 2853 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->f1=0,_tmp648->f2=1;_tmp648;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2858 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp649=_cycalloc(sizeof(*_tmp649));({struct Cyc_Absyn_Pat*_tmpB7E=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp649->hd=_tmpB7E;}),_tmp649->tl=0;_tmp649;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2860 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_Absyn_Pat*_tmpB80=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp64A->hd=_tmpB80;}),({struct Cyc_List_List*_tmpB7F=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp64A->tl=_tmpB7F;});_tmp64A;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2865 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->f1=0,({struct Cyc_Absyn_Pat*_tmpB81=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp64B->f2=_tmpB81;});_tmp64B;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2867 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp64C=_cycalloc(sizeof(*_tmp64C));({struct Cyc_List_List*_tmpB83=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp64C->f1=_tmpB83;}),({struct Cyc_Absyn_Pat*_tmpB82=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp64C->f2=_tmpB82;});_tmp64C;}));
goto _LL0;case 415U: _LL339: _LL33A:
# 2870
 yyval=Cyc_YY14(({struct _tuple23*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct Cyc_List_List*_tmpB84=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64D->f1=_tmpB84;}),_tmp64D->f2=0;_tmp64D;}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2871 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp64E=_cycalloc(sizeof(*_tmp64E));({struct Cyc_List_List*_tmpB85=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64E->f1=_tmpB85;}),_tmp64E->f2=1;_tmp64E;}));
goto _LL0;case 417U: _LL33D: _LL33E:
# 2872 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->f1=0,_tmp64F->f2=1;_tmp64F;}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2877 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp650=_cycalloc(sizeof(*_tmp650));({struct _tuple24*_tmpB86=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp650->hd=_tmpB86;}),_tmp650->tl=0;_tmp650;}));
goto _LL0;case 419U: _LL341: _LL342:
# 2879 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp651=_cycalloc(sizeof(*_tmp651));({struct _tuple24*_tmpB88=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp651->hd=_tmpB88;}),({struct Cyc_List_List*_tmpB87=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp651->tl=_tmpB87;});_tmp651;}));
goto _LL0;case 420U: _LL343: _LL344:
# 2885 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 421U: _LL345: _LL346:
# 2887 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB89=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpB8A,_tmpB89,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422U: _LL347: _LL348:
# 2892 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 423U: _LL349: _LL34A:
# 2894 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpB8C=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpB8B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpB8D,_tmpB8C,_tmpB8B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424U: _LL34B: _LL34C:
# 2896 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB8E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpB8F,_tmpB8E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425U: _LL34D: _LL34E:
# 2900 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 426U: _LL34F: _LL350:
# 2901 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->v=(void*)Cyc_Absyn_Times;_tmp652;}));
goto _LL0;case 427U: _LL351: _LL352:
# 2902 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->v=(void*)Cyc_Absyn_Div;_tmp653;}));
goto _LL0;case 428U: _LL353: _LL354:
# 2903 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->v=(void*)Cyc_Absyn_Mod;_tmp654;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2904 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)Cyc_Absyn_Plus;_tmp655;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2905 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->v=(void*)Cyc_Absyn_Minus;_tmp656;}));
goto _LL0;case 431U: _LL359: _LL35A:
# 2906 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->v=(void*)Cyc_Absyn_Bitlshift;_tmp657;}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2907 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->v=(void*)Cyc_Absyn_Bitlrshift;_tmp658;}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2908 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->v=(void*)Cyc_Absyn_Bitand;_tmp659;}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2909 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->v=(void*)Cyc_Absyn_Bitxor;_tmp65A;}));
goto _LL0;case 435U: _LL361: _LL362:
# 2910 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->v=(void*)Cyc_Absyn_Bitor;_tmp65B;}));
goto _LL0;case 436U: _LL363: _LL364:
# 2915 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437U: _LL365: _LL366:
# 2917 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB92=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB91=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpB92,_tmpB91,_tmpB90,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438U: _LL367: _LL368:
# 2920
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB93=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpB93,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 439U: _LL369: _LL36A:
# 2923
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB94=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB94,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2925 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB95=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB95,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 441U: _LL36D: _LL36E:
# 2927 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB97=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB96=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB97,_tmpB96,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2929 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB99=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB98=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB99,_tmpB98,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443U: _LL371: _LL372:
# 2933 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444U: _LL373: _LL374:
# 2937 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 445U: _LL375: _LL376:
# 2939 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpB9B,_tmpB9A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 446U: _LL377: _LL378:
# 2943 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 447U: _LL379: _LL37A:
# 2945 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpB9D,_tmpB9C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 448U: _LL37B: _LL37C:
# 2949 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 449U: _LL37D: _LL37E:
# 2951 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB9F,_tmpB9E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 450U: _LL37F: _LL380:
# 2955 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 451U: _LL381: _LL382:
# 2957 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpBA1,_tmpBA0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 452U: _LL383: _LL384:
# 2961 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 453U: _LL385: _LL386:
# 2963 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpBA3,_tmpBA2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 454U: _LL387: _LL388:
# 2967 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 455U: _LL389: _LL38A:
# 2969 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA5=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpBA5,_tmpBA4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 456U: _LL38B: _LL38C:
# 2971 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA7=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpBA7,_tmpBA6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 457U: _LL38D: _LL38E:
# 2975 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 458U: _LL38F: _LL390:
# 2977 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpBA9,_tmpBA8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 459U: _LL391: _LL392:
# 2979 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpBAB,_tmpBAA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 460U: _LL393: _LL394:
# 2981 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpBAD,_tmpBAC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2983 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpBAF,_tmpBAE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2987 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 463U: _LL399: _LL39A:
# 2989 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpBB1,_tmpBB0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464U: _LL39B: _LL39C:
# 2991 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpBB3,_tmpBB2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465U: _LL39D: _LL39E:
# 2995 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2997 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpBB5,_tmpBB4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 2999 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpBB7,_tmpBB6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468U: _LL3A3: _LL3A4:
# 3003 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 3005 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpBB9,_tmpBB8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 3007 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBB=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpBBB,_tmpBBA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 3009 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBD=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpBBD,_tmpBBC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 3013 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 473U: _LL3AD: _LL3AE: {
# 3015 "parse.y"
void*_tmp65C=({struct _tuple8*_tmpBBE=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpBBE,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp65C;
yyval=Cyc_Exp_tok(({void*_tmpBC0=t;struct Cyc_Absyn_Exp*_tmpBBF=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpBC0,_tmpBBF,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3018
goto _LL0;}case 474U: _LL3AF: _LL3B0:
# 3021 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 475U: _LL3B1: _LL3B2:
# 3022 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC1=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC1,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476U: _LL3B3: _LL3B4:
# 3023 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC2,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 3024 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpBC3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 478U: _LL3B7: _LL3B8:
# 3025 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpBC4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 3026 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpBC5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 3027 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpBC7=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC6=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpBC7,_tmpBC6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481U: _LL3BD: _LL3BE: {
# 3029 "parse.y"
void*_tmp65D=({struct _tuple8*_tmpBC8=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBC8,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65D;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 3032
goto _LL0;}case 482U: _LL3BF: _LL3C0:
# 3032 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC9=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpBC9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 483U: _LL3C1: _LL3C2: {
# 3034 "parse.y"
void*_tmp65E=({struct _tuple8*_tmpBCA=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBCA,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65E;
yyval=Cyc_Exp_tok(({void*_tmpBCC=t;struct Cyc_List_List*_tmpBCB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpBCC,_tmpBCB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3037
goto _LL0;}case 484U: _LL3C3: _LL3C4:
# 3039
 yyval=Cyc_Exp_tok(({void*_tmpBCE=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->tag=34U,(_tmp65F->f1).is_calloc=0,(_tmp65F->f1).rgn=0,(_tmp65F->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBCD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp65F->f1).num_elts=_tmpBCD;}),(_tmp65F->f1).fat_result=0,(_tmp65F->f1).inline_call=0;_tmp65F;});Cyc_Absyn_new_exp(_tmpBCE,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3041
goto _LL0;case 485U: _LL3C5: _LL3C6:
# 3042 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBD1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->tag=34U,(_tmp660->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBD0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp660->f1).rgn=_tmpBD0;}),(_tmp660->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBCF=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp660->f1).num_elts=_tmpBCF;}),(_tmp660->f1).fat_result=0,(_tmp660->f1).inline_call=0;_tmp660;});Cyc_Absyn_new_exp(_tmpBD1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3044
goto _LL0;case 486U: _LL3C7: _LL3C8:
# 3045 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBD4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=34U,(_tmp661->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBD3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp661->f1).rgn=_tmpBD3;}),(_tmp661->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD2=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp661->f1).num_elts=_tmpBD2;}),(_tmp661->f1).fat_result=0,(_tmp661->f1).inline_call=1;_tmp661;});Cyc_Absyn_new_exp(_tmpBD4,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3047
goto _LL0;case 487U: _LL3C9: _LL3CA: {
# 3048 "parse.y"
void*_tmp662=({struct _tuple8*_tmpBD5=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpBD5,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp662;
yyval=Cyc_Exp_tok(({void*_tmpBD8=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->tag=34U,(_tmp664->f1).is_calloc=1,(_tmp664->f1).rgn=0,({void**_tmpBD7=({void**_tmp663=_cycalloc(sizeof(*_tmp663));*_tmp663=t;_tmp663;});(_tmp664->f1).elt_type=_tmpBD7;}),({struct Cyc_Absyn_Exp*_tmpBD6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp664->f1).num_elts=_tmpBD6;}),(_tmp664->f1).fat_result=0,(_tmp664->f1).inline_call=0;_tmp664;});Cyc_Absyn_new_exp(_tmpBD8,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3051
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3053
void*_tmp665=({struct _tuple8*_tmpBD9=Cyc_yyget_YY37(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpBD9,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp665;
yyval=Cyc_Exp_tok(({void*_tmpBDD=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tag=34U,(_tmp667->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBDC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp667->f1).rgn=_tmpBDC;}),({void**_tmpBDB=({void**_tmp666=_cycalloc(sizeof(*_tmp666));*_tmp666=t;_tmp666;});(_tmp667->f1).elt_type=_tmpBDB;}),({struct Cyc_Absyn_Exp*_tmpBDA=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp667->f1).num_elts=_tmpBDA;}),(_tmp667->f1).fat_result=0,(_tmp667->f1).inline_call=0;_tmp667;});Cyc_Absyn_new_exp(_tmpBDD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3056
goto _LL0;}case 489U: _LL3CD: _LL3CE:
# 3057 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBDF=({struct Cyc_Absyn_Exp*_tmp668[1U];({struct Cyc_Absyn_Exp*_tmpBDE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp668[0]=_tmpBDE;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp668,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBDF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 490U: _LL3CF: _LL3D0:
# 3059 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE3=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->tag=38U,({struct Cyc_Absyn_Exp*_tmpBE2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp66A->f1=_tmpBE2;}),({struct _fat_ptr*_tmpBE1=({struct _fat_ptr*_tmp669=_cycalloc(sizeof(*_tmp669));({struct _fat_ptr _tmpBE0=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp669=_tmpBE0;});_tmp669;});_tmp66A->f2=_tmpBE1;});_tmp66A;});Cyc_Absyn_new_exp(_tmpBE3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 491U: _LL3D1: _LL3D2:
# 3061 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE8=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->tag=38U,({struct Cyc_Absyn_Exp*_tmpBE7=({struct Cyc_Absyn_Exp*_tmpBE6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpBE6,(unsigned)((yyyvsp[2]).l).first_line);});_tmp66C->f1=_tmpBE7;}),({struct _fat_ptr*_tmpBE5=({struct _fat_ptr*_tmp66B=_cycalloc(sizeof(*_tmp66B));({struct _fat_ptr _tmpBE4=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp66B=_tmpBE4;});_tmp66B;});_tmp66C->f2=_tmpBE5;});_tmp66C;});Cyc_Absyn_new_exp(_tmpBE8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 492U: _LL3D3: _LL3D4: {
# 3063 "parse.y"
void*_tmp66D=({struct _tuple8*_tmpBE9=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBE9,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp66D;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 493U: _LL3D5: _LL3D6:
# 3066 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBEA=Cyc_yyget_YY58(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpBEA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 494U: _LL3D7: _LL3D8:
# 3067 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBEB=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpBEB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 495U: _LL3D9: _LL3DA: {
# 3072 "parse.y"
struct _tuple29*_tmp66E=Cyc_yyget_YY59(&(yyyvsp[3]).v);struct _tuple29*_stmttmp30=_tmp66E;struct _tuple29*_tmp66F=_stmttmp30;struct Cyc_List_List*_tmp672;struct Cyc_List_List*_tmp671;struct Cyc_List_List*_tmp670;_LL4B8: _tmp670=_tmp66F->f1;_tmp671=_tmp66F->f2;_tmp672=_tmp66F->f3;_LL4B9: {struct Cyc_List_List*outlist=_tmp670;struct Cyc_List_List*inlist=_tmp671;struct Cyc_List_List*clobbers=_tmp672;
yyval=Cyc_YY58((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->tag=40U,({int _tmpBED=Cyc_yyget_YY31(&(yyyvsp[0]).v);_tmp673->f1=_tmpBED;}),({struct _fat_ptr _tmpBEC=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp673->f2=_tmpBEC;}),_tmp673->f3=outlist,_tmp673->f4=inlist,_tmp673->f5=clobbers;_tmp673;}));
goto _LL0;}}case 496U: _LL3DB: _LL3DC:
# 3077 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 497U: _LL3DD: _LL3DE:
# 3078 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 498U: _LL3DF: _LL3E0:
# 3082 "parse.y"
 yyval=Cyc_YY59(({struct _tuple29*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->f1=0,_tmp674->f2=0,_tmp674->f3=0;_tmp674;}));
goto _LL0;case 499U: _LL3E1: _LL3E2: {
# 3084 "parse.y"
struct _tuple30*_tmp675=Cyc_yyget_YY60(&(yyyvsp[1]).v);struct _tuple30*_stmttmp31=_tmp675;struct _tuple30*_tmp676=_stmttmp31;struct Cyc_List_List*_tmp678;struct Cyc_List_List*_tmp677;_LL4BB: _tmp677=_tmp676->f1;_tmp678=_tmp676->f2;_LL4BC: {struct Cyc_List_List*inlist=_tmp677;struct Cyc_List_List*clobbers=_tmp678;
yyval=Cyc_YY59(({struct _tuple29*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->f1=0,_tmp679->f2=inlist,_tmp679->f3=clobbers;_tmp679;}));
goto _LL0;}}case 500U: _LL3E3: _LL3E4: {
# 3087 "parse.y"
struct _tuple30*_tmp67A=Cyc_yyget_YY60(&(yyyvsp[2]).v);struct _tuple30*_stmttmp32=_tmp67A;struct _tuple30*_tmp67B=_stmttmp32;struct Cyc_List_List*_tmp67D;struct Cyc_List_List*_tmp67C;_LL4BE: _tmp67C=_tmp67B->f1;_tmp67D=_tmp67B->f2;_LL4BF: {struct Cyc_List_List*inlist=_tmp67C;struct Cyc_List_List*clobbers=_tmp67D;
yyval=Cyc_YY59(({struct _tuple29*_tmp67E=_cycalloc(sizeof(*_tmp67E));({struct Cyc_List_List*_tmpBEE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp67E->f1=_tmpBEE;}),_tmp67E->f2=inlist,_tmp67E->f3=clobbers;_tmp67E;}));
goto _LL0;}}case 501U: _LL3E5: _LL3E6:
# 3092 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct _tuple31*_tmpBEF=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp67F->hd=_tmpBEF;}),_tmp67F->tl=0;_tmp67F;}));
goto _LL0;case 502U: _LL3E7: _LL3E8:
# 3093 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp680=_cycalloc(sizeof(*_tmp680));({struct _tuple31*_tmpBF1=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp680->hd=_tmpBF1;}),({struct Cyc_List_List*_tmpBF0=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp680->tl=_tmpBF0;});_tmp680;}));
goto _LL0;case 503U: _LL3E9: _LL3EA:
# 3097 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681->f1=0,_tmp681->f2=0;_tmp681;}));
goto _LL0;case 504U: _LL3EB: _LL3EC:
# 3099 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->f1=0,({struct Cyc_List_List*_tmpBF2=Cyc_yyget_YY61(&(yyyvsp[1]).v);_tmp682->f2=_tmpBF2;});_tmp682;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 3101 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp683=_cycalloc(sizeof(*_tmp683));({struct Cyc_List_List*_tmpBF4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp683->f1=_tmpBF4;}),({struct Cyc_List_List*_tmpBF3=Cyc_yyget_YY61(&(yyyvsp[2]).v);_tmp683->f2=_tmpBF3;});_tmp683;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 3105 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp684=_cycalloc(sizeof(*_tmp684));({struct _tuple31*_tmpBF5=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp684->hd=_tmpBF5;}),_tmp684->tl=0;_tmp684;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 3106 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct _tuple31*_tmpBF7=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp685->hd=_tmpBF7;}),({struct Cyc_List_List*_tmpBF6=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp685->tl=_tmpBF6;});_tmp685;}));
goto _LL0;case 508U: _LL3F3: _LL3F4: {
# 3111 "parse.y"
struct Cyc_Absyn_Exp*_tmp686=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*pf_exp=_tmp686;
yyval=Cyc_YY63(({struct _tuple31*_tmp687=_cycalloc(sizeof(*_tmp687));({struct _fat_ptr _tmpBF9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp687->f1=_tmpBF9;}),({struct Cyc_Absyn_Exp*_tmpBF8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp687->f2=_tmpBF8;});_tmp687;}));
goto _LL0;}case 509U: _LL3F5: _LL3F6:
# 3117 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 510U: _LL3F7: _LL3F8:
# 3118 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 511U: _LL3F9: _LL3FA:
# 3119 "parse.y"
 yyval=Cyc_YY61(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY61(&(yyyvsp[1]).v)));
goto _LL0;case 512U: _LL3FB: _LL3FC:
# 3123 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp689=_cycalloc(sizeof(*_tmp689));({struct _fat_ptr*_tmpBFB=({struct _fat_ptr*_tmp688=_cycalloc(sizeof(*_tmp688));({struct _fat_ptr _tmpBFA=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp688=_tmpBFA;});_tmp688;});_tmp689->hd=_tmpBFB;}),_tmp689->tl=0;_tmp689;}));
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3124 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp68B=_cycalloc(sizeof(*_tmp68B));({struct _fat_ptr*_tmpBFE=({struct _fat_ptr*_tmp68A=_cycalloc(sizeof(*_tmp68A));({struct _fat_ptr _tmpBFD=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68A=_tmpBFD;});_tmp68A;});_tmp68B->hd=_tmpBFE;}),({struct Cyc_List_List*_tmpBFC=Cyc_yyget_YY61(&(yyyvsp[0]).v);_tmp68B->tl=_tmpBFC;});_tmp68B;}));
goto _LL0;case 514U: _LL3FF: _LL400:
# 3128 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 515U: _LL401: _LL402:
# 3129 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 516U: _LL403: _LL404:
# 3130 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 517U: _LL405: _LL406:
# 3135 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 518U: _LL407: _LL408:
# 3137 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC00=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBFF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpC00,_tmpBFF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 519U: _LL409: _LL40A:
# 3139 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC01=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpC01,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520U: _LL40B: _LL40C:
# 3141 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC03=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC02=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpC03,_tmpC02,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521U: _LL40D: _LL40E:
# 3143 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC06=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC05=({struct _fat_ptr*_tmp68C=_cycalloc(sizeof(*_tmp68C));({struct _fat_ptr _tmpC04=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68C=_tmpC04;});_tmp68C;});Cyc_Absyn_aggrmember_exp(_tmpC06,_tmpC05,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3145 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC09=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC08=({struct _fat_ptr*_tmp68D=_cycalloc(sizeof(*_tmp68D));({struct _fat_ptr _tmpC07=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68D=_tmpC07;});_tmp68D;});Cyc_Absyn_aggrarrow_exp(_tmpC09,_tmpC08,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3147 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC0A,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3149 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC0B,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 525U: _LL415: _LL416:
# 3151 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC0D=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->tag=25U,({struct _tuple8*_tmpC0C=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68E->f1=_tmpC0C;}),_tmp68E->f2=0;_tmp68E;});Cyc_Absyn_new_exp(_tmpC0D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526U: _LL417: _LL418:
# 3153 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC10=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=25U,({struct _tuple8*_tmpC0F=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68F->f1=_tmpC0F;}),({struct Cyc_List_List*_tmpC0E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp68F->f2=_tmpC0E;});_tmp68F;});Cyc_Absyn_new_exp(_tmpC10,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3155 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC13=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->tag=25U,({struct _tuple8*_tmpC12=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp690->f1=_tmpC12;}),({struct Cyc_List_List*_tmpC11=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp690->f2=_tmpC11;});_tmp690;});Cyc_Absyn_new_exp(_tmpC13,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528U: _LL41B: _LL41C:
# 3160 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp693=_cycalloc(sizeof(*_tmp693));({void*_tmpC16=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->tag=0U,({struct _fat_ptr*_tmpC15=({struct _fat_ptr*_tmp691=_cycalloc(sizeof(*_tmp691));({struct _fat_ptr _tmpC14=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp691=_tmpC14;});_tmp691;});_tmp692->f1=_tmpC15;});_tmp692;});_tmp693->hd=_tmpC16;}),_tmp693->tl=0;_tmp693;}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3163
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));({void*_tmpC19=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->tag=1U,({unsigned _tmpC18=({unsigned _tmpC17=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpC17,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp694->f1=_tmpC18;});_tmp694;});_tmp695->hd=_tmpC19;}),_tmp695->tl=0;_tmp695;}));
goto _LL0;case 530U: _LL41F: _LL420:
# 3165 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));({void*_tmpC1D=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->tag=0U,({struct _fat_ptr*_tmpC1C=({struct _fat_ptr*_tmp696=_cycalloc(sizeof(*_tmp696));({struct _fat_ptr _tmpC1B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp696=_tmpC1B;});_tmp696;});_tmp697->f1=_tmpC1C;});_tmp697;});_tmp698->hd=_tmpC1D;}),({struct Cyc_List_List*_tmpC1A=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp698->tl=_tmpC1A;});_tmp698;}));
goto _LL0;case 531U: _LL421: _LL422:
# 3168
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp69A=_cycalloc(sizeof(*_tmp69A));({void*_tmpC21=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->tag=1U,({unsigned _tmpC20=({unsigned _tmpC1F=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpC1F,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp699->f1=_tmpC20;});_tmp699;});_tmp69A->hd=_tmpC21;}),({struct Cyc_List_List*_tmpC1E=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp69A->tl=_tmpC1E;});_tmp69A;}));
goto _LL0;case 532U: _LL423: _LL424:
# 3174 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpC22=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpC22,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 533U: _LL425: _LL426:
# 3176 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC23=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpC23,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534U: _LL427: _LL428:
# 3178 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 535U: _LL429: _LL42A:
# 3180 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC24=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpC24,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536U: _LL42B: _LL42C:
# 3182 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC25=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpC25,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 537U: _LL42D: _LL42E:
# 3184 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 538U: _LL42F: _LL430:
# 3189 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC26=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpC26,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 539U: _LL431: _LL432:
# 3191 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC28=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC27=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpC28,_tmpC27,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 540U: _LL433: _LL434:
# 3194
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpC29=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpC29,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 541U: _LL435: _LL436:
# 3197
 yyval=Cyc_Exp_tok(({void*_tmpC2D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->tag=29U,({struct _tuple0*_tmpC2C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp69B->f1=_tmpC2C;}),({struct Cyc_List_List*_tmpC2B=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp69B->f2=_tmpC2B;}),({struct Cyc_List_List*_tmpC2A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp69B->f3=_tmpC2A;}),_tmp69B->f4=0;_tmp69B;});Cyc_Absyn_new_exp(_tmpC2D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 542U: _LL437: _LL438:
# 3200
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpC2E=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpC2E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 543U: _LL439: _LL43A:
# 3204 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 544U: _LL43B: _LL43C:
# 3210 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp69C=_cycalloc(sizeof(*_tmp69C));({struct Cyc_Absyn_Exp*_tmpC2F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp69C->hd=_tmpC2F;}),_tmp69C->tl=0;_tmp69C;}));
goto _LL0;case 545U: _LL43D: _LL43E:
# 3212 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));({struct Cyc_Absyn_Exp*_tmpC31=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp69D->hd=_tmpC31;}),({struct Cyc_List_List*_tmpC30=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp69D->tl=_tmpC30;});_tmp69D;}));
goto _LL0;case 546U: _LL43F: _LL440:
# 3218 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpC32=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpC32,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 547U: _LL441: _LL442:
# 3219 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpC33=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpC33,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 548U: _LL443: _LL444:
# 3220 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC34=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpC34,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 549U: _LL445: _LL446: {
# 3222 "parse.y"
struct _fat_ptr _tmp69E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp69E;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3230
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 3232
goto _LL0;}case 550U: _LL447: _LL448:
# 3233 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 551U: _LL449: _LL44A:
# 3237 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));({union Cyc_Absyn_Nmspace _tmpC37=Cyc_Absyn_Rel_n(0);_tmp6A0->f1=_tmpC37;}),({struct _fat_ptr*_tmpC36=({struct _fat_ptr*_tmp69F=_cycalloc(sizeof(*_tmp69F));({struct _fat_ptr _tmpC35=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp69F=_tmpC35;});_tmp69F;});_tmp6A0->f2=_tmpC36;});_tmp6A0;}));
goto _LL0;case 552U: _LL44B: _LL44C:
# 3238 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 553U: _LL44D: _LL44E:
# 3241
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));({union Cyc_Absyn_Nmspace _tmpC3A=Cyc_Absyn_Rel_n(0);_tmp6A2->f1=_tmpC3A;}),({struct _fat_ptr*_tmpC39=({struct _fat_ptr*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));({struct _fat_ptr _tmpC38=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp6A1=_tmpC38;});_tmp6A1;});_tmp6A2->f2=_tmpC39;});_tmp6A2;}));
goto _LL0;case 554U: _LL44F: _LL450:
# 3242 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 555U: _LL451: _LL452:
# 3247 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 556U: _LL453: _LL454:
# 3248 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 557U: _LL455: _LL456:
# 3251
 yyval=(yyyvsp[0]).v;
goto _LL0;case 558U: _LL457: _LL458:
# 3252 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 559U: _LL459: _LL45A:
# 3257 "parse.y"
 goto _LL0;case 560U: _LL45B: _LL45C:
# 3257 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL45D: _LL45E:
# 3261
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
({int _tmpC3B=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpC3B;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpC3C=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,162U,sizeof(short),yyn - 151));_tmpC3C + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 7649)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7650U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,162U,sizeof(short),yyn - 151));}
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
if(yyn > - 32768 && yyn < 7649){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 313U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7650U,sizeof(short),x + yyn))== x)
({unsigned long _tmpC3D=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,313U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpC3D;}),count ++;}
msg=({unsigned _tmp6A4=(unsigned)(sze + 15)+ 1U;char*_tmp6A3=({struct _RegionHandle*_tmpC3E=yyregion;_region_malloc(_tmpC3E,_check_times(_tmp6A4,sizeof(char)));});({{unsigned _tmp763=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp763;++ i){_tmp6A3[i]='\000';}_tmp6A3[_tmp763]=0;}0;});_tag_fat(_tmp6A3,sizeof(char),_tmp6A4);});
({struct _fat_ptr _tmpC3F=msg;Cyc_strcpy(_tmpC3F,({const char*_tmp6A5="parse error";_tag_fat(_tmp6A5,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 313U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7650U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpC40=msg;Cyc_strcat(_tmpC40,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp6A6=", expecting `";_tag_fat(_tmp6A6,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp6A7=" or `";_tag_fat(_tmp6A7,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,313U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpC41=msg;Cyc_strcat(_tmpC41,({const char*_tmp6A8="'";_tag_fat(_tmp6A8,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpC43=({const char*_tmp6A9="parse error";_tag_fat(_tmp6A9,sizeof(char),12U);});int _tmpC42=yystate;Cyc_yyerror(_tmpC43,_tmpC42,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp6AA=1;_npop_handler(0U);return _tmp6AA;}
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
 if(yyssp_offset == 0){int _tmp6AB=1;_npop_handler(0U);return _tmp6AB;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1133U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 7649)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7650U,sizeof(short),yyn))!= 1)goto yyerrdefault;
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
if(yyn == 1132){
int _tmp6AC=0;_npop_handler(0U);return _tmp6AC;}
# 546
({struct Cyc_Yystacktype _tmpC44=({struct Cyc_Yystacktype _tmp764;_tmp764.v=yylval,_tmp764.l=yylloc;_tmp764;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpC44;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region(yyregion);}
# 3260 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6B0=v;struct Cyc_Absyn_Stmt*_tmp6B1;struct Cyc_Absyn_Exp*_tmp6B2;struct _tuple0*_tmp6B3;struct _fat_ptr _tmp6B4;char _tmp6B5;union Cyc_Absyn_Cnst _tmp6B6;switch((_tmp6B0.Stmt_tok).tag){case 1U: _LL1: _tmp6B6=(_tmp6B0.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp6B6;
({struct Cyc_String_pa_PrintArg_struct _tmp6B9=({struct Cyc_String_pa_PrintArg_struct _tmp767;_tmp767.tag=0U,({struct _fat_ptr _tmpC45=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp767.f1=_tmpC45;});_tmp767;});void*_tmp6B7[1U];_tmp6B7[0]=& _tmp6B9;({struct Cyc___cycFILE*_tmpC47=Cyc_stderr;struct _fat_ptr _tmpC46=({const char*_tmp6B8="%s";_tag_fat(_tmp6B8,sizeof(char),3U);});Cyc_fprintf(_tmpC47,_tmpC46,_tag_fat(_tmp6B7,sizeof(void*),1U));});});goto _LL0;}case 2U: _LL3: _tmp6B5=(_tmp6B0.Char_tok).val;_LL4: {char c=_tmp6B5;
({struct Cyc_Int_pa_PrintArg_struct _tmp6BC=({struct Cyc_Int_pa_PrintArg_struct _tmp768;_tmp768.tag=1U,_tmp768.f1=(unsigned long)((int)c);_tmp768;});void*_tmp6BA[1U];_tmp6BA[0]=& _tmp6BC;({struct Cyc___cycFILE*_tmpC49=Cyc_stderr;struct _fat_ptr _tmpC48=({const char*_tmp6BB="%c";_tag_fat(_tmp6BB,sizeof(char),3U);});Cyc_fprintf(_tmpC49,_tmpC48,_tag_fat(_tmp6BA,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL5: _tmp6B4=(_tmp6B0.String_tok).val;_LL6: {struct _fat_ptr s=_tmp6B4;
({struct Cyc_String_pa_PrintArg_struct _tmp6BF=({struct Cyc_String_pa_PrintArg_struct _tmp769;_tmp769.tag=0U,_tmp769.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp769;});void*_tmp6BD[1U];_tmp6BD[0]=& _tmp6BF;({struct Cyc___cycFILE*_tmpC4B=Cyc_stderr;struct _fat_ptr _tmpC4A=({const char*_tmp6BE="\"%s\"";_tag_fat(_tmp6BE,sizeof(char),5U);});Cyc_fprintf(_tmpC4B,_tmpC4A,_tag_fat(_tmp6BD,sizeof(void*),1U));});});goto _LL0;}case 5U: _LL7: _tmp6B3=(_tmp6B0.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp6B3;
({struct Cyc_String_pa_PrintArg_struct _tmp6C2=({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0U,({struct _fat_ptr _tmpC4C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp76A.f1=_tmpC4C;});_tmp76A;});void*_tmp6C0[1U];_tmp6C0[0]=& _tmp6C2;({struct Cyc___cycFILE*_tmpC4E=Cyc_stderr;struct _fat_ptr _tmpC4D=({const char*_tmp6C1="%s";_tag_fat(_tmp6C1,sizeof(char),3U);});Cyc_fprintf(_tmpC4E,_tmpC4D,_tag_fat(_tmp6C0,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL9: _tmp6B2=(_tmp6B0.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6B2;
({struct Cyc_String_pa_PrintArg_struct _tmp6C5=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,({struct _fat_ptr _tmpC4F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp76B.f1=_tmpC4F;});_tmp76B;});void*_tmp6C3[1U];_tmp6C3[0]=& _tmp6C5;({struct Cyc___cycFILE*_tmpC51=Cyc_stderr;struct _fat_ptr _tmpC50=({const char*_tmp6C4="%s";_tag_fat(_tmp6C4,sizeof(char),3U);});Cyc_fprintf(_tmpC51,_tmpC50,_tag_fat(_tmp6C3,sizeof(void*),1U));});});goto _LL0;}case 8U: _LLB: _tmp6B1=(_tmp6B0.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp6B1;
({struct Cyc_String_pa_PrintArg_struct _tmp6C8=({struct Cyc_String_pa_PrintArg_struct _tmp76C;_tmp76C.tag=0U,({struct _fat_ptr _tmpC52=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp76C.f1=_tmpC52;});_tmp76C;});void*_tmp6C6[1U];_tmp6C6[0]=& _tmp6C8;({struct Cyc___cycFILE*_tmpC54=Cyc_stderr;struct _fat_ptr _tmpC53=({const char*_tmp6C7="%s";_tag_fat(_tmp6C7,sizeof(char),3U);});Cyc_fprintf(_tmpC54,_tmpC53,_tag_fat(_tmp6C6,sizeof(void*),1U));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp6C9=0U;({struct Cyc___cycFILE*_tmpC56=Cyc_stderr;struct _fat_ptr _tmpC55=({const char*_tmp6CA="?";_tag_fat(_tmp6CA,sizeof(char),2U);});Cyc_fprintf(_tmpC56,_tmpC55,_tag_fat(_tmp6C9,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3272
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6CB="end-of-file";_tag_fat(_tmp6CB,sizeof(char),12U);});
if(token == 366)
return Cyc_Lex_token_string;else{
if(token == 375)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0 && token <= 378?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,379U,sizeof(short),token)): 313;
if((unsigned)z < 313U)
return Cyc_yytname[z];else{
return _tag_fat(0,0,0);}}}
# 3286
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6CC=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6CC;_push_region(yyr);
({struct _RegionHandle*_tmpC57=yyr;Cyc_yyparse(_tmpC57,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6CD=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6CD;}
# 3289
;_pop_region(yyr);}}
