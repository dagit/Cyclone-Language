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
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 153
extern void*Cyc_List_fold_right(void*(*)(void*,void*),struct Cyc_List_List*,void*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 195
extern struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 52
extern int Cyc_zstrcmp(struct _fat_ptr,struct _fat_ptr);
# 54
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 60
extern struct _fat_ptr Cyc_strcat(struct _fat_ptr,struct _fat_ptr);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 71
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr,struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 283
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 296
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 303
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 663 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 825
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 850
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 858
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 864
void*Cyc_Absyn_compress(void*);
# 877
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 879
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 882
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 884
extern void*Cyc_Absyn_sint_type;
# 886
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 889
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 893
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 895
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 920
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 924
void*Cyc_Absyn_bounds_one (void);
# 926
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 946
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 949
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 967
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 970
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 974
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 982
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _fat_ptr*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1036
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1041
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1052
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1055
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1058
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1062
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1067
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1069
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1076
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1083
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1086
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1091
void*Cyc_Absyn_function_type(struct Cyc_List_List*,void*,struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,int,struct Cyc_Absyn_VarargInfo*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 29 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 35
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 26 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 34
extern int Cyc_Flags_no_register;
# 78
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 92 "flags.h"
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
void*Cyc_Tcutil_promote_array(void*,void*,int);
# 238
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);
# 29 "attributes.h"
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Atts_Const_att_val;
# 46
void*Cyc_Atts_parse_nullary_att(unsigned,struct _fat_ptr);
void*Cyc_Atts_parse_unary_att(unsigned,struct _fat_ptr,unsigned,struct Cyc_Absyn_Exp*);
void*Cyc_Atts_parse_format_att(unsigned,unsigned,struct _fat_ptr,struct _fat_ptr,unsigned,unsigned);
# 68
int Cyc_Atts_fntype_att(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
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
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned varloc;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};struct _tuple13{struct _tuple13*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};
# 54 "parse.h"
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc =0U,Cyc_Parse_Extern_sc =1U,Cyc_Parse_ExternC_sc =2U,Cyc_Parse_Static_sc =3U,Cyc_Parse_Auto_sc =4U,Cyc_Parse_Register_sc =5U,Cyc_Parse_Abstract_sc =6U,Cyc_Parse_None_sc =7U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 73 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*);
void Cyc_Lex_enter_namespace(struct _fat_ptr*);
void Cyc_Lex_leave_namespace (void);
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using (void);
void Cyc_Lex_enter_extern_c (void);
void Cyc_Lex_leave_extern_c (void);
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _fat_ptr Cyc_Lex_token_string;
# 97 "parse.y"
int Cyc_Parse_parsing_tempest=0;
# 103
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
# 114
return first;}}char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 167
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier,unsigned);struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 172
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 174
static void*Cyc_Parse_parse_abort(unsigned loc,struct _fat_ptr msg){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62E;_tmp62E.tag=0,_tmp62E.f1=msg;_tmp62E;});void*_tmp0[1];_tmp0[0]=& _tmp1;({unsigned _tmp6D6=loc;Cyc_Warn_err2(_tmp6D6,_tag_fat(_tmp0,sizeof(void*),1));});});
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_Parse_Exit;_tmp2;}));}
# 179
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 181
struct _tuple8*_tmp3=tqt;void*_tmp5;struct Cyc_Absyn_Tqual _tmp4;_tmp4=_tmp3->f2;_tmp5=_tmp3->f3;{struct Cyc_Absyn_Tqual tq=_tmp4;void*t=_tmp5;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc != (unsigned)0)loc=tq.loc;
({void*_tmp6=0U;({unsigned _tmp6D8=loc;struct _fat_ptr _tmp6D7=({const char*_tmp7="qualifier on type is ignored";_tag_fat(_tmp7,sizeof(char),29U);});Cyc_Warn_warn(_tmp6D8,_tmp6D7,_tag_fat(_tmp6,sizeof(void*),0));});});}
# 186
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 189
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 195
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*released=Cyc_Tcutil_any_bool(0);
for(1;pqs != 0;pqs=pqs->tl){
void*_tmp8=(void*)pqs->hd;void*_stmttmp0=_tmp8;void*_tmp9=_stmttmp0;void*_tmpA;switch(*((int*)_tmp9)){case 4:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 6:
 released=Cyc_Absyn_true_type;goto _LL0;case 8:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 7:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _tmpA=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmp9)->f1;{struct Cyc_Absyn_Exp*e=_tmpA;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}default: _tmpA=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmp9)->f1;{void*t=_tmpA;
rgn=t;goto _LL0;}}_LL0:;}
# 209
return({struct _tuple15 _tmp62F;_tmp62F.f1=nullable,_tmp62F.f2=bound,_tmp62F.f3=zeroterm,_tmp62F.f4=rgn,_tmp62F.f5=released;_tmp62F;});}
# 215
static struct _tuple0*Cyc_Parse_gensym_enum (void){
# 217
static int enum_counter=0;
return({struct _tuple0*_tmpF=_cycalloc(sizeof(*_tmpF));({union Cyc_Absyn_Nmspace _tmp6DC=Cyc_Absyn_Rel_n(0);_tmpF->f1=_tmp6DC;}),({
struct _fat_ptr*_tmp6DB=({struct _fat_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _fat_ptr _tmp6DA=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpD=({struct Cyc_Int_pa_PrintArg_struct _tmp630;_tmp630.tag=1,_tmp630.f1=(unsigned long)enum_counter ++;_tmp630;});void*_tmpB[1];_tmpB[0]=& _tmpD;({struct _fat_ptr _tmp6D9=({const char*_tmpC="__anonymous_enum_%d__";_tag_fat(_tmpC,sizeof(char),22U);});Cyc_aprintf(_tmp6D9,_tag_fat(_tmpB,sizeof(void*),1));});});*_tmpE=_tmp6DA;});_tmpE;});_tmpF->f2=_tmp6DB;});_tmpF;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 222
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 227
struct _tuple18*_tmp10=field_info;void*_tmp18;void*_tmp17;void*_tmp16;void*_tmp15;void*_tmp14;struct Cyc_Absyn_Tqual _tmp13;void*_tmp12;unsigned _tmp11;_tmp11=(_tmp10->f1)->f1;_tmp12=(_tmp10->f1)->f2;_tmp13=(_tmp10->f1)->f3;_tmp14=(_tmp10->f1)->f4;_tmp15=(_tmp10->f1)->f5;_tmp16=(_tmp10->f1)->f6;_tmp17=(_tmp10->f2)->f1;_tmp18=(_tmp10->f2)->f2;{unsigned varloc=_tmp11;struct _tuple0*qid=_tmp12;struct Cyc_Absyn_Tqual tq=_tmp13;void*t=_tmp14;struct Cyc_List_List*tvs=_tmp15;struct Cyc_List_List*atts=_tmp16;struct Cyc_Absyn_Exp*widthopt=_tmp17;struct Cyc_Absyn_Exp*reqopt=_tmp18;
if(tvs != 0)
({void*_tmp19=0U;({unsigned _tmp6DE=loc;struct _fat_ptr _tmp6DD=({const char*_tmp1A="bad type params in struct field";_tag_fat(_tmp1A,sizeof(char),32U);});Cyc_Warn_err(_tmp6DE,_tmp6DD,_tag_fat(_tmp19,sizeof(void*),0));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp1B=0U;({unsigned _tmp6E0=loc;struct _fat_ptr _tmp6DF=({const char*_tmp1C="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp1C,sizeof(char),59U);});Cyc_Warn_err(_tmp6E0,_tmp6DF,_tag_fat(_tmp1B,sizeof(void*),0));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1D=_cycalloc(sizeof(*_tmp1D));_tmp1D->name=(*qid).f2,_tmp1D->tq=tq,_tmp1D->type=t,_tmp1D->width=widthopt,_tmp1D->attributes=atts,_tmp1D->requires_clause=reqopt;_tmp1D;});}}
# 237
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp631;_tmp631.Signed_spec=0,_tmp631.Unsigned_spec=0,_tmp631.Short_spec=0,_tmp631.Long_spec=0,_tmp631.Long_Long_spec=0,_tmp631.Valid_type_spec=0,_tmp631.Type_spec=Cyc_Absyn_sint_type,_tmp631.loc=loc;_tmp631;});}
# 248
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier _tmp1E=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp1E;
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 254
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp1F=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp1F;
s.Signed_spec=1;
return s;}
# 259
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp20=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp20;
s.Unsigned_spec=1;
return s;}
# 264
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp21=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp21;
s.Short_spec=1;
return s;}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp22=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp22;
s.Long_spec=1;
return s;}
# 276
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 278
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp6E1=t;Cyc_Tcutil_promote_array(_tmp6E1,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 291 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp23=0;struct Cyc_List_List*res=_tmp23;
for(1;x != 0;x=x->tl){
struct _tuple8*_tmp24=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp1=_tmp24;struct _tuple8*_tmp25=_stmttmp1;struct _fat_ptr _tmp26;void*_tmp28;void*_tmp27;if(*((int*)((struct _tuple8*)_tmp25)->f3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2)->tl == 0){_tmp27=_tmp25->f1;_tmp28=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25->f3)->f2)->hd;if((struct _fat_ptr*)_tmp27 != 0){struct _fat_ptr*v=_tmp27;void*i=_tmp28;
# 296
{void*_tmp29=i;void*_tmp2A;if(*((int*)_tmp29)== 1){_tmp2A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp29)->f2;{void**z=(void**)_tmp2A;
# 300
struct _fat_ptr*nm=({struct _fat_ptr*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr _tmp6E3=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp632;_tmp632.tag=0,_tmp632.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp632;});void*_tmp2D[1];_tmp2D[0]=& _tmp2F;({struct _fat_ptr _tmp6E2=({const char*_tmp2E="`%s";_tag_fat(_tmp2E,sizeof(char),4U);});Cyc_aprintf(_tmp6E2,_tag_fat(_tmp2D,sizeof(void*),1));});});*_tmp30=_tmp6E3;});_tmp30;});
({void*_tmp6E5=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->name=nm,_tmp2C->identity=- 1,({void*_tmp6E4=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=0,_tmp2B->f1=& Cyc_Kinds_ik;_tmp2B;});_tmp2C->kind=_tmp6E4;});_tmp2C;}));*z=_tmp6E5;});
goto _LL7;}}else{
goto _LL7;}_LL7:;}
# 305
res=({struct Cyc_List_List*_tmp32=_cycalloc(sizeof(*_tmp32));({struct _tuple19*_tmp6E6=({struct _tuple19*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->f1=v,_tmp31->f2=i;_tmp31;});_tmp32->hd=_tmp6E6;}),_tmp32->tl=res;_tmp32;});goto _LL0;}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2)->hd)== 1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp25)->f3)->f2)->tl == 0){_tmp26=*_tmp25->f1;_tmp27=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25->f3)->f2)->hd)->f2;{struct _fat_ptr v=_tmp26;void**z=(void**)_tmp27;
# 309
struct _fat_ptr*nm=({struct _fat_ptr*_tmp38=_cycalloc(sizeof(*_tmp38));({struct _fat_ptr _tmp6E8=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp37=({struct Cyc_String_pa_PrintArg_struct _tmp633;_tmp633.tag=0,_tmp633.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp633;});void*_tmp35[1];_tmp35[0]=& _tmp37;({struct _fat_ptr _tmp6E7=({const char*_tmp36="`%s";_tag_fat(_tmp36,sizeof(char),4U);});Cyc_aprintf(_tmp6E7,_tag_fat(_tmp35,sizeof(void*),1));});});*_tmp38=_tmp6E8;});_tmp38;});
({void*_tmp6EA=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->name=nm,_tmp34->identity=- 1,({void*_tmp6E9=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->tag=0,_tmp33->f1=& Cyc_Kinds_rk;_tmp33;});_tmp34->kind=_tmp6E9;});_tmp34;}));*z=_tmp6EA;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp25)->f1 != 0)goto _LL5;else{_LL5:
 goto _LL0;}}_LL0:;}
# 314
return res;}
# 318
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp39=0;struct Cyc_List_List*res=_tmp39;
for(1;x != 0;x=x->tl){
void*_tmp3A=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_stmttmp2=_tmp3A;void*_tmp3B=_stmttmp2;void*_tmp3C;if(*((int*)_tmp3B)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2)->tl == 0){_tmp3C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2)->hd;{void*i=_tmp3C;
# 323
res=({struct Cyc_List_List*_tmp3E=_cycalloc(sizeof(*_tmp3E));({struct _tuple19*_tmp6EB=({struct _tuple19*_tmp3D=_cycalloc(sizeof(*_tmp3D));_tmp3D->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp3D->f2=i;_tmp3D;});_tmp3E->hd=_tmp6EB;}),_tmp3E->tl=res;_tmp3E;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 326
return res;}
# 330
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp3F=e->r;void*_stmttmp3=_tmp3F;void*_tmp40=_stmttmp3;void*_tmp41;if(*((int*)_tmp40)== 1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)== 0){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)->f1)->f1).Rel_n).val == 0){_tmp41=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp40)->f1)->f1)->f2;{struct _fat_ptr*y=_tmp41;
# 333
{struct Cyc_List_List*_tmp42=tags;struct Cyc_List_List*ts=_tmp42;for(0;ts != 0;ts=ts->tl){
struct _tuple19*_tmp43=(struct _tuple19*)ts->hd;struct _tuple19*_stmttmp4=_tmp43;struct _tuple19*_tmp44=_stmttmp4;void*_tmp46;void*_tmp45;_tmp45=_tmp44->f1;_tmp46=_tmp44->f2;{struct _fat_ptr*x=_tmp45;void*i=_tmp46;
if(Cyc_strptrcmp(x,y)== 0)
return({void*_tmp6ED=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->tag=39,({void*_tmp6EC=Cyc_Tcutil_copy_type(i);_tmp47->f1=_tmp6EC;});_tmp47;});Cyc_Absyn_new_exp(_tmp6ED,e->loc);});}}}
# 338
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}
# 341
return e;}
# 346
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp48=t;void*_tmp50;void*_tmp4F;void*_tmp4E;void*_tmp4D;unsigned _tmp51;void*_tmp4C;void*_tmp4B;struct Cyc_Absyn_Tqual _tmp4A;void*_tmp49;switch(*((int*)_tmp48)){case 4: _tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).tq;_tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).num_elts;_tmp4C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).zero_term;_tmp51=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp48)->f1).zt_loc;{void*et=_tmp49;struct Cyc_Absyn_Tqual tq=_tmp4A;struct Cyc_Absyn_Exp*nelts=_tmp4B;void*zt=_tmp4C;unsigned ztloc=_tmp51;
# 349
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*_tmp52=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp52;
if(nelts != nelts2 || et != et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3: _tmp49=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).elt_tq;_tmp4B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).rgn;_tmp4C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).nullable;_tmp4D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).bounds;_tmp4E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).zero_term;_tmp4F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).ptrloc;_tmp50=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48)->f1).ptr_atts).released;{void*et=_tmp49;struct Cyc_Absyn_Tqual tq=_tmp4A;void*r=_tmp4B;void*n=_tmp4C;void*b=_tmp4D;void*zt=_tmp4E;struct Cyc_Absyn_PtrLoc*pl=_tmp4F;void*rel=_tmp50;
# 357
void*_tmp53=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp53;
void*_tmp54=Cyc_Parse_substitute_tags(tags,b);void*b2=_tmp54;
if(et2 != et || b2 != b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp634;_tmp634.elt_type=et2,_tmp634.elt_tq=tq,(_tmp634.ptr_atts).rgn=r,(_tmp634.ptr_atts).nullable=n,(_tmp634.ptr_atts).bounds=b2,(_tmp634.ptr_atts).zero_term=zt,(_tmp634.ptr_atts).ptrloc=pl,(_tmp634.ptr_atts).released=rel;_tmp634;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f1)== 13){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2)->tl == 0){_tmp49=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48)->f2)->hd;{void*t=_tmp49;
# 363
void*_tmp55=Cyc_Parse_substitute_tags(tags,t);void*t2=_tmp55;
if(t != t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9: _tmp49=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp48)->f1;{struct Cyc_Absyn_Exp*e=_tmp49;
# 367
struct Cyc_Absyn_Exp*_tmp56=Cyc_Parse_substitute_tags_exp(tags,e);struct Cyc_Absyn_Exp*e2=_tmp56;
if(e2 != e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9:
 goto _LL0;}_LL0:;}
# 372
return t;}
# 377
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp6EE=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp6EE;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 384
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple21*t){
return({struct _tuple20*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=(*t).f2,_tmp57->f2=(*t).f3;_tmp57;});}
# 388
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp58=tm;if(*((int*)_tmp58)== 4)
return 1;else{
return 0;};}
# 397
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(({struct _fat_ptr _tmp6EF=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6EF,({const char*_tmp59="`H";_tag_fat(_tmp59,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp6F0=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6F0,({const char*_tmp5A="`U";_tag_fat(_tmp5A,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp6F1=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6F1,({const char*_tmp5B="`RC";_tag_fat(_tmp5B,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp5D=_cycalloc(sizeof(*_tmp5D));({struct _fat_ptr*_tmp6F2=({struct _fat_ptr*_tmp5C=_cycalloc(sizeof(*_tmp5C));*_tmp5C=s;_tmp5C;});_tmp5D->name=_tmp6F2;}),_tmp5D->identity=- 1,_tmp5D->kind=k;_tmp5D;}));}
# 409
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(({struct _fat_ptr _tmp6F3=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6F3,({const char*_tmp5E="`H";_tag_fat(_tmp5E,sizeof(char),3U);}));})== 0)
({void*_tmp5F=0U;({unsigned _tmp6F5=loc;struct _fat_ptr _tmp6F4=({const char*_tmp60="bad occurrence of heap region";_tag_fat(_tmp60,sizeof(char),30U);});Cyc_Warn_err(_tmp6F5,_tmp6F4,_tag_fat(_tmp5F,sizeof(void*),0));});});
if(({struct _fat_ptr _tmp6F6=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6F6,({const char*_tmp61="`U";_tag_fat(_tmp61,sizeof(char),3U);}));})== 0)
({void*_tmp62=0U;({unsigned _tmp6F8=loc;struct _fat_ptr _tmp6F7=({const char*_tmp63="bad occurrence of unique region";_tag_fat(_tmp63,sizeof(char),32U);});Cyc_Warn_err(_tmp6F8,_tmp6F7,_tag_fat(_tmp62,sizeof(void*),0));});});
if(({struct _fat_ptr _tmp6F9=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp6F9,({const char*_tmp64="`RC";_tag_fat(_tmp64,sizeof(char),4U);}));})== 0)
({void*_tmp65=0U;({unsigned _tmp6FB=loc;struct _fat_ptr _tmp6FA=({const char*_tmp66="bad occurrence of refcounted region";_tag_fat(_tmp66,sizeof(char),36U);});Cyc_Warn_err(_tmp6FB,_tmp6FA,_tag_fat(_tmp65,sizeof(void*),0));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
({void*_tmp67=0U;({unsigned _tmp6FD=loc;struct _fat_ptr _tmp6FC=({const char*_tmp68="bad occurrence of \"current\" region";_tag_fat(_tmp68,sizeof(char),35U);});Cyc_Warn_err(_tmp6FD,_tmp6FC,_tag_fat(_tmp67,sizeof(void*),0));});});}
# 424
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp69=t;void*_tmp6A;if(*((int*)_tmp69)== 2){_tmp6A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;{struct Cyc_Absyn_Tvar*pr=_tmp6A;
return pr;}}else{
({unsigned _tmp6FE=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp6FE,({const char*_tmp6B="expecting a list of type variables, not types";_tag_fat(_tmp6B,sizeof(char),46U);}));});};}
# 432
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6C=Cyc_Absyn_compress(t);void*_stmttmp5=_tmp6C;void*_tmp6D=_stmttmp5;void*_tmp6E;if(*((int*)_tmp6D)== 2){_tmp6E=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1)->kind;{void**cptr=(void**)_tmp6E;
# 435
void*_tmp6F=Cyc_Kinds_compress_kb(*cptr);void*_stmttmp6=_tmp6F;void*_tmp70=_stmttmp6;if(*((int*)_tmp70)== 1){
# 437
({void*_tmp6FF=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2,_tmp71->f1=0,_tmp71->f2=k;_tmp71;}): Cyc_Kinds_kind_to_bound(k);*cptr=_tmp6FF;});
return;}else{
return;};}}else{
# 441
return;};}
# 446
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 454
if(tms == 0)return 0;{
# 456
void*_tmp72=(void*)tms->hd;void*_stmttmp7=_tmp72;void*_tmp73=_stmttmp7;void*_tmp74;if(*((int*)_tmp73)== 3){_tmp74=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp73)->f1;{void*args=_tmp74;
# 459
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 462
void*_tmp75=args;void*_tmp76;if(*((int*)_tmp75)== 1){
# 464
({void*_tmp77=0U;({unsigned _tmp701=loc;struct _fat_ptr _tmp700=({const char*_tmp78="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp78,sizeof(char),93U);});Cyc_Warn_warn(_tmp701,_tmp700,_tag_fat(_tmp77,sizeof(void*),0));});});
# 466
return tms;}else{_tmp76=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp75)->f1;{struct Cyc_List_List*ids=_tmp76;
# 468
if(({int _tmp702=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ids);_tmp702 != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(tds);}))
({unsigned _tmp703=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp703,({const char*_tmp79="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp79,sizeof(char),73U);}));});{
# 472
struct Cyc_List_List*rev_new_params=0;
for(1;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp7A=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp7A;
void*_tmp7B=x->r;void*_stmttmp8=_tmp7B;void*_tmp7C=_stmttmp8;void*_tmp7D;if(*((int*)_tmp7C)== 0){_tmp7D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7C)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp7D;
# 479
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({unsigned _tmp704=x->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp704,({const char*_tmp7E="initializer found in parameter declaration";_tag_fat(_tmp7E,sizeof(char),43U);}));});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({unsigned _tmp705=x->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp705,({const char*_tmp7F="namespaces forbidden in parameter declarations";_tag_fat(_tmp7F,sizeof(char),47U);}));});
rev_new_params=({struct Cyc_List_List*_tmp81=_cycalloc(sizeof(*_tmp81));({struct _tuple8*_tmp706=({struct _tuple8*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->f1=(*vd->name).f2,_tmp80->f2=vd->tq,_tmp80->f3=vd->type;_tmp80;});_tmp81->hd=_tmp706;}),_tmp81->tl=rev_new_params;_tmp81;});
# 487
goto L;}}else{
({unsigned _tmp707=x->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp707,({const char*_tmp82="nonvariable declaration in parameter type";_tag_fat(_tmp82,sizeof(char),42U);}));});};}
# 491
L: if(tds2 == 0)
({unsigned _tmp709=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp709,(struct _fat_ptr)({struct _fat_ptr _tmp708=(struct _fat_ptr)*((struct _fat_ptr*)ids->hd);Cyc_strconcat(_tmp708,({const char*_tmp83=" is not given a type";_tag_fat(_tmp83,sizeof(char),21U);}));}));});}
# 494
return({struct Cyc_List_List*_tmp86=_region_malloc(yy,sizeof(*_tmp86));
({void*_tmp70C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp85=_region_malloc(yy,sizeof(*_tmp85));_tmp85->tag=3,({void*_tmp70B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp84=_region_malloc(yy,sizeof(*_tmp84));_tmp84->tag=1,({struct Cyc_List_List*_tmp70A=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(rev_new_params);_tmp84->f1=_tmp70A;}),_tmp84->f2=0,_tmp84->f3=0,_tmp84->f4=0,_tmp84->f5=0,_tmp84->f6=0,_tmp84->f7=0;_tmp84;});_tmp85->f1=_tmp70B;});_tmp85;});_tmp86->hd=_tmp70C;}),_tmp86->tl=0;_tmp86;});}}};}
# 501
goto _LL4;}}else{_LL4:
 return({struct Cyc_List_List*_tmp87=_region_malloc(yy,sizeof(*_tmp87));_tmp87->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp70D=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp87->tl=_tmp70D;});_tmp87;});};}}
# 509
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 513
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp635;_tmp635.id=d.id,_tmp635.varloc=d.varloc,({struct Cyc_List_List*_tmp70E=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp635.tms=_tmp70E;});_tmp635;});{
# 516
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 522
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;{
# 528
enum Cyc_Parse_Storage_class _tmp88=dso->sc;enum Cyc_Parse_Storage_class _stmttmp9=_tmp88;enum Cyc_Parse_Storage_class _tmp89=_stmttmp9;switch((int)_tmp89){case Cyc_Parse_None_sc:
 goto _LL0;case Cyc_Parse_Extern_sc:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc:
 sc=0U;goto _LL0;default:
({void*_tmp8A=0U;({unsigned _tmp710=loc;struct _fat_ptr _tmp70F=({const char*_tmp8B="bad storage class on function";_tag_fat(_tmp8B,sizeof(char),30U);});Cyc_Warn_err(_tmp710,_tmp70F,_tag_fat(_tmp8A,sizeof(void*),0));});});goto _LL0;}_LL0:;}}{
# 535
void*_tmp8C=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp8C;
struct _tuple14 _tmp8D=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpA=_tmp8D;struct _tuple14 _tmp8E=_stmttmpA;void*_tmp92;void*_tmp91;void*_tmp90;struct Cyc_Absyn_Tqual _tmp8F;_tmp8F=_tmp8E.f1;_tmp90=_tmp8E.f2;_tmp91=_tmp8E.f3;_tmp92=_tmp8E.f4;{struct Cyc_Absyn_Tqual fn_tqual=_tmp8F;void*fn_type=_tmp90;struct Cyc_List_List*x=_tmp91;struct Cyc_List_List*out_atts=_tmp92;
# 540
if(x != 0)
({void*_tmp93=0U;({unsigned _tmp712=loc;struct _fat_ptr _tmp711=({const char*_tmp94="bad type params, ignoring";_tag_fat(_tmp94,sizeof(char),26U);});Cyc_Warn_warn(_tmp712,_tmp711,_tag_fat(_tmp93,sizeof(void*),0));});});{
# 543
void*_tmp95=fn_type;struct Cyc_Absyn_FnInfo _tmp96;if(*((int*)_tmp95)== 5){_tmp96=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95)->f1;{struct Cyc_Absyn_FnInfo i=_tmp96;
# 545
{struct Cyc_List_List*_tmp97=i.args;struct Cyc_List_List*args2=_tmp97;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmp98=0U;({unsigned _tmp714=loc;struct _fat_ptr _tmp713=({const char*_tmp99="missing argument variable in function prototype";_tag_fat(_tmp99,sizeof(char),48U);});Cyc_Warn_err(_tmp714,_tmp713,_tag_fat(_tmp98,sizeof(void*),0));});});
({struct _fat_ptr*_tmp716=({struct _fat_ptr*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct _fat_ptr _tmp715=({const char*_tmp9A="?";_tag_fat(_tmp9A,sizeof(char),2U);});*_tmp9B=_tmp715;});_tmp9B;});(*((struct _tuple8*)args2->hd)).f1=_tmp716;});}}}
# 552
({struct Cyc_List_List*_tmp717=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp717;});
return({struct Cyc_Absyn_Fndecl*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->sc=sc,_tmp9C->is_inline=is_inline,_tmp9C->name=d.id,_tmp9C->body=body,_tmp9C->i=i,_tmp9C->cached_type=0,_tmp9C->param_vardecls=0,_tmp9C->fn_vardecl=0,_tmp9C->orig_scope=sc;_tmp9C;});}}else{
# 556
({unsigned _tmp718=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp718,({const char*_tmp9D="declarator is not a function prototype";_tag_fat(_tmp9D,sizeof(char),39U);}));});};}}}}}static char _tmp9E[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 560
static struct _fat_ptr Cyc_Parse_msg1={_tmp9E,_tmp9E,_tmp9E + 76U};static char _tmp9F[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 562
static struct _fat_ptr Cyc_Parse_msg2={_tmp9F,_tmp9F,_tmp9F + 84U};
# 569
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 572
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpA0=0U;({unsigned _tmp71A=loc;struct _fat_ptr _tmp719=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp71A,_tmp719,_tag_fat(_tmpA0,sizeof(void*),0));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpA1=0U;({unsigned _tmp71C=loc;struct _fat_ptr _tmp71B=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp71C,_tmp71B,_tag_fat(_tmpA1,sizeof(void*),0));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpA2=0U;({unsigned _tmp71E=loc;struct _fat_ptr _tmp71D=({const char*_tmpA3="too many occurrences of short in specifiers";_tag_fat(_tmpA3,sizeof(char),44U);});Cyc_Warn_warn(_tmp71E,_tmp71D,_tag_fat(_tmpA2,sizeof(void*),0));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpA4=0U;({unsigned _tmp720=loc;struct _fat_ptr _tmp71F=({const char*_tmpA5="too many occurrences of long in specifiers";_tag_fat(_tmpA5,sizeof(char),43U);});Cyc_Warn_warn(_tmp720,_tmp71F,_tag_fat(_tmpA4,sizeof(void*),0));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpA6=0U;({unsigned _tmp722=loc;struct _fat_ptr _tmp721=Cyc_Parse_msg1;Cyc_Warn_err(_tmp722,_tmp721,_tag_fat(_tmpA6,sizeof(void*),0));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 594
return s1;}
# 600
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned loc){
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec || ts.Unsigned_spec;
int seen_size=(ts.Short_spec || ts.Long_spec)|| ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 608
if(seen_size || seen_sign){
if(ts.Signed_spec && ts.Unsigned_spec)
({void*_tmpA7=0U;({unsigned _tmp724=loc;struct _fat_ptr _tmp723=Cyc_Parse_msg2;Cyc_Warn_err(_tmp724,_tmp723,_tag_fat(_tmpA7,sizeof(void*),0));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpA8=0U;({unsigned _tmp726=loc;struct _fat_ptr _tmp725=Cyc_Parse_msg2;Cyc_Warn_err(_tmp726,_tmp725,_tag_fat(_tmpA8,sizeof(void*),0));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 621
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpA9=0U;({unsigned _tmp728=loc;struct _fat_ptr _tmp727=({const char*_tmpAA="missing type within specifier";_tag_fat(_tmpAA,sizeof(char),30U);});Cyc_Warn_warn(_tmp728,_tmp727,_tag_fat(_tmpA9,sizeof(void*),0));});});
return Cyc_Absyn_int_type(sgn,sz);}
# 626
{void*_tmpAB=t;enum Cyc_Absyn_Size_of _tmpAD;enum Cyc_Absyn_Sign _tmpAC;if(*((int*)_tmpAB)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAB)->f1)){case 1: _tmpAC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAB)->f1)->f1;_tmpAD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpAB)->f1)->f2;{enum Cyc_Absyn_Sign sgn2=_tmpAC;enum Cyc_Absyn_Size_of sz2=_tmpAD;
# 628
if(seen_sign &&(int)sgn2 != (int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 632
if(seen_size &&(int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2:
# 636
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5:
# 640
 if(seen_sign)
({void*_tmpAE=0U;({unsigned _tmp72A=loc;struct _fat_ptr _tmp729=({const char*_tmpAF="sign specification on non-integral type";_tag_fat(_tmpAF,sizeof(char),40U);});Cyc_Warn_err(_tmp72A,_tmp729,_tag_fat(_tmpAE,sizeof(void*),0));});});
if(seen_size)
({void*_tmpB0=0U;({unsigned _tmp72C=loc;struct _fat_ptr _tmp72B=({const char*_tmpB1="size qualifier on non-integral type";_tag_fat(_tmpB1,sizeof(char),36U);});Cyc_Warn_err(_tmp72C,_tmp72B,_tag_fat(_tmpB0,sizeof(void*),0));});});
goto _LL0;}_LL0:;}
# 646
return t;}
# 649
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 653
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpB2=d.id;struct _tuple0*q=_tmpB2;
unsigned _tmpB3=d.varloc;unsigned varloc=_tmpB3;
struct _tuple14 _tmpB4=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpB=_tmpB4;struct _tuple14 _tmpB5=_stmttmpB;void*_tmpB9;void*_tmpB8;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;_tmpB6=_tmpB5.f1;_tmpB7=_tmpB5.f2;_tmpB8=_tmpB5.f3;_tmpB9=_tmpB5.f4;{struct Cyc_Absyn_Tqual tq2=_tmpB6;void*new_typ=_tmpB7;struct Cyc_List_List*tvs=_tmpB8;struct Cyc_List_List*atts=_tmpB9;
# 660
struct Cyc_List_List*_tmpBA=ds->tl == 0?0:({struct _RegionHandle*_tmp730=r;struct Cyc_Absyn_Tqual _tmp72F=tq;void*_tmp72E=Cyc_Tcutil_copy_type(t);struct _tuple13*_tmp72D=ds->tl;Cyc_Parse_apply_tmss(_tmp730,_tmp72F,_tmp72E,_tmp72D,shared_atts);});struct Cyc_List_List*tl=_tmpBA;
return({struct Cyc_List_List*_tmpBC=_region_malloc(r,sizeof(*_tmpBC));({struct _tuple16*_tmp731=({struct _tuple16*_tmpBB=_region_malloc(r,sizeof(*_tmpBB));_tmpBB->f1=varloc,_tmpBB->f2=q,_tmpBB->f3=tq2,_tmpBB->f4=new_typ,_tmpBB->f5=tvs,_tmpBB->f6=atts;_tmpBB;});_tmpBC->hd=_tmp731;}),_tmpBC->tl=tl;_tmpBC;});}}}
# 664
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 667
if(tms == 0)return({struct _tuple14 _tmp636;_tmp636.f1=tq,_tmp636.f2=t,_tmp636.f3=0,_tmp636.f4=atts;_tmp636;});{
void*_tmpBD=(void*)tms->hd;void*_stmttmpC=_tmpBD;void*_tmpBE=_stmttmpC;struct Cyc_Absyn_Tqual _tmpC2;struct Cyc_Absyn_PtrAtts _tmpC1;void*_tmpC3;unsigned _tmpBF;void*_tmpC0;switch(*((int*)_tmpBE)){case 0: _tmpC0=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpBF=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{void*zeroterm=_tmpC0;unsigned ztloc=_tmpBF;
# 670
return({struct Cyc_Absyn_Tqual _tmp734=Cyc_Absyn_empty_tqual(0U);void*_tmp733=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 670
struct Cyc_List_List*_tmp732=atts;Cyc_Parse_apply_tms(_tmp734,_tmp733,_tmp732,tms->tl);});}case 1: _tmpC0=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpC3=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;_tmpBF=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpBE)->f3;{struct Cyc_Absyn_Exp*e=_tmpC0;void*zeroterm=_tmpC3;unsigned ztloc=_tmpBF;
# 673
return({struct Cyc_Absyn_Tqual _tmp737=Cyc_Absyn_empty_tqual(0U);void*_tmp736=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 673
struct Cyc_List_List*_tmp735=atts;Cyc_Parse_apply_tms(_tmp737,_tmp736,_tmp735,tms->tl);});}case 3: _tmpC0=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;{void*args=_tmpC0;
# 676
void*_tmpC4=args;unsigned _tmpC5;void*_tmpCC;void*_tmpCB;void*_tmpCA;void*_tmpC9;void*_tmpC8;int _tmpC7;void*_tmpC6;if(*((int*)_tmpC4)== 1){_tmpC6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f1;_tmpC7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f2;_tmpC8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f3;_tmpC9=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f4;_tmpCA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f5;_tmpCB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f6;_tmpCC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpC4)->f7;{struct Cyc_List_List*args2=_tmpC6;int c_vararg=_tmpC7;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpC8;void*eff=_tmpC9;struct Cyc_List_List*rgn_po=_tmpCA;struct Cyc_Absyn_Exp*req=_tmpCB;struct Cyc_Absyn_Exp*ens=_tmpCC;
# 678
struct Cyc_List_List*typvars=0;
# 680
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(void*)as->hd,_tmpCD->tl=fn_atts;_tmpCD;});else{
# 685
new_atts=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=(void*)as->hd,_tmpCE->tl=new_atts;_tmpCE;});}}}
# 687
if(tms->tl != 0){
void*_tmpCF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpD=_tmpCF;void*_tmpD0=_stmttmpD;void*_tmpD1;if(*((int*)_tmpD0)== 4){_tmpD1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->f1;{struct Cyc_List_List*ts=_tmpD1;
# 690
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{
goto _LL12;}_LL12:;}
# 696
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 701
args2=0;{
# 704
struct Cyc_List_List*_tmpD2=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpD2;
# 706
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 711
{struct Cyc_List_List*_tmpD3=args2;struct Cyc_List_List*a=_tmpD3;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpD4=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpE=_tmpD4;struct _tuple8*_tmpD5=_stmttmpE;void*_tmpD8;struct Cyc_Absyn_Tqual _tmpD7;void*_tmpD6;_tmpD6=_tmpD5->f1;_tmpD7=_tmpD5->f2;_tmpD8=(void**)& _tmpD5->f3;{struct _fat_ptr*vopt=_tmpD6;struct Cyc_Absyn_Tqual tq=_tmpD7;void**t=(void**)_tmpD8;
if(tags != 0)
({void*_tmp738=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp738;});
({void*_tmp739=Cyc_Parse_array2ptr(*t,1);*t=_tmp739;});}}}
# 723
return({struct Cyc_Absyn_Tqual _tmp73C=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp73B=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 723
struct Cyc_List_List*_tmp73A=new_atts;Cyc_Parse_apply_tms(_tmp73C,_tmp73B,_tmp73A,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_tmpC5=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpC4)->f2;{unsigned loc=_tmpC5;
# 730
({unsigned _tmp73D=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp73D,({const char*_tmpD9="function declaration without parameter types";_tag_fat(_tmpD9,sizeof(char),45U);}));});}};}case 4: _tmpC0=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpBF=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{struct Cyc_List_List*ts=_tmpC0;unsigned loc=_tmpBF;
# 737
if(tms->tl == 0)
return({struct _tuple14 _tmp637;_tmp637.f1=tq,_tmp637.f2=t,_tmp637.f3=ts,_tmp637.f4=atts;_tmp637;});
# 742
({unsigned _tmp73E=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp73E,({const char*_tmpDA="type parameters must appear before function arguments in declarator";_tag_fat(_tmpDA,sizeof(char),68U);}));});}case 2: _tmpC1=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpC2=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{struct Cyc_Absyn_PtrAtts ptratts=_tmpC1;struct Cyc_Absyn_Tqual tq2=_tmpC2;
# 745
return({struct Cyc_Absyn_Tqual _tmp741=tq2;void*_tmp740=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp638;_tmp638.elt_type=t,_tmp638.elt_tq=tq,_tmp638.ptr_atts=ptratts;_tmp638;}));struct Cyc_List_List*_tmp73F=atts;Cyc_Parse_apply_tms(_tmp741,_tmp740,_tmp73F,tms->tl);});}default: _tmpBF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpBE)->f1;_tmpC0=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpBE)->f2;{unsigned loc=_tmpBF;struct Cyc_List_List*atts2=_tmpC0;
# 750
return({struct Cyc_Absyn_Tqual _tmp744=tq;void*_tmp743=t;struct Cyc_List_List*_tmp742=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp744,_tmp743,_tmp742,tms->tl);});}};}}
# 756
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 765
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpDB=t;void*_tmpE1;void*_tmpE0;void*_tmpDF;struct Cyc_Absyn_Tqual _tmpDE;void*_tmpDD;unsigned _tmpDC;_tmpDC=_tmpDB->f1;_tmpDD=_tmpDB->f2;_tmpDE=_tmpDB->f3;_tmpDF=_tmpDB->f4;_tmpE0=_tmpDB->f5;_tmpE1=_tmpDB->f6;{unsigned varloc=_tmpDC;struct _tuple0*x=_tmpDD;struct Cyc_Absyn_Tqual tq=_tmpDE;void*typ=_tmpDF;struct Cyc_List_List*tvs=_tmpE0;struct Cyc_List_List*atts=_tmpE1;
# 768
Cyc_Lex_register_typedef(x);{
# 770
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpE2=typ;void*_tmpE3;if(*((int*)_tmpE2)== 1){_tmpE3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpE2)->f1;{struct Cyc_Core_Opt*kopt=_tmpE3;
# 774
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{
kind=0;type=typ;goto _LL3;}_LL3:;}
# 779
return({void*_tmp746=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->tag=8,({struct Cyc_Absyn_Typedefdecl*_tmp745=({struct Cyc_Absyn_Typedefdecl*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->name=x,_tmpE4->tvs=tvs,_tmpE4->kind=kind,_tmpE4->defn=type,_tmpE4->atts=atts,_tmpE4->tq=tq,_tmpE4->extern_c=0;_tmpE4;});_tmpE5->f1=_tmp745;});_tmpE5;});Cyc_Absyn_new_decl(_tmp746,loc);});}}}
# 786
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp747=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->tag=12,_tmpE6->f1=d,_tmpE6->f2=s;_tmpE6;});Cyc_Absyn_new_stmt(_tmp747,d->loc);});}
# 790
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 794
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 797
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds != 0;ds=ds->tl){
struct _tuple12 _tmpE7=ds->hd;struct _tuple12 _stmttmpF=_tmpE7;struct _tuple12 _tmpE8=_stmttmpF;void*_tmpEA;struct Cyc_Parse_Declarator _tmpE9;_tmpE9=_tmpE8.f1;_tmpEA=_tmpE8.f2;{struct Cyc_Parse_Declarator d=_tmpE9;struct Cyc_Absyn_Exp*e=_tmpEA;
declarators=({struct _tuple13*_tmpEB=_region_malloc(r,sizeof(*_tmpEB));_tmpEB->tl=declarators,_tmpEB->hd=d;_tmpEB;});
exprs=({struct Cyc_List_List*_tmpEC=_region_malloc(r,sizeof(*_tmpEC));_tmpEC->hd=e,_tmpEC->tl=exprs;_tmpEC;});}}
# 804
({struct Cyc_List_List*_tmp748=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(exprs);*es=_tmp748;});
({struct _tuple13*_tmp749=((struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp749;});}
# 813
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 816
struct _RegionHandle _tmpED=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpED;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpEE=ds;void*_tmpF1;struct Cyc_Parse_Type_specifier _tmpF0;struct Cyc_Absyn_Tqual _tmpEF;_tmpEF=_tmpEE.tq;_tmpF0=_tmpEE.type_specs;_tmpF1=_tmpEE.attributes;{struct Cyc_Absyn_Tqual tq=_tmpEF;struct Cyc_Parse_Type_specifier tss=_tmpF0;struct Cyc_List_List*atts=_tmpF1;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmpF2=0U;({unsigned _tmp74B=loc;struct _fat_ptr _tmp74A=({const char*_tmpF3="inline qualifier on non-function definition";_tag_fat(_tmpF3,sizeof(char),44U);});Cyc_Warn_warn(_tmp74B,_tmp74A,_tag_fat(_tmpF2,sizeof(void*),0));});});{
# 822
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _tmpF4=ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmpF4;enum Cyc_Parse_Storage_class _tmpF5=_stmttmp10;switch((int)_tmpF5){case Cyc_Parse_Typedef_sc:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc:
 s=Cyc_Flags_no_register?2U: 5U;goto _LL3;case Cyc_Parse_Abstract_sc:
 s=1U;goto _LL3;default:
 goto _LL3;}_LL3:;}{
# 838
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 842
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 850
void*_tmpF6=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmpF6;
if(declarators == 0){
# 854
void*_tmpF7=base_type;int _tmpF9;void*_tmpFA;enum Cyc_Absyn_AggrKind _tmpFB;void*_tmpF8;switch(*((int*)_tmpF7)){case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpF7)->f1)->r)){case 0: _tmpF8=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpF7)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpF8;
# 856
({struct Cyc_List_List*_tmp74C=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp74C;});
ad->sc=s;{
struct Cyc_List_List*_tmpFE=({struct Cyc_List_List*_tmpFD=_cycalloc(sizeof(*_tmpFD));({struct Cyc_Absyn_Decl*_tmp74E=({void*_tmp74D=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=5,_tmpFC->f1=ad;_tmpFC;});Cyc_Absyn_new_decl(_tmp74D,loc);});_tmpFD->hd=_tmp74E;}),_tmpFD->tl=0;_tmpFD;});_npop_handler(0);return _tmpFE;}}case 1: _tmpF8=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpF7)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmpF8;
# 860
if(atts != 0)({void*_tmpFF=0U;({unsigned _tmp750=loc;struct _fat_ptr _tmp74F=({const char*_tmp100="attributes on enum not supported";_tag_fat(_tmp100,sizeof(char),33U);});Cyc_Warn_err(_tmp750,_tmp74F,_tag_fat(_tmpFF,sizeof(void*),0));});});
ed->sc=s;{
struct Cyc_List_List*_tmp103=({struct Cyc_List_List*_tmp102=_cycalloc(sizeof(*_tmp102));({struct Cyc_Absyn_Decl*_tmp752=({void*_tmp751=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=7,_tmp101->f1=ed;_tmp101;});Cyc_Absyn_new_decl(_tmp751,loc);});_tmp102->hd=_tmp752;}),_tmp102->tl=0;_tmp102;});_npop_handler(0);return _tmp103;}}default: _tmpF8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmpF7)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmpF8;
# 864
if(atts != 0)({void*_tmp104=0U;({unsigned _tmp754=loc;struct _fat_ptr _tmp753=({const char*_tmp105="attributes on datatypes not supported";_tag_fat(_tmp105,sizeof(char),38U);});Cyc_Warn_err(_tmp754,_tmp753,_tag_fat(_tmp104,sizeof(void*),0));});});
dd->sc=s;{
struct Cyc_List_List*_tmp108=({struct Cyc_List_List*_tmp107=_cycalloc(sizeof(*_tmp107));({struct Cyc_Absyn_Decl*_tmp756=({void*_tmp755=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=6,_tmp106->f1=dd;_tmp106;});Cyc_Absyn_new_decl(_tmp755,loc);});_tmp107->hd=_tmp756;}),_tmp107->tl=0;_tmp107;});_npop_handler(0);return _tmp108;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).UnknownAggr).tag == 1){_tmpFB=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).UnknownAggr).val).f1;_tmpF8=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).UnknownAggr).val).f2;_tmpFA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f2;{enum Cyc_Absyn_AggrKind k=_tmpFB;struct _tuple0*n=_tmpF8;struct Cyc_List_List*ts=_tmpFA;
# 868
struct Cyc_List_List*_tmp109=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp109;
struct Cyc_Absyn_Aggrdecl*_tmp10A=({struct Cyc_Absyn_Aggrdecl*_tmp110=_cycalloc(sizeof(*_tmp110));_tmp110->kind=k,_tmp110->sc=s,_tmp110->name=n,_tmp110->tvs=ts2,_tmp110->impl=0,_tmp110->attributes=0,_tmp110->expected_mem_kind=0;_tmp110;});struct Cyc_Absyn_Aggrdecl*ad=_tmp10A;
if(atts != 0)({void*_tmp10B=0U;({unsigned _tmp758=loc;struct _fat_ptr _tmp757=({const char*_tmp10C="bad attributes on type declaration";_tag_fat(_tmp10C,sizeof(char),35U);});Cyc_Warn_err(_tmp758,_tmp757,_tag_fat(_tmp10B,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp10F=({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));({struct Cyc_Absyn_Decl*_tmp75A=({void*_tmp759=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D->tag=5,_tmp10D->f1=ad;_tmp10D;});Cyc_Absyn_new_decl(_tmp759,loc);});_tmp10E->hd=_tmp75A;}),_tmp10E->tl=0;_tmp10E;});_npop_handler(0);return _tmp10F;}}}else{goto _LL25;}case 18: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatype).tag == 2){_tmpF8=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).KnownDatatype).val;{struct Cyc_Absyn_Datatypedecl**tudp=_tmpF8;
# 873
if(atts != 0)({void*_tmp111=0U;({unsigned _tmp75C=loc;struct _fat_ptr _tmp75B=({const char*_tmp112="bad attributes on datatype";_tag_fat(_tmp112,sizeof(char),27U);});Cyc_Warn_err(_tmp75C,_tmp75B,_tag_fat(_tmp111,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp115=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));({struct Cyc_Absyn_Decl*_tmp75E=({void*_tmp75D=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=6,_tmp113->f1=*tudp;_tmp113;});Cyc_Absyn_new_decl(_tmp75D,loc);});_tmp114->hd=_tmp75E;}),_tmp114->tl=0;_tmp114;});_npop_handler(0);return _tmp115;}}}else{_tmpF8=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).UnknownDatatype).val).name;_tmpF9=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1).UnknownDatatype).val).is_extensible;_tmpFA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f2;{struct _tuple0*n=_tmpF8;int isx=_tmpF9;struct Cyc_List_List*ts=_tmpFA;
# 876
struct Cyc_List_List*_tmp116=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp116;
struct Cyc_Absyn_Decl*_tmp117=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp117;
if(atts != 0)({void*_tmp118=0U;({unsigned _tmp760=loc;struct _fat_ptr _tmp75F=({const char*_tmp119="bad attributes on datatype";_tag_fat(_tmp119,sizeof(char),27U);});Cyc_Warn_err(_tmp760,_tmp75F,_tag_fat(_tmp118,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp11B=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->hd=tud,_tmp11A->tl=0;_tmp11A;});_npop_handler(0);return _tmp11B;}}}case 15: _tmpF8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1;{struct _tuple0*n=_tmpF8;
# 881
struct Cyc_Absyn_Enumdecl*_tmp11C=({struct Cyc_Absyn_Enumdecl*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->sc=s,_tmp123->name=n,_tmp123->fields=0;_tmp123;});struct Cyc_Absyn_Enumdecl*ed=_tmp11C;
if(atts != 0)({void*_tmp11D=0U;({unsigned _tmp762=loc;struct _fat_ptr _tmp761=({const char*_tmp11E="bad attributes on enum";_tag_fat(_tmp11E,sizeof(char),23U);});Cyc_Warn_err(_tmp762,_tmp761,_tag_fat(_tmp11D,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp122=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Absyn_Decl*_tmp764=({struct Cyc_Absyn_Decl*_tmp120=_cycalloc(sizeof(*_tmp120));({void*_tmp763=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=7,_tmp11F->f1=ed;_tmp11F;});_tmp120->r=_tmp763;}),_tmp120->loc=loc;_tmp120;});_tmp121->hd=_tmp764;}),_tmp121->tl=0;_tmp121;});_npop_handler(0);return _tmp122;}}case 16: _tmpF8=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF7)->f1)->f1;{struct Cyc_List_List*fs=_tmpF8;
# 887
struct Cyc_Absyn_Enumdecl*_tmp124=({struct Cyc_Absyn_Enumdecl*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->sc=s,({struct _tuple0*_tmp766=Cyc_Parse_gensym_enum();_tmp12C->name=_tmp766;}),({struct Cyc_Core_Opt*_tmp765=({struct Cyc_Core_Opt*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->v=fs;_tmp12B;});_tmp12C->fields=_tmp765;});_tmp12C;});struct Cyc_Absyn_Enumdecl*ed=_tmp124;
if(atts != 0)({void*_tmp125=0U;({unsigned _tmp768=loc;struct _fat_ptr _tmp767=({const char*_tmp126="bad attributes on enum";_tag_fat(_tmp126,sizeof(char),23U);});Cyc_Warn_err(_tmp768,_tmp767,_tag_fat(_tmp125,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp12A=({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));({struct Cyc_Absyn_Decl*_tmp76A=({struct Cyc_Absyn_Decl*_tmp128=_cycalloc(sizeof(*_tmp128));({void*_tmp769=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=7,_tmp127->f1=ed;_tmp127;});_tmp128->r=_tmp769;}),_tmp128->loc=loc;_tmp128;});_tmp129->hd=_tmp76A;}),_tmp129->tl=0;_tmp129;});_npop_handler(0);return _tmp12A;}}default: goto _LL25;}default: _LL25:
({void*_tmp12D=0U;({unsigned _tmp76C=loc;struct _fat_ptr _tmp76B=({const char*_tmp12E="missing declarator";_tag_fat(_tmp12E,sizeof(char),19U);});Cyc_Warn_err(_tmp76C,_tmp76B,_tag_fat(_tmp12D,sizeof(void*),0));});});{struct Cyc_List_List*_tmp12F=0;_npop_handler(0);return _tmp12F;}};}{
# 894
struct Cyc_List_List*_tmp130=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp130;
if(istypedef){
# 899
if(!exps_empty)
({void*_tmp131=0U;({unsigned _tmp76E=loc;struct _fat_ptr _tmp76D=({const char*_tmp132="initializer in typedef declaration";_tag_fat(_tmp132,sizeof(char),35U);});Cyc_Warn_err(_tmp76E,_tmp76D,_tag_fat(_tmp131,sizeof(void*),0));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple16*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp133=decls;_npop_handler(0);return _tmp133;}}{
# 905
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp134=fields;struct Cyc_List_List*ds=_tmp134;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp135=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp135;struct _tuple16*_tmp136=_stmttmp11;void*_tmp13C;void*_tmp13B;void*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;void*_tmp138;unsigned _tmp137;_tmp137=_tmp136->f1;_tmp138=_tmp136->f2;_tmp139=_tmp136->f3;_tmp13A=_tmp136->f4;_tmp13B=_tmp136->f5;_tmp13C=_tmp136->f6;{unsigned varloc=_tmp137;struct _tuple0*x=_tmp138;struct Cyc_Absyn_Tqual tq2=_tmp139;void*t2=_tmp13A;struct Cyc_List_List*tvs2=_tmp13B;struct Cyc_List_List*atts2=_tmp13C;
if(tvs2 != 0)
({void*_tmp13D=0U;({unsigned _tmp770=loc;struct _fat_ptr _tmp76F=({const char*_tmp13E="bad type params, ignoring";_tag_fat(_tmp13E,sizeof(char),26U);});Cyc_Warn_warn(_tmp770,_tmp76F,_tag_fat(_tmp13D,sizeof(void*),0));});});
if(exprs == 0)
({unsigned _tmp771=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp771,({const char*_tmp13F="unexpected NULL in parse!";_tag_fat(_tmp13F,sizeof(char),26U);}));});{
struct Cyc_Absyn_Vardecl*_tmp140=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd);struct Cyc_Absyn_Vardecl*vd=_tmp140;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));({struct Cyc_Absyn_Decl*_tmp773=({struct Cyc_Absyn_Decl*_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp772=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->tag=0,_tmp141->f1=vd;_tmp141;});_tmp142->r=_tmp772;}),_tmp142->loc=loc;_tmp142;});_tmp143->hd=_tmp773;}),_tmp143->tl=decls;_tmp143;});}}}}{
# 918
struct Cyc_List_List*_tmp144=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp144;}}}}}}}}}
# 817
;_pop_region();}
# 922
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp145=x;long long _tmp146;char _tmp147;int _tmp148;switch((_tmp145.LongLong_c).tag){case 5: _tmp148=((_tmp145.Int_c).val).f2;{int i=_tmp148;
return(unsigned)i;}case 2: _tmp147=((_tmp145.Char_c).val).f2;{char c=_tmp147;
return(unsigned)c;}case 6: _tmp146=((_tmp145.LongLong_c).val).f2;{long long x=_tmp146;
# 927
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp149=0U;({unsigned _tmp775=loc;struct _fat_ptr _tmp774=({const char*_tmp14A="integer constant too large";_tag_fat(_tmp14A,sizeof(char),27U);});Cyc_Warn_err(_tmp775,_tmp774,_tag_fat(_tmp149,sizeof(void*),0));});});
return(unsigned)x;}default:
# 932
({struct Cyc_String_pa_PrintArg_struct _tmp14D=({struct Cyc_String_pa_PrintArg_struct _tmp639;_tmp639.tag=0,({struct _fat_ptr _tmp776=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp639.f1=_tmp776;});_tmp639;});void*_tmp14B[1];_tmp14B[0]=& _tmp14D;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp14C="expected integer constant but found %s";_tag_fat(_tmp14C,sizeof(char),39U);});Cyc_Warn_err(_tmp778,_tmp777,_tag_fat(_tmp14B,sizeof(void*),1));});});
return 0U;};}
# 938
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp14E=p->r;void*_stmttmp12=_tmp14E;void*_tmp14F=_stmttmp12;void*_tmp151;struct _fat_ptr _tmp152;char _tmp154;int _tmp153;enum Cyc_Absyn_Sign _tmp155;void*_tmp150;switch(*((int*)_tmp14F)){case 15: _tmp150=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;{struct _tuple0*x=_tmp150;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp14F)->f2)->r)== 0){_tmp150=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp150;
# 942
return({struct Cyc_Absyn_Exp*_tmp779=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp779,p->loc);});}}else{goto _LL13;}case 6: _tmp150=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;{struct Cyc_Absyn_Pat*p2=_tmp150;
return({struct Cyc_Absyn_Exp*_tmp77A=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp77A,p->loc);});}case 9:
 return Cyc_Absyn_null_exp(p->loc);case 10: _tmp155=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;_tmp153=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp14F)->f2;{enum Cyc_Absyn_Sign s=_tmp155;int i=_tmp153;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11: _tmp154=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;{char c=_tmp154;
return Cyc_Absyn_char_exp(c,p->loc);}case 12: _tmp152=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;_tmp153=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp14F)->f2;{struct _fat_ptr s=_tmp152;int i=_tmp153;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp14F)->f3 == 0){_tmp150=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;_tmp151=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp14F)->f2;{struct _tuple0*x=_tmp150;struct Cyc_List_List*ps=_tmp151;
# 949
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17: _tmp150=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp14F)->f1;{struct Cyc_Absyn_Exp*e=_tmp150;
return e;}default: _LL13:
# 954
({void*_tmp156=0U;({unsigned _tmp77C=p->loc;struct _fat_ptr _tmp77B=({const char*_tmp157="cannot mix patterns and expressions in case";_tag_fat(_tmp157,sizeof(char),44U);});Cyc_Warn_err(_tmp77C,_tmp77B,_tag_fat(_tmp156,sizeof(void*),0));});});
return Cyc_Absyn_null_exp(p->loc);};}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f1;enum Cyc_Absyn_AggrKind f2;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _tuple26{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY29{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple27 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _tuple29{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY54{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY59{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY60{int tag;void*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY61{int tag;struct _tuple30*val;};struct _tuple31{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY62{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY63{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY64{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY65{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1040
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->tag=Cyc_Core_Failure,_tmp158->f1=s;_tmp158;}));}static char _tmp15B[7U]="cnst_t";
# 1009 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp15B,_tmp15B,_tmp15B + 7U};
union Cyc_YYSTYPE*_tmp159=yy1;union Cyc_Absyn_Cnst _tmp15A;if((((union Cyc_YYSTYPE*)_tmp159)->Int_tok).tag == 1){_tmp15A=(_tmp159->Int_tok).val;{union Cyc_Absyn_Cnst yy=_tmp15A;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1016
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp63A;(_tmp63A.Int_tok).tag=1U,(_tmp63A.Int_tok).val=yy1;_tmp63A;});}static char _tmp15E[5U]="char";
# 1010 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp15E,_tmp15E,_tmp15E + 5U};
union Cyc_YYSTYPE*_tmp15C=yy1;char _tmp15D;if((((union Cyc_YYSTYPE*)_tmp15C)->Char_tok).tag == 2){_tmp15D=(_tmp15C->Char_tok).val;{char yy=_tmp15D;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1017
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp63B;(_tmp63B.Char_tok).tag=2U,(_tmp63B.Char_tok).val=yy1;_tmp63B;});}static char _tmp161[13U]="string_t<`H>";
# 1011 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp161,_tmp161,_tmp161 + 13U};
union Cyc_YYSTYPE*_tmp15F=yy1;struct _fat_ptr _tmp160;if((((union Cyc_YYSTYPE*)_tmp15F)->String_tok).tag == 3){_tmp160=(_tmp15F->String_tok).val;{struct _fat_ptr yy=_tmp160;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1018
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp63C;(_tmp63C.String_tok).tag=3U,(_tmp63C.String_tok).val=yy1;_tmp63C;});}static char _tmp164[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1014 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp164,_tmp164,_tmp164 + 35U};
union Cyc_YYSTYPE*_tmp162=yy1;void*_tmp163;if((((union Cyc_YYSTYPE*)_tmp162)->YY1).tag == 7){_tmp163=(_tmp162->YY1).val;{struct _tuple22*yy=_tmp163;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1021
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp63D;(_tmp63D.YY1).tag=7U,(_tmp63D.YY1).val=yy1;_tmp63D;});}static char _tmp167[11U]="ptrbound_t";
# 1015 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp167,_tmp167,_tmp167 + 11U};
union Cyc_YYSTYPE*_tmp165=yy1;void*_tmp166;if((((union Cyc_YYSTYPE*)_tmp165)->YY2).tag == 8){_tmp166=(_tmp165->YY2).val;{void*yy=_tmp166;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1022
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp63E;(_tmp63E.YY2).tag=8U,(_tmp63E.YY2).val=yy1;_tmp63E;});}static char _tmp16A[28U]="list_t<offsetof_field_t,`H>";
# 1016 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp16A,_tmp16A,_tmp16A + 28U};
union Cyc_YYSTYPE*_tmp168=yy1;void*_tmp169;if((((union Cyc_YYSTYPE*)_tmp168)->YY3).tag == 9){_tmp169=(_tmp168->YY3).val;{struct Cyc_List_List*yy=_tmp169;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1023
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp63F;(_tmp63F.YY3).tag=9U,(_tmp63F.YY3).val=yy1;_tmp63F;});}static char _tmp16D[6U]="exp_t";
# 1017 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp16D,_tmp16D,_tmp16D + 6U};
union Cyc_YYSTYPE*_tmp16B=yy1;void*_tmp16C;if((((union Cyc_YYSTYPE*)_tmp16B)->Exp_tok).tag == 5){_tmp16C=(_tmp16B->Exp_tok).val;{struct Cyc_Absyn_Exp*yy=_tmp16C;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1024
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp640;(_tmp640.Exp_tok).tag=5U,(_tmp640.Exp_tok).val=yy1;_tmp640;});}static char _tmp170[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp170,_tmp170,_tmp170 + 17U};
union Cyc_YYSTYPE*_tmp16E=yy1;void*_tmp16F;if((((union Cyc_YYSTYPE*)_tmp16E)->YY4).tag == 10){_tmp16F=(_tmp16E->YY4).val;{struct Cyc_List_List*yy=_tmp16F;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1032
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp641;(_tmp641.YY4).tag=10U,(_tmp641.YY4).val=yy1;_tmp641;});}static char _tmp173[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1026 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp173,_tmp173,_tmp173 + 47U};
union Cyc_YYSTYPE*_tmp171=yy1;void*_tmp172;if((((union Cyc_YYSTYPE*)_tmp171)->YY5).tag == 11){_tmp172=(_tmp171->YY5).val;{struct Cyc_List_List*yy=_tmp172;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1033
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp642;(_tmp642.YY5).tag=11U,(_tmp642.YY5).val=yy1;_tmp642;});}static char _tmp176[9U]="primop_t";
# 1027 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp176,_tmp176,_tmp176 + 9U};
union Cyc_YYSTYPE*_tmp174=yy1;enum Cyc_Absyn_Primop _tmp175;if((((union Cyc_YYSTYPE*)_tmp174)->YY6).tag == 12){_tmp175=(_tmp174->YY6).val;{enum Cyc_Absyn_Primop yy=_tmp175;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1034
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp643;(_tmp643.YY6).tag=12U,(_tmp643.YY6).val=yy1;_tmp643;});}static char _tmp179[19U]="opt_t<primop_t,`H>";
# 1028 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp179,_tmp179,_tmp179 + 19U};
union Cyc_YYSTYPE*_tmp177=yy1;void*_tmp178;if((((union Cyc_YYSTYPE*)_tmp177)->YY7).tag == 13){_tmp178=(_tmp177->YY7).val;{struct Cyc_Core_Opt*yy=_tmp178;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1035
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp644;(_tmp644.YY7).tag=13U,(_tmp644.YY7).val=yy1;_tmp644;});}static char _tmp17C[7U]="qvar_t";
# 1029 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp17C,_tmp17C,_tmp17C + 7U};
union Cyc_YYSTYPE*_tmp17A=yy1;void*_tmp17B;if((((union Cyc_YYSTYPE*)_tmp17A)->QualId_tok).tag == 4){_tmp17B=(_tmp17A->QualId_tok).val;{struct _tuple0*yy=_tmp17B;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1036
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp645;(_tmp645.QualId_tok).tag=4U,(_tmp645.QualId_tok).val=yy1;_tmp645;});}static char _tmp17F[7U]="stmt_t";
# 1032 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp17F,_tmp17F,_tmp17F + 7U};
union Cyc_YYSTYPE*_tmp17D=yy1;void*_tmp17E;if((((union Cyc_YYSTYPE*)_tmp17D)->Stmt_tok).tag == 6){_tmp17E=(_tmp17D->Stmt_tok).val;{struct Cyc_Absyn_Stmt*yy=_tmp17E;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1039
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp646;(_tmp646.Stmt_tok).tag=6U,(_tmp646.Stmt_tok).val=yy1;_tmp646;});}static char _tmp182[27U]="list_t<switch_clause_t,`H>";
# 1035 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp182,_tmp182,_tmp182 + 27U};
union Cyc_YYSTYPE*_tmp180=yy1;void*_tmp181;if((((union Cyc_YYSTYPE*)_tmp180)->YY8).tag == 14){_tmp181=(_tmp180->YY8).val;{struct Cyc_List_List*yy=_tmp181;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1042
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp647;(_tmp647.YY8).tag=14U,(_tmp647.YY8).val=yy1;_tmp647;});}static char _tmp185[6U]="pat_t";
# 1036 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp185,_tmp185,_tmp185 + 6U};
union Cyc_YYSTYPE*_tmp183=yy1;void*_tmp184;if((((union Cyc_YYSTYPE*)_tmp183)->YY9).tag == 15){_tmp184=(_tmp183->YY9).val;{struct Cyc_Absyn_Pat*yy=_tmp184;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1043
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp648;(_tmp648.YY9).tag=15U,(_tmp648.YY9).val=yy1;_tmp648;});}static char _tmp188[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1041 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp188,_tmp188,_tmp188 + 28U};
union Cyc_YYSTYPE*_tmp186=yy1;void*_tmp187;if((((union Cyc_YYSTYPE*)_tmp186)->YY10).tag == 16){_tmp187=(_tmp186->YY10).val;{struct _tuple23*yy=_tmp187;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1048
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp649;(_tmp649.YY10).tag=16U,(_tmp649.YY10).val=yy1;_tmp649;});}static char _tmp18B[17U]="list_t<pat_t,`H>";
# 1042 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18B,_tmp18B,_tmp18B + 17U};
union Cyc_YYSTYPE*_tmp189=yy1;void*_tmp18A;if((((union Cyc_YYSTYPE*)_tmp189)->YY11).tag == 17){_tmp18A=(_tmp189->YY11).val;{struct Cyc_List_List*yy=_tmp18A;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1049
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp64A;(_tmp64A.YY11).tag=17U,(_tmp64A.YY11).val=yy1;_tmp64A;});}static char _tmp18E[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1043 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18E,_tmp18E,_tmp18E + 36U};
union Cyc_YYSTYPE*_tmp18C=yy1;void*_tmp18D;if((((union Cyc_YYSTYPE*)_tmp18C)->YY12).tag == 18){_tmp18D=(_tmp18C->YY12).val;{struct _tuple24*yy=_tmp18D;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1050
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp64B;(_tmp64B.YY12).tag=18U,(_tmp64B.YY12).val=yy1;_tmp64B;});}static char _tmp191[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1044 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp191,_tmp191,_tmp191 + 47U};
union Cyc_YYSTYPE*_tmp18F=yy1;void*_tmp190;if((((union Cyc_YYSTYPE*)_tmp18F)->YY13).tag == 19){_tmp190=(_tmp18F->YY13).val;{struct Cyc_List_List*yy=_tmp190;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1051
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp64C;(_tmp64C.YY13).tag=19U,(_tmp64C.YY13).val=yy1;_tmp64C;});}static char _tmp194[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1045 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp194,_tmp194,_tmp194 + 58U};
union Cyc_YYSTYPE*_tmp192=yy1;void*_tmp193;if((((union Cyc_YYSTYPE*)_tmp192)->YY14).tag == 20){_tmp193=(_tmp192->YY14).val;{struct _tuple23*yy=_tmp193;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1052
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp64D;(_tmp64D.YY14).tag=20U,(_tmp64D.YY14).val=yy1;_tmp64D;});}static char _tmp197[9U]="fndecl_t";
# 1046 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp197,_tmp197,_tmp197 + 9U};
union Cyc_YYSTYPE*_tmp195=yy1;void*_tmp196;if((((union Cyc_YYSTYPE*)_tmp195)->YY15).tag == 21){_tmp196=(_tmp195->YY15).val;{struct Cyc_Absyn_Fndecl*yy=_tmp196;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1053
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp64E;(_tmp64E.YY15).tag=21U,(_tmp64E.YY15).val=yy1;_tmp64E;});}static char _tmp19A[18U]="list_t<decl_t,`H>";
# 1047 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19A,_tmp19A,_tmp19A + 18U};
union Cyc_YYSTYPE*_tmp198=yy1;void*_tmp199;if((((union Cyc_YYSTYPE*)_tmp198)->YY16).tag == 22){_tmp199=(_tmp198->YY16).val;{struct Cyc_List_List*yy=_tmp199;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1054
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp64F;(_tmp64F.YY16).tag=22U,(_tmp64F.YY16).val=yy1;_tmp64F;});}static char _tmp19D[12U]="decl_spec_t";
# 1050 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19D,_tmp19D,_tmp19D + 12U};
union Cyc_YYSTYPE*_tmp19B=yy1;struct Cyc_Parse_Declaration_spec _tmp19C;if((((union Cyc_YYSTYPE*)_tmp19B)->YY17).tag == 23){_tmp19C=(_tmp19B->YY17).val;{struct Cyc_Parse_Declaration_spec yy=_tmp19C;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1057
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp650;(_tmp650.YY17).tag=23U,(_tmp650.YY17).val=yy1;_tmp650;});}static char _tmp1A0[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1051 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A0,_tmp1A0,_tmp1A0 + 31U};
union Cyc_YYSTYPE*_tmp19E=yy1;struct _tuple12 _tmp19F;if((((union Cyc_YYSTYPE*)_tmp19E)->YY18).tag == 24){_tmp19F=(_tmp19E->YY18).val;{struct _tuple12 yy=_tmp19F;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1058
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp651;(_tmp651.YY18).tag=24U,(_tmp651.YY18).val=yy1;_tmp651;});}static char _tmp1A3[23U]="declarator_list_t<`yy>";
# 1052 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A3,_tmp1A3,_tmp1A3 + 23U};
union Cyc_YYSTYPE*_tmp1A1=yy1;void*_tmp1A2;if((((union Cyc_YYSTYPE*)_tmp1A1)->YY19).tag == 25){_tmp1A2=(_tmp1A1->YY19).val;{struct _tuple11*yy=_tmp1A2;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1059
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){return({union Cyc_YYSTYPE _tmp652;(_tmp652.YY19).tag=25U,(_tmp652.YY19).val=yy1;_tmp652;});}static char _tmp1A6[16U]="storage_class_t";
# 1053 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A6,_tmp1A6,_tmp1A6 + 16U};
union Cyc_YYSTYPE*_tmp1A4=yy1;enum Cyc_Parse_Storage_class _tmp1A5;if((((union Cyc_YYSTYPE*)_tmp1A4)->YY20).tag == 26){_tmp1A5=(_tmp1A4->YY20).val;{enum Cyc_Parse_Storage_class yy=_tmp1A5;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1060
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _tmp653;(_tmp653.YY20).tag=26U,(_tmp653.YY20).val=yy1;_tmp653;});}static char _tmp1A9[17U]="type_specifier_t";
# 1054 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A9,_tmp1A9,_tmp1A9 + 17U};
union Cyc_YYSTYPE*_tmp1A7=yy1;struct Cyc_Parse_Type_specifier _tmp1A8;if((((union Cyc_YYSTYPE*)_tmp1A7)->YY21).tag == 27){_tmp1A8=(_tmp1A7->YY21).val;{struct Cyc_Parse_Type_specifier yy=_tmp1A8;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1061
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp654;(_tmp654.YY21).tag=27U,(_tmp654.YY21).val=yy1;_tmp654;});}static char _tmp1AC[12U]="aggr_kind_t";
# 1056 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AC,_tmp1AC,_tmp1AC + 12U};
union Cyc_YYSTYPE*_tmp1AA=yy1;enum Cyc_Absyn_AggrKind _tmp1AB;if((((union Cyc_YYSTYPE*)_tmp1AA)->YY22).tag == 28){_tmp1AB=(_tmp1AA->YY22).val;{enum Cyc_Absyn_AggrKind yy=_tmp1AB;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1063
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp655;(_tmp655.YY22).tag=28U,(_tmp655.YY22).val=yy1;_tmp655;});}static char _tmp1AF[20U]="$(bool,aggr_kind_t)";
# 1057 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AF,_tmp1AF,_tmp1AF + 20U};
union Cyc_YYSTYPE*_tmp1AD=yy1;struct _tuple25 _tmp1AE;if((((union Cyc_YYSTYPE*)_tmp1AD)->YY23).tag == 29){_tmp1AE=(_tmp1AD->YY23).val;{struct _tuple25 yy=_tmp1AE;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1064
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp656;(_tmp656.YY23).tag=29U,(_tmp656.YY23).val=yy1;_tmp656;});}static char _tmp1B2[8U]="tqual_t";
# 1058 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B2,_tmp1B2,_tmp1B2 + 8U};
union Cyc_YYSTYPE*_tmp1B0=yy1;struct Cyc_Absyn_Tqual _tmp1B1;if((((union Cyc_YYSTYPE*)_tmp1B0)->YY24).tag == 30){_tmp1B1=(_tmp1B0->YY24).val;{struct Cyc_Absyn_Tqual yy=_tmp1B1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1065
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp657;(_tmp657.YY24).tag=30U,(_tmp657.YY24).val=yy1;_tmp657;});}static char _tmp1B5[23U]="list_t<aggrfield_t,`H>";
# 1059 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B5,_tmp1B5,_tmp1B5 + 23U};
union Cyc_YYSTYPE*_tmp1B3=yy1;void*_tmp1B4;if((((union Cyc_YYSTYPE*)_tmp1B3)->YY25).tag == 31){_tmp1B4=(_tmp1B3->YY25).val;{struct Cyc_List_List*yy=_tmp1B4;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1066
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp658;(_tmp658.YY25).tag=31U,(_tmp658.YY25).val=yy1;_tmp658;});}static char _tmp1B8[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1060 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B8,_tmp1B8,_tmp1B8 + 34U};
union Cyc_YYSTYPE*_tmp1B6=yy1;void*_tmp1B7;if((((union Cyc_YYSTYPE*)_tmp1B6)->YY26).tag == 32){_tmp1B7=(_tmp1B6->YY26).val;{struct Cyc_List_List*yy=_tmp1B7;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1067
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp659;(_tmp659.YY26).tag=32U,(_tmp659.YY26).val=yy1;_tmp659;});}static char _tmp1BB[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1061 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BB,_tmp1BB,_tmp1BB + 33U};
union Cyc_YYSTYPE*_tmp1B9=yy1;void*_tmp1BA;if((((union Cyc_YYSTYPE*)_tmp1B9)->YY27).tag == 33){_tmp1BA=(_tmp1B9->YY27).val;{struct Cyc_List_List*yy=_tmp1BA;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1068
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp65A;(_tmp65A.YY27).tag=33U,(_tmp65A.YY27).val=yy1;_tmp65A;});}static char _tmp1BE[18U]="declarator_t<`yy>";
# 1062 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BE,_tmp1BE,_tmp1BE + 18U};
union Cyc_YYSTYPE*_tmp1BC=yy1;struct Cyc_Parse_Declarator _tmp1BD;if((((union Cyc_YYSTYPE*)_tmp1BC)->YY28).tag == 34){_tmp1BD=(_tmp1BC->YY28).val;{struct Cyc_Parse_Declarator yy=_tmp1BD;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1069
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp65B;(_tmp65B.YY28).tag=34U,(_tmp65B.YY28).val=yy1;_tmp65B;});}static char _tmp1C1[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1063 "parse.y"
static struct _tuple26*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C1,_tmp1C1,_tmp1C1 + 45U};
union Cyc_YYSTYPE*_tmp1BF=yy1;void*_tmp1C0;if((((union Cyc_YYSTYPE*)_tmp1BF)->YY29).tag == 35){_tmp1C0=(_tmp1BF->YY29).val;{struct _tuple26*yy=_tmp1C0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1070
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp65C;(_tmp65C.YY29).tag=35U,(_tmp65C.YY29).val=yy1;_tmp65C;});}static char _tmp1C4[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1064 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C4,_tmp1C4,_tmp1C4 + 57U};
union Cyc_YYSTYPE*_tmp1C2=yy1;void*_tmp1C3;if((((union Cyc_YYSTYPE*)_tmp1C2)->YY30).tag == 36){_tmp1C3=(_tmp1C2->YY30).val;{struct Cyc_List_List*yy=_tmp1C3;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1071
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp65D;(_tmp65D.YY30).tag=36U,(_tmp65D.YY30).val=yy1;_tmp65D;});}static char _tmp1C7[26U]="abstractdeclarator_t<`yy>";
# 1065 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C7,_tmp1C7,_tmp1C7 + 26U};
union Cyc_YYSTYPE*_tmp1C5=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1C6;if((((union Cyc_YYSTYPE*)_tmp1C5)->YY31).tag == 37){_tmp1C6=(_tmp1C5->YY31).val;{struct Cyc_Parse_Abstractdeclarator yy=_tmp1C6;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1072
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp65E;(_tmp65E.YY31).tag=37U,(_tmp65E.YY31).val=yy1;_tmp65E;});}static char _tmp1CA[5U]="bool";
# 1066 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CA,_tmp1CA,_tmp1CA + 5U};
union Cyc_YYSTYPE*_tmp1C8=yy1;int _tmp1C9;if((((union Cyc_YYSTYPE*)_tmp1C8)->YY32).tag == 38){_tmp1C9=(_tmp1C8->YY32).val;{int yy=_tmp1C9;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1073
static union Cyc_YYSTYPE Cyc_YY32(int yy1){return({union Cyc_YYSTYPE _tmp65F;(_tmp65F.YY32).tag=38U,(_tmp65F.YY32).val=yy1;_tmp65F;});}static char _tmp1CD[8U]="scope_t";
# 1067 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CD,_tmp1CD,_tmp1CD + 8U};
union Cyc_YYSTYPE*_tmp1CB=yy1;enum Cyc_Absyn_Scope _tmp1CC;if((((union Cyc_YYSTYPE*)_tmp1CB)->YY33).tag == 39){_tmp1CC=(_tmp1CB->YY33).val;{enum Cyc_Absyn_Scope yy=_tmp1CC;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1074
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp660;(_tmp660.YY33).tag=39U,(_tmp660.YY33).val=yy1;_tmp660;});}static char _tmp1D0[16U]="datatypefield_t";
# 1068 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D0,_tmp1D0,_tmp1D0 + 16U};
union Cyc_YYSTYPE*_tmp1CE=yy1;void*_tmp1CF;if((((union Cyc_YYSTYPE*)_tmp1CE)->YY34).tag == 40){_tmp1CF=(_tmp1CE->YY34).val;{struct Cyc_Absyn_Datatypefield*yy=_tmp1CF;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1075
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp661;(_tmp661.YY34).tag=40U,(_tmp661.YY34).val=yy1;_tmp661;});}static char _tmp1D3[27U]="list_t<datatypefield_t,`H>";
# 1069 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D3,_tmp1D3,_tmp1D3 + 27U};
union Cyc_YYSTYPE*_tmp1D1=yy1;void*_tmp1D2;if((((union Cyc_YYSTYPE*)_tmp1D1)->YY35).tag == 41){_tmp1D2=(_tmp1D1->YY35).val;{struct Cyc_List_List*yy=_tmp1D2;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1076
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp662;(_tmp662.YY35).tag=41U,(_tmp662.YY35).val=yy1;_tmp662;});}static char _tmp1D6[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1070 "parse.y"
static struct _tuple27 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D6,_tmp1D6,_tmp1D6 + 41U};
union Cyc_YYSTYPE*_tmp1D4=yy1;struct _tuple27 _tmp1D5;if((((union Cyc_YYSTYPE*)_tmp1D4)->YY36).tag == 42){_tmp1D5=(_tmp1D4->YY36).val;{struct _tuple27 yy=_tmp1D5;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1077
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple27 yy1){return({union Cyc_YYSTYPE _tmp663;(_tmp663.YY36).tag=42U,(_tmp663.YY36).val=yy1;_tmp663;});}static char _tmp1D9[17U]="list_t<var_t,`H>";
# 1071 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D9,_tmp1D9,_tmp1D9 + 17U};
union Cyc_YYSTYPE*_tmp1D7=yy1;void*_tmp1D8;if((((union Cyc_YYSTYPE*)_tmp1D7)->YY37).tag == 43){_tmp1D8=(_tmp1D7->YY37).val;{struct Cyc_List_List*yy=_tmp1D8;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1078
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp664;(_tmp664.YY37).tag=43U,(_tmp664.YY37).val=yy1;_tmp664;});}static char _tmp1DC[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1072 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DC,_tmp1DC,_tmp1DC + 31U};
union Cyc_YYSTYPE*_tmp1DA=yy1;void*_tmp1DB;if((((union Cyc_YYSTYPE*)_tmp1DA)->YY38).tag == 44){_tmp1DB=(_tmp1DA->YY38).val;{struct _tuple8*yy=_tmp1DB;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1079
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp665;(_tmp665.YY38).tag=44U,(_tmp665.YY38).val=yy1;_tmp665;});}static char _tmp1DF[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1073 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DF,_tmp1DF,_tmp1DF + 42U};
union Cyc_YYSTYPE*_tmp1DD=yy1;void*_tmp1DE;if((((union Cyc_YYSTYPE*)_tmp1DD)->YY39).tag == 45){_tmp1DE=(_tmp1DD->YY39).val;{struct Cyc_List_List*yy=_tmp1DE;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1080
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp666;(_tmp666.YY39).tag=45U,(_tmp666.YY39).val=yy1;_tmp666;});}static char _tmp1E2[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1074 "parse.y"
static struct _tuple28*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E2,_tmp1E2,_tmp1E2 + 115U};
union Cyc_YYSTYPE*_tmp1E0=yy1;void*_tmp1E1;if((((union Cyc_YYSTYPE*)_tmp1E0)->YY40).tag == 46){_tmp1E1=(_tmp1E0->YY40).val;{struct _tuple28*yy=_tmp1E1;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1081
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp667;(_tmp667.YY40).tag=46U,(_tmp667.YY40).val=yy1;_tmp667;});}static char _tmp1E5[8U]="types_t";
# 1075 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E5,_tmp1E5,_tmp1E5 + 8U};
union Cyc_YYSTYPE*_tmp1E3=yy1;void*_tmp1E4;if((((union Cyc_YYSTYPE*)_tmp1E3)->YY41).tag == 47){_tmp1E4=(_tmp1E3->YY41).val;{struct Cyc_List_List*yy=_tmp1E4;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1082
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp668;(_tmp668.YY41).tag=47U,(_tmp668.YY41).val=yy1;_tmp668;});}static char _tmp1E8[24U]="list_t<designator_t,`H>";
# 1077 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E8,_tmp1E8,_tmp1E8 + 24U};
union Cyc_YYSTYPE*_tmp1E6=yy1;void*_tmp1E7;if((((union Cyc_YYSTYPE*)_tmp1E6)->YY42).tag == 48){_tmp1E7=(_tmp1E6->YY42).val;{struct Cyc_List_List*yy=_tmp1E7;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1084
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp669;(_tmp669.YY42).tag=48U,(_tmp669.YY42).val=yy1;_tmp669;});}static char _tmp1EB[13U]="designator_t";
# 1078 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EB,_tmp1EB,_tmp1EB + 13U};
union Cyc_YYSTYPE*_tmp1E9=yy1;void*_tmp1EA;if((((union Cyc_YYSTYPE*)_tmp1E9)->YY43).tag == 49){_tmp1EA=(_tmp1E9->YY43).val;{void*yy=_tmp1EA;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1085
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _tmp66A;(_tmp66A.YY43).tag=49U,(_tmp66A.YY43).val=yy1;_tmp66A;});}static char _tmp1EE[7U]="kind_t";
# 1079 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EE,_tmp1EE,_tmp1EE + 7U};
union Cyc_YYSTYPE*_tmp1EC=yy1;void*_tmp1ED;if((((union Cyc_YYSTYPE*)_tmp1EC)->YY44).tag == 50){_tmp1ED=(_tmp1EC->YY44).val;{struct Cyc_Absyn_Kind*yy=_tmp1ED;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1086
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp66B;(_tmp66B.YY44).tag=50U,(_tmp66B.YY44).val=yy1;_tmp66B;});}static char _tmp1F1[7U]="type_t";
# 1080 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F1,_tmp1F1,_tmp1F1 + 7U};
union Cyc_YYSTYPE*_tmp1EF=yy1;void*_tmp1F0;if((((union Cyc_YYSTYPE*)_tmp1EF)->YY45).tag == 51){_tmp1F0=(_tmp1EF->YY45).val;{void*yy=_tmp1F0;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1087
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){return({union Cyc_YYSTYPE _tmp66C;(_tmp66C.YY45).tag=51U,(_tmp66C.YY45).val=yy1;_tmp66C;});}static char _tmp1F4[23U]="list_t<attribute_t,`H>";
# 1081 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F4,_tmp1F4,_tmp1F4 + 23U};
union Cyc_YYSTYPE*_tmp1F2=yy1;void*_tmp1F3;if((((union Cyc_YYSTYPE*)_tmp1F2)->YY46).tag == 52){_tmp1F3=(_tmp1F2->YY46).val;{struct Cyc_List_List*yy=_tmp1F3;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1088
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp66D;(_tmp66D.YY46).tag=52U,(_tmp66D.YY46).val=yy1;_tmp66D;});}static char _tmp1F7[12U]="attribute_t";
# 1082 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F7,_tmp1F7,_tmp1F7 + 12U};
union Cyc_YYSTYPE*_tmp1F5=yy1;void*_tmp1F6;if((((union Cyc_YYSTYPE*)_tmp1F5)->YY47).tag == 53){_tmp1F6=(_tmp1F5->YY47).val;{void*yy=_tmp1F6;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1089
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){return({union Cyc_YYSTYPE _tmp66E;(_tmp66E.YY47).tag=53U,(_tmp66E.YY47).val=yy1;_tmp66E;});}static char _tmp1FA[12U]="enumfield_t";
# 1083 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FA,_tmp1FA,_tmp1FA + 12U};
union Cyc_YYSTYPE*_tmp1F8=yy1;void*_tmp1F9;if((((union Cyc_YYSTYPE*)_tmp1F8)->YY48).tag == 54){_tmp1F9=(_tmp1F8->YY48).val;{struct Cyc_Absyn_Enumfield*yy=_tmp1F9;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1090
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp66F;(_tmp66F.YY48).tag=54U,(_tmp66F.YY48).val=yy1;_tmp66F;});}static char _tmp1FD[23U]="list_t<enumfield_t,`H>";
# 1084 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FD,_tmp1FD,_tmp1FD + 23U};
union Cyc_YYSTYPE*_tmp1FB=yy1;void*_tmp1FC;if((((union Cyc_YYSTYPE*)_tmp1FB)->YY49).tag == 55){_tmp1FC=(_tmp1FB->YY49).val;{struct Cyc_List_List*yy=_tmp1FC;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1091
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp670;(_tmp670.YY49).tag=55U,(_tmp670.YY49).val=yy1;_tmp670;});}static char _tmp200[11U]="type_opt_t";
# 1085 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp200,_tmp200,_tmp200 + 11U};
union Cyc_YYSTYPE*_tmp1FE=yy1;void*_tmp1FF;if((((union Cyc_YYSTYPE*)_tmp1FE)->YY50).tag == 56){_tmp1FF=(_tmp1FE->YY50).val;{void*yy=_tmp1FF;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1092
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){return({union Cyc_YYSTYPE _tmp671;(_tmp671.YY50).tag=56U,(_tmp671.YY50).val=yy1;_tmp671;});}static char _tmp203[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1086 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp203,_tmp203,_tmp203 + 31U};
union Cyc_YYSTYPE*_tmp201=yy1;void*_tmp202;if((((union Cyc_YYSTYPE*)_tmp201)->YY51).tag == 57){_tmp202=(_tmp201->YY51).val;{struct Cyc_List_List*yy=_tmp202;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1093
static union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp672;(_tmp672.YY51).tag=57U,(_tmp672.YY51).val=yy1;_tmp672;});}static char _tmp206[11U]="booltype_t";
# 1087 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp206,_tmp206,_tmp206 + 11U};
union Cyc_YYSTYPE*_tmp204=yy1;void*_tmp205;if((((union Cyc_YYSTYPE*)_tmp204)->YY52).tag == 58){_tmp205=(_tmp204->YY52).val;{void*yy=_tmp205;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1094
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){return({union Cyc_YYSTYPE _tmp673;(_tmp673.YY52).tag=58U,(_tmp673.YY52).val=yy1;_tmp673;});}static char _tmp209[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1088 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp209,_tmp209,_tmp209 + 35U};
union Cyc_YYSTYPE*_tmp207=yy1;void*_tmp208;if((((union Cyc_YYSTYPE*)_tmp207)->YY53).tag == 59){_tmp208=(_tmp207->YY53).val;{struct Cyc_List_List*yy=_tmp208;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1095
static union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp674;(_tmp674.YY53).tag=59U,(_tmp674.YY53).val=yy1;_tmp674;});}static char _tmp20C[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1089 "parse.y"
static struct _tuple29*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20C,_tmp20C,_tmp20C + 48U};
union Cyc_YYSTYPE*_tmp20A=yy1;void*_tmp20B;if((((union Cyc_YYSTYPE*)_tmp20A)->YY54).tag == 60){_tmp20B=(_tmp20A->YY54).val;{struct _tuple29*yy=_tmp20B;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1096
static union Cyc_YYSTYPE Cyc_YY54(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp675;(_tmp675.YY54).tag=60U,(_tmp675.YY54).val=yy1;_tmp675;});}static char _tmp20F[18U]="list_t<qvar_t,`H>";
# 1090 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20F,_tmp20F,_tmp20F + 18U};
union Cyc_YYSTYPE*_tmp20D=yy1;void*_tmp20E;if((((union Cyc_YYSTYPE*)_tmp20D)->YY55).tag == 61){_tmp20E=(_tmp20D->YY55).val;{struct Cyc_List_List*yy=_tmp20E;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1097
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp676;(_tmp676.YY55).tag=61U,(_tmp676.YY55).val=yy1;_tmp676;});}static char _tmp212[20U]="pointer_qual_t<`yy>";
# 1091 "parse.y"
static void*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp212,_tmp212,_tmp212 + 20U};
union Cyc_YYSTYPE*_tmp210=yy1;void*_tmp211;if((((union Cyc_YYSTYPE*)_tmp210)->YY56).tag == 62){_tmp211=(_tmp210->YY56).val;{void*yy=_tmp211;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1098
static union Cyc_YYSTYPE Cyc_YY56(void*yy1){return({union Cyc_YYSTYPE _tmp677;(_tmp677.YY56).tag=62U,(_tmp677.YY56).val=yy1;_tmp677;});}static char _tmp215[21U]="pointer_quals_t<`yy>";
# 1092 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp215,_tmp215,_tmp215 + 21U};
union Cyc_YYSTYPE*_tmp213=yy1;void*_tmp214;if((((union Cyc_YYSTYPE*)_tmp213)->YY57).tag == 63){_tmp214=(_tmp213->YY57).val;{struct Cyc_List_List*yy=_tmp214;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1099
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp678;(_tmp678.YY57).tag=63U,(_tmp678.YY57).val=yy1;_tmp678;});}static char _tmp218[10U]="exp_opt_t";
# 1093 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp218,_tmp218,_tmp218 + 10U};
union Cyc_YYSTYPE*_tmp216=yy1;void*_tmp217;if((((union Cyc_YYSTYPE*)_tmp216)->YY58).tag == 64){_tmp217=(_tmp216->YY58).val;{struct Cyc_Absyn_Exp*yy=_tmp217;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1100
static union Cyc_YYSTYPE Cyc_YY58(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp679;(_tmp679.YY58).tag=64U,(_tmp679.YY58).val=yy1;_tmp679;});}static char _tmp21B[23U]="$(exp_opt_t,exp_opt_t)";
# 1094 "parse.y"
static struct _tuple17 Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21B,_tmp21B,_tmp21B + 23U};
union Cyc_YYSTYPE*_tmp219=yy1;struct _tuple17 _tmp21A;if((((union Cyc_YYSTYPE*)_tmp219)->YY59).tag == 65){_tmp21A=(_tmp219->YY59).val;{struct _tuple17 yy=_tmp21A;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1101
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple17 yy1){return({union Cyc_YYSTYPE _tmp67A;(_tmp67A.YY59).tag=65U,(_tmp67A.YY59).val=yy1;_tmp67A;});}static char _tmp21E[10U]="raw_exp_t";
# 1095 "parse.y"
static void*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21E,_tmp21E,_tmp21E + 10U};
union Cyc_YYSTYPE*_tmp21C=yy1;void*_tmp21D;if((((union Cyc_YYSTYPE*)_tmp21C)->YY60).tag == 66){_tmp21D=(_tmp21C->YY60).val;{void*yy=_tmp21D;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1102
static union Cyc_YYSTYPE Cyc_YY60(void*yy1){return({union Cyc_YYSTYPE _tmp67B;(_tmp67B.YY60).tag=66U,(_tmp67B.YY60).val=yy1;_tmp67B;});}static char _tmp221[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1096 "parse.y"
static struct _tuple30*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp221,_tmp221,_tmp221 + 112U};
union Cyc_YYSTYPE*_tmp21F=yy1;void*_tmp220;if((((union Cyc_YYSTYPE*)_tmp21F)->YY61).tag == 67){_tmp220=(_tmp21F->YY61).val;{struct _tuple30*yy=_tmp220;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1103
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp67C;(_tmp67C.YY61).tag=67U,(_tmp67C.YY61).val=yy1;_tmp67C;});}static char _tmp224[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1097 "parse.y"
static struct _tuple31*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp224,_tmp224,_tmp224 + 73U};
union Cyc_YYSTYPE*_tmp222=yy1;void*_tmp223;if((((union Cyc_YYSTYPE*)_tmp222)->YY62).tag == 68){_tmp223=(_tmp222->YY62).val;{struct _tuple31*yy=_tmp223;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1104
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp67D;(_tmp67D.YY62).tag=68U,(_tmp67D.YY62).val=yy1;_tmp67D;});}static char _tmp227[28U]="list_t<string_t<`H>@`H, `H>";
# 1098 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp227,_tmp227,_tmp227 + 28U};
union Cyc_YYSTYPE*_tmp225=yy1;void*_tmp226;if((((union Cyc_YYSTYPE*)_tmp225)->YY63).tag == 69){_tmp226=(_tmp225->YY63).val;{struct Cyc_List_List*yy=_tmp226;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1105
static union Cyc_YYSTYPE Cyc_YY63(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp67E;(_tmp67E.YY63).tag=69U,(_tmp67E.YY63).val=yy1;_tmp67E;});}static char _tmp22A[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1099 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22A,_tmp22A,_tmp22A + 38U};
union Cyc_YYSTYPE*_tmp228=yy1;void*_tmp229;if((((union Cyc_YYSTYPE*)_tmp228)->YY64).tag == 70){_tmp229=(_tmp228->YY64).val;{struct Cyc_List_List*yy=_tmp229;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1106
static union Cyc_YYSTYPE Cyc_YY64(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp67F;(_tmp67F.YY64).tag=70U,(_tmp67F.YY64).val=yy1;_tmp67F;});}static char _tmp22D[26U]="$(string_t<`H>, exp_t)@`H";
# 1100 "parse.y"
static struct _tuple32*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22D,_tmp22D,_tmp22D + 26U};
union Cyc_YYSTYPE*_tmp22B=yy1;void*_tmp22C;if((((union Cyc_YYSTYPE*)_tmp22B)->YY65).tag == 71){_tmp22C=(_tmp22B->YY65).val;{struct _tuple32*yy=_tmp22C;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1107
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple32*yy1){return({union Cyc_YYSTYPE _tmp680;(_tmp680.YY65).tag=71U,(_tmp680.YY65).val=yy1;_tmp680;});}static char _tmp230[16U]="exp_maker_fun_t";
# 1101 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_tmp230,_tmp230,_tmp230 + 16U};
union Cyc_YYSTYPE*_tmp22E=yy1;void*_tmp22F;if((((union Cyc_YYSTYPE*)_tmp22E)->YY66).tag == 72){_tmp22F=(_tmp22E->YY66).val;{struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_tmp22F;
return yy;}}else{
Cyc_yythrowfail(s);};}
# 1108
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _tmp681;(_tmp681.YY66).tag=72U,(_tmp681.YY66).val=yy1;_tmp681;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1124
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp682;_tmp682.timestamp=0,_tmp682.first_line=0,_tmp682.first_column=0,_tmp682.last_line=0,_tmp682.last_column=0;_tmp682;});}
# 1127
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1138 "parse.y"
static short Cyc_yytranslate[381U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,152,2,2,137,150,147,2,134,135,130,144,129,148,139,149,2,2,2,2,2,2,2,2,2,2,138,126,132,131,133,143,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,140,2,141,146,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,127,145,128,151,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125};static char _tmp231[2U]="$";static char _tmp232[6U]="error";static char _tmp233[12U]="$undefined.";static char _tmp234[5U]="AUTO";static char _tmp235[9U]="REGISTER";static char _tmp236[7U]="STATIC";static char _tmp237[7U]="EXTERN";static char _tmp238[8U]="TYPEDEF";static char _tmp239[5U]="VOID";static char _tmp23A[5U]="CHAR";static char _tmp23B[6U]="SHORT";static char _tmp23C[4U]="INT";static char _tmp23D[5U]="LONG";static char _tmp23E[6U]="FLOAT";static char _tmp23F[7U]="DOUBLE";static char _tmp240[7U]="SIGNED";static char _tmp241[9U]="UNSIGNED";static char _tmp242[6U]="CONST";static char _tmp243[9U]="VOLATILE";static char _tmp244[9U]="RESTRICT";static char _tmp245[7U]="STRUCT";static char _tmp246[6U]="UNION";static char _tmp247[5U]="CASE";static char _tmp248[8U]="DEFAULT";static char _tmp249[7U]="INLINE";static char _tmp24A[7U]="SIZEOF";static char _tmp24B[9U]="OFFSETOF";static char _tmp24C[3U]="IF";static char _tmp24D[5U]="ELSE";static char _tmp24E[7U]="SWITCH";static char _tmp24F[6U]="WHILE";static char _tmp250[3U]="DO";static char _tmp251[4U]="FOR";static char _tmp252[5U]="GOTO";static char _tmp253[9U]="CONTINUE";static char _tmp254[6U]="BREAK";static char _tmp255[7U]="RETURN";static char _tmp256[5U]="ENUM";static char _tmp257[7U]="TYPEOF";static char _tmp258[16U]="BUILTIN_VA_LIST";static char _tmp259[10U]="EXTENSION";static char _tmp25A[8U]="NULL_kw";static char _tmp25B[4U]="LET";static char _tmp25C[6U]="THROW";static char _tmp25D[4U]="TRY";static char _tmp25E[6U]="CATCH";static char _tmp25F[7U]="EXPORT";static char _tmp260[9U]="OVERRIDE";static char _tmp261[5U]="HIDE";static char _tmp262[4U]="NEW";static char _tmp263[9U]="ABSTRACT";static char _tmp264[9U]="FALLTHRU";static char _tmp265[6U]="USING";static char _tmp266[10U]="NAMESPACE";static char _tmp267[9U]="DATATYPE";static char _tmp268[7U]="MALLOC";static char _tmp269[8U]="RMALLOC";static char _tmp26A[15U]="RMALLOC_INLINE";static char _tmp26B[7U]="CALLOC";static char _tmp26C[8U]="RCALLOC";static char _tmp26D[5U]="SWAP";static char _tmp26E[9U]="REGION_T";static char _tmp26F[6U]="TAG_T";static char _tmp270[7U]="REGION";static char _tmp271[5U]="RNEW";static char _tmp272[8U]="REGIONS";static char _tmp273[7U]="PORTON";static char _tmp274[8U]="PORTOFF";static char _tmp275[7U]="PRAGMA";static char _tmp276[10U]="TEMPESTON";static char _tmp277[11U]="TEMPESTOFF";static char _tmp278[8U]="NUMELTS";static char _tmp279[8U]="VALUEOF";static char _tmp27A[10U]="VALUEOF_T";static char _tmp27B[9U]="TAGCHECK";static char _tmp27C[13U]="NUMELTS_QUAL";static char _tmp27D[10U]="THIN_QUAL";static char _tmp27E[9U]="FAT_QUAL";static char _tmp27F[13U]="NOTNULL_QUAL";static char _tmp280[14U]="NULLABLE_QUAL";static char _tmp281[14U]="REQUIRES_QUAL";static char _tmp282[13U]="ENSURES_QUAL";static char _tmp283[12U]="REGION_QUAL";static char _tmp284[16U]="NOZEROTERM_QUAL";static char _tmp285[14U]="ZEROTERM_QUAL";static char _tmp286[12U]="TAGGED_QUAL";static char _tmp287[12U]="ASSERT_QUAL";static char _tmp288[16U]="EXTENSIBLE_QUAL";static char _tmp289[18U]="AUTORELEASED_QUAL";static char _tmp28A[7U]="PTR_OP";static char _tmp28B[7U]="INC_OP";static char _tmp28C[7U]="DEC_OP";static char _tmp28D[8U]="LEFT_OP";static char _tmp28E[9U]="RIGHT_OP";static char _tmp28F[6U]="LE_OP";static char _tmp290[6U]="GE_OP";static char _tmp291[6U]="EQ_OP";static char _tmp292[6U]="NE_OP";static char _tmp293[7U]="AND_OP";static char _tmp294[6U]="OR_OP";static char _tmp295[11U]="MUL_ASSIGN";static char _tmp296[11U]="DIV_ASSIGN";static char _tmp297[11U]="MOD_ASSIGN";static char _tmp298[11U]="ADD_ASSIGN";static char _tmp299[11U]="SUB_ASSIGN";static char _tmp29A[12U]="LEFT_ASSIGN";static char _tmp29B[13U]="RIGHT_ASSIGN";static char _tmp29C[11U]="AND_ASSIGN";static char _tmp29D[11U]="XOR_ASSIGN";static char _tmp29E[10U]="OR_ASSIGN";static char _tmp29F[9U]="ELLIPSIS";static char _tmp2A0[11U]="LEFT_RIGHT";static char _tmp2A1[12U]="COLON_COLON";static char _tmp2A2[11U]="IDENTIFIER";static char _tmp2A3[17U]="INTEGER_CONSTANT";static char _tmp2A4[7U]="STRING";static char _tmp2A5[8U]="WSTRING";static char _tmp2A6[19U]="CHARACTER_CONSTANT";static char _tmp2A7[20U]="WCHARACTER_CONSTANT";static char _tmp2A8[18U]="FLOATING_CONSTANT";static char _tmp2A9[9U]="TYPE_VAR";static char _tmp2AA[13U]="TYPEDEF_NAME";static char _tmp2AB[16U]="QUAL_IDENTIFIER";static char _tmp2AC[18U]="QUAL_TYPEDEF_NAME";static char _tmp2AD[10U]="ATTRIBUTE";static char _tmp2AE[8U]="ASM_TOK";static char _tmp2AF[4U]="';'";static char _tmp2B0[4U]="'{'";static char _tmp2B1[4U]="'}'";static char _tmp2B2[4U]="','";static char _tmp2B3[4U]="'*'";static char _tmp2B4[4U]="'='";static char _tmp2B5[4U]="'<'";static char _tmp2B6[4U]="'>'";static char _tmp2B7[4U]="'('";static char _tmp2B8[4U]="')'";static char _tmp2B9[4U]="'_'";static char _tmp2BA[4U]="'$'";static char _tmp2BB[4U]="':'";static char _tmp2BC[4U]="'.'";static char _tmp2BD[4U]="'['";static char _tmp2BE[4U]="']'";static char _tmp2BF[4U]="'@'";static char _tmp2C0[4U]="'?'";static char _tmp2C1[4U]="'+'";static char _tmp2C2[4U]="'|'";static char _tmp2C3[4U]="'^'";static char _tmp2C4[4U]="'&'";static char _tmp2C5[4U]="'-'";static char _tmp2C6[4U]="'/'";static char _tmp2C7[4U]="'%'";static char _tmp2C8[4U]="'~'";static char _tmp2C9[4U]="'!'";static char _tmp2CA[5U]="prog";static char _tmp2CB[17U]="translation_unit";static char _tmp2CC[18U]="tempest_on_action";static char _tmp2CD[19U]="tempest_off_action";static char _tmp2CE[16U]="extern_c_action";static char _tmp2CF[13U]="end_extern_c";static char _tmp2D0[14U]="hide_list_opt";static char _tmp2D1[17U]="hide_list_values";static char _tmp2D2[16U]="export_list_opt";static char _tmp2D3[12U]="export_list";static char _tmp2D4[19U]="export_list_values";static char _tmp2D5[13U]="override_opt";static char _tmp2D6[21U]="external_declaration";static char _tmp2D7[14U]="optional_semi";static char _tmp2D8[20U]="function_definition";static char _tmp2D9[21U]="function_definition2";static char _tmp2DA[13U]="using_action";static char _tmp2DB[15U]="unusing_action";static char _tmp2DC[17U]="namespace_action";static char _tmp2DD[19U]="unnamespace_action";static char _tmp2DE[12U]="declaration";static char _tmp2DF[9U]="open_opt";static char _tmp2E0[17U]="declaration_list";static char _tmp2E1[23U]="declaration_specifiers";static char _tmp2E2[24U]="storage_class_specifier";static char _tmp2E3[15U]="attributes_opt";static char _tmp2E4[11U]="attributes";static char _tmp2E5[15U]="attribute_list";static char _tmp2E6[10U]="attribute";static char _tmp2E7[15U]="type_specifier";static char _tmp2E8[25U]="type_specifier_notypedef";static char _tmp2E9[5U]="kind";static char _tmp2EA[15U]="type_qualifier";static char _tmp2EB[15U]="enum_specifier";static char _tmp2EC[11U]="enum_field";static char _tmp2ED[22U]="enum_declaration_list";static char _tmp2EE[26U]="struct_or_union_specifier";static char _tmp2EF[26U]="maybe_tagged_struct_union";static char _tmp2F0[16U]="struct_or_union";static char _tmp2F1[16U]="type_params_opt";static char _tmp2F2[24U]="struct_declaration_list";static char _tmp2F3[25U]="struct_declaration_list0";static char _tmp2F4[25U]="init_declarator_list_rev";static char _tmp2F5[16U]="init_declarator";static char _tmp2F6[19U]="struct_declaration";static char _tmp2F7[25U]="specifier_qualifier_list";static char _tmp2F8[35U]="notypedef_specifier_qualifier_list";static char _tmp2F9[27U]="struct_declarator_list_rev";static char _tmp2FA[18U]="struct_declarator";static char _tmp2FB[20U]="requires_clause_opt";static char _tmp2FC[19U]="datatype_specifier";static char _tmp2FD[14U]="qual_datatype";static char _tmp2FE[19U]="datatypefield_list";static char _tmp2FF[20U]="datatypefield_scope";static char _tmp300[14U]="datatypefield";static char _tmp301[11U]="declarator";static char _tmp302[23U]="declarator_withtypedef";static char _tmp303[18U]="direct_declarator";static char _tmp304[30U]="direct_declarator_withtypedef";static char _tmp305[8U]="pointer";static char _tmp306[12U]="one_pointer";static char _tmp307[14U]="pointer_quals";static char _tmp308[13U]="pointer_qual";static char _tmp309[23U]="pointer_null_and_bound";static char _tmp30A[14U]="pointer_bound";static char _tmp30B[18U]="zeroterm_qual_opt";static char _tmp30C[8U]="rgn_opt";static char _tmp30D[11U]="tqual_list";static char _tmp30E[20U]="parameter_type_list";static char _tmp30F[9U]="type_var";static char _tmp310[16U]="optional_effect";static char _tmp311[19U]="optional_rgn_order";static char _tmp312[10U]="rgn_order";static char _tmp313[16U]="optional_inject";static char _tmp314[11U]="effect_set";static char _tmp315[14U]="atomic_effect";static char _tmp316[11U]="region_set";static char _tmp317[15U]="parameter_list";static char _tmp318[22U]="parameter_declaration";static char _tmp319[16U]="identifier_list";static char _tmp31A[17U]="identifier_list0";static char _tmp31B[12U]="initializer";static char _tmp31C[18U]="array_initializer";static char _tmp31D[17U]="initializer_list";static char _tmp31E[12U]="designation";static char _tmp31F[16U]="designator_list";static char _tmp320[11U]="designator";static char _tmp321[10U]="type_name";static char _tmp322[14U]="any_type_name";static char _tmp323[15U]="type_name_list";static char _tmp324[20U]="abstract_declarator";static char _tmp325[27U]="direct_abstract_declarator";static char _tmp326[25U]="requires_and_ensures_opt";static char _tmp327[10U]="statement";static char _tmp328[18U]="labeled_statement";static char _tmp329[21U]="expression_statement";static char _tmp32A[19U]="compound_statement";static char _tmp32B[16U]="block_item_list";static char _tmp32C[20U]="selection_statement";static char _tmp32D[15U]="switch_clauses";static char _tmp32E[20U]="iteration_statement";static char _tmp32F[12U]="for_exp_opt";static char _tmp330[15U]="jump_statement";static char _tmp331[12U]="exp_pattern";static char _tmp332[20U]="conditional_pattern";static char _tmp333[19U]="logical_or_pattern";static char _tmp334[20U]="logical_and_pattern";static char _tmp335[21U]="inclusive_or_pattern";static char _tmp336[21U]="exclusive_or_pattern";static char _tmp337[12U]="and_pattern";static char _tmp338[17U]="equality_pattern";static char _tmp339[19U]="relational_pattern";static char _tmp33A[14U]="shift_pattern";static char _tmp33B[17U]="additive_pattern";static char _tmp33C[23U]="multiplicative_pattern";static char _tmp33D[13U]="cast_pattern";static char _tmp33E[14U]="unary_pattern";static char _tmp33F[16U]="postfix_pattern";static char _tmp340[16U]="primary_pattern";static char _tmp341[8U]="pattern";static char _tmp342[19U]="tuple_pattern_list";static char _tmp343[20U]="tuple_pattern_list0";static char _tmp344[14U]="field_pattern";static char _tmp345[19U]="field_pattern_list";static char _tmp346[20U]="field_pattern_list0";static char _tmp347[11U]="expression";static char _tmp348[22U]="assignment_expression";static char _tmp349[20U]="assignment_operator";static char _tmp34A[23U]="conditional_expression";static char _tmp34B[20U]="constant_expression";static char _tmp34C[22U]="logical_or_expression";static char _tmp34D[23U]="logical_and_expression";static char _tmp34E[24U]="inclusive_or_expression";static char _tmp34F[24U]="exclusive_or_expression";static char _tmp350[15U]="and_expression";static char _tmp351[20U]="equality_expression";static char _tmp352[22U]="relational_expression";static char _tmp353[17U]="shift_expression";static char _tmp354[20U]="additive_expression";static char _tmp355[26U]="multiplicative_expression";static char _tmp356[12U]="equality_op";static char _tmp357[14U]="relational_op";static char _tmp358[12U]="additive_op";static char _tmp359[18U]="multiplicative_op";static char _tmp35A[16U]="cast_expression";static char _tmp35B[17U]="unary_expression";static char _tmp35C[15U]="unary_operator";static char _tmp35D[9U]="asm_expr";static char _tmp35E[13U]="volatile_opt";static char _tmp35F[12U]="asm_out_opt";static char _tmp360[12U]="asm_outlist";static char _tmp361[11U]="asm_in_opt";static char _tmp362[11U]="asm_inlist";static char _tmp363[11U]="asm_io_elt";static char _tmp364[16U]="asm_clobber_opt";static char _tmp365[17U]="asm_clobber_list";static char _tmp366[19U]="postfix_expression";static char _tmp367[17U]="field_expression";static char _tmp368[19U]="primary_expression";static char _tmp369[25U]="argument_expression_list";static char _tmp36A[26U]="argument_expression_list0";static char _tmp36B[9U]="constant";static char _tmp36C[20U]="qual_opt_identifier";static char _tmp36D[17U]="qual_opt_typedef";static char _tmp36E[18U]="struct_union_name";static char _tmp36F[11U]="field_name";static char _tmp370[12U]="right_angle";
# 1491 "parse.y"
static struct _fat_ptr Cyc_yytname[320U]={{_tmp231,_tmp231,_tmp231 + 2U},{_tmp232,_tmp232,_tmp232 + 6U},{_tmp233,_tmp233,_tmp233 + 12U},{_tmp234,_tmp234,_tmp234 + 5U},{_tmp235,_tmp235,_tmp235 + 9U},{_tmp236,_tmp236,_tmp236 + 7U},{_tmp237,_tmp237,_tmp237 + 7U},{_tmp238,_tmp238,_tmp238 + 8U},{_tmp239,_tmp239,_tmp239 + 5U},{_tmp23A,_tmp23A,_tmp23A + 5U},{_tmp23B,_tmp23B,_tmp23B + 6U},{_tmp23C,_tmp23C,_tmp23C + 4U},{_tmp23D,_tmp23D,_tmp23D + 5U},{_tmp23E,_tmp23E,_tmp23E + 6U},{_tmp23F,_tmp23F,_tmp23F + 7U},{_tmp240,_tmp240,_tmp240 + 7U},{_tmp241,_tmp241,_tmp241 + 9U},{_tmp242,_tmp242,_tmp242 + 6U},{_tmp243,_tmp243,_tmp243 + 9U},{_tmp244,_tmp244,_tmp244 + 9U},{_tmp245,_tmp245,_tmp245 + 7U},{_tmp246,_tmp246,_tmp246 + 6U},{_tmp247,_tmp247,_tmp247 + 5U},{_tmp248,_tmp248,_tmp248 + 8U},{_tmp249,_tmp249,_tmp249 + 7U},{_tmp24A,_tmp24A,_tmp24A + 7U},{_tmp24B,_tmp24B,_tmp24B + 9U},{_tmp24C,_tmp24C,_tmp24C + 3U},{_tmp24D,_tmp24D,_tmp24D + 5U},{_tmp24E,_tmp24E,_tmp24E + 7U},{_tmp24F,_tmp24F,_tmp24F + 6U},{_tmp250,_tmp250,_tmp250 + 3U},{_tmp251,_tmp251,_tmp251 + 4U},{_tmp252,_tmp252,_tmp252 + 5U},{_tmp253,_tmp253,_tmp253 + 9U},{_tmp254,_tmp254,_tmp254 + 6U},{_tmp255,_tmp255,_tmp255 + 7U},{_tmp256,_tmp256,_tmp256 + 5U},{_tmp257,_tmp257,_tmp257 + 7U},{_tmp258,_tmp258,_tmp258 + 16U},{_tmp259,_tmp259,_tmp259 + 10U},{_tmp25A,_tmp25A,_tmp25A + 8U},{_tmp25B,_tmp25B,_tmp25B + 4U},{_tmp25C,_tmp25C,_tmp25C + 6U},{_tmp25D,_tmp25D,_tmp25D + 4U},{_tmp25E,_tmp25E,_tmp25E + 6U},{_tmp25F,_tmp25F,_tmp25F + 7U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 5U},{_tmp262,_tmp262,_tmp262 + 4U},{_tmp263,_tmp263,_tmp263 + 9U},{_tmp264,_tmp264,_tmp264 + 9U},{_tmp265,_tmp265,_tmp265 + 6U},{_tmp266,_tmp266,_tmp266 + 10U},{_tmp267,_tmp267,_tmp267 + 9U},{_tmp268,_tmp268,_tmp268 + 7U},{_tmp269,_tmp269,_tmp269 + 8U},{_tmp26A,_tmp26A,_tmp26A + 15U},{_tmp26B,_tmp26B,_tmp26B + 7U},{_tmp26C,_tmp26C,_tmp26C + 8U},{_tmp26D,_tmp26D,_tmp26D + 5U},{_tmp26E,_tmp26E,_tmp26E + 9U},{_tmp26F,_tmp26F,_tmp26F + 6U},{_tmp270,_tmp270,_tmp270 + 7U},{_tmp271,_tmp271,_tmp271 + 5U},{_tmp272,_tmp272,_tmp272 + 8U},{_tmp273,_tmp273,_tmp273 + 7U},{_tmp274,_tmp274,_tmp274 + 8U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 10U},{_tmp277,_tmp277,_tmp277 + 11U},{_tmp278,_tmp278,_tmp278 + 8U},{_tmp279,_tmp279,_tmp279 + 8U},{_tmp27A,_tmp27A,_tmp27A + 10U},{_tmp27B,_tmp27B,_tmp27B + 9U},{_tmp27C,_tmp27C,_tmp27C + 13U},{_tmp27D,_tmp27D,_tmp27D + 10U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 13U},{_tmp280,_tmp280,_tmp280 + 14U},{_tmp281,_tmp281,_tmp281 + 14U},{_tmp282,_tmp282,_tmp282 + 13U},{_tmp283,_tmp283,_tmp283 + 12U},{_tmp284,_tmp284,_tmp284 + 16U},{_tmp285,_tmp285,_tmp285 + 14U},{_tmp286,_tmp286,_tmp286 + 12U},{_tmp287,_tmp287,_tmp287 + 12U},{_tmp288,_tmp288,_tmp288 + 16U},{_tmp289,_tmp289,_tmp289 + 18U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 7U},{_tmp28C,_tmp28C,_tmp28C + 7U},{_tmp28D,_tmp28D,_tmp28D + 8U},{_tmp28E,_tmp28E,_tmp28E + 9U},{_tmp28F,_tmp28F,_tmp28F + 6U},{_tmp290,_tmp290,_tmp290 + 6U},{_tmp291,_tmp291,_tmp291 + 6U},{_tmp292,_tmp292,_tmp292 + 6U},{_tmp293,_tmp293,_tmp293 + 7U},{_tmp294,_tmp294,_tmp294 + 6U},{_tmp295,_tmp295,_tmp295 + 11U},{_tmp296,_tmp296,_tmp296 + 11U},{_tmp297,_tmp297,_tmp297 + 11U},{_tmp298,_tmp298,_tmp298 + 11U},{_tmp299,_tmp299,_tmp299 + 11U},{_tmp29A,_tmp29A,_tmp29A + 12U},{_tmp29B,_tmp29B,_tmp29B + 13U},{_tmp29C,_tmp29C,_tmp29C + 11U},{_tmp29D,_tmp29D,_tmp29D + 11U},{_tmp29E,_tmp29E,_tmp29E + 10U},{_tmp29F,_tmp29F,_tmp29F + 9U},{_tmp2A0,_tmp2A0,_tmp2A0 + 11U},{_tmp2A1,_tmp2A1,_tmp2A1 + 12U},{_tmp2A2,_tmp2A2,_tmp2A2 + 11U},{_tmp2A3,_tmp2A3,_tmp2A3 + 17U},{_tmp2A4,_tmp2A4,_tmp2A4 + 7U},{_tmp2A5,_tmp2A5,_tmp2A5 + 8U},{_tmp2A6,_tmp2A6,_tmp2A6 + 19U},{_tmp2A7,_tmp2A7,_tmp2A7 + 20U},{_tmp2A8,_tmp2A8,_tmp2A8 + 18U},{_tmp2A9,_tmp2A9,_tmp2A9 + 9U},{_tmp2AA,_tmp2AA,_tmp2AA + 13U},{_tmp2AB,_tmp2AB,_tmp2AB + 16U},{_tmp2AC,_tmp2AC,_tmp2AC + 18U},{_tmp2AD,_tmp2AD,_tmp2AD + 10U},{_tmp2AE,_tmp2AE,_tmp2AE + 8U},{_tmp2AF,_tmp2AF,_tmp2AF + 4U},{_tmp2B0,_tmp2B0,_tmp2B0 + 4U},{_tmp2B1,_tmp2B1,_tmp2B1 + 4U},{_tmp2B2,_tmp2B2,_tmp2B2 + 4U},{_tmp2B3,_tmp2B3,_tmp2B3 + 4U},{_tmp2B4,_tmp2B4,_tmp2B4 + 4U},{_tmp2B5,_tmp2B5,_tmp2B5 + 4U},{_tmp2B6,_tmp2B6,_tmp2B6 + 4U},{_tmp2B7,_tmp2B7,_tmp2B7 + 4U},{_tmp2B8,_tmp2B8,_tmp2B8 + 4U},{_tmp2B9,_tmp2B9,_tmp2B9 + 4U},{_tmp2BA,_tmp2BA,_tmp2BA + 4U},{_tmp2BB,_tmp2BB,_tmp2BB + 4U},{_tmp2BC,_tmp2BC,_tmp2BC + 4U},{_tmp2BD,_tmp2BD,_tmp2BD + 4U},{_tmp2BE,_tmp2BE,_tmp2BE + 4U},{_tmp2BF,_tmp2BF,_tmp2BF + 4U},{_tmp2C0,_tmp2C0,_tmp2C0 + 4U},{_tmp2C1,_tmp2C1,_tmp2C1 + 4U},{_tmp2C2,_tmp2C2,_tmp2C2 + 4U},{_tmp2C3,_tmp2C3,_tmp2C3 + 4U},{_tmp2C4,_tmp2C4,_tmp2C4 + 4U},{_tmp2C5,_tmp2C5,_tmp2C5 + 4U},{_tmp2C6,_tmp2C6,_tmp2C6 + 4U},{_tmp2C7,_tmp2C7,_tmp2C7 + 4U},{_tmp2C8,_tmp2C8,_tmp2C8 + 4U},{_tmp2C9,_tmp2C9,_tmp2C9 + 4U},{_tmp2CA,_tmp2CA,_tmp2CA + 5U},{_tmp2CB,_tmp2CB,_tmp2CB + 17U},{_tmp2CC,_tmp2CC,_tmp2CC + 18U},{_tmp2CD,_tmp2CD,_tmp2CD + 19U},{_tmp2CE,_tmp2CE,_tmp2CE + 16U},{_tmp2CF,_tmp2CF,_tmp2CF + 13U},{_tmp2D0,_tmp2D0,_tmp2D0 + 14U},{_tmp2D1,_tmp2D1,_tmp2D1 + 17U},{_tmp2D2,_tmp2D2,_tmp2D2 + 16U},{_tmp2D3,_tmp2D3,_tmp2D3 + 12U},{_tmp2D4,_tmp2D4,_tmp2D4 + 19U},{_tmp2D5,_tmp2D5,_tmp2D5 + 13U},{_tmp2D6,_tmp2D6,_tmp2D6 + 21U},{_tmp2D7,_tmp2D7,_tmp2D7 + 14U},{_tmp2D8,_tmp2D8,_tmp2D8 + 20U},{_tmp2D9,_tmp2D9,_tmp2D9 + 21U},{_tmp2DA,_tmp2DA,_tmp2DA + 13U},{_tmp2DB,_tmp2DB,_tmp2DB + 15U},{_tmp2DC,_tmp2DC,_tmp2DC + 17U},{_tmp2DD,_tmp2DD,_tmp2DD + 19U},{_tmp2DE,_tmp2DE,_tmp2DE + 12U},{_tmp2DF,_tmp2DF,_tmp2DF + 9U},{_tmp2E0,_tmp2E0,_tmp2E0 + 17U},{_tmp2E1,_tmp2E1,_tmp2E1 + 23U},{_tmp2E2,_tmp2E2,_tmp2E2 + 24U},{_tmp2E3,_tmp2E3,_tmp2E3 + 15U},{_tmp2E4,_tmp2E4,_tmp2E4 + 11U},{_tmp2E5,_tmp2E5,_tmp2E5 + 15U},{_tmp2E6,_tmp2E6,_tmp2E6 + 10U},{_tmp2E7,_tmp2E7,_tmp2E7 + 15U},{_tmp2E8,_tmp2E8,_tmp2E8 + 25U},{_tmp2E9,_tmp2E9,_tmp2E9 + 5U},{_tmp2EA,_tmp2EA,_tmp2EA + 15U},{_tmp2EB,_tmp2EB,_tmp2EB + 15U},{_tmp2EC,_tmp2EC,_tmp2EC + 11U},{_tmp2ED,_tmp2ED,_tmp2ED + 22U},{_tmp2EE,_tmp2EE,_tmp2EE + 26U},{_tmp2EF,_tmp2EF,_tmp2EF + 26U},{_tmp2F0,_tmp2F0,_tmp2F0 + 16U},{_tmp2F1,_tmp2F1,_tmp2F1 + 16U},{_tmp2F2,_tmp2F2,_tmp2F2 + 24U},{_tmp2F3,_tmp2F3,_tmp2F3 + 25U},{_tmp2F4,_tmp2F4,_tmp2F4 + 25U},{_tmp2F5,_tmp2F5,_tmp2F5 + 16U},{_tmp2F6,_tmp2F6,_tmp2F6 + 19U},{_tmp2F7,_tmp2F7,_tmp2F7 + 25U},{_tmp2F8,_tmp2F8,_tmp2F8 + 35U},{_tmp2F9,_tmp2F9,_tmp2F9 + 27U},{_tmp2FA,_tmp2FA,_tmp2FA + 18U},{_tmp2FB,_tmp2FB,_tmp2FB + 20U},{_tmp2FC,_tmp2FC,_tmp2FC + 19U},{_tmp2FD,_tmp2FD,_tmp2FD + 14U},{_tmp2FE,_tmp2FE,_tmp2FE + 19U},{_tmp2FF,_tmp2FF,_tmp2FF + 20U},{_tmp300,_tmp300,_tmp300 + 14U},{_tmp301,_tmp301,_tmp301 + 11U},{_tmp302,_tmp302,_tmp302 + 23U},{_tmp303,_tmp303,_tmp303 + 18U},{_tmp304,_tmp304,_tmp304 + 30U},{_tmp305,_tmp305,_tmp305 + 8U},{_tmp306,_tmp306,_tmp306 + 12U},{_tmp307,_tmp307,_tmp307 + 14U},{_tmp308,_tmp308,_tmp308 + 13U},{_tmp309,_tmp309,_tmp309 + 23U},{_tmp30A,_tmp30A,_tmp30A + 14U},{_tmp30B,_tmp30B,_tmp30B + 18U},{_tmp30C,_tmp30C,_tmp30C + 8U},{_tmp30D,_tmp30D,_tmp30D + 11U},{_tmp30E,_tmp30E,_tmp30E + 20U},{_tmp30F,_tmp30F,_tmp30F + 9U},{_tmp310,_tmp310,_tmp310 + 16U},{_tmp311,_tmp311,_tmp311 + 19U},{_tmp312,_tmp312,_tmp312 + 10U},{_tmp313,_tmp313,_tmp313 + 16U},{_tmp314,_tmp314,_tmp314 + 11U},{_tmp315,_tmp315,_tmp315 + 14U},{_tmp316,_tmp316,_tmp316 + 11U},{_tmp317,_tmp317,_tmp317 + 15U},{_tmp318,_tmp318,_tmp318 + 22U},{_tmp319,_tmp319,_tmp319 + 16U},{_tmp31A,_tmp31A,_tmp31A + 17U},{_tmp31B,_tmp31B,_tmp31B + 12U},{_tmp31C,_tmp31C,_tmp31C + 18U},{_tmp31D,_tmp31D,_tmp31D + 17U},{_tmp31E,_tmp31E,_tmp31E + 12U},{_tmp31F,_tmp31F,_tmp31F + 16U},{_tmp320,_tmp320,_tmp320 + 11U},{_tmp321,_tmp321,_tmp321 + 10U},{_tmp322,_tmp322,_tmp322 + 14U},{_tmp323,_tmp323,_tmp323 + 15U},{_tmp324,_tmp324,_tmp324 + 20U},{_tmp325,_tmp325,_tmp325 + 27U},{_tmp326,_tmp326,_tmp326 + 25U},{_tmp327,_tmp327,_tmp327 + 10U},{_tmp328,_tmp328,_tmp328 + 18U},{_tmp329,_tmp329,_tmp329 + 21U},{_tmp32A,_tmp32A,_tmp32A + 19U},{_tmp32B,_tmp32B,_tmp32B + 16U},{_tmp32C,_tmp32C,_tmp32C + 20U},{_tmp32D,_tmp32D,_tmp32D + 15U},{_tmp32E,_tmp32E,_tmp32E + 20U},{_tmp32F,_tmp32F,_tmp32F + 12U},{_tmp330,_tmp330,_tmp330 + 15U},{_tmp331,_tmp331,_tmp331 + 12U},{_tmp332,_tmp332,_tmp332 + 20U},{_tmp333,_tmp333,_tmp333 + 19U},{_tmp334,_tmp334,_tmp334 + 20U},{_tmp335,_tmp335,_tmp335 + 21U},{_tmp336,_tmp336,_tmp336 + 21U},{_tmp337,_tmp337,_tmp337 + 12U},{_tmp338,_tmp338,_tmp338 + 17U},{_tmp339,_tmp339,_tmp339 + 19U},{_tmp33A,_tmp33A,_tmp33A + 14U},{_tmp33B,_tmp33B,_tmp33B + 17U},{_tmp33C,_tmp33C,_tmp33C + 23U},{_tmp33D,_tmp33D,_tmp33D + 13U},{_tmp33E,_tmp33E,_tmp33E + 14U},{_tmp33F,_tmp33F,_tmp33F + 16U},{_tmp340,_tmp340,_tmp340 + 16U},{_tmp341,_tmp341,_tmp341 + 8U},{_tmp342,_tmp342,_tmp342 + 19U},{_tmp343,_tmp343,_tmp343 + 20U},{_tmp344,_tmp344,_tmp344 + 14U},{_tmp345,_tmp345,_tmp345 + 19U},{_tmp346,_tmp346,_tmp346 + 20U},{_tmp347,_tmp347,_tmp347 + 11U},{_tmp348,_tmp348,_tmp348 + 22U},{_tmp349,_tmp349,_tmp349 + 20U},{_tmp34A,_tmp34A,_tmp34A + 23U},{_tmp34B,_tmp34B,_tmp34B + 20U},{_tmp34C,_tmp34C,_tmp34C + 22U},{_tmp34D,_tmp34D,_tmp34D + 23U},{_tmp34E,_tmp34E,_tmp34E + 24U},{_tmp34F,_tmp34F,_tmp34F + 24U},{_tmp350,_tmp350,_tmp350 + 15U},{_tmp351,_tmp351,_tmp351 + 20U},{_tmp352,_tmp352,_tmp352 + 22U},{_tmp353,_tmp353,_tmp353 + 17U},{_tmp354,_tmp354,_tmp354 + 20U},{_tmp355,_tmp355,_tmp355 + 26U},{_tmp356,_tmp356,_tmp356 + 12U},{_tmp357,_tmp357,_tmp357 + 14U},{_tmp358,_tmp358,_tmp358 + 12U},{_tmp359,_tmp359,_tmp359 + 18U},{_tmp35A,_tmp35A,_tmp35A + 16U},{_tmp35B,_tmp35B,_tmp35B + 17U},{_tmp35C,_tmp35C,_tmp35C + 15U},{_tmp35D,_tmp35D,_tmp35D + 9U},{_tmp35E,_tmp35E,_tmp35E + 13U},{_tmp35F,_tmp35F,_tmp35F + 12U},{_tmp360,_tmp360,_tmp360 + 12U},{_tmp361,_tmp361,_tmp361 + 11U},{_tmp362,_tmp362,_tmp362 + 11U},{_tmp363,_tmp363,_tmp363 + 11U},{_tmp364,_tmp364,_tmp364 + 16U},{_tmp365,_tmp365,_tmp365 + 17U},{_tmp366,_tmp366,_tmp366 + 19U},{_tmp367,_tmp367,_tmp367 + 17U},{_tmp368,_tmp368,_tmp368 + 19U},{_tmp369,_tmp369,_tmp369 + 25U},{_tmp36A,_tmp36A,_tmp36A + 26U},{_tmp36B,_tmp36B,_tmp36B + 9U},{_tmp36C,_tmp36C,_tmp36C + 20U},{_tmp36D,_tmp36D,_tmp36D + 17U},{_tmp36E,_tmp36E,_tmp36E + 18U},{_tmp36F,_tmp36F,_tmp36F + 11U},{_tmp370,_tmp370,_tmp370 + 12U}};
# 1551
static short Cyc_yyr1[548U]={0,153,154,154,154,154,154,154,154,154,154,154,154,155,156,157,158,159,159,160,160,160,161,161,162,162,162,163,163,164,164,165,165,165,166,166,167,167,167,167,168,168,169,170,171,172,173,173,173,173,173,173,174,174,175,175,176,176,176,176,176,176,176,176,176,176,176,177,177,177,177,177,177,177,178,178,179,180,180,181,181,181,181,182,182,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,183,184,185,185,185,186,186,186,187,187,188,188,188,189,189,189,190,190,191,191,192,192,193,193,194,194,195,195,196,196,197,198,198,198,198,198,198,199,199,199,199,199,199,200,200,201,201,201,201,202,202,203,203,203,204,204,205,205,205,205,206,206,206,207,207,208,208,209,209,210,210,210,210,210,210,210,210,210,211,211,211,211,211,211,211,211,211,211,212,212,213,214,214,215,215,215,215,215,215,215,215,215,216,216,216,217,217,218,218,218,219,219,219,220,220,221,221,221,221,221,222,222,223,223,224,224,225,225,226,226,227,227,228,228,228,228,229,229,230,230,231,231,231,232,233,233,234,234,235,235,235,235,235,236,236,236,236,237,237,238,238,239,239,240,240,241,241,241,241,241,242,242,243,243,243,244,244,244,244,244,244,244,244,244,245,245,245,245,246,246,246,246,246,246,247,248,248,249,249,250,250,250,250,250,250,250,250,251,251,251,251,251,251,252,252,252,252,252,252,253,253,253,253,254,254,255,255,255,255,255,255,255,255,256,257,257,258,258,259,259,260,260,261,261,262,262,263,263,264,264,265,265,265,266,266,267,267,268,268,269,269,269,269,269,270,271,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,272,273,273,273,274,274,275,275,276,276,276,277,277,278,278,279,279,279,280,280,280,280,280,280,280,280,280,280,280,281,281,281,281,281,281,281,282,283,283,284,284,285,285,286,286,287,287,288,288,289,289,290,290,290,291,291,292,292,293,293,294,294,294,294,295,295,296,296,296,297,297,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,298,299,299,299,299,300,301,301,302,302,302,303,303,304,304,304,305,305,306,307,307,307,308,308,309,309,309,309,309,309,309,309,309,309,309,310,310,310,310,311,311,311,311,311,311,311,311,311,311,311,312,313,313,314,314,314,314,314,315,315,316,316,317,317,318,318,319,319};
# 1609
static short Cyc_yyr2[548U]={0,1,2,3,5,3,5,8,3,3,3,3,0,1,1,2,1,0,4,1,2,3,0,1,4,3,4,2,3,0,4,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,4,0,5,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,8,3,2,1,1,1,0,3,0,1,1,2,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,3,2,2,0,3,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,5,4,4,2,1,1,3,4,4,5,5,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,4,5,4,2,0,4,4,8,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,9,8,0,1,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,4,1,2,4,2,6,1,1,1,3,1,2,1,3,6,6,4,4,5,4,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,1,3,1,1,1,1,1,1,1,1,1,1,1,1,4,1,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,2,2,4,1,1,1,1,5,0,1,0,2,3,1,3,0,2,3,1,3,4,0,1,2,1,3,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,5,4,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1667
static short Cyc_yydefact[1067U]={0,33,67,68,69,70,72,85,86,87,88,89,90,91,92,93,109,110,111,125,126,63,0,0,97,0,0,73,0,0,161,104,106,0,0,0,13,14,0,0,0,538,227,540,539,541,0,212,0,100,0,212,211,1,0,0,0,0,31,0,0,32,0,56,65,59,83,61,94,95,0,124,98,0,0,172,0,195,198,99,176,127,71,70,64,0,113,0,58,536,0,538,533,534,535,537,0,127,0,0,373,0,0,0,250,0,377,375,42,44,0,0,52,0,0,0,0,123,162,0,0,0,209,0,0,0,0,210,0,0,0,2,0,0,0,0,46,0,133,135,57,66,60,62,542,543,127,129,127,0,54,0,0,35,0,229,0,184,173,196,0,202,203,207,208,0,206,205,204,217,198,0,84,71,117,0,115,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,522,523,486,0,0,0,484,0,483,481,482,0,402,404,418,426,428,430,432,434,436,438,440,443,445,458,0,460,504,521,519,538,385,0,0,0,0,386,0,0,376,49,0,0,127,0,0,0,142,138,140,270,272,0,0,0,0,0,8,9,0,544,545,228,108,0,0,0,177,101,248,0,245,10,11,0,3,0,5,0,47,0,0,0,35,122,0,130,131,154,0,159,0,0,0,0,0,0,0,0,0,0,0,0,538,302,304,0,312,306,0,310,295,296,297,0,298,299,300,0,55,35,135,34,36,277,0,235,251,0,0,231,229,0,214,0,0,0,219,74,218,199,0,118,114,0,0,0,467,0,0,479,420,458,0,421,422,0,0,0,0,0,0,0,0,0,0,0,461,462,487,478,0,464,0,0,0,0,463,0,96,0,0,0,0,0,0,447,448,0,451,452,449,450,0,0,0,453,454,0,455,456,457,0,0,408,409,410,411,412,413,414,415,416,417,407,0,465,0,510,511,0,0,0,525,0,127,378,0,0,0,399,538,545,0,0,0,0,266,395,400,0,397,0,0,374,392,393,0,390,252,0,0,0,0,273,0,243,143,148,144,146,139,141,229,0,279,271,280,547,546,0,103,105,0,51,0,107,80,79,0,77,213,178,229,247,174,279,249,185,186,0,102,16,29,43,0,45,0,134,136,254,253,35,37,127,120,132,0,0,0,150,157,0,127,167,0,0,0,0,0,538,0,330,0,333,334,335,0,0,337,0,0,0,313,307,135,311,305,303,38,0,183,236,0,0,0,242,230,237,291,0,222,0,231,182,216,215,179,214,0,0,220,75,128,119,425,116,112,0,0,0,0,538,255,260,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,524,531,0,530,403,427,0,429,431,433,435,437,439,441,442,444,446,406,405,509,506,0,508,0,0,0,388,389,0,269,0,396,264,267,384,0,265,387,381,0,48,0,382,0,274,0,149,145,147,0,0,214,0,281,0,229,0,290,276,0,0,0,0,0,142,0,127,0,229,0,194,175,246,0,22,4,6,39,231,0,153,137,154,0,0,152,160,169,168,0,0,163,0,0,0,320,0,0,0,330,0,331,332,336,0,0,0,308,301,0,40,278,229,0,239,0,0,0,0,181,232,0,235,223,180,200,201,220,197,466,0,0,0,256,0,261,469,0,0,0,0,0,520,474,477,0,0,480,488,0,0,459,528,0,0,507,505,0,0,0,0,268,398,401,391,394,383,275,244,291,282,283,214,0,0,214,0,0,50,538,0,76,78,0,187,0,0,0,214,0,0,0,17,23,129,151,0,155,158,170,167,167,0,0,0,0,0,0,0,0,0,330,320,338,0,309,41,231,0,240,238,0,0,0,231,0,221,516,0,515,0,257,262,0,0,0,0,0,423,424,509,508,493,0,529,512,0,532,419,526,527,0,379,380,287,285,289,291,284,214,53,0,81,188,193,291,192,189,214,0,0,0,0,0,0,0,166,165,314,320,0,0,0,0,0,340,341,343,345,347,349,351,353,355,357,360,362,364,366,371,372,0,0,317,326,0,330,0,0,339,225,241,0,0,233,224,229,468,0,0,263,470,471,0,0,476,475,0,499,493,489,491,485,513,0,288,286,0,191,190,30,25,0,0,35,0,7,121,156,0,0,0,320,0,369,0,0,320,0,0,0,0,0,0,0,0,0,0,0,0,0,367,320,0,0,330,319,292,293,0,231,518,517,0,0,0,0,500,499,496,494,0,490,514,0,26,24,0,27,0,19,171,315,316,0,0,0,0,320,322,344,0,346,348,350,352,354,356,358,359,361,363,0,321,327,0,0,0,234,226,0,0,0,0,0,502,501,0,495,492,0,28,18,20,0,318,368,0,365,323,0,320,329,0,0,259,258,472,0,498,0,497,82,21,0,342,320,324,328,0,0,503,370,325,294,473,0,0,0};
# 1777
static short Cyc_yydefgoto[167U]={1064,53,54,55,56,490,860,987,780,781,931,662,57,320,58,304,59,492,60,494,61,252,151,62,63,558,244,476,477,245,66,259,246,68,174,175,69,70,71,172,282,283,137,138,284,247,458,506,507,673,72,73,677,678,679,74,508,75,482,76,77,169,170,78,122,554,335,716,637,79,327,548,708,540,544,545,452,328,268,103,104,571,497,572,432,433,434,248,321,322,638,464,707,307,308,309,310,311,312,795,313,688,314,872,873,874,875,876,877,878,879,880,881,882,883,884,885,886,887,435,444,445,436,437,438,315,208,412,209,563,210,211,212,213,214,215,216,217,218,219,385,390,395,399,220,221,222,367,368,831,917,918,976,919,978,1024,223,816,224,590,591,225,226,81,932,439,468};
# 1797
static short Cyc_yypact[1067U]={2868,- -32768,- -32768,- -32768,- -32768,58,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3682,162,- 56,- -32768,3682,3076,- -32768,- 36,13,- -32768,105,124,115,94,174,- -32768,- -32768,221,297,234,- -32768,264,- -32768,- -32768,- -32768,246,286,849,311,300,286,- -32768,- -32768,313,318,340,2724,- -32768,396,446,- -32768,867,3682,3682,3682,- -32768,3682,- -32768,- -32768,493,357,- -32768,- 36,3591,345,92,231,959,- -32768,- -32768,368,376,433,- -32768,- 36,463,5793,- -32768,- -32768,3128,253,- -32768,- -32768,- -32768,- -32768,428,368,482,5793,- -32768,477,3128,487,500,506,- -32768,- 7,- -32768,- -32768,3842,3842,523,540,2724,2724,5793,- -32768,- -32768,200,528,5793,- -32768,610,538,200,4162,- -32768,2724,2724,3011,- -32768,2724,3011,2724,3011,- -32768,249,- -32768,3456,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,368,4162,- 51,1681,- -32768,3591,867,555,3842,3762,4889,- -32768,345,- -32768,553,- -32768,- -32768,- -32768,- -32768,560,- -32768,- -32768,- -32768,67,959,3842,- -32768,- -32768,572,579,578,- 36,6108,593,6206,5793,5958,611,617,628,635,659,665,671,673,675,676,679,6206,6206,- -32768,- -32768,771,6258,2426,680,- -32768,6258,- -32768,- -32768,- -32768,48,- -32768,- -32768,- 52,714,672,670,682,607,- 23,416,- 4,143,- -32768,986,6258,180,- 55,- -32768,691,117,- -32768,3128,88,707,1071,710,325,2829,- -32768,- -32768,713,5793,368,2829,705,3922,4162,4242,4162,542,- -32768,- 8,- 8,727,720,709,- -32768,- -32768,358,- -32768,- -32768,- -32768,- -32768,44,719,715,- -32768,- -32768,229,378,- -32768,- -32768,- -32768,723,- -32768,725,- -32768,726,- -32768,610,5002,3591,555,728,729,4162,- -32768,634,- 36,731,733,126,734,4312,735,746,736,744,5115,2281,4312,284,737,- -32768,- -32768,738,1831,1831,867,1831,- -32768,- -32768,- -32768,743,- -32768,- -32768,- -32768,339,- -32768,555,730,- -32768,- -32768,739,78,760,- -32768,66,747,740,408,765,622,754,5793,3842,- -32768,750,- -32768,- -32768,78,- 36,- -32768,5793,751,2426,- -32768,4162,2426,- -32768,- -32768,- -32768,1298,- -32768,782,5793,5793,5793,5793,5793,5793,786,5793,4162,796,5793,- -32768,- -32768,- -32768,- -32768,768,- -32768,1831,769,382,5793,- -32768,5793,- -32768,6258,5793,6258,6258,6258,6258,- -32768,- -32768,6258,- -32768,- -32768,- -32768,- -32768,6258,6258,6258,- -32768,- -32768,6258,- -32768,- -32768,- -32768,6258,5793,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,5793,- -32768,200,- -32768,- -32768,5228,200,5793,- -32768,773,368,- -32768,770,789,791,- -32768,43,428,200,5793,3128,171,- -32768,- -32768,- -32768,797,790,788,3128,- -32768,- -32768,- -32768,792,800,- -32768,429,1071,799,3842,- -32768,804,808,- -32768,4242,4242,4242,- -32768,- -32768,3283,5341,280,- -32768,464,- -32768,- -32768,66,- -32768,- -32768,807,- -32768,831,- -32768,- -32768,812,813,818,- -32768,- -32768,3147,- -32768,584,285,- -32768,- -32768,- -32768,4162,- -32768,- -32768,902,- -32768,2724,- -32768,2724,- -32768,- -32768,- -32768,- -32768,555,- -32768,368,- -32768,- -32768,658,5793,430,- -32768,- 27,164,368,706,5793,5793,817,826,5793,816,925,2131,830,- -32768,- -32768,- -32768,513,915,- -32768,5454,1981,2571,- -32768,- -32768,3456,- -32768,- -32768,- -32768,- -32768,3842,- -32768,- -32768,4162,827,4002,- -32768,- -32768,819,678,66,- -32768,4082,740,- -32768,- -32768,- -32768,- -32768,622,832,35,373,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,833,840,839,862,838,- -32768,- -32768,637,5002,842,852,853,855,857,391,859,860,861,602,863,872,874,6056,- -32768,- -32768,865,875,- -32768,714,187,672,670,682,607,- 23,416,- 4,- 4,143,- -32768,- -32768,- -32768,- -32768,- -32768,868,- -32768,154,3842,4773,4162,- -32768,4162,- -32768,864,- -32768,- -32768,- -32768,- -32768,2685,- -32768,- -32768,- -32768,2972,- -32768,878,- -32768,137,- -32768,4162,- -32768,- -32768,- -32768,873,876,622,871,464,3842,1012,5567,- -32768,- -32768,5793,887,5906,879,44,3315,881,368,3842,3762,5680,- -32768,584,- -32768,890,972,- -32768,- -32768,- -32768,740,764,- -32768,- -32768,634,885,5793,- -32768,- -32768,- -32768,- -32768,916,- 36,520,401,410,5793,716,412,4312,911,5793,922,923,- -32768,- -32768,926,929,921,1831,- -32768,3591,- -32768,739,931,3842,- -32768,930,66,927,928,- -32768,- -32768,934,96,- -32768,- -32768,- -32768,- -32768,373,- -32768,932,141,932,936,- -32768,4425,- -32768,- -32768,5793,5793,1035,5793,5958,- -32768,- -32768,- -32768,200,200,- -32768,938,942,4541,- -32768,- -32768,5793,5793,- -32768,- -32768,78,646,950,951,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,678,- -32768,- -32768,622,78,943,622,939,440,- -32768,940,944,- -32768,- -32768,946,- -32768,78,947,948,622,955,3011,971,1053,- -32768,4162,- -32768,5793,- -32768,- -32768,968,63,706,4312,976,969,1349,967,978,4312,5793,981,5793,716,- -32768,982,- -32768,- -32768,740,209,- -32768,- -32768,5793,5793,990,740,4162,- -32768,- -32768,86,- -32768,5793,- -32768,- -32768,5002,979,985,987,984,- -32768,782,988,989,- 26,992,- -32768,- -32768,655,- -32768,- -32768,- -32768,- -32768,4773,- -32768,- -32768,- -32768,- -32768,- -32768,678,- -32768,622,- -32768,1002,- -32768,- -32768,- -32768,678,- -32768,- -32768,622,1000,226,1004,2724,1001,999,4162,- -32768,- -32768,1117,716,1019,6356,1017,2571,1014,- -32768,- 37,- -32768,1055,1009,639,607,- 23,703,- 4,143,- -32768,- -32768,- -32768,- -32768,1057,6258,1831,- -32768,- -32768,457,5793,1030,1029,- -32768,- -32768,- -32768,1023,1024,1032,- -32768,931,- -32768,281,302,- -32768,- -32768,- -32768,4162,1138,- -32768,- -32768,1031,59,309,- -32768,- -32768,- -32768,- -32768,4657,- -32768,- -32768,1037,- -32768,- -32768,- -32768,- -32768,1039,1040,535,493,- -32768,- -32768,- -32768,492,4312,1042,716,2426,- -32768,4162,1036,1526,6258,5793,6258,6258,6258,6258,6258,6258,6258,6258,6258,6258,5793,- -32768,716,1046,1038,5793,- -32768,1093,- -32768,66,740,- -32768,- -32768,2571,1041,1043,5793,1060,320,- -32768,- -32768,1063,- -32768,- -32768,1065,- -32768,- -32768,493,- -32768,1052,545,- -32768,- -32768,- -32768,1058,1056,1067,6258,716,- -32768,714,333,672,670,670,607,- 23,416,- 4,- 4,143,- -32768,337,- -32768,- -32768,4312,1059,1066,- -32768,- -32768,1069,669,1068,4162,501,- -32768,1073,1063,- -32768,- -32768,1074,- -32768,- -32768,- -32768,493,- -32768,932,141,- -32768,- -32768,5793,1526,- -32768,4312,5793,- -32768,- -32768,- -32768,1077,- -32768,1098,- -32768,- -32768,- -32768,197,- -32768,716,- -32768,- -32768,1079,1080,- -32768,- -32768,- -32768,- -32768,- -32768,1216,1217,- -32768};
# 1907
static short Cyc_yypgoto[167U]={- -32768,- 11,- -32768,- -32768,- -32768,- -32768,- -32768,188,- -32768,- -32768,236,- -32768,- -32768,- 267,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 69,- -32768,- 97,125,- -32768,- -32768,0,568,- -32768,194,- 190,1097,33,- -32768,- -32768,- 138,- -32768,- -32768,1184,56,442,- -32768,- -32768,952,945,- 103,243,- -32768,557,- -32768,- -32768,- -32768,14,- -32768,- -32768,90,- 251,1149,- 410,269,- -32768,1061,- -32768,- -32768,1179,- 395,- -32768,517,- 114,- 161,- 317,- 471,267,525,532,- 453,- 491,- 120,- 371,- 112,- -32768,- 176,- 162,- 579,- 333,- -32768,805,374,- 92,- 140,- 171,- 277,- 708,- 75,- -32768,- -32768,- 46,- 278,- -32768,- 104,- -32768,- 603,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,11,998,- -32768,619,795,- -32768,312,787,- -32768,- 141,- 422,- 150,- 376,- 343,- 356,866,- 344,- 355,- 368,- 332,- 388,361,364,367,363,- 187,453,458,- -32768,- -32768,- -32768,- -32768,335,- -32768,- 871,274,- -32768,892,220,- -32768,- 351,- -32768,192,448,- 68,- 67,- 115,- 95};
# 1931
static short Cyc_yytable[6509U]={64,593,145,146,260,150,267,603,336,618,260,550,369,500,646,481,374,573,249,250,351,64,600,266,596,64,530,531,153,533,599,338,352,67,746,413,595,105,598,342,348,326,279,329,285,977,131,377,123,842,536,703,266,671,67,456,420,64,67,601,602,474,946,64,64,64,609,64,675,676,150,386,387,659,64,157,463,41,87,711,305,171,316,668,798,465,44,421,286,915,67,378,586,280,709,484,67,67,67,659,67,228,496,254,255,317,947,67,1027,388,389,672,916,236,64,64,660,269,270,271,240,272,273,274,275,466,109,241,64,64,64,541,64,64,64,64,467,923,124,64,393,454,756,459,394,926,84,67,67,64,88,64,139,232,1049,469,229,475,157,834,712,67,67,67,543,67,67,67,67,700,714,465,67,82,915,230,694,375,660,467,285,- 544,67,376,67,641,42,42,140,141,142,- 164,143,542,65,782,895,975,486,152,562,561,281,334,287,41,641,537,424,539,316,466,604,263,44,65,518,486,106,65,114,905,325,525,425,906,48,538,112,653,229,- 229,665,499,- 229,305,305,110,305,41,423,557,318,560,759,455,443,113,44,230,785,65,443,653,257,815,111,65,65,65,513,65,258,514,152,456,456,456,65,414,415,416,755,396,306,41,152,41,457,64,573,467,106,375,44,43,44,45,118,85,562,962,397,398,106,744,448,64,504,607,115,305,620,610,64,64,543,64,65,65,430,431,67,257,417,617,375,19,20,418,419,258,65,65,65,742,65,65,65,65,67,792,1060,65,898,559,906,67,67,41,67,903,41,65,899,65,159,43,44,45,43,44,45,467,929,116,930,266,631,47,1014,47,862,480,562,843,229,318,846,461,64,51,52,51,52,276,119,266,277,- 251,120,855,- 251,805,266,230,543,900,901,821,16,17,18,812,257,969,532,723,41,207,739,771,258,67,152,573,43,44,45,267,526,234,654,121,460,486,771,803,527,480,461,106,88,125,106,461,266,106,256,306,306,375,306,106,126,697,486,266,979,128,971,486,904,619,129,699,266,916,80,1025,687,625,924,696,375,455,455,455,975,695,441,927,375,150,645,535,375,130,375,46,86,1038,745,65,107,1039,108,154,613,155,652,663,658,664,147,156,698,375,457,457,457,65,64,473,64,306,80,1017,65,65,171,65,761,- 15,667,80,821,487,391,392,80,375,372,488,709,773,462,588,144,64,375,148,132,133,80,67,729,67,64,762,375,562,64,176,325,483,790,549,107,375,266,375,774,543,775,791,820,796,173,454,107,447,67,266,509,628,669,666,375,670,231,67,80,80,65,67,674,826,1008,375,998,80,134,135,371,848,80,80,80,827,80,80,80,80,654,1005,375,968,46,821,177,715,961,1001,1002,233,642,1004,643,486,80,836,486,817,644,1000,41,1003,524,806,696,235,960,237,43,44,45,453,828,829,1057,487,1006,1007,106,176,305,989,316,238,375,344,106,347,349,349,1047,239,753,691,106,645,375,562,152,908,788,364,365,789,837,804,349,152,251,372,349,152,372,658,253,319,261,820,985,986,844,996,562,562,579,1031,47,264,1032,349,460,107,852,285,107,319,461,107,51,52,65,332,65,107,594,733,415,416,333,64,896,64,634,635,636,339,959,383,384,552,553,340,46,341,266,675,676,65,485,866,655,565,656,566,567,892,65,41,657,80,65,345,67,462,67,611,44,485,510,582,417,515,793,794,47,734,419,937,48,353,820,41,715,483,347,354,51,52,80,43,44,45,705,706,266,1054,355,939,47,721,722,857,504,356,1009,41,505,509,838,839,51,52,64,43,44,45,46,921,922,950,951,176,47,366,145,970,504,357,349,954,955,1044,375,358,51,52,864,865,888,359,543,360,1036,361,362,67,379,363,373,106,381,380,422,106,306,305,152,440,680,681,446,426,684,382,349,689,349,349,349,349,992,89,349,450,470,997,472,349,349,349,471,478,349,934,479,489,349,491,493,501,1011,502,511,520,64,278,521,990,189,145,988,512,516,519,522,534,529,539,46,528,305,41,547,564,107,377,546,467,349,43,44,45,107,65,64,65,1037,67,47,555,107,1053,504,580,551,562,585,614,587,612,51,52,262,41,92,196,197,93,94,95,453,145,44,623,817,509,615,67,616,622,624,626,485,627,346,485,632,201,630,1055,509,633,1040,509,80,647,80,331,648,64,649,651,650,661,1061,682,485,683,685,686,690,485,349,765,692,701,41,704,145,988,1056,713,717,718,305,44,65,46,719,720,- 544,724,67,47,41,725,726,48,727,106,728,736,747,44,748,51,52,136,730,731,732,47,735,689,740,48,737,743,741,749,754,453,757,51,52,758,760,766,769,306,772,778,779,784,7,8,9,10,11,12,13,14,15,16,17,18,19,20,160,161,162,163,164,64,349,165,166,167,786,797,400,168,799,22,23,24,375,800,65,801,802,325,807,738,824,809,810,840,841,498,30,811,818,849,306,107,67,31,32,107,830,832,845,850,847,851,853,854,65,38,401,402,403,404,405,406,407,408,409,410,856,39,858,40,485,859,863,867,868,890,891,894,897,893,902,689,89,912,909,485,925,411,485,556,910,911,323,913,914,349,787,920,928,935,907,933,42,43,936,45,46,498,325,65,574,575,576,577,578,938,940,581,49,50,584,943,945,948,949,958,963,964,965,966,589,967,592,973,306,974,982,983,984,90,991,995,1012,1013,1015,1023,1020,1021,915,1028,1030,427,349,234,428,92,1033,605,93,94,95,1034,429,44,1041,349,1035,1043,97,606,1042,98,1048,1045,589,99,689,100,101,1050,430,431,1058,1059,1062,1063,1065,1066,102,770,1051,1029,265,117,861,158,80,783,503,495,127,337,814,65,1016,813,808,349,621,449,952,107,751,629,953,944,957,597,640,956,1026,889,980,372,583,1052,0,0,0,999,0,0,349,349,0,0,0,0,0,0,1010,0,0,0,0,689,0,0,0,0,0,0,0,1019,0,972,1022,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,144,0,80,0,0,0,0,0,589,993,589,994,0,0,0,0,942,178,179,0,0,0,0,0,568,0,0,0,0,0,0,0,180,89,0,181,349,0,0,1018,0,182,0,0,0,0,0,183,184,185,186,187,0,0,498,0,188,0,0,0,189,0,0,190,191,0,192,0,869,870,0,0,0,0,0,144,0,0,193,0,0,0,194,195,89,0,0,0,0,1046,0,0,0,349,498,349,349,349,349,349,349,349,349,349,349,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,570,0,199,0,0,764,200,144,0,201,768,430,431,0,0,0,202,0,777,203,204,90,349,205,206,0,0,0,0,0,0,0,0,0,0,0,227,92,0,0,93,94,95,589,96,44,0,0,0,0,97,0,0,98,144,0,0,871,0,100,101,0,0,0,0,349,0,202,0,349,102,204,0,0,205,206,0,0,0,0,0,0,0,498,0,0,822,823,0,825,0,0,0,0,0,0,0,0,0,498,0,0,835,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,793,794,21,178,179,288,0,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,498,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,498,0,0,0,0,0,0,0,0,0,0,0,0,300,92,196,197,93,94,95,42,43,44,45,46,198,301,149,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,0,0,0,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,288,498,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,92,196,197,93,94,95,42,43,44,45,46,198,301,149,302,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,288,0,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,300,92,196,197,93,94,95,42,43,44,45,46,198,301,149,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,288,0,289,290,291,292,293,294,295,296,22,23,24,297,89,26,181,298,0,0,0,0,182,27,299,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,517,92,196,197,93,94,95,42,43,44,45,46,198,301,149,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,297,89,26,181,0,0,0,0,0,182,27,0,0,0,30,183,184,185,186,187,0,31,32,33,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,0,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,297,89,0,0,0,0,0,0,0,0,27,0,0,0,30,183,184,185,186,187,0,31,32,0,0,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,0,0,0,199,0,0,0,346,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,89,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,0,31,32,0,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,370,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,178,179,0,0,0,0,0,0,0,0,0,0,22,23,24,180,89,0,181,0,0,0,0,0,182,0,0,0,0,30,183,184,185,186,187,0,31,32,0,188,0,0,0,189,0,0,190,191,38,192,0,0,0,0,0,0,0,0,0,0,39,193,40,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,42,43,44,45,46,198,0,0,0,0,199,0,0,0,200,0,49,303,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,750,0,38,428,92,0,0,93,94,95,0,429,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,430,431,0,0,0,0,0,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,- 12,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,442,0,38,227,92,0,0,93,94,95,0,96,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,0,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,1,89,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,28,29,30,0,0,0,0,90,0,31,32,33,0,0,34,35,0,36,37,752,0,38,227,92,0,0,93,94,95,0,96,44,0,39,0,40,97,0,0,98,0,0,0,99,0,100,101,0,0,0,0,0,0,0,89,0,102,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 12,0,47,0,0,0,48,0,49,50,0,0,0,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,89,0,0,0,0,90,0,0,0,0,0,0,0,0,0,22,23,24,0,0,91,92,0,0,93,94,95,0,96,44,0,0,30,0,97,0,0,98,0,31,32,99,0,100,101,0,0,0,0,0,0,38,0,0,102,0,0,90,0,0,0,0,0,39,0,40,0,0,0,0,0,0,227,92,0,0,93,94,95,0,96,44,0,0,0,0,97,0,323,98,0,41,0,99,0,100,101,0,42,43,44,45,46,0,325,0,102,0,47,0,0,0,480,0,49,50,0,0,461,0,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,7,8,9,10,11,12,13,14,15,16,17,18,19,20,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,22,23,24,0,38,0,0,0,0,0,0,0,0,0,0,0,39,30,40,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,323,0,0,0,0,0,0,39,0,40,42,43,0,45,46,0,325,0,0,0,47,0,0,0,460,0,49,50,0,0,461,0,51,52,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,47,0,0,0,504,0,49,50,0,0,0,0,51,52,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,0,0,149,0,0,0,278,0,0,0,0,49,50,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,2,3,4,83,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,0,49,50,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,323,0,0,324,0,0,0,22,23,24,42,43,0,45,46,0,325,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,242,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,243,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,451,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,702,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,710,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,42,43,0,45,46,0,0,0,0,0,0,0,0,0,30,0,49,50,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,0,40,0,0,0,0,0,0,0,178,179,288,0,289,290,291,292,293,294,295,296,0,0,0,180,89,0,181,298,0,0,0,0,182,42,299,0,0,46,183,184,185,186,187,0,0,0,0,188,0,49,50,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,517,92,196,197,93,94,95,0,0,44,0,0,198,301,149,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,819,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,833,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,981,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,569,92,196,197,93,94,95,0,258,44,0,0,198,0,350,0,0,199,0,0,0,200,0,0,201,0,430,431,178,179,0,202,0,0,203,204,0,0,205,206,0,0,0,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,330,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,350,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,523,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,608,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,639,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,693,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,763,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,776,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,200,0,0,201,178,179,0,0,0,0,202,0,0,203,204,0,0,205,206,180,89,0,181,0,0,0,0,0,182,0,0,0,0,0,183,184,185,186,187,0,0,0,0,188,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,89,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,767,92,196,197,93,94,95,189,0,44,190,191,198,192,0,0,0,199,0,0,0,200,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,178,179,198,0,350,0,0,199,0,0,0,200,0,0,201,180,89,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,89,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,41,92,196,197,93,94,95,189,0,44,190,191,198,192,738,0,0,199,0,0,0,200,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,178,179,198,0,0,0,0,199,0,0,0,343,0,0,201,180,89,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,178,179,0,0,0,0,0,0,0,193,0,0,0,194,195,180,89,0,0,0,0,0,0,0,0,0,0,0,0,0,183,184,185,186,187,0,41,92,196,197,93,94,95,189,0,44,190,191,198,192,0,0,0,199,0,0,0,346,0,0,201,193,0,0,0,194,195,202,0,0,203,204,0,0,205,206,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,178,179,198,0,0,0,0,199,0,0,0,200,0,0,201,180,89,0,0,0,0,202,0,0,203,204,0,0,205,206,183,184,185,186,187,0,0,0,0,0,0,0,0,189,0,0,190,191,0,192,0,0,0,0,0,0,0,0,0,0,0,193,0,0,0,194,195,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,92,196,197,93,94,95,0,0,44,0,0,198,0,0,0,0,199,0,0,0,941,0,0,201,0,0,0,0,0,0,202,0,0,203,204,0,0,205,206};
# 2585
static short Cyc_yycheck[6509U]={0,377,70,70,119,74,126,395,169,431,125,328,199,280,467,266,203,350,110,111,182,21,390,126,380,25,304,305,74,307,385,171,182,0,613,222,379,26,382,177,181,155,139,155,147,916,57,99,48,757,317,542,155,80,21,245,111,57,25,391,392,17,99,63,64,65,417,67,5,6,139,94,95,483,74,75,247,113,134,550,149,132,151,505,687,93,122,142,139,115,57,143,370,139,547,266,63,64,65,509,67,90,278,114,115,151,143,74,979,132,133,138,138,102,114,115,487,128,129,130,127,132,133,134,135,133,113,134,128,129,130,65,132,133,134,135,144,845,48,139,144,244,633,246,148,853,21,114,115,149,25,151,62,97,1025,250,113,113,158,738,555,128,129,130,325,132,133,134,135,540,135,93,139,115,115,132,527,129,549,144,283,138,149,135,151,462,120,120,63,64,65,128,67,127,0,666,799,138,266,74,341,339,146,136,148,113,483,129,120,113,279,133,399,123,122,21,291,285,26,25,126,135,126,298,136,139,134,322,113,480,113,135,499,279,138,304,305,132,307,113,229,333,152,338,639,245,235,132,122,132,672,57,241,504,113,114,132,63,64,65,134,67,121,137,139,455,456,457,74,89,90,91,135,130,149,113,151,113,245,279,613,144,90,129,122,121,122,123,54,127,431,894,149,150,102,141,240,297,134,414,126,370,131,418,304,305,467,307,114,115,139,140,279,113,134,430,129,20,21,139,140,121,128,129,130,138,132,133,134,135,297,682,135,139,805,335,139,304,305,113,307,812,113,149,135,151,77,121,122,123,121,122,123,144,128,134,130,460,450,130,963,130,784,134,505,760,113,277,763,140,370,142,143,142,143,126,112,480,129,126,134,776,129,700,487,132,547,809,810,722,17,18,19,710,113,114,306,573,113,87,587,652,121,370,279,738,121,122,123,529,126,99,480,127,134,483,667,695,134,134,140,229,297,112,232,140,529,235,116,304,305,129,307,241,134,532,504,540,129,126,138,509,813,432,126,537,549,138,0,129,519,440,847,528,129,455,456,457,138,528,135,856,129,532,464,126,129,127,129,124,22,138,612,279,26,138,28,132,422,134,480,492,482,494,127,140,532,129,455,456,457,297,492,135,494,370,48,968,304,305,132,307,642,127,504,57,839,129,92,93,62,129,200,135,967,655,247,135,70,519,129,73,126,127,76,492,135,494,528,643,129,672,532,85,126,266,135,129,90,129,643,129,656,704,656,135,722,135,115,652,102,239,519,656,285,126,126,501,129,129,132,528,114,115,370,532,510,729,956,129,946,123,126,127,200,135,128,129,130,729,132,133,134,135,652,953,129,904,124,922,127,558,135,949,950,113,132,952,134,667,152,742,670,718,140,948,113,951,296,701,685,134,890,126,121,122,123,243,733,734,1042,129,954,955,432,177,695,135,697,129,129,178,440,180,181,182,135,131,627,126,448,641,129,784,519,821,126,194,195,129,745,697,199,528,131,343,203,532,346,659,120,126,134,839,129,932,761,945,809,810,358,126,130,135,129,222,134,229,773,782,232,126,140,235,142,143,492,134,494,241,378,89,90,91,134,695,800,697,455,456,457,129,889,96,97,83,84,128,124,131,813,5,6,519,266,790,132,343,134,345,346,796,528,113,140,277,532,134,695,460,697,419,122,285,286,361,134,289,22,23,130,139,140,863,134,134,922,113,715,480,297,134,142,143,306,121,122,123,80,81,863,1039,134,867,130,128,129,778,134,134,957,113,138,504,128,129,142,143,778,121,122,123,124,128,129,146,147,339,130,18,858,906,134,134,341,92,93,128,129,134,142,143,788,789,793,134,967,134,995,134,134,778,98,134,134,623,146,145,127,627,695,890,697,113,512,513,113,120,516,147,377,519,379,380,381,382,940,41,385,134,113,945,133,390,391,392,126,128,395,860,135,128,399,128,128,127,960,128,127,113,860,131,126,938,68,933,933,134,134,134,126,128,134,113,124,138,945,113,138,128,432,99,135,144,431,121,122,123,440,695,890,697,996,860,130,141,448,1038,134,113,135,1042,134,133,135,132,142,143,121,113,114,115,116,117,118,119,542,985,122,129,1035,652,133,890,133,128,138,135,480,129,134,483,128,137,135,1039,667,129,1013,670,492,134,494,156,113,945,134,129,135,47,1054,134,504,127,138,30,126,509,505,647,45,134,113,144,1032,1032,1041,135,135,129,1039,122,778,124,135,113,138,135,945,130,113,129,129,134,129,793,129,115,614,122,616,142,143,126,135,135,135,130,135,687,135,134,128,135,129,141,128,633,135,142,143,135,141,126,135,890,135,127,46,134,8,9,10,11,12,13,14,15,16,17,18,19,20,21,75,76,77,78,79,1039,587,82,83,84,128,134,60,88,126,37,38,39,129,127,860,126,135,126,128,127,25,134,134,113,113,278,54,133,132,129,945,623,1039,61,62,627,138,135,135,135,141,135,135,135,890,73,100,101,102,103,104,105,106,107,108,109,141,85,127,87,652,48,134,127,135,138,128,126,126,797,120,799,41,129,135,667,114,131,670,332,135,134,110,135,135,672,678,135,128,128,818,127,120,121,135,123,124,350,126,945,353,354,355,356,357,28,127,360,136,137,363,134,138,98,145,98,126,128,135,135,373,129,375,25,1039,134,129,128,128,98,128,135,126,135,81,115,135,134,115,114,128,110,729,871,113,114,128,400,117,118,119,135,121,122,135,742,129,128,127,412,134,130,129,135,417,134,894,136,137,135,139,140,135,115,135,135,0,0,147,651,1032,985,125,39,782,76,778,670,283,277,51,170,715,1039,967,710,704,784,433,241,879,793,623,448,880,871,883,381,461,882,976,793,917,941,362,1035,- 1,- 1,- 1,947,- 1,- 1,809,810,- 1,- 1,- 1,- 1,- 1,- 1,958,- 1,- 1,- 1,- 1,963,- 1,- 1,- 1,- 1,- 1,- 1,- 1,971,- 1,911,974,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,858,- 1,860,- 1,- 1,- 1,- 1,- 1,527,941,529,943,- 1,- 1,- 1,- 1,869,25,26,- 1,- 1,- 1,- 1,- 1,32,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,41,- 1,43,889,- 1,- 1,971,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,573,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,25,26,- 1,- 1,- 1,- 1,- 1,933,- 1,- 1,86,- 1,- 1,- 1,90,91,41,- 1,- 1,- 1,- 1,1021,- 1,- 1,- 1,946,613,948,949,950,951,952,953,954,955,956,957,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,644,134,985,- 1,137,649,139,140,- 1,- 1,- 1,144,- 1,657,147,148,98,995,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,682,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,1032,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,1038,- 1,144,- 1,1042,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,722,- 1,- 1,725,726,- 1,728,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,738,- 1,- 1,741,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,821,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,839,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,922,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,- 1,- 1,49,50,51,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,- 1,- 1,49,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,63,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,54,55,56,57,58,59,- 1,61,62,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,73,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,87,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,120,121,122,123,124,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,139,140,- 1,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,0,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,1,41,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,52,53,54,- 1,- 1,- 1,- 1,98,- 1,61,62,63,- 1,- 1,66,67,- 1,69,70,110,- 1,73,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,85,- 1,87,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,147,- 1,- 1,- 1,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,128,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,54,- 1,127,- 1,- 1,130,- 1,61,62,134,- 1,136,137,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,147,- 1,- 1,98,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,113,114,- 1,- 1,117,118,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,110,130,- 1,113,- 1,134,- 1,136,137,- 1,120,121,122,123,124,- 1,126,- 1,147,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,- 1,142,143,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,8,9,10,11,12,13,14,15,16,17,18,19,20,21,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,54,87,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,140,- 1,142,143,- 1,113,- 1,- 1,- 1,- 1,- 1,- 1,120,121,122,123,124,- 1,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,136,137,- 1,- 1,- 1,- 1,142,143,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,- 1,- 1,- 1,131,- 1,- 1,- 1,- 1,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,120,121,- 1,123,124,- 1,- 1,127,37,38,39,40,- 1,- 1,- 1,- 1,136,137,- 1,- 1,- 1,50,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,113,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,126,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,65,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,127,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,128,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,110,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,120,121,- 1,123,124,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,54,- 1,136,137,- 1,- 1,- 1,61,62,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,- 1,- 1,49,120,51,- 1,- 1,124,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,136,137,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,128,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,121,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,139,140,25,26,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,126,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,141,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,25,26,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,40,41,- 1,43,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,- 1,- 1,- 1,64,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,127,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,40,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,55,56,57,58,59,- 1,113,114,115,116,117,118,119,68,- 1,122,71,72,125,74,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,86,- 1,- 1,- 1,90,91,144,- 1,- 1,147,148,- 1,- 1,151,152,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,25,26,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,40,41,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152,55,56,57,58,59,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,68,- 1,- 1,71,72,- 1,74,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,- 1,- 1,- 1,90,91,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,113,114,115,116,117,118,119,- 1,- 1,122,- 1,- 1,125,- 1,- 1,- 1,- 1,130,- 1,- 1,- 1,134,- 1,- 1,137,- 1,- 1,- 1,- 1,- 1,- 1,144,- 1,- 1,147,148,- 1,- 1,151,152};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
extern void Cyc_yyerror(struct _fat_ptr,int,int);
# 82 "cycbison.simple"
extern int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*,struct Cyc_Yyltype*);struct Cyc_Yystacktype{union Cyc_YYSTYPE v;struct Cyc_Yyltype l;};struct _tuple33{unsigned f1;struct _tuple0*f2;int f3;};struct _tuple34{void*f1;void*f2;};struct _tuple35{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 145 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 148
struct _RegionHandle _tmp371=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp371;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp6CF;(_tmp6CF.YYINITIALSVAL).tag=73U,(_tmp6CF.YYINITIALSVAL).val=0;_tmp6CF;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp611=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp611),sizeof(short),_tmp611);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp610=200U;struct Cyc_Yystacktype*_tmp60F=({struct _RegionHandle*_tmp77D=yyregion;_region_malloc(_tmp77D,_check_times(_tmp610,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp6CE=200U;unsigned i;for(i=0;i < _tmp6CE;++ i){(_tmp60F[i]).v=yylval,({struct Cyc_Yyltype _tmp77E=Cyc_yynewloc();(_tmp60F[i]).l=_tmp77E;});}}0;});_tmp60F;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp780=({const char*_tmp372="parser stack overflow";_tag_fat(_tmp372,sizeof(char),22U);});int _tmp77F=yystate;Cyc_yyerror(_tmp780,_tmp77F,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp376=(unsigned)yystacksize;short*_tmp375=({struct _RegionHandle*_tmp781=yyregion;_region_malloc(_tmp781,_check_times(_tmp376,sizeof(short)));});({{unsigned _tmp684=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp684;++ i){
i <= (unsigned)yyssp_offset?_tmp375[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp375[i]=0);}}0;});_tag_fat(_tmp375,sizeof(short),_tmp376);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp374=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp373=({struct _RegionHandle*_tmp782=yyregion;_region_malloc(_tmp782,_check_times(_tmp374,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp683=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp683;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp373[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp373[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp373,sizeof(struct Cyc_Yystacktype),_tmp374);});
# 230
yyss=yyss1;
yyvs=yyvs1;}}
# 240
goto yybackup;
# 242
yybackup:
# 254 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1067U,sizeof(short),yystate));
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
yychar1=yychar > 0 && yychar <= 380?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,381U,sizeof(short),yychar)): 320;}
# 299 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0 || yyn > 6508)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),yyn))!= yychar1)goto yydefault;
# 302
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,6509U,sizeof(short),yyn));
# 309
if(yyn < 0){
# 311
if(yyn == -32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 315
if(yyn == 0)goto yyerrlab;}
# 317
if(yyn == 1066){
int _tmp377=0;_npop_handler(0);return _tmp377;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp783=({struct Cyc_Yystacktype _tmp685;_tmp685.v=yylval,_tmp685.l=yylloc;_tmp685;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp783;});
# 338
if(yyerrstatus != 0)-- yyerrstatus;
# 340
yystate=yyn;
goto yynewstate;
# 344
yydefault:
# 346
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefact,1067U,sizeof(short),yystate));
if(yyn == 0)goto yyerrlab;
# 351
yyreduce:
# 353
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,548U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=(yyyvsp[0]).v;
# 370 "cycbison.simple"
{int _tmp378=yyn;switch((int)_tmp378){case 1:
# 1109 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1112
goto _LL0;case 2:
# 1115 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp784=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp784,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3:
# 1119 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp37B=_cycalloc(sizeof(*_tmp37B));({struct Cyc_Absyn_Decl*_tmp788=({struct Cyc_Absyn_Decl*_tmp37A=_cycalloc(sizeof(*_tmp37A));({void*_tmp787=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->tag=10,({struct _tuple0*_tmp786=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp379->f1=_tmp786;}),({struct Cyc_List_List*_tmp785=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp379->f2=_tmp785;});_tmp379;});_tmp37A->r=_tmp787;}),_tmp37A->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp37A;});_tmp37B->hd=_tmp788;}),_tmp37B->tl=0;_tmp37B;}));
Cyc_Lex_leave_using();
# 1122
goto _LL0;case 4:
# 1123 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp37E=_cycalloc(sizeof(*_tmp37E));({struct Cyc_Absyn_Decl*_tmp78D=({struct Cyc_Absyn_Decl*_tmp37D=_cycalloc(sizeof(*_tmp37D));({void*_tmp78C=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->tag=10,({struct _tuple0*_tmp78B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp37C->f1=_tmp78B;}),({struct Cyc_List_List*_tmp78A=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp37C->f2=_tmp78A;});_tmp37C;});_tmp37D->r=_tmp78C;}),_tmp37D->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp37D;});_tmp37E->hd=_tmp78D;}),({struct Cyc_List_List*_tmp789=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp37E->tl=_tmp789;});_tmp37E;}));
goto _LL0;case 5:
# 1126
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp382=_cycalloc(sizeof(*_tmp382));({struct Cyc_Absyn_Decl*_tmp792=({struct Cyc_Absyn_Decl*_tmp381=_cycalloc(sizeof(*_tmp381));({void*_tmp791=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->tag=9,({struct _fat_ptr*_tmp790=({struct _fat_ptr*_tmp37F=_cycalloc(sizeof(*_tmp37F));({struct _fat_ptr _tmp78F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp37F=_tmp78F;});_tmp37F;});_tmp380->f1=_tmp790;}),({struct Cyc_List_List*_tmp78E=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp380->f2=_tmp78E;});_tmp380;});_tmp381->r=_tmp791;}),_tmp381->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp381;});_tmp382->hd=_tmp792;}),_tmp382->tl=0;_tmp382;}));
Cyc_Lex_leave_namespace();
# 1129
goto _LL0;case 6:
# 1130 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp386=_cycalloc(sizeof(*_tmp386));({struct Cyc_Absyn_Decl*_tmp798=({struct Cyc_Absyn_Decl*_tmp385=_cycalloc(sizeof(*_tmp385));({void*_tmp797=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384->tag=9,({struct _fat_ptr*_tmp796=({struct _fat_ptr*_tmp383=_cycalloc(sizeof(*_tmp383));({struct _fat_ptr _tmp795=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp383=_tmp795;});_tmp383;});_tmp384->f1=_tmp796;}),({struct Cyc_List_List*_tmp794=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp384->f2=_tmp794;});_tmp384;});_tmp385->r=_tmp797;}),_tmp385->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp385;});_tmp386->hd=_tmp798;}),({struct Cyc_List_List*_tmp793=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp386->tl=_tmp793;});_tmp386;}));
goto _LL0;case 7:  {
# 1132 "parse.y"
int _tmp387=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_c_include=_tmp387;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple29*_tmp388=Cyc_yyget_YY54(&(yyyvsp[5]).v);struct _tuple29*_stmttmp13=_tmp388;struct _tuple29*_tmp389=_stmttmp13;unsigned _tmp38B;void*_tmp38A;_tmp38A=_tmp389->f1;_tmp38B=_tmp389->f2;{struct Cyc_List_List*exs=_tmp38A;unsigned wc=_tmp38B;
struct Cyc_List_List*_tmp38C=Cyc_yyget_YY55(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp38C;
if(exs != 0 && hides != 0)
({void*_tmp38D=0U;({unsigned _tmp79A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp799=({const char*_tmp38E="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp38E,sizeof(char),65U);});Cyc_Warn_err(_tmp79A,_tmp799,_tag_fat(_tmp38D,sizeof(void*),0));});});
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp38F=0U;({unsigned _tmp79C=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp79B=({const char*_tmp390="expecting \"C include\"";_tag_fat(_tmp390,sizeof(char),22U);});Cyc_Warn_err(_tmp79C,_tmp79B,_tag_fat(_tmp38F,sizeof(void*),0));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp394=_cycalloc(sizeof(*_tmp394));({struct Cyc_Absyn_Decl*_tmp7A1=({struct Cyc_Absyn_Decl*_tmp393=_cycalloc(sizeof(*_tmp393));({void*_tmp7A0=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->tag=12,({struct Cyc_List_List*_tmp79F=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp392->f1=_tmp79F;}),_tmp392->f2=cycdecls,_tmp392->f3=exs,({struct _tuple10*_tmp79E=({struct _tuple10*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->f1=wc,_tmp391->f2=hides;_tmp391;});_tmp392->f4=_tmp79E;});_tmp392;});_tmp393->r=_tmp7A0;}),_tmp393->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp393;});_tmp394->hd=_tmp7A1;}),({struct Cyc_List_List*_tmp79D=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp394->tl=_tmp79D;});_tmp394;}));}else{
# 1146
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp397=_cycalloc(sizeof(*_tmp397));({struct Cyc_Absyn_Decl*_tmp7A5=({struct Cyc_Absyn_Decl*_tmp396=_cycalloc(sizeof(*_tmp396));({void*_tmp7A4=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->tag=11,({struct Cyc_List_List*_tmp7A3=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp395->f1=_tmp7A3;});_tmp395;});_tmp396->r=_tmp7A4;}),_tmp396->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp396;});_tmp397->hd=_tmp7A5;}),({struct Cyc_List_List*_tmp7A2=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp397->tl=_tmp7A2;});_tmp397;}));}}else{
# 1149
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct Cyc_Absyn_Decl*_tmp7AA=({struct Cyc_Absyn_Decl*_tmp39A=_cycalloc(sizeof(*_tmp39A));({void*_tmp7A9=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp399=_cycalloc(sizeof(*_tmp399));_tmp399->tag=12,({struct Cyc_List_List*_tmp7A8=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp399->f1=_tmp7A8;}),_tmp399->f2=cycdecls,_tmp399->f3=exs,({struct _tuple10*_tmp7A7=({struct _tuple10*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->f1=wc,_tmp398->f2=hides;_tmp398;});_tmp399->f4=_tmp7A7;});_tmp399;});_tmp39A->r=_tmp7A9;}),_tmp39A->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39A;});_tmp39B->hd=_tmp7AA;}),({struct Cyc_List_List*_tmp7A6=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp39B->tl=_tmp7A6;});_tmp39B;}));}
# 1151
goto _LL0;}}case 8:
# 1152 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39D=_cycalloc(sizeof(*_tmp39D));({struct Cyc_Absyn_Decl*_tmp7AC=({struct Cyc_Absyn_Decl*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp39C->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39C;});_tmp39D->hd=_tmp7AC;}),({struct Cyc_List_List*_tmp7AB=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39D->tl=_tmp7AB;});_tmp39D;}));
goto _LL0;case 9:
# 1154 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39F=_cycalloc(sizeof(*_tmp39F));({struct Cyc_Absyn_Decl*_tmp7AE=({struct Cyc_Absyn_Decl*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp39E->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39E;});_tmp39F->hd=_tmp7AE;}),({struct Cyc_List_List*_tmp7AD=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39F->tl=_tmp7AD;});_tmp39F;}));
goto _LL0;case 10:
# 1156 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_Absyn_Decl*_tmp7B0=({struct Cyc_Absyn_Decl*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3A0->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A0;});_tmp3A1->hd=_tmp7B0;}),({struct Cyc_List_List*_tmp7AF=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A1->tl=_tmp7AF;});_tmp3A1;}));
goto _LL0;case 11:
# 1158 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_Absyn_Decl*_tmp7B2=({struct Cyc_Absyn_Decl*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3A2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A2;});_tmp3A3->hd=_tmp7B2;}),({struct Cyc_List_List*_tmp7B1=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A3->tl=_tmp7B1;});_tmp3A3;}));
goto _LL0;case 12:
# 1159 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13:
# 1163 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14:
# 1166
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15:
# 1171 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp7B3=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp7B3,({const char*_tmp3A4="C";_tag_fat(_tmp3A4,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _tmp7B4=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp7B4,({const char*_tmp3A5="C include";_tag_fat(_tmp3A5,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY32(1);else{
# 1177
({void*_tmp3A6=0U;({unsigned _tmp7B6=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp7B5=({const char*_tmp3A7="expecting \"C\" or \"C include\"";_tag_fat(_tmp3A7,sizeof(char),29U);});Cyc_Warn_err(_tmp7B6,_tmp7B5,_tag_fat(_tmp3A6,sizeof(void*),0));});});
yyval=Cyc_YY32(1);}}
# 1181
goto _LL0;case 16:
# 1184 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17:
# 1188 "parse.y"
 yyval=Cyc_YY55(0);
goto _LL0;case 18:
# 1189 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19:
# 1193 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({struct _tuple0*_tmp7B7=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3A8->hd=_tmp7B7;}),_tmp3A8->tl=0;_tmp3A8;}));
goto _LL0;case 20:
# 1194 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({struct _tuple0*_tmp7B8=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3A9->hd=_tmp7B8;}),_tmp3A9->tl=0;_tmp3A9;}));
goto _LL0;case 21:
# 1195 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));({struct _tuple0*_tmp7BA=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3AA->hd=_tmp7BA;}),({struct Cyc_List_List*_tmp7B9=Cyc_yyget_YY55(&(yyyvsp[2]).v);_tmp3AA->tl=_tmp7B9;});_tmp3AA;}));
goto _LL0;case 22:
# 1199 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->f1=0,_tmp3AB->f2=0U;_tmp3AB;}));
goto _LL0;case 23:
# 1200 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24:
# 1204 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({struct Cyc_List_List*_tmp7BB=Cyc_yyget_YY53(&(yyyvsp[2]).v);_tmp3AC->f1=_tmp7BB;}),_tmp3AC->f2=0U;_tmp3AC;}));
goto _LL0;case 25:
# 1205 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->f1=0,_tmp3AD->f2=0U;_tmp3AD;}));
goto _LL0;case 26:
# 1206 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->f1=0,_tmp3AE->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AE;}));
goto _LL0;case 27:
# 1211 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));({struct _tuple33*_tmp7BD=({struct _tuple33*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp7BC=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3AF->f2=_tmp7BC;}),_tmp3AF->f3=0;_tmp3AF;});_tmp3B0->hd=_tmp7BD;}),_tmp3B0->tl=0;_tmp3B0;}));
goto _LL0;case 28:
# 1213 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct _tuple33*_tmp7C0=({struct _tuple33*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp7BF=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3B1->f2=_tmp7BF;}),_tmp3B1->f3=0;_tmp3B1;});_tmp3B2->hd=_tmp7C0;}),({struct Cyc_List_List*_tmp7BE=Cyc_yyget_YY53(&(yyyvsp[2]).v);_tmp3B2->tl=_tmp7BE;});_tmp3B2;}));
goto _LL0;case 29:
# 1217 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30:
# 1218 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 31:
# 1222 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));({struct Cyc_Absyn_Decl*_tmp7C3=({void*_tmp7C2=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->tag=1,({struct Cyc_Absyn_Fndecl*_tmp7C1=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3B3->f1=_tmp7C1;});_tmp3B3;});Cyc_Absyn_new_decl(_tmp7C2,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3B4->hd=_tmp7C3;}),_tmp3B4->tl=0;_tmp3B4;}));
goto _LL0;case 32:
# 1223 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 33:
# 1224 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36:
# 1234 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp7C6=yyr;struct Cyc_Parse_Declarator _tmp7C5=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp7C4=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp7C6,0,_tmp7C5,0,_tmp7C4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 37:  {
# 1236 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3B5=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3B5;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7C9=yyr;struct Cyc_Parse_Declarator _tmp7C8=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7C7=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7C9,& d,_tmp7C8,0,_tmp7C7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 38:
# 1248 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp7CD=yyr;struct Cyc_Parse_Declarator _tmp7CC=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp7CB=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7CA=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7CD,0,_tmp7CC,_tmp7CB,_tmp7CA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 39:  {
# 1250 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3B6=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3B6;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7D1=yyr;struct Cyc_Parse_Declarator _tmp7D0=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp7CF=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp7CE=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp7D1,& d,_tmp7D0,_tmp7CF,_tmp7CE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 40:  {
# 1258 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3B7=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3B7;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7D4=yyr;struct Cyc_Parse_Declarator _tmp7D3=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7D2=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7D4,& d,_tmp7D3,0,_tmp7D2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41:  {
# 1261 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3B8=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3B8;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7D8=yyr;struct Cyc_Parse_Declarator _tmp7D7=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp7D6=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp7D5=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp7D8,& d,_tmp7D7,_tmp7D6,_tmp7D5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42:
# 1266 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 43:
# 1269
 Cyc_Lex_leave_using();
goto _LL0;case 44:
# 1272
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct _fat_ptr _tmp7D9=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3B9=_tmp7D9;});_tmp3B9;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 45:
# 1275
 Cyc_Lex_leave_namespace();
goto _LL0;case 46:
# 1281 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp7DB=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp7DA=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp7DB,0,_tmp7DA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 47:
# 1283 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp7DE=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple11*_tmp7DD=((struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[1]).v));unsigned _tmp7DC=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp7DE,_tmp7DD,_tmp7DC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 48:
# 1286
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({struct Cyc_Absyn_Decl*_tmp7E1=({struct Cyc_Absyn_Pat*_tmp7E0=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp7DF=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp7E0,_tmp7DF,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3BA->hd=_tmp7E1;}),_tmp3BA->tl=0;_tmp3BA;}));
goto _LL0;case 49:  {
# 1288 "parse.y"
struct Cyc_List_List*_tmp3BB=0;struct Cyc_List_List*vds=_tmp3BB;
{struct Cyc_List_List*_tmp3BC=Cyc_yyget_YY37(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3BC;for(0;ids != 0;ids=ids->tl){
struct _tuple0*qv=({struct _tuple0*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({union Cyc_Absyn_Nmspace _tmp7E2=Cyc_Absyn_Rel_n(0);_tmp3BE->f1=_tmp7E2;}),_tmp3BE->f2=(struct _fat_ptr*)ids->hd;_tmp3BE;});
vds=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({struct Cyc_Absyn_Vardecl*_tmp7E4=({struct _tuple0*_tmp7E3=qv;Cyc_Absyn_new_vardecl(0U,_tmp7E3,Cyc_Absyn_wildtyp(0),0);});_tmp3BD->hd=_tmp7E4;}),_tmp3BD->tl=vds;_tmp3BD;});}}
# 1293
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct Cyc_Absyn_Decl*_tmp7E6=({struct Cyc_List_List*_tmp7E5=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(vds);Cyc_Absyn_letv_decl(_tmp7E5,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3BF->hd=_tmp7E6;}),_tmp3BF->tl=0;_tmp3BF;}));
# 1295
goto _LL0;}case 50:
# 1298 "parse.y"
({struct _fat_ptr _tmp7E7=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmp7E7,(unsigned)((yyyvsp[2]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct _fat_ptr*_tmp7EA=({struct _fat_ptr*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct _fat_ptr _tmp7E9=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp3C3=_tmp7E9;});_tmp3C3;});_tmp3C4->name=_tmp7EA;}),_tmp3C4->identity=- 1,({void*_tmp7E8=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3C4->kind=_tmp7E8;});_tmp3C4;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp7EE=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp7ED=({struct _tuple0*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp7EC=({struct _fat_ptr*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));({struct _fat_ptr _tmp7EB=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3C1=_tmp7EB;});_tmp3C1;});_tmp3C2->f2=_tmp7EC;});_tmp3C2;});Cyc_Absyn_new_vardecl(_tmp7EE,_tmp7ED,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct Cyc_Absyn_Decl*_tmp7EF=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3C0->hd=_tmp7EF;}),_tmp3C0->tl=0;_tmp3C0;}));
# 1304
goto _LL0;}case 51:  {
# 1306
struct _fat_ptr tvstring=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3CC=({struct Cyc_String_pa_PrintArg_struct _tmp686;_tmp686.tag=0,({struct _fat_ptr _tmp7F0=(struct _fat_ptr)((struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v));_tmp686.f1=_tmp7F0;});_tmp686;});void*_tmp3CA[1];_tmp3CA[0]=& _tmp3CC;({struct _fat_ptr _tmp7F1=({const char*_tmp3CB="`%s";_tag_fat(_tmp3CB,sizeof(char),4U);});Cyc_aprintf(_tmp7F1,_tag_fat(_tmp3CA,sizeof(void*),1));});});
({struct _fat_ptr _tmp7F2=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_Parse_tvar_ok(_tmp7F2,(unsigned)((yyyvsp[1]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({struct _fat_ptr*_tmp7F4=({struct _fat_ptr*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));*_tmp3C8=tvstring;_tmp3C8;});_tmp3C9->name=_tmp7F4;}),_tmp3C9->identity=- 1,({void*_tmp7F3=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3C9->kind=_tmp7F3;});_tmp3C9;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp7F8=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp7F7=({struct _tuple0*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp7F6=({struct _fat_ptr*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct _fat_ptr _tmp7F5=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3C6=_tmp7F5;});_tmp3C6;});_tmp3C7->f2=_tmp7F6;});_tmp3C7;});Cyc_Absyn_new_vardecl(_tmp7F8,_tmp7F7,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct Cyc_Absyn_Decl*_tmp7FC=({struct Cyc_Absyn_Tvar*_tmp7FB=tv;struct Cyc_Absyn_Vardecl*_tmp7FA=vd;struct Cyc_Absyn_Exp*_tmp7F9=Cyc_yyget_YY58(&(yyyvsp[2]).v);Cyc_Absyn_region_decl(_tmp7FB,_tmp7FA,_tmp7F9,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3C5->hd=_tmp7FC;}),_tmp3C5->tl=0;_tmp3C5;}));
# 1313
goto _LL0;}}case 52:
# 1316 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 53:
# 1318 "parse.y"
 if(({struct _fat_ptr _tmp7FD=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp7FD,({const char*_tmp3CD="open";_tag_fat(_tmp3CD,sizeof(char),5U);}));})!= 0)({void*_tmp3CE=0U;({unsigned _tmp7FF=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp7FE=({const char*_tmp3CF="expecting `open'";_tag_fat(_tmp3CF,sizeof(char),17U);});Cyc_Warn_err(_tmp7FF,_tmp7FE,_tag_fat(_tmp3CE,sizeof(void*),0));});});
yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[3]).v));
goto _LL0;case 54:
# 1322
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55:
# 1323 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp800=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp800,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56:
# 1329 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp687;({enum Cyc_Parse_Storage_class _tmp803=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp687.sc=_tmp803;}),({struct Cyc_Absyn_Tqual _tmp802=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp687.tq=_tmp802;}),({
struct Cyc_Parse_Type_specifier _tmp801=Cyc_Parse_empty_spec(0U);_tmp687.type_specs=_tmp801;}),_tmp687.is_inline=0,_tmp687.attributes=0;_tmp687;}));
goto _LL0;case 57:  {
# 1332 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D0=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3D0;
if((int)two.sc != (int)Cyc_Parse_None_sc)
({void*_tmp3D1=0U;({unsigned _tmp805=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp804=({const char*_tmp3D2="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp3D2,sizeof(char),73U);});Cyc_Warn_warn(_tmp805,_tmp804,_tag_fat(_tmp3D1,sizeof(void*),0));});});
# 1336
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp688;({enum Cyc_Parse_Storage_class _tmp806=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp688.sc=_tmp806;}),_tmp688.tq=two.tq,_tmp688.type_specs=two.type_specs,_tmp688.is_inline=two.is_inline,_tmp688.attributes=two.attributes;_tmp688;}));
# 1339
goto _LL0;}case 58:
# 1340 "parse.y"
({void*_tmp3D3=0U;({unsigned _tmp808=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp807=({const char*_tmp3D4="__extension__ keyword ignored in declaration";_tag_fat(_tmp3D4,sizeof(char),45U);});Cyc_Warn_warn(_tmp808,_tmp807,_tag_fat(_tmp3D3,sizeof(void*),0));});});
yyval=(yyyvsp[1]).v;
# 1343
goto _LL0;case 59:
# 1344 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp689;_tmp689.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp80A=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp689.tq=_tmp80A;}),({
struct Cyc_Parse_Type_specifier _tmp809=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp689.type_specs=_tmp809;}),_tmp689.is_inline=0,_tmp689.attributes=0;_tmp689;}));
goto _LL0;case 60:  {
# 1347 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D5=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3D5;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp68A;_tmp68A.sc=two.sc,_tmp68A.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp80D=({unsigned _tmp80C=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp80B=two.type_specs;Cyc_Parse_combine_specifiers(_tmp80C,_tmp80B,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp68A.type_specs=_tmp80D;}),_tmp68A.is_inline=two.is_inline,_tmp68A.attributes=two.attributes;_tmp68A;}));
# 1352
goto _LL0;}case 61:
# 1353 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp68B;_tmp68B.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp80F=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp68B.tq=_tmp80F;}),({struct Cyc_Parse_Type_specifier _tmp80E=Cyc_Parse_empty_spec(0U);_tmp68B.type_specs=_tmp80E;}),_tmp68B.is_inline=0,_tmp68B.attributes=0;_tmp68B;}));
goto _LL0;case 62:  {
# 1355 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D6=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3D6;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp68C;_tmp68C.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp811=({struct Cyc_Absyn_Tqual _tmp810=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp810,two.tq);});_tmp68C.tq=_tmp811;}),_tmp68C.type_specs=two.type_specs,_tmp68C.is_inline=two.is_inline,_tmp68C.attributes=two.attributes;_tmp68C;}));
# 1359
goto _LL0;}case 63:
# 1360 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp68D;_tmp68D.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp813=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp68D.tq=_tmp813;}),({
struct Cyc_Parse_Type_specifier _tmp812=Cyc_Parse_empty_spec(0U);_tmp68D.type_specs=_tmp812;}),_tmp68D.is_inline=1,_tmp68D.attributes=0;_tmp68D;}));
goto _LL0;case 64:  {
# 1363 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D7=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3D7;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp68E;_tmp68E.sc=two.sc,_tmp68E.tq=two.tq,_tmp68E.type_specs=two.type_specs,_tmp68E.is_inline=1,_tmp68E.attributes=two.attributes;_tmp68E;}));
# 1366
goto _LL0;}case 65:
# 1367 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp68F;_tmp68F.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp816=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp68F.tq=_tmp816;}),({
struct Cyc_Parse_Type_specifier _tmp815=Cyc_Parse_empty_spec(0U);_tmp68F.type_specs=_tmp815;}),_tmp68F.is_inline=0,({struct Cyc_List_List*_tmp814=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp68F.attributes=_tmp814;});_tmp68F;}));
goto _LL0;case 66:  {
# 1370 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D8=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3D8;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp690;_tmp690.sc=two.sc,_tmp690.tq=two.tq,_tmp690.type_specs=two.type_specs,_tmp690.is_inline=two.is_inline,({
# 1373
struct Cyc_List_List*_tmp818=({struct Cyc_List_List*_tmp817=Cyc_yyget_YY46(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp817,two.attributes);});_tmp690.attributes=_tmp818;});_tmp690;}));
goto _LL0;}case 67:
# 1377 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Auto_sc);
goto _LL0;case 68:
# 1378 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Register_sc);
goto _LL0;case 69:
# 1379 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Static_sc);
goto _LL0;case 70:
# 1380 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Extern_sc);
goto _LL0;case 71:
# 1382 "parse.y"
 if(({struct _fat_ptr _tmp819=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp819,({const char*_tmp3D9="C";_tag_fat(_tmp3D9,sizeof(char),2U);}));})!= 0)
({void*_tmp3DA=0U;({unsigned _tmp81B=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp81A=({const char*_tmp3DB="only extern or extern \"C\" is allowed";_tag_fat(_tmp3DB,sizeof(char),37U);});Cyc_Warn_err(_tmp81B,_tmp81A,_tag_fat(_tmp3DA,sizeof(void*),0));});});
yyval=Cyc_YY20(Cyc_Parse_ExternC_sc);
# 1386
goto _LL0;case 72:
# 1386 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Typedef_sc);
goto _LL0;case 73:
# 1388 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Abstract_sc);
goto _LL0;case 74:
# 1393 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 75:
# 1394 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76:
# 1398 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77:
# 1401
 yyval=Cyc_YY46(({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));({void*_tmp81C=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp3DC->hd=_tmp81C;}),_tmp3DC->tl=0;_tmp3DC;}));
goto _LL0;case 78:
# 1402 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({void*_tmp81E=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp3DD->hd=_tmp81E;}),({struct Cyc_List_List*_tmp81D=Cyc_yyget_YY46(&(yyyvsp[2]).v);_tmp3DD->tl=_tmp81D;});_tmp3DD;}));
goto _LL0;case 79:
# 1405
 yyval=Cyc_YY47(({unsigned _tmp81F=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Atts_parse_nullary_att(_tmp81F,Cyc_yyget_String_tok(&(yyyvsp[0]).v));}));
goto _LL0;case 80:
# 1406 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81:
# 1408 "parse.y"
 yyval=Cyc_YY47(({unsigned _tmp822=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp821=Cyc_yyget_String_tok(&(yyyvsp[0]).v);unsigned _tmp820=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Atts_parse_unary_att(_tmp822,_tmp821,_tmp820,Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));}));
goto _LL0;case 82:
# 1410 "parse.y"
 yyval=Cyc_YY47(({unsigned _tmp829=(unsigned)((yyyvsp[0]).l).first_line;unsigned _tmp828=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp827=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr _tmp826=Cyc_yyget_String_tok(&(yyyvsp[2]).v);unsigned _tmp825=({
unsigned _tmp823=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp823,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});
# 1410
Cyc_Atts_parse_format_att(_tmp829,_tmp828,_tmp827,_tmp826,_tmp825,({
# 1412
unsigned _tmp824=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp824,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));}));}));
goto _LL0;case 83:
# 1422 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84:
# 1424 "parse.y"
 yyval=Cyc_YY21(({void*_tmp82B=({struct _tuple0*_tmp82A=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp82A,Cyc_yyget_YY41(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp82B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85:
# 1428 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86:
# 1429 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87:
# 1430 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88:
# 1431 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89:
# 1432 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90:
# 1433 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91:
# 1434 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92:
# 1435 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93:
# 1436 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94:
# 1437 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95:
# 1438 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96:
# 1440 "parse.y"
 yyval=Cyc_YY21(({void*_tmp82C=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp82C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97:
# 1442 "parse.y"
 yyval=Cyc_YY21(({void*_tmp82D=Cyc_Absyn_builtin_type(({const char*_tmp3DE="__builtin_va_list";_tag_fat(_tmp3DE,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_tmp82D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98:
# 1444 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99:
# 1446 "parse.y"
 yyval=Cyc_YY21(({void*_tmp82E=Cyc_yyget_YY45(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp82E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100:
# 1448 "parse.y"
 yyval=Cyc_YY21(({void*_tmp82F=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp82F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101:
# 1450 "parse.y"
 yyval=Cyc_YY21(({void*_tmp830=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp830,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102:
# 1452 "parse.y"
 yyval=Cyc_YY21(({void*_tmp833=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));_tmp3DF->tag=6,({struct Cyc_List_List*_tmp832=({unsigned _tmp831=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)((struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ,_tmp831,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[2]).v)));});_tmp3DF->f1=_tmp832;});_tmp3DF;});Cyc_Parse_type_spec(_tmp833,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1454
goto _LL0;case 103:
# 1455 "parse.y"
 yyval=Cyc_YY21(({void*_tmp834=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp834,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104:
# 1457 "parse.y"
 yyval=Cyc_YY21(({void*_tmp835=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_tmp835,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 105:
# 1459 "parse.y"
 yyval=Cyc_YY21(({void*_tmp836=Cyc_Absyn_tag_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp836,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106:
# 1461 "parse.y"
 yyval=Cyc_YY21(({void*_tmp837=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_tmp837,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107:
# 1463 "parse.y"
 yyval=Cyc_YY21(({void*_tmp838=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp838,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108:
# 1468 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp839=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_tmp839,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109:  {
# 1472 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp691;_tmp691.print_const=1,_tmp691.q_volatile=0,_tmp691.q_restrict=0,_tmp691.real_const=1,_tmp691.loc=loc;_tmp691;}));
goto _LL0;}case 110:
# 1474 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp692;_tmp692.print_const=0,_tmp692.q_volatile=1,_tmp692.q_restrict=0,_tmp692.real_const=0,_tmp692.loc=0U;_tmp692;}));
goto _LL0;case 111:
# 1475 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp693;_tmp693.print_const=0,_tmp693.q_volatile=0,_tmp693.q_restrict=1,_tmp693.real_const=0,_tmp693.loc=0U;_tmp693;}));
goto _LL0;case 112:  {
# 1481 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp3E0=({struct Cyc_Absyn_TypeDecl*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));({void*_tmp83E=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));_tmp3E4->tag=1,({struct Cyc_Absyn_Enumdecl*_tmp83D=({struct Cyc_Absyn_Enumdecl*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp83C=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp3E3->name=_tmp83C;}),({struct Cyc_Core_Opt*_tmp83B=({struct Cyc_Core_Opt*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));({struct Cyc_List_List*_tmp83A=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp3E2->v=_tmp83A;});_tmp3E2;});_tmp3E3->fields=_tmp83B;});_tmp3E3;});_tmp3E4->f1=_tmp83D;});_tmp3E4;});_tmp3E5->r=_tmp83E;}),_tmp3E5->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3E5;});struct Cyc_Absyn_TypeDecl*ed=_tmp3E0;
# 1483
yyval=Cyc_YY21(({void*_tmp83F=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));_tmp3E1->tag=10,_tmp3E1->f1=ed,_tmp3E1->f2=0;_tmp3E1;});Cyc_Parse_type_spec(_tmp83F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1485
goto _LL0;}case 113:
# 1486 "parse.y"
 yyval=Cyc_YY21(({void*_tmp840=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp840,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114:
# 1488 "parse.y"
 yyval=Cyc_YY21(({void*_tmp841=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp841,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115:
# 1494 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({struct _tuple0*_tmp842=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3E6->name=_tmp842;}),_tmp3E6->tag=0,_tmp3E6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3E6;}));
goto _LL0;case 116:
# 1496 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));({struct _tuple0*_tmp844=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3E7->name=_tmp844;}),({struct Cyc_Absyn_Exp*_tmp843=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp3E7->tag=_tmp843;}),_tmp3E7->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3E7;}));
goto _LL0;case 117:
# 1500 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));({struct Cyc_Absyn_Enumfield*_tmp845=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp3E8->hd=_tmp845;}),_tmp3E8->tl=0;_tmp3E8;}));
goto _LL0;case 118:
# 1501 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({struct Cyc_Absyn_Enumfield*_tmp846=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp3E9->hd=_tmp846;}),_tmp3E9->tl=0;_tmp3E9;}));
goto _LL0;case 119:
# 1502 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({struct Cyc_Absyn_Enumfield*_tmp848=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp3EA->hd=_tmp848;}),({struct Cyc_List_List*_tmp847=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp3EA->tl=_tmp847;});_tmp3EA;}));
goto _LL0;case 120:
# 1508 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84B=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->tag=7,({enum Cyc_Absyn_AggrKind _tmp84A=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp3EB->f1=_tmp84A;}),({struct Cyc_List_List*_tmp849=Cyc_yyget_YY25(&(yyyvsp[2]).v);_tmp3EB->f2=_tmp849;});_tmp3EB;});Cyc_Parse_type_spec(_tmp84B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121:  {
# 1512 "parse.y"
struct Cyc_List_List*_tmp3EC=({unsigned _tmp84C=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp84C,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp3EC;
struct Cyc_List_List*_tmp3ED=({unsigned _tmp84D=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp84D,Cyc_yyget_YY41(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp3ED;
struct Cyc_Absyn_TypeDecl*_tmp3EE=({enum Cyc_Absyn_AggrKind _tmp854=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_tmp853=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp852=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp851=({
struct Cyc_List_List*_tmp850=exist_ts;struct Cyc_List_List*_tmp84F=Cyc_yyget_YY51(&(yyyvsp[5]).v);struct Cyc_List_List*_tmp84E=Cyc_yyget_YY25(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp850,_tmp84F,_tmp84E,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1);});
# 1514
Cyc_Absyn_aggr_tdecl(_tmp854,Cyc_Absyn_Public,_tmp853,_tmp852,_tmp851,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp3EE;
# 1517
yyval=Cyc_YY21(({void*_tmp855=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF->tag=10,_tmp3EF->f1=td,_tmp3EF->f2=0;_tmp3EF;});Cyc_Parse_type_spec(_tmp855,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1519
goto _LL0;}case 122:
# 1520 "parse.y"
 yyval=Cyc_YY21(({void*_tmp859=({union Cyc_Absyn_AggrInfo _tmp858=({enum Cyc_Absyn_AggrKind _tmp857=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_tmp856=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_UnknownAggr(_tmp857,_tmp856,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1?({struct Cyc_Core_Opt*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->v=(void*)1;_tmp3F0;}): 0);});Cyc_Absyn_aggr_type(_tmp858,
Cyc_yyget_YY41(&(yyyvsp[2]).v));});
# 1520
Cyc_Parse_type_spec(_tmp859,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1522
goto _LL0;case 123:
# 1525 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _tmp694;_tmp694.f1=1,({enum Cyc_Absyn_AggrKind _tmp85A=Cyc_yyget_YY22(&(yyyvsp[1]).v);_tmp694.f2=_tmp85A;});_tmp694;}));
goto _LL0;case 124:
# 1526 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _tmp695;_tmp695.f1=0,({enum Cyc_Absyn_AggrKind _tmp85B=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp695.f2=_tmp85B;});_tmp695;}));
goto _LL0;case 125:
# 1529
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 126:
# 1530 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 127:
# 1533
 yyval=Cyc_YY41(0);
goto _LL0;case 128:
# 1534 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 129:
# 1539 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 130:  {
# 1541 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp3F1=Cyc_yyget_YY26(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp3F1;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1545
struct Cyc_List_List*_tmp3F2=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp3F2;
if(tags != 0)
((void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1550
goto _LL0;}}case 131:
# 1554 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));({struct Cyc_List_List*_tmp85C=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp3F3->hd=_tmp85C;}),_tmp3F3->tl=0;_tmp3F3;}));
goto _LL0;case 132:
# 1555 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));({struct Cyc_List_List*_tmp85E=Cyc_yyget_YY25(&(yyyvsp[1]).v);_tmp3F4->hd=_tmp85E;}),({struct Cyc_List_List*_tmp85D=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp3F4->tl=_tmp85D;});_tmp3F4;}));
goto _LL0;case 133:
# 1561 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_tmp3F5=_region_malloc(yyr,sizeof(*_tmp3F5));_tmp3F5->tl=0,({struct _tuple12 _tmp85F=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp3F5->hd=_tmp85F;});_tmp3F5;}));
goto _LL0;case 134:
# 1563 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_tmp3F6=_region_malloc(yyr,sizeof(*_tmp3F6));({struct _tuple11*_tmp861=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp3F6->tl=_tmp861;}),({struct _tuple12 _tmp860=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp3F6->hd=_tmp860;});_tmp3F6;}));
goto _LL0;case 135:
# 1567 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp696;({struct Cyc_Parse_Declarator _tmp862=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp696.f1=_tmp862;}),_tmp696.f2=0;_tmp696;}));
goto _LL0;case 136:
# 1568 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp697;({struct Cyc_Parse_Declarator _tmp864=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp697.f1=_tmp864;}),({struct Cyc_Absyn_Exp*_tmp863=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp697.f2=_tmp863;});_tmp697;}));
goto _LL0;case 137:  {
# 1573 "parse.y"
struct _RegionHandle _tmp3F7=_new_region("temp");struct _RegionHandle*temp=& _tmp3F7;_push_region(temp);
{struct _tuple27 _tmp3F8=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp14=_tmp3F8;struct _tuple27 _tmp3F9=_stmttmp14;void*_tmp3FC;struct Cyc_Parse_Type_specifier _tmp3FB;struct Cyc_Absyn_Tqual _tmp3FA;_tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;_tmp3FC=_tmp3F9.f3;{struct Cyc_Absyn_Tqual tq=_tmp3FA;struct Cyc_Parse_Type_specifier tspecs=_tmp3FB;struct Cyc_List_List*atts=_tmp3FC;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp3FD=Cyc_yyget_YY30(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp3FD;for(0;x != 0;x=x->tl){
struct _tuple26*_tmp3FE=(struct _tuple26*)x->hd;struct _tuple26*_stmttmp15=_tmp3FE;struct _tuple26*_tmp3FF=_stmttmp15;void*_tmp402;void*_tmp401;struct Cyc_Parse_Declarator _tmp400;_tmp400=_tmp3FF->f1;_tmp401=_tmp3FF->f2;_tmp402=_tmp3FF->f3;{struct Cyc_Parse_Declarator d=_tmp400;struct Cyc_Absyn_Exp*wd=_tmp401;struct Cyc_Absyn_Exp*wh=_tmp402;
decls=({struct _tuple13*_tmp403=_region_malloc(temp,sizeof(*_tmp403));_tmp403->tl=decls,_tmp403->hd=d;_tmp403;});
widths_and_reqs=({struct Cyc_List_List*_tmp405=_region_malloc(temp,sizeof(*_tmp405));({struct _tuple17*_tmp865=({struct _tuple17*_tmp404=_region_malloc(temp,sizeof(*_tmp404));_tmp404->f1=wd,_tmp404->f2=wh;_tmp404;});_tmp405->hd=_tmp865;}),_tmp405->tl=widths_and_reqs;_tmp405;});}}}{
# 1583
void*_tmp406=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp406;
struct Cyc_List_List*_tmp407=({struct _RegionHandle*_tmp868=temp;struct _RegionHandle*_tmp867=temp;struct Cyc_List_List*_tmp866=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1584
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(_tmp868,_tmp867,_tmp866,widths_and_reqs);});struct Cyc_List_List*info=_tmp407;
# 1586
yyval=Cyc_YY25(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple18*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1588
_npop_handler(0);goto _LL0;}}}}
# 1574
;_pop_region();}case 138:
# 1595 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp698;({struct Cyc_Absyn_Tqual _tmp86A=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp698.f1=_tmp86A;}),({struct Cyc_Parse_Type_specifier _tmp869=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp698.f2=_tmp869;}),_tmp698.f3=0;_tmp698;}));
goto _LL0;case 139:  {
# 1597 "parse.y"
struct _tuple27 _tmp408=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp408;yyval=Cyc_YY36(({struct _tuple27 _tmp699;_tmp699.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp86D=({unsigned _tmp86C=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp86B=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp86C,_tmp86B,two.f2);});_tmp699.f2=_tmp86D;}),_tmp699.f3=two.f3;_tmp699;}));
goto _LL0;}case 140:
# 1599 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp69A;({struct Cyc_Absyn_Tqual _tmp86F=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp69A.f1=_tmp86F;}),({struct Cyc_Parse_Type_specifier _tmp86E=Cyc_Parse_empty_spec(0U);_tmp69A.f2=_tmp86E;}),_tmp69A.f3=0;_tmp69A;}));
goto _LL0;case 141:  {
# 1601 "parse.y"
struct _tuple27 _tmp409=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp409;yyval=Cyc_YY36(({struct _tuple27 _tmp69B;({struct Cyc_Absyn_Tqual _tmp871=({struct Cyc_Absyn_Tqual _tmp870=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp870,two.f1);});_tmp69B.f1=_tmp871;}),_tmp69B.f2=two.f2,_tmp69B.f3=two.f3;_tmp69B;}));
goto _LL0;}case 142:
# 1603 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp69C;({struct Cyc_Absyn_Tqual _tmp874=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp69C.f1=_tmp874;}),({struct Cyc_Parse_Type_specifier _tmp873=Cyc_Parse_empty_spec(0U);_tmp69C.f2=_tmp873;}),({struct Cyc_List_List*_tmp872=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp69C.f3=_tmp872;});_tmp69C;}));
goto _LL0;case 143:  {
# 1605 "parse.y"
struct _tuple27 _tmp40A=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp40A;yyval=Cyc_YY36(({struct _tuple27 _tmp69D;_tmp69D.f1=two.f1,_tmp69D.f2=two.f2,({struct Cyc_List_List*_tmp876=({struct Cyc_List_List*_tmp875=Cyc_yyget_YY46(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp875,two.f3);});_tmp69D.f3=_tmp876;});_tmp69D;}));
goto _LL0;}case 144:
# 1611 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp69E;({struct Cyc_Absyn_Tqual _tmp878=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp69E.f1=_tmp878;}),({struct Cyc_Parse_Type_specifier _tmp877=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp69E.f2=_tmp877;}),_tmp69E.f3=0;_tmp69E;}));
goto _LL0;case 145:  {
# 1613 "parse.y"
struct _tuple27 _tmp40B=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp40B;yyval=Cyc_YY36(({struct _tuple27 _tmp69F;_tmp69F.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp87B=({unsigned _tmp87A=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp879=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp87A,_tmp879,two.f2);});_tmp69F.f2=_tmp87B;}),_tmp69F.f3=two.f3;_tmp69F;}));
goto _LL0;}case 146:
# 1615 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6A0;({struct Cyc_Absyn_Tqual _tmp87D=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp6A0.f1=_tmp87D;}),({struct Cyc_Parse_Type_specifier _tmp87C=Cyc_Parse_empty_spec(0U);_tmp6A0.f2=_tmp87C;}),_tmp6A0.f3=0;_tmp6A0;}));
goto _LL0;case 147:  {
# 1617 "parse.y"
struct _tuple27 _tmp40C=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp40C;yyval=Cyc_YY36(({struct _tuple27 _tmp6A1;({struct Cyc_Absyn_Tqual _tmp87F=({struct Cyc_Absyn_Tqual _tmp87E=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp87E,two.f1);});_tmp6A1.f1=_tmp87F;}),_tmp6A1.f2=two.f2,_tmp6A1.f3=two.f3;_tmp6A1;}));
goto _LL0;}case 148:
# 1619 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6A2;({struct Cyc_Absyn_Tqual _tmp882=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6A2.f1=_tmp882;}),({struct Cyc_Parse_Type_specifier _tmp881=Cyc_Parse_empty_spec(0U);_tmp6A2.f2=_tmp881;}),({struct Cyc_List_List*_tmp880=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp6A2.f3=_tmp880;});_tmp6A2;}));
goto _LL0;case 149:  {
# 1621 "parse.y"
struct _tuple27 _tmp40D=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp40D;yyval=Cyc_YY36(({struct _tuple27 _tmp6A3;_tmp6A3.f1=two.f1,_tmp6A3.f2=two.f2,({struct Cyc_List_List*_tmp884=({struct Cyc_List_List*_tmp883=Cyc_yyget_YY46(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp883,two.f3);});_tmp6A3.f3=_tmp884;});_tmp6A3;}));
goto _LL0;}case 150:
# 1627 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_tmp40E=_region_malloc(yyr,sizeof(*_tmp40E));({struct _tuple26*_tmp885=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp40E->hd=_tmp885;}),_tmp40E->tl=0;_tmp40E;}));
goto _LL0;case 151:
# 1629 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_tmp40F=_region_malloc(yyr,sizeof(*_tmp40F));({struct _tuple26*_tmp887=Cyc_yyget_YY29(&(yyyvsp[2]).v);_tmp40F->hd=_tmp887;}),({struct Cyc_List_List*_tmp886=Cyc_yyget_YY30(&(yyyvsp[0]).v);_tmp40F->tl=_tmp886;});_tmp40F;}));
goto _LL0;case 152:
# 1634 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp410=_region_malloc(yyr,sizeof(*_tmp410));({struct Cyc_Parse_Declarator _tmp889=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp410->f1=_tmp889;}),_tmp410->f2=0,({struct Cyc_Absyn_Exp*_tmp888=Cyc_yyget_YY58(&(yyyvsp[1]).v);_tmp410->f3=_tmp888;});_tmp410;}));
goto _LL0;case 153:
# 1638 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp414=_region_malloc(yyr,sizeof(*_tmp414));({struct _tuple0*_tmp88E=({struct _tuple0*_tmp413=_cycalloc(sizeof(*_tmp413));({union Cyc_Absyn_Nmspace _tmp88D=Cyc_Absyn_Rel_n(0);_tmp413->f1=_tmp88D;}),({struct _fat_ptr*_tmp88C=({struct _fat_ptr*_tmp412=_cycalloc(sizeof(*_tmp412));({struct _fat_ptr _tmp88B=({const char*_tmp411="";_tag_fat(_tmp411,sizeof(char),1U);});*_tmp412=_tmp88B;});_tmp412;});_tmp413->f2=_tmp88C;});_tmp413;});(_tmp414->f1).id=_tmp88E;}),(_tmp414->f1).varloc=0U,(_tmp414->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp88A=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp414->f2=_tmp88A;}),_tmp414->f3=0;_tmp414;}));
# 1640
goto _LL0;case 154:
# 1643 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp418=_region_malloc(yyr,sizeof(*_tmp418));({struct _tuple0*_tmp892=({struct _tuple0*_tmp417=_cycalloc(sizeof(*_tmp417));({union Cyc_Absyn_Nmspace _tmp891=Cyc_Absyn_Rel_n(0);_tmp417->f1=_tmp891;}),({struct _fat_ptr*_tmp890=({struct _fat_ptr*_tmp416=_cycalloc(sizeof(*_tmp416));({struct _fat_ptr _tmp88F=({const char*_tmp415="";_tag_fat(_tmp415,sizeof(char),1U);});*_tmp416=_tmp88F;});_tmp416;});_tmp417->f2=_tmp890;});_tmp417;});(_tmp418->f1).id=_tmp892;}),(_tmp418->f1).varloc=0U,(_tmp418->f1).tms=0,_tmp418->f2=0,_tmp418->f3=0;_tmp418;}));
# 1645
goto _LL0;case 155:
# 1646 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp419=_region_malloc(yyr,sizeof(*_tmp419));({struct Cyc_Parse_Declarator _tmp894=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp419->f1=_tmp894;}),({struct Cyc_Absyn_Exp*_tmp893=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp419->f2=_tmp893;}),_tmp419->f3=0;_tmp419;}));
goto _LL0;case 156:
# 1650 "parse.y"
 yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 157:
# 1651 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 158:  {
# 1657 "parse.y"
int _tmp41A=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp41A;
struct Cyc_List_List*_tmp41B=({unsigned _tmp895=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp895,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp41B;
struct Cyc_Absyn_TypeDecl*_tmp41C=({struct _tuple0*_tmp89A=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp899=ts;struct Cyc_Core_Opt*_tmp898=({struct Cyc_Core_Opt*_tmp41E=_cycalloc(sizeof(*_tmp41E));({struct Cyc_List_List*_tmp896=Cyc_yyget_YY35(&(yyyvsp[4]).v);_tmp41E->v=_tmp896;});_tmp41E;});int _tmp897=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp89A,_tmp899,_tmp898,_tmp897,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp41C;
# 1661
yyval=Cyc_YY21(({void*_tmp89B=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->tag=10,_tmp41D->f1=dd,_tmp41D->f2=0;_tmp41D;});Cyc_Parse_type_spec(_tmp89B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1663
goto _LL0;}case 159:  {
# 1664 "parse.y"
int _tmp41F=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp41F;
yyval=Cyc_YY21(({void*_tmp89E=({union Cyc_Absyn_DatatypeInfo _tmp89D=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp6A4;({struct _tuple0*_tmp89C=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp6A4.name=_tmp89C;}),_tmp6A4.is_extensible=is_extensible;_tmp6A4;}));Cyc_Absyn_datatype_type(_tmp89D,Cyc_yyget_YY41(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp89E,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1667
goto _LL0;}case 160:  {
# 1668 "parse.y"
int _tmp420=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp420;
yyval=Cyc_YY21(({void*_tmp8A2=({union Cyc_Absyn_DatatypeFieldInfo _tmp8A1=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp6A5;({struct _tuple0*_tmp8A0=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp6A5.datatype_name=_tmp8A0;}),({struct _tuple0*_tmp89F=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp6A5.field_name=_tmp89F;}),_tmp6A5.is_extensible=is_extensible;_tmp6A5;}));Cyc_Absyn_datatype_field_type(_tmp8A1,Cyc_yyget_YY41(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp8A2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1671
goto _LL0;}case 161:
# 1674 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 162:
# 1675 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 163:
# 1679 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));({struct Cyc_Absyn_Datatypefield*_tmp8A3=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp421->hd=_tmp8A3;}),_tmp421->tl=0;_tmp421;}));
goto _LL0;case 164:
# 1680 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));({struct Cyc_Absyn_Datatypefield*_tmp8A4=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp422->hd=_tmp8A4;}),_tmp422->tl=0;_tmp422;}));
goto _LL0;case 165:
# 1681 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp423=_cycalloc(sizeof(*_tmp423));({struct Cyc_Absyn_Datatypefield*_tmp8A6=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp423->hd=_tmp8A6;}),({struct Cyc_List_List*_tmp8A5=Cyc_yyget_YY35(&(yyyvsp[2]).v);_tmp423->tl=_tmp8A5;});_tmp423;}));
goto _LL0;case 166:
# 1682 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp424=_cycalloc(sizeof(*_tmp424));({struct Cyc_Absyn_Datatypefield*_tmp8A8=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp424->hd=_tmp8A8;}),({struct Cyc_List_List*_tmp8A7=Cyc_yyget_YY35(&(yyyvsp[2]).v);_tmp424->tl=_tmp8A7;});_tmp424;}));
goto _LL0;case 167:
# 1686 "parse.y"
 yyval=Cyc_YY33(Cyc_Absyn_Public);
goto _LL0;case 168:
# 1687 "parse.y"
 yyval=Cyc_YY33(Cyc_Absyn_Extern);
goto _LL0;case 169:
# 1688 "parse.y"
 yyval=Cyc_YY33(Cyc_Absyn_Static);
goto _LL0;case 170:
# 1692 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_tmp425=_cycalloc(sizeof(*_tmp425));({struct _tuple0*_tmp8AA=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp425->name=_tmp8AA;}),_tmp425->typs=0,_tmp425->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp8A9=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp425->sc=_tmp8A9;});_tmp425;}));
goto _LL0;case 171:  {
# 1694 "parse.y"
struct Cyc_List_List*_tmp426=({unsigned _tmp8AB=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)((struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ,_tmp8AB,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp426;
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_tmp427=_cycalloc(sizeof(*_tmp427));({struct _tuple0*_tmp8AD=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp427->name=_tmp8AD;}),_tmp427->typs=typs,_tmp427->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp8AC=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp427->sc=_tmp8AC;});_tmp427;}));
goto _LL0;}case 172:
# 1700 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 173:  {
# 1702 "parse.y"
struct Cyc_Parse_Declarator _tmp428=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp428;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6A6;_tmp6A6.id=two.id,_tmp6A6.varloc=two.varloc,({struct Cyc_List_List*_tmp8AF=({struct Cyc_List_List*_tmp8AE=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp8AE,two.tms);});_tmp6A6.tms=_tmp8AF;});_tmp6A6;}));
goto _LL0;}case 174:
# 1709 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 175:  {
# 1711 "parse.y"
struct Cyc_Parse_Declarator _tmp429=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp429;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6A7;_tmp6A7.id=two.id,_tmp6A7.varloc=two.varloc,({struct Cyc_List_List*_tmp8B1=({struct Cyc_List_List*_tmp8B0=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp8B0,two.tms);});_tmp6A7.tms=_tmp8B1;});_tmp6A7;}));
goto _LL0;}case 176:
# 1716 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6A8;({struct _tuple0*_tmp8B2=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6A8.id=_tmp8B2;}),_tmp6A8.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6A8.tms=0;_tmp6A8;}));
goto _LL0;case 177:
# 1717 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 178:  {
# 1721 "parse.y"
struct Cyc_Parse_Declarator _tmp42A=Cyc_yyget_YY28(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp42A;
({struct Cyc_List_List*_tmp8B5=({struct Cyc_List_List*_tmp42C=_region_malloc(yyr,sizeof(*_tmp42C));({void*_tmp8B4=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp42B=_region_malloc(yyr,sizeof(*_tmp42B));_tmp42B->tag=5,_tmp42B->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8B3=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp42B->f2=_tmp8B3;});_tmp42B;});_tmp42C->hd=_tmp8B4;}),_tmp42C->tl=d.tms;_tmp42C;});d.tms=_tmp8B5;});
yyval=(yyyvsp[2]).v;
# 1725
goto _LL0;}case 179:
# 1726 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6A9;({struct _tuple0*_tmp8BB=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6A9.id=_tmp8BB;}),({unsigned _tmp8BA=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6A9.varloc=_tmp8BA;}),({struct Cyc_List_List*_tmp8B9=({struct Cyc_List_List*_tmp42E=_region_malloc(yyr,sizeof(*_tmp42E));({void*_tmp8B8=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp42D=_region_malloc(yyr,sizeof(*_tmp42D));_tmp42D->tag=0,({void*_tmp8B7=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp42D->f1=_tmp8B7;}),_tmp42D->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp42D;});_tmp42E->hd=_tmp8B8;}),({struct Cyc_List_List*_tmp8B6=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp42E->tl=_tmp8B6;});_tmp42E;});_tmp6A9.tms=_tmp8B9;});_tmp6A9;}));
goto _LL0;case 180:
# 1728 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6AA;({struct _tuple0*_tmp8C2=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6AA.id=_tmp8C2;}),({unsigned _tmp8C1=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6AA.varloc=_tmp8C1;}),({
struct Cyc_List_List*_tmp8C0=({struct Cyc_List_List*_tmp430=_region_malloc(yyr,sizeof(*_tmp430));({void*_tmp8BF=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp42F=_region_malloc(yyr,sizeof(*_tmp42F));_tmp42F->tag=1,({struct Cyc_Absyn_Exp*_tmp8BE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp42F->f1=_tmp8BE;}),({void*_tmp8BD=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp42F->f2=_tmp8BD;}),_tmp42F->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp42F;});_tmp430->hd=_tmp8BF;}),({struct Cyc_List_List*_tmp8BC=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp430->tl=_tmp8BC;});_tmp430;});_tmp6AA.tms=_tmp8C0;});_tmp6AA;}));
goto _LL0;case 181:  {
# 1731 "parse.y"
struct _tuple28*_tmp431=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp16=_tmp431;struct _tuple28*_tmp432=_stmttmp16;void*_tmp437;void*_tmp436;void*_tmp435;int _tmp434;void*_tmp433;_tmp433=_tmp432->f1;_tmp434=_tmp432->f2;_tmp435=_tmp432->f3;_tmp436=_tmp432->f4;_tmp437=_tmp432->f5;{struct Cyc_List_List*lis=_tmp433;int b=_tmp434;struct Cyc_Absyn_VarargInfo*c=_tmp435;void*eff=_tmp436;struct Cyc_List_List*po=_tmp437;
struct Cyc_Absyn_Exp*_tmp438=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;struct Cyc_Absyn_Exp*req=_tmp438;
struct Cyc_Absyn_Exp*_tmp439=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;struct Cyc_Absyn_Exp*ens=_tmp439;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6AB;({struct _tuple0*_tmp8C8=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6AB.id=_tmp8C8;}),({unsigned _tmp8C7=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6AB.varloc=_tmp8C7;}),({struct Cyc_List_List*_tmp8C6=({struct Cyc_List_List*_tmp43C=_region_malloc(yyr,sizeof(*_tmp43C));({void*_tmp8C5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp43B=_region_malloc(yyr,sizeof(*_tmp43B));_tmp43B->tag=3,({void*_tmp8C4=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp43A=_region_malloc(yyr,sizeof(*_tmp43A));_tmp43A->tag=1,_tmp43A->f1=lis,_tmp43A->f2=b,_tmp43A->f3=c,_tmp43A->f4=eff,_tmp43A->f5=po,_tmp43A->f6=req,_tmp43A->f7=ens;_tmp43A;});_tmp43B->f1=_tmp8C4;});_tmp43B;});_tmp43C->hd=_tmp8C5;}),({struct Cyc_List_List*_tmp8C3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp43C->tl=_tmp8C3;});_tmp43C;});_tmp6AB.tms=_tmp8C6;});_tmp6AB;}));
# 1736
goto _LL0;}}case 182:
# 1737 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6AC;({struct _tuple0*_tmp8CF=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6AC.id=_tmp8CF;}),({unsigned _tmp8CE=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6AC.varloc=_tmp8CE;}),({struct Cyc_List_List*_tmp8CD=({struct Cyc_List_List*_tmp43F=_region_malloc(yyr,sizeof(*_tmp43F));({void*_tmp8CC=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp43E=_region_malloc(yyr,sizeof(*_tmp43E));_tmp43E->tag=3,({void*_tmp8CB=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp43D=_region_malloc(yyr,sizeof(*_tmp43D));_tmp43D->tag=0,({struct Cyc_List_List*_tmp8CA=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp43D->f1=_tmp8CA;}),_tmp43D->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp43D;});_tmp43E->f1=_tmp8CB;});_tmp43E;});_tmp43F->hd=_tmp8CC;}),({struct Cyc_List_List*_tmp8C9=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp43F->tl=_tmp8C9;});_tmp43F;});_tmp6AC.tms=_tmp8CD;});_tmp6AC;}));
goto _LL0;case 183:  {
# 1740
struct Cyc_List_List*_tmp440=({unsigned _tmp8D0=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8D0,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp440;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6AD;({struct _tuple0*_tmp8D5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6AD.id=_tmp8D5;}),({unsigned _tmp8D4=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6AD.varloc=_tmp8D4;}),({struct Cyc_List_List*_tmp8D3=({struct Cyc_List_List*_tmp442=_region_malloc(yyr,sizeof(*_tmp442));({void*_tmp8D2=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp441=_region_malloc(yyr,sizeof(*_tmp441));_tmp441->tag=4,_tmp441->f1=ts,_tmp441->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp441->f3=0;_tmp441;});_tmp442->hd=_tmp8D2;}),({struct Cyc_List_List*_tmp8D1=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp442->tl=_tmp8D1;});_tmp442;});_tmp6AD.tms=_tmp8D3;});_tmp6AD;}));
# 1743
goto _LL0;}case 184:
# 1744 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6AE;({struct _tuple0*_tmp8DB=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6AE.id=_tmp8DB;}),({unsigned _tmp8DA=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6AE.varloc=_tmp8DA;}),({struct Cyc_List_List*_tmp8D9=({struct Cyc_List_List*_tmp444=_region_malloc(yyr,sizeof(*_tmp444));({void*_tmp8D8=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp443=_region_malloc(yyr,sizeof(*_tmp443));_tmp443->tag=5,_tmp443->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8D7=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp443->f2=_tmp8D7;});_tmp443;});_tmp444->hd=_tmp8D8;}),({
struct Cyc_List_List*_tmp8D6=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp444->tl=_tmp8D6;});_tmp444;});
# 1744
_tmp6AE.tms=_tmp8D9;});_tmp6AE;}));
# 1747
goto _LL0;case 185:
# 1751 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6AF;({struct _tuple0*_tmp8DC=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6AF.id=_tmp8DC;}),_tmp6AF.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6AF.tms=0;_tmp6AF;}));
goto _LL0;case 186:
# 1752 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B0;({struct _tuple0*_tmp8DD=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6B0.id=_tmp8DD;}),_tmp6B0.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6B0.tms=0;_tmp6B0;}));
goto _LL0;case 187:
# 1753 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 188:  {
# 1757 "parse.y"
struct Cyc_Parse_Declarator _tmp445=Cyc_yyget_YY28(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp445;
({struct Cyc_List_List*_tmp8E0=({struct Cyc_List_List*_tmp447=_region_malloc(yyr,sizeof(*_tmp447));({void*_tmp8DF=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp446=_region_malloc(yyr,sizeof(*_tmp446));_tmp446->tag=5,_tmp446->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8DE=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp446->f2=_tmp8DE;});_tmp446;});_tmp447->hd=_tmp8DF;}),_tmp447->tl=d.tms;_tmp447;});d.tms=_tmp8E0;});
yyval=(yyyvsp[2]).v;
# 1761
goto _LL0;}case 189:  {
# 1762 "parse.y"
struct Cyc_Parse_Declarator _tmp448=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp448;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B1;_tmp6B1.id=one.id,_tmp6B1.varloc=one.varloc,({
struct Cyc_List_List*_tmp8E3=({struct Cyc_List_List*_tmp44A=_region_malloc(yyr,sizeof(*_tmp44A));({void*_tmp8E2=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp449=_region_malloc(yyr,sizeof(*_tmp449));_tmp449->tag=0,({void*_tmp8E1=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp449->f1=_tmp8E1;}),_tmp449->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp449;});_tmp44A->hd=_tmp8E2;}),_tmp44A->tl=one.tms;_tmp44A;});_tmp6B1.tms=_tmp8E3;});_tmp6B1;}));
goto _LL0;}case 190:  {
# 1766 "parse.y"
struct Cyc_Parse_Declarator _tmp44B=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp44B;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B2;_tmp6B2.id=one.id,_tmp6B2.varloc=one.varloc,({
struct Cyc_List_List*_tmp8E7=({struct Cyc_List_List*_tmp44D=_region_malloc(yyr,sizeof(*_tmp44D));({void*_tmp8E6=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp44C=_region_malloc(yyr,sizeof(*_tmp44C));_tmp44C->tag=1,({struct Cyc_Absyn_Exp*_tmp8E5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp44C->f1=_tmp8E5;}),({void*_tmp8E4=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp44C->f2=_tmp8E4;}),_tmp44C->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp44C;});_tmp44D->hd=_tmp8E6;}),_tmp44D->tl=one.tms;_tmp44D;});_tmp6B2.tms=_tmp8E7;});_tmp6B2;}));
# 1770
goto _LL0;}case 191:  {
# 1771 "parse.y"
struct _tuple28*_tmp44E=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp17=_tmp44E;struct _tuple28*_tmp44F=_stmttmp17;void*_tmp454;void*_tmp453;void*_tmp452;int _tmp451;void*_tmp450;_tmp450=_tmp44F->f1;_tmp451=_tmp44F->f2;_tmp452=_tmp44F->f3;_tmp453=_tmp44F->f4;_tmp454=_tmp44F->f5;{struct Cyc_List_List*lis=_tmp450;int b=_tmp451;struct Cyc_Absyn_VarargInfo*c=_tmp452;void*eff=_tmp453;struct Cyc_List_List*po=_tmp454;
struct Cyc_Absyn_Exp*_tmp455=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;struct Cyc_Absyn_Exp*req=_tmp455;
struct Cyc_Absyn_Exp*_tmp456=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;struct Cyc_Absyn_Exp*ens=_tmp456;
struct Cyc_Parse_Declarator _tmp457=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp457;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B3;_tmp6B3.id=one.id,_tmp6B3.varloc=one.varloc,({struct Cyc_List_List*_tmp8EA=({struct Cyc_List_List*_tmp45A=_region_malloc(yyr,sizeof(*_tmp45A));({void*_tmp8E9=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp459=_region_malloc(yyr,sizeof(*_tmp459));_tmp459->tag=3,({void*_tmp8E8=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp458=_region_malloc(yyr,sizeof(*_tmp458));_tmp458->tag=1,_tmp458->f1=lis,_tmp458->f2=b,_tmp458->f3=c,_tmp458->f4=eff,_tmp458->f5=po,_tmp458->f6=req,_tmp458->f7=ens;_tmp458;});_tmp459->f1=_tmp8E8;});_tmp459;});_tmp45A->hd=_tmp8E9;}),_tmp45A->tl=one.tms;_tmp45A;});_tmp6B3.tms=_tmp8EA;});_tmp6B3;}));
# 1777
goto _LL0;}}case 192:  {
# 1778 "parse.y"
struct Cyc_Parse_Declarator _tmp45B=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp45B;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B4;_tmp6B4.id=one.id,_tmp6B4.varloc=one.varloc,({struct Cyc_List_List*_tmp8EE=({struct Cyc_List_List*_tmp45E=_region_malloc(yyr,sizeof(*_tmp45E));({void*_tmp8ED=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp45D=_region_malloc(yyr,sizeof(*_tmp45D));_tmp45D->tag=3,({void*_tmp8EC=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp45C=_region_malloc(yyr,sizeof(*_tmp45C));_tmp45C->tag=0,({struct Cyc_List_List*_tmp8EB=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp45C->f1=_tmp8EB;}),_tmp45C->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp45C;});_tmp45D->f1=_tmp8EC;});_tmp45D;});_tmp45E->hd=_tmp8ED;}),_tmp45E->tl=one.tms;_tmp45E;});_tmp6B4.tms=_tmp8EE;});_tmp6B4;}));
goto _LL0;}case 193:  {
# 1782
struct Cyc_List_List*_tmp45F=({unsigned _tmp8EF=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8EF,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp45F;
struct Cyc_Parse_Declarator _tmp460=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp460;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B5;_tmp6B5.id=one.id,_tmp6B5.varloc=one.varloc,({struct Cyc_List_List*_tmp8F1=({struct Cyc_List_List*_tmp462=_region_malloc(yyr,sizeof(*_tmp462));({void*_tmp8F0=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp461=_region_malloc(yyr,sizeof(*_tmp461));_tmp461->tag=4,_tmp461->f1=ts,_tmp461->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp461->f3=0;_tmp461;});_tmp462->hd=_tmp8F0;}),_tmp462->tl=one.tms;_tmp462;});_tmp6B5.tms=_tmp8F1;});_tmp6B5;}));
goto _LL0;}case 194:  {
# 1786 "parse.y"
struct Cyc_Parse_Declarator _tmp463=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp463;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6B6;_tmp6B6.id=one.id,_tmp6B6.varloc=one.varloc,({struct Cyc_List_List*_tmp8F4=({struct Cyc_List_List*_tmp465=_region_malloc(yyr,sizeof(*_tmp465));({void*_tmp8F3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp464=_region_malloc(yyr,sizeof(*_tmp464));_tmp464->tag=5,_tmp464->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8F2=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp464->f2=_tmp8F2;});_tmp464;});_tmp465->hd=_tmp8F3;}),_tmp465->tl=one.tms;_tmp465;});_tmp6B6.tms=_tmp8F4;});_tmp6B6;}));
goto _LL0;}case 195:
# 1792 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 196:
# 1793 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_tmp8F5=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp8F5,Cyc_yyget_YY27(&(yyyvsp[1]).v));}));
goto _LL0;case 197:  {
# 1797 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp467=_region_malloc(yyr,sizeof(*_tmp467));({void*_tmp8F7=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp466=_region_malloc(yyr,sizeof(*_tmp466));_tmp466->tag=5,_tmp466->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp8F6=Cyc_yyget_YY46(&(yyyvsp[3]).v);_tmp466->f2=_tmp8F6;});_tmp466;});_tmp467->hd=_tmp8F7;}),_tmp467->tl=ans;_tmp467;});{
# 1801
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp468=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp18=_tmp468;struct _tuple22 _tmp469=_stmttmp18;void*_tmp46C;void*_tmp46B;unsigned _tmp46A;_tmp46A=_tmp469.f1;_tmp46B=_tmp469.f2;_tmp46C=_tmp469.f3;{unsigned ploc=_tmp46A;void*nullable=_tmp46B;void*bound=_tmp46C;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->ptr_loc=ploc,_tmp46D->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp46D->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp46D;});{
struct _tuple15 _tmp46E=({unsigned _tmp8FB=ploc;void*_tmp8FA=nullable;void*_tmp8F9=bound;void*_tmp8F8=Cyc_yyget_YY45(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp8FB,_tmp8FA,_tmp8F9,_tmp8F8,Cyc_yyget_YY57(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp19=_tmp46E;struct _tuple15 _tmp46F=_stmttmp19;void*_tmp474;void*_tmp473;void*_tmp472;void*_tmp471;void*_tmp470;_tmp470=_tmp46F.f1;_tmp471=_tmp46F.f2;_tmp472=_tmp46F.f3;_tmp473=_tmp46F.f4;_tmp474=_tmp46F.f5;{void*nullable=_tmp470;void*bound=_tmp471;void*zeroterm=_tmp472;void*rgn_opt=_tmp473;void*released=_tmp474;
ans=({struct Cyc_List_List*_tmp476=_region_malloc(yyr,sizeof(*_tmp476));({void*_tmp8FD=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp475=_region_malloc(yyr,sizeof(*_tmp475));_tmp475->tag=2,(_tmp475->f1).rgn=rgn_opt,(_tmp475->f1).nullable=nullable,(_tmp475->f1).bounds=bound,(_tmp475->f1).zero_term=zeroterm,(_tmp475->f1).ptrloc=ptrloc,(_tmp475->f1).released=released,({struct Cyc_Absyn_Tqual _tmp8FC=Cyc_yyget_YY24(&(yyyvsp[4]).v);_tmp475->f2=_tmp8FC;});_tmp475;});_tmp476->hd=_tmp8FD;}),_tmp476->tl=ans;_tmp476;});
yyval=Cyc_YY27(ans);
# 1809
goto _LL0;}}}}}case 198:
# 1811
 yyval=Cyc_YY57(0);
goto _LL0;case 199:
# 1812 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_tmp477=_region_malloc(yyr,sizeof(*_tmp477));({void*_tmp8FF=Cyc_yyget_YY56(&(yyyvsp[0]).v);_tmp477->hd=_tmp8FF;}),({struct Cyc_List_List*_tmp8FE=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp477->tl=_tmp8FE;});_tmp477;}));
goto _LL0;case 200:
# 1817 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp478=_region_malloc(yyr,sizeof(*_tmp478));_tmp478->tag=0,({struct Cyc_Absyn_Exp*_tmp900=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp478->f1=_tmp900;});_tmp478;}));
goto _LL0;case 201:
# 1818 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp479=_region_malloc(yyr,sizeof(*_tmp479));_tmp479->tag=1,({void*_tmp901=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp479->f1=_tmp901;});_tmp479;}));
goto _LL0;case 202:
# 1819 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp47A=_region_malloc(yyr,sizeof(*_tmp47A));_tmp47A->tag=2;_tmp47A;}));
goto _LL0;case 203:
# 1820 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp47B=_region_malloc(yyr,sizeof(*_tmp47B));_tmp47B->tag=3;_tmp47B;}));
goto _LL0;case 204:
# 1821 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_tmp47C=_region_malloc(yyr,sizeof(*_tmp47C));_tmp47C->tag=6;_tmp47C;}));
goto _LL0;case 205:
# 1822 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp47D=_region_malloc(yyr,sizeof(*_tmp47D));_tmp47D->tag=4;_tmp47D;}));
goto _LL0;case 206:
# 1823 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp47E=_region_malloc(yyr,sizeof(*_tmp47E));_tmp47E->tag=5;_tmp47E;}));
goto _LL0;case 207:
# 1824 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp47F=_region_malloc(yyr,sizeof(*_tmp47F));_tmp47F->tag=7;_tmp47F;}));
goto _LL0;case 208:
# 1825 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp480=_region_malloc(yyr,sizeof(*_tmp480));_tmp480->tag=8;_tmp480;}));
goto _LL0;case 209:
# 1831 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp481->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp481->f3=Cyc_Absyn_fat_bound_type:({void*_tmp902=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp481->f3=_tmp902;});_tmp481;}));
goto _LL0;case 210:
# 1832 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp482->f2=Cyc_Absyn_false_type,({void*_tmp903=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp482->f3=_tmp903;});_tmp482;}));
goto _LL0;case 211:
# 1833 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp483->f2=Cyc_Absyn_true_type,_tmp483->f3=Cyc_Absyn_fat_bound_type;_tmp483;}));
goto _LL0;case 212:
# 1836
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 213:
# 1837 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 214:
# 1840
 yyval=Cyc_YY52(Cyc_Tcutil_any_bool(0));
goto _LL0;case 215:
# 1841 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_true_type);
goto _LL0;case 216:
# 1842 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_false_type);
goto _LL0;case 217:
# 1847 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 218:
# 1848 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(&(yyyvsp[0]).v),& Cyc_Kinds_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 219:
# 1849 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 220:
# 1853 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 221:
# 1854 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp904=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp904,Cyc_yyget_YY24(&(yyyvsp[1]).v));}));
goto _LL0;case 222:
# 1858 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->f1=0,_tmp484->f2=0,_tmp484->f3=0,({void*_tmp906=Cyc_yyget_YY50(&(yyyvsp[0]).v);_tmp484->f4=_tmp906;}),({struct Cyc_List_List*_tmp905=Cyc_yyget_YY51(&(yyyvsp[1]).v);_tmp484->f5=_tmp905;});_tmp484;}));
goto _LL0;case 223:
# 1860 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp485=_cycalloc(sizeof(*_tmp485));({struct Cyc_List_List*_tmp909=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[0]).v));_tmp485->f1=_tmp909;}),_tmp485->f2=0,_tmp485->f3=0,({void*_tmp908=Cyc_yyget_YY50(&(yyyvsp[1]).v);_tmp485->f4=_tmp908;}),({struct Cyc_List_List*_tmp907=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp485->f5=_tmp907;});_tmp485;}));
goto _LL0;case 224:
# 1862 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp486=_cycalloc(sizeof(*_tmp486));({struct Cyc_List_List*_tmp90C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[0]).v));_tmp486->f1=_tmp90C;}),_tmp486->f2=1,_tmp486->f3=0,({void*_tmp90B=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp486->f4=_tmp90B;}),({struct Cyc_List_List*_tmp90A=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp486->f5=_tmp90A;});_tmp486;}));
goto _LL0;case 225:  {
# 1865
struct _tuple8*_tmp487=Cyc_yyget_YY38(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1A=_tmp487;struct _tuple8*_tmp488=_stmttmp1A;void*_tmp48B;struct Cyc_Absyn_Tqual _tmp48A;void*_tmp489;_tmp489=_tmp488->f1;_tmp48A=_tmp488->f2;_tmp48B=_tmp488->f3;{struct _fat_ptr*n=_tmp489;struct Cyc_Absyn_Tqual tq=_tmp48A;void*t=_tmp48B;
struct Cyc_Absyn_VarargInfo*_tmp48C=({struct Cyc_Absyn_VarargInfo*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->name=n,_tmp48E->tq=tq,_tmp48E->type=t,({int _tmp90D=Cyc_yyget_YY32(&(yyyvsp[1]).v);_tmp48E->inject=_tmp90D;});_tmp48E;});struct Cyc_Absyn_VarargInfo*v=_tmp48C;
yyval=Cyc_YY40(({struct _tuple28*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->f1=0,_tmp48D->f2=0,_tmp48D->f3=v,({void*_tmp90F=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp48D->f4=_tmp90F;}),({struct Cyc_List_List*_tmp90E=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp48D->f5=_tmp90E;});_tmp48D;}));
# 1869
goto _LL0;}}case 226:  {
# 1871
struct _tuple8*_tmp48F=Cyc_yyget_YY38(&(yyyvsp[4]).v);struct _tuple8*_stmttmp1B=_tmp48F;struct _tuple8*_tmp490=_stmttmp1B;void*_tmp493;struct Cyc_Absyn_Tqual _tmp492;void*_tmp491;_tmp491=_tmp490->f1;_tmp492=_tmp490->f2;_tmp493=_tmp490->f3;{struct _fat_ptr*n=_tmp491;struct Cyc_Absyn_Tqual tq=_tmp492;void*t=_tmp493;
struct Cyc_Absyn_VarargInfo*_tmp494=({struct Cyc_Absyn_VarargInfo*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->name=n,_tmp496->tq=tq,_tmp496->type=t,({int _tmp910=Cyc_yyget_YY32(&(yyyvsp[3]).v);_tmp496->inject=_tmp910;});_tmp496;});struct Cyc_Absyn_VarargInfo*v=_tmp494;
yyval=Cyc_YY40(({struct _tuple28*_tmp495=_cycalloc(sizeof(*_tmp495));({struct Cyc_List_List*_tmp913=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[0]).v));_tmp495->f1=_tmp913;}),_tmp495->f2=0,_tmp495->f3=v,({void*_tmp912=Cyc_yyget_YY50(&(yyyvsp[5]).v);_tmp495->f4=_tmp912;}),({struct Cyc_List_List*_tmp911=Cyc_yyget_YY51(&(yyyvsp[6]).v);_tmp495->f5=_tmp911;});_tmp495;}));
# 1875
goto _LL0;}}case 227:
# 1879 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _tmp914=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp914,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->tag=1,_tmp497->f1=0;_tmp497;}));}));
goto _LL0;case 228:
# 1880 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _tmp915=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp915,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(&(yyyvsp[2]).v)));}));
goto _LL0;case 229:
# 1883
 yyval=Cyc_YY50(0);
goto _LL0;case 230:
# 1884 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 231:
# 1888 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 232:
# 1889 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 233:  {
# 1897 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp498=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C->tag=2,_tmp49C->f1=0,_tmp49C->f2=& Cyc_Kinds_trk;_tmp49C;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp498;
void*_tmp499=({struct _fat_ptr _tmp916=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp916,(void*)kb);});void*t=_tmp499;
yyval=Cyc_YY51(({struct Cyc_List_List*_tmp49B=_cycalloc(sizeof(*_tmp49B));({struct _tuple34*_tmp918=({struct _tuple34*_tmp49A=_cycalloc(sizeof(*_tmp49A));({void*_tmp917=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_tmp49A->f1=_tmp917;}),_tmp49A->f2=t;_tmp49A;});_tmp49B->hd=_tmp918;}),_tmp49B->tl=0;_tmp49B;}));
# 1901
goto _LL0;}case 234:  {
# 1903 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp49D=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->tag=2,_tmp4A1->f1=0,_tmp4A1->f2=& Cyc_Kinds_trk;_tmp4A1;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp49D;
void*_tmp49E=({struct _fat_ptr _tmp919=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp919,(void*)kb);});void*t=_tmp49E;
yyval=Cyc_YY51(({struct Cyc_List_List*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));({struct _tuple34*_tmp91C=({struct _tuple34*_tmp49F=_cycalloc(sizeof(*_tmp49F));({void*_tmp91B=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_tmp49F->f1=_tmp91B;}),_tmp49F->f2=t;_tmp49F;});_tmp4A0->hd=_tmp91C;}),({struct Cyc_List_List*_tmp91A=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4A0->tl=_tmp91A;});_tmp4A0;}));
# 1907
goto _LL0;}case 235:
# 1911 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 236:
# 1913 "parse.y"
 if(({struct _fat_ptr _tmp91D=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp91D,({const char*_tmp4A2="inject";_tag_fat(_tmp4A2,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B7;_tmp6B7.tag=0,({struct _fat_ptr _tmp91E=({const char*_tmp4A5="missing type in function declaration";_tag_fat(_tmp4A5,sizeof(char),37U);});_tmp6B7.f1=_tmp91E;});_tmp6B7;});void*_tmp4A3[1];_tmp4A3[0]=& _tmp4A4;({unsigned _tmp91F=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmp91F,_tag_fat(_tmp4A3,sizeof(void*),1));});});
yyval=Cyc_YY32(1);
# 1917
goto _LL0;case 237:
# 1920 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 238:
# 1921 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp920=Cyc_yyget_YY41(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp920,Cyc_yyget_YY41(&(yyyvsp[2]).v));}));
goto _LL0;case 239:
# 1925 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 240:
# 1926 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 241:
# 1927 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));({void*_tmp921=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(&(yyyvsp[2]).v));_tmp4A6->hd=_tmp921;}),_tmp4A6->tl=0;_tmp4A6;}));
goto _LL0;case 242:
# 1929 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));({void*_tmp922=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp4A7->hd=_tmp922;}),_tmp4A7->tl=0;_tmp4A7;}));
# 1932
goto _LL0;case 243:
# 1937 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));({void*_tmp924=Cyc_Absyn_access_eff(({struct _tuple8*_tmp923=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp923,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp4A8->hd=_tmp924;}),_tmp4A8->tl=0;_tmp4A8;}));
goto _LL0;case 244:
# 1939 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));({void*_tmp927=Cyc_Absyn_access_eff(({struct _tuple8*_tmp926=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp926,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp4A9->hd=_tmp927;}),({struct Cyc_List_List*_tmp925=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp4A9->tl=_tmp925;});_tmp4A9;}));
goto _LL0;case 245:
# 1944 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));({struct _tuple8*_tmp928=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp4AA->hd=_tmp928;}),_tmp4AA->tl=0;_tmp4AA;}));
goto _LL0;case 246:
# 1945 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct _tuple8*_tmp92A=Cyc_yyget_YY38(&(yyyvsp[2]).v);_tmp4AB->hd=_tmp92A;}),({struct Cyc_List_List*_tmp929=Cyc_yyget_YY39(&(yyyvsp[0]).v);_tmp4AB->tl=_tmp929;});_tmp4AB;}));
goto _LL0;case 247:  {
# 1951 "parse.y"
struct _tuple27 _tmp4AC=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp1C=_tmp4AC;struct _tuple27 _tmp4AD=_stmttmp1C;void*_tmp4B0;struct Cyc_Parse_Type_specifier _tmp4AF;struct Cyc_Absyn_Tqual _tmp4AE;_tmp4AE=_tmp4AD.f1;_tmp4AF=_tmp4AD.f2;_tmp4B0=_tmp4AD.f3;{struct Cyc_Absyn_Tqual tq=_tmp4AE;struct Cyc_Parse_Type_specifier tspecs=_tmp4AF;struct Cyc_List_List*atts=_tmp4B0;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp4B1=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp1D=_tmp4B1;struct Cyc_Parse_Declarator _tmp4B2=_stmttmp1D;void*_tmp4B5;unsigned _tmp4B4;void*_tmp4B3;_tmp4B3=_tmp4B2.id;_tmp4B4=_tmp4B2.varloc;_tmp4B5=_tmp4B2.tms;{struct _tuple0*qv=_tmp4B3;unsigned varloc=_tmp4B4;struct Cyc_List_List*tms=_tmp4B5;
void*_tmp4B6=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4B6;
struct _tuple14 _tmp4B7=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp1E=_tmp4B7;struct _tuple14 _tmp4B8=_stmttmp1E;void*_tmp4BC;void*_tmp4BB;void*_tmp4BA;struct Cyc_Absyn_Tqual _tmp4B9;_tmp4B9=_tmp4B8.f1;_tmp4BA=_tmp4B8.f2;_tmp4BB=_tmp4B8.f3;_tmp4BC=_tmp4B8.f4;{struct Cyc_Absyn_Tqual tq2=_tmp4B9;void*t2=_tmp4BA;struct Cyc_List_List*tvs=_tmp4BB;struct Cyc_List_List*atts2=_tmp4BC;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B8;_tmp6B8.tag=0,({struct _fat_ptr _tmp92B=({const char*_tmp4BF="parameter with bad type params";_tag_fat(_tmp4BF,sizeof(char),31U);});_tmp6B8.f1=_tmp92B;});_tmp6B8;});void*_tmp4BD[1];_tmp4BD[0]=& _tmp4BE;({unsigned _tmp92C=(unsigned)((yyyvsp[1]).l).first_line;Cyc_Warn_err2(_tmp92C,_tag_fat(_tmp4BD,sizeof(void*),1));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B9;_tmp6B9.tag=0,({struct _fat_ptr _tmp92D=({const char*_tmp4C2="parameter cannot be qualified with a namespace";_tag_fat(_tmp4C2,sizeof(char),47U);});_tmp6B9.f1=_tmp92D;});_tmp6B9;});void*_tmp4C0[1];_tmp4C0[0]=& _tmp4C1;({unsigned _tmp92E=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmp92E,_tag_fat(_tmp4C0,sizeof(void*),1));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BA;_tmp6BA.tag=0,({struct _fat_ptr _tmp92F=({const char*_tmp4C5="extra attributes on parameter, ignoring";_tag_fat(_tmp4C5,sizeof(char),40U);});_tmp6BA.f1=_tmp92F;});_tmp6BA;});void*_tmp4C3[1];_tmp4C3[0]=& _tmp4C4;({unsigned _tmp930=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp930,_tag_fat(_tmp4C3,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6->f1=idopt,_tmp4C6->f2=tq2,_tmp4C6->f3=t2;_tmp4C6;}));
# 1965
goto _LL0;}}}}}}case 248:  {
# 1966 "parse.y"
struct _tuple27 _tmp4C7=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp1F=_tmp4C7;struct _tuple27 _tmp4C8=_stmttmp1F;void*_tmp4CB;struct Cyc_Parse_Type_specifier _tmp4CA;struct Cyc_Absyn_Tqual _tmp4C9;_tmp4C9=_tmp4C8.f1;_tmp4CA=_tmp4C8.f2;_tmp4CB=_tmp4C8.f3;{struct Cyc_Absyn_Tqual tq=_tmp4C9;struct Cyc_Parse_Type_specifier tspecs=_tmp4CA;struct Cyc_List_List*atts=_tmp4CB;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp4CC=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4CC;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BB;_tmp6BB.tag=0,({struct _fat_ptr _tmp931=({const char*_tmp4CF="bad attributes on parameter, ignoring";_tag_fat(_tmp4CF,sizeof(char),38U);});_tmp6BB.f1=_tmp931;});_tmp6BB;});void*_tmp4CD[1];_tmp4CD[0]=& _tmp4CE;({unsigned _tmp932=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp932,_tag_fat(_tmp4CD,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));_tmp4D0->f1=0,_tmp4D0->f2=tq,_tmp4D0->f3=t;_tmp4D0;}));
# 1973
goto _LL0;}}}case 249:  {
# 1974 "parse.y"
struct _tuple27 _tmp4D1=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp20=_tmp4D1;struct _tuple27 _tmp4D2=_stmttmp20;void*_tmp4D5;struct Cyc_Parse_Type_specifier _tmp4D4;struct Cyc_Absyn_Tqual _tmp4D3;_tmp4D3=_tmp4D2.f1;_tmp4D4=_tmp4D2.f2;_tmp4D5=_tmp4D2.f3;{struct Cyc_Absyn_Tqual tq=_tmp4D3;struct Cyc_Parse_Type_specifier tspecs=_tmp4D4;struct Cyc_List_List*atts=_tmp4D5;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp4D6=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4D6;
struct Cyc_List_List*_tmp4D7=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp4D7;
struct _tuple14 _tmp4D8=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp21=_tmp4D8;struct _tuple14 _tmp4D9=_stmttmp21;void*_tmp4DD;void*_tmp4DC;void*_tmp4DB;struct Cyc_Absyn_Tqual _tmp4DA;_tmp4DA=_tmp4D9.f1;_tmp4DB=_tmp4D9.f2;_tmp4DC=_tmp4D9.f3;_tmp4DD=_tmp4D9.f4;{struct Cyc_Absyn_Tqual tq2=_tmp4DA;void*t2=_tmp4DB;struct Cyc_List_List*tvs=_tmp4DC;struct Cyc_List_List*atts2=_tmp4DD;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BC;_tmp6BC.tag=0,({
struct _fat_ptr _tmp933=({const char*_tmp4E0="bad type parameters on formal argument, ignoring";_tag_fat(_tmp4E0,sizeof(char),49U);});_tmp6BC.f1=_tmp933;});_tmp6BC;});void*_tmp4DE[1];_tmp4DE[0]=& _tmp4DF;({unsigned _tmp934=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp934,_tag_fat(_tmp4DE,sizeof(void*),1));});});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6BD;_tmp6BD.tag=0,({struct _fat_ptr _tmp935=({const char*_tmp4E3="bad attributes on parameter, ignoring";_tag_fat(_tmp4E3,sizeof(char),38U);});_tmp6BD.f1=_tmp935;});_tmp6BD;});void*_tmp4E1[1];_tmp4E1[0]=& _tmp4E2;({unsigned _tmp936=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp936,_tag_fat(_tmp4E1,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4->f1=0,_tmp4E4->f2=tq2,_tmp4E4->f3=t2;_tmp4E4;}));
# 1986
goto _LL0;}}}}case 250:
# 1989 "parse.y"
 yyval=Cyc_YY37(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY37(&(yyyvsp[0]).v)));
goto _LL0;case 251:
# 1992
 yyval=Cyc_YY37(({struct Cyc_List_List*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));({struct _fat_ptr*_tmp938=({struct _fat_ptr*_tmp4E5=_cycalloc(sizeof(*_tmp4E5));({struct _fat_ptr _tmp937=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp4E5=_tmp937;});_tmp4E5;});_tmp4E6->hd=_tmp938;}),_tmp4E6->tl=0;_tmp4E6;}));
goto _LL0;case 252:
# 1993 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_tmp4E8=_cycalloc(sizeof(*_tmp4E8));({struct _fat_ptr*_tmp93B=({struct _fat_ptr*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));({struct _fat_ptr _tmp93A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp4E7=_tmp93A;});_tmp4E7;});_tmp4E8->hd=_tmp93B;}),({struct Cyc_List_List*_tmp939=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp4E8->tl=_tmp939;});_tmp4E8;}));
goto _LL0;case 253:
# 1997 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 254:
# 1998 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 255:
# 2003 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp93C=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->tag=36,_tmp4E9->f1=0,_tmp4E9->f2=0;_tmp4E9;});Cyc_Absyn_new_exp(_tmp93C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 256:
# 2005 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp93E=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA->tag=36,_tmp4EA->f1=0,({struct Cyc_List_List*_tmp93D=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp4EA->f2=_tmp93D;});_tmp4EA;});Cyc_Absyn_new_exp(_tmp93E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 257:
# 2007 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp940=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp4EB=_cycalloc(sizeof(*_tmp4EB));_tmp4EB->tag=36,_tmp4EB->f1=0,({struct Cyc_List_List*_tmp93F=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp4EB->f2=_tmp93F;});_tmp4EB;});Cyc_Absyn_new_exp(_tmp940,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 258:  {
# 2009 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp4EC=({unsigned _tmp945=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp944=({struct _tuple0*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp942=({struct _fat_ptr*_tmp4EE=_cycalloc(sizeof(*_tmp4EE));({struct _fat_ptr _tmp941=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp4EE=_tmp941;});_tmp4EE;});_tmp4EF->f2=_tmp942;});_tmp4EF;});void*_tmp943=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmp945,_tmp944,_tmp943,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2009
struct Cyc_Absyn_Vardecl*vd=_tmp4EC;
# 2012
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmp948=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp4ED=_cycalloc(sizeof(*_tmp4ED));_tmp4ED->tag=27,_tmp4ED->f1=vd,({struct Cyc_Absyn_Exp*_tmp947=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp4ED->f2=_tmp947;}),({struct Cyc_Absyn_Exp*_tmp946=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp4ED->f3=_tmp946;}),_tmp4ED->f4=0;_tmp4ED;});Cyc_Absyn_new_exp(_tmp948,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2015
goto _LL0;}case 259:  {
# 2017 "parse.y"
void*_tmp4F0=({struct _tuple8*_tmp949=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmp949,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp4F0;
yyval=Cyc_Exp_tok(({void*_tmp94B=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));_tmp4F1->tag=28,({struct Cyc_Absyn_Exp*_tmp94A=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp4F1->f1=_tmp94A;}),_tmp4F1->f2=t,_tmp4F1->f3=0;_tmp4F1;});Cyc_Absyn_new_exp(_tmp94B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2020
goto _LL0;}case 260:
# 2025 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp4F3=_cycalloc(sizeof(*_tmp4F3));({struct _tuple35*_tmp94D=({struct _tuple35*_tmp4F2=_cycalloc(sizeof(*_tmp4F2));_tmp4F2->f1=0,({struct Cyc_Absyn_Exp*_tmp94C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp4F2->f2=_tmp94C;});_tmp4F2;});_tmp4F3->hd=_tmp94D;}),_tmp4F3->tl=0;_tmp4F3;}));
goto _LL0;case 261:
# 2027 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));({struct _tuple35*_tmp950=({struct _tuple35*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));({struct Cyc_List_List*_tmp94F=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp4F4->f1=_tmp94F;}),({struct Cyc_Absyn_Exp*_tmp94E=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp4F4->f2=_tmp94E;});_tmp4F4;});_tmp4F5->hd=_tmp950;}),_tmp4F5->tl=0;_tmp4F5;}));
goto _LL0;case 262:
# 2029 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));({struct _tuple35*_tmp953=({struct _tuple35*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->f1=0,({struct Cyc_Absyn_Exp*_tmp952=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp4F6->f2=_tmp952;});_tmp4F6;});_tmp4F7->hd=_tmp953;}),({struct Cyc_List_List*_tmp951=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp4F7->tl=_tmp951;});_tmp4F7;}));
goto _LL0;case 263:
# 2031 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));({struct _tuple35*_tmp957=({struct _tuple35*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));({struct Cyc_List_List*_tmp956=Cyc_yyget_YY42(&(yyyvsp[2]).v);_tmp4F8->f1=_tmp956;}),({struct Cyc_Absyn_Exp*_tmp955=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp4F8->f2=_tmp955;});_tmp4F8;});_tmp4F9->hd=_tmp957;}),({struct Cyc_List_List*_tmp954=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp4F9->tl=_tmp954;});_tmp4F9;}));
goto _LL0;case 264:
# 2035 "parse.y"
 yyval=Cyc_YY42(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY42(&(yyyvsp[0]).v)));
goto _LL0;case 265:
# 2036 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp4FC=_cycalloc(sizeof(*_tmp4FC));({void*_tmp95A=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));_tmp4FB->tag=1,({struct _fat_ptr*_tmp959=({struct _fat_ptr*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));({struct _fat_ptr _tmp958=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp4FA=_tmp958;});_tmp4FA;});_tmp4FB->f1=_tmp959;});_tmp4FB;});_tmp4FC->hd=_tmp95A;}),_tmp4FC->tl=0;_tmp4FC;}));
goto _LL0;case 266:
# 2041 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp4FD=_cycalloc(sizeof(*_tmp4FD));({void*_tmp95B=Cyc_yyget_YY43(&(yyyvsp[0]).v);_tmp4FD->hd=_tmp95B;}),_tmp4FD->tl=0;_tmp4FD;}));
goto _LL0;case 267:
# 2042 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp4FE=_cycalloc(sizeof(*_tmp4FE));({void*_tmp95D=Cyc_yyget_YY43(&(yyyvsp[1]).v);_tmp4FE->hd=_tmp95D;}),({struct Cyc_List_List*_tmp95C=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp4FE->tl=_tmp95C;});_tmp4FE;}));
goto _LL0;case 268:
# 2046 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->tag=0,({struct Cyc_Absyn_Exp*_tmp95E=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp4FF->f1=_tmp95E;});_tmp4FF;}));
goto _LL0;case 269:
# 2047 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->tag=1,({struct _fat_ptr*_tmp960=({struct _fat_ptr*_tmp500=_cycalloc(sizeof(*_tmp500));({struct _fat_ptr _tmp95F=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp500=_tmp95F;});_tmp500;});_tmp501->f1=_tmp960;});_tmp501;}));
goto _LL0;case 270:  {
# 2052 "parse.y"
struct _tuple27 _tmp502=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp22=_tmp502;struct _tuple27 _tmp503=_stmttmp22;void*_tmp506;struct Cyc_Parse_Type_specifier _tmp505;struct Cyc_Absyn_Tqual _tmp504;_tmp504=_tmp503.f1;_tmp505=_tmp503.f2;_tmp506=_tmp503.f3;{struct Cyc_Absyn_Tqual tq=_tmp504;struct Cyc_Parse_Type_specifier tss=_tmp505;struct Cyc_List_List*atts=_tmp506;
void*_tmp507=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp507;
if(atts != 0)
({void*_tmp508=0U;({unsigned _tmp962=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp961=({const char*_tmp509="ignoring attributes in type";_tag_fat(_tmp509,sizeof(char),28U);});Cyc_Warn_warn(_tmp962,_tmp961,_tag_fat(_tmp508,sizeof(void*),0));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp50A=_cycalloc(sizeof(*_tmp50A));_tmp50A->f1=0,_tmp50A->f2=tq,_tmp50A->f3=t;_tmp50A;}));
# 2058
goto _LL0;}}case 271:  {
# 2059 "parse.y"
struct _tuple27 _tmp50B=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp23=_tmp50B;struct _tuple27 _tmp50C=_stmttmp23;void*_tmp50F;struct Cyc_Parse_Type_specifier _tmp50E;struct Cyc_Absyn_Tqual _tmp50D;_tmp50D=_tmp50C.f1;_tmp50E=_tmp50C.f2;_tmp50F=_tmp50C.f3;{struct Cyc_Absyn_Tqual tq=_tmp50D;struct Cyc_Parse_Type_specifier tss=_tmp50E;struct Cyc_List_List*atts=_tmp50F;
void*_tmp510=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp510;
struct Cyc_List_List*_tmp511=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp511;
struct _tuple14 _tmp512=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp512;
if(t_info.f3 != 0)
({void*_tmp513=0U;({unsigned _tmp964=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp963=({const char*_tmp514="bad type params, ignoring";_tag_fat(_tmp514,sizeof(char),26U);});Cyc_Warn_warn(_tmp964,_tmp963,_tag_fat(_tmp513,sizeof(void*),0));});});
if(t_info.f4 != 0)
({void*_tmp515=0U;({unsigned _tmp966=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp965=({const char*_tmp516="bad specifiers, ignoring";_tag_fat(_tmp516,sizeof(char),25U);});Cyc_Warn_warn(_tmp966,_tmp965,_tag_fat(_tmp515,sizeof(void*),0));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->f1=0,_tmp517->f2=t_info.f1,_tmp517->f3=t_info.f2;_tmp517;}));
# 2069
goto _LL0;}}case 272:
# 2072 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_tmp967=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp967,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 273:
# 2073 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 274:
# 2074 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 275:
# 2075 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(&(yyyvsp[2]).v)));
goto _LL0;case 276:
# 2076 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp518=_cycalloc(sizeof(*_tmp518));({void*_tmp969=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp518->hd=_tmp969;}),({struct Cyc_List_List*_tmp968=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp518->tl=_tmp968;});_tmp518;})));
goto _LL0;case 277:
# 2082 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp519=_cycalloc(sizeof(*_tmp519));({void*_tmp96A=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp519->hd=_tmp96A;}),_tmp519->tl=0;_tmp519;}));
goto _LL0;case 278:
# 2083 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp51A=_cycalloc(sizeof(*_tmp51A));({void*_tmp96C=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp51A->hd=_tmp96C;}),({struct Cyc_List_List*_tmp96B=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp51A->tl=_tmp96B;});_tmp51A;}));
goto _LL0;case 279:
# 2087 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6BE;({struct Cyc_List_List*_tmp96D=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp6BE.tms=_tmp96D;});_tmp6BE;}));
goto _LL0;case 280:
# 2088 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 281:
# 2090 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6BF;({struct Cyc_List_List*_tmp96F=({struct Cyc_List_List*_tmp96E=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp96E,(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms);});_tmp6BF.tms=_tmp96F;});_tmp6BF;}));
goto _LL0;case 282:
# 2095 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 283:
# 2097 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C0;({struct Cyc_List_List*_tmp972=({struct Cyc_List_List*_tmp51C=_region_malloc(yyr,sizeof(*_tmp51C));({void*_tmp971=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp51B=_region_malloc(yyr,sizeof(*_tmp51B));_tmp51B->tag=0,({void*_tmp970=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp51B->f1=_tmp970;}),_tmp51B->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp51B;});_tmp51C->hd=_tmp971;}),_tmp51C->tl=0;_tmp51C;});_tmp6C0.tms=_tmp972;});_tmp6C0;}));
goto _LL0;case 284:
# 2099 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C1;({struct Cyc_List_List*_tmp976=({struct Cyc_List_List*_tmp51E=_region_malloc(yyr,sizeof(*_tmp51E));({void*_tmp975=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp51D=_region_malloc(yyr,sizeof(*_tmp51D));_tmp51D->tag=0,({void*_tmp974=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp51D->f1=_tmp974;}),_tmp51D->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp51D;});_tmp51E->hd=_tmp975;}),({struct Cyc_List_List*_tmp973=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp51E->tl=_tmp973;});_tmp51E;});_tmp6C1.tms=_tmp976;});_tmp6C1;}));
goto _LL0;case 285:
# 2101 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C2;({struct Cyc_List_List*_tmp97A=({struct Cyc_List_List*_tmp520=_region_malloc(yyr,sizeof(*_tmp520));({void*_tmp979=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp51F=_region_malloc(yyr,sizeof(*_tmp51F));_tmp51F->tag=1,({struct Cyc_Absyn_Exp*_tmp978=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp51F->f1=_tmp978;}),({void*_tmp977=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp51F->f2=_tmp977;}),_tmp51F->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp51F;});_tmp520->hd=_tmp979;}),_tmp520->tl=0;_tmp520;});_tmp6C2.tms=_tmp97A;});_tmp6C2;}));
goto _LL0;case 286:
# 2103 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C3;({struct Cyc_List_List*_tmp97F=({struct Cyc_List_List*_tmp522=_region_malloc(yyr,sizeof(*_tmp522));({void*_tmp97E=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp521=_region_malloc(yyr,sizeof(*_tmp521));_tmp521->tag=1,({struct Cyc_Absyn_Exp*_tmp97D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp521->f1=_tmp97D;}),({void*_tmp97C=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp521->f2=_tmp97C;}),_tmp521->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp521;});_tmp522->hd=_tmp97E;}),({
struct Cyc_List_List*_tmp97B=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp522->tl=_tmp97B;});_tmp522;});
# 2103
_tmp6C3.tms=_tmp97F;});_tmp6C3;}));
# 2106
goto _LL0;case 287:  {
# 2107 "parse.y"
struct _tuple28*_tmp523=Cyc_yyget_YY40(&(yyyvsp[1]).v);struct _tuple28*_stmttmp24=_tmp523;struct _tuple28*_tmp524=_stmttmp24;void*_tmp529;void*_tmp528;void*_tmp527;int _tmp526;void*_tmp525;_tmp525=_tmp524->f1;_tmp526=_tmp524->f2;_tmp527=_tmp524->f3;_tmp528=_tmp524->f4;_tmp529=_tmp524->f5;{struct Cyc_List_List*lis=_tmp525;int b=_tmp526;struct Cyc_Absyn_VarargInfo*c=_tmp527;void*eff=_tmp528;struct Cyc_List_List*po=_tmp529;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C4;({struct Cyc_List_List*_tmp984=({struct Cyc_List_List*_tmp52C=_region_malloc(yyr,sizeof(*_tmp52C));({void*_tmp983=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp52B=_region_malloc(yyr,sizeof(*_tmp52B));_tmp52B->tag=3,({void*_tmp982=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp52A=_region_malloc(yyr,sizeof(*_tmp52A));_tmp52A->tag=1,_tmp52A->f1=lis,_tmp52A->f2=b,_tmp52A->f3=c,_tmp52A->f4=eff,_tmp52A->f5=po,({struct Cyc_Absyn_Exp*_tmp981=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f1;_tmp52A->f6=_tmp981;}),({struct Cyc_Absyn_Exp*_tmp980=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f2;_tmp52A->f7=_tmp980;});_tmp52A;});_tmp52B->f1=_tmp982;});_tmp52B;});_tmp52C->hd=_tmp983;}),_tmp52C->tl=0;_tmp52C;});_tmp6C4.tms=_tmp984;});_tmp6C4;}));
# 2110
goto _LL0;}}case 288:  {
# 2111 "parse.y"
struct _tuple28*_tmp52D=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp25=_tmp52D;struct _tuple28*_tmp52E=_stmttmp25;void*_tmp533;void*_tmp532;void*_tmp531;int _tmp530;void*_tmp52F;_tmp52F=_tmp52E->f1;_tmp530=_tmp52E->f2;_tmp531=_tmp52E->f3;_tmp532=_tmp52E->f4;_tmp533=_tmp52E->f5;{struct Cyc_List_List*lis=_tmp52F;int b=_tmp530;struct Cyc_Absyn_VarargInfo*c=_tmp531;void*eff=_tmp532;struct Cyc_List_List*po=_tmp533;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C5;({struct Cyc_List_List*_tmp98A=({struct Cyc_List_List*_tmp536=_region_malloc(yyr,sizeof(*_tmp536));({void*_tmp989=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp535=_region_malloc(yyr,sizeof(*_tmp535));_tmp535->tag=3,({void*_tmp988=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp534=_region_malloc(yyr,sizeof(*_tmp534));_tmp534->tag=1,_tmp534->f1=lis,_tmp534->f2=b,_tmp534->f3=c,_tmp534->f4=eff,_tmp534->f5=po,({
struct Cyc_Absyn_Exp*_tmp987=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;_tmp534->f6=_tmp987;}),({struct Cyc_Absyn_Exp*_tmp986=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;_tmp534->f7=_tmp986;});_tmp534;});
# 2112
_tmp535->f1=_tmp988;});_tmp535;});_tmp536->hd=_tmp989;}),({
struct Cyc_List_List*_tmp985=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp536->tl=_tmp985;});_tmp536;});
# 2112
_tmp6C5.tms=_tmp98A;});_tmp6C5;}));
# 2115
goto _LL0;}}case 289:  {
# 2117
struct Cyc_List_List*_tmp537=({unsigned _tmp98B=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp98B,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp537;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C6;({struct Cyc_List_List*_tmp98E=({struct Cyc_List_List*_tmp539=_region_malloc(yyr,sizeof(*_tmp539));({void*_tmp98D=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp538=_region_malloc(yyr,sizeof(*_tmp538));_tmp538->tag=4,_tmp538->f1=ts,_tmp538->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp538->f3=0;_tmp538;});_tmp539->hd=_tmp98D;}),({
struct Cyc_List_List*_tmp98C=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp539->tl=_tmp98C;});_tmp539;});
# 2118
_tmp6C6.tms=_tmp98E;});_tmp6C6;}));
# 2121
goto _LL0;}case 290:
# 2122 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6C7;({struct Cyc_List_List*_tmp992=({struct Cyc_List_List*_tmp53B=_region_malloc(yyr,sizeof(*_tmp53B));({void*_tmp991=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp53A=_region_malloc(yyr,sizeof(*_tmp53A));_tmp53A->tag=5,_tmp53A->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp990=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp53A->f2=_tmp990;});_tmp53A;});_tmp53B->hd=_tmp991;}),({struct Cyc_List_List*_tmp98F=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp53B->tl=_tmp98F;});_tmp53B;});_tmp6C7.tms=_tmp992;});_tmp6C7;}));
# 2124
goto _LL0;case 291:
# 2127 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6C8;_tmp6C8.f1=0,_tmp6C8.f2=0;_tmp6C8;}));
goto _LL0;case 292:
# 2128 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6C9;({struct Cyc_Absyn_Exp*_tmp993=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6C9.f1=_tmp993;}),_tmp6C9.f2=0;_tmp6C9;}));
goto _LL0;case 293:
# 2129 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6CA;_tmp6CA.f1=0,({struct Cyc_Absyn_Exp*_tmp994=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6CA.f2=_tmp994;});_tmp6CA;}));
goto _LL0;case 294:
# 2131 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6CB;({struct Cyc_Absyn_Exp*_tmp996=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6CB.f1=_tmp996;}),({struct Cyc_Absyn_Exp*_tmp995=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp6CB.f2=_tmp995;});_tmp6CB;}));
goto _LL0;case 295:
# 2135 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 296:
# 2136 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 297:
# 2137 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 298:
# 2138 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299:
# 2139 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300:
# 2140 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301:
# 2146 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp99A=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->tag=13,({struct _fat_ptr*_tmp999=({struct _fat_ptr*_tmp53C=_cycalloc(sizeof(*_tmp53C));({struct _fat_ptr _tmp998=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp53C=_tmp998;});_tmp53C;});_tmp53D->f1=_tmp999;}),({struct Cyc_Absyn_Stmt*_tmp997=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp53D->f2=_tmp997;});_tmp53D;});Cyc_Absyn_new_stmt(_tmp99A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 302:
# 2150 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 303:
# 2151 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp99B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmp99B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304:
# 2156 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305:
# 2157 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 306:
# 2162 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp99C=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmp99C,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 307:
# 2163 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp99D=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmp99D,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 308:
# 2165 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9A2=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->tag=13,({struct _fat_ptr*_tmp9A1=({struct _fat_ptr*_tmp53E=_cycalloc(sizeof(*_tmp53E));({struct _fat_ptr _tmp9A0=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp53E=_tmp9A0;});_tmp53E;});_tmp53F->f1=_tmp9A1;}),({struct Cyc_Absyn_Stmt*_tmp99F=({struct Cyc_List_List*_tmp99E=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmp99E,Cyc_Absyn_skip_stmt(0U));});_tmp53F->f2=_tmp99F;});_tmp53F;});Cyc_Absyn_new_stmt(_tmp9A2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 309:
# 2167 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9A7=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp541=_cycalloc(sizeof(*_tmp541));_tmp541->tag=13,({struct _fat_ptr*_tmp9A6=({struct _fat_ptr*_tmp540=_cycalloc(sizeof(*_tmp540));({struct _fat_ptr _tmp9A5=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp540=_tmp9A5;});_tmp540;});_tmp541->f1=_tmp9A6;}),({struct Cyc_Absyn_Stmt*_tmp9A4=({struct Cyc_List_List*_tmp9A3=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmp9A3,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp541->f2=_tmp9A4;});_tmp541;});Cyc_Absyn_new_stmt(_tmp9A7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 310:
# 2168 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 311:
# 2169 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9A9=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp9A8=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmp9A9,_tmp9A8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 312:
# 2170 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmp9AC=({void*_tmp9AB=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->tag=1,({struct Cyc_Absyn_Fndecl*_tmp9AA=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp542->f1=_tmp9AA;});_tmp542;});Cyc_Absyn_new_decl(_tmp9AB,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmp9AC,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 313:
# 2173 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmp9AF=({void*_tmp9AE=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->tag=1,({struct Cyc_Absyn_Fndecl*_tmp9AD=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp543->f1=_tmp9AD;});_tmp543;});Cyc_Absyn_new_decl(_tmp9AE,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmp9AF,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 314:
# 2178 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9B2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9B1=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmp9B0=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmp9B2,_tmp9B1,_tmp9B0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 315:
# 2180 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9B5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9B4=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmp9B3=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmp9B5,_tmp9B4,_tmp9B3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 316:
# 2186 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9B7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp9B6=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmp9B7,_tmp9B6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317:
# 2189
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9BA=({struct _tuple0*_tmp9B8=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmp9B8,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_tmp9B9=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmp9BA,_tmp9B9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318:
# 2192
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9BD=({struct Cyc_List_List*_tmp9BB=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmp9BB,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_tmp9BC=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmp9BD,_tmp9BC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319:
# 2196 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9BF=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp9BE=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmp9BF,_tmp9BE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 320:
# 2210 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 321:
# 2213 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp545=_cycalloc(sizeof(*_tmp545));({struct Cyc_Absyn_Switch_clause*_tmp9C3=({struct Cyc_Absyn_Switch_clause*_tmp544=_cycalloc(sizeof(*_tmp544));({struct Cyc_Absyn_Pat*_tmp9C2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp544->pattern=_tmp9C2;}),_tmp544->pat_vars=0,_tmp544->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmp9C1=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp544->body=_tmp9C1;}),_tmp544->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp544;});
# 2213
_tmp545->hd=_tmp9C3;}),({
struct Cyc_List_List*_tmp9C0=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp545->tl=_tmp9C0;});_tmp545;}));
goto _LL0;case 322:
# 2216 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp547=_cycalloc(sizeof(*_tmp547));({struct Cyc_Absyn_Switch_clause*_tmp9C7=({struct Cyc_Absyn_Switch_clause*_tmp546=_cycalloc(sizeof(*_tmp546));({struct Cyc_Absyn_Pat*_tmp9C6=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp546->pattern=_tmp9C6;}),_tmp546->pat_vars=0,_tmp546->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmp9C5=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp546->body=_tmp9C5;}),_tmp546->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp546;});
# 2216
_tmp547->hd=_tmp9C7;}),({
# 2218
struct Cyc_List_List*_tmp9C4=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp547->tl=_tmp9C4;});_tmp547;}));
goto _LL0;case 323:
# 2220 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp549=_cycalloc(sizeof(*_tmp549));({struct Cyc_Absyn_Switch_clause*_tmp9CB=({struct Cyc_Absyn_Switch_clause*_tmp548=_cycalloc(sizeof(*_tmp548));({struct Cyc_Absyn_Pat*_tmp9CA=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp548->pattern=_tmp9CA;}),_tmp548->pat_vars=0,_tmp548->where_clause=0,({struct Cyc_Absyn_Stmt*_tmp9C9=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp548->body=_tmp9C9;}),_tmp548->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp548;});_tmp549->hd=_tmp9CB;}),({struct Cyc_List_List*_tmp9C8=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp549->tl=_tmp9C8;});_tmp549;}));
goto _LL0;case 324:
# 2222 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp54B=_cycalloc(sizeof(*_tmp54B));({struct Cyc_Absyn_Switch_clause*_tmp9D0=({struct Cyc_Absyn_Switch_clause*_tmp54A=_cycalloc(sizeof(*_tmp54A));({struct Cyc_Absyn_Pat*_tmp9CF=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp54A->pattern=_tmp9CF;}),_tmp54A->pat_vars=0,({struct Cyc_Absyn_Exp*_tmp9CE=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp54A->where_clause=_tmp9CE;}),({
struct Cyc_Absyn_Stmt*_tmp9CD=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp54A->body=_tmp9CD;}),_tmp54A->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp54A;});
# 2222
_tmp54B->hd=_tmp9D0;}),({
# 2224
struct Cyc_List_List*_tmp9CC=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp54B->tl=_tmp9CC;});_tmp54B;}));
goto _LL0;case 325:
# 2226 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp54D=_cycalloc(sizeof(*_tmp54D));({struct Cyc_Absyn_Switch_clause*_tmp9D5=({struct Cyc_Absyn_Switch_clause*_tmp54C=_cycalloc(sizeof(*_tmp54C));({struct Cyc_Absyn_Pat*_tmp9D4=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp54C->pattern=_tmp9D4;}),_tmp54C->pat_vars=0,({struct Cyc_Absyn_Exp*_tmp9D3=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp54C->where_clause=_tmp9D3;}),({struct Cyc_Absyn_Stmt*_tmp9D2=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp54C->body=_tmp9D2;}),_tmp54C->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp54C;});_tmp54D->hd=_tmp9D5;}),({struct Cyc_List_List*_tmp9D1=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp54D->tl=_tmp9D1;});_tmp54D;}));
goto _LL0;case 326:
# 2233 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9D7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9D6=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmp9D7,_tmp9D6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 327:
# 2237 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9D9=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp9D8=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmp9D9,_tmp9D8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 328:
# 2241 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9DD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmp9DC=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmp9DB=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmp9DA=Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);Cyc_Absyn_for_stmt(_tmp9DD,_tmp9DC,_tmp9DB,_tmp9DA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329:  {
# 2243 "parse.y"
struct Cyc_Absyn_Stmt*_tmp54E=({struct Cyc_Absyn_Exp*_tmp9E1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmp9E0=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmp9DF=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmp9DE=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmp9E1,_tmp9E0,_tmp9DF,_tmp9DE,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp54E;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp9E2=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmp9E2,s);}));
goto _LL0;}case 330:
# 2247
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 331:
# 2248 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_yyget_Exp_tok(&(yyyvsp[0]).v));
goto _LL0;case 332:
# 2251
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmp9E4=({struct _fat_ptr*_tmp54F=_cycalloc(sizeof(*_tmp54F));({struct _fat_ptr _tmp9E3=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp54F=_tmp9E3;});_tmp54F;});Cyc_Absyn_goto_stmt(_tmp9E4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 333:
# 2252 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 334:
# 2253 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 335:
# 2254 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 336:
# 2255 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9E5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmp9E5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 337:
# 2257 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 338:
# 2258 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 339:
# 2260 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp9E6=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmp9E6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 340:
# 2269 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 341:
# 2272
 yyval=(yyyvsp[0]).v;
goto _LL0;case 342:
# 2274 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9E9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9E8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmp9E7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmp9E9,_tmp9E8,_tmp9E7,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 343:
# 2277
 yyval=(yyyvsp[0]).v;
goto _LL0;case 344:
# 2279 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9EB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9EA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmp9EB,_tmp9EA,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 345:
# 2282
 yyval=(yyyvsp[0]).v;
goto _LL0;case 346:
# 2284 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9ED=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9EC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmp9ED,_tmp9EC,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 347:
# 2287
 yyval=(yyyvsp[0]).v;
goto _LL0;case 348:
# 2289 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9EF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9EE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmp9EF,_tmp9EE,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 349:
# 2292
 yyval=(yyyvsp[0]).v;
goto _LL0;case 350:
# 2294 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9F1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9F0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmp9F1,_tmp9F0,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 351:
# 2297
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352:
# 2299 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9F3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9F2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmp9F3,_tmp9F2,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353:
# 2302
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354:
# 2304 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_tmp9F6)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp9F5=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9F4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp9F6(_tmp9F5,_tmp9F4,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355:
# 2307
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356:
# 2309 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_tmp9F9)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp9F8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9F7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp9F9(_tmp9F8,_tmp9F7,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357:
# 2312
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358:
# 2314 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9FB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9FA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmp9FB,_tmp9FA,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359:
# 2316 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmp9FD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9FC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmp9FD,_tmp9FC,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 360:
# 2319
 yyval=(yyyvsp[0]).v;
goto _LL0;case 361:
# 2321 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA00=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp9FF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmp9FE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpA00,_tmp9FF,_tmp9FE,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 362:
# 2324
 yyval=(yyyvsp[0]).v;
goto _LL0;case 363:
# 2326 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA03=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA02=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA01=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpA03,_tmpA02,_tmpA01,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 364:
# 2329
 yyval=(yyyvsp[0]).v;
goto _LL0;case 365:  {
# 2331 "parse.y"
void*_tmp550=({struct _tuple8*_tmpA04=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpA04,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp550;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpA06=t;struct Cyc_Absyn_Exp*_tmpA05=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpA06,_tmpA05,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2334
goto _LL0;}case 366:
# 2337 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367:
# 2341 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA08=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA07=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpA08,_tmpA07,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368:  {
# 2343 "parse.y"
void*_tmp551=({struct _tuple8*_tmpA09=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpA09,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp551;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
goto _LL0;}case 369:
# 2346 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpA0A,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370:
# 2348 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpA0C=(*Cyc_yyget_YY38(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpA0B=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpA0C,_tmpA0B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371:
# 2353 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 372:
# 2361 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 373:
# 2365 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 374:
# 2366 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 375:
# 2367 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA0E=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->tag=15,({struct _tuple0*_tmpA0D=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp552->f1=_tmpA0D;});_tmp552;});Cyc_Absyn_new_pat(_tmpA0E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 376:
# 2368 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA10=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->tag=6,({struct Cyc_Absyn_Pat*_tmpA0F=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp553->f1=_tmpA0F;});_tmp553;});Cyc_Absyn_new_pat(_tmpA10,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 377:  {
# 2370 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp554=e->r;void*_stmttmp26=_tmp554;void*_tmp555=_stmttmp26;struct _fat_ptr _tmp556;int _tmp557;short _tmp559;char _tmp55A;enum Cyc_Absyn_Sign _tmp558;if(*((int*)_tmp555)== 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).LongLong_c).tag){case 2: _tmp558=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Char_c).val).f1;_tmp55A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Char_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp558;char i=_tmp55A;
# 2374
yyval=Cyc_YY9(({void*_tmpA11=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->tag=11,_tmp55B->f1=i;_tmp55B;});Cyc_Absyn_new_pat(_tmpA11,e->loc);}));goto _LL47E;}case 4: _tmp558=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Short_c).val).f1;_tmp559=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Short_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp558;short i=_tmp559;
# 2376
yyval=Cyc_YY9(({void*_tmpA12=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp55C=_cycalloc(sizeof(*_tmp55C));_tmp55C->tag=10,_tmp55C->f1=s,_tmp55C->f2=(int)i;_tmp55C;});Cyc_Absyn_new_pat(_tmpA12,e->loc);}));goto _LL47E;}case 5: _tmp558=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Int_c).val).f1;_tmp557=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Int_c).val).f2;{enum Cyc_Absyn_Sign s=_tmp558;int i=_tmp557;
# 2378
yyval=Cyc_YY9(({void*_tmpA13=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=10,_tmp55D->f1=s,_tmp55D->f2=i;_tmp55D;});Cyc_Absyn_new_pat(_tmpA13,e->loc);}));goto _LL47E;}case 7: _tmp556=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Float_c).val).f1;_tmp557=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp555)->f1).Float_c).val).f2;{struct _fat_ptr s=_tmp556;int i=_tmp557;
# 2380
yyval=Cyc_YY9(({void*_tmpA14=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->tag=12,_tmp55E->f1=s,_tmp55E->f2=i;_tmp55E;});Cyc_Absyn_new_pat(_tmpA14,e->loc);}));goto _LL47E;}case 1:
# 2382
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL47E;case 8:
 goto _LL48C;case 9: _LL48C:
# 2385
({void*_tmp55F=0U;({unsigned _tmpA16=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA15=({const char*_tmp560="strings cannot occur within patterns";_tag_fat(_tmp560,sizeof(char),37U);});Cyc_Warn_err(_tmpA16,_tmpA15,_tag_fat(_tmp55F,sizeof(void*),0));});});goto _LL47E;case 6:
# 2387
({void*_tmp561=0U;({unsigned _tmpA18=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA17=({const char*_tmp562="long long's in patterns not yet implemented";_tag_fat(_tmp562,sizeof(char),44U);});Cyc_Warn_err(_tmpA18,_tmpA17,_tag_fat(_tmp561,sizeof(void*),0));});});goto _LL47E;default: goto _LL48F;}else{_LL48F:
# 2389
({void*_tmp563=0U;({unsigned _tmpA1A=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA19=({const char*_tmp564="bad constant in case";_tag_fat(_tmp564,sizeof(char),21U);});Cyc_Warn_err(_tmpA1A,_tmpA19,_tag_fat(_tmp563,sizeof(void*),0));});});}_LL47E:;}
# 2392
goto _LL0;}case 378:
# 2393 "parse.y"
 if(({struct _fat_ptr _tmpA1B=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpA1B,({const char*_tmp565="as";_tag_fat(_tmp565,sizeof(char),3U);}));})!= 0)
({void*_tmp566=0U;({unsigned _tmpA1D=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA1C=({const char*_tmp567="expecting `as'";_tag_fat(_tmp567,sizeof(char),15U);});Cyc_Warn_err(_tmpA1D,_tmpA1C,_tag_fat(_tmp566,sizeof(void*),0));});});
yyval=Cyc_YY9(({void*_tmpA24=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->tag=1,({struct Cyc_Absyn_Vardecl*_tmpA23=({unsigned _tmpA22=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA21=({struct _tuple0*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA20=({struct _fat_ptr*_tmp568=_cycalloc(sizeof(*_tmp568));({struct _fat_ptr _tmpA1F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp568=_tmpA1F;});_tmp568;});_tmp569->f2=_tmpA20;});_tmp569;});Cyc_Absyn_new_vardecl(_tmpA22,_tmpA21,Cyc_Absyn_void_type,0);});_tmp56A->f1=_tmpA23;}),({
struct Cyc_Absyn_Pat*_tmpA1E=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp56A->f2=_tmpA1E;});_tmp56A;});
# 2395
Cyc_Absyn_new_pat(_tmpA24,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2398
goto _LL0;case 379:
# 2399 "parse.y"
 if(({struct _fat_ptr _tmpA25=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpA25,({const char*_tmp56B="alias";_tag_fat(_tmp56B,sizeof(char),6U);}));})!= 0)
({void*_tmp56C=0U;({unsigned _tmpA27=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA26=({const char*_tmp56D="expecting `alias'";_tag_fat(_tmp56D,sizeof(char),18U);});Cyc_Warn_err(_tmpA27,_tmpA26,_tag_fat(_tmp56C,sizeof(void*),0));});});{
int _tmp56E=((yyyvsp[0]).l).first_line;int location=_tmp56E;
({struct _fat_ptr _tmpA28=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpA28,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp574=_cycalloc(sizeof(*_tmp574));({struct _fat_ptr*_tmpA2B=({struct _fat_ptr*_tmp572=_cycalloc(sizeof(*_tmp572));({struct _fat_ptr _tmpA2A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp572=_tmpA2A;});_tmp572;});_tmp574->name=_tmpA2B;}),_tmp574->identity=- 1,({void*_tmpA29=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=0,_tmp573->f1=& Cyc_Kinds_rk;_tmp573;});_tmp574->kind=_tmpA29;});_tmp574;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpA30=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA2F=({struct _tuple0*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA2D=({struct _fat_ptr*_tmp570=_cycalloc(sizeof(*_tmp570));({struct _fat_ptr _tmpA2C=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp570=_tmpA2C;});_tmp570;});_tmp571->f2=_tmpA2D;});_tmp571;});Cyc_Absyn_new_vardecl(_tmpA30,_tmpA2F,({
struct _tuple8*_tmpA2E=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpA2E,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpA31=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=2,_tmp56F->f1=tv,_tmp56F->f2=vd;_tmp56F;});Cyc_Absyn_new_pat(_tmpA31,(unsigned)location);}));
# 2408
goto _LL0;}}case 380:
# 2409 "parse.y"
 if(({struct _fat_ptr _tmpA32=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpA32,({const char*_tmp575="alias";_tag_fat(_tmp575,sizeof(char),6U);}));})!= 0)
({void*_tmp576=0U;({unsigned _tmpA34=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA33=({const char*_tmp577="expecting `alias'";_tag_fat(_tmp577,sizeof(char),18U);});Cyc_Warn_err(_tmpA34,_tmpA33,_tag_fat(_tmp576,sizeof(void*),0));});});{
int _tmp578=((yyyvsp[0]).l).first_line;int location=_tmp578;
({struct _fat_ptr _tmpA35=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpA35,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct _fat_ptr*_tmpA38=({struct _fat_ptr*_tmp57C=_cycalloc(sizeof(*_tmp57C));({struct _fat_ptr _tmpA37=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp57C=_tmpA37;});_tmp57C;});_tmp57E->name=_tmpA38;}),_tmp57E->identity=- 1,({void*_tmpA36=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=0,_tmp57D->f1=& Cyc_Kinds_rk;_tmp57D;});_tmp57E->kind=_tmpA36;});_tmp57E;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpA3D=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA3C=({struct _tuple0*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA3A=({struct _fat_ptr*_tmp57A=_cycalloc(sizeof(*_tmp57A));({struct _fat_ptr _tmpA39=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp57A=_tmpA39;});_tmp57A;});_tmp57B->f2=_tmpA3A;});_tmp57B;});Cyc_Absyn_new_vardecl(_tmpA3D,_tmpA3C,({
struct _tuple8*_tmpA3B=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpA3B,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpA3E=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=2,_tmp579->f1=tv,_tmp579->f2=vd;_tmp579;});Cyc_Absyn_new_pat(_tmpA3E,(unsigned)location);}));
# 2418
goto _LL0;}}case 381:  {
# 2419 "parse.y"
struct _tuple23 _tmp57F=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp27=_tmp57F;struct _tuple23 _tmp580=_stmttmp27;int _tmp582;void*_tmp581;_tmp581=_tmp580.f1;_tmp582=_tmp580.f2;{struct Cyc_List_List*ps=_tmp581;int dots=_tmp582;
yyval=Cyc_YY9(({void*_tmpA3F=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp583=_cycalloc(sizeof(*_tmp583));_tmp583->tag=5,_tmp583->f1=ps,_tmp583->f2=dots;_tmp583;});Cyc_Absyn_new_pat(_tmpA3F,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2422
goto _LL0;}}case 382:  {
# 2423 "parse.y"
struct _tuple23 _tmp584=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp28=_tmp584;struct _tuple23 _tmp585=_stmttmp28;int _tmp587;void*_tmp586;_tmp586=_tmp585.f1;_tmp587=_tmp585.f2;{struct Cyc_List_List*ps=_tmp586;int dots=_tmp587;
yyval=Cyc_YY9(({void*_tmpA41=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->tag=16,({struct _tuple0*_tmpA40=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp588->f1=_tmpA40;}),_tmp588->f2=ps,_tmp588->f3=dots;_tmp588;});Cyc_Absyn_new_pat(_tmpA41,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2426
goto _LL0;}}case 383:  {
# 2427 "parse.y"
struct _tuple23 _tmp589=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp29=_tmp589;struct _tuple23 _tmp58A=_stmttmp29;int _tmp58C;void*_tmp58B;_tmp58B=_tmp58A.f1;_tmp58C=_tmp58A.f2;{struct Cyc_List_List*fps=_tmp58B;int dots=_tmp58C;
struct Cyc_List_List*_tmp58D=({unsigned _tmpA42=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA42,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp58D;
yyval=Cyc_YY9(({void*_tmpA45=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->tag=7,({union Cyc_Absyn_AggrInfo*_tmpA44=({union Cyc_Absyn_AggrInfo*_tmp58E=_cycalloc(sizeof(*_tmp58E));({union Cyc_Absyn_AggrInfo _tmpA43=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp58E=_tmpA43;});_tmp58E;});_tmp58F->f1=_tmpA44;}),_tmp58F->f2=exist_ts,_tmp58F->f3=fps,_tmp58F->f4=dots;_tmp58F;});Cyc_Absyn_new_pat(_tmpA45,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2432
goto _LL0;}}case 384:  {
# 2433 "parse.y"
struct _tuple23 _tmp590=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2A=_tmp590;struct _tuple23 _tmp591=_stmttmp2A;int _tmp593;void*_tmp592;_tmp592=_tmp591.f1;_tmp593=_tmp591.f2;{struct Cyc_List_List*fps=_tmp592;int dots=_tmp593;
struct Cyc_List_List*_tmp594=({unsigned _tmpA46=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA46,Cyc_yyget_YY41(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp594;
yyval=Cyc_YY9(({void*_tmpA47=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->tag=7,_tmp595->f1=0,_tmp595->f2=exist_ts,_tmp595->f3=fps,_tmp595->f4=dots;_tmp595;});Cyc_Absyn_new_pat(_tmpA47,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2437
goto _LL0;}}case 385:
# 2438 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA4B=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->tag=6,({struct Cyc_Absyn_Pat*_tmpA4A=({void*_tmpA49=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->tag=6,({struct Cyc_Absyn_Pat*_tmpA48=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp596->f1=_tmpA48;});_tmp596;});Cyc_Absyn_new_pat(_tmpA49,(unsigned)((yyyvsp[0]).l).first_line);});_tmp597->f1=_tmpA4A;});_tmp597;});Cyc_Absyn_new_pat(_tmpA4B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 386:
# 2440 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA52=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->tag=3,({struct Cyc_Absyn_Vardecl*_tmpA51=({unsigned _tmpA50=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA4F=({struct _tuple0*_tmp599=_cycalloc(sizeof(*_tmp599));_tmp599->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA4E=({struct _fat_ptr*_tmp598=_cycalloc(sizeof(*_tmp598));({struct _fat_ptr _tmpA4D=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp598=_tmpA4D;});_tmp598;});_tmp599->f2=_tmpA4E;});_tmp599;});Cyc_Absyn_new_vardecl(_tmpA50,_tmpA4F,Cyc_Absyn_void_type,0);});_tmp59A->f1=_tmpA51;}),({
# 2442
struct Cyc_Absyn_Pat*_tmpA4C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp59A->f2=_tmpA4C;});_tmp59A;});
# 2440
Cyc_Absyn_new_pat(_tmpA52,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2444
goto _LL0;case 387:
# 2445 "parse.y"
 if(({struct _fat_ptr _tmpA53=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpA53,({const char*_tmp59B="as";_tag_fat(_tmp59B,sizeof(char),3U);}));})!= 0)
({void*_tmp59C=0U;({unsigned _tmpA55=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpA54=({const char*_tmp59D="expecting `as'";_tag_fat(_tmp59D,sizeof(char),15U);});Cyc_Warn_err(_tmpA55,_tmpA54,_tag_fat(_tmp59C,sizeof(void*),0));});});
yyval=Cyc_YY9(({void*_tmpA5C=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->tag=3,({struct Cyc_Absyn_Vardecl*_tmpA5B=({unsigned _tmpA5A=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA59=({struct _tuple0*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA58=({struct _fat_ptr*_tmp59E=_cycalloc(sizeof(*_tmp59E));({struct _fat_ptr _tmpA57=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp59E=_tmpA57;});_tmp59E;});_tmp59F->f2=_tmpA58;});_tmp59F;});Cyc_Absyn_new_vardecl(_tmpA5A,_tmpA59,Cyc_Absyn_void_type,0);});_tmp5A0->f1=_tmpA5B;}),({
# 2449
struct Cyc_Absyn_Pat*_tmpA56=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp5A0->f2=_tmpA56;});_tmp5A0;});
# 2447
Cyc_Absyn_new_pat(_tmpA5C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2451
goto _LL0;case 388:  {
# 2452 "parse.y"
void*_tmp5A1=({struct _fat_ptr _tmpA5D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpA5D,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});void*tag=_tmp5A1;
yyval=Cyc_YY9(({void*_tmpA64=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));_tmp5A4->tag=4,({struct Cyc_Absyn_Tvar*_tmpA63=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp5A4->f1=_tmpA63;}),({
struct Cyc_Absyn_Vardecl*_tmpA62=({unsigned _tmpA61=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA60=({struct _tuple0*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA5F=({struct _fat_ptr*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));({struct _fat_ptr _tmpA5E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5A2=_tmpA5E;});_tmp5A2;});_tmp5A3->f2=_tmpA5F;});_tmp5A3;});Cyc_Absyn_new_vardecl(_tmpA61,_tmpA60,
Cyc_Absyn_tag_type(tag),0);});
# 2454
_tmp5A4->f2=_tmpA62;});_tmp5A4;});
# 2453
Cyc_Absyn_new_pat(_tmpA64,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2457
goto _LL0;}case 389:  {
# 2458 "parse.y"
struct Cyc_Absyn_Tvar*_tmp5A5=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp5A5;
yyval=Cyc_YY9(({void*_tmpA6A=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->tag=4,_tmp5A8->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpA69=({unsigned _tmpA68=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA67=({struct _tuple0*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA66=({struct _fat_ptr*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));({struct _fat_ptr _tmpA65=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5A6=_tmpA65;});_tmp5A6;});_tmp5A7->f2=_tmpA66;});_tmp5A7;});Cyc_Absyn_new_vardecl(_tmpA68,_tmpA67,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2460
_tmp5A8->f2=_tmpA69;});_tmp5A8;});
# 2459
Cyc_Absyn_new_pat(_tmpA6A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2463
goto _LL0;}case 390:
# 2466 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5A9=_cycalloc(sizeof(*_tmp5A9));({struct Cyc_List_List*_tmpA6B=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp5A9->f1=_tmpA6B;}),_tmp5A9->f2=0;_tmp5A9;}));
goto _LL0;case 391:
# 2467 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));({struct Cyc_List_List*_tmpA6C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp5AA->f1=_tmpA6C;}),_tmp5AA->f2=1;_tmp5AA;}));
goto _LL0;case 392:
# 2468 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->f1=0,_tmp5AB->f2=1;_tmp5AB;}));
goto _LL0;case 393:
# 2471
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));({struct Cyc_Absyn_Pat*_tmpA6D=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp5AC->hd=_tmpA6D;}),_tmp5AC->tl=0;_tmp5AC;}));
goto _LL0;case 394:
# 2472 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));({struct Cyc_Absyn_Pat*_tmpA6F=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp5AD->hd=_tmpA6F;}),({struct Cyc_List_List*_tmpA6E=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp5AD->tl=_tmpA6E;});_tmp5AD;}));
goto _LL0;case 395:
# 2475
 yyval=Cyc_YY12(({struct _tuple24*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->f1=0,({struct Cyc_Absyn_Pat*_tmpA70=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp5AE->f2=_tmpA70;});_tmp5AE;}));
goto _LL0;case 396:
# 2476 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp5AF=_cycalloc(sizeof(*_tmp5AF));({struct Cyc_List_List*_tmpA72=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp5AF->f1=_tmpA72;}),({struct Cyc_Absyn_Pat*_tmpA71=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5AF->f2=_tmpA71;});_tmp5AF;}));
goto _LL0;case 397:
# 2479
 yyval=Cyc_YY14(({struct _tuple23*_tmp5B0=_cycalloc(sizeof(*_tmp5B0));({struct Cyc_List_List*_tmpA73=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp5B0->f1=_tmpA73;}),_tmp5B0->f2=0;_tmp5B0;}));
goto _LL0;case 398:
# 2480 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));({struct Cyc_List_List*_tmpA74=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp5B1->f1=_tmpA74;}),_tmp5B1->f2=1;_tmp5B1;}));
goto _LL0;case 399:
# 2481 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));_tmp5B2->f1=0,_tmp5B2->f2=1;_tmp5B2;}));
goto _LL0;case 400:
# 2484
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));({struct _tuple24*_tmpA75=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp5B3->hd=_tmpA75;}),_tmp5B3->tl=0;_tmp5B3;}));
goto _LL0;case 401:
# 2485 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));({struct _tuple24*_tmpA77=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp5B4->hd=_tmpA77;}),({struct Cyc_List_List*_tmpA76=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp5B4->tl=_tmpA76;});_tmp5B4;}));
goto _LL0;case 402:
# 2490 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 403:
# 2491 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA79=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA78=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpA79,_tmpA78,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404:
# 2495 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 405:
# 2497 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpA7B=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA7A=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpA7C,_tmpA7B,_tmpA7A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 406:
# 2499 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA7D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpA7E,_tmpA7D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 407:
# 2503 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 408:
# 2504 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->v=(void*)Cyc_Absyn_Times;_tmp5B5;}));
goto _LL0;case 409:
# 2505 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->v=(void*)Cyc_Absyn_Div;_tmp5B6;}));
goto _LL0;case 410:
# 2506 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->v=(void*)Cyc_Absyn_Mod;_tmp5B7;}));
goto _LL0;case 411:
# 2507 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->v=(void*)Cyc_Absyn_Plus;_tmp5B8;}));
goto _LL0;case 412:
# 2508 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->v=(void*)Cyc_Absyn_Minus;_tmp5B9;}));
goto _LL0;case 413:
# 2509 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->v=(void*)Cyc_Absyn_Bitlshift;_tmp5BA;}));
goto _LL0;case 414:
# 2510 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->v=(void*)Cyc_Absyn_Bitlrshift;_tmp5BB;}));
goto _LL0;case 415:
# 2511 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->v=(void*)Cyc_Absyn_Bitand;_tmp5BC;}));
goto _LL0;case 416:
# 2512 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->v=(void*)Cyc_Absyn_Bitxor;_tmp5BD;}));
goto _LL0;case 417:
# 2513 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->v=(void*)Cyc_Absyn_Bitor;_tmp5BE;}));
goto _LL0;case 418:
# 2517 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 419:
# 2519 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA81=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA80=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA7F=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpA81,_tmpA80,_tmpA7F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 420:
# 2521 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA82=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpA82,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 421:
# 2523 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA83=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpA83,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422:
# 2524 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA84=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpA84,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 423:
# 2525 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA86=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA85=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpA86,_tmpA85,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424:
# 2527 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA88=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA87=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpA88,_tmpA87,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425:
# 2531 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 426:
# 2534
 yyval=(yyyvsp[0]).v;
goto _LL0;case 427:
# 2536 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA89=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpA8A,_tmpA89,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 428:
# 2539
 yyval=(yyyvsp[0]).v;
goto _LL0;case 429:
# 2541 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA8C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA8B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpA8C,_tmpA8B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 430:
# 2544
 yyval=(yyyvsp[0]).v;
goto _LL0;case 431:
# 2546 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA8E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA8D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpA8E,_tmpA8D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 432:
# 2549
 yyval=(yyyvsp[0]).v;
goto _LL0;case 433:
# 2551 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA90=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA8F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpA90,_tmpA8F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 434:
# 2554
 yyval=(yyyvsp[0]).v;
goto _LL0;case 435:
# 2556 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA92=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA91=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpA92,_tmpA91,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 436:
# 2559
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437:
# 2561 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_tmpA95)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA94=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA93=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpA95(_tmpA94,_tmpA93,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438:
# 2564
 yyval=(yyyvsp[0]).v;
goto _LL0;case 439:
# 2566 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_tmpA98)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA97=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA96=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpA98(_tmpA97,_tmpA96,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440:
# 2569
 yyval=(yyyvsp[0]).v;
goto _LL0;case 441:
# 2571 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA9A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA99=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpA9A,_tmpA99,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442:
# 2573 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA9C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA9B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpA9C,_tmpA9B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443:
# 2576
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444:
# 2578 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpA9F=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA9E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA9D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpA9F,_tmpA9E,_tmpA9D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 445:
# 2581
 yyval=(yyyvsp[0]).v;
goto _LL0;case 446:
# 2583 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpAA2=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAA1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAA0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpAA2,_tmpAA1,_tmpAA0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 447:
# 2586
 yyval=Cyc_YY66(Cyc_Absyn_eq_exp);
goto _LL0;case 448:
# 2587 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_neq_exp);
goto _LL0;case 449:
# 2590
 yyval=Cyc_YY66(Cyc_Absyn_lt_exp);
goto _LL0;case 450:
# 2591 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gt_exp);
goto _LL0;case 451:
# 2592 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_lte_exp);
goto _LL0;case 452:
# 2593 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gte_exp);
goto _LL0;case 453:
# 2596
 yyval=Cyc_YY6(Cyc_Absyn_Plus);
goto _LL0;case 454:
# 2597 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 455:
# 2600
 yyval=Cyc_YY6(Cyc_Absyn_Times);
goto _LL0;case 456:
# 2601 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Div);
goto _LL0;case 457:
# 2602 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Mod);
goto _LL0;case 458:
# 2606 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 459:  {
# 2608 "parse.y"
void*_tmp5BF=({struct _tuple8*_tmpAA3=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpAA3,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5BF;
yyval=Cyc_Exp_tok(({void*_tmpAA5=t;struct Cyc_Absyn_Exp*_tmpAA4=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpAA5,_tmpAA4,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 460:
# 2613 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 461:
# 2614 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA6=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpAA6,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462:
# 2615 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpAA7,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 463:
# 2616 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpAA8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464:
# 2617 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpAA9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465:
# 2618 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpAAB=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAAA=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpAAB,_tmpAAA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 466:  {
# 2620 "parse.y"
void*_tmp5C0=({struct _tuple8*_tmpAAC=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpAAC,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5C0;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2623
goto _LL0;}case 467:
# 2623 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpAAD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468:  {
# 2625 "parse.y"
void*_tmp5C1=({struct _tuple8*_tmpAAE=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpAAE,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5C1;
yyval=Cyc_Exp_tok(({void*_tmpAB0=t;struct Cyc_List_List*_tmpAAF=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpAB0,_tmpAAF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2628
goto _LL0;}case 469:
# 2630
 yyval=Cyc_Exp_tok(({void*_tmpAB2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->tag=34,(_tmp5C2->f1).is_calloc=0,(_tmp5C2->f1).rgn=0,(_tmp5C2->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5C2->f1).num_elts=_tmpAB1;}),(_tmp5C2->f1).fat_result=0,(_tmp5C2->f1).inline_call=0;_tmp5C2;});Cyc_Absyn_new_exp(_tmpAB2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2632
goto _LL0;case 470:
# 2633 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAB5=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->tag=34,(_tmp5C3->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpAB4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5C3->f1).rgn=_tmpAB4;}),(_tmp5C3->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5C3->f1).num_elts=_tmpAB3;}),(_tmp5C3->f1).fat_result=0,(_tmp5C3->f1).inline_call=0;_tmp5C3;});Cyc_Absyn_new_exp(_tmpAB5,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2635
goto _LL0;case 471:
# 2636 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAB8=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->tag=34,(_tmp5C4->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpAB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5C4->f1).rgn=_tmpAB7;}),(_tmp5C4->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAB6=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5C4->f1).num_elts=_tmpAB6;}),(_tmp5C4->f1).fat_result=0,(_tmp5C4->f1).inline_call=1;_tmp5C4;});Cyc_Absyn_new_exp(_tmpAB8,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2638
goto _LL0;case 472:  {
# 2639 "parse.y"
void*_tmp5C5=({struct _tuple8*_tmpAB9=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpAB9,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp5C5;
yyval=Cyc_Exp_tok(({void*_tmpABC=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->tag=34,(_tmp5C7->f1).is_calloc=1,(_tmp5C7->f1).rgn=0,({void**_tmpABB=({void**_tmp5C6=_cycalloc(sizeof(*_tmp5C6));*_tmp5C6=t;_tmp5C6;});(_tmp5C7->f1).elt_type=_tmpABB;}),({struct Cyc_Absyn_Exp*_tmpABA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5C7->f1).num_elts=_tmpABA;}),(_tmp5C7->f1).fat_result=0,(_tmp5C7->f1).inline_call=0;_tmp5C7;});Cyc_Absyn_new_exp(_tmpABC,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2642
goto _LL0;}case 473:  {
# 2644
void*_tmp5C8=({struct _tuple8*_tmpABD=Cyc_yyget_YY38(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpABD,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp5C8;
yyval=Cyc_Exp_tok(({void*_tmpAC1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->tag=34,(_tmp5CA->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpAC0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5CA->f1).rgn=_tmpAC0;}),({void**_tmpABF=({void**_tmp5C9=_cycalloc(sizeof(*_tmp5C9));*_tmp5C9=t;_tmp5C9;});(_tmp5CA->f1).elt_type=_tmpABF;}),({struct Cyc_Absyn_Exp*_tmpABE=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5CA->f1).num_elts=_tmpABE;}),(_tmp5CA->f1).fat_result=0,(_tmp5CA->f1).inline_call=0;_tmp5CA;});Cyc_Absyn_new_exp(_tmpAC1,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2647
goto _LL0;}case 474:
# 2648 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpAC3=({struct Cyc_Absyn_Exp*_tmp5CB[1];({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5CB[0]=_tmpAC2;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp5CB,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpAC3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 475:
# 2650 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAC7=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->tag=38,({struct Cyc_Absyn_Exp*_tmpAC6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5CD->f1=_tmpAC6;}),({struct _fat_ptr*_tmpAC5=({struct _fat_ptr*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));({struct _fat_ptr _tmpAC4=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp5CC=_tmpAC4;});_tmp5CC;});_tmp5CD->f2=_tmpAC5;});_tmp5CD;});Cyc_Absyn_new_exp(_tmpAC7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476:
# 2652 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpACC=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->tag=38,({struct Cyc_Absyn_Exp*_tmpACB=({struct Cyc_Absyn_Exp*_tmpACA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpACA,(unsigned)((yyyvsp[2]).l).first_line);});_tmp5CF->f1=_tmpACB;}),({struct _fat_ptr*_tmpAC9=({struct _fat_ptr*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));({struct _fat_ptr _tmpAC8=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp5CE=_tmpAC8;});_tmp5CE;});_tmp5CF->f2=_tmpAC9;});_tmp5CF;});Cyc_Absyn_new_exp(_tmpACC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477:  {
# 2654 "parse.y"
void*_tmp5D0=({struct _tuple8*_tmpACD=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpACD,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5D0;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 478:
# 2656 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpACE=Cyc_yyget_YY60(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpACE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479:
# 2657 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpACF=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpACF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480:
# 2658 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpAD0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481:
# 2662 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 482:
# 2663 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 483:
# 2664 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 484:
# 2665 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Plus);
goto _LL0;case 485:  {
# 2670 "parse.y"
struct _tuple30*_tmp5D1=Cyc_yyget_YY61(&(yyyvsp[3]).v);struct _tuple30*_stmttmp2B=_tmp5D1;struct _tuple30*_tmp5D2=_stmttmp2B;void*_tmp5D5;void*_tmp5D4;void*_tmp5D3;_tmp5D3=_tmp5D2->f1;_tmp5D4=_tmp5D2->f2;_tmp5D5=_tmp5D2->f3;{struct Cyc_List_List*outlist=_tmp5D3;struct Cyc_List_List*inlist=_tmp5D4;struct Cyc_List_List*clobbers=_tmp5D5;
yyval=Cyc_YY60((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->tag=40,({int _tmpAD2=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp5D6->f1=_tmpAD2;}),({struct _fat_ptr _tmpAD1=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp5D6->f2=_tmpAD1;}),_tmp5D6->f3=outlist,_tmp5D6->f4=inlist,_tmp5D6->f5=clobbers;_tmp5D6;}));
goto _LL0;}}case 486:
# 2675 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 487:
# 2676 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 488:
# 2680 "parse.y"
 yyval=Cyc_YY61(({struct _tuple30*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->f1=0,_tmp5D7->f2=0,_tmp5D7->f3=0;_tmp5D7;}));
goto _LL0;case 489:  {
# 2682 "parse.y"
struct _tuple31*_tmp5D8=Cyc_yyget_YY62(&(yyyvsp[1]).v);struct _tuple31*_stmttmp2C=_tmp5D8;struct _tuple31*_tmp5D9=_stmttmp2C;void*_tmp5DB;void*_tmp5DA;_tmp5DA=_tmp5D9->f1;_tmp5DB=_tmp5D9->f2;{struct Cyc_List_List*inlist=_tmp5DA;struct Cyc_List_List*clobbers=_tmp5DB;
yyval=Cyc_YY61(({struct _tuple30*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->f1=0,_tmp5DC->f2=inlist,_tmp5DC->f3=clobbers;_tmp5DC;}));
goto _LL0;}}case 490:  {
# 2685 "parse.y"
struct _tuple31*_tmp5DD=Cyc_yyget_YY62(&(yyyvsp[2]).v);struct _tuple31*_stmttmp2D=_tmp5DD;struct _tuple31*_tmp5DE=_stmttmp2D;void*_tmp5E0;void*_tmp5DF;_tmp5DF=_tmp5DE->f1;_tmp5E0=_tmp5DE->f2;{struct Cyc_List_List*inlist=_tmp5DF;struct Cyc_List_List*clobbers=_tmp5E0;
yyval=Cyc_YY61(({struct _tuple30*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));({struct Cyc_List_List*_tmpAD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY64(&(yyyvsp[1]).v));_tmp5E1->f1=_tmpAD3;}),_tmp5E1->f2=inlist,_tmp5E1->f3=clobbers;_tmp5E1;}));
goto _LL0;}}case 491:
# 2689
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));({struct _tuple32*_tmpAD4=Cyc_yyget_YY65(&(yyyvsp[0]).v);_tmp5E2->hd=_tmpAD4;}),_tmp5E2->tl=0;_tmp5E2;}));
goto _LL0;case 492:
# 2690 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));({struct _tuple32*_tmpAD6=Cyc_yyget_YY65(&(yyyvsp[2]).v);_tmp5E3->hd=_tmpAD6;}),({struct Cyc_List_List*_tmpAD5=Cyc_yyget_YY64(&(yyyvsp[0]).v);_tmp5E3->tl=_tmpAD5;});_tmp5E3;}));
goto _LL0;case 493:
# 2694 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->f1=0,_tmp5E4->f2=0;_tmp5E4;}));
goto _LL0;case 494:
# 2695 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->f1=0,({struct Cyc_List_List*_tmpAD7=Cyc_yyget_YY63(&(yyyvsp[1]).v);_tmp5E5->f2=_tmpAD7;});_tmp5E5;}));
goto _LL0;case 495:
# 2696 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));({struct Cyc_List_List*_tmpAD9=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY64(&(yyyvsp[1]).v));_tmp5E6->f1=_tmpAD9;}),({struct Cyc_List_List*_tmpAD8=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp5E6->f2=_tmpAD8;});_tmp5E6;}));
goto _LL0;case 496:
# 2699
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));({struct _tuple32*_tmpADA=Cyc_yyget_YY65(&(yyyvsp[0]).v);_tmp5E7->hd=_tmpADA;}),_tmp5E7->tl=0;_tmp5E7;}));
goto _LL0;case 497:
# 2700 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));({struct _tuple32*_tmpADC=Cyc_yyget_YY65(&(yyyvsp[2]).v);_tmp5E8->hd=_tmpADC;}),({struct Cyc_List_List*_tmpADB=Cyc_yyget_YY64(&(yyyvsp[0]).v);_tmp5E8->tl=_tmpADB;});_tmp5E8;}));
goto _LL0;case 498:
# 2704 "parse.y"
 yyval=Cyc_YY65(({struct _tuple32*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));({struct _fat_ptr _tmpADE=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp5E9->f1=_tmpADE;}),({struct Cyc_Absyn_Exp*_tmpADD=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5E9->f2=_tmpADD;});_tmp5E9;}));
goto _LL0;case 499:
# 2709 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 500:
# 2710 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 501:
# 2711 "parse.y"
 yyval=Cyc_YY63(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY63(&(yyyvsp[1]).v)));
goto _LL0;case 502:
# 2714
 yyval=Cyc_YY63(({struct Cyc_List_List*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));({struct _fat_ptr*_tmpAE0=({struct _fat_ptr*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));({struct _fat_ptr _tmpADF=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5EA=_tmpADF;});_tmp5EA;});_tmp5EB->hd=_tmpAE0;}),_tmp5EB->tl=0;_tmp5EB;}));
goto _LL0;case 503:
# 2715 "parse.y"
 yyval=Cyc_YY63(({struct Cyc_List_List*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));({struct _fat_ptr*_tmpAE3=({struct _fat_ptr*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));({struct _fat_ptr _tmpAE2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp5EC=_tmpAE2;});_tmp5EC;});_tmp5ED->hd=_tmpAE3;}),({struct Cyc_List_List*_tmpAE1=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp5ED->tl=_tmpAE1;});_tmp5ED;}));
goto _LL0;case 504:
# 2720 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 505:
# 2722 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAE5=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAE4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpAE5,_tmpAE4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 506:
# 2724 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAE6=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpAE6,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 507:
# 2726 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAE8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpAE7=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpAE8,_tmpAE7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 508:
# 2728 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAEB=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpAEA=({struct _fat_ptr*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));({struct _fat_ptr _tmpAE9=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp5EE=_tmpAE9;});_tmp5EE;});Cyc_Absyn_aggrmember_exp(_tmpAEB,_tmpAEA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 509:
# 2730 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAEE=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpAED=({struct _fat_ptr*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));({struct _fat_ptr _tmpAEC=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp5EF=_tmpAEC;});_tmp5EF;});Cyc_Absyn_aggrarrow_exp(_tmpAEE,_tmpAED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 510:
# 2732 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAEF=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpAEF,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 511:
# 2734 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAF0=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpAF0,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 512:
# 2736 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAF2=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->tag=25,({struct _tuple8*_tmpAF1=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp5F0->f1=_tmpAF1;}),_tmp5F0->f2=0;_tmp5F0;});Cyc_Absyn_new_exp(_tmpAF2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 513:
# 2738 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAF5=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->tag=25,({struct _tuple8*_tmpAF4=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp5F1->f1=_tmpAF4;}),({struct Cyc_List_List*_tmpAF3=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp5F1->f2=_tmpAF3;});_tmp5F1;});Cyc_Absyn_new_exp(_tmpAF5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 514:
# 2740 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAF8=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->tag=25,({struct _tuple8*_tmpAF7=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp5F2->f1=_tmpAF7;}),({struct Cyc_List_List*_tmpAF6=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp5F2->f2=_tmpAF6;});_tmp5F2;});Cyc_Absyn_new_exp(_tmpAF8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 515:
# 2745 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));({void*_tmpAFB=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->tag=0,({struct _fat_ptr*_tmpAFA=({struct _fat_ptr*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));({struct _fat_ptr _tmpAF9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5F3=_tmpAF9;});_tmp5F3;});_tmp5F4->f1=_tmpAFA;});_tmp5F4;});_tmp5F5->hd=_tmpAFB;}),_tmp5F5->tl=0;_tmp5F5;}));
goto _LL0;case 516:
# 2748
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({void*_tmpAFE=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=1,({unsigned _tmpAFD=({unsigned _tmpAFC=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpAFC,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp5F6->f1=_tmpAFD;});_tmp5F6;});_tmp5F7->hd=_tmpAFE;}),_tmp5F7->tl=0;_tmp5F7;}));
goto _LL0;case 517:
# 2750 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({void*_tmpB02=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));_tmp5F9->tag=0,({struct _fat_ptr*_tmpB01=({struct _fat_ptr*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));({struct _fat_ptr _tmpB00=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp5F8=_tmpB00;});_tmp5F8;});_tmp5F9->f1=_tmpB01;});_tmp5F9;});_tmp5FA->hd=_tmpB02;}),({struct Cyc_List_List*_tmpAFF=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp5FA->tl=_tmpAFF;});_tmp5FA;}));
goto _LL0;case 518:
# 2753
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));({void*_tmpB06=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->tag=1,({unsigned _tmpB05=({unsigned _tmpB04=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpB04,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp5FB->f1=_tmpB05;});_tmp5FB;});_tmp5FC->hd=_tmpB06;}),({struct Cyc_List_List*_tmpB03=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp5FC->tl=_tmpB03;});_tmp5FC;}));
goto _LL0;case 519:
# 2758 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpB07=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpB07,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520:
# 2759 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB08=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521:
# 2760 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 522:
# 2761 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB09=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpB09,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523:
# 2762 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB0A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpB0A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524:
# 2763 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 525:
# 2767 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB0B=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpB0B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526:
# 2770
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB0D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpB0C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpB0D,_tmpB0C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527:
# 2773
 yyval=Cyc_Exp_tok(({void*_tmpB11=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->tag=29,({struct _tuple0*_tmpB10=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp5FD->f1=_tmpB10;}),({struct Cyc_List_List*_tmpB0F=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp5FD->f2=_tmpB0F;}),({struct Cyc_List_List*_tmpB0E=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp5FD->f3=_tmpB0E;}),_tmp5FD->f4=0;_tmp5FD;});Cyc_Absyn_new_exp(_tmpB11,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528:
# 2775 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpB12=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpB12,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529:
# 2777 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpB13=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpB13,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 530:
# 2781 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 531:
# 2786 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));({struct Cyc_Absyn_Exp*_tmpB14=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp5FE->hd=_tmpB14;}),_tmp5FE->tl=0;_tmp5FE;}));
goto _LL0;case 532:
# 2788 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));({struct Cyc_Absyn_Exp*_tmpB16=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5FF->hd=_tmpB16;}),({struct Cyc_List_List*_tmpB15=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp5FF->tl=_tmpB15;});_tmp5FF;}));
goto _LL0;case 533:
# 2794 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpB17=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpB17,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534:
# 2795 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpB18=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpB18,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535:
# 2796 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB19=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpB19,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536:
# 2798 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 537:  {
# 2800 "parse.y"
struct _fat_ptr _tmp600=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp600;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 2808
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 2810
goto _LL0;}case 538:
# 2813 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp602=_cycalloc(sizeof(*_tmp602));({union Cyc_Absyn_Nmspace _tmpB1C=Cyc_Absyn_Rel_n(0);_tmp602->f1=_tmpB1C;}),({struct _fat_ptr*_tmpB1B=({struct _fat_ptr*_tmp601=_cycalloc(sizeof(*_tmp601));({struct _fat_ptr _tmpB1A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp601=_tmpB1A;});_tmp601;});_tmp602->f2=_tmpB1B;});_tmp602;}));
goto _LL0;case 539:
# 2814 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 540:
# 2817
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp604=_cycalloc(sizeof(*_tmp604));({union Cyc_Absyn_Nmspace _tmpB1F=Cyc_Absyn_Rel_n(0);_tmp604->f1=_tmpB1F;}),({struct _fat_ptr*_tmpB1E=({struct _fat_ptr*_tmp603=_cycalloc(sizeof(*_tmp603));({struct _fat_ptr _tmpB1D=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp603=_tmpB1D;});_tmp603;});_tmp604->f2=_tmpB1E;});_tmp604;}));
goto _LL0;case 541:
# 2818 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 542:
# 2823 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 543:
# 2824 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 544:
# 2827
 yyval=(yyyvsp[0]).v;
goto _LL0;case 545:
# 2828 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 546:
# 2833 "parse.y"
 goto _LL0;case 547:
# 2833 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default:
# 2837
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
({int _tmpB20=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpB20;});
({int _tmpB21=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_column=_tmpB21;});}
# 409
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr1,548U,sizeof(short),yyn));
# 411
yystate=({int _tmpB22=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,167U,sizeof(short),yyn - 153));_tmpB22 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 6508)&&({int _tmpB23=(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),yystate));_tmpB23 == (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));}))
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,6509U,sizeof(short),yystate));else{
# 415
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,167U,sizeof(short),yyn - 153));}
# 417
goto yynewstate;
# 419
yyerrlab:
# 421
 if(yyerrstatus == 0){
# 424
++ yynerrs;
# 427
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1067U,sizeof(short),yystate));
# 429
if(yyn > - 32768 && yyn < 6508){
# 431
int sze=0;
struct _fat_ptr msg;
int x;int count;
# 435
count=0;
# 437
for(x=yyn < 0?- yyn: 0;(unsigned)x < 320U / sizeof(char*);++ x){
# 439
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),x + yyn))== x)
({unsigned long _tmpB24=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpB24;}),count ++;}
msg=({unsigned _tmp606=(unsigned)(sze + 15)+ 1U;char*_tmp605=({struct _RegionHandle*_tmpB25=yyregion;_region_malloc(_tmpB25,_check_times(_tmp606,sizeof(char)));});({{unsigned _tmp6CC=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp6CC;++ i){_tmp605[i]='\000';}_tmp605[_tmp6CC]=0;}0;});_tag_fat(_tmp605,sizeof(char),_tmp606);});
({struct _fat_ptr _tmpB26=msg;Cyc_strcpy(_tmpB26,({const char*_tmp607="parse error";_tag_fat(_tmp607,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 320U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpB27=msg;Cyc_strcat(_tmpB27,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp608=", expecting `";_tag_fat(_tmp608,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp609=" or `";_tag_fat(_tmp609,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpB28=msg;Cyc_strcat(_tmpB28,({const char*_tmp60A="'";_tag_fat(_tmp60A,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpB2A=({const char*_tmp60B="parse error";_tag_fat(_tmp60B,sizeof(char),12U);});int _tmpB29=yystate;Cyc_yyerror(_tmpB2A,_tmpB29,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp60C=1;_npop_handler(0);return _tmp60C;}
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
 if(yyssp_offset == 0){int _tmp60D=1;_npop_handler(0);return _tmp60D;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_fat_subscript(yyss,sizeof(short),-- yyssp_offset));
# 521 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1067U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 525
yyn +=1;
if((yyn < 0 || yyn > 6508)||(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),yyn))!= 1)goto yyerrdefault;
# 528
yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yytable,6509U,sizeof(short),yyn));
if(yyn < 0){
# 531
if(yyn == -32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn == 0)goto yyerrpop;}
# 537
if(yyn == 1066){
int _tmp60E=0;_npop_handler(0);return _tmp60E;}
# 546
({struct Cyc_Yystacktype _tmpB2B=({struct Cyc_Yystacktype _tmp6CD;_tmp6CD.v=yylval,_tmp6CD.l=yylloc;_tmp6CD;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpB2B;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 2836 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp612=v;void*_tmp613;struct _fat_ptr _tmp614;char _tmp615;union Cyc_Absyn_Cnst _tmp616;switch((_tmp612.Stmt_tok).tag){case 1: _tmp616=(_tmp612.Int_tok).val;{union Cyc_Absyn_Cnst c=_tmp616;
({struct Cyc_String_pa_PrintArg_struct _tmp619=({struct Cyc_String_pa_PrintArg_struct _tmp6D0;_tmp6D0.tag=0,({struct _fat_ptr _tmpB2C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp6D0.f1=_tmpB2C;});_tmp6D0;});void*_tmp617[1];_tmp617[0]=& _tmp619;({struct Cyc___cycFILE*_tmpB2E=Cyc_stderr;struct _fat_ptr _tmpB2D=({const char*_tmp618="%s";_tag_fat(_tmp618,sizeof(char),3U);});Cyc_fprintf(_tmpB2E,_tmpB2D,_tag_fat(_tmp617,sizeof(void*),1));});});goto _LL0;}case 2: _tmp615=(_tmp612.Char_tok).val;{char c=_tmp615;
({struct Cyc_Int_pa_PrintArg_struct _tmp61C=({struct Cyc_Int_pa_PrintArg_struct _tmp6D1;_tmp6D1.tag=1,_tmp6D1.f1=(unsigned long)((int)c);_tmp6D1;});void*_tmp61A[1];_tmp61A[0]=& _tmp61C;({struct Cyc___cycFILE*_tmpB30=Cyc_stderr;struct _fat_ptr _tmpB2F=({const char*_tmp61B="%c";_tag_fat(_tmp61B,sizeof(char),3U);});Cyc_fprintf(_tmpB30,_tmpB2F,_tag_fat(_tmp61A,sizeof(void*),1));});});goto _LL0;}case 3: _tmp614=(_tmp612.String_tok).val;{struct _fat_ptr s=_tmp614;
({struct Cyc_String_pa_PrintArg_struct _tmp61F=({struct Cyc_String_pa_PrintArg_struct _tmp6D2;_tmp6D2.tag=0,_tmp6D2.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp6D2;});void*_tmp61D[1];_tmp61D[0]=& _tmp61F;({struct Cyc___cycFILE*_tmpB32=Cyc_stderr;struct _fat_ptr _tmpB31=({const char*_tmp61E="\"%s\"";_tag_fat(_tmp61E,sizeof(char),5U);});Cyc_fprintf(_tmpB32,_tmpB31,_tag_fat(_tmp61D,sizeof(void*),1));});});goto _LL0;}case 4: _tmp613=(_tmp612.QualId_tok).val;{struct _tuple0*q=_tmp613;
({struct Cyc_String_pa_PrintArg_struct _tmp622=({struct Cyc_String_pa_PrintArg_struct _tmp6D3;_tmp6D3.tag=0,({struct _fat_ptr _tmpB33=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp6D3.f1=_tmpB33;});_tmp6D3;});void*_tmp620[1];_tmp620[0]=& _tmp622;({struct Cyc___cycFILE*_tmpB35=Cyc_stderr;struct _fat_ptr _tmpB34=({const char*_tmp621="%s";_tag_fat(_tmp621,sizeof(char),3U);});Cyc_fprintf(_tmpB35,_tmpB34,_tag_fat(_tmp620,sizeof(void*),1));});});goto _LL0;}case 5: _tmp613=(_tmp612.Exp_tok).val;{struct Cyc_Absyn_Exp*e=_tmp613;
({struct Cyc_String_pa_PrintArg_struct _tmp625=({struct Cyc_String_pa_PrintArg_struct _tmp6D4;_tmp6D4.tag=0,({struct _fat_ptr _tmpB36=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp6D4.f1=_tmpB36;});_tmp6D4;});void*_tmp623[1];_tmp623[0]=& _tmp625;({struct Cyc___cycFILE*_tmpB38=Cyc_stderr;struct _fat_ptr _tmpB37=({const char*_tmp624="%s";_tag_fat(_tmp624,sizeof(char),3U);});Cyc_fprintf(_tmpB38,_tmpB37,_tag_fat(_tmp623,sizeof(void*),1));});});goto _LL0;}case 6: _tmp613=(_tmp612.Stmt_tok).val;{struct Cyc_Absyn_Stmt*s=_tmp613;
({struct Cyc_String_pa_PrintArg_struct _tmp628=({struct Cyc_String_pa_PrintArg_struct _tmp6D5;_tmp6D5.tag=0,({struct _fat_ptr _tmpB39=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp6D5.f1=_tmpB39;});_tmp6D5;});void*_tmp626[1];_tmp626[0]=& _tmp628;({struct Cyc___cycFILE*_tmpB3B=Cyc_stderr;struct _fat_ptr _tmpB3A=({const char*_tmp627="%s";_tag_fat(_tmp627,sizeof(char),3U);});Cyc_fprintf(_tmpB3B,_tmpB3A,_tag_fat(_tmp626,sizeof(void*),1));});});goto _LL0;}default:
({void*_tmp629=0U;({struct Cyc___cycFILE*_tmpB3D=Cyc_stderr;struct _fat_ptr _tmpB3C=({const char*_tmp62A="?";_tag_fat(_tmp62A,sizeof(char),2U);});Cyc_fprintf(_tmpB3D,_tmpB3C,_tag_fat(_tmp629,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 2848
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp62B="end-of-file";_tag_fat(_tmp62B,sizeof(char),12U);});
if(token == 368)
return Cyc_Lex_token_string;
if(token == 377)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 380?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,381U,sizeof(short),token)): 320;
if((unsigned)z < 320U)
return*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),z));
return _tag_fat(0,0,0);}}
# 2862
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp62C=_new_region("yyr");struct _RegionHandle*yyr=& _tmp62C;_push_region(yyr);
({struct _RegionHandle*_tmpB3E=yyr;Cyc_yyparse(_tmpB3E,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp62D=Cyc_Parse_parse_result;_npop_handler(0);return _tmp62D;}
# 2865
;_pop_region();}}
