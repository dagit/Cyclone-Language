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
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)

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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;
# 175
void Cyc_Core_ufree(void*ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 231
struct Cyc_Core_NewDynamicRegion Cyc_Core__new_ukey(const char*file,const char*func,int lineno);
# 251 "core.h"
void Cyc_Core_free_ukey(struct Cyc_Core_DynamicRegion*k);
# 261
void*Cyc_Core_open_region(struct Cyc_Core_DynamicRegion*key,void*arg,void*(*body)(struct _RegionHandle*h,void*arg));struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 197 "cycboot.h"
int Cyc_sscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _fat_ptr lex_base;struct _fat_ptr lex_backtrk;struct _fat_ptr lex_default;struct _fat_ptr lex_trans;struct _fat_ptr lex_check;};
# 84 "lexing.h"
struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 127
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
# 54
int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 73
struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 109 "string.h"
struct _fat_ptr Cyc_substring(struct _fat_ptr,int ofs,unsigned long n);struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 42 "xarray.h"
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 51
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate(struct _RegionHandle*,int,void*);
# 66
void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
# 52 "parse.h"
enum Cyc_Storage_class{Cyc_Typedef_sc =0U,Cyc_Extern_sc =1U,Cyc_ExternC_sc =2U,Cyc_Static_sc =3U,Cyc_Auto_sc =4U,Cyc_Register_sc =5U,Cyc_Abstract_sc =6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 83 "parse_tab.h"
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 110 "dict.h"
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 30 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 28
void Cyc_Warn_warn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 34
void Cyc_Warn_err(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);
# 39
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Lex_Trie;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple19{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple19 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 50 "lex.cyl"
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){return({union Cyc_Lex_TrieChildren _tmp142;(_tmp142.Zero).tag=1U,(_tmp142.Zero).val=0;_tmp142;});}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
return({union Cyc_Lex_TrieChildren _tmp143;(_tmp143.One).tag=2U,((_tmp143.One).val).f1=i,((_tmp143.One).val).f2=t;_tmp143;});}
# 54
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
return({union Cyc_Lex_TrieChildren _tmp144;(_tmp144.Many).tag=3U,(_tmp144.Many).val=ts;_tmp144;});}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};struct Cyc_Lex_DynTrie{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;};struct Cyc_Lex_DynTrieSymbols{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 78
static int Cyc_Lex_num_kws=0;struct Cyc_Lex_KeyWordInfo{int token_index;int is_c_keyword;};
# 80
static struct _fat_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};
# 82
int Cyc_Lex_compile_for_boot_r=0;
int Cyc_Lex_expand_specials=0;
# 85
static int Cyc_Lex_in_extern_c=0;
static struct Cyc_List_List*Cyc_Lex_prev_extern_c=0;
void Cyc_Lex_enter_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp0=x;struct Cyc_List_List*_tmp1=Cyc_Lex_prev_extern_c;x=_tmp1;Cyc_Lex_prev_extern_c=_tmp0;});
Cyc_Lex_prev_extern_c=({struct Cyc_List_List*_tmp2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp2));_tmp2->hd=(void*)Cyc_Lex_in_extern_c,_tmp2->tl=x;_tmp2;});
Cyc_Lex_in_extern_c=1;}
# 93
void Cyc_Lex_leave_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp3=x;struct Cyc_List_List*_tmp4=Cyc_Lex_prev_extern_c;x=_tmp4;Cyc_Lex_prev_extern_c=_tmp3;});
Cyc_Lex_in_extern_c=(int)((struct Cyc_List_List*)_check_null(x))->hd;
Cyc_Lex_prev_extern_c=x->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);}
# 101
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 104
static int Cyc_Lex_comment_depth=0;
# 107
static union Cyc_Absyn_Cnst Cyc_Lex_token_int={.Int_c={5,{Cyc_Absyn_Signed,0}}};static char _tmp5[8U]="*bogus*";
static struct _fat_ptr Cyc_Lex_bogus_string={_tmp5,_tmp5,_tmp5 + 8U};
static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};
# 112
static char Cyc_Lex_token_char='\000';static char _tmp6[1U]="";
struct _fat_ptr Cyc_Lex_token_string={_tmp6,_tmp6,_tmp6 + 1U};
struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;static char _tmp7[1U]="";
static struct _tuple12 Cyc_Lex_token_asm={0,{_tmp7,_tmp7,_tmp7 + 1U}};
# 117
static int Cyc_Lex_runaway_start=0;
static int Cyc_Lex_paren_depth=0;
# 120
static void Cyc_Lex_err(struct _fat_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned s=(unsigned)Cyc_Lexing_lexeme_start(lb);
({void*_tmp8=0U;({unsigned _tmp16A=s;struct _fat_ptr _tmp169=msg;Cyc_Warn_err(_tmp16A,_tmp169,_tag_fat(_tmp8,sizeof(void*),0U));});});}
# 124
static void Cyc_Lex_runaway_err(struct _fat_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned s=(unsigned)Cyc_Lex_runaway_start;
({void*_tmp9=0U;({unsigned _tmp16C=s;struct _fat_ptr _tmp16B=msg;Cyc_Warn_err(_tmp16C,_tmp16B,_tag_fat(_tmp9,sizeof(void*),0U));});});}
# 128
static char Cyc_Lex_cnst2char(union Cyc_Absyn_Cnst cnst,struct Cyc_Lexing_lexbuf*lb){
union Cyc_Absyn_Cnst _tmpA=cnst;char _tmpE;long long _tmpD;int _tmpC;switch((_tmpA.Char_c).tag){case 5U: _LL1: _tmpC=((_tmpA.Int_c).val).f2;_LL2:
 return(char)_tmpC;case 6U: _LL3: _tmpD=((_tmpA.LongLong_c).val).f2;_LL4:
 return(char)_tmpD;case 2U: _LL5: _tmpE=((_tmpA.Char_c).val).f2;_LL6:
 return _tmpE;default: _LL7: _LL8:
({struct _fat_ptr _tmp16D=({const char*_tmpB="couldn't convert constant to char!";_tag_fat(_tmpB,sizeof(char),35U);});Cyc_Lex_err(_tmp16D,lb);});return'\000';}_LL0:;}static char _tmpF[14U]="__attribute__";static char _tmp10[9U]="abstract";static char _tmp11[5U]="auto";static char _tmp12[6U]="break";static char _tmp13[18U]="__builtin_va_list";static char _tmp14[7U]="calloc";static char _tmp15[5U]="case";static char _tmp16[6U]="catch";static char _tmp17[5U]="char";static char _tmp18[6U]="const";static char _tmp19[10U]="__const__";static char _tmp1A[9U]="continue";static char _tmp1B[17U]="cyclone_override";static char _tmp1C[9U]="datatype";static char _tmp1D[8U]="default";static char _tmp1E[3U]="do";static char _tmp1F[7U]="double";static char _tmp20[5U]="else";static char _tmp21[5U]="enum";static char _tmp22[7U]="export";static char _tmp23[14U]="__extension__";static char _tmp24[7U]="extern";static char _tmp25[9U]="fallthru";static char _tmp26[6U]="float";static char _tmp27[4U]="for";static char _tmp28[5U]="goto";static char _tmp29[3U]="if";static char _tmp2A[7U]="inline";static char _tmp2B[11U]="__inline__";static char _tmp2C[4U]="int";static char _tmp2D[4U]="let";static char _tmp2E[5U]="long";static char _tmp2F[7U]="malloc";static char _tmp30[10U]="namespace";static char _tmp31[4U]="new";static char _tmp32[5U]="NULL";static char _tmp33[8U]="numelts";static char _tmp34[9U]="offsetof";static char _tmp35[20U]="__cyclone_port_on__";static char _tmp36[21U]="__cyclone_port_off__";static char _tmp37[19U]="__cyclone_pragma__";static char _tmp38[8U]="rcalloc";static char _tmp39[9U]="region_t";static char _tmp3A[7U]="region";static char _tmp3B[8U]="regions";static char _tmp3C[9U]="register";static char _tmp3D[9U]="restrict";static char _tmp3E[7U]="return";static char _tmp3F[8U]="rmalloc";static char _tmp40[15U]="rmalloc_inline";static char _tmp41[5U]="rnew";static char _tmp42[6U]="short";static char _tmp43[7U]="signed";static char _tmp44[11U]="__signed__";static char _tmp45[7U]="sizeof";static char _tmp46[7U]="static";static char _tmp47[7U]="struct";static char _tmp48[7U]="switch";static char _tmp49[9U]="tagcheck";static char _tmp4A[6U]="tag_t";static char _tmp4B[15U]="__tempest_on__";static char _tmp4C[16U]="__tempest_off__";static char _tmp4D[6U]="throw";static char _tmp4E[4U]="try";static char _tmp4F[8U]="typedef";static char _tmp50[7U]="typeof";static char _tmp51[11U]="__typeof__";static char _tmp52[6U]="union";static char _tmp53[9U]="unsigned";static char _tmp54[13U]="__unsigned__";static char _tmp55[6U]="using";static char _tmp56[8U]="valueof";static char _tmp57[10U]="valueof_t";static char _tmp58[5U]="void";static char _tmp59[9U]="volatile";static char _tmp5A[13U]="__volatile__";static char _tmp5B[6U]="while";struct _tuple20{struct _fat_ptr f1;short f2;int f3;};
# 140
static struct _tuple20 Cyc_Lex_rw_array[77U]={{{_tmpF,_tmpF,_tmpF + 14U},376,1},{{_tmp10,_tmp10,_tmp10 + 9U},304,0},{{_tmp11,_tmp11,_tmp11 + 5U},258,1},{{_tmp12,_tmp12,_tmp12 + 6U},290,1},{{_tmp13,_tmp13,_tmp13 + 18U},294,1},{{_tmp14,_tmp14,_tmp14 + 7U},312,0},{{_tmp15,_tmp15,_tmp15 + 5U},277,1},{{_tmp16,_tmp16,_tmp16 + 6U},300,1},{{_tmp17,_tmp17,_tmp17 + 5U},264,1},{{_tmp18,_tmp18,_tmp18 + 6U},272,1},{{_tmp19,_tmp19,_tmp19 + 10U},272,1},{{_tmp1A,_tmp1A,_tmp1A + 9U},289,1},{{_tmp1B,_tmp1B,_tmp1B + 17U},302,0},{{_tmp1C,_tmp1C,_tmp1C + 9U},308,0},{{_tmp1D,_tmp1D,_tmp1D + 8U},278,1},{{_tmp1E,_tmp1E,_tmp1E + 3U},286,1},{{_tmp1F,_tmp1F,_tmp1F + 7U},269,1},{{_tmp20,_tmp20,_tmp20 + 5U},283,1},{{_tmp21,_tmp21,_tmp21 + 5U},292,1},{{_tmp22,_tmp22,_tmp22 + 7U},301,0},{{_tmp23,_tmp23,_tmp23 + 14U},295,1},{{_tmp24,_tmp24,_tmp24 + 7U},261,1},{{_tmp25,_tmp25,_tmp25 + 9U},305,0},{{_tmp26,_tmp26,_tmp26 + 6U},268,1},{{_tmp27,_tmp27,_tmp27 + 4U},287,1},{{_tmp28,_tmp28,_tmp28 + 5U},288,1},{{_tmp29,_tmp29,_tmp29 + 3U},282,1},{{_tmp2A,_tmp2A,_tmp2A + 7U},279,1},{{_tmp2B,_tmp2B,_tmp2B + 11U},279,1},{{_tmp2C,_tmp2C,_tmp2C + 4U},266,1},{{_tmp2D,_tmp2D,_tmp2D + 4U},297,0},{{_tmp2E,_tmp2E,_tmp2E + 5U},267,1},{{_tmp2F,_tmp2F,_tmp2F + 7U},309,0},{{_tmp30,_tmp30,_tmp30 + 10U},307,0},{{_tmp31,_tmp31,_tmp31 + 4U},303,0},{{_tmp32,_tmp32,_tmp32 + 5U},296,0},{{_tmp33,_tmp33,_tmp33 + 8U},325,0},{{_tmp34,_tmp34,_tmp34 + 9U},281,1},{{_tmp35,_tmp35,_tmp35 + 20U},320,0},{{_tmp36,_tmp36,_tmp36 + 21U},321,0},{{_tmp37,_tmp37,_tmp37 + 19U},322,0},{{_tmp38,_tmp38,_tmp38 + 8U},313,0},{{_tmp39,_tmp39,_tmp39 + 9U},315,0},{{_tmp3A,_tmp3A,_tmp3A + 7U},317,0},{{_tmp3B,_tmp3B,_tmp3B + 8U},319,0},{{_tmp3C,_tmp3C,_tmp3C + 9U},259,1},{{_tmp3D,_tmp3D,_tmp3D + 9U},274,1},{{_tmp3E,_tmp3E,_tmp3E + 7U},291,1},{{_tmp3F,_tmp3F,_tmp3F + 8U},310,0},{{_tmp40,_tmp40,_tmp40 + 15U},311,0},{{_tmp41,_tmp41,_tmp41 + 5U},318,0},{{_tmp42,_tmp42,_tmp42 + 6U},265,1},{{_tmp43,_tmp43,_tmp43 + 7U},270,1},{{_tmp44,_tmp44,_tmp44 + 11U},270,1},{{_tmp45,_tmp45,_tmp45 + 7U},280,1},{{_tmp46,_tmp46,_tmp46 + 7U},260,1},{{_tmp47,_tmp47,_tmp47 + 7U},275,1},{{_tmp48,_tmp48,_tmp48 + 7U},284,1},{{_tmp49,_tmp49,_tmp49 + 9U},328,0},{{_tmp4A,_tmp4A,_tmp4A + 6U},316,0},{{_tmp4B,_tmp4B,_tmp4B + 15U},323,0},{{_tmp4C,_tmp4C,_tmp4C + 16U},324,0},{{_tmp4D,_tmp4D,_tmp4D + 6U},298,0},{{_tmp4E,_tmp4E,_tmp4E + 4U},299,0},{{_tmp4F,_tmp4F,_tmp4F + 8U},262,1},{{_tmp50,_tmp50,_tmp50 + 7U},293,1},{{_tmp51,_tmp51,_tmp51 + 11U},293,1},{{_tmp52,_tmp52,_tmp52 + 6U},276,1},{{_tmp53,_tmp53,_tmp53 + 9U},271,1},{{_tmp54,_tmp54,_tmp54 + 13U},271,1},{{_tmp55,_tmp55,_tmp55 + 6U},306,0},{{_tmp56,_tmp56,_tmp56 + 8U},326,0},{{_tmp57,_tmp57,_tmp57 + 10U},327,0},{{_tmp58,_tmp58,_tmp58 + 5U},263,1},{{_tmp59,_tmp59,_tmp59 + 9U},273,1},{{_tmp5A,_tmp5A,_tmp5A + 13U},273,1},{{_tmp5B,_tmp5B,_tmp5B + 6U},285,1}};
# 220
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned i=0U;for(0;i < 77U;++ i){
if(include_cyclone_keywords ||(Cyc_Lex_rw_array[(int)i]).f3)
++ sum;}}
return sum;}
# 229
static struct Cyc_Lex_Trie*Cyc_Lex_empty_trie(struct _RegionHandle*d,int dummy){
return({struct Cyc_Lex_Trie*_tmp5C=_region_malloc(d,sizeof(*_tmp5C));({union Cyc_Lex_TrieChildren _tmp16E=Cyc_Lex_Zero();_tmp5C->children=_tmp16E;}),_tmp5C->shared_str=0;_tmp5C;});}
# 233
static int Cyc_Lex_trie_char(int c){
# 235
if(c >= 95)return c - 59;else{
if(c > 64)return c - 55;}
return c - 48;}
# 240
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _fat_ptr buff,int offset,unsigned len){
# 242
unsigned i=0U;
buff=_fat_ptr_plus(buff,sizeof(char),offset);
if(!(len < _get_fat_size(buff,sizeof(char))))
({void*_tmp5D=0U;({struct _fat_ptr _tmp16F=({const char*_tmp5E="array bounds in trie_lookup";_tag_fat(_tmp5E,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Warn_impos)(_tmp16F,_tag_fat(_tmp5D,sizeof(void*),0U));});});
while(i < len){
union Cyc_Lex_TrieChildren _tmp5F=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _tmp60=_tmp5F;int _tmp6B;struct Cyc_Lex_Trie*_tmp6A;struct Cyc_Lex_Trie**_tmp69;switch((_tmp60.One).tag){case 3U: _LL1: _tmp69=(_tmp60.Many).val;_LL2: {
# 250
int ch=Cyc_Lex_trie_char((int)((const char*)buff.curr)[(int)i]);
if(*((struct Cyc_Lex_Trie**)_check_known_subscript_notnull(_tmp69,64U,sizeof(struct Cyc_Lex_Trie*),ch))== 0)
({struct Cyc_Lex_Trie*_tmp171=({struct Cyc_Lex_Trie*_tmp61=_region_malloc(r,sizeof(*_tmp61));({union Cyc_Lex_TrieChildren _tmp170=Cyc_Lex_Zero();_tmp61->children=_tmp170;}),_tmp61->shared_str=0;_tmp61;});_tmp69[ch]=_tmp171;});
t=_tmp69[ch];
++ i;
goto _LL0;}case 2U: _LL3: _tmp6B=((_tmp60.One).val).f1;_tmp6A=((_tmp60.One).val).f2;_LL4:
# 258
 if(_tmp6B == (int)((const char*)buff.curr)[(int)i])
t=_tmp6A;else{
# 261
struct Cyc_Lex_Trie**_tmp62=({unsigned _tmp66=64U;struct Cyc_Lex_Trie**_tmp65=({struct _RegionHandle*_tmp172=r;_region_malloc(_tmp172,_check_times(_tmp66,sizeof(struct Cyc_Lex_Trie*)));});({{unsigned _tmp145=64U;unsigned j;for(j=0;j < _tmp145;++ j){_tmp65[j]=0;}}0;});_tmp65;});
({struct Cyc_Lex_Trie*_tmp174=_tmp6A;*((struct Cyc_Lex_Trie**)({typeof(_tmp62 )_tmp173=_tmp62;_check_known_subscript_notnull(_tmp173,64U,sizeof(struct Cyc_Lex_Trie*),Cyc_Lex_trie_char(_tmp6B));}))=_tmp174;});{
int _tmp63=Cyc_Lex_trie_char((int)((const char*)buff.curr)[(int)i]);
({struct Cyc_Lex_Trie*_tmp176=({struct Cyc_Lex_Trie*_tmp64=_region_malloc(r,sizeof(*_tmp64));({union Cyc_Lex_TrieChildren _tmp175=Cyc_Lex_Zero();_tmp64->children=_tmp175;}),_tmp64->shared_str=0;_tmp64;});*((struct Cyc_Lex_Trie**)_check_known_subscript_notnull(_tmp62,64U,sizeof(struct Cyc_Lex_Trie*),_tmp63))=_tmp176;});
({union Cyc_Lex_TrieChildren _tmp177=Cyc_Lex_Many(_tmp62);t->children=_tmp177;});
t=_tmp62[_tmp63];};}
# 268
++ i;
goto _LL0;default: _LL5: _LL6:
# 272
 while(i < len){
struct Cyc_Lex_Trie*_tmp67=({struct Cyc_Lex_Trie*_tmp68=_region_malloc(r,sizeof(*_tmp68));({union Cyc_Lex_TrieChildren _tmp178=Cyc_Lex_Zero();_tmp68->children=_tmp178;}),_tmp68->shared_str=0;_tmp68;});
({union Cyc_Lex_TrieChildren _tmp179=Cyc_Lex_One((int)*((const char*)_check_fat_subscript(buff,sizeof(char),(int)i ++)),_tmp67);t->children=_tmp179;});
t=_tmp67;}
# 277
return t;}_LL0:;}
# 280
return t;}struct _tuple21{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;struct _fat_ptr f3;int f4;int f5;};
# 283
static int Cyc_Lex_str_index_body(struct _RegionHandle*d,struct _tuple21*env){
# 286
struct _tuple21 _tmp6C=*env;struct _tuple21 _tmp6D=_tmp6C;struct Cyc_Lex_Trie*_tmp75;struct Cyc_Xarray_Xarray*_tmp74;struct _fat_ptr _tmp73;int _tmp72;int _tmp71;_LL1: _tmp75=_tmp6D.f1;_tmp74=_tmp6D.f2;_tmp73=_tmp6D.f3;_tmp72=_tmp6D.f4;_tmp71=_tmp6D.f5;_LL2:;{
struct Cyc_Lex_Trie*_tmp6E=Cyc_Lex_trie_lookup(d,_tmp75,_tmp73,_tmp72,(unsigned)_tmp71);
# 289
if(((struct Cyc_Lex_Trie*)_check_null(_tmp6E))->shared_str == 0){
struct _fat_ptr _tmp6F=Cyc_Core_new_string((unsigned)(_tmp71 + 1));
({struct _fat_ptr _tmp17B=_fat_ptr_decrease_size(_tmp6F,sizeof(char),1U);struct _fat_ptr _tmp17A=(struct _fat_ptr)_fat_ptr_plus(_tmp73,sizeof(char),_tmp72);Cyc_zstrncpy(_tmp17B,_tmp17A,(unsigned long)_tmp71);});{
int ans=({struct Cyc_Xarray_Xarray*_tmp17C=_tmp74;((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(_tmp17C,({struct _fat_ptr*_tmp70=_cycalloc(sizeof(*_tmp70));*_tmp70=(struct _fat_ptr)_tmp6F;_tmp70;}));});
_tmp6E->shared_str=ans;};}
# 295
return _tmp6E->shared_str;};}
# 298
static int Cyc_Lex_str_index(struct _fat_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp76=idt;struct Cyc_Lex_DynTrieSymbols*_tmp77=Cyc_Lex_ids_trie;idt=_tmp77;Cyc_Lex_ids_trie=_tmp76;});{
struct Cyc_Lex_DynTrieSymbols _tmp78=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp79=_tmp78;struct Cyc_Core_DynamicRegion*_tmp80;struct Cyc_Lex_Trie*_tmp7F;struct Cyc_Xarray_Xarray*_tmp7E;_LL1: _tmp80=_tmp79.dyn;_tmp7F=_tmp79.t;_tmp7E=_tmp79.symbols;_LL2:;{
struct _tuple21 _tmp7A=({struct _tuple21 _tmp147;_tmp147.f1=_tmp7F,_tmp147.f2=_tmp7E,_tmp147.f3=buff,_tmp147.f4=offset,_tmp147.f5=len;_tmp147;});
int _tmp7B=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple21*arg,int(*body)(struct _RegionHandle*h,struct _tuple21*arg)))Cyc_Core_open_region)(_tmp80,& _tmp7A,Cyc_Lex_str_index_body);
({struct Cyc_Lex_DynTrieSymbols _tmp17D=({struct Cyc_Lex_DynTrieSymbols _tmp146;_tmp146.dyn=_tmp80,_tmp146.t=_tmp7F,_tmp146.symbols=_tmp7E;_tmp146;});*idt=_tmp17D;});
({struct Cyc_Lex_DynTrieSymbols*_tmp7C=idt;struct Cyc_Lex_DynTrieSymbols*_tmp7D=Cyc_Lex_ids_trie;idt=_tmp7D;Cyc_Lex_ids_trie=_tmp7C;});
return _tmp7B;};};}
# 309
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _fat_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}struct _tuple22{struct Cyc_Lex_Trie*f1;struct _fat_ptr f2;};
# 315
static int Cyc_Lex_insert_typedef_body(struct _RegionHandle*h,struct _tuple22*arg){
# 317
struct _tuple22 _tmp81=*arg;struct _tuple22 _tmp82=_tmp81;struct Cyc_Lex_Trie*_tmp85;struct _fat_ptr _tmp84;_LL1: _tmp85=_tmp82.f1;_tmp84=_tmp82.f2;_LL2:;{
struct Cyc_Lex_Trie*_tmp83=Cyc_Lex_trie_lookup(h,_tmp85,_tmp84,0,_get_fat_size(_tmp84,sizeof(char))- (unsigned)1);
((struct Cyc_Lex_Trie*)_check_null(_tmp83))->shared_str=1;
return 0;};}
# 324
static void Cyc_Lex_insert_typedef(struct _fat_ptr*sptr){
struct _fat_ptr _tmp86=*sptr;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp87=tdefs;struct Cyc_Lex_DynTrie*_tmp88=Cyc_Lex_typedefs_trie;tdefs=_tmp88;Cyc_Lex_typedefs_trie=_tmp87;});{
struct Cyc_Lex_DynTrie _tmp89=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmp8A=_tmp89;struct Cyc_Core_DynamicRegion*_tmp8F;struct Cyc_Lex_Trie*_tmp8E;_LL1: _tmp8F=_tmp8A.dyn;_tmp8E=_tmp8A.t;_LL2:;{
struct _tuple22 _tmp8B=({struct _tuple22 _tmp149;_tmp149.f1=_tmp8E,_tmp149.f2=_tmp86;_tmp149;});
((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple22*arg,int(*body)(struct _RegionHandle*h,struct _tuple22*arg)))Cyc_Core_open_region)(_tmp8F,& _tmp8B,Cyc_Lex_insert_typedef_body);
({struct Cyc_Lex_DynTrie _tmp17E=({struct Cyc_Lex_DynTrie _tmp148;_tmp148.dyn=_tmp8F,_tmp148.t=_tmp8E;_tmp148;});*tdefs=_tmp17E;});
({struct Cyc_Lex_DynTrie*_tmp8C=tdefs;struct Cyc_Lex_DynTrie*_tmp8D=Cyc_Lex_typedefs_trie;tdefs=_tmp8D;Cyc_Lex_typedefs_trie=_tmp8C;});
return;};};}struct _tuple23{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;int f3;};
# 336
static struct _fat_ptr*Cyc_Lex_get_symbol_body(struct _RegionHandle*dyn,struct _tuple23*env){
struct _tuple23 _tmp90=*env;struct _tuple23 _tmp91=_tmp90;struct Cyc_Lex_Trie*_tmp94;struct Cyc_Xarray_Xarray*_tmp93;int _tmp92;_LL1: _tmp94=_tmp91.f1;_tmp93=_tmp91.f2;_tmp92=_tmp91.f3;_LL2:;
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp93,_tmp92);}
# 341
static struct _fat_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp95=idt;struct Cyc_Lex_DynTrieSymbols*_tmp96=Cyc_Lex_ids_trie;idt=_tmp96;Cyc_Lex_ids_trie=_tmp95;});{
struct Cyc_Lex_DynTrieSymbols _tmp97=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp98=_tmp97;struct Cyc_Core_DynamicRegion*_tmp9F;struct Cyc_Lex_Trie*_tmp9E;struct Cyc_Xarray_Xarray*_tmp9D;_LL1: _tmp9F=_tmp98.dyn;_tmp9E=_tmp98.t;_tmp9D=_tmp98.symbols;_LL2:;{
struct _tuple23 _tmp99=({struct _tuple23 _tmp14B;_tmp14B.f1=_tmp9E,_tmp14B.f2=_tmp9D,_tmp14B.f3=symbol_num;_tmp14B;});
struct _fat_ptr*_tmp9A=((struct _fat_ptr*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple23*arg,struct _fat_ptr*(*body)(struct _RegionHandle*h,struct _tuple23*arg)))Cyc_Core_open_region)(_tmp9F,& _tmp99,Cyc_Lex_get_symbol_body);
({struct Cyc_Lex_DynTrieSymbols _tmp17F=({struct Cyc_Lex_DynTrieSymbols _tmp14A;_tmp14A.dyn=_tmp9F,_tmp14A.t=_tmp9E,_tmp14A.symbols=_tmp9D;_tmp14A;});*idt=_tmp17F;});
({struct Cyc_Lex_DynTrieSymbols*_tmp9B=idt;struct Cyc_Lex_DynTrieSymbols*_tmp9C=Cyc_Lex_ids_trie;idt=_tmp9C;Cyc_Lex_ids_trie=_tmp9B;});
return _tmp9A;};};}
# 360 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if((int)'0' <= (int)c &&(int)c <= (int)'9')return(int)c - (int)'0';else{
if((int)'a' <= (int)c &&(int)c <= (int)'f')return(10 + (int)c)- (int)'a';else{
if((int)'A' <= (int)c &&(int)c <= (int)'F')return(10 + (int)c)- (int)'A';else{
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp180=({const char*_tmpA0="string to integer conversion";_tag_fat(_tmpA0,sizeof(char),29U);});_tmpA1->f1=_tmp180;});_tmpA1;}));}}}}
# 368
static union Cyc_Absyn_Cnst Cyc_Lex_intconst(struct Cyc_Lexing_lexbuf*lbuf,int start,int end,int base){
enum Cyc_Absyn_Sign sn=2U;
start +=lbuf->lex_start_pos;{
struct _fat_ptr buff=_fat_ptr_plus(lbuf->lex_buffer,sizeof(char),start);
int end2=lbuf->lex_curr_pos - end;
int len=end2 - start;
enum Cyc_Absyn_Size_of size=2U;
int declared_size=0;
union Cyc_Absyn_Cnst res;
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'l' ||(int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'L')){
# 379
len -=1;
declared_size=1;
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'l' ||(int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'L')){
len -=1;
size=4U;}}
# 386
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'u' ||(int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'U')){
len -=1;
sn=1U;}
# 390
if((int)sn == (int)1U){
unsigned long long n=(unsigned long long)0;
{int i=0;for(0;i < len;++ i){
n=({unsigned long long _tmp181=n * (unsigned long long)base;_tmp181 + (unsigned long long)Cyc_Lex_int_of_char(*((char*)_check_fat_subscript(buff,sizeof(char),i)));});}}
if(n > (unsigned long long)-1){
if(declared_size &&(int)size == (int)2U)
({struct _fat_ptr _tmp182=({const char*_tmpA2="integer constant too large";_tag_fat(_tmpA2,sizeof(char),27U);});Cyc_Lex_err(_tmp182,lbuf);});
size=4U;}
# 399
if((int)size == (int)2U)
res=({union Cyc_Absyn_Cnst _tmp14C;(_tmp14C.Int_c).tag=5U,((_tmp14C.Int_c).val).f1=sn,((_tmp14C.Int_c).val).f2=(int)((unsigned)n);_tmp14C;});else{
# 402
res=({union Cyc_Absyn_Cnst _tmp14D;(_tmp14D.LongLong_c).tag=6U,((_tmp14D.LongLong_c).val).f1=sn,((_tmp14D.LongLong_c).val).f2=(long long)n;_tmp14D;});}}else{
# 405
long long n=(long long)0;
{int i=0;for(0;i < len;++ i){
n=({long long _tmp183=n * (long long)base;_tmp183 + (long long)Cyc_Lex_int_of_char(*((char*)_check_fat_subscript(buff,sizeof(char),i)));});}}{
unsigned long long x=(unsigned long long)n >> (unsigned long long)32;
if(x != (unsigned long long)-1 && x != (unsigned long long)0){
if(declared_size &&(int)size == (int)2U)
({struct _fat_ptr _tmp184=({const char*_tmpA3="integer constant too large";_tag_fat(_tmpA3,sizeof(char),27U);});Cyc_Lex_err(_tmp184,lbuf);});
size=4U;}
# 414
if((int)size == (int)2U)
res=({union Cyc_Absyn_Cnst _tmp14E;(_tmp14E.Int_c).tag=5U,((_tmp14E.Int_c).val).f1=sn,((_tmp14E.Int_c).val).f2=(int)n;_tmp14E;});else{
# 417
res=({union Cyc_Absyn_Cnst _tmp14F;(_tmp14F.LongLong_c).tag=6U,((_tmp14F.LongLong_c).val).f1=sn,((_tmp14F.LongLong_c).val).f2=n;_tmp14F;});}};}
# 420
return res;};}
# 425
char Cyc_Lex_string_buffer_v[11U]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _fat_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11U)};
int Cyc_Lex_string_pos=0;
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_fat_size(Cyc_Lex_string_buffer,sizeof(char))- (unsigned)1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
struct _fat_ptr str=({unsigned _tmpA5=(unsigned)newsz + 1U;char*_tmpA4=_cycalloc_atomic(_check_times(_tmpA5,sizeof(char)));({{unsigned _tmp150=(unsigned)newsz;unsigned i;for(i=0;i < _tmp150;++ i){i < (unsigned)sz?_tmpA4[i]=*((char*)_check_fat_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):(_tmpA4[i]='\000');}_tmpA4[_tmp150]=0;}0;});_tag_fat(_tmpA4,sizeof(char),_tmpA5);});
Cyc_Lex_string_buffer=str;};}
# 436
({struct _fat_ptr _tmpA6=_fat_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);char _tmpA7=*((char*)_check_fat_subscript(_tmpA6,sizeof(char),0U));char _tmpA8=c;if(_get_fat_size(_tmpA6,sizeof(char))== 1U &&(_tmpA7 == 0 && _tmpA8 != 0))_throw_arraybounds();*((char*)_tmpA6.curr)=_tmpA8;});
++ Cyc_Lex_string_pos;}
# 439
void Cyc_Lex_store_string(struct _fat_ptr s){
int sz=(int)Cyc_strlen((struct _fat_ptr)s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_fat_subscript(s,sizeof(char),i)));}}
# 444
struct _fat_ptr Cyc_Lex_get_stored_string(){
struct _fat_ptr str=Cyc_substring((struct _fat_ptr)Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;};
# 462 "lex.cyl"
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 464
static struct Cyc_Lex_Ldecls*Cyc_Lex_mk_empty_ldecls(int ignore){
return({struct Cyc_Lex_Ldecls*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_Set_Set*_tmp185=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpA9->typedefs=_tmp185;});_tmpA9;});}
# 468
static void Cyc_Lex_typedef_init(){
Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct Cyc_Binding_NSCtxt*_tmp186=((struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Lex_mk_empty_ldecls);_tmpAA->v=_tmp186;});_tmpAA;});}
# 472
static void Cyc_Lex_recompute_typedefs(){
# 474
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpAB=tdefs;struct Cyc_Lex_DynTrie*_tmpAC=Cyc_Lex_typedefs_trie;tdefs=_tmpAC;Cyc_Lex_typedefs_trie=_tmpAB;});{
struct Cyc_Lex_DynTrie _tmpAD=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmpAE=_tmpAD;struct Cyc_Core_DynamicRegion*_tmpBB;struct Cyc_Lex_Trie*_tmpBA;_LL1: _tmpBB=_tmpAE.dyn;_tmpBA=_tmpAE.t;_LL2:;
Cyc_Core_free_ukey(_tmpBB);{
struct Cyc_Core_NewDynamicRegion _tmpAF=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmpB0=_tmpAF;struct Cyc_Core_DynamicRegion*_tmpB9;_LL4: _tmpB9=_tmpB0.key;_LL5:;{
struct Cyc_Lex_Trie*t2=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmpB9,0,Cyc_Lex_empty_trie);
({struct Cyc_Lex_DynTrie _tmp187=({struct Cyc_Lex_DynTrie _tmp151;_tmp151.dyn=_tmpB9,_tmp151.t=t2;_tmp151;});*tdefs=_tmp187;});
({struct Cyc_Lex_DynTrie*_tmpB1=Cyc_Lex_typedefs_trie;struct Cyc_Lex_DynTrie*_tmpB2=tdefs;Cyc_Lex_typedefs_trie=_tmpB2;tdefs=_tmpB1;});{
# 484
struct Cyc_List_List*_tmpB3=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->availables;for(0;_tmpB3 != 0;_tmpB3=_tmpB3->tl){
void*_tmpB4=(void*)_tmpB3->hd;void*_tmpB5=_tmpB4;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpB5)->tag == 1U){_LL7: _tmpB7=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpB5)->f1;_LL8:
 _tmpB8=_tmpB7;goto _LLA;}else{_LL9: _tmpB8=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpB5)->f1;_LLA: {
# 488
struct Cyc_Lex_Ldecls*_tmpB6=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,_tmpB8);
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpB6->typedefs);
goto _LL6;}}_LL6:;}};};};};}
# 496
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _fat_ptr*v){
struct Cyc_List_List*ans=((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*))Cyc_Binding_resolve_rel_ns)(0U,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,ns);
struct Cyc_Lex_Ldecls*_tmpBC=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,ans);
return((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(_tmpBC->typedefs,v);}struct _tuple24{struct Cyc_List_List*f1;struct Cyc_Lex_Trie*f2;struct _fat_ptr f3;};
# 502
static int Cyc_Lex_is_typedef_body(struct _RegionHandle*d,struct _tuple24*env){
# 505
struct _tuple24 _tmpBD=*env;struct _tuple24 _tmpBE=_tmpBD;struct Cyc_List_List*_tmpC8;struct Cyc_Lex_Trie*_tmpC7;struct _fat_ptr _tmpC6;_LL1: _tmpC8=_tmpBE.f1;_tmpC7=_tmpBE.f2;_tmpC6=_tmpBE.f3;_LL2:;{
int len=(int)(_get_fat_size(_tmpC6,sizeof(char))- (unsigned)1);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmpBF=((struct Cyc_Lex_Trie*)_check_null(_tmpC7))->children;union Cyc_Lex_TrieChildren _tmpC0=_tmpBF;struct Cyc_Lex_Trie**_tmpC5;struct Cyc_Lex_Trie*_tmpC4;int _tmpC3;struct Cyc_Lex_Trie*_tmpC2;switch((_tmpC0.One).tag){case 1U: _LL4: _LL5:
 return 0;case 2U: _LL6: _tmpC3=((_tmpC0.One).val).f1;_tmpC2=((_tmpC0.One).val).f2;if(_tmpC3 != (int)*((const char*)_check_fat_subscript(_tmpC6,sizeof(char),i))){_LL7:
 return 0;}else{_LL8: _tmpC4=((_tmpC0.One).val).f2;_LL9:
 _tmpC7=_tmpC4;goto _LL3;}default: _LLA: _tmpC5=(_tmpC0.Many).val;_LLB: {
# 513
struct Cyc_Lex_Trie*_tmpC1=*((struct Cyc_Lex_Trie**)({typeof(_tmpC5 )_tmp188=_tmpC5;_check_known_subscript_notnull(_tmp188,64U,sizeof(struct Cyc_Lex_Trie*),Cyc_Lex_trie_char((int)((const char*)_tmpC6.curr)[i]));}));
if(_tmpC1 == 0)
return 0;
_tmpC7=_tmpC1;
goto _LL3;}}_LL3:;}}
# 519
return((struct Cyc_Lex_Trie*)_check_null(_tmpC7))->shared_str;};}
# 521
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _fat_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 526
struct _fat_ptr _tmpC9=*v;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpCA=tdefs;struct Cyc_Lex_DynTrie*_tmpCB=Cyc_Lex_typedefs_trie;tdefs=_tmpCB;Cyc_Lex_typedefs_trie=_tmpCA;});{
struct Cyc_Lex_DynTrie _tmpCC=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmpCD=_tmpCC;struct Cyc_Core_DynamicRegion*_tmpD3;struct Cyc_Lex_Trie*_tmpD2;_LL1: _tmpD3=_tmpCD.dyn;_tmpD2=_tmpCD.t;_LL2:;{
struct _tuple24 _tmpCE=({struct _tuple24 _tmp153;_tmp153.f1=ns,_tmp153.f2=_tmpD2,_tmp153.f3=_tmpC9;_tmp153;});
int _tmpCF=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple24*arg,int(*body)(struct _RegionHandle*h,struct _tuple24*arg)))Cyc_Core_open_region)(_tmpD3,& _tmpCE,Cyc_Lex_is_typedef_body);
({struct Cyc_Lex_DynTrie _tmp189=({struct Cyc_Lex_DynTrie _tmp152;_tmp152.dyn=_tmpD3,_tmp152.t=_tmpD2;_tmp152;});*tdefs=_tmp189;});
({struct Cyc_Lex_DynTrie*_tmpD0=tdefs;struct Cyc_Lex_DynTrie*_tmpD1=Cyc_Lex_typedefs_trie;tdefs=_tmpD1;Cyc_Lex_typedefs_trie=_tmpD0;});
return _tmpCF;};};};}
# 537
void Cyc_Lex_enter_namespace(struct _fat_ptr*s){
((void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_enter_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,s,1,Cyc_Lex_mk_empty_ldecls);{
struct Cyc_Lex_Ldecls*_tmpD4=({struct Cyc_Dict_Dict _tmp18A=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data;((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp18A,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);});
# 542
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpD4->typedefs);};}
# 544
void Cyc_Lex_leave_namespace(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
Cyc_Lex_recompute_typedefs();}
# 548
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*_tmpD5=((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(0U,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,q);
struct Cyc_Lex_Ldecls*_tmpD6=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,_tmpD5);
# 553
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,_tmpD6->typedefs);}
# 555
void Cyc_Lex_leave_using(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
# 558
Cyc_Lex_recompute_typedefs();}
# 561
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 563
struct Cyc_Lex_Ldecls*_tmpD7=({struct Cyc_Dict_Dict _tmp18B=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data;((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp18B,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);});
({struct Cyc_Set_Set*_tmp18C=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(_tmpD7->typedefs,(*q).f2);_tmpD7->typedefs=_tmp18C;});
# 568
Cyc_Lex_insert_typedef((*q).f2);}
# 571
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 575
if(symbol_num <= Cyc_Lex_num_kws){
# 577
if(!Cyc_Lex_in_extern_c ||(*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).is_c_keyword){
short res=(short)(*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).token_index;
return res;}}{
# 583
struct _fat_ptr*_tmpD8=Cyc_Lex_get_symbol(symbol_num);
Cyc_Lex_token_string=*_tmpD8;
# 587
if(Cyc_Lex_is_typedef(0,_tmpD8))
return 373;
# 591
return 365;};}
# 594
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
if(Cyc_Lex_in_extern_c)
({struct _fat_ptr _tmp18D=({const char*_tmpD9="qualified identifiers not allowed in C code";_tag_fat(_tmpD9,sizeof(char),44U);});Cyc_Lex_err(_tmp18D,lbuf);});{
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _fat_ptr s=lbuf->lex_buffer;
# 601
struct _fat_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 604
while(i < end){
int start=i;
for(0;i < end &&(int)*((char*)_check_fat_subscript(s,sizeof(char),i))!= (int)':';++ i){
;}
if(start == i)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp18E=({const char*_tmpDA="bad namespace";_tag_fat(_tmpDA,sizeof(char),14U);});_tmpDB->f1=_tmp18E;});_tmpDB;}));{
int vlen=i - start;
if(v != 0)
rev_vs=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=v,_tmpDC->tl=rev_vs;_tmpDC;});
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _fat_ptr)s,start,vlen));
i +=2;};}
# 616
if(v == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp18F=({const char*_tmpDD="bad namespace";_tag_fat(_tmpDD,sizeof(char),14U);});_tmpDE->f1=_tmp18F;});_tmpDE;}));{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 621
if(vs != 0 &&({struct _fat_ptr _tmp190=(struct _fat_ptr)*((struct _fat_ptr*)vs->hd);Cyc_strcmp(_tmp190,({const char*_tmpDF="Cyc";_tag_fat(_tmpDF,sizeof(char),4U);}));})== 0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_tmpE0=_cycalloc(sizeof(*_tmpE0));({union Cyc_Absyn_Nmspace _tmp191=Cyc_Absyn_Abs_n(vs,0);_tmpE0->f1=_tmp191;}),_tmpE0->f2=v;_tmpE0;});}else{
# 625
if(vs != 0 &&({struct _fat_ptr _tmp192=(struct _fat_ptr)*((struct _fat_ptr*)vs->hd);Cyc_strcmp(_tmp192,({const char*_tmpE1="C";_tag_fat(_tmpE1,sizeof(char),2U);}));})== 0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_tmpE2=_cycalloc(sizeof(*_tmpE2));({union Cyc_Absyn_Nmspace _tmp193=Cyc_Absyn_Abs_n(vs,1);_tmpE2->f1=_tmp193;}),_tmpE2->f2=v;_tmpE2;});}else{
# 629
Cyc_Lex_token_qvar=({struct _tuple0*_tmpE3=_cycalloc(sizeof(*_tmpE3));({union Cyc_Absyn_Nmspace _tmp194=Cyc_Absyn_Rel_n(vs);_tmpE3->f1=_tmp194;}),_tmpE3->f2=v;_tmpE3;});}}
if(Cyc_Lex_is_typedef(vs,v))
return 375;
return 374;};};}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned starting_line;struct _fat_ptr filename;struct _fat_ptr linenumpos;unsigned linenumpos_offset;};
# 648
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 650
static int Cyc_Lex_linenumber=1;
# 653
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _fat_ptr filename,unsigned starting_line,struct Cyc_Lex_PosInfo*next){
# 657
struct _fat_ptr linenumpos=({unsigned _tmpE5=10U;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(unsigned),_tmpE5),sizeof(unsigned),_tmpE5);});
*((unsigned*)_check_fat_subscript(linenumpos,sizeof(unsigned),0))=(unsigned)Cyc_yylloc.first_line;
return({struct Cyc_Lex_PosInfo*_tmpE4=_region_malloc(r,sizeof(*_tmpE4));_tmpE4->next=next,_tmpE4->starting_line=starting_line,_tmpE4->filename=(struct _fat_ptr)filename,_tmpE4->linenumpos=linenumpos,_tmpE4->linenumpos_offset=1U;_tmpE4;});}
# 666
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0)(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp195=({const char*_tmpE6="empty position info!";_tag_fat(_tmpE6,sizeof(char),21U);});_tmpE7->f1=_tmp195;});_tmpE7;}));{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _fat_ptr linenums=_tag_fat(0,0,0);
({struct _fat_ptr _tmpE8=p->linenumpos;struct _fat_ptr _tmpE9=linenums;p->linenumpos=_tmpE9;linenums=_tmpE8;});{
unsigned offset=p->linenumpos_offset;
unsigned n=_get_fat_size(linenums,sizeof(unsigned));
# 674
if(offset >= n){
# 676
struct _fat_ptr newlinenums=({unsigned _tmpEC=n * (unsigned)2;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(unsigned),_tmpEC),sizeof(unsigned),_tmpEC);});
{unsigned i=0U;for(0;i < n;++ i){
*((unsigned*)_check_fat_subscript(newlinenums,sizeof(unsigned),(int)i))=((unsigned*)linenums.curr)[(int)i];}}
({struct _fat_ptr _tmpEA=linenums;struct _fat_ptr _tmpEB=newlinenums;linenums=_tmpEB;newlinenums=_tmpEA;});
# 681
((void(*)(unsigned*ptr))Cyc_Core_ufree)((unsigned*)_untag_fat_ptr(newlinenums,sizeof(unsigned),1U));}
# 684
*((unsigned*)_check_fat_subscript(linenums,sizeof(unsigned),(int)offset))=(unsigned)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + (unsigned)1;
++ Cyc_Lex_linenumber;
({struct _fat_ptr _tmpED=p->linenumpos;struct _fat_ptr _tmpEE=linenums;p->linenumpos=_tmpEE;linenums=_tmpED;});};};}
# 691
static void Cyc_Lex_process_directive(struct _fat_ptr line){
int i;
char buf[100U];
struct _fat_ptr filename=({const char*_tmpFC="";_tag_fat(_tmpFC,sizeof(char),1U);});
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmpF1=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp155;_tmp155.tag=2U,_tmp155.f1=& i;_tmp155;});struct Cyc_CharPtr_sa_ScanfArg_struct _tmpF2=({struct Cyc_CharPtr_sa_ScanfArg_struct _tmp154;_tmp154.tag=7U,({struct _fat_ptr _tmp196=_tag_fat(buf,sizeof(char),100U);_tmp154.f1=_tmp196;});_tmp154;});void*_tmpEF[2U];_tmpEF[0]=& _tmpF1,_tmpEF[1]=& _tmpF2;({struct _fat_ptr _tmp198=(struct _fat_ptr)line;struct _fat_ptr _tmp197=({const char*_tmpF0="# %d \"%s";_tag_fat(_tmpF0,sizeof(char),9U);});Cyc_sscanf(_tmp198,_tmp197,_tag_fat(_tmpEF,sizeof(void*),2U));});})== 2){
if(Cyc_Lex_compile_for_boot_r){
# 699
int i=(int)(Cyc_strlen(_tag_fat(buf,sizeof(char),100U))- (unsigned long)1);
int last_slash=-1;
while(i >= 0){
if((int)*((char*)_check_known_subscript_notnull(buf,100U,sizeof(char),i))== (int)'/'){last_slash=i;break;}
-- i;}
# 705
filename=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U,({struct _fat_ptr _tmp19A=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(({struct _fat_ptr _tmp199=_tag_fat(buf,sizeof(char),100U);_fat_ptr_plus(_tmp199,sizeof(char),last_slash);}),sizeof(char),1));_tmp156.f1=_tmp19A;});_tmp156;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({struct _fat_ptr _tmp19B=({const char*_tmpF4="\"%s";_tag_fat(_tmpF4,sizeof(char),4U);});Cyc_aprintf(_tmp19B,_tag_fat(_tmpF3,sizeof(void*),1U));});});}else{
# 707
filename=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF8=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U,({struct _fat_ptr _tmp19C=(struct _fat_ptr)_tag_fat(buf,sizeof(char),100U);_tmp157.f1=_tmp19C;});_tmp157;});void*_tmpF6[1U];_tmpF6[0]=& _tmpF8;({struct _fat_ptr _tmp19D=({const char*_tmpF7="\"%s";_tag_fat(_tmpF7,sizeof(char),4U);});Cyc_aprintf(_tmp19D,_tag_fat(_tmpF6,sizeof(void*),1U));});});}
if((Cyc_Lex_linenumber == i && Cyc_Lex_pos_info != 0)&& Cyc_strcmp((struct _fat_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _fat_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmpFB=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp158;_tmp158.tag=2U,_tmp158.f1=& i;_tmp158;});void*_tmpF9[1U];_tmpF9[0]=& _tmpFB;({struct _fat_ptr _tmp19F=(struct _fat_ptr)line;struct _fat_ptr _tmp19E=({const char*_tmpFA="# %d";_tag_fat(_tmpFA,sizeof(char),5U);});Cyc_sscanf(_tmp19F,_tmp19E,_tag_fat(_tmpF9,sizeof(void*),1U));});})== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 715
++ Cyc_Lex_linenumber;
return;}}
# 718
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple25{struct _fat_ptr f1;unsigned f2;};
# 724
struct _tuple25 Cyc_Lex_xlate_pos(unsigned char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p != 0;p=p->next){
unsigned first_char_offset=*((unsigned*)_check_fat_subscript(p->linenumpos,sizeof(unsigned),0));
if(char_offset < first_char_offset && p->next != 0)continue;{
# 730
unsigned base=0U;
unsigned size=p->linenumpos_offset;
while(size > (unsigned)1){
int half=(int)(size / (unsigned)2);
int mid=(int)(base + (unsigned)half);
if(char_offset > *((unsigned*)_check_fat_subscript(p->linenumpos,sizeof(unsigned),mid))){
base=base + (unsigned)half;
size=size - (unsigned)half;}else{
# 739
size=(unsigned)half;}}
# 742
return({struct _tuple25 _tmp159;_tmp159.f1=p->filename,_tmp159.f2=p->starting_line + base;_tmp159;});};}}
# 744
return({struct _tuple25 _tmp15A;({struct _fat_ptr _tmp1A0=_tag_fat(0,0,0);_tmp15A.f1=_tmp1A0;}),_tmp15A.f2=0U;_tmp15A;});}
# 749
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*);
# 761
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 763
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);
({int _tmp1A2=({int _tmp1A1=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf);yyllocptr->first_line=_tmp1A1;});Cyc_yylloc.first_line=_tmp1A2;});
({int _tmp1A4=({int _tmp1A3=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf);yyllocptr->last_line=_tmp1A3;});Cyc_yylloc.last_line=_tmp1A4;});
{int _tmpFD=ans;switch(_tmpFD){case 365U: _LL1: _LL2:
# 768
({union Cyc_YYSTYPE _tmp1A5=({union Cyc_YYSTYPE _tmp15B;(_tmp15B.String_tok).tag=3U,(_tmp15B.String_tok).val=Cyc_Lex_token_string;_tmp15B;});*yylval=_tmp1A5;});goto _LL0;case 373U: _LL3: _LL4:
# 770
({union Cyc_YYSTYPE _tmp1A6=({union Cyc_YYSTYPE _tmp15C;(_tmp15C.String_tok).tag=3U,(_tmp15C.String_tok).val=Cyc_Lex_token_string;_tmp15C;});*yylval=_tmp1A6;});goto _LL0;case 374U: _LL5: _LL6:
# 772
({union Cyc_YYSTYPE _tmp1A7=({union Cyc_YYSTYPE _tmp15D;(_tmp15D.QualId_tok).tag=5U,(_tmp15D.QualId_tok).val=Cyc_Lex_token_qvar;_tmp15D;});*yylval=_tmp1A7;});goto _LL0;case 375U: _LL7: _LL8:
# 774
({union Cyc_YYSTYPE _tmp1A8=({union Cyc_YYSTYPE _tmp15E;(_tmp15E.QualId_tok).tag=5U,(_tmp15E.QualId_tok).val=Cyc_Lex_token_qvar;_tmp15E;});*yylval=_tmp1A8;});goto _LL0;case 372U: _LL9: _LLA:
# 776
({union Cyc_YYSTYPE _tmp1A9=({union Cyc_YYSTYPE _tmp15F;(_tmp15F.String_tok).tag=3U,(_tmp15F.String_tok).val=Cyc_Lex_token_string;_tmp15F;});*yylval=_tmp1A9;});goto _LL0;case 366U: _LLB: _LLC:
# 778
({union Cyc_YYSTYPE _tmp1AA=({union Cyc_YYSTYPE _tmp160;(_tmp160.Int_tok).tag=1U,(_tmp160.Int_tok).val=Cyc_Lex_token_int;_tmp160;});*yylval=_tmp1AA;});goto _LL0;case 369U: _LLD: _LLE:
# 780
({union Cyc_YYSTYPE _tmp1AB=({union Cyc_YYSTYPE _tmp161;(_tmp161.Char_tok).tag=2U,(_tmp161.Char_tok).val=Cyc_Lex_token_char;_tmp161;});*yylval=_tmp1AB;});goto _LL0;case 370U: _LLF: _LL10:
# 782
({union Cyc_YYSTYPE _tmp1AC=({union Cyc_YYSTYPE _tmp162;(_tmp162.String_tok).tag=3U,(_tmp162.String_tok).val=Cyc_Lex_token_string;_tmp162;});*yylval=_tmp1AC;});goto _LL0;case 371U: _LL11: _LL12:
# 784
({union Cyc_YYSTYPE _tmp1AD=({union Cyc_YYSTYPE _tmp163;(_tmp163.String_tok).tag=3U,(_tmp163.String_tok).val=Cyc_Lex_token_string;_tmp163;});*yylval=_tmp1AD;});goto _LL0;case 367U: _LL13: _LL14:
# 786
({union Cyc_YYSTYPE _tmp1AE=({union Cyc_YYSTYPE _tmp164;(_tmp164.String_tok).tag=3U,(_tmp164.String_tok).val=Cyc_Lex_token_string;_tmp164;});*yylval=_tmp1AE;});goto _LL0;case 368U: _LL15: _LL16:
# 788
({union Cyc_YYSTYPE _tmp1AF=({union Cyc_YYSTYPE _tmp165;(_tmp165.String_tok).tag=3U,(_tmp165.String_tok).val=Cyc_Lex_token_string;_tmp165;});*yylval=_tmp1AF;});goto _LL0;case 377U: _LL17: _LL18:
# 790
({union Cyc_YYSTYPE _tmp1B0=({union Cyc_YYSTYPE _tmp166;(_tmp166.Asm_tok).tag=6U,(_tmp166.Asm_tok).val=Cyc_Lex_token_asm;_tmp166;});*yylval=_tmp1B0;});goto _LL0;default: _LL19: _LL1A:
 goto _LL0;}_LL0:;}
