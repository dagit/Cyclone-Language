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
# 82 "lexing.h"
struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Set_Set;
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
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
# 52 "parse.h"
enum Cyc_Storage_class{Cyc_Typedef_sc =0U,Cyc_Extern_sc =1U,Cyc_ExternC_sc =2U,Cyc_Static_sc =3U,Cyc_Auto_sc =4U,Cyc_Register_sc =5U,Cyc_Abstract_sc =6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple21{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple21*val;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple23{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 91 "parse_tab.h"
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 110 "dict.h"
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46 "binding.h"
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct Cyc_Lex_Trie;struct _union_TrieChildren_Zero{int tag;int val;};struct _tuple24{int f1;struct Cyc_Lex_Trie*f2;};struct _union_TrieChildren_One{int tag;struct _tuple24 val;};struct _union_TrieChildren_Many{int tag;struct Cyc_Lex_Trie**val;};union Cyc_Lex_TrieChildren{struct _union_TrieChildren_Zero Zero;struct _union_TrieChildren_One One;struct _union_TrieChildren_Many Many;};
# 50 "lex.cyl"
union Cyc_Lex_TrieChildren Cyc_Lex_Zero(){return({union Cyc_Lex_TrieChildren _tmp13F;(_tmp13F.Zero).tag=1U,(_tmp13F.Zero).val=0;_tmp13F;});}
union Cyc_Lex_TrieChildren Cyc_Lex_One(int i,struct Cyc_Lex_Trie*t){
return({union Cyc_Lex_TrieChildren _tmp140;(_tmp140.One).tag=2U,((_tmp140.One).val).f1=i,((_tmp140.One).val).f2=t;_tmp140;});}
# 54
union Cyc_Lex_TrieChildren Cyc_Lex_Many(struct Cyc_Lex_Trie**ts){
return({union Cyc_Lex_TrieChildren _tmp141;(_tmp141.Many).tag=3U,(_tmp141.Many).val=ts;_tmp141;});}struct Cyc_Lex_Trie{union Cyc_Lex_TrieChildren children;int shared_str;};struct Cyc_Lex_DynTrie{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;};struct Cyc_Lex_DynTrieSymbols{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Lex_Trie*t;struct Cyc_Xarray_Xarray*symbols;};
# 76
static int Cyc_Lex_num_kws=0;struct Cyc_Lex_KeyWordInfo{int token_index;int is_c_keyword;};
# 78
static struct _fat_ptr Cyc_Lex_kw_nums={(void*)0,(void*)0,(void*)(0 + 0)};
# 80
int Cyc_Lex_compile_for_boot_r=0;
int Cyc_Lex_expand_specials=0;
# 83
static int Cyc_Lex_in_extern_c=0;
static struct Cyc_List_List*Cyc_Lex_prev_extern_c=0;
void Cyc_Lex_enter_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp0=x;struct Cyc_List_List*_tmp1=Cyc_Lex_prev_extern_c;x=_tmp1;Cyc_Lex_prev_extern_c=_tmp0;});
Cyc_Lex_prev_extern_c=({struct Cyc_List_List*_tmp2=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp2));_tmp2->hd=(void*)Cyc_Lex_in_extern_c,_tmp2->tl=x;_tmp2;});
Cyc_Lex_in_extern_c=1;}
# 91
void Cyc_Lex_leave_extern_c(){
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp3=x;struct Cyc_List_List*_tmp4=Cyc_Lex_prev_extern_c;x=_tmp4;Cyc_Lex_prev_extern_c=_tmp3;});
Cyc_Lex_in_extern_c=(int)((struct Cyc_List_List*)_check_null(x))->hd;
Cyc_Lex_prev_extern_c=x->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);}
# 99
static struct Cyc_Lex_DynTrieSymbols*Cyc_Lex_ids_trie=0;
static struct Cyc_Lex_DynTrie*Cyc_Lex_typedefs_trie=0;
# 102
static int Cyc_Lex_comment_depth=0;
# 105
static union Cyc_Absyn_Cnst Cyc_Lex_token_int={.Int_c={5,{Cyc_Absyn_Signed,0}}};static char _tmp5[8U]="*bogus*";
static struct _fat_ptr Cyc_Lex_bogus_string={_tmp5,_tmp5,_tmp5 + 8U};
static struct _tuple0 Cyc_Lex_token_id_pair={{.Loc_n={4,0}},& Cyc_Lex_bogus_string};
# 110
static char Cyc_Lex_token_char='\000';static char _tmp6[1U]="";
struct _fat_ptr Cyc_Lex_token_string={_tmp6,_tmp6,_tmp6 + 1U};
struct _tuple0*Cyc_Lex_token_qvar=& Cyc_Lex_token_id_pair;
# 115
static int Cyc_Lex_runaway_start=0;
static int Cyc_Lex_paren_depth=0;
# 118
static void Cyc_Lex_err(struct _fat_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned s=(unsigned)Cyc_Lexing_lexeme_start(lb);
({void*_tmp7=0U;({unsigned _tmp15F=s;struct _fat_ptr _tmp15E=msg;Cyc_Warn_err(_tmp15F,_tmp15E,_tag_fat(_tmp7,sizeof(void*),0U));});});}
# 122
static void Cyc_Lex_runaway_err(struct _fat_ptr msg,struct Cyc_Lexing_lexbuf*lb){
unsigned s=(unsigned)Cyc_Lex_runaway_start;
({void*_tmp8=0U;({unsigned _tmp161=s;struct _fat_ptr _tmp160=msg;Cyc_Warn_err(_tmp161,_tmp160,_tag_fat(_tmp8,sizeof(void*),0U));});});}
# 126
static char Cyc_Lex_cnst2char(union Cyc_Absyn_Cnst cnst,struct Cyc_Lexing_lexbuf*lb){
union Cyc_Absyn_Cnst _tmp9=cnst;char _tmpA;long long _tmpB;int _tmpC;switch((_tmp9.Char_c).tag){case 5U: _LL1: _tmpC=((_tmp9.Int_c).val).f2;_LL2: {int i=_tmpC;
return(char)i;}case 6U: _LL3: _tmpB=((_tmp9.LongLong_c).val).f2;_LL4: {long long i=_tmpB;
return(char)i;}case 2U: _LL5: _tmpA=((_tmp9.Char_c).val).f2;_LL6: {char c=_tmpA;
return c;}default: _LL7: _LL8:
({struct _fat_ptr _tmp162=({const char*_tmpD="couldn't convert constant to char!";_tag_fat(_tmpD,sizeof(char),35U);});Cyc_Lex_err(_tmp162,lb);});return'\000';}_LL0:;}static char _tmpE[14U]="__attribute__";static char _tmpF[12U]="__attribute";static char _tmp10[9U]="abstract";static char _tmp11[5U]="auto";static char _tmp12[6U]="break";static char _tmp13[18U]="__builtin_va_list";static char _tmp14[7U]="calloc";static char _tmp15[5U]="case";static char _tmp16[6U]="catch";static char _tmp17[5U]="char";static char _tmp18[6U]="const";static char _tmp19[10U]="__const__";static char _tmp1A[9U]="continue";static char _tmp1B[17U]="cyclone_override";static char _tmp1C[9U]="datatype";static char _tmp1D[8U]="default";static char _tmp1E[3U]="do";static char _tmp1F[7U]="double";static char _tmp20[5U]="else";static char _tmp21[5U]="enum";static char _tmp22[7U]="export";static char _tmp23[5U]="hide";static char _tmp24[14U]="__extension__";static char _tmp25[7U]="extern";static char _tmp26[9U]="fallthru";static char _tmp27[6U]="float";static char _tmp28[4U]="for";static char _tmp29[5U]="goto";static char _tmp2A[3U]="if";static char _tmp2B[7U]="inline";static char _tmp2C[11U]="__inline__";static char _tmp2D[9U]="__inline";static char _tmp2E[4U]="int";static char _tmp2F[4U]="let";static char _tmp30[5U]="long";static char _tmp31[7U]="malloc";static char _tmp32[10U]="namespace";static char _tmp33[4U]="new";static char _tmp34[5U]="NULL";static char _tmp35[8U]="numelts";static char _tmp36[9U]="offsetof";static char _tmp37[20U]="__cyclone_port_on__";static char _tmp38[21U]="__cyclone_port_off__";static char _tmp39[19U]="__cyclone_pragma__";static char _tmp3A[8U]="rcalloc";static char _tmp3B[9U]="region_t";static char _tmp3C[7U]="region";static char _tmp3D[8U]="regions";static char _tmp3E[9U]="register";static char _tmp3F[9U]="restrict";static char _tmp40[7U]="return";static char _tmp41[8U]="rmalloc";static char _tmp42[15U]="rmalloc_inline";static char _tmp43[5U]="rnew";static char _tmp44[6U]="short";static char _tmp45[7U]="signed";static char _tmp46[11U]="__signed__";static char _tmp47[7U]="sizeof";static char _tmp48[7U]="static";static char _tmp49[7U]="struct";static char _tmp4A[7U]="switch";static char _tmp4B[9U]="tagcheck";static char _tmp4C[6U]="tag_t";static char _tmp4D[15U]="__tempest_on__";static char _tmp4E[16U]="__tempest_off__";static char _tmp4F[6U]="throw";static char _tmp50[4U]="try";static char _tmp51[8U]="typedef";static char _tmp52[7U]="typeof";static char _tmp53[11U]="__typeof__";static char _tmp54[6U]="union";static char _tmp55[9U]="unsigned";static char _tmp56[13U]="__unsigned__";static char _tmp57[6U]="using";static char _tmp58[8U]="valueof";static char _tmp59[10U]="valueof_t";static char _tmp5A[5U]="void";static char _tmp5B[9U]="volatile";static char _tmp5C[13U]="__volatile__";static char _tmp5D[4U]="asm";static char _tmp5E[8U]="__asm__";static char _tmp5F[6U]="while";struct _tuple25{struct _fat_ptr f1;short f2;int f3;};
# 138
static struct _tuple25 Cyc_Lex_rw_array[82U]={{{_tmpE,_tmpE,_tmpE + 14U},378,1},{{_tmpF,_tmpF,_tmpF + 12U},378,1},{{_tmp10,_tmp10,_tmp10 + 9U},305,0},{{_tmp11,_tmp11,_tmp11 + 5U},258,1},{{_tmp12,_tmp12,_tmp12 + 6U},290,1},{{_tmp13,_tmp13,_tmp13 + 18U},294,1},{{_tmp14,_tmp14,_tmp14 + 7U},313,0},{{_tmp15,_tmp15,_tmp15 + 5U},277,1},{{_tmp16,_tmp16,_tmp16 + 6U},300,1},{{_tmp17,_tmp17,_tmp17 + 5U},264,1},{{_tmp18,_tmp18,_tmp18 + 6U},272,1},{{_tmp19,_tmp19,_tmp19 + 10U},272,1},{{_tmp1A,_tmp1A,_tmp1A + 9U},289,1},{{_tmp1B,_tmp1B,_tmp1B + 17U},302,0},{{_tmp1C,_tmp1C,_tmp1C + 9U},309,0},{{_tmp1D,_tmp1D,_tmp1D + 8U},278,1},{{_tmp1E,_tmp1E,_tmp1E + 3U},286,1},{{_tmp1F,_tmp1F,_tmp1F + 7U},269,1},{{_tmp20,_tmp20,_tmp20 + 5U},283,1},{{_tmp21,_tmp21,_tmp21 + 5U},292,1},{{_tmp22,_tmp22,_tmp22 + 7U},301,0},{{_tmp23,_tmp23,_tmp23 + 5U},303,0},{{_tmp24,_tmp24,_tmp24 + 14U},295,1},{{_tmp25,_tmp25,_tmp25 + 7U},261,1},{{_tmp26,_tmp26,_tmp26 + 9U},306,0},{{_tmp27,_tmp27,_tmp27 + 6U},268,1},{{_tmp28,_tmp28,_tmp28 + 4U},287,1},{{_tmp29,_tmp29,_tmp29 + 5U},288,1},{{_tmp2A,_tmp2A,_tmp2A + 3U},282,1},{{_tmp2B,_tmp2B,_tmp2B + 7U},279,1},{{_tmp2C,_tmp2C,_tmp2C + 11U},279,1},{{_tmp2D,_tmp2D,_tmp2D + 9U},279,1},{{_tmp2E,_tmp2E,_tmp2E + 4U},266,1},{{_tmp2F,_tmp2F,_tmp2F + 4U},297,0},{{_tmp30,_tmp30,_tmp30 + 5U},267,1},{{_tmp31,_tmp31,_tmp31 + 7U},310,0},{{_tmp32,_tmp32,_tmp32 + 10U},308,0},{{_tmp33,_tmp33,_tmp33 + 4U},304,0},{{_tmp34,_tmp34,_tmp34 + 5U},296,0},{{_tmp35,_tmp35,_tmp35 + 8U},326,0},{{_tmp36,_tmp36,_tmp36 + 9U},281,1},{{_tmp37,_tmp37,_tmp37 + 20U},321,0},{{_tmp38,_tmp38,_tmp38 + 21U},322,0},{{_tmp39,_tmp39,_tmp39 + 19U},323,0},{{_tmp3A,_tmp3A,_tmp3A + 8U},314,0},{{_tmp3B,_tmp3B,_tmp3B + 9U},316,0},{{_tmp3C,_tmp3C,_tmp3C + 7U},318,0},{{_tmp3D,_tmp3D,_tmp3D + 8U},320,0},{{_tmp3E,_tmp3E,_tmp3E + 9U},259,1},{{_tmp3F,_tmp3F,_tmp3F + 9U},274,1},{{_tmp40,_tmp40,_tmp40 + 7U},291,1},{{_tmp41,_tmp41,_tmp41 + 8U},311,0},{{_tmp42,_tmp42,_tmp42 + 15U},312,0},{{_tmp43,_tmp43,_tmp43 + 5U},319,0},{{_tmp44,_tmp44,_tmp44 + 6U},265,1},{{_tmp45,_tmp45,_tmp45 + 7U},270,1},{{_tmp46,_tmp46,_tmp46 + 11U},270,1},{{_tmp47,_tmp47,_tmp47 + 7U},280,1},{{_tmp48,_tmp48,_tmp48 + 7U},260,1},{{_tmp49,_tmp49,_tmp49 + 7U},275,1},{{_tmp4A,_tmp4A,_tmp4A + 7U},284,1},{{_tmp4B,_tmp4B,_tmp4B + 9U},329,0},{{_tmp4C,_tmp4C,_tmp4C + 6U},317,0},{{_tmp4D,_tmp4D,_tmp4D + 15U},324,0},{{_tmp4E,_tmp4E,_tmp4E + 16U},325,0},{{_tmp4F,_tmp4F,_tmp4F + 6U},298,0},{{_tmp50,_tmp50,_tmp50 + 4U},299,0},{{_tmp51,_tmp51,_tmp51 + 8U},262,1},{{_tmp52,_tmp52,_tmp52 + 7U},293,1},{{_tmp53,_tmp53,_tmp53 + 11U},293,1},{{_tmp54,_tmp54,_tmp54 + 6U},276,1},{{_tmp55,_tmp55,_tmp55 + 9U},271,1},{{_tmp56,_tmp56,_tmp56 + 13U},271,1},{{_tmp57,_tmp57,_tmp57 + 6U},307,0},{{_tmp58,_tmp58,_tmp58 + 8U},327,0},{{_tmp59,_tmp59,_tmp59 + 10U},328,0},{{_tmp5A,_tmp5A,_tmp5A + 5U},263,1},{{_tmp5B,_tmp5B,_tmp5B + 9U},273,1},{{_tmp5C,_tmp5C,_tmp5C + 13U},273,1},{{_tmp5D,_tmp5D,_tmp5D + 4U},379,1},{{_tmp5E,_tmp5E,_tmp5E + 8U},379,1},{{_tmp5F,_tmp5F,_tmp5F + 6U},285,1}};
# 223
static int Cyc_Lex_num_keywords(int include_cyclone_keywords){
int sum=0;
{unsigned i=0U;for(0;i < 82U;++ i){
if(include_cyclone_keywords ||(Cyc_Lex_rw_array[(int)i]).f3)
++ sum;}}
return sum;}
# 232
static struct Cyc_Lex_Trie*Cyc_Lex_empty_trie(struct _RegionHandle*d,int dummy){
return({struct Cyc_Lex_Trie*_tmp60=_region_malloc(d,sizeof(*_tmp60));({union Cyc_Lex_TrieChildren _tmp163=Cyc_Lex_Zero();_tmp60->children=_tmp163;}),_tmp60->shared_str=0;_tmp60;});}
# 236
static int Cyc_Lex_trie_char(int c){
# 238
if(c >= 95)return c - 59;else{
if(c > 64)return c - 55;}
return c - 48;}
# 243
static struct Cyc_Lex_Trie*Cyc_Lex_trie_lookup(struct _RegionHandle*r,struct Cyc_Lex_Trie*t,struct _fat_ptr buff,int offset,unsigned len){
# 245
unsigned i=0U;
buff=_fat_ptr_plus(buff,sizeof(char),offset);
if(!(len < _get_fat_size(buff,sizeof(char))))
({void*_tmp61=0U;({struct _fat_ptr _tmp164=({const char*_tmp62="array bounds in trie_lookup";_tag_fat(_tmp62,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp164,_tag_fat(_tmp61,sizeof(void*),0U));});});
while(i < len){
union Cyc_Lex_TrieChildren _tmp63=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _stmttmp0=_tmp63;union Cyc_Lex_TrieChildren _tmp64=_stmttmp0;struct Cyc_Lex_Trie*_tmp66;int _tmp65;struct Cyc_Lex_Trie**_tmp67;switch((_tmp64.One).tag){case 3U: _LL1: _tmp67=(_tmp64.Many).val;_LL2: {struct Cyc_Lex_Trie**arr=_tmp67;
# 253
int ch=Cyc_Lex_trie_char((int)((const char*)buff.curr)[(int)i]);
if(*((struct Cyc_Lex_Trie**)_check_known_subscript_notnull(arr,64U,sizeof(struct Cyc_Lex_Trie*),ch))== 0)
({struct Cyc_Lex_Trie*_tmp166=({struct Cyc_Lex_Trie*_tmp68=_region_malloc(r,sizeof(*_tmp68));({union Cyc_Lex_TrieChildren _tmp165=Cyc_Lex_Zero();_tmp68->children=_tmp165;}),_tmp68->shared_str=0;_tmp68;});arr[ch]=_tmp166;});
t=arr[ch];
++ i;
goto _LL0;}case 2U: _LL3: _tmp65=((_tmp64.One).val).f1;_tmp66=((_tmp64.One).val).f2;_LL4: {int one_ch=_tmp65;struct Cyc_Lex_Trie*one_trie=_tmp66;
# 261
if(one_ch == (int)((const char*)buff.curr)[(int)i])
t=one_trie;else{
# 264
struct Cyc_Lex_Trie**_tmp69=({unsigned _tmp6D=64U;struct Cyc_Lex_Trie**_tmp6C=({struct _RegionHandle*_tmp167=r;_region_malloc(_tmp167,_check_times(_tmp6D,sizeof(struct Cyc_Lex_Trie*)));});({{unsigned _tmp142=64U;unsigned j;for(j=0;j < _tmp142;++ j){_tmp6C[j]=0;}}0;});_tmp6C;});struct Cyc_Lex_Trie**arr=_tmp69;
({struct Cyc_Lex_Trie*_tmp169=one_trie;*((struct Cyc_Lex_Trie**)({typeof(arr )_tmp168=arr;_check_known_subscript_notnull(_tmp168,64U,sizeof(struct Cyc_Lex_Trie*),Cyc_Lex_trie_char(one_ch));}))=_tmp169;});{
int _tmp6A=Cyc_Lex_trie_char((int)((const char*)buff.curr)[(int)i]);int ch=_tmp6A;
({struct Cyc_Lex_Trie*_tmp16B=({struct Cyc_Lex_Trie*_tmp6B=_region_malloc(r,sizeof(*_tmp6B));({union Cyc_Lex_TrieChildren _tmp16A=Cyc_Lex_Zero();_tmp6B->children=_tmp16A;}),_tmp6B->shared_str=0;_tmp6B;});*((struct Cyc_Lex_Trie**)_check_known_subscript_notnull(arr,64U,sizeof(struct Cyc_Lex_Trie*),ch))=_tmp16B;});
({union Cyc_Lex_TrieChildren _tmp16C=Cyc_Lex_Many(arr);t->children=_tmp16C;});
t=arr[ch];}}
# 271
++ i;
goto _LL0;}default: _LL5: _LL6:
# 275
 while(i < len){
struct Cyc_Lex_Trie*_tmp6E=({struct Cyc_Lex_Trie*_tmp6F=_region_malloc(r,sizeof(*_tmp6F));({union Cyc_Lex_TrieChildren _tmp16D=Cyc_Lex_Zero();_tmp6F->children=_tmp16D;}),_tmp6F->shared_str=0;_tmp6F;});struct Cyc_Lex_Trie*next=_tmp6E;
({union Cyc_Lex_TrieChildren _tmp16E=Cyc_Lex_One((int)*((const char*)_check_fat_subscript(buff,sizeof(char),(int)i ++)),next);t->children=_tmp16E;});
t=next;}
# 280
return t;}_LL0:;}
# 283
return t;}struct _tuple26{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;struct _fat_ptr f3;int f4;int f5;};
# 286
static int Cyc_Lex_str_index_body(struct _RegionHandle*d,struct _tuple26*env){
# 289
struct _tuple26 _tmp70=*env;struct _tuple26 _stmttmp1=_tmp70;struct _tuple26 _tmp71=_stmttmp1;int _tmp76;int _tmp75;struct _fat_ptr _tmp74;struct Cyc_Xarray_Xarray*_tmp73;struct Cyc_Lex_Trie*_tmp72;_LL1: _tmp72=_tmp71.f1;_tmp73=_tmp71.f2;_tmp74=_tmp71.f3;_tmp75=_tmp71.f4;_tmp76=_tmp71.f5;_LL2: {struct Cyc_Lex_Trie*tree=_tmp72;struct Cyc_Xarray_Xarray*symbols=_tmp73;struct _fat_ptr buff=_tmp74;int offset=_tmp75;int len=_tmp76;
struct Cyc_Lex_Trie*_tmp77=Cyc_Lex_trie_lookup(d,tree,buff,offset,(unsigned)len);struct Cyc_Lex_Trie*t=_tmp77;
# 292
if(((struct Cyc_Lex_Trie*)_check_null(t))->shared_str == 0){
struct _fat_ptr _tmp78=Cyc_Core_new_string((unsigned)(len + 1));struct _fat_ptr newstr=_tmp78;
({struct _fat_ptr _tmp170=_fat_ptr_decrease_size(newstr,sizeof(char),1U);struct _fat_ptr _tmp16F=(struct _fat_ptr)_fat_ptr_plus(buff,sizeof(char),offset);Cyc_zstrncpy(_tmp170,_tmp16F,(unsigned long)len);});{
int ans=({struct Cyc_Xarray_Xarray*_tmp171=symbols;((int(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add_ind)(_tmp171,({struct _fat_ptr*_tmp79=_cycalloc(sizeof(*_tmp79));*_tmp79=(struct _fat_ptr)newstr;_tmp79;}));});
t->shared_str=ans;}}
# 298
return t->shared_str;}}
# 301
static int Cyc_Lex_str_index(struct _fat_ptr buff,int offset,int len){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp7A=idt;struct Cyc_Lex_DynTrieSymbols*_tmp7B=Cyc_Lex_ids_trie;idt=_tmp7B;Cyc_Lex_ids_trie=_tmp7A;});{
struct Cyc_Lex_DynTrieSymbols _tmp7C=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _stmttmp2=_tmp7C;struct Cyc_Lex_DynTrieSymbols _tmp7D=_stmttmp2;struct Cyc_Xarray_Xarray*_tmp80;struct Cyc_Lex_Trie*_tmp7F;struct Cyc_Core_DynamicRegion*_tmp7E;_LL1: _tmp7E=_tmp7D.dyn;_tmp7F=_tmp7D.t;_tmp80=_tmp7D.symbols;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp7E;struct Cyc_Lex_Trie*t=_tmp7F;struct Cyc_Xarray_Xarray*symbols=_tmp80;
struct _tuple26 _tmp81=({struct _tuple26 _tmp144;_tmp144.f1=t,_tmp144.f2=symbols,_tmp144.f3=buff,_tmp144.f4=offset,_tmp144.f5=len;_tmp144;});struct _tuple26 env=_tmp81;
int _tmp82=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple26*arg,int(*body)(struct _RegionHandle*h,struct _tuple26*arg)))Cyc_Core_open_region)(dyn,& env,Cyc_Lex_str_index_body);int res=_tmp82;
({struct Cyc_Lex_DynTrieSymbols _tmp172=({struct Cyc_Lex_DynTrieSymbols _tmp143;_tmp143.dyn=dyn,_tmp143.t=t,_tmp143.symbols=symbols;_tmp143;});*idt=_tmp172;});
({struct Cyc_Lex_DynTrieSymbols*_tmp83=idt;struct Cyc_Lex_DynTrieSymbols*_tmp84=Cyc_Lex_ids_trie;idt=_tmp84;Cyc_Lex_ids_trie=_tmp83;});
return res;}}}
# 312
static int Cyc_Lex_str_index_lbuf(struct Cyc_Lexing_lexbuf*lbuf){
return Cyc_Lex_str_index((struct _fat_ptr)lbuf->lex_buffer,lbuf->lex_start_pos,lbuf->lex_curr_pos - lbuf->lex_start_pos);}struct _tuple27{struct Cyc_Lex_Trie*f1;struct _fat_ptr f2;};
# 318
static int Cyc_Lex_insert_typedef_body(struct _RegionHandle*h,struct _tuple27*arg){
# 320
struct _tuple27 _tmp85=*arg;struct _tuple27 _stmttmp3=_tmp85;struct _tuple27 _tmp86=_stmttmp3;struct _fat_ptr _tmp88;struct Cyc_Lex_Trie*_tmp87;_LL1: _tmp87=_tmp86.f1;_tmp88=_tmp86.f2;_LL2: {struct Cyc_Lex_Trie*t=_tmp87;struct _fat_ptr s=_tmp88;
struct Cyc_Lex_Trie*_tmp89=Cyc_Lex_trie_lookup(h,t,s,0,_get_fat_size(s,sizeof(char))- (unsigned)1);struct Cyc_Lex_Trie*t_node=_tmp89;
((struct Cyc_Lex_Trie*)_check_null(t_node))->shared_str=1;
return 0;}}
# 327
static void Cyc_Lex_insert_typedef(struct _fat_ptr*sptr){
struct _fat_ptr _tmp8A=*sptr;struct _fat_ptr s=_tmp8A;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp8B=tdefs;struct Cyc_Lex_DynTrie*_tmp8C=Cyc_Lex_typedefs_trie;tdefs=_tmp8C;Cyc_Lex_typedefs_trie=_tmp8B;});{
struct Cyc_Lex_DynTrie _tmp8D=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _stmttmp4=_tmp8D;struct Cyc_Lex_DynTrie _tmp8E=_stmttmp4;struct Cyc_Lex_Trie*_tmp90;struct Cyc_Core_DynamicRegion*_tmp8F;_LL1: _tmp8F=_tmp8E.dyn;_tmp90=_tmp8E.t;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp8F;struct Cyc_Lex_Trie*t=_tmp90;
struct _tuple27 _tmp91=({struct _tuple27 _tmp146;_tmp146.f1=t,_tmp146.f2=s;_tmp146;});struct _tuple27 env=_tmp91;
((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple27*arg,int(*body)(struct _RegionHandle*h,struct _tuple27*arg)))Cyc_Core_open_region)(dyn,& env,Cyc_Lex_insert_typedef_body);
({struct Cyc_Lex_DynTrie _tmp173=({struct Cyc_Lex_DynTrie _tmp145;_tmp145.dyn=dyn,_tmp145.t=t;_tmp145;});*tdefs=_tmp173;});
({struct Cyc_Lex_DynTrie*_tmp92=tdefs;struct Cyc_Lex_DynTrie*_tmp93=Cyc_Lex_typedefs_trie;tdefs=_tmp93;Cyc_Lex_typedefs_trie=_tmp92;});
return;}}}struct _tuple28{struct Cyc_Lex_Trie*f1;struct Cyc_Xarray_Xarray*f2;int f3;};
# 339
static struct _fat_ptr*Cyc_Lex_get_symbol_body(struct _RegionHandle*dyn,struct _tuple28*env){
struct _tuple28 _tmp94=*env;struct _tuple28 _stmttmp5=_tmp94;struct _tuple28 _tmp95=_stmttmp5;int _tmp98;struct Cyc_Xarray_Xarray*_tmp97;struct Cyc_Lex_Trie*_tmp96;_LL1: _tmp96=_tmp95.f1;_tmp97=_tmp95.f2;_tmp98=_tmp95.f3;_LL2: {struct Cyc_Lex_Trie*t=_tmp96;struct Cyc_Xarray_Xarray*symbols=_tmp97;int symbol_num=_tmp98;
return((struct _fat_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(symbols,symbol_num);}}
# 344
static struct _fat_ptr*Cyc_Lex_get_symbol(int symbol_num){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp99=idt;struct Cyc_Lex_DynTrieSymbols*_tmp9A=Cyc_Lex_ids_trie;idt=_tmp9A;Cyc_Lex_ids_trie=_tmp99;});{
struct Cyc_Lex_DynTrieSymbols _tmp9B=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _stmttmp6=_tmp9B;struct Cyc_Lex_DynTrieSymbols _tmp9C=_stmttmp6;struct Cyc_Xarray_Xarray*_tmp9F;struct Cyc_Lex_Trie*_tmp9E;struct Cyc_Core_DynamicRegion*_tmp9D;_LL1: _tmp9D=_tmp9C.dyn;_tmp9E=_tmp9C.t;_tmp9F=_tmp9C.symbols;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp9D;struct Cyc_Lex_Trie*t=_tmp9E;struct Cyc_Xarray_Xarray*symbols=_tmp9F;
struct _tuple28 _tmpA0=({struct _tuple28 _tmp148;_tmp148.f1=t,_tmp148.f2=symbols,_tmp148.f3=symbol_num;_tmp148;});struct _tuple28 env=_tmpA0;
struct _fat_ptr*_tmpA1=((struct _fat_ptr*(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple28*arg,struct _fat_ptr*(*body)(struct _RegionHandle*h,struct _tuple28*arg)))Cyc_Core_open_region)(dyn,& env,Cyc_Lex_get_symbol_body);struct _fat_ptr*res=_tmpA1;
({struct Cyc_Lex_DynTrieSymbols _tmp174=({struct Cyc_Lex_DynTrieSymbols _tmp147;_tmp147.dyn=dyn,_tmp147.t=t,_tmp147.symbols=symbols;_tmp147;});*idt=_tmp174;});
({struct Cyc_Lex_DynTrieSymbols*_tmpA2=idt;struct Cyc_Lex_DynTrieSymbols*_tmpA3=Cyc_Lex_ids_trie;idt=_tmpA3;Cyc_Lex_ids_trie=_tmpA2;});
return res;}}}
# 363 "lex.cyl"
static int Cyc_Lex_int_of_char(char c){
if((int)'0' <= (int)c &&(int)c <= (int)'9')return(int)c - (int)'0';else{
if((int)'a' <= (int)c &&(int)c <= (int)'f')return(10 + (int)c)- (int)'a';else{
if((int)'A' <= (int)c &&(int)c <= (int)'F')return(10 + (int)c)- (int)'A';else{
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp175=({const char*_tmpA4="string to integer conversion";_tag_fat(_tmpA4,sizeof(char),29U);});_tmpA5->f1=_tmp175;});_tmpA5;}));}}}}
# 371
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
# 382
len -=1;
declared_size=1;
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'l' ||(int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'L')){
len -=1;
size=4U;}}
# 389
if(len >= 1 &&((int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'u' ||(int)*((char*)_check_fat_subscript(buff,sizeof(char),len - 1))== (int)'U')){
len -=1;
sn=1U;}
# 393
if((int)sn == (int)1U){
unsigned long long n=(unsigned long long)0;
{int i=0;for(0;i < len;++ i){
n=({unsigned long long _tmp176=n * (unsigned long long)base;_tmp176 + (unsigned long long)Cyc_Lex_int_of_char(*((char*)_check_fat_subscript(buff,sizeof(char),i)));});}}
if(n > (unsigned long long)-1){
if(declared_size &&(int)size == (int)2U)
({struct _fat_ptr _tmp177=({const char*_tmpA6="integer constant too large";_tag_fat(_tmpA6,sizeof(char),27U);});Cyc_Lex_err(_tmp177,lbuf);});
size=4U;}
# 402
if((int)size == (int)2U)
res=({union Cyc_Absyn_Cnst _tmp149;(_tmp149.Int_c).tag=5U,((_tmp149.Int_c).val).f1=sn,((_tmp149.Int_c).val).f2=(int)((unsigned)n);_tmp149;});else{
# 405
res=({union Cyc_Absyn_Cnst _tmp14A;(_tmp14A.LongLong_c).tag=6U,((_tmp14A.LongLong_c).val).f1=sn,((_tmp14A.LongLong_c).val).f2=(long long)n;_tmp14A;});}}else{
# 408
long long n=(long long)0;
{int i=0;for(0;i < len;++ i){
n=({long long _tmp178=n * (long long)base;_tmp178 + (long long)Cyc_Lex_int_of_char(*((char*)_check_fat_subscript(buff,sizeof(char),i)));});}}{
unsigned long long x=(unsigned long long)n >> (unsigned long long)32;
if(x != (unsigned long long)-1 && x != (unsigned long long)0){
if(declared_size &&(int)size == (int)2U)
({struct _fat_ptr _tmp179=({const char*_tmpA7="integer constant too large";_tag_fat(_tmpA7,sizeof(char),27U);});Cyc_Lex_err(_tmp179,lbuf);});
size=4U;}
# 417
if((int)size == (int)2U)
res=({union Cyc_Absyn_Cnst _tmp14B;(_tmp14B.Int_c).tag=5U,((_tmp14B.Int_c).val).f1=sn,((_tmp14B.Int_c).val).f2=(int)n;_tmp14B;});else{
# 420
res=({union Cyc_Absyn_Cnst _tmp14C;(_tmp14C.LongLong_c).tag=6U,((_tmp14C.LongLong_c).val).f1=sn,((_tmp14C.LongLong_c).val).f2=n;_tmp14C;});}}}
# 423
return res;}}
# 428
char Cyc_Lex_string_buffer_v[11U]={'x','x','x','x','x','x','x','x','x','x','\000'};
struct _fat_ptr Cyc_Lex_string_buffer={(void*)Cyc_Lex_string_buffer_v,(void*)Cyc_Lex_string_buffer_v,(void*)(Cyc_Lex_string_buffer_v + 11U)};
int Cyc_Lex_string_pos=0;
void Cyc_Lex_store_string_char(char c){
int sz=(int)(_get_fat_size(Cyc_Lex_string_buffer,sizeof(char))- (unsigned)1);
if(Cyc_Lex_string_pos >= sz){
int newsz=sz;
while(Cyc_Lex_string_pos >= newsz){newsz=newsz * 2;}{
struct _fat_ptr str=({unsigned _tmpA9=(unsigned)newsz + 1U;char*_tmpA8=_cycalloc_atomic(_check_times(_tmpA9,sizeof(char)));({{unsigned _tmp14D=(unsigned)newsz;unsigned i;for(i=0;i < _tmp14D;++ i){i < (unsigned)sz?_tmpA8[i]=*((char*)_check_fat_subscript(Cyc_Lex_string_buffer,sizeof(char),(int)i)):(_tmpA8[i]='\000');}_tmpA8[_tmp14D]=0;}0;});_tag_fat(_tmpA8,sizeof(char),_tmpA9);});
Cyc_Lex_string_buffer=str;}}
# 439
({struct _fat_ptr _tmpAA=_fat_ptr_plus(Cyc_Lex_string_buffer,sizeof(char),Cyc_Lex_string_pos);char _tmpAB=*((char*)_check_fat_subscript(_tmpAA,sizeof(char),0U));char _tmpAC=c;if(_get_fat_size(_tmpAA,sizeof(char))== 1U &&(_tmpAB == 0 && _tmpAC != 0))_throw_arraybounds();*((char*)_tmpAA.curr)=_tmpAC;});
++ Cyc_Lex_string_pos;}
# 442
void Cyc_Lex_store_string(struct _fat_ptr s){
int sz=(int)Cyc_strlen((struct _fat_ptr)s);
int i=0;for(0;i < sz;++ i){
Cyc_Lex_store_string_char(*((const char*)_check_fat_subscript(s,sizeof(char),i)));}}
# 447
struct _fat_ptr Cyc_Lex_get_stored_string(){
struct _fat_ptr str=Cyc_substring((struct _fat_ptr)Cyc_Lex_string_buffer,0,(unsigned long)Cyc_Lex_string_pos);
Cyc_Lex_string_pos=0;
return str;}struct Cyc_Lex_Ldecls{struct Cyc_Set_Set*typedefs;};
# 465 "lex.cyl"
static struct Cyc_Core_Opt*Cyc_Lex_lstate=0;
# 467
static struct Cyc_Lex_Ldecls*Cyc_Lex_mk_empty_ldecls(int ignore){
return({struct Cyc_Lex_Ldecls*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct Cyc_Set_Set*_tmp17A=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty)(Cyc_zstrptrcmp);_tmpAD->typedefs=_tmp17A;});_tmpAD;});}
# 471
static void Cyc_Lex_typedef_init(){
Cyc_Lex_lstate=({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*_tmpAE));({struct Cyc_Binding_NSCtxt*_tmp17B=((struct Cyc_Binding_NSCtxt*(*)(int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Lex_mk_empty_ldecls);_tmpAE->v=_tmp17B;});_tmpAE;});}
# 475
static void Cyc_Lex_recompute_typedefs(){
# 477
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpAF=tdefs;struct Cyc_Lex_DynTrie*_tmpB0=Cyc_Lex_typedefs_trie;tdefs=_tmpB0;Cyc_Lex_typedefs_trie=_tmpAF;});{
struct Cyc_Lex_DynTrie _tmpB1=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _stmttmp7=_tmpB1;struct Cyc_Lex_DynTrie _tmpB2=_stmttmp7;struct Cyc_Lex_Trie*_tmpB4;struct Cyc_Core_DynamicRegion*_tmpB3;_LL1: _tmpB3=_tmpB2.dyn;_tmpB4=_tmpB2.t;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmpB3;struct Cyc_Lex_Trie*t=_tmpB4;
Cyc_Core_free_ukey(dyn);{
struct Cyc_Core_NewDynamicRegion _tmpB5=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp8=_tmpB5;struct Cyc_Core_NewDynamicRegion _tmpB6=_stmttmp8;struct Cyc_Core_DynamicRegion*_tmpB7;_LL4: _tmpB7=_tmpB6.key;_LL5: {struct Cyc_Core_DynamicRegion*dyn2=_tmpB7;
struct Cyc_Lex_Trie*t2=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(dyn2,0,Cyc_Lex_empty_trie);
({struct Cyc_Lex_DynTrie _tmp17C=({struct Cyc_Lex_DynTrie _tmp14E;_tmp14E.dyn=dyn2,_tmp14E.t=t2;_tmp14E;});*tdefs=_tmp17C;});
({struct Cyc_Lex_DynTrie*_tmpB8=Cyc_Lex_typedefs_trie;struct Cyc_Lex_DynTrie*_tmpB9=tdefs;Cyc_Lex_typedefs_trie=_tmpB9;tdefs=_tmpB8;});{
# 487
struct Cyc_List_List*_tmpBA=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->availables;struct Cyc_List_List*as=_tmpBA;for(0;as != 0;as=as->tl){
void*_tmpBB=(void*)as->hd;void*_stmttmp9=_tmpBB;void*_tmpBC=_stmttmp9;struct Cyc_List_List*_tmpBD;struct Cyc_List_List*_tmpBE;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpBC)->tag == 1U){_LL7: _tmpBE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpBC)->f1;_LL8: {struct Cyc_List_List*ns=_tmpBE;
_tmpBD=ns;goto _LLA;}}else{_LL9: _tmpBD=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpBC)->f1;_LLA: {struct Cyc_List_List*ns=_tmpBD;
# 491
struct Cyc_Lex_Ldecls*_tmpBF=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,ns);struct Cyc_Lex_Ldecls*ts=_tmpBF;
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ts->typedefs);
goto _LL6;}}_LL6:;}}}}}}}
# 499
static int Cyc_Lex_is_typedef_in_namespace(struct Cyc_List_List*ns,struct _fat_ptr*v){
struct Cyc_List_List*ans=((struct Cyc_List_List*(*)(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*))Cyc_Binding_resolve_rel_ns)(0U,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,ns);
struct Cyc_Lex_Ldecls*_tmpC0=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,ans);struct Cyc_Lex_Ldecls*ts=_tmpC0;
return((int(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_member)(ts->typedefs,v);}struct _tuple29{struct Cyc_List_List*f1;struct Cyc_Lex_Trie*f2;struct _fat_ptr f3;};
# 505
static int Cyc_Lex_is_typedef_body(struct _RegionHandle*d,struct _tuple29*env){
# 508
struct _tuple29 _tmpC1=*env;struct _tuple29 _stmttmpA=_tmpC1;struct _tuple29 _tmpC2=_stmttmpA;struct _fat_ptr _tmpC5;struct Cyc_Lex_Trie*_tmpC4;struct Cyc_List_List*_tmpC3;_LL1: _tmpC3=_tmpC2.f1;_tmpC4=_tmpC2.f2;_tmpC5=_tmpC2.f3;_LL2: {struct Cyc_List_List*ns=_tmpC3;struct Cyc_Lex_Trie*t=_tmpC4;struct _fat_ptr s=_tmpC5;
int len=(int)(_get_fat_size(s,sizeof(char))- (unsigned)1);
{int i=0;for(0;i < len;++ i){
union Cyc_Lex_TrieChildren _tmpC6=((struct Cyc_Lex_Trie*)_check_null(t))->children;union Cyc_Lex_TrieChildren _stmttmpB=_tmpC6;union Cyc_Lex_TrieChildren _tmpC7=_stmttmpB;struct Cyc_Lex_Trie**_tmpC8;struct Cyc_Lex_Trie*_tmpC9;struct Cyc_Lex_Trie*_tmpCB;int _tmpCA;switch((_tmpC7.One).tag){case 1U: _LL4: _LL5:
 return 0;case 2U: _LL6: _tmpCA=((_tmpC7.One).val).f1;_tmpCB=((_tmpC7.One).val).f2;if(_tmpCA != (int)*((const char*)_check_fat_subscript(s,sizeof(char),i))){_LL7: {int one_ch=_tmpCA;struct Cyc_Lex_Trie*one_trie=_tmpCB;
return 0;}}else{_LL8: _tmpC9=((_tmpC7.One).val).f2;_LL9: {struct Cyc_Lex_Trie*one_trie=_tmpC9;
t=one_trie;goto _LL3;}}default: _LLA: _tmpC8=(_tmpC7.Many).val;_LLB: {struct Cyc_Lex_Trie**arr=_tmpC8;
# 516
struct Cyc_Lex_Trie*_tmpCC=*((struct Cyc_Lex_Trie**)({typeof(arr )_tmp17D=arr;_check_known_subscript_notnull(_tmp17D,64U,sizeof(struct Cyc_Lex_Trie*),Cyc_Lex_trie_char((int)((const char*)s.curr)[i]));}));struct Cyc_Lex_Trie*next=_tmpCC;
if(next == 0)
return 0;
t=next;
goto _LL3;}}_LL3:;}}
# 522
return((struct Cyc_Lex_Trie*)_check_null(t))->shared_str;}}
# 524
static int Cyc_Lex_is_typedef(struct Cyc_List_List*ns,struct _fat_ptr*v){
if(ns != 0)
return Cyc_Lex_is_typedef_in_namespace(ns,v);{
# 529
struct _fat_ptr _tmpCD=*v;struct _fat_ptr s=_tmpCD;
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmpCE=tdefs;struct Cyc_Lex_DynTrie*_tmpCF=Cyc_Lex_typedefs_trie;tdefs=_tmpCF;Cyc_Lex_typedefs_trie=_tmpCE;});{
struct Cyc_Lex_DynTrie _tmpD0=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _stmttmpC=_tmpD0;struct Cyc_Lex_DynTrie _tmpD1=_stmttmpC;struct Cyc_Lex_Trie*_tmpD3;struct Cyc_Core_DynamicRegion*_tmpD2;_LL1: _tmpD2=_tmpD1.dyn;_tmpD3=_tmpD1.t;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmpD2;struct Cyc_Lex_Trie*t=_tmpD3;
struct _tuple29 _tmpD4=({struct _tuple29 _tmp150;_tmp150.f1=ns,_tmp150.f2=t,_tmp150.f3=s;_tmp150;});struct _tuple29 env=_tmpD4;
int _tmpD5=((int(*)(struct Cyc_Core_DynamicRegion*key,struct _tuple29*arg,int(*body)(struct _RegionHandle*h,struct _tuple29*arg)))Cyc_Core_open_region)(dyn,& env,Cyc_Lex_is_typedef_body);int res=_tmpD5;
({struct Cyc_Lex_DynTrie _tmp17E=({struct Cyc_Lex_DynTrie _tmp14F;_tmp14F.dyn=dyn,_tmp14F.t=t;_tmp14F;});*tdefs=_tmp17E;});
({struct Cyc_Lex_DynTrie*_tmpD6=tdefs;struct Cyc_Lex_DynTrie*_tmpD7=Cyc_Lex_typedefs_trie;tdefs=_tmpD7;Cyc_Lex_typedefs_trie=_tmpD6;});
return res;}}}}
# 540
void Cyc_Lex_enter_namespace(struct _fat_ptr*s){
((void(*)(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,int,struct Cyc_Lex_Ldecls*(*mkdata)(int)))Cyc_Binding_enter_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,s,1,Cyc_Lex_mk_empty_ldecls);{
struct Cyc_Lex_Ldecls*_tmpD8=({struct Cyc_Dict_Dict _tmp17F=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data;((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp17F,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);});struct Cyc_Lex_Ldecls*ts=_tmpD8;
# 545
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ts->typedefs);}}
# 547
void Cyc_Lex_leave_namespace(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
Cyc_Lex_recompute_typedefs();}
# 551
void Cyc_Lex_enter_using(struct _tuple0*q){
struct Cyc_List_List*_tmpD9=((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(0U,(struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v,q);struct Cyc_List_List*ns=_tmpD9;
struct Cyc_Lex_Ldecls*_tmpDA=((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data,ns);struct Cyc_Lex_Ldecls*ts=_tmpDA;
# 556
((void(*)(void(*f)(struct _fat_ptr*),struct Cyc_Set_Set*s))Cyc_Set_iter)(Cyc_Lex_insert_typedef,ts->typedefs);}
# 558
void Cyc_Lex_leave_using(){
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v);
# 561
Cyc_Lex_recompute_typedefs();}
# 564
void Cyc_Lex_register_typedef(struct _tuple0*q){
# 566
struct Cyc_Lex_Ldecls*_tmpDB=({struct Cyc_Dict_Dict _tmp180=((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->ns_data;((struct Cyc_Lex_Ldecls*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(_tmp180,((struct Cyc_Binding_NSCtxt*)((struct Cyc_Core_Opt*)_check_null(Cyc_Lex_lstate))->v)->curr_ns);});struct Cyc_Lex_Ldecls*ts=_tmpDB;
({struct Cyc_Set_Set*_tmp181=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _fat_ptr*elt))Cyc_Set_insert)(ts->typedefs,(*q).f2);ts->typedefs=_tmp181;});
# 571
Cyc_Lex_insert_typedef((*q).f2);}
# 574
static short Cyc_Lex_process_id(struct Cyc_Lexing_lexbuf*lbuf){
int symbol_num=Cyc_Lex_str_index_lbuf(lbuf);
# 578
if(symbol_num <= Cyc_Lex_num_kws){
# 580
if(!Cyc_Lex_in_extern_c ||(*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).is_c_keyword){
short res=(short)(*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),symbol_num - 1))).token_index;
return res;}}{
# 586
struct _fat_ptr*_tmpDC=Cyc_Lex_get_symbol(symbol_num);struct _fat_ptr*s=_tmpDC;
Cyc_Lex_token_string=*s;
# 590
if(Cyc_Lex_is_typedef(0,s))
return 375;
# 594
return 367;}}
# 597
static short Cyc_Lex_process_qual_id(struct Cyc_Lexing_lexbuf*lbuf){
if(Cyc_Lex_in_extern_c)
({struct _fat_ptr _tmp182=({const char*_tmpDD="qualified identifiers not allowed in C code";_tag_fat(_tmpDD,sizeof(char),44U);});Cyc_Lex_err(_tmp182,lbuf);});{
int i=lbuf->lex_start_pos;
int end=lbuf->lex_curr_pos;
struct _fat_ptr s=lbuf->lex_buffer;
# 604
struct _fat_ptr*v=0;
struct Cyc_List_List*rev_vs=0;
# 607
while(i < end){
int start=i;
for(0;i < end &&(int)*((char*)_check_fat_subscript(s,sizeof(char),i))!= (int)':';++ i){
;}
if(start == i)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp183=({const char*_tmpDE="bad namespace";_tag_fat(_tmpDE,sizeof(char),14U);});_tmpDF->f1=_tmp183;});_tmpDF;}));{
int vlen=i - start;
if(v != 0)
rev_vs=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->hd=v,_tmpE0->tl=rev_vs;_tmpE0;});
v=Cyc_Lex_get_symbol(Cyc_Lex_str_index((struct _fat_ptr)s,start,vlen));
i +=2;}}
# 619
if(v == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp184=({const char*_tmpE1="bad namespace";_tag_fat(_tmpE1,sizeof(char),14U);});_tmpE2->f1=_tmp184;});_tmpE2;}));{
struct Cyc_List_List*vs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_vs);
# 624
if(vs != 0 &&({struct _fat_ptr _tmp185=(struct _fat_ptr)*((struct _fat_ptr*)vs->hd);Cyc_strcmp(_tmp185,({const char*_tmpE3="Cyc";_tag_fat(_tmpE3,sizeof(char),4U);}));})== 0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_tmpE4=_cycalloc(sizeof(*_tmpE4));({union Cyc_Absyn_Nmspace _tmp186=Cyc_Absyn_Abs_n(vs,0);_tmpE4->f1=_tmp186;}),_tmpE4->f2=v;_tmpE4;});}else{
# 628
if(vs != 0 &&({struct _fat_ptr _tmp187=(struct _fat_ptr)*((struct _fat_ptr*)vs->hd);Cyc_strcmp(_tmp187,({const char*_tmpE5="C";_tag_fat(_tmpE5,sizeof(char),2U);}));})== 0){
vs=vs->tl;
Cyc_Lex_token_qvar=({struct _tuple0*_tmpE6=_cycalloc(sizeof(*_tmpE6));({union Cyc_Absyn_Nmspace _tmp188=Cyc_Absyn_Abs_n(vs,1);_tmpE6->f1=_tmp188;}),_tmpE6->f2=v;_tmpE6;});}else{
# 632
Cyc_Lex_token_qvar=({struct _tuple0*_tmpE7=_cycalloc(sizeof(*_tmpE7));({union Cyc_Absyn_Nmspace _tmp189=Cyc_Absyn_Rel_n(vs);_tmpE7->f1=_tmp189;}),_tmpE7->f2=v;_tmpE7;});}}
if(Cyc_Lex_is_typedef(vs,v))
return 377;
return 376;}}}struct Cyc_Lex_PosInfo{struct Cyc_Lex_PosInfo*next;unsigned starting_line;struct _fat_ptr filename;struct _fat_ptr linenumpos;unsigned linenumpos_offset;};
# 651
static struct Cyc_Lex_PosInfo*Cyc_Lex_pos_info=0;
# 653
static int Cyc_Lex_linenumber=1;
# 656
static struct Cyc_Lex_PosInfo*Cyc_Lex_rnew_filepos(struct _RegionHandle*r,struct _fat_ptr filename,unsigned starting_line,struct Cyc_Lex_PosInfo*next){
# 660
struct _fat_ptr linenumpos=({unsigned _tmpE9=10U;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(unsigned),_tmpE9),sizeof(unsigned),_tmpE9);});
*((unsigned*)_check_fat_subscript(linenumpos,sizeof(unsigned),0))=(unsigned)Cyc_yylloc.first_line;
return({struct Cyc_Lex_PosInfo*_tmpE8=_region_malloc(r,sizeof(*_tmpE8));_tmpE8->next=next,_tmpE8->starting_line=starting_line,_tmpE8->filename=(struct _fat_ptr)filename,_tmpE8->linenumpos=linenumpos,_tmpE8->linenumpos_offset=1U;_tmpE8;});}
# 669
static void Cyc_Lex_inc_linenumber(){
if(Cyc_Lex_pos_info == 0)(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp18A=({const char*_tmpEA="empty position info!";_tag_fat(_tmpEA,sizeof(char),21U);});_tmpEB->f1=_tmp18A;});_tmpEB;}));{
struct Cyc_Lex_PosInfo*p=(struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info);
struct _fat_ptr linenums=_tag_fat(0,0,0);
({struct _fat_ptr _tmpEC=p->linenumpos;struct _fat_ptr _tmpED=linenums;p->linenumpos=_tmpED;linenums=_tmpEC;});{
unsigned offset=p->linenumpos_offset;
unsigned n=_get_fat_size(linenums,sizeof(unsigned));
# 677
if(offset >= n){
# 679
struct _fat_ptr newlinenums=({unsigned _tmpF0=n * (unsigned)2;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(unsigned),_tmpF0),sizeof(unsigned),_tmpF0);});
{unsigned i=0U;for(0;i < n;++ i){
*((unsigned*)_check_fat_subscript(newlinenums,sizeof(unsigned),(int)i))=((unsigned*)linenums.curr)[(int)i];}}
({struct _fat_ptr _tmpEE=linenums;struct _fat_ptr _tmpEF=newlinenums;linenums=_tmpEF;newlinenums=_tmpEE;});
# 684
((void(*)(unsigned*ptr))Cyc_Core_ufree)((unsigned*)_untag_fat_ptr(newlinenums,sizeof(unsigned),1U));}
# 687
*((unsigned*)_check_fat_subscript(linenums,sizeof(unsigned),(int)offset))=(unsigned)Cyc_yylloc.first_line;
p->linenumpos_offset=offset + (unsigned)1;
++ Cyc_Lex_linenumber;
({struct _fat_ptr _tmpF1=p->linenumpos;struct _fat_ptr _tmpF2=linenums;p->linenumpos=_tmpF2;linenums=_tmpF1;});}}}
# 694
static void Cyc_Lex_process_directive(struct _fat_ptr line){
int i;
char buf[100U];
struct _fat_ptr filename=({const char*_tmp100="";_tag_fat(_tmp100,sizeof(char),1U);});
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmpF5=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp152;_tmp152.tag=2U,_tmp152.f1=& i;_tmp152;});struct Cyc_CharPtr_sa_ScanfArg_struct _tmpF6=({struct Cyc_CharPtr_sa_ScanfArg_struct _tmp151;_tmp151.tag=7U,({struct _fat_ptr _tmp18B=_tag_fat(buf,sizeof(char),100U);_tmp151.f1=_tmp18B;});_tmp151;});void*_tmpF3[2U];_tmpF3[0]=& _tmpF5,_tmpF3[1]=& _tmpF6;({struct _fat_ptr _tmp18D=(struct _fat_ptr)line;struct _fat_ptr _tmp18C=({const char*_tmpF4="# %d \"%s";_tag_fat(_tmpF4,sizeof(char),9U);});Cyc_sscanf(_tmp18D,_tmp18C,_tag_fat(_tmpF3,sizeof(void*),2U));});})== 2){
if(Cyc_Lex_compile_for_boot_r){
# 702
int i=(int)(Cyc_strlen(_tag_fat(buf,sizeof(char),100U))- (unsigned long)1);
int last_slash=-1;
while(i >= 0){
if((int)*((char*)_check_known_subscript_notnull(buf,100U,sizeof(char),i))== (int)'/'){last_slash=i;break;}
-- i;}
# 708
filename=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF9=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,({struct _fat_ptr _tmp18F=(struct _fat_ptr)((struct _fat_ptr)_fat_ptr_plus(({struct _fat_ptr _tmp18E=_tag_fat(buf,sizeof(char),100U);_fat_ptr_plus(_tmp18E,sizeof(char),last_slash);}),sizeof(char),1));_tmp153.f1=_tmp18F;});_tmp153;});void*_tmpF7[1U];_tmpF7[0]=& _tmpF9;({struct _fat_ptr _tmp190=({const char*_tmpF8="\"%s";_tag_fat(_tmpF8,sizeof(char),4U);});Cyc_aprintf(_tmp190,_tag_fat(_tmpF7,sizeof(void*),1U));});});}else{
# 710
filename=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpFC=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,({struct _fat_ptr _tmp191=(struct _fat_ptr)_tag_fat(buf,sizeof(char),100U);_tmp154.f1=_tmp191;});_tmp154;});void*_tmpFA[1U];_tmpFA[0]=& _tmpFC;({struct _fat_ptr _tmp192=({const char*_tmpFB="\"%s";_tag_fat(_tmpFB,sizeof(char),4U);});Cyc_aprintf(_tmp192,_tag_fat(_tmpFA,sizeof(void*),1U));});});}
if((Cyc_Lex_linenumber == i && Cyc_Lex_pos_info != 0)&& Cyc_strcmp((struct _fat_ptr)((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename,(struct _fat_ptr)filename)== 0)return;
Cyc_Lex_linenumber=i;}else{
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmpFF=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp155;_tmp155.tag=2U,_tmp155.f1=& i;_tmp155;});void*_tmpFD[1U];_tmpFD[0]=& _tmpFF;({struct _fat_ptr _tmp194=(struct _fat_ptr)line;struct _fat_ptr _tmp193=({const char*_tmpFE="# %d";_tag_fat(_tmpFE,sizeof(char),5U);});Cyc_sscanf(_tmp194,_tmp193,_tag_fat(_tmpFD,sizeof(void*),1U));});})== 1){
if(Cyc_Lex_linenumber == i)return;
Cyc_Lex_linenumber=i;
if(Cyc_Lex_pos_info != 0)filename=((struct Cyc_Lex_PosInfo*)_check_null(Cyc_Lex_pos_info))->filename;}else{
# 718
++ Cyc_Lex_linenumber;
return;}}
# 721
Cyc_Lex_pos_info=Cyc_Lex_rnew_filepos(Cyc_Core_heap_region,filename,(unsigned)Cyc_Lex_linenumber,Cyc_Lex_pos_info);}struct _tuple30{struct _fat_ptr f1;unsigned f2;};
# 726
struct _tuple30 Cyc_Lex_xlate_pos(unsigned char_offset){
{struct Cyc_Lex_PosInfo*p=Cyc_Lex_pos_info;for(0;p != 0;p=p->next){
unsigned first_char_offset=*((unsigned*)_check_fat_subscript(p->linenumpos,sizeof(unsigned),0));
if(char_offset < first_char_offset && p->next != 0)continue;{
# 732
unsigned base=0U;
unsigned size=p->linenumpos_offset;
while(size > (unsigned)1){
int half=(int)(size / (unsigned)2);
int mid=(int)(base + (unsigned)half);
if(char_offset > *((unsigned*)_check_fat_subscript(p->linenumpos,sizeof(unsigned),mid))){
base=base + (unsigned)half;
size=size - (unsigned)half;}else{
# 741
size=(unsigned)half;}}
# 744
return({struct _tuple30 _tmp156;_tmp156.f1=p->filename,_tmp156.f2=p->starting_line + base;_tmp156;});}}}
# 746
return({struct _tuple30 _tmp157;({struct _fat_ptr _tmp195=_tag_fat(0,0,0);_tmp157.f1=_tmp195;}),_tmp157.f2=0U;_tmp157;});}
# 750
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*);
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*);
# 762
int Cyc_yylex(struct Cyc_Lexing_lexbuf*lbuf,union Cyc_YYSTYPE*yylval,struct Cyc_Yyltype*yyllocptr){
# 764
int ans=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lex_token)(lbuf);
({int _tmp197=({int _tmp196=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_start)(lbuf);yyllocptr->first_line=_tmp196;});Cyc_yylloc.first_line=_tmp197;});
({int _tmp199=({int _tmp198=((int(*)(struct Cyc_Lexing_lexbuf*))Cyc_Lexing_lexeme_end)(lbuf);yyllocptr->last_line=_tmp198;});Cyc_yylloc.last_line=_tmp199;});
{int _tmp101=ans;switch(_tmp101){case 374U: _LL1: _LL2:
 goto _LL4;case 372U: _LL3: _LL4:
 goto _LL6;case 373U: _LL5: _LL6:
 goto _LL8;case 369U: _LL7: _LL8:
 goto _LLA;case 370U: _LL9: _LLA:
 goto _LLC;case 367U: _LLB: _LLC:
 goto _LLE;case 375U: _LLD: _LLE:
({union Cyc_YYSTYPE _tmp19A=({union Cyc_YYSTYPE _tmp158;(_tmp158.String_tok).tag=3U,(_tmp158.String_tok).val=Cyc_Lex_token_string;_tmp158;});*yylval=_tmp19A;});goto _LL0;case 376U: _LLF: _LL10:
 goto _LL12;case 377U: _LL11: _LL12:
({union Cyc_YYSTYPE _tmp19B=({union Cyc_YYSTYPE _tmp159;(_tmp159.QualId_tok).tag=5U,(_tmp159.QualId_tok).val=Cyc_Lex_token_qvar;_tmp159;});*yylval=_tmp19B;});goto _LL0;case 368U: _LL13: _LL14:
({union Cyc_YYSTYPE _tmp19C=({union Cyc_YYSTYPE _tmp15A;(_tmp15A.Int_tok).tag=1U,(_tmp15A.Int_tok).val=Cyc_Lex_token_int;_tmp15A;});*yylval=_tmp19C;});goto _LL0;case 371U: _LL15: _LL16:
({union Cyc_YYSTYPE _tmp19D=({union Cyc_YYSTYPE _tmp15B;(_tmp15B.Char_tok).tag=2U,(_tmp15B.Char_tok).val=Cyc_Lex_token_char;_tmp15B;});*yylval=_tmp19D;});goto _LL0;default: _LL17: _LL18:
# 780
 goto _LL0;}_LL0:;}
