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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;
# 292 "core.h"
void Cyc_Core_rethrow(void*);
# 38 "cycboot.h"
extern int Cyc_open(const char*,int,struct _fat_ptr);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
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
extern int Cyc_fputs(const char*,struct Cyc___cycFILE*);
# 224 "cycboot.h"
extern int Cyc_vfprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 300 "cycboot.h"
extern int isspace(int);
# 310
extern int toupper(int);
# 318
extern int system(const char*);
extern void exit(int);
# 331
extern int mkdir(const char*,unsigned short);
# 334
extern int close(int);
extern int chdir(const char*);
extern struct _fat_ptr Cyc_getcwd(struct _fat_ptr,unsigned long);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
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
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
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
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);extern char Cyc_Set_Absent[7U];
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
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);extern char Cyc_Arg_Bad[4U];extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
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
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 1061 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1112
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1118
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1126
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
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);
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
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp3F7=Cyc_stderr;struct _fat_ptr _tmp3F6=({const char*_tmp3="Internal error: log file is NULL\n";_tag_fat(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp3F7,_tmp3F6,_tag_fat(_tmp2,sizeof(void*),0));});});
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
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp3F8=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);*_tmp5=_tmp3F8;});_tmp5;});}struct _tuple13{struct _fat_ptr*f1;struct Cyc_Set_Set*f2;};
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
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0,_tmp399.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp399;});void*_tmp8[1];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp3FA=Cyc_stderr;struct _fat_ptr _tmp3F9=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_fat(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp3FA,_tmp3F9,_tag_fat(_tmp8,sizeof(void*),1));});});
return symbol;}
# 183
return(struct _fat_ptr)({struct _fat_ptr _tmp3FD=(struct _fat_ptr)symbol;int _tmp3FC=(int)prefix_len;Cyc_substring(_tmp3FD,_tmp3FC,({unsigned long _tmp3FB=Cyc_strlen((struct _fat_ptr)symbol);_tmp3FB - prefix_len;}));});}
# 186
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;void*_tmpD;switch(*((int*)_tmpC)){case 5: _tmpD=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_tmpD;
# 189
({struct _fat_ptr*_tmp3FF=({struct _fat_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _fat_ptr _tmp3FE=Cyc_remove_user_prefix(*(*ad->name).f2);*_tmpE=_tmp3FE;});_tmpE;});(*ad->name).f2=_tmp3FF;});goto _LL0;}case 7: _tmpD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;{struct Cyc_Absyn_Enumdecl*ed=_tmpD;
# 191
({struct _fat_ptr*_tmp401=({struct _fat_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _fat_ptr _tmp400=Cyc_remove_user_prefix(*(*ed->name).f2);*_tmpF=_tmp400;});_tmpF;});(*ed->name).f2=_tmp401;});goto _LL0;}case 8: _tmpD=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;{struct Cyc_Absyn_Typedefdecl*td=_tmpD;
# 193
({struct _fat_ptr*_tmp403=({struct _fat_ptr*_tmp10=_cycalloc(sizeof(*_tmp10));({struct _fat_ptr _tmp402=Cyc_remove_user_prefix(*(*td->name).f2);*_tmp10=_tmp402;});_tmp10;});(*td->name).f2=_tmp403;});goto _LL0;}case 1:
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
({void*_tmp11=0U;({struct Cyc___cycFILE*_tmp405=Cyc_stderr;struct _fat_ptr _tmp404=({const char*_tmp12="Error in .cys file: bad user-defined type definition\n";_tag_fat(_tmp12,sizeof(char),54U);});Cyc_fprintf(_tmp405,_tmp404,_tag_fat(_tmp11,sizeof(void*),0));});});
exit(1);}_LL0:;}
# 216
const int Cyc_lex_base[444U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 24,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 21,302,306,304,314,321,324,- 16,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 18,416,420,415,437,433,450,429,452,455,443,447,438,438,- 20,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,645,592,594,582,584,587,589,590,584,609,619,634,635,1,691,697,691,692,634,636,644,645,2,703,649,650,1078,673,684,685,686,649,647,662,678,679,734,735,737,- 15,711,713,768,770,797,744,754,809,810,811,- 12,758,759,814,815,816,763,764,819,820,821,- 13,2601,1254,- 23,769,1365,- 22,1278,768,769,766,765,764,759,761,764,765,763,776,1370,779,780,778,791,1480,1504,794,795,812,813,811,824,1597,817,857,855,868,1599,- 7,- 8,8,1372,2570,9,930,2603,2658,1103,1390,- 49,1257,- 2,907,- 4,908,1111,1251,909,1104,1119,1446,925,2685,2728,929,928,930,975,2798,931,1053,- 36,- 42,- 37,2873,- 28,942,- 40,- 25,977,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1086,3189,3227,1078,1120,1114,1200,1192,1203,- 6,- 34,993,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,1002,3267,1029,1040,1806,1043,1045,1078,1121,1127,1129,1130,1134,1139,3340,3424,- 17,1908,1143,- 19,- 41,- 38,- 35,- 26,1377,3506,1194,3589,1186,15,1129,1170,1170,1168,1164,1177,1250};
const int Cyc_lex_backtrk[444U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,23,23,23,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,16,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,18,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[444U]={- 1,- 1,- 1,338,327,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,243,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,436,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,242,251,437,208,22,- 1,- 1,22,- 1,- 1,48,209,46,228,22,434,434,434,434,434,434,434,434,434,434,31,106,22,210,117,42,219,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,31,222,223,226,434,135,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,432,432,432,432,432,432,432,432,432,432,124,20,77,22,70,57,58,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,59,60,61,62,432,63,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,64,65,132,339,340,339,339,340,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,339,341,342,75,78,343,344,345,48,48,346,347,48,348,349,350,351,352,352,352,352,352,352,352,352,352,353,79,354,355,356,48,19,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,19,- 1,- 1,358,357,80,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,328,359,104,329,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,330,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,331,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,290,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,49,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,229,103,230,- 1,231,- 1,232,233,104,104,246,52,104,234,235,236,237,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,238,148,149,150,- 1,151,- 1,43,43,152,239,43,240,241,243,245,94,247,153,53,248,244,249,250,54,55,252,253,254,284,43,56,142,142,142,142,142,142,142,142,279,273,268,260,261,262,44,44,44,44,44,44,44,44,44,44,263,264,265,266,36,267,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,270,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,105,274,45,35,35,35,35,35,35,35,35,275,276,277,278,280,281,282,283,96,285,286,287,288,289,37,294,309,304,300,301,302,303,102,305,306,307,308,38,39,39,39,39,39,39,39,39,39,39,310,311,312,313,322,317,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,318,319,320,321,39,323,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,- 1,22,33,33,33,33,33,33,33,33,33,33,324,325,326,430,429,406,31,33,33,33,33,33,33,336,336,336,336,336,336,336,336,397,372,368,369,370,367,362,41,41,41,41,41,41,41,41,41,41,366,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,364,365,371,400,41,143,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,196,41,41,41,41,41,41,41,41,41,41,426,36,36,160,35,182,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,360,35,41,105,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,255,404,46,427,256,31,31,31,31,31,31,31,31,257,278,258,35,31,401,405,289,46,49,267,314,314,428,167,314,296,296,361,423,296,402,403,295,47,47,47,47,47,47,47,47,47,47,314,35,31,31,259,296,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,21,21,31,438,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,315,- 1,46,291,291,297,431,291,431,431,316,439,440,441,442,298,102,396,443,396,299,46,21,0,0,0,291,296,296,431,- 1,296,0,0,292,0,0,47,47,47,47,47,47,47,47,47,47,0,102,396,296,396,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,408,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,294,294,81,297,294,308,308,337,337,308,0,337,431,298,431,431,0,0,0,0,0,81,0,294,0,337,337,0,308,337,337,295,21,0,0,431,19,82,82,82,82,82,82,82,82,82,82,0,337,0,21,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,313,313,83,398,313,399,399,399,399,399,399,399,399,399,399,0,0,0,- 1,0,0,83,0,313,314,314,0,0,314,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,314,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,315,0,92,321,321,326,326,321,0,326,316,379,379,379,379,379,379,379,379,379,379,92,0,0,0,0,321,0,326,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,378,95,378,0,0,379,379,379,379,379,379,379,379,379,379,0,382,0,382,0,95,383,383,383,383,383,383,383,383,383,383,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,383,383,383,383,383,383,383,383,383,383,295,0,0,0,0,0,0,0,100,424,424,424,424,424,424,424,424,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,295,0,103,0,0,0,0,0,0,425,425,425,425,425,425,425,425,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,332,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,291,291,0,0,291,0,0,0,333,333,333,333,333,333,333,333,0,0,0,0,0,0,0,291,0,141,141,141,141,141,141,292,0,0,0,0,0,0,0,0,0,335,335,335,335,335,335,335,335,335,335,0,0,0,0,0,0,28,335,335,335,335,335,335,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,334,0,0,0,0,0,293,0,0,0,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,0,0,0,0,0,0,0,335,335,335,335,335,335,0,0,373,0,384,384,384,384,384,384,384,384,385,385,0,0,0,0,0,0,0,0,0,0,0,375,335,335,335,335,335,335,386,0,0,0,0,0,0,0,0,387,0,0,388,373,0,374,374,374,374,374,374,374,374,374,374,375,0,0,0,0,0,0,386,0,0,0,375,0,0,0,0,387,0,376,388,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,375,0,0,0,0,0,0,376,0,0,0,0,0,0,0,0,377,363,363,363,363,363,363,363,363,363,363,0,0,0,0,0,0,0,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,0,0,0,0,363,0,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,0,0,0,0,0,0,0,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,0,0,0,0,363,0,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,380,380,380,380,380,380,380,380,380,380,0,0,0,0,0,0,0,0,0,0,0,381,96,0,0,0,0,373,96,374,374,374,374,374,374,374,374,374,374,379,379,379,379,379,379,379,379,379,379,0,375,0,0,381,96,0,0,376,0,0,96,94,0,0,0,0,377,94,0,380,380,380,380,380,380,380,380,380,380,0,0,0,375,0,0,0,0,0,0,376,381,96,0,94,0,0,0,96,377,94,0,0,383,383,383,383,383,383,383,383,383,383,0,0,0,0,0,0,0,0,0,0,381,96,96,0,0,0,0,96,96,373,0,384,384,384,384,384,384,384,384,385,385,0,0,0,0,0,0,0,0,0,0,0,375,0,96,0,0,0,0,394,96,0,0,0,0,0,0,373,395,385,385,385,385,385,385,385,385,385,385,0,0,0,0,0,375,0,0,0,0,0,375,394,0,0,0,0,0,392,0,0,395,0,0,0,0,0,393,0,0,399,399,399,399,399,399,399,399,399,399,0,0,0,375,0,0,0,0,0,0,392,381,96,0,0,0,0,0,96,393,389,389,389,389,389,389,389,389,389,389,0,0,0,0,0,0,0,389,389,389,389,389,389,381,96,0,0,0,0,0,96,0,0,0,0,0,0,0,389,389,389,389,389,389,389,389,389,389,0,389,389,389,389,389,389,389,389,389,389,389,389,0,0,0,409,0,390,0,0,410,0,0,0,0,0,391,0,0,411,411,411,411,411,411,411,411,0,389,389,389,389,389,389,412,0,0,0,0,390,0,0,0,0,0,0,0,0,391,0,0,0,0,0,0,0,0,0,0,0,0,0,0,413,0,0,0,0,414,415,0,0,0,416,0,0,0,0,0,0,0,417,0,0,0,418,0,419,0,420,0,421,422,422,422,422,422,422,422,422,422,422,0,0,0,0,0,0,0,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,0,0,0,0,0,0,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,292,0,0,0,0,0,0,0,0,422,422,422,422,422,422,422,422,422,422,0,0,0,0,0,0,0,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,0,0,0,0,0,0,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,19,0,0,433,0,0,0,432,432,432,432,432,432,432,432,432,432,0,0,0,0,0,0,0,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,0,0,0,0,432,0,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,435,0,0,0,0,0,0,0,434,434,434,434,434,434,434,434,434,434,0,0,0,0,0,0,0,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,0,0,0,0,434,0,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,329,332,43,43,46,46,43,436,46,- 1,- 1,- 1,- 1,- 1,123,241,250,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,228,103,229,106,230,113,231,231,104,104,232,14,104,233,234,235,236,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,237,147,147,147,114,147,135,15,15,147,238,15,239,240,242,244,245,246,147,14,247,243,248,249,14,14,251,252,253,255,15,14,139,139,139,139,139,139,139,139,256,257,258,259,260,261,15,15,15,15,15,15,15,15,15,15,262,263,264,265,103,266,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,275,276,277,279,280,281,282,283,284,285,286,287,288,16,293,297,298,299,300,301,302,303,304,305,306,307,16,16,16,16,16,16,16,16,16,16,16,309,310,311,312,315,316,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,317,318,319,320,16,322,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,243,16,33,33,33,33,33,33,33,33,33,33,323,324,325,341,343,346,15,33,33,33,33,33,33,333,333,333,333,333,333,333,333,350,353,354,354,354,355,358,39,39,39,39,39,39,39,39,39,39,365,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,356,356,368,398,39,407,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,409,41,41,41,41,41,41,41,41,41,41,410,376,377,412,386,413,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,376,377,359,386,41,414,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,254,347,44,344,254,336,336,336,336,336,336,336,336,254,415,254,387,390,348,347,416,44,417,418,144,144,344,419,144,145,145,359,420,145,348,348,425,44,44,44,44,44,44,44,44,44,44,144,387,390,391,254,145,392,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,433,435,391,437,44,392,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,345,47,291,291,145,339,291,339,339,144,438,439,440,441,145,393,394,442,395,145,47,443,- 1,- 1,- 1,291,296,296,339,345,296,- 1,- 1,291,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,393,394,296,395,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,345,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,294,294,81,296,294,308,308,330,330,308,- 1,330,431,296,431,431,- 1,- 1,- 1,- 1,- 1,81,- 1,294,- 1,337,337,- 1,308,337,330,294,330,- 1,- 1,431,308,81,81,81,81,81,81,81,81,81,81,- 1,337,- 1,337,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,313,313,82,349,313,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,345,- 1,- 1,82,- 1,313,314,314,- 1,- 1,314,- 1,- 1,313,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,314,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,314,- 1,92,321,321,326,326,321,- 1,326,314,378,378,378,378,378,378,378,378,378,378,92,- 1,- 1,- 1,- 1,321,- 1,326,- 1,- 1,- 1,- 1,- 1,321,- 1,326,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,375,93,375,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,381,- 1,381,- 1,93,381,381,381,381,381,381,381,381,381,381,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,382,382,382,382,382,382,382,382,382,382,411,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,424,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,424,424,424,424,424,424,424,424,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,331,- 1,- 1,331,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,290,290,- 1,- 1,290,- 1,- 1,- 1,331,331,331,331,331,331,331,331,- 1,- 1,- 1,- 1,- 1,- 1,- 1,290,- 1,141,141,141,141,141,141,290,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,334,334,334,334,334,334,334,334,334,334,- 1,- 1,- 1,- 1,- 1,- 1,331,334,334,334,334,334,334,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,331,- 1,- 1,- 1,- 1,- 1,290,- 1,- 1,- 1,334,334,334,334,334,334,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,- 1,- 1,- 1,- 1,335,335,335,335,335,335,- 1,- 1,351,- 1,351,351,351,351,351,351,351,351,351,351,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,351,335,335,335,335,335,335,351,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,351,- 1,- 1,351,352,- 1,352,352,352,352,352,352,352,352,352,352,351,- 1,- 1,- 1,- 1,- 1,- 1,351,- 1,- 1,- 1,352,- 1,- 1,- 1,- 1,351,- 1,352,351,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,- 1,- 1,- 1,- 1,- 1,- 1,352,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,352,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,- 1,- 1,- 1,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,- 1,- 1,- 1,- 1,357,- 1,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,357,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,- 1,- 1,- 1,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,- 1,- 1,- 1,- 1,363,- 1,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,363,373,373,373,373,373,373,373,373,373,373,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,373,373,- 1,- 1,- 1,- 1,374,373,374,374,374,374,374,374,374,374,374,374,379,379,379,379,379,379,379,379,379,379,- 1,374,- 1,- 1,373,373,- 1,- 1,374,- 1,- 1,373,379,- 1,- 1,- 1,- 1,374,379,- 1,380,380,380,380,380,380,380,380,380,380,- 1,- 1,- 1,374,- 1,- 1,- 1,- 1,- 1,- 1,374,380,380,- 1,379,- 1,- 1,- 1,380,374,379,- 1,- 1,383,383,383,383,383,383,383,383,383,383,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,380,380,383,- 1,- 1,- 1,- 1,380,383,384,- 1,384,384,384,384,384,384,384,384,384,384,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,384,- 1,383,- 1,- 1,- 1,- 1,384,383,- 1,- 1,- 1,- 1,- 1,- 1,385,384,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,- 1,- 1,384,- 1,- 1,- 1,- 1,- 1,385,384,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,384,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,399,399,399,399,399,399,399,399,399,399,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,385,399,399,- 1,- 1,- 1,- 1,- 1,399,385,388,388,388,388,388,388,388,388,388,388,- 1,- 1,- 1,- 1,- 1,- 1,- 1,388,388,388,388,388,388,399,399,- 1,- 1,- 1,- 1,- 1,399,- 1,- 1,- 1,- 1,- 1,- 1,- 1,389,389,389,389,389,389,389,389,389,389,- 1,388,388,388,388,388,388,389,389,389,389,389,389,- 1,- 1,- 1,408,- 1,389,- 1,- 1,408,- 1,- 1,- 1,- 1,- 1,389,- 1,- 1,408,408,408,408,408,408,408,408,- 1,389,389,389,389,389,389,408,- 1,- 1,- 1,- 1,389,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,389,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,- 1,- 1,- 1,- 1,408,408,- 1,- 1,- 1,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,- 1,- 1,- 1,408,- 1,408,- 1,408,- 1,408,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,422,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,422,422,422,422,422,422,422,422,422,422,- 1,- 1,- 1,- 1,- 1,- 1,- 1,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,- 1,- 1,- 1,- 1,- 1,- 1,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,422,432,- 1,- 1,432,- 1,- 1,- 1,432,432,432,432,432,432,432,432,432,432,- 1,- 1,- 1,- 1,- 1,- 1,- 1,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,- 1,- 1,- 1,- 1,432,- 1,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,432,434,- 1,- 1,- 1,- 1,- 1,- 1,- 1,434,434,434,434,434,434,434,434,434,434,- 1,- 1,- 1,- 1,- 1,- 1,- 1,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,- 1,- 1,- 1,- 1,434,- 1,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,434,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 223
int state;int base;int backtrk;
int c;
state=start_state;
# 227
if(state >= 0){
({int _tmp406=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp406;});
lbuf->lex_last_action=-1;}else{
# 231
state=(- state)- 1;}
# 233
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,444U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=*((const int*)_check_known_subscript_notnull(Cyc_lex_backtrk,444U,sizeof(int),state));
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
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,3846U,sizeof(int),base + c));else{
# 253
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_default,444U,sizeof(int),state));}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == -1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp407=({const char*_tmp13="empty token";_tag_fat(_tmp13,sizeof(char),12U);});_tmp14->f1=_tmp407;});_tmp14;}));else{
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
Cyc_current_targets=({struct Cyc_Set_Set**_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Set_Set*_tmp40A=({struct Cyc_Set_Set*(*_tmp409)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_delete;});struct Cyc_Set_Set*_tmp408=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp409(_tmp408,(struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});*_tmp16=_tmp40A;});_tmp16;});}
# 230
return({struct _tuple13*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->f1=(struct _fat_ptr*)_check_null(Cyc_current_source),_tmp17->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp17;});case 1:
# 233 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2:
# 235
 return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);};}
# 239
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp40B=({const char*_tmp18="some action didn't return!";_tag_fat(_tmp18,sizeof(char),27U);});_tmp19->f1=_tmp40B;});_tmp19;}));}
# 241
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp1A=lexstate;switch((int)_tmp1A){case 0:
# 239 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_tmp1B=_cycalloc(sizeof(*_tmp1B));({struct _fat_ptr _tmp40E=(struct _fat_ptr)({struct _fat_ptr _tmp40D=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp40D,0,(unsigned long)(({
int _tmp40C=Cyc_Lexing_lexeme_end(lexbuf);_tmp40C - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 239
*_tmp1B=_tmp40E;});_tmp1B;});
# 241
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1C=_cycalloc(sizeof(*_tmp1C));({struct Cyc_Set_Set*_tmp40F=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp1C=_tmp40F;});_tmp1C;});
Cyc_token(lexbuf);
return 0;case 1:
# 247
 Cyc_current_source=({struct _fat_ptr*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct _fat_ptr _tmp412=(struct _fat_ptr)({struct _fat_ptr _tmp411=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp411,0,(unsigned long)(({
int _tmp410=Cyc_Lexing_lexeme_end(lexbuf);_tmp410 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 247
*_tmp1D=_tmp412;});_tmp1D;});
# 249
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct Cyc_Set_Set*_tmp413=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp1E=_tmp413;});_tmp1E;});
Cyc_args(lexbuf);
return 0;case 2:
# 255
 Cyc_current_source=({struct _fat_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));({struct _fat_ptr _tmp414=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp1F=_tmp414;});_tmp1F;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp20=_cycalloc(sizeof(*_tmp20));({struct Cyc_Set_Set*_tmp415=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp20=_tmp415;});_tmp20;});
