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
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 173
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 280 "core.h"
extern void Cyc_Core_rethrow(void*);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};
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
extern int mkdir(const char*pathname,unsigned short mode);
# 334
extern int close(int);
extern int chdir(const char*);
extern struct _fat_ptr Cyc_getcwd(struct _fat_ptr buf,unsigned long size);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _fat_ptr lex_base;struct _fat_ptr lex_backtrk;struct _fat_ptr lex_default;struct _fat_ptr lex_trans;struct _fat_ptr lex_check;};
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
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
extern struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 94
extern int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
extern struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
extern int Cyc_strncmp(struct _fat_ptr s1,struct _fat_ptr s2,unsigned long len);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr src);
# 109
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*p);
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
extern void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr args);struct Cyc_Buffer_t;
# 50 "buffer.h"
extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned n);
# 58
extern struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 81
extern void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 92 "buffer.h"
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 28 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1086 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1140
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1146
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1154
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 28 "parse.h"
extern struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;};struct _tuple14{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple13{struct _tuple13*tl;struct _tuple14 hd  __attribute__((aligned )) ;};
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc =0U,Cyc_Extern_sc =1U,Cyc_ExternC_sc =2U,Cyc_Static_sc =3U,Cyc_Auto_sc =4U,Cyc_Register_sc =5U,Cyc_Abstract_sc =6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple15{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple15 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple16{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple17{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple14 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple19{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple20 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple9*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple21{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple22{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY60{int tag;struct _tuple0*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple24{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 31 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 30 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 29 "specsfile.h"
struct _fat_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _fat_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr s);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr file);
# 36
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _fat_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _fat_ptr Cyc_Specsfile_def_lib_path;
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr argv);
# 44
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr s);
# 80 "buildlib.cyl"
extern void Cyc_Lex_lex_init(int use_cyclone_keywords);static char _tmp0[4U]="gcc";
# 90
static struct _fat_ptr Cyc_cyclone_cc={_tmp0,_tmp0,_tmp0 + 4U};static char _tmp1[1U]="";
static struct _fat_ptr Cyc_target_cflags={_tmp1,_tmp1,_tmp1 + 1U};
# 93
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
int Cyc_noexpand_r=1;
# 97
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 101
int Cyc_log(struct _fat_ptr fmt,struct _fat_ptr ap){
# 104
if(Cyc_log_file == 0){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp430=Cyc_stderr;struct _fat_ptr _tmp42F=({const char*_tmp3="Internal error: log file is NULL\n";_tag_fat(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp430,_tmp42F,_tag_fat(_tmp2,sizeof(void*),0U));});});
exit(1);}{
# 108
int _tmp4=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);int x=_tmp4;
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return x;}}
# 113
static struct _fat_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _fat_ptr*sptr){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp431=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);*_tmp5=_tmp431;});_tmp5;});}struct _tuple25{struct _fat_ptr*f1;struct Cyc_Set_Set*f2;};
# 121
struct _tuple25*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 127
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple26{struct _fat_ptr f1;struct _fat_ptr*f2;};
# 135
struct _tuple26*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _fat_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple27{struct _fat_ptr f1;struct _fat_ptr f2;};struct _tuple28{struct _fat_ptr*f1;struct _fat_ptr*f2;};struct _tuple29{struct _fat_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;struct Cyc_List_List*f8;};
# 151
struct _tuple29*Cyc_spec(struct Cyc_Lexing_lexbuf*);
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
# 172
int Cyc_numdef=0;
# 174
static struct Cyc_List_List*Cyc_cppargs=0;static char _tmp6[14U]="BUILDLIB_sym_";
# 176
struct _fat_ptr Cyc_user_prefix={_tmp6,_tmp6,_tmp6 + 14U};
static struct _fat_ptr*Cyc_add_user_prefix(struct _fat_ptr*symbol){
struct _fat_ptr s=(struct _fat_ptr)Cyc_strconcat((struct _fat_ptr)Cyc_user_prefix,(struct _fat_ptr)*symbol);
return({struct _fat_ptr*_tmp7=_cycalloc(sizeof(*_tmp7));*_tmp7=s;_tmp7;});}
# 181
static struct _fat_ptr Cyc_remove_user_prefix(struct _fat_ptr symbol){
unsigned prefix_len=Cyc_strlen((struct _fat_ptr)Cyc_user_prefix);
if(Cyc_strncmp((struct _fat_ptr)symbol,(struct _fat_ptr)Cyc_user_prefix,prefix_len)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0U,_tmp3D2.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp3D2;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp433=Cyc_stderr;struct _fat_ptr _tmp432=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_fat(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp433,_tmp432,_tag_fat(_tmp8,sizeof(void*),1U));});});
return symbol;}
# 187
return(struct _fat_ptr)({struct _fat_ptr _tmp436=(struct _fat_ptr)symbol;int _tmp435=(int)prefix_len;Cyc_substring(_tmp436,_tmp435,({unsigned long _tmp434=Cyc_strlen((struct _fat_ptr)symbol);_tmp434 - prefix_len;}));});}
# 190
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;struct Cyc_Absyn_Typedefdecl*_tmpD;struct Cyc_Absyn_Enumdecl*_tmpE;struct Cyc_Absyn_Aggrdecl*_tmpF;switch(*((int*)_tmpC)){case 5U: _LL1: _tmpF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF;
# 193
({struct _fat_ptr*_tmp438=({struct _fat_ptr*_tmp10=_cycalloc(sizeof(*_tmp10));({struct _fat_ptr _tmp437=Cyc_remove_user_prefix(*(*ad->name).f2);*_tmp10=_tmp437;});_tmp10;});(*ad->name).f2=_tmp438;});
goto _LL0;}case 7U: _LL3: _tmpE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4: {struct Cyc_Absyn_Enumdecl*ed=_tmpE;
# 196
({struct _fat_ptr*_tmp43A=({struct _fat_ptr*_tmp11=_cycalloc(sizeof(*_tmp11));({struct _fat_ptr _tmp439=Cyc_remove_user_prefix(*(*ed->name).f2);*_tmp11=_tmp439;});_tmp11;});(*ed->name).f2=_tmp43A;});
goto _LL0;}case 8U: _LL5: _tmpD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmpD;
# 199
({struct _fat_ptr*_tmp43C=({struct _fat_ptr*_tmp12=_cycalloc(sizeof(*_tmp12));({struct _fat_ptr _tmp43B=Cyc_remove_user_prefix(*(*td->name).f2);*_tmp12=_tmp43B;});_tmp12;});(*td->name).f2=_tmp43C;});
goto _LL0;}case 1U: _LL7: _LL8:
# 202
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 4U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 2U: _LLF: _LL10:
 goto _LL12;case 3U: _LL11: _LL12:
 goto _LL14;case 9U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 216
({void*_tmp13=0U;({struct Cyc___cycFILE*_tmp43E=Cyc_stderr;struct _fat_ptr _tmp43D=({const char*_tmp14="Error in .cys file: bad user-defined type definition\n";_tag_fat(_tmp14,sizeof(char),54U);});Cyc_fprintf(_tmp43E,_tmp43D,_tag_fat(_tmp13,sizeof(void*),0U));});});
exit(1);}_LL0:;}
# 223
const int Cyc_lex_base[431U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431U]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 230
int state;int base;int backtrk;
int c;
state=start_state;
# 234
if(state >= 0){
({int _tmp43F=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp43F;});
lbuf->lex_last_action=- 1;}else{
# 238
state=(- state)- 1;}
# 240
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,431U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 248
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 252
c=256;}}else{
# 254
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 257
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,3846U,sizeof(int),base + c))== state)
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,3846U,sizeof(int),base + c));else{
# 260
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp440=({const char*_tmp15="empty token";_tag_fat(_tmp15,sizeof(char),12U);});_tmp16->f1=_tmp440;});_tmp16;}));else{
# 266
return lbuf->lex_last_action;}}else{
# 269
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 273
struct _tuple25*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp17=lexstate;switch(_tmp17){case 0U: _LL1: _LL2:
# 232 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp18=_cycalloc(sizeof(*_tmp18));({struct Cyc_Set_Set*_tmp442=({struct Cyc_Set_Set*_tmp441=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_delete)(_tmp441,(struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});*_tmp18=_tmp442;});_tmp18;});}
# 237
return({struct _tuple25*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->f1=(struct _fat_ptr*)_check_null(Cyc_current_source),_tmp19->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp19;});case 1U: _LL3: _LL4:
# 240 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2U: _LL5: _LL6:
# 242
 return 0;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 246
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp443=({const char*_tmp1A="some action didn't return!";_tag_fat(_tmp1A,sizeof(char),27U);});_tmp1B->f1=_tmp443;});_tmp1B;}));}
# 248
struct _tuple25*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp1C=lexstate;switch(_tmp1C){case 0U: _LL1: _LL2:
# 246 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct _fat_ptr _tmp446=(struct _fat_ptr)({struct _fat_ptr _tmp445=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp445,0,(unsigned long)(({
int _tmp444=Cyc_Lexing_lexeme_end(lexbuf);_tmp444 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 246
*_tmp1D=_tmp446;});_tmp1D;});
# 248
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct Cyc_Set_Set*_tmp447=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp1E=_tmp447;});_tmp1E;});
Cyc_token(lexbuf);
return 0;case 1U: _LL3: _LL4:
# 254
 Cyc_current_source=({struct _fat_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));({struct _fat_ptr _tmp44A=(struct _fat_ptr)({struct _fat_ptr _tmp449=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp449,0,(unsigned long)(({
int _tmp448=Cyc_Lexing_lexeme_end(lexbuf);_tmp448 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 254
*_tmp1F=_tmp44A;});_tmp1F;});
# 256
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp20=_cycalloc(sizeof(*_tmp20));({struct Cyc_Set_Set*_tmp44B=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp20=_tmp44B;});_tmp20;});
Cyc_args(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 262
 Cyc_current_source=({struct _fat_ptr*_tmp21=_cycalloc(sizeof(*_tmp21));({struct _fat_ptr _tmp44C=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp21=_tmp44C;});_tmp21;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp22=_cycalloc(sizeof(*_tmp22));({struct Cyc_Set_Set*_tmp44D=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp22=_tmp44D;});_tmp22;});
Cyc_token(lexbuf);
return 0;default: _LL7: _LL8:
# 268
((lexbuf->refill_buff))(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 271
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp44E=({const char*_tmp23="some action didn't return!";_tag_fat(_tmp23,sizeof(char),27U);});_tmp24->f1=_tmp44E;});_tmp24;}));}
# 273
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp25=lexstate;switch(_tmp25){case 0U: _LL1: _LL2: {
# 271 "buildlib.cyl"
struct _fat_ptr*_tmp26=({struct _fat_ptr*_tmp28=_cycalloc(sizeof(*_tmp28));({struct _fat_ptr _tmp451=(struct _fat_ptr)({struct _fat_ptr _tmp450=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp450,0,(unsigned long)(({
int _tmp44F=Cyc_Lexing_lexeme_end(lexbuf);_tmp44F - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 271
*_tmp28=_tmp451;});_tmp28;});struct _fat_ptr*a=_tmp26;
# 273
Cyc_current_args=({struct Cyc_List_List*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=a,_tmp27->tl=Cyc_current_args;_tmp27;});
return Cyc_args(lexbuf);}case 1U: _LL3: _LL4: {
# 277
struct _fat_ptr*_tmp29=({struct _fat_ptr*_tmp2B=_cycalloc(sizeof(*_tmp2B));({struct _fat_ptr _tmp454=(struct _fat_ptr)({struct _fat_ptr _tmp453=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp453,0,(unsigned long)(({
int _tmp452=Cyc_Lexing_lexeme_end(lexbuf);_tmp452 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 277
*_tmp2B=_tmp454;});_tmp2B;});struct _fat_ptr*a=_tmp29;
# 279
Cyc_current_args=({struct Cyc_List_List*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=a,_tmp2A->tl=Cyc_current_args;_tmp2A;});
return Cyc_args(lexbuf);}case 2U: _LL5: _LL6: {
# 283
struct _fat_ptr*_tmp2C=({struct _fat_ptr*_tmp2E=_cycalloc(sizeof(*_tmp2E));({struct _fat_ptr _tmp457=(struct _fat_ptr)({struct _fat_ptr _tmp456=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp456,0,(unsigned long)(({
int _tmp455=Cyc_Lexing_lexeme_end(lexbuf);_tmp455 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 283
*_tmp2E=_tmp457;});_tmp2E;});struct _fat_ptr*a=_tmp2C;
# 285
Cyc_current_args=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=a,_tmp2D->tl=Cyc_current_args;_tmp2D;});
return Cyc_token(lexbuf);}default: _LL7: _LL8:
# 288
((lexbuf->refill_buff))(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL0:;}
# 291
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp458=({const char*_tmp2F="some action didn't return!";_tag_fat(_tmp2F,sizeof(char),27U);});_tmp30->f1=_tmp458;});_tmp30;}));}
# 293
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp31=lexstate;switch(_tmp31){case 0U: _LL1: _LL2:
# 292 "buildlib.cyl"
 Cyc_add_target(({struct _fat_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));({struct _fat_ptr _tmp459=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp32=_tmp459;});_tmp32;}));return Cyc_token(lexbuf);case 1U: _LL3: _LL4:
# 295 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 298 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3U: _LL7: _LL8:
# 301 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4U: _LL9: _LLA:
# 304 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5U: _LLB: _LLC:
# 306
 return Cyc_token(lexbuf);case 6U: _LLD: _LLE:
# 308
 return Cyc_token(lexbuf);case 7U: _LLF: _LL10:
# 310
 return Cyc_token(lexbuf);case 8U: _LL11: _LL12:
# 313 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9U: _LL13: _LL14:
# 316 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10U: _LL15: _LL16:
# 319 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11U: _LL17: _LL18:
# 321
 return Cyc_token(lexbuf);case 12U: _LL19: _LL1A:
# 323
 return Cyc_token(lexbuf);case 13U: _LL1B: _LL1C:
# 325
 return Cyc_token(lexbuf);case 14U: _LL1D: _LL1E:
# 327
 return Cyc_token(lexbuf);case 15U: _LL1F: _LL20:
# 329
 return Cyc_token(lexbuf);case 16U: _LL21: _LL22:
# 331
 return Cyc_token(lexbuf);case 17U: _LL23: _LL24:
# 333
 return Cyc_token(lexbuf);case 18U: _LL25: _LL26:
# 335
 return Cyc_token(lexbuf);case 19U: _LL27: _LL28:
# 337
 return Cyc_token(lexbuf);case 20U: _LL29: _LL2A:
# 339
 return Cyc_token(lexbuf);case 21U: _LL2B: _LL2C:
# 341
 return Cyc_token(lexbuf);case 22U: _LL2D: _LL2E:
# 343
 return Cyc_token(lexbuf);case 23U: _LL2F: _LL30:
# 345
 return Cyc_token(lexbuf);case 24U: _LL31: _LL32:
# 348 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25U: _LL33: _LL34:
# 350
 return Cyc_token(lexbuf);case 26U: _LL35: _LL36:
# 352
 return Cyc_token(lexbuf);case 27U: _LL37: _LL38:
# 354
 return Cyc_token(lexbuf);case 28U: _LL39: _LL3A:
# 356
 return Cyc_token(lexbuf);case 29U: _LL3B: _LL3C:
# 358
 return Cyc_token(lexbuf);case 30U: _LL3D: _LL3E:
# 360
 return Cyc_token(lexbuf);case 31U: _LL3F: _LL40:
# 362
 return Cyc_token(lexbuf);case 32U: _LL41: _LL42:
# 364
 return Cyc_token(lexbuf);case 33U: _LL43: _LL44:
# 366
 return Cyc_token(lexbuf);case 34U: _LL45: _LL46:
# 368
 return Cyc_token(lexbuf);case 35U: _LL47: _LL48:
# 370
 return Cyc_token(lexbuf);case 36U: _LL49: _LL4A:
# 372
 return Cyc_token(lexbuf);case 37U: _LL4B: _LL4C:
# 374
 return Cyc_token(lexbuf);case 38U: _LL4D: _LL4E:
# 376
 return Cyc_token(lexbuf);case 39U: _LL4F: _LL50:
# 378
 return Cyc_token(lexbuf);case 40U: _LL51: _LL52:
# 380
 return Cyc_token(lexbuf);case 41U: _LL53: _LL54:
# 382
 return Cyc_token(lexbuf);case 42U: _LL55: _LL56:
# 384
 return Cyc_token(lexbuf);case 43U: _LL57: _LL58:
# 386
 return Cyc_token(lexbuf);case 44U: _LL59: _LL5A:
# 388
 return Cyc_token(lexbuf);case 45U: _LL5B: _LL5C:
# 390
 return Cyc_token(lexbuf);case 46U: _LL5D: _LL5E:
# 392
 return Cyc_token(lexbuf);case 47U: _LL5F: _LL60:
# 394
 return Cyc_token(lexbuf);case 48U: _LL61: _LL62:
# 397 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL63: _LL64:
((lexbuf->refill_buff))(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL0:;}
# 401
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp45A=({const char*_tmp33="some action didn't return!";_tag_fat(_tmp33,sizeof(char),27U);});_tmp34->f1=_tmp45A;});_tmp34;}));}
# 403
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp35=lexstate;switch(_tmp35){case 0U: _LL1: _LL2:
# 402 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1U: _LL3: _LL4:
# 403 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 404 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3U: _LL7: _LL8:
# 405 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4U: _LL9: _LLA:
# 408 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5U: _LLB: _LLC:
# 411 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6U: _LLD: _LLE:
# 413
 return Cyc_string(lexbuf);case 7U: _LLF: _LL10:
# 414 "buildlib.cyl"
 return 0;case 8U: _LL11: _LL12:
# 415 "buildlib.cyl"
 return 0;case 9U: _LL13: _LL14:
# 416 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL15: _LL16:
((lexbuf->refill_buff))(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL0:;}
# 420
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp45B=({const char*_tmp36="some action didn't return!";_tag_fat(_tmp36,sizeof(char),27U);});_tmp37->f1=_tmp45B;});_tmp37;}));}
# 422
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp38=lexstate;switch(_tmp38){case 0U: _LL1: _LL2:
# 425 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 427
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2U: _LL5: _LL6:
# 434 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp39=0U;({struct _fat_ptr _tmp45C=({const char*_tmp3A="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp3A,sizeof(char),44U);});Cyc_log(_tmp45C,_tag_fat(_tmp39,sizeof(void*),0U));});});
return 1;case 3U: _LL7: _LL8:
# 440 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3B=0U;({struct _fat_ptr _tmp45D=({const char*_tmp3C="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp3C,sizeof(char),44U);});Cyc_log(_tmp45D,_tag_fat(_tmp3B,sizeof(void*),0U));});});
return 1;case 4U: _LL9: _LLA:
# 446 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3D=0U;({struct _fat_ptr _tmp45E=({const char*_tmp3E="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp3E,sizeof(char),44U);});Cyc_log(_tmp45E,_tag_fat(_tmp3D,sizeof(void*),0U));});});
return 1;case 5U: _LLB: _LLC:
# 452 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3F=0U;({struct _fat_ptr _tmp45F=({const char*_tmp40="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp40,sizeof(char),44U);});Cyc_log(_tmp45F,_tag_fat(_tmp3F,sizeof(void*),0U));});});
return 1;case 6U: _LLD: _LLE:
# 456
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp41=0U;({struct _fat_ptr _tmp460=({const char*_tmp42="Warning: use of region sidestepped\n";_tag_fat(_tmp42,sizeof(char),36U);});Cyc_log(_tmp460,_tag_fat(_tmp41,sizeof(void*),0U));});});
return 1;case 7U: _LLF: _LL10:
# 460
({void*_tmp43=0U;({struct _fat_ptr _tmp461=({const char*_tmp44="Warning: use of __extension__ deleted\n";_tag_fat(_tmp44,sizeof(char),39U);});Cyc_log(_tmp461,_tag_fat(_tmp43,sizeof(void*),0U));});});
return 1;case 8U: _LL11: _LL12:
# 465 "buildlib.cyl"
({void*_tmp45=0U;({struct _fat_ptr _tmp462=({const char*_tmp46="Warning: use of mode HI deleted\n";_tag_fat(_tmp46,sizeof(char),33U);});Cyc_log(_tmp462,_tag_fat(_tmp45,sizeof(void*),0U));});});
return 1;case 9U: _LL13: _LL14:
# 468
({void*_tmp47=0U;({struct _fat_ptr _tmp463=({const char*_tmp48="Warning: use of mode SI deleted\n";_tag_fat(_tmp48,sizeof(char),33U);});Cyc_log(_tmp463,_tag_fat(_tmp47,sizeof(void*),0U));});});
return 1;case 10U: _LL15: _LL16:
# 471
({void*_tmp49=0U;({struct _fat_ptr _tmp464=({const char*_tmp4A="Warning: use of mode QI deleted\n";_tag_fat(_tmp4A,sizeof(char),33U);});Cyc_log(_tmp464,_tag_fat(_tmp49,sizeof(void*),0U));});});
return 1;case 11U: _LL17: _LL18:
# 474
({void*_tmp4B=0U;({struct _fat_ptr _tmp465=({const char*_tmp4C="Warning: use of mode DI deleted\n";_tag_fat(_tmp4C,sizeof(char),33U);});Cyc_log(_tmp465,_tag_fat(_tmp4B,sizeof(void*),0U));});});
return 1;case 12U: _LL19: _LL1A:
# 477
({void*_tmp4D=0U;({struct _fat_ptr _tmp466=({const char*_tmp4E="Warning: use of mode DI deleted\n";_tag_fat(_tmp4E,sizeof(char),33U);});Cyc_log(_tmp466,_tag_fat(_tmp4D,sizeof(void*),0U));});});
return 1;case 13U: _LL1B: _LL1C:
# 480
({void*_tmp4F=0U;({struct _fat_ptr _tmp467=({const char*_tmp50="Warning: use of mode word deleted\n";_tag_fat(_tmp50,sizeof(char),35U);});Cyc_log(_tmp467,_tag_fat(_tmp4F,sizeof(void*),0U));});});
return 1;case 14U: _LL1D: _LL1E:
# 483
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15U: _LL1F: _LL20:
# 485
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16U: _LL21: _LL22:
# 487
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17U: _LL23: _LL24:
# 489
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18U: _LL25: _LL26:
# 494 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19U: _LL27: _LL28:
# 496
 return 1;case 20U: _LL29: _LL2A:
