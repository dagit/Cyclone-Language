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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 292 "core.h"
void Cyc_Core_rethrow(void*);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};
# 38 "cycboot.h"
extern int Cyc_open(const char*,int,struct _fat_ptr);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 104
extern int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
extern int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 224 "cycboot.h"
extern int Cyc_vfprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 300 "cycboot.h"
extern int isspace(int);
# 310
extern int toupper(int);
# 318
extern int system(const char*);
extern void exit(int);struct Cyc_timeval{long tv_sec;long tv_usec;};
# 331
extern int mkdir(const char*,unsigned short);
# 334
extern int close(int);
extern int chdir(const char*);
extern struct _fat_ptr Cyc_getcwd(struct _fat_ptr,unsigned long);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _fat_ptr lex_base;struct _fat_ptr lex_backtrk;struct _fat_ptr lex_default;struct _fat_ptr lex_trans;struct _fat_ptr lex_check;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 82
extern struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
extern char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
extern int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 75
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 82
extern struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 85
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*,void*);
# 94
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
extern struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*,struct Cyc_Set_Set*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 109
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);
# 30 "filename.h"
extern struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr,struct _fat_ptr);
# 34
extern struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr);
# 40
extern struct _fat_ptr Cyc_Filename_dirname(struct _fat_ptr);
# 43
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr,struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);struct Cyc_Buffer_t;
# 50 "buffer.h"
extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned);
# 58
extern struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 81
extern void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 92 "buffer.h"
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 28 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1062 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1119
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1127
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 24 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 29 "specsfile.h"
struct _fat_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr);
# 36
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*,struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _fat_ptr Cyc_Specsfile_def_lib_path;
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 44
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr);
# 77 "buildlib.cyl"
extern void Cyc_Lex_lex_init(int);static char _tmp0[4U]="gcc";
# 87
static struct _fat_ptr Cyc_cyclone_cc={_tmp0,_tmp0,_tmp0 + 4U};static char _tmp1[1U]="";
static struct _fat_ptr Cyc_target_cflags={_tmp1,_tmp1,_tmp1 + 1U};
# 90
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
# 93
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 97
int Cyc_log(struct _fat_ptr fmt,struct _fat_ptr ap){
# 100
if(Cyc_log_file == 0){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp3F5=Cyc_stderr;struct _fat_ptr _tmp3F4=({const char*_tmp3="Internal error: log file is NULL\n";_tag_fat(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp3F5,_tmp3F4,_tag_fat(_tmp2,sizeof(void*),0));});});
exit(1);}{
# 104
int _tmp4=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);int x=_tmp4;
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return x;}}
# 109
static struct _fat_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _fat_ptr*sptr){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp3F6=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);*_tmp5=_tmp3F6;});_tmp5;});}struct _tuple13{struct _fat_ptr*f1;struct Cyc_Set_Set*f2;};
# 117
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 123
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple14{struct _fat_ptr f1;struct _fat_ptr*f2;};
# 131
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _fat_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple15{struct _fat_ptr f1;struct _fat_ptr f2;};struct _tuple16{struct _fat_ptr*f1;struct _fat_ptr*f2;};struct _tuple17{struct _fat_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;struct Cyc_List_List*f8;};
# 147
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _fat_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_user_defs=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
struct _fat_ptr Cyc_current_symbol={(void*)0,(void*)0,(void*)(0 + 0)};
int Cyc_rename_current_symbol=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 168
int Cyc_numdef=0;
# 170
static struct Cyc_List_List*Cyc_cppargs=0;static char _tmp6[14U]="BUILDLIB_sym_";
# 172
struct _fat_ptr Cyc_user_prefix={_tmp6,_tmp6,_tmp6 + 14U};
static struct _fat_ptr*Cyc_add_user_prefix(struct _fat_ptr*symbol){
struct _fat_ptr s=(struct _fat_ptr)Cyc_strconcat((struct _fat_ptr)Cyc_user_prefix,(struct _fat_ptr)*symbol);
return({struct _fat_ptr*_tmp7=_cycalloc(sizeof(*_tmp7));*_tmp7=s;_tmp7;});}
# 177
static struct _fat_ptr Cyc_remove_user_prefix(struct _fat_ptr symbol){
unsigned prefix_len=Cyc_strlen((struct _fat_ptr)Cyc_user_prefix);
if(Cyc_strncmp((struct _fat_ptr)symbol,(struct _fat_ptr)Cyc_user_prefix,prefix_len)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp397;_tmp397.tag=0,_tmp397.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp397;});void*_tmp8[1];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp3F8=Cyc_stderr;struct _fat_ptr _tmp3F7=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_fat(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp3F8,_tmp3F7,_tag_fat(_tmp8,sizeof(void*),1));});});
return symbol;}
# 183
return(struct _fat_ptr)({struct _fat_ptr _tmp3FB=(struct _fat_ptr)symbol;int _tmp3FA=(int)prefix_len;Cyc_substring(_tmp3FB,_tmp3FA,({unsigned long _tmp3F9=Cyc_strlen((struct _fat_ptr)symbol);_tmp3F9 - prefix_len;}));});}
# 186
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;void*_tmpD;switch(*((int*)_tmpC)){case 5: _tmpD=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpD;
# 189
({struct _fat_ptr*_tmp3FD=({struct _fat_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _fat_ptr _tmp3FC=Cyc_remove_user_prefix(*(*ad->name).f2);*_tmpE=_tmp3FC;});_tmpE;});(*ad->name).f2=_tmp3FD;});goto _LL0;}case 7: _tmpD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmpD;
# 191
({struct _fat_ptr*_tmp3FF=({struct _fat_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _fat_ptr _tmp3FE=Cyc_remove_user_prefix(*(*ed->name).f2);*_tmpF=_tmp3FE;});_tmpF;});(*ed->name).f2=_tmp3FF;});goto _LL0;}case 8: _tmpD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmpD;
# 193
({struct _fat_ptr*_tmp401=({struct _fat_ptr*_tmp10=_cycalloc(sizeof(*_tmp10));({struct _fat_ptr _tmp400=Cyc_remove_user_prefix(*(*td->name).f2);*_tmp10=_tmp400;});_tmp10;});(*td->name).f2=_tmp401;});goto _LL0;}case 1:
# 195
 goto _LLA;case 0: _LLA:
 goto _LLC;case 4: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 2: _LL10:
 goto _LL12;case 3: _LL12:
 goto _LL14;case 9: _LL14:
 goto _LL16;case 10: _LL16:
 goto _LL18;case 11: _LL18:
 goto _LL1A;case 12: _LL1A:
 goto _LL1C;case 13: _LL1C:
 goto _LL1E;case 14: _LL1E:
 goto _LL20;case 15: _LL20:
 goto _LL22;default: _LL22:
# 209
({void*_tmp11=0U;({struct Cyc___cycFILE*_tmp403=Cyc_stderr;struct _fat_ptr _tmp402=({const char*_tmp12="Error in .cys file: bad user-defined type definition\n";_tag_fat(_tmp12,sizeof(char),54U);});Cyc_fprintf(_tmp403,_tmp402,_tag_fat(_tmp11,sizeof(void*),0));});});
exit(1);}_LL0:;}
# 216
const int Cyc_lex_base[431U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431U]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 223
int state;int base;int backtrk;
int c;
state=start_state;
# 227
if(state >= 0){
({int _tmp404=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp404;});
lbuf->lex_last_action=-1;}else{
# 231
state=(- state)- 1;}
# 233
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,431U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 241
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 245
c=256;}}else{
# 247
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 250
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,3846U,sizeof(int),base + c))== state)
state=Cyc_lex_trans[base + c];else{
# 253
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == -1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp405=({const char*_tmp13="empty token";_tag_fat(_tmp13,sizeof(char),12U);});_tmp14->f1=_tmp405;});_tmp14;}));else{
# 259
return lbuf->lex_last_action;}}else{
# 262
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 266
struct _tuple13*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp15=lexstate;switch((int)_tmp15){case 0:
# 225 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(1;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Set_Set*_tmp408=({struct Cyc_Set_Set*(*_tmp407)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_delete;});struct Cyc_Set_Set*_tmp406=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp407(_tmp406,(struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});*_tmp16=_tmp408;});_tmp16;});}
# 230
return({struct _tuple13*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->f1=(struct _fat_ptr*)_check_null(Cyc_current_source),_tmp17->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp17;});case 1:
# 233 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2:
# 235
 return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);};}
# 239
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp409=({const char*_tmp18="some action didn't return!";_tag_fat(_tmp18,sizeof(char),27U);});_tmp19->f1=_tmp409;});_tmp19;}));}
# 241
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp1A=lexstate;switch((int)_tmp1A){case 0:
# 239 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_tmp1B=_cycalloc(sizeof(*_tmp1B));({struct _fat_ptr _tmp40C=(struct _fat_ptr)({struct _fat_ptr _tmp40B=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp40B,0,(unsigned long)(({
int _tmp40A=Cyc_Lexing_lexeme_end(lexbuf);_tmp40A - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 239
*_tmp1B=_tmp40C;});_tmp1B;});
# 241
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1C=_cycalloc(sizeof(*_tmp1C));({struct Cyc_Set_Set*_tmp40D=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp1C=_tmp40D;});_tmp1C;});
Cyc_token(lexbuf);
return 0;case 1:
# 247
 Cyc_current_source=({struct _fat_ptr*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct _fat_ptr _tmp410=(struct _fat_ptr)({struct _fat_ptr _tmp40F=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp40F,0,(unsigned long)(({
int _tmp40E=Cyc_Lexing_lexeme_end(lexbuf);_tmp40E - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 247
*_tmp1D=_tmp410;});_tmp1D;});
# 249
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct Cyc_Set_Set*_tmp411=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp1E=_tmp411;});_tmp1E;});
Cyc_args(lexbuf);
return 0;case 2:
# 255
 Cyc_current_source=({struct _fat_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));({struct _fat_ptr _tmp412=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp1F=_tmp412;});_tmp1F;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp20=_cycalloc(sizeof(*_tmp20));({struct Cyc_Set_Set*_tmp413=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp20=_tmp413;});_tmp20;});
Cyc_token(lexbuf);
return 0;default:
# 261
((lexbuf->refill_buff))(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);};}
# 264
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp414=({const char*_tmp21="some action didn't return!";_tag_fat(_tmp21,sizeof(char),27U);});_tmp22->f1=_tmp414;});_tmp22;}));}
# 266
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp23=lexstate;switch((int)_tmp23){case 0:  {
# 264 "buildlib.cyl"
struct _fat_ptr*_tmp24=({struct _fat_ptr*_tmp26=_cycalloc(sizeof(*_tmp26));({struct _fat_ptr _tmp417=(struct _fat_ptr)({struct _fat_ptr _tmp416=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp416,0,(unsigned long)(({
int _tmp415=Cyc_Lexing_lexeme_end(lexbuf);_tmp415 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 264
*_tmp26=_tmp417;});_tmp26;});struct _fat_ptr*a=_tmp24;
# 266
Cyc_current_args=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=a,_tmp25->tl=Cyc_current_args;_tmp25;});
return Cyc_args(lexbuf);}case 1:  {
# 270
struct _fat_ptr*_tmp27=({struct _fat_ptr*_tmp29=_cycalloc(sizeof(*_tmp29));({struct _fat_ptr _tmp41A=(struct _fat_ptr)({struct _fat_ptr _tmp419=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp419,0,(unsigned long)(({
int _tmp418=Cyc_Lexing_lexeme_end(lexbuf);_tmp418 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 270
*_tmp29=_tmp41A;});_tmp29;});struct _fat_ptr*a=_tmp27;
# 272
Cyc_current_args=({struct Cyc_List_List*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->hd=a,_tmp28->tl=Cyc_current_args;_tmp28;});
return Cyc_args(lexbuf);}case 2:  {
# 276
struct _fat_ptr*_tmp2A=({struct _fat_ptr*_tmp2C=_cycalloc(sizeof(*_tmp2C));({struct _fat_ptr _tmp41D=(struct _fat_ptr)({struct _fat_ptr _tmp41C=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp41C,0,(unsigned long)(({
int _tmp41B=Cyc_Lexing_lexeme_end(lexbuf);_tmp41B - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 276
*_tmp2C=_tmp41D;});_tmp2C;});struct _fat_ptr*a=_tmp2A;
# 278
Cyc_current_args=({struct Cyc_List_List*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->hd=a,_tmp2B->tl=Cyc_current_args;_tmp2B;});
return Cyc_token(lexbuf);}default:
# 281
((lexbuf->refill_buff))(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);};}
# 284
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp41E=({const char*_tmp2D="some action didn't return!";_tag_fat(_tmp2D,sizeof(char),27U);});_tmp2E->f1=_tmp41E;});_tmp2E;}));}
# 286
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp2F=lexstate;switch((int)_tmp2F){case 0:
# 285 "buildlib.cyl"
 Cyc_add_target(({struct _fat_ptr*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr _tmp41F=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp30=_tmp41F;});_tmp30;}));return Cyc_token(lexbuf);case 1:
# 288 "buildlib.cyl"
 return 0;case 2:
# 291 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3:
# 294 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4:
# 297 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5:
# 299
 return Cyc_token(lexbuf);case 6:
# 301
 return Cyc_token(lexbuf);case 7:
# 303
 return Cyc_token(lexbuf);case 8:
# 306 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9:
# 309 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10:
# 312 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11:
# 314
 return Cyc_token(lexbuf);case 12:
# 316
 return Cyc_token(lexbuf);case 13:
# 318
 return Cyc_token(lexbuf);case 14:
# 320
 return Cyc_token(lexbuf);case 15:
# 322
 return Cyc_token(lexbuf);case 16:
# 324
 return Cyc_token(lexbuf);case 17:
# 326
 return Cyc_token(lexbuf);case 18:
# 328
 return Cyc_token(lexbuf);case 19:
# 330
 return Cyc_token(lexbuf);case 20:
# 332
 return Cyc_token(lexbuf);case 21:
# 334
 return Cyc_token(lexbuf);case 22:
# 336
 return Cyc_token(lexbuf);case 23:
# 338
 return Cyc_token(lexbuf);case 24:
# 341 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25:
# 343
 return Cyc_token(lexbuf);case 26:
# 345
 return Cyc_token(lexbuf);case 27:
# 347
 return Cyc_token(lexbuf);case 28:
# 349
 return Cyc_token(lexbuf);case 29:
# 351
 return Cyc_token(lexbuf);case 30:
# 353
 return Cyc_token(lexbuf);case 31:
# 355
 return Cyc_token(lexbuf);case 32:
# 357
 return Cyc_token(lexbuf);case 33:
# 359
 return Cyc_token(lexbuf);case 34:
# 361
 return Cyc_token(lexbuf);case 35:
# 363
 return Cyc_token(lexbuf);case 36:
# 365
 return Cyc_token(lexbuf);case 37:
# 367
 return Cyc_token(lexbuf);case 38:
# 369
 return Cyc_token(lexbuf);case 39:
# 371
 return Cyc_token(lexbuf);case 40:
# 373
 return Cyc_token(lexbuf);case 41:
# 375
 return Cyc_token(lexbuf);case 42:
# 377
 return Cyc_token(lexbuf);case 43:
# 379
 return Cyc_token(lexbuf);case 44:
# 381
 return Cyc_token(lexbuf);case 45:
# 383
 return Cyc_token(lexbuf);case 46:
# 385
 return Cyc_token(lexbuf);case 47:
# 387
 return Cyc_token(lexbuf);case 48:
# 390 "buildlib.cyl"
 return Cyc_token(lexbuf);default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);};}
# 394
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp420=({const char*_tmp31="some action didn't return!";_tag_fat(_tmp31,sizeof(char),27U);});_tmp32->f1=_tmp420;});_tmp32;}));}
# 396
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp33=lexstate;switch((int)_tmp33){case 0:
# 395 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1:
# 396 "buildlib.cyl"
 return 0;case 2:
# 397 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3:
# 398 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4:
# 401 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5:
# 404 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6:
# 406
 return Cyc_string(lexbuf);case 7:
# 407 "buildlib.cyl"
 return 0;case 8:
# 408 "buildlib.cyl"
 return 0;case 9:
# 409 "buildlib.cyl"
 return Cyc_string(lexbuf);default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);};}
# 413
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp421=({const char*_tmp34="some action didn't return!";_tag_fat(_tmp34,sizeof(char),27U);});_tmp35->f1=_tmp421;});_tmp35;}));}
# 415
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp36=lexstate;switch((int)_tmp36){case 0:
# 418 "buildlib.cyl"
 return 0;case 1:
# 420
 Cyc_fputc(34,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2:
# 427 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp37=0U;({struct _fat_ptr _tmp422=({const char*_tmp38="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp38,sizeof(char),44U);});Cyc_log(_tmp422,_tag_fat(_tmp37,sizeof(void*),0));});});
return 1;case 3:
# 433 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp39=0U;({struct _fat_ptr _tmp423=({const char*_tmp3A="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp3A,sizeof(char),44U);});Cyc_log(_tmp423,_tag_fat(_tmp39,sizeof(void*),0));});});
return 1;case 4:
# 439 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3B=0U;({struct _fat_ptr _tmp424=({const char*_tmp3C="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp3C,sizeof(char),44U);});Cyc_log(_tmp424,_tag_fat(_tmp3B,sizeof(void*),0));});});
return 1;case 5:
# 445 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3D=0U;({struct _fat_ptr _tmp425=({const char*_tmp3E="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp3E,sizeof(char),44U);});Cyc_log(_tmp425,_tag_fat(_tmp3D,sizeof(void*),0));});});
return 1;case 6:
# 449
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3F=0U;({struct _fat_ptr _tmp426=({const char*_tmp40="Warning: use of region sidestepped\n";_tag_fat(_tmp40,sizeof(char),36U);});Cyc_log(_tmp426,_tag_fat(_tmp3F,sizeof(void*),0));});});
return 1;case 7:
# 453
({void*_tmp41=0U;({struct _fat_ptr _tmp427=({const char*_tmp42="Warning: use of __extension__ deleted\n";_tag_fat(_tmp42,sizeof(char),39U);});Cyc_log(_tmp427,_tag_fat(_tmp41,sizeof(void*),0));});});
return 1;case 8:
# 458 "buildlib.cyl"
({void*_tmp43=0U;({struct _fat_ptr _tmp428=({const char*_tmp44="Warning: use of mode HI deleted\n";_tag_fat(_tmp44,sizeof(char),33U);});Cyc_log(_tmp428,_tag_fat(_tmp43,sizeof(void*),0));});});
return 1;case 9:
# 461
({void*_tmp45=0U;({struct _fat_ptr _tmp429=({const char*_tmp46="Warning: use of mode SI deleted\n";_tag_fat(_tmp46,sizeof(char),33U);});Cyc_log(_tmp429,_tag_fat(_tmp45,sizeof(void*),0));});});
return 1;case 10:
# 464
({void*_tmp47=0U;({struct _fat_ptr _tmp42A=({const char*_tmp48="Warning: use of mode QI deleted\n";_tag_fat(_tmp48,sizeof(char),33U);});Cyc_log(_tmp42A,_tag_fat(_tmp47,sizeof(void*),0));});});
return 1;case 11:
# 467
({void*_tmp49=0U;({struct _fat_ptr _tmp42B=({const char*_tmp4A="Warning: use of mode DI deleted\n";_tag_fat(_tmp4A,sizeof(char),33U);});Cyc_log(_tmp42B,_tag_fat(_tmp49,sizeof(void*),0));});});
return 1;case 12:
# 470
({void*_tmp4B=0U;({struct _fat_ptr _tmp42C=({const char*_tmp4C="Warning: use of mode DI deleted\n";_tag_fat(_tmp4C,sizeof(char),33U);});Cyc_log(_tmp42C,_tag_fat(_tmp4B,sizeof(void*),0));});});
return 1;case 13:
# 473
({void*_tmp4D=0U;({struct _fat_ptr _tmp42D=({const char*_tmp4E="Warning: use of mode word deleted\n";_tag_fat(_tmp4E,sizeof(char),35U);});Cyc_log(_tmp42D,_tag_fat(_tmp4D,sizeof(void*),0));});});
return 1;case 14:
# 476
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15:
# 478
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16:
# 480
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17:
# 482
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18:
# 487 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19:
# 489
 return 1;case 20:
