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

/* Pools */
struct _pool; // defined in runtime_memory.c
struct _PoolHandle {
  struct _RuntimeStack s;
  struct _pool *p;
};
struct _PoolHandle _new_pool(void);
void _free_pool(struct _PoolHandle *h);
void _push_pool(struct _PoolHandle * r);
void _pop_pool(void);

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
# 289 "core.h"
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Pool_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1091 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1146
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _fat_ptr*name);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1152
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1160
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
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 29 "binding.h"
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
# 77 "buildlib.cyl"
extern void Cyc_Lex_lex_init(int use_cyclone_keywords);static char _tmp0[4U]="gcc";
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
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp432=Cyc_stderr;struct _fat_ptr _tmp431=({const char*_tmp3="Internal error: log file is NULL\n";_tag_fat(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp432,_tmp431,_tag_fat(_tmp2,sizeof(void*),0U));});});
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
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp433=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);*_tmp5=_tmp433;});_tmp5;});}struct _tuple13{struct _fat_ptr*f1;struct Cyc_Set_Set*f2;};
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
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=0U,_tmp3D4.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp3D4;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp435=Cyc_stderr;struct _fat_ptr _tmp434=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_fat(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp435,_tmp434,_tag_fat(_tmp8,sizeof(void*),1U));});});
return symbol;}
# 183
return(struct _fat_ptr)({struct _fat_ptr _tmp438=(struct _fat_ptr)symbol;int _tmp437=(int)prefix_len;Cyc_substring(_tmp438,_tmp437,({unsigned long _tmp436=Cyc_strlen((struct _fat_ptr)symbol);_tmp436 - prefix_len;}));});}
# 186
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;struct Cyc_Absyn_Typedefdecl*_tmpD;struct Cyc_Absyn_Enumdecl*_tmpE;struct Cyc_Absyn_Aggrdecl*_tmpF;switch(*((int*)_tmpC)){case 6U: _LL1: _tmpF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2: {struct Cyc_Absyn_Aggrdecl*ad=_tmpF;
# 189
({struct _fat_ptr*_tmp43A=({struct _fat_ptr*_tmp10=_cycalloc(sizeof(*_tmp10));({struct _fat_ptr _tmp439=Cyc_remove_user_prefix(*(*ad->name).f2);*_tmp10=_tmp439;});_tmp10;});(*ad->name).f2=_tmp43A;});goto _LL0;}case 8U: _LL3: _tmpE=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4: {struct Cyc_Absyn_Enumdecl*ed=_tmpE;
# 191
({struct _fat_ptr*_tmp43C=({struct _fat_ptr*_tmp11=_cycalloc(sizeof(*_tmp11));({struct _fat_ptr _tmp43B=Cyc_remove_user_prefix(*(*ed->name).f2);*_tmp11=_tmp43B;});_tmp11;});(*ed->name).f2=_tmp43C;});goto _LL0;}case 9U: _LL5: _tmpD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6: {struct Cyc_Absyn_Typedefdecl*td=_tmpD;
# 193
({struct _fat_ptr*_tmp43E=({struct _fat_ptr*_tmp12=_cycalloc(sizeof(*_tmp12));({struct _fat_ptr _tmp43D=Cyc_remove_user_prefix(*(*td->name).f2);*_tmp12=_tmp43D;});_tmp12;});(*td->name).f2=_tmp43E;});goto _LL0;}case 1U: _LL7: _LL8:
# 195
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 4U: _LLB: _LLC:
 goto _LLE;case 5U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 2U: _LL11: _LL12:
 goto _LL14;case 3U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;case 16U: _LL21: _LL22:
 goto _LL24;default: _LL23: _LL24:
# 210
({void*_tmp13=0U;({struct Cyc___cycFILE*_tmp440=Cyc_stderr;struct _fat_ptr _tmp43F=({const char*_tmp14="Error in .cys file: bad user-defined type definition\n";_tag_fat(_tmp14,sizeof(char),54U);});Cyc_fprintf(_tmp440,_tmp43F,_tag_fat(_tmp13,sizeof(void*),0U));});});
exit(1);}_LL0:;}
# 217
const int Cyc_lex_base[431U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431U]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 224
int state;int base;int backtrk;
int c;
state=start_state;
# 228
if(state >= 0){
({int _tmp441=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp441;});
lbuf->lex_last_action=- 1;}else{
# 232
state=(- state)- 1;}
# 234
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,431U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 242
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 246
c=256;}}else{
# 248
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 251
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,3846U,sizeof(int),base + c))== state)
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,3846U,sizeof(int),base + c));else{
# 254
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp442=({const char*_tmp15="empty token";_tag_fat(_tmp15,sizeof(char),12U);});_tmp16->f1=_tmp442;});_tmp16;}));else{
# 260
return lbuf->lex_last_action;}}else{
# 263
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 267
struct _tuple13*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp17=lexstate;switch(_tmp17){case 0U: _LL1: _LL2:
# 226 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp18=_cycalloc(sizeof(*_tmp18));({struct Cyc_Set_Set*_tmp444=({struct Cyc_Set_Set*_tmp443=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_delete)(_tmp443,(struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});*_tmp18=_tmp444;});_tmp18;});}
# 231
return({struct _tuple13*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->f1=(struct _fat_ptr*)_check_null(Cyc_current_source),_tmp19->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp19;});case 1U: _LL3: _LL4:
# 234 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2U: _LL5: _LL6:
# 236
 return 0;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 240
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp445=({const char*_tmp1A="some action didn't return!";_tag_fat(_tmp1A,sizeof(char),27U);});_tmp1B->f1=_tmp445;});_tmp1B;}));}
# 242
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp1C=lexstate;switch(_tmp1C){case 0U: _LL1: _LL2:
# 240 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct _fat_ptr _tmp448=(struct _fat_ptr)({struct _fat_ptr _tmp447=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp447,0,(unsigned long)(({
int _tmp446=Cyc_Lexing_lexeme_end(lexbuf);_tmp446 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 240
*_tmp1D=_tmp448;});_tmp1D;});
# 242
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct Cyc_Set_Set*_tmp449=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp1E=_tmp449;});_tmp1E;});
Cyc_token(lexbuf);
return 0;case 1U: _LL3: _LL4:
# 248
 Cyc_current_source=({struct _fat_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));({struct _fat_ptr _tmp44C=(struct _fat_ptr)({struct _fat_ptr _tmp44B=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp44B,0,(unsigned long)(({
int _tmp44A=Cyc_Lexing_lexeme_end(lexbuf);_tmp44A - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 248
*_tmp1F=_tmp44C;});_tmp1F;});
# 250
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp20=_cycalloc(sizeof(*_tmp20));({struct Cyc_Set_Set*_tmp44D=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp20=_tmp44D;});_tmp20;});
Cyc_args(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 256
 Cyc_current_source=({struct _fat_ptr*_tmp21=_cycalloc(sizeof(*_tmp21));({struct _fat_ptr _tmp44E=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp21=_tmp44E;});_tmp21;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp22=_cycalloc(sizeof(*_tmp22));({struct Cyc_Set_Set*_tmp44F=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp22=_tmp44F;});_tmp22;});
Cyc_token(lexbuf);
return 0;default: _LL7: _LL8:
# 262
((lexbuf->refill_buff))(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 265
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp450=({const char*_tmp23="some action didn't return!";_tag_fat(_tmp23,sizeof(char),27U);});_tmp24->f1=_tmp450;});_tmp24;}));}
# 267
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp25=lexstate;switch(_tmp25){case 0U: _LL1: _LL2: {
# 265 "buildlib.cyl"
struct _fat_ptr*_tmp26=({struct _fat_ptr*_tmp28=_cycalloc(sizeof(*_tmp28));({struct _fat_ptr _tmp453=(struct _fat_ptr)({struct _fat_ptr _tmp452=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp452,0,(unsigned long)(({
int _tmp451=Cyc_Lexing_lexeme_end(lexbuf);_tmp451 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 265
*_tmp28=_tmp453;});_tmp28;});struct _fat_ptr*a=_tmp26;
# 267
Cyc_current_args=({struct Cyc_List_List*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=a,_tmp27->tl=Cyc_current_args;_tmp27;});
return Cyc_args(lexbuf);}case 1U: _LL3: _LL4: {
# 271
struct _fat_ptr*_tmp29=({struct _fat_ptr*_tmp2B=_cycalloc(sizeof(*_tmp2B));({struct _fat_ptr _tmp456=(struct _fat_ptr)({struct _fat_ptr _tmp455=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp455,0,(unsigned long)(({
int _tmp454=Cyc_Lexing_lexeme_end(lexbuf);_tmp454 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 271
*_tmp2B=_tmp456;});_tmp2B;});struct _fat_ptr*a=_tmp29;
# 273
Cyc_current_args=({struct Cyc_List_List*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=a,_tmp2A->tl=Cyc_current_args;_tmp2A;});
return Cyc_args(lexbuf);}case 2U: _LL5: _LL6: {
# 277
struct _fat_ptr*_tmp2C=({struct _fat_ptr*_tmp2E=_cycalloc(sizeof(*_tmp2E));({struct _fat_ptr _tmp459=(struct _fat_ptr)({struct _fat_ptr _tmp458=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp458,0,(unsigned long)(({
int _tmp457=Cyc_Lexing_lexeme_end(lexbuf);_tmp457 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 277
*_tmp2E=_tmp459;});_tmp2E;});struct _fat_ptr*a=_tmp2C;
# 279
Cyc_current_args=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=a,_tmp2D->tl=Cyc_current_args;_tmp2D;});
return Cyc_token(lexbuf);}default: _LL7: _LL8:
# 282
((lexbuf->refill_buff))(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL0:;}
# 285
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp45A=({const char*_tmp2F="some action didn't return!";_tag_fat(_tmp2F,sizeof(char),27U);});_tmp30->f1=_tmp45A;});_tmp30;}));}
# 287
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp31=lexstate;switch(_tmp31){case 0U: _LL1: _LL2:
# 286 "buildlib.cyl"
 Cyc_add_target(({struct _fat_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));({struct _fat_ptr _tmp45B=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp32=_tmp45B;});_tmp32;}));return Cyc_token(lexbuf);case 1U: _LL3: _LL4:
# 289 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 292 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3U: _LL7: _LL8:
# 295 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4U: _LL9: _LLA:
# 298 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5U: _LLB: _LLC:
# 300
 return Cyc_token(lexbuf);case 6U: _LLD: _LLE:
# 302
 return Cyc_token(lexbuf);case 7U: _LLF: _LL10:
# 304
 return Cyc_token(lexbuf);case 8U: _LL11: _LL12:
# 307 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9U: _LL13: _LL14:
# 310 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10U: _LL15: _LL16:
# 313 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11U: _LL17: _LL18:
# 315
 return Cyc_token(lexbuf);case 12U: _LL19: _LL1A:
# 317
 return Cyc_token(lexbuf);case 13U: _LL1B: _LL1C:
# 319
 return Cyc_token(lexbuf);case 14U: _LL1D: _LL1E:
# 321
 return Cyc_token(lexbuf);case 15U: _LL1F: _LL20:
# 323
 return Cyc_token(lexbuf);case 16U: _LL21: _LL22:
# 325
 return Cyc_token(lexbuf);case 17U: _LL23: _LL24:
# 327
 return Cyc_token(lexbuf);case 18U: _LL25: _LL26:
# 329
 return Cyc_token(lexbuf);case 19U: _LL27: _LL28:
# 331
 return Cyc_token(lexbuf);case 20U: _LL29: _LL2A:
# 333
 return Cyc_token(lexbuf);case 21U: _LL2B: _LL2C:
# 335
 return Cyc_token(lexbuf);case 22U: _LL2D: _LL2E:
# 337
 return Cyc_token(lexbuf);case 23U: _LL2F: _LL30:
# 339
 return Cyc_token(lexbuf);case 24U: _LL31: _LL32:
# 342 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25U: _LL33: _LL34:
# 344
 return Cyc_token(lexbuf);case 26U: _LL35: _LL36:
# 346
 return Cyc_token(lexbuf);case 27U: _LL37: _LL38:
# 348
 return Cyc_token(lexbuf);case 28U: _LL39: _LL3A:
# 350
 return Cyc_token(lexbuf);case 29U: _LL3B: _LL3C:
# 352
 return Cyc_token(lexbuf);case 30U: _LL3D: _LL3E:
# 354
 return Cyc_token(lexbuf);case 31U: _LL3F: _LL40:
# 356
 return Cyc_token(lexbuf);case 32U: _LL41: _LL42:
# 358
 return Cyc_token(lexbuf);case 33U: _LL43: _LL44:
# 360
 return Cyc_token(lexbuf);case 34U: _LL45: _LL46:
# 362
 return Cyc_token(lexbuf);case 35U: _LL47: _LL48:
# 364
 return Cyc_token(lexbuf);case 36U: _LL49: _LL4A:
# 366
 return Cyc_token(lexbuf);case 37U: _LL4B: _LL4C:
# 368
 return Cyc_token(lexbuf);case 38U: _LL4D: _LL4E:
# 370
 return Cyc_token(lexbuf);case 39U: _LL4F: _LL50:
# 372
 return Cyc_token(lexbuf);case 40U: _LL51: _LL52:
# 374
 return Cyc_token(lexbuf);case 41U: _LL53: _LL54:
# 376
 return Cyc_token(lexbuf);case 42U: _LL55: _LL56:
# 378
 return Cyc_token(lexbuf);case 43U: _LL57: _LL58:
# 380
 return Cyc_token(lexbuf);case 44U: _LL59: _LL5A:
# 382
 return Cyc_token(lexbuf);case 45U: _LL5B: _LL5C:
# 384
 return Cyc_token(lexbuf);case 46U: _LL5D: _LL5E:
# 386
 return Cyc_token(lexbuf);case 47U: _LL5F: _LL60:
# 388
 return Cyc_token(lexbuf);case 48U: _LL61: _LL62:
# 391 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL63: _LL64:
((lexbuf->refill_buff))(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL0:;}
# 395
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp45C=({const char*_tmp33="some action didn't return!";_tag_fat(_tmp33,sizeof(char),27U);});_tmp34->f1=_tmp45C;});_tmp34;}));}
# 397
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp35=lexstate;switch(_tmp35){case 0U: _LL1: _LL2:
# 396 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1U: _LL3: _LL4:
# 397 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 398 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3U: _LL7: _LL8:
# 399 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4U: _LL9: _LLA:
# 402 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5U: _LLB: _LLC:
# 405 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6U: _LLD: _LLE:
# 407
 return Cyc_string(lexbuf);case 7U: _LLF: _LL10:
# 408 "buildlib.cyl"
 return 0;case 8U: _LL11: _LL12:
# 409 "buildlib.cyl"
 return 0;case 9U: _LL13: _LL14:
# 410 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL15: _LL16:
((lexbuf->refill_buff))(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL0:;}
# 414
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp45D=({const char*_tmp36="some action didn't return!";_tag_fat(_tmp36,sizeof(char),27U);});_tmp37->f1=_tmp45D;});_tmp37;}));}
# 416
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp38=lexstate;switch(_tmp38){case 0U: _LL1: _LL2:
# 419 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 421
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2U: _LL5: _LL6:
# 428 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp39=0U;({struct _fat_ptr _tmp45E=({const char*_tmp3A="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp3A,sizeof(char),44U);});Cyc_log(_tmp45E,_tag_fat(_tmp39,sizeof(void*),0U));});});
return 1;case 3U: _LL7: _LL8:
# 434 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3B=0U;({struct _fat_ptr _tmp45F=({const char*_tmp3C="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp3C,sizeof(char),44U);});Cyc_log(_tmp45F,_tag_fat(_tmp3B,sizeof(void*),0U));});});
return 1;case 4U: _LL9: _LLA:
# 440 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3D=0U;({struct _fat_ptr _tmp460=({const char*_tmp3E="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp3E,sizeof(char),44U);});Cyc_log(_tmp460,_tag_fat(_tmp3D,sizeof(void*),0U));});});
return 1;case 5U: _LLB: _LLC:
# 446 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3F=0U;({struct _fat_ptr _tmp461=({const char*_tmp40="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp40,sizeof(char),44U);});Cyc_log(_tmp461,_tag_fat(_tmp3F,sizeof(void*),0U));});});
return 1;case 6U: _LLD: _LLE:
# 450
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp41=0U;({struct _fat_ptr _tmp462=({const char*_tmp42="Warning: use of region sidestepped\n";_tag_fat(_tmp42,sizeof(char),36U);});Cyc_log(_tmp462,_tag_fat(_tmp41,sizeof(void*),0U));});});
return 1;case 7U: _LLF: _LL10:
# 454
({void*_tmp43=0U;({struct _fat_ptr _tmp463=({const char*_tmp44="Warning: use of __extension__ deleted\n";_tag_fat(_tmp44,sizeof(char),39U);});Cyc_log(_tmp463,_tag_fat(_tmp43,sizeof(void*),0U));});});
return 1;case 8U: _LL11: _LL12:
# 459 "buildlib.cyl"
({void*_tmp45=0U;({struct _fat_ptr _tmp464=({const char*_tmp46="Warning: use of mode HI deleted\n";_tag_fat(_tmp46,sizeof(char),33U);});Cyc_log(_tmp464,_tag_fat(_tmp45,sizeof(void*),0U));});});
return 1;case 9U: _LL13: _LL14:
# 462
({void*_tmp47=0U;({struct _fat_ptr _tmp465=({const char*_tmp48="Warning: use of mode SI deleted\n";_tag_fat(_tmp48,sizeof(char),33U);});Cyc_log(_tmp465,_tag_fat(_tmp47,sizeof(void*),0U));});});
return 1;case 10U: _LL15: _LL16:
# 465
({void*_tmp49=0U;({struct _fat_ptr _tmp466=({const char*_tmp4A="Warning: use of mode QI deleted\n";_tag_fat(_tmp4A,sizeof(char),33U);});Cyc_log(_tmp466,_tag_fat(_tmp49,sizeof(void*),0U));});});
return 1;case 11U: _LL17: _LL18:
# 468
({void*_tmp4B=0U;({struct _fat_ptr _tmp467=({const char*_tmp4C="Warning: use of mode DI deleted\n";_tag_fat(_tmp4C,sizeof(char),33U);});Cyc_log(_tmp467,_tag_fat(_tmp4B,sizeof(void*),0U));});});
return 1;case 12U: _LL19: _LL1A:
# 471
({void*_tmp4D=0U;({struct _fat_ptr _tmp468=({const char*_tmp4E="Warning: use of mode DI deleted\n";_tag_fat(_tmp4E,sizeof(char),33U);});Cyc_log(_tmp468,_tag_fat(_tmp4D,sizeof(void*),0U));});});
return 1;case 13U: _LL1B: _LL1C:
# 474
({void*_tmp4F=0U;({struct _fat_ptr _tmp469=({const char*_tmp50="Warning: use of mode word deleted\n";_tag_fat(_tmp50,sizeof(char),35U);});Cyc_log(_tmp469,_tag_fat(_tmp4F,sizeof(void*),0U));});});
return 1;case 14U: _LL1D: _LL1E:
# 477
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15U: _LL1F: _LL20:
# 479
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16U: _LL21: _LL22:
# 481
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17U: _LL23: _LL24:
# 483
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18U: _LL25: _LL26:
# 488 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19U: _LL27: _LL28:
# 490
 return 1;case 20U: _LL29: _LL2A:
# 492
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp51=0U;({struct _fat_ptr _tmp46A=({const char*_tmp52="Warning: replacing use of __asm__ with 0\n";_tag_fat(_tmp52,sizeof(char),42U);});Cyc_log(_tmp46A,_tag_fat(_tmp51,sizeof(void*),0U));});});
return 1;case 21U: _LL2B: _LL2C:
# 498
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp53=0U;({struct _fat_ptr _tmp46B=({const char*_tmp54="Warning: replacing use of __asm with 0\n";_tag_fat(_tmp54,sizeof(char),40U);});Cyc_log(_tmp46B,_tag_fat(_tmp53,sizeof(void*),0U));});});
return 1;case 22U: _LL2D: _LL2E:
# 504
({int _tmp46C=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp46C,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default: _LL2F: _LL30:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL0:;}
# 508
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp46D=({const char*_tmp55="some action didn't return!";_tag_fat(_tmp55,sizeof(char),27U);});_tmp56->f1=_tmp46D;});_tmp56;}));}
# 510
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp57=lexstate;switch(_tmp57){case 0U: _LL1: _LL2:
# 508 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 510
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2U: _LL5: _LL6:
# 512
({void*_tmp58=0U;({struct _fat_ptr _tmp46E=({const char*_tmp59="Warning: unclosed string\n";_tag_fat(_tmp59,sizeof(char),26U);});Cyc_log(_tmp46E,_tag_fat(_tmp58,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0U,({struct _fat_ptr _tmp46F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D5.f1=_tmp46F;});_tmp3D5;});void*_tmp5A[1U];_tmp5A[0]=& _tmp5C;({struct Cyc___cycFILE*_tmp471=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp470=({const char*_tmp5B="%s";_tag_fat(_tmp5B,sizeof(char),3U);});Cyc_fprintf(_tmp471,_tmp470,_tag_fat(_tmp5A,sizeof(void*),1U));});});return 1;case 3U: _LL7: _LL8:
# 515
({struct Cyc_String_pa_PrintArg_struct _tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0U,({struct _fat_ptr _tmp472=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D6.f1=_tmp472;});_tmp3D6;});void*_tmp5D[1U];_tmp5D[0]=& _tmp5F;({struct Cyc___cycFILE*_tmp474=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp473=({const char*_tmp5E="%s";_tag_fat(_tmp5E,sizeof(char),3U);});Cyc_fprintf(_tmp474,_tmp473,_tag_fat(_tmp5D,sizeof(void*),1U));});});return 1;case 4U: _LL9: _LLA:
# 517
({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=0U,({struct _fat_ptr _tmp475=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D7.f1=_tmp475;});_tmp3D7;});void*_tmp60[1U];_tmp60[0]=& _tmp62;({struct Cyc___cycFILE*_tmp477=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp476=({const char*_tmp61="%s";_tag_fat(_tmp61,sizeof(char),3U);});Cyc_fprintf(_tmp477,_tmp476,_tag_fat(_tmp60,sizeof(void*),1U));});});return 1;case 5U: _LLB: _LLC:
# 519
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp3D8;_tmp3D8.tag=0U,({struct _fat_ptr _tmp478=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D8.f1=_tmp478;});_tmp3D8;});void*_tmp63[1U];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmp47A=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp479=({const char*_tmp64="%s";_tag_fat(_tmp64,sizeof(char),3U);});Cyc_fprintf(_tmp47A,_tmp479,_tag_fat(_tmp63,sizeof(void*),1U));});});return 1;case 6U: _LLD: _LLE:
# 521
({struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0U,({struct _fat_ptr _tmp47B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D9.f1=_tmp47B;});_tmp3D9;});void*_tmp66[1U];_tmp66[0]=& _tmp68;({struct Cyc___cycFILE*_tmp47D=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp47C=({const char*_tmp67="%s";_tag_fat(_tmp67,sizeof(char),3U);});Cyc_fprintf(_tmp47D,_tmp47C,_tag_fat(_tmp66,sizeof(void*),1U));});});return 1;case 7U: _LLF: _LL10:
# 523
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=0U,({struct _fat_ptr _tmp47E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3DA.f1=_tmp47E;});_tmp3DA;});void*_tmp69[1U];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp480=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp47F=({const char*_tmp6A="%s";_tag_fat(_tmp6A,sizeof(char),3U);});Cyc_fprintf(_tmp480,_tmp47F,_tag_fat(_tmp69,sizeof(void*),1U));});});return 1;case 8U: _LL11: _LL12:
# 525
({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0U,({struct _fat_ptr _tmp481=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3DB.f1=_tmp481;});_tmp3DB;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6E;({struct Cyc___cycFILE*_tmp483=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp482=({const char*_tmp6D="%s";_tag_fat(_tmp6D,sizeof(char),3U);});Cyc_fprintf(_tmp483,_tmp482,_tag_fat(_tmp6C,sizeof(void*),1U));});});return 1;default: _LL13: _LL14:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL0:;}
# 529
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp484=({const char*_tmp6F="some action didn't return!";_tag_fat(_tmp6F,sizeof(char),27U);});_tmp70->f1=_tmp484;});_tmp70;}));}
# 531
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp71=lexstate;switch(_tmp71){case 0U: _LL1: _LL2:
# 535 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 537
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2U: _LL5: _LL6:
# 541
 ++ Cyc_parens_to_match;
return 1;case 3U: _LL7: _LL8:
# 544
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 547
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 550
 return 1;case 6U: _LLD: _LLE:
# 552
 return 1;default: _LLF: _LL10:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL0:;}