# 782
return ans;}
# 789
const int Cyc_Lex_lex_base[287U]={0,113,119,120,125,126,127,131,- 6,4,12,2,- 3,- 1,- 2,115,- 4,121,- 1,131,- 5,209,217,240,272,132,- 4,- 3,- 2,5,2,133,- 17,138,- 1,351,- 18,6,- 12,- 11,280,- 13,- 10,- 7,- 8,- 9,424,447,295,- 14,154,- 17,7,- 1,- 15,- 16,8,- 1,502,303,575,650,367,- 16,- 69,178,- 49,9,2,- 51,137,30,107,117,31,115,101,377,150,727,770,135,138,32,141,340,840,929,98,1004,1062,110,- 68,- 33,- 39,1137,1212,- 34,- 52,- 53,1287,103,1345,1420,1495,117,62,94,94,104,94,122,105,94,105,101,119,108,118,1570,128,127,125,1645,133,136,138,1730,137,134,125,138,130,147,137,1805,142,144,1890,159,168,147,177,171,164,183,1965,202,202,193,201,2050,193,203,197,201,197,235,245,251,2125,245,255,257,2210,250,2285,252,252,269,262,259,273,281,272,283,2370,269,277,291,280,2445,- 15,281,296,284,291,2530,- 25,347,- 37,- 22,348,- 24,- 42,- 36,- 45,353,- 46,2605,2634,518,- 21,500,336,337,- 20,351,737,2673,2704,2739,2779,502,362,2849,2887,724,375,376,370,410,403,725,429,430,- 19,422,726,432,434,- 18,426,- 50,453,- 31,- 49,10,378,2819,- 44,- 27,- 29,- 43,- 26,- 28,- 30,1,2927,2,396,480,400,401,407,408,418,419,420,421,425,3000,3084,- 66,- 60,- 59,- 58,- 57,- 56,- 55,- 54,- 61,- 64,- 65,809,443,- 62,- 63,- 67,- 38,- 35,- 32,549,- 47,11,- 23,570};
const int Cyc_Lex_lex_backtrk[287U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,- 1,5,3,4,- 1,- 1,- 1,2,- 1,2,- 1,5,- 1,2,- 1,2,2,2,- 1,- 1,- 1,1,3,15,- 1,15,- 1,18,- 1,1,- 1,- 1,13,- 1,- 1,- 1,- 1,- 1,- 1,14,13,- 1,15,- 1,1,- 1,- 1,- 1,14,- 1,17,12,- 1,13,12,- 1,- 1,47,- 1,48,68,- 1,68,68,68,68,68,68,68,68,68,17,19,68,68,68,68,68,0,0,68,68,68,68,- 1,- 1,- 1,2,0,- 1,- 1,- 1,0,- 1,- 1,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,- 1,39,- 1,- 1,- 1,- 1,- 1,- 1,20,19,- 1,- 1,19,19,19,- 1,19,- 1,20,20,17,18,17,17,- 1,16,16,16,16,16,17,17,18,18,18,- 1,18,17,17,17,- 1,17,- 1,- 1,- 1,- 1,48,- 1,20,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,46,- 1,47};
const int Cyc_Lex_lex_default[287U]={64,54,26,31,26,15,7,7,0,- 1,- 1,- 1,0,0,0,25,0,25,0,- 1,0,- 1,- 1,- 1,- 1,25,0,0,0,- 1,- 1,50,0,50,0,- 1,0,- 1,0,0,- 1,0,0,0,0,0,- 1,- 1,- 1,0,50,0,- 1,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,0,- 1,0,- 1,- 1,0,282,- 1,- 1,247,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,- 1,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,- 1,- 1,- 1,39,- 1,- 1,- 1,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,49,- 1,- 1,45,- 1,- 1,- 1,- 1,- 1,- 1,- 1,42,- 1,- 1,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,43,- 1,- 1,- 1,20,- 1,38,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,44,- 1,- 1,- 1,- 1,182,0,- 1,- 1,- 1,- 1,63,0,- 1,0,0,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,0,- 1,0,234,0,0,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,0,- 1,- 1,0,0,0,0,0,0,282,0,- 1,0,- 1};
const int Cyc_Lex_lex_trans[3341U]={0,0,0,0,0,0,0,0,0,65,66,65,65,67,8,14,14,14,63,236,236,283,0,0,0,0,0,0,0,0,0,0,65,68,69,70,13,71,72,73,278,277,74,75,13,76,77,78,79,80,80,80,80,80,80,80,80,80,81,14,82,83,84,285,85,86,86,86,86,86,86,86,86,86,86,86,87,86,86,86,86,86,86,86,86,86,86,86,86,86,86,281,246,191,88,89,90,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,55,91,- 1,56,- 1,27,28,32,26,52,33,27,28,16,8,29,17,9,- 1,- 1,- 1,- 1,279,241,283,51,- 1,284,27,57,53,34,- 1,- 1,27,244,97,18,102,242,243,- 1,14,- 1,- 1,280,10,14,93,- 1,- 1,11,102,245,183,- 1,21,21,21,21,21,21,21,21,286,- 1,286,286,165,233,196,14,128,197,234,192,193,194,30,188,189,167,58,113,- 1,114,248,286,235,35,- 1,168,139,115,116,19,120,117,140,118,14,- 1,- 1,121,119,14,14,- 1,124,122,14,94,123,125,126,127,136,129,14,131,132,133,14,- 1,14,134,14,130,22,135,137,138,147,92,24,24,24,24,24,24,24,24,23,23,23,23,23,23,23,23,23,23,152,141,142,143,144,145,153,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,23,146,148,149,150,151,161,154,23,23,23,23,23,23,155,156,157,23,23,23,23,23,23,12,12,12,12,12,12,12,12,48,48,48,48,48,48,48,48,158,23,23,23,23,23,23,49,49,49,49,49,49,49,49,62,62,62,62,62,62,62,62,159,160,14,162,163,37,164,166,177,169,51,170,- 1,171,- 1,172,- 1,36,- 1,173,174,175,- 1,20,12,176,38,178,- 1,- 1,- 1,39,179,180,- 1,- 1,181,184,185,186,40,40,40,40,40,40,40,40,187,190,195,- 1,198,205,206,41,41,41,41,41,41,41,41,41,238,240,239,239,239,239,239,239,239,239,239,239,276,206,106,221,272,271,107,108,42,205,206,270,269,12,26,109,219,51,20,110,51,111,268,267,266,265,8,112,236,264,43,237,44,206,45,221,46,47,47,47,47,47,47,47,47,47,47,273,219,51,0,36,51,36,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,225,226,226,230,0,231,231,47,47,47,47,47,47,36,273,36,47,47,47,47,47,47,0,274,274,274,274,274,274,274,274,39,225,226,226,230,42,231,231,47,47,47,47,47,47,59,59,59,59,59,59,59,59,0,283,0,208,284,208,0,38,209,209,209,209,209,209,209,209,209,209,207,0,222,286,0,286,286,0,0,206,0,36,0,0,0,0,0,0,45,0,0,0,0,14,12,0,286,0,26,0,0,0,207,0,222,0,20,0,0,0,8,206,43,36,44,0,60,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,- 1,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,0,0,0,0,0,0,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,199,0,211,211,211,211,211,211,211,211,212,212,209,209,209,209,209,209,209,209,209,209,0,201,202,0,0,220,227,232,213,0,- 1,0,0,0,51,226,231,214,0,0,215,199,0,200,200,200,200,200,200,200,200,200,200,201,202,0,0,220,227,232,213,0,0,0,201,202,51,226,231,214,0,203,215,273,0,0,0,0,0,0,204,0,275,275,275,275,275,275,275,275,0,0,0,0,0,0,201,202,0,0,0,0,0,203,0,0,0,0,0,0,0,0,204,100,100,100,100,100,100,100,100,100,100,101,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,0,0,0,0,100,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,98,0,0,0,0,99,0,0,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,101,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,0,0,0,0,100,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,96,96,96,96,96,96,96,96,96,96,0,0,0,0,0,0,0,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,0,0,0,0,96,0,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,0,0,0,0,95,0,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,0,0,0,0,0,0,0,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,0,0,0,0,95,0,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,96,96,96,96,96,96,96,96,96,96,0,0,0,0,0,0,0,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,0,0,0,0,96,0,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,100,100,100,100,100,100,100,100,100,100,101,0,0,0,0,0,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,0,0,0,0,100,0,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,0,0,0,0,104,0,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,105,0,0,0,0,0,0,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,0,0,0,0,103,0,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,104,104,104,104,104,104,104,104,104,104,0,0,0,0,0,0,0,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,0,0,0,0,104,0,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,210,210,210,210,210,210,210,210,210,210,0,0,0,0,0,0,0,0,0,0,0,201,202,0,0,0,0,199,202,200,200,200,200,200,200,200,200,200,200,0,0,0,0,0,0,0,0,0,- 1,0,201,202,0,201,202,0,0,203,0,0,202,0,0,0,0,0,204,0,209,209,209,209,209,209,209,209,209,209,0,0,0,0,201,202,0,0,0,0,0,203,202,0,0,0,0,0,202,0,204,210,210,210,210,210,210,210,210,210,210,0,0,0,0,0,0,0,0,0,0,0,201,202,202,0,0,0,0,202,202,0,0,0,199,- 1,211,211,211,211,211,211,211,211,212,212,0,0,0,0,0,0,0,0,201,202,0,201,202,0,0,202,0,0,228,0,0,0,0,0,0,0,0,229,199,0,212,212,212,212,212,212,212,212,212,212,0,0,0,201,202,0,0,0,0,0,228,201,202,0,0,0,0,0,223,229,0,0,0,0,0,0,0,224,0,0,239,239,239,239,239,239,239,239,239,239,0,0,0,201,202,0,0,0,0,0,223,201,202,0,0,0,0,0,202,224,216,216,216,216,216,216,216,216,216,216,0,0,0,0,0,0,0,216,216,216,216,216,216,201,202,0,0,0,0,0,202,0,0,0,0,0,0,0,216,216,216,216,216,216,216,216,216,216,0,216,216,216,216,216,216,216,216,216,216,216,216,0,0,0,249,0,217,0,0,250,0,0,0,0,0,218,0,0,251,251,251,251,251,251,251,251,0,216,216,216,216,216,216,252,0,0,0,0,217,0,0,0,0,0,0,0,0,218,0,0,0,0,0,0,0,0,0,0,0,0,0,0,253,0,0,0,0,254,255,0,0,0,256,0,0,0,0,0,0,0,257,0,0,0,258,0,259,0,260,0,261,262,262,262,262,262,262,262,262,262,262,0,0,0,0,0,0,0,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,0,0,0,0,0,0,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,263,0,0,0,0,0,0,0,0,262,262,262,262,262,262,262,262,262,262,0,0,0,0,0,0,0,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,0,0,0,0,0,0,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_Lex_lex_check[3341U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,0,9,29,37,52,56,67,237,284,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,0,30,0,0,0,247,249,0,0,11,0,0,0,0,0,0,0,0,0,0,0,0,0,0,10,0,0,0,68,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,71,74,83,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,15,1,73,2,2,3,17,2,3,4,4,5,6,4,5,6,7,25,31,7,72,76,70,33,15,70,2,1,2,3,17,73,4,75,88,5,101,76,76,50,19,25,31,72,6,19,91,33,7,6,105,75,106,7,19,19,19,19,19,19,19,19,65,50,65,65,108,78,81,19,110,81,78,82,82,82,4,84,84,107,1,112,15,113,73,65,78,3,17,107,109,114,115,5,111,116,109,117,19,25,31,111,118,19,19,33,120,121,19,91,122,124,125,126,129,128,19,130,131,132,19,50,19,133,19,128,19,134,136,137,141,0,21,21,21,21,21,21,21,21,22,22,22,22,22,22,22,22,22,22,139,140,140,142,143,144,139,22,22,22,22,22,22,23,23,23,23,23,23,23,23,23,23,145,147,148,149,150,152,153,23,23,23,23,23,23,154,155,156,22,22,22,22,22,22,24,24,24,24,24,24,24,24,40,40,40,40,40,40,40,40,157,23,23,23,23,23,23,48,48,48,48,48,48,48,48,59,59,59,59,59,59,59,59,158,159,35,161,162,35,163,165,167,168,1,169,15,170,73,171,2,3,17,172,173,174,4,5,6,175,35,177,7,25,31,35,178,179,70,33,180,183,184,185,35,35,35,35,35,35,35,35,186,189,192,50,197,204,205,35,62,62,62,62,62,62,62,62,77,238,77,77,77,77,77,77,77,77,77,77,250,207,85,214,252,253,85,85,35,204,205,254,255,35,35,85,218,219,35,85,220,85,256,257,258,259,35,85,234,260,35,234,35,207,35,214,35,46,46,46,46,46,46,46,46,46,46,275,218,219,- 1,221,220,222,46,46,46,46,46,46,47,47,47,47,47,47,47,47,47,47,224,225,227,229,- 1,230,232,47,47,47,47,47,47,221,251,222,46,46,46,46,46,46,- 1,251,251,251,251,251,251,251,251,58,224,225,227,229,58,230,232,47,47,47,47,47,47,58,58,58,58,58,58,58,58,- 1,282,- 1,201,282,201,- 1,58,201,201,201,201,201,201,201,201,201,201,203,- 1,213,286,- 1,286,286,- 1,- 1,203,- 1,213,- 1,- 1,- 1,- 1,- 1,- 1,58,- 1,- 1,- 1,- 1,58,58,- 1,286,- 1,58,- 1,- 1,- 1,203,- 1,213,- 1,58,- 1,- 1,- 1,58,203,58,213,58,- 1,58,60,60,60,60,60,60,60,60,60,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,- 1,- 1,- 1,- 1,- 1,- 1,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,60,61,61,61,61,61,61,61,61,61,61,- 1,234,- 1,- 1,- 1,- 1,- 1,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,- 1,- 1,- 1,- 1,- 1,- 1,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,61,79,- 1,79,79,79,79,79,79,79,79,79,79,208,208,208,208,208,208,208,208,208,208,- 1,79,79,- 1,- 1,217,223,228,79,- 1,282,- 1,- 1,- 1,217,223,228,79,- 1,- 1,79,80,- 1,80,80,80,80,80,80,80,80,80,80,79,79,- 1,- 1,217,223,228,79,- 1,- 1,- 1,80,80,217,223,228,79,- 1,80,79,274,- 1,- 1,- 1,- 1,- 1,- 1,80,- 1,274,274,274,274,274,274,274,274,- 1,- 1,- 1,- 1,- 1,- 1,80,80,- 1,- 1,- 1,- 1,- 1,80,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,80,86,86,86,86,86,86,86,86,86,86,86,- 1,- 1,- 1,- 1,- 1,- 1,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,- 1,- 1,- 1,- 1,86,- 1,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,86,87,- 1,- 1,- 1,- 1,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,87,87,87,87,87,87,87,87,87,87,87,- 1,- 1,- 1,- 1,- 1,- 1,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,- 1,- 1,- 1,- 1,87,- 1,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,87,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,- 1,- 1,- 1,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,- 1,- 1,- 1,- 1,89,- 1,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,89,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,- 1,- 1,- 1,- 1,90,- 1,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,90,95,95,95,95,95,95,95,95,95,95,- 1,- 1,- 1,- 1,- 1,- 1,- 1,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,- 1,- 1,- 1,- 1,95,- 1,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,95,96,96,96,96,96,96,96,96,96,96,- 1,- 1,- 1,- 1,- 1,- 1,- 1,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,- 1,- 1,- 1,- 1,96,- 1,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,96,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,- 1,- 1,- 1,- 1,102,- 1,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,102,103,103,103,103,103,103,103,103,103,103,103,- 1,- 1,- 1,- 1,- 1,- 1,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,- 1,- 1,- 1,- 1,103,- 1,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,103,104,104,104,104,104,104,104,104,104,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,- 1,- 1,- 1,- 1,104,- 1,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,104,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,- 1,- 1,- 1,- 1,119,- 1,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,119,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,123,- 1,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,123,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,127,127,127,127,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,- 1,- 1,- 1,- 1,127,119,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,135,135,135,135,135,135,135,135,135,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,- 1,- 1,- 1,- 1,135,123,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,- 1,- 1,- 1,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,- 1,- 1,- 1,- 1,138,127,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,138,146,146,146,146,146,146,146,146,146,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,- 1,- 1,- 1,- 1,146,135,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,146,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,151,151,151,151,151,151,151,151,151,151,- 1,- 1,- 1,- 1,- 1,- 1,- 1,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,- 1,- 1,- 1,- 1,151,138,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,151,160,160,160,160,160,160,160,160,160,160,- 1,- 1,- 1,- 1,- 1,- 1,- 1,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,- 1,- 1,- 1,- 1,160,146,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,160,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,164,164,164,164,164,164,164,164,164,164,- 1,- 1,- 1,- 1,- 1,- 1,- 1,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,- 1,- 1,- 1,- 1,164,151,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,164,166,166,166,166,166,166,166,166,166,166,- 1,- 1,- 1,- 1,- 1,- 1,- 1,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,- 1,- 1,- 1,- 1,166,160,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,166,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,176,176,176,176,176,176,176,176,176,176,- 1,- 1,- 1,- 1,- 1,- 1,- 1,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,- 1,- 1,- 1,- 1,176,164,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,176,181,181,181,181,181,181,181,181,181,181,- 1,- 1,- 1,- 1,- 1,- 1,- 1,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,- 1,- 1,- 1,- 1,181,166,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,181,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,- 1,- 1,- 1,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,- 1,- 1,- 1,- 1,187,176,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,187,199,199,199,199,199,199,199,199,199,199,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,199,199,- 1,- 1,- 1,- 1,200,199,200,200,200,200,200,200,200,200,200,200,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,181,- 1,200,200,- 1,199,199,- 1,- 1,200,- 1,- 1,199,- 1,- 1,- 1,- 1,- 1,200,- 1,209,209,209,209,209,209,209,209,209,209,- 1,- 1,- 1,- 1,200,200,- 1,- 1,- 1,- 1,- 1,200,209,- 1,- 1,- 1,- 1,- 1,209,- 1,200,210,210,210,210,210,210,210,210,210,210,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,210,210,209,- 1,- 1,- 1,- 1,210,209,- 1,- 1,- 1,211,187,211,211,211,211,211,211,211,211,211,211,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,210,210,- 1,211,211,- 1,- 1,210,- 1,- 1,211,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,211,212,- 1,212,212,212,212,212,212,212,212,212,212,- 1,- 1,- 1,211,211,- 1,- 1,- 1,- 1,- 1,211,212,212,- 1,- 1,- 1,- 1,- 1,212,211,- 1,- 1,- 1,- 1,- 1,- 1,- 1,212,- 1,- 1,239,239,239,239,239,239,239,239,239,239,- 1,- 1,- 1,212,212,- 1,- 1,- 1,- 1,- 1,212,239,239,- 1,- 1,- 1,- 1,- 1,239,212,215,215,215,215,215,215,215,215,215,215,- 1,- 1,- 1,- 1,- 1,- 1,- 1,215,215,215,215,215,215,239,239,- 1,- 1,- 1,- 1,- 1,239,- 1,- 1,- 1,- 1,- 1,- 1,- 1,216,216,216,216,216,216,216,216,216,216,- 1,215,215,215,215,215,215,216,216,216,216,216,216,- 1,- 1,- 1,248,- 1,216,- 1,- 1,248,- 1,- 1,- 1,- 1,- 1,216,- 1,- 1,248,248,248,248,248,248,248,248,- 1,216,216,216,216,216,216,248,- 1,- 1,- 1,- 1,216,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,216,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,248,- 1,- 1,- 1,- 1,248,248,- 1,- 1,- 1,248,- 1,- 1,- 1,- 1,- 1,- 1,- 1,248,- 1,- 1,- 1,248,- 1,248,- 1,248,- 1,248,261,261,261,261,261,261,261,261,261,261,- 1,- 1,- 1,- 1,- 1,- 1,- 1,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,- 1,- 1,- 1,- 1,- 1,- 1,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,261,262,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,262,262,262,262,262,262,262,262,262,262,- 1,- 1,- 1,- 1,- 1,- 1,- 1,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,- 1,- 1,- 1,- 1,- 1,- 1,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,262,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_Lex_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 796
int state;int base;int backtrk;
int c;
state=start_state;
# 800
if(state >= 0){
({int _tmp19E=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp19E;});
lbuf->lex_last_action=- 1;}else{
# 804
state=(- state)- 1;}
# 806
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_base,287U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=Cyc_Lex_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 814
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 818
c=256;}}else{
# 820
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 823
if(*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_check,3341U,sizeof(int),base + c))== state)
state=*((const int*)_check_known_subscript_notnull(Cyc_Lex_lex_trans,3341U,sizeof(int),base + c));else{
# 826
state=Cyc_Lex_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp19F=({const char*_tmp102="empty token";_tag_fat(_tmp102,sizeof(char),12U);});_tmp103->f1=_tmp19F;});_tmp103;}));else{
# 832
return lbuf->lex_last_action;}}else{
# 835
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 839
int Cyc_Lex_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp104=lexstate;switch(_tmp104){case 0U: _LL1: _LL2:
# 800 "lex.cyl"
 return(int)Cyc_Lex_process_id(lexbuf);case 1U: _LL3: _LL4:
# 803 "lex.cyl"
 return(int)Cyc_Lex_process_qual_id(lexbuf);case 2U: _LL5: _LL6:
# 806 "lex.cyl"
 Cyc_Lex_token_string=*Cyc_Lex_get_symbol(Cyc_Lex_str_index_lbuf(lexbuf));
return 374;case 3U: _LL7: _LL8:
# 813 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 340;case 4U: _LL9: _LLA:
# 814 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 333;case 5U: _LLB: _LLC:
# 815 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 339;case 6U: _LLD: _LLE:
# 816 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 338;case 7U: _LLF: _LL10:
# 817 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 342;case 8U: _LL11: _LL12:
# 818 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 337;case 9U: _LL13: _LL14:
# 819 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 330;case 10U: _LL15: _LL16:
# 820 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 331;case 11U: _LL17: _LL18:
# 821 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 332;case 12U: _LL19: _LL1A:
# 822 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 334;case 13U: _LL1B: _LL1C:
# 823 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 335;case 14U: _LL1D: _LL1E:
# 824 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 336;case 15U: _LL1F: _LL20:
# 825 "lex.cyl"
 -- lexbuf->lex_curr_pos;return 341;case 16U: _LL21: _LL22:
# 828 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,2,0,16);
return 368;case 17U: _LL23: _LL24:
# 831
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,8);
return 368;case 18U: _LL25: _LL26:
# 837 "lex.cyl"
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 368;case 19U: _LL27: _LL28:
# 840
 Cyc_Lex_token_int=Cyc_Lex_intconst(lexbuf,0,0,10);
return 368;case 20U: _LL29: _LL2A:
# 845 "lex.cyl"
 Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);