# 498
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp51=0U;({struct _fat_ptr _tmp468=({const char*_tmp52="Warning: replacing use of __asm__ with 0\n";_tag_fat(_tmp52,sizeof(char),42U);});Cyc_log(_tmp468,_tag_fat(_tmp51,sizeof(void*),0U));});});
return 1;case 21U: _LL2B: _LL2C:
# 504
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp53=0U;({struct _fat_ptr _tmp469=({const char*_tmp54="Warning: replacing use of __asm with 0\n";_tag_fat(_tmp54,sizeof(char),40U);});Cyc_log(_tmp469,_tag_fat(_tmp53,sizeof(void*),0U));});});
return 1;case 22U: _LL2D: _LL2E:
# 510
({int _tmp46A=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp46A,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default: _LL2F: _LL30:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL0:;}
# 514
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp46B=({const char*_tmp55="some action didn't return!";_tag_fat(_tmp55,sizeof(char),27U);});_tmp56->f1=_tmp46B;});_tmp56;}));}
# 516
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp57=lexstate;switch(_tmp57){case 0U: _LL1: _LL2:
# 514 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 516
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2U: _LL5: _LL6:
# 518
({void*_tmp58=0U;({struct _fat_ptr _tmp46C=({const char*_tmp59="Warning: unclosed string\n";_tag_fat(_tmp59,sizeof(char),26U);});Cyc_log(_tmp46C,_tag_fat(_tmp58,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0U,({struct _fat_ptr _tmp46D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D3.f1=_tmp46D;});_tmp3D3;});void*_tmp5A[1U];_tmp5A[0]=& _tmp5C;({struct Cyc___cycFILE*_tmp46F=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp46E=({const char*_tmp5B="%s";_tag_fat(_tmp5B,sizeof(char),3U);});Cyc_fprintf(_tmp46F,_tmp46E,_tag_fat(_tmp5A,sizeof(void*),1U));});});return 1;case 3U: _LL7: _LL8:
# 521
({struct Cyc_String_pa_PrintArg_struct _tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=0U,({struct _fat_ptr _tmp470=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D4.f1=_tmp470;});_tmp3D4;});void*_tmp5D[1U];_tmp5D[0]=& _tmp5F;({struct Cyc___cycFILE*_tmp472=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp471=({const char*_tmp5E="%s";_tag_fat(_tmp5E,sizeof(char),3U);});Cyc_fprintf(_tmp472,_tmp471,_tag_fat(_tmp5D,sizeof(void*),1U));});});return 1;case 4U: _LL9: _LLA:
# 523
({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0U,({struct _fat_ptr _tmp473=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D5.f1=_tmp473;});_tmp3D5;});void*_tmp60[1U];_tmp60[0]=& _tmp62;({struct Cyc___cycFILE*_tmp475=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp474=({const char*_tmp61="%s";_tag_fat(_tmp61,sizeof(char),3U);});Cyc_fprintf(_tmp475,_tmp474,_tag_fat(_tmp60,sizeof(void*),1U));});});return 1;case 5U: _LLB: _LLC:
# 525
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0U,({struct _fat_ptr _tmp476=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D6.f1=_tmp476;});_tmp3D6;});void*_tmp63[1U];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmp478=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp477=({const char*_tmp64="%s";_tag_fat(_tmp64,sizeof(char),3U);});Cyc_fprintf(_tmp478,_tmp477,_tag_fat(_tmp63,sizeof(void*),1U));});});return 1;case 6U: _LLD: _LLE:
# 527
({struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=0U,({struct _fat_ptr _tmp479=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D7.f1=_tmp479;});_tmp3D7;});void*_tmp66[1U];_tmp66[0]=& _tmp68;({struct Cyc___cycFILE*_tmp47B=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp47A=({const char*_tmp67="%s";_tag_fat(_tmp67,sizeof(char),3U);});Cyc_fprintf(_tmp47B,_tmp47A,_tag_fat(_tmp66,sizeof(void*),1U));});});return 1;case 7U: _LLF: _LL10:
# 529
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp3D8;_tmp3D8.tag=0U,({struct _fat_ptr _tmp47C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D8.f1=_tmp47C;});_tmp3D8;});void*_tmp69[1U];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp47E=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp47D=({const char*_tmp6A="%s";_tag_fat(_tmp6A,sizeof(char),3U);});Cyc_fprintf(_tmp47E,_tmp47D,_tag_fat(_tmp69,sizeof(void*),1U));});});return 1;case 8U: _LL11: _LL12:
# 531
({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0U,({struct _fat_ptr _tmp47F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D9.f1=_tmp47F;});_tmp3D9;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6E;({struct Cyc___cycFILE*_tmp481=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp480=({const char*_tmp6D="%s";_tag_fat(_tmp6D,sizeof(char),3U);});Cyc_fprintf(_tmp481,_tmp480,_tag_fat(_tmp6C,sizeof(void*),1U));});});return 1;default: _LL13: _LL14:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL0:;}
# 535
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp482=({const char*_tmp6F="some action didn't return!";_tag_fat(_tmp6F,sizeof(char),27U);});_tmp70->f1=_tmp482;});_tmp70;}));}
# 537
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp71=lexstate;switch(_tmp71){case 0U: _LL1: _LL2:
# 541 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 543
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2U: _LL5: _LL6:
# 547
 ++ Cyc_parens_to_match;
return 1;case 3U: _LL7: _LL8:
# 550
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 553
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 556
 return 1;case 6U: _LLD: _LLE:
# 558
 return 1;default: _LLF: _LL10:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL0:;}
# 562
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp483=({const char*_tmp72="some action didn't return!";_tag_fat(_tmp72,sizeof(char),27U);});_tmp73->f1=_tmp483;});_tmp73;}));}
# 564
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp74=lexstate;switch(_tmp74){case 0U: _LL1: _LL2:
# 562 "buildlib.cyl"
({void*_tmp75=0U;({struct _fat_ptr _tmp484=({const char*_tmp76="Warning: unclosed string\n";_tag_fat(_tmp76,sizeof(char),26U);});Cyc_log(_tmp484,_tag_fat(_tmp75,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 564
 return 0;case 2U: _LL5: _LL6:
# 566
({void*_tmp77=0U;({struct _fat_ptr _tmp485=({const char*_tmp78="Warning: unclosed string\n";_tag_fat(_tmp78,sizeof(char),26U);});Cyc_log(_tmp485,_tag_fat(_tmp77,sizeof(void*),0U));});});return 1;case 3U: _LL7: _LL8:
# 568
 return 1;case 4U: _LL9: _LLA:
# 570
 return 1;case 5U: _LLB: _LLC:
# 572
 return 1;case 6U: _LLD: _LLE:
# 574
 return 1;case 7U: _LLF: _LL10:
# 576
 return 1;case 8U: _LL11: _LL12:
# 578
 return 1;default: _LL13: _LL14:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL0:;}
# 582
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp486=({const char*_tmp79="some action didn't return!";_tag_fat(_tmp79,sizeof(char),27U);});_tmp7A->f1=_tmp486;});_tmp7A;}));}
# 584
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7B=lexstate;switch(_tmp7B){case 0U: _LL1: _LL2:
# 582 "buildlib.cyl"
({void*_tmp7C=0U;({struct _fat_ptr _tmp487=({const char*_tmp7D="Warning: unclosed comment\n";_tag_fat(_tmp7D,sizeof(char),27U);});Cyc_log(_tmp487,_tag_fat(_tmp7C,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 584
 return 0;case 2U: _LL5: _LL6:
# 586
 return 1;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL0:;}
# 590
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp488=({const char*_tmp7E="some action didn't return!";_tag_fat(_tmp7E,sizeof(char),27U);});_tmp7F->f1=_tmp488;});_tmp7F;}));}
# 592
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple26*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp80=lexstate;switch(_tmp80){case 0U: _LL1: _LL2:
# 594 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp81="#define ";_tag_fat(_tmp81,sizeof(char),9U);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple26*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=Cyc_current_line,_tmp82->f2=(struct _fat_ptr*)_check_null(Cyc_current_source);_tmp82;});case 1U: _LL3: _LL4:
# 598
 return Cyc_suck_line(lexbuf);case 2U: _LL5: _LL6:
# 600
 return 0;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL0:;}
# 604
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp489=({const char*_tmp83="some action didn't return!";_tag_fat(_tmp83,sizeof(char),27U);});_tmp84->f1=_tmp489;});_tmp84;}));}
# 606
struct _tuple26*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp85=lexstate;if(_tmp85 == 0){_LL1: _LL2:
# 604 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_tmp86=_cycalloc(sizeof(*_tmp86));({struct _fat_ptr _tmp48A=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp86=_tmp48A;});_tmp86;});
Cyc_current_line=(struct _fat_ptr)Cyc_strconcat((struct _fat_ptr)Cyc_current_line,(struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL3: _LL4:
# 608
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 611
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp48B=({const char*_tmp87="some action didn't return!";_tag_fat(_tmp87,sizeof(char),27U);});_tmp88->f1=_tmp48B;});_tmp88;}));}
# 613
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp89=lexstate;if(_tmp89 == 0){_LL1: _LL2:
# 611 "buildlib.cyl"
 Cyc_current_line=(struct _fat_ptr)({struct _fat_ptr _tmp48C=(struct _fat_ptr)Cyc_current_line;Cyc_strconcat(_tmp48C,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});return 0;}else{_LL3: _LL4:
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL0:;}
# 615
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp48D=({const char*_tmp8A="some action didn't return!";_tag_fat(_tmp8A,sizeof(char),27U);});_tmp8B->f1=_tmp48D;});_tmp8B;}));}
# 617
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple29*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp8C=lexstate;switch(_tmp8C){case 0U: _LL1: _LL2:
# 618 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1U: _LL3: _LL4:
# 620
 Cyc_current_headerfile=(struct _fat_ptr)({
struct _fat_ptr _tmp48F=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp48F,0,(unsigned long)(({
int _tmp48E=Cyc_Lexing_lexeme_end(lexbuf);_tmp48E - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
Cyc_current_cpp=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
return({struct _tuple29*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->f1=Cyc_current_headerfile,_tmp8D->f2=Cyc_current_symbols,_tmp8D->f3=Cyc_current_user_defs,_tmp8D->f4=Cyc_current_omit_symbols,_tmp8D->f5=Cyc_current_hstubs,_tmp8D->f6=Cyc_current_cstubs,_tmp8D->f7=Cyc_current_cycstubs,_tmp8D->f8=Cyc_current_cpp;_tmp8D;});case 2U: _LL5: _LL6:
# 645
 return Cyc_spec(lexbuf);case 3U: _LL7: _LL8:
# 647
 return 0;case 4U: _LL9: _LLA:
# 649
({struct Cyc_Int_pa_PrintArg_struct _tmp90=({struct Cyc_Int_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=1U,({
# 651
unsigned long _tmp490=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3DA.f1=_tmp490;});_tmp3DA;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({struct Cyc___cycFILE*_tmp492=Cyc_stderr;struct _fat_ptr _tmp491=({const char*_tmp8F="Error in .cys file: expected header file name, found '%c' instead\n";_tag_fat(_tmp8F,sizeof(char),67U);});Cyc_fprintf(_tmp492,_tmp491,_tag_fat(_tmp8E,sizeof(void*),1U));});});
return 0;default: _LLB: _LLC:
((lexbuf->refill_buff))(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL0:;}
# 656
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp493=({const char*_tmp91="some action didn't return!";_tag_fat(_tmp91,sizeof(char),27U);});_tmp92->f1=_tmp493;});_tmp92;}));}
# 658
struct _tuple29*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp93=lexstate;switch(_tmp93){case 0U: _LL1: _LL2:
# 656 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 658
 return 0;case 2U: _LL5: _LL6:
# 660
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3U: _LL7: _LL8:
# 665
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp94=Cyc_current_user_defs;struct Cyc_List_List*tmp_user_defs=_tmp94;
while(Cyc_snarfsymbols(lexbuf)){;}
if(tmp_user_defs != Cyc_current_user_defs){
({void*_tmp95=0U;({struct Cyc___cycFILE*_tmp495=Cyc_stderr;struct _fat_ptr _tmp494=({const char*_tmp96="Error in .cys file: got optional definition in omitsymbols\n";_tag_fat(_tmp96,sizeof(char),60U);});Cyc_fprintf(_tmp495,_tmp494,_tag_fat(_tmp95,sizeof(void*),0U));});});
# 671
return 0;}
# 673
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;}case 4U: _LL9: _LLA:
# 676
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple27*x=({struct _tuple27*_tmp98=_cycalloc(sizeof(*_tmp98));({struct _fat_ptr _tmp497=(struct _fat_ptr)_tag_fat(0,0,0);_tmp98->f1=_tmp497;}),({
struct _fat_ptr _tmp496=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp98->f2=_tmp496;});_tmp98;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=x,_tmp97->tl=Cyc_current_hstubs;_tmp97;});
return 1;}case 5U: _LLB: _LLC: {
# 684
struct _fat_ptr _tmp99=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmp99;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmp9A="hstub";_tag_fat(_tmp9A,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmp9B=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmp9B;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple27*x=({struct _tuple27*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp498=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9D->f2=_tmp498;});_tmp9D;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=x,_tmp9C->tl=Cyc_current_hstubs;_tmp9C;});
return 1;}}}}case 6U: _LLD: _LLE:
# 698
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple27*x=({struct _tuple27*_tmp9F=_cycalloc(sizeof(*_tmp9F));({struct _fat_ptr _tmp49A=(struct _fat_ptr)_tag_fat(0,0,0);_tmp9F->f1=_tmp49A;}),({
struct _fat_ptr _tmp499=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9F->f2=_tmp499;});_tmp9F;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=x,_tmp9E->tl=Cyc_current_cstubs;_tmp9E;});
return 1;}case 7U: _LLF: _LL10: {
# 706
struct _fat_ptr _tmpA0=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpA0;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmpA1="cstub";_tag_fat(_tmpA1,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmpA2=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmpA2;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple27*x=({struct _tuple27*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp49B=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA4->f2=_tmp49B;});_tmpA4;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=x,_tmpA3->tl=Cyc_current_cstubs;_tmpA3;});
return 1;}}}}case 8U: _LL11: _LL12:
# 720
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple27*x=({struct _tuple27*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _fat_ptr _tmp49D=(struct _fat_ptr)_tag_fat(0,0,0);_tmpA6->f1=_tmp49D;}),({
struct _fat_ptr _tmp49C=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA6->f2=_tmp49C;});_tmpA6;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=x,_tmpA5->tl=Cyc_current_cycstubs;_tmpA5;});
return 1;}case 9U: _LL13: _LL14: {
# 728
struct _fat_ptr _tmpA7=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpA7;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmpA8="cycstub";_tag_fat(_tmpA8,sizeof(char),8U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmpA9=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmpA9;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple27*x=({struct _tuple27*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp49E=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpAB->f2=_tmp49E;});_tmpAB;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->hd=x,_tmpAA->tl=Cyc_current_cycstubs;_tmpAA;});
return 1;}}}}case 10U: _LL15: _LL16:
# 742
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _fat_ptr*x=({struct _fat_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _fat_ptr _tmp49F=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpAD=_tmp49F;});_tmpAD;});
Cyc_current_cpp=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=x,_tmpAC->tl=Cyc_current_cpp;_tmpAC;});
return 1;}case 11U: _LL17: _LL18:
# 749
 return 1;case 12U: _LL19: _LL1A:
# 751
 return 1;case 13U: _LL1B: _LL1C:
# 753
({struct Cyc_Int_pa_PrintArg_struct _tmpB0=({struct Cyc_Int_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=1U,({
# 755
unsigned long _tmp4A0=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3DB.f1=_tmp4A0;});_tmp3DB;});void*_tmpAE[1U];_tmpAE[0]=& _tmpB0;({struct Cyc___cycFILE*_tmp4A2=Cyc_stderr;struct _fat_ptr _tmp4A1=({const char*_tmpAF="Error in .cys file: expected command, found '%c' instead\n";_tag_fat(_tmpAF,sizeof(char),58U);});Cyc_fprintf(_tmp4A2,_tmp4A1,_tag_fat(_tmpAE,sizeof(void*),1U));});});
return 0;default: _LL1D: _LL1E:
((lexbuf->refill_buff))(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL0:;}
# 760
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4A3=({const char*_tmpB1="some action didn't return!";_tag_fat(_tmpB1,sizeof(char),27U);});_tmpB2->f1=_tmp4A3;});_tmpB2;}));}
# 762
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB3=lexstate;switch(_tmpB3){case 0U: _LL1: _LL2:
# 766 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _fat_ptr*_tmp4A5=({struct _fat_ptr*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct _fat_ptr _tmp4A4=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpB4=_tmp4A4;});_tmpB4;});_tmpB5->hd=_tmp4A5;}),_tmpB5->tl=Cyc_snarfed_symbols;_tmpB5;});
return 1;case 1U: _LL3: _LL4: {
# 769
struct _fat_ptr _tmpB6=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpB6;
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}
# 778
Cyc_rename_current_symbol=0;{
struct _tuple28*user_def=({struct _tuple28*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _fat_ptr*_tmp4A8=({struct _fat_ptr*_tmpBA=_cycalloc(sizeof(*_tmpBA));*_tmpBA=Cyc_current_symbol;_tmpBA;});_tmpBC->f1=_tmp4A8;}),({
struct _fat_ptr*_tmp4A7=({struct _fat_ptr*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _fat_ptr _tmp4A6=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpBB=_tmp4A6;});_tmpBB;});_tmpBC->f2=_tmp4A7;});_tmpBC;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct _fat_ptr*_tmp4A9=({struct _fat_ptr*_tmpB7=_cycalloc(sizeof(*_tmpB7));*_tmpB7=(struct _fat_ptr)Cyc_current_symbol;_tmpB7;});_tmpB8->hd=_tmp4A9;}),_tmpB8->tl=Cyc_snarfed_symbols;_tmpB8;});
Cyc_current_user_defs=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=user_def,_tmpB9->tl=Cyc_current_user_defs;_tmpB9;});
return 1;}}case 2U: _LL5: _LL6:
# 785
 return 1;case 3U: _LL7: _LL8:
# 787
 return 0;case 4U: _LL9: _LLA:
