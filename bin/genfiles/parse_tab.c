#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_cyc.c */
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/***********************************************************************/
/* Runtime Stack routines (runtime_stack.c).                           */
/***********************************************************************/

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

/***********************************************************************/
/* Low-level representations etc.                                      */
/***********************************************************************/

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Tagged arrays */
struct _dyneither_ptr {
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

// A dynamic region is just a region handle.  We have the
// wrapper struct for type abstraction reasons.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern void* _throw_null_fn(const char *filename, unsigned lineno);
extern void* _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern void* _throw_badalloc_fn(const char *filename, unsigned lineno);
extern void* _throw_match_fn(const char *filename, unsigned lineno);
extern void* _throw_fn(void* e, const char *filename, unsigned lineno);
extern void* _rethrow(void* e);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

extern struct _xtunion_struct *_exn_thrown;

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
#define _check_known_subscript_null(ptr,bound,elt_sz,index)\
   ((char *)ptr) + (elt_sz)*(index))
#define _check_known_subscript_notnull(bound,index) (index)
#define _check_known_subscript_notnullX(bound,index)\
   ((char *)ptr) + (elt_sz)*(index))

#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  (_cks_ptr) + _cks_elt_sz*_cks_index; })
#define _check_known_subscript_notnull(ptr,bound,elt_sz,index) ({ \
  char*_cks_ptr = (char*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  (_cks_ptr) + _cks_elt_sz*_cks_index; })

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
char * _zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
short * _zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
int * _zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno);
float * _zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
double * _zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno);
long double * _zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno);
void * _zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno);
#endif

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

/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
int _get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset);
int _get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset);

/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus_<type>_fn. */
char * _zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno);
short * _zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno);
int * _zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno);
float * _zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno);
double * _zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno);
long double * _zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno);
void * _zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno);
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

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
char * _zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno);
short * _zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno);
int * _zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno);
float * _zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno);
double * _zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno);
long double * _zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno);
void ** _zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno);
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

/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif

#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  /* JGM: if we're tagging NULL, ignore num_elts */ \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base ? (_tag_arr_ans.base + (elt_sz) * (num_elts)) : 0; \
  _tag_arr_ans; })

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
#endif

#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})

#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })

#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })

#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })

/* This is not a macro since initialization order matters.  Defined in
   runtime_zeroterm.c. */
extern struct _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
  unsigned int sz,
  unsigned int numelts);

/* Allocation */
extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);
/* bound the allocation size to be less than MAX_ALLOC_SIZE,
   which is defined in runtime_memory.c
*/
extern void* _bounded_GC_malloc(int,const char *file,int lineno);
extern void* _bounded_GC_malloc_atomic(int,const char *file,int lineno);
extern void* _bounded_GC_calloc(unsigned n, unsigned s,
                                const char *file,int lineno);
extern void* _bounded_GC_calloc_atomic(unsigned n, unsigned s,
                                       const char *file,
                                       int lineno);
/* FIX?  Not sure if we want to pass filename and lineno in here... */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n) _bounded_GC_malloc(n,__FILE__,__LINE__)
#define _cycalloc_atomic(n) _bounded_GC_malloc_atomic(n,__FILE__,__LINE__)
#define _cyccalloc(n,s) _bounded_GC_calloc(n,s,__FILE__,__LINE__)
#define _cyccalloc_atomic(n,s) _bounded_GC_calloc_atomic(n,s,__FILE__,__LINE__)
#endif

#define MAX_MALLOC_SIZE (1 << 28)
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

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,const char *file,const char *func,
                                int lineno);
extern void* _profile_GC_malloc_atomic(int,const char *file,
                                       const char *func,int lineno);
extern void* _profile_GC_calloc(unsigned n, unsigned s,
                                const char *file, const char *func, int lineno);
extern void* _profile_GC_calloc_atomic(unsigned n, unsigned s,
                                       const char *file, const char *func,
                                       int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern void* _profile_region_calloc(struct _RegionHandle *, unsigned,
                                    unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						const char *file,
						const char *func,
                                                int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 const char *file,
                                 const char *func,
                                 int lineno);
#  if !defined(RUNTIME_CYC)
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 80 "lexing.h"
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 153
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 52
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
# 54
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 108 "string.h"
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Position_Error;
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 28
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 30
void Cyc_Warn_flush_warnings();
# 32
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 39
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 159
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 180
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 185
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 213
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 215
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 315
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 463
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 470
extern struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct Cyc_Absyn_Shared_att_val;
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;
extern struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct Cyc_Absyn_Weak_att_val;
extern struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct Cyc_Absyn_Dllimport_att_val;
extern struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct Cyc_Absyn_Dllexport_att_val;
extern struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct Cyc_Absyn_No_instrument_function_att_val;
extern struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct Cyc_Absyn_Constructor_att_val;
extern struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct Cyc_Absyn_Destructor_att_val;
extern struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct Cyc_Absyn_No_check_memory_usage_att_val;
extern struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct Cyc_Absyn_Pure_att_val;
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 707 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 866
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 895
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 899
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 903
void*Cyc_Absyn_compress_kb(void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 916
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 919
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 921
extern void*Cyc_Absyn_sint_type;
# 923
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 957
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 961
void*Cyc_Absyn_bounds_one();
# 963
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 987
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 990
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1068
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1073
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1092
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1096
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1101
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1103
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1110
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1119
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1124
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1130
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1155
int Cyc_Absyn_fntype_att(void*);
# 1173
extern int Cyc_Absyn_porting_c_code;struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 49 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*t);
# 99
void*Cyc_Tcutil_copy_type(void*t);
# 112
void*Cyc_Tcutil_compress(void*t);
# 139
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 148
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 153
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 158
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 160
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 162
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 167
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 177
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 315 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 355
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 365
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 66 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_cnst2string(union Cyc_Absyn_Cnst);
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 69 "parse.y"
void Cyc_Lex_register_typedef(struct _tuple0*s);
void Cyc_Lex_enter_namespace(struct _dyneither_ptr*);
void Cyc_Lex_leave_namespace();
void Cyc_Lex_enter_using(struct _tuple0*);
void Cyc_Lex_leave_using();
void Cyc_Lex_enter_extern_c();
void Cyc_Lex_leave_extern_c();
extern struct _tuple0*Cyc_Lex_token_qvar;
extern struct _dyneither_ptr Cyc_Lex_token_string;
# 94 "parse.y"
int Cyc_Parse_parsing_tempest=0;struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};
# 100
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 103
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 112
return first;}}
# 117
int Cyc_Parse_no_register=0;char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 134
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0U,Cyc_Parse_Extern_sc  = 1U,Cyc_Parse_ExternC_sc  = 2U,Cyc_Parse_Static_sc  = 3U,Cyc_Parse_Auto_sc  = 4U,Cyc_Parse_Register_sc  = 5U,Cyc_Parse_Abstract_sc  = 6U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned int varloc;struct Cyc_List_List*tms;};struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 170
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 174
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp5;struct Cyc_Absyn_Exp*_tmp4;_LL1: _tmp5=_tmp1.f1;_tmp4=_tmp1.f2;_LL2:;
declarators=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators,_tmp2->hd=_tmp5;_tmp2;});
exprs=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4,_tmp3->tl=exprs;_tmp3;});}
# 181
({struct Cyc_List_List*_tmp735=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp735;});
({struct _tuple10*_tmp736=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp736;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 191
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 193
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 198
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 200
static void*Cyc_Parse_parse_abort(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 202
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=Cyc_Parse_Exit;_tmp6;}));}
# 206
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 208
struct _tuple8*_tmp7=tqt;struct Cyc_Absyn_Tqual _tmpB;void*_tmpA;_LL1: _tmpB=_tmp7->f2;_tmpA=_tmp7->f3;_LL2:;
if((_tmpB.print_const  || _tmpB.q_volatile) || _tmpB.q_restrict){
if(_tmpB.loc != (unsigned int)0)loc=_tmpB.loc;
({void*_tmp8=0U;({unsigned int _tmp738=loc;struct _dyneither_ptr _tmp737=({const char*_tmp9="qualifier on type is ignored";_tag_dyneither(_tmp9,sizeof(char),29U);});Cyc_Warn_warn(_tmp738,_tmp737,_tag_dyneither(_tmp8,sizeof(void*),0U));});});}
# 213
return _tmpA;}struct _tuple14{void*f1;void*f2;void*f3;void*f4;};
# 216
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 222
void*zeroterm=Cyc_Tcutil_any_bool(0);
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;void*_tmpD=_tmpC;void*_tmpF;struct Cyc_Absyn_Exp*_tmpE;switch(*((int*)_tmpD)){case 4U: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5U: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7U: _LL5: _LL6:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 6U: _LL7: _LL8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3U: _LL9: _LLA:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2U: _LLB: _LLC:
 bound=Cyc_Absyn_bounds_one();goto _LL0;case 0U: _LLD: _tmpE=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LLE:
 bound=Cyc_Absyn_thin_bounds_exp(_tmpE);goto _LL0;default: _LLF: _tmpF=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10:
 rgn=_tmpF;goto _LL0;}_LL0:;}
# 234
return({struct _tuple14 _tmp69D;_tmp69D.f1=nullable,_tmp69D.f2=bound,_tmp69D.f3=zeroterm,_tmp69D.f4=rgn;_tmp69D;});}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp73C=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp73C;}),({
struct _dyneither_ptr*_tmp73B=({struct _dyneither_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _dyneither_ptr _tmp73A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp69E;_tmp69E.tag=1U,_tmp69E.f1=(unsigned long)enum_counter ++;_tmp69E;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _dyneither_ptr _tmp739=({const char*_tmp11="__anonymous_enum_%d__";_tag_dyneither(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp739,_tag_dyneither(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp73A;});_tmp13;});_tmp14->f2=_tmp73B;});_tmp14;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple17*_tmp15=field_info;unsigned int _tmp22;struct _tuple0*_tmp21;struct Cyc_Absyn_Tqual _tmp20;void*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;_LL1: _tmp22=(_tmp15->f1)->f1;_tmp21=(_tmp15->f1)->f2;_tmp20=(_tmp15->f1)->f3;_tmp1F=(_tmp15->f1)->f4;_tmp1E=(_tmp15->f1)->f5;_tmp1D=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1B=(_tmp15->f2)->f2;_LL2:;
if(_tmp1E != 0)
({void*_tmp16=0U;({unsigned int _tmp73E=loc;struct _dyneither_ptr _tmp73D=({const char*_tmp17="bad type params in struct field";_tag_dyneither(_tmp17,sizeof(char),32U);});Cyc_Warn_err(_tmp73E,_tmp73D,_tag_dyneither(_tmp16,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp21))
({void*_tmp18=0U;({unsigned int _tmp740=loc;struct _dyneither_ptr _tmp73F=({const char*_tmp19="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp19,sizeof(char),59U);});Cyc_Warn_err(_tmp740,_tmp73F,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->name=(*_tmp21).f2,_tmp1A->tq=_tmp20,_tmp1A->type=_tmp1F,_tmp1A->width=_tmp1C,_tmp1A->attributes=_tmp1D,_tmp1A->requires_clause=_tmp1B;_tmp1A;});}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp69F;_tmp69F.Signed_spec=0,_tmp69F.Unsigned_spec=0,_tmp69F.Short_spec=0,_tmp69F.Long_spec=0,_tmp69F.Long_Long_spec=0,_tmp69F.Valid_type_spec=0,_tmp69F.Type_spec=Cyc_Absyn_sint_type,_tmp69F.loc=loc;_tmp69F;});}
# 273
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);
_tmp23.Type_spec=t;
_tmp23.Valid_type_spec=1;
return _tmp23;}
# 279
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);
_tmp24.Signed_spec=1;
return _tmp24;}
# 284
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);
_tmp25.Unsigned_spec=1;
return _tmp25;}
# 289
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);
_tmp26.Short_spec=1;
return _tmp26;}
# 294
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);
_tmp27.Long_spec=1;
return _tmp27;}
# 301
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 303
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp741=t;Cyc_Tcutil_promote_array(_tmp741,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 316 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_tmp2A=_tmp29;struct _dyneither_ptr _tmp3E;void**_tmp3D;struct _dyneither_ptr*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp3C=_tmp2A->f1;_tmp3B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp3C != 0){_LL2:
# 321
{void*_tmp2B=_tmp3B;void**_tmp32;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->tag == 1U){_LL8: _tmp32=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2;_LL9: {
# 325
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _dyneither_ptr _tmp743=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp6A0;_tmp6A0.tag=0U,_tmp6A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3C);_tmp6A0;});void*_tmp2E[1U];_tmp2E[0]=& _tmp30;({struct _dyneither_ptr _tmp742=({const char*_tmp2F="`%s";_tag_dyneither(_tmp2F,sizeof(char),4U);});Cyc_aprintf(_tmp742,_tag_dyneither(_tmp2E,sizeof(void*),1U));});});*_tmp31=_tmp743;});_tmp31;});
({void*_tmp745=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->name=nm,_tmp2D->identity=- 1,({void*_tmp744=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,_tmp2C->f1=& Cyc_Tcutil_ik;_tmp2C;});_tmp2D->kind=_tmp744;});_tmp2D;}));*_tmp32=_tmp745;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 330
_tmp28=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));({struct _tuple18*_tmp746=({struct _tuple18*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=_tmp3C,_tmp33->f2=_tmp3B;_tmp33;});_tmp34->hd=_tmp746;}),_tmp34->tl=_tmp28;_tmp34;});goto _LL0;}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp3E=*_tmp2A->f1;_tmp3D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {
# 334
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct _dyneither_ptr _tmp748=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp6A1;_tmp6A1.tag=0U,_tmp6A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E);_tmp6A1;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({struct _dyneither_ptr _tmp747=({const char*_tmp38="`%s";_tag_dyneither(_tmp38,sizeof(char),4U);});Cyc_aprintf(_tmp747,_tag_dyneither(_tmp37,sizeof(void*),1U));});});*_tmp3A=_tmp748;});_tmp3A;});
({void*_tmp74A=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->name=nm,_tmp36->identity=- 1,({void*_tmp749=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=0U,_tmp35->f1=& Cyc_Tcutil_rk;_tmp35;});_tmp36->kind=_tmp749;});_tmp36;}));*_tmp3D=_tmp74A;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 340
return _tmp28;}
# 344
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3F=0;
for(0;x != 0;x=x->tl){
void*_tmp40=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp41=_tmp40;void*_tmp44;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->tl == 0){_LL1: _tmp44=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd;_LL2:
# 349
 _tmp3F=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));({struct _tuple18*_tmp74B=({struct _tuple18*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp42->f2=_tmp44;_tmp42;});_tmp43->hd=_tmp74B;}),_tmp43->tl=_tmp3F;_tmp43;});goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 353
return _tmp3F;}
# 357
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp45=e->r;void*_tmp46=_tmp45;struct _dyneither_ptr*_tmp4D;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp4D=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f2;_LL2:
# 360
{struct Cyc_List_List*_tmp47=tags;for(0;_tmp47 != 0;_tmp47=_tmp47->tl){
struct _tuple18*_tmp48=(struct _tuple18*)_tmp47->hd;struct _tuple18*_tmp49=_tmp48;struct _dyneither_ptr*_tmp4C;void*_tmp4B;_LL6: _tmp4C=_tmp49->f1;_tmp4B=_tmp49->f2;_LL7:;
if(Cyc_strptrcmp(_tmp4C,_tmp4D)== 0)
return({void*_tmp74D=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=39U,({void*_tmp74C=Cyc_Tcutil_copy_type(_tmp4B);_tmp4A->f1=_tmp74C;});_tmp4A;});Cyc_Absyn_new_exp(_tmp74D,e->loc);});}}
# 365
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 368
return e;}
# 373
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4E=t;struct Cyc_Absyn_Exp*_tmp61;void*_tmp60;void*_tmp5F;struct Cyc_Absyn_Tqual _tmp5E;void*_tmp5D;void*_tmp5C;void*_tmp5B;void*_tmp5A;struct Cyc_Absyn_PtrLoc*_tmp59;void*_tmp58;struct Cyc_Absyn_Tqual _tmp57;struct Cyc_Absyn_Exp*_tmp56;void*_tmp55;unsigned int _tmp54;switch(*((int*)_tmp4E)){case 4U: _LL1: _tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).tq;_tmp56=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).num_elts;_tmp55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zero_term;_tmp54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zt_loc;_LL2: {
# 376
struct Cyc_Absyn_Exp*nelts2=_tmp56;
if(_tmp56 != 0)
nelts2=Cyc_Parse_substitute_tags_exp(tags,_tmp56);{
# 380
void*_tmp4F=Cyc_Parse_substitute_tags(tags,_tmp58);
if(_tmp56 != nelts2  || _tmp58 != _tmp4F)
return Cyc_Absyn_array_type(_tmp4F,_tmp57,nelts2,_tmp55,_tmp54);
goto _LL0;};}case 3U: _LL3: _tmp5F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp5E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_tq;_tmp5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).rgn;_tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).nullable;_tmp5B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).bounds;_tmp5A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).zero_term;_tmp59=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).ptrloc;_LL4: {
# 385
void*_tmp50=Cyc_Parse_substitute_tags(tags,_tmp5F);
void*_tmp51=Cyc_Parse_substitute_tags(tags,_tmp5B);
if(_tmp50 != _tmp5F  || _tmp51 != _tmp5B)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6A2;_tmp6A2.elt_type=_tmp50,_tmp6A2.elt_tq=_tmp5E,(_tmp6A2.ptr_atts).rgn=_tmp5D,(_tmp6A2.ptr_atts).nullable=_tmp5C,(_tmp6A2.ptr_atts).bounds=_tmp51,(_tmp6A2.ptr_atts).zero_term=_tmp5A,(_tmp6A2.ptr_atts).ptrloc=_tmp59;_tmp6A2;}));
goto _LL0;}case 0U: if(((struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f1)->tag == 13U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->tl == 0){_LL5: _tmp60=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->hd;_LL6: {
# 391
void*_tmp52=Cyc_Parse_substitute_tags(tags,_tmp60);
if(_tmp60 != _tmp52)return Cyc_Absyn_thin_bounds_type(_tmp52);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9U: _LL7: _tmp61=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E)->f1;_LL8: {
# 395
struct Cyc_Absyn_Exp*_tmp53=Cyc_Parse_substitute_tags_exp(tags,_tmp61);
if(_tmp53 != _tmp61)return Cyc_Absyn_valueof_type(_tmp53);
goto _LL0;}default: _LL9: _LLA:
# 401
 goto _LL0;}_LL0:;}
# 403
return t;}
# 408
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp74E=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp74E;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 415
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 417
return({struct _tuple19*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=(*t).f2,_tmp62->f2=(*t).f3;_tmp62;});}
# 420
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp63=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp63)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 429
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(({struct _dyneither_ptr _tmp74F=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp74F,({const char*_tmp64="`H";_tag_dyneither(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;else{
if(({struct _dyneither_ptr _tmp750=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp750,({const char*_tmp65="`U";_tag_dyneither(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;else{
if(({struct _dyneither_ptr _tmp751=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp751,({const char*_tmp66="`RC";_tag_dyneither(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;else{
# 437
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _dyneither_ptr*_tmp752=({struct _dyneither_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp752;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}}}}
# 444
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp69=t;struct Cyc_Absyn_Tvar*_tmp6C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->tag == 2U){_LL1: _tmp6C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;_LL2:
 return _tmp6C;}else{_LL3: _LL4:
({void*_tmp6A=0U;({unsigned int _tmp754=loc;struct _dyneither_ptr _tmp753=({const char*_tmp6B="expecting a list of type variables, not types";_tag_dyneither(_tmp6B,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp754,_tmp753,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});}_LL0:;}
# 452
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_tmp6E=_tmp6D;void**_tmp72;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->tag == 2U){_LL1: _tmp72=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1)->kind;_LL2: {
# 455
void*_tmp6F=Cyc_Absyn_compress_kb(*_tmp72);void*_tmp70=_tmp6F;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp70)->tag == 1U){_LL6: _LL7:
# 457
 if(!leq)({void*_tmp755=Cyc_Tcutil_kind_to_bound(k);*_tmp72=_tmp755;});else{
({void*_tmp756=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2U,_tmp71->f1=0,_tmp71->f2=k;_tmp71;});*_tmp72=_tmp756;});}
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 462
 return;}_LL0:;}
# 467
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 473
if(tds == 0)return tms;
# 478
if(tms == 0)return 0;{
# 480
void*_tmp73=(void*)tms->hd;void*_tmp74=_tmp73;void*_tmp8F;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->tag == 3U){_LL1: _tmp8F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->f1;_LL2:
# 483
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 486
void*_tmp75=_tmp8F;struct Cyc_List_List*_tmp8D;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp75)->tag == 1U){_LL6: _LL7:
# 488
({void*_tmp76=0U;({unsigned int _tmp758=loc;struct _dyneither_ptr _tmp757=({const char*_tmp77="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp77,sizeof(char),93U);});Cyc_Warn_warn(_tmp758,_tmp757,_tag_dyneither(_tmp76,sizeof(void*),0U));});});
# 490
return tms;}else{_LL8: _tmp8D=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp75)->f1;_LL9:
# 492
 if(({int _tmp759=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);_tmp759 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp78=0U;({unsigned int _tmp75B=loc;struct _dyneither_ptr _tmp75A=({const char*_tmp79="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp79,sizeof(char),73U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp75B,_tmp75A,_tag_dyneither(_tmp78,sizeof(void*),0U));});});{
# 496
struct Cyc_List_List*rev_new_params=0;
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
struct Cyc_List_List*_tmp7A=tds;
for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){
struct Cyc_Absyn_Decl*_tmp7B=(struct Cyc_Absyn_Decl*)_tmp7A->hd;
void*_tmp7C=_tmp7B->r;void*_tmp7D=_tmp7C;struct Cyc_Absyn_Vardecl*_tmp86;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->tag == 0U){_LLB: _tmp86=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LLC:
# 503
 if(Cyc_zstrptrcmp((*_tmp86->name).f2,(struct _dyneither_ptr*)_tmp8D->hd)!= 0)
continue;
if(_tmp86->initializer != 0)
({void*_tmp7E=0U;({unsigned int _tmp75D=_tmp7B->loc;struct _dyneither_ptr _tmp75C=({const char*_tmp7F="initializer found in parameter declaration";_tag_dyneither(_tmp7F,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp75D,_tmp75C,_tag_dyneither(_tmp7E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp86->name))
({void*_tmp80=0U;({unsigned int _tmp75F=_tmp7B->loc;struct _dyneither_ptr _tmp75E=({const char*_tmp81="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp81,sizeof(char),47U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp75F,_tmp75E,_tag_dyneither(_tmp80,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));
({struct _tuple8*_tmp760=({struct _tuple8*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(*_tmp86->name).f2,_tmp82->f2=_tmp86->tq,_tmp82->f3=_tmp86->type;_tmp82;});_tmp83->hd=_tmp760;}),_tmp83->tl=rev_new_params;_tmp83;});
# 512
goto L;}else{_LLD: _LLE:
({void*_tmp84=0U;({unsigned int _tmp762=_tmp7B->loc;struct _dyneither_ptr _tmp761=({const char*_tmp85="nonvariable declaration in parameter type";_tag_dyneither(_tmp85,sizeof(char),42U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp762,_tmp761,_tag_dyneither(_tmp84,sizeof(void*),0U));});});}_LLA:;}
# 516
L: if(_tmp7A == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp6A3;_tmp6A3.tag=0U,_tmp6A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp8D->hd));_tmp6A3;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({unsigned int _tmp764=loc;struct _dyneither_ptr _tmp763=({const char*_tmp88="%s is not given a type";_tag_dyneither(_tmp88,sizeof(char),23U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp764,_tmp763,_tag_dyneither(_tmp87,sizeof(void*),1U));});});}
# 519
return({struct Cyc_List_List*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));
({void*_tmp767=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8B=_region_malloc(yy,sizeof(*_tmp8B));_tmp8B->tag=3U,({void*_tmp766=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8A=_region_malloc(yy,sizeof(*_tmp8A));_tmp8A->tag=1U,({struct Cyc_List_List*_tmp765=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp8A->f1=_tmp765;}),_tmp8A->f2=0,_tmp8A->f3=0,_tmp8A->f4=0,_tmp8A->f5=0,_tmp8A->f6=0,_tmp8A->f7=0;_tmp8A;});_tmp8B->f1=_tmp766;});_tmp8B;});_tmp8C->hd=_tmp767;}),_tmp8C->tl=0;_tmp8C;});};}_LL5:;}
# 526
goto _LL4;}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));_tmp8E->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp768=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8E->tl=_tmp768;});_tmp8E;});}_LL0:;};}
# 534
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 538
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6A4;_tmp6A4.id=d.id,_tmp6A4.varloc=d.varloc,({struct Cyc_List_List*_tmp769=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6A4.tms=_tmp769;});_tmp6A4;});{
# 541
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 547
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 553
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmp90=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmp91=_tmp90;switch(_tmp91){case Cyc_Parse_Extern_sc: _LL1: _LL2:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL3: _LL4:
 sc=0U;goto _LL0;default: _LL5: _LL6:
({void*_tmp92=0U;({unsigned int _tmp76B=loc;struct _dyneither_ptr _tmp76A=({const char*_tmp93="bad storage class on function";_tag_dyneither(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp76B,_tmp76A,_tag_dyneither(_tmp92,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 560
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmp95=Cyc_Parse_apply_tms(tq,_tmp94,atts,d.tms);struct _tuple13 _tmp96=_tmp95;struct Cyc_Absyn_Tqual _tmpA6;void*_tmpA5;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA3;_LL8: _tmpA6=_tmp96.f1;_tmpA5=_tmp96.f2;_tmpA4=_tmp96.f3;_tmpA3=_tmp96.f4;_LL9:;
# 565
if(_tmpA4 != 0)
# 568
({void*_tmp97=0U;({unsigned int _tmp76D=loc;struct _dyneither_ptr _tmp76C=({const char*_tmp98="bad type params, ignoring";_tag_dyneither(_tmp98,sizeof(char),26U);});Cyc_Warn_warn(_tmp76D,_tmp76C,_tag_dyneither(_tmp97,sizeof(void*),0U));});});{
# 570
void*_tmp99=_tmpA5;struct Cyc_Absyn_FnInfo _tmpA2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->tag == 5U){_LLB: _tmpA2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1;_LLC:
# 576
{struct Cyc_List_List*_tmp9A=_tmpA2.args;for(0;_tmp9A != 0;_tmp9A=_tmp9A->tl){
if((*((struct _tuple8*)_tmp9A->hd)).f1 == 0){
({void*_tmp9B=0U;({unsigned int _tmp76F=loc;struct _dyneither_ptr _tmp76E=({const char*_tmp9C="missing argument variable in function prototype";_tag_dyneither(_tmp9C,sizeof(char),48U);});Cyc_Warn_err(_tmp76F,_tmp76E,_tag_dyneither(_tmp9B,sizeof(void*),0U));});});
({struct _dyneither_ptr*_tmp771=({struct _dyneither_ptr*_tmp9E=_cycalloc(sizeof(*_tmp9E));({struct _dyneither_ptr _tmp770=({const char*_tmp9D="?";_tag_dyneither(_tmp9D,sizeof(char),2U);});*_tmp9E=_tmp770;});_tmp9E;});(*((struct _tuple8*)_tmp9A->hd)).f1=_tmp771;});}}}
# 583
({struct Cyc_List_List*_tmp772=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA2.attributes,_tmpA3);_tmpA2.attributes=_tmp772;});
return({struct Cyc_Absyn_Fndecl*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->sc=sc,_tmp9F->is_inline=is_inline,_tmp9F->name=d.id,_tmp9F->body=body,_tmp9F->i=_tmpA2,_tmp9F->cached_type=0,_tmp9F->param_vardecls=0,_tmp9F->fn_vardecl=0;_tmp9F;});}else{_LLD: _LLE:
# 587
({void*_tmpA0=0U;({unsigned int _tmp774=loc;struct _dyneither_ptr _tmp773=({const char*_tmpA1="declarator is not a function prototype";_tag_dyneither(_tmpA1,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp774,_tmp773,_tag_dyneither(_tmpA0,sizeof(void*),0U));});});}_LLA:;};};};}static char _tmpA7[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 591
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpA7,_tmpA7,_tmpA7 + 76U};static char _tmpA8[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 593
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpA8,_tmpA8,_tmpA8 + 87U};static char _tmpA9[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 595
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpA9,_tmpA9,_tmpA9 + 74U};static char _tmpAA[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 597
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpAA,_tmpAA,_tmpAA + 84U};
# 604
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 607
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpAB=0U;({unsigned int _tmp776=loc;struct _dyneither_ptr _tmp775=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp776,_tmp775,_tag_dyneither(_tmpAB,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpAC=0U;({unsigned int _tmp778=loc;struct _dyneither_ptr _tmp777=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp778,_tmp777,_tag_dyneither(_tmpAC,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpAD=0U;({unsigned int _tmp77A=loc;struct _dyneither_ptr _tmp779=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp77A,_tmp779,_tag_dyneither(_tmpAD,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpAE=0U;({unsigned int _tmp77C=loc;struct _dyneither_ptr _tmp77B=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp77C,_tmp77B,_tag_dyneither(_tmpAE,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpAF=0U;({unsigned int _tmp77E=loc;struct _dyneither_ptr _tmp77D=Cyc_Parse_msg1;Cyc_Warn_err(_tmp77E,_tmp77D,_tag_dyneither(_tmpAF,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 629
return s1;}
# 635
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 638
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 645
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpB0=0U;({unsigned int _tmp780=loc;struct _dyneither_ptr _tmp77F=Cyc_Parse_msg4;Cyc_Warn_err(_tmp780,_tmp77F,_tag_dyneither(_tmpB0,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpB1=0U;({unsigned int _tmp782=loc;struct _dyneither_ptr _tmp781=Cyc_Parse_msg4;Cyc_Warn_err(_tmp782,_tmp781,_tag_dyneither(_tmpB1,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 657
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpB2=0U;({unsigned int _tmp784=loc;struct _dyneither_ptr _tmp783=({const char*_tmpB3="missing type within specifier";_tag_dyneither(_tmpB3,sizeof(char),30U);});Cyc_Warn_warn(_tmp784,_tmp783,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});
t=Cyc_Absyn_int_type(sgn,sz);}else{
# 662
if(seen_sign){
void*_tmpB4=t;enum Cyc_Absyn_Sign _tmpB8;enum Cyc_Absyn_Size_of _tmpB7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->tag == 1U){_LL1: _tmpB8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f1;_tmpB7=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f2;_LL2:
# 665
 if((int)_tmpB8 != (int)sgn)
t=Cyc_Absyn_int_type(sgn,_tmpB7);
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpB5=0U;({unsigned int _tmp786=loc;struct _dyneither_ptr _tmp785=({const char*_tmpB6="sign specification on non-integral type";_tag_dyneither(_tmpB6,sizeof(char),40U);});Cyc_Warn_err(_tmp786,_tmp785,_tag_dyneither(_tmpB5,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 670
if(seen_size){
void*_tmpB9=t;enum Cyc_Absyn_Sign _tmpBD;enum Cyc_Absyn_Size_of _tmpBC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)){case 1U: _LL6: _tmpBD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f1;_tmpBC=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB9)->f1)->f2;_LL7:
# 673
 if((int)_tmpBC != (int)sz)
t=Cyc_Absyn_int_type(_tmpBD,sz);
goto _LL5;case 2U: _LL8: _LL9:
# 677
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpBA=0U;({unsigned int _tmp788=loc;struct _dyneither_ptr _tmp787=({const char*_tmpBB="size qualifier on non-integral type";_tag_dyneither(_tmpBB,sizeof(char),36U);});Cyc_Warn_err(_tmp788,_tmp787,_tag_dyneither(_tmpBA,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 681
return t;}
# 684
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 688
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpBE=d.id;
unsigned int _tmpBF=d.varloc;
struct _tuple13 _tmpC0=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpC1=_tmpC0;struct Cyc_Absyn_Tqual _tmpC9;void*_tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*_tmpC6;_LL1: _tmpC9=_tmpC1.f1;_tmpC8=_tmpC1.f2;_tmpC7=_tmpC1.f3;_tmpC6=_tmpC1.f4;_LL2:;
# 695
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpC3=_region_malloc(r,sizeof(*_tmpC3));({struct _tuple15*_tmp789=({struct _tuple15*_tmpC2=_region_malloc(r,sizeof(*_tmpC2));_tmpC2->f1=_tmpBF,_tmpC2->f2=_tmpBE,_tmpC2->f3=_tmpC9,_tmpC2->f4=_tmpC8,_tmpC2->f5=_tmpC7,_tmpC2->f6=_tmpC6;_tmpC2;});_tmpC3->hd=_tmp789;}),_tmpC3->tl=0;_tmpC3;});else{
# 698
return({struct Cyc_List_List*_tmpC5=_region_malloc(r,sizeof(*_tmpC5));({struct _tuple15*_tmp78F=({struct _tuple15*_tmpC4=_region_malloc(r,sizeof(*_tmpC4));_tmpC4->f1=_tmpBF,_tmpC4->f2=_tmpBE,_tmpC4->f3=_tmpC9,_tmpC4->f4=_tmpC8,_tmpC4->f5=_tmpC7,_tmpC4->f6=_tmpC6;_tmpC4;});_tmpC5->hd=_tmp78F;}),({
struct Cyc_List_List*_tmp78E=({struct _RegionHandle*_tmp78D=r;struct Cyc_Absyn_Tqual _tmp78C=tq;void*_tmp78B=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp78A=ds->tl;Cyc_Parse_apply_tmss(_tmp78D,_tmp78C,_tmp78B,_tmp78A,shared_atts);});_tmpC5->tl=_tmp78E;});_tmpC5;});}};}
# 702
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 705
if(tms == 0)return({struct _tuple13 _tmp6A5;_tmp6A5.f1=tq,_tmp6A5.f2=t,_tmp6A5.f3=0,_tmp6A5.f4=atts;_tmp6A5;});{
void*_tmpCA=(void*)tms->hd;void*_tmpCB=_tmpCA;unsigned int _tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_PtrAtts _tmpEE;struct Cyc_Absyn_Tqual _tmpED;struct Cyc_List_List*_tmpEC;unsigned int _tmpEB;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpE8;unsigned int _tmpE7;void*_tmpE6;unsigned int _tmpE5;switch(*((int*)_tmpCB)){case 0U: _LL1: _tmpE6=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpE5=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL2:
# 708
 return({struct Cyc_Absyn_Tqual _tmp792=Cyc_Absyn_empty_tqual(0U);void*_tmp791=
Cyc_Absyn_array_type(t,tq,0,_tmpE6,_tmpE5);
# 708
struct Cyc_List_List*_tmp790=atts;Cyc_Parse_apply_tms(_tmp792,_tmp791,_tmp790,tms->tl);});case 1U: _LL3: _tmpE9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpE8=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_tmpE7=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpCB)->f3;_LL4:
# 711
 return({struct Cyc_Absyn_Tqual _tmp795=Cyc_Absyn_empty_tqual(0U);void*_tmp794=
Cyc_Absyn_array_type(t,tq,_tmpE9,_tmpE8,_tmpE7);
# 711
struct Cyc_List_List*_tmp793=atts;Cyc_Parse_apply_tms(_tmp795,_tmp794,_tmp793,tms->tl);});case 3U: _LL5: _tmpEA=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_LL6: {
# 714
void*_tmpCC=_tmpEA;unsigned int _tmpE2;struct Cyc_List_List*_tmpE1;int _tmpE0;struct Cyc_Absyn_VarargInfo*_tmpDF;void*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->tag == 1U){_LLE: _tmpE1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f1;_tmpE0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f2;_tmpDF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f3;_tmpDE=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f4;_tmpDD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f5;_tmpDC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f6;_tmpDB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpCC)->f7;_LLF: {
# 716
struct Cyc_List_List*typvars=0;
# 718
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->hd=(void*)as->hd,_tmpCD->tl=fn_atts;_tmpCD;});else{
# 723
new_atts=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=(void*)as->hd,_tmpCE->tl=new_atts;_tmpCE;});}}}
# 726
if(tms->tl != 0){
void*_tmpCF=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmpD0=_tmpCF;struct Cyc_List_List*_tmpD1;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->tag == 4U){_LL13: _tmpD1=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpD0)->f1;_LL14:
# 729
 typvars=_tmpD1;
tms=tms->tl;
goto _LL12;}else{_LL15: _LL16:
# 733
 goto _LL12;}_LL12:;}
# 737
if(((((!_tmpE0  && _tmpDF == 0) && _tmpE1 != 0) && _tmpE1->tl == 0) && (*((struct _tuple8*)_tmpE1->hd)).f1 == 0) && (*((struct _tuple8*)_tmpE1->hd)).f3 == Cyc_Absyn_void_type)
# 742
_tmpE1=0;{
# 745
struct Cyc_List_List*_tmpD2=Cyc_Parse_get_arg_tags(_tmpE1);
# 747
if(_tmpD2 != 0)
t=Cyc_Parse_substitute_tags(_tmpD2,t);
t=Cyc_Parse_array2ptr(t,0);
# 752
{struct Cyc_List_List*_tmpD3=_tmpE1;for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
struct _tuple8*_tmpD4=(struct _tuple8*)_tmpD3->hd;struct _tuple8*_tmpD5=_tmpD4;struct _dyneither_ptr*_tmpD8;struct Cyc_Absyn_Tqual _tmpD7;void**_tmpD6;_LL18: _tmpD8=_tmpD5->f1;_tmpD7=_tmpD5->f2;_tmpD6=(void**)& _tmpD5->f3;_LL19:;
if(_tmpD2 != 0)
({void*_tmp796=Cyc_Parse_substitute_tags(_tmpD2,*_tmpD6);*_tmpD6=_tmp796;});
({void*_tmp797=Cyc_Parse_array2ptr(*_tmpD6,1);*_tmpD6=_tmp797;});}}
# 764
return({struct Cyc_Absyn_Tqual _tmp79A=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp799=
Cyc_Absyn_function_type(typvars,_tmpDE,tq,t,_tmpE1,_tmpE0,_tmpDF,_tmpDD,fn_atts,_tmpDC,_tmpDB);
# 764
struct Cyc_List_List*_tmp798=new_atts;Cyc_Parse_apply_tms(_tmp79A,_tmp799,_tmp798,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL10: _tmpE2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpCC)->f2;_LL11:
# 771
({void*_tmpD9=0U;({unsigned int _tmp79C=_tmpE2;struct _dyneither_ptr _tmp79B=({const char*_tmpDA="function declaration without parameter types";_tag_dyneither(_tmpDA,sizeof(char),45U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp79C,_tmp79B,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});}_LLD:;}case 4U: _LL7: _tmpEC=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpEB=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LL8:
# 778
 if(tms->tl == 0)
return({struct _tuple13 _tmp6A6;_tmp6A6.f1=tq,_tmp6A6.f2=t,_tmp6A6.f3=_tmpEC,_tmp6A6.f4=atts;_tmp6A6;});
# 783
({void*_tmpE3=0U;({unsigned int _tmp79E=_tmpEB;struct _dyneither_ptr _tmp79D=({const char*_tmpE4="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmpE4,sizeof(char),68U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp79E,_tmp79D,_tag_dyneither(_tmpE3,sizeof(void*),0U));});});case 2U: _LL9: _tmpEE=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpED=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLA:
# 786
 return({struct Cyc_Absyn_Tqual _tmp7A1=_tmpED;void*_tmp7A0=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6A7;_tmp6A7.elt_type=t,_tmp6A7.elt_tq=tq,_tmp6A7.ptr_atts=_tmpEE;_tmp6A7;}));struct Cyc_List_List*_tmp79F=atts;Cyc_Parse_apply_tms(_tmp7A1,_tmp7A0,_tmp79F,tms->tl);});default: _LLB: _tmpF0=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f1;_tmpEF=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpCB)->f2;_LLC:
# 791
 return({struct Cyc_Absyn_Tqual _tmp7A4=tq;void*_tmp7A3=t;struct Cyc_List_List*_tmp7A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmpEF);Cyc_Parse_apply_tms(_tmp7A4,_tmp7A3,_tmp7A2,tms->tl);});}_LL0:;};}
# 797
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 806
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmpF1=t;unsigned int _tmpFB;struct _tuple0*_tmpFA;struct Cyc_Absyn_Tqual _tmpF9;void*_tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_List_List*_tmpF6;_LL1: _tmpFB=_tmpF1->f1;_tmpFA=_tmpF1->f2;_tmpF9=_tmpF1->f3;_tmpF8=_tmpF1->f4;_tmpF7=_tmpF1->f5;_tmpF6=_tmpF1->f6;_LL2:;
# 809
Cyc_Lex_register_typedef(_tmpFA);{
# 811
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmpF2=_tmpF8;struct Cyc_Core_Opt*_tmpF3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF2)->tag == 1U){_LL4: _tmpF3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmpF2)->f1;_LL5:
# 815
 type=0;
if(_tmpF3 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmpF3;}
goto _LL3;}else{_LL6: _LL7:
 kind=0;type=_tmpF8;goto _LL3;}_LL3:;}
# 821
return({void*_tmp7A6=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7A5=({struct Cyc_Absyn_Typedefdecl*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->name=_tmpFA,_tmpF4->tvs=_tmpF7,_tmpF4->kind=kind,_tmpF4->defn=type,_tmpF4->atts=_tmpF6,_tmpF4->tq=_tmpF9,_tmpF4->extern_c=0;_tmpF4;});_tmpF5->f1=_tmp7A5;});_tmpF5;});Cyc_Absyn_new_decl(_tmp7A6,loc);});};}
# 828
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7A7=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=12U,_tmpFC->f1=d,_tmpFC->f2=s;_tmpFC;});Cyc_Absyn_new_stmt(_tmp7A7,d->loc);});}
# 834
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 843
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 847
struct _RegionHandle _tmpFD=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmpFD;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmpFE=ds;struct Cyc_Absyn_Tqual _tmp15F;struct Cyc_Parse_Type_specifier _tmp15E;struct Cyc_List_List*_tmp15D;_LL1: _tmp15F=_tmpFE.tq;_tmp15E=_tmpFE.type_specs;_tmp15D=_tmpFE.attributes;_LL2:;
if(_tmp15F.loc == (unsigned int)0)_tmp15F.loc=tqual_loc;
if(ds.is_inline)
({void*_tmpFF=0U;({unsigned int _tmp7A9=loc;struct _dyneither_ptr _tmp7A8=({const char*_tmp100="inline qualifier on non-function definition";_tag_dyneither(_tmp100,sizeof(char),44U);});Cyc_Warn_warn(_tmp7A9,_tmp7A8,_tag_dyneither(_tmpFF,sizeof(void*),0U));});});{
# 853
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp101=*ds.sc;enum Cyc_Parse_Storage_class _tmp102=_tmp101;switch(_tmp102){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 if(Cyc_Parse_no_register)s=2U;else{s=5U;}goto _LL3;default: _LL10: _LL11:
 s=1U;goto _LL3;}_LL3:;}{
# 869
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 873
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 881
void*_tmp103=Cyc_Parse_collapse_type_specifiers(_tmp15E,loc);
if(declarators == 0){
# 885
void*_tmp104=_tmp103;struct Cyc_List_List*_tmp144;struct _tuple0*_tmp143;struct _tuple0*_tmp142;int _tmp141;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Datatypedecl**_tmp13F;enum Cyc_Absyn_AggrKind _tmp13E;struct _tuple0*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_Absyn_Datatypedecl*_tmp13B;struct Cyc_Absyn_Enumdecl*_tmp13A;struct Cyc_Absyn_Aggrdecl*_tmp139;switch(*((int*)_tmp104)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)){case 0U: _LL13: _tmp139=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL14:
# 887
({struct Cyc_List_List*_tmp7AA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp139->attributes,_tmp15D);_tmp139->attributes=_tmp7AA;});
_tmp139->sc=s;{
struct Cyc_List_List*_tmp107=({struct Cyc_List_List*_tmp106=_cycalloc(sizeof(*_tmp106));({struct Cyc_Absyn_Decl*_tmp7AC=({void*_tmp7AB=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=5U,_tmp105->f1=_tmp139;_tmp105;});Cyc_Absyn_new_decl(_tmp7AB,loc);});_tmp106->hd=_tmp7AC;}),_tmp106->tl=0;_tmp106;});_npop_handler(0U);return _tmp107;};case 1U: _LL15: _tmp13A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL16:
# 891
 if(_tmp15D != 0)({void*_tmp108=0U;({unsigned int _tmp7AE=loc;struct _dyneither_ptr _tmp7AD=({const char*_tmp109="attributes on enum not supported";_tag_dyneither(_tmp109,sizeof(char),33U);});Cyc_Warn_err(_tmp7AE,_tmp7AD,_tag_dyneither(_tmp108,sizeof(void*),0U));});});
_tmp13A->sc=s;{
struct Cyc_List_List*_tmp10C=({struct Cyc_List_List*_tmp10B=_cycalloc(sizeof(*_tmp10B));({struct Cyc_Absyn_Decl*_tmp7B0=({void*_tmp7AF=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->tag=7U,_tmp10A->f1=_tmp13A;_tmp10A;});Cyc_Absyn_new_decl(_tmp7AF,loc);});_tmp10B->hd=_tmp7B0;}),_tmp10B->tl=0;_tmp10B;});_npop_handler(0U);return _tmp10C;};default: _LL17: _tmp13B=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp104)->f1)->r)->f1;_LL18:
# 895
 if(_tmp15D != 0)({void*_tmp10D=0U;({unsigned int _tmp7B2=loc;struct _dyneither_ptr _tmp7B1=({const char*_tmp10E="attributes on datatypes not supported";_tag_dyneither(_tmp10E,sizeof(char),38U);});Cyc_Warn_err(_tmp7B2,_tmp7B1,_tag_dyneither(_tmp10D,sizeof(void*),0U));});});
_tmp13B->sc=s;{
struct Cyc_List_List*_tmp111=({struct Cyc_List_List*_tmp110=_cycalloc(sizeof(*_tmp110));({struct Cyc_Absyn_Decl*_tmp7B4=({void*_tmp7B3=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F->tag=6U,_tmp10F->f1=_tmp13B;_tmp10F;});Cyc_Absyn_new_decl(_tmp7B3,loc);});_tmp110->hd=_tmp7B4;}),_tmp110->tl=0;_tmp110;});_npop_handler(0U);return _tmp111;};}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).tag == 1){_LL19: _tmp13E=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).val).f1;_tmp13D=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownAggr).val).f2;_tmp13C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f2;_LL1A: {
# 899
struct Cyc_List_List*_tmp112=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp13C);
struct Cyc_Absyn_Aggrdecl*_tmp113=({struct Cyc_Absyn_Aggrdecl*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->kind=_tmp13E,_tmp119->sc=s,_tmp119->name=_tmp13D,_tmp119->tvs=_tmp112,_tmp119->impl=0,_tmp119->attributes=0,_tmp119->expected_mem_kind=0;_tmp119;});
if(_tmp15D != 0)({void*_tmp114=0U;({unsigned int _tmp7B6=loc;struct _dyneither_ptr _tmp7B5=({const char*_tmp115="bad attributes on type declaration";_tag_dyneither(_tmp115,sizeof(char),35U);});Cyc_Warn_err(_tmp7B6,_tmp7B5,_tag_dyneither(_tmp114,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp118=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_Decl*_tmp7B8=({void*_tmp7B7=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=5U,_tmp116->f1=_tmp113;_tmp116;});Cyc_Absyn_new_decl(_tmp7B7,loc);});_tmp117->hd=_tmp7B8;}),_tmp117->tl=0;_tmp117;});_npop_handler(0U);return _tmp118;};}}else{goto _LL23;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).KnownDatatype).tag == 2){_LL1B: _tmp13F=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).KnownDatatype).val;_LL1C:
# 904
 if(_tmp15D != 0)({void*_tmp11A=0U;({unsigned int _tmp7BA=loc;struct _dyneither_ptr _tmp7B9=({const char*_tmp11B="bad attributes on datatype";_tag_dyneither(_tmp11B,sizeof(char),27U);});Cyc_Warn_err(_tmp7BA,_tmp7B9,_tag_dyneither(_tmp11A,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp11E=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));({struct Cyc_Absyn_Decl*_tmp7BC=({void*_tmp7BB=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=6U,_tmp11C->f1=*_tmp13F;_tmp11C;});Cyc_Absyn_new_decl(_tmp7BB,loc);});_tmp11D->hd=_tmp7BC;}),_tmp11D->tl=0;_tmp11D;});_npop_handler(0U);return _tmp11E;};}else{_LL1D: _tmp142=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownDatatype).val).name;_tmp141=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp140=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f2;_LL1E: {
# 907
struct Cyc_List_List*_tmp11F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp140);
struct Cyc_Absyn_Decl*_tmp120=Cyc_Absyn_datatype_decl(s,_tmp142,_tmp11F,0,_tmp141,loc);
if(_tmp15D != 0)({void*_tmp121=0U;({unsigned int _tmp7BE=loc;struct _dyneither_ptr _tmp7BD=({const char*_tmp122="bad attributes on datatype";_tag_dyneither(_tmp122,sizeof(char),27U);});Cyc_Warn_err(_tmp7BE,_tmp7BD,_tag_dyneither(_tmp121,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp124=({struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->hd=_tmp120,_tmp123->tl=0;_tmp123;});_npop_handler(0U);return _tmp124;};}}case 15U: _LL1F: _tmp143=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1;_LL20: {
# 912
struct Cyc_Absyn_Enumdecl*_tmp125=({struct Cyc_Absyn_Enumdecl*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->sc=s,_tmp12C->name=_tmp143,_tmp12C->fields=0;_tmp12C;});
if(_tmp15D != 0)({void*_tmp126=0U;({unsigned int _tmp7C0=loc;struct _dyneither_ptr _tmp7BF=({const char*_tmp127="bad attributes on enum";_tag_dyneither(_tmp127,sizeof(char),23U);});Cyc_Warn_err(_tmp7C0,_tmp7BF,_tag_dyneither(_tmp126,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12B=({struct Cyc_List_List*_tmp12A=_cycalloc(sizeof(*_tmp12A));({struct Cyc_Absyn_Decl*_tmp7C2=({struct Cyc_Absyn_Decl*_tmp129=_cycalloc(sizeof(*_tmp129));({void*_tmp7C1=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=7U,_tmp128->f1=_tmp125;_tmp128;});_tmp129->r=_tmp7C1;}),_tmp129->loc=loc;_tmp129;});_tmp12A->hd=_tmp7C2;}),_tmp12A->tl=0;_tmp12A;});_npop_handler(0U);return _tmp12B;};}case 16U: _LL21: _tmp144=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp104)->f1)->f1;_LL22: {
# 918
struct Cyc_Absyn_Enumdecl*_tmp12D=({struct Cyc_Absyn_Enumdecl*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->sc=s,({struct _tuple0*_tmp7C4=Cyc_Parse_gensym_enum();_tmp135->name=_tmp7C4;}),({struct Cyc_Core_Opt*_tmp7C3=({struct Cyc_Core_Opt*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->v=_tmp144;_tmp134;});_tmp135->fields=_tmp7C3;});_tmp135;});
if(_tmp15D != 0)({void*_tmp12E=0U;({unsigned int _tmp7C6=loc;struct _dyneither_ptr _tmp7C5=({const char*_tmp12F="bad attributes on enum";_tag_dyneither(_tmp12F,sizeof(char),23U);});Cyc_Warn_err(_tmp7C6,_tmp7C5,_tag_dyneither(_tmp12E,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp133=({struct Cyc_List_List*_tmp132=_cycalloc(sizeof(*_tmp132));({struct Cyc_Absyn_Decl*_tmp7C8=({struct Cyc_Absyn_Decl*_tmp131=_cycalloc(sizeof(*_tmp131));({void*_tmp7C7=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=7U,_tmp130->f1=_tmp12D;_tmp130;});_tmp131->r=_tmp7C7;}),_tmp131->loc=loc;_tmp131;});_tmp132->hd=_tmp7C8;}),_tmp132->tl=0;_tmp132;});_npop_handler(0U);return _tmp133;};}default: goto _LL23;}default: _LL23: _LL24:
({void*_tmp136=0U;({unsigned int _tmp7CA=loc;struct _dyneither_ptr _tmp7C9=({const char*_tmp137="missing declarator";_tag_dyneither(_tmp137,sizeof(char),19U);});Cyc_Warn_err(_tmp7CA,_tmp7C9,_tag_dyneither(_tmp136,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp138=0;_npop_handler(0U);return _tmp138;};}_LL12:;}else{
# 925
struct Cyc_List_List*_tmp145=Cyc_Parse_apply_tmss(mkrgn,_tmp15F,_tmp103,declarators,_tmp15D);
if(istypedef){
# 930
if(!exps_empty)
({void*_tmp146=0U;({unsigned int _tmp7CC=loc;struct _dyneither_ptr _tmp7CB=({const char*_tmp147="initializer in typedef declaration";_tag_dyneither(_tmp147,sizeof(char),35U);});Cyc_Warn_err(_tmp7CC,_tmp7CB,_tag_dyneither(_tmp146,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp145);
struct Cyc_List_List*_tmp148=decls;_npop_handler(0U);return _tmp148;};}else{
# 936
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp149=_tmp145;for(0;_tmp149 != 0;(_tmp149=_tmp149->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp14A=(struct _tuple15*)_tmp149->hd;struct _tuple15*_tmp14B=_tmp14A;unsigned int _tmp15B;struct _tuple0*_tmp15A;struct Cyc_Absyn_Tqual _tmp159;void*_tmp158;struct Cyc_List_List*_tmp157;struct Cyc_List_List*_tmp156;_LL26: _tmp15B=_tmp14B->f1;_tmp15A=_tmp14B->f2;_tmp159=_tmp14B->f3;_tmp158=_tmp14B->f4;_tmp157=_tmp14B->f5;_tmp156=_tmp14B->f6;_LL27:;
if(_tmp157 != 0)
({void*_tmp14C=0U;({unsigned int _tmp7CE=loc;struct _dyneither_ptr _tmp7CD=({const char*_tmp14D="bad type params, ignoring";_tag_dyneither(_tmp14D,sizeof(char),26U);});Cyc_Warn_warn(_tmp7CE,_tmp7CD,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp14E=0U;({unsigned int _tmp7D0=loc;struct _dyneither_ptr _tmp7CF=({const char*_tmp14F="unexpected NULL in parse!";_tag_dyneither(_tmp14F,sizeof(char),26U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7D0,_tmp7CF,_tag_dyneither(_tmp14E,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp150=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp151=Cyc_Absyn_new_vardecl(_tmp15B,_tmp15A,_tmp158,_tmp150);
_tmp151->tq=_tmp159;
_tmp151->sc=s;
_tmp151->attributes=_tmp156;{
struct Cyc_Absyn_Decl*_tmp152=({struct Cyc_Absyn_Decl*_tmp155=_cycalloc(sizeof(*_tmp155));({void*_tmp7D1=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->tag=0U,_tmp154->f1=_tmp151;_tmp154;});_tmp155->r=_tmp7D1;}),_tmp155->loc=loc;_tmp155;});
decls=({struct Cyc_List_List*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->hd=_tmp152,_tmp153->tl=decls;_tmp153;});};};}}{
# 951
struct Cyc_List_List*_tmp15C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp15C;};}}};};};};}
# 848
;_pop_region(mkrgn);}
# 957
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== (unsigned long)1  || Cyc_strlen((struct _dyneither_ptr)s)== (unsigned long)2){
char _tmp160=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp161=_tmp160;switch(_tmp161){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 967
{char _tmp162=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp163=_tmp162;switch(_tmp163){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 974
goto _LL0;case 84U: _LLF: _LL10:
# 976
{char _tmp164=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp165=_tmp164;switch(_tmp165){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 983
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 986
({struct Cyc_String_pa_PrintArg_struct _tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp6A9;_tmp6A9.tag=0U,_tmp6A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp6A9;});struct Cyc_Int_pa_PrintArg_struct _tmp169=({struct Cyc_Int_pa_PrintArg_struct _tmp6A8;_tmp6A8.tag=1U,({unsigned long _tmp7D2=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp6A8.f1=_tmp7D2;});_tmp6A8;});void*_tmp166[2U];_tmp166[0]=& _tmp168,_tmp166[1]=& _tmp169;({unsigned int _tmp7D4=loc;struct _dyneither_ptr _tmp7D3=({const char*_tmp167="bad kind: %s; strlen=%d";_tag_dyneither(_tmp167,sizeof(char),24U);});Cyc_Warn_err(_tmp7D4,_tmp7D3,_tag_dyneither(_tmp166,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 991
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp16A=e->r;void*_tmp16B=_tmp16A;int _tmp16E;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).tag == 5){_LL1: _tmp16E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp16B)->f1).Int_c).val).f2;_LL2:
 return _tmp16E;}else{goto _LL3;}}else{_LL3: _LL4:
# 995
({void*_tmp16C=0U;({unsigned int _tmp7D6=loc;struct _dyneither_ptr _tmp7D5=({const char*_tmp16D="expecting integer constant";_tag_dyneither(_tmp16D,sizeof(char),27U);});Cyc_Warn_err(_tmp7D6,_tmp7D5,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 1001
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp16F=e->r;void*_tmp170=_tmp16F;struct _dyneither_ptr _tmp173;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).tag == 8){_LL1: _tmp173=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp170)->f1).String_c).val;_LL2:
 return _tmp173;}else{goto _LL3;}}else{_LL3: _LL4:
# 1005
({void*_tmp171=0U;({unsigned int _tmp7D8=loc;struct _dyneither_ptr _tmp7D7=({const char*_tmp172="expecting string constant";_tag_dyneither(_tmp172,sizeof(char),26U);});Cyc_Warn_err(_tmp7D8,_tmp7D7,_tag_dyneither(_tmp171,sizeof(void*),0U));});});
return _tag_dyneither(0,0,0);}_LL0:;}
# 1011
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp174=x;long long _tmp17C;char _tmp17B;int _tmp17A;switch((_tmp174.LongLong_c).tag){case 5U: _LL1: _tmp17A=((_tmp174.Int_c).val).f2;_LL2:
 return(unsigned int)_tmp17A;case 2U: _LL3: _tmp17B=((_tmp174.Char_c).val).f2;_LL4:
 return(unsigned int)_tmp17B;case 6U: _LL5: _tmp17C=((_tmp174.LongLong_c).val).f2;_LL6: {
# 1016
unsigned long long y=(unsigned long long)_tmp17C;
if(y > (unsigned long long)-1)
({void*_tmp175=0U;({unsigned int _tmp7DA=loc;struct _dyneither_ptr _tmp7D9=({const char*_tmp176="integer constant too large";_tag_dyneither(_tmp176,sizeof(char),27U);});Cyc_Warn_err(_tmp7DA,_tmp7D9,_tag_dyneither(_tmp175,sizeof(void*),0U));});});
return(unsigned int)_tmp17C;}default: _LL7: _LL8:
# 1021
({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp6AA;_tmp6AA.tag=0U,({struct _dyneither_ptr _tmp7DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp6AA.f1=_tmp7DB;});_tmp6AA;});void*_tmp177[1U];_tmp177[0]=& _tmp179;({unsigned int _tmp7DD=loc;struct _dyneither_ptr _tmp7DC=({const char*_tmp178="expected integer constant but found %s";_tag_dyneither(_tmp178,sizeof(char),39U);});Cyc_Warn_err(_tmp7DD,_tmp7DC,_tag_dyneither(_tmp177,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1027
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp17D=p->r;void*_tmp17E=_tmp17D;struct Cyc_Absyn_Exp*_tmp18B;struct _tuple0*_tmp18A;struct Cyc_List_List*_tmp189;struct _dyneither_ptr _tmp188;int _tmp187;char _tmp186;enum Cyc_Absyn_Sign _tmp185;int _tmp184;struct Cyc_Absyn_Pat*_tmp183;struct Cyc_Absyn_Vardecl*_tmp182;struct _tuple0*_tmp181;switch(*((int*)_tmp17E)){case 15U: _LL1: _tmp181=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL2:
 return Cyc_Absyn_unknownid_exp(_tmp181,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f2)->r)->tag == 0U){_LL3: _tmp182=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL4:
# 1031
 return({struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Absyn_unknownid_exp(_tmp182->name,p->loc);Cyc_Absyn_deref_exp(_tmp7DE,p->loc);});}else{goto _LL13;}case 6U: _LL5: _tmp183=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL6:
 return({struct Cyc_Absyn_Exp*_tmp7DF=Cyc_Parse_pat2exp(_tmp183);Cyc_Absyn_address_exp(_tmp7DF,p->loc);});case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp185=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp184=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLA:
 return Cyc_Absyn_int_exp(_tmp185,_tmp184,p->loc);case 11U: _LLB: _tmp186=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LLC:
 return Cyc_Absyn_char_exp(_tmp186,p->loc);case 12U: _LLD: _tmp188=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp187=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LLE:
 return Cyc_Absyn_float_exp(_tmp188,_tmp187,p->loc);case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f3 == 0){_LLF: _tmp18A=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_tmp189=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp17E)->f2;_LL10: {
# 1038
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp18A,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp189);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp18B=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp17E)->f1;_LL12:
 return _tmp18B;default: _LL13: _LL14:
# 1043
({void*_tmp17F=0U;({unsigned int _tmp7E1=p->loc;struct _dyneither_ptr _tmp7E0=({const char*_tmp180="cannot mix patterns and expressions in case";_tag_dyneither(_tmp180,sizeof(char),44U);});Cyc_Warn_err(_tmp7E1,_tmp7E0,_tag_dyneither(_tmp17F,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp18C[7U]="cnst_t";
# 1100 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp18C,_tmp18C,_tmp18C + 7U}};
# 1102
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp18D=yy1;union Cyc_Absyn_Cnst _tmp18E;if((_tmp18D.Int_tok).tag == 1){_LL1: _tmp18E=(_tmp18D.Int_tok).val;_LL2:
# 1106
 yyzzz=_tmp18E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL0:;}
# 1110
return yyzzz;}
# 1112
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6AB;(_tmp6AB.Int_tok).tag=1U,(_tmp6AB.Int_tok).val=yy1;_tmp6AB;});}static char _tmp18F[5U]="char";
# 1101 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp18F,_tmp18F,_tmp18F + 5U}};
# 1103
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp190=yy1;char _tmp191;if((_tmp190.Char_tok).tag == 2){_LL1: _tmp191=(_tmp190.Char_tok).val;_LL2:
# 1107
 yyzzz=_tmp191;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL0:;}
# 1111
return yyzzz;}
# 1113
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6AC;(_tmp6AC.Char_tok).tag=2U,(_tmp6AC.Char_tok).val=yy1;_tmp6AC;});}static char _tmp192[13U]="string_t<`H>";
# 1102 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp192,_tmp192,_tmp192 + 13U}};
# 1104
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp193=yy1;struct _dyneither_ptr _tmp194;if((_tmp193.String_tok).tag == 3){_LL1: _tmp194=(_tmp193.String_tok).val;_LL2:
# 1108
 yyzzz=_tmp194;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL0:;}
# 1112
return yyzzz;}
# 1114
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp6AD;(_tmp6AD.String_tok).tag=3U,(_tmp6AD.String_tok).val=yy1;_tmp6AD;});}static char _tmp195[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1105 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp195,_tmp195,_tmp195 + 45U}};
# 1107
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp196=yy1;struct _tuple21*_tmp197;if((_tmp196.YY1).tag == 9){_LL1: _tmp197=(_tmp196.YY1).val;_LL2:
# 1111
 yyzzz=_tmp197;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL0:;}
# 1115
return yyzzz;}
# 1117
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp6AE;(_tmp6AE.YY1).tag=9U,(_tmp6AE.YY1).val=yy1;_tmp6AE;});}static char _tmp198[11U]="ptrbound_t";
# 1106 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp198,_tmp198,_tmp198 + 11U}};
# 1108
void*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp199=yy1;void*_tmp19A;if((_tmp199.YY2).tag == 10){_LL1: _tmp19A=(_tmp199.YY2).val;_LL2:
# 1112
 yyzzz=_tmp19A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL0:;}
# 1116
return yyzzz;}
# 1118
union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6AF;(_tmp6AF.YY2).tag=10U,(_tmp6AF.YY2).val=yy1;_tmp6AF;});}static char _tmp19B[28U]="list_t<offsetof_field_t,`H>";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp19B,_tmp19B,_tmp19B + 28U}};
# 1109
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp19C=yy1;struct Cyc_List_List*_tmp19D;if((_tmp19C.YY3).tag == 11){_LL1: _tmp19D=(_tmp19C.YY3).val;_LL2:
# 1113
 yyzzz=_tmp19D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL0:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B0;(_tmp6B0.YY3).tag=11U,(_tmp6B0.YY3).val=yy1;_tmp6B0;});}static char _tmp19E[6U]="exp_t";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp19E,_tmp19E,_tmp19E + 6U}};
# 1110
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp19F=yy1;struct Cyc_Absyn_Exp*_tmp1A0;if((_tmp19F.Exp_tok).tag == 7){_LL1: _tmp1A0=(_tmp19F.Exp_tok).val;_LL2:
# 1114
 yyzzz=_tmp1A0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL0:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6B1;(_tmp6B1.Exp_tok).tag=7U,(_tmp6B1.Exp_tok).val=yy1;_tmp6B1;});}static char _tmp1A1[17U]="list_t<exp_t,`H>";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1A1,_tmp1A1,_tmp1A1 + 17U}};
# 1118
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1A2=yy1;struct Cyc_List_List*_tmp1A3;if((_tmp1A2.YY4).tag == 12){_LL1: _tmp1A3=(_tmp1A2.YY4).val;_LL2:
# 1122
 yyzzz=_tmp1A3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL0:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B2;(_tmp6B2.YY4).tag=12U,(_tmp6B2.YY4).val=yy1;_tmp6B2;});}static char _tmp1A4[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1A4,_tmp1A4,_tmp1A4 + 47U}};
# 1119
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1A5=yy1;struct Cyc_List_List*_tmp1A6;if((_tmp1A5.YY5).tag == 13){_LL1: _tmp1A6=(_tmp1A5.YY5).val;_LL2:
# 1123
 yyzzz=_tmp1A6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL0:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B3;(_tmp6B3.YY5).tag=13U,(_tmp6B3.YY5).val=yy1;_tmp6B3;});}static char _tmp1A7[9U]="primop_t";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1A7,_tmp1A7,_tmp1A7 + 9U}};
# 1120
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1A8=yy1;enum Cyc_Absyn_Primop _tmp1A9;if((_tmp1A8.YY6).tag == 14){_LL1: _tmp1A9=(_tmp1A8.YY6).val;_LL2:
# 1124
 yyzzz=_tmp1A9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL0:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6B4;(_tmp6B4.YY6).tag=14U,(_tmp6B4.YY6).val=yy1;_tmp6B4;});}static char _tmp1AA[19U]="opt_t<primop_t,`H>";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 19U}};
# 1121
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1AB=yy1;struct Cyc_Core_Opt*_tmp1AC;if((_tmp1AB.YY7).tag == 15){_LL1: _tmp1AC=(_tmp1AB.YY7).val;_LL2:
# 1125
 yyzzz=_tmp1AC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL0:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6B5;(_tmp6B5.YY7).tag=15U,(_tmp6B5.YY7).val=yy1;_tmp6B5;});}static char _tmp1AD[7U]="qvar_t";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1AD,_tmp1AD,_tmp1AD + 7U}};
# 1122
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1AE=yy1;struct _tuple0*_tmp1AF;if((_tmp1AE.QualId_tok).tag == 5){_LL1: _tmp1AF=(_tmp1AE.QualId_tok).val;_LL2:
# 1126
 yyzzz=_tmp1AF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL0:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6B6;(_tmp6B6.QualId_tok).tag=5U,(_tmp6B6.QualId_tok).val=yy1;_tmp6B6;});}static char _tmp1B0[7U]="stmt_t";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp1B0,_tmp1B0,_tmp1B0 + 7U}};
# 1125
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp1B1=yy1;struct Cyc_Absyn_Stmt*_tmp1B2;if((_tmp1B1.Stmt_tok).tag == 8){_LL1: _tmp1B2=(_tmp1B1.Stmt_tok).val;_LL2:
# 1129
 yyzzz=_tmp1B2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL0:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6B7;(_tmp6B7.Stmt_tok).tag=8U,(_tmp6B7.Stmt_tok).val=yy1;_tmp6B7;});}static char _tmp1B3[27U]="list_t<switch_clause_t,`H>";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 27U}};
# 1129
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1B4=yy1;struct Cyc_List_List*_tmp1B5;if((_tmp1B4.YY8).tag == 16){_LL1: _tmp1B5=(_tmp1B4.YY8).val;_LL2:
# 1133
 yyzzz=_tmp1B5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL0:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6B8;(_tmp6B8.YY8).tag=16U,(_tmp6B8.YY8).val=yy1;_tmp6B8;});}static char _tmp1B6[6U]="pat_t";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1B6,_tmp1B6,_tmp1B6 + 6U}};
# 1130
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp1B7=yy1;struct Cyc_Absyn_Pat*_tmp1B8;if((_tmp1B7.YY9).tag == 17){_LL1: _tmp1B8=(_tmp1B7.YY9).val;_LL2:
# 1134
 yyzzz=_tmp1B8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL0:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6B9;(_tmp6B9.YY9).tag=17U,(_tmp6B9.YY9).val=yy1;_tmp6B9;});}static char _tmp1B9[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1B9,_tmp1B9,_tmp1B9 + 28U}};
# 1135
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1BA=yy1;struct _tuple22*_tmp1BB;if((_tmp1BA.YY10).tag == 18){_LL1: _tmp1BB=(_tmp1BA.YY10).val;_LL2:
# 1139
 yyzzz=_tmp1BB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL0:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6BA;(_tmp6BA.YY10).tag=18U,(_tmp6BA.YY10).val=yy1;_tmp6BA;});}static char _tmp1BC[17U]="list_t<pat_t,`H>";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1BC,_tmp1BC,_tmp1BC + 17U}};
# 1136
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1BD=yy1;struct Cyc_List_List*_tmp1BE;if((_tmp1BD.YY11).tag == 19){_LL1: _tmp1BE=(_tmp1BD.YY11).val;_LL2:
# 1140
 yyzzz=_tmp1BE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL0:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BB;(_tmp6BB.YY11).tag=19U,(_tmp6BB.YY11).val=yy1;_tmp6BB;});}static char _tmp1BF[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1BF,_tmp1BF,_tmp1BF + 36U}};
# 1137
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp1C0=yy1;struct _tuple23*_tmp1C1;if((_tmp1C0.YY12).tag == 20){_LL1: _tmp1C1=(_tmp1C0.YY12).val;_LL2:
# 1141
 yyzzz=_tmp1C1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL0:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6BC;(_tmp6BC.YY12).tag=20U,(_tmp6BC.YY12).val=yy1;_tmp6BC;});}static char _tmp1C2[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 47U}};
# 1138
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1C3=yy1;struct Cyc_List_List*_tmp1C4;if((_tmp1C3.YY13).tag == 21){_LL1: _tmp1C4=(_tmp1C3.YY13).val;_LL2:
# 1142
 yyzzz=_tmp1C4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL0:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BD;(_tmp6BD.YY13).tag=21U,(_tmp6BD.YY13).val=yy1;_tmp6BD;});}static char _tmp1C5[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 58U}};
# 1139
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1C6=yy1;struct _tuple22*_tmp1C7;if((_tmp1C6.YY14).tag == 22){_LL1: _tmp1C7=(_tmp1C6.YY14).val;_LL2:
# 1143
 yyzzz=_tmp1C7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL0:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6BE;(_tmp6BE.YY14).tag=22U,(_tmp6BE.YY14).val=yy1;_tmp6BE;});}static char _tmp1C8[9U]="fndecl_t";
# 1138 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1C8,_tmp1C8,_tmp1C8 + 9U}};
# 1140
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp1C9=yy1;struct Cyc_Absyn_Fndecl*_tmp1CA;if((_tmp1C9.YY15).tag == 23){_LL1: _tmp1CA=(_tmp1C9.YY15).val;_LL2:
# 1144
 yyzzz=_tmp1CA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL0:;}
# 1148
return yyzzz;}
# 1150
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6BF;(_tmp6BF.YY15).tag=23U,(_tmp6BF.YY15).val=yy1;_tmp6BF;});}static char _tmp1CB[18U]="list_t<decl_t,`H>";
# 1139 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 18U}};
# 1141
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1CC=yy1;struct Cyc_List_List*_tmp1CD;if((_tmp1CC.YY16).tag == 24){_LL1: _tmp1CD=(_tmp1CC.YY16).val;_LL2:
# 1145
 yyzzz=_tmp1CD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL0:;}
# 1149
return yyzzz;}
# 1151
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C0;(_tmp6C0.YY16).tag=24U,(_tmp6C0.YY16).val=yy1;_tmp6C0;});}static char _tmp1CE[12U]="decl_spec_t";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 12U}};
# 1143
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp1CF=yy1;struct Cyc_Parse_Declaration_spec _tmp1D0;if((_tmp1CF.YY17).tag == 25){_LL1: _tmp1D0=(_tmp1CF.YY17).val;_LL2:
# 1147
 yyzzz=_tmp1D0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL0:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6C1;(_tmp6C1.YY17).tag=25U,(_tmp6C1.YY17).val=yy1;_tmp6C1;});}static char _tmp1D1[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1D1,_tmp1D1,_tmp1D1 + 31U}};
# 1144
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp1D2=yy1;struct _tuple11 _tmp1D3;if((_tmp1D2.YY18).tag == 26){_LL1: _tmp1D3=(_tmp1D2.YY18).val;_LL2:
# 1148
 yyzzz=_tmp1D3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL0:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp6C2;(_tmp6C2.YY18).tag=26U,(_tmp6C2.YY18).val=yy1;_tmp6C2;});}static char _tmp1D4[23U]="declarator_list_t<`yy>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,_tmp1D4 + 23U}};
# 1145
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp1D5=yy1;struct _tuple12*_tmp1D6;if((_tmp1D5.YY19).tag == 27){_LL1: _tmp1D6=(_tmp1D5.YY19).val;_LL2:
# 1149
 yyzzz=_tmp1D6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL0:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp6C3;(_tmp6C3.YY19).tag=27U,(_tmp6C3.YY19).val=yy1;_tmp6C3;});}static char _tmp1D7[19U]="storage_class_t@`H";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1D7,_tmp1D7,_tmp1D7 + 19U}};
# 1146
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp1D8=yy1;enum Cyc_Parse_Storage_class*_tmp1D9;if((_tmp1D8.YY20).tag == 28){_LL1: _tmp1D9=(_tmp1D8.YY20).val;_LL2:
# 1150
 yyzzz=_tmp1D9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL0:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6C4;(_tmp6C4.YY20).tag=28U,(_tmp6C4.YY20).val=yy1;_tmp6C4;});}static char _tmp1DA[17U]="type_specifier_t";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 17U}};
# 1147
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp1DB=yy1;struct Cyc_Parse_Type_specifier _tmp1DC;if((_tmp1DB.YY21).tag == 29){_LL1: _tmp1DC=(_tmp1DB.YY21).val;_LL2:
# 1151
 yyzzz=_tmp1DC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL0:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6C5;(_tmp6C5.YY21).tag=29U,(_tmp6C5.YY21).val=yy1;_tmp6C5;});}static char _tmp1DD[12U]="aggr_kind_t";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1DD,_tmp1DD,_tmp1DD + 12U}};
# 1149
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp1DE=yy1;enum Cyc_Absyn_AggrKind _tmp1DF;if((_tmp1DE.YY22).tag == 30){_LL1: _tmp1DF=(_tmp1DE.YY22).val;_LL2:
# 1153
 yyzzz=_tmp1DF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL0:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6C6;(_tmp6C6.YY22).tag=30U,(_tmp6C6.YY22).val=yy1;_tmp6C6;});}static char _tmp1E0[8U]="tqual_t";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1E0,_tmp1E0,_tmp1E0 + 8U}};
# 1150
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp1E1=yy1;struct Cyc_Absyn_Tqual _tmp1E2;if((_tmp1E1.YY23).tag == 31){_LL1: _tmp1E2=(_tmp1E1.YY23).val;_LL2:
# 1154
 yyzzz=_tmp1E2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL0:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6C7;(_tmp6C7.YY23).tag=31U,(_tmp6C7.YY23).val=yy1;_tmp6C7;});}static char _tmp1E3[23U]="list_t<aggrfield_t,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 23U}};
# 1151
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E4=yy1;struct Cyc_List_List*_tmp1E5;if((_tmp1E4.YY24).tag == 32){_LL1: _tmp1E5=(_tmp1E4.YY24).val;_LL2:
# 1155
 yyzzz=_tmp1E5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL0:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C8;(_tmp6C8.YY24).tag=32U,(_tmp6C8.YY24).val=yy1;_tmp6C8;});}static char _tmp1E6[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1E6,_tmp1E6,_tmp1E6 + 34U}};
# 1152
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E7=yy1;struct Cyc_List_List*_tmp1E8;if((_tmp1E7.YY25).tag == 33){_LL1: _tmp1E8=(_tmp1E7.YY25).val;_LL2:
# 1156
 yyzzz=_tmp1E8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL0:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C9;(_tmp6C9.YY25).tag=33U,(_tmp6C9.YY25).val=yy1;_tmp6C9;});}static char _tmp1E9[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1E9,_tmp1E9,_tmp1E9 + 33U}};
# 1153
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EA=yy1;struct Cyc_List_List*_tmp1EB;if((_tmp1EA.YY26).tag == 34){_LL1: _tmp1EB=(_tmp1EA.YY26).val;_LL2:
# 1157
 yyzzz=_tmp1EB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL0:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CA;(_tmp6CA.YY26).tag=34U,(_tmp6CA.YY26).val=yy1;_tmp6CA;});}static char _tmp1EC[18U]="declarator_t<`yy>";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp1EC,_tmp1EC,_tmp1EC + 18U}};
# 1154
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp1ED=yy1;struct Cyc_Parse_Declarator _tmp1EE;if((_tmp1ED.YY27).tag == 35){_LL1: _tmp1EE=(_tmp1ED.YY27).val;_LL2:
# 1158
 yyzzz=_tmp1EE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL0:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6CB;(_tmp6CB.YY27).tag=35U,(_tmp6CB.YY27).val=yy1;_tmp6CB;});}static char _tmp1EF[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 45U}};
# 1155
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp1F0=yy1;struct _tuple24*_tmp1F1;if((_tmp1F0.YY28).tag == 36){_LL1: _tmp1F1=(_tmp1F0.YY28).val;_LL2:
# 1159
 yyzzz=_tmp1F1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL0:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6CC;(_tmp6CC.YY28).tag=36U,(_tmp6CC.YY28).val=yy1;_tmp6CC;});}static char _tmp1F2[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 57U}};
# 1156
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;struct Cyc_List_List*_tmp1F4;if((_tmp1F3.YY29).tag == 37){_LL1: _tmp1F4=(_tmp1F3.YY29).val;_LL2:
# 1160
 yyzzz=_tmp1F4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL0:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CD;(_tmp6CD.YY29).tag=37U,(_tmp6CD.YY29).val=yy1;_tmp6CD;});}static char _tmp1F5[26U]="abstractdeclarator_t<`yy>";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp1F5,_tmp1F5,_tmp1F5 + 26U}};
# 1157
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp1F6=yy1;struct Cyc_Parse_Abstractdeclarator _tmp1F7;if((_tmp1F6.YY30).tag == 38){_LL1: _tmp1F7=(_tmp1F6.YY30).val;_LL2:
# 1161
 yyzzz=_tmp1F7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL0:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6CE;(_tmp6CE.YY30).tag=38U,(_tmp6CE.YY30).val=yy1;_tmp6CE;});}static char _tmp1F8[5U]="bool";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 5U}};
# 1158
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp1F9=yy1;int _tmp1FA;if((_tmp1F9.YY31).tag == 39){_LL1: _tmp1FA=(_tmp1F9.YY31).val;_LL2:
# 1162
 yyzzz=_tmp1FA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL0:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6CF;(_tmp6CF.YY31).tag=39U,(_tmp6CF.YY31).val=yy1;_tmp6CF;});}static char _tmp1FB[8U]="scope_t";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp1FB,_tmp1FB,_tmp1FB + 8U}};
# 1159
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp1FC=yy1;enum Cyc_Absyn_Scope _tmp1FD;if((_tmp1FC.YY32).tag == 40){_LL1: _tmp1FD=(_tmp1FC.YY32).val;_LL2:
# 1163
 yyzzz=_tmp1FD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL0:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6D0;(_tmp6D0.YY32).tag=40U,(_tmp6D0.YY32).val=yy1;_tmp6D0;});}static char _tmp1FE[16U]="datatypefield_t";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp1FE,_tmp1FE,_tmp1FE + 16U}};
# 1160
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp1FF=yy1;struct Cyc_Absyn_Datatypefield*_tmp200;if((_tmp1FF.YY33).tag == 41){_LL1: _tmp200=(_tmp1FF.YY33).val;_LL2:
# 1164
 yyzzz=_tmp200;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL0:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6D1;(_tmp6D1.YY33).tag=41U,(_tmp6D1.YY33).val=yy1;_tmp6D1;});}static char _tmp201[27U]="list_t<datatypefield_t,`H>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 27U}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_List_List*_tmp203;if((_tmp202.YY34).tag == 42){_LL1: _tmp203=(_tmp202.YY34).val;_LL2:
# 1165
 yyzzz=_tmp203;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL0:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D2;(_tmp6D2.YY34).tag=42U,(_tmp6D2.YY34).val=yy1;_tmp6D2;});}static char _tmp204[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp204,_tmp204,_tmp204 + 41U}};
# 1162
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp205=yy1;struct _tuple25 _tmp206;if((_tmp205.YY35).tag == 43){_LL1: _tmp206=(_tmp205.YY35).val;_LL2:
# 1166
 yyzzz=_tmp206;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL0:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp6D3;(_tmp6D3.YY35).tag=43U,(_tmp6D3.YY35).val=yy1;_tmp6D3;});}static char _tmp207[17U]="list_t<var_t,`H>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 17U}};
# 1163
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp208=yy1;struct Cyc_List_List*_tmp209;if((_tmp208.YY36).tag == 44){_LL1: _tmp209=(_tmp208.YY36).val;_LL2:
# 1167
 yyzzz=_tmp209;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL0:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D4;(_tmp6D4.YY36).tag=44U,(_tmp6D4.YY36).val=yy1;_tmp6D4;});}static char _tmp20A[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 31U}};
# 1164
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp20B=yy1;struct _tuple8*_tmp20C;if((_tmp20B.YY37).tag == 45){_LL1: _tmp20C=(_tmp20B.YY37).val;_LL2:
# 1168
 yyzzz=_tmp20C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL0:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp6D5;(_tmp6D5.YY37).tag=45U,(_tmp6D5.YY37).val=yy1;_tmp6D5;});}static char _tmp20D[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 42U}};
# 1165
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp20E=yy1;struct Cyc_List_List*_tmp20F;if((_tmp20E.YY38).tag == 46){_LL1: _tmp20F=(_tmp20E.YY38).val;_LL2:
# 1169
 yyzzz=_tmp20F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL0:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D6;(_tmp6D6.YY38).tag=46U,(_tmp6D6.YY38).val=yy1;_tmp6D6;});}static char _tmp210[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 115U}};
# 1166
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct _tuple26*_tmp212;if((_tmp211.YY39).tag == 47){_LL1: _tmp212=(_tmp211.YY39).val;_LL2:
# 1170
 yyzzz=_tmp212;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL0:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp6D7;(_tmp6D7.YY39).tag=47U,(_tmp6D7.YY39).val=yy1;_tmp6D7;});}static char _tmp213[8U]="types_t";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp213,_tmp213,_tmp213 + 8U}};
# 1167
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp214=yy1;struct Cyc_List_List*_tmp215;if((_tmp214.YY40).tag == 48){_LL1: _tmp215=(_tmp214.YY40).val;_LL2:
# 1171
 yyzzz=_tmp215;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL0:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.YY40).tag=48U,(_tmp6D8.YY40).val=yy1;_tmp6D8;});}static char _tmp216[24U]="list_t<designator_t,`H>";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 24U}};
# 1169
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp217=yy1;struct Cyc_List_List*_tmp218;if((_tmp217.YY41).tag == 49){_LL1: _tmp218=(_tmp217.YY41).val;_LL2:
# 1173
 yyzzz=_tmp218;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL0:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.YY41).tag=49U,(_tmp6D9.YY41).val=yy1;_tmp6D9;});}static char _tmp219[13U]="designator_t";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp219,_tmp219,_tmp219 + 13U}};
# 1170
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp21A=yy1;void*_tmp21B;if((_tmp21A.YY42).tag == 50){_LL1: _tmp21B=(_tmp21A.YY42).val;_LL2:
# 1174
 yyzzz=_tmp21B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL0:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.YY42).tag=50U,(_tmp6DA.YY42).val=yy1;_tmp6DA;});}static char _tmp21C[7U]="kind_t";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp21C,_tmp21C,_tmp21C + 7U}};
# 1171
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp21D=yy1;struct Cyc_Absyn_Kind*_tmp21E;if((_tmp21D.YY43).tag == 51){_LL1: _tmp21E=(_tmp21D.YY43).val;_LL2:
# 1175
 yyzzz=_tmp21E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL0:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.YY43).tag=51U,(_tmp6DB.YY43).val=yy1;_tmp6DB;});}static char _tmp21F[7U]="type_t";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 7U}};
# 1172
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;void*_tmp221;if((_tmp220.YY44).tag == 52){_LL1: _tmp221=(_tmp220.YY44).val;_LL2:
# 1176
 yyzzz=_tmp221;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL0:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.YY44).tag=52U,(_tmp6DC.YY44).val=yy1;_tmp6DC;});}static char _tmp222[23U]="list_t<attribute_t,`H>";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp222,_tmp222,_tmp222 + 23U}};
# 1173
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp223=yy1;struct Cyc_List_List*_tmp224;if((_tmp223.YY45).tag == 53){_LL1: _tmp224=(_tmp223.YY45).val;_LL2:
# 1177
 yyzzz=_tmp224;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL0:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.YY45).tag=53U,(_tmp6DD.YY45).val=yy1;_tmp6DD;});}static char _tmp225[12U]="attribute_t";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp225,_tmp225,_tmp225 + 12U}};
# 1174
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp226=yy1;void*_tmp227;if((_tmp226.YY46).tag == 54){_LL1: _tmp227=(_tmp226.YY46).val;_LL2:
# 1178
 yyzzz=_tmp227;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL0:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY46).tag=54U,(_tmp6DE.YY46).val=yy1;_tmp6DE;});}static char _tmp228[12U]="enumfield_t";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp228,_tmp228,_tmp228 + 12U}};
# 1175
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp229=yy1;struct Cyc_Absyn_Enumfield*_tmp22A;if((_tmp229.YY47).tag == 55){_LL1: _tmp22A=(_tmp229.YY47).val;_LL2:
# 1179
 yyzzz=_tmp22A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL0:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY47).tag=55U,(_tmp6DF.YY47).val=yy1;_tmp6DF;});}static char _tmp22B[23U]="list_t<enumfield_t,`H>";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp22B,_tmp22B,_tmp22B + 23U}};
# 1176
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22C=yy1;struct Cyc_List_List*_tmp22D;if((_tmp22C.YY48).tag == 56){_LL1: _tmp22D=(_tmp22C.YY48).val;_LL2:
# 1180
 yyzzz=_tmp22D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL0:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY48).tag=56U,(_tmp6E0.YY48).val=yy1;_tmp6E0;});}static char _tmp22E[11U]="type_opt_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 11U}};
# 1177
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;void*_tmp230;if((_tmp22F.YY49).tag == 57){_LL1: _tmp230=(_tmp22F.YY49).val;_LL2:
# 1181
 yyzzz=_tmp230;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL0:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY49).tag=57U,(_tmp6E1.YY49).val=yy1;_tmp6E1;});}static char _tmp231[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp231,_tmp231,_tmp231 + 31U}};
# 1178
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp232=yy1;struct Cyc_List_List*_tmp233;if((_tmp232.YY50).tag == 58){_LL1: _tmp233=(_tmp232.YY50).val;_LL2:
# 1182
 yyzzz=_tmp233;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL0:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY50).tag=58U,(_tmp6E2.YY50).val=yy1;_tmp6E2;});}static char _tmp234[11U]="booltype_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 11U}};
# 1179
void*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp235=yy1;void*_tmp236;if((_tmp235.YY51).tag == 59){_LL1: _tmp236=(_tmp235.YY51).val;_LL2:
# 1183
 yyzzz=_tmp236;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL0:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY51).tag=59U,(_tmp6E3.YY51).val=yy1;_tmp6E3;});}static char _tmp237[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 45U}};
# 1180
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp238=yy1;struct Cyc_List_List*_tmp239;if((_tmp238.YY52).tag == 60){_LL1: _tmp239=(_tmp238.YY52).val;_LL2:
# 1184
 yyzzz=_tmp239;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL0:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY52).tag=60U,(_tmp6E4.YY52).val=yy1;_tmp6E4;});}static char _tmp23A[20U]="pointer_qual_t<`yy>";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp23A,_tmp23A,_tmp23A + 20U}};
# 1181
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp23B=yy1;void*_tmp23C;if((_tmp23B.YY53).tag == 61){_LL1: _tmp23C=(_tmp23B.YY53).val;_LL2:
# 1185
 yyzzz=_tmp23C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL0:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY53).tag=61U,(_tmp6E5.YY53).val=yy1;_tmp6E5;});}static char _tmp23D[21U]="pointer_quals_t<`yy>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 21U}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;struct Cyc_List_List*_tmp23F;if((_tmp23E.YY54).tag == 62){_LL1: _tmp23F=(_tmp23E.YY54).val;_LL2:
# 1186
 yyzzz=_tmp23F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL0:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY54).tag=62U,(_tmp6E6.YY54).val=yy1;_tmp6E6;});}static char _tmp240[21U]="$(bool,string_t<`H>)";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp240,_tmp240,_tmp240 + 21U}};
# 1183
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp241=yy1;struct _tuple20 _tmp242;if((_tmp241.Asm_tok).tag == 6){_LL1: _tmp242=(_tmp241.Asm_tok).val;_LL2:
# 1187
 yyzzz=_tmp242;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL0:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.Asm_tok).tag=6U,(_tmp6E7.Asm_tok).val=yy1;_tmp6E7;});}static char _tmp243[10U]="exp_opt_t";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 10U}};
# 1184
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp244=yy1;struct Cyc_Absyn_Exp*_tmp245;if((_tmp244.YY55).tag == 63){_LL1: _tmp245=(_tmp244.YY55).val;_LL2:
# 1188
 yyzzz=_tmp245;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL0:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY55).tag=63U,(_tmp6E8.YY55).val=yy1;_tmp6E8;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1210
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp6E9;_tmp6E9.timestamp=0,_tmp6E9.first_line=0,_tmp6E9.first_column=0,_tmp6E9.last_line=0,_tmp6E9.last_column=0;_tmp6E9;});}
# 1213
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1224 "parse.y"
static short Cyc_yytranslate[377U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,148,2,2,132,146,143,2,129,130,137,140,125,144,134,145,2,2,2,2,2,2,2,2,2,2,133,122,127,126,128,139,138,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,135,2,136,142,131,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,123,141,124,147,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121};static char _tmp246[2U]="$";static char _tmp247[6U]="error";static char _tmp248[12U]="$undefined.";static char _tmp249[5U]="AUTO";static char _tmp24A[9U]="REGISTER";static char _tmp24B[7U]="STATIC";static char _tmp24C[7U]="EXTERN";static char _tmp24D[8U]="TYPEDEF";static char _tmp24E[5U]="VOID";static char _tmp24F[5U]="CHAR";static char _tmp250[6U]="SHORT";static char _tmp251[4U]="INT";static char _tmp252[5U]="LONG";static char _tmp253[6U]="FLOAT";static char _tmp254[7U]="DOUBLE";static char _tmp255[7U]="SIGNED";static char _tmp256[9U]="UNSIGNED";static char _tmp257[6U]="CONST";static char _tmp258[9U]="VOLATILE";static char _tmp259[9U]="RESTRICT";static char _tmp25A[7U]="STRUCT";static char _tmp25B[6U]="UNION";static char _tmp25C[5U]="CASE";static char _tmp25D[8U]="DEFAULT";static char _tmp25E[7U]="INLINE";static char _tmp25F[7U]="SIZEOF";static char _tmp260[9U]="OFFSETOF";static char _tmp261[3U]="IF";static char _tmp262[5U]="ELSE";static char _tmp263[7U]="SWITCH";static char _tmp264[6U]="WHILE";static char _tmp265[3U]="DO";static char _tmp266[4U]="FOR";static char _tmp267[5U]="GOTO";static char _tmp268[9U]="CONTINUE";static char _tmp269[6U]="BREAK";static char _tmp26A[7U]="RETURN";static char _tmp26B[5U]="ENUM";static char _tmp26C[7U]="TYPEOF";static char _tmp26D[16U]="BUILTIN_VA_LIST";static char _tmp26E[10U]="EXTENSION";static char _tmp26F[8U]="NULL_kw";static char _tmp270[4U]="LET";static char _tmp271[6U]="THROW";static char _tmp272[4U]="TRY";static char _tmp273[6U]="CATCH";static char _tmp274[7U]="EXPORT";static char _tmp275[4U]="NEW";static char _tmp276[9U]="ABSTRACT";static char _tmp277[9U]="FALLTHRU";static char _tmp278[6U]="USING";static char _tmp279[10U]="NAMESPACE";static char _tmp27A[9U]="DATATYPE";static char _tmp27B[7U]="MALLOC";static char _tmp27C[8U]="RMALLOC";static char _tmp27D[15U]="RMALLOC_INLINE";static char _tmp27E[7U]="CALLOC";static char _tmp27F[8U]="RCALLOC";static char _tmp280[5U]="SWAP";static char _tmp281[9U]="REGION_T";static char _tmp282[6U]="TAG_T";static char _tmp283[7U]="REGION";static char _tmp284[5U]="RNEW";static char _tmp285[8U]="REGIONS";static char _tmp286[7U]="PORTON";static char _tmp287[8U]="PORTOFF";static char _tmp288[7U]="PRAGMA";static char _tmp289[10U]="TEMPESTON";static char _tmp28A[11U]="TEMPESTOFF";static char _tmp28B[8U]="NUMELTS";static char _tmp28C[8U]="VALUEOF";static char _tmp28D[10U]="VALUEOF_T";static char _tmp28E[9U]="TAGCHECK";static char _tmp28F[13U]="NUMELTS_QUAL";static char _tmp290[10U]="THIN_QUAL";static char _tmp291[9U]="FAT_QUAL";static char _tmp292[13U]="NOTNULL_QUAL";static char _tmp293[14U]="NULLABLE_QUAL";static char _tmp294[14U]="REQUIRES_QUAL";static char _tmp295[13U]="ENSURES_QUAL";static char _tmp296[12U]="REGION_QUAL";static char _tmp297[16U]="NOZEROTERM_QUAL";static char _tmp298[14U]="ZEROTERM_QUAL";static char _tmp299[12U]="TAGGED_QUAL";static char _tmp29A[16U]="EXTENSIBLE_QUAL";static char _tmp29B[7U]="PTR_OP";static char _tmp29C[7U]="INC_OP";static char _tmp29D[7U]="DEC_OP";static char _tmp29E[8U]="LEFT_OP";static char _tmp29F[9U]="RIGHT_OP";static char _tmp2A0[6U]="LE_OP";static char _tmp2A1[6U]="GE_OP";static char _tmp2A2[6U]="EQ_OP";static char _tmp2A3[6U]="NE_OP";static char _tmp2A4[7U]="AND_OP";static char _tmp2A5[6U]="OR_OP";static char _tmp2A6[11U]="MUL_ASSIGN";static char _tmp2A7[11U]="DIV_ASSIGN";static char _tmp2A8[11U]="MOD_ASSIGN";static char _tmp2A9[11U]="ADD_ASSIGN";static char _tmp2AA[11U]="SUB_ASSIGN";static char _tmp2AB[12U]="LEFT_ASSIGN";static char _tmp2AC[13U]="RIGHT_ASSIGN";static char _tmp2AD[11U]="AND_ASSIGN";static char _tmp2AE[11U]="XOR_ASSIGN";static char _tmp2AF[10U]="OR_ASSIGN";static char _tmp2B0[9U]="ELLIPSIS";static char _tmp2B1[11U]="LEFT_RIGHT";static char _tmp2B2[12U]="COLON_COLON";static char _tmp2B3[11U]="IDENTIFIER";static char _tmp2B4[17U]="INTEGER_CONSTANT";static char _tmp2B5[7U]="STRING";static char _tmp2B6[8U]="WSTRING";static char _tmp2B7[19U]="CHARACTER_CONSTANT";static char _tmp2B8[20U]="WCHARACTER_CONSTANT";static char _tmp2B9[18U]="FLOATING_CONSTANT";static char _tmp2BA[9U]="TYPE_VAR";static char _tmp2BB[13U]="TYPEDEF_NAME";static char _tmp2BC[16U]="QUAL_IDENTIFIER";static char _tmp2BD[18U]="QUAL_TYPEDEF_NAME";static char _tmp2BE[10U]="ATTRIBUTE";static char _tmp2BF[4U]="ASM";static char _tmp2C0[4U]="';'";static char _tmp2C1[4U]="'{'";static char _tmp2C2[4U]="'}'";static char _tmp2C3[4U]="','";static char _tmp2C4[4U]="'='";static char _tmp2C5[4U]="'<'";static char _tmp2C6[4U]="'>'";static char _tmp2C7[4U]="'('";static char _tmp2C8[4U]="')'";static char _tmp2C9[4U]="'_'";static char _tmp2CA[4U]="'$'";static char _tmp2CB[4U]="':'";static char _tmp2CC[4U]="'.'";static char _tmp2CD[4U]="'['";static char _tmp2CE[4U]="']'";static char _tmp2CF[4U]="'*'";static char _tmp2D0[4U]="'@'";static char _tmp2D1[4U]="'?'";static char _tmp2D2[4U]="'+'";static char _tmp2D3[4U]="'|'";static char _tmp2D4[4U]="'^'";static char _tmp2D5[4U]="'&'";static char _tmp2D6[4U]="'-'";static char _tmp2D7[4U]="'/'";static char _tmp2D8[4U]="'%'";static char _tmp2D9[4U]="'~'";static char _tmp2DA[4U]="'!'";static char _tmp2DB[5U]="prog";static char _tmp2DC[17U]="translation_unit";static char _tmp2DD[18U]="tempest_on_action";static char _tmp2DE[19U]="tempest_off_action";static char _tmp2DF[16U]="extern_c_action";static char _tmp2E0[13U]="end_extern_c";static char _tmp2E1[12U]="export_list";static char _tmp2E2[19U]="export_list_values";static char _tmp2E3[21U]="external_declaration";static char _tmp2E4[15U]="optional_comma";static char _tmp2E5[20U]="function_definition";static char _tmp2E6[21U]="function_definition2";static char _tmp2E7[13U]="using_action";static char _tmp2E8[15U]="unusing_action";static char _tmp2E9[17U]="namespace_action";static char _tmp2EA[19U]="unnamespace_action";static char _tmp2EB[12U]="declaration";static char _tmp2EC[17U]="declaration_list";static char _tmp2ED[23U]="declaration_specifiers";static char _tmp2EE[24U]="storage_class_specifier";static char _tmp2EF[15U]="attributes_opt";static char _tmp2F0[11U]="attributes";static char _tmp2F1[15U]="attribute_list";static char _tmp2F2[10U]="attribute";static char _tmp2F3[15U]="type_specifier";static char _tmp2F4[25U]="type_specifier_notypedef";static char _tmp2F5[5U]="kind";static char _tmp2F6[15U]="type_qualifier";static char _tmp2F7[15U]="enum_specifier";static char _tmp2F8[11U]="enum_field";static char _tmp2F9[22U]="enum_declaration_list";static char _tmp2FA[26U]="struct_or_union_specifier";static char _tmp2FB[16U]="type_params_opt";static char _tmp2FC[16U]="struct_or_union";static char _tmp2FD[24U]="struct_declaration_list";static char _tmp2FE[25U]="struct_declaration_list0";static char _tmp2FF[21U]="init_declarator_list";static char _tmp300[22U]="init_declarator_list0";static char _tmp301[16U]="init_declarator";static char _tmp302[19U]="struct_declaration";static char _tmp303[25U]="specifier_qualifier_list";static char _tmp304[35U]="notypedef_specifier_qualifier_list";static char _tmp305[23U]="struct_declarator_list";static char _tmp306[24U]="struct_declarator_list0";static char _tmp307[18U]="struct_declarator";static char _tmp308[20U]="requires_clause_opt";static char _tmp309[19U]="ensures_clause_opt";static char _tmp30A[19U]="datatype_specifier";static char _tmp30B[14U]="qual_datatype";static char _tmp30C[19U]="datatypefield_list";static char _tmp30D[20U]="datatypefield_scope";static char _tmp30E[14U]="datatypefield";static char _tmp30F[11U]="declarator";static char _tmp310[23U]="declarator_withtypedef";static char _tmp311[18U]="direct_declarator";static char _tmp312[30U]="direct_declarator_withtypedef";static char _tmp313[8U]="pointer";static char _tmp314[12U]="one_pointer";static char _tmp315[14U]="pointer_quals";static char _tmp316[13U]="pointer_qual";static char _tmp317[23U]="pointer_null_and_bound";static char _tmp318[14U]="pointer_bound";static char _tmp319[18U]="zeroterm_qual_opt";static char _tmp31A[8U]="rgn_opt";static char _tmp31B[11U]="tqual_list";static char _tmp31C[20U]="parameter_type_list";static char _tmp31D[9U]="type_var";static char _tmp31E[16U]="optional_effect";static char _tmp31F[19U]="optional_rgn_order";static char _tmp320[10U]="rgn_order";static char _tmp321[16U]="optional_inject";static char _tmp322[11U]="effect_set";static char _tmp323[14U]="atomic_effect";static char _tmp324[11U]="region_set";static char _tmp325[15U]="parameter_list";static char _tmp326[22U]="parameter_declaration";static char _tmp327[16U]="identifier_list";static char _tmp328[17U]="identifier_list0";static char _tmp329[12U]="initializer";static char _tmp32A[18U]="array_initializer";static char _tmp32B[17U]="initializer_list";static char _tmp32C[12U]="designation";static char _tmp32D[16U]="designator_list";static char _tmp32E[11U]="designator";static char _tmp32F[10U]="type_name";static char _tmp330[14U]="any_type_name";static char _tmp331[15U]="type_name_list";static char _tmp332[20U]="abstract_declarator";static char _tmp333[27U]="direct_abstract_declarator";static char _tmp334[10U]="statement";static char _tmp335[18U]="labeled_statement";static char _tmp336[21U]="expression_statement";static char _tmp337[19U]="compound_statement";static char _tmp338[16U]="block_item_list";static char _tmp339[20U]="selection_statement";static char _tmp33A[15U]="switch_clauses";static char _tmp33B[20U]="iteration_statement";static char _tmp33C[15U]="jump_statement";static char _tmp33D[12U]="exp_pattern";static char _tmp33E[20U]="conditional_pattern";static char _tmp33F[19U]="logical_or_pattern";static char _tmp340[20U]="logical_and_pattern";static char _tmp341[21U]="inclusive_or_pattern";static char _tmp342[21U]="exclusive_or_pattern";static char _tmp343[12U]="and_pattern";static char _tmp344[17U]="equality_pattern";static char _tmp345[19U]="relational_pattern";static char _tmp346[14U]="shift_pattern";static char _tmp347[17U]="additive_pattern";static char _tmp348[23U]="multiplicative_pattern";static char _tmp349[13U]="cast_pattern";static char _tmp34A[14U]="unary_pattern";static char _tmp34B[16U]="postfix_pattern";static char _tmp34C[16U]="primary_pattern";static char _tmp34D[8U]="pattern";static char _tmp34E[19U]="tuple_pattern_list";static char _tmp34F[20U]="tuple_pattern_list0";static char _tmp350[14U]="field_pattern";static char _tmp351[19U]="field_pattern_list";static char _tmp352[20U]="field_pattern_list0";static char _tmp353[11U]="expression";static char _tmp354[22U]="assignment_expression";static char _tmp355[20U]="assignment_operator";static char _tmp356[23U]="conditional_expression";static char _tmp357[20U]="constant_expression";static char _tmp358[22U]="logical_or_expression";static char _tmp359[23U]="logical_and_expression";static char _tmp35A[24U]="inclusive_or_expression";static char _tmp35B[24U]="exclusive_or_expression";static char _tmp35C[15U]="and_expression";static char _tmp35D[20U]="equality_expression";static char _tmp35E[22U]="relational_expression";static char _tmp35F[17U]="shift_expression";static char _tmp360[20U]="additive_expression";static char _tmp361[26U]="multiplicative_expression";static char _tmp362[16U]="cast_expression";static char _tmp363[17U]="unary_expression";static char _tmp364[15U]="unary_operator";static char _tmp365[19U]="postfix_expression";static char _tmp366[17U]="field_expression";static char _tmp367[19U]="primary_expression";static char _tmp368[25U]="argument_expression_list";static char _tmp369[26U]="argument_expression_list0";static char _tmp36A[9U]="constant";static char _tmp36B[20U]="qual_opt_identifier";static char _tmp36C[17U]="qual_opt_typedef";static char _tmp36D[18U]="struct_union_name";static char _tmp36E[11U]="field_name";static char _tmp36F[12U]="right_angle";
# 1581 "parse.y"
static struct _dyneither_ptr Cyc_yytname[298U]={{_tmp246,_tmp246,_tmp246 + 2U},{_tmp247,_tmp247,_tmp247 + 6U},{_tmp248,_tmp248,_tmp248 + 12U},{_tmp249,_tmp249,_tmp249 + 5U},{_tmp24A,_tmp24A,_tmp24A + 9U},{_tmp24B,_tmp24B,_tmp24B + 7U},{_tmp24C,_tmp24C,_tmp24C + 7U},{_tmp24D,_tmp24D,_tmp24D + 8U},{_tmp24E,_tmp24E,_tmp24E + 5U},{_tmp24F,_tmp24F,_tmp24F + 5U},{_tmp250,_tmp250,_tmp250 + 6U},{_tmp251,_tmp251,_tmp251 + 4U},{_tmp252,_tmp252,_tmp252 + 5U},{_tmp253,_tmp253,_tmp253 + 6U},{_tmp254,_tmp254,_tmp254 + 7U},{_tmp255,_tmp255,_tmp255 + 7U},{_tmp256,_tmp256,_tmp256 + 9U},{_tmp257,_tmp257,_tmp257 + 6U},{_tmp258,_tmp258,_tmp258 + 9U},{_tmp259,_tmp259,_tmp259 + 9U},{_tmp25A,_tmp25A,_tmp25A + 7U},{_tmp25B,_tmp25B,_tmp25B + 6U},{_tmp25C,_tmp25C,_tmp25C + 5U},{_tmp25D,_tmp25D,_tmp25D + 8U},{_tmp25E,_tmp25E,_tmp25E + 7U},{_tmp25F,_tmp25F,_tmp25F + 7U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 3U},{_tmp262,_tmp262,_tmp262 + 5U},{_tmp263,_tmp263,_tmp263 + 7U},{_tmp264,_tmp264,_tmp264 + 6U},{_tmp265,_tmp265,_tmp265 + 3U},{_tmp266,_tmp266,_tmp266 + 4U},{_tmp267,_tmp267,_tmp267 + 5U},{_tmp268,_tmp268,_tmp268 + 9U},{_tmp269,_tmp269,_tmp269 + 6U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 5U},{_tmp26C,_tmp26C,_tmp26C + 7U},{_tmp26D,_tmp26D,_tmp26D + 16U},{_tmp26E,_tmp26E,_tmp26E + 10U},{_tmp26F,_tmp26F,_tmp26F + 8U},{_tmp270,_tmp270,_tmp270 + 4U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 4U},{_tmp273,_tmp273,_tmp273 + 6U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 4U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 9U},{_tmp278,_tmp278,_tmp278 + 6U},{_tmp279,_tmp279,_tmp279 + 10U},{_tmp27A,_tmp27A,_tmp27A + 9U},{_tmp27B,_tmp27B,_tmp27B + 7U},{_tmp27C,_tmp27C,_tmp27C + 8U},{_tmp27D,_tmp27D,_tmp27D + 15U},{_tmp27E,_tmp27E,_tmp27E + 7U},{_tmp27F,_tmp27F,_tmp27F + 8U},{_tmp280,_tmp280,_tmp280 + 5U},{_tmp281,_tmp281,_tmp281 + 9U},{_tmp282,_tmp282,_tmp282 + 6U},{_tmp283,_tmp283,_tmp283 + 7U},{_tmp284,_tmp284,_tmp284 + 5U},{_tmp285,_tmp285,_tmp285 + 8U},{_tmp286,_tmp286,_tmp286 + 7U},{_tmp287,_tmp287,_tmp287 + 8U},{_tmp288,_tmp288,_tmp288 + 7U},{_tmp289,_tmp289,_tmp289 + 10U},{_tmp28A,_tmp28A,_tmp28A + 11U},{_tmp28B,_tmp28B,_tmp28B + 8U},{_tmp28C,_tmp28C,_tmp28C + 8U},{_tmp28D,_tmp28D,_tmp28D + 10U},{_tmp28E,_tmp28E,_tmp28E + 9U},{_tmp28F,_tmp28F,_tmp28F + 13U},{_tmp290,_tmp290,_tmp290 + 10U},{_tmp291,_tmp291,_tmp291 + 9U},{_tmp292,_tmp292,_tmp292 + 13U},{_tmp293,_tmp293,_tmp293 + 14U},{_tmp294,_tmp294,_tmp294 + 14U},{_tmp295,_tmp295,_tmp295 + 13U},{_tmp296,_tmp296,_tmp296 + 12U},{_tmp297,_tmp297,_tmp297 + 16U},{_tmp298,_tmp298,_tmp298 + 14U},{_tmp299,_tmp299,_tmp299 + 12U},{_tmp29A,_tmp29A,_tmp29A + 16U},{_tmp29B,_tmp29B,_tmp29B + 7U},{_tmp29C,_tmp29C,_tmp29C + 7U},{_tmp29D,_tmp29D,_tmp29D + 7U},{_tmp29E,_tmp29E,_tmp29E + 8U},{_tmp29F,_tmp29F,_tmp29F + 9U},{_tmp2A0,_tmp2A0,_tmp2A0 + 6U},{_tmp2A1,_tmp2A1,_tmp2A1 + 6U},{_tmp2A2,_tmp2A2,_tmp2A2 + 6U},{_tmp2A3,_tmp2A3,_tmp2A3 + 6U},{_tmp2A4,_tmp2A4,_tmp2A4 + 7U},{_tmp2A5,_tmp2A5,_tmp2A5 + 6U},{_tmp2A6,_tmp2A6,_tmp2A6 + 11U},{_tmp2A7,_tmp2A7,_tmp2A7 + 11U},{_tmp2A8,_tmp2A8,_tmp2A8 + 11U},{_tmp2A9,_tmp2A9,_tmp2A9 + 11U},{_tmp2AA,_tmp2AA,_tmp2AA + 11U},{_tmp2AB,_tmp2AB,_tmp2AB + 12U},{_tmp2AC,_tmp2AC,_tmp2AC + 13U},{_tmp2AD,_tmp2AD,_tmp2AD + 11U},{_tmp2AE,_tmp2AE,_tmp2AE + 11U},{_tmp2AF,_tmp2AF,_tmp2AF + 10U},{_tmp2B0,_tmp2B0,_tmp2B0 + 9U},{_tmp2B1,_tmp2B1,_tmp2B1 + 11U},{_tmp2B2,_tmp2B2,_tmp2B2 + 12U},{_tmp2B3,_tmp2B3,_tmp2B3 + 11U},{_tmp2B4,_tmp2B4,_tmp2B4 + 17U},{_tmp2B5,_tmp2B5,_tmp2B5 + 7U},{_tmp2B6,_tmp2B6,_tmp2B6 + 8U},{_tmp2B7,_tmp2B7,_tmp2B7 + 19U},{_tmp2B8,_tmp2B8,_tmp2B8 + 20U},{_tmp2B9,_tmp2B9,_tmp2B9 + 18U},{_tmp2BA,_tmp2BA,_tmp2BA + 9U},{_tmp2BB,_tmp2BB,_tmp2BB + 13U},{_tmp2BC,_tmp2BC,_tmp2BC + 16U},{_tmp2BD,_tmp2BD,_tmp2BD + 18U},{_tmp2BE,_tmp2BE,_tmp2BE + 10U},{_tmp2BF,_tmp2BF,_tmp2BF + 4U},{_tmp2C0,_tmp2C0,_tmp2C0 + 4U},{_tmp2C1,_tmp2C1,_tmp2C1 + 4U},{_tmp2C2,_tmp2C2,_tmp2C2 + 4U},{_tmp2C3,_tmp2C3,_tmp2C3 + 4U},{_tmp2C4,_tmp2C4,_tmp2C4 + 4U},{_tmp2C5,_tmp2C5,_tmp2C5 + 4U},{_tmp2C6,_tmp2C6,_tmp2C6 + 4U},{_tmp2C7,_tmp2C7,_tmp2C7 + 4U},{_tmp2C8,_tmp2C8,_tmp2C8 + 4U},{_tmp2C9,_tmp2C9,_tmp2C9 + 4U},{_tmp2CA,_tmp2CA,_tmp2CA + 4U},{_tmp2CB,_tmp2CB,_tmp2CB + 4U},{_tmp2CC,_tmp2CC,_tmp2CC + 4U},{_tmp2CD,_tmp2CD,_tmp2CD + 4U},{_tmp2CE,_tmp2CE,_tmp2CE + 4U},{_tmp2CF,_tmp2CF,_tmp2CF + 4U},{_tmp2D0,_tmp2D0,_tmp2D0 + 4U},{_tmp2D1,_tmp2D1,_tmp2D1 + 4U},{_tmp2D2,_tmp2D2,_tmp2D2 + 4U},{_tmp2D3,_tmp2D3,_tmp2D3 + 4U},{_tmp2D4,_tmp2D4,_tmp2D4 + 4U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 5U},{_tmp2DC,_tmp2DC,_tmp2DC + 17U},{_tmp2DD,_tmp2DD,_tmp2DD + 18U},{_tmp2DE,_tmp2DE,_tmp2DE + 19U},{_tmp2DF,_tmp2DF,_tmp2DF + 16U},{_tmp2E0,_tmp2E0,_tmp2E0 + 13U},{_tmp2E1,_tmp2E1,_tmp2E1 + 12U},{_tmp2E2,_tmp2E2,_tmp2E2 + 19U},{_tmp2E3,_tmp2E3,_tmp2E3 + 21U},{_tmp2E4,_tmp2E4,_tmp2E4 + 15U},{_tmp2E5,_tmp2E5,_tmp2E5 + 20U},{_tmp2E6,_tmp2E6,_tmp2E6 + 21U},{_tmp2E7,_tmp2E7,_tmp2E7 + 13U},{_tmp2E8,_tmp2E8,_tmp2E8 + 15U},{_tmp2E9,_tmp2E9,_tmp2E9 + 17U},{_tmp2EA,_tmp2EA,_tmp2EA + 19U},{_tmp2EB,_tmp2EB,_tmp2EB + 12U},{_tmp2EC,_tmp2EC,_tmp2EC + 17U},{_tmp2ED,_tmp2ED,_tmp2ED + 23U},{_tmp2EE,_tmp2EE,_tmp2EE + 24U},{_tmp2EF,_tmp2EF,_tmp2EF + 15U},{_tmp2F0,_tmp2F0,_tmp2F0 + 11U},{_tmp2F1,_tmp2F1,_tmp2F1 + 15U},{_tmp2F2,_tmp2F2,_tmp2F2 + 10U},{_tmp2F3,_tmp2F3,_tmp2F3 + 15U},{_tmp2F4,_tmp2F4,_tmp2F4 + 25U},{_tmp2F5,_tmp2F5,_tmp2F5 + 5U},{_tmp2F6,_tmp2F6,_tmp2F6 + 15U},{_tmp2F7,_tmp2F7,_tmp2F7 + 15U},{_tmp2F8,_tmp2F8,_tmp2F8 + 11U},{_tmp2F9,_tmp2F9,_tmp2F9 + 22U},{_tmp2FA,_tmp2FA,_tmp2FA + 26U},{_tmp2FB,_tmp2FB,_tmp2FB + 16U},{_tmp2FC,_tmp2FC,_tmp2FC + 16U},{_tmp2FD,_tmp2FD,_tmp2FD + 24U},{_tmp2FE,_tmp2FE,_tmp2FE + 25U},{_tmp2FF,_tmp2FF,_tmp2FF + 21U},{_tmp300,_tmp300,_tmp300 + 22U},{_tmp301,_tmp301,_tmp301 + 16U},{_tmp302,_tmp302,_tmp302 + 19U},{_tmp303,_tmp303,_tmp303 + 25U},{_tmp304,_tmp304,_tmp304 + 35U},{_tmp305,_tmp305,_tmp305 + 23U},{_tmp306,_tmp306,_tmp306 + 24U},{_tmp307,_tmp307,_tmp307 + 18U},{_tmp308,_tmp308,_tmp308 + 20U},{_tmp309,_tmp309,_tmp309 + 19U},{_tmp30A,_tmp30A,_tmp30A + 19U},{_tmp30B,_tmp30B,_tmp30B + 14U},{_tmp30C,_tmp30C,_tmp30C + 19U},{_tmp30D,_tmp30D,_tmp30D + 20U},{_tmp30E,_tmp30E,_tmp30E + 14U},{_tmp30F,_tmp30F,_tmp30F + 11U},{_tmp310,_tmp310,_tmp310 + 23U},{_tmp311,_tmp311,_tmp311 + 18U},{_tmp312,_tmp312,_tmp312 + 30U},{_tmp313,_tmp313,_tmp313 + 8U},{_tmp314,_tmp314,_tmp314 + 12U},{_tmp315,_tmp315,_tmp315 + 14U},{_tmp316,_tmp316,_tmp316 + 13U},{_tmp317,_tmp317,_tmp317 + 23U},{_tmp318,_tmp318,_tmp318 + 14U},{_tmp319,_tmp319,_tmp319 + 18U},{_tmp31A,_tmp31A,_tmp31A + 8U},{_tmp31B,_tmp31B,_tmp31B + 11U},{_tmp31C,_tmp31C,_tmp31C + 20U},{_tmp31D,_tmp31D,_tmp31D + 9U},{_tmp31E,_tmp31E,_tmp31E + 16U},{_tmp31F,_tmp31F,_tmp31F + 19U},{_tmp320,_tmp320,_tmp320 + 10U},{_tmp321,_tmp321,_tmp321 + 16U},{_tmp322,_tmp322,_tmp322 + 11U},{_tmp323,_tmp323,_tmp323 + 14U},{_tmp324,_tmp324,_tmp324 + 11U},{_tmp325,_tmp325,_tmp325 + 15U},{_tmp326,_tmp326,_tmp326 + 22U},{_tmp327,_tmp327,_tmp327 + 16U},{_tmp328,_tmp328,_tmp328 + 17U},{_tmp329,_tmp329,_tmp329 + 12U},{_tmp32A,_tmp32A,_tmp32A + 18U},{_tmp32B,_tmp32B,_tmp32B + 17U},{_tmp32C,_tmp32C,_tmp32C + 12U},{_tmp32D,_tmp32D,_tmp32D + 16U},{_tmp32E,_tmp32E,_tmp32E + 11U},{_tmp32F,_tmp32F,_tmp32F + 10U},{_tmp330,_tmp330,_tmp330 + 14U},{_tmp331,_tmp331,_tmp331 + 15U},{_tmp332,_tmp332,_tmp332 + 20U},{_tmp333,_tmp333,_tmp333 + 27U},{_tmp334,_tmp334,_tmp334 + 10U},{_tmp335,_tmp335,_tmp335 + 18U},{_tmp336,_tmp336,_tmp336 + 21U},{_tmp337,_tmp337,_tmp337 + 19U},{_tmp338,_tmp338,_tmp338 + 16U},{_tmp339,_tmp339,_tmp339 + 20U},{_tmp33A,_tmp33A,_tmp33A + 15U},{_tmp33B,_tmp33B,_tmp33B + 20U},{_tmp33C,_tmp33C,_tmp33C + 15U},{_tmp33D,_tmp33D,_tmp33D + 12U},{_tmp33E,_tmp33E,_tmp33E + 20U},{_tmp33F,_tmp33F,_tmp33F + 19U},{_tmp340,_tmp340,_tmp340 + 20U},{_tmp341,_tmp341,_tmp341 + 21U},{_tmp342,_tmp342,_tmp342 + 21U},{_tmp343,_tmp343,_tmp343 + 12U},{_tmp344,_tmp344,_tmp344 + 17U},{_tmp345,_tmp345,_tmp345 + 19U},{_tmp346,_tmp346,_tmp346 + 14U},{_tmp347,_tmp347,_tmp347 + 17U},{_tmp348,_tmp348,_tmp348 + 23U},{_tmp349,_tmp349,_tmp349 + 13U},{_tmp34A,_tmp34A,_tmp34A + 14U},{_tmp34B,_tmp34B,_tmp34B + 16U},{_tmp34C,_tmp34C,_tmp34C + 16U},{_tmp34D,_tmp34D,_tmp34D + 8U},{_tmp34E,_tmp34E,_tmp34E + 19U},{_tmp34F,_tmp34F,_tmp34F + 20U},{_tmp350,_tmp350,_tmp350 + 14U},{_tmp351,_tmp351,_tmp351 + 19U},{_tmp352,_tmp352,_tmp352 + 20U},{_tmp353,_tmp353,_tmp353 + 11U},{_tmp354,_tmp354,_tmp354 + 22U},{_tmp355,_tmp355,_tmp355 + 20U},{_tmp356,_tmp356,_tmp356 + 23U},{_tmp357,_tmp357,_tmp357 + 20U},{_tmp358,_tmp358,_tmp358 + 22U},{_tmp359,_tmp359,_tmp359 + 23U},{_tmp35A,_tmp35A,_tmp35A + 24U},{_tmp35B,_tmp35B,_tmp35B + 24U},{_tmp35C,_tmp35C,_tmp35C + 15U},{_tmp35D,_tmp35D,_tmp35D + 20U},{_tmp35E,_tmp35E,_tmp35E + 22U},{_tmp35F,_tmp35F,_tmp35F + 17U},{_tmp360,_tmp360,_tmp360 + 20U},{_tmp361,_tmp361,_tmp361 + 26U},{_tmp362,_tmp362,_tmp362 + 16U},{_tmp363,_tmp363,_tmp363 + 17U},{_tmp364,_tmp364,_tmp364 + 15U},{_tmp365,_tmp365,_tmp365 + 19U},{_tmp366,_tmp366,_tmp366 + 17U},{_tmp367,_tmp367,_tmp367 + 19U},{_tmp368,_tmp368,_tmp368 + 25U},{_tmp369,_tmp369,_tmp369 + 26U},{_tmp36A,_tmp36A,_tmp36A + 9U},{_tmp36B,_tmp36B,_tmp36B + 20U},{_tmp36C,_tmp36C,_tmp36C + 17U},{_tmp36D,_tmp36D,_tmp36D + 18U},{_tmp36E,_tmp36E,_tmp36E + 11U},{_tmp36F,_tmp36F,_tmp36F + 12U}};
# 1637
static short Cyc_yyr1[533U]={0,149,150,150,150,150,150,150,150,150,150,150,150,150,151,152,153,154,155,155,156,156,156,157,157,157,158,158,159,159,159,159,160,160,161,162,163,164,165,165,165,165,165,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,168,168,168,168,168,168,168,169,169,170,171,171,172,172,172,172,173,173,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,175,176,176,176,177,177,177,178,178,179,179,179,180,180,180,180,180,181,181,182,182,183,183,184,184,185,186,186,187,187,188,189,189,189,189,189,189,190,190,190,190,190,190,191,192,192,193,193,193,193,194,194,195,195,196,196,196,197,197,198,198,198,198,199,199,199,200,200,201,201,202,202,203,203,203,203,203,203,203,203,203,203,204,204,204,204,204,204,204,204,204,204,204,205,205,206,207,207,208,208,208,208,208,208,208,208,209,209,209,210,210,211,211,211,212,212,212,213,213,214,214,214,214,215,215,216,216,217,217,218,218,219,219,220,220,221,221,221,221,222,222,223,223,224,224,224,225,226,226,227,227,228,228,228,228,228,229,229,229,229,230,230,231,231,232,232,233,233,234,234,234,234,234,235,235,236,236,236,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,239,240,240,241,241,242,242,242,242,242,242,242,242,243,243,243,243,243,243,244,244,244,244,244,244,245,245,245,245,245,245,245,245,245,245,245,245,245,245,246,246,246,246,246,246,246,246,247,248,248,249,249,250,250,251,251,252,252,253,253,254,254,254,255,255,255,255,255,256,256,256,257,257,257,258,258,258,258,259,259,260,260,260,260,260,260,261,262,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,264,264,264,265,265,266,266,267,267,267,268,268,269,269,270,270,270,271,271,271,271,271,271,271,271,271,271,271,272,272,272,272,272,272,272,273,274,274,275,275,276,276,277,277,278,278,279,279,279,280,280,280,280,280,281,281,281,282,282,282,283,283,283,283,284,284,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,286,286,286,287,287,287,287,287,287,287,287,287,287,287,288,288,288,288,289,289,289,289,289,289,289,289,289,289,289,290,291,291,292,292,292,292,292,293,293,294,294,295,295,296,296,297,297};
# 1694
static short Cyc_yyr2[533U]={0,1,2,3,5,3,5,5,6,3,3,3,3,0,1,1,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,2,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1751
static short Cyc_yydefact[1087U]={0,25,58,59,60,61,63,76,77,78,79,80,81,82,83,84,100,101,102,118,119,54,0,0,88,0,0,64,0,0,156,95,97,0,0,0,14,15,0,0,0,523,222,525,524,526,0,0,91,0,208,208,207,1,0,0,0,0,23,0,0,24,0,47,56,50,74,52,85,86,0,89,0,0,167,0,192,195,90,171,116,62,61,55,0,104,0,49,522,0,523,518,519,520,521,0,116,0,382,0,0,0,0,245,0,384,385,34,36,0,0,0,0,0,0,0,0,157,0,0,0,0,0,0,0,205,206,0,0,0,2,0,0,0,0,38,0,124,125,127,48,57,51,53,120,527,528,116,116,0,45,0,0,27,0,224,0,180,168,193,0,199,200,203,204,0,202,201,213,195,0,75,62,108,0,106,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,507,508,484,0,0,0,0,0,488,486,487,0,411,413,427,435,437,439,441,443,445,448,453,456,459,463,0,465,489,506,504,523,394,0,0,0,0,0,0,395,393,41,0,0,116,0,0,0,134,130,132,265,267,0,0,43,0,0,9,10,0,116,529,530,223,99,0,0,172,92,243,0,240,0,11,12,0,3,0,5,0,39,0,0,0,27,0,121,122,147,115,0,154,0,0,0,0,0,0,0,0,0,0,0,0,523,295,297,0,305,299,0,303,288,289,290,0,291,292,293,0,46,27,127,26,28,272,0,230,246,0,0,226,224,0,210,0,0,0,215,65,214,196,0,109,105,0,0,0,473,0,0,485,429,463,0,430,431,0,0,0,0,0,0,0,0,0,0,466,467,0,0,0,0,469,470,468,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,417,418,419,420,421,422,423,424,425,426,416,0,471,0,495,496,0,0,0,510,0,116,386,0,0,0,408,523,530,0,0,0,0,261,404,409,0,406,0,383,401,402,0,399,0,247,0,0,0,0,268,0,238,135,140,136,138,131,133,224,0,274,266,275,532,531,0,94,96,0,0,98,114,71,70,0,68,173,224,242,169,274,244,181,182,0,93,209,17,0,35,0,37,0,126,128,249,248,27,29,111,123,0,0,0,142,143,150,0,116,116,162,0,0,0,0,0,523,0,0,0,334,335,336,0,0,338,0,0,0,306,300,127,304,298,296,30,0,179,231,0,0,0,237,225,232,150,0,0,0,226,178,212,211,174,210,0,0,216,66,117,110,434,107,103,0,0,0,0,523,250,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,509,516,0,515,412,436,0,438,440,442,444,446,447,451,452,449,450,454,455,457,458,460,461,462,415,414,494,491,0,493,0,0,0,397,398,0,264,0,405,259,262,392,0,260,389,0,396,40,0,390,0,269,0,141,137,139,0,226,0,210,0,276,0,224,0,287,271,0,0,116,0,0,0,134,0,116,0,224,0,191,170,241,0,7,0,4,6,31,0,146,129,147,0,0,145,226,155,164,163,0,0,158,0,0,0,313,0,0,0,0,0,0,333,337,0,0,0,301,294,0,32,273,224,0,234,0,0,152,227,0,150,230,218,175,197,198,216,194,472,0,0,0,251,0,256,475,0,0,0,0,0,505,480,483,0,0,0,0,464,512,0,0,492,490,0,0,0,0,263,407,410,400,403,391,270,239,150,0,277,278,210,0,0,226,210,0,0,42,226,523,0,67,69,0,183,0,0,226,0,210,0,0,8,144,0,148,120,153,165,162,162,0,0,0,0,0,0,0,0,0,0,0,0,0,313,339,0,302,33,226,0,235,233,0,176,0,152,226,0,217,501,0,500,0,252,257,0,0,0,0,0,432,433,494,493,514,497,0,517,428,511,513,0,387,388,152,150,280,286,150,0,279,210,0,120,0,72,184,190,150,0,189,185,210,19,0,20,0,0,0,161,160,307,313,0,0,0,0,0,0,341,342,344,346,348,350,352,354,357,362,365,368,372,374,380,381,0,0,310,319,0,0,0,0,0,0,0,0,0,0,340,220,236,0,228,177,219,224,474,0,0,258,476,477,0,0,482,481,498,0,283,152,152,150,281,44,0,0,152,150,186,18,21,0,149,113,0,0,0,313,0,378,0,0,375,313,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,376,313,0,321,0,0,0,329,0,0,0,0,0,0,312,0,0,226,503,502,0,0,0,499,282,285,152,112,0,187,152,22,166,308,309,0,0,0,0,313,315,345,0,347,349,351,353,355,356,360,361,358,359,363,364,366,367,369,370,371,0,314,320,322,323,0,331,330,0,325,0,0,0,151,229,221,0,0,0,0,284,0,188,311,377,0,373,316,0,313,324,332,326,327,0,254,253,478,0,73,0,343,313,317,328,0,379,318,479,0,0,0};
# 1863
static short Cyc_yydefgoto[149U]={1084,53,54,55,56,482,663,864,57,319,58,303,59,484,60,486,61,151,62,63,551,242,468,469,243,66,258,244,68,173,174,69,171,70,280,281,136,137,138,282,245,450,497,498,499,673,813,71,72,678,679,680,73,500,74,473,75,76,168,169,77,125,547,334,716,635,78,636,541,707,533,537,538,444,327,266,102,103,564,489,565,424,425,426,246,320,321,637,456,306,307,308,309,310,311,795,312,313,878,879,880,881,882,883,884,885,886,887,888,889,890,891,892,893,427,435,436,428,429,430,314,206,404,207,556,208,209,210,211,212,213,214,215,216,217,218,219,220,221,820,222,581,582,223,224,80,865,431,460};
# 1881
static short Cyc_yypact[1087U]={2925,- -32768,- -32768,- -32768,- -32768,- 58,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3766,306,- 60,- -32768,3766,884,- -32768,68,- 28,- -32768,27,54,- 33,- 12,99,- -32768,- -32768,94,357,203,- -32768,241,- -32768,- -32768,- -32768,170,302,246,271,290,290,- -32768,- -32768,284,294,300,2785,- -32768,451,597,- -32768,365,3766,3766,3766,- -32768,3766,- -32768,- -32768,832,- -32768,68,3679,259,191,296,962,- -32768,- -32768,325,385,380,- -32768,68,391,6731,- -32768,- -32768,3530,249,- -32768,- -32768,- -32768,- -32768,406,325,6731,- -32768,413,426,3530,430,481,486,- -32768,179,- -32768,- -32768,3975,3975,83,512,2785,2785,6731,371,- -32768,219,505,647,508,219,4439,6731,- -32768,- -32768,2785,2785,3064,- -32768,2785,3064,2785,3064,- -32768,526,528,- -32768,3549,- -32768,- -32768,- -32768,- -32768,4439,- -32768,- -32768,325,154,1922,- -32768,3679,365,558,3975,3850,4101,- -32768,259,- -32768,536,- -32768,- -32768,- -32768,- -32768,553,- -32768,- -32768,71,962,3975,- -32768,- -32768,559,613,577,68,7028,582,7126,6731,1168,621,632,643,645,649,651,689,712,718,728,7126,7126,- -32768,- -32768,- -32768,2355,730,7224,7224,7224,- -32768,- -32768,- -32768,111,- -32768,- -32768,- 37,650,720,726,732,676,106,710,314,291,- -32768,589,7224,232,- 56,- -32768,755,- 1,- -32768,3530,101,773,3375,202,1019,762,- -32768,- -32768,793,6731,325,1019,768,4187,4439,4523,4439,658,- -32768,61,61,- -32768,801,772,- -32768,- -32768,218,325,- -32768,- -32768,- -32768,- -32768,29,776,- -32768,- -32768,713,274,- -32768,794,- -32768,- -32768,795,- -32768,800,- -32768,802,- -32768,647,5152,3679,558,803,4439,- -32768,891,798,68,805,811,278,814,4591,817,808,807,809,5261,1485,4591,138,815,- -32768,- -32768,818,2068,2068,365,2068,- -32768,- -32768,- -32768,828,- -32768,- -32768,- -32768,510,- -32768,558,831,- -32768,- -32768,820,32,849,- -32768,72,834,840,633,846,701,825,6731,3975,- -32768,857,- -32768,- -32768,32,68,- -32768,6731,855,2355,- -32768,4439,2355,- -32768,- -32768,- -32768,1583,- -32768,885,6731,6731,6731,6731,6731,6731,872,6731,4439,824,- -32768,- -32768,2068,852,381,6731,- -32768,- -32768,- -32768,6731,- -32768,7224,6731,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,6731,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6731,- -32768,219,- -32768,- -32768,5359,219,6731,- -32768,858,325,- -32768,861,867,868,- -32768,89,406,219,6731,3530,329,- -32768,- -32768,- -32768,862,878,871,- -32768,- -32768,- -32768,876,889,3530,- -32768,635,3375,887,3975,- -32768,895,897,- -32768,4523,4523,4523,- -32768,- -32768,3280,5457,196,- -32768,350,- -32768,- -32768,72,- -32768,- -32768,894,917,- -32768,909,- -32768,911,918,922,- -32768,3148,- -32768,523,431,- -32768,- -32768,- -32768,4439,- -32768,- -32768,- -32768,2645,- -32768,2785,- -32768,2785,- -32768,- -32768,- -32768,- -32768,558,- -32768,- -32768,- -32768,774,6731,929,930,- -32768,- 24,683,325,325,799,6731,6731,925,933,6731,924,1028,1324,937,- -32768,- -32768,- -32768,637,1021,- -32768,5555,2214,2496,- -32768,- -32768,3549,- -32768,- -32768,- -32768,- -32768,3975,- -32768,- -32768,4439,940,4271,- -32768,- -32768,932,996,72,945,4355,840,- -32768,- -32768,- -32768,- -32768,701,947,40,810,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,956,963,960,982,959,- -32768,- -32768,715,5152,964,968,971,974,980,452,978,986,987,283,994,6927,- -32768,- -32768,990,985,- -32768,650,248,720,726,732,676,106,106,710,710,710,710,314,314,291,291,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,992,- -32768,28,3975,5039,4439,- -32768,4439,- -32768,988,- -32768,- -32768,- -32768,- -32768,3814,- -32768,- -32768,3261,- -32768,- -32768,999,- -32768,118,- -32768,4439,- -32768,- -32768,- -32768,997,840,1000,701,1002,350,3975,4062,5653,- -32768,- -32768,6731,1004,325,6829,1011,29,3412,1014,325,3975,3850,5751,- -32768,523,- -32768,1022,- -32768,2785,- -32768,- -32768,- -32768,944,- -32768,- -32768,891,1017,6731,- -32768,840,- -32768,- -32768,- -32768,1023,68,641,477,484,6731,823,524,4591,1020,5849,5947,666,- -32768,- -32768,1031,1033,1029,2068,- -32768,3679,- -32768,820,1036,3975,- -32768,1037,72,1084,- -32768,1055,996,120,- -32768,- -32768,- -32768,- -32768,810,- -32768,1042,175,1042,1057,- -32768,4700,- -32768,- -32768,6731,6731,1160,6731,1168,- -32768,- -32768,- -32768,219,219,1056,4813,- -32768,- -32768,6731,6731,- -32768,- -32768,32,739,1078,1079,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,996,1059,- -32768,- -32768,701,32,1060,840,701,1061,530,- -32768,840,1066,1062,- -32768,- -32768,1065,- -32768,32,1068,840,1069,701,1064,462,- -32768,- -32768,6731,- -32768,4439,- -32768,1067,100,799,4591,1080,1072,3112,1071,1081,4591,6731,6045,688,6143,692,6241,823,- -32768,1085,- -32768,- -32768,840,133,- -32768,- -32768,1077,- -32768,1094,1084,840,4439,- -32768,- -32768,153,- -32768,6731,- -32768,- -32768,5152,1082,1083,1087,1086,- -32768,885,1089,1090,- -32768,- -32768,742,- -32768,- -32768,- -32768,- -32768,5039,- -32768,- -32768,1084,996,- -32768,- -32768,996,1096,- -32768,701,1095,4439,1117,- -32768,- -32768,- -32768,996,1098,- -32768,- -32768,701,- -32768,1105,694,1100,1107,4439,- -32768,- -32768,1204,823,1110,7322,1106,2496,7224,1103,- -32768,- 17,- -32768,1145,1101,731,788,224,797,328,330,- -32768,- -32768,- -32768,- -32768,1147,7224,2068,- -32768,- -32768,532,4591,542,6339,4591,543,6437,6535,698,1119,- -32768,- -32768,- -32768,6731,1120,- -32768,- -32768,1036,- -32768,420,276,- -32768,- -32768,- -32768,4439,1219,- -32768,- -32768,- -32768,4926,- -32768,1084,1084,996,- -32768,- -32768,1123,1124,1084,996,- -32768,- -32768,- -32768,371,- -32768,- -32768,551,4591,1126,823,2355,- -32768,4439,1118,- -32768,1776,7224,6731,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,6731,- -32768,823,1129,- -32768,4591,4591,579,- -32768,4591,4591,587,4591,609,6633,- -32768,1122,72,840,- -32768,- -32768,2496,1128,1127,- -32768,- -32768,- -32768,1084,- -32768,1143,- -32768,1084,- -32768,- -32768,- -32768,- -32768,1135,1130,1137,7224,823,- -32768,650,368,720,726,726,676,106,106,710,710,710,710,314,314,291,291,- -32768,- -32768,- -32768,372,- -32768,- -32768,- -32768,- -32768,4591,- -32768,- -32768,4591,- -32768,4591,4591,615,- -32768,- -32768,- -32768,1139,763,1136,4439,- -32768,1141,- -32768,- -32768,1042,175,- -32768,- -32768,6731,1776,- -32768,- -32768,- -32768,- -32768,4591,- -32768,- -32768,- -32768,1142,- -32768,455,- -32768,823,- -32768,- -32768,1144,- -32768,- -32768,- -32768,1265,1267,- -32768};
# 1993
static short Cyc_yypgoto[149U]={- -32768,143,- -32768,- -32768,- -32768,- -32768,- -32768,327,- -32768,- 239,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 59,- 130,20,- -32768,- -32768,0,624,- -32768,34,- 183,1154,79,- -32768,- -32768,- 134,- -32768,142,1245,- 741,- -32768,- -32768,- -32768,1009,1007,91,387,- -32768,- -32768,620,- 94,- 505,- -32768,- -32768,126,- -32768,- -32768,239,- 202,1217,- 386,253,- -32768,1125,- -32768,- -32768,1242,- 370,- -32768,580,- 25,- 148,- 143,93,305,588,596,- 416,- 486,- 119,- 472,- 122,- -32768,- 253,- 176,- 545,- 317,- -32768,874,- 170,- 92,- 132,- 108,- 379,66,- -32768,- -32768,43,- 275,- -32768,- 717,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,22,1063,- -32768,682,864,- -32768,165,748,- -32768,- 141,- 393,- 159,- 365,- 356,- 363,936,- 355,- 378,- 264,- 375,- 372,- 164,791,513,946,250,- -32768,- 341,- -32768,114,594,- 44,- 20,- 115,- 196};
# 2015
static short Cyc_yytable[7471U]={64,590,591,259,265,350,660,259,584,278,596,597,326,587,150,598,599,247,248,586,335,64,351,589,488,64,146,365,523,524,616,526,566,328,65,368,369,370,337,347,492,83,341,645,867,87,466,120,104,704,147,412,461,81,671,65,405,64,373,65,448,701,472,64,64,64,744,64,607,86,660,445,146,64,157,640,111,529,955,67,150,108,413,140,141,142,908,143,659,577,304,65,315,152,112,640,255,65,65,65,67,65,374,668,67,676,677,65,227,672,113,226,935,64,64,659,153,592,593,594,595,457,956,234,708,531,228,64,64,64,325,64,64,64,64,534,67,455,467,64,105,553,67,67,67,754,67,65,65,64,457,64,67,371,109,947,475,530,157,152,458,65,65,65,742,65,65,65,65,305,714,152,558,65,559,560,536,41,712,695,459,110,279,65,543,65,44,42,42,458,575,836,67,67,316,535,381,382,227,555,130,459,333,105,554,249,67,67,67,250,67,67,67,67,264,105,228,416,67,315,477,114,- 529,115,- 159,600,601,602,67,532,67,1010,417,383,384,283,371,1015,230,477,550,372,324,447,304,304,264,304,753,415,- 224,205,666,- 224,434,117,252,253,459,1036,519,434,231,911,448,448,448,520,758,653,268,269,270,459,271,272,273,274,64,784,254,170,555,917,256,819,121,918,285,284,286,605,257,653,566,608,64,1062,152,119,41,139,238,64,64,304,64,615,239,44,914,536,65,723,963,964,87,406,407,408,47,491,449,305,305,452,305,371,256,159,65,453,432,446,552,451,257,65,65,929,65,105,792,371,105,916,105,1078,464,118,629,965,966,105,122,555,511,67,227,261,1082,409,366,518,64,445,410,411,733,407,408,- 246,283,371,- 246,67,228,19,20,46,440,740,67,67,305,67,154,41,155,846,866,317,536,850,156,698,44,465,65,478,123,371,439,265,479,825,127,506,861,995,507,41,409,124,737,41,128,734,411,566,44,806,46,129,44,999,1000,654,389,84,477,47,1004,50,51,52,390,391,700,50,51,52,745,67,746,706,617,447,447,447,772,477,170,689,387,618,644,477,388,625,517,445,696,422,423,772,150,971,969,824,46,652,970,658,41,972,973,641,743,642,41,933,64,44,64,643,64,135,43,44,45,172,939,371,47,667,1055,371,454,762,1057,1063,50,51,52,1064,371,366,- 16,760,366,579,64,776,176,317,65,474,65,990,65,64,572,774,825,64,449,449,449,256,993,555,152,229,777,233,501,257,105,585,41,152,232,264,525,152,65,839,43,44,45,105,235,830,105,65,611,536,808,65,471,67,264,67,847,67,453,816,699,264,831,41,920,131,132,708,609,371,857,43,44,45,729,1022,1023,1081,863,697,824,918,1016,67,1028,1029,79,1019,1020,1030,1031,838,67,1018,371,821,67,1021,236,790,654,371,809,825,237,264,791,815,85,761,832,833,106,976,107,477,264,662,477,664,251,665,715,775,528,264,260,371,711,304,262,315,644,79,555,46,674,675,751,392,275,371,655,79,656,276,796,371,79,371,657,658,852,844,977,64,145,331,148,371,371,79,681,682,979,983,685,824,478,690,175,1014,318,1007,332,106,338,393,394,395,396,397,398,399,400,401,402,106,64,65,64,1024,1025,1026,1027,340,371,454,952,79,79,1040,145,344,371,953,79,403,305,1043,152,133,134,79,79,79,474,79,79,79,79,756,65,975,65,264,371,105,146,339,105,1045,371,807,67,446,375,1069,79,264,501,945,352,930,697,996,931,324,41,626,542,692,371,353,371,788,937,44,789,785,379,380,175,555,354,992,355,67,47,67,356,1011,357,1012,545,546,50,51,52,452,802,1077,767,371,41,453,715,50,51,52,385,386,43,44,45,994,676,677,781,1032,1033,1034,902,765,495,371,905,894,941,371,358,942,988,106,41,371,106,1051,106,16,17,18,43,44,45,106,632,633,634,304,1001,721,722,359,471,536,1005,793,794,360,453,1061,50,51,52,799,801,849,871,361,476,367,853,376,898,840,841,88,927,928,377,859,79,437,267,959,960,378,283,476,414,503,961,962,508,41,1073,967,968,1071,371,418,188,43,44,45,46,304,64,442,146,79,463,910,438,495,330,501,470,105,264,915,462,50,51,52,869,870,305,513,480,481,501,502,1076,501,483,88,485,493,504,514,65,515,175,41,91,194,195,92,93,94,505,41,44,509,283,821,512,522,521,43,44,45,527,345,64,144,198,277,532,264,459,548,899,901,539,904,978,907,343,982,346,348,348,540,305,67,544,46,89,557,373,573,578,362,363,610,620,919,65,612,348,348,348,90,91,613,614,92,93,94,41,95,44,621,622,304,623,96,43,44,45,348,1008,97,624,98,99,628,106,630,495,100,631,646,496,490,647,101,50,51,52,106,648,67,106,160,161,162,163,164,649,231,165,166,167,1038,1039,651,650,1041,1042,669,1044,41,683,670,684,686,687,691,88,43,44,45,64,476,693,981,476,702,985,987,705,495,671,709,79,713,79,549,79,50,51,52,305,1050,717,346,718,476,719,720,- 529,725,724,476,726,490,65,727,567,568,569,570,571,728,1065,574,730,1066,739,1067,1068,89,366,580,731,732,735,583,738,1017,741,752,747,433,766,755,225,91,757,348,92,93,94,1079,95,44,759,1035,603,770,96,67,773,780,783,786,97,797,98,99,604,1047,803,804,100,580,324,805,1052,810,101,812,348,736,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,814,822,828,834,842,843,845,848,854,855,177,178,856,868,851,858,860,862,639,873,872,896,897,912,909,179,88,913,924,921,922,348,106,923,934,106,925,926,182,183,184,185,186,932,936,938,940,943,944,946,948,188,951,954,189,190,957,191,974,958,989,997,991,476,1002,1013,1003,1009,1037,1048,1056,192,193,1054,79,1053,1058,1059,476,1060,1070,476,1085,1072,1086,580,1006,580,1074,1080,787,1083,771,263,41,91,194,195,92,93,94,116,487,44,348,494,196,782,349,158,126,336,818,1049,197,817,619,198,811,441,749,627,199,895,576,200,0,1075,201,202,588,490,203,204,0,0,0,0,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,490,0,22,23,24,296,88,26,180,0,348,0,181,27,0,145,0,30,182,183,184,185,186,0,31,32,33,187,106,0,0,188,764,0,189,190,38,191,769,0,0,0,0,0,0,0,779,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,580,0,41,91,194,195,92,93,94,42,43,44,45,46,196,688,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,348,200,0,0,201,202,0,490,203,204,826,827,0,829,0,0,0,0,0,0,0,490,0,0,837,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,348,0,22,23,24,296,88,0,0,0,0,348,0,27,0,0,145,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,490,348,0,0,0,0,0,0,0,0,0,0,0,0,0,0,490,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,177,178,0,0,0,0,345,561,48,302,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,950,0,0,348,192,193,0,0,0,0,0,490,0,0,0,0,0,0,0,0,0,348,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,348,196,0,349,563,0,0,0,0,197,0,0,198,0,422,423,0,199,0,0,200,0,0,201,202,0,0,203,204,0,0,0,0,0,0,0,0,0,0,0,0,0,0,348,0,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,793,794,21,177,178,287,348,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,348,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,301,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,364,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,- 13,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,661,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 13,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,- 13,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 13,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,- 13,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,874,875,0,0,0,0,0,0,0,0,39,40,0,0,0,0,88,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,22,23,24,- 13,0,0,0,0,47,0,48,49,0,0,0,30,50,51,52,0,0,89,31,32,0,0,0,0,0,0,0,0,0,0,38,0,225,91,0,0,92,93,94,0,95,44,39,40,0,0,96,0,0,0,0,0,876,0,98,99,0,0,0,0,100,0,0,877,0,322,101,202,41,0,203,204,0,0,0,42,43,44,45,46,0,324,0,0,0,0,0,0,471,0,48,49,0,0,453,0,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,89,0,0,0,0,0,0,0,39,40,0,0,750,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,322,0,0,0,97,0,98,99,0,0,42,43,100,45,46,0,324,0,101,0,0,0,0,452,0,48,49,0,0,453,88,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,89,0,31,32,0,0,0,0,0,0,0,0,419,0,38,420,91,0,0,92,93,94,0,421,44,0,39,40,0,96,0,0,0,0,0,97,0,98,99,0,422,423,0,100,0,0,0,0,0,101,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,495,0,48,49,0,0,0,0,50,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,89,0,0,0,0,0,0,0,39,40,0,0,0,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,0,0,0,97,0,98,99,0,0,42,43,100,45,46,0,0,149,101,0,277,0,0,0,0,48,49,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,48,49,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,88,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,89,31,32,0,0,0,0,0,0,0,0,0,748,38,0,420,91,0,0,92,93,94,0,421,44,39,40,0,0,96,0,0,0,0,0,97,0,98,99,0,422,423,0,100,0,0,0,0,322,101,0,323,0,0,0,0,0,0,42,43,0,45,46,0,324,0,0,0,0,0,0,0,0,48,49,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,240,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,241,22,23,24,0,0,0,0,48,49,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,177,178,0,0,0,0,0,38,0,0,0,0,0,0,0,179,88,0,180,39,40,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,322,0,189,190,0,191,0,0,0,0,42,43,0,45,46,0,324,0,0,192,193,0,0,0,0,48,49,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,22,23,24,0,0,0,197,0,0,198,0,0,0,329,199,30,0,200,0,0,201,202,31,32,203,204,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,443,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,703,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,710,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,177,178,287,0,288,289,290,291,292,293,294,295,0,0,0,179,88,0,180,297,0,0,181,42,298,0,0,46,182,183,184,185,186,0,0,0,0,187,48,49,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,0,0,44,0,0,196,300,149,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,823,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,835,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,998,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,516,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,606,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,638,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,694,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,763,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,778,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,798,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,800,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,900,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,903,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,906,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,980,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,984,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,986,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,1046,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,768,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,736,0,0,177,178,0,197,0,0,198,0,0,0,0,199,0,0,200,179,88,201,202,0,0,203,204,0,0,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,342,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,345,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,0,0,0,949,0,0,198,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204};
# 2765
static short Cyc_yycheck[7471U]={0,379,380,118,123,181,478,122,373,139,385,386,155,376,73,387,388,109,110,375,168,21,181,378,277,25,70,197,303,304,423,306,349,155,0,199,200,201,170,180,279,21,176,459,785,25,17,47,26,535,70,107,248,111,78,21,220,57,95,25,243,533,264,63,64,65,611,67,409,129,542,241,116,73,74,454,109,316,95,0,139,109,138,63,64,65,803,67,474,364,149,57,151,73,127,474,116,63,64,65,21,67,139,496,25,5,6,73,109,133,122,89,853,113,114,501,73,381,382,383,384,89,139,101,540,321,127,127,128,129,155,131,132,133,134,63,57,245,109,139,26,337,63,64,65,631,67,113,114,149,89,151,73,125,127,872,264,125,158,139,128,127,128,129,136,131,132,133,134,149,130,151,342,139,344,345,324,109,548,520,140,127,139,149,327,151,118,116,116,128,360,736,113,114,151,123,90,91,109,340,57,140,131,89,338,122,127,128,129,126,131,132,133,134,123,101,127,116,139,278,264,122,133,129,124,389,390,391,149,109,151,948,131,127,128,144,125,954,96,283,332,130,122,243,303,304,155,306,130,227,130,86,491,133,232,52,113,114,140,976,122,239,97,130,447,448,449,129,638,471,127,128,129,140,131,132,133,134,278,672,115,127,423,130,109,110,47,134,134,147,148,406,117,495,611,410,296,1014,278,129,109,62,123,303,304,364,306,422,129,118,815,459,278,566,90,91,296,85,86,87,129,278,243,303,304,129,306,125,109,76,296,135,130,242,334,244,117,303,304,844,306,227,683,125,230,817,232,1064,130,108,442,127,128,239,108,496,290,278,109,120,1077,129,197,297,364,535,134,135,85,86,87,122,281,125,125,296,127,20,21,120,238,133,303,304,364,306,127,109,129,759,783,152,540,763,135,525,118,255,364,125,129,125,237,522,130,722,122,129,778,133,132,109,129,123,578,109,122,134,135,736,118,696,120,123,118,930,931,471,137,123,474,129,937,137,138,139,145,146,530,137,138,139,612,364,614,539,424,447,448,449,652,495,127,512,140,126,456,501,144,437,295,631,521,134,135,667,525,137,140,722,120,471,144,473,109,145,146,127,610,129,109,851,482,118,484,135,486,122,117,118,119,111,862,125,129,495,1001,125,245,642,1005,133,137,138,139,133,125,342,123,641,345,130,512,656,123,276,482,264,484,912,486,521,357,655,841,525,447,448,449,109,110,672,512,127,656,109,283,117,424,374,109,521,129,452,305,525,512,743,117,118,119,437,122,729,440,521,414,705,701,525,129,482,471,484,760,486,135,710,525,478,729,109,825,122,123,991,411,125,774,117,118,119,130,961,962,130,124,521,841,134,955,512,967,968,0,958,959,969,970,740,521,957,125,718,525,960,125,130,652,125,702,928,126,522,130,709,22,642,733,734,26,896,28,667,533,482,670,484,116,486,551,656,122,542,129,125,543,696,130,698,640,47,783,120,502,503,624,58,122,125,127,57,129,125,130,125,62,125,135,659,130,755,130,663,70,129,72,125,125,75,505,506,130,130,509,928,125,512,84,954,122,130,129,89,125,96,97,98,99,100,101,102,103,104,105,101,696,663,698,963,964,965,966,126,125,452,876,113,114,130,116,129,125,877,120,126,696,130,698,122,123,127,128,129,471,131,132,133,134,636,696,895,698,642,125,621,780,124,624,130,125,698,663,652,94,130,152,656,495,868,129,845,686,923,848,122,109,122,125,122,125,129,125,122,858,118,125,674,92,93,176,912,129,916,129,696,129,698,129,949,129,951,81,82,137,138,139,129,122,1064,648,125,109,135,715,137,138,139,88,89,117,118,119,918,5,6,663,971,972,973,122,646,129,125,122,793,122,125,129,125,122,227,109,125,230,995,232,17,18,19,117,118,119,239,447,448,449,896,932,124,125,129,129,991,938,22,23,129,135,1013,137,138,139,688,689,762,790,129,264,129,767,141,796,124,125,41,124,125,142,776,276,109,124,142,143,143,785,283,123,285,92,93,288,109,1054,88,89,124,125,116,66,117,118,119,120,954,896,129,942,305,128,808,109,129,156,652,130,793,817,816,109,137,138,139,788,789,896,109,124,124,667,123,1063,670,124,41,124,124,123,122,896,122,338,109,110,111,112,113,114,115,129,109,118,129,853,1060,129,129,133,117,118,119,124,129,954,123,132,126,109,868,140,136,797,798,130,800,900,802,177,903,179,180,181,133,954,896,130,120,94,124,95,109,130,192,193,127,124,822,954,128,199,200,201,109,110,128,128,113,114,115,109,117,118,125,133,1064,130,123,117,118,119,220,946,129,125,131,132,130,424,124,129,137,125,129,133,277,109,143,137,138,139,437,123,954,440,73,74,75,76,77,129,876,80,81,82,979,980,125,130,983,984,122,986,109,129,125,123,133,30,122,41,117,118,119,1064,471,45,902,474,129,905,906,140,129,78,130,482,130,484,331,486,137,138,139,1064,992,130,296,125,495,130,109,133,125,130,501,125,349,1064,125,352,353,354,355,356,125,1040,359,130,1043,125,1045,1046,94,949,367,130,130,124,371,130,956,130,124,136,106,122,130,109,110,130,340,113,114,115,1069,117,118,136,974,392,130,123,1064,130,123,129,124,129,129,131,132,404,988,123,122,137,409,122,130,995,124,143,79,373,123,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,128,127,25,130,109,109,130,130,125,130,25,26,130,129,136,130,130,136,453,130,123,133,124,129,122,40,41,116,125,130,130,423,621,129,122,624,130,130,53,54,55,56,57,130,110,130,124,130,124,28,123,66,129,133,69,70,94,72,94,141,124,25,125,652,124,130,125,124,122,130,110,86,87,129,663,130,124,130,667,125,124,670,0,130,0,520,942,522,130,130,679,130,651,122,109,110,111,112,113,114,115,39,276,118,496,281,121,670,123,75,51,169,715,991,129,710,425,132,705,239,621,440,137,793,361,140,- 1,1060,143,144,377,566,147,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,611,- 1,37,38,39,40,41,42,43,- 1,578,- 1,47,48,- 1,780,- 1,52,53,54,55,56,57,- 1,59,60,61,62,793,- 1,- 1,66,643,- 1,69,70,71,72,649,- 1,- 1,- 1,- 1,- 1,- 1,- 1,657,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,683,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,672,140,- 1,- 1,143,144,- 1,722,147,148,725,726,- 1,728,- 1,- 1,- 1,- 1,- 1,- 1,- 1,736,- 1,- 1,739,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,729,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,740,- 1,48,- 1,- 1,942,52,53,54,55,56,57,- 1,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,825,783,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,841,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,25,26,- 1,- 1,- 1,- 1,129,32,131,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,874,- 1,- 1,877,86,87,- 1,- 1,- 1,- 1,- 1,928,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,895,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,912,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,955,- 1,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1013,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,1063,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,41,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,37,38,39,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,52,137,138,139,- 1,- 1,94,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,83,84,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,106,143,144,109,- 1,147,148,- 1,- 1,- 1,116,117,118,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,135,- 1,137,138,139,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,- 1,- 1,- 1,- 1,123,- 1,106,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,116,117,137,119,120,- 1,122,- 1,143,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,135,41,137,138,139,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,94,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,71,109,110,- 1,- 1,113,114,115,- 1,117,118,- 1,83,84,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,134,135,- 1,137,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,- 1,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,116,117,137,119,120,- 1,- 1,123,143,- 1,126,- 1,- 1,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,- 1,123,37,38,39,40,- 1,- 1,- 1,131,132,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,41,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,94,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,71,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,83,84,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,134,135,- 1,137,- 1,- 1,- 1,- 1,106,143,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,- 1,123,37,38,39,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,41,- 1,43,83,84,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,106,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,122,- 1,- 1,86,87,- 1,- 1,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,37,38,39,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,52,- 1,140,- 1,- 1,143,144,59,60,147,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,124,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,124,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,47,116,49,- 1,- 1,120,53,54,55,56,57,- 1,- 1,- 1,- 1,62,131,132,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,25,26,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,25,26,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,25,26,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,40,41,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp40C[8U]="stdcall";static char _tmp40D[6U]="cdecl";static char _tmp40E[9U]="fastcall";static char _tmp40F[9U]="noreturn";static char _tmp410[6U]="const";static char _tmp411[8U]="aligned";static char _tmp412[7U]="packed";static char _tmp413[7U]="shared";static char _tmp414[7U]="unused";static char _tmp415[5U]="weak";static char _tmp416[10U]="dllimport";static char _tmp417[10U]="dllexport";static char _tmp418[23U]="no_instrument_function";static char _tmp419[12U]="constructor";static char _tmp41A[11U]="destructor";static char _tmp41B[22U]="no_check_memory_usage";static char _tmp41C[5U]="pure";static char _tmp41D[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp370=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp370;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp72E;(_tmp72E.YYINITIALSVAL).tag=64U,(_tmp72E.YYINITIALSVAL).val=0;_tmp72E;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp67E=200U;_tag_dyneither(_region_calloc(yyregion,sizeof(short),_tmp67E),sizeof(short),_tmp67E);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp67D=200U;union Cyc_YYSTYPE*_tmp67C=({struct _RegionHandle*_tmp7E2=yyregion;_region_malloc(_tmp7E2,_check_times(_tmp67D,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp72D=200U;unsigned int i;for(i=0;i < _tmp72D;++ i){_tmp67C[i]=yylval;}}0;});_tmp67C;}),sizeof(union Cyc_YYSTYPE),200U);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp67B=200U;struct Cyc_Yyltype*_tmp67A=({struct _RegionHandle*_tmp7E3=yyregion;_region_malloc(_tmp7E3,_check_times(_tmp67B,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp72C=200U;unsigned int i;for(i=0;i < _tmp72C;++ i){({struct Cyc_Yyltype _tmp7E4=Cyc_yynewloc();_tmp67A[i]=_tmp7E4;});}}0;});_tmp67A;}),sizeof(struct Cyc_Yyltype),200U);
# 170
int yystacksize=200;
# 172
union Cyc_YYSTYPE yyval=yylval;
# 176
int yylen;
# 183
yystate=0;
yyerrstatus=0;
yynerrs=0;
yychar=-2;
# 193
yyssp_offset=-1;
yyvsp_offset=0;
# 196
yylsp_offset=0;
# 202
yynewstate:
# 204
*((short*)_check_dyneither_subscript(yyss,sizeof(short),++ yyssp_offset))=(short)yystate;
# 206
if(yyssp_offset >= yystacksize - 1){
# 208
if(yystacksize >= 10000){
({struct _dyneither_ptr _tmp7E6=({const char*_tmp371="parser stack overflow";_tag_dyneither(_tmp371,sizeof(char),22U);});int _tmp7E5=yystate;Cyc_yyerror(_tmp7E6,_tmp7E5,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp377=(unsigned int)yystacksize;short*_tmp376=({struct _RegionHandle*_tmp7E7=yyregion;_region_malloc(_tmp7E7,_check_times(_tmp377,sizeof(short)));});({{unsigned int _tmp6EC=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6EC;++ i){
i <= (unsigned int)yyssp_offset?_tmp376[i]=*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)):(_tmp376[i]=0);}}0;});_tag_dyneither(_tmp376,sizeof(short),_tmp377);});
struct _dyneither_ptr yyvs1=({unsigned int _tmp375=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp374=({struct _RegionHandle*_tmp7E8=yyregion;_region_malloc(_tmp7E8,_check_times(_tmp375,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp6EB=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6EB;++ i){
# 219
i <= (unsigned int)yyssp_offset?_tmp374[i]=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)):(_tmp374[i]=yylval);}}0;});_tag_dyneither(_tmp374,sizeof(union Cyc_YYSTYPE),_tmp375);});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp373=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp372=({struct _RegionHandle*_tmp7E9=yyregion;_region_malloc(_tmp7E9,_check_times(_tmp373,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp6EA=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6EA;++ i){
i <= (unsigned int)yyssp_offset?_tmp372[i]=*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):({
struct Cyc_Yyltype _tmp7EA=Cyc_yynewloc();_tmp372[i]=_tmp7EA;});}}0;});_tag_dyneither(_tmp372,sizeof(struct Cyc_Yyltype),_tmp373);});
# 225
yyss=yyss1;
yyvs=yyvs1;
# 228
yyls=yyls1;};}
# 238
goto yybackup;
# 240
yybackup:
# 252 "cycbison.simple"
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1087U,sizeof(short),yystate));
if(yyn == -32768)goto yydefault;
# 259
if(yychar == -2)
# 265
yychar=Cyc_yylex(yylex_buf,& yylval,& yylloc);
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 376?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,377U,sizeof(short),yychar)): 298;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7470) || (int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),yyn))!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[yyn];
# 307
if(yyn < 0){
# 309
if(yyn == -32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1086){
int _tmp378=0;_npop_handler(0U);return _tmp378;}
# 326 "cycbison.simple"
if(yychar != 0)
yychar=-2;
# 329
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 331
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 336
if(yyerrstatus != 0)-- yyerrstatus;
# 338
yystate=yyn;
goto yynewstate;
# 342
yydefault:
# 344
 yyn=(int)Cyc_yydefact[yystate];
if(yyn == 0)goto yyerrlab;
# 349
yyreduce:
# 351
 yylen=(int)*((short*)_check_known_subscript_notnull(Cyc_yyr2,533U,sizeof(short),yyn));
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp379=yyn;switch(_tmp379){case 1U: _LL1: _LL2: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1189 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1192
goto _LL0;}case 2U: _LL3: _LL4: {
# 1194
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1195 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp37A;_push_handler(& _tmp37A);{int _tmp37C=0;if(setjmp(_tmp37A.handler))_tmp37C=1;if(!_tmp37C){x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp37B=(void*)_exn_thrown;void*_tmp37D=_tmp37B;void*_tmp37E;if(((struct Cyc_Core_Failure_exn_struct*)_tmp37D)->tag == Cyc_Core_Failure){_LL428: _LL429:
 x=0;goto _LL427;}else{_LL42A: _tmp37E=_tmp37D;_LL42B:(int)_rethrow(_tmp37E);}_LL427:;}};}
{struct _handler_cons _tmp37F;_push_handler(& _tmp37F);{int _tmp381=0;if(setjmp(_tmp37F.handler))_tmp381=1;if(!_tmp381){y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp380=(void*)_exn_thrown;void*_tmp382=_tmp380;void*_tmp383;if(((struct Cyc_Core_Failure_exn_struct*)_tmp382)->tag == Cyc_Core_Failure){_LL42D: _LL42E:
 y=0;goto _LL42C;}else{_LL42F: _tmp383=_tmp382;_LL430:(int)_rethrow(_tmp383);}_LL42C:;}};}
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1202
goto _LL0;}case 3U: _LL5: _LL6: {
# 1204
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1206
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1205 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp386=_cycalloc(sizeof(*_tmp386));({struct Cyc_Absyn_Decl*_tmp7EE=({struct Cyc_Absyn_Decl*_tmp385=_cycalloc(sizeof(*_tmp385));({void*_tmp7ED=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp384=_cycalloc(sizeof(*_tmp384));_tmp384->tag=10U,({struct _tuple0*_tmp7EC=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp384->f1=_tmp7EC;}),({struct Cyc_List_List*_tmp7EB=Cyc_yyget_YY16(yyyyvsp[2]);_tmp384->f2=_tmp7EB;});_tmp384;});_tmp385->r=_tmp7ED;}),_tmp385->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp385;});_tmp386->hd=_tmp7EE;}),_tmp386->tl=0;_tmp386;}));
Cyc_Lex_leave_using();
# 1208
goto _LL0;}case 4U: _LL7: _LL8: {
# 1210
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1212
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1209 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp389=_cycalloc(sizeof(*_tmp389));({struct Cyc_Absyn_Decl*_tmp7F3=({struct Cyc_Absyn_Decl*_tmp388=_cycalloc(sizeof(*_tmp388));({void*_tmp7F2=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->tag=10U,({struct _tuple0*_tmp7F1=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp387->f1=_tmp7F1;}),({struct Cyc_List_List*_tmp7F0=Cyc_yyget_YY16(yyyyvsp[2]);_tmp387->f2=_tmp7F0;});_tmp387;});_tmp388->r=_tmp7F2;}),_tmp388->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp388;});_tmp389->hd=_tmp7F3;}),({struct Cyc_List_List*_tmp7EF=Cyc_yyget_YY16(yyyyvsp[4]);_tmp389->tl=_tmp7EF;});_tmp389;}));
goto _LL0;}case 5U: _LL9: _LLA: {
# 1212
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1214
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1212 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp38D=_cycalloc(sizeof(*_tmp38D));({struct Cyc_Absyn_Decl*_tmp7F8=({struct Cyc_Absyn_Decl*_tmp38C=_cycalloc(sizeof(*_tmp38C));({void*_tmp7F7=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp38B=_cycalloc(sizeof(*_tmp38B));_tmp38B->tag=9U,({struct _dyneither_ptr*_tmp7F6=({struct _dyneither_ptr*_tmp38A=_cycalloc(sizeof(*_tmp38A));({struct _dyneither_ptr _tmp7F5=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp38A=_tmp7F5;});_tmp38A;});_tmp38B->f1=_tmp7F6;}),({struct Cyc_List_List*_tmp7F4=Cyc_yyget_YY16(yyyyvsp[2]);_tmp38B->f2=_tmp7F4;});_tmp38B;});_tmp38C->r=_tmp7F7;}),_tmp38C->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp38C;});_tmp38D->hd=_tmp7F8;}),_tmp38D->tl=0;_tmp38D;}));
Cyc_Lex_leave_namespace();
# 1215
goto _LL0;}case 6U: _LLB: _LLC: {
# 1217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1217 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp38E;_push_handler(& _tmp38E);{int _tmp390=0;if(setjmp(_tmp38E.handler))_tmp390=1;if(!_tmp390){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp38F=(void*)_exn_thrown;void*_tmp391=_tmp38F;void*_tmp393;if(((struct Cyc_Core_Failure_exn_struct*)_tmp391)->tag == Cyc_Core_Failure){_LL432: _LL433:
 nspace=({const char*_tmp392="";_tag_dyneither(_tmp392,sizeof(char),1U);});goto _LL431;}else{_LL434: _tmp393=_tmp391;_LL435:(int)_rethrow(_tmp393);}_LL431:;}};}
# 1222
{struct _handler_cons _tmp394;_push_handler(& _tmp394);{int _tmp396=0;if(setjmp(_tmp394.handler))_tmp396=1;if(!_tmp396){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp395=(void*)_exn_thrown;void*_tmp397=_tmp395;void*_tmp398;if(((struct Cyc_Core_Failure_exn_struct*)_tmp397)->tag == Cyc_Core_Failure){_LL437: _LL438:
 x=0;goto _LL436;}else{_LL439: _tmp398=_tmp397;_LL43A:(int)_rethrow(_tmp398);}_LL436:;}};}
# 1225
{struct _handler_cons _tmp399;_push_handler(& _tmp399);{int _tmp39B=0;if(setjmp(_tmp399.handler))_tmp39B=1;if(!_tmp39B){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp39A=(void*)_exn_thrown;void*_tmp39C=_tmp39A;void*_tmp39D;if(((struct Cyc_Core_Failure_exn_struct*)_tmp39C)->tag == Cyc_Core_Failure){_LL43C: _LL43D:
 y=0;goto _LL43B;}else{_LL43E: _tmp39D=_tmp39C;_LL43F:(int)_rethrow(_tmp39D);}_LL43B:;}};}
# 1228
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_Absyn_Decl*_tmp7FB=({struct Cyc_Absyn_Decl*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({void*_tmp7FA=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->tag=9U,({struct _dyneither_ptr*_tmp7F9=({struct _dyneither_ptr*_tmp39E=_cycalloc(sizeof(*_tmp39E));*_tmp39E=nspace;_tmp39E;});_tmp39F->f1=_tmp7F9;}),_tmp39F->f2=x;_tmp39F;});_tmp3A0->r=_tmp7FA;}),_tmp3A0->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A0;});_tmp3A1->hd=_tmp7FB;}),_tmp3A1->tl=y;_tmp3A1;}));
# 1230
goto _LL0;}case 7U: _LLD: _LLE: {
# 1232
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1234
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1231 "parse.y"
int _tmp3A2=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp3A2)
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({struct Cyc_Absyn_Decl*_tmp7FF=({struct Cyc_Absyn_Decl*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({void*_tmp7FE=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));_tmp3A3->tag=11U,({struct Cyc_List_List*_tmp7FD=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A3->f1=_tmp7FD;});_tmp3A3;});_tmp3A4->r=_tmp7FE;}),_tmp3A4->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A4;});_tmp3A5->hd=_tmp7FF;}),({struct Cyc_List_List*_tmp7FC=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3A5->tl=_tmp7FC;});_tmp3A5;}));else{
# 1235
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({struct Cyc_Absyn_Decl*_tmp803=({struct Cyc_Absyn_Decl*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({void*_tmp802=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->tag=12U,({struct Cyc_List_List*_tmp801=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A6->f1=_tmp801;}),_tmp3A6->f2=0;_tmp3A6;});_tmp3A7->r=_tmp802;}),_tmp3A7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A7;});_tmp3A8->hd=_tmp803;}),({struct Cyc_List_List*_tmp800=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3A8->tl=_tmp800;});_tmp3A8;}));}
# 1237
goto _LL0;}case 8U: _LLF: _LL10: {
# 1239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1238 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp3A9=0U;({unsigned int _tmp805=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp804=({const char*_tmp3AA="expecting \"C include\"";_tag_dyneither(_tmp3AA,sizeof(char),22U);});Cyc_Warn_err(_tmp805,_tmp804,_tag_dyneither(_tmp3A9,sizeof(void*),0U));});});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct Cyc_Absyn_Decl*_tmp809=({struct Cyc_Absyn_Decl*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({void*_tmp808=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3AB=_cycalloc(sizeof(*_tmp3AB));_tmp3AB->tag=12U,({struct Cyc_List_List*_tmp807=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3AB->f1=_tmp807;}),_tmp3AB->f2=exs;_tmp3AB;});_tmp3AC->r=_tmp808;}),_tmp3AC->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3AC;});_tmp3AD->hd=_tmp809;}),({struct Cyc_List_List*_tmp806=Cyc_yyget_YY16(yyyyvsp[5]);_tmp3AD->tl=_tmp806;});_tmp3AD;}));
# 1243
goto _LL0;};}case 9U: _LL11: _LL12: {
# 1245
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1244 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));({struct Cyc_Absyn_Decl*_tmp80B=({struct Cyc_Absyn_Decl*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));_tmp3AE->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3AE->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3AE;});_tmp3AF->hd=_tmp80B;}),({struct Cyc_List_List*_tmp80A=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3AF->tl=_tmp80A;});_tmp3AF;}));
goto _LL0;}case 10U: _LL13: _LL14: {
# 1247
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1249
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1246 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct Cyc_Absyn_Decl*_tmp80D=({struct Cyc_Absyn_Decl*_tmp3B0=_cycalloc(sizeof(*_tmp3B0));_tmp3B0->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3B0->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B0;});_tmp3B1->hd=_tmp80D;}),({struct Cyc_List_List*_tmp80C=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B1->tl=_tmp80C;});_tmp3B1;}));
goto _LL0;}case 11U: _LL15: _LL16: {
# 1249
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1251
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1248 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));({struct Cyc_Absyn_Decl*_tmp80F=({struct Cyc_Absyn_Decl*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3B2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B2;});_tmp3B3->hd=_tmp80F;}),({struct Cyc_List_List*_tmp80E=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B3->tl=_tmp80E;});_tmp3B3;}));
goto _LL0;}case 12U: _LL17: _LL18: {
# 1251
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1253
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1250 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));({struct Cyc_Absyn_Decl*_tmp811=({struct Cyc_Absyn_Decl*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3B4->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B4;});_tmp3B5->hd=_tmp811;}),({struct Cyc_List_List*_tmp810=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B5->tl=_tmp810;});_tmp3B5;}));
goto _LL0;}case 13U: _LL19: _LL1A:
# 1253
 yyval=Cyc_YY16(0);
goto _LL0;case 14U: _LL1B: _LL1C: {
# 1256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1256 "parse.y"
Cyc_Parse_parsing_tempest=1;
goto _LL0;}case 15U: _LL1D: _LL1E: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1261 "parse.y"
Cyc_Parse_parsing_tempest=0;
goto _LL0;}case 16U: _LL1F: _LL20: {
# 1264
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1266 "parse.y"
struct _dyneither_ptr _tmp3B6=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({struct _dyneither_ptr _tmp812=(struct _dyneither_ptr)_tmp3B6;Cyc_strcmp(_tmp812,({const char*_tmp3B7="C";_tag_dyneither(_tmp3B7,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _dyneither_ptr _tmp813=(struct _dyneither_ptr)_tmp3B6;Cyc_strcmp(_tmp813,({const char*_tmp3B8="C include";_tag_dyneither(_tmp3B8,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1273
({void*_tmp3B9=0U;({unsigned int _tmp815=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp814=({const char*_tmp3BA="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3BA,sizeof(char),29U);});Cyc_Warn_err(_tmp815,_tmp814,_tag_dyneither(_tmp3B9,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1277
goto _LL0;}case 17U: _LL21: _LL22: {
# 1279
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1281
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1280 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL0;}case 18U: _LL23: _LL24: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1284 "parse.y"
yyval=yyyyvsp[2];
goto _LL0;}case 19U: _LL25: _LL26: {
# 1287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1285 "parse.y"
yyval=Cyc_YY52(0);
goto _LL0;}case 20U: _LL27: _LL28: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1289 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct _tuple27*_tmp817=({struct _tuple27*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));_tmp3BB->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp816=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3BB->f2=_tmp816;}),_tmp3BB->f3=0;_tmp3BB;});_tmp3BC->hd=_tmp817;}),_tmp3BC->tl=0;_tmp3BC;}));
goto _LL0;}case 21U: _LL29: _LL2A: {
# 1292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1290 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({struct _tuple27*_tmp819=({struct _tuple27*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp818=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3BD->f2=_tmp818;}),_tmp3BD->f3=0;_tmp3BD;});_tmp3BE->hd=_tmp819;}),_tmp3BE->tl=0;_tmp3BE;}));
goto _LL0;}case 22U: _LL2B: _LL2C: {
# 1293
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1292 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct _tuple27*_tmp81C=({struct _tuple27*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));_tmp3BF->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp81B=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3BF->f2=_tmp81B;}),_tmp3BF->f3=0;_tmp3BF;});_tmp3C0->hd=_tmp81C;}),({struct Cyc_List_List*_tmp81A=Cyc_yyget_YY52(yyyyvsp[2]);_tmp3C0->tl=_tmp81A;});_tmp3C0;}));
goto _LL0;}case 23U: _LL2D: _LL2E: {
# 1295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1296 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Decl*_tmp81F=({void*_tmp81E=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp81D=Cyc_yyget_YY15(yyyyvsp[0]);_tmp3C1->f1=_tmp81D;});_tmp3C1;});Cyc_Absyn_new_decl(_tmp81E,(unsigned int)(yyyylsp[0]).first_line);});_tmp3C2->hd=_tmp81F;}),_tmp3C2->tl=0;_tmp3C2;}));
goto _LL0;}case 24U: _LL2F: _LL30: {
# 1299
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1297 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 25U: _LL31: _LL32: {
# 1300
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1298 "parse.y"
yyval=Cyc_YY16(0);
goto _LL0;}case 28U: _LL33: _LL34: {
# 1301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1307 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp822=yyr;struct Cyc_Parse_Declarator _tmp821=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp820=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp822,0,_tmp821,0,_tmp820,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 29U: _LL35: _LL36: {
# 1310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1309 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C3=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp825=yyr;struct Cyc_Parse_Declarator _tmp824=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp823=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp825,& _tmp3C3,_tmp824,0,_tmp823,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 30U: _LL37: _LL38: {
# 1313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1322 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp829=yyr;struct Cyc_Parse_Declarator _tmp828=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp827=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp826=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp829,0,_tmp828,_tmp827,_tmp826,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 31U: _LL39: _LL3A: {
# 1325
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1327
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1324 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C4=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp82D=yyr;struct Cyc_Parse_Declarator _tmp82C=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp82B=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp82A=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp82D,& _tmp3C4,_tmp82C,_tmp82B,_tmp82A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 32U: _LL3B: _LL3C: {
# 1328
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1332 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C5=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp830=yyr;struct Cyc_Parse_Declarator _tmp82F=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp82E=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp830,& _tmp3C5,_tmp82F,0,_tmp82E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 33U: _LL3D: _LL3E: {
# 1336
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1335 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3C6=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp834=yyr;struct Cyc_Parse_Declarator _tmp833=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp832=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp831=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp834,& _tmp3C6,_tmp833,_tmp832,_tmp831,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 34U: _LL3F: _LL40: {
# 1339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1340 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL0;}case 35U: _LL41: _LL42: {
# 1343
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1343 "parse.y"
Cyc_Lex_leave_using();
goto _LL0;}case 36U: _LL43: _LL44: {
# 1346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1346 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct _dyneither_ptr _tmp835=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp3C7=_tmp835;});_tmp3C7;}));yyval=yyyyvsp[1];
goto _LL0;}case 37U: _LL45: _LL46: {
# 1349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1349 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL0;}case 38U: _LL47: _LL48: {
# 1352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
int _tmp3C8=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp837=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp836=(unsigned int)_tmp3C8;Cyc_Parse_make_declarations(_tmp837,0,_tmp836,(unsigned int)_tmp3C8);}));
goto _LL0;}case 39U: _LL49: _LL4A: {
# 1359
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1358 "parse.y"
int _tmp3C9=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp83A=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp839=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp838=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp83A,_tmp839,_tmp838,(unsigned int)_tmp3C9);}));
goto _LL0;}case 40U: _LL4B: _LL4C: {
# 1362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1362 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct Cyc_Absyn_Decl*_tmp83D=({struct Cyc_Absyn_Pat*_tmp83C=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp83B=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp83C,_tmp83B,(unsigned int)(yyyylsp[0]).first_line);});_tmp3CA->hd=_tmp83D;}),_tmp3CA->tl=0;_tmp3CA;}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1364 "parse.y"
struct Cyc_List_List*_tmp3CB=0;
{struct Cyc_List_List*_tmp3CC=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp3CC != 0;_tmp3CC=_tmp3CC->tl){
struct _dyneither_ptr*_tmp3CD=(struct _dyneither_ptr*)_tmp3CC->hd;
struct _tuple0*qv=({struct _tuple0*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({union Cyc_Absyn_Nmspace _tmp83E=Cyc_Absyn_Rel_n(0);_tmp3D0->f1=_tmp83E;}),_tmp3D0->f2=_tmp3CD;_tmp3D0;});
struct Cyc_Absyn_Vardecl*_tmp3CE=({struct _tuple0*_tmp83F=qv;Cyc_Absyn_new_vardecl(0U,_tmp83F,Cyc_Absyn_wildtyp(0),0);});
_tmp3CB=({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->hd=_tmp3CE,_tmp3CF->tl=_tmp3CB;_tmp3CF;});}}
# 1371
_tmp3CB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3CB);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct Cyc_Absyn_Decl*_tmp840=Cyc_Absyn_letv_decl(_tmp3CB,(unsigned int)(yyyylsp[0]).first_line);_tmp3D1->hd=_tmp840;}),_tmp3D1->tl=0;_tmp3D1;}));
# 1374
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1377 "parse.y"
struct _dyneither_ptr _tmp3D2=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1379
if(({struct _dyneither_ptr _tmp841=(struct _dyneither_ptr)_tmp3D2;Cyc_zstrcmp(_tmp841,({const char*_tmp3D3="`H";_tag_dyneither(_tmp3D3,sizeof(char),3U);}));})== 0)
({void*_tmp3D4=0U;({unsigned int _tmp843=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp842=({const char*_tmp3D5="bad occurrence of heap region";_tag_dyneither(_tmp3D5,sizeof(char),30U);});Cyc_Warn_err(_tmp843,_tmp842,_tag_dyneither(_tmp3D4,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp844=(struct _dyneither_ptr)_tmp3D2;Cyc_zstrcmp(_tmp844,({const char*_tmp3D6="`U";_tag_dyneither(_tmp3D6,sizeof(char),3U);}));})== 0)
({void*_tmp3D7=0U;({unsigned int _tmp846=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp845=({const char*_tmp3D8="bad occurrence of unique region";_tag_dyneither(_tmp3D8,sizeof(char),32U);});Cyc_Warn_err(_tmp846,_tmp845,_tag_dyneither(_tmp3D7,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct _dyneither_ptr*_tmp848=({struct _dyneither_ptr*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));*_tmp3DC=_tmp3D2;_tmp3DC;});_tmp3DD->name=_tmp848;}),_tmp3DD->identity=- 1,({void*_tmp847=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3DD->kind=_tmp847;});_tmp3DD;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp84C=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp84B=({struct _tuple0*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp84A=({struct _dyneither_ptr*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({struct _dyneither_ptr _tmp849=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp3DA=_tmp849;});_tmp3DA;});_tmp3DB->f2=_tmp84A;});_tmp3DB;});Cyc_Absyn_new_vardecl(_tmp84C,_tmp84B,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct Cyc_Absyn_Decl*_tmp84D=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3D9->hd=_tmp84D;}),_tmp3D9->tl=0;_tmp3D9;}));
# 1388
goto _LL0;};}case 43U: _LL51: _LL52: {
# 1390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1390 "parse.y"
struct _dyneither_ptr _tmp3DE=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({struct _dyneither_ptr _tmp84E=(struct _dyneither_ptr)_tmp3DE;Cyc_zstrcmp(_tmp84E,({const char*_tmp3DF="H";_tag_dyneither(_tmp3DF,sizeof(char),2U);}));})== 0)
({void*_tmp3E0=0U;({unsigned int _tmp850=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp84F=({const char*_tmp3E1="bad occurrence of heap region `H";_tag_dyneither(_tmp3E1,sizeof(char),33U);});Cyc_Warn_err(_tmp850,_tmp84F,_tag_dyneither(_tmp3E0,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp851=(struct _dyneither_ptr)_tmp3DE;Cyc_zstrcmp(_tmp851,({const char*_tmp3E2="U";_tag_dyneither(_tmp3E2,sizeof(char),2U);}));})== 0)
({void*_tmp3E3=0U;({unsigned int _tmp853=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp852=({const char*_tmp3E4="bad occurrence of unique region `U";_tag_dyneither(_tmp3E4,sizeof(char),35U);});Cyc_Warn_err(_tmp853,_tmp852,_tag_dyneither(_tmp3E3,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct _dyneither_ptr*_tmp857=({struct _dyneither_ptr*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({struct _dyneither_ptr _tmp856=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3EA=({struct Cyc_String_pa_PrintArg_struct _tmp6ED;_tmp6ED.tag=0U,_tmp6ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3DE);_tmp6ED;});void*_tmp3E8[1U];_tmp3E8[0]=& _tmp3EA;({struct _dyneither_ptr _tmp855=({const char*_tmp3E9="`%s";_tag_dyneither(_tmp3E9,sizeof(char),4U);});Cyc_aprintf(_tmp855,_tag_dyneither(_tmp3E8,sizeof(void*),1U));});});*_tmp3EB=_tmp856;});_tmp3EB;});_tmp3EC->name=_tmp857;}),_tmp3EC->identity=- 1,({
void*_tmp854=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3EC->kind=_tmp854;});_tmp3EC;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp85A=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp859=({struct _tuple0*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp858=({struct _dyneither_ptr*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));*_tmp3E6=_tmp3DE;_tmp3E6;});_tmp3E7->f2=_tmp858;});_tmp3E7;});Cyc_Absyn_new_vardecl(_tmp85A,_tmp859,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));({struct Cyc_Absyn_Decl*_tmp85B=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3E5->hd=_tmp85B;}),_tmp3E5->tl=0;_tmp3E5;}));
# 1401
goto _LL0;};}case 44U: _LL53: _LL54: {
# 1403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1403 "parse.y"
struct _dyneither_ptr _tmp3ED=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp3EE=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp3EF=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({struct _dyneither_ptr _tmp85C=(struct _dyneither_ptr)_tmp3EE;Cyc_strcmp(_tmp85C,({const char*_tmp3F0="open";_tag_dyneither(_tmp3F0,sizeof(char),5U);}));})!= 0)({void*_tmp3F1=0U;({unsigned int _tmp85E=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp85D=({const char*_tmp3F2="expecting `open'";_tag_dyneither(_tmp3F2,sizeof(char),17U);});Cyc_Warn_err(_tmp85E,_tmp85D,_tag_dyneither(_tmp3F1,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));({struct _dyneither_ptr*_tmp862=({struct _dyneither_ptr*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({struct _dyneither_ptr _tmp861=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F8=({struct Cyc_String_pa_PrintArg_struct _tmp6EE;_tmp6EE.tag=0U,_tmp6EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3ED);_tmp6EE;});void*_tmp3F6[1U];_tmp3F6[0]=& _tmp3F8;({struct _dyneither_ptr _tmp860=({const char*_tmp3F7="`%s";_tag_dyneither(_tmp3F7,sizeof(char),4U);});Cyc_aprintf(_tmp860,_tag_dyneither(_tmp3F6,sizeof(void*),1U));});});*_tmp3F9=_tmp861;});_tmp3F9;});_tmp3FA->name=_tmp862;}),_tmp3FA->identity=- 1,({
void*_tmp85F=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3FA->kind=_tmp85F;});_tmp3FA;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp865=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp864=({struct _tuple0*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp863=({struct _dyneither_ptr*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));*_tmp3F4=_tmp3ED;_tmp3F4;});_tmp3F5->f2=_tmp863;});_tmp3F5;});Cyc_Absyn_new_vardecl(_tmp865,_tmp864,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));({struct Cyc_Absyn_Decl*_tmp866=Cyc_Absyn_region_decl(tv,vd,_tmp3EF,(unsigned int)(yyyylsp[0]).first_line);_tmp3F3->hd=_tmp866;}),_tmp3F3->tl=0;_tmp3F3;}));
# 1413
goto _LL0;};}case 45U: _LL55: _LL56: {
# 1415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1417 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 46U: _LL57: _LL58: {
# 1420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1419 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp867=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp867,Cyc_yyget_YY16(yyyyvsp[1]));}));
goto _LL0;}case 47U: _LL59: _LL5A: {
# 1422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6EF;({enum Cyc_Parse_Storage_class*_tmp86A=Cyc_yyget_YY20(yyyyvsp[0]);_tmp6EF.sc=_tmp86A;}),({struct Cyc_Absyn_Tqual _tmp869=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6EF.tq=_tmp869;}),({
struct Cyc_Parse_Type_specifier _tmp868=Cyc_Parse_empty_spec(0U);_tmp6EF.type_specs=_tmp868;}),_tmp6EF.is_inline=0,_tmp6EF.attributes=0;_tmp6EF;}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1428 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3FB=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp3FB.sc != 0)
({void*_tmp3FC=0U;({unsigned int _tmp86C=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp86B=({const char*_tmp3FD="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp3FD,sizeof(char),73U);});Cyc_Warn_warn(_tmp86C,_tmp86B,_tag_dyneither(_tmp3FC,sizeof(void*),0U));});});
# 1432
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F0;({enum Cyc_Parse_Storage_class*_tmp86D=Cyc_yyget_YY20(yyyyvsp[0]);_tmp6F0.sc=_tmp86D;}),_tmp6F0.tq=_tmp3FB.tq,_tmp6F0.type_specs=_tmp3FB.type_specs,_tmp6F0.is_inline=_tmp3FB.is_inline,_tmp6F0.attributes=_tmp3FB.attributes;_tmp6F0;}));
# 1436
goto _LL0;}case 49U: _LL5D: _LL5E: {
# 1438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1437 "parse.y"
({void*_tmp3FE=0U;({unsigned int _tmp86F=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp86E=({const char*_tmp3FF="__extension__ keyword ignored in declaration";_tag_dyneither(_tmp3FF,sizeof(char),45U);});Cyc_Warn_warn(_tmp86F,_tmp86E,_tag_dyneither(_tmp3FE,sizeof(void*),0U));});});
yyval=yyyyvsp[1];
# 1440
goto _LL0;}case 50U: _LL5F: _LL60: {
# 1442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1441 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F1;_tmp6F1.sc=0,({struct Cyc_Absyn_Tqual _tmp871=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6F1.tq=_tmp871;}),({
struct Cyc_Parse_Type_specifier _tmp870=Cyc_yyget_YY21(yyyyvsp[0]);_tmp6F1.type_specs=_tmp870;}),_tmp6F1.is_inline=0,_tmp6F1.attributes=0;_tmp6F1;}));
goto _LL0;}case 51U: _LL61: _LL62: {
# 1445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1444 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp400=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F2;_tmp6F2.sc=_tmp400.sc,_tmp6F2.tq=_tmp400.tq,({
struct Cyc_Parse_Type_specifier _tmp874=({unsigned int _tmp873=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp872=_tmp400.type_specs;Cyc_Parse_combine_specifiers(_tmp873,_tmp872,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp6F2.type_specs=_tmp874;}),_tmp6F2.is_inline=_tmp400.is_inline,_tmp6F2.attributes=_tmp400.attributes;_tmp6F2;}));
# 1450
goto _LL0;}case 52U: _LL63: _LL64: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1451 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F3;_tmp6F3.sc=0,({struct Cyc_Absyn_Tqual _tmp876=Cyc_yyget_YY23(yyyyvsp[0]);_tmp6F3.tq=_tmp876;}),({struct Cyc_Parse_Type_specifier _tmp875=Cyc_Parse_empty_spec(0U);_tmp6F3.type_specs=_tmp875;}),_tmp6F3.is_inline=0,_tmp6F3.attributes=0;_tmp6F3;}));
goto _LL0;}case 53U: _LL65: _LL66: {
# 1454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1453 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp401=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F4;_tmp6F4.sc=_tmp401.sc,({struct Cyc_Absyn_Tqual _tmp878=({struct Cyc_Absyn_Tqual _tmp877=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp877,_tmp401.tq);});_tmp6F4.tq=_tmp878;}),_tmp6F4.type_specs=_tmp401.type_specs,_tmp6F4.is_inline=_tmp401.is_inline,_tmp6F4.attributes=_tmp401.attributes;_tmp6F4;}));
# 1458
goto _LL0;}case 54U: _LL67: _LL68: {
# 1460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1459 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F5;_tmp6F5.sc=0,({struct Cyc_Absyn_Tqual _tmp87A=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6F5.tq=_tmp87A;}),({
struct Cyc_Parse_Type_specifier _tmp879=Cyc_Parse_empty_spec(0U);_tmp6F5.type_specs=_tmp879;}),_tmp6F5.is_inline=1,_tmp6F5.attributes=0;_tmp6F5;}));
goto _LL0;}case 55U: _LL69: _LL6A: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1462 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp402=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F6;_tmp6F6.sc=_tmp402.sc,_tmp6F6.tq=_tmp402.tq,_tmp6F6.type_specs=_tmp402.type_specs,_tmp6F6.is_inline=1,_tmp6F6.attributes=_tmp402.attributes;_tmp6F6;}));
# 1466
goto _LL0;}case 56U: _LL6B: _LL6C: {
# 1468
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1470
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1467 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F7;_tmp6F7.sc=0,({struct Cyc_Absyn_Tqual _tmp87D=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6F7.tq=_tmp87D;}),({
struct Cyc_Parse_Type_specifier _tmp87C=Cyc_Parse_empty_spec(0U);_tmp6F7.type_specs=_tmp87C;}),_tmp6F7.is_inline=0,({struct Cyc_List_List*_tmp87B=Cyc_yyget_YY45(yyyyvsp[0]);_tmp6F7.attributes=_tmp87B;});_tmp6F7;}));
goto _LL0;}case 57U: _LL6D: _LL6E: {
# 1471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1470 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp403=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6F8;_tmp6F8.sc=_tmp403.sc,_tmp6F8.tq=_tmp403.tq,_tmp6F8.type_specs=_tmp403.type_specs,_tmp6F8.is_inline=_tmp403.is_inline,({
# 1473
struct Cyc_List_List*_tmp87F=({struct Cyc_List_List*_tmp87E=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp87E,_tmp403.attributes);});_tmp6F8.attributes=_tmp87F;});_tmp6F8;}));
goto _LL0;}case 58U: _LL6F: _LL70: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1477 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL0;}case 59U: _LL71: _LL72: {
# 1481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1479 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL0;}case 60U: _LL73: _LL74: {
# 1483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1481 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL0;}case 61U: _LL75: _LL76: {
# 1485
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1487
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1483 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL0;}case 62U: _LL77: _LL78: {
# 1487
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1489
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1486 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _dyneither_ptr _tmp880=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmp880,({const char*_tmp404="C";_tag_dyneither(_tmp404,sizeof(char),2U);}));})!= 0)
({void*_tmp405=0U;({unsigned int _tmp882=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp881=({const char*_tmp406="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp406,sizeof(char),37U);});Cyc_Warn_err(_tmp882,_tmp881,_tag_dyneither(_tmp405,sizeof(void*),0U));});});
yyval=Cyc_YY20(& externC_sc);
# 1491
goto _LL0;}case 63U: _LL79: _LL7A: {
# 1493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1491 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL0;}case 64U: _LL7B: _LL7C: {
# 1495
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1497
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1494 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL0;}case 65U: _LL7D: _LL7E:
# 1498
 yyval=Cyc_YY45(0);
goto _LL0;case 66U: _LL7F: _LL80: {
# 1501
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1501 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 67U: _LL81: _LL82: {
# 1504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1506 "parse.y"
yyval=yyyyvsp[3];
goto _LL0;}case 68U: _LL83: _LL84: {
# 1509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1510 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp407=_cycalloc(sizeof(*_tmp407));({void*_tmp883=Cyc_yyget_YY46(yyyyvsp[0]);_tmp407->hd=_tmp883;}),_tmp407->tl=0;_tmp407;}));
goto _LL0;}case 69U: _LL85: _LL86: {
# 1513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1511 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp408=_cycalloc(sizeof(*_tmp408));({void*_tmp885=Cyc_yyget_YY46(yyyyvsp[0]);_tmp408->hd=_tmp885;}),({struct Cyc_List_List*_tmp884=Cyc_yyget_YY45(yyyyvsp[2]);_tmp408->tl=_tmp884;});_tmp408;}));
goto _LL0;}case 70U: _LL87: _LL88: {
# 1514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1516 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp40C,_tmp40C,_tmp40C + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp40D,_tmp40D,_tmp40D + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp40E,_tmp40E,_tmp40E + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp40F,_tmp40F,_tmp40F + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp410,_tmp410,_tmp410 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp411,_tmp411,_tmp411 + 8U},(void*)& att_aligned},{{_tmp412,_tmp412,_tmp412 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp413,_tmp413,_tmp413 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp414,_tmp414,_tmp414 + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp415,_tmp415,_tmp415 + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp416,_tmp416,_tmp416 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp417,_tmp417,_tmp417 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp418,_tmp418,_tmp418 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp419,_tmp419,_tmp419 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp41A,_tmp41A,_tmp41A + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp41B,_tmp41B,_tmp41B + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp41C,_tmp41C,_tmp41C + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp41D,_tmp41D,_tmp41D + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1537
struct _dyneither_ptr _tmp409=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1539
if((((_get_dyneither_size(_tmp409,sizeof(char))> (unsigned int)4  && (int)*((const char*)_check_dyneither_subscript(_tmp409,sizeof(char),0))== (int)'_') && (int)*((const char*)_check_dyneither_subscript(_tmp409,sizeof(char),1))== (int)'_') && (int)*((const char*)_check_dyneither_subscript(_tmp409,sizeof(char),(int)(_get_dyneither_size(_tmp409,sizeof(char))- (unsigned int)2)))== (int)'_') && (int)*((const char*)_check_dyneither_subscript(_tmp409,sizeof(char),(int)(_get_dyneither_size(_tmp409,sizeof(char))- (unsigned int)3)))== (int)'_')
# 1541
_tmp409=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp409,2,_get_dyneither_size(_tmp409,sizeof(char))- (unsigned int)5);{
int i=0;
for(0;(unsigned int)i < 18U;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp409,(struct _dyneither_ptr)(*((struct _tuple28*)_check_known_subscript_notnull(att_map,18U,sizeof(struct _tuple28),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1548
if((unsigned int)i == 18U){
({void*_tmp40A=0U;({unsigned int _tmp887=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp886=({const char*_tmp40B="unrecognized attribute";_tag_dyneither(_tmp40B,sizeof(char),23U);});Cyc_Warn_err(_tmp887,_tmp886,_tag_dyneither(_tmp40A,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1553
goto _LL0;};}case 71U: _LL89: _LL8A: {
# 1555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1553 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1556
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1558
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1555 "parse.y"
struct _dyneither_ptr _tmp41E=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp41F=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({struct _dyneither_ptr _tmp889=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp889,({const char*_tmp420="aligned";_tag_dyneither(_tmp420,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp888=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp888,({const char*_tmp421="__aligned__";_tag_dyneither(_tmp421,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->tag=6U,_tmp422->f1=_tmp41F;_tmp422;});else{
if(({struct _dyneither_ptr _tmp88B=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp88B,({const char*_tmp423="section";_tag_dyneither(_tmp423,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp88A=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp88A,({const char*_tmp424="__section__";_tag_dyneither(_tmp424,sizeof(char),12U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp41F);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->tag=8U,_tmp425->f1=str;_tmp425;});}else{
if(({struct _dyneither_ptr _tmp88C=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp88C,({const char*_tmp426="__mode__";_tag_dyneither(_tmp426,sizeof(char),9U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp41F);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427->tag=24U,_tmp427->f1=str;_tmp427;});}else{
if(({struct _dyneither_ptr _tmp88D=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp88D,({const char*_tmp428="alias";_tag_dyneither(_tmp428,sizeof(char),6U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp41F);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->tag=25U,_tmp429->f1=str;_tmp429;});}else{
# 1571
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp41F);
if(({struct _dyneither_ptr _tmp88F=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp88F,({const char*_tmp42A="regparm";_tag_dyneither(_tmp42A,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp88E=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp88E,({const char*_tmp42B="__regparm__";_tag_dyneither(_tmp42B,sizeof(char),12U);}));})== 0){
if(n < 0  || n > 3)
({void*_tmp42C=0U;({unsigned int _tmp891=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp890=({const char*_tmp42D="regparm requires value between 0 and 3";_tag_dyneither(_tmp42D,sizeof(char),39U);});Cyc_Warn_err(_tmp891,_tmp890,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->tag=0U,_tmp42E->f1=n;_tmp42E;});}else{
if(({struct _dyneither_ptr _tmp893=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp893,({const char*_tmp42F="initializes";_tag_dyneither(_tmp42F,sizeof(char),12U);}));})== 0  || ({struct _dyneither_ptr _tmp892=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp892,({const char*_tmp430="__initializes__";_tag_dyneither(_tmp430,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->tag=20U,_tmp431->f1=n;_tmp431;});else{
if(({struct _dyneither_ptr _tmp895=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp895,({const char*_tmp432="noliveunique";_tag_dyneither(_tmp432,sizeof(char),13U);}));})== 0  || ({struct _dyneither_ptr _tmp894=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp894,({const char*_tmp433="__noliveunique__";_tag_dyneither(_tmp433,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->tag=21U,_tmp434->f1=n;_tmp434;});else{
if(({struct _dyneither_ptr _tmp897=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp897,({const char*_tmp435="consume";_tag_dyneither(_tmp435,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp896=(struct _dyneither_ptr)_tmp41E;Cyc_zstrcmp(_tmp896,({const char*_tmp436="__consume__";_tag_dyneither(_tmp436,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=22U,_tmp437->f1=n;_tmp437;});else{
# 1583
({void*_tmp438=0U;({unsigned int _tmp899=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp898=({const char*_tmp439="unrecognized attribute";_tag_dyneither(_tmp439,sizeof(char),23U);});Cyc_Warn_err(_tmp899,_tmp898,_tag_dyneither(_tmp438,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1587
yyval=Cyc_YY46(a);
# 1589
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1591
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1593
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1590 "parse.y"
struct _dyneither_ptr _tmp43A=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp43B=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp43C=({unsigned int _tmp89A=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmp89A,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp43D=({unsigned int _tmp89B=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmp89B,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _dyneither_ptr _tmp89D=(struct _dyneither_ptr)_tmp43A;Cyc_zstrcmp(_tmp89D,({const char*_tmp43E="format";_tag_dyneither(_tmp43E,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp89C=(struct _dyneither_ptr)_tmp43A;Cyc_zstrcmp(_tmp89C,({const char*_tmp43F="__format__";_tag_dyneither(_tmp43F,sizeof(char),11U);}));})== 0){
if(({struct _dyneither_ptr _tmp89F=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp89F,({const char*_tmp440="printf";_tag_dyneither(_tmp440,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp89E=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp89E,({const char*_tmp441="__printf__";_tag_dyneither(_tmp441,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=19U,_tmp442->f1=Cyc_Absyn_Printf_ft,_tmp442->f2=(int)_tmp43C,_tmp442->f3=(int)_tmp43D;_tmp442;});else{
if(({struct _dyneither_ptr _tmp8A1=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp8A1,({const char*_tmp443="scanf";_tag_dyneither(_tmp443,sizeof(char),6U);}));})== 0  || ({struct _dyneither_ptr _tmp8A0=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp8A0,({const char*_tmp444="__scanf__";_tag_dyneither(_tmp444,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=19U,_tmp445->f1=Cyc_Absyn_Scanf_ft,_tmp445->f2=(int)_tmp43C,_tmp445->f3=(int)_tmp43D;_tmp445;});else{
# 1601
({void*_tmp446=0U;({unsigned int _tmp8A3=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8A2=({const char*_tmp447="unrecognized format type";_tag_dyneither(_tmp447,sizeof(char),25U);});Cyc_Warn_err(_tmp8A3,_tmp8A2,_tag_dyneither(_tmp446,sizeof(void*),0U));});});}}}else{
# 1603
({void*_tmp448=0U;({unsigned int _tmp8A5=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8A4=({const char*_tmp449="unrecognized attribute";_tag_dyneither(_tmp449,sizeof(char),23U);});Cyc_Warn_err(_tmp8A5,_tmp8A4,_tag_dyneither(_tmp448,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1606
goto _LL0;}case 74U: _LL8F: _LL90: {
# 1608
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1610
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1615 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 75U: _LL91: _LL92: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1617 "parse.y"
yyval=Cyc_YY21(({void*_tmp8A7=({struct _tuple0*_tmp8A6=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_typedef_type(_tmp8A6,Cyc_yyget_YY40(yyyyvsp[1]),0,0);});Cyc_Parse_type_spec(_tmp8A7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 76U: _LL93: _LL94: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1621 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 77U: _LL95: _LL96: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1622 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 78U: _LL97: _LL98: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1623 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 79U: _LL99: _LL9A: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1624 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 80U: _LL9B: _LL9C: {
# 1627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1625 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 81U: _LL9D: _LL9E: {
# 1628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1626 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1627 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1628 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 84U: _LLA3: _LLA4: {
# 1631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1629 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 85U: _LLA5: _LLA6: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1630 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 86U: _LLA7: _LLA8: {
# 1633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1631 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 87U: _LLA9: _LLAA: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1634 "parse.y"
yyval=Cyc_YY21(({void*_tmp8A8=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8A8,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 88U: _LLAB: _LLAC: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1636 "parse.y"
yyval=Cyc_YY21(({void*_tmp8A9=Cyc_Absyn_builtin_type(({const char*_tmp44A="__builtin_va_list";_tag_dyneither(_tmp44A,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp8A9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 89U: _LLAD: _LLAE: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1638 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 90U: _LLAF: _LLB0: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1640 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AA=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmp8AA,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 91U: _LLB1: _LLB2: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1642 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AB=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8AB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 92U: _LLB3: _LLB4: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1644 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AC=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);Cyc_Parse_type_spec(_tmp8AC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 93U: _LLB5: _LLB6: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1646 "parse.y"
yyval=Cyc_YY21(({void*_tmp8AF=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=6U,({struct Cyc_List_List*_tmp8AE=({unsigned int _tmp8AD=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8AD,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});
# 1646
_tmp44B->f1=_tmp8AE;});_tmp44B;});Cyc_Parse_type_spec(_tmp8AF,(unsigned int)(yyyylsp[0]).first_line);}));
# 1649
goto _LL0;}case 94U: _LLB7: _LLB8: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1650 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B0=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8B0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 95U: _LLB9: _LLBA: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1652 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B1=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp8B1,(unsigned int)(yyyylsp[0]).first_line);}));
# 1654
goto _LL0;}case 96U: _LLBB: _LLBC: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1655 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B2=Cyc_Absyn_tag_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8B2,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 97U: _LLBD: _LLBE: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1657 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B3=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp8B3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 98U: _LLBF: _LLC0: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1659 "parse.y"
yyval=Cyc_YY21(({void*_tmp8B4=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8B4,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 99U: _LLC1: _LLC2: {
# 1662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY43(({struct _dyneither_ptr _tmp8B5=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmp8B5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 100U: _LLC3: _LLC4: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1669 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6F9;_tmp6F9.print_const=1,_tmp6F9.q_volatile=0,_tmp6F9.q_restrict=0,_tmp6F9.real_const=1,_tmp6F9.loc=loc;_tmp6F9;}));
goto _LL0;}case 101U: _LLC5: _LLC6: {
# 1673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1671 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6FA;_tmp6FA.print_const=0,_tmp6FA.q_volatile=1,_tmp6FA.q_restrict=0,_tmp6FA.real_const=0,_tmp6FA.loc=0U;_tmp6FA;}));
goto _LL0;}case 102U: _LLC7: _LLC8: {
# 1674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1672 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp6FB;_tmp6FB.print_const=0,_tmp6FB.q_volatile=0,_tmp6FB.q_restrict=1,_tmp6FB.real_const=0,_tmp6FB.loc=0U;_tmp6FB;}));
goto _LL0;}case 103U: _LLC9: _LLCA: {
# 1675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
struct Cyc_Absyn_TypeDecl*_tmp44C=({struct Cyc_Absyn_TypeDecl*_tmp451=_cycalloc(sizeof(*_tmp451));({void*_tmp8BA=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8B9=({struct Cyc_Absyn_Enumdecl*_tmp44F=_cycalloc(sizeof(*_tmp44F));_tmp44F->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8B8=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp44F->name=_tmp8B8;}),({struct Cyc_Core_Opt*_tmp8B7=({struct Cyc_Core_Opt*_tmp44E=_cycalloc(sizeof(*_tmp44E));({struct Cyc_List_List*_tmp8B6=Cyc_yyget_YY48(yyyyvsp[3]);_tmp44E->v=_tmp8B6;});_tmp44E;});_tmp44F->fields=_tmp8B7;});_tmp44F;});_tmp450->f1=_tmp8B9;});_tmp450;});_tmp451->r=_tmp8BA;}),_tmp451->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp451;});
# 1680
yyval=Cyc_YY21(({void*_tmp8BB=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->tag=10U,_tmp44D->f1=_tmp44C,_tmp44D->f2=0;_tmp44D;});Cyc_Parse_type_spec(_tmp8BB,(unsigned int)(yyyylsp[0]).first_line);}));
# 1682
goto _LL0;}case 104U: _LLCB: _LLCC: {
# 1684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1683 "parse.y"
yyval=Cyc_YY21(({void*_tmp8BC=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(yyyyvsp[1]),0);Cyc_Parse_type_spec(_tmp8BC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 105U: _LLCD: _LLCE: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1685 "parse.y"
yyval=Cyc_YY21(({void*_tmp8BD=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8BD,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 106U: _LLCF: _LLD0: {
# 1688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp452=_cycalloc(sizeof(*_tmp452));({struct _tuple0*_tmp8BE=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp452->name=_tmp8BE;}),_tmp452->tag=0,_tmp452->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp452;}));
goto _LL0;}case 107U: _LLD1: _LLD2: {
# 1694
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1696
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1693 "parse.y"
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp453=_cycalloc(sizeof(*_tmp453));({struct _tuple0*_tmp8C0=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp453->name=_tmp8C0;}),({struct Cyc_Absyn_Exp*_tmp8BF=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp453->tag=_tmp8BF;}),_tmp453->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp453;}));
goto _LL0;}case 108U: _LLD3: _LLD4: {
# 1696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1697 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp454=_cycalloc(sizeof(*_tmp454));({struct Cyc_Absyn_Enumfield*_tmp8C1=Cyc_yyget_YY47(yyyyvsp[0]);_tmp454->hd=_tmp8C1;}),_tmp454->tl=0;_tmp454;}));
goto _LL0;}case 109U: _LLD5: _LLD6: {
# 1700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1698 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp455=_cycalloc(sizeof(*_tmp455));({struct Cyc_Absyn_Enumfield*_tmp8C2=Cyc_yyget_YY47(yyyyvsp[0]);_tmp455->hd=_tmp8C2;}),_tmp455->tl=0;_tmp455;}));
goto _LL0;}case 110U: _LLD7: _LLD8: {
# 1701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1699 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp456=_cycalloc(sizeof(*_tmp456));({struct Cyc_Absyn_Enumfield*_tmp8C4=Cyc_yyget_YY47(yyyyvsp[0]);_tmp456->hd=_tmp8C4;}),({struct Cyc_List_List*_tmp8C3=Cyc_yyget_YY48(yyyyvsp[2]);_tmp456->tl=_tmp8C3;});_tmp456;}));
goto _LL0;}case 111U: _LLD9: _LLDA: {
# 1702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
yyval=Cyc_YY21(({void*_tmp8C7=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->tag=7U,({enum Cyc_Absyn_AggrKind _tmp8C6=Cyc_yyget_YY22(yyyyvsp[0]);_tmp457->f1=_tmp8C6;}),({struct Cyc_List_List*_tmp8C5=Cyc_yyget_YY24(yyyyvsp[2]);_tmp457->f2=_tmp8C5;});_tmp457;});Cyc_Parse_type_spec(_tmp8C7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 112U: _LLDB: _LLDC: {
# 1708
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 1710
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 1711 "parse.y"
struct Cyc_List_List*_tmp458=({unsigned int _tmp8C8=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8C8,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp459=({unsigned int _tmp8C9=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8C9,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp45A=({enum Cyc_Absyn_AggrKind _tmp8CF=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp8CE=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmp8CD=_tmp458;struct Cyc_Absyn_AggrdeclImpl*_tmp8CC=({
struct Cyc_List_List*_tmp8CB=_tmp459;struct Cyc_List_List*_tmp8CA=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmp8CB,_tmp8CA,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1713
Cyc_Absyn_aggr_tdecl(_tmp8CF,Cyc_Absyn_Public,_tmp8CE,_tmp8CD,_tmp8CC,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1716
yyval=Cyc_YY21(({void*_tmp8D0=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->tag=10U,_tmp45B->f1=_tmp45A,_tmp45B->f2=0;_tmp45B;});Cyc_Parse_type_spec(_tmp8D0,(unsigned int)(yyyylsp[0]).first_line);}));
# 1718
goto _LL0;}case 113U: _LLDD: _LLDE: {
# 1720
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1722
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1722 "parse.y"
struct Cyc_List_List*_tmp45C=({unsigned int _tmp8D1=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8D1,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp45D=({unsigned int _tmp8D2=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8D2,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp45E=({enum Cyc_Absyn_AggrKind _tmp8D8=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmp8D7=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp8D6=_tmp45C;struct Cyc_Absyn_AggrdeclImpl*_tmp8D5=({
struct Cyc_List_List*_tmp8D4=_tmp45D;struct Cyc_List_List*_tmp8D3=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmp8D4,_tmp8D3,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1724
Cyc_Absyn_aggr_tdecl(_tmp8D8,Cyc_Absyn_Public,_tmp8D7,_tmp8D6,_tmp8D5,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1727
yyval=Cyc_YY21(({void*_tmp8D9=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->tag=10U,_tmp45F->f1=_tmp45E,_tmp45F->f2=0;_tmp45F;});Cyc_Parse_type_spec(_tmp8D9,(unsigned int)(yyyylsp[0]).first_line);}));
# 1729
goto _LL0;}case 114U: _LLDF: _LLE0: {
# 1731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1730 "parse.y"
yyval=Cyc_YY21(({void*_tmp8DD=({union Cyc_Absyn_AggrInfo _tmp8DC=({enum Cyc_Absyn_AggrKind _tmp8DB=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp8DA=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmp8DB,_tmp8DA,({struct Cyc_Core_Opt*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->v=(void*)1;_tmp460;}));});Cyc_Absyn_aggr_type(_tmp8DC,Cyc_yyget_YY40(yyyyvsp[3]));});Cyc_Parse_type_spec(_tmp8DD,(unsigned int)(yyyylsp[0]).first_line);}));
# 1733
goto _LL0;}case 115U: _LLE1: _LLE2: {
# 1735
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1734 "parse.y"
yyval=Cyc_YY21(({void*_tmp8E0=({union Cyc_Absyn_AggrInfo _tmp8DF=({enum Cyc_Absyn_AggrKind _tmp8DE=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmp8DE,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});Cyc_Absyn_aggr_type(_tmp8DF,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp8E0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 116U: _LLE3: _LLE4:
# 1737
 yyval=Cyc_YY40(0);
goto _LL0;case 117U: _LLE5: _LLE6: {
# 1740
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1742
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1741 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 118U: _LLE7: _LLE8: {
# 1744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1745 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;}case 119U: _LLE9: _LLEA: {
# 1748
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1750
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1746 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;}case 120U: _LLEB: _LLEC:
# 1749
 yyval=Cyc_YY24(0);
goto _LL0;case 121U: _LLED: _LLEE: {
# 1752
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1754
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1753 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp461=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp461 != 0;_tmp461=_tmp461->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp461->hd,decls);}}{
# 1757
struct Cyc_List_List*_tmp462=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp462 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp462,decls);
yyval=Cyc_YY24(decls);
# 1762
goto _LL0;};}case 122U: _LLEF: _LLF0: {
# 1764
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1766
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp463=_cycalloc(sizeof(*_tmp463));({struct Cyc_List_List*_tmp8E1=Cyc_yyget_YY24(yyyyvsp[0]);_tmp463->hd=_tmp8E1;}),_tmp463->tl=0;_tmp463;}));
goto _LL0;}case 123U: _LLF1: _LLF2: {
# 1770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1769 "parse.y"
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp464=_cycalloc(sizeof(*_tmp464));({struct Cyc_List_List*_tmp8E3=Cyc_yyget_YY24(yyyyvsp[1]);_tmp464->hd=_tmp8E3;}),({struct Cyc_List_List*_tmp8E2=Cyc_yyget_YY25(yyyyvsp[0]);_tmp464->tl=_tmp8E2;});_tmp464;}));
goto _LL0;}case 124U: _LLF3: _LLF4: {
# 1772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1773 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
goto _LL0;}case 125U: _LLF5: _LLF6: {
# 1776
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1778
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY19(({struct _tuple12*_tmp465=_region_malloc(yyr,sizeof(*_tmp465));_tmp465->tl=0,({struct _tuple11 _tmp8E4=Cyc_yyget_YY18(yyyyvsp[0]);_tmp465->hd=_tmp8E4;});_tmp465;}));
goto _LL0;}case 126U: _LLF7: _LLF8: {
# 1782
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1784
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1781 "parse.y"
yyval=Cyc_YY19(({struct _tuple12*_tmp466=_region_malloc(yyr,sizeof(*_tmp466));({struct _tuple12*_tmp8E6=Cyc_yyget_YY19(yyyyvsp[0]);_tmp466->tl=_tmp8E6;}),({struct _tuple11 _tmp8E5=Cyc_yyget_YY18(yyyyvsp[2]);_tmp466->hd=_tmp8E5;});_tmp466;}));
goto _LL0;}case 127U: _LLF9: _LLFA: {
# 1784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1786 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp6FC;({struct Cyc_Parse_Declarator _tmp8E7=Cyc_yyget_YY27(yyyyvsp[0]);_tmp6FC.f1=_tmp8E7;}),_tmp6FC.f2=0;_tmp6FC;}));
goto _LL0;}case 128U: _LLFB: _LLFC: {
# 1789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1788 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp6FD;({struct Cyc_Parse_Declarator _tmp8E9=Cyc_yyget_YY27(yyyyvsp[0]);_tmp6FD.f1=_tmp8E9;}),({struct Cyc_Absyn_Exp*_tmp8E8=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp6FD.f2=_tmp8E8;});_tmp6FD;}));
goto _LL0;}case 129U: _LLFD: _LLFE: {
# 1791
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1794 "parse.y"
struct _RegionHandle _tmp467=_new_region("temp");struct _RegionHandle*temp=& _tmp467;_push_region(temp);
{struct _tuple25 _tmp468=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp469=_tmp468;struct Cyc_Absyn_Tqual _tmp477;struct Cyc_Parse_Type_specifier _tmp476;struct Cyc_List_List*_tmp475;_LL441: _tmp477=_tmp469.f1;_tmp476=_tmp469.f2;_tmp475=_tmp469.f3;_LL442:;
if(_tmp477.loc == (unsigned int)0)_tmp477.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp46A=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp46A != 0;_tmp46A=_tmp46A->tl){
struct _tuple24*_tmp46B=(struct _tuple24*)_tmp46A->hd;struct _tuple24*_tmp46C=_tmp46B;struct Cyc_Parse_Declarator _tmp472;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_Absyn_Exp*_tmp470;_LL444: _tmp472=_tmp46C->f1;_tmp471=_tmp46C->f2;_tmp470=_tmp46C->f3;_LL445:;
decls=({struct _tuple10*_tmp46D=_region_malloc(temp,sizeof(*_tmp46D));_tmp46D->tl=decls,_tmp46D->hd=_tmp472;_tmp46D;});
widths_and_reqs=({struct Cyc_List_List*_tmp46F=_region_malloc(temp,sizeof(*_tmp46F));
({struct _tuple16*_tmp8EA=({struct _tuple16*_tmp46E=_region_malloc(temp,sizeof(*_tmp46E));_tmp46E->f1=_tmp471,_tmp46E->f2=_tmp470;_tmp46E;});_tmp46F->hd=_tmp8EA;}),_tmp46F->tl=widths_and_reqs;_tmp46F;});}}
# 1805
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp473=Cyc_Parse_speclist2typ(_tmp476,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp474=({struct _RegionHandle*_tmp8ED=temp;struct _RegionHandle*_tmp8EC=temp;struct Cyc_List_List*_tmp8EB=
Cyc_Parse_apply_tmss(temp,_tmp477,_tmp473,decls,_tmp475);
# 1808
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp8ED,_tmp8EC,_tmp8EB,widths_and_reqs);});
# 1811
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp474));
# 1813
_npop_handler(0U);goto _LL0;};};}
# 1795
;_pop_region(temp);}case 130U: _LLFF: _LL100: {
# 1815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1821 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp6FE;({struct Cyc_Absyn_Tqual _tmp8EF=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6FE.f1=_tmp8EF;}),({struct Cyc_Parse_Type_specifier _tmp8EE=Cyc_yyget_YY21(yyyyvsp[0]);_tmp6FE.f2=_tmp8EE;}),_tmp6FE.f3=0;_tmp6FE;}));
goto _LL0;}case 131U: _LL101: _LL102: {
# 1824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1823 "parse.y"
struct _tuple25 _tmp478=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp6FF;_tmp6FF.f1=_tmp478.f1,({struct Cyc_Parse_Type_specifier _tmp8F2=({unsigned int _tmp8F1=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp8F0=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp8F1,_tmp8F0,_tmp478.f2);});_tmp6FF.f2=_tmp8F2;}),_tmp6FF.f3=_tmp478.f3;_tmp6FF;}));
goto _LL0;}case 132U: _LL103: _LL104: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1825 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp700;({struct Cyc_Absyn_Tqual _tmp8F4=Cyc_yyget_YY23(yyyyvsp[0]);_tmp700.f1=_tmp8F4;}),({struct Cyc_Parse_Type_specifier _tmp8F3=Cyc_Parse_empty_spec(0U);_tmp700.f2=_tmp8F3;}),_tmp700.f3=0;_tmp700;}));
goto _LL0;}case 133U: _LL105: _LL106: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1827 "parse.y"
struct _tuple25 _tmp479=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp701;({struct Cyc_Absyn_Tqual _tmp8F6=({struct Cyc_Absyn_Tqual _tmp8F5=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp8F5,_tmp479.f1);});_tmp701.f1=_tmp8F6;}),_tmp701.f2=_tmp479.f2,_tmp701.f3=_tmp479.f3;_tmp701;}));
goto _LL0;}case 134U: _LL107: _LL108: {
# 1831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1830 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp702;({struct Cyc_Absyn_Tqual _tmp8F9=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp702.f1=_tmp8F9;}),({struct Cyc_Parse_Type_specifier _tmp8F8=Cyc_Parse_empty_spec(0U);_tmp702.f2=_tmp8F8;}),({struct Cyc_List_List*_tmp8F7=Cyc_yyget_YY45(yyyyvsp[0]);_tmp702.f3=_tmp8F7;});_tmp702;}));
goto _LL0;}case 135U: _LL109: _LL10A: {
# 1833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1832 "parse.y"
struct _tuple25 _tmp47A=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp703;_tmp703.f1=_tmp47A.f1,_tmp703.f2=_tmp47A.f2,({struct Cyc_List_List*_tmp8FB=({struct Cyc_List_List*_tmp8FA=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp8FA,_tmp47A.f3);});_tmp703.f3=_tmp8FB;});_tmp703;}));
goto _LL0;}case 136U: _LL10B: _LL10C: {
# 1835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY35(({struct _tuple25 _tmp704;({struct Cyc_Absyn_Tqual _tmp8FD=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp704.f1=_tmp8FD;}),({struct Cyc_Parse_Type_specifier _tmp8FC=Cyc_yyget_YY21(yyyyvsp[0]);_tmp704.f2=_tmp8FC;}),_tmp704.f3=0;_tmp704;}));
goto _LL0;}case 137U: _LL10D: _LL10E: {
# 1841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1840 "parse.y"
struct _tuple25 _tmp47B=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp705;_tmp705.f1=_tmp47B.f1,({struct Cyc_Parse_Type_specifier _tmp900=({unsigned int _tmp8FF=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp8FE=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp8FF,_tmp8FE,_tmp47B.f2);});_tmp705.f2=_tmp900;}),_tmp705.f3=_tmp47B.f3;_tmp705;}));
goto _LL0;}case 138U: _LL10F: _LL110: {
# 1843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1842 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp706;({struct Cyc_Absyn_Tqual _tmp902=Cyc_yyget_YY23(yyyyvsp[0]);_tmp706.f1=_tmp902;}),({struct Cyc_Parse_Type_specifier _tmp901=Cyc_Parse_empty_spec(0U);_tmp706.f2=_tmp901;}),_tmp706.f3=0;_tmp706;}));
goto _LL0;}case 139U: _LL111: _LL112: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1844 "parse.y"
struct _tuple25 _tmp47C=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp707;({struct Cyc_Absyn_Tqual _tmp904=({struct Cyc_Absyn_Tqual _tmp903=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp903,_tmp47C.f1);});_tmp707.f1=_tmp904;}),_tmp707.f2=_tmp47C.f2,_tmp707.f3=_tmp47C.f3;_tmp707;}));
goto _LL0;}case 140U: _LL113: _LL114: {
# 1848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1847 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp708;({struct Cyc_Absyn_Tqual _tmp907=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp708.f1=_tmp907;}),({struct Cyc_Parse_Type_specifier _tmp906=Cyc_Parse_empty_spec(0U);_tmp708.f2=_tmp906;}),({struct Cyc_List_List*_tmp905=Cyc_yyget_YY45(yyyyvsp[0]);_tmp708.f3=_tmp905;});_tmp708;}));
goto _LL0;}case 141U: _LL115: _LL116: {
# 1850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1849 "parse.y"
struct _tuple25 _tmp47D=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp709;_tmp709.f1=_tmp47D.f1,_tmp709.f2=_tmp47D.f2,({struct Cyc_List_List*_tmp909=({struct Cyc_List_List*_tmp908=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp908,_tmp47D.f3);});_tmp709.f3=_tmp909;});_tmp709;}));
goto _LL0;}case 142U: _LL117: _LL118: {
# 1852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1853 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
goto _LL0;}case 143U: _LL119: _LL11A: {
# 1856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp47E=_region_malloc(yyr,sizeof(*_tmp47E));({struct _tuple24*_tmp90A=Cyc_yyget_YY28(yyyyvsp[0]);_tmp47E->hd=_tmp90A;}),_tmp47E->tl=0;_tmp47E;}));
goto _LL0;}case 144U: _LL11B: _LL11C: {
# 1862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1861 "parse.y"
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp47F=_region_malloc(yyr,sizeof(*_tmp47F));({struct _tuple24*_tmp90C=Cyc_yyget_YY28(yyyyvsp[2]);_tmp47F->hd=_tmp90C;}),({struct Cyc_List_List*_tmp90B=Cyc_yyget_YY29(yyyyvsp[0]);_tmp47F->tl=_tmp90B;});_tmp47F;}));
goto _LL0;}case 145U: _LL11D: _LL11E: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1866 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp480=_region_malloc(yyr,sizeof(*_tmp480));({struct Cyc_Parse_Declarator _tmp90E=Cyc_yyget_YY27(yyyyvsp[0]);_tmp480->f1=_tmp90E;}),_tmp480->f2=0,({struct Cyc_Absyn_Exp*_tmp90D=Cyc_yyget_YY55(yyyyvsp[1]);_tmp480->f3=_tmp90D;});_tmp480;}));
goto _LL0;}case 146U: _LL11F: _LL120: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1870 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp484=_region_malloc(yyr,sizeof(*_tmp484));({struct _tuple0*_tmp913=({struct _tuple0*_tmp483=_cycalloc(sizeof(*_tmp483));({union Cyc_Absyn_Nmspace _tmp912=Cyc_Absyn_Rel_n(0);_tmp483->f1=_tmp912;}),({struct _dyneither_ptr*_tmp911=({struct _dyneither_ptr*_tmp482=_cycalloc(sizeof(*_tmp482));({struct _dyneither_ptr _tmp910=({const char*_tmp481="";_tag_dyneither(_tmp481,sizeof(char),1U);});*_tmp482=_tmp910;});_tmp482;});_tmp483->f2=_tmp911;});_tmp483;});(_tmp484->f1).id=_tmp913;}),(_tmp484->f1).varloc=0U,(_tmp484->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp90F=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp484->f2=_tmp90F;}),_tmp484->f3=0;_tmp484;}));
# 1872
goto _LL0;}case 147U: _LL121: _LL122:
# 1876
 yyval=Cyc_YY28(({struct _tuple24*_tmp488=_region_malloc(yyr,sizeof(*_tmp488));({struct _tuple0*_tmp917=({struct _tuple0*_tmp487=_cycalloc(sizeof(*_tmp487));({union Cyc_Absyn_Nmspace _tmp916=Cyc_Absyn_Rel_n(0);_tmp487->f1=_tmp916;}),({struct _dyneither_ptr*_tmp915=({struct _dyneither_ptr*_tmp486=_cycalloc(sizeof(*_tmp486));({struct _dyneither_ptr _tmp914=({const char*_tmp485="";_tag_dyneither(_tmp485,sizeof(char),1U);});*_tmp486=_tmp914;});_tmp486;});_tmp487->f2=_tmp915;});_tmp487;});(_tmp488->f1).id=_tmp917;}),(_tmp488->f1).varloc=0U,(_tmp488->f1).tms=0,_tmp488->f2=0,_tmp488->f3=0;_tmp488;}));
# 1878
goto _LL0;case 148U: _LL123: _LL124: {
# 1880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1878 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp489=_region_malloc(yyr,sizeof(*_tmp489));({struct Cyc_Parse_Declarator _tmp919=Cyc_yyget_YY27(yyyyvsp[0]);_tmp489->f1=_tmp919;}),({struct Cyc_Absyn_Exp*_tmp918=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp489->f2=_tmp918;}),_tmp489->f3=0;_tmp489;}));
goto _LL0;}case 149U: _LL125: _LL126: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1882 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL0;}case 150U: _LL127: _LL128:
# 1885
 yyval=Cyc_YY55(0);
goto _LL0;case 151U: _LL129: _LL12A: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1887 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL0;}case 152U: _LL12B: _LL12C:
# 1890
 yyval=Cyc_YY55(0);
goto _LL0;case 153U: _LL12D: _LL12E: {
# 1893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1894 "parse.y"
int _tmp48A=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp48B=({unsigned int _tmp91A=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp91A,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp48C=({struct _tuple0*_tmp91F=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp91E=_tmp48B;struct Cyc_Core_Opt*_tmp91D=({struct Cyc_Core_Opt*_tmp48E=_cycalloc(sizeof(*_tmp48E));({struct Cyc_List_List*_tmp91B=Cyc_yyget_YY34(yyyyvsp[4]);_tmp48E->v=_tmp91B;});_tmp48E;});int _tmp91C=_tmp48A;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp91F,_tmp91E,_tmp91D,_tmp91C,(unsigned int)(yyyylsp[0]).first_line);});
# 1898
yyval=Cyc_YY21(({void*_tmp920=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->tag=10U,_tmp48D->f1=_tmp48C,_tmp48D->f2=0;_tmp48D;});Cyc_Parse_type_spec(_tmp920,(unsigned int)(yyyylsp[0]).first_line);}));
# 1900
goto _LL0;}case 154U: _LL12F: _LL130: {
# 1902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1901 "parse.y"
int _tmp48F=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(({void*_tmp923=({union Cyc_Absyn_DatatypeInfo _tmp922=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp70A;({struct _tuple0*_tmp921=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp70A.name=_tmp921;}),_tmp70A.is_extensible=_tmp48F;_tmp70A;}));Cyc_Absyn_datatype_type(_tmp922,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp923,(unsigned int)(yyyylsp[0]).first_line);}));
# 1904
goto _LL0;}case 155U: _LL131: _LL132: {
# 1906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1905 "parse.y"
int _tmp490=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(({void*_tmp927=({union Cyc_Absyn_DatatypeFieldInfo _tmp926=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp70B;({struct _tuple0*_tmp925=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp70B.datatype_name=_tmp925;}),({struct _tuple0*_tmp924=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp70B.field_name=_tmp924;}),_tmp70B.is_extensible=_tmp490;_tmp70B;}));Cyc_Absyn_datatype_field_type(_tmp926,Cyc_yyget_YY40(yyyyvsp[4]));});Cyc_Parse_type_spec(_tmp927,(unsigned int)(yyyylsp[0]).first_line);}));
# 1908
goto _LL0;}case 156U: _LL133: _LL134: {
# 1910
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1911 "parse.y"
yyval=Cyc_YY31(0);
goto _LL0;}case 157U: _LL135: _LL136: {
# 1914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1912 "parse.y"
yyval=Cyc_YY31(1);
goto _LL0;}case 158U: _LL137: _LL138: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1916 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp491=_cycalloc(sizeof(*_tmp491));({struct Cyc_Absyn_Datatypefield*_tmp928=Cyc_yyget_YY33(yyyyvsp[0]);_tmp491->hd=_tmp928;}),_tmp491->tl=0;_tmp491;}));
goto _LL0;}case 159U: _LL139: _LL13A: {
# 1919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1917 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp492=_cycalloc(sizeof(*_tmp492));({struct Cyc_Absyn_Datatypefield*_tmp929=Cyc_yyget_YY33(yyyyvsp[0]);_tmp492->hd=_tmp929;}),_tmp492->tl=0;_tmp492;}));
goto _LL0;}case 160U: _LL13B: _LL13C: {
# 1920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1918 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp493=_cycalloc(sizeof(*_tmp493));({struct Cyc_Absyn_Datatypefield*_tmp92B=Cyc_yyget_YY33(yyyyvsp[0]);_tmp493->hd=_tmp92B;}),({struct Cyc_List_List*_tmp92A=Cyc_yyget_YY34(yyyyvsp[2]);_tmp493->tl=_tmp92A;});_tmp493;}));
goto _LL0;}case 161U: _LL13D: _LL13E: {
# 1921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1919 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp494=_cycalloc(sizeof(*_tmp494));({struct Cyc_Absyn_Datatypefield*_tmp92D=Cyc_yyget_YY33(yyyyvsp[0]);_tmp494->hd=_tmp92D;}),({struct Cyc_List_List*_tmp92C=Cyc_yyget_YY34(yyyyvsp[2]);_tmp494->tl=_tmp92C;});_tmp494;}));
goto _LL0;}case 162U: _LL13F: _LL140:
# 1922
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 163U: _LL141: _LL142: {
# 1925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1924 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;}case 164U: _LL143: _LL144: {
# 1927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1925 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;}case 165U: _LL145: _LL146: {
# 1928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1929 "parse.y"
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp495=_cycalloc(sizeof(*_tmp495));({struct _tuple0*_tmp92F=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp495->name=_tmp92F;}),_tmp495->typs=0,_tmp495->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp92E=Cyc_yyget_YY32(yyyyvsp[0]);_tmp495->sc=_tmp92E;});_tmp495;}));
goto _LL0;}case 166U: _LL147: _LL148: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1931 "parse.y"
struct Cyc_List_List*_tmp496=({unsigned int _tmp930=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp930,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp497=_cycalloc(sizeof(*_tmp497));({struct _tuple0*_tmp932=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp497->name=_tmp932;}),_tmp497->typs=_tmp496,_tmp497->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp931=Cyc_yyget_YY32(yyyyvsp[0]);_tmp497->sc=_tmp931;});_tmp497;}));
goto _LL0;}case 167U: _LL149: _LL14A: {
# 1935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1937 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 168U: _LL14B: _LL14C: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1939 "parse.y"
struct Cyc_Parse_Declarator _tmp498=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp70C;_tmp70C.id=_tmp498.id,_tmp70C.varloc=_tmp498.varloc,({struct Cyc_List_List*_tmp934=({struct Cyc_List_List*_tmp933=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp933,_tmp498.tms);});_tmp70C.tms=_tmp934;});_tmp70C;}));
goto _LL0;}case 169U: _LL14D: _LL14E: {
# 1943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 170U: _LL14F: _LL150: {
# 1949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1948 "parse.y"
struct Cyc_Parse_Declarator _tmp499=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp70D;_tmp70D.id=_tmp499.id,_tmp70D.varloc=_tmp499.varloc,({struct Cyc_List_List*_tmp936=({struct Cyc_List_List*_tmp935=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp935,_tmp499.tms);});_tmp70D.tms=_tmp936;});_tmp70D;}));
goto _LL0;}case 171U: _LL151: _LL152: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1954 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp70E;({struct _tuple0*_tmp937=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp70E.id=_tmp937;}),_tmp70E.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp70E.tms=0;_tmp70E;}));
goto _LL0;}case 172U: _LL153: _LL154: {
# 1957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1956 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 173U: _LL155: _LL156: {
# 1959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1960 "parse.y"
struct Cyc_Parse_Declarator _tmp49A=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmp93A=({struct Cyc_List_List*_tmp49C=_region_malloc(yyr,sizeof(*_tmp49C));({void*_tmp939=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp49B=_region_malloc(yyr,sizeof(*_tmp49B));_tmp49B->tag=5U,_tmp49B->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp938=Cyc_yyget_YY45(yyyyvsp[1]);_tmp49B->f2=_tmp938;});_tmp49B;});_tmp49C->hd=_tmp939;}),_tmp49C->tl=_tmp49A.tms;_tmp49C;});_tmp49A.tms=_tmp93A;});
yyval=yyyyvsp[2];
# 1964
goto _LL0;}case 174U: _LL157: _LL158: {
# 1966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1965 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp70F;({struct _tuple0*_tmp940=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp70F.id=_tmp940;}),({unsigned int _tmp93F=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp70F.varloc=_tmp93F;}),({struct Cyc_List_List*_tmp93E=({struct Cyc_List_List*_tmp49E=_region_malloc(yyr,sizeof(*_tmp49E));({void*_tmp93D=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp49D=_region_malloc(yyr,sizeof(*_tmp49D));_tmp49D->tag=0U,({void*_tmp93C=Cyc_yyget_YY51(yyyyvsp[3]);_tmp49D->f1=_tmp93C;}),_tmp49D->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp49D;});_tmp49E->hd=_tmp93D;}),({struct Cyc_List_List*_tmp93B=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp49E->tl=_tmp93B;});_tmp49E;});_tmp70F.tms=_tmp93E;});_tmp70F;}));
goto _LL0;}case 175U: _LL159: _LL15A: {
# 1968
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1970
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1967 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp710;({struct _tuple0*_tmp947=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp710.id=_tmp947;}),({unsigned int _tmp946=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp710.varloc=_tmp946;}),({
struct Cyc_List_List*_tmp945=({struct Cyc_List_List*_tmp4A0=_region_malloc(yyr,sizeof(*_tmp4A0));({void*_tmp944=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp49F=_region_malloc(yyr,sizeof(*_tmp49F));_tmp49F->tag=1U,({struct Cyc_Absyn_Exp*_tmp943=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp49F->f1=_tmp943;}),({void*_tmp942=Cyc_yyget_YY51(yyyyvsp[4]);_tmp49F->f2=_tmp942;}),_tmp49F->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp49F;});_tmp4A0->hd=_tmp944;}),({struct Cyc_List_List*_tmp941=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4A0->tl=_tmp941;});_tmp4A0;});_tmp710.tms=_tmp945;});_tmp710;}));
goto _LL0;}case 176U: _LL15B: _LL15C: {
# 1971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1970 "parse.y"
struct _tuple26*_tmp4A1=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4A2=_tmp4A1;struct Cyc_List_List*_tmp4AC;int _tmp4AB;struct Cyc_Absyn_VarargInfo*_tmp4AA;void*_tmp4A9;struct Cyc_List_List*_tmp4A8;_LL447: _tmp4AC=_tmp4A2->f1;_tmp4AB=_tmp4A2->f2;_tmp4AA=_tmp4A2->f3;_tmp4A9=_tmp4A2->f4;_tmp4A8=_tmp4A2->f5;_LL448:;{
struct Cyc_Absyn_Exp*_tmp4A3=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4A4=Cyc_yyget_YY55(yyyyvsp[5]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp711;({struct _tuple0*_tmp94D=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp711.id=_tmp94D;}),({unsigned int _tmp94C=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp711.varloc=_tmp94C;}),({struct Cyc_List_List*_tmp94B=({struct Cyc_List_List*_tmp4A7=_region_malloc(yyr,sizeof(*_tmp4A7));({void*_tmp94A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4A6=_region_malloc(yyr,sizeof(*_tmp4A6));_tmp4A6->tag=3U,({void*_tmp949=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4A5=_region_malloc(yyr,sizeof(*_tmp4A5));_tmp4A5->tag=1U,_tmp4A5->f1=_tmp4AC,_tmp4A5->f2=_tmp4AB,_tmp4A5->f3=_tmp4AA,_tmp4A5->f4=_tmp4A9,_tmp4A5->f5=_tmp4A8,_tmp4A5->f6=_tmp4A3,_tmp4A5->f7=_tmp4A4;_tmp4A5;});_tmp4A6->f1=_tmp949;});_tmp4A6;});_tmp4A7->hd=_tmp94A;}),({struct Cyc_List_List*_tmp948=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4A7->tl=_tmp948;});_tmp4A7;});_tmp711.tms=_tmp94B;});_tmp711;}));
# 1975
goto _LL0;};}case 177U: _LL15D: _LL15E: {
# 1977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1976 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp712;({struct _tuple0*_tmp957=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp712.id=_tmp957;}),({unsigned int _tmp956=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp712.varloc=_tmp956;}),({
struct Cyc_List_List*_tmp955=({struct Cyc_List_List*_tmp4AF=_region_malloc(yyr,sizeof(*_tmp4AF));({void*_tmp954=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4AE=_region_malloc(yyr,sizeof(*_tmp4AE));_tmp4AE->tag=3U,({void*_tmp953=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4AD=_region_malloc(yyr,sizeof(*_tmp4AD));_tmp4AD->tag=1U,_tmp4AD->f1=0,_tmp4AD->f2=0,_tmp4AD->f3=0,({
# 1979
void*_tmp952=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4AD->f4=_tmp952;}),({struct Cyc_List_List*_tmp951=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4AD->f5=_tmp951;}),({struct Cyc_Absyn_Exp*_tmp950=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4AD->f6=_tmp950;}),({struct Cyc_Absyn_Exp*_tmp94F=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4AD->f7=_tmp94F;});_tmp4AD;});
# 1977
_tmp4AE->f1=_tmp953;});_tmp4AE;});_tmp4AF->hd=_tmp954;}),({
# 1980
struct Cyc_List_List*_tmp94E=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4AF->tl=_tmp94E;});_tmp4AF;});
# 1977
_tmp712.tms=_tmp955;});_tmp712;}));
# 1982
goto _LL0;}case 178U: _LL15F: _LL160: {
# 1984
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1983 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp713;({struct _tuple0*_tmp95E=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp713.id=_tmp95E;}),({unsigned int _tmp95D=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp713.varloc=_tmp95D;}),({struct Cyc_List_List*_tmp95C=({struct Cyc_List_List*_tmp4B2=_region_malloc(yyr,sizeof(*_tmp4B2));({void*_tmp95B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4B1=_region_malloc(yyr,sizeof(*_tmp4B1));_tmp4B1->tag=3U,({void*_tmp95A=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4B0=_region_malloc(yyr,sizeof(*_tmp4B0));_tmp4B0->tag=0U,({struct Cyc_List_List*_tmp959=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4B0->f1=_tmp959;}),_tmp4B0->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4B0;});_tmp4B1->f1=_tmp95A;});_tmp4B1;});_tmp4B2->hd=_tmp95B;}),({struct Cyc_List_List*_tmp958=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B2->tl=_tmp958;});_tmp4B2;});_tmp713.tms=_tmp95C;});_tmp713;}));
goto _LL0;}case 179U: _LL161: _LL162: {
# 1986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1986 "parse.y"
struct Cyc_List_List*_tmp4B3=({unsigned int _tmp95F=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp95F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp714;({struct _tuple0*_tmp964=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp714.id=_tmp964;}),({unsigned int _tmp963=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp714.varloc=_tmp963;}),({struct Cyc_List_List*_tmp962=({struct Cyc_List_List*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));({void*_tmp961=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4B4=_region_malloc(yyr,sizeof(*_tmp4B4));_tmp4B4->tag=4U,_tmp4B4->f1=_tmp4B3,_tmp4B4->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4B4->f3=0;_tmp4B4;});_tmp4B5->hd=_tmp961;}),({struct Cyc_List_List*_tmp960=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B5->tl=_tmp960;});_tmp4B5;});_tmp714.tms=_tmp962;});_tmp714;}));
# 1989
goto _LL0;}case 180U: _LL163: _LL164: {
# 1991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1990 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp715;({struct _tuple0*_tmp96A=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp715.id=_tmp96A;}),({unsigned int _tmp969=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp715.varloc=_tmp969;}),({struct Cyc_List_List*_tmp968=({struct Cyc_List_List*_tmp4B7=_region_malloc(yyr,sizeof(*_tmp4B7));({void*_tmp967=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B6=_region_malloc(yyr,sizeof(*_tmp4B6));_tmp4B6->tag=5U,_tmp4B6->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp966=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4B6->f2=_tmp966;});_tmp4B6;});_tmp4B7->hd=_tmp967;}),({
struct Cyc_List_List*_tmp965=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B7->tl=_tmp965;});_tmp4B7;});
# 1990
_tmp715.tms=_tmp968;});_tmp715;}));
# 1993
goto _LL0;}case 181U: _LL165: _LL166: {
# 1995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp716;({struct _tuple0*_tmp96B=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp716.id=_tmp96B;}),_tmp716.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp716.tms=0;_tmp716;}));
goto _LL0;}case 182U: _LL167: _LL168: {
# 2001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2000 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp717;({struct _tuple0*_tmp96C=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp717.id=_tmp96C;}),_tmp717.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp717.tms=0;_tmp717;}));
goto _LL0;}case 183U: _LL169: _LL16A: {
# 2003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2002 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 184U: _LL16B: _LL16C: {
# 2005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2006 "parse.y"
struct Cyc_Parse_Declarator _tmp4B8=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmp96F=({struct Cyc_List_List*_tmp4BA=_region_malloc(yyr,sizeof(*_tmp4BA));({void*_tmp96E=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B9=_region_malloc(yyr,sizeof(*_tmp4B9));_tmp4B9->tag=5U,_tmp4B9->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp96D=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4B9->f2=_tmp96D;});_tmp4B9;});_tmp4BA->hd=_tmp96E;}),_tmp4BA->tl=_tmp4B8.tms;_tmp4BA;});_tmp4B8.tms=_tmp96F;});
yyval=yyyyvsp[2];
# 2010
goto _LL0;}case 185U: _LL16D: _LL16E: {
# 2012
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2014
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2011 "parse.y"
struct Cyc_Parse_Declarator _tmp4BB=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp718;_tmp718.id=_tmp4BB.id,_tmp718.varloc=_tmp4BB.varloc,({
struct Cyc_List_List*_tmp972=({struct Cyc_List_List*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));({void*_tmp971=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4BC=_region_malloc(yyr,sizeof(*_tmp4BC));_tmp4BC->tag=0U,({void*_tmp970=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4BC->f1=_tmp970;}),_tmp4BC->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4BC;});_tmp4BD->hd=_tmp971;}),_tmp4BD->tl=_tmp4BB.tms;_tmp4BD;});_tmp718.tms=_tmp972;});_tmp718;}));
goto _LL0;}case 186U: _LL16F: _LL170: {
# 2016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2015 "parse.y"
struct Cyc_Parse_Declarator _tmp4BE=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp719;_tmp719.id=_tmp4BE.id,_tmp719.varloc=_tmp4BE.varloc,({
struct Cyc_List_List*_tmp976=({struct Cyc_List_List*_tmp4C0=_region_malloc(yyr,sizeof(*_tmp4C0));({void*_tmp975=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4BF=_region_malloc(yyr,sizeof(*_tmp4BF));_tmp4BF->tag=1U,({struct Cyc_Absyn_Exp*_tmp974=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4BF->f1=_tmp974;}),({void*_tmp973=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4BF->f2=_tmp973;}),_tmp4BF->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4BF;});_tmp4C0->hd=_tmp975;}),_tmp4C0->tl=_tmp4BE.tms;_tmp4C0;});_tmp719.tms=_tmp976;});_tmp719;}));
# 2019
goto _LL0;}case 187U: _LL171: _LL172: {
# 2021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2020 "parse.y"
struct _tuple26*_tmp4C1=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4C2=_tmp4C1;struct Cyc_List_List*_tmp4CD;int _tmp4CC;struct Cyc_Absyn_VarargInfo*_tmp4CB;void*_tmp4CA;struct Cyc_List_List*_tmp4C9;_LL44A: _tmp4CD=_tmp4C2->f1;_tmp4CC=_tmp4C2->f2;_tmp4CB=_tmp4C2->f3;_tmp4CA=_tmp4C2->f4;_tmp4C9=_tmp4C2->f5;_LL44B:;{
struct Cyc_Absyn_Exp*_tmp4C3=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4C4=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp4C5=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71A;_tmp71A.id=_tmp4C5.id,_tmp71A.varloc=_tmp4C5.varloc,({struct Cyc_List_List*_tmp979=({struct Cyc_List_List*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));({void*_tmp978=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));_tmp4C7->tag=3U,({void*_tmp977=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));_tmp4C6->tag=1U,_tmp4C6->f1=_tmp4CD,_tmp4C6->f2=_tmp4CC,_tmp4C6->f3=_tmp4CB,_tmp4C6->f4=_tmp4CA,_tmp4C6->f5=_tmp4C9,_tmp4C6->f6=_tmp4C3,_tmp4C6->f7=_tmp4C4;_tmp4C6;});_tmp4C7->f1=_tmp977;});_tmp4C7;});_tmp4C8->hd=_tmp978;}),_tmp4C8->tl=_tmp4C5.tms;_tmp4C8;});_tmp71A.tms=_tmp979;});_tmp71A;}));
# 2026
goto _LL0;};}case 188U: _LL173: _LL174: {
# 2028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2027 "parse.y"
struct Cyc_Parse_Declarator _tmp4CE=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71B;_tmp71B.id=_tmp4CE.id,_tmp71B.varloc=_tmp4CE.varloc,({
struct Cyc_List_List*_tmp980=({struct Cyc_List_List*_tmp4D1=_region_malloc(yyr,sizeof(*_tmp4D1));({void*_tmp97F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));_tmp4D0->tag=3U,({void*_tmp97E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));_tmp4CF->tag=1U,_tmp4CF->f1=0,_tmp4CF->f2=0,_tmp4CF->f3=0,({
# 2031
void*_tmp97D=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4CF->f4=_tmp97D;}),({struct Cyc_List_List*_tmp97C=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4CF->f5=_tmp97C;}),({struct Cyc_Absyn_Exp*_tmp97B=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4CF->f6=_tmp97B;}),({struct Cyc_Absyn_Exp*_tmp97A=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4CF->f7=_tmp97A;});_tmp4CF;});
# 2029
_tmp4D0->f1=_tmp97E;});_tmp4D0;});_tmp4D1->hd=_tmp97F;}),_tmp4D1->tl=_tmp4CE.tms;_tmp4D1;});_tmp71B.tms=_tmp980;});_tmp71B;}));
# 2034
goto _LL0;}case 189U: _LL175: _LL176: {
# 2036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2035 "parse.y"
struct Cyc_Parse_Declarator _tmp4D2=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71C;_tmp71C.id=_tmp4D2.id,_tmp71C.varloc=_tmp4D2.varloc,({struct Cyc_List_List*_tmp984=({struct Cyc_List_List*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));({void*_tmp983=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D4=_region_malloc(yyr,sizeof(*_tmp4D4));_tmp4D4->tag=3U,({void*_tmp982=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));_tmp4D3->tag=0U,({struct Cyc_List_List*_tmp981=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4D3->f1=_tmp981;}),_tmp4D3->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4D3;});_tmp4D4->f1=_tmp982;});_tmp4D4;});_tmp4D5->hd=_tmp983;}),_tmp4D5->tl=_tmp4D2.tms;_tmp4D5;});_tmp71C.tms=_tmp984;});_tmp71C;}));
goto _LL0;}case 190U: _LL177: _LL178: {
# 2039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2039 "parse.y"
struct Cyc_List_List*_tmp4D6=({unsigned int _tmp985=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp985,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp4D7=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71D;_tmp71D.id=_tmp4D7.id,_tmp71D.varloc=_tmp4D7.varloc,({struct Cyc_List_List*_tmp987=({struct Cyc_List_List*_tmp4D9=_region_malloc(yyr,sizeof(*_tmp4D9));({void*_tmp986=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4D8=_region_malloc(yyr,sizeof(*_tmp4D8));_tmp4D8->tag=4U,_tmp4D8->f1=_tmp4D6,_tmp4D8->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4D8->f3=0;_tmp4D8;});_tmp4D9->hd=_tmp986;}),_tmp4D9->tl=_tmp4D7.tms;_tmp4D9;});_tmp71D.tms=_tmp987;});_tmp71D;}));
# 2043
goto _LL0;}case 191U: _LL179: _LL17A: {
# 2045
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2047
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2044 "parse.y"
struct Cyc_Parse_Declarator _tmp4DA=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71E;_tmp71E.id=_tmp4DA.id,_tmp71E.varloc=_tmp4DA.varloc,({struct Cyc_List_List*_tmp98A=({struct Cyc_List_List*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));({void*_tmp989=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4DB=_region_malloc(yyr,sizeof(*_tmp4DB));_tmp4DB->tag=5U,_tmp4DB->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp988=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4DB->f2=_tmp988;});_tmp4DB;});_tmp4DC->hd=_tmp989;}),_tmp4DC->tl=_tmp4DA.tms;_tmp4DC;});_tmp71E.tms=_tmp98A;});_tmp71E;}));
# 2047
goto _LL0;}case 192U: _LL17B: _LL17C: {
# 2049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2051 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 193U: _LL17D: _LL17E: {
# 2054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2052 "parse.y"
yyval=Cyc_YY26(({struct Cyc_List_List*_tmp98B=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp98B,Cyc_yyget_YY26(yyyyvsp[1]));}));
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2056 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
ans=({struct Cyc_List_List*_tmp4DE=_region_malloc(yyr,sizeof(*_tmp4DE));({void*_tmp98D=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4DD=_region_malloc(yyr,sizeof(*_tmp4DD));_tmp4DD->tag=5U,_tmp4DD->f1=(unsigned int)(yyyylsp[3]).first_line,({struct Cyc_List_List*_tmp98C=Cyc_yyget_YY45(yyyyvsp[3]);_tmp4DD->f2=_tmp98C;});_tmp4DD;});_tmp4DE->hd=_tmp98D;}),_tmp4DE->tl=ans;_tmp4DE;});{
# 2060
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp4DF=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp4E0=_tmp4DF;unsigned int _tmp4EC;void*_tmp4EB;void*_tmp4EA;_LL44D: _tmp4EC=_tmp4E0.f1;_tmp4EB=_tmp4E0.f2;_tmp4EA=_tmp4E0.f3;_LL44E:;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->ptr_loc=_tmp4EC,_tmp4E1->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,_tmp4E1->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp4E1;});{
# 2065
struct _tuple14 _tmp4E2=({unsigned int _tmp991=_tmp4EC;void*_tmp990=_tmp4EB;void*_tmp98F=_tmp4EA;void*_tmp98E=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmp991,_tmp990,_tmp98F,_tmp98E,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp4E3=_tmp4E2;void*_tmp4E9;void*_tmp4E8;void*_tmp4E7;void*_tmp4E6;_LL450: _tmp4E9=_tmp4E3.f1;_tmp4E8=_tmp4E3.f2;_tmp4E7=_tmp4E3.f3;_tmp4E6=_tmp4E3.f4;_LL451:;
ans=({struct Cyc_List_List*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));({void*_tmp993=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));_tmp4E4->tag=2U,(_tmp4E4->f1).rgn=_tmp4E6,(_tmp4E4->f1).nullable=_tmp4E9,(_tmp4E4->f1).bounds=_tmp4E8,(_tmp4E4->f1).zero_term=_tmp4E7,(_tmp4E4->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp992=Cyc_yyget_YY23(yyyyvsp[4]);_tmp4E4->f2=_tmp992;});_tmp4E4;});_tmp4E5->hd=_tmp993;}),_tmp4E5->tl=ans;_tmp4E5;});
yyval=Cyc_YY26(ans);
# 2069
goto _LL0;};};}case 195U: _LL181: _LL182:
# 2071
 yyval=Cyc_YY54(0);
goto _LL0;case 196U: _LL183: _LL184: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2072 "parse.y"
yyval=Cyc_YY54(({struct Cyc_List_List*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));({void*_tmp995=Cyc_yyget_YY53(yyyyvsp[0]);_tmp4ED->hd=_tmp995;}),({struct Cyc_List_List*_tmp994=Cyc_yyget_YY54(yyyyvsp[1]);_tmp4ED->tl=_tmp994;});_tmp4ED;}));
goto _LL0;}case 197U: _LL185: _LL186: {
# 2075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2077 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));_tmp4EE->tag=0U,({struct Cyc_Absyn_Exp*_tmp996=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4EE->f1=_tmp996;});_tmp4EE;}));
goto _LL0;}case 198U: _LL187: _LL188: {
# 2080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2079 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp4EF=_region_malloc(yyr,sizeof(*_tmp4EF));_tmp4EF->tag=1U,({void*_tmp997=Cyc_yyget_YY44(yyyyvsp[2]);_tmp4EF->f1=_tmp997;});_tmp4EF;}));
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2081 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp4F0=_region_malloc(yyr,sizeof(*_tmp4F0));_tmp4F0->tag=2U;_tmp4F0;}));
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2083 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));_tmp4F1->tag=3U;_tmp4F1;}));
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2086
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2088
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2085 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp4F2=_region_malloc(yyr,sizeof(*_tmp4F2));_tmp4F2->tag=4U;_tmp4F2;}));
goto _LL0;}case 202U: _LL18F: _LL190: {
# 2088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2087 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));_tmp4F3->tag=5U;_tmp4F3;}));
goto _LL0;}case 203U: _LL191: _LL192: {
# 2090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2089 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));_tmp4F4->tag=6U;_tmp4F4;}));
goto _LL0;}case 204U: _LL193: _LL194: {
# 2092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2091 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));_tmp4F5->tag=7U;_tmp4F5;}));
goto _LL0;}case 205U: _LL195: _LL196: {
# 2094
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2096
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2097 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
if(!Cyc_Parse_parsing_tempest)
yyval=Cyc_YY1(({struct _tuple21*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->f1=loc,_tmp4F6->f2=Cyc_Absyn_true_type,({void*_tmp998=Cyc_yyget_YY2(yyyyvsp[1]);_tmp4F6->f3=_tmp998;});_tmp4F6;}));else{
# 2101
yyval=Cyc_YY1(({struct _tuple21*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->f1=loc,_tmp4F7->f2=Cyc_Absyn_true_type,_tmp4F7->f3=Cyc_Absyn_fat_bound_type;_tmp4F7;}));}
# 2103
goto _LL0;}case 206U: _LL197: _LL198: {
# 2105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2104 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->f1=loc,_tmp4F8->f2=Cyc_Absyn_false_type,({void*_tmp999=Cyc_yyget_YY2(yyyyvsp[1]);_tmp4F8->f3=_tmp999;});_tmp4F8;}));
# 2107
goto _LL0;}case 207U: _LL199: _LL19A: {
# 2109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2108 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->f1=loc,_tmp4F9->f2=Cyc_Absyn_true_type,_tmp4F9->f3=Cyc_Absyn_fat_bound_type;_tmp4F9;}));
# 2111
goto _LL0;}case 208U: _LL19B: _LL19C:
# 2113
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 209U: _LL19D: _LL19E: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2114 "parse.y"
yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 210U: _LL19F: _LL1A0:
# 2117
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 211U: _LL1A1: _LL1A2: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2118 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;}case 212U: _LL1A3: _LL1A4: {
# 2121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2119 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;}case 213U: _LL1A5: _LL1A6:
# 2122
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 214U: _LL1A7: _LL1A8: {
# 2125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2125 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL0;}case 215U: _LL1A9: _LL1AA: {
# 2128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2126 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;}case 216U: _LL1AB: _LL1AC:
# 2129
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
goto _LL0;case 217U: _LL1AD: _LL1AE: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2131 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp99A=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp99A,Cyc_yyget_YY23(yyyyvsp[1]));}));
goto _LL0;}case 218U: _LL1AF: _LL1B0: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2136 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));({struct Cyc_List_List*_tmp99D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp4FA->f1=_tmp99D;}),_tmp4FA->f2=0,_tmp4FA->f3=0,({void*_tmp99C=Cyc_yyget_YY49(yyyyvsp[1]);_tmp4FA->f4=_tmp99C;}),({struct Cyc_List_List*_tmp99B=Cyc_yyget_YY50(yyyyvsp[2]);_tmp4FA->f5=_tmp99B;});_tmp4FA;}));
goto _LL0;}case 219U: _LL1B1: _LL1B2: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2138 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));({struct Cyc_List_List*_tmp9A0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp4FB->f1=_tmp9A0;}),_tmp4FB->f2=1,_tmp4FB->f3=0,({void*_tmp99F=Cyc_yyget_YY49(yyyyvsp[3]);_tmp4FB->f4=_tmp99F;}),({struct Cyc_List_List*_tmp99E=Cyc_yyget_YY50(yyyyvsp[4]);_tmp4FB->f5=_tmp99E;});_tmp4FB;}));
goto _LL0;}case 220U: _LL1B3: _LL1B4: {
# 2141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2141 "parse.y"
struct _tuple8*_tmp4FC=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp4FD=_tmp4FC;struct _dyneither_ptr*_tmp503;struct Cyc_Absyn_Tqual _tmp502;void*_tmp501;_LL453: _tmp503=_tmp4FD->f1;_tmp502=_tmp4FD->f2;_tmp501=_tmp4FD->f3;_LL454:;{
struct Cyc_Absyn_VarargInfo*_tmp4FE=({struct Cyc_Absyn_VarargInfo*_tmp500=_cycalloc(sizeof(*_tmp500));_tmp500->name=_tmp503,_tmp500->tq=_tmp502,_tmp500->type=_tmp501,({int _tmp9A1=Cyc_yyget_YY31(yyyyvsp[1]);_tmp500->inject=_tmp9A1;});_tmp500;});
yyval=Cyc_YY39(({struct _tuple26*_tmp4FF=_cycalloc(sizeof(*_tmp4FF));_tmp4FF->f1=0,_tmp4FF->f2=0,_tmp4FF->f3=_tmp4FE,({void*_tmp9A3=Cyc_yyget_YY49(yyyyvsp[3]);_tmp4FF->f4=_tmp9A3;}),({struct Cyc_List_List*_tmp9A2=Cyc_yyget_YY50(yyyyvsp[4]);_tmp4FF->f5=_tmp9A2;});_tmp4FF;}));
# 2145
goto _LL0;};}case 221U: _LL1B5: _LL1B6: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2147 "parse.y"
struct _tuple8*_tmp504=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp505=_tmp504;struct _dyneither_ptr*_tmp50B;struct Cyc_Absyn_Tqual _tmp50A;void*_tmp509;_LL456: _tmp50B=_tmp505->f1;_tmp50A=_tmp505->f2;_tmp509=_tmp505->f3;_LL457:;{
struct Cyc_Absyn_VarargInfo*_tmp506=({struct Cyc_Absyn_VarargInfo*_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508->name=_tmp50B,_tmp508->tq=_tmp50A,_tmp508->type=_tmp509,({int _tmp9A4=Cyc_yyget_YY31(yyyyvsp[3]);_tmp508->inject=_tmp9A4;});_tmp508;});
yyval=Cyc_YY39(({struct _tuple26*_tmp507=_cycalloc(sizeof(*_tmp507));({struct Cyc_List_List*_tmp9A7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp507->f1=_tmp9A7;}),_tmp507->f2=0,_tmp507->f3=_tmp506,({void*_tmp9A6=Cyc_yyget_YY49(yyyyvsp[5]);_tmp507->f4=_tmp9A6;}),({struct Cyc_List_List*_tmp9A5=Cyc_yyget_YY50(yyyyvsp[6]);_tmp507->f5=_tmp9A5;});_tmp507;}));
# 2151
goto _LL0;};}case 222U: _LL1B7: _LL1B8: {
# 2153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2155 "parse.y"
yyval=Cyc_YY44(({struct _dyneither_ptr _tmp9A8=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmp9A8,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->tag=1U,_tmp50C->f1=0;_tmp50C;}));}));
goto _LL0;}case 223U: _LL1B9: _LL1BA: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2156 "parse.y"
yyval=Cyc_YY44(({struct _dyneither_ptr _tmp9A9=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmp9A9,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));
goto _LL0;}case 224U: _LL1BB: _LL1BC:
# 2159
 yyval=Cyc_YY49(0);
goto _LL0;case 225U: _LL1BD: _LL1BE: {
# 2162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2160 "parse.y"
yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 226U: _LL1BF: _LL1C0:
# 2163
 yyval=Cyc_YY50(0);
goto _LL0;case 227U: _LL1C1: _LL1C2: {
# 2166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2165 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 228U: _LL1C3: _LL1C4: {
# 2168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2173 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50D=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->tag=2U,_tmp512->f1=0,_tmp512->f2=& Cyc_Tcutil_trk;_tmp512;});
struct _dyneither_ptr _tmp50E=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp50F=Cyc_Parse_id2type(_tmp50E,(void*)_tmp50D);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp511=_cycalloc(sizeof(*_tmp511));({struct _tuple29*_tmp9AB=({struct _tuple29*_tmp510=_cycalloc(sizeof(*_tmp510));({void*_tmp9AA=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp510->f1=_tmp9AA;}),_tmp510->f2=_tmp50F;_tmp510;});_tmp511->hd=_tmp9AB;}),_tmp511->tl=0;_tmp511;}));
# 2178
goto _LL0;}case 229U: _LL1C5: _LL1C6: {
# 2180
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2180 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp513=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->tag=2U,_tmp518->f1=0,_tmp518->f2=& Cyc_Tcutil_trk;_tmp518;});
struct _dyneither_ptr _tmp514=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp515=Cyc_Parse_id2type(_tmp514,(void*)_tmp513);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp517=_cycalloc(sizeof(*_tmp517));({struct _tuple29*_tmp9AE=({struct _tuple29*_tmp516=_cycalloc(sizeof(*_tmp516));({void*_tmp9AD=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp516->f1=_tmp9AD;}),_tmp516->f2=_tmp515;_tmp516;});_tmp517->hd=_tmp9AE;}),({struct Cyc_List_List*_tmp9AC=Cyc_yyget_YY50(yyyyvsp[4]);_tmp517->tl=_tmp9AC;});_tmp517;}));
# 2185
goto _LL0;}case 230U: _LL1C7: _LL1C8:
# 2187
 yyval=Cyc_YY31(0);
goto _LL0;case 231U: _LL1C9: _LL1CA: {
# 2190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2191 "parse.y"
if(({struct _dyneither_ptr _tmp9AF=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmp9AF,({const char*_tmp519="inject";_tag_dyneither(_tmp519,sizeof(char),7U);}));})!= 0)
({void*_tmp51A=0U;({unsigned int _tmp9B1=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9B0=({const char*_tmp51B="missing type in function declaration";_tag_dyneither(_tmp51B,sizeof(char),37U);});Cyc_Warn_err(_tmp9B1,_tmp9B0,_tag_dyneither(_tmp51A,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);
# 2195
goto _LL0;}case 232U: _LL1CB: _LL1CC: {
# 2197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2198 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 233U: _LL1CD: _LL1CE: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2199 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp9B2=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9B2,Cyc_yyget_YY40(yyyyvsp[2]));}));
goto _LL0;}case 234U: _LL1CF: _LL1D0: {
# 2202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2203 "parse.y"
yyval=Cyc_YY40(0);
goto _LL0;}case 235U: _LL1D1: _LL1D2: {
# 2206
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2204 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 236U: _LL1D3: _LL1D4: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2206 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp51C=_cycalloc(sizeof(*_tmp51C));({void*_tmp9B3=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2]));_tmp51C->hd=_tmp9B3;}),_tmp51C->tl=0;_tmp51C;}));
goto _LL0;}case 237U: _LL1D5: _LL1D6: {
# 2209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2208 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp51D=_cycalloc(sizeof(*_tmp51D));({void*_tmp9B4=Cyc_yyget_YY44(yyyyvsp[0]);_tmp51D->hd=_tmp9B4;}),_tmp51D->tl=0;_tmp51D;}));
# 2211
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2228 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp51E=_cycalloc(sizeof(*_tmp51E));({void*_tmp9B6=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9B5=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9B5,(unsigned int)(yyyylsp[0]).first_line);}));_tmp51E->hd=_tmp9B6;}),_tmp51E->tl=0;_tmp51E;}));
goto _LL0;}case 239U: _LL1D9: _LL1DA: {
# 2231
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2230 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp51F=_cycalloc(sizeof(*_tmp51F));({void*_tmp9B9=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9B8=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9B8,(unsigned int)(yyyylsp[0]).first_line);}));_tmp51F->hd=_tmp9B9;}),({struct Cyc_List_List*_tmp9B7=Cyc_yyget_YY40(yyyyvsp[2]);_tmp51F->tl=_tmp9B7;});_tmp51F;}));
goto _LL0;}case 240U: _LL1DB: _LL1DC: {
# 2233
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2235
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp520=_cycalloc(sizeof(*_tmp520));({struct _tuple8*_tmp9BA=Cyc_yyget_YY37(yyyyvsp[0]);_tmp520->hd=_tmp9BA;}),_tmp520->tl=0;_tmp520;}));
goto _LL0;}case 241U: _LL1DD: _LL1DE: {
# 2239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2238 "parse.y"
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp521=_cycalloc(sizeof(*_tmp521));({struct _tuple8*_tmp9BC=Cyc_yyget_YY37(yyyyvsp[2]);_tmp521->hd=_tmp9BC;}),({struct Cyc_List_List*_tmp9BB=Cyc_yyget_YY38(yyyyvsp[0]);_tmp521->tl=_tmp9BB;});_tmp521;}));
goto _LL0;}case 242U: _LL1DF: _LL1E0: {
# 2241
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2243
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
struct _tuple25 _tmp522=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp523=_tmp522;struct Cyc_Absyn_Tqual _tmp539;struct Cyc_Parse_Type_specifier _tmp538;struct Cyc_List_List*_tmp537;_LL459: _tmp539=_tmp523.f1;_tmp538=_tmp523.f2;_tmp537=_tmp523.f3;_LL45A:;
if(_tmp539.loc == (unsigned int)0)_tmp539.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp524=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp525=_tmp524;struct _tuple0*_tmp536;unsigned int _tmp535;struct Cyc_List_List*_tmp534;_LL45C: _tmp536=_tmp525.id;_tmp535=_tmp525.varloc;_tmp534=_tmp525.tms;_LL45D:;{
void*_tmp526=Cyc_Parse_speclist2typ(_tmp538,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp527=Cyc_Parse_apply_tms(_tmp539,_tmp526,_tmp537,_tmp534);struct _tuple13 _tmp528=_tmp527;struct Cyc_Absyn_Tqual _tmp533;void*_tmp532;struct Cyc_List_List*_tmp531;struct Cyc_List_List*_tmp530;_LL45F: _tmp533=_tmp528.f1;_tmp532=_tmp528.f2;_tmp531=_tmp528.f3;_tmp530=_tmp528.f4;_LL460:;
if(_tmp531 != 0)
({void*_tmp529=0U;({unsigned int _tmp9BE=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9BD=({const char*_tmp52A="parameter with bad type params";_tag_dyneither(_tmp52A,sizeof(char),31U);});Cyc_Warn_err(_tmp9BE,_tmp9BD,_tag_dyneither(_tmp529,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp536))
({void*_tmp52B=0U;({unsigned int _tmp9C0=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9BF=({const char*_tmp52C="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp52C,sizeof(char),47U);});Cyc_Warn_err(_tmp9C0,_tmp9BF,_tag_dyneither(_tmp52B,sizeof(void*),0U));});});{
struct _dyneither_ptr*idopt=(*_tmp536).f2;
if(_tmp530 != 0)
({void*_tmp52D=0U;({unsigned int _tmp9C2=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C1=({const char*_tmp52E="extra attributes on parameter, ignoring";_tag_dyneither(_tmp52E,sizeof(char),40U);});Cyc_Warn_warn(_tmp9C2,_tmp9C1,_tag_dyneither(_tmp52D,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp52F=_cycalloc(sizeof(*_tmp52F));_tmp52F->f1=idopt,_tmp52F->f2=_tmp533,_tmp52F->f3=_tmp532;_tmp52F;}));
# 2258
goto _LL0;};};};}case 243U: _LL1E1: _LL1E2: {
# 2260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2259 "parse.y"
struct _tuple25 _tmp53A=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp53B=_tmp53A;struct Cyc_Absyn_Tqual _tmp542;struct Cyc_Parse_Type_specifier _tmp541;struct Cyc_List_List*_tmp540;_LL462: _tmp542=_tmp53B.f1;_tmp541=_tmp53B.f2;_tmp540=_tmp53B.f3;_LL463:;
if(_tmp542.loc == (unsigned int)0)_tmp542.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp53C=Cyc_Parse_speclist2typ(_tmp541,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp540 != 0)
({void*_tmp53D=0U;({unsigned int _tmp9C4=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C3=({const char*_tmp53E="bad attributes on parameter, ignoring";_tag_dyneither(_tmp53E,sizeof(char),38U);});Cyc_Warn_warn(_tmp9C4,_tmp9C3,_tag_dyneither(_tmp53D,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->f1=0,_tmp53F->f2=_tmp542,_tmp53F->f3=_tmp53C;_tmp53F;}));
# 2266
goto _LL0;};}case 244U: _LL1E3: _LL1E4: {
# 2268
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2270
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2267 "parse.y"
struct _tuple25 _tmp543=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp544=_tmp543;struct Cyc_Absyn_Tqual _tmp554;struct Cyc_Parse_Type_specifier _tmp553;struct Cyc_List_List*_tmp552;_LL465: _tmp554=_tmp544.f1;_tmp553=_tmp544.f2;_tmp552=_tmp544.f3;_LL466:;
if(_tmp554.loc == (unsigned int)0)_tmp554.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp545=Cyc_Parse_speclist2typ(_tmp553,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp546=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp547=Cyc_Parse_apply_tms(_tmp554,_tmp545,_tmp552,_tmp546);struct _tuple13 _tmp548=_tmp547;struct Cyc_Absyn_Tqual _tmp551;void*_tmp550;struct Cyc_List_List*_tmp54F;struct Cyc_List_List*_tmp54E;_LL468: _tmp551=_tmp548.f1;_tmp550=_tmp548.f2;_tmp54F=_tmp548.f3;_tmp54E=_tmp548.f4;_LL469:;
if(_tmp54F != 0)
({void*_tmp549=0U;({unsigned int _tmp9C6=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C5=({const char*_tmp54A="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp54A,sizeof(char),49U);});Cyc_Warn_warn(_tmp9C6,_tmp9C5,_tag_dyneither(_tmp549,sizeof(void*),0U));});});
# 2275
if(_tmp54E != 0)
({void*_tmp54B=0U;({unsigned int _tmp9C8=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C7=({const char*_tmp54C="bad attributes on parameter, ignoring";_tag_dyneither(_tmp54C,sizeof(char),38U);});Cyc_Warn_warn(_tmp9C8,_tmp9C7,_tag_dyneither(_tmp54B,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->f1=0,_tmp54D->f2=_tmp551,_tmp54D->f3=_tmp550;_tmp54D;}));
# 2279
goto _LL0;};}case 245U: _LL1E5: _LL1E6: {
# 2281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2283 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
goto _LL0;}case 246U: _LL1E7: _LL1E8: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2287 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp556=_cycalloc(sizeof(*_tmp556));({struct _dyneither_ptr*_tmp9CA=({struct _dyneither_ptr*_tmp555=_cycalloc(sizeof(*_tmp555));({struct _dyneither_ptr _tmp9C9=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp555=_tmp9C9;});_tmp555;});_tmp556->hd=_tmp9CA;}),_tmp556->tl=0;_tmp556;}));
goto _LL0;}case 247U: _LL1E9: _LL1EA: {
# 2290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2289 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp558=_cycalloc(sizeof(*_tmp558));({struct _dyneither_ptr*_tmp9CD=({struct _dyneither_ptr*_tmp557=_cycalloc(sizeof(*_tmp557));({struct _dyneither_ptr _tmp9CC=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp557=_tmp9CC;});_tmp557;});_tmp558->hd=_tmp9CD;}),({struct Cyc_List_List*_tmp9CB=Cyc_yyget_YY36(yyyyvsp[0]);_tmp558->tl=_tmp9CB;});_tmp558;}));
goto _LL0;}case 248U: _LL1EB: _LL1EC: {
# 2292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2293 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 249U: _LL1ED: _LL1EE: {
# 2296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2294 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 250U: _LL1EF: _LL1F0: {
# 2297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2299 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9CE=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559->tag=36U,_tmp559->f1=0,_tmp559->f2=0;_tmp559;});Cyc_Absyn_new_exp(_tmp9CE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 251U: _LL1F1: _LL1F2: {
# 2302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2301 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9D0=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->tag=36U,_tmp55A->f1=0,({struct Cyc_List_List*_tmp9CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp55A->f2=_tmp9CF;});_tmp55A;});Cyc_Absyn_new_exp(_tmp9D0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 252U: _LL1F3: _LL1F4: {
# 2304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2303 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9D2=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->tag=36U,_tmp55B->f1=0,({struct Cyc_List_List*_tmp9D1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp55B->f2=_tmp9D1;});_tmp55B;});Cyc_Absyn_new_exp(_tmp9D2,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 253U: _LL1F5: _LL1F6: {
# 2306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2305 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp55C=({unsigned int _tmp9D7=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp9D6=({struct _tuple0*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp9D4=({struct _dyneither_ptr*_tmp55E=_cycalloc(sizeof(*_tmp55E));({struct _dyneither_ptr _tmp9D3=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp55E=_tmp9D3;});_tmp55E;});_tmp55F->f2=_tmp9D4;});_tmp55F;});void*_tmp9D5=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmp9D7,_tmp9D6,_tmp9D5,
Cyc_Absyn_uint_exp(0U,(unsigned int)(yyyylsp[2]).first_line));});
# 2308
(_tmp55C->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmp9DA=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=27U,_tmp55D->f1=_tmp55C,({struct Cyc_Absyn_Exp*_tmp9D9=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp55D->f2=_tmp9D9;}),({struct Cyc_Absyn_Exp*_tmp9D8=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp55D->f3=_tmp9D8;}),_tmp55D->f4=0;_tmp55D;});Cyc_Absyn_new_exp(_tmp9DA,(unsigned int)(yyyylsp[0]).first_line);}));
# 2311
goto _LL0;}case 254U: _LL1F7: _LL1F8: {
# 2313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2313 "parse.y"
void*_tmp560=({struct _tuple8*_tmp9DB=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmp9DB,(unsigned int)(yyyylsp[6]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmp9DD=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->tag=28U,({struct Cyc_Absyn_Exp*_tmp9DC=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp561->f1=_tmp9DC;}),_tmp561->f2=_tmp560,_tmp561->f3=0;_tmp561;});Cyc_Absyn_new_exp(_tmp9DD,(unsigned int)(yyyylsp[0]).first_line);}));
# 2316
goto _LL0;}case 255U: _LL1F9: _LL1FA: {
# 2318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp563=_cycalloc(sizeof(*_tmp563));({struct _tuple30*_tmp9DF=({struct _tuple30*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->f1=0,({struct Cyc_Absyn_Exp*_tmp9DE=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp562->f2=_tmp9DE;});_tmp562;});_tmp563->hd=_tmp9DF;}),_tmp563->tl=0;_tmp563;}));
goto _LL0;}case 256U: _LL1FB: _LL1FC: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2323 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp565=_cycalloc(sizeof(*_tmp565));({struct _tuple30*_tmp9E2=({struct _tuple30*_tmp564=_cycalloc(sizeof(*_tmp564));({struct Cyc_List_List*_tmp9E1=Cyc_yyget_YY41(yyyyvsp[0]);_tmp564->f1=_tmp9E1;}),({struct Cyc_Absyn_Exp*_tmp9E0=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp564->f2=_tmp9E0;});_tmp564;});_tmp565->hd=_tmp9E2;}),_tmp565->tl=0;_tmp565;}));
goto _LL0;}case 257U: _LL1FD: _LL1FE: {
# 2326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2325 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp567=_cycalloc(sizeof(*_tmp567));({struct _tuple30*_tmp9E5=({struct _tuple30*_tmp566=_cycalloc(sizeof(*_tmp566));_tmp566->f1=0,({struct Cyc_Absyn_Exp*_tmp9E4=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp566->f2=_tmp9E4;});_tmp566;});_tmp567->hd=_tmp9E5;}),({struct Cyc_List_List*_tmp9E3=Cyc_yyget_YY5(yyyyvsp[0]);_tmp567->tl=_tmp9E3;});_tmp567;}));
goto _LL0;}case 258U: _LL1FF: _LL200: {
# 2328
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2327 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp569=_cycalloc(sizeof(*_tmp569));({struct _tuple30*_tmp9E9=({struct _tuple30*_tmp568=_cycalloc(sizeof(*_tmp568));({struct Cyc_List_List*_tmp9E8=Cyc_yyget_YY41(yyyyvsp[2]);_tmp568->f1=_tmp9E8;}),({struct Cyc_Absyn_Exp*_tmp9E7=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp568->f2=_tmp9E7;});_tmp568;});_tmp569->hd=_tmp9E9;}),({struct Cyc_List_List*_tmp9E6=Cyc_yyget_YY5(yyyyvsp[0]);_tmp569->tl=_tmp9E6;});_tmp569;}));
goto _LL0;}case 259U: _LL201: _LL202: {
# 2330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2331 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
goto _LL0;}case 260U: _LL203: _LL204: {
# 2334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2332 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));({void*_tmp9EC=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->tag=1U,({struct _dyneither_ptr*_tmp9EB=({struct _dyneither_ptr*_tmp56A=_cycalloc(sizeof(*_tmp56A));({struct _dyneither_ptr _tmp9EA=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp56A=_tmp9EA;});_tmp56A;});_tmp56B->f1=_tmp9EB;});_tmp56B;});_tmp56C->hd=_tmp9EC;}),_tmp56C->tl=0;_tmp56C;}));
goto _LL0;}case 261U: _LL205: _LL206: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2337 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp56D=_cycalloc(sizeof(*_tmp56D));({void*_tmp9ED=Cyc_yyget_YY42(yyyyvsp[0]);_tmp56D->hd=_tmp9ED;}),_tmp56D->tl=0;_tmp56D;}));
goto _LL0;}case 262U: _LL207: _LL208: {
# 2340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2338 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));({void*_tmp9EF=Cyc_yyget_YY42(yyyyvsp[1]);_tmp56E->hd=_tmp9EF;}),({struct Cyc_List_List*_tmp9EE=Cyc_yyget_YY41(yyyyvsp[0]);_tmp56E->tl=_tmp9EE;});_tmp56E;}));
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2342 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=0U,({struct Cyc_Absyn_Exp*_tmp9F0=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp56F->f1=_tmp9F0;});_tmp56F;}));
goto _LL0;}case 264U: _LL20B: _LL20C: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2343 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->tag=1U,({struct _dyneither_ptr*_tmp9F2=({struct _dyneither_ptr*_tmp570=_cycalloc(sizeof(*_tmp570));({struct _dyneither_ptr _tmp9F1=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp570=_tmp9F1;});_tmp570;});_tmp571->f1=_tmp9F2;});_tmp571;}));
goto _LL0;}case 265U: _LL20D: _LL20E: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2348 "parse.y"
struct _tuple25 _tmp572=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp573=_tmp572;struct Cyc_Absyn_Tqual _tmp57A;struct Cyc_Parse_Type_specifier _tmp579;struct Cyc_List_List*_tmp578;_LL46B: _tmp57A=_tmp573.f1;_tmp579=_tmp573.f2;_tmp578=_tmp573.f3;_LL46C:;{
void*_tmp574=Cyc_Parse_speclist2typ(_tmp579,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp578 != 0)
({void*_tmp575=0U;({unsigned int _tmp9F4=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9F3=({const char*_tmp576="ignoring attributes in type";_tag_dyneither(_tmp576,sizeof(char),28U);});Cyc_Warn_warn(_tmp9F4,_tmp9F3,_tag_dyneither(_tmp575,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->f1=0,_tmp577->f2=_tmp57A,_tmp577->f3=_tmp574;_tmp577;}));
# 2354
goto _LL0;};}case 266U: _LL20F: _LL210: {
# 2356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2355 "parse.y"
struct _tuple25 _tmp57B=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp57C=_tmp57B;struct Cyc_Absyn_Tqual _tmp587;struct Cyc_Parse_Type_specifier _tmp586;struct Cyc_List_List*_tmp585;_LL46E: _tmp587=_tmp57C.f1;_tmp586=_tmp57C.f2;_tmp585=_tmp57C.f3;_LL46F:;{
void*_tmp57D=Cyc_Parse_speclist2typ(_tmp586,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57E=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp57F=Cyc_Parse_apply_tms(_tmp587,_tmp57D,_tmp585,_tmp57E);
if(_tmp57F.f3 != 0)
# 2361
({void*_tmp580=0U;({unsigned int _tmp9F6=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9F5=({const char*_tmp581="bad type params, ignoring";_tag_dyneither(_tmp581,sizeof(char),26U);});Cyc_Warn_warn(_tmp9F6,_tmp9F5,_tag_dyneither(_tmp580,sizeof(void*),0U));});});
if(_tmp57F.f4 != 0)
({void*_tmp582=0U;({unsigned int _tmp9F8=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9F7=({const char*_tmp583="bad specifiers, ignoring";_tag_dyneither(_tmp583,sizeof(char),25U);});Cyc_Warn_warn(_tmp9F8,_tmp9F7,_tag_dyneither(_tmp582,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp584=_cycalloc(sizeof(*_tmp584));_tmp584->f1=0,_tmp584->f2=_tmp57F.f1,_tmp584->f3=_tmp57F.f2;_tmp584;}));
# 2366
goto _LL0;};}case 267U: _LL211: _LL212: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2369 "parse.y"
yyval=Cyc_YY44(({struct _tuple8*_tmp9F9=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9F9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 268U: _LL213: _LL214: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2370 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;}case 269U: _LL215: _LL216: {
# 2373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2371 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 270U: _LL217: _LL218: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2372 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2])));
goto _LL0;}case 271U: _LL219: _LL21A: {
# 2375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2373 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp588=_cycalloc(sizeof(*_tmp588));({void*_tmp9FB=Cyc_yyget_YY44(yyyyvsp[0]);_tmp588->hd=_tmp9FB;}),({struct Cyc_List_List*_tmp9FA=Cyc_yyget_YY40(yyyyvsp[2]);_tmp588->tl=_tmp9FA;});_tmp588;})));
goto _LL0;}case 272U: _LL21B: _LL21C: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));({void*_tmp9FC=Cyc_yyget_YY44(yyyyvsp[0]);_tmp589->hd=_tmp9FC;}),_tmp589->tl=0;_tmp589;}));
goto _LL0;}case 273U: _LL21D: _LL21E: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2380 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp58A=_cycalloc(sizeof(*_tmp58A));({void*_tmp9FE=Cyc_yyget_YY44(yyyyvsp[2]);_tmp58A->hd=_tmp9FE;}),({struct Cyc_List_List*_tmp9FD=Cyc_yyget_YY40(yyyyvsp[0]);_tmp58A->tl=_tmp9FD;});_tmp58A;}));
goto _LL0;}case 274U: _LL21F: _LL220: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2385 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp71F;({struct Cyc_List_List*_tmp9FF=Cyc_yyget_YY26(yyyyvsp[0]);_tmp71F.tms=_tmp9FF;});_tmp71F;}));
goto _LL0;}case 275U: _LL221: _LL222: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2387 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 276U: _LL223: _LL224: {
# 2390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2389 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp720;({struct Cyc_List_List*_tmpA01=({struct Cyc_List_List*_tmpA00=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA00,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp720.tms=_tmpA01;});_tmp720;}));
goto _LL0;}case 277U: _LL225: _LL226: {
# 2392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2394 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 278U: _LL227: _LL228: {
# 2397
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2399
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2396 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp721;({struct Cyc_List_List*_tmpA04=({struct Cyc_List_List*_tmp58C=_region_malloc(yyr,sizeof(*_tmp58C));({void*_tmpA03=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp58B=_region_malloc(yyr,sizeof(*_tmp58B));_tmp58B->tag=0U,({void*_tmpA02=Cyc_yyget_YY51(yyyyvsp[2]);_tmp58B->f1=_tmpA02;}),_tmp58B->f2=(unsigned int)(yyyylsp[2]).first_line;_tmp58B;});_tmp58C->hd=_tmpA03;}),_tmp58C->tl=0;_tmp58C;});_tmp721.tms=_tmpA04;});_tmp721;}));
goto _LL0;}case 279U: _LL229: _LL22A: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2398 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp722;({struct Cyc_List_List*_tmpA08=({struct Cyc_List_List*_tmp58E=_region_malloc(yyr,sizeof(*_tmp58E));({void*_tmpA07=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp58D=_region_malloc(yyr,sizeof(*_tmp58D));_tmp58D->tag=0U,({void*_tmpA06=Cyc_yyget_YY51(yyyyvsp[3]);_tmp58D->f1=_tmpA06;}),_tmp58D->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp58D;});_tmp58E->hd=_tmpA07;}),({struct Cyc_List_List*_tmpA05=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp58E->tl=_tmpA05;});_tmp58E;});_tmp722.tms=_tmpA08;});_tmp722;}));
goto _LL0;}case 280U: _LL22B: _LL22C: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2400 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp723;({struct Cyc_List_List*_tmpA0C=({struct Cyc_List_List*_tmp590=_region_malloc(yyr,sizeof(*_tmp590));({void*_tmpA0B=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp58F=_region_malloc(yyr,sizeof(*_tmp58F));_tmp58F->tag=1U,({struct Cyc_Absyn_Exp*_tmpA0A=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp58F->f1=_tmpA0A;}),({void*_tmpA09=Cyc_yyget_YY51(yyyyvsp[3]);_tmp58F->f2=_tmpA09;}),_tmp58F->f3=(unsigned int)(yyyylsp[3]).first_line;_tmp58F;});_tmp590->hd=_tmpA0B;}),_tmp590->tl=0;_tmp590;});_tmp723.tms=_tmpA0C;});_tmp723;}));
goto _LL0;}case 281U: _LL22D: _LL22E: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2402 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp724;({struct Cyc_List_List*_tmpA11=({struct Cyc_List_List*_tmp592=_region_malloc(yyr,sizeof(*_tmp592));({void*_tmpA10=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp591=_region_malloc(yyr,sizeof(*_tmp591));_tmp591->tag=1U,({struct Cyc_Absyn_Exp*_tmpA0F=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp591->f1=_tmpA0F;}),({void*_tmpA0E=Cyc_yyget_YY51(yyyyvsp[4]);_tmp591->f2=_tmpA0E;}),_tmp591->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp591;});_tmp592->hd=_tmpA10;}),({
struct Cyc_List_List*_tmpA0D=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp592->tl=_tmpA0D;});_tmp592;});
# 2402
_tmp724.tms=_tmpA11;});_tmp724;}));
# 2405
goto _LL0;}case 282U: _LL22F: _LL230: {
# 2407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2406 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp725;({struct Cyc_List_List*_tmpA18=({struct Cyc_List_List*_tmp595=_region_malloc(yyr,sizeof(*_tmp595));({void*_tmpA17=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp594=_region_malloc(yyr,sizeof(*_tmp594));_tmp594->tag=3U,({void*_tmpA16=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp593=_region_malloc(yyr,sizeof(*_tmp593));_tmp593->tag=1U,_tmp593->f1=0,_tmp593->f2=0,_tmp593->f3=0,({void*_tmpA15=Cyc_yyget_YY49(yyyyvsp[1]);_tmp593->f4=_tmpA15;}),({struct Cyc_List_List*_tmpA14=Cyc_yyget_YY50(yyyyvsp[2]);_tmp593->f5=_tmpA14;}),({struct Cyc_Absyn_Exp*_tmpA13=Cyc_yyget_YY55(yyyyvsp[4]);_tmp593->f6=_tmpA13;}),({struct Cyc_Absyn_Exp*_tmpA12=Cyc_yyget_YY55(yyyyvsp[5]);_tmp593->f7=_tmpA12;});_tmp593;});_tmp594->f1=_tmpA16;});_tmp594;});_tmp595->hd=_tmpA17;}),_tmp595->tl=0;_tmp595;});_tmp725.tms=_tmpA18;});_tmp725;}));
# 2408
goto _LL0;}case 283U: _LL231: _LL232: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2409 "parse.y"
struct _tuple26*_tmp596=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp597=_tmp596;struct Cyc_List_List*_tmp59F;int _tmp59E;struct Cyc_Absyn_VarargInfo*_tmp59D;void*_tmp59C;struct Cyc_List_List*_tmp59B;_LL471: _tmp59F=_tmp597->f1;_tmp59E=_tmp597->f2;_tmp59D=_tmp597->f3;_tmp59C=_tmp597->f4;_tmp59B=_tmp597->f5;_LL472:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp726;({struct Cyc_List_List*_tmpA1D=({struct Cyc_List_List*_tmp59A=_region_malloc(yyr,sizeof(*_tmp59A));({void*_tmpA1C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp599=_region_malloc(yyr,sizeof(*_tmp599));_tmp599->tag=3U,({void*_tmpA1B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp598=_region_malloc(yyr,sizeof(*_tmp598));_tmp598->tag=1U,_tmp598->f1=_tmp59F,_tmp598->f2=_tmp59E,_tmp598->f3=_tmp59D,_tmp598->f4=_tmp59C,_tmp598->f5=_tmp59B,({struct Cyc_Absyn_Exp*_tmpA1A=Cyc_yyget_YY55(yyyyvsp[3]);_tmp598->f6=_tmpA1A;}),({struct Cyc_Absyn_Exp*_tmpA19=Cyc_yyget_YY55(yyyyvsp[4]);_tmp598->f7=_tmpA19;});_tmp598;});_tmp599->f1=_tmpA1B;});_tmp599;});_tmp59A->hd=_tmpA1C;}),_tmp59A->tl=0;_tmp59A;});_tmp726.tms=_tmpA1D;});_tmp726;}));
# 2412
goto _LL0;}case 284U: _LL233: _LL234: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2413 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp727;({struct Cyc_List_List*_tmpA25=({struct Cyc_List_List*_tmp5A2=_region_malloc(yyr,sizeof(*_tmp5A2));({void*_tmpA24=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5A1=_region_malloc(yyr,sizeof(*_tmp5A1));_tmp5A1->tag=3U,({void*_tmpA23=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A0=_region_malloc(yyr,sizeof(*_tmp5A0));_tmp5A0->tag=1U,_tmp5A0->f1=0,_tmp5A0->f2=0,_tmp5A0->f3=0,({void*_tmpA22=Cyc_yyget_YY49(yyyyvsp[2]);_tmp5A0->f4=_tmpA22;}),({struct Cyc_List_List*_tmpA21=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5A0->f5=_tmpA21;}),({struct Cyc_Absyn_Exp*_tmpA20=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5A0->f6=_tmpA20;}),({struct Cyc_Absyn_Exp*_tmpA1F=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5A0->f7=_tmpA1F;});_tmp5A0;});_tmp5A1->f1=_tmpA23;});_tmp5A1;});_tmp5A2->hd=_tmpA24;}),({
struct Cyc_List_List*_tmpA1E=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5A2->tl=_tmpA1E;});_tmp5A2;});
# 2413
_tmp727.tms=_tmpA25;});_tmp727;}));
# 2416
goto _LL0;}case 285U: _LL235: _LL236: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2417 "parse.y"
struct _tuple26*_tmp5A3=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5A4=_tmp5A3;struct Cyc_List_List*_tmp5AC;int _tmp5AB;struct Cyc_Absyn_VarargInfo*_tmp5AA;void*_tmp5A9;struct Cyc_List_List*_tmp5A8;_LL474: _tmp5AC=_tmp5A4->f1;_tmp5AB=_tmp5A4->f2;_tmp5AA=_tmp5A4->f3;_tmp5A9=_tmp5A4->f4;_tmp5A8=_tmp5A4->f5;_LL475:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp728;({struct Cyc_List_List*_tmpA2B=({struct Cyc_List_List*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));({void*_tmpA2A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5A6=_region_malloc(yyr,sizeof(*_tmp5A6));_tmp5A6->tag=3U,({void*_tmpA29=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A5=_region_malloc(yyr,sizeof(*_tmp5A5));_tmp5A5->tag=1U,_tmp5A5->f1=_tmp5AC,_tmp5A5->f2=_tmp5AB,_tmp5A5->f3=_tmp5AA,_tmp5A5->f4=_tmp5A9,_tmp5A5->f5=_tmp5A8,({
struct Cyc_Absyn_Exp*_tmpA28=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5A5->f6=_tmpA28;}),({struct Cyc_Absyn_Exp*_tmpA27=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5A5->f7=_tmpA27;});_tmp5A5;});
# 2418
_tmp5A6->f1=_tmpA29;});_tmp5A6;});_tmp5A7->hd=_tmpA2A;}),({
struct Cyc_List_List*_tmpA26=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5A7->tl=_tmpA26;});_tmp5A7;});
# 2418
_tmp728.tms=_tmpA2B;});_tmp728;}));
# 2421
goto _LL0;}case 286U: _LL237: _LL238: {
# 2423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2423 "parse.y"
struct Cyc_List_List*_tmp5AD=({unsigned int _tmpA2C=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA2C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp729;({struct Cyc_List_List*_tmpA2F=({struct Cyc_List_List*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));({void*_tmpA2E=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));_tmp5AE->tag=4U,_tmp5AE->f1=_tmp5AD,_tmp5AE->f2=(unsigned int)(yyyylsp[1]).first_line,_tmp5AE->f3=0;_tmp5AE;});_tmp5AF->hd=_tmpA2E;}),({
struct Cyc_List_List*_tmpA2D=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5AF->tl=_tmpA2D;});_tmp5AF;});
# 2424
_tmp729.tms=_tmpA2F;});_tmp729;}));
# 2427
goto _LL0;}case 287U: _LL239: _LL23A: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2428 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72A;({struct Cyc_List_List*_tmpA33=({struct Cyc_List_List*_tmp5B1=_region_malloc(yyr,sizeof(*_tmp5B1));({void*_tmpA32=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5B0=_region_malloc(yyr,sizeof(*_tmp5B0));_tmp5B0->tag=5U,_tmp5B0->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA31=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5B0->f2=_tmpA31;});_tmp5B0;});_tmp5B1->hd=_tmpA32;}),({struct Cyc_List_List*_tmpA30=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5B1->tl=_tmpA30;});_tmp5B1;});_tmp72A.tms=_tmpA33;});_tmp72A;}));
# 2430
goto _LL0;}case 288U: _LL23B: _LL23C: {
# 2432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2434 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 289U: _LL23D: _LL23E: {
# 2437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2435 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 290U: _LL23F: _LL240: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2436 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 291U: _LL241: _LL242: {
# 2439
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2437 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 292U: _LL243: _LL244: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2438 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 293U: _LL245: _LL246: {
# 2441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2439 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 294U: _LL247: _LL248: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
yyval=Cyc_Stmt_tok(({void*_tmpA37=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5B3=_cycalloc(sizeof(*_tmp5B3));_tmp5B3->tag=13U,({struct _dyneither_ptr*_tmpA36=({struct _dyneither_ptr*_tmp5B2=_cycalloc(sizeof(*_tmp5B2));({struct _dyneither_ptr _tmpA35=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5B2=_tmpA35;});_tmp5B2;});_tmp5B3->f1=_tmpA36;}),({struct Cyc_Absyn_Stmt*_tmpA34=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5B3->f2=_tmpA34;});_tmp5B3;});Cyc_Absyn_new_stmt(_tmpA37,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 295U: _LL249: _LL24A: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2449 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 296U: _LL24B: _LL24C: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2450 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA38=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpA38,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 297U: _LL24D: _LL24E: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2455 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 298U: _LL24F: _LL250: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2456 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 299U: _LL251: _LL252: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2461 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA39=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpA39,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));
goto _LL0;}case 300U: _LL253: _LL254: {
# 2464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2462 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA3A=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpA3A,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));
goto _LL0;}case 301U: _LL255: _LL256: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2463 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA3F=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->tag=13U,({struct _dyneither_ptr*_tmpA3E=({struct _dyneither_ptr*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));({struct _dyneither_ptr _tmpA3D=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5B4=_tmpA3D;});_tmp5B4;});_tmp5B5->f1=_tmpA3E;}),({struct Cyc_Absyn_Stmt*_tmpA3C=({struct Cyc_List_List*_tmpA3B=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpA3B,Cyc_Absyn_skip_stmt(0U));});_tmp5B5->f2=_tmpA3C;});_tmp5B5;});Cyc_Absyn_new_stmt(_tmpA3F,(unsigned int)(yyyylsp[0]).first_line);}));
# 2465
goto _LL0;}case 302U: _LL257: _LL258: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2465 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA44=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->tag=13U,({struct _dyneither_ptr*_tmpA43=({struct _dyneither_ptr*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));({struct _dyneither_ptr _tmpA42=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5B6=_tmpA42;});_tmp5B6;});_tmp5B7->f1=_tmpA43;}),({struct Cyc_Absyn_Stmt*_tmpA41=({struct Cyc_List_List*_tmpA40=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpA40,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp5B7->f2=_tmpA41;});_tmp5B7;});Cyc_Absyn_new_stmt(_tmpA44,(unsigned int)(yyyylsp[0]).first_line);}));
# 2467
goto _LL0;}case 303U: _LL259: _LL25A: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2467 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 304U: _LL25B: _LL25C: {
# 2470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2468 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA46=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpA45=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpA46,_tmpA45,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 305U: _LL25D: _LL25E: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2469 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA49=({void*_tmpA48=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA47=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5B8->f1=_tmpA47;});_tmp5B8;});Cyc_Absyn_new_decl(_tmpA48,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpA49,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;}case 306U: _LL25F: _LL260: {
# 2473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2472 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA4C=({void*_tmpA4B=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA4A=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5B9->f1=_tmpA4A;});_tmp5B9;});Cyc_Absyn_new_decl(_tmpA4B,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpA4C,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));
goto _LL0;}case 307U: _LL261: _LL262: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2477 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA4F=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA4E=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA4D=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA4F,_tmpA4E,_tmpA4D,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 308U: _LL263: _LL264: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2479 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA52=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA51=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA50=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpA52,_tmpA51,_tmpA50,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 309U: _LL265: _LL266: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA54=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpA53=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpA54,_tmpA53,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 310U: _LL267: _LL268: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2488 "parse.y"
struct Cyc_Absyn_Exp*_tmp5BA=({struct _tuple0*_tmpA55=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpA55,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA57=_tmp5BA;struct Cyc_List_List*_tmpA56=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpA57,_tmpA56,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 311U: _LL269: _LL26A: {
# 2492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2492 "parse.y"
struct Cyc_Absyn_Exp*_tmp5BB=({struct Cyc_List_List*_tmpA58=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpA58,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA5A=_tmp5BB;struct Cyc_List_List*_tmpA59=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpA5A,_tmpA59,(unsigned int)(yyyylsp[0]).first_line);}));
# 2495
goto _LL0;}case 312U: _LL26B: _LL26C: {
# 2497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2498 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA5C=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpA5B=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpA5C,_tmpA5B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 313U: _LL26D: _LL26E:
# 2501
 yyval=Cyc_YY8(0);
goto _LL0;case 314U: _LL26F: _LL270: {
# 2504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2516 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));({struct Cyc_Absyn_Switch_clause*_tmpA60=({struct Cyc_Absyn_Switch_clause*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));({struct Cyc_Absyn_Pat*_tmpA5F=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp5BC->pattern=_tmpA5F;}),_tmp5BC->pat_vars=0,_tmp5BC->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA5E=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5BC->body=_tmpA5E;}),_tmp5BC->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5BC;});
# 2516
_tmp5BD->hd=_tmpA60;}),({
# 2518
struct Cyc_List_List*_tmpA5D=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5BD->tl=_tmpA5D;});_tmp5BD;}));
goto _LL0;}case 315U: _LL271: _LL272: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2520 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));({struct Cyc_Absyn_Switch_clause*_tmpA64=({struct Cyc_Absyn_Switch_clause*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));({struct Cyc_Absyn_Pat*_tmpA63=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5BE->pattern=_tmpA63;}),_tmp5BE->pat_vars=0,_tmp5BE->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA62=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp5BE->body=_tmpA62;}),_tmp5BE->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5BE;});
# 2520
_tmp5BF->hd=_tmpA64;}),({
# 2522
struct Cyc_List_List*_tmpA61=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5BF->tl=_tmpA61;});_tmp5BF;}));
goto _LL0;}case 316U: _LL273: _LL274: {
# 2525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2524 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));({struct Cyc_Absyn_Switch_clause*_tmpA68=({struct Cyc_Absyn_Switch_clause*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));({struct Cyc_Absyn_Pat*_tmpA67=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5C0->pattern=_tmpA67;}),_tmp5C0->pat_vars=0,_tmp5C0->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpA66=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp5C0->body=_tmpA66;}),_tmp5C0->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5C0;});_tmp5C1->hd=_tmpA68;}),({struct Cyc_List_List*_tmpA65=Cyc_yyget_YY8(yyyyvsp[4]);_tmp5C1->tl=_tmpA65;});_tmp5C1;}));
goto _LL0;}case 317U: _LL275: _LL276: {
# 2527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2526 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));({struct Cyc_Absyn_Switch_clause*_tmpA6D=({struct Cyc_Absyn_Switch_clause*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));({struct Cyc_Absyn_Pat*_tmpA6C=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5C2->pattern=_tmpA6C;}),_tmp5C2->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5C2->where_clause=_tmpA6B;}),({
struct Cyc_Absyn_Stmt*_tmpA6A=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp5C2->body=_tmpA6A;}),_tmp5C2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5C2;});
# 2526
_tmp5C3->hd=_tmpA6D;}),({
# 2528
struct Cyc_List_List*_tmpA69=Cyc_yyget_YY8(yyyyvsp[5]);_tmp5C3->tl=_tmpA69;});_tmp5C3;}));
goto _LL0;}case 318U: _LL277: _LL278: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2530 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));({struct Cyc_Absyn_Switch_clause*_tmpA72=({struct Cyc_Absyn_Switch_clause*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));({struct Cyc_Absyn_Pat*_tmpA71=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5C4->pattern=_tmpA71;}),_tmp5C4->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA70=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5C4->where_clause=_tmpA70;}),({struct Cyc_Absyn_Stmt*_tmpA6F=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp5C4->body=_tmpA6F;}),_tmp5C4->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5C4;});_tmp5C5->hd=_tmpA72;}),({struct Cyc_List_List*_tmpA6E=Cyc_yyget_YY8(yyyyvsp[6]);_tmp5C5->tl=_tmpA6E;});_tmp5C5;}));
goto _LL0;}case 319U: _LL279: _LL27A: {
# 2533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2537 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA74=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA73=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpA74,_tmpA73,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2540
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2541 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA76=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpA75=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpA76,_tmpA75,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2545 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA79=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA78=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA77=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2545
Cyc_Absyn_for_stmt(_tmpA7A,_tmpA79,_tmpA78,_tmpA77,(unsigned int)(yyyylsp[0]).first_line);}));
# 2547
goto _LL0;}case 322U: _LL27F: _LL280: {
# 2549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2548 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA7C=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA7B=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2548
Cyc_Absyn_for_stmt(_tmpA7E,_tmpA7D,_tmpA7C,_tmpA7B,(unsigned int)(yyyylsp[0]).first_line);}));
# 2550
goto _LL0;}case 323U: _LL281: _LL282: {
# 2552
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2551 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA82=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA81=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA80=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA7F=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2551
Cyc_Absyn_for_stmt(_tmpA82,_tmpA81,_tmpA80,_tmpA7F,(unsigned int)(yyyylsp[0]).first_line);}));
# 2553
goto _LL0;}case 324U: _LL283: _LL284: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2554 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA86=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA85=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA84=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpA83=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2554
Cyc_Absyn_for_stmt(_tmpA86,_tmpA85,_tmpA84,_tmpA83,(unsigned int)(yyyylsp[0]).first_line);}));
# 2556
goto _LL0;}case 325U: _LL285: _LL286: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2557 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA89=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA88=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA87=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2557
Cyc_Absyn_for_stmt(_tmpA8A,_tmpA89,_tmpA88,_tmpA87,(unsigned int)(yyyylsp[0]).first_line);}));
# 2559
goto _LL0;}case 326U: _LL287: _LL288: {
# 2561
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2560 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA8E=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA8D=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA8C=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpA8B=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2560
Cyc_Absyn_for_stmt(_tmpA8E,_tmpA8D,_tmpA8C,_tmpA8B,(unsigned int)(yyyylsp[0]).first_line);}));
# 2562
goto _LL0;}case 327U: _LL289: _LL28A: {
# 2564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2563 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA92=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA91=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpA90=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA8F=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2563
Cyc_Absyn_for_stmt(_tmpA92,_tmpA91,_tmpA90,_tmpA8F,(unsigned int)(yyyylsp[0]).first_line);}));
# 2565
goto _LL0;}case 328U: _LL28B: _LL28C: {
# 2567
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 2569
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 2566 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA96=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpA95=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpA94=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpA93=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2566
Cyc_Absyn_for_stmt(_tmpA96,_tmpA95,_tmpA94,_tmpA93,(unsigned int)(yyyylsp[0]).first_line);}));
# 2568
goto _LL0;}case 329U: _LL28D: _LL28E: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2569 "parse.y"
struct Cyc_List_List*_tmp5C6=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5C7=({struct Cyc_Absyn_Exp*_tmpA9A=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA99=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA98=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA97=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2570
Cyc_Absyn_for_stmt(_tmpA9A,_tmpA99,_tmpA98,_tmpA97,(unsigned int)(yyyylsp[0]).first_line);});
# 2572
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5C6,_tmp5C7));
# 2574
goto _LL0;}case 330U: _LL28F: _LL290: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2575 "parse.y"
struct Cyc_List_List*_tmp5C8=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5C9=({struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA9D=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA9C=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA9B=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2576
Cyc_Absyn_for_stmt(_tmpA9E,_tmpA9D,_tmpA9C,_tmpA9B,(unsigned int)(yyyylsp[0]).first_line);});
# 2578
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5C8,_tmp5C9));
# 2580
goto _LL0;}case 331U: _LL291: _LL292: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2581 "parse.y"
struct Cyc_List_List*_tmp5CA=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5CB=({struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAA0=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA9F=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2582
Cyc_Absyn_for_stmt(_tmpAA2,_tmpAA1,_tmpAA0,_tmpA9F,(unsigned int)(yyyylsp[0]).first_line);});
# 2584
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5CA,_tmp5CB));
# 2586
goto _LL0;}case 332U: _LL293: _LL294: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2587 "parse.y"
struct Cyc_List_List*_tmp5CC=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5CD=({struct Cyc_Absyn_Exp*_tmpAA6=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAA5=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpAA4=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpAA3=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2588
Cyc_Absyn_for_stmt(_tmpAA6,_tmpAA5,_tmpAA4,_tmpAA3,(unsigned int)(yyyylsp[0]).first_line);});
# 2590
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5CC,_tmp5CD));
# 2592
goto _LL0;}case 333U: _LL295: _LL296: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
yyval=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpAA8=({struct _dyneither_ptr*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));({struct _dyneither_ptr _tmpAA7=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp5CE=_tmpAA7;});_tmp5CE;});Cyc_Absyn_goto_stmt(_tmpAA8,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 334U: _LL297: _LL298: {
# 2600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2598 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 335U: _LL299: _LL29A: {
# 2601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2599 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 336U: _LL29B: _LL29C: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2600 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 337U: _LL29D: _LL29E: {
# 2603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2601 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA9=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpAA9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 338U: _LL29F: _LL2A0: {
# 2604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2603 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2604 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2606 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAAA=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpAAA,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2615 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2618 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 343U: _LL2A9: _LL2AA: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2620 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAAC=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpAAB=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpAAD,_tmpAAC,_tmpAAB,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 344U: _LL2AB: _LL2AC: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2623 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 345U: _LL2AD: _LL2AE: {
# 2626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2625 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAAF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAAE=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpAAF,_tmpAAE,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 346U: _LL2AF: _LL2B0: {
# 2628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2628 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 347U: _LL2B1: _LL2B2: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2630 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB0=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpAB1,_tmpAB0,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 348U: _LL2B3: _LL2B4: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2633 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 349U: _LL2B5: _LL2B6: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2635 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB2=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpAB3,_tmpAB2,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 350U: _LL2B7: _LL2B8: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2638 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 351U: _LL2B9: _LL2BA: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2640 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB5=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB4=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpAB5,_tmpAB4,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 352U: _LL2BB: _LL2BC: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2643 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 353U: _LL2BD: _LL2BE: {
# 2646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2645 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB6=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpAB7,_tmpAB6,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 354U: _LL2BF: _LL2C0: {
# 2648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2648 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 355U: _LL2C1: _LL2C2: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2650 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAB9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAB8=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpAB9,_tmpAB8,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 356U: _LL2C3: _LL2C4: {
# 2653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2652 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpABB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpABA=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpABB,_tmpABA,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 357U: _LL2C5: _LL2C6: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2655 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 358U: _LL2C7: _LL2C8: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2657 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpABD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpABC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpABD,_tmpABC,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 359U: _LL2C9: _LL2CA: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2659 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpABF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpABE=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpABF,_tmpABE,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 360U: _LL2CB: _LL2CC: {
# 2662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2661 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC0=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpAC1,_tmpAC0,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 361U: _LL2CD: _LL2CE: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2663 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC3=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpAC3,_tmpAC2,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 362U: _LL2CF: _LL2D0: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2666 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 363U: _LL2D1: _LL2D2: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2668 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC5=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC4=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpAC5,_tmpAC4,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 364U: _LL2D3: _LL2D4: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2670 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC7=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC6=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpAC7,_tmpAC6,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 365U: _LL2D5: _LL2D6: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2673 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 366U: _LL2D7: _LL2D8: {
# 2676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2675 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC9=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC8=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpAC9,_tmpAC8,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 367U: _LL2D9: _LL2DA: {
# 2678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2677 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACB=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACA=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpACB,_tmpACA,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 368U: _LL2DB: _LL2DC: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2680 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 369U: _LL2DD: _LL2DE: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2682 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACD=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpACD,_tmpACC,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 370U: _LL2DF: _LL2E0: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2684 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACF=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACE=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpACF,_tmpACE,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 371U: _LL2E1: _LL2E2: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2686 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD1=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD0=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpAD1,_tmpAD0,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 372U: _LL2E3: _LL2E4: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2689 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 373U: _LL2E5: _LL2E6: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2691 "parse.y"
void*_tmp5CF=({struct _tuple8*_tmpAD2=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpAD2,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpAD4=_tmp5CF;struct Cyc_Absyn_Exp*_tmpAD3=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpAD4,_tmpAD3,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));
# 2694
goto _LL0;}case 374U: _LL2E7: _LL2E8: {
# 2696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2697 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 375U: _LL2E9: _LL2EA: {
# 2700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2700 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 376U: _LL2EB: _LL2EC: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2702 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpAD6=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpAD5=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpAD6,_tmpAD5,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 377U: _LL2ED: _LL2EE: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2704 "parse.y"
void*_tmp5D0=({struct _tuple8*_tmpAD7=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpAD7,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(_tmp5D0,(unsigned int)(yyyylsp[0]).first_line)));
# 2707
goto _LL0;}case 378U: _LL2EF: _LL2F0: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2708 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpAD8,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 379U: _LL2F1: _LL2F2: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2710 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpADA=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpAD9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpADA,_tmpAD9,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 380U: _LL2F3: _LL2F4: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2715 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 381U: _LL2F5: _LL2F6: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2723 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 382U: _LL2F7: _LL2F8: {
# 2726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2728 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 383U: _LL2F9: _LL2FA: {
# 2731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2730 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 384U: _LL2FB: _LL2FC: {
# 2733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2732 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp5D1=e->r;void*_tmp5D2=_tmp5D1;struct _dyneither_ptr _tmp5E6;int _tmp5E5;enum Cyc_Absyn_Sign _tmp5E4;int _tmp5E3;enum Cyc_Absyn_Sign _tmp5E2;short _tmp5E1;enum Cyc_Absyn_Sign _tmp5E0;char _tmp5DF;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).LongLong_c).tag){case 2U: _LL477: _tmp5E0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Char_c).val).f1;_tmp5DF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Char_c).val).f2;_LL478:
# 2736
 yyval=Cyc_YY9(({void*_tmpADB=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->tag=11U,_tmp5D3->f1=_tmp5DF;_tmp5D3;});Cyc_Absyn_new_pat(_tmpADB,e->loc);}));goto _LL476;case 4U: _LL479: _tmp5E2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Short_c).val).f1;_tmp5E1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Short_c).val).f2;_LL47A:
# 2738
 yyval=Cyc_YY9(({void*_tmpADC=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->tag=10U,_tmp5D4->f1=_tmp5E2,_tmp5D4->f2=(int)_tmp5E1;_tmp5D4;});Cyc_Absyn_new_pat(_tmpADC,e->loc);}));goto _LL476;case 5U: _LL47B: _tmp5E4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Int_c).val).f1;_tmp5E3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Int_c).val).f2;_LL47C:
# 2740
 yyval=Cyc_YY9(({void*_tmpADD=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->tag=10U,_tmp5D5->f1=_tmp5E4,_tmp5D5->f2=_tmp5E3;_tmp5D5;});Cyc_Absyn_new_pat(_tmpADD,e->loc);}));goto _LL476;case 7U: _LL47D: _tmp5E6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Float_c).val).f1;_tmp5E5=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5D2)->f1).Float_c).val).f2;_LL47E:
# 2742
 yyval=Cyc_YY9(({void*_tmpADE=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));_tmp5D6->tag=12U,_tmp5D6->f1=_tmp5E6,_tmp5D6->f2=_tmp5E5;_tmp5D6;});Cyc_Absyn_new_pat(_tmpADE,e->loc);}));goto _LL476;case 1U: _LL47F: _LL480:
# 2744
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL476;case 8U: _LL481: _LL482:
# 2746
({void*_tmp5D7=0U;({unsigned int _tmpAE0=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpADF=({const char*_tmp5D8="strings cannot occur within patterns";_tag_dyneither(_tmp5D8,sizeof(char),37U);});Cyc_Warn_err(_tmpAE0,_tmpADF,_tag_dyneither(_tmp5D7,sizeof(void*),0U));});});goto _LL476;case 9U: _LL483: _LL484:
# 2748
({void*_tmp5D9=0U;({unsigned int _tmpAE2=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAE1=({const char*_tmp5DA="strings cannot occur within patterns";_tag_dyneither(_tmp5DA,sizeof(char),37U);});Cyc_Warn_err(_tmpAE2,_tmpAE1,_tag_dyneither(_tmp5D9,sizeof(void*),0U));});});goto _LL476;case 6U: _LL485: _LL486:
# 2750
({void*_tmp5DB=0U;({unsigned int _tmpAE4=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAE3=({const char*_tmp5DC="long long's in patterns not yet implemented";_tag_dyneither(_tmp5DC,sizeof(char),44U);});Cyc_Warn_err(_tmpAE4,_tmpAE3,_tag_dyneither(_tmp5DB,sizeof(void*),0U));});});goto _LL476;default: goto _LL487;}else{_LL487: _LL488:
# 2752
({void*_tmp5DD=0U;({unsigned int _tmpAE6=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAE5=({const char*_tmp5DE="bad constant in case";_tag_dyneither(_tmp5DE,sizeof(char),21U);});Cyc_Warn_err(_tmpAE6,_tmpAE5,_tag_dyneither(_tmp5DD,sizeof(void*),0U));});});}_LL476:;}
# 2755
goto _LL0;}case 385U: _LL2FD: _LL2FE: {
# 2757
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2756 "parse.y"
yyval=Cyc_YY9(({void*_tmpAE8=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->tag=15U,({struct _tuple0*_tmpAE7=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5E7->f1=_tmpAE7;});_tmp5E7;});Cyc_Absyn_new_pat(_tmpAE8,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 386U: _LL2FF: _LL300: {
# 2759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2758 "parse.y"
if(({struct _dyneither_ptr _tmpAE9=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpAE9,({const char*_tmp5E8="as";_tag_dyneither(_tmp5E8,sizeof(char),3U);}));})!= 0)
({void*_tmp5E9=0U;({unsigned int _tmpAEB=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpAEA=({const char*_tmp5EA="expecting `as'";_tag_dyneither(_tmp5EA,sizeof(char),15U);});Cyc_Warn_err(_tmpAEB,_tmpAEA,_tag_dyneither(_tmp5E9,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpAF2=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpAF1=({unsigned int _tmpAF0=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpAEF=({struct _tuple0*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpAEE=({struct _dyneither_ptr*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));({struct _dyneither_ptr _tmpAED=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5EB=_tmpAED;});_tmp5EB;});_tmp5EC->f2=_tmpAEE;});_tmp5EC;});Cyc_Absyn_new_vardecl(_tmpAF0,_tmpAEF,Cyc_Absyn_void_type,0);});_tmp5ED->f1=_tmpAF1;}),({
struct Cyc_Absyn_Pat*_tmpAEC=Cyc_yyget_YY9(yyyyvsp[2]);_tmp5ED->f2=_tmpAEC;});_tmp5ED;});
# 2760
Cyc_Absyn_new_pat(_tmpAF2,(unsigned int)(yyyylsp[0]).first_line);}));
# 2763
goto _LL0;}case 387U: _LL301: _LL302: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2764 "parse.y"
if(({struct _dyneither_ptr _tmpAF3=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpAF3,({const char*_tmp5EE="alias";_tag_dyneither(_tmp5EE,sizeof(char),6U);}));})!= 0)
({void*_tmp5EF=0U;({unsigned int _tmpAF5=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpAF4=({const char*_tmp5F0="expecting `alias'";_tag_dyneither(_tmp5F0,sizeof(char),18U);});Cyc_Warn_err(_tmpAF5,_tmpAF4,_tag_dyneither(_tmp5EF,sizeof(void*),0U));});});{
int _tmp5F1=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({struct _dyneither_ptr*_tmpAF8=({struct _dyneither_ptr*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));({struct _dyneither_ptr _tmpAF7=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp5F5=_tmpAF7;});_tmp5F5;});_tmp5F7->name=_tmpAF8;}),_tmp5F7->identity=- 1,({void*_tmpAF6=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->tag=0U,_tmp5F6->f1=& Cyc_Tcutil_rk;_tmp5F6;});_tmp5F7->kind=_tmpAF6;});_tmp5F7;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpAFD=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpAFC=({struct _tuple0*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));_tmp5F4->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpAFA=({struct _dyneither_ptr*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));({struct _dyneither_ptr _tmpAF9=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp5F3=_tmpAF9;});_tmp5F3;});_tmp5F4->f2=_tmpAFA;});_tmp5F4;});Cyc_Absyn_new_vardecl(_tmpAFD,_tmpAFC,({
struct _tuple8*_tmpAFB=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpAFB,(unsigned int)(yyyylsp[4]).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpAFE=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->tag=2U,_tmp5F2->f1=tv,_tmp5F2->f2=vd;_tmp5F2;});Cyc_Absyn_new_pat(_tmpAFE,(unsigned int)_tmp5F1);}));
# 2772
goto _LL0;};}case 388U: _LL303: _LL304: {
# 2774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2773 "parse.y"
if(({struct _dyneither_ptr _tmpAFF=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpAFF,({const char*_tmp5F8="alias";_tag_dyneither(_tmp5F8,sizeof(char),6U);}));})!= 0)
({void*_tmp5F9=0U;({unsigned int _tmpB01=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB00=({const char*_tmp5FA="expecting `alias'";_tag_dyneither(_tmp5FA,sizeof(char),18U);});Cyc_Warn_err(_tmpB01,_tmpB00,_tag_dyneither(_tmp5F9,sizeof(void*),0U));});});{
int _tmp5FB=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp601=_cycalloc(sizeof(*_tmp601));({struct _dyneither_ptr*_tmpB04=({struct _dyneither_ptr*_tmp5FF=_cycalloc(sizeof(*_tmp5FF));({struct _dyneither_ptr _tmpB03=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp5FF=_tmpB03;});_tmp5FF;});_tmp601->name=_tmpB04;}),_tmp601->identity=- 1,({void*_tmpB02=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=0U,_tmp600->f1=& Cyc_Tcutil_rk;_tmp600;});_tmp601->kind=_tmpB02;});_tmp601;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpB09=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB08=({struct _tuple0*_tmp5FE=_cycalloc(sizeof(*_tmp5FE));_tmp5FE->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB06=({struct _dyneither_ptr*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));({struct _dyneither_ptr _tmpB05=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp5FD=_tmpB05;});_tmp5FD;});_tmp5FE->f2=_tmpB06;});_tmp5FE;});Cyc_Absyn_new_vardecl(_tmpB09,_tmpB08,({
struct _tuple8*_tmpB07=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpB07,(unsigned int)(yyyylsp[4]).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB0A=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->tag=2U,_tmp5FC->f1=tv,_tmp5FC->f2=vd;_tmp5FC;});Cyc_Absyn_new_pat(_tmpB0A,(unsigned int)_tmp5FB);}));
# 2781
goto _LL0;};}case 389U: _LL305: _LL306: {
# 2783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2782 "parse.y"
struct _tuple22 _tmp602=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp603=_tmp602;struct Cyc_List_List*_tmp606;int _tmp605;_LL48A: _tmp606=_tmp603.f1;_tmp605=_tmp603.f2;_LL48B:;
yyval=Cyc_YY9(({void*_tmpB0B=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->tag=5U,_tmp604->f1=_tmp606,_tmp604->f2=_tmp605;_tmp604;});Cyc_Absyn_new_pat(_tmpB0B,(unsigned int)(yyyylsp[0]).first_line);}));
# 2785
goto _LL0;}case 390U: _LL307: _LL308: {
# 2787
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2789
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2786 "parse.y"
struct _tuple22 _tmp607=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp608=_tmp607;struct Cyc_List_List*_tmp60B;int _tmp60A;_LL48D: _tmp60B=_tmp608.f1;_tmp60A=_tmp608.f2;_LL48E:;
yyval=Cyc_YY9(({void*_tmpB0D=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->tag=16U,({struct _tuple0*_tmpB0C=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp609->f1=_tmpB0C;}),_tmp609->f2=_tmp60B,_tmp609->f3=_tmp60A;_tmp609;});Cyc_Absyn_new_pat(_tmpB0D,(unsigned int)(yyyylsp[0]).first_line);}));
# 2789
goto _LL0;}case 391U: _LL309: _LL30A: {
# 2791
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2790 "parse.y"
struct _tuple22 _tmp60C=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp60D=_tmp60C;struct Cyc_List_List*_tmp612;int _tmp611;_LL490: _tmp612=_tmp60D.f1;_tmp611=_tmp60D.f2;_LL491:;{
struct Cyc_List_List*_tmp60E=({unsigned int _tmpB0E=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB0E,Cyc_yyget_YY40(yyyyvsp[2]));});
yyval=Cyc_YY9(({void*_tmpB11=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB10=({union Cyc_Absyn_AggrInfo*_tmp60F=_cycalloc(sizeof(*_tmp60F));({union Cyc_Absyn_AggrInfo _tmpB0F=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);*_tmp60F=_tmpB0F;});_tmp60F;});_tmp610->f1=_tmpB10;}),_tmp610->f2=_tmp60E,_tmp610->f3=_tmp612,_tmp610->f4=_tmp611;_tmp610;});Cyc_Absyn_new_pat(_tmpB11,(unsigned int)(yyyylsp[0]).first_line);}));
# 2795
goto _LL0;};}case 392U: _LL30B: _LL30C: {
# 2797
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2796 "parse.y"
struct _tuple22 _tmp613=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp614=_tmp613;struct Cyc_List_List*_tmp618;int _tmp617;_LL493: _tmp618=_tmp614.f1;_tmp617=_tmp614.f2;_LL494:;{
struct Cyc_List_List*_tmp615=({unsigned int _tmpB12=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB12,Cyc_yyget_YY40(yyyyvsp[1]));});
yyval=Cyc_YY9(({void*_tmpB13=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=7U,_tmp616->f1=0,_tmp616->f2=_tmp615,_tmp616->f3=_tmp618,_tmp616->f4=_tmp617;_tmp616;});Cyc_Absyn_new_pat(_tmpB13,(unsigned int)(yyyylsp[0]).first_line);}));
# 2800
goto _LL0;};}case 393U: _LL30D: _LL30E: {
# 2802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2801 "parse.y"
yyval=Cyc_YY9(({void*_tmpB15=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp619=_cycalloc(sizeof(*_tmp619));_tmp619->tag=6U,({struct Cyc_Absyn_Pat*_tmpB14=Cyc_yyget_YY9(yyyyvsp[1]);_tmp619->f1=_tmpB14;});_tmp619;});Cyc_Absyn_new_pat(_tmpB15,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 394U: _LL30F: _LL310: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2803 "parse.y"
yyval=Cyc_YY9(({void*_tmpB19=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->tag=6U,({struct Cyc_Absyn_Pat*_tmpB18=({void*_tmpB17=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=6U,({struct Cyc_Absyn_Pat*_tmpB16=Cyc_yyget_YY9(yyyyvsp[1]);_tmp61A->f1=_tmpB16;});_tmp61A;});Cyc_Absyn_new_pat(_tmpB17,(unsigned int)(yyyylsp[0]).first_line);});_tmp61B->f1=_tmpB18;});_tmp61B;});Cyc_Absyn_new_pat(_tmpB19,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 395U: _LL311: _LL312: {
# 2806
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2805 "parse.y"
yyval=Cyc_YY9(({void*_tmpB20=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB1F=({unsigned int _tmpB1E=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB1D=({struct _tuple0*_tmp61D=_cycalloc(sizeof(*_tmp61D));_tmp61D->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB1C=({struct _dyneither_ptr*_tmp61C=_cycalloc(sizeof(*_tmp61C));({struct _dyneither_ptr _tmpB1B=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp61C=_tmpB1B;});_tmp61C;});_tmp61D->f2=_tmpB1C;});_tmp61D;});Cyc_Absyn_new_vardecl(_tmpB1E,_tmpB1D,Cyc_Absyn_void_type,0);});_tmp61E->f1=_tmpB1F;}),({
# 2807
struct Cyc_Absyn_Pat*_tmpB1A=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp61E->f2=_tmpB1A;});_tmp61E;});
# 2805
Cyc_Absyn_new_pat(_tmpB20,(unsigned int)(yyyylsp[0]).first_line);}));
# 2809
goto _LL0;}case 396U: _LL313: _LL314: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2810 "parse.y"
if(({struct _dyneither_ptr _tmpB21=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpB21,({const char*_tmp61F="as";_tag_dyneither(_tmp61F,sizeof(char),3U);}));})!= 0)
({void*_tmp620=0U;({unsigned int _tmpB23=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpB22=({const char*_tmp621="expecting `as'";_tag_dyneither(_tmp621,sizeof(char),15U);});Cyc_Warn_err(_tmpB23,_tmpB22,_tag_dyneither(_tmp620,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB2A=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB29=({unsigned int _tmpB28=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB27=({struct _tuple0*_tmp623=_cycalloc(sizeof(*_tmp623));_tmp623->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB26=({struct _dyneither_ptr*_tmp622=_cycalloc(sizeof(*_tmp622));({struct _dyneither_ptr _tmpB25=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp622=_tmpB25;});_tmp622;});_tmp623->f2=_tmpB26;});_tmp623;});Cyc_Absyn_new_vardecl(_tmpB28,_tmpB27,Cyc_Absyn_void_type,0);});_tmp624->f1=_tmpB29;}),({
# 2814
struct Cyc_Absyn_Pat*_tmpB24=Cyc_yyget_YY9(yyyyvsp[3]);_tmp624->f2=_tmpB24;});_tmp624;});
# 2812
Cyc_Absyn_new_pat(_tmpB2A,(unsigned int)(yyyylsp[0]).first_line);}));
# 2816
goto _LL0;}case 397U: _LL315: _LL316: {
# 2818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2817 "parse.y"
void*_tmp625=({struct _dyneither_ptr _tmpB2B=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpB2B,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
yyval=Cyc_YY9(({void*_tmpB32=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB31=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp625);_tmp628->f1=_tmpB31;}),({
struct Cyc_Absyn_Vardecl*_tmpB30=({unsigned int _tmpB2F=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB2E=({struct _tuple0*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB2D=({struct _dyneither_ptr*_tmp626=_cycalloc(sizeof(*_tmp626));({struct _dyneither_ptr _tmpB2C=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp626=_tmpB2C;});_tmp626;});_tmp627->f2=_tmpB2D;});_tmp627;});Cyc_Absyn_new_vardecl(_tmpB2F,_tmpB2E,
Cyc_Absyn_tag_type(_tmp625),0);});
# 2819
_tmp628->f2=_tmpB30;});_tmp628;});
# 2818
Cyc_Absyn_new_pat(_tmpB32,(unsigned int)(yyyylsp[0]).first_line);}));
# 2822
goto _LL0;}case 398U: _LL317: _LL318: {
# 2824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2823 "parse.y"
struct Cyc_Absyn_Tvar*_tmp629=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(({void*_tmpB38=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=4U,_tmp62C->f1=_tmp629,({
struct Cyc_Absyn_Vardecl*_tmpB37=({unsigned int _tmpB36=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB35=({struct _tuple0*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB34=({struct _dyneither_ptr*_tmp62A=_cycalloc(sizeof(*_tmp62A));({struct _dyneither_ptr _tmpB33=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp62A=_tmpB33;});_tmp62A;});_tmp62B->f2=_tmpB34;});_tmp62B;});Cyc_Absyn_new_vardecl(_tmpB36,_tmpB35,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(_tmp629)),0);});
# 2825
_tmp62C->f2=_tmpB37;});_tmp62C;});
# 2824
Cyc_Absyn_new_pat(_tmpB38,(unsigned int)(yyyylsp[0]).first_line);}));
# 2828
goto _LL0;}case 399U: _LL319: _LL31A: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2831 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp62D=_cycalloc(sizeof(*_tmp62D));({struct Cyc_List_List*_tmpB39=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp62D->f1=_tmpB39;}),_tmp62D->f2=0;_tmp62D;}));
goto _LL0;}case 400U: _LL31B: _LL31C: {
# 2834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2832 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp62E=_cycalloc(sizeof(*_tmp62E));({struct Cyc_List_List*_tmpB3A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp62E->f1=_tmpB3A;}),_tmp62E->f2=1;_tmp62E;}));
goto _LL0;}case 401U: _LL31D: _LL31E: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2833 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->f1=0,_tmp62F->f2=1;_tmp62F;}));
goto _LL0;}case 402U: _LL31F: _LL320: {
# 2836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2838 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp630=_cycalloc(sizeof(*_tmp630));({struct Cyc_Absyn_Pat*_tmpB3B=Cyc_yyget_YY9(yyyyvsp[0]);_tmp630->hd=_tmpB3B;}),_tmp630->tl=0;_tmp630;}));
goto _LL0;}case 403U: _LL321: _LL322: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2840 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp631=_cycalloc(sizeof(*_tmp631));({struct Cyc_Absyn_Pat*_tmpB3D=Cyc_yyget_YY9(yyyyvsp[2]);_tmp631->hd=_tmpB3D;}),({struct Cyc_List_List*_tmpB3C=Cyc_yyget_YY11(yyyyvsp[0]);_tmp631->tl=_tmpB3C;});_tmp631;}));
goto _LL0;}case 404U: _LL323: _LL324: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2845 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->f1=0,({struct Cyc_Absyn_Pat*_tmpB3E=Cyc_yyget_YY9(yyyyvsp[0]);_tmp632->f2=_tmpB3E;});_tmp632;}));
goto _LL0;}case 405U: _LL325: _LL326: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2847 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp633=_cycalloc(sizeof(*_tmp633));({struct Cyc_List_List*_tmpB40=Cyc_yyget_YY41(yyyyvsp[0]);_tmp633->f1=_tmpB40;}),({struct Cyc_Absyn_Pat*_tmpB3F=Cyc_yyget_YY9(yyyyvsp[1]);_tmp633->f2=_tmpB3F;});_tmp633;}));
goto _LL0;}case 406U: _LL327: _LL328: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2850 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp634=_cycalloc(sizeof(*_tmp634));({struct Cyc_List_List*_tmpB41=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp634->f1=_tmpB41;}),_tmp634->f2=0;_tmp634;}));
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2851 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp635=_cycalloc(sizeof(*_tmp635));({struct Cyc_List_List*_tmpB42=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp635->f1=_tmpB42;}),_tmp635->f2=1;_tmp635;}));
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2852 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->f1=0,_tmp636->f2=1;_tmp636;}));
goto _LL0;}case 409U: _LL32D: _LL32E: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2857 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp637=_cycalloc(sizeof(*_tmp637));({struct _tuple23*_tmpB43=Cyc_yyget_YY12(yyyyvsp[0]);_tmp637->hd=_tmpB43;}),_tmp637->tl=0;_tmp637;}));
goto _LL0;}case 410U: _LL32F: _LL330: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2859 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp638=_cycalloc(sizeof(*_tmp638));({struct _tuple23*_tmpB45=Cyc_yyget_YY12(yyyyvsp[2]);_tmp638->hd=_tmpB45;}),({struct Cyc_List_List*_tmpB44=Cyc_yyget_YY13(yyyyvsp[0]);_tmp638->tl=_tmpB44;});_tmp638;}));
goto _LL0;}case 411U: _LL331: _LL332: {
# 2862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 412U: _LL333: _LL334: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2867 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB47=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB46=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpB47,_tmpB46,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 413U: _LL335: _LL336: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2872 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 414U: _LL337: _LL338: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2874 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpB49=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpB48=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpB4A,_tmpB49,_tmpB48,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 415U: _LL339: _LL33A: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2876 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB4B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpB4C,_tmpB4B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 416U: _LL33B: _LL33C: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2880 "parse.y"
yyval=Cyc_YY7(0);
goto _LL0;}case 417U: _LL33D: _LL33E: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2881 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->v=(void*)Cyc_Absyn_Times;_tmp639;}));
goto _LL0;}case 418U: _LL33F: _LL340: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2882 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->v=(void*)Cyc_Absyn_Div;_tmp63A;}));
goto _LL0;}case 419U: _LL341: _LL342: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2883 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->v=(void*)Cyc_Absyn_Mod;_tmp63B;}));
goto _LL0;}case 420U: _LL343: _LL344: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2884 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->v=(void*)Cyc_Absyn_Plus;_tmp63C;}));
goto _LL0;}case 421U: _LL345: _LL346: {
# 2887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2885 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->v=(void*)Cyc_Absyn_Minus;_tmp63D;}));
goto _LL0;}case 422U: _LL347: _LL348: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2886 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->v=(void*)Cyc_Absyn_Bitlshift;_tmp63E;}));
goto _LL0;}case 423U: _LL349: _LL34A: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2887 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->v=(void*)Cyc_Absyn_Bitlrshift;_tmp63F;}));
goto _LL0;}case 424U: _LL34B: _LL34C: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2888 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->v=(void*)Cyc_Absyn_Bitand;_tmp640;}));
goto _LL0;}case 425U: _LL34D: _LL34E: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2889 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->v=(void*)Cyc_Absyn_Bitxor;_tmp641;}));
goto _LL0;}case 426U: _LL34F: _LL350: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2890 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->v=(void*)Cyc_Absyn_Bitor;_tmp642;}));
goto _LL0;}case 427U: _LL351: _LL352: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2895 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 428U: _LL353: _LL354: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2897 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB4F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB4E=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB4D=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpB4F,_tmpB4E,_tmpB4D,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 429U: _LL355: _LL356: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2900 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB50=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpB50,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 430U: _LL357: _LL358: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2903 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB51=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpB51,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 431U: _LL359: _LL35A: {
# 2906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2905 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB52=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpB52,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 432U: _LL35B: _LL35C: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2907 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB54=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB53=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpB54,_tmpB53,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 433U: _LL35D: _LL35E: {
# 2910
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2909 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB56=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB55=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpB56,_tmpB55,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 434U: _LL35F: _LL360: {
# 2912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2913 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 435U: _LL361: _LL362: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2917 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 436U: _LL363: _LL364: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2919 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB58=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB57=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpB58,_tmpB57,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 437U: _LL365: _LL366: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2923 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 438U: _LL367: _LL368: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2925 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB59=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpB5A,_tmpB59,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 439U: _LL369: _LL36A: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2929 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 440U: _LL36B: _LL36C: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2931 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB5B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB5C,_tmpB5B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 441U: _LL36D: _LL36E: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2935 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 442U: _LL36F: _LL370: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2937 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB5E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB5D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpB5E,_tmpB5D,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 443U: _LL371: _LL372: {
# 2940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2941 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 444U: _LL373: _LL374: {
# 2944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2943 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB60=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB5F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB60,_tmpB5F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 445U: _LL375: _LL376: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2947 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 446U: _LL377: _LL378: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2949 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB62=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB61=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpB62,_tmpB61,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 447U: _LL379: _LL37A: {
# 2952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2951 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB64=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB63=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpB64,_tmpB63,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 448U: _LL37B: _LL37C: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2955 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 449U: _LL37D: _LL37E: {
# 2958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2957 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB66=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB65=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpB66,_tmpB65,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 450U: _LL37F: _LL380: {
# 2960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2959 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB68=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB67=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpB68,_tmpB67,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 451U: _LL381: _LL382: {
# 2962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2961 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB69=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpB6A,_tmpB69,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 452U: _LL383: _LL384: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2963 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB6B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpB6C,_tmpB6B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 453U: _LL385: _LL386: {
# 2966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2967 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 454U: _LL387: _LL388: {
# 2970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2969 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB6D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB6E,_tmpB6D,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 455U: _LL389: _LL38A: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2971 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB70=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB6F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB70,_tmpB6F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 456U: _LL38B: _LL38C: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2975 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 457U: _LL38D: _LL38E: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2977 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB72=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB71=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB72,_tmpB71,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 458U: _LL38F: _LL390: {
# 2980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2979 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB74=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB73=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB74,_tmpB73,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 459U: _LL391: _LL392: {
# 2982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2983 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 460U: _LL393: _LL394: {
# 2986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2985 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB76=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB75=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB76,_tmpB75,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 461U: _LL395: _LL396: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2987 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB78=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB77=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB78,_tmpB77,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 462U: _LL397: _LL398: {
# 2990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2989 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB79=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB7A,_tmpB79,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 463U: _LL399: _LL39A: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2993 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 464U: _LL39B: _LL39C: {
# 2996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2995 "parse.y"
void*_tmp643=({struct _tuple8*_tmpB7B=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpB7B,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB7D=_tmp643;struct Cyc_Absyn_Exp*_tmpB7C=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpB7D,_tmpB7C,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));
# 2998
goto _LL0;}case 465U: _LL39D: _LL39E: {
# 3000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3001 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 466U: _LL39F: _LL3A0: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3002 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7E=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpB7E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 467U: _LL3A1: _LL3A2: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3003 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7F=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpB7F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 468U: _LL3A3: _LL3A4: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3004 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB80=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpB80,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 469U: _LL3A5: _LL3A6: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3005 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB81=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpB81,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 470U: _LL3A7: _LL3A8: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3006 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB82=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpB82,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 471U: _LL3A9: _LL3AA: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3007 "parse.y"
yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpB84=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB83=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpB84,_tmpB83,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 472U: _LL3AB: _LL3AC: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3009 "parse.y"
void*_tmp644=({struct _tuple8*_tmpB85=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpB85,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(_tmp644,(unsigned int)(yyyylsp[0]).first_line));
# 3012
goto _LL0;}case 473U: _LL3AD: _LL3AE: {
# 3014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3012 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB86=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpB86,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 474U: _LL3AF: _LL3B0: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3014 "parse.y"
void*_tmp645=({struct _tuple8*_tmpB87=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpB87,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB89=_tmp645;struct Cyc_List_List*_tmpB88=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpB89,_tmpB88,(unsigned int)(yyyylsp[0]).first_line);}));
# 3017
goto _LL0;}case 475U: _LL3B1: _LL3B2: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3019 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB8B=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->tag=34U,(_tmp646->f1).is_calloc=0,(_tmp646->f1).rgn=0,(_tmp646->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB8A=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp646->f1).num_elts=_tmpB8A;}),(_tmp646->f1).fat_result=0,(_tmp646->f1).inline_call=0;_tmp646;});Cyc_Absyn_new_exp(_tmpB8B,(unsigned int)(yyyylsp[0]).first_line);}));
# 3021
goto _LL0;}case 476U: _LL3B3: _LL3B4: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3022 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB8E=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->tag=34U,(_tmp647->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp647->f1).rgn=_tmpB8D;}),(_tmp647->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB8C=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp647->f1).num_elts=_tmpB8C;}),(_tmp647->f1).fat_result=0,(_tmp647->f1).inline_call=0;_tmp647;});Cyc_Absyn_new_exp(_tmpB8E,(unsigned int)(yyyylsp[0]).first_line);}));
# 3024
goto _LL0;}case 477U: _LL3B5: _LL3B6: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3025 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpB91=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->tag=34U,(_tmp648->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp648->f1).rgn=_tmpB90;}),(_tmp648->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpB8F=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp648->f1).num_elts=_tmpB8F;}),(_tmp648->f1).fat_result=0,(_tmp648->f1).inline_call=1;_tmp648;});Cyc_Absyn_new_exp(_tmpB91,(unsigned int)(yyyylsp[0]).first_line);}));
# 3027
goto _LL0;}case 478U: _LL3B7: _LL3B8: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 3028 "parse.y"
void*_tmp649=({struct _tuple8*_tmpB92=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpB92,(unsigned int)(yyyylsp[6]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB95=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->tag=34U,(_tmp64B->f1).is_calloc=1,(_tmp64B->f1).rgn=0,({void**_tmpB94=({void**_tmp64A=_cycalloc(sizeof(*_tmp64A));*_tmp64A=_tmp649;_tmp64A;});(_tmp64B->f1).elt_type=_tmpB94;}),({struct Cyc_Absyn_Exp*_tmpB93=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp64B->f1).num_elts=_tmpB93;}),(_tmp64B->f1).fat_result=0,(_tmp64B->f1).inline_call=0;_tmp64B;});Cyc_Absyn_new_exp(_tmpB95,(unsigned int)(yyyylsp[0]).first_line);}));
# 3031
goto _LL0;}case 479U: _LL3B9: _LL3BA: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11U));
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11U));
# 3033 "parse.y"
void*_tmp64C=({struct _tuple8*_tmpB96=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpB96,(unsigned int)(yyyylsp[8]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB9A=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->tag=34U,(_tmp64E->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpB99=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp64E->f1).rgn=_tmpB99;}),({void**_tmpB98=({void**_tmp64D=_cycalloc(sizeof(*_tmp64D));*_tmp64D=_tmp64C;_tmp64D;});(_tmp64E->f1).elt_type=_tmpB98;}),({struct Cyc_Absyn_Exp*_tmpB97=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp64E->f1).num_elts=_tmpB97;}),(_tmp64E->f1).fat_result=0,(_tmp64E->f1).inline_call=0;_tmp64E;});Cyc_Absyn_new_exp(_tmpB9A,(unsigned int)(yyyylsp[0]).first_line);}));
# 3036
goto _LL0;}case 480U: _LL3BB: _LL3BC: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3037 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpB9C=({struct Cyc_Absyn_Exp*_tmp64F[1U];({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp64F[0]=_tmpB9B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp64F,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpB9C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 481U: _LL3BD: _LL3BE: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3039 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA0=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->tag=38U,({struct Cyc_Absyn_Exp*_tmpB9F=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp651->f1=_tmpB9F;}),({struct _dyneither_ptr*_tmpB9E=({struct _dyneither_ptr*_tmp650=_cycalloc(sizeof(*_tmp650));({struct _dyneither_ptr _tmpB9D=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp650=_tmpB9D;});_tmp650;});_tmp651->f2=_tmpB9E;});_tmp651;});Cyc_Absyn_new_exp(_tmpBA0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 482U: _LL3BF: _LL3C0: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3041 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA5=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->tag=38U,({struct Cyc_Absyn_Exp*_tmpBA4=({struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpBA3,(unsigned int)(yyyylsp[2]).first_line);});_tmp653->f1=_tmpBA4;}),({struct _dyneither_ptr*_tmpBA2=({struct _dyneither_ptr*_tmp652=_cycalloc(sizeof(*_tmp652));({struct _dyneither_ptr _tmpBA1=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp652=_tmpBA1;});_tmp652;});_tmp653->f2=_tmpBA2;});_tmp653;});Cyc_Absyn_new_exp(_tmpBA5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 483U: _LL3C1: _LL3C2: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3043 "parse.y"
void*_tmp654=({struct _tuple8*_tmpBA6=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpBA6,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp654,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 484U: _LL3C3: _LL3C4: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3046 "parse.y"
struct _tuple20 _tmp655=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp656=_tmp655;int _tmp658;struct _dyneither_ptr _tmp657;_LL496: _tmp658=_tmp656.f1;_tmp657=_tmp656.f2;_LL497:;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp658,_tmp657,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 485U: _LL3C5: _LL3C6: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3048 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA7=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_extension_exp(_tmpBA7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 486U: _LL3C7: _LL3C8: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3052 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;}case 487U: _LL3C9: _LL3CA: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3053 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3054 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3059 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 490U: _LL3CF: _LL3D0: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3061 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpBA9,_tmpBA8,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 491U: _LL3D1: _LL3D2: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3063 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpBAA,0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 492U: _LL3D3: _LL3D4: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3065 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpBAB=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpBAC,_tmpBAB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 493U: _LL3D5: _LL3D6: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3067 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBAF=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpBAE=({struct _dyneither_ptr*_tmp659=_cycalloc(sizeof(*_tmp659));({struct _dyneither_ptr _tmpBAD=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp659=_tmpBAD;});_tmp659;});Cyc_Absyn_aggrmember_exp(_tmpBAF,_tmpBAE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 494U: _LL3D7: _LL3D8: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3069 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpBB1=({struct _dyneither_ptr*_tmp65A=_cycalloc(sizeof(*_tmp65A));({struct _dyneither_ptr _tmpBB0=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp65A=_tmpBB0;});_tmp65A;});Cyc_Absyn_aggrarrow_exp(_tmpBB2,_tmpBB1,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 495U: _LL3D9: _LL3DA: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3071 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpBB3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 496U: _LL3DB: _LL3DC: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3073 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpBB4,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 497U: _LL3DD: _LL3DE: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3075 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBB6=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->tag=25U,({struct _tuple8*_tmpBB5=Cyc_yyget_YY37(yyyyvsp[1]);_tmp65B->f1=_tmpBB5;}),_tmp65B->f2=0;_tmp65B;});Cyc_Absyn_new_exp(_tmpBB6,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 498U: _LL3DF: _LL3E0: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3077 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBB9=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->tag=25U,({struct _tuple8*_tmpBB8=Cyc_yyget_YY37(yyyyvsp[1]);_tmp65C->f1=_tmpBB8;}),({struct Cyc_List_List*_tmpBB7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp65C->f2=_tmpBB7;});_tmp65C;});Cyc_Absyn_new_exp(_tmpBB9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 499U: _LL3E1: _LL3E2: {
# 3080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 3079 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBBC=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->tag=25U,({struct _tuple8*_tmpBBB=Cyc_yyget_YY37(yyyyvsp[1]);_tmp65D->f1=_tmpBBB;}),({struct Cyc_List_List*_tmpBBA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp65D->f2=_tmpBBA;});_tmp65D;});Cyc_Absyn_new_exp(_tmpBBC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 500U: _LL3E3: _LL3E4: {
# 3082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3084 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp660=_cycalloc(sizeof(*_tmp660));({void*_tmpBBF=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->tag=0U,({struct _dyneither_ptr*_tmpBBE=({struct _dyneither_ptr*_tmp65E=_cycalloc(sizeof(*_tmp65E));({struct _dyneither_ptr _tmpBBD=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp65E=_tmpBBD;});_tmp65E;});_tmp65F->f1=_tmpBBE;});_tmp65F;});_tmp660->hd=_tmpBBF;}),_tmp660->tl=0;_tmp660;}));
goto _LL0;}case 501U: _LL3E5: _LL3E6: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3087 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp662=_cycalloc(sizeof(*_tmp662));({void*_tmpBC2=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=1U,({unsigned int _tmpBC1=({unsigned int _tmpBC0=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpBC0,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp661->f1=_tmpBC1;});_tmp661;});_tmp662->hd=_tmpBC2;}),_tmp662->tl=0;_tmp662;}));
goto _LL0;}case 502U: _LL3E7: _LL3E8: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3089 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp665=_cycalloc(sizeof(*_tmp665));({void*_tmpBC6=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->tag=0U,({struct _dyneither_ptr*_tmpBC5=({struct _dyneither_ptr*_tmp663=_cycalloc(sizeof(*_tmp663));({struct _dyneither_ptr _tmpBC4=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp663=_tmpBC4;});_tmp663;});_tmp664->f1=_tmpBC5;});_tmp664;});_tmp665->hd=_tmpBC6;}),({struct Cyc_List_List*_tmpBC3=Cyc_yyget_YY3(yyyyvsp[0]);_tmp665->tl=_tmpBC3;});_tmp665;}));
goto _LL0;}case 503U: _LL3E9: _LL3EA: {
# 3092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3092 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp667=_cycalloc(sizeof(*_tmp667));({void*_tmpBCA=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp666=_cycalloc(sizeof(*_tmp666));_tmp666->tag=1U,({unsigned int _tmpBC9=({unsigned int _tmpBC8=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpBC8,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp666->f1=_tmpBC9;});_tmp666;});_tmp667->hd=_tmpBCA;}),({struct Cyc_List_List*_tmpBC7=Cyc_yyget_YY3(yyyyvsp[0]);_tmp667->tl=_tmpBC7;});_tmp667;}));
goto _LL0;}case 504U: _LL3EB: _LL3EC: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_Exp_tok(({struct _tuple0*_tmpBCB=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpBCB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 505U: _LL3ED: _LL3EE: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3100 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBCC=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Absyn_pragma_exp(_tmpBCC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 506U: _LL3EF: _LL3F0: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3102 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 507U: _LL3F1: _LL3F2: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3104 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBCD=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpBCD,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 508U: _LL3F3: _LL3F4: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3106 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBCE=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpBCE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 509U: _LL3F5: _LL3F6: {
# 3109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3108 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 510U: _LL3F7: _LL3F8: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3113 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCF=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpBCF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 511U: _LL3F9: _LL3FA: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3115 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBD1=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpBD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpBD1,_tmpBD0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 512U: _LL3FB: _LL3FC: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3118 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBD2=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpBD2,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 513U: _LL3FD: _LL3FE: {
# 3121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3121 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBD6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->tag=29U,({struct _tuple0*_tmpBD5=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp668->f1=_tmpBD5;}),({struct Cyc_List_List*_tmpBD4=Cyc_yyget_YY40(yyyyvsp[2]);_tmp668->f2=_tmpBD4;}),({struct Cyc_List_List*_tmpBD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp668->f3=_tmpBD3;}),_tmp668->f4=0;_tmp668;});Cyc_Absyn_new_exp(_tmpBD6,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 514U: _LL3FF: _LL400: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3124 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpBD7=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpBD7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 515U: _LL401: _LL402: {
# 3127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3128 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
goto _LL0;}case 516U: _LL403: _LL404: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp669=_cycalloc(sizeof(*_tmp669));({struct Cyc_Absyn_Exp*_tmpBD8=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp669->hd=_tmpBD8;}),_tmp669->tl=0;_tmp669;}));
goto _LL0;}case 517U: _LL405: _LL406: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3136 "parse.y"
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp66A=_cycalloc(sizeof(*_tmp66A));({struct Cyc_Absyn_Exp*_tmpBDA=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp66A->hd=_tmpBDA;}),({struct Cyc_List_List*_tmpBD9=Cyc_yyget_YY4(yyyyvsp[0]);_tmp66A->tl=_tmpBD9;});_tmp66A;}));
goto _LL0;}case 518U: _LL407: _LL408: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpBDB=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpBDB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 519U: _LL409: _LL40A: {
# 3145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3143 "parse.y"
yyval=Cyc_Exp_tok(({char _tmpBDC=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpBDC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 520U: _LL40B: _LL40C: {
# 3146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3144 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBDD=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpBDD,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 521U: _LL40D: _LL40E: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3146 "parse.y"
struct _dyneither_ptr _tmp66B=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp66B);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp66B,sizeof(char),l - 1));
if((int)c == (int)'f'  || (int)c == (int)'F')i=0;else{
if((int)c == (int)'l'  || (int)c == (int)'L')i=2;}}
# 3154
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp66B,i,(unsigned int)(yyyylsp[0]).first_line));
# 3156
goto _LL0;}case 522U: _LL40F: _LL410: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3157 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 523U: _LL411: _LL412: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3161 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp66D=_cycalloc(sizeof(*_tmp66D));({union Cyc_Absyn_Nmspace _tmpBE0=Cyc_Absyn_Rel_n(0);_tmp66D->f1=_tmpBE0;}),({struct _dyneither_ptr*_tmpBDF=({struct _dyneither_ptr*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct _dyneither_ptr _tmpBDE=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp66C=_tmpBDE;});_tmp66C;});_tmp66D->f2=_tmpBDF;});_tmp66D;}));
goto _LL0;}case 524U: _LL413: _LL414: {
# 3164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3162 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 525U: _LL415: _LL416: {
# 3165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3165 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp66F=_cycalloc(sizeof(*_tmp66F));({union Cyc_Absyn_Nmspace _tmpBE3=Cyc_Absyn_Rel_n(0);_tmp66F->f1=_tmpBE3;}),({struct _dyneither_ptr*_tmpBE2=({struct _dyneither_ptr*_tmp66E=_cycalloc(sizeof(*_tmp66E));({struct _dyneither_ptr _tmpBE1=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp66E=_tmpBE1;});_tmp66E;});_tmp66F->f2=_tmpBE2;});_tmp66F;}));
goto _LL0;}case 526U: _LL417: _LL418: {
# 3168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3166 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 527U: _LL419: _LL41A: {
# 3169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3171 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 528U: _LL41B: _LL41C: {
# 3174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3172 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 529U: _LL41D: _LL41E: {
# 3175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3175 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 530U: _LL41F: _LL420: {
# 3178
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3176 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 531U: _LL421: _LL422: {
# 3179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3181 "parse.y"
goto _LL0;}case 532U: _LL423: _LL424: {
# 3183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3181 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL0;}default: _LL425: _LL426:
# 3186
 goto _LL0;}_LL0:;}
# 375 "cycbison.simple"
yyvsp_offset -=yylen;
yyssp_offset -=yylen;
# 378
yylsp_offset -=yylen;
# 392 "cycbison.simple"
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yyval;
# 395
++ yylsp_offset;
if(yylen == 0){
# 398
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_line=yylloc.first_line;
(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).first_column=yylloc.first_column;
({int _tmpBE4=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpBE4;});
({int _tmpBE5=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=_tmpBE5;});}else{
# 405
({int _tmpBE6=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpBE6;});
({int _tmpBE7=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=_tmpBE7;});}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
yystate=({int _tmpBE8=(int)*((short*)_check_known_subscript_notnull(Cyc_yypgoto,149U,sizeof(short),yyn - 149));_tmpBE8 + (int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0  && yystate <= 7470) && ({int _tmpBE9=(int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),yystate));_tmpBE9 == (int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));}))
yystate=(int)Cyc_yytable[yystate];else{
# 421
yystate=(int)*((short*)_check_known_subscript_notnull(Cyc_yydefgoto,149U,sizeof(short),yyn - 149));}
# 423
goto yynewstate;
# 425
yyerrlab:
# 427
 if(yyerrstatus == 0){
# 430
++ yynerrs;
# 433
yyn=(int)Cyc_yypact[yystate];
# 435
if(yyn > - 32768  && yyn < 7470){
# 437
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 441
count=0;
# 443
for(x=yyn < 0?- yyn: 0;(unsigned int)x < 298U / sizeof(char*);++ x){
# 445
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),x + yyn))== x)
({unsigned long _tmpBEA=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_known_subscript_notnull(Cyc_yytname,298U,sizeof(struct _dyneither_ptr),x)))+ (unsigned long)15;sze +=_tmpBEA;}),count ++;}
msg=({unsigned int _tmp671=(unsigned int)(sze + 15)+ 1U;char*_tmp670=({struct _RegionHandle*_tmpBEB=yyregion;_region_malloc(_tmpBEB,_check_times(_tmp671,sizeof(char)));});({{unsigned int _tmp72B=(unsigned int)(sze + 15);unsigned int i;for(i=0;i < _tmp72B;++ i){_tmp670[i]='\000';}_tmp670[_tmp72B]=0;}0;});_tag_dyneither(_tmp670,sizeof(char),_tmp671);});
({struct _dyneither_ptr _tmpBEC=msg;Cyc_strcpy(_tmpBEC,({const char*_tmp672="parse error";_tag_dyneither(_tmp672,sizeof(char),12U);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;(unsigned int)x < 298U / sizeof(char*);++ x){
# 455
if((int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),x + yyn))== x){
# 457
({struct _dyneither_ptr _tmpBED=msg;Cyc_strcat(_tmpBED,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp673=", expecting `";_tag_dyneither(_tmp673,sizeof(char),14U);}):(struct _dyneither_ptr)({const char*_tmp674=" or `";_tag_dyneither(_tmp674,sizeof(char),6U);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_known_subscript_notnull(Cyc_yytname,298U,sizeof(struct _dyneither_ptr),x)));
({struct _dyneither_ptr _tmpBEE=msg;Cyc_strcat(_tmpBEE,({const char*_tmp675="'";_tag_dyneither(_tmp675,sizeof(char),2U);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpBF0=({const char*_tmp676="parse error";_tag_dyneither(_tmp676,sizeof(char),12U);});int _tmpBEF=yystate;Cyc_yyerror(_tmpBF0,_tmpBEF,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp677=1;_npop_handler(0U);return _tmp677;}
# 489
yychar=-2;}
# 495
yyerrstatus=3;
# 497
goto yyerrhandle;
# 499
yyerrdefault:
# 509 "cycbison.simple"
 yyerrpop:
# 511
 if(yyssp_offset == 0){int _tmp678=1;_npop_handler(0U);return _tmp678;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1087U,sizeof(short),yystate));
if(yyn == -32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7470) || (int)*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),yyn))!= 1)goto yyerrdefault;
# 537
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 540
if(yyn == -32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 544
if(yyn == 0)goto yyerrpop;}
# 546
if(yyn == 1086){
int _tmp679=0;_npop_handler(0U);return _tmp679;}
# 554
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 556
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 559
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3184 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp67F=v;struct Cyc_Absyn_Stmt*_tmp699;struct Cyc_Absyn_Exp*_tmp698;struct _tuple0*_tmp697;struct _dyneither_ptr _tmp696;char _tmp695;union Cyc_Absyn_Cnst _tmp694;switch((_tmp67F.Stmt_tok).tag){case 1U: _LL1: _tmp694=(_tmp67F.Int_tok).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp682=({struct Cyc_String_pa_PrintArg_struct _tmp72F;_tmp72F.tag=0U,({struct _dyneither_ptr _tmpBF1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp694));_tmp72F.f1=_tmpBF1;});_tmp72F;});void*_tmp680[1U];_tmp680[0]=& _tmp682;({struct Cyc___cycFILE*_tmpBF3=Cyc_stderr;struct _dyneither_ptr _tmpBF2=({const char*_tmp681="%s";_tag_dyneither(_tmp681,sizeof(char),3U);});Cyc_fprintf(_tmpBF3,_tmpBF2,_tag_dyneither(_tmp680,sizeof(void*),1U));});});goto _LL0;case 2U: _LL3: _tmp695=(_tmp67F.Char_tok).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp685=({struct Cyc_Int_pa_PrintArg_struct _tmp730;_tmp730.tag=1U,_tmp730.f1=(unsigned long)((int)_tmp695);_tmp730;});void*_tmp683[1U];_tmp683[0]=& _tmp685;({struct Cyc___cycFILE*_tmpBF5=Cyc_stderr;struct _dyneither_ptr _tmpBF4=({const char*_tmp684="%c";_tag_dyneither(_tmp684,sizeof(char),3U);});Cyc_fprintf(_tmpBF5,_tmpBF4,_tag_dyneither(_tmp683,sizeof(void*),1U));});});goto _LL0;case 3U: _LL5: _tmp696=(_tmp67F.String_tok).val;_LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp688=({struct Cyc_String_pa_PrintArg_struct _tmp731;_tmp731.tag=0U,_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp696);_tmp731;});void*_tmp686[1U];_tmp686[0]=& _tmp688;({struct Cyc___cycFILE*_tmpBF7=Cyc_stderr;struct _dyneither_ptr _tmpBF6=({const char*_tmp687="\"%s\"";_tag_dyneither(_tmp687,sizeof(char),5U);});Cyc_fprintf(_tmpBF7,_tmpBF6,_tag_dyneither(_tmp686,sizeof(void*),1U));});});goto _LL0;case 5U: _LL7: _tmp697=(_tmp67F.QualId_tok).val;_LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp68B=({struct Cyc_String_pa_PrintArg_struct _tmp732;_tmp732.tag=0U,({struct _dyneither_ptr _tmpBF8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp697));_tmp732.f1=_tmpBF8;});_tmp732;});void*_tmp689[1U];_tmp689[0]=& _tmp68B;({struct Cyc___cycFILE*_tmpBFA=Cyc_stderr;struct _dyneither_ptr _tmpBF9=({const char*_tmp68A="%s";_tag_dyneither(_tmp68A,sizeof(char),3U);});Cyc_fprintf(_tmpBFA,_tmpBF9,_tag_dyneither(_tmp689,sizeof(void*),1U));});});goto _LL0;case 7U: _LL9: _tmp698=(_tmp67F.Exp_tok).val;_LLA:
({struct Cyc_String_pa_PrintArg_struct _tmp68E=({struct Cyc_String_pa_PrintArg_struct _tmp733;_tmp733.tag=0U,({struct _dyneither_ptr _tmpBFB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp698));_tmp733.f1=_tmpBFB;});_tmp733;});void*_tmp68C[1U];_tmp68C[0]=& _tmp68E;({struct Cyc___cycFILE*_tmpBFD=Cyc_stderr;struct _dyneither_ptr _tmpBFC=({const char*_tmp68D="%s";_tag_dyneither(_tmp68D,sizeof(char),3U);});Cyc_fprintf(_tmpBFD,_tmpBFC,_tag_dyneither(_tmp68C,sizeof(void*),1U));});});goto _LL0;case 8U: _LLB: _tmp699=(_tmp67F.Stmt_tok).val;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp691=({struct Cyc_String_pa_PrintArg_struct _tmp734;_tmp734.tag=0U,({struct _dyneither_ptr _tmpBFE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp699));_tmp734.f1=_tmpBFE;});_tmp734;});void*_tmp68F[1U];_tmp68F[0]=& _tmp691;({struct Cyc___cycFILE*_tmpC00=Cyc_stderr;struct _dyneither_ptr _tmpBFF=({const char*_tmp690="%s";_tag_dyneither(_tmp690,sizeof(char),3U);});Cyc_fprintf(_tmpC00,_tmpBFF,_tag_dyneither(_tmp68F,sizeof(void*),1U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp692=0U;({struct Cyc___cycFILE*_tmpC02=Cyc_stderr;struct _dyneither_ptr _tmpC01=({const char*_tmp693="?";_tag_dyneither(_tmp693,sizeof(char),2U);});Cyc_fprintf(_tmpC02,_tmpC01,_tag_dyneither(_tmp692,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3196
struct _dyneither_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp69A="end-of-file";_tag_dyneither(_tmp69A,sizeof(char),12U);});
# 3200
if(token == 364)
return Cyc_Lex_token_string;else{
if(token == 373)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0  && token <= 376?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,377U,sizeof(short),token)): 298;
if((unsigned int)z < 298U)
return Cyc_yytname[z];else{
return _tag_dyneither(0,0,0);}};}
# 3211
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp69B=_new_region("yyr");struct _RegionHandle*yyr=& _tmp69B;_push_region(yyr);
({struct _RegionHandle*_tmpC03=yyr;Cyc_yyparse(_tmpC03,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp69C=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp69C;};
# 3214
;_pop_region(yyr);};}
