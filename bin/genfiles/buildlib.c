#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
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
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
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
void* _region_vmalloc(struct _RegionHandle*, unsigned);
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

void* Cyc_Core_get_exn_thrown();
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
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

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void *);
#endif
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

static inline unsigned int _check_times(unsigned x, unsigned y) {
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
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
extern int Cyc_vfprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
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
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
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
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
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
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
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
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 173
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 465 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};
# 504
enum Cyc_Absyn_MallocKind{Cyc_Absyn_Malloc =0U,Cyc_Absyn_Calloc =1U,Cyc_Absyn_Vmalloc =2U};struct Cyc_Absyn_MallocInfo{enum Cyc_Absyn_MallocKind mknd;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 1068 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*,unsigned);
# 1119
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*,struct _fat_ptr*);struct _tuple12{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1125
struct _tuple12 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1133
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
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
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
extern void Cyc_Lex_lex_init(int);static char _TmpG0[4U]="gcc";
# 87
static struct _fat_ptr Cyc_cyclone_cc={_TmpG0,_TmpG0,_TmpG0 + 4U};static char _TmpG1[1U]="";
static struct _fat_ptr Cyc_target_cflags={_TmpG1,_TmpG1,_TmpG1 + 1U};
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
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Internal error: log file is NULL\n";_tag_fat(_Tmp0,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);}{
# 104
int x=Cyc_vfprintf(_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush(_check_null(Cyc_log_file));
return x;}}
# 109
static struct _fat_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _fat_ptr*sptr){
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(*_check_null(Cyc_current_targets),sptr);*_Tmp0=_Tmp1;});_Tmp0;});}struct _tuple13{struct _fat_ptr*f1;struct Cyc_Set_Set*f2;};
# 117
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 123
struct Cyc___cycFILE*Cyc_slurp_out=0;
# 125
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
static struct Cyc_List_List*Cyc_cppargs=0;static char _TmpG2[14U]="BUILDLIB_sym_";
# 172
struct _fat_ptr Cyc_user_prefix={_TmpG2,_TmpG2,_TmpG2 + 14U};
static struct _fat_ptr*Cyc_add_user_prefix(struct _fat_ptr*symbol){
struct _fat_ptr s=Cyc_strconcat(Cyc_user_prefix,*symbol);
return({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));*_Tmp0=s;_Tmp0;});}
# 177
static struct _fat_ptr Cyc_remove_user_prefix(struct _fat_ptr symbol){
unsigned prefix_len=Cyc_strlen(Cyc_user_prefix);
if(Cyc_strncmp(symbol,Cyc_user_prefix,prefix_len)!= 0){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=symbol;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Internal error: bad user type name %s\n";_tag_fat(_Tmp2,sizeof(char),39U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return symbol;}
# 183
return({struct _fat_ptr _Tmp0=symbol;int _Tmp1=(int)prefix_len;Cyc_substring(_Tmp0,_Tmp1,({unsigned long _Tmp2=Cyc_strlen(symbol);_Tmp2 - prefix_len;}));});}
# 186
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_stmttmp0=d->r;void*_Tmp0=_stmttmp0;void*_Tmp1;switch(*((int*)_Tmp0)){case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
# 189
({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_remove_user_prefix(*(*ad->name).f2);*_Tmp3=_Tmp4;});_Tmp3;});(*ad->name).f2=_Tmp2;});goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
# 191
({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_remove_user_prefix(*(*ed->name).f2);*_Tmp3=_Tmp4;});_Tmp3;});(*ed->name).f2=_Tmp2;});goto _LL0;}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp1;
# 193
({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_remove_user_prefix(*(*td->name).f2);*_Tmp3=_Tmp4;});_Tmp3;});(*td->name).f2=_Tmp2;});goto _LL0;}case 1:
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
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error in .cys file: bad user-defined type definition\n";_tag_fat(_Tmp2,sizeof(char),54U);}),_tag_fat(0U,sizeof(void*),0));
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
({int _Tmp0=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_Tmp0;});
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
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="empty token";_tag_fat(_Tmp1,sizeof(char),12U);});_Tmp0;}));else{
# 259
return lbuf->lex_last_action;}}else{
# 262
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 266
struct _tuple13*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 225 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(1;Cyc_current_args != 0;Cyc_current_args=(_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp2=({struct Cyc_Set_Set*_Tmp3=*_check_null(Cyc_current_targets);({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_delete;})(_Tmp3,(struct _fat_ptr*)(_check_null(Cyc_current_args))->hd);});*_Tmp1=_Tmp2;});_Tmp1;});}
# 230
return({struct _tuple13*_Tmp1=_cycalloc(sizeof(struct _tuple13));_Tmp1->f1=_check_null(Cyc_current_source),_Tmp1->f2=*_check_null(Cyc_current_targets);_Tmp1;});case 1:
# 233 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2:
# 235
 return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);};}
# 239
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 241
struct _tuple13*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 239 "buildlib.cyl"
 Cyc_current_source=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp3,0,(unsigned long)(({
int _Tmp4=Cyc_Lexing_lexeme_end(lexbuf);_Tmp4 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 239
*_Tmp1=_Tmp2;});_Tmp1;});
# 241
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp2=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_Tmp1=_Tmp2;});_Tmp1;});
Cyc_token(lexbuf);
return 0;case 1:
# 247
 Cyc_current_source=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp3,0,(unsigned long)(({
int _Tmp4=Cyc_Lexing_lexeme_end(lexbuf);_Tmp4 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 247
*_Tmp1=_Tmp2;});_Tmp1;});
# 249
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp2=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_Tmp1=_Tmp2;});_Tmp1;});
Cyc_args(lexbuf);
return 0;case 2:
# 255
 Cyc_current_source=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);*_Tmp1=_Tmp2;});_Tmp1;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp2=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_Tmp1=_Tmp2;});_Tmp1;});
Cyc_token(lexbuf);
return 0;default:
# 261
((lexbuf->refill_buff))(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);};}
# 264
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 266
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:  {
# 264 "buildlib.cyl"
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp1=({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp2,0,(unsigned long)(({
int _Tmp3=Cyc_Lexing_lexeme_end(lexbuf);_Tmp3 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 264
*a=_Tmp1;});
# 266
Cyc_current_args=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=a,_Tmp1->tl=Cyc_current_args;_Tmp1;});
return Cyc_args(lexbuf);}case 1:  {
# 270
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp1=({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp2,0,(unsigned long)(({
int _Tmp3=Cyc_Lexing_lexeme_end(lexbuf);_Tmp3 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 270
*a=_Tmp1;});
# 272
Cyc_current_args=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=a,_Tmp1->tl=Cyc_current_args;_Tmp1;});
return Cyc_args(lexbuf);}case 2:  {
# 276
struct _fat_ptr*a;a=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp1=({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp2,0,(unsigned long)(({
int _Tmp3=Cyc_Lexing_lexeme_end(lexbuf);_Tmp3 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 276
*a=_Tmp1;});
# 278
Cyc_current_args=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=a,_Tmp1->tl=Cyc_current_args;_Tmp1;});
return Cyc_token(lexbuf);}default:
# 281
((lexbuf->refill_buff))(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);};}
# 284
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 286
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 285 "buildlib.cyl"
 Cyc_add_target(({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);*_Tmp1=_Tmp2;});_Tmp1;}));return Cyc_token(lexbuf);case 1:
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
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 396
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
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
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 415
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 418 "buildlib.cyl"
 return 0;case 1:
# 420
 Cyc_fputc(34,_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2:
# 427 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(({const char*_Tmp1="Warning: declaration of malloc sidestepped\n";_tag_fat(_Tmp1,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 3:
# 433 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(({const char*_Tmp1="Warning: declaration of malloc sidestepped\n";_tag_fat(_Tmp1,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 4:
# 439 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(({const char*_Tmp1="Warning: declaration of calloc sidestepped\n";_tag_fat(_Tmp1,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 5:
# 445 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",_check_null(Cyc_slurp_out));
Cyc_log(({const char*_Tmp1="Warning: declaration of calloc sidestepped\n";_tag_fat(_Tmp1,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 6:
# 449
 Cyc_fputs("__region",_check_null(Cyc_slurp_out));
Cyc_log(({const char*_Tmp1="Warning: use of region sidestepped\n";_tag_fat(_Tmp1,sizeof(char),36U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 7:
# 453
 Cyc_log(({const char*_Tmp1="Warning: use of __extension__ deleted\n";_tag_fat(_Tmp1,sizeof(char),39U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 8:
# 457 "buildlib.cyl"
 Cyc_log(({const char*_Tmp1="Warning: use of nonnull attribute deleted\n";_tag_fat(_Tmp1,sizeof(char),43U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 9:
# 462 "buildlib.cyl"
 Cyc_log(({const char*_Tmp1="Warning: use of mode HI deleted\n";_tag_fat(_Tmp1,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 10:
# 465
 Cyc_log(({const char*_Tmp1="Warning: use of mode SI deleted\n";_tag_fat(_Tmp1,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 11:
# 468
 Cyc_log(({const char*_Tmp1="Warning: use of mode QI deleted\n";_tag_fat(_Tmp1,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 12:
# 471
 Cyc_log(({const char*_Tmp1="Warning: use of mode DI deleted\n";_tag_fat(_Tmp1,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 13:
# 474
 Cyc_log(({const char*_Tmp1="Warning: use of mode DI deleted\n";_tag_fat(_Tmp1,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 14:
# 477
 Cyc_log(({const char*_Tmp1="Warning: use of mode word deleted\n";_tag_fat(_Tmp1,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 15:
# 480
 Cyc_fputs("inline",_check_null(Cyc_slurp_out));return 1;case 16:
# 482
 Cyc_fputs("inline",_check_null(Cyc_slurp_out));return 1;case 17:
# 484
 Cyc_fputs("const",_check_null(Cyc_slurp_out));return 1;case 18:
# 486
 Cyc_fputs("const",_check_null(Cyc_slurp_out));return 1;case 19:
# 491 "buildlib.cyl"
 Cyc_fputs("int",_check_null(Cyc_slurp_out));return 1;case 20:
# 493
 return 1;case 21:
# 495
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",_check_null(Cyc_slurp_out));
Cyc_log(({const char*_Tmp1="Warning: replacing use of __asm__ with 0\n";_tag_fat(_Tmp1,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 22:
# 501
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",_check_null(Cyc_slurp_out));
Cyc_log(({const char*_Tmp1="Warning: replacing use of __asm with 0\n";_tag_fat(_Tmp1,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
return 1;case 23:
# 507
({int _Tmp1=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_Tmp1,_check_null(Cyc_slurp_out));});return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);};}
# 511
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 513
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 511 "buildlib.cyl"
 return 0;case 1:
# 513
 Cyc_fputc(34,_check_null(Cyc_slurp_out));return 0;case 2:
# 515
 Cyc_log(({const char*_Tmp1="Warning: unclosed string\n";_tag_fat(_Tmp1,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp3="%s";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(_Tmp2,sizeof(void*),1));});return 1;case 3:
# 518
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp3="%s";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(_Tmp2,sizeof(void*),1));});return 1;case 4:
# 520
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp3="%s";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(_Tmp2,sizeof(void*),1));});return 1;case 5:
# 522
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp3="%s";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(_Tmp2,sizeof(void*),1));});return 1;case 6:
# 524
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp3="%s";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(_Tmp2,sizeof(void*),1));});return 1;case 7:
# 526
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp3="%s";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(_Tmp2,sizeof(void*),1));});return 1;case 8:
# 528
({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_Lexing_lexeme(lexbuf);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp3="%s";_tag_fat(_Tmp3,sizeof(char),3U);}),_tag_fat(_Tmp2,sizeof(void*),1));});return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);};}
# 532
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 534
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
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
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 561
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 559 "buildlib.cyl"
 Cyc_log(({const char*_Tmp1="Warning: unclosed string\n";_tag_fat(_Tmp1,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 561
 return 0;case 2:
# 563
 Cyc_log(({const char*_Tmp1="Warning: unclosed string\n";_tag_fat(_Tmp1,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));return 1;case 3:
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
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 581
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 579 "buildlib.cyl"
 Cyc_log(({const char*_Tmp1="Warning: unclosed comment\n";_tag_fat(_Tmp1,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 581
 return 0;case 2:
# 583
 return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);};}
# 587
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 589
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple14*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 591 "buildlib.cyl"
 Cyc_current_line=({const char*_Tmp1="#define ";_tag_fat(_Tmp1,sizeof(char),9U);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple14*_Tmp1=_cycalloc(sizeof(struct _tuple14));_Tmp1->f1=Cyc_current_line,_Tmp1->f2=_check_null(Cyc_current_source);_Tmp1;});case 1:
# 595
 return Cyc_suck_line(lexbuf);case 2:
# 597
 return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);};}
# 601
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 603
struct _tuple14*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;if(_Tmp0 == 0){
# 601 "buildlib.cyl"
Cyc_current_source=({struct _fat_ptr*_Tmp1=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp2=Cyc_Lexing_lexeme(lexbuf);*_Tmp1=_Tmp2;});_Tmp1;});
Cyc_current_line=Cyc_strconcat(Cyc_current_line,*_check_null(Cyc_current_source));
return Cyc_suck_restofline(lexbuf);}else{
# 605
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);};}
# 608
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 610
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;if(_Tmp0 == 0){
# 608 "buildlib.cyl"
Cyc_current_line=({struct _fat_ptr _Tmp1=Cyc_current_line;Cyc_strconcat(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});return 0;}else{
((lexbuf->refill_buff))(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);};}
# 612
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 614
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple17*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 615 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1:
# 617
 Cyc_current_headerfile=({
struct _fat_ptr _Tmp1=Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_Tmp1,0,(unsigned long)(({
int _Tmp2=Cyc_Lexing_lexeme_end(lexbuf);_Tmp2 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
Cyc_current_cpp=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=Cyc_List_imp_rev(Cyc_current_hstubs);
Cyc_current_cstubs=Cyc_List_imp_rev(Cyc_current_cstubs);
Cyc_current_cycstubs=Cyc_List_imp_rev(Cyc_current_cycstubs);
Cyc_current_cpp=Cyc_List_imp_rev(Cyc_current_cpp);
return({struct _tuple17*_Tmp1=_cycalloc(sizeof(struct _tuple17));_Tmp1->f1=Cyc_current_headerfile,_Tmp1->f2=Cyc_current_symbols,_Tmp1->f3=Cyc_current_user_defs,_Tmp1->f4=Cyc_current_omit_symbols,_Tmp1->f5=Cyc_current_hstubs,_Tmp1->f6=Cyc_current_cstubs,_Tmp1->f7=Cyc_current_cycstubs,_Tmp1->f8=Cyc_current_cpp;_Tmp1;});case 2:
# 642
 return Cyc_spec(lexbuf);case 3:
# 644
 return 0;case 4:
# 646
({struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,({
# 648
unsigned long _Tmp3=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Error in .cys file: expected header file name, found '%c' instead\n";_tag_fat(_Tmp3,sizeof(char),67U);}),_tag_fat(_Tmp2,sizeof(void*),1));});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);};}
# 653
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 655
struct _tuple17*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 653 "buildlib.cyl"
 return 0;case 1:
# 655
 return 0;case 2:
# 657
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=Cyc_List_append(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3:
# 662
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*tmp_user_defs=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){;}
if(tmp_user_defs != Cyc_current_user_defs){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp1="Error in .cys file: got optional definition in omitsymbols\n";_tag_fat(_Tmp1,sizeof(char),60U);}),_tag_fat(0U,sizeof(void*),0));
# 668
return 0;}
# 670
Cyc_current_omit_symbols=Cyc_List_append(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;}case 4:
# 673
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f1=_tag_fat(0,0,0),({
struct _fat_ptr _Tmp1=Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f2=_Tmp1;});
Cyc_current_hstubs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=Cyc_current_hstubs;_Tmp1;});
return 1;}case 5:  {
# 681
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_Tmp1="hstub";_tag_fat(_Tmp1,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f1=symbol,({
struct _fat_ptr _Tmp1=Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f2=_Tmp1;});
Cyc_current_hstubs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=Cyc_current_hstubs;_Tmp1;});
return 1;}}}}case 6:
# 695
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f1=_tag_fat(0,0,0),({
struct _fat_ptr _Tmp1=Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f2=_Tmp1;});
Cyc_current_cstubs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=Cyc_current_cstubs;_Tmp1;});
return 1;}case 7:  {
# 703
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_Tmp1="cstub";_tag_fat(_Tmp1,sizeof(char),6U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f1=symbol,({
struct _fat_ptr _Tmp1=Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f2=_Tmp1;});
Cyc_current_cstubs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=Cyc_current_cstubs;_Tmp1;});
return 1;}}}}case 8:
# 717
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f1=_tag_fat(0,0,0),({
struct _fat_ptr _Tmp1=Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f2=_Tmp1;});
Cyc_current_cycstubs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=Cyc_current_cycstubs;_Tmp1;});
return 1;}case 9:  {
# 725
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
_fat_ptr_inplace_plus(& s,sizeof(char),(int)Cyc_strlen(({const char*_Tmp1="cycstub";_tag_fat(_Tmp1,sizeof(char),8U);})));
while(isspace((int)*((char*)_check_fat_subscript(s,sizeof(char),0U)))){_fat_ptr_inplace_plus(& s,sizeof(char),1);}{
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _fat_ptr symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple15*x;x=_cycalloc(sizeof(struct _tuple15)),x->f1=symbol,({
struct _fat_ptr _Tmp1=Cyc_Buffer_contents(_check_null(Cyc_specbuf));x->f2=_Tmp1;});
Cyc_current_cycstubs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=Cyc_current_cycstubs;_Tmp1;});
return 1;}}}}case 10:
# 739
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _fat_ptr*x;x=_cycalloc(sizeof(struct _fat_ptr)),({struct _fat_ptr _Tmp1=Cyc_Buffer_contents(_check_null(Cyc_specbuf));*x=_Tmp1;});
Cyc_current_cpp=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=Cyc_current_cpp;_Tmp1;});
return 1;}case 11:
# 746
 return 1;case 12:
# 748
 return 1;case 13:
# 750
({struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,({
# 752
unsigned long _Tmp3=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Error in .cys file: expected command, found '%c' instead\n";_tag_fat(_Tmp3,sizeof(char),58U);}),_tag_fat(_Tmp2,sizeof(void*),1));});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);};}
# 757
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 759
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 763 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=Cyc_Lexing_lexeme(lexbuf);*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=Cyc_snarfed_symbols;_Tmp1;});
return 1;case 1:  {
# 766
struct _fat_ptr s=Cyc_Lexing_lexeme(lexbuf);
struct _fat_ptr t=s;
while(!isspace((int)*((char*)_check_fat_subscript(t,sizeof(char),0U)))){_fat_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=Cyc_substring(s,0,(unsigned long)((t.curr - s.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}
# 775
Cyc_rename_current_symbol=0;{
struct _tuple16*user_def;user_def=_cycalloc(sizeof(struct _tuple16)),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=Cyc_current_symbol;_Tmp2;});user_def->f1=_Tmp1;}),({
struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=Cyc_Buffer_contents(_check_null(Cyc_specbuf));*_Tmp2=_Tmp3;});_Tmp2;});user_def->f2=_Tmp1;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=Cyc_current_symbol;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=Cyc_snarfed_symbols;_Tmp1;});
Cyc_current_user_defs=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=user_def,_Tmp1->tl=Cyc_current_user_defs;_Tmp1;});
return 1;}}case 2:
# 782
 return 1;case 3:
# 784
 return 0;case 4:
# 786
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp1="Error in .cys file: unexpected end-of-file\n";_tag_fat(_Tmp1,sizeof(char),44U);}),_tag_fat(0U,sizeof(void*),0));
# 788
return 0;case 5:
# 790
({struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,({
# 792
unsigned long _Tmp3=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_fprintf(Cyc_stderr,({const char*_Tmp3="Error in .cys file: expected symbol, found '%c' instead\n";_tag_fat(_Tmp3,sizeof(char),57U);}),_tag_fat(_Tmp2,sizeof(void*),1));});
return 0;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);};}
# 797
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 799
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 803 "buildlib.cyl"
 Cyc_log(({const char*_Tmp1="Warning: unclosed brace\n";_tag_fat(_Tmp1,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 805
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'}');
return 1;case 2:
# 810
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'{');
return 1;case 3:
# 814
 Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4:
# 818
 Cyc_Buffer_add_string(_check_null(Cyc_specbuf),({const char*_Tmp1="/*";_tag_fat(_Tmp1,sizeof(char),3U);}));
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5:
# 822
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:  {
# 825
struct _fat_ptr symbol=Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol && !Cyc_strcmp(symbol,Cyc_current_symbol))
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,*Cyc_add_user_prefix(({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=symbol;_Tmp2;})));});else{
# 829
Cyc_Buffer_add_string(_check_null(Cyc_specbuf),symbol);}
return 1;}case 7:
# 832
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_Tmp1,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);};}
# 837
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 839
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 837 "buildlib.cyl"
 Cyc_log(({const char*_Tmp1="Warning: unclosed string\n";_tag_fat(_Tmp1,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 839
 Cyc_Buffer_add_char(_check_null(Cyc_specbuf),'"');return 0;case 2:
# 841
 Cyc_log(({const char*_Tmp1="Warning: unclosed string\n";_tag_fat(_Tmp1,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3:
# 845
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4:
# 848
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5:
# 851
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6:
# 854
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7:
# 857
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8:
# 860
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);};}
# 865
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 867
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _Tmp0=lexstate;switch((int)_Tmp0){case 0:
# 865 "buildlib.cyl"
 Cyc_log(({const char*_Tmp1="Warning: unclosed comment\n";_tag_fat(_Tmp1,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));return 0;case 1:
# 867
 Cyc_Buffer_add_string(_check_null(Cyc_specbuf),({const char*_Tmp1="*/";_tag_fat(_Tmp1,sizeof(char),3U);}));return 0;case 2:
# 869
({struct Cyc_Buffer_t*_Tmp1=_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_Tmp1,Cyc_Lexing_lexeme(lexbuf));});
return 1;default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);};}
# 874
(void*)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Lexing_Error_exn_struct));_Tmp0->tag=Cyc_Lexing_Error,_Tmp0->f1=({const char*_Tmp1="some action didn't return!";_tag_fat(_Tmp1,sizeof(char),27U);});_Tmp0;}));}
# 876
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 882 "buildlib.cyl"
void Cyc_scan_type(void*,struct Cyc_Hashtable_Table*);struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_stmttmp1=(_check_null(e))->r;void*_Tmp0=_stmttmp1;enum Cyc_Absyn_MallocKind _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 1: _Tmp4=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp4;
# 886
struct _fat_ptr*v=(*Cyc_Absyn_binding2qvar(b)).f2;
Cyc_add_target(v);
return;}case 3: _Tmp4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*x=_Tmp4;
# 890
for(1;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 893
return;}case 23: _Tmp4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 895
_Tmp4=e1;_Tmp3=e2;goto _LL8;}case 9: _Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 897
_Tmp4=e1;_Tmp3=e2;goto _LLA;}case 4: _Tmp4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_LLA: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 899
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 41: _Tmp4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp4;
_Tmp4=e1;goto _LLE;}case 20: _Tmp4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LLE: {struct Cyc_Absyn_Exp*e1=_Tmp4;
# 904
_Tmp4=e1;goto _LL10;}case 18: _Tmp4=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL10: {struct Cyc_Absyn_Exp*e1=_Tmp4;
# 906
_Tmp4=e1;goto _LL12;}case 15: _Tmp4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_Tmp4;
# 908
_Tmp4=e1;goto _LL14;}case 5: _Tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_Tmp4;
# 910
Cyc_scan_exp(e1,dep);
return;}case 6: _Tmp4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;struct Cyc_Absyn_Exp*e3=_Tmp2;
# 913
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
Cyc_scan_exp(e3,dep);
return;}case 7: _Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
_Tmp4=e1;_Tmp3=e2;goto _LL1A;}case 8: _Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_Absyn_Exp*e2=_Tmp3;
# 919
Cyc_scan_exp(e1,dep);
Cyc_scan_exp(e2,dep);
return;}case 10: _Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct Cyc_List_List*x=_Tmp3;
# 923
Cyc_scan_exp(e1,dep);
for(1;x != 0;x=x->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)x->hd,dep);}
# 927
return;}case 14: _Tmp4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t1=_Tmp4;struct Cyc_Absyn_Exp*e1=_Tmp3;
# 929
Cyc_scan_type(t1,dep);
Cyc_scan_exp(e1,dep);
return;}case 34: _Tmp1=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).mknd;_Tmp4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).rgn;_Tmp3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).elt_type;_Tmp2=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_Tmp0)->f1).num_elts;{enum Cyc_Absyn_MallocKind mknd=_Tmp1;struct Cyc_Absyn_Exp*ropt=_Tmp4;void**topt=_Tmp3;struct Cyc_Absyn_Exp*e=_Tmp2;
# 933
if(ropt != 0)Cyc_scan_exp(ropt,dep);
if(topt != 0)Cyc_scan_type(*topt,dep);
Cyc_scan_exp(e,dep);
return;}case 38: _Tmp4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
# 938
Cyc_scan_exp(e,dep);return;}case 39: _Tmp4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*t1=_Tmp4;
_Tmp4=t1;goto _LL26;}case 17: _Tmp4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL26: {void*t1=_Tmp4;
# 941
Cyc_scan_type(t1,dep);
return;}case 21: _Tmp4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp4;struct _fat_ptr*fn=_Tmp3;
# 944
_Tmp4=e1;_Tmp3=fn;goto _LL2A;}case 22: _Tmp4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_Tmp4;struct _fat_ptr*fn=_Tmp3;
# 946
Cyc_scan_exp(e1,dep);
Cyc_add_target(fn);
return;}case 19: _Tmp4=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{void*t1=_Tmp4;struct Cyc_List_List*f=_Tmp3;
# 950
Cyc_scan_type(t1,dep);
# 952
{void*_stmttmp2=(void*)(_check_null(f))->hd;void*_Tmp5=_stmttmp2;void*_Tmp6;if(*((int*)_Tmp5)== 0){_Tmp6=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_Tmp5)->f1;{struct _fat_ptr*fn=_Tmp6;
Cyc_add_target(fn);goto _LL57;}}else{
goto _LL57;}_LL57:;}
# 956
return;}case 0:
# 958
 return;case 36: _Tmp4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*x=_Tmp4;