# 491
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp4F=0U;({struct _fat_ptr _tmp42E=({const char*_tmp50="Warning: replacing use of __asm__ with 0\n";_tag_fat(_tmp50,sizeof(char),42U);});Cyc_log(_tmp42E,_tag_fat(_tmp4F,sizeof(void*),0));});});
return 1;case 21:
# 497
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp51=0U;({struct _fat_ptr _tmp42F=({const char*_tmp52="Warning: replacing use of __asm with 0\n";_tag_fat(_tmp52,sizeof(char),40U);});Cyc_log(_tmp42F,_tag_fat(_tmp51,sizeof(void*),0));});});
return 1;case 22:
# 503
({int _tmp430=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp430,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);};}
# 507
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp431=({const char*_tmp53="some action didn't return!";_tag_fat(_tmp53,sizeof(char),27U);});_tmp54->f1=_tmp431;});_tmp54;}));}
# 509
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp55=lexstate;switch((int)_tmp55){case 0:
# 507 "buildlib.cyl"
 return 0;case 1:
# 509
 Cyc_fputc(34,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2:
# 511
({void*_tmp56=0U;({struct _fat_ptr _tmp432=({const char*_tmp57="Warning: unclosed string\n";_tag_fat(_tmp57,sizeof(char),26U);});Cyc_log(_tmp432,_tag_fat(_tmp56,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp5A=({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0,({struct _fat_ptr _tmp433=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp398.f1=_tmp433;});_tmp398;});void*_tmp58[1];_tmp58[0]=& _tmp5A;({struct Cyc___cycFILE*_tmp435=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp434=({const char*_tmp59="%s";_tag_fat(_tmp59,sizeof(char),3U);});Cyc_fprintf(_tmp435,_tmp434,_tag_fat(_tmp58,sizeof(void*),1));});});return 1;case 3:
# 514
({struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0,({struct _fat_ptr _tmp436=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp399.f1=_tmp436;});_tmp399;});void*_tmp5B[1];_tmp5B[0]=& _tmp5D;({struct Cyc___cycFILE*_tmp438=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp437=({const char*_tmp5C="%s";_tag_fat(_tmp5C,sizeof(char),3U);});Cyc_fprintf(_tmp438,_tmp437,_tag_fat(_tmp5B,sizeof(void*),1));});});return 1;case 4:
# 516
({struct Cyc_String_pa_PrintArg_struct _tmp60=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0,({struct _fat_ptr _tmp439=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39A.f1=_tmp439;});_tmp39A;});void*_tmp5E[1];_tmp5E[0]=& _tmp60;({struct Cyc___cycFILE*_tmp43B=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp43A=({const char*_tmp5F="%s";_tag_fat(_tmp5F,sizeof(char),3U);});Cyc_fprintf(_tmp43B,_tmp43A,_tag_fat(_tmp5E,sizeof(void*),1));});});return 1;case 5:
# 518
({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp39B;_tmp39B.tag=0,({struct _fat_ptr _tmp43C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39B.f1=_tmp43C;});_tmp39B;});void*_tmp61[1];_tmp61[0]=& _tmp63;({struct Cyc___cycFILE*_tmp43E=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp43D=({const char*_tmp62="%s";_tag_fat(_tmp62,sizeof(char),3U);});Cyc_fprintf(_tmp43E,_tmp43D,_tag_fat(_tmp61,sizeof(void*),1));});});return 1;case 6:
# 520
({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0,({struct _fat_ptr _tmp43F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39C.f1=_tmp43F;});_tmp39C;});void*_tmp64[1];_tmp64[0]=& _tmp66;({struct Cyc___cycFILE*_tmp441=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp440=({const char*_tmp65="%s";_tag_fat(_tmp65,sizeof(char),3U);});Cyc_fprintf(_tmp441,_tmp440,_tag_fat(_tmp64,sizeof(void*),1));});});return 1;case 7:
# 522
({struct Cyc_String_pa_PrintArg_struct _tmp69=({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0,({struct _fat_ptr _tmp442=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39D.f1=_tmp442;});_tmp39D;});void*_tmp67[1];_tmp67[0]=& _tmp69;({struct Cyc___cycFILE*_tmp444=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp443=({const char*_tmp68="%s";_tag_fat(_tmp68,sizeof(char),3U);});Cyc_fprintf(_tmp444,_tmp443,_tag_fat(_tmp67,sizeof(void*),1));});});return 1;case 8:
# 524
({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp39E;_tmp39E.tag=0,({struct _fat_ptr _tmp445=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39E.f1=_tmp445;});_tmp39E;});void*_tmp6A[1];_tmp6A[0]=& _tmp6C;({struct Cyc___cycFILE*_tmp447=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp446=({const char*_tmp6B="%s";_tag_fat(_tmp6B,sizeof(char),3U);});Cyc_fprintf(_tmp447,_tmp446,_tag_fat(_tmp6A,sizeof(void*),1));});});return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);};}
# 528
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp448=({const char*_tmp6D="some action didn't return!";_tag_fat(_tmp6D,sizeof(char),27U);});_tmp6E->f1=_tmp448;});_tmp6E;}));}
# 530
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp6F=lexstate;switch((int)_tmp6F){case 0:
# 534 "buildlib.cyl"
 return 0;case 1:
# 536
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2:
# 540
 ++ Cyc_parens_to_match;
return 1;case 3:
# 543
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4:
# 546
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5:
# 549
 return 1;case 6:
# 551
 return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);};}
# 555
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp449=({const char*_tmp70="some action didn't return!";_tag_fat(_tmp70,sizeof(char),27U);});_tmp71->f1=_tmp449;});_tmp71;}));}
# 557
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp72=lexstate;switch((int)_tmp72){case 0:
# 555 "buildlib.cyl"
({void*_tmp73=0U;({struct _fat_ptr _tmp44A=({const char*_tmp74="Warning: unclosed string\n";_tag_fat(_tmp74,sizeof(char),26U);});Cyc_log(_tmp44A,_tag_fat(_tmp73,sizeof(void*),0));});});return 0;case 1:
# 557
 return 0;case 2:
# 559
({void*_tmp75=0U;({struct _fat_ptr _tmp44B=({const char*_tmp76="Warning: unclosed string\n";_tag_fat(_tmp76,sizeof(char),26U);});Cyc_log(_tmp44B,_tag_fat(_tmp75,sizeof(void*),0));});});return 1;case 3:
# 561
 return 1;case 4:
# 563
 return 1;case 5:
# 565
 return 1;case 6:
# 567
 return 1;case 7:
# 569
 return 1;case 8:
# 571
 return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);};}
# 575
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp44C=({const char*_tmp77="some action didn't return!";_tag_fat(_tmp77,sizeof(char),27U);});_tmp78->f1=_tmp44C;});_tmp78;}));}
# 577
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp79=lexstate;switch((int)_tmp79){case 0:
# 575 "buildlib.cyl"
({void*_tmp7A=0U;({struct _fat_ptr _tmp44D=({const char*_tmp7B="Warning: unclosed comment\n";_tag_fat(_tmp7B,sizeof(char),27U);});Cyc_log(_tmp44D,_tag_fat(_tmp7A,sizeof(void*),0));});});return 0;case 1:
# 577
 return 0;case 2:
# 579
 return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);};}
# 583
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp44E=({const char*_tmp7C="some action didn't return!";_tag_fat(_tmp7C,sizeof(char),27U);});_tmp7D->f1=_tmp44E;});_tmp7D;}));}
# 585
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple14*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7E=lexstate;switch((int)_tmp7E){case 0:
# 587 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp7F="#define ";_tag_fat(_tmp7F,sizeof(char),9U);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple14*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->f1=Cyc_current_line,_tmp80->f2=(struct _fat_ptr*)_check_null(Cyc_current_source);_tmp80;});case 1:
# 591
 return Cyc_suck_line(lexbuf);case 2:
# 593
 return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);};}
# 597
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp44F=({const char*_tmp81="some action didn't return!";_tag_fat(_tmp81,sizeof(char),27U);});_tmp82->f1=_tmp44F;});_tmp82;}));}
# 599
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp83=lexstate;if(_tmp83 == 0){
# 597 "buildlib.cyl"
Cyc_current_source=({struct _fat_ptr*_tmp84=_cycalloc(sizeof(*_tmp84));({struct _fat_ptr _tmp450=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp84=_tmp450;});_tmp84;});
Cyc_current_line=(struct _fat_ptr)Cyc_strconcat((struct _fat_ptr)Cyc_current_line,(struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{
# 601
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);};}
# 604
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp451=({const char*_tmp85="some action didn't return!";_tag_fat(_tmp85,sizeof(char),27U);});_tmp86->f1=_tmp451;});_tmp86;}));}
# 606
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp87=lexstate;if(_tmp87 == 0){
# 604 "buildlib.cyl"
Cyc_current_line=(struct _fat_ptr)({struct _fat_ptr _tmp452=(struct _fat_ptr)Cyc_current_line;Cyc_strconcat(_tmp452,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});return 0;}else{
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);};}
# 608
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp453=({const char*_tmp88="some action didn't return!";_tag_fat(_tmp88,sizeof(char),27U);});_tmp89->f1=_tmp453;});_tmp89;}));}
# 610
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple17*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp8A=lexstate;switch((int)_tmp8A){case 0:
# 611 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1:
# 613
 Cyc_current_headerfile=(struct _fat_ptr)({
struct _fat_ptr _tmp455=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp455,0,(unsigned long)(({
int _tmp454=Cyc_Lexing_lexeme_end(lexbuf);_tmp454 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
Cyc_current_cpp=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(Cyc_current_hstubs);
Cyc_current_cstubs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(Cyc_current_cstubs);
Cyc_current_cycstubs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(Cyc_current_cycstubs);
Cyc_current_cpp=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(Cyc_current_cpp);
return({struct _tuple17*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->f1=Cyc_current_headerfile,_tmp8B->f2=Cyc_current_symbols,_tmp8B->f3=Cyc_current_user_defs,_tmp8B->f4=Cyc_current_omit_symbols,_tmp8B->f5=Cyc_current_hstubs,_tmp8B->f6=Cyc_current_cstubs,_tmp8B->f7=Cyc_current_cycstubs,_tmp8B->f8=Cyc_current_cpp;_tmp8B;});case 2:
# 638
 return Cyc_spec(lexbuf);case 3:
# 640
 return 0;case 4:
# 642
({struct Cyc_Int_pa_PrintArg_struct _tmp8E=({struct Cyc_Int_pa_PrintArg_struct _tmp39F;_tmp39F.tag=1,({
# 644
unsigned long _tmp456=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp39F.f1=_tmp456;});_tmp39F;});void*_tmp8C[1];_tmp8C[0]=& _tmp8E;({struct Cyc___cycFILE*_tmp458=Cyc_stderr;struct _fat_ptr _tmp457=({const char*_tmp8D="Error in .cys file: expected header file name, found '%c' instead\n";_tag_fat(_tmp8D,sizeof(char),67U);});Cyc_fprintf(_tmp458,_tmp457,_tag_fat(_tmp8C,sizeof(void*),1));});});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);};}
# 649
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp459=({const char*_tmp8F="some action didn't return!";_tag_fat(_tmp8F,sizeof(char),27U);});_tmp90->f1=_tmp459;});_tmp90;}));}
# 651
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp91=lexstate;switch((int)_tmp91){case 0:
# 649 "buildlib.cyl"
 return 0;case 1:
# 651
 return 0;case 2:
# 653
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3:
# 658
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp92=Cyc_current_user_defs;struct Cyc_List_List*tmp_user_defs=_tmp92;
while(Cyc_snarfsymbols(lexbuf)){;}
if(tmp_user_defs != Cyc_current_user_defs){
({void*_tmp93=0U;({struct Cyc___cycFILE*_tmp45B=Cyc_stderr;struct _fat_ptr _tmp45A=({const char*_tmp94="Error in .cys file: got optional definition in omitsymbols\n";_tag_fat(_tmp94,sizeof(char),60U);});Cyc_fprintf(_tmp45B,_tmp45A,_tag_fat(_tmp93,sizeof(void*),0));});});
# 664
return 0;}
# 666
Cyc_current_omit_symbols=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;}case 4:
# 669
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp96=_cycalloc(sizeof(*_tmp96));({struct _fat_ptr _tmp45D=(struct _fat_ptr)_tag_fat(0,0,0);_tmp96->f1=_tmp45D;}),({
struct _fat_ptr _tmp45C=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp96->f2=_tmp45C;});_tmp96;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->hd=x,_tmp95->tl=Cyc_current_hstubs;_tmp95;});
return 1;}case 5:  {
# 677
struct _fat_ptr _tmp97=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmp97;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmp98="hstub";_tag_fat(_tmp98,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmp99=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmp99;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp45E=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9B->f2=_tmp45E;});_tmp9B;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->hd=x,_tmp9A->tl=Cyc_current_hstubs;_tmp9A;});
return 1;}}}}case 6:
# 691
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct _fat_ptr _tmp460=(struct _fat_ptr)_tag_fat(0,0,0);_tmp9D->f1=_tmp460;}),({
struct _fat_ptr _tmp45F=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9D->f2=_tmp45F;});_tmp9D;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=x,_tmp9C->tl=Cyc_current_cstubs;_tmp9C;});
return 1;}case 7:  {
# 699
struct _fat_ptr _tmp9E=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmp9E;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmp9F="cstub";_tag_fat(_tmp9F,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmpA0=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmpA0;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp461=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA2->f2=_tmp461;});_tmpA2;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->hd=x,_tmpA1->tl=Cyc_current_cstubs;_tmpA1;});
return 1;}}}}case 8:
# 713
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmpA4=_cycalloc(sizeof(*_tmpA4));({struct _fat_ptr _tmp463=(struct _fat_ptr)_tag_fat(0,0,0);_tmpA4->f1=_tmp463;}),({
struct _fat_ptr _tmp462=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA4->f2=_tmp462;});_tmpA4;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=x,_tmpA3->tl=Cyc_current_cycstubs;_tmpA3;});
return 1;}case 9:  {
# 721
struct _fat_ptr _tmpA5=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpA5;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmpA6="cycstub";_tag_fat(_tmpA6,sizeof(char),8U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmpA7=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmpA7;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp464=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA9->f2=_tmp464;});_tmpA9;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->hd=x,_tmpA8->tl=Cyc_current_cycstubs;_tmpA8;});
return 1;}}}}case 10:
# 735
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _fat_ptr*x=({struct _fat_ptr*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct _fat_ptr _tmp465=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpAB=_tmp465;});_tmpAB;});
Cyc_current_cpp=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->hd=x,_tmpAA->tl=Cyc_current_cpp;_tmpAA;});
return 1;}case 11:
# 742
 return 1;case 12:
# 744
 return 1;case 13:
# 746
({struct Cyc_Int_pa_PrintArg_struct _tmpAE=({struct Cyc_Int_pa_PrintArg_struct _tmp3A0;_tmp3A0.tag=1,({
# 748
unsigned long _tmp466=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3A0.f1=_tmp466;});_tmp3A0;});void*_tmpAC[1];_tmpAC[0]=& _tmpAE;({struct Cyc___cycFILE*_tmp468=Cyc_stderr;struct _fat_ptr _tmp467=({const char*_tmpAD="Error in .cys file: expected command, found '%c' instead\n";_tag_fat(_tmpAD,sizeof(char),58U);});Cyc_fprintf(_tmp468,_tmp467,_tag_fat(_tmpAC,sizeof(void*),1));});});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);};}
# 753
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp469=({const char*_tmpAF="some action didn't return!";_tag_fat(_tmpAF,sizeof(char),27U);});_tmpB0->f1=_tmp469;});_tmpB0;}));}
# 755
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB1=lexstate;switch((int)_tmpB1){case 0:
# 759 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct _fat_ptr*_tmp46B=({struct _fat_ptr*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct _fat_ptr _tmp46A=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpB2=_tmp46A;});_tmpB2;});_tmpB3->hd=_tmp46B;}),_tmpB3->tl=Cyc_snarfed_symbols;_tmpB3;});
return 1;case 1:  {
# 762
struct _fat_ptr _tmpB4=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpB4;
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}
# 771
Cyc_rename_current_symbol=0;{
struct _tuple16*user_def=({struct _tuple16*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct _fat_ptr*_tmp46E=({struct _fat_ptr*_tmpB8=_cycalloc(sizeof(*_tmpB8));*_tmpB8=Cyc_current_symbol;_tmpB8;});_tmpBA->f1=_tmp46E;}),({
struct _fat_ptr*_tmp46D=({struct _fat_ptr*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct _fat_ptr _tmp46C=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpB9=_tmp46C;});_tmpB9;});_tmpBA->f2=_tmp46D;});_tmpBA;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct _fat_ptr*_tmp46F=({struct _fat_ptr*_tmpB5=_cycalloc(sizeof(*_tmpB5));*_tmpB5=(struct _fat_ptr)Cyc_current_symbol;_tmpB5;});_tmpB6->hd=_tmp46F;}),_tmpB6->tl=Cyc_snarfed_symbols;_tmpB6;});
Cyc_current_user_defs=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->hd=user_def,_tmpB7->tl=Cyc_current_user_defs;_tmpB7;});
return 1;}}case 2:
# 778
 return 1;case 3:
# 780
 return 0;case 4:
# 782
({void*_tmpBB=0U;({struct Cyc___cycFILE*_tmp471=Cyc_stderr;struct _fat_ptr _tmp470=({const char*_tmpBC="Error in .cys file: unexpected end-of-file\n";_tag_fat(_tmpBC,sizeof(char),44U);});Cyc_fprintf(_tmp471,_tmp470,_tag_fat(_tmpBB,sizeof(void*),0));});});
# 784
return 0;case 5:
# 786
({struct Cyc_Int_pa_PrintArg_struct _tmpBF=({struct Cyc_Int_pa_PrintArg_struct _tmp3A1;_tmp3A1.tag=1,({
# 788
unsigned long _tmp472=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3A1.f1=_tmp472;});_tmp3A1;});void*_tmpBD[1];_tmpBD[0]=& _tmpBF;({struct Cyc___cycFILE*_tmp474=Cyc_stderr;struct _fat_ptr _tmp473=({const char*_tmpBE="Error in .cys file: expected symbol, found '%c' instead\n";_tag_fat(_tmpBE,sizeof(char),57U);});Cyc_fprintf(_tmp474,_tmp473,_tag_fat(_tmpBD,sizeof(void*),1));});});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);};}
# 793
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp475=({const char*_tmpC0="some action didn't return!";_tag_fat(_tmpC0,sizeof(char),27U);});_tmpC1->f1=_tmp475;});_tmpC1;}));}
# 795
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC2=lexstate;switch((int)_tmpC2){case 0:
# 799 "buildlib.cyl"
({void*_tmpC3=0U;({struct _fat_ptr _tmp476=({const char*_tmpC4="Warning: unclosed brace\n";_tag_fat(_tmpC4,sizeof(char),25U);});Cyc_log(_tmp476,_tag_fat(_tmpC3,sizeof(void*),0));});});return 0;case 1:
# 801
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2:
# 806
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3:
# 810
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4:
# 814
({struct Cyc_Buffer_t*_tmp477=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp477,({const char*_tmpC5="/*";_tag_fat(_tmpC5,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5:
# 818
({struct Cyc_Buffer_t*_tmp478=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp478,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:  {
# 821
struct _fat_ptr _tmpC6=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr symbol=_tmpC6;
if(Cyc_rename_current_symbol && !Cyc_strcmp((struct _fat_ptr)symbol,(struct _fat_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp479=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp479,*Cyc_add_user_prefix(({struct _fat_ptr*_tmpC7=_cycalloc(sizeof(*_tmpC7));*_tmpC7=symbol;_tmpC7;})));});else{
# 825
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),symbol);}
return 1;}case 7:
# 828
({struct Cyc_Buffer_t*_tmp47A=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp47A,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);};}
# 833
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp47B=({const char*_tmpC8="some action didn't return!";_tag_fat(_tmpC8,sizeof(char),27U);});_tmpC9->f1=_tmp47B;});_tmpC9;}));}
# 835
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpCA=lexstate;switch((int)_tmpCA){case 0:
# 833 "buildlib.cyl"
({void*_tmpCB=0U;({struct _fat_ptr _tmp47C=({const char*_tmpCC="Warning: unclosed string\n";_tag_fat(_tmpCC,sizeof(char),26U);});Cyc_log(_tmp47C,_tag_fat(_tmpCB,sizeof(void*),0));});});return 0;case 1:
# 835
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2:
# 837
({void*_tmpCD=0U;({struct _fat_ptr _tmp47D=({const char*_tmpCE="Warning: unclosed string\n";_tag_fat(_tmpCE,sizeof(char),26U);});Cyc_log(_tmp47D,_tag_fat(_tmpCD,sizeof(void*),0));});});
({struct Cyc_Buffer_t*_tmp47E=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp47E,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3:
# 841
({struct Cyc_Buffer_t*_tmp47F=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp47F,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4:
# 844
({struct Cyc_Buffer_t*_tmp480=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp480,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5:
# 847
({struct Cyc_Buffer_t*_tmp481=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp481,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:
# 850
({struct Cyc_Buffer_t*_tmp482=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp482,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7:
# 853
({struct Cyc_Buffer_t*_tmp483=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp483,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8:
# 856
({struct Cyc_Buffer_t*_tmp484=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp484,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);};}
# 861
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp485=({const char*_tmpCF="some action didn't return!";_tag_fat(_tmpCF,sizeof(char),27U);});_tmpD0->f1=_tmp485;});_tmpD0;}));}
# 863
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpD1=lexstate;switch((int)_tmpD1){case 0:
# 861 "buildlib.cyl"
({void*_tmpD2=0U;({struct _fat_ptr _tmp486=({const char*_tmpD3="Warning: unclosed comment\n";_tag_fat(_tmpD3,sizeof(char),27U);});Cyc_log(_tmp486,_tag_fat(_tmpD2,sizeof(void*),0));});});return 0;case 1:
# 863
({struct Cyc_Buffer_t*_tmp487=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp487,({const char*_tmpD4="*/";_tag_fat(_tmpD4,sizeof(char),3U);}));});return 0;case 2:
# 865
({struct Cyc_Buffer_t*_tmp488=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp488,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);};}
# 870
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp489=({const char*_tmpD5="some action didn't return!";_tag_fat(_tmpD5,sizeof(char),27U);});_tmpD6->f1=_tmp489;});_tmpD6;}));}
# 872
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 878 "buildlib.cyl"
void Cyc_scan_type(void*,struct Cyc_Hashtable_Table*);struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpD7=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_stmttmp1=_tmpD7;void*_tmpD8=_stmttmp1;int _tmpDB;void*_tmpDC;void*_tmpDA;void*_tmpD9;switch(*((int*)_tmpD8)){case 1: _tmpD9=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;{void*b=_tmpD9;
# 882
struct _fat_ptr*_tmpDD=(*Cyc_Absyn_binding2qvar(b)).f2;struct _fat_ptr*v=_tmpDD;
Cyc_add_target(v);
return;}case 3: _tmpD9=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{struct Cyc_List_List*x=_tmpD9;
# 886
for(1;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 889
return;}case 23: _tmpD9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_Absyn_Exp*e2=_tmpDA;
# 891
_tmpD9=e1;_tmpDA=e2;goto _LL8;}case 9: _tmpD9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_Absyn_Exp*e2=_tmpDA;
# 893
_tmpD9=e1;_tmpDA=e2;goto _LLA;}case 4: _tmpD9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_Absyn_Exp*e2=_tmpDA;
# 895
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 41: _tmpD9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;{struct Cyc_Absyn_Exp*e1=_tmpD9;
_tmpD9=e1;goto _LLE;}case 20: _tmpD9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LLE: {struct Cyc_Absyn_Exp*e1=_tmpD9;
# 900
_tmpD9=e1;goto _LL10;}case 18: _tmpD9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpD9;
# 902
_tmpD9=e1;goto _LL12;}case 15: _tmpD9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpD9;
# 904
_tmpD9=e1;goto _LL14;}case 5: _tmpD9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpD9;
# 906
Cyc_scan_exp(e1,dep);
return;}case 6: _tmpD9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_tmpDC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpD8)->f3;{struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_Absyn_Exp*e2=_tmpDA;struct Cyc_Absyn_Exp*e3=_tmpDC;
# 909
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
Cyc_scan_exp(e3,dep);
return;}case 7: _tmpD9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_Absyn_Exp*e2=_tmpDA;
_tmpD9=e1;_tmpDA=e2;goto _LL1A;}case 8: _tmpD9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_Absyn_Exp*e2=_tmpDA;
# 915
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 10: _tmpD9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_List_List*x=_tmpDA;
# 919
Cyc_scan_exp(e1,dep);
for(1;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 923
return;}case 14: _tmpD9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{void*t1=_tmpD9;struct Cyc_Absyn_Exp*e1=_tmpDA;
# 925
Cyc_scan_type(t1,dep);
Cyc_scan_exp(e1,dep);
return;}case 34: _tmpDB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).is_calloc;_tmpD9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).rgn;_tmpDA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).elt_type;_tmpDC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpD8)->f1).num_elts;{int iscalloc=_tmpDB;struct Cyc_Absyn_Exp*ropt=_tmpD9;void**topt=_tmpDA;struct Cyc_Absyn_Exp*e=_tmpDC;
# 929
if(ropt != 0)Cyc_scan_exp(ropt,dep);
if(topt != 0)Cyc_scan_type(*topt,dep);
Cyc_scan_exp(e,dep);
return;}case 38: _tmpD9=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;{struct Cyc_Absyn_Exp*e=_tmpD9;
# 934
Cyc_scan_exp(e,dep);return;}case 39: _tmpD9=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;{void*t1=_tmpD9;
_tmpD9=t1;goto _LL26;}case 17: _tmpD9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_LL26: {void*t1=_tmpD9;
# 937
Cyc_scan_type(t1,dep);
return;}case 21: _tmpD9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{struct Cyc_Absyn_Exp*e1=_tmpD9;struct _fat_ptr*fn=_tmpDA;
# 940
_tmpD9=e1;_tmpDA=fn;goto _LL2A;}case 22: _tmpD9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpD9;struct _fat_ptr*fn=_tmpDA;
# 942
Cyc_scan_exp(e1,dep);
Cyc_add_target(fn);
return;}case 19: _tmpD9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD8)->f1;_tmpDA=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{void*t1=_tmpD9;struct Cyc_List_List*f=_tmpDA;
# 946
Cyc_scan_type(t1,dep);
# 948
{void*_tmpDE=(void*)((struct Cyc_List_List*)_check_null(f))->hd;void*_stmttmp2=_tmpDE;void*_tmpDF=_stmttmp2;void*_tmpE0;if(*((int*)_tmpDF)== 0){_tmpE0=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpDF)->f1;{struct _fat_ptr*fn=_tmpE0;
Cyc_add_target(fn);goto _LL57;}}else{
goto _LL57;}_LL57:;}
# 952
return;}case 0:
# 954
 return;case 36: _tmpD9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpD8)->f2;{struct Cyc_List_List*x=_tmpD9;
# 956
for(1;x != 0;x=x->tl){
struct _tuple18*_tmpE1=(struct _tuple18*)x->hd;struct _tuple18*_stmttmp3=_tmpE1;struct _tuple18*_tmpE2=_stmttmp3;void*_tmpE3;_tmpE3=_tmpE2->f2;{struct Cyc_Absyn_Exp*e1=_tmpE3;
Cyc_scan_exp(e1,dep);}}
# 960
return;}case 40:
 return;case 2:
# 963
({void*_tmpE4=0U;({struct Cyc___cycFILE*_tmp48B=Cyc_stderr;struct _fat_ptr _tmp48A=({const char*_tmpE5="Error: unexpected Pragma_e\n";_tag_fat(_tmpE5,sizeof(char),28U);});Cyc_fprintf(_tmp48B,_tmp48A,_tag_fat(_tmpE4,sizeof(void*),0));});});
exit(1);return;case 35:
# 966
({void*_tmpE6=0U;({struct Cyc___cycFILE*_tmp48D=Cyc_stderr;struct _fat_ptr _tmp48C=({const char*_tmpE7="Error: unexpected Swap_e\n";_tag_fat(_tmpE7,sizeof(char),26U);});Cyc_fprintf(_tmp48D,_tmp48C,_tag_fat(_tmpE6,sizeof(void*),0));});});
exit(1);return;case 37:
# 969
({void*_tmpE8=0U;({struct Cyc___cycFILE*_tmp48F=Cyc_stderr;struct _fat_ptr _tmp48E=({const char*_tmpE9="Error: unexpected Stmt_e\n";_tag_fat(_tmpE9,sizeof(char),26U);});Cyc_fprintf(_tmp48F,_tmp48E,_tag_fat(_tmpE8,sizeof(void*),0));});});
exit(1);return;case 42:
# 972
({void*_tmpEA=0U;({struct Cyc___cycFILE*_tmp491=Cyc_stderr;struct _fat_ptr _tmp490=({const char*_tmpEB="Error: unexpected Assert_e\n";_tag_fat(_tmpEB,sizeof(char),28U);});Cyc_fprintf(_tmp491,_tmp490,_tag_fat(_tmpEA,sizeof(void*),0));});});
exit(1);return;case 11:
# 975
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp493=Cyc_stderr;struct _fat_ptr _tmp492=({const char*_tmpED="Error: unexpected Throw_e\n";_tag_fat(_tmpED,sizeof(char),27U);});Cyc_fprintf(_tmp493,_tmp492,_tag_fat(_tmpEC,sizeof(void*),0));});});
exit(1);return;case 12:
# 978
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp495=Cyc_stderr;struct _fat_ptr _tmp494=({const char*_tmpEF="Error: unexpected NoInstantiate_e\n";_tag_fat(_tmpEF,sizeof(char),35U);});Cyc_fprintf(_tmp495,_tmp494,_tag_fat(_tmpEE,sizeof(void*),0));});});
exit(1);return;case 13:
# 981
({void*_tmpF0=0U;({struct Cyc___cycFILE*_tmp497=Cyc_stderr;struct _fat_ptr _tmp496=({const char*_tmpF1="Error: unexpected Instantiate_e\n";_tag_fat(_tmpF1,sizeof(char),33U);});Cyc_fprintf(_tmp497,_tmp496,_tag_fat(_tmpF0,sizeof(void*),0));});});
exit(1);return;case 16:
# 984
({void*_tmpF2=0U;({struct Cyc___cycFILE*_tmp499=Cyc_stderr;struct _fat_ptr _tmp498=({const char*_tmpF3="Error: unexpected New_e\n";_tag_fat(_tmpF3,sizeof(char),25U);});Cyc_fprintf(_tmp499,_tmp498,_tag_fat(_tmpF2,sizeof(void*),0));});});
exit(1);return;case 24:
# 987
({void*_tmpF4=0U;({struct Cyc___cycFILE*_tmp49B=Cyc_stderr;struct _fat_ptr _tmp49A=({const char*_tmpF5="Error: unexpected Tuple_e\n";_tag_fat(_tmpF5,sizeof(char),27U);});Cyc_fprintf(_tmp49B,_tmp49A,_tag_fat(_tmpF4,sizeof(void*),0));});});
exit(1);return;case 25:
# 990
({void*_tmpF6=0U;({struct Cyc___cycFILE*_tmp49D=Cyc_stderr;struct _fat_ptr _tmp49C=({const char*_tmpF7="Error: unexpected CompoundLit_e\n";_tag_fat(_tmpF7,sizeof(char),33U);});Cyc_fprintf(_tmp49D,_tmp49C,_tag_fat(_tmpF6,sizeof(void*),0));});});
exit(1);return;case 26:
# 993
({void*_tmpF8=0U;({struct Cyc___cycFILE*_tmp49F=Cyc_stderr;struct _fat_ptr _tmp49E=({const char*_tmpF9="Error: unexpected Array_e\n";_tag_fat(_tmpF9,sizeof(char),27U);});Cyc_fprintf(_tmp49F,_tmp49E,_tag_fat(_tmpF8,sizeof(void*),0));});});
exit(1);return;case 27:
# 996
({void*_tmpFA=0U;({struct Cyc___cycFILE*_tmp4A1=Cyc_stderr;struct _fat_ptr _tmp4A0=({const char*_tmpFB="Error: unexpected Comprehension_e\n";_tag_fat(_tmpFB,sizeof(char),35U);});Cyc_fprintf(_tmp4A1,_tmp4A0,_tag_fat(_tmpFA,sizeof(void*),0));});});
exit(1);return;case 28:
# 999
({void*_tmpFC=0U;({struct Cyc___cycFILE*_tmp4A3=Cyc_stderr;struct _fat_ptr _tmp4A2=({const char*_tmpFD="Error: unexpected ComprehensionNoinit_e\n";_tag_fat(_tmpFD,sizeof(char),41U);});Cyc_fprintf(_tmp4A3,_tmp4A2,_tag_fat(_tmpFC,sizeof(void*),0));});});
exit(1);return;case 29:
# 1002
({void*_tmpFE=0U;({struct Cyc___cycFILE*_tmp4A5=Cyc_stderr;struct _fat_ptr _tmp4A4=({const char*_tmpFF="Error: unexpected Aggregate_e\n";_tag_fat(_tmpFF,sizeof(char),31U);});Cyc_fprintf(_tmp4A5,_tmp4A4,_tag_fat(_tmpFE,sizeof(void*),0));});});
exit(1);return;case 30:
# 1005
({void*_tmp100=0U;({struct Cyc___cycFILE*_tmp4A7=Cyc_stderr;struct _fat_ptr _tmp4A6=({const char*_tmp101="Error: unexpected AnonStruct_e\n";_tag_fat(_tmp101,sizeof(char),32U);});Cyc_fprintf(_tmp4A7,_tmp4A6,_tag_fat(_tmp100,sizeof(void*),0));});});
exit(1);return;case 31:
# 1008
({void*_tmp102=0U;({struct Cyc___cycFILE*_tmp4A9=Cyc_stderr;struct _fat_ptr _tmp4A8=({const char*_tmp103="Error: unexpected Datatype_e\n";_tag_fat(_tmp103,sizeof(char),30U);});Cyc_fprintf(_tmp4A9,_tmp4A8,_tag_fat(_tmp102,sizeof(void*),0));});});
exit(1);return;case 32:
# 1011
({void*_tmp104=0U;({struct Cyc___cycFILE*_tmp4AB=Cyc_stderr;struct _fat_ptr _tmp4AA=({const char*_tmp105="Error: unexpected Enum_e\n";_tag_fat(_tmp105,sizeof(char),26U);});Cyc_fprintf(_tmp4AB,_tmp4AA,_tag_fat(_tmp104,sizeof(void*),0));});});
exit(1);return;default:
# 1014
({void*_tmp106=0U;({struct Cyc___cycFILE*_tmp4AD=Cyc_stderr;struct _fat_ptr _tmp4AC=({const char*_tmp107="Error: unexpected AnonEnum_e\n";_tag_fat(_tmp107,sizeof(char),30U);});Cyc_fprintf(_tmp4AD,_tmp4AC,_tag_fat(_tmp106,sizeof(void*),0));});});
exit(1);return;};}
# 1019
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned)eo)Cyc_scan_exp(eo,dep);
return;}
# 1024
void Cyc_scan_decl(struct Cyc_Absyn_Decl*,struct Cyc_Hashtable_Table*);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp108=t;struct Cyc_Absyn_FnInfo _tmp10A;void*_tmp10C;struct Cyc_Absyn_PtrInfo _tmp10D;void*_tmp10B;void*_tmp109;switch(*((int*)_tmp108)){case 0: _tmp109=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp108)->f1;_tmp10B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp108)->f2;{void*c=_tmp109;struct Cyc_List_List*ts=_tmp10B;
# 1028
void*_tmp10E=c;void*_tmp10F;union Cyc_Absyn_AggrInfo _tmp110;switch(*((int*)_tmp10E)){case 0:
 goto _LL21;case 1: _LL21:
 goto _LL23;case 17: _LL23:
 goto _LL25;case 2: _LL25:
 goto _LL27;case 16: _LL27:
# 1034
 return;case 20: _tmp110=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp10E)->f1;{union Cyc_Absyn_AggrInfo info=_tmp110;
# 1036
struct _tuple12 _tmp111=Cyc_Absyn_aggr_kinded_name(info);struct _tuple12 _stmttmp4=_tmp111;struct _tuple12 _tmp112=_stmttmp4;void*_tmp113;_tmp113=(_tmp112.f2)->f2;{struct _fat_ptr*v=_tmp113;
_tmp10F=v;goto _LL2B;}}case 15: _tmp10F=(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp10E)->f1)->f2;_LL2B: {struct _fat_ptr*v=_tmp10F;
Cyc_add_target(v);return;}case 18:
 goto _LL2F;case 19: _LL2F: goto _LL31;case 3: _LL31:
 goto _LL33;case 5: _LL33: goto _LL35;case 6: _LL35:
 goto _LL37;case 7: _LL37: goto _LL39;case 8: _LL39:
 goto _LL3B;case 9: _LL3B: goto _LL3D;case 10: _LL3D:
 goto _LL3F;case 4: _LL3F: goto _LL41;case 11: _LL41:
 goto _LL43;case 12: _LL43: goto _LL45;case 13: _LL45:
 goto _LL47;default: _LL47:
({struct Cyc_String_pa_PrintArg_struct _tmp116=({struct Cyc_String_pa_PrintArg_struct _tmp3A2;_tmp3A2.tag=0,({struct _fat_ptr _tmp4AE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp3A2.f1=_tmp4AE;});_tmp3A2;});void*_tmp114[1];_tmp114[0]=& _tmp116;({struct Cyc___cycFILE*_tmp4B0=Cyc_stderr;struct _fat_ptr _tmp4AF=({const char*_tmp115="Error: unexpected %s\n";_tag_fat(_tmp115,sizeof(char),22U);});Cyc_fprintf(_tmp4B0,_tmp4AF,_tag_fat(_tmp114,sizeof(void*),1));});});
exit(1);return;};}case 3: _tmp10D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp108)->f1;{struct Cyc_Absyn_PtrInfo x=_tmp10D;
# 1051
Cyc_scan_type(x.elt_type,dep);
return;}case 4: _tmp109=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp108)->f1).elt_type;_tmp10B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp108)->f1).num_elts;_tmp10C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp108)->f1).zero_term;{void*t=_tmp109;struct Cyc_Absyn_Exp*sz=_tmp10B;void*zt=_tmp10C;
# 1054
Cyc_scan_type(t,dep);
Cyc_scan_exp_opt(sz,dep);
return;}case 11: _tmp109=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp108)->f1;{struct Cyc_Absyn_Exp*e=_tmp109;
# 1058
Cyc_scan_exp(e,dep);
return;}case 5: _tmp10A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp108)->f1;{struct Cyc_Absyn_FnInfo x=_tmp10A;
# 1061
Cyc_scan_type(x.ret_type,dep);
{struct Cyc_List_List*_tmp117=x.args;struct Cyc_List_List*a=_tmp117;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp118=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5=_tmp118;struct _tuple9*_tmp119=_stmttmp5;void*_tmp11A;_tmp11A=_tmp119->f3;{void*argt=_tmp11A;
Cyc_scan_type(argt,dep);}}}
# 1066
if(x.cyc_varargs != 0)
Cyc_scan_type((x.cyc_varargs)->type,dep);
return;}case 7: _tmp109=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp108)->f2;{struct Cyc_List_List*sfs=_tmp109;
# 1070
for(1;sfs != 0;sfs=sfs->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->width,dep);}
# 1074
return;}case 8: _tmp109=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp108)->f1)->f2;{struct _fat_ptr*v=_tmp109;
# 1076
Cyc_add_target(v);
return;}case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp108)->f1)->r)){case 0: _tmp109=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp108)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp109;
# 1080
({struct Cyc_Absyn_Decl*_tmp4B1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=5,_tmp11B->f1=x;_tmp11B;}),0U);Cyc_scan_decl(_tmp4B1,dep);});{
struct _tuple1*_tmp11C=x->name;struct _tuple1*_stmttmp6=_tmp11C;struct _tuple1*_tmp11D=_stmttmp6;void*_tmp11E;_tmp11E=_tmp11D->f2;{struct _fat_ptr*n=_tmp11E;
Cyc_add_target(n);
return;}}}case 1: _tmp109=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp108)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp109;
# 1086
({struct Cyc_Absyn_Decl*_tmp4B2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=7,_tmp11F->f1=x;_tmp11F;}),0U);Cyc_scan_decl(_tmp4B2,dep);});{
struct _tuple1*_tmp120=x->name;struct _tuple1*_stmttmp7=_tmp120;struct _tuple1*_tmp121=_stmttmp7;void*_tmp122;_tmp122=_tmp121->f2;{struct _fat_ptr*n=_tmp122;
Cyc_add_target(n);
return;}}}default: _tmp109=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp108)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp109;
# 1092
({void*_tmp123=0U;({struct Cyc___cycFILE*_tmp4B4=Cyc_stderr;struct _fat_ptr _tmp4B3=({const char*_tmp124="Error: unexpected Datatype declaration\n";_tag_fat(_tmp124,sizeof(char),40U);});Cyc_fprintf(_tmp4B4,_tmp4B3,_tag_fat(_tmp123,sizeof(void*),0));});});
exit(1);return;}}case 1:
# 1095
({void*_tmp125=0U;({struct Cyc___cycFILE*_tmp4B6=Cyc_stderr;struct _fat_ptr _tmp4B5=({const char*_tmp126="Error: unexpected Evar\n";_tag_fat(_tmp126,sizeof(char),24U);});Cyc_fprintf(_tmp4B6,_tmp4B5,_tag_fat(_tmp125,sizeof(void*),0));});});
exit(1);return;case 2:
# 1098
({void*_tmp127=0U;({struct Cyc___cycFILE*_tmp4B8=Cyc_stderr;struct _fat_ptr _tmp4B7=({const char*_tmp128="Error: unexpected VarType\n";_tag_fat(_tmp128,sizeof(char),27U);});Cyc_fprintf(_tmp4B8,_tmp4B7,_tag_fat(_tmp127,sizeof(void*),0));});});
exit(1);return;case 6:
# 1101
({void*_tmp129=0U;({struct Cyc___cycFILE*_tmp4BA=Cyc_stderr;struct _fat_ptr _tmp4B9=({const char*_tmp12A="Error: unexpected TupleType\n";_tag_fat(_tmp12A,sizeof(char),29U);});Cyc_fprintf(_tmp4BA,_tmp4B9,_tag_fat(_tmp129,sizeof(void*),0));});});
exit(1);return;default:
# 1104
({void*_tmp12B=0U;({struct Cyc___cycFILE*_tmp4BC=Cyc_stderr;struct _fat_ptr _tmp4BB=({const char*_tmp12C="Error: unexpected valueof_t\n";_tag_fat(_tmp12C,sizeof(char),29U);});Cyc_fprintf(_tmp4BC,_tmp4BB,_tag_fat(_tmp12B,sizeof(void*),0));});});
exit(1);return;};}
# 1109
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp12D=Cyc_current_targets;struct Cyc_Set_Set**saved_targets=_tmp12D;
struct _fat_ptr*_tmp12E=Cyc_current_source;struct _fat_ptr*saved_source=_tmp12E;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp12F=_cycalloc(sizeof(*_tmp12F));({struct Cyc_Set_Set*_tmp4BD=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp12F=_tmp4BD;});_tmp12F;});
{void*_tmp130=d->r;void*_stmttmp8=_tmp130;void*_tmp131=_stmttmp8;void*_tmp132;switch(*((int*)_tmp131)){case 0: _tmp132=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp131)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp132;
# 1115
struct _tuple1*_tmp133=x->name;struct _tuple1*_stmttmp9=_tmp133;struct _tuple1*_tmp134=_stmttmp9;void*_tmp135;_tmp135=_tmp134->f2;{struct _fat_ptr*v=_tmp135;
Cyc_current_source=v;
Cyc_scan_type(x->type,dep);
Cyc_scan_exp_opt(x->initializer,dep);
goto _LL0;}}case 1: _tmp132=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp131)->f1;{struct Cyc_Absyn_Fndecl*x=_tmp132;
# 1121
struct _tuple1*_tmp136=x->name;struct _tuple1*_stmttmpA=_tmp136;struct _tuple1*_tmp137=_stmttmpA;void*_tmp138;_tmp138=_tmp137->f2;{struct _fat_ptr*v=_tmp138;
Cyc_current_source=v;
Cyc_scan_type((x->i).ret_type,dep);
{struct Cyc_List_List*_tmp139=(x->i).args;struct Cyc_List_List*a=_tmp139;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp13A=(struct _tuple9*)a->hd;struct _tuple9*_stmttmpB=_tmp13A;struct _tuple9*_tmp13B=_stmttmpB;void*_tmp13C;_tmp13C=_tmp13B->f3;{void*t1=_tmp13C;
Cyc_scan_type(t1,dep);}}}
# 1128
if((x->i).cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null((x->i).cyc_varargs))->type,dep);
if(x->is_inline)
({void*_tmp13D=0U;({struct Cyc___cycFILE*_tmp4BF=Cyc_stderr;struct _fat_ptr _tmp4BE=({const char*_tmp13E="Warning: ignoring inline function\n";_tag_fat(_tmp13E,sizeof(char),35U);});Cyc_fprintf(_tmp4BF,_tmp4BE,_tag_fat(_tmp13D,sizeof(void*),0));});});
goto _LL0;}}case 5: _tmp132=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp131)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp132;
# 1134
struct _tuple1*_tmp13F=x->name;struct _tuple1*_stmttmpC=_tmp13F;struct _tuple1*_tmp140=_stmttmpC;void*_tmp141;_tmp141=_tmp140->f2;{struct _fat_ptr*v=_tmp141;
Cyc_current_source=v;
if((unsigned)x->impl){
{struct Cyc_List_List*_tmp142=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp142;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp143=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp143;
Cyc_scan_type(f->type,dep);
Cyc_scan_exp_opt(f->width,dep);}}{
# 1144
struct Cyc_List_List*_tmp144=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp144;for(0;fs != 0;fs=fs->tl){;}}}
# 1148
goto _LL0;}}case 7: _tmp132=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp131)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp132;
# 1150
struct _tuple1*_tmp145=x->name;struct _tuple1*_stmttmpD=_tmp145;struct _tuple1*_tmp146=_stmttmpD;void*_tmp147;_tmp147=_tmp146->f2;{struct _fat_ptr*v=_tmp147;
Cyc_current_source=v;
if((unsigned)x->fields){
{struct Cyc_List_List*_tmp148=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp148;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp149=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp149;
Cyc_scan_exp_opt(f->tag,dep);}}{
# 1159
struct Cyc_List_List*_tmp14A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp14A;for(0;fs != 0;fs=fs->tl){;}}}
# 1163
goto _LL0;}}case 8: _tmp132=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp131)->f1;{struct Cyc_Absyn_Typedefdecl*x=_tmp132;
# 1165
struct _tuple1*_tmp14B=x->name;struct _tuple1*_stmttmpE=_tmp14B;struct _tuple1*_tmp14C=_stmttmpE;void*_tmp14D;_tmp14D=_tmp14C->f2;{struct _fat_ptr*v=_tmp14D;
Cyc_current_source=v;
if((unsigned)x->defn)
Cyc_scan_type((void*)_check_null(x->defn),dep);
goto _LL0;}}case 4:
# 1171
({void*_tmp14E=0U;({struct Cyc___cycFILE*_tmp4C1=Cyc_stderr;struct _fat_ptr _tmp4C0=({const char*_tmp14F="Error: unexpected region declaration";_tag_fat(_tmp14F,sizeof(char),37U);});Cyc_fprintf(_tmp4C1,_tmp4C0,_tag_fat(_tmp14E,sizeof(void*),0));});});
exit(1);case 13:
# 1174
({void*_tmp150=0U;({struct Cyc___cycFILE*_tmp4C3=Cyc_stderr;struct _fat_ptr _tmp4C2=({const char*_tmp151="Error: unexpected __cyclone_port_on__";_tag_fat(_tmp151,sizeof(char),38U);});Cyc_fprintf(_tmp4C3,_tmp4C2,_tag_fat(_tmp150,sizeof(void*),0));});});
exit(1);case 14:
# 1177
({void*_tmp152=0U;({struct Cyc___cycFILE*_tmp4C5=Cyc_stderr;struct _fat_ptr _tmp4C4=({const char*_tmp153="Error: unexpected __cyclone_port_off__";_tag_fat(_tmp153,sizeof(char),39U);});Cyc_fprintf(_tmp4C5,_tmp4C4,_tag_fat(_tmp152,sizeof(void*),0));});});
exit(1);case 15:
# 1180
({void*_tmp154=0U;({struct Cyc___cycFILE*_tmp4C7=Cyc_stderr;struct _fat_ptr _tmp4C6=({const char*_tmp155="Error: unexpected __tempest_on__";_tag_fat(_tmp155,sizeof(char),33U);});Cyc_fprintf(_tmp4C7,_tmp4C6,_tag_fat(_tmp154,sizeof(void*),0));});});
exit(1);case 16:
# 1183
({void*_tmp156=0U;({struct Cyc___cycFILE*_tmp4C9=Cyc_stderr;struct _fat_ptr _tmp4C8=({const char*_tmp157="Error: unexpected __tempest_off__";_tag_fat(_tmp157,sizeof(char),34U);});Cyc_fprintf(_tmp4C9,_tmp4C8,_tag_fat(_tmp156,sizeof(void*),0));});});
exit(1);case 2:
# 1186
({void*_tmp158=0U;({struct Cyc___cycFILE*_tmp4CB=Cyc_stderr;struct _fat_ptr _tmp4CA=({const char*_tmp159="Error: unexpected let declaration\n";_tag_fat(_tmp159,sizeof(char),35U);});Cyc_fprintf(_tmp4CB,_tmp4CA,_tag_fat(_tmp158,sizeof(void*),0));});});
exit(1);case 6:
# 1189
({void*_tmp15A=0U;({struct Cyc___cycFILE*_tmp4CD=Cyc_stderr;struct _fat_ptr _tmp4CC=({const char*_tmp15B="Error: unexpected datatype declaration\n";_tag_fat(_tmp15B,sizeof(char),40U);});Cyc_fprintf(_tmp4CD,_tmp4CC,_tag_fat(_tmp15A,sizeof(void*),0));});});
exit(1);case 3:
# 1192
({void*_tmp15C=0U;({struct Cyc___cycFILE*_tmp4CF=Cyc_stderr;struct _fat_ptr _tmp4CE=({const char*_tmp15D="Error: unexpected let declaration\n";_tag_fat(_tmp15D,sizeof(char),35U);});Cyc_fprintf(_tmp4CF,_tmp4CE,_tag_fat(_tmp15C,sizeof(void*),0));});});
exit(1);case 9:
# 1195
({void*_tmp15E=0U;({struct Cyc___cycFILE*_tmp4D1=Cyc_stderr;struct _fat_ptr _tmp4D0=({const char*_tmp15F="Error: unexpected namespace declaration\n";_tag_fat(_tmp15F,sizeof(char),41U);});Cyc_fprintf(_tmp4D1,_tmp4D0,_tag_fat(_tmp15E,sizeof(void*),0));});});
exit(1);case 10:
# 1198
({void*_tmp160=0U;({struct Cyc___cycFILE*_tmp4D3=Cyc_stderr;struct _fat_ptr _tmp4D2=({const char*_tmp161="Error: unexpected using declaration\n";_tag_fat(_tmp161,sizeof(char),37U);});Cyc_fprintf(_tmp4D3,_tmp4D2,_tag_fat(_tmp160,sizeof(void*),0));});});
exit(1);case 11:
# 1201
({void*_tmp162=0U;({struct Cyc___cycFILE*_tmp4D5=Cyc_stderr;struct _fat_ptr _tmp4D4=({const char*_tmp163="Error: unexpected extern \"C\" declaration\n";_tag_fat(_tmp163,sizeof(char),42U);});Cyc_fprintf(_tmp4D5,_tmp4D4,_tag_fat(_tmp162,sizeof(void*),0));});});
exit(1);default:
# 1204
({void*_tmp164=0U;({struct Cyc___cycFILE*_tmp4D7=Cyc_stderr;struct _fat_ptr _tmp4D6=({const char*_tmp165="Error: unexpected extern \"C include\" declaration\n";_tag_fat(_tmp165,sizeof(char),50U);});Cyc_fprintf(_tmp4D7,_tmp4D6,_tag_fat(_tmp164,sizeof(void*),0));});});
exit(1);}_LL0:;}{
# 1212
struct Cyc_Set_Set*old;
struct _fat_ptr*_tmp166=(struct _fat_ptr*)_check_null(Cyc_current_source);struct _fat_ptr*name=_tmp166;
{struct _handler_cons _tmp167;_push_handler(& _tmp167);{int _tmp169=0;if(setjmp(_tmp167.handler))_tmp169=1;if(!_tmp169){
old=({(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(dep,name);;_pop_handler();}else{void*_tmp168=(void*)Cyc_Core_get_exn_thrown();void*_tmp16A=_tmp168;void*_tmp16B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp16A)->tag == Cyc_Core_Not_found){
# 1217
old=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);goto _LL35;}else{_tmp16B=_tmp16A;{void*exn=_tmp16B;(int)_rethrow(exn);}}_LL35:;}}}{
# 1219
struct Cyc_Set_Set*_tmp16C=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_union_two;})(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);struct Cyc_Set_Set*targets=_tmp16C;
({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;})(dep,name,targets);
# 1222
Cyc_current_targets=saved_targets;
Cyc_current_source=saved_source;}}}
# 1226
struct Cyc_Hashtable_Table*Cyc_new_deps (void){
return({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;})(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1230
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _fat_ptr*x){
struct _handler_cons _tmp16D;_push_handler(& _tmp16D);{int _tmp16F=0;if(setjmp(_tmp16D.handler))_tmp16F=1;if(!_tmp16F){{struct Cyc_Set_Set*_tmp170=({(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(t,x);_npop_handler(0);return _tmp170;};_pop_handler();}else{void*_tmp16E=(void*)Cyc_Core_get_exn_thrown();void*_tmp171=_tmp16E;void*_tmp172;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp171)->tag == Cyc_Core_Not_found)
# 1233
return({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);else{_tmp172=_tmp171;{void*exn=_tmp172;(int)_rethrow(exn);}};}}}
# 1237
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1247 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(curr,(struct _fat_ptr*)init->hd);}{
# 1252
struct Cyc_Set_Set*_tmp173=curr;struct Cyc_Set_Set*delta=_tmp173;
# 1254
struct _fat_ptr*_tmp174=({struct _fat_ptr*_tmp178=_cycalloc(sizeof(*_tmp178));({struct _fat_ptr _tmp4D8=({const char*_tmp177="";_tag_fat(_tmp177,sizeof(char),1U);});*_tmp178=_tmp4D8;});_tmp178;});struct _fat_ptr*sptr=_tmp174;
while(({(int(*)(struct Cyc_Set_Set*))Cyc_Set_cardinality;})(delta)> 0){
struct Cyc_Set_Set*_tmp175=emptyset;struct Cyc_Set_Set*next=_tmp175;
struct Cyc_Iter_Iter _tmp176=({(struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Set_Set*))Cyc_Set_make_iter;})(Cyc_Core_heap_region,delta);struct Cyc_Iter_Iter iter=_tmp176;
while(({(int(*)(struct Cyc_Iter_Iter,struct _fat_ptr**))Cyc_Iter_next;})(iter,& sptr)){
next=({struct Cyc_Set_Set*(*_tmp4DA)(struct Cyc_Set_Set*,struct Cyc_Set_Set*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_union_two;});struct Cyc_Set_Set*_tmp4D9=next;_tmp4DA(_tmp4D9,Cyc_find(t,sptr));});}
delta=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_diff;})(next,curr);
curr=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_union_two;})(curr,delta);}
# 1263
return curr;}}
# 1266
enum Cyc_buildlib_mode{Cyc_NORMAL =0U,Cyc_GATHER =1U,Cyc_GATHERSCRIPT =2U,Cyc_FINISH =3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering (void){
return(int)Cyc_mode == 1 ||(int)Cyc_mode == 2;}
# 1272
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _fat_ptr fmt,struct _fat_ptr ap){
# 1276
if(Cyc_script_file == 0){
({void*_tmp179=0U;({struct Cyc___cycFILE*_tmp4DC=Cyc_stderr;struct _fat_ptr _tmp4DB=({const char*_tmp17A="Internal error: script file is NULL\n";_tag_fat(_tmp17A,sizeof(char),37U);});Cyc_fprintf(_tmp4DC,_tmp4DB,_tag_fat(_tmp179,sizeof(void*),0));});});
exit(1);}
# 1280
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1283
int Cyc_force_directory(struct _fat_ptr d){
if((int)Cyc_mode == 2)
({struct Cyc_String_pa_PrintArg_struct _tmp17D=({struct Cyc_String_pa_PrintArg_struct _tmp3A4;_tmp3A4.tag=0,_tmp3A4.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3A4;});struct Cyc_String_pa_PrintArg_struct _tmp17E=({struct Cyc_String_pa_PrintArg_struct _tmp3A3;_tmp3A3.tag=0,_tmp3A3.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3A3;});void*_tmp17B[2];_tmp17B[0]=& _tmp17D,_tmp17B[1]=& _tmp17E;({struct _fat_ptr _tmp4DD=({const char*_tmp17C="if ! test -e %s; then mkdir %s; fi\n";_tag_fat(_tmp17C,sizeof(char),36U);});Cyc_prscript(_tmp4DD,_tag_fat(_tmp17B,sizeof(void*),2));});});else{
# 1290
int _tmp17F=({void*_tmp183=0U;({const char*_tmp4DE=(const char*)_untag_fat_ptr(d,sizeof(char),1U);Cyc_open(_tmp4DE,0,_tag_fat(_tmp183,sizeof(unsigned short),0));});});int fd=_tmp17F;
if(fd == -1){
if(mkdir((const char*)_check_null(_untag_fat_ptr(d,sizeof(char),1U)),448)== -1){
({struct Cyc_String_pa_PrintArg_struct _tmp182=({struct Cyc_String_pa_PrintArg_struct _tmp3A5;_tmp3A5.tag=0,_tmp3A5.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3A5;});void*_tmp180[1];_tmp180[0]=& _tmp182;({struct Cyc___cycFILE*_tmp4E0=Cyc_stderr;struct _fat_ptr _tmp4DF=({const char*_tmp181="Error: could not create directory %s\n";_tag_fat(_tmp181,sizeof(char),38U);});Cyc_fprintf(_tmp4E0,_tmp4DF,_tag_fat(_tmp180,sizeof(void*),1));});});
return 1;}}else{
# 1297
close(fd);}}
# 1299
return 0;}
# 1302
int Cyc_force_directory_prefixes(struct _fat_ptr file){
# 1306
struct _fat_ptr _tmp184=Cyc_strdup((struct _fat_ptr)file);struct _fat_ptr curr=_tmp184;
# 1308
struct Cyc_List_List*_tmp185=0;struct Cyc_List_List*x=_tmp185;
while(1){
curr=Cyc_Filename_dirname((struct _fat_ptr)curr);
if(Cyc_strlen((struct _fat_ptr)curr)== 0U)break;
x=({struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*_tmp187));({struct _fat_ptr*_tmp4E1=({struct _fat_ptr*_tmp186=_cycalloc(sizeof(*_tmp186));*_tmp186=(struct _fat_ptr)curr;_tmp186;});_tmp187->hd=_tmp4E1;}),_tmp187->tl=x;_tmp187;});}
# 1315
for(1;x != 0;x=x->tl){
if(Cyc_force_directory(*((struct _fat_ptr*)x->hd)))return 1;}
# 1318
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _fat_ptr f1;};
# 1325
static int Cyc_is_other_special(char c){
char _tmp188=c;switch((int)_tmp188){case 92:
 goto _LL4;case 34: _LL4:
 goto _LL6;case 59: _LL6:
 goto _LL8;case 38: _LL8:
 goto _LLA;case 40: _LLA:
 goto _LLC;case 41: _LLC:
 goto _LLE;case 124: _LLE:
 goto _LL10;case 94: _LL10:
 goto _LL12;case 60: _LL12:
 goto _LL14;case 62: _LL14:
 goto _LL16;case 10: _LL16:
# 1340
 goto _LL18;case 9: _LL18:
 return 1;default:
 return 0;};}