# 556
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp485=({const char*_tmp72="some action didn't return!";_tag_fat(_tmp72,sizeof(char),27U);});_tmp73->f1=_tmp485;});_tmp73;}));}
# 558
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp74=lexstate;switch(_tmp74){case 0U: _LL1: _LL2:
# 556 "buildlib.cyl"
({void*_tmp75=0U;({struct _fat_ptr _tmp486=({const char*_tmp76="Warning: unclosed string\n";_tag_fat(_tmp76,sizeof(char),26U);});Cyc_log(_tmp486,_tag_fat(_tmp75,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 558
 return 0;case 2U: _LL5: _LL6:
# 560
({void*_tmp77=0U;({struct _fat_ptr _tmp487=({const char*_tmp78="Warning: unclosed string\n";_tag_fat(_tmp78,sizeof(char),26U);});Cyc_log(_tmp487,_tag_fat(_tmp77,sizeof(void*),0U));});});return 1;case 3U: _LL7: _LL8:
# 562
 return 1;case 4U: _LL9: _LLA:
# 564
 return 1;case 5U: _LLB: _LLC:
# 566
 return 1;case 6U: _LLD: _LLE:
# 568
 return 1;case 7U: _LLF: _LL10:
# 570
 return 1;case 8U: _LL11: _LL12:
# 572
 return 1;default: _LL13: _LL14:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL0:;}
# 576
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp488=({const char*_tmp79="some action didn't return!";_tag_fat(_tmp79,sizeof(char),27U);});_tmp7A->f1=_tmp488;});_tmp7A;}));}
# 578
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7B=lexstate;switch(_tmp7B){case 0U: _LL1: _LL2:
# 576 "buildlib.cyl"
({void*_tmp7C=0U;({struct _fat_ptr _tmp489=({const char*_tmp7D="Warning: unclosed comment\n";_tag_fat(_tmp7D,sizeof(char),27U);});Cyc_log(_tmp489,_tag_fat(_tmp7C,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 578
 return 0;case 2U: _LL5: _LL6:
# 580
 return 1;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL0:;}
# 584
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp48A=({const char*_tmp7E="some action didn't return!";_tag_fat(_tmp7E,sizeof(char),27U);});_tmp7F->f1=_tmp48A;});_tmp7F;}));}
# 586
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple14*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp80=lexstate;switch(_tmp80){case 0U: _LL1: _LL2:
# 588 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp81="#define ";_tag_fat(_tmp81,sizeof(char),9U);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple14*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=Cyc_current_line,_tmp82->f2=(struct _fat_ptr*)_check_null(Cyc_current_source);_tmp82;});case 1U: _LL3: _LL4:
# 592
 return Cyc_suck_line(lexbuf);case 2U: _LL5: _LL6:
# 594
 return 0;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL0:;}
# 598
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp48B=({const char*_tmp83="some action didn't return!";_tag_fat(_tmp83,sizeof(char),27U);});_tmp84->f1=_tmp48B;});_tmp84;}));}
# 600
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp85=lexstate;if(_tmp85 == 0){_LL1: _LL2:
# 598 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_tmp86=_cycalloc(sizeof(*_tmp86));({struct _fat_ptr _tmp48C=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp86=_tmp48C;});_tmp86;});
Cyc_current_line=(struct _fat_ptr)Cyc_strconcat((struct _fat_ptr)Cyc_current_line,(struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL3: _LL4:
# 602
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 605
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp48D=({const char*_tmp87="some action didn't return!";_tag_fat(_tmp87,sizeof(char),27U);});_tmp88->f1=_tmp48D;});_tmp88;}));}
# 607
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp89=lexstate;if(_tmp89 == 0){_LL1: _LL2:
# 605 "buildlib.cyl"
 Cyc_current_line=(struct _fat_ptr)({struct _fat_ptr _tmp48E=(struct _fat_ptr)Cyc_current_line;Cyc_strconcat(_tmp48E,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});return 0;}else{_LL3: _LL4:
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL0:;}
# 609
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp48F=({const char*_tmp8A="some action didn't return!";_tag_fat(_tmp8A,sizeof(char),27U);});_tmp8B->f1=_tmp48F;});_tmp8B;}));}
# 611
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple17*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp8C=lexstate;switch(_tmp8C){case 0U: _LL1: _LL2:
# 612 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1U: _LL3: _LL4:
# 614
 Cyc_current_headerfile=(struct _fat_ptr)({
struct _fat_ptr _tmp491=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp491,0,(unsigned long)(({
int _tmp490=Cyc_Lexing_lexeme_end(lexbuf);_tmp490 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
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
return({struct _tuple17*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->f1=Cyc_current_headerfile,_tmp8D->f2=Cyc_current_symbols,_tmp8D->f3=Cyc_current_user_defs,_tmp8D->f4=Cyc_current_omit_symbols,_tmp8D->f5=Cyc_current_hstubs,_tmp8D->f6=Cyc_current_cstubs,_tmp8D->f7=Cyc_current_cycstubs,_tmp8D->f8=Cyc_current_cpp;_tmp8D;});case 2U: _LL5: _LL6:
# 639
 return Cyc_spec(lexbuf);case 3U: _LL7: _LL8:
# 641
 return 0;case 4U: _LL9: _LLA:
# 643
({struct Cyc_Int_pa_PrintArg_struct _tmp90=({struct Cyc_Int_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=1U,({
# 645
unsigned long _tmp492=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3DC.f1=_tmp492;});_tmp3DC;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({struct Cyc___cycFILE*_tmp494=Cyc_stderr;struct _fat_ptr _tmp493=({const char*_tmp8F="Error in .cys file: expected header file name, found '%c' instead\n";_tag_fat(_tmp8F,sizeof(char),67U);});Cyc_fprintf(_tmp494,_tmp493,_tag_fat(_tmp8E,sizeof(void*),1U));});});
return 0;default: _LLB: _LLC:
((lexbuf->refill_buff))(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL0:;}
# 650
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp495=({const char*_tmp91="some action didn't return!";_tag_fat(_tmp91,sizeof(char),27U);});_tmp92->f1=_tmp495;});_tmp92;}));}
# 652
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp93=lexstate;switch(_tmp93){case 0U: _LL1: _LL2:
# 650 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 652
 return 0;case 2U: _LL5: _LL6:
# 654
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3U: _LL7: _LL8:
# 659
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp94=Cyc_current_user_defs;struct Cyc_List_List*tmp_user_defs=_tmp94;
while(Cyc_snarfsymbols(lexbuf)){;}
if(tmp_user_defs != Cyc_current_user_defs){
({void*_tmp95=0U;({struct Cyc___cycFILE*_tmp497=Cyc_stderr;struct _fat_ptr _tmp496=({const char*_tmp96="Error in .cys file: got optional definition in omitsymbols\n";_tag_fat(_tmp96,sizeof(char),60U);});Cyc_fprintf(_tmp497,_tmp496,_tag_fat(_tmp95,sizeof(void*),0U));});});
# 665
return 0;}
# 667
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;}case 4U: _LL9: _LLA:
# 670
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp98=_cycalloc(sizeof(*_tmp98));({struct _fat_ptr _tmp499=(struct _fat_ptr)_tag_fat(0,0,0);_tmp98->f1=_tmp499;}),({
struct _fat_ptr _tmp498=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp98->f2=_tmp498;});_tmp98;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=x,_tmp97->tl=Cyc_current_hstubs;_tmp97;});
return 1;}case 5U: _LLB: _LLC: {
# 678
struct _fat_ptr _tmp99=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmp99;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmp9A="hstub";_tag_fat(_tmp9A,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmp9B=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmp9B;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp49A=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9D->f2=_tmp49A;});_tmp9D;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=x,_tmp9C->tl=Cyc_current_hstubs;_tmp9C;});
return 1;}}}}case 6U: _LLD: _LLE:
# 692
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp9F=_cycalloc(sizeof(*_tmp9F));({struct _fat_ptr _tmp49C=(struct _fat_ptr)_tag_fat(0,0,0);_tmp9F->f1=_tmp49C;}),({
struct _fat_ptr _tmp49B=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9F->f2=_tmp49B;});_tmp9F;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=x,_tmp9E->tl=Cyc_current_cstubs;_tmp9E;});
return 1;}case 7U: _LLF: _LL10: {
# 700
struct _fat_ptr _tmpA0=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpA0;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmpA1="cstub";_tag_fat(_tmpA1,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmpA2=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmpA2;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp49D=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA4->f2=_tmp49D;});_tmpA4;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=x,_tmpA3->tl=Cyc_current_cstubs;_tmpA3;});
return 1;}}}}case 8U: _LL11: _LL12:
# 714
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _fat_ptr _tmp49F=(struct _fat_ptr)_tag_fat(0,0,0);_tmpA6->f1=_tmp49F;}),({
struct _fat_ptr _tmp49E=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA6->f2=_tmp49E;});_tmpA6;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=x,_tmpA5->tl=Cyc_current_cycstubs;_tmpA5;});
return 1;}case 9U: _LL13: _LL14: {
# 722
struct _fat_ptr _tmpA7=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpA7;
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_tmpA8="cycstub";_tag_fat(_tmpA8,sizeof(char),8U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr _tmpA9=Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));struct _fat_ptr symbol=_tmpA9;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=(struct _fat_ptr)symbol,({
struct _fat_ptr _tmp4A0=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpAB->f2=_tmp4A0;});_tmpAB;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->hd=x,_tmpAA->tl=Cyc_current_cycstubs;_tmpAA;});
return 1;}}}}case 10U: _LL15: _LL16:
# 736
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _fat_ptr*x=({struct _fat_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _fat_ptr _tmp4A1=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpAD=_tmp4A1;});_tmpAD;});
Cyc_current_cpp=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=x,_tmpAC->tl=Cyc_current_cpp;_tmpAC;});
return 1;}case 11U: _LL17: _LL18:
# 743
 return 1;case 12U: _LL19: _LL1A:
# 745
 return 1;case 13U: _LL1B: _LL1C:
# 747
({struct Cyc_Int_pa_PrintArg_struct _tmpB0=({struct Cyc_Int_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=1U,({
# 749
unsigned long _tmp4A2=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3DD.f1=_tmp4A2;});_tmp3DD;});void*_tmpAE[1U];_tmpAE[0]=& _tmpB0;({struct Cyc___cycFILE*_tmp4A4=Cyc_stderr;struct _fat_ptr _tmp4A3=({const char*_tmpAF="Error in .cys file: expected command, found '%c' instead\n";_tag_fat(_tmpAF,sizeof(char),58U);});Cyc_fprintf(_tmp4A4,_tmp4A3,_tag_fat(_tmpAE,sizeof(void*),1U));});});
return 0;default: _LL1D: _LL1E:
((lexbuf->refill_buff))(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL0:;}
# 754
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4A5=({const char*_tmpB1="some action didn't return!";_tag_fat(_tmpB1,sizeof(char),27U);});_tmpB2->f1=_tmp4A5;});_tmpB2;}));}
# 756
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB3=lexstate;switch(_tmpB3){case 0U: _LL1: _LL2:
# 760 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _fat_ptr*_tmp4A7=({struct _fat_ptr*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct _fat_ptr _tmp4A6=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpB4=_tmp4A6;});_tmpB4;});_tmpB5->hd=_tmp4A7;}),_tmpB5->tl=Cyc_snarfed_symbols;_tmpB5;});
return 1;case 1U: _LL3: _LL4: {
# 763
struct _fat_ptr _tmpB6=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpB6;
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}
# 772
Cyc_rename_current_symbol=0;{
struct _tuple16*user_def=({struct _tuple16*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _fat_ptr*_tmp4AA=({struct _fat_ptr*_tmpBA=_cycalloc(sizeof(*_tmpBA));*_tmpBA=Cyc_current_symbol;_tmpBA;});_tmpBC->f1=_tmp4AA;}),({
struct _fat_ptr*_tmp4A9=({struct _fat_ptr*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _fat_ptr _tmp4A8=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpBB=_tmp4A8;});_tmpBB;});_tmpBC->f2=_tmp4A9;});_tmpBC;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct _fat_ptr*_tmp4AB=({struct _fat_ptr*_tmpB7=_cycalloc(sizeof(*_tmpB7));*_tmpB7=(struct _fat_ptr)Cyc_current_symbol;_tmpB7;});_tmpB8->hd=_tmp4AB;}),_tmpB8->tl=Cyc_snarfed_symbols;_tmpB8;});
Cyc_current_user_defs=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=user_def,_tmpB9->tl=Cyc_current_user_defs;_tmpB9;});
return 1;}}case 2U: _LL5: _LL6:
# 779
 return 1;case 3U: _LL7: _LL8:
# 781
 return 0;case 4U: _LL9: _LLA:
