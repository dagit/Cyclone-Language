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
extern struct _fat_ptr Cyc_strcpy(struct _fat_ptr,struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 90 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 153
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 174
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 179
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 184
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 287
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 300
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 307
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 410 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 480
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 487
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 505
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 673 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 835
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 865
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 869
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Tqual);
# 875
void*Cyc_Absyn_compress(void*);
# 889
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 891
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 894
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 896
extern void*Cyc_Absyn_sint_type;
# 898
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 901
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 905
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 907
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_tag_type(void*);extern void*Cyc_Absyn_rgn_handle_type(void*);extern void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple0*,struct Cyc_Absyn_Enumdecl*);extern void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);extern void*Cyc_Absyn_builtin_type(struct _fat_ptr,struct Cyc_Absyn_Kind*);extern void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 932
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 936
void*Cyc_Absyn_bounds_one (void);
# 938
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 958
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 961
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 966
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 969
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 972
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);
# 976
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _fat_ptr,int,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _fat_ptr,unsigned);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 993
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1010
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
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1038
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
# 1043
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Stmt*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*,unsigned);
# 1054
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _fat_ptr*,unsigned);
# 1057
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned);
# 1060
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1064
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
# 1069
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple0*,void*,struct Cyc_Absyn_Exp*);
# 1071
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,int);
# 1078
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind,enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_AggrdeclImpl*,struct Cyc_List_List*,unsigned);
# 1085
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1088
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope,struct _tuple0*,struct Cyc_List_List*,struct Cyc_Core_Opt*,int,unsigned);
# 1093
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
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64F;_tmp64F.tag=0,_tmp64F.f1=msg;_tmp64F;});void*_tmp0[1];_tmp0[0]=& _tmp1;({unsigned _tmp6F7=loc;Cyc_Warn_err2(_tmp6F7,_tag_fat(_tmp0,sizeof(void*),1));});});
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_Parse_Exit;_tmp2;}));}
# 179
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned loc){
# 181
struct _tuple8*_tmp3=tqt;void*_tmp5;struct Cyc_Absyn_Tqual _tmp4;_LL1: _tmp4=_tmp3->f2;_tmp5=_tmp3->f3;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp4;void*t=_tmp5;
if((tq.print_const || tq.q_volatile)|| tq.q_restrict){
if(tq.loc != (unsigned)0)loc=tq.loc;
({void*_tmp6=0U;({unsigned _tmp6F9=loc;struct _fat_ptr _tmp6F8=({const char*_tmp7="qualifier on type is ignored";_tag_fat(_tmp7,sizeof(char),29U);});Cyc_Warn_warn(_tmp6F9,_tmp6F8,_tag_fat(_tmp6,sizeof(void*),0));});});}
# 186
return t;}}struct _tuple15{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 189
static struct _tuple15 Cyc_Parse_collapse_pointer_quals(unsigned loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 195
void*zeroterm=Cyc_Tcutil_any_bool(0);
void*released=Cyc_Tcutil_any_bool(0);
for(1;pqs != 0;pqs=pqs->tl){
void*_tmp8=(void*)pqs->hd;void*_stmttmp0=_tmp8;void*_tmp9=_stmttmp0;void*_tmpA;struct Cyc_Absyn_Exp*_tmpB;switch(*((int*)_tmp9)){case 4: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 6: _LL5: _LL6:
 released=Cyc_Absyn_true_type;goto _LL0;case 8: _LL7: _LL8:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 7: _LL9: _LLA:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3: _LLB: _LLC:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2: _LLD: _LLE:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0: _LLF: _tmpB=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmp9)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_tmpB;
bound=Cyc_Absyn_thin_bounds_exp(e);goto _LL0;}default: _LL11: _tmpA=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmp9)->f1;_LL12: {void*t=_tmpA;
rgn=t;goto _LL0;}}_LL0:;}
# 209
return({struct _tuple15 _tmp650;_tmp650.f1=nullable,_tmp650.f2=bound,_tmp650.f3=zeroterm,_tmp650.f4=rgn,_tmp650.f5=released;_tmp650;});}
# 215
static struct _tuple0*Cyc_Parse_gensym_enum (void){
# 217
static int enum_counter=0;
return({struct _tuple0*_tmp10=_cycalloc(sizeof(*_tmp10));({union Cyc_Absyn_Nmspace _tmp6FD=Cyc_Absyn_Rel_n(0);_tmp10->f1=_tmp6FD;}),({
struct _fat_ptr*_tmp6FC=({struct _fat_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _fat_ptr _tmp6FB=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE=({struct Cyc_Int_pa_PrintArg_struct _tmp651;_tmp651.tag=1,_tmp651.f1=(unsigned long)enum_counter ++;_tmp651;});void*_tmpC[1];_tmpC[0]=& _tmpE;({struct _fat_ptr _tmp6FA=({const char*_tmpD="__anonymous_enum_%d__";_tag_fat(_tmpD,sizeof(char),22U);});Cyc_aprintf(_tmp6FA,_tag_fat(_tmpC,sizeof(void*),1));});});*_tmpF=_tmp6FB;});_tmpF;});_tmp10->f2=_tmp6FC;});_tmp10;});}struct _tuple16{unsigned f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple17{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple18{struct _tuple16*f1;struct _tuple17*f2;};
# 222
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned loc,struct _tuple18*field_info){
# 227
struct _tuple18*_tmp11=field_info;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_List_List*_tmp17;struct Cyc_List_List*_tmp16;void*_tmp15;struct Cyc_Absyn_Tqual _tmp14;struct _tuple0*_tmp13;unsigned _tmp12;_LL1: _tmp12=(_tmp11->f1)->f1;_tmp13=(_tmp11->f1)->f2;_tmp14=(_tmp11->f1)->f3;_tmp15=(_tmp11->f1)->f4;_tmp16=(_tmp11->f1)->f5;_tmp17=(_tmp11->f1)->f6;_tmp18=(_tmp11->f2)->f1;_tmp19=(_tmp11->f2)->f2;_LL2: {unsigned varloc=_tmp12;struct _tuple0*qid=_tmp13;struct Cyc_Absyn_Tqual tq=_tmp14;void*t=_tmp15;struct Cyc_List_List*tvs=_tmp16;struct Cyc_List_List*atts=_tmp17;struct Cyc_Absyn_Exp*widthopt=_tmp18;struct Cyc_Absyn_Exp*reqopt=_tmp19;
if(tvs != 0)
({void*_tmp1A=0U;({unsigned _tmp6FF=loc;struct _fat_ptr _tmp6FE=({const char*_tmp1B="bad type params in struct field";_tag_fat(_tmp1B,sizeof(char),32U);});Cyc_Warn_err(_tmp6FF,_tmp6FE,_tag_fat(_tmp1A,sizeof(void*),0));});});
if(Cyc_Absyn_is_qvar_qualified(qid))
({void*_tmp1C=0U;({unsigned _tmp701=loc;struct _fat_ptr _tmp700=({const char*_tmp1D="struct or union field cannot be qualified with a namespace";_tag_fat(_tmp1D,sizeof(char),59U);});Cyc_Warn_err(_tmp701,_tmp700,_tag_fat(_tmp1C,sizeof(void*),0));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E->name=(*qid).f2,_tmp1E->tq=tq,_tmp1E->type=t,_tmp1E->width=widthopt,_tmp1E->attributes=atts,_tmp1E->requires_clause=reqopt;_tmp1E;});}}
# 237
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned loc){
return({struct Cyc_Parse_Type_specifier _tmp652;_tmp652.Signed_spec=0,_tmp652.Unsigned_spec=0,_tmp652.Short_spec=0,_tmp652.Long_spec=0,_tmp652.Long_Long_spec=0,_tmp652.Valid_type_spec=0,_tmp652.Type_spec=Cyc_Absyn_sint_type,_tmp652.loc=loc;_tmp652;});}
# 248
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned loc){
struct Cyc_Parse_Type_specifier _tmp1F=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp1F;
s.Type_spec=t;
s.Valid_type_spec=1;
return s;}
# 254
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp20=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp20;
s.Signed_spec=1;
return s;}
# 259
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp21=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp21;
s.Unsigned_spec=1;
return s;}
# 264
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp22=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp22;
s.Short_spec=1;
return s;}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);struct Cyc_Parse_Type_specifier s=_tmp23;
s.Long_spec=1;
return s;}
# 276
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 278
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp702=t;Cyc_Tcutil_promote_array(_tmp702,argposn?Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple19{struct _fat_ptr*f1;void*f2;};
# 291 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp24=0;struct Cyc_List_List*res=_tmp24;
for(1;x != 0;x=x->tl){
struct _tuple8*_tmp25=(struct _tuple8*)x->hd;struct _tuple8*_stmttmp1=_tmp25;struct _tuple8*_tmp26=_stmttmp1;void**_tmp28;struct _fat_ptr _tmp27;void*_tmp2A;struct _fat_ptr*_tmp29;if(*((int*)((struct _tuple8*)_tmp26)->f3)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp26)->f3)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp26)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp26)->f3)->f2)->tl == 0){_LL1: _tmp29=_tmp26->f1;_tmp2A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26->f3)->f2)->hd;if(_tmp29 != 0){_LL2: {struct _fat_ptr*v=_tmp29;void*i=_tmp2A;
# 296
{void*_tmp2B=i;void**_tmp2C;if(*((int*)_tmp2B)== 1){_LL8: _tmp2C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2;_LL9: {void**z=_tmp2C;
# 300
struct _fat_ptr*nm=({struct _fat_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));({struct _fat_ptr _tmp704=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp653;_tmp653.tag=0,_tmp653.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp653;});void*_tmp2F[1];_tmp2F[0]=& _tmp31;({struct _fat_ptr _tmp703=({const char*_tmp30="`%s";_tag_fat(_tmp30,sizeof(char),4U);});Cyc_aprintf(_tmp703,_tag_fat(_tmp2F,sizeof(void*),1));});});*_tmp32=_tmp704;});_tmp32;});
({void*_tmp706=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->name=nm,_tmp2E->identity=- 1,({void*_tmp705=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=0,_tmp2D->f1=& Cyc_Kinds_ik;_tmp2D;});_tmp2E->kind=_tmp705;});_tmp2E;}));*z=_tmp706;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 305
res=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));({struct _tuple19*_tmp707=({struct _tuple19*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=v,_tmp33->f2=i;_tmp33;});_tmp34->hd=_tmp707;}),_tmp34->tl=res;_tmp34;});goto _LL0;}}else{if(((struct _tuple8*)_tmp26)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp26)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp26)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp26)->f1 != 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp26)->f3)->f1)== 3){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp26)->f3)->f2 != 0){if(*((int*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp26)->f3)->f2)->hd)== 1){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp26)->f3)->f2)->tl == 0){_LL3: _tmp27=*_tmp26->f1;_tmp28=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp26->f3)->f2)->hd)->f2;_LL4: {struct _fat_ptr v=_tmp27;void**z=_tmp28;
# 309
struct _fat_ptr*nm=({struct _fat_ptr*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct _fat_ptr _tmp709=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp654;_tmp654.tag=0,_tmp654.f1=(struct _fat_ptr)((struct _fat_ptr)v);_tmp654;});void*_tmp37[1];_tmp37[0]=& _tmp39;({struct _fat_ptr _tmp708=({const char*_tmp38="`%s";_tag_fat(_tmp38,sizeof(char),4U);});Cyc_aprintf(_tmp708,_tag_fat(_tmp37,sizeof(void*),1));});});*_tmp3A=_tmp709;});_tmp3A;});
({void*_tmp70B=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->name=nm,_tmp36->identity=- 1,({void*_tmp70A=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=0,_tmp35->f1=& Cyc_Kinds_rk;_tmp35;});_tmp36->kind=_tmp70A;});_tmp36;}));*z=_tmp70B;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp26)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 314
return res;}
# 318
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3B=0;struct Cyc_List_List*res=_tmp3B;
for(1;x != 0;x=x->tl){
void*_tmp3C=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_stmttmp2=_tmp3C;void*_tmp3D=_stmttmp2;void*_tmp3E;if(*((int*)_tmp3D)== 0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f1)== 4){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f2)->tl == 0){_LL1: _tmp3E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D)->f2)->hd;_LL2: {void*i=_tmp3E;
# 323
res=({struct Cyc_List_List*_tmp40=_cycalloc(sizeof(*_tmp40));({struct _tuple19*_tmp70C=({struct _tuple19*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp3F->f2=i;_tmp3F;});_tmp40->hd=_tmp70C;}),_tmp40->tl=res;_tmp40;});goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 326
return res;}
# 330
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp41=e->r;void*_stmttmp3=_tmp41;void*_tmp42=_stmttmp3;struct _fat_ptr*_tmp43;if(*((int*)_tmp42)== 1){if(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42)->f1)== 0){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp43=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp42)->f1)->f1)->f2;_LL2: {struct _fat_ptr*y=_tmp43;
# 333
{struct Cyc_List_List*_tmp44=tags;struct Cyc_List_List*ts=_tmp44;for(0;ts != 0;ts=ts->tl){
struct _tuple19*_tmp45=(struct _tuple19*)ts->hd;struct _tuple19*_stmttmp4=_tmp45;struct _tuple19*_tmp46=_stmttmp4;void*_tmp48;struct _fat_ptr*_tmp47;_LL6: _tmp47=_tmp46->f1;_tmp48=_tmp46->f2;_LL7: {struct _fat_ptr*x=_tmp47;void*i=_tmp48;
if(Cyc_strptrcmp(x,y)== 0)
return({void*_tmp70E=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->tag=39,({void*_tmp70D=Cyc_Tcutil_copy_type(i);_tmp49->f1=_tmp70D;});_tmp49;});Cyc_Absyn_new_exp(_tmp70E,e->loc);});}}}
# 338
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 341
return e;}
# 346
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4A=t;struct Cyc_Absyn_Exp*_tmp4B;void*_tmp4C;void*_tmp54;struct Cyc_Absyn_PtrLoc*_tmp53;void*_tmp52;void*_tmp51;void*_tmp50;void*_tmp4F;struct Cyc_Absyn_Tqual _tmp4E;void*_tmp4D;unsigned _tmp59;void*_tmp58;struct Cyc_Absyn_Exp*_tmp57;struct Cyc_Absyn_Tqual _tmp56;void*_tmp55;switch(*((int*)_tmp4A)){case 4: _LL1: _tmp55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).elt_type;_tmp56=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).tq;_tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).num_elts;_tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).zero_term;_tmp59=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4A)->f1).zt_loc;_LL2: {void*et=_tmp55;struct Cyc_Absyn_Tqual tq=_tmp56;struct Cyc_Absyn_Exp*nelts=_tmp57;void*zt=_tmp58;unsigned ztloc=_tmp59;
# 349
struct Cyc_Absyn_Exp*nelts2=nelts;
if(nelts != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,nelts);{
void*_tmp5A=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5A;
if(nelts != nelts2 || et != et2)
return Cyc_Absyn_array_type(et2,tq,nelts2,zt,ztloc);
goto _LL0;}}case 3: _LL3: _tmp4D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).elt_type;_tmp4E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).elt_tq;_tmp4F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).ptr_atts).rgn;_tmp50=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).ptr_atts).nullable;_tmp51=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).ptr_atts).bounds;_tmp52=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).ptr_atts).zero_term;_tmp53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).ptr_atts).ptrloc;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4A)->f1).ptr_atts).released;_LL4: {void*et=_tmp4D;struct Cyc_Absyn_Tqual tq=_tmp4E;void*r=_tmp4F;void*n=_tmp50;void*b=_tmp51;void*zt=_tmp52;struct Cyc_Absyn_PtrLoc*pl=_tmp53;void*rel=_tmp54;
# 357
void*_tmp5B=Cyc_Parse_substitute_tags(tags,et);void*et2=_tmp5B;
void*_tmp5C=Cyc_Parse_substitute_tags(tags,b);void*b2=_tmp5C;
if(et2 != et || b2 != b)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp655;_tmp655.elt_type=et2,_tmp655.elt_tq=tq,(_tmp655.ptr_atts).rgn=r,(_tmp655.ptr_atts).nullable=n,(_tmp655.ptr_atts).bounds=b2,(_tmp655.ptr_atts).zero_term=zt,(_tmp655.ptr_atts).ptrloc=pl,(_tmp655.ptr_atts).released=rel;_tmp655;}));
goto _LL0;}case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f1)== 13){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2)->tl == 0){_LL5: _tmp4C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A)->f2)->hd;_LL6: {void*t=_tmp4C;
# 363
void*_tmp5D=Cyc_Parse_substitute_tags(tags,t);void*t2=_tmp5D;
if(t != t2)return Cyc_Absyn_thin_bounds_type(t2);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9: _LL7: _tmp4B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4A)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp4B;
# 367
struct Cyc_Absyn_Exp*_tmp5E=Cyc_Parse_substitute_tags_exp(tags,e);struct Cyc_Absyn_Exp*e2=_tmp5E;
if(e2 != e)return Cyc_Absyn_valueof_type(e2);
goto _LL0;}default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 372
return t;}
# 377
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp70F=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp70F;});}struct _tuple20{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple21{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 384
static struct _tuple20*Cyc_Parse_get_tqual_typ(unsigned loc,struct _tuple21*t){
return({struct _tuple20*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->f1=(*t).f2,_tmp5F->f2=(*t).f3;_tmp5F;});}
# 388
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp60=tm;if(*((int*)_tmp60)== 4){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 397
static void*Cyc_Parse_id2type(struct _fat_ptr s,void*k){
if(({struct _fat_ptr _tmp710=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp710,({const char*_tmp61="`H";_tag_fat(_tmp61,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;
if(({struct _fat_ptr _tmp711=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp711,({const char*_tmp62="`U";_tag_fat(_tmp62,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;
if(({struct _fat_ptr _tmp712=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp712,({const char*_tmp63="`RC";_tag_fat(_tmp63,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
return Cyc_CurRgn_curr_rgn_type();
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp65=_cycalloc(sizeof(*_tmp65));({struct _fat_ptr*_tmp713=({struct _fat_ptr*_tmp64=_cycalloc(sizeof(*_tmp64));*_tmp64=s;_tmp64;});_tmp65->name=_tmp713;}),_tmp65->identity=- 1,_tmp65->kind=k;_tmp65;}));}
# 409
static void Cyc_Parse_tvar_ok(struct _fat_ptr s,unsigned loc){
if(({struct _fat_ptr _tmp714=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp714,({const char*_tmp66="`H";_tag_fat(_tmp66,sizeof(char),3U);}));})== 0)
({void*_tmp67=0U;({unsigned _tmp716=loc;struct _fat_ptr _tmp715=({const char*_tmp68="bad occurrence of heap region";_tag_fat(_tmp68,sizeof(char),30U);});Cyc_Warn_err(_tmp716,_tmp715,_tag_fat(_tmp67,sizeof(void*),0));});});
if(({struct _fat_ptr _tmp717=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp717,({const char*_tmp69="`U";_tag_fat(_tmp69,sizeof(char),3U);}));})== 0)
({void*_tmp6A=0U;({unsigned _tmp719=loc;struct _fat_ptr _tmp718=({const char*_tmp6B="bad occurrence of unique region";_tag_fat(_tmp6B,sizeof(char),32U);});Cyc_Warn_err(_tmp719,_tmp718,_tag_fat(_tmp6A,sizeof(void*),0));});});
if(({struct _fat_ptr _tmp71A=(struct _fat_ptr)s;Cyc_zstrcmp(_tmp71A,({const char*_tmp6C="`RC";_tag_fat(_tmp6C,sizeof(char),4U);}));})== 0)
({void*_tmp6D=0U;({unsigned _tmp71C=loc;struct _fat_ptr _tmp71B=({const char*_tmp6E="bad occurrence of refcounted region";_tag_fat(_tmp6E,sizeof(char),36U);});Cyc_Warn_err(_tmp71C,_tmp71B,_tag_fat(_tmp6D,sizeof(void*),0));});});
if(Cyc_zstrcmp((struct _fat_ptr)s,(struct _fat_ptr)Cyc_CurRgn_curr_rgn_name)== 0)
({void*_tmp6F=0U;({unsigned _tmp71E=loc;struct _fat_ptr _tmp71D=({const char*_tmp70="bad occurrence of \"current\" region";_tag_fat(_tmp70,sizeof(char),35U);});Cyc_Warn_err(_tmp71E,_tmp71D,_tag_fat(_tmp6F,sizeof(void*),0));});});}
# 424
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned loc,void*t){
void*_tmp71=t;struct Cyc_Absyn_Tvar*_tmp72;if(*((int*)_tmp71)== 2){_LL1: _tmp72=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp71)->f1;_LL2: {struct Cyc_Absyn_Tvar*pr=_tmp72;
return pr;}}else{_LL3: _LL4:
({unsigned _tmp71F=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp71F,({const char*_tmp73="expecting a list of type variables, not types";_tag_fat(_tmp73,sizeof(char),46U);}));});}_LL0:;}
# 432
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp74=Cyc_Absyn_compress(t);void*_stmttmp5=_tmp74;void*_tmp75=_stmttmp5;void**_tmp76;if(*((int*)_tmp75)== 2){_LL1: _tmp76=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1)->kind;_LL2: {void**cptr=_tmp76;
# 435
void*_tmp77=Cyc_Kinds_compress_kb(*cptr);void*_stmttmp6=_tmp77;void*_tmp78=_stmttmp6;if(*((int*)_tmp78)== 1){_LL6: _LL7:
# 437
({void*_tmp720=leq?(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=2,_tmp79->f1=0,_tmp79->f2=k;_tmp79;}): Cyc_Kinds_kind_to_bound(k);*cptr=_tmp720;});
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 441
 return;}_LL0:;}
# 446
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned loc){
# 454
if(tms == 0)return 0;{
# 456
void*_tmp7A=(void*)tms->hd;void*_stmttmp7=_tmp7A;void*_tmp7B=_stmttmp7;void*_tmp7C;if(*((int*)_tmp7B)== 3){_LL1: _tmp7C=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp7B)->f1;_LL2: {void*args=_tmp7C;
# 459
if(tms->tl == 0 ||
 Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd)&&((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 462
void*_tmp7D=args;struct Cyc_List_List*_tmp7E;if(*((int*)_tmp7D)== 1){_LL6: _LL7:
# 464
({void*_tmp7F=0U;({unsigned _tmp722=loc;struct _fat_ptr _tmp721=({const char*_tmp80="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_fat(_tmp80,sizeof(char),93U);});Cyc_Warn_warn(_tmp722,_tmp721,_tag_fat(_tmp7F,sizeof(void*),0));});});
# 466
return tms;}else{_LL8: _tmp7E=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp7D)->f1;_LL9: {struct Cyc_List_List*ids=_tmp7E;
# 468
if(({int _tmp723=((int(*)(struct Cyc_List_List*))Cyc_List_length)(ids);_tmp723 != ((int(*)(struct Cyc_List_List*))Cyc_List_length)(tds);}))
({unsigned _tmp724=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp724,({const char*_tmp81="wrong number of parameter declarations in old-style function declaration";_tag_fat(_tmp81,sizeof(char),73U);}));});{
# 472
struct Cyc_List_List*rev_new_params=0;
for(1;ids != 0;ids=ids->tl){
struct Cyc_List_List*tds2=tds;
for(1;tds2 != 0;tds2=tds2->tl){
struct Cyc_Absyn_Decl*_tmp82=(struct Cyc_Absyn_Decl*)tds2->hd;struct Cyc_Absyn_Decl*x=_tmp82;
void*_tmp83=x->r;void*_stmttmp8=_tmp83;void*_tmp84=_stmttmp8;struct Cyc_Absyn_Vardecl*_tmp85;if(*((int*)_tmp84)== 0){_LLB: _tmp85=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp84)->f1;_LLC: {struct Cyc_Absyn_Vardecl*vd=_tmp85;
# 479
if(Cyc_zstrptrcmp((*vd->name).f2,(struct _fat_ptr*)ids->hd)!= 0)
continue;
if(vd->initializer != 0)
({unsigned _tmp725=x->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp725,({const char*_tmp86="initializer found in parameter declaration";_tag_fat(_tmp86,sizeof(char),43U);}));});
if(Cyc_Absyn_is_qvar_qualified(vd->name))
({unsigned _tmp726=x->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp726,({const char*_tmp87="namespaces forbidden in parameter declarations";_tag_fat(_tmp87,sizeof(char),47U);}));});
rev_new_params=({struct Cyc_List_List*_tmp89=_cycalloc(sizeof(*_tmp89));({struct _tuple8*_tmp727=({struct _tuple8*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=(*vd->name).f2,_tmp88->f2=vd->tq,_tmp88->f3=vd->type;_tmp88;});_tmp89->hd=_tmp727;}),_tmp89->tl=rev_new_params;_tmp89;});
# 487
goto L;}}else{_LLD: _LLE:
({unsigned _tmp728=x->loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp728,({const char*_tmp8A="nonvariable declaration in parameter type";_tag_fat(_tmp8A,sizeof(char),42U);}));});}_LLA:;}
# 491
L: if(tds2 == 0)
({unsigned _tmp72A=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp72A,(struct _fat_ptr)({struct _fat_ptr _tmp729=(struct _fat_ptr)*((struct _fat_ptr*)ids->hd);Cyc_strconcat(_tmp729,({const char*_tmp8B=" is not given a type";_tag_fat(_tmp8B,sizeof(char),21U);}));}));});}
# 494
return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));
({void*_tmp72D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8D=_region_malloc(yy,sizeof(*_tmp8D));_tmp8D->tag=3,({void*_tmp72C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));_tmp8C->tag=1,({struct Cyc_List_List*_tmp72B=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(rev_new_params);_tmp8C->f1=_tmp72B;}),_tmp8C->f2=0,_tmp8C->f3=0,_tmp8C->f4=0,_tmp8C->f5=0,_tmp8C->f6=0,_tmp8C->f7=0;_tmp8C;});_tmp8D->f1=_tmp72C;});_tmp8D;});_tmp8E->hd=_tmp72D;}),_tmp8E->tl=0;_tmp8E;});}}}_LL5:;}
# 501
goto _LL4;}}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8F=_region_malloc(yy,sizeof(*_tmp8F));_tmp8F->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp72E=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8F->tl=_tmp72E;});_tmp8F;});}_LL0:;}}
# 509
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned loc){
# 513
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp656;_tmp656.id=d.id,_tmp656.varloc=d.varloc,({struct Cyc_List_List*_tmp72F=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp656.tms=_tmp72F;});_tmp656;});{
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
enum Cyc_Parse_Storage_class _tmp90=dso->sc;enum Cyc_Parse_Storage_class _stmttmp9=_tmp90;enum Cyc_Parse_Storage_class _tmp91=_stmttmp9;switch(_tmp91){case Cyc_Parse_None_sc: _LL1: _LL2:
 goto _LL0;case Cyc_Parse_Extern_sc: _LL3: _LL4:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL5: _LL6:
 sc=0U;goto _LL0;default: _LL7: _LL8:
({void*_tmp92=0U;({unsigned _tmp731=loc;struct _fat_ptr _tmp730=({const char*_tmp93="bad storage class on function";_tag_fat(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp731,_tmp730,_tag_fat(_tmp92,sizeof(void*),0));});});goto _LL0;}_LL0:;}}{
# 535
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);void*t=_tmp94;
struct _tuple14 _tmp95=Cyc_Parse_apply_tms(tq,t,atts,d.tms);struct _tuple14 _stmttmpA=_tmp95;struct _tuple14 _tmp96=_stmttmpA;struct Cyc_List_List*_tmp9A;struct Cyc_List_List*_tmp99;void*_tmp98;struct Cyc_Absyn_Tqual _tmp97;_LLA: _tmp97=_tmp96.f1;_tmp98=_tmp96.f2;_tmp99=_tmp96.f3;_tmp9A=_tmp96.f4;_LLB: {struct Cyc_Absyn_Tqual fn_tqual=_tmp97;void*fn_type=_tmp98;struct Cyc_List_List*x=_tmp99;struct Cyc_List_List*out_atts=_tmp9A;
# 540
if(x != 0)
({void*_tmp9B=0U;({unsigned _tmp733=loc;struct _fat_ptr _tmp732=({const char*_tmp9C="bad type params, ignoring";_tag_fat(_tmp9C,sizeof(char),26U);});Cyc_Warn_warn(_tmp733,_tmp732,_tag_fat(_tmp9B,sizeof(void*),0));});});{
# 543
void*_tmp9D=fn_type;struct Cyc_Absyn_FnInfo _tmp9E;if(*((int*)_tmp9D)== 5){_LLD: _tmp9E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9D)->f1;_LLE: {struct Cyc_Absyn_FnInfo i=_tmp9E;
# 545
{struct Cyc_List_List*_tmp9F=i.args;struct Cyc_List_List*args2=_tmp9F;for(0;args2 != 0;args2=args2->tl){
if((*((struct _tuple8*)args2->hd)).f1 == 0){
({void*_tmpA0=0U;({unsigned _tmp735=loc;struct _fat_ptr _tmp734=({const char*_tmpA1="missing argument variable in function prototype";_tag_fat(_tmpA1,sizeof(char),48U);});Cyc_Warn_err(_tmp735,_tmp734,_tag_fat(_tmpA0,sizeof(void*),0));});});
({struct _fat_ptr*_tmp737=({struct _fat_ptr*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct _fat_ptr _tmp736=({const char*_tmpA2="?";_tag_fat(_tmpA2,sizeof(char),2U);});*_tmpA3=_tmp736;});_tmpA3;});(*((struct _tuple8*)args2->hd)).f1=_tmp737;});}}}
# 552
({struct Cyc_List_List*_tmp738=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(i.attributes,out_atts);i.attributes=_tmp738;});
return({struct Cyc_Absyn_Fndecl*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->sc=sc,_tmpA4->is_inline=is_inline,_tmpA4->name=d.id,_tmpA4->body=body,_tmpA4->i=i,_tmpA4->cached_type=0,_tmpA4->param_vardecls=0,_tmpA4->fn_vardecl=0,_tmpA4->orig_scope=sc;_tmpA4;});}}else{_LLF: _LL10:
# 556
({unsigned _tmp739=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp739,({const char*_tmpA5="declarator is not a function prototype";_tag_fat(_tmpA5,sizeof(char),39U);}));});}_LLC:;}}}}}static char _tmpA6[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 560
static struct _fat_ptr Cyc_Parse_msg1={_tmpA6,_tmpA6,_tmpA6 + 76U};static char _tmpA7[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 562
static struct _fat_ptr Cyc_Parse_msg2={_tmpA7,_tmpA7,_tmpA7 + 84U};
# 569
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 572
if(s1.Signed_spec && s2.Signed_spec)
({void*_tmpA8=0U;({unsigned _tmp73B=loc;struct _fat_ptr _tmp73A=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp73B,_tmp73A,_tag_fat(_tmpA8,sizeof(void*),0));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec && s2.Unsigned_spec)
({void*_tmpA9=0U;({unsigned _tmp73D=loc;struct _fat_ptr _tmp73C=Cyc_Parse_msg2;Cyc_Warn_warn(_tmp73D,_tmp73C,_tag_fat(_tmpA9,sizeof(void*),0));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec && s2.Short_spec)
({void*_tmpAA=0U;({unsigned _tmp73F=loc;struct _fat_ptr _tmp73E=({const char*_tmpAB="too many occurrences of short in specifiers";_tag_fat(_tmpAB,sizeof(char),44U);});Cyc_Warn_warn(_tmp73F,_tmp73E,_tag_fat(_tmpAA,sizeof(void*),0));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec && s2.Long_Long_spec ||
 s1.Long_Long_spec && s2.Long_spec)||
 s2.Long_Long_spec && s1.Long_spec)
({void*_tmpAC=0U;({unsigned _tmp741=loc;struct _fat_ptr _tmp740=({const char*_tmpAD="too many occurrences of long in specifiers";_tag_fat(_tmpAD,sizeof(char),43U);});Cyc_Warn_warn(_tmp741,_tmp740,_tag_fat(_tmpAC,sizeof(void*),0));});});
s1.Long_Long_spec=
(s1.Long_Long_spec || s2.Long_Long_spec)|| s1.Long_spec && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec &&(s1.Long_spec || s2.Long_spec);
if(s1.Valid_type_spec && s2.Valid_type_spec)
({void*_tmpAE=0U;({unsigned _tmp743=loc;struct _fat_ptr _tmp742=Cyc_Parse_msg1;Cyc_Warn_err(_tmp743,_tmp742,_tag_fat(_tmpAE,sizeof(void*),0));});});else{
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
({void*_tmpAF=0U;({unsigned _tmp745=loc;struct _fat_ptr _tmp744=Cyc_Parse_msg2;Cyc_Warn_err(_tmp745,_tmp744,_tag_fat(_tmpAF,sizeof(void*),0));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec &&(ts.Long_spec || ts.Long_Long_spec)||
 ts.Long_spec && ts.Long_Long_spec)
({void*_tmpB0=0U;({unsigned _tmp747=loc;struct _fat_ptr _tmp746=Cyc_Parse_msg2;Cyc_Warn_err(_tmp747,_tmp746,_tag_fat(_tmpB0,sizeof(void*),0));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;}
# 621
if(!seen_type){
if(!seen_sign && !seen_size)
({void*_tmpB1=0U;({unsigned _tmp749=loc;struct _fat_ptr _tmp748=({const char*_tmpB2="missing type within specifier";_tag_fat(_tmpB2,sizeof(char),30U);});Cyc_Warn_warn(_tmp749,_tmp748,_tag_fat(_tmpB1,sizeof(void*),0));});});
return Cyc_Absyn_int_type(sgn,sz);}
# 626
{void*_tmpB3=t;enum Cyc_Absyn_Size_of _tmpB5;enum Cyc_Absyn_Sign _tmpB4;if(*((int*)_tmpB3)== 0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB3)->f1)){case 1: _LL1: _tmpB4=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB3)->f1)->f1;_tmpB5=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB3)->f1)->f2;_LL2: {enum Cyc_Absyn_Sign sgn2=_tmpB4;enum Cyc_Absyn_Size_of sz2=_tmpB5;
# 628
if(seen_sign &&(int)sgn2 != (int)sgn){
sgn2=sgn;
t=Cyc_Absyn_int_type(sgn,sz2);}
# 632
if(seen_size &&(int)sz2 != (int)sz)
t=Cyc_Absyn_int_type(sgn2,sz);
goto _LL0;}case 2: _LL3: _LL4:
# 636
 if(seen_size)
t=Cyc_Absyn_long_double_type;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
# 640
 if(seen_sign)
({void*_tmpB6=0U;({unsigned _tmp74B=loc;struct _fat_ptr _tmp74A=({const char*_tmpB7="sign specification on non-integral type";_tag_fat(_tmpB7,sizeof(char),40U);});Cyc_Warn_err(_tmp74B,_tmp74A,_tag_fat(_tmpB6,sizeof(void*),0));});});
if(seen_size)
({void*_tmpB8=0U;({unsigned _tmp74D=loc;struct _fat_ptr _tmp74C=({const char*_tmpB9="size qualifier on non-integral type";_tag_fat(_tmpB9,sizeof(char),36U);});Cyc_Warn_err(_tmp74D,_tmp74C,_tag_fat(_tmpB8,sizeof(void*),0));});});
goto _LL0;}_LL0:;}
# 646
return t;}
# 649
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple13*ds,struct Cyc_List_List*shared_atts){
# 653
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpBA=d.id;struct _tuple0*q=_tmpBA;
unsigned _tmpBB=d.varloc;unsigned varloc=_tmpBB;
struct _tuple14 _tmpBC=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple14 _stmttmpB=_tmpBC;struct _tuple14 _tmpBD=_stmttmpB;struct Cyc_List_List*_tmpC1;struct Cyc_List_List*_tmpC0;void*_tmpBF;struct Cyc_Absyn_Tqual _tmpBE;_LL1: _tmpBE=_tmpBD.f1;_tmpBF=_tmpBD.f2;_tmpC0=_tmpBD.f3;_tmpC1=_tmpBD.f4;_LL2: {struct Cyc_Absyn_Tqual tq2=_tmpBE;void*new_typ=_tmpBF;struct Cyc_List_List*tvs=_tmpC0;struct Cyc_List_List*atts=_tmpC1;
# 660
struct Cyc_List_List*_tmpC2=ds->tl == 0?0:({struct _RegionHandle*_tmp751=r;struct Cyc_Absyn_Tqual _tmp750=tq;void*_tmp74F=Cyc_Tcutil_copy_type(t);struct _tuple13*_tmp74E=ds->tl;Cyc_Parse_apply_tmss(_tmp751,_tmp750,_tmp74F,_tmp74E,shared_atts);});struct Cyc_List_List*tl=_tmpC2;
return({struct Cyc_List_List*_tmpC4=_region_malloc(r,sizeof(*_tmpC4));({struct _tuple16*_tmp752=({struct _tuple16*_tmpC3=_region_malloc(r,sizeof(*_tmpC3));_tmpC3->f1=varloc,_tmpC3->f2=q,_tmpC3->f3=tq2,_tmpC3->f4=new_typ,_tmpC3->f5=tvs,_tmpC3->f6=atts;_tmpC3;});_tmpC4->hd=_tmp752;}),_tmpC4->tl=tl;_tmpC4;});}}}
# 664
static struct _tuple14 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 667
if(tms == 0)return({struct _tuple14 _tmp657;_tmp657.f1=tq,_tmp657.f2=t,_tmp657.f3=0,_tmp657.f4=atts;_tmp657;});{
void*_tmpC5=(void*)tms->hd;void*_stmttmpC=_tmpC5;void*_tmpC6=_stmttmpC;struct Cyc_List_List*_tmpC8;unsigned _tmpC7;struct Cyc_Absyn_Tqual _tmpCA;struct Cyc_Absyn_PtrAtts _tmpC9;unsigned _tmpCC;struct Cyc_List_List*_tmpCB;void*_tmpCD;unsigned _tmpD0;void*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;unsigned _tmpD2;void*_tmpD1;switch(*((int*)_tmpC6)){case 0: _LL1: _tmpD1=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpC6)->f1;_tmpD2=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpC6)->f2;_LL2: {void*zeroterm=_tmpD1;unsigned ztloc=_tmpD2;
# 670
return({struct Cyc_Absyn_Tqual _tmp755=Cyc_Absyn_empty_tqual(0U);void*_tmp754=
Cyc_Absyn_array_type(t,tq,0,zeroterm,ztloc);
# 670
struct Cyc_List_List*_tmp753=atts;Cyc_Parse_apply_tms(_tmp755,_tmp754,_tmp753,tms->tl);});}case 1: _LL3: _tmpCE=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpC6)->f1;_tmpCF=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpC6)->f2;_tmpD0=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpC6)->f3;_LL4: {struct Cyc_Absyn_Exp*e=_tmpCE;void*zeroterm=_tmpCF;unsigned ztloc=_tmpD0;
# 673
return({struct Cyc_Absyn_Tqual _tmp758=Cyc_Absyn_empty_tqual(0U);void*_tmp757=
Cyc_Absyn_array_type(t,tq,e,zeroterm,ztloc);
# 673
struct Cyc_List_List*_tmp756=atts;Cyc_Parse_apply_tms(_tmp758,_tmp757,_tmp756,tms->tl);});}case 3: _LL5: _tmpCD=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpC6)->f1;_LL6: {void*args=_tmpCD;
# 676
void*_tmpD3=args;unsigned _tmpD4;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_List_List*_tmpD9;void*_tmpD8;struct Cyc_Absyn_VarargInfo*_tmpD7;int _tmpD6;struct Cyc_List_List*_tmpD5;if(*((int*)_tmpD3)== 1){_LLE: _tmpD5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD3)->f1;_tmpD6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD3)->f2;_tmpD7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD3)->f3;_tmpD8=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD3)->f4;_tmpD9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD3)->f5;_tmpDA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD3)->f6;_tmpDB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpD3)->f7;_LLF: {struct Cyc_List_List*args2=_tmpD5;int c_vararg=_tmpD6;struct Cyc_Absyn_VarargInfo*cyc_vararg=_tmpD7;void*eff=_tmpD8;struct Cyc_List_List*rgn_po=_tmpD9;struct Cyc_Absyn_Exp*req=_tmpDA;struct Cyc_Absyn_Exp*ens=_tmpDB;
# 678
struct Cyc_List_List*typvars=0;
# 680
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Atts_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=(void*)as->hd,_tmpDC->tl=fn_atts;_tmpDC;});else{
# 685
new_atts=({struct Cyc_List_List*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->hd=(void*)as->hd,_tmpDD->tl=new_atts;_tmpDD;});}}}
# 687
if(tms->tl != 0){
void*_tmpDE=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_stmttmpD=_tmpDE;void*_tmpDF=_stmttmpD;struct Cyc_List_List*_tmpE0;if(*((int*)_tmpDF)== 4){_LL13: _tmpE0=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpDF)->f1;_LL14: {struct Cyc_List_List*ts=_tmpE0;
# 690
typvars=ts;
tms=tms->tl;
goto _LL12;}}else{_LL15: _LL16:
 goto _LL12;}_LL12:;}
# 696
if(((((!c_vararg && cyc_vararg == 0)&& args2 != 0)&& args2->tl == 0)&&(*((struct _tuple8*)args2->hd)).f1 == 0)&&(*((struct _tuple8*)args2->hd)).f3 == Cyc_Absyn_void_type)
# 701
args2=0;{
# 704
struct Cyc_List_List*_tmpE1=Cyc_Parse_get_arg_tags(args2);struct Cyc_List_List*tags=_tmpE1;
# 706
if(tags != 0)
t=Cyc_Parse_substitute_tags(tags,t);
t=Cyc_Parse_array2ptr(t,0);
# 711
{struct Cyc_List_List*_tmpE2=args2;struct Cyc_List_List*a=_tmpE2;for(0;a != 0;a=a->tl){
struct _tuple8*_tmpE3=(struct _tuple8*)a->hd;struct _tuple8*_stmttmpE=_tmpE3;struct _tuple8*_tmpE4=_stmttmpE;void**_tmpE7;struct Cyc_Absyn_Tqual _tmpE6;struct _fat_ptr*_tmpE5;_LL18: _tmpE5=_tmpE4->f1;_tmpE6=_tmpE4->f2;_tmpE7=(void**)& _tmpE4->f3;_LL19: {struct _fat_ptr*vopt=_tmpE5;struct Cyc_Absyn_Tqual tq=_tmpE6;void**t=_tmpE7;
if(tags != 0)
({void*_tmp759=Cyc_Parse_substitute_tags(tags,*t);*t=_tmp759;});
({void*_tmp75A=Cyc_Parse_array2ptr(*t,1);*t=_tmp75A;});}}}
# 723
return({struct Cyc_Absyn_Tqual _tmp75D=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp75C=
Cyc_Absyn_function_type(typvars,eff,tq,t,args2,c_vararg,cyc_vararg,rgn_po,fn_atts,req,ens);
# 723
struct Cyc_List_List*_tmp75B=new_atts;Cyc_Parse_apply_tms(_tmp75D,_tmp75C,_tmp75B,((struct Cyc_List_List*)_check_null(tms))->tl);});}}}else{_LL10: _tmpD4=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpD3)->f2;_LL11: {unsigned loc=_tmpD4;
# 730
({unsigned _tmp75E=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp75E,({const char*_tmpE8="function declaration without parameter types";_tag_fat(_tmpE8,sizeof(char),45U);}));});}}_LLD:;}case 4: _LL7: _tmpCB=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpC6)->f1;_tmpCC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpC6)->f2;_LL8: {struct Cyc_List_List*ts=_tmpCB;unsigned loc=_tmpCC;
# 737
if(tms->tl == 0)
return({struct _tuple14 _tmp658;_tmp658.f1=tq,_tmp658.f2=t,_tmp658.f3=ts,_tmp658.f4=atts;_tmp658;});
# 742
({unsigned _tmp75F=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp75F,({const char*_tmpE9="type parameters must appear before function arguments in declarator";_tag_fat(_tmpE9,sizeof(char),68U);}));});}case 2: _LL9: _tmpC9=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpC6)->f1;_tmpCA=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpC6)->f2;_LLA: {struct Cyc_Absyn_PtrAtts ptratts=_tmpC9;struct Cyc_Absyn_Tqual tq2=_tmpCA;
# 745
return({struct Cyc_Absyn_Tqual _tmp762=tq2;void*_tmp761=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp659;_tmp659.elt_type=t,_tmp659.elt_tq=tq,_tmp659.ptr_atts=ptratts;_tmp659;}));struct Cyc_List_List*_tmp760=atts;Cyc_Parse_apply_tms(_tmp762,_tmp761,_tmp760,tms->tl);});}default: _LLB: _tmpC7=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpC6)->f1;_tmpC8=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpC6)->f2;_LLC: {unsigned loc=_tmpC7;struct Cyc_List_List*atts2=_tmpC8;
# 750
return({struct Cyc_Absyn_Tqual _tmp765=tq;void*_tmp764=t;struct Cyc_List_List*_tmp763=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(atts,atts2);Cyc_Parse_apply_tms(_tmp765,_tmp764,_tmp763,tms->tl);});}}_LL0:;}}
# 756
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 765
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned loc,struct _tuple16*t){
struct _tuple16*_tmpEA=t;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpEF;void*_tmpEE;struct Cyc_Absyn_Tqual _tmpED;struct _tuple0*_tmpEC;unsigned _tmpEB;_LL1: _tmpEB=_tmpEA->f1;_tmpEC=_tmpEA->f2;_tmpED=_tmpEA->f3;_tmpEE=_tmpEA->f4;_tmpEF=_tmpEA->f5;_tmpF0=_tmpEA->f6;_LL2: {unsigned varloc=_tmpEB;struct _tuple0*x=_tmpEC;struct Cyc_Absyn_Tqual tq=_tmpED;void*typ=_tmpEE;struct Cyc_List_List*tvs=_tmpEF;struct Cyc_List_List*atts=_tmpF0;
# 768
Cyc_Lex_register_typedef(x);{
# 770
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpF1=typ;struct Cyc_Core_Opt*_tmpF2;if(*((int*)_tmpF1)== 1){_LL4: _tmpF2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF1)->f1;_LL5: {struct Cyc_Core_Opt*kopt=_tmpF2;
# 774
type=0;
kind=kopt == 0?& Cyc_Kinds_bko: kopt;
goto _LL3;}}else{_LL6: _LL7:
 kind=0;type=typ;goto _LL3;}_LL3:;}
# 779
return({void*_tmp767=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=8,({struct Cyc_Absyn_Typedefdecl*_tmp766=({struct Cyc_Absyn_Typedefdecl*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->name=x,_tmpF3->tvs=tvs,_tmpF3->kind=kind,_tmpF3->defn=type,_tmpF3->atts=atts,_tmpF3->tq=tq,_tmpF3->extern_c=0;_tmpF3;});_tmpF4->f1=_tmp766;});_tmpF4;});Cyc_Absyn_new_decl(_tmp767,loc);});}}}
# 786
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp768=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=12,_tmpF5->f1=d,_tmpF5->f2=s;_tmpF5;});Cyc_Absyn_new_stmt(_tmp768,d->loc);});}
# 790
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*,struct Cyc_Absyn_Stmt*))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 794
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple11*ds,struct _tuple13**decls,struct Cyc_List_List**es){
# 797
struct _tuple13*declarators=0;
struct Cyc_List_List*exprs=0;
for(1;ds != 0;ds=ds->tl){
struct _tuple12 _tmpF6=ds->hd;struct _tuple12 _stmttmpF=_tmpF6;struct _tuple12 _tmpF7=_stmttmpF;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Parse_Declarator _tmpF8;_LL1: _tmpF8=_tmpF7.f1;_tmpF9=_tmpF7.f2;_LL2: {struct Cyc_Parse_Declarator d=_tmpF8;struct Cyc_Absyn_Exp*e=_tmpF9;
declarators=({struct _tuple13*_tmpFA=_region_malloc(r,sizeof(*_tmpFA));_tmpFA->tl=declarators,_tmpFA->hd=d;_tmpFA;});
exprs=({struct Cyc_List_List*_tmpFB=_region_malloc(r,sizeof(*_tmpFB));_tmpFB->hd=e,_tmpFB->tl=exprs;_tmpFB;});}}
# 804
({struct Cyc_List_List*_tmp769=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(exprs);*es=_tmp769;});
({struct _tuple13*_tmp76A=((struct _tuple13*(*)(struct _tuple13*))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp76A;});}
# 813
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple11*ids,unsigned tqual_loc,unsigned loc){
# 816
struct _RegionHandle _tmpFC=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpFC;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpFD=ds;struct Cyc_List_List*_tmp100;struct Cyc_Parse_Type_specifier _tmpFF;struct Cyc_Absyn_Tqual _tmpFE;_LL1: _tmpFE=_tmpFD.tq;_tmpFF=_tmpFD.type_specs;_tmp100=_tmpFD.attributes;_LL2: {struct Cyc_Absyn_Tqual tq=_tmpFE;struct Cyc_Parse_Type_specifier tss=_tmpFF;struct Cyc_List_List*atts=_tmp100;
if(tq.loc == (unsigned)0)tq.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp101=0U;({unsigned _tmp76C=loc;struct _fat_ptr _tmp76B=({const char*_tmp102="inline qualifier on non-function definition";_tag_fat(_tmp102,sizeof(char),44U);});Cyc_Warn_warn(_tmp76C,_tmp76B,_tag_fat(_tmp101,sizeof(void*),0));});});{
# 822
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
{enum Cyc_Parse_Storage_class _tmp103=ds.sc;enum Cyc_Parse_Storage_class _stmttmp10=_tmp103;enum Cyc_Parse_Storage_class _tmp104=_stmttmp10;switch(_tmp104){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 s=Cyc_Flags_no_register?Cyc_Absyn_Public: Cyc_Absyn_Register;goto _LL3;case Cyc_Parse_Abstract_sc: _LL10: _LL11:
 s=1U;goto _LL3;default: _LL12: _LL13:
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
void*_tmp105=Cyc_Parse_collapse_type_specifiers(tss,loc);void*base_type=_tmp105;
if(declarators == 0){
# 854
void*_tmp106=base_type;struct Cyc_List_List*_tmp107;struct _tuple0*_tmp108;struct Cyc_List_List*_tmp10B;int _tmp10A;struct _tuple0*_tmp109;struct Cyc_Absyn_Datatypedecl**_tmp10C;struct Cyc_List_List*_tmp10F;struct _tuple0*_tmp10E;enum Cyc_Absyn_AggrKind _tmp10D;struct Cyc_Absyn_Datatypedecl*_tmp110;struct Cyc_Absyn_Enumdecl*_tmp111;struct Cyc_Absyn_Aggrdecl*_tmp112;switch(*((int*)_tmp106)){case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp106)->f1)->r)){case 0: _LL15: _tmp112=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp106)->f1)->r)->f1;_LL16: {struct Cyc_Absyn_Aggrdecl*ad=_tmp112;
# 856
({struct Cyc_List_List*_tmp76D=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(ad->attributes,atts);ad->attributes=_tmp76D;});
ad->sc=s;{
struct Cyc_List_List*_tmp115=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));({struct Cyc_Absyn_Decl*_tmp76F=({void*_tmp76E=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=5,_tmp113->f1=ad;_tmp113;});Cyc_Absyn_new_decl(_tmp76E,loc);});_tmp114->hd=_tmp76F;}),_tmp114->tl=0;_tmp114;});_npop_handler(0);return _tmp115;}}case 1: _LL17: _tmp111=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp106)->f1)->r)->f1;_LL18: {struct Cyc_Absyn_Enumdecl*ed=_tmp111;
# 860
if(atts != 0)({void*_tmp116=0U;({unsigned _tmp771=loc;struct _fat_ptr _tmp770=({const char*_tmp117="attributes on enum not supported";_tag_fat(_tmp117,sizeof(char),33U);});Cyc_Warn_err(_tmp771,_tmp770,_tag_fat(_tmp116,sizeof(void*),0));});});
ed->sc=s;{
struct Cyc_List_List*_tmp11A=({struct Cyc_List_List*_tmp119=_cycalloc(sizeof(*_tmp119));({struct Cyc_Absyn_Decl*_tmp773=({void*_tmp772=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->tag=7,_tmp118->f1=ed;_tmp118;});Cyc_Absyn_new_decl(_tmp772,loc);});_tmp119->hd=_tmp773;}),_tmp119->tl=0;_tmp119;});_npop_handler(0);return _tmp11A;}}default: _LL19: _tmp110=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp106)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Datatypedecl*dd=_tmp110;
# 864
if(atts != 0)({void*_tmp11B=0U;({unsigned _tmp775=loc;struct _fat_ptr _tmp774=({const char*_tmp11C="attributes on datatypes not supported";_tag_fat(_tmp11C,sizeof(char),38U);});Cyc_Warn_err(_tmp775,_tmp774,_tag_fat(_tmp11B,sizeof(void*),0));});});
dd->sc=s;{
struct Cyc_List_List*_tmp11F=({struct Cyc_List_List*_tmp11E=_cycalloc(sizeof(*_tmp11E));({struct Cyc_Absyn_Decl*_tmp777=({void*_tmp776=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=6,_tmp11D->f1=dd;_tmp11D;});Cyc_Absyn_new_decl(_tmp776,loc);});_tmp11E->hd=_tmp777;}),_tmp11E->tl=0;_tmp11E;});_npop_handler(0);return _tmp11F;}}}case 0: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)){case 20: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1).UnknownAggr).tag == 1){_LL1B: _tmp10D=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1).UnknownAggr).val).f1;_tmp10E=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1).UnknownAggr).val).f2;_tmp10F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f2;_LL1C: {enum Cyc_Absyn_AggrKind k=_tmp10D;struct _tuple0*n=_tmp10E;struct Cyc_List_List*ts=_tmp10F;
# 868
struct Cyc_List_List*_tmp120=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp120;
struct Cyc_Absyn_Aggrdecl*_tmp121=({struct Cyc_Absyn_Aggrdecl*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->kind=k,_tmp127->sc=s,_tmp127->name=n,_tmp127->tvs=ts2,_tmp127->impl=0,_tmp127->attributes=0,_tmp127->expected_mem_kind=0;_tmp127;});struct Cyc_Absyn_Aggrdecl*ad=_tmp121;
if(atts != 0)({void*_tmp122=0U;({unsigned _tmp779=loc;struct _fat_ptr _tmp778=({const char*_tmp123="bad attributes on type declaration";_tag_fat(_tmp123,sizeof(char),35U);});Cyc_Warn_err(_tmp779,_tmp778,_tag_fat(_tmp122,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp126=({struct Cyc_List_List*_tmp125=_cycalloc(sizeof(*_tmp125));({struct Cyc_Absyn_Decl*_tmp77B=({void*_tmp77A=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=5,_tmp124->f1=ad;_tmp124;});Cyc_Absyn_new_decl(_tmp77A,loc);});_tmp125->hd=_tmp77B;}),_tmp125->tl=0;_tmp125;});_npop_handler(0);return _tmp126;}}}else{goto _LL25;}case 18: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1).KnownDatatype).tag == 2){_LL1D: _tmp10C=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1).KnownDatatype).val;_LL1E: {struct Cyc_Absyn_Datatypedecl**tudp=_tmp10C;
# 873
if(atts != 0)({void*_tmp128=0U;({unsigned _tmp77D=loc;struct _fat_ptr _tmp77C=({const char*_tmp129="bad attributes on datatype";_tag_fat(_tmp129,sizeof(char),27U);});Cyc_Warn_err(_tmp77D,_tmp77C,_tag_fat(_tmp128,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp12C=({struct Cyc_List_List*_tmp12B=_cycalloc(sizeof(*_tmp12B));({struct Cyc_Absyn_Decl*_tmp77F=({void*_tmp77E=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=6,_tmp12A->f1=*tudp;_tmp12A;});Cyc_Absyn_new_decl(_tmp77E,loc);});_tmp12B->hd=_tmp77F;}),_tmp12B->tl=0;_tmp12B;});_npop_handler(0);return _tmp12C;}}}else{_LL1F: _tmp109=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1).UnknownDatatype).val).name;_tmp10A=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp10B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f2;_LL20: {struct _tuple0*n=_tmp109;int isx=_tmp10A;struct Cyc_List_List*ts=_tmp10B;
# 876
struct Cyc_List_List*_tmp12D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,ts);struct Cyc_List_List*ts2=_tmp12D;
struct Cyc_Absyn_Decl*_tmp12E=Cyc_Absyn_datatype_decl(s,n,ts2,0,isx,loc);struct Cyc_Absyn_Decl*tud=_tmp12E;
if(atts != 0)({void*_tmp12F=0U;({unsigned _tmp781=loc;struct _fat_ptr _tmp780=({const char*_tmp130="bad attributes on datatype";_tag_fat(_tmp130,sizeof(char),27U);});Cyc_Warn_err(_tmp781,_tmp780,_tag_fat(_tmp12F,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp132=({struct Cyc_List_List*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->hd=tud,_tmp131->tl=0;_tmp131;});_npop_handler(0);return _tmp132;}}}case 15: _LL21: _tmp108=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1;_LL22: {struct _tuple0*n=_tmp108;
# 881
struct Cyc_Absyn_Enumdecl*_tmp133=({struct Cyc_Absyn_Enumdecl*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->sc=s,_tmp13A->name=n,_tmp13A->fields=0;_tmp13A;});struct Cyc_Absyn_Enumdecl*ed=_tmp133;
if(atts != 0)({void*_tmp134=0U;({unsigned _tmp783=loc;struct _fat_ptr _tmp782=({const char*_tmp135="bad attributes on enum";_tag_fat(_tmp135,sizeof(char),23U);});Cyc_Warn_err(_tmp783,_tmp782,_tag_fat(_tmp134,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp139=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));({struct Cyc_Absyn_Decl*_tmp785=({struct Cyc_Absyn_Decl*_tmp137=_cycalloc(sizeof(*_tmp137));({void*_tmp784=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->tag=7,_tmp136->f1=ed;_tmp136;});_tmp137->r=_tmp784;}),_tmp137->loc=loc;_tmp137;});_tmp138->hd=_tmp785;}),_tmp138->tl=0;_tmp138;});_npop_handler(0);return _tmp139;}}case 16: _LL23: _tmp107=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp106)->f1)->f1;_LL24: {struct Cyc_List_List*fs=_tmp107;
# 887
struct Cyc_Absyn_Enumdecl*_tmp13B=({struct Cyc_Absyn_Enumdecl*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->sc=s,({struct _tuple0*_tmp787=Cyc_Parse_gensym_enum();_tmp143->name=_tmp787;}),({struct Cyc_Core_Opt*_tmp786=({struct Cyc_Core_Opt*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->v=fs;_tmp142;});_tmp143->fields=_tmp786;});_tmp143;});struct Cyc_Absyn_Enumdecl*ed=_tmp13B;
if(atts != 0)({void*_tmp13C=0U;({unsigned _tmp789=loc;struct _fat_ptr _tmp788=({const char*_tmp13D="bad attributes on enum";_tag_fat(_tmp13D,sizeof(char),23U);});Cyc_Warn_err(_tmp789,_tmp788,_tag_fat(_tmp13C,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp141=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_Absyn_Decl*_tmp78B=({struct Cyc_Absyn_Decl*_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp78A=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->tag=7,_tmp13E->f1=ed;_tmp13E;});_tmp13F->r=_tmp78A;}),_tmp13F->loc=loc;_tmp13F;});_tmp140->hd=_tmp78B;}),_tmp140->tl=0;_tmp140;});_npop_handler(0);return _tmp141;}}default: goto _LL25;}default: _LL25: _LL26:
({void*_tmp144=0U;({unsigned _tmp78D=loc;struct _fat_ptr _tmp78C=({const char*_tmp145="missing declarator";_tag_fat(_tmp145,sizeof(char),19U);});Cyc_Warn_err(_tmp78D,_tmp78C,_tag_fat(_tmp144,sizeof(void*),0));});});{struct Cyc_List_List*_tmp146=0;_npop_handler(0);return _tmp146;}}_LL14:;}{
# 894
struct Cyc_List_List*_tmp147=Cyc_Parse_apply_tmss(mkrgn,tq,base_type,declarators,atts);struct Cyc_List_List*fields=_tmp147;
if(istypedef){
# 899
if(!exps_empty)
({void*_tmp148=0U;({unsigned _tmp78F=loc;struct _fat_ptr _tmp78E=({const char*_tmp149="initializer in typedef declaration";_tag_fat(_tmp149,sizeof(char),35U);});Cyc_Warn_err(_tmp78F,_tmp78E,_tag_fat(_tmp148,sizeof(void*),0));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(unsigned,struct _tuple16*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,fields);
struct Cyc_List_List*_tmp14A=decls;_npop_handler(0);return _tmp14A;}}{
# 905
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp14B=fields;struct Cyc_List_List*ds=_tmp14B;for(0;ds != 0;(ds=ds->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple16*_tmp14C=(struct _tuple16*)ds->hd;struct _tuple16*_stmttmp11=_tmp14C;struct _tuple16*_tmp14D=_stmttmp11;struct Cyc_List_List*_tmp153;struct Cyc_List_List*_tmp152;void*_tmp151;struct Cyc_Absyn_Tqual _tmp150;struct _tuple0*_tmp14F;unsigned _tmp14E;_LL28: _tmp14E=_tmp14D->f1;_tmp14F=_tmp14D->f2;_tmp150=_tmp14D->f3;_tmp151=_tmp14D->f4;_tmp152=_tmp14D->f5;_tmp153=_tmp14D->f6;_LL29: {unsigned varloc=_tmp14E;struct _tuple0*x=_tmp14F;struct Cyc_Absyn_Tqual tq2=_tmp150;void*t2=_tmp151;struct Cyc_List_List*tvs2=_tmp152;struct Cyc_List_List*atts2=_tmp153;
if(tvs2 != 0)
({void*_tmp154=0U;({unsigned _tmp791=loc;struct _fat_ptr _tmp790=({const char*_tmp155="bad type params, ignoring";_tag_fat(_tmp155,sizeof(char),26U);});Cyc_Warn_warn(_tmp791,_tmp790,_tag_fat(_tmp154,sizeof(void*),0));});});
if(exprs == 0)
({unsigned _tmp792=loc;((int(*)(unsigned,struct _fat_ptr))Cyc_Parse_parse_abort)(_tmp792,({const char*_tmp156="unexpected NULL in parse!";_tag_fat(_tmp156,sizeof(char),26U);}));});{
struct Cyc_Absyn_Vardecl*_tmp157=Cyc_Absyn_new_vardecl(varloc,x,t2,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd);struct Cyc_Absyn_Vardecl*vd=_tmp157;
vd->tq=tq2;
vd->sc=s;
vd->attributes=atts2;
decls=({struct Cyc_List_List*_tmp15A=_cycalloc(sizeof(*_tmp15A));({struct Cyc_Absyn_Decl*_tmp794=({struct Cyc_Absyn_Decl*_tmp159=_cycalloc(sizeof(*_tmp159));({void*_tmp793=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->tag=0,_tmp158->f1=vd;_tmp158;});_tmp159->r=_tmp793;}),_tmp159->loc=loc;_tmp159;});_tmp15A->hd=_tmp794;}),_tmp15A->tl=decls;_tmp15A;});}}}}{
# 918
struct Cyc_List_List*_tmp15B=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(decls);_npop_handler(0);return _tmp15B;}}}}}}}}}
# 817
;_pop_region();}
# 922
static unsigned Cyc_Parse_cnst2uint(unsigned loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp15C=x;long long _tmp15D;char _tmp15E;int _tmp15F;switch((_tmp15C.LongLong_c).tag){case 5: _LL1: _tmp15F=((_tmp15C.Int_c).val).f2;_LL2: {int i=_tmp15F;
return(unsigned)i;}case 2: _LL3: _tmp15E=((_tmp15C.Char_c).val).f2;_LL4: {char c=_tmp15E;
return(unsigned)c;}case 6: _LL5: _tmp15D=((_tmp15C.LongLong_c).val).f2;_LL6: {long long x=_tmp15D;
# 927
unsigned long long y=(unsigned long long)x;
if(y > (unsigned long long)-1)
({void*_tmp160=0U;({unsigned _tmp796=loc;struct _fat_ptr _tmp795=({const char*_tmp161="integer constant too large";_tag_fat(_tmp161,sizeof(char),27U);});Cyc_Warn_err(_tmp796,_tmp795,_tag_fat(_tmp160,sizeof(void*),0));});});
return(unsigned)x;}default: _LL7: _LL8:
# 932
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp65A;_tmp65A.tag=0,({struct _fat_ptr _tmp797=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(x));_tmp65A.f1=_tmp797;});_tmp65A;});void*_tmp162[1];_tmp162[0]=& _tmp164;({unsigned _tmp799=loc;struct _fat_ptr _tmp798=({const char*_tmp163="expected integer constant but found %s";_tag_fat(_tmp163,sizeof(char),39U);});Cyc_Warn_err(_tmp799,_tmp798,_tag_fat(_tmp162,sizeof(void*),1));});});
return 0U;}_LL0:;}
# 938
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp165=p->r;void*_stmttmp12=_tmp165;void*_tmp166=_stmttmp12;struct Cyc_Absyn_Exp*_tmp167;struct Cyc_List_List*_tmp169;struct _tuple0*_tmp168;int _tmp16B;struct _fat_ptr _tmp16A;char _tmp16C;int _tmp16E;enum Cyc_Absyn_Sign _tmp16D;struct Cyc_Absyn_Pat*_tmp16F;struct Cyc_Absyn_Vardecl*_tmp170;struct _tuple0*_tmp171;switch(*((int*)_tmp166)){case 15: _LL1: _tmp171=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_LL2: {struct _tuple0*x=_tmp171;
return Cyc_Absyn_unknownid_exp(x,p->loc);}case 3: if(*((int*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp166)->f2)->r)== 0){_LL3: _tmp170=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp170;
# 942
return({struct Cyc_Absyn_Exp*_tmp79A=Cyc_Absyn_unknownid_exp(vd->name,p->loc);Cyc_Absyn_deref_exp(_tmp79A,p->loc);});}}else{goto _LL13;}case 6: _LL5: _tmp16F=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_LL6: {struct Cyc_Absyn_Pat*p2=_tmp16F;
return({struct Cyc_Absyn_Exp*_tmp79B=Cyc_Parse_pat2exp(p2);Cyc_Absyn_address_exp(_tmp79B,p->loc);});}case 9: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10: _LL9: _tmp16D=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_tmp16E=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp166)->f2;_LLA: {enum Cyc_Absyn_Sign s=_tmp16D;int i=_tmp16E;
return Cyc_Absyn_int_exp(s,i,p->loc);}case 11: _LLB: _tmp16C=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_LLC: {char c=_tmp16C;
return Cyc_Absyn_char_exp(c,p->loc);}case 12: _LLD: _tmp16A=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_tmp16B=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp166)->f2;_LLE: {struct _fat_ptr s=_tmp16A;int i=_tmp16B;
return Cyc_Absyn_float_exp(s,i,p->loc);}case 16: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp166)->f3 == 0){_LLF: _tmp168=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_tmp169=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp166)->f2;_LL10: {struct _tuple0*x=_tmp168;struct Cyc_List_List*ps=_tmp169;
# 949
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(x,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*))Cyc_List_map)(Cyc_Parse_pat2exp,ps);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17: _LL11: _tmp167=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp166)->f1;_LL12: {struct Cyc_Absyn_Exp*e=_tmp167;
return e;}default: _LL13: _LL14:
# 954
({void*_tmp172=0U;({unsigned _tmp79D=p->loc;struct _fat_ptr _tmp79C=({const char*_tmp173="cannot mix patterns and expressions in case";_tag_fat(_tmp173,sizeof(char),44U);});Cyc_Warn_err(_tmp79D,_tmp79C,_tag_fat(_tmp172,sizeof(void*),0));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple22{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple23{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _tuple25{int f1;enum Cyc_Absyn_AggrKind f2;};struct _union_YYSTYPE_YY23{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY28{int tag;struct Cyc_Parse_Declarator val;};struct _tuple26{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY29{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY31{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY32{int tag;int val;};struct _union_YYSTYPE_YY33{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY35{int tag;struct Cyc_List_List*val;};struct _tuple27{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY36{int tag;struct _tuple27 val;};struct _union_YYSTYPE_YY37{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY38{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY39{int tag;struct Cyc_List_List*val;};struct _tuple28{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY40{int tag;struct _tuple28*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY43{int tag;void*val;};struct _union_YYSTYPE_YY44{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY45{int tag;void*val;};struct _union_YYSTYPE_YY46{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY47{int tag;void*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY49{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY50{int tag;void*val;};struct _union_YYSTYPE_YY51{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY52{int tag;void*val;};struct _union_YYSTYPE_YY53{int tag;struct Cyc_List_List*val;};struct _tuple29{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY54{int tag;struct _tuple29*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY56{int tag;void*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY58{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY59{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY60{int tag;void*val;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY61{int tag;struct _tuple30*val;};struct _tuple31{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY62{int tag;struct _tuple31*val;};struct _union_YYSTYPE_YY63{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY64{int tag;struct Cyc_List_List*val;};struct _tuple32{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY65{int tag;struct _tuple32*val;};struct _union_YYSTYPE_YY66{int tag;struct Cyc_Absyn_Exp*(*val)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YY64 YY64;struct _union_YYSTYPE_YY65 YY65;struct _union_YYSTYPE_YY66 YY66;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};
# 1040
static void Cyc_yythrowfail(struct _fat_ptr s){
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->tag=Cyc_Core_Failure,_tmp174->f1=s;_tmp174;}));}static char _tmp177[7U]="cnst_t";
# 1009 "parse.y"
static union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp177,_tmp177,_tmp177 + 7U};
union Cyc_YYSTYPE*_tmp175=yy1;union Cyc_Absyn_Cnst _tmp176;if((((union Cyc_YYSTYPE*)_tmp175)->Int_tok).tag == 1){_LL1: _tmp176=(_tmp175->Int_tok).val;_LL2: {union Cyc_Absyn_Cnst yy=_tmp176;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1016
static union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp65B;(_tmp65B.Int_tok).tag=1U,(_tmp65B.Int_tok).val=yy1;_tmp65B;});}static char _tmp17A[5U]="char";
# 1010 "parse.y"
static char Cyc_yyget_Char_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp17A,_tmp17A,_tmp17A + 5U};
union Cyc_YYSTYPE*_tmp178=yy1;char _tmp179;if((((union Cyc_YYSTYPE*)_tmp178)->Char_tok).tag == 2){_LL1: _tmp179=(_tmp178->Char_tok).val;_LL2: {char yy=_tmp179;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1017
static union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp65C;(_tmp65C.Char_tok).tag=2U,(_tmp65C.Char_tok).val=yy1;_tmp65C;});}static char _tmp17D[13U]="string_t<`H>";
# 1011 "parse.y"
static struct _fat_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp17D,_tmp17D,_tmp17D + 13U};
union Cyc_YYSTYPE*_tmp17B=yy1;struct _fat_ptr _tmp17C;if((((union Cyc_YYSTYPE*)_tmp17B)->String_tok).tag == 3){_LL1: _tmp17C=(_tmp17B->String_tok).val;_LL2: {struct _fat_ptr yy=_tmp17C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1018
static union Cyc_YYSTYPE Cyc_String_tok(struct _fat_ptr yy1){return({union Cyc_YYSTYPE _tmp65D;(_tmp65D.String_tok).tag=3U,(_tmp65D.String_tok).val=yy1;_tmp65D;});}static char _tmp180[35U]="$(seg_t,booltype_t, ptrbound_t)@`H";
# 1014 "parse.y"
static struct _tuple22*Cyc_yyget_YY1(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp180,_tmp180,_tmp180 + 35U};
union Cyc_YYSTYPE*_tmp17E=yy1;struct _tuple22*_tmp17F;if((((union Cyc_YYSTYPE*)_tmp17E)->YY1).tag == 7){_LL1: _tmp17F=(_tmp17E->YY1).val;_LL2: {struct _tuple22*yy=_tmp17F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1021
static union Cyc_YYSTYPE Cyc_YY1(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp65E;(_tmp65E.YY1).tag=7U,(_tmp65E.YY1).val=yy1;_tmp65E;});}static char _tmp183[11U]="ptrbound_t";
# 1015 "parse.y"
static void*Cyc_yyget_YY2(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp183,_tmp183,_tmp183 + 11U};
union Cyc_YYSTYPE*_tmp181=yy1;void*_tmp182;if((((union Cyc_YYSTYPE*)_tmp181)->YY2).tag == 8){_LL1: _tmp182=(_tmp181->YY2).val;_LL2: {void*yy=_tmp182;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1022
static union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp65F;(_tmp65F.YY2).tag=8U,(_tmp65F.YY2).val=yy1;_tmp65F;});}static char _tmp186[28U]="list_t<offsetof_field_t,`H>";
# 1016 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp186,_tmp186,_tmp186 + 28U};
union Cyc_YYSTYPE*_tmp184=yy1;struct Cyc_List_List*_tmp185;if((((union Cyc_YYSTYPE*)_tmp184)->YY3).tag == 9){_LL1: _tmp185=(_tmp184->YY3).val;_LL2: {struct Cyc_List_List*yy=_tmp185;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1023
static union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp660;(_tmp660.YY3).tag=9U,(_tmp660.YY3).val=yy1;_tmp660;});}static char _tmp189[6U]="exp_t";
# 1017 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp189,_tmp189,_tmp189 + 6U};
union Cyc_YYSTYPE*_tmp187=yy1;struct Cyc_Absyn_Exp*_tmp188;if((((union Cyc_YYSTYPE*)_tmp187)->Exp_tok).tag == 5){_LL1: _tmp188=(_tmp187->Exp_tok).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp188;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1024
static union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp661;(_tmp661.Exp_tok).tag=5U,(_tmp661.Exp_tok).val=yy1;_tmp661;});}static char _tmp18C[17U]="list_t<exp_t,`H>";
static struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18C,_tmp18C,_tmp18C + 17U};
union Cyc_YYSTYPE*_tmp18A=yy1;struct Cyc_List_List*_tmp18B;if((((union Cyc_YYSTYPE*)_tmp18A)->YY4).tag == 10){_LL1: _tmp18B=(_tmp18A->YY4).val;_LL2: {struct Cyc_List_List*yy=_tmp18B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1032
static union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp662;(_tmp662.YY4).tag=10U,(_tmp662.YY4).val=yy1;_tmp662;});}static char _tmp18F[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1026 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp18F,_tmp18F,_tmp18F + 47U};
union Cyc_YYSTYPE*_tmp18D=yy1;struct Cyc_List_List*_tmp18E;if((((union Cyc_YYSTYPE*)_tmp18D)->YY5).tag == 11){_LL1: _tmp18E=(_tmp18D->YY5).val;_LL2: {struct Cyc_List_List*yy=_tmp18E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1033
static union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp663;(_tmp663.YY5).tag=11U,(_tmp663.YY5).val=yy1;_tmp663;});}static char _tmp192[9U]="primop_t";
# 1027 "parse.y"
static enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp192,_tmp192,_tmp192 + 9U};
union Cyc_YYSTYPE*_tmp190=yy1;enum Cyc_Absyn_Primop _tmp191;if((((union Cyc_YYSTYPE*)_tmp190)->YY6).tag == 12){_LL1: _tmp191=(_tmp190->YY6).val;_LL2: {enum Cyc_Absyn_Primop yy=_tmp191;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1034
static union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp664;(_tmp664.YY6).tag=12U,(_tmp664.YY6).val=yy1;_tmp664;});}static char _tmp195[19U]="opt_t<primop_t,`H>";
# 1028 "parse.y"
static struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp195,_tmp195,_tmp195 + 19U};
union Cyc_YYSTYPE*_tmp193=yy1;struct Cyc_Core_Opt*_tmp194;if((((union Cyc_YYSTYPE*)_tmp193)->YY7).tag == 13){_LL1: _tmp194=(_tmp193->YY7).val;_LL2: {struct Cyc_Core_Opt*yy=_tmp194;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1035
static union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp665;(_tmp665.YY7).tag=13U,(_tmp665.YY7).val=yy1;_tmp665;});}static char _tmp198[7U]="qvar_t";
# 1029 "parse.y"
static struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp198,_tmp198,_tmp198 + 7U};
union Cyc_YYSTYPE*_tmp196=yy1;struct _tuple0*_tmp197;if((((union Cyc_YYSTYPE*)_tmp196)->QualId_tok).tag == 4){_LL1: _tmp197=(_tmp196->QualId_tok).val;_LL2: {struct _tuple0*yy=_tmp197;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1036
static union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp666;(_tmp666.QualId_tok).tag=4U,(_tmp666.QualId_tok).val=yy1;_tmp666;});}static char _tmp19B[7U]="stmt_t";
# 1032 "parse.y"
static struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19B,_tmp19B,_tmp19B + 7U};
union Cyc_YYSTYPE*_tmp199=yy1;struct Cyc_Absyn_Stmt*_tmp19A;if((((union Cyc_YYSTYPE*)_tmp199)->Stmt_tok).tag == 6){_LL1: _tmp19A=(_tmp199->Stmt_tok).val;_LL2: {struct Cyc_Absyn_Stmt*yy=_tmp19A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1039
static union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp667;(_tmp667.Stmt_tok).tag=6U,(_tmp667.Stmt_tok).val=yy1;_tmp667;});}static char _tmp19E[27U]="list_t<switch_clause_t,`H>";
# 1035 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp19E,_tmp19E,_tmp19E + 27U};
union Cyc_YYSTYPE*_tmp19C=yy1;struct Cyc_List_List*_tmp19D;if((((union Cyc_YYSTYPE*)_tmp19C)->YY8).tag == 14){_LL1: _tmp19D=(_tmp19C->YY8).val;_LL2: {struct Cyc_List_List*yy=_tmp19D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1042
static union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp668;(_tmp668.YY8).tag=14U,(_tmp668.YY8).val=yy1;_tmp668;});}static char _tmp1A1[6U]="pat_t";
# 1036 "parse.y"
static struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A1,_tmp1A1,_tmp1A1 + 6U};
union Cyc_YYSTYPE*_tmp19F=yy1;struct Cyc_Absyn_Pat*_tmp1A0;if((((union Cyc_YYSTYPE*)_tmp19F)->YY9).tag == 15){_LL1: _tmp1A0=(_tmp19F->YY9).val;_LL2: {struct Cyc_Absyn_Pat*yy=_tmp1A0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1043
static union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp669;(_tmp669.YY9).tag=15U,(_tmp669.YY9).val=yy1;_tmp669;});}static char _tmp1A4[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1041 "parse.y"
static struct _tuple23*Cyc_yyget_YY10(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A4,_tmp1A4,_tmp1A4 + 28U};
union Cyc_YYSTYPE*_tmp1A2=yy1;struct _tuple23*_tmp1A3;if((((union Cyc_YYSTYPE*)_tmp1A2)->YY10).tag == 16){_LL1: _tmp1A3=(_tmp1A2->YY10).val;_LL2: {struct _tuple23*yy=_tmp1A3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1048
static union Cyc_YYSTYPE Cyc_YY10(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp66A;(_tmp66A.YY10).tag=16U,(_tmp66A.YY10).val=yy1;_tmp66A;});}static char _tmp1A7[17U]="list_t<pat_t,`H>";
# 1042 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1A7,_tmp1A7,_tmp1A7 + 17U};
union Cyc_YYSTYPE*_tmp1A5=yy1;struct Cyc_List_List*_tmp1A6;if((((union Cyc_YYSTYPE*)_tmp1A5)->YY11).tag == 17){_LL1: _tmp1A6=(_tmp1A5->YY11).val;_LL2: {struct Cyc_List_List*yy=_tmp1A6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1049
static union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp66B;(_tmp66B.YY11).tag=17U,(_tmp66B.YY11).val=yy1;_tmp66B;});}static char _tmp1AA[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1043 "parse.y"
static struct _tuple24*Cyc_yyget_YY12(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AA,_tmp1AA,_tmp1AA + 36U};
union Cyc_YYSTYPE*_tmp1A8=yy1;struct _tuple24*_tmp1A9;if((((union Cyc_YYSTYPE*)_tmp1A8)->YY12).tag == 18){_LL1: _tmp1A9=(_tmp1A8->YY12).val;_LL2: {struct _tuple24*yy=_tmp1A9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1050
static union Cyc_YYSTYPE Cyc_YY12(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp66C;(_tmp66C.YY12).tag=18U,(_tmp66C.YY12).val=yy1;_tmp66C;});}static char _tmp1AD[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1044 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1AD,_tmp1AD,_tmp1AD + 47U};
union Cyc_YYSTYPE*_tmp1AB=yy1;struct Cyc_List_List*_tmp1AC;if((((union Cyc_YYSTYPE*)_tmp1AB)->YY13).tag == 19){_LL1: _tmp1AC=(_tmp1AB->YY13).val;_LL2: {struct Cyc_List_List*yy=_tmp1AC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1051
static union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp66D;(_tmp66D.YY13).tag=19U,(_tmp66D.YY13).val=yy1;_tmp66D;});}static char _tmp1B0[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1045 "parse.y"
static struct _tuple23*Cyc_yyget_YY14(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B0,_tmp1B0,_tmp1B0 + 58U};
union Cyc_YYSTYPE*_tmp1AE=yy1;struct _tuple23*_tmp1AF;if((((union Cyc_YYSTYPE*)_tmp1AE)->YY14).tag == 20){_LL1: _tmp1AF=(_tmp1AE->YY14).val;_LL2: {struct _tuple23*yy=_tmp1AF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1052
static union Cyc_YYSTYPE Cyc_YY14(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp66E;(_tmp66E.YY14).tag=20U,(_tmp66E.YY14).val=yy1;_tmp66E;});}static char _tmp1B3[9U]="fndecl_t";
# 1046 "parse.y"
static struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B3,_tmp1B3,_tmp1B3 + 9U};
union Cyc_YYSTYPE*_tmp1B1=yy1;struct Cyc_Absyn_Fndecl*_tmp1B2;if((((union Cyc_YYSTYPE*)_tmp1B1)->YY15).tag == 21){_LL1: _tmp1B2=(_tmp1B1->YY15).val;_LL2: {struct Cyc_Absyn_Fndecl*yy=_tmp1B2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1053
static union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp66F;(_tmp66F.YY15).tag=21U,(_tmp66F.YY15).val=yy1;_tmp66F;});}static char _tmp1B6[18U]="list_t<decl_t,`H>";
# 1047 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B6,_tmp1B6,_tmp1B6 + 18U};
union Cyc_YYSTYPE*_tmp1B4=yy1;struct Cyc_List_List*_tmp1B5;if((((union Cyc_YYSTYPE*)_tmp1B4)->YY16).tag == 22){_LL1: _tmp1B5=(_tmp1B4->YY16).val;_LL2: {struct Cyc_List_List*yy=_tmp1B5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1054
static union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp670;(_tmp670.YY16).tag=22U,(_tmp670.YY16).val=yy1;_tmp670;});}static char _tmp1B9[12U]="decl_spec_t";
# 1050 "parse.y"
static struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1B9,_tmp1B9,_tmp1B9 + 12U};
union Cyc_YYSTYPE*_tmp1B7=yy1;struct Cyc_Parse_Declaration_spec _tmp1B8;if((((union Cyc_YYSTYPE*)_tmp1B7)->YY17).tag == 23){_LL1: _tmp1B8=(_tmp1B7->YY17).val;_LL2: {struct Cyc_Parse_Declaration_spec yy=_tmp1B8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1057
static union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp671;(_tmp671.YY17).tag=23U,(_tmp671.YY17).val=yy1;_tmp671;});}static char _tmp1BC[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1051 "parse.y"
static struct _tuple12 Cyc_yyget_YY18(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BC,_tmp1BC,_tmp1BC + 31U};
union Cyc_YYSTYPE*_tmp1BA=yy1;struct _tuple12 _tmp1BB;if((((union Cyc_YYSTYPE*)_tmp1BA)->YY18).tag == 24){_LL1: _tmp1BB=(_tmp1BA->YY18).val;_LL2: {struct _tuple12 yy=_tmp1BB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1058
static union Cyc_YYSTYPE Cyc_YY18(struct _tuple12 yy1){return({union Cyc_YYSTYPE _tmp672;(_tmp672.YY18).tag=24U,(_tmp672.YY18).val=yy1;_tmp672;});}static char _tmp1BF[23U]="declarator_list_t<`yy>";
# 1052 "parse.y"
static struct _tuple11*Cyc_yyget_YY19(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1BF,_tmp1BF,_tmp1BF + 23U};
union Cyc_YYSTYPE*_tmp1BD=yy1;struct _tuple11*_tmp1BE;if((((union Cyc_YYSTYPE*)_tmp1BD)->YY19).tag == 25){_LL1: _tmp1BE=(_tmp1BD->YY19).val;_LL2: {struct _tuple11*yy=_tmp1BE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1059
static union Cyc_YYSTYPE Cyc_YY19(struct _tuple11*yy1){return({union Cyc_YYSTYPE _tmp673;(_tmp673.YY19).tag=25U,(_tmp673.YY19).val=yy1;_tmp673;});}static char _tmp1C2[16U]="storage_class_t";
# 1053 "parse.y"
static enum Cyc_Parse_Storage_class Cyc_yyget_YY20(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C2,_tmp1C2,_tmp1C2 + 16U};
union Cyc_YYSTYPE*_tmp1C0=yy1;enum Cyc_Parse_Storage_class _tmp1C1;if((((union Cyc_YYSTYPE*)_tmp1C0)->YY20).tag == 26){_LL1: _tmp1C1=(_tmp1C0->YY20).val;_LL2: {enum Cyc_Parse_Storage_class yy=_tmp1C1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1060
static union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class yy1){return({union Cyc_YYSTYPE _tmp674;(_tmp674.YY20).tag=26U,(_tmp674.YY20).val=yy1;_tmp674;});}static char _tmp1C5[17U]="type_specifier_t";
# 1054 "parse.y"
static struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C5,_tmp1C5,_tmp1C5 + 17U};
union Cyc_YYSTYPE*_tmp1C3=yy1;struct Cyc_Parse_Type_specifier _tmp1C4;if((((union Cyc_YYSTYPE*)_tmp1C3)->YY21).tag == 27){_LL1: _tmp1C4=(_tmp1C3->YY21).val;_LL2: {struct Cyc_Parse_Type_specifier yy=_tmp1C4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1061
static union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp675;(_tmp675.YY21).tag=27U,(_tmp675.YY21).val=yy1;_tmp675;});}static char _tmp1C8[12U]="aggr_kind_t";
# 1056 "parse.y"
static enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1C8,_tmp1C8,_tmp1C8 + 12U};
union Cyc_YYSTYPE*_tmp1C6=yy1;enum Cyc_Absyn_AggrKind _tmp1C7;if((((union Cyc_YYSTYPE*)_tmp1C6)->YY22).tag == 28){_LL1: _tmp1C7=(_tmp1C6->YY22).val;_LL2: {enum Cyc_Absyn_AggrKind yy=_tmp1C7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1063
static union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp676;(_tmp676.YY22).tag=28U,(_tmp676.YY22).val=yy1;_tmp676;});}static char _tmp1CB[20U]="$(bool,aggr_kind_t)";
# 1057 "parse.y"
static struct _tuple25 Cyc_yyget_YY23(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CB,_tmp1CB,_tmp1CB + 20U};
union Cyc_YYSTYPE*_tmp1C9=yy1;struct _tuple25 _tmp1CA;if((((union Cyc_YYSTYPE*)_tmp1C9)->YY23).tag == 29){_LL1: _tmp1CA=(_tmp1C9->YY23).val;_LL2: {struct _tuple25 yy=_tmp1CA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1064
static union Cyc_YYSTYPE Cyc_YY23(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp677;(_tmp677.YY23).tag=29U,(_tmp677.YY23).val=yy1;_tmp677;});}static char _tmp1CE[8U]="tqual_t";
# 1058 "parse.y"
static struct Cyc_Absyn_Tqual Cyc_yyget_YY24(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1CE,_tmp1CE,_tmp1CE + 8U};
union Cyc_YYSTYPE*_tmp1CC=yy1;struct Cyc_Absyn_Tqual _tmp1CD;if((((union Cyc_YYSTYPE*)_tmp1CC)->YY24).tag == 30){_LL1: _tmp1CD=(_tmp1CC->YY24).val;_LL2: {struct Cyc_Absyn_Tqual yy=_tmp1CD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1065
static union Cyc_YYSTYPE Cyc_YY24(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp678;(_tmp678.YY24).tag=30U,(_tmp678.YY24).val=yy1;_tmp678;});}static char _tmp1D1[23U]="list_t<aggrfield_t,`H>";
# 1059 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D1,_tmp1D1,_tmp1D1 + 23U};
union Cyc_YYSTYPE*_tmp1CF=yy1;struct Cyc_List_List*_tmp1D0;if((((union Cyc_YYSTYPE*)_tmp1CF)->YY25).tag == 31){_LL1: _tmp1D0=(_tmp1CF->YY25).val;_LL2: {struct Cyc_List_List*yy=_tmp1D0;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1066
static union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp679;(_tmp679.YY25).tag=31U,(_tmp679.YY25).val=yy1;_tmp679;});}static char _tmp1D4[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1060 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D4,_tmp1D4,_tmp1D4 + 34U};
union Cyc_YYSTYPE*_tmp1D2=yy1;struct Cyc_List_List*_tmp1D3;if((((union Cyc_YYSTYPE*)_tmp1D2)->YY26).tag == 32){_LL1: _tmp1D3=(_tmp1D2->YY26).val;_LL2: {struct Cyc_List_List*yy=_tmp1D3;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1067
static union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp67A;(_tmp67A.YY26).tag=32U,(_tmp67A.YY26).val=yy1;_tmp67A;});}static char _tmp1D7[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1061 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY27(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1D7,_tmp1D7,_tmp1D7 + 33U};
union Cyc_YYSTYPE*_tmp1D5=yy1;struct Cyc_List_List*_tmp1D6;if((((union Cyc_YYSTYPE*)_tmp1D5)->YY27).tag == 33){_LL1: _tmp1D6=(_tmp1D5->YY27).val;_LL2: {struct Cyc_List_List*yy=_tmp1D6;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1068
static union Cyc_YYSTYPE Cyc_YY27(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp67B;(_tmp67B.YY27).tag=33U,(_tmp67B.YY27).val=yy1;_tmp67B;});}static char _tmp1DA[18U]="declarator_t<`yy>";
# 1062 "parse.y"
static struct Cyc_Parse_Declarator Cyc_yyget_YY28(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DA,_tmp1DA,_tmp1DA + 18U};
union Cyc_YYSTYPE*_tmp1D8=yy1;struct Cyc_Parse_Declarator _tmp1D9;if((((union Cyc_YYSTYPE*)_tmp1D8)->YY28).tag == 34){_LL1: _tmp1D9=(_tmp1D8->YY28).val;_LL2: {struct Cyc_Parse_Declarator yy=_tmp1D9;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1069
static union Cyc_YYSTYPE Cyc_YY28(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp67C;(_tmp67C.YY28).tag=34U,(_tmp67C.YY28).val=yy1;_tmp67C;});}static char _tmp1DD[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1063 "parse.y"
static struct _tuple26*Cyc_yyget_YY29(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1DD,_tmp1DD,_tmp1DD + 45U};
union Cyc_YYSTYPE*_tmp1DB=yy1;struct _tuple26*_tmp1DC;if((((union Cyc_YYSTYPE*)_tmp1DB)->YY29).tag == 35){_LL1: _tmp1DC=(_tmp1DB->YY29).val;_LL2: {struct _tuple26*yy=_tmp1DC;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1070
static union Cyc_YYSTYPE Cyc_YY29(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp67D;(_tmp67D.YY29).tag=35U,(_tmp67D.YY29).val=yy1;_tmp67D;});}static char _tmp1E0[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1064 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY30(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E0,_tmp1E0,_tmp1E0 + 57U};
union Cyc_YYSTYPE*_tmp1DE=yy1;struct Cyc_List_List*_tmp1DF;if((((union Cyc_YYSTYPE*)_tmp1DE)->YY30).tag == 36){_LL1: _tmp1DF=(_tmp1DE->YY30).val;_LL2: {struct Cyc_List_List*yy=_tmp1DF;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1071
static union Cyc_YYSTYPE Cyc_YY30(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp67E;(_tmp67E.YY30).tag=36U,(_tmp67E.YY30).val=yy1;_tmp67E;});}static char _tmp1E3[26U]="abstractdeclarator_t<`yy>";
# 1065 "parse.y"
static struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY31(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E3,_tmp1E3,_tmp1E3 + 26U};
union Cyc_YYSTYPE*_tmp1E1=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1E2;if((((union Cyc_YYSTYPE*)_tmp1E1)->YY31).tag == 37){_LL1: _tmp1E2=(_tmp1E1->YY31).val;_LL2: {struct Cyc_Parse_Abstractdeclarator yy=_tmp1E2;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1072
static union Cyc_YYSTYPE Cyc_YY31(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp67F;(_tmp67F.YY31).tag=37U,(_tmp67F.YY31).val=yy1;_tmp67F;});}static char _tmp1E6[5U]="bool";
# 1066 "parse.y"
static int Cyc_yyget_YY32(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E6,_tmp1E6,_tmp1E6 + 5U};
union Cyc_YYSTYPE*_tmp1E4=yy1;int _tmp1E5;if((((union Cyc_YYSTYPE*)_tmp1E4)->YY32).tag == 38){_LL1: _tmp1E5=(_tmp1E4->YY32).val;_LL2: {int yy=_tmp1E5;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1073
static union Cyc_YYSTYPE Cyc_YY32(int yy1){return({union Cyc_YYSTYPE _tmp680;(_tmp680.YY32).tag=38U,(_tmp680.YY32).val=yy1;_tmp680;});}static char _tmp1E9[8U]="scope_t";
# 1067 "parse.y"
static enum Cyc_Absyn_Scope Cyc_yyget_YY33(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1E9,_tmp1E9,_tmp1E9 + 8U};
union Cyc_YYSTYPE*_tmp1E7=yy1;enum Cyc_Absyn_Scope _tmp1E8;if((((union Cyc_YYSTYPE*)_tmp1E7)->YY33).tag == 39){_LL1: _tmp1E8=(_tmp1E7->YY33).val;_LL2: {enum Cyc_Absyn_Scope yy=_tmp1E8;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1074
static union Cyc_YYSTYPE Cyc_YY33(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp681;(_tmp681.YY33).tag=39U,(_tmp681.YY33).val=yy1;_tmp681;});}static char _tmp1EC[16U]="datatypefield_t";
# 1068 "parse.y"
static struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY34(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EC,_tmp1EC,_tmp1EC + 16U};
union Cyc_YYSTYPE*_tmp1EA=yy1;struct Cyc_Absyn_Datatypefield*_tmp1EB;if((((union Cyc_YYSTYPE*)_tmp1EA)->YY34).tag == 40){_LL1: _tmp1EB=(_tmp1EA->YY34).val;_LL2: {struct Cyc_Absyn_Datatypefield*yy=_tmp1EB;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1075
static union Cyc_YYSTYPE Cyc_YY34(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp682;(_tmp682.YY34).tag=40U,(_tmp682.YY34).val=yy1;_tmp682;});}static char _tmp1EF[27U]="list_t<datatypefield_t,`H>";
# 1069 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY35(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1EF,_tmp1EF,_tmp1EF + 27U};
union Cyc_YYSTYPE*_tmp1ED=yy1;struct Cyc_List_List*_tmp1EE;if((((union Cyc_YYSTYPE*)_tmp1ED)->YY35).tag == 41){_LL1: _tmp1EE=(_tmp1ED->YY35).val;_LL2: {struct Cyc_List_List*yy=_tmp1EE;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1076
static union Cyc_YYSTYPE Cyc_YY35(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp683;(_tmp683.YY35).tag=41U,(_tmp683.YY35).val=yy1;_tmp683;});}static char _tmp1F2[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1070 "parse.y"
static struct _tuple27 Cyc_yyget_YY36(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F2,_tmp1F2,_tmp1F2 + 41U};
union Cyc_YYSTYPE*_tmp1F0=yy1;struct _tuple27 _tmp1F1;if((((union Cyc_YYSTYPE*)_tmp1F0)->YY36).tag == 42){_LL1: _tmp1F1=(_tmp1F0->YY36).val;_LL2: {struct _tuple27 yy=_tmp1F1;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1077
static union Cyc_YYSTYPE Cyc_YY36(struct _tuple27 yy1){return({union Cyc_YYSTYPE _tmp684;(_tmp684.YY36).tag=42U,(_tmp684.YY36).val=yy1;_tmp684;});}static char _tmp1F5[17U]="list_t<var_t,`H>";
# 1071 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY37(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F5,_tmp1F5,_tmp1F5 + 17U};
union Cyc_YYSTYPE*_tmp1F3=yy1;struct Cyc_List_List*_tmp1F4;if((((union Cyc_YYSTYPE*)_tmp1F3)->YY37).tag == 43){_LL1: _tmp1F4=(_tmp1F3->YY37).val;_LL2: {struct Cyc_List_List*yy=_tmp1F4;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1078
static union Cyc_YYSTYPE Cyc_YY37(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp685;(_tmp685.YY37).tag=43U,(_tmp685.YY37).val=yy1;_tmp685;});}static char _tmp1F8[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1072 "parse.y"
static struct _tuple8*Cyc_yyget_YY38(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1F8,_tmp1F8,_tmp1F8 + 31U};
union Cyc_YYSTYPE*_tmp1F6=yy1;struct _tuple8*_tmp1F7;if((((union Cyc_YYSTYPE*)_tmp1F6)->YY38).tag == 44){_LL1: _tmp1F7=(_tmp1F6->YY38).val;_LL2: {struct _tuple8*yy=_tmp1F7;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1079
static union Cyc_YYSTYPE Cyc_YY38(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp686;(_tmp686.YY38).tag=44U,(_tmp686.YY38).val=yy1;_tmp686;});}static char _tmp1FB[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1073 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY39(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FB,_tmp1FB,_tmp1FB + 42U};
union Cyc_YYSTYPE*_tmp1F9=yy1;struct Cyc_List_List*_tmp1FA;if((((union Cyc_YYSTYPE*)_tmp1F9)->YY39).tag == 45){_LL1: _tmp1FA=(_tmp1F9->YY39).val;_LL2: {struct Cyc_List_List*yy=_tmp1FA;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1080
static union Cyc_YYSTYPE Cyc_YY39(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp687;(_tmp687.YY39).tag=45U,(_tmp687.YY39).val=yy1;_tmp687;});}static char _tmp1FE[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1074 "parse.y"
static struct _tuple28*Cyc_yyget_YY40(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp1FE,_tmp1FE,_tmp1FE + 115U};
union Cyc_YYSTYPE*_tmp1FC=yy1;struct _tuple28*_tmp1FD;if((((union Cyc_YYSTYPE*)_tmp1FC)->YY40).tag == 46){_LL1: _tmp1FD=(_tmp1FC->YY40).val;_LL2: {struct _tuple28*yy=_tmp1FD;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1081
static union Cyc_YYSTYPE Cyc_YY40(struct _tuple28*yy1){return({union Cyc_YYSTYPE _tmp688;(_tmp688.YY40).tag=46U,(_tmp688.YY40).val=yy1;_tmp688;});}static char _tmp201[8U]="types_t";
# 1075 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp201,_tmp201,_tmp201 + 8U};
union Cyc_YYSTYPE*_tmp1FF=yy1;struct Cyc_List_List*_tmp200;if((((union Cyc_YYSTYPE*)_tmp1FF)->YY41).tag == 47){_LL1: _tmp200=(_tmp1FF->YY41).val;_LL2: {struct Cyc_List_List*yy=_tmp200;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1082
static union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp689;(_tmp689.YY41).tag=47U,(_tmp689.YY41).val=yy1;_tmp689;});}static char _tmp204[24U]="list_t<designator_t,`H>";
# 1077 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY42(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp204,_tmp204,_tmp204 + 24U};
union Cyc_YYSTYPE*_tmp202=yy1;struct Cyc_List_List*_tmp203;if((((union Cyc_YYSTYPE*)_tmp202)->YY42).tag == 48){_LL1: _tmp203=(_tmp202->YY42).val;_LL2: {struct Cyc_List_List*yy=_tmp203;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1084
static union Cyc_YYSTYPE Cyc_YY42(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp68A;(_tmp68A.YY42).tag=48U,(_tmp68A.YY42).val=yy1;_tmp68A;});}static char _tmp207[13U]="designator_t";
# 1078 "parse.y"
static void*Cyc_yyget_YY43(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp207,_tmp207,_tmp207 + 13U};
union Cyc_YYSTYPE*_tmp205=yy1;void*_tmp206;if((((union Cyc_YYSTYPE*)_tmp205)->YY43).tag == 49){_LL1: _tmp206=(_tmp205->YY43).val;_LL2: {void*yy=_tmp206;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1085
static union Cyc_YYSTYPE Cyc_YY43(void*yy1){return({union Cyc_YYSTYPE _tmp68B;(_tmp68B.YY43).tag=49U,(_tmp68B.YY43).val=yy1;_tmp68B;});}static char _tmp20A[7U]="kind_t";
# 1079 "parse.y"
static struct Cyc_Absyn_Kind*Cyc_yyget_YY44(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20A,_tmp20A,_tmp20A + 7U};
union Cyc_YYSTYPE*_tmp208=yy1;struct Cyc_Absyn_Kind*_tmp209;if((((union Cyc_YYSTYPE*)_tmp208)->YY44).tag == 50){_LL1: _tmp209=(_tmp208->YY44).val;_LL2: {struct Cyc_Absyn_Kind*yy=_tmp209;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1086
static union Cyc_YYSTYPE Cyc_YY44(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp68C;(_tmp68C.YY44).tag=50U,(_tmp68C.YY44).val=yy1;_tmp68C;});}static char _tmp20D[7U]="type_t";
# 1080 "parse.y"
static void*Cyc_yyget_YY45(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp20D,_tmp20D,_tmp20D + 7U};
union Cyc_YYSTYPE*_tmp20B=yy1;void*_tmp20C;if((((union Cyc_YYSTYPE*)_tmp20B)->YY45).tag == 51){_LL1: _tmp20C=(_tmp20B->YY45).val;_LL2: {void*yy=_tmp20C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1087
static union Cyc_YYSTYPE Cyc_YY45(void*yy1){return({union Cyc_YYSTYPE _tmp68D;(_tmp68D.YY45).tag=51U,(_tmp68D.YY45).val=yy1;_tmp68D;});}static char _tmp210[23U]="list_t<attribute_t,`H>";
# 1081 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY46(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp210,_tmp210,_tmp210 + 23U};
union Cyc_YYSTYPE*_tmp20E=yy1;struct Cyc_List_List*_tmp20F;if((((union Cyc_YYSTYPE*)_tmp20E)->YY46).tag == 52){_LL1: _tmp20F=(_tmp20E->YY46).val;_LL2: {struct Cyc_List_List*yy=_tmp20F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1088
static union Cyc_YYSTYPE Cyc_YY46(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp68E;(_tmp68E.YY46).tag=52U,(_tmp68E.YY46).val=yy1;_tmp68E;});}static char _tmp213[12U]="attribute_t";
# 1082 "parse.y"
static void*Cyc_yyget_YY47(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp213,_tmp213,_tmp213 + 12U};
union Cyc_YYSTYPE*_tmp211=yy1;void*_tmp212;if((((union Cyc_YYSTYPE*)_tmp211)->YY47).tag == 53){_LL1: _tmp212=(_tmp211->YY47).val;_LL2: {void*yy=_tmp212;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1089
static union Cyc_YYSTYPE Cyc_YY47(void*yy1){return({union Cyc_YYSTYPE _tmp68F;(_tmp68F.YY47).tag=53U,(_tmp68F.YY47).val=yy1;_tmp68F;});}static char _tmp216[12U]="enumfield_t";
# 1083 "parse.y"
static struct Cyc_Absyn_Enumfield*Cyc_yyget_YY48(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp216,_tmp216,_tmp216 + 12U};
union Cyc_YYSTYPE*_tmp214=yy1;struct Cyc_Absyn_Enumfield*_tmp215;if((((union Cyc_YYSTYPE*)_tmp214)->YY48).tag == 54){_LL1: _tmp215=(_tmp214->YY48).val;_LL2: {struct Cyc_Absyn_Enumfield*yy=_tmp215;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1090
static union Cyc_YYSTYPE Cyc_YY48(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp690;(_tmp690.YY48).tag=54U,(_tmp690.YY48).val=yy1;_tmp690;});}static char _tmp219[23U]="list_t<enumfield_t,`H>";
# 1084 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY49(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp219,_tmp219,_tmp219 + 23U};
union Cyc_YYSTYPE*_tmp217=yy1;struct Cyc_List_List*_tmp218;if((((union Cyc_YYSTYPE*)_tmp217)->YY49).tag == 55){_LL1: _tmp218=(_tmp217->YY49).val;_LL2: {struct Cyc_List_List*yy=_tmp218;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1091
static union Cyc_YYSTYPE Cyc_YY49(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp691;(_tmp691.YY49).tag=55U,(_tmp691.YY49).val=yy1;_tmp691;});}static char _tmp21C[11U]="type_opt_t";
# 1085 "parse.y"
static void*Cyc_yyget_YY50(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21C,_tmp21C,_tmp21C + 11U};
union Cyc_YYSTYPE*_tmp21A=yy1;void*_tmp21B;if((((union Cyc_YYSTYPE*)_tmp21A)->YY50).tag == 56){_LL1: _tmp21B=(_tmp21A->YY50).val;_LL2: {void*yy=_tmp21B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1092
static union Cyc_YYSTYPE Cyc_YY50(void*yy1){return({union Cyc_YYSTYPE _tmp692;(_tmp692.YY50).tag=56U,(_tmp692.YY50).val=yy1;_tmp692;});}static char _tmp21F[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1086 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY51(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp21F,_tmp21F,_tmp21F + 31U};
union Cyc_YYSTYPE*_tmp21D=yy1;struct Cyc_List_List*_tmp21E;if((((union Cyc_YYSTYPE*)_tmp21D)->YY51).tag == 57){_LL1: _tmp21E=(_tmp21D->YY51).val;_LL2: {struct Cyc_List_List*yy=_tmp21E;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1093
static union Cyc_YYSTYPE Cyc_YY51(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp693;(_tmp693.YY51).tag=57U,(_tmp693.YY51).val=yy1;_tmp693;});}static char _tmp222[11U]="booltype_t";
# 1087 "parse.y"
static void*Cyc_yyget_YY52(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp222,_tmp222,_tmp222 + 11U};
union Cyc_YYSTYPE*_tmp220=yy1;void*_tmp221;if((((union Cyc_YYSTYPE*)_tmp220)->YY52).tag == 58){_LL1: _tmp221=(_tmp220->YY52).val;_LL2: {void*yy=_tmp221;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1094
static union Cyc_YYSTYPE Cyc_YY52(void*yy1){return({union Cyc_YYSTYPE _tmp694;(_tmp694.YY52).tag=58U,(_tmp694.YY52).val=yy1;_tmp694;});}static char _tmp225[35U]="list_t<$(seg_t,qvar_t,bool)@`H,`H>";
# 1088 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY53(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp225,_tmp225,_tmp225 + 35U};
union Cyc_YYSTYPE*_tmp223=yy1;struct Cyc_List_List*_tmp224;if((((union Cyc_YYSTYPE*)_tmp223)->YY53).tag == 59){_LL1: _tmp224=(_tmp223->YY53).val;_LL2: {struct Cyc_List_List*yy=_tmp224;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1095
static union Cyc_YYSTYPE Cyc_YY53(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp695;(_tmp695.YY53).tag=59U,(_tmp695.YY53).val=yy1;_tmp695;});}static char _tmp228[48U]="$(list_t<$(seg_t,qvar_t,bool)@`H,`H>, seg_t)@`H";
# 1089 "parse.y"
static struct _tuple29*Cyc_yyget_YY54(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp228,_tmp228,_tmp228 + 48U};
union Cyc_YYSTYPE*_tmp226=yy1;struct _tuple29*_tmp227;if((((union Cyc_YYSTYPE*)_tmp226)->YY54).tag == 60){_LL1: _tmp227=(_tmp226->YY54).val;_LL2: {struct _tuple29*yy=_tmp227;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1096
static union Cyc_YYSTYPE Cyc_YY54(struct _tuple29*yy1){return({union Cyc_YYSTYPE _tmp696;(_tmp696.YY54).tag=60U,(_tmp696.YY54).val=yy1;_tmp696;});}static char _tmp22B[18U]="list_t<qvar_t,`H>";
# 1090 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY55(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22B,_tmp22B,_tmp22B + 18U};
union Cyc_YYSTYPE*_tmp229=yy1;struct Cyc_List_List*_tmp22A;if((((union Cyc_YYSTYPE*)_tmp229)->YY55).tag == 61){_LL1: _tmp22A=(_tmp229->YY55).val;_LL2: {struct Cyc_List_List*yy=_tmp22A;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1097
static union Cyc_YYSTYPE Cyc_YY55(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp697;(_tmp697.YY55).tag=61U,(_tmp697.YY55).val=yy1;_tmp697;});}static char _tmp22E[20U]="pointer_qual_t<`yy>";
# 1091 "parse.y"
static void*Cyc_yyget_YY56(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp22E,_tmp22E,_tmp22E + 20U};
union Cyc_YYSTYPE*_tmp22C=yy1;void*_tmp22D;if((((union Cyc_YYSTYPE*)_tmp22C)->YY56).tag == 62){_LL1: _tmp22D=(_tmp22C->YY56).val;_LL2: {void*yy=_tmp22D;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1098
static union Cyc_YYSTYPE Cyc_YY56(void*yy1){return({union Cyc_YYSTYPE _tmp698;(_tmp698.YY56).tag=62U,(_tmp698.YY56).val=yy1;_tmp698;});}static char _tmp231[21U]="pointer_quals_t<`yy>";
# 1092 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY57(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp231,_tmp231,_tmp231 + 21U};
union Cyc_YYSTYPE*_tmp22F=yy1;struct Cyc_List_List*_tmp230;if((((union Cyc_YYSTYPE*)_tmp22F)->YY57).tag == 63){_LL1: _tmp230=(_tmp22F->YY57).val;_LL2: {struct Cyc_List_List*yy=_tmp230;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1099
static union Cyc_YYSTYPE Cyc_YY57(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp699;(_tmp699.YY57).tag=63U,(_tmp699.YY57).val=yy1;_tmp699;});}static char _tmp234[10U]="exp_opt_t";
# 1093 "parse.y"
static struct Cyc_Absyn_Exp*Cyc_yyget_YY58(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp234,_tmp234,_tmp234 + 10U};
union Cyc_YYSTYPE*_tmp232=yy1;struct Cyc_Absyn_Exp*_tmp233;if((((union Cyc_YYSTYPE*)_tmp232)->YY58).tag == 64){_LL1: _tmp233=(_tmp232->YY58).val;_LL2: {struct Cyc_Absyn_Exp*yy=_tmp233;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1100
static union Cyc_YYSTYPE Cyc_YY58(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp69A;(_tmp69A.YY58).tag=64U,(_tmp69A.YY58).val=yy1;_tmp69A;});}static char _tmp237[23U]="$(exp_opt_t,exp_opt_t)";
# 1094 "parse.y"
static struct _tuple17 Cyc_yyget_YY59(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp237,_tmp237,_tmp237 + 23U};
union Cyc_YYSTYPE*_tmp235=yy1;struct _tuple17 _tmp236;if((((union Cyc_YYSTYPE*)_tmp235)->YY59).tag == 65){_LL1: _tmp236=(_tmp235->YY59).val;_LL2: {struct _tuple17 yy=_tmp236;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1101
static union Cyc_YYSTYPE Cyc_YY59(struct _tuple17 yy1){return({union Cyc_YYSTYPE _tmp69B;(_tmp69B.YY59).tag=65U,(_tmp69B.YY59).val=yy1;_tmp69B;});}static char _tmp23A[10U]="raw_exp_t";
# 1095 "parse.y"
static void*Cyc_yyget_YY60(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23A,_tmp23A,_tmp23A + 10U};
union Cyc_YYSTYPE*_tmp238=yy1;void*_tmp239;if((((union Cyc_YYSTYPE*)_tmp238)->YY60).tag == 66){_LL1: _tmp239=(_tmp238->YY60).val;_LL2: {void*yy=_tmp239;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1102
static union Cyc_YYSTYPE Cyc_YY60(void*yy1){return({union Cyc_YYSTYPE _tmp69C;(_tmp69C.YY60).tag=66U,(_tmp69C.YY60).val=yy1;_tmp69C;});}static char _tmp23D[112U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1096 "parse.y"
static struct _tuple30*Cyc_yyget_YY61(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp23D,_tmp23D,_tmp23D + 112U};
union Cyc_YYSTYPE*_tmp23B=yy1;struct _tuple30*_tmp23C;if((((union Cyc_YYSTYPE*)_tmp23B)->YY61).tag == 67){_LL1: _tmp23C=(_tmp23B->YY61).val;_LL2: {struct _tuple30*yy=_tmp23C;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1103
static union Cyc_YYSTYPE Cyc_YY61(struct _tuple30*yy1){return({union Cyc_YYSTYPE _tmp69D;(_tmp69D.YY61).tag=67U,(_tmp69D.YY61).val=yy1;_tmp69D;});}static char _tmp240[73U]="$(list_t<$(string_t<`H>, exp_t)@`H, `H>, list_t<string_t<`H>@`H, `H>)@`H";
# 1097 "parse.y"
static struct _tuple31*Cyc_yyget_YY62(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp240,_tmp240,_tmp240 + 73U};
union Cyc_YYSTYPE*_tmp23E=yy1;struct _tuple31*_tmp23F;if((((union Cyc_YYSTYPE*)_tmp23E)->YY62).tag == 68){_LL1: _tmp23F=(_tmp23E->YY62).val;_LL2: {struct _tuple31*yy=_tmp23F;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1104
static union Cyc_YYSTYPE Cyc_YY62(struct _tuple31*yy1){return({union Cyc_YYSTYPE _tmp69E;(_tmp69E.YY62).tag=68U,(_tmp69E.YY62).val=yy1;_tmp69E;});}static char _tmp243[28U]="list_t<string_t<`H>@`H, `H>";
# 1098 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY63(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp243,_tmp243,_tmp243 + 28U};
union Cyc_YYSTYPE*_tmp241=yy1;struct Cyc_List_List*_tmp242;if((((union Cyc_YYSTYPE*)_tmp241)->YY63).tag == 69){_LL1: _tmp242=(_tmp241->YY63).val;_LL2: {struct Cyc_List_List*yy=_tmp242;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1105
static union Cyc_YYSTYPE Cyc_YY63(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp69F;(_tmp69F.YY63).tag=69U,(_tmp69F.YY63).val=yy1;_tmp69F;});}static char _tmp246[38U]="list_t<$(string_t<`H>, exp_t)@`H, `H>";
# 1099 "parse.y"
static struct Cyc_List_List*Cyc_yyget_YY64(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp246,_tmp246,_tmp246 + 38U};
union Cyc_YYSTYPE*_tmp244=yy1;struct Cyc_List_List*_tmp245;if((((union Cyc_YYSTYPE*)_tmp244)->YY64).tag == 70){_LL1: _tmp245=(_tmp244->YY64).val;_LL2: {struct Cyc_List_List*yy=_tmp245;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1106
static union Cyc_YYSTYPE Cyc_YY64(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6A0;(_tmp6A0.YY64).tag=70U,(_tmp6A0.YY64).val=yy1;_tmp6A0;});}static char _tmp249[26U]="$(string_t<`H>, exp_t)@`H";
# 1100 "parse.y"
static struct _tuple32*Cyc_yyget_YY65(union Cyc_YYSTYPE*yy1){
static struct _fat_ptr s={_tmp249,_tmp249,_tmp249 + 26U};
union Cyc_YYSTYPE*_tmp247=yy1;struct _tuple32*_tmp248;if((((union Cyc_YYSTYPE*)_tmp247)->YY65).tag == 71){_LL1: _tmp248=(_tmp247->YY65).val;_LL2: {struct _tuple32*yy=_tmp248;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1107
static union Cyc_YYSTYPE Cyc_YY65(struct _tuple32*yy1){return({union Cyc_YYSTYPE _tmp6A1;(_tmp6A1.YY65).tag=71U,(_tmp6A1.YY65).val=yy1;_tmp6A1;});}static char _tmp24C[16U]="exp_maker_fun_t";
# 1101 "parse.y"
static struct Cyc_Absyn_Exp*(*Cyc_yyget_YY66(union Cyc_YYSTYPE*yy1))(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned){
static struct _fat_ptr s={_tmp24C,_tmp24C,_tmp24C + 16U};
union Cyc_YYSTYPE*_tmp24A=yy1;struct Cyc_Absyn_Exp*(*_tmp24B)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);if((((union Cyc_YYSTYPE*)_tmp24A)->YY66).tag == 72){_LL1: _tmp24B=(_tmp24A->YY66).val;_LL2: {struct Cyc_Absyn_Exp*(*yy)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=_tmp24B;
return yy;}}else{_LL3: _LL4:
 Cyc_yythrowfail(s);}_LL0:;}
# 1108
static union Cyc_YYSTYPE Cyc_YY66(struct Cyc_Absyn_Exp*(*yy1)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)){return({union Cyc_YYSTYPE _tmp6A2;(_tmp6A2.YY66).tag=72U,(_tmp6A2.YY66).val=yy1;_tmp6A2;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1124
struct Cyc_Yyltype Cyc_yynewloc (void){
return({struct Cyc_Yyltype _tmp6A3;_tmp6A3.timestamp=0,_tmp6A3.first_line=0,_tmp6A3.first_column=0,_tmp6A3.last_line=0,_tmp6A3.last_column=0;_tmp6A3;});}
# 1127
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1138 "parse.y"
static short Cyc_yytranslate[381U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,152,2,2,137,150,147,2,134,135,130,144,129,148,139,149,2,2,2,2,2,2,2,2,2,2,138,126,132,131,133,143,142,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,140,2,141,146,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,127,145,128,151,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,125};static char _tmp24D[2U]="$";static char _tmp24E[6U]="error";static char _tmp24F[12U]="$undefined.";static char _tmp250[5U]="AUTO";static char _tmp251[9U]="REGISTER";static char _tmp252[7U]="STATIC";static char _tmp253[7U]="EXTERN";static char _tmp254[8U]="TYPEDEF";static char _tmp255[5U]="VOID";static char _tmp256[5U]="CHAR";static char _tmp257[6U]="SHORT";static char _tmp258[4U]="INT";static char _tmp259[5U]="LONG";static char _tmp25A[6U]="FLOAT";static char _tmp25B[7U]="DOUBLE";static char _tmp25C[7U]="SIGNED";static char _tmp25D[9U]="UNSIGNED";static char _tmp25E[6U]="CONST";static char _tmp25F[9U]="VOLATILE";static char _tmp260[9U]="RESTRICT";static char _tmp261[7U]="STRUCT";static char _tmp262[6U]="UNION";static char _tmp263[5U]="CASE";static char _tmp264[8U]="DEFAULT";static char _tmp265[7U]="INLINE";static char _tmp266[7U]="SIZEOF";static char _tmp267[9U]="OFFSETOF";static char _tmp268[3U]="IF";static char _tmp269[5U]="ELSE";static char _tmp26A[7U]="SWITCH";static char _tmp26B[6U]="WHILE";static char _tmp26C[3U]="DO";static char _tmp26D[4U]="FOR";static char _tmp26E[5U]="GOTO";static char _tmp26F[9U]="CONTINUE";static char _tmp270[6U]="BREAK";static char _tmp271[7U]="RETURN";static char _tmp272[5U]="ENUM";static char _tmp273[7U]="TYPEOF";static char _tmp274[16U]="BUILTIN_VA_LIST";static char _tmp275[10U]="EXTENSION";static char _tmp276[8U]="NULL_kw";static char _tmp277[4U]="LET";static char _tmp278[6U]="THROW";static char _tmp279[4U]="TRY";static char _tmp27A[6U]="CATCH";static char _tmp27B[7U]="EXPORT";static char _tmp27C[9U]="OVERRIDE";static char _tmp27D[5U]="HIDE";static char _tmp27E[4U]="NEW";static char _tmp27F[9U]="ABSTRACT";static char _tmp280[9U]="FALLTHRU";static char _tmp281[6U]="USING";static char _tmp282[10U]="NAMESPACE";static char _tmp283[9U]="DATATYPE";static char _tmp284[7U]="MALLOC";static char _tmp285[8U]="RMALLOC";static char _tmp286[15U]="RMALLOC_INLINE";static char _tmp287[7U]="CALLOC";static char _tmp288[8U]="RCALLOC";static char _tmp289[5U]="SWAP";static char _tmp28A[9U]="REGION_T";static char _tmp28B[6U]="TAG_T";static char _tmp28C[7U]="REGION";static char _tmp28D[5U]="RNEW";static char _tmp28E[8U]="REGIONS";static char _tmp28F[7U]="PORTON";static char _tmp290[8U]="PORTOFF";static char _tmp291[7U]="PRAGMA";static char _tmp292[10U]="TEMPESTON";static char _tmp293[11U]="TEMPESTOFF";static char _tmp294[8U]="NUMELTS";static char _tmp295[8U]="VALUEOF";static char _tmp296[10U]="VALUEOF_T";static char _tmp297[9U]="TAGCHECK";static char _tmp298[13U]="NUMELTS_QUAL";static char _tmp299[10U]="THIN_QUAL";static char _tmp29A[9U]="FAT_QUAL";static char _tmp29B[13U]="NOTNULL_QUAL";static char _tmp29C[14U]="NULLABLE_QUAL";static char _tmp29D[14U]="REQUIRES_QUAL";static char _tmp29E[13U]="ENSURES_QUAL";static char _tmp29F[12U]="REGION_QUAL";static char _tmp2A0[16U]="NOZEROTERM_QUAL";static char _tmp2A1[14U]="ZEROTERM_QUAL";static char _tmp2A2[12U]="TAGGED_QUAL";static char _tmp2A3[12U]="ASSERT_QUAL";static char _tmp2A4[16U]="EXTENSIBLE_QUAL";static char _tmp2A5[18U]="AUTORELEASED_QUAL";static char _tmp2A6[7U]="PTR_OP";static char _tmp2A7[7U]="INC_OP";static char _tmp2A8[7U]="DEC_OP";static char _tmp2A9[8U]="LEFT_OP";static char _tmp2AA[9U]="RIGHT_OP";static char _tmp2AB[6U]="LE_OP";static char _tmp2AC[6U]="GE_OP";static char _tmp2AD[6U]="EQ_OP";static char _tmp2AE[6U]="NE_OP";static char _tmp2AF[7U]="AND_OP";static char _tmp2B0[6U]="OR_OP";static char _tmp2B1[11U]="MUL_ASSIGN";static char _tmp2B2[11U]="DIV_ASSIGN";static char _tmp2B3[11U]="MOD_ASSIGN";static char _tmp2B4[11U]="ADD_ASSIGN";static char _tmp2B5[11U]="SUB_ASSIGN";static char _tmp2B6[12U]="LEFT_ASSIGN";static char _tmp2B7[13U]="RIGHT_ASSIGN";static char _tmp2B8[11U]="AND_ASSIGN";static char _tmp2B9[11U]="XOR_ASSIGN";static char _tmp2BA[10U]="OR_ASSIGN";static char _tmp2BB[9U]="ELLIPSIS";static char _tmp2BC[11U]="LEFT_RIGHT";static char _tmp2BD[12U]="COLON_COLON";static char _tmp2BE[11U]="IDENTIFIER";static char _tmp2BF[17U]="INTEGER_CONSTANT";static char _tmp2C0[7U]="STRING";static char _tmp2C1[8U]="WSTRING";static char _tmp2C2[19U]="CHARACTER_CONSTANT";static char _tmp2C3[20U]="WCHARACTER_CONSTANT";static char _tmp2C4[18U]="FLOATING_CONSTANT";static char _tmp2C5[9U]="TYPE_VAR";static char _tmp2C6[13U]="TYPEDEF_NAME";static char _tmp2C7[16U]="QUAL_IDENTIFIER";static char _tmp2C8[18U]="QUAL_TYPEDEF_NAME";static char _tmp2C9[10U]="ATTRIBUTE";static char _tmp2CA[8U]="ASM_TOK";static char _tmp2CB[4U]="';'";static char _tmp2CC[4U]="'{'";static char _tmp2CD[4U]="'}'";static char _tmp2CE[4U]="','";static char _tmp2CF[4U]="'*'";static char _tmp2D0[4U]="'='";static char _tmp2D1[4U]="'<'";static char _tmp2D2[4U]="'>'";static char _tmp2D3[4U]="'('";static char _tmp2D4[4U]="')'";static char _tmp2D5[4U]="'_'";static char _tmp2D6[4U]="'$'";static char _tmp2D7[4U]="':'";static char _tmp2D8[4U]="'.'";static char _tmp2D9[4U]="'['";static char _tmp2DA[4U]="']'";static char _tmp2DB[4U]="'@'";static char _tmp2DC[4U]="'?'";static char _tmp2DD[4U]="'+'";static char _tmp2DE[4U]="'|'";static char _tmp2DF[4U]="'^'";static char _tmp2E0[4U]="'&'";static char _tmp2E1[4U]="'-'";static char _tmp2E2[4U]="'/'";static char _tmp2E3[4U]="'%'";static char _tmp2E4[4U]="'~'";static char _tmp2E5[4U]="'!'";static char _tmp2E6[5U]="prog";static char _tmp2E7[17U]="translation_unit";static char _tmp2E8[18U]="tempest_on_action";static char _tmp2E9[19U]="tempest_off_action";static char _tmp2EA[16U]="extern_c_action";static char _tmp2EB[13U]="end_extern_c";static char _tmp2EC[14U]="hide_list_opt";static char _tmp2ED[17U]="hide_list_values";static char _tmp2EE[16U]="export_list_opt";static char _tmp2EF[12U]="export_list";static char _tmp2F0[19U]="export_list_values";static char _tmp2F1[13U]="override_opt";static char _tmp2F2[21U]="external_declaration";static char _tmp2F3[14U]="optional_semi";static char _tmp2F4[20U]="function_definition";static char _tmp2F5[21U]="function_definition2";static char _tmp2F6[13U]="using_action";static char _tmp2F7[15U]="unusing_action";static char _tmp2F8[17U]="namespace_action";static char _tmp2F9[19U]="unnamespace_action";static char _tmp2FA[12U]="declaration";static char _tmp2FB[9U]="open_opt";static char _tmp2FC[17U]="declaration_list";static char _tmp2FD[23U]="declaration_specifiers";static char _tmp2FE[24U]="storage_class_specifier";static char _tmp2FF[15U]="attributes_opt";static char _tmp300[11U]="attributes";static char _tmp301[15U]="attribute_list";static char _tmp302[10U]="attribute";static char _tmp303[15U]="type_specifier";static char _tmp304[25U]="type_specifier_notypedef";static char _tmp305[5U]="kind";static char _tmp306[15U]="type_qualifier";static char _tmp307[15U]="enum_specifier";static char _tmp308[11U]="enum_field";static char _tmp309[22U]="enum_declaration_list";static char _tmp30A[26U]="struct_or_union_specifier";static char _tmp30B[26U]="maybe_tagged_struct_union";static char _tmp30C[16U]="struct_or_union";static char _tmp30D[16U]="type_params_opt";static char _tmp30E[24U]="struct_declaration_list";static char _tmp30F[25U]="struct_declaration_list0";static char _tmp310[25U]="init_declarator_list_rev";static char _tmp311[16U]="init_declarator";static char _tmp312[19U]="struct_declaration";static char _tmp313[25U]="specifier_qualifier_list";static char _tmp314[35U]="notypedef_specifier_qualifier_list";static char _tmp315[27U]="struct_declarator_list_rev";static char _tmp316[18U]="struct_declarator";static char _tmp317[20U]="requires_clause_opt";static char _tmp318[19U]="datatype_specifier";static char _tmp319[14U]="qual_datatype";static char _tmp31A[19U]="datatypefield_list";static char _tmp31B[20U]="datatypefield_scope";static char _tmp31C[14U]="datatypefield";static char _tmp31D[11U]="declarator";static char _tmp31E[23U]="declarator_withtypedef";static char _tmp31F[18U]="direct_declarator";static char _tmp320[30U]="direct_declarator_withtypedef";static char _tmp321[8U]="pointer";static char _tmp322[12U]="one_pointer";static char _tmp323[14U]="pointer_quals";static char _tmp324[13U]="pointer_qual";static char _tmp325[23U]="pointer_null_and_bound";static char _tmp326[14U]="pointer_bound";static char _tmp327[18U]="zeroterm_qual_opt";static char _tmp328[8U]="rgn_opt";static char _tmp329[11U]="tqual_list";static char _tmp32A[20U]="parameter_type_list";static char _tmp32B[9U]="type_var";static char _tmp32C[16U]="optional_effect";static char _tmp32D[19U]="optional_rgn_order";static char _tmp32E[10U]="rgn_order";static char _tmp32F[16U]="optional_inject";static char _tmp330[11U]="effect_set";static char _tmp331[14U]="atomic_effect";static char _tmp332[11U]="region_set";static char _tmp333[15U]="parameter_list";static char _tmp334[22U]="parameter_declaration";static char _tmp335[16U]="identifier_list";static char _tmp336[17U]="identifier_list0";static char _tmp337[12U]="initializer";static char _tmp338[18U]="array_initializer";static char _tmp339[17U]="initializer_list";static char _tmp33A[12U]="designation";static char _tmp33B[16U]="designator_list";static char _tmp33C[11U]="designator";static char _tmp33D[10U]="type_name";static char _tmp33E[14U]="any_type_name";static char _tmp33F[15U]="type_name_list";static char _tmp340[20U]="abstract_declarator";static char _tmp341[27U]="direct_abstract_declarator";static char _tmp342[25U]="requires_and_ensures_opt";static char _tmp343[10U]="statement";static char _tmp344[18U]="labeled_statement";static char _tmp345[21U]="expression_statement";static char _tmp346[19U]="compound_statement";static char _tmp347[16U]="block_item_list";static char _tmp348[20U]="selection_statement";static char _tmp349[15U]="switch_clauses";static char _tmp34A[20U]="iteration_statement";static char _tmp34B[12U]="for_exp_opt";static char _tmp34C[15U]="jump_statement";static char _tmp34D[12U]="exp_pattern";static char _tmp34E[20U]="conditional_pattern";static char _tmp34F[19U]="logical_or_pattern";static char _tmp350[20U]="logical_and_pattern";static char _tmp351[21U]="inclusive_or_pattern";static char _tmp352[21U]="exclusive_or_pattern";static char _tmp353[12U]="and_pattern";static char _tmp354[17U]="equality_pattern";static char _tmp355[19U]="relational_pattern";static char _tmp356[14U]="shift_pattern";static char _tmp357[17U]="additive_pattern";static char _tmp358[23U]="multiplicative_pattern";static char _tmp359[13U]="cast_pattern";static char _tmp35A[14U]="unary_pattern";static char _tmp35B[16U]="postfix_pattern";static char _tmp35C[16U]="primary_pattern";static char _tmp35D[8U]="pattern";static char _tmp35E[19U]="tuple_pattern_list";static char _tmp35F[20U]="tuple_pattern_list0";static char _tmp360[14U]="field_pattern";static char _tmp361[19U]="field_pattern_list";static char _tmp362[20U]="field_pattern_list0";static char _tmp363[11U]="expression";static char _tmp364[22U]="assignment_expression";static char _tmp365[20U]="assignment_operator";static char _tmp366[23U]="conditional_expression";static char _tmp367[20U]="constant_expression";static char _tmp368[22U]="logical_or_expression";static char _tmp369[23U]="logical_and_expression";static char _tmp36A[24U]="inclusive_or_expression";static char _tmp36B[24U]="exclusive_or_expression";static char _tmp36C[15U]="and_expression";static char _tmp36D[20U]="equality_expression";static char _tmp36E[22U]="relational_expression";static char _tmp36F[17U]="shift_expression";static char _tmp370[20U]="additive_expression";static char _tmp371[26U]="multiplicative_expression";static char _tmp372[12U]="equality_op";static char _tmp373[14U]="relational_op";static char _tmp374[12U]="additive_op";static char _tmp375[18U]="multiplicative_op";static char _tmp376[16U]="cast_expression";static char _tmp377[17U]="unary_expression";static char _tmp378[15U]="unary_operator";static char _tmp379[9U]="asm_expr";static char _tmp37A[13U]="volatile_opt";static char _tmp37B[12U]="asm_out_opt";static char _tmp37C[12U]="asm_outlist";static char _tmp37D[11U]="asm_in_opt";static char _tmp37E[11U]="asm_inlist";static char _tmp37F[11U]="asm_io_elt";static char _tmp380[16U]="asm_clobber_opt";static char _tmp381[17U]="asm_clobber_list";static char _tmp382[19U]="postfix_expression";static char _tmp383[17U]="field_expression";static char _tmp384[19U]="primary_expression";static char _tmp385[25U]="argument_expression_list";static char _tmp386[26U]="argument_expression_list0";static char _tmp387[9U]="constant";static char _tmp388[20U]="qual_opt_identifier";static char _tmp389[17U]="qual_opt_typedef";static char _tmp38A[18U]="struct_union_name";static char _tmp38B[11U]="field_name";static char _tmp38C[12U]="right_angle";
# 1491 "parse.y"
static struct _fat_ptr Cyc_yytname[320U]={{_tmp24D,_tmp24D,_tmp24D + 2U},{_tmp24E,_tmp24E,_tmp24E + 6U},{_tmp24F,_tmp24F,_tmp24F + 12U},{_tmp250,_tmp250,_tmp250 + 5U},{_tmp251,_tmp251,_tmp251 + 9U},{_tmp252,_tmp252,_tmp252 + 7U},{_tmp253,_tmp253,_tmp253 + 7U},{_tmp254,_tmp254,_tmp254 + 8U},{_tmp255,_tmp255,_tmp255 + 5U},{_tmp256,_tmp256,_tmp256 + 5U},{_tmp257,_tmp257,_tmp257 + 6U},{_tmp258,_tmp258,_tmp258 + 4U},{_tmp259,_tmp259,_tmp259 + 5U},{_tmp25A,_tmp25A,_tmp25A + 6U},{_tmp25B,_tmp25B,_tmp25B + 7U},{_tmp25C,_tmp25C,_tmp25C + 7U},{_tmp25D,_tmp25D,_tmp25D + 9U},{_tmp25E,_tmp25E,_tmp25E + 6U},{_tmp25F,_tmp25F,_tmp25F + 9U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 7U},{_tmp262,_tmp262,_tmp262 + 6U},{_tmp263,_tmp263,_tmp263 + 5U},{_tmp264,_tmp264,_tmp264 + 8U},{_tmp265,_tmp265,_tmp265 + 7U},{_tmp266,_tmp266,_tmp266 + 7U},{_tmp267,_tmp267,_tmp267 + 9U},{_tmp268,_tmp268,_tmp268 + 3U},{_tmp269,_tmp269,_tmp269 + 5U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 6U},{_tmp26C,_tmp26C,_tmp26C + 3U},{_tmp26D,_tmp26D,_tmp26D + 4U},{_tmp26E,_tmp26E,_tmp26E + 5U},{_tmp26F,_tmp26F,_tmp26F + 9U},{_tmp270,_tmp270,_tmp270 + 6U},{_tmp271,_tmp271,_tmp271 + 7U},{_tmp272,_tmp272,_tmp272 + 5U},{_tmp273,_tmp273,_tmp273 + 7U},{_tmp274,_tmp274,_tmp274 + 16U},{_tmp275,_tmp275,_tmp275 + 10U},{_tmp276,_tmp276,_tmp276 + 8U},{_tmp277,_tmp277,_tmp277 + 4U},{_tmp278,_tmp278,_tmp278 + 6U},{_tmp279,_tmp279,_tmp279 + 4U},{_tmp27A,_tmp27A,_tmp27A + 6U},{_tmp27B,_tmp27B,_tmp27B + 7U},{_tmp27C,_tmp27C,_tmp27C + 9U},{_tmp27D,_tmp27D,_tmp27D + 5U},{_tmp27E,_tmp27E,_tmp27E + 4U},{_tmp27F,_tmp27F,_tmp27F + 9U},{_tmp280,_tmp280,_tmp280 + 9U},{_tmp281,_tmp281,_tmp281 + 6U},{_tmp282,_tmp282,_tmp282 + 10U},{_tmp283,_tmp283,_tmp283 + 9U},{_tmp284,_tmp284,_tmp284 + 7U},{_tmp285,_tmp285,_tmp285 + 8U},{_tmp286,_tmp286,_tmp286 + 15U},{_tmp287,_tmp287,_tmp287 + 7U},{_tmp288,_tmp288,_tmp288 + 8U},{_tmp289,_tmp289,_tmp289 + 5U},{_tmp28A,_tmp28A,_tmp28A + 9U},{_tmp28B,_tmp28B,_tmp28B + 6U},{_tmp28C,_tmp28C,_tmp28C + 7U},{_tmp28D,_tmp28D,_tmp28D + 5U},{_tmp28E,_tmp28E,_tmp28E + 8U},{_tmp28F,_tmp28F,_tmp28F + 7U},{_tmp290,_tmp290,_tmp290 + 8U},{_tmp291,_tmp291,_tmp291 + 7U},{_tmp292,_tmp292,_tmp292 + 10U},{_tmp293,_tmp293,_tmp293 + 11U},{_tmp294,_tmp294,_tmp294 + 8U},{_tmp295,_tmp295,_tmp295 + 8U},{_tmp296,_tmp296,_tmp296 + 10U},{_tmp297,_tmp297,_tmp297 + 9U},{_tmp298,_tmp298,_tmp298 + 13U},{_tmp299,_tmp299,_tmp299 + 10U},{_tmp29A,_tmp29A,_tmp29A + 9U},{_tmp29B,_tmp29B,_tmp29B + 13U},{_tmp29C,_tmp29C,_tmp29C + 14U},{_tmp29D,_tmp29D,_tmp29D + 14U},{_tmp29E,_tmp29E,_tmp29E + 13U},{_tmp29F,_tmp29F,_tmp29F + 12U},{_tmp2A0,_tmp2A0,_tmp2A0 + 16U},{_tmp2A1,_tmp2A1,_tmp2A1 + 14U},{_tmp2A2,_tmp2A2,_tmp2A2 + 12U},{_tmp2A3,_tmp2A3,_tmp2A3 + 12U},{_tmp2A4,_tmp2A4,_tmp2A4 + 16U},{_tmp2A5,_tmp2A5,_tmp2A5 + 18U},{_tmp2A6,_tmp2A6,_tmp2A6 + 7U},{_tmp2A7,_tmp2A7,_tmp2A7 + 7U},{_tmp2A8,_tmp2A8,_tmp2A8 + 7U},{_tmp2A9,_tmp2A9,_tmp2A9 + 8U},{_tmp2AA,_tmp2AA,_tmp2AA + 9U},{_tmp2AB,_tmp2AB,_tmp2AB + 6U},{_tmp2AC,_tmp2AC,_tmp2AC + 6U},{_tmp2AD,_tmp2AD,_tmp2AD + 6U},{_tmp2AE,_tmp2AE,_tmp2AE + 6U},{_tmp2AF,_tmp2AF,_tmp2AF + 7U},{_tmp2B0,_tmp2B0,_tmp2B0 + 6U},{_tmp2B1,_tmp2B1,_tmp2B1 + 11U},{_tmp2B2,_tmp2B2,_tmp2B2 + 11U},{_tmp2B3,_tmp2B3,_tmp2B3 + 11U},{_tmp2B4,_tmp2B4,_tmp2B4 + 11U},{_tmp2B5,_tmp2B5,_tmp2B5 + 11U},{_tmp2B6,_tmp2B6,_tmp2B6 + 12U},{_tmp2B7,_tmp2B7,_tmp2B7 + 13U},{_tmp2B8,_tmp2B8,_tmp2B8 + 11U},{_tmp2B9,_tmp2B9,_tmp2B9 + 11U},{_tmp2BA,_tmp2BA,_tmp2BA + 10U},{_tmp2BB,_tmp2BB,_tmp2BB + 9U},{_tmp2BC,_tmp2BC,_tmp2BC + 11U},{_tmp2BD,_tmp2BD,_tmp2BD + 12U},{_tmp2BE,_tmp2BE,_tmp2BE + 11U},{_tmp2BF,_tmp2BF,_tmp2BF + 17U},{_tmp2C0,_tmp2C0,_tmp2C0 + 7U},{_tmp2C1,_tmp2C1,_tmp2C1 + 8U},{_tmp2C2,_tmp2C2,_tmp2C2 + 19U},{_tmp2C3,_tmp2C3,_tmp2C3 + 20U},{_tmp2C4,_tmp2C4,_tmp2C4 + 18U},{_tmp2C5,_tmp2C5,_tmp2C5 + 9U},{_tmp2C6,_tmp2C6,_tmp2C6 + 13U},{_tmp2C7,_tmp2C7,_tmp2C7 + 16U},{_tmp2C8,_tmp2C8,_tmp2C8 + 18U},{_tmp2C9,_tmp2C9,_tmp2C9 + 10U},{_tmp2CA,_tmp2CA,_tmp2CA + 8U},{_tmp2CB,_tmp2CB,_tmp2CB + 4U},{_tmp2CC,_tmp2CC,_tmp2CC + 4U},{_tmp2CD,_tmp2CD,_tmp2CD + 4U},{_tmp2CE,_tmp2CE,_tmp2CE + 4U},{_tmp2CF,_tmp2CF,_tmp2CF + 4U},{_tmp2D0,_tmp2D0,_tmp2D0 + 4U},{_tmp2D1,_tmp2D1,_tmp2D1 + 4U},{_tmp2D2,_tmp2D2,_tmp2D2 + 4U},{_tmp2D3,_tmp2D3,_tmp2D3 + 4U},{_tmp2D4,_tmp2D4,_tmp2D4 + 4U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 5U},{_tmp2E7,_tmp2E7,_tmp2E7 + 17U},{_tmp2E8,_tmp2E8,_tmp2E8 + 18U},{_tmp2E9,_tmp2E9,_tmp2E9 + 19U},{_tmp2EA,_tmp2EA,_tmp2EA + 16U},{_tmp2EB,_tmp2EB,_tmp2EB + 13U},{_tmp2EC,_tmp2EC,_tmp2EC + 14U},{_tmp2ED,_tmp2ED,_tmp2ED + 17U},{_tmp2EE,_tmp2EE,_tmp2EE + 16U},{_tmp2EF,_tmp2EF,_tmp2EF + 12U},{_tmp2F0,_tmp2F0,_tmp2F0 + 19U},{_tmp2F1,_tmp2F1,_tmp2F1 + 13U},{_tmp2F2,_tmp2F2,_tmp2F2 + 21U},{_tmp2F3,_tmp2F3,_tmp2F3 + 14U},{_tmp2F4,_tmp2F4,_tmp2F4 + 20U},{_tmp2F5,_tmp2F5,_tmp2F5 + 21U},{_tmp2F6,_tmp2F6,_tmp2F6 + 13U},{_tmp2F7,_tmp2F7,_tmp2F7 + 15U},{_tmp2F8,_tmp2F8,_tmp2F8 + 17U},{_tmp2F9,_tmp2F9,_tmp2F9 + 19U},{_tmp2FA,_tmp2FA,_tmp2FA + 12U},{_tmp2FB,_tmp2FB,_tmp2FB + 9U},{_tmp2FC,_tmp2FC,_tmp2FC + 17U},{_tmp2FD,_tmp2FD,_tmp2FD + 23U},{_tmp2FE,_tmp2FE,_tmp2FE + 24U},{_tmp2FF,_tmp2FF,_tmp2FF + 15U},{_tmp300,_tmp300,_tmp300 + 11U},{_tmp301,_tmp301,_tmp301 + 15U},{_tmp302,_tmp302,_tmp302 + 10U},{_tmp303,_tmp303,_tmp303 + 15U},{_tmp304,_tmp304,_tmp304 + 25U},{_tmp305,_tmp305,_tmp305 + 5U},{_tmp306,_tmp306,_tmp306 + 15U},{_tmp307,_tmp307,_tmp307 + 15U},{_tmp308,_tmp308,_tmp308 + 11U},{_tmp309,_tmp309,_tmp309 + 22U},{_tmp30A,_tmp30A,_tmp30A + 26U},{_tmp30B,_tmp30B,_tmp30B + 26U},{_tmp30C,_tmp30C,_tmp30C + 16U},{_tmp30D,_tmp30D,_tmp30D + 16U},{_tmp30E,_tmp30E,_tmp30E + 24U},{_tmp30F,_tmp30F,_tmp30F + 25U},{_tmp310,_tmp310,_tmp310 + 25U},{_tmp311,_tmp311,_tmp311 + 16U},{_tmp312,_tmp312,_tmp312 + 19U},{_tmp313,_tmp313,_tmp313 + 25U},{_tmp314,_tmp314,_tmp314 + 35U},{_tmp315,_tmp315,_tmp315 + 27U},{_tmp316,_tmp316,_tmp316 + 18U},{_tmp317,_tmp317,_tmp317 + 20U},{_tmp318,_tmp318,_tmp318 + 19U},{_tmp319,_tmp319,_tmp319 + 14U},{_tmp31A,_tmp31A,_tmp31A + 19U},{_tmp31B,_tmp31B,_tmp31B + 20U},{_tmp31C,_tmp31C,_tmp31C + 14U},{_tmp31D,_tmp31D,_tmp31D + 11U},{_tmp31E,_tmp31E,_tmp31E + 23U},{_tmp31F,_tmp31F,_tmp31F + 18U},{_tmp320,_tmp320,_tmp320 + 30U},{_tmp321,_tmp321,_tmp321 + 8U},{_tmp322,_tmp322,_tmp322 + 12U},{_tmp323,_tmp323,_tmp323 + 14U},{_tmp324,_tmp324,_tmp324 + 13U},{_tmp325,_tmp325,_tmp325 + 23U},{_tmp326,_tmp326,_tmp326 + 14U},{_tmp327,_tmp327,_tmp327 + 18U},{_tmp328,_tmp328,_tmp328 + 8U},{_tmp329,_tmp329,_tmp329 + 11U},{_tmp32A,_tmp32A,_tmp32A + 20U},{_tmp32B,_tmp32B,_tmp32B + 9U},{_tmp32C,_tmp32C,_tmp32C + 16U},{_tmp32D,_tmp32D,_tmp32D + 19U},{_tmp32E,_tmp32E,_tmp32E + 10U},{_tmp32F,_tmp32F,_tmp32F + 16U},{_tmp330,_tmp330,_tmp330 + 11U},{_tmp331,_tmp331,_tmp331 + 14U},{_tmp332,_tmp332,_tmp332 + 11U},{_tmp333,_tmp333,_tmp333 + 15U},{_tmp334,_tmp334,_tmp334 + 22U},{_tmp335,_tmp335,_tmp335 + 16U},{_tmp336,_tmp336,_tmp336 + 17U},{_tmp337,_tmp337,_tmp337 + 12U},{_tmp338,_tmp338,_tmp338 + 18U},{_tmp339,_tmp339,_tmp339 + 17U},{_tmp33A,_tmp33A,_tmp33A + 12U},{_tmp33B,_tmp33B,_tmp33B + 16U},{_tmp33C,_tmp33C,_tmp33C + 11U},{_tmp33D,_tmp33D,_tmp33D + 10U},{_tmp33E,_tmp33E,_tmp33E + 14U},{_tmp33F,_tmp33F,_tmp33F + 15U},{_tmp340,_tmp340,_tmp340 + 20U},{_tmp341,_tmp341,_tmp341 + 27U},{_tmp342,_tmp342,_tmp342 + 25U},{_tmp343,_tmp343,_tmp343 + 10U},{_tmp344,_tmp344,_tmp344 + 18U},{_tmp345,_tmp345,_tmp345 + 21U},{_tmp346,_tmp346,_tmp346 + 19U},{_tmp347,_tmp347,_tmp347 + 16U},{_tmp348,_tmp348,_tmp348 + 20U},{_tmp349,_tmp349,_tmp349 + 15U},{_tmp34A,_tmp34A,_tmp34A + 20U},{_tmp34B,_tmp34B,_tmp34B + 12U},{_tmp34C,_tmp34C,_tmp34C + 15U},{_tmp34D,_tmp34D,_tmp34D + 12U},{_tmp34E,_tmp34E,_tmp34E + 20U},{_tmp34F,_tmp34F,_tmp34F + 19U},{_tmp350,_tmp350,_tmp350 + 20U},{_tmp351,_tmp351,_tmp351 + 21U},{_tmp352,_tmp352,_tmp352 + 21U},{_tmp353,_tmp353,_tmp353 + 12U},{_tmp354,_tmp354,_tmp354 + 17U},{_tmp355,_tmp355,_tmp355 + 19U},{_tmp356,_tmp356,_tmp356 + 14U},{_tmp357,_tmp357,_tmp357 + 17U},{_tmp358,_tmp358,_tmp358 + 23U},{_tmp359,_tmp359,_tmp359 + 13U},{_tmp35A,_tmp35A,_tmp35A + 14U},{_tmp35B,_tmp35B,_tmp35B + 16U},{_tmp35C,_tmp35C,_tmp35C + 16U},{_tmp35D,_tmp35D,_tmp35D + 8U},{_tmp35E,_tmp35E,_tmp35E + 19U},{_tmp35F,_tmp35F,_tmp35F + 20U},{_tmp360,_tmp360,_tmp360 + 14U},{_tmp361,_tmp361,_tmp361 + 19U},{_tmp362,_tmp362,_tmp362 + 20U},{_tmp363,_tmp363,_tmp363 + 11U},{_tmp364,_tmp364,_tmp364 + 22U},{_tmp365,_tmp365,_tmp365 + 20U},{_tmp366,_tmp366,_tmp366 + 23U},{_tmp367,_tmp367,_tmp367 + 20U},{_tmp368,_tmp368,_tmp368 + 22U},{_tmp369,_tmp369,_tmp369 + 23U},{_tmp36A,_tmp36A,_tmp36A + 24U},{_tmp36B,_tmp36B,_tmp36B + 24U},{_tmp36C,_tmp36C,_tmp36C + 15U},{_tmp36D,_tmp36D,_tmp36D + 20U},{_tmp36E,_tmp36E,_tmp36E + 22U},{_tmp36F,_tmp36F,_tmp36F + 17U},{_tmp370,_tmp370,_tmp370 + 20U},{_tmp371,_tmp371,_tmp371 + 26U},{_tmp372,_tmp372,_tmp372 + 12U},{_tmp373,_tmp373,_tmp373 + 14U},{_tmp374,_tmp374,_tmp374 + 12U},{_tmp375,_tmp375,_tmp375 + 18U},{_tmp376,_tmp376,_tmp376 + 16U},{_tmp377,_tmp377,_tmp377 + 17U},{_tmp378,_tmp378,_tmp378 + 15U},{_tmp379,_tmp379,_tmp379 + 9U},{_tmp37A,_tmp37A,_tmp37A + 13U},{_tmp37B,_tmp37B,_tmp37B + 12U},{_tmp37C,_tmp37C,_tmp37C + 12U},{_tmp37D,_tmp37D,_tmp37D + 11U},{_tmp37E,_tmp37E,_tmp37E + 11U},{_tmp37F,_tmp37F,_tmp37F + 11U},{_tmp380,_tmp380,_tmp380 + 16U},{_tmp381,_tmp381,_tmp381 + 17U},{_tmp382,_tmp382,_tmp382 + 19U},{_tmp383,_tmp383,_tmp383 + 17U},{_tmp384,_tmp384,_tmp384 + 19U},{_tmp385,_tmp385,_tmp385 + 25U},{_tmp386,_tmp386,_tmp386 + 26U},{_tmp387,_tmp387,_tmp387 + 9U},{_tmp388,_tmp388,_tmp388 + 20U},{_tmp389,_tmp389,_tmp389 + 17U},{_tmp38A,_tmp38A,_tmp38A + 18U},{_tmp38B,_tmp38B,_tmp38B + 11U},{_tmp38C,_tmp38C,_tmp38C + 12U}};
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
struct _RegionHandle _tmp38D=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp38D;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 154
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YYINITIALSVAL).tag=73U,(_tmp6F0.YYINITIALSVAL).val=0;_tmp6F0;});
int yynerrs;
# 158
struct Cyc_Yyltype yylloc;
# 162
int yyssp_offset;
# 164
struct _fat_ptr yyss=({unsigned _tmp630=200U;_tag_fat(_region_calloc(yyregion,sizeof(short),_tmp630),sizeof(short),_tmp630);});
# 166
int yyvsp_offset;
# 168
struct _fat_ptr yyvs=
_tag_fat(({unsigned _tmp62F=200U;struct Cyc_Yystacktype*_tmp62E=({struct _RegionHandle*_tmp79E=yyregion;_region_malloc(_tmp79E,_check_times(_tmp62F,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp6EF=200U;unsigned i;for(i=0;i < _tmp6EF;++ i){(_tmp62E[i]).v=yylval,({struct Cyc_Yyltype _tmp79F=Cyc_yynewloc();(_tmp62E[i]).l=_tmp79F;});}}0;});_tmp62E;}),sizeof(struct Cyc_Yystacktype),200U);
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
({struct _fat_ptr _tmp7A1=({const char*_tmp38E="parser stack overflow";_tag_fat(_tmp38E,sizeof(char),22U);});int _tmp7A0=yystate;Cyc_yyerror(_tmp7A1,_tmp7A0,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 216
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _fat_ptr yyss1=({unsigned _tmp392=(unsigned)yystacksize;short*_tmp391=({struct _RegionHandle*_tmp7A2=yyregion;_region_malloc(_tmp7A2,_check_times(_tmp392,sizeof(short)));});({{unsigned _tmp6A5=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp6A5;++ i){
i <= (unsigned)yyssp_offset?_tmp391[i]=*((short*)_check_fat_subscript(yyss,sizeof(short),(int)i)):(_tmp391[i]=0);}}0;});_tag_fat(_tmp391,sizeof(short),_tmp392);});
# 222
struct _fat_ptr yyvs1=({unsigned _tmp390=(unsigned)yystacksize;struct Cyc_Yystacktype*_tmp38F=({struct _RegionHandle*_tmp7A3=yyregion;_region_malloc(_tmp7A3,_check_times(_tmp390,sizeof(struct Cyc_Yystacktype)));});({{unsigned _tmp6A4=(unsigned)yystacksize;unsigned i;for(i=0;i < _tmp6A4;++ i){
# 224
i <= (unsigned)yyssp_offset?_tmp38F[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(int)i)):(_tmp38F[i]=*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),0)));}}0;});_tag_fat(_tmp38F,sizeof(struct Cyc_Yystacktype),_tmp390);});
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
if(yyn == 1066){
int _tmp393=0;_npop_handler(0);return _tmp393;}
# 328 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 332
({struct Cyc_Yystacktype _tmp7A4=({struct Cyc_Yystacktype _tmp6A6;_tmp6A6.v=yylval,_tmp6A6.l=yylloc;_tmp6A6;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmp7A4;});
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
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,548U,sizeof(short),yyn));
yyyvsp=(struct Cyc_Yystacktype*)_check_null(_untag_fat_ptr(_fat_ptr_plus(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + 1)- yylen),sizeof(struct Cyc_Yystacktype),12U));
if(yylen > 0)
yyval=(yyyvsp[0]).v;
# 370 "cycbison.simple"
{int _tmp394=yyn;switch(_tmp394){case 1: _LL1: _LL2:
# 1109 "parse.y"
 yyval=(yyyvsp[0]).v;
Cyc_Parse_parse_result=Cyc_yyget_YY16(&(yyyvsp[0]).v);
# 1112
goto _LL0;case 2: _LL3: _LL4:
# 1115 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp7A5=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp7A5,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 3: _LL5: _LL6:
# 1119 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp397=_cycalloc(sizeof(*_tmp397));({struct Cyc_Absyn_Decl*_tmp7A9=({struct Cyc_Absyn_Decl*_tmp396=_cycalloc(sizeof(*_tmp396));({void*_tmp7A8=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->tag=10,({struct _tuple0*_tmp7A7=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp395->f1=_tmp7A7;}),({struct Cyc_List_List*_tmp7A6=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp395->f2=_tmp7A6;});_tmp395;});_tmp396->r=_tmp7A8;}),_tmp396->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp396;});_tmp397->hd=_tmp7A9;}),_tmp397->tl=0;_tmp397;}));
Cyc_Lex_leave_using();
# 1122
goto _LL0;case 4: _LL7: _LL8:
# 1123 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39A=_cycalloc(sizeof(*_tmp39A));({struct Cyc_Absyn_Decl*_tmp7AE=({struct Cyc_Absyn_Decl*_tmp399=_cycalloc(sizeof(*_tmp399));({void*_tmp7AD=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->tag=10,({struct _tuple0*_tmp7AC=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp398->f1=_tmp7AC;}),({struct Cyc_List_List*_tmp7AB=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp398->f2=_tmp7AB;});_tmp398;});_tmp399->r=_tmp7AD;}),_tmp399->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp399;});_tmp39A->hd=_tmp7AE;}),({struct Cyc_List_List*_tmp7AA=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp39A->tl=_tmp7AA;});_tmp39A;}));
goto _LL0;case 5: _LL9: _LLA:
# 1126
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39E=_cycalloc(sizeof(*_tmp39E));({struct Cyc_Absyn_Decl*_tmp7B3=({struct Cyc_Absyn_Decl*_tmp39D=_cycalloc(sizeof(*_tmp39D));({void*_tmp7B2=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->tag=9,({struct _fat_ptr*_tmp7B1=({struct _fat_ptr*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct _fat_ptr _tmp7B0=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp39B=_tmp7B0;});_tmp39B;});_tmp39C->f1=_tmp7B1;}),({struct Cyc_List_List*_tmp7AF=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp39C->f2=_tmp7AF;});_tmp39C;});_tmp39D->r=_tmp7B2;}),_tmp39D->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp39D;});_tmp39E->hd=_tmp7B3;}),_tmp39E->tl=0;_tmp39E;}));
Cyc_Lex_leave_namespace();
# 1129
goto _LL0;case 6: _LLB: _LLC:
# 1130 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({struct Cyc_Absyn_Decl*_tmp7B9=({struct Cyc_Absyn_Decl*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({void*_tmp7B8=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));_tmp3A0->tag=9,({struct _fat_ptr*_tmp7B7=({struct _fat_ptr*_tmp39F=_cycalloc(sizeof(*_tmp39F));({struct _fat_ptr _tmp7B6=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp39F=_tmp7B6;});_tmp39F;});_tmp3A0->f1=_tmp7B7;}),({struct Cyc_List_List*_tmp7B5=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3A0->f2=_tmp7B5;});_tmp3A0;});_tmp3A1->r=_tmp7B8;}),_tmp3A1->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3A1;});_tmp3A2->hd=_tmp7B9;}),({struct Cyc_List_List*_tmp7B4=Cyc_yyget_YY16(&(yyyvsp[4]).v);_tmp3A2->tl=_tmp7B4;});_tmp3A2;}));
goto _LL0;case 7: _LLD: _LLE: {
# 1132 "parse.y"
int _tmp3A3=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_c_include=_tmp3A3;
struct Cyc_List_List*cycdecls=Cyc_yyget_YY16(&(yyyvsp[4]).v);
struct _tuple29*_tmp3A4=Cyc_yyget_YY54(&(yyyvsp[5]).v);struct _tuple29*_stmttmp13=_tmp3A4;struct _tuple29*_tmp3A5=_stmttmp13;unsigned _tmp3A7;struct Cyc_List_List*_tmp3A6;_LL446: _tmp3A6=_tmp3A5->f1;_tmp3A7=_tmp3A5->f2;_LL447: {struct Cyc_List_List*exs=_tmp3A6;unsigned wc=_tmp3A7;
struct Cyc_List_List*_tmp3A8=Cyc_yyget_YY55(&(yyyvsp[6]).v);struct Cyc_List_List*hides=_tmp3A8;
if(exs != 0 && hides != 0)
({void*_tmp3A9=0U;({unsigned _tmp7BB=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp7BA=({const char*_tmp3AA="hide list can only be used with export { * }, or no export block";_tag_fat(_tmp3AA,sizeof(char),65U);});Cyc_Warn_err(_tmp7BB,_tmp7BA,_tag_fat(_tmp3A9,sizeof(void*),0));});});
if((unsigned)hides && !((int)wc))
wc=(unsigned)((yyyvsp[6]).l).first_line;
if(!is_c_include){
if(exs != 0 || cycdecls != 0){
({void*_tmp3AB=0U;({unsigned _tmp7BD=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp7BC=({const char*_tmp3AC="expecting \"C include\"";_tag_fat(_tmp3AC,sizeof(char),22U);});Cyc_Warn_err(_tmp7BD,_tmp7BC,_tag_fat(_tmp3AB,sizeof(void*),0));});});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));({struct Cyc_Absyn_Decl*_tmp7C2=({struct Cyc_Absyn_Decl*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));({void*_tmp7C1=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->tag=12,({struct Cyc_List_List*_tmp7C0=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3AE->f1=_tmp7C0;}),_tmp3AE->f2=cycdecls,_tmp3AE->f3=exs,({struct _tuple10*_tmp7BF=({struct _tuple10*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->f1=wc,_tmp3AD->f2=hides;_tmp3AD;});_tmp3AE->f4=_tmp7BF;});_tmp3AE;});_tmp3AF->r=_tmp7C1;}),_tmp3AF->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3AF;});_tmp3B0->hd=_tmp7C2;}),({struct Cyc_List_List*_tmp7BE=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B0->tl=_tmp7BE;});_tmp3B0;}));}else{
# 1146
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));({struct Cyc_Absyn_Decl*_tmp7C6=({struct Cyc_Absyn_Decl*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({void*_tmp7C5=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->tag=11,({struct Cyc_List_List*_tmp7C4=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B1->f1=_tmp7C4;});_tmp3B1;});_tmp3B2->r=_tmp7C5;}),_tmp3B2->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B2;});_tmp3B3->hd=_tmp7C6;}),({struct Cyc_List_List*_tmp7C3=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B3->tl=_tmp7C3;});_tmp3B3;}));}}else{
# 1149
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct Cyc_Absyn_Decl*_tmp7CB=({struct Cyc_Absyn_Decl*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));({void*_tmp7CA=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->tag=12,({struct Cyc_List_List*_tmp7C9=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B5->f1=_tmp7C9;}),_tmp3B5->f2=cycdecls,_tmp3B5->f3=exs,({struct _tuple10*_tmp7C8=({struct _tuple10*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->f1=wc,_tmp3B4->f2=hides;_tmp3B4;});_tmp3B5->f4=_tmp7C8;});_tmp3B5;});_tmp3B6->r=_tmp7CA;}),_tmp3B6->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B6;});_tmp3B7->hd=_tmp7CB;}),({struct Cyc_List_List*_tmp7C7=Cyc_yyget_YY16(&(yyyvsp[7]).v);_tmp3B7->tl=_tmp7C7;});_tmp3B7;}));}
# 1151
goto _LL0;}}case 8: _LLF: _LL10:
# 1152 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));({struct Cyc_Absyn_Decl*_tmp7CD=({struct Cyc_Absyn_Decl*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3B8->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3B8;});_tmp3B9->hd=_tmp7CD;}),({struct Cyc_List_List*_tmp7CC=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3B9->tl=_tmp7CC;});_tmp3B9;}));
goto _LL0;case 9: _LL11: _LL12:
# 1154 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct Cyc_Absyn_Decl*_tmp7CF=({struct Cyc_Absyn_Decl*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3BA->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BA;});_tmp3BB->hd=_tmp7CF;}),({struct Cyc_List_List*_tmp7CE=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BB->tl=_tmp7CE;});_tmp3BB;}));
goto _LL0;case 10: _LL13: _LL14:
# 1156 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({struct Cyc_Absyn_Decl*_tmp7D1=({struct Cyc_Absyn_Decl*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3BC->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BC;});_tmp3BD->hd=_tmp7D1;}),({struct Cyc_List_List*_tmp7D0=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BD->tl=_tmp7D0;});_tmp3BD;}));
goto _LL0;case 11: _LL15: _LL16:
# 1158 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct Cyc_Absyn_Decl*_tmp7D3=({struct Cyc_Absyn_Decl*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3BE->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp3BE;});_tmp3BF->hd=_tmp7D3;}),({struct Cyc_List_List*_tmp7D2=Cyc_yyget_YY16(&(yyyvsp[2]).v);_tmp3BF->tl=_tmp7D2;});_tmp3BF;}));
goto _LL0;case 12: _LL17: _LL18:
# 1159 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 13: _LL19: _LL1A:
# 1163 "parse.y"
 Cyc_Parse_parsing_tempest=1;
goto _LL0;case 14: _LL1B: _LL1C:
# 1166
 Cyc_Parse_parsing_tempest=0;
goto _LL0;case 15: _LL1D: _LL1E:
# 1171 "parse.y"
 Cyc_Lex_enter_extern_c();
if(({struct _fat_ptr _tmp7D4=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp7D4,({const char*_tmp3C0="C";_tag_fat(_tmp3C0,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY32(0);else{
if(({struct _fat_ptr _tmp7D5=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp7D5,({const char*_tmp3C1="C include";_tag_fat(_tmp3C1,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY32(1);else{
# 1177
({void*_tmp3C2=0U;({unsigned _tmp7D7=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp7D6=({const char*_tmp3C3="expecting \"C\" or \"C include\"";_tag_fat(_tmp3C3,sizeof(char),29U);});Cyc_Warn_err(_tmp7D7,_tmp7D6,_tag_fat(_tmp3C2,sizeof(void*),0));});});
yyval=Cyc_YY32(1);}}
# 1181
goto _LL0;case 16: _LL1F: _LL20:
# 1184 "parse.y"
 Cyc_Lex_leave_extern_c();
goto _LL0;case 17: _LL21: _LL22:
# 1188 "parse.y"
 yyval=Cyc_YY55(0);
goto _LL0;case 18: _LL23: _LL24:
# 1189 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 19: _LL25: _LL26:
# 1193 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct _tuple0*_tmp7D8=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3C4->hd=_tmp7D8;}),_tmp3C4->tl=0;_tmp3C4;}));
goto _LL0;case 20: _LL27: _LL28:
# 1194 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct _tuple0*_tmp7D9=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3C5->hd=_tmp7D9;}),_tmp3C5->tl=0;_tmp3C5;}));
goto _LL0;case 21: _LL29: _LL2A:
# 1195 "parse.y"
 yyval=Cyc_YY55(({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct _tuple0*_tmp7DB=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3C6->hd=_tmp7DB;}),({struct Cyc_List_List*_tmp7DA=Cyc_yyget_YY55(&(yyyvsp[2]).v);_tmp3C6->tl=_tmp7DA;});_tmp3C6;}));
goto _LL0;case 22: _LL2B: _LL2C:
# 1199 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->f1=0,_tmp3C7->f2=0U;_tmp3C7;}));
goto _LL0;case 23: _LL2D: _LL2E:
# 1200 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 24: _LL2F: _LL30:
# 1204 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));({struct Cyc_List_List*_tmp7DC=Cyc_yyget_YY53(&(yyyvsp[2]).v);_tmp3C8->f1=_tmp7DC;}),_tmp3C8->f2=0U;_tmp3C8;}));
goto _LL0;case 25: _LL31: _LL32:
# 1205 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->f1=0,_tmp3C9->f2=0U;_tmp3C9;}));
goto _LL0;case 26: _LL33: _LL34:
# 1206 "parse.y"
 yyval=Cyc_YY54(({struct _tuple29*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->f1=0,_tmp3CA->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp3CA;}));
goto _LL0;case 27: _LL35: _LL36:
# 1211 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));({struct _tuple33*_tmp7DE=({struct _tuple33*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp7DD=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CB->f2=_tmp7DD;}),_tmp3CB->f3=0;_tmp3CB;});_tmp3CC->hd=_tmp7DE;}),_tmp3CC->tl=0;_tmp3CC;}));
goto _LL0;case 28: _LL37: _LL38:
# 1213 "parse.y"
 yyval=Cyc_YY53(({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct _tuple33*_tmp7E1=({struct _tuple33*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->f1=(unsigned)((yyyvsp[0]).l).first_line,({struct _tuple0*_tmp7E0=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp3CD->f2=_tmp7E0;}),_tmp3CD->f3=0;_tmp3CD;});_tmp3CE->hd=_tmp7E1;}),({struct Cyc_List_List*_tmp7DF=Cyc_yyget_YY53(&(yyyvsp[2]).v);_tmp3CE->tl=_tmp7DF;});_tmp3CE;}));
goto _LL0;case 29: _LL39: _LL3A:
# 1217 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 30: _LL3B: _LL3C:
# 1218 "parse.y"
 yyval=(yyyvsp[2]).v;
goto _LL0;case 31: _LL3D: _LL3E:
# 1222 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct Cyc_Absyn_Decl*_tmp7E4=({void*_tmp7E3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->tag=1,({struct Cyc_Absyn_Fndecl*_tmp7E2=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp3CF->f1=_tmp7E2;});_tmp3CF;});Cyc_Absyn_new_decl(_tmp7E3,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3D0->hd=_tmp7E4;}),_tmp3D0->tl=0;_tmp3D0;}));
goto _LL0;case 32: _LL3F: _LL40:
# 1223 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 33: _LL41: _LL42:
# 1224 "parse.y"
 yyval=Cyc_YY16(0);
goto _LL0;case 36: _LL43: _LL44:
# 1234 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp7E7=yyr;struct Cyc_Parse_Declarator _tmp7E6=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp7E5=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Parse_make_function(_tmp7E7,0,_tmp7E6,0,_tmp7E5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 37: _LL45: _LL46: {
# 1236 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D1=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3D1;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7EA=yyr;struct Cyc_Parse_Declarator _tmp7E9=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7E8=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7EA,& d,_tmp7E9,0,_tmp7E8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 38: _LL47: _LL48:
# 1248 "parse.y"
 yyval=Cyc_YY15(({struct _RegionHandle*_tmp7EE=yyr;struct Cyc_Parse_Declarator _tmp7ED=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_List_List*_tmp7EC=Cyc_yyget_YY16(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7EB=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7EE,0,_tmp7ED,_tmp7EC,_tmp7EB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 39: _LL49: _LL4A: {
# 1250 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D2=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3D2;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7F2=yyr;struct Cyc_Parse_Declarator _tmp7F1=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp7F0=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp7EF=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp7F2,& d,_tmp7F1,_tmp7F0,_tmp7EF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 40: _LL4B: _LL4C: {
# 1258 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D3=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3D3;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7F5=yyr;struct Cyc_Parse_Declarator _tmp7F4=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Absyn_Stmt*_tmp7F3=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Parse_make_function(_tmp7F5,& d,_tmp7F4,0,_tmp7F3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 41: _LL4D: _LL4E: {
# 1261 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D4=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct Cyc_Parse_Declaration_spec d=_tmp3D4;
yyval=Cyc_YY15(({struct _RegionHandle*_tmp7F9=yyr;struct Cyc_Parse_Declarator _tmp7F8=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp7F7=Cyc_yyget_YY16(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp7F6=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);Cyc_Parse_make_function(_tmp7F9,& d,_tmp7F8,_tmp7F7,_tmp7F6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 42: _LL4F: _LL50:
# 1266 "parse.y"
 Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v));yyval=(yyyvsp[1]).v;
goto _LL0;case 43: _LL51: _LL52:
# 1269
 Cyc_Lex_leave_using();
goto _LL0;case 44: _LL53: _LL54:
# 1272
 Cyc_Lex_enter_namespace(({struct _fat_ptr*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct _fat_ptr _tmp7FA=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3D5=_tmp7FA;});_tmp3D5;}));yyval=(yyyvsp[1]).v;
goto _LL0;case 45: _LL55: _LL56:
# 1275
 Cyc_Lex_leave_namespace();
goto _LL0;case 46: _LL57: _LL58:
# 1281 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp7FC=Cyc_yyget_YY17(&(yyyvsp[0]).v);unsigned _tmp7FB=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp7FC,0,_tmp7FB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 47: _LL59: _LL5A:
# 1283 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp7FF=Cyc_yyget_YY17(&(yyyvsp[0]).v);struct _tuple11*_tmp7FE=((struct _tuple11*(*)(struct _tuple11*))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(&(yyyvsp[1]).v));unsigned _tmp7FD=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_make_declarations(_tmp7FF,_tmp7FE,_tmp7FD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 48: _LL5B: _LL5C:
# 1286
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct Cyc_Absyn_Decl*_tmp802=({struct Cyc_Absyn_Pat*_tmp801=Cyc_yyget_YY9(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp800=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_let_decl(_tmp801,_tmp800,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3D6->hd=_tmp802;}),_tmp3D6->tl=0;_tmp3D6;}));
goto _LL0;case 49: _LL5D: _LL5E: {
# 1288 "parse.y"
struct Cyc_List_List*_tmp3D7=0;struct Cyc_List_List*vds=_tmp3D7;
{struct Cyc_List_List*_tmp3D8=Cyc_yyget_YY37(&(yyyvsp[1]).v);struct Cyc_List_List*ids=_tmp3D8;for(0;ids != 0;ids=ids->tl){
struct _tuple0*qv=({struct _tuple0*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({union Cyc_Absyn_Nmspace _tmp803=Cyc_Absyn_Rel_n(0);_tmp3DA->f1=_tmp803;}),_tmp3DA->f2=(struct _fat_ptr*)ids->hd;_tmp3DA;});
vds=({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct Cyc_Absyn_Vardecl*_tmp805=({struct _tuple0*_tmp804=qv;Cyc_Absyn_new_vardecl(0U,_tmp804,Cyc_Absyn_wildtyp(0),0);});_tmp3D9->hd=_tmp805;}),_tmp3D9->tl=vds;_tmp3D9;});}}
# 1293
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));({struct Cyc_Absyn_Decl*_tmp807=({struct Cyc_List_List*_tmp806=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(vds);Cyc_Absyn_letv_decl(_tmp806,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3DB->hd=_tmp807;}),_tmp3DB->tl=0;_tmp3DB;}));
# 1295
goto _LL0;}case 50: _LL5F: _LL60:
# 1298 "parse.y"
({struct _fat_ptr _tmp808=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmp808,(unsigned)((yyyvsp[2]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct _fat_ptr*_tmp80B=({struct _fat_ptr*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));({struct _fat_ptr _tmp80A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp3DF=_tmp80A;});_tmp3DF;});_tmp3E0->name=_tmp80B;}),_tmp3E0->identity=- 1,({void*_tmp809=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3E0->kind=_tmp809;});_tmp3E0;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp80F=(unsigned)((yyyvsp[4]).l).first_line;struct _tuple0*_tmp80E=({struct _tuple0*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp80D=({struct _fat_ptr*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct _fat_ptr _tmp80C=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp3DD=_tmp80C;});_tmp3DD;});_tmp3DE->f2=_tmp80D;});_tmp3DE;});Cyc_Absyn_new_vardecl(_tmp80F,_tmp80E,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));({struct Cyc_Absyn_Decl*_tmp810=Cyc_Absyn_region_decl(tv,vd,0,(unsigned)((yyyvsp[0]).l).first_line);_tmp3DC->hd=_tmp810;}),_tmp3DC->tl=0;_tmp3DC;}));
# 1304
goto _LL0;}case 51: _LL61: _LL62: {
# 1306
struct _fat_ptr tvstring=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3E8=({struct Cyc_String_pa_PrintArg_struct _tmp6A7;_tmp6A7.tag=0,({struct _fat_ptr _tmp811=(struct _fat_ptr)((struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v));_tmp6A7.f1=_tmp811;});_tmp6A7;});void*_tmp3E6[1];_tmp3E6[0]=& _tmp3E8;({struct _fat_ptr _tmp812=({const char*_tmp3E7="`%s";_tag_fat(_tmp3E7,sizeof(char),4U);});Cyc_aprintf(_tmp812,_tag_fat(_tmp3E6,sizeof(void*),1));});});
({struct _fat_ptr _tmp813=Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_Parse_tvar_ok(_tmp813,(unsigned)((yyyvsp[1]).l).first_line);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));({struct _fat_ptr*_tmp815=({struct _fat_ptr*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));*_tmp3E4=tvstring;_tmp3E4;});_tmp3E5->name=_tmp815;}),_tmp3E5->identity=- 1,({void*_tmp814=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);_tmp3E5->kind=_tmp814;});_tmp3E5;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmp819=(unsigned)((yyyvsp[1]).l).first_line;struct _tuple0*_tmp818=({struct _tuple0*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp817=({struct _fat_ptr*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));({struct _fat_ptr _tmp816=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp3E2=_tmp816;});_tmp3E2;});_tmp3E3->f2=_tmp817;});_tmp3E3;});Cyc_Absyn_new_vardecl(_tmp819,_tmp818,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));({struct Cyc_Absyn_Decl*_tmp81D=({struct Cyc_Absyn_Tvar*_tmp81C=tv;struct Cyc_Absyn_Vardecl*_tmp81B=vd;struct Cyc_Absyn_Exp*_tmp81A=Cyc_yyget_YY58(&(yyyvsp[2]).v);Cyc_Absyn_region_decl(_tmp81C,_tmp81B,_tmp81A,(unsigned)((yyyvsp[0]).l).first_line);});_tmp3E1->hd=_tmp81D;}),_tmp3E1->tl=0;_tmp3E1;}));
# 1313
goto _LL0;}}case 52: _LL63: _LL64:
# 1316 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 53: _LL65: _LL66:
# 1318 "parse.y"
 if(({struct _fat_ptr _tmp81E=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp81E,({const char*_tmp3E9="open";_tag_fat(_tmp3E9,sizeof(char),5U);}));})!= 0)({void*_tmp3EA=0U;({unsigned _tmp820=(unsigned)((yyyvsp[3]).l).first_line;struct _fat_ptr _tmp81F=({const char*_tmp3EB="expecting `open'";_tag_fat(_tmp3EB,sizeof(char),17U);});Cyc_Warn_err(_tmp820,_tmp81F,_tag_fat(_tmp3EA,sizeof(void*),0));});});
yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[3]).v));
goto _LL0;case 54: _LL67: _LL68:
# 1322
 yyval=(yyyvsp[0]).v;
goto _LL0;case 55: _LL69: _LL6A:
# 1323 "parse.y"
 yyval=Cyc_YY16(({struct Cyc_List_List*_tmp821=Cyc_yyget_YY16(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp821,Cyc_yyget_YY16(&(yyyvsp[1]).v));}));
goto _LL0;case 56: _LL6B: _LL6C:
# 1329 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6A8;({enum Cyc_Parse_Storage_class _tmp824=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp6A8.sc=_tmp824;}),({struct Cyc_Absyn_Tqual _tmp823=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6A8.tq=_tmp823;}),({
struct Cyc_Parse_Type_specifier _tmp822=Cyc_Parse_empty_spec(0U);_tmp6A8.type_specs=_tmp822;}),_tmp6A8.is_inline=0,_tmp6A8.attributes=0;_tmp6A8;}));
goto _LL0;case 57: _LL6D: _LL6E: {
# 1332 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3EC=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3EC;
if((int)two.sc != (int)Cyc_Parse_None_sc)
({void*_tmp3ED=0U;({unsigned _tmp826=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp825=({const char*_tmp3EE="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_fat(_tmp3EE,sizeof(char),73U);});Cyc_Warn_warn(_tmp826,_tmp825,_tag_fat(_tmp3ED,sizeof(void*),0));});});
# 1336
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6A9;({enum Cyc_Parse_Storage_class _tmp827=Cyc_yyget_YY20(&(yyyvsp[0]).v);_tmp6A9.sc=_tmp827;}),_tmp6A9.tq=two.tq,_tmp6A9.type_specs=two.type_specs,_tmp6A9.is_inline=two.is_inline,_tmp6A9.attributes=two.attributes;_tmp6A9;}));
# 1339
goto _LL0;}case 58: _LL6F: _LL70:
# 1340 "parse.y"
({void*_tmp3EF=0U;({unsigned _tmp829=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp828=({const char*_tmp3F0="__extension__ keyword ignored in declaration";_tag_fat(_tmp3F0,sizeof(char),45U);});Cyc_Warn_warn(_tmp829,_tmp828,_tag_fat(_tmp3EF,sizeof(void*),0));});});
yyval=(yyyvsp[1]).v;
# 1343
goto _LL0;case 59: _LL71: _LL72:
# 1344 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6AA;_tmp6AA.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp82B=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6AA.tq=_tmp82B;}),({
struct Cyc_Parse_Type_specifier _tmp82A=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp6AA.type_specs=_tmp82A;}),_tmp6AA.is_inline=0,_tmp6AA.attributes=0;_tmp6AA;}));
goto _LL0;case 60: _LL73: _LL74: {
# 1347 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3F1=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3F1;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6AB;_tmp6AB.sc=two.sc,_tmp6AB.tq=two.tq,({
struct Cyc_Parse_Type_specifier _tmp82E=({unsigned _tmp82D=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp82C=two.type_specs;Cyc_Parse_combine_specifiers(_tmp82D,_tmp82C,Cyc_yyget_YY21(&(yyyvsp[0]).v));});_tmp6AB.type_specs=_tmp82E;}),_tmp6AB.is_inline=two.is_inline,_tmp6AB.attributes=two.attributes;_tmp6AB;}));
# 1352
goto _LL0;}case 61: _LL75: _LL76:
# 1353 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6AC;_tmp6AC.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp830=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp6AC.tq=_tmp830;}),({struct Cyc_Parse_Type_specifier _tmp82F=Cyc_Parse_empty_spec(0U);_tmp6AC.type_specs=_tmp82F;}),_tmp6AC.is_inline=0,_tmp6AC.attributes=0;_tmp6AC;}));
goto _LL0;case 62: _LL77: _LL78: {
# 1355 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3F2=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3F2;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6AD;_tmp6AD.sc=two.sc,({struct Cyc_Absyn_Tqual _tmp832=({struct Cyc_Absyn_Tqual _tmp831=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp831,two.tq);});_tmp6AD.tq=_tmp832;}),_tmp6AD.type_specs=two.type_specs,_tmp6AD.is_inline=two.is_inline,_tmp6AD.attributes=two.attributes;_tmp6AD;}));
# 1359
goto _LL0;}case 63: _LL79: _LL7A:
# 1360 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6AE;_tmp6AE.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp834=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6AE.tq=_tmp834;}),({
struct Cyc_Parse_Type_specifier _tmp833=Cyc_Parse_empty_spec(0U);_tmp6AE.type_specs=_tmp833;}),_tmp6AE.is_inline=1,_tmp6AE.attributes=0;_tmp6AE;}));
goto _LL0;case 64: _LL7B: _LL7C: {
# 1363 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3F3=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3F3;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6AF;_tmp6AF.sc=two.sc,_tmp6AF.tq=two.tq,_tmp6AF.type_specs=two.type_specs,_tmp6AF.is_inline=1,_tmp6AF.attributes=two.attributes;_tmp6AF;}));
# 1366
goto _LL0;}case 65: _LL7D: _LL7E:
# 1367 "parse.y"
 yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6B0;_tmp6B0.sc=Cyc_Parse_None_sc,({struct Cyc_Absyn_Tqual _tmp837=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6B0.tq=_tmp837;}),({
struct Cyc_Parse_Type_specifier _tmp836=Cyc_Parse_empty_spec(0U);_tmp6B0.type_specs=_tmp836;}),_tmp6B0.is_inline=0,({struct Cyc_List_List*_tmp835=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp6B0.attributes=_tmp835;});_tmp6B0;}));
goto _LL0;case 66: _LL7F: _LL80: {
# 1370 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3F4=Cyc_yyget_YY17(&(yyyvsp[1]).v);struct Cyc_Parse_Declaration_spec two=_tmp3F4;
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6B1;_tmp6B1.sc=two.sc,_tmp6B1.tq=two.tq,_tmp6B1.type_specs=two.type_specs,_tmp6B1.is_inline=two.is_inline,({
# 1373
struct Cyc_List_List*_tmp839=({struct Cyc_List_List*_tmp838=Cyc_yyget_YY46(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp838,two.attributes);});_tmp6B1.attributes=_tmp839;});_tmp6B1;}));
goto _LL0;}case 67: _LL81: _LL82:
# 1377 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Auto_sc);
goto _LL0;case 68: _LL83: _LL84:
# 1378 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Register_sc);
goto _LL0;case 69: _LL85: _LL86:
# 1379 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Static_sc);
goto _LL0;case 70: _LL87: _LL88:
# 1380 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Extern_sc);
goto _LL0;case 71: _LL89: _LL8A:
# 1382 "parse.y"
 if(({struct _fat_ptr _tmp83A=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmp83A,({const char*_tmp3F5="C";_tag_fat(_tmp3F5,sizeof(char),2U);}));})!= 0)
({void*_tmp3F6=0U;({unsigned _tmp83C=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp83B=({const char*_tmp3F7="only extern or extern \"C\" is allowed";_tag_fat(_tmp3F7,sizeof(char),37U);});Cyc_Warn_err(_tmp83C,_tmp83B,_tag_fat(_tmp3F6,sizeof(void*),0));});});
yyval=Cyc_YY20(Cyc_Parse_ExternC_sc);
# 1386
goto _LL0;case 72: _LL8B: _LL8C:
# 1386 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Typedef_sc);
goto _LL0;case 73: _LL8D: _LL8E:
# 1388 "parse.y"
 yyval=Cyc_YY20(Cyc_Parse_Abstract_sc);
goto _LL0;case 74: _LL8F: _LL90:
# 1393 "parse.y"
 yyval=Cyc_YY46(0);
goto _LL0;case 75: _LL91: _LL92:
# 1394 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 76: _LL93: _LL94:
# 1398 "parse.y"
 yyval=(yyyvsp[3]).v;
goto _LL0;case 77: _LL95: _LL96:
# 1402 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));({void*_tmp83D=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp3F8->hd=_tmp83D;}),_tmp3F8->tl=0;_tmp3F8;}));
goto _LL0;case 78: _LL97: _LL98:
# 1403 "parse.y"
 yyval=Cyc_YY46(({struct Cyc_List_List*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({void*_tmp83F=Cyc_yyget_YY47(&(yyyvsp[0]).v);_tmp3F9->hd=_tmp83F;}),({struct Cyc_List_List*_tmp83E=Cyc_yyget_YY46(&(yyyvsp[2]).v);_tmp3F9->tl=_tmp83E;});_tmp3F9;}));
goto _LL0;case 79: _LL99: _LL9A:
# 1407 "parse.y"
 yyval=Cyc_YY47(({unsigned _tmp840=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Atts_parse_nullary_att(_tmp840,Cyc_yyget_String_tok(&(yyyvsp[0]).v));}));
goto _LL0;case 80: _LL9B: _LL9C:
# 1408 "parse.y"
 yyval=Cyc_YY47((void*)& Cyc_Atts_Const_att_val);
goto _LL0;case 81: _LL9D: _LL9E:
# 1410 "parse.y"
 yyval=Cyc_YY47(({unsigned _tmp843=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp842=Cyc_yyget_String_tok(&(yyyvsp[0]).v);unsigned _tmp841=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Atts_parse_unary_att(_tmp843,_tmp842,_tmp841,Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));}));
goto _LL0;case 82: _LL9F: _LLA0:
# 1412 "parse.y"
 yyval=Cyc_YY47(({unsigned _tmp84A=(unsigned)((yyyvsp[0]).l).first_line;unsigned _tmp849=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmp848=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr _tmp847=Cyc_yyget_String_tok(&(yyyvsp[2]).v);unsigned _tmp846=({
unsigned _tmp844=(unsigned)((yyyvsp[4]).l).first_line;Cyc_Parse_cnst2uint(_tmp844,Cyc_yyget_Int_tok(&(yyyvsp[4]).v));});
# 1412
Cyc_Atts_parse_format_att(_tmp84A,_tmp849,_tmp848,_tmp847,_tmp846,({
# 1414
unsigned _tmp845=(unsigned)((yyyvsp[6]).l).first_line;Cyc_Parse_cnst2uint(_tmp845,Cyc_yyget_Int_tok(&(yyyvsp[6]).v));}));}));
goto _LL0;case 83: _LLA1: _LLA2:
# 1424 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 84: _LLA3: _LLA4:
# 1426 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84C=({struct _tuple0*_tmp84B=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_typedef_type(_tmp84B,Cyc_yyget_YY41(&(yyyvsp[1]).v),0,0);});Cyc_Parse_type_spec(_tmp84C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 85: _LLA5: _LLA6:
# 1430 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 86: _LLA7: _LLA8:
# 1431 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 87: _LLA9: _LLAA:
# 1432 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 88: _LLAB: _LLAC:
# 1433 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 89: _LLAD: _LLAE:
# 1434 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 90: _LLAF: _LLB0:
# 1435 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 91: _LLB1: _LLB2:
# 1436 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 92: _LLB3: _LLB4:
# 1437 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 93: _LLB5: _LLB6:
# 1438 "parse.y"
 yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 94: _LLB7: _LLB8:
# 1439 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 95: _LLB9: _LLBA:
# 1440 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 96: _LLBB: _LLBC:
# 1442 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84D=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp84D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 97: _LLBD: _LLBE:
# 1444 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84E=Cyc_Absyn_builtin_type(({const char*_tmp3FA="__builtin_va_list";_tag_fat(_tmp3FA,sizeof(char),18U);}),& Cyc_Kinds_bk);Cyc_Parse_type_spec(_tmp84E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 98: _LLBF: _LLC0:
# 1446 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 99: _LLC1: _LLC2:
# 1448 "parse.y"
 yyval=Cyc_YY21(({void*_tmp84F=Cyc_yyget_YY45(&(yyyvsp[0]).v);Cyc_Parse_type_spec(_tmp84F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 100: _LLC3: _LLC4:
# 1450 "parse.y"
 yyval=Cyc_YY21(({void*_tmp850=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp850,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 101: _LLC5: _LLC6:
# 1452 "parse.y"
 yyval=Cyc_YY21(({void*_tmp851=Cyc_Absyn_new_evar(Cyc_Kinds_kind_to_opt(Cyc_yyget_YY44(&(yyyvsp[2]).v)),0);Cyc_Parse_type_spec(_tmp851,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 102: _LLC7: _LLC8:
# 1454 "parse.y"
 yyval=Cyc_YY21(({void*_tmp854=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->tag=6,({struct Cyc_List_List*_tmp853=({unsigned _tmp852=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)((struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ,_tmp852,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[2]).v)));});_tmp3FB->f1=_tmp853;});_tmp3FB;});Cyc_Parse_type_spec(_tmp854,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1456
goto _LL0;case 103: _LLC9: _LLCA:
# 1457 "parse.y"
 yyval=Cyc_YY21(({void*_tmp855=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp855,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 104: _LLCB: _LLCC:
# 1459 "parse.y"
 yyval=Cyc_YY21(({void*_tmp856=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Kinds_rko,0));Cyc_Parse_type_spec(_tmp856,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 105: _LLCD: _LLCE:
# 1461 "parse.y"
 yyval=Cyc_YY21(({void*_tmp857=Cyc_Absyn_tag_type(Cyc_yyget_YY45(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp857,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 106: _LLCF: _LLD0:
# 1463 "parse.y"
 yyval=Cyc_YY21(({void*_tmp858=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Kinds_iko,0));Cyc_Parse_type_spec(_tmp858,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 107: _LLD1: _LLD2:
# 1465 "parse.y"
 yyval=Cyc_YY21(({void*_tmp859=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp859,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 108: _LLD3: _LLD4:
# 1470 "parse.y"
 yyval=Cyc_YY44(({struct _fat_ptr _tmp85A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Kinds_id_to_kind(_tmp85A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 109: _LLD5: _LLD6: {
# 1474 "parse.y"
unsigned loc=(unsigned)(Cyc_Flags_porting_c_code?((yyyvsp[0]).l).first_line:(int)0U);
yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp6B2;_tmp6B2.print_const=1,_tmp6B2.q_volatile=0,_tmp6B2.q_restrict=0,_tmp6B2.real_const=1,_tmp6B2.loc=loc;_tmp6B2;}));
goto _LL0;}case 110: _LLD7: _LLD8:
# 1476 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp6B3;_tmp6B3.print_const=0,_tmp6B3.q_volatile=1,_tmp6B3.q_restrict=0,_tmp6B3.real_const=0,_tmp6B3.loc=0U;_tmp6B3;}));
goto _LL0;case 111: _LLD9: _LLDA:
# 1477 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp6B4;_tmp6B4.print_const=0,_tmp6B4.q_volatile=0,_tmp6B4.q_restrict=1,_tmp6B4.real_const=0,_tmp6B4.loc=0U;_tmp6B4;}));
goto _LL0;case 112: _LLDB: _LLDC: {
# 1483 "parse.y"
struct Cyc_Absyn_TypeDecl*_tmp3FC=({struct Cyc_Absyn_TypeDecl*_tmp401=_cycalloc(sizeof(*_tmp401));({void*_tmp85F=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->tag=1,({struct Cyc_Absyn_Enumdecl*_tmp85E=({struct Cyc_Absyn_Enumdecl*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp85D=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp3FF->name=_tmp85D;}),({struct Cyc_Core_Opt*_tmp85C=({struct Cyc_Core_Opt*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));({struct Cyc_List_List*_tmp85B=Cyc_yyget_YY49(&(yyyvsp[3]).v);_tmp3FE->v=_tmp85B;});_tmp3FE;});_tmp3FF->fields=_tmp85C;});_tmp3FF;});_tmp400->f1=_tmp85E;});_tmp400;});_tmp401->r=_tmp85F;}),_tmp401->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp401;});struct Cyc_Absyn_TypeDecl*ed=_tmp3FC;
# 1485
yyval=Cyc_YY21(({void*_tmp860=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->tag=10,_tmp3FD->f1=ed,_tmp3FD->f2=0;_tmp3FD;});Cyc_Parse_type_spec(_tmp860,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1487
goto _LL0;}case 113: _LLDD: _LLDE:
# 1488 "parse.y"
 yyval=Cyc_YY21(({void*_tmp861=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(&(yyyvsp[1]).v),0);Cyc_Parse_type_spec(_tmp861,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 114: _LLDF: _LLE0:
# 1490 "parse.y"
 yyval=Cyc_YY21(({void*_tmp862=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY49(&(yyyvsp[2]).v));Cyc_Parse_type_spec(_tmp862,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 115: _LLE1: _LLE2:
# 1496 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_tmp402=_cycalloc(sizeof(*_tmp402));({struct _tuple0*_tmp863=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp402->name=_tmp863;}),_tmp402->tag=0,_tmp402->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp402;}));
goto _LL0;case 116: _LLE3: _LLE4:
# 1498 "parse.y"
 yyval=Cyc_YY48(({struct Cyc_Absyn_Enumfield*_tmp403=_cycalloc(sizeof(*_tmp403));({struct _tuple0*_tmp865=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp403->name=_tmp865;}),({struct Cyc_Absyn_Exp*_tmp864=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp403->tag=_tmp864;}),_tmp403->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp403;}));
goto _LL0;case 117: _LLE5: _LLE6:
# 1502 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp404=_cycalloc(sizeof(*_tmp404));({struct Cyc_Absyn_Enumfield*_tmp866=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp404->hd=_tmp866;}),_tmp404->tl=0;_tmp404;}));
goto _LL0;case 118: _LLE7: _LLE8:
# 1503 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp405=_cycalloc(sizeof(*_tmp405));({struct Cyc_Absyn_Enumfield*_tmp867=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp405->hd=_tmp867;}),_tmp405->tl=0;_tmp405;}));
goto _LL0;case 119: _LLE9: _LLEA:
# 1504 "parse.y"
 yyval=Cyc_YY49(({struct Cyc_List_List*_tmp406=_cycalloc(sizeof(*_tmp406));({struct Cyc_Absyn_Enumfield*_tmp869=Cyc_yyget_YY48(&(yyyvsp[0]).v);_tmp406->hd=_tmp869;}),({struct Cyc_List_List*_tmp868=Cyc_yyget_YY49(&(yyyvsp[2]).v);_tmp406->tl=_tmp868;});_tmp406;}));
goto _LL0;case 120: _LLEB: _LLEC:
# 1510 "parse.y"
 yyval=Cyc_YY21(({void*_tmp86C=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->tag=7,({enum Cyc_Absyn_AggrKind _tmp86B=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp407->f1=_tmp86B;}),({struct Cyc_List_List*_tmp86A=Cyc_yyget_YY25(&(yyyvsp[2]).v);_tmp407->f2=_tmp86A;});_tmp407;});Cyc_Parse_type_spec(_tmp86C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 121: _LLED: _LLEE: {
# 1514 "parse.y"
struct Cyc_List_List*_tmp408=({unsigned _tmp86D=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp86D,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp408;
struct Cyc_List_List*_tmp409=({unsigned _tmp86E=(unsigned)((yyyvsp[4]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp86E,Cyc_yyget_YY41(&(yyyvsp[4]).v));});struct Cyc_List_List*exist_ts=_tmp409;
struct Cyc_Absyn_TypeDecl*_tmp40A=({enum Cyc_Absyn_AggrKind _tmp875=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_tmp874=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp873=ts;struct Cyc_Absyn_AggrdeclImpl*_tmp872=({
struct Cyc_List_List*_tmp871=exist_ts;struct Cyc_List_List*_tmp870=Cyc_yyget_YY51(&(yyyvsp[5]).v);struct Cyc_List_List*_tmp86F=Cyc_yyget_YY25(&(yyyvsp[6]).v);Cyc_Absyn_aggrdecl_impl(_tmp871,_tmp870,_tmp86F,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1);});
# 1516
Cyc_Absyn_aggr_tdecl(_tmp875,Cyc_Absyn_Public,_tmp874,_tmp873,_tmp872,0,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*td=_tmp40A;
# 1519
yyval=Cyc_YY21(({void*_tmp876=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->tag=10,_tmp40B->f1=td,_tmp40B->f2=0;_tmp40B;});Cyc_Parse_type_spec(_tmp876,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1521
goto _LL0;}case 122: _LLEF: _LLF0:
# 1522 "parse.y"
 yyval=Cyc_YY21(({void*_tmp87A=({union Cyc_Absyn_AggrInfo _tmp879=({enum Cyc_Absyn_AggrKind _tmp878=(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f2;struct _tuple0*_tmp877=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_UnknownAggr(_tmp878,_tmp877,(Cyc_yyget_YY23(&(yyyvsp[0]).v)).f1?({struct Cyc_Core_Opt*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->v=(void*)1;_tmp40C;}): 0);});Cyc_Absyn_aggr_type(_tmp879,
Cyc_yyget_YY41(&(yyyvsp[2]).v));});
# 1522
Cyc_Parse_type_spec(_tmp87A,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1524
goto _LL0;case 123: _LLF1: _LLF2:
# 1527 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _tmp6B5;_tmp6B5.f1=1,({enum Cyc_Absyn_AggrKind _tmp87B=Cyc_yyget_YY22(&(yyyvsp[1]).v);_tmp6B5.f2=_tmp87B;});_tmp6B5;}));
goto _LL0;case 124: _LLF3: _LLF4:
# 1528 "parse.y"
 yyval=Cyc_YY23(({struct _tuple25 _tmp6B6;_tmp6B6.f1=0,({enum Cyc_Absyn_AggrKind _tmp87C=Cyc_yyget_YY22(&(yyyvsp[0]).v);_tmp6B6.f2=_tmp87C;});_tmp6B6;}));
goto _LL0;case 125: _LLF5: _LLF6:
# 1531
 yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;case 126: _LLF7: _LLF8:
# 1532 "parse.y"
 yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;case 127: _LLF9: _LLFA:
# 1535
 yyval=Cyc_YY41(0);
goto _LL0;case 128: _LLFB: _LLFC:
# 1536 "parse.y"
 yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 129: _LLFD: _LLFE:
# 1541 "parse.y"
 yyval=Cyc_YY25(0);
goto _LL0;case 130: _LLFF: _LL100: {
# 1543 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp40D=Cyc_yyget_YY26(&(yyyvsp[0]).v);struct Cyc_List_List*x=_tmp40D;for(0;x != 0;x=x->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)((struct Cyc_List_List*)x->hd,decls);}}{
# 1547
struct Cyc_List_List*_tmp40E=Cyc_Parse_get_aggrfield_tags(decls);struct Cyc_List_List*tags=_tmp40E;
if(tags != 0)
((void(*)(void(*)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,tags,decls);
yyval=Cyc_YY25(decls);
# 1552
goto _LL0;}}case 131: _LL101: _LL102:
# 1556 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp40F=_cycalloc(sizeof(*_tmp40F));({struct Cyc_List_List*_tmp87D=Cyc_yyget_YY25(&(yyyvsp[0]).v);_tmp40F->hd=_tmp87D;}),_tmp40F->tl=0;_tmp40F;}));
goto _LL0;case 132: _LL103: _LL104:
# 1557 "parse.y"
 yyval=Cyc_YY26(({struct Cyc_List_List*_tmp410=_cycalloc(sizeof(*_tmp410));({struct Cyc_List_List*_tmp87F=Cyc_yyget_YY25(&(yyyvsp[1]).v);_tmp410->hd=_tmp87F;}),({struct Cyc_List_List*_tmp87E=Cyc_yyget_YY26(&(yyyvsp[0]).v);_tmp410->tl=_tmp87E;});_tmp410;}));
goto _LL0;case 133: _LL105: _LL106:
# 1563 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_tmp411=_region_malloc(yyr,sizeof(*_tmp411));_tmp411->tl=0,({struct _tuple12 _tmp880=Cyc_yyget_YY18(&(yyyvsp[0]).v);_tmp411->hd=_tmp880;});_tmp411;}));
goto _LL0;case 134: _LL107: _LL108:
# 1565 "parse.y"
 yyval=Cyc_YY19(({struct _tuple11*_tmp412=_region_malloc(yyr,sizeof(*_tmp412));({struct _tuple11*_tmp882=Cyc_yyget_YY19(&(yyyvsp[0]).v);_tmp412->tl=_tmp882;}),({struct _tuple12 _tmp881=Cyc_yyget_YY18(&(yyyvsp[2]).v);_tmp412->hd=_tmp881;});_tmp412;}));
goto _LL0;case 135: _LL109: _LL10A:
# 1569 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp6B7;({struct Cyc_Parse_Declarator _tmp883=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp6B7.f1=_tmp883;}),_tmp6B7.f2=0;_tmp6B7;}));
goto _LL0;case 136: _LL10B: _LL10C:
# 1570 "parse.y"
 yyval=Cyc_YY18(({struct _tuple12 _tmp6B8;({struct Cyc_Parse_Declarator _tmp885=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp6B8.f1=_tmp885;}),({struct Cyc_Absyn_Exp*_tmp884=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6B8.f2=_tmp884;});_tmp6B8;}));
goto _LL0;case 137: _LL10D: _LL10E: {
# 1575 "parse.y"
struct _RegionHandle _tmp413=_new_region("temp");struct _RegionHandle*temp=& _tmp413;_push_region(temp);
{struct _tuple27 _tmp414=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp14=_tmp414;struct _tuple27 _tmp415=_stmttmp14;struct Cyc_List_List*_tmp418;struct Cyc_Parse_Type_specifier _tmp417;struct Cyc_Absyn_Tqual _tmp416;_LL449: _tmp416=_tmp415.f1;_tmp417=_tmp415.f2;_tmp418=_tmp415.f3;_LL44A: {struct Cyc_Absyn_Tqual tq=_tmp416;struct Cyc_Parse_Type_specifier tspecs=_tmp417;struct Cyc_List_List*atts=_tmp418;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct _tuple13*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp419=Cyc_yyget_YY30(&(yyyvsp[1]).v);struct Cyc_List_List*x=_tmp419;for(0;x != 0;x=x->tl){
struct _tuple26*_tmp41A=(struct _tuple26*)x->hd;struct _tuple26*_stmttmp15=_tmp41A;struct _tuple26*_tmp41B=_stmttmp15;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Parse_Declarator _tmp41C;_LL44C: _tmp41C=_tmp41B->f1;_tmp41D=_tmp41B->f2;_tmp41E=_tmp41B->f3;_LL44D: {struct Cyc_Parse_Declarator d=_tmp41C;struct Cyc_Absyn_Exp*wd=_tmp41D;struct Cyc_Absyn_Exp*wh=_tmp41E;
decls=({struct _tuple13*_tmp41F=_region_malloc(temp,sizeof(*_tmp41F));_tmp41F->tl=decls,_tmp41F->hd=d;_tmp41F;});
widths_and_reqs=({struct Cyc_List_List*_tmp421=_region_malloc(temp,sizeof(*_tmp421));({struct _tuple17*_tmp886=({struct _tuple17*_tmp420=_region_malloc(temp,sizeof(*_tmp420));_tmp420->f1=wd,_tmp420->f2=wh;_tmp420;});_tmp421->hd=_tmp886;}),_tmp421->tl=widths_and_reqs;_tmp421;});}}}{
# 1585
void*_tmp422=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp422;
struct Cyc_List_List*_tmp423=({struct _RegionHandle*_tmp889=temp;struct _RegionHandle*_tmp888=temp;struct Cyc_List_List*_tmp887=
Cyc_Parse_apply_tmss(temp,tq,t,decls,atts);
# 1586
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_rzip)(_tmp889,_tmp888,_tmp887,widths_and_reqs);});struct Cyc_List_List*info=_tmp423;
# 1588
yyval=Cyc_YY25(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*)(unsigned,struct _tuple18*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned)((yyyvsp[0]).l).first_line,info));
# 1590
_npop_handler(0);goto _LL0;}}}}
# 1576
;_pop_region();}case 138: _LL10F: _LL110:
# 1597 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6B9;({struct Cyc_Absyn_Tqual _tmp88B=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6B9.f1=_tmp88B;}),({struct Cyc_Parse_Type_specifier _tmp88A=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp6B9.f2=_tmp88A;}),_tmp6B9.f3=0;_tmp6B9;}));
goto _LL0;case 139: _LL111: _LL112: {
# 1599 "parse.y"
struct _tuple27 _tmp424=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp424;yyval=Cyc_YY36(({struct _tuple27 _tmp6BA;_tmp6BA.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp88E=({unsigned _tmp88D=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp88C=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp88D,_tmp88C,two.f2);});_tmp6BA.f2=_tmp88E;}),_tmp6BA.f3=two.f3;_tmp6BA;}));
goto _LL0;}case 140: _LL113: _LL114:
# 1601 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6BB;({struct Cyc_Absyn_Tqual _tmp890=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp6BB.f1=_tmp890;}),({struct Cyc_Parse_Type_specifier _tmp88F=Cyc_Parse_empty_spec(0U);_tmp6BB.f2=_tmp88F;}),_tmp6BB.f3=0;_tmp6BB;}));
goto _LL0;case 141: _LL115: _LL116: {
# 1603 "parse.y"
struct _tuple27 _tmp425=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp425;yyval=Cyc_YY36(({struct _tuple27 _tmp6BC;({struct Cyc_Absyn_Tqual _tmp892=({struct Cyc_Absyn_Tqual _tmp891=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp891,two.f1);});_tmp6BC.f1=_tmp892;}),_tmp6BC.f2=two.f2,_tmp6BC.f3=two.f3;_tmp6BC;}));
goto _LL0;}case 142: _LL117: _LL118:
# 1605 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6BD;({struct Cyc_Absyn_Tqual _tmp895=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6BD.f1=_tmp895;}),({struct Cyc_Parse_Type_specifier _tmp894=Cyc_Parse_empty_spec(0U);_tmp6BD.f2=_tmp894;}),({struct Cyc_List_List*_tmp893=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp6BD.f3=_tmp893;});_tmp6BD;}));
goto _LL0;case 143: _LL119: _LL11A: {
# 1607 "parse.y"
struct _tuple27 _tmp426=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp426;yyval=Cyc_YY36(({struct _tuple27 _tmp6BE;_tmp6BE.f1=two.f1,_tmp6BE.f2=two.f2,({struct Cyc_List_List*_tmp897=({struct Cyc_List_List*_tmp896=Cyc_yyget_YY46(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp896,two.f3);});_tmp6BE.f3=_tmp897;});_tmp6BE;}));
goto _LL0;}case 144: _LL11B: _LL11C:
# 1613 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6BF;({struct Cyc_Absyn_Tqual _tmp899=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6BF.f1=_tmp899;}),({struct Cyc_Parse_Type_specifier _tmp898=Cyc_yyget_YY21(&(yyyvsp[0]).v);_tmp6BF.f2=_tmp898;}),_tmp6BF.f3=0;_tmp6BF;}));
goto _LL0;case 145: _LL11D: _LL11E: {
# 1615 "parse.y"
struct _tuple27 _tmp427=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp427;yyval=Cyc_YY36(({struct _tuple27 _tmp6C0;_tmp6C0.f1=two.f1,({struct Cyc_Parse_Type_specifier _tmp89C=({unsigned _tmp89B=(unsigned)((yyyvsp[0]).l).first_line;struct Cyc_Parse_Type_specifier _tmp89A=Cyc_yyget_YY21(&(yyyvsp[0]).v);Cyc_Parse_combine_specifiers(_tmp89B,_tmp89A,two.f2);});_tmp6C0.f2=_tmp89C;}),_tmp6C0.f3=two.f3;_tmp6C0;}));
goto _LL0;}case 146: _LL11F: _LL120:
# 1617 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6C1;({struct Cyc_Absyn_Tqual _tmp89E=Cyc_yyget_YY24(&(yyyvsp[0]).v);_tmp6C1.f1=_tmp89E;}),({struct Cyc_Parse_Type_specifier _tmp89D=Cyc_Parse_empty_spec(0U);_tmp6C1.f2=_tmp89D;}),_tmp6C1.f3=0;_tmp6C1;}));
goto _LL0;case 147: _LL121: _LL122: {
# 1619 "parse.y"
struct _tuple27 _tmp428=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp428;yyval=Cyc_YY36(({struct _tuple27 _tmp6C2;({struct Cyc_Absyn_Tqual _tmp8A0=({struct Cyc_Absyn_Tqual _tmp89F=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp89F,two.f1);});_tmp6C2.f1=_tmp8A0;}),_tmp6C2.f2=two.f2,_tmp6C2.f3=two.f3;_tmp6C2;}));
goto _LL0;}case 148: _LL123: _LL124:
# 1621 "parse.y"
 yyval=Cyc_YY36(({struct _tuple27 _tmp6C3;({struct Cyc_Absyn_Tqual _tmp8A3=Cyc_Absyn_empty_tqual((unsigned)((yyyvsp[0]).l).first_line);_tmp6C3.f1=_tmp8A3;}),({struct Cyc_Parse_Type_specifier _tmp8A2=Cyc_Parse_empty_spec(0U);_tmp6C3.f2=_tmp8A2;}),({struct Cyc_List_List*_tmp8A1=Cyc_yyget_YY46(&(yyyvsp[0]).v);_tmp6C3.f3=_tmp8A1;});_tmp6C3;}));
goto _LL0;case 149: _LL125: _LL126: {
# 1623 "parse.y"
struct _tuple27 _tmp429=Cyc_yyget_YY36(&(yyyvsp[1]).v);struct _tuple27 two=_tmp429;yyval=Cyc_YY36(({struct _tuple27 _tmp6C4;_tmp6C4.f1=two.f1,_tmp6C4.f2=two.f2,({struct Cyc_List_List*_tmp8A5=({struct Cyc_List_List*_tmp8A4=Cyc_yyget_YY46(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp8A4,two.f3);});_tmp6C4.f3=_tmp8A5;});_tmp6C4;}));
goto _LL0;}case 150: _LL127: _LL128:
# 1629 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_tmp42A=_region_malloc(yyr,sizeof(*_tmp42A));({struct _tuple26*_tmp8A6=Cyc_yyget_YY29(&(yyyvsp[0]).v);_tmp42A->hd=_tmp8A6;}),_tmp42A->tl=0;_tmp42A;}));
goto _LL0;case 151: _LL129: _LL12A:
# 1631 "parse.y"
 yyval=Cyc_YY30(({struct Cyc_List_List*_tmp42B=_region_malloc(yyr,sizeof(*_tmp42B));({struct _tuple26*_tmp8A8=Cyc_yyget_YY29(&(yyyvsp[2]).v);_tmp42B->hd=_tmp8A8;}),({struct Cyc_List_List*_tmp8A7=Cyc_yyget_YY30(&(yyyvsp[0]).v);_tmp42B->tl=_tmp8A7;});_tmp42B;}));
goto _LL0;case 152: _LL12B: _LL12C:
# 1636 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp42C=_region_malloc(yyr,sizeof(*_tmp42C));({struct Cyc_Parse_Declarator _tmp8AA=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp42C->f1=_tmp8AA;}),_tmp42C->f2=0,({struct Cyc_Absyn_Exp*_tmp8A9=Cyc_yyget_YY58(&(yyyvsp[1]).v);_tmp42C->f3=_tmp8A9;});_tmp42C;}));
goto _LL0;case 153: _LL12D: _LL12E:
# 1640 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp430=_region_malloc(yyr,sizeof(*_tmp430));({struct _tuple0*_tmp8AF=({struct _tuple0*_tmp42F=_cycalloc(sizeof(*_tmp42F));({union Cyc_Absyn_Nmspace _tmp8AE=Cyc_Absyn_Rel_n(0);_tmp42F->f1=_tmp8AE;}),({struct _fat_ptr*_tmp8AD=({struct _fat_ptr*_tmp42E=_cycalloc(sizeof(*_tmp42E));({struct _fat_ptr _tmp8AC=({const char*_tmp42D="";_tag_fat(_tmp42D,sizeof(char),1U);});*_tmp42E=_tmp8AC;});_tmp42E;});_tmp42F->f2=_tmp8AD;});_tmp42F;});(_tmp430->f1).id=_tmp8AF;}),(_tmp430->f1).varloc=0U,(_tmp430->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp8AB=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp430->f2=_tmp8AB;}),_tmp430->f3=0;_tmp430;}));
# 1642
goto _LL0;case 154: _LL12F: _LL130:
# 1645 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp434=_region_malloc(yyr,sizeof(*_tmp434));({struct _tuple0*_tmp8B3=({struct _tuple0*_tmp433=_cycalloc(sizeof(*_tmp433));({union Cyc_Absyn_Nmspace _tmp8B2=Cyc_Absyn_Rel_n(0);_tmp433->f1=_tmp8B2;}),({struct _fat_ptr*_tmp8B1=({struct _fat_ptr*_tmp432=_cycalloc(sizeof(*_tmp432));({struct _fat_ptr _tmp8B0=({const char*_tmp431="";_tag_fat(_tmp431,sizeof(char),1U);});*_tmp432=_tmp8B0;});_tmp432;});_tmp433->f2=_tmp8B1;});_tmp433;});(_tmp434->f1).id=_tmp8B3;}),(_tmp434->f1).varloc=0U,(_tmp434->f1).tms=0,_tmp434->f2=0,_tmp434->f3=0;_tmp434;}));
# 1647
goto _LL0;case 155: _LL131: _LL132:
# 1648 "parse.y"
 yyval=Cyc_YY29(({struct _tuple26*_tmp435=_region_malloc(yyr,sizeof(*_tmp435));({struct Cyc_Parse_Declarator _tmp8B5=Cyc_yyget_YY28(&(yyyvsp[0]).v);_tmp435->f1=_tmp8B5;}),({struct Cyc_Absyn_Exp*_tmp8B4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp435->f2=_tmp8B4;}),_tmp435->f3=0;_tmp435;}));
goto _LL0;case 156: _LL133: _LL134:
# 1652 "parse.y"
 yyval=Cyc_YY58(Cyc_yyget_Exp_tok(&(yyyvsp[2]).v));
goto _LL0;case 157: _LL135: _LL136:
# 1653 "parse.y"
 yyval=Cyc_YY58(0);
goto _LL0;case 158: _LL137: _LL138: {
# 1659 "parse.y"
int _tmp436=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp436;
struct Cyc_List_List*_tmp437=({unsigned _tmp8B6=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8B6,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*ts=_tmp437;
struct Cyc_Absyn_TypeDecl*_tmp438=({struct _tuple0*_tmp8BB=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp8BA=ts;struct Cyc_Core_Opt*_tmp8B9=({struct Cyc_Core_Opt*_tmp43A=_cycalloc(sizeof(*_tmp43A));({struct Cyc_List_List*_tmp8B7=Cyc_yyget_YY35(&(yyyvsp[4]).v);_tmp43A->v=_tmp8B7;});_tmp43A;});int _tmp8B8=is_extensible;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp8BB,_tmp8BA,_tmp8B9,_tmp8B8,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_TypeDecl*dd=_tmp438;
# 1663
yyval=Cyc_YY21(({void*_tmp8BC=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->tag=10,_tmp439->f1=dd,_tmp439->f2=0;_tmp439;});Cyc_Parse_type_spec(_tmp8BC,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1665
goto _LL0;}case 159: _LL139: _LL13A: {
# 1666 "parse.y"
int _tmp43B=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp43B;
yyval=Cyc_YY21(({void*_tmp8BF=({union Cyc_Absyn_DatatypeInfo _tmp8BE=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp6C5;({struct _tuple0*_tmp8BD=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp6C5.name=_tmp8BD;}),_tmp6C5.is_extensible=is_extensible;_tmp6C5;}));Cyc_Absyn_datatype_type(_tmp8BE,Cyc_yyget_YY41(&(yyyvsp[2]).v));});Cyc_Parse_type_spec(_tmp8BF,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1669
goto _LL0;}case 160: _LL13B: _LL13C: {
# 1670 "parse.y"
int _tmp43C=Cyc_yyget_YY32(&(yyyvsp[0]).v);int is_extensible=_tmp43C;
yyval=Cyc_YY21(({void*_tmp8C3=({union Cyc_Absyn_DatatypeFieldInfo _tmp8C2=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp6C6;({struct _tuple0*_tmp8C1=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp6C6.datatype_name=_tmp8C1;}),({struct _tuple0*_tmp8C0=Cyc_yyget_QualId_tok(&(yyyvsp[3]).v);_tmp6C6.field_name=_tmp8C0;}),_tmp6C6.is_extensible=is_extensible;_tmp6C6;}));Cyc_Absyn_datatype_field_type(_tmp8C2,Cyc_yyget_YY41(&(yyyvsp[4]).v));});Cyc_Parse_type_spec(_tmp8C3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 1673
goto _LL0;}case 161: _LL13D: _LL13E:
# 1676 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 162: _LL13F: _LL140:
# 1677 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 163: _LL141: _LL142:
# 1681 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp43D=_cycalloc(sizeof(*_tmp43D));({struct Cyc_Absyn_Datatypefield*_tmp8C4=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp43D->hd=_tmp8C4;}),_tmp43D->tl=0;_tmp43D;}));
goto _LL0;case 164: _LL143: _LL144:
# 1682 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp43E=_cycalloc(sizeof(*_tmp43E));({struct Cyc_Absyn_Datatypefield*_tmp8C5=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp43E->hd=_tmp8C5;}),_tmp43E->tl=0;_tmp43E;}));
goto _LL0;case 165: _LL145: _LL146:
# 1683 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp43F=_cycalloc(sizeof(*_tmp43F));({struct Cyc_Absyn_Datatypefield*_tmp8C7=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp43F->hd=_tmp8C7;}),({struct Cyc_List_List*_tmp8C6=Cyc_yyget_YY35(&(yyyvsp[2]).v);_tmp43F->tl=_tmp8C6;});_tmp43F;}));
goto _LL0;case 166: _LL147: _LL148:
# 1684 "parse.y"
 yyval=Cyc_YY35(({struct Cyc_List_List*_tmp440=_cycalloc(sizeof(*_tmp440));({struct Cyc_Absyn_Datatypefield*_tmp8C9=Cyc_yyget_YY34(&(yyyvsp[0]).v);_tmp440->hd=_tmp8C9;}),({struct Cyc_List_List*_tmp8C8=Cyc_yyget_YY35(&(yyyvsp[2]).v);_tmp440->tl=_tmp8C8;});_tmp440;}));
goto _LL0;case 167: _LL149: _LL14A:
# 1688 "parse.y"
 yyval=Cyc_YY33(Cyc_Absyn_Public);
goto _LL0;case 168: _LL14B: _LL14C:
# 1689 "parse.y"
 yyval=Cyc_YY33(Cyc_Absyn_Extern);
goto _LL0;case 169: _LL14D: _LL14E:
# 1690 "parse.y"
 yyval=Cyc_YY33(Cyc_Absyn_Static);
goto _LL0;case 170: _LL14F: _LL150:
# 1694 "parse.y"
 yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_tmp441=_cycalloc(sizeof(*_tmp441));({struct _tuple0*_tmp8CB=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp441->name=_tmp8CB;}),_tmp441->typs=0,_tmp441->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp8CA=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp441->sc=_tmp8CA;});_tmp441;}));
goto _LL0;case 171: _LL151: _LL152: {
# 1696 "parse.y"
struct Cyc_List_List*_tmp442=({unsigned _tmp8CC=(unsigned)((yyyvsp[3]).l).first_line;((struct Cyc_List_List*(*)(struct _tuple20*(*)(unsigned,struct _tuple8*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)((struct _tuple20*(*)(unsigned,struct _tuple8*))Cyc_Parse_get_tqual_typ,_tmp8CC,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[3]).v)));});struct Cyc_List_List*typs=_tmp442;
yyval=Cyc_YY34(({struct Cyc_Absyn_Datatypefield*_tmp443=_cycalloc(sizeof(*_tmp443));({struct _tuple0*_tmp8CE=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);_tmp443->name=_tmp8CE;}),_tmp443->typs=typs,_tmp443->loc=(unsigned)((yyyvsp[0]).l).first_line,({enum Cyc_Absyn_Scope _tmp8CD=Cyc_yyget_YY33(&(yyyvsp[0]).v);_tmp443->sc=_tmp8CD;});_tmp443;}));
goto _LL0;}case 172: _LL153: _LL154:
# 1702 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 173: _LL155: _LL156: {
# 1704 "parse.y"
struct Cyc_Parse_Declarator _tmp444=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp444;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C7;_tmp6C7.id=two.id,_tmp6C7.varloc=two.varloc,({struct Cyc_List_List*_tmp8D0=({struct Cyc_List_List*_tmp8CF=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp8CF,two.tms);});_tmp6C7.tms=_tmp8D0;});_tmp6C7;}));
goto _LL0;}case 174: _LL157: _LL158:
# 1711 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 175: _LL159: _LL15A: {
# 1713 "parse.y"
struct Cyc_Parse_Declarator _tmp445=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator two=_tmp445;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C8;_tmp6C8.id=two.id,_tmp6C8.varloc=two.varloc,({struct Cyc_List_List*_tmp8D2=({struct Cyc_List_List*_tmp8D1=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp8D1,two.tms);});_tmp6C8.tms=_tmp8D2;});_tmp6C8;}));
goto _LL0;}case 176: _LL15B: _LL15C:
# 1718 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6C9;({struct _tuple0*_tmp8D3=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6C9.id=_tmp8D3;}),_tmp6C9.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6C9.tms=0;_tmp6C9;}));
goto _LL0;case 177: _LL15D: _LL15E:
# 1719 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 178: _LL15F: _LL160: {
# 1723 "parse.y"
struct Cyc_Parse_Declarator _tmp446=Cyc_yyget_YY28(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp446;
({struct Cyc_List_List*_tmp8D6=({struct Cyc_List_List*_tmp448=_region_malloc(yyr,sizeof(*_tmp448));({void*_tmp8D5=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp447=_region_malloc(yyr,sizeof(*_tmp447));_tmp447->tag=5,_tmp447->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8D4=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp447->f2=_tmp8D4;});_tmp447;});_tmp448->hd=_tmp8D5;}),_tmp448->tl=d.tms;_tmp448;});d.tms=_tmp8D6;});
yyval=(yyyvsp[2]).v;
# 1727
goto _LL0;}case 179: _LL161: _LL162:
# 1728 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6CA;({struct _tuple0*_tmp8DC=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6CA.id=_tmp8DC;}),({unsigned _tmp8DB=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6CA.varloc=_tmp8DB;}),({struct Cyc_List_List*_tmp8DA=({struct Cyc_List_List*_tmp44A=_region_malloc(yyr,sizeof(*_tmp44A));({void*_tmp8D9=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp449=_region_malloc(yyr,sizeof(*_tmp449));_tmp449->tag=0,({void*_tmp8D8=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp449->f1=_tmp8D8;}),_tmp449->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp449;});_tmp44A->hd=_tmp8D9;}),({struct Cyc_List_List*_tmp8D7=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp44A->tl=_tmp8D7;});_tmp44A;});_tmp6CA.tms=_tmp8DA;});_tmp6CA;}));
goto _LL0;case 180: _LL163: _LL164:
# 1730 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6CB;({struct _tuple0*_tmp8E3=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6CB.id=_tmp8E3;}),({unsigned _tmp8E2=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6CB.varloc=_tmp8E2;}),({
struct Cyc_List_List*_tmp8E1=({struct Cyc_List_List*_tmp44C=_region_malloc(yyr,sizeof(*_tmp44C));({void*_tmp8E0=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp44B=_region_malloc(yyr,sizeof(*_tmp44B));_tmp44B->tag=1,({struct Cyc_Absyn_Exp*_tmp8DF=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp44B->f1=_tmp8DF;}),({void*_tmp8DE=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp44B->f2=_tmp8DE;}),_tmp44B->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp44B;});_tmp44C->hd=_tmp8E0;}),({struct Cyc_List_List*_tmp8DD=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp44C->tl=_tmp8DD;});_tmp44C;});_tmp6CB.tms=_tmp8E1;});_tmp6CB;}));
goto _LL0;case 181: _LL165: _LL166: {
# 1733 "parse.y"
struct _tuple28*_tmp44D=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp16=_tmp44D;struct _tuple28*_tmp44E=_stmttmp16;struct Cyc_List_List*_tmp453;void*_tmp452;struct Cyc_Absyn_VarargInfo*_tmp451;int _tmp450;struct Cyc_List_List*_tmp44F;_LL44F: _tmp44F=_tmp44E->f1;_tmp450=_tmp44E->f2;_tmp451=_tmp44E->f3;_tmp452=_tmp44E->f4;_tmp453=_tmp44E->f5;_LL450: {struct Cyc_List_List*lis=_tmp44F;int b=_tmp450;struct Cyc_Absyn_VarargInfo*c=_tmp451;void*eff=_tmp452;struct Cyc_List_List*po=_tmp453;
struct Cyc_Absyn_Exp*_tmp454=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;struct Cyc_Absyn_Exp*req=_tmp454;
struct Cyc_Absyn_Exp*_tmp455=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;struct Cyc_Absyn_Exp*ens=_tmp455;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6CC;({struct _tuple0*_tmp8E9=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6CC.id=_tmp8E9;}),({unsigned _tmp8E8=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6CC.varloc=_tmp8E8;}),({struct Cyc_List_List*_tmp8E7=({struct Cyc_List_List*_tmp458=_region_malloc(yyr,sizeof(*_tmp458));({void*_tmp8E6=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp457=_region_malloc(yyr,sizeof(*_tmp457));_tmp457->tag=3,({void*_tmp8E5=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp456=_region_malloc(yyr,sizeof(*_tmp456));_tmp456->tag=1,_tmp456->f1=lis,_tmp456->f2=b,_tmp456->f3=c,_tmp456->f4=eff,_tmp456->f5=po,_tmp456->f6=req,_tmp456->f7=ens;_tmp456;});_tmp457->f1=_tmp8E5;});_tmp457;});_tmp458->hd=_tmp8E6;}),({struct Cyc_List_List*_tmp8E4=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp458->tl=_tmp8E4;});_tmp458;});_tmp6CC.tms=_tmp8E7;});_tmp6CC;}));
# 1738
goto _LL0;}}case 182: _LL167: _LL168:
# 1739 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6CD;({struct _tuple0*_tmp8F0=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6CD.id=_tmp8F0;}),({unsigned _tmp8EF=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6CD.varloc=_tmp8EF;}),({struct Cyc_List_List*_tmp8EE=({struct Cyc_List_List*_tmp45B=_region_malloc(yyr,sizeof(*_tmp45B));({void*_tmp8ED=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp45A=_region_malloc(yyr,sizeof(*_tmp45A));_tmp45A->tag=3,({void*_tmp8EC=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp459=_region_malloc(yyr,sizeof(*_tmp459));_tmp459->tag=0,({struct Cyc_List_List*_tmp8EB=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp459->f1=_tmp8EB;}),_tmp459->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp459;});_tmp45A->f1=_tmp8EC;});_tmp45A;});_tmp45B->hd=_tmp8ED;}),({struct Cyc_List_List*_tmp8EA=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp45B->tl=_tmp8EA;});_tmp45B;});_tmp6CD.tms=_tmp8EE;});_tmp6CD;}));
goto _LL0;case 183: _LL169: _LL16A: {
# 1742
struct Cyc_List_List*_tmp45C=({unsigned _tmp8F1=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8F1,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp45C;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6CE;({struct _tuple0*_tmp8F6=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6CE.id=_tmp8F6;}),({unsigned _tmp8F5=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6CE.varloc=_tmp8F5;}),({struct Cyc_List_List*_tmp8F4=({struct Cyc_List_List*_tmp45E=_region_malloc(yyr,sizeof(*_tmp45E));({void*_tmp8F3=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp45D=_region_malloc(yyr,sizeof(*_tmp45D));_tmp45D->tag=4,_tmp45D->f1=ts,_tmp45D->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp45D->f3=0;_tmp45D;});_tmp45E->hd=_tmp8F3;}),({struct Cyc_List_List*_tmp8F2=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp45E->tl=_tmp8F2;});_tmp45E;});_tmp6CE.tms=_tmp8F4;});_tmp6CE;}));
# 1745
goto _LL0;}case 184: _LL16B: _LL16C:
# 1746 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6CF;({struct _tuple0*_tmp8FC=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).id;_tmp6CF.id=_tmp8FC;}),({unsigned _tmp8FB=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).varloc;_tmp6CF.varloc=_tmp8FB;}),({struct Cyc_List_List*_tmp8FA=({struct Cyc_List_List*_tmp460=_region_malloc(yyr,sizeof(*_tmp460));({void*_tmp8F9=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp45F=_region_malloc(yyr,sizeof(*_tmp45F));_tmp45F->tag=5,_tmp45F->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8F8=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp45F->f2=_tmp8F8;});_tmp45F;});_tmp460->hd=_tmp8F9;}),({
struct Cyc_List_List*_tmp8F7=(Cyc_yyget_YY28(&(yyyvsp[0]).v)).tms;_tmp460->tl=_tmp8F7;});_tmp460;});
# 1746
_tmp6CF.tms=_tmp8FA;});_tmp6CF;}));
# 1749
goto _LL0;case 185: _LL16D: _LL16E:
# 1753 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D0;({struct _tuple0*_tmp8FD=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6D0.id=_tmp8FD;}),_tmp6D0.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6D0.tms=0;_tmp6D0;}));
goto _LL0;case 186: _LL16F: _LL170:
# 1754 "parse.y"
 yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D1;({struct _tuple0*_tmp8FE=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp6D1.id=_tmp8FE;}),_tmp6D1.varloc=(unsigned)((yyyvsp[0]).l).first_line,_tmp6D1.tms=0;_tmp6D1;}));
goto _LL0;case 187: _LL171: _LL172:
# 1755 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 188: _LL173: _LL174: {
# 1759 "parse.y"
struct Cyc_Parse_Declarator _tmp461=Cyc_yyget_YY28(&(yyyvsp[2]).v);struct Cyc_Parse_Declarator d=_tmp461;
({struct Cyc_List_List*_tmp901=({struct Cyc_List_List*_tmp463=_region_malloc(yyr,sizeof(*_tmp463));({void*_tmp900=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp462=_region_malloc(yyr,sizeof(*_tmp462));_tmp462->tag=5,_tmp462->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp8FF=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp462->f2=_tmp8FF;});_tmp462;});_tmp463->hd=_tmp900;}),_tmp463->tl=d.tms;_tmp463;});d.tms=_tmp901;});
yyval=(yyyvsp[2]).v;
# 1763
goto _LL0;}case 189: _LL175: _LL176: {
# 1764 "parse.y"
struct Cyc_Parse_Declarator _tmp464=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp464;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D2;_tmp6D2.id=one.id,_tmp6D2.varloc=one.varloc,({
struct Cyc_List_List*_tmp904=({struct Cyc_List_List*_tmp466=_region_malloc(yyr,sizeof(*_tmp466));({void*_tmp903=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp465=_region_malloc(yyr,sizeof(*_tmp465));_tmp465->tag=0,({void*_tmp902=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp465->f1=_tmp902;}),_tmp465->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp465;});_tmp466->hd=_tmp903;}),_tmp466->tl=one.tms;_tmp466;});_tmp6D2.tms=_tmp904;});_tmp6D2;}));
goto _LL0;}case 190: _LL177: _LL178: {
# 1768 "parse.y"
struct Cyc_Parse_Declarator _tmp467=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp467;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D3;_tmp6D3.id=one.id,_tmp6D3.varloc=one.varloc,({
struct Cyc_List_List*_tmp908=({struct Cyc_List_List*_tmp469=_region_malloc(yyr,sizeof(*_tmp469));({void*_tmp907=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp468=_region_malloc(yyr,sizeof(*_tmp468));_tmp468->tag=1,({struct Cyc_Absyn_Exp*_tmp906=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp468->f1=_tmp906;}),({void*_tmp905=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp468->f2=_tmp905;}),_tmp468->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp468;});_tmp469->hd=_tmp907;}),_tmp469->tl=one.tms;_tmp469;});_tmp6D3.tms=_tmp908;});_tmp6D3;}));
# 1772
goto _LL0;}case 191: _LL179: _LL17A: {
# 1773 "parse.y"
struct _tuple28*_tmp46A=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp17=_tmp46A;struct _tuple28*_tmp46B=_stmttmp17;struct Cyc_List_List*_tmp470;void*_tmp46F;struct Cyc_Absyn_VarargInfo*_tmp46E;int _tmp46D;struct Cyc_List_List*_tmp46C;_LL452: _tmp46C=_tmp46B->f1;_tmp46D=_tmp46B->f2;_tmp46E=_tmp46B->f3;_tmp46F=_tmp46B->f4;_tmp470=_tmp46B->f5;_LL453: {struct Cyc_List_List*lis=_tmp46C;int b=_tmp46D;struct Cyc_Absyn_VarargInfo*c=_tmp46E;void*eff=_tmp46F;struct Cyc_List_List*po=_tmp470;
struct Cyc_Absyn_Exp*_tmp471=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;struct Cyc_Absyn_Exp*req=_tmp471;
struct Cyc_Absyn_Exp*_tmp472=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;struct Cyc_Absyn_Exp*ens=_tmp472;
struct Cyc_Parse_Declarator _tmp473=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp473;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D4;_tmp6D4.id=one.id,_tmp6D4.varloc=one.varloc,({struct Cyc_List_List*_tmp90B=({struct Cyc_List_List*_tmp476=_region_malloc(yyr,sizeof(*_tmp476));({void*_tmp90A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp475=_region_malloc(yyr,sizeof(*_tmp475));_tmp475->tag=3,({void*_tmp909=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp474=_region_malloc(yyr,sizeof(*_tmp474));_tmp474->tag=1,_tmp474->f1=lis,_tmp474->f2=b,_tmp474->f3=c,_tmp474->f4=eff,_tmp474->f5=po,_tmp474->f6=req,_tmp474->f7=ens;_tmp474;});_tmp475->f1=_tmp909;});_tmp475;});_tmp476->hd=_tmp90A;}),_tmp476->tl=one.tms;_tmp476;});_tmp6D4.tms=_tmp90B;});_tmp6D4;}));
# 1779
goto _LL0;}}case 192: _LL17B: _LL17C: {
# 1780 "parse.y"
struct Cyc_Parse_Declarator _tmp477=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp477;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D5;_tmp6D5.id=one.id,_tmp6D5.varloc=one.varloc,({struct Cyc_List_List*_tmp90F=({struct Cyc_List_List*_tmp47A=_region_malloc(yyr,sizeof(*_tmp47A));({void*_tmp90E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp479=_region_malloc(yyr,sizeof(*_tmp479));_tmp479->tag=3,({void*_tmp90D=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp478=_region_malloc(yyr,sizeof(*_tmp478));_tmp478->tag=0,({struct Cyc_List_List*_tmp90C=Cyc_yyget_YY37(&(yyyvsp[2]).v);_tmp478->f1=_tmp90C;}),_tmp478->f2=(unsigned)((yyyvsp[0]).l).first_line;_tmp478;});_tmp479->f1=_tmp90D;});_tmp479;});_tmp47A->hd=_tmp90E;}),_tmp47A->tl=one.tms;_tmp47A;});_tmp6D5.tms=_tmp90F;});_tmp6D5;}));
goto _LL0;}case 193: _LL17D: _LL17E: {
# 1784
struct Cyc_List_List*_tmp47B=({unsigned _tmp910=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp910,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp47B;
struct Cyc_Parse_Declarator _tmp47C=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp47C;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D6;_tmp6D6.id=one.id,_tmp6D6.varloc=one.varloc,({struct Cyc_List_List*_tmp912=({struct Cyc_List_List*_tmp47E=_region_malloc(yyr,sizeof(*_tmp47E));({void*_tmp911=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp47D=_region_malloc(yyr,sizeof(*_tmp47D));_tmp47D->tag=4,_tmp47D->f1=ts,_tmp47D->f2=(unsigned)((yyyvsp[0]).l).first_line,_tmp47D->f3=0;_tmp47D;});_tmp47E->hd=_tmp911;}),_tmp47E->tl=one.tms;_tmp47E;});_tmp6D6.tms=_tmp912;});_tmp6D6;}));
goto _LL0;}case 194: _LL17F: _LL180: {
# 1788 "parse.y"
struct Cyc_Parse_Declarator _tmp47F=Cyc_yyget_YY28(&(yyyvsp[0]).v);struct Cyc_Parse_Declarator one=_tmp47F;
yyval=Cyc_YY28(({struct Cyc_Parse_Declarator _tmp6D7;_tmp6D7.id=one.id,_tmp6D7.varloc=one.varloc,({struct Cyc_List_List*_tmp915=({struct Cyc_List_List*_tmp481=_region_malloc(yyr,sizeof(*_tmp481));({void*_tmp914=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp480=_region_malloc(yyr,sizeof(*_tmp480));_tmp480->tag=5,_tmp480->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp913=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp480->f2=_tmp913;});_tmp480;});_tmp481->hd=_tmp914;}),_tmp481->tl=one.tms;_tmp481;});_tmp6D7.tms=_tmp915;});_tmp6D7;}));
goto _LL0;}case 195: _LL181: _LL182:
# 1794 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 196: _LL183: _LL184:
# 1795 "parse.y"
 yyval=Cyc_YY27(({struct Cyc_List_List*_tmp916=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp916,Cyc_yyget_YY27(&(yyyvsp[1]).v));}));
goto _LL0;case 197: _LL185: _LL186: {
# 1799 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY46(&(yyyvsp[3]).v)!= 0)
ans=({struct Cyc_List_List*_tmp483=_region_malloc(yyr,sizeof(*_tmp483));({void*_tmp918=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp482=_region_malloc(yyr,sizeof(*_tmp482));_tmp482->tag=5,_tmp482->f1=(unsigned)((yyyvsp[3]).l).first_line,({struct Cyc_List_List*_tmp917=Cyc_yyget_YY46(&(yyyvsp[3]).v);_tmp482->f2=_tmp917;});_tmp482;});_tmp483->hd=_tmp918;}),_tmp483->tl=ans;_tmp483;});{
# 1803
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple22 _tmp484=*Cyc_yyget_YY1(&(yyyvsp[0]).v);struct _tuple22 _stmttmp18=_tmp484;struct _tuple22 _tmp485=_stmttmp18;void*_tmp488;void*_tmp487;unsigned _tmp486;_LL455: _tmp486=_tmp485.f1;_tmp487=_tmp485.f2;_tmp488=_tmp485.f3;_LL456: {unsigned ploc=_tmp486;void*nullable=_tmp487;void*bound=_tmp488;
if(Cyc_Flags_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->ptr_loc=ploc,_tmp489->rgn_loc=(unsigned)((yyyvsp[2]).l).first_line,_tmp489->zt_loc=(unsigned)((yyyvsp[1]).l).first_line;_tmp489;});{
struct _tuple15 _tmp48A=({unsigned _tmp91C=ploc;void*_tmp91B=nullable;void*_tmp91A=bound;void*_tmp919=Cyc_yyget_YY45(&(yyyvsp[2]).v);Cyc_Parse_collapse_pointer_quals(_tmp91C,_tmp91B,_tmp91A,_tmp919,Cyc_yyget_YY57(&(yyyvsp[1]).v));});struct _tuple15 _stmttmp19=_tmp48A;struct _tuple15 _tmp48B=_stmttmp19;void*_tmp490;void*_tmp48F;void*_tmp48E;void*_tmp48D;void*_tmp48C;_LL458: _tmp48C=_tmp48B.f1;_tmp48D=_tmp48B.f2;_tmp48E=_tmp48B.f3;_tmp48F=_tmp48B.f4;_tmp490=_tmp48B.f5;_LL459: {void*nullable=_tmp48C;void*bound=_tmp48D;void*zeroterm=_tmp48E;void*rgn_opt=_tmp48F;void*released=_tmp490;
ans=({struct Cyc_List_List*_tmp492=_region_malloc(yyr,sizeof(*_tmp492));({void*_tmp91E=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp491=_region_malloc(yyr,sizeof(*_tmp491));_tmp491->tag=2,(_tmp491->f1).rgn=rgn_opt,(_tmp491->f1).nullable=nullable,(_tmp491->f1).bounds=bound,(_tmp491->f1).zero_term=zeroterm,(_tmp491->f1).ptrloc=ptrloc,(_tmp491->f1).released=released,({struct Cyc_Absyn_Tqual _tmp91D=Cyc_yyget_YY24(&(yyyvsp[4]).v);_tmp491->f2=_tmp91D;});_tmp491;});_tmp492->hd=_tmp91E;}),_tmp492->tl=ans;_tmp492;});
yyval=Cyc_YY27(ans);
# 1811
goto _LL0;}}}}}case 198: _LL187: _LL188:
# 1813
 yyval=Cyc_YY57(0);
goto _LL0;case 199: _LL189: _LL18A:
# 1814 "parse.y"
 yyval=Cyc_YY57(({struct Cyc_List_List*_tmp493=_region_malloc(yyr,sizeof(*_tmp493));({void*_tmp920=Cyc_yyget_YY56(&(yyyvsp[0]).v);_tmp493->hd=_tmp920;}),({struct Cyc_List_List*_tmp91F=Cyc_yyget_YY57(&(yyyvsp[1]).v);_tmp493->tl=_tmp91F;});_tmp493;}));
goto _LL0;case 200: _LL18B: _LL18C:
# 1819 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp494=_region_malloc(yyr,sizeof(*_tmp494));_tmp494->tag=0,({struct Cyc_Absyn_Exp*_tmp921=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp494->f1=_tmp921;});_tmp494;}));
goto _LL0;case 201: _LL18D: _LL18E:
# 1820 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp495=_region_malloc(yyr,sizeof(*_tmp495));_tmp495->tag=1,({void*_tmp922=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp495->f1=_tmp922;});_tmp495;}));
goto _LL0;case 202: _LL18F: _LL190:
# 1821 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp496=_region_malloc(yyr,sizeof(*_tmp496));_tmp496->tag=2;_tmp496;}));
goto _LL0;case 203: _LL191: _LL192:
# 1822 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp497=_region_malloc(yyr,sizeof(*_tmp497));_tmp497->tag=3;_tmp497;}));
goto _LL0;case 204: _LL193: _LL194:
# 1823 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Autoreleased_ptrqual_Parse_Pointer_qual_struct*_tmp498=_region_malloc(yyr,sizeof(*_tmp498));_tmp498->tag=6;_tmp498;}));
goto _LL0;case 205: _LL195: _LL196:
# 1824 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp499=_region_malloc(yyr,sizeof(*_tmp499));_tmp499->tag=4;_tmp499;}));
goto _LL0;case 206: _LL197: _LL198:
# 1825 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp49A=_region_malloc(yyr,sizeof(*_tmp49A));_tmp49A->tag=5;_tmp49A;}));
goto _LL0;case 207: _LL199: _LL19A:
# 1826 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp49B=_region_malloc(yyr,sizeof(*_tmp49B));_tmp49B->tag=7;_tmp49B;}));
goto _LL0;case 208: _LL19B: _LL19C:
# 1827 "parse.y"
 yyval=Cyc_YY56((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp49C=_region_malloc(yyr,sizeof(*_tmp49C));_tmp49C->tag=8;_tmp49C;}));
goto _LL0;case 209: _LL19D: _LL19E:
# 1833 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp49D->f2=Cyc_Absyn_true_type,Cyc_Parse_parsing_tempest?_tmp49D->f3=Cyc_Absyn_fat_bound_type:({void*_tmp923=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp49D->f3=_tmp923;});_tmp49D;}));
goto _LL0;case 210: _LL19F: _LL1A0:
# 1834 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp49E->f2=Cyc_Absyn_false_type,({void*_tmp924=Cyc_yyget_YY2(&(yyyvsp[1]).v);_tmp49E->f3=_tmp924;});_tmp49E;}));
goto _LL0;case 211: _LL1A1: _LL1A2:
# 1835 "parse.y"
 yyval=Cyc_YY1(({struct _tuple22*_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F->f1=(unsigned)((yyyvsp[0]).l).first_line,_tmp49F->f2=Cyc_Absyn_true_type,_tmp49F->f3=Cyc_Absyn_fat_bound_type;_tmp49F;}));
goto _LL0;case 212: _LL1A3: _LL1A4:
# 1838
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 213: _LL1A5: _LL1A6:
# 1839 "parse.y"
 yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 214: _LL1A7: _LL1A8:
# 1842
 yyval=Cyc_YY52(Cyc_Tcutil_any_bool(0));
goto _LL0;case 215: _LL1A9: _LL1AA:
# 1843 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_true_type);
goto _LL0;case 216: _LL1AB: _LL1AC:
# 1844 "parse.y"
 yyval=Cyc_YY52(Cyc_Absyn_false_type);
goto _LL0;case 217: _LL1AD: _LL1AE:
# 1849 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 218: _LL1AF: _LL1B0:
# 1850 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(&(yyyvsp[0]).v),& Cyc_Kinds_trk,1);yyval=(yyyvsp[0]).v;
goto _LL0;case 219: _LL1B1: _LL1B2:
# 1851 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_new_evar(& Cyc_Kinds_trko,0));
goto _LL0;case 220: _LL1B3: _LL1B4:
# 1855 "parse.y"
 yyval=Cyc_YY24(Cyc_Absyn_empty_tqual((unsigned)((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset + 1))).l).first_line));
goto _LL0;case 221: _LL1B5: _LL1B6:
# 1856 "parse.y"
 yyval=Cyc_YY24(({struct Cyc_Absyn_Tqual _tmp925=Cyc_yyget_YY24(&(yyyvsp[0]).v);Cyc_Absyn_combine_tqual(_tmp925,Cyc_yyget_YY24(&(yyyvsp[1]).v));}));
goto _LL0;case 222: _LL1B7: _LL1B8:
# 1860 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->f1=0,_tmp4A0->f2=0,_tmp4A0->f3=0,({void*_tmp927=Cyc_yyget_YY50(&(yyyvsp[0]).v);_tmp4A0->f4=_tmp927;}),({struct Cyc_List_List*_tmp926=Cyc_yyget_YY51(&(yyyvsp[1]).v);_tmp4A0->f5=_tmp926;});_tmp4A0;}));
goto _LL0;case 223: _LL1B9: _LL1BA:
# 1862 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));({struct Cyc_List_List*_tmp92A=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[0]).v));_tmp4A1->f1=_tmp92A;}),_tmp4A1->f2=0,_tmp4A1->f3=0,({void*_tmp929=Cyc_yyget_YY50(&(yyyvsp[1]).v);_tmp4A1->f4=_tmp929;}),({struct Cyc_List_List*_tmp928=Cyc_yyget_YY51(&(yyyvsp[2]).v);_tmp4A1->f5=_tmp928;});_tmp4A1;}));
goto _LL0;case 224: _LL1BB: _LL1BC:
# 1864 "parse.y"
 yyval=Cyc_YY40(({struct _tuple28*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));({struct Cyc_List_List*_tmp92D=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[0]).v));_tmp4A2->f1=_tmp92D;}),_tmp4A2->f2=1,_tmp4A2->f3=0,({void*_tmp92C=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4A2->f4=_tmp92C;}),({struct Cyc_List_List*_tmp92B=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4A2->f5=_tmp92B;});_tmp4A2;}));
goto _LL0;case 225: _LL1BD: _LL1BE: {
# 1867
struct _tuple8*_tmp4A3=Cyc_yyget_YY38(&(yyyvsp[2]).v);struct _tuple8*_stmttmp1A=_tmp4A3;struct _tuple8*_tmp4A4=_stmttmp1A;void*_tmp4A7;struct Cyc_Absyn_Tqual _tmp4A6;struct _fat_ptr*_tmp4A5;_LL45B: _tmp4A5=_tmp4A4->f1;_tmp4A6=_tmp4A4->f2;_tmp4A7=_tmp4A4->f3;_LL45C: {struct _fat_ptr*n=_tmp4A5;struct Cyc_Absyn_Tqual tq=_tmp4A6;void*t=_tmp4A7;
struct Cyc_Absyn_VarargInfo*_tmp4A8=({struct Cyc_Absyn_VarargInfo*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->name=n,_tmp4AA->tq=tq,_tmp4AA->type=t,({int _tmp92E=Cyc_yyget_YY32(&(yyyvsp[1]).v);_tmp4AA->inject=_tmp92E;});_tmp4AA;});struct Cyc_Absyn_VarargInfo*v=_tmp4A8;
yyval=Cyc_YY40(({struct _tuple28*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->f1=0,_tmp4A9->f2=0,_tmp4A9->f3=v,({void*_tmp930=Cyc_yyget_YY50(&(yyyvsp[3]).v);_tmp4A9->f4=_tmp930;}),({struct Cyc_List_List*_tmp92F=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4A9->f5=_tmp92F;});_tmp4A9;}));
# 1871
goto _LL0;}}case 226: _LL1BF: _LL1C0: {
# 1873
struct _tuple8*_tmp4AB=Cyc_yyget_YY38(&(yyyvsp[4]).v);struct _tuple8*_stmttmp1B=_tmp4AB;struct _tuple8*_tmp4AC=_stmttmp1B;void*_tmp4AF;struct Cyc_Absyn_Tqual _tmp4AE;struct _fat_ptr*_tmp4AD;_LL45E: _tmp4AD=_tmp4AC->f1;_tmp4AE=_tmp4AC->f2;_tmp4AF=_tmp4AC->f3;_LL45F: {struct _fat_ptr*n=_tmp4AD;struct Cyc_Absyn_Tqual tq=_tmp4AE;void*t=_tmp4AF;
struct Cyc_Absyn_VarargInfo*_tmp4B0=({struct Cyc_Absyn_VarargInfo*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2->name=n,_tmp4B2->tq=tq,_tmp4B2->type=t,({int _tmp931=Cyc_yyget_YY32(&(yyyvsp[3]).v);_tmp4B2->inject=_tmp931;});_tmp4B2;});struct Cyc_Absyn_VarargInfo*v=_tmp4B0;
yyval=Cyc_YY40(({struct _tuple28*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct Cyc_List_List*_tmp934=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY39(&(yyyvsp[0]).v));_tmp4B1->f1=_tmp934;}),_tmp4B1->f2=0,_tmp4B1->f3=v,({void*_tmp933=Cyc_yyget_YY50(&(yyyvsp[5]).v);_tmp4B1->f4=_tmp933;}),({struct Cyc_List_List*_tmp932=Cyc_yyget_YY51(&(yyyvsp[6]).v);_tmp4B1->f5=_tmp932;});_tmp4B1;}));
# 1877
goto _LL0;}}case 227: _LL1C1: _LL1C2:
# 1881 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _tmp935=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp935,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));_tmp4B3->tag=1,_tmp4B3->f1=0;_tmp4B3;}));}));
goto _LL0;case 228: _LL1C3: _LL1C4:
# 1882 "parse.y"
 yyval=Cyc_YY45(({struct _fat_ptr _tmp936=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Parse_id2type(_tmp936,Cyc_Kinds_kind_to_bound(Cyc_yyget_YY44(&(yyyvsp[2]).v)));}));
goto _LL0;case 229: _LL1C5: _LL1C6:
# 1885
 yyval=Cyc_YY50(0);
goto _LL0;case 230: _LL1C7: _LL1C8:
# 1886 "parse.y"
 yyval=Cyc_YY50(Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 231: _LL1C9: _LL1CA:
# 1890 "parse.y"
 yyval=Cyc_YY51(0);
goto _LL0;case 232: _LL1CB: _LL1CC:
# 1891 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 233: _LL1CD: _LL1CE: {
# 1899 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4B4=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8->tag=2,_tmp4B8->f1=0,_tmp4B8->f2=& Cyc_Kinds_trk;_tmp4B8;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp4B4;
void*_tmp4B5=({struct _fat_ptr _tmp937=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp937,(void*)kb);});void*t=_tmp4B5;
yyval=Cyc_YY51(({struct Cyc_List_List*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));({struct _tuple34*_tmp939=({struct _tuple34*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));({void*_tmp938=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_tmp4B6->f1=_tmp938;}),_tmp4B6->f2=t;_tmp4B6;});_tmp4B7->hd=_tmp939;}),_tmp4B7->tl=0;_tmp4B7;}));
# 1903
goto _LL0;}case 234: _LL1CF: _LL1D0: {
# 1905 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4B9=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->tag=2,_tmp4BD->f1=0,_tmp4BD->f2=& Cyc_Kinds_trk;_tmp4BD;});struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*kb=_tmp4B9;
void*_tmp4BA=({struct _fat_ptr _tmp93A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmp93A,(void*)kb);});void*t=_tmp4BA;
yyval=Cyc_YY51(({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));({struct _tuple34*_tmp93D=({struct _tuple34*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));({void*_tmp93C=Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[0]).v));_tmp4BB->f1=_tmp93C;}),_tmp4BB->f2=t;_tmp4BB;});_tmp4BC->hd=_tmp93D;}),({struct Cyc_List_List*_tmp93B=Cyc_yyget_YY51(&(yyyvsp[4]).v);_tmp4BC->tl=_tmp93B;});_tmp4BC;}));
# 1909
goto _LL0;}case 235: _LL1D1: _LL1D2:
# 1913 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 236: _LL1D3: _LL1D4:
# 1915 "parse.y"
 if(({struct _fat_ptr _tmp93E=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_zstrcmp(_tmp93E,({const char*_tmp4BE="inject";_tag_fat(_tmp4BE,sizeof(char),7U);}));})!= 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D8;_tmp6D8.tag=0,({struct _fat_ptr _tmp93F=({const char*_tmp4C1="missing type in function declaration";_tag_fat(_tmp4C1,sizeof(char),37U);});_tmp6D8.f1=_tmp93F;});_tmp6D8;});void*_tmp4BF[1];_tmp4BF[0]=& _tmp4C0;({unsigned _tmp940=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmp940,_tag_fat(_tmp4BF,sizeof(void*),1));});});
yyval=Cyc_YY32(1);
# 1919
goto _LL0;case 237: _LL1D5: _LL1D6:
# 1922 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 238: _LL1D7: _LL1D8:
# 1923 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp941=Cyc_yyget_YY41(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp941,Cyc_yyget_YY41(&(yyyvsp[2]).v));}));
goto _LL0;case 239: _LL1D9: _LL1DA:
# 1927 "parse.y"
 yyval=Cyc_YY41(0);
goto _LL0;case 240: _LL1DB: _LL1DC:
# 1928 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 241: _LL1DD: _LL1DE:
# 1929 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));({void*_tmp942=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(&(yyyvsp[2]).v));_tmp4C2->hd=_tmp942;}),_tmp4C2->tl=0;_tmp4C2;}));
goto _LL0;case 242: _LL1DF: _LL1E0:
# 1931 "parse.y"
 Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY45(&(yyyvsp[0]).v),& Cyc_Kinds_ek,0);
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));({void*_tmp943=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp4C3->hd=_tmp943;}),_tmp4C3->tl=0;_tmp4C3;}));
# 1934
goto _LL0;case 243: _LL1E1: _LL1E2:
# 1939 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));({void*_tmp945=Cyc_Absyn_access_eff(({struct _tuple8*_tmp944=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp944,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp4C4->hd=_tmp945;}),_tmp4C4->tl=0;_tmp4C4;}));
goto _LL0;case 244: _LL1E3: _LL1E4:
# 1941 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));({void*_tmp948=Cyc_Absyn_access_eff(({struct _tuple8*_tmp947=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp947,(unsigned)((yyyvsp[0]).l).first_line);}));_tmp4C5->hd=_tmp948;}),({struct Cyc_List_List*_tmp946=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp4C5->tl=_tmp946;});_tmp4C5;}));
goto _LL0;case 245: _LL1E5: _LL1E6:
# 1946 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));({struct _tuple8*_tmp949=Cyc_yyget_YY38(&(yyyvsp[0]).v);_tmp4C6->hd=_tmp949;}),_tmp4C6->tl=0;_tmp4C6;}));
goto _LL0;case 246: _LL1E7: _LL1E8:
# 1947 "parse.y"
 yyval=Cyc_YY39(({struct Cyc_List_List*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));({struct _tuple8*_tmp94B=Cyc_yyget_YY38(&(yyyvsp[2]).v);_tmp4C7->hd=_tmp94B;}),({struct Cyc_List_List*_tmp94A=Cyc_yyget_YY39(&(yyyvsp[0]).v);_tmp4C7->tl=_tmp94A;});_tmp4C7;}));
goto _LL0;case 247: _LL1E9: _LL1EA: {
# 1953 "parse.y"
struct _tuple27 _tmp4C8=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp1C=_tmp4C8;struct _tuple27 _tmp4C9=_stmttmp1C;struct Cyc_List_List*_tmp4CC;struct Cyc_Parse_Type_specifier _tmp4CB;struct Cyc_Absyn_Tqual _tmp4CA;_LL461: _tmp4CA=_tmp4C9.f1;_tmp4CB=_tmp4C9.f2;_tmp4CC=_tmp4C9.f3;_LL462: {struct Cyc_Absyn_Tqual tq=_tmp4CA;struct Cyc_Parse_Type_specifier tspecs=_tmp4CB;struct Cyc_List_List*atts=_tmp4CC;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
struct Cyc_Parse_Declarator _tmp4CD=Cyc_yyget_YY28(&(yyyvsp[1]).v);struct Cyc_Parse_Declarator _stmttmp1D=_tmp4CD;struct Cyc_Parse_Declarator _tmp4CE=_stmttmp1D;struct Cyc_List_List*_tmp4D1;unsigned _tmp4D0;struct _tuple0*_tmp4CF;_LL464: _tmp4CF=_tmp4CE.id;_tmp4D0=_tmp4CE.varloc;_tmp4D1=_tmp4CE.tms;_LL465: {struct _tuple0*qv=_tmp4CF;unsigned varloc=_tmp4D0;struct Cyc_List_List*tms=_tmp4D1;
void*_tmp4D2=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4D2;
struct _tuple14 _tmp4D3=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp1E=_tmp4D3;struct _tuple14 _tmp4D4=_stmttmp1E;struct Cyc_List_List*_tmp4D8;struct Cyc_List_List*_tmp4D7;void*_tmp4D6;struct Cyc_Absyn_Tqual _tmp4D5;_LL467: _tmp4D5=_tmp4D4.f1;_tmp4D6=_tmp4D4.f2;_tmp4D7=_tmp4D4.f3;_tmp4D8=_tmp4D4.f4;_LL468: {struct Cyc_Absyn_Tqual tq2=_tmp4D5;void*t2=_tmp4D6;struct Cyc_List_List*tvs=_tmp4D7;struct Cyc_List_List*atts2=_tmp4D8;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6D9;_tmp6D9.tag=0,({struct _fat_ptr _tmp94C=({const char*_tmp4DB="parameter with bad type params";_tag_fat(_tmp4DB,sizeof(char),31U);});_tmp6D9.f1=_tmp94C;});_tmp6D9;});void*_tmp4D9[1];_tmp4D9[0]=& _tmp4DA;({unsigned _tmp94D=(unsigned)((yyyvsp[1]).l).first_line;Cyc_Warn_err2(_tmp94D,_tag_fat(_tmp4D9,sizeof(void*),1));});});
if(Cyc_Absyn_is_qvar_qualified(qv))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DA;_tmp6DA.tag=0,({struct _fat_ptr _tmp94E=({const char*_tmp4DE="parameter cannot be qualified with a namespace";_tag_fat(_tmp4DE,sizeof(char),47U);});_tmp6DA.f1=_tmp94E;});_tmp6DA;});void*_tmp4DC[1];_tmp4DC[0]=& _tmp4DD;({unsigned _tmp94F=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_err2(_tmp94F,_tag_fat(_tmp4DC,sizeof(void*),1));});});{
struct _fat_ptr*idopt=(*qv).f2;
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DB;_tmp6DB.tag=0,({struct _fat_ptr _tmp950=({const char*_tmp4E1="extra attributes on parameter, ignoring";_tag_fat(_tmp4E1,sizeof(char),40U);});_tmp6DB.f1=_tmp950;});_tmp6DB;});void*_tmp4DF[1];_tmp4DF[0]=& _tmp4E0;({unsigned _tmp951=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp951,_tag_fat(_tmp4DF,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));_tmp4E2->f1=idopt,_tmp4E2->f2=tq2,_tmp4E2->f3=t2;_tmp4E2;}));
# 1967
goto _LL0;}}}}}}case 248: _LL1EB: _LL1EC: {
# 1968 "parse.y"
struct _tuple27 _tmp4E3=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp1F=_tmp4E3;struct _tuple27 _tmp4E4=_stmttmp1F;struct Cyc_List_List*_tmp4E7;struct Cyc_Parse_Type_specifier _tmp4E6;struct Cyc_Absyn_Tqual _tmp4E5;_LL46A: _tmp4E5=_tmp4E4.f1;_tmp4E6=_tmp4E4.f2;_tmp4E7=_tmp4E4.f3;_LL46B: {struct Cyc_Absyn_Tqual tq=_tmp4E5;struct Cyc_Parse_Type_specifier tspecs=_tmp4E6;struct Cyc_List_List*atts=_tmp4E7;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp4E8=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4E8;
if(atts != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DC;_tmp6DC.tag=0,({struct _fat_ptr _tmp952=({const char*_tmp4EB="bad attributes on parameter, ignoring";_tag_fat(_tmp4EB,sizeof(char),38U);});_tmp6DC.f1=_tmp952;});_tmp6DC;});void*_tmp4E9[1];_tmp4E9[0]=& _tmp4EA;({unsigned _tmp953=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp953,_tag_fat(_tmp4E9,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp4EC=_cycalloc(sizeof(*_tmp4EC));_tmp4EC->f1=0,_tmp4EC->f2=tq,_tmp4EC->f3=t;_tmp4EC;}));
# 1975
goto _LL0;}}}case 249: _LL1ED: _LL1EE: {
# 1976 "parse.y"
struct _tuple27 _tmp4ED=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp20=_tmp4ED;struct _tuple27 _tmp4EE=_stmttmp20;struct Cyc_List_List*_tmp4F1;struct Cyc_Parse_Type_specifier _tmp4F0;struct Cyc_Absyn_Tqual _tmp4EF;_LL46D: _tmp4EF=_tmp4EE.f1;_tmp4F0=_tmp4EE.f2;_tmp4F1=_tmp4EE.f3;_LL46E: {struct Cyc_Absyn_Tqual tq=_tmp4EF;struct Cyc_Parse_Type_specifier tspecs=_tmp4F0;struct Cyc_List_List*atts=_tmp4F1;
if(tq.loc == (unsigned)0)tq.loc=(unsigned)((yyyvsp[0]).l).first_line;{
void*_tmp4F2=Cyc_Parse_speclist2typ(tspecs,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp4F2;
struct Cyc_List_List*_tmp4F3=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp4F3;
struct _tuple14 _tmp4F4=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 _stmttmp21=_tmp4F4;struct _tuple14 _tmp4F5=_stmttmp21;struct Cyc_List_List*_tmp4F9;struct Cyc_List_List*_tmp4F8;void*_tmp4F7;struct Cyc_Absyn_Tqual _tmp4F6;_LL470: _tmp4F6=_tmp4F5.f1;_tmp4F7=_tmp4F5.f2;_tmp4F8=_tmp4F5.f3;_tmp4F9=_tmp4F5.f4;_LL471: {struct Cyc_Absyn_Tqual tq2=_tmp4F6;void*t2=_tmp4F7;struct Cyc_List_List*tvs=_tmp4F8;struct Cyc_List_List*atts2=_tmp4F9;
if(tvs != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DD;_tmp6DD.tag=0,({
struct _fat_ptr _tmp954=({const char*_tmp4FC="bad type parameters on formal argument, ignoring";_tag_fat(_tmp4FC,sizeof(char),49U);});_tmp6DD.f1=_tmp954;});_tmp6DD;});void*_tmp4FA[1];_tmp4FA[0]=& _tmp4FB;({unsigned _tmp955=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp955,_tag_fat(_tmp4FA,sizeof(void*),1));});});
if(atts2 != 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6DE;_tmp6DE.tag=0,({struct _fat_ptr _tmp956=({const char*_tmp4FF="bad attributes on parameter, ignoring";_tag_fat(_tmp4FF,sizeof(char),38U);});_tmp6DE.f1=_tmp956;});_tmp6DE;});void*_tmp4FD[1];_tmp4FD[0]=& _tmp4FE;({unsigned _tmp957=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Warn_warn2(_tmp957,_tag_fat(_tmp4FD,sizeof(void*),1));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->f1=0,_tmp500->f2=tq2,_tmp500->f3=t2;_tmp500;}));
# 1988
goto _LL0;}}}}case 250: _LL1EF: _LL1F0:
# 1991 "parse.y"
 yyval=Cyc_YY37(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY37(&(yyyvsp[0]).v)));
goto _LL0;case 251: _LL1F1: _LL1F2:
# 1994
 yyval=Cyc_YY37(({struct Cyc_List_List*_tmp502=_cycalloc(sizeof(*_tmp502));({struct _fat_ptr*_tmp959=({struct _fat_ptr*_tmp501=_cycalloc(sizeof(*_tmp501));({struct _fat_ptr _tmp958=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp501=_tmp958;});_tmp501;});_tmp502->hd=_tmp959;}),_tmp502->tl=0;_tmp502;}));
goto _LL0;case 252: _LL1F3: _LL1F4:
# 1995 "parse.y"
 yyval=Cyc_YY37(({struct Cyc_List_List*_tmp504=_cycalloc(sizeof(*_tmp504));({struct _fat_ptr*_tmp95C=({struct _fat_ptr*_tmp503=_cycalloc(sizeof(*_tmp503));({struct _fat_ptr _tmp95B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp503=_tmp95B;});_tmp503;});_tmp504->hd=_tmp95C;}),({struct Cyc_List_List*_tmp95A=Cyc_yyget_YY37(&(yyyvsp[0]).v);_tmp504->tl=_tmp95A;});_tmp504;}));
goto _LL0;case 253: _LL1F5: _LL1F6:
# 1999 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 254: _LL1F7: _LL1F8:
# 2000 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 255: _LL1F9: _LL1FA:
# 2005 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp95D=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->tag=36,_tmp505->f1=0,_tmp505->f2=0;_tmp505;});Cyc_Absyn_new_exp(_tmp95D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 256: _LL1FB: _LL1FC:
# 2007 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp95F=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->tag=36,_tmp506->f1=0,({struct Cyc_List_List*_tmp95E=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp506->f2=_tmp95E;});_tmp506;});Cyc_Absyn_new_exp(_tmp95F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 257: _LL1FD: _LL1FE:
# 2009 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmp961=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->tag=36,_tmp507->f1=0,({struct Cyc_List_List*_tmp960=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[1]).v));_tmp507->f2=_tmp960;});_tmp507;});Cyc_Absyn_new_exp(_tmp961,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 258: _LL1FF: _LL200: {
# 2011 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp508=({unsigned _tmp966=(unsigned)((yyyvsp[2]).l).first_line;struct _tuple0*_tmp965=({struct _tuple0*_tmp50B=_cycalloc(sizeof(*_tmp50B));_tmp50B->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp963=({struct _fat_ptr*_tmp50A=_cycalloc(sizeof(*_tmp50A));({struct _fat_ptr _tmp962=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp50A=_tmp962;});_tmp50A;});_tmp50B->f2=_tmp963;});_tmp50B;});void*_tmp964=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmp966,_tmp965,_tmp964,
Cyc_Absyn_uint_exp(0U,(unsigned)((yyyvsp[2]).l).first_line));});
# 2011
struct Cyc_Absyn_Vardecl*vd=_tmp508;
# 2014
(vd->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmp969=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp509=_cycalloc(sizeof(*_tmp509));_tmp509->tag=27,_tmp509->f1=vd,({struct Cyc_Absyn_Exp*_tmp968=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp509->f2=_tmp968;}),({struct Cyc_Absyn_Exp*_tmp967=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp509->f3=_tmp967;}),_tmp509->f4=0;_tmp509;});Cyc_Absyn_new_exp(_tmp969,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2017
goto _LL0;}case 259: _LL201: _LL202: {
# 2019 "parse.y"
void*_tmp50C=({struct _tuple8*_tmp96A=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmp96A,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp50C;
yyval=Cyc_Exp_tok(({void*_tmp96C=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->tag=28,({struct Cyc_Absyn_Exp*_tmp96B=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);_tmp50D->f1=_tmp96B;}),_tmp50D->f2=t,_tmp50D->f3=0;_tmp50D;});Cyc_Absyn_new_exp(_tmp96C,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2022
goto _LL0;}case 260: _LL203: _LL204:
# 2027 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp50F=_cycalloc(sizeof(*_tmp50F));({struct _tuple35*_tmp96E=({struct _tuple35*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=0,({struct Cyc_Absyn_Exp*_tmp96D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp50E->f2=_tmp96D;});_tmp50E;});_tmp50F->hd=_tmp96E;}),_tmp50F->tl=0;_tmp50F;}));
goto _LL0;case 261: _LL205: _LL206:
# 2029 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp511=_cycalloc(sizeof(*_tmp511));({struct _tuple35*_tmp971=({struct _tuple35*_tmp510=_cycalloc(sizeof(*_tmp510));({struct Cyc_List_List*_tmp970=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp510->f1=_tmp970;}),({struct Cyc_Absyn_Exp*_tmp96F=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp510->f2=_tmp96F;});_tmp510;});_tmp511->hd=_tmp971;}),_tmp511->tl=0;_tmp511;}));
goto _LL0;case 262: _LL207: _LL208:
# 2031 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp513=_cycalloc(sizeof(*_tmp513));({struct _tuple35*_tmp974=({struct _tuple35*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->f1=0,({struct Cyc_Absyn_Exp*_tmp973=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp512->f2=_tmp973;});_tmp512;});_tmp513->hd=_tmp974;}),({struct Cyc_List_List*_tmp972=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp513->tl=_tmp972;});_tmp513;}));
goto _LL0;case 263: _LL209: _LL20A:
# 2033 "parse.y"
 yyval=Cyc_YY5(({struct Cyc_List_List*_tmp515=_cycalloc(sizeof(*_tmp515));({struct _tuple35*_tmp978=({struct _tuple35*_tmp514=_cycalloc(sizeof(*_tmp514));({struct Cyc_List_List*_tmp977=Cyc_yyget_YY42(&(yyyvsp[2]).v);_tmp514->f1=_tmp977;}),({struct Cyc_Absyn_Exp*_tmp976=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp514->f2=_tmp976;});_tmp514;});_tmp515->hd=_tmp978;}),({struct Cyc_List_List*_tmp975=Cyc_yyget_YY5(&(yyyvsp[0]).v);_tmp515->tl=_tmp975;});_tmp515;}));
goto _LL0;case 264: _LL20B: _LL20C:
# 2037 "parse.y"
 yyval=Cyc_YY42(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY42(&(yyyvsp[0]).v)));
goto _LL0;case 265: _LL20D: _LL20E:
# 2038 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp518=_cycalloc(sizeof(*_tmp518));({void*_tmp97B=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp517=_cycalloc(sizeof(*_tmp517));_tmp517->tag=1,({struct _fat_ptr*_tmp97A=({struct _fat_ptr*_tmp516=_cycalloc(sizeof(*_tmp516));({struct _fat_ptr _tmp979=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp516=_tmp979;});_tmp516;});_tmp517->f1=_tmp97A;});_tmp517;});_tmp518->hd=_tmp97B;}),_tmp518->tl=0;_tmp518;}));
goto _LL0;case 266: _LL20F: _LL210:
# 2043 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp519=_cycalloc(sizeof(*_tmp519));({void*_tmp97C=Cyc_yyget_YY43(&(yyyvsp[0]).v);_tmp519->hd=_tmp97C;}),_tmp519->tl=0;_tmp519;}));
goto _LL0;case 267: _LL211: _LL212:
# 2044 "parse.y"
 yyval=Cyc_YY42(({struct Cyc_List_List*_tmp51A=_cycalloc(sizeof(*_tmp51A));({void*_tmp97E=Cyc_yyget_YY43(&(yyyvsp[1]).v);_tmp51A->hd=_tmp97E;}),({struct Cyc_List_List*_tmp97D=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp51A->tl=_tmp97D;});_tmp51A;}));
goto _LL0;case 268: _LL213: _LL214:
# 2048 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->tag=0,({struct Cyc_Absyn_Exp*_tmp97F=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp51B->f1=_tmp97F;});_tmp51B;}));
goto _LL0;case 269: _LL215: _LL216:
# 2049 "parse.y"
 yyval=Cyc_YY43((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->tag=1,({struct _fat_ptr*_tmp981=({struct _fat_ptr*_tmp51C=_cycalloc(sizeof(*_tmp51C));({struct _fat_ptr _tmp980=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp51C=_tmp980;});_tmp51C;});_tmp51D->f1=_tmp981;});_tmp51D;}));
goto _LL0;case 270: _LL217: _LL218: {
# 2054 "parse.y"
struct _tuple27 _tmp51E=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp22=_tmp51E;struct _tuple27 _tmp51F=_stmttmp22;struct Cyc_List_List*_tmp522;struct Cyc_Parse_Type_specifier _tmp521;struct Cyc_Absyn_Tqual _tmp520;_LL473: _tmp520=_tmp51F.f1;_tmp521=_tmp51F.f2;_tmp522=_tmp51F.f3;_LL474: {struct Cyc_Absyn_Tqual tq=_tmp520;struct Cyc_Parse_Type_specifier tss=_tmp521;struct Cyc_List_List*atts=_tmp522;
void*_tmp523=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp523;
if(atts != 0)
({void*_tmp524=0U;({unsigned _tmp983=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmp982=({const char*_tmp525="ignoring attributes in type";_tag_fat(_tmp525,sizeof(char),28U);});Cyc_Warn_warn(_tmp983,_tmp982,_tag_fat(_tmp524,sizeof(void*),0));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->f1=0,_tmp526->f2=tq,_tmp526->f3=t;_tmp526;}));
# 2060
goto _LL0;}}case 271: _LL219: _LL21A: {
# 2061 "parse.y"
struct _tuple27 _tmp527=Cyc_yyget_YY36(&(yyyvsp[0]).v);struct _tuple27 _stmttmp23=_tmp527;struct _tuple27 _tmp528=_stmttmp23;struct Cyc_List_List*_tmp52B;struct Cyc_Parse_Type_specifier _tmp52A;struct Cyc_Absyn_Tqual _tmp529;_LL476: _tmp529=_tmp528.f1;_tmp52A=_tmp528.f2;_tmp52B=_tmp528.f3;_LL477: {struct Cyc_Absyn_Tqual tq=_tmp529;struct Cyc_Parse_Type_specifier tss=_tmp52A;struct Cyc_List_List*atts=_tmp52B;
void*_tmp52C=Cyc_Parse_speclist2typ(tss,(unsigned)((yyyvsp[0]).l).first_line);void*t=_tmp52C;
struct Cyc_List_List*_tmp52D=(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms;struct Cyc_List_List*tms=_tmp52D;
struct _tuple14 _tmp52E=Cyc_Parse_apply_tms(tq,t,atts,tms);struct _tuple14 t_info=_tmp52E;
if(t_info.f3 != 0)
({void*_tmp52F=0U;({unsigned _tmp985=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp984=({const char*_tmp530="bad type params, ignoring";_tag_fat(_tmp530,sizeof(char),26U);});Cyc_Warn_warn(_tmp985,_tmp984,_tag_fat(_tmp52F,sizeof(void*),0));});});
if(t_info.f4 != 0)
({void*_tmp531=0U;({unsigned _tmp987=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmp986=({const char*_tmp532="bad specifiers, ignoring";_tag_fat(_tmp532,sizeof(char),25U);});Cyc_Warn_warn(_tmp987,_tmp986,_tag_fat(_tmp531,sizeof(void*),0));});});
yyval=Cyc_YY38(({struct _tuple8*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->f1=0,_tmp533->f2=t_info.f1,_tmp533->f3=t_info.f2;_tmp533;}));
# 2071
goto _LL0;}}case 272: _LL21B: _LL21C:
# 2074 "parse.y"
 yyval=Cyc_YY45(({struct _tuple8*_tmp988=Cyc_yyget_YY38(&(yyyvsp[0]).v);Cyc_Parse_type_name_to_type(_tmp988,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 273: _LL21D: _LL21E:
# 2075 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(0));
goto _LL0;case 274: _LL21F: _LL220:
# 2076 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(Cyc_yyget_YY41(&(yyyvsp[1]).v)));
goto _LL0;case 275: _LL221: _LL222:
# 2077 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY45(&(yyyvsp[2]).v)));
goto _LL0;case 276: _LL223: _LL224:
# 2078 "parse.y"
 yyval=Cyc_YY45(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp534=_cycalloc(sizeof(*_tmp534));({void*_tmp98A=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp534->hd=_tmp98A;}),({struct Cyc_List_List*_tmp989=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp534->tl=_tmp989;});_tmp534;})));
goto _LL0;case 277: _LL225: _LL226:
# 2084 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp535=_cycalloc(sizeof(*_tmp535));({void*_tmp98B=Cyc_yyget_YY45(&(yyyvsp[0]).v);_tmp535->hd=_tmp98B;}),_tmp535->tl=0;_tmp535;}));
goto _LL0;case 278: _LL227: _LL228:
# 2085 "parse.y"
 yyval=Cyc_YY41(({struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));({void*_tmp98D=Cyc_yyget_YY45(&(yyyvsp[2]).v);_tmp536->hd=_tmp98D;}),({struct Cyc_List_List*_tmp98C=Cyc_yyget_YY41(&(yyyvsp[0]).v);_tmp536->tl=_tmp98C;});_tmp536;}));
goto _LL0;case 279: _LL229: _LL22A:
# 2089 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6DF;({struct Cyc_List_List*_tmp98E=Cyc_yyget_YY27(&(yyyvsp[0]).v);_tmp6DF.tms=_tmp98E;});_tmp6DF;}));
goto _LL0;case 280: _LL22B: _LL22C:
# 2090 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 281: _LL22D: _LL22E:
# 2092 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E0;({struct Cyc_List_List*_tmp990=({struct Cyc_List_List*_tmp98F=Cyc_yyget_YY27(&(yyyvsp[0]).v);((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_imp_append)(_tmp98F,(Cyc_yyget_YY31(&(yyyvsp[1]).v)).tms);});_tmp6E0.tms=_tmp990;});_tmp6E0;}));
goto _LL0;case 282: _LL22F: _LL230:
# 2097 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 283: _LL231: _LL232:
# 2099 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E1;({struct Cyc_List_List*_tmp993=({struct Cyc_List_List*_tmp538=_region_malloc(yyr,sizeof(*_tmp538));({void*_tmp992=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp537=_region_malloc(yyr,sizeof(*_tmp537));_tmp537->tag=0,({void*_tmp991=Cyc_yyget_YY52(&(yyyvsp[2]).v);_tmp537->f1=_tmp991;}),_tmp537->f2=(unsigned)((yyyvsp[2]).l).first_line;_tmp537;});_tmp538->hd=_tmp992;}),_tmp538->tl=0;_tmp538;});_tmp6E1.tms=_tmp993;});_tmp6E1;}));
goto _LL0;case 284: _LL233: _LL234:
# 2101 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E2;({struct Cyc_List_List*_tmp997=({struct Cyc_List_List*_tmp53A=_region_malloc(yyr,sizeof(*_tmp53A));({void*_tmp996=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp539=_region_malloc(yyr,sizeof(*_tmp539));_tmp539->tag=0,({void*_tmp995=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp539->f1=_tmp995;}),_tmp539->f2=(unsigned)((yyyvsp[3]).l).first_line;_tmp539;});_tmp53A->hd=_tmp996;}),({struct Cyc_List_List*_tmp994=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp53A->tl=_tmp994;});_tmp53A;});_tmp6E2.tms=_tmp997;});_tmp6E2;}));
goto _LL0;case 285: _LL235: _LL236:
# 2103 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E3;({struct Cyc_List_List*_tmp99B=({struct Cyc_List_List*_tmp53C=_region_malloc(yyr,sizeof(*_tmp53C));({void*_tmp99A=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp53B=_region_malloc(yyr,sizeof(*_tmp53B));_tmp53B->tag=1,({struct Cyc_Absyn_Exp*_tmp999=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);_tmp53B->f1=_tmp999;}),({void*_tmp998=Cyc_yyget_YY52(&(yyyvsp[3]).v);_tmp53B->f2=_tmp998;}),_tmp53B->f3=(unsigned)((yyyvsp[3]).l).first_line;_tmp53B;});_tmp53C->hd=_tmp99A;}),_tmp53C->tl=0;_tmp53C;});_tmp6E3.tms=_tmp99B;});_tmp6E3;}));
goto _LL0;case 286: _LL237: _LL238:
# 2105 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E4;({struct Cyc_List_List*_tmp9A0=({struct Cyc_List_List*_tmp53E=_region_malloc(yyr,sizeof(*_tmp53E));({void*_tmp99F=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp53D=_region_malloc(yyr,sizeof(*_tmp53D));_tmp53D->tag=1,({struct Cyc_Absyn_Exp*_tmp99E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp53D->f1=_tmp99E;}),({void*_tmp99D=Cyc_yyget_YY52(&(yyyvsp[4]).v);_tmp53D->f2=_tmp99D;}),_tmp53D->f3=(unsigned)((yyyvsp[4]).l).first_line;_tmp53D;});_tmp53E->hd=_tmp99F;}),({
struct Cyc_List_List*_tmp99C=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp53E->tl=_tmp99C;});_tmp53E;});
# 2105
_tmp6E4.tms=_tmp9A0;});_tmp6E4;}));
# 2108
goto _LL0;case 287: _LL239: _LL23A: {
# 2109 "parse.y"
struct _tuple28*_tmp53F=Cyc_yyget_YY40(&(yyyvsp[1]).v);struct _tuple28*_stmttmp24=_tmp53F;struct _tuple28*_tmp540=_stmttmp24;struct Cyc_List_List*_tmp545;void*_tmp544;struct Cyc_Absyn_VarargInfo*_tmp543;int _tmp542;struct Cyc_List_List*_tmp541;_LL479: _tmp541=_tmp540->f1;_tmp542=_tmp540->f2;_tmp543=_tmp540->f3;_tmp544=_tmp540->f4;_tmp545=_tmp540->f5;_LL47A: {struct Cyc_List_List*lis=_tmp541;int b=_tmp542;struct Cyc_Absyn_VarargInfo*c=_tmp543;void*eff=_tmp544;struct Cyc_List_List*po=_tmp545;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E5;({struct Cyc_List_List*_tmp9A5=({struct Cyc_List_List*_tmp548=_region_malloc(yyr,sizeof(*_tmp548));({void*_tmp9A4=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp547=_region_malloc(yyr,sizeof(*_tmp547));_tmp547->tag=3,({void*_tmp9A3=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp546=_region_malloc(yyr,sizeof(*_tmp546));_tmp546->tag=1,_tmp546->f1=lis,_tmp546->f2=b,_tmp546->f3=c,_tmp546->f4=eff,_tmp546->f5=po,({struct Cyc_Absyn_Exp*_tmp9A2=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f1;_tmp546->f6=_tmp9A2;}),({struct Cyc_Absyn_Exp*_tmp9A1=(Cyc_yyget_YY59(&(yyyvsp[3]).v)).f2;_tmp546->f7=_tmp9A1;});_tmp546;});_tmp547->f1=_tmp9A3;});_tmp547;});_tmp548->hd=_tmp9A4;}),_tmp548->tl=0;_tmp548;});_tmp6E5.tms=_tmp9A5;});_tmp6E5;}));
# 2112
goto _LL0;}}case 288: _LL23B: _LL23C: {
# 2113 "parse.y"
struct _tuple28*_tmp549=Cyc_yyget_YY40(&(yyyvsp[2]).v);struct _tuple28*_stmttmp25=_tmp549;struct _tuple28*_tmp54A=_stmttmp25;struct Cyc_List_List*_tmp54F;void*_tmp54E;struct Cyc_Absyn_VarargInfo*_tmp54D;int _tmp54C;struct Cyc_List_List*_tmp54B;_LL47C: _tmp54B=_tmp54A->f1;_tmp54C=_tmp54A->f2;_tmp54D=_tmp54A->f3;_tmp54E=_tmp54A->f4;_tmp54F=_tmp54A->f5;_LL47D: {struct Cyc_List_List*lis=_tmp54B;int b=_tmp54C;struct Cyc_Absyn_VarargInfo*c=_tmp54D;void*eff=_tmp54E;struct Cyc_List_List*po=_tmp54F;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E6;({struct Cyc_List_List*_tmp9AB=({struct Cyc_List_List*_tmp552=_region_malloc(yyr,sizeof(*_tmp552));({void*_tmp9AA=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp551=_region_malloc(yyr,sizeof(*_tmp551));_tmp551->tag=3,({void*_tmp9A9=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp550=_region_malloc(yyr,sizeof(*_tmp550));_tmp550->tag=1,_tmp550->f1=lis,_tmp550->f2=b,_tmp550->f3=c,_tmp550->f4=eff,_tmp550->f5=po,({
struct Cyc_Absyn_Exp*_tmp9A8=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f1;_tmp550->f6=_tmp9A8;}),({struct Cyc_Absyn_Exp*_tmp9A7=(Cyc_yyget_YY59(&(yyyvsp[4]).v)).f2;_tmp550->f7=_tmp9A7;});_tmp550;});
# 2114
_tmp551->f1=_tmp9A9;});_tmp551;});_tmp552->hd=_tmp9AA;}),({
struct Cyc_List_List*_tmp9A6=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp552->tl=_tmp9A6;});_tmp552;});
# 2114
_tmp6E6.tms=_tmp9AB;});_tmp6E6;}));
# 2117
goto _LL0;}}case 289: _LL23D: _LL23E: {
# 2119
struct Cyc_List_List*_tmp553=({unsigned _tmp9AC=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9AC,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[2]).v)));});struct Cyc_List_List*ts=_tmp553;
yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E7;({struct Cyc_List_List*_tmp9AF=({struct Cyc_List_List*_tmp555=_region_malloc(yyr,sizeof(*_tmp555));({void*_tmp9AE=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp554=_region_malloc(yyr,sizeof(*_tmp554));_tmp554->tag=4,_tmp554->f1=ts,_tmp554->f2=(unsigned)((yyyvsp[1]).l).first_line,_tmp554->f3=0;_tmp554;});_tmp555->hd=_tmp9AE;}),({
struct Cyc_List_List*_tmp9AD=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp555->tl=_tmp9AD;});_tmp555;});
# 2120
_tmp6E7.tms=_tmp9AF;});_tmp6E7;}));
# 2123
goto _LL0;}case 290: _LL23F: _LL240:
# 2124 "parse.y"
 yyval=Cyc_YY31(({struct Cyc_Parse_Abstractdeclarator _tmp6E8;({struct Cyc_List_List*_tmp9B3=({struct Cyc_List_List*_tmp557=_region_malloc(yyr,sizeof(*_tmp557));({void*_tmp9B2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp556=_region_malloc(yyr,sizeof(*_tmp556));_tmp556->tag=5,_tmp556->f1=(unsigned)((yyyvsp[1]).l).first_line,({struct Cyc_List_List*_tmp9B1=Cyc_yyget_YY46(&(yyyvsp[1]).v);_tmp556->f2=_tmp9B1;});_tmp556;});_tmp557->hd=_tmp9B2;}),({struct Cyc_List_List*_tmp9B0=(Cyc_yyget_YY31(&(yyyvsp[0]).v)).tms;_tmp557->tl=_tmp9B0;});_tmp557;});_tmp6E8.tms=_tmp9B3;});_tmp6E8;}));
# 2126
goto _LL0;case 291: _LL241: _LL242:
# 2129 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6E9;_tmp6E9.f1=0,_tmp6E9.f2=0;_tmp6E9;}));
goto _LL0;case 292: _LL243: _LL244:
# 2130 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6EA;({struct Cyc_Absyn_Exp*_tmp9B4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6EA.f1=_tmp9B4;}),_tmp6EA.f2=0;_tmp6EA;}));
goto _LL0;case 293: _LL245: _LL246:
# 2131 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6EB;_tmp6EB.f1=0,({struct Cyc_Absyn_Exp*_tmp9B5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6EB.f2=_tmp9B5;});_tmp6EB;}));
goto _LL0;case 294: _LL247: _LL248:
# 2133 "parse.y"
 yyval=Cyc_YY59(({struct _tuple17 _tmp6EC;({struct Cyc_Absyn_Exp*_tmp9B7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp6EC.f1=_tmp9B7;}),({struct Cyc_Absyn_Exp*_tmp9B6=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);_tmp6EC.f2=_tmp9B6;});_tmp6EC;}));
goto _LL0;case 295: _LL249: _LL24A:
# 2137 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 296: _LL24B: _LL24C:
# 2138 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 297: _LL24D: _LL24E:
# 2139 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 298: _LL24F: _LL250:
# 2140 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 299: _LL251: _LL252:
# 2141 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 300: _LL253: _LL254:
# 2142 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 301: _LL255: _LL256:
# 2148 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9BB=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->tag=13,({struct _fat_ptr*_tmp9BA=({struct _fat_ptr*_tmp558=_cycalloc(sizeof(*_tmp558));({struct _fat_ptr _tmp9B9=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp558=_tmp9B9;});_tmp558;});_tmp559->f1=_tmp9BA;}),({struct Cyc_Absyn_Stmt*_tmp9B8=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp559->f2=_tmp9B8;});_tmp559;});Cyc_Absyn_new_stmt(_tmp9BB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 302: _LL257: _LL258:
# 2152 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 303: _LL259: _LL25A:
# 2153 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9BC=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_exp_stmt(_tmp9BC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 304: _LL25B: _LL25C:
# 2158 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 305: _LL25D: _LL25E:
# 2159 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 306: _LL25F: _LL260:
# 2164 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp9BD=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmp9BD,Cyc_Absyn_skip_stmt((unsigned)((yyyvsp[0]).l).first_line));}));
goto _LL0;case 307: _LL261: _LL262:
# 2165 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmp9BE=Cyc_yyget_YY16(&(yyyvsp[0]).v);Cyc_Parse_flatten_declarations(_tmp9BE,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 308: _LL263: _LL264:
# 2167 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9C3=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->tag=13,({struct _fat_ptr*_tmp9C2=({struct _fat_ptr*_tmp55A=_cycalloc(sizeof(*_tmp55A));({struct _fat_ptr _tmp9C1=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp55A=_tmp9C1;});_tmp55A;});_tmp55B->f1=_tmp9C2;}),({struct Cyc_Absyn_Stmt*_tmp9C0=({struct Cyc_List_List*_tmp9BF=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmp9BF,Cyc_Absyn_skip_stmt(0U));});_tmp55B->f2=_tmp9C0;});_tmp55B;});Cyc_Absyn_new_stmt(_tmp9C3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 309: _LL265: _LL266:
# 2169 "parse.y"
 yyval=Cyc_Stmt_tok(({void*_tmp9C8=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=13,({struct _fat_ptr*_tmp9C7=({struct _fat_ptr*_tmp55C=_cycalloc(sizeof(*_tmp55C));({struct _fat_ptr _tmp9C6=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp55C=_tmp9C6;});_tmp55C;});_tmp55D->f1=_tmp9C7;}),({struct Cyc_Absyn_Stmt*_tmp9C5=({struct Cyc_List_List*_tmp9C4=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmp9C4,Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v));});_tmp55D->f2=_tmp9C5;});_tmp55D;});Cyc_Absyn_new_stmt(_tmp9C8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 310: _LL267: _LL268:
# 2170 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 311: _LL269: _LL26A:
# 2171 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9CA=Cyc_yyget_Stmt_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Stmt*_tmp9C9=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);Cyc_Absyn_seq_stmt(_tmp9CA,_tmp9C9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 312: _LL26B: _LL26C:
# 2172 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmp9CD=({void*_tmp9CC=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->tag=1,({struct Cyc_Absyn_Fndecl*_tmp9CB=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp55E->f1=_tmp9CB;});_tmp55E;});Cyc_Absyn_new_decl(_tmp9CC,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmp9CD,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;case 313: _LL26D: _LL26E:
# 2175 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmp9D0=({void*_tmp9CF=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->tag=1,({struct Cyc_Absyn_Fndecl*_tmp9CE=Cyc_yyget_YY15(&(yyyvsp[0]).v);_tmp55F->f1=_tmp9CE;});_tmp55F;});Cyc_Absyn_new_decl(_tmp9CF,(unsigned)((yyyvsp[0]).l).first_line);});Cyc_Parse_flatten_decl(_tmp9D0,Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v));}));
goto _LL0;case 314: _LL26F: _LL270:
# 2180 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9D3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9D2=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmp9D1=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmp9D3,_tmp9D2,_tmp9D1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 315: _LL271: _LL272:
# 2182 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9D6=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9D5=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Stmt*_tmp9D4=Cyc_yyget_Stmt_tok(&(yyyvsp[6]).v);Cyc_Absyn_ifthenelse_stmt(_tmp9D6,_tmp9D5,_tmp9D4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 316: _LL273: _LL274:
# 2188 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9D8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_List_List*_tmp9D7=Cyc_yyget_YY8(&(yyyvsp[5]).v);Cyc_Absyn_switch_stmt(_tmp9D8,_tmp9D7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 317: _LL275: _LL276:
# 2191
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9DB=({struct _tuple0*_tmp9D9=Cyc_yyget_QualId_tok(&(yyyvsp[1]).v);Cyc_Absyn_unknownid_exp(_tmp9D9,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_tmp9DA=Cyc_yyget_YY8(&(yyyvsp[3]).v);Cyc_Absyn_switch_stmt(_tmp9DB,_tmp9DA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 318: _LL277: _LL278:
# 2194
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9DE=({struct Cyc_List_List*_tmp9DC=Cyc_yyget_YY4(&(yyyvsp[3]).v);Cyc_Absyn_tuple_exp(_tmp9DC,(unsigned)((yyyvsp[1]).l).first_line);});struct Cyc_List_List*_tmp9DD=Cyc_yyget_YY8(&(yyyvsp[6]).v);Cyc_Absyn_switch_stmt(_tmp9DE,_tmp9DD,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 319: _LL279: _LL27A:
# 2198 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9E0=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_List_List*_tmp9DF=Cyc_yyget_YY8(&(yyyvsp[4]).v);Cyc_Absyn_trycatch_stmt(_tmp9E0,_tmp9DF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 320: _LL27B: _LL27C:
# 2212 "parse.y"
 yyval=Cyc_YY8(0);
goto _LL0;case 321: _LL27D: _LL27E:
# 2215 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp561=_cycalloc(sizeof(*_tmp561));({struct Cyc_Absyn_Switch_clause*_tmp9E4=({struct Cyc_Absyn_Switch_clause*_tmp560=_cycalloc(sizeof(*_tmp560));({struct Cyc_Absyn_Pat*_tmp9E3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line);_tmp560->pattern=_tmp9E3;}),_tmp560->pat_vars=0,_tmp560->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmp9E2=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);_tmp560->body=_tmp9E2;}),_tmp560->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp560;});
# 2215
_tmp561->hd=_tmp9E4;}),({
struct Cyc_List_List*_tmp9E1=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp561->tl=_tmp9E1;});_tmp561;}));
goto _LL0;case 322: _LL27F: _LL280:
# 2218 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp563=_cycalloc(sizeof(*_tmp563));({struct Cyc_Absyn_Switch_clause*_tmp9E8=({struct Cyc_Absyn_Switch_clause*_tmp562=_cycalloc(sizeof(*_tmp562));({struct Cyc_Absyn_Pat*_tmp9E7=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp562->pattern=_tmp9E7;}),_tmp562->pat_vars=0,_tmp562->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmp9E6=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[2]).l).first_line);_tmp562->body=_tmp9E6;}),_tmp562->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp562;});
# 2218
_tmp563->hd=_tmp9E8;}),({
# 2220
struct Cyc_List_List*_tmp9E5=Cyc_yyget_YY8(&(yyyvsp[3]).v);_tmp563->tl=_tmp9E5;});_tmp563;}));
goto _LL0;case 323: _LL281: _LL282:
# 2222 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp565=_cycalloc(sizeof(*_tmp565));({struct Cyc_Absyn_Switch_clause*_tmp9EC=({struct Cyc_Absyn_Switch_clause*_tmp564=_cycalloc(sizeof(*_tmp564));({struct Cyc_Absyn_Pat*_tmp9EB=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp564->pattern=_tmp9EB;}),_tmp564->pat_vars=0,_tmp564->where_clause=0,({struct Cyc_Absyn_Stmt*_tmp9EA=Cyc_yyget_Stmt_tok(&(yyyvsp[3]).v);_tmp564->body=_tmp9EA;}),_tmp564->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp564;});_tmp565->hd=_tmp9EC;}),({struct Cyc_List_List*_tmp9E9=Cyc_yyget_YY8(&(yyyvsp[4]).v);_tmp565->tl=_tmp9E9;});_tmp565;}));
goto _LL0;case 324: _LL283: _LL284:
# 2224 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp567=_cycalloc(sizeof(*_tmp567));({struct Cyc_Absyn_Switch_clause*_tmp9F1=({struct Cyc_Absyn_Switch_clause*_tmp566=_cycalloc(sizeof(*_tmp566));({struct Cyc_Absyn_Pat*_tmp9F0=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp566->pattern=_tmp9F0;}),_tmp566->pat_vars=0,({struct Cyc_Absyn_Exp*_tmp9EF=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp566->where_clause=_tmp9EF;}),({
struct Cyc_Absyn_Stmt*_tmp9EE=Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[4]).l).first_line);_tmp566->body=_tmp9EE;}),_tmp566->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp566;});
# 2224
_tmp567->hd=_tmp9F1;}),({
# 2226
struct Cyc_List_List*_tmp9ED=Cyc_yyget_YY8(&(yyyvsp[5]).v);_tmp567->tl=_tmp9ED;});_tmp567;}));
goto _LL0;case 325: _LL285: _LL286:
# 2228 "parse.y"
 yyval=Cyc_YY8(({struct Cyc_List_List*_tmp569=_cycalloc(sizeof(*_tmp569));({struct Cyc_Absyn_Switch_clause*_tmp9F6=({struct Cyc_Absyn_Switch_clause*_tmp568=_cycalloc(sizeof(*_tmp568));({struct Cyc_Absyn_Pat*_tmp9F5=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp568->pattern=_tmp9F5;}),_tmp568->pat_vars=0,({struct Cyc_Absyn_Exp*_tmp9F4=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);_tmp568->where_clause=_tmp9F4;}),({struct Cyc_Absyn_Stmt*_tmp9F3=Cyc_yyget_Stmt_tok(&(yyyvsp[5]).v);_tmp568->body=_tmp9F3;}),_tmp568->loc=(unsigned)((yyyvsp[0]).l).first_line;_tmp568;});_tmp569->hd=_tmp9F6;}),({struct Cyc_List_List*_tmp9F2=Cyc_yyget_YY8(&(yyyvsp[6]).v);_tmp569->tl=_tmp9F2;});_tmp569;}));
goto _LL0;case 326: _LL287: _LL288:
# 2235 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9F8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Stmt*_tmp9F7=Cyc_yyget_Stmt_tok(&(yyyvsp[4]).v);Cyc_Absyn_while_stmt(_tmp9F8,_tmp9F7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 327: _LL289: _LL28A:
# 2239 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmp9FA=Cyc_yyget_Stmt_tok(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmp9F9=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_do_stmt(_tmp9FA,_tmp9F9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 328: _LL28B: _LL28C:
# 2243 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmp9FE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmp9FD=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);struct Cyc_Absyn_Exp*_tmp9FC=Cyc_yyget_Exp_tok(&(yyyvsp[6]).v);struct Cyc_Absyn_Stmt*_tmp9FB=Cyc_yyget_Stmt_tok(&(yyyvsp[8]).v);Cyc_Absyn_for_stmt(_tmp9FE,_tmp9FD,_tmp9FC,_tmp9FB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 329: _LL28D: _LL28E: {
# 2245 "parse.y"
struct Cyc_Absyn_Stmt*_tmp56A=({struct Cyc_Absyn_Exp*_tmpA02=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA01=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);struct Cyc_Absyn_Exp*_tmpA00=Cyc_yyget_Exp_tok(&(yyyvsp[5]).v);struct Cyc_Absyn_Stmt*_tmp9FF=Cyc_yyget_Stmt_tok(&(yyyvsp[7]).v);Cyc_Absyn_for_stmt(_tmpA02,_tmpA01,_tmpA00,_tmp9FF,(unsigned)((yyyvsp[0]).l).first_line);});struct Cyc_Absyn_Stmt*s=_tmp56A;
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA03=Cyc_yyget_YY16(&(yyyvsp[2]).v);Cyc_Parse_flatten_declarations(_tmpA03,s);}));
goto _LL0;}case 330: _LL28F: _LL290:
# 2249
 yyval=Cyc_Exp_tok(Cyc_Absyn_true_exp(0U));
goto _LL0;case 331: _LL291: _LL292:
# 2250 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_yyget_Exp_tok(&(yyyvsp[0]).v));
goto _LL0;case 332: _LL293: _LL294:
# 2253
 yyval=Cyc_Stmt_tok(({struct _fat_ptr*_tmpA05=({struct _fat_ptr*_tmp56B=_cycalloc(sizeof(*_tmp56B));({struct _fat_ptr _tmpA04=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp56B=_tmpA04;});_tmp56B;});Cyc_Absyn_goto_stmt(_tmpA05,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 333: _LL295: _LL296:
# 2254 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 334: _LL297: _LL298:
# 2255 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 335: _LL299: _LL29A:
# 2256 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 336: _LL29B: _LL29C:
# 2257 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA06=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_return_stmt(_tmpA06,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 337: _LL29D: _LL29E:
# 2259 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 338: _LL29F: _LL2A0:
# 2260 "parse.y"
 yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 339: _LL2A1: _LL2A2:
# 2262 "parse.y"
 yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA07=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_fallthru_stmt(_tmpA07,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 340: _LL2A3: _LL2A4:
# 2271 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 341: _LL2A5: _LL2A6:
# 2274
 yyval=(yyyvsp[0]).v;
goto _LL0;case 342: _LL2A7: _LL2A8:
# 2276 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA09=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpA08=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpA0A,_tmpA09,_tmpA08,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 343: _LL2A9: _LL2AA:
# 2279
 yyval=(yyyvsp[0]).v;
goto _LL0;case 344: _LL2AB: _LL2AC:
# 2281 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA0C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA0B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpA0C,_tmpA0B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 345: _LL2AD: _LL2AE:
# 2284
 yyval=(yyyvsp[0]).v;
goto _LL0;case 346: _LL2AF: _LL2B0:
# 2286 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA0E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA0D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpA0E,_tmpA0D,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 347: _LL2B1: _LL2B2:
# 2289
 yyval=(yyyvsp[0]).v;
goto _LL0;case 348: _LL2B3: _LL2B4:
# 2291 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA10=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA0F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpA10,_tmpA0F,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 349: _LL2B5: _LL2B6:
# 2294
 yyval=(yyyvsp[0]).v;
goto _LL0;case 350: _LL2B7: _LL2B8:
# 2296 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA12=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA11=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpA12,_tmpA11,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 351: _LL2B9: _LL2BA:
# 2299
 yyval=(yyyvsp[0]).v;
goto _LL0;case 352: _LL2BB: _LL2BC:
# 2301 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA14=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA13=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpA14,_tmpA13,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 353: _LL2BD: _LL2BE:
# 2304
 yyval=(yyyvsp[0]).v;
goto _LL0;case 354: _LL2BF: _LL2C0:
# 2306 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_tmpA17)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA16=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA15=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpA17(_tmpA16,_tmpA15,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 355: _LL2C1: _LL2C2:
# 2309
 yyval=(yyyvsp[0]).v;
goto _LL0;case 356: _LL2C3: _LL2C4:
# 2311 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*(*_tmpA1A)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA19=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA18=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpA1A(_tmpA19,_tmpA18,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 357: _LL2C5: _LL2C6:
# 2314
 yyval=(yyyvsp[0]).v;
goto _LL0;case 358: _LL2C7: _LL2C8:
# 2316 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA1C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA1B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpA1C,_tmpA1B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 359: _LL2C9: _LL2CA:
# 2318 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA1E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA1D=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpA1E,_tmpA1D,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 360: _LL2CB: _LL2CC:
# 2321
 yyval=(yyyvsp[0]).v;
goto _LL0;case 361: _LL2CD: _LL2CE:
# 2323 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA21=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA20=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA1F=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpA21,_tmpA20,_tmpA1F,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 362: _LL2CF: _LL2D0:
# 2326
 yyval=(yyyvsp[0]).v;
goto _LL0;case 363: _LL2D1: _LL2D2:
# 2328 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA24=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA23=Cyc_Parse_pat2exp(Cyc_yyget_YY9(&(yyyvsp[0]).v));struct Cyc_Absyn_Exp*_tmpA22=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpA24,_tmpA23,_tmpA22,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 364: _LL2D3: _LL2D4:
# 2331
 yyval=(yyyvsp[0]).v;
goto _LL0;case 365: _LL2D5: _LL2D6: {
# 2333 "parse.y"
void*_tmp56C=({struct _tuple8*_tmpA25=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpA25,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp56C;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpA27=t;struct Cyc_Absyn_Exp*_tmpA26=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpA27,_tmpA26,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);})));
# 2336
goto _LL0;}case 366: _LL2D7: _LL2D8:
# 2339 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 367: _LL2D9: _LL2DA:
# 2343 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpA29=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA28=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpA29,_tmpA28,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 368: _LL2DB: _LL2DC: {
# 2345 "parse.y"
void*_tmp56D=({struct _tuple8*_tmpA2A=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpA2A,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp56D;
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line)));
goto _LL0;}case 369: _LL2DD: _LL2DE:
# 2348 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpA2B=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpA2B,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 370: _LL2DF: _LL2E0:
# 2350 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpA2D=(*Cyc_yyget_YY38(&(yyyvsp[2]).v)).f3;struct Cyc_List_List*_tmpA2C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpA2D,_tmpA2C,(unsigned)((yyyvsp[0]).l).first_line);})));
goto _LL0;case 371: _LL2E1: _LL2E2:
# 2355 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 372: _LL2E3: _LL2E4:
# 2363 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 373: _LL2E5: _LL2E6:
# 2367 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 374: _LL2E7: _LL2E8:
# 2368 "parse.y"
 yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(&(yyyvsp[1]).v)));
goto _LL0;case 375: _LL2E9: _LL2EA:
# 2369 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA2F=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp56E=_cycalloc(sizeof(*_tmp56E));_tmp56E->tag=15,({struct _tuple0*_tmpA2E=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp56E->f1=_tmpA2E;});_tmp56E;});Cyc_Absyn_new_pat(_tmpA2F,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 376: _LL2EB: _LL2EC:
# 2370 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA31=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=6,({struct Cyc_Absyn_Pat*_tmpA30=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp56F->f1=_tmpA30;});_tmp56F;});Cyc_Absyn_new_pat(_tmpA31,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 377: _LL2ED: _LL2EE: {
# 2372 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);
{void*_tmp570=e->r;void*_stmttmp26=_tmp570;void*_tmp571=_stmttmp26;int _tmp573;struct _fat_ptr _tmp572;int _tmp575;enum Cyc_Absyn_Sign _tmp574;short _tmp577;enum Cyc_Absyn_Sign _tmp576;char _tmp579;enum Cyc_Absyn_Sign _tmp578;if(*((int*)_tmp571)== 0)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).LongLong_c).tag){case 2: _LL47F: _tmp578=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Char_c).val).f1;_tmp579=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Char_c).val).f2;_LL480: {enum Cyc_Absyn_Sign s=_tmp578;char i=_tmp579;
# 2376
yyval=Cyc_YY9(({void*_tmpA32=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp57A=_cycalloc(sizeof(*_tmp57A));_tmp57A->tag=11,_tmp57A->f1=i;_tmp57A;});Cyc_Absyn_new_pat(_tmpA32,e->loc);}));goto _LL47E;}case 4: _LL481: _tmp576=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Short_c).val).f1;_tmp577=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Short_c).val).f2;_LL482: {enum Cyc_Absyn_Sign s=_tmp576;short i=_tmp577;
# 2378
yyval=Cyc_YY9(({void*_tmpA33=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->tag=10,_tmp57B->f1=s,_tmp57B->f2=(int)i;_tmp57B;});Cyc_Absyn_new_pat(_tmpA33,e->loc);}));goto _LL47E;}case 5: _LL483: _tmp574=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Int_c).val).f1;_tmp575=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Int_c).val).f2;_LL484: {enum Cyc_Absyn_Sign s=_tmp574;int i=_tmp575;
# 2380
yyval=Cyc_YY9(({void*_tmpA34=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->tag=10,_tmp57C->f1=s,_tmp57C->f2=i;_tmp57C;});Cyc_Absyn_new_pat(_tmpA34,e->loc);}));goto _LL47E;}case 7: _LL485: _tmp572=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Float_c).val).f1;_tmp573=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp571)->f1).Float_c).val).f2;_LL486: {struct _fat_ptr s=_tmp572;int i=_tmp573;
# 2382
yyval=Cyc_YY9(({void*_tmpA35=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=12,_tmp57D->f1=s,_tmp57D->f2=i;_tmp57D;});Cyc_Absyn_new_pat(_tmpA35,e->loc);}));goto _LL47E;}case 1: _LL487: _LL488:
# 2384
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL47E;case 8: _LL489: _LL48A:
 goto _LL48C;case 9: _LL48B: _LL48C:
# 2387
({void*_tmp57E=0U;({unsigned _tmpA37=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA36=({const char*_tmp57F="strings cannot occur within patterns";_tag_fat(_tmp57F,sizeof(char),37U);});Cyc_Warn_err(_tmpA37,_tmpA36,_tag_fat(_tmp57E,sizeof(void*),0));});});goto _LL47E;case 6: _LL48D: _LL48E:
# 2389
({void*_tmp580=0U;({unsigned _tmpA39=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA38=({const char*_tmp581="long long's in patterns not yet implemented";_tag_fat(_tmp581,sizeof(char),44U);});Cyc_Warn_err(_tmpA39,_tmpA38,_tag_fat(_tmp580,sizeof(void*),0));});});goto _LL47E;default: goto _LL48F;}else{_LL48F: _LL490:
# 2391
({void*_tmp582=0U;({unsigned _tmpA3B=(unsigned)((yyyvsp[0]).l).first_line;struct _fat_ptr _tmpA3A=({const char*_tmp583="bad constant in case";_tag_fat(_tmp583,sizeof(char),21U);});Cyc_Warn_err(_tmpA3B,_tmpA3A,_tag_fat(_tmp582,sizeof(void*),0));});});}_LL47E:;}
# 2394
goto _LL0;}case 378: _LL2EF: _LL2F0:
# 2395 "parse.y"
 if(({struct _fat_ptr _tmpA3C=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[1]).v);Cyc_strcmp(_tmpA3C,({const char*_tmp584="as";_tag_fat(_tmp584,sizeof(char),3U);}));})!= 0)
({void*_tmp585=0U;({unsigned _tmpA3E=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA3D=({const char*_tmp586="expecting `as'";_tag_fat(_tmp586,sizeof(char),15U);});Cyc_Warn_err(_tmpA3E,_tmpA3D,_tag_fat(_tmp585,sizeof(void*),0));});});
yyval=Cyc_YY9(({void*_tmpA45=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->tag=1,({struct Cyc_Absyn_Vardecl*_tmpA44=({unsigned _tmpA43=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA42=({struct _tuple0*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA41=({struct _fat_ptr*_tmp587=_cycalloc(sizeof(*_tmp587));({struct _fat_ptr _tmpA40=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp587=_tmpA40;});_tmp587;});_tmp588->f2=_tmpA41;});_tmp588;});Cyc_Absyn_new_vardecl(_tmpA43,_tmpA42,Cyc_Absyn_void_type,0);});_tmp589->f1=_tmpA44;}),({
struct Cyc_Absyn_Pat*_tmpA3F=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp589->f2=_tmpA3F;});_tmp589;});
# 2397
Cyc_Absyn_new_pat(_tmpA45,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2400
goto _LL0;case 379: _LL2F1: _LL2F2:
# 2401 "parse.y"
 if(({struct _fat_ptr _tmpA46=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpA46,({const char*_tmp58A="alias";_tag_fat(_tmp58A,sizeof(char),6U);}));})!= 0)
({void*_tmp58B=0U;({unsigned _tmpA48=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA47=({const char*_tmp58C="expecting `alias'";_tag_fat(_tmp58C,sizeof(char),18U);});Cyc_Warn_err(_tmpA48,_tmpA47,_tag_fat(_tmp58B,sizeof(void*),0));});});{
int _tmp58D=((yyyvsp[0]).l).first_line;int location=_tmp58D;
({struct _fat_ptr _tmpA49=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpA49,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp593=_cycalloc(sizeof(*_tmp593));({struct _fat_ptr*_tmpA4C=({struct _fat_ptr*_tmp591=_cycalloc(sizeof(*_tmp591));({struct _fat_ptr _tmpA4B=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp591=_tmpA4B;});_tmp591;});_tmp593->name=_tmpA4C;}),_tmp593->identity=- 1,({void*_tmpA4A=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->tag=0,_tmp592->f1=& Cyc_Kinds_rk;_tmp592;});_tmp593->kind=_tmpA4A;});_tmp593;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpA51=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA50=({struct _tuple0*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA4E=({struct _fat_ptr*_tmp58F=_cycalloc(sizeof(*_tmp58F));({struct _fat_ptr _tmpA4D=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp58F=_tmpA4D;});_tmp58F;});_tmp590->f2=_tmpA4E;});_tmp590;});Cyc_Absyn_new_vardecl(_tmpA51,_tmpA50,({
struct _tuple8*_tmpA4F=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpA4F,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpA52=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->tag=2,_tmp58E->f1=tv,_tmp58E->f2=vd;_tmp58E;});Cyc_Absyn_new_pat(_tmpA52,(unsigned)location);}));
# 2410
goto _LL0;}}case 380: _LL2F3: _LL2F4:
# 2411 "parse.y"
 if(({struct _fat_ptr _tmpA53=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_strcmp(_tmpA53,({const char*_tmp594="alias";_tag_fat(_tmp594,sizeof(char),6U);}));})!= 0)
({void*_tmp595=0U;({unsigned _tmpA55=(unsigned)((yyyvsp[1]).l).first_line;struct _fat_ptr _tmpA54=({const char*_tmp596="expecting `alias'";_tag_fat(_tmp596,sizeof(char),18U);});Cyc_Warn_err(_tmpA55,_tmpA54,_tag_fat(_tmp595,sizeof(void*),0));});});{
int _tmp597=((yyyvsp[0]).l).first_line;int location=_tmp597;
({struct _fat_ptr _tmpA56=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_tvar_ok(_tmpA56,(unsigned)location);});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp59D=_cycalloc(sizeof(*_tmp59D));({struct _fat_ptr*_tmpA59=({struct _fat_ptr*_tmp59B=_cycalloc(sizeof(*_tmp59B));({struct _fat_ptr _tmpA58=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp59B=_tmpA58;});_tmp59B;});_tmp59D->name=_tmpA59;}),_tmp59D->identity=- 1,({void*_tmpA57=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->tag=0,_tmp59C->f1=& Cyc_Kinds_rk;_tmp59C;});_tmp59D->kind=_tmpA57;});_tmp59D;});
struct Cyc_Absyn_Vardecl*vd=({unsigned _tmpA5E=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA5D=({struct _tuple0*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA5B=({struct _fat_ptr*_tmp599=_cycalloc(sizeof(*_tmp599));({struct _fat_ptr _tmpA5A=Cyc_yyget_String_tok(&(yyyvsp[5]).v);*_tmp599=_tmpA5A;});_tmp599;});_tmp59A->f2=_tmpA5B;});_tmp59A;});Cyc_Absyn_new_vardecl(_tmpA5E,_tmpA5D,({
struct _tuple8*_tmpA5C=Cyc_yyget_YY38(&(yyyvsp[4]).v);Cyc_Parse_type_name_to_type(_tmpA5C,(unsigned)((yyyvsp[4]).l).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpA5F=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp598=_cycalloc(sizeof(*_tmp598));_tmp598->tag=2,_tmp598->f1=tv,_tmp598->f2=vd;_tmp598;});Cyc_Absyn_new_pat(_tmpA5F,(unsigned)location);}));
# 2420
goto _LL0;}}case 381: _LL2F5: _LL2F6: {
# 2421 "parse.y"
struct _tuple23 _tmp59E=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp27=_tmp59E;struct _tuple23 _tmp59F=_stmttmp27;int _tmp5A1;struct Cyc_List_List*_tmp5A0;_LL492: _tmp5A0=_tmp59F.f1;_tmp5A1=_tmp59F.f2;_LL493: {struct Cyc_List_List*ps=_tmp5A0;int dots=_tmp5A1;
yyval=Cyc_YY9(({void*_tmpA60=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->tag=5,_tmp5A2->f1=ps,_tmp5A2->f2=dots;_tmp5A2;});Cyc_Absyn_new_pat(_tmpA60,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2424
goto _LL0;}}case 382: _LL2F7: _LL2F8: {
# 2425 "parse.y"
struct _tuple23 _tmp5A3=*Cyc_yyget_YY10(&(yyyvsp[2]).v);struct _tuple23 _stmttmp28=_tmp5A3;struct _tuple23 _tmp5A4=_stmttmp28;int _tmp5A6;struct Cyc_List_List*_tmp5A5;_LL495: _tmp5A5=_tmp5A4.f1;_tmp5A6=_tmp5A4.f2;_LL496: {struct Cyc_List_List*ps=_tmp5A5;int dots=_tmp5A6;
yyval=Cyc_YY9(({void*_tmpA62=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp5A7=_cycalloc(sizeof(*_tmp5A7));_tmp5A7->tag=16,({struct _tuple0*_tmpA61=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp5A7->f1=_tmpA61;}),_tmp5A7->f2=ps,_tmp5A7->f3=dots;_tmp5A7;});Cyc_Absyn_new_pat(_tmpA62,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2428
goto _LL0;}}case 383: _LL2F9: _LL2FA: {
# 2429 "parse.y"
struct _tuple23 _tmp5A8=*Cyc_yyget_YY14(&(yyyvsp[3]).v);struct _tuple23 _stmttmp29=_tmp5A8;struct _tuple23 _tmp5A9=_stmttmp29;int _tmp5AB;struct Cyc_List_List*_tmp5AA;_LL498: _tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;_LL499: {struct Cyc_List_List*fps=_tmp5AA;int dots=_tmp5AB;
struct Cyc_List_List*_tmp5AC=({unsigned _tmpA63=(unsigned)((yyyvsp[2]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA63,Cyc_yyget_YY41(&(yyyvsp[2]).v));});struct Cyc_List_List*exist_ts=_tmp5AC;
yyval=Cyc_YY9(({void*_tmpA66=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->tag=7,({union Cyc_Absyn_AggrInfo*_tmpA65=({union Cyc_Absyn_AggrInfo*_tmp5AD=_cycalloc(sizeof(*_tmp5AD));({union Cyc_Absyn_AggrInfo _tmpA64=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(&(yyyvsp[0]).v),0);*_tmp5AD=_tmpA64;});_tmp5AD;});_tmp5AE->f1=_tmpA65;}),_tmp5AE->f2=exist_ts,_tmp5AE->f3=fps,_tmp5AE->f4=dots;_tmp5AE;});Cyc_Absyn_new_pat(_tmpA66,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2434
goto _LL0;}}case 384: _LL2FB: _LL2FC: {
# 2435 "parse.y"
struct _tuple23 _tmp5AF=*Cyc_yyget_YY14(&(yyyvsp[2]).v);struct _tuple23 _stmttmp2A=_tmp5AF;struct _tuple23 _tmp5B0=_stmttmp2A;int _tmp5B2;struct Cyc_List_List*_tmp5B1;_LL49B: _tmp5B1=_tmp5B0.f1;_tmp5B2=_tmp5B0.f2;_LL49C: {struct Cyc_List_List*fps=_tmp5B1;int dots=_tmp5B2;
struct Cyc_List_List*_tmp5B3=({unsigned _tmpA67=(unsigned)((yyyvsp[1]).l).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(unsigned,void*),unsigned,struct Cyc_List_List*))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA67,Cyc_yyget_YY41(&(yyyvsp[1]).v));});struct Cyc_List_List*exist_ts=_tmp5B3;
yyval=Cyc_YY9(({void*_tmpA68=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->tag=7,_tmp5B4->f1=0,_tmp5B4->f2=exist_ts,_tmp5B4->f3=fps,_tmp5B4->f4=dots;_tmp5B4;});Cyc_Absyn_new_pat(_tmpA68,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2439
goto _LL0;}}case 385: _LL2FD: _LL2FE:
# 2440 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA6C=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->tag=6,({struct Cyc_Absyn_Pat*_tmpA6B=({void*_tmpA6A=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->tag=6,({struct Cyc_Absyn_Pat*_tmpA69=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5B5->f1=_tmpA69;});_tmp5B5;});Cyc_Absyn_new_pat(_tmpA6A,(unsigned)((yyyvsp[0]).l).first_line);});_tmp5B6->f1=_tmpA6B;});_tmp5B6;});Cyc_Absyn_new_pat(_tmpA6C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 386: _LL2FF: _LL300:
# 2442 "parse.y"
 yyval=Cyc_YY9(({void*_tmpA73=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->tag=3,({struct Cyc_Absyn_Vardecl*_tmpA72=({unsigned _tmpA71=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA70=({struct _tuple0*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA6F=({struct _fat_ptr*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));({struct _fat_ptr _tmpA6E=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5B7=_tmpA6E;});_tmp5B7;});_tmp5B8->f2=_tmpA6F;});_tmp5B8;});Cyc_Absyn_new_vardecl(_tmpA71,_tmpA70,Cyc_Absyn_void_type,0);});_tmp5B9->f1=_tmpA72;}),({
# 2444
struct Cyc_Absyn_Pat*_tmpA6D=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned)((yyyvsp[1]).l).first_line);_tmp5B9->f2=_tmpA6D;});_tmp5B9;});
# 2442
Cyc_Absyn_new_pat(_tmpA73,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2446
goto _LL0;case 387: _LL301: _LL302:
# 2447 "parse.y"
 if(({struct _fat_ptr _tmpA74=(struct _fat_ptr)Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_strcmp(_tmpA74,({const char*_tmp5BA="as";_tag_fat(_tmp5BA,sizeof(char),3U);}));})!= 0)
({void*_tmp5BB=0U;({unsigned _tmpA76=(unsigned)((yyyvsp[2]).l).first_line;struct _fat_ptr _tmpA75=({const char*_tmp5BC="expecting `as'";_tag_fat(_tmp5BC,sizeof(char),15U);});Cyc_Warn_err(_tmpA76,_tmpA75,_tag_fat(_tmp5BB,sizeof(void*),0));});});
yyval=Cyc_YY9(({void*_tmpA7D=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->tag=3,({struct Cyc_Absyn_Vardecl*_tmpA7C=({unsigned _tmpA7B=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA7A=({struct _tuple0*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA79=({struct _fat_ptr*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));({struct _fat_ptr _tmpA78=Cyc_yyget_String_tok(&(yyyvsp[1]).v);*_tmp5BD=_tmpA78;});_tmp5BD;});_tmp5BE->f2=_tmpA79;});_tmp5BE;});Cyc_Absyn_new_vardecl(_tmpA7B,_tmpA7A,Cyc_Absyn_void_type,0);});_tmp5BF->f1=_tmpA7C;}),({
# 2451
struct Cyc_Absyn_Pat*_tmpA77=Cyc_yyget_YY9(&(yyyvsp[3]).v);_tmp5BF->f2=_tmpA77;});_tmp5BF;});
# 2449
Cyc_Absyn_new_pat(_tmpA7D,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2453
goto _LL0;case 388: _LL303: _LL304: {
# 2454 "parse.y"
void*_tmp5C0=({struct _fat_ptr _tmpA7E=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Parse_id2type(_tmpA7E,Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));});void*tag=_tmp5C0;
yyval=Cyc_YY9(({void*_tmpA85=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->tag=4,({struct Cyc_Absyn_Tvar*_tmpA84=Cyc_Parse_typ2tvar((unsigned)((yyyvsp[2]).l).first_line,tag);_tmp5C3->f1=_tmpA84;}),({
struct Cyc_Absyn_Vardecl*_tmpA83=({unsigned _tmpA82=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA81=({struct _tuple0*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA80=({struct _fat_ptr*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));({struct _fat_ptr _tmpA7F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5C1=_tmpA7F;});_tmp5C1;});_tmp5C2->f2=_tmpA80;});_tmp5C2;});Cyc_Absyn_new_vardecl(_tmpA82,_tmpA81,
Cyc_Absyn_tag_type(tag),0);});
# 2456
_tmp5C3->f2=_tmpA83;});_tmp5C3;});
# 2455
Cyc_Absyn_new_pat(_tmpA85,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2459
goto _LL0;}case 389: _LL305: _LL306: {
# 2460 "parse.y"
struct Cyc_Absyn_Tvar*_tmp5C4=Cyc_Tcutil_new_tvar(Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ik));struct Cyc_Absyn_Tvar*tv=_tmp5C4;
yyval=Cyc_YY9(({void*_tmpA8B=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->tag=4,_tmp5C7->f1=tv,({
struct Cyc_Absyn_Vardecl*_tmpA8A=({unsigned _tmpA89=(unsigned)((yyyvsp[0]).l).first_line;struct _tuple0*_tmpA88=({struct _tuple0*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmpA87=({struct _fat_ptr*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));({struct _fat_ptr _tmpA86=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp5C5=_tmpA86;});_tmp5C5;});_tmp5C6->f2=_tmpA87;});_tmp5C6;});Cyc_Absyn_new_vardecl(_tmpA89,_tmpA88,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(tv)),0);});
# 2462
_tmp5C7->f2=_tmpA8A;});_tmp5C7;});
# 2461
Cyc_Absyn_new_pat(_tmpA8B,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2465
goto _LL0;}case 390: _LL307: _LL308:
# 2468 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));({struct Cyc_List_List*_tmpA8C=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp5C8->f1=_tmpA8C;}),_tmp5C8->f2=0;_tmp5C8;}));
goto _LL0;case 391: _LL309: _LL30A:
# 2469 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));({struct Cyc_List_List*_tmpA8D=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY11(&(yyyvsp[0]).v));_tmp5C9->f1=_tmpA8D;}),_tmp5C9->f2=1;_tmp5C9;}));
goto _LL0;case 392: _LL30B: _LL30C:
# 2470 "parse.y"
 yyval=Cyc_YY10(({struct _tuple23*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->f1=0,_tmp5CA->f2=1;_tmp5CA;}));
goto _LL0;case 393: _LL30D: _LL30E:
# 2473
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));({struct Cyc_Absyn_Pat*_tmpA8E=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp5CB->hd=_tmpA8E;}),_tmp5CB->tl=0;_tmp5CB;}));
goto _LL0;case 394: _LL30F: _LL310:
# 2474 "parse.y"
 yyval=Cyc_YY11(({struct Cyc_List_List*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));({struct Cyc_Absyn_Pat*_tmpA90=Cyc_yyget_YY9(&(yyyvsp[2]).v);_tmp5CC->hd=_tmpA90;}),({struct Cyc_List_List*_tmpA8F=Cyc_yyget_YY11(&(yyyvsp[0]).v);_tmp5CC->tl=_tmpA8F;});_tmp5CC;}));
goto _LL0;case 395: _LL311: _LL312:
# 2477
 yyval=Cyc_YY12(({struct _tuple24*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->f1=0,({struct Cyc_Absyn_Pat*_tmpA91=Cyc_yyget_YY9(&(yyyvsp[0]).v);_tmp5CD->f2=_tmpA91;});_tmp5CD;}));
goto _LL0;case 396: _LL313: _LL314:
# 2478 "parse.y"
 yyval=Cyc_YY12(({struct _tuple24*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));({struct Cyc_List_List*_tmpA93=Cyc_yyget_YY42(&(yyyvsp[0]).v);_tmp5CE->f1=_tmpA93;}),({struct Cyc_Absyn_Pat*_tmpA92=Cyc_yyget_YY9(&(yyyvsp[1]).v);_tmp5CE->f2=_tmpA92;});_tmp5CE;}));
goto _LL0;case 397: _LL315: _LL316:
# 2481
 yyval=Cyc_YY14(({struct _tuple23*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));({struct Cyc_List_List*_tmpA94=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp5CF->f1=_tmpA94;}),_tmp5CF->f2=0;_tmp5CF;}));
goto _LL0;case 398: _LL317: _LL318:
# 2482 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));({struct Cyc_List_List*_tmpA95=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_yyget_YY13(&(yyyvsp[0]).v));_tmp5D0->f1=_tmpA95;}),_tmp5D0->f2=1;_tmp5D0;}));
goto _LL0;case 399: _LL319: _LL31A:
# 2483 "parse.y"
 yyval=Cyc_YY14(({struct _tuple23*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->f1=0,_tmp5D1->f2=1;_tmp5D1;}));
goto _LL0;case 400: _LL31B: _LL31C:
# 2486
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));({struct _tuple24*_tmpA96=Cyc_yyget_YY12(&(yyyvsp[0]).v);_tmp5D2->hd=_tmpA96;}),_tmp5D2->tl=0;_tmp5D2;}));
goto _LL0;case 401: _LL31D: _LL31E:
# 2487 "parse.y"
 yyval=Cyc_YY13(({struct Cyc_List_List*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));({struct _tuple24*_tmpA98=Cyc_yyget_YY12(&(yyyvsp[2]).v);_tmp5D3->hd=_tmpA98;}),({struct Cyc_List_List*_tmpA97=Cyc_yyget_YY13(&(yyyvsp[0]).v);_tmp5D3->tl=_tmpA97;});_tmp5D3;}));
goto _LL0;case 402: _LL31F: _LL320:
# 2492 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 403: _LL321: _LL322:
# 2493 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA9A=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA99=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_seq_exp(_tmpA9A,_tmpA99,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 404: _LL323: _LL324:
# 2497 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 405: _LL325: _LL326:
# 2499 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Core_Opt*_tmpA9C=Cyc_yyget_YY7(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpA9B=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assignop_exp(_tmpA9D,_tmpA9C,_tmpA9B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 406: _LL327: _LL328:
# 2501 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpA9F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpA9E=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_swap_exp(_tmpA9F,_tmpA9E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 407: _LL329: _LL32A:
# 2505 "parse.y"
 yyval=Cyc_YY7(0);
goto _LL0;case 408: _LL32B: _LL32C:
# 2506 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->v=(void*)Cyc_Absyn_Times;_tmp5D4;}));
goto _LL0;case 409: _LL32D: _LL32E:
# 2507 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->v=(void*)Cyc_Absyn_Div;_tmp5D5;}));
goto _LL0;case 410: _LL32F: _LL330:
# 2508 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->v=(void*)Cyc_Absyn_Mod;_tmp5D6;}));
goto _LL0;case 411: _LL331: _LL332:
# 2509 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));_tmp5D7->v=(void*)Cyc_Absyn_Plus;_tmp5D7;}));
goto _LL0;case 412: _LL333: _LL334:
# 2510 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->v=(void*)Cyc_Absyn_Minus;_tmp5D8;}));
goto _LL0;case 413: _LL335: _LL336:
# 2511 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->v=(void*)Cyc_Absyn_Bitlshift;_tmp5D9;}));
goto _LL0;case 414: _LL337: _LL338:
# 2512 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->v=(void*)Cyc_Absyn_Bitlrshift;_tmp5DA;}));
goto _LL0;case 415: _LL339: _LL33A:
# 2513 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->v=(void*)Cyc_Absyn_Bitand;_tmp5DB;}));
goto _LL0;case 416: _LL33B: _LL33C:
# 2514 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));_tmp5DC->v=(void*)Cyc_Absyn_Bitxor;_tmp5DC;}));
goto _LL0;case 417: _LL33D: _LL33E:
# 2515 "parse.y"
 yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->v=(void*)Cyc_Absyn_Bitor;_tmp5DD;}));
goto _LL0;case 418: _LL33F: _LL340:
# 2519 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 419: _LL341: _LL342:
# 2521 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAA1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAA0=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_conditional_exp(_tmpAA2,_tmpAA1,_tmpAA0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 420: _LL343: _LL344:
# 2523 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA3=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_throw_exp(_tmpAA3,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 421: _LL345: _LL346:
# 2525 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA4=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpAA4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 422: _LL347: _LL348:
# 2526 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA5=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_New_exp(0,_tmpAA5,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 423: _LL349: _LL34A:
# 2527 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAA6=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpAA7,_tmpAA6,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 424: _LL34B: _LL34C:
# 2529 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);struct Cyc_Absyn_Exp*_tmpAA8=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);Cyc_Absyn_New_exp(_tmpAA9,_tmpAA8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 425: _LL34D: _LL34E:
# 2533 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 426: _LL34F: _LL350:
# 2536
 yyval=(yyyvsp[0]).v;
goto _LL0;case 427: _LL351: _LL352:
# 2538 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAAB=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAAA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_or_exp(_tmpAAB,_tmpAAA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 428: _LL353: _LL354:
# 2541
 yyval=(yyyvsp[0]).v;
goto _LL0;case 429: _LL355: _LL356:
# 2543 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAAC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_and_exp(_tmpAAD,_tmpAAC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 430: _LL357: _LL358:
# 2546
 yyval=(yyyvsp[0]).v;
goto _LL0;case 431: _LL359: _LL35A:
# 2548 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAAF=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAAE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpAAF,_tmpAAE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 432: _LL35B: _LL35C:
# 2551
 yyval=(yyyvsp[0]).v;
goto _LL0;case 433: _LL35D: _LL35E:
# 2553 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAB0=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpAB1,_tmpAB0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 434: _LL35F: _LL360:
# 2556
 yyval=(yyyvsp[0]).v;
goto _LL0;case 435: _LL361: _LL362:
# 2558 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAB2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpAB3,_tmpAB2,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 436: _LL363: _LL364:
# 2561
 yyval=(yyyvsp[0]).v;
goto _LL0;case 437: _LL365: _LL366:
# 2563 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_tmpAB6)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAB5=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAB4=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpAB6(_tmpAB5,_tmpAB4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 438: _LL367: _LL368:
# 2566
 yyval=(yyyvsp[0]).v;
goto _LL0;case 439: _LL369: _LL36A:
# 2568 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*(*_tmpAB9)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned)=Cyc_yyget_YY66(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAB8=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAB7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmpAB9(_tmpAB8,_tmpAB7,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 440: _LL36B: _LL36C:
# 2571
 yyval=(yyyvsp[0]).v;
goto _LL0;case 441: _LL36D: _LL36E:
# 2573 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpABB=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpABA=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpABB,_tmpABA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 442: _LL36F: _LL370:
# 2575 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpABD=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpABC=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpABD,_tmpABC,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 443: _LL371: _LL372:
# 2578
 yyval=(yyyvsp[0]).v;
goto _LL0;case 444: _LL373: _LL374:
# 2580 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpAC0=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpABF=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpABE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpAC0,_tmpABF,_tmpABE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 445: _LL375: _LL376:
# 2583
 yyval=(yyyvsp[0]).v;
goto _LL0;case 446: _LL377: _LL378:
# 2585 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpAC3=Cyc_yyget_YY6(&(yyyvsp[1]).v);struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpAC1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_prim2_exp(_tmpAC3,_tmpAC2,_tmpAC1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 447: _LL379: _LL37A:
# 2588
 yyval=Cyc_YY66(Cyc_Absyn_eq_exp);
goto _LL0;case 448: _LL37B: _LL37C:
# 2589 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_neq_exp);
goto _LL0;case 449: _LL37D: _LL37E:
# 2592
 yyval=Cyc_YY66(Cyc_Absyn_lt_exp);
goto _LL0;case 450: _LL37F: _LL380:
# 2593 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gt_exp);
goto _LL0;case 451: _LL381: _LL382:
# 2594 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_lte_exp);
goto _LL0;case 452: _LL383: _LL384:
# 2595 "parse.y"
 yyval=Cyc_YY66(Cyc_Absyn_gte_exp);
goto _LL0;case 453: _LL385: _LL386:
# 2598
 yyval=Cyc_YY6(Cyc_Absyn_Plus);
goto _LL0;case 454: _LL387: _LL388:
# 2599 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 455: _LL389: _LL38A:
# 2602
 yyval=Cyc_YY6(Cyc_Absyn_Times);
goto _LL0;case 456: _LL38B: _LL38C:
# 2603 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Div);
goto _LL0;case 457: _LL38D: _LL38E:
# 2604 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Mod);
goto _LL0;case 458: _LL38F: _LL390:
# 2608 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 459: _LL391: _LL392: {
# 2610 "parse.y"
void*_tmp5DE=({struct _tuple8*_tmpAC4=Cyc_yyget_YY38(&(yyyvsp[1]).v);Cyc_Parse_type_name_to_type(_tmpAC4,(unsigned)((yyyvsp[1]).l).first_line);});void*t=_tmp5DE;
yyval=Cyc_Exp_tok(({void*_tmpAC6=t;struct Cyc_Absyn_Exp*_tmpAC5=Cyc_yyget_Exp_tok(&(yyyvsp[3]).v);Cyc_Absyn_cast_exp(_tmpAC6,_tmpAC5,1,Cyc_Absyn_Unknown_coercion,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;}case 460: _LL393: _LL394:
# 2615 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 461: _LL395: _LL396:
# 2616 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpAC7,Cyc_Absyn_PreInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 462: _LL397: _LL398:
# 2617 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_increment_exp(_tmpAC8,Cyc_Absyn_PreDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 463: _LL399: _LL39A:
# 2618 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAC9=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_address_exp(_tmpAC9,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 464: _LL39B: _LL39C:
# 2619 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpACA=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_deref_exp(_tmpACA,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 465: _LL39D: _LL39E:
# 2620 "parse.y"
 yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpACC=Cyc_yyget_YY6(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpACB=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_prim1_exp(_tmpACC,_tmpACB,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 466: _LL39F: _LL3A0: {
# 2622 "parse.y"
void*_tmp5DF=({struct _tuple8*_tmpACD=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpACD,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5DF;
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
# 2625
goto _LL0;}case 467: _LL3A1: _LL3A2:
# 2625 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpACE=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_sizeofexp_exp(_tmpACE,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 468: _LL3A3: _LL3A4: {
# 2627 "parse.y"
void*_tmp5E0=({struct _tuple8*_tmpACF=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpACF,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5E0;
yyval=Cyc_Exp_tok(({void*_tmpAD1=t;struct Cyc_List_List*_tmpAD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY3(&(yyyvsp[4]).v));Cyc_Absyn_offsetof_exp(_tmpAD1,_tmpAD0,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2630
goto _LL0;}case 469: _LL3A5: _LL3A6:
# 2632
 yyval=Cyc_Exp_tok(({void*_tmpAD3=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->tag=34,(_tmp5E1->f1).is_calloc=0,(_tmp5E1->f1).rgn=0,(_tmp5E1->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAD2=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5E1->f1).num_elts=_tmpAD2;}),(_tmp5E1->f1).fat_result=0,(_tmp5E1->f1).inline_call=0;_tmp5E1;});Cyc_Absyn_new_exp(_tmpAD3,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2634
goto _LL0;case 470: _LL3A7: _LL3A8:
# 2635 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAD6=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->tag=34,(_tmp5E2->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpAD5=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5E2->f1).rgn=_tmpAD5;}),(_tmp5E2->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAD4=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5E2->f1).num_elts=_tmpAD4;}),(_tmp5E2->f1).fat_result=0,(_tmp5E2->f1).inline_call=0;_tmp5E2;});Cyc_Absyn_new_exp(_tmpAD6,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2637
goto _LL0;case 471: _LL3A9: _LL3AA:
# 2638 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAD9=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->tag=34,(_tmp5E3->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5E3->f1).rgn=_tmpAD8;}),(_tmp5E3->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5E3->f1).num_elts=_tmpAD7;}),(_tmp5E3->f1).fat_result=0,(_tmp5E3->f1).inline_call=1;_tmp5E3;});Cyc_Absyn_new_exp(_tmpAD9,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2640
goto _LL0;case 472: _LL3AB: _LL3AC: {
# 2641 "parse.y"
void*_tmp5E4=({struct _tuple8*_tmpADA=Cyc_yyget_YY38(&(yyyvsp[6]).v);Cyc_Parse_type_name_to_type(_tmpADA,(unsigned)((yyyvsp[6]).l).first_line);});void*t=_tmp5E4;
yyval=Cyc_Exp_tok(({void*_tmpADD=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->tag=34,(_tmp5E6->f1).is_calloc=1,(_tmp5E6->f1).rgn=0,({void**_tmpADC=({void**_tmp5E5=_cycalloc(sizeof(*_tmp5E5));*_tmp5E5=t;_tmp5E5;});(_tmp5E6->f1).elt_type=_tmpADC;}),({struct Cyc_Absyn_Exp*_tmpADB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5E6->f1).num_elts=_tmpADB;}),(_tmp5E6->f1).fat_result=0,(_tmp5E6->f1).inline_call=0;_tmp5E6;});Cyc_Absyn_new_exp(_tmpADD,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2644
goto _LL0;}case 473: _LL3AD: _LL3AE: {
# 2646
void*_tmp5E7=({struct _tuple8*_tmpADE=Cyc_yyget_YY38(&(yyyvsp[8]).v);Cyc_Parse_type_name_to_type(_tmpADE,(unsigned)((yyyvsp[8]).l).first_line);});void*t=_tmp5E7;
yyval=Cyc_Exp_tok(({void*_tmpAE2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->tag=34,(_tmp5E9->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpAE1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);(_tmp5E9->f1).rgn=_tmpAE1;}),({void**_tmpAE0=({void**_tmp5E8=_cycalloc(sizeof(*_tmp5E8));*_tmp5E8=t;_tmp5E8;});(_tmp5E9->f1).elt_type=_tmpAE0;}),({struct Cyc_Absyn_Exp*_tmpADF=Cyc_yyget_Exp_tok(&(yyyvsp[4]).v);(_tmp5E9->f1).num_elts=_tmpADF;}),(_tmp5E9->f1).fat_result=0,(_tmp5E9->f1).inline_call=0;_tmp5E9;});Cyc_Absyn_new_exp(_tmpAE2,(unsigned)((yyyvsp[0]).l).first_line);}));
# 2649
goto _LL0;}case 474: _LL3AF: _LL3B0:
# 2650 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpAE4=({struct Cyc_Absyn_Exp*_tmp5EA[1];({struct Cyc_Absyn_Exp*_tmpAE3=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5EA[0]=_tmpAE3;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp5EA,sizeof(struct Cyc_Absyn_Exp*),1));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpAE4,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 475: _LL3B1: _LL3B2:
# 2652 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAE8=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->tag=38,({struct Cyc_Absyn_Exp*_tmpAE7=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp5EC->f1=_tmpAE7;}),({struct _fat_ptr*_tmpAE6=({struct _fat_ptr*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));({struct _fat_ptr _tmpAE5=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp5EB=_tmpAE5;});_tmp5EB;});_tmp5EC->f2=_tmpAE6;});_tmp5EC;});Cyc_Absyn_new_exp(_tmpAE8,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 476: _LL3B3: _LL3B4:
# 2654 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAED=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->tag=38,({struct Cyc_Absyn_Exp*_tmpAEC=({struct Cyc_Absyn_Exp*_tmpAEB=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_deref_exp(_tmpAEB,(unsigned)((yyyvsp[2]).l).first_line);});_tmp5EE->f1=_tmpAEC;}),({struct _fat_ptr*_tmpAEA=({struct _fat_ptr*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));({struct _fat_ptr _tmpAE9=Cyc_yyget_String_tok(&(yyyvsp[4]).v);*_tmp5ED=_tmpAE9;});_tmp5ED;});_tmp5EE->f2=_tmpAEA;});_tmp5EE;});Cyc_Absyn_new_exp(_tmpAED,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 477: _LL3B5: _LL3B6: {
# 2656 "parse.y"
void*_tmp5EF=({struct _tuple8*_tmpAEE=Cyc_yyget_YY38(&(yyyvsp[2]).v);Cyc_Parse_type_name_to_type(_tmpAEE,(unsigned)((yyyvsp[2]).l).first_line);});void*t=_tmp5EF;
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(t,(unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;}case 478: _LL3B7: _LL3B8:
# 2658 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpAEF=Cyc_yyget_YY60(&(yyyvsp[1]).v);Cyc_Absyn_new_exp(_tmpAEF,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 479: _LL3B9: _LL3BA:
# 2659 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAF0=Cyc_yyget_Exp_tok(&(yyyvsp[1]).v);Cyc_Absyn_extension_exp(_tmpAF0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 480: _LL3BB: _LL3BC:
# 2660 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpAF1=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_assert_exp(_tmpAF1,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 481: _LL3BD: _LL3BE:
# 2664 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;case 482: _LL3BF: _LL3C0:
# 2665 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;case 483: _LL3C1: _LL3C2:
# 2666 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;case 484: _LL3C3: _LL3C4:
# 2667 "parse.y"
 yyval=Cyc_YY6(Cyc_Absyn_Plus);
goto _LL0;case 485: _LL3C5: _LL3C6: {
# 2672 "parse.y"
struct _tuple30*_tmp5F0=Cyc_yyget_YY61(&(yyyvsp[3]).v);struct _tuple30*_stmttmp2B=_tmp5F0;struct _tuple30*_tmp5F1=_stmttmp2B;struct Cyc_List_List*_tmp5F4;struct Cyc_List_List*_tmp5F3;struct Cyc_List_List*_tmp5F2;_LL49E: _tmp5F2=_tmp5F1->f1;_tmp5F3=_tmp5F1->f2;_tmp5F4=_tmp5F1->f3;_LL49F: {struct Cyc_List_List*outlist=_tmp5F2;struct Cyc_List_List*inlist=_tmp5F3;struct Cyc_List_List*clobbers=_tmp5F4;
yyval=Cyc_YY60((void*)({struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=40,({int _tmpAF3=Cyc_yyget_YY32(&(yyyvsp[0]).v);_tmp5F5->f1=_tmpAF3;}),({struct _fat_ptr _tmpAF2=Cyc_yyget_String_tok(&(yyyvsp[2]).v);_tmp5F5->f2=_tmpAF2;}),_tmp5F5->f3=outlist,_tmp5F5->f4=inlist,_tmp5F5->f5=clobbers;_tmp5F5;}));
goto _LL0;}}case 486: _LL3C7: _LL3C8:
# 2677 "parse.y"
 yyval=Cyc_YY32(0);
goto _LL0;case 487: _LL3C9: _LL3CA:
# 2678 "parse.y"
 yyval=Cyc_YY32(1);
goto _LL0;case 488: _LL3CB: _LL3CC:
# 2682 "parse.y"
 yyval=Cyc_YY61(({struct _tuple30*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=0,_tmp5F6->f2=0,_tmp5F6->f3=0;_tmp5F6;}));
goto _LL0;case 489: _LL3CD: _LL3CE: {
# 2684 "parse.y"
struct _tuple31*_tmp5F7=Cyc_yyget_YY62(&(yyyvsp[1]).v);struct _tuple31*_stmttmp2C=_tmp5F7;struct _tuple31*_tmp5F8=_stmttmp2C;struct Cyc_List_List*_tmp5FA;struct Cyc_List_List*_tmp5F9;_LL4A1: _tmp5F9=_tmp5F8->f1;_tmp5FA=_tmp5F8->f2;_LL4A2: {struct Cyc_List_List*inlist=_tmp5F9;struct Cyc_List_List*clobbers=_tmp5FA;
yyval=Cyc_YY61(({struct _tuple30*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->f1=0,_tmp5FB->f2=inlist,_tmp5FB->f3=clobbers;_tmp5FB;}));
goto _LL0;}}case 490: _LL3CF: _LL3D0: {
# 2687 "parse.y"
struct _tuple31*_tmp5FC=Cyc_yyget_YY62(&(yyyvsp[2]).v);struct _tuple31*_stmttmp2D=_tmp5FC;struct _tuple31*_tmp5FD=_stmttmp2D;struct Cyc_List_List*_tmp5FF;struct Cyc_List_List*_tmp5FE;_LL4A4: _tmp5FE=_tmp5FD->f1;_tmp5FF=_tmp5FD->f2;_LL4A5: {struct Cyc_List_List*inlist=_tmp5FE;struct Cyc_List_List*clobbers=_tmp5FF;
yyval=Cyc_YY61(({struct _tuple30*_tmp600=_cycalloc(sizeof(*_tmp600));({struct Cyc_List_List*_tmpAF4=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY64(&(yyyvsp[1]).v));_tmp600->f1=_tmpAF4;}),_tmp600->f2=inlist,_tmp600->f3=clobbers;_tmp600;}));
goto _LL0;}}case 491: _LL3D1: _LL3D2:
# 2691
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));({struct _tuple32*_tmpAF5=Cyc_yyget_YY65(&(yyyvsp[0]).v);_tmp601->hd=_tmpAF5;}),_tmp601->tl=0;_tmp601;}));
goto _LL0;case 492: _LL3D3: _LL3D4:
# 2692 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp602=_cycalloc(sizeof(*_tmp602));({struct _tuple32*_tmpAF7=Cyc_yyget_YY65(&(yyyvsp[2]).v);_tmp602->hd=_tmpAF7;}),({struct Cyc_List_List*_tmpAF6=Cyc_yyget_YY64(&(yyyvsp[0]).v);_tmp602->tl=_tmpAF6;});_tmp602;}));
goto _LL0;case 493: _LL3D5: _LL3D6:
# 2696 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->f1=0,_tmp603->f2=0;_tmp603;}));
goto _LL0;case 494: _LL3D7: _LL3D8:
# 2697 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->f1=0,({struct Cyc_List_List*_tmpAF8=Cyc_yyget_YY63(&(yyyvsp[1]).v);_tmp604->f2=_tmpAF8;});_tmp604;}));
goto _LL0;case 495: _LL3D9: _LL3DA:
# 2698 "parse.y"
 yyval=Cyc_YY62(({struct _tuple31*_tmp605=_cycalloc(sizeof(*_tmp605));({struct Cyc_List_List*_tmpAFA=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY64(&(yyyvsp[1]).v));_tmp605->f1=_tmpAFA;}),({struct Cyc_List_List*_tmpAF9=Cyc_yyget_YY63(&(yyyvsp[2]).v);_tmp605->f2=_tmpAF9;});_tmp605;}));
goto _LL0;case 496: _LL3DB: _LL3DC:
# 2701
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp606=_cycalloc(sizeof(*_tmp606));({struct _tuple32*_tmpAFB=Cyc_yyget_YY65(&(yyyvsp[0]).v);_tmp606->hd=_tmpAFB;}),_tmp606->tl=0;_tmp606;}));
goto _LL0;case 497: _LL3DD: _LL3DE:
# 2702 "parse.y"
 yyval=Cyc_YY64(({struct Cyc_List_List*_tmp607=_cycalloc(sizeof(*_tmp607));({struct _tuple32*_tmpAFD=Cyc_yyget_YY65(&(yyyvsp[2]).v);_tmp607->hd=_tmpAFD;}),({struct Cyc_List_List*_tmpAFC=Cyc_yyget_YY64(&(yyyvsp[0]).v);_tmp607->tl=_tmpAFC;});_tmp607;}));
goto _LL0;case 498: _LL3DF: _LL3E0:
# 2706 "parse.y"
 yyval=Cyc_YY65(({struct _tuple32*_tmp608=_cycalloc(sizeof(*_tmp608));({struct _fat_ptr _tmpAFF=Cyc_yyget_String_tok(&(yyyvsp[0]).v);_tmp608->f1=_tmpAFF;}),({struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp608->f2=_tmpAFE;});_tmp608;}));
goto _LL0;case 499: _LL3E1: _LL3E2:
# 2711 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 500: _LL3E3: _LL3E4:
# 2712 "parse.y"
 yyval=Cyc_YY63(0);
goto _LL0;case 501: _LL3E5: _LL3E6:
# 2713 "parse.y"
 yyval=Cyc_YY63(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY63(&(yyyvsp[1]).v)));
goto _LL0;case 502: _LL3E7: _LL3E8:
# 2716
 yyval=Cyc_YY63(({struct Cyc_List_List*_tmp60A=_cycalloc(sizeof(*_tmp60A));({struct _fat_ptr*_tmpB01=({struct _fat_ptr*_tmp609=_cycalloc(sizeof(*_tmp609));({struct _fat_ptr _tmpB00=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp609=_tmpB00;});_tmp609;});_tmp60A->hd=_tmpB01;}),_tmp60A->tl=0;_tmp60A;}));
goto _LL0;case 503: _LL3E9: _LL3EA:
# 2717 "parse.y"
 yyval=Cyc_YY63(({struct Cyc_List_List*_tmp60C=_cycalloc(sizeof(*_tmp60C));({struct _fat_ptr*_tmpB04=({struct _fat_ptr*_tmp60B=_cycalloc(sizeof(*_tmp60B));({struct _fat_ptr _tmpB03=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60B=_tmpB03;});_tmp60B;});_tmp60C->hd=_tmpB04;}),({struct Cyc_List_List*_tmpB02=Cyc_yyget_YY63(&(yyyvsp[0]).v);_tmp60C->tl=_tmpB02;});_tmp60C;}));
goto _LL0;case 504: _LL3EB: _LL3EC:
# 2722 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 505: _LL3ED: _LL3EE:
# 2724 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB06=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_Absyn_Exp*_tmpB05=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);Cyc_Absyn_subscript_exp(_tmpB06,_tmpB05,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 506: _LL3EF: _LL3F0:
# 2726 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB07=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknowncall_exp(_tmpB07,0,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 507: _LL3F1: _LL3F2:
# 2728 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB09=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpB08=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_unknowncall_exp(_tmpB09,_tmpB08,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 508: _LL3F3: _LL3F4:
# 2730 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB0C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpB0B=({struct _fat_ptr*_tmp60D=_cycalloc(sizeof(*_tmp60D));({struct _fat_ptr _tmpB0A=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60D=_tmpB0A;});_tmp60D;});Cyc_Absyn_aggrmember_exp(_tmpB0C,_tmpB0B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 509: _LL3F5: _LL3F6:
# 2732 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB0F=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct _fat_ptr*_tmpB0E=({struct _fat_ptr*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct _fat_ptr _tmpB0D=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp60E=_tmpB0D;});_tmp60E;});Cyc_Absyn_aggrarrow_exp(_tmpB0F,_tmpB0E,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 510: _LL3F7: _LL3F8:
# 2734 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB10=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpB10,Cyc_Absyn_PostInc,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 511: _LL3F9: _LL3FA:
# 2736 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB11=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_increment_exp(_tmpB11,Cyc_Absyn_PostDec,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 512: _LL3FB: _LL3FC:
# 2738 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB13=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp60F=_cycalloc(sizeof(*_tmp60F));_tmp60F->tag=25,({struct _tuple8*_tmpB12=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp60F->f1=_tmpB12;}),_tmp60F->f2=0;_tmp60F;});Cyc_Absyn_new_exp(_tmpB13,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 513: _LL3FD: _LL3FE:
# 2740 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB16=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=25,({struct _tuple8*_tmpB15=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp610->f1=_tmpB15;}),({struct Cyc_List_List*_tmpB14=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp610->f2=_tmpB14;});_tmp610;});Cyc_Absyn_new_exp(_tmpB16,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 514: _LL3FF: _LL400:
# 2742 "parse.y"
 yyval=Cyc_Exp_tok(({void*_tmpB19=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->tag=25,({struct _tuple8*_tmpB18=Cyc_yyget_YY38(&(yyyvsp[1]).v);_tmp611->f1=_tmpB18;}),({struct Cyc_List_List*_tmpB17=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[4]).v));_tmp611->f2=_tmpB17;});_tmp611;});Cyc_Absyn_new_exp(_tmpB19,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 515: _LL401: _LL402:
# 2747 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp614=_cycalloc(sizeof(*_tmp614));({void*_tmpB1C=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp613=_cycalloc(sizeof(*_tmp613));_tmp613->tag=0,({struct _fat_ptr*_tmpB1B=({struct _fat_ptr*_tmp612=_cycalloc(sizeof(*_tmp612));({struct _fat_ptr _tmpB1A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp612=_tmpB1A;});_tmp612;});_tmp613->f1=_tmpB1B;});_tmp613;});_tmp614->hd=_tmpB1C;}),_tmp614->tl=0;_tmp614;}));
goto _LL0;case 516: _LL403: _LL404:
# 2750
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp616=_cycalloc(sizeof(*_tmp616));({void*_tmpB1F=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->tag=1,({unsigned _tmpB1E=({unsigned _tmpB1D=(unsigned)((yyyvsp[0]).l).first_line;Cyc_Parse_cnst2uint(_tmpB1D,Cyc_yyget_Int_tok(&(yyyvsp[0]).v));});_tmp615->f1=_tmpB1E;});_tmp615;});_tmp616->hd=_tmpB1F;}),_tmp616->tl=0;_tmp616;}));
goto _LL0;case 517: _LL405: _LL406:
# 2752 "parse.y"
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp619=_cycalloc(sizeof(*_tmp619));({void*_tmpB23=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618->tag=0,({struct _fat_ptr*_tmpB22=({struct _fat_ptr*_tmp617=_cycalloc(sizeof(*_tmp617));({struct _fat_ptr _tmpB21=Cyc_yyget_String_tok(&(yyyvsp[2]).v);*_tmp617=_tmpB21;});_tmp617;});_tmp618->f1=_tmpB22;});_tmp618;});_tmp619->hd=_tmpB23;}),({struct Cyc_List_List*_tmpB20=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp619->tl=_tmpB20;});_tmp619;}));
goto _LL0;case 518: _LL407: _LL408:
# 2755
 yyval=Cyc_YY3(({struct Cyc_List_List*_tmp61B=_cycalloc(sizeof(*_tmp61B));({void*_tmpB27=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=1,({unsigned _tmpB26=({unsigned _tmpB25=(unsigned)((yyyvsp[2]).l).first_line;Cyc_Parse_cnst2uint(_tmpB25,Cyc_yyget_Int_tok(&(yyyvsp[2]).v));});_tmp61A->f1=_tmpB26;});_tmp61A;});_tmp61B->hd=_tmpB27;}),({struct Cyc_List_List*_tmpB24=Cyc_yyget_YY3(&(yyyvsp[0]).v);_tmp61B->tl=_tmpB24;});_tmp61B;}));
goto _LL0;case 519: _LL409: _LL40A:
# 2760 "parse.y"
 yyval=Cyc_Exp_tok(({struct _tuple0*_tmpB28=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);Cyc_Absyn_unknownid_exp(_tmpB28,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 520: _LL40B: _LL40C:
# 2761 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB29=Cyc_yyget_String_tok(&(yyyvsp[2]).v);Cyc_Absyn_pragma_exp(_tmpB29,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 521: _LL40D: _LL40E:
# 2762 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 522: _LL40F: _LL410:
# 2763 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB2A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_string_exp(_tmpB2A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 523: _LL411: _LL412:
# 2764 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB2B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wstring_exp(_tmpB2B,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 524: _LL413: _LL414:
# 2765 "parse.y"
 yyval=(yyyvsp[1]).v;
goto _LL0;case 525: _LL415: _LL416:
# 2769 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB2C=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);Cyc_Absyn_noinstantiate_exp(_tmpB2C,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 526: _LL417: _LL418:
# 2772
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB2E=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);struct Cyc_List_List*_tmpB2D=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY41(&(yyyvsp[3]).v));Cyc_Absyn_instantiate_exp(_tmpB2E,_tmpB2D,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 527: _LL419: _LL41A:
# 2775
 yyval=Cyc_Exp_tok(({void*_tmpB32=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tag=29,({struct _tuple0*_tmpB31=Cyc_yyget_QualId_tok(&(yyyvsp[0]).v);_tmp61C->f1=_tmpB31;}),({struct Cyc_List_List*_tmpB30=Cyc_yyget_YY41(&(yyyvsp[2]).v);_tmp61C->f2=_tmpB30;}),({struct Cyc_List_List*_tmpB2F=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY5(&(yyyvsp[3]).v));_tmp61C->f3=_tmpB2F;}),_tmp61C->f4=0;_tmp61C;});Cyc_Absyn_new_exp(_tmpB32,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 528: _LL41B: _LL41C:
# 2777 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpB33=Cyc_yyget_YY4(&(yyyvsp[2]).v);Cyc_Absyn_tuple_exp(_tmpB33,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 529: _LL41D: _LL41E:
# 2779 "parse.y"
 yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpB34=Cyc_yyget_Stmt_tok(&(yyyvsp[2]).v);Cyc_Absyn_stmt_exp(_tmpB34,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 530: _LL41F: _LL420:
# 2783 "parse.y"
 yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(Cyc_yyget_YY4(&(yyyvsp[0]).v)));
goto _LL0;case 531: _LL421: _LL422:
# 2788 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp61D=_cycalloc(sizeof(*_tmp61D));({struct Cyc_Absyn_Exp*_tmpB35=Cyc_yyget_Exp_tok(&(yyyvsp[0]).v);_tmp61D->hd=_tmpB35;}),_tmp61D->tl=0;_tmp61D;}));
goto _LL0;case 532: _LL423: _LL424:
# 2790 "parse.y"
 yyval=Cyc_YY4(({struct Cyc_List_List*_tmp61E=_cycalloc(sizeof(*_tmp61E));({struct Cyc_Absyn_Exp*_tmpB37=Cyc_yyget_Exp_tok(&(yyyvsp[2]).v);_tmp61E->hd=_tmpB37;}),({struct Cyc_List_List*_tmpB36=Cyc_yyget_YY4(&(yyyvsp[0]).v);_tmp61E->tl=_tmpB36;});_tmp61E;}));
goto _LL0;case 533: _LL425: _LL426:
# 2796 "parse.y"
 yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpB38=Cyc_yyget_Int_tok(&(yyyvsp[0]).v);Cyc_Absyn_const_exp(_tmpB38,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 534: _LL427: _LL428:
# 2797 "parse.y"
 yyval=Cyc_Exp_tok(({char _tmpB39=Cyc_yyget_Char_tok(&(yyyvsp[0]).v);Cyc_Absyn_char_exp(_tmpB39,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 535: _LL429: _LL42A:
# 2798 "parse.y"
 yyval=Cyc_Exp_tok(({struct _fat_ptr _tmpB3A=Cyc_yyget_String_tok(&(yyyvsp[0]).v);Cyc_Absyn_wchar_exp(_tmpB3A,(unsigned)((yyyvsp[0]).l).first_line);}));
goto _LL0;case 536: _LL42B: _LL42C:
# 2800 "parse.y"
 yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned)((yyyvsp[0]).l).first_line));
goto _LL0;case 537: _LL42D: _LL42E: {
# 2802 "parse.y"
struct _fat_ptr _tmp61F=Cyc_yyget_String_tok(&(yyyvsp[0]).v);struct _fat_ptr f=_tmp61F;
int l=(int)Cyc_strlen((struct _fat_ptr)f);
int i=1;
if(l > 0){
char c=*((const char*)_check_fat_subscript(f,sizeof(char),l - 1));
if((int)c == (int)'f' ||(int)c == (int)'F')i=0;else{
if((int)c == (int)'l' ||(int)c == (int)'L')i=2;}}
# 2810
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(f,i,(unsigned)((yyyvsp[0]).l).first_line));
# 2812
goto _LL0;}case 538: _LL42F: _LL430:
# 2815 "parse.y"
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp621=_cycalloc(sizeof(*_tmp621));({union Cyc_Absyn_Nmspace _tmpB3D=Cyc_Absyn_Rel_n(0);_tmp621->f1=_tmpB3D;}),({struct _fat_ptr*_tmpB3C=({struct _fat_ptr*_tmp620=_cycalloc(sizeof(*_tmp620));({struct _fat_ptr _tmpB3B=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp620=_tmpB3B;});_tmp620;});_tmp621->f2=_tmpB3C;});_tmp621;}));
goto _LL0;case 539: _LL431: _LL432:
# 2816 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 540: _LL433: _LL434:
# 2819
 yyval=Cyc_QualId_tok(({struct _tuple0*_tmp623=_cycalloc(sizeof(*_tmp623));({union Cyc_Absyn_Nmspace _tmpB40=Cyc_Absyn_Rel_n(0);_tmp623->f1=_tmpB40;}),({struct _fat_ptr*_tmpB3F=({struct _fat_ptr*_tmp622=_cycalloc(sizeof(*_tmp622));({struct _fat_ptr _tmpB3E=Cyc_yyget_String_tok(&(yyyvsp[0]).v);*_tmp622=_tmpB3E;});_tmp622;});_tmp623->f2=_tmpB3F;});_tmp623;}));
goto _LL0;case 541: _LL435: _LL436:
# 2820 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 542: _LL437: _LL438:
# 2825 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 543: _LL439: _LL43A:
# 2826 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 544: _LL43B: _LL43C:
# 2829
 yyval=(yyyvsp[0]).v;
goto _LL0;case 545: _LL43D: _LL43E:
# 2830 "parse.y"
 yyval=(yyyvsp[0]).v;
goto _LL0;case 546: _LL43F: _LL440:
# 2835 "parse.y"
 goto _LL0;case 547: _LL441: _LL442:
# 2835 "parse.y"
 yylex_buf->lex_curr_pos -=1;
goto _LL0;default: _LL443: _LL444:
# 2839
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
({int _tmpB41=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_line;((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),yyvsp_offset))).l).last_line=_tmpB41;});
((((struct Cyc_Yystacktype*)yyvs.curr)[yyvsp_offset]).l).last_column=((*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),(yyvsp_offset + yylen)- 1))).l).last_column;}
# 409
yyn=(int)Cyc_yyr1[yyn];
# 411
yystate=({int _tmpB42=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,167U,sizeof(short),yyn - 153));_tmpB42 + (int)*((short*)_check_fat_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0 && yystate <= 6508)&&(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),yystate))== (int)((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
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
yyn=(int)Cyc_yypact[yystate];
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
({unsigned long _tmpB43=Cyc_strlen((struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)))+ (unsigned long)15;sze +=_tmpB43;}),count ++;}
msg=({unsigned _tmp625=(unsigned)(sze + 15)+ 1U;char*_tmp624=({struct _RegionHandle*_tmpB44=yyregion;_region_malloc(_tmpB44,_check_times(_tmp625,sizeof(char)));});({{unsigned _tmp6ED=(unsigned)(sze + 15);unsigned i;for(i=0;i < _tmp6ED;++ i){_tmp624[i]='\000';}_tmp624[_tmp6ED]=0;}0;});_tag_fat(_tmp624,sizeof(char),_tmp625);});
({struct _fat_ptr _tmpB45=msg;Cyc_strcpy(_tmpB45,({const char*_tmp626="parse error";_tag_fat(_tmp626,sizeof(char),12U);}));});
# 444
if(count < 5){
# 446
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned)x < 320U / sizeof(char*);++ x){
# 449
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,6509U,sizeof(short),x + yyn))== x){
# 451
({struct _fat_ptr _tmpB46=msg;Cyc_strcat(_tmpB46,(struct _fat_ptr)(count == 0?(struct _fat_ptr)({const char*_tmp627=", expecting `";_tag_fat(_tmp627,sizeof(char),14U);}):(struct _fat_ptr)({const char*_tmp628=" or `";_tag_fat(_tmp628,sizeof(char),6U);})));});
# 454
Cyc_strcat(msg,(struct _fat_ptr)*((struct _fat_ptr*)_check_known_subscript_notnull(Cyc_yytname,320U,sizeof(struct _fat_ptr),x)));
({struct _fat_ptr _tmpB47=msg;Cyc_strcat(_tmpB47,({const char*_tmp629="'";_tag_fat(_tmp629,sizeof(char),2U);}));});
++ count;}}}
# 459
Cyc_yyerror((struct _fat_ptr)msg,yystate,yychar);}else{
# 463
({struct _fat_ptr _tmpB49=({const char*_tmp62A="parse error";_tag_fat(_tmp62A,sizeof(char),12U);});int _tmpB48=yystate;Cyc_yyerror(_tmpB49,_tmpB48,yychar);});}}
# 465
goto yyerrlab1;
# 467
yyerrlab1:
# 469
 if(yyerrstatus == 3){
# 474
if(yychar == 0){
int _tmp62B=1;_npop_handler(0);return _tmp62B;}
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
 if(yyssp_offset == 0){int _tmp62C=1;_npop_handler(0);return _tmp62C;}
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
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 531
if(yyn == -32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 535
if(yyn == 0)goto yyerrpop;}
# 537
if(yyn == 1066){
int _tmp62D=0;_npop_handler(0);return _tmp62D;}
# 546
({struct Cyc_Yystacktype _tmpB4A=({struct Cyc_Yystacktype _tmp6EE;_tmp6EE.v=yylval,_tmp6EE.l=yylloc;_tmp6EE;});*((struct Cyc_Yystacktype*)_check_fat_subscript(yyvs,sizeof(struct Cyc_Yystacktype),++ yyvsp_offset))=_tmpB4A;});
# 551
goto yynewstate;}
# 149 "cycbison.simple"
;_pop_region();}
# 2838 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp631=v;struct Cyc_Absyn_Stmt*_tmp632;struct Cyc_Absyn_Exp*_tmp633;struct _tuple0*_tmp634;struct _fat_ptr _tmp635;char _tmp636;union Cyc_Absyn_Cnst _tmp637;switch((_tmp631.Stmt_tok).tag){case 1: _LL1: _tmp637=(_tmp631.Int_tok).val;_LL2: {union Cyc_Absyn_Cnst c=_tmp637;
({struct Cyc_String_pa_PrintArg_struct _tmp63A=({struct Cyc_String_pa_PrintArg_struct _tmp6F1;_tmp6F1.tag=0,({struct _fat_ptr _tmpB4B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_cnst2string(c));_tmp6F1.f1=_tmpB4B;});_tmp6F1;});void*_tmp638[1];_tmp638[0]=& _tmp63A;({struct Cyc___cycFILE*_tmpB4D=Cyc_stderr;struct _fat_ptr _tmpB4C=({const char*_tmp639="%s";_tag_fat(_tmp639,sizeof(char),3U);});Cyc_fprintf(_tmpB4D,_tmpB4C,_tag_fat(_tmp638,sizeof(void*),1));});});goto _LL0;}case 2: _LL3: _tmp636=(_tmp631.Char_tok).val;_LL4: {char c=_tmp636;
({struct Cyc_Int_pa_PrintArg_struct _tmp63D=({struct Cyc_Int_pa_PrintArg_struct _tmp6F2;_tmp6F2.tag=1,_tmp6F2.f1=(unsigned long)((int)c);_tmp6F2;});void*_tmp63B[1];_tmp63B[0]=& _tmp63D;({struct Cyc___cycFILE*_tmpB4F=Cyc_stderr;struct _fat_ptr _tmpB4E=({const char*_tmp63C="%c";_tag_fat(_tmp63C,sizeof(char),3U);});Cyc_fprintf(_tmpB4F,_tmpB4E,_tag_fat(_tmp63B,sizeof(void*),1));});});goto _LL0;}case 3: _LL5: _tmp635=(_tmp631.String_tok).val;_LL6: {struct _fat_ptr s=_tmp635;
({struct Cyc_String_pa_PrintArg_struct _tmp640=({struct Cyc_String_pa_PrintArg_struct _tmp6F3;_tmp6F3.tag=0,_tmp6F3.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp6F3;});void*_tmp63E[1];_tmp63E[0]=& _tmp640;({struct Cyc___cycFILE*_tmpB51=Cyc_stderr;struct _fat_ptr _tmpB50=({const char*_tmp63F="\"%s\"";_tag_fat(_tmp63F,sizeof(char),5U);});Cyc_fprintf(_tmpB51,_tmpB50,_tag_fat(_tmp63E,sizeof(void*),1));});});goto _LL0;}case 4: _LL7: _tmp634=(_tmp631.QualId_tok).val;_LL8: {struct _tuple0*q=_tmp634;
({struct Cyc_String_pa_PrintArg_struct _tmp643=({struct Cyc_String_pa_PrintArg_struct _tmp6F4;_tmp6F4.tag=0,({struct _fat_ptr _tmpB52=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(q));_tmp6F4.f1=_tmpB52;});_tmp6F4;});void*_tmp641[1];_tmp641[0]=& _tmp643;({struct Cyc___cycFILE*_tmpB54=Cyc_stderr;struct _fat_ptr _tmpB53=({const char*_tmp642="%s";_tag_fat(_tmp642,sizeof(char),3U);});Cyc_fprintf(_tmpB54,_tmpB53,_tag_fat(_tmp641,sizeof(void*),1));});});goto _LL0;}case 5: _LL9: _tmp633=(_tmp631.Exp_tok).val;_LLA: {struct Cyc_Absyn_Exp*e=_tmp633;
({struct Cyc_String_pa_PrintArg_struct _tmp646=({struct Cyc_String_pa_PrintArg_struct _tmp6F5;_tmp6F5.tag=0,({struct _fat_ptr _tmpB55=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp6F5.f1=_tmpB55;});_tmp6F5;});void*_tmp644[1];_tmp644[0]=& _tmp646;({struct Cyc___cycFILE*_tmpB57=Cyc_stderr;struct _fat_ptr _tmpB56=({const char*_tmp645="%s";_tag_fat(_tmp645,sizeof(char),3U);});Cyc_fprintf(_tmpB57,_tmpB56,_tag_fat(_tmp644,sizeof(void*),1));});});goto _LL0;}case 6: _LLB: _tmp632=(_tmp631.Stmt_tok).val;_LLC: {struct Cyc_Absyn_Stmt*s=_tmp632;
({struct Cyc_String_pa_PrintArg_struct _tmp649=({struct Cyc_String_pa_PrintArg_struct _tmp6F6;_tmp6F6.tag=0,({struct _fat_ptr _tmpB58=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_stmt2string(s));_tmp6F6.f1=_tmpB58;});_tmp6F6;});void*_tmp647[1];_tmp647[0]=& _tmp649;({struct Cyc___cycFILE*_tmpB5A=Cyc_stderr;struct _fat_ptr _tmpB59=({const char*_tmp648="%s";_tag_fat(_tmp648,sizeof(char),3U);});Cyc_fprintf(_tmpB5A,_tmpB59,_tag_fat(_tmp647,sizeof(void*),1));});});goto _LL0;}default: _LLD: _LLE:
({void*_tmp64A=0U;({struct Cyc___cycFILE*_tmpB5C=Cyc_stderr;struct _fat_ptr _tmpB5B=({const char*_tmp64B="?";_tag_fat(_tmp64B,sizeof(char),2U);});Cyc_fprintf(_tmpB5C,_tmpB5B,_tag_fat(_tmp64A,sizeof(void*),0));});});goto _LL0;}_LL0:;}
# 2850
struct _fat_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp64C="end-of-file";_tag_fat(_tmp64C,sizeof(char),12U);});
if(token == 368)
return Cyc_Lex_token_string;
if(token == 377)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);{
int z=token > 0 && token <= 380?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,381U,sizeof(short),token)): 320;
if((unsigned)z < 320U)
return Cyc_yytname[z];
return _tag_fat(0,0,0);}}
# 2864
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp64D=_new_region("yyr");struct _RegionHandle*yyr=& _tmp64D;_push_region(yyr);
({struct _RegionHandle*_tmpB5D=yyr;Cyc_yyparse(_tmpB5D,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp64E=Cyc_Parse_parse_result;_npop_handler(0);return _tmp64E;}
# 2867
;_pop_region();}}