# 960
for(1;x != 0;x=x->tl){
struct _tuple18*_stmttmp3=(struct _tuple18*)x->hd;struct _tuple18*_Tmp5=_stmttmp3;void*_Tmp6;_Tmp6=_Tmp5->f2;{struct Cyc_Absyn_Exp*e1=_Tmp6;
Cyc_scan_exp(e1,dep);}}
# 964
return;}case 40:
 return;case 2:
# 967
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Pragma_e\n";_tag_fat(_Tmp5,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 35:
# 970
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Swap_e\n";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 37:
# 973
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Stmt_e\n";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 42:
# 976
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Assert_e\n";_tag_fat(_Tmp5,sizeof(char),28U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 11:
# 979
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Throw_e\n";_tag_fat(_Tmp5,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 12:
# 982
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected NoInstantiate_e\n";_tag_fat(_Tmp5,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 13:
# 985
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Instantiate_e\n";_tag_fat(_Tmp5,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 16:
# 988
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected New_e\n";_tag_fat(_Tmp5,sizeof(char),25U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 24:
# 991
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Tuple_e\n";_tag_fat(_Tmp5,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 25:
# 994
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected CompoundLit_e\n";_tag_fat(_Tmp5,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 26:
# 997
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Array_e\n";_tag_fat(_Tmp5,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 27:
# 1000
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Comprehension_e\n";_tag_fat(_Tmp5,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 28:
# 1003
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected ComprehensionNoinit_e\n";_tag_fat(_Tmp5,sizeof(char),41U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 29:
# 1006
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Aggregate_e\n";_tag_fat(_Tmp5,sizeof(char),31U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 30:
# 1009
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected AnonStruct_e\n";_tag_fat(_Tmp5,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 31:
# 1012
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Datatype_e\n";_tag_fat(_Tmp5,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 32:
# 1015
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected Enum_e\n";_tag_fat(_Tmp5,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;default:
# 1018
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp5="Error: unexpected AnonEnum_e\n";_tag_fat(_Tmp5,sizeof(char),30U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;};}
# 1023
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned)eo)Cyc_scan_exp(eo,dep);
return;}
# 1028
void Cyc_scan_decl(struct Cyc_Absyn_Decl*,struct Cyc_Hashtable_Table*);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_Tmp0=t;struct Cyc_Absyn_FnInfo _Tmp1;void*_Tmp2;struct Cyc_Absyn_PtrInfo _Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0: _Tmp5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f2;{void*c=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 1032
void*_Tmp6=c;void*_Tmp7;union Cyc_Absyn_AggrInfo _Tmp8;switch(*((int*)_Tmp6)){case 0:
 goto _LL21;case 1: _LL21:
 goto _LL23;case 17: _LL23:
 goto _LL25;case 2: _LL25:
 goto _LL27;case 16: _LL27:
# 1038
 return;case 20: _Tmp8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_Tmp6)->f1;{union Cyc_Absyn_AggrInfo info=_Tmp8;
# 1040
struct _tuple12 _stmttmp4=Cyc_Absyn_aggr_kinded_name(info);struct _tuple12 _Tmp9=_stmttmp4;void*_TmpA;_TmpA=(_Tmp9.f2)->f2;{struct _fat_ptr*v=_TmpA;
_Tmp7=v;goto _LL2B;}}case 15: _Tmp7=(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_Tmp6)->f1)->f2;_LL2B: {struct _fat_ptr*v=_Tmp7;
Cyc_add_target(v);return;}case 18:
 goto _LL2F;case 19: _LL2F: goto _LL31;case 3: _LL31:
 goto _LL33;case 5: _LL33: goto _LL35;case 6: _LL35:
 goto _LL37;case 7: _LL37: goto _LL39;case 8: _LL39:
 goto _LL3B;case 9: _LL3B: goto _LL3D;case 10: _LL3D:
 goto _LL3F;case 4: _LL3F: goto _LL41;case 11: _LL41:
 goto _LL43;case 12: _LL43: goto _LL45;case 13: _LL45:
 goto _LL47;default: _LL47:
({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=Cyc_Absynpp_typ2string(t);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[1];_TmpA[0]=& _Tmp9;Cyc_fprintf(Cyc_stderr,({const char*_TmpB="Error: unexpected %s\n";_tag_fat(_TmpB,sizeof(char),22U);}),_tag_fat(_TmpA,sizeof(void*),1));});
exit(1);return;};}case 3: _Tmp3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_PtrInfo x=_Tmp3;
# 1055
Cyc_scan_type(x.elt_type,dep);
return;}case 4: _Tmp5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).elt_type;_Tmp4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).num_elts;_Tmp2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp0)->f1).zero_term;{void*t=_Tmp5;struct Cyc_Absyn_Exp*sz=_Tmp4;void*zt=_Tmp2;
# 1058
Cyc_scan_type(t,dep);
Cyc_scan_exp_opt(sz,dep);
return;}case 11: _Tmp5=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp5;
# 1062
Cyc_scan_exp(e,dep);
return;}case 5: _Tmp1=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp0)->f1;{struct Cyc_Absyn_FnInfo x=_Tmp1;
# 1065
Cyc_scan_type(x.ret_type,dep);
{struct Cyc_List_List*a=x.args;for(0;a != 0;a=a->tl){
struct _tuple9*_stmttmp5=(struct _tuple9*)a->hd;struct _tuple9*_Tmp6=_stmttmp5;void*_Tmp7;_Tmp7=_Tmp6->f3;{void*argt=_Tmp7;
Cyc_scan_type(argt,dep);}}}
# 1070
if(x.cyc_varargs != 0)
Cyc_scan_type((x.cyc_varargs)->type,dep);
return;}case 7: _Tmp5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp0)->f2;{struct Cyc_List_List*sfs=_Tmp5;
# 1074
for(1;sfs != 0;sfs=sfs->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)sfs->hd)->width,dep);}
# 1078
return;}case 8: _Tmp5=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_Tmp0)->f1)->f2;{struct _fat_ptr*v=_Tmp5;
# 1080
Cyc_add_target(v);
return;}case 10: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)){case 0: _Tmp5=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp5;
# 1084
({struct Cyc_Absyn_Decl*_Tmp6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct));_Tmp7->tag=5,_Tmp7->f1=x;_Tmp7;}),0U);Cyc_scan_decl(_Tmp6,dep);});{
struct _tuple1*_stmttmp6=x->name;struct _tuple1*_Tmp6=_stmttmp6;void*_Tmp7;_Tmp7=_Tmp6->f2;{struct _fat_ptr*n=_Tmp7;
Cyc_add_target(n);
return;}}}case 1: _Tmp5=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp5;
# 1090
({struct Cyc_Absyn_Decl*_Tmp6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct));_Tmp7->tag=7,_Tmp7->f1=x;_Tmp7;}),0U);Cyc_scan_decl(_Tmp6,dep);});{
struct _tuple1*_stmttmp7=x->name;struct _tuple1*_Tmp6=_stmttmp7;void*_Tmp7;_Tmp7=_Tmp6->f2;{struct _fat_ptr*n=_Tmp7;
Cyc_add_target(n);
return;}}}default: _Tmp5=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_Tmp0)->f1)->r)->f1;{struct Cyc_Absyn_Datatypedecl*dd=_Tmp5;
# 1096
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Error: unexpected Datatype declaration\n";_tag_fat(_Tmp6,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;}}case 1:
# 1099
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Error: unexpected Evar\n";_tag_fat(_Tmp6,sizeof(char),24U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 2:
# 1102
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Error: unexpected VarType\n";_tag_fat(_Tmp6,sizeof(char),27U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;case 6:
# 1105
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Error: unexpected TupleType\n";_tag_fat(_Tmp6,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;default:
# 1108
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Error: unexpected valueof_t\n";_tag_fat(_Tmp6,sizeof(char),29U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);return;};}
# 1113
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**saved_targets=Cyc_current_targets;
struct _fat_ptr*saved_source=Cyc_current_source;
Cyc_current_targets=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
{void*_stmttmp8=d->r;void*_Tmp0=_stmttmp8;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp1;
# 1119
struct _tuple1*_stmttmp9=x->name;struct _tuple1*_Tmp2=_stmttmp9;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
Cyc_scan_type(x->type,dep);
Cyc_scan_exp_opt(x->initializer,dep);
goto _LL0;}}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*x=_Tmp1;
# 1125
struct _tuple1*_stmttmpA=x->name;struct _tuple1*_Tmp2=_stmttmpA;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
Cyc_scan_type((x->i).ret_type,dep);
{struct Cyc_List_List*a=(x->i).args;for(0;a != 0;a=a->tl){
struct _tuple9*_stmttmpB=(struct _tuple9*)a->hd;struct _tuple9*_Tmp4=_stmttmpB;void*_Tmp5;_Tmp5=_Tmp4->f3;{void*t1=_Tmp5;
Cyc_scan_type(t1,dep);}}}
# 1132
if((x->i).cyc_varargs != 0)
Cyc_scan_type((_check_null((x->i).cyc_varargs))->type,dep);
if(x->is_inline)
Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="Warning: ignoring inline function\n";_tag_fat(_Tmp4,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));
goto _LL0;}}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp1;
# 1138
struct _tuple1*_stmttmpC=x->name;struct _tuple1*_Tmp2=_stmttmpC;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
if((unsigned)x->impl){
{struct Cyc_List_List*fs=(_check_null(x->impl))->fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*f=(struct Cyc_Absyn_Aggrfield*)fs->hd;
Cyc_scan_type(f->type,dep);
Cyc_scan_exp_opt(f->width,dep);}}{
# 1148
struct Cyc_List_List*fs=(_check_null(x->impl))->fields;for(0;fs != 0;fs=fs->tl){;}}}
# 1152
goto _LL0;}}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp1;
# 1154
struct _tuple1*_stmttmpD=x->name;struct _tuple1*_Tmp2=_stmttmpD;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
if((unsigned)x->fields){
{struct Cyc_List_List*fs=(struct Cyc_List_List*)(_check_null(x->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_scan_exp_opt(f->tag,dep);}}{
# 1163
struct Cyc_List_List*fs=(struct Cyc_List_List*)(_check_null(x->fields))->v;for(0;fs != 0;fs=fs->tl){;}}}
# 1167
goto _LL0;}}case 8: _Tmp1=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*x=_Tmp1;
# 1169
struct _tuple1*_stmttmpE=x->name;struct _tuple1*_Tmp2=_stmttmpE;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct _fat_ptr*v=_Tmp3;
Cyc_current_source=v;
if((unsigned)x->defn)
Cyc_scan_type(_check_null(x->defn),dep);
goto _LL0;}}case 4:
# 1175
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected region declaration";_tag_fat(_Tmp2,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 13:
# 1178
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected __cyclone_port_on__";_tag_fat(_Tmp2,sizeof(char),38U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 14:
# 1181
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected __cyclone_port_off__";_tag_fat(_Tmp2,sizeof(char),39U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 15:
# 1184
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected __tempest_on__";_tag_fat(_Tmp2,sizeof(char),33U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 16:
# 1187
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected __tempest_off__";_tag_fat(_Tmp2,sizeof(char),34U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 2:
# 1190
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected let declaration\n";_tag_fat(_Tmp2,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 6:
# 1193
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected datatype declaration\n";_tag_fat(_Tmp2,sizeof(char),40U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 3:
# 1196
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected let declaration\n";_tag_fat(_Tmp2,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 9:
# 1199
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected namespace declaration\n";_tag_fat(_Tmp2,sizeof(char),41U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 10:
# 1202
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected using declaration\n";_tag_fat(_Tmp2,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);case 11:
# 1205
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected extern \"C\" declaration\n";_tag_fat(_Tmp2,sizeof(char),42U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);default:
# 1208
 Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: unexpected extern \"C include\" declaration\n";_tag_fat(_Tmp2,sizeof(char),50U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);}_LL0:;}{
# 1216
struct Cyc_Set_Set*old;
struct _fat_ptr*name=_check_null(Cyc_current_source);
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
old=({(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(dep,name);;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3=_Tmp2;void*_Tmp4;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp3)->tag == Cyc_Core_Not_found){
# 1221
old=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);goto _LL35;}else{_Tmp4=_Tmp3;{void*exn=_Tmp4;(void*)_rethrow(exn);}}_LL35:;}}}{
# 1223
struct Cyc_Set_Set*targets=Cyc_Set_union_two(*_check_null(Cyc_current_targets),old);
({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;})(dep,name,targets);
# 1226
Cyc_current_targets=saved_targets;
Cyc_current_source=saved_source;}}}
# 1230
struct Cyc_Hashtable_Table*Cyc_new_deps (void){
return({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;})(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1234
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _fat_ptr*x){
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){{struct Cyc_Set_Set*_Tmp2=({(struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(t,x);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3=_Tmp2;void*_Tmp4;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp3)->tag == Cyc_Core_Not_found)
# 1237
return({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);else{_Tmp4=_Tmp3;{void*exn=_Tmp4;(void*)_rethrow(exn);}};}}}
# 1241
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1251 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(curr,(struct _fat_ptr*)init->hd);}{
# 1256
struct Cyc_Set_Set*delta=curr;
# 1258
struct _fat_ptr*sptr;sptr=_cycalloc(sizeof(struct _fat_ptr)),*sptr=({const char*_Tmp0="";_tag_fat(_Tmp0,sizeof(char),1U);});
while(Cyc_Set_cardinality(delta)> 0){
struct Cyc_Set_Set*next=emptyset;
struct Cyc_Iter_Iter iter=Cyc_Set_make_iter(Cyc_Core_heap_region,delta);
while(({(int(*)(struct Cyc_Iter_Iter,struct _fat_ptr**))Cyc_Iter_next;})(iter,& sptr)){
next=({struct Cyc_Set_Set*_Tmp0=next;Cyc_Set_union_two(_Tmp0,Cyc_find(t,sptr));});}
delta=Cyc_Set_diff(next,curr);
curr=Cyc_Set_union_two(curr,delta);}
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
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Internal error: script file is NULL\n";_tag_fat(_Tmp0,sizeof(char),37U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);}
# 1284
return Cyc_vfprintf(_check_null(Cyc_script_file),fmt,ap);}
# 1287
int Cyc_force_directory(struct _fat_ptr d){
if((int)Cyc_mode == 2)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=d;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=d;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_prscript(({const char*_Tmp3="if ! test -e %s; then mkdir %s; fi\n";_tag_fat(_Tmp3,sizeof(char),36U);}),_tag_fat(_Tmp2,sizeof(void*),2));});else{
# 1294
int fd=Cyc_open((const char*)_untag_fat_ptr(d,sizeof(char),1U),0,_tag_fat(0U,sizeof(unsigned short),0));
if(fd == -1){
if(mkdir((const char*)_check_null(_untag_fat_ptr(d,sizeof(char),1U)),448)== -1){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=d;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not create directory %s\n";_tag_fat(_Tmp2,sizeof(char),38U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}}else{
# 1301
close(fd);}}
# 1303
return 0;}
# 1306
int Cyc_force_directory_prefixes(struct _fat_ptr file){
# 1310
struct _fat_ptr curr=Cyc_strdup(file);
# 1312
struct Cyc_List_List*x=0;
while(1){
curr=Cyc_Filename_dirname(curr);
if(Cyc_strlen(curr)== 0U)break;
x=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=curr;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=x;_Tmp0;});}
# 1319
for(1;x != 0;x=x->tl){
if(Cyc_force_directory(*((struct _fat_ptr*)x->hd)))return 1;}
# 1322
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _fat_ptr f1;};
# 1329
static int Cyc_is_other_special(char c){
char _Tmp0=c;switch((int)_Tmp0){case 92:
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
unsigned long len=Cyc_strlen(s);
# 1354
int single_quotes=0;
int other_special=0;
{int i=0;for(0;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c == 39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 1363
if(single_quotes == 0 && other_special == 0)
return s;
# 1367
if(single_quotes == 0)
return Cyc_strconcat_l(({struct _fat_ptr*_Tmp0[3];({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3="'";_tag_fat(_Tmp3,sizeof(char),2U);});_Tmp2;});_Tmp0[0]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0[1]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=({const char*_Tmp3="'";_tag_fat(_Tmp3,sizeof(char),2U);});_Tmp2;});_Tmp0[2]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _fat_ptr*),3));}));{
# 1371
unsigned long len2=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;
struct _fat_ptr s2=({unsigned _Tmp0=(len2 + 1U)+ 1U;_tag_fat(({char*_Tmp1=_cycalloc_atomic(_check_times(_Tmp0,sizeof(char)));({{unsigned _Tmp2=len2 + 1U;unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]='\000';}_Tmp1[_Tmp2]=0;}0;});_Tmp1;}),sizeof(char),_Tmp0);});
int i=0;
int j=0;
for(1;(unsigned long)i < len;++ i){
char c=((const char*)s.curr)[i];
if((int)c == 39 || Cyc_is_other_special(c))
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\\';if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(s2,sizeof(char),j ++);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))== 1U &&(_Tmp1 == 0 && _Tmp2 != 0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 1381
return s2;}}
# 1383
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_Tmp0=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp1=Cyc_sh_escape_string(*sp);*_Tmp0=_Tmp1;});_Tmp0;});}
# 1388
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1396
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1401
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="********************************* %s...\n";_tag_fat(_Tmp2,sizeof(char),41U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1404
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_log(({const char*_Tmp2="\n%s:\n";_tag_fat(_Tmp2,sizeof(char),6U);}),_tag_fat(_Tmp1,sizeof(void*),1));});{
# 1416 "buildlib.cyl"
struct _fat_ptr basename=Cyc_Filename_basename(({const char*_Tmp0=filename;_tag_fat(_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}));
struct _fat_ptr dirname=Cyc_Filename_dirname(({const char*_Tmp0=filename;_tag_fat(_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));}));
struct _fat_ptr choppedname=Cyc_Filename_chop_extension(basename);
const char*cppinfile=(const char*)_check_null(_untag_fat_ptr(Cyc_strconcat(choppedname,({const char*_Tmp0=".iA";_tag_fat(_Tmp0,sizeof(char),4U);})),sizeof(char),1U));
# 1421
const char*macrosfile=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=choppedname;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="%s.iB";_tag_fat(_Tmp2,sizeof(char),6U);}),_tag_fat(_Tmp1,sizeof(void*),1));}):({struct _fat_ptr _Tmp0=dirname;Cyc_Filename_concat(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=choppedname;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(({const char*_Tmp3="%s.iB";_tag_fat(_Tmp3,sizeof(char),6U);}),_tag_fat(_Tmp2,sizeof(void*),1));}));}),sizeof(char),1U));
# 1426
const char*declsfile=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=choppedname;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="%s.iC";_tag_fat(_Tmp2,sizeof(char),6U);}),_tag_fat(_Tmp1,sizeof(void*),1));}):({struct _fat_ptr _Tmp0=dirname;Cyc_Filename_concat(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=choppedname;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(({const char*_Tmp3="%s.iC";_tag_fat(_Tmp3,sizeof(char),6U);}),_tag_fat(_Tmp2,sizeof(void*),1));}));}),sizeof(char),1U));
# 1431
const char*filtereddeclsfile=(const char*)_check_null(_untag_fat_ptr(_get_fat_size(dirname,sizeof(char))== 0U?({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=choppedname;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(({const char*_Tmp2="%s.iD";_tag_fat(_Tmp2,sizeof(char),6U);}),_tag_fat(_Tmp1,sizeof(void*),1));}):({struct _fat_ptr _Tmp0=dirname;Cyc_Filename_concat(_Tmp0,({struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=choppedname;_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_aprintf(({const char*_Tmp3="%s.iD";_tag_fat(_Tmp3,sizeof(char),6U);}),_tag_fat(_Tmp2,sizeof(void*),1));}));}),sizeof(char),1U));
# 1436
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
# 1439
if(Cyc_force_directory_prefixes(({const char*_Tmp2=filename;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));}))){
int _Tmp2=1;_npop_handler(0);return _Tmp2;}
# 1444
if((int)Cyc_mode != 3){
if((int)Cyc_mode == 2){
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(({const char*_Tmp4="cat >%s <<XXX\n";_tag_fat(_Tmp4,sizeof(char),15U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
{struct Cyc_List_List*l=cpp_insert;for(0;l != 0;l=l->tl){
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=*((struct _fat_ptr*)l->hd);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(({const char*_Tmp4="%s";_tag_fat(_Tmp4,sizeof(char),3U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}}
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(({const char*_Tmp4="#include <%s>\n";_tag_fat(_Tmp4,sizeof(char),15U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
Cyc_prscript(({const char*_Tmp2="XXX\n";_tag_fat(_Tmp2,sizeof(char),5U);}),_tag_fat(0U,sizeof(void*),0));
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_target_cflags;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=macrosfile;_tag_fat(_Tmp5,sizeof(char),_get_zero_arr_size_char((void*)_Tmp5,1U));});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=cppinfile;_tag_fat(_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;Cyc_prscript(({const char*_Tmp6="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_fat(_Tmp6,sizeof(char),35U);}),_tag_fat(_Tmp5,sizeof(void*),3));});
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_target_cflags;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=({const char*_Tmp5=declsfile;_tag_fat(_Tmp5,sizeof(char),_get_zero_arr_size_char((void*)_Tmp5,1U));});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=cppinfile;_tag_fat(_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;Cyc_prscript(({const char*_Tmp6="$GCC %s -E     -o %s -x c %s;\n";_tag_fat(_Tmp6,sizeof(char),31U);}),_tag_fat(_Tmp5,sizeof(void*),3));});
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_prscript(({const char*_Tmp4="rm %s\n";_tag_fat(_Tmp4,sizeof(char),7U);}),_tag_fat(_Tmp3,sizeof(void*),1));});}else{
# 1456
maybe=Cyc_fopen(cppinfile,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="Error: could not create file %s\n";_tag_fat(_Tmp4,sizeof(char),33U);}),_tag_fat(_Tmp3,sizeof(void*),1));});{
int _Tmp2=1;_npop_handler(0);return _Tmp2;}}
# 1461
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=cppinfile;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="Creating %s\n";_tag_fat(_Tmp4,sizeof(char),13U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
out_file=maybe;
{struct Cyc_List_List*l=cpp_insert;for(0;l != 0;l=l->tl){
Cyc_fputs((const char*)_check_null(_untag_fat_ptr(*((struct _fat_ptr*)l->hd),sizeof(char),1U)),out_file);}}
# 1467
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(out_file,({const char*_Tmp4="#include <%s>\n";_tag_fat(_Tmp4,sizeof(char),15U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
Cyc_fclose(out_file);{
struct _fat_ptr cppargs_string=({
struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));*_Tmp5=({const char*_Tmp6="";_tag_fat(_Tmp6,sizeof(char),1U);});_Tmp5;});_Tmp3->hd=_Tmp4;}),({
struct Cyc_List_List*_Tmp4=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_List_rev(Cyc_cppargs));_Tmp3->tl=_Tmp4;});_Tmp3;});
# 1470
Cyc_str_sepstr(_Tmp2,({const char*_Tmp3=" ";_tag_fat(_Tmp3,sizeof(char),2U);}));});
# 1473
char*cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_cyclone_cc;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=Cyc_target_cflags;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=cppargs_string;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7=macrosfile;_tag_fat(_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=cppinfile;_tag_fat(_Tmp8,sizeof(char),_get_zero_arr_size_char((void*)_Tmp8,1U));});_Tmp7;});void*_Tmp7[5];_Tmp7[0]=& _Tmp2,_Tmp7[1]=& _Tmp3,_Tmp7[2]=& _Tmp4,_Tmp7[3]=& _Tmp5,_Tmp7[4]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="%s %s %s -E -dM -o %s -x c %s";_tag_fat(_Tmp8,sizeof(char),30U);}),_tag_fat(_Tmp7,sizeof(void*),5));}),sizeof(char),1U));
# 1476
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({char*_Tmp4=cmd;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="%s\n";_tag_fat(_Tmp4,sizeof(char),4U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
if(!system((const char*)cmd)){
# 1481
cmd=(char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=Cyc_cyclone_cc;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=Cyc_target_cflags;_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=cppargs_string;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=({const char*_Tmp7=declsfile;_tag_fat(_Tmp7,sizeof(char),_get_zero_arr_size_char((void*)_Tmp7,1U));});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=({const char*_Tmp8=cppinfile;_tag_fat(_Tmp8,sizeof(char),_get_zero_arr_size_char((void*)_Tmp8,1U));});_Tmp7;});void*_Tmp7[5];_Tmp7[0]=& _Tmp2,_Tmp7[1]=& _Tmp3,_Tmp7[2]=& _Tmp4,_Tmp7[3]=& _Tmp5,_Tmp7[4]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="%s %s %s -E -o %s -x c %s";_tag_fat(_Tmp8,sizeof(char),26U);}),_tag_fat(_Tmp7,sizeof(void*),5));}),sizeof(char),1U));
# 1484
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({char*_Tmp4=cmd;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(Cyc_stderr,({const char*_Tmp4="%s\n";_tag_fat(_Tmp4,sizeof(char),4U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
system((const char*)cmd);}}}}
# 1491
if(Cyc_gathering()){int _Tmp2=0;_npop_handler(0);return _Tmp2;}{
# 1494
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(void*)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp2->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=macrosfile;_tag_fat(_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="can't open macrosfile %s";_tag_fat(_Tmp6,sizeof(char),25U);}),_tag_fat(_Tmp5,sizeof(void*),1));});_Tmp2->f1=_Tmp3;});_Tmp2;}));
# 1499
in_file=maybe;{
struct Cyc_Lexing_lexbuf*l=Cyc_Lexing_from_file(in_file);
struct _tuple13*entry;
while((entry=Cyc_line(l))!= 0){
struct _tuple13*_stmttmpF=_check_null(entry);struct _tuple13*_Tmp2=_stmttmpF;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr*name=_Tmp4;struct Cyc_Set_Set*uses=_Tmp3;
({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_Set_Set*))Cyc_Hashtable_insert;})(t,name,uses);}}
# 1508
Cyc_fclose(in_file);
# 1511
maybe=Cyc_fopen(declsfile,"r");
if(!((unsigned)maybe))(void*)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp2->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=declsfile;_tag_fat(_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="can't open declsfile %s";_tag_fat(_Tmp6,sizeof(char),24U);}),_tag_fat(_Tmp5,sizeof(void*),1));});_Tmp2->f1=_Tmp3;});_Tmp2;}));
# 1515
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(filtereddeclsfile,"w");
if(!((unsigned)Cyc_slurp_out)){int _Tmp2=1;_npop_handler(0);return _Tmp2;}
while(Cyc_slurp(l)){;}{
# 1521
struct Cyc_List_List*x=user_defs;
while(x != 0){
struct _tuple16*_stmttmp10=(struct _tuple16*)x->hd;struct _tuple16*_Tmp2=_stmttmp10;void*_Tmp3;_Tmp3=_Tmp2->f2;{struct _fat_ptr*s=_Tmp3;
({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=*s;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_fprintf(_check_null(Cyc_slurp_out),({const char*_Tmp6="%s";_tag_fat(_Tmp6,sizeof(char),3U);}),_tag_fat(_Tmp5,sizeof(void*),1));});
x=x->tl;}}
# 1527
Cyc_fclose(in_file);
Cyc_fclose(_check_null(Cyc_slurp_out));
if((int)Cyc_mode != 3)
;
# 1533
maybe=Cyc_fopen(filtereddeclsfile,"r");
if(!((unsigned)maybe)){int _Tmp2=1;_npop_handler(0);return _Tmp2;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_Tmp2=filtereddeclsfile;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*decls=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1543
{struct Cyc_List_List*d=decls;for(0;d != 0;d=d->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)d->hd,t);}}{
# 1547
struct Cyc_List_List*user_symbols=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_add_user_prefix,(Cyc_List_split(user_defs)).f1);
struct Cyc_Set_Set*reachable_set=({struct Cyc_List_List*_Tmp2=Cyc_List_append(start_symbols,user_symbols);Cyc_reachable(_Tmp2,t);});
# 1551
struct Cyc_List_List*reachable_decls=0;
struct Cyc_List_List*user_decls=0;
struct Cyc_Set_Set*defined_symbols=({(struct Cyc_Set_Set*(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Set_empty;})(Cyc_strptrcmp);
{struct Cyc_List_List*d=decls;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)d->hd;
struct _fat_ptr*name;
{void*_stmttmp11=decl->r;void*_Tmp2=_stmttmp11;void*_Tmp3;switch(*((int*)_Tmp2)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp3;
# 1559
struct _tuple1*_stmttmp12=x->name;struct _tuple1*_Tmp4=_stmttmp12;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
defined_symbols=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(defined_symbols,v);
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 1: _Tmp3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Fndecl*x=_Tmp3;
# 1565
struct _tuple1*_stmttmp13=x->name;struct _tuple1*_Tmp4=_stmttmp13;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
defined_symbols=({(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_insert;})(defined_symbols,v);
if(({(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;})(Cyc_strptrcmp,omit_symbols,v))name=0;else{
name=v;}
goto _LL6;}}case 5: _Tmp3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp3;
# 1571
struct _tuple1*_stmttmp14=x->name;struct _tuple1*_Tmp4=_stmttmp14;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL6;}}case 7: _Tmp3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp3;
# 1575
struct _tuple1*_stmttmp15=x->name;struct _tuple1*_Tmp4=_stmttmp15;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
name=v;
# 1579
if(name != 0 &&({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name))
reachable_decls=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=decl,_Tmp6->tl=reachable_decls;_Tmp6;});else{
# 1582
if((unsigned)x->fields){
struct Cyc_List_List*fs=(struct Cyc_List_List*)(_check_null(x->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
struct _tuple1*_stmttmp16=f->name;struct _tuple1*_Tmp6=_stmttmp16;void*_Tmp7;_Tmp7=_Tmp6->f2;{struct _fat_ptr*v=_Tmp7;
if(({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,v)){
reachable_decls=({struct Cyc_List_List*_Tmp8=_cycalloc(sizeof(struct Cyc_List_List));_Tmp8->hd=decl,_Tmp8->tl=reachable_decls;_Tmp8;});
break;}}}}}
# 1592
name=0;
goto _LL6;}}case 8: _Tmp3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Typedefdecl*x=_Tmp3;
# 1595
struct _tuple1*_stmttmp17=x->name;struct _tuple1*_Tmp4=_stmttmp17;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
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
if(({struct _fat_ptr _Tmp2=*name;struct _fat_ptr _Tmp3=Cyc_user_prefix;Cyc_strncmp(_Tmp2,_Tmp3,Cyc_strlen(Cyc_user_prefix));})!= 0)
reachable_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=reachable_decls;_Tmp2;});else{
# 1619
Cyc_rename_decl(decl);
user_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=user_decls;_Tmp2;});}}}}
# 1626
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){int _Tmp2=1;_npop_handler(0);return _Tmp2;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _fat_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_aprintf(({const char*_Tmp4="_%s_";_tag_fat(_Tmp4,sizeof(char),5U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
{int j=0;for(0;(unsigned)j < _get_fat_size(ifdefmacro,sizeof(char));++ j){
if((int)((char*)ifdefmacro.curr)[j]== 46 ||(int)((char*)ifdefmacro.curr)[j]== 47)
({struct _fat_ptr _Tmp2=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4='_';if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});else{
if((int)((char*)ifdefmacro.curr)[j]!= 95 &&(int)((char*)ifdefmacro.curr)[j]!= 47)
({struct _fat_ptr _Tmp2=_fat_ptr_plus(ifdefmacro,sizeof(char),j);char _Tmp3=*((char*)_check_fat_subscript(_Tmp2,sizeof(char),0U));char _Tmp4=(char)toupper((int)((char*)ifdefmacro.curr)[j]);if(_get_fat_size(_Tmp2,sizeof(char))== 1U &&(_Tmp3 == 0 && _Tmp4 != 0))_throw_arraybounds();*((char*)_Tmp2.curr)=_Tmp4;});}}}
# 1638
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=ifdefmacro;_Tmp3;});struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=ifdefmacro;_Tmp4;});void*_Tmp4[2];_Tmp4[0]=& _Tmp2,_Tmp4[1]=& _Tmp3;Cyc_fprintf(out_file,({const char*_Tmp5="#ifndef %s\n#define %s\n";_tag_fat(_Tmp5,sizeof(char),23U);}),_tag_fat(_Tmp4,sizeof(void*),2));});{
# 1645
struct Cyc_List_List*print_decls=0;
struct Cyc_List_List*names=0;
{struct Cyc_List_List*d=reachable_decls;for(0;d != 0;d=d->tl){
struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)d->hd;
int anon_enum=0;
struct _fat_ptr*name;
{void*_stmttmp18=decl->r;void*_Tmp2=_stmttmp18;void*_Tmp3;switch(*((int*)_Tmp2)){case 0: _Tmp3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Vardecl*x=_Tmp3;
# 1653
struct _tuple1*_stmttmp19=x->name;struct _tuple1*_Tmp4=_stmttmp19;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}case 1: _Tmp3=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Fndecl*x=_Tmp3;
# 1657
if(x->is_inline){name=0;goto _LL3B;}{
struct _tuple1*_stmttmp1A=x->name;struct _tuple1*_Tmp4=_stmttmp1A;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}}case 5: _Tmp3=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Aggrdecl*x=_Tmp3;
# 1662
struct _tuple1*_stmttmp1B=x->name;struct _tuple1*_Tmp4=_stmttmp1B;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}case 7: _Tmp3=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Enumdecl*x=_Tmp3;
# 1666
struct _tuple1*_stmttmp1C=x->name;struct _tuple1*_Tmp4=_stmttmp1C;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
name=v;
goto _LL3B;}}case 8: _Tmp3=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp2)->f1;{struct Cyc_Absyn_Typedefdecl*x=_Tmp3;
# 1670
struct _tuple1*_stmttmp1D=x->name;struct _tuple1*_Tmp4=_stmttmp1D;void*_Tmp5;_Tmp5=_Tmp4->f2;{struct _fat_ptr*v=_Tmp5;
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
if(({struct Cyc_Set_Set*_Tmp2=reachable_set;({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(_Tmp2,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*user_decl=Cyc_Absyn_lookup_decl(user_decls,name);
if(user_decl == 0)
(void*)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp2->tag=Cyc_Core_Impossible,_Tmp2->f1=({const char*_Tmp3="Internal Error: bad user-def name";_tag_fat(_Tmp3,sizeof(char),34U);});_Tmp2;}));else{
# 1699
void*_stmttmp1E=user_decl->r;void*_Tmp2=_stmttmp1E;switch(*((int*)_Tmp2)){case 0:
 goto _LL71;case 1: _LL71:
# 1702
(void*)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp3->tag=Cyc_NO_SUPPORT,_Tmp3->f1=({const char*_Tmp4="user defintions for function or variable decls";_tag_fat(_Tmp4,sizeof(char),47U);});_Tmp3;}));default:
 goto _LL6D;}_LL6D:;}
# 1708
print_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=print_decls;_Tmp2;});}else{
# 1711
print_decls=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=decl,_Tmp2->tl=print_decls;_Tmp2;});}
names=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=name,_Tmp2->tl=names;_Tmp2;});}}
# 1716
{struct _handler_cons _Tmp2;_push_handler(& _Tmp2);{int _Tmp3=0;if(setjmp(_Tmp2.handler))_Tmp3=1;if(!_Tmp3){
Cyc_Binding_resolve_all(print_decls);
({struct Cyc_Tcenv_Tenv*_Tmp4=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_Tmp4,1,print_decls);});
# 1717
;_pop_handler();}else{void*_Tmp4=(void*)Cyc_Core_get_exn_thrown();void*_Tmp5=_Tmp4;
# 1720
(void*)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp6->tag=Cyc_NO_SUPPORT,_Tmp6->f1=({const char*_Tmp7="can't typecheck acquired declarations";_tag_fat(_Tmp7,sizeof(char),38U);});_Tmp6;}));;}}}
# 1724
{struct _tuple0 _stmttmp1F=({struct _tuple0 _Tmp2;_Tmp2.f1=print_decls,_Tmp2.f2=names;_Tmp2;});struct _tuple0 _Tmp2=_stmttmp1F;void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*d=_Tmp4;struct Cyc_List_List*n=_Tmp3;for(0;
d != 0 && n != 0;(d=d->tl,n=n->tl)){
struct Cyc_Absyn_Decl*decl=(struct Cyc_Absyn_Decl*)d->hd;
struct _fat_ptr*name=(struct _fat_ptr*)n->hd;
int anon_enum=0;
if(!((unsigned)name))
anon_enum=1;
# 1733
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned)name){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*name;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="_%s_def_";_tag_fat(_Tmp7,sizeof(char),9U);}),_tag_fat(_Tmp6,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=ifdefmacro;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,({const char*_Tmp7="#ifndef %s\n";_tag_fat(_Tmp7,sizeof(char),12U);}),_tag_fat(_Tmp6,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=ifdefmacro;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,({const char*_Tmp7="#define %s\n";_tag_fat(_Tmp7,sizeof(char),12U);}),_tag_fat(_Tmp6,sizeof(void*),1));});
# 1739
({struct Cyc_List_List*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6[1];_Tmp6[0]=decl;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_Tmp5,out_file);});
Cyc_fprintf(out_file,({const char*_Tmp5="#endif\n";_tag_fat(_Tmp5,sizeof(char),8U);}),_tag_fat(0U,sizeof(void*),0));}else{
# 1744
({struct Cyc_List_List*_Tmp5=({struct Cyc_Absyn_Decl*_Tmp6[1];_Tmp6[0]=decl;Cyc_List_list(_tag_fat(_Tmp6,sizeof(struct Cyc_Absyn_Decl*),1));});Cyc_Absynpp_decllist2file(_Tmp5,out_file);});}}}}
# 1749
maybe=Cyc_fopen(macrosfile,"r");
if(!((unsigned)maybe))(void*)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_NO_SUPPORT_exn_struct));_Tmp2->tag=Cyc_NO_SUPPORT,({struct _fat_ptr _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=({const char*_Tmp6=macrosfile;_tag_fat(_Tmp6,sizeof(char),_get_zero_arr_size_char((void*)_Tmp6,1U));});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="can't open macrosfile %s";_tag_fat(_Tmp6,sizeof(char),25U);}),_tag_fat(_Tmp5,sizeof(void*),1));});_Tmp2->f1=_Tmp3;});_Tmp2;}));
# 1752
in_file=maybe;
l=Cyc_Lexing_from_file(in_file);{
struct _tuple14*entry2;
while((entry2=Cyc_suck_line(l))!= 0){
struct _tuple14*_stmttmp20=_check_null(entry2);struct _tuple14*_Tmp2=_stmttmp20;void*_Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr line=_Tmp4;struct _fat_ptr*name=_Tmp3;
if(({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(reachable_set,name)){
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=*name;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,({const char*_Tmp7="#ifndef %s\n";_tag_fat(_Tmp7,sizeof(char),12U);}),_tag_fat(_Tmp6,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=line;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_fprintf(out_file,({const char*_Tmp7="%s\n";_tag_fat(_Tmp7,sizeof(char),4U);}),_tag_fat(_Tmp6,sizeof(void*),1));});
Cyc_fprintf(out_file,({const char*_Tmp5="#endif\n";_tag_fat(_Tmp5,sizeof(char),8U);}),_tag_fat(0U,sizeof(void*),0));}}}
# 1763
Cyc_fclose(in_file);
if((int)Cyc_mode != 3);
# 1766
if(hstubs != 0){
struct Cyc_List_List*x=hstubs;for(0;x != 0;x=x->tl){
struct _tuple15*_stmttmp21=(struct _tuple15*)x->hd;struct _tuple15*_Tmp2=_stmttmp21;struct _fat_ptr _Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr symbol=_Tmp4;struct _fat_ptr text=_Tmp3;
if((char*)text.curr != (char*)(_tag_fat(0,0,0)).curr &&(
(char*)symbol.curr == (char*)(_tag_fat(0,0,0)).curr ||({struct Cyc_Set_Set*_Tmp5=defined_symbols;({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(_Tmp5,({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=symbol;_Tmp6;}));})))
# 1772
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);else{
# 1774
({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=symbol;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_log(({const char*_Tmp7="%s is not supported on this platform\n";_tag_fat(_Tmp7,sizeof(char),38U);}),_tag_fat(_Tmp6,sizeof(void*),1));});}}}}
# 1777
Cyc_fprintf(out_file,({const char*_Tmp2="#endif\n";_tag_fat(_Tmp2,sizeof(char),8U);}),_tag_fat(0U,sizeof(void*),0));
if(Cyc_do_setjmp){int _Tmp2=0;_npop_handler(0);return _Tmp2;}else{
Cyc_fclose(out_file);}
# 1782
if(cstubs != 0){
out_file=_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*x=cstubs;for(0;x != 0;x=x->tl){
struct _tuple15*_stmttmp22=(struct _tuple15*)x->hd;struct _tuple15*_Tmp2=_stmttmp22;struct _fat_ptr _Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr symbol=_Tmp4;struct _fat_ptr text=_Tmp3;
if((char*)text.curr != (char*)(_tag_fat(0,0,0)).curr &&(
(char*)symbol.curr == (char*)(_tag_fat(0,0,0)).curr ||({struct Cyc_Set_Set*_Tmp5=defined_symbols;({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(_Tmp5,({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=symbol;_Tmp6;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}}
# 1793
out_file=_check_null(Cyc_cycstubs_file);
if(cycstubs != 0){
# 1798
({struct Cyc_String_pa_PrintArg_struct _Tmp2=({struct Cyc_String_pa_PrintArg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4=filename;_tag_fat(_Tmp4,sizeof(char),_get_zero_arr_size_char((void*)_Tmp4,1U));});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_fprintf(out_file,({const char*_Tmp4="#include <%s>\n\n";_tag_fat(_Tmp4,sizeof(char),16U);}),_tag_fat(_Tmp3,sizeof(void*),1));});
out_file=_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*x=cycstubs;for(0;x != 0;x=x->tl){
struct _tuple15*_stmttmp23=(struct _tuple15*)x->hd;struct _tuple15*_Tmp2=_stmttmp23;struct _fat_ptr _Tmp3;struct _fat_ptr _Tmp4;_Tmp4=_Tmp2->f1;_Tmp3=_Tmp2->f2;{struct _fat_ptr symbol=_Tmp4;struct _fat_ptr text=_Tmp3;
if((char*)text.curr != (char*)(_tag_fat(0,0,0)).curr &&(
(char*)symbol.curr == (char*)(_tag_fat(0,0,0)).curr ||({struct Cyc_Set_Set*_Tmp5=defined_symbols;({(int(*)(struct Cyc_Set_Set*,struct _fat_ptr*))Cyc_Set_member;})(_Tmp5,({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=symbol;_Tmp6;}));})))
Cyc_fputs((const char*)_untag_fat_ptr(text,sizeof(char),1U),out_file);}}}
# 1806
Cyc_fprintf(out_file,({const char*_Tmp2="\n";_tag_fat(_Tmp2,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}{
# 1809
int _Tmp2=0;_npop_handler(0);return _Tmp2;}}}}}}}}}
# 1439
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3=_Tmp2;void*_Tmp4;struct _fat_ptr _Tmp5;if(((struct Cyc_Core_Impossible_exn_struct*)_Tmp3)->tag == Cyc_Core_Impossible){_Tmp5=((struct Cyc_Core_Impossible_exn_struct*)_Tmp3)->f1;{struct _fat_ptr s=_Tmp5;
# 1813
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_fprintf(Cyc_stderr,({const char*_Tmp8="Got Core::Impossible(%s)\n";_tag_fat(_Tmp8,sizeof(char),26U);}),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL86;}}else{if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp3)->tag == Cyc_Dict_Absent){
# 1815
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Got Dict::Absent\n";_tag_fat(_Tmp6,sizeof(char),18U);}),_tag_fat(0U,sizeof(void*),0));goto _LL86;}else{if(((struct Cyc_Core_Failure_exn_struct*)_Tmp3)->tag == Cyc_Core_Failure){_Tmp5=((struct Cyc_Core_Failure_exn_struct*)_Tmp3)->f1;{struct _fat_ptr s=_Tmp5;
# 1817
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_fprintf(Cyc_stderr,({const char*_Tmp8="Got Core::Failure(%s)\n";_tag_fat(_Tmp8,sizeof(char),23U);}),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL86;}}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_Tmp3)->tag == Cyc_Core_Invalid_argument){_Tmp5=((struct Cyc_Core_Invalid_argument_exn_struct*)_Tmp3)->f1;{struct _fat_ptr s=_Tmp5;
# 1819
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_fprintf(Cyc_stderr,({const char*_Tmp8="Got Invalid_argument(%s)\n";_tag_fat(_Tmp8,sizeof(char),26U);}),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL86;}}else{if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp3)->tag == Cyc_Core_Not_found){
# 1821
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Got Not_found\n";_tag_fat(_Tmp6,sizeof(char),15U);}),_tag_fat(0U,sizeof(void*),0));goto _LL86;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_Tmp3)->tag == Cyc_NO_SUPPORT){_Tmp5=((struct Cyc_NO_SUPPORT_exn_struct*)_Tmp3)->f1;{struct _fat_ptr s=_Tmp5;
# 1823
({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=s;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_fprintf(Cyc_stderr,({const char*_Tmp8="No support because %s\n";_tag_fat(_Tmp8,sizeof(char),23U);}),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL86;}}else{_Tmp4=_Tmp3;{void*x=_Tmp4;
# 1825
Cyc_fprintf(Cyc_stderr,({const char*_Tmp6="Got unknown exception\n";_tag_fat(_Tmp6,sizeof(char),23U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_Core_rethrow(x);}}}}}}}_LL86:;}}}
# 1831
maybe=Cyc_fopen(filename,"w");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not create file %s\n";_tag_fat(_Tmp2,sizeof(char),33U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 1836
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(out_file,({const char*_Tmp2="#error -- %s is not supported on this platform\n";_tag_fat(_Tmp2,sizeof(char),48U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1840
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=filename;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Warning: %s will not be supported on this platform\n";_tag_fat(_Tmp2,sizeof(char),52U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 1843
Cyc_log(({const char*_Tmp0="Not supported on this platform\n";_tag_fat(_Tmp0,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));
# 1850
return 0;}}
# 1854
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=file;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Processing %s\n";_tag_fat(_Tmp2,sizeof(char),15U);}),_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc___cycFILE*maybe=Cyc_fopen(file,"r");
if(!((unsigned)maybe)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=file;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not open %s\n";_tag_fat(_Tmp2,sizeof(char),26U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}{
# 1862
struct Cyc___cycFILE*in_file=maybe;
# 1866
struct _fat_ptr buf=({char*_Tmp0=({unsigned _Tmp1=1024U + 1U;({char*_Tmp2=_cycalloc_atomic(_check_times(_Tmp1,sizeof(char)));({{unsigned _Tmp3=1024U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;});});_tag_fat(_Tmp0,sizeof(char),1025U);});
struct _fat_ptr cwd=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));
if((int)Cyc_mode != 2){
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=dir;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: can't change directory to %s\n";_tag_fat(_Tmp2,sizeof(char),37U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}}
# 1874
if((int)Cyc_mode == 1){
# 1876
struct _fat_ptr cmd=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_cyclone_cc;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=Cyc_target_cflags;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(({const char*_Tmp3="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_fat(_Tmp3,sizeof(char),39U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
# 1878
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cmd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="%s\n";_tag_fat(_Tmp2,sizeof(char),4U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)));}{
# 1883
struct Cyc_Lexing_lexbuf*l=Cyc_Lexing_from_file(in_file);
struct _tuple17*entry;
while((entry=Cyc_spec(l))!= 0){
struct _tuple17*_stmttmp24=
_check_null(entry);
# 1886
struct _tuple17*_Tmp0=_stmttmp24;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;struct _fat_ptr _Tmp8;_Tmp8=_Tmp0->f1;_Tmp7=_Tmp0->f2;_Tmp6=_Tmp0->f3;_Tmp5=_Tmp0->f4;_Tmp4=_Tmp0->f5;_Tmp3=_Tmp0->f6;_Tmp2=_Tmp0->f7;_Tmp1=_Tmp0->f8;{struct _fat_ptr headerfile=_Tmp8;struct Cyc_List_List*start_symbols=_Tmp7;struct Cyc_List_List*user_defs=_Tmp6;struct Cyc_List_List*omit_symbols=_Tmp5;struct Cyc_List_List*hstubs=_Tmp4;struct Cyc_List_List*cstubs=_Tmp3;struct Cyc_List_List*cycstubs=_Tmp2;struct Cyc_List_List*cpp_insert=_Tmp1;
# 1888
if(Cyc_process_file((const char*)_check_null(_untag_fat_ptr(headerfile,sizeof(char),1U)),start_symbols,user_defs,omit_symbols,hstubs,cstubs,cycstubs,cpp_insert))
# 1890
return 1;}}
# 1892
Cyc_fclose(in_file);
# 1894
if((int)Cyc_mode != 2){
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cwd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not change directory to %s\n";_tag_fat(_Tmp2,sizeof(char),41U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}}
# 1900
return 0;}}}}
# 1904
int Cyc_process_setjmp(const char*dir){
# 1907
struct _fat_ptr buf=({char*_Tmp0=({unsigned _Tmp1=1024U + 1U;({char*_Tmp2=_cycalloc_atomic(_check_times(_Tmp1,sizeof(char)));({{unsigned _Tmp3=1024U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]='\000';}_Tmp2[_Tmp3]=0;}0;});_Tmp2;});});_tag_fat(_Tmp0,sizeof(char),1025U);});
struct _fat_ptr cwd=Cyc_getcwd(buf,_get_fat_size(buf,sizeof(char)));
if(chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=({const char*_Tmp2=dir;_tag_fat(_Tmp2,sizeof(char),_get_zero_arr_size_char((void*)_Tmp2,1U));});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: can't change directory to %s\n";_tag_fat(_Tmp2,sizeof(char),37U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 1913
if(({struct Cyc_List_List*_Tmp0=({struct _fat_ptr*_Tmp1[1];({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));*_Tmp3=({const char*_Tmp4="jmp_buf";_tag_fat(_Tmp4,sizeof(char),8U);});_Tmp3;});_Tmp1[0]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(struct _fat_ptr*),1));});Cyc_process_file("setjmp.h",_Tmp0,0,0,({struct _tuple15*_Tmp1[1];({struct _tuple15*_Tmp2=({struct _tuple15*_Tmp3=_cycalloc(sizeof(struct _tuple15));
_Tmp3->f1=({const char*_Tmp4="setjmp";_tag_fat(_Tmp4,sizeof(char),7U);}),_Tmp3->f2=({const char*_Tmp4="extern int setjmp(jmp_buf);\n";_tag_fat(_Tmp4,sizeof(char),29U);});_Tmp3;});_Tmp1[0]=_Tmp2;});Cyc_List_list(_tag_fat(_Tmp1,sizeof(struct _tuple15*),1));}),0,0,0);}))
# 1916
return 1;
if(chdir((const char*)((char*)_check_null(_untag_fat_ptr(cwd,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=cwd;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not change directory to %s\n";_tag_fat(_Tmp2,sizeof(char),41U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 1921
return 0;}static char _TmpG3[13U]="BUILDLIB.OUT";
# 1925
static struct _fat_ptr Cyc_output_dir={_TmpG3,_TmpG3,_TmpG3 + 13U};
static void Cyc_set_output_dir(struct _fat_ptr s){
Cyc_output_dir=s;}
# 1929
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _fat_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=(const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)),_Tmp0->tl=Cyc_spec_files;_Tmp0;});}
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
Cyc_cppargs=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=s;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=Cyc_cppargs;_Tmp0;});}
# 1946
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _fat_ptr s){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Unsupported option %s\n";_tag_fat(_Tmp2,sizeof(char),23U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_badparse=1;}
# 1955
extern void GC_blacklist_warn_clear (void);struct _tuple19{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
int Cyc_main(int argc,struct _fat_ptr argv){
GC_blacklist_warn_clear();{
# 1959
struct Cyc_List_List*options=({struct _tuple19*_Tmp0[9];({
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-d";_tag_fat(_Tmp3,sizeof(char),3U);}),_Tmp2->f2=0,_Tmp2->f3=({const char*_Tmp3=" <file>";_tag_fat(_Tmp3,sizeof(char),8U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_Tmp4->tag=5,_Tmp4->f1=Cyc_set_output_dir;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Set the output directory to <file>";_tag_fat(_Tmp3,sizeof(char),35U);});_Tmp2;});
# 1960
_Tmp0[0]=_Tmp1;}),({
# 1963
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-gather";_tag_fat(_Tmp3,sizeof(char),8U);}),_Tmp2->f2=0,_Tmp2->f3=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_Tmp4->tag=0,_Tmp4->f1=Cyc_set_GATHER;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Gather C library info but don't produce Cyclone headers";_tag_fat(_Tmp3,sizeof(char),56U);});_Tmp2;});
# 1963
_Tmp0[1]=_Tmp1;}),({
# 1966
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-gatherscript";_tag_fat(_Tmp3,sizeof(char),14U);}),_Tmp2->f2=0,_Tmp2->f3=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_Tmp4->tag=0,_Tmp4->f1=Cyc_set_GATHERSCRIPT;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Produce a script to gather C library info";_tag_fat(_Tmp3,sizeof(char),42U);});_Tmp2;});
# 1966
_Tmp0[2]=_Tmp1;}),({
# 1969
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-finish";_tag_fat(_Tmp3,sizeof(char),8U);}),_Tmp2->f2=0,_Tmp2->f3=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Unit_spec_Arg_Spec_struct));_Tmp4->tag=0,_Tmp4->f1=Cyc_set_FINISH;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Produce Cyclone headers from pre-gathered C library info";_tag_fat(_Tmp3,sizeof(char),57U);});_Tmp2;});
# 1969
_Tmp0[3]=_Tmp1;}),({
# 1972
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-setjmp";_tag_fat(_Tmp3,sizeof(char),8U);}),_Tmp2->f2=0,_Tmp2->f3=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_Arg_Spec_struct));_Tmp4->tag=3,_Tmp4->f1=& Cyc_do_setjmp;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_fat(_Tmp3,sizeof(char),186U);});_Tmp2;});
# 1972
_Tmp0[4]=_Tmp1;}),({
# 1978
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-b";_tag_fat(_Tmp3,sizeof(char),3U);}),_Tmp2->f2=0,_Tmp2->f3=({const char*_Tmp3=" <machine>";_tag_fat(_Tmp3,sizeof(char),11U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_String_spec_Arg_Spec_struct));_Tmp4->tag=5,_Tmp4->f1=Cyc_Specsfile_set_target_arch;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Set the target machine for compilation to <machine>";_tag_fat(_Tmp3,sizeof(char),52U);});_Tmp2;});
# 1978
_Tmp0[5]=_Tmp1;}),({
# 1981
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-B";_tag_fat(_Tmp3,sizeof(char),3U);}),_Tmp2->f2=1,_Tmp2->f3=({const char*_Tmp3="<file>";_tag_fat(_Tmp3,sizeof(char),7U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_Tmp4->tag=1,_Tmp4->f1=Cyc_Specsfile_add_cyclone_exec_path;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Add to the list of directories to search for compiler files";_tag_fat(_Tmp3,sizeof(char),60U);});_Tmp2;});
# 1981
_Tmp0[6]=_Tmp1;}),({
# 1984
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-v";_tag_fat(_Tmp3,sizeof(char),3U);}),_Tmp2->f2=0,_Tmp2->f3=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Set_spec_Arg_Spec_struct));_Tmp4->tag=3,_Tmp4->f1=& Cyc_verbose;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="Verbose operation";_tag_fat(_Tmp3,sizeof(char),18U);});_Tmp2;});
# 1984
_Tmp0[7]=_Tmp1;}),({
# 1987
struct _tuple19*_Tmp1=({struct _tuple19*_Tmp2=_cycalloc(sizeof(struct _tuple19));_Tmp2->f1=({const char*_Tmp3="-";_tag_fat(_Tmp3,sizeof(char),2U);}),_Tmp2->f2=1,_Tmp2->f3=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);}),({
void*_Tmp3=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Arg_Flag_spec_Arg_Spec_struct));_Tmp4->tag=1,_Tmp4->f1=Cyc_add_cpparg;_Tmp4;});_Tmp2->f4=_Tmp3;}),_Tmp2->f5=({const char*_Tmp3="";_tag_fat(_Tmp3,sizeof(char),1U);});_Tmp2;});
# 1987
_Tmp0[8]=_Tmp1;});Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _tuple19*),9));});
# 1992
struct _fat_ptr otherargs=Cyc_Specsfile_parse_b(options,Cyc_add_spec_file,Cyc_no_other,({const char*_Tmp0="Options:";_tag_fat(_Tmp0,sizeof(char),9U);}),argv);
# 1994
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_spec_file,Cyc_no_other,({const char*_Tmp0="Options:";_tag_fat(_Tmp0,sizeof(char),9U);}),otherargs);
if((((Cyc_badparse ||
 !Cyc_do_setjmp && Cyc_spec_files == 0)||
 Cyc_do_setjmp && Cyc_spec_files != 0)||
 Cyc_do_setjmp &&(int)Cyc_mode == 1)||
 Cyc_do_setjmp &&(int)Cyc_mode == 2){
Cyc_Arg_usage(options,({const char*_Tmp0="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_fat(_Tmp0,sizeof(char),59U);}));
# 2004
return 1;}{
# 2010
struct _fat_ptr specs_file=Cyc_Specsfile_find_in_arch_path(({const char*_Tmp0="cycspecs";_tag_fat(_Tmp0,sizeof(char),9U);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=specs_file;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Reading from specs file %s\n";_tag_fat(_Tmp2,sizeof(char),28U);}),_tag_fat(_Tmp1,sizeof(void*),1));});{
struct Cyc_List_List*specs=Cyc_Specsfile_read_specs(specs_file);
Cyc_target_cflags=Cyc_Specsfile_get_spec(specs,({const char*_Tmp0="cyclone_target_cflags";_tag_fat(_Tmp0,sizeof(char),22U);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_target_cflags;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Target cflags are %s\n";_tag_fat(_Tmp2,sizeof(char),22U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
Cyc_cyclone_cc=Cyc_Specsfile_get_spec(specs,({const char*_Tmp0="cyclone_cc";_tag_fat(_Tmp0,sizeof(char),11U);}));
if(!((unsigned)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_Tmp0="gcc";_tag_fat(_Tmp0,sizeof(char),4U);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_cyclone_cc;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="C compiler is %s\n";_tag_fat(_Tmp2,sizeof(char),18U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 2019
if((int)Cyc_mode == 2){
if(Cyc_verbose)
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Creating BUILDLIB.sh\n";_tag_fat(_Tmp0,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned)Cyc_script_file)){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="Could not create file BUILDLIB.sh\n";_tag_fat(_Tmp0,sizeof(char),35U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);}
# 2027
Cyc_prscript(({const char*_Tmp0="#!/bin/sh\n";_tag_fat(_Tmp0,sizeof(char),11U);}),_tag_fat(0U,sizeof(void*),0));
Cyc_prscript(({const char*_Tmp0="GCC=\"gcc\"\n";_tag_fat(_Tmp0,sizeof(char),11U);}),_tag_fat(0U,sizeof(void*),0));}
# 2032
if(Cyc_force_directory_prefixes(Cyc_output_dir)|| Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not create directory %s\n";_tag_fat(_Tmp2,sizeof(char),38U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 2036
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Output directory is %s\n";_tag_fat(_Tmp2,sizeof(char),24U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 2039
if((int)Cyc_mode == 2){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_prscript(({const char*_Tmp2="cd %s\n";_tag_fat(_Tmp2,sizeof(char),7U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_target_cflags;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_prscript(({const char*_Tmp2="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_fat(_Tmp2,sizeof(char),41U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}
# 2045
if(!Cyc_gathering()){
# 2048
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_Tmp0="BUILDLIB.LOG";_tag_fat(_Tmp0,sizeof(char),13U);})),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not create log file in directory %s\n";_tag_fat(_Tmp2,sizeof(char),50U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 2054
if(!Cyc_do_setjmp){
# 2056
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_Tmp0="cstubs.c";_tag_fat(_Tmp0,sizeof(char),9U);})),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not create cstubs.c in directory %s\n";_tag_fat(_Tmp2,sizeof(char),50U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;}
# 2063
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(Cyc_Filename_concat(Cyc_output_dir,({const char*_Tmp0="cycstubs.cyc";_tag_fat(_Tmp0,sizeof(char),13U);})),sizeof(char),1U)),"w");
if(!((unsigned)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=Cyc_output_dir;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_fprintf(Cyc_stderr,({const char*_Tmp2="Error: could not create cycstubs.c in directory %s\n";_tag_fat(_Tmp2,sizeof(char),52U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
# 2068
return 1;}
# 2070
Cyc_fprintf(_check_null(Cyc_cycstubs_file),({const char*_Tmp0="#include <core.h>\nusing Core;\n\n";_tag_fat(_Tmp0,sizeof(char),32U);}),_tag_fat(0U,sizeof(void*),0));}}{
# 2077
const char*outdir=(const char*)_check_null(_untag_fat_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp && Cyc_process_setjmp(outdir))
return 1;else{
# 2083
for(1;Cyc_spec_files != 0;Cyc_spec_files=(_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)(_check_null(Cyc_spec_files))->hd,outdir)){
Cyc_fprintf(Cyc_stderr,({const char*_Tmp0="FATAL ERROR -- QUIT!\n";_tag_fat(_Tmp0,sizeof(char),22U);}),_tag_fat(0U,sizeof(void*),0));
exit(1);}}}
# 2091
if((int)Cyc_mode == 2)
Cyc_fclose(_check_null(Cyc_script_file));else{
# 2094
if(!Cyc_gathering()){
Cyc_fclose(_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose(_check_null(Cyc_cstubs_file));
Cyc_fclose(_check_null(Cyc_cycstubs_file));}}}
# 2102
return 0;}}}}}
