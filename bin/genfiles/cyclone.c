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
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;
# 292 "core.h"
void Cyc_Core_rethrow(void*);
# 296
const char*Cyc_Core_get_exn_name(void*);
# 298
const char*Cyc_Core_get_exn_filename (void);
# 305
int Cyc_Core_get_exn_lineno (void);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);extern char Cyc_Arg_Bad[4U];extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 84
extern int Cyc_feof(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 108
extern unsigned long Cyc_fread(struct _fat_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 140 "cycboot.h"
extern unsigned long Cyc_fwrite(struct _fat_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 167
extern int remove(const char*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 271 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_file_open(struct _fat_ptr,struct _fat_ptr);
extern void Cyc_file_close(struct Cyc___cycFILE*);
# 313
extern char*getenv(const char*);
# 318
extern int system(const char*);
extern void exit(int);struct Cyc_timeval{long tv_sec;long tv_usec;};
# 328
extern int gettimeofday(struct Cyc_timeval*,void*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 51
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
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);
# 120
extern struct _fat_ptr Cyc_strchr(struct _fat_ptr,char);
# 30 "filename.h"
extern struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr,struct _fat_ptr);
# 34
extern struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr);
# 40
extern struct _fat_ptr Cyc_Filename_dirname(struct _fat_ptr);
# 43
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);
# 46
extern int Cyc_Filename_check_suffix(struct _fat_ptr,struct _fat_ptr);
# 27 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);
# 45
extern int Cyc_Position_use_gcc_style_location;
# 47
extern int Cyc_Position_max_errors;
# 49
extern int Cyc_Position_error_p (void);
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};
# 391 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};
# 464
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 471
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};
# 489
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};extern char Cyc_Absyn_EmptyAnnot[11U];
# 24 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 26 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*,struct Cyc___cycFILE*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];extern char Cyc_Dict_Absent[7U];
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 38
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*);
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 33
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 35 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*,struct Cyc_List_List*);extern char Cyc_Toc_Dest[5U];
# 48
void Cyc_Toc_finish (void);
# 26 "remove_aggregates.h"
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*);
# 25 "remove_labels.h"
struct Cyc_List_List*Cyc_RemoveLabels_remove_unused_labels(struct Cyc_List_List*);
# 27 "toseqc.h"
extern struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*);
# 27 "low_treeshake.h"
struct Cyc_List_List*Cyc_LowTreeShake_shake(struct Cyc_List_List*);extern char Cyc_Set_Absent[7U];struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*);
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];extern char Cyc_CfFlowInfo_NotZero[8U];extern char Cyc_CfFlowInfo_UnknownZ[9U];
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*);
# 31 "warn.h"
void Cyc_Warn_flush_warnings (void);
# 80 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};extern char Cyc_InsertChecks_FatBound[9U];extern char Cyc_InsertChecks_NoCheck[8U];extern char Cyc_InsertChecks_NullAndFatBound[16U];extern char Cyc_InsertChecks_NullAndThinBound[17U];extern char Cyc_InsertChecks_NullOnly[9U];extern char Cyc_InsertChecks_ThinBound[10U];
# 40 "insert_checks.h"
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*,int);struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty (void);
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final (void);
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple12{struct _fat_ptr f1;struct _fat_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*,struct Cyc_Interface_I*,struct _tuple12*);
# 72 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*)(void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 26 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 30
extern int Cyc_Flags_no_regions;
# 34
extern int Cyc_Flags_no_register;
# 37
extern int Cyc_Flags_warn_override;
# 47
extern int Cyc_Flags_warn_alias_coerce;
# 51
extern int Cyc_Flags_warn_region_coerce;
# 54
extern int Cyc_Flags_warn_lose_unique;
# 58
extern int Cyc_Flags_warn_bounds_checks;
# 62
extern int Cyc_Flags_warn_all_null_deref;
# 66
extern int Cyc_Flags_noexpand_r;
# 71
extern int Cyc_Flags_allpaths;
# 77
extern int Cyc_Flags_tovc_r;
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 82
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 92 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 109
extern enum Cyc_Flags_Cyclone_Passes Cyc_Flags_stop_after_pass;
# 111
void Cyc_Flags_set_cpponly (void);
void Cyc_Flags_set_parseonly (void);
void Cyc_Flags_set_crgnonly (void);
void Cyc_Flags_set_tconly (void);
void Cyc_Flags_set_cfonly (void);
void Cyc_Flags_set_toconly (void);
# 122
extern int Cyc_Flags_compile_for_boot;
# 126
extern int Cyc_Flags_print_parser_state_and_token;
# 129
void Cyc_Flags_set_all_warnings (void);
# 29 "specsfile.h"
struct _fat_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_split_specs(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*,struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _fat_ptr Cyc_Specsfile_def_lib_path;
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 44
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_find_in_exec_path(struct _fat_ptr);
# 38 "cifc.h"
void Cyc_Cifc_set_inst_tvar (void);
# 45 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 48
void Cyc_Lex_pos_init (void);
void Cyc_Lex_lex_init(int);
# 56
static int Cyc_pp_r=0;
static int Cyc_noshake_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
static int Cyc_toseqc_r=1;
static int Cyc_pthread_r=0;
static int Cyc_vcgen_r=1;
# 83
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _fat_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));({struct _fat_ptr*_tmp2CF=({struct _fat_ptr*_tmp0=_cycalloc(sizeof(*_tmp0));*_tmp0=s;_tmp0;});_tmp1->hd=_tmp2CF;}),_tmp1->tl=Cyc_ccargs;_tmp1;});}
# 91
void Cyc_set_c_compiler(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2D0=(struct _fat_ptr)s;Cyc_strcmp(_tmp2D0,({const char*_tmp2="vc";_tag_fat(_tmp2,sizeof(char),3U);}));})== 0){
Cyc_Flags_c_compiler=1U;
Cyc_add_ccarg(({const char*_tmp3="-DVC_C";_tag_fat(_tmp3,sizeof(char),7U);}));}else{
# 96
if(({struct _fat_ptr _tmp2D1=(struct _fat_ptr)s;Cyc_strcmp(_tmp2D1,({const char*_tmp4="gcc";_tag_fat(_tmp4,sizeof(char),4U);}));})== 0){
Cyc_Flags_c_compiler=0U;
Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";_tag_fat(_tmp5,sizeof(char),8U);}));}}}
# 103
void Cyc_set_port_c_code (void){
Cyc_Flags_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_Flags_set_parseonly();}
# 110
static struct _fat_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _fat_ptr s){
Cyc_output_file=({struct _fat_ptr*_tmp6=_cycalloc(sizeof(*_tmp6));*_tmp6=s;_tmp6;});}
# 115
static struct _fat_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _fat_ptr s){
Cyc_specified_interface=s;}
# 125
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Cversion;static char _tmp7[1U]="";
# 130
static struct _fat_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1U};
static void Cyc_set_cpp(struct _fat_ptr s){
Cyc_cpp=s;}
# 135
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));({struct _fat_ptr*_tmp2D2=({struct _fat_ptr*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=s;_tmp8;});_tmp9->hd=_tmp2D2;}),_tmp9->tl=Cyc_cppargs;_tmp9;});}
# 140
static void Cyc_add_cpp_and_ccarg(struct _fat_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 145
static void Cyc_add_iprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0,_tmp297.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp297;});void*_tmpA[1];_tmpA[0]=& _tmpC;({struct _fat_ptr _tmp2D3=({const char*_tmpB="-iprefix %s";_tag_fat(_tmpB,sizeof(char),12U);});Cyc_aprintf(_tmp2D3,_tag_fat(_tmpA,sizeof(void*),1));});}));}
# 148
static void Cyc_add_iwithprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp298;_tmp298.tag=0,_tmp298.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp298;});void*_tmpD[1];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp2D4=({const char*_tmpE="-iwithprefix %s";_tag_fat(_tmpE,sizeof(char),16U);});Cyc_aprintf(_tmp2D4,_tag_fat(_tmpD,sizeof(void*),1));});}));}
# 151
static void Cyc_add_iwithprefixbefore(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp299;_tmp299.tag=0,_tmp299.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp299;});void*_tmp10[1];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp2D5=({const char*_tmp11="-iwithprefixbefore %s";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp2D5,_tag_fat(_tmp10,sizeof(void*),1));});}));}
# 154
static void Cyc_add_isystem(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp29A;_tmp29A.tag=0,_tmp29A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp29A;});void*_tmp13[1];_tmp13[0]=& _tmp15;({struct _fat_ptr _tmp2D6=({const char*_tmp14="-isystem %s";_tag_fat(_tmp14,sizeof(char),12U);});Cyc_aprintf(_tmp2D6,_tag_fat(_tmp13,sizeof(void*),1));});}));}
# 157
static void Cyc_add_idirafter(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp29B;_tmp29B.tag=0,_tmp29B.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp29B;});void*_tmp16[1];_tmp16[0]=& _tmp18;({struct _fat_ptr _tmp2D7=({const char*_tmp17="-idirafter %s";_tag_fat(_tmp17,sizeof(char),14U);});Cyc_aprintf(_tmp2D7,_tag_fat(_tmp16,sizeof(void*),1));});}));}
# 161
static void Cyc_set_many_errors (void){
Cyc_Position_max_errors=65535;}static char _tmp19[20U]="_see_cycspecs_file_";
# 165
static struct _fat_ptr Cyc_def_inc_path={_tmp19,_tmp19,_tmp19 + 20U};
# 168
static void Cyc_print_version (void){
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp29C;_tmp29C.tag=0,({struct _fat_ptr _tmp2D8=(struct _fat_ptr)({char*_tmp1D=Cversion;_tag_fat(_tmp1D,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D,1U));});_tmp29C.f1=_tmp2D8;});_tmp29C;});void*_tmp1A[1];_tmp1A[0]=& _tmp1C;({struct _fat_ptr _tmp2D9=({const char*_tmp1B="The Cyclone compiler, version %s\n";_tag_fat(_tmp1B,sizeof(char),34U);});Cyc_printf(_tmp2D9,_tag_fat(_tmp1A,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0,({struct _fat_ptr _tmp2DA=(struct _fat_ptr)({char*_tmp21=Carch;_tag_fat(_tmp21,sizeof(char),_get_zero_arr_size_char((void*)_tmp21,1U));});_tmp29D.f1=_tmp2DA;});_tmp29D;});void*_tmp1E[1];_tmp1E[0]=& _tmp20;({struct _fat_ptr _tmp2DB=({const char*_tmp1F="Compiled for architecture: %s\n";_tag_fat(_tmp1F,sizeof(char),31U);});Cyc_printf(_tmp2DB,_tag_fat(_tmp1E,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0,({struct _fat_ptr _tmp2DC=(struct _fat_ptr)({char*_tmp25=Cdef_lib_path;_tag_fat(_tmp25,sizeof(char),_get_zero_arr_size_char((void*)_tmp25,1U));});_tmp29E.f1=_tmp2DC;});_tmp29E;});void*_tmp22[1];_tmp22[0]=& _tmp24;({struct _fat_ptr _tmp2DD=({const char*_tmp23="Standard library directory: %s\n";_tag_fat(_tmp23,sizeof(char),32U);});Cyc_printf(_tmp2DD,_tag_fat(_tmp22,sizeof(void*),1));});});
# 173
({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp29F;_tmp29F.tag=0,_tmp29F.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_def_inc_path);_tmp29F;});void*_tmp26[1];_tmp26[0]=& _tmp28;({struct _fat_ptr _tmp2DE=({const char*_tmp27="Standard include directory: %s\n";_tag_fat(_tmp27,sizeof(char),32U);});Cyc_printf(_tmp2DE,_tag_fat(_tmp26,sizeof(void*),1));});});
exit(0);}
# 177
static int Cyc_is_cyclone_sourcefile(struct _fat_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp29;
if(len <= 4U)return 0;
return({struct _fat_ptr _tmp2DF=(struct _fat_ptr)_fat_ptr_plus(s,sizeof(char),(int)(len - 4U));Cyc_strcmp(_tmp2DF,({const char*_tmp2A=".cyc";_tag_fat(_tmp2A,sizeof(char),5U);}));})== 0;}
# 193 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2E0=(struct _fat_ptr)s;Cyc_strcmp(_tmp2E0,({const char*_tmp2B="-lxml";_tag_fat(_tmp2B,sizeof(char),6U);}));})== 0)
Cyc_add_ccarg(Cyc_pa_r?({const char*_tmp2C="-lxml_a";_tag_fat(_tmp2C,sizeof(char),8U);}): s);else{
if(({struct _fat_ptr _tmp2E1=(struct _fat_ptr)s;Cyc_strncmp(_tmp2E1,({const char*_tmp2D="-lcyc";_tag_fat(_tmp2D,sizeof(char),6U);}),5U);})== 0)
Cyc_add_ccarg(s);else{
# 200
if(({struct _fat_ptr _tmp2E2=(struct _fat_ptr)s;Cyc_strncmp(_tmp2E2,({const char*_tmp2E="-lpthread";_tag_fat(_tmp2E,sizeof(char),10U);}),9U);})== 0)
Cyc_pthread_r=1;
Cyc_libargs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr*_tmp2E3=({struct _fat_ptr*_tmp2F=_cycalloc(sizeof(*_tmp2F));*_tmp2F=s;_tmp2F;});_tmp30->hd=_tmp2E3;}),_tmp30->tl=Cyc_libargs;_tmp30;});}}}
# 206
static void Cyc_add_marg(struct _fat_ptr s){
Cyc_add_ccarg(s);}
# 210
static void Cyc_set_save_temps (void){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_tmp31="-save-temps";_tag_fat(_tmp31,sizeof(char),12U);}));}
# 215
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies (void){
Cyc_Flags_stop_after_pass=0U;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_tmp32="-M";_tag_fat(_tmp32,sizeof(char),3U);}));}
# 222
static struct _fat_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _fat_ptr s){
Cyc_dependencies_target=({struct _fat_ptr*_tmp33=_cycalloc(sizeof(*_tmp33));*_tmp33=s;_tmp33;});}
# 227
static void Cyc_set_stop_after_objectfile (void){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_tmp34="-c";_tag_fat(_tmp34,sizeof(char),3U);}));}
# 232
static void Cyc_set_nocppprecomp (void){
Cyc_add_cpp_and_ccarg(({const char*_tmp35="-no-cpp-precomp";_tag_fat(_tmp35,sizeof(char),16U);}));}
# 236
static void Cyc_clear_lineno (void){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 240
static void Cyc_set_elim_se (void){
Cyc_elim_se_r=1;}
# 243
static void Cyc_set_tovc (void){
Cyc_Flags_tovc_r=1;
Cyc_add_ccarg(({const char*_tmp36="-DCYC_ANSI_OUTPUT";_tag_fat(_tmp36,sizeof(char),18U);}));
Cyc_set_elim_se();}
# 248
static void Cyc_set_notoseqc (void){
Cyc_toseqc_r=0;}
# 251
static void Cyc_set_noboundschecks (void){
Cyc_add_ccarg(({const char*_tmp37="-DNO_CYC_BOUNDS_CHECKS";_tag_fat(_tmp37,sizeof(char),23U);}));}
# 254
static void Cyc_set_nonullchecks (void){
Cyc_add_ccarg(({const char*_tmp38="-DNO_CYC_NULL_CHECKS";_tag_fat(_tmp38,sizeof(char),21U);}));}
# 257
static void Cyc_set_nochecks (void){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 263
static void Cyc_set_nocyc (void){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_tmp39="-DNO_CYC_PREFIX";_tag_fat(_tmp39,sizeof(char),16U);}));}
# 268
static void Cyc_set_pa (void){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_tmp3A="-DCYC_REGION_PROFILE";_tag_fat(_tmp3A,sizeof(char),21U);}));
Cyc_add_cpparg(({const char*_tmp3B="-DCYC_REGION_PROFILE";_tag_fat(_tmp3B,sizeof(char),21U);}));}
# 274
static void Cyc_set_pg (void){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_tmp3C="-pg";_tag_fat(_tmp3C,sizeof(char),4U);}));}
# 279
static void Cyc_set_stop_after_asmfile (void){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_tmp3D="-S";_tag_fat(_tmp3D,sizeof(char),3U);}));}
# 285
enum Cyc_inputtype{Cyc_DEFAULTINPUT =0U,Cyc_CYCLONEFILE =1U};
# 289
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2E4=(struct _fat_ptr)s;Cyc_strcmp(_tmp2E4,({const char*_tmp3E="cyc";_tag_fat(_tmp3E,sizeof(char),4U);}));})== 0)
Cyc_intype=1U;else{
if(({struct _fat_ptr _tmp2E5=(struct _fat_ptr)s;Cyc_strcmp(_tmp2E5,({const char*_tmp3F="none";_tag_fat(_tmp3F,sizeof(char),5U);}));})== 0)
Cyc_intype=0U;else{
# 296
({struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0,_tmp2A0.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2A0;});void*_tmp40[1];_tmp40[0]=& _tmp42;({struct Cyc___cycFILE*_tmp2E7=Cyc_stderr;struct _fat_ptr _tmp2E6=({const char*_tmp41="Input type '%s' not supported\n";_tag_fat(_tmp41,sizeof(char),31U);});Cyc_fprintf(_tmp2E7,_tmp2E6,_tag_fat(_tmp40,sizeof(void*),1));});});}}}
# 299
struct _fat_ptr Cyc_make_base_filename(struct _fat_ptr s,struct _fat_ptr*output_file){
struct _fat_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct _fat_ptr outdir=_tmp43;
struct _fat_ptr _tmp44=Cyc_Filename_chop_extension((struct _fat_ptr)Cyc_Filename_basename(s));struct _fat_ptr base=_tmp44;
# 305
struct _fat_ptr _tmp45=Cyc_strlen((struct _fat_ptr)outdir)> 0U?Cyc_Filename_concat((struct _fat_ptr)outdir,(struct _fat_ptr)base): base;struct _fat_ptr basename=_tmp45;
return(struct _fat_ptr)basename;}
# 311
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 313
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _fat_ptr s){
# 316
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
if((int)Cyc_intype == 1 || Cyc_is_cyclone_sourcefile(s)){
# 321
Cyc_cyclone_files=({struct Cyc_List_List*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _fat_ptr*_tmp2E8=({struct _fat_ptr*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=s;_tmp46;});_tmp47->hd=_tmp2E8;}),_tmp47->tl=Cyc_cyclone_files;_tmp47;});{
# 327
struct _fat_ptr _tmp48=Cyc_make_base_filename(s,0);struct _fat_ptr basename=_tmp48;
struct _fat_ptr _tmp49=({struct _fat_ptr _tmp2E9=(struct _fat_ptr)basename;Cyc_strconcat(_tmp2E9,({const char*_tmp4A=".c";_tag_fat(_tmp4A,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmp49;
Cyc_add_ccarg((struct _fat_ptr)cfile);}}else{
# 331
Cyc_add_ccarg(s);}}}
# 336
static int Cyc_assume_gcc_flag=1;struct _tuple13{struct _fat_ptr flag;int optargs;};static char _tmp4B[3U]="-s";static char _tmp4C[3U]="-O";static char _tmp4D[4U]="-O0";static char _tmp4E[4U]="-O2";static char _tmp4F[4U]="-O3";static char _tmp50[21U]="-fomit-frame-pointer";static char _tmp51[13U]="-fno-builtin";static char _tmp52[3U]="-g";static char _tmp53[3U]="-p";static char _tmp54[8U]="-static";
static int Cyc_add_other_flag(struct _fat_ptr s){
static struct _tuple13 known_gcc_flags[10U]={{.flag={_tmp4B,_tmp4B,_tmp4B + 3U},.optargs=0},{.flag={_tmp4C,_tmp4C,_tmp4C + 3U},.optargs=0},{.flag={_tmp4D,_tmp4D,_tmp4D + 4U},.optargs=0},{.flag={_tmp4E,_tmp4E,_tmp4E + 4U},.optargs=0},{.flag={_tmp4F,_tmp4F,_tmp4F + 4U},.optargs=0},{.flag={_tmp50,_tmp50,_tmp50 + 21U},.optargs=0},{.flag={_tmp51,_tmp51,_tmp51 + 13U},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 3U},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 3U},.optargs=0},{.flag={_tmp54,_tmp54,_tmp54 + 8U},.optargs=0}};
# 350
if(Cyc_assume_gcc_flag){
Cyc_add_ccarg(s);
return 1;}else{
# 354
{int i=0;for(0;(unsigned)i < 10U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)(known_gcc_flags[i]).flag,(struct _fat_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(*((struct _tuple13*)_check_known_subscript_notnull(known_gcc_flags,10U,sizeof(struct _tuple13),i))).optargs;
break;}}}
# 360
return 0;}}
# 364
static void Cyc_noassume_gcc_flag (void){
Cyc_assume_gcc_flag=0;}
# 368
static void Cyc_remove_file(struct _fat_ptr s){
if(!Cyc_save_temps_r)
remove((const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}
# 375
int Cyc_compile_failure=0;
# 377
struct Cyc___cycFILE*Cyc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){{struct Cyc___cycFILE*_tmp58=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp58;};_pop_handler();}else{void*_tmp56=(void*)Cyc_Core_get_exn_thrown();void*_tmp59=_tmp56;
# 380
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0,_tmp2A2.f1=(struct _fat_ptr)((struct _fat_ptr)msg_part);_tmp2A2;});struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0,_tmp2A1.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp2A1;});void*_tmp5A[2];_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D;({struct Cyc___cycFILE*_tmp2EB=Cyc_stderr;struct _fat_ptr _tmp2EA=({const char*_tmp5B="\nError: couldn't open %s %s\n";_tag_fat(_tmp5B,sizeof(char),29U);});Cyc_fprintf(_tmp2EB,_tmp2EA,_tag_fat(_tmp5A,sizeof(void*),2));});});
Cyc_fflush(Cyc_stderr);
return 0;;}}}struct _tuple14{void*f1;const char*f2;int f3;};
# 387
struct _tuple14*Cyc_uncaught_exn=0;
# 389
extern void CYCALLOCPROFILE_mark(const char*);
# 391
void*Cyc_do_stage(struct _fat_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void*onfail){
# 393
CYCALLOCPROFILE_mark((const char*)_untag_fat_ptr(stage_name,sizeof(char),1U));{
struct Cyc_timeval start;struct Cyc_timeval end;
if(Cyc_v_r)
gettimeofday(& start,0);{
# 398
void*ans;
{struct _handler_cons _tmp5E;_push_handler(& _tmp5E);{int _tmp60=0;if(setjmp(_tmp5E.handler))_tmp60=1;if(!_tmp60){ans=f(env,tds);;_pop_handler();}else{void*_tmp5F=(void*)Cyc_Core_get_exn_thrown();void*_tmp61=_tmp5F;void*_tmp62;_tmp62=_tmp61;{void*x=_tmp62;
# 401
ans=onfail;
if(Cyc_Position_error_p())
goto _LL0;
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0,_tmp2A3.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp2A3;});void*_tmp63[1];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmp2ED=Cyc_stderr;struct _fat_ptr _tmp2EC=({const char*_tmp64="COMPILER STAGE %s (unexpected exception) \n";_tag_fat(_tmp64,sizeof(char),43U);});Cyc_fprintf(_tmp2ED,_tmp2EC,_tag_fat(_tmp63,sizeof(void*),1));});});
Cyc_uncaught_exn=({struct _tuple14*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=x,({const char*_tmp2EF=Cyc_Core_get_exn_filename();_tmp66->f2=_tmp2EF;}),({int _tmp2EE=Cyc_Core_get_exn_lineno();_tmp66->f3=_tmp2EE;});_tmp66;});
goto _LL0;}_LL0:;}}}
# 409
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
({void*_tmp67=0U;({struct Cyc___cycFILE*_tmp2F1=Cyc_stderr;struct _fat_ptr _tmp2F0=({const char*_tmp68="\nCOMPILATION FAILED!\n";_tag_fat(_tmp68,sizeof(char),22U);});Cyc_fprintf(_tmp2F1,_tmp2F0,_tag_fat(_tmp67,sizeof(void*),0));});});
Cyc_fflush(Cyc_stderr);}else{
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0,_tmp2A4.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp2A4;});void*_tmp69[1];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp2F3=Cyc_stderr;struct _fat_ptr _tmp2F2=({const char*_tmp6A="%s completed ";_tag_fat(_tmp6A,sizeof(char),14U);});Cyc_fprintf(_tmp2F3,_tmp2F2,_tag_fat(_tmp69,sizeof(void*),1));});});
gettimeofday(& end,0);
end.tv_sec -=start.tv_sec;
end.tv_usec -=start.tv_usec;
if(end.tv_usec < 0){
end.tv_usec +=1000000;
-- end.tv_sec;}
# 423
({struct Cyc_Int_pa_PrintArg_struct _tmp6E=({struct Cyc_Int_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=1,_tmp2A6.f1=(unsigned long)end.tv_sec;_tmp2A6;});struct Cyc_Int_pa_PrintArg_struct _tmp6F=({struct Cyc_Int_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=1,_tmp2A5.f1=(unsigned long)end.tv_usec;_tmp2A5;});void*_tmp6C[2];_tmp6C[0]=& _tmp6E,_tmp6C[1]=& _tmp6F;({struct Cyc___cycFILE*_tmp2F5=Cyc_stderr;struct _fat_ptr _tmp2F4=({const char*_tmp6D="(%d.%06d secs)\n";_tag_fat(_tmp6D,sizeof(char),16U);});Cyc_fprintf(_tmp2F5,_tmp2F4,_tag_fat(_tmp6C,sizeof(void*),2));});});
Cyc_fflush(Cyc_stderr);}}
# 426
return ans;}}}
# 429
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp70;_push_handler(& _tmp70);{int _tmp72=0;if(setjmp(_tmp70.handler))_tmp72=1;if(!_tmp72){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 434
;_pop_handler();}else{void*_tmp71=(void*)Cyc_Core_get_exn_thrown();void*_tmp73=_tmp71;void*_tmp74;if(((struct Cyc_Parse_Exit_exn_struct*)_tmp73)->tag == Cyc_Parse_Exit){
# 437
Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_tmp74=_tmp73;{void*e=_tmp74;
Cyc_file_close(f);Cyc_Core_rethrow(e);}}_LL0:;}}}
# 440
Cyc_Lex_lex_init(1);
return ans;}}
# 444
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 449
int Cyc_do_adjust(int ignore,struct Cyc_List_List*tds){
Cyc_CurRgn_adjust_all(tds);
return 1;}
# 454
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 456
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);
return tds;}
# 462
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 464
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 467
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 469
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}
# 473
int Cyc_do_insert_checks(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds,tables,Cyc_vcgen_r);
return 1;}struct _tuple15{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 480
struct Cyc_List_List*Cyc_do_interface(struct _tuple15*params,struct Cyc_List_List*tds){
# 482
struct _tuple15*_tmp75=params;void*_tmp78;void*_tmp77;void*_tmp76;_tmp76=_tmp75->f1;_tmp77=_tmp75->f2;_tmp78=_tmp75->f3;{struct Cyc_Tcenv_Tenv*te=_tmp76;struct Cyc___cycFILE*fi=_tmp77;struct Cyc___cycFILE*fo=_tmp78;
struct Cyc_Interface_I*_tmp79=Cyc_Interface_extract(te->ae,tds);struct Cyc_Interface_I*i1=_tmp79;
if(fi == 0)
Cyc_Interface_save(i1,fo);else{
# 487
struct Cyc_Interface_I*_tmp7A=Cyc_Interface_parse(fi);struct Cyc_Interface_I*i0=_tmp7A;
if(!({struct Cyc_Interface_I*_tmp2F9=i0;struct Cyc_Interface_I*_tmp2F8=i1;Cyc_Interface_is_subinterface(_tmp2F9,_tmp2F8,({struct _tuple12*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct _fat_ptr _tmp2F7=({const char*_tmp7B="written interface";_tag_fat(_tmp7B,sizeof(char),18U);});_tmp7D->f1=_tmp2F7;}),({struct _fat_ptr _tmp2F6=({const char*_tmp7C="maximal interface";_tag_fat(_tmp7C,sizeof(char),18U);});_tmp7D->f2=_tmp2F6;});_tmp7D;}));}))
Cyc_compile_failure=1;else{
# 491
Cyc_Interface_save(i0,fo);}}
# 493
return tds;}}
# 496
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 499
return Cyc_Toc_toc(pops,tds);}
# 501
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 504
struct Cyc_List_List*Cyc_do_removelabs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveLabels_remove_unused_labels(tds);}
# 508
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 511
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Flags_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 515
struct Cyc_List_List*Cyc_do_lowtreeshake(int ignore,struct Cyc_List_List*tds){
return Cyc_LowTreeShake_shake(tds);}
# 519
static struct _fat_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 521
static struct _fat_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 523
static void Cyc_set_cyc_include(struct _fat_ptr f){
Cyc_cyc_include=f;}
# 528
int Cyc_copy_internal_file(struct _fat_ptr file,struct Cyc___cycFILE*out_file){
# 531
if((char*)file.curr == (char*)(_tag_fat(0,0,0)).curr){
({void*_tmp7E=0U;({struct Cyc___cycFILE*_tmp2FB=Cyc_stderr;struct _fat_ptr _tmp2FA=({const char*_tmp7F="Internal error: copy_internal_file called with NULL\n";_tag_fat(_tmp7F,sizeof(char),53U);});Cyc_fprintf(_tmp2FB,_tmp2FA,_tag_fat(_tmp7E,sizeof(void*),0));});});
return 1;}{
# 535
struct Cyc___cycFILE*_tmp80=({struct _fat_ptr _tmp2FD=file;struct _fat_ptr _tmp2FC=({const char*_tmp87="r";_tag_fat(_tmp87,sizeof(char),2U);});Cyc_try_file_open(_tmp2FD,_tmp2FC,({const char*_tmp88="internal compiler file";_tag_fat(_tmp88,sizeof(char),23U);}));});struct Cyc___cycFILE*file_f=_tmp80;
if(file_f == 0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned _tmp2A9=1024U;unsigned i;for(i=0;i < _tmp2A9;++ i){buf[i]='\000';}}0;});
while(n_read == 1024U){
n_read=Cyc_fread(_tag_fat(buf,sizeof(char),1024U),1U,1024U,file_f);
if(n_read != 1024U && !Cyc_feof(file_f)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0,_tmp2A7.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2A7;});void*_tmp81[1];_tmp81[0]=& _tmp83;({struct Cyc___cycFILE*_tmp2FF=Cyc_stderr;struct _fat_ptr _tmp2FE=({const char*_tmp82="\nError: problem copying %s\n";_tag_fat(_tmp82,sizeof(char),28U);});Cyc_fprintf(_tmp2FF,_tmp2FE,_tag_fat(_tmp81,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return 1;}
# 548
n_written=Cyc_fwrite(_tag_fat(buf,sizeof(char),1024U),1U,n_read,out_file);
if(n_read != n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp86=({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0,_tmp2A8.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2A8;});void*_tmp84[1];_tmp84[0]=& _tmp86;({struct Cyc___cycFILE*_tmp301=Cyc_stderr;struct _fat_ptr _tmp300=({const char*_tmp85="\nError: problem copying %s\n";_tag_fat(_tmp85,sizeof(char),28U);});Cyc_fprintf(_tmp301,_tmp300,_tag_fat(_tmp84,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 556
Cyc_fclose(file_f);
return 0;}}}
# 562
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles (void){
if(!Cyc_save_c_r)
for(1;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file((struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 568
static void Cyc_find_fail(struct _fat_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp303=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0,_tmp2AA.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2AA;});void*_tmp89[1];_tmp89[0]=& _tmp8B;({struct _fat_ptr _tmp302=({const char*_tmp8A="Error: can't find internal compiler file %s\n";_tag_fat(_tmp8A,sizeof(char),45U);});Cyc_aprintf(_tmp302,_tag_fat(_tmp89,sizeof(void*),1));});});_tmp8C->f1=_tmp303;});_tmp8C;}));}
# 573
static struct _fat_ptr Cyc_find_in_arch_path(struct _fat_ptr s){
struct _fat_ptr _tmp8D=Cyc_Specsfile_find_in_arch_path(s);struct _fat_ptr r=_tmp8D;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}
# 578
static struct _fat_ptr Cyc_find_in_exec_path(struct _fat_ptr s){
struct _fat_ptr _tmp8E=Cyc_Specsfile_find_in_exec_path(s);struct _fat_ptr r=_tmp8E;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}struct _tuple16{struct Cyc___cycFILE*f1;struct _fat_ptr f2;};
# 584
struct Cyc_List_List*Cyc_do_print(struct _tuple16*env,struct Cyc_List_List*tds){
# 586
struct _tuple16*_tmp8F=env;struct _fat_ptr _tmp91;void*_tmp90;_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;{struct Cyc___cycFILE*out_file=_tmp90;struct _fat_ptr cfile=_tmp91;
struct Cyc_Absynpp_Params _tmp92=(int)Cyc_Flags_stop_after_pass < 9?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;struct Cyc_Absynpp_Params params_r=_tmp92;
params_r.expand_typedefs=!Cyc_Flags_noexpand_r;
params_r.to_VC=Cyc_Flags_tovc_r;
params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;
params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;
params_r.print_all_effects=Cyc_print_all_effects_r;
# 598
if((int)Cyc_Flags_stop_after_pass > 6){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Flags_compile_for_boot)
({void*_tmp93=0U;({struct Cyc___cycFILE*_tmp305=out_file;struct _fat_ptr _tmp304=({const char*_tmp94="#include <setjmp.h>\n";_tag_fat(_tmp94,sizeof(char),21U);});Cyc_fprintf(_tmp305,_tmp304,_tag_fat(_tmp93,sizeof(void*),0));});});else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 604
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 606
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 610
Cyc_Absyndump_set_params(& params_r);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 613
Cyc_fflush(out_file);
return tds;}}
# 618
static struct Cyc_List_List*Cyc_split_by_char(struct _fat_ptr s,char c){
if((char*)s.curr == (char*)(_tag_fat(0,0,0)).curr)return 0;{
struct Cyc_List_List*_tmp95=0;struct Cyc_List_List*result=_tmp95;
unsigned long _tmp96=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp96;
# 623
while(len > 0U){
struct _fat_ptr _tmp97=Cyc_strchr(s,c);struct _fat_ptr end=_tmp97;
if((char*)end.curr == (char*)(_tag_fat(0,0,0)).curr){
result=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));({struct _fat_ptr*_tmp306=({struct _fat_ptr*_tmp98=_cycalloc(sizeof(*_tmp98));*_tmp98=s;_tmp98;});_tmp99->hd=_tmp306;}),_tmp99->tl=result;_tmp99;});
break;}else{
# 629
result=({struct Cyc_List_List*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct _fat_ptr*_tmp308=({struct _fat_ptr*_tmp9A=_cycalloc(sizeof(*_tmp9A));({struct _fat_ptr _tmp307=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char)));*_tmp9A=_tmp307;});_tmp9A;});_tmp9B->hd=_tmp308;}),_tmp9B->tl=result;_tmp9B;});
len -=(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char));
s=_fat_ptr_plus(end,sizeof(char),1);}}
# 634
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(result);}}
# 639
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){
# 641
struct Cyc_List_List*_tmp9C=0;struct Cyc_List_List*l=_tmp9C;
for(1;dirs != 0;dirs=dirs->tl){
l=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));({struct _fat_ptr*_tmp30A=({struct _fat_ptr*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct _fat_ptr _tmp309=(struct _fat_ptr)Cyc_Filename_concat(*((struct _fat_ptr*)dirs->hd),subdir);*_tmp9D=_tmp309;});_tmp9D;});_tmp9E->hd=_tmp30A;}),_tmp9E->tl=l;_tmp9E;});}
l=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(l);
return l;}
# 649
static int Cyc_is_other_special(char c){
char _tmp9F=c;switch((int)_tmp9F){case 92:
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
# 664
 goto _LL18;case 9: _LL18:
 return 1;default:
 return 0;};}