# 793
return ans;}
# 800
const int Cyc_Lex_lex_base[322U]={0,113,119,120,125,126,127,132,136,- 8,- 7,4,- 1,5,- 4,- 5,2,- 6,- 2,6,- 3,7,- 7,175,- 1,30,16,23,50,32,45,47,55,181,40,48,53,65,47,60,58,66,73,78,187,169,10,12,21,168,- 4,194,- 1,186,- 5,259,287,345,267,198,- 4,- 3,- 2,11,2,199,- 17,213,- 1,422,- 18,12,- 12,- 11,275,- 13,- 10,- 9,495,518,310,- 14,240,- 17,13,- 1,- 15,- 16,14,- 1,573,368,646,721,438,- 16,- 69,233,- 49,15,30,- 51,16,31,142,171,32,163,169,449,206,798,841,168,197,114,190,407,911,1000,128,1075,1133,1208,156,- 68,- 33,- 39,1283,137,1358,1433,1491,1566,1641,164,1716,1791,1866,1941,2016,2091,2166,2241,- 34,- 52,- 53,144,146,158,145,169,154,154,160,160,176,165,172,2316,179,180,177,2391,186,191,194,2476,264,190,180,193,185,200,188,2551,- 15,220,223,2636,229,238,226,247,241,234,251,2711,271,272,263,271,2796,263,273,264,268,264,289,277,294,2871,287,297,298,2956,291,3031,293,293,325,318,314,325,333,325,333,3116,319,323,337,324,3191,- 25,379,- 37,- 22,380,- 24,- 42,- 36,- 45,390,- 46,3266,3295,589,- 21,571,373,375,- 20,367,808,3305,3335,3369,3409,573,386,3479,3517,795,402,403,395,431,437,796,449,450,- 19,442,797,500,501,- 18,493,- 50,450,- 31,- 49,17,407,3449,- 44,- 27,- 29,- 43,- 26,- 28,- 30,1,3557,2,418,550,419,420,425,426,427,428,429,443,474,3630,3714,- 66,- 60,- 59,- 58,- 57,- 56,- 55,- 54,- 61,- 64,- 65,880,476,- 62,- 63,- 67,- 38,- 35,- 32,524,- 47,18,- 23,641};
const int Cyc_Lex_lex_backtrk[322U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,7,- 1,- 1,7,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,5,3,4,2,- 1,2,- 1,5,- 1,2,- 1,2,2,2,- 1,- 1,- 1,1,3,15,- 1,15,- 1,18,- 1,1,- 1,- 1,13,- 1,- 1,- 1,- 1,14,13,- 1,15,- 1,1,- 1,- 1,- 1,14,- 1,17,12,- 1,13,12,- 1,- 1,47,- 1,48,68,- 1,68,68,68,68,68,68,68,68,68,17,19,68,68,68,68,68,1,1,68,68,68,1,68,- 1,- 1,- 1,1,- 1,1,0,- 1,2,2,- 1,3,1,1,1,1,1,1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,- 1,39,- 1,- 1,- 1,- 1,- 1,- 1,20,19,- 1,- 1,19,19,19,- 1,19,- 1,20,20,17,18,17,17,- 1,16,16,16,16,16,17,17,18,18,18,- 1,18,17,17,17,- 1,17,- 1,- 1,- 1,- 1,48,- 1,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,46,- 1,47};
const int Cyc_Lex_lex_default[322U]={96,86,60,65,60,49,45,- 1,9,0,0,- 1,0,19,0,0,- 1,0,0,19,0,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,45,- 1,- 1,- 1,59,0,59,0,- 1,0,- 1,- 1,- 1,- 1,59,0,0,0,- 1,- 1,82,0,82,0,- 1,0,- 1,0,0,- 1,0,0,0,- 1,- 1,- 1,0,82,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,- 1,0,- 1,- 1,0,317,- 1,- 1,282,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,22,- 1,- 1,- 1,72,- 1,- 1,- 1,54,- 1,- 1,- 1,- 1,- 1,- 1,- 1,176,0,- 1,- 1,76,- 1,- 1,- 1,- 1,- 1,- 1,- 1,73,- 1,- 1,- 1,- 1,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,9,- 1,- 1,- 1,17,- 1,75,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,77,- 1,- 1,- 1,- 1,95,0,- 1,0,0,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,0,- 1,0,269,0,0,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,317,0,- 1,0,- 1};
const int Cyc_Lex_lex_trans[3971U]={0,0,0,0,0,0,0,0,0,97,98,97,97,99,22,20,20,20,21,21,17,18,18,18,95,271,318,271,318,319,0,0,97,100,101,102,24,103,104,105,313,312,106,107,18,108,109,110,111,112,112,112,112,112,112,112,112,112,113,18,114,115,116,24,117,118,118,118,118,118,118,118,118,118,118,118,119,118,118,118,118,118,118,118,118,118,118,118,118,118,118,320,316,281,120,121,122,123,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,87,124,34,88,27,61,62,66,28,84,67,61,62,50,17,63,51,46,23,23,23,23,23,10,29,30,11,31,61,89,85,68,32,33,61,35,36,52,37,38,39,23,40,41,42,43,47,12,13,24,44,48,226,14,15,- 1,- 1,314,- 1,- 1,16,23,23,23,23,23,144,33,33,33,33,33,132,44,44,44,44,44,64,- 1,315,60,90,279,23,- 1,- 1,- 1,- 1,69,33,276,24,- 1,126,53,44,18,18,132,83,280,18,231,20,- 1,232,277,278,- 1,- 1,55,55,55,55,55,55,55,55,321,206,321,321,168,- 1,268,18,- 1,223,224,269,208,153,125,227,228,229,- 1,- 1,- 1,283,209,321,160,270,154,180,25,155,- 1,161,- 1,181,156,157,18,158,127,159,164,18,18,162,- 1,163,18,165,- 1,- 1,166,26,167,177,18,171,172,173,18,174,18,175,18,- 1,56,58,58,58,58,58,58,58,58,20,20,20,20,20,20,20,20,80,80,80,80,80,80,80,80,178,- 1,179,188,57,57,57,57,57,57,57,57,57,57,193,182,183,184,185,186,194,57,57,57,57,57,57,81,81,81,81,81,81,81,81,187,169,189,83,190,191,192,202,195,- 1,70,170,196,197,198,- 1,54,20,57,57,57,57,57,57,199,200,17,57,57,57,57,57,57,57,57,57,57,201,203,204,205,207,218,210,57,57,57,57,57,57,94,94,94,94,94,94,94,94,- 1,- 1,211,- 1,212,213,214,215,18,216,217,71,219,220,221,222,225,230,57,57,57,57,57,57,233,240,- 1,241,241,275,- 1,- 1,72,311,307,306,271,73,256,272,305,304,303,302,301,- 1,74,74,74,74,74,74,74,74,254,83,83,240,300,241,241,75,75,75,75,75,75,75,75,75,256,273,- 1,274,274,274,274,274,274,274,274,274,274,70,147,148,254,83,83,299,76,308,0,149,0,20,60,150,70,151,54,260,261,261,0,152,0,0,17,0,318,0,22,319,9,70,77,0,78,79,79,79,79,79,79,79,79,79,79,0,70,0,0,260,261,261,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,79,265,266,266,0,0,0,0,79,79,79,79,79,79,308,0,0,79,79,79,79,79,79,309,309,309,309,309,309,309,309,0,73,265,266,266,0,76,0,0,79,79,79,79,79,79,91,91,91,91,91,91,91,91,0,0,0,243,0,243,0,72,244,244,244,244,244,244,244,244,244,244,242,0,257,321,0,321,321,0,0,241,0,70,0,0,0,0,0,0,77,0,0,0,0,18,20,0,321,0,60,0,0,0,242,0,257,0,54,0,0,0,17,241,22,70,9,0,92,93,93,93,93,93,93,93,93,93,93,0,0,- 1,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,- 1,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,234,0,246,246,246,246,246,246,246,246,247,247,244,244,244,244,244,244,244,244,244,244,0,236,237,0,0,255,262,267,248,0,0,0,0,0,83,261,266,249,0,0,250,234,0,235,235,235,235,235,235,235,235,235,235,236,237,0,0,255,262,267,248,0,0,0,236,237,83,261,266,249,0,238,250,308,0,0,0,0,0,0,239,0,310,310,310,310,310,310,310,310,0,0,0,0,0,0,236,237,0,0,0,0,0,238,0,0,0,0,0,0,0,0,239,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,145,0,0,0,0,146,0,0,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,138,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,136,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,130,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,131,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,129,0,0,0,0,0,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,0,0,0,0,128,0,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,134,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,135,0,0,0,0,0,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,0,0,0,0,133,0,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,0,0,0,0,0,0,0,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,0,0,0,0,134,0,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,136,136,136,136,136,136,136,136,136,136,0,0,0,0,0,0,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,0,0,0,0,136,0,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,139,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,140,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,141,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,142,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,143,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,0,0,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,0,0,0,0,137,0,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,245,245,245,245,245,245,245,245,245,245,0,0,0,0,0,0,0,0,0,0,0,236,237,0,0,0,0,234,237,235,235,235,235,235,235,235,235,235,235,244,244,244,244,244,244,244,244,244,244,0,236,237,0,236,237,0,0,238,- 1,0,237,237,0,0,0,0,239,237,0,245,245,245,245,245,245,245,245,245,245,0,0,0,236,237,0,0,0,0,0,238,236,237,0,237,0,0,0,237,239,237,0,234,0,246,246,246,246,246,246,246,246,247,247,0,0,0,0,0,0,0,0,0,236,237,236,237,0,0,0,237,0,263,0,- 1,0,0,0,0,0,0,264,234,0,247,247,247,247,247,247,247,247,247,247,0,0,0,236,237,0,0,0,0,0,263,236,237,0,0,0,0,0,258,264,0,0,0,0,0,0,0,259,0,0,274,274,274,274,274,274,274,274,274,274,0,0,0,236,237,0,0,0,0,0,258,236,237,0,0,0,0,0,237,259,251,251,251,251,251,251,251,251,251,251,0,0,0,0,0,0,0,251,251,251,251,251,251,236,237,0,0,0,0,0,237,0,0,0,0,0,0,0,251,251,251,251,251,251,251,251,251,251,0,251,251,251,251,251,251,251,251,251,251,251,251,0,0,0,284,0,252,0,0,285,0,0,0,0,0,253,0,0,286,286,286,286,286,286,286,286,0,251,251,251,251,251,251,287,0,0,0,0,252,0,0,0,0,0,0,0,0,253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,0,0,0,0,289,290,0,0,0,291,0,0,0,0,0,0,0,292,0,0,0,293,0,294,0,295,0,296,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,298,0,0,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[3971U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,11,13,19,21,13,19,46,63,71,84,88,99,102,272,319,102,- 1,- 1,0,0,0,0,64,0,0,0,282,284,0,0,16,0,0,0,0,0,0,0,0,0,0,0,0,0,0,47,0,0,0,48,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,100,103,106,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,25,1,26,2,2,3,27,2,3,4,4,5,6,4,5,6,7,7,7,7,7,8,28,29,8,30,2,1,2,3,31,32,4,34,35,5,36,37,38,7,39,40,41,42,6,8,8,7,43,6,115,8,8,49,45,104,105,45,8,23,23,23,23,23,120,33,33,33,33,33,129,44,44,44,44,44,4,49,104,51,1,107,23,59,65,105,45,3,33,108,23,45,124,5,44,53,33,135,67,107,53,113,44,51,113,108,108,59,65,53,53,53,53,53,53,53,53,97,148,97,97,150,67,110,53,82,116,116,110,147,152,0,114,114,114,49,13,19,105,147,97,151,110,153,149,23,154,102,151,82,149,155,156,53,157,124,158,160,53,53,161,51,162,53,164,59,65,165,23,166,169,53,170,171,172,53,173,53,174,53,67,53,55,55,55,55,55,55,55,55,58,58,58,58,58,58,58,58,74,74,74,74,74,74,74,74,177,82,178,182,56,56,56,56,56,56,56,56,56,56,180,181,181,183,184,185,180,56,56,56,56,56,56,80,80,80,80,80,80,80,80,186,168,188,1,189,190,191,193,194,2,3,168,195,196,197,4,5,6,56,56,56,56,56,56,198,199,8,57,57,57,57,57,57,57,57,57,57,200,202,203,204,206,208,209,57,57,57,57,57,57,91,91,91,91,91,91,91,91,49,45,210,105,211,212,213,214,69,215,216,69,218,219,220,221,224,227,57,57,57,57,57,57,232,239,51,240,242,273,59,65,69,285,287,288,269,69,249,269,289,290,291,292,293,67,69,69,69,69,69,69,69,69,253,254,255,239,294,240,242,69,94,94,94,94,94,94,94,94,249,109,82,109,109,109,109,109,109,109,109,109,109,256,117,117,253,254,255,295,69,310,- 1,117,- 1,69,69,117,257,117,69,259,260,262,- 1,117,- 1,- 1,69,- 1,317,- 1,69,317,69,256,69,- 1,69,78,78,78,78,78,78,78,78,78,78,- 1,257,- 1,- 1,259,260,262,78,78,78,78,78,78,79,79,79,79,79,79,79,79,79,79,264,265,267,- 1,- 1,- 1,- 1,79,79,79,79,79,79,286,- 1,- 1,78,78,78,78,78,78,286,286,286,286,286,286,286,286,- 1,90,264,265,267,- 1,90,- 1,- 1,79,79,79,79,79,79,90,90,90,90,90,90,90,90,- 1,- 1,- 1,236,- 1,236,- 1,90,236,236,236,236,236,236,236,236,236,236,238,- 1,248,321,- 1,321,321,- 1,- 1,238,- 1,248,- 1,- 1,- 1,- 1,- 1,- 1,90,- 1,- 1,- 1,- 1,90,90,- 1,321,- 1,90,- 1,- 1,- 1,238,- 1,248,- 1,90,- 1,- 1,- 1,90,238,90,248,90,- 1,90,92,92,92,92,92,92,92,92,92,92,- 1,- 1,269,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,93,93,93,93,93,93,93,93,- 1,317,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,111,- 1,111,111,111,111,111,111,111,111,111,111,243,243,243,243,243,243,243,243,243,243,- 1,111,111,- 1,- 1,252,258,263,111,- 1,- 1,- 1,- 1,- 1,252,258,263,111,- 1,- 1,111,112,- 1,112,112,112,112,112,112,112,112,112,112,111,111,- 1,- 1,252,258,263,111,- 1,- 1,- 1,112,112,252,258,263,111,- 1,112,111,309,- 1,- 1,- 1,- 1,- 1,- 1,112,- 1,309,309,309,309,309,309,309,309,- 1,- 1,- 1,- 1,- 1,- 1,112,112,- 1,- 1,- 1,- 1,- 1,112,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,112,118,118,118,118,118,118,118,118,118,118,118,- 1,- 1,- 1,- 1,- 1,- 1,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,- 1,- 1,- 1,- 1,118,- 1,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,118,119,- 1,- 1,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,119,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,- 1,- 1,- 1,- 1,121,- 1,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,121,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,- 1,- 1,- 1,- 1,122,- 1,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,122,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,- 1,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,- 1,- 1,- 1,- 1,128,- 1,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,130,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,- 1,- 1,- 1,- 1,130,- 1,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,130,131,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,- 1,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,- 1,- 1,- 1,- 1,131,- 1,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,131,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,- 1,- 1,- 1,- 1,132,- 1,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,132,133,133,133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,- 1,- 1,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,- 1,- 1,- 1,- 1,133,- 1,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,133,134,134,134,134,134,134,134,134,134,134,- 1,- 1,- 1,- 1,- 1,- 1,- 1,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,- 1,- 1,- 1,- 1,134,- 1,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,134,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,- 1,- 1,- 1,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,- 1,- 1,- 1,- 1,136,- 1,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,136,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,137,- 1,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,137,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,138,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,- 1,- 1,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,- 1,- 1,- 1,- 1,139,- 1,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,139,140,140,140,140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,- 1,- 1,- 1,- 1,140,- 1,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,140,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,- 1,- 1,- 1,- 1,141,- 1,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,141,142,142,142,142,142,142,142,142,142,142,- 1,- 1,- 1,- 1,- 1,- 1,- 1,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,- 1,- 1,- 1,- 1,142,- 1,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,142,143,143,143,143,143,143,143,143,143,143,- 1,- 1,- 1,- 1,- 1,- 1,- 1,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,- 1,- 1,- 1,- 1,143,- 1,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,143,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,- 1,- 1,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,- 1,- 1,- 1,- 1,159,- 1,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,159,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,- 1,- 1,- 1,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,163,- 1,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,163,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,167,167,167,167,167,167,167,167,167,- 1,- 1,- 1,- 1,- 1,- 1,- 1,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,- 1,- 1,- 1,- 1,167,159,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,167,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,- 1,- 1,- 1,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,175,163,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,175,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,179,179,179,179,179,179,179,179,179,179,- 1,- 1,- 1,- 1,- 1,- 1,- 1,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,- 1,- 1,- 1,- 1,179,167,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,179,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,- 1,- 1,- 1,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,187,175,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,- 1,- 1,- 1,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,- 1,- 1,- 1,- 1,192,179,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,192,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,- 1,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,201,187,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,201,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,205,205,205,205,205,205,205,205,205,205,- 1,- 1,- 1,- 1,- 1,- 1,- 1,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,- 1,- 1,- 1,- 1,205,192,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,205,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,- 1,- 1,- 1,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,207,201,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,207,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,217,217,217,217,217,217,217,217,217,217,- 1,- 1,- 1,- 1,- 1,- 1,- 1,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,- 1,- 1,- 1,- 1,217,205,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,217,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,- 1,- 1,- 1,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,- 1,- 1,- 1,- 1,222,207,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,222,234,234,234,234,234,234,234,234,234,234,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,234,234,- 1,- 1,- 1,- 1,235,234,235,235,235,235,235,235,235,235,235,235,244,244,244,244,244,244,244,244,244,244,- 1,235,235,- 1,234,234,- 1,- 1,235,217,- 1,234,244,- 1,- 1,- 1,- 1,235,244,- 1,245,245,245,245,245,245,245,245,245,245,- 1,- 1,- 1,235,235,- 1,- 1,- 1,- 1,- 1,235,245,245,- 1,244,- 1,- 1,- 1,245,235,244,- 1,246,- 1,246,246,246,246,246,246,246,246,246,246,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,245,245,246,246,- 1,- 1,- 1,245,- 1,246,- 1,222,- 1,- 1,- 1,- 1,- 1,- 1,246,247,- 1,247,247,247,247,247,247,247,247,247,247,- 1,- 1,- 1,246,246,- 1,- 1,- 1,- 1,- 1,246,247,247,- 1,- 1,- 1,- 1,- 1,247,246,- 1,- 1,- 1,- 1,- 1,- 1,- 1,247,- 1,- 1,274,274,274,274,274,274,274,274,274,274,- 1,- 1,- 1,247,247,- 1,- 1,- 1,- 1,- 1,247,274,274,- 1,- 1,- 1,- 1,- 1,274,247,250,250,250,250,250,250,250,250,250,250,- 1,- 1,- 1,- 1,- 1,- 1,- 1,250,250,250,250,250,250,274,274,- 1,- 1,- 1,- 1,- 1,274,- 1,- 1,- 1,- 1,- 1,- 1,- 1,251,251,251,251,251,251,251,251,251,251,- 1,250,250,250,250,250,250,251,251,251,251,251,251,- 1,- 1,- 1,283,- 1,251,- 1,- 1,283,- 1,- 1,- 1,- 1,- 1,251,- 1,- 1,283,283,283,283,283,283,283,283,- 1,251,251,251,251,251,251,283,- 1,- 1,- 1,- 1,251,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,251,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,283,- 1,- 1,- 1,- 1,283,283,- 1,- 1,- 1,283,- 1,- 1,- 1,- 1,- 1,- 1,- 1,283,- 1,- 1,- 1,283,- 1,283,- 1,283,- 1,283,296,296,296,296,296,296,296,296,296,296,- 1,- 1,- 1,- 1,- 1,- 1,- 1,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,- 1,- 1,- 1,- 1,- 1,- 1,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,296,297,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,297,297,297,297,297,297,297,297,297,297,- 1,- 1,- 1,- 1,- 1,- 1,- 1,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,- 1,- 1,- 1,- 1,- 1,- 1,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,297,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 807
int state;int base;int backtrk;
int c;
state=start_state;
# 811
if(state >= 0){
({int _tmp1B1=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp1B1;});
lbuf->lex_last_action=- 1;}else{
# 815
state=(- state)- 1;}
# 817
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_base,322U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 825
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 829
c=256;}}else{
# 831
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 834
if(*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_check,3971U,sizeof(int),base + c))== state)
state=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_trans,3971U,sizeof(int),base + c));else{
# 837
state=Cyc_Lex_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1B2=({const char*_tmpFE="empty token";_tag_fat(_tmpFE,sizeof(char),12U);});_tmpFF->f1=_tmp1B2;});_tmpFF;}));else{
# 843
return lbuf->lex_last_action;}}else{
# 846
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 850
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp100=lexstate;switch(_tmp100){case 0U: _LL1: _LL2:
# 809 "lex.cyl"
 return Cyc_Lex_snarf_asm(lexbuf);case 1U: _LL3: _LL4:
# 811
 return(int)Cyc_Lex_process_id(lexbuf);case 2U: _LL5: _LL6:
# 814 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 3U: _LL7: _LL8:
# 817 "lex.cyl"
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 372;case 4U: _LL9: _LLA:
# 824 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 339;case 5U: _LLB: _LLC:
# 825 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 332;case 6U: _LLD: _LLE:
# 826 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 338;case 7U: _LLF: _LL10:
# 827 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 337;case 8U: _LL11: _LL12:
# 828 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 340;case 9U: _LL13: _LL14:
# 829 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 336;case 10U: _LL15: _LL16:
# 830 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 329;case 11U: _LL17: _LL18:
# 831 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 330;case 12U: _LL19: _LL1A:
# 832 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 331;case 13U: _LL1B: _LL1C:
# 833 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 333;case 14U: _LL1D: _LL1E:
# 834 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 334;case 15U: _LL1F: _LL20:
# 835 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 335;case 16U: _LL21: _LL22:
# 839 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 366;case 17U: _LL23: _LL24:
# 842
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 366;case 18U: _LL25: _LL26:
# 848 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 366;case 19U: _LL27: _LL28:
# 851
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 366;case 20U: _LL29: _LL2A:
# 856 "lex.cyl"
 Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);
return 371;case 21U: _LL2B: _LL2C:
# 859
 return 348;case 22U: _LL2D: _LL2E:
# 860 "lex.cyl"
 return 349;case 23U: _LL2F: _LL30:
# 861 "lex.cyl"
 return 346;case 24U: _LL31: _LL32:
# 862 "lex.cyl"
 return 347;case 25U: _LL33: _LL34:
# 863 "lex.cyl"
 return 342;case 26U: _LL35: _LL36:
# 864 "lex.cyl"
 return 343;case 27U: _LL37: _LL38:
# 865 "lex.cyl"
 return 355;case 28U: _LL39: _LL3A:
# 866 "lex.cyl"
 return 356;case 29U: _LL3B: _LL3C:
# 867 "lex.cyl"
 return 352;case 30U: _LL3D: _LL3E:
# 868 "lex.cyl"
 return 353;case 31U: _LL3F: _LL40:
# 869 "lex.cyl"
 return 354;case 32U: _LL41: _LL42:
# 870 "lex.cyl"
 return 361;case 33U: _LL43: _LL44:
# 871 "lex.cyl"
 return 360;case 34U: _LL45: _LL46:
# 872 "lex.cyl"
 return 359;case 35U: _LL47: _LL48:
# 873 "lex.cyl"
 return 357;case 36U: _LL49: _LL4A:
# 874 "lex.cyl"
 return 358;case 37U: _LL4B: _LL4C:
# 875 "lex.cyl"
 return 350;case 38U: _LL4D: _LL4E:
# 876 "lex.cyl"
 return 351;case 39U: _LL4F: _LL50:
# 877 "lex.cyl"
 return 344;case 40U: _LL51: _LL52:
# 879
 return 345;case 41U: _LL53: _LL54:
# 880 "lex.cyl"
 return 363;case 42U: _LL55: _LL56:
# 881 "lex.cyl"
 return 341;case 43U: _LL57: _LL58:
# 882 "lex.cyl"
 return 362;case 44U: _LL59: _LL5A:
# 883 "lex.cyl"
 return 364;case 45U: _LL5B: _LL5C:
# 885
 return 314;case 46U: _LL5D: _LL5E:
# 888 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 47U: _LL5F: _LL60:
# 889 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 48U: _LL61: _LL62:
# 890 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 49U: _LL63: _LL64:
# 891 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 50U: _LL65: _LL66:
# 897 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_strng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lex_get_stored_string();
return 367;case 51U: _LL67: _LL68:
# 905 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lex_get_stored_string();
return 368;case 52U: _LL69: _LL6A:
# 913 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lex_get_stored_string();
return 370;case 53U: _LL6B: _LL6C:
# 920
 Cyc_Lex_token_char='\a';return 369;case 54U: _LL6D: _LL6E:
# 921 "lex.cyl"
 Cyc_Lex_token_char='\b';return 369;case 55U: _LL6F: _LL70:
# 922 "lex.cyl"
 Cyc_Lex_token_char='\f';return 369;case 56U: _LL71: _LL72:
# 923 "lex.cyl"
 Cyc_Lex_token_char='\n';return 369;case 57U: _LL73: _LL74:
# 924 "lex.cyl"
 Cyc_Lex_token_char='\r';return 369;case 58U: _LL75: _LL76:
# 925 "lex.cyl"
 Cyc_Lex_token_char='\t';return 369;case 59U: _LL77: _LL78:
# 926 "lex.cyl"
 Cyc_Lex_token_char='\v';return 369;case 60U: _LL79: _LL7A:
# 927 "lex.cyl"
 Cyc_Lex_token_char='\\';return 369;case 61U: _LL7B: _LL7C:
# 928 "lex.cyl"
 Cyc_Lex_token_char='\'';return 369;case 62U: _LL7D: _LL7E:
# 929 "lex.cyl"
 Cyc_Lex_token_char='"';return 369;case 63U: _LL7F: _LL80:
# 930 "lex.cyl"
 Cyc_Lex_token_char='?';return 369;case 64U: _LL81: _LL82:
# 933 "lex.cyl"
 Cyc_Lex_token_char=({union Cyc_Absyn_Cnst _tmp1B3=Cyc_Lex_intconst(lexbuf,2,1,8);Cyc_Lex_cnst2char(_tmp1B3,lexbuf);});
return 369;case 65U: _LL83: _LL84:
# 938 "lex.cyl"
 Cyc_Lex_token_char=({union Cyc_Absyn_Cnst _tmp1B4=Cyc_Lex_intconst(lexbuf,3,1,16);Cyc_Lex_cnst2char(_tmp1B4,lexbuf);});
return 369;case 66U: _LL85: _LL86:
# 942
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 369;case 67U: _LL87: _LL88:
# 946
 return - 1;case 68U: _LL89: _LL8A:
# 948
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL8B: _LL8C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}_LL0:;}
# 952
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1B5=({const char*_tmp101="some action didn't return!";_tag_fat(_tmp101,sizeof(char),27U);});_tmp102->f1=_tmp1B5;});_tmp102;}));}
# 954
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp103=lexstate;switch(_tmp103){case 0U: _LL1: _LL2:
# 951 "lex.cyl"
 return 0;case 1U: _LL3: _LL4:
# 953
 Cyc_Lex_store_string_char('\a');return 1;case 2U: _LL5: _LL6:
# 954 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3U: _LL7: _LL8:
# 955 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4U: _LL9: _LLA:
# 956 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5U: _LLB: _LLC:
# 957 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6U: _LLD: _LLE:
# 958 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7U: _LLF: _LL10:
# 959 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8U: _LL11: _LL12:
# 960 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9U: _LL13: _LL14:
# 961 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10U: _LL15: _LL16:
# 962 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11U: _LL17: _LL18:
# 963 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12U: _LL19: _LL1A:
# 966 "lex.cyl"
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13U: _LL1B: _LL1C:
# 969 "lex.cyl"
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14U: _LL1D: _LL1E:
# 971
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15U: _LL1F: _LL20:
# 973
 Cyc_Lex_inc_linenumber();({struct _fat_ptr _tmp1B6=({const char*_tmp104="wide character ends in newline";_tag_fat(_tmp104,sizeof(char),31U);});Cyc_Lex_runaway_err(_tmp1B6,lexbuf);});return 0;case 16U: _LL21: _LL22:
# 974 "lex.cyl"
({struct _fat_ptr _tmp1B7=({const char*_tmp105="unterminated wide character";_tag_fat(_tmp105,sizeof(char),28U);});Cyc_Lex_runaway_err(_tmp1B7,lexbuf);});return 0;case 17U: _LL23: _LL24:
# 975 "lex.cyl"
({struct _fat_ptr _tmp1B8=({const char*_tmp106="bad character following backslash in wide character";_tag_fat(_tmp106,sizeof(char),52U);});Cyc_Lex_err(_tmp1B8,lexbuf);});return 1;default: _LL25: _LL26:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}_LL0:;}
# 979
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1B9=({const char*_tmp107="some action didn't return!";_tag_fat(_tmp107,sizeof(char),27U);});_tmp108->f1=_tmp1B9;});_tmp108;}));}
# 981
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,1);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp109=lexstate;switch(_tmp109){case 0U: _LL1: _LL2:
# 980 "lex.cyl"
 return 1;case 1U: _LL3: _LL4:
# 981 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2U: _LL5: _LL6:
# 982 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3U: _LL7: _LL8:
# 984
 lexbuf->lex_curr_pos -=1;return 0;default: _LL9: _LLA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}_LL0:;}