# 789
({void*_tmpBD=0U;({struct Cyc___cycFILE*_tmp4AB=Cyc_stderr;struct _fat_ptr _tmp4AA=({const char*_tmpBE="Error in .cys file: unexpected end-of-file\n";_tag_fat(_tmpBE,sizeof(char),44U);});Cyc_fprintf(_tmp4AB,_tmp4AA,_tag_fat(_tmpBD,sizeof(void*),0U));});});
# 791
return 0;case 5U: _LLB: _LLC:
# 793
({struct Cyc_Int_pa_PrintArg_struct _tmpC1=({struct Cyc_Int_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=1U,({
# 795
unsigned long _tmp4AC=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3DC.f1=_tmp4AC;});_tmp3DC;});void*_tmpBF[1U];_tmpBF[0]=& _tmpC1;({struct Cyc___cycFILE*_tmp4AE=Cyc_stderr;struct _fat_ptr _tmp4AD=({const char*_tmpC0="Error in .cys file: expected symbol, found '%c' instead\n";_tag_fat(_tmpC0,sizeof(char),57U);});Cyc_fprintf(_tmp4AE,_tmp4AD,_tag_fat(_tmpBF,sizeof(void*),1U));});});
return 0;default: _LLD: _LLE:
((lexbuf->refill_buff))(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL0:;}
# 800
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4AF=({const char*_tmpC2="some action didn't return!";_tag_fat(_tmpC2,sizeof(char),27U);});_tmpC3->f1=_tmp4AF;});_tmpC3;}));}
# 802
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC4=lexstate;switch(_tmpC4){case 0U: _LL1: _LL2:
# 806 "buildlib.cyl"
({void*_tmpC5=0U;({struct _fat_ptr _tmp4B0=({const char*_tmpC6="Warning: unclosed brace\n";_tag_fat(_tmpC6,sizeof(char),25U);});Cyc_log(_tmp4B0,_tag_fat(_tmpC5,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 808
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2U: _LL5: _LL6:
# 813
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3U: _LL7: _LL8:
# 817
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 821
({struct Cyc_Buffer_t*_tmp4B1=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B1,({const char*_tmpC7="/*";_tag_fat(_tmpC7,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 825
({struct Cyc_Buffer_t*_tmp4B2=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B2,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE: {
# 828
struct _fat_ptr _tmpC8=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr symbol=_tmpC8;
if(Cyc_rename_current_symbol && !Cyc_strcmp((struct _fat_ptr)symbol,(struct _fat_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp4B3=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B3,*Cyc_add_user_prefix(({struct _fat_ptr*_tmpC9=_cycalloc(sizeof(*_tmpC9));*_tmpC9=symbol;_tmpC9;})));});else{
# 832
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),symbol);}
return 1;}case 7U: _LLF: _LL10:
# 835
({struct Cyc_Buffer_t*_tmp4B4=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp4B4,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default: _LL11: _LL12:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL0:;}
# 840
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4B5=({const char*_tmpCA="some action didn't return!";_tag_fat(_tmpCA,sizeof(char),27U);});_tmpCB->f1=_tmp4B5;});_tmpCB;}));}
# 842
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpCC=lexstate;switch(_tmpCC){case 0U: _LL1: _LL2:
# 840 "buildlib.cyl"
({void*_tmpCD=0U;({struct _fat_ptr _tmp4B6=({const char*_tmpCE="Warning: unclosed string\n";_tag_fat(_tmpCE,sizeof(char),26U);});Cyc_log(_tmp4B6,_tag_fat(_tmpCD,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 842
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2U: _LL5: _LL6:
# 844
({void*_tmpCF=0U;({struct _fat_ptr _tmp4B7=({const char*_tmpD0="Warning: unclosed string\n";_tag_fat(_tmpD0,sizeof(char),26U);});Cyc_log(_tmp4B7,_tag_fat(_tmpCF,sizeof(void*),0U));});});
({struct Cyc_Buffer_t*_tmp4B8=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B8,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3U: _LL7: _LL8:
# 848
({struct Cyc_Buffer_t*_tmp4B9=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B9,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4U: _LL9: _LLA:
# 851
({struct Cyc_Buffer_t*_tmp4BA=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BA,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5U: _LLB: _LLC:
# 854
({struct Cyc_Buffer_t*_tmp4BB=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BB,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE:
# 857
({struct Cyc_Buffer_t*_tmp4BC=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BC,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7U: _LLF: _LL10:
# 860
({struct Cyc_Buffer_t*_tmp4BD=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BD,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8U: _LL11: _LL12:
# 863
({struct Cyc_Buffer_t*_tmp4BE=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BE,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL13: _LL14:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL0:;}
# 868
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4BF=({const char*_tmpD1="some action didn't return!";_tag_fat(_tmpD1,sizeof(char),27U);});_tmpD2->f1=_tmp4BF;});_tmpD2;}));}
# 870
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpD3=lexstate;switch(_tmpD3){case 0U: _LL1: _LL2:
# 868 "buildlib.cyl"
({void*_tmpD4=0U;({struct _fat_ptr _tmp4C0=({const char*_tmpD5="Warning: unclosed comment\n";_tag_fat(_tmpD5,sizeof(char),27U);});Cyc_log(_tmp4C0,_tag_fat(_tmpD4,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 870
({struct Cyc_Buffer_t*_tmp4C1=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4C1,({const char*_tmpD6="*/";_tag_fat(_tmpD6,sizeof(char),3U);}));});return 0;case 2U: _LL5: _LL6:
# 872
({struct Cyc_Buffer_t*_tmp4C2=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4C2,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL0:;}
# 877
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4C3=({const char*_tmpD7="some action didn't return!";_tag_fat(_tmpD7,sizeof(char),27U);});_tmpD8->f1=_tmp4C3;});_tmpD8;}));}
# 879
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 885 "buildlib.cyl"
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpD9=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_stmttmp1=_tmpD9;void*_tmpDA=_stmttmp1;struct Cyc_List_List*_tmpDB;struct Cyc_List_List*_tmpDD;void*_tmpDC;struct _fat_ptr*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct _fat_ptr*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;void*_tmpE2;void*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE8;void**_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;int _tmpE5;struct Cyc_Absyn_Exp*_tmpEA;void*_tmpE9;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFF;void*_tmp100;switch(*((int*)_tmpDA)){case 1U: _LL1: _tmp100=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL2: {void*b=_tmp100;
# 889
struct _fat_ptr*_tmp101=(*Cyc_Absyn_binding2qvar(b)).f2;struct _fat_ptr*v=_tmp101;
Cyc_add_target(v);
return;}case 3U: _LL3: _tmpFF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL4: {struct Cyc_List_List*x=_tmpFF;
# 893
for(0;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 896
return;}case 23U: _LL5: _tmpFD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpFE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL6: {struct Cyc_Absyn_Exp*e1=_tmpFD;struct Cyc_Absyn_Exp*e2=_tmpFE;
# 898
_tmpFB=e1;_tmpFC=e2;goto _LL8;}case 9U: _LL7: _tmpFB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpFC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmpFB;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 900
_tmpF9=e1;_tmpFA=e2;goto _LLA;}case 4U: _LL9: _tmpF9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpFA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmpF9;struct Cyc_Absyn_Exp*e2=_tmpFA;
# 902
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 41U: _LLB: _tmpF8=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LLC: {struct Cyc_Absyn_Exp*e1=_tmpF8;
_tmpF7=e1;goto _LLE;}case 20U: _LLD: _tmpF7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LLE: {struct Cyc_Absyn_Exp*e1=_tmpF7;
# 907
_tmpF6=e1;goto _LL10;}case 18U: _LLF: _tmpF6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpF6;
# 909
_tmpF5=e1;goto _LL12;}case 15U: _LL11: _tmpF5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpF5;
# 911
_tmpF4=e1;goto _LL14;}case 5U: _LL13: _tmpF4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpF4;
# 913
Cyc_scan_exp(e1,dep);
return;}case 6U: _LL15: _tmpF1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpF2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_tmpF3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpF1;struct Cyc_Absyn_Exp*e2=_tmpF2;struct Cyc_Absyn_Exp*e3=_tmpF3;
# 916
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
Cyc_scan_exp(e3,dep);
return;}case 7U: _LL17: _tmpEF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpF0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpEF;struct Cyc_Absyn_Exp*e2=_tmpF0;
_tmpED=e1;_tmpEE=e2;goto _LL1A;}case 8U: _LL19: _tmpED=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpEE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;
# 922
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 10U: _LL1B: _tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpEC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpEB;struct Cyc_List_List*x=_tmpEC;
# 926
Cyc_scan_exp(e1,dep);
for(0;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 930
return;}case 14U: _LL1D: _tmpE9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpEA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1E: {void*t1=_tmpE9;struct Cyc_Absyn_Exp*e1=_tmpEA;
# 932
Cyc_scan_type(t1,dep);
Cyc_scan_exp(e1,dep);
return;}case 34U: _LL1F: _tmpE5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).is_calloc;_tmpE6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).rgn;_tmpE7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).elt_type;_tmpE8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).num_elts;_LL20: {int iscalloc=_tmpE5;struct Cyc_Absyn_Exp*ropt=_tmpE6;void**topt=_tmpE7;struct Cyc_Absyn_Exp*e=_tmpE8;
# 936
if(ropt != 0)Cyc_scan_exp(ropt,dep);
if(topt != 0)Cyc_scan_type(*topt,dep);
Cyc_scan_exp(e,dep);
return;}case 38U: _LL21: _tmpE4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmpE4;
# 941
Cyc_scan_exp(e,dep);return;}case 39U: _LL23: _tmpE3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL24: {void*t1=_tmpE3;
_tmpE2=t1;goto _LL26;}case 17U: _LL25: _tmpE2=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL26: {void*t1=_tmpE2;
# 944
Cyc_scan_type(t1,dep);
return;}case 21U: _LL27: _tmpE0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpE1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpE0;struct _fat_ptr*fn=_tmpE1;
# 947
_tmpDE=e1;_tmpDF=fn;goto _LL2A;}case 22U: _LL29: _tmpDE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpDE;struct _fat_ptr*fn=_tmpDF;
# 949
Cyc_scan_exp(e1,dep);
Cyc_add_target(fn);
return;}case 19U: _LL2B: _tmpDC=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDD=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL2C: {void*t1=_tmpDC;struct Cyc_List_List*f=_tmpDD;
# 953
Cyc_scan_type(t1,dep);
# 955
{void*_tmp102=(void*)((struct Cyc_List_List*)_check_null(f))->hd;void*_stmttmp2=_tmp102;void*_tmp103=_stmttmp2;struct _fat_ptr*_tmp104;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp103)->tag == 0U){_LL58: _tmp104=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp103)->f1;_LL59: {struct _fat_ptr*fn=_tmp104;
Cyc_add_target(fn);goto _LL57;}}else{_LL5A: _LL5B:
 goto _LL57;}_LL57:;}
# 959
return;}case 0U: _LL2D: _LL2E:
# 961
 return;case 36U: _LL2F: _tmpDB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL30: {struct Cyc_List_List*x=_tmpDB;
# 963
for(0;x != 0;x=x->tl){
struct _tuple30*_tmp105=(struct _tuple30*)x->hd;struct _tuple30*_stmttmp3=_tmp105;struct _tuple30*_tmp106=_stmttmp3;struct Cyc_Absyn_Exp*_tmp107;_LL5D: _tmp107=_tmp106->f2;_LL5E: {struct Cyc_Absyn_Exp*e1=_tmp107;
Cyc_scan_exp(e1,dep);}}
# 967
return;}case 40U: _LL31: _LL32:
 return;case 2U: _LL33: _LL34:
# 970
({void*_tmp108=0U;({struct Cyc___cycFILE*_tmp4C5=Cyc_stderr;struct _fat_ptr _tmp4C4=({const char*_tmp109="Error: unexpected Pragma_e\n";_tag_fat(_tmp109,sizeof(char),28U);});Cyc_fprintf(_tmp4C5,_tmp4C4,_tag_fat(_tmp108,sizeof(void*),0U));});});
exit(1);return;case 35U: _LL35: _LL36:
# 973
({void*_tmp10A=0U;({struct Cyc___cycFILE*_tmp4C7=Cyc_stderr;struct _fat_ptr _tmp4C6=({const char*_tmp10B="Error: unexpected Swap_e\n";_tag_fat(_tmp10B,sizeof(char),26U);});Cyc_fprintf(_tmp4C7,_tmp4C6,_tag_fat(_tmp10A,sizeof(void*),0U));});});
exit(1);return;case 37U: _LL37: _LL38:
# 976
({void*_tmp10C=0U;({struct Cyc___cycFILE*_tmp4C9=Cyc_stderr;struct _fat_ptr _tmp4C8=({const char*_tmp10D="Error: unexpected Stmt_e\n";_tag_fat(_tmp10D,sizeof(char),26U);});Cyc_fprintf(_tmp4C9,_tmp4C8,_tag_fat(_tmp10C,sizeof(void*),0U));});});
exit(1);return;case 42U: _LL39: _LL3A:
# 979
({void*_tmp10E=0U;({struct Cyc___cycFILE*_tmp4CB=Cyc_stderr;struct _fat_ptr _tmp4CA=({const char*_tmp10F="Error: unexpected Assert_e\n";_tag_fat(_tmp10F,sizeof(char),28U);});Cyc_fprintf(_tmp4CB,_tmp4CA,_tag_fat(_tmp10E,sizeof(void*),0U));});});
exit(1);return;case 11U: _LL3B: _LL3C:
# 982
({void*_tmp110=0U;({struct Cyc___cycFILE*_tmp4CD=Cyc_stderr;struct _fat_ptr _tmp4CC=({const char*_tmp111="Error: unexpected Throw_e\n";_tag_fat(_tmp111,sizeof(char),27U);});Cyc_fprintf(_tmp4CD,_tmp4CC,_tag_fat(_tmp110,sizeof(void*),0U));});});
exit(1);return;case 12U: _LL3D: _LL3E:
# 985
({void*_tmp112=0U;({struct Cyc___cycFILE*_tmp4CF=Cyc_stderr;struct _fat_ptr _tmp4CE=({const char*_tmp113="Error: unexpected NoInstantiate_e\n";_tag_fat(_tmp113,sizeof(char),35U);});Cyc_fprintf(_tmp4CF,_tmp4CE,_tag_fat(_tmp112,sizeof(void*),0U));});});
exit(1);return;case 13U: _LL3F: _LL40:
# 988
({void*_tmp114=0U;({struct Cyc___cycFILE*_tmp4D1=Cyc_stderr;struct _fat_ptr _tmp4D0=({const char*_tmp115="Error: unexpected Instantiate_e\n";_tag_fat(_tmp115,sizeof(char),33U);});Cyc_fprintf(_tmp4D1,_tmp4D0,_tag_fat(_tmp114,sizeof(void*),0U));});});
exit(1);return;case 16U: _LL41: _LL42:
# 991
({void*_tmp116=0U;({struct Cyc___cycFILE*_tmp4D3=Cyc_stderr;struct _fat_ptr _tmp4D2=({const char*_tmp117="Error: unexpected New_e\n";_tag_fat(_tmp117,sizeof(char),25U);});Cyc_fprintf(_tmp4D3,_tmp4D2,_tag_fat(_tmp116,sizeof(void*),0U));});});
exit(1);return;case 24U: _LL43: _LL44:
# 994
({void*_tmp118=0U;({struct Cyc___cycFILE*_tmp4D5=Cyc_stderr;struct _fat_ptr _tmp4D4=({const char*_tmp119="Error: unexpected Tuple_e\n";_tag_fat(_tmp119,sizeof(char),27U);});Cyc_fprintf(_tmp4D5,_tmp4D4,_tag_fat(_tmp118,sizeof(void*),0U));});});
exit(1);return;case 25U: _LL45: _LL46:
# 997
({void*_tmp11A=0U;({struct Cyc___cycFILE*_tmp4D7=Cyc_stderr;struct _fat_ptr _tmp4D6=({const char*_tmp11B="Error: unexpected CompoundLit_e\n";_tag_fat(_tmp11B,sizeof(char),33U);});Cyc_fprintf(_tmp4D7,_tmp4D6,_tag_fat(_tmp11A,sizeof(void*),0U));});});
exit(1);return;case 26U: _LL47: _LL48:
# 1000
({void*_tmp11C=0U;({struct Cyc___cycFILE*_tmp4D9=Cyc_stderr;struct _fat_ptr _tmp4D8=({const char*_tmp11D="Error: unexpected Array_e\n";_tag_fat(_tmp11D,sizeof(char),27U);});Cyc_fprintf(_tmp4D9,_tmp4D8,_tag_fat(_tmp11C,sizeof(void*),0U));});});
exit(1);return;case 27U: _LL49: _LL4A:
# 1003
({void*_tmp11E=0U;({struct Cyc___cycFILE*_tmp4DB=Cyc_stderr;struct _fat_ptr _tmp4DA=({const char*_tmp11F="Error: unexpected Comprehension_e\n";_tag_fat(_tmp11F,sizeof(char),35U);});Cyc_fprintf(_tmp4DB,_tmp4DA,_tag_fat(_tmp11E,sizeof(void*),0U));});});
exit(1);return;case 28U: _LL4B: _LL4C:
# 1006
({void*_tmp120=0U;({struct Cyc___cycFILE*_tmp4DD=Cyc_stderr;struct _fat_ptr _tmp4DC=({const char*_tmp121="Error: unexpected ComprehensionNoinit_e\n";_tag_fat(_tmp121,sizeof(char),41U);});Cyc_fprintf(_tmp4DD,_tmp4DC,_tag_fat(_tmp120,sizeof(void*),0U));});});
exit(1);return;case 29U: _LL4D: _LL4E:
# 1009
({void*_tmp122=0U;({struct Cyc___cycFILE*_tmp4DF=Cyc_stderr;struct _fat_ptr _tmp4DE=({const char*_tmp123="Error: unexpected Aggregate_e\n";_tag_fat(_tmp123,sizeof(char),31U);});Cyc_fprintf(_tmp4DF,_tmp4DE,_tag_fat(_tmp122,sizeof(void*),0U));});});
exit(1);return;case 30U: _LL4F: _LL50:
# 1012
({void*_tmp124=0U;({struct Cyc___cycFILE*_tmp4E1=Cyc_stderr;struct _fat_ptr _tmp4E0=({const char*_tmp125="Error: unexpected AnonStruct_e\n";_tag_fat(_tmp125,sizeof(char),32U);});Cyc_fprintf(_tmp4E1,_tmp4E0,_tag_fat(_tmp124,sizeof(void*),0U));});});
exit(1);return;case 31U: _LL51: _LL52:
# 1015
({void*_tmp126=0U;({struct Cyc___cycFILE*_tmp4E3=Cyc_stderr;struct _fat_ptr _tmp4E2=({const char*_tmp127="Error: unexpected Datatype_e\n";_tag_fat(_tmp127,sizeof(char),30U);});Cyc_fprintf(_tmp4E3,_tmp4E2,_tag_fat(_tmp126,sizeof(void*),0U));});});
exit(1);return;case 32U: _LL53: _LL54:
# 1018
({void*_tmp128=0U;({struct Cyc___cycFILE*_tmp4E5=Cyc_stderr;struct _fat_ptr _tmp4E4=({const char*_tmp129="Error: unexpected Enum_e\n";_tag_fat(_tmp129,sizeof(char),26U);});Cyc_fprintf(_tmp4E5,_tmp4E4,_tag_fat(_tmp128,sizeof(void*),0U));});});
exit(1);return;default: _LL55: _LL56:
# 1021
({void*_tmp12A=0U;({struct Cyc___cycFILE*_tmp4E7=Cyc_stderr;struct _fat_ptr _tmp4E6=({const char*_tmp12B="Error: unexpected AnonEnum_e\n";_tag_fat(_tmp12B,sizeof(char),30U);});Cyc_fprintf(_tmp4E7,_tmp4E6,_tag_fat(_tmp12A,sizeof(void*),0U));});});
exit(1);return;}_LL0:;}
# 1026
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned)eo)Cyc_scan_exp(eo,dep);
return;}
# 1031
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp12C=t;struct Cyc_Absyn_Datatypedecl*_tmp12D;struct Cyc_Absyn_Enumdecl*_tmp12E;struct Cyc_Absyn_Aggrdecl*_tmp12F;struct _fat_ptr*_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_Absyn_FnInfo _tmp132;struct Cyc_Absyn_Exp*_tmp133;void*_tmp136;struct Cyc_Absyn_Exp*_tmp135;void*_tmp134;struct Cyc_Absyn_PtrInfo _tmp137;struct Cyc_List_List*_tmp139;void*_tmp138;switch(*((int*)_tmp12C)){case 0U: _LL1: _tmp138=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12C)->f1;_tmp139=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12C)->f2;_LL2: {void*c=_tmp138;struct Cyc_List_List*ts=_tmp139;
# 1035
void*_tmp13A=c;struct _fat_ptr*_tmp13B;union Cyc_Absyn_AggrInfo _tmp13C;switch(*((int*)_tmp13A)){case 0U: _LL1E: _LL1F:
 goto _LL21;case 1U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 goto _LL25;case 2U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
# 1041
 return;case 20U: _LL28: _tmp13C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp13A)->f1;_LL29: {union Cyc_Absyn_AggrInfo info=_tmp13C;
# 1043
struct _tuple12 _tmp13D=Cyc_Absyn_aggr_kinded_name(info);struct _tuple12 _stmttmp4=_tmp13D;struct _tuple12 _tmp13E=_stmttmp4;struct _fat_ptr*_tmp13F;_LL49: _tmp13F=(_tmp13E.f2)->f2;_LL4A: {struct _fat_ptr*v=_tmp13F;
_tmp13B=v;goto _LL2B;}}case 15U: _LL2A: _tmp13B=(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp13A)->f1)->f2;_LL2B: {struct _fat_ptr*v=_tmp13B;
Cyc_add_target(v);return;}case 18U: _LL2C: _LL2D:
 goto _LL2F;case 19U: _LL2E: _LL2F: goto _LL31;case 3U: _LL30: _LL31:
 goto _LL33;case 5U: _LL32: _LL33: goto _LL35;case 6U: _LL34: _LL35:
 goto _LL37;case 7U: _LL36: _LL37: goto _LL39;case 8U: _LL38: _LL39:
 goto _LL3B;case 9U: _LL3A: _LL3B: goto _LL3D;case 10U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F: goto _LL41;case 11U: _LL40: _LL41:
 goto _LL43;case 12U: _LL42: _LL43: goto _LL45;case 13U: _LL44: _LL45:
 goto _LL47;default: _LL46: _LL47:
({struct Cyc_String_pa_PrintArg_struct _tmp142=({struct Cyc_String_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=0U,({struct _fat_ptr _tmp4E8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp3DD.f1=_tmp4E8;});_tmp3DD;});void*_tmp140[1U];_tmp140[0]=& _tmp142;({struct Cyc___cycFILE*_tmp4EA=Cyc_stderr;struct _fat_ptr _tmp4E9=({const char*_tmp141="Error: unexpected %s\n";_tag_fat(_tmp141,sizeof(char),22U);});Cyc_fprintf(_tmp4EA,_tmp4E9,_tag_fat(_tmp140,sizeof(void*),1U));});});
exit(1);return;}_LL1D:;}case 3U: _LL3: _tmp137=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C)->f1;_LL4: {struct Cyc_Absyn_PtrInfo x=_tmp137;
# 1058
Cyc_scan_type(x.elt_type,dep);
return;}case 4U: _LL5: _tmp134=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C)->f1).elt_type;_tmp135=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C)->f1).num_elts;_tmp136=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C)->f1).zero_term;_LL6: {void*t=_tmp134;struct Cyc_Absyn_Exp*sz=_tmp135;void*zt=_tmp136;
# 1061
Cyc_scan_type(t,dep);
Cyc_scan_exp_opt(sz,dep);
return;}case 11U: _LL7: _tmp133=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp12C)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp133;
# 1065
Cyc_scan_exp(e,dep);
return;}case 5U: _LL9: _tmp132=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12C)->f1;_LLA: {struct Cyc_Absyn_FnInfo x=_tmp132;
# 1068
Cyc_scan_type(x.ret_type,dep);
{struct Cyc_List_List*_tmp143=x.args;struct Cyc_List_List*a=_tmp143;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp144=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5=_tmp144;struct _tuple9*_tmp145=_stmttmp5;void*_tmp146;_LL4C: _tmp146=_tmp145->f3;_LL4D: {void*argt=_tmp146;
Cyc_scan_type(argt,dep);}}}
# 1073
if(x.cyc_varargs != 0)
Cyc_scan_type((x.cyc_varargs)->type,dep);
return;}case 7U: _LLB: _tmp131=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12C)->f2;_LLC: {struct Cyc_List_List*sfs=_tmp131;
# 1077
for(0;sfs != 0;sfs=sfs->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->width,dep);}
# 1081
return;}case 8U: _LLD: _tmp130=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp12C)->f1)->f2;_LLE: {struct _fat_ptr*v=_tmp130;
# 1083
Cyc_add_target(v);
return;}case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)){case 0U: _LLF: _tmp12F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)->f1;_LL10: {struct Cyc_Absyn_Aggrdecl*x=_tmp12F;
# 1087
({struct Cyc_Absyn_Decl*_tmp4EB=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->tag=5U,_tmp147->f1=x;_tmp147;}),0U);Cyc_scan_decl(_tmp4EB,dep);});{
struct _tuple1*_tmp148=x->name;struct _tuple1*_stmttmp6=_tmp148;struct _tuple1*_tmp149=_stmttmp6;struct _fat_ptr*_tmp14A;_LL4F: _tmp14A=_tmp149->f2;_LL50: {struct _fat_ptr*n=_tmp14A;
Cyc_add_target(n);
return;}}}case 1U: _LL11: _tmp12E=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*x=_tmp12E;
# 1093
({struct Cyc_Absyn_Decl*_tmp4EC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->tag=7U,_tmp14B->f1=x;_tmp14B;}),0U);Cyc_scan_decl(_tmp4EC,dep);});{
struct _tuple1*_tmp14C=x->name;struct _tuple1*_stmttmp7=_tmp14C;struct _tuple1*_tmp14D=_stmttmp7;struct _fat_ptr*_tmp14E;_LL52: _tmp14E=_tmp14D->f2;_LL53: {struct _fat_ptr*n=_tmp14E;
Cyc_add_target(n);
return;}}}default: _LL13: _tmp12D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)->f1;_LL14: {struct Cyc_Absyn_Datatypedecl*dd=_tmp12D;
# 1099
({void*_tmp14F=0U;({struct Cyc___cycFILE*_tmp4EE=Cyc_stderr;struct _fat_ptr _tmp4ED=({const char*_tmp150="Error: unexpected Datatype declaration\n";_tag_fat(_tmp150,sizeof(char),40U);});Cyc_fprintf(_tmp4EE,_tmp4ED,_tag_fat(_tmp14F,sizeof(void*),0U));});});
exit(1);return;}}case 1U: _LL15: _LL16:
# 1102
({void*_tmp151=0U;({struct Cyc___cycFILE*_tmp4F0=Cyc_stderr;struct _fat_ptr _tmp4EF=({const char*_tmp152="Error: unexpected Evar\n";_tag_fat(_tmp152,sizeof(char),24U);});Cyc_fprintf(_tmp4F0,_tmp4EF,_tag_fat(_tmp151,sizeof(void*),0U));});});
exit(1);return;case 2U: _LL17: _LL18:
# 1105
({void*_tmp153=0U;({struct Cyc___cycFILE*_tmp4F2=Cyc_stderr;struct _fat_ptr _tmp4F1=({const char*_tmp154="Error: unexpected VarType\n";_tag_fat(_tmp154,sizeof(char),27U);});Cyc_fprintf(_tmp4F2,_tmp4F1,_tag_fat(_tmp153,sizeof(void*),0U));});});
exit(1);return;case 6U: _LL19: _LL1A:
# 1108
({void*_tmp155=0U;({struct Cyc___cycFILE*_tmp4F4=Cyc_stderr;struct _fat_ptr _tmp4F3=({const char*_tmp156="Error: unexpected TupleType\n";_tag_fat(_tmp156,sizeof(char),29U);});Cyc_fprintf(_tmp4F4,_tmp4F3,_tag_fat(_tmp155,sizeof(void*),0U));});});
exit(1);return;default: _LL1B: _LL1C:
# 1111
({void*_tmp157=0U;({struct Cyc___cycFILE*_tmp4F6=Cyc_stderr;struct _fat_ptr _tmp4F5=({const char*_tmp158="Error: unexpected valueof_t\n";_tag_fat(_tmp158,sizeof(char),29U);});Cyc_fprintf(_tmp4F6,_tmp4F5,_tag_fat(_tmp157,sizeof(void*),0U));});});
exit(1);return;}_LL0:;}
# 1116
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp159=Cyc_current_targets;struct Cyc_Set_Set**saved_targets=_tmp159;
struct _fat_ptr*_tmp15A=Cyc_current_source;struct _fat_ptr*saved_source=_tmp15A;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp15B=_cycalloc(sizeof(*_tmp15B));({struct Cyc_Set_Set*_tmp4F7=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp15B=_tmp4F7;});_tmp15B;});
{void*_tmp15C=d->r;void*_stmttmp8=_tmp15C;void*_tmp15D=_stmttmp8;struct Cyc_Absyn_Typedefdecl*_tmp15E;struct Cyc_Absyn_Enumdecl*_tmp15F;struct Cyc_Absyn_Aggrdecl*_tmp160;struct Cyc_Absyn_Fndecl*_tmp161;struct Cyc_Absyn_Vardecl*_tmp162;switch(*((int*)_tmp15D)){case 0U: _LL1: _tmp162=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp162;
# 1122
struct _tuple1*_tmp163=x->name;struct _tuple1*_stmttmp9=_tmp163;struct _tuple1*_tmp164=_stmttmp9;struct _fat_ptr*_tmp165;_LL24: _tmp165=_tmp164->f2;_LL25: {struct _fat_ptr*v=_tmp165;
Cyc_current_source=v;
Cyc_scan_type(x->type,dep);
Cyc_scan_exp_opt(x->initializer,dep);
goto _LL0;}}case 1U: _LL3: _tmp161=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL4: {struct Cyc_Absyn_Fndecl*x=_tmp161;
# 1128
struct _tuple1*_tmp166=x->name;struct _tuple1*_stmttmpA=_tmp166;struct _tuple1*_tmp167=_stmttmpA;struct _fat_ptr*_tmp168;_LL27: _tmp168=_tmp167->f2;_LL28: {struct _fat_ptr*v=_tmp168;
Cyc_current_source=v;
Cyc_scan_type((x->i).ret_type,dep);
{struct Cyc_List_List*_tmp169=(x->i).args;struct Cyc_List_List*a=_tmp169;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp16A=(struct _tuple9*)a->hd;struct _tuple9*_stmttmpB=_tmp16A;struct _tuple9*_tmp16B=_stmttmpB;void*_tmp16C;_LL2A: _tmp16C=_tmp16B->f3;_LL2B: {void*t1=_tmp16C;
Cyc_scan_type(t1,dep);}}}
# 1135
if((x->i).cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null((x->i).cyc_varargs))->type,dep);
if(x->is_inline)
({void*_tmp16D=0U;({struct Cyc___cycFILE*_tmp4F9=Cyc_stderr;struct _fat_ptr _tmp4F8=({const char*_tmp16E="Warning: ignoring inline function\n";_tag_fat(_tmp16E,sizeof(char),35U);});Cyc_fprintf(_tmp4F9,_tmp4F8,_tag_fat(_tmp16D,sizeof(void*),0U));});});
goto _LL0;}}case 5U: _LL5: _tmp160=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*x=_tmp160;
# 1141
struct _tuple1*_tmp16F=x->name;struct _tuple1*_stmttmpC=_tmp16F;struct _tuple1*_tmp170=_stmttmpC;struct _fat_ptr*_tmp171;_LL2D: _tmp171=_tmp170->f2;_LL2E: {struct _fat_ptr*v=_tmp171;
Cyc_current_source=v;
if((unsigned)x->impl){
{struct Cyc_List_List*_tmp172=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp172;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp173=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp173;
Cyc_scan_type(f->type,dep);
Cyc_scan_exp_opt(f->width,dep);}}{
# 1151
struct Cyc_List_List*_tmp174=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp174;for(0;fs != 0;fs=fs->tl){;}}}
# 1155
goto _LL0;}}case 7U: _LL7: _tmp15F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*x=_tmp15F;
# 1157
struct _tuple1*_tmp175=x->name;struct _tuple1*_stmttmpD=_tmp175;struct _tuple1*_tmp176=_stmttmpD;struct _fat_ptr*_tmp177;_LL30: _tmp177=_tmp176->f2;_LL31: {struct _fat_ptr*v=_tmp177;
Cyc_current_source=v;
if((unsigned)x->fields){
{struct Cyc_List_List*_tmp178=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp178;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp179=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp179;
Cyc_scan_exp_opt(f->tag,dep);}}{
# 1166
struct Cyc_List_List*_tmp17A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp17A;for(0;fs != 0;fs=fs->tl){;}}}
# 1170
goto _LL0;}}case 8U: _LL9: _tmp15E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*x=_tmp15E;
# 1172
struct _tuple1*_tmp17B=x->name;struct _tuple1*_stmttmpE=_tmp17B;struct _tuple1*_tmp17C=_stmttmpE;struct _fat_ptr*_tmp17D;_LL33: _tmp17D=_tmp17C->f2;_LL34: {struct _fat_ptr*v=_tmp17D;
Cyc_current_source=v;
if((unsigned)x->defn)
Cyc_scan_type((void*)_check_null(x->defn),dep);
goto _LL0;}}case 4U: _LLB: _LLC:
# 1178
({void*_tmp17E=0U;({struct Cyc___cycFILE*_tmp4FB=Cyc_stderr;struct _fat_ptr _tmp4FA=({const char*_tmp17F="Error: unexpected region declaration";_tag_fat(_tmp17F,sizeof(char),37U);});Cyc_fprintf(_tmp4FB,_tmp4FA,_tag_fat(_tmp17E,sizeof(void*),0U));});});
exit(1);case 13U: _LLD: _LLE:
# 1181
({void*_tmp180=0U;({struct Cyc___cycFILE*_tmp4FD=Cyc_stderr;struct _fat_ptr _tmp4FC=({const char*_tmp181="Error: unexpected __cyclone_port_on__";_tag_fat(_tmp181,sizeof(char),38U);});Cyc_fprintf(_tmp4FD,_tmp4FC,_tag_fat(_tmp180,sizeof(void*),0U));});});
exit(1);case 14U: _LLF: _LL10:
# 1184
({void*_tmp182=0U;({struct Cyc___cycFILE*_tmp4FF=Cyc_stderr;struct _fat_ptr _tmp4FE=({const char*_tmp183="Error: unexpected __cyclone_port_off__";_tag_fat(_tmp183,sizeof(char),39U);});Cyc_fprintf(_tmp4FF,_tmp4FE,_tag_fat(_tmp182,sizeof(void*),0U));});});
exit(1);case 15U: _LL11: _LL12:
# 1187
({void*_tmp184=0U;({struct Cyc___cycFILE*_tmp501=Cyc_stderr;struct _fat_ptr _tmp500=({const char*_tmp185="Error: unexpected __tempest_on__";_tag_fat(_tmp185,sizeof(char),33U);});Cyc_fprintf(_tmp501,_tmp500,_tag_fat(_tmp184,sizeof(void*),0U));});});
exit(1);case 16U: _LL13: _LL14:
# 1190
({void*_tmp186=0U;({struct Cyc___cycFILE*_tmp503=Cyc_stderr;struct _fat_ptr _tmp502=({const char*_tmp187="Error: unexpected __tempest_off__";_tag_fat(_tmp187,sizeof(char),34U);});Cyc_fprintf(_tmp503,_tmp502,_tag_fat(_tmp186,sizeof(void*),0U));});});
exit(1);case 2U: _LL15: _LL16:
# 1193
({void*_tmp188=0U;({struct Cyc___cycFILE*_tmp505=Cyc_stderr;struct _fat_ptr _tmp504=({const char*_tmp189="Error: unexpected let declaration\n";_tag_fat(_tmp189,sizeof(char),35U);});Cyc_fprintf(_tmp505,_tmp504,_tag_fat(_tmp188,sizeof(void*),0U));});});
exit(1);case 6U: _LL17: _LL18:
# 1196
({void*_tmp18A=0U;({struct Cyc___cycFILE*_tmp507=Cyc_stderr;struct _fat_ptr _tmp506=({const char*_tmp18B="Error: unexpected datatype declaration\n";_tag_fat(_tmp18B,sizeof(char),40U);});Cyc_fprintf(_tmp507,_tmp506,_tag_fat(_tmp18A,sizeof(void*),0U));});});
exit(1);case 3U: _LL19: _LL1A:
# 1199
({void*_tmp18C=0U;({struct Cyc___cycFILE*_tmp509=Cyc_stderr;struct _fat_ptr _tmp508=({const char*_tmp18D="Error: unexpected let declaration\n";_tag_fat(_tmp18D,sizeof(char),35U);});Cyc_fprintf(_tmp509,_tmp508,_tag_fat(_tmp18C,sizeof(void*),0U));});});
exit(1);case 9U: _LL1B: _LL1C:
# 1202
({void*_tmp18E=0U;({struct Cyc___cycFILE*_tmp50B=Cyc_stderr;struct _fat_ptr _tmp50A=({const char*_tmp18F="Error: unexpected namespace declaration\n";_tag_fat(_tmp18F,sizeof(char),41U);});Cyc_fprintf(_tmp50B,_tmp50A,_tag_fat(_tmp18E,sizeof(void*),0U));});});
exit(1);case 10U: _LL1D: _LL1E:
# 1205
({void*_tmp190=0U;({struct Cyc___cycFILE*_tmp50D=Cyc_stderr;struct _fat_ptr _tmp50C=({const char*_tmp191="Error: unexpected using declaration\n";_tag_fat(_tmp191,sizeof(char),37U);});Cyc_fprintf(_tmp50D,_tmp50C,_tag_fat(_tmp190,sizeof(void*),0U));});});
exit(1);case 11U: _LL1F: _LL20:
# 1208
({void*_tmp192=0U;({struct Cyc___cycFILE*_tmp50F=Cyc_stderr;struct _fat_ptr _tmp50E=({const char*_tmp193="Error: unexpected extern \"C\" declaration\n";_tag_fat(_tmp193,sizeof(char),42U);});Cyc_fprintf(_tmp50F,_tmp50E,_tag_fat(_tmp192,sizeof(void*),0U));});});
exit(1);default: _LL21: _LL22:
# 1211
({void*_tmp194=0U;({struct Cyc___cycFILE*_tmp511=Cyc_stderr;struct _fat_ptr _tmp510=({const char*_tmp195="Error: unexpected extern \"C include\" declaration\n";_tag_fat(_tmp195,sizeof(char),50U);});Cyc_fprintf(_tmp511,_tmp510,_tag_fat(_tmp194,sizeof(void*),0U));});});
exit(1);}_LL0:;}{
# 1219
struct Cyc_Set_Set*old;
struct _fat_ptr*_tmp196=(struct _fat_ptr*)_check_null(Cyc_current_source);struct _fat_ptr*name=_tmp196;
{struct _handler_cons _tmp197;_push_handler(& _tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key))Cyc_Hashtable_lookup)(dep,name);;_pop_handler();}else{void*_tmp198=(void*)Cyc_Core_get_exn_thrown();void*_tmp19A=_tmp198;void*_tmp19B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp19A)->tag == Cyc_Core_Not_found){_LL36: _LL37:
# 1224
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL35;}else{_LL38: _tmp19B=_tmp19A;_LL39: {void*exn=_tmp19B;(int)_rethrow(exn);}}_LL35:;}}}{
# 1226
struct Cyc_Set_Set*_tmp19C=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);struct Cyc_Set_Set*targets=_tmp19C;
((void(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,name,targets);
# 1229
Cyc_current_targets=saved_targets;
Cyc_current_source=saved_source;}}}
# 1233
struct Cyc_Hashtable_Table*Cyc_new_deps (void){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),int(*hash)(struct _fat_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1237
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _fat_ptr*x){
struct _handler_cons _tmp19D;_push_handler(& _tmp19D);{int _tmp19F=0;if(setjmp(_tmp19D.handler))_tmp19F=1;if(!_tmp19F){{struct Cyc_Set_Set*_tmp1A0=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0U);return _tmp1A0;};_pop_handler();}else{void*_tmp19E=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A1=_tmp19E;void*_tmp1A2;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp1A1)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 1240
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL3: _tmp1A2=_tmp1A1;_LL4: {void*exn=_tmp1A2;(int)_rethrow(exn);}}_LL0:;}}}
# 1244
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1254 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(curr,(struct _fat_ptr*)init->hd);}{
# 1259
struct Cyc_Set_Set*_tmp1A3=curr;struct Cyc_Set_Set*delta=_tmp1A3;
# 1261
struct _fat_ptr*_tmp1A4=({struct _fat_ptr*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));({struct _fat_ptr _tmp512=({const char*_tmp1A7="";_tag_fat(_tmp1A7,sizeof(char),1U);});*_tmp1A8=_tmp512;});_tmp1A8;});struct _fat_ptr*sptr=_tmp1A4;
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(delta)> 0){
struct Cyc_Set_Set*_tmp1A5=emptyset;struct Cyc_Set_Set*next=_tmp1A5;
struct Cyc_Iter_Iter _tmp1A6=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,delta);struct Cyc_Iter_Iter iter=_tmp1A6;
while(((int(*)(struct Cyc_Iter_Iter,struct _fat_ptr**))Cyc_Iter_next)(iter,& sptr)){
next=({struct Cyc_Set_Set*_tmp513=next;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp513,Cyc_find(t,sptr));});}
delta=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(next,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,delta);}
# 1270
return curr;}}
# 1273
enum Cyc_buildlib_mode{Cyc_NORMAL =0U,Cyc_GATHER =1U,Cyc_GATHERSCRIPT =2U,Cyc_FINISH =3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering (void){
return(int)Cyc_mode == (int)Cyc_GATHER ||(int)Cyc_mode == (int)Cyc_GATHERSCRIPT;}
# 1279
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _fat_ptr fmt,struct _fat_ptr ap){
# 1283
if(Cyc_script_file == 0){
({void*_tmp1A9=0U;({struct Cyc___cycFILE*_tmp515=Cyc_stderr;struct _fat_ptr _tmp514=({const char*_tmp1AA="Internal error: script file is NULL\n";_tag_fat(_tmp1AA,sizeof(char),37U);});Cyc_fprintf(_tmp515,_tmp514,_tag_fat(_tmp1A9,sizeof(void*),0U));});});
exit(1);}
# 1287
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1290
int Cyc_force_directory(struct _fat_ptr d){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp1AD=({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0U,_tmp3DF.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3DF;});struct Cyc_String_pa_PrintArg_struct _tmp1AE=({struct Cyc_String_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=0U,_tmp3DE.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3DE;});void*_tmp1AB[2U];_tmp1AB[0]=& _tmp1AD,_tmp1AB[1]=& _tmp1AE;({struct _fat_ptr _tmp516=({const char*_tmp1AC="if ! test -e %s; then mkdir %s; fi\n";_tag_fat(_tmp1AC,sizeof(char),36U);});Cyc_prscript(_tmp516,_tag_fat(_tmp1AB,sizeof(void*),2U));});});else{
# 1297
int _tmp1AF=({void*_tmp1B3=0U;({const char*_tmp517=(const char*)_untag_fat_ptr(d,sizeof(char),1U);Cyc_open(_tmp517,0,_tag_fat(_tmp1B3,sizeof(unsigned short),0U));});});int fd=_tmp1AF;
if(fd == -1){
if(mkdir((const char*)_check_null(_untag_fat_ptr(d,sizeof(char),1U)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp1B2=({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0U,_tmp3E0.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3E0;});void*_tmp1B0[1U];_tmp1B0[0]=& _tmp1B2;({struct Cyc___cycFILE*_tmp519=Cyc_stderr;struct _fat_ptr _tmp518=({const char*_tmp1B1="Error: could not create directory %s\n";_tag_fat(_tmp1B1,sizeof(char),38U);});Cyc_fprintf(_tmp519,_tmp518,_tag_fat(_tmp1B0,sizeof(void*),1U));});});
return 1;}}else{
# 1304
close(fd);}}
# 1306
return 0;}
# 1309
int Cyc_force_directory_prefixes(struct _fat_ptr file){
# 1313
struct _fat_ptr _tmp1B4=Cyc_strdup((struct _fat_ptr)file);struct _fat_ptr curr=_tmp1B4;
# 1315
struct Cyc_List_List*_tmp1B5=0;struct Cyc_List_List*x=_tmp1B5;
while(1){
curr=Cyc_Filename_dirname((struct _fat_ptr)curr);
if(Cyc_strlen((struct _fat_ptr)curr)== (unsigned long)0)break;
x=({struct Cyc_List_List*_tmp1B7=_cycalloc(sizeof(*_tmp1B7));({struct _fat_ptr*_tmp51A=({struct _fat_ptr*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));*_tmp1B6=(struct _fat_ptr)curr;_tmp1B6;});_tmp1B7->hd=_tmp51A;}),_tmp1B7->tl=x;_tmp1B7;});}
# 1322
for(0;x != 0;x=x->tl){
if(Cyc_force_directory(*((struct _fat_ptr*)x->hd)))return 1;}
# 1325
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _fat_ptr f1;};
# 1332
static int Cyc_is_other_special(char c){
char _tmp1B8=c;switch(_tmp1B8){case 92U: _LL1: _LL2:
 goto _LL4;case 34U: _LL3: _LL4:
 goto _LL6;case 59U: _LL5: _LL6:
 goto _LL8;case 38U: _LL7: _LL8:
 goto _LLA;case 40U: _LL9: _LLA:
 goto _LLC;case 41U: _LLB: _LLC:
 goto _LLE;case 124U: _LLD: _LLE:
 goto _LL10;case 94U: _LLF: _LL10:
 goto _LL12;case 60U: _LL11: _LL12:
 goto _LL14;case 62U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
# 1347
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 1353
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmp1B9=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp1B9;
# 1357
int _tmp1BA=0;int single_quotes=_tmp1BA;
int _tmp1BB=0;int other_special=_tmp1BB;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmp1BC=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmp1BC;
if((int)c == (int)'\'')++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 1366
if(single_quotes == 0 && other_special == 0)
return s;
# 1370
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmp1BD[3U];({struct _fat_ptr*_tmp51F=({struct _fat_ptr*_tmp1BF=_cycalloc(sizeof(*_tmp1BF));({struct _fat_ptr _tmp51E=({const char*_tmp1BE="'";_tag_fat(_tmp1BE,sizeof(char),2U);});*_tmp1BF=_tmp51E;});_tmp1BF;});_tmp1BD[0]=_tmp51F;}),({struct _fat_ptr*_tmp51D=({struct _fat_ptr*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));*_tmp1C0=(struct _fat_ptr)s;_tmp1C0;});_tmp1BD[1]=_tmp51D;}),({struct _fat_ptr*_tmp51C=({struct _fat_ptr*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));({struct _fat_ptr _tmp51B=({const char*_tmp1C1="'";_tag_fat(_tmp1C1,sizeof(char),2U);});*_tmp1C2=_tmp51B;});_tmp1C2;});_tmp1BD[2]=_tmp51C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BD,sizeof(struct _fat_ptr*),3U));}));{
# 1374
unsigned long _tmp1C3=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmp1C3;
struct _fat_ptr s2=({unsigned _tmp1CE=(len2 + (unsigned long)1)+ 1U;char*_tmp1CD=_cycalloc_atomic(_check_times(_tmp1CE,sizeof(char)));({{unsigned _tmp3E1=len2 + (unsigned long)1;unsigned i;for(i=0;i < _tmp3E1;++ i){_tmp1CD[i]='\000';}_tmp1CD[_tmp3E1]=0;}0;});_tag_fat(_tmp1CD,sizeof(char),_tmp1CE);});
int _tmp1C4=0;int i=_tmp1C4;
int _tmp1C5=0;int j=_tmp1C5;
for(0;(unsigned long)i < len;++ i){
char _tmp1C6=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmp1C6;
if((int)c == (int)'\'' || Cyc_is_other_special(c))
({struct _fat_ptr _tmp1C7=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp1C8=*((char*)_check_fat_subscript(_tmp1C7,sizeof(char),0U));char _tmp1C9='\\';if(_get_fat_size(_tmp1C7,sizeof(char))== 1U &&(_tmp1C8 == 0 && _tmp1C9 != 0))_throw_arraybounds();*((char*)_tmp1C7.curr)=_tmp1C9;});
({struct _fat_ptr _tmp1CA=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp1CB=*((char*)_check_fat_subscript(_tmp1CA,sizeof(char),0U));char _tmp1CC=c;if(_get_fat_size(_tmp1CA,sizeof(char))== 1U &&(_tmp1CB == 0 && _tmp1CC != 0))_throw_arraybounds();*((char*)_tmp1CA.curr)=_tmp1CC;});}
# 1384
return(struct _fat_ptr)s2;}}
# 1386
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({struct _fat_ptr _tmp520=Cyc_sh_escape_string(*sp);*_tmp1CF=_tmp520;});_tmp1CF;});}
# 1391
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1399
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1404
({struct Cyc_String_pa_PrintArg_struct _tmp1D2=({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0U,({
struct _fat_ptr _tmp521=(struct _fat_ptr)({const char*_tmp1D3=filename;_tag_fat(_tmp1D3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D3,1U));});_tmp3E2.f1=_tmp521;});_tmp3E2;});void*_tmp1D0[1U];_tmp1D0[0]=& _tmp1D2;({struct Cyc___cycFILE*_tmp523=Cyc_stderr;struct _fat_ptr _tmp522=({const char*_tmp1D1="********************************* %s...\n";_tag_fat(_tmp1D1,sizeof(char),41U);});Cyc_fprintf(_tmp523,_tmp522,_tag_fat(_tmp1D0,sizeof(void*),1U));});});
# 1407
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp1D6=({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0U,({struct _fat_ptr _tmp524=(struct _fat_ptr)({const char*_tmp1D7=filename;_tag_fat(_tmp1D7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D7,1U));});_tmp3E3.f1=_tmp524;});_tmp3E3;});void*_tmp1D4[1U];_tmp1D4[0]=& _tmp1D6;({struct _fat_ptr _tmp525=({const char*_tmp1D5="\n%s:\n";_tag_fat(_tmp1D5,sizeof(char),6U);});Cyc_log(_tmp525,_tag_fat(_tmp1D4,sizeof(void*),1U));});});{
# 1419 "buildlib.cyl"
struct _fat_ptr _tmp1D8=Cyc_Filename_basename(({const char*_tmp32B=filename;_tag_fat(_tmp32B,sizeof(char),_get_zero_arr_size_char((void*)_tmp32B,1U));}));struct _fat_ptr basename=_tmp1D8;
struct _fat_ptr _tmp1D9=Cyc_Filename_dirname(({const char*_tmp32A=filename;_tag_fat(_tmp32A,sizeof(char),_get_zero_arr_size_char((void*)_tmp32A,1U));}));struct _fat_ptr dirname=_tmp1D9;
struct _fat_ptr _tmp1DA=Cyc_Filename_chop_extension((struct _fat_ptr)basename);struct _fat_ptr choppedname=_tmp1DA;
const char*_tmp1DB=(const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp526=(struct _fat_ptr)choppedname;Cyc_strconcat(_tmp526,({const char*_tmp329=".iA";_tag_fat(_tmp329,sizeof(char),4U);}));}),sizeof(char),1U));const char*cppinfile=_tmp1DB;
# 1424
const char*_tmp1DC=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== (unsigned)0?({struct Cyc_String_pa_PrintArg_struct _tmp325=({struct Cyc_String_pa_PrintArg_struct _tmp417;_tmp417.tag=0U,_tmp417.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp417;});void*_tmp323[1U];_tmp323[0]=& _tmp325;({struct _fat_ptr _tmp529=({const char*_tmp324="%s.iB";_tag_fat(_tmp324,sizeof(char),6U);});Cyc_aprintf(_tmp529,_tag_fat(_tmp323,sizeof(void*),1U));});}):({struct _fat_ptr _tmp528=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp528,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp328=({struct Cyc_String_pa_PrintArg_struct _tmp418;_tmp418.tag=0U,_tmp418.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp418;});void*_tmp326[1U];_tmp326[0]=& _tmp328;({struct _fat_ptr _tmp527=({const char*_tmp327="%s.iB";_tag_fat(_tmp327,sizeof(char),6U);});Cyc_aprintf(_tmp527,_tag_fat(_tmp326,sizeof(void*),1U));});}));}),sizeof(char),1U));const char*macrosfile=_tmp1DC;
# 1429
const char*_tmp1DD=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== (unsigned)0?({struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp415;_tmp415.tag=0U,_tmp415.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp415;});void*_tmp31D[1U];_tmp31D[0]=& _tmp31F;({struct _fat_ptr _tmp52C=({const char*_tmp31E="%s.iC";_tag_fat(_tmp31E,sizeof(char),6U);});Cyc_aprintf(_tmp52C,_tag_fat(_tmp31D,sizeof(void*),1U));});}):({struct _fat_ptr _tmp52B=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp52B,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp322=({struct Cyc_String_pa_PrintArg_struct _tmp416;_tmp416.tag=0U,_tmp416.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp416;});void*_tmp320[1U];_tmp320[0]=& _tmp322;({struct _fat_ptr _tmp52A=({const char*_tmp321="%s.iC";_tag_fat(_tmp321,sizeof(char),6U);});Cyc_aprintf(_tmp52A,_tag_fat(_tmp320,sizeof(void*),1U));});}));}),sizeof(char),1U));const char*declsfile=_tmp1DD;
# 1434
const char*_tmp1DE=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== (unsigned)0?({struct Cyc_String_pa_PrintArg_struct _tmp319=({struct Cyc_String_pa_PrintArg_struct _tmp413;_tmp413.tag=0U,_tmp413.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp413;});void*_tmp317[1U];_tmp317[0]=& _tmp319;({struct _fat_ptr _tmp52F=({const char*_tmp318="%s.iD";_tag_fat(_tmp318,sizeof(char),6U);});Cyc_aprintf(_tmp52F,_tag_fat(_tmp317,sizeof(void*),1U));});}):({struct _fat_ptr _tmp52E=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp52E,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31C=({struct Cyc_String_pa_PrintArg_struct _tmp414;_tmp414.tag=0U,_tmp414.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp414;});void*_tmp31A[1U];_tmp31A[0]=& _tmp31C;({struct _fat_ptr _tmp52D=({const char*_tmp31B="%s.iD";_tag_fat(_tmp31B,sizeof(char),6U);});Cyc_aprintf(_tmp52D,_tag_fat(_tmp31A,sizeof(void*),1U));});}));}),sizeof(char),1U));const char*filtereddeclsfile=_tmp1DE;
# 1439
{struct _handler_cons _tmp1DF;_push_handler(& _tmp1DF);{int _tmp1E1=0;if(setjmp(_tmp1DF.handler))_tmp1E1=1;if(!_tmp1E1){
# 1442
if(Cyc_force_directory_prefixes(({const char*_tmp1E2=filename;_tag_fat(_tmp1E2,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E2,1U));}))){
int _tmp1E3=1;_npop_handler(0U);return _tmp1E3;}
# 1447
if((int)Cyc_mode != (int)Cyc_FINISH){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp1E6=({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0U,({struct _fat_ptr _tmp530=(struct _fat_ptr)({const char*_tmp1E7=cppinfile;_tag_fat(_tmp1E7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E7,1U));});_tmp3E4.f1=_tmp530;});_tmp3E4;});void*_tmp1E4[1U];_tmp1E4[0]=& _tmp1E6;({struct _fat_ptr _tmp531=({const char*_tmp1E5="cat >%s <<XXX\n";_tag_fat(_tmp1E5,sizeof(char),15U);});Cyc_prscript(_tmp531,_tag_fat(_tmp1E4,sizeof(void*),1U));});});
{struct Cyc_List_List*_tmp1E8=cpp_insert;struct Cyc_List_List*l=_tmp1E8;for(0;l != 0;l=l->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp1EB=({struct Cyc_String_pa_PrintArg_struct _tmp3E5;_tmp3E5.tag=0U,_tmp3E5.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)l->hd));_tmp3E5;});void*_tmp1E9[1U];_tmp1E9[0]=& _tmp1EB;({struct _fat_ptr _tmp532=({const char*_tmp1EA="%s";_tag_fat(_tmp1EA,sizeof(char),3U);});Cyc_prscript(_tmp532,_tag_fat(_tmp1E9,sizeof(void*),1U));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp1EE=({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0U,({struct _fat_ptr _tmp533=(struct _fat_ptr)({const char*_tmp1EF=filename;_tag_fat(_tmp1EF,sizeof(char),_get_zero_arr_size_char((void*)_tmp1EF,1U));});_tmp3E6.f1=_tmp533;});_tmp3E6;});void*_tmp1EC[1U];_tmp1EC[0]=& _tmp1EE;({struct _fat_ptr _tmp534=({const char*_tmp1ED="#include <%s>\n";_tag_fat(_tmp1ED,sizeof(char),15U);});Cyc_prscript(_tmp534,_tag_fat(_tmp1EC,sizeof(void*),1U));});});
({void*_tmp1F0=0U;({struct _fat_ptr _tmp535=({const char*_tmp1F1="XXX\n";_tag_fat(_tmp1F1,sizeof(char),5U);});Cyc_prscript(_tmp535,_tag_fat(_tmp1F0,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1F4=({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0U,_tmp3E9.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3E9;});struct Cyc_String_pa_PrintArg_struct _tmp1F5=({struct Cyc_String_pa_PrintArg_struct _tmp3E8;_tmp3E8.tag=0U,({struct _fat_ptr _tmp536=(struct _fat_ptr)({const char*_tmp1F8=macrosfile;_tag_fat(_tmp1F8,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F8,1U));});_tmp3E8.f1=_tmp536;});_tmp3E8;});struct Cyc_String_pa_PrintArg_struct _tmp1F6=({struct Cyc_String_pa_PrintArg_struct _tmp3E7;_tmp3E7.tag=0U,({struct _fat_ptr _tmp537=(struct _fat_ptr)({const char*_tmp1F7=cppinfile;_tag_fat(_tmp1F7,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F7,1U));});_tmp3E7.f1=_tmp537;});_tmp3E7;});void*_tmp1F2[3U];_tmp1F2[0]=& _tmp1F4,_tmp1F2[1]=& _tmp1F5,_tmp1F2[2]=& _tmp1F6;({struct _fat_ptr _tmp538=({const char*_tmp1F3="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_fat(_tmp1F3,sizeof(char),35U);});Cyc_prscript(_tmp538,_tag_fat(_tmp1F2,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1FB=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0U,_tmp3EC.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3EC;});struct Cyc_String_pa_PrintArg_struct _tmp1FC=({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0U,({struct _fat_ptr _tmp539=(struct _fat_ptr)({const char*_tmp1FF=declsfile;_tag_fat(_tmp1FF,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FF,1U));});_tmp3EB.f1=_tmp539;});_tmp3EB;});struct Cyc_String_pa_PrintArg_struct _tmp1FD=({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0U,({struct _fat_ptr _tmp53A=(struct _fat_ptr)({const char*_tmp1FE=cppinfile;_tag_fat(_tmp1FE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FE,1U));});_tmp3EA.f1=_tmp53A;});_tmp3EA;});void*_tmp1F9[3U];_tmp1F9[0]=& _tmp1FB,_tmp1F9[1]=& _tmp1FC,_tmp1F9[2]=& _tmp1FD;({struct _fat_ptr _tmp53B=({const char*_tmp1FA="$GCC %s -E     -o %s -x c %s;\n";_tag_fat(_tmp1FA,sizeof(char),31U);});Cyc_prscript(_tmp53B,_tag_fat(_tmp1F9,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp202=({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0U,({struct _fat_ptr _tmp53C=(struct _fat_ptr)({const char*_tmp203=cppinfile;_tag_fat(_tmp203,sizeof(char),_get_zero_arr_size_char((void*)_tmp203,1U));});_tmp3ED.f1=_tmp53C;});_tmp3ED;});void*_tmp200[1U];_tmp200[0]=& _tmp202;({struct _fat_ptr _tmp53D=({const char*_tmp201="rm %s\n";_tag_fat(_tmp201,sizeof(char),7U);});Cyc_prscript(_tmp53D,_tag_fat(_tmp200,sizeof(void*),1U));});});}else{
# 1459
maybe=Cyc_fopen(cppinfile,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp206=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0U,({struct _fat_ptr _tmp53E=(struct _fat_ptr)({const char*_tmp207=cppinfile;_tag_fat(_tmp207,sizeof(char),_get_zero_arr_size_char((void*)_tmp207,1U));});_tmp3EE.f1=_tmp53E;});_tmp3EE;});void*_tmp204[1U];_tmp204[0]=& _tmp206;({struct Cyc___cycFILE*_tmp540=Cyc_stderr;struct _fat_ptr _tmp53F=({const char*_tmp205="Error: could not create file %s\n";_tag_fat(_tmp205,sizeof(char),33U);});Cyc_fprintf(_tmp540,_tmp53F,_tag_fat(_tmp204,sizeof(void*),1U));});});{
int _tmp208=1;_npop_handler(0U);return _tmp208;}}
# 1464
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp20B=({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0U,({struct _fat_ptr _tmp541=(struct _fat_ptr)({const char*_tmp20C=cppinfile;_tag_fat(_tmp20C,sizeof(char),_get_zero_arr_size_char((void*)_tmp20C,1U));});_tmp3EF.f1=_tmp541;});_tmp3EF;});void*_tmp209[1U];_tmp209[0]=& _tmp20B;({struct Cyc___cycFILE*_tmp543=Cyc_stderr;struct _fat_ptr _tmp542=({const char*_tmp20A="Creating %s\n";_tag_fat(_tmp20A,sizeof(char),13U);});Cyc_fprintf(_tmp543,_tmp542,_tag_fat(_tmp209,sizeof(void*),1U));});});
out_file=maybe;
{struct Cyc_List_List*_tmp20D=cpp_insert;struct Cyc_List_List*l=_tmp20D;for(0;l != 0;l=l->tl){
Cyc_fputs((const char*)_check_null(_untag_fat_ptr(*((struct _fat_ptr*)l->hd),sizeof(char),1U)),out_file);}}
# 1470
({struct Cyc_String_pa_PrintArg_struct _tmp210=({struct Cyc_String_pa_PrintArg_struct _tmp3F0;_tmp3F0.tag=0U,({struct _fat_ptr _tmp544=(struct _fat_ptr)({const char*_tmp211=filename;_tag_fat(_tmp211,sizeof(char),_get_zero_arr_size_char((void*)_tmp211,1U));});_tmp3F0.f1=_tmp544;});_tmp3F0;});void*_tmp20E[1U];_tmp20E[0]=& _tmp210;({struct Cyc___cycFILE*_tmp546=out_file;struct _fat_ptr _tmp545=({const char*_tmp20F="#include <%s>\n";_tag_fat(_tmp20F,sizeof(char),15U);});Cyc_fprintf(_tmp546,_tmp545,_tag_fat(_tmp20E,sizeof(void*),1U));});});
Cyc_fclose(out_file);{
struct _fat_ptr _tmp212=({
struct Cyc_List_List*_tmp54A=({struct Cyc_List_List*_tmp22F=_cycalloc(sizeof(*_tmp22F));({struct _fat_ptr*_tmp549=({struct _fat_ptr*_tmp22E=_cycalloc(sizeof(*_tmp22E));({struct _fat_ptr _tmp548=(struct _fat_ptr)({const char*_tmp22D="";_tag_fat(_tmp22D,sizeof(char),1U);});*_tmp22E=_tmp548;});_tmp22E;});_tmp22F->hd=_tmp549;}),({
struct Cyc_List_List*_tmp547=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp22F->tl=_tmp547;});_tmp22F;});
# 1473
Cyc_str_sepstr(_tmp54A,({const char*_tmp230=" ";_tag_fat(_tmp230,sizeof(char),2U);}));});
# 1472
struct _fat_ptr cppargs_string=_tmp212;
# 1476
char*cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp226=({struct Cyc_String_pa_PrintArg_struct _tmp3FC;_tmp3FC.tag=0U,_tmp3FC.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3FC;});struct Cyc_String_pa_PrintArg_struct _tmp227=({struct Cyc_String_pa_PrintArg_struct _tmp3FB;_tmp3FB.tag=0U,_tmp3FB.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3FB;});struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp3FA;_tmp3FA.tag=0U,_tmp3FA.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3FA;});struct Cyc_String_pa_PrintArg_struct _tmp229=({struct Cyc_String_pa_PrintArg_struct _tmp3F9;_tmp3F9.tag=0U,({struct _fat_ptr _tmp54B=(struct _fat_ptr)({const char*_tmp22C=macrosfile;_tag_fat(_tmp22C,sizeof(char),_get_zero_arr_size_char((void*)_tmp22C,1U));});_tmp3F9.f1=_tmp54B;});_tmp3F9;});struct Cyc_String_pa_PrintArg_struct _tmp22A=({struct Cyc_String_pa_PrintArg_struct _tmp3F8;_tmp3F8.tag=0U,({struct _fat_ptr _tmp54C=(struct _fat_ptr)({const char*_tmp22B=cppinfile;_tag_fat(_tmp22B,sizeof(char),_get_zero_arr_size_char((void*)_tmp22B,1U));});_tmp3F8.f1=_tmp54C;});_tmp3F8;});void*_tmp224[5U];_tmp224[0]=& _tmp226,_tmp224[1]=& _tmp227,_tmp224[2]=& _tmp228,_tmp224[3]=& _tmp229,_tmp224[4]=& _tmp22A;({struct _fat_ptr _tmp54D=({const char*_tmp225="%s %s %s -E -dM -o %s -x c %s";_tag_fat(_tmp225,sizeof(char),30U);});Cyc_aprintf(_tmp54D,_tag_fat(_tmp224,sizeof(void*),5U));});}),sizeof(char),1U));
# 1479
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp215=({struct Cyc_String_pa_PrintArg_struct _tmp3F1;_tmp3F1.tag=0U,({struct _fat_ptr _tmp54E=(struct _fat_ptr)({char*_tmp216=cmd;_tag_fat(_tmp216,sizeof(char),_get_zero_arr_size_char((void*)_tmp216,1U));});_tmp3F1.f1=_tmp54E;});_tmp3F1;});void*_tmp213[1U];_tmp213[0]=& _tmp215;({struct Cyc___cycFILE*_tmp550=Cyc_stderr;struct _fat_ptr _tmp54F=({const char*_tmp214="%s\n";_tag_fat(_tmp214,sizeof(char),4U);});Cyc_fprintf(_tmp550,_tmp54F,_tag_fat(_tmp213,sizeof(void*),1U));});});
if(!system((const char*)cmd)){
# 1484
cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp219=({struct Cyc_String_pa_PrintArg_struct _tmp3F6;_tmp3F6.tag=0U,_tmp3F6.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3F6;});struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp3F5;_tmp3F5.tag=0U,_tmp3F5.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3F5;});struct Cyc_String_pa_PrintArg_struct _tmp21B=({struct Cyc_String_pa_PrintArg_struct _tmp3F4;_tmp3F4.tag=0U,_tmp3F4.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3F4;});struct Cyc_String_pa_PrintArg_struct _tmp21C=({struct Cyc_String_pa_PrintArg_struct _tmp3F3;_tmp3F3.tag=0U,({struct _fat_ptr _tmp551=(struct _fat_ptr)({const char*_tmp21F=declsfile;_tag_fat(_tmp21F,sizeof(char),_get_zero_arr_size_char((void*)_tmp21F,1U));});_tmp3F3.f1=_tmp551;});_tmp3F3;});struct Cyc_String_pa_PrintArg_struct _tmp21D=({struct Cyc_String_pa_PrintArg_struct _tmp3F2;_tmp3F2.tag=0U,({struct _fat_ptr _tmp552=(struct _fat_ptr)({const char*_tmp21E=cppinfile;_tag_fat(_tmp21E,sizeof(char),_get_zero_arr_size_char((void*)_tmp21E,1U));});_tmp3F2.f1=_tmp552;});_tmp3F2;});void*_tmp217[5U];_tmp217[0]=& _tmp219,_tmp217[1]=& _tmp21A,_tmp217[2]=& _tmp21B,_tmp217[3]=& _tmp21C,_tmp217[4]=& _tmp21D;({struct _fat_ptr _tmp553=({const char*_tmp218="%s %s %s -E -o %s -x c %s";_tag_fat(_tmp218,sizeof(char),26U);});Cyc_aprintf(_tmp553,_tag_fat(_tmp217,sizeof(void*),5U));});}),sizeof(char),1U));
# 1487
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp222=({struct Cyc_String_pa_PrintArg_struct _tmp3F7;_tmp3F7.tag=0U,({struct _fat_ptr _tmp554=(struct _fat_ptr)({char*_tmp223=cmd;_tag_fat(_tmp223,sizeof(char),_get_zero_arr_size_char((void*)_tmp223,1U));});_tmp3F7.f1=_tmp554;});_tmp3F7;});void*_tmp220[1U];_tmp220[0]=& _tmp222;({struct Cyc___cycFILE*_tmp556=Cyc_stderr;struct _fat_ptr _tmp555=({const char*_tmp221="%s\n";_tag_fat(_tmp221,sizeof(char),4U);});Cyc_fprintf(_tmp556,_tmp555,_tag_fat(_tmp220,sizeof(void*),1U));});});
system((const char*)cmd);}}}}
# 1494
if(Cyc_gathering()){int _tmp231=0;_npop_handler(0U);return _tmp231;}{
# 1497
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp559=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp234=({struct Cyc_String_pa_PrintArg_struct _tmp3FD;_tmp3FD.tag=0U,({struct _fat_ptr _tmp557=(struct _fat_ptr)({const char*_tmp235=macrosfile;_tag_fat(_tmp235,sizeof(char),_get_zero_arr_size_char((void*)_tmp235,1U));});_tmp3FD.f1=_tmp557;});_tmp3FD;});void*_tmp232[1U];_tmp232[0]=& _tmp234;({struct _fat_ptr _tmp558=({const char*_tmp233="can't open macrosfile %s";_tag_fat(_tmp233,sizeof(char),25U);});Cyc_aprintf(_tmp558,_tag_fat(_tmp232,sizeof(void*),1U));});});_tmp236->f1=_tmp559;});_tmp236;}));
# 1502
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp237=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp237;
struct _tuple25*entry;
while((entry=((struct _tuple25*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(l))!= 0){
struct _tuple25*_tmp238=(struct _tuple25*)_check_null(entry);struct _tuple25*_stmttmpF=_tmp238;struct _tuple25*_tmp239=_stmttmpF;struct Cyc_Set_Set*_tmp23B;struct _fat_ptr*_tmp23A;_LL1: _tmp23A=_tmp239->f1;_tmp23B=_tmp239->f2;_LL2: {struct _fat_ptr*name=_tmp23A;struct Cyc_Set_Set*uses=_tmp23B;
((void(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,name,uses);}}
# 1511
Cyc_fclose(in_file);
# 1514
maybe=Cyc_fopen(declsfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp55C=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp23E=({struct Cyc_String_pa_PrintArg_struct _tmp3FE;_tmp3FE.tag=0U,({struct _fat_ptr _tmp55A=(struct _fat_ptr)({const char*_tmp23F=declsfile;_tag_fat(_tmp23F,sizeof(char),_get_zero_arr_size_char((void*)_tmp23F,1U));});_tmp3FE.f1=_tmp55A;});_tmp3FE;});void*_tmp23C[1U];_tmp23C[0]=& _tmp23E;({struct _fat_ptr _tmp55B=({const char*_tmp23D="can't open declsfile %s";_tag_fat(_tmp23D,sizeof(char),24U);});Cyc_aprintf(_tmp55B,_tag_fat(_tmp23C,sizeof(void*),1U));});});_tmp240->f1=_tmp55C;});_tmp240;}));
# 1518
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(filtereddeclsfile,"w");
if(!((unsigned)Cyc_slurp_out)){int _tmp241=1;_npop_handler(0U);return _tmp241;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(l)){;}{
# 1524
struct Cyc_List_List*_tmp242=user_defs;struct Cyc_List_List*x=_tmp242;
while(x != 0){
struct _tuple28*_tmp243=(struct _tuple28*)x->hd;struct _tuple28*_stmttmp10=_tmp243;struct _tuple28*_tmp244=_stmttmp10;struct _fat_ptr*_tmp245;_LL4: _tmp245=_tmp244->f2;_LL5: {struct _fat_ptr*s=_tmp245;
({struct Cyc_String_pa_PrintArg_struct _tmp248=({struct Cyc_String_pa_PrintArg_struct _tmp3FF;_tmp3FF.tag=0U,_tmp3FF.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp3FF;});void*_tmp246[1U];_tmp246[0]=& _tmp248;({struct Cyc___cycFILE*_tmp55E=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp55D=({const char*_tmp247="%s";_tag_fat(_tmp247,sizeof(char),3U);});Cyc_fprintf(_tmp55E,_tmp55D,_tag_fat(_tmp246,sizeof(void*),1U));});});
x=x->tl;}}
# 1530
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if((int)Cyc_mode != (int)Cyc_FINISH)
;
# 1536
maybe=Cyc_fopen(filtereddeclsfile,"r");
if(!((unsigned)maybe)){int _tmp249=1;_npop_handler(0U);return _tmp249;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp24A=filtereddeclsfile;_tag_fat(_tmp24A,sizeof(char),_get_zero_arr_size_char((void*)_tmp24A,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp24B=Cyc_Parse_parse_file(in_file);struct Cyc_List_List*decls=_tmp24B;
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1546
{struct Cyc_List_List*_tmp24C=decls;struct Cyc_List_List*d=_tmp24C;for(0;d != 0;d=d->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)d->hd,t);}}{
# 1550
struct Cyc_List_List*_tmp24D=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);struct Cyc_List_List*user_symbols=_tmp24D;
struct Cyc_Set_Set*_tmp24E=({struct Cyc_List_List*_tmp55F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,user_symbols);Cyc_reachable(_tmp55F,t);});struct Cyc_Set_Set*reachable_set=_tmp24E;
# 1554
struct Cyc_List_List*_tmp24F=0;struct Cyc_List_List*reachable_decls=_tmp24F;
struct Cyc_List_List*_tmp250=0;struct Cyc_List_List*user_decls=_tmp250;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp251=decls;struct Cyc_List_List*d=_tmp251;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp252=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp252;
struct _fat_ptr*name;
{void*_tmp253=decl->r;void*_stmttmp11=_tmp253;void*_tmp254=_stmttmp11;struct Cyc_Absyn_Typedefdecl*_tmp255;struct Cyc_Absyn_Enumdecl*_tmp256;struct Cyc_Absyn_Aggrdecl*_tmp257;struct Cyc_Absyn_Fndecl*_tmp258;struct Cyc_Absyn_Vardecl*_tmp259;switch(*((int*)_tmp254)){case 0U: _LL7: _tmp259=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp259;
# 1562
struct _tuple1*_tmp25A=x->name;struct _tuple1*_stmttmp12=_tmp25A;struct _tuple1*_tmp25B=_stmttmp12;struct _fat_ptr*_tmp25C;_LL2A: _tmp25C=_tmp25B->f2;_LL2B: {struct _fat_ptr*v=_tmp25C;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(defined_symbols,v);
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 1U: _LL9: _tmp258=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LLA: {struct Cyc_Absyn_Fndecl*x=_tmp258;
# 1568
struct _tuple1*_tmp25D=x->name;struct _tuple1*_stmttmp13=_tmp25D;struct _tuple1*_tmp25E=_stmttmp13;struct _fat_ptr*_tmp25F;_LL2D: _tmp25F=_tmp25E->f2;_LL2E: {struct _fat_ptr*v=_tmp25F;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(defined_symbols,v);
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 5U: _LLB: _tmp257=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*x=_tmp257;
# 1574
struct _tuple1*_tmp260=x->name;struct _tuple1*_stmttmp14=_tmp260;struct _tuple1*_tmp261=_stmttmp14;struct _fat_ptr*_tmp262;_LL30: _tmp262=_tmp261->f2;_LL31: {struct _fat_ptr*v=_tmp262;
name=v;
goto _LL6;}}case 7U: _LLD: _tmp256=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LLE: {struct Cyc_Absyn_Enumdecl*x=_tmp256;
# 1578
struct _tuple1*_tmp263=x->name;struct _tuple1*_stmttmp15=_tmp263;struct _tuple1*_tmp264=_stmttmp15;struct _fat_ptr*_tmp265;_LL33: _tmp265=_tmp264->f2;_LL34: {struct _fat_ptr*v=_tmp265;
name=v;
# 1582
if(name != 0 &&((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,name))
reachable_decls=({struct Cyc_List_List*_tmp266=_cycalloc(sizeof(*_tmp266));_tmp266->hd=decl,_tmp266->tl=reachable_decls;_tmp266;});else{
# 1585
if((unsigned)x->fields){
struct Cyc_List_List*_tmp267=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp267;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp268=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp268;
struct _tuple1*_tmp269=f->name;struct _tuple1*_stmttmp16=_tmp269;struct _tuple1*_tmp26A=_stmttmp16;struct _fat_ptr*_tmp26B;_LL36: _tmp26B=_tmp26A->f2;_LL37: {struct _fat_ptr*v=_tmp26B;
if(((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,v)){
reachable_decls=({struct Cyc_List_List*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->hd=decl,_tmp26C->tl=reachable_decls;_tmp26C;});
break;}}}}}
# 1595
name=0;
goto _LL6;}}case 8U: _LLF: _tmp255=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp254)->f1;_LL10: {struct Cyc_Absyn_Typedefdecl*x=_tmp255;
# 1598
struct _tuple1*_tmp26D=x->name;struct _tuple1*_stmttmp17=_tmp26D;struct _tuple1*_tmp26E=_stmttmp17;struct _fat_ptr*_tmp26F;_LL39: _tmp26F=_tmp26E->f2;_LL3A: {struct _fat_ptr*v=_tmp26F;
name=v;
goto _LL6;}}case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 15U: _LL15: _LL16:
 goto _LL18;case 16U: _LL17: _LL18:
 goto _LL1A;case 2U: _LL19: _LL1A:
 goto _LL1C;case 6U: _LL1B: _LL1C:
 goto _LL1E;case 3U: _LL1D: _LL1E:
 goto _LL20;case 9U: _LL1F: _LL20:
 goto _LL22;case 10U: _LL21: _LL22:
 goto _LL24;case 11U: _LL23: _LL24:
 goto _LL26;case 12U: _LL25: _LL26:
 goto _LL28;default: _LL27: _LL28:
# 1613
 name=0;
goto _LL6;}_LL6:;}
# 1617
if(name != 0 &&((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,name)){
if(({struct _fat_ptr _tmp561=(struct _fat_ptr)*name;struct _fat_ptr _tmp560=(struct _fat_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp561,_tmp560,Cyc_strlen((struct _fat_ptr)Cyc_user_prefix));})!= 0)
reachable_decls=({struct Cyc_List_List*_tmp270=_cycalloc(sizeof(*_tmp270));_tmp270->hd=decl,_tmp270->tl=reachable_decls;_tmp270;});else{
# 1622
Cyc_rename_decl(decl);
user_decls=({struct Cyc_List_List*_tmp271=_cycalloc(sizeof(*_tmp271));_tmp271->hd=decl,_tmp271->tl=user_decls;_tmp271;});}}}}
# 1629
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){int _tmp272=1;_npop_handler(0U);return _tmp272;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _fat_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2EF=({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0U,({struct _fat_ptr _tmp562=(struct _fat_ptr)({const char*_tmp2F0=filename;_tag_fat(_tmp2F0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F0,1U));});_tmp40B.f1=_tmp562;});_tmp40B;});void*_tmp2ED[1U];_tmp2ED[0]=& _tmp2EF;({struct _fat_ptr _tmp563=({const char*_tmp2EE="_%s_";_tag_fat(_tmp2EE,sizeof(char),5U);});Cyc_aprintf(_tmp563,_tag_fat(_tmp2ED,sizeof(void*),1U));});});
{int _tmp273=0;int j=_tmp273;for(0;(unsigned)j < _get_fat_size(ifdefmacro,sizeof(char));++ j){
if((int)((char*)ifdefmacro.curr)[j]== (int)'.' ||(int)((char*)ifdefmacro.curr)[j]== (int)'/')
({struct _fat_ptr _tmp274=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp275=*((char*)_check_fat_subscript(_tmp274,sizeof(char),0U));char _tmp276='_';if(_get_fat_size(_tmp274,sizeof(char))== 1U &&(_tmp275 == 0 && _tmp276 != 0))_throw_arraybounds();*((char*)_tmp274.curr)=_tmp276;});else{
if((int)((char*)ifdefmacro.curr)[j]!= (int)'_' &&(int)((char*)ifdefmacro.curr)[j]!= (int)'/')
({struct _fat_ptr _tmp277=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp278=*((char*)_check_fat_subscript(_tmp277,sizeof(char),0U));char _tmp279=(char)toupper((int)((char*)ifdefmacro.curr)[j]);if(_get_fat_size(_tmp277,sizeof(char))== 1U &&(_tmp278 == 0 && _tmp279 != 0))_throw_arraybounds();*((char*)_tmp277.curr)=_tmp279;});}}}
# 1641
({struct Cyc_String_pa_PrintArg_struct _tmp27C=({struct Cyc_String_pa_PrintArg_struct _tmp401;_tmp401.tag=0U,_tmp401.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp401;});struct Cyc_String_pa_PrintArg_struct _tmp27D=({struct Cyc_String_pa_PrintArg_struct _tmp400;_tmp400.tag=0U,_tmp400.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp400;});void*_tmp27A[2U];_tmp27A[0]=& _tmp27C,_tmp27A[1]=& _tmp27D;({struct Cyc___cycFILE*_tmp565=out_file;struct _fat_ptr _tmp564=({const char*_tmp27B="#ifndef %s\n#define %s\n";_tag_fat(_tmp27B,sizeof(char),23U);});Cyc_fprintf(_tmp565,_tmp564,_tag_fat(_tmp27A,sizeof(void*),2U));});});{
# 1648
struct Cyc_List_List*_tmp27E=0;struct Cyc_List_List*print_decls=_tmp27E;
struct Cyc_List_List*_tmp27F=0;struct Cyc_List_List*names=_tmp27F;
{struct Cyc_List_List*_tmp280=reachable_decls;struct Cyc_List_List*d=_tmp280;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp281=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp281;
int _tmp282=0;int anon_enum=_tmp282;
struct _fat_ptr*name;
{void*_tmp283=decl->r;void*_stmttmp18=_tmp283;void*_tmp284=_stmttmp18;struct Cyc_Absyn_Typedefdecl*_tmp285;struct Cyc_Absyn_Enumdecl*_tmp286;struct Cyc_Absyn_Aggrdecl*_tmp287;struct Cyc_Absyn_Fndecl*_tmp288;struct Cyc_Absyn_Vardecl*_tmp289;switch(*((int*)_tmp284)){case 0U: _LL3C: _tmp289=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp284)->f1;_LL3D: {struct Cyc_Absyn_Vardecl*x=_tmp289;
# 1656
struct _tuple1*_tmp28A=x->name;struct _tuple1*_stmttmp19=_tmp28A;struct _tuple1*_tmp28B=_stmttmp19;struct _fat_ptr*_tmp28C;_LL5F: _tmp28C=_tmp28B->f2;_LL60: {struct _fat_ptr*v=_tmp28C;
name=v;
goto _LL3B;}}case 1U: _LL3E: _tmp288=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp284)->f1;_LL3F: {struct Cyc_Absyn_Fndecl*x=_tmp288;
# 1660
if(x->is_inline){name=0;goto _LL3B;}{
struct _tuple1*_tmp28D=x->name;struct _tuple1*_stmttmp1A=_tmp28D;struct _tuple1*_tmp28E=_stmttmp1A;struct _fat_ptr*_tmp28F;_LL62: _tmp28F=_tmp28E->f2;_LL63: {struct _fat_ptr*v=_tmp28F;
name=v;
goto _LL3B;}}}case 5U: _LL40: _tmp287=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp284)->f1;_LL41: {struct Cyc_Absyn_Aggrdecl*x=_tmp287;
# 1665
struct _tuple1*_tmp290=x->name;struct _tuple1*_stmttmp1B=_tmp290;struct _tuple1*_tmp291=_stmttmp1B;struct _fat_ptr*_tmp292;_LL65: _tmp292=_tmp291->f2;_LL66: {struct _fat_ptr*v=_tmp292;
name=v;
goto _LL3B;}}case 7U: _LL42: _tmp286=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp284)->f1;_LL43: {struct Cyc_Absyn_Enumdecl*x=_tmp286;
# 1669
struct _tuple1*_tmp293=x->name;struct _tuple1*_stmttmp1C=_tmp293;struct _tuple1*_tmp294=_stmttmp1C;struct _fat_ptr*_tmp295;_LL68: _tmp295=_tmp294->f2;_LL69: {struct _fat_ptr*v=_tmp295;
name=v;
goto _LL3B;}}case 8U: _LL44: _tmp285=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp284)->f1;_LL45: {struct Cyc_Absyn_Typedefdecl*x=_tmp285;
# 1673
struct _tuple1*_tmp296=x->name;struct _tuple1*_stmttmp1D=_tmp296;struct _tuple1*_tmp297=_stmttmp1D;struct _fat_ptr*_tmp298;_LL6B: _tmp298=_tmp297->f2;_LL6C: {struct _fat_ptr*v=_tmp298;
name=v;
goto _LL3B;}}case 4U: _LL46: _LL47:
 goto _LL49;case 13U: _LL48: _LL49:
 goto _LL4B;case 14U: _LL4A: _LL4B:
 goto _LL4D;case 15U: _LL4C: _LL4D:
 goto _LL4F;case 16U: _LL4E: _LL4F:
 goto _LL51;case 2U: _LL50: _LL51:
 goto _LL53;case 6U: _LL52: _LL53:
 goto _LL55;case 3U: _LL54: _LL55:
 goto _LL57;case 9U: _LL56: _LL57:
 goto _LL59;case 10U: _LL58: _LL59:
 goto _LL5B;case 11U: _LL5A: _LL5B:
 goto _LL5D;default: _LL5C: _LL5D:
# 1688
 name=0;
goto _LL3B;}_LL3B:;}
# 1691
if(!((unsigned)name)&& !anon_enum)continue;
# 1696
if(({struct Cyc_Set_Set*_tmp566=reachable_set;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp566,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp299=Cyc_Absyn_lookup_decl(user_decls,name);struct Cyc_Absyn_Decl*user_decl=_tmp299;
if(user_decl == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp567=({const char*_tmp29A="Internal Error: bad user-def name";_tag_fat(_tmp29A,sizeof(char),34U);});_tmp29B->f1=_tmp567;});_tmp29B;}));else{
# 1702
void*_tmp29C=user_decl->r;void*_stmttmp1E=_tmp29C;void*_tmp29D=_stmttmp1E;switch(*((int*)_tmp29D)){case 0U: _LL6E: _LL6F:
 goto _LL71;case 1U: _LL70: _LL71:
# 1705
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp568=({const char*_tmp29E="user defintions for function or variable decls";_tag_fat(_tmp29E,sizeof(char),47U);});_tmp29F->f1=_tmp568;});_tmp29F;}));default: _LL72: _LL73:
 goto _LL6D;}_LL6D:;}
# 1711
print_decls=({struct Cyc_List_List*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->hd=decl,_tmp2A0->tl=print_decls;_tmp2A0;});}else{
# 1714
print_decls=({struct Cyc_List_List*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->hd=decl,_tmp2A1->tl=print_decls;_tmp2A1;});}
names=({struct Cyc_List_List*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->hd=name,_tmp2A2->tl=names;_tmp2A2;});}}
# 1719
{struct _handler_cons _tmp2A3;_push_handler(& _tmp2A3);{int _tmp2A5=0;if(setjmp(_tmp2A3.handler))_tmp2A5=1;if(!_tmp2A5){
Cyc_Binding_resolve_all(print_decls);
({struct Cyc_Tcenv_Tenv*_tmp569=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp569,1,print_decls);});
# 1720
;_pop_handler();}else{void*_tmp2A4=(void*)Cyc_Core_get_exn_thrown();void*_tmp2A6=_tmp2A4;_LL75: _LL76:
# 1723
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp56A=({const char*_tmp2A7="can't typecheck acquired declarations";_tag_fat(_tmp2A7,sizeof(char),38U);});_tmp2A8->f1=_tmp56A;});_tmp2A8;}));_LL74:;}}}
# 1727
{struct _tuple0 _tmp2A9=({struct _tuple0 _tmp405;_tmp405.f1=print_decls,_tmp405.f2=names;_tmp405;});struct _tuple0 _stmttmp1F=_tmp2A9;struct _tuple0 _tmp2AA=_stmttmp1F;struct Cyc_List_List*_tmp2AC;struct Cyc_List_List*_tmp2AB;_LL7A: _tmp2AB=_tmp2AA.f1;_tmp2AC=_tmp2AA.f2;_LL7B: {struct Cyc_List_List*d=_tmp2AB;struct Cyc_List_List*n=_tmp2AC;for(0;
d != 0 && n != 0;(d=d->tl,n=n->tl)){
struct Cyc_Absyn_Decl*_tmp2AD=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp2AD;
struct _fat_ptr*_tmp2AE=(struct _fat_ptr*)n->hd;struct _fat_ptr*name=_tmp2AE;
int _tmp2AF=0;int anon_enum=_tmp2AF;
if(!((unsigned)name))
anon_enum=1;
# 1736
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned)name){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2B2=({struct Cyc_String_pa_PrintArg_struct _tmp402;_tmp402.tag=0U,_tmp402.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp402;});void*_tmp2B0[1U];_tmp2B0[0]=& _tmp2B2;({struct _fat_ptr _tmp56B=({const char*_tmp2B1="_%s_def_";_tag_fat(_tmp2B1,sizeof(char),9U);});Cyc_aprintf(_tmp56B,_tag_fat(_tmp2B0,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2B5=({struct Cyc_String_pa_PrintArg_struct _tmp403;_tmp403.tag=0U,_tmp403.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp403;});void*_tmp2B3[1U];_tmp2B3[0]=& _tmp2B5;({struct Cyc___cycFILE*_tmp56D=out_file;struct _fat_ptr _tmp56C=({const char*_tmp2B4="#ifndef %s\n";_tag_fat(_tmp2B4,sizeof(char),12U);});Cyc_fprintf(_tmp56D,_tmp56C,_tag_fat(_tmp2B3,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2B8=({struct Cyc_String_pa_PrintArg_struct _tmp404;_tmp404.tag=0U,_tmp404.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp404;});void*_tmp2B6[1U];_tmp2B6[0]=& _tmp2B8;({struct Cyc___cycFILE*_tmp56F=out_file;struct _fat_ptr _tmp56E=({const char*_tmp2B7="#define %s\n";_tag_fat(_tmp2B7,sizeof(char),12U);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_fat(_tmp2B6,sizeof(void*),1U));});});
# 1742
({struct Cyc_List_List*_tmp570=({struct Cyc_Absyn_Decl*_tmp2B9[1U];_tmp2B9[0]=decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2B9,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp570,out_file);});
({void*_tmp2BA=0U;({struct Cyc___cycFILE*_tmp572=out_file;struct _fat_ptr _tmp571=({const char*_tmp2BB="#endif\n";_tag_fat(_tmp2BB,sizeof(char),8U);});Cyc_fprintf(_tmp572,_tmp571,_tag_fat(_tmp2BA,sizeof(void*),0U));});});}else{
# 1747
({struct Cyc_List_List*_tmp573=({struct Cyc_Absyn_Decl*_tmp2BC[1U];_tmp2BC[0]=decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2BC,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp573,out_file);});}}}}
# 1752
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp576=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp406;_tmp406.tag=0U,({struct _fat_ptr _tmp574=(struct _fat_ptr)({const char*_tmp2C0=macrosfile;_tag_fat(_tmp2C0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C0,1U));});_tmp406.f1=_tmp574;});_tmp406;});void*_tmp2BD[1U];_tmp2BD[0]=& _tmp2BF;({struct _fat_ptr _tmp575=({const char*_tmp2BE="can't open macrosfile %s";_tag_fat(_tmp2BE,sizeof(char),25U);});Cyc_aprintf(_tmp575,_tag_fat(_tmp2BD,sizeof(void*),1U));});});_tmp2C1->f1=_tmp576;});_tmp2C1;}));
# 1755
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);{
struct _tuple26*entry2;
while((entry2=((struct _tuple26*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(l))!= 0){
struct _tuple26*_tmp2C2=(struct _tuple26*)_check_null(entry2);struct _tuple26*_stmttmp20=_tmp2C2;struct _tuple26*_tmp2C3=_stmttmp20;struct _fat_ptr*_tmp2C5;struct _fat_ptr _tmp2C4;_LL7D: _tmp2C4=_tmp2C3->f1;_tmp2C5=_tmp2C3->f2;_LL7E: {struct _fat_ptr line=_tmp2C4;struct _fat_ptr*name=_tmp2C5;
if(((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,name)){
({struct Cyc_String_pa_PrintArg_struct _tmp2C8=({struct Cyc_String_pa_PrintArg_struct _tmp407;_tmp407.tag=0U,_tmp407.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp407;});void*_tmp2C6[1U];_tmp2C6[0]=& _tmp2C8;({struct Cyc___cycFILE*_tmp578=out_file;struct _fat_ptr _tmp577=({const char*_tmp2C7="#ifndef %s\n";_tag_fat(_tmp2C7,sizeof(char),12U);});Cyc_fprintf(_tmp578,_tmp577,_tag_fat(_tmp2C6,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2CB=({struct Cyc_String_pa_PrintArg_struct _tmp408;_tmp408.tag=0U,_tmp408.f1=(struct _fat_ptr)((struct _fat_ptr)line);_tmp408;});void*_tmp2C9[1U];_tmp2C9[0]=& _tmp2CB;({struct Cyc___cycFILE*_tmp57A=out_file;struct _fat_ptr _tmp579=({const char*_tmp2CA="%s\n";_tag_fat(_tmp2CA,sizeof(char),4U);});Cyc_fprintf(_tmp57A,_tmp579,_tag_fat(_tmp2C9,sizeof(void*),1U));});});
({void*_tmp2CC=0U;({struct Cyc___cycFILE*_tmp57C=out_file;struct _fat_ptr _tmp57B=({const char*_tmp2CD="#endif\n";_tag_fat(_tmp2CD,sizeof(char),8U);});Cyc_fprintf(_tmp57C,_tmp57B,_tag_fat(_tmp2CC,sizeof(void*),0U));});});}}}
# 1766
Cyc_fclose(in_file);
if((int)Cyc_mode != (int)Cyc_FINISH);
# 1769
if(hstubs != 0){
struct Cyc_List_List*_tmp2CE=hstubs;struct Cyc_List_List*x=_tmp2CE;for(0;x != 0;x=x->tl){
struct _tuple27*_tmp2CF=(struct _tuple27*)x->hd;struct _tuple27*_stmttmp21=_tmp2CF;struct _tuple27*_tmp2D0=_stmttmp21;struct _fat_ptr _tmp2D2;struct _fat_ptr _tmp2D1;_LL80: _tmp2D1=_tmp2D0->f1;_tmp2D2=_tmp2D0->f2;_LL81: {struct _fat_ptr symbol=_tmp2D1;struct _fat_ptr text=_tmp2D2;
if(({char*_tmp57F=(char*)text.curr;_tmp57F != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp57E=(char*)symbol.curr;_tmp57E == (char*)(_tag_fat(0,0,0)).curr;})||({struct Cyc_Set_Set*_tmp57D=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp57D,({struct _fat_ptr*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));*_tmp2D3=symbol;_tmp2D3;}));})))
# 1775
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);else{
# 1777
({struct Cyc_String_pa_PrintArg_struct _tmp2D6=({struct Cyc_String_pa_PrintArg_struct _tmp409;_tmp409.tag=0U,_tmp409.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp409;});void*_tmp2D4[1U];_tmp2D4[0]=& _tmp2D6;({struct _fat_ptr _tmp580=({const char*_tmp2D5="%s is not supported on this platform\n";_tag_fat(_tmp2D5,sizeof(char),38U);});Cyc_log(_tmp580,_tag_fat(_tmp2D4,sizeof(void*),1U));});});}}}}
# 1780
({void*_tmp2D7=0U;({struct Cyc___cycFILE*_tmp582=out_file;struct _fat_ptr _tmp581=({const char*_tmp2D8="#endif\n";_tag_fat(_tmp2D8,sizeof(char),8U);});Cyc_fprintf(_tmp582,_tmp581,_tag_fat(_tmp2D7,sizeof(void*),0U));});});
if(Cyc_do_setjmp){int _tmp2D9=0;_npop_handler(0U);return _tmp2D9;}else{
Cyc_fclose(out_file);}
# 1785
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2DA=cstubs;struct Cyc_List_List*x=_tmp2DA;for(0;x != 0;x=x->tl){
struct _tuple27*_tmp2DB=(struct _tuple27*)x->hd;struct _tuple27*_stmttmp22=_tmp2DB;struct _tuple27*_tmp2DC=_stmttmp22;struct _fat_ptr _tmp2DE;struct _fat_ptr _tmp2DD;_LL83: _tmp2DD=_tmp2DC->f1;_tmp2DE=_tmp2DC->f2;_LL84: {struct _fat_ptr symbol=_tmp2DD;struct _fat_ptr text=_tmp2DE;
if(({char*_tmp585=(char*)text.curr;_tmp585 != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp584=(char*)symbol.curr;_tmp584 == (char*)(_tag_fat(0,0,0)).curr;})||({struct Cyc_Set_Set*_tmp583=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp583,({struct _fat_ptr*_tmp2DF=_cycalloc(sizeof(*_tmp2DF));*_tmp2DF=symbol;_tmp2DF;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}}
# 1796
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
if(cycstubs != 0){
# 1801
({struct Cyc_String_pa_PrintArg_struct _tmp2E2=({struct Cyc_String_pa_PrintArg_struct _tmp40A;_tmp40A.tag=0U,({struct _fat_ptr _tmp586=(struct _fat_ptr)({const char*_tmp2E3=filename;_tag_fat(_tmp2E3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E3,1U));});_tmp40A.f1=_tmp586;});_tmp40A;});void*_tmp2E0[1U];_tmp2E0[0]=& _tmp2E2;({struct Cyc___cycFILE*_tmp588=out_file;struct _fat_ptr _tmp587=({const char*_tmp2E1="#include <%s>\n\n";_tag_fat(_tmp2E1,sizeof(char),16U);});Cyc_fprintf(_tmp588,_tmp587,_tag_fat(_tmp2E0,sizeof(void*),1U));});});
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2E4=cycstubs;struct Cyc_List_List*x=_tmp2E4;for(0;x != 0;x=x->tl){
struct _tuple27*_tmp2E5=(struct _tuple27*)x->hd;struct _tuple27*_stmttmp23=_tmp2E5;struct _tuple27*_tmp2E6=_stmttmp23;struct _fat_ptr _tmp2E8;struct _fat_ptr _tmp2E7;_LL86: _tmp2E7=_tmp2E6->f1;_tmp2E8=_tmp2E6->f2;_LL87: {struct _fat_ptr symbol=_tmp2E7;struct _fat_ptr text=_tmp2E8;
if(({char*_tmp58B=(char*)text.curr;_tmp58B != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp58A=(char*)symbol.curr;_tmp58A == (char*)(_tag_fat(0,0,0)).curr;})||({struct Cyc_Set_Set*_tmp589=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp589,({struct _fat_ptr*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));*_tmp2E9=symbol;_tmp2E9;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}
# 1809
({void*_tmp2EA=0U;({struct Cyc___cycFILE*_tmp58D=out_file;struct _fat_ptr _tmp58C=({const char*_tmp2EB="\n";_tag_fat(_tmp2EB,sizeof(char),2U);});Cyc_fprintf(_tmp58D,_tmp58C,_tag_fat(_tmp2EA,sizeof(void*),0U));});});}{
# 1812
int _tmp2EC=0;_npop_handler(0U);return _tmp2EC;}}}}}}}}}
# 1442
;_pop_handler();}else{void*_tmp1E0=(void*)Cyc_Core_get_exn_thrown();void*_tmp2F1=_tmp1E0;void*_tmp2F2;struct _fat_ptr _tmp2F3;struct _fat_ptr _tmp2F4;struct _fat_ptr _tmp2F5;struct _fat_ptr _tmp2F6;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2F1)->tag == Cyc_Core_Impossible){_LL89: _tmp2F6=((struct Cyc_Core_Impossible_exn_struct*)_tmp2F1)->f1;_LL8A: {struct _fat_ptr s=_tmp2F6;
# 1816
({struct Cyc_String_pa_PrintArg_struct _tmp2F9=({struct Cyc_String_pa_PrintArg_struct _tmp40C;_tmp40C.tag=0U,_tmp40C.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp40C;});void*_tmp2F7[1U];_tmp2F7[0]=& _tmp2F9;({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _fat_ptr _tmp58E=({const char*_tmp2F8="Got Core::Impossible(%s)\n";_tag_fat(_tmp2F8,sizeof(char),26U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_fat(_tmp2F7,sizeof(void*),1U));});});goto _LL88;}}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2F1)->tag == Cyc_Dict_Absent){_LL8B: _LL8C:
# 1818
({void*_tmp2FA=0U;({struct Cyc___cycFILE*_tmp591=Cyc_stderr;struct _fat_ptr _tmp590=({const char*_tmp2FB="Got Dict::Absent\n";_tag_fat(_tmp2FB,sizeof(char),18U);});Cyc_fprintf(_tmp591,_tmp590,_tag_fat(_tmp2FA,sizeof(void*),0U));});});goto _LL88;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2F1)->tag == Cyc_Core_Failure){_LL8D: _tmp2F5=((struct Cyc_Core_Failure_exn_struct*)_tmp2F1)->f1;_LL8E: {struct _fat_ptr s=_tmp2F5;
# 1820
({struct Cyc_String_pa_PrintArg_struct _tmp2FE=({struct Cyc_String_pa_PrintArg_struct _tmp40D;_tmp40D.tag=0U,_tmp40D.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp40D;});void*_tmp2FC[1U];_tmp2FC[0]=& _tmp2FE;({struct Cyc___cycFILE*_tmp593=Cyc_stderr;struct _fat_ptr _tmp592=({const char*_tmp2FD="Got Core::Failure(%s)\n";_tag_fat(_tmp2FD,sizeof(char),23U);});Cyc_fprintf(_tmp593,_tmp592,_tag_fat(_tmp2FC,sizeof(void*),1U));});});goto _LL88;}}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2F1)->tag == Cyc_Core_Invalid_argument){_LL8F: _tmp2F4=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2F1)->f1;_LL90: {struct _fat_ptr s=_tmp2F4;
# 1822
({struct Cyc_String_pa_PrintArg_struct _tmp301=({struct Cyc_String_pa_PrintArg_struct _tmp40E;_tmp40E.tag=0U,_tmp40E.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp40E;});void*_tmp2FF[1U];_tmp2FF[0]=& _tmp301;({struct Cyc___cycFILE*_tmp595=Cyc_stderr;struct _fat_ptr _tmp594=({const char*_tmp300="Got Invalid_argument(%s)\n";_tag_fat(_tmp300,sizeof(char),26U);});Cyc_fprintf(_tmp595,_tmp594,_tag_fat(_tmp2FF,sizeof(void*),1U));});});goto _LL88;}}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2F1)->tag == Cyc_Core_Not_found){_LL91: _LL92:
# 1824
({void*_tmp302=0U;({struct Cyc___cycFILE*_tmp597=Cyc_stderr;struct _fat_ptr _tmp596=({const char*_tmp303="Got Not_found\n";_tag_fat(_tmp303,sizeof(char),15U);});Cyc_fprintf(_tmp597,_tmp596,_tag_fat(_tmp302,sizeof(void*),0U));});});goto _LL88;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2F1)->tag == Cyc_NO_SUPPORT){_LL93: _tmp2F3=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2F1)->f1;_LL94: {struct _fat_ptr s=_tmp2F3;
# 1826
({struct Cyc_String_pa_PrintArg_struct _tmp306=({struct Cyc_String_pa_PrintArg_struct _tmp40F;_tmp40F.tag=0U,_tmp40F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp40F;});void*_tmp304[1U];_tmp304[0]=& _tmp306;({struct Cyc___cycFILE*_tmp599=Cyc_stderr;struct _fat_ptr _tmp598=({const char*_tmp305="No support because %s\n";_tag_fat(_tmp305,sizeof(char),23U);});Cyc_fprintf(_tmp599,_tmp598,_tag_fat(_tmp304,sizeof(void*),1U));});});goto _LL88;}}else{_LL95: _tmp2F2=_tmp2F1;_LL96: {void*x=_tmp2F2;
# 1828
({void*_tmp307=0U;({struct Cyc___cycFILE*_tmp59B=Cyc_stderr;struct _fat_ptr _tmp59A=({const char*_tmp308="Got unknown exception\n";_tag_fat(_tmp308,sizeof(char),23U);});Cyc_fprintf(_tmp59B,_tmp59A,_tag_fat(_tmp307,sizeof(void*),0U));});});
Cyc_Core_rethrow(x);}}}}}}}_LL88:;}}}
# 1834
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp30B=({struct Cyc_String_pa_PrintArg_struct _tmp410;_tmp410.tag=0U,({struct _fat_ptr _tmp59C=(struct _fat_ptr)({const char*_tmp30C=filename;_tag_fat(_tmp30C,sizeof(char),_get_zero_arr_size_char((void*)_tmp30C,1U));});_tmp410.f1=_tmp59C;});_tmp410;});void*_tmp309[1U];_tmp309[0]=& _tmp30B;({struct Cyc___cycFILE*_tmp59E=Cyc_stderr;struct _fat_ptr _tmp59D=({const char*_tmp30A="Error: could not create file %s\n";_tag_fat(_tmp30A,sizeof(char),33U);});Cyc_fprintf(_tmp59E,_tmp59D,_tag_fat(_tmp309,sizeof(void*),1U));});});
return 1;}
# 1839
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp30F=({struct Cyc_String_pa_PrintArg_struct _tmp411;_tmp411.tag=0U,({
# 1842
struct _fat_ptr _tmp59F=(struct _fat_ptr)({const char*_tmp310=filename;_tag_fat(_tmp310,sizeof(char),_get_zero_arr_size_char((void*)_tmp310,1U));});_tmp411.f1=_tmp59F;});_tmp411;});void*_tmp30D[1U];_tmp30D[0]=& _tmp30F;({struct Cyc___cycFILE*_tmp5A1=out_file;struct _fat_ptr _tmp5A0=({const char*_tmp30E="#error -- %s is not supported on this platform\n";_tag_fat(_tmp30E,sizeof(char),48U);});Cyc_fprintf(_tmp5A1,_tmp5A0,_tag_fat(_tmp30D,sizeof(void*),1U));});});
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp313=({struct Cyc_String_pa_PrintArg_struct _tmp412;_tmp412.tag=0U,({
struct _fat_ptr _tmp5A2=(struct _fat_ptr)({const char*_tmp314=filename;_tag_fat(_tmp314,sizeof(char),_get_zero_arr_size_char((void*)_tmp314,1U));});_tmp412.f1=_tmp5A2;});_tmp412;});void*_tmp311[1U];_tmp311[0]=& _tmp313;({struct Cyc___cycFILE*_tmp5A4=Cyc_stderr;struct _fat_ptr _tmp5A3=({const char*_tmp312="Warning: %s will not be supported on this platform\n";_tag_fat(_tmp312,sizeof(char),52U);});Cyc_fprintf(_tmp5A4,_tmp5A3,_tag_fat(_tmp311,sizeof(void*),1U));});});
({void*_tmp315=0U;({struct _fat_ptr _tmp5A5=({const char*_tmp316="Not supported on this platform\n";_tag_fat(_tmp316,sizeof(char),32U);});Cyc_log(_tmp5A5,_tag_fat(_tmp315,sizeof(void*),0U));});});
# 1853
return 0;}}
# 1857
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp32E=({struct Cyc_String_pa_PrintArg_struct _tmp419;_tmp419.tag=0U,({struct _fat_ptr _tmp5A6=(struct _fat_ptr)({const char*_tmp32F=file;_tag_fat(_tmp32F,sizeof(char),_get_zero_arr_size_char((void*)_tmp32F,1U));});_tmp419.f1=_tmp5A6;});_tmp419;});void*_tmp32C[1U];_tmp32C[0]=& _tmp32E;({struct Cyc___cycFILE*_tmp5A8=Cyc_stderr;struct _fat_ptr _tmp5A7=({const char*_tmp32D="Processing %s\n";_tag_fat(_tmp32D,sizeof(char),15U);});Cyc_fprintf(_tmp5A8,_tmp5A7,_tag_fat(_tmp32C,sizeof(void*),1U));});});{
struct Cyc___cycFILE*_tmp330=Cyc_fopen(file,"r");struct Cyc___cycFILE*maybe=_tmp330;
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp333=({struct Cyc_String_pa_PrintArg_struct _tmp41A;_tmp41A.tag=0U,({struct _fat_ptr _tmp5A9=(struct _fat_ptr)({const char*_tmp334=file;_tag_fat(_tmp334,sizeof(char),_get_zero_arr_size_char((void*)_tmp334,1U));});_tmp41A.f1=_tmp5A9;});_tmp41A;});void*_tmp331[1U];_tmp331[0]=& _tmp333;({struct Cyc___cycFILE*_tmp5AB=Cyc_stderr;struct _fat_ptr _tmp5AA=({const char*_tmp332="Error: could not open %s\n";_tag_fat(_tmp332,sizeof(char),26U);});Cyc_fprintf(_tmp5AB,_tmp5AA,_tag_fat(_tmp331,sizeof(void*),1U));});});
return 1;}{
# 1865
struct Cyc___cycFILE*_tmp335=maybe;struct Cyc___cycFILE*in_file=_tmp335;
# 1869
struct _fat_ptr buf=({char*_tmp353=({unsigned _tmp352=1024U + 1U;char*_tmp351=_cycalloc_atomic(_check_times(_tmp352,sizeof(char)));({{unsigned _tmp420=1024U;unsigned i;for(i=0;i < _tmp420;++ i){_tmp351[i]='\000';}_tmp351[_tmp420]=0;}0;});_tmp351;});_tag_fat(_tmp353,sizeof(char),1025U);});
struct _fat_ptr _tmp336=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp336;
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp339=({struct Cyc_String_pa_PrintArg_struct _tmp41B;_tmp41B.tag=0U,({struct _fat_ptr _tmp5AC=(struct _fat_ptr)({const char*_tmp33A=dir;_tag_fat(_tmp33A,sizeof(char),_get_zero_arr_size_char((void*)_tmp33A,1U));});_tmp41B.f1=_tmp5AC;});_tmp41B;});void*_tmp337[1U];_tmp337[0]=& _tmp339;({struct Cyc___cycFILE*_tmp5AE=Cyc_stderr;struct _fat_ptr _tmp5AD=({const char*_tmp338="Error: can't change directory to %s\n";_tag_fat(_tmp338,sizeof(char),37U);});Cyc_fprintf(_tmp5AE,_tmp5AD,_tag_fat(_tmp337,sizeof(void*),1U));});});
return 1;}}
# 1877
if((int)Cyc_mode == (int)Cyc_GATHER){
# 1879
struct _fat_ptr _tmp33B=({struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp41E;_tmp41E.tag=0U,_tmp41E.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp41E;});struct Cyc_String_pa_PrintArg_struct _tmp342=({struct Cyc_String_pa_PrintArg_struct _tmp41D;_tmp41D.tag=0U,_tmp41D.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp41D;});void*_tmp33F[2U];_tmp33F[0]=& _tmp341,_tmp33F[1]=& _tmp342;({struct _fat_ptr _tmp5AF=({const char*_tmp340="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp340,sizeof(char),39U);});Cyc_aprintf(_tmp5AF,_tag_fat(_tmp33F,sizeof(void*),2U));});});struct _fat_ptr cmd=_tmp33B;
# 1881
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp33E=({struct Cyc_String_pa_PrintArg_struct _tmp41C;_tmp41C.tag=0U,_tmp41C.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp41C;});void*_tmp33C[1U];_tmp33C[0]=& _tmp33E;({struct Cyc___cycFILE*_tmp5B1=Cyc_stderr;struct _fat_ptr _tmp5B0=({const char*_tmp33D="%s\n";_tag_fat(_tmp33D,sizeof(char),4U);});Cyc_fprintf(_tmp5B1,_tmp5B0,_tag_fat(_tmp33C,sizeof(void*),1U));});});
system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)));}{
# 1886
struct Cyc_Lexing_lexbuf*_tmp343=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp343;
struct _tuple29*entry;
while((entry=((struct _tuple29*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(l))!= 0){
struct _tuple29*_tmp344=(struct _tuple29*)_check_null(entry);struct _tuple29*_stmttmp24=_tmp344;struct _tuple29*_tmp345=_stmttmp24;struct Cyc_List_List*_tmp34D;struct Cyc_List_List*_tmp34C;struct Cyc_List_List*_tmp34B;struct Cyc_List_List*_tmp34A;struct Cyc_List_List*_tmp349;struct Cyc_List_List*_tmp348;struct Cyc_List_List*_tmp347;struct _fat_ptr _tmp346;_LL1: _tmp346=_tmp345->f1;_tmp347=_tmp345->f2;_tmp348=_tmp345->f3;_tmp349=_tmp345->f4;_tmp34A=_tmp345->f5;_tmp34B=_tmp345->f6;_tmp34C=_tmp345->f7;_tmp34D=_tmp345->f8;_LL2: {struct _fat_ptr headerfile=_tmp346;struct Cyc_List_List*start_symbols=_tmp347;struct Cyc_List_List*user_defs=_tmp348;struct Cyc_List_List*omit_symbols=_tmp349;struct Cyc_List_List*hstubs=_tmp34A;struct Cyc_List_List*cstubs=_tmp34B;struct Cyc_List_List*cycstubs=_tmp34C;struct Cyc_List_List*cpp_insert=_tmp34D;
# 1891
if(Cyc_process_file((const char*)_check_null(_untag_fat_ptr(headerfile,sizeof(char),1U)),start_symbols,user_defs,omit_symbols,hstubs,cstubs,cycstubs,cpp_insert))
# 1893
return 1;}}
# 1895
Cyc_fclose(in_file);
# 1897
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp350=({struct Cyc_String_pa_PrintArg_struct _tmp41F;_tmp41F.tag=0U,_tmp41F.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp41F;});void*_tmp34E[1U];_tmp34E[0]=& _tmp350;({struct Cyc___cycFILE*_tmp5B3=Cyc_stderr;struct _fat_ptr _tmp5B2=({const char*_tmp34F="Error: could not change directory to %s\n";_tag_fat(_tmp34F,sizeof(char),41U);});Cyc_fprintf(_tmp5B3,_tmp5B2,_tag_fat(_tmp34E,sizeof(void*),1U));});});
return 1;}}
# 1903
return 0;}}}}
# 1907
int Cyc_process_setjmp(const char*dir){
# 1910
struct _fat_ptr buf=({char*_tmp365=({unsigned _tmp364=1024U + 1U;char*_tmp363=_cycalloc_atomic(_check_times(_tmp364,sizeof(char)));({{unsigned _tmp423=1024U;unsigned i;for(i=0;i < _tmp423;++ i){_tmp363[i]='\000';}_tmp363[_tmp423]=0;}0;});_tmp363;});_tag_fat(_tmp365,sizeof(char),1025U);});
struct _fat_ptr _tmp354=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp354;
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp357=({struct Cyc_String_pa_PrintArg_struct _tmp421;_tmp421.tag=0U,({struct _fat_ptr _tmp5B4=(struct _fat_ptr)({const char*_tmp358=dir;_tag_fat(_tmp358,sizeof(char),_get_zero_arr_size_char((void*)_tmp358,1U));});_tmp421.f1=_tmp5B4;});_tmp421;});void*_tmp355[1U];_tmp355[0]=& _tmp357;({struct Cyc___cycFILE*_tmp5B6=Cyc_stderr;struct _fat_ptr _tmp5B5=({const char*_tmp356="Error: can't change directory to %s\n";_tag_fat(_tmp356,sizeof(char),37U);});Cyc_fprintf(_tmp5B6,_tmp5B5,_tag_fat(_tmp355,sizeof(void*),1U));});});
return 1;}
# 1916
if(({struct Cyc_List_List*_tmp5BC=({struct _fat_ptr*_tmp359[1U];({struct _fat_ptr*_tmp5B8=({struct _fat_ptr*_tmp35B=_cycalloc(sizeof(*_tmp35B));({struct _fat_ptr _tmp5B7=({const char*_tmp35A="jmp_buf";_tag_fat(_tmp35A,sizeof(char),8U);});*_tmp35B=_tmp5B7;});_tmp35B;});_tmp359[0]=_tmp5B8;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp359,sizeof(struct _fat_ptr*),1U));});Cyc_process_file("setjmp.h",_tmp5BC,0,0,({struct _tuple27*_tmp35C[1U];({struct _tuple27*_tmp5BB=({struct _tuple27*_tmp35F=_cycalloc(sizeof(*_tmp35F));
({struct _fat_ptr _tmp5BA=({const char*_tmp35D="setjmp";_tag_fat(_tmp35D,sizeof(char),7U);});_tmp35F->f1=_tmp5BA;}),({struct _fat_ptr _tmp5B9=({const char*_tmp35E="extern int setjmp(jmp_buf);\n";_tag_fat(_tmp35E,sizeof(char),29U);});_tmp35F->f2=_tmp5B9;});_tmp35F;});_tmp35C[0]=_tmp5BB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp35C,sizeof(struct _tuple27*),1U));}),0,0,0);}))
# 1919
return 1;
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp362=({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0U,_tmp422.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp422;});void*_tmp360[1U];_tmp360[0]=& _tmp362;({struct Cyc___cycFILE*_tmp5BE=Cyc_stderr;struct _fat_ptr _tmp5BD=({const char*_tmp361="Error: could not change directory to %s\n";_tag_fat(_tmp361,sizeof(char),41U);});Cyc_fprintf(_tmp5BE,_tmp5BD,_tag_fat(_tmp360,sizeof(void*),1U));});});
return 1;}
# 1924
return 0;}static char _tmp366[13U]="BUILDLIB.OUT";
# 1928
static struct _fat_ptr Cyc_output_dir={_tmp366,_tmp366,_tmp366 + 13U};
static void Cyc_set_output_dir(struct _fat_ptr s){
Cyc_output_dir=s;}
# 1932
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _fat_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp367=_cycalloc(sizeof(*_tmp367));_tmp367->hd=(const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)),_tmp367->tl=Cyc_spec_files;_tmp367;});}
# 1936
static int Cyc_no_other(struct _fat_ptr s){return 0;}
static void Cyc_set_GATHER (void){
Cyc_mode=Cyc_GATHER;}
# 1940
static void Cyc_set_GATHERSCRIPT (void){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1943
static void Cyc_set_FINISH (void){
Cyc_mode=Cyc_FINISH;}
# 1946
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp369=_cycalloc(sizeof(*_tmp369));({struct _fat_ptr*_tmp5BF=({struct _fat_ptr*_tmp368=_cycalloc(sizeof(*_tmp368));*_tmp368=s;_tmp368;});_tmp369->hd=_tmp5BF;}),_tmp369->tl=Cyc_cppargs;_tmp369;});}
# 1949
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp36C=({struct Cyc_String_pa_PrintArg_struct _tmp424;_tmp424.tag=0U,_tmp424.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp424;});void*_tmp36A[1U];_tmp36A[0]=& _tmp36C;({struct Cyc___cycFILE*_tmp5C1=Cyc_stderr;struct _fat_ptr _tmp5C0=({const char*_tmp36B="Unsupported option %s\n";_tag_fat(_tmp36B,sizeof(char),23U);});Cyc_fprintf(_tmp5C1,_tmp5C0,_tag_fat(_tmp36A,sizeof(void*),1U));});});
Cyc_badparse=1;}
# 1958
extern void GC_blacklist_warn_clear (void);struct _tuple31{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
int Cyc_main(int argc,struct _fat_ptr argv){
GC_blacklist_warn_clear();{
# 1962
struct Cyc_List_List*options=({struct _tuple31*_tmp3A4[9U];({
struct _tuple31*_tmp5EE=({struct _tuple31*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({struct _fat_ptr _tmp5ED=({const char*_tmp3A5="-d";_tag_fat(_tmp3A5,sizeof(char),3U);});_tmp3A9->f1=_tmp5ED;}),_tmp3A9->f2=0,({struct _fat_ptr _tmp5EC=({const char*_tmp3A6=" <file>";_tag_fat(_tmp3A6,sizeof(char),8U);});_tmp3A9->f3=_tmp5EC;}),({
void*_tmp5EB=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->tag=5U,_tmp3A7->f1=Cyc_set_output_dir;_tmp3A7;});_tmp3A9->f4=_tmp5EB;}),({
struct _fat_ptr _tmp5EA=({const char*_tmp3A8="Set the output directory to <file>";_tag_fat(_tmp3A8,sizeof(char),35U);});_tmp3A9->f5=_tmp5EA;});_tmp3A9;});
# 1963
_tmp3A4[0]=_tmp5EE;}),({
# 1966
struct _tuple31*_tmp5E9=({struct _tuple31*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));({struct _fat_ptr _tmp5E8=({const char*_tmp3AA="-gather";_tag_fat(_tmp3AA,sizeof(char),8U);});_tmp3AE->f1=_tmp5E8;}),_tmp3AE->f2=0,({struct _fat_ptr _tmp5E7=({const char*_tmp3AB="";_tag_fat(_tmp3AB,sizeof(char),1U);});_tmp3AE->f3=_tmp5E7;}),({
void*_tmp5E6=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->tag=0U,_tmp3AC->f1=Cyc_set_GATHER;_tmp3AC;});_tmp3AE->f4=_tmp5E6;}),({
struct _fat_ptr _tmp5E5=({const char*_tmp3AD="Gather C library info but don't produce Cyclone headers";_tag_fat(_tmp3AD,sizeof(char),56U);});_tmp3AE->f5=_tmp5E5;});_tmp3AE;});
# 1966
_tmp3A4[1]=_tmp5E9;}),({
# 1969
struct _tuple31*_tmp5E4=({struct _tuple31*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));({struct _fat_ptr _tmp5E3=({const char*_tmp3AF="-gatherscript";_tag_fat(_tmp3AF,sizeof(char),14U);});_tmp3B3->f1=_tmp5E3;}),_tmp3B3->f2=0,({struct _fat_ptr _tmp5E2=({const char*_tmp3B0="";_tag_fat(_tmp3B0,sizeof(char),1U);});_tmp3B3->f3=_tmp5E2;}),({
void*_tmp5E1=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->tag=0U,_tmp3B1->f1=Cyc_set_GATHERSCRIPT;_tmp3B1;});_tmp3B3->f4=_tmp5E1;}),({
struct _fat_ptr _tmp5E0=({const char*_tmp3B2="Produce a script to gather C library info";_tag_fat(_tmp3B2,sizeof(char),42U);});_tmp3B3->f5=_tmp5E0;});_tmp3B3;});
# 1969
_tmp3A4[2]=_tmp5E4;}),({
# 1972
struct _tuple31*_tmp5DF=({struct _tuple31*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct _fat_ptr _tmp5DE=({const char*_tmp3B4="-finish";_tag_fat(_tmp3B4,sizeof(char),8U);});_tmp3B8->f1=_tmp5DE;}),_tmp3B8->f2=0,({struct _fat_ptr _tmp5DD=({const char*_tmp3B5="";_tag_fat(_tmp3B5,sizeof(char),1U);});_tmp3B8->f3=_tmp5DD;}),({
void*_tmp5DC=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=0U,_tmp3B6->f1=Cyc_set_FINISH;_tmp3B6;});_tmp3B8->f4=_tmp5DC;}),({
struct _fat_ptr _tmp5DB=({const char*_tmp3B7="Produce Cyclone headers from pre-gathered C library info";_tag_fat(_tmp3B7,sizeof(char),57U);});_tmp3B8->f5=_tmp5DB;});_tmp3B8;});
# 1972
_tmp3A4[3]=_tmp5DF;}),({
# 1975
struct _tuple31*_tmp5DA=({struct _tuple31*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({struct _fat_ptr _tmp5D9=({const char*_tmp3B9="-setjmp";_tag_fat(_tmp3B9,sizeof(char),8U);});_tmp3BD->f1=_tmp5D9;}),_tmp3BD->f2=0,({struct _fat_ptr _tmp5D8=({const char*_tmp3BA="";_tag_fat(_tmp3BA,sizeof(char),1U);});_tmp3BD->f3=_tmp5D8;}),({
void*_tmp5D7=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->tag=3U,_tmp3BB->f1=& Cyc_do_setjmp;_tmp3BB;});_tmp3BD->f4=_tmp5D7;}),({
# 1980
struct _fat_ptr _tmp5D6=({const char*_tmp3BC="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_fat(_tmp3BC,sizeof(char),186U);});_tmp3BD->f5=_tmp5D6;});_tmp3BD;});
# 1975
_tmp3A4[4]=_tmp5DA;}),({
# 1981
struct _tuple31*_tmp5D5=({struct _tuple31*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct _fat_ptr _tmp5D4=({const char*_tmp3BE="-b";_tag_fat(_tmp3BE,sizeof(char),3U);});_tmp3C2->f1=_tmp5D4;}),_tmp3C2->f2=0,({struct _fat_ptr _tmp5D3=({const char*_tmp3BF=" <machine>";_tag_fat(_tmp3BF,sizeof(char),11U);});_tmp3C2->f3=_tmp5D3;}),({
void*_tmp5D2=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->tag=5U,_tmp3C0->f1=Cyc_Specsfile_set_target_arch;_tmp3C0;});_tmp3C2->f4=_tmp5D2;}),({
struct _fat_ptr _tmp5D1=({const char*_tmp3C1="Set the target machine for compilation to <machine>";_tag_fat(_tmp3C1,sizeof(char),52U);});_tmp3C2->f5=_tmp5D1;});_tmp3C2;});
# 1981
_tmp3A4[5]=_tmp5D5;}),({
# 1984
struct _tuple31*_tmp5D0=({struct _tuple31*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct _fat_ptr _tmp5CF=({const char*_tmp3C3="-B";_tag_fat(_tmp3C3,sizeof(char),3U);});_tmp3C7->f1=_tmp5CF;}),_tmp3C7->f2=1,({struct _fat_ptr _tmp5CE=({const char*_tmp3C4="<file>";_tag_fat(_tmp3C4,sizeof(char),7U);});_tmp3C7->f3=_tmp5CE;}),({
void*_tmp5CD=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->tag=1U,_tmp3C5->f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp3C5;});_tmp3C7->f4=_tmp5CD;}),({
struct _fat_ptr _tmp5CC=({const char*_tmp3C6="Add to the list of directories to search for compiler files";_tag_fat(_tmp3C6,sizeof(char),60U);});_tmp3C7->f5=_tmp5CC;});_tmp3C7;});
# 1984
_tmp3A4[6]=_tmp5D0;}),({
# 1987
struct _tuple31*_tmp5CB=({struct _tuple31*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));({struct _fat_ptr _tmp5CA=({const char*_tmp3C8="-v";_tag_fat(_tmp3C8,sizeof(char),3U);});_tmp3CC->f1=_tmp5CA;}),_tmp3CC->f2=0,({struct _fat_ptr _tmp5C9=({const char*_tmp3C9="";_tag_fat(_tmp3C9,sizeof(char),1U);});_tmp3CC->f3=_tmp5C9;}),({
void*_tmp5C8=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->tag=3U,_tmp3CA->f1=& Cyc_verbose;_tmp3CA;});_tmp3CC->f4=_tmp5C8;}),({
struct _fat_ptr _tmp5C7=({const char*_tmp3CB="Verbose operation";_tag_fat(_tmp3CB,sizeof(char),18U);});_tmp3CC->f5=_tmp5C7;});_tmp3CC;});
# 1987
_tmp3A4[7]=_tmp5CB;}),({
# 1990
struct _tuple31*_tmp5C6=({struct _tuple31*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct _fat_ptr _tmp5C5=({const char*_tmp3CD="-";_tag_fat(_tmp3CD,sizeof(char),2U);});_tmp3D1->f1=_tmp5C5;}),_tmp3D1->f2=1,({struct _fat_ptr _tmp5C4=({const char*_tmp3CE="";_tag_fat(_tmp3CE,sizeof(char),1U);});_tmp3D1->f3=_tmp5C4;}),({
void*_tmp5C3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->tag=1U,_tmp3CF->f1=Cyc_add_cpparg;_tmp3CF;});_tmp3D1->f4=_tmp5C3;}),({
struct _fat_ptr _tmp5C2=({const char*_tmp3D0="";_tag_fat(_tmp3D0,sizeof(char),1U);});_tmp3D1->f5=_tmp5C2;});_tmp3D1;});
# 1990
_tmp3A4[8]=_tmp5C6;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3A4,sizeof(struct _tuple31*),9U));});
# 1995
struct _fat_ptr _tmp36D=({struct Cyc_List_List*_tmp5F0=options;struct _fat_ptr _tmp5EF=({const char*_tmp3A3="Options:";_tag_fat(_tmp3A3,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp5F0,Cyc_add_spec_file,Cyc_no_other,_tmp5EF,argv);});struct _fat_ptr otherargs=_tmp36D;
# 1997
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp5F2=options;struct _fat_ptr _tmp5F1=({const char*_tmp36E="Options:";_tag_fat(_tmp36E,sizeof(char),9U);});Cyc_Arg_parse(_tmp5F2,Cyc_add_spec_file,Cyc_no_other,_tmp5F1,otherargs);});
if((((Cyc_badparse ||
 !Cyc_do_setjmp && Cyc_spec_files == 0)||
 Cyc_do_setjmp && Cyc_spec_files != 0)||
 Cyc_do_setjmp &&(int)Cyc_mode == (int)Cyc_GATHER)||
 Cyc_do_setjmp &&(int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_List_List*_tmp5F3=options;Cyc_Arg_usage(_tmp5F3,({const char*_tmp36F="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_fat(_tmp36F,sizeof(char),59U);}));});
# 2007
return 1;}{
# 2013
struct _fat_ptr _tmp370=Cyc_Specsfile_find_in_arch_path(({const char*_tmp3A2="cycspecs";_tag_fat(_tmp3A2,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp370;
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp373=({struct Cyc_String_pa_PrintArg_struct _tmp425;_tmp425.tag=0U,_tmp425.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp425;});void*_tmp371[1U];_tmp371[0]=& _tmp373;({struct Cyc___cycFILE*_tmp5F5=Cyc_stderr;struct _fat_ptr _tmp5F4=({const char*_tmp372="Reading from specs file %s\n";_tag_fat(_tmp372,sizeof(char),28U);});Cyc_fprintf(_tmp5F5,_tmp5F4,_tag_fat(_tmp371,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp374=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp374;
Cyc_target_cflags=({struct Cyc_List_List*_tmp5F6=specs;Cyc_Specsfile_get_spec(_tmp5F6,({const char*_tmp375="cyclone_target_cflags";_tag_fat(_tmp375,sizeof(char),22U);}));});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp378=({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0U,_tmp426.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp426;});void*_tmp376[1U];_tmp376[0]=& _tmp378;({struct Cyc___cycFILE*_tmp5F8=Cyc_stderr;struct _fat_ptr _tmp5F7=({const char*_tmp377="Target cflags are %s\n";_tag_fat(_tmp377,sizeof(char),22U);});Cyc_fprintf(_tmp5F8,_tmp5F7,_tag_fat(_tmp376,sizeof(void*),1U));});});
Cyc_cyclone_cc=({struct Cyc_List_List*_tmp5F9=specs;Cyc_Specsfile_get_spec(_tmp5F9,({const char*_tmp379="cyclone_cc";_tag_fat(_tmp379,sizeof(char),11U);}));});
if(!((unsigned)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp37A="gcc";_tag_fat(_tmp37A,sizeof(char),4U);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp37D=({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0U,_tmp427.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp427;});void*_tmp37B[1U];_tmp37B[0]=& _tmp37D;({struct Cyc___cycFILE*_tmp5FB=Cyc_stderr;struct _fat_ptr _tmp5FA=({const char*_tmp37C="C compiler is %s\n";_tag_fat(_tmp37C,sizeof(char),18U);});Cyc_fprintf(_tmp5FB,_tmp5FA,_tag_fat(_tmp37B,sizeof(void*),1U));});});
# 2022
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp37E=0U;({struct Cyc___cycFILE*_tmp5FD=Cyc_stderr;struct _fat_ptr _tmp5FC=({const char*_tmp37F="Creating BUILDLIB.sh\n";_tag_fat(_tmp37F,sizeof(char),22U);});Cyc_fprintf(_tmp5FD,_tmp5FC,_tag_fat(_tmp37E,sizeof(void*),0U));});});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned)Cyc_script_file)){
({void*_tmp380=0U;({struct Cyc___cycFILE*_tmp5FF=Cyc_stderr;struct _fat_ptr _tmp5FE=({const char*_tmp381="Could not create file BUILDLIB.sh\n";_tag_fat(_tmp381,sizeof(char),35U);});Cyc_fprintf(_tmp5FF,_tmp5FE,_tag_fat(_tmp380,sizeof(void*),0U));});});
exit(1);}
# 2030
({void*_tmp382=0U;({struct _fat_ptr _tmp600=({const char*_tmp383="#!/bin/sh\n";_tag_fat(_tmp383,sizeof(char),11U);});Cyc_prscript(_tmp600,_tag_fat(_tmp382,sizeof(void*),0U));});});
({void*_tmp384=0U;({struct _fat_ptr _tmp601=({const char*_tmp385="GCC=\"gcc\"\n";_tag_fat(_tmp385,sizeof(char),11U);});Cyc_prscript(_tmp601,_tag_fat(_tmp384,sizeof(void*),0U));});});}
# 2035
if(Cyc_force_directory_prefixes(Cyc_output_dir)|| Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp388=({struct Cyc_String_pa_PrintArg_struct _tmp428;_tmp428.tag=0U,_tmp428.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp428;});void*_tmp386[1U];_tmp386[0]=& _tmp388;({struct Cyc___cycFILE*_tmp603=Cyc_stderr;struct _fat_ptr _tmp602=({const char*_tmp387="Error: could not create directory %s\n";_tag_fat(_tmp387,sizeof(char),38U);});Cyc_fprintf(_tmp603,_tmp602,_tag_fat(_tmp386,sizeof(void*),1U));});});
return 1;}
# 2039
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp38B=({struct Cyc_String_pa_PrintArg_struct _tmp429;_tmp429.tag=0U,_tmp429.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp429;});void*_tmp389[1U];_tmp389[0]=& _tmp38B;({struct Cyc___cycFILE*_tmp605=Cyc_stderr;struct _fat_ptr _tmp604=({const char*_tmp38A="Output directory is %s\n";_tag_fat(_tmp38A,sizeof(char),24U);});Cyc_fprintf(_tmp605,_tmp604,_tag_fat(_tmp389,sizeof(void*),1U));});});
# 2042
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp38E=({struct Cyc_String_pa_PrintArg_struct _tmp42A;_tmp42A.tag=0U,_tmp42A.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42A;});void*_tmp38C[1U];_tmp38C[0]=& _tmp38E;({struct _fat_ptr _tmp606=({const char*_tmp38D="cd %s\n";_tag_fat(_tmp38D,sizeof(char),7U);});Cyc_prscript(_tmp606,_tag_fat(_tmp38C,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp391=({struct Cyc_String_pa_PrintArg_struct _tmp42B;_tmp42B.tag=0U,_tmp42B.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp42B;});void*_tmp38F[1U];_tmp38F[0]=& _tmp391;({struct _fat_ptr _tmp607=({const char*_tmp390="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp390,sizeof(char),41U);});Cyc_prscript(_tmp607,_tag_fat(_tmp38F,sizeof(void*),1U));});});}
# 2048
if(!Cyc_gathering()){
# 2051
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp608=Cyc_output_dir;Cyc_Filename_concat(_tmp608,({const char*_tmp392="BUILDLIB.LOG";_tag_fat(_tmp392,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp42C;_tmp42C.tag=0U,_tmp42C.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42C;});void*_tmp393[1U];_tmp393[0]=& _tmp395;({struct Cyc___cycFILE*_tmp60A=Cyc_stderr;struct _fat_ptr _tmp609=({const char*_tmp394="Error: could not create log file in directory %s\n";_tag_fat(_tmp394,sizeof(char),50U);});Cyc_fprintf(_tmp60A,_tmp609,_tag_fat(_tmp393,sizeof(void*),1U));});});
return 1;}
# 2057
if(!Cyc_do_setjmp){
# 2059
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp60B=Cyc_output_dir;Cyc_Filename_concat(_tmp60B,({const char*_tmp396="cstubs.c";_tag_fat(_tmp396,sizeof(char),9U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp399=({struct Cyc_String_pa_PrintArg_struct _tmp42D;_tmp42D.tag=0U,_tmp42D.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42D;});void*_tmp397[1U];_tmp397[0]=& _tmp399;({struct Cyc___cycFILE*_tmp60D=Cyc_stderr;struct _fat_ptr _tmp60C=({const char*_tmp398="Error: could not create cstubs.c in directory %s\n";_tag_fat(_tmp398,sizeof(char),50U);});Cyc_fprintf(_tmp60D,_tmp60C,_tag_fat(_tmp397,sizeof(void*),1U));});});
return 1;}
# 2066
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp60E=Cyc_output_dir;Cyc_Filename_concat(_tmp60E,({const char*_tmp39A="cycstubs.cyc";_tag_fat(_tmp39A,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp39D=({struct Cyc_String_pa_PrintArg_struct _tmp42E;_tmp42E.tag=0U,_tmp42E.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42E;});void*_tmp39B[1U];_tmp39B[0]=& _tmp39D;({struct Cyc___cycFILE*_tmp610=Cyc_stderr;struct _fat_ptr _tmp60F=({const char*_tmp39C="Error: could not create cycstubs.c in directory %s\n";_tag_fat(_tmp39C,sizeof(char),52U);});Cyc_fprintf(_tmp610,_tmp60F,_tag_fat(_tmp39B,sizeof(void*),1U));});});
# 2071
return 1;}
# 2073
({void*_tmp39E=0U;({struct Cyc___cycFILE*_tmp612=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _fat_ptr _tmp611=({const char*_tmp39F="#include <core.h>\nusing Core;\n\n";_tag_fat(_tmp39F,sizeof(char),32U);});Cyc_fprintf(_tmp612,_tmp611,_tag_fat(_tmp39E,sizeof(void*),0U));});});}}{
# 2080
const char*outdir=(const char*)_check_null(_untag_fat_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp && Cyc_process_setjmp(outdir))
return 1;else{
# 2086
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp3A0=0U;({struct Cyc___cycFILE*_tmp614=Cyc_stderr;struct _fat_ptr _tmp613=({const char*_tmp3A1="FATAL ERROR -- QUIT!\n";_tag_fat(_tmp3A1,sizeof(char),22U);});Cyc_fprintf(_tmp614,_tmp613,_tag_fat(_tmp3A0,sizeof(void*),0U));});});
exit(1);}}}
# 2094
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2097
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2105
return 0;}}}}}