# 671
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmpA0=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmpA0;
# 675
int _tmpA1=0;int single_quotes=_tmpA1;
int _tmpA2=0;int other_special=_tmpA2;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmpA3=((const char*)s.curr)[i];char c=_tmpA3;
if((int)c == 39)++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 684
if(single_quotes == 0 && other_special == 0)
return s;
# 688
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmpA4[3];({struct _fat_ptr*_tmp30F=({struct _fat_ptr*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _fat_ptr _tmp30E=({const char*_tmpA5="'";_tag_fat(_tmpA5,sizeof(char),2U);});*_tmpA6=_tmp30E;});_tmpA6;});_tmpA4[0]=_tmp30F;}),({struct _fat_ptr*_tmp30D=({struct _fat_ptr*_tmpA7=_cycalloc(sizeof(*_tmpA7));*_tmpA7=(struct _fat_ptr)s;_tmpA7;});_tmpA4[1]=_tmp30D;}),({struct _fat_ptr*_tmp30C=({struct _fat_ptr*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct _fat_ptr _tmp30B=({const char*_tmpA8="'";_tag_fat(_tmpA8,sizeof(char),2U);});*_tmpA9=_tmp30B;});_tmpA9;});_tmpA4[2]=_tmp30C;});({(struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list;})(_tag_fat(_tmpA4,sizeof(struct _fat_ptr*),3));}));{
# 692
unsigned long _tmpAA=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmpAA;
struct _fat_ptr s2=({unsigned _tmpB5=(len2 + 1U)+ 1U;char*_tmpB4=_cycalloc_atomic(_check_times(_tmpB5,sizeof(char)));({{unsigned _tmp2AB=len2 + 1U;unsigned i;for(i=0;i < _tmp2AB;++ i){_tmpB4[i]='\000';}_tmpB4[_tmp2AB]=0;}0;});_tag_fat(_tmpB4,sizeof(char),_tmpB5);});
int _tmpAB=0;int i=_tmpAB;
int _tmpAC=0;int j=_tmpAC;
for(1;(unsigned long)i < len;++ i){
char _tmpAD=((const char*)s.curr)[i];char c=_tmpAD;
if((int)c == 39 || Cyc_is_other_special(c))
({struct _fat_ptr _tmpAE=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpAF=*((char*)_check_fat_subscript(_tmpAE,sizeof(char),0U));char _tmpB0='\\';if(_get_fat_size(_tmpAE,sizeof(char))== 1U &&(_tmpAF == 0 && _tmpB0 != 0))_throw_arraybounds();*((char*)_tmpAE.curr)=_tmpB0;});
({struct _fat_ptr _tmpB1=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpB2=*((char*)_check_fat_subscript(_tmpB1,sizeof(char),0U));char _tmpB3=c;if(_get_fat_size(_tmpB1,sizeof(char))== 1U &&(_tmpB2 == 0 && _tmpB3 != 0))_throw_arraybounds();*((char*)_tmpB1.curr)=_tmpB3;});}
# 702
return(struct _fat_ptr)s2;}}
# 704
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct _fat_ptr _tmp310=Cyc_sh_escape_string(*sp);*_tmpB6=_tmp310;});_tmpB6;});}
# 708
static void Cyc_process_file(struct _fat_ptr filename){
# 710
struct _fat_ptr _tmpB7=Cyc_make_base_filename(filename,Cyc_output_file);struct _fat_ptr basename=_tmpB7;
struct _fat_ptr _tmpB8=({struct _fat_ptr _tmp311=(struct _fat_ptr)basename;Cyc_strconcat(_tmp311,({const char*_tmp11E=".cyp";_tag_fat(_tmp11E,sizeof(char),5U);}));});struct _fat_ptr preprocfile=_tmpB8;
struct _fat_ptr _tmpB9=(char*)Cyc_specified_interface.curr != (char*)(_tag_fat(0,0,0)).curr?Cyc_specified_interface:(struct _fat_ptr)({
struct _fat_ptr _tmp312=(struct _fat_ptr)basename;Cyc_strconcat(_tmp312,({const char*_tmp11D=".cyci";_tag_fat(_tmp11D,sizeof(char),6U);}));});
# 712
struct _fat_ptr interfacefile=_tmpB9;
# 714
struct _fat_ptr _tmpBA=({struct _fat_ptr _tmp313=(struct _fat_ptr)basename;Cyc_strconcat(_tmp313,({const char*_tmp11C=".cycio";_tag_fat(_tmp11C,sizeof(char),7U);}));});struct _fat_ptr interfaceobjfile=_tmpBA;
# 718
struct _fat_ptr _tmpBB=({struct _fat_ptr _tmp314=(struct _fat_ptr)Cyc_make_base_filename(filename,0);Cyc_strconcat(_tmp314,({const char*_tmp11B=".c";_tag_fat(_tmp11B,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmpBB;
struct _fat_ptr*_tmpBC=({struct _fat_ptr*_tmp11A=_cycalloc(sizeof(*_tmp11A));*_tmp11A=preprocfile;_tmp11A;});struct _fat_ptr*preprocfileptr=_tmpBC;
# 721
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpBF=({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0,_tmp2AC.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp2AC;});void*_tmpBD[1];_tmpBD[0]=& _tmpBF;({struct Cyc___cycFILE*_tmp316=Cyc_stderr;struct _fat_ptr _tmp315=({const char*_tmpBE="Compiling %s\n";_tag_fat(_tmpBE,sizeof(char),14U);});Cyc_fprintf(_tmp316,_tmp315,_tag_fat(_tmpBD,sizeof(void*),1));});});{
# 724
struct Cyc___cycFILE*f0=({struct _fat_ptr _tmp318=filename;struct _fat_ptr _tmp317=({const char*_tmp118="r";_tag_fat(_tmp118,sizeof(char),2U);});Cyc_try_file_open(_tmp318,_tmp317,({const char*_tmp119="input file";_tag_fat(_tmp119,sizeof(char),11U);}));});
if(Cyc_compile_failure || !((unsigned)f0))
return;
Cyc_fclose(f0);{
# 731
struct _fat_ptr _tmpC0=({
struct Cyc_List_List*_tmp31D=({struct Cyc_List_List*_tmp116=_cycalloc(sizeof(*_tmp116));({struct _fat_ptr*_tmp31C=({struct _fat_ptr*_tmp115=_cycalloc(sizeof(*_tmp115));({struct _fat_ptr _tmp31B=(struct _fat_ptr)({const char*_tmp114="";_tag_fat(_tmp114,sizeof(char),1U);});*_tmp115=_tmp31B;});_tmp115;});_tmp116->hd=_tmp31C;}),({
struct Cyc_List_List*_tmp31A=({struct Cyc_List_List*(*_tmp319)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;});_tmp319(Cyc_sh_escape_stringptr,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_cppargs));});_tmp116->tl=_tmp31A;});_tmp116;});
# 732
Cyc_str_sepstr(_tmp31D,({const char*_tmp117=" ";_tag_fat(_tmp117,sizeof(char),2U);}));});
# 731
struct _fat_ptr cppargs_string=_tmpC0;
# 741
struct Cyc_List_List*_tmpC1=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);struct Cyc_List_List*stdinc_dirs=_tmpC1;
stdinc_dirs=({struct Cyc_List_List*_tmp31E=stdinc_dirs;Cyc_add_subdir(_tmp31E,({const char*_tmpC2="include";_tag_fat(_tmpC2,sizeof(char),8U);}));});
if(Cyc_strlen((struct _fat_ptr)Cyc_def_inc_path)> 0U)
stdinc_dirs=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct _fat_ptr*_tmp31F=({struct _fat_ptr*_tmpC3=_cycalloc(sizeof(*_tmpC3));*_tmpC3=Cyc_def_inc_path;_tmpC3;});_tmpC4->hd=_tmp31F;}),_tmpC4->tl=stdinc_dirs;_tmpC4;});{
char*_tmpC5=getenv("CYCLONE_INCLUDE_PATH");char*cyclone_include_path=_tmpC5;
if(cyclone_include_path != 0)
stdinc_dirs=({struct Cyc_List_List*(*_tmp321)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp320=Cyc_split_by_char(({char*_tmpC6=cyclone_include_path;_tag_fat(_tmpC6,sizeof(char),_get_zero_arr_size_char((void*)_tmpC6,1U));}),':');_tmp321(_tmp320,stdinc_dirs);});{
struct _fat_ptr stdinc_string=(struct _fat_ptr)({
struct Cyc_List_List*_tmp325=({struct Cyc_List_List*_tmp112=_cycalloc(sizeof(*_tmp112));({struct _fat_ptr*_tmp324=({struct _fat_ptr*_tmp111=_cycalloc(sizeof(*_tmp111));({struct _fat_ptr _tmp323=(struct _fat_ptr)({const char*_tmp110="";_tag_fat(_tmp110,sizeof(char),1U);});*_tmp111=_tmp323;});_tmp111;});_tmp112->hd=_tmp324;}),({
struct Cyc_List_List*_tmp322=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,stdinc_dirs);_tmp112->tl=_tmp322;});_tmp112;});
# 749
Cyc_str_sepstr(_tmp325,({const char*_tmp113=" -I";_tag_fat(_tmp113,sizeof(char),4U);}));});
# 752
struct _fat_ptr ofile_string;
if((int)Cyc_Flags_stop_after_pass <= 0)
ofile_string=Cyc_output_file == 0?({const char*_tmpC7="";_tag_fat(_tmpC7,sizeof(char),1U);}):(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpCA=({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0,_tmp2AD.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_output_file)));_tmp2AD;});void*_tmpC8[1];_tmpC8[0]=& _tmpCA;({struct _fat_ptr _tmp326=({const char*_tmpC9=" > %s";_tag_fat(_tmpC9,sizeof(char),6U);});Cyc_aprintf(_tmp326,_tag_fat(_tmpC8,sizeof(void*),1));});});else{
# 756
ofile_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpCD=({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0,({struct _fat_ptr _tmp327=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string((struct _fat_ptr)preprocfile));_tmp2AE.f1=_tmp327;});_tmp2AE;});void*_tmpCB[1];_tmpCB[0]=& _tmpCD;({struct _fat_ptr _tmp328=({const char*_tmpCC=" > %s";_tag_fat(_tmpCC,sizeof(char),6U);});Cyc_aprintf(_tmp328,_tag_fat(_tmpCB,sizeof(void*),1));});});}{
# 758
struct _fat_ptr fixup_string;
if(Cyc_produce_dependencies){
# 762
if(Cyc_dependencies_target == 0)
# 766
fixup_string=({const char*_tmpCE=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_fat(_tmpCE,sizeof(char),35U);});else{
# 771
fixup_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0,_tmp2AF.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_dependencies_target)));_tmp2AF;});void*_tmpCF[1];_tmpCF[0]=& _tmpD1;({struct _fat_ptr _tmp329=({const char*_tmpD0=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_fat(_tmpD0,sizeof(char),29U);});Cyc_aprintf(_tmp329,_tag_fat(_tmpCF,sizeof(void*),1));});});}}else{
# 774
fixup_string=({const char*_tmpD2="";_tag_fat(_tmpD2,sizeof(char),1U);});}{
# 776
struct _fat_ptr _tmpD3=({struct Cyc_String_pa_PrintArg_struct _tmp10A=({struct Cyc_String_pa_PrintArg_struct _tmp2BA;_tmp2BA.tag=0,_tmp2BA.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cpp);_tmp2BA;});struct Cyc_String_pa_PrintArg_struct _tmp10B=({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0,_tmp2B9.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp2B9;});struct Cyc_String_pa_PrintArg_struct _tmp10C=({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0,_tmp2B8.f1=(struct _fat_ptr)((struct _fat_ptr)stdinc_string);_tmp2B8;});struct Cyc_String_pa_PrintArg_struct _tmp10D=({struct Cyc_String_pa_PrintArg_struct _tmp2B7;_tmp2B7.tag=0,({
# 778
struct _fat_ptr _tmp32A=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(filename));_tmp2B7.f1=_tmp32A;});_tmp2B7;});struct Cyc_String_pa_PrintArg_struct _tmp10E=({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0,_tmp2B6.f1=(struct _fat_ptr)((struct _fat_ptr)fixup_string);_tmp2B6;});struct Cyc_String_pa_PrintArg_struct _tmp10F=({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0,_tmp2B5.f1=(struct _fat_ptr)((struct _fat_ptr)ofile_string);_tmp2B5;});void*_tmp108[6];_tmp108[0]=& _tmp10A,_tmp108[1]=& _tmp10B,_tmp108[2]=& _tmp10C,_tmp108[3]=& _tmp10D,_tmp108[4]=& _tmp10E,_tmp108[5]=& _tmp10F;({struct _fat_ptr _tmp32B=({const char*_tmp109="%s %s%s %s%s%s";_tag_fat(_tmp109,sizeof(char),15U);});Cyc_aprintf(_tmp32B,_tag_fat(_tmp108,sizeof(void*),6));});});
# 776
struct _fat_ptr cmd=_tmpD3;
# 780
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0,_tmp2B0.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp2B0;});void*_tmpD4[1];_tmpD4[0]=& _tmpD6;({struct Cyc___cycFILE*_tmp32D=Cyc_stderr;struct _fat_ptr _tmp32C=({const char*_tmpD5="%s\n";_tag_fat(_tmpD5,sizeof(char),4U);});Cyc_fprintf(_tmp32D,_tmp32C,_tag_fat(_tmpD4,sizeof(void*),1));});});
if(system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)))!= 0){
Cyc_compile_failure=1;
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp32F=Cyc_stderr;struct _fat_ptr _tmp32E=({const char*_tmpD8="\nError: preprocessing\n";_tag_fat(_tmpD8,sizeof(char),23U);});Cyc_fprintf(_tmp32F,_tmp32E,_tag_fat(_tmpD7,sizeof(void*),0));});});
if((int)Cyc_Flags_stop_after_pass > 0)Cyc_remove_file((struct _fat_ptr)preprocfile);
return;}
# 787
if((int)Cyc_Flags_stop_after_pass <= 0)return;
# 790
Cyc_Position_reset_position((struct _fat_ptr)preprocfile);{
struct Cyc___cycFILE*in_file=({struct _fat_ptr _tmp331=(struct _fat_ptr)preprocfile;struct _fat_ptr _tmp330=({const char*_tmp106="r";_tag_fat(_tmp106,sizeof(char),2U);});Cyc_try_file_open(_tmp331,_tmp330,({const char*_tmp107="file";_tag_fat(_tmp107,sizeof(char),5U);}));});
# 793
struct Cyc_List_List*tds=0;
struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpD9=0;struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables=_tmpD9;
struct Cyc_Tcenv_Tenv*_tmpDA=Cyc_Tcenv_tc_init();struct Cyc_Tcenv_Tenv*te=_tmpDA;
# 798
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 1)
tds=({struct Cyc_List_List*(*_tmp335)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp334=({const char*_tmpDB="parsing";_tag_fat(_tmpDB,sizeof(char),8U);});struct Cyc_List_List*_tmp333=tds;struct Cyc___cycFILE*_tmp332=(struct Cyc___cycFILE*)_check_null(in_file);_tmp335(_tmp334,_tmp333,Cyc_do_parse,_tmp332,tds);});
# 802
if(!Cyc_compile_failure && Cyc_Flags_porting_c_code){
Cyc_Port_port(tds);
Cyc_remove_file((struct _fat_ptr)preprocfile);
return;}
# 809
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 2)
({int(*_tmp337)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int)=({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;});struct _fat_ptr _tmp336=({const char*_tmpDC="binding";_tag_fat(_tmpDC,sizeof(char),8U);});_tmp337(_tmp336,tds,Cyc_do_binding,1,1);});
# 813
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 3)
({int(*_tmp339)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int)=({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage;});struct _fat_ptr _tmp338=({const char*_tmpDD="current region resolution";_tag_fat(_tmpDD,sizeof(char),26U);});_tmp339(_tmp338,tds,Cyc_do_adjust,1,1);});
# 817
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 4)
tds=({struct Cyc_List_List*(*_tmp33D)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp33C=({const char*_tmpDE="type checking";_tag_fat(_tmpDE,sizeof(char),14U);});struct Cyc_List_List*_tmp33B=tds;struct Cyc_Tcenv_Tenv*_tmp33A=te;_tmp33D(_tmp33C,_tmp33B,Cyc_do_typecheck,_tmp33A,tds);});
# 821
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 5)
jump_tables=({struct Cyc_JumpAnalysis_Jump_Anal_Result*(*_tmp33F)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*)=({(struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*))Cyc_do_stage;});struct _fat_ptr _tmp33E=({const char*_tmpDF="jump checking";_tag_fat(_tmpDF,sizeof(char),14U);});_tmp33F(_tmp33E,tds,Cyc_do_jumpanalysis,1,0);});
# 825
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 6)
tds=({struct Cyc_List_List*(*_tmp343)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp342=({const char*_tmpE0="control-flow checking";_tag_fat(_tmpE0,sizeof(char),22U);});struct Cyc_List_List*_tmp341=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp340=(struct Cyc_JumpAnalysis_Jump_Anal_Result*)_check_null(jump_tables);_tmp343(_tmp342,_tmp341,Cyc_do_cfcheck,_tmp340,tds);});
# 829
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 8)
({int(*_tmp346)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int)=({(int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int))Cyc_do_stage;});struct _fat_ptr _tmp345=({const char*_tmpE1="check insertion";_tag_fat(_tmpE1,sizeof(char),16U);});struct Cyc_List_List*_tmp344=tds;_tmp346(_tmp345,_tmp344,Cyc_do_insert_checks,(struct Cyc_JumpAnalysis_Jump_Anal_Result*)_check_null(jump_tables),1);});
# 833
if((!Cyc_compile_failure && Cyc_generate_interface_r)&&(int)Cyc_Flags_stop_after_pass >= 9){
# 835
struct Cyc___cycFILE*inter_file=({struct _fat_ptr _tmp348=({const char*_tmpE6=(const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U));_tag_fat(_tmpE6,sizeof(char),_get_zero_arr_size_char((void*)_tmpE6,1U));});struct _fat_ptr _tmp347=({const char*_tmpE7="w";_tag_fat(_tmpE7,sizeof(char),2U);});Cyc_try_file_open(_tmp348,_tmp347,({const char*_tmpE8="interface file";_tag_fat(_tmpE8,sizeof(char),15U);}));});
if(inter_file == 0)
Cyc_compile_failure=1;else{
# 839
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 846
if(!Cyc_compile_failure && Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_fat_ptr(interfacefile,sizeof(char),1U),"r");
struct Cyc___cycFILE*inter_objfile=({struct _fat_ptr _tmp34A=(struct _fat_ptr)interfaceobjfile;struct _fat_ptr _tmp349=({const char*_tmpE4="w";_tag_fat(_tmpE4,sizeof(char),2U);});Cyc_try_file_open(_tmp34A,_tmp349,({const char*_tmpE5="interface object file";_tag_fat(_tmpE5,sizeof(char),22U);}));});
if(inter_objfile == 0)
Cyc_compile_failure=1;else{
# 852
Cyc_Position_reset_position(interfacefile);{
struct _tuple15 _tmpE2=({struct _tuple15 _tmp2B1;_tmp2B1.f1=te,_tmp2B1.f2=inter_file,_tmp2B1.f3=inter_objfile;_tmp2B1;});struct _tuple15 int_env=_tmpE2;
tds=({struct Cyc_List_List*(*_tmp34D)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp34C=({const char*_tmpE3="interface checking";_tag_fat(_tmpE3,sizeof(char),19U);});struct Cyc_List_List*_tmp34B=tds;_tmp34D(_tmp34C,_tmp34B,Cyc_do_interface,& int_env,tds);});
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);}}}}
# 863
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
tds=({struct Cyc_List_List*(*_tmp351)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp350=({const char*_tmpE9="translation to C";_tag_fat(_tmpE9,sizeof(char),17U);});struct Cyc_List_List*_tmp34F=tds;struct Cyc_Hashtable_Table*_tmp34E=((struct Cyc_JumpAnalysis_Jump_Anal_Result*)_check_null(jump_tables))->pop_tables;_tmp351(_tmp350,_tmp34F,Cyc_do_translate,_tmp34E,tds);});
# 867
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 10)
tds=({struct Cyc_List_List*(*_tmp354)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp353=({const char*_tmpEA="aggregate removal";_tag_fat(_tmpEA,sizeof(char),18U);});struct Cyc_List_List*_tmp352=tds;_tmp354(_tmp353,_tmp352,Cyc_do_removeaggrs,1,tds);});
# 870
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 11)
tds=({struct Cyc_List_List*(*_tmp357)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp356=({const char*_tmpEB="unused label removal";_tag_fat(_tmpEB,sizeof(char),21U);});struct Cyc_List_List*_tmp355=tds;_tmp357(_tmp356,_tmp355,Cyc_do_removelabs,1,tds);});
# 873
if((!Cyc_compile_failure && Cyc_toseqc_r)&&(int)Cyc_Flags_stop_after_pass >= 12)
tds=({struct Cyc_List_List*(*_tmp35A)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp359=({const char*_tmpEC="L-to-R evaluation order";_tag_fat(_tmpEC,sizeof(char),24U);});struct Cyc_List_List*_tmp358=tds;_tmp35A(_tmp359,_tmp358,Cyc_do_toseqc,1,tds);});
# 876
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
Cyc_Toc_finish();
# 879
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
tds=({struct Cyc_List_List*(*_tmp35D)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp35C=({const char*_tmpED="low treehsaking";_tag_fat(_tmpED,sizeof(char),16U);});struct Cyc_List_List*_tmp35B=tds;_tmp35D(_tmp35C,_tmp35B,Cyc_do_lowtreeshake,1,tds);});
# 883
if(!Cyc_compile_failure &&(Cyc_Flags_tovc_r || Cyc_elim_se_r))
tds=({struct Cyc_List_List*(*_tmp360)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp35F=({const char*_tmpEE="post-pass to VC";_tag_fat(_tmpEE,sizeof(char),16U);});struct Cyc_List_List*_tmp35E=tds;_tmp360(_tmp35F,_tmp35E,Cyc_do_tovc,1,tds);});
# 886
if(!Cyc_compile_failure)
Cyc_Warn_flush_warnings();
Cyc_remove_file((struct _fat_ptr)preprocfile);
if(Cyc_uncaught_exn != 0){
struct _tuple14*_tmpEF=(struct _tuple14*)_check_null(Cyc_uncaught_exn);struct _tuple14*_stmttmp0=_tmpEF;struct _tuple14*_tmpF0=_stmttmp0;int _tmpF3;const char*_tmpF2;void*_tmpF1;_tmpF1=_tmpF0->f1;_tmpF2=_tmpF0->f2;_tmpF3=_tmpF0->f3;{void*e=_tmpF1;const char*fn=_tmpF2;int line=_tmpF3;
({struct Cyc_String_pa_PrintArg_struct _tmpF6=({struct Cyc_String_pa_PrintArg_struct _tmp2B4;_tmp2B4.tag=0,({
struct _fat_ptr _tmp361=(struct _fat_ptr)({const char*_tmpFA=Cyc_Core_get_exn_name(e);_tag_fat(_tmpFA,sizeof(char),_get_zero_arr_size_char((void*)_tmpFA,1U));});_tmp2B4.f1=_tmp361;});_tmp2B4;});struct Cyc_String_pa_PrintArg_struct _tmpF7=({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0,({struct _fat_ptr _tmp362=(struct _fat_ptr)({const char*_tmpF9=fn;_tag_fat(_tmpF9,sizeof(char),_get_zero_arr_size_char((void*)_tmpF9,1U));});_tmp2B3.f1=_tmp362;});_tmp2B3;});struct Cyc_Int_pa_PrintArg_struct _tmpF8=({struct Cyc_Int_pa_PrintArg_struct _tmp2B2;_tmp2B2.tag=1,_tmp2B2.f1=(unsigned long)line;_tmp2B2;});void*_tmpF4[3];_tmpF4[0]=& _tmpF6,_tmpF4[1]=& _tmpF7,_tmpF4[2]=& _tmpF8;({struct Cyc___cycFILE*_tmp364=Cyc_stderr;struct _fat_ptr _tmp363=({const char*_tmpF5="uncaught exception: %s from %s, line %d\n";_tag_fat(_tmpF5,sizeof(char),41U);});Cyc_fprintf(_tmp364,_tmp363,_tag_fat(_tmpF4,sizeof(void*),3));});});
Cyc_fflush(Cyc_stderr);
return;}}{
# 898
struct Cyc___cycFILE*out_file;
if((int)Cyc_Flags_stop_after_pass <= 6){
if(Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp366=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp365=({const char*_tmpFB="w";_tag_fat(_tmpFB,sizeof(char),2U);});Cyc_try_file_open(_tmp366,_tmp365,({const char*_tmpFC="output file";_tag_fat(_tmpFC,sizeof(char),12U);}));});else{
# 903
out_file=Cyc_stdout;}}else{
if((int)Cyc_Flags_stop_after_pass < 13 && Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp368=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp367=({const char*_tmpFD="w";_tag_fat(_tmpFD,sizeof(char),2U);});Cyc_try_file_open(_tmp368,_tmp367,({const char*_tmpFE="output file";_tag_fat(_tmpFE,sizeof(char),12U);}));});else{
# 907
out_file=({struct _fat_ptr _tmp36A=(struct _fat_ptr)cfile;struct _fat_ptr _tmp369=({const char*_tmpFF="w";_tag_fat(_tmpFF,sizeof(char),2U);});Cyc_try_file_open(_tmp36A,_tmp369,({const char*_tmp100="output file";_tag_fat(_tmp100,sizeof(char),12U);}));});}}
# 909
if(Cyc_compile_failure || !((unsigned)out_file))return;
# 911
if(!Cyc_noprint_r){
struct _tuple16*_tmp101=({struct _tuple16*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->f1=out_file,_tmp105->f2=cfile;_tmp105;});struct _tuple16*env=_tmp101;
tds=({struct Cyc_List_List*(*_tmp36E)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple16*,struct Cyc_List_List*),struct _tuple16*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple16*,struct Cyc_List_List*),struct _tuple16*,struct Cyc_List_List*))Cyc_do_stage;});struct _fat_ptr _tmp36D=({const char*_tmp102="printing";_tag_fat(_tmp102,sizeof(char),9U);});struct Cyc_List_List*_tmp36C=tds;struct _tuple16*_tmp36B=env;_tmp36E(_tmp36D,_tmp36C,Cyc_do_print,_tmp36B,tds);});
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmp104=_cycalloc(sizeof(*_tmp104));({struct _fat_ptr*_tmp370=({struct _fat_ptr*_tmp103=_cycalloc(sizeof(*_tmp103));({struct _fat_ptr _tmp36F=Cyc_strdup((struct _fat_ptr)cfile);*_tmp103=_tmp36F;});_tmp103;});_tmp104->hd=_tmp370;}),_tmp104->tl=Cyc_cfiles;_tmp104;});}}}}}}}}}}static char _tmp11F[8U]="<final>";
# 919
static struct _fat_ptr Cyc_final_str={_tmp11F,_tmp11F,_tmp11F + 8U};
static struct _fat_ptr*Cyc_final_strptr=& Cyc_final_str;
# 922
static struct Cyc_Interface_I*Cyc_read_cycio(struct _fat_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
struct _fat_ptr basename;
{struct _handler_cons _tmp120;_push_handler(& _tmp120);{int _tmp122=0;if(setjmp(_tmp120.handler))_tmp122=1;if(!_tmp122){basename=(struct _fat_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp121=(void*)Cyc_Core_get_exn_thrown();void*_tmp123=_tmp121;void*_tmp124;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp123)->tag == Cyc_Core_Invalid_argument){
basename=*n;goto _LL0;}else{_tmp124=_tmp123;{void*exn=_tmp124;(int)_rethrow(exn);}}_LL0:;}}}{
# 929
struct _fat_ptr _tmp125=({struct _fat_ptr _tmp371=(struct _fat_ptr)basename;Cyc_strconcat(_tmp371,({const char*_tmp12A=".cycio";_tag_fat(_tmp12A,sizeof(char),7U);}));});struct _fat_ptr nf=_tmp125;
struct Cyc___cycFILE*_tmp126=({struct _fat_ptr _tmp373=(struct _fat_ptr)nf;struct _fat_ptr _tmp372=({const char*_tmp128="rb";_tag_fat(_tmp128,sizeof(char),3U);});Cyc_try_file_open(_tmp373,_tmp372,({const char*_tmp129="interface object file";_tag_fat(_tmp129,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp126;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 936
Cyc_Position_reset_position((struct _fat_ptr)nf);{
struct Cyc_Interface_I*_tmp127=Cyc_Interface_load(f);struct Cyc_Interface_I*i=_tmp127;
Cyc_file_close(f);
return i;}}}}
# 942
static int Cyc_is_cfile(struct _fat_ptr*n){
return(int)*((const char*)_check_fat_subscript(*n,sizeof(char),0))!= 45;}
# 946
extern void GC_blacklist_warn_clear (void);struct _tuple17{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 956
void Cyc_print_options (void);
# 964
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3,& Cyc_v_r};static char _tmp12B[3U]="-v";static char _tmp12C[1U]="";static char _tmp12D[35U]="print compilation stages verbosely";struct _tuple17 Cyc_opt1_tuple={{_tmp12B,_tmp12B,_tmp12B + 3U},0,{_tmp12C,_tmp12C,_tmp12C + 1U},(void*)& Cyc_opt1_spec,{_tmp12D,_tmp12D,_tmp12D + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 966
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0,Cyc_print_version};static char _tmp12E[10U]="--version";static char _tmp12F[1U]="";static char _tmp130[35U]="Print version information and exit";struct _tuple17 Cyc_opt2_tuple={{_tmp12E,_tmp12E,_tmp12E + 10U},0,{_tmp12F,_tmp12F,_tmp12F + 1U},(void*)& Cyc_opt2_spec,{_tmp130,_tmp130,_tmp130 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 969
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5,Cyc_set_output_file};static char _tmp131[3U]="-o";static char _tmp132[8U]=" <file>";static char _tmp133[35U]="Set the output file name to <file>";struct _tuple17 Cyc_opt3_tuple={{_tmp131,_tmp131,_tmp131 + 3U},0,{_tmp132,_tmp132,_tmp132 + 8U},(void*)& Cyc_opt3_spec,{_tmp133,_tmp133,_tmp133 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 972
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1,Cyc_add_cpparg};static char _tmp134[3U]="-D";static char _tmp135[17U]="<name>[=<value>]";static char _tmp136[32U]="Pass definition to preprocessor";struct _tuple17 Cyc_opt4_tuple={{_tmp134,_tmp134,_tmp134 + 3U},1,{_tmp135,_tmp135,_tmp135 + 17U},(void*)& Cyc_opt4_spec,{_tmp136,_tmp136,_tmp136 + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 975
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1,Cyc_Specsfile_add_cyclone_exec_path};static char _tmp137[3U]="-B";static char _tmp138[7U]="<file>";static char _tmp139[60U]="Add to the list of directories to search for compiler files";struct _tuple17 Cyc_opt5_tuple={{_tmp137,_tmp137,_tmp137 + 3U},1,{_tmp138,_tmp138,_tmp138 + 7U},(void*)& Cyc_opt5_spec,{_tmp139,_tmp139,_tmp139 + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 978
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1,Cyc_add_cpparg};static char _tmp13A[3U]="-I";static char _tmp13B[6U]="<dir>";static char _tmp13C[59U]="Add to the list of directories to search for include files";struct _tuple17 Cyc_opt6_tuple={{_tmp13A,_tmp13A,_tmp13A + 3U},1,{_tmp13B,_tmp13B,_tmp13B + 6U},(void*)& Cyc_opt6_spec,{_tmp13C,_tmp13C,_tmp13C + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 981
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1,Cyc_add_ccarg};static char _tmp13D[3U]="-L";static char _tmp13E[6U]="<dir>";static char _tmp13F[38U]="Add to the list of directories for -l";struct _tuple17 Cyc_opt7_tuple={{_tmp13D,_tmp13D,_tmp13D + 3U},1,{_tmp13E,_tmp13E,_tmp13E + 6U},(void*)& Cyc_opt7_spec,{_tmp13F,_tmp13F,_tmp13F + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 984
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1,Cyc_add_libarg};static char _tmp140[3U]="-l";static char _tmp141[10U]="<libname>";static char _tmp142[13U]="Library file";struct _tuple17 Cyc_opt8_tuple={{_tmp140,_tmp140,_tmp140 + 3U},1,{_tmp141,_tmp141,_tmp141 + 10U},(void*)& Cyc_opt8_spec,{_tmp142,_tmp142,_tmp142 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 987
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1,Cyc_add_marg};static char _tmp143[3U]="-m";static char _tmp144[9U]="<option>";static char _tmp145[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple17 Cyc_opt9_tuple={{_tmp143,_tmp143,_tmp143 + 3U},1,{_tmp144,_tmp144,_tmp144 + 9U},(void*)& Cyc_opt9_spec,{_tmp145,_tmp145,_tmp145 + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 990
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0,Cyc_set_stop_after_objectfile};static char _tmp146[3U]="-c";static char _tmp147[1U]="";static char _tmp148[61U]="Produce an object file instead of an executable; do not link";struct _tuple17 Cyc_opt10_tuple={{_tmp146,_tmp146,_tmp146 + 3U},0,{_tmp147,_tmp147,_tmp147 + 1U},(void*)& Cyc_opt10_spec,{_tmp148,_tmp148,_tmp148 + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 993
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5,Cyc_set_inputtype};static char _tmp149[3U]="-x";static char _tmp14A[12U]=" <language>";static char _tmp14B[49U]="Specify <language> for the following input files";struct _tuple17 Cyc_opt11_tuple={{_tmp149,_tmp149,_tmp149 + 3U},0,{_tmp14A,_tmp14A,_tmp14A + 12U},(void*)& Cyc_opt11_spec,{_tmp14B,_tmp14B,_tmp14B + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 996
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0,Cyc_set_pa};static char _tmp14C[4U]="-pa";static char _tmp14D[1U]="";static char _tmp14E[33U]="Compile for profiling with aprof";struct _tuple17 Cyc_opt12_tuple={{_tmp14C,_tmp14C,_tmp14C + 4U},0,{_tmp14D,_tmp14D,_tmp14D + 1U},(void*)& Cyc_opt12_spec,{_tmp14E,_tmp14E,_tmp14E + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 999
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0,Cyc_set_stop_after_asmfile};static char _tmp14F[3U]="-S";static char _tmp150[1U]="";static char _tmp151[35U]="Stop after producing assembly code";struct _tuple17 Cyc_opt13_tuple={{_tmp14F,_tmp14F,_tmp14F + 3U},0,{_tmp150,_tmp150,_tmp150 + 1U},(void*)& Cyc_opt13_spec,{_tmp151,_tmp151,_tmp151 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1002
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0,Cyc_set_produce_dependencies};static char _tmp152[3U]="-M";static char _tmp153[1U]="";static char _tmp154[21U]="Produce dependencies";struct _tuple17 Cyc_opt14_tuple={{_tmp152,_tmp152,_tmp152 + 3U},0,{_tmp153,_tmp153,_tmp153 + 1U},(void*)& Cyc_opt14_spec,{_tmp154,_tmp154,_tmp154 + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1005
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1,Cyc_add_cpparg};static char _tmp155[4U]="-MG";static char _tmp156[1U]="";static char _tmp157[68U]="When producing dependencies assume that missing files are generated";struct _tuple17 Cyc_opt15_tuple={{_tmp155,_tmp155,_tmp155 + 4U},0,{_tmp156,_tmp156,_tmp156 + 1U},(void*)& Cyc_opt15_spec,{_tmp157,_tmp157,_tmp157 + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1009
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5,Cyc_set_dependencies_target};static char _tmp158[4U]="-MT";static char _tmp159[10U]=" <target>";static char _tmp15A[29U]="Give target for dependencies";struct _tuple17 Cyc_opt16_tuple={{_tmp158,_tmp158,_tmp158 + 4U},0,{_tmp159,_tmp159,_tmp159 + 10U},(void*)& Cyc_opt16_spec,{_tmp15A,_tmp15A,_tmp15A + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1012
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5,Cyc_Specsfile_set_target_arch};static char _tmp15B[3U]="-b";static char _tmp15C[10U]="<machine>";static char _tmp15D[19U]="Set target machine";struct _tuple17 Cyc_opt17_tuple={{_tmp15B,_tmp15B,_tmp15B + 3U},0,{_tmp15C,_tmp15C,_tmp15C + 10U},(void*)& Cyc_opt17_spec,{_tmp15D,_tmp15D,_tmp15D + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1015
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3,& Cyc_Flags_warn_lose_unique};static char _tmp15E[14U]="-Wlose-unique";static char _tmp15F[1U]="";static char _tmp160[49U]="Try to warn when a unique pointer might get lost";struct _tuple17 Cyc_opt18_tuple={{_tmp15E,_tmp15E,_tmp15E + 14U},0,{_tmp15F,_tmp15F,_tmp15F + 1U},(void*)& Cyc_opt18_spec,{_tmp160,_tmp160,_tmp160 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1018
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3,& Cyc_Flags_warn_override};static char _tmp161[11U]="-Woverride";static char _tmp162[1U]="";static char _tmp163[58U]="Warn when a local variable overrides an existing variable";struct _tuple17 Cyc_opt19_tuple={{_tmp161,_tmp161,_tmp161 + 11U},0,{_tmp162,_tmp162,_tmp162 + 1U},(void*)& Cyc_opt19_spec,{_tmp163,_tmp163,_tmp163 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1021
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0,Cyc_Flags_set_all_warnings};static char _tmp164[6U]="-Wall";static char _tmp165[1U]="";static char _tmp166[21U]="Turn on all warnings";struct _tuple17 Cyc_opt20_tuple={{_tmp164,_tmp164,_tmp164 + 6U},0,{_tmp165,_tmp165,_tmp165 + 1U},(void*)& Cyc_opt20_spec,{_tmp166,_tmp166,_tmp166 + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1024
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt21_spec={0,Cyc_Flags_set_cpponly};static char _tmp167[3U]="-E";static char _tmp168[1U]="";static char _tmp169[25U]="Stop after preprocessing";struct _tuple17 Cyc_opt21_tuple={{_tmp167,_tmp167,_tmp167 + 3U},0,{_tmp168,_tmp168,_tmp168 + 1U},(void*)& Cyc_opt21_spec,{_tmp169,_tmp169,_tmp169 + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1027
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt22_spec={0,Cyc_Flags_set_parseonly};static char _tmp16A[17U]="-stopafter-parse";static char _tmp16B[1U]="";static char _tmp16C[19U]="Stop after parsing";struct _tuple17 Cyc_opt22_tuple={{_tmp16A,_tmp16A,_tmp16A + 17U},0,{_tmp16B,_tmp16B,_tmp16B + 1U},(void*)& Cyc_opt22_spec,{_tmp16C,_tmp16C,_tmp16C + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1030
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt23_spec={0,Cyc_Flags_set_tconly};static char _tmp16D[14U]="-stopafter-tc";static char _tmp16E[1U]="";static char _tmp16F[25U]="Stop after type checking";struct _tuple17 Cyc_opt23_tuple={{_tmp16D,_tmp16D,_tmp16D + 14U},0,{_tmp16E,_tmp16E,_tmp16E + 1U},(void*)& Cyc_opt23_spec,{_tmp16F,_tmp16F,_tmp16F + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1033
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3,& Cyc_noprint_r};static char _tmp170[9U]="-noprint";static char _tmp171[1U]="";static char _tmp172[42U]="Do not print output (when stopping early)";struct _tuple17 Cyc_opt24_tuple={{_tmp170,_tmp170,_tmp170 + 9U},0,{_tmp171,_tmp171,_tmp171 + 1U},(void*)& Cyc_opt24_spec,{_tmp172,_tmp172,_tmp172 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1036
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt25_spec={0,Cyc_Flags_set_cfonly};static char _tmp173[14U]="-stopafter-cf";static char _tmp174[1U]="";static char _tmp175[33U]="Stop after control-flow checking";struct _tuple17 Cyc_opt25_tuple={{_tmp173,_tmp173,_tmp173 + 14U},0,{_tmp174,_tmp174,_tmp174 + 1U},(void*)& Cyc_opt25_spec,{_tmp175,_tmp175,_tmp175 + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1039
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt26_spec={0,Cyc_Flags_set_toconly};static char _tmp176[15U]="-stopafter-toc";static char _tmp177[1U]="";static char _tmp178[28U]="Stop after translation to C";struct _tuple17 Cyc_opt26_tuple={{_tmp176,_tmp176,_tmp176 + 15U},0,{_tmp177,_tmp177,_tmp177 + 1U},(void*)& Cyc_opt26_spec,{_tmp178,_tmp178,_tmp178 + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1042
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3,& Cyc_ic_r};static char _tmp179[4U]="-ic";static char _tmp17A[1U]="";static char _tmp17B[26U]="Activate the link-checker";struct _tuple17 Cyc_opt27_tuple={{_tmp179,_tmp179,_tmp179 + 4U},0,{_tmp17A,_tmp17A,_tmp17A + 1U},(void*)& Cyc_opt27_spec,{_tmp17B,_tmp17B,_tmp17B + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1045
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3,& Cyc_pp_r};static char _tmp17C[4U]="-pp";static char _tmp17D[1U]="";static char _tmp17E[47U]="Pretty print the C code that Cyclone generates";struct _tuple17 Cyc_opt28_tuple={{_tmp17C,_tmp17C,_tmp17C + 4U},0,{_tmp17D,_tmp17D,_tmp17D + 1U},(void*)& Cyc_opt28_spec,{_tmp17E,_tmp17E,_tmp17E + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1048
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4,& Cyc_pp_r};static char _tmp17F[4U]="-up";static char _tmp180[1U]="";static char _tmp181[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple17 Cyc_opt29_tuple={{_tmp17F,_tmp17F,_tmp17F + 4U},0,{_tmp180,_tmp180,_tmp180 + 1U},(void*)& Cyc_opt29_spec,{_tmp181,_tmp181,_tmp181 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1051
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3,& Cyc_elim_se_r};static char _tmp182[28U]="-elim-statement-expressions";static char _tmp183[1U]="";static char _tmp184[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple17 Cyc_opt30_tuple={{_tmp182,_tmp182,_tmp182 + 28U},0,{_tmp183,_tmp183,_tmp183 + 1U},(void*)& Cyc_opt30_spec,{_tmp184,_tmp184,_tmp184 + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1055
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4,& Cyc_elim_se_r};static char _tmp185[31U]="-no-elim-statement-expressions";static char _tmp186[1U]="";static char _tmp187[47U]="Do not avoid statement expressions in C output";struct _tuple17 Cyc_opt31_tuple={{_tmp185,_tmp185,_tmp185 + 31U},0,{_tmp186,_tmp186,_tmp186 + 1U},(void*)& Cyc_opt31_spec,{_tmp187,_tmp187,_tmp187 + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1058
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0,Cyc_set_tovc};static char _tmp188[8U]="-un-gcc";static char _tmp189[1U]="";static char _tmp18A[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple17 Cyc_opt32_tuple={{_tmp188,_tmp188,_tmp188 + 8U},0,{_tmp189,_tmp189,_tmp189 + 1U},(void*)& Cyc_opt32_spec,{_tmp18A,_tmp18A,_tmp18A + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1061
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5,Cyc_set_c_compiler};static char _tmp18B[8U]="-c-comp";static char _tmp18C[12U]=" <compiler>";static char _tmp18D[40U]="Produce C output for the given compiler";struct _tuple17 Cyc_opt33_tuple={{_tmp18B,_tmp18B,_tmp18B + 8U},0,{_tmp18C,_tmp18C,_tmp18C + 12U},(void*)& Cyc_opt33_spec,{_tmp18D,_tmp18D,_tmp18D + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1064
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0,Cyc_set_save_temps};static char _tmp18E[12U]="-save-temps";static char _tmp18F[1U]="";static char _tmp190[29U]="Don't delete temporary files";struct _tuple17 Cyc_opt34_tuple={{_tmp18E,_tmp18E,_tmp18E + 12U},0,{_tmp18F,_tmp18F,_tmp18F + 1U},(void*)& Cyc_opt34_spec,{_tmp190,_tmp190,_tmp190 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1067
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3,& Cyc_save_c_r};static char _tmp191[8U]="-save-c";static char _tmp192[1U]="";static char _tmp193[31U]="Don't delete temporary C files";struct _tuple17 Cyc_opt35_tuple={{_tmp191,_tmp191,_tmp191 + 8U},0,{_tmp192,_tmp192,_tmp192 + 1U},(void*)& Cyc_opt35_spec,{_tmp193,_tmp193,_tmp193 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1070
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0,Cyc_clear_lineno};static char _tmp194[11U]="--nolineno";static char _tmp195[1U]="";static char _tmp196[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple17 Cyc_opt36_tuple={{_tmp194,_tmp194,_tmp194 + 11U},0,{_tmp195,_tmp195,_tmp195 + 1U},(void*)& Cyc_opt36_spec,{_tmp196,_tmp196,_tmp196 + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1073
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0,Cyc_set_nochecks};static char _tmp197[11U]="--nochecks";static char _tmp198[1U]="";static char _tmp199[27U]="Disable bounds/null checks";struct _tuple17 Cyc_opt37_tuple={{_tmp197,_tmp197,_tmp197 + 11U},0,{_tmp198,_tmp198,_tmp198 + 1U},(void*)& Cyc_opt37_spec,{_tmp199,_tmp199,_tmp199 + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1076
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0,Cyc_set_nonullchecks};static char _tmp19A[15U]="--nonullchecks";static char _tmp19B[1U]="";static char _tmp19C[20U]="Disable null checks";struct _tuple17 Cyc_opt38_tuple={{_tmp19A,_tmp19A,_tmp19A + 15U},0,{_tmp19B,_tmp19B,_tmp19B + 1U},(void*)& Cyc_opt38_spec,{_tmp19C,_tmp19C,_tmp19C + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1079
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0,Cyc_set_noboundschecks};static char _tmp19D[17U]="--noboundschecks";static char _tmp19E[1U]="";static char _tmp19F[22U]="Disable bounds checks";struct _tuple17 Cyc_opt39_tuple={{_tmp19D,_tmp19D,_tmp19D + 17U},0,{_tmp19E,_tmp19E,_tmp19E + 1U},(void*)& Cyc_opt39_spec,{_tmp19F,_tmp19F,_tmp19F + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1082
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5,Cyc_set_cpp};static char _tmp1A0[9U]="-use-cpp";static char _tmp1A1[7U]="<path>";static char _tmp1A2[35U]="Indicate which preprocessor to use";struct _tuple17 Cyc_opt41_tuple={{_tmp1A0,_tmp1A0,_tmp1A0 + 9U},0,{_tmp1A1,_tmp1A1,_tmp1A1 + 7U},(void*)& Cyc_opt41_spec,{_tmp1A2,_tmp1A2,_tmp1A2 + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1085
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0,Cyc_set_nocppprecomp};static char _tmp1A3[16U]="-no-cpp-precomp";static char _tmp1A4[1U]="";static char _tmp1A5[40U]="Don't do smart preprocessing (mac only)";struct _tuple17 Cyc_opt42_tuple={{_tmp1A3,_tmp1A3,_tmp1A3 + 16U},0,{_tmp1A4,_tmp1A4,_tmp1A4 + 1U},(void*)& Cyc_opt42_spec,{_tmp1A5,_tmp1A5,_tmp1A5 + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1088
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0,Cyc_set_nocyc};static char _tmp1A6[7U]="-nocyc";static char _tmp1A7[1U]="";static char _tmp1A8[33U]="Don't add implicit namespace Cyc";struct _tuple17 Cyc_opt43_tuple={{_tmp1A6,_tmp1A6,_tmp1A6 + 7U},0,{_tmp1A7,_tmp1A7,_tmp1A7 + 1U},(void*)& Cyc_opt43_spec,{_tmp1A8,_tmp1A8,_tmp1A8 + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1091
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3,& Cyc_nogc_r};static char _tmp1A9[6U]="-nogc";static char _tmp1AA[1U]="";static char _tmp1AB[36U]="Don't link in the garbage collector";struct _tuple17 Cyc_opt44_tuple={{_tmp1A9,_tmp1A9,_tmp1A9 + 6U},0,{_tmp1AA,_tmp1AA,_tmp1AA + 1U},(void*)& Cyc_opt44_spec,{_tmp1AB,_tmp1AB,_tmp1AB + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1094
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3,& Cyc_noshake_r};static char _tmp1AC[16U]="-noremoveunused";static char _tmp1AD[1U]="";static char _tmp1AE[49U]="Don't remove externed variables that aren't used";struct _tuple17 Cyc_opt45_tuple={{_tmp1AC,_tmp1AC,_tmp1AC + 16U},0,{_tmp1AD,_tmp1AD,_tmp1AD + 1U},(void*)& Cyc_opt45_spec,{_tmp1AE,_tmp1AE,_tmp1AE + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1097
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3,& Cyc_Flags_noexpand_r};static char _tmp1AF[18U]="-noexpandtypedefs";static char _tmp1B0[1U]="";static char _tmp1B1[41U]="Don't expand typedefs in pretty printing";struct _tuple17 Cyc_opt46_tuple={{_tmp1AF,_tmp1AF,_tmp1AF + 18U},0,{_tmp1B0,_tmp1B0,_tmp1B0 + 1U},(void*)& Cyc_opt46_spec,{_tmp1B1,_tmp1B1,_tmp1B1 + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1100
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3,& Cyc_print_all_tvars_r};static char _tmp1B2[15U]="-printalltvars";static char _tmp1B3[1U]="";static char _tmp1B4[57U]="Print all type variables (even implicit default effects)";struct _tuple17 Cyc_opt47_tuple={{_tmp1B2,_tmp1B2,_tmp1B2 + 15U},0,{_tmp1B3,_tmp1B3,_tmp1B3 + 1U},(void*)& Cyc_opt47_spec,{_tmp1B4,_tmp1B4,_tmp1B4 + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1103
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3,& Cyc_print_all_kinds_r};static char _tmp1B5[15U]="-printallkinds";static char _tmp1B6[1U]="";static char _tmp1B7[37U]="Always print kinds of type variables";struct _tuple17 Cyc_opt48_tuple={{_tmp1B5,_tmp1B5,_tmp1B5 + 15U},0,{_tmp1B6,_tmp1B6,_tmp1B6 + 1U},(void*)& Cyc_opt48_spec,{_tmp1B7,_tmp1B7,_tmp1B7 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1106
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3,& Cyc_print_full_evars_r};static char _tmp1B8[16U]="-printfullevars";static char _tmp1B9[1U]="";static char _tmp1BA[50U]="Print full information for evars (type debugging)";struct _tuple17 Cyc_opt49_tuple={{_tmp1B8,_tmp1B8,_tmp1B8 + 16U},0,{_tmp1B9,_tmp1B9,_tmp1B9 + 1U},(void*)& Cyc_opt49_spec,{_tmp1BA,_tmp1BA,_tmp1BA + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1109
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3,& Cyc_print_all_effects_r};static char _tmp1BB[17U]="-printalleffects";static char _tmp1BC[1U]="";static char _tmp1BD[45U]="Print effects for functions (type debugging)";struct _tuple17 Cyc_opt50_tuple={{_tmp1BB,_tmp1BB,_tmp1BB + 17U},0,{_tmp1BC,_tmp1BC,_tmp1BC + 1U},(void*)& Cyc_opt50_spec,{_tmp1BD,_tmp1BD,_tmp1BD + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1112
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3,& Cyc_nocyc_setjmp_r};static char _tmp1BE[14U]="-nocyc_setjmp";static char _tmp1BF[1U]="";static char _tmp1C0[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple17 Cyc_opt51_tuple={{_tmp1BE,_tmp1BE,_tmp1BE + 14U},0,{_tmp1BF,_tmp1BF,_tmp1BF + 1U},(void*)& Cyc_opt51_spec,{_tmp1C0,_tmp1C0,_tmp1C0 + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1115
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3,& Cyc_Flags_compile_for_boot};static char _tmp1C1[18U]="-compile-for-boot";static char _tmp1C2[1U]="";static char _tmp1C3[71U]="Compile using the special boot library instead of the standard library";struct _tuple17 Cyc_opt52_tuple={{_tmp1C1,_tmp1C1,_tmp1C1 + 18U},0,{_tmp1C2,_tmp1C2,_tmp1C2 + 1U},(void*)& Cyc_opt52_spec,{_tmp1C3,_tmp1C3,_tmp1C3 + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1118
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5,Cyc_set_cyc_include};static char _tmp1C4[4U]="-CI";static char _tmp1C5[8U]=" <file>";static char _tmp1C6[31U]="Set cyc_include.h to be <file>";struct _tuple17 Cyc_opt53_tuple={{_tmp1C4,_tmp1C4,_tmp1C4 + 4U},0,{_tmp1C5,_tmp1C5,_tmp1C5 + 8U},(void*)& Cyc_opt53_spec,{_tmp1C6,_tmp1C6,_tmp1C6 + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1121
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3,& Cyc_Flags_warn_bounds_checks};static char _tmp1C7[18U]="-warnboundschecks";static char _tmp1C8[1U]="";static char _tmp1C9[44U]="Warn when bounds checks can't be eliminated";struct _tuple17 Cyc_opt54_tuple={{_tmp1C7,_tmp1C7,_tmp1C7 + 18U},0,{_tmp1C8,_tmp1C8,_tmp1C8 + 1U},(void*)& Cyc_opt54_spec,{_tmp1C9,_tmp1C9,_tmp1C9 + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1124
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3,& Cyc_Flags_warn_all_null_deref};static char _tmp1CA[16U]="-warnnullchecks";static char _tmp1CB[1U]="";static char _tmp1CC[45U]="Warn when any null check can't be eliminated";struct _tuple17 Cyc_opt55_tuple={{_tmp1CA,_tmp1CA,_tmp1CA + 16U},0,{_tmp1CB,_tmp1CB,_tmp1CB + 1U},(void*)& Cyc_opt55_spec,{_tmp1CC,_tmp1CC,_tmp1CC + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1127
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3,& Cyc_Flags_warn_alias_coerce};static char _tmp1CD[19U]="-warnaliascoercion";static char _tmp1CE[1U]="";static char _tmp1CF[41U]="Warn when any alias coercion is inserted";struct _tuple17 Cyc_opt56_tuple={{_tmp1CD,_tmp1CD,_tmp1CD + 19U},0,{_tmp1CE,_tmp1CE,_tmp1CE + 1U},(void*)& Cyc_opt56_spec,{_tmp1CF,_tmp1CF,_tmp1CF + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1130
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3,& Cyc_Flags_warn_region_coerce};static char _tmp1D0[18U]="-warnregioncoerce";static char _tmp1D1[1U]="";static char _tmp1D2[42U]="Warn when any region coercion is inserted";struct _tuple17 Cyc_opt57_tuple={{_tmp1D0,_tmp1D0,_tmp1D0 + 18U},0,{_tmp1D1,_tmp1D1,_tmp1D1 + 1U},(void*)& Cyc_opt57_spec,{_tmp1D2,_tmp1D2,_tmp1D2 + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1133
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3,& Cyc_Flags_no_register};static char _tmp1D3[12U]="-noregister";static char _tmp1D4[1U]="";static char _tmp1D5[39U]="Treat register storage class as public";struct _tuple17 Cyc_opt58_tuple={{_tmp1D3,_tmp1D3,_tmp1D3 + 12U},0,{_tmp1D4,_tmp1D4,_tmp1D4 + 1U},(void*)& Cyc_opt58_spec,{_tmp1D5,_tmp1D5,_tmp1D5 + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1136
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4,& Cyc_Position_use_gcc_style_location};static char _tmp1D6[18U]="-detailedlocation";static char _tmp1D7[1U]="";static char _tmp1D8[58U]="Try to give more detailed location information for errors";struct _tuple17 Cyc_opt59_tuple={{_tmp1D6,_tmp1D6,_tmp1D6 + 18U},0,{_tmp1D7,_tmp1D7,_tmp1D7 + 1U},(void*)& Cyc_opt59_spec,{_tmp1D8,_tmp1D8,_tmp1D8 + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1139
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0,Cyc_set_port_c_code};static char _tmp1D9[6U]="-port";static char _tmp1DA[1U]="";static char _tmp1DB[38U]="Suggest how to port C code to Cyclone";struct _tuple17 Cyc_opt60_tuple={{_tmp1D9,_tmp1D9,_tmp1D9 + 6U},0,{_tmp1DA,_tmp1DA,_tmp1DA + 1U},(void*)& Cyc_opt60_spec,{_tmp1DB,_tmp1DB,_tmp1DB + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1142
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3,& Cyc_Flags_no_regions};static char _tmp1DC[11U]="-noregions";static char _tmp1DD[1U]="";static char _tmp1DE[39U]="Generate code that doesn't use regions";struct _tuple17 Cyc_opt61_tuple={{_tmp1DC,_tmp1DC,_tmp1DC + 11U},0,{_tmp1DD,_tmp1DD,_tmp1DD + 1U},(void*)& Cyc_opt61_spec,{_tmp1DE,_tmp1DE,_tmp1DE + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1145
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5,Cyc_add_iprefix};static char _tmp1DF[9U]="-iprefix";static char _tmp1E0[9U]="<prefix>";static char _tmp1E1[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple17 Cyc_opt62_tuple={{_tmp1DF,_tmp1DF,_tmp1DF + 9U},0,{_tmp1E0,_tmp1E0,_tmp1E0 + 9U},(void*)& Cyc_opt62_spec,{_tmp1E1,_tmp1E1,_tmp1E1 + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1148
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5,Cyc_add_iwithprefix};static char _tmp1E2[13U]="-iwithprefix";static char _tmp1E3[6U]="<dir>";static char _tmp1E4[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple17 Cyc_opt63_tuple={{_tmp1E2,_tmp1E2,_tmp1E2 + 13U},0,{_tmp1E3,_tmp1E3,_tmp1E3 + 6U},(void*)& Cyc_opt63_spec,{_tmp1E4,_tmp1E4,_tmp1E4 + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1151
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5,Cyc_add_iwithprefixbefore};static char _tmp1E5[19U]="-iwithprefixbefore";static char _tmp1E6[6U]="<dir>";static char _tmp1E7[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple17 Cyc_opt64_tuple={{_tmp1E5,_tmp1E5,_tmp1E5 + 19U},0,{_tmp1E6,_tmp1E6,_tmp1E6 + 6U},(void*)& Cyc_opt64_spec,{_tmp1E7,_tmp1E7,_tmp1E7 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1154
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5,Cyc_add_isystem};static char _tmp1E8[9U]="-isystem";static char _tmp1E9[6U]="<dir>";static char _tmp1EA[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple17 Cyc_opt65_tuple={{_tmp1E8,_tmp1E8,_tmp1E8 + 9U},0,{_tmp1E9,_tmp1E9,_tmp1E9 + 6U},(void*)& Cyc_opt65_spec,{_tmp1EA,_tmp1EA,_tmp1EA + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1157
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5,Cyc_add_idirafter};static char _tmp1EB[11U]="-idirafter";static char _tmp1EC[6U]="<dir>";static char _tmp1ED[46U]="Add the directory to the second include path.";struct _tuple17 Cyc_opt66_tuple={{_tmp1EB,_tmp1EB,_tmp1EB + 11U},0,{_tmp1EC,_tmp1EC,_tmp1EC + 6U},(void*)& Cyc_opt66_spec,{_tmp1ED,_tmp1ED,_tmp1ED + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1160
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3,& Cyc_generate_interface_r};static char _tmp1EE[15U]="--geninterface";static char _tmp1EF[1U]="";static char _tmp1F0[25U]="Generate interface files";struct _tuple17 Cyc_opt67_tuple={{_tmp1EE,_tmp1EE,_tmp1EE + 15U},0,{_tmp1EF,_tmp1EF,_tmp1EF + 1U},(void*)& Cyc_opt67_spec,{_tmp1F0,_tmp1F0,_tmp1F0 + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1163
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5,Cyc_set_specified_interface};static char _tmp1F1[12U]="--interface";static char _tmp1F2[8U]=" <file>";static char _tmp1F3[37U]="Set the interface file to be <file>.";struct _tuple17 Cyc_opt68_tuple={{_tmp1F1,_tmp1F1,_tmp1F1 + 12U},0,{_tmp1F2,_tmp1F2,_tmp1F2 + 8U},(void*)& Cyc_opt68_spec,{_tmp1F3,_tmp1F3,_tmp1F3 + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1166
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0,Cyc_set_many_errors};static char _tmp1F4[13U]="--manyerrors";static char _tmp1F5[1U]="";static char _tmp1F6[36U]="don't stop after only a few errors.";struct _tuple17 Cyc_opt69_tuple={{_tmp1F4,_tmp1F4,_tmp1F4 + 13U},0,{_tmp1F5,_tmp1F5,_tmp1F5 + 1U},(void*)& Cyc_opt69_spec,{_tmp1F6,_tmp1F6,_tmp1F6 + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1169
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3,& Cyc_Flags_print_parser_state_and_token};static char _tmp1F7[13U]="--parsestate";static char _tmp1F8[1U]="";static char _tmp1F9[50U]="print the parse state and token on a syntax error";struct _tuple17 Cyc_opt70_tuple={{_tmp1F7,_tmp1F7,_tmp1F7 + 13U},0,{_tmp1F8,_tmp1F8,_tmp1F8 + 1U},(void*)& Cyc_opt70_spec,{_tmp1F9,_tmp1F9,_tmp1F9 + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1172
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0,Cyc_noassume_gcc_flag};static char _tmp1FA[22U]="-known-gcc-flags-only";static char _tmp1FB[1U]="";static char _tmp1FC[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple17 Cyc_opt71_tuple={{_tmp1FA,_tmp1FA,_tmp1FA + 22U},0,{_tmp1FB,_tmp1FB,_tmp1FB + 1U},(void*)& Cyc_opt71_spec,{_tmp1FC,_tmp1FC,_tmp1FC + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1175
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0,Cyc_print_options};static char _tmp1FD[6U]="-help";static char _tmp1FE[1U]="";static char _tmp1FF[32U]="print out the available options";struct _tuple17 Cyc_opt72_tuple={{_tmp1FD,_tmp1FD,_tmp1FD + 6U},0,{_tmp1FE,_tmp1FE,_tmp1FE + 1U},(void*)& Cyc_opt72_spec,{_tmp1FF,_tmp1FF,_tmp1FF + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1178
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0,Cyc_print_options};static char _tmp200[7U]="-usage";static char _tmp201[1U]="";static char _tmp202[32U]="print out the available options";struct _tuple17 Cyc_opt73_tuple={{_tmp200,_tmp200,_tmp200 + 7U},0,{_tmp201,_tmp201,_tmp201 + 1U},(void*)& Cyc_opt73_spec,{_tmp202,_tmp202,_tmp202 + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1181
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0,Cyc_set_notoseqc};static char _tmp203[10U]="-no-seq-c";static char _tmp204[1U]="";static char _tmp205[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple17 Cyc_opt74_tuple={{_tmp203,_tmp203,_tmp203 + 10U},0,{_tmp204,_tmp204,_tmp204 + 1U},(void*)& Cyc_opt74_spec,{_tmp205,_tmp205,_tmp205 + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1184
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0,Cyc_set_pg};static char _tmp206[4U]="-pg";static char _tmp207[1U]="";static char _tmp208[33U]="Compile for profiling with gprof";struct _tuple17 Cyc_opt75_tuple={{_tmp206,_tmp206,_tmp206 + 4U},0,{_tmp207,_tmp207,_tmp207 + 1U},(void*)& Cyc_opt75_spec,{_tmp208,_tmp208,_tmp208 + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1187
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt76_spec={4,& Cyc_vcgen_r};static char _tmp209[10U]="--novcgen";static char _tmp20A[1U]="";static char _tmp20B[39U]="do not compute verification-conditions";struct _tuple17 Cyc_opt76_tuple={{_tmp209,_tmp209,_tmp209 + 10U},0,{_tmp20A,_tmp20A,_tmp20A + 1U},(void*)& Cyc_opt76_spec,{_tmp20B,_tmp20B,_tmp20B + 39U}};struct Cyc_List_List Cyc_opt76={(void*)& Cyc_opt76_tuple,0};
# 1190
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt77_spec={0,Cyc_Cifc_set_inst_tvar};static char _tmp20C[17U]="--cifc-inst-tvar";static char _tmp20D[1U]="";static char _tmp20E[59U]="instantiate type variables instead of introducing new ones";struct _tuple17 Cyc_opt77_tuple={{_tmp20C,_tmp20C,_tmp20C + 17U},0,{_tmp20D,_tmp20D,_tmp20D + 1U},(void*)& Cyc_opt77_spec,{_tmp20E,_tmp20E,_tmp20E + 59U}};struct Cyc_List_List Cyc_opt77={(void*)& Cyc_opt77_tuple,0};
# 1193
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt78_spec={0,Cyc_Flags_set_crgnonly};static char _tmp20F[16U]="-stopafter-crgn";static char _tmp210[1U]="";static char _tmp211[19U]="Stop after currrgn";struct _tuple17 Cyc_opt78_tuple={{_tmp20F,_tmp20F,_tmp20F + 16U},0,{_tmp210,_tmp210,_tmp210 + 1U},(void*)& Cyc_opt78_spec,{_tmp211,_tmp211,_tmp211 + 19U}};struct Cyc_List_List Cyc_opt78={(void*)& Cyc_opt78_tuple,0};
# 1196
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt79_spec={3,& Cyc_Flags_allpaths};static char _tmp212[11U]="--allpaths";static char _tmp213[1U]="";static char _tmp214[62U]="analyze all forward paths in verification-condition generator";struct _tuple17 Cyc_opt79_tuple={{_tmp212,_tmp212,_tmp212 + 11U},0,{_tmp213,_tmp213,_tmp213 + 1U},(void*)& Cyc_opt79_spec,{_tmp214,_tmp214,_tmp214 + 62U}};struct Cyc_List_List Cyc_opt79={(void*)& Cyc_opt79_tuple,0};
# 1199
struct Cyc_List_List*Cyc_global_options[78U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73,& Cyc_opt74,& Cyc_opt75,& Cyc_opt76,& Cyc_opt77,& Cyc_opt78,& Cyc_opt79};
# 1213
void Cyc_print_options (void){
({struct Cyc_List_List*_tmp374=Cyc_global_options[0];Cyc_Arg_usage(_tmp374,({const char*_tmp215="<program.cyc>";_tag_fat(_tmp215,sizeof(char),14U);}));});}
# 1218
int Cyc_main(int argc,struct _fat_ptr argv){
# 1221
GC_blacklist_warn_clear();{
# 1224
struct _fat_ptr optstring=({const char*_tmp296="Options:";_tag_fat(_tmp296,sizeof(char),9U);});
# 1226
{int i=1;for(0;(unsigned)i < 78U;++ i){
(*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,78U,sizeof(struct Cyc_List_List*),i - 1)))->tl=Cyc_global_options[i];}}{
struct Cyc_List_List*options=Cyc_global_options[0];
# 1230
struct _fat_ptr _tmp216=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);struct _fat_ptr otherargs=_tmp216;
# 1236
struct _fat_ptr _tmp217=Cyc_find_in_arch_path(({const char*_tmp295="cycspecs";_tag_fat(_tmp295,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp217;
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp2BB;_tmp2BB.tag=0,_tmp2BB.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2BB;});void*_tmp218[1];_tmp218[0]=& _tmp21A;({struct Cyc___cycFILE*_tmp376=Cyc_stderr;struct _fat_ptr _tmp375=({const char*_tmp219="Reading from specs file %s\n";_tag_fat(_tmp219,sizeof(char),28U);});Cyc_fprintf(_tmp376,_tmp375,_tag_fat(_tmp218,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp21B=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp21B;
# 1240
struct _fat_ptr _tmp21C=Cyc_Specsfile_split_specs(({struct Cyc_List_List*_tmp377=specs;Cyc_Specsfile_get_spec(_tmp377,({const char*_tmp294="cyclone";_tag_fat(_tmp294,sizeof(char),8U);}));}));struct _fat_ptr args_from_specs_file=_tmp21C;
if((struct _fat_ptr*)args_from_specs_file.curr != (struct _fat_ptr*)(_tag_fat(0,0,0)).curr){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,args_from_specs_file);}{
# 1245
struct _fat_ptr _tmp21D=({struct Cyc_List_List*_tmp378=specs;Cyc_Specsfile_get_spec(_tmp378,({const char*_tmp293="cyclone_target_cflags";_tag_fat(_tmp293,sizeof(char),22U);}));});struct _fat_ptr target_cflags=_tmp21D;
struct _fat_ptr _tmp21E=({struct Cyc_List_List*_tmp379=specs;Cyc_Specsfile_get_spec(_tmp379,({const char*_tmp292="cyclone_cc";_tag_fat(_tmp292,sizeof(char),11U);}));});struct _fat_ptr cyclone_cc=_tmp21E;
if(!((unsigned)cyclone_cc.curr))cyclone_cc=({const char*_tmp21F="gcc";_tag_fat(_tmp21F,sizeof(char),4U);});
Cyc_def_inc_path=({struct Cyc_List_List*_tmp37A=specs;Cyc_Specsfile_get_spec(_tmp37A,({const char*_tmp220="cyclone_inc_path";_tag_fat(_tmp220,sizeof(char),17U);}));});
# 1251
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,otherargs);
if(({struct _fat_ptr _tmp37B=(struct _fat_ptr)Cyc_cpp;Cyc_strcmp(_tmp37B,({const char*_tmp221="";_tag_fat(_tmp221,sizeof(char),1U);}));})== 0){
# 1269 "cyclone.cyc"
const char*_tmp222=Cyc_produce_dependencies?"":" -E";const char*dash_E=_tmp222;
Cyc_set_cpp((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp225=({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0,_tmp2BF.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2BF;});struct Cyc_String_pa_PrintArg_struct _tmp226=({struct Cyc_String_pa_PrintArg_struct _tmp2BE;_tmp2BE.tag=0,_tmp2BE.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2BE;});struct Cyc_String_pa_PrintArg_struct _tmp227=({struct Cyc_String_pa_PrintArg_struct _tmp2BD;_tmp2BD.tag=0,({
struct _fat_ptr _tmp37C=(struct _fat_ptr)({const char*_tmp229=dash_E;_tag_fat(_tmp229,sizeof(char),_get_zero_arr_size_char((void*)_tmp229,1U));});_tmp2BD.f1=_tmp37C;});_tmp2BD;});struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=0,_tmp2BC.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2BC;});void*_tmp223[4];_tmp223[0]=& _tmp225,_tmp223[1]=& _tmp226,_tmp223[2]=& _tmp227,_tmp223[3]=& _tmp228;({struct _fat_ptr _tmp37D=({const char*_tmp224="%s %s -w -x c%s -specs %s";_tag_fat(_tmp224,sizeof(char),26U);});Cyc_aprintf(_tmp37D,_tag_fat(_tmp223,sizeof(void*),4));});}));}
# 1273
if(Cyc_cyclone_files == 0 && Cyc_ccargs == 0){
({void*_tmp22A=0U;({struct Cyc___cycFILE*_tmp37F=Cyc_stderr;struct _fat_ptr _tmp37E=({const char*_tmp22B="missing file\n";_tag_fat(_tmp22B,sizeof(char),14U);});Cyc_fprintf(_tmp37F,_tmp37E,_tag_fat(_tmp22A,sizeof(void*),0));});});
exit(1);}
# 1282
if((int)Cyc_Flags_stop_after_pass > 0 && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_find_in_arch_path(({const char*_tmp22C="cyc_setjmp.h";_tag_fat(_tmp22C,sizeof(char),13U);}));
if((int)Cyc_Flags_stop_after_pass > 0 && Cyc_strlen((struct _fat_ptr)Cyc_cyc_include)== 0U)
Cyc_cyc_include=Cyc_find_in_exec_path(({const char*_tmp22D="cyc_include.h";_tag_fat(_tmp22D,sizeof(char),14U);}));
# 1288
{struct _handler_cons _tmp22E;_push_handler(& _tmp22E);{int _tmp230=0;if(setjmp(_tmp22E.handler))_tmp230=1;if(!_tmp230){
{struct Cyc_List_List*_tmp231=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_cyclone_files);struct Cyc_List_List*l=_tmp231;for(0;l != 0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));
if(Cyc_compile_failure){int _tmp232=1;_npop_handler(0);return _tmp232;}}}
# 1289
;_pop_handler();}else{void*_tmp22F=(void*)Cyc_Core_get_exn_thrown();void*_tmp233=_tmp22F;void*_tmp234;_tmp234=_tmp233;{void*x=_tmp234;
# 1294
if(Cyc_compile_failure)return 1;
Cyc_Core_rethrow(x);};}}}
# 1298
if((int)Cyc_Flags_stop_after_pass < 13)return 0;
# 1304
if(Cyc_ccargs == 0)return 0;
# 1307
if((unsigned)Cyc_Specsfile_target_arch.curr &&(unsigned)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg((struct _fat_ptr)({struct Cyc_List_List*_tmp383=({struct Cyc_List_List*_tmp237=_cycalloc(sizeof(*_tmp237));({struct _fat_ptr*_tmp382=({struct _fat_ptr*_tmp236=_cycalloc(sizeof(*_tmp236));({struct _fat_ptr _tmp381=({const char*_tmp235="";_tag_fat(_tmp235,sizeof(char),1U);});*_tmp236=_tmp381;});_tmp236;});_tmp237->hd=_tmp382;}),({
struct Cyc_List_List*_tmp380=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_tmp237->tl=_tmp380;});_tmp237;});
# 1308
Cyc_str_sepstr(_tmp383,({const char*_tmp238=" -L";_tag_fat(_tmp238,sizeof(char),4U);}));}));
# 1311
Cyc_add_ccarg((struct _fat_ptr)({struct _fat_ptr _tmp384=({const char*_tmp239="-L";_tag_fat(_tmp239,sizeof(char),3U);});Cyc_strconcat(_tmp384,(struct _fat_ptr)Cyc_Specsfile_def_lib_path);}));
# 1313
Cyc_ccargs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_ccargs);{
struct _fat_ptr _tmp23A=({struct Cyc_List_List*_tmp385=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_tmp385,({const char*_tmp291=" ";_tag_fat(_tmp291,sizeof(char),2U);}));});struct _fat_ptr ccargs_string=_tmp23A;
Cyc_libargs=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev;})(Cyc_libargs);{
struct _fat_ptr _tmp23B=({struct Cyc_List_List*_tmp389=({struct Cyc_List_List*_tmp28F=_cycalloc(sizeof(*_tmp28F));({struct _fat_ptr*_tmp388=({struct _fat_ptr*_tmp28E=_cycalloc(sizeof(*_tmp28E));({struct _fat_ptr _tmp387=({const char*_tmp28D="";_tag_fat(_tmp28D,sizeof(char),1U);});*_tmp28E=_tmp387;});_tmp28E;});_tmp28F->hd=_tmp388;}),({
struct Cyc_List_List*_tmp386=({(struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map;})(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp28F->tl=_tmp386;});_tmp28F;});
# 1316
Cyc_str_sepstr(_tmp389,({const char*_tmp290=" ";_tag_fat(_tmp290,sizeof(char),2U);}));});struct _fat_ptr libargs_string=_tmp23B;
# 1319
struct Cyc_List_List*stdlib;
struct _fat_ptr stdlib_string;
int _tmp23C=
((Cyc_stop_after_asmfile_r || Cyc_stop_after_objectfile_r)||
# 1324
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp38B=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp38B,({const char*_tmp28B=".a";_tag_fat(_tmp28B,sizeof(char),3U);}));}))||
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp38A=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp38A,({const char*_tmp28C=".lib";_tag_fat(_tmp28C,sizeof(char),5U);}));});
# 1321
int is_not_executable=_tmp23C;
# 1326
if(is_not_executable){
stdlib=0;
stdlib_string=(struct _fat_ptr)({const char*_tmp23D="";_tag_fat(_tmp23D,sizeof(char),1U);});}else{
# 1331
struct _fat_ptr libcyc_flag;
struct _fat_ptr nogc_filename;
struct _fat_ptr runtime_filename;
struct _fat_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=({const char*_tmp23E="-lcyc_a";_tag_fat(_tmp23E,sizeof(char),8U);});
nogc_filename=({const char*_tmp23F="nogc_a.a";_tag_fat(_tmp23F,sizeof(char),9U);});
runtime_filename=({const char*_tmp240="runtime_cyc_a.a";_tag_fat(_tmp240,sizeof(char),16U);});}else{
if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp241="-lcyc_nocheck";_tag_fat(_tmp241,sizeof(char),14U);});
nogc_filename=({const char*_tmp242="nogc.a";_tag_fat(_tmp242,sizeof(char),7U);});
runtime_filename=({const char*_tmp243="runtime_cyc.a";_tag_fat(_tmp243,sizeof(char),14U);});}else{
if(Cyc_pg_r){
libcyc_flag=({const char*_tmp244="-lcyc_pg";_tag_fat(_tmp244,sizeof(char),9U);});
runtime_filename=({const char*_tmp245="runtime_cyc_pg.a";_tag_fat(_tmp245,sizeof(char),17U);});
nogc_filename=({const char*_tmp246="nogc_pg.a";_tag_fat(_tmp246,sizeof(char),10U);});}else{
if(Cyc_Flags_compile_for_boot){
# 1349
libcyc_flag=({const char*_tmp247="-lcycboot";_tag_fat(_tmp247,sizeof(char),10U);});
nogc_filename=({const char*_tmp248="nogc.a";_tag_fat(_tmp248,sizeof(char),7U);});
runtime_filename=({const char*_tmp249="runtime_cyc.a";_tag_fat(_tmp249,sizeof(char),14U);});}else{
if(Cyc_pthread_r){
libcyc_flag=({const char*_tmp24A="-lcyc";_tag_fat(_tmp24A,sizeof(char),6U);});
nogc_filename=({const char*_tmp24B="nogc_pthread.a";_tag_fat(_tmp24B,sizeof(char),15U);});
runtime_filename=({const char*_tmp24C="runtime_cyc_pthread.a";_tag_fat(_tmp24C,sizeof(char),22U);});}else{
# 1357
libcyc_flag=({const char*_tmp24D="-lcyc";_tag_fat(_tmp24D,sizeof(char),6U);});
nogc_filename=({const char*_tmp24E="nogc.a";_tag_fat(_tmp24E,sizeof(char),7U);});
runtime_filename=({const char*_tmp24F="runtime_cyc.a";_tag_fat(_tmp24F,sizeof(char),14U);});}}}}}
# 1361
if(!Cyc_pthread_r)
gc_filename=({const char*_tmp250="gc.a";_tag_fat(_tmp250,sizeof(char),5U);});else{
# 1364
gc_filename=({const char*_tmp251="gc_pthread.a";_tag_fat(_tmp251,sizeof(char),13U);});}{
# 1366
struct _fat_ptr _tmp252=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);struct _fat_ptr gc=_tmp252;
struct _fat_ptr _tmp253=Cyc_find_in_arch_path(runtime_filename);struct _fat_ptr runtime=_tmp253;
# 1370
stdlib=0;
stdlib_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp256=({struct Cyc_String_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=0,_tmp2C2.f1=(struct _fat_ptr)((struct _fat_ptr)libcyc_flag);_tmp2C2;});struct Cyc_String_pa_PrintArg_struct _tmp257=({struct Cyc_String_pa_PrintArg_struct _tmp2C1;_tmp2C1.tag=0,_tmp2C1.f1=(struct _fat_ptr)((struct _fat_ptr)runtime);_tmp2C1;});struct Cyc_String_pa_PrintArg_struct _tmp258=({struct Cyc_String_pa_PrintArg_struct _tmp2C0;_tmp2C0.tag=0,_tmp2C0.f1=(struct _fat_ptr)((struct _fat_ptr)gc);_tmp2C0;});void*_tmp254[3];_tmp254[0]=& _tmp256,_tmp254[1]=& _tmp257,_tmp254[2]=& _tmp258;({struct _fat_ptr _tmp38C=({const char*_tmp255=" %s %s %s ";_tag_fat(_tmp255,sizeof(char),11U);});Cyc_aprintf(_tmp38C,_tag_fat(_tmp254,sizeof(void*),3));});});}}
# 1374
if(Cyc_ic_r){struct _handler_cons _tmp259;_push_handler(& _tmp259);{int _tmp25B=0;if(setjmp(_tmp259.handler))_tmp25B=1;if(!_tmp25B){
Cyc_ccargs=({(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=({(struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter;})(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp25C=({struct Cyc_List_List*(*_tmp38E)(struct Cyc_List_List*,struct Cyc_List_List*)=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;});struct Cyc_List_List*_tmp38D=stdlib;_tmp38E(_tmp38D,({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append;})(Cyc_ccargs,Cyc_libargs));});struct Cyc_List_List*lf=_tmp25C;
if(!is_not_executable)
lf=({struct Cyc_List_List*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->hd=Cyc_final_strptr,_tmp25D->tl=lf;_tmp25D;});{
# 1381
struct Cyc_Interface_I*_tmp25E=({(struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*)(struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_Interface_get_and_merge_list;})(Cyc_read_cycio,lf,lf);struct Cyc_Interface_I*i=_tmp25E;
if(i == 0){
({void*_tmp25F=0U;({struct Cyc___cycFILE*_tmp390=Cyc_stderr;struct _fat_ptr _tmp38F=({const char*_tmp260="Error: interfaces incompatible\n";_tag_fat(_tmp260,sizeof(char),32U);});Cyc_fprintf(_tmp390,_tmp38F,_tag_fat(_tmp25F,sizeof(void*),0));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp261=1;_npop_handler(0);return _tmp261;}}
# 1388
if(is_not_executable){
if(Cyc_output_file != 0){
struct _fat_ptr _tmp262=({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp2C3;_tmp2C3.tag=0,({struct _fat_ptr _tmp391=(struct _fat_ptr)((struct _fat_ptr)Cyc_Filename_chop_extension(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2C3.f1=_tmp391;});_tmp2C3;});void*_tmp267[1];_tmp267[0]=& _tmp269;({struct _fat_ptr _tmp392=({const char*_tmp268="%s.cycio";_tag_fat(_tmp268,sizeof(char),9U);});Cyc_aprintf(_tmp392,_tag_fat(_tmp267,sizeof(void*),1));});});struct _fat_ptr output_file_io=_tmp262;
struct Cyc___cycFILE*_tmp263=({struct _fat_ptr _tmp394=(struct _fat_ptr)output_file_io;struct _fat_ptr _tmp393=({const char*_tmp265="wb";_tag_fat(_tmp265,sizeof(char),3U);});Cyc_try_file_open(_tmp394,_tmp393,({const char*_tmp266="interface object file";_tag_fat(_tmp266,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp263;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp264=1;_npop_handler(0);return _tmp264;}}
# 1397
Cyc_Interface_save(i,f);
Cyc_file_close(f);}}else{
# 1401
if(!({struct Cyc_Interface_I*_tmp398=Cyc_Interface_empty();struct Cyc_Interface_I*_tmp397=i;Cyc_Interface_is_subinterface(_tmp398,_tmp397,({struct _tuple12*_tmp26C=_cycalloc(sizeof(*_tmp26C));
({struct _fat_ptr _tmp396=({const char*_tmp26A="empty interface";_tag_fat(_tmp26A,sizeof(char),16U);});_tmp26C->f1=_tmp396;}),({struct _fat_ptr _tmp395=({const char*_tmp26B="global interface";_tag_fat(_tmp26B,sizeof(char),17U);});_tmp26C->f2=_tmp395;});_tmp26C;}));})){
({void*_tmp26D=0U;({struct Cyc___cycFILE*_tmp39A=Cyc_stderr;struct _fat_ptr _tmp399=({const char*_tmp26E="Error: some objects are still undefined\n";_tag_fat(_tmp26E,sizeof(char),41U);});Cyc_fprintf(_tmp39A,_tmp399,_tag_fat(_tmp26D,sizeof(void*),0));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp26F=1;_npop_handler(0);return _tmp26F;}}}}}
# 1375
;_pop_handler();}else{void*_tmp25A=(void*)Cyc_Core_get_exn_thrown();void*_tmp270=_tmp25A;void*_tmp271;_tmp271=_tmp270;{void*e=_tmp271;
# 1411
({struct Cyc_String_pa_PrintArg_struct _tmp274=({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0,({struct _fat_ptr _tmp39B=(struct _fat_ptr)({const char*_tmp278=Cyc_Core_get_exn_name(e);_tag_fat(_tmp278,sizeof(char),_get_zero_arr_size_char((void*)_tmp278,1U));});_tmp2C6.f1=_tmp39B;});_tmp2C6;});struct Cyc_String_pa_PrintArg_struct _tmp275=({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0,({struct _fat_ptr _tmp39C=(struct _fat_ptr)({const char*_tmp277=Cyc_Core_get_exn_filename();_tag_fat(_tmp277,sizeof(char),_get_zero_arr_size_char((void*)_tmp277,1U));});_tmp2C5.f1=_tmp39C;});_tmp2C5;});struct Cyc_Int_pa_PrintArg_struct _tmp276=({struct Cyc_Int_pa_PrintArg_struct _tmp2C4;_tmp2C4.tag=1,({unsigned long _tmp39D=(unsigned long)Cyc_Core_get_exn_lineno();_tmp2C4.f1=_tmp39D;});_tmp2C4;});void*_tmp272[3];_tmp272[0]=& _tmp274,_tmp272[1]=& _tmp275,_tmp272[2]=& _tmp276;({struct Cyc___cycFILE*_tmp39F=Cyc_stderr;struct _fat_ptr _tmp39E=({const char*_tmp273="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_fat(_tmp273,sizeof(char),131U);});Cyc_fprintf(_tmp39F,_tmp39E,_tag_fat(_tmp272,sizeof(void*),3));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;};}}}{
# 1417
struct _fat_ptr outfile_str=({const char*_tmp28A="";_tag_fat(_tmp28A,sizeof(char),1U);});
if(Cyc_output_file != 0)
outfile_str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27B=({struct Cyc_String_pa_PrintArg_struct _tmp2C7;_tmp2C7.tag=0,({struct _fat_ptr _tmp3A0=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2C7.f1=_tmp3A0;});_tmp2C7;});void*_tmp279[1];_tmp279[0]=& _tmp27B;({struct _fat_ptr _tmp3A1=({const char*_tmp27A=" -o %s";_tag_fat(_tmp27A,sizeof(char),7U);});Cyc_aprintf(_tmp3A1,_tag_fat(_tmp279,sizeof(void*),1));});});{
# 1421
struct _fat_ptr _tmp27C=({struct Cyc_String_pa_PrintArg_struct _tmp284=({struct Cyc_String_pa_PrintArg_struct _tmp2CE;_tmp2CE.tag=0,_tmp2CE.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2CE;});struct Cyc_String_pa_PrintArg_struct _tmp285=({struct Cyc_String_pa_PrintArg_struct _tmp2CD;_tmp2CD.tag=0,_tmp2CD.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2CD;});struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp2CC;_tmp2CC.tag=0,_tmp2CC.f1=(struct _fat_ptr)((struct _fat_ptr)ccargs_string);_tmp2CC;});struct Cyc_String_pa_PrintArg_struct _tmp287=({struct Cyc_String_pa_PrintArg_struct _tmp2CB;_tmp2CB.tag=0,_tmp2CB.f1=(struct _fat_ptr)((struct _fat_ptr)outfile_str);_tmp2CB;});struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp2CA;_tmp2CA.tag=0,_tmp2CA.f1=(struct _fat_ptr)((struct _fat_ptr)stdlib_string);_tmp2CA;});struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp2C9;_tmp2C9.tag=0,_tmp2C9.f1=(struct _fat_ptr)((struct _fat_ptr)libargs_string);_tmp2C9;});void*_tmp282[6];_tmp282[0]=& _tmp284,_tmp282[1]=& _tmp285,_tmp282[2]=& _tmp286,_tmp282[3]=& _tmp287,_tmp282[4]=& _tmp288,_tmp282[5]=& _tmp289;({struct _fat_ptr _tmp3A2=({const char*_tmp283="%s %s %s%s %s%s";_tag_fat(_tmp283,sizeof(char),16U);});Cyc_aprintf(_tmp3A2,_tag_fat(_tmp282,sizeof(void*),6));});});struct _fat_ptr cccmd=_tmp27C;
# 1426
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=0,_tmp2C8.f1=(struct _fat_ptr)((struct _fat_ptr)cccmd);_tmp2C8;});void*_tmp27D[1];_tmp27D[0]=& _tmp27F;({struct Cyc___cycFILE*_tmp3A4=Cyc_stderr;struct _fat_ptr _tmp3A3=({const char*_tmp27E="%s\n";_tag_fat(_tmp27E,sizeof(char),4U);});Cyc_fprintf(_tmp3A4,_tmp3A3,_tag_fat(_tmp27D,sizeof(void*),1));});});Cyc_fflush(Cyc_stderr);}
if(system((const char*)_check_null(_untag_fat_ptr(cccmd,sizeof(char),1U)))!= 0){
({void*_tmp280=0U;({struct Cyc___cycFILE*_tmp3A6=Cyc_stderr;struct _fat_ptr _tmp3A5=({const char*_tmp281="Error: C compiler failed\n";_tag_fat(_tmp281,sizeof(char),26U);});Cyc_fprintf(_tmp3A6,_tmp3A5,_tag_fat(_tmp280,sizeof(void*),0));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1433
Cyc_remove_cfiles();
# 1435
return Cyc_compile_failure?1: 0;}}}}}}}}}