# 988
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1BA=({const char*_tmp10A="some action didn't return!";_tag_fat(_tmp10A,sizeof(char),27U);});_tmp10B->f1=_tmp1BA;});_tmp10B;}));}
# 990
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,2);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp10C=lexstate;switch(_tmp10C){case 0U: _LL1: _LL2:
# 988 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1U: _LL3: _LL4:
# 989 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2U: _LL5: _LL6:
# 990 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3U: _LL7: _LL8:
# 991 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4U: _LL9: _LLA:
# 992 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5U: _LLB: _LLC:
# 993 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('n');}else{
# 997
Cyc_Lex_store_string_char('\n');}
return 1;case 6U: _LLD: _LLE:
# 1000 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7U: _LLF: _LL10:
# 1001 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('t');}else{
# 1005
Cyc_Lex_store_string_char('\t');}
return 1;case 8U: _LL11: _LL12:
# 1007 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9U: _LL13: _LL14:
# 1008 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10U: _LL15: _LL16:
# 1009 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11U: _LL17: _LL18:
# 1010 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12U: _LL19: _LL1A:
# 1011 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13U: _LL1B: _LL1C:
# 1014 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _tmp1BB=Cyc_Lex_intconst(lexbuf,1,0,8);Cyc_Lex_cnst2char(_tmp1BB,lexbuf);}));
return 1;case 14U: _LL1D: _LL1E:
# 1019 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _tmp1BC=Cyc_Lex_intconst(lexbuf,2,0,16);Cyc_Lex_cnst2char(_tmp1BC,lexbuf);}));
return 1;case 15U: _LL1F: _LL20:
# 1023
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 16U: _LL21: _LL22:
# 1025 "lex.cyl"
 Cyc_Lex_inc_linenumber();