# 783
({void*_tmpBD=0U;({struct Cyc___cycFILE*_tmp4AD=Cyc_stderr;struct _fat_ptr _tmp4AC=({const char*_tmpBE="Error in .cys file: unexpected end-of-file\n";_tag_fat(_tmpBE,sizeof(char),44U);});Cyc_fprintf(_tmp4AD,_tmp4AC,_tag_fat(_tmpBD,sizeof(void*),0U));});});
# 785
return 0;case 5U: _LLB: _LLC:
# 787
({struct Cyc_Int_pa_PrintArg_struct _tmpC1=({struct Cyc_Int_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=1U,({
# 789
unsigned long _tmp4AE=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3DE.f1=_tmp4AE;});_tmp3DE;});void*_tmpBF[1U];_tmpBF[0]=& _tmpC1;({struct Cyc___cycFILE*_tmp4B0=Cyc_stderr;struct _fat_ptr _tmp4AF=({const char*_tmpC0="Error in .cys file: expected symbol, found '%c' instead\n";_tag_fat(_tmpC0,sizeof(char),57U);});Cyc_fprintf(_tmp4B0,_tmp4AF,_tag_fat(_tmpBF,sizeof(void*),1U));});});
return 0;default: _LLD: _LLE:
((lexbuf->refill_buff))(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL0:;}
# 794
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4B1=({const char*_tmpC2="some action didn't return!";_tag_fat(_tmpC2,sizeof(char),27U);});_tmpC3->f1=_tmp4B1;});_tmpC3;}));}
# 796
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC4=lexstate;switch(_tmpC4){case 0U: _LL1: _LL2:
# 800 "buildlib.cyl"
({void*_tmpC5=0U;({struct _fat_ptr _tmp4B2=({const char*_tmpC6="Warning: unclosed brace\n";_tag_fat(_tmpC6,sizeof(char),25U);});Cyc_log(_tmp4B2,_tag_fat(_tmpC5,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 802
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2U: _LL5: _LL6:
# 807
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3U: _LL7: _LL8:
# 811
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 815
({struct Cyc_Buffer_t*_tmp4B3=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B3,({const char*_tmpC7="/*";_tag_fat(_tmpC7,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 819
({struct Cyc_Buffer_t*_tmp4B4=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B4,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE: {
# 822
struct _fat_ptr _tmpC8=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr symbol=_tmpC8;
if(Cyc_rename_current_symbol && !Cyc_strcmp((struct _fat_ptr)symbol,(struct _fat_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp4B5=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B5,*Cyc_add_user_prefix(({struct _fat_ptr*_tmpC9=_cycalloc(sizeof(*_tmpC9));*_tmpC9=symbol;_tmpC9;})));});else{
# 826
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),symbol);}
return 1;}case 7U: _LLF: _LL10:
# 829
({struct Cyc_Buffer_t*_tmp4B6=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp4B6,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default: _LL11: _LL12:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL0:;}
# 834
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4B7=({const char*_tmpCA="some action didn't return!";_tag_fat(_tmpCA,sizeof(char),27U);});_tmpCB->f1=_tmp4B7;});_tmpCB;}));}
# 836
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpCC=lexstate;switch(_tmpCC){case 0U: _LL1: _LL2:
# 834 "buildlib.cyl"
({void*_tmpCD=0U;({struct _fat_ptr _tmp4B8=({const char*_tmpCE="Warning: unclosed string\n";_tag_fat(_tmpCE,sizeof(char),26U);});Cyc_log(_tmp4B8,_tag_fat(_tmpCD,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 836
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2U: _LL5: _LL6:
# 838
({void*_tmpCF=0U;({struct _fat_ptr _tmp4B9=({const char*_tmpD0="Warning: unclosed string\n";_tag_fat(_tmpD0,sizeof(char),26U);});Cyc_log(_tmp4B9,_tag_fat(_tmpCF,sizeof(void*),0U));});});
({struct Cyc_Buffer_t*_tmp4BA=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BA,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3U: _LL7: _LL8:
# 842
({struct Cyc_Buffer_t*_tmp4BB=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BB,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4U: _LL9: _LLA:
# 845
({struct Cyc_Buffer_t*_tmp4BC=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BC,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5U: _LLB: _LLC:
# 848
({struct Cyc_Buffer_t*_tmp4BD=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BD,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE:
# 851
({struct Cyc_Buffer_t*_tmp4BE=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BE,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7U: _LLF: _LL10:
# 854
({struct Cyc_Buffer_t*_tmp4BF=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BF,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8U: _LL11: _LL12:
# 857
({struct Cyc_Buffer_t*_tmp4C0=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4C0,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL13: _LL14:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL0:;}
# 862
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4C1=({const char*_tmpD1="some action didn't return!";_tag_fat(_tmpD1,sizeof(char),27U);});_tmpD2->f1=_tmp4C1;});_tmpD2;}));}
# 864
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpD3=lexstate;switch(_tmpD3){case 0U: _LL1: _LL2:
# 862 "buildlib.cyl"
({void*_tmpD4=0U;({struct _fat_ptr _tmp4C2=({const char*_tmpD5="Warning: unclosed comment\n";_tag_fat(_tmpD5,sizeof(char),27U);});Cyc_log(_tmp4C2,_tag_fat(_tmpD4,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 864
({struct Cyc_Buffer_t*_tmp4C3=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4C3,({const char*_tmpD6="*/";_tag_fat(_tmpD6,sizeof(char),3U);}));});return 0;case 2U: _LL5: _LL6:
# 866
({struct Cyc_Buffer_t*_tmp4C4=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4C4,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL7: _LL8:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL0:;}
# 871
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp4C5=({const char*_tmpD7="some action didn't return!";_tag_fat(_tmpD7,sizeof(char),27U);});_tmpD8->f1=_tmp4C5;});_tmpD8;}));}
# 873
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 879 "buildlib.cyl"
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpD9=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_stmttmp1=_tmpD9;void*_tmpDA=_stmttmp1;struct Cyc_List_List*_tmpDB;struct Cyc_List_List*_tmpDD;void*_tmpDC;struct _fat_ptr*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct _fat_ptr*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;void*_tmpE2;void*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE8;void**_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;int _tmpE5;struct Cyc_Absyn_Exp*_tmpEA;void*_tmpE9;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_List_List*_tmpFF;void*_tmp100;switch(*((int*)_tmpDA)){case 1U: _LL1: _tmp100=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL2: {void*b=_tmp100;
# 883
struct _fat_ptr*_tmp101=(*Cyc_Absyn_binding2qvar(b)).f2;struct _fat_ptr*v=_tmp101;
Cyc_add_target(v);
return;}case 3U: _LL3: _tmpFF=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL4: {struct Cyc_List_List*x=_tmpFF;
# 887
for(0;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 890
return;}case 23U: _LL5: _tmpFD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpFE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL6: {struct Cyc_Absyn_Exp*e1=_tmpFD;struct Cyc_Absyn_Exp*e2=_tmpFE;
# 892
_tmpFB=e1;_tmpFC=e2;goto _LL8;}case 9U: _LL7: _tmpFB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpFC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmpFB;struct Cyc_Absyn_Exp*e2=_tmpFC;
# 894
_tmpF9=e1;_tmpFA=e2;goto _LLA;}case 4U: _LL9: _tmpF9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpFA=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmpF9;struct Cyc_Absyn_Exp*e2=_tmpFA;
# 896
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 41U: _LLB: _tmpF8=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LLC: {struct Cyc_Absyn_Exp*e1=_tmpF8;
_tmpF7=e1;goto _LLE;}case 20U: _LLD: _tmpF7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LLE: {struct Cyc_Absyn_Exp*e1=_tmpF7;
# 901
_tmpF6=e1;goto _LL10;}case 18U: _LLF: _tmpF6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpF6;
# 903
_tmpF5=e1;goto _LL12;}case 15U: _LL11: _tmpF5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpF5;
# 905
_tmpF4=e1;goto _LL14;}case 5U: _LL13: _tmpF4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpF4;
# 907
Cyc_scan_exp(e1,dep);
return;}case 6U: _LL15: _tmpF1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpF2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_tmpF3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpF1;struct Cyc_Absyn_Exp*e2=_tmpF2;struct Cyc_Absyn_Exp*e3=_tmpF3;
# 910
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
Cyc_scan_exp(e3,dep);
return;}case 7U: _LL17: _tmpEF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpF0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpEF;struct Cyc_Absyn_Exp*e2=_tmpF0;
_tmpED=e1;_tmpEE=e2;goto _LL1A;}case 8U: _LL19: _tmpED=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpEE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpED;struct Cyc_Absyn_Exp*e2=_tmpEE;
# 916
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 10U: _LL1B: _tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpEC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpEB;struct Cyc_List_List*x=_tmpEC;
# 920
Cyc_scan_exp(e1,dep);
for(0;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 924
return;}case 14U: _LL1D: _tmpE9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpEA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1E: {void*t1=_tmpE9;struct Cyc_Absyn_Exp*e1=_tmpEA;
# 926
Cyc_scan_type(t1,dep);
Cyc_scan_exp(e1,dep);
return;}case 34U: _LL1F: _tmpE5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).is_calloc;_tmpE6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).rgn;_tmpE7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).elt_type;_tmpE8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).num_elts;_LL20: {int iscalloc=_tmpE5;struct Cyc_Absyn_Exp*ropt=_tmpE6;void**topt=_tmpE7;struct Cyc_Absyn_Exp*e=_tmpE8;
# 930
if(ropt != 0)Cyc_scan_exp(ropt,dep);
if(topt != 0)Cyc_scan_type(*topt,dep);
Cyc_scan_exp(e,dep);
return;}case 38U: _LL21: _tmpE4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmpE4;
# 935
Cyc_scan_exp(e,dep);return;}case 39U: _LL23: _tmpE3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL24: {void*t1=_tmpE3;
_tmpE2=t1;goto _LL26;}case 17U: _LL25: _tmpE2=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL26: {void*t1=_tmpE2;
# 938
Cyc_scan_type(t1,dep);
return;}case 21U: _LL27: _tmpE0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpE1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpE0;struct _fat_ptr*fn=_tmpE1;
# 941
_tmpDE=e1;_tmpDF=fn;goto _LL2A;}case 22U: _LL29: _tmpDE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpDE;struct _fat_ptr*fn=_tmpDF;
# 943
Cyc_scan_exp(e1,dep);
Cyc_add_target(fn);
return;}case 19U: _LL2B: _tmpDC=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDD=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL2C: {void*t1=_tmpDC;struct Cyc_List_List*f=_tmpDD;
# 947
Cyc_scan_type(t1,dep);
# 949
{void*_tmp102=(void*)((struct Cyc_List_List*)_check_null(f))->hd;void*_stmttmp2=_tmp102;void*_tmp103=_stmttmp2;struct _fat_ptr*_tmp104;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp103)->tag == 0U){_LL58: _tmp104=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp103)->f1;_LL59: {struct _fat_ptr*fn=_tmp104;
Cyc_add_target(fn);goto _LL57;}}else{_LL5A: _LL5B:
 goto _LL57;}_LL57:;}
# 953
return;}case 0U: _LL2D: _LL2E:
# 955
 return;case 36U: _LL2F: _tmpDB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL30: {struct Cyc_List_List*x=_tmpDB;
# 957
for(0;x != 0;x=x->tl){
struct _tuple18*_tmp105=(struct _tuple18*)x->hd;struct _tuple18*_stmttmp3=_tmp105;struct _tuple18*_tmp106=_stmttmp3;struct Cyc_Absyn_Exp*_tmp107;_LL5D: _tmp107=_tmp106->f2;_LL5E: {struct Cyc_Absyn_Exp*e1=_tmp107;
Cyc_scan_exp(e1,dep);}}
# 961
return;}case 40U: _LL31: _LL32:
 return;case 2U: _LL33: _LL34:
# 964
({void*_tmp108=0U;({struct Cyc___cycFILE*_tmp4C7=Cyc_stderr;struct _fat_ptr _tmp4C6=({const char*_tmp109="Error: unexpected Pragma_e\n";_tag_fat(_tmp109,sizeof(char),28U);});Cyc_fprintf(_tmp4C7,_tmp4C6,_tag_fat(_tmp108,sizeof(void*),0U));});});
exit(1);return;case 35U: _LL35: _LL36:
# 967
({void*_tmp10A=0U;({struct Cyc___cycFILE*_tmp4C9=Cyc_stderr;struct _fat_ptr _tmp4C8=({const char*_tmp10B="Error: unexpected Swap_e\n";_tag_fat(_tmp10B,sizeof(char),26U);});Cyc_fprintf(_tmp4C9,_tmp4C8,_tag_fat(_tmp10A,sizeof(void*),0U));});});
exit(1);return;case 37U: _LL37: _LL38:
# 970
({void*_tmp10C=0U;({struct Cyc___cycFILE*_tmp4CB=Cyc_stderr;struct _fat_ptr _tmp4CA=({const char*_tmp10D="Error: unexpected Stmt_e\n";_tag_fat(_tmp10D,sizeof(char),26U);});Cyc_fprintf(_tmp4CB,_tmp4CA,_tag_fat(_tmp10C,sizeof(void*),0U));});});
exit(1);return;case 42U: _LL39: _LL3A:
# 973
({void*_tmp10E=0U;({struct Cyc___cycFILE*_tmp4CD=Cyc_stderr;struct _fat_ptr _tmp4CC=({const char*_tmp10F="Error: unexpected Assert_e\n";_tag_fat(_tmp10F,sizeof(char),28U);});Cyc_fprintf(_tmp4CD,_tmp4CC,_tag_fat(_tmp10E,sizeof(void*),0U));});});
exit(1);return;case 11U: _LL3B: _LL3C:
# 976
({void*_tmp110=0U;({struct Cyc___cycFILE*_tmp4CF=Cyc_stderr;struct _fat_ptr _tmp4CE=({const char*_tmp111="Error: unexpected Throw_e\n";_tag_fat(_tmp111,sizeof(char),27U);});Cyc_fprintf(_tmp4CF,_tmp4CE,_tag_fat(_tmp110,sizeof(void*),0U));});});
exit(1);return;case 12U: _LL3D: _LL3E:
# 979
({void*_tmp112=0U;({struct Cyc___cycFILE*_tmp4D1=Cyc_stderr;struct _fat_ptr _tmp4D0=({const char*_tmp113="Error: unexpected NoInstantiate_e\n";_tag_fat(_tmp113,sizeof(char),35U);});Cyc_fprintf(_tmp4D1,_tmp4D0,_tag_fat(_tmp112,sizeof(void*),0U));});});
exit(1);return;case 13U: _LL3F: _LL40:
# 982
({void*_tmp114=0U;({struct Cyc___cycFILE*_tmp4D3=Cyc_stderr;struct _fat_ptr _tmp4D2=({const char*_tmp115="Error: unexpected Instantiate_e\n";_tag_fat(_tmp115,sizeof(char),33U);});Cyc_fprintf(_tmp4D3,_tmp4D2,_tag_fat(_tmp114,sizeof(void*),0U));});});
exit(1);return;case 16U: _LL41: _LL42:
# 985
({void*_tmp116=0U;({struct Cyc___cycFILE*_tmp4D5=Cyc_stderr;struct _fat_ptr _tmp4D4=({const char*_tmp117="Error: unexpected New_e\n";_tag_fat(_tmp117,sizeof(char),25U);});Cyc_fprintf(_tmp4D5,_tmp4D4,_tag_fat(_tmp116,sizeof(void*),0U));});});
exit(1);return;case 24U: _LL43: _LL44:
# 988
({void*_tmp118=0U;({struct Cyc___cycFILE*_tmp4D7=Cyc_stderr;struct _fat_ptr _tmp4D6=({const char*_tmp119="Error: unexpected Tuple_e\n";_tag_fat(_tmp119,sizeof(char),27U);});Cyc_fprintf(_tmp4D7,_tmp4D6,_tag_fat(_tmp118,sizeof(void*),0U));});});
exit(1);return;case 25U: _LL45: _LL46:
# 991
({void*_tmp11A=0U;({struct Cyc___cycFILE*_tmp4D9=Cyc_stderr;struct _fat_ptr _tmp4D8=({const char*_tmp11B="Error: unexpected CompoundLit_e\n";_tag_fat(_tmp11B,sizeof(char),33U);});Cyc_fprintf(_tmp4D9,_tmp4D8,_tag_fat(_tmp11A,sizeof(void*),0U));});});
exit(1);return;case 26U: _LL47: _LL48:
# 994
({void*_tmp11C=0U;({struct Cyc___cycFILE*_tmp4DB=Cyc_stderr;struct _fat_ptr _tmp4DA=({const char*_tmp11D="Error: unexpected Array_e\n";_tag_fat(_tmp11D,sizeof(char),27U);});Cyc_fprintf(_tmp4DB,_tmp4DA,_tag_fat(_tmp11C,sizeof(void*),0U));});});
exit(1);return;case 27U: _LL49: _LL4A:
# 997
({void*_tmp11E=0U;({struct Cyc___cycFILE*_tmp4DD=Cyc_stderr;struct _fat_ptr _tmp4DC=({const char*_tmp11F="Error: unexpected Comprehension_e\n";_tag_fat(_tmp11F,sizeof(char),35U);});Cyc_fprintf(_tmp4DD,_tmp4DC,_tag_fat(_tmp11E,sizeof(void*),0U));});});
exit(1);return;case 28U: _LL4B: _LL4C:
# 1000
({void*_tmp120=0U;({struct Cyc___cycFILE*_tmp4DF=Cyc_stderr;struct _fat_ptr _tmp4DE=({const char*_tmp121="Error: unexpected ComprehensionNoinit_e\n";_tag_fat(_tmp121,sizeof(char),41U);});Cyc_fprintf(_tmp4DF,_tmp4DE,_tag_fat(_tmp120,sizeof(void*),0U));});});
exit(1);return;case 29U: _LL4D: _LL4E:
# 1003
({void*_tmp122=0U;({struct Cyc___cycFILE*_tmp4E1=Cyc_stderr;struct _fat_ptr _tmp4E0=({const char*_tmp123="Error: unexpected Aggregate_e\n";_tag_fat(_tmp123,sizeof(char),31U);});Cyc_fprintf(_tmp4E1,_tmp4E0,_tag_fat(_tmp122,sizeof(void*),0U));});});
exit(1);return;case 30U: _LL4F: _LL50:
# 1006
({void*_tmp124=0U;({struct Cyc___cycFILE*_tmp4E3=Cyc_stderr;struct _fat_ptr _tmp4E2=({const char*_tmp125="Error: unexpected AnonStruct_e\n";_tag_fat(_tmp125,sizeof(char),32U);});Cyc_fprintf(_tmp4E3,_tmp4E2,_tag_fat(_tmp124,sizeof(void*),0U));});});
exit(1);return;case 31U: _LL51: _LL52:
# 1009
({void*_tmp126=0U;({struct Cyc___cycFILE*_tmp4E5=Cyc_stderr;struct _fat_ptr _tmp4E4=({const char*_tmp127="Error: unexpected Datatype_e\n";_tag_fat(_tmp127,sizeof(char),30U);});Cyc_fprintf(_tmp4E5,_tmp4E4,_tag_fat(_tmp126,sizeof(void*),0U));});});
exit(1);return;case 32U: _LL53: _LL54:
# 1012
({void*_tmp128=0U;({struct Cyc___cycFILE*_tmp4E7=Cyc_stderr;struct _fat_ptr _tmp4E6=({const char*_tmp129="Error: unexpected Enum_e\n";_tag_fat(_tmp129,sizeof(char),26U);});Cyc_fprintf(_tmp4E7,_tmp4E6,_tag_fat(_tmp128,sizeof(void*),0U));});});
exit(1);return;default: _LL55: _LL56:
# 1015
({void*_tmp12A=0U;({struct Cyc___cycFILE*_tmp4E9=Cyc_stderr;struct _fat_ptr _tmp4E8=({const char*_tmp12B="Error: unexpected AnonEnum_e\n";_tag_fat(_tmp12B,sizeof(char),30U);});Cyc_fprintf(_tmp4E9,_tmp4E8,_tag_fat(_tmp12A,sizeof(void*),0U));});});
exit(1);return;}_LL0:;}
# 1020
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned)eo)Cyc_scan_exp(eo,dep);
return;}
# 1025
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp12C=t;struct Cyc_Absyn_Datatypedecl*_tmp12D;struct Cyc_Absyn_Enumdecl*_tmp12E;struct Cyc_Absyn_Aggrdecl*_tmp12F;struct _fat_ptr*_tmp130;struct Cyc_List_List*_tmp131;struct Cyc_Absyn_FnInfo _tmp132;struct Cyc_Absyn_Exp*_tmp133;void*_tmp136;struct Cyc_Absyn_Exp*_tmp135;void*_tmp134;struct Cyc_Absyn_PtrInfo _tmp137;struct Cyc_List_List*_tmp139;void*_tmp138;switch(*((int*)_tmp12C)){case 0U: _LL1: _tmp138=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12C)->f1;_tmp139=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12C)->f2;_LL2: {void*c=_tmp138;struct Cyc_List_List*ts=_tmp139;
# 1029
void*_tmp13A=c;struct _fat_ptr*_tmp13B;union Cyc_Absyn_AggrInfo _tmp13C;switch(*((int*)_tmp13A)){case 0U: _LL1E: _LL1F:
 goto _LL21;case 1U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 goto _LL25;case 2U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
# 1035
 return;case 20U: _LL28: _tmp13C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp13A)->f1;_LL29: {union Cyc_Absyn_AggrInfo info=_tmp13C;
# 1037
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
({struct Cyc_String_pa_PrintArg_struct _tmp142=({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0U,({struct _fat_ptr _tmp4EA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp3DF.f1=_tmp4EA;});_tmp3DF;});void*_tmp140[1U];_tmp140[0]=& _tmp142;({struct Cyc___cycFILE*_tmp4EC=Cyc_stderr;struct _fat_ptr _tmp4EB=({const char*_tmp141="Error: unexpected %s\n";_tag_fat(_tmp141,sizeof(char),22U);});Cyc_fprintf(_tmp4EC,_tmp4EB,_tag_fat(_tmp140,sizeof(void*),1U));});});
exit(1);return;}_LL1D:;}case 3U: _LL3: _tmp137=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12C)->f1;_LL4: {struct Cyc_Absyn_PtrInfo x=_tmp137;
# 1052
Cyc_scan_type(x.elt_type,dep);
return;}case 4U: _LL5: _tmp134=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C)->f1).elt_type;_tmp135=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C)->f1).num_elts;_tmp136=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12C)->f1).zero_term;_LL6: {void*t=_tmp134;struct Cyc_Absyn_Exp*sz=_tmp135;void*zt=_tmp136;
# 1055
Cyc_scan_type(t,dep);
Cyc_scan_exp_opt(sz,dep);
return;}case 11U: _LL7: _tmp133=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp12C)->f1;_LL8: {struct Cyc_Absyn_Exp*e=_tmp133;
# 1059
Cyc_scan_exp(e,dep);
return;}case 5U: _LL9: _tmp132=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12C)->f1;_LLA: {struct Cyc_Absyn_FnInfo x=_tmp132;
# 1062
Cyc_scan_type(x.ret_type,dep);
{struct Cyc_List_List*_tmp143=x.args;struct Cyc_List_List*a=_tmp143;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp144=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5=_tmp144;struct _tuple9*_tmp145=_stmttmp5;void*_tmp146;_LL4C: _tmp146=_tmp145->f3;_LL4D: {void*argt=_tmp146;
Cyc_scan_type(argt,dep);}}}
# 1067
if(x.cyc_varargs != 0)
Cyc_scan_type((x.cyc_varargs)->type,dep);
return;}case 7U: _LLB: _tmp131=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12C)->f2;_LLC: {struct Cyc_List_List*sfs=_tmp131;
# 1071
for(0;sfs != 0;sfs=sfs->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->width,dep);}
# 1075
return;}case 8U: _LLD: _tmp130=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp12C)->f1)->f2;_LLE: {struct _fat_ptr*v=_tmp130;
# 1077
Cyc_add_target(v);
return;}case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)){case 0U: _LLF: _tmp12F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)->f1;_LL10: {struct Cyc_Absyn_Aggrdecl*x=_tmp12F;
# 1081
({struct Cyc_Absyn_Decl*_tmp4ED=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->tag=6U,_tmp147->f1=x;_tmp147;}),0U);Cyc_scan_decl(_tmp4ED,dep);});{
struct _tuple1*_tmp148=x->name;struct _tuple1*_stmttmp6=_tmp148;struct _tuple1*_tmp149=_stmttmp6;struct _fat_ptr*_tmp14A;_LL4F: _tmp14A=_tmp149->f2;_LL50: {struct _fat_ptr*n=_tmp14A;
Cyc_add_target(n);
return;}}}case 1U: _LL11: _tmp12E=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)->f1;_LL12: {struct Cyc_Absyn_Enumdecl*x=_tmp12E;
# 1087
({struct Cyc_Absyn_Decl*_tmp4EE=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->tag=8U,_tmp14B->f1=x;_tmp14B;}),0U);Cyc_scan_decl(_tmp4EE,dep);});{
struct _tuple1*_tmp14C=x->name;struct _tuple1*_stmttmp7=_tmp14C;struct _tuple1*_tmp14D=_stmttmp7;struct _fat_ptr*_tmp14E;_LL52: _tmp14E=_tmp14D->f2;_LL53: {struct _fat_ptr*n=_tmp14E;
Cyc_add_target(n);
return;}}}default: _LL13: _tmp12D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12C)->f1)->r)->f1;_LL14: {struct Cyc_Absyn_Datatypedecl*dd=_tmp12D;
# 1093
({void*_tmp14F=0U;({struct Cyc___cycFILE*_tmp4F0=Cyc_stderr;struct _fat_ptr _tmp4EF=({const char*_tmp150="Error: unexpected Datatype declaration\n";_tag_fat(_tmp150,sizeof(char),40U);});Cyc_fprintf(_tmp4F0,_tmp4EF,_tag_fat(_tmp14F,sizeof(void*),0U));});});
exit(1);return;}}case 1U: _LL15: _LL16:
# 1096
({void*_tmp151=0U;({struct Cyc___cycFILE*_tmp4F2=Cyc_stderr;struct _fat_ptr _tmp4F1=({const char*_tmp152="Error: unexpected Evar\n";_tag_fat(_tmp152,sizeof(char),24U);});Cyc_fprintf(_tmp4F2,_tmp4F1,_tag_fat(_tmp151,sizeof(void*),0U));});});
exit(1);return;case 2U: _LL17: _LL18:
# 1099
({void*_tmp153=0U;({struct Cyc___cycFILE*_tmp4F4=Cyc_stderr;struct _fat_ptr _tmp4F3=({const char*_tmp154="Error: unexpected VarType\n";_tag_fat(_tmp154,sizeof(char),27U);});Cyc_fprintf(_tmp4F4,_tmp4F3,_tag_fat(_tmp153,sizeof(void*),0U));});});
exit(1);return;case 6U: _LL19: _LL1A:
# 1102
({void*_tmp155=0U;({struct Cyc___cycFILE*_tmp4F6=Cyc_stderr;struct _fat_ptr _tmp4F5=({const char*_tmp156="Error: unexpected TupleType\n";_tag_fat(_tmp156,sizeof(char),29U);});Cyc_fprintf(_tmp4F6,_tmp4F5,_tag_fat(_tmp155,sizeof(void*),0U));});});
exit(1);return;default: _LL1B: _LL1C:
# 1105
({void*_tmp157=0U;({struct Cyc___cycFILE*_tmp4F8=Cyc_stderr;struct _fat_ptr _tmp4F7=({const char*_tmp158="Error: unexpected valueof_t\n";_tag_fat(_tmp158,sizeof(char),29U);});Cyc_fprintf(_tmp4F8,_tmp4F7,_tag_fat(_tmp157,sizeof(void*),0U));});});
exit(1);return;}_LL0:;}
# 1110
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp159=Cyc_current_targets;struct Cyc_Set_Set**saved_targets=_tmp159;
struct _fat_ptr*_tmp15A=Cyc_current_source;struct _fat_ptr*saved_source=_tmp15A;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp15B=_cycalloc(sizeof(*_tmp15B));({struct Cyc_Set_Set*_tmp4F9=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp15B=_tmp4F9;});_tmp15B;});
{void*_tmp15C=d->r;void*_stmttmp8=_tmp15C;void*_tmp15D=_stmttmp8;struct Cyc_Absyn_Typedefdecl*_tmp15E;struct Cyc_Absyn_Enumdecl*_tmp15F;struct Cyc_Absyn_Aggrdecl*_tmp160;struct Cyc_Absyn_Fndecl*_tmp161;struct Cyc_Absyn_Vardecl*_tmp162;switch(*((int*)_tmp15D)){case 0U: _LL1: _tmp162=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp162;
# 1116
struct _tuple1*_tmp163=x->name;struct _tuple1*_stmttmp9=_tmp163;struct _tuple1*_tmp164=_stmttmp9;struct _fat_ptr*_tmp165;_LL26: _tmp165=_tmp164->f2;_LL27: {struct _fat_ptr*v=_tmp165;
Cyc_current_source=v;
Cyc_scan_type(x->type,dep);
Cyc_scan_exp_opt(x->initializer,dep);
goto _LL0;}}case 1U: _LL3: _tmp161=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL4: {struct Cyc_Absyn_Fndecl*x=_tmp161;
# 1122
struct _tuple1*_tmp166=x->name;struct _tuple1*_stmttmpA=_tmp166;struct _tuple1*_tmp167=_stmttmpA;struct _fat_ptr*_tmp168;_LL29: _tmp168=_tmp167->f2;_LL2A: {struct _fat_ptr*v=_tmp168;
Cyc_current_source=v;
Cyc_scan_type((x->i).ret_type,dep);
{struct Cyc_List_List*_tmp169=(x->i).args;struct Cyc_List_List*a=_tmp169;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp16A=(struct _tuple9*)a->hd;struct _tuple9*_stmttmpB=_tmp16A;struct _tuple9*_tmp16B=_stmttmpB;void*_tmp16C;_LL2C: _tmp16C=_tmp16B->f3;_LL2D: {void*t1=_tmp16C;
Cyc_scan_type(t1,dep);}}}
# 1129
if((x->i).cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null((x->i).cyc_varargs))->type,dep);
if(x->is_inline)
({void*_tmp16D=0U;({struct Cyc___cycFILE*_tmp4FB=Cyc_stderr;struct _fat_ptr _tmp4FA=({const char*_tmp16E="Warning: ignoring inline function\n";_tag_fat(_tmp16E,sizeof(char),35U);});Cyc_fprintf(_tmp4FB,_tmp4FA,_tag_fat(_tmp16D,sizeof(void*),0U));});});
goto _LL0;}}case 6U: _LL5: _tmp160=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL6: {struct Cyc_Absyn_Aggrdecl*x=_tmp160;
# 1135
struct _tuple1*_tmp16F=x->name;struct _tuple1*_stmttmpC=_tmp16F;struct _tuple1*_tmp170=_stmttmpC;struct _fat_ptr*_tmp171;_LL2F: _tmp171=_tmp170->f2;_LL30: {struct _fat_ptr*v=_tmp171;
Cyc_current_source=v;
if((unsigned)x->impl){
{struct Cyc_List_List*_tmp172=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp172;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp173=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp173;
Cyc_scan_type(f->type,dep);
Cyc_scan_exp_opt(f->width,dep);}}{
# 1145
struct Cyc_List_List*_tmp174=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp174;for(0;fs != 0;fs=fs->tl){;}}}
# 1149
goto _LL0;}}case 8U: _LL7: _tmp15F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LL8: {struct Cyc_Absyn_Enumdecl*x=_tmp15F;
# 1151
struct _tuple1*_tmp175=x->name;struct _tuple1*_stmttmpD=_tmp175;struct _tuple1*_tmp176=_stmttmpD;struct _fat_ptr*_tmp177;_LL32: _tmp177=_tmp176->f2;_LL33: {struct _fat_ptr*v=_tmp177;
Cyc_current_source=v;
if((unsigned)x->fields){
{struct Cyc_List_List*_tmp178=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp178;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp179=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp179;
Cyc_scan_exp_opt(f->tag,dep);}}{
# 1160
struct Cyc_List_List*_tmp17A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp17A;for(0;fs != 0;fs=fs->tl){;}}}
# 1164
goto _LL0;}}case 9U: _LL9: _tmp15E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp15D)->f1;_LLA: {struct Cyc_Absyn_Typedefdecl*x=_tmp15E;
# 1166
struct _tuple1*_tmp17B=x->name;struct _tuple1*_stmttmpE=_tmp17B;struct _tuple1*_tmp17C=_stmttmpE;struct _fat_ptr*_tmp17D;_LL35: _tmp17D=_tmp17C->f2;_LL36: {struct _fat_ptr*v=_tmp17D;
Cyc_current_source=v;
if((unsigned)x->defn)
Cyc_scan_type((void*)_check_null(x->defn),dep);
goto _LL0;}}case 4U: _LLB: _LLC:
# 1172
({void*_tmp17E=0U;({struct Cyc___cycFILE*_tmp4FD=Cyc_stderr;struct _fat_ptr _tmp4FC=({const char*_tmp17F="Error: unexpected region declaration";_tag_fat(_tmp17F,sizeof(char),37U);});Cyc_fprintf(_tmp4FD,_tmp4FC,_tag_fat(_tmp17E,sizeof(void*),0U));});});
exit(1);case 5U: _LLD: _LLE:
# 1175
({void*_tmp180=0U;({struct Cyc___cycFILE*_tmp4FF=Cyc_stderr;struct _fat_ptr _tmp4FE=({const char*_tmp181="Error: unexpected pool declaration";_tag_fat(_tmp181,sizeof(char),35U);});Cyc_fprintf(_tmp4FF,_tmp4FE,_tag_fat(_tmp180,sizeof(void*),0U));});});
exit(1);case 14U: _LLF: _LL10:
# 1178
({void*_tmp182=0U;({struct Cyc___cycFILE*_tmp501=Cyc_stderr;struct _fat_ptr _tmp500=({const char*_tmp183="Error: unexpected __cyclone_port_on__";_tag_fat(_tmp183,sizeof(char),38U);});Cyc_fprintf(_tmp501,_tmp500,_tag_fat(_tmp182,sizeof(void*),0U));});});
exit(1);case 15U: _LL11: _LL12:
# 1181
({void*_tmp184=0U;({struct Cyc___cycFILE*_tmp503=Cyc_stderr;struct _fat_ptr _tmp502=({const char*_tmp185="Error: unexpected __cyclone_port_off__";_tag_fat(_tmp185,sizeof(char),39U);});Cyc_fprintf(_tmp503,_tmp502,_tag_fat(_tmp184,sizeof(void*),0U));});});
exit(1);case 16U: _LL13: _LL14:
# 1184
({void*_tmp186=0U;({struct Cyc___cycFILE*_tmp505=Cyc_stderr;struct _fat_ptr _tmp504=({const char*_tmp187="Error: unexpected __tempest_on__";_tag_fat(_tmp187,sizeof(char),33U);});Cyc_fprintf(_tmp505,_tmp504,_tag_fat(_tmp186,sizeof(void*),0U));});});
exit(1);case 17U: _LL15: _LL16:
# 1187
({void*_tmp188=0U;({struct Cyc___cycFILE*_tmp507=Cyc_stderr;struct _fat_ptr _tmp506=({const char*_tmp189="Error: unexpected __tempest_off__";_tag_fat(_tmp189,sizeof(char),34U);});Cyc_fprintf(_tmp507,_tmp506,_tag_fat(_tmp188,sizeof(void*),0U));});});
exit(1);case 2U: _LL17: _LL18:
# 1190
({void*_tmp18A=0U;({struct Cyc___cycFILE*_tmp509=Cyc_stderr;struct _fat_ptr _tmp508=({const char*_tmp18B="Error: unexpected let declaration\n";_tag_fat(_tmp18B,sizeof(char),35U);});Cyc_fprintf(_tmp509,_tmp508,_tag_fat(_tmp18A,sizeof(void*),0U));});});
exit(1);case 7U: _LL19: _LL1A:
# 1193
({void*_tmp18C=0U;({struct Cyc___cycFILE*_tmp50B=Cyc_stderr;struct _fat_ptr _tmp50A=({const char*_tmp18D="Error: unexpected datatype declaration\n";_tag_fat(_tmp18D,sizeof(char),40U);});Cyc_fprintf(_tmp50B,_tmp50A,_tag_fat(_tmp18C,sizeof(void*),0U));});});
exit(1);case 3U: _LL1B: _LL1C:
# 1196
({void*_tmp18E=0U;({struct Cyc___cycFILE*_tmp50D=Cyc_stderr;struct _fat_ptr _tmp50C=({const char*_tmp18F="Error: unexpected let declaration\n";_tag_fat(_tmp18F,sizeof(char),35U);});Cyc_fprintf(_tmp50D,_tmp50C,_tag_fat(_tmp18E,sizeof(void*),0U));});});
exit(1);case 10U: _LL1D: _LL1E:
# 1199
({void*_tmp190=0U;({struct Cyc___cycFILE*_tmp50F=Cyc_stderr;struct _fat_ptr _tmp50E=({const char*_tmp191="Error: unexpected namespace declaration\n";_tag_fat(_tmp191,sizeof(char),41U);});Cyc_fprintf(_tmp50F,_tmp50E,_tag_fat(_tmp190,sizeof(void*),0U));});});
exit(1);case 11U: _LL1F: _LL20:
# 1202
({void*_tmp192=0U;({struct Cyc___cycFILE*_tmp511=Cyc_stderr;struct _fat_ptr _tmp510=({const char*_tmp193="Error: unexpected using declaration\n";_tag_fat(_tmp193,sizeof(char),37U);});Cyc_fprintf(_tmp511,_tmp510,_tag_fat(_tmp192,sizeof(void*),0U));});});
exit(1);case 12U: _LL21: _LL22:
# 1205
({void*_tmp194=0U;({struct Cyc___cycFILE*_tmp513=Cyc_stderr;struct _fat_ptr _tmp512=({const char*_tmp195="Error: unexpected extern \"C\" declaration\n";_tag_fat(_tmp195,sizeof(char),42U);});Cyc_fprintf(_tmp513,_tmp512,_tag_fat(_tmp194,sizeof(void*),0U));});});
exit(1);default: _LL23: _LL24:
# 1208
({void*_tmp196=0U;({struct Cyc___cycFILE*_tmp515=Cyc_stderr;struct _fat_ptr _tmp514=({const char*_tmp197="Error: unexpected extern \"C include\" declaration\n";_tag_fat(_tmp197,sizeof(char),50U);});Cyc_fprintf(_tmp515,_tmp514,_tag_fat(_tmp196,sizeof(void*),0U));});});
exit(1);}_LL0:;}{
# 1216
struct Cyc_Set_Set*old;
struct _fat_ptr*_tmp198=(struct _fat_ptr*)_check_null(Cyc_current_source);struct _fat_ptr*name=_tmp198;
{struct _handler_cons _tmp199;_push_handler(& _tmp199);{int _tmp19B=0;if(setjmp(_tmp199.handler))_tmp19B=1;if(!_tmp19B){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key))Cyc_Hashtable_lookup)(dep,name);;_pop_handler();}else{void*_tmp19A=(void*)Cyc_Core_get_exn_thrown();void*_tmp19C=_tmp19A;void*_tmp19D;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp19C)->tag == Cyc_Core_Not_found){_LL38: _LL39:
# 1221
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL37;}else{_LL3A: _tmp19D=_tmp19C;_LL3B: {void*exn=_tmp19D;(int)_rethrow(exn);}}_LL37:;}}}{
# 1223
struct Cyc_Set_Set*_tmp19E=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);struct Cyc_Set_Set*targets=_tmp19E;
((void(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,name,targets);
# 1226
Cyc_current_targets=saved_targets;
Cyc_current_source=saved_source;}}}
# 1230
struct Cyc_Hashtable_Table*Cyc_new_deps (void){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),int(*hash)(struct _fat_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1234
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _fat_ptr*x){
struct _handler_cons _tmp19F;_push_handler(& _tmp19F);{int _tmp1A1=0;if(setjmp(_tmp19F.handler))_tmp1A1=1;if(!_tmp1A1){{struct Cyc_Set_Set*_tmp1A2=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0U);return _tmp1A2;};_pop_handler();}else{void*_tmp1A0=(void*)Cyc_Core_get_exn_thrown();void*_tmp1A3=_tmp1A0;void*_tmp1A4;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp1A3)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 1237
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL3: _tmp1A4=_tmp1A3;_LL4: {void*exn=_tmp1A4;(int)_rethrow(exn);}}_LL0:;}}}
# 1241
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1251 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(curr,(struct _fat_ptr*)init->hd);}{
# 1256
struct Cyc_Set_Set*_tmp1A5=curr;struct Cyc_Set_Set*delta=_tmp1A5;
# 1258
struct _fat_ptr*_tmp1A6=({struct _fat_ptr*_tmp1AA=_cycalloc(sizeof(*_tmp1AA));({struct _fat_ptr _tmp516=({const char*_tmp1A9="";_tag_fat(_tmp1A9,sizeof(char),1U);});*_tmp1AA=_tmp516;});_tmp1AA;});struct _fat_ptr*sptr=_tmp1A6;
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(delta)> 0){
struct Cyc_Set_Set*_tmp1A7=emptyset;struct Cyc_Set_Set*next=_tmp1A7;
struct Cyc_Iter_Iter _tmp1A8=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,delta);struct Cyc_Iter_Iter iter=_tmp1A8;
while(((int(*)(struct Cyc_Iter_Iter,struct _fat_ptr**))Cyc_Iter_next)(iter,& sptr)){
next=({struct Cyc_Set_Set*_tmp517=next;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp517,Cyc_find(t,sptr));});}
delta=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(next,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,delta);}
# 1267
return curr;}}
# 1270
enum Cyc_buildlib_mode{Cyc_NORMAL =0U,Cyc_GATHER =1U,Cyc_GATHERSCRIPT =2U,Cyc_FINISH =3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering (void){
return(int)Cyc_mode == (int)Cyc_GATHER ||(int)Cyc_mode == (int)Cyc_GATHERSCRIPT;}
# 1276
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _fat_ptr fmt,struct _fat_ptr ap){
# 1280
if(Cyc_script_file == 0){
({void*_tmp1AB=0U;({struct Cyc___cycFILE*_tmp519=Cyc_stderr;struct _fat_ptr _tmp518=({const char*_tmp1AC="Internal error: script file is NULL\n";_tag_fat(_tmp1AC,sizeof(char),37U);});Cyc_fprintf(_tmp519,_tmp518,_tag_fat(_tmp1AB,sizeof(void*),0U));});});
exit(1);}
# 1284
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1287
int Cyc_force_directory(struct _fat_ptr d){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp1AF=({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0U,_tmp3E1.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3E1;});struct Cyc_String_pa_PrintArg_struct _tmp1B0=({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0U,_tmp3E0.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3E0;});void*_tmp1AD[2U];_tmp1AD[0]=& _tmp1AF,_tmp1AD[1]=& _tmp1B0;({struct _fat_ptr _tmp51A=({const char*_tmp1AE="if ! test -e %s; then mkdir %s; fi\n";_tag_fat(_tmp1AE,sizeof(char),36U);});Cyc_prscript(_tmp51A,_tag_fat(_tmp1AD,sizeof(void*),2U));});});else{
# 1294
int _tmp1B1=({void*_tmp1B5=0U;({const char*_tmp51B=(const char*)_untag_fat_ptr(d,sizeof(char),1U);Cyc_open(_tmp51B,0,_tag_fat(_tmp1B5,sizeof(unsigned short),0U));});});int fd=_tmp1B1;
if(fd == -1){
if(mkdir((const char*)_check_null(_untag_fat_ptr(d,sizeof(char),1U)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp1B4=({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0U,_tmp3E2.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3E2;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B4;({struct Cyc___cycFILE*_tmp51D=Cyc_stderr;struct _fat_ptr _tmp51C=({const char*_tmp1B3="Error: could not create directory %s\n";_tag_fat(_tmp1B3,sizeof(char),38U);});Cyc_fprintf(_tmp51D,_tmp51C,_tag_fat(_tmp1B2,sizeof(void*),1U));});});
return 1;}}else{
# 1301
close(fd);}}
# 1303
return 0;}
# 1306
int Cyc_force_directory_prefixes(struct _fat_ptr file){
# 1310
struct _fat_ptr _tmp1B6=Cyc_strdup((struct _fat_ptr)file);struct _fat_ptr curr=_tmp1B6;
# 1312
struct Cyc_List_List*_tmp1B7=0;struct Cyc_List_List*x=_tmp1B7;
while(1){
curr=Cyc_Filename_dirname((struct _fat_ptr)curr);
if(Cyc_strlen((struct _fat_ptr)curr)== (unsigned long)0)break;
x=({struct Cyc_List_List*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));({struct _fat_ptr*_tmp51E=({struct _fat_ptr*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));*_tmp1B8=(struct _fat_ptr)curr;_tmp1B8;});_tmp1B9->hd=_tmp51E;}),_tmp1B9->tl=x;_tmp1B9;});}
# 1319
for(0;x != 0;x=x->tl){
if(Cyc_force_directory(*((struct _fat_ptr*)x->hd)))return 1;}
# 1322
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _fat_ptr f1;};
# 1329
static int Cyc_is_other_special(char c){
char _tmp1BA=c;switch(_tmp1BA){case 92U: _LL1: _LL2:
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
# 1344
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 1350
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmp1BB=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp1BB;
# 1354
int _tmp1BC=0;int single_quotes=_tmp1BC;
int _tmp1BD=0;int other_special=_tmp1BD;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmp1BE=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmp1BE;
if((int)c == (int)'\'')++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 1363
if(single_quotes == 0 && other_special == 0)
return s;
# 1367
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmp1BF[3U];({struct _fat_ptr*_tmp523=({struct _fat_ptr*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));({struct _fat_ptr _tmp522=({const char*_tmp1C0="'";_tag_fat(_tmp1C0,sizeof(char),2U);});*_tmp1C1=_tmp522;});_tmp1C1;});_tmp1BF[0]=_tmp523;}),({struct _fat_ptr*_tmp521=({struct _fat_ptr*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));*_tmp1C2=(struct _fat_ptr)s;_tmp1C2;});_tmp1BF[1]=_tmp521;}),({struct _fat_ptr*_tmp520=({struct _fat_ptr*_tmp1C4=_cycalloc(sizeof(*_tmp1C4));({struct _fat_ptr _tmp51F=({const char*_tmp1C3="'";_tag_fat(_tmp1C3,sizeof(char),2U);});*_tmp1C4=_tmp51F;});_tmp1C4;});_tmp1BF[2]=_tmp520;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BF,sizeof(struct _fat_ptr*),3U));}));{
# 1371
unsigned long _tmp1C5=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmp1C5;
struct _fat_ptr s2=({unsigned _tmp1D0=(len2 + (unsigned long)1)+ 1U;char*_tmp1CF=_cycalloc_atomic(_check_times(_tmp1D0,sizeof(char)));({{unsigned _tmp3E3=len2 + (unsigned long)1;unsigned i;for(i=0;i < _tmp3E3;++ i){_tmp1CF[i]='\000';}_tmp1CF[_tmp3E3]=0;}0;});_tag_fat(_tmp1CF,sizeof(char),_tmp1D0);});
int _tmp1C6=0;int i=_tmp1C6;
int _tmp1C7=0;int j=_tmp1C7;
for(0;(unsigned long)i < len;++ i){
char _tmp1C8=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmp1C8;
if((int)c == (int)'\'' || Cyc_is_other_special(c))
({struct _fat_ptr _tmp1C9=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp1CA=*((char*)_check_fat_subscript(_tmp1C9,sizeof(char),0U));char _tmp1CB='\\';if(_get_fat_size(_tmp1C9,sizeof(char))== 1U &&(_tmp1CA == 0 && _tmp1CB != 0))_throw_arraybounds();*((char*)_tmp1C9.curr)=_tmp1CB;});
({struct _fat_ptr _tmp1CC=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp1CD=*((char*)_check_fat_subscript(_tmp1CC,sizeof(char),0U));char _tmp1CE=c;if(_get_fat_size(_tmp1CC,sizeof(char))== 1U &&(_tmp1CD == 0 && _tmp1CE != 0))_throw_arraybounds();*((char*)_tmp1CC.curr)=_tmp1CE;});}
# 1381
return(struct _fat_ptr)s2;}}
# 1383
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));({struct _fat_ptr _tmp524=Cyc_sh_escape_string(*sp);*_tmp1D1=_tmp524;});_tmp1D1;});}
# 1388
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1396
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1401
({struct Cyc_String_pa_PrintArg_struct _tmp1D4=({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0U,({
struct _fat_ptr _tmp525=(struct _fat_ptr)({const char*_tmp1D5=filename;_tag_fat(_tmp1D5,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D5,1U));});_tmp3E4.f1=_tmp525;});_tmp3E4;});void*_tmp1D2[1U];_tmp1D2[0]=& _tmp1D4;({struct Cyc___cycFILE*_tmp527=Cyc_stderr;struct _fat_ptr _tmp526=({const char*_tmp1D3="********************************* %s...\n";_tag_fat(_tmp1D3,sizeof(char),41U);});Cyc_fprintf(_tmp527,_tmp526,_tag_fat(_tmp1D2,sizeof(void*),1U));});});
# 1404
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp1D8=({struct Cyc_String_pa_PrintArg_struct _tmp3E5;_tmp3E5.tag=0U,({struct _fat_ptr _tmp528=(struct _fat_ptr)({const char*_tmp1D9=filename;_tag_fat(_tmp1D9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D9,1U));});_tmp3E5.f1=_tmp528;});_tmp3E5;});void*_tmp1D6[1U];_tmp1D6[0]=& _tmp1D8;({struct _fat_ptr _tmp529=({const char*_tmp1D7="\n%s:\n";_tag_fat(_tmp1D7,sizeof(char),6U);});Cyc_log(_tmp529,_tag_fat(_tmp1D6,sizeof(void*),1U));});});{
# 1416 "buildlib.cyl"
struct _fat_ptr _tmp1DA=Cyc_Filename_basename(({const char*_tmp32D=filename;_tag_fat(_tmp32D,sizeof(char),_get_zero_arr_size_char((void*)_tmp32D,1U));}));struct _fat_ptr basename=_tmp1DA;
struct _fat_ptr _tmp1DB=Cyc_Filename_dirname(({const char*_tmp32C=filename;_tag_fat(_tmp32C,sizeof(char),_get_zero_arr_size_char((void*)_tmp32C,1U));}));struct _fat_ptr dirname=_tmp1DB;
struct _fat_ptr _tmp1DC=Cyc_Filename_chop_extension((struct _fat_ptr)basename);struct _fat_ptr choppedname=_tmp1DC;
const char*_tmp1DD=(const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp52A=(struct _fat_ptr)choppedname;Cyc_strconcat(_tmp52A,({const char*_tmp32B=".iA";_tag_fat(_tmp32B,sizeof(char),4U);}));}),sizeof(char),1U));const char*cppinfile=_tmp1DD;
# 1421
const char*_tmp1DE=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== (unsigned)0?({struct Cyc_String_pa_PrintArg_struct _tmp327=({struct Cyc_String_pa_PrintArg_struct _tmp419;_tmp419.tag=0U,_tmp419.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp419;});void*_tmp325[1U];_tmp325[0]=& _tmp327;({struct _fat_ptr _tmp52D=({const char*_tmp326="%s.iB";_tag_fat(_tmp326,sizeof(char),6U);});Cyc_aprintf(_tmp52D,_tag_fat(_tmp325,sizeof(void*),1U));});}):({struct _fat_ptr _tmp52C=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp52C,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32A=({struct Cyc_String_pa_PrintArg_struct _tmp41A;_tmp41A.tag=0U,_tmp41A.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp41A;});void*_tmp328[1U];_tmp328[0]=& _tmp32A;({struct _fat_ptr _tmp52B=({const char*_tmp329="%s.iB";_tag_fat(_tmp329,sizeof(char),6U);});Cyc_aprintf(_tmp52B,_tag_fat(_tmp328,sizeof(void*),1U));});}));}),sizeof(char),1U));const char*macrosfile=_tmp1DE;
# 1426
const char*_tmp1DF=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== (unsigned)0?({struct Cyc_String_pa_PrintArg_struct _tmp321=({struct Cyc_String_pa_PrintArg_struct _tmp417;_tmp417.tag=0U,_tmp417.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp417;});void*_tmp31F[1U];_tmp31F[0]=& _tmp321;({struct _fat_ptr _tmp530=({const char*_tmp320="%s.iC";_tag_fat(_tmp320,sizeof(char),6U);});Cyc_aprintf(_tmp530,_tag_fat(_tmp31F,sizeof(void*),1U));});}):({struct _fat_ptr _tmp52F=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp52F,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp324=({struct Cyc_String_pa_PrintArg_struct _tmp418;_tmp418.tag=0U,_tmp418.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp418;});void*_tmp322[1U];_tmp322[0]=& _tmp324;({struct _fat_ptr _tmp52E=({const char*_tmp323="%s.iC";_tag_fat(_tmp323,sizeof(char),6U);});Cyc_aprintf(_tmp52E,_tag_fat(_tmp322,sizeof(void*),1U));});}));}),sizeof(char),1U));const char*declsfile=_tmp1DF;
# 1431
const char*_tmp1E0=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== (unsigned)0?({struct Cyc_String_pa_PrintArg_struct _tmp31B=({struct Cyc_String_pa_PrintArg_struct _tmp415;_tmp415.tag=0U,_tmp415.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp415;});void*_tmp319[1U];_tmp319[0]=& _tmp31B;({struct _fat_ptr _tmp533=({const char*_tmp31A="%s.iD";_tag_fat(_tmp31A,sizeof(char),6U);});Cyc_aprintf(_tmp533,_tag_fat(_tmp319,sizeof(void*),1U));});}):({struct _fat_ptr _tmp532=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp532,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp416;_tmp416.tag=0U,_tmp416.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp416;});void*_tmp31C[1U];_tmp31C[0]=& _tmp31E;({struct _fat_ptr _tmp531=({const char*_tmp31D="%s.iD";_tag_fat(_tmp31D,sizeof(char),6U);});Cyc_aprintf(_tmp531,_tag_fat(_tmp31C,sizeof(void*),1U));});}));}),sizeof(char),1U));const char*filtereddeclsfile=_tmp1E0;
# 1436
{struct _handler_cons _tmp1E1;_push_handler(& _tmp1E1);{int _tmp1E3=0;if(setjmp(_tmp1E1.handler))_tmp1E3=1;if(!_tmp1E3){
# 1439
if(Cyc_force_directory_prefixes(({const char*_tmp1E4=filename;_tag_fat(_tmp1E4,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E4,1U));}))){
int _tmp1E5=1;_npop_handler(0U);return _tmp1E5;}
# 1444
if((int)Cyc_mode != (int)Cyc_FINISH){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp1E8=({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0U,({struct _fat_ptr _tmp534=(struct _fat_ptr)({const char*_tmp1E9=cppinfile;_tag_fat(_tmp1E9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E9,1U));});_tmp3E6.f1=_tmp534;});_tmp3E6;});void*_tmp1E6[1U];_tmp1E6[0]=& _tmp1E8;({struct _fat_ptr _tmp535=({const char*_tmp1E7="cat >%s <<XXX\n";_tag_fat(_tmp1E7,sizeof(char),15U);});Cyc_prscript(_tmp535,_tag_fat(_tmp1E6,sizeof(void*),1U));});});
{struct Cyc_List_List*_tmp1EA=cpp_insert;struct Cyc_List_List*l=_tmp1EA;for(0;l != 0;l=l->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp1ED=({struct Cyc_String_pa_PrintArg_struct _tmp3E7;_tmp3E7.tag=0U,_tmp3E7.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)l->hd));_tmp3E7;});void*_tmp1EB[1U];_tmp1EB[0]=& _tmp1ED;({struct _fat_ptr _tmp536=({const char*_tmp1EC="%s";_tag_fat(_tmp1EC,sizeof(char),3U);});Cyc_prscript(_tmp536,_tag_fat(_tmp1EB,sizeof(void*),1U));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp1F0=({struct Cyc_String_pa_PrintArg_struct _tmp3E8;_tmp3E8.tag=0U,({struct _fat_ptr _tmp537=(struct _fat_ptr)({const char*_tmp1F1=filename;_tag_fat(_tmp1F1,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F1,1U));});_tmp3E8.f1=_tmp537;});_tmp3E8;});void*_tmp1EE[1U];_tmp1EE[0]=& _tmp1F0;({struct _fat_ptr _tmp538=({const char*_tmp1EF="#include <%s>\n";_tag_fat(_tmp1EF,sizeof(char),15U);});Cyc_prscript(_tmp538,_tag_fat(_tmp1EE,sizeof(void*),1U));});});
({void*_tmp1F2=0U;({struct _fat_ptr _tmp539=({const char*_tmp1F3="XXX\n";_tag_fat(_tmp1F3,sizeof(char),5U);});Cyc_prscript(_tmp539,_tag_fat(_tmp1F2,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1F6=({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0U,_tmp3EB.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3EB;});struct Cyc_String_pa_PrintArg_struct _tmp1F7=({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0U,({struct _fat_ptr _tmp53A=(struct _fat_ptr)({const char*_tmp1FA=macrosfile;_tag_fat(_tmp1FA,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FA,1U));});_tmp3EA.f1=_tmp53A;});_tmp3EA;});struct Cyc_String_pa_PrintArg_struct _tmp1F8=({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0U,({struct _fat_ptr _tmp53B=(struct _fat_ptr)({const char*_tmp1F9=cppinfile;_tag_fat(_tmp1F9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F9,1U));});_tmp3E9.f1=_tmp53B;});_tmp3E9;});void*_tmp1F4[3U];_tmp1F4[0]=& _tmp1F6,_tmp1F4[1]=& _tmp1F7,_tmp1F4[2]=& _tmp1F8;({struct _fat_ptr _tmp53C=({const char*_tmp1F5="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_fat(_tmp1F5,sizeof(char),35U);});Cyc_prscript(_tmp53C,_tag_fat(_tmp1F4,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1FD=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0U,_tmp3EE.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3EE;});struct Cyc_String_pa_PrintArg_struct _tmp1FE=({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0U,({struct _fat_ptr _tmp53D=(struct _fat_ptr)({const char*_tmp201=declsfile;_tag_fat(_tmp201,sizeof(char),_get_zero_arr_size_char((void*)_tmp201,1U));});_tmp3ED.f1=_tmp53D;});_tmp3ED;});struct Cyc_String_pa_PrintArg_struct _tmp1FF=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0U,({struct _fat_ptr _tmp53E=(struct _fat_ptr)({const char*_tmp200=cppinfile;_tag_fat(_tmp200,sizeof(char),_get_zero_arr_size_char((void*)_tmp200,1U));});_tmp3EC.f1=_tmp53E;});_tmp3EC;});void*_tmp1FB[3U];_tmp1FB[0]=& _tmp1FD,_tmp1FB[1]=& _tmp1FE,_tmp1FB[2]=& _tmp1FF;({struct _fat_ptr _tmp53F=({const char*_tmp1FC="$GCC %s -E     -o %s -x c %s;\n";_tag_fat(_tmp1FC,sizeof(char),31U);});Cyc_prscript(_tmp53F,_tag_fat(_tmp1FB,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp204=({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0U,({struct _fat_ptr _tmp540=(struct _fat_ptr)({const char*_tmp205=cppinfile;_tag_fat(_tmp205,sizeof(char),_get_zero_arr_size_char((void*)_tmp205,1U));});_tmp3EF.f1=_tmp540;});_tmp3EF;});void*_tmp202[1U];_tmp202[0]=& _tmp204;({struct _fat_ptr _tmp541=({const char*_tmp203="rm %s\n";_tag_fat(_tmp203,sizeof(char),7U);});Cyc_prscript(_tmp541,_tag_fat(_tmp202,sizeof(void*),1U));});});}else{
# 1456
maybe=Cyc_fopen(cppinfile,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp208=({struct Cyc_String_pa_PrintArg_struct _tmp3F0;_tmp3F0.tag=0U,({struct _fat_ptr _tmp542=(struct _fat_ptr)({const char*_tmp209=cppinfile;_tag_fat(_tmp209,sizeof(char),_get_zero_arr_size_char((void*)_tmp209,1U));});_tmp3F0.f1=_tmp542;});_tmp3F0;});void*_tmp206[1U];_tmp206[0]=& _tmp208;({struct Cyc___cycFILE*_tmp544=Cyc_stderr;struct _fat_ptr _tmp543=({const char*_tmp207="Error: could not create file %s\n";_tag_fat(_tmp207,sizeof(char),33U);});Cyc_fprintf(_tmp544,_tmp543,_tag_fat(_tmp206,sizeof(void*),1U));});});{
int _tmp20A=1;_npop_handler(0U);return _tmp20A;}}
# 1461
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp20D=({struct Cyc_String_pa_PrintArg_struct _tmp3F1;_tmp3F1.tag=0U,({struct _fat_ptr _tmp545=(struct _fat_ptr)({const char*_tmp20E=cppinfile;_tag_fat(_tmp20E,sizeof(char),_get_zero_arr_size_char((void*)_tmp20E,1U));});_tmp3F1.f1=_tmp545;});_tmp3F1;});void*_tmp20B[1U];_tmp20B[0]=& _tmp20D;({struct Cyc___cycFILE*_tmp547=Cyc_stderr;struct _fat_ptr _tmp546=({const char*_tmp20C="Creating %s\n";_tag_fat(_tmp20C,sizeof(char),13U);});Cyc_fprintf(_tmp547,_tmp546,_tag_fat(_tmp20B,sizeof(void*),1U));});});
out_file=maybe;
{struct Cyc_List_List*_tmp20F=cpp_insert;struct Cyc_List_List*l=_tmp20F;for(0;l != 0;l=l->tl){
Cyc_fputs((const char*)_check_null(_untag_fat_ptr(*((struct _fat_ptr*)l->hd),sizeof(char),1U)),out_file);}}
# 1467
({struct Cyc_String_pa_PrintArg_struct _tmp212=({struct Cyc_String_pa_PrintArg_struct _tmp3F2;_tmp3F2.tag=0U,({struct _fat_ptr _tmp548=(struct _fat_ptr)({const char*_tmp213=filename;_tag_fat(_tmp213,sizeof(char),_get_zero_arr_size_char((void*)_tmp213,1U));});_tmp3F2.f1=_tmp548;});_tmp3F2;});void*_tmp210[1U];_tmp210[0]=& _tmp212;({struct Cyc___cycFILE*_tmp54A=out_file;struct _fat_ptr _tmp549=({const char*_tmp211="#include <%s>\n";_tag_fat(_tmp211,sizeof(char),15U);});Cyc_fprintf(_tmp54A,_tmp549,_tag_fat(_tmp210,sizeof(void*),1U));});});
Cyc_fclose(out_file);{
struct _fat_ptr _tmp214=({
struct Cyc_List_List*_tmp54E=({struct Cyc_List_List*_tmp231=_cycalloc(sizeof(*_tmp231));({struct _fat_ptr*_tmp54D=({struct _fat_ptr*_tmp230=_cycalloc(sizeof(*_tmp230));({struct _fat_ptr _tmp54C=(struct _fat_ptr)({const char*_tmp22F="";_tag_fat(_tmp22F,sizeof(char),1U);});*_tmp230=_tmp54C;});_tmp230;});_tmp231->hd=_tmp54D;}),({
struct Cyc_List_List*_tmp54B=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp231->tl=_tmp54B;});_tmp231;});
# 1470
Cyc_str_sepstr(_tmp54E,({const char*_tmp232=" ";_tag_fat(_tmp232,sizeof(char),2U);}));});
# 1469
struct _fat_ptr cppargs_string=_tmp214;
# 1473
char*cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp3FE;_tmp3FE.tag=0U,_tmp3FE.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3FE;});struct Cyc_String_pa_PrintArg_struct _tmp229=({struct Cyc_String_pa_PrintArg_struct _tmp3FD;_tmp3FD.tag=0U,_tmp3FD.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3FD;});struct Cyc_String_pa_PrintArg_struct _tmp22A=({struct Cyc_String_pa_PrintArg_struct _tmp3FC;_tmp3FC.tag=0U,_tmp3FC.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3FC;});struct Cyc_String_pa_PrintArg_struct _tmp22B=({struct Cyc_String_pa_PrintArg_struct _tmp3FB;_tmp3FB.tag=0U,({struct _fat_ptr _tmp54F=(struct _fat_ptr)({const char*_tmp22E=macrosfile;_tag_fat(_tmp22E,sizeof(char),_get_zero_arr_size_char((void*)_tmp22E,1U));});_tmp3FB.f1=_tmp54F;});_tmp3FB;});struct Cyc_String_pa_PrintArg_struct _tmp22C=({struct Cyc_String_pa_PrintArg_struct _tmp3FA;_tmp3FA.tag=0U,({struct _fat_ptr _tmp550=(struct _fat_ptr)({const char*_tmp22D=cppinfile;_tag_fat(_tmp22D,sizeof(char),_get_zero_arr_size_char((void*)_tmp22D,1U));});_tmp3FA.f1=_tmp550;});_tmp3FA;});void*_tmp226[5U];_tmp226[0]=& _tmp228,_tmp226[1]=& _tmp229,_tmp226[2]=& _tmp22A,_tmp226[3]=& _tmp22B,_tmp226[4]=& _tmp22C;({struct _fat_ptr _tmp551=({const char*_tmp227="%s %s %s -E -dM -o %s -x c %s";_tag_fat(_tmp227,sizeof(char),30U);});Cyc_aprintf(_tmp551,_tag_fat(_tmp226,sizeof(void*),5U));});}),sizeof(char),1U));
# 1476
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp217=({struct Cyc_String_pa_PrintArg_struct _tmp3F3;_tmp3F3.tag=0U,({struct _fat_ptr _tmp552=(struct _fat_ptr)({char*_tmp218=cmd;_tag_fat(_tmp218,sizeof(char),_get_zero_arr_size_char((void*)_tmp218,1U));});_tmp3F3.f1=_tmp552;});_tmp3F3;});void*_tmp215[1U];_tmp215[0]=& _tmp217;({struct Cyc___cycFILE*_tmp554=Cyc_stderr;struct _fat_ptr _tmp553=({const char*_tmp216="%s\n";_tag_fat(_tmp216,sizeof(char),4U);});Cyc_fprintf(_tmp554,_tmp553,_tag_fat(_tmp215,sizeof(void*),1U));});});
if(!system((const char*)cmd)){
# 1481
cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp21B=({struct Cyc_String_pa_PrintArg_struct _tmp3F8;_tmp3F8.tag=0U,_tmp3F8.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3F8;});struct Cyc_String_pa_PrintArg_struct _tmp21C=({struct Cyc_String_pa_PrintArg_struct _tmp3F7;_tmp3F7.tag=0U,_tmp3F7.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3F7;});struct Cyc_String_pa_PrintArg_struct _tmp21D=({struct Cyc_String_pa_PrintArg_struct _tmp3F6;_tmp3F6.tag=0U,_tmp3F6.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3F6;});struct Cyc_String_pa_PrintArg_struct _tmp21E=({struct Cyc_String_pa_PrintArg_struct _tmp3F5;_tmp3F5.tag=0U,({struct _fat_ptr _tmp555=(struct _fat_ptr)({const char*_tmp221=declsfile;_tag_fat(_tmp221,sizeof(char),_get_zero_arr_size_char((void*)_tmp221,1U));});_tmp3F5.f1=_tmp555;});_tmp3F5;});struct Cyc_String_pa_PrintArg_struct _tmp21F=({struct Cyc_String_pa_PrintArg_struct _tmp3F4;_tmp3F4.tag=0U,({struct _fat_ptr _tmp556=(struct _fat_ptr)({const char*_tmp220=cppinfile;_tag_fat(_tmp220,sizeof(char),_get_zero_arr_size_char((void*)_tmp220,1U));});_tmp3F4.f1=_tmp556;});_tmp3F4;});void*_tmp219[5U];_tmp219[0]=& _tmp21B,_tmp219[1]=& _tmp21C,_tmp219[2]=& _tmp21D,_tmp219[3]=& _tmp21E,_tmp219[4]=& _tmp21F;({struct _fat_ptr _tmp557=({const char*_tmp21A="%s %s %s -E -o %s -x c %s";_tag_fat(_tmp21A,sizeof(char),26U);});Cyc_aprintf(_tmp557,_tag_fat(_tmp219,sizeof(void*),5U));});}),sizeof(char),1U));
# 1484
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp224=({struct Cyc_String_pa_PrintArg_struct _tmp3F9;_tmp3F9.tag=0U,({struct _fat_ptr _tmp558=(struct _fat_ptr)({char*_tmp225=cmd;_tag_fat(_tmp225,sizeof(char),_get_zero_arr_size_char((void*)_tmp225,1U));});_tmp3F9.f1=_tmp558;});_tmp3F9;});void*_tmp222[1U];_tmp222[0]=& _tmp224;({struct Cyc___cycFILE*_tmp55A=Cyc_stderr;struct _fat_ptr _tmp559=({const char*_tmp223="%s\n";_tag_fat(_tmp223,sizeof(char),4U);});Cyc_fprintf(_tmp55A,_tmp559,_tag_fat(_tmp222,sizeof(void*),1U));});});
system((const char*)cmd);}}}}
# 1491
if(Cyc_gathering()){int _tmp233=0;_npop_handler(0U);return _tmp233;}{
# 1494
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp55D=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp236=({struct Cyc_String_pa_PrintArg_struct _tmp3FF;_tmp3FF.tag=0U,({struct _fat_ptr _tmp55B=(struct _fat_ptr)({const char*_tmp237=macrosfile;_tag_fat(_tmp237,sizeof(char),_get_zero_arr_size_char((void*)_tmp237,1U));});_tmp3FF.f1=_tmp55B;});_tmp3FF;});void*_tmp234[1U];_tmp234[0]=& _tmp236;({struct _fat_ptr _tmp55C=({const char*_tmp235="can't open macrosfile %s";_tag_fat(_tmp235,sizeof(char),25U);});Cyc_aprintf(_tmp55C,_tag_fat(_tmp234,sizeof(void*),1U));});});_tmp238->f1=_tmp55D;});_tmp238;}));
# 1499
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp239=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp239;
struct _tuple13*entry;
while((entry=((struct _tuple13*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(l))!= 0){
struct _tuple13*_tmp23A=(struct _tuple13*)_check_null(entry);struct _tuple13*_stmttmpF=_tmp23A;struct _tuple13*_tmp23B=_stmttmpF;struct Cyc_Set_Set*_tmp23D;struct _fat_ptr*_tmp23C;_LL1: _tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;_LL2: {struct _fat_ptr*name=_tmp23C;struct Cyc_Set_Set*uses=_tmp23D;
((void(*)(struct Cyc_Hashtable_Table*t,struct _fat_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,name,uses);}}
# 1508
Cyc_fclose(in_file);
# 1511
maybe=Cyc_fopen(declsfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp242=_cycalloc(sizeof(*_tmp242));_tmp242->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp560=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp240=({struct Cyc_String_pa_PrintArg_struct _tmp400;_tmp400.tag=0U,({struct _fat_ptr _tmp55E=(struct _fat_ptr)({const char*_tmp241=declsfile;_tag_fat(_tmp241,sizeof(char),_get_zero_arr_size_char((void*)_tmp241,1U));});_tmp400.f1=_tmp55E;});_tmp400;});void*_tmp23E[1U];_tmp23E[0]=& _tmp240;({struct _fat_ptr _tmp55F=({const char*_tmp23F="can't open declsfile %s";_tag_fat(_tmp23F,sizeof(char),24U);});Cyc_aprintf(_tmp55F,_tag_fat(_tmp23E,sizeof(void*),1U));});});_tmp242->f1=_tmp560;});_tmp242;}));
# 1515
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(filtereddeclsfile,"w");
if(!((unsigned)Cyc_slurp_out)){int _tmp243=1;_npop_handler(0U);return _tmp243;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(l)){;}{
# 1521
struct Cyc_List_List*_tmp244=user_defs;struct Cyc_List_List*x=_tmp244;
while(x != 0){
struct _tuple16*_tmp245=(struct _tuple16*)x->hd;struct _tuple16*_stmttmp10=_tmp245;struct _tuple16*_tmp246=_stmttmp10;struct _fat_ptr*_tmp247;_LL4: _tmp247=_tmp246->f2;_LL5: {struct _fat_ptr*s=_tmp247;
({struct Cyc_String_pa_PrintArg_struct _tmp24A=({struct Cyc_String_pa_PrintArg_struct _tmp401;_tmp401.tag=0U,_tmp401.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp401;});void*_tmp248[1U];_tmp248[0]=& _tmp24A;({struct Cyc___cycFILE*_tmp562=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp561=({const char*_tmp249="%s";_tag_fat(_tmp249,sizeof(char),3U);});Cyc_fprintf(_tmp562,_tmp561,_tag_fat(_tmp248,sizeof(void*),1U));});});
x=x->tl;}}
# 1527
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if((int)Cyc_mode != (int)Cyc_FINISH)
;
# 1533
maybe=Cyc_fopen(filtereddeclsfile,"r");
if(!((unsigned)maybe)){int _tmp24B=1;_npop_handler(0U);return _tmp24B;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp24C=filtereddeclsfile;_tag_fat(_tmp24C,sizeof(char),_get_zero_arr_size_char((void*)_tmp24C,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp24D=Cyc_Parse_parse_file(in_file);struct Cyc_List_List*decls=_tmp24D;
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1543
{struct Cyc_List_List*_tmp24E=decls;struct Cyc_List_List*d=_tmp24E;for(0;d != 0;d=d->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)d->hd,t);}}{
# 1547
struct Cyc_List_List*_tmp24F=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);struct Cyc_List_List*user_symbols=_tmp24F;
struct Cyc_Set_Set*_tmp250=({struct Cyc_List_List*_tmp563=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,user_symbols);Cyc_reachable(_tmp563,t);});struct Cyc_Set_Set*reachable_set=_tmp250;
# 1551
struct Cyc_List_List*_tmp251=0;struct Cyc_List_List*reachable_decls=_tmp251;
struct Cyc_List_List*_tmp252=0;struct Cyc_List_List*user_decls=_tmp252;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp253=decls;struct Cyc_List_List*d=_tmp253;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp254=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp254;
struct _fat_ptr*name;
{void*_tmp255=decl->r;void*_stmttmp11=_tmp255;void*_tmp256=_stmttmp11;struct Cyc_Absyn_Typedefdecl*_tmp257;struct Cyc_Absyn_Enumdecl*_tmp258;struct Cyc_Absyn_Aggrdecl*_tmp259;struct Cyc_Absyn_Fndecl*_tmp25A;struct Cyc_Absyn_Vardecl*_tmp25B;switch(*((int*)_tmp256)){case 0U: _LL7: _tmp25B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp256)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp25B;
# 1559
struct _tuple1*_tmp25C=x->name;struct _tuple1*_stmttmp12=_tmp25C;struct _tuple1*_tmp25D=_stmttmp12;struct _fat_ptr*_tmp25E;_LL2C: _tmp25E=_tmp25D->f2;_LL2D: {struct _fat_ptr*v=_tmp25E;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(defined_symbols,v);
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 1U: _LL9: _tmp25A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp256)->f1;_LLA: {struct Cyc_Absyn_Fndecl*x=_tmp25A;
# 1565
struct _tuple1*_tmp25F=x->name;struct _tuple1*_stmttmp13=_tmp25F;struct _tuple1*_tmp260=_stmttmp13;struct _fat_ptr*_tmp261;_LL2F: _tmp261=_tmp260->f2;_LL30: {struct _fat_ptr*v=_tmp261;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(defined_symbols,v);
if(((int(*)(int(*compare)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*l,struct _fat_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 6U: _LLB: _tmp259=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp256)->f1;_LLC: {struct Cyc_Absyn_Aggrdecl*x=_tmp259;
# 1571
struct _tuple1*_tmp262=x->name;struct _tuple1*_stmttmp14=_tmp262;struct _tuple1*_tmp263=_stmttmp14;struct _fat_ptr*_tmp264;_LL32: _tmp264=_tmp263->f2;_LL33: {struct _fat_ptr*v=_tmp264;
name=v;
goto _LL6;}}case 8U: _LLD: _tmp258=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp256)->f1;_LLE: {struct Cyc_Absyn_Enumdecl*x=_tmp258;
# 1575
struct _tuple1*_tmp265=x->name;struct _tuple1*_stmttmp15=_tmp265;struct _tuple1*_tmp266=_stmttmp15;struct _fat_ptr*_tmp267;_LL35: _tmp267=_tmp266->f2;_LL36: {struct _fat_ptr*v=_tmp267;
name=v;
# 1579
if(name != 0 &&((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,name))
reachable_decls=({struct Cyc_List_List*_tmp268=_cycalloc(sizeof(*_tmp268));_tmp268->hd=decl,_tmp268->tl=reachable_decls;_tmp268;});else{
# 1582
if((unsigned)x->fields){
struct Cyc_List_List*_tmp269=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp269;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp26A=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp26A;
struct _tuple1*_tmp26B=f->name;struct _tuple1*_stmttmp16=_tmp26B;struct _tuple1*_tmp26C=_stmttmp16;struct _fat_ptr*_tmp26D;_LL38: _tmp26D=_tmp26C->f2;_LL39: {struct _fat_ptr*v=_tmp26D;
if(((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,v)){
reachable_decls=({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->hd=decl,_tmp26E->tl=reachable_decls;_tmp26E;});
break;}}}}}
# 1592
name=0;
goto _LL6;}}case 9U: _LLF: _tmp257=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp256)->f1;_LL10: {struct Cyc_Absyn_Typedefdecl*x=_tmp257;
# 1595
struct _tuple1*_tmp26F=x->name;struct _tuple1*_stmttmp17=_tmp26F;struct _tuple1*_tmp270=_stmttmp17;struct _fat_ptr*_tmp271;_LL3B: _tmp271=_tmp270->f2;_LL3C: {struct _fat_ptr*v=_tmp271;
name=v;
goto _LL6;}}case 14U: _LL11: _LL12:
 goto _LL14;case 15U: _LL13: _LL14:
 goto _LL16;case 16U: _LL15: _LL16:
 goto _LL18;case 17U: _LL17: _LL18:
 goto _LL1A;case 2U: _LL19: _LL1A:
 goto _LL1C;case 7U: _LL1B: _LL1C:
 goto _LL1E;case 3U: _LL1D: _LL1E:
 goto _LL20;case 10U: _LL1F: _LL20:
 goto _LL22;case 11U: _LL21: _LL22:
 goto _LL24;case 12U: _LL23: _LL24:
 goto _LL26;case 13U: _LL25: _LL26:
 goto _LL28;case 4U: _LL27: _LL28:
 goto _LL2A;default: _LL29: _LL2A:
# 1611
 name=0;
goto _LL6;}_LL6:;}
# 1615
if(name != 0 &&((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,name)){
if(({struct _fat_ptr _tmp565=(struct _fat_ptr)*name;struct _fat_ptr _tmp564=(struct _fat_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp565,_tmp564,Cyc_strlen((struct _fat_ptr)Cyc_user_prefix));})!= 0)
reachable_decls=({struct Cyc_List_List*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->hd=decl,_tmp272->tl=reachable_decls;_tmp272;});else{
# 1620
Cyc_rename_decl(decl);
user_decls=({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=decl,_tmp273->tl=user_decls;_tmp273;});}}}}
# 1627
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){int _tmp274=1;_npop_handler(0U);return _tmp274;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _fat_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2F1=({struct Cyc_String_pa_PrintArg_struct _tmp40D;_tmp40D.tag=0U,({struct _fat_ptr _tmp566=(struct _fat_ptr)({const char*_tmp2F2=filename;_tag_fat(_tmp2F2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F2,1U));});_tmp40D.f1=_tmp566;});_tmp40D;});void*_tmp2EF[1U];_tmp2EF[0]=& _tmp2F1;({struct _fat_ptr _tmp567=({const char*_tmp2F0="_%s_";_tag_fat(_tmp2F0,sizeof(char),5U);});Cyc_aprintf(_tmp567,_tag_fat(_tmp2EF,sizeof(void*),1U));});});
{int _tmp275=0;int j=_tmp275;for(0;(unsigned)j < _get_fat_size(ifdefmacro,sizeof(char));++ j){
if((int)((char*)ifdefmacro.curr)[j]== (int)'.' ||(int)((char*)ifdefmacro.curr)[j]== (int)'/')
({struct _fat_ptr _tmp276=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp277=*((char*)_check_fat_subscript(_tmp276,sizeof(char),0U));char _tmp278='_';if(_get_fat_size(_tmp276,sizeof(char))== 1U &&(_tmp277 == 0 && _tmp278 != 0))_throw_arraybounds();*((char*)_tmp276.curr)=_tmp278;});else{
if((int)((char*)ifdefmacro.curr)[j]!= (int)'_' &&(int)((char*)ifdefmacro.curr)[j]!= (int)'/')
({struct _fat_ptr _tmp279=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp27A=*((char*)_check_fat_subscript(_tmp279,sizeof(char),0U));char _tmp27B=(char)toupper((int)((char*)ifdefmacro.curr)[j]);if(_get_fat_size(_tmp279,sizeof(char))== 1U &&(_tmp27A == 0 && _tmp27B != 0))_throw_arraybounds();*((char*)_tmp279.curr)=_tmp27B;});}}}
# 1639
({struct Cyc_String_pa_PrintArg_struct _tmp27E=({struct Cyc_String_pa_PrintArg_struct _tmp403;_tmp403.tag=0U,_tmp403.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp403;});struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp402;_tmp402.tag=0U,_tmp402.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp402;});void*_tmp27C[2U];_tmp27C[0]=& _tmp27E,_tmp27C[1]=& _tmp27F;({struct Cyc___cycFILE*_tmp569=out_file;struct _fat_ptr _tmp568=({const char*_tmp27D="#ifndef %s\n#define %s\n";_tag_fat(_tmp27D,sizeof(char),23U);});Cyc_fprintf(_tmp569,_tmp568,_tag_fat(_tmp27C,sizeof(void*),2U));});});{
# 1646
struct Cyc_List_List*_tmp280=0;struct Cyc_List_List*print_decls=_tmp280;
struct Cyc_List_List*_tmp281=0;struct Cyc_List_List*names=_tmp281;
{struct Cyc_List_List*_tmp282=reachable_decls;struct Cyc_List_List*d=_tmp282;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp283=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp283;
int _tmp284=0;int anon_enum=_tmp284;
struct _fat_ptr*name;
{void*_tmp285=decl->r;void*_stmttmp18=_tmp285;void*_tmp286=_stmttmp18;struct Cyc_Absyn_Typedefdecl*_tmp287;struct Cyc_Absyn_Enumdecl*_tmp288;struct Cyc_Absyn_Aggrdecl*_tmp289;struct Cyc_Absyn_Fndecl*_tmp28A;struct Cyc_Absyn_Vardecl*_tmp28B;switch(*((int*)_tmp286)){case 0U: _LL3E: _tmp28B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp286)->f1;_LL3F: {struct Cyc_Absyn_Vardecl*x=_tmp28B;
# 1654
struct _tuple1*_tmp28C=x->name;struct _tuple1*_stmttmp19=_tmp28C;struct _tuple1*_tmp28D=_stmttmp19;struct _fat_ptr*_tmp28E;_LL63: _tmp28E=_tmp28D->f2;_LL64: {struct _fat_ptr*v=_tmp28E;
name=v;
goto _LL3D;}}case 1U: _LL40: _tmp28A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp286)->f1;_LL41: {struct Cyc_Absyn_Fndecl*x=_tmp28A;
# 1658
if(x->is_inline){name=0;goto _LL3D;}{
struct _tuple1*_tmp28F=x->name;struct _tuple1*_stmttmp1A=_tmp28F;struct _tuple1*_tmp290=_stmttmp1A;struct _fat_ptr*_tmp291;_LL66: _tmp291=_tmp290->f2;_LL67: {struct _fat_ptr*v=_tmp291;
name=v;
goto _LL3D;}}}case 6U: _LL42: _tmp289=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp286)->f1;_LL43: {struct Cyc_Absyn_Aggrdecl*x=_tmp289;
# 1663
struct _tuple1*_tmp292=x->name;struct _tuple1*_stmttmp1B=_tmp292;struct _tuple1*_tmp293=_stmttmp1B;struct _fat_ptr*_tmp294;_LL69: _tmp294=_tmp293->f2;_LL6A: {struct _fat_ptr*v=_tmp294;
name=v;
goto _LL3D;}}case 8U: _LL44: _tmp288=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp286)->f1;_LL45: {struct Cyc_Absyn_Enumdecl*x=_tmp288;
# 1667
struct _tuple1*_tmp295=x->name;struct _tuple1*_stmttmp1C=_tmp295;struct _tuple1*_tmp296=_stmttmp1C;struct _fat_ptr*_tmp297;_LL6C: _tmp297=_tmp296->f2;_LL6D: {struct _fat_ptr*v=_tmp297;
name=v;
goto _LL3D;}}case 9U: _LL46: _tmp287=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp286)->f1;_LL47: {struct Cyc_Absyn_Typedefdecl*x=_tmp287;
# 1671
struct _tuple1*_tmp298=x->name;struct _tuple1*_stmttmp1D=_tmp298;struct _tuple1*_tmp299=_stmttmp1D;struct _fat_ptr*_tmp29A;_LL6F: _tmp29A=_tmp299->f2;_LL70: {struct _fat_ptr*v=_tmp29A;
name=v;
goto _LL3D;}}case 4U: _LL48: _LL49:
 goto _LL4B;case 5U: _LL4A: _LL4B:
 goto _LL4D;case 14U: _LL4C: _LL4D:
 goto _LL4F;case 15U: _LL4E: _LL4F:
 goto _LL51;case 16U: _LL50: _LL51:
 goto _LL53;case 17U: _LL52: _LL53:
 goto _LL55;case 2U: _LL54: _LL55:
 goto _LL57;case 7U: _LL56: _LL57:
 goto _LL59;case 3U: _LL58: _LL59:
 goto _LL5B;case 10U: _LL5A: _LL5B:
 goto _LL5D;case 11U: _LL5C: _LL5D:
 goto _LL5F;case 12U: _LL5E: _LL5F:
 goto _LL61;default: _LL60: _LL61:
# 1687
 name=0;
goto _LL3D;}_LL3D:;}
# 1690
if(!((unsigned)name)&& !anon_enum)continue;
# 1695
if(({struct Cyc_Set_Set*_tmp56A=reachable_set;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp56A,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp29B=Cyc_Absyn_lookup_decl(user_decls,name);struct Cyc_Absyn_Decl*user_decl=_tmp29B;
if(user_decl == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp56B=({const char*_tmp29C="Internal Error: bad user-def name";_tag_fat(_tmp29C,sizeof(char),34U);});_tmp29D->f1=_tmp56B;});_tmp29D;}));else{
# 1701
void*_tmp29E=user_decl->r;void*_stmttmp1E=_tmp29E;void*_tmp29F=_stmttmp1E;switch(*((int*)_tmp29F)){case 0U: _LL72: _LL73:
 goto _LL75;case 1U: _LL74: _LL75:
# 1704
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp56C=({const char*_tmp2A0="user defintions for function or variable decls";_tag_fat(_tmp2A0,sizeof(char),47U);});_tmp2A1->f1=_tmp56C;});_tmp2A1;}));default: _LL76: _LL77:
 goto _LL71;}_LL71:;}
# 1710
print_decls=({struct Cyc_List_List*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->hd=decl,_tmp2A2->tl=print_decls;_tmp2A2;});}else{
# 1713
print_decls=({struct Cyc_List_List*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3->hd=decl,_tmp2A3->tl=print_decls;_tmp2A3;});}
names=({struct Cyc_List_List*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4->hd=name,_tmp2A4->tl=names;_tmp2A4;});}}
# 1718
{struct _handler_cons _tmp2A5;_push_handler(& _tmp2A5);{int _tmp2A7=0;if(setjmp(_tmp2A5.handler))_tmp2A7=1;if(!_tmp2A7){
Cyc_Binding_resolve_all(print_decls);
({struct Cyc_Tcenv_Tenv*_tmp56D=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp56D,1,print_decls);});
# 1719
;_pop_handler();}else{void*_tmp2A6=(void*)Cyc_Core_get_exn_thrown();void*_tmp2A8=_tmp2A6;_LL79: _LL7A:
# 1722
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2AA=_cycalloc(sizeof(*_tmp2AA));_tmp2AA->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp56E=({const char*_tmp2A9="can't typecheck acquired declarations";_tag_fat(_tmp2A9,sizeof(char),38U);});_tmp2AA->f1=_tmp56E;});_tmp2AA;}));_LL78:;}}}
# 1726
{struct _tuple0 _tmp2AB=({struct _tuple0 _tmp407;_tmp407.f1=print_decls,_tmp407.f2=names;_tmp407;});struct _tuple0 _stmttmp1F=_tmp2AB;struct _tuple0 _tmp2AC=_stmttmp1F;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2AD;_LL7C: _tmp2AD=_tmp2AC.f1;_tmp2AE=_tmp2AC.f2;_LL7D: {struct Cyc_List_List*d=_tmp2AD;struct Cyc_List_List*n=_tmp2AE;for(0;
d != 0 && n != 0;(d=d->tl,n=n->tl)){
struct Cyc_Absyn_Decl*_tmp2AF=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp2AF;
struct _fat_ptr*_tmp2B0=(struct _fat_ptr*)n->hd;struct _fat_ptr*name=_tmp2B0;
int _tmp2B1=0;int anon_enum=_tmp2B1;
if(!((unsigned)name))
anon_enum=1;
# 1735
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned)name){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2B4=({struct Cyc_String_pa_PrintArg_struct _tmp404;_tmp404.tag=0U,_tmp404.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp404;});void*_tmp2B2[1U];_tmp2B2[0]=& _tmp2B4;({struct _fat_ptr _tmp56F=({const char*_tmp2B3="_%s_def_";_tag_fat(_tmp2B3,sizeof(char),9U);});Cyc_aprintf(_tmp56F,_tag_fat(_tmp2B2,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2B7=({struct Cyc_String_pa_PrintArg_struct _tmp405;_tmp405.tag=0U,_tmp405.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp405;});void*_tmp2B5[1U];_tmp2B5[0]=& _tmp2B7;({struct Cyc___cycFILE*_tmp571=out_file;struct _fat_ptr _tmp570=({const char*_tmp2B6="#ifndef %s\n";_tag_fat(_tmp2B6,sizeof(char),12U);});Cyc_fprintf(_tmp571,_tmp570,_tag_fat(_tmp2B5,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2BA=({struct Cyc_String_pa_PrintArg_struct _tmp406;_tmp406.tag=0U,_tmp406.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp406;});void*_tmp2B8[1U];_tmp2B8[0]=& _tmp2BA;({struct Cyc___cycFILE*_tmp573=out_file;struct _fat_ptr _tmp572=({const char*_tmp2B9="#define %s\n";_tag_fat(_tmp2B9,sizeof(char),12U);});Cyc_fprintf(_tmp573,_tmp572,_tag_fat(_tmp2B8,sizeof(void*),1U));});});
# 1741
({struct Cyc_List_List*_tmp574=({struct Cyc_Absyn_Decl*_tmp2BB[1U];_tmp2BB[0]=decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2BB,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp574,out_file);});
({void*_tmp2BC=0U;({struct Cyc___cycFILE*_tmp576=out_file;struct _fat_ptr _tmp575=({const char*_tmp2BD="#endif\n";_tag_fat(_tmp2BD,sizeof(char),8U);});Cyc_fprintf(_tmp576,_tmp575,_tag_fat(_tmp2BC,sizeof(void*),0U));});});}else{
# 1746
({struct Cyc_List_List*_tmp577=({struct Cyc_Absyn_Decl*_tmp2BE[1U];_tmp2BE[0]=decl;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp2BE,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp577,out_file);});}}}}
# 1751
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));_tmp2C3->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp57A=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C1=({struct Cyc_String_pa_PrintArg_struct _tmp408;_tmp408.tag=0U,({struct _fat_ptr _tmp578=(struct _fat_ptr)({const char*_tmp2C2=macrosfile;_tag_fat(_tmp2C2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C2,1U));});_tmp408.f1=_tmp578;});_tmp408;});void*_tmp2BF[1U];_tmp2BF[0]=& _tmp2C1;({struct _fat_ptr _tmp579=({const char*_tmp2C0="can't open macrosfile %s";_tag_fat(_tmp2C0,sizeof(char),25U);});Cyc_aprintf(_tmp579,_tag_fat(_tmp2BF,sizeof(void*),1U));});});_tmp2C3->f1=_tmp57A;});_tmp2C3;}));
# 1754
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);{
struct _tuple14*entry2;
while((entry2=((struct _tuple14*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(l))!= 0){
struct _tuple14*_tmp2C4=(struct _tuple14*)_check_null(entry2);struct _tuple14*_stmttmp20=_tmp2C4;struct _tuple14*_tmp2C5=_stmttmp20;struct _fat_ptr*_tmp2C7;struct _fat_ptr _tmp2C6;_LL7F: _tmp2C6=_tmp2C5->f1;_tmp2C7=_tmp2C5->f2;_LL80: {struct _fat_ptr line=_tmp2C6;struct _fat_ptr*name=_tmp2C7;
if(((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(reachable_set,name)){
({struct Cyc_String_pa_PrintArg_struct _tmp2CA=({struct Cyc_String_pa_PrintArg_struct _tmp409;_tmp409.tag=0U,_tmp409.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp409;});void*_tmp2C8[1U];_tmp2C8[0]=& _tmp2CA;({struct Cyc___cycFILE*_tmp57C=out_file;struct _fat_ptr _tmp57B=({const char*_tmp2C9="#ifndef %s\n";_tag_fat(_tmp2C9,sizeof(char),12U);});Cyc_fprintf(_tmp57C,_tmp57B,_tag_fat(_tmp2C8,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2CD=({struct Cyc_String_pa_PrintArg_struct _tmp40A;_tmp40A.tag=0U,_tmp40A.f1=(struct _fat_ptr)((struct _fat_ptr)line);_tmp40A;});void*_tmp2CB[1U];_tmp2CB[0]=& _tmp2CD;({struct Cyc___cycFILE*_tmp57E=out_file;struct _fat_ptr _tmp57D=({const char*_tmp2CC="%s\n";_tag_fat(_tmp2CC,sizeof(char),4U);});Cyc_fprintf(_tmp57E,_tmp57D,_tag_fat(_tmp2CB,sizeof(void*),1U));});});
({void*_tmp2CE=0U;({struct Cyc___cycFILE*_tmp580=out_file;struct _fat_ptr _tmp57F=({const char*_tmp2CF="#endif\n";_tag_fat(_tmp2CF,sizeof(char),8U);});Cyc_fprintf(_tmp580,_tmp57F,_tag_fat(_tmp2CE,sizeof(void*),0U));});});}}}
# 1765
Cyc_fclose(in_file);
if((int)Cyc_mode != (int)Cyc_FINISH);
# 1768
if(hstubs != 0){
struct Cyc_List_List*_tmp2D0=hstubs;struct Cyc_List_List*x=_tmp2D0;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp2D1=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp21=_tmp2D1;struct _tuple15*_tmp2D2=_stmttmp21;struct _fat_ptr _tmp2D4;struct _fat_ptr _tmp2D3;_LL82: _tmp2D3=_tmp2D2->f1;_tmp2D4=_tmp2D2->f2;_LL83: {struct _fat_ptr symbol=_tmp2D3;struct _fat_ptr text=_tmp2D4;
if(({char*_tmp583=(char*)text.curr;_tmp583 != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp582=(char*)symbol.curr;_tmp582 == (char*)(_tag_fat(0,0,0)).curr;})||({struct Cyc_Set_Set*_tmp581=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp581,({struct _fat_ptr*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));*_tmp2D5=symbol;_tmp2D5;}));})))
# 1774
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);else{
# 1776
({struct Cyc_String_pa_PrintArg_struct _tmp2D8=({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0U,_tmp40B.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp40B;});void*_tmp2D6[1U];_tmp2D6[0]=& _tmp2D8;({struct _fat_ptr _tmp584=({const char*_tmp2D7="%s is not supported on this platform\n";_tag_fat(_tmp2D7,sizeof(char),38U);});Cyc_log(_tmp584,_tag_fat(_tmp2D6,sizeof(void*),1U));});});}}}}
# 1779
({void*_tmp2D9=0U;({struct Cyc___cycFILE*_tmp586=out_file;struct _fat_ptr _tmp585=({const char*_tmp2DA="#endif\n";_tag_fat(_tmp2DA,sizeof(char),8U);});Cyc_fprintf(_tmp586,_tmp585,_tag_fat(_tmp2D9,sizeof(void*),0U));});});
if(Cyc_do_setjmp){int _tmp2DB=0;_npop_handler(0U);return _tmp2DB;}else{
Cyc_fclose(out_file);}
# 1784
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2DC=cstubs;struct Cyc_List_List*x=_tmp2DC;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp2DD=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp22=_tmp2DD;struct _tuple15*_tmp2DE=_stmttmp22;struct _fat_ptr _tmp2E0;struct _fat_ptr _tmp2DF;_LL85: _tmp2DF=_tmp2DE->f1;_tmp2E0=_tmp2DE->f2;_LL86: {struct _fat_ptr symbol=_tmp2DF;struct _fat_ptr text=_tmp2E0;
if(({char*_tmp589=(char*)text.curr;_tmp589 != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp588=(char*)symbol.curr;_tmp588 == (char*)(_tag_fat(0,0,0)).curr;})||({struct Cyc_Set_Set*_tmp587=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp587,({struct _fat_ptr*_tmp2E1=_cycalloc(sizeof(*_tmp2E1));*_tmp2E1=symbol;_tmp2E1;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}}
# 1795
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
if(cycstubs != 0){
# 1800
({struct Cyc_String_pa_PrintArg_struct _tmp2E4=({struct Cyc_String_pa_PrintArg_struct _tmp40C;_tmp40C.tag=0U,({struct _fat_ptr _tmp58A=(struct _fat_ptr)({const char*_tmp2E5=filename;_tag_fat(_tmp2E5,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E5,1U));});_tmp40C.f1=_tmp58A;});_tmp40C;});void*_tmp2E2[1U];_tmp2E2[0]=& _tmp2E4;({struct Cyc___cycFILE*_tmp58C=out_file;struct _fat_ptr _tmp58B=({const char*_tmp2E3="#include <%s>\n\n";_tag_fat(_tmp2E3,sizeof(char),16U);});Cyc_fprintf(_tmp58C,_tmp58B,_tag_fat(_tmp2E2,sizeof(void*),1U));});});
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2E6=cycstubs;struct Cyc_List_List*x=_tmp2E6;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp2E7=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp23=_tmp2E7;struct _tuple15*_tmp2E8=_stmttmp23;struct _fat_ptr _tmp2EA;struct _fat_ptr _tmp2E9;_LL88: _tmp2E9=_tmp2E8->f1;_tmp2EA=_tmp2E8->f2;_LL89: {struct _fat_ptr symbol=_tmp2E9;struct _fat_ptr text=_tmp2EA;
if(({char*_tmp58F=(char*)text.curr;_tmp58F != (char*)(_tag_fat(0,0,0)).curr;})&&(
({char*_tmp58E=(char*)symbol.curr;_tmp58E == (char*)(_tag_fat(0,0,0)).curr;})||({struct Cyc_Set_Set*_tmp58D=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmp58D,({struct _fat_ptr*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));*_tmp2EB=symbol;_tmp2EB;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}
# 1808
({void*_tmp2EC=0U;({struct Cyc___cycFILE*_tmp591=out_file;struct _fat_ptr _tmp590=({const char*_tmp2ED="\n";_tag_fat(_tmp2ED,sizeof(char),2U);});Cyc_fprintf(_tmp591,_tmp590,_tag_fat(_tmp2EC,sizeof(void*),0U));});});}{
# 1811
int _tmp2EE=0;_npop_handler(0U);return _tmp2EE;}}}}}}}}}
# 1439
;_pop_handler();}else{void*_tmp1E2=(void*)Cyc_Core_get_exn_thrown();void*_tmp2F3=_tmp1E2;void*_tmp2F4;struct _fat_ptr _tmp2F5;struct _fat_ptr _tmp2F6;struct _fat_ptr _tmp2F7;struct _fat_ptr _tmp2F8;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2F3)->tag == Cyc_Core_Impossible){_LL8B: _tmp2F8=((struct Cyc_Core_Impossible_exn_struct*)_tmp2F3)->f1;_LL8C: {struct _fat_ptr s=_tmp2F8;
# 1815
({struct Cyc_String_pa_PrintArg_struct _tmp2FB=({struct Cyc_String_pa_PrintArg_struct _tmp40E;_tmp40E.tag=0U,_tmp40E.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp40E;});void*_tmp2F9[1U];_tmp2F9[0]=& _tmp2FB;({struct Cyc___cycFILE*_tmp593=Cyc_stderr;struct _fat_ptr _tmp592=({const char*_tmp2FA="Got Core::Impossible(%s)\n";_tag_fat(_tmp2FA,sizeof(char),26U);});Cyc_fprintf(_tmp593,_tmp592,_tag_fat(_tmp2F9,sizeof(void*),1U));});});goto _LL8A;}}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2F3)->tag == Cyc_Dict_Absent){_LL8D: _LL8E:
# 1817
({void*_tmp2FC=0U;({struct Cyc___cycFILE*_tmp595=Cyc_stderr;struct _fat_ptr _tmp594=({const char*_tmp2FD="Got Dict::Absent\n";_tag_fat(_tmp2FD,sizeof(char),18U);});Cyc_fprintf(_tmp595,_tmp594,_tag_fat(_tmp2FC,sizeof(void*),0U));});});goto _LL8A;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2F3)->tag == Cyc_Core_Failure){_LL8F: _tmp2F7=((struct Cyc_Core_Failure_exn_struct*)_tmp2F3)->f1;_LL90: {struct _fat_ptr s=_tmp2F7;
# 1819
({struct Cyc_String_pa_PrintArg_struct _tmp300=({struct Cyc_String_pa_PrintArg_struct _tmp40F;_tmp40F.tag=0U,_tmp40F.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp40F;});void*_tmp2FE[1U];_tmp2FE[0]=& _tmp300;({struct Cyc___cycFILE*_tmp597=Cyc_stderr;struct _fat_ptr _tmp596=({const char*_tmp2FF="Got Core::Failure(%s)\n";_tag_fat(_tmp2FF,sizeof(char),23U);});Cyc_fprintf(_tmp597,_tmp596,_tag_fat(_tmp2FE,sizeof(void*),1U));});});goto _LL8A;}}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2F3)->tag == Cyc_Core_Invalid_argument){_LL91: _tmp2F6=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2F3)->f1;_LL92: {struct _fat_ptr s=_tmp2F6;
# 1821
({struct Cyc_String_pa_PrintArg_struct _tmp303=({struct Cyc_String_pa_PrintArg_struct _tmp410;_tmp410.tag=0U,_tmp410.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp410;});void*_tmp301[1U];_tmp301[0]=& _tmp303;({struct Cyc___cycFILE*_tmp599=Cyc_stderr;struct _fat_ptr _tmp598=({const char*_tmp302="Got Invalid_argument(%s)\n";_tag_fat(_tmp302,sizeof(char),26U);});Cyc_fprintf(_tmp599,_tmp598,_tag_fat(_tmp301,sizeof(void*),1U));});});goto _LL8A;}}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2F3)->tag == Cyc_Core_Not_found){_LL93: _LL94:
# 1823
({void*_tmp304=0U;({struct Cyc___cycFILE*_tmp59B=Cyc_stderr;struct _fat_ptr _tmp59A=({const char*_tmp305="Got Not_found\n";_tag_fat(_tmp305,sizeof(char),15U);});Cyc_fprintf(_tmp59B,_tmp59A,_tag_fat(_tmp304,sizeof(void*),0U));});});goto _LL8A;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2F3)->tag == Cyc_NO_SUPPORT){_LL95: _tmp2F5=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2F3)->f1;_LL96: {struct _fat_ptr s=_tmp2F5;
# 1825
({struct Cyc_String_pa_PrintArg_struct _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp411;_tmp411.tag=0U,_tmp411.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp411;});void*_tmp306[1U];_tmp306[0]=& _tmp308;({struct Cyc___cycFILE*_tmp59D=Cyc_stderr;struct _fat_ptr _tmp59C=({const char*_tmp307="No support because %s\n";_tag_fat(_tmp307,sizeof(char),23U);});Cyc_fprintf(_tmp59D,_tmp59C,_tag_fat(_tmp306,sizeof(void*),1U));});});goto _LL8A;}}else{_LL97: _tmp2F4=_tmp2F3;_LL98: {void*x=_tmp2F4;
# 1827
({void*_tmp309=0U;({struct Cyc___cycFILE*_tmp59F=Cyc_stderr;struct _fat_ptr _tmp59E=({const char*_tmp30A="Got unknown exception\n";_tag_fat(_tmp30A,sizeof(char),23U);});Cyc_fprintf(_tmp59F,_tmp59E,_tag_fat(_tmp309,sizeof(void*),0U));});});
Cyc_Core_rethrow(x);}}}}}}}_LL8A:;}}}
# 1833
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp30D=({struct Cyc_String_pa_PrintArg_struct _tmp412;_tmp412.tag=0U,({struct _fat_ptr _tmp5A0=(struct _fat_ptr)({const char*_tmp30E=filename;_tag_fat(_tmp30E,sizeof(char),_get_zero_arr_size_char((void*)_tmp30E,1U));});_tmp412.f1=_tmp5A0;});_tmp412;});void*_tmp30B[1U];_tmp30B[0]=& _tmp30D;({struct Cyc___cycFILE*_tmp5A2=Cyc_stderr;struct _fat_ptr _tmp5A1=({const char*_tmp30C="Error: could not create file %s\n";_tag_fat(_tmp30C,sizeof(char),33U);});Cyc_fprintf(_tmp5A2,_tmp5A1,_tag_fat(_tmp30B,sizeof(void*),1U));});});
return 1;}
# 1838
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp311=({struct Cyc_String_pa_PrintArg_struct _tmp413;_tmp413.tag=0U,({
# 1841
struct _fat_ptr _tmp5A3=(struct _fat_ptr)({const char*_tmp312=filename;_tag_fat(_tmp312,sizeof(char),_get_zero_arr_size_char((void*)_tmp312,1U));});_tmp413.f1=_tmp5A3;});_tmp413;});void*_tmp30F[1U];_tmp30F[0]=& _tmp311;({struct Cyc___cycFILE*_tmp5A5=out_file;struct _fat_ptr _tmp5A4=({const char*_tmp310="#error -- %s is not supported on this platform\n";_tag_fat(_tmp310,sizeof(char),48U);});Cyc_fprintf(_tmp5A5,_tmp5A4,_tag_fat(_tmp30F,sizeof(void*),1U));});});
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp315=({struct Cyc_String_pa_PrintArg_struct _tmp414;_tmp414.tag=0U,({
struct _fat_ptr _tmp5A6=(struct _fat_ptr)({const char*_tmp316=filename;_tag_fat(_tmp316,sizeof(char),_get_zero_arr_size_char((void*)_tmp316,1U));});_tmp414.f1=_tmp5A6;});_tmp414;});void*_tmp313[1U];_tmp313[0]=& _tmp315;({struct Cyc___cycFILE*_tmp5A8=Cyc_stderr;struct _fat_ptr _tmp5A7=({const char*_tmp314="Warning: %s will not be supported on this platform\n";_tag_fat(_tmp314,sizeof(char),52U);});Cyc_fprintf(_tmp5A8,_tmp5A7,_tag_fat(_tmp313,sizeof(void*),1U));});});
({void*_tmp317=0U;({struct _fat_ptr _tmp5A9=({const char*_tmp318="Not supported on this platform\n";_tag_fat(_tmp318,sizeof(char),32U);});Cyc_log(_tmp5A9,_tag_fat(_tmp317,sizeof(void*),0U));});});
# 1852
return 0;}}
# 1856
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp330=({struct Cyc_String_pa_PrintArg_struct _tmp41B;_tmp41B.tag=0U,({struct _fat_ptr _tmp5AA=(struct _fat_ptr)({const char*_tmp331=file;_tag_fat(_tmp331,sizeof(char),_get_zero_arr_size_char((void*)_tmp331,1U));});_tmp41B.f1=_tmp5AA;});_tmp41B;});void*_tmp32E[1U];_tmp32E[0]=& _tmp330;({struct Cyc___cycFILE*_tmp5AC=Cyc_stderr;struct _fat_ptr _tmp5AB=({const char*_tmp32F="Processing %s\n";_tag_fat(_tmp32F,sizeof(char),15U);});Cyc_fprintf(_tmp5AC,_tmp5AB,_tag_fat(_tmp32E,sizeof(void*),1U));});});{
struct Cyc___cycFILE*_tmp332=Cyc_fopen(file,"r");struct Cyc___cycFILE*maybe=_tmp332;
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp335=({struct Cyc_String_pa_PrintArg_struct _tmp41C;_tmp41C.tag=0U,({struct _fat_ptr _tmp5AD=(struct _fat_ptr)({const char*_tmp336=file;_tag_fat(_tmp336,sizeof(char),_get_zero_arr_size_char((void*)_tmp336,1U));});_tmp41C.f1=_tmp5AD;});_tmp41C;});void*_tmp333[1U];_tmp333[0]=& _tmp335;({struct Cyc___cycFILE*_tmp5AF=Cyc_stderr;struct _fat_ptr _tmp5AE=({const char*_tmp334="Error: could not open %s\n";_tag_fat(_tmp334,sizeof(char),26U);});Cyc_fprintf(_tmp5AF,_tmp5AE,_tag_fat(_tmp333,sizeof(void*),1U));});});
return 1;}{
# 1864
struct Cyc___cycFILE*_tmp337=maybe;struct Cyc___cycFILE*in_file=_tmp337;
# 1868
struct _fat_ptr buf=({char*_tmp355=({unsigned _tmp354=1024U + 1U;char*_tmp353=_cycalloc_atomic(_check_times(_tmp354,sizeof(char)));({{unsigned _tmp422=1024U;unsigned i;for(i=0;i < _tmp422;++ i){_tmp353[i]='\000';}_tmp353[_tmp422]=0;}0;});_tmp353;});_tag_fat(_tmp355,sizeof(char),1025U);});
struct _fat_ptr _tmp338=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp338;
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp33B=({struct Cyc_String_pa_PrintArg_struct _tmp41D;_tmp41D.tag=0U,({struct _fat_ptr _tmp5B0=(struct _fat_ptr)({const char*_tmp33C=dir;_tag_fat(_tmp33C,sizeof(char),_get_zero_arr_size_char((void*)_tmp33C,1U));});_tmp41D.f1=_tmp5B0;});_tmp41D;});void*_tmp339[1U];_tmp339[0]=& _tmp33B;({struct Cyc___cycFILE*_tmp5B2=Cyc_stderr;struct _fat_ptr _tmp5B1=({const char*_tmp33A="Error: can't change directory to %s\n";_tag_fat(_tmp33A,sizeof(char),37U);});Cyc_fprintf(_tmp5B2,_tmp5B1,_tag_fat(_tmp339,sizeof(void*),1U));});});
return 1;}}
# 1876
if((int)Cyc_mode == (int)Cyc_GATHER){
# 1878
struct _fat_ptr _tmp33D=({struct Cyc_String_pa_PrintArg_struct _tmp343=({struct Cyc_String_pa_PrintArg_struct _tmp420;_tmp420.tag=0U,_tmp420.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp420;});struct Cyc_String_pa_PrintArg_struct _tmp344=({struct Cyc_String_pa_PrintArg_struct _tmp41F;_tmp41F.tag=0U,_tmp41F.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp41F;});void*_tmp341[2U];_tmp341[0]=& _tmp343,_tmp341[1]=& _tmp344;({struct _fat_ptr _tmp5B3=({const char*_tmp342="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp342,sizeof(char),39U);});Cyc_aprintf(_tmp5B3,_tag_fat(_tmp341,sizeof(void*),2U));});});struct _fat_ptr cmd=_tmp33D;
# 1880
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp340=({struct Cyc_String_pa_PrintArg_struct _tmp41E;_tmp41E.tag=0U,_tmp41E.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp41E;});void*_tmp33E[1U];_tmp33E[0]=& _tmp340;({struct Cyc___cycFILE*_tmp5B5=Cyc_stderr;struct _fat_ptr _tmp5B4=({const char*_tmp33F="%s\n";_tag_fat(_tmp33F,sizeof(char),4U);});Cyc_fprintf(_tmp5B5,_tmp5B4,_tag_fat(_tmp33E,sizeof(void*),1U));});});
system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)));}{
# 1885
struct Cyc_Lexing_lexbuf*_tmp345=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp345;
struct _tuple17*entry;
while((entry=((struct _tuple17*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(l))!= 0){
struct _tuple17*_tmp346=(struct _tuple17*)_check_null(entry);struct _tuple17*_stmttmp24=_tmp346;struct _tuple17*_tmp347=_stmttmp24;struct Cyc_List_List*_tmp34F;struct Cyc_List_List*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_List_List*_tmp34C;struct Cyc_List_List*_tmp34B;struct Cyc_List_List*_tmp34A;struct Cyc_List_List*_tmp349;struct _fat_ptr _tmp348;_LL1: _tmp348=_tmp347->f1;_tmp349=_tmp347->f2;_tmp34A=_tmp347->f3;_tmp34B=_tmp347->f4;_tmp34C=_tmp347->f5;_tmp34D=_tmp347->f6;_tmp34E=_tmp347->f7;_tmp34F=_tmp347->f8;_LL2: {struct _fat_ptr headerfile=_tmp348;struct Cyc_List_List*start_symbols=_tmp349;struct Cyc_List_List*user_defs=_tmp34A;struct Cyc_List_List*omit_symbols=_tmp34B;struct Cyc_List_List*hstubs=_tmp34C;struct Cyc_List_List*cstubs=_tmp34D;struct Cyc_List_List*cycstubs=_tmp34E;struct Cyc_List_List*cpp_insert=_tmp34F;
# 1890
if(Cyc_process_file((const char*)_check_null(_untag_fat_ptr(headerfile,sizeof(char),1U)),start_symbols,user_defs,omit_symbols,hstubs,cstubs,cycstubs,cpp_insert))
# 1892
return 1;}}
# 1894
Cyc_fclose(in_file);
# 1896
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp352=({struct Cyc_String_pa_PrintArg_struct _tmp421;_tmp421.tag=0U,_tmp421.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp421;});void*_tmp350[1U];_tmp350[0]=& _tmp352;({struct Cyc___cycFILE*_tmp5B7=Cyc_stderr;struct _fat_ptr _tmp5B6=({const char*_tmp351="Error: could not change directory to %s\n";_tag_fat(_tmp351,sizeof(char),41U);});Cyc_fprintf(_tmp5B7,_tmp5B6,_tag_fat(_tmp350,sizeof(void*),1U));});});
return 1;}}
# 1902
return 0;}}}}
# 1906
int Cyc_process_setjmp(const char*dir){
# 1909
struct _fat_ptr buf=({char*_tmp367=({unsigned _tmp366=1024U + 1U;char*_tmp365=_cycalloc_atomic(_check_times(_tmp366,sizeof(char)));({{unsigned _tmp425=1024U;unsigned i;for(i=0;i < _tmp425;++ i){_tmp365[i]='\000';}_tmp365[_tmp425]=0;}0;});_tmp365;});_tag_fat(_tmp367,sizeof(char),1025U);});
struct _fat_ptr _tmp356=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp356;
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp359=({struct Cyc_String_pa_PrintArg_struct _tmp423;_tmp423.tag=0U,({struct _fat_ptr _tmp5B8=(struct _fat_ptr)({const char*_tmp35A=dir;_tag_fat(_tmp35A,sizeof(char),_get_zero_arr_size_char((void*)_tmp35A,1U));});_tmp423.f1=_tmp5B8;});_tmp423;});void*_tmp357[1U];_tmp357[0]=& _tmp359;({struct Cyc___cycFILE*_tmp5BA=Cyc_stderr;struct _fat_ptr _tmp5B9=({const char*_tmp358="Error: can't change directory to %s\n";_tag_fat(_tmp358,sizeof(char),37U);});Cyc_fprintf(_tmp5BA,_tmp5B9,_tag_fat(_tmp357,sizeof(void*),1U));});});
return 1;}
# 1915
if(({struct Cyc_List_List*_tmp5C0=({struct _fat_ptr*_tmp35B[1U];({struct _fat_ptr*_tmp5BC=({struct _fat_ptr*_tmp35D=_cycalloc(sizeof(*_tmp35D));({struct _fat_ptr _tmp5BB=({const char*_tmp35C="jmp_buf";_tag_fat(_tmp35C,sizeof(char),8U);});*_tmp35D=_tmp5BB;});_tmp35D;});_tmp35B[0]=_tmp5BC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp35B,sizeof(struct _fat_ptr*),1U));});Cyc_process_file("setjmp.h",_tmp5C0,0,0,({struct _tuple15*_tmp35E[1U];({struct _tuple15*_tmp5BF=({struct _tuple15*_tmp361=_cycalloc(sizeof(*_tmp361));
({struct _fat_ptr _tmp5BE=({const char*_tmp35F="setjmp";_tag_fat(_tmp35F,sizeof(char),7U);});_tmp361->f1=_tmp5BE;}),({struct _fat_ptr _tmp5BD=({const char*_tmp360="extern int setjmp(jmp_buf);\n";_tag_fat(_tmp360,sizeof(char),29U);});_tmp361->f2=_tmp5BD;});_tmp361;});_tmp35E[0]=_tmp5BF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp35E,sizeof(struct _tuple15*),1U));}),0,0,0);}))
# 1918
return 1;
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp424;_tmp424.tag=0U,_tmp424.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp424;});void*_tmp362[1U];_tmp362[0]=& _tmp364;({struct Cyc___cycFILE*_tmp5C2=Cyc_stderr;struct _fat_ptr _tmp5C1=({const char*_tmp363="Error: could not change directory to %s\n";_tag_fat(_tmp363,sizeof(char),41U);});Cyc_fprintf(_tmp5C2,_tmp5C1,_tag_fat(_tmp362,sizeof(void*),1U));});});
return 1;}
# 1923
return 0;}static char _tmp368[13U]="BUILDLIB.OUT";
# 1927
static struct _fat_ptr Cyc_output_dir={_tmp368,_tmp368,_tmp368 + 13U};
static void Cyc_set_output_dir(struct _fat_ptr s){
Cyc_output_dir=s;}
# 1931
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _fat_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->hd=(const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)),_tmp369->tl=Cyc_spec_files;_tmp369;});}
# 1935
static int Cyc_no_other(struct _fat_ptr s){return 0;}
static void Cyc_set_GATHER (void){
Cyc_mode=Cyc_GATHER;}
# 1939
static void Cyc_set_GATHERSCRIPT (void){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1942
static void Cyc_set_FINISH (void){
Cyc_mode=Cyc_FINISH;}
# 1945
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp36B=_cycalloc(sizeof(*_tmp36B));({struct _fat_ptr*_tmp5C3=({struct _fat_ptr*_tmp36A=_cycalloc(sizeof(*_tmp36A));*_tmp36A=s;_tmp36A;});_tmp36B->hd=_tmp5C3;}),_tmp36B->tl=Cyc_cppargs;_tmp36B;});}
# 1948
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp36E=({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0U,_tmp426.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp426;});void*_tmp36C[1U];_tmp36C[0]=& _tmp36E;({struct Cyc___cycFILE*_tmp5C5=Cyc_stderr;struct _fat_ptr _tmp5C4=({const char*_tmp36D="Unsupported option %s\n";_tag_fat(_tmp36D,sizeof(char),23U);});Cyc_fprintf(_tmp5C5,_tmp5C4,_tag_fat(_tmp36C,sizeof(void*),1U));});});
Cyc_badparse=1;}
# 1957
extern void GC_blacklist_warn_clear (void);struct _tuple19{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
int Cyc_main(int argc,struct _fat_ptr argv){
GC_blacklist_warn_clear();{
# 1961
struct Cyc_List_List*options=({struct _tuple19*_tmp3A6[9U];({
struct _tuple19*_tmp5F2=({struct _tuple19*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));({struct _fat_ptr _tmp5F1=({const char*_tmp3A7="-d";_tag_fat(_tmp3A7,sizeof(char),3U);});_tmp3AB->f1=_tmp5F1;}),_tmp3AB->f2=0,({struct _fat_ptr _tmp5F0=({const char*_tmp3A8=" <file>";_tag_fat(_tmp3A8,sizeof(char),8U);});_tmp3AB->f3=_tmp5F0;}),({
void*_tmp5EF=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));_tmp3A9->tag=5U,_tmp3A9->f1=Cyc_set_output_dir;_tmp3A9;});_tmp3AB->f4=_tmp5EF;}),({
struct _fat_ptr _tmp5EE=({const char*_tmp3AA="Set the output directory to <file>";_tag_fat(_tmp3AA,sizeof(char),35U);});_tmp3AB->f5=_tmp5EE;});_tmp3AB;});
# 1962
_tmp3A6[0]=_tmp5F2;}),({
# 1965
struct _tuple19*_tmp5ED=({struct _tuple19*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));({struct _fat_ptr _tmp5EC=({const char*_tmp3AC="-gather";_tag_fat(_tmp3AC,sizeof(char),8U);});_tmp3B0->f1=_tmp5EC;}),_tmp3B0->f2=0,({struct _fat_ptr _tmp5EB=({const char*_tmp3AD="";_tag_fat(_tmp3AD,sizeof(char),1U);});_tmp3B0->f3=_tmp5EB;}),({
void*_tmp5EA=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->tag=0U,_tmp3AE->f1=Cyc_set_GATHER;_tmp3AE;});_tmp3B0->f4=_tmp5EA;}),({
struct _fat_ptr _tmp5E9=({const char*_tmp3AF="Gather C library info but don't produce Cyclone headers";_tag_fat(_tmp3AF,sizeof(char),56U);});_tmp3B0->f5=_tmp5E9;});_tmp3B0;});
# 1965
_tmp3A6[1]=_tmp5ED;}),({
# 1968
struct _tuple19*_tmp5E8=({struct _tuple19*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));({struct _fat_ptr _tmp5E7=({const char*_tmp3B1="-gatherscript";_tag_fat(_tmp3B1,sizeof(char),14U);});_tmp3B5->f1=_tmp5E7;}),_tmp3B5->f2=0,({struct _fat_ptr _tmp5E6=({const char*_tmp3B2="";_tag_fat(_tmp3B2,sizeof(char),1U);});_tmp3B5->f3=_tmp5E6;}),({
void*_tmp5E5=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));_tmp3B3->tag=0U,_tmp3B3->f1=Cyc_set_GATHERSCRIPT;_tmp3B3;});_tmp3B5->f4=_tmp5E5;}),({
struct _fat_ptr _tmp5E4=({const char*_tmp3B4="Produce a script to gather C library info";_tag_fat(_tmp3B4,sizeof(char),42U);});_tmp3B5->f5=_tmp5E4;});_tmp3B5;});
# 1968
_tmp3A6[2]=_tmp5E8;}),({
# 1971
struct _tuple19*_tmp5E3=({struct _tuple19*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({struct _fat_ptr _tmp5E2=({const char*_tmp3B6="-finish";_tag_fat(_tmp3B6,sizeof(char),8U);});_tmp3BA->f1=_tmp5E2;}),_tmp3BA->f2=0,({struct _fat_ptr _tmp5E1=({const char*_tmp3B7="";_tag_fat(_tmp3B7,sizeof(char),1U);});_tmp3BA->f3=_tmp5E1;}),({
void*_tmp5E0=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));_tmp3B8->tag=0U,_tmp3B8->f1=Cyc_set_FINISH;_tmp3B8;});_tmp3BA->f4=_tmp5E0;}),({
struct _fat_ptr _tmp5DF=({const char*_tmp3B9="Produce Cyclone headers from pre-gathered C library info";_tag_fat(_tmp3B9,sizeof(char),57U);});_tmp3BA->f5=_tmp5DF;});_tmp3BA;});
# 1971
_tmp3A6[3]=_tmp5E3;}),({
# 1974
struct _tuple19*_tmp5DE=({struct _tuple19*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct _fat_ptr _tmp5DD=({const char*_tmp3BB="-setjmp";_tag_fat(_tmp3BB,sizeof(char),8U);});_tmp3BF->f1=_tmp5DD;}),_tmp3BF->f2=0,({struct _fat_ptr _tmp5DC=({const char*_tmp3BC="";_tag_fat(_tmp3BC,sizeof(char),1U);});_tmp3BF->f3=_tmp5DC;}),({
void*_tmp5DB=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=3U,_tmp3BD->f1=& Cyc_do_setjmp;_tmp3BD;});_tmp3BF->f4=_tmp5DB;}),({
# 1979
struct _fat_ptr _tmp5DA=({const char*_tmp3BE="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_fat(_tmp3BE,sizeof(char),186U);});_tmp3BF->f5=_tmp5DA;});_tmp3BF;});
# 1974
_tmp3A6[4]=_tmp5DE;}),({
# 1980
struct _tuple19*_tmp5D9=({struct _tuple19*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct _fat_ptr _tmp5D8=({const char*_tmp3C0="-b";_tag_fat(_tmp3C0,sizeof(char),3U);});_tmp3C4->f1=_tmp5D8;}),_tmp3C4->f2=0,({struct _fat_ptr _tmp5D7=({const char*_tmp3C1=" <machine>";_tag_fat(_tmp3C1,sizeof(char),11U);});_tmp3C4->f3=_tmp5D7;}),({
void*_tmp5D6=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->tag=5U,_tmp3C2->f1=Cyc_Specsfile_set_target_arch;_tmp3C2;});_tmp3C4->f4=_tmp5D6;}),({
struct _fat_ptr _tmp5D5=({const char*_tmp3C3="Set the target machine for compilation to <machine>";_tag_fat(_tmp3C3,sizeof(char),52U);});_tmp3C4->f5=_tmp5D5;});_tmp3C4;});
# 1980
_tmp3A6[5]=_tmp5D9;}),({
# 1983
struct _tuple19*_tmp5D4=({struct _tuple19*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({struct _fat_ptr _tmp5D3=({const char*_tmp3C5="-B";_tag_fat(_tmp3C5,sizeof(char),3U);});_tmp3C9->f1=_tmp5D3;}),_tmp3C9->f2=1,({struct _fat_ptr _tmp5D2=({const char*_tmp3C6="<file>";_tag_fat(_tmp3C6,sizeof(char),7U);});_tmp3C9->f3=_tmp5D2;}),({
void*_tmp5D1=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->tag=1U,_tmp3C7->f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp3C7;});_tmp3C9->f4=_tmp5D1;}),({
struct _fat_ptr _tmp5D0=({const char*_tmp3C8="Add to the list of directories to search for compiler files";_tag_fat(_tmp3C8,sizeof(char),60U);});_tmp3C9->f5=_tmp5D0;});_tmp3C9;});
# 1983
_tmp3A6[6]=_tmp5D4;}),({
# 1986
struct _tuple19*_tmp5CF=({struct _tuple19*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));({struct _fat_ptr _tmp5CE=({const char*_tmp3CA="-v";_tag_fat(_tmp3CA,sizeof(char),3U);});_tmp3CE->f1=_tmp5CE;}),_tmp3CE->f2=0,({struct _fat_ptr _tmp5CD=({const char*_tmp3CB="";_tag_fat(_tmp3CB,sizeof(char),1U);});_tmp3CE->f3=_tmp5CD;}),({
void*_tmp5CC=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->tag=3U,_tmp3CC->f1=& Cyc_verbose;_tmp3CC;});_tmp3CE->f4=_tmp5CC;}),({
struct _fat_ptr _tmp5CB=({const char*_tmp3CD="Verbose operation";_tag_fat(_tmp3CD,sizeof(char),18U);});_tmp3CE->f5=_tmp5CB;});_tmp3CE;});
# 1986
_tmp3A6[7]=_tmp5CF;}),({
# 1989
struct _tuple19*_tmp5CA=({struct _tuple19*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct _fat_ptr _tmp5C9=({const char*_tmp3CF="-";_tag_fat(_tmp3CF,sizeof(char),2U);});_tmp3D3->f1=_tmp5C9;}),_tmp3D3->f2=1,({struct _fat_ptr _tmp5C8=({const char*_tmp3D0="";_tag_fat(_tmp3D0,sizeof(char),1U);});_tmp3D3->f3=_tmp5C8;}),({
void*_tmp5C7=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->tag=1U,_tmp3D1->f1=Cyc_add_cpparg;_tmp3D1;});_tmp3D3->f4=_tmp5C7;}),({
struct _fat_ptr _tmp5C6=({const char*_tmp3D2="";_tag_fat(_tmp3D2,sizeof(char),1U);});_tmp3D3->f5=_tmp5C6;});_tmp3D3;});
# 1989
_tmp3A6[8]=_tmp5CA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3A6,sizeof(struct _tuple19*),9U));});
# 1994
struct _fat_ptr _tmp36F=({struct Cyc_List_List*_tmp5F4=options;struct _fat_ptr _tmp5F3=({const char*_tmp3A5="Options:";_tag_fat(_tmp3A5,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp5F4,Cyc_add_spec_file,Cyc_no_other,_tmp5F3,argv);});struct _fat_ptr otherargs=_tmp36F;
# 1996
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp5F6=options;struct _fat_ptr _tmp5F5=({const char*_tmp370="Options:";_tag_fat(_tmp370,sizeof(char),9U);});Cyc_Arg_parse(_tmp5F6,Cyc_add_spec_file,Cyc_no_other,_tmp5F5,otherargs);});
if((((Cyc_badparse ||
 !Cyc_do_setjmp && Cyc_spec_files == 0)||
 Cyc_do_setjmp && Cyc_spec_files != 0)||
 Cyc_do_setjmp &&(int)Cyc_mode == (int)Cyc_GATHER)||
 Cyc_do_setjmp &&(int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_List_List*_tmp5F7=options;Cyc_Arg_usage(_tmp5F7,({const char*_tmp371="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_fat(_tmp371,sizeof(char),59U);}));});
# 2006
return 1;}{
# 2012
struct _fat_ptr _tmp372=Cyc_Specsfile_find_in_arch_path(({const char*_tmp3A4="cycspecs";_tag_fat(_tmp3A4,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp372;
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp375=({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0U,_tmp427.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp427;});void*_tmp373[1U];_tmp373[0]=& _tmp375;({struct Cyc___cycFILE*_tmp5F9=Cyc_stderr;struct _fat_ptr _tmp5F8=({const char*_tmp374="Reading from specs file %s\n";_tag_fat(_tmp374,sizeof(char),28U);});Cyc_fprintf(_tmp5F9,_tmp5F8,_tag_fat(_tmp373,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp376=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp376;
Cyc_target_cflags=({struct Cyc_List_List*_tmp5FA=specs;Cyc_Specsfile_get_spec(_tmp5FA,({const char*_tmp377="cyclone_target_cflags";_tag_fat(_tmp377,sizeof(char),22U);}));});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp37A=({struct Cyc_String_pa_PrintArg_struct _tmp428;_tmp428.tag=0U,_tmp428.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp428;});void*_tmp378[1U];_tmp378[0]=& _tmp37A;({struct Cyc___cycFILE*_tmp5FC=Cyc_stderr;struct _fat_ptr _tmp5FB=({const char*_tmp379="Target cflags are %s\n";_tag_fat(_tmp379,sizeof(char),22U);});Cyc_fprintf(_tmp5FC,_tmp5FB,_tag_fat(_tmp378,sizeof(void*),1U));});});
Cyc_cyclone_cc=({struct Cyc_List_List*_tmp5FD=specs;Cyc_Specsfile_get_spec(_tmp5FD,({const char*_tmp37B="cyclone_cc";_tag_fat(_tmp37B,sizeof(char),11U);}));});
if(!((unsigned)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp37C="gcc";_tag_fat(_tmp37C,sizeof(char),4U);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp429;_tmp429.tag=0U,_tmp429.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp429;});void*_tmp37D[1U];_tmp37D[0]=& _tmp37F;({struct Cyc___cycFILE*_tmp5FF=Cyc_stderr;struct _fat_ptr _tmp5FE=({const char*_tmp37E="C compiler is %s\n";_tag_fat(_tmp37E,sizeof(char),18U);});Cyc_fprintf(_tmp5FF,_tmp5FE,_tag_fat(_tmp37D,sizeof(void*),1U));});});
# 2021
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp380=0U;({struct Cyc___cycFILE*_tmp601=Cyc_stderr;struct _fat_ptr _tmp600=({const char*_tmp381="Creating BUILDLIB.sh\n";_tag_fat(_tmp381,sizeof(char),22U);});Cyc_fprintf(_tmp601,_tmp600,_tag_fat(_tmp380,sizeof(void*),0U));});});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned)Cyc_script_file)){
({void*_tmp382=0U;({struct Cyc___cycFILE*_tmp603=Cyc_stderr;struct _fat_ptr _tmp602=({const char*_tmp383="Could not create file BUILDLIB.sh\n";_tag_fat(_tmp383,sizeof(char),35U);});Cyc_fprintf(_tmp603,_tmp602,_tag_fat(_tmp382,sizeof(void*),0U));});});
exit(1);}
# 2029
({void*_tmp384=0U;({struct _fat_ptr _tmp604=({const char*_tmp385="#!/bin/sh\n";_tag_fat(_tmp385,sizeof(char),11U);});Cyc_prscript(_tmp604,_tag_fat(_tmp384,sizeof(void*),0U));});});
({void*_tmp386=0U;({struct _fat_ptr _tmp605=({const char*_tmp387="GCC=\"gcc\"\n";_tag_fat(_tmp387,sizeof(char),11U);});Cyc_prscript(_tmp605,_tag_fat(_tmp386,sizeof(void*),0U));});});}
# 2034
if(Cyc_force_directory_prefixes(Cyc_output_dir)|| Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp38A=({struct Cyc_String_pa_PrintArg_struct _tmp42A;_tmp42A.tag=0U,_tmp42A.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42A;});void*_tmp388[1U];_tmp388[0]=& _tmp38A;({struct Cyc___cycFILE*_tmp607=Cyc_stderr;struct _fat_ptr _tmp606=({const char*_tmp389="Error: could not create directory %s\n";_tag_fat(_tmp389,sizeof(char),38U);});Cyc_fprintf(_tmp607,_tmp606,_tag_fat(_tmp388,sizeof(void*),1U));});});
return 1;}
# 2038
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp38D=({struct Cyc_String_pa_PrintArg_struct _tmp42B;_tmp42B.tag=0U,_tmp42B.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42B;});void*_tmp38B[1U];_tmp38B[0]=& _tmp38D;({struct Cyc___cycFILE*_tmp609=Cyc_stderr;struct _fat_ptr _tmp608=({const char*_tmp38C="Output directory is %s\n";_tag_fat(_tmp38C,sizeof(char),24U);});Cyc_fprintf(_tmp609,_tmp608,_tag_fat(_tmp38B,sizeof(void*),1U));});});
# 2041
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp390=({struct Cyc_String_pa_PrintArg_struct _tmp42C;_tmp42C.tag=0U,_tmp42C.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42C;});void*_tmp38E[1U];_tmp38E[0]=& _tmp390;({struct _fat_ptr _tmp60A=({const char*_tmp38F="cd %s\n";_tag_fat(_tmp38F,sizeof(char),7U);});Cyc_prscript(_tmp60A,_tag_fat(_tmp38E,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp42D;_tmp42D.tag=0U,_tmp42D.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp42D;});void*_tmp391[1U];_tmp391[0]=& _tmp393;({struct _fat_ptr _tmp60B=({const char*_tmp392="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp392,sizeof(char),41U);});Cyc_prscript(_tmp60B,_tag_fat(_tmp391,sizeof(void*),1U));});});}
# 2047
if(!Cyc_gathering()){
# 2050
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp60C=Cyc_output_dir;Cyc_Filename_concat(_tmp60C,({const char*_tmp394="BUILDLIB.LOG";_tag_fat(_tmp394,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp397=({struct Cyc_String_pa_PrintArg_struct _tmp42E;_tmp42E.tag=0U,_tmp42E.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42E;});void*_tmp395[1U];_tmp395[0]=& _tmp397;({struct Cyc___cycFILE*_tmp60E=Cyc_stderr;struct _fat_ptr _tmp60D=({const char*_tmp396="Error: could not create log file in directory %s\n";_tag_fat(_tmp396,sizeof(char),50U);});Cyc_fprintf(_tmp60E,_tmp60D,_tag_fat(_tmp395,sizeof(void*),1U));});});
return 1;}
# 2056
if(!Cyc_do_setjmp){
# 2058
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp60F=Cyc_output_dir;Cyc_Filename_concat(_tmp60F,({const char*_tmp398="cstubs.c";_tag_fat(_tmp398,sizeof(char),9U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp39B=({struct Cyc_String_pa_PrintArg_struct _tmp42F;_tmp42F.tag=0U,_tmp42F.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp42F;});void*_tmp399[1U];_tmp399[0]=& _tmp39B;({struct Cyc___cycFILE*_tmp611=Cyc_stderr;struct _fat_ptr _tmp610=({const char*_tmp39A="Error: could not create cstubs.c in directory %s\n";_tag_fat(_tmp39A,sizeof(char),50U);});Cyc_fprintf(_tmp611,_tmp610,_tag_fat(_tmp399,sizeof(void*),1U));});});
return 1;}
# 2065
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp612=Cyc_output_dir;Cyc_Filename_concat(_tmp612,({const char*_tmp39C="cycstubs.cyc";_tag_fat(_tmp39C,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp39F=({struct Cyc_String_pa_PrintArg_struct _tmp430;_tmp430.tag=0U,_tmp430.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp430;});void*_tmp39D[1U];_tmp39D[0]=& _tmp39F;({struct Cyc___cycFILE*_tmp614=Cyc_stderr;struct _fat_ptr _tmp613=({const char*_tmp39E="Error: could not create cycstubs.c in directory %s\n";_tag_fat(_tmp39E,sizeof(char),52U);});Cyc_fprintf(_tmp614,_tmp613,_tag_fat(_tmp39D,sizeof(void*),1U));});});
# 2070
return 1;}
# 2072
({void*_tmp3A0=0U;({struct Cyc___cycFILE*_tmp616=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _fat_ptr _tmp615=({const char*_tmp3A1="#include <core.h>\nusing Core;\n\n";_tag_fat(_tmp3A1,sizeof(char),32U);});Cyc_fprintf(_tmp616,_tmp615,_tag_fat(_tmp3A0,sizeof(void*),0U));});});}}{
# 2079
const char*outdir=(const char*)_check_null(_untag_fat_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp && Cyc_process_setjmp(outdir))
return 1;else{
# 2085
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp3A2=0U;({struct Cyc___cycFILE*_tmp618=Cyc_stderr;struct _fat_ptr _tmp617=({const char*_tmp3A3="FATAL ERROR -- QUIT!\n";_tag_fat(_tmp3A3,sizeof(char),22U);});Cyc_fprintf(_tmp618,_tmp617,_tag_fat(_tmp3A2,sizeof(void*),0U));});});
exit(1);}}}
# 2093
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2096
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2104
return 0;}}}}}
