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

# 95 "core.h"
 struct _fat_ptr Cyc_Core_new_string(unsigned);struct _tuple0{void*f1;void*f2;};
# 108
void*Cyc_Core_fst(struct _tuple0*);
# 110
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
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
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 165
extern int puts(const char*);
# 167
extern int remove(const char*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 272 "cycboot.h"
extern void Cyc_file_close(struct Cyc___cycFILE*);
# 318
extern int system(const char*);
extern void exit(int);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};
# 78 "lexing.h"
extern struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
extern int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*,struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 109 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Lineno_Pos{struct _fat_ptr logical_file;struct _fat_ptr line;int line_no;int col;};
# 32 "lineno.h"
void Cyc_Lineno_poss_of_abss(struct _fat_ptr,struct Cyc_List_List*);
# 34 "filename.h"
extern struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr);extern char Cyc_Arg_Bad[4U];extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 24 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Doc;
# 53
extern struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*,int);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 52 "absynpp.h"
extern int Cyc_Absynpp_print_for_cycdoc;
# 54
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 60
struct Cyc_PP_Doc*Cyc_Absynpp_decl2doc(struct Cyc_Absyn_Decl*);
# 29 "cycdoc.cyl"
extern void Cyc_Lex_lex_init(int);struct Cyc_MatchDecl_Comment_struct{int tag;struct _fat_ptr f1;};struct Cyc_Standalone_Comment_struct{int tag;struct _fat_ptr f1;};struct _tuple12{int f1;void*f2;};
# 44
struct _tuple12*Cyc_token(struct Cyc_Lexing_lexbuf*);
# 47
const int Cyc_lex_base[15U]={0,- 4,0,- 3,1,2,3,0,4,6,7,- 1,8,9,- 2};
const int Cyc_lex_backtrk[15U]={- 1,- 1,3,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[15U]={1,0,- 1,0,- 1,- 1,6,- 1,8,8,8,0,6,6,0};
const int Cyc_lex_trans[266U]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,6,0,0,0,0,0,0,0,4,5,7,12,9,2,10,10,13,13,0,11,- 1,14,- 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,- 1,- 1,0,- 1,- 1,- 1,- 1};
const int Cyc_lex_check[266U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,7,- 1,5,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,4,5,6,8,0,9,10,12,13,- 1,9,10,12,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,6,8,- 1,9,10,12,13};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 54
int state;int base;int backtrk;
int c;
state=start_state;
# 58
if(state >= 0){
({int _tmp8F=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp8F;});
lbuf->lex_last_action=-1;}else{
# 62
state=(- state)- 1;}
# 64
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,15U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=*((const int*)_check_known_subscript_notnull(Cyc_lex_backtrk,15U,sizeof(int),state));
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 72
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 76
c=256;}}else{
# 78
c=(int)*((char*)_check_fat_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 81
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,266U,sizeof(int),base + c))== state)
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,266U,sizeof(int),base + c));else{
# 84
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_default,15U,sizeof(int),state));}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == -1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp90=({const char*_tmp0="empty token";_tag_fat(_tmp0,sizeof(char),12U);});_tmp1->f1=_tmp90;});_tmp1;}));else{
# 90
return lbuf->lex_last_action;}}else{
# 93
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 97
struct _tuple12*Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp2=lexstate;switch((int)_tmp2){case 0:  {
# 51 "cycdoc.cyl"
int _tmp3=lexbuf->lex_start_pos + 5;int start=_tmp3;
int _tmp4=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 7;int len=_tmp4;
return({struct _tuple12*_tmp6=_cycalloc(sizeof(*_tmp6));({int _tmp93=Cyc_Lexing_lexeme_start(lexbuf);_tmp6->f1=_tmp93;}),({
void*_tmp92=(void*)({struct Cyc_Standalone_Comment_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=1,({struct _fat_ptr _tmp91=Cyc_substring((struct _fat_ptr)lexbuf->lex_buffer,start,(unsigned long)len);_tmp5->f1=_tmp91;});_tmp5;});_tmp6->f2=_tmp92;});_tmp6;});}case 1:  {
# 56
int _tmp7=lexbuf->lex_start_pos + 4;int start=_tmp7;
int _tmp8=(lexbuf->lex_curr_pos - lexbuf->lex_start_pos)- 6;int len=_tmp8;
return({struct _tuple12*_tmpA=_cycalloc(sizeof(*_tmpA));({int _tmp96=Cyc_Lexing_lexeme_start(lexbuf);_tmpA->f1=_tmp96;}),({
void*_tmp95=(void*)({struct Cyc_MatchDecl_Comment_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=0,({struct _fat_ptr _tmp94=Cyc_substring((struct _fat_ptr)lexbuf->lex_buffer,start,(unsigned long)len);_tmp9->f1=_tmp94;});_tmp9;});_tmpA->f2=_tmp95;});_tmpA;});}case 2:
# 60 "cycdoc.cyl"
 return 0;case 3:
# 61 "cycdoc.cyl"
 return Cyc_token(lexbuf);default:
((lexbuf->refill_buff))(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);};}
# 65
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC->tag=Cyc_Lexing_Error,({struct _fat_ptr _tmp97=({const char*_tmpB="some action didn't return!";_tag_fat(_tmpB,sizeof(char),27U);});_tmpC->f1=_tmp97;});_tmpC;}));}
# 67
struct _tuple12*Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,0);}
# 65 "cycdoc.cyl"
static void Cyc_dump_begin (void){
# 67
puts("%%HEVEA \\begin{latexonly}\n\\begin{list}{}{\\setlength\\itemsep{0pt}\\setlength\\topsep{0pt}\\setlength\\leftmargin{\\parindent}\\setlength\\itemindent{-\\leftmargin}}\\item[]\\colorbox{cycdochighlight}{\\ttfamily\\begin{minipage}[b]{\\textwidth}\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}<dl><dt><table><tr><td bgcolor=\"c0d0ff\">\\end{rawhtml}\n\\begin{tabbing}");}
# 81
static void Cyc_dump_middle (void){
puts("\\end{tabbing}\n%%HEVEA \\begin{latexonly}\n\\end{minipage}}\\\\\\strut\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</td></tr></table><dd>\\end{rawhtml}");}
# 88
static void Cyc_dump_end (void){
# 90
puts("%%HEVEA \\begin{latexonly}\n\\end{list}\\smallskip\n%%HEVEA \\end{latexonly}\n%%HEVEA \\begin{rawhtml}</dl>\\end{rawhtml}");}
# 99
static void Cyc_pr_comment(struct Cyc___cycFILE*outf,struct _fat_ptr s){
int depth=0;
int len=(int)Cyc_strlen((struct _fat_ptr)s);
int i=0;for(0;i < len;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c != 91){Cyc_fputc((int)c,outf);continue;}
Cyc_fputs("\\texttt{",outf);
++ i;
++ depth;
for(1;i < len;++ i){
char c=*((const char*)_check_fat_subscript(s,sizeof(char),i));
if((int)c == 93){
-- depth;
if(depth == 0){
Cyc_fputc(125,outf);
break;}}else{
# 117
if((int)c == 91)++ depth;}
Cyc_fputc((int)c,outf);}}}
# 124
static int Cyc_width=50;
static void Cyc_set_width(int w){
Cyc_width=w;}
# 128
static struct Cyc_List_List*Cyc_cycdoc_files=0;
static void Cyc_add_other(struct _fat_ptr s){
Cyc_cycdoc_files=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _fat_ptr*_tmp98=({struct _fat_ptr*_tmpD=_cycalloc(sizeof(*_tmpD));*_tmpD=s;_tmpD;});_tmpE->hd=_tmp98;}),_tmpE->tl=Cyc_cycdoc_files;_tmpE;});}
# 132
static int Cyc_no_other(struct _fat_ptr s){
return 0;}
# 135
static struct Cyc_List_List*Cyc_cycargs=0;
static void Cyc_add_cycarg(struct _fat_ptr s){
Cyc_cycargs=({struct Cyc_List_List*_tmp10=_cycalloc(sizeof(*_tmp10));({struct _fat_ptr*_tmp99=({struct _fat_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));*_tmpF=s;_tmpF;});_tmp10->hd=_tmp99;}),_tmp10->tl=Cyc_cycargs;_tmp10;});}static char _tmp11[8U]="cyclone";
# 139
static struct _fat_ptr Cyc_cyclone_file={_tmp11,_tmp11,_tmp11 + 8U};
static void Cyc_set_cyclone_file(struct _fat_ptr s){
Cyc_cyclone_file=s;}
# 145
static void Cyc_dumpdecl(struct Cyc_Absyn_Decl*d,struct _fat_ptr comment){
Cyc_dump_begin();
# 151
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0,({struct _fat_ptr _tmp9A=(struct _fat_ptr)((struct _fat_ptr)Cyc_PP_string_of_doc(Cyc_Absynpp_decl2doc(d),50));_tmp89.f1=_tmp9A;});_tmp89;});void*_tmp12[1];_tmp12[0]=& _tmp14;({struct _fat_ptr _tmp9B=({const char*_tmp13="%s";_tag_fat(_tmp13,sizeof(char),3U);});Cyc_printf(_tmp9B,_tag_fat(_tmp12,sizeof(void*),1));});});
Cyc_dump_middle();
Cyc_pr_comment(Cyc_stdout,comment);
({void*_tmp15=0U;({struct _fat_ptr _tmp9C=({const char*_tmp16="\n";_tag_fat(_tmp16,sizeof(char),2U);});Cyc_printf(_tmp9C,_tag_fat(_tmp15,sizeof(void*),0));});});
Cyc_dump_end();}
# 159
static int Cyc_is_other_special(char c){
char _tmp17=c;switch((int)_tmp17){case 92:
 goto _LL4;case 34: _LL4:
 goto _LL6;case 59: _LL6:
 goto _LL8;case 38: _LL8:
 goto _LLA;case 40: _LLA:
 goto _LLC;case 41: _LLC:
 goto _LLE;case 124: _LLE:
 goto _LL10;case 94: _LL10:
 goto _LL12;case 60: _LL12:
 goto _LL14;case 62: _LL14:
 goto _LL16;case 32: _LL16:
 goto _LL18;case 10: _LL18:
 goto _LL1A;case 9: _LL1A:
 return 1;default:
 return 0;};}