({struct _fat_ptr _tmp1BD=({const char*_tmp10D="string ends in newline";_tag_fat(_tmp10D,sizeof(char),23U);});Cyc_Lex_runaway_err(_tmp1BD,lexbuf);});
return 0;case 17U: _LL23: _LL24:
# 1029 "lex.cyl"
({struct _fat_ptr _tmp1BE=({const char*_tmp10E="unterminated string";_tag_fat(_tmp10E,sizeof(char),20U);});Cyc_Lex_runaway_err(_tmp1BE,lexbuf);});
return 0;case 18U: _LL25: _LL26:
# 1032 "lex.cyl"
({struct _fat_ptr _tmp1BF=({const char*_tmp10F="bad character following backslash in string";_tag_fat(_tmp10F,sizeof(char),44U);});Cyc_Lex_err(_tmp1BF,lexbuf);});
return 1;default: _LL27: _LL28:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}_LL0:;}
# 1037
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1C0=({const char*_tmp110="some action didn't return!";_tag_fat(_tmp110,sizeof(char),27U);});_tmp111->f1=_tmp1C0;});_tmp111;}));}
# 1039
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,3);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp112=lexstate;switch(_tmp112){case 0U: _LL1: _LL2:
# 1042 "lex.cyl"
 Cyc_Lex_store_string(({const char*_tmp113="\" L\"";_tag_fat(_tmp113,sizeof(char),5U);}));return 1;case 1U: _LL3: _LL4:
# 1043 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2U: _LL5: _LL6:
# 1044 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3U: _LL7: _LL8:
# 1046
 lexbuf->lex_curr_pos -=1;return 0;default: _LL9: _LLA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}_LL0:;}
