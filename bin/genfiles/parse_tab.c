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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
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
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};
# 879
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;
extern struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempeston_d_val;
extern struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Tempestoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 908
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 912
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 916
void*Cyc_Absyn_compress_kb(void*);
# 927
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 929
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 932
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 934
extern void*Cyc_Absyn_sint_type;
# 936
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 939
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 943
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 945
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 970
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 974
void*Cyc_Absyn_bounds_one();
# 976
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 1000
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1003
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1056
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
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1099
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1102
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1105
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1109
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1114
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1116
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1123
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1132
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1137
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1143
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1168
int Cyc_Absyn_fntype_att(void*);
# 1186
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
# 50 "tcutil.h"
int Cyc_Tcutil_is_array_type(void*t);
# 102
void*Cyc_Tcutil_copy_type(void*t);
# 115
void*Cyc_Tcutil_compress(void*t);
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 151
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 156
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 161
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 163
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 165
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 170
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 180
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 330 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 381
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 391
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
({struct Cyc_List_List*_tmp74B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp74B;});
({struct _tuple10*_tmp74C=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp74C;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
if(_tmpB.loc != 0)loc=_tmpB.loc;
({void*_tmp8=0U;({unsigned int _tmp74E=loc;struct _dyneither_ptr _tmp74D=({const char*_tmp9="qualifier on type is ignored";_tag_dyneither(_tmp9,sizeof(char),29U);});Cyc_Warn_warn(_tmp74E,_tmp74D,_tag_dyneither(_tmp8,sizeof(void*),0U));});});}
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
return({struct _tuple14 _tmp6B3;_tmp6B3.f1=nullable,_tmp6B3.f2=bound,_tmp6B3.f3=zeroterm,_tmp6B3.f4=rgn;_tmp6B3;});}
# 240
struct _tuple0*Cyc_Parse_gensym_enum(){
# 242
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp752=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp752;}),({
struct _dyneither_ptr*_tmp751=({struct _dyneither_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _dyneither_ptr _tmp750=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6B4;_tmp6B4.tag=1U,_tmp6B4.f1=(unsigned long)enum_counter ++;_tmp6B4;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _dyneither_ptr _tmp74F=({const char*_tmp11="__anonymous_enum_%d__";_tag_dyneither(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp74F,_tag_dyneither(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp750;});_tmp13;});_tmp14->f2=_tmp751;});_tmp14;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 247
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 252
struct _tuple17*_tmp15=field_info;unsigned int _tmp22;struct _tuple0*_tmp21;struct Cyc_Absyn_Tqual _tmp20;void*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;_LL1: _tmp22=(_tmp15->f1)->f1;_tmp21=(_tmp15->f1)->f2;_tmp20=(_tmp15->f1)->f3;_tmp1F=(_tmp15->f1)->f4;_tmp1E=(_tmp15->f1)->f5;_tmp1D=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1B=(_tmp15->f2)->f2;_LL2:;
if(_tmp1E != 0)
({void*_tmp16=0U;({unsigned int _tmp754=loc;struct _dyneither_ptr _tmp753=({const char*_tmp17="bad type params in struct field";_tag_dyneither(_tmp17,sizeof(char),32U);});Cyc_Warn_err(_tmp754,_tmp753,_tag_dyneither(_tmp16,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp21))
({void*_tmp18=0U;({unsigned int _tmp756=loc;struct _dyneither_ptr _tmp755=({const char*_tmp19="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp19,sizeof(char),59U);});Cyc_Warn_err(_tmp756,_tmp755,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->name=(*_tmp21).f2,_tmp1A->tq=_tmp20,_tmp1A->type=_tmp1F,_tmp1A->width=_tmp1C,_tmp1A->attributes=_tmp1D,_tmp1A->requires_clause=_tmp1B;_tmp1A;});}
# 262
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp6B5;_tmp6B5.Signed_spec=0,_tmp6B5.Unsigned_spec=0,_tmp6B5.Short_spec=0,_tmp6B5.Long_spec=0,_tmp6B5.Long_Long_spec=0,_tmp6B5.Valid_type_spec=0,_tmp6B5.Type_spec=Cyc_Absyn_sint_type,_tmp6B5.loc=loc;_tmp6B5;});}
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
void*_tmp757=t;Cyc_Tcutil_promote_array(_tmp757,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 316 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_tmp2A=_tmp29;struct _dyneither_ptr _tmp3E;void**_tmp3D;struct _dyneither_ptr*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp3C=_tmp2A->f1;_tmp3B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp3C != 0){_LL2:
# 321
{void*_tmp2B=_tmp3B;void**_tmp32;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->tag == 1U){_LL8: _tmp32=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2;_LL9: {
# 325
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _dyneither_ptr _tmp759=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp6B6;_tmp6B6.tag=0U,_tmp6B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3C);_tmp6B6;});void*_tmp2E[1U];_tmp2E[0]=& _tmp30;({struct _dyneither_ptr _tmp758=({const char*_tmp2F="`%s";_tag_dyneither(_tmp2F,sizeof(char),4U);});Cyc_aprintf(_tmp758,_tag_dyneither(_tmp2E,sizeof(void*),1U));});});*_tmp31=_tmp759;});_tmp31;});
({void*_tmp75B=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->name=nm,_tmp2D->identity=- 1,({void*_tmp75A=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,_tmp2C->f1=& Cyc_Tcutil_ik;_tmp2C;});_tmp2D->kind=_tmp75A;});_tmp2D;}));*_tmp32=_tmp75B;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 330
_tmp28=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));({struct _tuple18*_tmp75C=({struct _tuple18*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=_tmp3C,_tmp33->f2=_tmp3B;_tmp33;});_tmp34->hd=_tmp75C;}),_tmp34->tl=_tmp28;_tmp34;});goto _LL0;}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp3E=*_tmp2A->f1;_tmp3D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {
# 334
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct _dyneither_ptr _tmp75E=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp6B7;_tmp6B7.tag=0U,_tmp6B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E);_tmp6B7;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({struct _dyneither_ptr _tmp75D=({const char*_tmp38="`%s";_tag_dyneither(_tmp38,sizeof(char),4U);});Cyc_aprintf(_tmp75D,_tag_dyneither(_tmp37,sizeof(void*),1U));});});*_tmp3A=_tmp75E;});_tmp3A;});
({void*_tmp760=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->name=nm,_tmp36->identity=- 1,({void*_tmp75F=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=0U,_tmp35->f1=& Cyc_Tcutil_rk;_tmp35;});_tmp36->kind=_tmp75F;});_tmp36;}));*_tmp3D=_tmp760;});
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
 _tmp3F=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));({struct _tuple18*_tmp761=({struct _tuple18*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp42->f2=_tmp44;_tmp42;});_tmp43->hd=_tmp761;}),_tmp43->tl=_tmp3F;_tmp43;});goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
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
return({void*_tmp763=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=39U,({void*_tmp762=Cyc_Tcutil_copy_type(_tmp4B);_tmp4A->f1=_tmp762;});_tmp4A;});Cyc_Absyn_new_exp(_tmp763,e->loc);});}}
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
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6B8;_tmp6B8.elt_type=_tmp50,_tmp6B8.elt_tq=_tmp5E,(_tmp6B8.ptr_atts).rgn=_tmp5D,(_tmp6B8.ptr_atts).nullable=_tmp5C,(_tmp6B8.ptr_atts).bounds=_tmp51,(_tmp6B8.ptr_atts).zero_term=_tmp5A,(_tmp6B8.ptr_atts).ptrloc=_tmp59;_tmp6B8;}));
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
({void*_tmp764=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp764;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
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
if(({struct _dyneither_ptr _tmp765=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp765,({const char*_tmp64="`H";_tag_dyneither(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;else{
if(({struct _dyneither_ptr _tmp766=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp766,({const char*_tmp65="`U";_tag_dyneither(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;else{
if(({struct _dyneither_ptr _tmp767=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp767,({const char*_tmp66="`RC";_tag_dyneither(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;else{
# 437
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _dyneither_ptr*_tmp768=({struct _dyneither_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp768;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}}}}
# 444
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp69=t;struct Cyc_Absyn_Tvar*_tmp6C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->tag == 2U){_LL1: _tmp6C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;_LL2:
 return _tmp6C;}else{_LL3: _LL4:
({void*_tmp6A=0U;({unsigned int _tmp76A=loc;struct _dyneither_ptr _tmp769=({const char*_tmp6B="expecting a list of type variables, not types";_tag_dyneither(_tmp6B,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp76A,_tmp769,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});}_LL0:;}
# 452
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_tmp6E=_tmp6D;void**_tmp72;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->tag == 2U){_LL1: _tmp72=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1)->kind;_LL2: {
# 455
void*_tmp6F=Cyc_Absyn_compress_kb(*_tmp72);void*_tmp70=_tmp6F;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp70)->tag == 1U){_LL6: _LL7:
# 457
 if(!leq)({void*_tmp76B=Cyc_Tcutil_kind_to_bound(k);*_tmp72=_tmp76B;});else{
({void*_tmp76C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2U,_tmp71->f1=0,_tmp71->f2=k;_tmp71;});*_tmp72=_tmp76C;});}
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
({void*_tmp76=0U;({unsigned int _tmp76E=loc;struct _dyneither_ptr _tmp76D=({const char*_tmp77="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp77,sizeof(char),93U);});Cyc_Warn_warn(_tmp76E,_tmp76D,_tag_dyneither(_tmp76,sizeof(void*),0U));});});
# 490
return tms;}else{_LL8: _tmp8D=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp75)->f1;_LL9:
# 492
 if(({int _tmp76F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);_tmp76F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp78=0U;({unsigned int _tmp771=loc;struct _dyneither_ptr _tmp770=({const char*_tmp79="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp79,sizeof(char),73U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp771,_tmp770,_tag_dyneither(_tmp78,sizeof(void*),0U));});});{
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
({void*_tmp7E=0U;({unsigned int _tmp773=_tmp7B->loc;struct _dyneither_ptr _tmp772=({const char*_tmp7F="initializer found in parameter declaration";_tag_dyneither(_tmp7F,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp773,_tmp772,_tag_dyneither(_tmp7E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp86->name))
({void*_tmp80=0U;({unsigned int _tmp775=_tmp7B->loc;struct _dyneither_ptr _tmp774=({const char*_tmp81="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp81,sizeof(char),47U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp775,_tmp774,_tag_dyneither(_tmp80,sizeof(void*),0U));});});
rev_new_params=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));
({struct _tuple8*_tmp776=({struct _tuple8*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(*_tmp86->name).f2,_tmp82->f2=_tmp86->tq,_tmp82->f3=_tmp86->type;_tmp82;});_tmp83->hd=_tmp776;}),_tmp83->tl=rev_new_params;_tmp83;});
# 512
goto L;}else{_LLD: _LLE:
({void*_tmp84=0U;({unsigned int _tmp778=_tmp7B->loc;struct _dyneither_ptr _tmp777=({const char*_tmp85="nonvariable declaration in parameter type";_tag_dyneither(_tmp85,sizeof(char),42U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp778,_tmp777,_tag_dyneither(_tmp84,sizeof(void*),0U));});});}_LLA:;}
# 516
L: if(_tmp7A == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp6B9;_tmp6B9.tag=0U,_tmp6B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp8D->hd));_tmp6B9;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({unsigned int _tmp77A=loc;struct _dyneither_ptr _tmp779=({const char*_tmp88="%s is not given a type";_tag_dyneither(_tmp88,sizeof(char),23U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp77A,_tmp779,_tag_dyneither(_tmp87,sizeof(void*),1U));});});}
# 519
return({struct Cyc_List_List*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));
({void*_tmp77D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8B=_region_malloc(yy,sizeof(*_tmp8B));_tmp8B->tag=3U,({void*_tmp77C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8A=_region_malloc(yy,sizeof(*_tmp8A));_tmp8A->tag=1U,({struct Cyc_List_List*_tmp77B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp8A->f1=_tmp77B;}),_tmp8A->f2=0,_tmp8A->f3=0,_tmp8A->f4=0,_tmp8A->f5=0,_tmp8A->f6=0,_tmp8A->f7=0;_tmp8A;});_tmp8B->f1=_tmp77C;});_tmp8B;});_tmp8C->hd=_tmp77D;}),_tmp8C->tl=0;_tmp8C;});};}_LL5:;}
# 526
goto _LL4;}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));_tmp8E->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp77E=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8E->tl=_tmp77E;});_tmp8E;});}_LL0:;};}
# 534
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 538
if(tds != 0)
d=({struct Cyc_Parse_Declarator _tmp6BA;_tmp6BA.id=d.id,_tmp6BA.varloc=d.varloc,({struct Cyc_List_List*_tmp77F=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6BA.tms=_tmp77F;});_tmp6BA;});{
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
({void*_tmp92=0U;({unsigned int _tmp781=loc;struct _dyneither_ptr _tmp780=({const char*_tmp93="bad storage class on function";_tag_dyneither(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp781,_tmp780,_tag_dyneither(_tmp92,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 560
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmp95=Cyc_Parse_apply_tms(tq,_tmp94,atts,d.tms);struct _tuple13 _tmp96=_tmp95;struct Cyc_Absyn_Tqual _tmpBC;void*_tmpBB;struct Cyc_List_List*_tmpBA;struct Cyc_List_List*_tmpB9;_LL8: _tmpBC=_tmp96.f1;_tmpBB=_tmp96.f2;_tmpBA=_tmp96.f3;_tmpB9=_tmp96.f4;_LL9:;
# 565
if(_tmpBA != 0)
# 568
({void*_tmp97=0U;({unsigned int _tmp783=loc;struct _dyneither_ptr _tmp782=({const char*_tmp98="bad type params, ignoring";_tag_dyneither(_tmp98,sizeof(char),26U);});Cyc_Warn_warn(_tmp783,_tmp782,_tag_dyneither(_tmp97,sizeof(void*),0U));});});{
# 570
void*_tmp99=_tmpBB;struct Cyc_List_List*_tmpB8;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;void*_tmpB5;struct Cyc_List_List*_tmpB4;int _tmpB3;struct Cyc_Absyn_VarargInfo*_tmpB2;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_List_List*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_List_List*_tmpAC;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->tag == 5U){_LLB: _tmpB8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).tvars;_tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).effect;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ret_tqual;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ret_type;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).args;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).c_varargs;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).cyc_varargs;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).rgn_po;_tmpB0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).attributes;_tmpAF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).requires_clause;_tmpAE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).requires_relns;_tmpAD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ensures_clause;_tmpAC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ensures_relns;_LLC: {
# 574
struct Cyc_List_List*_tmp9A=0;
{struct Cyc_List_List*_tmp9B=_tmpB4;for(0;_tmp9B != 0;_tmp9B=_tmp9B->tl){
struct _tuple8*_tmp9C=(struct _tuple8*)_tmp9B->hd;struct _tuple8*_tmp9D=_tmp9C;struct _dyneither_ptr*_tmpA8;struct Cyc_Absyn_Tqual _tmpA7;void*_tmpA6;_LL10: _tmpA8=_tmp9D->f1;_tmpA7=_tmp9D->f2;_tmpA6=_tmp9D->f3;_LL11:;
if(_tmpA8 == 0){
({void*_tmp9E=0U;({unsigned int _tmp785=loc;struct _dyneither_ptr _tmp784=({const char*_tmp9F="missing argument variable in function prototype";_tag_dyneither(_tmp9F,sizeof(char),48U);});Cyc_Warn_err(_tmp785,_tmp784,_tag_dyneither(_tmp9E,sizeof(void*),0U));});});
_tmp9A=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct _tuple8*_tmp788=({struct _tuple8*_tmpA2=_cycalloc(sizeof(*_tmpA2));({struct _dyneither_ptr*_tmp787=({struct _dyneither_ptr*_tmpA1=_cycalloc(sizeof(*_tmpA1));({struct _dyneither_ptr _tmp786=({const char*_tmpA0="?";_tag_dyneither(_tmpA0,sizeof(char),2U);});*_tmpA1=_tmp786;});_tmpA1;});_tmpA2->f1=_tmp787;}),_tmpA2->f2=_tmpA7,_tmpA2->f3=_tmpA6;_tmpA2;});_tmpA3->hd=_tmp788;}),_tmpA3->tl=_tmp9A;_tmpA3;});}else{
# 581
_tmp9A=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));({struct _tuple8*_tmp789=({struct _tuple8*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=_tmpA8,_tmpA4->f2=_tmpA7,_tmpA4->f3=_tmpA6;_tmpA4;});_tmpA5->hd=_tmp789;}),_tmpA5->tl=_tmp9A;_tmpA5;});}}}
# 585
return({struct Cyc_Absyn_Fndecl*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->sc=sc,_tmpA9->name=d.id,_tmpA9->tvs=_tmpB8,_tmpA9->is_inline=is_inline,_tmpA9->effect=_tmpB7,_tmpA9->ret_tqual=_tmpB6,_tmpA9->ret_type=_tmpB5,({
# 588
struct Cyc_List_List*_tmp78B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9A);_tmpA9->args=_tmp78B;}),_tmpA9->c_varargs=_tmpB3,_tmpA9->cyc_varargs=_tmpB2,_tmpA9->rgn_po=_tmpB1,_tmpA9->body=body,_tmpA9->cached_type=0,_tmpA9->param_vardecls=0,_tmpA9->fn_vardecl=0,({
# 595
struct Cyc_List_List*_tmp78A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB0,_tmpB9);_tmpA9->attributes=_tmp78A;}),_tmpA9->requires_clause=_tmpAF,_tmpA9->requires_relns=0,_tmpA9->ensures_clause=_tmpAD,_tmpA9->ensures_relns=0;_tmpA9;});}}else{_LLD: _LLE:
# 600
({void*_tmpAA=0U;({unsigned int _tmp78D=loc;struct _dyneither_ptr _tmp78C=({const char*_tmpAB="declarator is not a function prototype";_tag_dyneither(_tmpAB,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp78D,_tmp78C,_tag_dyneither(_tmpAA,sizeof(void*),0U));});});}_LLA:;};};};}static char _tmpBD[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 604
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpBD,_tmpBD,_tmpBD + 76U};static char _tmpBE[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 606
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpBE,_tmpBE,_tmpBE + 87U};static char _tmpBF[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 608
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpBF,_tmpBF,_tmpBF + 74U};static char _tmpC0[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 610
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpC0,_tmpC0,_tmpC0 + 84U};
# 617
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 620
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpC1=0U;({unsigned int _tmp78F=loc;struct _dyneither_ptr _tmp78E=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp78F,_tmp78E,_tag_dyneither(_tmpC1,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpC2=0U;({unsigned int _tmp791=loc;struct _dyneither_ptr _tmp790=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp791,_tmp790,_tag_dyneither(_tmpC2,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpC3=0U;({unsigned int _tmp793=loc;struct _dyneither_ptr _tmp792=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp793,_tmp792,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpC4=0U;({unsigned int _tmp795=loc;struct _dyneither_ptr _tmp794=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp795,_tmp794,_tag_dyneither(_tmpC4,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpC5=0U;({unsigned int _tmp797=loc;struct _dyneither_ptr _tmp796=Cyc_Parse_msg1;Cyc_Warn_err(_tmp797,_tmp796,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 642
return s1;}
# 648
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 651
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 658
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpC6=0U;({unsigned int _tmp799=loc;struct _dyneither_ptr _tmp798=Cyc_Parse_msg4;Cyc_Warn_err(_tmp799,_tmp798,_tag_dyneither(_tmpC6,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpC7=0U;({unsigned int _tmp79B=loc;struct _dyneither_ptr _tmp79A=Cyc_Parse_msg4;Cyc_Warn_err(_tmp79B,_tmp79A,_tag_dyneither(_tmpC7,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 670
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpC8=0U;({unsigned int _tmp79D=loc;struct _dyneither_ptr _tmp79C=({const char*_tmpC9="missing type within specifier";_tag_dyneither(_tmpC9,sizeof(char),30U);});Cyc_Warn_warn(_tmp79D,_tmp79C,_tag_dyneither(_tmpC8,sizeof(void*),0U));});});
t=Cyc_Absyn_int_type(sgn,sz);}else{
# 675
if(seen_sign){
void*_tmpCA=t;enum Cyc_Absyn_Sign _tmpCE;enum Cyc_Absyn_Size_of _tmpCD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->f1)->tag == 1U){_LL1: _tmpCE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->f1)->f1;_tmpCD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->f1)->f2;_LL2:
# 678
 if(_tmpCE != sgn)
t=Cyc_Absyn_int_type(sgn,_tmpCD);
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpCB=0U;({unsigned int _tmp79F=loc;struct _dyneither_ptr _tmp79E=({const char*_tmpCC="sign specification on non-integral type";_tag_dyneither(_tmpCC,sizeof(char),40U);});Cyc_Warn_err(_tmp79F,_tmp79E,_tag_dyneither(_tmpCB,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 683
if(seen_size){
void*_tmpCF=t;enum Cyc_Absyn_Sign _tmpD3;enum Cyc_Absyn_Size_of _tmpD2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->f1)){case 1U: _LL6: _tmpD3=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->f1)->f1;_tmpD2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->f1)->f2;_LL7:
# 686
 if(_tmpD2 != sz)
t=Cyc_Absyn_int_type(_tmpD3,sz);
goto _LL5;case 2U: _LL8: _LL9:
# 690
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpD0=0U;({unsigned int _tmp7A1=loc;struct _dyneither_ptr _tmp7A0=({const char*_tmpD1="size qualifier on non-integral type";_tag_dyneither(_tmpD1,sizeof(char),36U);});Cyc_Warn_err(_tmp7A1,_tmp7A0,_tag_dyneither(_tmpD0,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 694
return t;}
# 697
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 701
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpD4=d.id;
unsigned int _tmpD5=d.varloc;
struct _tuple13 _tmpD6=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpD7=_tmpD6;struct Cyc_Absyn_Tqual _tmpDF;void*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_List_List*_tmpDC;_LL1: _tmpDF=_tmpD7.f1;_tmpDE=_tmpD7.f2;_tmpDD=_tmpD7.f3;_tmpDC=_tmpD7.f4;_LL2:;
# 708
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpD9=_region_malloc(r,sizeof(*_tmpD9));({struct _tuple15*_tmp7A2=({struct _tuple15*_tmpD8=_region_malloc(r,sizeof(*_tmpD8));_tmpD8->f1=_tmpD5,_tmpD8->f2=_tmpD4,_tmpD8->f3=_tmpDF,_tmpD8->f4=_tmpDE,_tmpD8->f5=_tmpDD,_tmpD8->f6=_tmpDC;_tmpD8;});_tmpD9->hd=_tmp7A2;}),_tmpD9->tl=0;_tmpD9;});else{
# 711
return({struct Cyc_List_List*_tmpDB=_region_malloc(r,sizeof(*_tmpDB));({struct _tuple15*_tmp7A8=({struct _tuple15*_tmpDA=_region_malloc(r,sizeof(*_tmpDA));_tmpDA->f1=_tmpD5,_tmpDA->f2=_tmpD4,_tmpDA->f3=_tmpDF,_tmpDA->f4=_tmpDE,_tmpDA->f5=_tmpDD,_tmpDA->f6=_tmpDC;_tmpDA;});_tmpDB->hd=_tmp7A8;}),({
struct Cyc_List_List*_tmp7A7=({struct _RegionHandle*_tmp7A6=r;struct Cyc_Absyn_Tqual _tmp7A5=tq;void*_tmp7A4=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp7A3=ds->tl;Cyc_Parse_apply_tmss(_tmp7A6,_tmp7A5,_tmp7A4,_tmp7A3,shared_atts);});_tmpDB->tl=_tmp7A7;});_tmpDB;});}};}
# 715
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 718
if(tms == 0)return({struct _tuple13 _tmp6BB;_tmp6BB.f1=tq,_tmp6BB.f2=t,_tmp6BB.f3=0,_tmp6BB.f4=atts;_tmp6BB;});{
void*_tmpE0=(void*)tms->hd;void*_tmpE1=_tmpE0;unsigned int _tmp106;struct Cyc_List_List*_tmp105;struct Cyc_Absyn_PtrAtts _tmp104;struct Cyc_Absyn_Tqual _tmp103;struct Cyc_List_List*_tmp102;unsigned int _tmp101;void*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;void*_tmpFE;unsigned int _tmpFD;void*_tmpFC;unsigned int _tmpFB;switch(*((int*)_tmpE1)){case 0U: _LL1: _tmpFC=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmpFB=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LL2:
# 721
 return({struct Cyc_Absyn_Tqual _tmp7AB=Cyc_Absyn_empty_tqual(0U);void*_tmp7AA=
Cyc_Absyn_array_type(t,tq,0,_tmpFC,_tmpFB);
# 721
struct Cyc_List_List*_tmp7A9=atts;Cyc_Parse_apply_tms(_tmp7AB,_tmp7AA,_tmp7A9,tms->tl);});case 1U: _LL3: _tmpFF=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmpFE=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_tmpFD=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f3;_LL4:
# 724
 return({struct Cyc_Absyn_Tqual _tmp7AE=Cyc_Absyn_empty_tqual(0U);void*_tmp7AD=
Cyc_Absyn_array_type(t,tq,_tmpFF,_tmpFE,_tmpFD);
# 724
struct Cyc_List_List*_tmp7AC=atts;Cyc_Parse_apply_tms(_tmp7AE,_tmp7AD,_tmp7AC,tms->tl);});case 3U: _LL5: _tmp100=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_LL6: {
# 727
void*_tmpE2=_tmp100;unsigned int _tmpF8;struct Cyc_List_List*_tmpF7;int _tmpF6;struct Cyc_Absyn_VarargInfo*_tmpF5;void*_tmpF4;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->tag == 1U){_LLE: _tmpF7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f1;_tmpF6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f2;_tmpF5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f3;_tmpF4=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f4;_tmpF3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f5;_tmpF2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f6;_tmpF1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f7;_LLF: {
# 729
struct Cyc_List_List*typvars=0;
# 731
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
fn_atts=({struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->hd=(void*)as->hd,_tmpE3->tl=fn_atts;_tmpE3;});else{
# 736
new_atts=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->hd=(void*)as->hd,_tmpE4->tl=new_atts;_tmpE4;});}}}
# 739
if(tms->tl != 0){
void*_tmpE5=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmpE6=_tmpE5;struct Cyc_List_List*_tmpE7;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE6)->tag == 4U){_LL13: _tmpE7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE6)->f1;_LL14:
# 742
 typvars=_tmpE7;
tms=tms->tl;
goto _LL12;}else{_LL15: _LL16:
# 746
 goto _LL12;}_LL12:;}
# 750
if(((((!_tmpF6  && _tmpF5 == 0) && _tmpF7 != 0) && _tmpF7->tl == 0) && (*((struct _tuple8*)_tmpF7->hd)).f1 == 0) && (*((struct _tuple8*)_tmpF7->hd)).f3 == Cyc_Absyn_void_type)
# 755
_tmpF7=0;{
# 758
struct Cyc_List_List*_tmpE8=Cyc_Parse_get_arg_tags(_tmpF7);
# 760
if(_tmpE8 != 0)
t=Cyc_Parse_substitute_tags(_tmpE8,t);
t=Cyc_Parse_array2ptr(t,0);
# 765
{struct Cyc_List_List*_tmpE9=_tmpF7;for(0;_tmpE9 != 0;_tmpE9=_tmpE9->tl){
struct _tuple8*_tmpEA=(struct _tuple8*)_tmpE9->hd;struct _tuple8*_tmpEB=_tmpEA;struct _dyneither_ptr*_tmpEE;struct Cyc_Absyn_Tqual _tmpED;void**_tmpEC;_LL18: _tmpEE=_tmpEB->f1;_tmpED=_tmpEB->f2;_tmpEC=(void**)& _tmpEB->f3;_LL19:;
if(_tmpE8 != 0)
({void*_tmp7AF=Cyc_Parse_substitute_tags(_tmpE8,*_tmpEC);*_tmpEC=_tmp7AF;});
({void*_tmp7B0=Cyc_Parse_array2ptr(*_tmpEC,1);*_tmpEC=_tmp7B0;});}}
# 777
return({struct Cyc_Absyn_Tqual _tmp7B3=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7B2=
Cyc_Absyn_function_type(typvars,_tmpF4,tq,t,_tmpF7,_tmpF6,_tmpF5,_tmpF3,fn_atts,_tmpF2,_tmpF1);
# 777
struct Cyc_List_List*_tmp7B1=new_atts;Cyc_Parse_apply_tms(_tmp7B3,_tmp7B2,_tmp7B1,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL10: _tmpF8=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpE2)->f2;_LL11:
# 784
({void*_tmpEF=0U;({unsigned int _tmp7B5=_tmpF8;struct _dyneither_ptr _tmp7B4=({const char*_tmpF0="function declaration without parameter types";_tag_dyneither(_tmpF0,sizeof(char),45U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7B5,_tmp7B4,_tag_dyneither(_tmpEF,sizeof(void*),0U));});});}_LLD:;}case 4U: _LL7: _tmp102=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmp101=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LL8:
# 791
 if(tms->tl == 0)
return({struct _tuple13 _tmp6BC;_tmp6BC.f1=tq,_tmp6BC.f2=t,_tmp6BC.f3=_tmp102,_tmp6BC.f4=atts;_tmp6BC;});
# 796
({void*_tmpF9=0U;({unsigned int _tmp7B7=_tmp101;struct _dyneither_ptr _tmp7B6=({const char*_tmpFA="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmpFA,sizeof(char),68U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7B7,_tmp7B6,_tag_dyneither(_tmpF9,sizeof(void*),0U));});});case 2U: _LL9: _tmp104=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmp103=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LLA:
# 799
 return({struct Cyc_Absyn_Tqual _tmp7BA=_tmp103;void*_tmp7B9=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6BD;_tmp6BD.elt_type=t,_tmp6BD.elt_tq=tq,_tmp6BD.ptr_atts=_tmp104;_tmp6BD;}));struct Cyc_List_List*_tmp7B8=atts;Cyc_Parse_apply_tms(_tmp7BA,_tmp7B9,_tmp7B8,tms->tl);});default: _LLB: _tmp106=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmp105=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LLC:
# 804
 return({struct Cyc_Absyn_Tqual _tmp7BD=tq;void*_tmp7BC=t;struct Cyc_List_List*_tmp7BB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp105);Cyc_Parse_apply_tms(_tmp7BD,_tmp7BC,_tmp7BB,tms->tl);});}_LL0:;};}
# 810
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 819
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmp107=t;unsigned int _tmp111;struct _tuple0*_tmp110;struct Cyc_Absyn_Tqual _tmp10F;void*_tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_List_List*_tmp10C;_LL1: _tmp111=_tmp107->f1;_tmp110=_tmp107->f2;_tmp10F=_tmp107->f3;_tmp10E=_tmp107->f4;_tmp10D=_tmp107->f5;_tmp10C=_tmp107->f6;_LL2:;
# 822
Cyc_Lex_register_typedef(_tmp110);{
# 824
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp108=_tmp10E;struct Cyc_Core_Opt*_tmp109;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp108)->tag == 1U){_LL4: _tmp109=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp108)->f1;_LL5:
# 828
 type=0;
if(_tmp109 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp109;}
goto _LL3;}else{_LL6: _LL7:
 kind=0;type=_tmp10E;goto _LL3;}_LL3:;}
# 834
return({void*_tmp7BF=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7BE=({struct Cyc_Absyn_Typedefdecl*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->name=_tmp110,_tmp10A->tvs=_tmp10D,_tmp10A->kind=kind,_tmp10A->defn=type,_tmp10A->atts=_tmp10C,_tmp10A->tq=_tmp10F,_tmp10A->extern_c=0;_tmp10A;});_tmp10B->f1=_tmp7BE;});_tmp10B;});Cyc_Absyn_new_decl(_tmp7BF,loc);});};}
# 841
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7C0=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=12U,_tmp112->f1=d,_tmp112->f2=s;_tmp112;});Cyc_Absyn_new_stmt(_tmp7C0,d->loc);});}
# 847
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 856
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 860
struct _RegionHandle _tmp113=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp113;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp114=ds;struct Cyc_Absyn_Tqual _tmp175;struct Cyc_Parse_Type_specifier _tmp174;struct Cyc_List_List*_tmp173;_LL1: _tmp175=_tmp114.tq;_tmp174=_tmp114.type_specs;_tmp173=_tmp114.attributes;_LL2:;
if(_tmp175.loc == 0)_tmp175.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp115=0U;({unsigned int _tmp7C2=loc;struct _dyneither_ptr _tmp7C1=({const char*_tmp116="inline qualifier on non-function definition";_tag_dyneither(_tmp116,sizeof(char),44U);});Cyc_Warn_warn(_tmp7C2,_tmp7C1,_tag_dyneither(_tmp115,sizeof(void*),0U));});});{
# 866
enum Cyc_Absyn_Scope s=2U;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp117=*ds.sc;enum Cyc_Parse_Storage_class _tmp118=_tmp117;switch(_tmp118){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=3U;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=4U;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=0U;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=2U;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 if(Cyc_Parse_no_register)s=2U;else{s=5U;}goto _LL3;default: _LL10: _LL11:
 s=1U;goto _LL3;}_LL3:;}{
# 882
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 886
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 894
void*_tmp119=Cyc_Parse_collapse_type_specifiers(_tmp174,loc);
if(declarators == 0){
# 898
void*_tmp11A=_tmp119;struct Cyc_List_List*_tmp15A;struct _tuple0*_tmp159;struct _tuple0*_tmp158;int _tmp157;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_Datatypedecl**_tmp155;enum Cyc_Absyn_AggrKind _tmp154;struct _tuple0*_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Datatypedecl*_tmp151;struct Cyc_Absyn_Enumdecl*_tmp150;struct Cyc_Absyn_Aggrdecl*_tmp14F;switch(*((int*)_tmp11A)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)){case 0U: _LL13: _tmp14F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)->f1;_LL14:
# 900
({struct Cyc_List_List*_tmp7C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp14F->attributes,_tmp173);_tmp14F->attributes=_tmp7C3;});
_tmp14F->sc=s;{
struct Cyc_List_List*_tmp11D=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_Absyn_Decl*_tmp7C5=({void*_tmp7C4=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=5U,_tmp11B->f1=_tmp14F;_tmp11B;});Cyc_Absyn_new_decl(_tmp7C4,loc);});_tmp11C->hd=_tmp7C5;}),_tmp11C->tl=0;_tmp11C;});_npop_handler(0U);return _tmp11D;};case 1U: _LL15: _tmp150=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)->f1;_LL16:
# 904
 if(_tmp173 != 0)({void*_tmp11E=0U;({unsigned int _tmp7C7=loc;struct _dyneither_ptr _tmp7C6=({const char*_tmp11F="attributes on enum not supported";_tag_dyneither(_tmp11F,sizeof(char),33U);});Cyc_Warn_err(_tmp7C7,_tmp7C6,_tag_dyneither(_tmp11E,sizeof(void*),0U));});});
_tmp150->sc=s;{
struct Cyc_List_List*_tmp122=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Absyn_Decl*_tmp7C9=({void*_tmp7C8=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->tag=7U,_tmp120->f1=_tmp150;_tmp120;});Cyc_Absyn_new_decl(_tmp7C8,loc);});_tmp121->hd=_tmp7C9;}),_tmp121->tl=0;_tmp121;});_npop_handler(0U);return _tmp122;};default: _LL17: _tmp151=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)->f1;_LL18:
# 908
 if(_tmp173 != 0)({void*_tmp123=0U;({unsigned int _tmp7CB=loc;struct _dyneither_ptr _tmp7CA=({const char*_tmp124="attributes on datatypes not supported";_tag_dyneither(_tmp124,sizeof(char),38U);});Cyc_Warn_err(_tmp7CB,_tmp7CA,_tag_dyneither(_tmp123,sizeof(void*),0U));});});
_tmp151->sc=s;{
struct Cyc_List_List*_tmp127=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));({struct Cyc_Absyn_Decl*_tmp7CD=({void*_tmp7CC=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=6U,_tmp125->f1=_tmp151;_tmp125;});Cyc_Absyn_new_decl(_tmp7CC,loc);});_tmp126->hd=_tmp7CD;}),_tmp126->tl=0;_tmp126;});_npop_handler(0U);return _tmp127;};}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownAggr).tag == 1){_LL19: _tmp154=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownAggr).val).f1;_tmp153=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownAggr).val).f2;_tmp152=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f2;_LL1A: {
# 912
struct Cyc_List_List*_tmp128=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp152);
struct Cyc_Absyn_Aggrdecl*_tmp129=({struct Cyc_Absyn_Aggrdecl*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->kind=_tmp154,_tmp12F->sc=s,_tmp12F->name=_tmp153,_tmp12F->tvs=_tmp128,_tmp12F->impl=0,_tmp12F->attributes=0,_tmp12F->expected_mem_kind=0;_tmp12F;});
if(_tmp173 != 0)({void*_tmp12A=0U;({unsigned int _tmp7CF=loc;struct _dyneither_ptr _tmp7CE=({const char*_tmp12B="bad attributes on type declaration";_tag_dyneither(_tmp12B,sizeof(char),35U);});Cyc_Warn_err(_tmp7CF,_tmp7CE,_tag_dyneither(_tmp12A,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12E=({struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct Cyc_Absyn_Decl*_tmp7D1=({void*_tmp7D0=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=5U,_tmp12C->f1=_tmp129;_tmp12C;});Cyc_Absyn_new_decl(_tmp7D0,loc);});_tmp12D->hd=_tmp7D1;}),_tmp12D->tl=0;_tmp12D;});_npop_handler(0U);return _tmp12E;};}}else{goto _LL23;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).KnownDatatype).tag == 2){_LL1B: _tmp155=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).KnownDatatype).val;_LL1C:
# 917
 if(_tmp173 != 0)({void*_tmp130=0U;({unsigned int _tmp7D3=loc;struct _dyneither_ptr _tmp7D2=({const char*_tmp131="bad attributes on datatype";_tag_dyneither(_tmp131,sizeof(char),27U);});Cyc_Warn_err(_tmp7D3,_tmp7D2,_tag_dyneither(_tmp130,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp134=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));({struct Cyc_Absyn_Decl*_tmp7D5=({void*_tmp7D4=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=6U,_tmp132->f1=*_tmp155;_tmp132;});Cyc_Absyn_new_decl(_tmp7D4,loc);});_tmp133->hd=_tmp7D5;}),_tmp133->tl=0;_tmp133;});_npop_handler(0U);return _tmp134;};}else{_LL1D: _tmp158=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownDatatype).val).name;_tmp157=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp156=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f2;_LL1E: {
# 920
struct Cyc_List_List*_tmp135=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp156);
struct Cyc_Absyn_Decl*_tmp136=Cyc_Absyn_datatype_decl(s,_tmp158,_tmp135,0,_tmp157,loc);
if(_tmp173 != 0)({void*_tmp137=0U;({unsigned int _tmp7D7=loc;struct _dyneither_ptr _tmp7D6=({const char*_tmp138="bad attributes on datatype";_tag_dyneither(_tmp138,sizeof(char),27U);});Cyc_Warn_err(_tmp7D7,_tmp7D6,_tag_dyneither(_tmp137,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=_tmp136,_tmp139->tl=0;_tmp139;});_npop_handler(0U);return _tmp13A;};}}case 15U: _LL1F: _tmp159=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1;_LL20: {
# 925
struct Cyc_Absyn_Enumdecl*_tmp13B=({struct Cyc_Absyn_Enumdecl*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->sc=s,_tmp142->name=_tmp159,_tmp142->fields=0;_tmp142;});
if(_tmp173 != 0)({void*_tmp13C=0U;({unsigned int _tmp7D9=loc;struct _dyneither_ptr _tmp7D8=({const char*_tmp13D="bad attributes on enum";_tag_dyneither(_tmp13D,sizeof(char),23U);});Cyc_Warn_err(_tmp7D9,_tmp7D8,_tag_dyneither(_tmp13C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp141=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_Absyn_Decl*_tmp7DB=({struct Cyc_Absyn_Decl*_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp7DA=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->tag=7U,_tmp13E->f1=_tmp13B;_tmp13E;});_tmp13F->r=_tmp7DA;}),_tmp13F->loc=loc;_tmp13F;});_tmp140->hd=_tmp7DB;}),_tmp140->tl=0;_tmp140;});_npop_handler(0U);return _tmp141;};}case 16U: _LL21: _tmp15A=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1;_LL22: {
# 931
struct Cyc_Absyn_Enumdecl*_tmp143=({struct Cyc_Absyn_Enumdecl*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->sc=s,({struct _tuple0*_tmp7DD=Cyc_Parse_gensym_enum();_tmp14B->name=_tmp7DD;}),({struct Cyc_Core_Opt*_tmp7DC=({struct Cyc_Core_Opt*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->v=_tmp15A;_tmp14A;});_tmp14B->fields=_tmp7DC;});_tmp14B;});
if(_tmp173 != 0)({void*_tmp144=0U;({unsigned int _tmp7DF=loc;struct _dyneither_ptr _tmp7DE=({const char*_tmp145="bad attributes on enum";_tag_dyneither(_tmp145,sizeof(char),23U);});Cyc_Warn_err(_tmp7DF,_tmp7DE,_tag_dyneither(_tmp144,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Decl*_tmp7E1=({struct Cyc_Absyn_Decl*_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp7E0=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->tag=7U,_tmp146->f1=_tmp143;_tmp146;});_tmp147->r=_tmp7E0;}),_tmp147->loc=loc;_tmp147;});_tmp148->hd=_tmp7E1;}),_tmp148->tl=0;_tmp148;});_npop_handler(0U);return _tmp149;};}default: goto _LL23;}default: _LL23: _LL24:
({void*_tmp14C=0U;({unsigned int _tmp7E3=loc;struct _dyneither_ptr _tmp7E2=({const char*_tmp14D="missing declarator";_tag_dyneither(_tmp14D,sizeof(char),19U);});Cyc_Warn_err(_tmp7E3,_tmp7E2,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp14E=0;_npop_handler(0U);return _tmp14E;};}_LL12:;}else{
# 938
struct Cyc_List_List*_tmp15B=Cyc_Parse_apply_tmss(mkrgn,_tmp175,_tmp119,declarators,_tmp173);
if(istypedef){
# 943
if(!exps_empty)
({void*_tmp15C=0U;({unsigned int _tmp7E5=loc;struct _dyneither_ptr _tmp7E4=({const char*_tmp15D="initializer in typedef declaration";_tag_dyneither(_tmp15D,sizeof(char),35U);});Cyc_Warn_err(_tmp7E5,_tmp7E4,_tag_dyneither(_tmp15C,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp15B);
struct Cyc_List_List*_tmp15E=decls;_npop_handler(0U);return _tmp15E;};}else{
# 949
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp15F=_tmp15B;for(0;_tmp15F != 0;(_tmp15F=_tmp15F->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp160=(struct _tuple15*)_tmp15F->hd;struct _tuple15*_tmp161=_tmp160;unsigned int _tmp171;struct _tuple0*_tmp170;struct Cyc_Absyn_Tqual _tmp16F;void*_tmp16E;struct Cyc_List_List*_tmp16D;struct Cyc_List_List*_tmp16C;_LL26: _tmp171=_tmp161->f1;_tmp170=_tmp161->f2;_tmp16F=_tmp161->f3;_tmp16E=_tmp161->f4;_tmp16D=_tmp161->f5;_tmp16C=_tmp161->f6;_LL27:;
if(_tmp16D != 0)
({void*_tmp162=0U;({unsigned int _tmp7E7=loc;struct _dyneither_ptr _tmp7E6=({const char*_tmp163="bad type params, ignoring";_tag_dyneither(_tmp163,sizeof(char),26U);});Cyc_Warn_warn(_tmp7E7,_tmp7E6,_tag_dyneither(_tmp162,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp164=0U;({unsigned int _tmp7E9=loc;struct _dyneither_ptr _tmp7E8=({const char*_tmp165="unexpected NULL in parse!";_tag_dyneither(_tmp165,sizeof(char),26U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7E9,_tmp7E8,_tag_dyneither(_tmp164,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp166=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp167=Cyc_Absyn_new_vardecl(_tmp171,_tmp170,_tmp16E,_tmp166);
_tmp167->tq=_tmp16F;
_tmp167->sc=s;
_tmp167->attributes=_tmp16C;{
struct Cyc_Absyn_Decl*_tmp168=({struct Cyc_Absyn_Decl*_tmp16B=_cycalloc(sizeof(*_tmp16B));({void*_tmp7EA=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=0U,_tmp16A->f1=_tmp167;_tmp16A;});_tmp16B->r=_tmp7EA;}),_tmp16B->loc=loc;_tmp16B;});
decls=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=_tmp168,_tmp169->tl=decls;_tmp169;});};};}}{
# 964
struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp172;};}}};};};};}
# 861
;_pop_region(mkrgn);}
# 970
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp176=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp177=_tmp176;switch(_tmp177){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 980
{char _tmp178=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp179=_tmp178;switch(_tmp179){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 987
goto _LL0;case 84U: _LLF: _LL10:
# 989
{char _tmp17A=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp17B=_tmp17A;switch(_tmp17B){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 996
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 999
({struct Cyc_String_pa_PrintArg_struct _tmp17E=({struct Cyc_String_pa_PrintArg_struct _tmp6BF;_tmp6BF.tag=0U,_tmp6BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp6BF;});struct Cyc_Int_pa_PrintArg_struct _tmp17F=({struct Cyc_Int_pa_PrintArg_struct _tmp6BE;_tmp6BE.tag=1U,({unsigned long _tmp7EB=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp6BE.f1=_tmp7EB;});_tmp6BE;});void*_tmp17C[2U];_tmp17C[0]=& _tmp17E,_tmp17C[1]=& _tmp17F;({unsigned int _tmp7ED=loc;struct _dyneither_ptr _tmp7EC=({const char*_tmp17D="bad kind: %s; strlen=%d";_tag_dyneither(_tmp17D,sizeof(char),24U);});Cyc_Warn_err(_tmp7ED,_tmp7EC,_tag_dyneither(_tmp17C,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 1004
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp180=e->r;void*_tmp181=_tmp180;int _tmp184;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181)->f1).Int_c).tag == 5){_LL1: _tmp184=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181)->f1).Int_c).val).f2;_LL2:
 return _tmp184;}else{goto _LL3;}}else{_LL3: _LL4:
# 1008
({void*_tmp182=0U;({unsigned int _tmp7EF=loc;struct _dyneither_ptr _tmp7EE=({const char*_tmp183="expecting integer constant";_tag_dyneither(_tmp183,sizeof(char),27U);});Cyc_Warn_err(_tmp7EF,_tmp7EE,_tag_dyneither(_tmp182,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 1014
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp185=e->r;void*_tmp186=_tmp185;struct _dyneither_ptr _tmp189;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp186)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp186)->f1).String_c).tag == 8){_LL1: _tmp189=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp186)->f1).String_c).val;_LL2:
 return _tmp189;}else{goto _LL3;}}else{_LL3: _LL4:
# 1018
({void*_tmp187=0U;({unsigned int _tmp7F1=loc;struct _dyneither_ptr _tmp7F0=({const char*_tmp188="expecting string constant";_tag_dyneither(_tmp188,sizeof(char),26U);});Cyc_Warn_err(_tmp7F1,_tmp7F0,_tag_dyneither(_tmp187,sizeof(void*),0U));});});
return _tag_dyneither(0,0,0);}_LL0:;}
# 1024
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp18A=x;long long _tmp192;char _tmp191;int _tmp190;switch((_tmp18A.LongLong_c).tag){case 5U: _LL1: _tmp190=((_tmp18A.Int_c).val).f2;_LL2:
 return(unsigned int)_tmp190;case 2U: _LL3: _tmp191=((_tmp18A.Char_c).val).f2;_LL4:
 return(unsigned int)_tmp191;case 6U: _LL5: _tmp192=((_tmp18A.LongLong_c).val).f2;_LL6: {
# 1029
unsigned long long y=(unsigned long long)_tmp192;
if(y > -1)
({void*_tmp18B=0U;({unsigned int _tmp7F3=loc;struct _dyneither_ptr _tmp7F2=({const char*_tmp18C="integer constant too large";_tag_dyneither(_tmp18C,sizeof(char),27U);});Cyc_Warn_err(_tmp7F3,_tmp7F2,_tag_dyneither(_tmp18B,sizeof(void*),0U));});});
return(unsigned int)_tmp192;}default: _LL7: _LL8:
# 1034
({struct Cyc_String_pa_PrintArg_struct _tmp18F=({struct Cyc_String_pa_PrintArg_struct _tmp6C0;_tmp6C0.tag=0U,({struct _dyneither_ptr _tmp7F4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp6C0.f1=_tmp7F4;});_tmp6C0;});void*_tmp18D[1U];_tmp18D[0]=& _tmp18F;({unsigned int _tmp7F6=loc;struct _dyneither_ptr _tmp7F5=({const char*_tmp18E="expected integer constant but found %s";_tag_dyneither(_tmp18E,sizeof(char),39U);});Cyc_Warn_err(_tmp7F6,_tmp7F5,_tag_dyneither(_tmp18D,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1040
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp193=p->r;void*_tmp194=_tmp193;struct Cyc_Absyn_Exp*_tmp1A1;struct _tuple0*_tmp1A0;struct Cyc_List_List*_tmp19F;struct _dyneither_ptr _tmp19E;int _tmp19D;char _tmp19C;enum Cyc_Absyn_Sign _tmp19B;int _tmp19A;struct Cyc_Absyn_Pat*_tmp199;struct Cyc_Absyn_Vardecl*_tmp198;struct _tuple0*_tmp197;switch(*((int*)_tmp194)){case 15U: _LL1: _tmp197=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL2:
 return Cyc_Absyn_unknownid_exp(_tmp197,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp194)->f2)->r)->tag == 0U){_LL3: _tmp198=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL4:
# 1044
 return({struct Cyc_Absyn_Exp*_tmp7F7=Cyc_Absyn_unknownid_exp(_tmp198->name,p->loc);Cyc_Absyn_deref_exp(_tmp7F7,p->loc);});}else{goto _LL13;}case 6U: _LL5: _tmp199=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL6:
 return({struct Cyc_Absyn_Exp*_tmp7F8=Cyc_Parse_pat2exp(_tmp199);Cyc_Absyn_address_exp(_tmp7F8,p->loc);});case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp19B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_tmp19A=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp194)->f2;_LLA:
 return Cyc_Absyn_int_exp(_tmp19B,_tmp19A,p->loc);case 11U: _LLB: _tmp19C=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LLC:
 return Cyc_Absyn_char_exp(_tmp19C,p->loc);case 12U: _LLD: _tmp19E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_tmp19D=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp194)->f2;_LLE:
 return Cyc_Absyn_float_exp(_tmp19E,_tmp19D,p->loc);case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp194)->f3 == 0){_LLF: _tmp1A0=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_tmp19F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp194)->f2;_LL10: {
# 1051
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1A0,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp19F);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp1A1=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL12:
 return _tmp1A1;default: _LL13: _LL14:
# 1056
({void*_tmp195=0U;({unsigned int _tmp7FA=p->loc;struct _dyneither_ptr _tmp7F9=({const char*_tmp196="cannot mix patterns and expressions in case";_tag_dyneither(_tmp196,sizeof(char),44U);});Cyc_Warn_err(_tmp7FA,_tmp7F9,_tag_dyneither(_tmp195,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1A2[7U]="cnst_t";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1A2,_tmp1A2,_tmp1A2 + 7U}};
# 1115
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1A3=yy1;union Cyc_Absyn_Cnst _tmp1A4;if((_tmp1A3.Int_tok).tag == 1){_LL1: _tmp1A4=(_tmp1A3.Int_tok).val;_LL2:
# 1119
 yyzzz=_tmp1A4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL0:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6C1;(_tmp6C1.Int_tok).tag=1U,(_tmp6C1.Int_tok).val=yy1;_tmp6C1;});}static char _tmp1A5[5U]="char";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1A5,_tmp1A5,_tmp1A5 + 5U}};
# 1116
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1A6=yy1;char _tmp1A7;if((_tmp1A6.Char_tok).tag == 2){_LL1: _tmp1A7=(_tmp1A6.Char_tok).val;_LL2:
# 1120
 yyzzz=_tmp1A7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL0:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6C2;(_tmp6C2.Char_tok).tag=2U,(_tmp6C2.Char_tok).val=yy1;_tmp6C2;});}static char _tmp1A8[13U]="string_t<`H>";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1A8,_tmp1A8,_tmp1A8 + 13U}};
# 1117
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1A9=yy1;struct _dyneither_ptr _tmp1AA;if((_tmp1A9.String_tok).tag == 3){_LL1: _tmp1AA=(_tmp1A9.String_tok).val;_LL2:
# 1121
 yyzzz=_tmp1AA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL0:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp6C3;(_tmp6C3.String_tok).tag=3U,(_tmp6C3.String_tok).val=yy1;_tmp6C3;});}static char _tmp1AB[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1118 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1AB,_tmp1AB,_tmp1AB + 45U}};
# 1120
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1AC=yy1;struct _tuple21*_tmp1AD;if((_tmp1AC.YY1).tag == 9){_LL1: _tmp1AD=(_tmp1AC.YY1).val;_LL2:
# 1124
 yyzzz=_tmp1AD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL0:;}
# 1128
return yyzzz;}
# 1130
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp6C4;(_tmp6C4.YY1).tag=9U,(_tmp6C4.YY1).val=yy1;_tmp6C4;});}static char _tmp1AE[11U]="ptrbound_t";
# 1119 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1AE,_tmp1AE,_tmp1AE + 11U}};
# 1121
void*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp1AF=yy1;void*_tmp1B0;if((_tmp1AF.YY2).tag == 10){_LL1: _tmp1B0=(_tmp1AF.YY2).val;_LL2:
# 1125
 yyzzz=_tmp1B0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL0:;}
# 1129
return yyzzz;}
# 1131
union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6C5;(_tmp6C5.YY2).tag=10U,(_tmp6C5.YY2).val=yy1;_tmp6C5;});}static char _tmp1B1[28U]="list_t<offsetof_field_t,`H>";
# 1120 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1B1,_tmp1B1,_tmp1B1 + 28U}};
# 1122
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1B2=yy1;struct Cyc_List_List*_tmp1B3;if((_tmp1B2.YY3).tag == 11){_LL1: _tmp1B3=(_tmp1B2.YY3).val;_LL2:
# 1126
 yyzzz=_tmp1B3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL0:;}
# 1130
return yyzzz;}
# 1132
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C6;(_tmp6C6.YY3).tag=11U,(_tmp6C6.YY3).val=yy1;_tmp6C6;});}static char _tmp1B4[6U]="exp_t";
# 1121 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1B4,_tmp1B4,_tmp1B4 + 6U}};
# 1123
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1B5=yy1;struct Cyc_Absyn_Exp*_tmp1B6;if((_tmp1B5.Exp_tok).tag == 7){_LL1: _tmp1B6=(_tmp1B5.Exp_tok).val;_LL2:
# 1127
 yyzzz=_tmp1B6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL0:;}
# 1131
return yyzzz;}
# 1133
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6C7;(_tmp6C7.Exp_tok).tag=7U,(_tmp6C7.Exp_tok).val=yy1;_tmp6C7;});}static char _tmp1B7[17U]="list_t<exp_t,`H>";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1B7,_tmp1B7,_tmp1B7 + 17U}};
# 1131
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1B8=yy1;struct Cyc_List_List*_tmp1B9;if((_tmp1B8.YY4).tag == 12){_LL1: _tmp1B9=(_tmp1B8.YY4).val;_LL2:
# 1135
 yyzzz=_tmp1B9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL0:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C8;(_tmp6C8.YY4).tag=12U,(_tmp6C8.YY4).val=yy1;_tmp6C8;});}static char _tmp1BA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1BA,_tmp1BA,_tmp1BA + 47U}};
# 1132
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1BB=yy1;struct Cyc_List_List*_tmp1BC;if((_tmp1BB.YY5).tag == 13){_LL1: _tmp1BC=(_tmp1BB.YY5).val;_LL2:
# 1136
 yyzzz=_tmp1BC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL0:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C9;(_tmp6C9.YY5).tag=13U,(_tmp6C9.YY5).val=yy1;_tmp6C9;});}static char _tmp1BD[9U]="primop_t";
# 1131 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1BD,_tmp1BD,_tmp1BD + 9U}};
# 1133
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1BE=yy1;enum Cyc_Absyn_Primop _tmp1BF;if((_tmp1BE.YY6).tag == 14){_LL1: _tmp1BF=(_tmp1BE.YY6).val;_LL2:
# 1137
 yyzzz=_tmp1BF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL0:;}
# 1141
return yyzzz;}
# 1143
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6CA;(_tmp6CA.YY6).tag=14U,(_tmp6CA.YY6).val=yy1;_tmp6CA;});}static char _tmp1C0[19U]="opt_t<primop_t,`H>";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1C0,_tmp1C0,_tmp1C0 + 19U}};
# 1134
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1C1=yy1;struct Cyc_Core_Opt*_tmp1C2;if((_tmp1C1.YY7).tag == 15){_LL1: _tmp1C2=(_tmp1C1.YY7).val;_LL2:
# 1138
 yyzzz=_tmp1C2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL0:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6CB;(_tmp6CB.YY7).tag=15U,(_tmp6CB.YY7).val=yy1;_tmp6CB;});}static char _tmp1C3[7U]="qvar_t";
# 1133 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1C3,_tmp1C3,_tmp1C3 + 7U}};
# 1135
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1C4=yy1;struct _tuple0*_tmp1C5;if((_tmp1C4.QualId_tok).tag == 5){_LL1: _tmp1C5=(_tmp1C4.QualId_tok).val;_LL2:
# 1139
 yyzzz=_tmp1C5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL0:;}
# 1143
return yyzzz;}
# 1145
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6CC;(_tmp6CC.QualId_tok).tag=5U,(_tmp6CC.QualId_tok).val=yy1;_tmp6CC;});}static char _tmp1C6[7U]="stmt_t";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp1C6,_tmp1C6,_tmp1C6 + 7U}};
# 1138
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp1C7=yy1;struct Cyc_Absyn_Stmt*_tmp1C8;if((_tmp1C7.Stmt_tok).tag == 8){_LL1: _tmp1C8=(_tmp1C7.Stmt_tok).val;_LL2:
# 1142
 yyzzz=_tmp1C8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL0:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6CD;(_tmp6CD.Stmt_tok).tag=8U,(_tmp6CD.Stmt_tok).val=yy1;_tmp6CD;});}static char _tmp1C9[27U]="list_t<switch_clause_t,`H>";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1C9,_tmp1C9,_tmp1C9 + 27U}};
# 1142
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1CA=yy1;struct Cyc_List_List*_tmp1CB;if((_tmp1CA.YY8).tag == 16){_LL1: _tmp1CB=(_tmp1CA.YY8).val;_LL2:
# 1146
 yyzzz=_tmp1CB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL0:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CE;(_tmp6CE.YY8).tag=16U,(_tmp6CE.YY8).val=yy1;_tmp6CE;});}static char _tmp1CC[6U]="pat_t";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1CC,_tmp1CC,_tmp1CC + 6U}};
# 1143
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp1CD=yy1;struct Cyc_Absyn_Pat*_tmp1CE;if((_tmp1CD.YY9).tag == 17){_LL1: _tmp1CE=(_tmp1CD.YY9).val;_LL2:
# 1147
 yyzzz=_tmp1CE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL0:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6CF;(_tmp6CF.YY9).tag=17U,(_tmp6CF.YY9).val=yy1;_tmp6CF;});}static char _tmp1CF[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1CF,_tmp1CF,_tmp1CF + 28U}};
# 1148
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1D0=yy1;struct _tuple22*_tmp1D1;if((_tmp1D0.YY10).tag == 18){_LL1: _tmp1D1=(_tmp1D0.YY10).val;_LL2:
# 1152
 yyzzz=_tmp1D1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL0:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6D0;(_tmp6D0.YY10).tag=18U,(_tmp6D0.YY10).val=yy1;_tmp6D0;});}static char _tmp1D2[17U]="list_t<pat_t,`H>";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1D2,_tmp1D2,_tmp1D2 + 17U}};
# 1149
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1D3=yy1;struct Cyc_List_List*_tmp1D4;if((_tmp1D3.YY11).tag == 19){_LL1: _tmp1D4=(_tmp1D3.YY11).val;_LL2:
# 1153
 yyzzz=_tmp1D4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL0:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D1;(_tmp6D1.YY11).tag=19U,(_tmp6D1.YY11).val=yy1;_tmp6D1;});}static char _tmp1D5[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1D5,_tmp1D5,_tmp1D5 + 36U}};
# 1150
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp1D6=yy1;struct _tuple23*_tmp1D7;if((_tmp1D6.YY12).tag == 20){_LL1: _tmp1D7=(_tmp1D6.YY12).val;_LL2:
# 1154
 yyzzz=_tmp1D7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL0:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6D2;(_tmp6D2.YY12).tag=20U,(_tmp6D2.YY12).val=yy1;_tmp6D2;});}static char _tmp1D8[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1D8,_tmp1D8,_tmp1D8 + 47U}};
# 1151
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1D9=yy1;struct Cyc_List_List*_tmp1DA;if((_tmp1D9.YY13).tag == 21){_LL1: _tmp1DA=(_tmp1D9.YY13).val;_LL2:
# 1155
 yyzzz=_tmp1DA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL0:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D3;(_tmp6D3.YY13).tag=21U,(_tmp6D3.YY13).val=yy1;_tmp6D3;});}static char _tmp1DB[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 58U}};
# 1152
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1DC=yy1;struct _tuple22*_tmp1DD;if((_tmp1DC.YY14).tag == 22){_LL1: _tmp1DD=(_tmp1DC.YY14).val;_LL2:
# 1156
 yyzzz=_tmp1DD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL0:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6D4;(_tmp6D4.YY14).tag=22U,(_tmp6D4.YY14).val=yy1;_tmp6D4;});}static char _tmp1DE[9U]="fndecl_t";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 9U}};
# 1153
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp1DF=yy1;struct Cyc_Absyn_Fndecl*_tmp1E0;if((_tmp1DF.YY15).tag == 23){_LL1: _tmp1E0=(_tmp1DF.YY15).val;_LL2:
# 1157
 yyzzz=_tmp1E0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL0:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6D5;(_tmp6D5.YY15).tag=23U,(_tmp6D5.YY15).val=yy1;_tmp6D5;});}static char _tmp1E1[18U]="list_t<decl_t,`H>";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1E1,_tmp1E1,_tmp1E1 + 18U}};
# 1154
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E2=yy1;struct Cyc_List_List*_tmp1E3;if((_tmp1E2.YY16).tag == 24){_LL1: _tmp1E3=(_tmp1E2.YY16).val;_LL2:
# 1158
 yyzzz=_tmp1E3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL0:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D6;(_tmp6D6.YY16).tag=24U,(_tmp6D6.YY16).val=yy1;_tmp6D6;});}static char _tmp1E4[12U]="decl_spec_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1E4,_tmp1E4,_tmp1E4 + 12U}};
# 1156
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp1E5=yy1;struct Cyc_Parse_Declaration_spec _tmp1E6;if((_tmp1E5.YY17).tag == 25){_LL1: _tmp1E6=(_tmp1E5.YY17).val;_LL2:
# 1160
 yyzzz=_tmp1E6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL0:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6D7;(_tmp6D7.YY17).tag=25U,(_tmp6D7.YY17).val=yy1;_tmp6D7;});}static char _tmp1E7[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1E7,_tmp1E7,_tmp1E7 + 31U}};
# 1157
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp1E8=yy1;struct _tuple11 _tmp1E9;if((_tmp1E8.YY18).tag == 26){_LL1: _tmp1E9=(_tmp1E8.YY18).val;_LL2:
# 1161
 yyzzz=_tmp1E9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL0:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.YY18).tag=26U,(_tmp6D8.YY18).val=yy1;_tmp6D8;});}static char _tmp1EA[23U]="declarator_list_t<`yy>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 23U}};
# 1158
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp1EB=yy1;struct _tuple12*_tmp1EC;if((_tmp1EB.YY19).tag == 27){_LL1: _tmp1EC=(_tmp1EB.YY19).val;_LL2:
# 1162
 yyzzz=_tmp1EC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL0:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.YY19).tag=27U,(_tmp6D9.YY19).val=yy1;_tmp6D9;});}static char _tmp1ED[19U]="storage_class_t@`H";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 19U}};
# 1159
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp1EE=yy1;enum Cyc_Parse_Storage_class*_tmp1EF;if((_tmp1EE.YY20).tag == 28){_LL1: _tmp1EF=(_tmp1EE.YY20).val;_LL2:
# 1163
 yyzzz=_tmp1EF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL0:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.YY20).tag=28U,(_tmp6DA.YY20).val=yy1;_tmp6DA;});}static char _tmp1F0[17U]="type_specifier_t";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1F0,_tmp1F0,_tmp1F0 + 17U}};
# 1160
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp1F1=yy1;struct Cyc_Parse_Type_specifier _tmp1F2;if((_tmp1F1.YY21).tag == 29){_LL1: _tmp1F2=(_tmp1F1.YY21).val;_LL2:
# 1164
 yyzzz=_tmp1F2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL0:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.YY21).tag=29U,(_tmp6DB.YY21).val=yy1;_tmp6DB;});}static char _tmp1F3[12U]="aggr_kind_t";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 12U}};
# 1162
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp1F4=yy1;enum Cyc_Absyn_AggrKind _tmp1F5;if((_tmp1F4.YY22).tag == 30){_LL1: _tmp1F5=(_tmp1F4.YY22).val;_LL2:
# 1166
 yyzzz=_tmp1F5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL0:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.YY22).tag=30U,(_tmp6DC.YY22).val=yy1;_tmp6DC;});}static char _tmp1F6[8U]="tqual_t";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 8U}};
# 1163
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp1F7=yy1;struct Cyc_Absyn_Tqual _tmp1F8;if((_tmp1F7.YY23).tag == 31){_LL1: _tmp1F8=(_tmp1F7.YY23).val;_LL2:
# 1167
 yyzzz=_tmp1F8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL0:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.YY23).tag=31U,(_tmp6DD.YY23).val=yy1;_tmp6DD;});}static char _tmp1F9[23U]="list_t<aggrfield_t,`H>";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1F9,_tmp1F9,_tmp1F9 + 23U}};
# 1164
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1FA=yy1;struct Cyc_List_List*_tmp1FB;if((_tmp1FA.YY24).tag == 32){_LL1: _tmp1FB=(_tmp1FA.YY24).val;_LL2:
# 1168
 yyzzz=_tmp1FB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL0:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY24).tag=32U,(_tmp6DE.YY24).val=yy1;_tmp6DE;});}static char _tmp1FC[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 34U}};
# 1165
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1FD=yy1;struct Cyc_List_List*_tmp1FE;if((_tmp1FD.YY25).tag == 33){_LL1: _tmp1FE=(_tmp1FD.YY25).val;_LL2:
# 1169
 yyzzz=_tmp1FE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL0:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY25).tag=33U,(_tmp6DF.YY25).val=yy1;_tmp6DF;});}static char _tmp1FF[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FF,_tmp1FF,_tmp1FF + 33U}};
# 1166
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp200=yy1;struct Cyc_List_List*_tmp201;if((_tmp200.YY26).tag == 34){_LL1: _tmp201=(_tmp200.YY26).val;_LL2:
# 1170
 yyzzz=_tmp201;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL0:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY26).tag=34U,(_tmp6E0.YY26).val=yy1;_tmp6E0;});}static char _tmp202[18U]="declarator_t<`yy>";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp202,_tmp202,_tmp202 + 18U}};
# 1167
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp203=yy1;struct Cyc_Parse_Declarator _tmp204;if((_tmp203.YY27).tag == 35){_LL1: _tmp204=(_tmp203.YY27).val;_LL2:
# 1171
 yyzzz=_tmp204;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL0:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY27).tag=35U,(_tmp6E1.YY27).val=yy1;_tmp6E1;});}static char _tmp205[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp205,_tmp205,_tmp205 + 45U}};
# 1168
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp206=yy1;struct _tuple24*_tmp207;if((_tmp206.YY28).tag == 36){_LL1: _tmp207=(_tmp206.YY28).val;_LL2:
# 1172
 yyzzz=_tmp207;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL0:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY28).tag=36U,(_tmp6E2.YY28).val=yy1;_tmp6E2;});}static char _tmp208[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp208,_tmp208,_tmp208 + 57U}};
# 1169
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp209=yy1;struct Cyc_List_List*_tmp20A;if((_tmp209.YY29).tag == 37){_LL1: _tmp20A=(_tmp209.YY29).val;_LL2:
# 1173
 yyzzz=_tmp20A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL0:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY29).tag=37U,(_tmp6E3.YY29).val=yy1;_tmp6E3;});}static char _tmp20B[26U]="abstractdeclarator_t<`yy>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 26U}};
# 1170
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct Cyc_Parse_Abstractdeclarator _tmp20D;if((_tmp20C.YY30).tag == 38){_LL1: _tmp20D=(_tmp20C.YY30).val;_LL2:
# 1174
 yyzzz=_tmp20D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL0:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY30).tag=38U,(_tmp6E4.YY30).val=yy1;_tmp6E4;});}static char _tmp20E[5U]="bool";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp20E,_tmp20E,_tmp20E + 5U}};
# 1171
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp20F=yy1;int _tmp210;if((_tmp20F.YY31).tag == 39){_LL1: _tmp210=(_tmp20F.YY31).val;_LL2:
# 1175
 yyzzz=_tmp210;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL0:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY31).tag=39U,(_tmp6E5.YY31).val=yy1;_tmp6E5;});}static char _tmp211[8U]="scope_t";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp211,_tmp211,_tmp211 + 8U}};
# 1172
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp212=yy1;enum Cyc_Absyn_Scope _tmp213;if((_tmp212.YY32).tag == 40){_LL1: _tmp213=(_tmp212.YY32).val;_LL2:
# 1176
 yyzzz=_tmp213;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL0:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY32).tag=40U,(_tmp6E6.YY32).val=yy1;_tmp6E6;});}static char _tmp214[16U]="datatypefield_t";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp214,_tmp214,_tmp214 + 16U}};
# 1173
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp215=yy1;struct Cyc_Absyn_Datatypefield*_tmp216;if((_tmp215.YY33).tag == 41){_LL1: _tmp216=(_tmp215.YY33).val;_LL2:
# 1177
 yyzzz=_tmp216;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL0:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY33).tag=41U,(_tmp6E7.YY33).val=yy1;_tmp6E7;});}static char _tmp217[27U]="list_t<datatypefield_t,`H>";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp217,_tmp217,_tmp217 + 27U}};
# 1174
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp218=yy1;struct Cyc_List_List*_tmp219;if((_tmp218.YY34).tag == 42){_LL1: _tmp219=(_tmp218.YY34).val;_LL2:
# 1178
 yyzzz=_tmp219;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL0:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY34).tag=42U,(_tmp6E8.YY34).val=yy1;_tmp6E8;});}static char _tmp21A[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 41U}};
# 1175
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;struct _tuple25 _tmp21C;if((_tmp21B.YY35).tag == 43){_LL1: _tmp21C=(_tmp21B.YY35).val;_LL2:
# 1179
 yyzzz=_tmp21C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL0:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY35).tag=43U,(_tmp6E9.YY35).val=yy1;_tmp6E9;});}static char _tmp21D[17U]="list_t<var_t,`H>";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp21D,_tmp21D,_tmp21D + 17U}};
# 1176
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21E=yy1;struct Cyc_List_List*_tmp21F;if((_tmp21E.YY36).tag == 44){_LL1: _tmp21F=(_tmp21E.YY36).val;_LL2:
# 1180
 yyzzz=_tmp21F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL0:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY36).tag=44U,(_tmp6EA.YY36).val=yy1;_tmp6EA;});}static char _tmp220[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp220,_tmp220,_tmp220 + 31U}};
# 1177
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp221=yy1;struct _tuple8*_tmp222;if((_tmp221.YY37).tag == 45){_LL1: _tmp222=(_tmp221.YY37).val;_LL2:
# 1181
 yyzzz=_tmp222;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL0:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY37).tag=45U,(_tmp6EB.YY37).val=yy1;_tmp6EB;});}static char _tmp223[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp223,_tmp223,_tmp223 + 42U}};
# 1178
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp224=yy1;struct Cyc_List_List*_tmp225;if((_tmp224.YY38).tag == 46){_LL1: _tmp225=(_tmp224.YY38).val;_LL2:
# 1182
 yyzzz=_tmp225;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL0:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY38).tag=46U,(_tmp6EC.YY38).val=yy1;_tmp6EC;});}static char _tmp226[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp226,_tmp226,_tmp226 + 115U}};
# 1179
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp227=yy1;struct _tuple26*_tmp228;if((_tmp227.YY39).tag == 47){_LL1: _tmp228=(_tmp227.YY39).val;_LL2:
# 1183
 yyzzz=_tmp228;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL0:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY39).tag=47U,(_tmp6ED.YY39).val=yy1;_tmp6ED;});}static char _tmp229[8U]="types_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 8U}};
# 1180
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_List_List*_tmp22B;if((_tmp22A.YY40).tag == 48){_LL1: _tmp22B=(_tmp22A.YY40).val;_LL2:
# 1184
 yyzzz=_tmp22B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL0:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY40).tag=48U,(_tmp6EE.YY40).val=yy1;_tmp6EE;});}static char _tmp22C[24U]="list_t<designator_t,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp22C,_tmp22C,_tmp22C + 24U}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22D=yy1;struct Cyc_List_List*_tmp22E;if((_tmp22D.YY41).tag == 49){_LL1: _tmp22E=(_tmp22D.YY41).val;_LL2:
# 1186
 yyzzz=_tmp22E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL0:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY41).tag=49U,(_tmp6EF.YY41).val=yy1;_tmp6EF;});}static char _tmp22F[13U]="designator_t";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 13U}};
# 1183
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp230=yy1;void*_tmp231;if((_tmp230.YY42).tag == 50){_LL1: _tmp231=(_tmp230.YY42).val;_LL2:
# 1187
 yyzzz=_tmp231;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL0:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY42).tag=50U,(_tmp6F0.YY42).val=yy1;_tmp6F0;});}static char _tmp232[7U]="kind_t";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp232,_tmp232,_tmp232 + 7U}};
# 1184
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp233=yy1;struct Cyc_Absyn_Kind*_tmp234;if((_tmp233.YY43).tag == 51){_LL1: _tmp234=(_tmp233.YY43).val;_LL2:
# 1188
 yyzzz=_tmp234;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL0:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY43).tag=51U,(_tmp6F1.YY43).val=yy1;_tmp6F1;});}static char _tmp235[7U]="type_t";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp235,_tmp235,_tmp235 + 7U}};
# 1185
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp236=yy1;void*_tmp237;if((_tmp236.YY44).tag == 52){_LL1: _tmp237=(_tmp236.YY44).val;_LL2:
# 1189
 yyzzz=_tmp237;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL0:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY44).tag=52U,(_tmp6F2.YY44).val=yy1;_tmp6F2;});}static char _tmp238[23U]="list_t<attribute_t,`H>";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 23U}};
# 1186
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct Cyc_List_List*_tmp23A;if((_tmp239.YY45).tag == 53){_LL1: _tmp23A=(_tmp239.YY45).val;_LL2:
# 1190
 yyzzz=_tmp23A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL0:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY45).tag=53U,(_tmp6F3.YY45).val=yy1;_tmp6F3;});}static char _tmp23B[12U]="attribute_t";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp23B,_tmp23B,_tmp23B + 12U}};
# 1187
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp23C=yy1;void*_tmp23D;if((_tmp23C.YY46).tag == 54){_LL1: _tmp23D=(_tmp23C.YY46).val;_LL2:
# 1191
 yyzzz=_tmp23D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL0:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY46).tag=54U,(_tmp6F4.YY46).val=yy1;_tmp6F4;});}static char _tmp23E[12U]="enumfield_t";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp23E,_tmp23E,_tmp23E + 12U}};
# 1188
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp23F=yy1;struct Cyc_Absyn_Enumfield*_tmp240;if((_tmp23F.YY47).tag == 55){_LL1: _tmp240=(_tmp23F.YY47).val;_LL2:
# 1192
 yyzzz=_tmp240;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL0:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY47).tag=55U,(_tmp6F5.YY47).val=yy1;_tmp6F5;});}static char _tmp241[23U]="list_t<enumfield_t,`H>";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp241,_tmp241,_tmp241 + 23U}};
# 1189
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp242=yy1;struct Cyc_List_List*_tmp243;if((_tmp242.YY48).tag == 56){_LL1: _tmp243=(_tmp242.YY48).val;_LL2:
# 1193
 yyzzz=_tmp243;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL0:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY48).tag=56U,(_tmp6F6.YY48).val=yy1;_tmp6F6;});}static char _tmp244[11U]="type_opt_t";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp244,_tmp244,_tmp244 + 11U}};
# 1190
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp245=yy1;void*_tmp246;if((_tmp245.YY49).tag == 57){_LL1: _tmp246=(_tmp245.YY49).val;_LL2:
# 1194
 yyzzz=_tmp246;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL0:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY49).tag=57U,(_tmp6F7.YY49).val=yy1;_tmp6F7;});}static char _tmp247[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 31U}};
# 1191
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_List_List*_tmp249;if((_tmp248.YY50).tag == 58){_LL1: _tmp249=(_tmp248.YY50).val;_LL2:
# 1195
 yyzzz=_tmp249;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL0:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY50).tag=58U,(_tmp6F8.YY50).val=yy1;_tmp6F8;});}static char _tmp24A[11U]="booltype_t";
# 1190 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp24A,_tmp24A,_tmp24A + 11U}};
# 1192
void*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp24B=yy1;void*_tmp24C;if((_tmp24B.YY51).tag == 59){_LL1: _tmp24C=(_tmp24B.YY51).val;_LL2:
# 1196
 yyzzz=_tmp24C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL0:;}
# 1200
return yyzzz;}
# 1202
union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY51).tag=59U,(_tmp6F9.YY51).val=yy1;_tmp6F9;});}static char _tmp24D[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1191 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 45U}};
# 1193
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp24E=yy1;struct Cyc_List_List*_tmp24F;if((_tmp24E.YY52).tag == 60){_LL1: _tmp24F=(_tmp24E.YY52).val;_LL2:
# 1197
 yyzzz=_tmp24F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL0:;}
# 1201
return yyzzz;}
# 1203
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY52).tag=60U,(_tmp6FA.YY52).val=yy1;_tmp6FA;});}static char _tmp250[20U]="pointer_qual_t<`yy>";
# 1192 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp250,_tmp250,_tmp250 + 20U}};
# 1194
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp251=yy1;void*_tmp252;if((_tmp251.YY53).tag == 61){_LL1: _tmp252=(_tmp251.YY53).val;_LL2:
# 1198
 yyzzz=_tmp252;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL0:;}
# 1202
return yyzzz;}
# 1204
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY53).tag=61U,(_tmp6FB.YY53).val=yy1;_tmp6FB;});}static char _tmp253[21U]="pointer_quals_t<`yy>";
# 1193 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 21U}};
# 1195
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp254=yy1;struct Cyc_List_List*_tmp255;if((_tmp254.YY54).tag == 62){_LL1: _tmp255=(_tmp254.YY54).val;_LL2:
# 1199
 yyzzz=_tmp255;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL0:;}
# 1203
return yyzzz;}
# 1205
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY54).tag=62U,(_tmp6FC.YY54).val=yy1;_tmp6FC;});}static char _tmp256[21U]="$(bool,string_t<`H>)";
# 1194 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 21U}};
# 1196
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct _tuple20 _tmp258;if((_tmp257.Asm_tok).tag == 6){_LL1: _tmp258=(_tmp257.Asm_tok).val;_LL2:
# 1200
 yyzzz=_tmp258;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL0:;}
# 1204
return yyzzz;}
# 1206
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.Asm_tok).tag=6U,(_tmp6FD.Asm_tok).val=yy1;_tmp6FD;});}static char _tmp259[10U]="exp_opt_t";
# 1195 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp259,_tmp259,_tmp259 + 10U}};
# 1197
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp25A=yy1;struct Cyc_Absyn_Exp*_tmp25B;if((_tmp25A.YY55).tag == 63){_LL1: _tmp25B=(_tmp25A.YY55).val;_LL2:
# 1201
 yyzzz=_tmp25B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL0:;}
# 1205
return yyzzz;}
# 1207
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY55).tag=63U,(_tmp6FE.YY55).val=yy1;_tmp6FE;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1223
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp6FF;_tmp6FF.timestamp=0,_tmp6FF.first_line=0,_tmp6FF.first_column=0,_tmp6FF.last_line=0,_tmp6FF.last_column=0;_tmp6FF;});}
# 1226
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1237 "parse.y"
static short Cyc_yytranslate[377U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,148,2,2,132,146,143,2,129,130,137,140,125,144,134,145,2,2,2,2,2,2,2,2,2,2,133,122,127,126,128,139,138,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,135,2,136,142,131,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,123,141,124,147,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121};static char _tmp25C[2U]="$";static char _tmp25D[6U]="error";static char _tmp25E[12U]="$undefined.";static char _tmp25F[5U]="AUTO";static char _tmp260[9U]="REGISTER";static char _tmp261[7U]="STATIC";static char _tmp262[7U]="EXTERN";static char _tmp263[8U]="TYPEDEF";static char _tmp264[5U]="VOID";static char _tmp265[5U]="CHAR";static char _tmp266[6U]="SHORT";static char _tmp267[4U]="INT";static char _tmp268[5U]="LONG";static char _tmp269[6U]="FLOAT";static char _tmp26A[7U]="DOUBLE";static char _tmp26B[7U]="SIGNED";static char _tmp26C[9U]="UNSIGNED";static char _tmp26D[6U]="CONST";static char _tmp26E[9U]="VOLATILE";static char _tmp26F[9U]="RESTRICT";static char _tmp270[7U]="STRUCT";static char _tmp271[6U]="UNION";static char _tmp272[5U]="CASE";static char _tmp273[8U]="DEFAULT";static char _tmp274[7U]="INLINE";static char _tmp275[7U]="SIZEOF";static char _tmp276[9U]="OFFSETOF";static char _tmp277[3U]="IF";static char _tmp278[5U]="ELSE";static char _tmp279[7U]="SWITCH";static char _tmp27A[6U]="WHILE";static char _tmp27B[3U]="DO";static char _tmp27C[4U]="FOR";static char _tmp27D[5U]="GOTO";static char _tmp27E[9U]="CONTINUE";static char _tmp27F[6U]="BREAK";static char _tmp280[7U]="RETURN";static char _tmp281[5U]="ENUM";static char _tmp282[7U]="TYPEOF";static char _tmp283[16U]="BUILTIN_VA_LIST";static char _tmp284[10U]="EXTENSION";static char _tmp285[8U]="NULL_kw";static char _tmp286[4U]="LET";static char _tmp287[6U]="THROW";static char _tmp288[4U]="TRY";static char _tmp289[6U]="CATCH";static char _tmp28A[7U]="EXPORT";static char _tmp28B[4U]="NEW";static char _tmp28C[9U]="ABSTRACT";static char _tmp28D[9U]="FALLTHRU";static char _tmp28E[6U]="USING";static char _tmp28F[10U]="NAMESPACE";static char _tmp290[9U]="DATATYPE";static char _tmp291[7U]="MALLOC";static char _tmp292[8U]="RMALLOC";static char _tmp293[15U]="RMALLOC_INLINE";static char _tmp294[7U]="CALLOC";static char _tmp295[8U]="RCALLOC";static char _tmp296[5U]="SWAP";static char _tmp297[9U]="REGION_T";static char _tmp298[6U]="TAG_T";static char _tmp299[7U]="REGION";static char _tmp29A[5U]="RNEW";static char _tmp29B[8U]="REGIONS";static char _tmp29C[7U]="PORTON";static char _tmp29D[8U]="PORTOFF";static char _tmp29E[7U]="PRAGMA";static char _tmp29F[10U]="TEMPESTON";static char _tmp2A0[11U]="TEMPESTOFF";static char _tmp2A1[8U]="NUMELTS";static char _tmp2A2[8U]="VALUEOF";static char _tmp2A3[10U]="VALUEOF_T";static char _tmp2A4[9U]="TAGCHECK";static char _tmp2A5[13U]="NUMELTS_QUAL";static char _tmp2A6[10U]="THIN_QUAL";static char _tmp2A7[9U]="FAT_QUAL";static char _tmp2A8[13U]="NOTNULL_QUAL";static char _tmp2A9[14U]="NULLABLE_QUAL";static char _tmp2AA[14U]="REQUIRES_QUAL";static char _tmp2AB[13U]="ENSURES_QUAL";static char _tmp2AC[12U]="REGION_QUAL";static char _tmp2AD[16U]="NOZEROTERM_QUAL";static char _tmp2AE[14U]="ZEROTERM_QUAL";static char _tmp2AF[12U]="TAGGED_QUAL";static char _tmp2B0[16U]="EXTENSIBLE_QUAL";static char _tmp2B1[7U]="PTR_OP";static char _tmp2B2[7U]="INC_OP";static char _tmp2B3[7U]="DEC_OP";static char _tmp2B4[8U]="LEFT_OP";static char _tmp2B5[9U]="RIGHT_OP";static char _tmp2B6[6U]="LE_OP";static char _tmp2B7[6U]="GE_OP";static char _tmp2B8[6U]="EQ_OP";static char _tmp2B9[6U]="NE_OP";static char _tmp2BA[7U]="AND_OP";static char _tmp2BB[6U]="OR_OP";static char _tmp2BC[11U]="MUL_ASSIGN";static char _tmp2BD[11U]="DIV_ASSIGN";static char _tmp2BE[11U]="MOD_ASSIGN";static char _tmp2BF[11U]="ADD_ASSIGN";static char _tmp2C0[11U]="SUB_ASSIGN";static char _tmp2C1[12U]="LEFT_ASSIGN";static char _tmp2C2[13U]="RIGHT_ASSIGN";static char _tmp2C3[11U]="AND_ASSIGN";static char _tmp2C4[11U]="XOR_ASSIGN";static char _tmp2C5[10U]="OR_ASSIGN";static char _tmp2C6[9U]="ELLIPSIS";static char _tmp2C7[11U]="LEFT_RIGHT";static char _tmp2C8[12U]="COLON_COLON";static char _tmp2C9[11U]="IDENTIFIER";static char _tmp2CA[17U]="INTEGER_CONSTANT";static char _tmp2CB[7U]="STRING";static char _tmp2CC[8U]="WSTRING";static char _tmp2CD[19U]="CHARACTER_CONSTANT";static char _tmp2CE[20U]="WCHARACTER_CONSTANT";static char _tmp2CF[18U]="FLOATING_CONSTANT";static char _tmp2D0[9U]="TYPE_VAR";static char _tmp2D1[13U]="TYPEDEF_NAME";static char _tmp2D2[16U]="QUAL_IDENTIFIER";static char _tmp2D3[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D4[10U]="ATTRIBUTE";static char _tmp2D5[4U]="ASM";static char _tmp2D6[4U]="';'";static char _tmp2D7[4U]="'{'";static char _tmp2D8[4U]="'}'";static char _tmp2D9[4U]="','";static char _tmp2DA[4U]="'='";static char _tmp2DB[4U]="'<'";static char _tmp2DC[4U]="'>'";static char _tmp2DD[4U]="'('";static char _tmp2DE[4U]="')'";static char _tmp2DF[4U]="'_'";static char _tmp2E0[4U]="'$'";static char _tmp2E1[4U]="':'";static char _tmp2E2[4U]="'.'";static char _tmp2E3[4U]="'['";static char _tmp2E4[4U]="']'";static char _tmp2E5[4U]="'*'";static char _tmp2E6[4U]="'@'";static char _tmp2E7[4U]="'?'";static char _tmp2E8[4U]="'+'";static char _tmp2E9[4U]="'|'";static char _tmp2EA[4U]="'^'";static char _tmp2EB[4U]="'&'";static char _tmp2EC[4U]="'-'";static char _tmp2ED[4U]="'/'";static char _tmp2EE[4U]="'%'";static char _tmp2EF[4U]="'~'";static char _tmp2F0[4U]="'!'";static char _tmp2F1[5U]="prog";static char _tmp2F2[17U]="translation_unit";static char _tmp2F3[18U]="tempest_on_action";static char _tmp2F4[19U]="tempest_off_action";static char _tmp2F5[16U]="extern_c_action";static char _tmp2F6[13U]="end_extern_c";static char _tmp2F7[12U]="export_list";static char _tmp2F8[19U]="export_list_values";static char _tmp2F9[21U]="external_declaration";static char _tmp2FA[15U]="optional_comma";static char _tmp2FB[20U]="function_definition";static char _tmp2FC[21U]="function_definition2";static char _tmp2FD[13U]="using_action";static char _tmp2FE[15U]="unusing_action";static char _tmp2FF[17U]="namespace_action";static char _tmp300[19U]="unnamespace_action";static char _tmp301[12U]="declaration";static char _tmp302[17U]="declaration_list";static char _tmp303[23U]="declaration_specifiers";static char _tmp304[24U]="storage_class_specifier";static char _tmp305[15U]="attributes_opt";static char _tmp306[11U]="attributes";static char _tmp307[15U]="attribute_list";static char _tmp308[10U]="attribute";static char _tmp309[15U]="type_specifier";static char _tmp30A[25U]="type_specifier_notypedef";static char _tmp30B[5U]="kind";static char _tmp30C[15U]="type_qualifier";static char _tmp30D[15U]="enum_specifier";static char _tmp30E[11U]="enum_field";static char _tmp30F[22U]="enum_declaration_list";static char _tmp310[26U]="struct_or_union_specifier";static char _tmp311[16U]="type_params_opt";static char _tmp312[16U]="struct_or_union";static char _tmp313[24U]="struct_declaration_list";static char _tmp314[25U]="struct_declaration_list0";static char _tmp315[21U]="init_declarator_list";static char _tmp316[22U]="init_declarator_list0";static char _tmp317[16U]="init_declarator";static char _tmp318[19U]="struct_declaration";static char _tmp319[25U]="specifier_qualifier_list";static char _tmp31A[35U]="notypedef_specifier_qualifier_list";static char _tmp31B[23U]="struct_declarator_list";static char _tmp31C[24U]="struct_declarator_list0";static char _tmp31D[18U]="struct_declarator";static char _tmp31E[20U]="requires_clause_opt";static char _tmp31F[19U]="ensures_clause_opt";static char _tmp320[19U]="datatype_specifier";static char _tmp321[14U]="qual_datatype";static char _tmp322[19U]="datatypefield_list";static char _tmp323[20U]="datatypefield_scope";static char _tmp324[14U]="datatypefield";static char _tmp325[11U]="declarator";static char _tmp326[23U]="declarator_withtypedef";static char _tmp327[18U]="direct_declarator";static char _tmp328[30U]="direct_declarator_withtypedef";static char _tmp329[8U]="pointer";static char _tmp32A[12U]="one_pointer";static char _tmp32B[14U]="pointer_quals";static char _tmp32C[13U]="pointer_qual";static char _tmp32D[23U]="pointer_null_and_bound";static char _tmp32E[14U]="pointer_bound";static char _tmp32F[18U]="zeroterm_qual_opt";static char _tmp330[8U]="rgn_opt";static char _tmp331[11U]="tqual_list";static char _tmp332[20U]="parameter_type_list";static char _tmp333[9U]="type_var";static char _tmp334[16U]="optional_effect";static char _tmp335[19U]="optional_rgn_order";static char _tmp336[10U]="rgn_order";static char _tmp337[16U]="optional_inject";static char _tmp338[11U]="effect_set";static char _tmp339[14U]="atomic_effect";static char _tmp33A[11U]="region_set";static char _tmp33B[15U]="parameter_list";static char _tmp33C[22U]="parameter_declaration";static char _tmp33D[16U]="identifier_list";static char _tmp33E[17U]="identifier_list0";static char _tmp33F[12U]="initializer";static char _tmp340[18U]="array_initializer";static char _tmp341[17U]="initializer_list";static char _tmp342[12U]="designation";static char _tmp343[16U]="designator_list";static char _tmp344[11U]="designator";static char _tmp345[10U]="type_name";static char _tmp346[14U]="any_type_name";static char _tmp347[15U]="type_name_list";static char _tmp348[20U]="abstract_declarator";static char _tmp349[27U]="direct_abstract_declarator";static char _tmp34A[10U]="statement";static char _tmp34B[18U]="labeled_statement";static char _tmp34C[21U]="expression_statement";static char _tmp34D[19U]="compound_statement";static char _tmp34E[16U]="block_item_list";static char _tmp34F[20U]="selection_statement";static char _tmp350[15U]="switch_clauses";static char _tmp351[20U]="iteration_statement";static char _tmp352[15U]="jump_statement";static char _tmp353[12U]="exp_pattern";static char _tmp354[20U]="conditional_pattern";static char _tmp355[19U]="logical_or_pattern";static char _tmp356[20U]="logical_and_pattern";static char _tmp357[21U]="inclusive_or_pattern";static char _tmp358[21U]="exclusive_or_pattern";static char _tmp359[12U]="and_pattern";static char _tmp35A[17U]="equality_pattern";static char _tmp35B[19U]="relational_pattern";static char _tmp35C[14U]="shift_pattern";static char _tmp35D[17U]="additive_pattern";static char _tmp35E[23U]="multiplicative_pattern";static char _tmp35F[13U]="cast_pattern";static char _tmp360[14U]="unary_pattern";static char _tmp361[16U]="postfix_pattern";static char _tmp362[16U]="primary_pattern";static char _tmp363[8U]="pattern";static char _tmp364[19U]="tuple_pattern_list";static char _tmp365[20U]="tuple_pattern_list0";static char _tmp366[14U]="field_pattern";static char _tmp367[19U]="field_pattern_list";static char _tmp368[20U]="field_pattern_list0";static char _tmp369[11U]="expression";static char _tmp36A[22U]="assignment_expression";static char _tmp36B[20U]="assignment_operator";static char _tmp36C[23U]="conditional_expression";static char _tmp36D[20U]="constant_expression";static char _tmp36E[22U]="logical_or_expression";static char _tmp36F[23U]="logical_and_expression";static char _tmp370[24U]="inclusive_or_expression";static char _tmp371[24U]="exclusive_or_expression";static char _tmp372[15U]="and_expression";static char _tmp373[20U]="equality_expression";static char _tmp374[22U]="relational_expression";static char _tmp375[17U]="shift_expression";static char _tmp376[20U]="additive_expression";static char _tmp377[26U]="multiplicative_expression";static char _tmp378[16U]="cast_expression";static char _tmp379[17U]="unary_expression";static char _tmp37A[15U]="unary_operator";static char _tmp37B[19U]="postfix_expression";static char _tmp37C[17U]="field_expression";static char _tmp37D[19U]="primary_expression";static char _tmp37E[25U]="argument_expression_list";static char _tmp37F[26U]="argument_expression_list0";static char _tmp380[9U]="constant";static char _tmp381[20U]="qual_opt_identifier";static char _tmp382[17U]="qual_opt_typedef";static char _tmp383[18U]="struct_union_name";static char _tmp384[11U]="field_name";static char _tmp385[12U]="right_angle";
# 1594 "parse.y"
static struct _dyneither_ptr Cyc_yytname[298U]={{_tmp25C,_tmp25C,_tmp25C + 2U},{_tmp25D,_tmp25D,_tmp25D + 6U},{_tmp25E,_tmp25E,_tmp25E + 12U},{_tmp25F,_tmp25F,_tmp25F + 5U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 7U},{_tmp262,_tmp262,_tmp262 + 7U},{_tmp263,_tmp263,_tmp263 + 8U},{_tmp264,_tmp264,_tmp264 + 5U},{_tmp265,_tmp265,_tmp265 + 5U},{_tmp266,_tmp266,_tmp266 + 6U},{_tmp267,_tmp267,_tmp267 + 4U},{_tmp268,_tmp268,_tmp268 + 5U},{_tmp269,_tmp269,_tmp269 + 6U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 7U},{_tmp26C,_tmp26C,_tmp26C + 9U},{_tmp26D,_tmp26D,_tmp26D + 6U},{_tmp26E,_tmp26E,_tmp26E + 9U},{_tmp26F,_tmp26F,_tmp26F + 9U},{_tmp270,_tmp270,_tmp270 + 7U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 5U},{_tmp273,_tmp273,_tmp273 + 8U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 3U},{_tmp278,_tmp278,_tmp278 + 5U},{_tmp279,_tmp279,_tmp279 + 7U},{_tmp27A,_tmp27A,_tmp27A + 6U},{_tmp27B,_tmp27B,_tmp27B + 3U},{_tmp27C,_tmp27C,_tmp27C + 4U},{_tmp27D,_tmp27D,_tmp27D + 5U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 6U},{_tmp280,_tmp280,_tmp280 + 7U},{_tmp281,_tmp281,_tmp281 + 5U},{_tmp282,_tmp282,_tmp282 + 7U},{_tmp283,_tmp283,_tmp283 + 16U},{_tmp284,_tmp284,_tmp284 + 10U},{_tmp285,_tmp285,_tmp285 + 8U},{_tmp286,_tmp286,_tmp286 + 4U},{_tmp287,_tmp287,_tmp287 + 6U},{_tmp288,_tmp288,_tmp288 + 4U},{_tmp289,_tmp289,_tmp289 + 6U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 4U},{_tmp28C,_tmp28C,_tmp28C + 9U},{_tmp28D,_tmp28D,_tmp28D + 9U},{_tmp28E,_tmp28E,_tmp28E + 6U},{_tmp28F,_tmp28F,_tmp28F + 10U},{_tmp290,_tmp290,_tmp290 + 9U},{_tmp291,_tmp291,_tmp291 + 7U},{_tmp292,_tmp292,_tmp292 + 8U},{_tmp293,_tmp293,_tmp293 + 15U},{_tmp294,_tmp294,_tmp294 + 7U},{_tmp295,_tmp295,_tmp295 + 8U},{_tmp296,_tmp296,_tmp296 + 5U},{_tmp297,_tmp297,_tmp297 + 9U},{_tmp298,_tmp298,_tmp298 + 6U},{_tmp299,_tmp299,_tmp299 + 7U},{_tmp29A,_tmp29A,_tmp29A + 5U},{_tmp29B,_tmp29B,_tmp29B + 8U},{_tmp29C,_tmp29C,_tmp29C + 7U},{_tmp29D,_tmp29D,_tmp29D + 8U},{_tmp29E,_tmp29E,_tmp29E + 7U},{_tmp29F,_tmp29F,_tmp29F + 10U},{_tmp2A0,_tmp2A0,_tmp2A0 + 11U},{_tmp2A1,_tmp2A1,_tmp2A1 + 8U},{_tmp2A2,_tmp2A2,_tmp2A2 + 8U},{_tmp2A3,_tmp2A3,_tmp2A3 + 10U},{_tmp2A4,_tmp2A4,_tmp2A4 + 9U},{_tmp2A5,_tmp2A5,_tmp2A5 + 13U},{_tmp2A6,_tmp2A6,_tmp2A6 + 10U},{_tmp2A7,_tmp2A7,_tmp2A7 + 9U},{_tmp2A8,_tmp2A8,_tmp2A8 + 13U},{_tmp2A9,_tmp2A9,_tmp2A9 + 14U},{_tmp2AA,_tmp2AA,_tmp2AA + 14U},{_tmp2AB,_tmp2AB,_tmp2AB + 13U},{_tmp2AC,_tmp2AC,_tmp2AC + 12U},{_tmp2AD,_tmp2AD,_tmp2AD + 16U},{_tmp2AE,_tmp2AE,_tmp2AE + 14U},{_tmp2AF,_tmp2AF,_tmp2AF + 12U},{_tmp2B0,_tmp2B0,_tmp2B0 + 16U},{_tmp2B1,_tmp2B1,_tmp2B1 + 7U},{_tmp2B2,_tmp2B2,_tmp2B2 + 7U},{_tmp2B3,_tmp2B3,_tmp2B3 + 7U},{_tmp2B4,_tmp2B4,_tmp2B4 + 8U},{_tmp2B5,_tmp2B5,_tmp2B5 + 9U},{_tmp2B6,_tmp2B6,_tmp2B6 + 6U},{_tmp2B7,_tmp2B7,_tmp2B7 + 6U},{_tmp2B8,_tmp2B8,_tmp2B8 + 6U},{_tmp2B9,_tmp2B9,_tmp2B9 + 6U},{_tmp2BA,_tmp2BA,_tmp2BA + 7U},{_tmp2BB,_tmp2BB,_tmp2BB + 6U},{_tmp2BC,_tmp2BC,_tmp2BC + 11U},{_tmp2BD,_tmp2BD,_tmp2BD + 11U},{_tmp2BE,_tmp2BE,_tmp2BE + 11U},{_tmp2BF,_tmp2BF,_tmp2BF + 11U},{_tmp2C0,_tmp2C0,_tmp2C0 + 11U},{_tmp2C1,_tmp2C1,_tmp2C1 + 12U},{_tmp2C2,_tmp2C2,_tmp2C2 + 13U},{_tmp2C3,_tmp2C3,_tmp2C3 + 11U},{_tmp2C4,_tmp2C4,_tmp2C4 + 11U},{_tmp2C5,_tmp2C5,_tmp2C5 + 10U},{_tmp2C6,_tmp2C6,_tmp2C6 + 9U},{_tmp2C7,_tmp2C7,_tmp2C7 + 11U},{_tmp2C8,_tmp2C8,_tmp2C8 + 12U},{_tmp2C9,_tmp2C9,_tmp2C9 + 11U},{_tmp2CA,_tmp2CA,_tmp2CA + 17U},{_tmp2CB,_tmp2CB,_tmp2CB + 7U},{_tmp2CC,_tmp2CC,_tmp2CC + 8U},{_tmp2CD,_tmp2CD,_tmp2CD + 19U},{_tmp2CE,_tmp2CE,_tmp2CE + 20U},{_tmp2CF,_tmp2CF,_tmp2CF + 18U},{_tmp2D0,_tmp2D0,_tmp2D0 + 9U},{_tmp2D1,_tmp2D1,_tmp2D1 + 13U},{_tmp2D2,_tmp2D2,_tmp2D2 + 16U},{_tmp2D3,_tmp2D3,_tmp2D3 + 18U},{_tmp2D4,_tmp2D4,_tmp2D4 + 10U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 5U},{_tmp2F2,_tmp2F2,_tmp2F2 + 17U},{_tmp2F3,_tmp2F3,_tmp2F3 + 18U},{_tmp2F4,_tmp2F4,_tmp2F4 + 19U},{_tmp2F5,_tmp2F5,_tmp2F5 + 16U},{_tmp2F6,_tmp2F6,_tmp2F6 + 13U},{_tmp2F7,_tmp2F7,_tmp2F7 + 12U},{_tmp2F8,_tmp2F8,_tmp2F8 + 19U},{_tmp2F9,_tmp2F9,_tmp2F9 + 21U},{_tmp2FA,_tmp2FA,_tmp2FA + 15U},{_tmp2FB,_tmp2FB,_tmp2FB + 20U},{_tmp2FC,_tmp2FC,_tmp2FC + 21U},{_tmp2FD,_tmp2FD,_tmp2FD + 13U},{_tmp2FE,_tmp2FE,_tmp2FE + 15U},{_tmp2FF,_tmp2FF,_tmp2FF + 17U},{_tmp300,_tmp300,_tmp300 + 19U},{_tmp301,_tmp301,_tmp301 + 12U},{_tmp302,_tmp302,_tmp302 + 17U},{_tmp303,_tmp303,_tmp303 + 23U},{_tmp304,_tmp304,_tmp304 + 24U},{_tmp305,_tmp305,_tmp305 + 15U},{_tmp306,_tmp306,_tmp306 + 11U},{_tmp307,_tmp307,_tmp307 + 15U},{_tmp308,_tmp308,_tmp308 + 10U},{_tmp309,_tmp309,_tmp309 + 15U},{_tmp30A,_tmp30A,_tmp30A + 25U},{_tmp30B,_tmp30B,_tmp30B + 5U},{_tmp30C,_tmp30C,_tmp30C + 15U},{_tmp30D,_tmp30D,_tmp30D + 15U},{_tmp30E,_tmp30E,_tmp30E + 11U},{_tmp30F,_tmp30F,_tmp30F + 22U},{_tmp310,_tmp310,_tmp310 + 26U},{_tmp311,_tmp311,_tmp311 + 16U},{_tmp312,_tmp312,_tmp312 + 16U},{_tmp313,_tmp313,_tmp313 + 24U},{_tmp314,_tmp314,_tmp314 + 25U},{_tmp315,_tmp315,_tmp315 + 21U},{_tmp316,_tmp316,_tmp316 + 22U},{_tmp317,_tmp317,_tmp317 + 16U},{_tmp318,_tmp318,_tmp318 + 19U},{_tmp319,_tmp319,_tmp319 + 25U},{_tmp31A,_tmp31A,_tmp31A + 35U},{_tmp31B,_tmp31B,_tmp31B + 23U},{_tmp31C,_tmp31C,_tmp31C + 24U},{_tmp31D,_tmp31D,_tmp31D + 18U},{_tmp31E,_tmp31E,_tmp31E + 20U},{_tmp31F,_tmp31F,_tmp31F + 19U},{_tmp320,_tmp320,_tmp320 + 19U},{_tmp321,_tmp321,_tmp321 + 14U},{_tmp322,_tmp322,_tmp322 + 19U},{_tmp323,_tmp323,_tmp323 + 20U},{_tmp324,_tmp324,_tmp324 + 14U},{_tmp325,_tmp325,_tmp325 + 11U},{_tmp326,_tmp326,_tmp326 + 23U},{_tmp327,_tmp327,_tmp327 + 18U},{_tmp328,_tmp328,_tmp328 + 30U},{_tmp329,_tmp329,_tmp329 + 8U},{_tmp32A,_tmp32A,_tmp32A + 12U},{_tmp32B,_tmp32B,_tmp32B + 14U},{_tmp32C,_tmp32C,_tmp32C + 13U},{_tmp32D,_tmp32D,_tmp32D + 23U},{_tmp32E,_tmp32E,_tmp32E + 14U},{_tmp32F,_tmp32F,_tmp32F + 18U},{_tmp330,_tmp330,_tmp330 + 8U},{_tmp331,_tmp331,_tmp331 + 11U},{_tmp332,_tmp332,_tmp332 + 20U},{_tmp333,_tmp333,_tmp333 + 9U},{_tmp334,_tmp334,_tmp334 + 16U},{_tmp335,_tmp335,_tmp335 + 19U},{_tmp336,_tmp336,_tmp336 + 10U},{_tmp337,_tmp337,_tmp337 + 16U},{_tmp338,_tmp338,_tmp338 + 11U},{_tmp339,_tmp339,_tmp339 + 14U},{_tmp33A,_tmp33A,_tmp33A + 11U},{_tmp33B,_tmp33B,_tmp33B + 15U},{_tmp33C,_tmp33C,_tmp33C + 22U},{_tmp33D,_tmp33D,_tmp33D + 16U},{_tmp33E,_tmp33E,_tmp33E + 17U},{_tmp33F,_tmp33F,_tmp33F + 12U},{_tmp340,_tmp340,_tmp340 + 18U},{_tmp341,_tmp341,_tmp341 + 17U},{_tmp342,_tmp342,_tmp342 + 12U},{_tmp343,_tmp343,_tmp343 + 16U},{_tmp344,_tmp344,_tmp344 + 11U},{_tmp345,_tmp345,_tmp345 + 10U},{_tmp346,_tmp346,_tmp346 + 14U},{_tmp347,_tmp347,_tmp347 + 15U},{_tmp348,_tmp348,_tmp348 + 20U},{_tmp349,_tmp349,_tmp349 + 27U},{_tmp34A,_tmp34A,_tmp34A + 10U},{_tmp34B,_tmp34B,_tmp34B + 18U},{_tmp34C,_tmp34C,_tmp34C + 21U},{_tmp34D,_tmp34D,_tmp34D + 19U},{_tmp34E,_tmp34E,_tmp34E + 16U},{_tmp34F,_tmp34F,_tmp34F + 20U},{_tmp350,_tmp350,_tmp350 + 15U},{_tmp351,_tmp351,_tmp351 + 20U},{_tmp352,_tmp352,_tmp352 + 15U},{_tmp353,_tmp353,_tmp353 + 12U},{_tmp354,_tmp354,_tmp354 + 20U},{_tmp355,_tmp355,_tmp355 + 19U},{_tmp356,_tmp356,_tmp356 + 20U},{_tmp357,_tmp357,_tmp357 + 21U},{_tmp358,_tmp358,_tmp358 + 21U},{_tmp359,_tmp359,_tmp359 + 12U},{_tmp35A,_tmp35A,_tmp35A + 17U},{_tmp35B,_tmp35B,_tmp35B + 19U},{_tmp35C,_tmp35C,_tmp35C + 14U},{_tmp35D,_tmp35D,_tmp35D + 17U},{_tmp35E,_tmp35E,_tmp35E + 23U},{_tmp35F,_tmp35F,_tmp35F + 13U},{_tmp360,_tmp360,_tmp360 + 14U},{_tmp361,_tmp361,_tmp361 + 16U},{_tmp362,_tmp362,_tmp362 + 16U},{_tmp363,_tmp363,_tmp363 + 8U},{_tmp364,_tmp364,_tmp364 + 19U},{_tmp365,_tmp365,_tmp365 + 20U},{_tmp366,_tmp366,_tmp366 + 14U},{_tmp367,_tmp367,_tmp367 + 19U},{_tmp368,_tmp368,_tmp368 + 20U},{_tmp369,_tmp369,_tmp369 + 11U},{_tmp36A,_tmp36A,_tmp36A + 22U},{_tmp36B,_tmp36B,_tmp36B + 20U},{_tmp36C,_tmp36C,_tmp36C + 23U},{_tmp36D,_tmp36D,_tmp36D + 20U},{_tmp36E,_tmp36E,_tmp36E + 22U},{_tmp36F,_tmp36F,_tmp36F + 23U},{_tmp370,_tmp370,_tmp370 + 24U},{_tmp371,_tmp371,_tmp371 + 24U},{_tmp372,_tmp372,_tmp372 + 15U},{_tmp373,_tmp373,_tmp373 + 20U},{_tmp374,_tmp374,_tmp374 + 22U},{_tmp375,_tmp375,_tmp375 + 17U},{_tmp376,_tmp376,_tmp376 + 20U},{_tmp377,_tmp377,_tmp377 + 26U},{_tmp378,_tmp378,_tmp378 + 16U},{_tmp379,_tmp379,_tmp379 + 17U},{_tmp37A,_tmp37A,_tmp37A + 15U},{_tmp37B,_tmp37B,_tmp37B + 19U},{_tmp37C,_tmp37C,_tmp37C + 17U},{_tmp37D,_tmp37D,_tmp37D + 19U},{_tmp37E,_tmp37E,_tmp37E + 25U},{_tmp37F,_tmp37F,_tmp37F + 26U},{_tmp380,_tmp380,_tmp380 + 9U},{_tmp381,_tmp381,_tmp381 + 20U},{_tmp382,_tmp382,_tmp382 + 17U},{_tmp383,_tmp383,_tmp383 + 18U},{_tmp384,_tmp384,_tmp384 + 11U},{_tmp385,_tmp385,_tmp385 + 12U}};
# 1650
static short Cyc_yyr1[533U]={0,149,150,150,150,150,150,150,150,150,150,150,150,150,151,152,153,154,155,155,156,156,156,157,157,157,158,158,159,159,159,159,160,160,161,162,163,164,165,165,165,165,165,165,165,166,166,167,167,167,167,167,167,167,167,167,167,167,168,168,168,168,168,168,168,169,169,170,171,171,172,172,172,172,173,173,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,174,175,176,176,176,177,177,177,178,178,179,179,179,180,180,180,180,180,181,181,182,182,183,183,184,184,185,186,186,187,187,188,189,189,189,189,189,189,190,190,190,190,190,190,191,192,192,193,193,193,193,194,194,195,195,196,196,196,197,197,198,198,198,198,199,199,199,200,200,201,201,202,202,203,203,203,203,203,203,203,203,203,203,204,204,204,204,204,204,204,204,204,204,204,205,205,206,207,207,208,208,208,208,208,208,208,208,209,209,209,210,210,211,211,211,212,212,212,213,213,214,214,214,214,215,215,216,216,217,217,218,218,219,219,220,220,221,221,221,221,222,222,223,223,224,224,224,225,226,226,227,227,228,228,228,228,228,229,229,229,229,230,230,231,231,232,232,233,233,234,234,234,234,234,235,235,236,236,236,237,237,237,237,237,237,237,237,237,237,237,238,238,238,238,238,238,239,240,240,241,241,242,242,242,242,242,242,242,242,243,243,243,243,243,243,244,244,244,244,244,244,245,245,245,245,245,245,245,245,245,245,245,245,245,245,246,246,246,246,246,246,246,246,247,248,248,249,249,250,250,251,251,252,252,253,253,254,254,254,255,255,255,255,255,256,256,256,257,257,257,258,258,258,258,259,259,260,260,260,260,260,260,261,262,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,263,264,264,264,265,265,266,266,267,267,267,268,268,269,269,270,270,270,271,271,271,271,271,271,271,271,271,271,271,272,272,272,272,272,272,272,273,274,274,275,275,276,276,277,277,278,278,279,279,279,280,280,280,280,280,281,281,281,282,282,282,283,283,283,283,284,284,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,285,286,286,286,287,287,287,287,287,287,287,287,287,287,287,288,288,288,288,289,289,289,289,289,289,289,289,289,289,289,290,291,291,292,292,292,292,292,293,293,294,294,295,295,296,296,297,297};
# 1707
static short Cyc_yyr2[533U]={0,1,2,3,5,3,5,5,6,3,3,3,3,0,1,1,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,2,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1764
static short Cyc_yydefact[1087U]={0,25,58,59,60,61,63,76,77,78,79,80,81,82,83,84,100,101,102,118,119,54,0,0,88,0,0,64,0,0,156,95,97,0,0,0,14,15,0,0,0,523,222,525,524,526,0,0,91,0,208,208,207,1,0,0,0,0,23,0,0,24,0,47,56,50,74,52,85,86,0,89,0,0,167,0,192,195,90,171,116,62,61,55,0,104,0,49,522,0,523,518,519,520,521,0,116,0,382,0,0,0,0,245,0,384,385,34,36,0,0,0,0,0,0,0,0,157,0,0,0,0,0,0,0,205,206,0,0,0,2,0,0,0,0,38,0,124,125,127,48,57,51,53,120,527,528,116,116,0,45,0,0,27,0,224,0,180,168,193,0,199,200,203,204,0,202,201,213,195,0,75,62,108,0,106,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,507,508,484,0,0,0,0,0,488,486,487,0,411,413,427,435,437,439,441,443,445,448,453,456,459,463,0,465,489,506,504,523,394,0,0,0,0,0,0,395,393,41,0,0,116,0,0,0,134,130,132,265,267,0,0,43,0,0,9,10,0,116,529,530,223,99,0,0,172,92,243,0,240,0,11,12,0,3,0,5,0,39,0,0,0,27,0,121,122,147,115,0,154,0,0,0,0,0,0,0,0,0,0,0,0,523,295,297,0,305,299,0,303,288,289,290,0,291,292,293,0,46,27,127,26,28,272,0,230,246,0,0,226,224,0,210,0,0,0,215,65,214,196,0,109,105,0,0,0,473,0,0,485,429,463,0,430,431,0,0,0,0,0,0,0,0,0,0,466,467,0,0,0,0,469,470,468,0,87,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,417,418,419,420,421,422,423,424,425,426,416,0,471,0,495,496,0,0,0,510,0,116,386,0,0,0,408,523,530,0,0,0,0,261,404,409,0,406,0,383,401,402,0,399,0,247,0,0,0,0,268,0,238,135,140,136,138,131,133,224,0,274,266,275,532,531,0,94,96,0,0,98,114,71,70,0,68,173,224,242,169,274,244,181,182,0,93,209,17,0,35,0,37,0,126,128,249,248,27,29,111,123,0,0,0,142,143,150,0,116,116,162,0,0,0,0,0,523,0,0,0,334,335,336,0,0,338,0,0,0,306,300,127,304,298,296,30,0,179,231,0,0,0,237,225,232,150,0,0,0,226,178,212,211,174,210,0,0,216,66,117,110,434,107,103,0,0,0,0,523,250,255,0,0,0,0,0,0,0,0,0,0,0,0,0,0,509,516,0,515,412,436,0,438,440,442,444,446,447,451,452,449,450,454,455,457,458,460,461,462,415,414,494,491,0,493,0,0,0,397,398,0,264,0,405,259,262,392,0,260,389,0,396,40,0,390,0,269,0,141,137,139,0,226,0,210,0,276,0,224,0,287,271,0,0,116,0,0,0,134,0,116,0,224,0,191,170,241,0,7,0,4,6,31,0,146,129,147,0,0,145,226,155,164,163,0,0,158,0,0,0,313,0,0,0,0,0,0,333,337,0,0,0,301,294,0,32,273,224,0,234,0,0,152,227,0,150,230,218,175,197,198,216,194,472,0,0,0,251,0,256,475,0,0,0,0,0,505,480,483,0,0,0,0,464,512,0,0,492,490,0,0,0,0,263,407,410,400,403,391,270,239,150,0,277,278,210,0,0,226,210,0,0,42,226,523,0,67,69,0,183,0,0,226,0,210,0,0,8,144,0,148,120,153,165,162,162,0,0,0,0,0,0,0,0,0,0,0,0,0,313,339,0,302,33,226,0,235,233,0,176,0,152,226,0,217,501,0,500,0,252,257,0,0,0,0,0,432,433,494,493,514,497,0,517,428,511,513,0,387,388,152,150,280,286,150,0,279,210,0,120,0,72,184,190,150,0,189,185,210,19,0,20,0,0,0,161,160,307,313,0,0,0,0,0,0,341,342,344,346,348,350,352,354,357,362,365,368,372,374,380,381,0,0,310,319,0,0,0,0,0,0,0,0,0,0,340,220,236,0,228,177,219,224,474,0,0,258,476,477,0,0,482,481,498,0,283,152,152,150,281,44,0,0,152,150,186,18,21,0,149,113,0,0,0,313,0,378,0,0,375,313,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,376,313,0,321,0,0,0,329,0,0,0,0,0,0,312,0,0,226,503,502,0,0,0,499,282,285,152,112,0,187,152,22,166,308,309,0,0,0,0,313,315,345,0,347,349,351,353,355,356,360,361,358,359,363,364,366,367,369,370,371,0,314,320,322,323,0,331,330,0,325,0,0,0,151,229,221,0,0,0,0,284,0,188,311,377,0,373,316,0,313,324,332,326,327,0,254,253,478,0,73,0,343,313,317,328,0,379,318,479,0,0,0};
# 1876
static short Cyc_yydefgoto[149U]={1084,53,54,55,56,482,663,864,57,319,58,303,59,484,60,486,61,151,62,63,551,242,468,469,243,66,258,244,68,173,174,69,171,70,280,281,136,137,138,282,245,450,497,498,499,673,813,71,72,678,679,680,73,500,74,473,75,76,168,169,77,125,547,334,716,635,78,636,541,707,533,537,538,444,327,266,102,103,564,489,565,424,425,426,246,320,321,637,456,306,307,308,309,310,311,795,312,313,878,879,880,881,882,883,884,885,886,887,888,889,890,891,892,893,427,435,436,428,429,430,314,206,404,207,556,208,209,210,211,212,213,214,215,216,217,218,219,220,221,820,222,581,582,223,224,80,865,431,460};
# 1894
static short Cyc_yypact[1087U]={2925,- -32768,- -32768,- -32768,- -32768,- 58,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3766,306,- 60,- -32768,3766,884,- -32768,68,- 28,- -32768,27,54,- 33,- 12,99,- -32768,- -32768,94,357,203,- -32768,241,- -32768,- -32768,- -32768,170,302,246,271,290,290,- -32768,- -32768,284,294,300,2785,- -32768,451,597,- -32768,365,3766,3766,3766,- -32768,3766,- -32768,- -32768,832,- -32768,68,3679,259,191,296,962,- -32768,- -32768,325,385,380,- -32768,68,391,6731,- -32768,- -32768,3530,249,- -32768,- -32768,- -32768,- -32768,406,325,6731,- -32768,413,426,3530,430,481,486,- -32768,179,- -32768,- -32768,3975,3975,83,512,2785,2785,6731,371,- -32768,219,505,647,508,219,4439,6731,- -32768,- -32768,2785,2785,3064,- -32768,2785,3064,2785,3064,- -32768,526,528,- -32768,3549,- -32768,- -32768,- -32768,- -32768,4439,- -32768,- -32768,325,154,1922,- -32768,3679,365,558,3975,3850,4101,- -32768,259,- -32768,536,- -32768,- -32768,- -32768,- -32768,553,- -32768,- -32768,71,962,3975,- -32768,- -32768,559,613,577,68,7028,582,7126,6731,1168,621,632,643,645,649,651,689,712,718,728,7126,7126,- -32768,- -32768,- -32768,2355,730,7224,7224,7224,- -32768,- -32768,- -32768,111,- -32768,- -32768,- 37,650,720,726,732,676,106,710,314,291,- -32768,589,7224,232,- 56,- -32768,755,- 1,- -32768,3530,101,773,3375,202,1019,762,- -32768,- -32768,793,6731,325,1019,768,4187,4439,4523,4439,658,- -32768,61,61,- -32768,801,772,- -32768,- -32768,218,325,- -32768,- -32768,- -32768,- -32768,29,776,- -32768,- -32768,713,274,- -32768,794,- -32768,- -32768,795,- -32768,800,- -32768,802,- -32768,647,5152,3679,558,803,4439,- -32768,891,798,68,805,811,278,814,4591,817,808,807,809,5261,1485,4591,138,815,- -32768,- -32768,818,2068,2068,365,2068,- -32768,- -32768,- -32768,828,- -32768,- -32768,- -32768,510,- -32768,558,831,- -32768,- -32768,820,32,849,- -32768,72,834,840,633,846,701,825,6731,3975,- -32768,857,- -32768,- -32768,32,68,- -32768,6731,855,2355,- -32768,4439,2355,- -32768,- -32768,- -32768,1583,- -32768,885,6731,6731,6731,6731,6731,6731,872,6731,4439,824,- -32768,- -32768,2068,852,381,6731,- -32768,- -32768,- -32768,6731,- -32768,7224,6731,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,6731,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6731,- -32768,219,- -32768,- -32768,5359,219,6731,- -32768,858,325,- -32768,861,867,868,- -32768,89,406,219,6731,3530,329,- -32768,- -32768,- -32768,862,878,871,- -32768,- -32768,- -32768,876,889,3530,- -32768,635,3375,887,3975,- -32768,895,897,- -32768,4523,4523,4523,- -32768,- -32768,3280,5457,196,- -32768,350,- -32768,- -32768,72,- -32768,- -32768,894,917,- -32768,909,- -32768,911,918,922,- -32768,3148,- -32768,523,431,- -32768,- -32768,- -32768,4439,- -32768,- -32768,- -32768,2645,- -32768,2785,- -32768,2785,- -32768,- -32768,- -32768,- -32768,558,- -32768,- -32768,- -32768,774,6731,929,930,- -32768,- 24,683,325,325,799,6731,6731,925,933,6731,924,1028,1324,937,- -32768,- -32768,- -32768,637,1021,- -32768,5555,2214,2496,- -32768,- -32768,3549,- -32768,- -32768,- -32768,- -32768,3975,- -32768,- -32768,4439,940,4271,- -32768,- -32768,932,996,72,945,4355,840,- -32768,- -32768,- -32768,- -32768,701,947,40,810,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,956,963,960,982,959,- -32768,- -32768,715,5152,964,968,971,974,980,452,978,986,987,283,994,6927,- -32768,- -32768,990,985,- -32768,650,248,720,726,732,676,106,106,710,710,710,710,314,314,291,291,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,992,- -32768,28,3975,5039,4439,- -32768,4439,- -32768,988,- -32768,- -32768,- -32768,- -32768,3814,- -32768,- -32768,3261,- -32768,- -32768,999,- -32768,118,- -32768,4439,- -32768,- -32768,- -32768,997,840,1000,701,1002,350,3975,4062,5653,- -32768,- -32768,6731,1004,325,6829,1011,29,3412,1014,325,3975,3850,5751,- -32768,523,- -32768,1022,- -32768,2785,- -32768,- -32768,- -32768,944,- -32768,- -32768,891,1017,6731,- -32768,840,- -32768,- -32768,- -32768,1023,68,641,477,484,6731,823,524,4591,1020,5849,5947,666,- -32768,- -32768,1031,1033,1029,2068,- -32768,3679,- -32768,820,1036,3975,- -32768,1037,72,1084,- -32768,1055,996,120,- -32768,- -32768,- -32768,- -32768,810,- -32768,1042,175,1042,1057,- -32768,4700,- -32768,- -32768,6731,6731,1160,6731,1168,- -32768,- -32768,- -32768,219,219,1056,4813,- -32768,- -32768,6731,6731,- -32768,- -32768,32,739,1078,1079,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,996,1059,- -32768,- -32768,701,32,1060,840,701,1061,530,- -32768,840,1066,1062,- -32768,- -32768,1065,- -32768,32,1068,840,1069,701,1064,462,- -32768,- -32768,6731,- -32768,4439,- -32768,1067,100,799,4591,1080,1072,3112,1071,1081,4591,6731,6045,688,6143,692,6241,823,- -32768,1085,- -32768,- -32768,840,133,- -32768,- -32768,1077,- -32768,1094,1084,840,4439,- -32768,- -32768,153,- -32768,6731,- -32768,- -32768,5152,1082,1083,1087,1086,- -32768,885,1089,1090,- -32768,- -32768,742,- -32768,- -32768,- -32768,- -32768,5039,- -32768,- -32768,1084,996,- -32768,- -32768,996,1096,- -32768,701,1095,4439,1117,- -32768,- -32768,- -32768,996,1098,- -32768,- -32768,701,- -32768,1105,694,1100,1107,4439,- -32768,- -32768,1204,823,1110,7322,1106,2496,7224,1103,- -32768,- 17,- -32768,1145,1101,731,788,224,797,328,330,- -32768,- -32768,- -32768,- -32768,1147,7224,2068,- -32768,- -32768,532,4591,542,6339,4591,543,6437,6535,698,1119,- -32768,- -32768,- -32768,6731,1120,- -32768,- -32768,1036,- -32768,420,276,- -32768,- -32768,- -32768,4439,1219,- -32768,- -32768,- -32768,4926,- -32768,1084,1084,996,- -32768,- -32768,1123,1124,1084,996,- -32768,- -32768,- -32768,371,- -32768,- -32768,551,4591,1126,823,2355,- -32768,4439,1118,- -32768,1776,7224,6731,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,7224,6731,- -32768,823,1129,- -32768,4591,4591,579,- -32768,4591,4591,587,4591,609,6633,- -32768,1122,72,840,- -32768,- -32768,2496,1128,1127,- -32768,- -32768,- -32768,1084,- -32768,1143,- -32768,1084,- -32768,- -32768,- -32768,- -32768,1135,1130,1137,7224,823,- -32768,650,368,720,726,726,676,106,106,710,710,710,710,314,314,291,291,- -32768,- -32768,- -32768,372,- -32768,- -32768,- -32768,- -32768,4591,- -32768,- -32768,4591,- -32768,4591,4591,615,- -32768,- -32768,- -32768,1139,763,1136,4439,- -32768,1141,- -32768,- -32768,1042,175,- -32768,- -32768,6731,1776,- -32768,- -32768,- -32768,- -32768,4591,- -32768,- -32768,- -32768,1142,- -32768,455,- -32768,823,- -32768,- -32768,1144,- -32768,- -32768,- -32768,1265,1267,- -32768};
# 2006
static short Cyc_yypgoto[149U]={- -32768,143,- -32768,- -32768,- -32768,- -32768,- -32768,327,- -32768,- 239,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 59,- 130,20,- -32768,- -32768,0,624,- -32768,34,- 183,1154,79,- -32768,- -32768,- 134,- -32768,142,1245,- 741,- -32768,- -32768,- -32768,1009,1007,91,387,- -32768,- -32768,620,- 94,- 505,- -32768,- -32768,126,- -32768,- -32768,239,- 202,1217,- 386,253,- -32768,1125,- -32768,- -32768,1242,- 370,- -32768,580,- 25,- 148,- 143,93,305,588,596,- 416,- 486,- 119,- 472,- 122,- -32768,- 253,- 176,- 545,- 317,- -32768,874,- 170,- 92,- 132,- 108,- 379,66,- -32768,- -32768,43,- 275,- -32768,- 717,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,22,1063,- -32768,682,864,- -32768,165,748,- -32768,- 141,- 393,- 159,- 365,- 356,- 363,936,- 355,- 378,- 264,- 375,- 372,- 164,791,513,946,250,- -32768,- 341,- -32768,114,594,- 44,- 20,- 115,- 196};
# 2028
static short Cyc_yytable[7471U]={64,590,591,259,265,350,660,259,584,278,596,597,326,587,150,598,599,247,248,586,335,64,351,589,488,64,146,365,523,524,616,526,566,328,65,368,369,370,337,347,492,83,341,645,867,87,466,120,104,704,147,412,461,81,671,65,405,64,373,65,448,701,472,64,64,64,744,64,607,86,660,445,146,64,157,640,111,529,955,67,150,108,413,140,141,142,908,143,659,577,304,65,315,152,112,640,255,65,65,65,67,65,374,668,67,676,677,65,227,672,113,226,935,64,64,659,153,592,593,594,595,457,956,234,708,531,228,64,64,64,325,64,64,64,64,534,67,455,467,64,105,553,67,67,67,754,67,65,65,64,457,64,67,371,109,947,475,530,157,152,458,65,65,65,742,65,65,65,65,305,714,152,558,65,559,560,536,41,712,695,459,110,279,65,543,65,44,42,42,458,575,836,67,67,316,535,381,382,227,555,130,459,333,105,554,249,67,67,67,250,67,67,67,67,264,105,228,416,67,315,477,114,- 529,115,- 159,600,601,602,67,532,67,1010,417,383,384,283,371,1015,230,477,550,372,324,447,304,304,264,304,753,415,- 224,205,666,- 224,434,117,252,253,459,1036,519,434,231,911,448,448,448,520,758,653,268,269,270,459,271,272,273,274,64,784,254,170,555,917,256,819,121,918,285,284,286,605,257,653,566,608,64,1062,152,119,41,139,238,64,64,304,64,615,239,44,914,536,65,723,963,964,87,406,407,408,47,491,449,305,305,452,305,371,256,159,65,453,432,446,552,451,257,65,65,929,65,105,792,371,105,916,105,1078,464,118,629,965,966,105,122,555,511,67,227,261,1082,409,366,518,64,445,410,411,733,407,408,- 246,283,371,- 246,67,228,19,20,46,440,740,67,67,305,67,154,41,155,846,866,317,536,850,156,698,44,465,65,478,123,371,439,265,479,825,127,506,861,995,507,41,409,124,737,41,128,734,411,566,44,806,46,129,44,999,1000,654,389,84,477,47,1004,50,51,52,390,391,700,50,51,52,745,67,746,706,617,447,447,447,772,477,170,689,387,618,644,477,388,625,517,445,696,422,423,772,150,971,969,824,46,652,970,658,41,972,973,641,743,642,41,933,64,44,64,643,64,135,43,44,45,172,939,371,47,667,1055,371,454,762,1057,1063,50,51,52,1064,371,366,- 16,760,366,579,64,776,176,317,65,474,65,990,65,64,572,774,825,64,449,449,449,256,993,555,152,229,777,233,501,257,105,585,41,152,232,264,525,152,65,839,43,44,45,105,235,830,105,65,611,536,808,65,471,67,264,67,847,67,453,816,699,264,831,41,920,131,132,708,609,371,857,43,44,45,729,1022,1023,1081,863,697,824,918,1016,67,1028,1029,79,1019,1020,1030,1031,838,67,1018,371,821,67,1021,236,790,654,371,809,825,237,264,791,815,85,761,832,833,106,976,107,477,264,662,477,664,251,665,715,775,528,264,260,371,711,304,262,315,644,79,555,46,674,675,751,392,275,371,655,79,656,276,796,371,79,371,657,658,852,844,977,64,145,331,148,371,371,79,681,682,979,983,685,824,478,690,175,1014,318,1007,332,106,338,393,394,395,396,397,398,399,400,401,402,106,64,65,64,1024,1025,1026,1027,340,371,454,952,79,79,1040,145,344,371,953,79,403,305,1043,152,133,134,79,79,79,474,79,79,79,79,756,65,975,65,264,371,105,146,339,105,1045,371,807,67,446,375,1069,79,264,501,945,352,930,697,996,931,324,41,626,542,692,371,353,371,788,937,44,789,785,379,380,175,555,354,992,355,67,47,67,356,1011,357,1012,545,546,50,51,52,452,802,1077,767,371,41,453,715,50,51,52,385,386,43,44,45,994,676,677,781,1032,1033,1034,902,765,495,371,905,894,941,371,358,942,988,106,41,371,106,1051,106,16,17,18,43,44,45,106,632,633,634,304,1001,721,722,359,471,536,1005,793,794,360,453,1061,50,51,52,799,801,849,871,361,476,367,853,376,898,840,841,88,927,928,377,859,79,437,267,959,960,378,283,476,414,503,961,962,508,41,1073,967,968,1071,371,418,188,43,44,45,46,304,64,442,146,79,463,910,438,495,330,501,470,105,264,915,462,50,51,52,869,870,305,513,480,481,501,502,1076,501,483,88,485,493,504,514,65,515,175,41,91,194,195,92,93,94,505,41,44,509,283,821,512,522,521,43,44,45,527,345,64,144,198,277,532,264,459,548,899,901,539,904,978,907,343,982,346,348,348,540,305,67,544,46,89,557,373,573,578,362,363,610,620,919,65,612,348,348,348,90,91,613,614,92,93,94,41,95,44,621,622,304,623,96,43,44,45,348,1008,97,624,98,99,628,106,630,495,100,631,646,496,490,647,101,50,51,52,106,648,67,106,160,161,162,163,164,649,231,165,166,167,1038,1039,651,650,1041,1042,669,1044,41,683,670,684,686,687,691,88,43,44,45,64,476,693,981,476,702,985,987,705,495,671,709,79,713,79,549,79,50,51,52,305,1050,717,346,718,476,719,720,- 529,725,724,476,726,490,65,727,567,568,569,570,571,728,1065,574,730,1066,739,1067,1068,89,366,580,731,732,735,583,738,1017,741,752,747,433,766,755,225,91,757,348,92,93,94,1079,95,44,759,1035,603,770,96,67,773,780,783,786,97,797,98,99,604,1047,803,804,100,580,324,805,1052,810,101,812,348,736,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,814,822,828,834,842,843,845,848,854,855,177,178,856,868,851,858,860,862,639,873,872,896,897,912,909,179,88,913,924,921,922,348,106,923,934,106,925,926,182,183,184,185,186,932,936,938,940,943,944,946,948,188,951,954,189,190,957,191,974,958,989,997,991,476,1002,1013,1003,1009,1037,1048,1056,192,193,1054,79,1053,1058,1059,476,1060,1070,476,1085,1072,1086,580,1006,580,1074,1080,787,1083,771,263,41,91,194,195,92,93,94,116,487,44,348,494,196,782,349,158,126,336,818,1049,197,817,619,198,811,441,749,627,199,895,576,200,0,1075,201,202,588,490,203,204,0,0,0,0,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,490,0,22,23,24,296,88,26,180,0,348,0,181,27,0,145,0,30,182,183,184,185,186,0,31,32,33,187,106,0,0,188,764,0,189,190,38,191,769,0,0,0,0,0,0,0,779,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,580,0,41,91,194,195,92,93,94,42,43,44,45,46,196,688,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,348,200,0,0,201,202,0,490,203,204,826,827,0,829,0,0,0,0,0,0,0,490,0,0,837,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,0,0,0,0,0,0,0,0,348,0,22,23,24,296,88,0,0,0,0,348,0,27,0,0,145,30,182,183,184,185,186,0,31,32,0,0,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,490,348,0,0,0,0,0,0,0,0,0,0,0,0,0,0,490,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,177,178,0,0,0,0,345,561,48,302,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,950,0,0,348,192,193,0,0,0,0,0,490,0,0,0,0,0,0,0,0,0,348,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,348,196,0,349,563,0,0,0,0,197,0,0,198,0,422,423,0,199,0,0,200,0,0,201,202,0,0,203,204,0,0,0,0,0,0,0,0,0,0,0,0,0,0,348,0,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,348,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,793,794,21,177,178,287,348,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,348,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,301,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,299,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,177,178,287,0,288,289,290,291,292,293,294,295,22,23,24,296,88,26,180,297,0,0,181,27,298,0,0,30,182,183,184,185,186,0,31,32,33,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,42,43,44,45,46,196,300,149,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,364,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,177,178,0,0,0,0,0,0,0,0,0,0,22,23,24,179,88,0,180,0,0,0,181,0,0,0,0,30,182,183,184,185,186,0,31,32,0,187,0,0,0,188,0,0,189,190,38,191,0,0,0,0,0,0,0,0,0,0,39,40,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,42,43,44,45,46,196,0,0,0,0,0,0,0,197,0,48,302,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204,- 13,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,661,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 13,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,- 13,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,- 13,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,- 13,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,47,0,48,49,0,0,0,0,50,51,52,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,37,0,0,38,0,874,875,0,0,0,0,0,0,0,0,39,40,0,0,0,0,88,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,41,0,0,0,0,0,0,42,43,44,45,46,22,23,24,- 13,0,0,0,0,47,0,48,49,0,0,0,30,50,51,52,0,0,89,31,32,0,0,0,0,0,0,0,0,0,0,38,0,225,91,0,0,92,93,94,0,95,44,39,40,0,0,96,0,0,0,0,0,876,0,98,99,0,0,0,0,100,0,0,877,0,322,101,202,41,0,203,204,0,0,0,42,43,44,45,46,0,324,0,0,0,0,0,0,471,0,48,49,0,0,453,0,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,89,0,0,0,0,0,0,0,39,40,0,0,750,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,322,0,0,0,97,0,98,99,0,0,42,43,100,45,46,0,324,0,101,0,0,0,0,452,0,48,49,0,0,453,88,50,51,52,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,89,0,31,32,0,0,0,0,0,0,0,0,419,0,38,420,91,0,0,92,93,94,0,421,44,0,39,40,0,96,0,0,0,0,0,97,0,98,99,0,422,423,0,100,0,0,0,0,0,101,0,0,41,0,0,0,0,0,0,42,43,44,45,46,0,0,0,0,0,0,0,0,495,0,48,49,0,0,0,0,50,51,52,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,88,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,89,0,0,0,0,0,0,0,39,40,0,0,0,0,0,225,91,0,0,92,93,94,0,95,44,0,0,0,0,96,0,0,0,0,0,97,0,98,99,0,0,42,43,100,45,46,0,0,149,101,0,277,0,0,0,0,48,49,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,2,3,4,82,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,42,43,0,45,46,0,0,149,22,23,24,25,0,0,0,48,49,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,88,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,89,31,32,0,0,0,0,0,0,0,0,0,748,38,0,420,91,0,0,92,93,94,0,421,44,39,40,0,0,96,0,0,0,0,0,97,0,98,99,0,422,423,0,100,0,0,0,0,322,101,0,323,0,0,0,0,0,0,42,43,0,45,46,0,324,0,0,0,0,0,0,0,0,48,49,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,240,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,42,43,0,45,46,0,0,241,22,23,24,0,0,0,0,48,49,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,177,178,0,0,0,0,0,38,0,0,0,0,0,0,0,179,88,0,180,39,40,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,322,0,189,190,0,191,0,0,0,0,42,43,0,45,46,0,324,0,0,192,193,0,0,0,0,48,49,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,22,23,24,0,0,0,197,0,0,198,0,0,0,329,199,30,0,200,0,0,201,202,31,32,203,204,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,443,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,703,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,710,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,42,43,0,45,46,22,23,24,0,0,0,0,0,0,0,48,49,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,0,38,0,0,0,0,0,0,0,0,0,0,0,39,40,0,0,0,0,0,0,0,0,177,178,287,0,288,289,290,291,292,293,294,295,0,0,0,179,88,0,180,297,0,0,181,42,298,0,0,46,182,183,184,185,186,0,0,0,0,187,48,49,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,510,91,194,195,92,93,94,0,0,44,0,0,196,300,149,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,823,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,835,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,998,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,562,91,194,195,92,93,94,0,257,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,422,423,0,199,177,178,200,0,0,201,202,0,0,203,204,0,0,0,0,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,349,0,0,0,0,0,197,0,0,198,0,177,178,0,199,0,0,200,0,0,201,202,0,0,203,204,179,88,0,180,0,0,0,181,0,0,0,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,516,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,606,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,638,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,694,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,763,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,778,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,798,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,800,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,900,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,903,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,906,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,980,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,984,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,986,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,1046,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,180,0,201,202,181,0,203,204,0,0,182,183,184,185,186,0,0,0,0,187,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,768,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,736,0,0,177,178,0,197,0,0,198,0,0,0,0,199,0,0,200,179,88,201,202,0,0,203,204,0,0,0,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,342,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,345,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,177,178,0,0,0,0,197,0,0,198,0,0,0,0,199,179,88,200,0,0,201,202,0,0,203,204,0,0,182,183,184,185,186,0,0,0,0,0,0,0,0,188,0,0,189,190,0,191,0,0,0,0,0,0,0,0,0,0,0,0,0,192,193,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,41,91,194,195,92,93,94,0,0,44,0,0,196,0,0,0,0,0,0,0,949,0,0,198,0,0,0,0,199,0,0,200,0,0,201,202,0,0,203,204};
# 2778
static short Cyc_yycheck[7471U]={0,379,380,118,123,181,478,122,373,139,385,386,155,376,73,387,388,109,110,375,168,21,181,378,277,25,70,197,303,304,423,306,349,155,0,199,200,201,170,180,279,21,176,459,785,25,17,47,26,535,70,107,248,111,78,21,220,57,95,25,243,533,264,63,64,65,611,67,409,129,542,241,116,73,74,454,109,316,95,0,139,109,138,63,64,65,803,67,474,364,149,57,151,73,127,474,116,63,64,65,21,67,139,496,25,5,6,73,109,133,122,89,853,113,114,501,73,381,382,383,384,89,139,101,540,321,127,127,128,129,155,131,132,133,134,63,57,245,109,139,26,337,63,64,65,631,67,113,114,149,89,151,73,125,127,872,264,125,158,139,128,127,128,129,136,131,132,133,134,149,130,151,342,139,344,345,324,109,548,520,140,127,139,149,327,151,118,116,116,128,360,736,113,114,151,123,90,91,109,340,57,140,131,89,338,122,127,128,129,126,131,132,133,134,123,101,127,116,139,278,264,122,133,129,124,389,390,391,149,109,151,948,131,127,128,144,125,954,96,283,332,130,122,243,303,304,155,306,130,227,130,86,491,133,232,52,113,114,140,976,122,239,97,130,447,448,449,129,638,471,127,128,129,140,131,132,133,134,278,672,115,127,423,130,109,110,47,134,134,147,148,406,117,495,611,410,296,1014,278,129,109,62,123,303,304,364,306,422,129,118,815,459,278,566,90,91,296,85,86,87,129,278,243,303,304,129,306,125,109,76,296,135,130,242,334,244,117,303,304,844,306,227,683,125,230,817,232,1064,130,108,442,127,128,239,108,496,290,278,109,120,1077,129,197,297,364,535,134,135,85,86,87,122,281,125,125,296,127,20,21,120,238,133,303,304,364,306,127,109,129,759,783,152,540,763,135,525,118,255,364,125,129,125,237,522,130,722,122,129,778,133,132,109,129,123,578,109,122,134,135,736,118,696,120,123,118,930,931,471,137,123,474,129,937,137,138,139,145,146,530,137,138,139,612,364,614,539,424,447,448,449,652,495,127,512,140,126,456,501,144,437,295,631,521,134,135,667,525,137,140,722,120,471,144,473,109,145,146,127,610,129,109,851,482,118,484,135,486,122,117,118,119,111,862,125,129,495,1001,125,245,642,1005,133,137,138,139,133,125,342,123,641,345,130,512,656,123,276,482,264,484,912,486,521,357,655,841,525,447,448,449,109,110,672,512,127,656,109,283,117,424,374,109,521,129,452,305,525,512,743,117,118,119,437,122,729,440,521,414,705,701,525,129,482,471,484,760,486,135,710,525,478,729,109,825,122,123,991,411,125,774,117,118,119,130,961,962,130,124,521,841,134,955,512,967,968,0,958,959,969,970,740,521,957,125,718,525,960,125,130,652,125,702,928,126,522,130,709,22,642,733,734,26,896,28,667,533,482,670,484,116,486,551,656,122,542,129,125,543,696,130,698,640,47,783,120,502,503,624,58,122,125,127,57,129,125,130,125,62,125,135,659,130,755,130,663,70,129,72,125,125,75,505,506,130,130,509,928,125,512,84,954,122,130,129,89,125,96,97,98,99,100,101,102,103,104,105,101,696,663,698,963,964,965,966,126,125,452,876,113,114,130,116,129,125,877,120,126,696,130,698,122,123,127,128,129,471,131,132,133,134,636,696,895,698,642,125,621,780,124,624,130,125,698,663,652,94,130,152,656,495,868,129,845,686,923,848,122,109,122,125,122,125,129,125,122,858,118,125,674,92,93,176,912,129,916,129,696,129,698,129,949,129,951,81,82,137,138,139,129,122,1064,648,125,109,135,715,137,138,139,88,89,117,118,119,918,5,6,663,971,972,973,122,646,129,125,122,793,122,125,129,125,122,227,109,125,230,995,232,17,18,19,117,118,119,239,447,448,449,896,932,124,125,129,129,991,938,22,23,129,135,1013,137,138,139,688,689,762,790,129,264,129,767,141,796,124,125,41,124,125,142,776,276,109,124,142,143,143,785,283,123,285,92,93,288,109,1054,88,89,124,125,116,66,117,118,119,120,954,896,129,942,305,128,808,109,129,156,652,130,793,817,816,109,137,138,139,788,789,896,109,124,124,667,123,1063,670,124,41,124,124,123,122,896,122,338,109,110,111,112,113,114,115,129,109,118,129,853,1060,129,129,133,117,118,119,124,129,954,123,132,126,109,868,140,136,797,798,130,800,900,802,177,903,179,180,181,133,954,896,130,120,94,124,95,109,130,192,193,127,124,822,954,128,199,200,201,109,110,128,128,113,114,115,109,117,118,125,133,1064,130,123,117,118,119,220,946,129,125,131,132,130,424,124,129,137,125,129,133,277,109,143,137,138,139,437,123,954,440,73,74,75,76,77,129,876,80,81,82,979,980,125,130,983,984,122,986,109,129,125,123,133,30,122,41,117,118,119,1064,471,45,902,474,129,905,906,140,129,78,130,482,130,484,331,486,137,138,139,1064,992,130,296,125,495,130,109,133,125,130,501,125,349,1064,125,352,353,354,355,356,125,1040,359,130,1043,125,1045,1046,94,949,367,130,130,124,371,130,956,130,124,136,106,122,130,109,110,130,340,113,114,115,1069,117,118,136,974,392,130,123,1064,130,123,129,124,129,129,131,132,404,988,123,122,137,409,122,130,995,124,143,79,373,123,375,376,377,378,379,380,381,382,383,384,385,386,387,388,389,390,391,128,127,25,130,109,109,130,130,125,130,25,26,130,129,136,130,130,136,453,130,123,133,124,129,122,40,41,116,125,130,130,423,621,129,122,624,130,130,53,54,55,56,57,130,110,130,124,130,124,28,123,66,129,133,69,70,94,72,94,141,124,25,125,652,124,130,125,124,122,130,110,86,87,129,663,130,124,130,667,125,124,670,0,130,0,520,942,522,130,130,679,130,651,122,109,110,111,112,113,114,115,39,276,118,496,281,121,670,123,75,51,169,715,991,129,710,425,132,705,239,621,440,137,793,361,140,- 1,1060,143,144,377,566,147,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,611,- 1,37,38,39,40,41,42,43,- 1,578,- 1,47,48,- 1,780,- 1,52,53,54,55,56,57,- 1,59,60,61,62,793,- 1,- 1,66,643,- 1,69,70,71,72,649,- 1,- 1,- 1,- 1,- 1,- 1,- 1,657,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,683,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,672,140,- 1,- 1,143,144,- 1,722,147,148,725,726,- 1,728,- 1,- 1,- 1,- 1,- 1,- 1,- 1,736,- 1,- 1,739,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,729,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,740,- 1,48,- 1,- 1,942,52,53,54,55,56,57,- 1,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,825,783,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,841,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,25,26,- 1,- 1,- 1,- 1,129,32,131,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,874,- 1,- 1,877,86,87,- 1,- 1,- 1,- 1,- 1,928,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,895,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,912,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,955,- 1,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,973,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1013,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,1063,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,71,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,68,- 1,- 1,71,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,41,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,37,38,39,124,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,52,137,138,139,- 1,- 1,94,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,83,84,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,106,143,144,109,- 1,147,148,- 1,- 1,- 1,116,117,118,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,135,- 1,137,138,139,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,106,- 1,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,- 1,- 1,- 1,- 1,123,- 1,106,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,116,117,137,119,120,- 1,122,- 1,143,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,135,41,137,138,139,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,94,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,- 1,71,109,110,- 1,- 1,113,114,115,- 1,117,118,- 1,83,84,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,134,135,- 1,137,- 1,- 1,- 1,- 1,- 1,143,- 1,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,- 1,- 1,137,138,139,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,94,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,- 1,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,- 1,116,117,137,119,120,- 1,- 1,123,143,- 1,126,- 1,- 1,- 1,- 1,131,132,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,- 1,123,37,38,39,40,- 1,- 1,- 1,131,132,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,41,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,94,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,106,71,- 1,109,110,- 1,- 1,113,114,115,- 1,117,118,83,84,- 1,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,131,132,- 1,134,135,- 1,137,- 1,- 1,- 1,- 1,106,143,- 1,109,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,- 1,123,37,38,39,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,41,- 1,43,83,84,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,106,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,116,117,- 1,119,120,- 1,122,- 1,- 1,86,87,- 1,- 1,- 1,- 1,131,132,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,37,38,39,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,52,- 1,140,- 1,- 1,143,144,59,60,147,148,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,124,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,124,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,106,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,117,- 1,119,120,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,131,132,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,83,84,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,47,116,49,- 1,- 1,120,53,54,55,56,57,- 1,- 1,- 1,- 1,62,131,132,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,25,26,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,124,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,117,118,- 1,- 1,121,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,134,135,- 1,137,25,26,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,25,26,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,136,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,122,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,130,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,43,- 1,143,144,47,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,123,- 1,- 1,25,26,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,40,41,143,144,- 1,- 1,147,148,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,25,26,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,40,41,140,- 1,- 1,143,144,- 1,- 1,147,148,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,70,- 1,72,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,86,87,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,109,110,111,112,113,114,115,- 1,- 1,118,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,- 1,- 1,132,- 1,- 1,- 1,- 1,137,- 1,- 1,140,- 1,- 1,143,144,- 1,- 1,147,148};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp422[8U]="stdcall";static char _tmp423[6U]="cdecl";static char _tmp424[9U]="fastcall";static char _tmp425[9U]="noreturn";static char _tmp426[6U]="const";static char _tmp427[8U]="aligned";static char _tmp428[7U]="packed";static char _tmp429[7U]="shared";static char _tmp42A[7U]="unused";static char _tmp42B[5U]="weak";static char _tmp42C[10U]="dllimport";static char _tmp42D[10U]="dllexport";static char _tmp42E[23U]="no_instrument_function";static char _tmp42F[12U]="constructor";static char _tmp430[11U]="destructor";static char _tmp431[22U]="no_check_memory_usage";static char _tmp432[5U]="pure";static char _tmp433[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp386=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp386;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp744;(_tmp744.YYINITIALSVAL).tag=64U,(_tmp744.YYINITIALSVAL).val=0;_tmp744;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp694=200U;_tag_dyneither(_region_calloc(yyregion,sizeof(short),_tmp694),sizeof(short),_tmp694);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp693=200U;union Cyc_YYSTYPE*_tmp692=({struct _RegionHandle*_tmp7FB=yyregion;_region_malloc(_tmp7FB,_check_times(_tmp693,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp743=200U;unsigned int i;for(i=0;i < _tmp743;++ i){_tmp692[i]=yylval;}}0;});_tmp692;}),sizeof(union Cyc_YYSTYPE),200U);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp691=200U;struct Cyc_Yyltype*_tmp690=({struct _RegionHandle*_tmp7FC=yyregion;_region_malloc(_tmp7FC,_check_times(_tmp691,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp742=200U;unsigned int i;for(i=0;i < _tmp742;++ i){({struct Cyc_Yyltype _tmp7FD=Cyc_yynewloc();_tmp690[i]=_tmp7FD;});}}0;});_tmp690;}),sizeof(struct Cyc_Yyltype),200U);
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
({struct _dyneither_ptr _tmp7FF=({const char*_tmp387="parser stack overflow";_tag_dyneither(_tmp387,sizeof(char),22U);});int _tmp7FE=yystate;Cyc_yyerror(_tmp7FF,_tmp7FE,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp38D=(unsigned int)yystacksize;short*_tmp38C=({struct _RegionHandle*_tmp800=yyregion;_region_malloc(_tmp800,_check_times(_tmp38D,sizeof(short)));});({{unsigned int _tmp702=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp702;++ i){
i <= yyssp_offset?_tmp38C[i]=*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)):(_tmp38C[i]=0);}}0;});_tag_dyneither(_tmp38C,sizeof(short),_tmp38D);});
struct _dyneither_ptr yyvs1=({unsigned int _tmp38B=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp38A=({struct _RegionHandle*_tmp801=yyregion;_region_malloc(_tmp801,_check_times(_tmp38B,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp701=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp701;++ i){
# 219
i <= yyssp_offset?_tmp38A[i]=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)):(_tmp38A[i]=yylval);}}0;});_tag_dyneither(_tmp38A,sizeof(union Cyc_YYSTYPE),_tmp38B);});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp389=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp388=({struct _RegionHandle*_tmp802=yyregion;_region_malloc(_tmp802,_check_times(_tmp389,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp700=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp700;++ i){
i <= yyssp_offset?_tmp388[i]=*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):({
struct Cyc_Yyltype _tmp803=Cyc_yynewloc();_tmp388[i]=_tmp803;});}}0;});_tag_dyneither(_tmp388,sizeof(struct Cyc_Yyltype),_tmp389);});
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
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
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
if((yyn < 0  || yyn > 7470) || *((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),yyn))!= yychar1)goto yydefault;
# 300
yyn=(int)Cyc_yytable[yyn];
# 307
if(yyn < 0){
# 309
if(yyn == - 32768)goto yyerrlab;
yyn=- yyn;
goto yyreduce;}else{
# 313
if(yyn == 0)goto yyerrlab;}
# 315
if(yyn == 1086){
int _tmp38E=0;_npop_handler(0U);return _tmp38E;}
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
{int _tmp38F=yyn;switch(_tmp38F){case 1U: _LL1: _LL2: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1202 "parse.y"
yyval=yyyyvsp[0];
Cyc_Parse_parse_result=Cyc_yyget_YY16(yyyyvsp[0]);
# 1205
goto _LL0;}case 2U: _LL3: _LL4: {
# 1207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1208 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp390;_push_handler(& _tmp390);{int _tmp392=0;if(setjmp(_tmp390.handler))_tmp392=1;if(!_tmp392){x=Cyc_yyget_YY16(yyyyvsp[0]);;_pop_handler();}else{void*_tmp391=(void*)_exn_thrown;void*_tmp393=_tmp391;void*_tmp394;if(((struct Cyc_Core_Failure_exn_struct*)_tmp393)->tag == Cyc_Core_Failure){_LL428: _LL429:
 x=0;goto _LL427;}else{_LL42A: _tmp394=_tmp393;_LL42B:(int)_rethrow(_tmp394);}_LL427:;}};}
{struct _handler_cons _tmp395;_push_handler(& _tmp395);{int _tmp397=0;if(setjmp(_tmp395.handler))_tmp397=1;if(!_tmp397){y=Cyc_yyget_YY16(yyyyvsp[1]);;_pop_handler();}else{void*_tmp396=(void*)_exn_thrown;void*_tmp398=_tmp396;void*_tmp399;if(((struct Cyc_Core_Failure_exn_struct*)_tmp398)->tag == Cyc_Core_Failure){_LL42D: _LL42E:
 y=0;goto _LL42C;}else{_LL42F: _tmp399=_tmp398;_LL430:(int)_rethrow(_tmp399);}_LL42C:;}};}
yyval=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));
# 1215
goto _LL0;}case 3U: _LL5: _LL6: {
# 1217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1218 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));({struct Cyc_Absyn_Decl*_tmp807=({struct Cyc_Absyn_Decl*_tmp39B=_cycalloc(sizeof(*_tmp39B));({void*_tmp806=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->tag=10U,({struct _tuple0*_tmp805=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp39A->f1=_tmp805;}),({struct Cyc_List_List*_tmp804=Cyc_yyget_YY16(yyyyvsp[2]);_tmp39A->f2=_tmp804;});_tmp39A;});_tmp39B->r=_tmp806;}),_tmp39B->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp39B;});_tmp39C->hd=_tmp807;}),_tmp39C->tl=0;_tmp39C;}));
Cyc_Lex_leave_using();
# 1221
goto _LL0;}case 4U: _LL7: _LL8: {
# 1223
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1225
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1222 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp39F=_cycalloc(sizeof(*_tmp39F));({struct Cyc_Absyn_Decl*_tmp80C=({struct Cyc_Absyn_Decl*_tmp39E=_cycalloc(sizeof(*_tmp39E));({void*_tmp80B=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39D=_cycalloc(sizeof(*_tmp39D));_tmp39D->tag=10U,({struct _tuple0*_tmp80A=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp39D->f1=_tmp80A;}),({struct Cyc_List_List*_tmp809=Cyc_yyget_YY16(yyyyvsp[2]);_tmp39D->f2=_tmp809;});_tmp39D;});_tmp39E->r=_tmp80B;}),_tmp39E->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp39E;});_tmp39F->hd=_tmp80C;}),({struct Cyc_List_List*_tmp808=Cyc_yyget_YY16(yyyyvsp[4]);_tmp39F->tl=_tmp808;});_tmp39F;}));
goto _LL0;}case 5U: _LL9: _LLA: {
# 1225
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1227
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1225 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_Absyn_Decl*_tmp811=({struct Cyc_Absyn_Decl*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({void*_tmp810=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->tag=9U,({struct _dyneither_ptr*_tmp80F=({struct _dyneither_ptr*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct _dyneither_ptr _tmp80E=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp3A0=_tmp80E;});_tmp3A0;});_tmp3A1->f1=_tmp80F;}),({struct Cyc_List_List*_tmp80D=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A1->f2=_tmp80D;});_tmp3A1;});_tmp3A2->r=_tmp810;}),_tmp3A2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A2;});_tmp3A3->hd=_tmp811;}),_tmp3A3->tl=0;_tmp3A3;}));
Cyc_Lex_leave_namespace();
# 1228
goto _LL0;}case 6U: _LLB: _LLC: {
# 1230
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1232
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1230 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp3A4;_push_handler(& _tmp3A4);{int _tmp3A6=0;if(setjmp(_tmp3A4.handler))_tmp3A6=1;if(!_tmp3A6){nspace=Cyc_yyget_String_tok(yyyyvsp[0]);;_pop_handler();}else{void*_tmp3A5=(void*)_exn_thrown;void*_tmp3A7=_tmp3A5;void*_tmp3A9;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3A7)->tag == Cyc_Core_Failure){_LL432: _LL433:
 nspace=({const char*_tmp3A8="";_tag_dyneither(_tmp3A8,sizeof(char),1U);});goto _LL431;}else{_LL434: _tmp3A9=_tmp3A7;_LL435:(int)_rethrow(_tmp3A9);}_LL431:;}};}
# 1235
{struct _handler_cons _tmp3AA;_push_handler(& _tmp3AA);{int _tmp3AC=0;if(setjmp(_tmp3AA.handler))_tmp3AC=1;if(!_tmp3AC){x=Cyc_yyget_YY16(yyyyvsp[2]);;_pop_handler();}else{void*_tmp3AB=(void*)_exn_thrown;void*_tmp3AD=_tmp3AB;void*_tmp3AE;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3AD)->tag == Cyc_Core_Failure){_LL437: _LL438:
 x=0;goto _LL436;}else{_LL439: _tmp3AE=_tmp3AD;_LL43A:(int)_rethrow(_tmp3AE);}_LL436:;}};}
# 1238
{struct _handler_cons _tmp3AF;_push_handler(& _tmp3AF);{int _tmp3B1=0;if(setjmp(_tmp3AF.handler))_tmp3B1=1;if(!_tmp3B1){y=Cyc_yyget_YY16(yyyyvsp[4]);;_pop_handler();}else{void*_tmp3B0=(void*)_exn_thrown;void*_tmp3B2=_tmp3B0;void*_tmp3B3;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3B2)->tag == Cyc_Core_Failure){_LL43C: _LL43D:
 y=0;goto _LL43B;}else{_LL43E: _tmp3B3=_tmp3B2;_LL43F:(int)_rethrow(_tmp3B3);}_LL43B:;}};}
# 1241
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({struct Cyc_Absyn_Decl*_tmp814=({struct Cyc_Absyn_Decl*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));({void*_tmp813=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3B5=_cycalloc(sizeof(*_tmp3B5));_tmp3B5->tag=9U,({struct _dyneither_ptr*_tmp812=({struct _dyneither_ptr*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));*_tmp3B4=nspace;_tmp3B4;});_tmp3B5->f1=_tmp812;}),_tmp3B5->f2=x;_tmp3B5;});_tmp3B6->r=_tmp813;}),_tmp3B6->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B6;});_tmp3B7->hd=_tmp814;}),_tmp3B7->tl=y;_tmp3B7;}));
# 1243
goto _LL0;}case 7U: _LLD: _LLE: {
# 1245
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1247
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1244 "parse.y"
int _tmp3B8=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp3B8)
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct Cyc_Absyn_Decl*_tmp818=({struct Cyc_Absyn_Decl*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({void*_tmp817=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=11U,({struct Cyc_List_List*_tmp816=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B9->f1=_tmp816;});_tmp3B9;});_tmp3BA->r=_tmp817;}),_tmp3BA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BA;});_tmp3BB->hd=_tmp818;}),({struct Cyc_List_List*_tmp815=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3BB->tl=_tmp815;});_tmp3BB;}));else{
# 1248
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({struct Cyc_Absyn_Decl*_tmp81C=({struct Cyc_Absyn_Decl*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({void*_tmp81B=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));_tmp3BC->tag=12U,({struct Cyc_List_List*_tmp81A=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3BC->f1=_tmp81A;}),_tmp3BC->f2=0;_tmp3BC;});_tmp3BD->r=_tmp81B;}),_tmp3BD->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BD;});_tmp3BE->hd=_tmp81C;}),({struct Cyc_List_List*_tmp819=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3BE->tl=_tmp819;});_tmp3BE;}));}
# 1250
goto _LL0;}case 8U: _LLF: _LL10: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1251 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp3BF=0U;({unsigned int _tmp81E=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp81D=({const char*_tmp3C0="expecting \"C include\"";_tag_dyneither(_tmp3C0,sizeof(char),22U);});Cyc_Warn_err(_tmp81E,_tmp81D,_tag_dyneither(_tmp3BF,sizeof(void*),0U));});});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct Cyc_Absyn_Decl*_tmp822=({struct Cyc_Absyn_Decl*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({void*_tmp821=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->tag=12U,({struct Cyc_List_List*_tmp820=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C1->f1=_tmp820;}),_tmp3C1->f2=exs;_tmp3C1;});_tmp3C2->r=_tmp821;}),_tmp3C2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C2;});_tmp3C3->hd=_tmp822;}),({struct Cyc_List_List*_tmp81F=Cyc_yyget_YY16(yyyyvsp[5]);_tmp3C3->tl=_tmp81F;});_tmp3C3;}));
# 1256
goto _LL0;};}case 9U: _LL11: _LL12: {
# 1258
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1260
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1257 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct Cyc_Absyn_Decl*_tmp824=({struct Cyc_Absyn_Decl*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C4->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C4;});_tmp3C5->hd=_tmp824;}),({struct Cyc_List_List*_tmp823=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C5->tl=_tmp823;});_tmp3C5;}));
goto _LL0;}case 10U: _LL13: _LL14: {
# 1260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1259 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct Cyc_Absyn_Decl*_tmp826=({struct Cyc_Absyn_Decl*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3C6->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C6;});_tmp3C7->hd=_tmp826;}),({struct Cyc_List_List*_tmp825=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C7->tl=_tmp825;});_tmp3C7;}));
goto _LL0;}case 11U: _LL15: _LL16: {
# 1262
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1264
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1261 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({struct Cyc_Absyn_Decl*_tmp828=({struct Cyc_Absyn_Decl*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->r=(void*)& Cyc_Absyn_Tempeston_d_val,_tmp3C8->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C8;});_tmp3C9->hd=_tmp828;}),({struct Cyc_List_List*_tmp827=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C9->tl=_tmp827;});_tmp3C9;}));
goto _LL0;}case 12U: _LL17: _LL18: {
# 1264
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1263 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));({struct Cyc_Absyn_Decl*_tmp82A=({struct Cyc_Absyn_Decl*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->r=(void*)& Cyc_Absyn_Tempestoff_d_val,_tmp3CA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3CA;});_tmp3CB->hd=_tmp82A;}),({struct Cyc_List_List*_tmp829=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3CB->tl=_tmp829;});_tmp3CB;}));
goto _LL0;}case 13U: _LL19: _LL1A:
# 1266
 yyval=Cyc_YY16(0);
goto _LL0;case 14U: _LL1B: _LL1C: {
# 1269
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1271
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1269 "parse.y"
Cyc_Parse_parsing_tempest=1;
goto _LL0;}case 15U: _LL1D: _LL1E: {
# 1272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1274 "parse.y"
Cyc_Parse_parsing_tempest=0;
goto _LL0;}case 16U: _LL1F: _LL20: {
# 1277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1279 "parse.y"
struct _dyneither_ptr _tmp3CC=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({struct _dyneither_ptr _tmp82B=(struct _dyneither_ptr)_tmp3CC;Cyc_strcmp(_tmp82B,({const char*_tmp3CD="C";_tag_dyneither(_tmp3CD,sizeof(char),2U);}));})== 0)
yyval=Cyc_YY31(0);else{
if(({struct _dyneither_ptr _tmp82C=(struct _dyneither_ptr)_tmp3CC;Cyc_strcmp(_tmp82C,({const char*_tmp3CE="C include";_tag_dyneither(_tmp3CE,sizeof(char),10U);}));})== 0)
yyval=Cyc_YY31(1);else{
# 1286
({void*_tmp3CF=0U;({unsigned int _tmp82E=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp82D=({const char*_tmp3D0="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3D0,sizeof(char),29U);});Cyc_Warn_err(_tmp82E,_tmp82D,_tag_dyneither(_tmp3CF,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);}}
# 1290
goto _LL0;}case 17U: _LL21: _LL22: {
# 1292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1293 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL0;}case 18U: _LL23: _LL24: {
# 1296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1297 "parse.y"
yyval=yyyyvsp[2];
goto _LL0;}case 19U: _LL25: _LL26: {
# 1300
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1298 "parse.y"
yyval=Cyc_YY52(0);
goto _LL0;}case 20U: _LL27: _LL28: {
# 1301
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1303
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1302 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct _tuple27*_tmp830=({struct _tuple27*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp82F=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D1->f2=_tmp82F;}),_tmp3D1->f3=0;_tmp3D1;});_tmp3D2->hd=_tmp830;}),_tmp3D2->tl=0;_tmp3D2;}));
goto _LL0;}case 21U: _LL29: _LL2A: {
# 1305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1303 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct _tuple27*_tmp832=({struct _tuple27*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp831=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D3->f2=_tmp831;}),_tmp3D3->f3=0;_tmp3D3;});_tmp3D4->hd=_tmp832;}),_tmp3D4->tl=0;_tmp3D4;}));
goto _LL0;}case 22U: _LL2B: _LL2C: {
# 1306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1305 "parse.y"
yyval=Cyc_YY52(({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _tuple27*_tmp835=({struct _tuple27*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp834=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D5->f2=_tmp834;}),_tmp3D5->f3=0;_tmp3D5;});_tmp3D6->hd=_tmp835;}),({struct Cyc_List_List*_tmp833=Cyc_yyget_YY52(yyyyvsp[2]);_tmp3D6->tl=_tmp833;});_tmp3D6;}));
goto _LL0;}case 23U: _LL2D: _LL2E: {
# 1308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1309 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));({struct Cyc_Absyn_Decl*_tmp838=({void*_tmp837=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp836=Cyc_yyget_YY15(yyyyvsp[0]);_tmp3D7->f1=_tmp836;});_tmp3D7;});Cyc_Absyn_new_decl(_tmp837,(unsigned int)(yyyylsp[0]).first_line);});_tmp3D8->hd=_tmp838;}),_tmp3D8->tl=0;_tmp3D8;}));
goto _LL0;}case 24U: _LL2F: _LL30: {
# 1312
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1314
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1310 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 25U: _LL31: _LL32: {
# 1313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1311 "parse.y"
yyval=Cyc_YY16(0);
goto _LL0;}case 28U: _LL33: _LL34: {
# 1314
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1316
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1320 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp83B=yyr;struct Cyc_Parse_Declarator _tmp83A=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp839=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp83B,0,_tmp83A,0,_tmp839,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 29U: _LL35: _LL36: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1322 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D9=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp83E=yyr;struct Cyc_Parse_Declarator _tmp83D=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp83C=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp83E,& _tmp3D9,_tmp83D,0,_tmp83C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 30U: _LL37: _LL38: {
# 1326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1335 "parse.y"
yyval=Cyc_YY15(({struct _RegionHandle*_tmp842=yyr;struct Cyc_Parse_Declarator _tmp841=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp840=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp83F=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp842,0,_tmp841,_tmp840,_tmp83F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 31U: _LL39: _LL3A: {
# 1338
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1337 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DA=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp846=yyr;struct Cyc_Parse_Declarator _tmp845=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp844=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp843=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp846,& _tmp3DA,_tmp845,_tmp844,_tmp843,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 32U: _LL3B: _LL3C: {
# 1341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1345 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DB=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp849=yyr;struct Cyc_Parse_Declarator _tmp848=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp847=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp849,& _tmp3DB,_tmp848,0,_tmp847,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 33U: _LL3D: _LL3E: {
# 1349
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1351
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1348 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DC=Cyc_yyget_YY17(yyyyvsp[0]);
yyval=Cyc_YY15(({struct _RegionHandle*_tmp84D=yyr;struct Cyc_Parse_Declarator _tmp84C=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp84B=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp84A=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp84D,& _tmp3DC,_tmp84C,_tmp84B,_tmp84A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 34U: _LL3F: _LL40: {
# 1352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1353 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL0;}case 35U: _LL41: _LL42: {
# 1356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1356 "parse.y"
Cyc_Lex_leave_using();
goto _LL0;}case 36U: _LL43: _LL44: {
# 1359
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1359 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct _dyneither_ptr _tmp84E=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp3DD=_tmp84E;});_tmp3DD;}));yyval=yyyyvsp[1];
goto _LL0;}case 37U: _LL45: _LL46: {
# 1362
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1364
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1362 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL0;}case 38U: _LL47: _LL48: {
# 1365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
int _tmp3DE=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp850=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp84F=(unsigned int)_tmp3DE;Cyc_Parse_make_declarations(_tmp850,0,_tmp84F,(unsigned int)_tmp3DE);}));
goto _LL0;}case 39U: _LL49: _LL4A: {
# 1372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1371 "parse.y"
int _tmp3DF=(yyyylsp[0]).first_line;
yyval=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp853=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp852=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp851=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp853,_tmp852,_tmp851,(unsigned int)_tmp3DF);}));
goto _LL0;}case 40U: _LL4B: _LL4C: {
# 1375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1375 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct Cyc_Absyn_Decl*_tmp856=({struct Cyc_Absyn_Pat*_tmp855=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp854=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp855,_tmp854,(unsigned int)(yyyylsp[0]).first_line);});_tmp3E0->hd=_tmp856;}),_tmp3E0->tl=0;_tmp3E0;}));
goto _LL0;}case 41U: _LL4D: _LL4E: {
# 1378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1377 "parse.y"
struct Cyc_List_List*_tmp3E1=0;
{struct Cyc_List_List*_tmp3E2=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp3E2 != 0;_tmp3E2=_tmp3E2->tl){
struct _dyneither_ptr*_tmp3E3=(struct _dyneither_ptr*)_tmp3E2->hd;
struct _tuple0*qv=({struct _tuple0*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({union Cyc_Absyn_Nmspace _tmp857=Cyc_Absyn_Rel_n(0);_tmp3E6->f1=_tmp857;}),_tmp3E6->f2=_tmp3E3;_tmp3E6;});
struct Cyc_Absyn_Vardecl*_tmp3E4=({struct _tuple0*_tmp858=qv;Cyc_Absyn_new_vardecl(0U,_tmp858,Cyc_Absyn_wildtyp(0),0);});
_tmp3E1=({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->hd=_tmp3E4,_tmp3E5->tl=_tmp3E1;_tmp3E5;});}}
# 1384
_tmp3E1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3E1);
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));({struct Cyc_Absyn_Decl*_tmp859=Cyc_Absyn_letv_decl(_tmp3E1,(unsigned int)(yyyylsp[0]).first_line);_tmp3E7->hd=_tmp859;}),_tmp3E7->tl=0;_tmp3E7;}));
# 1387
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1390 "parse.y"
struct _dyneither_ptr _tmp3E8=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1392
if(({struct _dyneither_ptr _tmp85A=(struct _dyneither_ptr)_tmp3E8;Cyc_zstrcmp(_tmp85A,({const char*_tmp3E9="`H";_tag_dyneither(_tmp3E9,sizeof(char),3U);}));})== 0)
({void*_tmp3EA=0U;({unsigned int _tmp85C=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp85B=({const char*_tmp3EB="bad occurrence of heap region";_tag_dyneither(_tmp3EB,sizeof(char),30U);});Cyc_Warn_err(_tmp85C,_tmp85B,_tag_dyneither(_tmp3EA,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp85D=(struct _dyneither_ptr)_tmp3E8;Cyc_zstrcmp(_tmp85D,({const char*_tmp3EC="`U";_tag_dyneither(_tmp3EC,sizeof(char),3U);}));})== 0)
({void*_tmp3ED=0U;({unsigned int _tmp85F=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp85E=({const char*_tmp3EE="bad occurrence of unique region";_tag_dyneither(_tmp3EE,sizeof(char),32U);});Cyc_Warn_err(_tmp85F,_tmp85E,_tag_dyneither(_tmp3ED,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));({struct _dyneither_ptr*_tmp861=({struct _dyneither_ptr*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));*_tmp3F2=_tmp3E8;_tmp3F2;});_tmp3F3->name=_tmp861;}),_tmp3F3->identity=- 1,({void*_tmp860=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3F3->kind=_tmp860;});_tmp3F3;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp865=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp864=({struct _tuple0*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp863=({struct _dyneither_ptr*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));({struct _dyneither_ptr _tmp862=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp3F0=_tmp862;});_tmp3F0;});_tmp3F1->f2=_tmp863;});_tmp3F1;});Cyc_Absyn_new_vardecl(_tmp865,_tmp864,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));({struct Cyc_Absyn_Decl*_tmp866=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3EF->hd=_tmp866;}),_tmp3EF->tl=0;_tmp3EF;}));
# 1401
goto _LL0;};}case 43U: _LL51: _LL52: {
# 1403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1403 "parse.y"
struct _dyneither_ptr _tmp3F4=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({struct _dyneither_ptr _tmp867=(struct _dyneither_ptr)_tmp3F4;Cyc_zstrcmp(_tmp867,({const char*_tmp3F5="H";_tag_dyneither(_tmp3F5,sizeof(char),2U);}));})== 0)
({void*_tmp3F6=0U;({unsigned int _tmp869=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp868=({const char*_tmp3F7="bad occurrence of heap region `H";_tag_dyneither(_tmp3F7,sizeof(char),33U);});Cyc_Warn_err(_tmp869,_tmp868,_tag_dyneither(_tmp3F6,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp86A=(struct _dyneither_ptr)_tmp3F4;Cyc_zstrcmp(_tmp86A,({const char*_tmp3F8="U";_tag_dyneither(_tmp3F8,sizeof(char),2U);}));})== 0)
({void*_tmp3F9=0U;({unsigned int _tmp86C=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp86B=({const char*_tmp3FA="bad occurrence of unique region `U";_tag_dyneither(_tmp3FA,sizeof(char),35U);});Cyc_Warn_err(_tmp86C,_tmp86B,_tag_dyneither(_tmp3F9,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp402=_cycalloc(sizeof(*_tmp402));({struct _dyneither_ptr*_tmp870=({struct _dyneither_ptr*_tmp401=_cycalloc(sizeof(*_tmp401));({struct _dyneither_ptr _tmp86F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp400=({struct Cyc_String_pa_PrintArg_struct _tmp703;_tmp703.tag=0U,_tmp703.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F4);_tmp703;});void*_tmp3FE[1U];_tmp3FE[0]=& _tmp400;({struct _dyneither_ptr _tmp86E=({const char*_tmp3FF="`%s";_tag_dyneither(_tmp3FF,sizeof(char),4U);});Cyc_aprintf(_tmp86E,_tag_dyneither(_tmp3FE,sizeof(void*),1U));});});*_tmp401=_tmp86F;});_tmp401;});_tmp402->name=_tmp870;}),_tmp402->identity=- 1,({
void*_tmp86D=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp402->kind=_tmp86D;});_tmp402;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp873=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp872=({struct _tuple0*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp871=({struct _dyneither_ptr*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));*_tmp3FC=_tmp3F4;_tmp3FC;});_tmp3FD->f2=_tmp871;});_tmp3FD;});Cyc_Absyn_new_vardecl(_tmp873,_tmp872,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));({struct Cyc_Absyn_Decl*_tmp874=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3FB->hd=_tmp874;}),_tmp3FB->tl=0;_tmp3FB;}));
# 1414
goto _LL0;};}case 44U: _LL53: _LL54: {
# 1416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1416 "parse.y"
struct _dyneither_ptr _tmp403=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp404=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp405=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({struct _dyneither_ptr _tmp875=(struct _dyneither_ptr)_tmp404;Cyc_strcmp(_tmp875,({const char*_tmp406="open";_tag_dyneither(_tmp406,sizeof(char),5U);}));})!= 0)({void*_tmp407=0U;({unsigned int _tmp877=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp876=({const char*_tmp408="expecting `open'";_tag_dyneither(_tmp408,sizeof(char),17U);});Cyc_Warn_err(_tmp877,_tmp876,_tag_dyneither(_tmp407,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp410=_cycalloc(sizeof(*_tmp410));({struct _dyneither_ptr*_tmp87B=({struct _dyneither_ptr*_tmp40F=_cycalloc(sizeof(*_tmp40F));({struct _dyneither_ptr _tmp87A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp40E=({struct Cyc_String_pa_PrintArg_struct _tmp704;_tmp704.tag=0U,_tmp704.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp403);_tmp704;});void*_tmp40C[1U];_tmp40C[0]=& _tmp40E;({struct _dyneither_ptr _tmp879=({const char*_tmp40D="`%s";_tag_dyneither(_tmp40D,sizeof(char),4U);});Cyc_aprintf(_tmp879,_tag_dyneither(_tmp40C,sizeof(void*),1U));});});*_tmp40F=_tmp87A;});_tmp40F;});_tmp410->name=_tmp87B;}),_tmp410->identity=- 1,({
void*_tmp878=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp410->kind=_tmp878;});_tmp410;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp87E=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp87D=({struct _tuple0*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp87C=({struct _dyneither_ptr*_tmp40A=_cycalloc(sizeof(*_tmp40A));*_tmp40A=_tmp403;_tmp40A;});_tmp40B->f2=_tmp87C;});_tmp40B;});Cyc_Absyn_new_vardecl(_tmp87E,_tmp87D,Cyc_Absyn_rgn_handle_type(t),0);});
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp409=_cycalloc(sizeof(*_tmp409));({struct Cyc_Absyn_Decl*_tmp87F=Cyc_Absyn_region_decl(tv,vd,_tmp405,(unsigned int)(yyyylsp[0]).first_line);_tmp409->hd=_tmp87F;}),_tmp409->tl=0;_tmp409;}));
# 1426
goto _LL0;};}case 45U: _LL55: _LL56: {
# 1428
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1430
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1430 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 46U: _LL57: _LL58: {
# 1433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1432 "parse.y"
yyval=Cyc_YY16(({struct Cyc_List_List*_tmp880=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp880,Cyc_yyget_YY16(yyyyvsp[1]));}));
goto _LL0;}case 47U: _LL59: _LL5A: {
# 1435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp705;({enum Cyc_Parse_Storage_class*_tmp883=Cyc_yyget_YY20(yyyyvsp[0]);_tmp705.sc=_tmp883;}),({struct Cyc_Absyn_Tqual _tmp882=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp705.tq=_tmp882;}),({
struct Cyc_Parse_Type_specifier _tmp881=Cyc_Parse_empty_spec(0U);_tmp705.type_specs=_tmp881;}),_tmp705.is_inline=0,_tmp705.attributes=0;_tmp705;}));
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1441 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp411=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp411.sc != 0)
({void*_tmp412=0U;({unsigned int _tmp885=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp884=({const char*_tmp413="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp413,sizeof(char),73U);});Cyc_Warn_warn(_tmp885,_tmp884,_tag_dyneither(_tmp412,sizeof(void*),0U));});});
# 1445
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp706;({enum Cyc_Parse_Storage_class*_tmp886=Cyc_yyget_YY20(yyyyvsp[0]);_tmp706.sc=_tmp886;}),_tmp706.tq=_tmp411.tq,_tmp706.type_specs=_tmp411.type_specs,_tmp706.is_inline=_tmp411.is_inline,_tmp706.attributes=_tmp411.attributes;_tmp706;}));
# 1449
goto _LL0;}case 49U: _LL5D: _LL5E: {
# 1451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1450 "parse.y"
({void*_tmp414=0U;({unsigned int _tmp888=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp887=({const char*_tmp415="__extension__ keyword ignored in declaration";_tag_dyneither(_tmp415,sizeof(char),45U);});Cyc_Warn_warn(_tmp888,_tmp887,_tag_dyneither(_tmp414,sizeof(void*),0U));});});
yyval=yyyyvsp[1];
# 1453
goto _LL0;}case 50U: _LL5F: _LL60: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1454 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp707;_tmp707.sc=0,({struct Cyc_Absyn_Tqual _tmp88A=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp707.tq=_tmp88A;}),({
struct Cyc_Parse_Type_specifier _tmp889=Cyc_yyget_YY21(yyyyvsp[0]);_tmp707.type_specs=_tmp889;}),_tmp707.is_inline=0,_tmp707.attributes=0;_tmp707;}));
goto _LL0;}case 51U: _LL61: _LL62: {
# 1458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1457 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp416=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp708;_tmp708.sc=_tmp416.sc,_tmp708.tq=_tmp416.tq,({
struct Cyc_Parse_Type_specifier _tmp88D=({unsigned int _tmp88C=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp88B=_tmp416.type_specs;Cyc_Parse_combine_specifiers(_tmp88C,_tmp88B,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp708.type_specs=_tmp88D;}),_tmp708.is_inline=_tmp416.is_inline,_tmp708.attributes=_tmp416.attributes;_tmp708;}));
# 1463
goto _LL0;}case 52U: _LL63: _LL64: {
# 1465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1464 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp709;_tmp709.sc=0,({struct Cyc_Absyn_Tqual _tmp88F=Cyc_yyget_YY23(yyyyvsp[0]);_tmp709.tq=_tmp88F;}),({struct Cyc_Parse_Type_specifier _tmp88E=Cyc_Parse_empty_spec(0U);_tmp709.type_specs=_tmp88E;}),_tmp709.is_inline=0,_tmp709.attributes=0;_tmp709;}));
goto _LL0;}case 53U: _LL65: _LL66: {
# 1467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1466 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp417=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp70A;_tmp70A.sc=_tmp417.sc,({struct Cyc_Absyn_Tqual _tmp891=({struct Cyc_Absyn_Tqual _tmp890=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp890,_tmp417.tq);});_tmp70A.tq=_tmp891;}),_tmp70A.type_specs=_tmp417.type_specs,_tmp70A.is_inline=_tmp417.is_inline,_tmp70A.attributes=_tmp417.attributes;_tmp70A;}));
# 1471
goto _LL0;}case 54U: _LL67: _LL68: {
# 1473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1472 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp70B;_tmp70B.sc=0,({struct Cyc_Absyn_Tqual _tmp893=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp70B.tq=_tmp893;}),({
struct Cyc_Parse_Type_specifier _tmp892=Cyc_Parse_empty_spec(0U);_tmp70B.type_specs=_tmp892;}),_tmp70B.is_inline=1,_tmp70B.attributes=0;_tmp70B;}));
goto _LL0;}case 55U: _LL69: _LL6A: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1475 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp418=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp70C;_tmp70C.sc=_tmp418.sc,_tmp70C.tq=_tmp418.tq,_tmp70C.type_specs=_tmp418.type_specs,_tmp70C.is_inline=1,_tmp70C.attributes=_tmp418.attributes;_tmp70C;}));
# 1479
goto _LL0;}case 56U: _LL6B: _LL6C: {
# 1481
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1483
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1480 "parse.y"
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp70D;_tmp70D.sc=0,({struct Cyc_Absyn_Tqual _tmp896=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp70D.tq=_tmp896;}),({
struct Cyc_Parse_Type_specifier _tmp895=Cyc_Parse_empty_spec(0U);_tmp70D.type_specs=_tmp895;}),_tmp70D.is_inline=0,({struct Cyc_List_List*_tmp894=Cyc_yyget_YY45(yyyyvsp[0]);_tmp70D.attributes=_tmp894;});_tmp70D;}));
goto _LL0;}case 57U: _LL6D: _LL6E: {
# 1484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1483 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp419=Cyc_yyget_YY17(yyyyvsp[1]);
yyval=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp70E;_tmp70E.sc=_tmp419.sc,_tmp70E.tq=_tmp419.tq,_tmp70E.type_specs=_tmp419.type_specs,_tmp70E.is_inline=_tmp419.is_inline,({
# 1486
struct Cyc_List_List*_tmp898=({struct Cyc_List_List*_tmp897=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp897,_tmp419.attributes);});_tmp70E.attributes=_tmp898;});_tmp70E;}));
goto _LL0;}case 58U: _LL6F: _LL70: {
# 1489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1490 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
yyval=Cyc_YY20(& auto_sc);
goto _LL0;}case 59U: _LL71: _LL72: {
# 1494
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1496
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1492 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
yyval=Cyc_YY20(& register_sc);
goto _LL0;}case 60U: _LL73: _LL74: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1494 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
yyval=Cyc_YY20(& static_sc);
goto _LL0;}case 61U: _LL75: _LL76: {
# 1498
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1496 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
yyval=Cyc_YY20(& extern_sc);
goto _LL0;}case 62U: _LL77: _LL78: {
# 1500
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1499 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _dyneither_ptr _tmp899=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmp899,({const char*_tmp41A="C";_tag_dyneither(_tmp41A,sizeof(char),2U);}));})!= 0)
({void*_tmp41B=0U;({unsigned int _tmp89B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp89A=({const char*_tmp41C="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp41C,sizeof(char),37U);});Cyc_Warn_err(_tmp89B,_tmp89A,_tag_dyneither(_tmp41B,sizeof(void*),0U));});});
yyval=Cyc_YY20(& externC_sc);
# 1504
goto _LL0;}case 63U: _LL79: _LL7A: {
# 1506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1504 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
yyval=Cyc_YY20(& typedef_sc);
goto _LL0;}case 64U: _LL7B: _LL7C: {
# 1508
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1507 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
yyval=Cyc_YY20(& abstract_sc);
goto _LL0;}case 65U: _LL7D: _LL7E:
# 1511
 yyval=Cyc_YY45(0);
goto _LL0;case 66U: _LL7F: _LL80: {
# 1514
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1516
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1514 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 67U: _LL81: _LL82: {
# 1517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1519 "parse.y"
yyval=yyyyvsp[3];
goto _LL0;}case 68U: _LL83: _LL84: {
# 1522
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1524
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1523 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp41D=_cycalloc(sizeof(*_tmp41D));({void*_tmp89C=Cyc_yyget_YY46(yyyyvsp[0]);_tmp41D->hd=_tmp89C;}),_tmp41D->tl=0;_tmp41D;}));
goto _LL0;}case 69U: _LL85: _LL86: {
# 1526
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1528
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1524 "parse.y"
yyval=Cyc_YY45(({struct Cyc_List_List*_tmp41E=_cycalloc(sizeof(*_tmp41E));({void*_tmp89E=Cyc_yyget_YY46(yyyyvsp[0]);_tmp41E->hd=_tmp89E;}),({struct Cyc_List_List*_tmp89D=Cyc_yyget_YY45(yyyyvsp[2]);_tmp41E->tl=_tmp89D;});_tmp41E;}));
goto _LL0;}case 70U: _LL87: _LL88: {
# 1527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1529 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp422,_tmp422,_tmp422 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp423,_tmp423,_tmp423 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp424,_tmp424,_tmp424 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp425,_tmp425,_tmp425 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp426,_tmp426,_tmp426 + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp427,_tmp427,_tmp427 + 8U},(void*)& att_aligned},{{_tmp428,_tmp428,_tmp428 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp429,_tmp429,_tmp429 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp42A,_tmp42A,_tmp42A + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp42B,_tmp42B,_tmp42B + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp42C,_tmp42C,_tmp42C + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp42D,_tmp42D,_tmp42D + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp42E,_tmp42E,_tmp42E + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp42F,_tmp42F,_tmp42F + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp430,_tmp430,_tmp430 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp431,_tmp431,_tmp431 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp432,_tmp432,_tmp432 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp433,_tmp433,_tmp433 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1550
struct _dyneither_ptr _tmp41F=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1552
if((((_get_dyneither_size(_tmp41F,sizeof(char))> 4  && *((const char*)_check_dyneither_subscript(_tmp41F,sizeof(char),0))== '_') && *((const char*)_check_dyneither_subscript(_tmp41F,sizeof(char),1))== '_') && *((const char*)_check_dyneither_subscript(_tmp41F,sizeof(char),(int)(_get_dyneither_size(_tmp41F,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp41F,sizeof(char),(int)(_get_dyneither_size(_tmp41F,sizeof(char))- 3)))== '_')
# 1554
_tmp41F=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp41F,2,_get_dyneither_size(_tmp41F,sizeof(char))- 5);{
int i=0;
for(0;i < 18U;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp41F,(struct _dyneither_ptr)(*((struct _tuple28*)_check_known_subscript_notnull(att_map,18U,sizeof(struct _tuple28),i))).f1)== 0){
yyval=Cyc_YY46((att_map[i]).f2);
break;}}
# 1561
if(i == 18U){
({void*_tmp420=0U;({unsigned int _tmp8A0=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp89F=({const char*_tmp421="unrecognized attribute";_tag_dyneither(_tmp421,sizeof(char),23U);});Cyc_Warn_err(_tmp8A0,_tmp89F,_tag_dyneither(_tmp420,sizeof(void*),0U));});});
yyval=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);}
# 1566
goto _LL0;};}case 71U: _LL89: _LL8A: {
# 1568
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1566 "parse.y"
yyval=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1569
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1571
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1568 "parse.y"
struct _dyneither_ptr _tmp434=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp435=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({struct _dyneither_ptr _tmp8A2=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A2,({const char*_tmp436="aligned";_tag_dyneither(_tmp436,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp8A1=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A1,({const char*_tmp437="__aligned__";_tag_dyneither(_tmp437,sizeof(char),12U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=6U,_tmp438->f1=_tmp435;_tmp438;});else{
if(({struct _dyneither_ptr _tmp8A4=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A4,({const char*_tmp439="section";_tag_dyneither(_tmp439,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp8A3=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A3,({const char*_tmp43A="__section__";_tag_dyneither(_tmp43A,sizeof(char),12U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp435);
a=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->tag=8U,_tmp43B->f1=str;_tmp43B;});}else{
if(({struct _dyneither_ptr _tmp8A5=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A5,({const char*_tmp43C="__mode__";_tag_dyneither(_tmp43C,sizeof(char),9U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp435);
a=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->tag=24U,_tmp43D->f1=str;_tmp43D;});}else{
if(({struct _dyneither_ptr _tmp8A6=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A6,({const char*_tmp43E="alias";_tag_dyneither(_tmp43E,sizeof(char),6U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp435);
a=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=25U,_tmp43F->f1=str;_tmp43F;});}else{
# 1584
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp435);
if(({struct _dyneither_ptr _tmp8A8=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A8,({const char*_tmp440="regparm";_tag_dyneither(_tmp440,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp8A7=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8A7,({const char*_tmp441="__regparm__";_tag_dyneither(_tmp441,sizeof(char),12U);}));})== 0){
if(n < 0  || n > 3)
({void*_tmp442=0U;({unsigned int _tmp8AA=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8A9=({const char*_tmp443="regparm requires value between 0 and 3";_tag_dyneither(_tmp443,sizeof(char),39U);});Cyc_Warn_err(_tmp8AA,_tmp8A9,_tag_dyneither(_tmp442,sizeof(void*),0U));});});
a=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->tag=0U,_tmp444->f1=n;_tmp444;});}else{
if(({struct _dyneither_ptr _tmp8AC=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8AC,({const char*_tmp445="initializes";_tag_dyneither(_tmp445,sizeof(char),12U);}));})== 0  || ({struct _dyneither_ptr _tmp8AB=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8AB,({const char*_tmp446="__initializes__";_tag_dyneither(_tmp446,sizeof(char),16U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=20U,_tmp447->f1=n;_tmp447;});else{
if(({struct _dyneither_ptr _tmp8AE=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8AE,({const char*_tmp448="noliveunique";_tag_dyneither(_tmp448,sizeof(char),13U);}));})== 0  || ({struct _dyneither_ptr _tmp8AD=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8AD,({const char*_tmp449="__noliveunique__";_tag_dyneither(_tmp449,sizeof(char),17U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));_tmp44A->tag=21U,_tmp44A->f1=n;_tmp44A;});else{
if(({struct _dyneither_ptr _tmp8B0=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8B0,({const char*_tmp44B="noconsume";_tag_dyneither(_tmp44B,sizeof(char),10U);}));})== 0  || ({struct _dyneither_ptr _tmp8AF=(struct _dyneither_ptr)_tmp434;Cyc_zstrcmp(_tmp8AF,({const char*_tmp44C="__noconsume__";_tag_dyneither(_tmp44C,sizeof(char),14U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->tag=22U,_tmp44D->f1=n;_tmp44D;});else{
# 1596
({void*_tmp44E=0U;({unsigned int _tmp8B2=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8B1=({const char*_tmp44F="unrecognized attribute";_tag_dyneither(_tmp44F,sizeof(char),23U);});Cyc_Warn_err(_tmp8B2,_tmp8B1,_tag_dyneither(_tmp44E,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1600
yyval=Cyc_YY46(a);
# 1602
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1603 "parse.y"
struct _dyneither_ptr _tmp450=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp451=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp452=({unsigned int _tmp8B3=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmp8B3,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp453=({unsigned int _tmp8B4=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmp8B4,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _dyneither_ptr _tmp8B6=(struct _dyneither_ptr)_tmp450;Cyc_zstrcmp(_tmp8B6,({const char*_tmp454="format";_tag_dyneither(_tmp454,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp8B5=(struct _dyneither_ptr)_tmp450;Cyc_zstrcmp(_tmp8B5,({const char*_tmp455="__format__";_tag_dyneither(_tmp455,sizeof(char),11U);}));})== 0){
if(({struct _dyneither_ptr _tmp8B8=(struct _dyneither_ptr)_tmp451;Cyc_zstrcmp(_tmp8B8,({const char*_tmp456="printf";_tag_dyneither(_tmp456,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp8B7=(struct _dyneither_ptr)_tmp451;Cyc_zstrcmp(_tmp8B7,({const char*_tmp457="__printf__";_tag_dyneither(_tmp457,sizeof(char),11U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->tag=19U,_tmp458->f1=Cyc_Absyn_Printf_ft,_tmp458->f2=(int)_tmp452,_tmp458->f3=(int)_tmp453;_tmp458;});else{
if(({struct _dyneither_ptr _tmp8BA=(struct _dyneither_ptr)_tmp451;Cyc_zstrcmp(_tmp8BA,({const char*_tmp459="scanf";_tag_dyneither(_tmp459,sizeof(char),6U);}));})== 0  || ({struct _dyneither_ptr _tmp8B9=(struct _dyneither_ptr)_tmp451;Cyc_zstrcmp(_tmp8B9,({const char*_tmp45A="__scanf__";_tag_dyneither(_tmp45A,sizeof(char),10U);}));})== 0)
a=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->tag=19U,_tmp45B->f1=Cyc_Absyn_Scanf_ft,_tmp45B->f2=(int)_tmp452,_tmp45B->f3=(int)_tmp453;_tmp45B;});else{
# 1614
({void*_tmp45C=0U;({unsigned int _tmp8BC=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8BB=({const char*_tmp45D="unrecognized format type";_tag_dyneither(_tmp45D,sizeof(char),25U);});Cyc_Warn_err(_tmp8BC,_tmp8BB,_tag_dyneither(_tmp45C,sizeof(void*),0U));});});}}}else{
# 1616
({void*_tmp45E=0U;({unsigned int _tmp8BE=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8BD=({const char*_tmp45F="unrecognized attribute";_tag_dyneither(_tmp45F,sizeof(char),23U);});Cyc_Warn_err(_tmp8BE,_tmp8BD,_tag_dyneither(_tmp45E,sizeof(void*),0U));});});}
yyval=Cyc_YY46(a);
# 1619
goto _LL0;}case 74U: _LL8F: _LL90: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1628 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 75U: _LL91: _LL92: {
# 1631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1630 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C0=({struct _tuple0*_tmp8BF=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_typedef_type(_tmp8BF,Cyc_yyget_YY40(yyyyvsp[1]),0,0);});Cyc_Parse_type_spec(_tmp8C0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 76U: _LL93: _LL94: {
# 1633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1634 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 77U: _LL95: _LL96: {
# 1637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1635 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 78U: _LL97: _LL98: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1636 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 79U: _LL99: _LL9A: {
# 1639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1637 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 80U: _LL9B: _LL9C: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1638 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 81U: _LL9D: _LL9E: {
# 1641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1639 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1640 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1641 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 84U: _LLA3: _LLA4: {
# 1644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1642 "parse.y"
yyval=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 85U: _LLA5: _LLA6: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1643 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 86U: _LLA7: _LLA8: {
# 1646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1644 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 87U: _LLA9: _LLAA: {
# 1647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1647 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C1=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8C1,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 88U: _LLAB: _LLAC: {
# 1650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1649 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C2=Cyc_Absyn_builtin_type(({const char*_tmp460="__builtin_va_list";_tag_dyneither(_tmp460,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp8C2,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 89U: _LLAD: _LLAE: {
# 1652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1651 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 90U: _LLAF: _LLB0: {
# 1654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1653 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C3=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmp8C3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 91U: _LLB1: _LLB2: {
# 1656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1655 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C4=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp8C4,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 92U: _LLB3: _LLB4: {
# 1658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1657 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C5=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);Cyc_Parse_type_spec(_tmp8C5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 93U: _LLB5: _LLB6: {
# 1660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1659 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C8=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->tag=6U,({struct Cyc_List_List*_tmp8C7=({unsigned int _tmp8C6=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp8C6,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});
# 1659
_tmp461->f1=_tmp8C7;});_tmp461;});Cyc_Parse_type_spec(_tmp8C8,(unsigned int)(yyyylsp[0]).first_line);}));
# 1662
goto _LL0;}case 94U: _LLB7: _LLB8: {
# 1664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1663 "parse.y"
yyval=Cyc_YY21(({void*_tmp8C9=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8C9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 95U: _LLB9: _LLBA: {
# 1666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1665 "parse.y"
yyval=Cyc_YY21(({void*_tmp8CA=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp8CA,(unsigned int)(yyyylsp[0]).first_line);}));
# 1667
goto _LL0;}case 96U: _LLBB: _LLBC: {
# 1669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1668 "parse.y"
yyval=Cyc_YY21(({void*_tmp8CB=Cyc_Absyn_tag_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8CB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 97U: _LLBD: _LLBE: {
# 1671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1670 "parse.y"
yyval=Cyc_YY21(({void*_tmp8CC=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp8CC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 98U: _LLBF: _LLC0: {
# 1673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1672 "parse.y"
yyval=Cyc_YY21(({void*_tmp8CD=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8CD,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 99U: _LLC1: _LLC2: {
# 1675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY43(({struct _dyneither_ptr _tmp8CE=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmp8CE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 100U: _LLC3: _LLC4: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1682 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0U);
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp70F;_tmp70F.print_const=1,_tmp70F.q_volatile=0,_tmp70F.q_restrict=0,_tmp70F.real_const=1,_tmp70F.loc=loc;_tmp70F;}));
goto _LL0;}case 101U: _LLC5: _LLC6: {
# 1686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1684 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp710;_tmp710.print_const=0,_tmp710.q_volatile=1,_tmp710.q_restrict=0,_tmp710.real_const=0,_tmp710.loc=0U;_tmp710;}));
goto _LL0;}case 102U: _LLC7: _LLC8: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1685 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp711;_tmp711.print_const=0,_tmp711.q_volatile=0,_tmp711.q_restrict=1,_tmp711.real_const=0,_tmp711.loc=0U;_tmp711;}));
goto _LL0;}case 103U: _LLC9: _LLCA: {
# 1688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
struct Cyc_Absyn_TypeDecl*_tmp462=({struct Cyc_Absyn_TypeDecl*_tmp467=_cycalloc(sizeof(*_tmp467));({void*_tmp8D3=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp8D2=({struct Cyc_Absyn_Enumdecl*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp8D1=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp465->name=_tmp8D1;}),({struct Cyc_Core_Opt*_tmp8D0=({struct Cyc_Core_Opt*_tmp464=_cycalloc(sizeof(*_tmp464));({struct Cyc_List_List*_tmp8CF=Cyc_yyget_YY48(yyyyvsp[3]);_tmp464->v=_tmp8CF;});_tmp464;});_tmp465->fields=_tmp8D0;});_tmp465;});_tmp466->f1=_tmp8D2;});_tmp466;});_tmp467->r=_tmp8D3;}),_tmp467->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp467;});
# 1693
yyval=Cyc_YY21(({void*_tmp8D4=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp463=_cycalloc(sizeof(*_tmp463));_tmp463->tag=10U,_tmp463->f1=_tmp462,_tmp463->f2=0;_tmp463;});Cyc_Parse_type_spec(_tmp8D4,(unsigned int)(yyyylsp[0]).first_line);}));
# 1695
goto _LL0;}case 104U: _LLCB: _LLCC: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1696 "parse.y"
yyval=Cyc_YY21(({void*_tmp8D5=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(yyyyvsp[1]),0);Cyc_Parse_type_spec(_tmp8D5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 105U: _LLCD: _LLCE: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1698 "parse.y"
yyval=Cyc_YY21(({void*_tmp8D6=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp8D6,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 106U: _LLCF: _LLD0: {
# 1701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp468=_cycalloc(sizeof(*_tmp468));({struct _tuple0*_tmp8D7=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp468->name=_tmp8D7;}),_tmp468->tag=0,_tmp468->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp468;}));
goto _LL0;}case 107U: _LLD1: _LLD2: {
# 1707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1706 "parse.y"
yyval=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp469=_cycalloc(sizeof(*_tmp469));({struct _tuple0*_tmp8D9=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp469->name=_tmp8D9;}),({struct Cyc_Absyn_Exp*_tmp8D8=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp469->tag=_tmp8D8;}),_tmp469->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp469;}));
goto _LL0;}case 108U: _LLD3: _LLD4: {
# 1709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1710 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46A=_cycalloc(sizeof(*_tmp46A));({struct Cyc_Absyn_Enumfield*_tmp8DA=Cyc_yyget_YY47(yyyyvsp[0]);_tmp46A->hd=_tmp8DA;}),_tmp46A->tl=0;_tmp46A;}));
goto _LL0;}case 109U: _LLD5: _LLD6: {
# 1713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1711 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46B=_cycalloc(sizeof(*_tmp46B));({struct Cyc_Absyn_Enumfield*_tmp8DB=Cyc_yyget_YY47(yyyyvsp[0]);_tmp46B->hd=_tmp8DB;}),_tmp46B->tl=0;_tmp46B;}));
goto _LL0;}case 110U: _LLD7: _LLD8: {
# 1714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1712 "parse.y"
yyval=Cyc_YY48(({struct Cyc_List_List*_tmp46C=_cycalloc(sizeof(*_tmp46C));({struct Cyc_Absyn_Enumfield*_tmp8DD=Cyc_yyget_YY47(yyyyvsp[0]);_tmp46C->hd=_tmp8DD;}),({struct Cyc_List_List*_tmp8DC=Cyc_yyget_YY48(yyyyvsp[2]);_tmp46C->tl=_tmp8DC;});_tmp46C;}));
goto _LL0;}case 111U: _LLD9: _LLDA: {
# 1715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
yyval=Cyc_YY21(({void*_tmp8E0=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->tag=7U,({enum Cyc_Absyn_AggrKind _tmp8DF=Cyc_yyget_YY22(yyyyvsp[0]);_tmp46D->f1=_tmp8DF;}),({struct Cyc_List_List*_tmp8DE=Cyc_yyget_YY24(yyyyvsp[2]);_tmp46D->f2=_tmp8DE;});_tmp46D;});Cyc_Parse_type_spec(_tmp8E0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 112U: _LLDB: _LLDC: {
# 1721
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 1723
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 1724 "parse.y"
struct Cyc_List_List*_tmp46E=({unsigned int _tmp8E1=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8E1,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp46F=({unsigned int _tmp8E2=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8E2,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp470=({enum Cyc_Absyn_AggrKind _tmp8E8=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp8E7=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmp8E6=_tmp46E;struct Cyc_Absyn_AggrdeclImpl*_tmp8E5=({
struct Cyc_List_List*_tmp8E4=_tmp46F;struct Cyc_List_List*_tmp8E3=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmp8E4,_tmp8E3,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1726
Cyc_Absyn_aggr_tdecl(_tmp8E8,Cyc_Absyn_Public,_tmp8E7,_tmp8E6,_tmp8E5,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1729
yyval=Cyc_YY21(({void*_tmp8E9=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=10U,_tmp471->f1=_tmp470,_tmp471->f2=0;_tmp471;});Cyc_Parse_type_spec(_tmp8E9,(unsigned int)(yyyylsp[0]).first_line);}));
# 1731
goto _LL0;}case 113U: _LLDD: _LLDE: {
# 1733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1735 "parse.y"
struct Cyc_List_List*_tmp472=({unsigned int _tmp8EA=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8EA,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp473=({unsigned int _tmp8EB=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp8EB,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp474=({enum Cyc_Absyn_AggrKind _tmp8F1=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmp8F0=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp8EF=_tmp472;struct Cyc_Absyn_AggrdeclImpl*_tmp8EE=({
struct Cyc_List_List*_tmp8ED=_tmp473;struct Cyc_List_List*_tmp8EC=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmp8ED,_tmp8EC,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1737
Cyc_Absyn_aggr_tdecl(_tmp8F1,Cyc_Absyn_Public,_tmp8F0,_tmp8EF,_tmp8EE,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1740
yyval=Cyc_YY21(({void*_tmp8F2=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->tag=10U,_tmp475->f1=_tmp474,_tmp475->f2=0;_tmp475;});Cyc_Parse_type_spec(_tmp8F2,(unsigned int)(yyyylsp[0]).first_line);}));
# 1742
goto _LL0;}case 114U: _LLDF: _LLE0: {
# 1744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1743 "parse.y"
yyval=Cyc_YY21(({void*_tmp8F6=({union Cyc_Absyn_AggrInfo _tmp8F5=({enum Cyc_Absyn_AggrKind _tmp8F4=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp8F3=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmp8F4,_tmp8F3,({struct Cyc_Core_Opt*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->v=(void*)1;_tmp476;}));});Cyc_Absyn_aggr_type(_tmp8F5,Cyc_yyget_YY40(yyyyvsp[3]));});Cyc_Parse_type_spec(_tmp8F6,(unsigned int)(yyyylsp[0]).first_line);}));
# 1746
goto _LL0;}case 115U: _LLE1: _LLE2: {
# 1748
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1750
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1747 "parse.y"
yyval=Cyc_YY21(({void*_tmp8F9=({union Cyc_Absyn_AggrInfo _tmp8F8=({enum Cyc_Absyn_AggrKind _tmp8F7=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmp8F7,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});Cyc_Absyn_aggr_type(_tmp8F8,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp8F9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 116U: _LLE3: _LLE4:
# 1750
 yyval=Cyc_YY40(0);
goto _LL0;case 117U: _LLE5: _LLE6: {
# 1753
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1755
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1754 "parse.y"
yyval=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 118U: _LLE7: _LLE8: {
# 1757
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1758 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_StructA);
goto _LL0;}case 119U: _LLE9: _LLEA: {
# 1761
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1759 "parse.y"
yyval=Cyc_YY22(Cyc_Absyn_UnionA);
goto _LL0;}case 120U: _LLEB: _LLEC:
# 1762
 yyval=Cyc_YY24(0);
goto _LL0;case 121U: _LLED: _LLEE: {
# 1765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1766 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp477=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp477 != 0;_tmp477=_tmp477->tl){
decls=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp477->hd,decls);}}{
# 1770
struct Cyc_List_List*_tmp478=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp478 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp478,decls);
yyval=Cyc_YY24(decls);
# 1775
goto _LL0;};}case 122U: _LLEF: _LLF0: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp479=_cycalloc(sizeof(*_tmp479));({struct Cyc_List_List*_tmp8FA=Cyc_yyget_YY24(yyyyvsp[0]);_tmp479->hd=_tmp8FA;}),_tmp479->tl=0;_tmp479;}));
goto _LL0;}case 123U: _LLF1: _LLF2: {
# 1783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1782 "parse.y"
yyval=Cyc_YY25(({struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));({struct Cyc_List_List*_tmp8FC=Cyc_yyget_YY24(yyyyvsp[1]);_tmp47A->hd=_tmp8FC;}),({struct Cyc_List_List*_tmp8FB=Cyc_yyget_YY25(yyyyvsp[0]);_tmp47A->tl=_tmp8FB;});_tmp47A;}));
goto _LL0;}case 124U: _LLF3: _LLF4: {
# 1785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1786 "parse.y"
yyval=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));
goto _LL0;}case 125U: _LLF5: _LLF6: {
# 1789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY19(({struct _tuple12*_tmp47B=_region_malloc(yyr,sizeof(*_tmp47B));_tmp47B->tl=0,({struct _tuple11 _tmp8FD=Cyc_yyget_YY18(yyyyvsp[0]);_tmp47B->hd=_tmp8FD;});_tmp47B;}));
goto _LL0;}case 126U: _LLF7: _LLF8: {
# 1795
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1797
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1794 "parse.y"
yyval=Cyc_YY19(({struct _tuple12*_tmp47C=_region_malloc(yyr,sizeof(*_tmp47C));({struct _tuple12*_tmp8FF=Cyc_yyget_YY19(yyyyvsp[0]);_tmp47C->tl=_tmp8FF;}),({struct _tuple11 _tmp8FE=Cyc_yyget_YY18(yyyyvsp[2]);_tmp47C->hd=_tmp8FE;});_tmp47C;}));
goto _LL0;}case 127U: _LLF9: _LLFA: {
# 1797
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1799 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp712;({struct Cyc_Parse_Declarator _tmp900=Cyc_yyget_YY27(yyyyvsp[0]);_tmp712.f1=_tmp900;}),_tmp712.f2=0;_tmp712;}));
goto _LL0;}case 128U: _LLFB: _LLFC: {
# 1802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1801 "parse.y"
yyval=Cyc_YY18(({struct _tuple11 _tmp713;({struct Cyc_Parse_Declarator _tmp902=Cyc_yyget_YY27(yyyyvsp[0]);_tmp713.f1=_tmp902;}),({struct Cyc_Absyn_Exp*_tmp901=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp713.f2=_tmp901;});_tmp713;}));
goto _LL0;}case 129U: _LLFD: _LLFE: {
# 1804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1807 "parse.y"
struct _RegionHandle _tmp47D=_new_region("temp");struct _RegionHandle*temp=& _tmp47D;_push_region(temp);
{struct _tuple25 _tmp47E=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp47F=_tmp47E;struct Cyc_Absyn_Tqual _tmp48D;struct Cyc_Parse_Type_specifier _tmp48C;struct Cyc_List_List*_tmp48B;_LL441: _tmp48D=_tmp47F.f1;_tmp48C=_tmp47F.f2;_tmp48B=_tmp47F.f3;_LL442:;
if(_tmp48D.loc == 0)_tmp48D.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp480=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp480 != 0;_tmp480=_tmp480->tl){
struct _tuple24*_tmp481=(struct _tuple24*)_tmp480->hd;struct _tuple24*_tmp482=_tmp481;struct Cyc_Parse_Declarator _tmp488;struct Cyc_Absyn_Exp*_tmp487;struct Cyc_Absyn_Exp*_tmp486;_LL444: _tmp488=_tmp482->f1;_tmp487=_tmp482->f2;_tmp486=_tmp482->f3;_LL445:;
decls=({struct _tuple10*_tmp483=_region_malloc(temp,sizeof(*_tmp483));_tmp483->tl=decls,_tmp483->hd=_tmp488;_tmp483;});
widths_and_reqs=({struct Cyc_List_List*_tmp485=_region_malloc(temp,sizeof(*_tmp485));
({struct _tuple16*_tmp903=({struct _tuple16*_tmp484=_region_malloc(temp,sizeof(*_tmp484));_tmp484->f1=_tmp487,_tmp484->f2=_tmp486;_tmp484;});_tmp485->hd=_tmp903;}),_tmp485->tl=widths_and_reqs;_tmp485;});}}
# 1818
decls=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);
widths_and_reqs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);{
void*_tmp489=Cyc_Parse_speclist2typ(_tmp48C,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp48A=({struct _RegionHandle*_tmp906=temp;struct _RegionHandle*_tmp905=temp;struct Cyc_List_List*_tmp904=
Cyc_Parse_apply_tmss(temp,_tmp48D,_tmp489,decls,_tmp48B);
# 1821
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp906,_tmp905,_tmp904,widths_and_reqs);});
# 1824
yyval=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp48A));
# 1826
_npop_handler(0U);goto _LL0;};};}
# 1808
;_pop_region(temp);}case 130U: _LLFF: _LL100: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1834 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp714;({struct Cyc_Absyn_Tqual _tmp908=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp714.f1=_tmp908;}),({struct Cyc_Parse_Type_specifier _tmp907=Cyc_yyget_YY21(yyyyvsp[0]);_tmp714.f2=_tmp907;}),_tmp714.f3=0;_tmp714;}));
goto _LL0;}case 131U: _LL101: _LL102: {
# 1837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1836 "parse.y"
struct _tuple25 _tmp48E=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp715;_tmp715.f1=_tmp48E.f1,({struct Cyc_Parse_Type_specifier _tmp90B=({unsigned int _tmp90A=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp909=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp90A,_tmp909,_tmp48E.f2);});_tmp715.f2=_tmp90B;}),_tmp715.f3=_tmp48E.f3;_tmp715;}));
goto _LL0;}case 132U: _LL103: _LL104: {
# 1839
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1841
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1838 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp716;({struct Cyc_Absyn_Tqual _tmp90D=Cyc_yyget_YY23(yyyyvsp[0]);_tmp716.f1=_tmp90D;}),({struct Cyc_Parse_Type_specifier _tmp90C=Cyc_Parse_empty_spec(0U);_tmp716.f2=_tmp90C;}),_tmp716.f3=0;_tmp716;}));
goto _LL0;}case 133U: _LL105: _LL106: {
# 1841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1840 "parse.y"
struct _tuple25 _tmp48F=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp717;({struct Cyc_Absyn_Tqual _tmp90F=({struct Cyc_Absyn_Tqual _tmp90E=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp90E,_tmp48F.f1);});_tmp717.f1=_tmp90F;}),_tmp717.f2=_tmp48F.f2,_tmp717.f3=_tmp48F.f3;_tmp717;}));
goto _LL0;}case 134U: _LL107: _LL108: {
# 1844
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1843 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp718;({struct Cyc_Absyn_Tqual _tmp912=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp718.f1=_tmp912;}),({struct Cyc_Parse_Type_specifier _tmp911=Cyc_Parse_empty_spec(0U);_tmp718.f2=_tmp911;}),({struct Cyc_List_List*_tmp910=Cyc_yyget_YY45(yyyyvsp[0]);_tmp718.f3=_tmp910;});_tmp718;}));
goto _LL0;}case 135U: _LL109: _LL10A: {
# 1846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1845 "parse.y"
struct _tuple25 _tmp490=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp719;_tmp719.f1=_tmp490.f1,_tmp719.f2=_tmp490.f2,({struct Cyc_List_List*_tmp914=({struct Cyc_List_List*_tmp913=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp913,_tmp490.f3);});_tmp719.f3=_tmp914;});_tmp719;}));
goto _LL0;}case 136U: _LL10B: _LL10C: {
# 1848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY35(({struct _tuple25 _tmp71A;({struct Cyc_Absyn_Tqual _tmp916=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp71A.f1=_tmp916;}),({struct Cyc_Parse_Type_specifier _tmp915=Cyc_yyget_YY21(yyyyvsp[0]);_tmp71A.f2=_tmp915;}),_tmp71A.f3=0;_tmp71A;}));
goto _LL0;}case 137U: _LL10D: _LL10E: {
# 1854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1853 "parse.y"
struct _tuple25 _tmp491=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp71B;_tmp71B.f1=_tmp491.f1,({struct Cyc_Parse_Type_specifier _tmp919=({unsigned int _tmp918=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp917=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp918,_tmp917,_tmp491.f2);});_tmp71B.f2=_tmp919;}),_tmp71B.f3=_tmp491.f3;_tmp71B;}));
goto _LL0;}case 138U: _LL10F: _LL110: {
# 1856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1855 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp71C;({struct Cyc_Absyn_Tqual _tmp91B=Cyc_yyget_YY23(yyyyvsp[0]);_tmp71C.f1=_tmp91B;}),({struct Cyc_Parse_Type_specifier _tmp91A=Cyc_Parse_empty_spec(0U);_tmp71C.f2=_tmp91A;}),_tmp71C.f3=0;_tmp71C;}));
goto _LL0;}case 139U: _LL111: _LL112: {
# 1858
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1860
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1857 "parse.y"
struct _tuple25 _tmp492=Cyc_yyget_YY35(yyyyvsp[1]);
yyval=Cyc_YY35(({struct _tuple25 _tmp71D;({struct Cyc_Absyn_Tqual _tmp91D=({struct Cyc_Absyn_Tqual _tmp91C=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp91C,_tmp492.f1);});_tmp71D.f1=_tmp91D;}),_tmp71D.f2=_tmp492.f2,_tmp71D.f3=_tmp492.f3;_tmp71D;}));
goto _LL0;}case 140U: _LL113: _LL114: {
# 1861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1860 "parse.y"
yyval=Cyc_YY35(({struct _tuple25 _tmp71E;({struct Cyc_Absyn_Tqual _tmp920=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp71E.f1=_tmp920;}),({struct Cyc_Parse_Type_specifier _tmp91F=Cyc_Parse_empty_spec(0U);_tmp71E.f2=_tmp91F;}),({struct Cyc_List_List*_tmp91E=Cyc_yyget_YY45(yyyyvsp[0]);_tmp71E.f3=_tmp91E;});_tmp71E;}));
goto _LL0;}case 141U: _LL115: _LL116: {
# 1863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1862 "parse.y"
struct _tuple25 _tmp493=Cyc_yyget_YY35(yyyyvsp[1]);yyval=Cyc_YY35(({struct _tuple25 _tmp71F;_tmp71F.f1=_tmp493.f1,_tmp71F.f2=_tmp493.f2,({struct Cyc_List_List*_tmp922=({struct Cyc_List_List*_tmp921=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp921,_tmp493.f3);});_tmp71F.f3=_tmp922;});_tmp71F;}));
goto _LL0;}case 142U: _LL117: _LL118: {
# 1865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1866 "parse.y"
yyval=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));
goto _LL0;}case 143U: _LL119: _LL11A: {
# 1869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp494=_region_malloc(yyr,sizeof(*_tmp494));({struct _tuple24*_tmp923=Cyc_yyget_YY28(yyyyvsp[0]);_tmp494->hd=_tmp923;}),_tmp494->tl=0;_tmp494;}));
goto _LL0;}case 144U: _LL11B: _LL11C: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1874 "parse.y"
yyval=Cyc_YY29(({struct Cyc_List_List*_tmp495=_region_malloc(yyr,sizeof(*_tmp495));({struct _tuple24*_tmp925=Cyc_yyget_YY28(yyyyvsp[2]);_tmp495->hd=_tmp925;}),({struct Cyc_List_List*_tmp924=Cyc_yyget_YY29(yyyyvsp[0]);_tmp495->tl=_tmp924;});_tmp495;}));
goto _LL0;}case 145U: _LL11D: _LL11E: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1879 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp496=_region_malloc(yyr,sizeof(*_tmp496));({struct Cyc_Parse_Declarator _tmp927=Cyc_yyget_YY27(yyyyvsp[0]);_tmp496->f1=_tmp927;}),_tmp496->f2=0,({struct Cyc_Absyn_Exp*_tmp926=Cyc_yyget_YY55(yyyyvsp[1]);_tmp496->f3=_tmp926;});_tmp496;}));
goto _LL0;}case 146U: _LL11F: _LL120: {
# 1882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1883 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp49A=_region_malloc(yyr,sizeof(*_tmp49A));({struct _tuple0*_tmp92C=({struct _tuple0*_tmp499=_cycalloc(sizeof(*_tmp499));({union Cyc_Absyn_Nmspace _tmp92B=Cyc_Absyn_Rel_n(0);_tmp499->f1=_tmp92B;}),({struct _dyneither_ptr*_tmp92A=({struct _dyneither_ptr*_tmp498=_cycalloc(sizeof(*_tmp498));({struct _dyneither_ptr _tmp929=({const char*_tmp497="";_tag_dyneither(_tmp497,sizeof(char),1U);});*_tmp498=_tmp929;});_tmp498;});_tmp499->f2=_tmp92A;});_tmp499;});(_tmp49A->f1).id=_tmp92C;}),(_tmp49A->f1).varloc=0U,(_tmp49A->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp928=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp49A->f2=_tmp928;}),_tmp49A->f3=0;_tmp49A;}));
# 1885
goto _LL0;}case 147U: _LL121: _LL122:
# 1889
 yyval=Cyc_YY28(({struct _tuple24*_tmp49E=_region_malloc(yyr,sizeof(*_tmp49E));({struct _tuple0*_tmp930=({struct _tuple0*_tmp49D=_cycalloc(sizeof(*_tmp49D));({union Cyc_Absyn_Nmspace _tmp92F=Cyc_Absyn_Rel_n(0);_tmp49D->f1=_tmp92F;}),({struct _dyneither_ptr*_tmp92E=({struct _dyneither_ptr*_tmp49C=_cycalloc(sizeof(*_tmp49C));({struct _dyneither_ptr _tmp92D=({const char*_tmp49B="";_tag_dyneither(_tmp49B,sizeof(char),1U);});*_tmp49C=_tmp92D;});_tmp49C;});_tmp49D->f2=_tmp92E;});_tmp49D;});(_tmp49E->f1).id=_tmp930;}),(_tmp49E->f1).varloc=0U,(_tmp49E->f1).tms=0,_tmp49E->f2=0,_tmp49E->f3=0;_tmp49E;}));
# 1891
goto _LL0;case 148U: _LL123: _LL124: {
# 1893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1891 "parse.y"
yyval=Cyc_YY28(({struct _tuple24*_tmp49F=_region_malloc(yyr,sizeof(*_tmp49F));({struct Cyc_Parse_Declarator _tmp932=Cyc_yyget_YY27(yyyyvsp[0]);_tmp49F->f1=_tmp932;}),({struct Cyc_Absyn_Exp*_tmp931=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp49F->f2=_tmp931;}),_tmp49F->f3=0;_tmp49F;}));
goto _LL0;}case 149U: _LL125: _LL126: {
# 1894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1895 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL0;}case 150U: _LL127: _LL128:
# 1898
 yyval=Cyc_YY55(0);
goto _LL0;case 151U: _LL129: _LL12A: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1900 "parse.y"
yyval=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));
goto _LL0;}case 152U: _LL12B: _LL12C:
# 1903
 yyval=Cyc_YY55(0);
goto _LL0;case 153U: _LL12D: _LL12E: {
# 1906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1907 "parse.y"
int _tmp4A0=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp4A1=({unsigned int _tmp933=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp933,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp4A2=({struct _tuple0*_tmp938=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp937=_tmp4A1;struct Cyc_Core_Opt*_tmp936=({struct Cyc_Core_Opt*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));({struct Cyc_List_List*_tmp934=Cyc_yyget_YY34(yyyyvsp[4]);_tmp4A4->v=_tmp934;});_tmp4A4;});int _tmp935=_tmp4A0;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp938,_tmp937,_tmp936,_tmp935,(unsigned int)(yyyylsp[0]).first_line);});
# 1911
yyval=Cyc_YY21(({void*_tmp939=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->tag=10U,_tmp4A3->f1=_tmp4A2,_tmp4A3->f2=0;_tmp4A3;});Cyc_Parse_type_spec(_tmp939,(unsigned int)(yyyylsp[0]).first_line);}));
# 1913
goto _LL0;}case 154U: _LL12F: _LL130: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1914 "parse.y"
int _tmp4A5=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(({void*_tmp93C=({union Cyc_Absyn_DatatypeInfo _tmp93B=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp720;({struct _tuple0*_tmp93A=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp720.name=_tmp93A;}),_tmp720.is_extensible=_tmp4A5;_tmp720;}));Cyc_Absyn_datatype_type(_tmp93B,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp93C,(unsigned int)(yyyylsp[0]).first_line);}));
# 1917
goto _LL0;}case 155U: _LL131: _LL132: {
# 1919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1918 "parse.y"
int _tmp4A6=Cyc_yyget_YY31(yyyyvsp[0]);
yyval=Cyc_YY21(({void*_tmp940=({union Cyc_Absyn_DatatypeFieldInfo _tmp93F=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp721;({struct _tuple0*_tmp93E=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp721.datatype_name=_tmp93E;}),({struct _tuple0*_tmp93D=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp721.field_name=_tmp93D;}),_tmp721.is_extensible=_tmp4A6;_tmp721;}));Cyc_Absyn_datatype_field_type(_tmp93F,Cyc_yyget_YY40(yyyyvsp[4]));});Cyc_Parse_type_spec(_tmp940,(unsigned int)(yyyylsp[0]).first_line);}));
# 1921
goto _LL0;}case 156U: _LL133: _LL134: {
# 1923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1924 "parse.y"
yyval=Cyc_YY31(0);
goto _LL0;}case 157U: _LL135: _LL136: {
# 1927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1925 "parse.y"
yyval=Cyc_YY31(1);
goto _LL0;}case 158U: _LL137: _LL138: {
# 1928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1929 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));({struct Cyc_Absyn_Datatypefield*_tmp941=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4A7->hd=_tmp941;}),_tmp4A7->tl=0;_tmp4A7;}));
goto _LL0;}case 159U: _LL139: _LL13A: {
# 1932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1930 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));({struct Cyc_Absyn_Datatypefield*_tmp942=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4A8->hd=_tmp942;}),_tmp4A8->tl=0;_tmp4A8;}));
goto _LL0;}case 160U: _LL13B: _LL13C: {
# 1933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1931 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));({struct Cyc_Absyn_Datatypefield*_tmp944=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4A9->hd=_tmp944;}),({struct Cyc_List_List*_tmp943=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4A9->tl=_tmp943;});_tmp4A9;}));
goto _LL0;}case 161U: _LL13D: _LL13E: {
# 1934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1932 "parse.y"
yyval=Cyc_YY34(({struct Cyc_List_List*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));({struct Cyc_Absyn_Datatypefield*_tmp946=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4AA->hd=_tmp946;}),({struct Cyc_List_List*_tmp945=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4AA->tl=_tmp945;});_tmp4AA;}));
goto _LL0;}case 162U: _LL13F: _LL140:
# 1935
 yyval=Cyc_YY32(Cyc_Absyn_Public);
goto _LL0;case 163U: _LL141: _LL142: {
# 1938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1937 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Extern);
goto _LL0;}case 164U: _LL143: _LL144: {
# 1940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1938 "parse.y"
yyval=Cyc_YY32(Cyc_Absyn_Static);
goto _LL0;}case 165U: _LL145: _LL146: {
# 1941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1942 "parse.y"
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct _tuple0*_tmp948=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4AB->name=_tmp948;}),_tmp4AB->typs=0,_tmp4AB->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp947=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4AB->sc=_tmp947;});_tmp4AB;}));
goto _LL0;}case 166U: _LL147: _LL148: {
# 1945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1944 "parse.y"
struct Cyc_List_List*_tmp4AC=({unsigned int _tmp949=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp949,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
yyval=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));({struct _tuple0*_tmp94B=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4AD->name=_tmp94B;}),_tmp4AD->typs=_tmp4AC,_tmp4AD->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp94A=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4AD->sc=_tmp94A;});_tmp4AD;}));
goto _LL0;}case 167U: _LL149: _LL14A: {
# 1948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1950 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 168U: _LL14B: _LL14C: {
# 1953
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1952 "parse.y"
struct Cyc_Parse_Declarator _tmp4AE=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp722;_tmp722.id=_tmp4AE.id,_tmp722.varloc=_tmp4AE.varloc,({struct Cyc_List_List*_tmp94D=({struct Cyc_List_List*_tmp94C=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp94C,_tmp4AE.tms);});_tmp722.tms=_tmp94D;});_tmp722;}));
goto _LL0;}case 169U: _LL14D: _LL14E: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 170U: _LL14F: _LL150: {
# 1962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1961 "parse.y"
struct Cyc_Parse_Declarator _tmp4AF=Cyc_yyget_YY27(yyyyvsp[1]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp723;_tmp723.id=_tmp4AF.id,_tmp723.varloc=_tmp4AF.varloc,({struct Cyc_List_List*_tmp94F=({struct Cyc_List_List*_tmp94E=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp94E,_tmp4AF.tms);});_tmp723.tms=_tmp94F;});_tmp723;}));
goto _LL0;}case 171U: _LL151: _LL152: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1967 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp724;({struct _tuple0*_tmp950=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp724.id=_tmp950;}),_tmp724.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp724.tms=0;_tmp724;}));
goto _LL0;}case 172U: _LL153: _LL154: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1969 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 173U: _LL155: _LL156: {
# 1972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1973 "parse.y"
struct Cyc_Parse_Declarator _tmp4B0=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmp953=({struct Cyc_List_List*_tmp4B2=_region_malloc(yyr,sizeof(*_tmp4B2));({void*_tmp952=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4B1=_region_malloc(yyr,sizeof(*_tmp4B1));_tmp4B1->tag=5U,_tmp4B1->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp951=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4B1->f2=_tmp951;});_tmp4B1;});_tmp4B2->hd=_tmp952;}),_tmp4B2->tl=_tmp4B0.tms;_tmp4B2;});_tmp4B0.tms=_tmp953;});
yyval=yyyyvsp[2];
# 1977
goto _LL0;}case 174U: _LL157: _LL158: {
# 1979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1978 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp725;({struct _tuple0*_tmp959=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp725.id=_tmp959;}),({unsigned int _tmp958=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp725.varloc=_tmp958;}),({struct Cyc_List_List*_tmp957=({struct Cyc_List_List*_tmp4B4=_region_malloc(yyr,sizeof(*_tmp4B4));({void*_tmp956=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4B3=_region_malloc(yyr,sizeof(*_tmp4B3));_tmp4B3->tag=0U,({void*_tmp955=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4B3->f1=_tmp955;}),_tmp4B3->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4B3;});_tmp4B4->hd=_tmp956;}),({struct Cyc_List_List*_tmp954=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B4->tl=_tmp954;});_tmp4B4;});_tmp725.tms=_tmp957;});_tmp725;}));
goto _LL0;}case 175U: _LL159: _LL15A: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1980 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp726;({struct _tuple0*_tmp960=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp726.id=_tmp960;}),({unsigned int _tmp95F=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp726.varloc=_tmp95F;}),({
struct Cyc_List_List*_tmp95E=({struct Cyc_List_List*_tmp4B6=_region_malloc(yyr,sizeof(*_tmp4B6));({void*_tmp95D=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));_tmp4B5->tag=1U,({struct Cyc_Absyn_Exp*_tmp95C=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4B5->f1=_tmp95C;}),({void*_tmp95B=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4B5->f2=_tmp95B;}),_tmp4B5->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4B5;});_tmp4B6->hd=_tmp95D;}),({struct Cyc_List_List*_tmp95A=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B6->tl=_tmp95A;});_tmp4B6;});_tmp726.tms=_tmp95E;});_tmp726;}));
goto _LL0;}case 176U: _LL15B: _LL15C: {
# 1984
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1983 "parse.y"
struct _tuple26*_tmp4B7=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4B8=_tmp4B7;struct Cyc_List_List*_tmp4C2;int _tmp4C1;struct Cyc_Absyn_VarargInfo*_tmp4C0;void*_tmp4BF;struct Cyc_List_List*_tmp4BE;_LL447: _tmp4C2=_tmp4B8->f1;_tmp4C1=_tmp4B8->f2;_tmp4C0=_tmp4B8->f3;_tmp4BF=_tmp4B8->f4;_tmp4BE=_tmp4B8->f5;_LL448:;{
struct Cyc_Absyn_Exp*_tmp4B9=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4BA=Cyc_yyget_YY55(yyyyvsp[5]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp727;({struct _tuple0*_tmp966=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp727.id=_tmp966;}),({unsigned int _tmp965=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp727.varloc=_tmp965;}),({struct Cyc_List_List*_tmp964=({struct Cyc_List_List*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));({void*_tmp963=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4BC=_region_malloc(yyr,sizeof(*_tmp4BC));_tmp4BC->tag=3U,({void*_tmp962=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4BB=_region_malloc(yyr,sizeof(*_tmp4BB));_tmp4BB->tag=1U,_tmp4BB->f1=_tmp4C2,_tmp4BB->f2=_tmp4C1,_tmp4BB->f3=_tmp4C0,_tmp4BB->f4=_tmp4BF,_tmp4BB->f5=_tmp4BE,_tmp4BB->f6=_tmp4B9,_tmp4BB->f7=_tmp4BA;_tmp4BB;});_tmp4BC->f1=_tmp962;});_tmp4BC;});_tmp4BD->hd=_tmp963;}),({struct Cyc_List_List*_tmp961=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4BD->tl=_tmp961;});_tmp4BD;});_tmp727.tms=_tmp964;});_tmp727;}));
# 1988
goto _LL0;};}case 177U: _LL15D: _LL15E: {
# 1990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1989 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp728;({struct _tuple0*_tmp970=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp728.id=_tmp970;}),({unsigned int _tmp96F=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp728.varloc=_tmp96F;}),({
struct Cyc_List_List*_tmp96E=({struct Cyc_List_List*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));({void*_tmp96D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));_tmp4C4->tag=3U,({void*_tmp96C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));_tmp4C3->tag=1U,_tmp4C3->f1=0,_tmp4C3->f2=0,_tmp4C3->f3=0,({
# 1992
void*_tmp96B=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4C3->f4=_tmp96B;}),({struct Cyc_List_List*_tmp96A=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4C3->f5=_tmp96A;}),({struct Cyc_Absyn_Exp*_tmp969=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4C3->f6=_tmp969;}),({struct Cyc_Absyn_Exp*_tmp968=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4C3->f7=_tmp968;});_tmp4C3;});
# 1990
_tmp4C4->f1=_tmp96C;});_tmp4C4;});_tmp4C5->hd=_tmp96D;}),({
# 1993
struct Cyc_List_List*_tmp967=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4C5->tl=_tmp967;});_tmp4C5;});
# 1990
_tmp728.tms=_tmp96E;});_tmp728;}));
# 1995
goto _LL0;}case 178U: _LL15F: _LL160: {
# 1997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1996 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp729;({struct _tuple0*_tmp977=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp729.id=_tmp977;}),({unsigned int _tmp976=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp729.varloc=_tmp976;}),({struct Cyc_List_List*_tmp975=({struct Cyc_List_List*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));({void*_tmp974=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));_tmp4C7->tag=3U,({void*_tmp973=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));_tmp4C6->tag=0U,({struct Cyc_List_List*_tmp972=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4C6->f1=_tmp972;}),_tmp4C6->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4C6;});_tmp4C7->f1=_tmp973;});_tmp4C7;});_tmp4C8->hd=_tmp974;}),({struct Cyc_List_List*_tmp971=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4C8->tl=_tmp971;});_tmp4C8;});_tmp729.tms=_tmp975;});_tmp729;}));
goto _LL0;}case 179U: _LL161: _LL162: {
# 1999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1999 "parse.y"
struct Cyc_List_List*_tmp4C9=({unsigned int _tmp978=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp978,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72A;({struct _tuple0*_tmp97D=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp72A.id=_tmp97D;}),({unsigned int _tmp97C=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp72A.varloc=_tmp97C;}),({struct Cyc_List_List*_tmp97B=({struct Cyc_List_List*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));({void*_tmp97A=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=4U,_tmp4CA->f1=_tmp4C9,_tmp4CA->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4CA->f3=0;_tmp4CA;});_tmp4CB->hd=_tmp97A;}),({struct Cyc_List_List*_tmp979=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4CB->tl=_tmp979;});_tmp4CB;});_tmp72A.tms=_tmp97B;});_tmp72A;}));
# 2002
goto _LL0;}case 180U: _LL163: _LL164: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2003 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72B;({struct _tuple0*_tmp983=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp72B.id=_tmp983;}),({unsigned int _tmp982=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp72B.varloc=_tmp982;}),({struct Cyc_List_List*_tmp981=({struct Cyc_List_List*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));({void*_tmp980=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));_tmp4CC->tag=5U,_tmp4CC->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp97F=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4CC->f2=_tmp97F;});_tmp4CC;});_tmp4CD->hd=_tmp980;}),({
struct Cyc_List_List*_tmp97E=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4CD->tl=_tmp97E;});_tmp4CD;});
# 2003
_tmp72B.tms=_tmp981;});_tmp72B;}));
# 2006
goto _LL0;}case 181U: _LL165: _LL166: {
# 2008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72C;({struct _tuple0*_tmp984=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp72C.id=_tmp984;}),_tmp72C.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp72C.tms=0;_tmp72C;}));
goto _LL0;}case 182U: _LL167: _LL168: {
# 2014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2013 "parse.y"
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72D;({struct _tuple0*_tmp985=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp72D.id=_tmp985;}),_tmp72D.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp72D.tms=0;_tmp72D;}));
goto _LL0;}case 183U: _LL169: _LL16A: {
# 2016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2015 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 184U: _LL16B: _LL16C: {
# 2018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2019 "parse.y"
struct Cyc_Parse_Declarator _tmp4CE=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmp988=({struct Cyc_List_List*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));({void*_tmp987=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));_tmp4CF->tag=5U,_tmp4CF->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp986=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4CF->f2=_tmp986;});_tmp4CF;});_tmp4D0->hd=_tmp987;}),_tmp4D0->tl=_tmp4CE.tms;_tmp4D0;});_tmp4CE.tms=_tmp988;});
yyval=yyyyvsp[2];
# 2023
goto _LL0;}case 185U: _LL16D: _LL16E: {
# 2025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2024 "parse.y"
struct Cyc_Parse_Declarator _tmp4D1=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72E;_tmp72E.id=_tmp4D1.id,_tmp72E.varloc=_tmp4D1.varloc,({
struct Cyc_List_List*_tmp98B=({struct Cyc_List_List*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));({void*_tmp98A=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4D2=_region_malloc(yyr,sizeof(*_tmp4D2));_tmp4D2->tag=0U,({void*_tmp989=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4D2->f1=_tmp989;}),_tmp4D2->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4D2;});_tmp4D3->hd=_tmp98A;}),_tmp4D3->tl=_tmp4D1.tms;_tmp4D3;});_tmp72E.tms=_tmp98B;});_tmp72E;}));
goto _LL0;}case 186U: _LL16F: _LL170: {
# 2029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2028 "parse.y"
struct Cyc_Parse_Declarator _tmp4D4=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72F;_tmp72F.id=_tmp4D4.id,_tmp72F.varloc=_tmp4D4.varloc,({
struct Cyc_List_List*_tmp98F=({struct Cyc_List_List*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));({void*_tmp98E=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));_tmp4D5->tag=1U,({struct Cyc_Absyn_Exp*_tmp98D=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4D5->f1=_tmp98D;}),({void*_tmp98C=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4D5->f2=_tmp98C;}),_tmp4D5->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4D5;});_tmp4D6->hd=_tmp98E;}),_tmp4D6->tl=_tmp4D4.tms;_tmp4D6;});_tmp72F.tms=_tmp98F;});_tmp72F;}));
# 2032
goto _LL0;}case 187U: _LL171: _LL172: {
# 2034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2033 "parse.y"
struct _tuple26*_tmp4D7=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4D8=_tmp4D7;struct Cyc_List_List*_tmp4E3;int _tmp4E2;struct Cyc_Absyn_VarargInfo*_tmp4E1;void*_tmp4E0;struct Cyc_List_List*_tmp4DF;_LL44A: _tmp4E3=_tmp4D8->f1;_tmp4E2=_tmp4D8->f2;_tmp4E1=_tmp4D8->f3;_tmp4E0=_tmp4D8->f4;_tmp4DF=_tmp4D8->f5;_LL44B:;{
struct Cyc_Absyn_Exp*_tmp4D9=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4DA=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp4DB=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp730;_tmp730.id=_tmp4DB.id,_tmp730.varloc=_tmp4DB.varloc,({struct Cyc_List_List*_tmp992=({struct Cyc_List_List*_tmp4DE=_region_malloc(yyr,sizeof(*_tmp4DE));({void*_tmp991=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4DD=_region_malloc(yyr,sizeof(*_tmp4DD));_tmp4DD->tag=3U,({void*_tmp990=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));_tmp4DC->tag=1U,_tmp4DC->f1=_tmp4E3,_tmp4DC->f2=_tmp4E2,_tmp4DC->f3=_tmp4E1,_tmp4DC->f4=_tmp4E0,_tmp4DC->f5=_tmp4DF,_tmp4DC->f6=_tmp4D9,_tmp4DC->f7=_tmp4DA;_tmp4DC;});_tmp4DD->f1=_tmp990;});_tmp4DD;});_tmp4DE->hd=_tmp991;}),_tmp4DE->tl=_tmp4DB.tms;_tmp4DE;});_tmp730.tms=_tmp992;});_tmp730;}));
# 2039
goto _LL0;};}case 188U: _LL173: _LL174: {
# 2041
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2040 "parse.y"
struct Cyc_Parse_Declarator _tmp4E4=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp731;_tmp731.id=_tmp4E4.id,_tmp731.varloc=_tmp4E4.varloc,({
struct Cyc_List_List*_tmp999=({struct Cyc_List_List*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));({void*_tmp998=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E6=_region_malloc(yyr,sizeof(*_tmp4E6));_tmp4E6->tag=3U,({void*_tmp997=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));_tmp4E5->tag=1U,_tmp4E5->f1=0,_tmp4E5->f2=0,_tmp4E5->f3=0,({
# 2044
void*_tmp996=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4E5->f4=_tmp996;}),({struct Cyc_List_List*_tmp995=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4E5->f5=_tmp995;}),({struct Cyc_Absyn_Exp*_tmp994=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4E5->f6=_tmp994;}),({struct Cyc_Absyn_Exp*_tmp993=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4E5->f7=_tmp993;});_tmp4E5;});
# 2042
_tmp4E6->f1=_tmp997;});_tmp4E6;});_tmp4E7->hd=_tmp998;}),_tmp4E7->tl=_tmp4E4.tms;_tmp4E7;});_tmp731.tms=_tmp999;});_tmp731;}));
# 2047
goto _LL0;}case 189U: _LL175: _LL176: {
# 2049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2048 "parse.y"
struct Cyc_Parse_Declarator _tmp4E8=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp732;_tmp732.id=_tmp4E8.id,_tmp732.varloc=_tmp4E8.varloc,({struct Cyc_List_List*_tmp99D=({struct Cyc_List_List*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));({void*_tmp99C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));_tmp4EA->tag=3U,({void*_tmp99B=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4E9=_region_malloc(yyr,sizeof(*_tmp4E9));_tmp4E9->tag=0U,({struct Cyc_List_List*_tmp99A=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4E9->f1=_tmp99A;}),_tmp4E9->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4E9;});_tmp4EA->f1=_tmp99B;});_tmp4EA;});_tmp4EB->hd=_tmp99C;}),_tmp4EB->tl=_tmp4E8.tms;_tmp4EB;});_tmp732.tms=_tmp99D;});_tmp732;}));
goto _LL0;}case 190U: _LL177: _LL178: {
# 2052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2052 "parse.y"
struct Cyc_List_List*_tmp4EC=({unsigned int _tmp99E=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp99E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp4ED=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp733;_tmp733.id=_tmp4ED.id,_tmp733.varloc=_tmp4ED.varloc,({struct Cyc_List_List*_tmp9A0=({struct Cyc_List_List*_tmp4EF=_region_malloc(yyr,sizeof(*_tmp4EF));({void*_tmp99F=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));_tmp4EE->tag=4U,_tmp4EE->f1=_tmp4EC,_tmp4EE->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4EE->f3=0;_tmp4EE;});_tmp4EF->hd=_tmp99F;}),_tmp4EF->tl=_tmp4ED.tms;_tmp4EF;});_tmp733.tms=_tmp9A0;});_tmp733;}));
# 2056
goto _LL0;}case 191U: _LL179: _LL17A: {
# 2058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2057 "parse.y"
struct Cyc_Parse_Declarator _tmp4F0=Cyc_yyget_YY27(yyyyvsp[0]);
yyval=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp734;_tmp734.id=_tmp4F0.id,_tmp734.varloc=_tmp4F0.varloc,({struct Cyc_List_List*_tmp9A3=({struct Cyc_List_List*_tmp4F2=_region_malloc(yyr,sizeof(*_tmp4F2));({void*_tmp9A2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));_tmp4F1->tag=5U,_tmp4F1->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmp9A1=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4F1->f2=_tmp9A1;});_tmp4F1;});_tmp4F2->hd=_tmp9A2;}),_tmp4F2->tl=_tmp4F0.tms;_tmp4F2;});_tmp734.tms=_tmp9A3;});_tmp734;}));
# 2060
goto _LL0;}case 192U: _LL17B: _LL17C: {
# 2062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2064 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 193U: _LL17D: _LL17E: {
# 2067
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2069
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2065 "parse.y"
yyval=Cyc_YY26(({struct Cyc_List_List*_tmp9A4=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9A4,Cyc_yyget_YY26(yyyyvsp[1]));}));
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2069 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
ans=({struct Cyc_List_List*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));({void*_tmp9A6=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));_tmp4F3->tag=5U,_tmp4F3->f1=(unsigned int)(yyyylsp[3]).first_line,({struct Cyc_List_List*_tmp9A5=Cyc_yyget_YY45(yyyyvsp[3]);_tmp4F3->f2=_tmp9A5;});_tmp4F3;});_tmp4F4->hd=_tmp9A6;}),_tmp4F4->tl=ans;_tmp4F4;});{
# 2073
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp4F5=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp4F6=_tmp4F5;unsigned int _tmp502;void*_tmp501;void*_tmp500;_LL44D: _tmp502=_tmp4F6.f1;_tmp501=_tmp4F6.f2;_tmp500=_tmp4F6.f3;_LL44E:;
if(Cyc_Absyn_porting_c_code)
ptrloc=({struct Cyc_Absyn_PtrLoc*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->ptr_loc=_tmp502,_tmp4F7->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,_tmp4F7->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp4F7;});{
# 2078
struct _tuple14 _tmp4F8=({unsigned int _tmp9AA=_tmp502;void*_tmp9A9=_tmp501;void*_tmp9A8=_tmp500;void*_tmp9A7=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmp9AA,_tmp9A9,_tmp9A8,_tmp9A7,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp4F9=_tmp4F8;void*_tmp4FF;void*_tmp4FE;void*_tmp4FD;void*_tmp4FC;_LL450: _tmp4FF=_tmp4F9.f1;_tmp4FE=_tmp4F9.f2;_tmp4FD=_tmp4F9.f3;_tmp4FC=_tmp4F9.f4;_LL451:;
ans=({struct Cyc_List_List*_tmp4FB=_region_malloc(yyr,sizeof(*_tmp4FB));({void*_tmp9AC=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4FA=_region_malloc(yyr,sizeof(*_tmp4FA));_tmp4FA->tag=2U,(_tmp4FA->f1).rgn=_tmp4FC,(_tmp4FA->f1).nullable=_tmp4FF,(_tmp4FA->f1).bounds=_tmp4FE,(_tmp4FA->f1).zero_term=_tmp4FD,(_tmp4FA->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmp9AB=Cyc_yyget_YY23(yyyyvsp[4]);_tmp4FA->f2=_tmp9AB;});_tmp4FA;});_tmp4FB->hd=_tmp9AC;}),_tmp4FB->tl=ans;_tmp4FB;});
yyval=Cyc_YY26(ans);
# 2082
goto _LL0;};};}case 195U: _LL181: _LL182:
# 2084
 yyval=Cyc_YY54(0);
goto _LL0;case 196U: _LL183: _LL184: {
# 2087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2085 "parse.y"
yyval=Cyc_YY54(({struct Cyc_List_List*_tmp503=_region_malloc(yyr,sizeof(*_tmp503));({void*_tmp9AE=Cyc_yyget_YY53(yyyyvsp[0]);_tmp503->hd=_tmp9AE;}),({struct Cyc_List_List*_tmp9AD=Cyc_yyget_YY54(yyyyvsp[1]);_tmp503->tl=_tmp9AD;});_tmp503;}));
goto _LL0;}case 197U: _LL185: _LL186: {
# 2088
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2090
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2090 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp504=_region_malloc(yyr,sizeof(*_tmp504));_tmp504->tag=0U,({struct Cyc_Absyn_Exp*_tmp9AF=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp504->f1=_tmp9AF;});_tmp504;}));
goto _LL0;}case 198U: _LL187: _LL188: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2092 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp505=_region_malloc(yyr,sizeof(*_tmp505));_tmp505->tag=1U,({void*_tmp9B0=Cyc_yyget_YY44(yyyyvsp[2]);_tmp505->f1=_tmp9B0;});_tmp505;}));
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2094 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));_tmp506->tag=2U;_tmp506;}));
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2096 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));_tmp507->tag=3U;_tmp507;}));
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2098 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp508=_region_malloc(yyr,sizeof(*_tmp508));_tmp508->tag=4U;_tmp508;}));
goto _LL0;}case 202U: _LL18F: _LL190: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2100 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));_tmp509->tag=5U;_tmp509;}));
goto _LL0;}case 203U: _LL191: _LL192: {
# 2103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2102 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp50A=_region_malloc(yyr,sizeof(*_tmp50A));_tmp50A->tag=6U;_tmp50A;}));
goto _LL0;}case 204U: _LL193: _LL194: {
# 2105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2104 "parse.y"
yyval=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp50B=_region_malloc(yyr,sizeof(*_tmp50B));_tmp50B->tag=7U;_tmp50B;}));
goto _LL0;}case 205U: _LL195: _LL196: {
# 2107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2110 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
if(!Cyc_Parse_parsing_tempest)
yyval=Cyc_YY1(({struct _tuple21*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->f1=loc,_tmp50C->f2=Cyc_Absyn_true_type,({void*_tmp9B1=Cyc_yyget_YY2(yyyyvsp[1]);_tmp50C->f3=_tmp9B1;});_tmp50C;}));else{
# 2114
yyval=Cyc_YY1(({struct _tuple21*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->f1=loc,_tmp50D->f2=Cyc_Absyn_true_type,_tmp50D->f3=Cyc_Absyn_fat_bound_type;_tmp50D;}));}
# 2116
goto _LL0;}case 206U: _LL197: _LL198: {
# 2118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2117 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->f1=loc,_tmp50E->f2=Cyc_Absyn_false_type,({void*_tmp9B2=Cyc_yyget_YY2(yyyyvsp[1]);_tmp50E->f3=_tmp9B2;});_tmp50E;}));
# 2120
goto _LL0;}case 207U: _LL199: _LL19A: {
# 2122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2121 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
yyval=Cyc_YY1(({struct _tuple21*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->f1=loc,_tmp50F->f2=Cyc_Absyn_true_type,_tmp50F->f3=Cyc_Absyn_fat_bound_type;_tmp50F;}));
# 2124
goto _LL0;}case 208U: _LL19B: _LL19C:
# 2126
 yyval=Cyc_YY2(Cyc_Absyn_bounds_one());
goto _LL0;case 209U: _LL19D: _LL19E: {
# 2129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2127 "parse.y"
yyval=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 210U: _LL19F: _LL1A0:
# 2130
 yyval=Cyc_YY51(Cyc_Tcutil_any_bool(0));
goto _LL0;case 211U: _LL1A1: _LL1A2: {
# 2133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2131 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_true_type);
goto _LL0;}case 212U: _LL1A3: _LL1A4: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2132 "parse.y"
yyval=Cyc_YY51(Cyc_Absyn_false_type);
goto _LL0;}case 213U: _LL1A5: _LL1A6:
# 2135
 yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;case 214U: _LL1A7: _LL1A8: {
# 2138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2138 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL0;}case 215U: _LL1A9: _LL1AA: {
# 2141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2139 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));
goto _LL0;}case 216U: _LL1AB: _LL1AC:
# 2142
 yyval=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));
goto _LL0;case 217U: _LL1AD: _LL1AE: {
# 2145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2144 "parse.y"
yyval=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp9B3=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp9B3,Cyc_yyget_YY23(yyyyvsp[1]));}));
goto _LL0;}case 218U: _LL1AF: _LL1B0: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2149 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp510=_cycalloc(sizeof(*_tmp510));({struct Cyc_List_List*_tmp9B6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp510->f1=_tmp9B6;}),_tmp510->f2=0,_tmp510->f3=0,({void*_tmp9B5=Cyc_yyget_YY49(yyyyvsp[1]);_tmp510->f4=_tmp9B5;}),({struct Cyc_List_List*_tmp9B4=Cyc_yyget_YY50(yyyyvsp[2]);_tmp510->f5=_tmp9B4;});_tmp510;}));
goto _LL0;}case 219U: _LL1B1: _LL1B2: {
# 2152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2151 "parse.y"
yyval=Cyc_YY39(({struct _tuple26*_tmp511=_cycalloc(sizeof(*_tmp511));({struct Cyc_List_List*_tmp9B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp511->f1=_tmp9B9;}),_tmp511->f2=1,_tmp511->f3=0,({void*_tmp9B8=Cyc_yyget_YY49(yyyyvsp[3]);_tmp511->f4=_tmp9B8;}),({struct Cyc_List_List*_tmp9B7=Cyc_yyget_YY50(yyyyvsp[4]);_tmp511->f5=_tmp9B7;});_tmp511;}));
goto _LL0;}case 220U: _LL1B3: _LL1B4: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2154 "parse.y"
struct _tuple8*_tmp512=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp513=_tmp512;struct _dyneither_ptr*_tmp519;struct Cyc_Absyn_Tqual _tmp518;void*_tmp517;_LL453: _tmp519=_tmp513->f1;_tmp518=_tmp513->f2;_tmp517=_tmp513->f3;_LL454:;{
struct Cyc_Absyn_VarargInfo*_tmp514=({struct Cyc_Absyn_VarargInfo*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->name=_tmp519,_tmp516->tq=_tmp518,_tmp516->type=_tmp517,({int _tmp9BA=Cyc_yyget_YY31(yyyyvsp[1]);_tmp516->inject=_tmp9BA;});_tmp516;});
yyval=Cyc_YY39(({struct _tuple26*_tmp515=_cycalloc(sizeof(*_tmp515));_tmp515->f1=0,_tmp515->f2=0,_tmp515->f3=_tmp514,({void*_tmp9BC=Cyc_yyget_YY49(yyyyvsp[3]);_tmp515->f4=_tmp9BC;}),({struct Cyc_List_List*_tmp9BB=Cyc_yyget_YY50(yyyyvsp[4]);_tmp515->f5=_tmp9BB;});_tmp515;}));
# 2158
goto _LL0;};}case 221U: _LL1B5: _LL1B6: {
# 2160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2160 "parse.y"
struct _tuple8*_tmp51A=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp51B=_tmp51A;struct _dyneither_ptr*_tmp521;struct Cyc_Absyn_Tqual _tmp520;void*_tmp51F;_LL456: _tmp521=_tmp51B->f1;_tmp520=_tmp51B->f2;_tmp51F=_tmp51B->f3;_LL457:;{
struct Cyc_Absyn_VarargInfo*_tmp51C=({struct Cyc_Absyn_VarargInfo*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->name=_tmp521,_tmp51E->tq=_tmp520,_tmp51E->type=_tmp51F,({int _tmp9BD=Cyc_yyget_YY31(yyyyvsp[3]);_tmp51E->inject=_tmp9BD;});_tmp51E;});
yyval=Cyc_YY39(({struct _tuple26*_tmp51D=_cycalloc(sizeof(*_tmp51D));({struct Cyc_List_List*_tmp9C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp51D->f1=_tmp9C0;}),_tmp51D->f2=0,_tmp51D->f3=_tmp51C,({void*_tmp9BF=Cyc_yyget_YY49(yyyyvsp[5]);_tmp51D->f4=_tmp9BF;}),({struct Cyc_List_List*_tmp9BE=Cyc_yyget_YY50(yyyyvsp[6]);_tmp51D->f5=_tmp9BE;});_tmp51D;}));
# 2164
goto _LL0;};}case 222U: _LL1B7: _LL1B8: {
# 2166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2168 "parse.y"
yyval=Cyc_YY44(({struct _dyneither_ptr _tmp9C1=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmp9C1,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->tag=1U,_tmp522->f1=0;_tmp522;}));}));
goto _LL0;}case 223U: _LL1B9: _LL1BA: {
# 2171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2169 "parse.y"
yyval=Cyc_YY44(({struct _dyneither_ptr _tmp9C2=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmp9C2,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));
goto _LL0;}case 224U: _LL1BB: _LL1BC:
# 2172
 yyval=Cyc_YY49(0);
goto _LL0;case 225U: _LL1BD: _LL1BE: {
# 2175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2173 "parse.y"
yyval=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 226U: _LL1BF: _LL1C0:
# 2176
 yyval=Cyc_YY50(0);
goto _LL0;case 227U: _LL1C1: _LL1C2: {
# 2179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2178 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 228U: _LL1C3: _LL1C4: {
# 2181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2186 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp523=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp528=_cycalloc(sizeof(*_tmp528));_tmp528->tag=2U,_tmp528->f1=0,_tmp528->f2=& Cyc_Tcutil_trk;_tmp528;});
struct _dyneither_ptr _tmp524=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp525=Cyc_Parse_id2type(_tmp524,(void*)_tmp523);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp527=_cycalloc(sizeof(*_tmp527));({struct _tuple29*_tmp9C4=({struct _tuple29*_tmp526=_cycalloc(sizeof(*_tmp526));({void*_tmp9C3=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp526->f1=_tmp9C3;}),_tmp526->f2=_tmp525;_tmp526;});_tmp527->hd=_tmp9C4;}),_tmp527->tl=0;_tmp527;}));
# 2191
goto _LL0;}case 229U: _LL1C5: _LL1C6: {
# 2193
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2195
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2193 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp529=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->tag=2U,_tmp52E->f1=0,_tmp52E->f2=& Cyc_Tcutil_trk;_tmp52E;});
struct _dyneither_ptr _tmp52A=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp52B=Cyc_Parse_id2type(_tmp52A,(void*)_tmp529);
yyval=Cyc_YY50(({struct Cyc_List_List*_tmp52D=_cycalloc(sizeof(*_tmp52D));({struct _tuple29*_tmp9C7=({struct _tuple29*_tmp52C=_cycalloc(sizeof(*_tmp52C));({void*_tmp9C6=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp52C->f1=_tmp9C6;}),_tmp52C->f2=_tmp52B;_tmp52C;});_tmp52D->hd=_tmp9C7;}),({struct Cyc_List_List*_tmp9C5=Cyc_yyget_YY50(yyyyvsp[4]);_tmp52D->tl=_tmp9C5;});_tmp52D;}));
# 2198
goto _LL0;}case 230U: _LL1C7: _LL1C8:
# 2200
 yyval=Cyc_YY31(0);
goto _LL0;case 231U: _LL1C9: _LL1CA: {
# 2203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2204 "parse.y"
if(({struct _dyneither_ptr _tmp9C8=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmp9C8,({const char*_tmp52F="inject";_tag_dyneither(_tmp52F,sizeof(char),7U);}));})!= 0)
({void*_tmp530=0U;({unsigned int _tmp9CA=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9C9=({const char*_tmp531="missing type in function declaration";_tag_dyneither(_tmp531,sizeof(char),37U);});Cyc_Warn_err(_tmp9CA,_tmp9C9,_tag_dyneither(_tmp530,sizeof(void*),0U));});});
yyval=Cyc_YY31(1);
# 2208
goto _LL0;}case 232U: _LL1CB: _LL1CC: {
# 2210
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2212
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2211 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 233U: _LL1CD: _LL1CE: {
# 2214
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2216
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2212 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp9CB=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9CB,Cyc_yyget_YY40(yyyyvsp[2]));}));
goto _LL0;}case 234U: _LL1CF: _LL1D0: {
# 2215
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2217
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2216 "parse.y"
yyval=Cyc_YY40(0);
goto _LL0;}case 235U: _LL1D1: _LL1D2: {
# 2219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2217 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 236U: _LL1D3: _LL1D4: {
# 2220
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2222
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2219 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp532=_cycalloc(sizeof(*_tmp532));({void*_tmp9CC=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2]));_tmp532->hd=_tmp9CC;}),_tmp532->tl=0;_tmp532;}));
goto _LL0;}case 237U: _LL1D5: _LL1D6: {
# 2222
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2224
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2221 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp533=_cycalloc(sizeof(*_tmp533));({void*_tmp9CD=Cyc_yyget_YY44(yyyyvsp[0]);_tmp533->hd=_tmp9CD;}),_tmp533->tl=0;_tmp533;}));
# 2224
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2226
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2228
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2241 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp534=_cycalloc(sizeof(*_tmp534));({void*_tmp9CF=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9CE=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9CE,(unsigned int)(yyyylsp[0]).first_line);}));_tmp534->hd=_tmp9CF;}),_tmp534->tl=0;_tmp534;}));
goto _LL0;}case 239U: _LL1D9: _LL1DA: {
# 2244
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2246
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2243 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp535=_cycalloc(sizeof(*_tmp535));({void*_tmp9D2=Cyc_Absyn_access_eff(({struct _tuple8*_tmp9D1=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmp9D1,(unsigned int)(yyyylsp[0]).first_line);}));_tmp535->hd=_tmp9D2;}),({struct Cyc_List_List*_tmp9D0=Cyc_yyget_YY40(yyyyvsp[2]);_tmp535->tl=_tmp9D0;});_tmp535;}));
goto _LL0;}case 240U: _LL1DB: _LL1DC: {
# 2246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp536=_cycalloc(sizeof(*_tmp536));({struct _tuple8*_tmp9D3=Cyc_yyget_YY37(yyyyvsp[0]);_tmp536->hd=_tmp9D3;}),_tmp536->tl=0;_tmp536;}));
goto _LL0;}case 241U: _LL1DD: _LL1DE: {
# 2252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2251 "parse.y"
yyval=Cyc_YY38(({struct Cyc_List_List*_tmp537=_cycalloc(sizeof(*_tmp537));({struct _tuple8*_tmp9D5=Cyc_yyget_YY37(yyyyvsp[2]);_tmp537->hd=_tmp9D5;}),({struct Cyc_List_List*_tmp9D4=Cyc_yyget_YY38(yyyyvsp[0]);_tmp537->tl=_tmp9D4;});_tmp537;}));
goto _LL0;}case 242U: _LL1DF: _LL1E0: {
# 2254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
struct _tuple25 _tmp538=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp539=_tmp538;struct Cyc_Absyn_Tqual _tmp54F;struct Cyc_Parse_Type_specifier _tmp54E;struct Cyc_List_List*_tmp54D;_LL459: _tmp54F=_tmp539.f1;_tmp54E=_tmp539.f2;_tmp54D=_tmp539.f3;_LL45A:;
if(_tmp54F.loc == 0)_tmp54F.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp53A=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp53B=_tmp53A;struct _tuple0*_tmp54C;unsigned int _tmp54B;struct Cyc_List_List*_tmp54A;_LL45C: _tmp54C=_tmp53B.id;_tmp54B=_tmp53B.varloc;_tmp54A=_tmp53B.tms;_LL45D:;{
void*_tmp53C=Cyc_Parse_speclist2typ(_tmp54E,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp53D=Cyc_Parse_apply_tms(_tmp54F,_tmp53C,_tmp54D,_tmp54A);struct _tuple13 _tmp53E=_tmp53D;struct Cyc_Absyn_Tqual _tmp549;void*_tmp548;struct Cyc_List_List*_tmp547;struct Cyc_List_List*_tmp546;_LL45F: _tmp549=_tmp53E.f1;_tmp548=_tmp53E.f2;_tmp547=_tmp53E.f3;_tmp546=_tmp53E.f4;_LL460:;
if(_tmp547 != 0)
({void*_tmp53F=0U;({unsigned int _tmp9D7=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp9D6=({const char*_tmp540="parameter with bad type params";_tag_dyneither(_tmp540,sizeof(char),31U);});Cyc_Warn_err(_tmp9D7,_tmp9D6,_tag_dyneither(_tmp53F,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp54C))
({void*_tmp541=0U;({unsigned int _tmp9D9=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9D8=({const char*_tmp542="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp542,sizeof(char),47U);});Cyc_Warn_err(_tmp9D9,_tmp9D8,_tag_dyneither(_tmp541,sizeof(void*),0U));});});{
struct _dyneither_ptr*idopt=(*_tmp54C).f2;
if(_tmp546 != 0)
({void*_tmp543=0U;({unsigned int _tmp9DB=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9DA=({const char*_tmp544="extra attributes on parameter, ignoring";_tag_dyneither(_tmp544,sizeof(char),40U);});Cyc_Warn_warn(_tmp9DB,_tmp9DA,_tag_dyneither(_tmp543,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->f1=idopt,_tmp545->f2=_tmp549,_tmp545->f3=_tmp548;_tmp545;}));
# 2271
goto _LL0;};};};}case 243U: _LL1E1: _LL1E2: {
# 2273
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2275
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2272 "parse.y"
struct _tuple25 _tmp550=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp551=_tmp550;struct Cyc_Absyn_Tqual _tmp558;struct Cyc_Parse_Type_specifier _tmp557;struct Cyc_List_List*_tmp556;_LL462: _tmp558=_tmp551.f1;_tmp557=_tmp551.f2;_tmp556=_tmp551.f3;_LL463:;
if(_tmp558.loc == 0)_tmp558.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp552=Cyc_Parse_speclist2typ(_tmp557,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp556 != 0)
({void*_tmp553=0U;({unsigned int _tmp9DD=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9DC=({const char*_tmp554="bad attributes on parameter, ignoring";_tag_dyneither(_tmp554,sizeof(char),38U);});Cyc_Warn_warn(_tmp9DD,_tmp9DC,_tag_dyneither(_tmp553,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->f1=0,_tmp555->f2=_tmp558,_tmp555->f3=_tmp552;_tmp555;}));
# 2279
goto _LL0;};}case 244U: _LL1E3: _LL1E4: {
# 2281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2280 "parse.y"
struct _tuple25 _tmp559=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp55A=_tmp559;struct Cyc_Absyn_Tqual _tmp56A;struct Cyc_Parse_Type_specifier _tmp569;struct Cyc_List_List*_tmp568;_LL465: _tmp56A=_tmp55A.f1;_tmp569=_tmp55A.f2;_tmp568=_tmp55A.f3;_LL466:;
if(_tmp56A.loc == 0)_tmp56A.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp55B=Cyc_Parse_speclist2typ(_tmp569,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp55C=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp55D=Cyc_Parse_apply_tms(_tmp56A,_tmp55B,_tmp568,_tmp55C);struct _tuple13 _tmp55E=_tmp55D;struct Cyc_Absyn_Tqual _tmp567;void*_tmp566;struct Cyc_List_List*_tmp565;struct Cyc_List_List*_tmp564;_LL468: _tmp567=_tmp55E.f1;_tmp566=_tmp55E.f2;_tmp565=_tmp55E.f3;_tmp564=_tmp55E.f4;_LL469:;
if(_tmp565 != 0)
({void*_tmp55F=0U;({unsigned int _tmp9DF=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9DE=({const char*_tmp560="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp560,sizeof(char),49U);});Cyc_Warn_warn(_tmp9DF,_tmp9DE,_tag_dyneither(_tmp55F,sizeof(void*),0U));});});
# 2288
if(_tmp564 != 0)
({void*_tmp561=0U;({unsigned int _tmp9E1=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp9E0=({const char*_tmp562="bad attributes on parameter, ignoring";_tag_dyneither(_tmp562,sizeof(char),38U);});Cyc_Warn_warn(_tmp9E1,_tmp9E0,_tag_dyneither(_tmp561,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->f1=0,_tmp563->f2=_tmp567,_tmp563->f3=_tmp566;_tmp563;}));
# 2292
goto _LL0;};}case 245U: _LL1E5: _LL1E6: {
# 2294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2296 "parse.y"
yyval=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));
goto _LL0;}case 246U: _LL1E7: _LL1E8: {
# 2299
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2301
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2300 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp56C=_cycalloc(sizeof(*_tmp56C));({struct _dyneither_ptr*_tmp9E3=({struct _dyneither_ptr*_tmp56B=_cycalloc(sizeof(*_tmp56B));({struct _dyneither_ptr _tmp9E2=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp56B=_tmp9E2;});_tmp56B;});_tmp56C->hd=_tmp9E3;}),_tmp56C->tl=0;_tmp56C;}));
goto _LL0;}case 247U: _LL1E9: _LL1EA: {
# 2303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2302 "parse.y"
yyval=Cyc_YY36(({struct Cyc_List_List*_tmp56E=_cycalloc(sizeof(*_tmp56E));({struct _dyneither_ptr*_tmp9E6=({struct _dyneither_ptr*_tmp56D=_cycalloc(sizeof(*_tmp56D));({struct _dyneither_ptr _tmp9E5=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp56D=_tmp9E5;});_tmp56D;});_tmp56E->hd=_tmp9E6;}),({struct Cyc_List_List*_tmp9E4=Cyc_yyget_YY36(yyyyvsp[0]);_tmp56E->tl=_tmp9E4;});_tmp56E;}));
goto _LL0;}case 248U: _LL1EB: _LL1EC: {
# 2305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2306 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 249U: _LL1ED: _LL1EE: {
# 2309
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2307 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 250U: _LL1EF: _LL1F0: {
# 2310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2312 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9E7=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=36U,_tmp56F->f1=0,_tmp56F->f2=0;_tmp56F;});Cyc_Absyn_new_exp(_tmp9E7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 251U: _LL1F1: _LL1F2: {
# 2315
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2317
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2314 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9E9=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->tag=36U,_tmp570->f1=0,({struct Cyc_List_List*_tmp9E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp570->f2=_tmp9E8;});_tmp570;});Cyc_Absyn_new_exp(_tmp9E9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 252U: _LL1F3: _LL1F4: {
# 2317
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2319
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2316 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmp9EB=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp571=_cycalloc(sizeof(*_tmp571));_tmp571->tag=36U,_tmp571->f1=0,({struct Cyc_List_List*_tmp9EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp571->f2=_tmp9EA;});_tmp571;});Cyc_Absyn_new_exp(_tmp9EB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 253U: _LL1F5: _LL1F6: {
# 2319
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2321
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2318 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp572=({unsigned int _tmp9F0=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp9EF=({struct _tuple0*_tmp575=_cycalloc(sizeof(*_tmp575));_tmp575->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp9ED=({struct _dyneither_ptr*_tmp574=_cycalloc(sizeof(*_tmp574));({struct _dyneither_ptr _tmp9EC=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp574=_tmp9EC;});_tmp574;});_tmp575->f2=_tmp9ED;});_tmp575;});void*_tmp9EE=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmp9F0,_tmp9EF,_tmp9EE,
Cyc_Absyn_uint_exp(0U,(unsigned int)(yyyylsp[2]).first_line));});
# 2321
(_tmp572->tq).real_const=1;
yyval=Cyc_Exp_tok(({void*_tmp9F3=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->tag=27U,_tmp573->f1=_tmp572,({struct Cyc_Absyn_Exp*_tmp9F2=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp573->f2=_tmp9F2;}),({struct Cyc_Absyn_Exp*_tmp9F1=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp573->f3=_tmp9F1;}),_tmp573->f4=0;_tmp573;});Cyc_Absyn_new_exp(_tmp9F3,(unsigned int)(yyyylsp[0]).first_line);}));
# 2324
goto _LL0;}case 254U: _LL1F7: _LL1F8: {
# 2326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2326 "parse.y"
void*_tmp576=({struct _tuple8*_tmp9F4=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmp9F4,(unsigned int)(yyyylsp[6]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmp9F6=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp577=_cycalloc(sizeof(*_tmp577));_tmp577->tag=28U,({struct Cyc_Absyn_Exp*_tmp9F5=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp577->f1=_tmp9F5;}),_tmp577->f2=_tmp576,_tmp577->f3=0;_tmp577;});Cyc_Absyn_new_exp(_tmp9F6,(unsigned int)(yyyylsp[0]).first_line);}));
# 2329
goto _LL0;}case 255U: _LL1F9: _LL1FA: {
# 2331
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2333
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp579=_cycalloc(sizeof(*_tmp579));({struct _tuple30*_tmp9F8=({struct _tuple30*_tmp578=_cycalloc(sizeof(*_tmp578));_tmp578->f1=0,({struct Cyc_Absyn_Exp*_tmp9F7=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp578->f2=_tmp9F7;});_tmp578;});_tmp579->hd=_tmp9F8;}),_tmp579->tl=0;_tmp579;}));
goto _LL0;}case 256U: _LL1FB: _LL1FC: {
# 2337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2336 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57B=_cycalloc(sizeof(*_tmp57B));({struct _tuple30*_tmp9FB=({struct _tuple30*_tmp57A=_cycalloc(sizeof(*_tmp57A));({struct Cyc_List_List*_tmp9FA=Cyc_yyget_YY41(yyyyvsp[0]);_tmp57A->f1=_tmp9FA;}),({struct Cyc_Absyn_Exp*_tmp9F9=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp57A->f2=_tmp9F9;});_tmp57A;});_tmp57B->hd=_tmp9FB;}),_tmp57B->tl=0;_tmp57B;}));
goto _LL0;}case 257U: _LL1FD: _LL1FE: {
# 2339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2338 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57D=_cycalloc(sizeof(*_tmp57D));({struct _tuple30*_tmp9FE=({struct _tuple30*_tmp57C=_cycalloc(sizeof(*_tmp57C));_tmp57C->f1=0,({struct Cyc_Absyn_Exp*_tmp9FD=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp57C->f2=_tmp9FD;});_tmp57C;});_tmp57D->hd=_tmp9FE;}),({struct Cyc_List_List*_tmp9FC=Cyc_yyget_YY5(yyyyvsp[0]);_tmp57D->tl=_tmp9FC;});_tmp57D;}));
goto _LL0;}case 258U: _LL1FF: _LL200: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2340 "parse.y"
yyval=Cyc_YY5(({struct Cyc_List_List*_tmp57F=_cycalloc(sizeof(*_tmp57F));({struct _tuple30*_tmpA02=({struct _tuple30*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct Cyc_List_List*_tmpA01=Cyc_yyget_YY41(yyyyvsp[2]);_tmp57E->f1=_tmpA01;}),({struct Cyc_Absyn_Exp*_tmpA00=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp57E->f2=_tmpA00;});_tmp57E;});_tmp57F->hd=_tmpA02;}),({struct Cyc_List_List*_tmp9FF=Cyc_yyget_YY5(yyyyvsp[0]);_tmp57F->tl=_tmp9FF;});_tmp57F;}));
goto _LL0;}case 259U: _LL201: _LL202: {
# 2343
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2345
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2344 "parse.y"
yyval=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));
goto _LL0;}case 260U: _LL203: _LL204: {
# 2347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2345 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp582=_cycalloc(sizeof(*_tmp582));({void*_tmpA05=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->tag=1U,({struct _dyneither_ptr*_tmpA04=({struct _dyneither_ptr*_tmp580=_cycalloc(sizeof(*_tmp580));({struct _dyneither_ptr _tmpA03=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp580=_tmpA03;});_tmp580;});_tmp581->f1=_tmpA04;});_tmp581;});_tmp582->hd=_tmpA05;}),_tmp582->tl=0;_tmp582;}));
goto _LL0;}case 261U: _LL205: _LL206: {
# 2348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2350 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp583=_cycalloc(sizeof(*_tmp583));({void*_tmpA06=Cyc_yyget_YY42(yyyyvsp[0]);_tmp583->hd=_tmpA06;}),_tmp583->tl=0;_tmp583;}));
goto _LL0;}case 262U: _LL207: _LL208: {
# 2353
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2355
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2351 "parse.y"
yyval=Cyc_YY41(({struct Cyc_List_List*_tmp584=_cycalloc(sizeof(*_tmp584));({void*_tmpA08=Cyc_yyget_YY42(yyyyvsp[1]);_tmp584->hd=_tmpA08;}),({struct Cyc_List_List*_tmpA07=Cyc_yyget_YY41(yyyyvsp[0]);_tmp584->tl=_tmpA07;});_tmp584;}));
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2354
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2355 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->tag=0U,({struct Cyc_Absyn_Exp*_tmpA09=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp585->f1=_tmpA09;});_tmp585;}));
goto _LL0;}case 264U: _LL20B: _LL20C: {
# 2358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2356 "parse.y"
yyval=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp587=_cycalloc(sizeof(*_tmp587));_tmp587->tag=1U,({struct _dyneither_ptr*_tmpA0B=({struct _dyneither_ptr*_tmp586=_cycalloc(sizeof(*_tmp586));({struct _dyneither_ptr _tmpA0A=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp586=_tmpA0A;});_tmp586;});_tmp587->f1=_tmpA0B;});_tmp587;}));
goto _LL0;}case 265U: _LL20D: _LL20E: {
# 2359
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2361
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2361 "parse.y"
struct _tuple25 _tmp588=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp589=_tmp588;struct Cyc_Absyn_Tqual _tmp590;struct Cyc_Parse_Type_specifier _tmp58F;struct Cyc_List_List*_tmp58E;_LL46B: _tmp590=_tmp589.f1;_tmp58F=_tmp589.f2;_tmp58E=_tmp589.f3;_LL46C:;{
void*_tmp58A=Cyc_Parse_speclist2typ(_tmp58F,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp58E != 0)
({void*_tmp58B=0U;({unsigned int _tmpA0D=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpA0C=({const char*_tmp58C="ignoring attributes in type";_tag_dyneither(_tmp58C,sizeof(char),28U);});Cyc_Warn_warn(_tmpA0D,_tmpA0C,_tag_dyneither(_tmp58B,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->f1=0,_tmp58D->f2=_tmp590,_tmp58D->f3=_tmp58A;_tmp58D;}));
# 2367
goto _LL0;};}case 266U: _LL20F: _LL210: {
# 2369
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2368 "parse.y"
struct _tuple25 _tmp591=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp592=_tmp591;struct Cyc_Absyn_Tqual _tmp59D;struct Cyc_Parse_Type_specifier _tmp59C;struct Cyc_List_List*_tmp59B;_LL46E: _tmp59D=_tmp592.f1;_tmp59C=_tmp592.f2;_tmp59B=_tmp592.f3;_LL46F:;{
void*_tmp593=Cyc_Parse_speclist2typ(_tmp59C,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp594=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp595=Cyc_Parse_apply_tms(_tmp59D,_tmp593,_tmp59B,_tmp594);
if(_tmp595.f3 != 0)
# 2374
({void*_tmp596=0U;({unsigned int _tmpA0F=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpA0E=({const char*_tmp597="bad type params, ignoring";_tag_dyneither(_tmp597,sizeof(char),26U);});Cyc_Warn_warn(_tmpA0F,_tmpA0E,_tag_dyneither(_tmp596,sizeof(void*),0U));});});
if(_tmp595.f4 != 0)
({void*_tmp598=0U;({unsigned int _tmpA11=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpA10=({const char*_tmp599="bad specifiers, ignoring";_tag_dyneither(_tmp599,sizeof(char),25U);});Cyc_Warn_warn(_tmpA11,_tmpA10,_tag_dyneither(_tmp598,sizeof(void*),0U));});});
yyval=Cyc_YY37(({struct _tuple8*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->f1=0,_tmp59A->f2=_tmp595.f1,_tmp59A->f3=_tmp595.f2;_tmp59A;}));
# 2379
goto _LL0;};}case 267U: _LL211: _LL212: {
# 2381
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2383
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2382 "parse.y"
yyval=Cyc_YY44(({struct _tuple8*_tmpA12=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpA12,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 268U: _LL213: _LL214: {
# 2385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2383 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(0));
goto _LL0;}case 269U: _LL215: _LL216: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2384 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));
goto _LL0;}case 270U: _LL217: _LL218: {
# 2387
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2389
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2385 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2])));
goto _LL0;}case 271U: _LL219: _LL21A: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2386 "parse.y"
yyval=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp59E=_cycalloc(sizeof(*_tmp59E));({void*_tmpA14=Cyc_yyget_YY44(yyyyvsp[0]);_tmp59E->hd=_tmpA14;}),({struct Cyc_List_List*_tmpA13=Cyc_yyget_YY40(yyyyvsp[2]);_tmp59E->tl=_tmpA13;});_tmp59E;})));
goto _LL0;}case 272U: _LL21B: _LL21C: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp59F=_cycalloc(sizeof(*_tmp59F));({void*_tmpA15=Cyc_yyget_YY44(yyyyvsp[0]);_tmp59F->hd=_tmpA15;}),_tmp59F->tl=0;_tmp59F;}));
goto _LL0;}case 273U: _LL21D: _LL21E: {
# 2395
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2393 "parse.y"
yyval=Cyc_YY40(({struct Cyc_List_List*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));({void*_tmpA17=Cyc_yyget_YY44(yyyyvsp[2]);_tmp5A0->hd=_tmpA17;}),({struct Cyc_List_List*_tmpA16=Cyc_yyget_YY40(yyyyvsp[0]);_tmp5A0->tl=_tmpA16;});_tmp5A0;}));
goto _LL0;}case 274U: _LL21F: _LL220: {
# 2396
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2398 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp735;({struct Cyc_List_List*_tmpA18=Cyc_yyget_YY26(yyyyvsp[0]);_tmp735.tms=_tmpA18;});_tmp735;}));
goto _LL0;}case 275U: _LL221: _LL222: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2400 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 276U: _LL223: _LL224: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2402 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp736;({struct Cyc_List_List*_tmpA1A=({struct Cyc_List_List*_tmpA19=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA19,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp736.tms=_tmpA1A;});_tmp736;}));
goto _LL0;}case 277U: _LL225: _LL226: {
# 2405
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2407
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2407 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 278U: _LL227: _LL228: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2409 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp737;({struct Cyc_List_List*_tmpA1D=({struct Cyc_List_List*_tmp5A2=_region_malloc(yyr,sizeof(*_tmp5A2));({void*_tmpA1C=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A1=_region_malloc(yyr,sizeof(*_tmp5A1));_tmp5A1->tag=0U,({void*_tmpA1B=Cyc_yyget_YY51(yyyyvsp[2]);_tmp5A1->f1=_tmpA1B;}),_tmp5A1->f2=(unsigned int)(yyyylsp[2]).first_line;_tmp5A1;});_tmp5A2->hd=_tmpA1C;}),_tmp5A2->tl=0;_tmp5A2;});_tmp737.tms=_tmpA1D;});_tmp737;}));
goto _LL0;}case 279U: _LL229: _LL22A: {
# 2412
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2414
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2411 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp738;({struct Cyc_List_List*_tmpA21=({struct Cyc_List_List*_tmp5A4=_region_malloc(yyr,sizeof(*_tmp5A4));({void*_tmpA20=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5A3=_region_malloc(yyr,sizeof(*_tmp5A3));_tmp5A3->tag=0U,({void*_tmpA1F=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5A3->f1=_tmpA1F;}),_tmp5A3->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5A3;});_tmp5A4->hd=_tmpA20;}),({struct Cyc_List_List*_tmpA1E=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5A4->tl=_tmpA1E;});_tmp5A4;});_tmp738.tms=_tmpA21;});_tmp738;}));
goto _LL0;}case 280U: _LL22B: _LL22C: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2413 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp739;({struct Cyc_List_List*_tmpA25=({struct Cyc_List_List*_tmp5A6=_region_malloc(yyr,sizeof(*_tmp5A6));({void*_tmpA24=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5A5=_region_malloc(yyr,sizeof(*_tmp5A5));_tmp5A5->tag=1U,({struct Cyc_Absyn_Exp*_tmpA23=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp5A5->f1=_tmpA23;}),({void*_tmpA22=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5A5->f2=_tmpA22;}),_tmp5A5->f3=(unsigned int)(yyyylsp[3]).first_line;_tmp5A5;});_tmp5A6->hd=_tmpA24;}),_tmp5A6->tl=0;_tmp5A6;});_tmp739.tms=_tmpA25;});_tmp739;}));
goto _LL0;}case 281U: _LL22D: _LL22E: {
# 2416
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2418
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2415 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73A;({struct Cyc_List_List*_tmpA2A=({struct Cyc_List_List*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));({void*_tmpA29=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));_tmp5A7->tag=1U,({struct Cyc_Absyn_Exp*_tmpA28=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5A7->f1=_tmpA28;}),({void*_tmpA27=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5A7->f2=_tmpA27;}),_tmp5A7->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5A7;});_tmp5A8->hd=_tmpA29;}),({
struct Cyc_List_List*_tmpA26=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5A8->tl=_tmpA26;});_tmp5A8;});
# 2415
_tmp73A.tms=_tmpA2A;});_tmp73A;}));
# 2418
goto _LL0;}case 282U: _LL22F: _LL230: {
# 2420
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2422
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2419 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73B;({struct Cyc_List_List*_tmpA31=({struct Cyc_List_List*_tmp5AB=_region_malloc(yyr,sizeof(*_tmp5AB));({void*_tmpA30=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5AA=_region_malloc(yyr,sizeof(*_tmp5AA));_tmp5AA->tag=3U,({void*_tmpA2F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A9=_region_malloc(yyr,sizeof(*_tmp5A9));_tmp5A9->tag=1U,_tmp5A9->f1=0,_tmp5A9->f2=0,_tmp5A9->f3=0,({void*_tmpA2E=Cyc_yyget_YY49(yyyyvsp[1]);_tmp5A9->f4=_tmpA2E;}),({struct Cyc_List_List*_tmpA2D=Cyc_yyget_YY50(yyyyvsp[2]);_tmp5A9->f5=_tmpA2D;}),({struct Cyc_Absyn_Exp*_tmpA2C=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5A9->f6=_tmpA2C;}),({struct Cyc_Absyn_Exp*_tmpA2B=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5A9->f7=_tmpA2B;});_tmp5A9;});_tmp5AA->f1=_tmpA2F;});_tmp5AA;});_tmp5AB->hd=_tmpA30;}),_tmp5AB->tl=0;_tmp5AB;});_tmp73B.tms=_tmpA31;});_tmp73B;}));
# 2421
goto _LL0;}case 283U: _LL231: _LL232: {
# 2423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2422 "parse.y"
struct _tuple26*_tmp5AC=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp5AD=_tmp5AC;struct Cyc_List_List*_tmp5B5;int _tmp5B4;struct Cyc_Absyn_VarargInfo*_tmp5B3;void*_tmp5B2;struct Cyc_List_List*_tmp5B1;_LL471: _tmp5B5=_tmp5AD->f1;_tmp5B4=_tmp5AD->f2;_tmp5B3=_tmp5AD->f3;_tmp5B2=_tmp5AD->f4;_tmp5B1=_tmp5AD->f5;_LL472:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73C;({struct Cyc_List_List*_tmpA36=({struct Cyc_List_List*_tmp5B0=_region_malloc(yyr,sizeof(*_tmp5B0));({void*_tmpA35=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));_tmp5AF->tag=3U,({void*_tmpA34=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));_tmp5AE->tag=1U,_tmp5AE->f1=_tmp5B5,_tmp5AE->f2=_tmp5B4,_tmp5AE->f3=_tmp5B3,_tmp5AE->f4=_tmp5B2,_tmp5AE->f5=_tmp5B1,({struct Cyc_Absyn_Exp*_tmpA33=Cyc_yyget_YY55(yyyyvsp[3]);_tmp5AE->f6=_tmpA33;}),({struct Cyc_Absyn_Exp*_tmpA32=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5AE->f7=_tmpA32;});_tmp5AE;});_tmp5AF->f1=_tmpA34;});_tmp5AF;});_tmp5B0->hd=_tmpA35;}),_tmp5B0->tl=0;_tmp5B0;});_tmp73C.tms=_tmpA36;});_tmp73C;}));
# 2425
goto _LL0;}case 284U: _LL233: _LL234: {
# 2427
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2429
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2426 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73D;({struct Cyc_List_List*_tmpA3E=({struct Cyc_List_List*_tmp5B8=_region_malloc(yyr,sizeof(*_tmp5B8));({void*_tmpA3D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B7=_region_malloc(yyr,sizeof(*_tmp5B7));_tmp5B7->tag=3U,({void*_tmpA3C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B6=_region_malloc(yyr,sizeof(*_tmp5B6));_tmp5B6->tag=1U,_tmp5B6->f1=0,_tmp5B6->f2=0,_tmp5B6->f3=0,({void*_tmpA3B=Cyc_yyget_YY49(yyyyvsp[2]);_tmp5B6->f4=_tmpA3B;}),({struct Cyc_List_List*_tmpA3A=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5B6->f5=_tmpA3A;}),({struct Cyc_Absyn_Exp*_tmpA39=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5B6->f6=_tmpA39;}),({struct Cyc_Absyn_Exp*_tmpA38=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5B6->f7=_tmpA38;});_tmp5B6;});_tmp5B7->f1=_tmpA3C;});_tmp5B7;});_tmp5B8->hd=_tmpA3D;}),({
struct Cyc_List_List*_tmpA37=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5B8->tl=_tmpA37;});_tmp5B8;});
# 2426
_tmp73D.tms=_tmpA3E;});_tmp73D;}));
# 2429
goto _LL0;}case 285U: _LL235: _LL236: {
# 2431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2430 "parse.y"
struct _tuple26*_tmp5B9=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5BA=_tmp5B9;struct Cyc_List_List*_tmp5C2;int _tmp5C1;struct Cyc_Absyn_VarargInfo*_tmp5C0;void*_tmp5BF;struct Cyc_List_List*_tmp5BE;_LL474: _tmp5C2=_tmp5BA->f1;_tmp5C1=_tmp5BA->f2;_tmp5C0=_tmp5BA->f3;_tmp5BF=_tmp5BA->f4;_tmp5BE=_tmp5BA->f5;_LL475:;
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73E;({struct Cyc_List_List*_tmpA44=({struct Cyc_List_List*_tmp5BD=_region_malloc(yyr,sizeof(*_tmp5BD));({void*_tmpA43=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5BC=_region_malloc(yyr,sizeof(*_tmp5BC));_tmp5BC->tag=3U,({void*_tmpA42=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5BB=_region_malloc(yyr,sizeof(*_tmp5BB));_tmp5BB->tag=1U,_tmp5BB->f1=_tmp5C2,_tmp5BB->f2=_tmp5C1,_tmp5BB->f3=_tmp5C0,_tmp5BB->f4=_tmp5BF,_tmp5BB->f5=_tmp5BE,({
struct Cyc_Absyn_Exp*_tmpA41=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5BB->f6=_tmpA41;}),({struct Cyc_Absyn_Exp*_tmpA40=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5BB->f7=_tmpA40;});_tmp5BB;});
# 2431
_tmp5BC->f1=_tmpA42;});_tmp5BC;});_tmp5BD->hd=_tmpA43;}),({
struct Cyc_List_List*_tmpA3F=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5BD->tl=_tmpA3F;});_tmp5BD;});
# 2431
_tmp73E.tms=_tmpA44;});_tmp73E;}));
# 2434
goto _LL0;}case 286U: _LL237: _LL238: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2436 "parse.y"
struct Cyc_List_List*_tmp5C3=({unsigned int _tmpA45=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA45,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp73F;({struct Cyc_List_List*_tmpA48=({struct Cyc_List_List*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));({void*_tmpA47=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4->tag=4U,_tmp5C4->f1=_tmp5C3,_tmp5C4->f2=(unsigned int)(yyyylsp[1]).first_line,_tmp5C4->f3=0;_tmp5C4;});_tmp5C5->hd=_tmpA47;}),({
struct Cyc_List_List*_tmpA46=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5C5->tl=_tmpA46;});_tmp5C5;});
# 2437
_tmp73F.tms=_tmpA48;});_tmp73F;}));
# 2440
goto _LL0;}case 287U: _LL239: _LL23A: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2441 "parse.y"
yyval=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp740;({struct Cyc_List_List*_tmpA4C=({struct Cyc_List_List*_tmp5C7=_region_malloc(yyr,sizeof(*_tmp5C7));({void*_tmpA4B=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));_tmp5C6->tag=5U,_tmp5C6->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA4A=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5C6->f2=_tmpA4A;});_tmp5C6;});_tmp5C7->hd=_tmpA4B;}),({struct Cyc_List_List*_tmpA49=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5C7->tl=_tmpA49;});_tmp5C7;});_tmp740.tms=_tmpA4C;});_tmp740;}));
# 2443
goto _LL0;}case 288U: _LL23B: _LL23C: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2447 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 289U: _LL23D: _LL23E: {
# 2450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2448 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 290U: _LL23F: _LL240: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2449 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 291U: _LL241: _LL242: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2450 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 292U: _LL243: _LL244: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2451 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 293U: _LL245: _LL246: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2452 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 294U: _LL247: _LL248: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
yyval=Cyc_Stmt_tok(({void*_tmpA50=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->tag=13U,({struct _dyneither_ptr*_tmpA4F=({struct _dyneither_ptr*_tmp5C8=_cycalloc(sizeof(*_tmp5C8));({struct _dyneither_ptr _tmpA4E=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5C8=_tmpA4E;});_tmp5C8;});_tmp5C9->f1=_tmpA4F;}),({struct Cyc_Absyn_Stmt*_tmpA4D=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5C9->f2=_tmpA4D;});_tmp5C9;});Cyc_Absyn_new_stmt(_tmpA50,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 295U: _LL249: _LL24A: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2462 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 296U: _LL24B: _LL24C: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2463 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA51=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpA51,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 297U: _LL24D: _LL24E: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2468 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 298U: _LL24F: _LL250: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2469 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 299U: _LL251: _LL252: {
# 2472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2474 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA52=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpA52,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));
goto _LL0;}case 300U: _LL253: _LL254: {
# 2477
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2479
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2475 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpA53=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpA53,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));
goto _LL0;}case 301U: _LL255: _LL256: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2476 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA58=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->tag=13U,({struct _dyneither_ptr*_tmpA57=({struct _dyneither_ptr*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));({struct _dyneither_ptr _tmpA56=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5CA=_tmpA56;});_tmp5CA;});_tmp5CB->f1=_tmpA57;}),({struct Cyc_Absyn_Stmt*_tmpA55=({struct Cyc_List_List*_tmpA54=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpA54,Cyc_Absyn_skip_stmt(0U));});_tmp5CB->f2=_tmpA55;});_tmp5CB;});Cyc_Absyn_new_stmt(_tmpA58,(unsigned int)(yyyylsp[0]).first_line);}));
# 2478
goto _LL0;}case 302U: _LL257: _LL258: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2478 "parse.y"
yyval=Cyc_Stmt_tok(({void*_tmpA5D=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->tag=13U,({struct _dyneither_ptr*_tmpA5C=({struct _dyneither_ptr*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));({struct _dyneither_ptr _tmpA5B=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5CC=_tmpA5B;});_tmp5CC;});_tmp5CD->f1=_tmpA5C;}),({struct Cyc_Absyn_Stmt*_tmpA5A=({struct Cyc_List_List*_tmpA59=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpA59,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp5CD->f2=_tmpA5A;});_tmp5CD;});Cyc_Absyn_new_stmt(_tmpA5D,(unsigned int)(yyyylsp[0]).first_line);}));
# 2480
goto _LL0;}case 303U: _LL259: _LL25A: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2480 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 304U: _LL25B: _LL25C: {
# 2483
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2485
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2481 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA5F=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpA5E=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpA5F,_tmpA5E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 305U: _LL25D: _LL25E: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2482 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA62=({void*_tmpA61=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA60=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5CE->f1=_tmpA60;});_tmp5CE;});Cyc_Absyn_new_decl(_tmpA61,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpA62,
Cyc_Absyn_skip_stmt(0U));}));
goto _LL0;}case 306U: _LL25F: _LL260: {
# 2486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2485 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpA65=({void*_tmpA64=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpA63=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5CF->f1=_tmpA63;});_tmp5CF;});Cyc_Absyn_new_decl(_tmpA64,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpA65,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));
goto _LL0;}case 307U: _LL261: _LL262: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2490 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA68=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA67=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA66=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpA68,_tmpA67,_tmpA66,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 308U: _LL263: _LL264: {
# 2493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2492 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA6B=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA6A=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA69=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpA6B,_tmpA6A,_tmpA69,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 309U: _LL265: _LL266: {
# 2495
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2497
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpA6C=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpA6D,_tmpA6C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 310U: _LL267: _LL268: {
# 2501
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2503
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2501 "parse.y"
struct Cyc_Absyn_Exp*_tmp5D0=({struct _tuple0*_tmpA6E=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpA6E,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA70=_tmp5D0;struct Cyc_List_List*_tmpA6F=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpA70,_tmpA6F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 311U: _LL269: _LL26A: {
# 2505
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2507
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2505 "parse.y"
struct Cyc_Absyn_Exp*_tmp5D1=({struct Cyc_List_List*_tmpA71=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpA71,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA73=_tmp5D1;struct Cyc_List_List*_tmpA72=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpA73,_tmpA72,(unsigned int)(yyyylsp[0]).first_line);}));
# 2508
goto _LL0;}case 312U: _LL26B: _LL26C: {
# 2510
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2512
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2511 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA75=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpA74=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpA75,_tmpA74,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 313U: _LL26D: _LL26E:
# 2514
 yyval=Cyc_YY8(0);
goto _LL0;case 314U: _LL26F: _LL270: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2529 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));({struct Cyc_Absyn_Switch_clause*_tmpA79=({struct Cyc_Absyn_Switch_clause*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));({struct Cyc_Absyn_Pat*_tmpA78=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp5D2->pattern=_tmpA78;}),_tmp5D2->pat_vars=0,_tmp5D2->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA77=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5D2->body=_tmpA77;}),_tmp5D2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5D2;});
# 2529
_tmp5D3->hd=_tmpA79;}),({
# 2531
struct Cyc_List_List*_tmpA76=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5D3->tl=_tmpA76;});_tmp5D3;}));
goto _LL0;}case 315U: _LL271: _LL272: {
# 2534
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2536
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2533 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));({struct Cyc_Absyn_Switch_clause*_tmpA7D=({struct Cyc_Absyn_Switch_clause*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));({struct Cyc_Absyn_Pat*_tmpA7C=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5D4->pattern=_tmpA7C;}),_tmp5D4->pat_vars=0,_tmp5D4->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpA7B=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp5D4->body=_tmpA7B;}),_tmp5D4->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5D4;});
# 2533
_tmp5D5->hd=_tmpA7D;}),({
# 2535
struct Cyc_List_List*_tmpA7A=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5D5->tl=_tmpA7A;});_tmp5D5;}));
goto _LL0;}case 316U: _LL273: _LL274: {
# 2538
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2540
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2537 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D7=_cycalloc(sizeof(*_tmp5D7));({struct Cyc_Absyn_Switch_clause*_tmpA81=({struct Cyc_Absyn_Switch_clause*_tmp5D6=_cycalloc(sizeof(*_tmp5D6));({struct Cyc_Absyn_Pat*_tmpA80=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5D6->pattern=_tmpA80;}),_tmp5D6->pat_vars=0,_tmp5D6->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpA7F=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp5D6->body=_tmpA7F;}),_tmp5D6->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5D6;});_tmp5D7->hd=_tmpA81;}),({struct Cyc_List_List*_tmpA7E=Cyc_yyget_YY8(yyyyvsp[4]);_tmp5D7->tl=_tmpA7E;});_tmp5D7;}));
goto _LL0;}case 317U: _LL275: _LL276: {
# 2540
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2539 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));({struct Cyc_Absyn_Switch_clause*_tmpA86=({struct Cyc_Absyn_Switch_clause*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));({struct Cyc_Absyn_Pat*_tmpA85=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5D8->pattern=_tmpA85;}),_tmp5D8->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA84=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5D8->where_clause=_tmpA84;}),({
struct Cyc_Absyn_Stmt*_tmpA83=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp5D8->body=_tmpA83;}),_tmp5D8->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5D8;});
# 2539
_tmp5D9->hd=_tmpA86;}),({
# 2541
struct Cyc_List_List*_tmpA82=Cyc_yyget_YY8(yyyyvsp[5]);_tmp5D9->tl=_tmpA82;});_tmp5D9;}));
goto _LL0;}case 318U: _LL277: _LL278: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2543 "parse.y"
yyval=Cyc_YY8(({struct Cyc_List_List*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));({struct Cyc_Absyn_Switch_clause*_tmpA8B=({struct Cyc_Absyn_Switch_clause*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));({struct Cyc_Absyn_Pat*_tmpA8A=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5DA->pattern=_tmpA8A;}),_tmp5DA->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpA89=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5DA->where_clause=_tmpA89;}),({struct Cyc_Absyn_Stmt*_tmpA88=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp5DA->body=_tmpA88;}),_tmp5DA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5DA;});_tmp5DB->hd=_tmpA8B;}),({struct Cyc_List_List*_tmpA87=Cyc_yyget_YY8(yyyyvsp[6]);_tmp5DB->tl=_tmpA87;});_tmp5DB;}));
goto _LL0;}case 319U: _LL279: _LL27A: {
# 2546
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2548
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2550 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA8D=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpA8C=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpA8D,_tmpA8C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2553
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2555
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2554 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpA8F=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpA8E=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpA8F,_tmpA8E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2557
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2558 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA93=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA92=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA91=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA90=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2558
Cyc_Absyn_for_stmt(_tmpA93,_tmpA92,_tmpA91,_tmpA90,(unsigned int)(yyyylsp[0]).first_line);}));
# 2560
goto _LL0;}case 322U: _LL27F: _LL280: {
# 2562
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2561 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA97=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA96=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpA95=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpA94=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2561
Cyc_Absyn_for_stmt(_tmpA97,_tmpA96,_tmpA95,_tmpA94,(unsigned int)(yyyylsp[0]).first_line);}));
# 2563
goto _LL0;}case 323U: _LL281: _LL282: {
# 2565
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2567
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2564 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA9A=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA99=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpA98=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2564
Cyc_Absyn_for_stmt(_tmpA9B,_tmpA9A,_tmpA99,_tmpA98,(unsigned int)(yyyylsp[0]).first_line);}));
# 2566
goto _LL0;}case 324U: _LL283: _LL284: {
# 2568
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2567 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpA9F=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpA9E=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpA9D=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpA9C=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2567
Cyc_Absyn_for_stmt(_tmpA9F,_tmpA9E,_tmpA9D,_tmpA9C,(unsigned int)(yyyylsp[0]).first_line);}));
# 2569
goto _LL0;}case 325U: _LL285: _LL286: {
# 2571
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2573
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2570 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA3=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpAA2=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAA0=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2570
Cyc_Absyn_for_stmt(_tmpAA3,_tmpAA2,_tmpAA1,_tmpAA0,(unsigned int)(yyyylsp[0]).first_line);}));
# 2572
goto _LL0;}case 326U: _LL287: _LL288: {
# 2574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2573 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAA7=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpAA6=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAA5=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpAA4=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2573
Cyc_Absyn_for_stmt(_tmpAA7,_tmpAA6,_tmpAA5,_tmpAA4,(unsigned int)(yyyylsp[0]).first_line);}));
# 2575
goto _LL0;}case 327U: _LL289: _LL28A: {
# 2577
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2579
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2576 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAAB=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpAAA=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpAA9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAA8=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2576
Cyc_Absyn_for_stmt(_tmpAAB,_tmpAAA,_tmpAA9,_tmpAA8,(unsigned int)(yyyylsp[0]).first_line);}));
# 2578
goto _LL0;}case 328U: _LL28B: _LL28C: {
# 2580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 2579 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAAF=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpAAE=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpAAD=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpAAC=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2579
Cyc_Absyn_for_stmt(_tmpAAF,_tmpAAE,_tmpAAD,_tmpAAC,(unsigned int)(yyyylsp[0]).first_line);}));
# 2581
goto _LL0;}case 329U: _LL28D: _LL28E: {
# 2583
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2585
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2582 "parse.y"
struct Cyc_List_List*_tmp5DC=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5DD=({struct Cyc_Absyn_Exp*_tmpAB3=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAB2=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAB1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAB0=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2583
Cyc_Absyn_for_stmt(_tmpAB3,_tmpAB2,_tmpAB1,_tmpAB0,(unsigned int)(yyyylsp[0]).first_line);});
# 2585
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5DC,_tmp5DD));
# 2587
goto _LL0;}case 330U: _LL28F: _LL290: {
# 2589
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2591
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2588 "parse.y"
struct Cyc_List_List*_tmp5DE=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5DF=({struct Cyc_Absyn_Exp*_tmpAB7=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpAB6=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpAB5=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpAB4=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2589
Cyc_Absyn_for_stmt(_tmpAB7,_tmpAB6,_tmpAB5,_tmpAB4,(unsigned int)(yyyylsp[0]).first_line);});
# 2591
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5DE,_tmp5DF));
# 2593
goto _LL0;}case 331U: _LL291: _LL292: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2594 "parse.y"
struct Cyc_List_List*_tmp5E0=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5E1=({struct Cyc_Absyn_Exp*_tmpABB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpABA=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpAB9=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpAB8=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2595
Cyc_Absyn_for_stmt(_tmpABB,_tmpABA,_tmpAB9,_tmpAB8,(unsigned int)(yyyylsp[0]).first_line);});
# 2597
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5E0,_tmp5E1));
# 2599
goto _LL0;}case 332U: _LL293: _LL294: {
# 2601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2600 "parse.y"
struct Cyc_List_List*_tmp5E2=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5E3=({struct Cyc_Absyn_Exp*_tmpABF=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpABE=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpABD=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpABC=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2601
Cyc_Absyn_for_stmt(_tmpABF,_tmpABE,_tmpABD,_tmpABC,(unsigned int)(yyyylsp[0]).first_line);});
# 2603
yyval=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5E2,_tmp5E3));
# 2605
goto _LL0;}case 333U: _LL295: _LL296: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
yyval=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpAC1=({struct _dyneither_ptr*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));({struct _dyneither_ptr _tmpAC0=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp5E4=_tmpAC0;});_tmp5E4;});Cyc_Absyn_goto_stmt(_tmpAC1,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 334U: _LL297: _LL298: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2611 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 335U: _LL299: _LL29A: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2612 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 336U: _LL29B: _LL29C: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2613 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 337U: _LL29D: _LL29E: {
# 2616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2614 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpAC2,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 338U: _LL29F: _LL2A0: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2616 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2617 "parse.y"
yyval=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2619 "parse.y"
yyval=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpAC3=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpAC3,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2628 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2631 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 343U: _LL2A9: _LL2AA: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2633 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC6=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC5=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpAC4=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpAC6,_tmpAC5,_tmpAC4,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 344U: _LL2AB: _LL2AC: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2636 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 345U: _LL2AD: _LL2AE: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2638 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpAC8,_tmpAC7,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 346U: _LL2AF: _LL2B0: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2641 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 347U: _LL2B1: _LL2B2: {
# 2644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2643 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAC9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpACA,_tmpAC9,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 348U: _LL2B3: _LL2B4: {
# 2646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2646 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 349U: _LL2B5: _LL2B6: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2648 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACC=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpACC,_tmpACB,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 350U: _LL2B7: _LL2B8: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2651 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 351U: _LL2B9: _LL2BA: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2653 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpACE=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACD=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpACE,_tmpACD,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 352U: _LL2BB: _LL2BC: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2656 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 353U: _LL2BD: _LL2BE: {
# 2659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2658 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD0=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpACF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpAD0,_tmpACF,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 354U: _LL2BF: _LL2C0: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2661 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 355U: _LL2C1: _LL2C2: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2663 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpAD2,_tmpAD1,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 356U: _LL2C3: _LL2C4: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2665 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD4=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpAD4,_tmpAD3,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 357U: _LL2C5: _LL2C6: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2668 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 358U: _LL2C7: _LL2C8: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2670 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD6=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD5=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpAD6,_tmpAD5,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 359U: _LL2C9: _LL2CA: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2672 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpAD8,_tmpAD7,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 360U: _LL2CB: _LL2CC: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2674 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpADA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAD9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpADA,_tmpAD9,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 361U: _LL2CD: _LL2CE: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2676 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpADC=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpADB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpADC,_tmpADB,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 362U: _LL2CF: _LL2D0: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2679 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 363U: _LL2D1: _LL2D2: {
# 2682
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2684
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2681 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpADE=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpADD=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpADE,_tmpADD,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 364U: _LL2D3: _LL2D4: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2683 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAE0=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpADF=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpAE0,_tmpADF,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 365U: _LL2D5: _LL2D6: {
# 2686
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2688
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2686 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 366U: _LL2D7: _LL2D8: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2688 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAE2=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAE1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpAE2,_tmpAE1,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 367U: _LL2D9: _LL2DA: {
# 2691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2690 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAE4=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAE3=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpAE4,_tmpAE3,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 368U: _LL2DB: _LL2DC: {
# 2693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2693 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 369U: _LL2DD: _LL2DE: {
# 2696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2695 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAE6=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAE5=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpAE6,_tmpAE5,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 370U: _LL2DF: _LL2E0: {
# 2698
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2700
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2697 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAE8=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAE7=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpAE8,_tmpAE7,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 371U: _LL2E1: _LL2E2: {
# 2700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2699 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAEA=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpAE9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpAEA,_tmpAE9,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 372U: _LL2E3: _LL2E4: {
# 2702
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2704
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2702 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 373U: _LL2E5: _LL2E6: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2704 "parse.y"
void*_tmp5E5=({struct _tuple8*_tmpAEB=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpAEB,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpAED=_tmp5E5;struct Cyc_Absyn_Exp*_tmpAEC=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpAED,_tmpAEC,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));
# 2707
goto _LL0;}case 374U: _LL2E7: _LL2E8: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2710 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 375U: _LL2E9: _LL2EA: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2713 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 376U: _LL2EB: _LL2EC: {
# 2716
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2718
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2715 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpAEF=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpAEE=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpAEF,_tmpAEE,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 377U: _LL2ED: _LL2EE: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2717 "parse.y"
void*_tmp5E6=({struct _tuple8*_tmpAF0=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpAF0,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(_tmp5E6,(unsigned int)(yyyylsp[0]).first_line)));
# 2720
goto _LL0;}case 378U: _LL2EF: _LL2F0: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2721 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpAF1=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpAF1,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 379U: _LL2F1: _LL2F2: {
# 2724
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2726
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2723 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpAF3=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpAF2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpAF3,_tmpAF2,(unsigned int)(yyyylsp[0]).first_line);})));
goto _LL0;}case 380U: _LL2F3: _LL2F4: {
# 2726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2728 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 381U: _LL2F5: _LL2F6: {
# 2731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2736 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 382U: _LL2F7: _LL2F8: {
# 2739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2741 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 383U: _LL2F9: _LL2FA: {
# 2744
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2746
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2743 "parse.y"
yyval=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));
goto _LL0;}case 384U: _LL2FB: _LL2FC: {
# 2746
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2748
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2745 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp5E7=e->r;void*_tmp5E8=_tmp5E7;struct _dyneither_ptr _tmp5FC;int _tmp5FB;enum Cyc_Absyn_Sign _tmp5FA;int _tmp5F9;enum Cyc_Absyn_Sign _tmp5F8;short _tmp5F7;enum Cyc_Absyn_Sign _tmp5F6;char _tmp5F5;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).LongLong_c).tag){case 2U: _LL477: _tmp5F6=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Char_c).val).f1;_tmp5F5=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Char_c).val).f2;_LL478:
# 2749
 yyval=Cyc_YY9(({void*_tmpAF4=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));_tmp5E9->tag=11U,_tmp5E9->f1=_tmp5F5;_tmp5E9;});Cyc_Absyn_new_pat(_tmpAF4,e->loc);}));goto _LL476;case 4U: _LL479: _tmp5F8=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Short_c).val).f1;_tmp5F7=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Short_c).val).f2;_LL47A:
# 2751
 yyval=Cyc_YY9(({void*_tmpAF5=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->tag=10U,_tmp5EA->f1=_tmp5F8,_tmp5EA->f2=(int)_tmp5F7;_tmp5EA;});Cyc_Absyn_new_pat(_tmpAF5,e->loc);}));goto _LL476;case 5U: _LL47B: _tmp5FA=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Int_c).val).f1;_tmp5F9=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Int_c).val).f2;_LL47C:
# 2753
 yyval=Cyc_YY9(({void*_tmpAF6=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->tag=10U,_tmp5EB->f1=_tmp5FA,_tmp5EB->f2=_tmp5F9;_tmp5EB;});Cyc_Absyn_new_pat(_tmpAF6,e->loc);}));goto _LL476;case 7U: _LL47D: _tmp5FC=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Float_c).val).f1;_tmp5FB=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E8)->f1).Float_c).val).f2;_LL47E:
# 2755
 yyval=Cyc_YY9(({void*_tmpAF7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->tag=12U,_tmp5EC->f1=_tmp5FC,_tmp5EC->f2=_tmp5FB;_tmp5EC;});Cyc_Absyn_new_pat(_tmpAF7,e->loc);}));goto _LL476;case 1U: _LL47F: _LL480:
# 2757
 yyval=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));goto _LL476;case 8U: _LL481: _LL482:
# 2759
({void*_tmp5ED=0U;({unsigned int _tmpAF9=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAF8=({const char*_tmp5EE="strings cannot occur within patterns";_tag_dyneither(_tmp5EE,sizeof(char),37U);});Cyc_Warn_err(_tmpAF9,_tmpAF8,_tag_dyneither(_tmp5ED,sizeof(void*),0U));});});goto _LL476;case 9U: _LL483: _LL484:
# 2761
({void*_tmp5EF=0U;({unsigned int _tmpAFB=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAFA=({const char*_tmp5F0="strings cannot occur within patterns";_tag_dyneither(_tmp5F0,sizeof(char),37U);});Cyc_Warn_err(_tmpAFB,_tmpAFA,_tag_dyneither(_tmp5EF,sizeof(void*),0U));});});goto _LL476;case 6U: _LL485: _LL486:
# 2763
({void*_tmp5F1=0U;({unsigned int _tmpAFD=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAFC=({const char*_tmp5F2="long long's in patterns not yet implemented";_tag_dyneither(_tmp5F2,sizeof(char),44U);});Cyc_Warn_err(_tmpAFD,_tmpAFC,_tag_dyneither(_tmp5F1,sizeof(void*),0U));});});goto _LL476;default: goto _LL487;}else{_LL487: _LL488:
# 2765
({void*_tmp5F3=0U;({unsigned int _tmpAFF=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAFE=({const char*_tmp5F4="bad constant in case";_tag_dyneither(_tmp5F4,sizeof(char),21U);});Cyc_Warn_err(_tmpAFF,_tmpAFE,_tag_dyneither(_tmp5F3,sizeof(void*),0U));});});}_LL476:;}
# 2768
goto _LL0;}case 385U: _LL2FD: _LL2FE: {
# 2770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2769 "parse.y"
yyval=Cyc_YY9(({void*_tmpB01=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->tag=15U,({struct _tuple0*_tmpB00=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5FD->f1=_tmpB00;});_tmp5FD;});Cyc_Absyn_new_pat(_tmpB01,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 386U: _LL2FF: _LL300: {
# 2772
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2774
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2771 "parse.y"
if(({struct _dyneither_ptr _tmpB02=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpB02,({const char*_tmp5FE="as";_tag_dyneither(_tmp5FE,sizeof(char),3U);}));})!= 0)
({void*_tmp5FF=0U;({unsigned int _tmpB04=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB03=({const char*_tmp600="expecting `as'";_tag_dyneither(_tmp600,sizeof(char),15U);});Cyc_Warn_err(_tmpB04,_tmpB03,_tag_dyneither(_tmp5FF,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB0B=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpB0A=({unsigned int _tmpB09=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB08=({struct _tuple0*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB07=({struct _dyneither_ptr*_tmp601=_cycalloc(sizeof(*_tmp601));({struct _dyneither_ptr _tmpB06=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp601=_tmpB06;});_tmp601;});_tmp602->f2=_tmpB07;});_tmp602;});Cyc_Absyn_new_vardecl(_tmpB09,_tmpB08,Cyc_Absyn_void_type,0);});_tmp603->f1=_tmpB0A;}),({
struct Cyc_Absyn_Pat*_tmpB05=Cyc_yyget_YY9(yyyyvsp[2]);_tmp603->f2=_tmpB05;});_tmp603;});
# 2773
Cyc_Absyn_new_pat(_tmpB0B,(unsigned int)(yyyylsp[0]).first_line);}));
# 2776
goto _LL0;}case 387U: _LL301: _LL302: {
# 2778
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2777 "parse.y"
if(({struct _dyneither_ptr _tmpB0C=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpB0C,({const char*_tmp604="alias";_tag_dyneither(_tmp604,sizeof(char),6U);}));})!= 0)
({void*_tmp605=0U;({unsigned int _tmpB0E=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB0D=({const char*_tmp606="expecting `alias'";_tag_dyneither(_tmp606,sizeof(char),18U);});Cyc_Warn_err(_tmpB0E,_tmpB0D,_tag_dyneither(_tmp605,sizeof(void*),0U));});});{
int _tmp607=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp60D=_cycalloc(sizeof(*_tmp60D));({struct _dyneither_ptr*_tmpB11=({struct _dyneither_ptr*_tmp60B=_cycalloc(sizeof(*_tmp60B));({struct _dyneither_ptr _tmpB10=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp60B=_tmpB10;});_tmp60B;});_tmp60D->name=_tmpB11;}),_tmp60D->identity=- 1,({void*_tmpB0F=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=0U,_tmp60C->f1=& Cyc_Tcutil_rk;_tmp60C;});_tmp60D->kind=_tmpB0F;});_tmp60D;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpB16=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB15=({struct _tuple0*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB13=({struct _dyneither_ptr*_tmp609=_cycalloc(sizeof(*_tmp609));({struct _dyneither_ptr _tmpB12=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp609=_tmpB12;});_tmp609;});_tmp60A->f2=_tmpB13;});_tmp60A;});Cyc_Absyn_new_vardecl(_tmpB16,_tmpB15,({
struct _tuple8*_tmpB14=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpB14,(unsigned int)(yyyylsp[4]).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB17=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->tag=2U,_tmp608->f1=tv,_tmp608->f2=vd;_tmp608;});Cyc_Absyn_new_pat(_tmpB17,(unsigned int)_tmp607);}));
# 2785
goto _LL0;};}case 388U: _LL303: _LL304: {
# 2787
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2789
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2786 "parse.y"
if(({struct _dyneither_ptr _tmpB18=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpB18,({const char*_tmp60E="alias";_tag_dyneither(_tmp60E,sizeof(char),6U);}));})!= 0)
({void*_tmp60F=0U;({unsigned int _tmpB1A=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB19=({const char*_tmp610="expecting `alias'";_tag_dyneither(_tmp610,sizeof(char),18U);});Cyc_Warn_err(_tmpB1A,_tmpB19,_tag_dyneither(_tmp60F,sizeof(void*),0U));});});{
int _tmp611=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp617=_cycalloc(sizeof(*_tmp617));({struct _dyneither_ptr*_tmpB1D=({struct _dyneither_ptr*_tmp615=_cycalloc(sizeof(*_tmp615));({struct _dyneither_ptr _tmpB1C=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp615=_tmpB1C;});_tmp615;});_tmp617->name=_tmpB1D;}),_tmp617->identity=- 1,({void*_tmpB1B=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->tag=0U,_tmp616->f1=& Cyc_Tcutil_rk;_tmp616;});_tmp617->kind=_tmpB1B;});_tmp617;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpB22=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB21=({struct _tuple0*_tmp614=_cycalloc(sizeof(*_tmp614));_tmp614->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB1F=({struct _dyneither_ptr*_tmp613=_cycalloc(sizeof(*_tmp613));({struct _dyneither_ptr _tmpB1E=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp613=_tmpB1E;});_tmp613;});_tmp614->f2=_tmpB1F;});_tmp614;});Cyc_Absyn_new_vardecl(_tmpB22,_tmpB21,({
struct _tuple8*_tmpB20=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpB20,(unsigned int)(yyyylsp[4]).first_line);}),0);});
yyval=Cyc_YY9(({void*_tmpB23=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->tag=2U,_tmp612->f1=tv,_tmp612->f2=vd;_tmp612;});Cyc_Absyn_new_pat(_tmpB23,(unsigned int)_tmp611);}));
# 2794
goto _LL0;};}case 389U: _LL305: _LL306: {
# 2796
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2795 "parse.y"
struct _tuple22 _tmp618=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp619=_tmp618;struct Cyc_List_List*_tmp61C;int _tmp61B;_LL48A: _tmp61C=_tmp619.f1;_tmp61B=_tmp619.f2;_LL48B:;
yyval=Cyc_YY9(({void*_tmpB24=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A->tag=5U,_tmp61A->f1=_tmp61C,_tmp61A->f2=_tmp61B;_tmp61A;});Cyc_Absyn_new_pat(_tmpB24,(unsigned int)(yyyylsp[0]).first_line);}));
# 2798
goto _LL0;}case 390U: _LL307: _LL308: {
# 2800
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2802
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2799 "parse.y"
struct _tuple22 _tmp61D=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp61E=_tmp61D;struct Cyc_List_List*_tmp621;int _tmp620;_LL48D: _tmp621=_tmp61E.f1;_tmp620=_tmp61E.f2;_LL48E:;
yyval=Cyc_YY9(({void*_tmpB26=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->tag=16U,({struct _tuple0*_tmpB25=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp61F->f1=_tmpB25;}),_tmp61F->f2=_tmp621,_tmp61F->f3=_tmp620;_tmp61F;});Cyc_Absyn_new_pat(_tmpB26,(unsigned int)(yyyylsp[0]).first_line);}));
# 2802
goto _LL0;}case 391U: _LL309: _LL30A: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2803 "parse.y"
struct _tuple22 _tmp622=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp623=_tmp622;struct Cyc_List_List*_tmp628;int _tmp627;_LL490: _tmp628=_tmp623.f1;_tmp627=_tmp623.f2;_LL491:;{
struct Cyc_List_List*_tmp624=({unsigned int _tmpB27=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB27,Cyc_yyget_YY40(yyyyvsp[2]));});
yyval=Cyc_YY9(({void*_tmpB2A=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp626=_cycalloc(sizeof(*_tmp626));_tmp626->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpB29=({union Cyc_Absyn_AggrInfo*_tmp625=_cycalloc(sizeof(*_tmp625));({union Cyc_Absyn_AggrInfo _tmpB28=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);*_tmp625=_tmpB28;});_tmp625;});_tmp626->f1=_tmpB29;}),_tmp626->f2=_tmp624,_tmp626->f3=_tmp628,_tmp626->f4=_tmp627;_tmp626;});Cyc_Absyn_new_pat(_tmpB2A,(unsigned int)(yyyylsp[0]).first_line);}));
# 2808
goto _LL0;};}case 392U: _LL30B: _LL30C: {
# 2810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2809 "parse.y"
struct _tuple22 _tmp629=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp62A=_tmp629;struct Cyc_List_List*_tmp62E;int _tmp62D;_LL493: _tmp62E=_tmp62A.f1;_tmp62D=_tmp62A.f2;_LL494:;{
struct Cyc_List_List*_tmp62B=({unsigned int _tmpB2B=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB2B,Cyc_yyget_YY40(yyyyvsp[1]));});
yyval=Cyc_YY9(({void*_tmpB2C=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=7U,_tmp62C->f1=0,_tmp62C->f2=_tmp62B,_tmp62C->f3=_tmp62E,_tmp62C->f4=_tmp62D;_tmp62C;});Cyc_Absyn_new_pat(_tmpB2C,(unsigned int)(yyyylsp[0]).first_line);}));
# 2813
goto _LL0;};}case 393U: _LL30D: _LL30E: {
# 2815
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2817
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2814 "parse.y"
yyval=Cyc_YY9(({void*_tmpB2E=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=6U,({struct Cyc_Absyn_Pat*_tmpB2D=Cyc_yyget_YY9(yyyyvsp[1]);_tmp62F->f1=_tmpB2D;});_tmp62F;});Cyc_Absyn_new_pat(_tmpB2E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 394U: _LL30F: _LL310: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2816 "parse.y"
yyval=Cyc_YY9(({void*_tmpB32=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->tag=6U,({struct Cyc_Absyn_Pat*_tmpB31=({void*_tmpB30=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630->tag=6U,({struct Cyc_Absyn_Pat*_tmpB2F=Cyc_yyget_YY9(yyyyvsp[1]);_tmp630->f1=_tmpB2F;});_tmp630;});Cyc_Absyn_new_pat(_tmpB30,(unsigned int)(yyyylsp[0]).first_line);});_tmp631->f1=_tmpB31;});_tmp631;});Cyc_Absyn_new_pat(_tmpB32,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 395U: _LL311: _LL312: {
# 2819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2818 "parse.y"
yyval=Cyc_YY9(({void*_tmpB39=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB38=({unsigned int _tmpB37=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB36=({struct _tuple0*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB35=({struct _dyneither_ptr*_tmp632=_cycalloc(sizeof(*_tmp632));({struct _dyneither_ptr _tmpB34=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp632=_tmpB34;});_tmp632;});_tmp633->f2=_tmpB35;});_tmp633;});Cyc_Absyn_new_vardecl(_tmpB37,_tmpB36,Cyc_Absyn_void_type,0);});_tmp634->f1=_tmpB38;}),({
# 2820
struct Cyc_Absyn_Pat*_tmpB33=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp634->f2=_tmpB33;});_tmp634;});
# 2818
Cyc_Absyn_new_pat(_tmpB39,(unsigned int)(yyyylsp[0]).first_line);}));
# 2822
goto _LL0;}case 396U: _LL313: _LL314: {
# 2824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2823 "parse.y"
if(({struct _dyneither_ptr _tmpB3A=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpB3A,({const char*_tmp635="as";_tag_dyneither(_tmp635,sizeof(char),3U);}));})!= 0)
({void*_tmp636=0U;({unsigned int _tmpB3C=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpB3B=({const char*_tmp637="expecting `as'";_tag_dyneither(_tmp637,sizeof(char),15U);});Cyc_Warn_err(_tmpB3C,_tmpB3B,_tag_dyneither(_tmp636,sizeof(void*),0U));});});
yyval=Cyc_YY9(({void*_tmpB43=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpB42=({unsigned int _tmpB41=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB40=({struct _tuple0*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB3F=({struct _dyneither_ptr*_tmp638=_cycalloc(sizeof(*_tmp638));({struct _dyneither_ptr _tmpB3E=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp638=_tmpB3E;});_tmp638;});_tmp639->f2=_tmpB3F;});_tmp639;});Cyc_Absyn_new_vardecl(_tmpB41,_tmpB40,Cyc_Absyn_void_type,0);});_tmp63A->f1=_tmpB42;}),({
# 2827
struct Cyc_Absyn_Pat*_tmpB3D=Cyc_yyget_YY9(yyyyvsp[3]);_tmp63A->f2=_tmpB3D;});_tmp63A;});
# 2825
Cyc_Absyn_new_pat(_tmpB43,(unsigned int)(yyyylsp[0]).first_line);}));
# 2829
goto _LL0;}case 397U: _LL315: _LL316: {
# 2831
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2833
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2830 "parse.y"
void*_tmp63B=({struct _dyneither_ptr _tmpB44=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpB44,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
yyval=Cyc_YY9(({void*_tmpB4B=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->tag=4U,({struct Cyc_Absyn_Tvar*_tmpB4A=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp63B);_tmp63E->f1=_tmpB4A;}),({
struct Cyc_Absyn_Vardecl*_tmpB49=({unsigned int _tmpB48=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB47=({struct _tuple0*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB46=({struct _dyneither_ptr*_tmp63C=_cycalloc(sizeof(*_tmp63C));({struct _dyneither_ptr _tmpB45=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp63C=_tmpB45;});_tmp63C;});_tmp63D->f2=_tmpB46;});_tmp63D;});Cyc_Absyn_new_vardecl(_tmpB48,_tmpB47,
Cyc_Absyn_tag_type(_tmp63B),0);});
# 2832
_tmp63E->f2=_tmpB49;});_tmp63E;});
# 2831
Cyc_Absyn_new_pat(_tmpB4B,(unsigned int)(yyyylsp[0]).first_line);}));
# 2835
goto _LL0;}case 398U: _LL317: _LL318: {
# 2837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2836 "parse.y"
struct Cyc_Absyn_Tvar*_tmp63F=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
yyval=Cyc_YY9(({void*_tmpB51=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642->tag=4U,_tmp642->f1=_tmp63F,({
struct Cyc_Absyn_Vardecl*_tmpB50=({unsigned int _tmpB4F=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpB4E=({struct _tuple0*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB4D=({struct _dyneither_ptr*_tmp640=_cycalloc(sizeof(*_tmp640));({struct _dyneither_ptr _tmpB4C=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp640=_tmpB4C;});_tmp640;});_tmp641->f2=_tmpB4D;});_tmp641;});Cyc_Absyn_new_vardecl(_tmpB4F,_tmpB4E,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(_tmp63F)),0);});
# 2838
_tmp642->f2=_tmpB50;});_tmp642;});
# 2837
Cyc_Absyn_new_pat(_tmpB51,(unsigned int)(yyyylsp[0]).first_line);}));
# 2841
goto _LL0;}case 399U: _LL319: _LL31A: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2844 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp643=_cycalloc(sizeof(*_tmp643));({struct Cyc_List_List*_tmpB52=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp643->f1=_tmpB52;}),_tmp643->f2=0;_tmp643;}));
goto _LL0;}case 400U: _LL31B: _LL31C: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2845 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp644=_cycalloc(sizeof(*_tmp644));({struct Cyc_List_List*_tmpB53=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp644->f1=_tmpB53;}),_tmp644->f2=1;_tmp644;}));
goto _LL0;}case 401U: _LL31D: _LL31E: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2846 "parse.y"
yyval=Cyc_YY10(({struct _tuple22*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->f1=0,_tmp645->f2=1;_tmp645;}));
goto _LL0;}case 402U: _LL31F: _LL320: {
# 2849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2851 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));({struct Cyc_Absyn_Pat*_tmpB54=Cyc_yyget_YY9(yyyyvsp[0]);_tmp646->hd=_tmpB54;}),_tmp646->tl=0;_tmp646;}));
goto _LL0;}case 403U: _LL321: _LL322: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2853 "parse.y"
yyval=Cyc_YY11(({struct Cyc_List_List*_tmp647=_cycalloc(sizeof(*_tmp647));({struct Cyc_Absyn_Pat*_tmpB56=Cyc_yyget_YY9(yyyyvsp[2]);_tmp647->hd=_tmpB56;}),({struct Cyc_List_List*_tmpB55=Cyc_yyget_YY11(yyyyvsp[0]);_tmp647->tl=_tmpB55;});_tmp647;}));
goto _LL0;}case 404U: _LL323: _LL324: {
# 2856
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2858
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2858 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->f1=0,({struct Cyc_Absyn_Pat*_tmpB57=Cyc_yyget_YY9(yyyyvsp[0]);_tmp648->f2=_tmpB57;});_tmp648;}));
goto _LL0;}case 405U: _LL325: _LL326: {
# 2861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2860 "parse.y"
yyval=Cyc_YY12(({struct _tuple23*_tmp649=_cycalloc(sizeof(*_tmp649));({struct Cyc_List_List*_tmpB59=Cyc_yyget_YY41(yyyyvsp[0]);_tmp649->f1=_tmpB59;}),({struct Cyc_Absyn_Pat*_tmpB58=Cyc_yyget_YY9(yyyyvsp[1]);_tmp649->f2=_tmpB58;});_tmp649;}));
goto _LL0;}case 406U: _LL327: _LL328: {
# 2863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2863 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp64A=_cycalloc(sizeof(*_tmp64A));({struct Cyc_List_List*_tmpB5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp64A->f1=_tmpB5A;}),_tmp64A->f2=0;_tmp64A;}));
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2864 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp64B=_cycalloc(sizeof(*_tmp64B));({struct Cyc_List_List*_tmpB5B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp64B->f1=_tmpB5B;}),_tmp64B->f2=1;_tmp64B;}));
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2865 "parse.y"
yyval=Cyc_YY14(({struct _tuple22*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->f1=0,_tmp64C->f2=1;_tmp64C;}));
goto _LL0;}case 409U: _LL32D: _LL32E: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2870 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct _tuple23*_tmpB5C=Cyc_yyget_YY12(yyyyvsp[0]);_tmp64D->hd=_tmpB5C;}),_tmp64D->tl=0;_tmp64D;}));
goto _LL0;}case 410U: _LL32F: _LL330: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2872 "parse.y"
yyval=Cyc_YY13(({struct Cyc_List_List*_tmp64E=_cycalloc(sizeof(*_tmp64E));({struct _tuple23*_tmpB5E=Cyc_yyget_YY12(yyyyvsp[2]);_tmp64E->hd=_tmpB5E;}),({struct Cyc_List_List*_tmpB5D=Cyc_yyget_YY13(yyyyvsp[0]);_tmp64E->tl=_tmpB5D;});_tmp64E;}));
goto _LL0;}case 411U: _LL331: _LL332: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 412U: _LL333: _LL334: {
# 2881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2880 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB60=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB5F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpB60,_tmpB5F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 413U: _LL335: _LL336: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2885 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 414U: _LL337: _LL338: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2887 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB63=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpB62=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpB61=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpB63,_tmpB62,_tmpB61,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 415U: _LL339: _LL33A: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2889 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB65=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB64=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpB65,_tmpB64,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 416U: _LL33B: _LL33C: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2893 "parse.y"
yyval=Cyc_YY7(0);
goto _LL0;}case 417U: _LL33D: _LL33E: {
# 2896
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2898
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2894 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->v=(void*)Cyc_Absyn_Times;_tmp64F;}));
goto _LL0;}case 418U: _LL33F: _LL340: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2895 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->v=(void*)Cyc_Absyn_Div;_tmp650;}));
goto _LL0;}case 419U: _LL341: _LL342: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2896 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->v=(void*)Cyc_Absyn_Mod;_tmp651;}));
goto _LL0;}case 420U: _LL343: _LL344: {
# 2899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2897 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->v=(void*)Cyc_Absyn_Plus;_tmp652;}));
goto _LL0;}case 421U: _LL345: _LL346: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2898 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->v=(void*)Cyc_Absyn_Minus;_tmp653;}));
goto _LL0;}case 422U: _LL347: _LL348: {
# 2901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2899 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->v=(void*)Cyc_Absyn_Bitlshift;_tmp654;}));
goto _LL0;}case 423U: _LL349: _LL34A: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2900 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->v=(void*)Cyc_Absyn_Bitlrshift;_tmp655;}));
goto _LL0;}case 424U: _LL34B: _LL34C: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2901 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->v=(void*)Cyc_Absyn_Bitand;_tmp656;}));
goto _LL0;}case 425U: _LL34D: _LL34E: {
# 2904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2902 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->v=(void*)Cyc_Absyn_Bitxor;_tmp657;}));
goto _LL0;}case 426U: _LL34F: _LL350: {
# 2905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2903 "parse.y"
yyval=Cyc_YY7(({struct Cyc_Core_Opt*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->v=(void*)Cyc_Absyn_Bitor;_tmp658;}));
goto _LL0;}case 427U: _LL351: _LL352: {
# 2906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2908 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 428U: _LL353: _LL354: {
# 2911
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2913
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2910 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB68=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB67=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB66=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpB68,_tmpB67,_tmpB66,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 429U: _LL355: _LL356: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2913 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB69=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpB69,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 430U: _LL357: _LL358: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2916 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6A=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpB6A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 431U: _LL359: _LL35A: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2918 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6B=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpB6B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 432U: _LL35B: _LL35C: {
# 2921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2920 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6D=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB6C=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpB6D,_tmpB6C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 433U: _LL35D: _LL35E: {
# 2923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2922 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB6F=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpB6E=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpB6F,_tmpB6E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 434U: _LL35F: _LL360: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2926 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 435U: _LL361: _LL362: {
# 2929
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2931
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2930 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 436U: _LL363: _LL364: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2932 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB71=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB70=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpB71,_tmpB70,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 437U: _LL365: _LL366: {
# 2935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2936 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 438U: _LL367: _LL368: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2938 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB73=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB72=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpB73,_tmpB72,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 439U: _LL369: _LL36A: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2942 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 440U: _LL36B: _LL36C: {
# 2945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2944 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB75=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB74=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpB75,_tmpB74,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 441U: _LL36D: _LL36E: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2948 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 442U: _LL36F: _LL370: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2950 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB77=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB76=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpB77,_tmpB76,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 443U: _LL371: _LL372: {
# 2953
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2954 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 444U: _LL373: _LL374: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2956 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB79=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB78=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpB79,_tmpB78,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 445U: _LL375: _LL376: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2960 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 446U: _LL377: _LL378: {
# 2963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2962 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB7A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpB7B,_tmpB7A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 447U: _LL379: _LL37A: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2964 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB7C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpB7D,_tmpB7C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 448U: _LL37B: _LL37C: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2968 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 449U: _LL37D: _LL37E: {
# 2971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2970 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB7F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB7E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpB7F,_tmpB7E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 450U: _LL37F: _LL380: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2972 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB81=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB80=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpB81,_tmpB80,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 451U: _LL381: _LL382: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2974 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB83=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB82=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpB83,_tmpB82,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 452U: _LL383: _LL384: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2976 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB85=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB84=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpB85,_tmpB84,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 453U: _LL385: _LL386: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2980 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 454U: _LL387: _LL388: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2982 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB87=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB86=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpB87,_tmpB86,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 455U: _LL389: _LL38A: {
# 2985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2984 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB89=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB88=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpB89,_tmpB88,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 456U: _LL38B: _LL38C: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2988 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 457U: _LL38D: _LL38E: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2990 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB8A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpB8B,_tmpB8A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 458U: _LL38F: _LL390: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2992 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB8C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpB8D,_tmpB8C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 459U: _LL391: _LL392: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2996 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 460U: _LL393: _LL394: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2998 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB8F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB8E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpB8F,_tmpB8E,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 461U: _LL395: _LL396: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3000 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB91=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpB91,_tmpB90,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 462U: _LL397: _LL398: {
# 3003
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3005
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3002 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB93=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB92=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpB93,_tmpB92,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 463U: _LL399: _LL39A: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3006 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 464U: _LL39B: _LL39C: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3008 "parse.y"
void*_tmp659=({struct _tuple8*_tmpB94=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpB94,(unsigned int)(yyyylsp[1]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpB96=_tmp659;struct Cyc_Absyn_Exp*_tmpB95=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpB96,_tmpB95,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));
# 3011
goto _LL0;}case 465U: _LL39D: _LL39E: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3014 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 466U: _LL39F: _LL3A0: {
# 3017
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3019
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3015 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB97=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpB97,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 467U: _LL3A1: _LL3A2: {
# 3018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3016 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB98=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpB98,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 468U: _LL3A3: _LL3A4: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3017 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB99=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpB99,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 469U: _LL3A5: _LL3A6: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3018 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpB9A,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 470U: _LL3A7: _LL3A8: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3019 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpB9B,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 471U: _LL3A9: _LL3AA: {
# 3022
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3024
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3020 "parse.y"
yyval=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpB9D=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpB9C=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpB9D,_tmpB9C,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 472U: _LL3AB: _LL3AC: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3022 "parse.y"
void*_tmp65A=({struct _tuple8*_tmpB9E=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpB9E,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(_tmp65A,(unsigned int)(yyyylsp[0]).first_line));
# 3025
goto _LL0;}case 473U: _LL3AD: _LL3AE: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3025 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpB9F=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpB9F,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 474U: _LL3AF: _LL3B0: {
# 3028
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3030
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3027 "parse.y"
void*_tmp65B=({struct _tuple8*_tmpBA0=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpBA0,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpBA2=_tmp65B;struct Cyc_List_List*_tmpBA1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpBA2,_tmpBA1,(unsigned int)(yyyylsp[0]).first_line);}));
# 3030
goto _LL0;}case 475U: _LL3B1: _LL3B2: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3032 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA4=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->tag=34U,(_tmp65C->f1).is_calloc=0,(_tmp65C->f1).rgn=0,(_tmp65C->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp65C->f1).num_elts=_tmpBA3;}),(_tmp65C->f1).fat_result=0,(_tmp65C->f1).inline_call=0;_tmp65C;});Cyc_Absyn_new_exp(_tmpBA4,(unsigned int)(yyyylsp[0]).first_line);}));
# 3034
goto _LL0;}case 476U: _LL3B3: _LL3B4: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3035 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBA7=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->tag=34U,(_tmp65D->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp65D->f1).rgn=_tmpBA6;}),(_tmp65D->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBA5=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp65D->f1).num_elts=_tmpBA5;}),(_tmp65D->f1).fat_result=0,(_tmp65D->f1).inline_call=0;_tmp65D;});Cyc_Absyn_new_exp(_tmpBA7,(unsigned int)(yyyylsp[0]).first_line);}));
# 3037
goto _LL0;}case 477U: _LL3B5: _LL3B6: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3038 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBAA=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65E=_cycalloc(sizeof(*_tmp65E));_tmp65E->tag=34U,(_tmp65E->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp65E->f1).rgn=_tmpBA9;}),(_tmp65E->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpBA8=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp65E->f1).num_elts=_tmpBA8;}),(_tmp65E->f1).fat_result=0,(_tmp65E->f1).inline_call=1;_tmp65E;});Cyc_Absyn_new_exp(_tmpBAA,(unsigned int)(yyyylsp[0]).first_line);}));
# 3040
goto _LL0;}case 478U: _LL3B7: _LL3B8: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 3041 "parse.y"
void*_tmp65F=({struct _tuple8*_tmpBAB=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpBAB,(unsigned int)(yyyylsp[6]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpBAE=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=34U,(_tmp661->f1).is_calloc=1,(_tmp661->f1).rgn=0,({void**_tmpBAD=({void**_tmp660=_cycalloc(sizeof(*_tmp660));*_tmp660=_tmp65F;_tmp660;});(_tmp661->f1).elt_type=_tmpBAD;}),({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp661->f1).num_elts=_tmpBAC;}),(_tmp661->f1).fat_result=0,(_tmp661->f1).inline_call=0;_tmp661;});Cyc_Absyn_new_exp(_tmpBAE,(unsigned int)(yyyylsp[0]).first_line);}));
# 3044
goto _LL0;}case 479U: _LL3B9: _LL3BA: {
# 3046
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11U));
# 3048
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11U));
# 3046 "parse.y"
void*_tmp662=({struct _tuple8*_tmpBAF=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpBAF,(unsigned int)(yyyylsp[8]).first_line);});
yyval=Cyc_Exp_tok(({void*_tmpBB3=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->tag=34U,(_tmp664->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp664->f1).rgn=_tmpBB2;}),({void**_tmpBB1=({void**_tmp663=_cycalloc(sizeof(*_tmp663));*_tmp663=_tmp662;_tmp663;});(_tmp664->f1).elt_type=_tmpBB1;}),({struct Cyc_Absyn_Exp*_tmpBB0=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp664->f1).num_elts=_tmpBB0;}),(_tmp664->f1).fat_result=0,(_tmp664->f1).inline_call=0;_tmp664;});Cyc_Absyn_new_exp(_tmpBB3,(unsigned int)(yyyylsp[0]).first_line);}));
# 3049
goto _LL0;}case 480U: _LL3BB: _LL3BC: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3050 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBB5=({struct Cyc_Absyn_Exp*_tmp665[1U];({struct Cyc_Absyn_Exp*_tmpBB4=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp665[0]=_tmpBB4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp665,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpBB5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 481U: _LL3BD: _LL3BE: {
# 3053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3052 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBB9=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tag=38U,({struct Cyc_Absyn_Exp*_tmpBB8=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp667->f1=_tmpBB8;}),({struct _dyneither_ptr*_tmpBB7=({struct _dyneither_ptr*_tmp666=_cycalloc(sizeof(*_tmp666));({struct _dyneither_ptr _tmpBB6=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp666=_tmpBB6;});_tmp666;});_tmp667->f2=_tmpBB7;});_tmp667;});Cyc_Absyn_new_exp(_tmpBB9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 482U: _LL3BF: _LL3C0: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3054 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBBE=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tag=38U,({struct Cyc_Absyn_Exp*_tmpBBD=({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpBBC,(unsigned int)(yyyylsp[2]).first_line);});_tmp669->f1=_tmpBBD;}),({struct _dyneither_ptr*_tmpBBB=({struct _dyneither_ptr*_tmp668=_cycalloc(sizeof(*_tmp668));({struct _dyneither_ptr _tmpBBA=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp668=_tmpBBA;});_tmp668;});_tmp669->f2=_tmpBBB;});_tmp669;});Cyc_Absyn_new_exp(_tmpBBE,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 483U: _LL3C1: _LL3C2: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3056 "parse.y"
void*_tmp66A=({struct _tuple8*_tmpBBF=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpBBF,(unsigned int)(yyyylsp[2]).first_line);});
yyval=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp66A,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 484U: _LL3C3: _LL3C4: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3059 "parse.y"
struct _tuple20 _tmp66B=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp66C=_tmp66B;int _tmp66E;struct _dyneither_ptr _tmp66D;_LL496: _tmp66E=_tmp66C.f1;_tmp66D=_tmp66C.f2;_LL497:;
yyval=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp66E,_tmp66D,(unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 485U: _LL3C5: _LL3C6: {
# 3063
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3065
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3061 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_extension_exp(_tmpBC0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 486U: _LL3C7: _LL3C8: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3065 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Bitnot);
goto _LL0;}case 487U: _LL3C9: _LL3CA: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3066 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Not);
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3067 "parse.y"
yyval=Cyc_YY6(Cyc_Absyn_Minus);
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3072 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 490U: _LL3CF: _LL3D0: {
# 3075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3074 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpBC1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpBC2,_tmpBC1,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 491U: _LL3D1: _LL3D2: {
# 3077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3076 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC3=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpBC3,0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 492U: _LL3D3: _LL3D4: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3078 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC5=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpBC4=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpBC5,_tmpBC4,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 493U: _LL3D5: _LL3D6: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3080 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBC8=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpBC7=({struct _dyneither_ptr*_tmp66F=_cycalloc(sizeof(*_tmp66F));({struct _dyneither_ptr _tmpBC6=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp66F=_tmpBC6;});_tmp66F;});Cyc_Absyn_aggrmember_exp(_tmpBC8,_tmpBC7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 494U: _LL3D7: _LL3D8: {
# 3083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3082 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCB=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpBCA=({struct _dyneither_ptr*_tmp670=_cycalloc(sizeof(*_tmp670));({struct _dyneither_ptr _tmpBC9=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp670=_tmpBC9;});_tmp670;});Cyc_Absyn_aggrarrow_exp(_tmpBCB,_tmpBCA,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 495U: _LL3D9: _LL3DA: {
# 3085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3084 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCC=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpBCC,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 496U: _LL3DB: _LL3DC: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3086 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBCD=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpBCD,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 497U: _LL3DD: _LL3DE: {
# 3089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3088 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBCF=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->tag=25U,({struct _tuple8*_tmpBCE=Cyc_yyget_YY37(yyyyvsp[1]);_tmp671->f1=_tmpBCE;}),_tmp671->f2=0;_tmp671;});Cyc_Absyn_new_exp(_tmpBCF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 498U: _LL3DF: _LL3E0: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3090 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBD2=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->tag=25U,({struct _tuple8*_tmpBD1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp672->f1=_tmpBD1;}),({struct Cyc_List_List*_tmpBD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp672->f2=_tmpBD0;});_tmp672;});Cyc_Absyn_new_exp(_tmpBD2,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 499U: _LL3E1: _LL3E2: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 3092 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBD5=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp673=_cycalloc(sizeof(*_tmp673));_tmp673->tag=25U,({struct _tuple8*_tmpBD4=Cyc_yyget_YY37(yyyyvsp[1]);_tmp673->f1=_tmpBD4;}),({struct Cyc_List_List*_tmpBD3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp673->f2=_tmpBD3;});_tmp673;});Cyc_Absyn_new_exp(_tmpBD5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 500U: _LL3E3: _LL3E4: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3097 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp676=_cycalloc(sizeof(*_tmp676));({void*_tmpBD8=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->tag=0U,({struct _dyneither_ptr*_tmpBD7=({struct _dyneither_ptr*_tmp674=_cycalloc(sizeof(*_tmp674));({struct _dyneither_ptr _tmpBD6=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp674=_tmpBD6;});_tmp674;});_tmp675->f1=_tmpBD7;});_tmp675;});_tmp676->hd=_tmpBD8;}),_tmp676->tl=0;_tmp676;}));
goto _LL0;}case 501U: _LL3E5: _LL3E6: {
# 3100
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3102
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3100 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));({void*_tmpBDB=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->tag=1U,({unsigned int _tmpBDA=({unsigned int _tmpBD9=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpBD9,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp677->f1=_tmpBDA;});_tmp677;});_tmp678->hd=_tmpBDB;}),_tmp678->tl=0;_tmp678;}));
goto _LL0;}case 502U: _LL3E7: _LL3E8: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3102 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp67B=_cycalloc(sizeof(*_tmp67B));({void*_tmpBDF=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->tag=0U,({struct _dyneither_ptr*_tmpBDE=({struct _dyneither_ptr*_tmp679=_cycalloc(sizeof(*_tmp679));({struct _dyneither_ptr _tmpBDD=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp679=_tmpBDD;});_tmp679;});_tmp67A->f1=_tmpBDE;});_tmp67A;});_tmp67B->hd=_tmpBDF;}),({struct Cyc_List_List*_tmpBDC=Cyc_yyget_YY3(yyyyvsp[0]);_tmp67B->tl=_tmpBDC;});_tmp67B;}));
goto _LL0;}case 503U: _LL3E9: _LL3EA: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3105 "parse.y"
yyval=Cyc_YY3(({struct Cyc_List_List*_tmp67D=_cycalloc(sizeof(*_tmp67D));({void*_tmpBE3=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->tag=1U,({unsigned int _tmpBE2=({unsigned int _tmpBE1=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpBE1,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp67C->f1=_tmpBE2;});_tmp67C;});_tmp67D->hd=_tmpBE3;}),({struct Cyc_List_List*_tmpBE0=Cyc_yyget_YY3(yyyyvsp[0]);_tmp67D->tl=_tmpBE0;});_tmp67D;}));
goto _LL0;}case 504U: _LL3EB: _LL3EC: {
# 3108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_Exp_tok(({struct _tuple0*_tmpBE4=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpBE4,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 505U: _LL3ED: _LL3EE: {
# 3114
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3116
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3113 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBE5=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Absyn_pragma_exp(_tmpBE5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 506U: _LL3EF: _LL3F0: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3115 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 507U: _LL3F1: _LL3F2: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3117 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBE6=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpBE6,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 508U: _LL3F3: _LL3F4: {
# 3120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3119 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBE7=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpBE7,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 509U: _LL3F5: _LL3F6: {
# 3122
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3124
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3121 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 510U: _LL3F7: _LL3F8: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3126 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBE8=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpBE8,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 511U: _LL3F9: _LL3FA: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3128 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpBEA=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpBE9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpBEA,_tmpBE9,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 512U: _LL3FB: _LL3FC: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3131 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_List_List*_tmpBEB=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpBEB,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 513U: _LL3FD: _LL3FE: {
# 3134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3134 "parse.y"
yyval=Cyc_Exp_tok(({void*_tmpBEF=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->tag=29U,({struct _tuple0*_tmpBEE=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp67E->f1=_tmpBEE;}),({struct Cyc_List_List*_tmpBED=Cyc_yyget_YY40(yyyyvsp[2]);_tmp67E->f2=_tmpBED;}),({struct Cyc_List_List*_tmpBEC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp67E->f3=_tmpBEC;}),_tmp67E->f4=0;_tmp67E;});Cyc_Absyn_new_exp(_tmpBEF,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 514U: _LL3FF: _LL400: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3137 "parse.y"
yyval=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpBF0=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpBF0,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 515U: _LL401: _LL402: {
# 3140
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3142
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3141 "parse.y"
yyval=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));
goto _LL0;}case 516U: _LL403: _LL404: {
# 3144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp67F=_cycalloc(sizeof(*_tmp67F));({struct Cyc_Absyn_Exp*_tmpBF1=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp67F->hd=_tmpBF1;}),_tmp67F->tl=0;_tmp67F;}));
goto _LL0;}case 517U: _LL405: _LL406: {
# 3150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3149 "parse.y"
yyval=Cyc_YY4(({struct Cyc_List_List*_tmp680=_cycalloc(sizeof(*_tmp680));({struct Cyc_Absyn_Exp*_tmpBF3=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp680->hd=_tmpBF3;}),({struct Cyc_List_List*_tmpBF2=Cyc_yyget_YY4(yyyyvsp[0]);_tmp680->tl=_tmpBF2;});_tmp680;}));
goto _LL0;}case 518U: _LL407: _LL408: {
# 3152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpBF4=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpBF4,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 519U: _LL409: _LL40A: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3156 "parse.y"
yyval=Cyc_Exp_tok(({char _tmpBF5=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpBF5,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 520U: _LL40B: _LL40C: {
# 3159
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3161
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3157 "parse.y"
yyval=Cyc_Exp_tok(({struct _dyneither_ptr _tmpBF6=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpBF6,(unsigned int)(yyyylsp[0]).first_line);}));
goto _LL0;}case 521U: _LL40D: _LL40E: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3159 "parse.y"
struct _dyneither_ptr _tmp681=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp681);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp681,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3167
yyval=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp681,i,(unsigned int)(yyyylsp[0]).first_line));
# 3169
goto _LL0;}case 522U: _LL40F: _LL410: {
# 3171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3170 "parse.y"
yyval=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));
goto _LL0;}case 523U: _LL411: _LL412: {
# 3173
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3175
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3174 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp683=_cycalloc(sizeof(*_tmp683));({union Cyc_Absyn_Nmspace _tmpBF9=Cyc_Absyn_Rel_n(0);_tmp683->f1=_tmpBF9;}),({struct _dyneither_ptr*_tmpBF8=({struct _dyneither_ptr*_tmp682=_cycalloc(sizeof(*_tmp682));({struct _dyneither_ptr _tmpBF7=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp682=_tmpBF7;});_tmp682;});_tmp683->f2=_tmpBF8;});_tmp683;}));
goto _LL0;}case 524U: _LL413: _LL414: {
# 3177
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3179
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3175 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 525U: _LL415: _LL416: {
# 3178
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3178 "parse.y"
yyval=Cyc_QualId_tok(({struct _tuple0*_tmp685=_cycalloc(sizeof(*_tmp685));({union Cyc_Absyn_Nmspace _tmpBFC=Cyc_Absyn_Rel_n(0);_tmp685->f1=_tmpBFC;}),({struct _dyneither_ptr*_tmpBFB=({struct _dyneither_ptr*_tmp684=_cycalloc(sizeof(*_tmp684));({struct _dyneither_ptr _tmpBFA=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp684=_tmpBFA;});_tmp684;});_tmp685->f2=_tmpBFB;});_tmp685;}));
goto _LL0;}case 526U: _LL417: _LL418: {
# 3181
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3183
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3179 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 527U: _LL419: _LL41A: {
# 3182
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3184
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3184 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 528U: _LL41B: _LL41C: {
# 3187
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3189
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3185 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 529U: _LL41D: _LL41E: {
# 3188
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3190
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3188 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 530U: _LL41F: _LL420: {
# 3191
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3193
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3189 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 531U: _LL421: _LL422: {
# 3192
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3194
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3194 "parse.y"
goto _LL0;}case 532U: _LL423: _LL424: {
# 3196
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3198
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3194 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL0;}default: _LL425: _LL426:
# 3199
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
({int _tmpBFD=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpBFD;});
({int _tmpBFE=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=_tmpBFE;});}else{
# 405
({int _tmpBFF=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpBFF;});
({int _tmpC00=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_column=_tmpC00;});}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
yystate=({short _tmpC01=*((short*)_check_known_subscript_notnull(Cyc_yypgoto,149U,sizeof(short),yyn - 149));_tmpC01 + *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});
if((yystate >= 0  && yystate <= 7470) && ({short _tmpC02=*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),yystate));_tmpC02 == *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));}))
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
for(x=yyn < 0?- yyn: 0;x < 298U / sizeof(char*);++ x){
# 445
if(*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),x + yyn))== x)
({unsigned long _tmpC03=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_known_subscript_notnull(Cyc_yytname,298U,sizeof(struct _dyneither_ptr),x)))+ 15;sze +=_tmpC03;}),count ++;}
msg=({unsigned int _tmp687=(unsigned int)(sze + 15)+ 1U;char*_tmp686=({struct _RegionHandle*_tmpC04=yyregion;_region_malloc(_tmpC04,_check_times(_tmp687,sizeof(char)));});({{unsigned int _tmp741=(unsigned int)(sze + 15);unsigned int i;for(i=0;i < _tmp741;++ i){_tmp686[i]='\000';}_tmp686[_tmp741]=0;}0;});_tag_dyneither(_tmp686,sizeof(char),_tmp687);});
({struct _dyneither_ptr _tmpC05=msg;Cyc_strcpy(_tmpC05,({const char*_tmp688="parse error";_tag_dyneither(_tmp688,sizeof(char),12U);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;x < 298U / sizeof(char*);++ x){
# 455
if(*((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),x + yyn))== x){
# 457
({struct _dyneither_ptr _tmpC06=msg;Cyc_strcat(_tmpC06,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp689=", expecting `";_tag_dyneither(_tmp689,sizeof(char),14U);}):(struct _dyneither_ptr)({const char*_tmp68A=" or `";_tag_dyneither(_tmp68A,sizeof(char),6U);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_known_subscript_notnull(Cyc_yytname,298U,sizeof(struct _dyneither_ptr),x)));
({struct _dyneither_ptr _tmpC07=msg;Cyc_strcat(_tmpC07,({const char*_tmp68B="'";_tag_dyneither(_tmp68B,sizeof(char),2U);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpC09=({const char*_tmp68C="parse error";_tag_dyneither(_tmp68C,sizeof(char),12U);});int _tmpC08=yystate;Cyc_yyerror(_tmpC09,_tmpC08,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp68D=1;_npop_handler(0U);return _tmp68D;}
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
 if(yyssp_offset == 0){int _tmp68E=1;_npop_handler(0U);return _tmp68E;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)*((short*)_check_known_subscript_notnull(Cyc_yypact,1087U,sizeof(short),yystate));
if(yyn == - 32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7470) || *((short*)_check_known_subscript_notnull(Cyc_yycheck,7471U,sizeof(short),yyn))!= 1)goto yyerrdefault;
# 537
yyn=(int)Cyc_yytable[yyn];
if(yyn < 0){
# 540
if(yyn == - 32768)goto yyerrpop;
yyn=- yyn;
goto yyreduce;}else{
# 544
if(yyn == 0)goto yyerrpop;}
# 546
if(yyn == 1086){
int _tmp68F=0;_npop_handler(0U);return _tmp68F;}
# 554
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 556
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 559
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3197 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp695=v;struct Cyc_Absyn_Stmt*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6AE;struct _tuple0*_tmp6AD;struct _dyneither_ptr _tmp6AC;char _tmp6AB;union Cyc_Absyn_Cnst _tmp6AA;switch((_tmp695.Stmt_tok).tag){case 1U: _LL1: _tmp6AA=(_tmp695.Int_tok).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp698=({struct Cyc_String_pa_PrintArg_struct _tmp745;_tmp745.tag=0U,({struct _dyneither_ptr _tmpC0A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp6AA));_tmp745.f1=_tmpC0A;});_tmp745;});void*_tmp696[1U];_tmp696[0]=& _tmp698;({struct Cyc___cycFILE*_tmpC0C=Cyc_stderr;struct _dyneither_ptr _tmpC0B=({const char*_tmp697="%s";_tag_dyneither(_tmp697,sizeof(char),3U);});Cyc_fprintf(_tmpC0C,_tmpC0B,_tag_dyneither(_tmp696,sizeof(void*),1U));});});goto _LL0;case 2U: _LL3: _tmp6AB=(_tmp695.Char_tok).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp69B=({struct Cyc_Int_pa_PrintArg_struct _tmp746;_tmp746.tag=1U,_tmp746.f1=(unsigned long)((int)_tmp6AB);_tmp746;});void*_tmp699[1U];_tmp699[0]=& _tmp69B;({struct Cyc___cycFILE*_tmpC0E=Cyc_stderr;struct _dyneither_ptr _tmpC0D=({const char*_tmp69A="%c";_tag_dyneither(_tmp69A,sizeof(char),3U);});Cyc_fprintf(_tmpC0E,_tmpC0D,_tag_dyneither(_tmp699,sizeof(void*),1U));});});goto _LL0;case 3U: _LL5: _tmp6AC=(_tmp695.String_tok).val;_LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp69E=({struct Cyc_String_pa_PrintArg_struct _tmp747;_tmp747.tag=0U,_tmp747.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6AC);_tmp747;});void*_tmp69C[1U];_tmp69C[0]=& _tmp69E;({struct Cyc___cycFILE*_tmpC10=Cyc_stderr;struct _dyneither_ptr _tmpC0F=({const char*_tmp69D="\"%s\"";_tag_dyneither(_tmp69D,sizeof(char),5U);});Cyc_fprintf(_tmpC10,_tmpC0F,_tag_dyneither(_tmp69C,sizeof(void*),1U));});});goto _LL0;case 5U: _LL7: _tmp6AD=(_tmp695.QualId_tok).val;_LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp6A1=({struct Cyc_String_pa_PrintArg_struct _tmp748;_tmp748.tag=0U,({struct _dyneither_ptr _tmpC11=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6AD));_tmp748.f1=_tmpC11;});_tmp748;});void*_tmp69F[1U];_tmp69F[0]=& _tmp6A1;({struct Cyc___cycFILE*_tmpC13=Cyc_stderr;struct _dyneither_ptr _tmpC12=({const char*_tmp6A0="%s";_tag_dyneither(_tmp6A0,sizeof(char),3U);});Cyc_fprintf(_tmpC13,_tmpC12,_tag_dyneither(_tmp69F,sizeof(void*),1U));});});goto _LL0;case 7U: _LL9: _tmp6AE=(_tmp695.Exp_tok).val;_LLA:
({struct Cyc_String_pa_PrintArg_struct _tmp6A4=({struct Cyc_String_pa_PrintArg_struct _tmp749;_tmp749.tag=0U,({struct _dyneither_ptr _tmpC14=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp6AE));_tmp749.f1=_tmpC14;});_tmp749;});void*_tmp6A2[1U];_tmp6A2[0]=& _tmp6A4;({struct Cyc___cycFILE*_tmpC16=Cyc_stderr;struct _dyneither_ptr _tmpC15=({const char*_tmp6A3="%s";_tag_dyneither(_tmp6A3,sizeof(char),3U);});Cyc_fprintf(_tmpC16,_tmpC15,_tag_dyneither(_tmp6A2,sizeof(void*),1U));});});goto _LL0;case 8U: _LLB: _tmp6AF=(_tmp695.Stmt_tok).val;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp6A7=({struct Cyc_String_pa_PrintArg_struct _tmp74A;_tmp74A.tag=0U,({struct _dyneither_ptr _tmpC17=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp6AF));_tmp74A.f1=_tmpC17;});_tmp74A;});void*_tmp6A5[1U];_tmp6A5[0]=& _tmp6A7;({struct Cyc___cycFILE*_tmpC19=Cyc_stderr;struct _dyneither_ptr _tmpC18=({const char*_tmp6A6="%s";_tag_dyneither(_tmp6A6,sizeof(char),3U);});Cyc_fprintf(_tmpC19,_tmpC18,_tag_dyneither(_tmp6A5,sizeof(void*),1U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp6A8=0U;({struct Cyc___cycFILE*_tmpC1B=Cyc_stderr;struct _dyneither_ptr _tmpC1A=({const char*_tmp6A9="?";_tag_dyneither(_tmp6A9,sizeof(char),2U);});Cyc_fprintf(_tmpC1B,_tmpC1A,_tag_dyneither(_tmp6A8,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3209
struct _dyneither_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6B0="end-of-file";_tag_dyneither(_tmp6B0,sizeof(char),12U);});
# 3213
if(token == 364)
return Cyc_Lex_token_string;else{
if(token == 373)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0  && token <= 376?(int)*((short*)_check_known_subscript_notnull(Cyc_yytranslate,377U,sizeof(short),token)): 298;
if((unsigned int)z < 298U)
return Cyc_yytname[z];else{
return _tag_dyneither(0,0,0);}};}
# 3224
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6B1=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6B1;_push_region(yyr);
({struct _RegionHandle*_tmpC1C=yyr;Cyc_yyparse(_tmpC1C,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6B2=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6B2;};
# 3227
;_pop_region(yyr);};}