Cyc_token(lexbuf);
return 0;default:
# 261
((lexbuf->refill_buff))(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);};}
# 264
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp416=({const char*_tmp21="some action didn't return!";_tag_fat(_tmp21,sizeof(char),27U);});_tmp22->f1=_tmp416;});_tmp22;}));}
# 266
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp23=lexstate;switch((int)_tmp23){case 0:  {
# 264 "buildlib.cyl"
struct _fat_ptr*_tmp24=({struct _fat_ptr*_tmp26=_cycalloc(sizeof(*_tmp26));({struct _fat_ptr _tmp419=(struct _fat_ptr)({struct _fat_ptr _tmp418=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp418,0,(unsigned long)(({
int _tmp417=Cyc_Lexing_lexeme_end(lexbuf);_tmp417 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 264
*_tmp26=_tmp419;});_tmp26;});struct _fat_ptr*a=_tmp24;
# 266
Cyc_current_args=({struct Cyc_List_List*_tmp25=_cycalloc(sizeof(*_tmp25));_tmp25->hd=a,_tmp25->tl=Cyc_current_args;_tmp25;});
return Cyc_args(lexbuf);}case 1:  {
# 270
struct _fat_ptr*_tmp27=({struct _fat_ptr*_tmp29=_cycalloc(sizeof(*_tmp29));({struct _fat_ptr _tmp41C=(struct _fat_ptr)({struct _fat_ptr _tmp41B=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp41B,0,(unsigned long)(({
int _tmp41A=Cyc_Lexing_lexeme_end(lexbuf);_tmp41A - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 270
*_tmp29=_tmp41C;});_tmp29;});struct _fat_ptr*a=_tmp27;
# 272
Cyc_current_args=({struct Cyc_List_List*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->hd=a,_tmp28->tl=Cyc_current_args;_tmp28;});
return Cyc_args(lexbuf);}case 2:  {
# 276
struct _fat_ptr*_tmp2A=({struct _fat_ptr*_tmp2C=_cycalloc(sizeof(*_tmp2C));({struct _fat_ptr _tmp41F=(struct _fat_ptr)({struct _fat_ptr _tmp41E=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp41E,0,(unsigned long)(({
int _tmp41D=Cyc_Lexing_lexeme_end(lexbuf);_tmp41D - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 276
*_tmp2C=_tmp41F;});_tmp2C;});struct _fat_ptr*a=_tmp2A;
# 278
Cyc_current_args=({struct Cyc_List_List*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->hd=a,_tmp2B->tl=Cyc_current_args;_tmp2B;});
return Cyc_token(lexbuf);}default:
# 281
((lexbuf->refill_buff))(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);};}
# 284
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp420=({const char*_tmp2D="some action didn't return!";_tag_fat(_tmp2D,sizeof(char),27U);});_tmp2E->f1=_tmp420;});_tmp2E;}));}
# 286
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp2F=lexstate;switch((int)_tmp2F){case 0:
# 285 "buildlib.cyl"
 Cyc_add_target(({struct _fat_ptr*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr _tmp421=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp30=_tmp421;});_tmp30;}));return Cyc_token(lexbuf);case 1:
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
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp422=({const char*_tmp31="some action didn't return!";_tag_fat(_tmp31,sizeof(char),27U);});_tmp32->f1=_tmp422;});_tmp32;}));}
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
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp423=({const char*_tmp34="some action didn't return!";_tag_fat(_tmp34,sizeof(char),27U);});_tmp35->f1=_tmp423;});_tmp35;}));}
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
({void*_tmp37=0U;({struct _fat_ptr _tmp424=({const char*_tmp38="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp38,sizeof(char),44U);});Cyc_log(_tmp424,_tag_fat(_tmp37,sizeof(void*),0));});});
return 1;case 3:
# 433 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp39=0U;({struct _fat_ptr _tmp425=({const char*_tmp3A="Warning: declaration of malloc sidestepped\n";_tag_fat(_tmp3A,sizeof(char),44U);});Cyc_log(_tmp425,_tag_fat(_tmp39,sizeof(void*),0));});});
return 1;case 4:
# 439 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3B=0U;({struct _fat_ptr _tmp426=({const char*_tmp3C="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp3C,sizeof(char),44U);});Cyc_log(_tmp426,_tag_fat(_tmp3B,sizeof(void*),0));});});
return 1;case 5:
# 445 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3D=0U;({struct _fat_ptr _tmp427=({const char*_tmp3E="Warning: declaration of calloc sidestepped\n";_tag_fat(_tmp3E,sizeof(char),44U);});Cyc_log(_tmp427,_tag_fat(_tmp3D,sizeof(void*),0));});});
return 1;case 6:
# 449
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3F=0U;({struct _fat_ptr _tmp428=({const char*_tmp40="Warning: use of region sidestepped\n";_tag_fat(_tmp40,sizeof(char),36U);});Cyc_log(_tmp428,_tag_fat(_tmp3F,sizeof(void*),0));});});
return 1;case 7:
# 453
({void*_tmp41=0U;({struct _fat_ptr _tmp429=({const char*_tmp42="Warning: use of __extension__ deleted\n";_tag_fat(_tmp42,sizeof(char),39U);});Cyc_log(_tmp429,_tag_fat(_tmp41,sizeof(void*),0));});});
return 1;case 8:
# 457 "buildlib.cyl"
({void*_tmp43=0U;({struct _fat_ptr _tmp42A=({const char*_tmp44="Warning: use of nonnull attribute deleted\n";_tag_fat(_tmp44,sizeof(char),43U);});Cyc_log(_tmp42A,_tag_fat(_tmp43,sizeof(void*),0));});});
return 1;case 9:
# 462 "buildlib.cyl"
({void*_tmp45=0U;({struct _fat_ptr _tmp42B=({const char*_tmp46="Warning: use of mode HI deleted\n";_tag_fat(_tmp46,sizeof(char),33U);});Cyc_log(_tmp42B,_tag_fat(_tmp45,sizeof(void*),0));});});
return 1;case 10:
# 465
({void*_tmp47=0U;({struct _fat_ptr _tmp42C=({const char*_tmp48="Warning: use of mode SI deleted\n";_tag_fat(_tmp48,sizeof(char),33U);});Cyc_log(_tmp42C,_tag_fat(_tmp47,sizeof(void*),0));});});
return 1;case 11:
# 468
({void*_tmp49=0U;({struct _fat_ptr _tmp42D=({const char*_tmp4A="Warning: use of mode QI deleted\n";_tag_fat(_tmp4A,sizeof(char),33U);});Cyc_log(_tmp42D,_tag_fat(_tmp49,sizeof(void*),0));});});
return 1;case 12:
# 471
({void*_tmp4B=0U;({struct _fat_ptr _tmp42E=({const char*_tmp4C="Warning: use of mode DI deleted\n";_tag_fat(_tmp4C,sizeof(char),33U);});Cyc_log(_tmp42E,_tag_fat(_tmp4B,sizeof(void*),0));});});
return 1;case 13:
# 474
({void*_tmp4D=0U;({struct _fat_ptr _tmp42F=({const char*_tmp4E="Warning: use of mode DI deleted\n";_tag_fat(_tmp4E,sizeof(char),33U);});Cyc_log(_tmp42F,_tag_fat(_tmp4D,sizeof(void*),0));});});
return 1;case 14:
# 477
({void*_tmp4F=0U;({struct _fat_ptr _tmp430=({const char*_tmp50="Warning: use of mode word deleted\n";_tag_fat(_tmp50,sizeof(char),35U);});Cyc_log(_tmp430,_tag_fat(_tmp4F,sizeof(void*),0));});});
return 1;case 15:
# 480
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16:
# 482
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17:
# 484
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18:
# 486
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19:
# 491 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 20:
# 493
 return 1;case 21:
# 495
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp51=0U;({struct _fat_ptr _tmp431=({const char*_tmp52="Warning: replacing use of __asm__ with 0\n";_tag_fat(_tmp52,sizeof(char),42U);});Cyc_log(_tmp431,_tag_fat(_tmp51,sizeof(void*),0));});});
return 1;case 22:
# 501
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp53=0U;({struct _fat_ptr _tmp432=({const char*_tmp54="Warning: replacing use of __asm with 0\n";_tag_fat(_tmp54,sizeof(char),40U);});Cyc_log(_tmp432,_tag_fat(_tmp53,sizeof(void*),0));});});
return 1;case 23:
# 507
({int _tmp433=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp433,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);};}
# 511
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp434=({const char*_tmp55="some action didn't return!";_tag_fat(_tmp55,sizeof(char),27U);});_tmp56->f1=_tmp434;});_tmp56;}));}
# 513
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp57=lexstate;switch((int)_tmp57){case 0:
# 511 "buildlib.cyl"
 return 0;case 1:
# 513
 Cyc_fputc(34,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2:
# 515
({void*_tmp58=0U;({struct _fat_ptr _tmp435=({const char*_tmp59="Warning: unclosed string\n";_tag_fat(_tmp59,sizeof(char),26U);});Cyc_log(_tmp435,_tag_fat(_tmp58,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0,({struct _fat_ptr _tmp436=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39A.f1=_tmp436;});_tmp39A;});void*_tmp5A[1];_tmp5A[0]=& _tmp5C;({struct Cyc___cycFILE*_tmp438=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp437=({const char*_tmp5B="%s";_tag_fat(_tmp5B,sizeof(char),3U);});Cyc_fprintf(_tmp438,_tmp437,_tag_fat(_tmp5A,sizeof(void*),1));});});return 1;case 3:
# 518
({struct Cyc_String_pa_PrintArg_struct _tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp39B;_tmp39B.tag=0,({struct _fat_ptr _tmp439=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39B.f1=_tmp439;});_tmp39B;});void*_tmp5D[1];_tmp5D[0]=& _tmp5F;({struct Cyc___cycFILE*_tmp43B=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp43A=({const char*_tmp5E="%s";_tag_fat(_tmp5E,sizeof(char),3U);});Cyc_fprintf(_tmp43B,_tmp43A,_tag_fat(_tmp5D,sizeof(void*),1));});});return 1;case 4:
# 520
({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0,({struct _fat_ptr _tmp43C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39C.f1=_tmp43C;});_tmp39C;});void*_tmp60[1];_tmp60[0]=& _tmp62;({struct Cyc___cycFILE*_tmp43E=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp43D=({const char*_tmp61="%s";_tag_fat(_tmp61,sizeof(char),3U);});Cyc_fprintf(_tmp43E,_tmp43D,_tag_fat(_tmp60,sizeof(void*),1));});});return 1;case 5:
# 522
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp39D;_tmp39D.tag=0,({struct _fat_ptr _tmp43F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39D.f1=_tmp43F;});_tmp39D;});void*_tmp63[1];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmp441=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp440=({const char*_tmp64="%s";_tag_fat(_tmp64,sizeof(char),3U);});Cyc_fprintf(_tmp441,_tmp440,_tag_fat(_tmp63,sizeof(void*),1));});});return 1;case 6:
# 524
({struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp39E;_tmp39E.tag=0,({struct _fat_ptr _tmp442=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39E.f1=_tmp442;});_tmp39E;});void*_tmp66[1];_tmp66[0]=& _tmp68;({struct Cyc___cycFILE*_tmp444=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp443=({const char*_tmp67="%s";_tag_fat(_tmp67,sizeof(char),3U);});Cyc_fprintf(_tmp444,_tmp443,_tag_fat(_tmp66,sizeof(void*),1));});});return 1;case 7:
# 526
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp39F;_tmp39F.tag=0,({struct _fat_ptr _tmp445=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp39F.f1=_tmp445;});_tmp39F;});void*_tmp69[1];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp447=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp446=({const char*_tmp6A="%s";_tag_fat(_tmp6A,sizeof(char),3U);});Cyc_fprintf(_tmp447,_tmp446,_tag_fat(_tmp69,sizeof(void*),1));});});return 1;case 8:
# 528
({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp3A0;_tmp3A0.tag=0,({struct _fat_ptr _tmp448=(struct _fat_ptr)((struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3A0.f1=_tmp448;});_tmp3A0;});void*_tmp6C[1];_tmp6C[0]=& _tmp6E;({struct Cyc___cycFILE*_tmp44A=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp449=({const char*_tmp6D="%s";_tag_fat(_tmp6D,sizeof(char),3U);});Cyc_fprintf(_tmp44A,_tmp449,_tag_fat(_tmp6C,sizeof(void*),1));});});return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);};}
# 532
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp44B=({const char*_tmp6F="some action didn't return!";_tag_fat(_tmp6F,sizeof(char),27U);});_tmp70->f1=_tmp44B;});_tmp70;}));}
# 534
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp71=lexstate;switch((int)_tmp71){case 0:
# 538 "buildlib.cyl"
 return 0;case 1:
# 540
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2:
# 544
 ++ Cyc_parens_to_match;
return 1;case 3:
# 547
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4:
# 550
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5:
# 553
 return 1;case 6:
# 555
 return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);};}
# 559
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp44C=({const char*_tmp72="some action didn't return!";_tag_fat(_tmp72,sizeof(char),27U);});_tmp73->f1=_tmp44C;});_tmp73;}));}
# 561
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp74=lexstate;switch((int)_tmp74){case 0:
# 559 "buildlib.cyl"
({void*_tmp75=0U;({struct _fat_ptr _tmp44D=({const char*_tmp76="Warning: unclosed string\n";_tag_fat(_tmp76,sizeof(char),26U);});Cyc_log(_tmp44D,_tag_fat(_tmp75,sizeof(void*),0));});});return 0;case 1:
# 561
 return 0;case 2:
# 563
({void*_tmp77=0U;({struct _fat_ptr _tmp44E=({const char*_tmp78="Warning: unclosed string\n";_tag_fat(_tmp78,sizeof(char),26U);});Cyc_log(_tmp44E,_tag_fat(_tmp77,sizeof(void*),0));});});return 1;case 3:
# 565
 return 1;case 4:
# 567
 return 1;case 5:
# 569
 return 1;case 6:
# 571
 return 1;case 7:
# 573
 return 1;case 8:
# 575
 return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);};}
# 579
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp44F=({const char*_tmp79="some action didn't return!";_tag_fat(_tmp79,sizeof(char),27U);});_tmp7A->f1=_tmp44F;});_tmp7A;}));}
# 581
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7B=lexstate;switch((int)_tmp7B){case 0:
# 579 "buildlib.cyl"
({void*_tmp7C=0U;({struct _fat_ptr _tmp450=({const char*_tmp7D="Warning: unclosed comment\n";_tag_fat(_tmp7D,sizeof(char),27U);});Cyc_log(_tmp450,_tag_fat(_tmp7C,sizeof(void*),0));});});return 0;case 1:
# 581
 return 0;case 2:
# 583
 return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);};}
# 587
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp451=({const char*_tmp7E="some action didn't return!";_tag_fat(_tmp7E,sizeof(char),27U);});_tmp7F->f1=_tmp451;});_tmp7F;}));}
# 589
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple14*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp80=lexstate;switch((int)_tmp80){case 0:
# 591 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp81="#define ";_tag_fat(_tmp81,sizeof(char),9U);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple14*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=Cyc_current_line,_tmp82->f2=(struct _fat_ptr*)_check_null(Cyc_current_source);_tmp82;});case 1:
# 595
 return Cyc_suck_line(lexbuf);case 2:
# 597
 return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);};}
# 601
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp452=({const char*_tmp83="some action didn't return!";_tag_fat(_tmp83,sizeof(char),27U);});_tmp84->f1=_tmp452;});_tmp84;}));}
# 603
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp85=lexstate;if(_tmp85 == 0){
# 601 "buildlib.cyl"
Cyc_current_source=({struct _fat_ptr*_tmp86=_cycalloc(sizeof(*_tmp86));({struct _fat_ptr _tmp453=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp86=_tmp453;});_tmp86;});
Cyc_current_line=(struct _fat_ptr)Cyc_strconcat((struct _fat_ptr)Cyc_current_line,(struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{
# 605
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);};}
# 608
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp454=({const char*_tmp87="some action didn't return!";_tag_fat(_tmp87,sizeof(char),27U);});_tmp88->f1=_tmp454;});_tmp88;}));}
# 610
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp89=lexstate;if(_tmp89 == 0){
# 608 "buildlib.cyl"
Cyc_current_line=(struct _fat_ptr)({struct _fat_ptr _tmp455=(struct _fat_ptr)Cyc_current_line;Cyc_strconcat(_tmp455,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});return 0;}else{
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);};}
# 612
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp456=({const char*_tmp8A="some action didn't return!";_tag_fat(_tmp8A,sizeof(char),27U);});_tmp8B->f1=_tmp456;});_tmp8B;}));}
# 614
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple17*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp8C=lexstate;switch((int)_tmp8C){case 0:
# 615 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1:
# 617
 Cyc_current_headerfile=(struct _fat_ptr)({
struct _fat_ptr _tmp458=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp458,0,(unsigned long)(({
int _tmp457=Cyc_Lexing_lexeme_end(lexbuf);_tmp457 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
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
return({struct _tuple17*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->f1=Cyc_current_headerfile,_tmp8D->f2=Cyc_current_symbols,_tmp8D->f3=Cyc_current_user_defs,_tmp8D->f4=Cyc_current_omit_symbols,_tmp8D->f5=Cyc_current_hstubs,_tmp8D->f6=Cyc_current_cstubs,_tmp8D->f7=Cyc_current_cycstubs,_tmp8D->f8=Cyc_current_cpp;_tmp8D;});case 2:
# 642
 return Cyc_spec(lexbuf);case 3:
# 644
 return 0;case 4:
# 646
({struct Cyc_Int_pa_PrintArg_struct _tmp90=({struct Cyc_Int_pa_PrintArg_struct _tmp3A1;_tmp3A1.tag=1,({
# 648
unsigned long _tmp459=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3A1.f1=_tmp459;});_tmp3A1;});void*_tmp8E[1];_tmp8E[0]=& _tmp90;({struct Cyc___cycFILE*_tmp45B=Cyc_stderr;struct _fat_ptr _tmp45A=({const char*_tmp8F="Error in .cys file: expected header file name, found '%c' instead\n";_tag_fat(_tmp8F,sizeof(char),67U);});Cyc_fprintf(_tmp45B,_tmp45A,_tag_fat(_tmp8E,sizeof(void*),1));});});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);};}
# 653
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp45C=({const char*_tmp91="some action didn't return!";_tag_fat(_tmp91,sizeof(char),27U);});_tmp92->f1=_tmp45C;});_tmp92;}));}
# 655
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp93=lexstate;switch((int)_tmp93){case 0:
# 653 "buildlib.cyl"
 return 0;case 1:
# 655
 return 0;case 2:
# 657
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3:
# 662
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp94=Cyc_current_user_defs;struct Cyc_List_List*tmp_user_defs=_tmp94;
while(Cyc_snarfsymbols(lexbuf)){;}
if(tmp_user_defs != Cyc_current_user_defs){
({void*_tmp95=0U;({struct Cyc___cycFILE*_tmp45E=Cyc_stderr;struct _fat_ptr _tmp45D=({const char*_tmp96="Error in .cys file: got optional definition in omitsymbols\n";_tag_fat(_tmp96,sizeof(char),60U);});Cyc_fprintf(_tmp45E,_tmp45D,_tag_fat(_tmp95,sizeof(void*),0));});});
# 668
return 0;}
# 670
Cyc_current_omit_symbols=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;}case 4:
# 673
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->f1=(struct _fat_ptr)_tag_fat(0,0,0),({
struct _fat_ptr _tmp45F=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp98->f2=_tmp45F;});_tmp98;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=x,_tmp97->tl=Cyc_current_hstubs;_tmp97;});
return 1;}case 5:  {
# 681
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
struct _fat_ptr _tmp460=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9D->f2=_tmp460;});_tmp9D;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=x,_tmp9C->tl=Cyc_current_hstubs;_tmp9C;});
return 1;}}}}case 6:
# 695
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->f1=(struct _fat_ptr)_tag_fat(0,0,0),({
struct _fat_ptr _tmp461=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9F->f2=_tmp461;});_tmp9F;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=x,_tmp9E->tl=Cyc_current_cstubs;_tmp9E;});
return 1;}case 7:  {
# 703
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
struct _fat_ptr _tmp462=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA4->f2=_tmp462;});_tmpA4;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=x,_tmpA3->tl=Cyc_current_cstubs;_tmpA3;});
return 1;}}}}case 8:
# 717
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x=({struct _tuple15*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->f1=(struct _fat_ptr)_tag_fat(0,0,0),({
struct _fat_ptr _tmp463=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA6->f2=_tmp463;});_tmpA6;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=x,_tmpA5->tl=Cyc_current_cycstubs;_tmpA5;});
return 1;}case 9:  {
# 725
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
struct _fat_ptr _tmp464=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpAB->f2=_tmp464;});_tmpAB;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->hd=x,_tmpAA->tl=Cyc_current_cycstubs;_tmpAA;});
return 1;}}}}case 10:
# 739
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _fat_ptr*x=({struct _fat_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _fat_ptr _tmp465=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpAD=_tmp465;});_tmpAD;});
Cyc_current_cpp=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=x,_tmpAC->tl=Cyc_current_cpp;_tmpAC;});
return 1;}case 11:
# 746
 return 1;case 12:
# 748
 return 1;case 13:
# 750
({struct Cyc_Int_pa_PrintArg_struct _tmpB0=({struct Cyc_Int_pa_PrintArg_struct _tmp3A2;_tmp3A2.tag=1,({
# 752
unsigned long _tmp466=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3A2.f1=_tmp466;});_tmp3A2;});void*_tmpAE[1];_tmpAE[0]=& _tmpB0;({struct Cyc___cycFILE*_tmp468=Cyc_stderr;struct _fat_ptr _tmp467=({const char*_tmpAF="Error in .cys file: expected command, found '%c' instead\n";_tag_fat(_tmpAF,sizeof(char),58U);});Cyc_fprintf(_tmp468,_tmp467,_tag_fat(_tmpAE,sizeof(void*),1));});});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);};}
# 757
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp469=({const char*_tmpB1="some action didn't return!";_tag_fat(_tmpB1,sizeof(char),27U);});_tmpB2->f1=_tmp469;});_tmpB2;}));}
# 759
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB3=lexstate;switch((int)_tmpB3){case 0:
# 763 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _fat_ptr*_tmp46B=({struct _fat_ptr*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct _fat_ptr _tmp46A=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpB4=_tmp46A;});_tmpB4;});_tmpB5->hd=_tmp46B;}),_tmpB5->tl=Cyc_snarfed_symbols;_tmpB5;});
return 1;case 1:  {
# 766
struct _fat_ptr _tmpB6=Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr s=_tmpB6;
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}
# 775
Cyc_rename_current_symbol=0;{
struct _tuple16*user_def=({struct _tuple16*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _fat_ptr*_tmp46E=({struct _fat_ptr*_tmpBA=_cycalloc(sizeof(*_tmpBA));*_tmpBA=Cyc_current_symbol;_tmpBA;});_tmpBC->f1=_tmp46E;}),({
struct _fat_ptr*_tmp46D=({struct _fat_ptr*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _fat_ptr _tmp46C=(struct _fat_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpBB=_tmp46C;});_tmpBB;});_tmpBC->f2=_tmp46D;});_tmpBC;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct _fat_ptr*_tmp46F=({struct _fat_ptr*_tmpB7=_cycalloc(sizeof(*_tmpB7));*_tmpB7=(struct _fat_ptr)Cyc_current_symbol;_tmpB7;});_tmpB8->hd=_tmp46F;}),_tmpB8->tl=Cyc_snarfed_symbols;_tmpB8;});
Cyc_current_user_defs=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=user_def,_tmpB9->tl=Cyc_current_user_defs;_tmpB9;});
return 1;}}case 2:
# 782
 return 1;case 3:
# 784
 return 0;case 4:
# 786
({void*_tmpBD=0U;({struct Cyc___cycFILE*_tmp471=Cyc_stderr;struct _fat_ptr _tmp470=({const char*_tmpBE="Error in .cys file: unexpected end-of-file\n";_tag_fat(_tmpBE,sizeof(char),44U);});Cyc_fprintf(_tmp471,_tmp470,_tag_fat(_tmpBD,sizeof(void*),0));});});
# 788
return 0;case 5:
# 790
({struct Cyc_Int_pa_PrintArg_struct _tmpC1=({struct Cyc_Int_pa_PrintArg_struct _tmp3A3;_tmp3A3.tag=1,({
# 792
unsigned long _tmp472=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3A3.f1=_tmp472;});_tmp3A3;});void*_tmpBF[1];_tmpBF[0]=& _tmpC1;({struct Cyc___cycFILE*_tmp474=Cyc_stderr;struct _fat_ptr _tmp473=({const char*_tmpC0="Error in .cys file: expected symbol, found '%c' instead\n";_tag_fat(_tmpC0,sizeof(char),57U);});Cyc_fprintf(_tmp474,_tmp473,_tag_fat(_tmpBF,sizeof(void*),1));});});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);};}
# 797
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp475=({const char*_tmpC2="some action didn't return!";_tag_fat(_tmpC2,sizeof(char),27U);});_tmpC3->f1=_tmp475;});_tmpC3;}));}
# 799
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC4=lexstate;switch((int)_tmpC4){case 0:
# 803 "buildlib.cyl"
({void*_tmpC5=0U;({struct _fat_ptr _tmp476=({const char*_tmpC6="Warning: unclosed brace\n";_tag_fat(_tmpC6,sizeof(char),25U);});Cyc_log(_tmp476,_tag_fat(_tmpC5,sizeof(void*),0));});});return 0;case 1:
# 805
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2:
# 810
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3:
# 814
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4:
# 818
({struct Cyc_Buffer_t*_tmp477=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp477,({const char*_tmpC7="/*";_tag_fat(_tmpC7,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5:
# 822
({struct Cyc_Buffer_t*_tmp478=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp478,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:  {
# 825
struct _fat_ptr _tmpC8=(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf);struct _fat_ptr symbol=_tmpC8;
if(Cyc_rename_current_symbol && !Cyc_strcmp((struct _fat_ptr)symbol,(struct _fat_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp479=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp479,*Cyc_add_user_prefix(({struct _fat_ptr*_tmpC9=_cycalloc(sizeof(*_tmpC9));*_tmpC9=symbol;_tmpC9;})));});else{
# 829
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),symbol);}
return 1;}case 7:
# 832
({struct Cyc_Buffer_t*_tmp47A=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp47A,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);};}
# 837
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp47B=({const char*_tmpCA="some action didn't return!";_tag_fat(_tmpCA,sizeof(char),27U);});_tmpCB->f1=_tmp47B;});_tmpCB;}));}
# 839
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpCC=lexstate;switch((int)_tmpCC){case 0:
# 837 "buildlib.cyl"
({void*_tmpCD=0U;({struct _fat_ptr _tmp47C=({const char*_tmpCE="Warning: unclosed string\n";_tag_fat(_tmpCE,sizeof(char),26U);});Cyc_log(_tmp47C,_tag_fat(_tmpCD,sizeof(void*),0));});});return 0;case 1:
# 839
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2:
# 841
({void*_tmpCF=0U;({struct _fat_ptr _tmp47D=({const char*_tmpD0="Warning: unclosed string\n";_tag_fat(_tmpD0,sizeof(char),26U);});Cyc_log(_tmp47D,_tag_fat(_tmpCF,sizeof(void*),0));});});
({struct Cyc_Buffer_t*_tmp47E=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp47E,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3:
# 845
({struct Cyc_Buffer_t*_tmp47F=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp47F,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4:
# 848
({struct Cyc_Buffer_t*_tmp480=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp480,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5:
# 851
({struct Cyc_Buffer_t*_tmp481=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp481,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:
# 854
({struct Cyc_Buffer_t*_tmp482=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp482,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7:
# 857
({struct Cyc_Buffer_t*_tmp483=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp483,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8:
# 860
({struct Cyc_Buffer_t*_tmp484=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp484,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);};}
# 865
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp485=({const char*_tmpD1="some action didn't return!";_tag_fat(_tmpD1,sizeof(char),27U);});_tmpD2->f1=_tmp485;});_tmpD2;}));}
# 867
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpD3=lexstate;switch((int)_tmpD3){case 0:
# 865 "buildlib.cyl"
({void*_tmpD4=0U;({struct _fat_ptr _tmp486=({const char*_tmpD5="Warning: unclosed comment\n";_tag_fat(_tmpD5,sizeof(char),27U);});Cyc_log(_tmp486,_tag_fat(_tmpD4,sizeof(void*),0));});});return 0;case 1:
# 867
({struct Cyc_Buffer_t*_tmp487=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp487,({const char*_tmpD6="*/";_tag_fat(_tmpD6,sizeof(char),3U);}));});return 0;case 2:
# 869
({struct Cyc_Buffer_t*_tmp488=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp488,(struct _fat_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);};}
# 874
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp489=({const char*_tmpD7="some action didn't return!";_tag_fat(_tmpD7,sizeof(char),27U);});_tmpD8->f1=_tmp489;});_tmpD8;}));}
# 876
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 882 "buildlib.cyl"
void Cyc_scan_type(void*,struct Cyc_Hashtable_Table*);struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpD9=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_stmttmp1=_tmpD9;void*_tmpDA=_stmttmp1;int _tmpDD;void*_tmpDE;void*_tmpDC;void*_tmpDB;switch(*((int*)_tmpDA)){case 1: _tmpDB=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;{void*b=_tmpDB;
# 886
struct _fat_ptr*_tmpDF=(*Cyc_Absyn_binding2qvar(b)).f2;struct _fat_ptr*v=_tmpDF;
Cyc_add_target(v);
return;}case 3: _tmpDB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{struct Cyc_List_List*x=_tmpDB;
# 890
for(1;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 893
return;}case 23: _tmpDB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_Absyn_Exp*e2=_tmpDC;
# 895
_tmpDB=e1;_tmpDC=e2;goto _LL8;}case 9: _tmpDB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_Absyn_Exp*e2=_tmpDC;
# 897
_tmpDB=e1;_tmpDC=e2;goto _LLA;}case 4: _tmpDB=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_Absyn_Exp*e2=_tmpDC;
# 899
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 41: _tmpDB=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;{struct Cyc_Absyn_Exp*e1=_tmpDB;
_tmpDB=e1;goto _LLE;}case 20: _tmpDB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LLE: {struct Cyc_Absyn_Exp*e1=_tmpDB;
# 904
_tmpDB=e1;goto _LL10;}case 18: _tmpDB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_tmpDB;
# 906
_tmpDB=e1;goto _LL12;}case 15: _tmpDB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmpDB;
# 908
_tmpDB=e1;goto _LL14;}case 5: _tmpDB=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpDB;
# 910
Cyc_scan_exp(e1,dep);
return;}case 6: _tmpDB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_tmpDE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;{struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_Absyn_Exp*e2=_tmpDC;struct Cyc_Absyn_Exp*e3=_tmpDE;
# 913
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
Cyc_scan_exp(e3,dep);
return;}case 7: _tmpDB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_Absyn_Exp*e2=_tmpDC;
_tmpDB=e1;_tmpDC=e2;goto _LL1A;}case 8: _tmpDB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_Absyn_Exp*e2=_tmpDC;
# 919
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 10: _tmpDB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_List_List*x=_tmpDC;
# 923
Cyc_scan_exp(e1,dep);
for(1;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 927
return;}case 14: _tmpDB=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{void*t1=_tmpDB;struct Cyc_Absyn_Exp*e1=_tmpDC;
# 929
Cyc_scan_type(t1,dep);
Cyc_scan_exp(e1,dep);
return;}case 34: _tmpDD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).is_calloc;_tmpDB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).rgn;_tmpDC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).elt_type;_tmpDE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).num_elts;{int iscalloc=_tmpDD;struct Cyc_Absyn_Exp*ropt=_tmpDB;void**topt=_tmpDC;struct Cyc_Absyn_Exp*e=_tmpDE;
# 933
if(ropt != 0)Cyc_scan_exp(ropt,dep);
if(topt != 0)Cyc_scan_type(*topt,dep);
Cyc_scan_exp(e,dep);
return;}case 38: _tmpDB=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;{struct Cyc_Absyn_Exp*e=_tmpDB;
# 938
Cyc_scan_exp(e,dep);return;}case 39: _tmpDB=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;{void*t1=_tmpDB;
_tmpDB=t1;goto _LL26;}case 17: _tmpDB=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL26: {void*t1=_tmpDB;
# 941
Cyc_scan_type(t1,dep);
return;}case 21: _tmpDB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{struct Cyc_Absyn_Exp*e1=_tmpDB;struct _fat_ptr*fn=_tmpDC;
# 944
_tmpDB=e1;_tmpDC=fn;goto _LL2A;}case 22: _tmpDB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpDB;struct _fat_ptr*fn=_tmpDC;
# 946
Cyc_scan_exp(e1,dep);
Cyc_add_target(fn);
return;}case 19: _tmpDB=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmpDC=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{void*t1=_tmpDB;struct Cyc_List_List*f=_tmpDC;
# 950
Cyc_scan_type(t1,dep);
# 952
{void*_tmpE0=(void*)((struct Cyc_List_List*)_check_null(f))->hd;void*_stmttmp2=_tmpE0;void*_tmpE1=_stmttmp2;void*_tmpE2;if(*((int*)_tmpE1)== 0){_tmpE2=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpE1)->f1;{struct _fat_ptr*fn=_tmpE2;
Cyc_add_target(fn);goto _LL57;}}else{
goto _LL57;}_LL57:;}
# 956
return;}case 0:
# 958
 return;case 36: _tmpDB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;{struct Cyc_List_List*x=_tmpDB;
# 960
for(1;x != 0;x=x->tl){
struct _tuple18*_tmpE3=(struct _tuple18*)x->hd;struct _tuple18*_stmttmp3=_tmpE3;struct _tuple18*_tmpE4=_stmttmp3;void*_tmpE5;_tmpE5=_tmpE4->f2;{struct Cyc_Absyn_Exp*e1=_tmpE5;
Cyc_scan_exp(e1,dep);}}
# 964
return;}case 40:
 return;case 2:
# 967
({void*_tmpE6=0U;({struct Cyc___cycFILE*_tmp48B=Cyc_stderr;struct _fat_ptr _tmp48A=({const char*_tmpE7="Error: unexpected Pragma_e\n";_tag_fat(_tmpE7,sizeof(char),28U);});Cyc_fprintf(_tmp48B,_tmp48A,_tag_fat(_tmpE6,sizeof(void*),0));});});
exit(1);return;case 35:
# 970
({void*_tmpE8=0U;({struct Cyc___cycFILE*_tmp48D=Cyc_stderr;struct _fat_ptr _tmp48C=({const char*_tmpE9="Error: unexpected Swap_e\n";_tag_fat(_tmpE9,sizeof(char),26U);});Cyc_fprintf(_tmp48D,_tmp48C,_tag_fat(_tmpE8,sizeof(void*),0));});});
exit(1);return;case 37:
# 973
({void*_tmpEA=0U;({struct Cyc___cycFILE*_tmp48F=Cyc_stderr;struct _fat_ptr _tmp48E=({const char*_tmpEB="Error: unexpected Stmt_e\n";_tag_fat(_tmpEB,sizeof(char),26U);});Cyc_fprintf(_tmp48F,_tmp48E,_tag_fat(_tmpEA,sizeof(void*),0));});});
exit(1);return;case 42:
# 976
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp491=Cyc_stderr;struct _fat_ptr _tmp490=({const char*_tmpED="Error: unexpected Assert_e\n";_tag_fat(_tmpED,sizeof(char),28U);});Cyc_fprintf(_tmp491,_tmp490,_tag_fat(_tmpEC,sizeof(void*),0));});});
exit(1);return;case 11:
# 979
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp493=Cyc_stderr;struct _fat_ptr _tmp492=({const char*_tmpEF="Error: unexpected Throw_e\n";_tag_fat(_tmpEF,sizeof(char),27U);});Cyc_fprintf(_tmp493,_tmp492,_tag_fat(_tmpEE,sizeof(void*),0));});});
exit(1);return;case 12:
# 982
({void*_tmpF0=0U;({struct Cyc___cycFILE*_tmp495=Cyc_stderr;struct _fat_ptr _tmp494=({const char*_tmpF1="Error: unexpected NoInstantiate_e\n";_tag_fat(_tmpF1,sizeof(char),35U);});Cyc_fprintf(_tmp495,_tmp494,_tag_fat(_tmpF0,sizeof(void*),0));});});
exit(1);return;case 13:
# 985
({void*_tmpF2=0U;({struct Cyc___cycFILE*_tmp497=Cyc_stderr;struct _fat_ptr _tmp496=({const char*_tmpF3="Error: unexpected Instantiate_e\n";_tag_fat(_tmpF3,sizeof(char),33U);});Cyc_fprintf(_tmp497,_tmp496,_tag_fat(_tmpF2,sizeof(void*),0));});});
exit(1);return;case 16:
# 988
({void*_tmpF4=0U;({struct Cyc___cycFILE*_tmp499=Cyc_stderr;struct _fat_ptr _tmp498=({const char*_tmpF5="Error: unexpected New_e\n";_tag_fat(_tmpF5,sizeof(char),25U);});Cyc_fprintf(_tmp499,_tmp498,_tag_fat(_tmpF4,sizeof(void*),0));});});
exit(1);return;case 24:
# 991
({void*_tmpF6=0U;({struct Cyc___cycFILE*_tmp49B=Cyc_stderr;struct _fat_ptr _tmp49A=({const char*_tmpF7="Error: unexpected Tuple_e\n";_tag_fat(_tmpF7,sizeof(char),27U);});Cyc_fprintf(_tmp49B,_tmp49A,_tag_fat(_tmpF6,sizeof(void*),0));});});
exit(1);return;case 25:
# 994
({void*_tmpF8=0U;({struct Cyc___cycFILE*_tmp49D=Cyc_stderr;struct _fat_ptr _tmp49C=({const char*_tmpF9="Error: unexpected CompoundLit_e\n";_tag_fat(_tmpF9,sizeof(char),33U);});Cyc_fprintf(_tmp49D,_tmp49C,_tag_fat(_tmpF8,sizeof(void*),0));});});
exit(1);return;case 26:
# 997
({void*_tmpFA=0U;({struct Cyc___cycFILE*_tmp49F=Cyc_stderr;struct _fat_ptr _tmp49E=({const char*_tmpFB="Error: unexpected Array_e\n";_tag_fat(_tmpFB,sizeof(char),27U);});Cyc_fprintf(_tmp49F,_tmp49E,_tag_fat(_tmpFA,sizeof(void*),0));});});
exit(1);return;case 27:
# 1000
({void*_tmpFC=0U;({struct Cyc___cycFILE*_tmp4A1=Cyc_stderr;struct _fat_ptr _tmp4A0=({const char*_tmpFD="Error: unexpected Comprehension_e\n";_tag_fat(_tmpFD,sizeof(char),35U);});Cyc_fprintf(_tmp4A1,_tmp4A0,_tag_fat(_tmpFC,sizeof(void*),0));});});
exit(1);return;case 28:
# 1003
({void*_tmpFE=0U;({struct Cyc___cycFILE*_tmp4A3=Cyc_stderr;struct _fat_ptr _tmp4A2=({const char*_tmpFF="Error: unexpected ComprehensionNoinit_e\n";_tag_fat(_tmpFF,sizeof(char),41U);});Cyc_fprintf(_tmp4A3,_tmp4A2,_tag_fat(_tmpFE,sizeof(void*),0));});});
exit(1);return;case 29:
# 1006
({void*_tmp100=0U;({struct Cyc___cycFILE*_tmp4A5=Cyc_stderr;struct _fat_ptr _tmp4A4=({const char*_tmp101="Error: unexpected Aggregate_e\n";_tag_fat(_tmp101,sizeof(char),31U);});Cyc_fprintf(_tmp4A5,_tmp4A4,_tag_fat(_tmp100,sizeof(void*),0));});});
exit(1);return;case 30:
# 1009
({void*_tmp102=0U;({struct Cyc___cycFILE*_tmp4A7=Cyc_stderr;struct _fat_ptr _tmp4A6=({const char*_tmp103="Error: unexpected AnonStruct_e\n";_tag_fat(_tmp103,sizeof(char),32U);});Cyc_fprintf(_tmp4A7,_tmp4A6,_tag_fat(_tmp102,sizeof(void*),0));});});
exit(1);return;case 31:
# 1012
({void*_tmp104=0U;({struct Cyc___cycFILE*_tmp4A9=Cyc_stderr;struct _fat_ptr _tmp4A8=({const char*_tmp105="Error: unexpected Datatype_e\n";_tag_fat(_tmp105,sizeof(char),30U);});Cyc_fprintf(_tmp4A9,_tmp4A8,_tag_fat(_tmp104,sizeof(void*),0));});});
exit(1);return;case 32:
# 1015
({void*_tmp106=0U;({struct Cyc___cycFILE*_tmp4AB=Cyc_stderr;struct _fat_ptr _tmp4AA=({const char*_tmp107="Error: unexpected Enum_e\n";_tag_fat(_tmp107,sizeof(char),26U);});Cyc_fprintf(_tmp4AB,_tmp4AA,_tag_fat(_tmp106,sizeof(void*),0));});});
exit(1);return;default:
# 1018
({void*_tmp108=0U;({struct Cyc___cycFILE*_tmp4AD=Cyc_stderr;struct _fat_ptr _tmp4AC=({const char*_tmp109="Error: unexpected AnonEnum_e\n";_tag_fat(_tmp109,sizeof(char),30U);});Cyc_fprintf(_tmp4AD,_tmp4AC,_tag_fat(_tmp108,sizeof(void*),0));});});
exit(1);return;};}
# 1023
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned)eo)Cyc_scan_exp(eo,dep);
return;}
# 1028
void Cyc_scan_decl(struct Cyc_Absyn_Decl*,struct Cyc_Hashtable_Table*);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp10A=t;struct Cyc_Absyn_FnInfo _tmp10C;void*_tmp10E;struct Cyc_Absyn_PtrInfo _tmp10F;void*_tmp10D;void*_tmp10B;switch(*((int*)_tmp10A)){case 0: _tmp10B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10A)->f1;_tmp10D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp10A)->f2;{void*c=_tmp10B;struct Cyc_List_List*ts=_tmp10D;
# 1032
void*_tmp110=c;void*_tmp111;union Cyc_Absyn_AggrInfo _tmp112;switch(*((int*)_tmp110)){case 0:
 goto _LL21;case 1: _LL21:
 goto _LL23;case 17: _LL23:
 goto _LL25;case 2: _LL25:
 goto _LL27;case 16: _LL27:
# 1038
 return;case 20: _tmp112=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp110)->f1;{union Cyc_Absyn_AggrInfo info=_tmp112;
# 1040
struct _tuple12 _tmp113=Cyc_Absyn_aggr_kinded_name(info);struct _tuple12 _stmttmp4=_tmp113;struct _tuple12 _tmp114=_stmttmp4;void*_tmp115;_tmp115=(_tmp114.f2)->f2;{struct _fat_ptr*v=_tmp115;
_tmp111=v;goto _LL2B;}}case 15: _tmp111=(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp110)->f1)->f2;_LL2B: {struct _fat_ptr*v=_tmp111;
Cyc_add_target(v);return;}case 18:
 goto _LL2F;case 19: _LL2F: goto _LL31;case 3: _LL31:
 goto _LL33;case 5: _LL33: goto _LL35;case 6: _LL35:
 goto _LL37;case 7: _LL37: goto _LL39;case 8: _LL39:
 goto _LL3B;case 9: _LL3B: goto _LL3D;case 10: _LL3D:
 goto _LL3F;case 4: _LL3F: goto _LL41;case 11: _LL41:
 goto _LL43;case 12: _LL43: goto _LL45;case 13: _LL45:
 goto _LL47;default: _LL47:
({struct Cyc_String_pa_PrintArg_struct _tmp118=({struct Cyc_String_pa_PrintArg_struct _tmp3A4;_tmp3A4.tag=0,({struct _fat_ptr _tmp4AE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp3A4.f1=_tmp4AE;});_tmp3A4;});void*_tmp116[1];_tmp116[0]=& _tmp118;({struct Cyc___cycFILE*_tmp4B0=Cyc_stderr;struct _fat_ptr _tmp4AF=({const char*_tmp117="Error: unexpected %s\n";_tag_fat(_tmp117,sizeof(char),22U);});Cyc_fprintf(_tmp4B0,_tmp4AF,_tag_fat(_tmp116,sizeof(void*),1));});});
exit(1);return;};}case 3: _tmp10F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp10A)->f1;{struct Cyc_Absyn_PtrInfo x=_tmp10F;
# 1055
Cyc_scan_type(x.elt_type,dep);
return;}case 4: _tmp10B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp10A)->f1).elt_type;_tmp10D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp10A)->f1).num_elts;_tmp10E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp10A)->f1).zero_term;{void*t=_tmp10B;struct Cyc_Absyn_Exp*sz=_tmp10D;void*zt=_tmp10E;
# 1058
Cyc_scan_type(t,dep);
Cyc_scan_exp_opt(sz,dep);
return;}case 11: _tmp10B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp10A)->f1;{struct Cyc_Absyn_Exp*e=_tmp10B;
# 1062
Cyc_scan_exp(e,dep);
return;}case 5: _tmp10C=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp10A)->f1;{struct Cyc_Absyn_FnInfo x=_tmp10C;
# 1065
Cyc_scan_type(x.ret_type,dep);
{struct Cyc_List_List*_tmp119=x.args;struct Cyc_List_List*a=_tmp119;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp11A=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5=_tmp11A;struct _tuple9*_tmp11B=_stmttmp5;void*_tmp11C;_tmp11C=_tmp11B->f3;{void*argt=_tmp11C;
Cyc_scan_type(argt,dep);}}}
# 1070
if(x.cyc_varargs != 0)
Cyc_scan_type((x.cyc_varargs)->type,dep);
return;}case 7: _tmp10B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp10A)->f2;{struct Cyc_List_List*sfs=_tmp10B;
# 1074
for(1;sfs != 0;sfs=sfs->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->width,dep);}
# 1078
return;}case 8: _tmp10B=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp10A)->f1)->f2;{struct _fat_ptr*v=_tmp10B;
# 1080
Cyc_add_target(v);
return;}case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10A)->f1)->r)){case 0: _tmp10B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10A)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp10B;
# 1084
({struct Cyc_Absyn_Decl*_tmp4B1=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=5,_tmp11D->f1=x;_tmp11D;}),0U);Cyc_scan_decl(_tmp4B1,dep);});{
struct _tuple1*_tmp11E=x->name;struct _tuple1*_stmttmp6=_tmp11E;struct _tuple1*_tmp11F=_stmttmp6;void*_tmp120;_tmp120=_tmp11F->f2;{struct _fat_ptr*n=_tmp120;
Cyc_add_target(n);
return;}}}case 1: _tmp10B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10A)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp10B;
# 1090
({struct Cyc_Absyn_Decl*_tmp4B2=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->tag=7,_tmp121->f1=x;_tmp121;}),0U);Cyc_scan_decl(_tmp4B2,dep);});{
struct _tuple1*_tmp122=x->name;struct _tuple1*_stmttmp7=_tmp122;struct _tuple1*_tmp123=_stmttmp7;void*_tmp124;_tmp124=_tmp123->f2;{struct _fat_ptr*n=_tmp124;
Cyc_add_target(n);
return;}}}default: _tmp10B=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp10A)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_tmp10B;
# 1096
({void*_tmp125=0U;({struct Cyc___cycFILE*_tmp4B4=Cyc_stderr;struct _fat_ptr _tmp4B3=({const char*_tmp126="Error: unexpected Datatype declaration\n";_tag_fat(_tmp126,sizeof(char),40U);});Cyc_fprintf(_tmp4B4,_tmp4B3,_tag_fat(_tmp125,sizeof(void*),0));});});
exit(1);return;}}case 1:
# 1099
({void*_tmp127=0U;({struct Cyc___cycFILE*_tmp4B6=Cyc_stderr;struct _fat_ptr _tmp4B5=({const char*_tmp128="Error: unexpected Evar\n";_tag_fat(_tmp128,sizeof(char),24U);});Cyc_fprintf(_tmp4B6,_tmp4B5,_tag_fat(_tmp127,sizeof(void*),0));});});
exit(1);return;case 2:
# 1102
({void*_tmp129=0U;({struct Cyc___cycFILE*_tmp4B8=Cyc_stderr;struct _fat_ptr _tmp4B7=({const char*_tmp12A="Error: unexpected VarType\n";_tag_fat(_tmp12A,sizeof(char),27U);});Cyc_fprintf(_tmp4B8,_tmp4B7,_tag_fat(_tmp129,sizeof(void*),0));});});
exit(1);return;case 6:
# 1105
({void*_tmp12B=0U;({struct Cyc___cycFILE*_tmp4BA=Cyc_stderr;struct _fat_ptr _tmp4B9=({const char*_tmp12C="Error: unexpected TupleType\n";_tag_fat(_tmp12C,sizeof(char),29U);});Cyc_fprintf(_tmp4BA,_tmp4B9,_tag_fat(_tmp12B,sizeof(void*),0));});});
exit(1);return;default:
# 1108
({void*_tmp12D=0U;({struct Cyc___cycFILE*_tmp4BC=Cyc_stderr;struct _fat_ptr _tmp4BB=({const char*_tmp12E="Error: unexpected valueof_t\n";_tag_fat(_tmp12E,sizeof(char),29U);});Cyc_fprintf(_tmp4BC,_tmp4BB,_tag_fat(_tmp12D,sizeof(void*),0));});});
exit(1);return;};}
# 1113
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp12F=Cyc_current_targets;struct Cyc_Set_Set**saved_targets=_tmp12F;
struct _fat_ptr*_tmp130=Cyc_current_source;struct _fat_ptr*saved_source=_tmp130;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp131=_cycalloc(sizeof(*_tmp131));({struct Cyc_Set_Set*_tmp4BD=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_tmp131=_tmp4BD;});_tmp131;});
{void*_tmp132=d->r;void*_stmttmp8=_tmp132;void*_tmp133=_stmttmp8;void*_tmp134;switch(*((int*)_tmp133)){case 0: _tmp134=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp133)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp134;
# 1119
struct _tuple1*_tmp135=x->name;struct _tuple1*_stmttmp9=_tmp135;struct _tuple1*_tmp136=_stmttmp9;void*_tmp137;_tmp137=_tmp136->f2;{struct _fat_ptr*v=_tmp137;
Cyc_current_source=v;
Cyc_scan_type(x->type,dep);
Cyc_scan_exp_opt(x->initializer,dep);
goto _LL0;}}case 1: _tmp134=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp133)->f1;{struct Cyc_Absyn_Fndecl*x=_tmp134;
# 1125
struct _tuple1*_tmp138=x->name;struct _tuple1*_stmttmpA=_tmp138;struct _tuple1*_tmp139=_stmttmpA;void*_tmp13A;_tmp13A=_tmp139->f2;{struct _fat_ptr*v=_tmp13A;
Cyc_current_source=v;
Cyc_scan_type((x->i).ret_type,dep);
{struct Cyc_List_List*_tmp13B=(x->i).args;struct Cyc_List_List*a=_tmp13B;for(0;a != 0;a=a->tl){
struct _tuple9*_tmp13C=(struct _tuple9*)a->hd;struct _tuple9*_stmttmpB=_tmp13C;struct _tuple9*_tmp13D=_stmttmpB;void*_tmp13E;_tmp13E=_tmp13D->f3;{void*t1=_tmp13E;
Cyc_scan_type(t1,dep);}}}
# 1132
if((x->i).cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null((x->i).cyc_varargs))->type,dep);
if(x->is_inline)
({void*_tmp13F=0U;({struct Cyc___cycFILE*_tmp4BF=Cyc_stderr;struct _fat_ptr _tmp4BE=({const char*_tmp140="Warning: ignoring inline function\n";_tag_fat(_tmp140,sizeof(char),35U);});Cyc_fprintf(_tmp4BF,_tmp4BE,_tag_fat(_tmp13F,sizeof(void*),0));});});
goto _LL0;}}case 5: _tmp134=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp133)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp134;
# 1138
struct _tuple1*_tmp141=x->name;struct _tuple1*_stmttmpC=_tmp141;struct _tuple1*_tmp142=_stmttmpC;void*_tmp143;_tmp143=_tmp142->f2;{struct _fat_ptr*v=_tmp143;
Cyc_current_source=v;
if((unsigned)x->impl){
{struct Cyc_List_List*_tmp144=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp144;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp145=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp145;
Cyc_scan_type(f->type,dep);
Cyc_scan_exp_opt(f->width,dep);}}{
# 1148
struct Cyc_List_List*_tmp146=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(x->impl))->fields;struct Cyc_List_List*fs=_tmp146;for(0;fs != 0;fs=fs->tl){;}}}
# 1152
goto _LL0;}}case 7: _tmp134=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp133)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp134;
# 1154
struct _tuple1*_tmp147=x->name;struct _tuple1*_stmttmpD=_tmp147;struct _tuple1*_tmp148=_stmttmpD;void*_tmp149;_tmp149=_tmp148->f2;{struct _fat_ptr*v=_tmp149;
Cyc_current_source=v;
if((unsigned)x->fields){
{struct Cyc_List_List*_tmp14A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp14A;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp14B=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp14B;
Cyc_scan_exp_opt(f->tag,dep);}}{
# 1163
struct Cyc_List_List*_tmp14C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp14C;for(0;fs != 0;fs=fs->tl){;}}}
# 1167
goto _LL0;}}case 8: _tmp134=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp133)->f1;{struct Cyc_Absyn_Typedefdecl*x=_tmp134;
# 1169
struct _tuple1*_tmp14D=x->name;struct _tuple1*_stmttmpE=_tmp14D;struct _tuple1*_tmp14E=_stmttmpE;void*_tmp14F;_tmp14F=_tmp14E->f2;{struct _fat_ptr*v=_tmp14F;
Cyc_current_source=v;
if((unsigned)x->defn)
Cyc_scan_type((void*)_check_null(x->defn),dep);
goto _LL0;}}case 4:
# 1175
({void*_tmp150=0U;({struct Cyc___cycFILE*_tmp4C1=Cyc_stderr;struct _fat_ptr _tmp4C0=({const char*_tmp151="Error: unexpected region declaration";_tag_fat(_tmp151,sizeof(char),37U);});Cyc_fprintf(_tmp4C1,_tmp4C0,_tag_fat(_tmp150,sizeof(void*),0));});});
exit(1);case 13:
# 1178
({void*_tmp152=0U;({struct Cyc___cycFILE*_tmp4C3=Cyc_stderr;struct _fat_ptr _tmp4C2=({const char*_tmp153="Error: unexpected __cyclone_port_on__";_tag_fat(_tmp153,sizeof(char),38U);});Cyc_fprintf(_tmp4C3,_tmp4C2,_tag_fat(_tmp152,sizeof(void*),0));});});
exit(1);case 14:
# 1181
({void*_tmp154=0U;({struct Cyc___cycFILE*_tmp4C5=Cyc_stderr;struct _fat_ptr _tmp4C4=({const char*_tmp155="Error: unexpected __cyclone_port_off__";_tag_fat(_tmp155,sizeof(char),39U);});Cyc_fprintf(_tmp4C5,_tmp4C4,_tag_fat(_tmp154,sizeof(void*),0));});});
exit(1);case 15:
# 1184
({void*_tmp156=0U;({struct Cyc___cycFILE*_tmp4C7=Cyc_stderr;struct _fat_ptr _tmp4C6=({const char*_tmp157="Error: unexpected __tempest_on__";_tag_fat(_tmp157,sizeof(char),33U);});Cyc_fprintf(_tmp4C7,_tmp4C6,_tag_fat(_tmp156,sizeof(void*),0));});});
exit(1);case 16:
# 1187
({void*_tmp158=0U;({struct Cyc___cycFILE*_tmp4C9=Cyc_stderr;struct _fat_ptr _tmp4C8=({const char*_tmp159="Error: unexpected __tempest_off__";_tag_fat(_tmp159,sizeof(char),34U);});Cyc_fprintf(_tmp4C9,_tmp4C8,_tag_fat(_tmp158,sizeof(void*),0));});});
exit(1);case 2:
# 1190
({void*_tmp15A=0U;({struct Cyc___cycFILE*_tmp4CB=Cyc_stderr;struct _fat_ptr _tmp4CA=({const char*_tmp15B="Error: unexpected let declaration\n";_tag_fat(_tmp15B,sizeof(char),35U);});Cyc_fprintf(_tmp4CB,_tmp4CA,_tag_fat(_tmp15A,sizeof(void*),0));});});
exit(1);case 6:
# 1193
({void*_tmp15C=0U;({struct Cyc___cycFILE*_tmp4CD=Cyc_stderr;struct _fat_ptr _tmp4CC=({const char*_tmp15D="Error: unexpected datatype declaration\n";_tag_fat(_tmp15D,sizeof(char),40U);});Cyc_fprintf(_tmp4CD,_tmp4CC,_tag_fat(_tmp15C,sizeof(void*),0));});});
exit(1);case 3:
# 1196
({void*_tmp15E=0U;({struct Cyc___cycFILE*_tmp4CF=Cyc_stderr;struct _fat_ptr _tmp4CE=({const char*_tmp15F="Error: unexpected let declaration\n";_tag_fat(_tmp15F,sizeof(char),35U);});Cyc_fprintf(_tmp4CF,_tmp4CE,_tag_fat(_tmp15E,sizeof(void*),0));});});
exit(1);case 9:
# 1199
({void*_tmp160=0U;({struct Cyc___cycFILE*_tmp4D1=Cyc_stderr;struct _fat_ptr _tmp4D0=({const char*_tmp161="Error: unexpected namespace declaration\n";_tag_fat(_tmp161,sizeof(char),41U);});Cyc_fprintf(_tmp4D1,_tmp4D0,_tag_fat(_tmp160,sizeof(void*),0));});});
exit(1);case 10:
# 1202
({void*_tmp162=0U;({struct Cyc___cycFILE*_tmp4D3=Cyc_stderr;struct _fat_ptr _tmp4D2=({const char*_tmp163="Error: unexpected using declaration\n";_tag_fat(_tmp163,sizeof(char),37U);});Cyc_fprintf(_tmp4D3,_tmp4D2,_tag_fat(_tmp162,sizeof(void*),0));});});
exit(1);case 11:
# 1205
({void*_tmp164=0U;({struct Cyc___cycFILE*_tmp4D5=Cyc_stderr;struct _fat_ptr _tmp4D4=({const char*_tmp165="Error: unexpected extern \"C\" declaration\n";_tag_fat(_tmp165,sizeof(char),42U);});Cyc_fprintf(_tmp4D5,_tmp4D4,_tag_fat(_tmp164,sizeof(void*),0));});});
exit(1);default:
# 1208
({void*_tmp166=0U;({struct Cyc___cycFILE*_tmp4D7=Cyc_stderr;struct _fat_ptr _tmp4D6=({const char*_tmp167="Error: unexpected extern \"C include\" declaration\n";_tag_fat(_tmp167,sizeof(char),50U);});Cyc_fprintf(_tmp4D7,_tmp4D6,_tag_fat(_tmp166,sizeof(void*),0));});});
exit(1);}_LL0:;}{
# 1216
struct Cyc_Set_Set*old;
struct _fat_ptr*_tmp168=(struct _fat_ptr*)_check_null(Cyc_current_source);struct _fat_ptr*name=_tmp168;
{struct _handler_cons _tmp169;_push_handler(& _tmp169);{int _tmp16B=0;if(setjmp(_tmp169.handler))_tmp16B=1;if(!_tmp16B){
old=({(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(dep,name);;_pop_handler();}else{void*_tmp16A=(void*)Cyc_Core_get_exn_thrown();void*_tmp16C=_tmp16A;void*_tmp16D;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp16C)->tag == Cyc_Core_Not_found){
# 1221
old=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);goto _LL35;}else{_tmp16D=_tmp16C;{void*exn=_tmp16D;(int)_rethrow(exn);}}_LL35:;}}}{
# 1223
struct Cyc_Set_Set*_tmp16E=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_union_two;})(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);struct Cyc_Set_Set*targets=_tmp16E;
({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;})(dep,name,targets);
# 1226
Cyc_current_targets=saved_targets;
Cyc_current_source=saved_source;}}}
# 1230
struct Cyc_Hashtable_Table*Cyc_new_deps (void){
return({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;})(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1234
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _fat_ptr*x){
struct _handler_cons _tmp16F;_push_handler(& _tmp16F);{int _tmp171=0;if(setjmp(_tmp16F.handler))_tmp171=1;if(!_tmp171){{struct Cyc_Set_Set*_tmp172=({(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(t,x);_npop_handler(0);return _tmp172;};_pop_handler();}else{void*_tmp170=(void*)Cyc_Core_get_exn_thrown();void*_tmp173=_tmp170;void*_tmp174;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp173)->tag == Cyc_Core_Not_found)
# 1237
return({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);else{_tmp174=_tmp173;{void*exn=_tmp174;(int)_rethrow(exn);}};}}}
# 1241
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1251 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(curr,(struct _fat_ptr*)init->hd);}{
# 1256
struct Cyc_Set_Set*_tmp175=curr;struct Cyc_Set_Set*delta=_tmp175;
# 1258
struct _fat_ptr*_tmp176=({struct _fat_ptr*_tmp17A=_cycalloc(sizeof(*_tmp17A));({struct _fat_ptr _tmp4D8=({const char*_tmp179="";_tag_fat(_tmp179,sizeof(char),1U);});*_tmp17A=_tmp4D8;});_tmp17A;});struct _fat_ptr*sptr=_tmp176;
while(({(int(*)(struct Cyc_Set_Set*))Cyc_Set_cardinality;})(delta)> 0){
struct Cyc_Set_Set*_tmp177=emptyset;struct Cyc_Set_Set*next=_tmp177;
struct Cyc_Iter_Iter _tmp178=({(struct Cyc_Iter_Iter(*)(struct _RegionHandle*,struct Cyc_Set_Set*))Cyc_Set_make_iter;})(Cyc_Core_heap_region,delta);struct Cyc_Iter_Iter iter=_tmp178;
while(({(int(*)(struct Cyc_Iter_Iter,struct _fat_ptr**))Cyc_Iter_next;})(iter,& sptr)){
next=({struct Cyc_Set_Set*(*_tmp4DA)(struct Cyc_Set_Set*,struct Cyc_Set_Set*)=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_union_two;});struct Cyc_Set_Set*_tmp4D9=next;_tmp4DA(_tmp4D9,Cyc_find(t,sptr));});}
delta=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_diff;})(next,curr);
curr=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*))Cyc_Set_union_two;})(curr,delta);}
# 1267
return curr;}}
# 1270
enum Cyc_buildlib_mode{Cyc_NORMAL =0U,Cyc_GATHER =1U,Cyc_GATHERSCRIPT =2U,Cyc_FINISH =3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering (void){
return(int)Cyc_mode == 1 ||(int)Cyc_mode == 2;}
# 1276
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _fat_ptr fmt,struct _fat_ptr ap){
# 1280
if(Cyc_script_file == 0){
({void*_tmp17B=0U;({struct Cyc___cycFILE*_tmp4DC=Cyc_stderr;struct _fat_ptr _tmp4DB=({const char*_tmp17C="Internal error: script file is NULL\n";_tag_fat(_tmp17C,sizeof(char),37U);});Cyc_fprintf(_tmp4DC,_tmp4DB,_tag_fat(_tmp17B,sizeof(void*),0));});});
exit(1);}
# 1284
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1287
int Cyc_force_directory(struct _fat_ptr d){
if((int)Cyc_mode == 2)
({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=0,_tmp3A6.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3A6;});struct Cyc_String_pa_PrintArg_struct _tmp180=({struct Cyc_String_pa_PrintArg_struct _tmp3A5;_tmp3A5.tag=0,_tmp3A5.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3A5;});void*_tmp17D[2];_tmp17D[0]=& _tmp17F,_tmp17D[1]=& _tmp180;({struct _fat_ptr _tmp4DD=({const char*_tmp17E="if ! test -e %s; then mkdir %s; fi\n";_tag_fat(_tmp17E,sizeof(char),36U);});Cyc_prscript(_tmp4DD,_tag_fat(_tmp17D,sizeof(void*),2));});});else{
# 1294
int _tmp181=({void*_tmp185=0U;Cyc_open((const char*)_untag_fat_ptr(d,sizeof(char),1U),0,_tag_fat(_tmp185,sizeof(unsigned short),0));});int fd=_tmp181;
if(fd == -1){
if(mkdir((const char*)_check_null(_untag_fat_ptr(d,sizeof(char),1U)),448)== -1){
({struct Cyc_String_pa_PrintArg_struct _tmp184=({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0,_tmp3A7.f1=(struct _fat_ptr)((struct _fat_ptr)d);_tmp3A7;});void*_tmp182[1];_tmp182[0]=& _tmp184;({struct Cyc___cycFILE*_tmp4DF=Cyc_stderr;struct _fat_ptr _tmp4DE=({const char*_tmp183="Error: could not create directory %s\n";_tag_fat(_tmp183,sizeof(char),38U);});Cyc_fprintf(_tmp4DF,_tmp4DE,_tag_fat(_tmp182,sizeof(void*),1));});});
return 1;}}else{
# 1301
close(fd);}}
# 1303
return 0;}
# 1306
int Cyc_force_directory_prefixes(struct _fat_ptr file){
# 1310
struct _fat_ptr _tmp186=Cyc_strdup((struct _fat_ptr)file);struct _fat_ptr curr=_tmp186;
# 1312
struct Cyc_List_List*_tmp187=0;struct Cyc_List_List*x=_tmp187;
while(1){
curr=Cyc_Filename_dirname((struct _fat_ptr)curr);
if(Cyc_strlen((struct _fat_ptr)curr)== 0U)break;
x=({struct Cyc_List_List*_tmp189=_cycalloc(sizeof(*_tmp189));({struct _fat_ptr*_tmp4E0=({struct _fat_ptr*_tmp188=_cycalloc(sizeof(*_tmp188));*_tmp188=(struct _fat_ptr)curr;_tmp188;});_tmp189->hd=_tmp4E0;}),_tmp189->tl=x;_tmp189;});}
# 1319
for(1;x != 0;x=x->tl){
if(Cyc_force_directory(*((struct _fat_ptr*)x->hd)))return 1;}
# 1322
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _fat_ptr f1;};
# 1329
static int Cyc_is_other_special(char c){
char _tmp18A=c;switch((int)_tmp18A){case 92:
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
# 1344
 goto _LL18;case 9: _LL18:
 return 1;default:
 return 0;};}