# 1050
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1C1=({const char*_tmp114="some action didn't return!";_tag_fat(_tmp114,sizeof(char),27U);});_tmp115->f1=_tmp1C1;});_tmp115;}));}
# 1052
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,4);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp116=lexstate;switch(_tmp116){case 0U: _LL1: _LL2:
# 1049 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1U: _LL3: _LL4:
# 1051
 Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,1));
return 1;case 2U: _LL5: _LL6:
# 1057 "lex.cyl"
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3U: _LL7: _LL8:
# 1059 "lex.cyl"
 Cyc_Lex_inc_linenumber();
({struct _fat_ptr _tmp1C2=({const char*_tmp117="string ends in newline";_tag_fat(_tmp117,sizeof(char),23U);});Cyc_Lex_runaway_err(_tmp1C2,lexbuf);});
return 0;case 4U: _LL9: _LLA:
# 1062 "lex.cyl"
({struct _fat_ptr _tmp1C3=({const char*_tmp118="unterminated string";_tag_fat(_tmp118,sizeof(char),20U);});Cyc_Lex_runaway_err(_tmp1C3,lexbuf);});
return 0;case 5U: _LLB: _LLC:
# 1064 "lex.cyl"
({struct _fat_ptr _tmp1C4=({const char*_tmp119="bad character following backslash in string";_tag_fat(_tmp119,sizeof(char),44U);});Cyc_Lex_err(_tmp1C4,lexbuf);});
return 1;default: _LLD: _LLE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}_LL0:;}
# 1069
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1C5=({const char*_tmp11A="some action didn't return!";_tag_fat(_tmp11A,sizeof(char),27U);});_tmp11B->f1=_tmp1C5;});_tmp11B;}));}
# 1071
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,5);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp11C=lexstate;switch(_tmp11C){case 0U: _LL1: _LL2:
# 1067 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1U: _LL3: _LL4:
# 1068 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 1073 "lex.cyl"
({struct _fat_ptr _tmp1C6=({const char*_tmp11D="unterminated comment";_tag_fat(_tmp11D,sizeof(char),21U);});Cyc_Lex_runaway_err(_tmp1C6,lexbuf);});
return 0;case 3U: _LL7: _LL8:
# 1076 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4U: _LL9: _LLA:
# 1077 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5U: _LLB: _LLC:
# 1078 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6U: _LLD: _LLE:
# 1079 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LLF: _LL10:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}_LL0:;}
# 1083
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1C7=({const char*_tmp11E="some action didn't return!";_tag_fat(_tmp11E,sizeof(char),27U);});_tmp11F->f1=_tmp1C7;});_tmp11F;}));}
# 1085
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,6);}
int Cyc_Lex_snarf_asm_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp120=lexstate;switch(_tmp120){case 0U: _LL1: _LL2:
# 1086 "lex.cyl"
 Cyc_Lex_token_asm.f1=0;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
({struct _fat_ptr _tmp1C8=(struct _fat_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_asm.f2=_tmp1C8;});
return 377;case 1U: _LL3: _LL4:
# 1096 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
({struct _fat_ptr _tmp1C9=(struct _fat_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_asm.f2=_tmp1C9;});
return 377;case 2U: _LL5: _LL6:
# 1106 "lex.cyl"
 Cyc_Lex_token_asm.f1=1;
Cyc_Lex_paren_depth=1;
Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_snarf_asm_body(lexbuf)){;}
({struct _fat_ptr _tmp1CA=(struct _fat_ptr)Cyc_Lex_get_stored_string();Cyc_Lex_token_asm.f2=_tmp1CA;});
return 377;default: _LL7: _LL8:
# 1114
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_rec(lexbuf,lexstate);}_LL0:;}
# 1117
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1CB=({const char*_tmp121="some action didn't return!";_tag_fat(_tmp121,sizeof(char),27U);});_tmp122->f1=_tmp1CB;});_tmp122;}));}
# 1119
int Cyc_Lex_snarf_asm(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_rec(lexbuf,7);}
int Cyc_Lex_snarf_asm_body_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp123=lexstate;switch(_tmp123){case 0U: _LL1: _LL2:
# 1116 "lex.cyl"
 Cyc_Lex_store_string_char('"');
Cyc_Lex_expand_specials=1;
while(Cyc_Lex_strng(lexbuf)){;}
Cyc_Lex_expand_specials=0;
Cyc_Lex_store_string_char('"');
return 1;case 1U: _LL3: _LL4:
# 1122 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_comment(lexbuf);
Cyc_Lex_store_string_char(' ');
return 1;case 2U: _LL5: _LL6:
# 1126 "lex.cyl"
 Cyc_Lex_inc_linenumber();
Cyc_Lex_store_string_char('\n');return 1;case 3U: _LL7: _LL8:
# 1128 "lex.cyl"
 Cyc_Lex_paren_depth +=1;Cyc_Lex_store_string_char('(');return 1;case 4U: _LL9: _LLA:
# 1129 "lex.cyl"
 Cyc_Lex_paren_depth -=1;
if(Cyc_Lex_paren_depth == 0)return 0;else{
# 1132
Cyc_Lex_store_string_char(')');
return 1;}case 5U: _LLB: _LLC:
# 1135 "lex.cyl"
({struct _fat_ptr _tmp1CC=({const char*_tmp124="unterminated asm";_tag_fat(_tmp124,sizeof(char),17U);});Cyc_Lex_runaway_err(_tmp1CC,lexbuf);});return 0;case 6U: _LLD: _LLE:
# 1136 "lex.cyl"
 Cyc_Lex_inc_linenumber();Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 7U: _LLF: _LL10:
# 1137 "lex.cyl"
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;default: _LL11: _LL12:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_snarf_asm_body_rec(lexbuf,lexstate);}_LL0:;}
# 1141
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1CD=({const char*_tmp125="some action didn't return!";_tag_fat(_tmp125,sizeof(char),27U);});_tmp126->f1=_tmp1CD;});_tmp126;}));}
# 1143
int Cyc_Lex_snarf_asm_body(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_snarf_asm_body_rec(lexbuf,8);}
# 1146 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}
# 1151
static struct Cyc_Xarray_Xarray*Cyc_Lex_empty_xarray(struct _RegionHandle*id_rgn,int dummy){
struct Cyc_Xarray_Xarray*symbols=({
struct _RegionHandle*_tmp1CF=id_rgn;((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _fat_ptr*))Cyc_Xarray_rcreate)(_tmp1CF,101,({struct _fat_ptr*_tmp128=_cycalloc(sizeof(*_tmp128));({struct _fat_ptr _tmp1CE=(struct _fat_ptr)({const char*_tmp127="";_tag_fat(_tmp127,sizeof(char),1U);});*_tmp128=_tmp1CE;});_tmp128;}));});
# 1155
((void(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);
return symbols;}
# 1159
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1161
Cyc_Lex_in_extern_c=0;{
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp129=Cyc_Lex_prev_extern_c;struct Cyc_List_List*_tmp12A=x;Cyc_Lex_prev_extern_c=_tmp12A;x=_tmp129;});
while(x != 0){
struct Cyc_List_List*t=x->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);
x=t;}
# 1170
if(Cyc_Lex_ids_trie != 0){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp12B=idt;struct Cyc_Lex_DynTrieSymbols*_tmp12C=Cyc_Lex_ids_trie;idt=_tmp12C;Cyc_Lex_ids_trie=_tmp12B;});{
struct Cyc_Lex_DynTrieSymbols _tmp12D=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _tmp12E=_tmp12D;struct Cyc_Core_DynamicRegion*_tmp12F;_LL1: _tmp12F=_tmp12E.dyn;_LL2:;
Cyc_Core_free_ukey(_tmp12F);
((void(*)(struct Cyc_Lex_DynTrieSymbols*ptr))Cyc_Core_ufree)(idt);};}
# 1177
if(Cyc_Lex_typedefs_trie != 0){
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp130=tdefs;struct Cyc_Lex_DynTrie*_tmp131=Cyc_Lex_typedefs_trie;tdefs=_tmp131;Cyc_Lex_typedefs_trie=_tmp130;});{
struct Cyc_Lex_DynTrie _tmp132=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _tmp133=_tmp132;struct Cyc_Core_DynamicRegion*_tmp134;_LL4: _tmp134=_tmp133.dyn;_LL5:;
Cyc_Core_free_ukey(_tmp134);
((void(*)(struct Cyc_Lex_DynTrie*ptr))Cyc_Core_ufree)(tdefs);};}{
# 1185
struct Cyc_Core_NewDynamicRegion _tmp135=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp136=_tmp135;struct Cyc_Core_DynamicRegion*_tmp141;_LL7: _tmp141=_tmp136.key;_LL8:;{
struct Cyc_Lex_Trie*_tmp137=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp141,0,Cyc_Lex_empty_trie);
struct Cyc_Xarray_Xarray*_tmp138=((struct Cyc_Xarray_Xarray*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Xarray_Xarray*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp141,0,Cyc_Lex_empty_xarray);
Cyc_Lex_ids_trie=({struct Cyc_Lex_DynTrieSymbols*_tmp139=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp139));_tmp139->dyn=_tmp141,_tmp139->t=_tmp137,_tmp139->symbols=_tmp138;_tmp139;});{
# 1190
struct Cyc_Core_NewDynamicRegion _tmp13A=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _tmp13B=_tmp13A;struct Cyc_Core_DynamicRegion*_tmp140;_LLA: _tmp140=_tmp13B.key;_LLB:;{
struct Cyc_Lex_Trie*t=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(_tmp140,0,Cyc_Lex_empty_trie);
Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*_tmp13C=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13C));_tmp13C->dyn=_tmp140,_tmp13C->t=t;_tmp13C;});
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
Cyc_Lex_kw_nums=({unsigned _tmp13E=(unsigned)Cyc_Lex_num_kws;struct Cyc_Lex_KeyWordInfo*_tmp13D=_cycalloc(_check_times(_tmp13E,sizeof(struct Cyc_Lex_KeyWordInfo)));({{unsigned _tmp167=(unsigned)Cyc_Lex_num_kws;unsigned i;for(i=0;i < _tmp167;++ i){(_tmp13D[i]).token_index=0,(_tmp13D[i]).is_c_keyword=0;}}0;});_tag_fat(_tmp13D,sizeof(struct Cyc_Lex_KeyWordInfo),_tmp13E);});{
unsigned i=0U;
unsigned rwsze=77U;
{unsigned j=0U;for(0;j < rwsze;++ j){
if(include_cyclone_keywords ||(Cyc_Lex_rw_array[(int)j]).f3){
struct _fat_ptr _tmp13F=(Cyc_Lex_rw_array[(int)j]).f1;
({struct _fat_ptr _tmp1D0=_tmp13F;Cyc_Lex_str_index(_tmp1D0,0,(int)Cyc_strlen((struct _fat_ptr)_tmp13F));});
({struct Cyc_Lex_KeyWordInfo _tmp1D1=({struct Cyc_Lex_KeyWordInfo _tmp168;_tmp168.token_index=(int)(Cyc_Lex_rw_array[(int)j]).f2,_tmp168.is_c_keyword=(Cyc_Lex_rw_array[(int)j]).f3;_tmp168;});*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),(int)i))=_tmp1D1;});
++ i;}}}
# 1205
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;};};};};};};}