# 178
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmp18=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp18;
# 182
int _tmp19=0;int single_quotes=_tmp19;
int _tmp1A=0;int other_special=_tmp1A;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmp1B=((const char*)s.curr)[i];char c=_tmp1B;
if((int)c == 39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 191
if(single_quotes == 0 && other_special == 0)
return s;
# 195
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmp1C[3];({struct _fat_ptr*_tmpA1=({struct _fat_ptr*_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct _fat_ptr _tmpA0=({const char*_tmp1D="'";_tag_fat(_tmp1D,sizeof(char),2U);});*_tmp1E=_tmpA0;});_tmp1E;});_tmp1C[0]=_tmpA1;}),({struct _fat_ptr*_tmp9F=({struct _fat_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));*_tmp1F=(struct _fat_ptr)s;_tmp1F;});_tmp1C[1]=_tmp9F;}),({struct _fat_ptr*_tmp9E=({struct _fat_ptr*_tmp21=_cycalloc(sizeof(*_tmp21));({struct _fat_ptr _tmp9D=({const char*_tmp20="'";_tag_fat(_tmp20,sizeof(char),2U);});*_tmp21=_tmp9D;});_tmp21;});_tmp1C[2]=_tmp9E;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp1C,sizeof(struct _fat_ptr*),3));}));{
# 199
unsigned long _tmp22=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmp22;
struct _fat_ptr s2=({unsigned _tmp2D=(len2 + 1U)+ 1U;char*_tmp2C=_cycalloc_atomic(_check_times(_tmp2D,sizeof(char)));({{unsigned _tmp8A=len2 + 1U;unsigned i;for(i=0;i < _tmp8A;++ i){_tmp2C[i]='\000';}_tmp2C[_tmp8A]=0;}0;});_tag_fat(_tmp2C,sizeof(char),_tmp2D);});
int _tmp23=0;int i=_tmp23;
int _tmp24=0;int j=_tmp24;
for(1;(unsigned long)i < len;++ i){
char _tmp25=((const char*)s.curr)[i];char c=_tmp25;
if((int)c == 39 || Cyc_is_other_special(c))
({struct _fat_ptr _tmp26=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp27=*((char*)_check_fat_subscript(_tmp26,sizeof(char),0U));char _tmp28='\\';if(_get_fat_size(_tmp26,sizeof(char))== 1U &&(_tmp27 == 0 && _tmp28 != 0))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});
({struct _fat_ptr _tmp29=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmp2A=*((char*)_check_fat_subscript(_tmp29,sizeof(char),0U));char _tmp2B=c;if(_get_fat_size(_tmp29,sizeof(char))== 1U &&(_tmp2A == 0 && _tmp2B != 0))_throw_arraybounds();*((char*)_tmp29.curr)=_tmp2B;});}
# 209
return(struct _fat_ptr)s2;}}
# 211
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmp2E=_cycalloc(sizeof(*_tmp2E));({struct _fat_ptr _tmpA2=Cyc_sh_escape_string(*sp);*_tmp2E=_tmpA2;});_tmp2E;});}
# 219
static struct Cyc_Lineno_Pos*Cyc_new_pos (void){
return({struct Cyc_Lineno_Pos*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr _tmpA4=({const char*_tmp2F="";_tag_fat(_tmp2F,sizeof(char),1U);});_tmp30->logical_file=_tmpA4;}),({struct _fat_ptr _tmpA3=Cyc_Core_new_string(0U);_tmp30->line=_tmpA3;}),_tmp30->line_no=0,_tmp30->col=0;_tmp30;});}struct _tuple13{int f1;struct Cyc_Lineno_Pos*f2;};
# 222
static struct _tuple13*Cyc_start2pos(int x){
return({struct _tuple13*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->f1=x,({struct Cyc_Lineno_Pos*_tmpA5=Cyc_new_pos();_tmp31->f2=_tmpA5;});_tmp31;});}
# 227
static int Cyc_decl2start(struct Cyc_Absyn_Decl*d){
return(int)d->loc;}struct _tuple14{struct Cyc_Lineno_Pos*f1;void*f2;};
# 232
static struct Cyc_List_List*Cyc_this_file(struct _fat_ptr file,struct Cyc_List_List*x){
# 234
struct Cyc_List_List*_tmp32=0;struct Cyc_List_List*result=_tmp32;
for(1;x != 0;x=x->tl){
if(Cyc_strcmp((struct _fat_ptr)((*((struct _tuple14*)x->hd)).f1)->logical_file,(struct _fat_ptr)file)== 0)
result=({struct Cyc_List_List*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->hd=(struct _tuple14*)x->hd,_tmp33->tl=result;_tmp33;});}
# 239
result=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(result);
return result;}
# 244
static int Cyc_lineno(struct Cyc_Lineno_Pos*p){
return p->line_no;}
# 248
static struct Cyc_List_List*Cyc_flatten_decls(struct Cyc_List_List*decls){
struct Cyc_List_List*_tmp34=0;struct Cyc_List_List*result=_tmp34;
while(decls != 0){
void*_tmp35=((struct Cyc_Absyn_Decl*)decls->hd)->r;void*_stmttmp0=_tmp35;void*_tmp36=_stmttmp0;void*_tmp37;switch(*((int*)_tmp36)){case 9: _tmp37=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp36)->f2;{struct Cyc_List_List*tdl=_tmp37;
_tmp37=tdl;goto _LL4;}case 10: _tmp37=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp36)->f2;_LL4: {struct Cyc_List_List*tdl=_tmp37;
_tmp37=tdl;goto _LL6;}case 11: _tmp37=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp36)->f1;_LL6: {struct Cyc_List_List*tdl=_tmp37;
_tmp37=tdl;goto _LL8;}case 12: _tmp37=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp36)->f1;_LL8: {struct Cyc_List_List*tdl=_tmp37;
# 256
decls=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(tdl,decls->tl);
goto _LL0;}case 5:
 goto _LLC;case 0: _LLC:
 goto _LLE;case 6: _LLE:
 goto _LL10;case 7: _LL10:
 goto _LL12;case 8: _LL12:
 goto _LL14;case 1: _LL14:
 goto _LL16;case 2: _LL16:
 goto _LL18;case 3: _LL18:
 goto _LL1A;case 13: _LL1A:
 goto _LL1C;case 14: _LL1C:
 goto _LL1E;case 15: _LL1E:
 goto _LL20;case 16: _LL20:
 goto _LL22;default: _LL22:
# 271
 result=({struct Cyc_List_List*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->hd=(struct Cyc_Absyn_Decl*)decls->hd,_tmp38->tl=result;_tmp38;});
decls=decls->tl;
goto _LL0;}_LL0:;}
# 276
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(result);}struct _tuple15{int f1;struct Cyc_Absyn_Decl*f2;};struct _tuple16{struct Cyc_Lineno_Pos*f1;struct Cyc_Absyn_Decl*f2;};
# 280
static void Cyc_process_file(struct _fat_ptr filename){
struct _fat_ptr _tmp39=Cyc_Filename_chop_extension(filename);struct _fat_ptr basename=_tmp39;
# 283
const char*_tmp3A=(const char*)_check_null(_untag_fat_ptr(({struct _fat_ptr _tmpA6=(struct _fat_ptr)basename;Cyc_strconcat(_tmpA6,({const char*_tmp6B=".cyp";_tag_fat(_tmp6B,sizeof(char),5U);}));}),sizeof(char),1U));const char*preprocfile=_tmp3A;
struct _fat_ptr _tmp3B=({
struct Cyc_List_List*_tmpAB=({struct Cyc_List_List*_tmp69=_cycalloc(sizeof(*_tmp69));({struct _fat_ptr*_tmpAA=({struct _fat_ptr*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _fat_ptr _tmpA9=(struct _fat_ptr)({const char*_tmp67="";_tag_fat(_tmp67,sizeof(char),1U);});*_tmp68=_tmpA9;});_tmp68;});_tmp69->hd=_tmpAA;}),({
struct Cyc_List_List*_tmpA8=({struct Cyc_List_List*(*_tmpA7)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;});_tmpA7(Cyc_sh_escape_stringptr,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_cycargs));});_tmp69->tl=_tmpA8;});_tmp69;});
# 285
Cyc_str_sepstr(_tmpAB,({const char*_tmp6A=" ";_tag_fat(_tmp6A,sizeof(char),2U);}));});
# 284
struct _fat_ptr cycargs_string=_tmp3B;
# 288
const char*_tmp3C=(const char*)_check_null(_untag_fat_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp8E;_tmp8E.tag=0,_tmp8E.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cyclone_file);_tmp8E;});struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp8D;_tmp8D.tag=0,_tmp8D.f1=(struct _fat_ptr)((struct _fat_ptr)cycargs_string);_tmp8D;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0,({struct _fat_ptr _tmpAC=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(({const char*_tmp66=preprocfile;_tag_fat(_tmp66,sizeof(char),_get_zero_arr_size_char((void*)_tmp66,1U));})));_tmp8C.f1=_tmpAC;});_tmp8C;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0,({struct _fat_ptr _tmpAD=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(filename));_tmp8B.f1=_tmpAD;});_tmp8B;});void*_tmp60[4];_tmp60[0]=& _tmp62,_tmp60[1]=& _tmp63,_tmp60[2]=& _tmp64,_tmp60[3]=& _tmp65;({struct _fat_ptr _tmpAE=({const char*_tmp61="%s %s -E -o %s -x cyc %s";_tag_fat(_tmp61,sizeof(char),25U);});Cyc_aprintf(_tmpAE,_tag_fat(_tmp60,sizeof(void*),4));});}),sizeof(char),1U));const char*cmd=_tmp3C;
# 293
if(system(cmd)!= 0){
({void*_tmp3D=0U;({struct Cyc___cycFILE*_tmpB0=Cyc_stderr;struct _fat_ptr _tmpAF=({const char*_tmp3E="\nError: preprocessing\n";_tag_fat(_tmp3E,sizeof(char),23U);});Cyc_fprintf(_tmpB0,_tmpAF,_tag_fat(_tmp3D,sizeof(void*),0));});});
return;}
# 298
Cyc_Position_reset_position(({const char*_tmp3F=preprocfile;_tag_fat(_tmp3F,sizeof(char),_get_zero_arr_size_char((void*)_tmp3F,1U));}));{
struct Cyc___cycFILE*in_file=_check_null(Cyc_fopen(preprocfile,"r"));
Cyc_Lex_lex_init(1);{
struct Cyc_List_List*_tmp40=Cyc_Parse_parse_file(in_file);struct Cyc_List_List*decls=_tmp40;
Cyc_Lex_lex_init(1);
Cyc_file_close(in_file);
decls=Cyc_flatten_decls(decls);{
# 306
struct Cyc_List_List*_tmp41=({struct Cyc_List_List*(*_tmpB1)(struct _tuple13*(*)(int),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _tuple13*(*)(int),struct Cyc_List_List*))Cyc_List_map;});_tmpB1(Cyc_start2pos,({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Absyn_Decl*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_decl2start,decls));});struct Cyc_List_List*poss=_tmp41;
({struct _fat_ptr _tmpB2=({const char*_tmp42=preprocfile;_tag_fat(_tmp42,sizeof(char),_get_zero_arr_size_char((void*)_tmp42,1U));});Cyc_Lineno_poss_of_abss(_tmpB2,poss);});
remove(preprocfile);{
struct Cyc_List_List*_tmp43=({struct Cyc_List_List*(*_tmpB4)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;});struct Cyc_List_List*_tmpB3=({(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Lineno_Pos*(*)(struct _tuple13*))Cyc_Core_snd;}),poss);_tmpB4(_tmpB3,decls);});struct Cyc_List_List*pos_decls=_tmp43;
# 311
pos_decls=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*))Cyc_this_file;})(filename,pos_decls);{
struct Cyc_List_List*_tmp44=({struct Cyc_List_List*(*_tmpB7)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;});struct Cyc_List_List*_tmpB6=({struct Cyc_List_List*(*_tmpB5)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*))Cyc_List_map;});_tmpB5(Cyc_lineno,({(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Lineno_Pos*(*)(struct _tuple16*))Cyc_Core_fst;}),pos_decls));});_tmpB7(_tmpB6,
({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Decl*(*)(struct _tuple16*))Cyc_Core_snd;}),pos_decls));});
# 312
struct Cyc_List_List*lineno_decls=_tmp44;
# 316
struct Cyc___cycFILE*_tmp45=_check_null(Cyc_fopen((const char*)_check_null(_untag_fat_ptr(filename,sizeof(char),1U)),"r"));struct Cyc___cycFILE*f=_tmp45;
struct Cyc_Lexing_lexbuf*_tmp46=Cyc_Lexing_from_file(f);struct Cyc_Lexing_lexbuf*lb=_tmp46;
struct Cyc_List_List*_tmp47=0;struct Cyc_List_List*comments=_tmp47;
struct Cyc_List_List*_tmp48=0;struct Cyc_List_List*indices=_tmp48;
struct _tuple12*tok;
while((tok=({(struct _tuple12*(*)(struct Cyc_Lexing_lexbuf*))Cyc_token;})(lb))!= 0){
struct _tuple12 _tmp49=*_check_null(tok);struct _tuple12 _stmttmp1=_tmp49;struct _tuple12 _tmp4A=_stmttmp1;void*_tmp4C;int _tmp4B;_tmp4B=_tmp4A.f1;_tmp4C=_tmp4A.f2;{int index=_tmp4B;void*comment=_tmp4C;
comments=({struct Cyc_List_List*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->hd=comment,_tmp4D->tl=comments;_tmp4D;});
indices=({struct Cyc_List_List*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->hd=(void*)index,_tmp4E->tl=indices;_tmp4E;});}}
# 326
Cyc_fclose(f);
comments=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(comments);
indices=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(indices);{
# 330
struct Cyc_List_List*_tmp4F=({(struct Cyc_List_List*(*)(struct _tuple13*(*)(int),struct Cyc_List_List*))Cyc_List_map;})(Cyc_start2pos,indices);struct Cyc_List_List*poss=_tmp4F;
Cyc_Lineno_poss_of_abss(filename,poss);{
struct Cyc_List_List*_tmp50=({struct Cyc_List_List*(*_tmpBA)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_zip;});struct Cyc_List_List*_tmpB9=({struct Cyc_List_List*(*_tmpB8)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(int(*)(struct Cyc_Lineno_Pos*),struct Cyc_List_List*))Cyc_List_map;});_tmpB8(Cyc_lineno,({(struct Cyc_List_List*(*)(struct Cyc_Lineno_Pos*(*)(struct _tuple13*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Lineno_Pos*(*)(struct _tuple13*))Cyc_Core_snd;}),poss));});_tmpBA(_tmpB9,comments);});struct Cyc_List_List*lineno_comments=_tmp50;
# 337
while(lineno_decls != 0 && lineno_comments != 0){
struct _tuple15*_tmp51=(struct _tuple15*)lineno_decls->hd;struct _tuple15*_stmttmp2=_tmp51;struct _tuple15*_tmp52=_stmttmp2;void*_tmp54;int _tmp53;_tmp53=_tmp52->f1;_tmp54=_tmp52->f2;{int dline=_tmp53;struct Cyc_Absyn_Decl*decl=_tmp54;
struct _tuple12*_tmp55=(struct _tuple12*)lineno_comments->hd;struct _tuple12*_stmttmp3=_tmp55;struct _tuple12*_tmp56=_stmttmp3;void*_tmp58;int _tmp57;_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;{int cline=_tmp57;void*comment=_tmp58;
void*_tmp59=comment;struct _fat_ptr _tmp5A;if(*((int*)_tmp59)== 1){_tmp5A=((struct Cyc_Standalone_Comment_struct*)_tmp59)->f1;{struct _fat_ptr cmt=_tmp5A;
# 342
Cyc_pr_comment(Cyc_stdout,(struct _fat_ptr)cmt);
({void*_tmp5B=0U;({struct _fat_ptr _tmpBB=({const char*_tmp5C="\n";_tag_fat(_tmp5C,sizeof(char),2U);});Cyc_printf(_tmpBB,_tag_fat(_tmp5B,sizeof(void*),0));});});
lineno_comments=lineno_comments->tl;
goto _LL9;}}else{_tmp5A=((struct Cyc_MatchDecl_Comment_struct*)_tmp59)->f1;{struct _fat_ptr cmt=_tmp5A;
# 347
if(cline < dline){
# 349
lineno_comments=lineno_comments->tl;
continue;}
# 352
if(lineno_decls->tl != 0){
struct _tuple15*_tmp5D=(struct _tuple15*)(_check_null(lineno_decls->tl))->hd;struct _tuple15*_stmttmp4=_tmp5D;struct _tuple15*_tmp5E=_stmttmp4;int _tmp5F;_tmp5F=_tmp5E->f1;{int dline2=_tmp5F;
if(dline2 < cline){
# 356
lineno_decls=lineno_decls->tl;
continue;}}}
# 362
Cyc_dumpdecl(decl,(struct _fat_ptr)cmt);
Cyc_fflush(Cyc_stdout);
lineno_decls=lineno_decls->tl;
lineno_comments=lineno_comments->tl;
goto _LL9;}}_LL9:;}}}}}}}}}}}
# 372
extern void GC_blacklist_warn_clear (void);struct _tuple17{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 374
int Cyc_main(int argc,struct _fat_ptr argv){
# 379
GC_blacklist_warn_clear();{
# 381
struct Cyc_List_List*options=({struct _tuple17*_tmp6F[5];({
struct _tuple17*_tmpD4=({struct _tuple17*_tmp74=_cycalloc(sizeof(*_tmp74));({struct _fat_ptr _tmpD3=({const char*_tmp70="-cyclone";_tag_fat(_tmp70,sizeof(char),9U);});_tmp74->f1=_tmpD3;}),_tmp74->f2=0,({struct _fat_ptr _tmpD2=({const char*_tmp71=" <file>";_tag_fat(_tmp71,sizeof(char),8U);});_tmp74->f3=_tmpD2;}),({
void*_tmpD1=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72->tag=5,_tmp72->f1=Cyc_set_cyclone_file;_tmp72;});_tmp74->f4=_tmpD1;}),({
struct _fat_ptr _tmpD0=({const char*_tmp73="Use <file> as the cyclone compiler";_tag_fat(_tmp73,sizeof(char),35U);});_tmp74->f5=_tmpD0;});_tmp74;});
# 382
_tmp6F[0]=_tmpD4;}),({
# 385
struct _tuple17*_tmpCF=({struct _tuple17*_tmp79=_cycalloc(sizeof(*_tmp79));({struct _fat_ptr _tmpCE=({const char*_tmp75="-w";_tag_fat(_tmp75,sizeof(char),3U);});_tmp79->f1=_tmpCE;}),_tmp79->f2=0,({struct _fat_ptr _tmpCD=({const char*_tmp76=" <width>";_tag_fat(_tmp76,sizeof(char),9U);});_tmp79->f3=_tmpCD;}),({
void*_tmpCC=(void*)({struct Cyc_Arg_Int_spec_Arg_Spec_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=6,_tmp77->f1=Cyc_set_width;_tmp77;});_tmp79->f4=_tmpCC;}),({
struct _fat_ptr _tmpCB=({const char*_tmp78="Use <width> as the max width for printing declarations";_tag_fat(_tmp78,sizeof(char),55U);});_tmp79->f5=_tmpCB;});_tmp79;});
# 385
_tmp6F[1]=_tmpCF;}),({
# 388
struct _tuple17*_tmpCA=({struct _tuple17*_tmp7E=_cycalloc(sizeof(*_tmp7E));({struct _fat_ptr _tmpC9=({const char*_tmp7A="-D";_tag_fat(_tmp7A,sizeof(char),3U);});_tmp7E->f1=_tmpC9;}),_tmp7E->f2=1,({struct _fat_ptr _tmpC8=({const char*_tmp7B="<name>[=<value>]";_tag_fat(_tmp7B,sizeof(char),17U);});_tmp7E->f3=_tmpC8;}),({
void*_tmpC7=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=1,_tmp7C->f1=Cyc_add_cycarg;_tmp7C;});_tmp7E->f4=_tmpC7;}),({
struct _fat_ptr _tmpC6=({const char*_tmp7D="Pass definition to preprocessor";_tag_fat(_tmp7D,sizeof(char),32U);});_tmp7E->f5=_tmpC6;});_tmp7E;});
# 388
_tmp6F[2]=_tmpCA;}),({
# 391
struct _tuple17*_tmpC5=({struct _tuple17*_tmp83=_cycalloc(sizeof(*_tmp83));({struct _fat_ptr _tmpC4=({const char*_tmp7F="-I";_tag_fat(_tmp7F,sizeof(char),3U);});_tmp83->f1=_tmpC4;}),_tmp83->f2=1,({struct _fat_ptr _tmpC3=({const char*_tmp80="<dir>";_tag_fat(_tmp80,sizeof(char),6U);});_tmp83->f3=_tmpC3;}),({
void*_tmpC2=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=1,_tmp81->f1=Cyc_add_cycarg;_tmp81;});_tmp83->f4=_tmpC2;}),({
struct _fat_ptr _tmpC1=({const char*_tmp82="Add to the list of directories to search for include files";_tag_fat(_tmp82,sizeof(char),59U);});_tmp83->f5=_tmpC1;});_tmp83;});
# 391
_tmp6F[3]=_tmpC5;}),({
# 394
struct _tuple17*_tmpC0=({struct _tuple17*_tmp88=_cycalloc(sizeof(*_tmp88));({struct _fat_ptr _tmpBF=({const char*_tmp84="-B";_tag_fat(_tmp84,sizeof(char),3U);});_tmp88->f1=_tmpBF;}),_tmp88->f2=1,({struct _fat_ptr _tmpBE=({const char*_tmp85="<file>";_tag_fat(_tmp85,sizeof(char),7U);});_tmp88->f3=_tmpBE;}),({
void*_tmpBD=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=1,_tmp86->f1=Cyc_add_cycarg;_tmp86;});_tmp88->f4=_tmpBD;}),({
struct _fat_ptr _tmpBC=({const char*_tmp87="Add to the list of directories to search for compiler files";_tag_fat(_tmp87,sizeof(char),60U);});_tmp88->f5=_tmpBC;});_tmp88;});
# 394
_tmp6F[4]=_tmpC0;});({
# 382
(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmp6F,sizeof(struct _tuple17*),5));});
# 399
({struct Cyc_List_List*_tmpD6=options;struct _fat_ptr _tmpD5=({const char*_tmp6C="Options:";_tag_fat(_tmp6C,sizeof(char),9U);});Cyc_Arg_parse(_tmpD6,Cyc_add_other,Cyc_no_other,_tmpD5,argv);});
# 401
if(Cyc_cycdoc_files == 0){
({struct Cyc_List_List*_tmpD7=options;Cyc_Arg_usage(_tmpD7,({const char*_tmp6D="Usage: cycdoc [options] file1 file2 ...\nOptions:";_tag_fat(_tmp6D,sizeof(char),49U);}));});
# 405
exit(1);}
# 410
Cyc_PP_tex_output=1;
# 412
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Absynpp_print_for_cycdoc=1;
# 415
{struct Cyc_List_List*_tmp6E=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_cycdoc_files);struct Cyc_List_List*l=_tmp6E;for(0;l != 0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));}}
# 418
return 0;}}