return 373;case 21U: _LL2B: _LL2C:
# 848
 return 350;case 22U: _LL2D: _LL2E:
# 849 "lex.cyl"
 return 351;case 23U: _LL2F: _LL30:
# 850 "lex.cyl"
 return 348;case 24U: _LL31: _LL32:
# 851 "lex.cyl"
 return 349;case 25U: _LL33: _LL34:
# 852 "lex.cyl"
 return 344;case 26U: _LL35: _LL36:
# 853 "lex.cyl"
 return 345;case 27U: _LL37: _LL38:
# 854 "lex.cyl"
 return 357;case 28U: _LL39: _LL3A:
# 855 "lex.cyl"
 return 358;case 29U: _LL3B: _LL3C:
# 856 "lex.cyl"
 return 354;case 30U: _LL3D: _LL3E:
# 857 "lex.cyl"
 return 355;case 31U: _LL3F: _LL40:
# 858 "lex.cyl"
 return 356;case 32U: _LL41: _LL42:
# 859 "lex.cyl"
 return 363;case 33U: _LL43: _LL44:
# 860 "lex.cyl"
 return 362;case 34U: _LL45: _LL46:
# 861 "lex.cyl"
 return 361;case 35U: _LL47: _LL48:
# 862 "lex.cyl"
 return 359;case 36U: _LL49: _LL4A:
# 863 "lex.cyl"
 return 360;case 37U: _LL4B: _LL4C:
# 864 "lex.cyl"
 return 352;case 38U: _LL4D: _LL4E:
# 865 "lex.cyl"
 return 353;case 39U: _LL4F: _LL50:
# 866 "lex.cyl"
 return 346;case 40U: _LL51: _LL52:
# 868
 return 347;case 41U: _LL53: _LL54:
# 869 "lex.cyl"
 return 365;case 42U: _LL55: _LL56:
# 870 "lex.cyl"
 return 343;case 43U: _LL57: _LL58:
# 871 "lex.cyl"
 return 364;case 44U: _LL59: _LL5A:
# 872 "lex.cyl"
 return 366;case 45U: _LL5B: _LL5C:
# 874
 return 315;case 46U: _LL5D: _LL5E:
# 877 "lex.cyl"
 Cyc_Lex_process_directive(Cyc_Lexing_lexeme(lexbuf));return Cyc_Lex_token(lexbuf);case 47U: _LL5F: _LL60:
# 878 "lex.cyl"
 return Cyc_Lex_token(lexbuf);case 48U: _LL61: _LL62:
# 879 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_token(lexbuf);case 49U: _LL63: _LL64:
# 880 "lex.cyl"
 Cyc_Lex_comment_depth=1;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