# 1350
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmp18B=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp18B;
# 1354
int _tmp18C=0;int single_quotes=_tmp18C;
int _tmp18D=0;int other_special=_tmp18D;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmp18E=((const char*)s.curr)[i];char c=_tmp18E;
if((int)c == 39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 1363
if(single_quotes == 0 && other_special == 0)
return s;
# 1367
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmp18F[3];({struct _fat_ptr*_tmp4E5=({struct _fat_ptr*_tmp191=_cycalloc(sizeof(*_tmp191));({struct _fat_ptr _tmp4E4=({const char*_tmp190="'";_tag_fat(_tmp190,sizeof(char),2U);});*_tmp191=_tmp4E4;});_tmp191;});_tmp18F[0]=_tmp4E5;}),({struct _fat_ptr*_tmp4E3=({struct _fat_ptr*_tmp192=_cycalloc(sizeof(*_tmp192));*_tmp192=(struct _fat_ptr)s;_tmp192;});_tmp18F[1]=_tmp4E3;}),({struct _fat_ptr*_tmp4E2=({struct _fat_ptr*_tmp194=_cycalloc(sizeof(*_tmp194));({struct _fat_ptr _tmp4E1=({const char*_tmp193="'";_tag_fat(_tmp193,sizeof(char),2U);});*_tmp194=_tmp4E1;});_tmp194;});_tmp18F[2]=_tmp4E2;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp18F,sizeof(struct _fat_ptr*),3));}));{
# 1371
unsigned long _tmp195=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmp195;
struct _fat_ptr s2=({unsigned _tmp1A0=(len2 + 1U)+ 1U;char*_tmp19F=_cycalloc_atomic(_check_times(_tmp1A0,sizeof(char)));({{unsigned _tmp3A8=len2 + 1U;unsigned i;for(i=0;i < _tmp3A8;++ i){_tmp19F[i]='\000';}_tmp19F[_tmp3A8]=0;}0;});_tag_fat(_tmp19F,sizeof(char),_tmp1A0);});
int _tmp196=0;int i=_tmp196;
int _tmp197=0;int j=_tmp197;
for(1;(unsigned long)i < len;++ i){
char _tmp198=((const char*)s.curr)[i];char c=_tmp198;
if((int)c == 39 || Cyc_is_other_special(c))
({struct _fat_ptr _tmp199=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp19A=*((char*)_check_fat_subscript(_tmp199,sizeof(char),0U));char _tmp19B='\\';if(_get_fat_size(_tmp199,sizeof(char))== 1U &&(_tmp19A == 0 && _tmp19B != 0))_throw_arraybounds();*((char*)_tmp199.curr)=_tmp19B;});
({struct _fat_ptr _tmp19C=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp19D=*((char*)_check_fat_subscript(_tmp19C,sizeof(char),0U));char _tmp19E=c;if(_get_fat_size(_tmp19C,sizeof(char))== 1U &&(_tmp19D == 0 && _tmp19E != 0))_throw_arraybounds();*((char*)_tmp19C.curr)=_tmp19E;});}
# 1381
return(struct _fat_ptr)s2;}}
# 1383
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));({struct _fat_ptr _tmp4E6=Cyc_sh_escape_string(*sp);*_tmp1A1=_tmp4E6;});_tmp1A1;});}
# 1388
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1396
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1401
({struct Cyc_String_pa_PrintArg_struct _tmp1A4=({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0,({
struct _fat_ptr _tmp4E7=(struct _fat_ptr)({const char*_tmp1A5=filename;_tag_fat(_tmp1A5,sizeof(char),_get_zero_arr_size_char((void*)_tmp1A5,1U));});_tmp3A9.f1=_tmp4E7;});_tmp3A9;});void*_tmp1A2[1];_tmp1A2[0]=& _tmp1A4;({struct Cyc___cycFILE*_tmp4E9=Cyc_stderr;struct _fat_ptr _tmp4E8=({const char*_tmp1A3="********************************* %s...\n";_tag_fat(_tmp1A3,sizeof(char),41U);});Cyc_fprintf(_tmp4E9,_tmp4E8,_tag_fat(_tmp1A2,sizeof(void*),1));});});
# 1404
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp1A8=({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0,({struct _fat_ptr _tmp4EA=(struct _fat_ptr)({const char*_tmp1A9=filename;_tag_fat(_tmp1A9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1A9,1U));});_tmp3AA.f1=_tmp4EA;});_tmp3AA;});void*_tmp1A6[1];_tmp1A6[0]=& _tmp1A8;({struct _fat_ptr _tmp4EB=({const char*_tmp1A7="\n%s:\n";_tag_fat(_tmp1A7,sizeof(char),6U);});Cyc_log(_tmp4EB,_tag_fat(_tmp1A6,sizeof(void*),1));});});{
# 1416 "buildlib.cyl"
struct _fat_ptr _tmp1AA=Cyc_Filename_basename(({const char*_tmp2F2=filename;_tag_fat(_tmp2F2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F2,1U));}));struct _fat_ptr basename=_tmp1AA;
struct _fat_ptr _tmp1AB=Cyc_Filename_dirname(({const char*_tmp2F1=filename;_tag_fat(_tmp2F1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F1,1U));}));struct _fat_ptr dirname=_tmp1AB;
struct _fat_ptr _tmp1AC=Cyc_Filename_chop_extension((struct _fat_ptr)basename);struct _fat_ptr choppedname=_tmp1AC;
const char*_tmp1AD=(const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp4EC=(struct _fat_ptr)choppedname;Cyc_strconcat(_tmp4EC,({const char*_tmp2F0=".iA";_tag_fat(_tmp2F0,sizeof(char),4U);}));}),sizeof(char),1U));const char*cppinfile=_tmp1AD;
# 1421
const char*_tmp1AE=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _tmp2EC=({struct Cyc_String_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=0,_tmp3DE.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DE;});void*_tmp2EA[1];_tmp2EA[0]=& _tmp2EC;({struct _fat_ptr _tmp4EF=({const char*_tmp2EB="%s.iB";_tag_fat(_tmp2EB,sizeof(char),6U);});Cyc_aprintf(_tmp4EF,_tag_fat(_tmp2EA,sizeof(void*),1));});}):({struct _fat_ptr _tmp4EE=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp4EE,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EF=({struct Cyc_String_pa_PrintArg_struct _tmp3DF;_tmp3DF.tag=0,_tmp3DF.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DF;});void*_tmp2ED[1];_tmp2ED[0]=& _tmp2EF;({struct _fat_ptr _tmp4ED=({const char*_tmp2EE="%s.iB";_tag_fat(_tmp2EE,sizeof(char),6U);});Cyc_aprintf(_tmp4ED,_tag_fat(_tmp2ED,sizeof(void*),1));});}));}),sizeof(char),1U));const char*macrosfile=_tmp1AE;
# 1426
const char*_tmp1AF=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _tmp2E6=({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0,_tmp3DC.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DC;});void*_tmp2E4[1];_tmp2E4[0]=& _tmp2E6;({struct _fat_ptr _tmp4F2=({const char*_tmp2E5="%s.iC";_tag_fat(_tmp2E5,sizeof(char),6U);});Cyc_aprintf(_tmp4F2,_tag_fat(_tmp2E4,sizeof(void*),1));});}):({struct _fat_ptr _tmp4F1=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp4F1,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E9=({struct Cyc_String_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=0,_tmp3DD.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DD;});void*_tmp2E7[1];_tmp2E7[0]=& _tmp2E9;({struct _fat_ptr _tmp4F0=({const char*_tmp2E8="%s.iC";_tag_fat(_tmp2E8,sizeof(char),6U);});Cyc_aprintf(_tmp4F0,_tag_fat(_tmp2E7,sizeof(void*),1));});}));}),sizeof(char),1U));const char*declsfile=_tmp1AF;
# 1431
const char*_tmp1B0=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=0,_tmp3DA.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DA;});void*_tmp2DE[1];_tmp2DE[0]=& _tmp2E0;({struct _fat_ptr _tmp4F5=({const char*_tmp2DF="%s.iD";_tag_fat(_tmp2DF,sizeof(char),6U);});Cyc_aprintf(_tmp4F5,_tag_fat(_tmp2DE,sizeof(void*),1));});}):({struct _fat_ptr _tmp4F4=(struct _fat_ptr)dirname;Cyc_Filename_concat(_tmp4F4,(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2E3=({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0,_tmp3DB.f1=(struct _fat_ptr)((struct _fat_ptr)choppedname);_tmp3DB;});void*_tmp2E1[1];_tmp2E1[0]=& _tmp2E3;({struct _fat_ptr _tmp4F3=({const char*_tmp2E2="%s.iD";_tag_fat(_tmp2E2,sizeof(char),6U);});Cyc_aprintf(_tmp4F3,_tag_fat(_tmp2E1,sizeof(void*),1));});}));}),sizeof(char),1U));const char*filtereddeclsfile=_tmp1B0;
# 1436
{struct _handler_cons _tmp1B1;_push_handler(& _tmp1B1);{int _tmp1B3=0;if(setjmp(_tmp1B1.handler))_tmp1B3=1;if(!_tmp1B3){
# 1439
if(Cyc_force_directory_prefixes(({const char*_tmp1B4=filename;_tag_fat(_tmp1B4,sizeof(char),_get_zero_arr_size_char((void*)_tmp1B4,1U));}))){
int _tmp1B5=1;_npop_handler(0);return _tmp1B5;}
# 1444
if((int)Cyc_mode != 3){
if((int)Cyc_mode == 2){
({struct Cyc_String_pa_PrintArg_struct _tmp1B8=({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0,({struct _fat_ptr _tmp4F6=(struct _fat_ptr)({const char*_tmp1B9=cppinfile;_tag_fat(_tmp1B9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1B9,1U));});_tmp3AB.f1=_tmp4F6;});_tmp3AB;});void*_tmp1B6[1];_tmp1B6[0]=& _tmp1B8;({struct _fat_ptr _tmp4F7=({const char*_tmp1B7="cat >%s <<XXX\n";_tag_fat(_tmp1B7,sizeof(char),15U);});Cyc_prscript(_tmp4F7,_tag_fat(_tmp1B6,sizeof(void*),1));});});
{struct Cyc_List_List*_tmp1BA=cpp_insert;struct Cyc_List_List*l=_tmp1BA;for(0;l != 0;l=l->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp3AC;_tmp3AC.tag=0,_tmp3AC.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)l->hd));_tmp3AC;});void*_tmp1BB[1];_tmp1BB[0]=& _tmp1BD;({struct _fat_ptr _tmp4F8=({const char*_tmp1BC="%s";_tag_fat(_tmp1BC,sizeof(char),3U);});Cyc_prscript(_tmp4F8,_tag_fat(_tmp1BB,sizeof(void*),1));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp1C0=({struct Cyc_String_pa_PrintArg_struct _tmp3AD;_tmp3AD.tag=0,({struct _fat_ptr _tmp4F9=(struct _fat_ptr)({const char*_tmp1C1=filename;_tag_fat(_tmp1C1,sizeof(char),_get_zero_arr_size_char((void*)_tmp1C1,1U));});_tmp3AD.f1=_tmp4F9;});_tmp3AD;});void*_tmp1BE[1];_tmp1BE[0]=& _tmp1C0;({struct _fat_ptr _tmp4FA=({const char*_tmp1BF="#include <%s>\n";_tag_fat(_tmp1BF,sizeof(char),15U);});Cyc_prscript(_tmp4FA,_tag_fat(_tmp1BE,sizeof(void*),1));});});
({void*_tmp1C2=0U;({struct _fat_ptr _tmp4FB=({const char*_tmp1C3="XXX\n";_tag_fat(_tmp1C3,sizeof(char),5U);});Cyc_prscript(_tmp4FB,_tag_fat(_tmp1C2,sizeof(void*),0));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1C6=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0,_tmp3B0.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3B0;});struct Cyc_String_pa_PrintArg_struct _tmp1C7=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0,({struct _fat_ptr _tmp4FC=(struct _fat_ptr)({const char*_tmp1CA=macrosfile;_tag_fat(_tmp1CA,sizeof(char),_get_zero_arr_size_char((void*)_tmp1CA,1U));});_tmp3AF.f1=_tmp4FC;});_tmp3AF;});struct Cyc_String_pa_PrintArg_struct _tmp1C8=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0,({struct _fat_ptr _tmp4FD=(struct _fat_ptr)({const char*_tmp1C9=cppinfile;_tag_fat(_tmp1C9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1C9,1U));});_tmp3AE.f1=_tmp4FD;});_tmp3AE;});void*_tmp1C4[3];_tmp1C4[0]=& _tmp1C6,_tmp1C4[1]=& _tmp1C7,_tmp1C4[2]=& _tmp1C8;({struct _fat_ptr _tmp4FE=({const char*_tmp1C5="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_fat(_tmp1C5,sizeof(char),35U);});Cyc_prscript(_tmp4FE,_tag_fat(_tmp1C4,sizeof(void*),3));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1CD=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0,_tmp3B3.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3B3;});struct Cyc_String_pa_PrintArg_struct _tmp1CE=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0,({struct _fat_ptr _tmp4FF=(struct _fat_ptr)({const char*_tmp1D1=declsfile;_tag_fat(_tmp1D1,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D1,1U));});_tmp3B2.f1=_tmp4FF;});_tmp3B2;});struct Cyc_String_pa_PrintArg_struct _tmp1CF=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0,({struct _fat_ptr _tmp500=(struct _fat_ptr)({const char*_tmp1D0=cppinfile;_tag_fat(_tmp1D0,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D0,1U));});_tmp3B1.f1=_tmp500;});_tmp3B1;});void*_tmp1CB[3];_tmp1CB[0]=& _tmp1CD,_tmp1CB[1]=& _tmp1CE,_tmp1CB[2]=& _tmp1CF;({struct _fat_ptr _tmp501=({const char*_tmp1CC="$GCC %s -E     -o %s -x c %s;\n";_tag_fat(_tmp1CC,sizeof(char),31U);});Cyc_prscript(_tmp501,_tag_fat(_tmp1CB,sizeof(void*),3));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1D4=({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0,({struct _fat_ptr _tmp502=(struct _fat_ptr)({const char*_tmp1D5=cppinfile;_tag_fat(_tmp1D5,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D5,1U));});_tmp3B4.f1=_tmp502;});_tmp3B4;});void*_tmp1D2[1];_tmp1D2[0]=& _tmp1D4;({struct _fat_ptr _tmp503=({const char*_tmp1D3="rm %s\n";_tag_fat(_tmp1D3,sizeof(char),7U);});Cyc_prscript(_tmp503,_tag_fat(_tmp1D2,sizeof(void*),1));});});}else{
# 1456
maybe=Cyc_fopen(cppinfile,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp1D8=({struct Cyc_String_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=0,({struct _fat_ptr _tmp504=(struct _fat_ptr)({const char*_tmp1D9=cppinfile;_tag_fat(_tmp1D9,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D9,1U));});_tmp3B5.f1=_tmp504;});_tmp3B5;});void*_tmp1D6[1];_tmp1D6[0]=& _tmp1D8;({struct Cyc___cycFILE*_tmp506=Cyc_stderr;struct _fat_ptr _tmp505=({const char*_tmp1D7="Error: could not create file %s\n";_tag_fat(_tmp1D7,sizeof(char),33U);});Cyc_fprintf(_tmp506,_tmp505,_tag_fat(_tmp1D6,sizeof(void*),1));});});{
int _tmp1DA=1;_npop_handler(0);return _tmp1DA;}}
# 1461
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp1DD=({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0,({struct _fat_ptr _tmp507=(struct _fat_ptr)({const char*_tmp1DE=cppinfile;_tag_fat(_tmp1DE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1DE,1U));});_tmp3B6.f1=_tmp507;});_tmp3B6;});void*_tmp1DB[1];_tmp1DB[0]=& _tmp1DD;({struct Cyc___cycFILE*_tmp509=Cyc_stderr;struct _fat_ptr _tmp508=({const char*_tmp1DC="Creating %s\n";_tag_fat(_tmp1DC,sizeof(char),13U);});Cyc_fprintf(_tmp509,_tmp508,_tag_fat(_tmp1DB,sizeof(void*),1));});});
out_file=maybe;
{struct Cyc_List_List*_tmp1DF=cpp_insert;struct Cyc_List_List*l=_tmp1DF;for(0;l != 0;l=l->tl){
Cyc_fputs((const char*)_check_null(_untag_fat_ptr(*((struct _fat_ptr*)l->hd),sizeof(char),1U)),out_file);}}
# 1467
({struct Cyc_String_pa_PrintArg_struct _tmp1E2=({struct Cyc_String_pa_PrintArg_struct _tmp3B7;_tmp3B7.tag=0,({struct _fat_ptr _tmp50A=(struct _fat_ptr)({const char*_tmp1E3=filename;_tag_fat(_tmp1E3,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E3,1U));});_tmp3B7.f1=_tmp50A;});_tmp3B7;});void*_tmp1E0[1];_tmp1E0[0]=& _tmp1E2;({struct Cyc___cycFILE*_tmp50C=out_file;struct _fat_ptr _tmp50B=({const char*_tmp1E1="#include <%s>\n";_tag_fat(_tmp1E1,sizeof(char),15U);});Cyc_fprintf(_tmp50C,_tmp50B,_tag_fat(_tmp1E0,sizeof(void*),1));});});
Cyc_fclose(out_file);{
struct _fat_ptr _tmp1E4=({
struct Cyc_List_List*_tmp511=({struct Cyc_List_List*_tmp201=_cycalloc(sizeof(*_tmp201));({struct _fat_ptr*_tmp510=({struct _fat_ptr*_tmp200=_cycalloc(sizeof(*_tmp200));({struct _fat_ptr _tmp50F=(struct _fat_ptr)({const char*_tmp1FF="";_tag_fat(_tmp1FF,sizeof(char),1U);});*_tmp200=_tmp50F;});_tmp200;});_tmp201->hd=_tmp510;}),({
struct Cyc_List_List*_tmp50E=({struct Cyc_List_List*(*_tmp50D)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;});_tmp50D(Cyc_sh_escape_stringptr,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_cppargs));});_tmp201->tl=_tmp50E;});_tmp201;});
# 1470
Cyc_str_sepstr(_tmp511,({const char*_tmp202=" ";_tag_fat(_tmp202,sizeof(char),2U);}));});
# 1469
struct _fat_ptr cppargs_string=_tmp1E4;
# 1473
char*cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp1F8=({struct Cyc_String_pa_PrintArg_struct _tmp3C3;_tmp3C3.tag=0,_tmp3C3.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3C3;});struct Cyc_String_pa_PrintArg_struct _tmp1F9=({struct Cyc_String_pa_PrintArg_struct _tmp3C2;_tmp3C2.tag=0,_tmp3C2.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3C2;});struct Cyc_String_pa_PrintArg_struct _tmp1FA=({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0,_tmp3C1.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3C1;});struct Cyc_String_pa_PrintArg_struct _tmp1FB=({struct Cyc_String_pa_PrintArg_struct _tmp3C0;_tmp3C0.tag=0,({struct _fat_ptr _tmp512=(struct _fat_ptr)({const char*_tmp1FE=macrosfile;_tag_fat(_tmp1FE,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FE,1U));});_tmp3C0.f1=_tmp512;});_tmp3C0;});struct Cyc_String_pa_PrintArg_struct _tmp1FC=({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0,({struct _fat_ptr _tmp513=(struct _fat_ptr)({const char*_tmp1FD=cppinfile;_tag_fat(_tmp1FD,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FD,1U));});_tmp3BF.f1=_tmp513;});_tmp3BF;});void*_tmp1F6[5];_tmp1F6[0]=& _tmp1F8,_tmp1F6[1]=& _tmp1F9,_tmp1F6[2]=& _tmp1FA,_tmp1F6[3]=& _tmp1FB,_tmp1F6[4]=& _tmp1FC;({struct _fat_ptr _tmp514=({const char*_tmp1F7="%s %s %s -E -dM -o %s -x c %s";_tag_fat(_tmp1F7,sizeof(char),30U);});Cyc_aprintf(_tmp514,_tag_fat(_tmp1F6,sizeof(void*),5));});}),sizeof(char),1U));
# 1476
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp1E7=({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0,({struct _fat_ptr _tmp515=(struct _fat_ptr)({char*_tmp1E8=cmd;_tag_fat(_tmp1E8,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E8,1U));});_tmp3B8.f1=_tmp515;});_tmp3B8;});void*_tmp1E5[1];_tmp1E5[0]=& _tmp1E7;({struct Cyc___cycFILE*_tmp517=Cyc_stderr;struct _fat_ptr _tmp516=({const char*_tmp1E6="%s\n";_tag_fat(_tmp1E6,sizeof(char),4U);});Cyc_fprintf(_tmp517,_tmp516,_tag_fat(_tmp1E5,sizeof(void*),1));});});
if(!system((const char*)cmd)){
# 1481
cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp1EB=({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0,_tmp3BD.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3BD;});struct Cyc_String_pa_PrintArg_struct _tmp1EC=({struct Cyc_String_pa_PrintArg_struct _tmp3BC;_tmp3BC.tag=0,_tmp3BC.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3BC;});struct Cyc_String_pa_PrintArg_struct _tmp1ED=({struct Cyc_String_pa_PrintArg_struct _tmp3BB;_tmp3BB.tag=0,_tmp3BB.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp3BB;});struct Cyc_String_pa_PrintArg_struct _tmp1EE=({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0,({struct _fat_ptr _tmp518=(struct _fat_ptr)({const char*_tmp1F1=declsfile;_tag_fat(_tmp1F1,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F1,1U));});_tmp3BA.f1=_tmp518;});_tmp3BA;});struct Cyc_String_pa_PrintArg_struct _tmp1EF=({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0,({struct _fat_ptr _tmp519=(struct _fat_ptr)({const char*_tmp1F0=cppinfile;_tag_fat(_tmp1F0,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F0,1U));});_tmp3B9.f1=_tmp519;});_tmp3B9;});void*_tmp1E9[5];_tmp1E9[0]=& _tmp1EB,_tmp1E9[1]=& _tmp1EC,_tmp1E9[2]=& _tmp1ED,_tmp1E9[3]=& _tmp1EE,_tmp1E9[4]=& _tmp1EF;({struct _fat_ptr _tmp51A=({const char*_tmp1EA="%s %s %s -E -o %s -x c %s";_tag_fat(_tmp1EA,sizeof(char),26U);});Cyc_aprintf(_tmp51A,_tag_fat(_tmp1E9,sizeof(void*),5));});}),sizeof(char),1U));
# 1484
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp1F4=({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0,({struct _fat_ptr _tmp51B=(struct _fat_ptr)({char*_tmp1F5=cmd;_tag_fat(_tmp1F5,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F5,1U));});_tmp3BE.f1=_tmp51B;});_tmp3BE;});void*_tmp1F2[1];_tmp1F2[0]=& _tmp1F4;({struct Cyc___cycFILE*_tmp51D=Cyc_stderr;struct _fat_ptr _tmp51C=({const char*_tmp1F3="%s\n";_tag_fat(_tmp1F3,sizeof(char),4U);});Cyc_fprintf(_tmp51D,_tmp51C,_tag_fat(_tmp1F2,sizeof(void*),1));});});
system((const char*)cmd);}}}}
# 1491
if(Cyc_gathering()){int _tmp203=0;_npop_handler(0);return _tmp203;}{
# 1494
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp520=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp206=({struct Cyc_String_pa_PrintArg_struct _tmp3C4;_tmp3C4.tag=0,({struct _fat_ptr _tmp51E=(struct _fat_ptr)({const char*_tmp207=macrosfile;_tag_fat(_tmp207,sizeof(char),_get_zero_arr_size_char((void*)_tmp207,1U));});_tmp3C4.f1=_tmp51E;});_tmp3C4;});void*_tmp204[1];_tmp204[0]=& _tmp206;({struct _fat_ptr _tmp51F=({const char*_tmp205="can't open macrosfile %s";_tag_fat(_tmp205,sizeof(char),25U);});Cyc_aprintf(_tmp51F,_tag_fat(_tmp204,sizeof(void*),1));});});_tmp208->f1=_tmp520;});_tmp208;}));
# 1499
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp209=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp209;
struct _tuple13*entry;
while((entry=({(struct _tuple13*(*)(struct Cyc_Lexing_lexbuf*))Cyc_line;})(l))!= 0){
struct _tuple13*_tmp20A=(struct _tuple13*)_check_null(entry);struct _tuple13*_stmttmpF=_tmp20A;struct _tuple13*_tmp20B=_stmttmpF;void*_tmp20D;void*_tmp20C;_tmp20C=_tmp20B->f1;_tmp20D=_tmp20B->f2;{struct _fat_ptr*name=_tmp20C;struct Cyc_Set_Set*uses=_tmp20D;
({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;})(t,name,uses);}}
# 1508
Cyc_fclose(in_file);
# 1511
maybe=Cyc_fopen(declsfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp523=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp210=({struct Cyc_String_pa_PrintArg_struct _tmp3C5;_tmp3C5.tag=0,({struct _fat_ptr _tmp521=(struct _fat_ptr)({const char*_tmp211=declsfile;_tag_fat(_tmp211,sizeof(char),_get_zero_arr_size_char((void*)_tmp211,1U));});_tmp3C5.f1=_tmp521;});_tmp3C5;});void*_tmp20E[1];_tmp20E[0]=& _tmp210;({struct _fat_ptr _tmp522=({const char*_tmp20F="can't open declsfile %s";_tag_fat(_tmp20F,sizeof(char),24U);});Cyc_aprintf(_tmp522,_tag_fat(_tmp20E,sizeof(void*),1));});});_tmp212->f1=_tmp523;});_tmp212;}));
# 1515
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(filtereddeclsfile,"w");
if(!((unsigned)Cyc_slurp_out)){int _tmp213=1;_npop_handler(0);return _tmp213;}
while(({(int(*)(struct Cyc_Lexing_lexbuf*))Cyc_slurp;})(l)){;}{
# 1521
struct Cyc_List_List*_tmp214=user_defs;struct Cyc_List_List*x=_tmp214;
while(x != 0){
struct _tuple16*_tmp215=(struct _tuple16*)x->hd;struct _tuple16*_stmttmp10=_tmp215;struct _tuple16*_tmp216=_stmttmp10;void*_tmp217;_tmp217=_tmp216->f2;{struct _fat_ptr*s=_tmp217;
({struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=0,_tmp3C6.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_tmp3C6;});void*_tmp218[1];_tmp218[0]=& _tmp21A;({struct Cyc___cycFILE*_tmp525=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _fat_ptr _tmp524=({const char*_tmp219="%s";_tag_fat(_tmp219,sizeof(char),3U);});Cyc_fprintf(_tmp525,_tmp524,_tag_fat(_tmp218,sizeof(void*),1));});});
x=x->tl;}}
# 1527
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if((int)Cyc_mode != 3)
;
# 1533
maybe=Cyc_fopen(filtereddeclsfile,"r");
if(!((unsigned)maybe)){int _tmp21B=1;_npop_handler(0);return _tmp21B;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp21C=filtereddeclsfile;_tag_fat(_tmp21C,sizeof(char),_get_zero_arr_size_char((void*)_tmp21C,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp21D=Cyc_Parse_parse_file(in_file);struct Cyc_List_List*decls=_tmp21D;
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1543
{struct Cyc_List_List*_tmp21E=decls;struct Cyc_List_List*d=_tmp21E;for(0;d != 0;d=d->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)d->hd,t);}}{
# 1547
struct Cyc_List_List*_tmp21F=({struct Cyc_List_List*(*_tmp526)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;});_tmp526(Cyc_add_user_prefix,(({(struct _tuple0(*)(struct Cyc_List_List*))Cyc_List_split;})(user_defs)).f1);});struct Cyc_List_List*user_symbols=_tmp21F;
struct Cyc_Set_Set*_tmp220=({struct Cyc_List_List*_tmp527=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(start_symbols,user_symbols);Cyc_reachable(_tmp527,t);});struct Cyc_Set_Set*reachable_set=_tmp220;
# 1551
struct Cyc_List_List*_tmp221=0;struct Cyc_List_List*reachable_decls=_tmp221;
struct Cyc_List_List*_tmp222=0;struct Cyc_List_List*user_decls=_tmp222;
struct Cyc_Set_Set*defined_symbols=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp223=decls;struct Cyc_List_List*d=_tmp223;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp224=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp224;
struct _fat_ptr*name;
{void*_tmp225=decl->r;void*_stmttmp11=_tmp225;void*_tmp226=_stmttmp11;void*_tmp227;switch(*((int*)_tmp226)){case 0: _tmp227=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp226)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp227;
# 1559
struct _tuple1*_tmp228=x->name;struct _tuple1*_stmttmp12=_tmp228;struct _tuple1*_tmp229=_stmttmp12;void*_tmp22A;_tmp22A=_tmp229->f2;{struct _fat_ptr*v=_tmp22A;
defined_symbols=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(defined_symbols,v);
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 1: _tmp227=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp226)->f1;{struct Cyc_Absyn_Fndecl*x=_tmp227;
# 1565
struct _tuple1*_tmp22B=x->name;struct _tuple1*_stmttmp13=_tmp22B;struct _tuple1*_tmp22C=_stmttmp13;void*_tmp22D;_tmp22D=_tmp22C->f2;{struct _fat_ptr*v=_tmp22D;
defined_symbols=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(defined_symbols,v);
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 5: _tmp227=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp226)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp227;
# 1571
struct _tuple1*_tmp22E=x->name;struct _tuple1*_stmttmp14=_tmp22E;struct _tuple1*_tmp22F=_stmttmp14;void*_tmp230;_tmp230=_tmp22F->f2;{struct _fat_ptr*v=_tmp230;
name=v;
goto _LL6;}}case 7: _tmp227=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp226)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp227;
# 1575
struct _tuple1*_tmp231=x->name;struct _tuple1*_stmttmp15=_tmp231;struct _tuple1*_tmp232=_stmttmp15;void*_tmp233;_tmp233=_tmp232->f2;{struct _fat_ptr*v=_tmp233;
name=v;
# 1579
if(name != 0 &&({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name))
reachable_decls=({struct Cyc_List_List*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->hd=decl,_tmp234->tl=reachable_decls;_tmp234;});else{
# 1582
if((unsigned)x->fields){
struct Cyc_List_List*_tmp235=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(x->fields))->v;struct Cyc_List_List*fs=_tmp235;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp236=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp236;
struct _tuple1*_tmp237=f->name;struct _tuple1*_stmttmp16=_tmp237;struct _tuple1*_tmp238=_stmttmp16;void*_tmp239;_tmp239=_tmp238->f2;{struct _fat_ptr*v=_tmp239;
if(({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,v)){
reachable_decls=({struct Cyc_List_List*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->hd=decl,_tmp23A->tl=reachable_decls;_tmp23A;});
break;}}}}}
# 1592
name=0;
goto _LL6;}}case 8: _tmp227=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp226)->f1;{struct Cyc_Absyn_Typedefdecl*x=_tmp227;
# 1595
struct _tuple1*_tmp23B=x->name;struct _tuple1*_stmttmp17=_tmp23B;struct _tuple1*_tmp23C=_stmttmp17;void*_tmp23D;_tmp23D=_tmp23C->f2;{struct _fat_ptr*v=_tmp23D;
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
# 1610
 name=0;
goto _LL6;}_LL6:;}
# 1614
if(name != 0 &&({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name)){
if(({struct _fat_ptr _tmp529=(struct _fat_ptr)*name;struct _fat_ptr _tmp528=(struct _fat_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp529,_tmp528,Cyc_strlen((struct _fat_ptr)Cyc_user_prefix));})!= 0)
reachable_decls=({struct Cyc_List_List*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E->hd=decl,_tmp23E->tl=reachable_decls;_tmp23E;});else{
# 1619
Cyc_rename_decl(decl);
user_decls=({struct Cyc_List_List*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F->hd=decl,_tmp23F->tl=user_decls;_tmp23F;});}}}}
# 1626
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){int _tmp240=1;_npop_handler(0);return _tmp240;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _fat_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2B9=({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0,({struct _fat_ptr _tmp52A=(struct _fat_ptr)({const char*_tmp2BA=filename;_tag_fat(_tmp2BA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2BA,1U));});_tmp3D2.f1=_tmp52A;});_tmp3D2;});void*_tmp2B7[1];_tmp2B7[0]=& _tmp2B9;({struct _fat_ptr _tmp52B=({const char*_tmp2B8="_%s_";_tag_fat(_tmp2B8,sizeof(char),5U);});Cyc_aprintf(_tmp52B,_tag_fat(_tmp2B7,sizeof(void*),1));});});
{int _tmp241=0;int j=_tmp241;for(0;(unsigned)j < _get_fat_size(ifdefmacro,sizeof(char));++ j){
if((int)((char*)ifdefmacro.curr)[j]== 46 ||(int)((char*)ifdefmacro.curr)[j]== 47)
({struct _fat_ptr _tmp242=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp243=*((char*)_check_fat_subscript(_tmp242,sizeof(char),0U));char _tmp244='_';if(_get_fat_size(_tmp242,sizeof(char))== 1U &&(_tmp243 == 0 && _tmp244 != 0))_throw_arraybounds();*((char*)_tmp242.curr)=_tmp244;});else{
if((int)((char*)ifdefmacro.curr)[j]!= 95 &&(int)((char*)ifdefmacro.curr)[j]!= 47)
({struct _fat_ptr _tmp245=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _tmp246=*((char*)_check_fat_subscript(_tmp245,sizeof(char),0U));char _tmp247=(char)toupper((int)((char*)ifdefmacro.curr)[j]);if(_get_fat_size(_tmp245,sizeof(char))== 1U &&(_tmp246 == 0 && _tmp247 != 0))_throw_arraybounds();*((char*)_tmp245.curr)=_tmp247;});}}}
# 1638
({struct Cyc_String_pa_PrintArg_struct _tmp24A=({struct Cyc_String_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=0,_tmp3C8.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3C8;});struct Cyc_String_pa_PrintArg_struct _tmp24B=({struct Cyc_String_pa_PrintArg_struct _tmp3C7;_tmp3C7.tag=0,_tmp3C7.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3C7;});void*_tmp248[2];_tmp248[0]=& _tmp24A,_tmp248[1]=& _tmp24B;({struct Cyc___cycFILE*_tmp52D=out_file;struct _fat_ptr _tmp52C=({const char*_tmp249="#ifndef %s\n#define %s\n";_tag_fat(_tmp249,sizeof(char),23U);});Cyc_fprintf(_tmp52D,_tmp52C,_tag_fat(_tmp248,sizeof(void*),2));});});{
# 1645
struct Cyc_List_List*_tmp24C=0;struct Cyc_List_List*print_decls=_tmp24C;
struct Cyc_List_List*_tmp24D=0;struct Cyc_List_List*names=_tmp24D;
{struct Cyc_List_List*_tmp24E=reachable_decls;struct Cyc_List_List*d=_tmp24E;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*_tmp24F=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp24F;
int _tmp250=0;int anon_enum=_tmp250;
struct _fat_ptr*name;
{void*_tmp251=decl->r;void*_stmttmp18=_tmp251;void*_tmp252=_stmttmp18;void*_tmp253;switch(*((int*)_tmp252)){case 0: _tmp253=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp252)->f1;{struct Cyc_Absyn_Vardecl*x=_tmp253;
# 1653
struct _tuple1*_tmp254=x->name;struct _tuple1*_stmttmp19=_tmp254;struct _tuple1*_tmp255=_stmttmp19;void*_tmp256;_tmp256=_tmp255->f2;{struct _fat_ptr*v=_tmp256;
name=v;
goto _LL3B;}}case 1: _tmp253=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp252)->f1;{struct Cyc_Absyn_Fndecl*x=_tmp253;
# 1657
if(x->is_inline){name=0;goto _LL3B;}{
struct _tuple1*_tmp257=x->name;struct _tuple1*_stmttmp1A=_tmp257;struct _tuple1*_tmp258=_stmttmp1A;void*_tmp259;_tmp259=_tmp258->f2;{struct _fat_ptr*v=_tmp259;
name=v;
goto _LL3B;}}}case 5: _tmp253=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp252)->f1;{struct Cyc_Absyn_Aggrdecl*x=_tmp253;
# 1662
struct _tuple1*_tmp25A=x->name;struct _tuple1*_stmttmp1B=_tmp25A;struct _tuple1*_tmp25B=_stmttmp1B;void*_tmp25C;_tmp25C=_tmp25B->f2;{struct _fat_ptr*v=_tmp25C;
name=v;
goto _LL3B;}}case 7: _tmp253=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp252)->f1;{struct Cyc_Absyn_Enumdecl*x=_tmp253;
# 1666
struct _tuple1*_tmp25D=x->name;struct _tuple1*_stmttmp1C=_tmp25D;struct _tuple1*_tmp25E=_stmttmp1C;void*_tmp25F;_tmp25F=_tmp25E->f2;{struct _fat_ptr*v=_tmp25F;
name=v;
goto _LL3B;}}case 8: _tmp253=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp252)->f1;{struct Cyc_Absyn_Typedefdecl*x=_tmp253;
# 1670
struct _tuple1*_tmp260=x->name;struct _tuple1*_stmttmp1D=_tmp260;struct _tuple1*_tmp261=_stmttmp1D;void*_tmp262;_tmp262=_tmp261->f2;{struct _fat_ptr*v=_tmp262;
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
# 1685
 name=0;
goto _LL3B;}_LL3B:;}
# 1688
if(!((unsigned)name)&& !anon_enum)continue;
# 1693
if(({int(*_tmp52F)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp52E=reachable_set;_tmp52F(_tmp52E,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp263=Cyc_Absyn_lookup_decl(user_decls,name);struct Cyc_Absyn_Decl*user_decl=_tmp263;
if(user_decl == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp265=_cycalloc(sizeof(*_tmp265));_tmp265->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp530=({const char*_tmp264="Internal Error: bad user-def name";_tag_fat(_tmp264,sizeof(char),34U);});_tmp265->f1=_tmp530;});_tmp265;}));else{
# 1699
void*_tmp266=user_decl->r;void*_stmttmp1E=_tmp266;void*_tmp267=_stmttmp1E;switch(*((int*)_tmp267)){case 0:
 goto _LL71;case 1: _LL71:
# 1702
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp269=_cycalloc(sizeof(*_tmp269));_tmp269->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp531=({const char*_tmp268="user defintions for function or variable decls";_tag_fat(_tmp268,sizeof(char),47U);});_tmp269->f1=_tmp531;});_tmp269;}));default:
 goto _LL6D;}_LL6D:;}
