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
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _fat_ptr lex_base;struct _fat_ptr lex_backtrk;struct _fat_ptr lex_default;struct _fat_ptr lex_trans;struct _fat_ptr lex_check;};
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
# 907
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 909
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 912
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 914
extern void*Cyc_Absyn_sint_type;
# 916
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 919
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 923
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 925
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr s,struct Cyc_Absyn_Kind*k);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 950
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 954
void*Cyc_Absyn_bounds_one (void);
# 956
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 976
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned ztloc);
# 979
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
# 987
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1028
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
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1056
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1061
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1072
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1075
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1078
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1082
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned);
# 1087
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 1089
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1096
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1103
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1106
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned);
# 1111
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1140
int Cyc_Absyn_fntype_att(void*);
# 1165
extern int Cyc_Absyn_porting_c_code;
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 46 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*);
# 95
void*Cyc_Tcutil_copy_type(void*);
# 108
void*Cyc_Tcutil_compress(void*);
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 145
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 150
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 155
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 157
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 159
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 164
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 174
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 257
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 296
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 306
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 68 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _fat_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "parse.y"
extern void Cyc_Lex_register_typedef(struct _tuple0*s);
extern void Cyc_Lex_enter_namespace(struct _fat_ptr*);
extern void Cyc_Lex_leave_namespace (void);
extern void Cyc_Lex_enter_using(struct _tuple0*);
extern void Cyc_Lex_leave_using (void);
extern void Cyc_Lex_enter_extern_c (void);
extern void Cyc_Lex_leave_extern_c (void);
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
({struct Cyc_List_List*_tmp76E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp76E;});
({struct _tuple11*_tmp76F=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp76F;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
({void*_tmpA=0U;({unsigned _tmp771=loc;struct _fat_ptr _tmp770=({const char*_tmpB="qualifier on type is ignored";_tag_fat(_tmpB,sizeof(char),29U);});Cyc_Warn_warn(_tmp771,_tmp770,_tag_fat(_tmpA,sizeof(void*),0U));});});}
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
return({struct _tuple15 _tmp6CF;_tmp6CF.f1=nullable,_tmp6CF.f2=bound,_tmp6CF.f3=zeroterm,_tmp6CF.f4=rgn;_tmp6CF;});}
# 239
struct _tuple0*Cyc_Parse_gensym_enum (void){
# 241
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp775=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp775;}),({
struct _fat_ptr*_tmp774=({struct _fat_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _fat_ptr _tmp773=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6D0;_tmp6D0.tag=1U,_tmp6D0.f1=(unsigned long)enum_counter ++;_tmp6D0;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp772=({const char*_tmp11="__anonymous_enum_%d__";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp772,_tag_fat(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp773;});_tmp13;});_tmp14->f2=_tmp774;});_tmp14;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 246
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 251
struct _tuple18*_tmp15=field_info;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_List_List*_tmp1B;struct Cyc_List_List*_tmp1A;void*_tmp19;struct Cyc_Absyn_Tqual _tmp18;struct _tuple0*_tmp17;unsigned _tmp16;_LL1: _tmp16=(_tmp15->f1)->f1;_tmp17=(_tmp15->f1)->f2;_tmp18=(_tmp15->f1)->f3;_tmp19=(_tmp15->f1)->f4;_tmp1A=(_tmp15->f1)->f5;_tmp1B=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1D=(_tmp15->f2)->f2;_LL2: {unsigned varloc=_tmp16;struct _tuple0*qid=_tmp17;struct Cyc_Absyn_Tqual tq=_tmp18;void*t=_tmp19;struct Cyc_List_List*tvs=_tmp1A;struct Cyc_List_List*atts=_tmp1B;struct Cyc_Absyn_Exp*widthopt=_tmp1C;struct Cyc_Absyn_Exp*reqopt=_tmp1D;
if(tvs != 0)
({void*_tmp1E=0U;({unsigned _tmp777=loc;struct _fat_ptr _tmp776=({const char*_tmp1F="bad type params in struct field";_tag_fat(_tmp1F,sizeof(char),32U);});Cyc_Warn_err(_tmp777,_tmp776,_tag_fat(_tmp1E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp20=0U;({unsigned _tmp779=loc;struct _fat_ptr _tmp778=({const char*_tmp21="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp21,sizeof(char),59U);});Cyc_Warn_err(_tmp779,_tmp778,_tag_fat(_tmp20,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->name=(*qid).f2,_tmp22->tq=tq,_tmp22->type=t,_tmp22->width=widthopt,_tmp22->attributes=atts,_tmp22->requires_clause=reqopt;_tmp22;});}}
# 261
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp6D1;_tmp6D1.Signed_spec=0,_tmp6D1.Unsigned_spec=0,_tmp6D1.Short_spec=0,_tmp6D1.Long_spec=0,_tmp6D1.Long_Long_spec=0,_tmp6D1.Valid_type_spec=0,_tmp6D1.Type_spec=Cyc_Absyn_sint_type,_tmp6D1.loc=loc;_tmp6D1;});}
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
void*_tmp77A=t;Cyc_Tcutil_promote_array(_tmp77A,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 315 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;struct Cyc_List_List*res=_tmp28;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp2=_tmp29;struct _tuple8*_tmp2A=_stmttmp2;void**_tmp2C;struct _fat_ptr _tmp2B;void*_tmp2E;struct _fat_ptr*_tmp2D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp2D=_tmp2A->f1;_tmp2E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp2D != 0){_LL2: {struct _fat_ptr*v=_tmp2D;void*i=_tmp2E;
# 320
{void*_tmp2F=i;void**_tmp30;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->tag == 1U){_LL8: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2F)->f2;_LL9: {void**z=_tmp30;
# 324
struct _fat_ptr*nm=({struct _fat_ptr*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _fat_ptr _tmp77C=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35=({struct Cyc_String_pa_PrintArg_struct _tmp6D2;_tmp6D2.tag=0U,_tmp6D2.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp6D2;});void*_tmp33[1U];_tmp33[0]=& _tmp35;({struct _fat_ptr _tmp77B=({const char*_tmp34="`%s";_tag_fat(_tmp34,sizeof(char),4U);});Cyc_aprintf(_tmp77B,_tag_fat(_tmp33,sizeof(void*),1U));});});*_tmp36=_tmp77C;});_tmp36;});
({void*_tmp77E=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->name=nm,_tmp32->identity=- 1,({void*_tmp77D=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->tag=0U,_tmp31->f1=& Cyc_Tcutil_ik;_tmp31;});_tmp32->kind=_tmp77D;});_tmp32;}));*z=_tmp77E;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 329
res=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _tuple19*_tmp77F=({struct _tuple19*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=v,_tmp37->f2=i;_tmp37;});_tmp38->hd=_tmp77F;}),_tmp38->tl=res;_tmp38;});goto _LL0;}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp2B=*_tmp2A->f1;_tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp2B;void**z=_tmp2C;
# 333
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _fat_ptr _tmp781=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3D=({struct Cyc_String_pa_PrintArg_struct _tmp6D3;_tmp6D3.tag=0U,_tmp6D3.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp6D3;});void*_tmp3B[1U];_tmp3B[0]=& _tmp3D;({struct _fat_ptr _tmp780=({const char*_tmp3C="`%s";_tag_fat(_tmp3C,sizeof(char),4U);});Cyc_aprintf(_tmp780,_tag_fat(_tmp3B,sizeof(void*),1U));});});*_tmp3E=_tmp781;});_tmp3E;});
({void*_tmp783=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->name=nm,_tmp3A->identity=- 1,({void*_tmp782=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=& Cyc_Tcutil_rk;_tmp39;});_tmp3A->kind=_tmp782;});_tmp3A;}));*z=_tmp783;});
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
res=({struct Cyc_List_List*_tmp44=_cycalloc(sizeof(*_tmp44));({struct _tuple19*_tmp784=({struct _tuple19*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp43->f2=i;_tmp43;});_tmp44->hd=_tmp784;}),_tmp44->tl=res;_tmp44;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
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
return({void*_tmp786=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp785=Cyc_Tcutil_copy_type(i);_tmp4D->f1=_tmp785;});_tmp4D;});Cyc_Absyn_new_exp(_tmp786,e->loc);});}}}
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
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D4;_tmp6D4.elt_type=et2,_tmp6D4.elt_tq=tq,(_tmp6D4.ptr_atts).rgn=r,(_tmp6D4.ptr_atts).nullable=n,(_tmp6D4.ptr_atts).bounds=b2,(_tmp6D4.ptr_atts).zero_term=zt,(_tmp6D4.ptr_atts).ptrloc=pl;_tmp6D4;}));
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
({void*_tmp787=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp787;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};
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
if(({struct _fat_ptr _tmp788=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp788,({const char*_tmp64="`H";_tag_fat(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;else{
if(({struct _fat_ptr _tmp789=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp789,({const char*_tmp65="`U";_tag_fat(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;else{
if(({struct _fat_ptr _tmp78A=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp78A,({const char*_tmp66="`RC";_tag_fat(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;else{
# 436
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _fat_ptr*_tmp78B=({struct _fat_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp78B;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}}}}
# 443
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp69=t;struct Cyc_Absyn_Tvar*_tmp6A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->tag == 2U){_LL1: _tmp6A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp6A;
return pr;}}else{_LL3: _LL4:
({void*_tmp6B=0U;({unsigned _tmp78D=loc;struct _fat_ptr _tmp78C=({const char*_tmp6C="expecting a list of type variables, not types";_tag_fat(_tmp6C,sizeof(char),46U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp78D,_tmp78C,_tag_fat(_tmp6B,sizeof(void*),0U));});});}_LL0:;}
# 451
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmp6D;void*_tmp6E=_stmttmp6;void**_tmp6F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->tag == 2U){_LL1: _tmp6F=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1)->kind;_LL2: {void**cptr=_tmp6F;
# 454
void*_tmp70=Cyc_Absyn_compress_kb(*cptr);void*_stmttmp7=_tmp70;void*_tmp71=_stmttmp7;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp71)->tag == 1U){_LL6: _LL7:
# 456
 if(!leq)({void*_tmp78E=Cyc_Tcutil_kind_to_bound(k);*cptr=_tmp78E;});else{
({void*_tmp78F=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=2U,_tmp72->f1=0,_tmp72->f2=k;_tmp72;});*cptr=_tmp78F;});}
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
({void*_tmp78=0U;({unsigned _tmp791=loc;struct _fat_ptr _tmp790=({const char*_tmp79="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp79,sizeof(char),93U);});Cyc_Warn_warn(_tmp791,_tmp790,_tag_fat(_tmp78,sizeof(void*),0U));});});
# 489
return tms;}else{_LL8: _tmp77=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp76)->f1;_LL9: {struct Cyc_List_List*ids=_tmp77;
# 491
if(({int _tmp792=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ids);_tmp792 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp7A=0U;({unsigned _tmp794=loc;struct _fat_ptr _tmp793=({const char*_tmp7B="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp7B,sizeof(char),73U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp794,_tmp793,_tag_fat(_tmp7A,sizeof(void*),0U));});});{
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
({void*_tmp81=0U;({unsigned _tmp796=x->loc;struct _fat_ptr _tmp795=({const char*_tmp82="initializer found in parameter declaration";_tag_fat(_tmp82,sizeof(char),43U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp796,_tmp795,_tag_fat(_tmp81,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({void*_tmp83=0U;({unsigned _tmp798=x->loc;struct _fat_ptr _tmp797=({const char*_tmp84="namespaces forbidden in parameter declarations";_tag_fat(_tmp84,sizeof(char),47U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp798,_tmp797,_tag_fat(_tmp83,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp86=_cycalloc(sizeof(*_tmp86));
({struct _tuple8*_tmp799=({struct _tuple8*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=(*vd->name).f2,_tmp85->f2=vd->tq,_tmp85->f3=vd->type;_tmp85;});_tmp86->hd=_tmp799;}),_tmp86->tl=rev_new_params;_tmp86;});
# 511
goto L;}}else{_LLD: _LLE:
({void*_tmp87=0U;({unsigned _tmp79B=x->loc;struct _fat_ptr _tmp79A=({const char*_tmp88="nonvariable declaration in parameter type";_tag_fat(_tmp88,sizeof(char),42U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79B,_tmp79A,_tag_fat(_tmp87,sizeof(void*),0U));});});}_LLA:;}
# 515
L: if(tds2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp6D5;_tmp6D5.tag=0U,_tmp6D5.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)ids->hd));_tmp6D5;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({unsigned _tmp79D=loc;struct _fat_ptr _tmp79C=({const char*_tmp8A="%s is not given a type";_tag_fat(_tmp8A,sizeof(char),23U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp79D,_tmp79C,_tag_fat(_tmp89,sizeof(void*),1U));});});}
# 518
return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));
({void*_tmp7A0=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8D=_region_malloc(yy,sizeof(*_tmp8D));_tmp8D->tag=3U,({void*_tmp79F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));_tmp8C->tag=1U,({struct Cyc_List_List*_tmp79E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp8C->f1=_tmp79E;}),_tmp8C->f2=0,_tmp8C->f3=0,_tmp8C->f4=0,_tmp8C->f5=0,_tmp8C->f6=0,_tmp8C->f7=0;_tmp8C;});_tmp8D->f1=_tmp79F;});_tmp8D;});_tmp8E->hd=_tmp7A0;}),_tmp8E->tl=0;_tmp8E;});}}}_LL5:;}
# 525
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8F=_region_malloc(yy,sizeof(*_tmp8F));_tmp8F->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7A1=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8F->tl=_tmp7A1;});_tmp8F;});}_LL0:;}}
# 533
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 537
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6D6;_tmp6D6.id=d.id,_tmp6D6.varloc=d.varloc,({struct Cyc_List_List*_tmp7A2=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6D6.tms=_tmp7A2;});_tmp6D6;});{
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
({void*_tmp92=0U;({unsigned _tmp7A4=loc;struct _fat_ptr _tmp7A3=({const char*_tmp93="bad storage class on function";_tag_fat(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp7A4,_tmp7A3,_tag_fat(_tmp92,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 559
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp94;
struct _tuple14 _tmp95=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpB=_tmp95;struct _tuple14 _tmp96=_stmttmpB;struct Cyc_List_List*_tmp9A;struct Cyc_List_List*_tmp99;void*_tmp98;struct Cyc_Absyn_Tqual _tmp97;_LL8: _tmp97=_tmp96.f1;_tmp98=_tmp96.f2;_tmp99=_tmp96.f3;_tmp9A=_tmp96.f4;_LL9: {struct Cyc_Absyn_Tqual fn_tqual=_tmp97;void*fn_type=_tmp98;struct Cyc_List_List*x=_tmp99;struct Cyc_List_List*out_atts=_tmp9A;
# 564
if(x != 0)
# 567
({void*_tmp9B=0U;({unsigned _tmp7A6=loc;struct _fat_ptr _tmp7A5=({const char*_tmp9C="bad type params, ignoring";_tag_fat(_tmp9C,sizeof(char),26U);});Cyc_Warn_warn(_tmp7A6,_tmp7A5,_tag_fat(_tmp9B,sizeof(void*),0U));});});{
# 569
void*_tmp9D=fn_type;struct Cyc_Absyn_FnInfo _tmp9E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9D)->tag == 5U){_LLB: _tmp9E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9D)->f1;_LLC: {struct Cyc_Absyn_FnInfo i=_tmp9E;
# 571
{struct Cyc_List_List*_tmp9F=i.args;struct Cyc_List_List*args2=_tmp9F;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpA0=0U;({unsigned _tmp7A8=loc;struct _fat_ptr _tmp7A7=({const char*_tmpA1="missing argument variable in function prototype";_tag_fat(_tmpA1,sizeof(char),48U);});Cyc_Warn_err(_tmp7A8,_tmp7A7,_tag_fat(_tmpA0,sizeof(void*),0U));});});
({struct _fat_ptr*_tmp7AA=({struct _fat_ptr*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct _fat_ptr _tmp7A9=({const char*_tmpA2="?";_tag_fat(_tmpA2,sizeof(char),2U);});*_tmpA3=_tmp7A9;});_tmpA3;});(*((struct _tuple8*)args2->hd)).f1=_tmp7AA;});}}}
# 578
({struct Cyc_List_List*_tmp7AB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp7AB;});
return({struct Cyc_Absyn_Fndecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->sc=sc,_tmpA4->is_inline=is_inline,_tmpA4->name=d.id,_tmpA4->body=body,_tmpA4->i=i,_tmpA4->cached_type=0,_tmpA4->param_vardecls=0,_tmpA4->fn_vardecl=0,_tmpA4->orig_scope=sc;_tmpA4;});}}else{_LLD: _LLE:
# 583
({void*_tmpA5=0U;({unsigned _tmp7AD=loc;struct _fat_ptr _tmp7AC=({const char*_tmpA6="declarator is not a function prototype";_tag_fat(_tmpA6,sizeof(char),39U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7AD,_tmp7AC,_tag_fat(_tmpA5,sizeof(void*),0U));});});}_LLA:;}}}}}static char _tmpA7[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 587
static struct _fat_ptr Cyc_Parse_msg1={_tmpA7,_tmpA7,_tmpA7 + 76U};static char _tmpA8[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 589
static struct _fat_ptr Cyc_Parse_msg2={_tmpA8,_tmpA8,_tmpA8 + 87U};static char _tmpA9[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 591
static struct _fat_ptr Cyc_Parse_msg3={_tmpA9,_tmpA9,_tmpA9 + 74U};static char _tmpAA[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 593
static struct _fat_ptr Cyc_Parse_msg4={_tmpAA,_tmpAA,_tmpAA + 84U};
# 600
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 603
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpAB=0U;({unsigned _tmp7AF=loc;struct _fat_ptr _tmp7AE=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7AF,_tmp7AE,_tag_fat(_tmpAB,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpAC=0U;({unsigned _tmp7B1=loc;struct _fat_ptr _tmp7B0=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B1,_tmp7B0,_tag_fat(_tmpAC,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpAD=0U;({unsigned _tmp7B3=loc;struct _fat_ptr _tmp7B2=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B3,_tmp7B2,_tag_fat(_tmpAD,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpAE=0U;({unsigned _tmp7B5=loc;struct _fat_ptr _tmp7B4=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B5,_tmp7B4,_tag_fat(_tmpAE,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpAF=0U;({unsigned _tmp7B7=loc;struct _fat_ptr _tmp7B6=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7B7,_tmp7B6,_tag_fat(_tmpAF,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 625
return s1;}
# 631
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
# 634
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 641
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpB0=0U;({unsigned _tmp7B9=loc;struct _fat_ptr _tmp7B8=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7B9,_tmp7B8,_tag_fat(_tmpB0,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpB1=0U;({unsigned _tmp7BB=loc;struct _fat_ptr _tmp7BA=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7BB,_tmp7BA,_tag_fat(_tmpB1,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 653
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpB2=0U;({unsigned _tmp7BD=loc;struct _fat_ptr _tmp7BC=({const char*_tmpB3="missing type within specifier";_tag_fat(_tmpB3,sizeof(char),30U);});Cyc_Warn_warn(_tmp7BD,_tmp7BC,_tag_fat(_tmpB2,sizeof(void*),0U));});});
t=Cyc_Absyn_int_type(sgn,sz);}else{
# 658
if(seen_sign){
void*_tmpB4=t;enum Cyc_Absyn_Size_of _tmpB6;enum Cyc_Absyn_Sign _tmpB5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->tag == 1U){_LL1: _tmpB5=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f1;_tmpB6=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpB5;enum Cyc_Absyn_Size_of sz2=_tmpB6;
# 661
if((int)sgn2 != (int)sgn)
t=Cyc_Absyn_int_type(sgn,sz2);
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpB7=0U;({unsigned _tmp7BF=loc;struct _fat_ptr _tmp7BE=({const char*_tmpB8="sign specification on non-integral type";_tag_fat(_tmpB8,sizeof(char),40U);});Cyc_Warn_err(_tmp7BF,_tmp7BE,_tag_fat(_tmpB7,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 666
if(seen_size){
void*_tmpB9=t;enum Cyc_Absyn_Size_of _tmpBB;enum Cyc_Absyn_Sign _tmpBA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)){case 1U: _LL6: _tmpBA=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f1;_tmpBB=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f2;_LL7: {enum Cyc_Absyn_Sign sgn2=_tmpBA;enum Cyc_Absyn_Size_of sz2=_tmpBB;
# 669
if((int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL5;}case 2U: _LL8: _LL9:
# 673
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpBC=0U;({unsigned _tmp7C1=loc;struct _fat_ptr _tmp7C0=({const char*_tmpBD="size qualifier on non-integral type";_tag_fat(_tmpBD,sizeof(char),36U);});Cyc_Warn_err(_tmp7C1,_tmp7C0,_tag_fat(_tmpBC,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 677
return t;}
# 680
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple11*ds,struct Cyc_List_List*shared_atts){
# 684
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpBE=d.id;struct _tuple0*q=_tmpBE;
unsigned _tmpBF=d.varloc;unsigned varloc=_tmpBF;
struct _tuple14 _tmpC0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpC=_tmpC0;struct _tuple14 _tmpC1=_stmttmpC;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*_tmpC4;void*_tmpC3;struct Cyc_Absyn_Tqual _tmpC2;_LL1: _tmpC2=_tmpC1.f1;_tmpC3=_tmpC1.f2;_tmpC4=_tmpC1.f3;_tmpC5=_tmpC1.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpC2;void*new_typ=_tmpC3;struct Cyc_List_List*tvs=_tmpC4;struct Cyc_List_List*atts=_tmpC5;
# 691
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpC7=_region_malloc(r,sizeof(*_tmpC7));({struct _tuple16*_tmp7C2=({struct _tuple16*_tmpC6=_region_malloc(r,sizeof(*_tmpC6));_tmpC6->f1=varloc,_tmpC6->f2=q,_tmpC6->f3=tq2,_tmpC6->f4=new_typ,_tmpC6->f5=tvs,_tmpC6->f6=atts;_tmpC6;});_tmpC7->hd=_tmp7C2;}),_tmpC7->tl=0;_tmpC7;});else{
# 694
return({struct Cyc_List_List*_tmpC9=_region_malloc(r,sizeof(*_tmpC9));({struct _tuple16*_tmp7C8=({struct _tuple16*_tmpC8=_region_malloc(r,sizeof(*_tmpC8));_tmpC8->f1=varloc,_tmpC8->f2=q,_tmpC8->f3=tq2,_tmpC8->f4=new_typ,_tmpC8->f5=tvs,_tmpC8->f6=atts;_tmpC8;});_tmpC9->hd=_tmp7C8;}),({
struct Cyc_List_List*_tmp7C7=({struct _RegionHandle*_tmp7C6=r;struct Cyc_Absyn_Tqual _tmp7C5=tq;void*_tmp7C4=Cyc_Tcutil_copy_type(t);struct _tuple11*_tmp7C3=ds->tl;Cyc_Parse_apply_tmss(_tmp7C6,_tmp7C5,_tmp7C4,_tmp7C3,shared_atts);});_tmpC9->tl=_tmp7C7;});_tmpC9;});}}}}
# 698
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 701
if(tms == 0)return({struct _tuple14 _tmp6D7;_tmp6D7.f1=tq,_tmp6D7.f2=t,_tmp6D7.f3=0,_tmp6D7.f4=atts;_tmp6D7;});{
void*_tmpCA=(void*)tms->hd;void*_stmttmpD=_tmpCA;void*_tmpCB=_stmttmpD;struct Cyc_List_List*_tmpCD;unsigned _tmpCC;struct Cyc_Absyn_Tqual _tmpCF;struct Cyc_Absyn_PtrAtts _tmpCE;unsigned _tmpD1;struct Cyc_List_List*_tmpD0;void*_tmpD2;unsigned _tmpD5;void*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;unsigned _tmpD7;void*_tmpD6;switch(*((int*)_tmpCB)){case 0U: _LL1: _tmpD6=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpD7=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL2: {void*zeroterm=_tmpD6;unsigned ztloc=_tmpD7;
# 704
return({struct Cyc_Absyn_Tqual _tmp7CB=Cyc_Absyn_empty_tqual(0U);void*_tmp7CA=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 704
struct Cyc_List_List*_tmp7C9=atts;Cyc_Parse_apply_tms(_tmp7CB,_tmp7CA,_tmp7C9,tms->tl);});}case 1U: _LL3: _tmpD3=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpD4=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_tmpD5=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpD3;void*zeroterm=_tmpD4;unsigned ztloc=_tmpD5;
# 707
return({struct Cyc_Absyn_Tqual _tmp7CE=Cyc_Absyn_empty_tqual(0U);void*_tmp7CD=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 707
struct Cyc_List_List*_tmp7CC=atts;Cyc_Parse_apply_tms(_tmp7CE,_tmp7CD,_tmp7CC,tms->tl);});}case 3U: _LL5: _tmpD2=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_LL6: {void*args=_tmpD2;
# 710
void*_tmpD8=args;unsigned _tmpD9;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_List_List*_tmpDE;void*_tmpDD;struct Cyc_Absyn_VarargInfo*_tmpDC;int _tmpDB;struct Cyc_List_List*_tmpDA;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->tag == 1U){_LLE: _tmpDA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f1;_tmpDB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f2;_tmpDC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f3;_tmpDD=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f4;_tmpDE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f5;_tmpDF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f6;_tmpE0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD8)->f7;_LLF: {struct Cyc_List_List*args2=_tmpDA;int c_vararg=_tmpDB;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpDC;void*eff=_tmpDD;struct Cyc_List_List*rgn_po=_tmpDE;struct Cyc_Absyn_Exp*req=_tmpDF;struct Cyc_Absyn_Exp*ens=_tmpE0;
# 712
struct Cyc_List_List*typvars=0;
# 714
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->hd=(void*)as->hd,_tmpE1->tl=fn_atts;_tmpE1;});else{
# 719
new_atts=({struct Cyc_List_List*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->hd=(void*)as->hd,_tmpE2->tl=new_atts;_tmpE2;});}}}
# 722
if(tms->tl != 0){
void*_tmpE3=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpE=_tmpE3;void*_tmpE4=_stmttmpE;struct Cyc_List_List*_tmpE5;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE4)->tag == 4U){_LL13: _tmpE5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE4)->f1;_LL14: {struct Cyc_List_List*ts=_tmpE5;
# 725
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
# 729
 goto _LL12;}_LL12:;}
# 733
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 738
args2=0;{
# 741
struct Cyc_List_List*_tmpE6=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpE6;
# 743
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 748
{struct Cyc_List_List*_tmpE7=args2;struct Cyc_List_List*a=_tmpE7;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpE8=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpF=_tmpE8;struct _tuple8*_tmpE9=_stmttmpF;void**_tmpEC;struct Cyc_Absyn_Tqual _tmpEB;struct _fat_ptr*_tmpEA;_LL18: _tmpEA=_tmpE9->f1;_tmpEB=_tmpE9->f2;_tmpEC=(void**)& _tmpE9->f3;_LL19: {struct _fat_ptr*vopt=_tmpEA;struct Cyc_Absyn_Tqual tq=_tmpEB;void**t=_tmpEC;
if(tags != 0)
({void*_tmp7CF=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp7CF;});
({void*_tmp7D0=Cyc_Parse_array2ptr(*t,1);*t=_tmp7D0;});}}}
# 760
return({struct Cyc_Absyn_Tqual _tmp7D3=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7D2=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 760
struct Cyc_List_List*_tmp7D1=new_atts;Cyc_Parse_apply_tms(_tmp7D3,_tmp7D2,_tmp7D1,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpD9=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpD8)->f2;_LL11: {unsigned loc=_tmpD9;
# 767
({void*_tmpED=0U;({unsigned _tmp7D5=loc;struct _fat_ptr _tmp7D4=({const char*_tmpEE="function declaration without parameter types";_tag_fat(_tmpEE,sizeof(char),45U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7D5,_tmp7D4,_tag_fat(_tmpED,sizeof(void*),0U));});});}}_LLD:;}case 4U: _LL7: _tmpD0=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpD1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL8: {struct Cyc_List_List*ts=_tmpD0;unsigned loc=_tmpD1;
# 774
if(tms->tl == 0)
return({struct _tuple14 _tmp6D8;_tmp6D8.f1=tq,_tmp6D8.f2=t,_tmp6D8.f3=ts,_tmp6D8.f4=atts;_tmp6D8;});
# 779
({void*_tmpEF=0U;({unsigned _tmp7D7=loc;struct _fat_ptr _tmp7D6=({const char*_tmpF0="type parameters must appear before function arguments in declarator";_tag_fat(_tmpF0,sizeof(char),68U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp7D7,_tmp7D6,_tag_fat(_tmpEF,sizeof(void*),0U));});});}case 2U: _LL9: _tmpCE=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpCF=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpCE;struct Cyc_Absyn_Tqual tq2=_tmpCF;
# 782
return({struct Cyc_Absyn_Tqual _tmp7DA=tq2;void*_tmp7D9=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6D9;_tmp6D9.elt_type=t,_tmp6D9.elt_tq=tq,_tmp6D9.ptr_atts=ptratts;_tmp6D9;}));struct Cyc_List_List*_tmp7D8=atts;Cyc_Parse_apply_tms(_tmp7DA,_tmp7D9,_tmp7D8,tms->tl);});}default: _LLB: _tmpCC=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpCD=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLC: {unsigned loc=_tmpCC;struct Cyc_List_List*atts2=_tmpCD;
# 787
return({struct Cyc_Absyn_Tqual _tmp7DD=tq;void*_tmp7DC=t;struct Cyc_List_List*_tmp7DB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp7DD,_tmp7DC,_tmp7DB,tms->tl);});}}_LL0:;}}
# 793
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 802
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpF1=t;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF6;void*_tmpF5;struct Cyc_Absyn_Tqual _tmpF4;struct _tuple0*_tmpF3;unsigned _tmpF2;_LL1: _tmpF2=_tmpF1->f1;_tmpF3=_tmpF1->f2;_tmpF4=_tmpF1->f3;_tmpF5=_tmpF1->f4;_tmpF6=_tmpF1->f5;_tmpF7=_tmpF1->f6;_LL2: {unsigned varloc=_tmpF2;struct _tuple0*x=_tmpF3;struct Cyc_Absyn_Tqual tq=_tmpF4;void*typ=_tmpF5;struct Cyc_List_List*tvs=_tmpF6;struct Cyc_List_List*atts=_tmpF7;
# 805
Cyc_Lex_register_typedef(x);{
# 807
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpF8=typ;struct Cyc_Core_Opt*_tmpF9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF8)->tag == 1U){_LL4: _tmpF9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF8)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmpF9;
# 811
type=0;
if(kopt == 0)kind=& Cyc_Tcutil_bko;else{
kind=kopt;}
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 817
return({void*_tmp7DF=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7DE=({struct Cyc_Absyn_Typedefdecl*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->name=x,_tmpFA->tvs=tvs,_tmpFA->kind=kind,_tmpFA->defn=type,_tmpFA->atts=atts,_tmpFA->tq=tq,_tmpFA->extern_c=0;_tmpFA;});_tmpFB->f1=_tmp7DE;});_tmpFB;});Cyc_Absyn_new_decl(_tmp7DF,loc);});}}}
# 824
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7E0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=12U,_tmpFC->f1=d,_tmpFC->f2=s;_tmpFC;});Cyc_Absyn_new_stmt(_tmp7E0,d->loc);});}
# 830
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 839
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple13*ids,unsigned tqual_loc,unsigned loc){
# 843
struct _RegionHandle _tmpFD=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpFD;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpFE=ds;struct Cyc_List_List*_tmp101;struct Cyc_Parse_Type_specifier _tmp100;struct Cyc_Absyn_Tqual _tmpFF;_LL1: _tmpFF=_tmpFE.tq;_tmp100=_tmpFE.type_specs;_tmp101=_tmpFE.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmpFF;struct Cyc_Parse_Type_specifier tss=_tmp100;struct Cyc_List_List*atts=_tmp101;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp102=0U;({unsigned _tmp7E2=loc;struct _fat_ptr _tmp7E1=({const char*_tmp103="inline qualifier on non-function definition";_tag_fat(_tmp103,sizeof(char),44U);});Cyc_Warn_warn(_tmp7E2,_tmp7E1,_tag_fat(_tmp102,sizeof(void*),0U));});});{
# 849
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
# 866
struct _tuple11*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 870
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 878
void*_tmp106=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp106;
if(declarators == 0){
# 882
void*_tmp107=base_type;struct Cyc_List_List*_tmp108;struct _tuple0*_tmp109;struct Cyc_List_List*_tmp10C;int _tmp10B;struct _tuple0*_tmp10A;struct Cyc_Absyn_Datatypedecl**_tmp10D;struct Cyc_List_List*_tmp110;struct _tuple0*_tmp10F;enum Cyc_Absyn_AggrKind _tmp10E;struct Cyc_Absyn_Datatypedecl*_tmp111;struct Cyc_Absyn_Enumdecl*_tmp112;struct Cyc_Absyn_Aggrdecl*_tmp113;switch(*((int*)_tmp107)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)){case 0U: _LL15: _tmp113=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp113;
# 884
({struct Cyc_List_List*_tmp7E3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp7E3;});
ad->sc=s;{
struct Cyc_List_List*_tmp116=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_Decl*_tmp7E5=({void*_tmp7E4=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->tag=5U,_tmp114->f1=ad;_tmp114;});Cyc_Absyn_new_decl(_tmp7E4,loc);});_tmp115->hd=_tmp7E5;}),_tmp115->tl=0;_tmp115;});_npop_handler(0U);return _tmp116;}}case 1U: _LL17: _tmp112=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp112;
# 888
if(atts != 0)({void*_tmp117=0U;({unsigned _tmp7E7=loc;struct _fat_ptr _tmp7E6=({const char*_tmp118="attributes on enum not supported";_tag_fat(_tmp118,sizeof(char),33U);});Cyc_Warn_err(_tmp7E7,_tmp7E6,_tag_fat(_tmp117,sizeof(void*),0U));});});
ed->sc=s;{
struct Cyc_List_List*_tmp11B=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct Cyc_Absyn_Decl*_tmp7E9=({void*_tmp7E8=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=7U,_tmp119->f1=ed;_tmp119;});Cyc_Absyn_new_decl(_tmp7E8,loc);});_tmp11A->hd=_tmp7E9;}),_tmp11A->tl=0;_tmp11A;});_npop_handler(0U);return _tmp11B;}}default: _LL19: _tmp111=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp107)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp111;
# 892
if(atts != 0)({void*_tmp11C=0U;({unsigned _tmp7EB=loc;struct _fat_ptr _tmp7EA=({const char*_tmp11D="attributes on datatypes not supported";_tag_fat(_tmp11D,sizeof(char),38U);});Cyc_Warn_err(_tmp7EB,_tmp7EA,_tag_fat(_tmp11C,sizeof(void*),0U));});});
dd->sc=s;{
struct Cyc_List_List*_tmp120=({struct Cyc_List_List*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct Cyc_Absyn_Decl*_tmp7ED=({void*_tmp7EC=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->tag=6U,_tmp11E->f1=dd;_tmp11E;});Cyc_Absyn_new_decl(_tmp7EC,loc);});_tmp11F->hd=_tmp7ED;}),_tmp11F->tl=0;_tmp11F;});_npop_handler(0U);return _tmp120;}}}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp10E=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).val).f1;_tmp10F=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownAggr).val).f2;_tmp110=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp10E;struct _tuple0*n=_tmp10F;struct Cyc_List_List*ts=_tmp110;
# 896
struct Cyc_List_List*_tmp121=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp121;
struct Cyc_Absyn_Aggrdecl*_tmp122=({struct Cyc_Absyn_Aggrdecl*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->kind=k,_tmp128->sc=s,_tmp128->name=n,_tmp128->tvs=ts2,_tmp128->impl=0,_tmp128->attributes=0,_tmp128->expected_mem_kind=0;_tmp128;});struct Cyc_Absyn_Aggrdecl*ad=_tmp122;
if(atts != 0)({void*_tmp123=0U;({unsigned _tmp7EF=loc;struct _fat_ptr _tmp7EE=({const char*_tmp124="bad attributes on type declaration";_tag_fat(_tmp124,sizeof(char),35U);});Cyc_Warn_err(_tmp7EF,_tmp7EE,_tag_fat(_tmp123,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp127=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));({struct Cyc_Absyn_Decl*_tmp7F1=({void*_tmp7F0=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=5U,_tmp125->f1=ad;_tmp125;});Cyc_Absyn_new_decl(_tmp7F0,loc);});_tmp126->hd=_tmp7F1;}),_tmp126->tl=0;_tmp126;});_npop_handler(0U);return _tmp127;}}}else{goto _LL25;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp10D=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp10D;
# 901
if(atts != 0)({void*_tmp129=0U;({unsigned _tmp7F3=loc;struct _fat_ptr _tmp7F2=({const char*_tmp12A="bad attributes on datatype";_tag_fat(_tmp12A,sizeof(char),27U);});Cyc_Warn_err(_tmp7F3,_tmp7F2,_tag_fat(_tmp129,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12D=({struct Cyc_List_List*_tmp12C=_cycalloc(sizeof(*_tmp12C));({struct Cyc_Absyn_Decl*_tmp7F5=({void*_tmp7F4=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=6U,_tmp12B->f1=*tudp;_tmp12B;});Cyc_Absyn_new_decl(_tmp7F4,loc);});_tmp12C->hd=_tmp7F5;}),_tmp12C->tl=0;_tmp12C;});_npop_handler(0U);return _tmp12D;}}}else{_LL1F: _tmp10A=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownDatatype).val).name;_tmp10B=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp10C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f2;_LL20: {struct _tuple0*n=_tmp10A;int isx=_tmp10B;struct Cyc_List_List*ts=_tmp10C;
# 904
struct Cyc_List_List*_tmp12E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp12E;
struct Cyc_Absyn_Decl*_tmp12F=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp12F;
if(atts != 0)({void*_tmp130=0U;({unsigned _tmp7F7=loc;struct _fat_ptr _tmp7F6=({const char*_tmp131="bad attributes on datatype";_tag_fat(_tmp131,sizeof(char),27U);});Cyc_Warn_err(_tmp7F7,_tmp7F6,_tag_fat(_tmp130,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->hd=tud,_tmp132->tl=0;_tmp132;});_npop_handler(0U);return _tmp133;}}}case 15U: _LL21: _tmp109=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1;_LL22: {struct _tuple0*n=_tmp109;
# 909
struct Cyc_Absyn_Enumdecl*_tmp134=({struct Cyc_Absyn_Enumdecl*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->sc=s,_tmp13B->name=n,_tmp13B->fields=0;_tmp13B;});struct Cyc_Absyn_Enumdecl*ed=_tmp134;
if(atts != 0)({void*_tmp135=0U;({unsigned _tmp7F9=loc;struct _fat_ptr _tmp7F8=({const char*_tmp136="bad attributes on enum";_tag_fat(_tmp136,sizeof(char),23U);});Cyc_Warn_err(_tmp7F9,_tmp7F8,_tag_fat(_tmp135,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));({struct Cyc_Absyn_Decl*_tmp7FB=({struct Cyc_Absyn_Decl*_tmp138=_cycalloc(sizeof(*_tmp138));({void*_tmp7FA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->tag=7U,_tmp137->f1=ed;_tmp137;});_tmp138->r=_tmp7FA;}),_tmp138->loc=loc;_tmp138;});_tmp139->hd=_tmp7FB;}),_tmp139->tl=0;_tmp139;});_npop_handler(0U);return _tmp13A;}}case 16U: _LL23: _tmp108=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp107)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp108;
# 915
struct Cyc_Absyn_Enumdecl*_tmp13C=({struct Cyc_Absyn_Enumdecl*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->sc=s,({struct _tuple0*_tmp7FD=Cyc_Parse_gensym_enum();_tmp144->name=_tmp7FD;}),({struct Cyc_Core_Opt*_tmp7FC=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->v=fs;_tmp143;});_tmp144->fields=_tmp7FC;});_tmp144;});struct Cyc_Absyn_Enumdecl*ed=_tmp13C;
if(atts != 0)({void*_tmp13D=0U;({unsigned _tmp7FF=loc;struct _fat_ptr _tmp7FE=({const char*_tmp13E="bad attributes on enum";_tag_fat(_tmp13E,sizeof(char),23U);});Cyc_Warn_err(_tmp7FF,_tmp7FE,_tag_fat(_tmp13D,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));({struct Cyc_Absyn_Decl*_tmp801=({struct Cyc_Absyn_Decl*_tmp140=_cycalloc(sizeof(*_tmp140));({void*_tmp800=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->tag=7U,_tmp13F->f1=ed;_tmp13F;});_tmp140->r=_tmp800;}),_tmp140->loc=loc;_tmp140;});_tmp141->hd=_tmp801;}),_tmp141->tl=0;_tmp141;});_npop_handler(0U);return _tmp142;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp145=0U;({unsigned _tmp803=loc;struct _fat_ptr _tmp802=({const char*_tmp146="missing declarator";_tag_fat(_tmp146,sizeof(char),19U);});Cyc_Warn_err(_tmp803,_tmp802,_tag_fat(_tmp145,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp147=0;_npop_handler(0U);return _tmp147;}}_LL14:;}else{
# 922
struct Cyc_List_List*_tmp148=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp148;
if(istypedef){
# 927
if(!exps_empty)
({void*_tmp149=0U;({unsigned _tmp805=loc;struct _fat_ptr _tmp804=({const char*_tmp14A="initializer in typedef declaration";_tag_fat(_tmp14A,sizeof(char),35U);});Cyc_Warn_err(_tmp805,_tmp804,_tag_fat(_tmp149,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned,struct _tuple16*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp14B=decls;_npop_handler(0U);return _tmp14B;}}else{
# 933
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp14C=fields;struct Cyc_List_List*ds=_tmp14C;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp14D=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp14D;struct _tuple16*_tmp14E=_stmttmp11;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp153;void*_tmp152;struct Cyc_Absyn_Tqual _tmp151;struct _tuple0*_tmp150;unsigned _tmp14F;_LL28: _tmp14F=_tmp14E->f1;_tmp150=_tmp14E->f2;_tmp151=_tmp14E->f3;_tmp152=_tmp14E->f4;_tmp153=_tmp14E->f5;_tmp154=_tmp14E->f6;_LL29: {unsigned varloc=_tmp14F;struct _tuple0*x=_tmp150;struct Cyc_Absyn_Tqual tq2=_tmp151;void*t2=_tmp152;struct Cyc_List_List*tvs2=_tmp153;struct Cyc_List_List*atts2=_tmp154;
if(tvs2 != 0)
({void*_tmp155=0U;({unsigned _tmp807=loc;struct _fat_ptr _tmp806=({const char*_tmp156="bad type params, ignoring";_tag_fat(_tmp156,sizeof(char),26U);});Cyc_Warn_warn(_tmp807,_tmp806,_tag_fat(_tmp155,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp157=0U;({unsigned _tmp809=loc;struct _fat_ptr _tmp808=({const char*_tmp158="unexpected NULL in parse!";_tag_fat(_tmp158,sizeof(char),26U);});((int(*)(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Parse_parse_abort)(_tmp809,_tmp808,_tag_fat(_tmp157,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp159=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;struct Cyc_Absyn_Exp*eopt=_tmp159;
struct Cyc_Absyn_Vardecl*_tmp15A=Cyc_Absyn_new_vardecl(varloc,x,t2,eopt);struct Cyc_Absyn_Vardecl*vd=_tmp15A;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;{
struct Cyc_Absyn_Decl*_tmp15B=({struct Cyc_Absyn_Decl*_tmp15E=_cycalloc(sizeof(*_tmp15E));({void*_tmp80A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->tag=0U,_tmp15D->f1=vd;_tmp15D;});_tmp15E->r=_tmp80A;}),_tmp15E->loc=loc;_tmp15E;});struct Cyc_Absyn_Decl*d=_tmp15B;
decls=({struct Cyc_List_List*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->hd=d,_tmp15C->tl=decls;_tmp15C;});}}}}}{
# 948
struct Cyc_List_List*_tmp15F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp15F;}}}}}}}}}
# 844
;_pop_region();}
# 954
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _fat_ptr s,unsigned loc){
if(Cyc_strlen((struct _fat_ptr)s)== (unsigned long)1 || Cyc_strlen((struct _fat_ptr)s)== (unsigned long)2){
char _tmp160=*((const char*)_check_fat_subscript(s,sizeof(char),0));char _stmttmp12=_tmp160;char _tmp161=_stmttmp12;switch(_tmp161){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 964
{char _tmp162=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _stmttmp13=_tmp162;char _tmp163=_stmttmp13;switch(_tmp163){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 971
goto _LL0;case 84U: _LLF: _LL10:
# 973
{char _tmp164=*((const char*)_check_fat_subscript(s,sizeof(char),1));char _stmttmp14=_tmp164;char _tmp165=_stmttmp14;switch(_tmp165){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 980
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 983
({struct Cyc_String_pa_PrintArg_struct _tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp6DB;_tmp6DB.tag=0U,_tmp6DB.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp6DB;});struct Cyc_Int_pa_PrintArg_struct _tmp169=({struct Cyc_Int_pa_PrintArg_struct _tmp6DA;_tmp6DA.tag=1U,({unsigned long _tmp80B=(unsigned long)((int)Cyc_strlen((struct _fat_ptr)s));_tmp6DA.f1=_tmp80B;});_tmp6DA;});void*_tmp166[2U];_tmp166[0]=& _tmp168,_tmp166[1]=& _tmp169;({unsigned _tmp80D=loc;struct _fat_ptr _tmp80C=({const char*_tmp167="bad kind: %s; strlen=%d";_tag_fat(_tmp167,sizeof(char),24U);});Cyc_Warn_err(_tmp80D,_tmp80C,_tag_fat(_tmp166,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 988
static int Cyc_Parse_exp2int(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16A=e->r;void*_stmttmp15=_tmp16A;void*_tmp16B=_stmttmp15;int _tmp16C;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).tag == 5){_LL1: _tmp16C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).val).f2;_LL2: {int i=_tmp16C;
return i;}}else{goto _LL3;}}else{_LL3: _LL4:
# 992
({void*_tmp16D=0U;({unsigned _tmp80F=loc;struct _fat_ptr _tmp80E=({const char*_tmp16E="expecting integer constant";_tag_fat(_tmp16E,sizeof(char),27U);});Cyc_Warn_err(_tmp80F,_tmp80E,_tag_fat(_tmp16D,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 998
static struct _fat_ptr Cyc_Parse_exp2string(unsigned loc,struct Cyc_Absyn_Exp*e){
void*_tmp16F=e->r;void*_stmttmp16=_tmp16F;void*_tmp170=_stmttmp16;struct _fat_ptr _tmp171;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).tag == 8){_LL1: _tmp171=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).val;_LL2: {struct _fat_ptr s=_tmp171;
return s;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1002
({void*_tmp172=0U;({unsigned _tmp811=loc;struct _fat_ptr _tmp810=({const char*_tmp173="expecting string constant";_tag_fat(_tmp173,sizeof(char),26U);});Cyc_Warn_err(_tmp811,_tmp810,_tag_fat(_tmp172,sizeof(void*),0U));});});
return _tag_fat(0,0,0);}_LL0:;}
# 1008
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp174=x;long long _tmp175;char _tmp176;int _tmp177;switch((_tmp174.LongLong_c).tag){case 5U: _LL1: _tmp177=((_tmp174.Int_c).val).f2;_LL2: {int i=_tmp177;
return(unsigned)i;}case 2U: _LL3: _tmp176=((_tmp174.Char_c).val).f2;_LL4: {char c=_tmp176;
return(unsigned)c;}case 6U: _LL5: _tmp175=((_tmp174.LongLong_c).val).f2;_LL6: {long long x=_tmp175;
# 1013
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp178=0U;({unsigned _tmp813=loc;struct _fat_ptr _tmp812=({const char*_tmp179="integer constant too large";_tag_fat(_tmp179,sizeof(char),27U);});Cyc_Warn_err(_tmp813,_tmp812,_tag_fat(_tmp178,sizeof(void*),0U));});});
return(unsigned)x;}default: _LL7: _LL8:
# 1018
({struct Cyc_String_pa_PrintArg_struct _tmp17C=({struct Cyc_String_pa_PrintArg_struct _tmp6DC;_tmp6DC.tag=0U,({struct _fat_ptr _tmp814=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp6DC.f1=_tmp814;});_tmp6DC;});void*_tmp17A[1U];_tmp17A[0]=& _tmp17C;({unsigned _tmp816=loc;struct _fat_ptr _tmp815=({const char*_tmp17B="expected integer constant but found %s";_tag_fat(_tmp17B,sizeof(char),39U);});Cyc_Warn_err(_tmp816,_tmp815,_tag_fat(_tmp17A,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1024
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17D=p->r;void*_stmttmp17=_tmp17D;void*_tmp17E=_stmttmp17;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_List_List*_tmp181;struct _tuple0*_tmp180;int _tmp183;struct _fat_ptr _tmp182;char _tmp184;int _tmp186;enum Cyc_Absyn_Sign _tmp185;struct Cyc_Absyn_Pat*_tmp187;struct Cyc_Absyn_Vardecl*_tmp188;struct _tuple0*_tmp189;switch(*((int*)_tmp17E)){case 15U: _LL1: _tmp189=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL2: {struct _tuple0*x=_tmp189;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f2)->r)->tag == 0U){_LL3: _tmp188=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp188;
# 1028
return({struct Cyc_Absyn_Exp*_tmp817=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp817,p->loc);});}}else{goto _LL13;}case 6U: _LL5: _tmp187=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp187;
return({struct Cyc_Absyn_Exp*_tmp818=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp818,p->loc);});}case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp185=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp186=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp185;int i=_tmp186;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11U: _LLB: _tmp184=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LLC: {char c=_tmp184;
return Cyc_Absyn_char_exp(c,p->loc);}case 12U: _LLD: _tmp182=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp183=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLE: {struct _fat_ptr s=_tmp182;int i=_tmp183;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f3 == 0){_LLF: _tmp180=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp181=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LL10: {struct _tuple0*x=_tmp180;struct Cyc_List_List*ps=_tmp181;
# 1035
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp17F=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp17F;
return e;}default: _LL13: _LL14:
# 1040
({void*_tmp18A=0U;({unsigned _tmp81A=p->loc;struct _fat_ptr _tmp819=({const char*_tmp18B="cannot mix patterns and expressions in case";_tag_fat(_tmp18B,sizeof(char),44U);});Cyc_Warn_err(_tmp81A,_tmp819,_tag_fat(_tmp18A,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple21{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple21 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple25{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple25*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple26 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple27*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple29{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple29*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple30*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple31{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1125
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp18C=_cycalloc(sizeof(*_tmp18C));_tmp18C->tag=Cyc_Core_Failure,_tmp18C->f1=s;_tmp18C;}));}static char _tmp18F[7U]="cnst_t";
# 1097 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18F,_tmp18F,_tmp18F + 7U};
union Cyc_YYSTYPE*_tmp18D=yy1;union Cyc_Absyn_Cnst _tmp18E;if((((union Cyc_YYSTYPE*)_tmp18D)->Int_tok).tag == 1){_LL1: _tmp18E=(_tmp18D->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp18E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1104
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.Int_tok).tag=1U,(_tmp6DD.Int_tok).val=yy1;_tmp6DD;});}static char _tmp192[5U]="char";
# 1098 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp192,_tmp192,_tmp192 + 5U};
union Cyc_YYSTYPE*_tmp190=yy1;char _tmp191;if((((union Cyc_YYSTYPE*)_tmp190)->Char_tok).tag == 2){_LL1: _tmp191=(_tmp190->Char_tok).val;_LL2: {char yy=_tmp191;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.Char_tok).tag=2U,(_tmp6DE.Char_tok).val=yy1;_tmp6DE;});}static char _tmp195[13U]="string_t<`H>";
# 1099 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp195,_tmp195,_tmp195 + 13U};
union Cyc_YYSTYPE*_tmp193=yy1;struct _fat_ptr _tmp194;if((((union Cyc_YYSTYPE*)_tmp193)->String_tok).tag == 3){_LL1: _tmp194=(_tmp193->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp194;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1106
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.String_tok).tag=3U,(_tmp6DF.String_tok).val=yy1;_tmp6DF;});}static char _tmp198[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1102 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp198,_tmp198,_tmp198 + 45U};
union Cyc_YYSTYPE*_tmp196=yy1;struct _tuple22*_tmp197;if((((union Cyc_YYSTYPE*)_tmp196)->YY1).tag == 9){_LL1: _tmp197=(_tmp196->YY1).val;_LL2: {struct _tuple22*yy=_tmp197;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1109
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY1).tag=9U,(_tmp6E0.YY1).val=yy1;_tmp6E0;});}static char _tmp19B[11U]="ptrbound_t";
# 1103 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19B,_tmp19B,_tmp19B + 11U};
union Cyc_YYSTYPE*_tmp199=yy1;void*_tmp19A;if((((union Cyc_YYSTYPE*)_tmp199)->YY2).tag == 10){_LL1: _tmp19A=(_tmp199->YY2).val;_LL2: {void*yy=_tmp19A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1110
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY2).tag=10U,(_tmp6E1.YY2).val=yy1;_tmp6E1;});}static char _tmp19E[28U]="list_t<offsetof_field_t,`H>";
# 1104 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19E,_tmp19E,_tmp19E + 28U};
union Cyc_YYSTYPE*_tmp19C=yy1;struct Cyc_List_List*_tmp19D;if((((union Cyc_YYSTYPE*)_tmp19C)->YY3).tag == 11){_LL1: _tmp19D=(_tmp19C->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp19D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1111
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY3).tag=11U,(_tmp6E2.YY3).val=yy1;_tmp6E2;});}static char _tmp1A1[6U]="exp_t";
# 1105 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A1,_tmp1A1,_tmp1A1 + 6U};
union Cyc_YYSTYPE*_tmp19F=yy1;struct Cyc_Absyn_Exp*_tmp1A0;if((((union Cyc_YYSTYPE*)_tmp19F)->Exp_tok).tag == 7){_LL1: _tmp1A0=(_tmp19F->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp1A0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1112
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.Exp_tok).tag=7U,(_tmp6E3.Exp_tok).val=yy1;_tmp6E3;});}static char _tmp1A4[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A4,_tmp1A4,_tmp1A4 + 17U};
union Cyc_YYSTYPE*_tmp1A2=yy1;struct Cyc_List_List*_tmp1A3;if((((union Cyc_YYSTYPE*)_tmp1A2)->YY4).tag == 12){_LL1: _tmp1A3=(_tmp1A2->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp1A3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1120
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY4).tag=12U,(_tmp6E4.YY4).val=yy1;_tmp6E4;});}static char _tmp1A7[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1114 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A7,_tmp1A7,_tmp1A7 + 47U};
union Cyc_YYSTYPE*_tmp1A5=yy1;struct Cyc_List_List*_tmp1A6;if((((union Cyc_YYSTYPE*)_tmp1A5)->YY5).tag == 13){_LL1: _tmp1A6=(_tmp1A5->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp1A6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1121
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY5).tag=13U,(_tmp6E5.YY5).val=yy1;_tmp6E5;});}static char _tmp1AA[9U]="primop_t";
# 1115 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AA,_tmp1AA,_tmp1AA + 9U};
union Cyc_YYSTYPE*_tmp1A8=yy1;enum Cyc_Absyn_Primop _tmp1A9;if((((union Cyc_YYSTYPE*)_tmp1A8)->YY6).tag == 14){_LL1: _tmp1A9=(_tmp1A8->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp1A9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1122
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY6).tag=14U,(_tmp6E6.YY6).val=yy1;_tmp6E6;});}static char _tmp1AD[19U]="opt_t<primop_t,`H>";
# 1116 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AD,_tmp1AD,_tmp1AD + 19U};
union Cyc_YYSTYPE*_tmp1AB=yy1;struct Cyc_Core_Opt*_tmp1AC;if((((union Cyc_YYSTYPE*)_tmp1AB)->YY7).tag == 15){_LL1: _tmp1AC=(_tmp1AB->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp1AC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1123
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY7).tag=15U,(_tmp6E7.YY7).val=yy1;_tmp6E7;});}static char _tmp1B0[7U]="qvar_t";
# 1117 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B0,_tmp1B0,_tmp1B0 + 7U};
union Cyc_YYSTYPE*_tmp1AE=yy1;struct _tuple0*_tmp1AF;if((((union Cyc_YYSTYPE*)_tmp1AE)->QualId_tok).tag == 5){_LL1: _tmp1AF=(_tmp1AE->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp1AF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1124
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.QualId_tok).tag=5U,(_tmp6E8.QualId_tok).val=yy1;_tmp6E8;});}static char _tmp1B3[7U]="stmt_t";
# 1120 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B3,_tmp1B3,_tmp1B3 + 7U};
union Cyc_YYSTYPE*_tmp1B1=yy1;struct Cyc_Absyn_Stmt*_tmp1B2;if((((union Cyc_YYSTYPE*)_tmp1B1)->Stmt_tok).tag == 8){_LL1: _tmp1B2=(_tmp1B1->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp1B2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1127
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.Stmt_tok).tag=8U,(_tmp6E9.Stmt_tok).val=yy1;_tmp6E9;});}static char _tmp1B6[27U]="list_t<switch_clause_t,`H>";
# 1124 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B6,_tmp1B6,_tmp1B6 + 27U};
union Cyc_YYSTYPE*_tmp1B4=yy1;struct Cyc_List_List*_tmp1B5;if((((union Cyc_YYSTYPE*)_tmp1B4)->YY8).tag == 16){_LL1: _tmp1B5=(_tmp1B4->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp1B5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1131
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY8).tag=16U,(_tmp6EA.YY8).val=yy1;_tmp6EA;});}static char _tmp1B9[6U]="pat_t";
# 1125 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B9,_tmp1B9,_tmp1B9 + 6U};
union Cyc_YYSTYPE*_tmp1B7=yy1;struct Cyc_Absyn_Pat*_tmp1B8;if((((union Cyc_YYSTYPE*)_tmp1B7)->YY9).tag == 17){_LL1: _tmp1B8=(_tmp1B7->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1B8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1132
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY9).tag=17U,(_tmp6EB.YY9).val=yy1;_tmp6EB;});}static char _tmp1BC[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1130 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BC,_tmp1BC,_tmp1BC + 28U};
union Cyc_YYSTYPE*_tmp1BA=yy1;struct _tuple23*_tmp1BB;if((((union Cyc_YYSTYPE*)_tmp1BA)->YY10).tag == 18){_LL1: _tmp1BB=(_tmp1BA->YY10).val;_LL2: {struct _tuple23*yy=_tmp1BB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1137
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY10).tag=18U,(_tmp6EC.YY10).val=yy1;_tmp6EC;});}static char _tmp1BF[17U]="list_t<pat_t,`H>";
# 1131 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BF,_tmp1BF,_tmp1BF + 17U};
union Cyc_YYSTYPE*_tmp1BD=yy1;struct Cyc_List_List*_tmp1BE;if((((union Cyc_YYSTYPE*)_tmp1BD)->YY11).tag == 19){_LL1: _tmp1BE=(_tmp1BD->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1BE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1138
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY11).tag=19U,(_tmp6ED.YY11).val=yy1;_tmp6ED;});}static char _tmp1C2[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1132 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C2,_tmp1C2,_tmp1C2 + 36U};
union Cyc_YYSTYPE*_tmp1C0=yy1;struct _tuple24*_tmp1C1;if((((union Cyc_YYSTYPE*)_tmp1C0)->YY12).tag == 20){_LL1: _tmp1C1=(_tmp1C0->YY12).val;_LL2: {struct _tuple24*yy=_tmp1C1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1139
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY12).tag=20U,(_tmp6EE.YY12).val=yy1;_tmp6EE;});}static char _tmp1C5[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1133 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C5,_tmp1C5,_tmp1C5 + 47U};
union Cyc_YYSTYPE*_tmp1C3=yy1;struct Cyc_List_List*_tmp1C4;if((((union Cyc_YYSTYPE*)_tmp1C3)->YY13).tag == 21){_LL1: _tmp1C4=(_tmp1C3->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1C4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1140
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY13).tag=21U,(_tmp6EF.YY13).val=yy1;_tmp6EF;});}static char _tmp1C8[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1134 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C8,_tmp1C8,_tmp1C8 + 58U};
union Cyc_YYSTYPE*_tmp1C6=yy1;struct _tuple23*_tmp1C7;if((((union Cyc_YYSTYPE*)_tmp1C6)->YY14).tag == 22){_LL1: _tmp1C7=(_tmp1C6->YY14).val;_LL2: {struct _tuple23*yy=_tmp1C7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1141
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY14).tag=22U,(_tmp6F0.YY14).val=yy1;_tmp6F0;});}static char _tmp1CB[9U]="fndecl_t";
# 1135 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CB,_tmp1CB,_tmp1CB + 9U};
union Cyc_YYSTYPE*_tmp1C9=yy1;struct Cyc_Absyn_Fndecl*_tmp1CA;if((((union Cyc_YYSTYPE*)_tmp1C9)->YY15).tag == 23){_LL1: _tmp1CA=(_tmp1C9->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1CA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1142
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY15).tag=23U,(_tmp6F1.YY15).val=yy1;_tmp6F1;});}static char _tmp1CE[18U]="list_t<decl_t,`H>";
# 1136 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CE,_tmp1CE,_tmp1CE + 18U};
union Cyc_YYSTYPE*_tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;if((((union Cyc_YYSTYPE*)_tmp1CC)->YY16).tag == 24){_LL1: _tmp1CD=(_tmp1CC->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1CD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1143
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY16).tag=24U,(_tmp6F2.YY16).val=yy1;_tmp6F2;});}static char _tmp1D1[12U]="decl_spec_t";
# 1139 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D1,_tmp1D1,_tmp1D1 + 12U};
union Cyc_YYSTYPE*_tmp1CF=yy1;struct Cyc_Parse_Declaration_spec _tmp1D0;if((((union Cyc_YYSTYPE*)_tmp1CF)->YY17).tag == 25){_LL1: _tmp1D0=(_tmp1CF->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1D0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1146
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY17).tag=25U,(_tmp6F3.YY17).val=yy1;_tmp6F3;});}static char _tmp1D4[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1140 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D4,_tmp1D4,_tmp1D4 + 31U};
union Cyc_YYSTYPE*_tmp1D2=yy1;struct _tuple12 _tmp1D3;if((((union Cyc_YYSTYPE*)_tmp1D2)->YY18).tag == 26){_LL1: _tmp1D3=(_tmp1D2->YY18).val;_LL2: {struct _tuple12 yy=_tmp1D3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1147
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY18).tag=26U,(_tmp6F4.YY18).val=yy1;_tmp6F4;});}static char _tmp1D7[23U]="declarator_list_t<`yy>";
# 1141 "parse.y"
static struct _tuple13*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D7,_tmp1D7,_tmp1D7 + 23U};
union Cyc_YYSTYPE*_tmp1D5=yy1;struct _tuple13*_tmp1D6;if((((union Cyc_YYSTYPE*)_tmp1D5)->YY19).tag == 27){_LL1: _tmp1D6=(_tmp1D5->YY19).val;_LL2: {struct _tuple13*yy=_tmp1D6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1148
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple13*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY19).tag=27U,(_tmp6F5.YY19).val=yy1;_tmp6F5;});}static char _tmp1DA[19U]="storage_class_t@`H";
# 1142 "parse.y"
static enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DA,_tmp1DA,_tmp1DA + 19U};
union Cyc_YYSTYPE*_tmp1D8=yy1;enum Cyc_Parse_Storage_class*_tmp1D9;if((((union Cyc_YYSTYPE*)_tmp1D8)->YY20).tag == 28){_LL1: _tmp1D9=(_tmp1D8->YY20).val;_LL2: {enum Cyc_Parse_Storage_class*yy=_tmp1D9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1149
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY20).tag=28U,(_tmp6F6.YY20).val=yy1;_tmp6F6;});}static char _tmp1DD[17U]="type_specifier_t";
# 1143 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DD,_tmp1DD,_tmp1DD + 17U};
union Cyc_YYSTYPE*_tmp1DB=yy1;struct Cyc_Parse_Type_specifier _tmp1DC;if((((union Cyc_YYSTYPE*)_tmp1DB)->YY21).tag == 29){_LL1: _tmp1DC=(_tmp1DB->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1DC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1150
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY21).tag=29U,(_tmp6F7.YY21).val=yy1;_tmp6F7;});}static char _tmp1E0[12U]="aggr_kind_t";
# 1145 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E0,_tmp1E0,_tmp1E0 + 12U};
union Cyc_YYSTYPE*_tmp1DE=yy1;enum Cyc_Absyn_AggrKind _tmp1DF;if((((union Cyc_YYSTYPE*)_tmp1DE)->YY22).tag == 30){_LL1: _tmp1DF=(_tmp1DE->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1DF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1152
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY22).tag=30U,(_tmp6F8.YY22).val=yy1;_tmp6F8;});}static char _tmp1E3[8U]="tqual_t";
# 1146 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E3,_tmp1E3,_tmp1E3 + 8U};
union Cyc_YYSTYPE*_tmp1E1=yy1;struct Cyc_Absyn_Tqual _tmp1E2;if((((union Cyc_YYSTYPE*)_tmp1E1)->YY23).tag == 31){_LL1: _tmp1E2=(_tmp1E1->YY23).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1E2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1153
static union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY23).tag=31U,(_tmp6F9.YY23).val=yy1;_tmp6F9;});}static char _tmp1E6[23U]="list_t<aggrfield_t,`H>";
# 1147 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E6,_tmp1E6,_tmp1E6 + 23U};
union Cyc_YYSTYPE*_tmp1E4=yy1;struct Cyc_List_List*_tmp1E5;if((((union Cyc_YYSTYPE*)_tmp1E4)->YY24).tag == 32){_LL1: _tmp1E5=(_tmp1E4->YY24).val;_LL2: {struct Cyc_List_List*yy=_tmp1E5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1154
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY24).tag=32U,(_tmp6FA.YY24).val=yy1;_tmp6FA;});}static char _tmp1E9[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1148 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E9,_tmp1E9,_tmp1E9 + 34U};
union Cyc_YYSTYPE*_tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;if((((union Cyc_YYSTYPE*)_tmp1E7)->YY25).tag == 33){_LL1: _tmp1E8=(_tmp1E7->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1E8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1155
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY25).tag=33U,(_tmp6FB.YY25).val=yy1;_tmp6FB;});}static char _tmp1EC[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1149 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EC,_tmp1EC,_tmp1EC + 33U};
union Cyc_YYSTYPE*_tmp1EA=yy1;struct Cyc_List_List*_tmp1EB;if((((union Cyc_YYSTYPE*)_tmp1EA)->YY26).tag == 34){_LL1: _tmp1EB=(_tmp1EA->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1EB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1156
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY26).tag=34U,(_tmp6FC.YY26).val=yy1;_tmp6FC;});}static char _tmp1EF[18U]="declarator_t<`yy>";
# 1150 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EF,_tmp1EF,_tmp1EF + 18U};
union Cyc_YYSTYPE*_tmp1ED=yy1;struct Cyc_Parse_Declarator _tmp1EE;if((((union Cyc_YYSTYPE*)_tmp1ED)->YY27).tag == 35){_LL1: _tmp1EE=(_tmp1ED->YY27).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1EE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1157
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY27).tag=35U,(_tmp6FD.YY27).val=yy1;_tmp6FD;});}static char _tmp1F2[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1151 "parse.y"
static struct _tuple25*Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F2,_tmp1F2,_tmp1F2 + 45U};
union Cyc_YYSTYPE*_tmp1F0=yy1;struct _tuple25*_tmp1F1;if((((union Cyc_YYSTYPE*)_tmp1F0)->YY28).tag == 36){_LL1: _tmp1F1=(_tmp1F0->YY28).val;_LL2: {struct _tuple25*yy=_tmp1F1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1158
static union Cyc_YYSTYPE Cyc_YY28(struct _tuple25*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY28).tag=36U,(_tmp6FE.YY28).val=yy1;_tmp6FE;});}static char _tmp1F5[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1152 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F5,_tmp1F5,_tmp1F5 + 57U};
union Cyc_YYSTYPE*_tmp1F3=yy1;struct Cyc_List_List*_tmp1F4;if((((union Cyc_YYSTYPE*)_tmp1F3)->YY29).tag == 37){_LL1: _tmp1F4=(_tmp1F3->YY29).val;_LL2: {struct Cyc_List_List*yy=_tmp1F4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1159
static union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY29).tag=37U,(_tmp6FF.YY29).val=yy1;_tmp6FF;});}static char _tmp1F8[26U]="abstractdeclarator_t<`yy>";
# 1153 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F8,_tmp1F8,_tmp1F8 + 26U};
union Cyc_YYSTYPE*_tmp1F6=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F7;if((((union Cyc_YYSTYPE*)_tmp1F6)->YY30).tag == 38){_LL1: _tmp1F7=(_tmp1F6->YY30).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp1F7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1160
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY30).tag=38U,(_tmp700.YY30).val=yy1;_tmp700;});}static char _tmp1FB[5U]="bool";
# 1154 "parse.y"
static int Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FB,_tmp1FB,_tmp1FB + 5U};
union Cyc_YYSTYPE*_tmp1F9=yy1;int _tmp1FA;if((((union Cyc_YYSTYPE*)_tmp1F9)->YY31).tag == 39){_LL1: _tmp1FA=(_tmp1F9->YY31).val;_LL2: {int yy=_tmp1FA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1161
static union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY31).tag=39U,(_tmp701.YY31).val=yy1;_tmp701;});}static char _tmp1FE[8U]="scope_t";
# 1155 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FE,_tmp1FE,_tmp1FE + 8U};
union Cyc_YYSTYPE*_tmp1FC=yy1;enum Cyc_Absyn_Scope _tmp1FD;if((((union Cyc_YYSTYPE*)_tmp1FC)->YY32).tag == 40){_LL1: _tmp1FD=(_tmp1FC->YY32).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp1FD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1162
static union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY32).tag=40U,(_tmp702.YY32).val=yy1;_tmp702;});}static char _tmp201[16U]="datatypefield_t";
# 1156 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp201,_tmp201,_tmp201 + 16U};
union Cyc_YYSTYPE*_tmp1FF=yy1;struct Cyc_Absyn_Datatypefield*_tmp200;if((((union Cyc_YYSTYPE*)_tmp1FF)->YY33).tag == 41){_LL1: _tmp200=(_tmp1FF->YY33).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp200;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1163
static union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY33).tag=41U,(_tmp703.YY33).val=yy1;_tmp703;});}static char _tmp204[27U]="list_t<datatypefield_t,`H>";
# 1157 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp204,_tmp204,_tmp204 + 27U};
union Cyc_YYSTYPE*_tmp202=yy1;struct Cyc_List_List*_tmp203;if((((union Cyc_YYSTYPE*)_tmp202)->YY34).tag == 42){_LL1: _tmp203=(_tmp202->YY34).val;_LL2: {struct Cyc_List_List*yy=_tmp203;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1164
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY34).tag=42U,(_tmp704.YY34).val=yy1;_tmp704;});}static char _tmp207[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1158 "parse.y"
static struct _tuple26 Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp207,_tmp207,_tmp207 + 41U};
union Cyc_YYSTYPE*_tmp205=yy1;struct _tuple26 _tmp206;if((((union Cyc_YYSTYPE*)_tmp205)->YY35).tag == 43){_LL1: _tmp206=(_tmp205->YY35).val;_LL2: {struct _tuple26 yy=_tmp206;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1165
static union Cyc_YYSTYPE Cyc_YY35(struct _tuple26 yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY35).tag=43U,(_tmp705.YY35).val=yy1;_tmp705;});}static char _tmp20A[17U]="list_t<var_t,`H>";
# 1159 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20A,_tmp20A,_tmp20A + 17U};
union Cyc_YYSTYPE*_tmp208=yy1;struct Cyc_List_List*_tmp209;if((((union Cyc_YYSTYPE*)_tmp208)->YY36).tag == 44){_LL1: _tmp209=(_tmp208->YY36).val;_LL2: {struct Cyc_List_List*yy=_tmp209;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1166
static union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY36).tag=44U,(_tmp706.YY36).val=yy1;_tmp706;});}static char _tmp20D[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1160 "parse.y"
static struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20D,_tmp20D,_tmp20D + 31U};
union Cyc_YYSTYPE*_tmp20B=yy1;struct _tuple8*_tmp20C;if((((union Cyc_YYSTYPE*)_tmp20B)->YY37).tag == 45){_LL1: _tmp20C=(_tmp20B->YY37).val;_LL2: {struct _tuple8*yy=_tmp20C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1167
static union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY37).tag=45U,(_tmp707.YY37).val=yy1;_tmp707;});}static char _tmp210[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1161 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp210,_tmp210,_tmp210 + 42U};
union Cyc_YYSTYPE*_tmp20E=yy1;struct Cyc_List_List*_tmp20F;if((((union Cyc_YYSTYPE*)_tmp20E)->YY38).tag == 46){_LL1: _tmp20F=(_tmp20E->YY38).val;_LL2: {struct Cyc_List_List*yy=_tmp20F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1168
static union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY38).tag=46U,(_tmp708.YY38).val=yy1;_tmp708;});}static char _tmp213[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1162 "parse.y"
static struct _tuple27*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp213,_tmp213,_tmp213 + 115U};
union Cyc_YYSTYPE*_tmp211=yy1;struct _tuple27*_tmp212;if((((union Cyc_YYSTYPE*)_tmp211)->YY39).tag == 47){_LL1: _tmp212=(_tmp211->YY39).val;_LL2: {struct _tuple27*yy=_tmp212;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1169
static union Cyc_YYSTYPE Cyc_YY39(struct _tuple27*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY39).tag=47U,(_tmp709.YY39).val=yy1;_tmp709;});}static char _tmp216[8U]="types_t";
# 1163 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp216,_tmp216,_tmp216 + 8U};
union Cyc_YYSTYPE*_tmp214=yy1;struct Cyc_List_List*_tmp215;if((((union Cyc_YYSTYPE*)_tmp214)->YY40).tag == 48){_LL1: _tmp215=(_tmp214->YY40).val;_LL2: {struct Cyc_List_List*yy=_tmp215;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1170
static union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY40).tag=48U,(_tmp70A.YY40).val=yy1;_tmp70A;});}static char _tmp219[24U]="list_t<designator_t,`H>";
# 1165 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp219,_tmp219,_tmp219 + 24U};
union Cyc_YYSTYPE*_tmp217=yy1;struct Cyc_List_List*_tmp218;if((((union Cyc_YYSTYPE*)_tmp217)->YY41).tag == 49){_LL1: _tmp218=(_tmp217->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp218;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1172
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY41).tag=49U,(_tmp70B.YY41).val=yy1;_tmp70B;});}static char _tmp21C[13U]="designator_t";
# 1166 "parse.y"
static void*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21C,_tmp21C,_tmp21C + 13U};
union Cyc_YYSTYPE*_tmp21A=yy1;void*_tmp21B;if((((union Cyc_YYSTYPE*)_tmp21A)->YY42).tag == 50){_LL1: _tmp21B=(_tmp21A->YY42).val;_LL2: {void*yy=_tmp21B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1173
static union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY42).tag=50U,(_tmp70C.YY42).val=yy1;_tmp70C;});}static char _tmp21F[7U]="kind_t";
# 1167 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21F,_tmp21F,_tmp21F + 7U};
union Cyc_YYSTYPE*_tmp21D=yy1;struct Cyc_Absyn_Kind*_tmp21E;if((((union Cyc_YYSTYPE*)_tmp21D)->YY43).tag == 51){_LL1: _tmp21E=(_tmp21D->YY43).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp21E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1174
static union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY43).tag=51U,(_tmp70D.YY43).val=yy1;_tmp70D;});}static char _tmp222[7U]="type_t";
# 1168 "parse.y"
static void*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp222,_tmp222,_tmp222 + 7U};
union Cyc_YYSTYPE*_tmp220=yy1;void*_tmp221;if((((union Cyc_YYSTYPE*)_tmp220)->YY44).tag == 52){_LL1: _tmp221=(_tmp220->YY44).val;_LL2: {void*yy=_tmp221;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1175
static union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY44).tag=52U,(_tmp70E.YY44).val=yy1;_tmp70E;});}static char _tmp225[23U]="list_t<attribute_t,`H>";
# 1169 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp225,_tmp225,_tmp225 + 23U};
union Cyc_YYSTYPE*_tmp223=yy1;struct Cyc_List_List*_tmp224;if((((union Cyc_YYSTYPE*)_tmp223)->YY45).tag == 53){_LL1: _tmp224=(_tmp223->YY45).val;_LL2: {struct Cyc_List_List*yy=_tmp224;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1176
static union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY45).tag=53U,(_tmp70F.YY45).val=yy1;_tmp70F;});}static char _tmp228[12U]="attribute_t";
# 1170 "parse.y"
static void*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp228,_tmp228,_tmp228 + 12U};
union Cyc_YYSTYPE*_tmp226=yy1;void*_tmp227;if((((union Cyc_YYSTYPE*)_tmp226)->YY46).tag == 54){_LL1: _tmp227=(_tmp226->YY46).val;_LL2: {void*yy=_tmp227;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1177
static union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY46).tag=54U,(_tmp710.YY46).val=yy1;_tmp710;});}static char _tmp22B[12U]="enumfield_t";
# 1171 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22B,_tmp22B,_tmp22B + 12U};
union Cyc_YYSTYPE*_tmp229=yy1;struct Cyc_Absyn_Enumfield*_tmp22A;if((((union Cyc_YYSTYPE*)_tmp229)->YY47).tag == 55){_LL1: _tmp22A=(_tmp229->YY47).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp22A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1178
static union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY47).tag=55U,(_tmp711.YY47).val=yy1;_tmp711;});}static char _tmp22E[23U]="list_t<enumfield_t,`H>";
# 1172 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22E,_tmp22E,_tmp22E + 23U};
union Cyc_YYSTYPE*_tmp22C=yy1;struct Cyc_List_List*_tmp22D;if((((union Cyc_YYSTYPE*)_tmp22C)->YY48).tag == 56){_LL1: _tmp22D=(_tmp22C->YY48).val;_LL2: {struct Cyc_List_List*yy=_tmp22D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1179
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY48).tag=56U,(_tmp712.YY48).val=yy1;_tmp712;});}static char _tmp231[11U]="type_opt_t";
# 1173 "parse.y"
static void*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp231,_tmp231,_tmp231 + 11U};
union Cyc_YYSTYPE*_tmp22F=yy1;void*_tmp230;if((((union Cyc_YYSTYPE*)_tmp22F)->YY49).tag == 57){_LL1: _tmp230=(_tmp22F->YY49).val;_LL2: {void*yy=_tmp230;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1180
static union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY49).tag=57U,(_tmp713.YY49).val=yy1;_tmp713;});}static char _tmp234[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1174 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp234,_tmp234,_tmp234 + 31U};
union Cyc_YYSTYPE*_tmp232=yy1;struct Cyc_List_List*_tmp233;if((((union Cyc_YYSTYPE*)_tmp232)->YY50).tag == 58){_LL1: _tmp233=(_tmp232->YY50).val;_LL2: {struct Cyc_List_List*yy=_tmp233;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1181
static union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY50).tag=58U,(_tmp714.YY50).val=yy1;_tmp714;});}static char _tmp237[11U]="booltype_t";
# 1175 "parse.y"
static void*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp237,_tmp237,_tmp237 + 11U};
union Cyc_YYSTYPE*_tmp235=yy1;void*_tmp236;if((((union Cyc_YYSTYPE*)_tmp235)->YY51).tag == 59){_LL1: _tmp236=(_tmp235->YY51).val;_LL2: {void*yy=_tmp236;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1182
static union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY51).tag=59U,(_tmp715.YY51).val=yy1;_tmp715;});}static char _tmp23A[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1176 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23A,_tmp23A,_tmp23A + 45U};
union Cyc_YYSTYPE*_tmp238=yy1;struct Cyc_List_List*_tmp239;if((((union Cyc_YYSTYPE*)_tmp238)->YY52).tag == 60){_LL1: _tmp239=(_tmp238->YY52).val;_LL2: {struct Cyc_List_List*yy=_tmp239;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1183
static union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp716;(_tmp716.YY52).tag=60U,(_tmp716.YY52).val=yy1;_tmp716;});}static char _tmp23D[58U]="$(list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1177 "parse.y"
static struct _tuple28*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23D,_tmp23D,_tmp23D + 58U};
union Cyc_YYSTYPE*_tmp23B=yy1;struct _tuple28*_tmp23C;if((((union Cyc_YYSTYPE*)_tmp23B)->YY53).tag == 61){_LL1: _tmp23C=(_tmp23B->YY53).val;_LL2: {struct _tuple28*yy=_tmp23C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1184
static union Cyc_YYSTYPE Cyc_YY53(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp717;(_tmp717.YY53).tag=61U,(_tmp717.YY53).val=yy1;_tmp717;});}static char _tmp240[18U]="list_t<qvar_t,`H>";
# 1178 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp240,_tmp240,_tmp240 + 18U};
union Cyc_YYSTYPE*_tmp23E=yy1;struct Cyc_List_List*_tmp23F;if((((union Cyc_YYSTYPE*)_tmp23E)->YY54).tag == 62){_LL1: _tmp23F=(_tmp23E->YY54).val;_LL2: {struct Cyc_List_List*yy=_tmp23F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1185
static union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp718;(_tmp718.YY54).tag=62U,(_tmp718.YY54).val=yy1;_tmp718;});}static char _tmp243[20U]="pointer_qual_t<`yy>";
# 1179 "parse.y"
static void*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp243,_tmp243,_tmp243 + 20U};
union Cyc_YYSTYPE*_tmp241=yy1;void*_tmp242;if((((union Cyc_YYSTYPE*)_tmp241)->YY55).tag == 63){_LL1: _tmp242=(_tmp241->YY55).val;_LL2: {void*yy=_tmp242;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1186
static union Cyc_YYSTYPE Cyc_YY55(void*yy1){return({union Cyc_YYSTYPE _tmp719;(_tmp719.YY55).tag=63U,(_tmp719.YY55).val=yy1;_tmp719;});}static char _tmp246[21U]="pointer_quals_t<`yy>";
# 1180 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp246,_tmp246,_tmp246 + 21U};
union Cyc_YYSTYPE*_tmp244=yy1;struct Cyc_List_List*_tmp245;if((((union Cyc_YYSTYPE*)_tmp244)->YY56).tag == 64){_LL1: _tmp245=(_tmp244->YY56).val;_LL2: {struct Cyc_List_List*yy=_tmp245;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1187
static union Cyc_YYSTYPE Cyc_YY56(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71A;(_tmp71A.YY56).tag=64U,(_tmp71A.YY56).val=yy1;_tmp71A;});}static char _tmp249[10U]="exp_opt_t";
# 1181 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp249,_tmp249,_tmp249 + 10U};
union Cyc_YYSTYPE*_tmp247=yy1;struct Cyc_Absyn_Exp*_tmp248;if((((union Cyc_YYSTYPE*)_tmp247)->YY57).tag == 65){_LL1: _tmp248=(_tmp247->YY57).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp248;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1188
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp71B;(_tmp71B.YY57).tag=65U,(_tmp71B.YY57).val=yy1;_tmp71B;});}static char _tmp24C[10U]="raw_exp_t";
# 1182 "parse.y"
static void*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24C,_tmp24C,_tmp24C + 10U};
union Cyc_YYSTYPE*_tmp24A=yy1;void*_tmp24B;if((((union Cyc_YYSTYPE*)_tmp24A)->YY58).tag == 66){_LL1: _tmp24B=(_tmp24A->YY58).val;_LL2: {void*yy=_tmp24B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1189
static union Cyc_YYSTYPE Cyc_YY58(void*yy1){return({union Cyc_YYSTYPE _tmp71C;(_tmp71C.YY58).tag=66U,(_tmp71C.YY58).val=yy1;_tmp71C;});}static char _tmp24F[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1184 "parse.y"
static struct _tuple29*Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp24F,_tmp24F,_tmp24F + 112U};
union Cyc_YYSTYPE*_tmp24D=yy1;struct _tuple29*_tmp24E;if((((union Cyc_YYSTYPE*)_tmp24D)->YY59).tag == 67){_LL1: _tmp24E=(_tmp24D->YY59).val;_LL2: {struct _tuple29*yy=_tmp24E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1191
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp71D;(_tmp71D.YY59).tag=67U,(_tmp71D.YY59).val=yy1;_tmp71D;});}static char _tmp252[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1185 "parse.y"
static struct _tuple30*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp252,_tmp252,_tmp252 + 73U};
union Cyc_YYSTYPE*_tmp250=yy1;struct _tuple30*_tmp251;if((((union Cyc_YYSTYPE*)_tmp250)->YY60).tag == 68){_LL1: _tmp251=(_tmp250->YY60).val;_LL2: {struct _tuple30*yy=_tmp251;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1192
static union Cyc_YYSTYPE Cyc_YY60(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp71E;(_tmp71E.YY60).tag=68U,(_tmp71E.YY60).val=yy1;_tmp71E;});}static char _tmp255[28U]="list_t<string_t<`H>@`H, `H>";
# 1186 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp255,_tmp255,_tmp255 + 28U};
union Cyc_YYSTYPE*_tmp253=yy1;struct Cyc_List_List*_tmp254;if((((union Cyc_YYSTYPE*)_tmp253)->YY61).tag == 69){_LL1: _tmp254=(_tmp253->YY61).val;_LL2: {struct Cyc_List_List*yy=_tmp254;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1193
static union Cyc_YYSTYPE Cyc_YY61(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71F;(_tmp71F.YY61).tag=69U,(_tmp71F.YY61).val=yy1;_tmp71F;});}static char _tmp258[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1187 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp258,_tmp258,_tmp258 + 38U};
union Cyc_YYSTYPE*_tmp256=yy1;struct Cyc_List_List*_tmp257;if((((union Cyc_YYSTYPE*)_tmp256)->YY62).tag == 70){_LL1: _tmp257=(_tmp256->YY62).val;_LL2: {struct Cyc_List_List*yy=_tmp257;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1194
static union Cyc_YYSTYPE Cyc_YY62(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp720;(_tmp720.YY62).tag=70U,(_tmp720.YY62).val=yy1;_tmp720;});}static char _tmp25B[26U]="$(string_t<`H>, exp_t)@`H";
# 1188 "parse.y"
static struct _tuple31*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp25B,_tmp25B,_tmp25B + 26U};
union Cyc_YYSTYPE*_tmp259=yy1;struct _tuple31*_tmp25A;if((((union Cyc_YYSTYPE*)_tmp259)->YY63).tag == 71){_LL1: _tmp25A=(_tmp259->YY63).val;_LL2: {struct _tuple31*yy=_tmp25A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1195
static union Cyc_YYSTYPE Cyc_YY63(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp721;(_tmp721.YY63).tag=71U,(_tmp721.YY63).val=yy1;_tmp721;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1211
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp722;_tmp722.timestamp=0,_tmp722.first_line=0,_tmp722.first_column=0,_tmp722.last_line=0,_tmp722.last_column=0;_tmp722;});}
# 1214
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1225 "parse.y"
static short Cyc_yytranslate[380U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,151,2,2,136,149,146,2,133,134,129,143,128,147,138,148,2,2,2,2,2,2,2,2,2,2,137,125,131,130,132,142,141,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,139,2,140,145,135,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,126,144,127,150,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124};static char _tmp25C[2U]="$";static char _tmp25D[6U]="error";static char _tmp25E[12U]="$undefined.";static char _tmp25F[5U]="AUTO";static char _tmp260[9U]="REGISTER";static char _tmp261[7U]="STATIC";static char _tmp262[7U]="EXTERN";static char _tmp263[8U]="TYPEDEF";static char _tmp264[5U]="VOID";static char _tmp265[5U]="CHAR";static char _tmp266[6U]="SHORT";static char _tmp267[4U]="INT";static char _tmp268[5U]="LONG";static char _tmp269[6U]="FLOAT";static char _tmp26A[7U]="DOUBLE";static char _tmp26B[7U]="SIGNED";static char _tmp26C[9U]="UNSIGNED";static char _tmp26D[6U]="CONST";static char _tmp26E[9U]="VOLATILE";static char _tmp26F[9U]="RESTRICT";static char _tmp270[7U]="STRUCT";static char _tmp271[6U]="UNION";static char _tmp272[5U]="CASE";static char _tmp273[8U]="DEFAULT";static char _tmp274[7U]="INLINE";static char _tmp275[7U]="SIZEOF";static char _tmp276[9U]="OFFSETOF";static char _tmp277[3U]="IF";static char _tmp278[5U]="ELSE";static char _tmp279[7U]="SWITCH";static char _tmp27A[6U]="WHILE";static char _tmp27B[3U]="DO";static char _tmp27C[4U]="FOR";static char _tmp27D[5U]="GOTO";static char _tmp27E[9U]="CONTINUE";static char _tmp27F[6U]="BREAK";static char _tmp280[7U]="RETURN";static char _tmp281[5U]="ENUM";static char _tmp282[7U]="TYPEOF";static char _tmp283[16U]="BUILTIN_VA_LIST";static char _tmp284[10U]="EXTENSION";static char _tmp285[8U]="NULL_kw";static char _tmp286[4U]="LET";static char _tmp287[6U]="THROW";static char _tmp288[4U]="TRY";static char _tmp289[6U]="CATCH";static char _tmp28A[7U]="EXPORT";static char _tmp28B[9U]="OVERRIDE";static char _tmp28C[5U]="HIDE";static char _tmp28D[4U]="NEW";static char _tmp28E[9U]="ABSTRACT";static char _tmp28F[9U]="FALLTHRU";static char _tmp290[6U]="USING";static char _tmp291[10U]="NAMESPACE";static char _tmp292[9U]="DATATYPE";static char _tmp293[7U]="MALLOC";static char _tmp294[8U]="RMALLOC";static char _tmp295[15U]="RMALLOC_INLINE";static char _tmp296[7U]="CALLOC";static char _tmp297[8U]="RCALLOC";static char _tmp298[5U]="SWAP";static char _tmp299[9U]="REGION_T";static char _tmp29A[6U]="TAG_T";static char _tmp29B[7U]="REGION";static char _tmp29C[5U]="RNEW";static char _tmp29D[8U]="REGIONS";static char _tmp29E[7U]="PORTON";static char _tmp29F[8U]="PORTOFF";static char _tmp2A0[7U]="PRAGMA";static char _tmp2A1[10U]="TEMPESTON";static char _tmp2A2[11U]="TEMPESTOFF";static char _tmp2A3[8U]="NUMELTS";static char _tmp2A4[8U]="VALUEOF";static char _tmp2A5[10U]="VALUEOF_T";static char _tmp2A6[9U]="TAGCHECK";static char _tmp2A7[13U]="NUMELTS_QUAL";static char _tmp2A8[10U]="THIN_QUAL";static char _tmp2A9[9U]="FAT_QUAL";static char _tmp2AA[13U]="NOTNULL_QUAL";static char _tmp2AB[14U]="NULLABLE_QUAL";static char _tmp2AC[14U]="REQUIRES_QUAL";static char _tmp2AD[13U]="ENSURES_QUAL";static char _tmp2AE[12U]="REGION_QUAL";static char _tmp2AF[16U]="NOZEROTERM_QUAL";static char _tmp2B0[14U]="ZEROTERM_QUAL";static char _tmp2B1[12U]="TAGGED_QUAL";static char _tmp2B2[12U]="ASSERT_QUAL";static char _tmp2B3[16U]="EXTENSIBLE_QUAL";static char _tmp2B4[7U]="PTR_OP";static char _tmp2B5[7U]="INC_OP";static char _tmp2B6[7U]="DEC_OP";static char _tmp2B7[8U]="LEFT_OP";static char _tmp2B8[9U]="RIGHT_OP";static char _tmp2B9[6U]="LE_OP";static char _tmp2BA[6U]="GE_OP";static char _tmp2BB[6U]="EQ_OP";static char _tmp2BC[6U]="NE_OP";static char _tmp2BD[7U]="AND_OP";static char _tmp2BE[6U]="OR_OP";static char _tmp2BF[11U]="MUL_ASSIGN";static char _tmp2C0[11U]="DIV_ASSIGN";static char _tmp2C1[11U]="MOD_ASSIGN";static char _tmp2C2[11U]="ADD_ASSIGN";static char _tmp2C3[11U]="SUB_ASSIGN";static char _tmp2C4[12U]="LEFT_ASSIGN";static char _tmp2C5[13U]="RIGHT_ASSIGN";static char _tmp2C6[11U]="AND_ASSIGN";static char _tmp2C7[11U]="XOR_ASSIGN";static char _tmp2C8[10U]="OR_ASSIGN";static char _tmp2C9[9U]="ELLIPSIS";static char _tmp2CA[11U]="LEFT_RIGHT";static char _tmp2CB[12U]="COLON_COLON";static char _tmp2CC[11U]="IDENTIFIER";static char _tmp2CD[17U]="INTEGER_CONSTANT";static char _tmp2CE[7U]="STRING";static char _tmp2CF[8U]="WSTRING";static char _tmp2D0[19U]="CHARACTER_CONSTANT";static char _tmp2D1[20U]="WCHARACTER_CONSTANT";static char _tmp2D2[18U]="FLOATING_CONSTANT";static char _tmp2D3[9U]="TYPE_VAR";static char _tmp2D4[13U]="TYPEDEF_NAME";static char _tmp2D5[16U]="QUAL_IDENTIFIER";static char _tmp2D6[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D7[10U]="ATTRIBUTE";static char _tmp2D8[8U]="ASM_TOK";static char _tmp2D9[4U]="';'";static char _tmp2DA[4U]="'{'";static char _tmp2DB[4U]="'}'";static char _tmp2DC[4U]="','";static char _tmp2DD[4U]="'*'";static char _tmp2DE[4U]="'='";static char _tmp2DF[4U]="'<'";static char _tmp2E0[4U]="'>'";static char _tmp2E1[4U]="'('";static char _tmp2E2[4U]="')'";static char _tmp2E3[4U]="'_'";static char _tmp2E4[4U]="'$'";static char _tmp2E5[4U]="':'";static char _tmp2E6[4U]="'.'";static char _tmp2E7[4U]="'['";static char _tmp2E8[4U]="']'";static char _tmp2E9[4U]="'@'";static char _tmp2EA[4U]="'?'";static char _tmp2EB[4U]="'+'";static char _tmp2EC[4U]="'|'";static char _tmp2ED[4U]="'^'";static char _tmp2EE[4U]="'&'";static char _tmp2EF[4U]="'-'";static char _tmp2F0[4U]="'/'";static char _tmp2F1[4U]="'%'";static char _tmp2F2[4U]="'~'";static char _tmp2F3[4U]="'!'";static char _tmp2F4[5U]="prog";static char _tmp2F5[17U]="translation_unit";static char _tmp2F6[18U]="tempest_on_action";static char _tmp2F7[19U]="tempest_off_action";static char _tmp2F8[16U]="extern_c_action";static char _tmp2F9[13U]="end_extern_c";static char _tmp2FA[14U]="hide_list_opt";static char _tmp2FB[17U]="hide_list_values";static char _tmp2FC[16U]="export_list_opt";static char _tmp2FD[12U]="export_list";static char _tmp2FE[19U]="export_list_values";static char _tmp2FF[13U]="override_opt";static char _tmp300[21U]="external_declaration";static char _tmp301[15U]="optional_comma";static char _tmp302[20U]="function_definition";static char _tmp303[21U]="function_definition2";static char _tmp304[13U]="using_action";static char _tmp305[15U]="unusing_action";static char _tmp306[17U]="namespace_action";static char _tmp307[19U]="unnamespace_action";static char _tmp308[12U]="declaration";static char _tmp309[17U]="declaration_list";static char _tmp30A[23U]="declaration_specifiers";static char _tmp30B[24U]="storage_class_specifier";static char _tmp30C[15U]="attributes_opt";static char _tmp30D[11U]="attributes";static char _tmp30E[15U]="attribute_list";static char _tmp30F[10U]="attribute";static char _tmp310[15U]="type_specifier";static char _tmp311[25U]="type_specifier_notypedef";static char _tmp312[5U]="kind";static char _tmp313[15U]="type_qualifier";static char _tmp314[15U]="enum_specifier";static char _tmp315[11U]="enum_field";static char _tmp316[22U]="enum_declaration_list";static char _tmp317[26U]="struct_or_union_specifier";static char _tmp318[16U]="type_params_opt";static char _tmp319[16U]="struct_or_union";static char _tmp31A[24U]="struct_declaration_list";static char _tmp31B[25U]="struct_declaration_list0";static char _tmp31C[21U]="init_declarator_list";static char _tmp31D[22U]="init_declarator_list0";static char _tmp31E[16U]="init_declarator";static char _tmp31F[19U]="struct_declaration";static char _tmp320[25U]="specifier_qualifier_list";static char _tmp321[35U]="notypedef_specifier_qualifier_list";static char _tmp322[23U]="struct_declarator_list";static char _tmp323[24U]="struct_declarator_list0";static char _tmp324[18U]="struct_declarator";static char _tmp325[20U]="requires_clause_opt";static char _tmp326[19U]="ensures_clause_opt";static char _tmp327[19U]="datatype_specifier";static char _tmp328[14U]="qual_datatype";static char _tmp329[19U]="datatypefield_list";static char _tmp32A[20U]="datatypefield_scope";static char _tmp32B[14U]="datatypefield";static char _tmp32C[11U]="declarator";static char _tmp32D[23U]="declarator_withtypedef";static char _tmp32E[18U]="direct_declarator";static char _tmp32F[30U]="direct_declarator_withtypedef";static char _tmp330[8U]="pointer";static char _tmp331[12U]="one_pointer";static char _tmp332[14U]="pointer_quals";static char _tmp333[13U]="pointer_qual";static char _tmp334[23U]="pointer_null_and_bound";static char _tmp335[14U]="pointer_bound";static char _tmp336[18U]="zeroterm_qual_opt";static char _tmp337[8U]="rgn_opt";static char _tmp338[11U]="tqual_list";static char _tmp339[20U]="parameter_type_list";static char _tmp33A[9U]="type_var";static char _tmp33B[16U]="optional_effect";static char _tmp33C[19U]="optional_rgn_order";static char _tmp33D[10U]="rgn_order";static char _tmp33E[16U]="optional_inject";static char _tmp33F[11U]="effect_set";static char _tmp340[14U]="atomic_effect";static char _tmp341[11U]="region_set";static char _tmp342[15U]="parameter_list";static char _tmp343[22U]="parameter_declaration";static char _tmp344[16U]="identifier_list";static char _tmp345[17U]="identifier_list0";static char _tmp346[12U]="initializer";static char _tmp347[18U]="array_initializer";static char _tmp348[17U]="initializer_list";static char _tmp349[12U]="designation";static char _tmp34A[16U]="designator_list";static char _tmp34B[11U]="designator";static char _tmp34C[10U]="type_name";static char _tmp34D[14U]="any_type_name";static char _tmp34E[15U]="type_name_list";static char _tmp34F[20U]="abstract_declarator";static char _tmp350[27U]="direct_abstract_declarator";static char _tmp351[10U]="statement";static char _tmp352[18U]="labeled_statement";static char _tmp353[21U]="expression_statement";static char _tmp354[19U]="compound_statement";static char _tmp355[16U]="block_item_list";static char _tmp356[20U]="selection_statement";static char _tmp357[15U]="switch_clauses";static char _tmp358[20U]="iteration_statement";static char _tmp359[15U]="jump_statement";static char _tmp35A[12U]="exp_pattern";static char _tmp35B[20U]="conditional_pattern";static char _tmp35C[19U]="logical_or_pattern";static char _tmp35D[20U]="logical_and_pattern";static char _tmp35E[21U]="inclusive_or_pattern";static char _tmp35F[21U]="exclusive_or_pattern";static char _tmp360[12U]="and_pattern";static char _tmp361[17U]="equality_pattern";static char _tmp362[19U]="relational_pattern";static char _tmp363[14U]="shift_pattern";static char _tmp364[17U]="additive_pattern";static char _tmp365[23U]="multiplicative_pattern";static char _tmp366[13U]="cast_pattern";static char _tmp367[14U]="unary_pattern";static char _tmp368[16U]="postfix_pattern";static char _tmp369[16U]="primary_pattern";static char _tmp36A[8U]="pattern";static char _tmp36B[19U]="tuple_pattern_list";static char _tmp36C[20U]="tuple_pattern_list0";static char _tmp36D[14U]="field_pattern";static char _tmp36E[19U]="field_pattern_list";static char _tmp36F[20U]="field_pattern_list0";static char _tmp370[11U]="expression";static char _tmp371[22U]="assignment_expression";static char _tmp372[20U]="assignment_operator";static char _tmp373[23U]="conditional_expression";static char _tmp374[20U]="constant_expression";static char _tmp375[22U]="logical_or_expression";static char _tmp376[23U]="logical_and_expression";static char _tmp377[24U]="inclusive_or_expression";static char _tmp378[24U]="exclusive_or_expression";static char _tmp379[15U]="and_expression";static char _tmp37A[20U]="equality_expression";static char _tmp37B[22U]="relational_expression";static char _tmp37C[17U]="shift_expression";static char _tmp37D[20U]="additive_expression";static char _tmp37E[26U]="multiplicative_expression";static char _tmp37F[16U]="cast_expression";static char _tmp380[17U]="unary_expression";static char _tmp381[9U]="asm_expr";static char _tmp382[13U]="volatile_opt";static char _tmp383[12U]="asm_out_opt";static char _tmp384[12U]="asm_outlist";static char _tmp385[11U]="asm_in_opt";static char _tmp386[11U]="asm_inlist";static char _tmp387[11U]="asm_io_elt";static char _tmp388[16U]="asm_clobber_opt";static char _tmp389[17U]="asm_clobber_list";static char _tmp38A[15U]="unary_operator";static char _tmp38B[19U]="postfix_expression";static char _tmp38C[17U]="field_expression";static char _tmp38D[19U]="primary_expression";static char _tmp38E[25U]="argument_expression_list";static char _tmp38F[26U]="argument_expression_list0";static char _tmp390[9U]="constant";static char _tmp391[20U]="qual_opt_identifier";static char _tmp392[17U]="qual_opt_typedef";static char _tmp393[18U]="struct_union_name";static char _tmp394[11U]="field_name";static char _tmp395[12U]="right_angle";
# 1597 "parse.y"
static struct _fat_ptr Cyc_yytname[314U]={{_tmp25C,_tmp25C,_tmp25C + 2U},{_tmp25D,_tmp25D,_tmp25D + 6U},{_tmp25E,_tmp25E,_tmp25E + 12U},{_tmp25F,_tmp25F,_tmp25F + 5U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 7U},{_tmp262,_tmp262,_tmp262 + 7U},{_tmp263,_tmp263,_tmp263 + 8U},{_tmp264,_tmp264,_tmp264 + 5U},{_tmp265,_tmp265,_tmp265 + 5U},{_tmp266,_tmp266,_tmp266 + 6U},{_tmp267,_tmp267,_tmp267 + 4U},{_tmp268,_tmp268,_tmp268 + 5U},{_tmp269,_tmp269,_tmp269 + 6U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 7U},{_tmp26C,_tmp26C,_tmp26C + 9U},{_tmp26D,_tmp26D,_tmp26D + 6U},{_tmp26E,_tmp26E,_tmp26E + 9U},{_tmp26F,_tmp26F,_tmp26F + 9U},{_tmp270,_tmp270,_tmp270 + 7U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 5U},{_tmp273,_tmp273,_tmp273 + 8U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 3U},{_tmp278,_tmp278,_tmp278 + 5U},{_tmp279,_tmp279,_tmp279 + 7U},{_tmp27A,_tmp27A,_tmp27A + 6U},{_tmp27B,_tmp27B,_tmp27B + 3U},{_tmp27C,_tmp27C,_tmp27C + 4U},{_tmp27D,_tmp27D,_tmp27D + 5U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 6U},{_tmp280,_tmp280,_tmp280 + 7U},{_tmp281,_tmp281,_tmp281 + 5U},{_tmp282,_tmp282,_tmp282 + 7U},{_tmp283,_tmp283,_tmp283 + 16U},{_tmp284,_tmp284,_tmp284 + 10U},{_tmp285,_tmp285,_tmp285 + 8U},{_tmp286,_tmp286,_tmp286 + 4U},{_tmp287,_tmp287,_tmp287 + 6U},{_tmp288,_tmp288,_tmp288 + 4U},{_tmp289,_tmp289,_tmp289 + 6U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 9U},{_tmp28C,_tmp28C,_tmp28C + 5U},{_tmp28D,_tmp28D,_tmp28D + 4U},{_tmp28E,_tmp28E,_tmp28E + 9U},{_tmp28F,_tmp28F,_tmp28F + 9U},{_tmp290,_tmp290,_tmp290 + 6U},{_tmp291,_tmp291,_tmp291 + 10U},{_tmp292,_tmp292,_tmp292 + 9U},{_tmp293,_tmp293,_tmp293 + 7U},{_tmp294,_tmp294,_tmp294 + 8U},{_tmp295,_tmp295,_tmp295 + 15U},{_tmp296,_tmp296,_tmp296 + 7U},{_tmp297,_tmp297,_tmp297 + 8U},{_tmp298,_tmp298,_tmp298 + 5U},{_tmp299,_tmp299,_tmp299 + 9U},{_tmp29A,_tmp29A,_tmp29A + 6U},{_tmp29B,_tmp29B,_tmp29B + 7U},{_tmp29C,_tmp29C,_tmp29C + 5U},{_tmp29D,_tmp29D,_tmp29D + 8U},{_tmp29E,_tmp29E,_tmp29E + 7U},{_tmp29F,_tmp29F,_tmp29F + 8U},{_tmp2A0,_tmp2A0,_tmp2A0 + 7U},{_tmp2A1,_tmp2A1,_tmp2A1 + 10U},{_tmp2A2,_tmp2A2,_tmp2A2 + 11U},{_tmp2A3,_tmp2A3,_tmp2A3 + 8U},{_tmp2A4,_tmp2A4,_tmp2A4 + 8U},{_tmp2A5,_tmp2A5,_tmp2A5 + 10U},{_tmp2A6,_tmp2A6,_tmp2A6 + 9U},{_tmp2A7,_tmp2A7,_tmp2A7 + 13U},{_tmp2A8,_tmp2A8,_tmp2A8 + 10U},{_tmp2A9,_tmp2A9,_tmp2A9 + 9U},{_tmp2AA,_tmp2AA,_tmp2AA + 13U},{_tmp2AB,_tmp2AB,_tmp2AB + 14U},{_tmp2AC,_tmp2AC,_tmp2AC + 14U},{_tmp2AD,_tmp2AD,_tmp2AD + 13U},{_tmp2AE,_tmp2AE,_tmp2AE + 12U},{_tmp2AF,_tmp2AF,_tmp2AF + 16U},{_tmp2B0,_tmp2B0,_tmp2B0 + 14U},{_tmp2B1,_tmp2B1,_tmp2B1 + 12U},{_tmp2B2,_tmp2B2,_tmp2B2 + 12U},{_tmp2B3,_tmp2B3,_tmp2B3 + 16U},{_tmp2B4,_tmp2B4,_tmp2B4 + 7U},{_tmp2B5,_tmp2B5,_tmp2B5 + 7U},{_tmp2B6,_tmp2B6,_tmp2B6 + 7U},{_tmp2B7,_tmp2B7,_tmp2B7 + 8U},{_tmp2B8,_tmp2B8,_tmp2B8 + 9U},{_tmp2B9,_tmp2B9,_tmp2B9 + 6U},{_tmp2BA,_tmp2BA,_tmp2BA + 6U},{_tmp2BB,_tmp2BB,_tmp2BB + 6U},{_tmp2BC,_tmp2BC,_tmp2BC + 6U},{_tmp2BD,_tmp2BD,_tmp2BD + 7U},{_tmp2BE,_tmp2BE,_tmp2BE + 6U},{_tmp2BF,_tmp2BF,_tmp2BF + 11U},{_tmp2C0,_tmp2C0,_tmp2C0 + 11U},{_tmp2C1,_tmp2C1,_tmp2C1 + 11U},{_tmp2C2,_tmp2C2,_tmp2C2 + 11U},{_tmp2C3,_tmp2C3,_tmp2C3 + 11U},{_tmp2C4,_tmp2C4,_tmp2C4 + 12U},{_tmp2C5,_tmp2C5,_tmp2C5 + 13U},{_tmp2C6,_tmp2C6,_tmp2C6 + 11U},{_tmp2C7,_tmp2C7,_tmp2C7 + 11U},{_tmp2C8,_tmp2C8,_tmp2C8 + 10U},{_tmp2C9,_tmp2C9,_tmp2C9 + 9U},{_tmp2CA,_tmp2CA,_tmp2CA + 11U},{_tmp2CB,_tmp2CB,_tmp2CB + 12U},{_tmp2CC,_tmp2CC,_tmp2CC + 11U},{_tmp2CD,_tmp2CD,_tmp2CD + 17U},{_tmp2CE,_tmp2CE,_tmp2CE + 7U},{_tmp2CF,_tmp2CF,_tmp2CF + 8U},{_tmp2D0,_tmp2D0,_tmp2D0 + 19U},{_tmp2D1,_tmp2D1,_tmp2D1 + 20U},{_tmp2D2,_tmp2D2,_tmp2D2 + 18U},{_tmp2D3,_tmp2D3,_tmp2D3 + 9U},{_tmp2D4,_tmp2D4,_tmp2D4 + 13U},{_tmp2D5,_tmp2D5,_tmp2D5 + 16U},{_tmp2D6,_tmp2D6,_tmp2D6 + 18U},{_tmp2D7,_tmp2D7,_tmp2D7 + 10U},{_tmp2D8,_tmp2D8,_tmp2D8 + 8U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 4U},{_tmp2F3,_tmp2F3,_tmp2F3 + 4U},{_tmp2F4,_tmp2F4,_tmp2F4 + 5U},{_tmp2F5,_tmp2F5,_tmp2F5 + 17U},{_tmp2F6,_tmp2F6,_tmp2F6 + 18U},{_tmp2F7,_tmp2F7,_tmp2F7 + 19U},{_tmp2F8,_tmp2F8,_tmp2F8 + 16U},{_tmp2F9,_tmp2F9,_tmp2F9 + 13U},{_tmp2FA,_tmp2FA,_tmp2FA + 14U},{_tmp2FB,_tmp2FB,_tmp2FB + 17U},{_tmp2FC,_tmp2FC,_tmp2FC + 16U},{_tmp2FD,_tmp2FD,_tmp2FD + 12U},{_tmp2FE,_tmp2FE,_tmp2FE + 19U},{_tmp2FF,_tmp2FF,_tmp2FF + 13U},{_tmp300,_tmp300,_tmp300 + 21U},{_tmp301,_tmp301,_tmp301 + 15U},{_tmp302,_tmp302,_tmp302 + 20U},{_tmp303,_tmp303,_tmp303 + 21U},{_tmp304,_tmp304,_tmp304 + 13U},{_tmp305,_tmp305,_tmp305 + 15U},{_tmp306,_tmp306,_tmp306 + 17U},{_tmp307,_tmp307,_tmp307 + 19U},{_tmp308,_tmp308,_tmp308 + 12U},{_tmp309,_tmp309,_tmp309 + 17U},{_tmp30A,_tmp30A,_tmp30A + 23U},{_tmp30B,_tmp30B,_tmp30B + 24U},{_tmp30C,_tmp30C,_tmp30C + 15U},{_tmp30D,_tmp30D,_tmp30D + 11U},{_tmp30E,_tmp30E,_tmp30E + 15U},{_tmp30F,_tmp30F,_tmp30F + 10U},{_tmp310,_tmp310,_tmp310 + 15U},{_tmp311,_tmp311,_tmp311 + 25U},{_tmp312,_tmp312,_tmp312 + 5U},{_tmp313,_tmp313,_tmp313 + 15U},{_tmp314,_tmp314,_tmp314 + 15U},{_tmp315,_tmp315,_tmp315 + 11U},{_tmp316,_tmp316,_tmp316 + 22U},{_tmp317,_tmp317,_tmp317 + 26U},{_tmp318,_tmp318,_tmp318 + 16U},{_tmp319,_tmp319,_tmp319 + 16U},{_tmp31A,_tmp31A,_tmp31A + 24U},{_tmp31B,_tmp31B,_tmp31B + 25U},{_tmp31C,_tmp31C,_tmp31C + 21U},{_tmp31D,_tmp31D,_tmp31D + 22U},{_tmp31E,_tmp31E,_tmp31E + 16U},{_tmp31F,_tmp31F,_tmp31F + 19U},{_tmp320,_tmp320,_tmp320 + 25U},{_tmp321,_tmp321,_tmp321 + 35U},{_tmp322,_tmp322,_tmp322 + 23U},{_tmp323,_tmp323,_tmp323 + 24U},{_tmp324,_tmp324,_tmp324 + 18U},{_tmp325,_tmp325,_tmp325 + 20U},{_tmp326,_tmp326,_tmp326 + 19U},{_tmp327,_tmp327,_tmp327 + 19U},{_tmp328,_tmp328,_tmp328 + 14U},{_tmp329,_tmp329,_tmp329 + 19U},{_tmp32A,_tmp32A,_tmp32A + 20U},{_tmp32B,_tmp32B,_tmp32B + 14U},{_tmp32C,_tmp32C,_tmp32C + 11U},{_tmp32D,_tmp32D,_tmp32D + 23U},{_tmp32E,_tmp32E,_tmp32E + 18U},{_tmp32F,_tmp32F,_tmp32F + 30U},{_tmp330,_tmp330,_tmp330 + 8U},{_tmp331,_tmp331,_tmp331 + 12U},{_tmp332,_tmp332,_tmp332 + 14U},{_tmp333,_tmp333,_tmp333 + 13U},{_tmp334,_tmp334,_tmp334 + 23U},{_tmp335,_tmp335,_tmp335 + 14U},{_tmp336,_tmp336,_tmp336 + 18U},{_tmp337,_tmp337,_tmp337 + 8U},{_tmp338,_tmp338,_tmp338 + 11U},{_tmp339,_tmp339,_tmp339 + 20U},{_tmp33A,_tmp33A,_tmp33A + 9U},{_tmp33B,_tmp33B,_tmp33B + 16U},{_tmp33C,_tmp33C,_tmp33C + 19U},{_tmp33D,_tmp33D,_tmp33D + 10U},{_tmp33E,_tmp33E,_tmp33E + 16U},{_tmp33F,_tmp33F,_tmp33F + 11U},{_tmp340,_tmp340,_tmp340 + 14U},{_tmp341,_tmp341,_tmp341 + 11U},{_tmp342,_tmp342,_tmp342 + 15U},{_tmp343,_tmp343,_tmp343 + 22U},{_tmp344,_tmp344,_tmp344 + 16U},{_tmp345,_tmp345,_tmp345 + 17U},{_tmp346,_tmp346,_tmp346 + 12U},{_tmp347,_tmp347,_tmp347 + 18U},{_tmp348,_tmp348,_tmp348 + 17U},{_tmp349,_tmp349,_tmp349 + 12U},{_tmp34A,_tmp34A,_tmp34A + 16U},{_tmp34B,_tmp34B,_tmp34B + 11U},{_tmp34C,_tmp34C,_tmp34C + 10U},{_tmp34D,_tmp34D,_tmp34D + 14U},{_tmp34E,_tmp34E,_tmp34E + 15U},{_tmp34F,_tmp34F,_tmp34F + 20U},{_tmp350,_tmp350,_tmp350 + 27U},{_tmp351,_tmp351,_tmp351 + 10U},{_tmp352,_tmp352,_tmp352 + 18U},{_tmp353,_tmp353,_tmp353 + 21U},{_tmp354,_tmp354,_tmp354 + 19U},{_tmp355,_tmp355,_tmp355 + 16U},{_tmp356,_tmp356,_tmp356 + 20U},{_tmp357,_tmp357,_tmp357 + 15U},{_tmp358,_tmp358,_tmp358 + 20U},{_tmp359,_tmp359,_tmp359 + 15U},{_tmp35A,_tmp35A,_tmp35A + 12U},{_tmp35B,_tmp35B,_tmp35B + 20U},{_tmp35C,_tmp35C,_tmp35C + 19U},{_tmp35D,_tmp35D,_tmp35D + 20U},{_tmp35E,_tmp35E,_tmp35E + 21U},{_tmp35F,_tmp35F,_tmp35F + 21U},{_tmp360,_tmp360,_tmp360 + 12U},{_tmp361,_tmp361,_tmp361 + 17U},{_tmp362,_tmp362,_tmp362 + 19U},{_tmp363,_tmp363,_tmp363 + 14U},{_tmp364,_tmp364,_tmp364 + 17U},{_tmp365,_tmp365,_tmp365 + 23U},{_tmp366,_tmp366,_tmp366 + 13U},{_tmp367,_tmp367,_tmp367 + 14U},{_tmp368,_tmp368,_tmp368 + 16U},{_tmp369,_tmp369,_tmp369 + 16U},{_tmp36A,_tmp36A,_tmp36A + 8U},{_tmp36B,_tmp36B,_tmp36B + 19U},{_tmp36C,_tmp36C,_tmp36C + 20U},{_tmp36D,_tmp36D,_tmp36D + 14U},{_tmp36E,_tmp36E,_tmp36E + 19U},{_tmp36F,_tmp36F,_tmp36F + 20U},{_tmp370,_tmp370,_tmp370 + 11U},{_tmp371,_tmp371,_tmp371 + 22U},{_tmp372,_tmp372,_tmp372 + 20U},{_tmp373,_tmp373,_tmp373 + 23U},{_tmp374,_tmp374,_tmp374 + 20U},{_tmp375,_tmp375,_tmp375 + 22U},{_tmp376,_tmp376,_tmp376 + 23U},{_tmp377,_tmp377,_tmp377 + 24U},{_tmp378,_tmp378,_tmp378 + 24U},{_tmp379,_tmp379,_tmp379 + 15U},{_tmp37A,_tmp37A,_tmp37A + 20U},{_tmp37B,_tmp37B,_tmp37B + 22U},{_tmp37C,_tmp37C,_tmp37C + 17U},{_tmp37D,_tmp37D,_tmp37D + 20U},{_tmp37E,_tmp37E,_tmp37E + 26U},{_tmp37F,_tmp37F,_tmp37F + 16U},{_tmp380,_tmp380,_tmp380 + 17U},{_tmp381,_tmp381,_tmp381 + 9U},{_tmp382,_tmp382,_tmp382 + 13U},{_tmp383,_tmp383,_tmp383 + 12U},{_tmp384,_tmp384,_tmp384 + 12U},{_tmp385,_tmp385,_tmp385 + 11U},{_tmp386,_tmp386,_tmp386 + 11U},{_tmp387,_tmp387,_tmp387 + 11U},{_tmp388,_tmp388,_tmp388 + 16U},{_tmp389,_tmp389,_tmp389 + 17U},{_tmp38A,_tmp38A,_tmp38A + 15U},{_tmp38B,_tmp38B,_tmp38B + 19U},{_tmp38C,_tmp38C,_tmp38C + 17U},{_tmp38D,_tmp38D,_tmp38D + 19U},{_tmp38E,_tmp38E,_tmp38E + 25U},{_tmp38F,_tmp38F,_tmp38F + 26U},{_tmp390,_tmp390,_tmp390 + 9U},{_tmp391,_tmp391,_tmp391 + 20U},{_tmp392,_tmp392,_tmp392 + 17U},{_tmp393,_tmp393,_tmp393 + 18U},{_tmp394,_tmp394,_tmp394 + 11U},{_tmp395,_tmp395,_tmp395 + 12U}};
# 1655
static short Cyc_yyr1[562U]={0,152,153,153,153,153,153,153,153,153,153,153,153,154,155,156,157,158,158,159,159,159,160,160,161,161,161,162,162,162,163,163,164,164,164,165,165,166,166,166,166,167,167,168,169,170,171,172,172,172,172,172,172,172,173,173,174,174,174,174,174,174,174,174,174,174,174,175,175,175,175,175,175,175,176,176,177,178,178,179,179,179,179,180,180,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,182,183,183,183,184,184,184,185,185,186,186,186,187,187,187,187,187,188,188,189,189,190,190,191,191,192,193,193,194,194,195,196,196,196,196,196,196,197,197,197,197,197,197,198,199,199,200,200,200,200,201,201,202,202,203,203,203,204,204,205,205,205,205,206,206,206,207,207,208,208,209,209,210,210,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,211,211,211,211,212,212,213,214,214,215,215,215,215,215,215,215,215,216,216,216,217,217,218,218,218,219,219,219,220,220,221,221,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,228,228,229,229,230,230,231,231,231,232,233,233,234,234,235,235,235,235,235,236,236,236,236,237,237,238,238,239,239,240,240,241,241,241,241,241,242,242,243,243,243,244,244,244,244,244,244,244,244,244,244,244,245,245,245,245,245,245,246,247,247,248,248,249,249,249,249,249,249,249,249,250,250,250,250,250,250,251,251,251,251,251,251,252,252,252,252,252,252,252,252,252,252,252,252,252,252,253,253,253,253,253,253,253,253,254,255,255,256,256,257,257,258,258,259,259,260,260,261,261,261,262,262,262,262,262,263,263,263,264,264,264,265,265,265,265,266,266,267,267,267,267,267,267,268,269,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,270,271,271,271,272,272,273,273,274,274,274,275,275,276,276,277,277,277,278,278,278,278,278,278,278,278,278,278,278,279,279,279,279,279,279,279,280,281,281,282,282,283,283,284,284,285,285,286,286,286,287,287,287,287,287,288,288,288,289,289,289,290,290,290,290,291,291,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,292,293,294,294,295,295,295,296,296,297,297,297,298,298,299,300,300,300,301,301,302,302,302,303,303,303,303,303,303,303,303,303,303,303,304,304,304,304,305,305,305,305,305,305,305,305,305,305,305,306,307,307,308,308,308,308,308,309,309,310,310,311,311,312,312,313,313};
# 1715
static short Cyc_yyr2[562U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,1,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1775
static short Cyc_yydefact[1137U]={0,34,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,127,128,63,0,0,97,0,0,73,0,0,165,104,106,0,0,0,13,14,0,0,0,552,231,554,553,555,0,217,0,100,0,217,216,1,0,0,0,0,32,0,0,33,0,56,65,59,83,61,94,95,0,98,0,0,176,0,201,204,99,180,125,71,70,64,0,113,0,58,551,0,552,547,548,549,550,0,125,0,0,391,0,0,0,254,0,393,394,43,45,0,0,0,0,0,0,0,0,166,0,0,0,214,0,0,0,0,215,0,0,0,2,0,0,0,0,47,0,133,134,136,57,66,60,62,129,556,557,125,125,0,54,0,0,36,0,233,0,189,177,202,0,208,209,212,213,0,211,210,222,204,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,536,537,497,0,0,0,0,0,517,515,516,0,420,422,436,444,446,448,450,452,454,457,462,465,468,472,0,474,518,535,533,552,403,0,0,0,0,404,0,0,402,50,0,0,125,0,0,0,143,139,141,274,276,0,0,52,0,0,8,9,0,125,558,559,232,108,0,0,0,181,101,252,0,249,10,11,0,3,0,5,0,48,0,0,0,36,0,130,131,156,124,0,163,0,0,0,0,0,0,0,0,0,0,0,0,552,304,306,0,314,308,0,312,297,298,299,0,300,301,302,0,55,36,136,35,37,281,0,239,255,0,0,235,233,0,219,0,0,0,224,74,223,205,0,118,114,0,0,0,482,0,0,494,438,472,0,439,440,0,0,0,0,0,0,0,0,0,0,0,475,476,498,493,0,478,0,0,0,0,479,477,0,96,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,426,427,428,429,430,431,432,433,434,435,425,0,480,0,524,525,0,0,0,539,0,125,395,0,0,0,417,552,559,0,0,0,0,270,413,418,0,415,0,0,392,410,411,0,408,256,0,0,0,0,277,0,247,144,149,145,147,140,142,233,0,283,275,284,561,560,0,103,105,0,0,107,123,80,79,0,77,218,182,233,251,178,283,253,190,191,0,102,16,30,44,0,46,0,135,137,258,257,36,38,120,132,0,0,0,151,152,159,0,125,125,171,0,0,0,0,0,552,0,0,0,343,344,345,0,0,347,0,0,0,315,309,136,313,307,305,39,0,188,240,0,0,0,246,234,241,159,0,0,0,235,187,221,220,183,219,0,0,225,75,126,119,443,116,112,0,0,0,0,552,259,264,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,538,545,0,544,421,445,0,447,449,451,453,455,456,460,461,458,459,463,464,466,467,469,470,471,424,423,523,520,0,522,0,0,0,406,407,0,273,0,414,268,271,401,0,269,405,398,0,49,0,399,0,278,0,150,146,148,0,235,0,219,0,285,0,233,0,296,280,0,0,125,0,0,0,143,0,125,0,233,0,200,179,250,0,22,4,6,40,0,155,138,156,0,0,154,235,164,173,172,0,0,167,0,0,0,322,0,0,0,0,0,0,342,346,0,0,0,310,303,0,41,282,233,0,243,0,0,161,236,0,159,239,227,184,206,207,225,203,481,0,0,0,260,0,265,484,0,0,0,0,0,534,489,492,0,0,495,499,0,0,473,541,0,0,521,519,0,0,0,0,272,416,419,409,412,400,279,248,159,0,286,287,219,0,0,235,219,0,0,51,235,552,0,76,78,0,192,0,0,235,0,219,0,0,0,17,23,153,0,157,129,162,174,171,171,0,0,0,0,0,0,0,0,0,0,0,0,0,322,348,0,311,42,235,0,244,242,0,185,0,161,235,0,226,530,0,529,0,261,266,0,0,0,0,0,441,442,523,522,504,0,543,526,0,546,437,540,542,0,396,397,161,159,289,295,159,0,288,219,0,129,0,81,193,199,159,0,198,194,219,0,0,0,0,0,0,0,170,169,316,322,0,0,0,0,0,0,350,351,353,355,357,359,361,363,366,371,374,377,381,383,389,390,0,0,319,328,0,0,0,0,0,0,0,0,0,0,349,229,245,0,237,186,228,233,483,0,0,267,485,486,0,0,491,490,0,510,504,500,502,496,527,0,292,161,161,159,290,53,0,0,161,159,195,31,25,0,0,27,0,7,158,122,0,0,0,322,0,387,0,0,384,322,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,385,322,0,330,0,0,0,338,0,0,0,0,0,0,321,0,0,235,532,531,0,0,0,0,511,510,507,505,0,501,528,291,294,161,121,0,196,161,26,24,28,0,0,19,175,317,318,0,0,0,0,322,324,354,0,356,358,360,362,364,365,369,370,367,368,372,373,375,376,378,379,380,0,323,329,331,332,0,340,339,0,334,0,0,0,160,238,230,0,0,0,0,0,513,512,0,506,503,293,0,197,29,18,20,0,320,386,0,382,325,0,322,333,341,335,336,0,263,262,487,0,509,0,508,82,21,0,352,322,326,337,0,514,388,327,488,0,0,0};
# 1892
static short Cyc_yydefgoto[162U]={1134,53,54,55,56,487,878,1040,790,791,962,669,57,320,58,304,59,489,60,491,61,151,62,63,556,243,473,474,244,66,259,245,68,173,174,69,171,70,281,282,136,137,138,283,246,455,502,503,504,679,823,71,72,684,685,686,73,505,74,479,75,76,168,169,77,121,552,335,722,642,78,643,546,713,538,542,543,449,328,268,102,103,569,494,570,429,430,431,247,321,322,644,461,307,308,309,310,311,312,805,313,314,891,892,893,894,895,896,897,898,899,900,901,902,903,904,905,906,432,441,442,433,434,435,315,207,409,208,561,209,210,211,212,213,214,215,216,217,218,219,220,367,368,845,942,943,1023,944,1025,1092,221,222,830,223,588,589,224,225,80,963,436,465};
# 1912
static short Cyc_yypact[1137U]={3023,- -32768,- -32768,- -32768,- -32768,- 26,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3799,284,74,- -32768,3799,1492,- -32768,38,40,- -32768,110,136,58,157,185,- -32768,- -32768,203,100,264,- -32768,236,- -32768,- -32768,- -32768,227,263,1018,276,299,263,- -32768,- -32768,272,331,352,2880,- -32768,496,570,- -32768,1025,3799,3799,3799,- -32768,3799,- -32768,- -32768,476,- -32768,38,3709,71,199,250,750,- -32768,- -32768,392,407,429,- -32768,38,443,7242,- -32768,- -32768,2862,290,- -32768,- -32768,- -32768,- -32768,440,392,470,7242,- -32768,460,2862,459,484,504,- -32768,286,- -32768,- -32768,4015,4015,428,495,2880,2880,7242,424,- -32768,- 52,531,7242,- -32768,133,537,- 52,4495,- -32768,2880,2880,3165,- -32768,2880,3165,2880,3165,- -32768,553,554,- -32768,3575,- -32768,- -32768,- -32768,- -32768,4495,- -32768,- -32768,392,337,1844,- -32768,3709,1025,581,4015,3886,5338,- -32768,71,- -32768,580,- -32768,- -32768,- -32768,- -32768,596,- -32768,- -32768,159,750,4015,- -32768,- -32768,605,641,612,38,7553,633,7650,7242,7405,661,669,672,679,683,687,697,706,721,729,738,7650,7650,- -32768,- -32768,806,7701,2584,740,7701,7701,- -32768,- -32768,- -32768,360,- -32768,- -32768,45,761,716,732,741,280,132,691,138,72,- -32768,671,7701,91,- 39,- -32768,766,130,- -32768,2862,222,775,1133,786,393,1315,- -32768,- -32768,788,7242,392,1315,768,4234,4495,4582,4495,433,- -32768,12,12,- -32768,793,774,- -32768,- -32768,413,392,- -32768,- -32768,- -32768,- -32768,28,781,779,- -32768,- -32768,287,451,- -32768,- -32768,- -32768,787,- -32768,791,- -32768,792,- -32768,133,5450,3709,581,798,4495,- -32768,728,789,38,800,795,336,802,4651,803,819,808,820,5562,2440,4651,- 9,807,- -32768,- -32768,814,1993,1993,1025,1993,- -32768,- -32768,- -32768,822,- -32768,- -32768,- -32768,233,- -32768,581,818,- -32768,- -32768,810,81,838,- -32768,- 4,823,825,385,826,727,811,7242,4015,- -32768,831,- -32768,- -32768,81,38,- -32768,7242,829,2584,- -32768,4495,2584,- -32768,- -32768,- -32768,4763,- -32768,861,7242,7242,7242,7242,7242,7242,853,7242,4495,932,7242,- -32768,- -32768,- -32768,- -32768,834,- -32768,1993,841,453,7242,- -32768,- -32768,7242,- -32768,7701,7242,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7242,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,7242,- -32768,- 52,- -32768,- -32768,5674,- 52,7242,- -32768,837,392,- -32768,847,848,851,- -32768,26,440,- 52,7242,2862,183,- -32768,- -32768,- -32768,857,859,852,2862,- -32768,- -32768,- -32768,854,865,- -32768,467,1133,864,4015,- -32768,858,871,- -32768,4582,4582,4582,- -32768,- -32768,3300,5786,478,- -32768,307,- -32768,- -32768,- 4,- -32768,- -32768,868,890,- -32768,879,- -32768,875,877,884,- -32768,- -32768,1458,- -32768,308,261,- -32768,- -32768,- -32768,4495,- -32768,- -32768,966,- -32768,2880,- -32768,2880,- -32768,- -32768,- -32768,- -32768,581,- -32768,- -32768,- -32768,849,7242,889,891,- -32768,19,232,392,392,817,7242,7242,888,898,7242,880,996,2291,902,- -32768,- -32768,- -32768,475,983,- -32768,5898,2142,2728,- -32768,- -32768,3575,- -32768,- -32768,- -32768,- -32768,4015,- -32768,- -32768,4495,897,4321,- -32768,- -32768,894,951,- 4,900,4408,825,- -32768,- -32768,- -32768,- -32768,727,904,208,636,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,905,912,909,920,914,- -32768,- -32768,709,5450,918,926,927,928,929,516,924,935,936,239,937,946,934,7502,- -32768,- -32768,938,968,- -32768,761,152,716,732,741,280,132,132,691,691,691,691,138,138,72,72,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,940,- -32768,68,4015,5223,4495,- -32768,4495,- -32768,957,- -32768,- -32768,- -32768,- -32768,1265,- -32768,- -32768,- -32768,1440,- -32768,939,- -32768,317,- -32768,4495,- -32768,- -32768,- -32768,964,825,965,727,960,307,4015,4105,6010,- -32768,- -32768,7242,979,392,7354,971,28,3435,972,392,4015,3886,6122,- -32768,308,- -32768,985,1061,- -32768,- -32768,- -32768,874,- -32768,- -32768,728,984,7242,- -32768,825,- -32768,- -32768,- -32768,991,38,508,545,546,7242,821,555,4651,986,6234,6346,566,- -32768,- -32768,994,987,988,1993,- -32768,3709,- -32768,810,999,4015,- -32768,998,- 4,1040,- -32768,995,951,102,- -32768,- -32768,- -32768,- -32768,636,- -32768,1000,204,1000,1001,- -32768,4878,- -32768,- -32768,7242,7242,1104,7242,7405,- -32768,- -32768,- -32768,- 52,- 52,- -32768,997,1006,4993,- -32768,- -32768,7242,7242,- -32768,- -32768,81,719,1021,1023,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,951,1011,- -32768,- -32768,727,81,1022,825,727,1024,564,- -32768,825,1034,1029,- -32768,- -32768,1031,- -32768,81,1035,825,1036,727,1028,3165,1045,1124,- -32768,- -32768,7242,- -32768,4495,- -32768,1042,88,817,4651,1047,1044,1207,1043,1055,4651,7242,6458,592,6570,639,6682,821,- -32768,1058,- -32768,- -32768,825,339,- -32768,- -32768,1051,- -32768,1068,1040,825,4495,- -32768,- -32768,342,- -32768,7242,- -32768,- -32768,5450,1056,1057,1059,1065,- -32768,861,1060,1064,4,1066,- -32768,- -32768,725,- -32768,- -32768,- -32768,- -32768,5223,- -32768,- -32768,1040,951,- -32768,- -32768,951,1069,- -32768,727,1070,4495,1076,- -32768,- -32768,- -32768,951,1073,- -32768,- -32768,727,1072,981,1078,2880,1074,1082,4495,- -32768,- -32768,1182,821,1085,7798,1080,2728,7701,1079,- -32768,50,- -32768,1118,1075,710,771,194,790,349,147,- -32768,- -32768,- -32768,- -32768,1121,7701,1993,- -32768,- -32768,588,4651,602,6794,4651,604,6906,7018,662,1093,- -32768,- -32768,- -32768,7242,1094,- -32768,- -32768,999,- -32768,323,362,- -32768,- -32768,- -32768,4495,1200,- -32768,- -32768,1095,120,365,- -32768,- -32768,- -32768,- -32768,5108,- -32768,1040,1040,951,- -32768,- -32768,1102,1106,1040,951,- -32768,- -32768,- -32768,1108,1109,664,424,- -32768,- -32768,- -32768,613,4651,1110,821,2584,- -32768,4495,1105,- -32768,1695,7701,7242,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7701,7242,- -32768,821,1113,- -32768,4651,4651,615,- -32768,4651,4651,616,4651,617,7130,- -32768,1107,- 4,825,- -32768,- -32768,2728,1123,1111,7242,1126,370,- -32768,- -32768,1129,- -32768,- -32768,- -32768,- -32768,1040,- -32768,1134,- -32768,1040,- -32768,- -32768,- -32768,424,1125,668,- -32768,- -32768,- -32768,1128,1127,1135,7701,821,- -32768,761,381,716,732,732,280,132,132,691,691,691,691,138,138,72,72,- -32768,- -32768,- -32768,397,- -32768,- -32768,- -32768,- -32768,4651,- -32768,- -32768,4651,- -32768,4651,4651,625,- -32768,- -32768,- -32768,1137,757,1131,4495,628,- -32768,1139,1129,- -32768,- -32768,- -32768,1136,- -32768,- -32768,- -32768,- -32768,424,- -32768,1000,204,- -32768,- -32768,7242,1695,- -32768,- -32768,- -32768,- -32768,4651,- -32768,- -32768,- -32768,1140,- -32768,1159,- -32768,- -32768,- -32768,422,- -32768,821,- -32768,- -32768,1141,- -32768,- -32768,- -32768,- -32768,1276,1277,- -32768};
# 2029
static short Cyc_yypgoto[162U]={- -32768,119,- -32768,- -32768,- -32768,- -32768,- -32768,176,- -32768,- -32768,242,- -32768,- -32768,- 237,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 67,- 117,- 14,- -32768,- -32768,0,624,- -32768,235,- 182,1161,33,- -32768,- -32768,- 139,- -32768,30,1244,- 748,- -32768,- -32768,- -32768,1009,1007,659,355,- -32768,- -32768,614,- 211,- 231,- -32768,- -32768,92,- -32768,- -32768,121,- 154,1216,- 401,455,- -32768,1138,- -32768,- -32768,1241,- 452,- -32768,572,- 114,- 150,- 143,- 425,282,578,586,- 434,- 502,- 123,- 393,- 135,- -32768,- 270,- 168,- 576,- 247,- -32768,870,- 155,- 24,- 147,- 210,- 306,146,- -32768,- -32768,- 56,- 278,- -32768,- 423,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 23,1062,- -32768,675,860,- -32768,369,567,- -32768,- 170,- 298,- 165,- 363,- 361,- 353,930,- 351,- 345,- 279,- 321,- 317,- 197,695,- -32768,- -32768,- -32768,- -32768,367,- -32768,- 907,292,- -32768,507,954,216,- -32768,- 383,- -32768,- 12,529,- 35,- 61,- 72,- 54};
# 2053
static short Cyc_yytable[7950U]={64,369,267,104,374,375,150,83,493,147,348,87,327,351,105,591,352,153,336,593,329,64,279,338,410,64,528,529,594,531,652,614,596,67,1024,146,460,342,710,597,598,326,752,497,371,471,260,880,122,140,141,142,260,143,67,256,481,64,67,152,257,539,453,64,64,64,227,64,258,603,604,417,150,64,157,605,606,105,235,666,534,146,305,280,316,248,249,450,81,105,67,667,584,682,683,317,67,67,67,677,67,718,418,571,462,666,67,599,600,601,602,714,478,64,64,42,524,954,940,1095,19,20,540,717,525,152,231,64,64,64,623,64,64,64,64,306,762,152,228,64,472,941,701,378,463,707,67,67,978,64,41,64,108,647,667,464,678,229,157,44,67,67,67,- 558,67,67,67,67,848,123,111,560,67,462,647,541,130,285,287,411,412,413,67,139,67,548,1121,379,563,112,564,565,979,766,46,466,376,607,608,609,559,394,154,674,155,420,580,86,750,535,156,440,316,463,537,- 168,105,440,764,105,395,396,105,496,414,386,387,325,105,415,416,483,253,254,940,65,- 233,305,305,- 233,305,109,228,263,452,41,269,270,271,483,272,273,274,275,44,795,65,1022,560,672,65,229,47,388,389,152,48,110,536,445,453,453,453,318,51,52,994,454,42,64,376,392,113,87,558,393,470,986,987,748,306,306,65,306,334,995,996,64,65,65,65,729,65,305,64,64,802,64,65,555,114,41,67,625,541,858,257,829,117,862,44,427,428,660,258,988,989,739,412,413,67,560,48,712,873,557,115,67,67,612,67,421,720,615,41,861,660,118,65,65,865,464,43,44,45,622,306,422,533,871,119,376,65,65,65,500,65,65,65,65,64,571,414,41,65,384,385,740,416,47,794,43,44,45,65,450,65,124,745,120,921,51,52,923,477,541,41,127,318,41,458,928,228,67,267,44,624,43,44,45,84,952,239,704,630,- 255,47,105,- 255,240,477,229,958,636,816,105,458,530,51,52,46,46,125,105,929,257,1016,516,648,662,649,663,661,258,523,483,650,664,41,618,695,761,452,452,452,206,128,44,834,702,464,651,970,150,753,483,754,233,170,511,751,483,512,924,705,286,930,659,129,665,931,835,464,450,255,454,454,454,376,64,376,64,992,1026,377,879,993,571,1093,1018,673,768,941,152,825,780,770,1022,560,376,325,706,152,547,65,782,152,64,1108,780,784,376,67,170,67,376,64,438,785,79,64,159,65,- 15,1109,769,41,680,681,65,65,376,65,172,43,44,45,469,1045,783,67,85,856,250,1050,106,1131,107,251,67,931,541,47,67,818,933,457,840,372,176,841,230,458,826,51,52,1071,79,850,484,714,376,232,834,236,485,79,586,41,721,1085,79,633,234,927,376,43,44,45,145,698,148,144,376,79,65,835,444,670,759,671,457,237,175,252,1051,105,458,106,1053,105,131,132,560,661,948,1107,1013,1054,1055,106,999,1056,798,238,305,799,316,483,1057,1058,483,79,79,376,145,949,651,817,950,735,79,831,16,17,18,79,79,79,956,79,79,79,79,261,522,665,842,843,1063,1064,264,703,376,376,1065,1066,834,276,800,801,79,277,376,819,775,1127,262,306,806,152,812,376,976,376,133,134,851,864,1049,835,459,64,1132,64,175,319,1059,1060,1061,1062,998,372,332,859,372,376,915,1029,1030,376,480,1000,331,65,1034,65,577,869,333,376,397,376,339,975,67,1002,67,1006,506,1031,484,341,376,376,376,1035,1042,592,1075,1078,1080,65,376,721,560,376,106,968,1114,106,65,1119,106,918,65,345,376,340,106,398,399,400,401,402,403,404,405,406,407,907,1019,390,391,266,616,1015,1011,64,1038,376,105,1039,1101,353,482,1102,1067,1068,1069,1096,408,354,284,1098,355,79,639,640,641,550,551,356,482,266,508,357,1046,513,1047,358,67,682,683,366,160,161,162,163,164,359,1126,165,166,167,79,727,728,703,360,41,146,305,803,804,495,852,853,43,44,45,1106,946,947,361,982,983,47,380,1017,381,500,362,1086,541,501,984,985,175,51,52,363,344,373,347,349,349,382,64,687,688,990,991,691,1116,376,696,383,364,365,882,883,419,349,423,306,349,349,437,554,443,447,451,1041,456,467,468,875,475,64,305,67,459,476,486,507,349,495,488,490,572,573,574,575,576,498,509,579,510,146,582,518,480,519,1118,514,517,65,1125,65,587,284,67,590,526,520,884,527,278,532,537,553,911,464,46,506,562,544,106,378,549,41,545,306,610,578,106,583,617,43,44,45,46,88,106,585,611,64,47,619,620,587,500,621,627,637,41,628,631,629,51,52,347,632,43,44,45,965,635,638,188,653,654,47,146,655,482,500,656,482,67,657,658,668,675,51,52,692,79,676,79,689,773,65,690,646,693,697,699,482,708,677,726,831,715,482,349,711,719,723,724,1041,305,725,41,91,195,196,92,93,94,- 558,730,44,731,732,733,734,736,1001,742,743,1005,809,811,346,760,146,200,737,738,741,746,349,749,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,587,41,587,306,747,755,763,765,767,43,44,45,774,778,781,789,960,64,961,788,814,65,506,1043,266,793,796,807,813,822,815,349,325,820,744,824,506,838,41,506,832,854,844,855,266,41,495,44,846,46,67,266,65,857,44,47,1073,1074,135,48,1076,1077,47,1079,860,106,48,51,52,106,866,867,863,868,51,52,874,870,872,876,877,885,88,881,912,914,886,917,909,920,910,922,925,495,266,926,482,955,934,935,936,937,938,953,349,266,939,959,945,932,482,951,964,482,266,957,966,967,969,971,65,974,797,980,977,772,997,981,1012,1110,1014,777,1111,1020,1112,1113,1021,1032,89,787,887,888,1033,1036,1037,1044,1072,1048,1091,1083,424,940,1089,425,91,1097,88,92,93,94,1100,426,44,1103,587,1088,233,96,1128,1104,97,1105,1115,1117,98,1120,99,100,1122,427,428,1130,1129,1133,1135,1136,1123,101,349,1099,779,116,1004,265,492,1008,1010,499,792,158,126,828,827,495,1084,821,836,837,626,839,446,757,89,634,88,337,266,1027,908,495,595,0,849,1094,581,79,451,226,91,1124,266,92,93,94,0,95,44,0,0,0,106,96,0,0,97,0,0,0,889,372,99,100,65,0,0,0,1052,0,890,0,0,101,203,0,88,204,205,0,0,0,89,0,0,0,1070,0,0,0,0,0,0,349,756,0,0,425,91,0,1082,92,93,94,0,426,44,1087,0,0,1090,96,0,0,97,0,0,0,98,0,99,100,495,427,428,145,0,79,0,0,0,101,89,0,0,0,0,0,0,0,495,0,0,0,439,0,0,226,91,0,349,92,93,94,0,95,44,0,0,0,0,96,0,349,97,0,0,0,98,0,99,100,0,0,284,0,0,0,0,0,0,101,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,88,0,0,0,0,266,0,349,0,0,0,0,145,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,495,0,0,0,0,31,32,0,0,0,284,0,0,0,0,0,0,38,0,88,0,0,0,89,0,0,266,0,0,39,0,40,0,0,0,758,0,0,226,91,0,0,92,93,94,0,95,44,0,0,0,0,96,323,145,97,41,0,0,98,0,99,100,42,43,44,45,46,973,325,0,349,101,47,0,89,0,477,0,49,50,0,0,458,0,51,52,0,0,349,90,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,349,97,0,0,0,98,0,99,100,0,0,145,0,0,0,0,0,0,101,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,803,804,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,349,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,349,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,302,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,288,0,289,290,291,292,293,294,295,296,22,23,24,297,88,26,180,298,0,0,0,0,181,27,299,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,515,91,195,196,92,93,94,42,43,44,45,46,197,301,149,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,297,88,26,180,0,0,0,0,0,181,27,0,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,694,0,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,297,88,0,0,0,0,0,0,0,0,27,0,0,0,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,0,0,0,198,0,0,0,346,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,370,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,192,40,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,42,43,44,45,46,197,0,0,0,0,198,0,0,0,199,0,49,303,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,88,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,89,0,0,0,0,0,39,0,40,0,0,0,0,0,0,226,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,97,41,0,0,98,0,99,100,42,43,44,45,46,0,0,0,- 12,101,47,0,0,0,48,0,49,50,0,0,0,0,51,52,- 12,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,47,0,0,0,457,0,49,50,0,0,458,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,500,0,49,50,0,0,0,0,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,278,0,0,0,0,49,50,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,324,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,241,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,242,22,23,24,0,0,0,0,0,49,50,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,323,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,325,0,0,0,0,0,0,0,0,0,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,448,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,709,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,716,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,0,49,50,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,177,178,288,0,289,290,291,292,293,294,295,296,0,0,0,179,88,0,180,298,0,0,0,0,181,42,299,0,0,46,182,183,184,185,186,0,0,0,0,187,0,49,50,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,515,91,195,196,92,93,94,0,0,44,0,0,197,301,149,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,566,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,568,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,833,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,847,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,1028,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,567,91,195,196,92,93,94,0,258,44,0,0,197,0,350,0,0,198,0,0,0,199,0,0,200,0,427,428,177,178,0,201,0,0,202,203,0,0,204,205,0,0,0,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,330,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,350,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,521,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,613,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,645,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,700,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,771,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,786,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,808,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,810,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,913,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,916,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,919,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1003,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1007,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1009,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,1081,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,199,0,0,200,177,178,0,0,0,0,201,0,0,202,203,0,0,204,205,179,88,0,180,0,0,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,776,91,195,196,92,93,94,188,0,44,189,190,197,191,0,0,0,198,0,0,0,199,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,350,0,0,198,0,0,0,199,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,41,91,195,196,92,93,94,188,0,44,189,190,197,191,744,0,0,198,0,0,0,199,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,0,0,0,198,0,0,0,343,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,177,178,0,0,0,0,0,0,0,0,192,0,0,193,194,179,88,0,0,0,0,0,0,0,0,0,0,0,0,0,182,183,184,185,186,0,41,91,195,196,92,93,94,188,0,44,189,190,197,191,0,0,0,198,0,0,0,346,0,0,200,192,0,0,193,194,0,201,0,0,202,203,0,0,204,205,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,177,178,197,0,0,0,0,198,0,0,0,199,0,0,200,179,88,0,0,0,0,201,0,0,202,203,0,0,204,205,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,192,0,0,193,194,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,195,196,92,93,94,0,0,44,0,0,197,0,0,0,0,198,0,0,0,972,0,0,200,0,0,0,0,0,0,201,0,0,202,203,0,0,204,205};
# 2851
static short Cyc_yycheck[7950U]={0,198,125,26,201,202,73,21,278,70,180,25,155,181,26,378,181,73,168,380,155,21,139,170,221,25,304,305,381,307,464,414,383,0,941,70,246,176,540,384,385,155,618,280,199,17,118,795,48,63,64,65,124,67,21,116,266,57,25,73,112,65,244,63,64,65,89,67,120,390,391,110,139,73,74,392,393,89,101,480,317,116,149,139,151,109,110,242,114,101,57,484,370,5,6,151,63,64,65,80,67,553,141,350,92,506,73,386,387,388,389,545,266,113,114,119,125,865,114,1026,20,21,126,548,133,139,96,127,128,129,428,131,132,133,134,149,638,151,112,139,112,137,525,98,132,538,113,114,98,149,112,151,112,459,547,143,137,131,158,121,127,128,129,137,131,132,133,134,744,48,112,341,139,92,480,325,57,147,148,88,89,90,149,62,151,328,1093,142,343,131,345,346,142,645,123,249,128,394,395,396,339,129,131,501,133,228,361,133,140,128,139,234,279,132,112,127,228,240,643,231,148,149,234,279,133,93,94,125,240,138,139,266,113,114,114,0,134,304,305,137,307,131,112,122,244,112,127,128,129,284,131,132,133,134,121,680,21,137,428,496,25,131,129,131,132,279,133,131,322,239,452,453,454,152,141,142,129,244,119,279,128,143,125,297,338,147,256,93,94,137,304,305,57,307,135,148,149,297,63,64,65,571,67,370,304,305,689,307,73,333,125,112,279,130,464,767,112,113,54,771,121,138,139,477,120,131,132,88,89,90,297,501,133,544,786,335,133,304,305,411,307,119,134,415,112,770,500,111,113,114,775,143,120,121,122,427,370,135,125,784,133,128,127,128,129,133,131,132,133,134,370,618,133,112,139,95,96,138,139,129,678,120,121,122,149,540,151,111,585,126,813,141,142,818,133,545,112,125,277,112,139,826,112,370,527,121,429,120,121,122,126,863,126,530,437,125,129,429,128,133,133,131,874,447,702,437,139,306,141,142,123,123,133,445,827,112,113,291,131,131,133,133,477,120,298,480,139,139,112,419,517,134,452,453,454,86,125,121,728,526,143,461,885,530,619,500,621,98,131,133,617,506,136,134,530,138,134,477,126,479,138,728,143,638,115,452,453,454,128,489,128,491,143,128,134,793,147,744,128,137,500,648,137,517,715,659,649,137,678,128,125,535,526,128,279,662,530,517,137,673,663,128,489,131,491,128,526,134,663,0,530,76,297,126,137,649,112,507,508,304,305,128,307,114,120,121,122,134,971,663,517,22,763,125,977,26,134,28,130,526,138,711,129,530,707,835,133,735,199,126,735,131,139,716,141,142,999,48,748,128,1014,128,112,853,125,134,57,134,112,556,1015,62,125,133,825,128,120,121,122,70,125,72,126,128,75,370,853,238,489,632,491,133,128,84,119,978,628,139,89,980,632,125,126,793,659,856,1049,925,981,982,101,909,983,125,130,702,128,704,673,984,985,676,113,114,128,116,857,647,704,860,134,122,724,17,18,19,127,128,129,870,131,132,133,134,133,296,666,739,740,990,991,134,526,128,128,992,993,947,125,134,134,152,128,128,708,655,1109,120,702,134,704,125,128,890,128,125,126,751,134,977,947,246,702,1126,704,176,125,986,987,988,989,908,343,133,768,346,128,125,949,950,128,266,134,156,489,956,491,358,782,133,128,60,128,128,889,702,134,704,134,284,951,128,130,128,128,128,957,134,379,134,134,134,517,128,721,925,128,228,881,134,231,526,134,234,125,530,133,128,127,240,99,100,101,102,103,104,105,106,107,108,803,936,91,92,125,416,929,125,788,125,128,803,128,125,133,266,128,994,995,996,1031,130,133,144,1035,133,277,452,453,454,83,84,133,284,155,286,133,972,289,974,133,788,5,6,18,75,76,77,78,79,133,1109,82,83,84,306,127,128,692,133,112,876,909,22,23,278,127,128,120,121,122,1048,127,128,133,145,146,129,97,931,144,133,133,1018,1014,137,95,96,339,141,142,133,177,133,179,180,181,145,878,510,511,91,92,514,127,128,517,146,193,194,798,799,126,198,119,909,201,202,112,332,112,133,243,964,245,112,132,788,127,909,977,878,457,134,127,126,221,350,127,127,353,354,355,356,357,127,126,360,133,964,363,112,477,125,1089,133,133,702,1108,704,373,282,909,376,137,125,800,133,130,127,112,140,806,143,123,500,127,134,429,98,134,112,137,977,397,112,437,133,131,120,121,122,123,41,445,134,409,977,129,132,132,414,133,132,127,127,112,128,134,137,141,142,297,128,120,121,122,878,134,128,68,133,112,129,1039,126,477,133,133,480,977,134,128,47,125,141,142,137,489,128,491,133,653,788,126,458,30,125,45,500,133,80,112,1105,134,506,341,143,134,134,128,1102,1109,134,112,113,114,115,116,117,118,137,134,121,128,128,128,128,134,913,114,127,916,694,695,133,127,1102,136,134,134,134,134,378,134,380,381,382,383,384,385,386,387,388,389,390,391,392,393,394,395,396,525,112,527,1109,128,140,134,134,140,120,121,122,125,134,134,46,127,1109,129,126,125,878,659,969,457,133,127,133,126,81,134,428,125,127,126,132,673,25,112,676,131,112,137,112,477,112,571,121,134,123,1109,484,909,134,121,129,1002,1003,125,133,1006,1007,129,1009,134,628,133,141,142,632,128,134,140,134,141,142,140,134,134,126,48,126,41,133,807,808,134,810,137,812,127,125,133,618,527,119,659,113,134,134,133,128,134,125,501,538,134,127,134,832,673,134,126,676,547,134,134,127,28,126,977,133,685,97,137,650,97,144,127,1075,128,656,1078,25,1080,1081,133,127,97,664,25,26,128,127,127,127,125,134,114,134,109,114,133,112,113,113,41,116,117,118,127,120,121,127,689,134,889,126,1114,134,129,128,127,134,133,128,135,136,134,138,139,114,134,134,0,0,1102,146,585,1039,658,39,915,124,277,918,919,282,676,75,51,721,716,728,1014,711,731,732,430,734,240,628,97,445,41,169,649,942,803,744,382,- 1,747,1023,362,788,659,112,113,1105,663,116,117,118,- 1,120,121,- 1,- 1,- 1,803,126,- 1,- 1,129,- 1,- 1,- 1,133,972,135,136,1109,- 1,- 1,- 1,979,- 1,143,- 1,- 1,146,147,- 1,41,150,151,- 1,- 1,- 1,97,- 1,- 1,- 1,997,- 1,- 1,- 1,- 1,- 1,- 1,678,109,- 1,- 1,112,113,- 1,1011,116,117,118,- 1,120,121,1018,- 1,- 1,1021,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,835,138,139,876,- 1,878,- 1,- 1,- 1,146,97,- 1,- 1,- 1,- 1,- 1,- 1,- 1,853,- 1,- 1,- 1,109,- 1,- 1,112,113,- 1,735,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,748,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,795,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,- 1,- 1,- 1,- 1,827,- 1,793,- 1,- 1,- 1,- 1,964,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,947,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,865,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,41,- 1,- 1,- 1,97,- 1,- 1,881,- 1,- 1,85,- 1,87,- 1,- 1,- 1,109,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,109,1039,129,112,- 1,- 1,133,- 1,135,136,119,120,121,122,123,887,125,- 1,890,146,129,- 1,97,- 1,133,- 1,135,136,- 1,- 1,139,- 1,141,142,- 1,- 1,908,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,925,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,1102,- 1,- 1,- 1,- 1,- 1,- 1,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,978,- 1,980,981,982,983,984,985,986,987,988,989,990,991,992,993,994,995,996,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,1048,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1108,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,41,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,97,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,112,113,- 1,- 1,116,117,118,- 1,120,121,- 1,- 1,- 1,- 1,126,- 1,- 1,129,112,- 1,- 1,133,- 1,135,136,119,120,121,122,123,- 1,- 1,- 1,127,146,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,127,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,139,- 1,141,142,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,135,136,- 1,- 1,- 1,- 1,141,142,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,- 1,- 1,- 1,130,- 1,- 1,- 1,- 1,135,136,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,37,38,39,40,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,- 1,126,37,38,39,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,- 1,125,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,109,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,120,- 1,122,123,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,136,- 1,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,119,51,- 1,- 1,123,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,135,136,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,32,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,127,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,120,121,- 1,- 1,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,138,139,25,26,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,140,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,125,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,134,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,25,26,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,126,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,112,113,114,115,116,117,118,68,- 1,121,71,72,124,74,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,86,- 1,- 1,89,90,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,25,26,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,40,41,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,89,90,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,113,114,115,116,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,- 1,- 1,133,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,146,147,- 1,- 1,150,151};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int state,int token);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple32{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple33{struct _fat_ptr f1;void*f2;};static char _tmp426[8U]="stdcall";static char _tmp427[6U]="cdecl";static char _tmp428[9U]="fastcall";static char _tmp429[9U]="noreturn";static char _tmp42A[6U]="const";static char _tmp42B[8U]="aligned";static char _tmp42C[7U]="packed";static char _tmp42D[7U]="shared";static char _tmp42E[7U]="unused";static char _tmp42F[5U]="weak";static char _tmp430[10U]="dllimport";static char _tmp431[10U]="dllexport";static char _tmp432[23U]="no_instrument_function";static char _tmp433[12U]="constructor";static char _tmp434[11U]="destructor";static char _tmp435[22U]="no_check_memory_usage";static char _tmp436[5U]="pure";static char _tmp437[14U]="always_inline";struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp396=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp396;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp767;(_tmp767.YYINITIALSVAL).tag=72U,(_tmp767.YYINITIALSVAL).val=0;_tmp767;});
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
_tag_fat(({unsigned _tmp6AF=200U;struct Cyc_Yystacktype*_tmp6AE=({struct _RegionHandle*_tmp81B=yyregion;_region_malloc(_tmp81B,_check_times(_tmp6AF,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp766=200U;unsigned i;for(i=0;i < _tmp766;++ i){(_tmp6AE[i]).v=yylval,({struct Cyc_Yyltype _tmp81C=Cyc_yynewloc();(_tmp6AE[i]).l=_tmp81C;});}}0;});_tmp6AE;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp81E=({const char*_tmp397="parser stack overflow";_tag_fat(_tmp397,sizeof(char),22U);});int _tmp81D=yystate;Cyc_yyerror(_tmp81E,_tmp81D,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp39B=(unsigned)yystacksize;short*_tmp39A=({struct _RegionHandle*_tmp81F=yyregion;_region_malloc(_tmp81F,_check_times(_tmp39B,sizeof(short)));});({{unsigned _tmp724=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp724;++ i){
i <= (unsigned)yyssp_offset?_tmp39A[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp39A[i]=0);}}0;});_tag_fat(_tmp39A,sizeof(short),_tmp39B);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp399=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp398=({struct _RegionHandle*_tmp820=yyregion;_region_malloc(_tmp820,_check_times(_tmp399,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp723=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp723;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp398[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp398[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp398,sizeof(struct Cyc_Yystacktype),_tmp399);});
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
int _tmp39C=0;_npop_handler(0U);return _tmp39C;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp821=({struct Cyc_Yystacktype _tmp725;_tmp725.v=yylval,_tmp725.l=yylloc;_tmp725;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp821;});
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
{int _tmp39D=yyn;switch(_tmp39D){case 1U: _LL1: _LL2:
# 1195 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1198
goto _LL0;case 2U: _LL3: _LL4:
# 1201 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp822=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp822,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3U: _LL5: _LL6:
# 1205 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct Cyc_Absyn_Decl*_tmp826=({struct Cyc_Absyn_Decl*_tmp39F=_cycalloc(sizeof(*_tmp39F));({void*_tmp825=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->tag=10U,({struct _tuple0*_tmp824=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp39E->f1=_tmp824;}),({struct Cyc_List_List*_tmp823=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39E->f2=_tmp823;});_tmp39E;});_tmp39F->r=_tmp825;}),_tmp39F->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39F;});_tmp3A0->hd=_tmp826;}),_tmp3A0->tl=0;_tmp3A0;}));
Cyc_Lex_leave_using();
# 1208
goto _LL0;case 4U: _LL7: _LL8:
# 1209 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_Absyn_Decl*_tmp82B=({struct Cyc_Absyn_Decl*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({void*_tmp82A=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->tag=10U,({struct _tuple0*_tmp829=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3A1->f1=_tmp829;}),({struct Cyc_List_List*_tmp828=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A1->f2=_tmp828;});_tmp3A1;});_tmp3A2->r=_tmp82A;}),_tmp3A2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A2;});_tmp3A3->hd=_tmp82B;}),({struct Cyc_List_List*_tmp827=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A3->tl=_tmp827;});_tmp3A3;}));
goto _LL0;case 5U: _LL9: _LLA:
# 1212
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct Cyc_Absyn_Decl*_tmp830=({struct Cyc_Absyn_Decl*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({void*_tmp82F=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=9U,({struct _fat_ptr*_tmp82E=({struct _fat_ptr*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct _fat_ptr _tmp82D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A4=_tmp82D;});_tmp3A4;});_tmp3A5->f1=_tmp82E;}),({struct Cyc_List_List*_tmp82C=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A5->f2=_tmp82C;});_tmp3A5;});_tmp3A6->r=_tmp82F;}),_tmp3A6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A6;});_tmp3A7->hd=_tmp830;}),_tmp3A7->tl=0;_tmp3A7;}));
Cyc_Lex_leave_namespace();
# 1215
goto _LL0;case 6U: _LLB: _LLC:
# 1216 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));({struct Cyc_Absyn_Decl*_tmp836=({struct Cyc_Absyn_Decl*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({void*_tmp835=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->tag=9U,({struct _fat_ptr*_tmp834=({struct _fat_ptr*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({struct _fat_ptr _tmp833=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp3A8=_tmp833;});_tmp3A8;});_tmp3A9->f1=_tmp834;}),({struct Cyc_List_List*_tmp832=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A9->f2=_tmp832;});_tmp3A9;});_tmp3AA->r=_tmp835;}),_tmp3AA->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AA;});_tmp3AB->hd=_tmp836;}),({struct Cyc_List_List*_tmp831=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3AB->tl=_tmp831;});_tmp3AB;}));
goto _LL0;case 7U: _LLD: _LLE: {
# 1218 "parse.y"
int _tmp3AC=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_c_include=_tmp3AC;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple28*_tmp3AD=Cyc_yyget_YY53(&(yyyvsp[5]).v);struct _tuple28*_stmttmp18=_tmp3AD;struct _tuple28*_tmp3AE=_stmttmp18;unsigned _tmp3B0;struct Cyc_List_List*_tmp3AF;_LL462: _tmp3AF=_tmp3AE->f1;_tmp3B0=_tmp3AE->f2;_LL463: {struct Cyc_List_List*exs=_tmp3AF;unsigned wc=_tmp3B0;
struct Cyc_List_List*_tmp3B1=Cyc_yyget_YY54(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3B1;
if(exs != 0 && hides != 0)
({void*_tmp3B2=0U;({unsigned _tmp838=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp837=({const char*_tmp3B3="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3B3,sizeof(char),65U);});Cyc_Warn_err(_tmp838,_tmp837,_tag_fat(_tmp3B2,sizeof(void*),0U));});});
# 1225
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
# 1228
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3B4=0U;({unsigned _tmp83A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp839=({const char*_tmp3B5="expecting \"C include\"";_tag_fat(_tmp3B5,sizeof(char),22U);});Cyc_Warn_err(_tmp83A,_tmp839,_tag_fat(_tmp3B4,sizeof(void*),0U));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct Cyc_Absyn_Decl*_tmp83F=({struct Cyc_Absyn_Decl*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({void*_tmp83E=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->tag=12U,({struct Cyc_List_List*_tmp83D=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B7->f1=_tmp83D;}),_tmp3B7->f2=cycdecls,_tmp3B7->f3=exs,({struct _tuple10*_tmp83C=({struct _tuple10*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->f1=wc,_tmp3B6->f2=hides;_tmp3B6;});_tmp3B7->f4=_tmp83C;});_tmp3B7;});_tmp3B8->r=_tmp83E;}),_tmp3B8->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B8;});_tmp3B9->hd=_tmp83F;}),({struct Cyc_List_List*_tmp83B=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B9->tl=_tmp83B;});_tmp3B9;}));}else{
# 1234
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct Cyc_Absyn_Decl*_tmp843=({struct Cyc_Absyn_Decl*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({void*_tmp842=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->tag=11U,({struct Cyc_List_List*_tmp841=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BA->f1=_tmp841;});_tmp3BA;});_tmp3BB->r=_tmp842;}),_tmp3BB->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BB;});_tmp3BC->hd=_tmp843;}),({struct Cyc_List_List*_tmp840=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3BC->tl=_tmp840;});_tmp3BC;}));}}else{
# 1238
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct Cyc_Absyn_Decl*_tmp848=({struct Cyc_Absyn_Decl*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({void*_tmp847=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->tag=12U,({struct Cyc_List_List*_tmp846=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BE->f1=_tmp846;}),_tmp3BE->f2=cycdecls,_tmp3BE->f3=exs,({struct _tuple10*_tmp845=({struct _tuple10*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->f1=wc,_tmp3BD->f2=hides;_tmp3BD;});_tmp3BE->f4=_tmp845;});_tmp3BE;});_tmp3BF->r=_tmp847;}),_tmp3BF->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BF;});_tmp3C0->hd=_tmp848;}),({struct Cyc_List_List*_tmp844=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3C0->tl=_tmp844;});_tmp3C0;}));}
# 1241
goto _LL0;}}case 8U: _LLF: _LL10:
# 1242 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Decl*_tmp84A=({struct Cyc_Absyn_Decl*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C1;});_tmp3C2->hd=_tmp84A;}),({struct Cyc_List_List*_tmp849=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C2->tl=_tmp849;});_tmp3C2;}));
goto _LL0;case 9U: _LL11: _LL12:
# 1244 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct Cyc_Absyn_Decl*_tmp84C=({struct Cyc_Absyn_Decl*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3C3->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C3;});_tmp3C4->hd=_tmp84C;}),({struct Cyc_List_List*_tmp84B=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C4->tl=_tmp84B;});_tmp3C4;}));
goto _LL0;case 10U: _LL13: _LL14:
# 1246 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct Cyc_Absyn_Decl*_tmp84E=({struct Cyc_Absyn_Decl*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3C5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C5;});_tmp3C6->hd=_tmp84E;}),({struct Cyc_List_List*_tmp84D=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C6->tl=_tmp84D;});_tmp3C6;}));
goto _LL0;case 11U: _LL15: _LL16:
# 1248 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));({struct Cyc_Absyn_Decl*_tmp850=({struct Cyc_Absyn_Decl*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3C7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3C7;});_tmp3C8->hd=_tmp850;}),({struct Cyc_List_List*_tmp84F=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3C8->tl=_tmp84F;});_tmp3C8;}));
goto _LL0;case 12U: _LL17: _LL18:
# 1249 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13U: _LL19: _LL1A:
# 1254 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14U: _LL1B: _LL1C:
# 1259 "parse.y"
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15U: _LL1D: _LL1E: {
# 1264 "parse.y"
struct _fat_ptr _tmp3C9=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3C9;
Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp851=(struct _fat_ptr)two;Cyc_strcmp(_tmp851,({const char*_tmp3CA="C";_tag_fat(_tmp3CA,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _fat_ptr _tmp852=(struct _fat_ptr)two;Cyc_strcmp(_tmp852,({const char*_tmp3CB="C include";_tag_fat(_tmp3CB,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1271
({void*_tmp3CC=0U;({unsigned _tmp854=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp853=({const char*_tmp3CD="expecting \"C\" or \"C include\"";_tag_fat(_tmp3CD,sizeof(char),29U);});Cyc_Warn_err(_tmp854,_tmp853,_tag_fat(_tmp3CC,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1275
goto _LL0;}case 16U: _LL1F: _LL20:
# 1278 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17U: _LL21: _LL22:
# 1282 "parse.y"
 yyval=Cyc_YY54(0);
goto _LL0;case 18U: _LL23: _LL24:
# 1283 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19U: _LL25: _LL26:
# 1287 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct _tuple0*_tmp855=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CE->hd=_tmp855;}),_tmp3CE->tl=0;_tmp3CE;}));
goto _LL0;case 20U: _LL27: _LL28:
# 1288 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));({struct _tuple0*_tmp856=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CF->hd=_tmp856;}),_tmp3CF->tl=0;_tmp3CF;}));
goto _LL0;case 21U: _LL29: _LL2A:
# 1290 "parse.y"
 yyval=Cyc_YY54(({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct _tuple0*_tmp858=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D0->hd=_tmp858;}),({struct Cyc_List_List*_tmp857=Cyc_yyget_YY54(&(yyyvsp[2]).v);_tmp3D0->tl=_tmp857;});_tmp3D0;}));
goto _LL0;case 22U: _LL2B: _LL2C:
# 1294 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=0,_tmp3D1->f2=0U;_tmp3D1;}));
goto _LL0;case 23U: _LL2D: _LL2E:
# 1295 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24U: _LL2F: _LL30:
# 1299 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct Cyc_List_List*_tmp859=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3D2->f1=_tmp859;}),_tmp3D2->f2=0U;_tmp3D2;}));
goto _LL0;case 25U: _LL31: _LL32:
# 1300 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=0,_tmp3D3->f2=0U;_tmp3D3;}));
goto _LL0;case 26U: _LL33: _LL34:
# 1301 "parse.y"
 yyval=Cyc_YY53(({struct _tuple28*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->f1=0,_tmp3D4->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3D4;}));
goto _LL0;case 27U: _LL35: _LL36:
# 1305 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _tuple32*_tmp85B=({struct _tuple32*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D5->f2=_tmp85A;}),_tmp3D5->f3=0;_tmp3D5;});_tmp3D6->hd=_tmp85B;}),_tmp3D6->tl=0;_tmp3D6;}));
goto _LL0;case 28U: _LL37: _LL38:
# 1306 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));({struct _tuple32*_tmp85D=({struct _tuple32*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85C=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D7->f2=_tmp85C;}),_tmp3D7->f3=0;_tmp3D7;});_tmp3D8->hd=_tmp85D;}),_tmp3D8->tl=0;_tmp3D8;}));
goto _LL0;case 29U: _LL39: _LL3A:
# 1308 "parse.y"
 yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({struct _tuple32*_tmp860=({struct _tuple32*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp85F=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3D9->f2=_tmp85F;}),_tmp3D9->f3=0;_tmp3D9;});_tmp3DA->hd=_tmp860;}),({struct Cyc_List_List*_tmp85E=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp3DA->tl=_tmp85E;});_tmp3DA;}));
goto _LL0;case 30U: _LL3B: _LL3C:
# 1312 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 31U: _LL3D: _LL3E:
# 1313 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 32U: _LL3F: _LL40:
# 1317 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));({struct Cyc_Absyn_Decl*_tmp863=({void*_tmp862=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp861=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3DB->f1=_tmp861;});_tmp3DB;});Cyc_Absyn_new_decl(_tmp862,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3DC->hd=_tmp863;}),_tmp3DC->tl=0;_tmp3DC;}));
goto _LL0;case 33U: _LL41: _LL42:
# 1318 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 34U: _LL43: _LL44:
# 1319 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 37U: _LL45: _LL46:
# 1328 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp866=yyr;struct Cyc_Parse_Declarator _tmp865=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp864=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp866,0,_tmp865,0,_tmp864,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 38U: _LL47: _LL48: {
# 1330 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DD=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DD;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp869=yyr;struct Cyc_Parse_Declarator _tmp868=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp867=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp869,& d,_tmp868,0,_tmp867,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 39U: _LL49: _LL4A:
# 1343 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp86D=yyr;struct Cyc_Parse_Declarator _tmp86C=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp86B=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp86A=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp86D,0,_tmp86C,_tmp86B,_tmp86A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 40U: _LL4B: _LL4C: {
# 1345 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DE=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DE;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp871=yyr;struct Cyc_Parse_Declarator _tmp870=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp86F=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp86E=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp871,& d,_tmp870,_tmp86F,_tmp86E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1353 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DF=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3DF;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp874=yyr;struct Cyc_Parse_Declarator _tmp873=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp872=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp874,& d,_tmp873,0,_tmp872,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1356 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3E0=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3E0;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp878=yyr;struct Cyc_Parse_Declarator _tmp877=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp876=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp875=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp878,& d,_tmp877,_tmp876,_tmp875,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 43U: _LL51: _LL52:
# 1361 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 44U: _LL53: _LL54:
# 1364
 Cyc_Lex_leave_using();
goto _LL0;case 45U: _LL55: _LL56:
# 1367
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));({struct _fat_ptr _tmp879=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3E1=_tmp879;});_tmp3E1;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 46U: _LL57: _LL58:
# 1370
 Cyc_Lex_leave_namespace();
goto _LL0;case 47U: _LL59: _LL5A: {
# 1376 "parse.y"
int _tmp3E2=((yyyvsp[0]).l).first_line;int location=_tmp3E2;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87B=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp87A=(unsigned)location;Cyc_Parse_make_declarations(_tmp87B,0,_tmp87A,(unsigned)location);}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1379 "parse.y"
int _tmp3E3=((yyyvsp[0]).l).first_line;int location=_tmp3E3;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87E=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple13*_tmp87D=Cyc_yyget_YY19(&(yyyvsp[1]).v);unsigned _tmp87C=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp87E,_tmp87D,_tmp87C,(unsigned)location);}));
goto _LL0;}case 49U: _LL5D: _LL5E:
# 1383
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));({struct Cyc_Absyn_Decl*_tmp881=({struct Cyc_Absyn_Pat*_tmp880=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp87F=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp880,_tmp87F,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E4->hd=_tmp881;}),_tmp3E4->tl=0;_tmp3E4;}));
goto _LL0;case 50U: _LL5F: _LL60: {
# 1385 "parse.y"
struct Cyc_List_List*_tmp3E5=0;struct Cyc_List_List*vds=_tmp3E5;
{struct Cyc_List_List*_tmp3E6=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3E6;for(0;ids != 0;ids=ids->tl){
struct _fat_ptr*_tmp3E7=(struct _fat_ptr*)ids->hd;struct _fat_ptr*id=_tmp3E7;
struct _tuple0*qv=({struct _tuple0*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({union Cyc_Absyn_Nmspace _tmp882=Cyc_Absyn_Rel_n(0);_tmp3EA->f1=_tmp882;}),_tmp3EA->f2=id;_tmp3EA;});
struct Cyc_Absyn_Vardecl*_tmp3E8=({struct _tuple0*_tmp883=qv;Cyc_Absyn_new_vardecl(0U,_tmp883,Cyc_Absyn_wildtyp(0),0);});struct Cyc_Absyn_Vardecl*vd=_tmp3E8;
vds=({struct Cyc_List_List*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->hd=vd,_tmp3E9->tl=vds;_tmp3E9;});}}
# 1392
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({struct Cyc_Absyn_Decl*_tmp884=Cyc_Absyn_letv_decl(vds,(unsigned)((yyyvsp[0]).l).first_line);_tmp3EB->hd=_tmp884;}),_tmp3EB->tl=0;_tmp3EB;}));
# 1395
goto _LL0;}case 51U: _LL61: _LL62: {
# 1398 "parse.y"
struct _fat_ptr _tmp3EC=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr three=_tmp3EC;
# 1400
if(({struct _fat_ptr _tmp885=(struct _fat_ptr)three;Cyc_zstrcmp(_tmp885,({const char*_tmp3ED="`H";_tag_fat(_tmp3ED,sizeof(char),3U);}));})== 0)
({void*_tmp3EE=0U;({unsigned _tmp887=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp886=({const char*_tmp3EF="bad occurrence of heap region";_tag_fat(_tmp3EF,sizeof(char),30U);});Cyc_Warn_err(_tmp887,_tmp886,_tag_fat(_tmp3EE,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp888=(struct _fat_ptr)three;Cyc_zstrcmp(_tmp888,({const char*_tmp3F0="`U";_tag_fat(_tmp3F0,sizeof(char),3U);}));})== 0)
({void*_tmp3F1=0U;({unsigned _tmp88A=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp889=({const char*_tmp3F2="bad occurrence of unique region";_tag_fat(_tmp3F2,sizeof(char),32U);});Cyc_Warn_err(_tmp88A,_tmp889,_tag_fat(_tmp3F1,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));({struct _fat_ptr*_tmp88C=({struct _fat_ptr*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));*_tmp3F6=three;_tmp3F6;});_tmp3F7->name=_tmp88C;}),_tmp3F7->identity=- 1,({void*_tmp88B=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3F7->kind=_tmp88B;});_tmp3F7;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp890=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp88F=({struct _tuple0*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp88E=({struct _fat_ptr*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));({struct _fat_ptr _tmp88D=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3F4=_tmp88D;});_tmp3F4;});_tmp3F5->f2=_tmp88E;});_tmp3F5;});Cyc_Absyn_new_vardecl(_tmp890,_tmp88F,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));({struct Cyc_Absyn_Decl*_tmp891=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3F3->hd=_tmp891;}),_tmp3F3->tl=0;_tmp3F3;}));
# 1409
goto _LL0;}}case 52U: _LL63: _LL64: {
# 1411
struct _fat_ptr _tmp3F8=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp3F8;
if(({struct _fat_ptr _tmp892=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp892,({const char*_tmp3F9="H";_tag_fat(_tmp3F9,sizeof(char),2U);}));})== 0)
({void*_tmp3FA=0U;({unsigned _tmp894=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp893=({const char*_tmp3FB="bad occurrence of heap region `H";_tag_fat(_tmp3FB,sizeof(char),33U);});Cyc_Warn_err(_tmp894,_tmp893,_tag_fat(_tmp3FA,sizeof(void*),0U));});});
if(({struct _fat_ptr _tmp895=(struct _fat_ptr)two;Cyc_zstrcmp(_tmp895,({const char*_tmp3FC="U";_tag_fat(_tmp3FC,sizeof(char),2U);}));})== 0)
({void*_tmp3FD=0U;({unsigned _tmp897=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp896=({const char*_tmp3FE="bad occurrence of unique region `U";_tag_fat(_tmp3FE,sizeof(char),35U);});Cyc_Warn_err(_tmp897,_tmp896,_tag_fat(_tmp3FD,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp406=_cycalloc(sizeof(*_tmp406));({struct _fat_ptr*_tmp89B=({struct _fat_ptr*_tmp405=_cycalloc(sizeof(*_tmp405));({struct _fat_ptr _tmp89A=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp404=({struct Cyc_String_pa_PrintArg_struct _tmp726;_tmp726.tag=0U,_tmp726.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp726;});void*_tmp402[1U];_tmp402[0]=& _tmp404;({struct _fat_ptr _tmp899=({const char*_tmp403="`%s";_tag_fat(_tmp403,sizeof(char),4U);});Cyc_aprintf(_tmp899,_tag_fat(_tmp402,sizeof(void*),1U));});});*_tmp405=_tmp89A;});_tmp405;});_tmp406->name=_tmp89B;}),_tmp406->identity=- 1,({
void*_tmp898=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp406->kind=_tmp898;});_tmp406;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp89E=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp89D=({struct _tuple0*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp89C=({struct _fat_ptr*_tmp400=_cycalloc(sizeof(*_tmp400));*_tmp400=two;_tmp400;});_tmp401->f2=_tmp89C;});_tmp401;});Cyc_Absyn_new_vardecl(_tmp89E,_tmp89D,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));({struct Cyc_Absyn_Decl*_tmp89F=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3FF->hd=_tmp89F;}),_tmp3FF->tl=0;_tmp3FF;}));
# 1422
goto _LL0;}}case 53U: _LL65: _LL66: {
# 1424
struct _fat_ptr _tmp407=Cyc_yyget_String_tok(&(yyyvsp[1]).v);struct _fat_ptr two=_tmp407;
struct _fat_ptr _tmp408=Cyc_yyget_String_tok(&(yyyvsp[3]).v);struct _fat_ptr four=_tmp408;
struct Cyc_Absyn_Exp*_tmp409=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*six=_tmp409;
if(({struct _fat_ptr _tmp8A0=(struct _fat_ptr)four;Cyc_strcmp(_tmp8A0,({const char*_tmp40A="open";_tag_fat(_tmp40A,sizeof(char),5U);}));})!= 0)({void*_tmp40B=0U;({unsigned _tmp8A2=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp8A1=({const char*_tmp40C="expecting `open'";_tag_fat(_tmp40C,sizeof(char),17U);});Cyc_Warn_err(_tmp8A2,_tmp8A1,_tag_fat(_tmp40B,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp414=_cycalloc(sizeof(*_tmp414));({struct _fat_ptr*_tmp8A6=({struct _fat_ptr*_tmp413=_cycalloc(sizeof(*_tmp413));({struct _fat_ptr _tmp8A5=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp412=({struct Cyc_String_pa_PrintArg_struct _tmp727;_tmp727.tag=0U,_tmp727.f1=(struct _fat_ptr)((struct _fat_ptr)two);_tmp727;});void*_tmp410[1U];_tmp410[0]=& _tmp412;({struct _fat_ptr _tmp8A4=({const char*_tmp411="`%s";_tag_fat(_tmp411,sizeof(char),4U);});Cyc_aprintf(_tmp8A4,_tag_fat(_tmp410,sizeof(void*),1U));});});*_tmp413=_tmp8A5;});_tmp413;});_tmp414->name=_tmp8A6;}),_tmp414->identity=- 1,({
void*_tmp8A3=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp414->kind=_tmp8A3;});_tmp414;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp8A9=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp8A8=({struct _tuple0*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp8A7=({struct _fat_ptr*_tmp40E=_cycalloc(sizeof(*_tmp40E));*_tmp40E=two;_tmp40E;});_tmp40F->f2=_tmp8A7;});_tmp40F;});Cyc_Absyn_new_vardecl(_tmp8A9,_tmp8A8,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp40D=_cycalloc(sizeof(*_tmp40D));({struct Cyc_Absyn_Decl*_tmp8AA=Cyc_Absyn_region_decl(tv,vd,six,(unsigned)((yyyvsp[0]).l).first_line);_tmp40D->hd=_tmp8AA;}),_tmp40D->tl=0;_tmp40D;}));
# 1434
goto _LL0;}}case 54U: _LL67: _LL68:
# 1438 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55U: _LL69: _LL6A:
# 1440 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp8AB=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8AB,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56U: _LL6B: _LL6C:
# 1446 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp728;({enum Cyc_Parse_Storage_class*_tmp8AE=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp728.sc=_tmp8AE;}),({struct Cyc_Absyn_Tqual _tmp8AD=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp728.tq=_tmp8AD;}),({
struct Cyc_Parse_Type_specifier _tmp8AC=Cyc_Parse_empty_spec(0U);_tmp728.type_specs=_tmp8AC;}),_tmp728.is_inline=0,_tmp728.attributes=0;_tmp728;}));
goto _LL0;case 57U: _LL6D: _LL6E: {
# 1449 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp415=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp415;
if(two.sc != 0)
({void*_tmp416=0U;({unsigned _tmp8B0=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8AF=({const char*_tmp417="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp417,sizeof(char),73U);});Cyc_Warn_warn(_tmp8B0,_tmp8AF,_tag_fat(_tmp416,sizeof(void*),0U));});});
# 1453
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp729;({enum Cyc_Parse_Storage_class*_tmp8B1=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp729.sc=_tmp8B1;}),_tmp729.tq=two.tq,_tmp729.type_specs=two.type_specs,_tmp729.is_inline=two.is_inline,_tmp729.attributes=two.attributes;_tmp729;}));
# 1457
goto _LL0;}case 58U: _LL6F: _LL70:
# 1458 "parse.y"
({void*_tmp418=0U;({unsigned _tmp8B3=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8B2=({const char*_tmp419="__extension__ keyword ignored in declaration";_tag_fat(_tmp419,sizeof(char),45U);});Cyc_Warn_warn(_tmp8B3,_tmp8B2,_tag_fat(_tmp418,sizeof(void*),0U));});});
yyval=(yyyvsp[1]).v;
# 1461
goto _LL0;case 59U: _LL71: _LL72:
# 1462 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72A;_tmp72A.sc=0,({struct Cyc_Absyn_Tqual _tmp8B5=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72A.tq=_tmp8B5;}),({
struct Cyc_Parse_Type_specifier _tmp8B4=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp72A.type_specs=_tmp8B4;}),_tmp72A.is_inline=0,_tmp72A.attributes=0;_tmp72A;}));
goto _LL0;case 60U: _LL73: _LL74: {
# 1465 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41A=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41A;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72B;_tmp72B.sc=two.sc,_tmp72B.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp8B8=({unsigned _tmp8B7=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp8B6=two.type_specs;Cyc_Parse_combine_specifiers(_tmp8B7,_tmp8B6,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp72B.type_specs=_tmp8B8;}),_tmp72B.is_inline=two.is_inline,_tmp72B.attributes=two.attributes;_tmp72B;}));
# 1471
goto _LL0;}case 61U: _LL75: _LL76:
# 1472 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72C;_tmp72C.sc=0,({struct Cyc_Absyn_Tqual _tmp8BA=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp72C.tq=_tmp8BA;}),({struct Cyc_Parse_Type_specifier _tmp8B9=Cyc_Parse_empty_spec(0U);_tmp72C.type_specs=_tmp8B9;}),_tmp72C.is_inline=0,_tmp72C.attributes=0;_tmp72C;}));
goto _LL0;case 62U: _LL77: _LL78: {
# 1474 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41B=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41B;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72D;_tmp72D.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp8BC=({struct Cyc_Absyn_Tqual _tmp8BB=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp8BB,two.tq);});_tmp72D.tq=_tmp8BC;}),_tmp72D.type_specs=two.type_specs,_tmp72D.is_inline=two.is_inline,_tmp72D.attributes=two.attributes;_tmp72D;}));
# 1479
goto _LL0;}case 63U: _LL79: _LL7A:
# 1480 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72E;_tmp72E.sc=0,({struct Cyc_Absyn_Tqual _tmp8BE=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp72E.tq=_tmp8BE;}),({
struct Cyc_Parse_Type_specifier _tmp8BD=Cyc_Parse_empty_spec(0U);_tmp72E.type_specs=_tmp8BD;}),_tmp72E.is_inline=1,_tmp72E.attributes=0;_tmp72E;}));
goto _LL0;case 64U: _LL7B: _LL7C: {
# 1483 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41C=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41C;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72F;_tmp72F.sc=two.sc,_tmp72F.tq=two.tq,_tmp72F.type_specs=two.type_specs,_tmp72F.is_inline=1,_tmp72F.attributes=two.attributes;_tmp72F;}));
# 1487
goto _LL0;}case 65U: _LL7D: _LL7E:
# 1488 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp730;_tmp730.sc=0,({struct Cyc_Absyn_Tqual _tmp8C1=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp730.tq=_tmp8C1;}),({
struct Cyc_Parse_Type_specifier _tmp8C0=Cyc_Parse_empty_spec(0U);_tmp730.type_specs=_tmp8C0;}),_tmp730.is_inline=0,({struct Cyc_List_List*_tmp8BF=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp730.attributes=_tmp8BF;});_tmp730;}));
goto _LL0;case 66U: _LL7F: _LL80: {
# 1491 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41D=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp41D;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp731;_tmp731.sc=two.sc,_tmp731.tq=two.tq,_tmp731.type_specs=two.type_specs,_tmp731.is_inline=two.is_inline,({
# 1494
struct Cyc_List_List*_tmp8C3=({struct Cyc_List_List*_tmp8C2=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8C2,two.attributes);});_tmp731.attributes=_tmp8C3;});_tmp731;}));
goto _LL0;}case 67U: _LL81: _LL82: {
# 1498 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL0;}case 68U: _LL83: _LL84: {
# 1500 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL0;}case 69U: _LL85: _LL86: {
# 1502 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL0;}case 70U: _LL87: _LL88: {
# 1504 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL0;}case 71U: _LL89: _LL8A: {
# 1507 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _fat_ptr _tmp8C4=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp8C4,({const char*_tmp41E="C";_tag_fat(_tmp41E,sizeof(char),2U);}));})!= 0)
({void*_tmp41F=0U;({unsigned _tmp8C6=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8C5=({const char*_tmp420="only extern or extern \"C\" is allowed";_tag_fat(_tmp420,sizeof(char),37U);});Cyc_Warn_err(_tmp8C6,_tmp8C5,_tag_fat(_tmp41F,sizeof(void*),0U));});});
yyval=Cyc_YY20(& externC_sc);
# 1512
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1512 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1515 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL0;}case 74U: _LL8F: _LL90:
# 1521 "parse.y"
 yyval=Cyc_YY45(0);
goto _LL0;case 75U: _LL91: _LL92:
# 1522 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76U: _LL93: _LL94:
# 1527 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77U: _LL95: _LL96:
# 1531 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));({void*_tmp8C7=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp421->hd=_tmp8C7;}),_tmp421->tl=0;_tmp421;}));
goto _LL0;case 78U: _LL97: _LL98:
# 1532 "parse.y"
 yyval=Cyc_YY45(({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));({void*_tmp8C9=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp422->hd=_tmp8C9;}),({struct Cyc_List_List*_tmp8C8=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp422->tl=_tmp8C8;});_tmp422;}));
goto _LL0;case 79U: _LL99: _LL9A: {
# 1537 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple33 att_map[18U]={{{_tmp426,_tmp426,_tmp426 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp427,_tmp427,_tmp427 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp428,_tmp428,_tmp428 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp429,_tmp429,_tmp429 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp42A,_tmp42A,_tmp42A + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp42B,_tmp42B,_tmp42B + 8U},(void*)& att_aligned},{{_tmp42C,_tmp42C,_tmp42C + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp42D,_tmp42D,_tmp42D + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp42E,_tmp42E,_tmp42E + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp42F,_tmp42F,_tmp42F + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp430,_tmp430,_tmp430 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp431,_tmp431,_tmp431 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp432,_tmp432,_tmp432 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp433,_tmp433,_tmp433 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp434,_tmp434,_tmp434 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp435,_tmp435,_tmp435 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp436,_tmp436,_tmp436 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp437,_tmp437,_tmp437 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1558
struct _fat_ptr _tmp423=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp423;
# 1560
if((((_get_fat_size(s,sizeof(char))> (unsigned)4 &&(int)((const char*)s.curr)[0]== (int)'_')&&(int)((const char*)s.curr)[1]== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)2)))== (int)'_')&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),(int)(_get_fat_size(s,sizeof(char))- (unsigned)3)))== (int)'_')
# 1562
s=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,2,_get_fat_size(s,sizeof(char))- (unsigned)5);{
int i=0;
for(0;(unsigned)i < 18U;++ i){
if(Cyc_strcmp((struct _fat_ptr)s,(struct _fat_ptr)(*((struct _tuple33*)_check_known_subscript_notnull(att_map,18U,sizeof(struct _tuple33),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1569
if((unsigned)i == 18U){
({void*_tmp424=0U;({unsigned _tmp8CB=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8CA=({const char*_tmp425="unrecognized attribute";_tag_fat(_tmp425,sizeof(char),23U);});Cyc_Warn_err(_tmp8CB,_tmp8CA,_tag_fat(_tmp424,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1574
goto _LL0;}}case 80U: _LL9B: _LL9C:
# 1574 "parse.y"
 yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;case 81U: _LL9D: _LL9E: {
# 1576 "parse.y"
struct _fat_ptr _tmp438=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp438;
struct Cyc_Absyn_Exp*_tmp439=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*e=_tmp439;
void*a;
if(({struct _fat_ptr _tmp8CD=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CD,({const char*_tmp43A="aligned";_tag_fat(_tmp43A,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8CC=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CC,({const char*_tmp43B="__aligned__";_tag_fat(_tmp43B,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=6U,_tmp43C->f1=e;_tmp43C;});else{
if(({struct _fat_ptr _tmp8CF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CF,({const char*_tmp43D="section";_tag_fat(_tmp43D,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8CE=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8CE,({const char*_tmp43E="__section__";_tag_fat(_tmp43E,sizeof(char),12U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=8U,_tmp43F->f1=str;_tmp43F;});}else{
if(({struct _fat_ptr _tmp8D0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D0,({const char*_tmp440="__mode__";_tag_fat(_tmp440,sizeof(char),9U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->tag=24U,_tmp441->f1=str;_tmp441;});}else{
if(({struct _fat_ptr _tmp8D1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D1,({const char*_tmp442="alias";_tag_fat(_tmp442,sizeof(char),6U);}));})== 0){
struct _fat_ptr str=Cyc_Parse_exp2string((unsigned)((yyyvsp[2]).l).first_line,e);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->tag=25U,_tmp443->f1=str;_tmp443;});}else{
# 1592
int n=Cyc_Parse_exp2int((unsigned)((yyyvsp[2]).l).first_line,e);
if(({struct _fat_ptr _tmp8D3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D3,({const char*_tmp444="regparm";_tag_fat(_tmp444,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8D2=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D2,({const char*_tmp445="__regparm__";_tag_fat(_tmp445,sizeof(char),12U);}));})== 0){
if(n < 0 || n > 3)
({void*_tmp446=0U;({unsigned _tmp8D5=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8D4=({const char*_tmp447="regparm requires value between 0 and 3";_tag_fat(_tmp447,sizeof(char),39U);});Cyc_Warn_err(_tmp8D5,_tmp8D4,_tag_fat(_tmp446,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->tag=0U,_tmp448->f1=n;_tmp448;});}else{
if(({struct _fat_ptr _tmp8D7=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D7,({const char*_tmp449="initializes";_tag_fat(_tmp449,sizeof(char),12U);}));})== 0 ||({struct _fat_ptr _tmp8D6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D6,({const char*_tmp44A="__initializes__";_tag_fat(_tmp44A,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=20U,_tmp44B->f1=n;_tmp44B;});else{
if(({struct _fat_ptr _tmp8D9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D9,({const char*_tmp44C="noliveunique";_tag_fat(_tmp44C,sizeof(char),13U);}));})== 0 ||({struct _fat_ptr _tmp8D8=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8D8,({const char*_tmp44D="__noliveunique__";_tag_fat(_tmp44D,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->tag=21U,_tmp44E->f1=n;_tmp44E;});else{
if(({struct _fat_ptr _tmp8DB=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DB,({const char*_tmp44F="consume";_tag_fat(_tmp44F,sizeof(char),8U);}));})== 0 ||({struct _fat_ptr _tmp8DA=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8DA,({const char*_tmp450="__consume__";_tag_fat(_tmp450,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=22U,_tmp451->f1=n;_tmp451;});else{
# 1604
({void*_tmp452=0U;({unsigned _tmp8DD=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8DC=({const char*_tmp453="unrecognized attribute";_tag_fat(_tmp453,sizeof(char),23U);});Cyc_Warn_err(_tmp8DD,_tmp8DC,_tag_fat(_tmp452,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1608
yyval=Cyc_YY46(a);
# 1610
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1611 "parse.y"
struct _fat_ptr _tmp454=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr s=_tmp454;
struct _fat_ptr _tmp455=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr t=_tmp455;
unsigned _tmp456=({unsigned _tmp8DE=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DE,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});unsigned n=_tmp456;
unsigned _tmp457=({unsigned _tmp8DF=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp8DF,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));});unsigned m=_tmp457;
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _fat_ptr _tmp8E1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E1,({const char*_tmp458="format";_tag_fat(_tmp458,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8E0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp8E0,({const char*_tmp459="__format__";_tag_fat(_tmp459,sizeof(char),11U);}));})== 0){
if(({struct _fat_ptr _tmp8E3=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E3,({const char*_tmp45A="printf";_tag_fat(_tmp45A,sizeof(char),7U);}));})== 0 ||({struct _fat_ptr _tmp8E2=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E2,({const char*_tmp45B="__printf__";_tag_fat(_tmp45B,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=19U,_tmp45C->f1=Cyc_Absyn_Printf_ft,_tmp45C->f2=(int)n,_tmp45C->f3=(int)m;_tmp45C;});else{
if(({struct _fat_ptr _tmp8E5=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E5,({const char*_tmp45D="scanf";_tag_fat(_tmp45D,sizeof(char),6U);}));})== 0 ||({struct _fat_ptr _tmp8E4=(struct _fat_ptr)t;Cyc_zstrcmp(_tmp8E4,({const char*_tmp45E="__scanf__";_tag_fat(_tmp45E,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->tag=19U,_tmp45F->f1=Cyc_Absyn_Scanf_ft,_tmp45F->f2=(int)n,_tmp45F->f3=(int)m;_tmp45F;});else{
# 1622
({void*_tmp460=0U;({unsigned _tmp8E7=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp8E6=({const char*_tmp461="unrecognized format type";_tag_fat(_tmp461,sizeof(char),25U);});Cyc_Warn_err(_tmp8E7,_tmp8E6,_tag_fat(_tmp460,sizeof(void*),0U));});});}}}else{
# 1624
({void*_tmp462=0U;({unsigned _tmp8E9=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp8E8=({const char*_tmp463="unrecognized attribute";_tag_fat(_tmp463,sizeof(char),23U);});Cyc_Warn_err(_tmp8E9,_tmp8E8,_tag_fat(_tmp462,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1627
goto _LL0;}case 83U: _LLA1: _LLA2:
# 1636 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84U: _LLA3: _LLA4:
# 1638 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EB=({struct _tuple0*_tmp8EA=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp8EA,Cyc_yyget_YY40(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp8EB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85U: _LLA5: _LLA6:
# 1642 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86U: _LLA7: _LLA8:
# 1643 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87U: _LLA9: _LLAA:
# 1644 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88U: _LLAB: _LLAC:
# 1645 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89U: _LLAD: _LLAE:
# 1646 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90U: _LLAF: _LLB0:
# 1647 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91U: _LLB1: _LLB2:
# 1648 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92U: _LLB3: _LLB4:
# 1649 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93U: _LLB5: _LLB6:
# 1650 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94U: _LLB7: _LLB8:
# 1651 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95U: _LLB9: _LLBA:
# 1652 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96U: _LLBB: _LLBC:
# 1655
 yyval=Cyc_YY21(({void*_tmp8EC=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8EC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97U: _LLBD: _LLBE:
# 1657 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8ED=Cyc_Absyn_builtin_type(({const char*_tmp464="__builtin_va_list";_tag_fat(_tmp464,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp8ED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98U: _LLBF: _LLC0:
# 1659 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99U: _LLC1: _LLC2:
# 1661 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EE=Cyc_yyget_YY44(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp8EE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100U: _LLC3: _LLC4:
# 1663 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8EF=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8EF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101U: _LLC5: _LLC6:
# 1665 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F0=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp8F0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102U: _LLC7: _LLC8:
# 1667 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F3=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->tag=6U,({struct Cyc_List_List*_tmp8F2=({unsigned _tmp8F1=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8F1,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[2]).v)));});
# 1667
_tmp465->f1=_tmp8F2;});_tmp465;});Cyc_Parse_type_spec(_tmp8F3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1670
goto _LL0;case 103U: _LLC9: _LLCA:
# 1671 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F4=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104U: _LLCB: _LLCC:
# 1673 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F5=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp8F5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1675
goto _LL0;case 105U: _LLCD: _LLCE:
# 1676 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F6=Cyc_Absyn_tag_type(Cyc_yyget_YY44(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106U: _LLCF: _LLD0:
# 1678 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F7=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp8F7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107U: _LLD1: _LLD2:
# 1680 "parse.y"
 yyval=Cyc_YY21(({void*_tmp8F8=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp8F8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108U: _LLD3: _LLD4:
# 1686 "parse.y"
 yyval=Cyc_YY43(({struct _fat_ptr _tmp8F9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id_to_kind(_tmp8F9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109U: _LLD5: _LLD6: {
# 1690 "parse.y"
unsigned loc=(unsigned)(Cyc_Absyn_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp732;_tmp732.print_const=1,_tmp732.q_volatile=0,_tmp732.q_restrict=0,_tmp732.real_const=1,_tmp732.loc=loc;_tmp732;}));
goto _LL0;}case 110U: _LLD7: _LLD8:
# 1692 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp733;_tmp733.print_const=0,_tmp733.q_volatile=1,_tmp733.q_restrict=0,_tmp733.real_const=0,_tmp733.loc=0U;_tmp733;}));
goto _LL0;case 111U: _LLD9: _LLDA:
# 1693 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp734;_tmp734.print_const=0,_tmp734.q_volatile=0,_tmp734.q_restrict=1,_tmp734.real_const=0,_tmp734.loc=0U;_tmp734;}));
goto _LL0;case 112U: _LLDB: _LLDC: {
# 1699 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp466=({struct Cyc_Absyn_TypeDecl*_tmp46B=_cycalloc(sizeof(*_tmp46B));({void*_tmp8FE=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8FD=({struct Cyc_Absyn_Enumdecl*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8FC=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp469->name=_tmp8FC;}),({struct Cyc_Core_Opt*_tmp8FB=({struct Cyc_Core_Opt*_tmp468=_cycalloc(sizeof(*_tmp468));({struct Cyc_List_List*_tmp8FA=Cyc_yyget_YY48(&(yyyvsp[3]).v);_tmp468->v=_tmp8FA;});_tmp468;});_tmp469->fields=_tmp8FB;});_tmp469;});_tmp46A->f1=_tmp8FD;});_tmp46A;});_tmp46B->r=_tmp8FE;}),_tmp46B->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46B;});struct Cyc_Absyn_TypeDecl*ed=_tmp466;
# 1701
yyval=Cyc_YY21(({void*_tmp8FF=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=10U,_tmp467->f1=ed,_tmp467->f2=0;_tmp467;});Cyc_Parse_type_spec(_tmp8FF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1703
goto _LL0;}case 113U: _LLDD: _LLDE:
# 1704 "parse.y"
 yyval=Cyc_YY21(({void*_tmp900=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp900,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114U: _LLDF: _LLE0:
# 1706 "parse.y"
 yyval=Cyc_YY21(({void*_tmp901=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp901,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115U: _LLE1: _LLE2:
# 1712 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46C=_cycalloc(sizeof(*_tmp46C));({struct _tuple0*_tmp902=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46C->name=_tmp902;}),_tmp46C->tag=0,_tmp46C->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46C;}));
goto _LL0;case 116U: _LLE3: _LLE4:
# 1714 "parse.y"
 yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp46D=_cycalloc(sizeof(*_tmp46D));({struct _tuple0*_tmp904=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp46D->name=_tmp904;}),({struct Cyc_Absyn_Exp*_tmp903=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp46D->tag=_tmp903;}),_tmp46D->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp46D;}));
goto _LL0;case 117U: _LLE5: _LLE6:
# 1718 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46E=_cycalloc(sizeof(*_tmp46E));({struct Cyc_Absyn_Enumfield*_tmp905=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46E->hd=_tmp905;}),_tmp46E->tl=0;_tmp46E;}));
goto _LL0;case 118U: _LLE7: _LLE8:
# 1719 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46F=_cycalloc(sizeof(*_tmp46F));({struct Cyc_Absyn_Enumfield*_tmp906=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp46F->hd=_tmp906;}),_tmp46F->tl=0;_tmp46F;}));
goto _LL0;case 119U: _LLE9: _LLEA:
# 1720 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_List_List*_tmp470=_cycalloc(sizeof(*_tmp470));({struct Cyc_Absyn_Enumfield*_tmp908=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp470->hd=_tmp908;}),({struct Cyc_List_List*_tmp907=Cyc_yyget_YY48(&(yyyvsp[2]).v);_tmp470->tl=_tmp907;});_tmp470;}));
goto _LL0;case 120U: _LLEB: _LLEC:
# 1726 "parse.y"
 yyval=Cyc_YY21(({void*_tmp90B=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=7U,({enum Cyc_Absyn_AggrKind _tmp90A=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp471->f1=_tmp90A;}),({struct Cyc_List_List*_tmp909=Cyc_yyget_YY24(&(yyyvsp[2]).v);_tmp471->f2=_tmp909;});_tmp471;});Cyc_Parse_type_spec(_tmp90B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121U: _LLED: _LLEE: {
# 1732 "parse.y"
struct Cyc_List_List*_tmp472=({unsigned _tmp90C=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp90C,Cyc_yyget_YY40(&(yyyvsp[3]).v));});struct Cyc_List_List*ts=_tmp472;
struct Cyc_List_List*_tmp473=({unsigned _tmp90D=(unsigned)((yyyvsp[5]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp90D,Cyc_yyget_YY40(&(yyyvsp[5]).v));});struct Cyc_List_List*exist_ts=_tmp473;
struct Cyc_Absyn_TypeDecl*_tmp474=({enum Cyc_Absyn_AggrKind _tmp913=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp912=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp911=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp910=({
struct Cyc_List_List*_tmp90F=exist_ts;struct Cyc_List_List*_tmp90E=Cyc_yyget_YY50(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp90F,_tmp90E,Cyc_yyget_YY24(&(yyyvsp[7]).v),1);});
# 1734
Cyc_Absyn_aggr_tdecl(_tmp913,Cyc_Absyn_Public,_tmp912,_tmp911,_tmp910,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp474;
# 1737
yyval=Cyc_YY21(({void*_tmp914=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->tag=10U,_tmp475->f1=td,_tmp475->f2=0;_tmp475;});Cyc_Parse_type_spec(_tmp914,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1739
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1743 "parse.y"
struct Cyc_List_List*_tmp476=({unsigned _tmp915=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp915,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp476;
struct Cyc_List_List*_tmp477=({unsigned _tmp916=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp916,Cyc_yyget_YY40(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp477;
struct Cyc_Absyn_TypeDecl*_tmp478=({enum Cyc_Absyn_AggrKind _tmp91C=Cyc_yyget_YY22(&(yyyvsp[0]).v);struct _tuple0*_tmp91B=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp91A=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp919=({
struct Cyc_List_List*_tmp918=exist_ts;struct Cyc_List_List*_tmp917=Cyc_yyget_YY50(&(yyyvsp[5]).v);Cyc_Absyn_aggrdecl_impl(_tmp918,_tmp917,Cyc_yyget_YY24(&(yyyvsp[6]).v),0);});
# 1745
Cyc_Absyn_aggr_tdecl(_tmp91C,Cyc_Absyn_Public,_tmp91B,_tmp91A,_tmp919,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp478;
# 1748
yyval=Cyc_YY21(({void*_tmp91D=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp479=_cycalloc(sizeof(*_tmp479));_tmp479->tag=10U,_tmp479->f1=td,_tmp479->f2=0;_tmp479;});Cyc_Parse_type_spec(_tmp91D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1750
goto _LL0;}case 123U: _LLF1: _LLF2:
# 1751 "parse.y"
 yyval=Cyc_YY21(({void*_tmp921=({union Cyc_Absyn_AggrInfo _tmp920=({enum Cyc_Absyn_AggrKind _tmp91F=Cyc_yyget_YY22(&(yyyvsp[1]).v);struct _tuple0*_tmp91E=Cyc_yyget_QualId_tok(&(yyyvsp[2]).v);Cyc_Absyn_UnknownAggr(_tmp91F,_tmp91E,({struct Cyc_Core_Opt*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->v=(void*)1;_tmp47A;}));});Cyc_Absyn_aggr_type(_tmp920,Cyc_yyget_YY40(&(yyyvsp[3]).v));});Cyc_Parse_type_spec(_tmp921,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1754
goto _LL0;case 124U: _LLF3: _LLF4:
# 1755 "parse.y"
 yyval=Cyc_YY21(({void*_tmp924=({union Cyc_Absyn_AggrInfo _tmp923=({enum Cyc_Absyn_AggrKind _tmp922=Cyc_yyget_YY22(&(yyyvsp[0]).v);Cyc_Absyn_UnknownAggr(_tmp922,Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);});Cyc_Absyn_aggr_type(_tmp923,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp924,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 125U: _LLF5: _LLF6:
# 1760 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 126U: _LLF7: _LLF8:
# 1762 "parse.y"
 yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 127U: _LLF9: _LLFA:
# 1766 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 128U: _LLFB: _LLFC:
# 1767 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 129U: _LLFD: _LLFE:
# 1772 "parse.y"
 yyval=Cyc_YY24(0);
goto _LL0;case 130U: _LLFF: _LL100: {
# 1774 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp47B=Cyc_yyget_YY25(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp47B;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1778
struct Cyc_List_List*_tmp47C=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp47C;
if(tags != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY24(decls);
# 1783
goto _LL0;}}case 131U: _LL101: _LL102:
# 1788 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct Cyc_List_List*_tmp925=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp47D->hd=_tmp925;}),_tmp47D->tl=0;_tmp47D;}));
goto _LL0;case 132U: _LL103: _LL104:
# 1790 "parse.y"
 yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47E=_cycalloc(sizeof(*_tmp47E));({struct Cyc_List_List*_tmp927=Cyc_yyget_YY24(&(yyyvsp[1]).v);_tmp47E->hd=_tmp927;}),({struct Cyc_List_List*_tmp926=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp47E->tl=_tmp926;});_tmp47E;}));
goto _LL0;case 133U: _LL105: _LL106:
# 1794 "parse.y"
 yyval=Cyc_YY19(((struct _tuple13*(*)(struct _tuple13*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[0]).v)));
goto _LL0;case 134U: _LL107: _LL108:
# 1800 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp47F=_region_malloc(yyr,sizeof(*_tmp47F));_tmp47F->tl=0,({struct _tuple12 _tmp928=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp47F->hd=_tmp928;});_tmp47F;}));
goto _LL0;case 135U: _LL109: _LL10A:
# 1802 "parse.y"
 yyval=Cyc_YY19(({struct _tuple13*_tmp480=_region_malloc(yyr,sizeof(*_tmp480));({struct _tuple13*_tmp92A=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp480->tl=_tmp92A;}),({struct _tuple12 _tmp929=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp480->hd=_tmp929;});_tmp480;}));
goto _LL0;case 136U: _LL10B: _LL10C:
# 1807 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp735;({struct Cyc_Parse_Declarator _tmp92B=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp735.f1=_tmp92B;}),_tmp735.f2=0;_tmp735;}));
goto _LL0;case 137U: _LL10D: _LL10E:
# 1809 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp736;({struct Cyc_Parse_Declarator _tmp92D=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp736.f1=_tmp92D;}),({struct Cyc_Absyn_Exp*_tmp92C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp736.f2=_tmp92C;});_tmp736;}));
goto _LL0;case 138U: _LL10F: _LL110: {
# 1815 "parse.y"
struct _RegionHandle _tmp481=_new_region("temp");struct _RegionHandle*temp=& _tmp481;_push_region(temp);
{struct _tuple26 _tmp482=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp19=_tmp482;struct _tuple26 _tmp483=_stmttmp19;struct Cyc_List_List*_tmp486;struct Cyc_Parse_Type_specifier _tmp485;struct Cyc_Absyn_Tqual _tmp484;_LL465: _tmp484=_tmp483.f1;_tmp485=_tmp483.f2;_tmp486=_tmp483.f3;_LL466: {struct Cyc_Absyn_Tqual tq=_tmp484;struct Cyc_Parse_Type_specifier tspecs=_tmp485;struct Cyc_List_List*atts=_tmp486;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple11*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp487=Cyc_yyget_YY29(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp487;for(0;x != 0;x=x->tl){
struct _tuple25*_tmp488=(struct _tuple25*)x->hd;struct _tuple25*_stmttmp1A=_tmp488;struct _tuple25*_tmp489=_stmttmp1A;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_Absyn_Exp*_tmp48B;struct Cyc_Parse_Declarator _tmp48A;_LL468: _tmp48A=_tmp489->f1;_tmp48B=_tmp489->f2;_tmp48C=_tmp489->f3;_LL469: {struct Cyc_Parse_Declarator d=_tmp48A;struct Cyc_Absyn_Exp*wd=_tmp48B;struct Cyc_Absyn_Exp*wh=_tmp48C;
decls=({struct _tuple11*_tmp48D=_region_malloc(temp,sizeof(*_tmp48D));_tmp48D->tl=decls,_tmp48D->hd=d;_tmp48D;});
widths_and_reqs=({struct Cyc_List_List*_tmp48F=_region_malloc(temp,sizeof(*_tmp48F));
({struct _tuple17*_tmp92E=({struct _tuple17*_tmp48E=_region_malloc(temp,sizeof(*_tmp48E));_tmp48E->f1=wd,_tmp48E->f2=wh;_tmp48E;});_tmp48F->hd=_tmp92E;}),_tmp48F->tl=widths_and_reqs;_tmp48F;});}}}
# 1826
decls=((struct _tuple11*(*)(struct _tuple11*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp490=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp490;
struct Cyc_List_List*_tmp491=({struct _RegionHandle*_tmp931=temp;struct _RegionHandle*_tmp930=temp;struct Cyc_List_List*_tmp92F=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1829
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp931,_tmp930,_tmp92F,widths_and_reqs);});struct Cyc_List_List*info=_tmp491;
# 1832
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned,struct _tuple18*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1834
_npop_handler(0U);goto _LL0;}}}}
# 1816
;_pop_region();}case 139U: _LL111: _LL112:
# 1842 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp737;({struct Cyc_Absyn_Tqual _tmp933=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp737.f1=_tmp933;}),({struct Cyc_Parse_Type_specifier _tmp932=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp737.f2=_tmp932;}),_tmp737.f3=0;_tmp737;}));
goto _LL0;case 140U: _LL113: _LL114: {
# 1844 "parse.y"
struct _tuple26 _tmp492=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp492;yyval=Cyc_YY35(({struct _tuple26 _tmp738;_tmp738.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp936=({unsigned _tmp935=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp934=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp935,_tmp934,two.f2);});_tmp738.f2=_tmp936;}),_tmp738.f3=two.f3;_tmp738;}));
goto _LL0;}case 141U: _LL115: _LL116:
# 1846 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp739;({struct Cyc_Absyn_Tqual _tmp938=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp739.f1=_tmp938;}),({struct Cyc_Parse_Type_specifier _tmp937=Cyc_Parse_empty_spec(0U);_tmp739.f2=_tmp937;}),_tmp739.f3=0;_tmp739;}));
goto _LL0;case 142U: _LL117: _LL118: {
# 1848 "parse.y"
struct _tuple26 _tmp493=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp493;
yyval=Cyc_YY35(({struct _tuple26 _tmp73A;({struct Cyc_Absyn_Tqual _tmp93A=({struct Cyc_Absyn_Tqual _tmp939=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp939,two.f1);});_tmp73A.f1=_tmp93A;}),_tmp73A.f2=two.f2,_tmp73A.f3=two.f3;_tmp73A;}));
goto _LL0;}case 143U: _LL119: _LL11A:
# 1851 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73B;({struct Cyc_Absyn_Tqual _tmp93D=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73B.f1=_tmp93D;}),({struct Cyc_Parse_Type_specifier _tmp93C=Cyc_Parse_empty_spec(0U);_tmp73B.f2=_tmp93C;}),({struct Cyc_List_List*_tmp93B=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp73B.f3=_tmp93B;});_tmp73B;}));
goto _LL0;case 144U: _LL11B: _LL11C: {
# 1853 "parse.y"
struct _tuple26 _tmp494=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp494;yyval=Cyc_YY35(({struct _tuple26 _tmp73C;_tmp73C.f1=two.f1,_tmp73C.f2=two.f2,({struct Cyc_List_List*_tmp93F=({struct Cyc_List_List*_tmp93E=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp93E,two.f3);});_tmp73C.f3=_tmp93F;});_tmp73C;}));
goto _LL0;}case 145U: _LL11D: _LL11E:
# 1859 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73D;({struct Cyc_Absyn_Tqual _tmp941=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp73D.f1=_tmp941;}),({struct Cyc_Parse_Type_specifier _tmp940=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp73D.f2=_tmp940;}),_tmp73D.f3=0;_tmp73D;}));
goto _LL0;case 146U: _LL11F: _LL120: {
# 1861 "parse.y"
struct _tuple26 _tmp495=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp495;yyval=Cyc_YY35(({struct _tuple26 _tmp73E;_tmp73E.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp944=({unsigned _tmp943=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp942=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp943,_tmp942,two.f2);});_tmp73E.f2=_tmp944;}),_tmp73E.f3=two.f3;_tmp73E;}));
goto _LL0;}case 147U: _LL121: _LL122:
# 1863 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp73F;({struct Cyc_Absyn_Tqual _tmp946=Cyc_yyget_YY23(&(yyyvsp[0]).v);_tmp73F.f1=_tmp946;}),({struct Cyc_Parse_Type_specifier _tmp945=Cyc_Parse_empty_spec(0U);_tmp73F.f2=_tmp945;}),_tmp73F.f3=0;_tmp73F;}));
goto _LL0;case 148U: _LL123: _LL124: {
# 1865 "parse.y"
struct _tuple26 _tmp496=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp496;
yyval=Cyc_YY35(({struct _tuple26 _tmp740;({struct Cyc_Absyn_Tqual _tmp948=({struct Cyc_Absyn_Tqual _tmp947=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp947,two.f1);});_tmp740.f1=_tmp948;}),_tmp740.f2=two.f2,_tmp740.f3=two.f3;_tmp740;}));
goto _LL0;}case 149U: _LL125: _LL126:
# 1868 "parse.y"
 yyval=Cyc_YY35(({struct _tuple26 _tmp741;({struct Cyc_Absyn_Tqual _tmp94B=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp741.f1=_tmp94B;}),({struct Cyc_Parse_Type_specifier _tmp94A=Cyc_Parse_empty_spec(0U);_tmp741.f2=_tmp94A;}),({struct Cyc_List_List*_tmp949=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp741.f3=_tmp949;});_tmp741;}));
goto _LL0;case 150U: _LL127: _LL128: {
# 1870 "parse.y"
struct _tuple26 _tmp497=Cyc_yyget_YY35(&(yyyvsp[1]).v);struct _tuple26 two=_tmp497;yyval=Cyc_YY35(({struct _tuple26 _tmp742;_tmp742.f1=two.f1,_tmp742.f2=two.f2,({struct Cyc_List_List*_tmp94D=({struct Cyc_List_List*_tmp94C=Cyc_yyget_YY45(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp94C,two.f3);});_tmp742.f3=_tmp94D;});_tmp742;}));
goto _LL0;}case 151U: _LL129: _LL12A:
# 1874 "parse.y"
 yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(&(yyyvsp[0]).v)));
goto _LL0;case 152U: _LL12B: _LL12C:
# 1880 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp498=_region_malloc(yyr,sizeof(*_tmp498));({struct _tuple25*_tmp94E=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp498->hd=_tmp94E;}),_tmp498->tl=0;_tmp498;}));
goto _LL0;case 153U: _LL12D: _LL12E:
# 1882 "parse.y"
 yyval=Cyc_YY29(({struct Cyc_List_List*_tmp499=_region_malloc(yyr,sizeof(*_tmp499));({struct _tuple25*_tmp950=Cyc_yyget_YY28(&(yyyvsp[2]).v);_tmp499->hd=_tmp950;}),({struct Cyc_List_List*_tmp94F=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp499->tl=_tmp94F;});_tmp499;}));
goto _LL0;case 154U: _LL12F: _LL130:
# 1887 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49A=_region_malloc(yyr,sizeof(*_tmp49A));({struct Cyc_Parse_Declarator _tmp952=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp49A->f1=_tmp952;}),_tmp49A->f2=0,({struct Cyc_Absyn_Exp*_tmp951=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp49A->f3=_tmp951;});_tmp49A;}));
goto _LL0;case 155U: _LL131: _LL132:
# 1891 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp49E=_region_malloc(yyr,sizeof(*_tmp49E));({struct _tuple0*_tmp957=({struct _tuple0*_tmp49D=_cycalloc(sizeof(*_tmp49D));({union Cyc_Absyn_Nmspace _tmp956=Cyc_Absyn_Rel_n(0);_tmp49D->f1=_tmp956;}),({struct _fat_ptr*_tmp955=({struct _fat_ptr*_tmp49C=_cycalloc(sizeof(*_tmp49C));({struct _fat_ptr _tmp954=({const char*_tmp49B="";_tag_fat(_tmp49B,sizeof(char),1U);});*_tmp49C=_tmp954;});_tmp49C;});_tmp49D->f2=_tmp955;});_tmp49D;});(_tmp49E->f1).id=_tmp957;}),(_tmp49E->f1).varloc=0U,(_tmp49E->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp953=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp49E->f2=_tmp953;}),_tmp49E->f3=0;_tmp49E;}));
# 1893
goto _LL0;case 156U: _LL133: _LL134:
# 1896 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A2=_region_malloc(yyr,sizeof(*_tmp4A2));({struct _tuple0*_tmp95B=({struct _tuple0*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));({union Cyc_Absyn_Nmspace _tmp95A=Cyc_Absyn_Rel_n(0);_tmp4A1->f1=_tmp95A;}),({struct _fat_ptr*_tmp959=({struct _fat_ptr*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));({struct _fat_ptr _tmp958=({const char*_tmp49F="";_tag_fat(_tmp49F,sizeof(char),1U);});*_tmp4A0=_tmp958;});_tmp4A0;});_tmp4A1->f2=_tmp959;});_tmp4A1;});(_tmp4A2->f1).id=_tmp95B;}),(_tmp4A2->f1).varloc=0U,(_tmp4A2->f1).tms=0,_tmp4A2->f2=0,_tmp4A2->f3=0;_tmp4A2;}));
# 1898
goto _LL0;case 157U: _LL135: _LL136:
# 1899 "parse.y"
 yyval=Cyc_YY28(({struct _tuple25*_tmp4A3=_region_malloc(yyr,sizeof(*_tmp4A3));({struct Cyc_Parse_Declarator _tmp95D=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp4A3->f1=_tmp95D;}),({struct Cyc_Absyn_Exp*_tmp95C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4A3->f2=_tmp95C;}),_tmp4A3->f3=0;_tmp4A3;}));
goto _LL0;case 158U: _LL137: _LL138:
# 1903 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 159U: _LL139: _LL13A:
# 1904 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 160U: _LL13B: _LL13C:
# 1908 "parse.y"
 yyval=Cyc_YY57(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 161U: _LL13D: _LL13E:
# 1909 "parse.y"
 yyval=Cyc_YY57(0);
goto _LL0;case 162U: _LL13F: _LL140: {
# 1915 "parse.y"
int _tmp4A4=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A4;
struct Cyc_List_List*_tmp4A5=({unsigned _tmp95E=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp95E,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp4A5;
struct Cyc_Absyn_TypeDecl*_tmp4A6=({struct _tuple0*_tmp963=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp962=ts;struct Cyc_Core_Opt*_tmp961=({struct Cyc_Core_Opt*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));({struct Cyc_List_List*_tmp95F=Cyc_yyget_YY34(&(yyyvsp[4]).v);_tmp4A8->v=_tmp95F;});_tmp4A8;});int _tmp960=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp963,_tmp962,_tmp961,_tmp960,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp4A6;
# 1919
yyval=Cyc_YY21(({void*_tmp964=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->tag=10U,_tmp4A7->f1=dd,_tmp4A7->f2=0;_tmp4A7;});Cyc_Parse_type_spec(_tmp964,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1921
goto _LL0;}case 163U: _LL141: _LL142: {
# 1922 "parse.y"
int _tmp4A9=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4A9;
yyval=Cyc_YY21(({void*_tmp967=({union Cyc_Absyn_DatatypeInfo _tmp966=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp743;({struct _tuple0*_tmp965=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp743.name=_tmp965;}),_tmp743.is_extensible=is_extensible;_tmp743;}));Cyc_Absyn_datatype_type(_tmp966,Cyc_yyget_YY40(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp967,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1925
goto _LL0;}case 164U: _LL143: _LL144: {
# 1926 "parse.y"
int _tmp4AA=Cyc_yyget_YY31(&(yyyvsp[0]).v);int is_extensible=_tmp4AA;
yyval=Cyc_YY21(({void*_tmp96B=({union Cyc_Absyn_DatatypeFieldInfo _tmp96A=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp744;({struct _tuple0*_tmp969=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp744.datatype_name=_tmp969;}),({struct _tuple0*_tmp968=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp744.field_name=_tmp968;}),_tmp744.is_extensible=is_extensible;_tmp744;}));Cyc_Absyn_datatype_field_type(_tmp96A,Cyc_yyget_YY40(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp96B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1929
goto _LL0;}case 165U: _LL145: _LL146:
# 1932 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 166U: _LL147: _LL148:
# 1933 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 167U: _LL149: _LL14A:
# 1937 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct Cyc_Absyn_Datatypefield*_tmp96C=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AB->hd=_tmp96C;}),_tmp4AB->tl=0;_tmp4AB;}));
goto _LL0;case 168U: _LL14B: _LL14C:
# 1938 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));({struct Cyc_Absyn_Datatypefield*_tmp96D=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AC->hd=_tmp96D;}),_tmp4AC->tl=0;_tmp4AC;}));
goto _LL0;case 169U: _LL14D: _LL14E:
# 1939 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct Cyc_Absyn_Datatypefield*_tmp96F=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AD->hd=_tmp96F;}),({struct Cyc_List_List*_tmp96E=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AD->tl=_tmp96E;});_tmp4AD;}));
goto _LL0;case 170U: _LL14F: _LL150:
# 1940 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));({struct Cyc_Absyn_Datatypefield*_tmp971=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp4AE->hd=_tmp971;}),({struct Cyc_List_List*_tmp970=Cyc_yyget_YY34(&(yyyvsp[2]).v);_tmp4AE->tl=_tmp970;});_tmp4AE;}));
goto _LL0;case 171U: _LL151: _LL152:
# 1944 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 172U: _LL153: _LL154:
# 1945 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;case 173U: _LL155: _LL156:
# 1946 "parse.y"
 yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;case 174U: _LL157: _LL158:
# 1950 "parse.y"
 yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({struct _tuple0*_tmp973=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4AF->name=_tmp973;}),_tmp4AF->typs=0,_tmp4AF->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp972=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4AF->sc=_tmp972;});_tmp4AF;}));
goto _LL0;case 175U: _LL159: _LL15A: {
# 1952 "parse.y"
struct Cyc_List_List*_tmp4B0=({unsigned _tmp974=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*f)(unsigned,struct _tuple8*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp974,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp4B0;
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct _tuple0*_tmp976=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp4B1->name=_tmp976;}),_tmp4B1->typs=typs,_tmp4B1->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp975=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp4B1->sc=_tmp975;});_tmp4B1;}));
goto _LL0;}case 176U: _LL15B: _LL15C:
# 1958 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 177U: _LL15D: _LL15E: {
# 1960 "parse.y"
struct Cyc_Parse_Declarator _tmp4B2=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B2;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp745;_tmp745.id=two.id,_tmp745.varloc=two.varloc,({struct Cyc_List_List*_tmp978=({struct Cyc_List_List*_tmp977=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp977,two.tms);});_tmp745.tms=_tmp978;});_tmp745;}));
goto _LL0;}case 178U: _LL15F: _LL160:
# 1967 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 179U: _LL161: _LL162: {
# 1969 "parse.y"
struct Cyc_Parse_Declarator _tmp4B3=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp4B3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;_tmp746.id=two.id,_tmp746.varloc=two.varloc,({struct Cyc_List_List*_tmp97A=({struct Cyc_List_List*_tmp979=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp979,two.tms);});_tmp746.tms=_tmp97A;});_tmp746;}));
goto _LL0;}case 180U: _LL163: _LL164:
# 1975 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;({struct _tuple0*_tmp97B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp747.id=_tmp97B;}),_tmp747.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp747.tms=0;_tmp747;}));
goto _LL0;case 181U: _LL165: _LL166:
# 1977 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 182U: _LL167: _LL168: {
# 1981 "parse.y"
struct Cyc_Parse_Declarator _tmp4B4=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4B4;
({struct Cyc_List_List*_tmp97E=({struct Cyc_List_List*_tmp4B6=_region_malloc(yyr,sizeof(*_tmp4B6));({void*_tmp97D=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));_tmp4B5->tag=5U,_tmp4B5->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp97C=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4B5->f2=_tmp97C;});_tmp4B5;});_tmp4B6->hd=_tmp97D;}),_tmp4B6->tl=d.tms;_tmp4B6;});d.tms=_tmp97E;});
yyval=(yyyvsp[2]).v;
# 1985
goto _LL0;}case 183U: _LL169: _LL16A:
# 1986 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;({struct _tuple0*_tmp984=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp748.id=_tmp984;}),({unsigned _tmp983=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp748.varloc=_tmp983;}),({struct Cyc_List_List*_tmp982=({struct Cyc_List_List*_tmp4B8=_region_malloc(yyr,sizeof(*_tmp4B8));({void*_tmp981=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4B7=_region_malloc(yyr,sizeof(*_tmp4B7));_tmp4B7->tag=0U,({void*_tmp980=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4B7->f1=_tmp980;}),_tmp4B7->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4B7;});_tmp4B8->hd=_tmp981;}),({struct Cyc_List_List*_tmp97F=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4B8->tl=_tmp97F;});_tmp4B8;});_tmp748.tms=_tmp982;});_tmp748;}));
goto _LL0;case 184U: _LL16B: _LL16C:
# 1988 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;({struct _tuple0*_tmp98B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp749.id=_tmp98B;}),({unsigned _tmp98A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp749.varloc=_tmp98A;}),({
struct Cyc_List_List*_tmp989=({struct Cyc_List_List*_tmp4BA=_region_malloc(yyr,sizeof(*_tmp4BA));({void*_tmp988=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4B9=_region_malloc(yyr,sizeof(*_tmp4B9));_tmp4B9->tag=1U,({struct Cyc_Absyn_Exp*_tmp987=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4B9->f1=_tmp987;}),({void*_tmp986=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4B9->f2=_tmp986;}),_tmp4B9->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4B9;});_tmp4BA->hd=_tmp988;}),({struct Cyc_List_List*_tmp985=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4BA->tl=_tmp985;});_tmp4BA;});_tmp749.tms=_tmp989;});_tmp749;}));
goto _LL0;case 185U: _LL16D: _LL16E: {
# 1991 "parse.y"
struct _tuple27*_tmp4BB=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp1B=_tmp4BB;struct _tuple27*_tmp4BC=_stmttmp1B;struct Cyc_List_List*_tmp4C1;void*_tmp4C0;struct Cyc_Absyn_VarargInfo*_tmp4BF;int _tmp4BE;struct Cyc_List_List*_tmp4BD;_LL46B: _tmp4BD=_tmp4BC->f1;_tmp4BE=_tmp4BC->f2;_tmp4BF=_tmp4BC->f3;_tmp4C0=_tmp4BC->f4;_tmp4C1=_tmp4BC->f5;_LL46C: {struct Cyc_List_List*lis=_tmp4BD;int b=_tmp4BE;struct Cyc_Absyn_VarargInfo*c=_tmp4BF;void*eff=_tmp4C0;struct Cyc_List_List*po=_tmp4C1;
struct Cyc_Absyn_Exp*_tmp4C2=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4C2;
struct Cyc_Absyn_Exp*_tmp4C3=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4C3;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;({struct _tuple0*_tmp991=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74A.id=_tmp991;}),({unsigned _tmp990=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74A.varloc=_tmp990;}),({struct Cyc_List_List*_tmp98F=({struct Cyc_List_List*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));({void*_tmp98E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));_tmp4C5->tag=3U,({void*_tmp98D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));_tmp4C4->tag=1U,_tmp4C4->f1=lis,_tmp4C4->f2=b,_tmp4C4->f3=c,_tmp4C4->f4=eff,_tmp4C4->f5=po,_tmp4C4->f6=req,_tmp4C4->f7=ens;_tmp4C4;});_tmp4C5->f1=_tmp98D;});_tmp4C5;});_tmp4C6->hd=_tmp98E;}),({struct Cyc_List_List*_tmp98C=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C6->tl=_tmp98C;});_tmp4C6;});_tmp74A.tms=_tmp98F;});_tmp74A;}));
# 1996
goto _LL0;}}case 186U: _LL16F: _LL170:
# 1997 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74B;({struct _tuple0*_tmp99B=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74B.id=_tmp99B;}),({unsigned _tmp99A=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74B.varloc=_tmp99A;}),({
struct Cyc_List_List*_tmp999=({struct Cyc_List_List*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));({void*_tmp998=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));_tmp4C8->tag=3U,({void*_tmp997=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));_tmp4C7->tag=1U,_tmp4C7->f1=0,_tmp4C7->f2=0,_tmp4C7->f3=0,({
# 2000
void*_tmp996=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4C7->f4=_tmp996;}),({struct Cyc_List_List*_tmp995=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4C7->f5=_tmp995;}),({struct Cyc_Absyn_Exp*_tmp994=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4C7->f6=_tmp994;}),({struct Cyc_Absyn_Exp*_tmp993=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4C7->f7=_tmp993;});_tmp4C7;});
# 1998
_tmp4C8->f1=_tmp997;});_tmp4C8;});_tmp4C9->hd=_tmp998;}),({
# 2001
struct Cyc_List_List*_tmp992=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4C9->tl=_tmp992;});_tmp4C9;});
# 1998
_tmp74B.tms=_tmp999;});_tmp74B;}));
# 2003
goto _LL0;case 187U: _LL171: _LL172:
# 2004 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74C;({struct _tuple0*_tmp9A2=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74C.id=_tmp9A2;}),({unsigned _tmp9A1=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74C.varloc=_tmp9A1;}),({struct Cyc_List_List*_tmp9A0=({struct Cyc_List_List*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));({void*_tmp99F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));_tmp4CB->tag=3U,({void*_tmp99E=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=0U,({struct Cyc_List_List*_tmp99D=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4CA->f1=_tmp99D;}),_tmp4CA->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4CA;});_tmp4CB->f1=_tmp99E;});_tmp4CB;});_tmp4CC->hd=_tmp99F;}),({struct Cyc_List_List*_tmp99C=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CC->tl=_tmp99C;});_tmp4CC;});_tmp74C.tms=_tmp9A0;});_tmp74C;}));
goto _LL0;case 188U: _LL173: _LL174: {
# 2007
struct Cyc_List_List*_tmp4CD=({unsigned _tmp9A3=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9A3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4CD;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74D;({struct _tuple0*_tmp9A8=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74D.id=_tmp9A8;}),({unsigned _tmp9A7=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74D.varloc=_tmp9A7;}),({struct Cyc_List_List*_tmp9A6=({struct Cyc_List_List*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));({void*_tmp9A5=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));_tmp4CE->tag=4U,_tmp4CE->f1=ts,_tmp4CE->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4CE->f3=0;_tmp4CE;});_tmp4CF->hd=_tmp9A5;}),({struct Cyc_List_List*_tmp9A4=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4CF->tl=_tmp9A4;});_tmp4CF;});_tmp74D.tms=_tmp9A6;});_tmp74D;}));
# 2010
goto _LL0;}case 189U: _LL175: _LL176:
# 2011 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74E;({struct _tuple0*_tmp9AE=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).id;_tmp74E.id=_tmp9AE;}),({unsigned _tmp9AD=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).varloc;_tmp74E.varloc=_tmp9AD;}),({struct Cyc_List_List*_tmp9AC=({struct Cyc_List_List*_tmp4D1=_region_malloc(yyr,sizeof(*_tmp4D1));({void*_tmp9AB=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));_tmp4D0->tag=5U,_tmp4D0->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9AA=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D0->f2=_tmp9AA;});_tmp4D0;});_tmp4D1->hd=_tmp9AB;}),({
struct Cyc_List_List*_tmp9A9=(Cyc_yyget_YY27(&(yyyvsp[0]).v)).tms;_tmp4D1->tl=_tmp9A9;});_tmp4D1;});
# 2011
_tmp74E.tms=_tmp9AC;});_tmp74E;}));
# 2014
goto _LL0;case 190U: _LL177: _LL178:
# 2019 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74F;({struct _tuple0*_tmp9AF=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp74F.id=_tmp9AF;}),_tmp74F.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp74F.tms=0;_tmp74F;}));
goto _LL0;case 191U: _LL179: _LL17A:
# 2021 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp750;({struct _tuple0*_tmp9B0=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp750.id=_tmp9B0;}),_tmp750.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp750.tms=0;_tmp750;}));
goto _LL0;case 192U: _LL17B: _LL17C:
# 2023 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 193U: _LL17D: _LL17E: {
# 2027 "parse.y"
struct Cyc_Parse_Declarator _tmp4D2=Cyc_yyget_YY27(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp4D2;
({struct Cyc_List_List*_tmp9B3=({struct Cyc_List_List*_tmp4D4=_region_malloc(yyr,sizeof(*_tmp4D4));({void*_tmp9B2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));_tmp4D3->tag=5U,_tmp4D3->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9B1=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4D3->f2=_tmp9B1;});_tmp4D3;});_tmp4D4->hd=_tmp9B2;}),_tmp4D4->tl=d.tms;_tmp4D4;});d.tms=_tmp9B3;});
yyval=(yyyvsp[2]).v;
# 2031
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2032 "parse.y"
struct Cyc_Parse_Declarator _tmp4D5=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D5;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp751;_tmp751.id=one.id,_tmp751.varloc=one.varloc,({
struct Cyc_List_List*_tmp9B6=({struct Cyc_List_List*_tmp4D7=_region_malloc(yyr,sizeof(*_tmp4D7));({void*_tmp9B5=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));_tmp4D6->tag=0U,({void*_tmp9B4=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp4D6->f1=_tmp9B4;}),_tmp4D6->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp4D6;});_tmp4D7->hd=_tmp9B5;}),_tmp4D7->tl=one.tms;_tmp4D7;});_tmp751.tms=_tmp9B6;});_tmp751;}));
goto _LL0;}case 195U: _LL181: _LL182: {
# 2036 "parse.y"
struct Cyc_Parse_Declarator _tmp4D8=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4D8;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp752;_tmp752.id=one.id,_tmp752.varloc=one.varloc,({
struct Cyc_List_List*_tmp9BA=({struct Cyc_List_List*_tmp4DA=_region_malloc(yyr,sizeof(*_tmp4DA));({void*_tmp9B9=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D9=_region_malloc(yyr,sizeof(*_tmp4D9));_tmp4D9->tag=1U,({struct Cyc_Absyn_Exp*_tmp9B8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4D9->f1=_tmp9B8;}),({void*_tmp9B7=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4D9->f2=_tmp9B7;}),_tmp4D9->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp4D9;});_tmp4DA->hd=_tmp9B9;}),_tmp4DA->tl=one.tms;_tmp4DA;});_tmp752.tms=_tmp9BA;});_tmp752;}));
# 2040
goto _LL0;}case 196U: _LL183: _LL184: {
# 2041 "parse.y"
struct _tuple27*_tmp4DB=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp1C=_tmp4DB;struct _tuple27*_tmp4DC=_stmttmp1C;struct Cyc_List_List*_tmp4E1;void*_tmp4E0;struct Cyc_Absyn_VarargInfo*_tmp4DF;int _tmp4DE;struct Cyc_List_List*_tmp4DD;_LL46E: _tmp4DD=_tmp4DC->f1;_tmp4DE=_tmp4DC->f2;_tmp4DF=_tmp4DC->f3;_tmp4E0=_tmp4DC->f4;_tmp4E1=_tmp4DC->f5;_LL46F: {struct Cyc_List_List*lis=_tmp4DD;int b=_tmp4DE;struct Cyc_Absyn_VarargInfo*c=_tmp4DF;void*eff=_tmp4E0;struct Cyc_List_List*po=_tmp4E1;
struct Cyc_Absyn_Exp*_tmp4E2=Cyc_yyget_YY57(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*req=_tmp4E2;
struct Cyc_Absyn_Exp*_tmp4E3=Cyc_yyget_YY57(&(yyyvsp[5]).v);struct Cyc_Absyn_Exp*ens=_tmp4E3;
struct Cyc_Parse_Declarator _tmp4E4=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E4;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp753;_tmp753.id=one.id,_tmp753.varloc=one.varloc,({struct Cyc_List_List*_tmp9BD=({struct Cyc_List_List*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));({void*_tmp9BC=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E6=_region_malloc(yyr,sizeof(*_tmp4E6));_tmp4E6->tag=3U,({void*_tmp9BB=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));_tmp4E5->tag=1U,_tmp4E5->f1=lis,_tmp4E5->f2=b,_tmp4E5->f3=c,_tmp4E5->f4=eff,_tmp4E5->f5=po,_tmp4E5->f6=req,_tmp4E5->f7=ens;_tmp4E5;});_tmp4E6->f1=_tmp9BB;});_tmp4E6;});_tmp4E7->hd=_tmp9BC;}),_tmp4E7->tl=one.tms;_tmp4E7;});_tmp753.tms=_tmp9BD;});_tmp753;}));
# 2047
goto _LL0;}}case 197U: _LL185: _LL186: {
# 2048 "parse.y"
struct Cyc_Parse_Declarator _tmp4E8=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4E8;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp754;_tmp754.id=one.id,_tmp754.varloc=one.varloc,({
struct Cyc_List_List*_tmp9C4=({struct Cyc_List_List*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));({void*_tmp9C3=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));_tmp4EA->tag=3U,({void*_tmp9C2=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E9=_region_malloc(yyr,sizeof(*_tmp4E9));_tmp4E9->tag=1U,_tmp4E9->f1=0,_tmp4E9->f2=0,_tmp4E9->f3=0,({
# 2052
void*_tmp9C1=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp4E9->f4=_tmp9C1;}),({struct Cyc_List_List*_tmp9C0=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4E9->f5=_tmp9C0;}),({struct Cyc_Absyn_Exp*_tmp9BF=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp4E9->f6=_tmp9BF;}),({struct Cyc_Absyn_Exp*_tmp9BE=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp4E9->f7=_tmp9BE;});_tmp4E9;});
# 2050
_tmp4EA->f1=_tmp9C2;});_tmp4EA;});_tmp4EB->hd=_tmp9C3;}),_tmp4EB->tl=one.tms;_tmp4EB;});_tmp754.tms=_tmp9C4;});_tmp754;}));
# 2055
goto _LL0;}case 198U: _LL187: _LL188: {
# 2056 "parse.y"
struct Cyc_Parse_Declarator _tmp4EC=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4EC;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp755;_tmp755.id=one.id,_tmp755.varloc=one.varloc,({struct Cyc_List_List*_tmp9C8=({struct Cyc_List_List*_tmp4EF=_region_malloc(yyr,sizeof(*_tmp4EF));({void*_tmp9C7=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));_tmp4EE->tag=3U,({void*_tmp9C6=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));_tmp4ED->tag=0U,({struct Cyc_List_List*_tmp9C5=Cyc_yyget_YY36(&(yyyvsp[2]).v);_tmp4ED->f1=_tmp9C5;}),_tmp4ED->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp4ED;});_tmp4EE->f1=_tmp9C6;});_tmp4EE;});_tmp4EF->hd=_tmp9C7;}),_tmp4EF->tl=one.tms;_tmp4EF;});_tmp755.tms=_tmp9C8;});_tmp755;}));
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2060
struct Cyc_List_List*_tmp4F0=({unsigned _tmp9C9=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9C9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp4F0;
struct Cyc_Parse_Declarator _tmp4F1=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F1;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp756;_tmp756.id=one.id,_tmp756.varloc=one.varloc,({struct Cyc_List_List*_tmp9CB=({struct Cyc_List_List*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));({void*_tmp9CA=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4F2=_region_malloc(yyr,sizeof(*_tmp4F2));_tmp4F2->tag=4U,_tmp4F2->f1=ts,_tmp4F2->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp4F2->f3=0;_tmp4F2;});_tmp4F3->hd=_tmp9CA;}),_tmp4F3->tl=one.tms;_tmp4F3;});_tmp756.tms=_tmp9CB;});_tmp756;}));
# 2064
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2065 "parse.y"
struct Cyc_Parse_Declarator _tmp4F4=Cyc_yyget_YY27(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp4F4;
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp757;_tmp757.id=one.id,_tmp757.varloc=one.varloc,({struct Cyc_List_List*_tmp9CE=({struct Cyc_List_List*_tmp4F6=_region_malloc(yyr,sizeof(*_tmp4F6));({void*_tmp9CD=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));_tmp4F5->tag=5U,_tmp4F5->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9CC=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp4F5->f2=_tmp9CC;});_tmp4F5;});_tmp4F6->hd=_tmp9CD;}),_tmp4F6->tl=one.tms;_tmp4F6;});_tmp757.tms=_tmp9CE;});_tmp757;}));
# 2068
goto _LL0;}case 201U: _LL18D: _LL18E:
# 2072 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 202U: _LL18F: _LL190:
# 2073 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9CF=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9CF,Cyc_yyget_YY26(&(yyyvsp[1]).v));}));
goto _LL0;case 203U: _LL191: _LL192: {
# 2077 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp4F8=_region_malloc(yyr,sizeof(*_tmp4F8));({void*_tmp9D1=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F7=_region_malloc(yyr,sizeof(*_tmp4F7));_tmp4F7->tag=5U,_tmp4F7->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp9D0=Cyc_yyget_YY45(&(yyyvsp[3]).v);_tmp4F7->f2=_tmp9D0;});_tmp4F7;});_tmp4F8->hd=_tmp9D1;}),_tmp4F8->tl=ans;_tmp4F8;});{
# 2081
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp4F9=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp1D=_tmp4F9;struct _tuple22 _tmp4FA=_stmttmp1D;void*_tmp4FD;void*_tmp4FC;unsigned _tmp4FB;_LL471: _tmp4FB=_tmp4FA.f1;_tmp4FC=_tmp4FA.f2;_tmp4FD=_tmp4FA.f3;_LL472: {unsigned ploc=_tmp4FB;void*nullable=_tmp4FC;void*bound=_tmp4FD;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));_tmp4FE->ptr_loc=ploc,_tmp4FE->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp4FE->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp4FE;});{
# 2086
struct _tuple15 _tmp4FF=({unsigned _tmp9D5=ploc;void*_tmp9D4=nullable;void*_tmp9D3=bound;void*_tmp9D2=Cyc_yyget_YY44(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp9D5,_tmp9D4,_tmp9D3,_tmp9D2,Cyc_yyget_YY56(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp1E=_tmp4FF;struct _tuple15 _tmp500=_stmttmp1E;void*_tmp504;void*_tmp503;void*_tmp502;void*_tmp501;_LL474: _tmp501=_tmp500.f1;_tmp502=_tmp500.f2;_tmp503=_tmp500.f3;_tmp504=_tmp500.f4;_LL475: {void*nullable=_tmp501;void*bound=_tmp502;void*zeroterm=_tmp503;void*rgn_opt=_tmp504;
ans=({struct Cyc_List_List*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));({void*_tmp9D7=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp505=_region_malloc(yyr,sizeof(*_tmp505));_tmp505->tag=2U,(_tmp505->f1).rgn=rgn_opt,(_tmp505->f1).nullable=nullable,(_tmp505->f1).bounds=bound,(_tmp505->f1).zero_term=zeroterm,(_tmp505->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp9D6=Cyc_yyget_YY23(&(yyyvsp[4]).v);_tmp505->f2=_tmp9D6;});_tmp505;});_tmp506->hd=_tmp9D7;}),_tmp506->tl=ans;_tmp506;});
yyval=Cyc_YY26(ans);
# 2090
goto _LL0;}}}}}case 204U: _LL193: _LL194:
# 2092
 yyval=Cyc_YY56(0);
goto _LL0;case 205U: _LL195: _LL196:
# 2093 "parse.y"
 yyval=Cyc_YY56(({struct Cyc_List_List*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));({void*_tmp9D9=Cyc_yyget_YY55(&(yyyvsp[0]).v);_tmp507->hd=_tmp9D9;}),({struct Cyc_List_List*_tmp9D8=Cyc_yyget_YY56(&(yyyvsp[1]).v);_tmp507->tl=_tmp9D8;});_tmp507;}));
goto _LL0;case 206U: _LL197: _LL198:
# 2098 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp508=_region_malloc(yyr,sizeof(*_tmp508));_tmp508->tag=0U,({struct Cyc_Absyn_Exp*_tmp9DA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp508->f1=_tmp9DA;});_tmp508;}));
goto _LL0;case 207U: _LL199: _LL19A:
# 2100 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));_tmp509->tag=1U,({void*_tmp9DB=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp509->f1=_tmp9DB;});_tmp509;}));
goto _LL0;case 208U: _LL19B: _LL19C:
# 2102 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp50A=_region_malloc(yyr,sizeof(*_tmp50A));_tmp50A->tag=2U;_tmp50A;}));
goto _LL0;case 209U: _LL19D: _LL19E:
# 2104 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp50B=_region_malloc(yyr,sizeof(*_tmp50B));_tmp50B->tag=3U;_tmp50B;}));
goto _LL0;case 210U: _LL19F: _LL1A0:
# 2106 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50C=_region_malloc(yyr,sizeof(*_tmp50C));_tmp50C->tag=4U;_tmp50C;}));
goto _LL0;case 211U: _LL1A1: _LL1A2:
# 2108 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp50D=_region_malloc(yyr,sizeof(*_tmp50D));_tmp50D->tag=5U;_tmp50D;}));
goto _LL0;case 212U: _LL1A3: _LL1A4:
# 2110 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp50E=_region_malloc(yyr,sizeof(*_tmp50E));_tmp50E->tag=6U;_tmp50E;}));
goto _LL0;case 213U: _LL1A5: _LL1A6:
# 2112 "parse.y"
 yyval=Cyc_YY55((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp50F=_region_malloc(yyr,sizeof(*_tmp50F));_tmp50F->tag=7U;_tmp50F;}));
goto _LL0;case 214U: _LL1A7: _LL1A8: {
# 2118 "parse.y"
unsigned loc=(unsigned)((yyyvsp[0]).l).first_line;
if(!Cyc_Parse_parsing_tempest)
yyval=Cyc_YY1(({struct _tuple22*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->f1=loc,_tmp510->f2=Cyc_Absyn_true_type,({void*_tmp9DC=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp510->f3=_tmp9DC;});_tmp510;}));else{
# 2122
yyval=Cyc_YY1(({struct _tuple22*_tmp511=_cycalloc(sizeof(*_tmp511));_tmp511->f1=loc,_tmp511->f2=Cyc_Absyn_true_type,_tmp511->f3=Cyc_Absyn_fat_bound_type;_tmp511;}));}
# 2124
goto _LL0;}case 215U: _LL1A9: _LL1AA: {
# 2125 "parse.y"
unsigned loc=(unsigned)((yyyvsp[0]).l).first_line;
yyval=Cyc_YY1(({struct _tuple22*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->f1=loc,_tmp512->f2=Cyc_Absyn_false_type,({void*_tmp9DD=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp512->f3=_tmp9DD;});_tmp512;}));
# 2128
goto _LL0;}case 216U: _LL1AB: _LL1AC: {
# 2129 "parse.y"
unsigned loc=(unsigned)((yyyvsp[0]).l).first_line;
yyval=Cyc_YY1(({struct _tuple22*_tmp513=_cycalloc(sizeof(*_tmp513));_tmp513->f1=loc,_tmp513->f2=Cyc_Absyn_true_type,_tmp513->f3=Cyc_Absyn_fat_bound_type;_tmp513;}));
# 2132
goto _LL0;}case 217U: _LL1AD: _LL1AE:
# 2134
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 218U: _LL1AF: _LL1B0:
# 2135 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 219U: _LL1B1: _LL1B2:
# 2138
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 220U: _LL1B3: _LL1B4:
# 2139 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;case 221U: _LL1B5: _LL1B6:
# 2140 "parse.y"
 yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;case 222U: _LL1B7: _LL1B8:
# 2145 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 223U: _LL1B9: _LL1BA:
# 2146 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Tcutil_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 224U: _LL1BB: _LL1BC:
# 2147 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 225U: _LL1BD: _LL1BE:
# 2151 "parse.y"
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 226U: _LL1BF: _LL1C0:
# 2152 "parse.y"
 yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9DE=Cyc_yyget_YY23(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp9DE,Cyc_yyget_YY23(&(yyyvsp[1]).v));}));
goto _LL0;case 227U: _LL1C1: _LL1C2:
# 2157 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp514=_cycalloc(sizeof(*_tmp514));({struct Cyc_List_List*_tmp9E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp514->f1=_tmp9E1;}),_tmp514->f2=0,_tmp514->f3=0,({void*_tmp9E0=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp514->f4=_tmp9E0;}),({struct Cyc_List_List*_tmp9DF=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp514->f5=_tmp9DF;});_tmp514;}));
goto _LL0;case 228U: _LL1C3: _LL1C4:
# 2159 "parse.y"
 yyval=Cyc_YY39(({struct _tuple27*_tmp515=_cycalloc(sizeof(*_tmp515));({struct Cyc_List_List*_tmp9E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp515->f1=_tmp9E4;}),_tmp515->f2=1,_tmp515->f3=0,({void*_tmp9E3=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp515->f4=_tmp9E3;}),({struct Cyc_List_List*_tmp9E2=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp515->f5=_tmp9E2;});_tmp515;}));
goto _LL0;case 229U: _LL1C5: _LL1C6: {
# 2162
struct _tuple8*_tmp516=Cyc_yyget_YY37(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1F=_tmp516;struct _tuple8*_tmp517=_stmttmp1F;void*_tmp51A;struct Cyc_Absyn_Tqual _tmp519;struct _fat_ptr*_tmp518;_LL477: _tmp518=_tmp517->f1;_tmp519=_tmp517->f2;_tmp51A=_tmp517->f3;_LL478: {struct _fat_ptr*n=_tmp518;struct Cyc_Absyn_Tqual tq=_tmp519;void*t=_tmp51A;
struct Cyc_Absyn_VarargInfo*_tmp51B=({struct Cyc_Absyn_VarargInfo*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->name=n,_tmp51D->tq=tq,_tmp51D->type=t,({int _tmp9E5=Cyc_yyget_YY31(&(yyyvsp[1]).v);_tmp51D->inject=_tmp9E5;});_tmp51D;});struct Cyc_Absyn_VarargInfo*v=_tmp51B;
yyval=Cyc_YY39(({struct _tuple27*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->f1=0,_tmp51C->f2=0,_tmp51C->f3=v,({void*_tmp9E7=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp51C->f4=_tmp9E7;}),({struct Cyc_List_List*_tmp9E6=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp51C->f5=_tmp9E6;});_tmp51C;}));
# 2166
goto _LL0;}}case 230U: _LL1C7: _LL1C8: {
# 2168
struct _tuple8*_tmp51E=Cyc_yyget_YY37(&(yyyvsp[4]).v);struct _tuple8*_stmttmp20=_tmp51E;struct _tuple8*_tmp51F=_stmttmp20;void*_tmp522;struct Cyc_Absyn_Tqual _tmp521;struct _fat_ptr*_tmp520;_LL47A: _tmp520=_tmp51F->f1;_tmp521=_tmp51F->f2;_tmp522=_tmp51F->f3;_LL47B: {struct _fat_ptr*n=_tmp520;struct Cyc_Absyn_Tqual tq=_tmp521;void*t=_tmp522;
struct Cyc_Absyn_VarargInfo*_tmp523=({struct Cyc_Absyn_VarargInfo*_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525->name=n,_tmp525->tq=tq,_tmp525->type=t,({int _tmp9E8=Cyc_yyget_YY31(&(yyyvsp[3]).v);_tmp525->inject=_tmp9E8;});_tmp525;});struct Cyc_Absyn_VarargInfo*v=_tmp523;
yyval=Cyc_YY39(({struct _tuple27*_tmp524=_cycalloc(sizeof(*_tmp524));({struct Cyc_List_List*_tmp9EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(&(yyyvsp[0]).v));_tmp524->f1=_tmp9EB;}),_tmp524->f2=0,_tmp524->f3=v,({void*_tmp9EA=Cyc_yyget_YY49(&(yyyvsp[5]).v);_tmp524->f4=_tmp9EA;}),({struct Cyc_List_List*_tmp9E9=Cyc_yyget_YY50(&(yyyvsp[6]).v);_tmp524->f5=_tmp9E9;});_tmp524;}));
# 2172
goto _LL0;}}case 231U: _LL1C9: _LL1CA:
# 2176 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9EC=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9EC,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->tag=1U,_tmp526->f1=0;_tmp526;}));}));
goto _LL0;case 232U: _LL1CB: _LL1CC:
# 2177 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp9ED=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp9ED,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(&(yyyvsp[2]).v)));}));
goto _LL0;case 233U: _LL1CD: _LL1CE:
# 2180
 yyval=Cyc_YY49(0);
goto _LL0;case 234U: _LL1CF: _LL1D0:
# 2181 "parse.y"
 yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 235U: _LL1D1: _LL1D2:
# 2185 "parse.y"
 yyval=Cyc_YY50(0);
goto _LL0;case 236U: _LL1D3: _LL1D4:
# 2186 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 237U: _LL1D5: _LL1D6: {
# 2194 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp527=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->tag=2U,_tmp52C->f1=0,_tmp52C->f2=& Cyc_Tcutil_trk;_tmp52C;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp527;
struct _fat_ptr _tmp528=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr id=_tmp528;
void*_tmp529=Cyc_Parse_id2type(id,(void*)kb);void*t=_tmp529;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp52B=_cycalloc(sizeof(*_tmp52B));({struct _tuple34*_tmp9EF=({struct _tuple34*_tmp52A=_cycalloc(sizeof(*_tmp52A));({void*_tmp9EE=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp52A->f1=_tmp9EE;}),_tmp52A->f2=t;_tmp52A;});_tmp52B->hd=_tmp9EF;}),_tmp52B->tl=0;_tmp52B;}));
# 2199
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2201 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52D=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->tag=2U,_tmp532->f1=0,_tmp532->f2=& Cyc_Tcutil_trk;_tmp532;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp52D;
struct _fat_ptr _tmp52E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);struct _fat_ptr id=_tmp52E;
void*_tmp52F=Cyc_Parse_id2type(id,(void*)kb);void*t=_tmp52F;
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp531=_cycalloc(sizeof(*_tmp531));({struct _tuple34*_tmp9F2=({struct _tuple34*_tmp530=_cycalloc(sizeof(*_tmp530));({void*_tmp9F1=Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[0]).v));_tmp530->f1=_tmp9F1;}),_tmp530->f2=t;_tmp530;});_tmp531->hd=_tmp9F2;}),({struct Cyc_List_List*_tmp9F0=Cyc_yyget_YY50(&(yyyvsp[4]).v);_tmp531->tl=_tmp9F0;});_tmp531;}));
# 2206
goto _LL0;}case 239U: _LL1D9: _LL1DA:
# 2210 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 240U: _LL1DB: _LL1DC:
# 2212 "parse.y"
 if(({struct _fat_ptr _tmp9F3=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp9F3,({const char*_tmp533="inject";_tag_fat(_tmp533,sizeof(char),7U);}));})!= 0)
({void*_tmp534=0U;({unsigned _tmp9F5=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp9F4=({const char*_tmp535="missing type in function declaration";_tag_fat(_tmp535,sizeof(char),37U);});Cyc_Warn_err(_tmp9F5,_tmp9F4,_tag_fat(_tmp534,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);
# 2216
goto _LL0;case 241U: _LL1DD: _LL1DE:
# 2219 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 242U: _LL1DF: _LL1E0:
# 2220 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp9F6=Cyc_yyget_YY40(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F6,Cyc_yyget_YY40(&(yyyvsp[2]).v));}));
goto _LL0;case 243U: _LL1E1: _LL1E2:
# 2224 "parse.y"
 yyval=Cyc_YY40(0);
goto _LL0;case 244U: _LL1E3: _LL1E4:
# 2225 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 245U: _LL1E5: _LL1E6:
# 2227 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));({void*_tmp9F7=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v));_tmp536->hd=_tmp9F7;}),_tmp536->tl=0;_tmp536;}));
goto _LL0;case 246U: _LL1E7: _LL1E8:
# 2229 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(&(yyyvsp[0]).v),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));({void*_tmp9F8=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp537->hd=_tmp9F8;}),_tmp537->tl=0;_tmp537;}));
# 2232
goto _LL0;case 247U: _LL1E9: _LL1EA:
# 2249 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp538=_cycalloc(sizeof(*_tmp538));({void*_tmp9FA=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9F9=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9F9,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp538->hd=_tmp9FA;}),_tmp538->tl=0;_tmp538;}));
goto _LL0;case 248U: _LL1EB: _LL1EC:
# 2251 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp539=_cycalloc(sizeof(*_tmp539));({void*_tmp9FD=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9FC=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp9FC,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp539->hd=_tmp9FD;}),({struct Cyc_List_List*_tmp9FB=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp539->tl=_tmp9FB;});_tmp539;}));
goto _LL0;case 249U: _LL1ED: _LL1EE:
# 2257 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp53A=_cycalloc(sizeof(*_tmp53A));({struct _tuple8*_tmp9FE=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp53A->hd=_tmp9FE;}),_tmp53A->tl=0;_tmp53A;}));
goto _LL0;case 250U: _LL1EF: _LL1F0:
# 2259 "parse.y"
 yyval=Cyc_YY38(({struct Cyc_List_List*_tmp53B=_cycalloc(sizeof(*_tmp53B));({struct _tuple8*_tmpA00=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp53B->hd=_tmpA00;}),({struct Cyc_List_List*_tmp9FF=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp53B->tl=_tmp9FF;});_tmp53B;}));
goto _LL0;case 251U: _LL1F1: _LL1F2: {
# 2265 "parse.y"
struct _tuple26 _tmp53C=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp21=_tmp53C;struct _tuple26 _tmp53D=_stmttmp21;struct Cyc_List_List*_tmp540;struct Cyc_Parse_Type_specifier _tmp53F;struct Cyc_Absyn_Tqual _tmp53E;_LL47D: _tmp53E=_tmp53D.f1;_tmp53F=_tmp53D.f2;_tmp540=_tmp53D.f3;_LL47E: {struct Cyc_Absyn_Tqual tq=_tmp53E;struct Cyc_Parse_Type_specifier tspecs=_tmp53F;struct Cyc_List_List*atts=_tmp540;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp541=Cyc_yyget_YY27(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp22=_tmp541;struct Cyc_Parse_Declarator _tmp542=_stmttmp22;struct Cyc_List_List*_tmp545;unsigned _tmp544;struct _tuple0*_tmp543;_LL480: _tmp543=_tmp542.id;_tmp544=_tmp542.varloc;_tmp545=_tmp542.tms;_LL481: {struct _tuple0*qv=_tmp543;unsigned varloc=_tmp544;struct Cyc_List_List*tms=_tmp545;
void*_tmp546=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp546;
struct _tuple14 _tmp547=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp23=_tmp547;struct _tuple14 _tmp548=_stmttmp23;struct Cyc_List_List*_tmp54C;struct Cyc_List_List*_tmp54B;void*_tmp54A;struct Cyc_Absyn_Tqual _tmp549;_LL483: _tmp549=_tmp548.f1;_tmp54A=_tmp548.f2;_tmp54B=_tmp548.f3;_tmp54C=_tmp548.f4;_LL484: {struct Cyc_Absyn_Tqual tq2=_tmp549;void*t2=_tmp54A;struct Cyc_List_List*tvs=_tmp54B;struct Cyc_List_List*atts2=_tmp54C;
if(tvs != 0)
({void*_tmp54D=0U;({unsigned _tmpA02=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA01=({const char*_tmp54E="parameter with bad type params";_tag_fat(_tmp54E,sizeof(char),31U);});Cyc_Warn_err(_tmpA02,_tmpA01,_tag_fat(_tmp54D,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({void*_tmp54F=0U;({unsigned _tmpA04=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA03=({const char*_tmp550="parameter cannot be qualified with a namespace";_tag_fat(_tmp550,sizeof(char),47U);});Cyc_Warn_err(_tmpA04,_tmpA03,_tag_fat(_tmp54F,sizeof(void*),0U));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({void*_tmp551=0U;({unsigned _tmpA06=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA05=({const char*_tmp552="extra attributes on parameter, ignoring";_tag_fat(_tmp552,sizeof(char),40U);});Cyc_Warn_warn(_tmpA06,_tmpA05,_tag_fat(_tmp551,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->f1=idopt,_tmp553->f2=tq2,_tmp553->f3=t2;_tmp553;}));
# 2279
goto _LL0;}}}}}}case 252U: _LL1F3: _LL1F4: {
# 2280 "parse.y"
struct _tuple26 _tmp554=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp24=_tmp554;struct _tuple26 _tmp555=_stmttmp24;struct Cyc_List_List*_tmp558;struct Cyc_Parse_Type_specifier _tmp557;struct Cyc_Absyn_Tqual _tmp556;_LL486: _tmp556=_tmp555.f1;_tmp557=_tmp555.f2;_tmp558=_tmp555.f3;_LL487: {struct Cyc_Absyn_Tqual tq=_tmp556;struct Cyc_Parse_Type_specifier tspecs=_tmp557;struct Cyc_List_List*atts=_tmp558;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp559=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp559;
if(atts != 0)
({void*_tmp55A=0U;({unsigned _tmpA08=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA07=({const char*_tmp55B="bad attributes on parameter, ignoring";_tag_fat(_tmp55B,sizeof(char),38U);});Cyc_Warn_warn(_tmpA08,_tmpA07,_tag_fat(_tmp55A,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->f1=0,_tmp55C->f2=tq,_tmp55C->f3=t;_tmp55C;}));
# 2287
goto _LL0;}}}case 253U: _LL1F5: _LL1F6: {
# 2288 "parse.y"
struct _tuple26 _tmp55D=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp25=_tmp55D;struct _tuple26 _tmp55E=_stmttmp25;struct Cyc_List_List*_tmp561;struct Cyc_Parse_Type_specifier _tmp560;struct Cyc_Absyn_Tqual _tmp55F;_LL489: _tmp55F=_tmp55E.f1;_tmp560=_tmp55E.f2;_tmp561=_tmp55E.f3;_LL48A: {struct Cyc_Absyn_Tqual tq=_tmp55F;struct Cyc_Parse_Type_specifier tspecs=_tmp560;struct Cyc_List_List*atts=_tmp561;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp562=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp562;
struct Cyc_List_List*_tmp563=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp563;
struct _tuple14 _tmp564=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp26=_tmp564;struct _tuple14 _tmp565=_stmttmp26;struct Cyc_List_List*_tmp569;struct Cyc_List_List*_tmp568;void*_tmp567;struct Cyc_Absyn_Tqual _tmp566;_LL48C: _tmp566=_tmp565.f1;_tmp567=_tmp565.f2;_tmp568=_tmp565.f3;_tmp569=_tmp565.f4;_LL48D: {struct Cyc_Absyn_Tqual tq2=_tmp566;void*t2=_tmp567;struct Cyc_List_List*tvs=_tmp568;struct Cyc_List_List*atts2=_tmp569;
if(tvs != 0)
({void*_tmp56A=0U;({unsigned _tmpA0A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA09=({const char*_tmp56B="bad type parameters on formal argument, ignoring";_tag_fat(_tmp56B,sizeof(char),49U);});Cyc_Warn_warn(_tmpA0A,_tmpA09,_tag_fat(_tmp56A,sizeof(void*),0U));});});
# 2296
if(atts2 != 0)
({void*_tmp56C=0U;({unsigned _tmpA0C=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA0B=({const char*_tmp56D="bad attributes on parameter, ignoring";_tag_fat(_tmp56D,sizeof(char),38U);});Cyc_Warn_warn(_tmpA0C,_tmpA0B,_tag_fat(_tmp56C,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->f1=0,_tmp56E->f2=tq2,_tmp56E->f3=t2;_tmp56E;}));
# 2300
goto _LL0;}}}}case 254U: _LL1F7: _LL1F8:
# 2304 "parse.y"
 yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(&(yyyvsp[0]).v)));
goto _LL0;case 255U: _LL1F9: _LL1FA:
# 2308 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp570=_cycalloc(sizeof(*_tmp570));({struct _fat_ptr*_tmpA0E=({struct _fat_ptr*_tmp56F=_cycalloc(sizeof(*_tmp56F));({struct _fat_ptr _tmpA0D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp56F=_tmpA0D;});_tmp56F;});_tmp570->hd=_tmpA0E;}),_tmp570->tl=0;_tmp570;}));
goto _LL0;case 256U: _LL1FB: _LL1FC:
# 2310 "parse.y"
 yyval=Cyc_YY36(({struct Cyc_List_List*_tmp572=_cycalloc(sizeof(*_tmp572));({struct _fat_ptr*_tmpA11=({struct _fat_ptr*_tmp571=_cycalloc(sizeof(*_tmp571));({struct _fat_ptr _tmpA10=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp571=_tmpA10;});_tmp571;});_tmp572->hd=_tmpA11;}),({struct Cyc_List_List*_tmpA0F=Cyc_yyget_YY36(&(yyyvsp[0]).v);_tmp572->tl=_tmpA0F;});_tmp572;}));
goto _LL0;case 257U: _LL1FD: _LL1FE:
# 2314 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 258U: _LL1FF: _LL200:
# 2315 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 259U: _LL201: _LL202:
# 2320 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA12=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=36U,_tmp573->f1=0,_tmp573->f2=0;_tmp573;});Cyc_Absyn_new_exp(_tmpA12,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 260U: _LL203: _LL204:
# 2322 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA14=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->tag=36U,_tmp574->f1=0,({struct Cyc_List_List*_tmpA13=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp574->f2=_tmpA13;});_tmp574;});Cyc_Absyn_new_exp(_tmpA14,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 261U: _LL205: _LL206:
# 2324 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpA16=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->tag=36U,_tmp575->f1=0,({struct Cyc_List_List*_tmpA15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp575->f2=_tmpA15;});_tmp575;});Cyc_Absyn_new_exp(_tmpA16,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 262U: _LL207: _LL208: {
# 2326 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp576=({unsigned _tmpA1B=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmpA1A=({struct _tuple0*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA18=({struct _fat_ptr*_tmp578=_cycalloc(sizeof(*_tmp578));({struct _fat_ptr _tmpA17=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp578=_tmpA17;});_tmp578;});_tmp579->f2=_tmpA18;});_tmp579;});void*_tmpA19=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpA1B,_tmpA1A,_tmpA19,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2326
struct Cyc_Absyn_Vardecl*vd=_tmp576;
# 2329
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmpA1E=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->tag=27U,_tmp577->f1=vd,({struct Cyc_Absyn_Exp*_tmpA1D=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp577->f2=_tmpA1D;}),({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp577->f3=_tmpA1C;}),_tmp577->f4=0;_tmp577;});Cyc_Absyn_new_exp(_tmpA1E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2332
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2334 "parse.y"
void*_tmp57A=({struct _tuple8*_tmpA1F=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpA1F,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp57A;
yyval=Cyc_Exp_tok(({void*_tmpA21=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=28U,({struct Cyc_Absyn_Exp*_tmpA20=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp57B->f1=_tmpA20;}),_tmp57B->f2=t,_tmp57B->f3=0;_tmp57B;});Cyc_Absyn_new_exp(_tmpA21,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2337
goto _LL0;}case 264U: _LL20B: _LL20C:
# 2342 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57D=_cycalloc(sizeof(*_tmp57D));({struct _tuple35*_tmpA23=({struct _tuple35*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->f1=0,({struct Cyc_Absyn_Exp*_tmpA22=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp57C->f2=_tmpA22;});_tmp57C;});_tmp57D->hd=_tmpA23;}),_tmp57D->tl=0;_tmp57D;}));
goto _LL0;case 265U: _LL20D: _LL20E:
# 2344 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57F=_cycalloc(sizeof(*_tmp57F));({struct _tuple35*_tmpA26=({struct _tuple35*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct Cyc_List_List*_tmpA25=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp57E->f1=_tmpA25;}),({struct Cyc_Absyn_Exp*_tmpA24=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp57E->f2=_tmpA24;});_tmp57E;});_tmp57F->hd=_tmpA26;}),_tmp57F->tl=0;_tmp57F;}));
goto _LL0;case 266U: _LL20F: _LL210:
# 2346 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp581=_cycalloc(sizeof(*_tmp581));({struct _tuple35*_tmpA29=({struct _tuple35*_tmp580=_cycalloc(sizeof(*_tmp580));_tmp580->f1=0,({struct Cyc_Absyn_Exp*_tmpA28=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp580->f2=_tmpA28;});_tmp580;});_tmp581->hd=_tmpA29;}),({struct Cyc_List_List*_tmpA27=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp581->tl=_tmpA27;});_tmp581;}));
goto _LL0;case 267U: _LL211: _LL212:
# 2348 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));({struct _tuple35*_tmpA2D=({struct _tuple35*_tmp582=_cycalloc(sizeof(*_tmp582));({struct Cyc_List_List*_tmpA2C=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp582->f1=_tmpA2C;}),({struct Cyc_Absyn_Exp*_tmpA2B=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp582->f2=_tmpA2B;});_tmp582;});_tmp583->hd=_tmpA2D;}),({struct Cyc_List_List*_tmpA2A=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp583->tl=_tmpA2A;});_tmp583;}));
goto _LL0;case 268U: _LL213: _LL214:
# 2352 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[0]).v)));
goto _LL0;case 269U: _LL215: _LL216:
# 2353 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp586=_cycalloc(sizeof(*_tmp586));({void*_tmpA30=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->tag=1U,({struct _fat_ptr*_tmpA2F=({struct _fat_ptr*_tmp584=_cycalloc(sizeof(*_tmp584));({struct _fat_ptr _tmpA2E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp584=_tmpA2E;});_tmp584;});_tmp585->f1=_tmpA2F;});_tmp585;});_tmp586->hd=_tmpA30;}),_tmp586->tl=0;_tmp586;}));
goto _LL0;case 270U: _LL217: _LL218:
# 2358 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));({void*_tmpA31=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp587->hd=_tmpA31;}),_tmp587->tl=0;_tmp587;}));
goto _LL0;case 271U: _LL219: _LL21A:
# 2359 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp588=_cycalloc(sizeof(*_tmp588));({void*_tmpA33=Cyc_yyget_YY42(&(yyyvsp[1]).v);_tmp588->hd=_tmpA33;}),({struct Cyc_List_List*_tmpA32=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp588->tl=_tmpA32;});_tmp588;}));
goto _LL0;case 272U: _LL21B: _LL21C:
# 2363 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->tag=0U,({struct Cyc_Absyn_Exp*_tmpA34=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp589->f1=_tmpA34;});_tmp589;}));
goto _LL0;case 273U: _LL21D: _LL21E:
# 2364 "parse.y"
 yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->tag=1U,({struct _fat_ptr*_tmpA36=({struct _fat_ptr*_tmp58A=_cycalloc(sizeof(*_tmp58A));({struct _fat_ptr _tmpA35=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp58A=_tmpA35;});_tmp58A;});_tmp58B->f1=_tmpA36;});_tmp58B;}));
goto _LL0;case 274U: _LL21F: _LL220: {
# 2369 "parse.y"
struct _tuple26 _tmp58C=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp27=_tmp58C;struct _tuple26 _tmp58D=_stmttmp27;struct Cyc_List_List*_tmp590;struct Cyc_Parse_Type_specifier _tmp58F;struct Cyc_Absyn_Tqual _tmp58E;_LL48F: _tmp58E=_tmp58D.f1;_tmp58F=_tmp58D.f2;_tmp590=_tmp58D.f3;_LL490: {struct Cyc_Absyn_Tqual tq=_tmp58E;struct Cyc_Parse_Type_specifier tss=_tmp58F;struct Cyc_List_List*atts=_tmp590;
void*_tmp591=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp591;
if(atts != 0)
({void*_tmp592=0U;({unsigned _tmpA38=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA37=({const char*_tmp593="ignoring attributes in type";_tag_fat(_tmp593,sizeof(char),28U);});Cyc_Warn_warn(_tmpA38,_tmpA37,_tag_fat(_tmp592,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->f1=0,_tmp594->f2=tq,_tmp594->f3=t;_tmp594;}));
# 2375
goto _LL0;}}case 275U: _LL221: _LL222: {
# 2376 "parse.y"
struct _tuple26 _tmp595=Cyc_yyget_YY35(&(yyyvsp[0]).v);struct _tuple26 _stmttmp28=_tmp595;struct _tuple26 _tmp596=_stmttmp28;struct Cyc_List_List*_tmp599;struct Cyc_Parse_Type_specifier _tmp598;struct Cyc_Absyn_Tqual _tmp597;_LL492: _tmp597=_tmp596.f1;_tmp598=_tmp596.f2;_tmp599=_tmp596.f3;_LL493: {struct Cyc_Absyn_Tqual tq=_tmp597;struct Cyc_Parse_Type_specifier tss=_tmp598;struct Cyc_List_List*atts=_tmp599;
void*_tmp59A=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp59A;
struct Cyc_List_List*_tmp59B=(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp59B;
struct _tuple14 _tmp59C=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp59C;
if(t_info.f3 != 0)
# 2382
({void*_tmp59D=0U;({unsigned _tmpA3A=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA39=({const char*_tmp59E="bad type params, ignoring";_tag_fat(_tmp59E,sizeof(char),26U);});Cyc_Warn_warn(_tmpA3A,_tmpA39,_tag_fat(_tmp59D,sizeof(void*),0U));});});
if(t_info.f4 != 0)
({void*_tmp59F=0U;({unsigned _tmpA3C=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA3B=({const char*_tmp5A0="bad specifiers, ignoring";_tag_fat(_tmp5A0,sizeof(char),25U);});Cyc_Warn_warn(_tmpA3C,_tmpA3B,_tag_fat(_tmp59F,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));_tmp5A1->f1=0,_tmp5A1->f2=t_info.f1,_tmp5A1->f3=t_info.f2;_tmp5A1;}));
# 2387
goto _LL0;}}case 276U: _LL223: _LL224:
# 2390 "parse.y"
 yyval=Cyc_YY44(({struct _tuple8*_tmpA3D=Cyc_yyget_YY37(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmpA3D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 277U: _LL225: _LL226:
# 2391 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;case 278U: _LL227: _LL228:
# 2392 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(&(yyyvsp[1]).v)));
goto _LL0;case 279U: _LL229: _LL22A:
# 2393 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(&(yyyvsp[2]).v)));
goto _LL0;case 280U: _LL22B: _LL22C:
# 2394 "parse.y"
 yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));({void*_tmpA3F=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A2->hd=_tmpA3F;}),({struct Cyc_List_List*_tmpA3E=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp5A2->tl=_tmpA3E;});_tmp5A2;})));
goto _LL0;case 281U: _LL22D: _LL22E:
# 2400 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));({void*_tmpA40=Cyc_yyget_YY44(&(yyyvsp[0]).v);_tmp5A3->hd=_tmpA40;}),_tmp5A3->tl=0;_tmp5A3;}));
goto _LL0;case 282U: _LL22F: _LL230:
# 2401 "parse.y"
 yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));({void*_tmpA42=Cyc_yyget_YY44(&(yyyvsp[2]).v);_tmp5A4->hd=_tmpA42;}),({struct Cyc_List_List*_tmpA41=Cyc_yyget_YY40(&(yyyvsp[0]).v);_tmp5A4->tl=_tmpA41;});_tmp5A4;}));
goto _LL0;case 283U: _LL231: _LL232:
# 2406 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp758;({struct Cyc_List_List*_tmpA43=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp758.tms=_tmpA43;});_tmp758;}));
goto _LL0;case 284U: _LL233: _LL234:
# 2408 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 285U: _LL235: _LL236:
# 2410 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp759;({struct Cyc_List_List*_tmpA45=({struct Cyc_List_List*_tmpA44=Cyc_yyget_YY26(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA44,(Cyc_yyget_YY30(&(yyyvsp[1]).v)).tms);});_tmp759.tms=_tmpA45;});_tmp759;}));
goto _LL0;case 286U: _LL237: _LL238:
# 2415 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 287U: _LL239: _LL23A:
# 2417 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75A;({struct Cyc_List_List*_tmpA48=({struct Cyc_List_List*_tmp5A6=_region_malloc(yyr,sizeof(*_tmp5A6));({void*_tmpA47=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A5=_region_malloc(yyr,sizeof(*_tmp5A5));_tmp5A5->tag=0U,({void*_tmpA46=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp5A5->f1=_tmpA46;}),_tmp5A5->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp5A5;});_tmp5A6->hd=_tmpA47;}),_tmp5A6->tl=0;_tmp5A6;});_tmp75A.tms=_tmpA48;});_tmp75A;}));
goto _LL0;case 288U: _LL23B: _LL23C:
# 2419 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75B;({struct Cyc_List_List*_tmpA4C=({struct Cyc_List_List*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));({void*_tmpA4B=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));_tmp5A7->tag=0U,({void*_tmpA4A=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A7->f1=_tmpA4A;}),_tmp5A7->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A7;});_tmp5A8->hd=_tmpA4B;}),({struct Cyc_List_List*_tmpA49=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5A8->tl=_tmpA49;});_tmp5A8;});_tmp75B.tms=_tmpA4C;});_tmp75B;}));
goto _LL0;case 289U: _LL23D: _LL23E:
# 2421 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75C;({struct Cyc_List_List*_tmpA50=({struct Cyc_List_List*_tmp5AA=_region_malloc(yyr,sizeof(*_tmp5AA));({void*_tmpA4F=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5A9=_region_malloc(yyr,sizeof(*_tmp5A9));_tmp5A9->tag=1U,({struct Cyc_Absyn_Exp*_tmpA4E=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp5A9->f1=_tmpA4E;}),({void*_tmpA4D=Cyc_yyget_YY51(&(yyyvsp[3]).v);_tmp5A9->f2=_tmpA4D;}),_tmp5A9->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp5A9;});_tmp5AA->hd=_tmpA4F;}),_tmp5AA->tl=0;_tmp5AA;});_tmp75C.tms=_tmpA50;});_tmp75C;}));
goto _LL0;case 290U: _LL23F: _LL240:
# 2423 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75D;({struct Cyc_List_List*_tmpA55=({struct Cyc_List_List*_tmp5AC=_region_malloc(yyr,sizeof(*_tmp5AC));({void*_tmpA54=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5AB=_region_malloc(yyr,sizeof(*_tmp5AB));_tmp5AB->tag=1U,({struct Cyc_Absyn_Exp*_tmpA53=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5AB->f1=_tmpA53;}),({void*_tmpA52=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp5AB->f2=_tmpA52;}),_tmp5AB->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp5AB;});_tmp5AC->hd=_tmpA54;}),({
struct Cyc_List_List*_tmpA51=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5AC->tl=_tmpA51;});_tmp5AC;});
# 2423
_tmp75D.tms=_tmpA55;});_tmp75D;}));
# 2426
goto _LL0;case 291U: _LL241: _LL242:
# 2427 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75E;({struct Cyc_List_List*_tmpA5C=({struct Cyc_List_List*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));({void*_tmpA5B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));_tmp5AE->tag=3U,({void*_tmpA5A=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5AD=_region_malloc(yyr,sizeof(*_tmp5AD));_tmp5AD->tag=1U,_tmp5AD->f1=0,_tmp5AD->f2=0,_tmp5AD->f3=0,({void*_tmpA59=Cyc_yyget_YY49(&(yyyvsp[1]).v);_tmp5AD->f4=_tmpA59;}),({struct Cyc_List_List*_tmpA58=Cyc_yyget_YY50(&(yyyvsp[2]).v);_tmp5AD->f5=_tmpA58;}),({struct Cyc_Absyn_Exp*_tmpA57=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5AD->f6=_tmpA57;}),({struct Cyc_Absyn_Exp*_tmpA56=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5AD->f7=_tmpA56;});_tmp5AD;});_tmp5AE->f1=_tmpA5A;});_tmp5AE;});_tmp5AF->hd=_tmpA5B;}),_tmp5AF->tl=0;_tmp5AF;});_tmp75E.tms=_tmpA5C;});_tmp75E;}));
# 2429
goto _LL0;case 292U: _LL243: _LL244: {
# 2430 "parse.y"
struct _tuple27*_tmp5B0=Cyc_yyget_YY39(&(yyyvsp[1]).v);struct _tuple27*_stmttmp29=_tmp5B0;struct _tuple27*_tmp5B1=_stmttmp29;struct Cyc_List_List*_tmp5B6;void*_tmp5B5;struct Cyc_Absyn_VarargInfo*_tmp5B4;int _tmp5B3;struct Cyc_List_List*_tmp5B2;_LL495: _tmp5B2=_tmp5B1->f1;_tmp5B3=_tmp5B1->f2;_tmp5B4=_tmp5B1->f3;_tmp5B5=_tmp5B1->f4;_tmp5B6=_tmp5B1->f5;_LL496: {struct Cyc_List_List*lis=_tmp5B2;int b=_tmp5B3;struct Cyc_Absyn_VarargInfo*c=_tmp5B4;void*eff=_tmp5B5;struct Cyc_List_List*po=_tmp5B6;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75F;({struct Cyc_List_List*_tmpA61=({struct Cyc_List_List*_tmp5B9=_region_malloc(yyr,sizeof(*_tmp5B9));({void*_tmpA60=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B8=_region_malloc(yyr,sizeof(*_tmp5B8));_tmp5B8->tag=3U,({void*_tmpA5F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B7=_region_malloc(yyr,sizeof(*_tmp5B7));_tmp5B7->tag=1U,_tmp5B7->f1=lis,_tmp5B7->f2=b,_tmp5B7->f3=c,_tmp5B7->f4=eff,_tmp5B7->f5=po,({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_yyget_YY57(&(yyyvsp[3]).v);_tmp5B7->f6=_tmpA5E;}),({struct Cyc_Absyn_Exp*_tmpA5D=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5B7->f7=_tmpA5D;});_tmp5B7;});_tmp5B8->f1=_tmpA5F;});_tmp5B8;});_tmp5B9->hd=_tmpA60;}),_tmp5B9->tl=0;_tmp5B9;});_tmp75F.tms=_tmpA61;});_tmp75F;}));
# 2433
goto _LL0;}}case 293U: _LL245: _LL246:
# 2434 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp760;({struct Cyc_List_List*_tmpA69=({struct Cyc_List_List*_tmp5BC=_region_malloc(yyr,sizeof(*_tmp5BC));({void*_tmpA68=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5BB=_region_malloc(yyr,sizeof(*_tmp5BB));_tmp5BB->tag=3U,({void*_tmpA67=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5BA=_region_malloc(yyr,sizeof(*_tmp5BA));_tmp5BA->tag=1U,_tmp5BA->f1=0,_tmp5BA->f2=0,_tmp5BA->f3=0,({void*_tmpA66=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp5BA->f4=_tmpA66;}),({struct Cyc_List_List*_tmpA65=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp5BA->f5=_tmpA65;}),({struct Cyc_Absyn_Exp*_tmpA64=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5BA->f6=_tmpA64;}),({struct Cyc_Absyn_Exp*_tmpA63=Cyc_yyget_YY57(&(yyyvsp[6]).v);_tmp5BA->f7=_tmpA63;});_tmp5BA;});_tmp5BB->f1=_tmpA67;});_tmp5BB;});_tmp5BC->hd=_tmpA68;}),({
struct Cyc_List_List*_tmpA62=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5BC->tl=_tmpA62;});_tmp5BC;});
# 2434
_tmp760.tms=_tmpA69;});_tmp760;}));
# 2437
goto _LL0;case 294U: _LL247: _LL248: {
# 2438 "parse.y"
struct _tuple27*_tmp5BD=Cyc_yyget_YY39(&(yyyvsp[2]).v);struct _tuple27*_stmttmp2A=_tmp5BD;struct _tuple27*_tmp5BE=_stmttmp2A;struct Cyc_List_List*_tmp5C3;void*_tmp5C2;struct Cyc_Absyn_VarargInfo*_tmp5C1;int _tmp5C0;struct Cyc_List_List*_tmp5BF;_LL498: _tmp5BF=_tmp5BE->f1;_tmp5C0=_tmp5BE->f2;_tmp5C1=_tmp5BE->f3;_tmp5C2=_tmp5BE->f4;_tmp5C3=_tmp5BE->f5;_LL499: {struct Cyc_List_List*lis=_tmp5BF;int b=_tmp5C0;struct Cyc_Absyn_VarargInfo*c=_tmp5C1;void*eff=_tmp5C2;struct Cyc_List_List*po=_tmp5C3;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp761;({struct Cyc_List_List*_tmpA6F=({struct Cyc_List_List*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));({void*_tmpA6E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));_tmp5C5->tag=3U,({void*_tmpA6D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4->tag=1U,_tmp5C4->f1=lis,_tmp5C4->f2=b,_tmp5C4->f3=c,_tmp5C4->f4=eff,_tmp5C4->f5=po,({
struct Cyc_Absyn_Exp*_tmpA6C=Cyc_yyget_YY57(&(yyyvsp[4]).v);_tmp5C4->f6=_tmpA6C;}),({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_yyget_YY57(&(yyyvsp[5]).v);_tmp5C4->f7=_tmpA6B;});_tmp5C4;});
# 2439
_tmp5C5->f1=_tmpA6D;});_tmp5C5;});_tmp5C6->hd=_tmpA6E;}),({
struct Cyc_List_List*_tmpA6A=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C6->tl=_tmpA6A;});_tmp5C6;});
# 2439
_tmp761.tms=_tmpA6F;});_tmp761;}));
# 2442
goto _LL0;}}case 295U: _LL249: _LL24A: {
# 2444
struct Cyc_List_List*_tmp5C7=({unsigned _tmpA70=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA70,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp5C7;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp762;({struct Cyc_List_List*_tmpA73=({struct Cyc_List_List*_tmp5C9=_region_malloc(yyr,sizeof(*_tmp5C9));({void*_tmpA72=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5C8=_region_malloc(yyr,sizeof(*_tmp5C8));_tmp5C8->tag=4U,_tmp5C8->f1=ts,_tmp5C8->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp5C8->f3=0;_tmp5C8;});_tmp5C9->hd=_tmpA72;}),({
struct Cyc_List_List*_tmpA71=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5C9->tl=_tmpA71;});_tmp5C9;});
# 2445
_tmp762.tms=_tmpA73;});_tmp762;}));
# 2448
goto _LL0;}case 296U: _LL24B: _LL24C:
# 2449 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp763;({struct Cyc_List_List*_tmpA77=({struct Cyc_List_List*_tmp5CB=_region_malloc(yyr,sizeof(*_tmp5CB));({void*_tmpA76=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));_tmp5CA->tag=5U,_tmp5CA->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmpA75=Cyc_yyget_YY45(&(yyyvsp[1]).v);_tmp5CA->f2=_tmpA75;});_tmp5CA;});_tmp5CB->hd=_tmpA76;}),({struct Cyc_List_List*_tmpA74=(Cyc_yyget_YY30(&(yyyvsp[0]).v)).tms;_tmp5CB->tl=_tmpA74;});_tmp5CB;});_tmp763.tms=_tmpA77;});_tmp763;}));
# 2451
goto _LL0;case 297U: _LL24D: _LL24E:
# 2455 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 298U: _LL24F: _LL250:
# 2456 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299U: _LL251: _LL252:
# 2457 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300U: _LL253: _LL254:
# 2458 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301U: _LL255: _LL256:
# 2459 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 302U: _LL257: _LL258:
# 2460 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 303U: _LL259: _LL25A:
# 2466 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA7B=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->tag=13U,({struct _fat_ptr*_tmpA7A=({struct _fat_ptr*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));({struct _fat_ptr _tmpA79=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CC=_tmpA79;});_tmp5CC;});_tmp5CD->f1=_tmpA7A;}),({struct Cyc_Absyn_Stmt*_tmpA78=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5CD->f2=_tmpA78;});_tmp5CD;});Cyc_Absyn_new_stmt(_tmpA7B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304U: _LL25B: _LL25C:
# 2470 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305U: _LL25D: _LL25E:
# 2471 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmpA7C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 306U: _LL25F: _LL260:
# 2476 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 307U: _LL261: _LL262:
# 2477 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 308U: _LL263: _LL264:
# 2482 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA7D=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA7D,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 309U: _LL265: _LL266:
# 2483 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA7E=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmpA7E,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 310U: _LL267: _LL268:
# 2484 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA83=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->tag=13U,({struct _fat_ptr*_tmpA82=({struct _fat_ptr*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));({struct _fat_ptr _tmpA81=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5CE=_tmpA81;});_tmp5CE;});_tmp5CF->f1=_tmpA82;}),({struct Cyc_Absyn_Stmt*_tmpA80=({struct Cyc_List_List*_tmpA7F=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA7F,Cyc_Absyn_skip_stmt(0U));});_tmp5CF->f2=_tmpA80;});_tmp5CF;});Cyc_Absyn_new_stmt(_tmpA83,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2486
goto _LL0;case 311U: _LL269: _LL26A:
# 2486 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmpA88=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->tag=13U,({struct _fat_ptr*_tmpA87=({struct _fat_ptr*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));({struct _fat_ptr _tmpA86=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5D0=_tmpA86;});_tmp5D0;});_tmp5D1->f1=_tmpA87;}),({struct Cyc_Absyn_Stmt*_tmpA85=({struct Cyc_List_List*_tmpA84=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA84,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp5D1->f2=_tmpA85;});_tmp5D1;});Cyc_Absyn_new_stmt(_tmpA88,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2488
goto _LL0;case 312U: _LL26B: _LL26C:
# 2488 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 313U: _LL26D: _LL26E:
# 2489 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA8A=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmpA89=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmpA8A,_tmpA89,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 314U: _LL26F: _LL270:
# 2490 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA8D=({void*_tmpA8C=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA8B=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D2->f1=_tmpA8B;});_tmp5D2;});Cyc_Absyn_new_decl(_tmpA8C,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA8D,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 315U: _LL271: _LL272:
# 2493 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA90=({void*_tmpA8F=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA8E=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp5D3->f1=_tmpA8E;});_tmp5D3;});Cyc_Absyn_new_decl(_tmpA8F,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmpA90,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 316U: _LL273: _LL274:
# 2498 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA93=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA92=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA91=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA93,_tmpA92,_tmpA91,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317U: _LL275: _LL276:
# 2500 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA96=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpA95=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpA94=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmpA96,_tmpA95,_tmpA94,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318U: _LL277: _LL278:
# 2506 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA98=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmpA97=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmpA98,_tmpA97,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319U: _LL279: _LL27A: {
# 2509
struct Cyc_Absyn_Exp*_tmp5D4=({struct _tuple0*_tmpA99=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmpA99,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D4;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9B=e;struct Cyc_List_List*_tmpA9A=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmpA9B,_tmpA9A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2513
struct Cyc_Absyn_Exp*_tmp5D5=({struct Cyc_List_List*_tmpA9C=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmpA9C,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_Absyn_Exp*e=_tmp5D5;
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9E=e;struct Cyc_List_List*_tmpA9D=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmpA9E,_tmpA9D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2516
goto _LL0;}case 321U: _LL27D: _LL27E:
# 2519 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpAA0=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmpA9F=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmpAA0,_tmpA9F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 322U: _LL27F: _LL280:
# 2533 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 323U: _LL281: _LL282:
# 2537 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));({struct Cyc_Absyn_Switch_clause*_tmpAA4=({struct Cyc_Absyn_Switch_clause*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));({struct Cyc_Absyn_Pat*_tmpAA3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp5D6->pattern=_tmpAA3;}),_tmp5D6->pat_vars=0,_tmp5D6->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA2=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp5D6->body=_tmpAA2;}),_tmp5D6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D6;});
# 2537
_tmp5D7->hd=_tmpAA4;}),({
# 2539
struct Cyc_List_List*_tmpAA1=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D7->tl=_tmpAA1;});_tmp5D7;}));
goto _LL0;case 324U: _LL283: _LL284:
# 2541 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct Cyc_Absyn_Switch_clause*_tmpAA8=({struct Cyc_Absyn_Switch_clause*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));({struct Cyc_Absyn_Pat*_tmpAA7=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5D8->pattern=_tmpAA7;}),_tmp5D8->pat_vars=0,_tmp5D8->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpAA6=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp5D8->body=_tmpAA6;}),_tmp5D8->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5D8;});
# 2541
_tmp5D9->hd=_tmpAA8;}),({
# 2543
struct Cyc_List_List*_tmpAA5=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp5D9->tl=_tmpAA5;});_tmp5D9;}));
goto _LL0;case 325U: _LL285: _LL286:
# 2545 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));({struct Cyc_Absyn_Switch_clause*_tmpAAC=({struct Cyc_Absyn_Switch_clause*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));({struct Cyc_Absyn_Pat*_tmpAAB=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DA->pattern=_tmpAAB;}),_tmp5DA->pat_vars=0,_tmp5DA->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpAAA=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp5DA->body=_tmpAAA;}),_tmp5DA->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DA;});_tmp5DB->hd=_tmpAAC;}),({struct Cyc_List_List*_tmpAA9=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp5DB->tl=_tmpAA9;});_tmp5DB;}));
goto _LL0;case 326U: _LL287: _LL288:
# 2547 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));({struct Cyc_Absyn_Switch_clause*_tmpAB1=({struct Cyc_Absyn_Switch_clause*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct Cyc_Absyn_Pat*_tmpAB0=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DC->pattern=_tmpAB0;}),_tmp5DC->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAAF=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5DC->where_clause=_tmpAAF;}),({
struct Cyc_Absyn_Stmt*_tmpAAE=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp5DC->body=_tmpAAE;}),_tmp5DC->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DC;});
# 2547
_tmp5DD->hd=_tmpAB1;}),({
# 2549
struct Cyc_List_List*_tmpAAD=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp5DD->tl=_tmpAAD;});_tmp5DD;}));
goto _LL0;case 327U: _LL289: _LL28A:
# 2551 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));({struct Cyc_Absyn_Switch_clause*_tmpAB6=({struct Cyc_Absyn_Switch_clause*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));({struct Cyc_Absyn_Pat*_tmpAB5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5DE->pattern=_tmpAB5;}),_tmp5DE->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpAB4=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp5DE->where_clause=_tmpAB4;}),({struct Cyc_Absyn_Stmt*_tmpAB3=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp5DE->body=_tmpAB3;}),_tmp5DE->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp5DE;});_tmp5DF->hd=_tmpAB6;}),({struct Cyc_List_List*_tmpAB2=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp5DF->tl=_tmpAB2;});_tmp5DF;}));
goto _LL0;case 328U: _LL28B: _LL28C:
# 2558 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAB8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmpAB7=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmpAB8,_tmpAB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329U: _LL28D: _LL28E:
# 2562 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpABA=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAB9=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmpABA,_tmpAB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 330U: _LL28F: _LL290:
# 2566 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpABE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpABD=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpABC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpABB=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2566
Cyc_Absyn_for_stmt(_tmpABE,_tmpABD,_tmpABC,_tmpABB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2568
goto _LL0;case 331U: _LL291: _LL292:
# 2569 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC1=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAC0=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpABF=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2569
Cyc_Absyn_for_stmt(_tmpAC2,_tmpAC1,_tmpAC0,_tmpABF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2571
goto _LL0;case 332U: _LL293: _LL294:
# 2572 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC5=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAC4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAC3=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2572
Cyc_Absyn_for_stmt(_tmpAC6,_tmpAC5,_tmpAC4,_tmpAC3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2574
goto _LL0;case 333U: _LL295: _LL296:
# 2575 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpACA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAC9=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAC8=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAC7=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2575
Cyc_Absyn_for_stmt(_tmpACA,_tmpAC9,_tmpAC8,_tmpAC7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2577
goto _LL0;case 334U: _LL297: _LL298:
# 2578 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpACE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpACD=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpACC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpACB=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2578
Cyc_Absyn_for_stmt(_tmpACE,_tmpACD,_tmpACC,_tmpACB,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2580
goto _LL0;case 335U: _LL299: _LL29A:
# 2581 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD1=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAD0=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpACF=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2581
Cyc_Absyn_for_stmt(_tmpAD2,_tmpAD1,_tmpAD0,_tmpACF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2583
goto _LL0;case 336U: _LL29B: _LL29C:
# 2584 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAD6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD5=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAD4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAD3=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2584
Cyc_Absyn_for_stmt(_tmpAD6,_tmpAD5,_tmpAD4,_tmpAD3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2586
goto _LL0;case 337U: _LL29D: _LL29E:
# 2587 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpADA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAD9=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmpAD7=
Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);
# 2587
Cyc_Absyn_for_stmt(_tmpADA,_tmpAD9,_tmpAD8,_tmpAD7,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2589
goto _LL0;case 338U: _LL29F: _LL2A0: {
# 2590 "parse.y"
struct Cyc_List_List*_tmp5E0=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5E0;
struct Cyc_Absyn_Stmt*_tmp5E1=({struct Cyc_Absyn_Exp*_tmpADE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpADD=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpADC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADB=
Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);
# 2591
Cyc_Absyn_for_stmt(_tmpADE,_tmpADD,_tmpADC,_tmpADB,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E1;
# 2593
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2595
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2596 "parse.y"
struct Cyc_List_List*_tmp5E2=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5E2;
struct Cyc_Absyn_Stmt*_tmp5E3=({struct Cyc_Absyn_Exp*_tmpAE2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE1=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAE0=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpADF=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2597
Cyc_Absyn_for_stmt(_tmpAE2,_tmpAE1,_tmpAE0,_tmpADF,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E3;
# 2599
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2601
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2602 "parse.y"
struct Cyc_List_List*_tmp5E4=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5E4;
struct Cyc_Absyn_Stmt*_tmp5E5=({struct Cyc_Absyn_Exp*_tmpAE6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE5=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAE4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmpAE3=
Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);
# 2603
Cyc_Absyn_for_stmt(_tmpAE6,_tmpAE5,_tmpAE4,_tmpAE3,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E5;
# 2605
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2607
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2608 "parse.y"
struct Cyc_List_List*_tmp5E6=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_List_List*decls=_tmp5E6;
struct Cyc_Absyn_Stmt*_tmp5E7=({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAE9=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpAE8=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmpAE7=
Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);
# 2609
Cyc_Absyn_for_stmt(_tmpAEA,_tmpAE9,_tmpAE8,_tmpAE7,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp5E7;
# 2611
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(decls,s));
# 2613
goto _LL0;}case 342U: _LL2A7: _LL2A8:
# 2618 "parse.y"
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpAEC=({struct _fat_ptr*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));({struct _fat_ptr _tmpAEB=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5E8=_tmpAEB;});_tmp5E8;});Cyc_Absyn_goto_stmt(_tmpAEC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 343U: _LL2A9: _LL2AA:
# 2619 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 344U: _LL2AB: _LL2AC:
# 2620 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 345U: _LL2AD: _LL2AE:
# 2621 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 346U: _LL2AF: _LL2B0:
# 2622 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAED=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpAED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 347U: _LL2B1: _LL2B2:
# 2624 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 348U: _LL2B3: _LL2B4:
# 2625 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 349U: _LL2B5: _LL2B6:
# 2627 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAEE=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpAEE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 350U: _LL2B7: _LL2B8:
# 2636 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 351U: _LL2B9: _LL2BA:
# 2639
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352U: _LL2BB: _LL2BC:
# 2641 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAEF=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpAF1,_tmpAF0,_tmpAEF,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353U: _LL2BD: _LL2BE:
# 2644
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354U: _LL2BF: _LL2C0:
# 2646 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpAF3,_tmpAF2,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355U: _LL2C1: _LL2C2:
# 2649
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356U: _LL2C3: _LL2C4:
# 2651 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF5=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpAF5,_tmpAF4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357U: _LL2C5: _LL2C6:
# 2654
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358U: _LL2C7: _LL2C8:
# 2656 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpAF7,_tmpAF6,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359U: _LL2C9: _LL2CA:
# 2659
 yyval=(yyyvsp[0]).v;
goto _LL0;case 360U: _LL2CB: _LL2CC:
# 2661 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAF8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpAF9,_tmpAF8,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 361U: _LL2CD: _LL2CE:
# 2664
 yyval=(yyyvsp[0]).v;
goto _LL0;case 362U: _LL2CF: _LL2D0:
# 2666 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpAFB,_tmpAFA,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 363U: _LL2D1: _LL2D2:
# 2669
 yyval=(yyyvsp[0]).v;
goto _LL0;case 364U: _LL2D3: _LL2D4:
# 2671 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpAFD,_tmpAFC,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 365U: _LL2D5: _LL2D6:
# 2673 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAFF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpAFF,_tmpAFE,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 366U: _LL2D7: _LL2D8:
# 2676
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367U: _LL2D9: _LL2DA:
# 2678 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB01=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB00=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpB01,_tmpB00,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368U: _LL2DB: _LL2DC:
# 2680 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB03=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB02=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpB03,_tmpB02,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 369U: _LL2DD: _LL2DE:
# 2682 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB04=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpB05,_tmpB04,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370U: _LL2DF: _LL2E0:
# 2684 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB07=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB06=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpB07,_tmpB06,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371U: _LL2E1: _LL2E2:
# 2687
 yyval=(yyyvsp[0]).v;
goto _LL0;case 372U: _LL2E3: _LL2E4:
# 2689 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB09=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB08=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB09,_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 373U: _LL2E5: _LL2E6:
# 2691 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB0B,_tmpB0A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 374U: _LL2E7: _LL2E8:
# 2694
 yyval=(yyyvsp[0]).v;
goto _LL0;case 375U: _LL2E9: _LL2EA:
# 2696 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB0D,_tmpB0C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 376U: _LL2EB: _LL2EC:
# 2698 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB0E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB0F,_tmpB0E,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 377U: _LL2ED: _LL2EE:
# 2701
 yyval=(yyyvsp[0]).v;
goto _LL0;case 378U: _LL2EF: _LL2F0:
# 2703 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB10=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB11,_tmpB10,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 379U: _LL2F1: _LL2F2:
# 2705 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB13=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB12=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB13,_tmpB12,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 380U: _LL2F3: _LL2F4:
# 2707 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB15=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpB14=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB15,_tmpB14,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 381U: _LL2F5: _LL2F6:
# 2710
 yyval=(yyyvsp[0]).v;
goto _LL0;case 382U: _LL2F7: _LL2F8: {
# 2712 "parse.y"
void*_tmp5E9=({struct _tuple8*_tmpB16=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpB16,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5E9;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB18=t;struct Cyc_Absyn_Exp*_tmpB17=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpB18,_tmpB17,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2715
goto _LL0;}case 383U: _LL2F9: _LL2FA:
# 2718 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 384U: _LL2FB: _LL2FC:
# 2721
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 385U: _LL2FD: _LL2FE:
# 2723 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpB1A=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB19=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpB1A,_tmpB19,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 386U: _LL2FF: _LL300: {
# 2725 "parse.y"
void*_tmp5EA=({struct _tuple8*_tmpB1B=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpB1B,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5EA;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
# 2728
goto _LL0;}case 387U: _LL301: _LL302:
# 2729 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpB1C=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpB1C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 388U: _LL303: _LL304:
# 2731 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpB1E=(*Cyc_yyget_YY37(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpB1D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpB1E,_tmpB1D,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 389U: _LL305: _LL306:
# 2736 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 390U: _LL307: _LL308:
# 2744 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 391U: _LL309: _LL30A:
# 2749 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 392U: _LL30B: _LL30C:
# 2751 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 393U: _LL30D: _LL30E: {
# 2753 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp5EB=e->r;void*_stmttmp2B=_tmp5EB;void*_tmp5EC=_stmttmp2B;int _tmp5EE;struct _fat_ptr _tmp5ED;int _tmp5F0;enum Cyc_Absyn_Sign _tmp5EF;short _tmp5F2;enum Cyc_Absyn_Sign _tmp5F1;char _tmp5F4;enum Cyc_Absyn_Sign _tmp5F3;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).LongLong_c).tag){case 2U: _LL49B: _tmp5F3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Char_c).val).f1;_tmp5F4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Char_c).val).f2;_LL49C: {enum Cyc_Absyn_Sign s=_tmp5F3;char i=_tmp5F4;
# 2757
yyval=Cyc_YY9(({void*_tmpB1F=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=11U,_tmp5F5->f1=i;_tmp5F5;});Cyc_Absyn_new_pat(_tmpB1F,e->loc);}));goto _LL49A;}case 4U: _LL49D: _tmp5F1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Short_c).val).f1;_tmp5F2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Short_c).val).f2;_LL49E: {enum Cyc_Absyn_Sign s=_tmp5F1;short i=_tmp5F2;
# 2759
yyval=Cyc_YY9(({void*_tmpB20=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=10U,_tmp5F6->f1=s,_tmp5F6->f2=(int)i;_tmp5F6;});Cyc_Absyn_new_pat(_tmpB20,e->loc);}));goto _LL49A;}case 5U: _LL49F: _tmp5EF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Int_c).val).f1;_tmp5F0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Int_c).val).f2;_LL4A0: {enum Cyc_Absyn_Sign s=_tmp5EF;int i=_tmp5F0;
# 2761
yyval=Cyc_YY9(({void*_tmpB21=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));_tmp5F7->tag=10U,_tmp5F7->f1=s,_tmp5F7->f2=i;_tmp5F7;});Cyc_Absyn_new_pat(_tmpB21,e->loc);}));goto _LL49A;}case 7U: _LL4A1: _tmp5ED=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Float_c).val).f1;_tmp5EE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5EC)->f1).Float_c).val).f2;_LL4A2: {struct _fat_ptr s=_tmp5ED;int i=_tmp5EE;
# 2763
yyval=Cyc_YY9(({void*_tmpB22=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));_tmp5F8->tag=12U,_tmp5F8->f1=s,_tmp5F8->f2=i;_tmp5F8;});Cyc_Absyn_new_pat(_tmpB22,e->loc);}));goto _LL49A;}case 1U: _LL4A3: _LL4A4:
# 2765
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL49A;case 8U: _LL4A5: _LL4A6:
# 2767
({void*_tmp5F9=0U;({unsigned _tmpB24=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB23=({const char*_tmp5FA="strings cannot occur within patterns";_tag_fat(_tmp5FA,sizeof(char),37U);});Cyc_Warn_err(_tmpB24,_tmpB23,_tag_fat(_tmp5F9,sizeof(void*),0U));});});goto _LL49A;case 9U: _LL4A7: _LL4A8:
# 2769
({void*_tmp5FB=0U;({unsigned _tmpB26=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB25=({const char*_tmp5FC="strings cannot occur within patterns";_tag_fat(_tmp5FC,sizeof(char),37U);});Cyc_Warn_err(_tmpB26,_tmpB25,_tag_fat(_tmp5FB,sizeof(void*),0U));});});goto _LL49A;case 6U: _LL4A9: _LL4AA:
# 2771
({void*_tmp5FD=0U;({unsigned _tmpB28=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB27=({const char*_tmp5FE="long long's in patterns not yet implemented";_tag_fat(_tmp5FE,sizeof(char),44U);});Cyc_Warn_err(_tmpB28,_tmpB27,_tag_fat(_tmp5FD,sizeof(void*),0U));});});goto _LL49A;default: goto _LL4AB;}else{_LL4AB: _LL4AC:
# 2773
({void*_tmp5FF=0U;({unsigned _tmpB2A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpB29=({const char*_tmp600="bad constant in case";_tag_fat(_tmp600,sizeof(char),21U);});Cyc_Warn_err(_tmpB2A,_tmpB29,_tag_fat(_tmp5FF,sizeof(void*),0U));});});}_LL49A:;}
# 2776
goto _LL0;}case 394U: _LL30F: _LL310:
# 2777 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB2C=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->tag=15U,({struct _tuple0*_tmpB2B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp601->f1=_tmpB2B;});_tmp601;});Cyc_Absyn_new_pat(_tmpB2C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 395U: _LL311: _LL312:
# 2779 "parse.y"
 if(({struct _fat_ptr _tmpB2D=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpB2D,({const char*_tmp602="as";_tag_fat(_tmp602,sizeof(char),3U);}));})!= 0)
({void*_tmp603=0U;({unsigned _tmpB2F=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB2E=({const char*_tmp604="expecting `as'";_tag_fat(_tmp604,sizeof(char),15U);});Cyc_Warn_err(_tmpB2F,_tmpB2E,_tag_fat(_tmp603,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB36=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB35=({unsigned _tmpB34=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB33=({struct _tuple0*_tmp606=_cycalloc(sizeof(*_tmp606));_tmp606->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB32=({struct _fat_ptr*_tmp605=_cycalloc(sizeof(*_tmp605));({struct _fat_ptr _tmpB31=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp605=_tmpB31;});_tmp605;});_tmp606->f2=_tmpB32;});_tmp606;});Cyc_Absyn_new_vardecl(_tmpB34,_tmpB33,Cyc_Absyn_void_type,0);});_tmp607->f1=_tmpB35;}),({
struct Cyc_Absyn_Pat*_tmpB30=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp607->f2=_tmpB30;});_tmp607;});
# 2781
Cyc_Absyn_new_pat(_tmpB36,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2784
goto _LL0;case 396U: _LL313: _LL314:
# 2785 "parse.y"
 if(({struct _fat_ptr _tmpB37=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB37,({const char*_tmp608="alias";_tag_fat(_tmp608,sizeof(char),6U);}));})!= 0)
({void*_tmp609=0U;({unsigned _tmpB39=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB38=({const char*_tmp60A="expecting `alias'";_tag_fat(_tmp60A,sizeof(char),18U);});Cyc_Warn_err(_tmpB39,_tmpB38,_tag_fat(_tmp609,sizeof(void*),0U));});});{
int _tmp60B=((yyyvsp[0]).l).first_line;int location=_tmp60B;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp611=_cycalloc(sizeof(*_tmp611));({struct _fat_ptr*_tmpB3C=({struct _fat_ptr*_tmp60F=_cycalloc(sizeof(*_tmp60F));({struct _fat_ptr _tmpB3B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60F=_tmpB3B;});_tmp60F;});_tmp611->name=_tmpB3C;}),_tmp611->identity=- 1,({void*_tmpB3A=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=0U,_tmp610->f1=& Cyc_Tcutil_rk;_tmp610;});_tmp611->kind=_tmpB3A;});_tmp611;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB41=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB40=({struct _tuple0*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB3E=({struct _fat_ptr*_tmp60D=_cycalloc(sizeof(*_tmp60D));({struct _fat_ptr _tmpB3D=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp60D=_tmpB3D;});_tmp60D;});_tmp60E->f2=_tmpB3E;});_tmp60E;});Cyc_Absyn_new_vardecl(_tmpB41,_tmpB40,({
struct _tuple8*_tmpB3F=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB3F,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB42=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=2U,_tmp60C->f1=tv,_tmp60C->f2=vd;_tmp60C;});Cyc_Absyn_new_pat(_tmpB42,(unsigned)location);}));
# 2793
goto _LL0;}case 397U: _LL315: _LL316:
# 2794 "parse.y"
 if(({struct _fat_ptr _tmpB43=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpB43,({const char*_tmp612="alias";_tag_fat(_tmp612,sizeof(char),6U);}));})!= 0)
({void*_tmp613=0U;({unsigned _tmpB45=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpB44=({const char*_tmp614="expecting `alias'";_tag_fat(_tmp614,sizeof(char),18U);});Cyc_Warn_err(_tmpB45,_tmpB44,_tag_fat(_tmp613,sizeof(void*),0U));});});{
int _tmp615=((yyyvsp[0]).l).first_line;int location=_tmp615;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp61B=_cycalloc(sizeof(*_tmp61B));({struct _fat_ptr*_tmpB48=({struct _fat_ptr*_tmp619=_cycalloc(sizeof(*_tmp619));({struct _fat_ptr _tmpB47=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp619=_tmpB47;});_tmp619;});_tmp61B->name=_tmpB48;}),_tmp61B->identity=- 1,({void*_tmpB46=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=0U,_tmp61A->f1=& Cyc_Tcutil_rk;_tmp61A;});_tmp61B->kind=_tmpB46;});_tmp61B;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpB4D=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB4C=({struct _tuple0*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB4A=({struct _fat_ptr*_tmp617=_cycalloc(sizeof(*_tmp617));({struct _fat_ptr _tmpB49=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp617=_tmpB49;});_tmp617;});_tmp618->f2=_tmpB4A;});_tmp618;});Cyc_Absyn_new_vardecl(_tmpB4D,_tmpB4C,({
struct _tuple8*_tmpB4B=Cyc_yyget_YY37(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpB4B,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB4E=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=2U,_tmp616->f1=tv,_tmp616->f2=vd;_tmp616;});Cyc_Absyn_new_pat(_tmpB4E,(unsigned)location);}));
# 2802
goto _LL0;}case 398U: _LL317: _LL318: {
# 2803 "parse.y"
struct _tuple23 _tmp61C=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2C=_tmp61C;struct _tuple23 _tmp61D=_stmttmp2C;int _tmp61F;struct Cyc_List_List*_tmp61E;_LL4AE: _tmp61E=_tmp61D.f1;_tmp61F=_tmp61D.f2;_LL4AF: {struct Cyc_List_List*ps=_tmp61E;int dots=_tmp61F;
yyval=Cyc_YY9(({void*_tmpB4F=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620->tag=5U,_tmp620->f1=ps,_tmp620->f2=dots;_tmp620;});Cyc_Absyn_new_pat(_tmpB4F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2806
goto _LL0;}}case 399U: _LL319: _LL31A: {
# 2807 "parse.y"
struct _tuple23 _tmp621=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2D=_tmp621;struct _tuple23 _tmp622=_stmttmp2D;int _tmp624;struct Cyc_List_List*_tmp623;_LL4B1: _tmp623=_tmp622.f1;_tmp624=_tmp622.f2;_LL4B2: {struct Cyc_List_List*ps=_tmp623;int dots=_tmp624;
yyval=Cyc_YY9(({void*_tmpB51=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tag=16U,({struct _tuple0*_tmpB50=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp625->f1=_tmpB50;}),_tmp625->f2=ps,_tmp625->f3=dots;_tmp625;});Cyc_Absyn_new_pat(_tmpB51,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2810
goto _LL0;}}case 400U: _LL31B: _LL31C: {
# 2811 "parse.y"
struct _tuple23 _tmp626=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp2E=_tmp626;struct _tuple23 _tmp627=_stmttmp2E;int _tmp629;struct Cyc_List_List*_tmp628;_LL4B4: _tmp628=_tmp627.f1;_tmp629=_tmp627.f2;_LL4B5: {struct Cyc_List_List*fps=_tmp628;int dots=_tmp629;
struct Cyc_List_List*_tmp62A=({unsigned _tmpB52=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB52,Cyc_yyget_YY40(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp62A;
yyval=Cyc_YY9(({void*_tmpB55=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB54=({union Cyc_Absyn_AggrInfo*_tmp62B=_cycalloc(sizeof(*_tmp62B));({union Cyc_Absyn_AggrInfo _tmpB53=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp62B=_tmpB53;});_tmp62B;});_tmp62C->f1=_tmpB54;}),_tmp62C->f2=exist_ts,_tmp62C->f3=fps,_tmp62C->f4=dots;_tmp62C;});Cyc_Absyn_new_pat(_tmpB55,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2816
goto _LL0;}}case 401U: _LL31D: _LL31E: {
# 2817 "parse.y"
struct _tuple23 _tmp62D=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2F=_tmp62D;struct _tuple23 _tmp62E=_stmttmp2F;int _tmp630;struct Cyc_List_List*_tmp62F;_LL4B7: _tmp62F=_tmp62E.f1;_tmp630=_tmp62E.f2;_LL4B8: {struct Cyc_List_List*fps=_tmp62F;int dots=_tmp630;
struct Cyc_List_List*_tmp631=({unsigned _tmpB56=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned,void*),unsigned env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB56,Cyc_yyget_YY40(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp631;
yyval=Cyc_YY9(({void*_tmpB57=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=7U,_tmp632->f1=0,_tmp632->f2=exist_ts,_tmp632->f3=fps,_tmp632->f4=dots;_tmp632;});Cyc_Absyn_new_pat(_tmpB57,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2821
goto _LL0;}}case 402U: _LL31F: _LL320:
# 2822 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB59=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->tag=6U,({struct Cyc_Absyn_Pat*_tmpB58=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp633->f1=_tmpB58;});_tmp633;});Cyc_Absyn_new_pat(_tmpB59,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 403U: _LL321: _LL322:
# 2824 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB5D=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->tag=6U,({struct Cyc_Absyn_Pat*_tmpB5C=({void*_tmpB5B=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=6U,({struct Cyc_Absyn_Pat*_tmpB5A=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp634->f1=_tmpB5A;});_tmp634;});Cyc_Absyn_new_pat(_tmpB5B,(unsigned)((yyyvsp[0]).l).first_line);});_tmp635->f1=_tmpB5C;});_tmp635;});Cyc_Absyn_new_pat(_tmpB5D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404U: _LL323: _LL324:
# 2826 "parse.y"
 yyval=Cyc_YY9(({void*_tmpB64=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB63=({unsigned _tmpB62=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB61=({struct _tuple0*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB60=({struct _fat_ptr*_tmp636=_cycalloc(sizeof(*_tmp636));({struct _fat_ptr _tmpB5F=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp636=_tmpB5F;});_tmp636;});_tmp637->f2=_tmpB60;});_tmp637;});Cyc_Absyn_new_vardecl(_tmpB62,_tmpB61,Cyc_Absyn_void_type,0);});_tmp638->f1=_tmpB63;}),({
# 2828
struct Cyc_Absyn_Pat*_tmpB5E=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp638->f2=_tmpB5E;});_tmp638;});
# 2826
Cyc_Absyn_new_pat(_tmpB64,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2830
goto _LL0;case 405U: _LL325: _LL326:
# 2831 "parse.y"
 if(({struct _fat_ptr _tmpB65=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpB65,({const char*_tmp639="as";_tag_fat(_tmp639,sizeof(char),3U);}));})!= 0)
({void*_tmp63A=0U;({unsigned _tmpB67=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpB66=({const char*_tmp63B="expecting `as'";_tag_fat(_tmp63B,sizeof(char),15U);});Cyc_Warn_err(_tmpB67,_tmpB66,_tag_fat(_tmp63A,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB6E=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB6D=({unsigned _tmpB6C=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB6B=({struct _tuple0*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB6A=({struct _fat_ptr*_tmp63C=_cycalloc(sizeof(*_tmp63C));({struct _fat_ptr _tmpB69=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp63C=_tmpB69;});_tmp63C;});_tmp63D->f2=_tmpB6A;});_tmp63D;});Cyc_Absyn_new_vardecl(_tmpB6C,_tmpB6B,Cyc_Absyn_void_type,0);});_tmp63E->f1=_tmpB6D;}),({
# 2835
struct Cyc_Absyn_Pat*_tmpB68=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp63E->f2=_tmpB68;});_tmp63E;});
# 2833
Cyc_Absyn_new_pat(_tmpB6E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2837
goto _LL0;case 406U: _LL327: _LL328: {
# 2838 "parse.y"
void*_tmp63F=({struct _fat_ptr _tmpB6F=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpB6F,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});void*tag=_tmp63F;
yyval=Cyc_YY9(({void*_tmpB76=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB75=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp642->f1=_tmpB75;}),({
struct Cyc_Absyn_Vardecl*_tmpB74=({unsigned _tmpB73=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB72=({struct _tuple0*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB71=({struct _fat_ptr*_tmp640=_cycalloc(sizeof(*_tmp640));({struct _fat_ptr _tmpB70=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp640=_tmpB70;});_tmp640;});_tmp641->f2=_tmpB71;});_tmp641;});Cyc_Absyn_new_vardecl(_tmpB73,_tmpB72,
Cyc_Absyn_tag_type(tag),0);});
# 2840
_tmp642->f2=_tmpB74;});_tmp642;});
# 2839
Cyc_Absyn_new_pat(_tmpB76,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2843
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2844 "parse.y"
struct Cyc_Absyn_Tvar*_tmp643=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));struct Cyc_Absyn_Tvar*tv=_tmp643;
yyval=Cyc_YY9(({void*_tmpB7C=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->tag=4U,_tmp646->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpB7B=({unsigned _tmpB7A=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpB79=({struct _tuple0*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpB78=({struct _fat_ptr*_tmp644=_cycalloc(sizeof(*_tmp644));({struct _fat_ptr _tmpB77=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp644=_tmpB77;});_tmp644;});_tmp645->f2=_tmpB78;});_tmp645;});Cyc_Absyn_new_vardecl(_tmpB7A,_tmpB79,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2846
_tmp646->f2=_tmpB7B;});_tmp646;});
# 2845
Cyc_Absyn_new_pat(_tmpB7C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2849
goto _LL0;}case 408U: _LL32B: _LL32C:
# 2852 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp647=_cycalloc(sizeof(*_tmp647));({struct Cyc_List_List*_tmpB7D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp647->f1=_tmpB7D;}),_tmp647->f2=0;_tmp647;}));
goto _LL0;case 409U: _LL32D: _LL32E:
# 2853 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp648=_cycalloc(sizeof(*_tmp648));({struct Cyc_List_List*_tmpB7E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp648->f1=_tmpB7E;}),_tmp648->f2=1;_tmp648;}));
goto _LL0;case 410U: _LL32F: _LL330:
# 2854 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->f1=0,_tmp649->f2=1;_tmp649;}));
goto _LL0;case 411U: _LL331: _LL332:
# 2859 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_Absyn_Pat*_tmpB7F=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp64A->hd=_tmpB7F;}),_tmp64A->tl=0;_tmp64A;}));
goto _LL0;case 412U: _LL333: _LL334:
# 2861 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp64B=_cycalloc(sizeof(*_tmp64B));({struct Cyc_Absyn_Pat*_tmpB81=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp64B->hd=_tmpB81;}),({struct Cyc_List_List*_tmpB80=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp64B->tl=_tmpB80;});_tmp64B;}));
goto _LL0;case 413U: _LL335: _LL336:
# 2866 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->f1=0,({struct Cyc_Absyn_Pat*_tmpB82=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp64C->f2=_tmpB82;});_tmp64C;}));
goto _LL0;case 414U: _LL337: _LL338:
# 2868 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct Cyc_List_List*_tmpB84=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp64D->f1=_tmpB84;}),({struct Cyc_Absyn_Pat*_tmpB83=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp64D->f2=_tmpB83;});_tmp64D;}));
goto _LL0;case 415U: _LL339: _LL33A:
# 2871
 yyval=Cyc_YY14(({struct _tuple23*_tmp64E=_cycalloc(sizeof(*_tmp64E));({struct Cyc_List_List*_tmpB85=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64E->f1=_tmpB85;}),_tmp64E->f2=0;_tmp64E;}));
goto _LL0;case 416U: _LL33B: _LL33C:
# 2872 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp64F=_cycalloc(sizeof(*_tmp64F));({struct Cyc_List_List*_tmpB86=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp64F->f1=_tmpB86;}),_tmp64F->f2=1;_tmp64F;}));
goto _LL0;case 417U: _LL33D: _LL33E:
# 2873 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->f1=0,_tmp650->f2=1;_tmp650;}));
goto _LL0;case 418U: _LL33F: _LL340:
# 2878 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp651=_cycalloc(sizeof(*_tmp651));({struct _tuple24*_tmpB87=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp651->hd=_tmpB87;}),_tmp651->tl=0;_tmp651;}));
goto _LL0;case 419U: _LL341: _LL342:
# 2880 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp652=_cycalloc(sizeof(*_tmp652));({struct _tuple24*_tmpB89=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp652->hd=_tmpB89;}),({struct Cyc_List_List*_tmpB88=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp652->tl=_tmpB88;});_tmp652;}));
goto _LL0;case 420U: _LL343: _LL344:
# 2886 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 421U: _LL345: _LL346:
# 2888 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB8A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpB8B,_tmpB8A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422U: _LL347: _LL348:
# 2893 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 423U: _LL349: _LL34A:
# 2895 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpB8D=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpB8C=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpB8E,_tmpB8D,_tmpB8C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424U: _LL34B: _LL34C:
# 2897 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB8F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpB90,_tmpB8F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425U: _LL34D: _LL34E:
# 2901 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 426U: _LL34F: _LL350:
# 2902 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->v=(void*)Cyc_Absyn_Times;_tmp653;}));
goto _LL0;case 427U: _LL351: _LL352:
# 2903 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->v=(void*)Cyc_Absyn_Div;_tmp654;}));
goto _LL0;case 428U: _LL353: _LL354:
# 2904 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)Cyc_Absyn_Mod;_tmp655;}));
goto _LL0;case 429U: _LL355: _LL356:
# 2905 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->v=(void*)Cyc_Absyn_Plus;_tmp656;}));
goto _LL0;case 430U: _LL357: _LL358:
# 2906 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->v=(void*)Cyc_Absyn_Minus;_tmp657;}));
goto _LL0;case 431U: _LL359: _LL35A:
# 2907 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->v=(void*)Cyc_Absyn_Bitlshift;_tmp658;}));
goto _LL0;case 432U: _LL35B: _LL35C:
# 2908 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->v=(void*)Cyc_Absyn_Bitlrshift;_tmp659;}));
goto _LL0;case 433U: _LL35D: _LL35E:
# 2909 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->v=(void*)Cyc_Absyn_Bitand;_tmp65A;}));
goto _LL0;case 434U: _LL35F: _LL360:
# 2910 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->v=(void*)Cyc_Absyn_Bitxor;_tmp65B;}));
goto _LL0;case 435U: _LL361: _LL362:
# 2911 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->v=(void*)Cyc_Absyn_Bitor;_tmp65C;}));
goto _LL0;case 436U: _LL363: _LL364:
# 2916 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437U: _LL365: _LL366:
# 2918 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB93=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB92=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB91=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpB93,_tmpB92,_tmpB91,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438U: _LL367: _LL368:
# 2921
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB94=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpB94,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 439U: _LL369: _LL36A:
# 2924
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB95=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB95,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440U: _LL36B: _LL36C:
# 2926 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB96=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpB96,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 441U: _LL36D: _LL36E:
# 2928 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB98=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB97=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB98,_tmpB97,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442U: _LL36F: _LL370:
# 2930 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpB99=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpB9A,_tmpB99,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443U: _LL371: _LL372:
# 2934 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444U: _LL373: _LL374:
# 2938 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 445U: _LL375: _LL376:
# 2940 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpB9C,_tmpB9B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 446U: _LL377: _LL378:
# 2944 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 447U: _LL379: _LL37A:
# 2946 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpB9E,_tmpB9D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 448U: _LL37B: _LL37C:
# 2950 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 449U: _LL37D: _LL37E:
# 2952 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB9F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpBA0,_tmpB9F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 450U: _LL37F: _LL380:
# 2956 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 451U: _LL381: _LL382:
# 2958 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpBA2,_tmpBA1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 452U: _LL383: _LL384:
# 2962 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 453U: _LL385: _LL386:
# 2964 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpBA4,_tmpBA3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 454U: _LL387: _LL388:
# 2968 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 455U: _LL389: _LL38A:
# 2970 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_eq_exp(_tmpBA6,_tmpBA5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 456U: _LL38B: _LL38C:
# 2972 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_neq_exp(_tmpBA8,_tmpBA7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 457U: _LL38D: _LL38E:
# 2976 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 458U: _LL38F: _LL390:
# 2978 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lt_exp(_tmpBAA,_tmpBA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 459U: _LL391: _LL392:
# 2980 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gt_exp(_tmpBAC,_tmpBAB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 460U: _LL393: _LL394:
# 2982 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_lte_exp(_tmpBAE,_tmpBAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 461U: _LL395: _LL396:
# 2984 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_gte_exp(_tmpBB0,_tmpBAF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462U: _LL397: _LL398:
# 2988 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 463U: _LL399: _LL39A:
# 2990 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpBB2,_tmpBB1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464U: _LL39B: _LL39C:
# 2992 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpBB4,_tmpBB3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465U: _LL39D: _LL39E:
# 2996 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 466U: _LL39F: _LL3A0:
# 2998 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpBB6,_tmpBB5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 467U: _LL3A1: _LL3A2:
# 3000 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpBB8,_tmpBB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468U: _LL3A3: _LL3A4:
# 3004 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 469U: _LL3A5: _LL3A6:
# 3006 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBA=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpBBA,_tmpBB9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 470U: _LL3A7: _LL3A8:
# 3008 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpBBC,_tmpBBB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 471U: _LL3A9: _LL3AA:
# 3010 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBBE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBBD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpBBE,_tmpBBD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 472U: _LL3AB: _LL3AC:
# 3014 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 473U: _LL3AD: _LL3AE: {
# 3016 "parse.y"
void*_tmp65D=({struct _tuple8*_tmpBBF=Cyc_yyget_YY37(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpBBF,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp65D;
yyval=Cyc_Exp_tok(({void*_tmpBC1=t;struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpBC1,_tmpBC0,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3019
goto _LL0;}case 474U: _LL3AF: _LL3B0:
# 3022 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 475U: _LL3B1: _LL3B2:
# 3023 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC2,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476U: _LL3B3: _LL3B4:
# 3024 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpBC3,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477U: _LL3B5: _LL3B6:
# 3025 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpBC4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 478U: _LL3B7: _LL3B8:
# 3026 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpBC5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479U: _LL3B9: _LL3BA:
# 3027 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC6=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpBC6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480U: _LL3BB: _LL3BC:
# 3028 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpBC8=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpBC7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpBC8,_tmpBC7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481U: _LL3BD: _LL3BE: {
# 3030 "parse.y"
void*_tmp65E=({struct _tuple8*_tmpBC9=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBC9,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65E;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 3033
goto _LL0;}case 482U: _LL3BF: _LL3C0:
# 3033 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCA=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpBCA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 483U: _LL3C1: _LL3C2: {
# 3035 "parse.y"
void*_tmp65F=({struct _tuple8*_tmpBCB=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBCB,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp65F;
yyval=Cyc_Exp_tok(({void*_tmpBCD=t;struct Cyc_List_List*_tmpBCC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpBCD,_tmpBCC,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3038
goto _LL0;}case 484U: _LL3C3: _LL3C4:
# 3040
 yyval=Cyc_Exp_tok(({void*_tmpBCF=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->tag=34U,(_tmp660->f1).is_calloc=0,(_tmp660->f1).rgn=0,(_tmp660->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBCE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp660->f1).num_elts=_tmpBCE;}),(_tmp660->f1).fat_result=0,(_tmp660->f1).inline_call=0;_tmp660;});Cyc_Absyn_new_exp(_tmpBCF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3042
goto _LL0;case 485U: _LL3C5: _LL3C6:
# 3043 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBD2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=34U,(_tmp661->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBD1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp661->f1).rgn=_tmpBD1;}),(_tmp661->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD0=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp661->f1).num_elts=_tmpBD0;}),(_tmp661->f1).fat_result=0,(_tmp661->f1).inline_call=0;_tmp661;});Cyc_Absyn_new_exp(_tmpBD2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3045
goto _LL0;case 486U: _LL3C7: _LL3C8:
# 3046 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBD5=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp662=_cycalloc(sizeof(*_tmp662));_tmp662->tag=34U,(_tmp662->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBD4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp662->f1).rgn=_tmpBD4;}),(_tmp662->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBD3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp662->f1).num_elts=_tmpBD3;}),(_tmp662->f1).fat_result=0,(_tmp662->f1).inline_call=1;_tmp662;});Cyc_Absyn_new_exp(_tmpBD5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3048
goto _LL0;case 487U: _LL3C9: _LL3CA: {
# 3049 "parse.y"
void*_tmp663=({struct _tuple8*_tmpBD6=Cyc_yyget_YY37(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpBD6,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp663;
yyval=Cyc_Exp_tok(({void*_tmpBD9=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->tag=34U,(_tmp665->f1).is_calloc=1,(_tmp665->f1).rgn=0,({void**_tmpBD8=({void**_tmp664=_cycalloc(sizeof(*_tmp664));*_tmp664=t;_tmp664;});(_tmp665->f1).elt_type=_tmpBD8;}),({struct Cyc_Absyn_Exp*_tmpBD7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp665->f1).num_elts=_tmpBD7;}),(_tmp665->f1).fat_result=0,(_tmp665->f1).inline_call=0;_tmp665;});Cyc_Absyn_new_exp(_tmpBD9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3052
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3054
void*_tmp666=({struct _tuple8*_tmpBDA=Cyc_yyget_YY37(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpBDA,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp666;
yyval=Cyc_Exp_tok(({void*_tmpBDE=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->tag=34U,(_tmp668->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBDD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp668->f1).rgn=_tmpBDD;}),({void**_tmpBDC=({void**_tmp667=_cycalloc(sizeof(*_tmp667));*_tmp667=t;_tmp667;});(_tmp668->f1).elt_type=_tmpBDC;}),({struct Cyc_Absyn_Exp*_tmpBDB=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp668->f1).num_elts=_tmpBDB;}),(_tmp668->f1).fat_result=0,(_tmp668->f1).inline_call=0;_tmp668;});Cyc_Absyn_new_exp(_tmpBDE,(unsigned)((yyyvsp[0]).l).first_line);}));
# 3057
goto _LL0;}case 489U: _LL3CD: _LL3CE:
# 3058 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBE0=({struct Cyc_Absyn_Exp*_tmp669[1U];({struct Cyc_Absyn_Exp*_tmpBDF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp669[0]=_tmpBDF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp669,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBE0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 490U: _LL3CF: _LL3D0:
# 3060 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE4=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->tag=38U,({struct Cyc_Absyn_Exp*_tmpBE3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp66B->f1=_tmpBE3;}),({struct _fat_ptr*_tmpBE2=({struct _fat_ptr*_tmp66A=_cycalloc(sizeof(*_tmp66A));({struct _fat_ptr _tmpBE1=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp66A=_tmpBE1;});_tmp66A;});_tmp66B->f2=_tmpBE2;});_tmp66B;});Cyc_Absyn_new_exp(_tmpBE4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 491U: _LL3D1: _LL3D2:
# 3062 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBE9=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->tag=38U,({struct Cyc_Absyn_Exp*_tmpBE8=({struct Cyc_Absyn_Exp*_tmpBE7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpBE7,(unsigned)((yyyvsp[2]).l).first_line);});_tmp66D->f1=_tmpBE8;}),({struct _fat_ptr*_tmpBE6=({struct _fat_ptr*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct _fat_ptr _tmpBE5=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp66C=_tmpBE5;});_tmp66C;});_tmp66D->f2=_tmpBE6;});_tmp66D;});Cyc_Absyn_new_exp(_tmpBE9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 492U: _LL3D3: _LL3D4: {
# 3064 "parse.y"
void*_tmp66E=({struct _tuple8*_tmpBEA=Cyc_yyget_YY37(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpBEA,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp66E;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 493U: _LL3D5: _LL3D6:
# 3067 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpBEB=Cyc_yyget_YY58(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpBEB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 494U: _LL3D7: _LL3D8:
# 3068 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBEC=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpBEC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 495U: _LL3D9: _LL3DA:
# 3070 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBED=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpBED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 496U: _LL3DB: _LL3DC: {
# 3075 "parse.y"
struct _tuple29*_tmp66F=Cyc_yyget_YY59(&(yyyvsp[3]).v);struct _tuple29*_stmttmp30=_tmp66F;struct _tuple29*_tmp670=_stmttmp30;struct Cyc_List_List*_tmp673;struct Cyc_List_List*_tmp672;struct Cyc_List_List*_tmp671;_LL4BA: _tmp671=_tmp670->f1;_tmp672=_tmp670->f2;_tmp673=_tmp670->f3;_LL4BB: {struct Cyc_List_List*outlist=_tmp671;struct Cyc_List_List*inlist=_tmp672;struct Cyc_List_List*clobbers=_tmp673;
yyval=Cyc_YY58((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->tag=40U,({int _tmpBEF=Cyc_yyget_YY31(&(yyyvsp[0]).v);_tmp674->f1=_tmpBEF;}),({struct _fat_ptr _tmpBEE=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp674->f2=_tmpBEE;}),_tmp674->f3=outlist,_tmp674->f4=inlist,_tmp674->f5=clobbers;_tmp674;}));
goto _LL0;}}case 497U: _LL3DD: _LL3DE:
# 3080 "parse.y"
 yyval=Cyc_YY31(0);
goto _LL0;case 498U: _LL3DF: _LL3E0:
# 3081 "parse.y"
 yyval=Cyc_YY31(1);
goto _LL0;case 499U: _LL3E1: _LL3E2:
# 3085 "parse.y"
 yyval=Cyc_YY59(({struct _tuple29*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->f1=0,_tmp675->f2=0,_tmp675->f3=0;_tmp675;}));
goto _LL0;case 500U: _LL3E3: _LL3E4: {
# 3087 "parse.y"
struct _tuple30*_tmp676=Cyc_yyget_YY60(&(yyyvsp[1]).v);struct _tuple30*_stmttmp31=_tmp676;struct _tuple30*_tmp677=_stmttmp31;struct Cyc_List_List*_tmp679;struct Cyc_List_List*_tmp678;_LL4BD: _tmp678=_tmp677->f1;_tmp679=_tmp677->f2;_LL4BE: {struct Cyc_List_List*inlist=_tmp678;struct Cyc_List_List*clobbers=_tmp679;
yyval=Cyc_YY59(({struct _tuple29*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->f1=0,_tmp67A->f2=inlist,_tmp67A->f3=clobbers;_tmp67A;}));
goto _LL0;}}case 501U: _LL3E5: _LL3E6: {
# 3090 "parse.y"
struct _tuple30*_tmp67B=Cyc_yyget_YY60(&(yyyvsp[2]).v);struct _tuple30*_stmttmp32=_tmp67B;struct _tuple30*_tmp67C=_stmttmp32;struct Cyc_List_List*_tmp67E;struct Cyc_List_List*_tmp67D;_LL4C0: _tmp67D=_tmp67C->f1;_tmp67E=_tmp67C->f2;_LL4C1: {struct Cyc_List_List*inlist=_tmp67D;struct Cyc_List_List*clobbers=_tmp67E;
yyval=Cyc_YY59(({struct _tuple29*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct Cyc_List_List*_tmpBF0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp67F->f1=_tmpBF0;}),_tmp67F->f2=inlist,_tmp67F->f3=clobbers;_tmp67F;}));
goto _LL0;}}case 502U: _LL3E7: _LL3E8:
# 3095 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp680=_cycalloc(sizeof(*_tmp680));({struct _tuple31*_tmpBF1=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp680->hd=_tmpBF1;}),_tmp680->tl=0;_tmp680;}));
goto _LL0;case 503U: _LL3E9: _LL3EA:
# 3096 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp681=_cycalloc(sizeof(*_tmp681));({struct _tuple31*_tmpBF3=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp681->hd=_tmpBF3;}),({struct Cyc_List_List*_tmpBF2=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp681->tl=_tmpBF2;});_tmp681;}));
goto _LL0;case 504U: _LL3EB: _LL3EC:
# 3100 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->f1=0,_tmp682->f2=0;_tmp682;}));
goto _LL0;case 505U: _LL3ED: _LL3EE:
# 3102 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->f1=0,({struct Cyc_List_List*_tmpBF4=Cyc_yyget_YY61(&(yyyvsp[1]).v);_tmp683->f2=_tmpBF4;});_tmp683;}));
goto _LL0;case 506U: _LL3EF: _LL3F0:
# 3104 "parse.y"
 yyval=Cyc_YY60(({struct _tuple30*_tmp684=_cycalloc(sizeof(*_tmp684));({struct Cyc_List_List*_tmpBF6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY62(&(yyyvsp[1]).v));_tmp684->f1=_tmpBF6;}),({struct Cyc_List_List*_tmpBF5=Cyc_yyget_YY61(&(yyyvsp[2]).v);_tmp684->f2=_tmpBF5;});_tmp684;}));
goto _LL0;case 507U: _LL3F1: _LL3F2:
# 3108 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp685=_cycalloc(sizeof(*_tmp685));({struct _tuple31*_tmpBF7=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp685->hd=_tmpBF7;}),_tmp685->tl=0;_tmp685;}));
goto _LL0;case 508U: _LL3F3: _LL3F4:
# 3109 "parse.y"
 yyval=Cyc_YY62(({struct Cyc_List_List*_tmp686=_cycalloc(sizeof(*_tmp686));({struct _tuple31*_tmpBF9=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp686->hd=_tmpBF9;}),({struct Cyc_List_List*_tmpBF8=Cyc_yyget_YY62(&(yyyvsp[0]).v);_tmp686->tl=_tmpBF8;});_tmp686;}));
goto _LL0;case 509U: _LL3F5: _LL3F6: {
# 3114 "parse.y"
struct Cyc_Absyn_Exp*_tmp687=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*pf_exp=_tmp687;
yyval=Cyc_YY63(({struct _tuple31*_tmp688=_cycalloc(sizeof(*_tmp688));({struct _fat_ptr _tmpBFB=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp688->f1=_tmpBFB;}),({struct Cyc_Absyn_Exp*_tmpBFA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp688->f2=_tmpBFA;});_tmp688;}));
goto _LL0;}case 510U: _LL3F7: _LL3F8:
# 3120 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 511U: _LL3F9: _LL3FA:
# 3121 "parse.y"
 yyval=Cyc_YY61(0);
goto _LL0;case 512U: _LL3FB: _LL3FC:
# 3122 "parse.y"
 yyval=Cyc_YY61(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY61(&(yyyvsp[1]).v)));
goto _LL0;case 513U: _LL3FD: _LL3FE:
# 3126 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp68A=_cycalloc(sizeof(*_tmp68A));({struct _fat_ptr*_tmpBFD=({struct _fat_ptr*_tmp689=_cycalloc(sizeof(*_tmp689));({struct _fat_ptr _tmpBFC=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp689=_tmpBFC;});_tmp689;});_tmp68A->hd=_tmpBFD;}),_tmp68A->tl=0;_tmp68A;}));
goto _LL0;case 514U: _LL3FF: _LL400:
# 3127 "parse.y"
 yyval=Cyc_YY61(({struct Cyc_List_List*_tmp68C=_cycalloc(sizeof(*_tmp68C));({struct _fat_ptr*_tmpC00=({struct _fat_ptr*_tmp68B=_cycalloc(sizeof(*_tmp68B));({struct _fat_ptr _tmpBFF=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68B=_tmpBFF;});_tmp68B;});_tmp68C->hd=_tmpC00;}),({struct Cyc_List_List*_tmpBFE=Cyc_yyget_YY61(&(yyyvsp[0]).v);_tmp68C->tl=_tmpBFE;});_tmp68C;}));
goto _LL0;case 515U: _LL401: _LL402:
# 3131 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 516U: _LL403: _LL404:
# 3132 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 517U: _LL405: _LL406:
# 3133 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 518U: _LL407: _LL408:
# 3138 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 519U: _LL409: _LL40A:
# 3140 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC02=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpC01=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpC02,_tmpC01,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520U: _LL40B: _LL40C:
# 3142 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC03=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpC03,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521U: _LL40D: _LL40E:
# 3144 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC05=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC04=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpC05,_tmpC04,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 522U: _LL40F: _LL410:
# 3146 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC08=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC07=({struct _fat_ptr*_tmp68D=_cycalloc(sizeof(*_tmp68D));({struct _fat_ptr _tmpC06=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68D=_tmpC06;});_tmp68D;});Cyc_Absyn_aggrmember_exp(_tmpC08,_tmpC07,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523U: _LL411: _LL412:
# 3148 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpC0A=({struct _fat_ptr*_tmp68E=_cycalloc(sizeof(*_tmp68E));({struct _fat_ptr _tmpC09=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp68E=_tmpC09;});_tmp68E;});Cyc_Absyn_aggrarrow_exp(_tmpC0B,_tmpC0A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524U: _LL413: _LL414:
# 3150 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC0C,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 525U: _LL415: _LL416:
# 3152 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC0D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpC0D,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526U: _LL417: _LL418:
# 3154 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC0F=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=25U,({struct _tuple8*_tmpC0E=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp68F->f1=_tmpC0E;}),_tmp68F->f2=0;_tmp68F;});Cyc_Absyn_new_exp(_tmpC0F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527U: _LL419: _LL41A:
# 3156 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC12=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->tag=25U,({struct _tuple8*_tmpC11=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp690->f1=_tmpC11;}),({struct Cyc_List_List*_tmpC10=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp690->f2=_tmpC10;});_tmp690;});Cyc_Absyn_new_exp(_tmpC12,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528U: _LL41B: _LL41C:
# 3158 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpC15=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp691=_cycalloc(sizeof(*_tmp691));_tmp691->tag=25U,({struct _tuple8*_tmpC14=Cyc_yyget_YY37(&(yyyvsp[1]).v);_tmp691->f1=_tmpC14;}),({struct Cyc_List_List*_tmpC13=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp691->f2=_tmpC13;});_tmp691;});Cyc_Absyn_new_exp(_tmpC15,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529U: _LL41D: _LL41E:
# 3163 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp694=_cycalloc(sizeof(*_tmp694));({void*_tmpC18=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp693=_cycalloc(sizeof(*_tmp693));_tmp693->tag=0U,({struct _fat_ptr*_tmpC17=({struct _fat_ptr*_tmp692=_cycalloc(sizeof(*_tmp692));({struct _fat_ptr _tmpC16=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp692=_tmpC16;});_tmp692;});_tmp693->f1=_tmpC17;});_tmp693;});_tmp694->hd=_tmpC18;}),_tmp694->tl=0;_tmp694;}));
goto _LL0;case 530U: _LL41F: _LL420:
# 3166
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp696=_cycalloc(sizeof(*_tmp696));({void*_tmpC1B=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->tag=1U,({unsigned _tmpC1A=({unsigned _tmpC19=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpC19,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp695->f1=_tmpC1A;});_tmp695;});_tmp696->hd=_tmpC1B;}),_tmp696->tl=0;_tmp696;}));
goto _LL0;case 531U: _LL421: _LL422:
# 3168 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));({void*_tmpC1F=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->tag=0U,({struct _fat_ptr*_tmpC1E=({struct _fat_ptr*_tmp697=_cycalloc(sizeof(*_tmp697));({struct _fat_ptr _tmpC1D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp697=_tmpC1D;});_tmp697;});_tmp698->f1=_tmpC1E;});_tmp698;});_tmp699->hd=_tmpC1F;}),({struct Cyc_List_List*_tmpC1C=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp699->tl=_tmpC1C;});_tmp699;}));
goto _LL0;case 532U: _LL423: _LL424:
# 3171
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp69B=_cycalloc(sizeof(*_tmp69B));({void*_tmpC23=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->tag=1U,({unsigned _tmpC22=({unsigned _tmpC21=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpC21,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp69A->f1=_tmpC22;});_tmp69A;});_tmp69B->hd=_tmpC23;}),({struct Cyc_List_List*_tmpC20=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp69B->tl=_tmpC20;});_tmp69B;}));
goto _LL0;case 533U: _LL425: _LL426:
# 3177 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpC24=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpC24,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534U: _LL427: _LL428:
# 3179 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC25=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpC25,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535U: _LL429: _LL42A:
# 3181 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 536U: _LL42B: _LL42C:
# 3183 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC26=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpC26,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 537U: _LL42D: _LL42E:
# 3185 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC27=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpC27,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 538U: _LL42F: _LL430:
# 3187 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 539U: _LL431: _LL432:
# 3192 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC28=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpC28,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 540U: _LL433: _LL434:
# 3194 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpC2A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpC29=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpC2A,_tmpC29,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 541U: _LL435: _LL436:
# 3197
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpC2B=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpC2B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 542U: _LL437: _LL438:
# 3200
 yyval=Cyc_Exp_tok(({void*_tmpC2F=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->tag=29U,({struct _tuple0*_tmpC2E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp69C->f1=_tmpC2E;}),({struct Cyc_List_List*_tmpC2D=Cyc_yyget_YY40(&(yyyvsp[2]).v);_tmp69C->f2=_tmpC2D;}),({struct Cyc_List_List*_tmpC2C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp69C->f3=_tmpC2C;}),_tmp69C->f4=0;_tmp69C;});Cyc_Absyn_new_exp(_tmpC2F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 543U: _LL439: _LL43A:
# 3203
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpC30=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpC30,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 544U: _LL43B: _LL43C:
# 3207 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 545U: _LL43D: _LL43E:
# 3213 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));({struct Cyc_Absyn_Exp*_tmpC31=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp69D->hd=_tmpC31;}),_tmp69D->tl=0;_tmp69D;}));
goto _LL0;case 546U: _LL43F: _LL440:
# 3215 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp69E=_cycalloc(sizeof(*_tmp69E));({struct Cyc_Absyn_Exp*_tmpC33=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp69E->hd=_tmpC33;}),({struct Cyc_List_List*_tmpC32=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp69E->tl=_tmpC32;});_tmp69E;}));
goto _LL0;case 547U: _LL441: _LL442:
# 3221 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpC34=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpC34,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 548U: _LL443: _LL444:
# 3222 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpC35=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpC35,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 549U: _LL445: _LL446:
# 3223 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpC36=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpC36,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 550U: _LL447: _LL448: {
# 3225 "parse.y"
struct _fat_ptr _tmp69F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp69F;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 3233
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 3235
goto _LL0;}case 551U: _LL449: _LL44A:
# 3236 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 552U: _LL44B: _LL44C:
# 3240 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));({union Cyc_Absyn_Nmspace _tmpC39=Cyc_Absyn_Rel_n(0);_tmp6A1->f1=_tmpC39;}),({struct _fat_ptr*_tmpC38=({struct _fat_ptr*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));({struct _fat_ptr _tmpC37=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp6A0=_tmpC37;});_tmp6A0;});_tmp6A1->f2=_tmpC38;});_tmp6A1;}));
goto _LL0;case 553U: _LL44D: _LL44E:
# 3241 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 554U: _LL44F: _LL450:
# 3244
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));({union Cyc_Absyn_Nmspace _tmpC3C=Cyc_Absyn_Rel_n(0);_tmp6A3->f1=_tmpC3C;}),({struct _fat_ptr*_tmpC3B=({struct _fat_ptr*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));({struct _fat_ptr _tmpC3A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp6A2=_tmpC3A;});_tmp6A2;});_tmp6A3->f2=_tmpC3B;});_tmp6A3;}));
goto _LL0;case 555U: _LL451: _LL452:
# 3245 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 556U: _LL453: _LL454:
# 3250 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 557U: _LL455: _LL456:
# 3251 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 558U: _LL457: _LL458:
# 3254
 yyval=(yyyvsp[0]).v;
goto _LL0;case 559U: _LL459: _LL45A:
# 3255 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 560U: _LL45B: _LL45C:
# 3260 "parse.y"
 goto _LL0;case 561U: _LL45D: _LL45E:
# 3260 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL45F: _LL460:
# 3264
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
({int _tmpC3D=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpC3D;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpC3E=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,162U,sizeof(short),yyn - 152));_tmpC3E + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
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
({unsigned long _tmpC3F=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,314U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpC3F;}),count ++;}
msg=({unsigned _tmp6A5=(unsigned)(sze + 15)+ 1U;char*_tmp6A4=({struct _RegionHandle*_tmpC40=yyregion;_region_malloc(_tmpC40,_check_times(_tmp6A5,sizeof(char)));});({{unsigned _tmp764=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp764;++ i){_tmp6A4[i]='\000';}_tmp6A4[_tmp764]=0;}0;});_tag_fat(_tmp6A4,sizeof(char),_tmp6A5);});
({struct _fat_ptr _tmpC41=msg;Cyc_strcpy(_tmpC41,({const char*_tmp6A6="parse error";_tag_fat(_tmp6A6,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 314U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7950U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpC42=msg;Cyc_strcat(_tmpC42,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp6A7=", expecting `";_tag_fat(_tmp6A7,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp6A8=" or `";_tag_fat(_tmp6A8,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,314U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpC43=msg;Cyc_strcat(_tmpC43,({const char*_tmp6A9="'";_tag_fat(_tmp6A9,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpC45=({const char*_tmp6AA="parse error";_tag_fat(_tmp6AA,sizeof(char),12U);});int _tmpC44=yystate;Cyc_yyerror(_tmpC45,_tmpC44,yychar);});}}
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
int _tmp6AD=0;_npop_handler(0U);return _tmp6AD;}
# 546
({struct Cyc_Yystacktype _tmpC46=({struct Cyc_Yystacktype _tmp765;_tmp765.v=yylval,_tmp765.l=yylloc;_tmp765;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpC46;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 3263 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6B1=v;struct Cyc_Absyn_Stmt*_tmp6B2;struct Cyc_Absyn_Exp*_tmp6B3;struct _tuple0*_tmp6B4;struct _fat_ptr _tmp6B5;char _tmp6B6;union Cyc_Absyn_Cnst _tmp6B7;switch((_tmp6B1.Stmt_tok).tag){case 1U: _LL1: _tmp6B7=(_tmp6B1.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp6B7;
({struct Cyc_String_pa_PrintArg_struct _tmp6BA=({struct Cyc_String_pa_PrintArg_struct _tmp768;_tmp768.tag=0U,({struct _fat_ptr _tmpC47=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp768.f1=_tmpC47;});_tmp768;});void*_tmp6B8[1U];_tmp6B8[0]=& _tmp6BA;({struct Cyc___cycFILE*_tmpC49=Cyc_stderr;struct _fat_ptr _tmpC48=({const char*_tmp6B9="%s";_tag_fat(_tmp6B9,sizeof(char),3U);});Cyc_fprintf(_tmpC49,_tmpC48,_tag_fat(_tmp6B8,sizeof(void*),1U));});});goto _LL0;}case 2U: _LL3: _tmp6B6=(_tmp6B1.Char_tok).val;_LL4: {char c=_tmp6B6;
({struct Cyc_Int_pa_PrintArg_struct _tmp6BD=({struct Cyc_Int_pa_PrintArg_struct _tmp769;_tmp769.tag=1U,_tmp769.f1=(unsigned long)((int)c);_tmp769;});void*_tmp6BB[1U];_tmp6BB[0]=& _tmp6BD;({struct Cyc___cycFILE*_tmpC4B=Cyc_stderr;struct _fat_ptr _tmpC4A=({const char*_tmp6BC="%c";_tag_fat(_tmp6BC,sizeof(char),3U);});Cyc_fprintf(_tmpC4B,_tmpC4A,_tag_fat(_tmp6BB,sizeof(void*),1U));});});goto _LL0;}case 3U: _LL5: _tmp6B5=(_tmp6B1.String_tok).val;_LL6: {struct _fat_ptr s=_tmp6B5;
({struct Cyc_String_pa_PrintArg_struct _tmp6C0=({struct Cyc_String_pa_PrintArg_struct _tmp76A;_tmp76A.tag=0U,_tmp76A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp76A;});void*_tmp6BE[1U];_tmp6BE[0]=& _tmp6C0;({struct Cyc___cycFILE*_tmpC4D=Cyc_stderr;struct _fat_ptr _tmpC4C=({const char*_tmp6BF="\"%s\"";_tag_fat(_tmp6BF,sizeof(char),5U);});Cyc_fprintf(_tmpC4D,_tmpC4C,_tag_fat(_tmp6BE,sizeof(void*),1U));});});goto _LL0;}case 5U: _LL7: _tmp6B4=(_tmp6B1.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp6B4;
({struct Cyc_String_pa_PrintArg_struct _tmp6C3=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,({struct _fat_ptr _tmpC4E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp76B.f1=_tmpC4E;});_tmp76B;});void*_tmp6C1[1U];_tmp6C1[0]=& _tmp6C3;({struct Cyc___cycFILE*_tmpC50=Cyc_stderr;struct _fat_ptr _tmpC4F=({const char*_tmp6C2="%s";_tag_fat(_tmp6C2,sizeof(char),3U);});Cyc_fprintf(_tmpC50,_tmpC4F,_tag_fat(_tmp6C1,sizeof(void*),1U));});});goto _LL0;}case 7U: _LL9: _tmp6B3=(_tmp6B1.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp6B3;
({struct Cyc_String_pa_PrintArg_struct _tmp6C6=({struct Cyc_String_pa_PrintArg_struct _tmp76C;_tmp76C.tag=0U,({struct _fat_ptr _tmpC51=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp76C.f1=_tmpC51;});_tmp76C;});void*_tmp6C4[1U];_tmp6C4[0]=& _tmp6C6;({struct Cyc___cycFILE*_tmpC53=Cyc_stderr;struct _fat_ptr _tmpC52=({const char*_tmp6C5="%s";_tag_fat(_tmp6C5,sizeof(char),3U);});Cyc_fprintf(_tmpC53,_tmpC52,_tag_fat(_tmp6C4,sizeof(void*),1U));});});goto _LL0;}case 8U: _LLB: _tmp6B2=(_tmp6B1.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp6B2;
({struct Cyc_String_pa_PrintArg_struct _tmp6C9=({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0U,({struct _fat_ptr _tmpC54=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp76D.f1=_tmpC54;});_tmp76D;});void*_tmp6C7[1U];_tmp6C7[0]=& _tmp6C9;({struct Cyc___cycFILE*_tmpC56=Cyc_stderr;struct _fat_ptr _tmpC55=({const char*_tmp6C8="%s";_tag_fat(_tmp6C8,sizeof(char),3U);});Cyc_fprintf(_tmpC56,_tmpC55,_tag_fat(_tmp6C7,sizeof(void*),1U));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp6CA=0U;({struct Cyc___cycFILE*_tmpC58=Cyc_stderr;struct _fat_ptr _tmpC57=({const char*_tmp6CB="?";_tag_fat(_tmp6CB,sizeof(char),2U);});Cyc_fprintf(_tmpC58,_tmpC57,_tag_fat(_tmp6CA,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3275
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6CC="end-of-file";_tag_fat(_tmp6CC,sizeof(char),12U);});
if(token == 367)
return Cyc_Lex_token_string;else{
if(token == 376)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0 && token <= 379?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,380U,sizeof(short),token)): 314;
if((unsigned)z < 314U)
return Cyc_yytname[z];else{
return _tag_fat(0,0,0);}}}
# 3289
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6CD=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6CD;_push_region(yyr);
({struct _RegionHandle*_tmpC59=yyr;Cyc_yyparse(_tmpC59,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6CE=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6CE;}
# 3292
;_pop_region();}}