Cyc_Lex_comment(lexbuf);
return Cyc_Lex_token(lexbuf);case 50U: _LL65: _LL66:
# 886 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_strng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lex_get_stored_string();
return 369;case 51U: _LL67: _LL68:
# 894 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_wstrng(lexbuf)){
;}
Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lex_get_stored_string();
return 370;case 52U: _LL69: _LL6A:
# 902 "lex.cyl"
 Cyc_Lex_string_pos=0;
Cyc_Lex_runaway_start=Cyc_Lexing_lexeme_start(lexbuf);
while(Cyc_Lex_scan_charconst(lexbuf)){
;}
Cyc_Lex_token_string=(struct _fat_ptr)Cyc_Lex_get_stored_string();
return 372;case 53U: _LL6B: _LL6C:
# 909
 Cyc_Lex_token_char='\a';return 371;case 54U: _LL6D: _LL6E:
# 910 "lex.cyl"
 Cyc_Lex_token_char='\b';return 371;case 55U: _LL6F: _LL70:
# 911 "lex.cyl"
 Cyc_Lex_token_char='\f';return 371;case 56U: _LL71: _LL72:
# 912 "lex.cyl"
 Cyc_Lex_token_char='\n';return 371;case 57U: _LL73: _LL74:
# 913 "lex.cyl"
 Cyc_Lex_token_char='\r';return 371;case 58U: _LL75: _LL76:
# 914 "lex.cyl"
 Cyc_Lex_token_char='\t';return 371;case 59U: _LL77: _LL78:
# 915 "lex.cyl"
 Cyc_Lex_token_char='\v';return 371;case 60U: _LL79: _LL7A:
# 916 "lex.cyl"
 Cyc_Lex_token_char='\\';return 371;case 61U: _LL7B: _LL7C:
# 917 "lex.cyl"
 Cyc_Lex_token_char='\'';return 371;case 62U: _LL7D: _LL7E:
# 918 "lex.cyl"
 Cyc_Lex_token_char='"';return 371;case 63U: _LL7F: _LL80:
# 919 "lex.cyl"
 Cyc_Lex_token_char='?';return 371;case 64U: _LL81: _LL82:
# 922 "lex.cyl"
 Cyc_Lex_token_char=({union Cyc_Absyn_Cnst _tmp1A0=Cyc_Lex_intconst(lexbuf,2,1,8);Cyc_Lex_cnst2char(_tmp1A0,lexbuf);});
return 371;case 65U: _LL83: _LL84:
# 927 "lex.cyl"
 Cyc_Lex_token_char=({union Cyc_Absyn_Cnst _tmp1A1=Cyc_Lex_intconst(lexbuf,3,1,16);Cyc_Lex_cnst2char(_tmp1A1,lexbuf);});