# 1708
print_decls=({struct Cyc_List_List*_tmp26A=_cycalloc(sizeof(*_tmp26A));_tmp26A->hd=decl,_tmp26A->tl=print_decls;_tmp26A;});}else{
# 1711
print_decls=({struct Cyc_List_List*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->hd=decl,_tmp26B->tl=print_decls;_tmp26B;});}
names=({struct Cyc_List_List*_tmp26C=_cycalloc(sizeof(*_tmp26C));_tmp26C->hd=name,_tmp26C->tl=names;_tmp26C;});}}
# 1716
{struct _handler_cons _tmp26D;_push_handler(& _tmp26D);{int _tmp26F=0;if(setjmp(_tmp26D.handler))_tmp26F=1;if(!_tmp26F){
Cyc_Binding_resolve_all(print_decls);
({struct Cyc_Tcenv_Tenv*_tmp532=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp532,1,print_decls);});
# 1717
;_pop_handler();}else{void*_tmp26E=(void*)Cyc_Core_get_exn_thrown();void*_tmp270=_tmp26E;
# 1720
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp533=({const char*_tmp271="can't typecheck acquired declarations";_tag_fat(_tmp271,sizeof(char),38U);});_tmp272->f1=_tmp533;});_tmp272;}));;}}}
# 1724
{struct _tuple0 _tmp273=({struct _tuple0 _tmp3CC;_tmp3CC.f1=print_decls,_tmp3CC.f2=names;_tmp3CC;});struct _tuple0 _stmttmp1F=_tmp273;struct _tuple0 _tmp274=_stmttmp1F;void*_tmp276;void*_tmp275;_tmp275=_tmp274.f1;_tmp276=_tmp274.f2;{struct Cyc_List_List*d=_tmp275;struct Cyc_List_List*n=_tmp276;for(0;
d != 0 && n != 0;(d=d->tl,n=n->tl)){
struct Cyc_Absyn_Decl*_tmp277=(struct Cyc_Absyn_Decl*)d->hd;struct Cyc_Absyn_Decl*decl=_tmp277;
struct _fat_ptr*_tmp278=(struct _fat_ptr*)n->hd;struct _fat_ptr*name=_tmp278;
int _tmp279=0;int anon_enum=_tmp279;
if(!((unsigned)name))
anon_enum=1;
# 1733
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned)name){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp27C=({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0,_tmp3C9.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp3C9;});void*_tmp27A[1];_tmp27A[0]=& _tmp27C;({struct _fat_ptr _tmp534=({const char*_tmp27B="_%s_def_";_tag_fat(_tmp27B,sizeof(char),9U);});Cyc_aprintf(_tmp534,_tag_fat(_tmp27A,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0,_tmp3CA.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3CA;});void*_tmp27D[1];_tmp27D[0]=& _tmp27F;({struct Cyc___cycFILE*_tmp536=out_file;struct _fat_ptr _tmp535=({const char*_tmp27E="#ifndef %s\n";_tag_fat(_tmp27E,sizeof(char),12U);});Cyc_fprintf(_tmp536,_tmp535,_tag_fat(_tmp27D,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=0,_tmp3CB.f1=(struct _fat_ptr)((struct _fat_ptr)ifdefmacro);_tmp3CB;});void*_tmp280[1];_tmp280[0]=& _tmp282;({struct Cyc___cycFILE*_tmp538=out_file;struct _fat_ptr _tmp537=({const char*_tmp281="#define %s\n";_tag_fat(_tmp281,sizeof(char),12U);});Cyc_fprintf(_tmp538,_tmp537,_tag_fat(_tmp280,sizeof(void*),1));});});
# 1739
({struct Cyc_List_List*_tmp539=({struct Cyc_Absyn_Decl*_tmp283[1];_tmp283[0]=decl;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp283,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_tmp539,out_file);});
({void*_tmp284=0U;({struct Cyc___cycFILE*_tmp53B=out_file;struct _fat_ptr _tmp53A=({const char*_tmp285="#endif\n";_tag_fat(_tmp285,sizeof(char),8U);});Cyc_fprintf(_tmp53B,_tmp53A,_tag_fat(_tmp284,sizeof(void*),0));});});}else{
# 1744
({struct Cyc_List_List*_tmp53C=({struct Cyc_Absyn_Decl*_tmp286[1];_tmp286[0]=decl;({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp286,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_tmp53C,out_file);});}}}}
# 1749
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _tmp53F=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0,({struct _fat_ptr _tmp53D=(struct _fat_ptr)({const char*_tmp28A=macrosfile;_tag_fat(_tmp28A,sizeof(char),_get_zero_arr_size_char((void*)_tmp28A,1U));});_tmp3CD.f1=_tmp53D;});_tmp3CD;});void*_tmp287[1];_tmp287[0]=& _tmp289;({struct _fat_ptr _tmp53E=({const char*_tmp288="can't open macrosfile %s";_tag_fat(_tmp288,sizeof(char),25U);});Cyc_aprintf(_tmp53E,_tag_fat(_tmp287,sizeof(void*),1));});});_tmp28B->f1=_tmp53F;});_tmp28B;}));
# 1752
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);{
struct _tuple14*entry2;
while((entry2=({(struct _tuple14*(*)(struct Cyc_Lexing_lexbuf*))Cyc_suck_line;})(l))!= 0){
struct _tuple14*_tmp28C=(struct _tuple14*)_check_null(entry2);struct _tuple14*_stmttmp20=_tmp28C;struct _tuple14*_tmp28D=_stmttmp20;void*_tmp28F;struct _fat_ptr _tmp28E;_tmp28E=_tmp28D->f1;_tmp28F=_tmp28D->f2;{struct _fat_ptr line=_tmp28E;struct _fat_ptr*name=_tmp28F;
if(({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name)){
({struct Cyc_String_pa_PrintArg_struct _tmp292=({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0,_tmp3CE.f1=(struct _fat_ptr)((struct _fat_ptr)*name);_tmp3CE;});void*_tmp290[1];_tmp290[0]=& _tmp292;({struct Cyc___cycFILE*_tmp541=out_file;struct _fat_ptr _tmp540=({const char*_tmp291="#ifndef %s\n";_tag_fat(_tmp291,sizeof(char),12U);});Cyc_fprintf(_tmp541,_tmp540,_tag_fat(_tmp290,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp295=({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0,_tmp3CF.f1=(struct _fat_ptr)((struct _fat_ptr)line);_tmp3CF;});void*_tmp293[1];_tmp293[0]=& _tmp295;({struct Cyc___cycFILE*_tmp543=out_file;struct _fat_ptr _tmp542=({const char*_tmp294="%s\n";_tag_fat(_tmp294,sizeof(char),4U);});Cyc_fprintf(_tmp543,_tmp542,_tag_fat(_tmp293,sizeof(void*),1));});});
({void*_tmp296=0U;({struct Cyc___cycFILE*_tmp545=out_file;struct _fat_ptr _tmp544=({const char*_tmp297="#endif\n";_tag_fat(_tmp297,sizeof(char),8U);});Cyc_fprintf(_tmp545,_tmp544,_tag_fat(_tmp296,sizeof(void*),0));});});}}}
# 1763
Cyc_fclose(in_file);
if((int)Cyc_mode != 3);
# 1766
if(hstubs != 0){
struct Cyc_List_List*_tmp298=hstubs;struct Cyc_List_List*x=_tmp298;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp299=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp21=_tmp299;struct _tuple15*_tmp29A=_stmttmp21;struct _fat_ptr _tmp29C;struct _fat_ptr _tmp29B;_tmp29B=_tmp29A->f1;_tmp29C=_tmp29A->f2;{struct _fat_ptr symbol=_tmp29B;struct _fat_ptr text=_tmp29C;
if((char*)text.curr != (char*)(_tag_fat(0,0,0)).curr &&(
(char*)symbol.curr == (char*)(_tag_fat(0,0,0)).curr ||({int(*_tmp547)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp546=defined_symbols;_tmp547(_tmp546,({struct _fat_ptr*_tmp29D=_cycalloc(sizeof(*_tmp29D));*_tmp29D=symbol;_tmp29D;}));})))
# 1772
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);else{
# 1774
({struct Cyc_String_pa_PrintArg_struct _tmp2A0=({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0,_tmp3D0.f1=(struct _fat_ptr)((struct _fat_ptr)symbol);_tmp3D0;});void*_tmp29E[1];_tmp29E[0]=& _tmp2A0;({struct _fat_ptr _tmp548=({const char*_tmp29F="%s is not supported on this platform\n";_tag_fat(_tmp29F,sizeof(char),38U);});Cyc_log(_tmp548,_tag_fat(_tmp29E,sizeof(void*),1));});});}}}}
# 1777
({void*_tmp2A1=0U;({struct Cyc___cycFILE*_tmp54A=out_file;struct _fat_ptr _tmp549=({const char*_tmp2A2="#endif\n";_tag_fat(_tmp2A2,sizeof(char),8U);});Cyc_fprintf(_tmp54A,_tmp549,_tag_fat(_tmp2A1,sizeof(void*),0));});});
if(Cyc_do_setjmp){int _tmp2A3=0;_npop_handler(0);return _tmp2A3;}else{
Cyc_fclose(out_file);}
# 1782
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2A4=cstubs;struct Cyc_List_List*x=_tmp2A4;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp2A5=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp22=_tmp2A5;struct _tuple15*_tmp2A6=_stmttmp22;struct _fat_ptr _tmp2A8;struct _fat_ptr _tmp2A7;_tmp2A7=_tmp2A6->f1;_tmp2A8=_tmp2A6->f2;{struct _fat_ptr symbol=_tmp2A7;struct _fat_ptr text=_tmp2A8;
if((char*)text.curr != (char*)(_tag_fat(0,0,0)).curr &&(
(char*)symbol.curr == (char*)(_tag_fat(0,0,0)).curr ||({int(*_tmp54C)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp54B=defined_symbols;_tmp54C(_tmp54B,({struct _fat_ptr*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));*_tmp2A9=symbol;_tmp2A9;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}}
# 1793
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
if(cycstubs != 0){
# 1798
({struct Cyc_String_pa_PrintArg_struct _tmp2AC=({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0,({struct _fat_ptr _tmp54D=(struct _fat_ptr)({const char*_tmp2AD=filename;_tag_fat(_tmp2AD,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AD,1U));});_tmp3D1.f1=_tmp54D;});_tmp3D1;});void*_tmp2AA[1];_tmp2AA[0]=& _tmp2AC;({struct Cyc___cycFILE*_tmp54F=out_file;struct _fat_ptr _tmp54E=({const char*_tmp2AB="#include <%s>\n\n";_tag_fat(_tmp2AB,sizeof(char),16U);});Cyc_fprintf(_tmp54F,_tmp54E,_tag_fat(_tmp2AA,sizeof(void*),1));});});
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2AE=cycstubs;struct Cyc_List_List*x=_tmp2AE;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp2AF=(struct _tuple15*)x->hd;struct _tuple15*_stmttmp23=_tmp2AF;struct _tuple15*_tmp2B0=_stmttmp23;struct _fat_ptr _tmp2B2;struct _fat_ptr _tmp2B1;_tmp2B1=_tmp2B0->f1;_tmp2B2=_tmp2B0->f2;{struct _fat_ptr symbol=_tmp2B1;struct _fat_ptr text=_tmp2B2;
if((char*)text.curr != (char*)(_tag_fat(0,0,0)).curr &&(
(char*)symbol.curr == (char*)(_tag_fat(0,0,0)).curr ||({int(*_tmp551)(struct Cyc_Set_Set*,struct _fat_ptr*)=({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;});struct Cyc_Set_Set*_tmp550=defined_symbols;_tmp551(_tmp550,({struct _fat_ptr*_tmp2B3=_cycalloc(sizeof(*_tmp2B3));*_tmp2B3=symbol;_tmp2B3;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}
# 1806
({void*_tmp2B4=0U;({struct Cyc___cycFILE*_tmp553=out_file;struct _fat_ptr _tmp552=({const char*_tmp2B5="\n";_tag_fat(_tmp2B5,sizeof(char),2U);});Cyc_fprintf(_tmp553,_tmp552,_tag_fat(_tmp2B4,sizeof(void*),0));});});}{
# 1809
int _tmp2B6=0;_npop_handler(0);return _tmp2B6;}}}}}}}}}
# 1439
;_pop_handler();}else{void*_tmp1B2=(void*)Cyc_Core_get_exn_thrown();void*_tmp2BB=_tmp1B2;void*_tmp2BC;struct _fat_ptr _tmp2BD;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2BB)->tag == Cyc_Core_Impossible){_tmp2BD=((struct Cyc_Core_Impossible_exn_struct*)_tmp2BB)->f1;{struct _fat_ptr s=_tmp2BD;
# 1813
({struct Cyc_String_pa_PrintArg_struct _tmp2C0=({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0,_tmp3D3.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D3;});void*_tmp2BE[1];_tmp2BE[0]=& _tmp2C0;({struct Cyc___cycFILE*_tmp555=Cyc_stderr;struct _fat_ptr _tmp554=({const char*_tmp2BF="Got Core::Impossible(%s)\n";_tag_fat(_tmp2BF,sizeof(char),26U);});Cyc_fprintf(_tmp555,_tmp554,_tag_fat(_tmp2BE,sizeof(void*),1));});});goto _LL86;}}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2BB)->tag == Cyc_Dict_Absent){
# 1815
({void*_tmp2C1=0U;({struct Cyc___cycFILE*_tmp557=Cyc_stderr;struct _fat_ptr _tmp556=({const char*_tmp2C2="Got Dict::Absent\n";_tag_fat(_tmp2C2,sizeof(char),18U);});Cyc_fprintf(_tmp557,_tmp556,_tag_fat(_tmp2C1,sizeof(void*),0));});});goto _LL86;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2BB)->tag == Cyc_Core_Failure){_tmp2BD=((struct Cyc_Core_Failure_exn_struct*)_tmp2BB)->f1;{struct _fat_ptr s=_tmp2BD;
# 1817
({struct Cyc_String_pa_PrintArg_struct _tmp2C5=({struct Cyc_String_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=0,_tmp3D4.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D4;});void*_tmp2C3[1];_tmp2C3[0]=& _tmp2C5;({struct Cyc___cycFILE*_tmp559=Cyc_stderr;struct _fat_ptr _tmp558=({const char*_tmp2C4="Got Core::Failure(%s)\n";_tag_fat(_tmp2C4,sizeof(char),23U);});Cyc_fprintf(_tmp559,_tmp558,_tag_fat(_tmp2C3,sizeof(void*),1));});});goto _LL86;}}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2BB)->tag == Cyc_Core_Invalid_argument){_tmp2BD=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2BB)->f1;{struct _fat_ptr s=_tmp2BD;
# 1819
({struct Cyc_String_pa_PrintArg_struct _tmp2C8=({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0,_tmp3D5.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D5;});void*_tmp2C6[1];_tmp2C6[0]=& _tmp2C8;({struct Cyc___cycFILE*_tmp55B=Cyc_stderr;struct _fat_ptr _tmp55A=({const char*_tmp2C7="Got Invalid_argument(%s)\n";_tag_fat(_tmp2C7,sizeof(char),26U);});Cyc_fprintf(_tmp55B,_tmp55A,_tag_fat(_tmp2C6,sizeof(void*),1));});});goto _LL86;}}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2BB)->tag == Cyc_Core_Not_found){
# 1821
({void*_tmp2C9=0U;({struct Cyc___cycFILE*_tmp55D=Cyc_stderr;struct _fat_ptr _tmp55C=({const char*_tmp2CA="Got Not_found\n";_tag_fat(_tmp2CA,sizeof(char),15U);});Cyc_fprintf(_tmp55D,_tmp55C,_tag_fat(_tmp2C9,sizeof(void*),0));});});goto _LL86;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2BB)->tag == Cyc_NO_SUPPORT){_tmp2BD=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2BB)->f1;{struct _fat_ptr s=_tmp2BD;
# 1823
({struct Cyc_String_pa_PrintArg_struct _tmp2CD=({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0,_tmp3D6.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3D6;});void*_tmp2CB[1];_tmp2CB[0]=& _tmp2CD;({struct Cyc___cycFILE*_tmp55F=Cyc_stderr;struct _fat_ptr _tmp55E=({const char*_tmp2CC="No support because %s\n";_tag_fat(_tmp2CC,sizeof(char),23U);});Cyc_fprintf(_tmp55F,_tmp55E,_tag_fat(_tmp2CB,sizeof(void*),1));});});goto _LL86;}}else{_tmp2BC=_tmp2BB;{void*x=_tmp2BC;
# 1825
({void*_tmp2CE=0U;({struct Cyc___cycFILE*_tmp561=Cyc_stderr;struct _fat_ptr _tmp560=({const char*_tmp2CF="Got unknown exception\n";_tag_fat(_tmp2CF,sizeof(char),23U);});Cyc_fprintf(_tmp561,_tmp560,_tag_fat(_tmp2CE,sizeof(void*),0));});});
Cyc_Core_rethrow(x);}}}}}}}_LL86:;}}}
# 1831
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2D2=({struct Cyc_String_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=0,({struct _fat_ptr _tmp562=(struct _fat_ptr)({const char*_tmp2D3=filename;_tag_fat(_tmp2D3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D3,1U));});_tmp3D7.f1=_tmp562;});_tmp3D7;});void*_tmp2D0[1];_tmp2D0[0]=& _tmp2D2;({struct Cyc___cycFILE*_tmp564=Cyc_stderr;struct _fat_ptr _tmp563=({const char*_tmp2D1="Error: could not create file %s\n";_tag_fat(_tmp2D1,sizeof(char),33U);});Cyc_fprintf(_tmp564,_tmp563,_tag_fat(_tmp2D0,sizeof(void*),1));});});
return 1;}
# 1836
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp2D6=({struct Cyc_String_pa_PrintArg_struct _tmp3D8;_tmp3D8.tag=0,({
# 1839
struct _fat_ptr _tmp565=(struct _fat_ptr)({const char*_tmp2D7=filename;_tag_fat(_tmp2D7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D7,1U));});_tmp3D8.f1=_tmp565;});_tmp3D8;});void*_tmp2D4[1];_tmp2D4[0]=& _tmp2D6;({struct Cyc___cycFILE*_tmp567=out_file;struct _fat_ptr _tmp566=({const char*_tmp2D5="#error -- %s is not supported on this platform\n";_tag_fat(_tmp2D5,sizeof(char),48U);});Cyc_fprintf(_tmp567,_tmp566,_tag_fat(_tmp2D4,sizeof(void*),1));});});
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp2DA=({struct Cyc_String_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=0,({
struct _fat_ptr _tmp568=(struct _fat_ptr)({const char*_tmp2DB=filename;_tag_fat(_tmp2DB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2DB,1U));});_tmp3D9.f1=_tmp568;});_tmp3D9;});void*_tmp2D8[1];_tmp2D8[0]=& _tmp2DA;({struct Cyc___cycFILE*_tmp56A=Cyc_stderr;struct _fat_ptr _tmp569=({const char*_tmp2D9="Warning: %s will not be supported on this platform\n";_tag_fat(_tmp2D9,sizeof(char),52U);});Cyc_fprintf(_tmp56A,_tmp569,_tag_fat(_tmp2D8,sizeof(void*),1));});});
({void*_tmp2DC=0U;({struct _fat_ptr _tmp56B=({const char*_tmp2DD="Not supported on this platform\n";_tag_fat(_tmp2DD,sizeof(char),32U);});Cyc_log(_tmp56B,_tag_fat(_tmp2DC,sizeof(void*),0));});});
# 1850
return 0;}}
# 1854
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2F5=({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0,({struct _fat_ptr _tmp56C=(struct _fat_ptr)({const char*_tmp2F6=file;_tag_fat(_tmp2F6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2F6,1U));});_tmp3E0.f1=_tmp56C;});_tmp3E0;});void*_tmp2F3[1];_tmp2F3[0]=& _tmp2F5;({struct Cyc___cycFILE*_tmp56E=Cyc_stderr;struct _fat_ptr _tmp56D=({const char*_tmp2F4="Processing %s\n";_tag_fat(_tmp2F4,sizeof(char),15U);});Cyc_fprintf(_tmp56E,_tmp56D,_tag_fat(_tmp2F3,sizeof(void*),1));});});{
struct Cyc___cycFILE*_tmp2F7=Cyc_fopen(file,"r");struct Cyc___cycFILE*maybe=_tmp2F7;
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp2FA=({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0,({struct _fat_ptr _tmp56F=(struct _fat_ptr)({const char*_tmp2FB=file;_tag_fat(_tmp2FB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2FB,1U));});_tmp3E1.f1=_tmp56F;});_tmp3E1;});void*_tmp2F8[1];_tmp2F8[0]=& _tmp2FA;({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _fat_ptr _tmp570=({const char*_tmp2F9="Error: could not open %s\n";_tag_fat(_tmp2F9,sizeof(char),26U);});Cyc_fprintf(_tmp571,_tmp570,_tag_fat(_tmp2F8,sizeof(void*),1));});});
return 1;}{
# 1862
struct Cyc___cycFILE*_tmp2FC=maybe;struct Cyc___cycFILE*in_file=_tmp2FC;
# 1866
struct _fat_ptr buf=({char*_tmp31A=({unsigned _tmp319=1024U + 1U;char*_tmp318=_cycalloc_atomic(_check_times(_tmp319,sizeof(char)));({{unsigned _tmp3E7=1024U;unsigned i;for(i=0;i < _tmp3E7;++ i){_tmp318[i]='\000';}_tmp318[_tmp3E7]=0;}0;});_tmp318;});_tag_fat(_tmp31A,sizeof(char),1025U);});
struct _fat_ptr _tmp2FD=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp2FD;
if((int)Cyc_mode != 2){
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp300=({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0,({struct _fat_ptr _tmp572=(struct _fat_ptr)({const char*_tmp301=dir;_tag_fat(_tmp301,sizeof(char),_get_zero_arr_size_char((void*)_tmp301,1U));});_tmp3E2.f1=_tmp572;});_tmp3E2;});void*_tmp2FE[1];_tmp2FE[0]=& _tmp300;({struct Cyc___cycFILE*_tmp574=Cyc_stderr;struct _fat_ptr _tmp573=({const char*_tmp2FF="Error: can't change directory to %s\n";_tag_fat(_tmp2FF,sizeof(char),37U);});Cyc_fprintf(_tmp574,_tmp573,_tag_fat(_tmp2FE,sizeof(void*),1));});});
return 1;}}
# 1874
if((int)Cyc_mode == 1){
# 1876
struct _fat_ptr _tmp302=({struct Cyc_String_pa_PrintArg_struct _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp3E5;_tmp3E5.tag=0,_tmp3E5.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3E5;});struct Cyc_String_pa_PrintArg_struct _tmp309=({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0,_tmp3E4.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3E4;});void*_tmp306[2];_tmp306[0]=& _tmp308,_tmp306[1]=& _tmp309;({struct _fat_ptr _tmp575=({const char*_tmp307="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp307,sizeof(char),39U);});Cyc_aprintf(_tmp575,_tag_fat(_tmp306,sizeof(void*),2));});});struct _fat_ptr cmd=_tmp302;
# 1878
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp305=({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0,_tmp3E3.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp3E3;});void*_tmp303[1];_tmp303[0]=& _tmp305;({struct Cyc___cycFILE*_tmp577=Cyc_stderr;struct _fat_ptr _tmp576=({const char*_tmp304="%s\n";_tag_fat(_tmp304,sizeof(char),4U);});Cyc_fprintf(_tmp577,_tmp576,_tag_fat(_tmp303,sizeof(void*),1));});});
system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)));}{
# 1883
struct Cyc_Lexing_lexbuf*_tmp30A=Cyc_Lexing_from_file(in_file);struct Cyc_Lexing_lexbuf*l=_tmp30A;
struct _tuple17*entry;
while((entry=({(struct _tuple17*(*)(struct Cyc_Lexing_lexbuf*))Cyc_spec;})(l))!= 0){
struct _tuple17*_tmp30B=(struct _tuple17*)_check_null(entry);struct _tuple17*_stmttmp24=_tmp30B;struct _tuple17*_tmp30C=_stmttmp24;void*_tmp314;void*_tmp313;void*_tmp312;void*_tmp311;void*_tmp310;void*_tmp30F;void*_tmp30E;struct _fat_ptr _tmp30D;_tmp30D=_tmp30C->f1;_tmp30E=_tmp30C->f2;_tmp30F=_tmp30C->f3;_tmp310=_tmp30C->f4;_tmp311=_tmp30C->f5;_tmp312=_tmp30C->f6;_tmp313=_tmp30C->f7;_tmp314=_tmp30C->f8;{struct _fat_ptr headerfile=_tmp30D;struct Cyc_List_List*start_symbols=_tmp30E;struct Cyc_List_List*user_defs=_tmp30F;struct Cyc_List_List*omit_symbols=_tmp310;struct Cyc_List_List*hstubs=_tmp311;struct Cyc_List_List*cstubs=_tmp312;struct Cyc_List_List*cycstubs=_tmp313;struct Cyc_List_List*cpp_insert=_tmp314;
# 1888
if(Cyc_process_file((const char*)_check_null(_untag_fat_ptr(headerfile,sizeof(char),1U)),start_symbols,user_defs,omit_symbols,hstubs,cstubs,cycstubs,cpp_insert))
# 1890
return 1;}}
# 1892
Cyc_fclose(in_file);
# 1894
if((int)Cyc_mode != 2){
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp317=({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0,_tmp3E6.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp3E6;});void*_tmp315[1];_tmp315[0]=& _tmp317;({struct Cyc___cycFILE*_tmp579=Cyc_stderr;struct _fat_ptr _tmp578=({const char*_tmp316="Error: could not change directory to %s\n";_tag_fat(_tmp316,sizeof(char),41U);});Cyc_fprintf(_tmp579,_tmp578,_tag_fat(_tmp315,sizeof(void*),1));});});
return 1;}}
# 1900
return 0;}}}}
# 1904
int Cyc_process_setjmp(const char*dir){
# 1907
struct _fat_ptr buf=({char*_tmp32C=({unsigned _tmp32B=1024U + 1U;char*_tmp32A=_cycalloc_atomic(_check_times(_tmp32B,sizeof(char)));({{unsigned _tmp3EA=1024U;unsigned i;for(i=0;i < _tmp3EA;++ i){_tmp32A[i]='\000';}_tmp32A[_tmp3EA]=0;}0;});_tmp32A;});_tag_fat(_tmp32C,sizeof(char),1025U);});
struct _fat_ptr _tmp31B=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));struct _fat_ptr cwd=_tmp31B;
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp3E8;_tmp3E8.tag=0,({struct _fat_ptr _tmp57A=(struct _fat_ptr)({const char*_tmp31F=dir;_tag_fat(_tmp31F,sizeof(char),_get_zero_arr_size_char((void*)_tmp31F,1U));});_tmp3E8.f1=_tmp57A;});_tmp3E8;});void*_tmp31C[1];_tmp31C[0]=& _tmp31E;({struct Cyc___cycFILE*_tmp57C=Cyc_stderr;struct _fat_ptr _tmp57B=({const char*_tmp31D="Error: can't change directory to %s\n";_tag_fat(_tmp31D,sizeof(char),37U);});Cyc_fprintf(_tmp57C,_tmp57B,_tag_fat(_tmp31C,sizeof(void*),1));});});
return 1;}
# 1913
if(({struct Cyc_List_List*_tmp582=({struct _fat_ptr*_tmp320[1];({struct _fat_ptr*_tmp57E=({struct _fat_ptr*_tmp322=_cycalloc(sizeof(*_tmp322));({struct _fat_ptr _tmp57D=({const char*_tmp321="jmp_buf";_tag_fat(_tmp321,sizeof(char),8U);});*_tmp322=_tmp57D;});_tmp322;});_tmp320[0]=_tmp57E;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp320,sizeof(struct _fat_ptr*),1));});Cyc_process_file("setjmp.h",_tmp582,0,0,({struct _tuple15*_tmp323[1];({struct _tuple15*_tmp581=({struct _tuple15*_tmp326=_cycalloc(sizeof(*_tmp326));
({struct _fat_ptr _tmp580=({const char*_tmp324="setjmp";_tag_fat(_tmp324,sizeof(char),7U);});_tmp326->f1=_tmp580;}),({struct _fat_ptr _tmp57F=({const char*_tmp325="extern int setjmp(jmp_buf);\n";_tag_fat(_tmp325,sizeof(char),29U);});_tmp326->f2=_tmp57F;});_tmp326;});_tmp323[0]=_tmp581;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp323,sizeof(struct _tuple15*),1));}),0,0,0);}))
# 1916
return 1;
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp329=({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0,_tmp3E9.f1=(struct _fat_ptr)((struct _fat_ptr)cwd);_tmp3E9;});void*_tmp327[1];_tmp327[0]=& _tmp329;({struct Cyc___cycFILE*_tmp584=Cyc_stderr;struct _fat_ptr _tmp583=({const char*_tmp328="Error: could not change directory to %s\n";_tag_fat(_tmp328,sizeof(char),41U);});Cyc_fprintf(_tmp584,_tmp583,_tag_fat(_tmp327,sizeof(void*),1));});});
return 1;}
# 1921
return 0;}static char _tmp32D[13U]="BUILDLIB.OUT";
# 1925
static struct _fat_ptr Cyc_output_dir={_tmp32D,_tmp32D,_tmp32D + 13U};
static void Cyc_set_output_dir(struct _fat_ptr s){
Cyc_output_dir=s;}
# 1929
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _fat_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->hd=(const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)),_tmp32E->tl=Cyc_spec_files;_tmp32E;});}
# 1933
static int Cyc_no_other(struct _fat_ptr s){return 0;}
static void Cyc_set_GATHER (void){
Cyc_mode=1U;}
# 1937
static void Cyc_set_GATHERSCRIPT (void){
Cyc_mode=2U;}
# 1940
static void Cyc_set_FINISH (void){
Cyc_mode=3U;}
# 1943
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp330=_cycalloc(sizeof(*_tmp330));({struct _fat_ptr*_tmp585=({struct _fat_ptr*_tmp32F=_cycalloc(sizeof(*_tmp32F));*_tmp32F=s;_tmp32F;});_tmp330->hd=_tmp585;}),_tmp330->tl=Cyc_cppargs;_tmp330;});}
# 1946
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp333=({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0,_tmp3EB.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp3EB;});void*_tmp331[1];_tmp331[0]=& _tmp333;({struct Cyc___cycFILE*_tmp587=Cyc_stderr;struct _fat_ptr _tmp586=({const char*_tmp332="Unsupported option %s\n";_tag_fat(_tmp332,sizeof(char),23U);});Cyc_fprintf(_tmp587,_tmp586,_tag_fat(_tmp331,sizeof(void*),1));});});
Cyc_badparse=1;}
# 1955
extern void GC_blacklist_warn_clear (void);struct _tuple19{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
int Cyc_main(int argc,struct _fat_ptr argv){
GC_blacklist_warn_clear();{
# 1959
struct Cyc_List_List*options=({struct _tuple19*_tmp36B[9];({
struct _tuple19*_tmp5B4=({struct _tuple19*_tmp370=_cycalloc(sizeof(*_tmp370));({struct _fat_ptr _tmp5B3=({const char*_tmp36C="-d";_tag_fat(_tmp36C,sizeof(char),3U);});_tmp370->f1=_tmp5B3;}),_tmp370->f2=0,({struct _fat_ptr _tmp5B2=({const char*_tmp36D=" <file>";_tag_fat(_tmp36D,sizeof(char),8U);});_tmp370->f3=_tmp5B2;}),({
void*_tmp5B1=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->tag=5,_tmp36E->f1=Cyc_set_output_dir;_tmp36E;});_tmp370->f4=_tmp5B1;}),({
struct _fat_ptr _tmp5B0=({const char*_tmp36F="Set the output directory to <file>";_tag_fat(_tmp36F,sizeof(char),35U);});_tmp370->f5=_tmp5B0;});_tmp370;});
# 1960
_tmp36B[0]=_tmp5B4;}),({
# 1963
struct _tuple19*_tmp5AF=({struct _tuple19*_tmp375=_cycalloc(sizeof(*_tmp375));({struct _fat_ptr _tmp5AE=({const char*_tmp371="-gather";_tag_fat(_tmp371,sizeof(char),8U);});_tmp375->f1=_tmp5AE;}),_tmp375->f2=0,({struct _fat_ptr _tmp5AD=({const char*_tmp372="";_tag_fat(_tmp372,sizeof(char),1U);});_tmp375->f3=_tmp5AD;}),({
void*_tmp5AC=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->tag=0,_tmp373->f1=Cyc_set_GATHER;_tmp373;});_tmp375->f4=_tmp5AC;}),({
struct _fat_ptr _tmp5AB=({const char*_tmp374="Gather C library info but don't produce Cyclone headers";_tag_fat(_tmp374,sizeof(char),56U);});_tmp375->f5=_tmp5AB;});_tmp375;});
# 1963
_tmp36B[1]=_tmp5AF;}),({
# 1966
struct _tuple19*_tmp5AA=({struct _tuple19*_tmp37A=_cycalloc(sizeof(*_tmp37A));({struct _fat_ptr _tmp5A9=({const char*_tmp376="-gatherscript";_tag_fat(_tmp376,sizeof(char),14U);});_tmp37A->f1=_tmp5A9;}),_tmp37A->f2=0,({struct _fat_ptr _tmp5A8=({const char*_tmp377="";_tag_fat(_tmp377,sizeof(char),1U);});_tmp37A->f3=_tmp5A8;}),({
void*_tmp5A7=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp378=_cycalloc(sizeof(*_tmp378));_tmp378->tag=0,_tmp378->f1=Cyc_set_GATHERSCRIPT;_tmp378;});_tmp37A->f4=_tmp5A7;}),({
struct _fat_ptr _tmp5A6=({const char*_tmp379="Produce a script to gather C library info";_tag_fat(_tmp379,sizeof(char),42U);});_tmp37A->f5=_tmp5A6;});_tmp37A;});
# 1966
_tmp36B[2]=_tmp5AA;}),({
# 1969
struct _tuple19*_tmp5A5=({struct _tuple19*_tmp37F=_cycalloc(sizeof(*_tmp37F));({struct _fat_ptr _tmp5A4=({const char*_tmp37B="-finish";_tag_fat(_tmp37B,sizeof(char),8U);});_tmp37F->f1=_tmp5A4;}),_tmp37F->f2=0,({struct _fat_ptr _tmp5A3=({const char*_tmp37C="";_tag_fat(_tmp37C,sizeof(char),1U);});_tmp37F->f3=_tmp5A3;}),({
void*_tmp5A2=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->tag=0,_tmp37D->f1=Cyc_set_FINISH;_tmp37D;});_tmp37F->f4=_tmp5A2;}),({
struct _fat_ptr _tmp5A1=({const char*_tmp37E="Produce Cyclone headers from pre-gathered C library info";_tag_fat(_tmp37E,sizeof(char),57U);});_tmp37F->f5=_tmp5A1;});_tmp37F;});
# 1969
_tmp36B[3]=_tmp5A5;}),({
# 1972
struct _tuple19*_tmp5A0=({struct _tuple19*_tmp384=_cycalloc(sizeof(*_tmp384));({struct _fat_ptr _tmp59F=({const char*_tmp380="-setjmp";_tag_fat(_tmp380,sizeof(char),8U);});_tmp384->f1=_tmp59F;}),_tmp384->f2=0,({struct _fat_ptr _tmp59E=({const char*_tmp381="";_tag_fat(_tmp381,sizeof(char),1U);});_tmp384->f3=_tmp59E;}),({
void*_tmp59D=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp382=_cycalloc(sizeof(*_tmp382));_tmp382->tag=3,_tmp382->f1=& Cyc_do_setjmp;_tmp382;});_tmp384->f4=_tmp59D;}),({
# 1977
struct _fat_ptr _tmp59C=({const char*_tmp383="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_fat(_tmp383,sizeof(char),186U);});_tmp384->f5=_tmp59C;});_tmp384;});
# 1972
_tmp36B[4]=_tmp5A0;}),({
# 1978
struct _tuple19*_tmp59B=({struct _tuple19*_tmp389=_cycalloc(sizeof(*_tmp389));({struct _fat_ptr _tmp59A=({const char*_tmp385="-b";_tag_fat(_tmp385,sizeof(char),3U);});_tmp389->f1=_tmp59A;}),_tmp389->f2=0,({struct _fat_ptr _tmp599=({const char*_tmp386=" <machine>";_tag_fat(_tmp386,sizeof(char),11U);});_tmp389->f3=_tmp599;}),({
void*_tmp598=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->tag=5,_tmp387->f1=Cyc_Specsfile_set_target_arch;_tmp387;});_tmp389->f4=_tmp598;}),({
struct _fat_ptr _tmp597=({const char*_tmp388="Set the target machine for compilation to <machine>";_tag_fat(_tmp388,sizeof(char),52U);});_tmp389->f5=_tmp597;});_tmp389;});
# 1978
_tmp36B[5]=_tmp59B;}),({
# 1981
struct _tuple19*_tmp596=({struct _tuple19*_tmp38E=_cycalloc(sizeof(*_tmp38E));({struct _fat_ptr _tmp595=({const char*_tmp38A="-B";_tag_fat(_tmp38A,sizeof(char),3U);});_tmp38E->f1=_tmp595;}),_tmp38E->f2=1,({struct _fat_ptr _tmp594=({const char*_tmp38B="<file>";_tag_fat(_tmp38B,sizeof(char),7U);});_tmp38E->f3=_tmp594;}),({
void*_tmp593=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp38C=_cycalloc(sizeof(*_tmp38C));_tmp38C->tag=1,_tmp38C->f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp38C;});_tmp38E->f4=_tmp593;}),({
struct _fat_ptr _tmp592=({const char*_tmp38D="Add to the list of directories to search for compiler files";_tag_fat(_tmp38D,sizeof(char),60U);});_tmp38E->f5=_tmp592;});_tmp38E;});
# 1981
_tmp36B[6]=_tmp596;}),({
# 1984
struct _tuple19*_tmp591=({struct _tuple19*_tmp393=_cycalloc(sizeof(*_tmp393));({struct _fat_ptr _tmp590=({const char*_tmp38F="-v";_tag_fat(_tmp38F,sizeof(char),3U);});_tmp393->f1=_tmp590;}),_tmp393->f2=0,({struct _fat_ptr _tmp58F=({const char*_tmp390="";_tag_fat(_tmp390,sizeof(char),1U);});_tmp393->f3=_tmp58F;}),({
void*_tmp58E=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->tag=3,_tmp391->f1=& Cyc_verbose;_tmp391;});_tmp393->f4=_tmp58E;}),({
struct _fat_ptr _tmp58D=({const char*_tmp392="Verbose operation";_tag_fat(_tmp392,sizeof(char),18U);});_tmp393->f5=_tmp58D;});_tmp393;});
# 1984
_tmp36B[7]=_tmp591;}),({
# 1987
struct _tuple19*_tmp58C=({struct _tuple19*_tmp398=_cycalloc(sizeof(*_tmp398));({struct _fat_ptr _tmp58B=({const char*_tmp394="-";_tag_fat(_tmp394,sizeof(char),2U);});_tmp398->f1=_tmp58B;}),_tmp398->f2=1,({struct _fat_ptr _tmp58A=({const char*_tmp395="";_tag_fat(_tmp395,sizeof(char),1U);});_tmp398->f3=_tmp58A;}),({
void*_tmp589=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->tag=1,_tmp396->f1=Cyc_add_cpparg;_tmp396;});_tmp398->f4=_tmp589;}),({
struct _fat_ptr _tmp588=({const char*_tmp397="";_tag_fat(_tmp397,sizeof(char),1U);});_tmp398->f5=_tmp588;});_tmp398;});
# 1987
_tmp36B[8]=_tmp58C;});({
# 1960
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp36B,sizeof(struct _tuple19*),9));});
# 1992
struct _fat_ptr _tmp334=({struct Cyc_List_List*_tmp5B6=options;struct _fat_ptr _tmp5B5=({const char*_tmp36A="Options:";_tag_fat(_tmp36A,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp5B6,Cyc_add_spec_file,Cyc_no_other,_tmp5B5,argv);});struct _fat_ptr otherargs=_tmp334;
# 1994
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp5B8=options;struct _fat_ptr _tmp5B7=({const char*_tmp335="Options:";_tag_fat(_tmp335,sizeof(char),9U);});Cyc_Arg_parse(_tmp5B8,Cyc_add_spec_file,Cyc_no_other,_tmp5B7,otherargs);});
if((((Cyc_badparse ||
 !Cyc_do_setjmp && Cyc_spec_files == 0)||
 Cyc_do_setjmp && Cyc_spec_files != 0)||
 Cyc_do_setjmp &&(int)Cyc_mode == 1)||
 Cyc_do_setjmp &&(int)Cyc_mode == 2){
({struct Cyc_List_List*_tmp5B9=options;Cyc_Arg_usage(_tmp5B9,({const char*_tmp336="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_fat(_tmp336,sizeof(char),59U);}));});
# 2004
return 1;}{
# 2010
struct _fat_ptr _tmp337=Cyc_Specsfile_find_in_arch_path(({const char*_tmp369="cycspecs";_tag_fat(_tmp369,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp337;
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp33A=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0,_tmp3EC.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp3EC;});void*_tmp338[1];_tmp338[0]=& _tmp33A;({struct Cyc___cycFILE*_tmp5BB=Cyc_stderr;struct _fat_ptr _tmp5BA=({const char*_tmp339="Reading from specs file %s\n";_tag_fat(_tmp339,sizeof(char),28U);});Cyc_fprintf(_tmp5BB,_tmp5BA,_tag_fat(_tmp338,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp33B=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp33B;
Cyc_target_cflags=({struct Cyc_List_List*_tmp5BC=specs;Cyc_Specsfile_get_spec(_tmp5BC,({const char*_tmp33C="cyclone_target_cflags";_tag_fat(_tmp33C,sizeof(char),22U);}));});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp33F=({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0,_tmp3ED.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3ED;});void*_tmp33D[1];_tmp33D[0]=& _tmp33F;({struct Cyc___cycFILE*_tmp5BE=Cyc_stderr;struct _fat_ptr _tmp5BD=({const char*_tmp33E="Target cflags are %s\n";_tag_fat(_tmp33E,sizeof(char),22U);});Cyc_fprintf(_tmp5BE,_tmp5BD,_tag_fat(_tmp33D,sizeof(void*),1));});});
Cyc_cyclone_cc=({struct Cyc_List_List*_tmp5BF=specs;Cyc_Specsfile_get_spec(_tmp5BF,({const char*_tmp340="cyclone_cc";_tag_fat(_tmp340,sizeof(char),11U);}));});
if(!((unsigned)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp341="gcc";_tag_fat(_tmp341,sizeof(char),4U);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp344=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0,_tmp3EE.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_cc);_tmp3EE;});void*_tmp342[1];_tmp342[0]=& _tmp344;({struct Cyc___cycFILE*_tmp5C1=Cyc_stderr;struct _fat_ptr _tmp5C0=({const char*_tmp343="C compiler is %s\n";_tag_fat(_tmp343,sizeof(char),18U);});Cyc_fprintf(_tmp5C1,_tmp5C0,_tag_fat(_tmp342,sizeof(void*),1));});});
# 2019
if((int)Cyc_mode == 2){
if(Cyc_verbose)
({void*_tmp345=0U;({struct Cyc___cycFILE*_tmp5C3=Cyc_stderr;struct _fat_ptr _tmp5C2=({const char*_tmp346="Creating BUILDLIB.sh\n";_tag_fat(_tmp346,sizeof(char),22U);});Cyc_fprintf(_tmp5C3,_tmp5C2,_tag_fat(_tmp345,sizeof(void*),0));});});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned)Cyc_script_file)){
({void*_tmp347=0U;({struct Cyc___cycFILE*_tmp5C5=Cyc_stderr;struct _fat_ptr _tmp5C4=({const char*_tmp348="Could not create file BUILDLIB.sh\n";_tag_fat(_tmp348,sizeof(char),35U);});Cyc_fprintf(_tmp5C5,_tmp5C4,_tag_fat(_tmp347,sizeof(void*),0));});});
exit(1);}
# 2027
({void*_tmp349=0U;({struct _fat_ptr _tmp5C6=({const char*_tmp34A="#!/bin/sh\n";_tag_fat(_tmp34A,sizeof(char),11U);});Cyc_prscript(_tmp5C6,_tag_fat(_tmp349,sizeof(void*),0));});});
({void*_tmp34B=0U;({struct _fat_ptr _tmp5C7=({const char*_tmp34C="GCC=\"gcc\"\n";_tag_fat(_tmp34C,sizeof(char),11U);});Cyc_prscript(_tmp5C7,_tag_fat(_tmp34B,sizeof(void*),0));});});}
# 2032
if(Cyc_force_directory_prefixes(Cyc_output_dir)|| Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp34F=({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0,_tmp3EF.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3EF;});void*_tmp34D[1];_tmp34D[0]=& _tmp34F;({struct Cyc___cycFILE*_tmp5C9=Cyc_stderr;struct _fat_ptr _tmp5C8=({const char*_tmp34E="Error: could not create directory %s\n";_tag_fat(_tmp34E,sizeof(char),38U);});Cyc_fprintf(_tmp5C9,_tmp5C8,_tag_fat(_tmp34D,sizeof(void*),1));});});
return 1;}
# 2036
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp352=({struct Cyc_String_pa_PrintArg_struct _tmp3F0;_tmp3F0.tag=0,_tmp3F0.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F0;});void*_tmp350[1];_tmp350[0]=& _tmp352;({struct Cyc___cycFILE*_tmp5CB=Cyc_stderr;struct _fat_ptr _tmp5CA=({const char*_tmp351="Output directory is %s\n";_tag_fat(_tmp351,sizeof(char),24U);});Cyc_fprintf(_tmp5CB,_tmp5CA,_tag_fat(_tmp350,sizeof(void*),1));});});
# 2039
if((int)Cyc_mode == 2){
({struct Cyc_String_pa_PrintArg_struct _tmp355=({struct Cyc_String_pa_PrintArg_struct _tmp3F1;_tmp3F1.tag=0,_tmp3F1.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F1;});void*_tmp353[1];_tmp353[0]=& _tmp355;({struct _fat_ptr _tmp5CC=({const char*_tmp354="cd %s\n";_tag_fat(_tmp354,sizeof(char),7U);});Cyc_prscript(_tmp5CC,_tag_fat(_tmp353,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp358=({struct Cyc_String_pa_PrintArg_struct _tmp3F2;_tmp3F2.tag=0,_tmp3F2.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_target_cflags);_tmp3F2;});void*_tmp356[1];_tmp356[0]=& _tmp358;({struct _fat_ptr _tmp5CD=({const char*_tmp357="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_fat(_tmp357,sizeof(char),41U);});Cyc_prscript(_tmp5CD,_tag_fat(_tmp356,sizeof(void*),1));});});}
# 2045
if(!Cyc_gathering()){
# 2048
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp5CE=Cyc_output_dir;Cyc_Filename_concat(_tmp5CE,({const char*_tmp359="BUILDLIB.LOG";_tag_fat(_tmp359,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp35C=({struct Cyc_String_pa_PrintArg_struct _tmp3F3;_tmp3F3.tag=0,_tmp3F3.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F3;});void*_tmp35A[1];_tmp35A[0]=& _tmp35C;({struct Cyc___cycFILE*_tmp5D0=Cyc_stderr;struct _fat_ptr _tmp5CF=({const char*_tmp35B="Error: could not create log file in directory %s\n";_tag_fat(_tmp35B,sizeof(char),50U);});Cyc_fprintf(_tmp5D0,_tmp5CF,_tag_fat(_tmp35A,sizeof(void*),1));});});
return 1;}
# 2054
if(!Cyc_do_setjmp){
# 2056
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp5D1=Cyc_output_dir;Cyc_Filename_concat(_tmp5D1,({const char*_tmp35D="cstubs.c";_tag_fat(_tmp35D,sizeof(char),9U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp360=({struct Cyc_String_pa_PrintArg_struct _tmp3F4;_tmp3F4.tag=0,_tmp3F4.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F4;});void*_tmp35E[1];_tmp35E[0]=& _tmp360;({struct Cyc___cycFILE*_tmp5D3=Cyc_stderr;struct _fat_ptr _tmp5D2=({const char*_tmp35F="Error: could not create cstubs.c in directory %s\n";_tag_fat(_tmp35F,sizeof(char),50U);});Cyc_fprintf(_tmp5D3,_tmp5D2,_tag_fat(_tmp35E,sizeof(void*),1));});});
return 1;}
# 2063
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmp5D4=Cyc_output_dir;Cyc_Filename_concat(_tmp5D4,({const char*_tmp361="cycstubs.cyc";_tag_fat(_tmp361,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp3F5;_tmp3F5.tag=0,_tmp3F5.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_output_dir);_tmp3F5;});void*_tmp362[1];_tmp362[0]=& _tmp364;({struct Cyc___cycFILE*_tmp5D6=Cyc_stderr;struct _fat_ptr _tmp5D5=({const char*_tmp363="Error: could not create cycstubs.c in directory %s\n";_tag_fat(_tmp363,sizeof(char),52U);});Cyc_fprintf(_tmp5D6,_tmp5D5,_tag_fat(_tmp362,sizeof(void*),1));});});
# 2068
return 1;}
# 2070
({void*_tmp365=0U;({struct Cyc___cycFILE*_tmp5D8=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _fat_ptr _tmp5D7=({const char*_tmp366="#include <core.h>\nusing Core;\n\n";_tag_fat(_tmp366,sizeof(char),32U);});Cyc_fprintf(_tmp5D8,_tmp5D7,_tag_fat(_tmp365,sizeof(void*),0));});});}}{
# 2077
const char*outdir=(const char*)_check_null(_untag_fat_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp && Cyc_process_setjmp(outdir))
return 1;else{
# 2083
for(1;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp367=0U;({struct Cyc___cycFILE*_tmp5DA=Cyc_stderr;struct _fat_ptr _tmp5D9=({const char*_tmp368="FATAL ERROR -- QUIT!\n";_tag_fat(_tmp368,sizeof(char),22U);});Cyc_fprintf(_tmp5DA,_tmp5D9,_tag_fat(_tmp367,sizeof(void*),0));});});
exit(1);}}}
# 2091
if((int)Cyc_mode == 2)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2094
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2102
return 0;}}}}}