# 1346
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmp189=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp189;
# 1350
int _tmp18A=0;int single_quotes=_tmp18A;
int _tmp18B=0;int other_special=_tmp18B;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmp18C=((const char*)s.curr)[i];char c=_tmp18C;
if((int)c == 39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 1359
if(single_quotes == 0 && other_special == 0)
return s;
# 1363
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmp18D[3];({struct _fat_ptr*_tmp4E6=({struct _fat_ptr*_tmp18F=_cycalloc(sizeof(*_tmp18F));({struct _fat_ptr _tmp4E5=({const char*_tmp18E="'";_tag_fat(_tmp18E,sizeof(char),2U);});*_tmp18F=_tmp4E5;});_tmp18F;});_tmp18D[0]=_tmp4E6;}),({struct _fat_ptr*_tmp4E4=({struct _fat_ptr*_tmp190=_cycalloc(sizeof(*_tmp190));*_tmp190=(struct _fat_ptr)s;_tmp190;});_tmp18D[1]=_tmp4E4;}),({struct _fat_ptr*_tmp4E3=({struct _fat_ptr*_tmp192=_cycalloc(sizeof(*_tmp192));({struct _fat_ptr _tmp4E2=({const char*_tmp191="'";_tag_fat(_tmp191,sizeof(char),2U);});*_tmp192=_tmp4E2;});_tmp192;});_tmp18D[2]=_tmp4E3;});({struct Cyc_List_List*(*_tmp4E7)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp4E7(_tag_fat(_tmp18D,sizeof(struct _fat_ptr*),3));});}));{
# 1367
unsigned long _tmp193=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmp193;
struct _fat_ptr s2=({unsigned _tmp19E=(len2 + 1U)+ 1U;char*_tmp19D=_cycalloc_atomic(_check_times(_tmp19E,sizeof(char)));({{unsigned _tmp3A6=len2 + 1U;unsigned i;for(i=0;i < _tmp3A6;++ i){_tmp19D[i]='\000';}_tmp19D[_tmp3A6]=0;}0;});_tag_fat(_tmp19D,sizeof(char),_tmp19E);});
int _tmp194=0;int i=_tmp194;
int _tmp195=0;int j=_tmp195;
for(1;(unsigned long)i < len;++ i){
char _tmp196=((const char*)s.curr)[i];char c=_tmp196;
if((int)c == 39 || Cyc_is_other_special(c))
({struct _fat_ptr _tmp197=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp198=*((char*)_check_fat_subscript(_tmp197,sizeof(char),0U));char _tmp199='\\';if(_get_fat_size(_tmp197,sizeof(char))== 1U &&(_tmp198 == 0 && _tmp199 != 0))_throw_arraybounds();*((char*)_tmp197.curr)=_tmp199;});
({struct _fat_ptr _tmp19A=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp19B=*((char*)_check_fat_subscript(_tmp19A,sizeof(char),0U));char _tmp19C=c;if(_get_fat_size(_tmp19A,sizeof(char))== 1U &&(_tmp19B == 0 && _tmp19C != 0))_throw_arraybounds();*((char*)_tmp19A.curr)=_tmp19C;});}
# 1377
return(struct _fat_ptr)s2;}}
# 1379
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmp19F=_cycalloc(sizeof(*_tmp19F));({struct _fat_ptr _tmp4E8=Cyc_sh_escape_string(*sp);*_tmp19F=_tmp4E8;});_tmp19F;});}
# 1384
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1392
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1397
({struct Cyc_String_pa_PrintArg_struct _tmp1A2=({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0,({
struct _fat_ptr _tmp4E9=(struct _fat_ptr)({const char*_tmp1A3=filename;_tag_fat(_tmp1A3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1A3,1U));});_tmp3A7.f1=_tmp4E9;});_tmp3A7;});void*_tmp1A0[1];_tmp1A0[0]=& _tmp1A2;({struct Cyc___cycFILE*_tmp4EB=Cyc_stderr;struct _fat_ptr _tmp4EA=({const char*_tmp1A1="********************************* %s...\n";_tag_fat(_tmp1A1,sizeof(char),41U);});Cyc_fprintf(_tmp4EB,_tmp4EA,_tag_fat(_tmp1A0,sizeof(void*),1));});});
# 1400
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp1A6=({struct Cyc_String_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=0,({struct _fat_ptr _tmp4EC=(struct _fat_ptr)({const char*_tmp1A7=filename;_tag_fat(_tmp1A7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1A7,1U));});_tmp3A8.f1=_tmp4EC;});_tmp3A8;});void*_tmp1A4[1];_tmp1A4[0]=& _tmp1A6;({struct _fat_ptr _tmp4ED=({const char*_tmp1A5="\n%s:\n";_tag_fat(_tmp1A5,sizeof(char),6U);});Cyc_log(_tmp4ED,_tag_fat(_tmp1A4,sizeof(void*),1));});});{
# 1412 "buildlib.cyl"
struct _fat_ptr _tmp1A8=Cyc_Filename_basename(({const char*_tmp2F0=filename;_tag_fat(_tmp2F0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F0,1U));}));struct _fat_ptr basename=_tmp1A8;
struct _fat_ptr _tmp1A9=Cyc_Filename_dirname(({const char*_tmp2EF=filename;_tag_fat(_tmp2EF,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EF,1U));}));struct _fat_ptr dirname=_tmp1A9;
struct _fat_ptr _tmp1AA=Cyc_Filename_chop_extension((struct _fat_ptr)basename);struct _fat_ptr choppedname=_tmp1AA;
const char*_tmp1AB=(const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp4EE=(struct _fat_ptr)choppedname;Cyc_strconcat(_tmp4EE,({const char*_tmp2EE=".iA";_tag_fat(_tmp2EE,sizeof(char),4U);}));}),sizeof(char),1U));const char*cppinfile=_tmp1AB;
# 1417
const char*_tmp1AC=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _tmp2EA=({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0,_tmp3DC.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DC;});void*_tmp2E8[1];_tmp2E8[0]=& _tmp2EA;({struct _fat_ptr _tmp4F1=({const char*_tmp2E9="%s.iB";_tag_fat(_tmp2E9,sizeof(char),6U);});Cyc_aprintf(_tmp4F1,_tag_fat(_tmp2E8,sizeof(void*),1));});}):({struct _fat_ptr _tmp4F0=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp4F0,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2ED=({struct Cyc_String_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=0,_tmp3DD.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DD;});void*_tmp2EB[1];_tmp2EB[0]=& _tmp2ED;({struct _fat_ptr _tmp4EF=({const char*_tmp2EC="%s.iB";_tag_fat(_tmp2EC,sizeof(char),6U);});Cyc_aprintf(_tmp4EF,_tag_fat(_tmp2EB,sizeof(void*),1));});}));}),sizeof(char),1U));const char*macrosfile=_tmp1AC;
# 1422
const char*_tmp1AD=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _tmp2E4=({struct Cyc_String_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=0,_tmp3DA.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DA;});void*_tmp2E2[1];_tmp2E2[0]=& _tmp2E4;({struct _fat_ptr _tmp4F4=({const char*_tmp2E3="%s.iC";_tag_fat(_tmp2E3,sizeof(char),6U);});Cyc_aprintf(_tmp4F4,_tag_fat(_tmp2E2,sizeof(void*),1));});}):({struct _fat_ptr _tmp4F3=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp4F3,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E7=({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0,_tmp3DB.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DB;});void*_tmp2E5[1];_tmp2E5[0]=& _tmp2E7;({struct _fat_ptr _tmp4F2=({const char*_tmp2E6="%s.iC";_tag_fat(_tmp2E6,sizeof(char),6U);});Cyc_aprintf(_tmp4F2,_tag_fat(_tmp2E5,sizeof(void*),1));});}));}),sizeof(char),1U));const char*declsfile=_tmp1AD;
# 1427
const char*_tmp1AE=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _tmp2DE=({struct Cyc_String_pa_PrintArg_struct _tmp3D8;_tmp3D8.tag=0,_tmp3D8.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3D8;});void*_tmp2DC[1];_tmp2DC[0]=& _tmp2DE;({struct _fat_ptr _tmp4F7=({const char*_tmp2DD="%s.iD";_tag_fat(_tmp2DD,sizeof(char),6U);});Cyc_aprintf(_tmp4F7,_tag_fat(_tmp2DC,sizeof(void*),1));});}):({struct _fat_ptr _tmp4F6=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp4F6,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E1=({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0,_tmp3D9.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3D9;});void*_tmp2DF[1];_tmp2DF[0]=& _tmp2E1;({struct _fat_ptr _tmp4F5=({const char*_tmp2E0="%s.iD";_tag_fat(_tmp2E0,sizeof(char),6U);});Cyc_aprintf(_tmp4F5,_tag_fat(_tmp2DF,sizeof(void*),1));});}));}),sizeof(char),1U));const char*filtereddeclsfile=_tmp1AE;
# 1432
{struct _handler_cons _tmp1AF;_push_handler(& _tmp1AF);{int _tmp1B1=0;if(setjmp(_tmp1AF.handler))_tmp1B1=1;if(!_tmp1B1){
# 1435
if(Cyc_force_directory_prefixes(({const char*_tmp1B2=filename;_tag_fat(_tmp1B2,sizeof(char),_get_zero_arr_size_char((void*)_tmp1B2,1U));}))){
int _tmp1B3=1;_npop_handler(0);return _tmp1B3;}
# 1440
if((int)Cyc_mode != 3){
if((int)Cyc_mode == 2){
({struct Cyc_String_pa_PrintArg_struct _tmp1B6=({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0,({struct _fat_ptr _tmp4F8=(struct _fat_ptr)({const char*_tmp1B7=cppinfile;_tag_fat(_tmp1B7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1B7,1U));});_tmp3A9.f1=_tmp4F8;});_tmp3A9;});void*_tmp1B4[1];_tmp1B4[0]=& _tmp1B6;({struct _fat_ptr _tmp4F9=({const char*_tmp1B5="cat >%s <<XXX\n";_tag_fat(_tmp1B5,sizeof(char),15U);});Cyc_prscript(_tmp4F9,_tag_fat(_tmp1B4,sizeof(void*),1));});});
{struct Cyc_List_List*_tmp1B8=cpp_insert;struct Cyc_List_List*l=_tmp1B8;for(0;l != 0;l=l->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp1BB=({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0,_tmp3AA.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)l->hd));_tmp3AA;});void*_tmp1B9[1];_tmp1B9[0]=& _tmp1BB;({struct _fat_ptr _tmp4FA=({const char*_tmp1BA="%s";_tag_fat(_tmp1BA,sizeof(char),3U);});Cyc_prscript(_tmp4FA,_tag_fat(_tmp1B9,sizeof(void*),1));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp1BE=({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0,({struct _fat_ptr _tmp4FB=(struct _fat_ptr)({const char*_tmp1BF=filename;_tag_fat(_tmp1BF,sizeof(char),_get_zero_arr_size_char((void*)_tmp1BF,1U));});_tmp3AB.f1=_tmp4FB;});_tmp3AB;});void*_tmp1BC[1];_tmp1BC[0]=& _tmp1BE;({struct _fat_ptr _tmp4FC=({const char*_tmp1BD="#include <%s>\n";_tag_fat(_tmp1BD,sizeof(char),15U);});Cyc_prscript(_tmp4FC,_tag_fat(_tmp1BC,sizeof(void*),1));});});
({void*_tmp1C0=0U;({struct _fat_ptr _tmp4FD=({const char*_tmp1C1="XXX\n";_tag_fat(_tmp1C1,sizeof(char),5U);});Cyc_prscript(_tmp4FD,_tag_fat(_tmp1C0,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1C4=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0,_tmp3AE.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3AE;});struct Cyc_String_pa_PrintArg_struct _tmp1C5=({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0,({struct _fat_ptr _tmp4FE=(struct _fat_ptr)({const char*_tmp1C8=macrosfile;_tag_fat(_tmp1C8,sizeof(char),_get_zero_arr_size_char((void*)_tmp1C8,1U));});_tmp3AD.f1=_tmp4FE;});_tmp3AD;});struct Cyc_String_pa_PrintArg_struct _tmp1C6=({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0,({struct _fat_ptr _tmp4FF=(struct _fat_ptr)({const char*_tmp1C7=cppinfile;_tag_fat(_tmp1C7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1C7,1U));});_tmp3AC.f1=_tmp4FF;});_tmp3AC;});void*_tmp1C2[3];_tmp1C2[0]=& _tmp1C4,_tmp1C2[1]=& _tmp1C5,_tmp1C2[2]=& _tmp1C6;({struct _fat_ptr _tmp500=({const char*_tmp1C3="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_fat(_tmp1C3,sizeof(char),35U);});Cyc_prscript(_tmp500,_tag_fat(_tmp1C2,sizeof(void*),3));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1CB=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0,_tmp3B1.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3B1;});struct Cyc_String_pa_PrintArg_struct _tmp1CC=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0,({struct _fat_ptr _tmp501=(struct _fat_ptr)({const char*_tmp1CF=declsfile;_tag_fat(_tmp1CF,sizeof(char),_get_zero_arr_size_char((void*)_tmp1CF,1U));});_tmp3B0.f1=_tmp501;});_tmp3B0;});struct Cyc_String_pa_PrintArg_struct _tmp1CD=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0,({struct _fat_ptr _tmp502=(struct _fat_ptr)({const char*_tmp1CE=cppinfile;_tag_fat(_tmp1CE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1CE,1U));});_tmp3AF.f1=_tmp502;});_tmp3AF;});void*_tmp1C9[3];_tmp1C9[0]=& _tmp1CB,_tmp1C9[1]=& _tmp1CC,_tmp1C9[2]=& _tmp1CD;({struct _fat_ptr _tmp503=({const char*_tmp1CA="$GCC %s -E     -o %s -x c %s;\n";_tag_fat(_tmp1CA,sizeof(char),31U);});Cyc_prscript(_tmp503,_tag_fat(_tmp1C9,sizeof(void*),3));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1D2=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0,({struct _fat_ptr _tmp504=(struct _fat_ptr)({const char*_tmp1D3=cppinfile;_tag_fat(_tmp1D3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D3,1U));});_tmp3B2.f1=_tmp504;});_tmp3B2;});void*_tmp1D0[1];_tmp1D0[0]=& _tmp1D2;({struct _fat_ptr _tmp505=({const char*_tmp1D1="rm %s\n";_tag_fat(_tmp1D1,sizeof(char),7U);});Cyc_prscript(_tmp505,_tag_fat(_tmp1D0,sizeof(void*),1));});});}else{
# 1452
maybe=Cyc_fopen(cppinfile,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp1D6=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0,({struct _fat_ptr _tmp506=(struct _fat_ptr)({const char*_tmp1D7=cppinfile;_tag_fat(_tmp1D7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D7,1U));});_tmp3B3.f1=_tmp506;});_tmp3B3;});void*_tmp1D4[1];_tmp1D4[0]=& _tmp1D6;({struct Cyc___cycFILE*_tmp508=Cyc_stderr;struct _fat_ptr _tmp507=({const char*_tmp1D5="Error: could not create file %s\n";_tag_fat(_tmp1D5,sizeof(char),33U);});Cyc_fprintf(_tmp508,_tmp507,_tag_fat(_tmp1D4,sizeof(void*),1));});});{
int _tmp1D8=1;_npop_handler(0);return _tmp1D8;}}
# 1457
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp1DB=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0,({struct _fat_ptr _tmp509=(struct _fat_ptr)({const char*_tmp1DC=cppinfile;_tag_fat(_tmp1DC,sizeof(char),_get_zero_arr_size_char((void*)_tmp1DC,1U));});_tmp3B4.f1=_tmp509;});_tmp3B4;});void*_tmp1D9[1];_tmp1D9[0]=& _tmp1DB;({struct Cyc___cycFILE*_tmp50B=Cyc_stderr;struct _fat_ptr _tmp50A=({const char*_tmp1DA="Creating %s\n";_tag_fat(_tmp1DA,sizeof(char),13U);});Cyc_fprintf(_tmp50B,_tmp50A,_tag_fat(_tmp1D9,sizeof(void*),1));});});
out_file=maybe;
{struct Cyc_List_List*_tmp1DD=cpp_insert;struct Cyc_List_List*l=_tmp1DD;for(0;l != 0;l=l->tl){
Cyc_fputs((const char*)_check_null(_untag_fat_ptr(*((struct _fat_ptr*)l->hd),sizeof(char),1U)),out_file);}}
# 1463
({struct Cyc_String_pa_PrintArg_struct _tmp1E0=({struct Cyc_String_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=0,({struct _fat_ptr _tmp50C=(struct _fat_ptr)({const char*_tmp1E1=filename;_tag_fat(_tmp1E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E1,1U));});_tmp3B5.f1=_tmp50C;});_tmp3B5;});void*_tmp1DE[1];_tmp1DE[0]=& _tmp1E0;({struct Cyc___cycFILE*_tmp50E=out_file;struct _fat_ptr _tmp50D=({const char*_tmp1DF="#include <%s>\n";_tag_fat(_tmp1DF,sizeof(char),15U);});Cyc_fprintf(_tmp50E,_tmp50D,_tag_fat(_tmp1DE,sizeof(void*),1));});});
Cyc_fclose(out_file);{
struct _fat_ptr _tmp1E2=({
struct Cyc_List_List*_tmp513=({struct Cyc_List_List*_tmp1FF=_cycalloc(sizeof(*_tmp1FF));({struct _fat_ptr*_tmp512=({struct _fat_ptr*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));({struct _fat_ptr _tmp511=(struct _fat_ptr)({const char*_tmp1FD="";_tag_fat(_tmp1FD,sizeof(char),1U);});*_tmp1FE=_tmp511;});_tmp1FE;});_tmp1FF->hd=_tmp512;}),({
struct Cyc_List_List*_tmp510=({struct Cyc_List_List*(*_tmp50F)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;});_tmp50F(Cyc_sh_escape_stringptr,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_cppargs));});_tmp1FF->tl=_tmp510;});_tmp1FF;});
# 1466
Cyc_str_sepstr(_tmp513,({const char*_tmp200=" ";_tag_fat(_tmp200,sizeof(char),2U);}));});
# 1465
struct _fat_ptr cppargs_string=_tmp1E2;
# 1469
char*cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp1F6=({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0,_tmp3C1.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3C1;});struct Cyc_String_pa_PrintArg_struct _tmp1F7=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0,_tmp3C0.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3C0;});struct Cyc_String_pa_PrintArg_struct _tmp1F8=({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0,_tmp3BF.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3BF;});struct Cyc_String_pa_PrintArg_struct _tmp1F9=({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0,({struct _fat_ptr _tmp514=(struct _fat_ptr)({const char*_tmp1FC=macrosfile;_tag_fat(_tmp1FC,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FC,1U));});_tmp3BE.f1=_tmp514;});_tmp3BE;});struct Cyc_String_pa_PrintArg_struct _tmp1FA=({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0,({struct _fat_ptr _tmp515=(struct _fat_ptr)({const char*_tmp1FB=cppinfile;_tag_fat(_tmp1FB,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FB,1U));});_tmp3BD.f1=_tmp515;});_tmp3BD;});void*_tmp1F4[5];_tmp1F4[0]=& _tmp1F6,_tmp1F4[1]=& _tmp1F7,_tmp1F4[2]=& _tmp1F8,_tmp1F4[3]=& _tmp1F9,_tmp1F4[4]=& _tmp1FA;({struct _fat_ptr _tmp516=({const char*_tmp1F5="%s %s %s -E -dM -o %s -x c %s";_tag_fat(_tmp1F5,sizeof(char),30U);});Cyc_aprintf(_tmp516,_tag_fat(_tmp1F4,sizeof(void*),5));});}),sizeof(char),1U));
# 1472
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp1E5=({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0,({struct _fat_ptr _tmp517=(struct _fat_ptr)({char*_tmp1E6=cmd;_tag_fat(_tmp1E6,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E6,1U));});_tmp3B6.f1=_tmp517;});_tmp3B6;});void*_tmp1E3[1];_tmp1E3[0]=& _tmp1E5;({struct Cyc___cycFILE*_tmp519=Cyc_stderr;struct _fat_ptr _tmp518=({const char*_tmp1E4="%s\n";_tag_fat(_tmp1E4,sizeof(char),4U);});Cyc_fprintf(_tmp519,_tmp518,_tag_fat(_tmp1E3,sizeof(void*),1));});});
if(!system((const char*)cmd)){
# 1477
cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp1E9=({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0,_tmp3BB.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3BB;});struct Cyc_String_pa_PrintArg_struct _tmp1EA=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0,_tmp3BA.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3BA;});struct Cyc_String_pa_PrintArg_struct _tmp1EB=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0,_tmp3B9.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3B9;});struct Cyc_String_pa_PrintArg_struct _tmp1EC=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0,({struct _fat_ptr _tmp51A=(struct _fat_ptr)({const char*_tmp1EF=declsfile;_tag_fat(_tmp1EF,sizeof(char),_get_zero_arr_size_char((void*)_tmp1EF,1U));});_tmp3B8.f1=_tmp51A;});_tmp3B8;});struct Cyc_String_pa_PrintArg_struct _tmp1ED=({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0,({struct _fat_ptr _tmp51B=(struct _fat_ptr)({const char*_tmp1EE=cppinfile;_tag_fat(_tmp1EE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1EE,1U));});_tmp3B7.f1=_tmp51B;});_tmp3B7;});void*_tmp1E7[5];_tmp1E7[0]=& _tmp1E9,_tmp1E7[1]=& _tmp1EA,_tmp1E7[2]=& _tmp1EB,_tmp1E7[3]=& _tmp1EC,_tmp1E7[4]=& _tmp1ED;({struct _fat_ptr _tmp51C=({const char*_tmp1E8="%s %s %s -E -o %s -x c %s";_tag_fat(_tmp1E8,sizeof(char),26U);});Cyc_aprintf(_tmp51C,_tag_fat(_tmp1E7,sizeof(void*),5));});}),sizeof(char),1U));
# 1480
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp1F2=({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0,({struct _fat_ptr _tmp51D=(struct _fat_ptr)({char*_tmp1F3=cmd;_tag_fat(_tmp1F3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F3,1U));});_tmp3BC.f1=_tmp51D;});_tmp3BC;});void*_tmp1F0[1];_tmp1F0[0]=& _tmp1F2;({struct Cyc___cycFILE*_tmp51F=Cyc_stderr;struct _fat_ptr _tmp51E=({const char*_tmp1F1="%s\n";_tag_fat(_tmp1F1,sizeof(char),4U);});Cyc_fprintf(_tmp51F,_tmp51E,_tag_fat(_tmp1F0,sizeof(void*),1));});});
system((const char*)cmd);}}}}
# 1487
if(Cyc_gathering()){int _tmp201=0;_npop_handler(0);return _tmp201;}{
# 1490
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp522=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp204=({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0,({struct _fat_ptr _tmp520=(struct _fat_ptr)({const char*_tmp205=macrosfile;_tag_fat(_tmp205,sizeof(char),_get_zero_arr_size_char((void*)_tmp205,1U));});_tmp3C2.f1=_tmp520;});_tmp3C2;});void*_tmp202[1];_tmp202[0]=& _tmp204;({struct _fat_ptr _tmp521=({const char*_tmp203="can't open macrosfile %s";_tag_fat(_tmp203,sizeof(char),25U);});Cyc_aprintf(_tmp521,_tag_fat(_tmp202,sizeof(void*),1));});});_tmp206->f1=_tmp522;});_tmp206;}));
# 1495
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp207=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp207;
struct _tuple13*entry;
while((entry=({(struct _tuple13*(*)(struct Cyc_Lexing_lexbuf*))Cyc_line;})(l))!= 0){
struct _tuple13*_tmp208=(struct _tuple13*)_check_null(entry);struct _tuple13*_stmttmpF=_tmp208;struct _tuple13*_tmp209=_stmttmpF;void*_tmp20B;void*_tmp20A;_tmp20A=_tmp209->f1;_tmp20B=_tmp209->f2;{struct _fat_ptr*name=_tmp20A;struct Cyc_Set_Set*uses=_tmp20B;
({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;})(t,name,uses);}}
# 1504
Cyc_fclose(in_file);
# 1507
maybe=Cyc_fopen(declsfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp525=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp20E=({struct Cyc_String_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=0,({struct _fat_ptr _tmp523=(struct _fat_ptr)({const char*_tmp20F=declsfile;_tag_fat(_tmp20F,sizeof(char),_get_zero_arr_size_char((void*)_tmp20F,1U));});_tmp3C3.f1=_tmp523;});_tmp3C3;});void*_tmp20C[1];_tmp20C[0]=& _tmp20E;({struct _fat_ptr _tmp524=({const char*_tmp20D="can't open declsfile %s";_tag_fat(_tmp20D,sizeof(char),24U);});Cyc_aprintf(_tmp524,_tag_fat(_tmp20C,sizeof(void*),1));});});_tmp210->f1=_tmp525;});_tmp210;}));
# 1511
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(filtereddeclsfile,"w");
if(!((unsigned)Cyc_slurp_out)){int _tmp211=1;_npop_handler(0);return _tmp211;}
while(({(int(*)(struct Cyc_Lexing_lexbuf*))Cyc_slurp;})(l)){;}{
# 1517
struct Cyc_List_List*_tmp212=user_defs;struct Cyc_List_List*x=_tmp212;
while(x != 0){
struct _tuple16*_tmp213=(struct _tuple16*)x->hd;struct _tuple16*_stmttmp10=_tmp213;struct _tuple16*_tmp214=_stmttmp10;void*_tmp215;_tmp215=_tmp214->f2;{struct _fat_ptr*s=_tmp215;
({struct Cyc_String_pa_PrintArg_struct _tmp218=({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0,_tmp3C4.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp3C4;});void*_tmp216[1];_tmp216[0]=& _tmp218;({struct Cyc___cycFILE*_tmp527=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp526=({const char*_tmp217="%s";_tag_fat(_tmp217,sizeof(char),3U);});Cyc_fprintf(_tmp527,_tmp526,_tag_fat(_tmp216,sizeof(void*),1));});});
x=x->tl;}}
# 1523
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if((int)Cyc_mode != 3)
;
# 1529
maybe=Cyc_fopen(filtereddeclsfile,"r");
if(!((unsigned)maybe)){int _tmp219=1;_npop_handler(0);return _tmp219;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp21A=filtereddeclsfile;_tag_fat(_tmp21A,sizeof(char),_get_zero_arr_size_char((void*)_tmp21A,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp21B=Cyc_Parse_parse_file(in_file);struct Cyc_List_List*decls=_tmp21B;
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1539
{struct Cyc_List_List*_tmp21C=decls;struct Cyc_List_List*d=_tmp21C;for(0;d != 0;d=d->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)d->hd,t);}}{
# 1543
struct Cyc_List_List*_tmp21D=({struct Cyc_List_List*(*_tmp528)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;});_tmp528(Cyc_add_user_prefix,(({(struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split;})(user_defs)).f1);});struct Cyc_List_List*user_symbols=_tmp21D;
struct Cyc_Set_Set*_tmp21E=({struct Cyc_List_List*_tmp529=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(start_symbols,user_symbols);Cyc_reachable(_tmp529,t);});struct Cyc_Set_Set*reachable_set=_tmp21E;
# 1547
struct Cyc_List_List*_tmp21F=0;struct Cyc_List_List*reachable_decls=_tmp21F;
struct Cyc_List_List*_tmp220=0;struct Cyc_List_List*user_decls=_tmp220;
struct Cyc_Set_Set*defined_symbols=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp221=decls;struct Cyc_List_List*d=_tmp221;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp222=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp222;
struct _fat_ptr*name;
{void*_tmp223=decl->r;void*_stmttmp11=_tmp223;void*_tmp224=_stmttmp11;void*_tmp225;switch(*((int*)_tmp224)){case 0: _tmp225=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp224)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp225;
# 1555
struct _tuple1*_tmp226=x->name;struct _tuple1*_stmttmp12=_tmp226;struct _tuple1*_tmp227=_stmttmp12;void*_tmp228;_tmp228=_tmp227->f2;{struct _fat_ptr*v=_tmp228;
defined_symbols=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(defined_symbols,v);
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 1: _tmp225=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp224)->f1;{struct Cyc_Absyn_Fndecl*x=_tmp225;
# 1561
struct _tuple1*_tmp229=x->name;struct _tuple1*_stmttmp13=_tmp229;struct _tuple1*_tmp22A=_stmttmp13;void*_tmp22B;_tmp22B=_tmp22A->f2;{struct _fat_ptr*v=_tmp22B;
defined_symbols=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(defined_symbols,v);
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 5: _tmp225=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp224)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp225;
# 1567
struct _tuple1*_tmp22C=x->name;struct _tuple1*_stmttmp14=_tmp22C;struct _tuple1*_tmp22D=_stmttmp14;void*_tmp22E;_tmp22E=_tmp22D->f2;{struct _fat_ptr*v=_tmp22E;
name=v;
goto _LL6;}}case 7: _tmp225=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp224)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp225;
# 1571
struct _tuple1*_tmp22F=x->name;struct _tuple1*_stmttmp15=_tmp22F;struct _tuple1*_tmp230=_stmttmp15;void*_tmp231;_tmp231=_tmp230->f2;{struct _fat_ptr*v=_tmp231;
name=v;
# 1575
if(name != 0 &&({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name))
reachable_decls=({struct Cyc_List_List*_tmp232=_cycalloc(sizeof(*_tmp232));_tmp232->hd=decl,_tmp232->tl=reachable_decls;_tmp232;});else{
# 1578
if((unsigned)x->fields){
struct Cyc_List_List*_tmp233=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp233;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp234=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp234;
struct _tuple1*_tmp235=f->name;struct _tuple1*_stmttmp16=_tmp235;struct _tuple1*_tmp236=_stmttmp16;void*_tmp237;_tmp237=_tmp236->f2;{struct _fat_ptr*v=_tmp237;
if(({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,v)){
reachable_decls=({struct Cyc_List_List*_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238->hd=decl,_tmp238->tl=reachable_decls;_tmp238;});
break;}}}}}
# 1588
name=0;
goto _LL6;}}case 8: _tmp225=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp224)->f1;{struct Cyc_Absyn_Typedefdecl*x=_tmp225;
# 1591
struct _tuple1*_tmp239=x->name;struct _tuple1*_stmttmp17=_tmp239;struct _tuple1*_tmp23A=_stmttmp17;void*_tmp23B;_tmp23B=_tmp23A->f2;{struct _fat_ptr*v=_tmp23B;
name=v;
goto _LL6;}}case 13:
 goto _LL14;case 14: _LL14:
 goto _LL16;case 15: _LL16:
 goto _LL18;case 16: _LL18:
 goto _LL1A;case 2: _LL1A:
 goto _LL1C;case 6: _LL1C:
 goto _LL1E;case 3: _LL1E:
 goto _LL20;case 9: _LL20:
 goto _LL22;case 10: _LL22:
 goto _LL24;case 11: _LL24:
 goto _LL26;case 12: _LL26:
 goto _LL28;default: _LL28:
# 1606
 name=0;
goto _LL6;}_LL6:;}
# 1610
if(name != 0 &&({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name)){
if(({struct _fat_ptr _tmp52B=(struct _fat_ptr)*name;struct _fat_ptr _tmp52A=(struct _fat_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp52B,_tmp52A,Cyc_strlen((struct _fat_ptr)Cyc_user_prefix));})!= 0)
reachable_decls=({struct Cyc_List_List*_tmp23C=_cycalloc(sizeof(*_tmp23C));_tmp23C->hd=decl,_tmp23C->tl=reachable_decls;_tmp23C;});else{
# 1615
Cyc_rename_decl(decl);
user_decls=({struct Cyc_List_List*_tmp23D=_cycalloc(sizeof(*_tmp23D));_tmp23D->hd=decl,_tmp23D->tl=user_decls;_tmp23D;});}}}}
# 1622
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){int _tmp23E=1;_npop_handler(0);return _tmp23E;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _fat_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2B7=({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0,({struct _fat_ptr _tmp52C=(struct _fat_ptr)({const char*_tmp2B8=filename;_tag_fat(_tmp2B8,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B8,1U));});_tmp3D0.f1=_tmp52C;});_tmp3D0;});void*_tmp2B5[1];_tmp2B5[0]=& _tmp2B7;({struct _fat_ptr _tmp52D=({const char*_tmp2B6="_%s_";_tag_fat(_tmp2B6,sizeof(char),5U);});Cyc_aprintf(_tmp52D,_tag_fat(_tmp2B5,sizeof(void*),1));});});
{int _tmp23F=0;int j=_tmp23F;for(0;(unsigned)j < _get_fat_size(ifdefmacro,sizeof(char));++ j){
if((int)((char*)ifdefmacro.curr)[j]== 46 ||(int)((char*)ifdefmacro.curr)[j]== 47)
({struct _fat_ptr _tmp240=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp241=*((char*)_check_fat_subscript(_tmp240,sizeof(char),0U));char _tmp242='_';if(_get_fat_size(_tmp240,sizeof(char))== 1U &&(_tmp241 == 0 && _tmp242 != 0))_throw_arraybounds();*((char*)_tmp240.curr)=_tmp242;});else{
if((int)((char*)ifdefmacro.curr)[j]!= 95 &&(int)((char*)ifdefmacro.curr)[j]!= 47)
({struct _fat_ptr _tmp243=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp244=*((char*)_check_fat_subscript(_tmp243,sizeof(char),0U));char _tmp245=(char)toupper((int)((char*)ifdefmacro.curr)[j]);if(_get_fat_size(_tmp243,sizeof(char))== 1U &&(_tmp244 == 0 && _tmp245 != 0))_throw_arraybounds();*((char*)_tmp243.curr)=_tmp245;});}}}
# 1634
({struct Cyc_String_pa_PrintArg_struct _tmp248=({struct Cyc_String_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=0,_tmp3C6.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3C6;});struct Cyc_String_pa_PrintArg_struct _tmp249=({struct Cyc_String_pa_PrintArg_struct _tmp3C5;_tmp3C5.tag=0,_tmp3C5.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3C5;});void*_tmp246[2];_tmp246[0]=& _tmp248,_tmp246[1]=& _tmp249;({struct Cyc___cycFILE*_tmp52F=out_file;struct _fat_ptr _tmp52E=({const char*_tmp247="#ifndef %s\n#define %s\n";_tag_fat(_tmp247,sizeof(char),23U);});Cyc_fprintf(_tmp52F,_tmp52E,_tag_fat(_tmp246,sizeof(void*),2));});});{
# 1641
struct Cyc_List_List*_tmp24A=0;struct Cyc_List_List*print_decls=_tmp24A;
struct Cyc_List_List*_tmp24B=0;struct Cyc_List_List*names=_tmp24B;
{struct Cyc_List_List*_tmp24C=reachable_decls;struct Cyc_List_List*d=_tmp24C;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp24D=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp24D;
int _tmp24E=0;int anon_enum=_tmp24E;
struct _fat_ptr*name;
{void*_tmp24F=decl->r;void*_stmttmp18=_tmp24F;void*_tmp250=_stmttmp18;void*_tmp251;switch(*((int*)_tmp250)){case 0: _tmp251=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp250)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp251;
# 1649
struct _tuple1*_tmp252=x->name;struct _tuple1*_stmttmp19=_tmp252;struct _tuple1*_tmp253=_stmttmp19;void*_tmp254;_tmp254=_tmp253->f2;{struct _fat_ptr*v=_tmp254;
name=v;
goto _LL3B;}}case 1: _tmp251=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp250)->f1;{struct Cyc_Absyn_Fndecl*x=_tmp251;
# 1653
if(x->is_inline){name=0;goto _LL3B;}{
struct _tuple1*_tmp255=x->name;struct _tuple1*_stmttmp1A=_tmp255;struct _tuple1*_tmp256=_stmttmp1A;void*_tmp257;_tmp257=_tmp256->f2;{struct _fat_ptr*v=_tmp257;
name=v;
goto _LL3B;}}}case 5: _tmp251=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp250)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp251;
# 1658
struct _tuple1*_tmp258=x->name;struct _tuple1*_stmttmp1B=_tmp258;struct _tuple1*_tmp259=_stmttmp1B;void*_tmp25A;_tmp25A=_tmp259->f2;{struct _fat_ptr*v=_tmp25A;
name=v;
goto _LL3B;}}case 7: _tmp251=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp250)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp251;
# 1662
struct _tuple1*_tmp25B=x->name;struct _tuple1*_stmttmp1C=_tmp25B;struct _tuple1*_tmp25C=_stmttmp1C;void*_tmp25D;_tmp25D=_tmp25C->f2;{struct _fat_ptr*v=_tmp25D;
name=v;
goto _LL3B;}}case 8: _tmp251=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp250)->f1;{struct Cyc_Absyn_Typedefdecl*x=_tmp251;
# 1666
struct _tuple1*_tmp25E=x->name;struct _tuple1*_stmttmp1D=_tmp25E;struct _tuple1*_tmp25F=_stmttmp1D;void*_tmp260;_tmp260=_tmp25F->f2;{struct _fat_ptr*v=_tmp260;
name=v;
goto _LL3B;}}case 4:
 goto _LL49;case 13: _LL49:
 goto _LL4B;case 14: _LL4B:
 goto _LL4D;case 15: _LL4D:
 goto _LL4F;case 16: _LL4F:
 goto _LL51;case 2: _LL51:
 goto _LL53;case 6: _LL53:
 goto _LL55;case 3: _LL55:
 goto _LL57;case 9: _LL57:
 goto _LL59;case 10: _LL59:
 goto _LL5B;case 11: _LL5B:
 goto _LL5D;default: _LL5D:
# 1681
 name=0;
goto _LL3B;}_LL3B:;}
# 1684
if(!((unsigned)name)&& !anon_enum)continue;
# 1689
if(({int(*_tmp531)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp530=reachable_set;_tmp531(_tmp530,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp261=Cyc_Absyn_lookup_decl(user_decls,name);struct Cyc_Absyn_Decl*user_decl=_tmp261;
if(user_decl == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp532=({const char*_tmp262="Internal Error: bad user-def name";_tag_fat(_tmp262,sizeof(char),34U);});_tmp263->f1=_tmp532;});_tmp263;}));else{
# 1695
void*_tmp264=user_decl->r;void*_stmttmp1E=_tmp264;void*_tmp265=_stmttmp1E;switch(*((int*)_tmp265)){case 0:
 goto _LL71;case 1: _LL71:
# 1698
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp267=_cycalloc(sizeof(*_tmp267));_tmp267->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp533=({const char*_tmp266="user defintions for function or variable decls";_tag_fat(_tmp266,sizeof(char),47U);});_tmp267->f1=_tmp533;});_tmp267;}));default:
 goto _LL6D;}_LL6D:;}
# 1704
print_decls=({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->hd=decl,_tmp268->tl=print_decls;_tmp268;});}else{
# 1707
print_decls=({struct Cyc_List_List*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->hd=decl,_tmp269->tl=print_decls;_tmp269;});}
names=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=name,_tmp26A->tl=names;_tmp26A;});}}
# 1712
{struct _handler_cons _tmp26B;_push_handler(& _tmp26B);{int _tmp26D=0;if(setjmp(_tmp26B.handler))_tmp26D=1;if(!_tmp26D){
Cyc_Binding_resolve_all(print_decls);
({struct Cyc_Tcenv_Tenv*_tmp534=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp534,1,print_decls);});
# 1713
;_pop_handler();}else{void*_tmp26C=(void*)Cyc_Core_get_exn_thrown();void*_tmp26E=_tmp26C;
# 1716
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp535=({const char*_tmp26F="can't typecheck acquired declarations";_tag_fat(_tmp26F,sizeof(char),38U);});_tmp270->f1=_tmp535;});_tmp270;}));;}}}
# 1720
{struct _tuple0 _tmp271=({struct _tuple0 _tmp3CA;_tmp3CA.f1=print_decls,_tmp3CA.f2=names;_tmp3CA;});struct _tuple0 _stmttmp1F=_tmp271;struct _tuple0 _tmp272=_stmttmp1F;void*_tmp274;void*_tmp273;_tmp273=_tmp272.f1;_tmp274=_tmp272.f2;{struct Cyc_List_List*d=_tmp273;struct Cyc_List_List*n=_tmp274;for(0;
d != 0 && n != 0;(d=d->tl,n=n->tl)){
struct Cyc_Absyn_Decl*_tmp275=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp275;
struct _fat_ptr*_tmp276=(struct _fat_ptr*)n->hd;struct _fat_ptr*name=_tmp276;
int _tmp277=0;int anon_enum=_tmp277;
if(!((unsigned)name))
anon_enum=1;
# 1729
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned)name){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp3C7;_tmp3C7.tag=0,_tmp3C7.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp3C7;});void*_tmp278[1];_tmp278[0]=& _tmp27A;({struct _fat_ptr _tmp536=({const char*_tmp279="_%s_def_";_tag_fat(_tmp279,sizeof(char),9U);});Cyc_aprintf(_tmp536,_tag_fat(_tmp278,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp27D=({struct Cyc_String_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=0,_tmp3C8.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3C8;});void*_tmp27B[1];_tmp27B[0]=& _tmp27D;({struct Cyc___cycFILE*_tmp538=out_file;struct _fat_ptr _tmp537=({const char*_tmp27C="#ifndef %s\n";_tag_fat(_tmp27C,sizeof(char),12U);});Cyc_fprintf(_tmp538,_tmp537,_tag_fat(_tmp27B,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp280=({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0,_tmp3C9.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3C9;});void*_tmp27E[1];_tmp27E[0]=& _tmp280;({struct Cyc___cycFILE*_tmp53A=out_file;struct _fat_ptr _tmp539=({const char*_tmp27F="#define %s\n";_tag_fat(_tmp27F,sizeof(char),12U);});Cyc_fprintf(_tmp53A,_tmp539,_tag_fat(_tmp27E,sizeof(void*),1));});});
# 1735
({struct Cyc_List_List*_tmp53C=({struct Cyc_Absyn_Decl*_tmp281[1];_tmp281[0]=decl;({struct Cyc_List_List*(*_tmp53B)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp53B(_tag_fat(_tmp281,sizeof(struct Cyc_Absyn_Decl*),1));});});Cyc_Absynpp_decllist2file(_tmp53C,out_file);});
({void*_tmp282=0U;({struct Cyc___cycFILE*_tmp53E=out_file;struct _fat_ptr _tmp53D=({const char*_tmp283="#endif\n";_tag_fat(_tmp283,sizeof(char),8U);});Cyc_fprintf(_tmp53E,_tmp53D,_tag_fat(_tmp282,sizeof(void*),0));});});}else{
# 1740
({struct Cyc_List_List*_tmp540=({struct Cyc_Absyn_Decl*_tmp284[1];_tmp284[0]=decl;({struct Cyc_List_List*(*_tmp53F)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp53F(_tag_fat(_tmp284,sizeof(struct Cyc_Absyn_Decl*),1));});});Cyc_Absynpp_decllist2file(_tmp540,out_file);});}}}}
# 1745
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp543=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp287=({struct Cyc_String_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=0,({struct _fat_ptr _tmp541=(struct _fat_ptr)({const char*_tmp288=macrosfile;_tag_fat(_tmp288,sizeof(char),_get_zero_arr_size_char((void*)_tmp288,1U));});_tmp3CB.f1=_tmp541;});_tmp3CB;});void*_tmp285[1];_tmp285[0]=& _tmp287;({struct _fat_ptr _tmp542=({const char*_tmp286="can't open macrosfile %s";_tag_fat(_tmp286,sizeof(char),25U);});Cyc_aprintf(_tmp542,_tag_fat(_tmp285,sizeof(void*),1));});});_tmp289->f1=_tmp543;});_tmp289;}));
# 1748
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);{
struct _tuple14*entry2;
while((entry2=({(struct _tuple14*(*)(struct Cyc_Lexing_lexbuf*))Cyc_suck_line;})(l))!= 0){
struct _tuple14*_tmp28A=(struct _tuple14*)_check_null(entry2);struct _tuple14*_stmttmp20=_tmp28A;struct _tuple14*_tmp28B=_stmttmp20;void*_tmp28D;struct _fat_ptr _tmp28C;_tmp28C=_tmp28B->f1;_tmp28D=_tmp28B->f2;{struct _fat_ptr line=_tmp28C;struct _fat_ptr*name=_tmp28D;
if(({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name)){
({struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp3CC;_tmp3CC.tag=0,_tmp3CC.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp3CC;});void*_tmp28E[1];_tmp28E[0]=& _tmp290;({struct Cyc___cycFILE*_tmp545=out_file;struct _fat_ptr _tmp544=({const char*_tmp28F="#ifndef %s\n";_tag_fat(_tmp28F,sizeof(char),12U);});Cyc_fprintf(_tmp545,_tmp544,_tag_fat(_tmp28E,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp293=({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0,_tmp3CD.f1=(struct _fat_ptr)((struct _fat_ptr)line);_tmp3CD;});void*_tmp291[1];_tmp291[0]=& _tmp293;({struct Cyc___cycFILE*_tmp547=out_file;struct _fat_ptr _tmp546=({const char*_tmp292="%s\n";_tag_fat(_tmp292,sizeof(char),4U);});Cyc_fprintf(_tmp547,_tmp546,_tag_fat(_tmp291,sizeof(void*),1));});});
({void*_tmp294=0U;({struct Cyc___cycFILE*_tmp549=out_file;struct _fat_ptr _tmp548=({const char*_tmp295="#endif\n";_tag_fat(_tmp295,sizeof(char),8U);});Cyc_fprintf(_tmp549,_tmp548,_tag_fat(_tmp294,sizeof(void*),0));});});}}}
# 1759
Cyc_fclose(in_file);
if((int)Cyc_mode != 3);
# 1762
if(hstubs != 0){
struct Cyc_List_List*_tmp296=hstubs;struct Cyc_List_List*x=_tmp296;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp297=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp21=_tmp297;struct _tuple15*_tmp298=_stmttmp21;struct _fat_ptr _tmp29A;struct _fat_ptr _tmp299;_tmp299=_tmp298->f1;_tmp29A=_tmp298->f2;{struct _fat_ptr symbol=_tmp299;struct _fat_ptr text=_tmp29A;
if(({char*_tmp54D=(char*)text.curr;_tmp54D != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp54C=(char*)symbol.curr;_tmp54C == (char*)(_tag_fat(0,0,0)).curr;})||({int(*_tmp54B)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp54A=defined_symbols;_tmp54B(_tmp54A,({struct _fat_ptr*_tmp29B=_cycalloc(sizeof(*_tmp29B));*_tmp29B=symbol;_tmp29B;}));})))
# 1768
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);else{
# 1770
({struct Cyc_String_pa_PrintArg_struct _tmp29E=({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0,_tmp3CE.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp3CE;});void*_tmp29C[1];_tmp29C[0]=& _tmp29E;({struct _fat_ptr _tmp54E=({const char*_tmp29D="%s is not supported on this platform\n";_tag_fat(_tmp29D,sizeof(char),38U);});Cyc_log(_tmp54E,_tag_fat(_tmp29C,sizeof(void*),1));});});}}}}
# 1773
({void*_tmp29F=0U;({struct Cyc___cycFILE*_tmp550=out_file;struct _fat_ptr _tmp54F=({const char*_tmp2A0="#endif\n";_tag_fat(_tmp2A0,sizeof(char),8U);});Cyc_fprintf(_tmp550,_tmp54F,_tag_fat(_tmp29F,sizeof(void*),0));});});
if(Cyc_do_setjmp){int _tmp2A1=0;_npop_handler(0);return _tmp2A1;}else{
Cyc_fclose(out_file);}
# 1778
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2A2=cstubs;struct Cyc_List_List*x=_tmp2A2;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp2A3=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp22=_tmp2A3;struct _tuple15*_tmp2A4=_stmttmp22;struct _fat_ptr _tmp2A6;struct _fat_ptr _tmp2A5;_tmp2A5=_tmp2A4->f1;_tmp2A6=_tmp2A4->f2;{struct _fat_ptr symbol=_tmp2A5;struct _fat_ptr text=_tmp2A6;
if(({char*_tmp554=(char*)text.curr;_tmp554 != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp553=(char*)symbol.curr;_tmp553 == (char*)(_tag_fat(0,0,0)).curr;})||({int(*_tmp552)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp551=defined_symbols;_tmp552(_tmp551,({struct _fat_ptr*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));*_tmp2A7=symbol;_tmp2A7;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}}
# 1789
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
if(cycstubs != 0){
# 1794
({struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0,({struct _fat_ptr _tmp555=(struct _fat_ptr)({const char*_tmp2AB=filename;_tag_fat(_tmp2AB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AB,1U));});_tmp3CF.f1=_tmp555;});_tmp3CF;});void*_tmp2A8[1];_tmp2A8[0]=& _tmp2AA;({struct Cyc___cycFILE*_tmp557=out_file;struct _fat_ptr _tmp556=({const char*_tmp2A9="#include <%s>\n\n";_tag_fat(_tmp2A9,sizeof(char),16U);});Cyc_fprintf(_tmp557,_tmp556,_tag_fat(_tmp2A8,sizeof(void*),1));});});
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2AC=cycstubs;struct Cyc_List_List*x=_tmp2AC;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp2AD=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp23=_tmp2AD;struct _tuple15*_tmp2AE=_stmttmp23;struct _fat_ptr _tmp2B0;struct _fat_ptr _tmp2AF;_tmp2AF=_tmp2AE->f1;_tmp2B0=_tmp2AE->f2;{struct _fat_ptr symbol=_tmp2AF;struct _fat_ptr text=_tmp2B0;
if(({char*_tmp55B=(char*)text.curr;_tmp55B != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp55A=(char*)symbol.curr;_tmp55A == (char*)(_tag_fat(0,0,0)).curr;})||({int(*_tmp559)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp558=defined_symbols;_tmp559(_tmp558,({struct _fat_ptr*_tmp2B1=_cycalloc(sizeof(*_tmp2B1));*_tmp2B1=symbol;_tmp2B1;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}
# 1802
({void*_tmp2B2=0U;({struct Cyc___cycFILE*_tmp55D=out_file;struct _fat_ptr _tmp55C=({const char*_tmp2B3="\n";_tag_fat(_tmp2B3,sizeof(char),2U);});Cyc_fprintf(_tmp55D,_tmp55C,_tag_fat(_tmp2B2,sizeof(void*),0));});});}{
# 1805
int _tmp2B4=0;_npop_handler(0);return _tmp2B4;}}}}}}}}}
# 1435
;_pop_handler();}else{void*_tmp1B0=(void*)Cyc_Core_get_exn_thrown();void*_tmp2B9=_tmp1B0;void*_tmp2BA;struct _fat_ptr _tmp2BB;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2B9)->tag == Cyc_Core_Impossible){_tmp2BB=((struct Cyc_Core_Impossible_exn_struct*)_tmp2B9)->f1;{struct _fat_ptr s=_tmp2BB;
# 1809
({struct Cyc_String_pa_PrintArg_struct _tmp2BE=({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0,_tmp3D1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D1;});void*_tmp2BC[1];_tmp2BC[0]=& _tmp2BE;({struct Cyc___cycFILE*_tmp55F=Cyc_stderr;struct _fat_ptr _tmp55E=({const char*_tmp2BD="Got Core::Impossible(%s)\n";_tag_fat(_tmp2BD,sizeof(char),26U);});Cyc_fprintf(_tmp55F,_tmp55E,_tag_fat(_tmp2BC,sizeof(void*),1));});});goto _LL86;}}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2B9)->tag == Cyc_Dict_Absent){
# 1811
({void*_tmp2BF=0U;({struct Cyc___cycFILE*_tmp561=Cyc_stderr;struct _fat_ptr _tmp560=({const char*_tmp2C0="Got Dict::Absent\n";_tag_fat(_tmp2C0,sizeof(char),18U);});Cyc_fprintf(_tmp561,_tmp560,_tag_fat(_tmp2BF,sizeof(void*),0));});});goto _LL86;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2B9)->tag == Cyc_Core_Failure){_tmp2BB=((struct Cyc_Core_Failure_exn_struct*)_tmp2B9)->f1;{struct _fat_ptr s=_tmp2BB;
# 1813
({struct Cyc_String_pa_PrintArg_struct _tmp2C3=({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0,_tmp3D2.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D2;});void*_tmp2C1[1];_tmp2C1[0]=& _tmp2C3;({struct Cyc___cycFILE*_tmp563=Cyc_stderr;struct _fat_ptr _tmp562=({const char*_tmp2C2="Got Core::Failure(%s)\n";_tag_fat(_tmp2C2,sizeof(char),23U);});Cyc_fprintf(_tmp563,_tmp562,_tag_fat(_tmp2C1,sizeof(void*),1));});});goto _LL86;}}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2B9)->tag == Cyc_Core_Invalid_argument){_tmp2BB=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2B9)->f1;{struct _fat_ptr s=_tmp2BB;
# 1815
({struct Cyc_String_pa_PrintArg_struct _tmp2C6=({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0,_tmp3D3.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D3;});void*_tmp2C4[1];_tmp2C4[0]=& _tmp2C6;({struct Cyc___cycFILE*_tmp565=Cyc_stderr;struct _fat_ptr _tmp564=({const char*_tmp2C5="Got Invalid_argument(%s)\n";_tag_fat(_tmp2C5,sizeof(char),26U);});Cyc_fprintf(_tmp565,_tmp564,_tag_fat(_tmp2C4,sizeof(void*),1));});});goto _LL86;}}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2B9)->tag == Cyc_Core_Not_found){
# 1817
({void*_tmp2C7=0U;({struct Cyc___cycFILE*_tmp567=Cyc_stderr;struct _fat_ptr _tmp566=({const char*_tmp2C8="Got Not_found\n";_tag_fat(_tmp2C8,sizeof(char),15U);});Cyc_fprintf(_tmp567,_tmp566,_tag_fat(_tmp2C7,sizeof(void*),0));});});goto _LL86;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2B9)->tag == Cyc_NO_SUPPORT){_tmp2BB=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2B9)->f1;{struct _fat_ptr s=_tmp2BB;
# 1819
({struct Cyc_String_pa_PrintArg_struct _tmp2CB=({struct Cyc_String_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=0,_tmp3D4.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D4;});void*_tmp2C9[1];_tmp2C9[0]=& _tmp2CB;({struct Cyc___cycFILE*_tmp569=Cyc_stderr;struct _fat_ptr _tmp568=({const char*_tmp2CA="No support because %s\n";_tag_fat(_tmp2CA,sizeof(char),23U);});Cyc_fprintf(_tmp569,_tmp568,_tag_fat(_tmp2C9,sizeof(void*),1));});});goto _LL86;}}else{_tmp2BA=_tmp2B9;{void*x=_tmp2BA;
# 1821
({void*_tmp2CC=0U;({struct Cyc___cycFILE*_tmp56B=Cyc_stderr;struct _fat_ptr _tmp56A=({const char*_tmp2CD="Got unknown exception\n";_tag_fat(_tmp2CD,sizeof(char),23U);});Cyc_fprintf(_tmp56B,_tmp56A,_tag_fat(_tmp2CC,sizeof(void*),0));});});
Cyc_Core_rethrow(x);}}}}}}}_LL86:;}}}
# 1827
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2D0=({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0,({struct _fat_ptr _tmp56C=(struct _fat_ptr)({const char*_tmp2D1=filename;_tag_fat(_tmp2D1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D1,1U));});_tmp3D5.f1=_tmp56C;});_tmp3D5;});void*_tmp2CE[1];_tmp2CE[0]=& _tmp2D0;({struct Cyc___cycFILE*_tmp56E=Cyc_stderr;struct _fat_ptr _tmp56D=({const char*_tmp2CF="Error: could not create file %s\n";_tag_fat(_tmp2CF,sizeof(char),33U);});Cyc_fprintf(_tmp56E,_tmp56D,_tag_fat(_tmp2CE,sizeof(void*),1));});});
return 1;}
# 1832
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp2D4=({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0,({
# 1835
struct _fat_ptr _tmp56F=(struct _fat_ptr)({const char*_tmp2D5=filename;_tag_fat(_tmp2D5,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D5,1U));});_tmp3D6.f1=_tmp56F;});_tmp3D6;});void*_tmp2D2[1];_tmp2D2[0]=& _tmp2D4;({struct Cyc___cycFILE*_tmp571=out_file;struct _fat_ptr _tmp570=({const char*_tmp2D3="#error -- %s is not supported on this platform\n";_tag_fat(_tmp2D3,sizeof(char),48U);});Cyc_fprintf(_tmp571,_tmp570,_tag_fat(_tmp2D2,sizeof(void*),1));});});
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp2D8=({struct Cyc_String_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=0,({
struct _fat_ptr _tmp572=(struct _fat_ptr)({const char*_tmp2D9=filename;_tag_fat(_tmp2D9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D9,1U));});_tmp3D7.f1=_tmp572;});_tmp3D7;});void*_tmp2D6[1];_tmp2D6[0]=& _tmp2D8;({struct Cyc___cycFILE*_tmp574=Cyc_stderr;struct _fat_ptr _tmp573=({const char*_tmp2D7="Warning: %s will not be supported on this platform\n";_tag_fat(_tmp2D7,sizeof(char),52U);});Cyc_fprintf(_tmp574,_tmp573,_tag_fat(_tmp2D6,sizeof(void*),1));});});
({void*_tmp2DA=0U;({struct _fat_ptr _tmp575=({const char*_tmp2DB="Not supported on this platform\n";_tag_fat(_tmp2DB,sizeof(char),32U);});Cyc_log(_tmp575,_tag_fat(_tmp2DA,sizeof(void*),0));});});
# 1846
return 0;}}
# 1850
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2F3=({struct Cyc_String_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=0,({struct _fat_ptr _tmp576=(struct _fat_ptr)({const char*_tmp2F4=file;_tag_fat(_tmp2F4,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F4,1U));});_tmp3DE.f1=_tmp576;});_tmp3DE;});void*_tmp2F1[1];_tmp2F1[0]=& _tmp2F3;({struct Cyc___cycFILE*_tmp578=Cyc_stderr;struct _fat_ptr _tmp577=({const char*_tmp2F2="Processing %s\n";_tag_fat(_tmp2F2,sizeof(char),15U);});Cyc_fprintf(_tmp578,_tmp577,_tag_fat(_tmp2F1,sizeof(void*),1));});});{
struct Cyc___cycFILE*_tmp2F5=Cyc_fopen(file,"r");struct Cyc___cycFILE*maybe=_tmp2F5;
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2F8=({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0,({struct _fat_ptr _tmp579=(struct _fat_ptr)({const char*_tmp2F9=file;_tag_fat(_tmp2F9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F9,1U));});_tmp3DF.f1=_tmp579;});_tmp3DF;});void*_tmp2F6[1];_tmp2F6[0]=& _tmp2F8;({struct Cyc___cycFILE*_tmp57B=Cyc_stderr;struct _fat_ptr _tmp57A=({const char*_tmp2F7="Error: could not open %s\n";_tag_fat(_tmp2F7,sizeof(char),26U);});Cyc_fprintf(_tmp57B,_tmp57A,_tag_fat(_tmp2F6,sizeof(void*),1));});});
return 1;}{
# 1858
struct Cyc___cycFILE*_tmp2FA=maybe;struct Cyc___cycFILE*in_file=_tmp2FA;
# 1862
struct _fat_ptr buf=({char*_tmp318=({unsigned _tmp317=1024U + 1U;char*_tmp316=_cycalloc_atomic(_check_times(_tmp317,sizeof(char)));({{unsigned _tmp3E5=1024U;unsigned i;for(i=0;i < _tmp3E5;++ i){_tmp316[i]='\000';}_tmp316[_tmp3E5]=0;}0;});_tmp316;});_tag_fat(_tmp318,sizeof(char),1025U);});
struct _fat_ptr _tmp2FB=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp2FB;
if((int)Cyc_mode != 2){
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0,({struct _fat_ptr _tmp57C=(struct _fat_ptr)({const char*_tmp2FF=dir;_tag_fat(_tmp2FF,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FF,1U));});_tmp3E0.f1=_tmp57C;});_tmp3E0;});void*_tmp2FC[1];_tmp2FC[0]=& _tmp2FE;({struct Cyc___cycFILE*_tmp57E=Cyc_stderr;struct _fat_ptr _tmp57D=({const char*_tmp2FD="Error: can't change directory to %s\n";_tag_fat(_tmp2FD,sizeof(char),37U);});Cyc_fprintf(_tmp57E,_tmp57D,_tag_fat(_tmp2FC,sizeof(void*),1));});});
return 1;}}
# 1870
if((int)Cyc_mode == 1){
# 1872
struct _fat_ptr _tmp300=({struct Cyc_String_pa_PrintArg_struct _tmp306=({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0,_tmp3E3.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3E3;});struct Cyc_String_pa_PrintArg_struct _tmp307=({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0,_tmp3E2.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3E2;});void*_tmp304[2];_tmp304[0]=& _tmp306,_tmp304[1]=& _tmp307;({struct _fat_ptr _tmp57F=({const char*_tmp305="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp305,sizeof(char),39U);});Cyc_aprintf(_tmp57F,_tag_fat(_tmp304,sizeof(void*),2));});});struct _fat_ptr cmd=_tmp300;
# 1874
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp303=({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0,_tmp3E1.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp3E1;});void*_tmp301[1];_tmp301[0]=& _tmp303;({struct Cyc___cycFILE*_tmp581=Cyc_stderr;struct _fat_ptr _tmp580=({const char*_tmp302="%s\n";_tag_fat(_tmp302,sizeof(char),4U);});Cyc_fprintf(_tmp581,_tmp580,_tag_fat(_tmp301,sizeof(void*),1));});});
system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)));}{
# 1879
struct Cyc_Lexing_lexbuf*_tmp308=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp308;
struct _tuple17*entry;
while((entry=({(struct _tuple17*(*)(struct Cyc_Lexing_lexbuf*))Cyc_spec;})(l))!= 0){
struct _tuple17*_tmp309=(struct _tuple17*)_check_null(entry);struct _tuple17*_stmttmp24=_tmp309;struct _tuple17*_tmp30A=_stmttmp24;void*_tmp312;void*_tmp311;void*_tmp310;void*_tmp30F;void*_tmp30E;void*_tmp30D;void*_tmp30C;struct _fat_ptr _tmp30B;_tmp30B=_tmp30A->f1;_tmp30C=_tmp30A->f2;_tmp30D=_tmp30A->f3;_tmp30E=_tmp30A->f4;_tmp30F=_tmp30A->f5;_tmp310=_tmp30A->f6;_tmp311=_tmp30A->f7;_tmp312=_tmp30A->f8;{struct _fat_ptr headerfile=_tmp30B;struct Cyc_List_List*start_symbols=_tmp30C;struct Cyc_List_List*user_defs=_tmp30D;struct Cyc_List_List*omit_symbols=_tmp30E;struct Cyc_List_List*hstubs=_tmp30F;struct Cyc_List_List*cstubs=_tmp310;struct Cyc_List_List*cycstubs=_tmp311;struct Cyc_List_List*cpp_insert=_tmp312;
# 1884
if(Cyc_process_file((const char*)_check_null(_untag_fat_ptr(headerfile,sizeof(char),1U)),start_symbols,user_defs,omit_symbols,hstubs,cstubs,cycstubs,cpp_insert))
# 1886
return 1;}}
# 1888
Cyc_fclose(in_file);
# 1890
if((int)Cyc_mode != 2){
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp315=({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0,_tmp3E4.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp3E4;});void*_tmp313[1];_tmp313[0]=& _tmp315;({struct Cyc___cycFILE*_tmp583=Cyc_stderr;struct _fat_ptr _tmp582=({const char*_tmp314="Error: could not change directory to %s\n";_tag_fat(_tmp314,sizeof(char),41U);});Cyc_fprintf(_tmp583,_tmp582,_tag_fat(_tmp313,sizeof(void*),1));});});
return 1;}}
# 1896
return 0;}}}}
# 1900
int Cyc_process_setjmp(const char*dir){
# 1903
struct _fat_ptr buf=({char*_tmp32A=({unsigned _tmp329=1024U + 1U;char*_tmp328=_cycalloc_atomic(_check_times(_tmp329,sizeof(char)));({{unsigned _tmp3E8=1024U;unsigned i;for(i=0;i < _tmp3E8;++ i){_tmp328[i]='\000';}_tmp328[_tmp3E8]=0;}0;});_tmp328;});_tag_fat(_tmp32A,sizeof(char),1025U);});
struct _fat_ptr _tmp319=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp319;
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp31C=({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0,({struct _fat_ptr _tmp584=(struct _fat_ptr)({const char*_tmp31D=dir;_tag_fat(_tmp31D,sizeof(char),_get_zero_arr_size_char((void*)_tmp31D,1U));});_tmp3E6.f1=_tmp584;});_tmp3E6;});void*_tmp31A[1];_tmp31A[0]=& _tmp31C;({struct Cyc___cycFILE*_tmp586=Cyc_stderr;struct _fat_ptr _tmp585=({const char*_tmp31B="Error: can't change directory to %s\n";_tag_fat(_tmp31B,sizeof(char),37U);});Cyc_fprintf(_tmp586,_tmp585,_tag_fat(_tmp31A,sizeof(void*),1));});});
return 1;}
# 1909
if(({struct Cyc_List_List*_tmp58E=({struct _fat_ptr*_tmp31E[1];({struct _fat_ptr*_tmp588=({struct _fat_ptr*_tmp320=_cycalloc(sizeof(*_tmp320));({struct _fat_ptr _tmp587=({const char*_tmp31F="jmp_buf";_tag_fat(_tmp31F,sizeof(char),8U);});*_tmp320=_tmp587;});_tmp320;});_tmp31E[0]=_tmp588;});({struct Cyc_List_List*(*_tmp589)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp589(_tag_fat(_tmp31E,sizeof(struct _fat_ptr*),1));});});Cyc_process_file("setjmp.h",_tmp58E,0,0,({struct _tuple15*_tmp321[1];({struct _tuple15*_tmp58C=({struct _tuple15*_tmp324=_cycalloc(sizeof(*_tmp324));
({struct _fat_ptr _tmp58B=({const char*_tmp322="setjmp";_tag_fat(_tmp322,sizeof(char),7U);});_tmp324->f1=_tmp58B;}),({struct _fat_ptr _tmp58A=({const char*_tmp323="extern int setjmp(jmp_buf);\n";_tag_fat(_tmp323,sizeof(char),29U);});_tmp324->f2=_tmp58A;});_tmp324;});_tmp321[0]=_tmp58C;});({struct Cyc_List_List*(*_tmp58D)(struct _fat_ptr)=({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp58D(_tag_fat(_tmp321,sizeof(struct _tuple15*),1));});}),0,0,0);}))
# 1912
return 1;
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp327=({struct Cyc_String_pa_PrintArg_struct _tmp3E7;_tmp3E7.tag=0,_tmp3E7.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp3E7;});void*_tmp325[1];_tmp325[0]=& _tmp327;({struct Cyc___cycFILE*_tmp590=Cyc_stderr;struct _fat_ptr _tmp58F=({const char*_tmp326="Error: could not change directory to %s\n";_tag_fat(_tmp326,sizeof(char),41U);});Cyc_fprintf(_tmp590,_tmp58F,_tag_fat(_tmp325,sizeof(void*),1));});});
return 1;}
# 1917
return 0;}static char _tmp32B[13U]="BUILDLIB.OUT";
# 1921
static struct _fat_ptr Cyc_output_dir={_tmp32B,_tmp32B,_tmp32B + 13U};
static void Cyc_set_output_dir(struct _fat_ptr s){
Cyc_output_dir=s;}
# 1925
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _fat_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->hd=(const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)),_tmp32C->tl=Cyc_spec_files;_tmp32C;});}
# 1929
static int Cyc_no_other(struct _fat_ptr s){return 0;}
static void Cyc_set_GATHER (void){
Cyc_mode=1U;}
# 1933
static void Cyc_set_GATHERSCRIPT (void){
Cyc_mode=2U;}
# 1936
static void Cyc_set_FINISH (void){
Cyc_mode=3U;}
# 1939
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp32E=_cycalloc(sizeof(*_tmp32E));({struct _fat_ptr*_tmp591=({struct _fat_ptr*_tmp32D=_cycalloc(sizeof(*_tmp32D));*_tmp32D=s;_tmp32D;});_tmp32E->hd=_tmp591;}),_tmp32E->tl=Cyc_cppargs;_tmp32E;});}
# 1942
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp331=({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0,_tmp3E9.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3E9;});void*_tmp32F[1];_tmp32F[0]=& _tmp331;({struct Cyc___cycFILE*_tmp593=Cyc_stderr;struct _fat_ptr _tmp592=({const char*_tmp330="Unsupported option %s\n";_tag_fat(_tmp330,sizeof(char),23U);});Cyc_fprintf(_tmp593,_tmp592,_tag_fat(_tmp32F,sizeof(void*),1));});});
Cyc_badparse=1;}
# 1951
extern void GC_blacklist_warn_clear (void);struct _tuple19{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
int Cyc_main(int argc,struct _fat_ptr argv){
GC_blacklist_warn_clear();{
# 1955
struct Cyc_List_List*options=({struct _tuple19*_tmp369[9];({
struct _tuple19*_tmp5C0=({struct _tuple19*_tmp36E=_cycalloc(sizeof(*_tmp36E));({struct _fat_ptr _tmp5BF=({const char*_tmp36A="-d";_tag_fat(_tmp36A,sizeof(char),3U);});_tmp36E->f1=_tmp5BF;}),_tmp36E->f2=0,({struct _fat_ptr _tmp5BE=({const char*_tmp36B=" <file>";_tag_fat(_tmp36B,sizeof(char),8U);});_tmp36E->f3=_tmp5BE;}),({
void*_tmp5BD=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->tag=5,_tmp36C->f1=Cyc_set_output_dir;_tmp36C;});_tmp36E->f4=_tmp5BD;}),({
struct _fat_ptr _tmp5BC=({const char*_tmp36D="Set the output directory to <file>";_tag_fat(_tmp36D,sizeof(char),35U);});_tmp36E->f5=_tmp5BC;});_tmp36E;});
# 1956
_tmp369[0]=_tmp5C0;}),({
# 1959
struct _tuple19*_tmp5BB=({struct _tuple19*_tmp373=_cycalloc(sizeof(*_tmp373));({struct _fat_ptr _tmp5BA=({const char*_tmp36F="-gather";_tag_fat(_tmp36F,sizeof(char),8U);});_tmp373->f1=_tmp5BA;}),_tmp373->f2=0,({struct _fat_ptr _tmp5B9=({const char*_tmp370="";_tag_fat(_tmp370,sizeof(char),1U);});_tmp373->f3=_tmp5B9;}),({
void*_tmp5B8=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->tag=0,_tmp371->f1=Cyc_set_GATHER;_tmp371;});_tmp373->f4=_tmp5B8;}),({
struct _fat_ptr _tmp5B7=({const char*_tmp372="Gather C library info but don't produce Cyclone headers";_tag_fat(_tmp372,sizeof(char),56U);});_tmp373->f5=_tmp5B7;});_tmp373;});
# 1959
_tmp369[1]=_tmp5BB;}),({
# 1962
struct _tuple19*_tmp5B6=({struct _tuple19*_tmp378=_cycalloc(sizeof(*_tmp378));({struct _fat_ptr _tmp5B5=({const char*_tmp374="-gatherscript";_tag_fat(_tmp374,sizeof(char),14U);});_tmp378->f1=_tmp5B5;}),_tmp378->f2=0,({struct _fat_ptr _tmp5B4=({const char*_tmp375="";_tag_fat(_tmp375,sizeof(char),1U);});_tmp378->f3=_tmp5B4;}),({
void*_tmp5B3=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp376=_cycalloc(sizeof(*_tmp376));_tmp376->tag=0,_tmp376->f1=Cyc_set_GATHERSCRIPT;_tmp376;});_tmp378->f4=_tmp5B3;}),({
struct _fat_ptr _tmp5B2=({const char*_tmp377="Produce a script to gather C library info";_tag_fat(_tmp377,sizeof(char),42U);});_tmp378->f5=_tmp5B2;});_tmp378;});
# 1962
_tmp369[2]=_tmp5B6;}),({
# 1965
struct _tuple19*_tmp5B1=({struct _tuple19*_tmp37D=_cycalloc(sizeof(*_tmp37D));({struct _fat_ptr _tmp5B0=({const char*_tmp379="-finish";_tag_fat(_tmp379,sizeof(char),8U);});_tmp37D->f1=_tmp5B0;}),_tmp37D->f2=0,({struct _fat_ptr _tmp5AF=({const char*_tmp37A="";_tag_fat(_tmp37A,sizeof(char),1U);});_tmp37D->f3=_tmp5AF;}),({
void*_tmp5AE=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->tag=0,_tmp37B->f1=Cyc_set_FINISH;_tmp37B;});_tmp37D->f4=_tmp5AE;}),({
struct _fat_ptr _tmp5AD=({const char*_tmp37C="Produce Cyclone headers from pre-gathered C library info";_tag_fat(_tmp37C,sizeof(char),57U);});_tmp37D->f5=_tmp5AD;});_tmp37D;});
# 1965
_tmp369[3]=_tmp5B1;}),({
# 1968
struct _tuple19*_tmp5AC=({struct _tuple19*_tmp382=_cycalloc(sizeof(*_tmp382));({struct _fat_ptr _tmp5AB=({const char*_tmp37E="-setjmp";_tag_fat(_tmp37E,sizeof(char),8U);});_tmp382->f1=_tmp5AB;}),_tmp382->f2=0,({struct _fat_ptr _tmp5AA=({const char*_tmp37F="";_tag_fat(_tmp37F,sizeof(char),1U);});_tmp382->f3=_tmp5AA;}),({
void*_tmp5A9=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->tag=3,_tmp380->f1=& Cyc_do_setjmp;_tmp380;});_tmp382->f4=_tmp5A9;}),({
# 1973
struct _fat_ptr _tmp5A8=({const char*_tmp381="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_fat(_tmp381,sizeof(char),186U);});_tmp382->f5=_tmp5A8;});_tmp382;});
# 1968
_tmp369[4]=_tmp5AC;}),({
# 1974
struct _tuple19*_tmp5A7=({struct _tuple19*_tmp387=_cycalloc(sizeof(*_tmp387));({struct _fat_ptr _tmp5A6=({const char*_tmp383="-b";_tag_fat(_tmp383,sizeof(char),3U);});_tmp387->f1=_tmp5A6;}),_tmp387->f2=0,({struct _fat_ptr _tmp5A5=({const char*_tmp384=" <machine>";_tag_fat(_tmp384,sizeof(char),11U);});_tmp387->f3=_tmp5A5;}),({
void*_tmp5A4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->tag=5,_tmp385->f1=Cyc_Specsfile_set_target_arch;_tmp385;});_tmp387->f4=_tmp5A4;}),({
struct _fat_ptr _tmp5A3=({const char*_tmp386="Set the target machine for compilation to <machine>";_tag_fat(_tmp386,sizeof(char),52U);});_tmp387->f5=_tmp5A3;});_tmp387;});
# 1974
_tmp369[5]=_tmp5A7;}),({
# 1977
struct _tuple19*_tmp5A2=({struct _tuple19*_tmp38C=_cycalloc(sizeof(*_tmp38C));({struct _fat_ptr _tmp5A1=({const char*_tmp388="-B";_tag_fat(_tmp388,sizeof(char),3U);});_tmp38C->f1=_tmp5A1;}),_tmp38C->f2=1,({struct _fat_ptr _tmp5A0=({const char*_tmp389="<file>";_tag_fat(_tmp389,sizeof(char),7U);});_tmp38C->f3=_tmp5A0;}),({
void*_tmp59F=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp38A=_cycalloc(sizeof(*_tmp38A));_tmp38A->tag=1,_tmp38A->f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp38A;});_tmp38C->f4=_tmp59F;}),({
struct _fat_ptr _tmp59E=({const char*_tmp38B="Add to the list of directories to search for compiler files";_tag_fat(_tmp38B,sizeof(char),60U);});_tmp38C->f5=_tmp59E;});_tmp38C;});
# 1977
_tmp369[6]=_tmp5A2;}),({
# 1980
struct _tuple19*_tmp59D=({struct _tuple19*_tmp391=_cycalloc(sizeof(*_tmp391));({struct _fat_ptr _tmp59C=({const char*_tmp38D="-v";_tag_fat(_tmp38D,sizeof(char),3U);});_tmp391->f1=_tmp59C;}),_tmp391->f2=0,({struct _fat_ptr _tmp59B=({const char*_tmp38E="";_tag_fat(_tmp38E,sizeof(char),1U);});_tmp391->f3=_tmp59B;}),({
void*_tmp59A=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F->tag=3,_tmp38F->f1=& Cyc_verbose;_tmp38F;});_tmp391->f4=_tmp59A;}),({
struct _fat_ptr _tmp599=({const char*_tmp390="Verbose operation";_tag_fat(_tmp390,sizeof(char),18U);});_tmp391->f5=_tmp599;});_tmp391;});
# 1980
_tmp369[7]=_tmp59D;}),({
# 1983
struct _tuple19*_tmp598=({struct _tuple19*_tmp396=_cycalloc(sizeof(*_tmp396));({struct _fat_ptr _tmp597=({const char*_tmp392="-";_tag_fat(_tmp392,sizeof(char),2U);});_tmp396->f1=_tmp597;}),_tmp396->f2=1,({struct _fat_ptr _tmp596=({const char*_tmp393="";_tag_fat(_tmp393,sizeof(char),1U);});_tmp396->f3=_tmp596;}),({
void*_tmp595=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->tag=1,_tmp394->f1=Cyc_add_cpparg;_tmp394;});_tmp396->f4=_tmp595;}),({
struct _fat_ptr _tmp594=({const char*_tmp395="";_tag_fat(_tmp395,sizeof(char),1U);});_tmp396->f5=_tmp594;});_tmp396;});
# 1983
_tmp369[8]=_tmp598;});({struct Cyc_List_List*(*_tmp5C1)(struct _fat_ptr)=({
# 1956
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;});_tmp5C1(_tag_fat(_tmp369,sizeof(struct _tuple19*),9));});});
# 1988
struct _fat_ptr _tmp332=({struct Cyc_List_List*_tmp5C3=options;struct _fat_ptr _tmp5C2=({const char*_tmp368="Options:";_tag_fat(_tmp368,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp5C3,Cyc_add_spec_file,Cyc_no_other,_tmp5C2,argv);});struct _fat_ptr otherargs=_tmp332;
# 1990
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp5C5=options;struct _fat_ptr _tmp5C4=({const char*_tmp333="Options:";_tag_fat(_tmp333,sizeof(char),9U);});Cyc_Arg_parse(_tmp5C5,Cyc_add_spec_file,Cyc_no_other,_tmp5C4,otherargs);});
if((((Cyc_badparse ||
 !Cyc_do_setjmp && Cyc_spec_files == 0)||
 Cyc_do_setjmp && Cyc_spec_files != 0)||
 Cyc_do_setjmp &&(int)Cyc_mode == 1)||
 Cyc_do_setjmp &&(int)Cyc_mode == 2){
({struct Cyc_List_List*_tmp5C6=options;Cyc_Arg_usage(_tmp5C6,({const char*_tmp334="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_fat(_tmp334,sizeof(char),59U);}));});
# 2000
return 1;}{
# 2006
struct _fat_ptr _tmp335=Cyc_Specsfile_find_in_arch_path(({const char*_tmp367="cycspecs";_tag_fat(_tmp367,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp335;
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp338=({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0,_tmp3EA.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp3EA;});void*_tmp336[1];_tmp336[0]=& _tmp338;({struct Cyc___cycFILE*_tmp5C8=Cyc_stderr;struct _fat_ptr _tmp5C7=({const char*_tmp337="Reading from specs file %s\n";_tag_fat(_tmp337,sizeof(char),28U);});Cyc_fprintf(_tmp5C8,_tmp5C7,_tag_fat(_tmp336,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp339=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp339;
Cyc_target_cflags=({struct Cyc_List_List*_tmp5C9=specs;Cyc_Specsfile_get_spec(_tmp5C9,({const char*_tmp33A="cyclone_target_cflags";_tag_fat(_tmp33A,sizeof(char),22U);}));});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp33D=({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0,_tmp3EB.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3EB;});void*_tmp33B[1];_tmp33B[0]=& _tmp33D;({struct Cyc___cycFILE*_tmp5CB=Cyc_stderr;struct _fat_ptr _tmp5CA=({const char*_tmp33C="Target cflags are %s\n";_tag_fat(_tmp33C,sizeof(char),22U);});Cyc_fprintf(_tmp5CB,_tmp5CA,_tag_fat(_tmp33B,sizeof(void*),1));});});
Cyc_cyclone_cc=({struct Cyc_List_List*_tmp5CC=specs;Cyc_Specsfile_get_spec(_tmp5CC,({const char*_tmp33E="cyclone_cc";_tag_fat(_tmp33E,sizeof(char),11U);}));});
if(!((unsigned)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp33F="gcc";_tag_fat(_tmp33F,sizeof(char),4U);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp342=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0,_tmp3EC.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3EC;});void*_tmp340[1];_tmp340[0]=& _tmp342;({struct Cyc___cycFILE*_tmp5CE=Cyc_stderr;struct _fat_ptr _tmp5CD=({const char*_tmp341="C compiler is %s\n";_tag_fat(_tmp341,sizeof(char),18U);});Cyc_fprintf(_tmp5CE,_tmp5CD,_tag_fat(_tmp340,sizeof(void*),1));});});
# 2015
if((int)Cyc_mode == 2){
if(Cyc_verbose)
({void*_tmp343=0U;({struct Cyc___cycFILE*_tmp5D0=Cyc_stderr;struct _fat_ptr _tmp5CF=({const char*_tmp344="Creating BUILDLIB.sh\n";_tag_fat(_tmp344,sizeof(char),22U);});Cyc_fprintf(_tmp5D0,_tmp5CF,_tag_fat(_tmp343,sizeof(void*),0));});});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned)Cyc_script_file)){
({void*_tmp345=0U;({struct Cyc___cycFILE*_tmp5D2=Cyc_stderr;struct _fat_ptr _tmp5D1=({const char*_tmp346="Could not create file BUILDLIB.sh\n";_tag_fat(_tmp346,sizeof(char),35U);});Cyc_fprintf(_tmp5D2,_tmp5D1,_tag_fat(_tmp345,sizeof(void*),0));});});
exit(1);}
# 2023
({void*_tmp347=0U;({struct _fat_ptr _tmp5D3=({const char*_tmp348="#!/bin/sh\n";_tag_fat(_tmp348,sizeof(char),11U);});Cyc_prscript(_tmp5D3,_tag_fat(_tmp347,sizeof(void*),0));});});
({void*_tmp349=0U;({struct _fat_ptr _tmp5D4=({const char*_tmp34A="GCC=\"gcc\"\n";_tag_fat(_tmp34A,sizeof(char),11U);});Cyc_prscript(_tmp5D4,_tag_fat(_tmp349,sizeof(void*),0));});});}
# 2028
if(Cyc_force_directory_prefixes(Cyc_output_dir)|| Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp34D=({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0,_tmp3ED.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3ED;});void*_tmp34B[1];_tmp34B[0]=& _tmp34D;({struct Cyc___cycFILE*_tmp5D6=Cyc_stderr;struct _fat_ptr _tmp5D5=({const char*_tmp34C="Error: could not create directory %s\n";_tag_fat(_tmp34C,sizeof(char),38U);});Cyc_fprintf(_tmp5D6,_tmp5D5,_tag_fat(_tmp34B,sizeof(void*),1));});});
return 1;}
# 2032
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp350=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0,_tmp3EE.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3EE;});void*_tmp34E[1];_tmp34E[0]=& _tmp350;({struct Cyc___cycFILE*_tmp5D8=Cyc_stderr;struct _fat_ptr _tmp5D7=({const char*_tmp34F="Output directory is %s\n";_tag_fat(_tmp34F,sizeof(char),24U);});Cyc_fprintf(_tmp5D8,_tmp5D7,_tag_fat(_tmp34E,sizeof(void*),1));});});
# 2035
if((int)Cyc_mode == 2){
({struct Cyc_String_pa_PrintArg_struct _tmp353=({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0,_tmp3EF.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3EF;});void*_tmp351[1];_tmp351[0]=& _tmp353;({struct _fat_ptr _tmp5D9=({const char*_tmp352="cd %s\n";_tag_fat(_tmp352,sizeof(char),7U);});Cyc_prscript(_tmp5D9,_tag_fat(_tmp351,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp356=({struct Cyc_String_pa_PrintArg_struct _tmp3F0;_tmp3F0.tag=0,_tmp3F0.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3F0;});void*_tmp354[1];_tmp354[0]=& _tmp356;({struct _fat_ptr _tmp5DA=({const char*_tmp355="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp355,sizeof(char),41U);});Cyc_prscript(_tmp5DA,_tag_fat(_tmp354,sizeof(void*),1));});});}
# 2041
if(!Cyc_gathering()){
# 2044
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp5DB=Cyc_output_dir;Cyc_Filename_concat(_tmp5DB,({const char*_tmp357="BUILDLIB.LOG";_tag_fat(_tmp357,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp35A=({struct Cyc_String_pa_PrintArg_struct _tmp3F1;_tmp3F1.tag=0,_tmp3F1.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F1;});void*_tmp358[1];_tmp358[0]=& _tmp35A;({struct Cyc___cycFILE*_tmp5DD=Cyc_stderr;struct _fat_ptr _tmp5DC=({const char*_tmp359="Error: could not create log file in directory %s\n";_tag_fat(_tmp359,sizeof(char),50U);});Cyc_fprintf(_tmp5DD,_tmp5DC,_tag_fat(_tmp358,sizeof(void*),1));});});
return 1;}
# 2050
if(!Cyc_do_setjmp){
# 2052
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp5DE=Cyc_output_dir;Cyc_Filename_concat(_tmp5DE,({const char*_tmp35B="cstubs.c";_tag_fat(_tmp35B,sizeof(char),9U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp35E=({struct Cyc_String_pa_PrintArg_struct _tmp3F2;_tmp3F2.tag=0,_tmp3F2.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F2;});void*_tmp35C[1];_tmp35C[0]=& _tmp35E;({struct Cyc___cycFILE*_tmp5E0=Cyc_stderr;struct _fat_ptr _tmp5DF=({const char*_tmp35D="Error: could not create cstubs.c in directory %s\n";_tag_fat(_tmp35D,sizeof(char),50U);});Cyc_fprintf(_tmp5E0,_tmp5DF,_tag_fat(_tmp35C,sizeof(void*),1));});});
return 1;}
# 2059
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp5E1=Cyc_output_dir;Cyc_Filename_concat(_tmp5E1,({const char*_tmp35F="cycstubs.cyc";_tag_fat(_tmp35F,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp362=({struct Cyc_String_pa_PrintArg_struct _tmp3F3;_tmp3F3.tag=0,_tmp3F3.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F3;});void*_tmp360[1];_tmp360[0]=& _tmp362;({struct Cyc___cycFILE*_tmp5E3=Cyc_stderr;struct _fat_ptr _tmp5E2=({const char*_tmp361="Error: could not create cycstubs.c in directory %s\n";_tag_fat(_tmp361,sizeof(char),52U);});Cyc_fprintf(_tmp5E3,_tmp5E2,_tag_fat(_tmp360,sizeof(void*),1));});});
# 2064
return 1;}
# 2066
({void*_tmp363=0U;({struct Cyc___cycFILE*_tmp5E5=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _fat_ptr _tmp5E4=({const char*_tmp364="#include <core.h>\nusing Core;\n\n";_tag_fat(_tmp364,sizeof(char),32U);});Cyc_fprintf(_tmp5E5,_tmp5E4,_tag_fat(_tmp363,sizeof(void*),0));});});}}{
# 2073
const char*outdir=(const char*)_check_null(_untag_fat_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp && Cyc_process_setjmp(outdir))
return 1;else{
# 2079
for(1;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp365=0U;({struct Cyc___cycFILE*_tmp5E7=Cyc_stderr;struct _fat_ptr _tmp5E6=({const char*_tmp366="FATAL ERROR -- QUIT!\n";_tag_fat(_tmp366,sizeof(char),22U);});Cyc_fprintf(_tmp5E7,_tmp5E6,_tag_fat(_tmp365,sizeof(void*),0));});});
exit(1);}}}
# 2087
if((int)Cyc_mode == 2)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2090
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2098
return 0;}}}}}