return 371;case 66U: _LL85: _LL86:
# 931
 Cyc_Lex_token_char=Cyc_Lexing_lexeme_char(lexbuf,1);
return 371;case 67U: _LL87: _LL88:
# 935
 return - 1;case 68U: _LL89: _LL8A:
# 937
 return(int)Cyc_Lexing_lexeme_char(lexbuf,0);default: _LL8B: _LL8C:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_token_rec(lexbuf,lexstate);}_LL0:;}
# 941
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1A2=({const char*_tmp105="some action didn't return!";_tag_fat(_tmp105,sizeof(char),27U);});_tmp106->f1=_tmp1A2;});_tmp106;}));}
# 943
int Cyc_Lex_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_token_rec(lexbuf,0);}
int Cyc_Lex_scan_charconst_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp107=lexstate;switch(_tmp107){case 0U: _LL1: _LL2:
# 940 "lex.cyl"
 return 0;case 1U: _LL3: _LL4:
# 942
 Cyc_Lex_store_string_char('\a');return 1;case 2U: _LL5: _LL6:
# 943 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 3U: _LL7: _LL8:
# 944 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 4U: _LL9: _LLA:
# 945 "lex.cyl"
 Cyc_Lex_store_string_char('\n');return 1;case 5U: _LLB: _LLC:
# 946 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 6U: _LLD: _LLE:
# 947 "lex.cyl"
 Cyc_Lex_store_string_char('\t');return 1;case 7U: _LLF: _LL10:
# 948 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 8U: _LL11: _LL12:
# 949 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 9U: _LL13: _LL14:
# 950 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 10U: _LL15: _LL16:
# 951 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 11U: _LL17: _LL18:
# 952 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 12U: _LL19: _LL1A:
# 955 "lex.cyl"
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 13U: _LL1B: _LL1C:
# 958 "lex.cyl"
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));return 1;case 14U: _LL1D: _LL1E:
# 960
 Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,0));return 1;case 15U: _LL1F: _LL20:
# 962
 Cyc_Lex_inc_linenumber();({struct _fat_ptr _tmp1A3=({const char*_tmp108="wide character ends in newline";_tag_fat(_tmp108,sizeof(char),31U);});Cyc_Lex_runaway_err(_tmp1A3,lexbuf);});return 0;case 16U: _LL21: _LL22:
# 963 "lex.cyl"
({struct _fat_ptr _tmp1A4=({const char*_tmp109="unterminated wide character";_tag_fat(_tmp109,sizeof(char),28U);});Cyc_Lex_runaway_err(_tmp1A4,lexbuf);});return 0;case 17U: _LL23: _LL24:
# 964 "lex.cyl"
({struct _fat_ptr _tmp1A5=({const char*_tmp10A="bad character following backslash in wide character";_tag_fat(_tmp10A,sizeof(char),52U);});Cyc_Lex_err(_tmp1A5,lexbuf);});return 1;default: _LL25: _LL26:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_scan_charconst_rec(lexbuf,lexstate);}_LL0:;}
# 968
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1A6=({const char*_tmp10B="some action didn't return!";_tag_fat(_tmp10B,sizeof(char),27U);});_tmp10C->f1=_tmp1A6;});_tmp10C;}));}
# 970
int Cyc_Lex_scan_charconst(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_scan_charconst_rec(lexbuf,1);}
int Cyc_Lex_strng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp10D=lexstate;switch(_tmp10D){case 0U: _LL1: _LL2:
# 969 "lex.cyl"
 return 1;case 1U: _LL3: _LL4:
# 970 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_strng_next(lexbuf);case 2U: _LL5: _LL6:
# 971 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 3U: _LL7: _LL8:
# 973
 lexbuf->lex_curr_pos -=1;return 0;default: _LL9: _LLA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_next_rec(lexbuf,lexstate);}_LL0:;}
# 977
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1A7=({const char*_tmp10E="some action didn't return!";_tag_fat(_tmp10E,sizeof(char),27U);});_tmp10F->f1=_tmp1A7;});_tmp10F;}));}
# 979
int Cyc_Lex_strng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_next_rec(lexbuf,2);}
int Cyc_Lex_strng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp110=lexstate;switch(_tmp110){case 0U: _LL1: _LL2:
# 977 "lex.cyl"
 return Cyc_Lex_strng_next(lexbuf);case 1U: _LL3: _LL4:
# 978 "lex.cyl"
 Cyc_Lex_inc_linenumber();return 1;case 2U: _LL5: _LL6:
# 979 "lex.cyl"
 Cyc_Lex_store_string_char('\a');return 1;case 3U: _LL7: _LL8:
# 980 "lex.cyl"
 Cyc_Lex_store_string_char('\b');return 1;case 4U: _LL9: _LLA:
# 981 "lex.cyl"
 Cyc_Lex_store_string_char('\f');return 1;case 5U: _LLB: _LLC:
# 982 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('n');}else{
# 986
Cyc_Lex_store_string_char('\n');}
return 1;case 6U: _LLD: _LLE:
# 989 "lex.cyl"
 Cyc_Lex_store_string_char('\r');return 1;case 7U: _LLF: _LL10:
# 990 "lex.cyl"
 if(Cyc_Lex_expand_specials){
Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char('t');}else{
# 994
Cyc_Lex_store_string_char('\t');}
return 1;case 8U: _LL11: _LL12:
# 996 "lex.cyl"
 Cyc_Lex_store_string_char('\v');return 1;case 9U: _LL13: _LL14:
# 997 "lex.cyl"
 Cyc_Lex_store_string_char('\\');return 1;case 10U: _LL15: _LL16:
# 998 "lex.cyl"
 Cyc_Lex_store_string_char('\'');return 1;case 11U: _LL17: _LL18:
# 999 "lex.cyl"
 Cyc_Lex_store_string_char('"');return 1;case 12U: _LL19: _LL1A:
# 1000 "lex.cyl"
 Cyc_Lex_store_string_char('?');return 1;case 13U: _LL1B: _LL1C:
# 1003 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _tmp1A8=Cyc_Lex_intconst(lexbuf,1,0,8);Cyc_Lex_cnst2char(_tmp1A8,lexbuf);}));
return 1;case 14U: _LL1D: _LL1E:
# 1008 "lex.cyl"
 Cyc_Lex_store_string_char(({union Cyc_Absyn_Cnst _tmp1A9=Cyc_Lex_intconst(lexbuf,2,0,16);Cyc_Lex_cnst2char(_tmp1A9,lexbuf);}));
return 1;case 15U: _LL1F: _LL20:
# 1012
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 16U: _LL21: _LL22:
# 1014 "lex.cyl"
 Cyc_Lex_inc_linenumber();
({struct _fat_ptr _tmp1AA=({const char*_tmp111="string ends in newline";_tag_fat(_tmp111,sizeof(char),23U);});Cyc_Lex_runaway_err(_tmp1AA,lexbuf);});
return 0;case 17U: _LL23: _LL24:
# 1018 "lex.cyl"
({struct _fat_ptr _tmp1AB=({const char*_tmp112="unterminated string";_tag_fat(_tmp112,sizeof(char),20U);});Cyc_Lex_runaway_err(_tmp1AB,lexbuf);});
return 0;case 18U: _LL25: _LL26:
# 1021 "lex.cyl"
({struct _fat_ptr _tmp1AC=({const char*_tmp113="bad character following backslash in string";_tag_fat(_tmp113,sizeof(char),44U);});Cyc_Lex_err(_tmp1AC,lexbuf);});
return 1;default: _LL27: _LL28:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_strng_rec(lexbuf,lexstate);}_LL0:;}
# 1026
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1AD=({const char*_tmp114="some action didn't return!";_tag_fat(_tmp114,sizeof(char),27U);});_tmp115->f1=_tmp1AD;});_tmp115;}));}
# 1028
int Cyc_Lex_strng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_strng_rec(lexbuf,3);}
int Cyc_Lex_wstrng_next_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp116=lexstate;switch(_tmp116){case 0U: _LL1: _LL2:
# 1031 "lex.cyl"
 Cyc_Lex_store_string(({const char*_tmp117="\" L\"";_tag_fat(_tmp117,sizeof(char),5U);}));return 1;case 1U: _LL3: _LL4:
# 1032 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_wstrng_next(lexbuf);case 2U: _LL5: _LL6:
# 1033 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 3U: _LL7: _LL8:
# 1035
 lexbuf->lex_curr_pos -=1;return 0;default: _LL9: _LLA:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_next_rec(lexbuf,lexstate);}_LL0:;}
# 1039
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1AE=({const char*_tmp118="some action didn't return!";_tag_fat(_tmp118,sizeof(char),27U);});_tmp119->f1=_tmp1AE;});_tmp119;}));}
# 1041
int Cyc_Lex_wstrng_next(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_next_rec(lexbuf,4);}
int Cyc_Lex_wstrng_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp11A=lexstate;switch(_tmp11A){case 0U: _LL1: _LL2:
# 1038 "lex.cyl"
 return Cyc_Lex_wstrng_next(lexbuf);case 1U: _LL3: _LL4:
# 1040
 Cyc_Lex_store_string_char('\\');
Cyc_Lex_store_string_char(Cyc_Lexing_lexeme_char(lexbuf,1));
return 1;case 2U: _LL5: _LL6:
# 1046 "lex.cyl"
 Cyc_Lex_store_string((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));
return 1;case 3U: _LL7: _LL8:
# 1048 "lex.cyl"
 Cyc_Lex_inc_linenumber();
({struct _fat_ptr _tmp1AF=({const char*_tmp11B="string ends in newline";_tag_fat(_tmp11B,sizeof(char),23U);});Cyc_Lex_runaway_err(_tmp1AF,lexbuf);});
return 0;case 4U: _LL9: _LLA:
# 1051 "lex.cyl"
({struct _fat_ptr _tmp1B0=({const char*_tmp11C="unterminated string";_tag_fat(_tmp11C,sizeof(char),20U);});Cyc_Lex_runaway_err(_tmp1B0,lexbuf);});
return 0;case 5U: _LLB: _LLC:
# 1053 "lex.cyl"
({struct _fat_ptr _tmp1B1=({const char*_tmp11D="bad character following backslash in string";_tag_fat(_tmp11D,sizeof(char),44U);});Cyc_Lex_err(_tmp1B1,lexbuf);});
return 1;default: _LLD: _LLE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_wstrng_rec(lexbuf,lexstate);}_LL0:;}
# 1058
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1B2=({const char*_tmp11E="some action didn't return!";_tag_fat(_tmp11E,sizeof(char),27U);});_tmp11F->f1=_tmp1B2;});_tmp11F;}));}
# 1060
int Cyc_Lex_wstrng(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_wstrng_rec(lexbuf,5);}
int Cyc_Lex_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_Lex_lex_engine(lexstate,lexbuf);
{int _tmp120=lexstate;switch(_tmp120){case 0U: _LL1: _LL2:
# 1056 "lex.cyl"
 ++ Cyc_Lex_comment_depth;return Cyc_Lex_comment(lexbuf);case 1U: _LL3: _LL4:
# 1057 "lex.cyl"
 -- Cyc_Lex_comment_depth;
if(Cyc_Lex_comment_depth > 0)
return Cyc_Lex_comment(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 1062 "lex.cyl"
({struct _fat_ptr _tmp1B3=({const char*_tmp121="unterminated comment";_tag_fat(_tmp121,sizeof(char),21U);});Cyc_Lex_runaway_err(_tmp1B3,lexbuf);});
return 0;case 3U: _LL7: _LL8:
# 1065 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 4U: _LL9: _LLA:
# 1066 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);case 5U: _LLB: _LLC:
# 1067 "lex.cyl"
 Cyc_Lex_inc_linenumber();return Cyc_Lex_comment(lexbuf);case 6U: _LLD: _LLE:
# 1068 "lex.cyl"
 return Cyc_Lex_comment(lexbuf);default: _LLF: _LL10:
(lexbuf->refill_buff)(lexbuf);
return Cyc_Lex_comment_rec(lexbuf,lexstate);}_LL0:;}
# 1072
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp1B4=({const char*_tmp122="some action didn't return!";_tag_fat(_tmp122,sizeof(char),27U);});_tmp123->f1=_tmp1B4;});_tmp123;}));}
# 1074
int Cyc_Lex_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_Lex_comment_rec(lexbuf,6);}
# 1080 "lex.cyl"
void Cyc_Lex_pos_init(){
Cyc_Lex_linenumber=1;
Cyc_Lex_pos_info=0;}
# 1085
static struct Cyc_Xarray_Xarray*Cyc_Lex_empty_xarray(struct _RegionHandle*id_rgn,int dummy){
struct Cyc_Xarray_Xarray*symbols=({
struct _RegionHandle*_tmp1B6=id_rgn;((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*,int,struct _fat_ptr*))Cyc_Xarray_rcreate)(_tmp1B6,101,({struct _fat_ptr*_tmp125=_cycalloc(sizeof(*_tmp125));({struct _fat_ptr _tmp1B5=(struct _fat_ptr)({const char*_tmp124="";_tag_fat(_tmp124,sizeof(char),1U);});*_tmp125=_tmp1B5;});_tmp125;}));});
# 1089
((void(*)(struct Cyc_Xarray_Xarray*,struct _fat_ptr*))Cyc_Xarray_add)(symbols,& Cyc_Lex_bogus_string);
return symbols;}
# 1093
void Cyc_Lex_lex_init(int include_cyclone_keywords){
# 1095
Cyc_Lex_in_extern_c=0;{
struct Cyc_List_List*x=0;
({struct Cyc_List_List*_tmp126=Cyc_Lex_prev_extern_c;struct Cyc_List_List*_tmp127=x;Cyc_Lex_prev_extern_c=_tmp127;x=_tmp126;});
while(x != 0){
struct Cyc_List_List*t=x->tl;
((void(*)(struct Cyc_List_List*ptr))Cyc_Core_ufree)(x);
x=t;}
# 1104
if(Cyc_Lex_ids_trie != 0){
struct Cyc_Lex_DynTrieSymbols*idt=0;
({struct Cyc_Lex_DynTrieSymbols*_tmp128=idt;struct Cyc_Lex_DynTrieSymbols*_tmp129=Cyc_Lex_ids_trie;idt=_tmp129;Cyc_Lex_ids_trie=_tmp128;});{
struct Cyc_Lex_DynTrieSymbols _tmp12A=*((struct Cyc_Lex_DynTrieSymbols*)_check_null(idt));struct Cyc_Lex_DynTrieSymbols _stmttmpD=_tmp12A;struct Cyc_Lex_DynTrieSymbols _tmp12B=_stmttmpD;struct Cyc_Core_DynamicRegion*_tmp12C;_LL1: _tmp12C=_tmp12B.dyn;_LL2: {struct Cyc_Core_DynamicRegion*dyn=_tmp12C;
Cyc_Core_free_ukey(dyn);
((void(*)(struct Cyc_Lex_DynTrieSymbols*ptr))Cyc_Core_ufree)(idt);}}}
# 1111
if(Cyc_Lex_typedefs_trie != 0){
struct Cyc_Lex_DynTrie*tdefs=0;
({struct Cyc_Lex_DynTrie*_tmp12D=tdefs;struct Cyc_Lex_DynTrie*_tmp12E=Cyc_Lex_typedefs_trie;tdefs=_tmp12E;Cyc_Lex_typedefs_trie=_tmp12D;});{
struct Cyc_Lex_DynTrie _tmp12F=*((struct Cyc_Lex_DynTrie*)_check_null(tdefs));struct Cyc_Lex_DynTrie _stmttmpE=_tmp12F;struct Cyc_Lex_DynTrie _tmp130=_stmttmpE;struct Cyc_Core_DynamicRegion*_tmp131;_LL4: _tmp131=_tmp130.dyn;_LL5: {struct Cyc_Core_DynamicRegion*dyn=_tmp131;
Cyc_Core_free_ukey(dyn);
((void(*)(struct Cyc_Lex_DynTrie*ptr))Cyc_Core_ufree)(tdefs);}}}{
# 1119
struct Cyc_Core_NewDynamicRegion _tmp132=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmpF=_tmp132;struct Cyc_Core_NewDynamicRegion _tmp133=_stmttmpF;struct Cyc_Core_DynamicRegion*_tmp134;_LL7: _tmp134=_tmp133.key;_LL8: {struct Cyc_Core_DynamicRegion*id_dyn=_tmp134;
struct Cyc_Lex_Trie*_tmp135=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(id_dyn,0,Cyc_Lex_empty_trie);struct Cyc_Lex_Trie*ts=_tmp135;
struct Cyc_Xarray_Xarray*_tmp136=((struct Cyc_Xarray_Xarray*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Xarray_Xarray*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(id_dyn,0,Cyc_Lex_empty_xarray);struct Cyc_Xarray_Xarray*xa=_tmp136;
Cyc_Lex_ids_trie=({struct Cyc_Lex_DynTrieSymbols*_tmp137=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp137));_tmp137->dyn=id_dyn,_tmp137->t=ts,_tmp137->symbols=xa;_tmp137;});{
# 1124
struct Cyc_Core_NewDynamicRegion _tmp138=Cyc_Core__new_ukey("internal-error","internal-error",0);struct Cyc_Core_NewDynamicRegion _stmttmp10=_tmp138;struct Cyc_Core_NewDynamicRegion _tmp139=_stmttmp10;struct Cyc_Core_DynamicRegion*_tmp13A;_LLA: _tmp13A=_tmp139.key;_LLB: {struct Cyc_Core_DynamicRegion*typedefs_dyn=_tmp13A;
struct Cyc_Lex_Trie*t=((struct Cyc_Lex_Trie*(*)(struct Cyc_Core_DynamicRegion*key,int arg,struct Cyc_Lex_Trie*(*body)(struct _RegionHandle*h,int arg)))Cyc_Core_open_region)(typedefs_dyn,0,Cyc_Lex_empty_trie);
Cyc_Lex_typedefs_trie=({struct Cyc_Lex_DynTrie*_tmp13B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp13B));_tmp13B->dyn=typedefs_dyn,_tmp13B->t=t;_tmp13B;});
Cyc_Lex_num_kws=Cyc_Lex_num_keywords(include_cyclone_keywords);
Cyc_Lex_kw_nums=({unsigned _tmp13D=(unsigned)Cyc_Lex_num_kws;struct Cyc_Lex_KeyWordInfo*_tmp13C=_cycalloc(_check_times(_tmp13D,sizeof(struct Cyc_Lex_KeyWordInfo)));({{unsigned _tmp15C=(unsigned)Cyc_Lex_num_kws;unsigned i;for(i=0;i < _tmp15C;++ i){(_tmp13C[i]).token_index=0,(_tmp13C[i]).is_c_keyword=0;}}0;});_tag_fat(_tmp13C,sizeof(struct Cyc_Lex_KeyWordInfo),_tmp13D);});{
unsigned i=0U;
unsigned rwsze=82U;
{unsigned j=0U;for(0;j < rwsze;++ j){
if(include_cyclone_keywords ||(Cyc_Lex_rw_array[(int)j]).f3){
struct _fat_ptr _tmp13E=(Cyc_Lex_rw_array[(int)j]).f1;struct _fat_ptr str=_tmp13E;
({struct _fat_ptr _tmp1B7=str;Cyc_Lex_str_index(_tmp1B7,0,(int)Cyc_strlen((struct _fat_ptr)str));});
({struct Cyc_Lex_KeyWordInfo _tmp1B8=({struct Cyc_Lex_KeyWordInfo _tmp15D;_tmp15D.token_index=(int)(Cyc_Lex_rw_array[(int)j]).f2,_tmp15D.is_c_keyword=(Cyc_Lex_rw_array[(int)j]).f3;_tmp15D;});*((struct Cyc_Lex_KeyWordInfo*)_check_fat_subscript(Cyc_Lex_kw_nums,sizeof(struct Cyc_Lex_KeyWordInfo),(int)i))=_tmp1B8;});
++ i;}}}
# 1139
Cyc_Lex_typedef_init();
Cyc_Lex_comment_depth=0;}}}}}}}
