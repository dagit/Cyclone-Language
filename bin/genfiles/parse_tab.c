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
#define _check_null(ptr) (ptr ? : (void*)_throw_null())
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index)\
   ((char *)ptr) + (elt_sz)*(index))
#define _check_known_subscript_notnull(bound,index) (index)

#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })

#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })

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
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
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

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct
 _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */

extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

/* FIX?  Not sure if we want to pass filename and lineno in here... */
#ifndef CYC_REGION_PROFILE
#define _cycalloc(n)            (GC_malloc(n)          ? : _throw_badalloc())
#define _cycalloc_atomic(n)     (GC_malloc_atomic(n)   ? : _throw_badalloc())
#define _cyccalloc(n,s)         (GC_calloc(n,s)        ? : _throw_badalloc())
#define _cyccalloc_atomic(n,s)  (GC_calloc_atomic(n,s) ? : _throw_badalloc())
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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
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
# 25 "warn.h"
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 36
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 38
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 332
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 482
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
extern struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct Cyc_Absyn_Noreturn_att_val;
extern struct Cyc_Absyn_Const_att_Absyn_Attribute_struct Cyc_Absyn_Const_att_val;
extern struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct Cyc_Absyn_Packed_att_val;
# 489
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
extern struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct Cyc_Absyn_Always_inline_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 896
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 923
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 927
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 941
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 946
void*Cyc_Absyn_compress_kb(void*);
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 953
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 956
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 958
extern void*Cyc_Absyn_sint_typ;
# 960
void*Cyc_Absyn_float_typ(int);
# 1002
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1084
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1100
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1103
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1107
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1112
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1114
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1121
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1130
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1135
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1141
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1166
int Cyc_Absyn_fntype_att(void*);
# 1184
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 42 "tcutil.h"
void*Cyc_Tcutil_copy_type(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
# 115
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 117
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 130
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 294 "tcutil.h"
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 358
int Cyc_Tcutil_is_array(void*t);
# 362
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
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
extern struct _dyneither_ptr Cyc_Lex_token_string;struct Cyc_Parse_FlatList{struct Cyc_Parse_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};
# 96 "parse.y"
struct Cyc_Parse_FlatList*Cyc_Parse_flat_imp_rev(struct Cyc_Parse_FlatList*x){
if(x == 0)return x;else{
# 99
struct Cyc_Parse_FlatList*first=x;
struct Cyc_Parse_FlatList*second=x->tl;
x->tl=0;
while(second != 0){
struct Cyc_Parse_FlatList*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 108
return first;}}
# 113
int Cyc_Parse_no_register=0;char Cyc_Parse_Exit[5U]="Exit";struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_Parse_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};
# 130
enum Cyc_Parse_Storage_class{Cyc_Parse_Typedef_sc  = 0U,Cyc_Parse_Extern_sc  = 1U,Cyc_Parse_ExternC_sc  = 2U,Cyc_Parse_Static_sc  = 3U,Cyc_Parse_Auto_sc  = 4U,Cyc_Parse_Register_sc  = 5U,Cyc_Parse_Abstract_sc  = 6U};struct Cyc_Parse_Declaration_spec{enum Cyc_Parse_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Parse_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Parse_Declarator{struct _tuple0*id;unsigned int varloc;struct Cyc_List_List*tms;};struct _tuple10{struct _tuple10*tl;struct Cyc_Parse_Declarator hd  __attribute__((aligned )) ;};struct _tuple11{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple11 hd  __attribute__((aligned )) ;};struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct{int tag;};struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct{int tag;};
# 166
static void Cyc_Parse_decl_split(struct _RegionHandle*r,struct _tuple12*ds,struct _tuple10**decls,struct Cyc_List_List**es){
# 170
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
for(0;ds != 0;ds=ds->tl){
struct _tuple11 _tmp0=ds->hd;struct _tuple11 _tmp1=_tmp0;struct Cyc_Parse_Declarator _tmp5;struct Cyc_Absyn_Exp*_tmp4;_LL1: _tmp5=_tmp1.f1;_tmp4=_tmp1.f2;_LL2:;
({struct _tuple10*_tmp76F=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators,_tmp2->hd=_tmp5;_tmp2;});declarators=_tmp76F;});
({struct Cyc_List_List*_tmp770=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4,_tmp3->tl=exprs;_tmp3;});exprs=_tmp770;});}
# 177
({struct Cyc_List_List*_tmp771=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp771;});
({struct _tuple10*_tmp772=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp772;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
# 187
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc);struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};
# 189
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual,void*,struct Cyc_List_List*,struct Cyc_List_List*);
# 194
static struct Cyc_List_List*Cyc_Parse_parse_result=0;
# 196
static void*Cyc_Parse_parse_abort(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 198
Cyc_Warn_verr(loc,fmt,ap);
(int)_throw((void*)({struct Cyc_Parse_Exit_exn_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=Cyc_Parse_Exit;_tmp6;}));}
# 202
static void*Cyc_Parse_type_name_to_type(struct _tuple8*tqt,unsigned int loc){
# 204
struct _tuple8*_tmp7=tqt;struct Cyc_Absyn_Tqual _tmpB;void*_tmpA;_LL1: _tmpB=_tmp7->f2;_tmpA=_tmp7->f3;_LL2:;
if((_tmpB.print_const  || _tmpB.q_volatile) || _tmpB.q_restrict){
if(_tmpB.loc != 0)loc=_tmpB.loc;
({void*_tmp8=0U;({unsigned int _tmp774=loc;struct _dyneither_ptr _tmp773=({const char*_tmp9="qualifier on type is ignored";_tag_dyneither(_tmp9,sizeof(char),29U);});Cyc_Warn_warn(_tmp774,_tmp773,_tag_dyneither(_tmp8,sizeof(void*),0U));});});}
# 209
return _tmpA;}struct _tuple14{union Cyc_Absyn_Constraint*f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;void*f4;};
# 212
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,union Cyc_Absyn_Constraint*nullable,union Cyc_Absyn_Constraint*bound,void*rgn,struct Cyc_List_List*pqs){
# 218
union Cyc_Absyn_Constraint*zeroterm=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;void*_tmpD=_tmpC;void*_tmp10;struct Cyc_Absyn_Exp*_tmpF;switch(*((int*)_tmpD)){case 4U: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_conref;goto _LL0;case 5U: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_conref;goto _LL0;case 7U: _LL5: _LL6:
 nullable=Cyc_Absyn_true_conref;goto _LL0;case 6U: _LL7: _LL8:
 nullable=Cyc_Absyn_false_conref;goto _LL0;case 3U: _LL9: _LLA:
 bound=Cyc_Absyn_bounds_dyneither_conref;goto _LL0;case 2U: _LLB: _LLC:
 bound=Cyc_Absyn_bounds_one_conref;goto _LL0;case 0U: _LLD: _tmpF=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LLE:
({union Cyc_Absyn_Constraint*_tmp775=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->tag=1U,_tmpE->f1=_tmpF;_tmpE;}));bound=_tmp775;});goto _LL0;default: _LLF: _tmp10=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10:
 rgn=_tmp10;goto _LL0;}_LL0:;}
# 230
return({struct _tuple14 _tmp6D9;_tmp6D9.f1=nullable,_tmp6D9.f2=bound,_tmp6D9.f3=zeroterm,_tmp6D9.f4=rgn;_tmp6D9;});}
# 236
struct _tuple0*Cyc_Parse_gensym_enum(){
# 238
static int enum_counter=0;
return({struct _tuple0*_tmp15=_cycalloc(sizeof(*_tmp15));({union Cyc_Absyn_Nmspace _tmp779=Cyc_Absyn_Rel_n(0);_tmp15->f1=_tmp779;}),({
struct _dyneither_ptr*_tmp778=({struct _dyneither_ptr*_tmp14=_cycalloc(sizeof(*_tmp14));({struct _dyneither_ptr _tmp777=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp13=({struct Cyc_Int_pa_PrintArg_struct _tmp6DA;_tmp6DA.tag=1U,_tmp6DA.f1=(unsigned long)enum_counter ++;_tmp6DA;});void*_tmp11[1U];_tmp11[0]=& _tmp13;({struct _dyneither_ptr _tmp776=({const char*_tmp12="__anonymous_enum_%d__";_tag_dyneither(_tmp12,sizeof(char),22U);});Cyc_aprintf(_tmp776,_tag_dyneither(_tmp11,sizeof(void*),1U));});});*_tmp14=_tmp777;});_tmp14;});_tmp15->f2=_tmp778;});_tmp15;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 243
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 248
struct _tuple17*_tmp16=field_info;unsigned int _tmp23;struct _tuple0*_tmp22;struct Cyc_Absyn_Tqual _tmp21;void*_tmp20;struct Cyc_List_List*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;_LL1: _tmp23=(_tmp16->f1)->f1;_tmp22=(_tmp16->f1)->f2;_tmp21=(_tmp16->f1)->f3;_tmp20=(_tmp16->f1)->f4;_tmp1F=(_tmp16->f1)->f5;_tmp1E=(_tmp16->f1)->f6;_tmp1D=(_tmp16->f2)->f1;_tmp1C=(_tmp16->f2)->f2;_LL2:;
if(_tmp1F != 0)
({void*_tmp17=0U;({unsigned int _tmp77B=loc;struct _dyneither_ptr _tmp77A=({const char*_tmp18="bad type params in struct field";_tag_dyneither(_tmp18,sizeof(char),32U);});Cyc_Warn_err(_tmp77B,_tmp77A,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp22))
({void*_tmp19=0U;({unsigned int _tmp77D=loc;struct _dyneither_ptr _tmp77C=({const char*_tmp1A="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp1A,sizeof(char),59U);});Cyc_Warn_err(_tmp77D,_tmp77C,_tag_dyneither(_tmp19,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->name=(*_tmp22).f2,_tmp1B->tq=_tmp21,_tmp1B->type=_tmp20,_tmp1B->width=_tmp1D,_tmp1B->attributes=_tmp1E,_tmp1B->requires_clause=_tmp1C;_tmp1B;});}
# 258
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp6DB;_tmp6DB.Signed_spec=0,_tmp6DB.Unsigned_spec=0,_tmp6DB.Short_spec=0,_tmp6DB.Long_spec=0,_tmp6DB.Long_Long_spec=0,_tmp6DB.Valid_type_spec=0,_tmp6DB.Type_spec=Cyc_Absyn_sint_typ,_tmp6DB.loc=loc;_tmp6DB;});}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);
_tmp24.Type_spec=t;
_tmp24.Valid_type_spec=1;
return _tmp24;}
# 275
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);
_tmp25.Signed_spec=1;
return _tmp25;}
# 280
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);
_tmp26.Unsigned_spec=1;
return _tmp26;}
# 285
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);
_tmp27.Short_spec=1;
return _tmp27;}
# 290
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp28=Cyc_Parse_empty_spec(loc);
_tmp28.Long_spec=1;
return _tmp28;}
# 297
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 299
return Cyc_Tcutil_is_array(t)?({
void*_tmp77E=t;Cyc_Tcutil_promote_array(_tmp77E,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 312 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp29=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp2A=(struct _tuple8*)x->hd;struct _tuple8*_tmp2B=_tmp2A;struct _dyneither_ptr _tmp41;void**_tmp40;struct _dyneither_ptr*_tmp3F;void*_tmp3E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)((struct _tuple8*)_tmp2B)->f3)->tag == 19U){_LL1: _tmp3F=_tmp2B->f1;_tmp3E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B->f3)->f1;if(_tmp3F != 0){_LL2:
# 317
{void*_tmp2C=_tmp3E;void**_tmp34;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C)->tag == 1U){_LL8: _tmp34=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C)->f2;_LL9: {
# 321
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp33=_cycalloc(sizeof(*_tmp33));({struct _dyneither_ptr _tmp780=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp6DC;_tmp6DC.tag=0U,_tmp6DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3F);_tmp6DC;});void*_tmp30[1U];_tmp30[0]=& _tmp32;({struct _dyneither_ptr _tmp77F=({const char*_tmp31="`%s";_tag_dyneither(_tmp31,sizeof(char),4U);});Cyc_aprintf(_tmp77F,_tag_dyneither(_tmp30,sizeof(void*),1U));});});*_tmp33=_tmp780;});_tmp33;});
({void*_tmp783=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=2U,({struct Cyc_Absyn_Tvar*_tmp782=({struct Cyc_Absyn_Tvar*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->name=nm,_tmp2E->identity=- 1,({void*_tmp781=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=0U,_tmp2D->f1=& Cyc_Tcutil_ik;_tmp2D;});_tmp2E->kind=_tmp781;});_tmp2E;});_tmp2F->f1=_tmp782;});_tmp2F;});*_tmp34=_tmp783;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 326
({struct Cyc_List_List*_tmp785=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _tuple18*_tmp784=({struct _tuple18*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->f1=_tmp3F,_tmp35->f2=_tmp3E;_tmp35;});_tmp36->hd=_tmp784;}),_tmp36->tl=_tmp29;_tmp36;});_tmp29=_tmp785;});goto _LL0;}else{if(((struct _tuple8*)_tmp2B)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2B)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2B)->f3)->tag == 15U){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2B)->f3)->f1)->tag == 1U){_LL3: _tmp41=*_tmp2B->f1;_tmp40=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2B->f3)->f1)->f2;_LL4: {
# 330
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct _dyneither_ptr _tmp787=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C=({struct Cyc_String_pa_PrintArg_struct _tmp6DD;_tmp6DD.tag=0U,_tmp6DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41);_tmp6DD;});void*_tmp3A[1U];_tmp3A[0]=& _tmp3C;({struct _dyneither_ptr _tmp786=({const char*_tmp3B="`%s";_tag_dyneither(_tmp3B,sizeof(char),4U);});Cyc_aprintf(_tmp786,_tag_dyneither(_tmp3A,sizeof(void*),1U));});});*_tmp3D=_tmp787;});_tmp3D;});
({void*_tmp78A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=2U,({struct Cyc_Absyn_Tvar*_tmp789=({struct Cyc_Absyn_Tvar*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->name=nm,_tmp38->identity=- 1,({void*_tmp788=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->tag=0U,_tmp37->f1=& Cyc_Tcutil_rk;_tmp37;});_tmp38->kind=_tmp788;});_tmp38;});_tmp39->f1=_tmp789;});_tmp39;});*_tmp40=_tmp78A;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 336
return _tmp29;}
# 340
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp42=0;
for(0;x != 0;x=x->tl){
void*_tmp43=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp44=_tmp43;void*_tmp47;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp44)->tag == 19U){_LL1: _tmp47=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp44)->f1;_LL2:
# 345
({struct Cyc_List_List*_tmp78C=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));({struct _tuple18*_tmp78B=({struct _tuple18*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp45->f2=_tmp47;_tmp45;});_tmp46->hd=_tmp78B;}),_tmp46->tl=_tmp42;_tmp46;});_tmp42=_tmp78C;});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 349
return _tmp42;}
# 353
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp48=e->r;void*_tmp49=_tmp48;struct _dyneither_ptr*_tmp50;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp50=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp49)->f1)->f1)->f2;_LL2:
# 356
{struct Cyc_List_List*_tmp4A=tags;for(0;_tmp4A != 0;_tmp4A=_tmp4A->tl){
struct _tuple18*_tmp4B=(struct _tuple18*)_tmp4A->hd;struct _tuple18*_tmp4C=_tmp4B;struct _dyneither_ptr*_tmp4F;void*_tmp4E;_LL6: _tmp4F=_tmp4C->f1;_tmp4E=_tmp4C->f2;_LL7:;
if(Cyc_strptrcmp(_tmp4F,_tmp50)== 0)
return({void*_tmp78E=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp78D=Cyc_Tcutil_copy_type(_tmp4E);_tmp4D->f1=_tmp78D;});_tmp4D;});Cyc_Absyn_new_exp(_tmp78E,e->loc);});}}
# 361
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 364
return e;}
# 369
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp51=t;void*_tmp67;struct Cyc_Absyn_Tqual _tmp66;void*_tmp65;union Cyc_Absyn_Constraint*_tmp64;union Cyc_Absyn_Constraint*_tmp63;union Cyc_Absyn_Constraint*_tmp62;struct Cyc_Absyn_PtrLoc*_tmp61;void*_tmp60;struct Cyc_Absyn_Tqual _tmp5F;struct Cyc_Absyn_Exp*_tmp5E;union Cyc_Absyn_Constraint*_tmp5D;unsigned int _tmp5C;switch(*((int*)_tmp51)){case 8U: _LL1: _tmp60=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51)->f1).elt_type;_tmp5F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51)->f1).tq;_tmp5E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51)->f1).num_elts;_tmp5D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51)->f1).zero_term;_tmp5C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp51)->f1).zt_loc;_LL2: {
# 372
struct Cyc_Absyn_Exp*nelts2=_tmp5E;
if(_tmp5E != 0)
({struct Cyc_Absyn_Exp*_tmp78F=Cyc_Parse_substitute_tags_exp(tags,_tmp5E);nelts2=_tmp78F;});{
# 376
void*_tmp52=Cyc_Parse_substitute_tags(tags,_tmp60);
if(_tmp5E != nelts2  || _tmp60 != _tmp52)
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->tag=8U,(_tmp53->f1).elt_type=_tmp52,(_tmp53->f1).tq=_tmp5F,(_tmp53->f1).num_elts=nelts2,(_tmp53->f1).zero_term=_tmp5D,(_tmp53->f1).zt_loc=_tmp5C;_tmp53;});
goto _LL0;};}case 5U: _LL3: _tmp67=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51)->f1).elt_typ;_tmp66=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51)->f1).elt_tq;_tmp65=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51)->f1).ptr_atts).rgn;_tmp64=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51)->f1).ptr_atts).nullable;_tmp63=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51)->f1).ptr_atts).bounds;_tmp62=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51)->f1).ptr_atts).zero_term;_tmp61=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51)->f1).ptr_atts).ptrloc;_LL4: {
# 381
void*_tmp54=Cyc_Parse_substitute_tags(tags,_tmp67);
union Cyc_Absyn_Constraint*_tmp55=_tmp63;
{union Cyc_Absyn_Constraint _tmp56=*_tmp63;union Cyc_Absyn_Constraint _tmp57=_tmp56;struct Cyc_Absyn_Exp*_tmp5A;if((_tmp57.Eq_constr).tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp57.Eq_constr).val)->tag == 1U){_LL8: _tmp5A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)(_tmp57.Eq_constr).val)->f1;_LL9:
# 385
({struct Cyc_Absyn_Exp*_tmp790=Cyc_Parse_substitute_tags_exp(tags,_tmp5A);_tmp5A=_tmp790;});
({union Cyc_Absyn_Constraint*_tmp792=({union Cyc_Absyn_Constraint*_tmp59=_cycalloc(sizeof(*_tmp59));(_tmp59->Eq_constr).tag=1U,({void*_tmp791=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=1U,_tmp58->f1=_tmp5A;_tmp58;});(_tmp59->Eq_constr).val=_tmp791;});_tmp59;});_tmp55=_tmp792;});
goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 390
if(_tmp54 != _tmp67  || _tmp55 != _tmp63)
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->tag=5U,(_tmp5B->f1).elt_typ=_tmp54,(_tmp5B->f1).elt_tq=_tmp66,((_tmp5B->f1).ptr_atts).rgn=_tmp65,((_tmp5B->f1).ptr_atts).nullable=_tmp64,((_tmp5B->f1).ptr_atts).bounds=_tmp55,((_tmp5B->f1).ptr_atts).zero_term=_tmp62,((_tmp5B->f1).ptr_atts).ptrloc=_tmp61;_tmp5B;});
goto _LL0;}default: _LL5: _LL6:
# 395
 goto _LL0;}_LL0:;}
# 397
return t;}
# 402
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp793=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp793;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 409
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 411
return({struct _tuple19*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=(*t).f2,_tmp68->f2=(*t).f3;_tmp68;});}
# 414
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp69=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp69)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 423
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(({struct _dyneither_ptr _tmp794=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp794,({const char*_tmp6A="`H";_tag_dyneither(_tmp6A,sizeof(char),3U);}));})== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
if(({struct _dyneither_ptr _tmp795=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp795,({const char*_tmp6B="`U";_tag_dyneither(_tmp6B,sizeof(char),3U);}));})== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
if(({struct _dyneither_ptr _tmp796=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp796,({const char*_tmp6C="`RC";_tag_dyneither(_tmp6C,sizeof(char),4U);}));})== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 431
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=2U,({struct Cyc_Absyn_Tvar*_tmp798=({struct Cyc_Absyn_Tvar*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct _dyneither_ptr*_tmp797=({struct _dyneither_ptr*_tmp6D=_cycalloc(sizeof(*_tmp6D));*_tmp6D=s;_tmp6D;});_tmp6E->name=_tmp797;}),_tmp6E->identity=- 1,_tmp6E->kind=k;_tmp6E;});_tmp6F->f1=_tmp798;});_tmp6F;});}}}}
# 438
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp70=t;struct Cyc_Absyn_Tvar*_tmp73;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->tag == 2U){_LL1: _tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->f1;_LL2:
 return _tmp73;}else{_LL3: _LL4:
({void*_tmp71=0U;({unsigned int _tmp79A=loc;struct _dyneither_ptr _tmp799=({const char*_tmp72="expecting a list of type variables, not types";_tag_dyneither(_tmp72,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp79A,_tmp799,_tag_dyneither(_tmp71,sizeof(void*),0U));});});}_LL0:;}
# 446
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp74=Cyc_Tcutil_compress(t);void*_tmp75=_tmp74;void**_tmp79;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->tag == 2U){_LL1: _tmp79=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1)->kind;_LL2: {
# 449
void*_tmp76=Cyc_Absyn_compress_kb(*_tmp79);void*_tmp77=_tmp76;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp77)->tag == 1U){_LL6: _LL7:
# 451
 if(!leq)({void*_tmp79B=Cyc_Tcutil_kind_to_bound(k);*_tmp79=_tmp79B;});else{
({void*_tmp79C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=2U,_tmp78->f1=0,_tmp78->f2=k;_tmp78;});*_tmp79=_tmp79C;});}
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 456
 return;}_LL0:;}
# 461
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 467
if(tds == 0)return tms;
# 472
if(tms == 0)return 0;{
# 474
void*_tmp7A=(void*)tms->hd;void*_tmp7B=_tmp7A;void*_tmp96;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp7B)->tag == 3U){_LL1: _tmp96=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp7B)->f1;_LL2:
# 477
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 480
void*_tmp7C=_tmp96;struct Cyc_List_List*_tmp94;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp7C)->tag == 1U){_LL6: _LL7:
# 482
({void*_tmp7D=0U;({unsigned int _tmp79E=loc;struct _dyneither_ptr _tmp79D=({const char*_tmp7E="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp7E,sizeof(char),93U);});Cyc_Warn_warn(_tmp79E,_tmp79D,_tag_dyneither(_tmp7D,sizeof(void*),0U));});});
# 484
return tms;}else{_LL8: _tmp94=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp7C)->f1;_LL9:
# 486
 if(({int _tmp79F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp94);_tmp79F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp7F=0U;({unsigned int _tmp7A1=loc;struct _dyneither_ptr _tmp7A0=({const char*_tmp80="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp80,sizeof(char),73U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7A1,_tmp7A0,_tag_dyneither(_tmp7F,sizeof(void*),0U));});});{
# 490
struct Cyc_List_List*rev_new_params=0;
for(0;_tmp94 != 0;_tmp94=_tmp94->tl){
struct Cyc_List_List*_tmp81=tds;
for(0;_tmp81 != 0;_tmp81=_tmp81->tl){
struct Cyc_Absyn_Decl*_tmp82=(struct Cyc_Absyn_Decl*)_tmp81->hd;
void*_tmp83=_tmp82->r;void*_tmp84=_tmp83;struct Cyc_Absyn_Vardecl*_tmp8D;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp84)->tag == 0U){_LLB: _tmp8D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp84)->f1;_LLC:
# 497
 if(Cyc_zstrptrcmp((*_tmp8D->name).f2,(struct _dyneither_ptr*)_tmp94->hd)!= 0)
continue;
if(_tmp8D->initializer != 0)
({void*_tmp85=0U;({unsigned int _tmp7A3=_tmp82->loc;struct _dyneither_ptr _tmp7A2=({const char*_tmp86="initializer found in parameter declaration";_tag_dyneither(_tmp86,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7A3,_tmp7A2,_tag_dyneither(_tmp85,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp8D->name))
({void*_tmp87=0U;({unsigned int _tmp7A5=_tmp82->loc;struct _dyneither_ptr _tmp7A4=({const char*_tmp88="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp88,sizeof(char),47U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7A5,_tmp7A4,_tag_dyneither(_tmp87,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp7A7=({struct Cyc_List_List*_tmp8A=_cycalloc(sizeof(*_tmp8A));
({struct _tuple8*_tmp7A6=({struct _tuple8*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->f1=(*_tmp8D->name).f2,_tmp89->f2=_tmp8D->tq,_tmp89->f3=_tmp8D->type;_tmp89;});_tmp8A->hd=_tmp7A6;}),_tmp8A->tl=rev_new_params;_tmp8A;});
# 503
rev_new_params=_tmp7A7;});
# 506
goto L;}else{_LLD: _LLE:
({void*_tmp8B=0U;({unsigned int _tmp7A9=_tmp82->loc;struct _dyneither_ptr _tmp7A8=({const char*_tmp8C="nonvariable declaration in parameter type";_tag_dyneither(_tmp8C,sizeof(char),42U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7A9,_tmp7A8,_tag_dyneither(_tmp8B,sizeof(void*),0U));});});}_LLA:;}
# 510
L: if(_tmp81 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp6DE;_tmp6DE.tag=0U,_tmp6DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp94->hd));_tmp6DE;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({unsigned int _tmp7AB=loc;struct _dyneither_ptr _tmp7AA=({const char*_tmp8F="%s is not given a type";_tag_dyneither(_tmp8F,sizeof(char),23U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7AB,_tmp7AA,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});}
# 513
return({struct Cyc_List_List*_tmp93=_region_malloc(yy,sizeof(*_tmp93));
({void*_tmp7AE=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp92=_region_malloc(yy,sizeof(*_tmp92));_tmp92->tag=3U,({void*_tmp7AD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp91=_region_malloc(yy,sizeof(*_tmp91));_tmp91->tag=1U,({struct Cyc_List_List*_tmp7AC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp91->f1=_tmp7AC;}),_tmp91->f2=0,_tmp91->f3=0,_tmp91->f4=0,_tmp91->f5=0,_tmp91->f6=0,_tmp91->f7=0;_tmp91;});_tmp92->f1=_tmp7AD;});_tmp92;});_tmp93->hd=_tmp7AE;}),_tmp93->tl=0;_tmp93;});};}_LL5:;}
# 520
goto _LL4;}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp95=_region_malloc(yy,sizeof(*_tmp95));_tmp95->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7AF=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp95->tl=_tmp7AF;});_tmp95;});}_LL0:;};}
# 528
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 532
if(tds != 0)
({struct Cyc_Parse_Declarator _tmp7B1=({struct Cyc_Parse_Declarator _tmp6DF;_tmp6DF.id=d.id,_tmp6DF.varloc=d.varloc,({struct Cyc_List_List*_tmp7B0=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6DF.tms=_tmp7B0;});_tmp6DF;});d=_tmp7B1;});{
# 535
enum Cyc_Absyn_Scope sc=Cyc_Absyn_Public;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 541
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 547
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmp97=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmp98=_tmp97;switch(_tmp98){case Cyc_Parse_Extern_sc: _LL1: _LL2:
 sc=Cyc_Absyn_Extern;goto _LL0;case Cyc_Parse_Static_sc: _LL3: _LL4:
 sc=Cyc_Absyn_Static;goto _LL0;default: _LL5: _LL6:
({void*_tmp99=0U;({unsigned int _tmp7B3=loc;struct _dyneither_ptr _tmp7B2=({const char*_tmp9A="bad storage class on function";_tag_dyneither(_tmp9A,sizeof(char),30U);});Cyc_Warn_err(_tmp7B3,_tmp7B2,_tag_dyneither(_tmp99,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 554
void*_tmp9B=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmp9C=Cyc_Parse_apply_tms(tq,_tmp9B,atts,d.tms);struct _tuple13 _tmp9D=_tmp9C;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC2;struct Cyc_List_List*_tmpC1;struct Cyc_List_List*_tmpC0;_LL8: _tmpC3=_tmp9D.f1;_tmpC2=_tmp9D.f2;_tmpC1=_tmp9D.f3;_tmpC0=_tmp9D.f4;_LL9:;
# 559
if(_tmpC1 != 0)
# 562
({void*_tmp9E=0U;({unsigned int _tmp7B5=loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp9F="bad type params, ignoring";_tag_dyneither(_tmp9F,sizeof(char),26U);});Cyc_Warn_warn(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp9E,sizeof(void*),0U));});});{
# 564
void*_tmpA0=_tmpC2;struct Cyc_List_List*_tmpBF;void*_tmpBE;struct Cyc_Absyn_Tqual _tmpBD;void*_tmpBC;struct Cyc_List_List*_tmpBB;int _tmpBA;struct Cyc_Absyn_VarargInfo*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_List_List*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_List_List*_tmpB3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->tag == 9U){_LLB: _tmpBF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).tvars;_tmpBE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).effect;_tmpBD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ret_tqual;_tmpBC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ret_typ;_tmpBB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).args;_tmpBA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).c_varargs;_tmpB9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).cyc_varargs;_tmpB8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).rgn_po;_tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).attributes;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).requires_clause;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).requires_relns;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ensures_clause;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ensures_relns;_LLC: {
# 568
struct Cyc_List_List*_tmpA1=0;
{struct Cyc_List_List*_tmpA2=_tmpBB;for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
struct _tuple8*_tmpA3=(struct _tuple8*)_tmpA2->hd;struct _tuple8*_tmpA4=_tmpA3;struct _dyneither_ptr*_tmpAF;struct Cyc_Absyn_Tqual _tmpAE;void*_tmpAD;_LL10: _tmpAF=_tmpA4->f1;_tmpAE=_tmpA4->f2;_tmpAD=_tmpA4->f3;_LL11:;
if(_tmpAF == 0){
({void*_tmpA5=0U;({unsigned int _tmp7B7=loc;struct _dyneither_ptr _tmp7B6=({const char*_tmpA6="missing argument variable in function prototype";_tag_dyneither(_tmpA6,sizeof(char),48U);});Cyc_Warn_err(_tmp7B7,_tmp7B6,_tag_dyneither(_tmpA5,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp7BB=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct _tuple8*_tmp7BA=({struct _tuple8*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct _dyneither_ptr*_tmp7B9=({struct _dyneither_ptr*_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct _dyneither_ptr _tmp7B8=({const char*_tmpA7="?";_tag_dyneither(_tmpA7,sizeof(char),2U);});*_tmpA8=_tmp7B8;});_tmpA8;});_tmpA9->f1=_tmp7B9;}),_tmpA9->f2=_tmpAE,_tmpA9->f3=_tmpAD;_tmpA9;});_tmpAA->hd=_tmp7BA;}),_tmpAA->tl=_tmpA1;_tmpAA;});_tmpA1=_tmp7BB;});}else{
# 575
({struct Cyc_List_List*_tmp7BD=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));({struct _tuple8*_tmp7BC=({struct _tuple8*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=_tmpAF,_tmpAB->f2=_tmpAE,_tmpAB->f3=_tmpAD;_tmpAB;});_tmpAC->hd=_tmp7BC;}),_tmpAC->tl=_tmpA1;_tmpAC;});_tmpA1=_tmp7BD;});}}}
# 579
return({struct Cyc_Absyn_Fndecl*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->sc=sc,_tmpB0->name=d.id,_tmpB0->tvs=_tmpBF,_tmpB0->is_inline=is_inline,_tmpB0->effect=_tmpBE,_tmpB0->ret_tqual=_tmpBD,_tmpB0->ret_type=_tmpBC,({
# 582
struct Cyc_List_List*_tmp7BF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA1);_tmpB0->args=_tmp7BF;}),_tmpB0->c_varargs=_tmpBA,_tmpB0->cyc_varargs=_tmpB9,_tmpB0->rgn_po=_tmpB8,_tmpB0->body=body,_tmpB0->cached_typ=0,_tmpB0->param_vardecls=0,_tmpB0->fn_vardecl=0,({
# 589
struct Cyc_List_List*_tmp7BE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB7,_tmpC0);_tmpB0->attributes=_tmp7BE;}),_tmpB0->requires_clause=_tmpB6,_tmpB0->requires_relns=0,_tmpB0->ensures_clause=_tmpB4,_tmpB0->ensures_relns=0;_tmpB0;});}}else{_LLD: _LLE:
# 594
({void*_tmpB1=0U;({unsigned int _tmp7C1=loc;struct _dyneither_ptr _tmp7C0=({const char*_tmpB2="declarator is not a function prototype";_tag_dyneither(_tmpB2,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7C1,_tmp7C0,_tag_dyneither(_tmpB1,sizeof(void*),0U));});});}_LLA:;};};};}static char _tmpC4[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 598
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpC4,_tmpC4,_tmpC4 + 76U};static char _tmpC5[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 600
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpC5,_tmpC5,_tmpC5 + 87U};static char _tmpC6[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 602
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpC6,_tmpC6,_tmpC6 + 74U};static char _tmpC7[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 604
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpC7,_tmpC7,_tmpC7 + 84U};
# 611
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 614
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpC8=0U;({unsigned int _tmp7C3=loc;struct _dyneither_ptr _tmp7C2=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7C3,_tmp7C2,_tag_dyneither(_tmpC8,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpC9=0U;({unsigned int _tmp7C5=loc;struct _dyneither_ptr _tmp7C4=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7C5,_tmp7C4,_tag_dyneither(_tmpC9,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpCA=0U;({unsigned int _tmp7C7=loc;struct _dyneither_ptr _tmp7C6=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7C7,_tmp7C6,_tag_dyneither(_tmpCA,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpCB=0U;({unsigned int _tmp7C9=loc;struct _dyneither_ptr _tmp7C8=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7C9,_tmp7C8,_tag_dyneither(_tmpCB,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpCC=0U;({unsigned int _tmp7CB=loc;struct _dyneither_ptr _tmp7CA=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7CB,_tmp7CA,_tag_dyneither(_tmpCC,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 636
return s1;}
# 642
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 645
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec:(void*)& Cyc_Absyn_VoidType_val;
enum Cyc_Absyn_Size_of sz=Cyc_Absyn_Int_sz;
enum Cyc_Absyn_Sign sgn=Cyc_Absyn_Signed;
# 652
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpCD=0U;({unsigned int _tmp7CD=loc;struct _dyneither_ptr _tmp7CC=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7CD,_tmp7CC,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpCE=0U;({unsigned int _tmp7CF=loc;struct _dyneither_ptr _tmp7CE=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7CF,_tmp7CE,_tag_dyneither(_tmpCE,sizeof(void*),0U));});});
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 664
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpCF=0U;({unsigned int _tmp7D1=loc;struct _dyneither_ptr _tmp7D0=({const char*_tmpD0="missing type within specifier";_tag_dyneither(_tmpD0,sizeof(char),30U);});Cyc_Warn_warn(_tmp7D1,_tmp7D0,_tag_dyneither(_tmpCF,sizeof(void*),0U));});});
({void*_tmp7D2=Cyc_Absyn_int_typ(sgn,sz);t=_tmp7D2;});}else{
# 669
if(seen_sign){
void*_tmpD1=t;enum Cyc_Absyn_Sign _tmpD5;enum Cyc_Absyn_Size_of _tmpD4;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1)->tag == 6U){_LL1: _tmpD5=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1)->f1;_tmpD4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1)->f2;_LL2:
# 672
 if(_tmpD5 != sgn)
({void*_tmp7D3=Cyc_Absyn_int_typ(sgn,_tmpD4);t=_tmp7D3;});
goto _LL0;}else{_LL3: _LL4:
({void*_tmpD2=0U;({unsigned int _tmp7D5=loc;struct _dyneither_ptr _tmp7D4=({const char*_tmpD3="sign specification on non-integral type";_tag_dyneither(_tmpD3,sizeof(char),40U);});Cyc_Warn_err(_tmp7D5,_tmp7D4,_tag_dyneither(_tmpD2,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 677
if(seen_size){
void*_tmpD6=t;enum Cyc_Absyn_Sign _tmpDA;enum Cyc_Absyn_Size_of _tmpD9;switch(*((int*)_tmpD6)){case 6U: _LL6: _tmpDA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD6)->f1;_tmpD9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD6)->f2;_LL7:
# 680
 if(_tmpD9 != sz)
({void*_tmp7D6=Cyc_Absyn_int_typ(_tmpDA,sz);t=_tmp7D6;});
goto _LL5;case 7U: _LL8: _LL9:
# 684
({void*_tmp7D7=Cyc_Absyn_float_typ(2);t=_tmp7D7;});goto _LL5;default: _LLA: _LLB:
({void*_tmpD7=0U;({unsigned int _tmp7D9=loc;struct _dyneither_ptr _tmp7D8=({const char*_tmpD8="size qualifier on non-integral type";_tag_dyneither(_tmpD8,sizeof(char),36U);});Cyc_Warn_err(_tmp7D9,_tmp7D8,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 688
return t;}
# 691
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 695
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpDB=d.id;
unsigned int _tmpDC=d.varloc;
struct _tuple13 _tmpDD=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpDE=_tmpDD;struct Cyc_Absyn_Tqual _tmpE6;void*_tmpE5;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE3;_LL1: _tmpE6=_tmpDE.f1;_tmpE5=_tmpDE.f2;_tmpE4=_tmpDE.f3;_tmpE3=_tmpDE.f4;_LL2:;
# 702
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpE0=_region_malloc(r,sizeof(*_tmpE0));({struct _tuple15*_tmp7DA=({struct _tuple15*_tmpDF=_region_malloc(r,sizeof(*_tmpDF));_tmpDF->f1=_tmpDC,_tmpDF->f2=_tmpDB,_tmpDF->f3=_tmpE6,_tmpDF->f4=_tmpE5,_tmpDF->f5=_tmpE4,_tmpDF->f6=_tmpE3;_tmpDF;});_tmpE0->hd=_tmp7DA;}),_tmpE0->tl=0;_tmpE0;});else{
# 705
return({struct Cyc_List_List*_tmpE2=_region_malloc(r,sizeof(*_tmpE2));({struct _tuple15*_tmp7E0=({struct _tuple15*_tmpE1=_region_malloc(r,sizeof(*_tmpE1));_tmpE1->f1=_tmpDC,_tmpE1->f2=_tmpDB,_tmpE1->f3=_tmpE6,_tmpE1->f4=_tmpE5,_tmpE1->f5=_tmpE4,_tmpE1->f6=_tmpE3;_tmpE1;});_tmpE2->hd=_tmp7E0;}),({
struct Cyc_List_List*_tmp7DF=({struct _RegionHandle*_tmp7DE=r;struct Cyc_Absyn_Tqual _tmp7DD=tq;void*_tmp7DC=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp7DB=ds->tl;Cyc_Parse_apply_tmss(_tmp7DE,_tmp7DD,_tmp7DC,_tmp7DB,shared_atts);});_tmpE2->tl=_tmp7DF;});_tmpE2;});}};}
# 709
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 712
if(tms == 0)return({struct _tuple13 _tmp6E0;_tmp6E0.f1=tq,_tmp6E0.f2=t,_tmp6E0.f3=0,_tmp6E0.f4=atts;_tmp6E0;});{
void*_tmpE7=(void*)tms->hd;void*_tmpE8=_tmpE7;unsigned int _tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_PtrAtts _tmp10C;struct Cyc_Absyn_Tqual _tmp10B;struct Cyc_List_List*_tmp10A;unsigned int _tmp109;void*_tmp108;struct Cyc_Absyn_Exp*_tmp107;union Cyc_Absyn_Constraint*_tmp106;unsigned int _tmp105;union Cyc_Absyn_Constraint*_tmp104;unsigned int _tmp103;switch(*((int*)_tmpE8)){case 0U: _LL1: _tmp104=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp103=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LL2:
# 715
 return({struct Cyc_Absyn_Tqual _tmp7E3=Cyc_Absyn_empty_tqual(0U);void*_tmp7E2=
Cyc_Absyn_array_typ(t,tq,0,_tmp104,_tmp103);
# 715
struct Cyc_List_List*_tmp7E1=atts;Cyc_Parse_apply_tms(_tmp7E3,_tmp7E2,_tmp7E1,tms->tl);});case 1U: _LL3: _tmp107=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp106=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_tmp105=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f3;_LL4:
# 718
 return({struct Cyc_Absyn_Tqual _tmp7E6=Cyc_Absyn_empty_tqual(0U);void*_tmp7E5=
Cyc_Absyn_array_typ(t,tq,_tmp107,_tmp106,_tmp105);
# 718
struct Cyc_List_List*_tmp7E4=atts;Cyc_Parse_apply_tms(_tmp7E6,_tmp7E5,_tmp7E4,tms->tl);});case 3U: _LL5: _tmp108=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_LL6: {
# 721
void*_tmpE9=_tmp108;unsigned int _tmpFF;struct Cyc_List_List*_tmpFE;int _tmpFD;struct Cyc_Absyn_VarargInfo*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->tag == 1U){_LLE: _tmpFE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f1;_tmpFD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f2;_tmpFC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f3;_tmpFB=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f4;_tmpFA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f5;_tmpF9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f6;_tmpF8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f7;_LLF: {
# 723
struct Cyc_List_List*typvars=0;
# 725
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
({struct Cyc_List_List*_tmp7E7=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=(void*)as->hd,_tmpEA->tl=fn_atts;_tmpEA;});fn_atts=_tmp7E7;});else{
# 730
({struct Cyc_List_List*_tmp7E8=({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=(void*)as->hd,_tmpEB->tl=new_atts;_tmpEB;});new_atts=_tmp7E8;});}}}
# 733
if(tms->tl != 0){
void*_tmpEC=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmpED=_tmpEC;struct Cyc_List_List*_tmpEE;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpED)->tag == 4U){_LL13: _tmpEE=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpED)->f1;_LL14:
# 736
 typvars=_tmpEE;
tms=tms->tl;
goto _LL12;}else{_LL15: _LL16:
# 740
 goto _LL12;}_LL12:;}
# 744
if(((((!_tmpFD  && _tmpFC == 0) && _tmpFE != 0) && _tmpFE->tl == 0) && (*((struct _tuple8*)_tmpFE->hd)).f1 == 0) && (*((struct _tuple8*)_tmpFE->hd)).f3 == (void*)& Cyc_Absyn_VoidType_val)
# 749
_tmpFE=0;{
# 752
struct Cyc_List_List*_tmpEF=Cyc_Parse_get_arg_tags(_tmpFE);
# 754
if(_tmpEF != 0)
({void*_tmp7E9=Cyc_Parse_substitute_tags(_tmpEF,t);t=_tmp7E9;});
({void*_tmp7EA=Cyc_Parse_array2ptr(t,0);t=_tmp7EA;});
# 759
{struct Cyc_List_List*_tmpF0=_tmpFE;for(0;_tmpF0 != 0;_tmpF0=_tmpF0->tl){
struct _tuple8*_tmpF1=(struct _tuple8*)_tmpF0->hd;struct _tuple8*_tmpF2=_tmpF1;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Tqual _tmpF4;void**_tmpF3;_LL18: _tmpF5=_tmpF2->f1;_tmpF4=_tmpF2->f2;_tmpF3=(void**)& _tmpF2->f3;_LL19:;
if(_tmpEF != 0)
({void*_tmp7EB=Cyc_Parse_substitute_tags(_tmpEF,*_tmpF3);*_tmpF3=_tmp7EB;});
({void*_tmp7EC=Cyc_Parse_array2ptr(*_tmpF3,1);*_tmpF3=_tmp7EC;});}}
# 771
return({struct Cyc_Absyn_Tqual _tmp7EF=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7EE=
Cyc_Absyn_function_typ(typvars,_tmpFB,tq,t,_tmpFE,_tmpFD,_tmpFC,_tmpFA,fn_atts,_tmpF9,_tmpF8);
# 771
struct Cyc_List_List*_tmp7ED=new_atts;Cyc_Parse_apply_tms(_tmp7EF,_tmp7EE,_tmp7ED,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL10: _tmpFF=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpE9)->f2;_LL11:
# 778
({void*_tmpF6=0U;({unsigned int _tmp7F1=_tmpFF;struct _dyneither_ptr _tmp7F0=({const char*_tmpF7="function declaration without parameter types";_tag_dyneither(_tmpF7,sizeof(char),45U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7F1,_tmp7F0,_tag_dyneither(_tmpF6,sizeof(void*),0U));});});}_LLD:;}case 4U: _LL7: _tmp10A=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp109=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LL8:
# 785
 if(tms->tl == 0)
return({struct _tuple13 _tmp6E1;_tmp6E1.f1=tq,_tmp6E1.f2=t,_tmp6E1.f3=_tmp10A,_tmp6E1.f4=atts;_tmp6E1;});
# 790
({void*_tmp100=0U;({unsigned int _tmp7F3=_tmp109;struct _dyneither_ptr _tmp7F2=({const char*_tmp101="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmp101,sizeof(char),68U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7F3,_tmp7F2,_tag_dyneither(_tmp100,sizeof(void*),0U));});});case 2U: _LL9: _tmp10C=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp10B=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LLA:
# 793
 return({struct Cyc_Absyn_Tqual _tmp7F6=_tmp10B;void*_tmp7F5=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->tag=5U,(_tmp102->f1).elt_typ=t,(_tmp102->f1).elt_tq=tq,(_tmp102->f1).ptr_atts=_tmp10C;_tmp102;});struct Cyc_List_List*_tmp7F4=atts;Cyc_Parse_apply_tms(_tmp7F6,_tmp7F5,_tmp7F4,tms->tl);});default: _LLB: _tmp10E=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp10D=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LLC:
# 798
 return({struct Cyc_Absyn_Tqual _tmp7F9=tq;void*_tmp7F8=t;struct Cyc_List_List*_tmp7F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp10D);Cyc_Parse_apply_tms(_tmp7F9,_tmp7F8,_tmp7F7,tms->tl);});}_LL0:;};}
# 804
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 813
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmp10F=t;unsigned int _tmp119;struct _tuple0*_tmp118;struct Cyc_Absyn_Tqual _tmp117;void*_tmp116;struct Cyc_List_List*_tmp115;struct Cyc_List_List*_tmp114;_LL1: _tmp119=_tmp10F->f1;_tmp118=_tmp10F->f2;_tmp117=_tmp10F->f3;_tmp116=_tmp10F->f4;_tmp115=_tmp10F->f5;_tmp114=_tmp10F->f6;_LL2:;
# 816
Cyc_Lex_register_typedef(_tmp118);{
# 818
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp110=_tmp116;struct Cyc_Core_Opt*_tmp111;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp110)->tag == 1U){_LL4: _tmp111=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp110)->f1;_LL5:
# 822
 type=0;
if(_tmp111 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp111;}
goto _LL3;}else{_LL6: _LL7:
 kind=0;type=_tmp116;goto _LL3;}_LL3:;}
# 828
return({void*_tmp7FB=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7FA=({struct Cyc_Absyn_Typedefdecl*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp118,_tmp112->tvs=_tmp115,_tmp112->kind=kind,_tmp112->defn=type,_tmp112->atts=_tmp114,_tmp112->tq=_tmp117,_tmp112->extern_c=0;_tmp112;});_tmp113->f1=_tmp7FA;});_tmp113;});Cyc_Absyn_new_decl(_tmp7FB,loc);});};}
# 835
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7FC=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=12U,_tmp11A->f1=d,_tmp11A->f2=s;_tmp11A;});Cyc_Absyn_new_stmt(_tmp7FC,d->loc);});}
# 841
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 850
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 854
struct _RegionHandle _tmp11B=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp11B;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp11C=ds;struct Cyc_Absyn_Tqual _tmp17D;struct Cyc_Parse_Type_specifier _tmp17C;struct Cyc_List_List*_tmp17B;_LL1: _tmp17D=_tmp11C.tq;_tmp17C=_tmp11C.type_specs;_tmp17B=_tmp11C.attributes;_LL2:;
if(_tmp17D.loc == 0)_tmp17D.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp11D=0U;({unsigned int _tmp7FE=loc;struct _dyneither_ptr _tmp7FD=({const char*_tmp11E="inline qualifier on non-function definition";_tag_dyneither(_tmp11E,sizeof(char),44U);});Cyc_Warn_warn(_tmp7FE,_tmp7FD,_tag_dyneither(_tmp11D,sizeof(void*),0U));});});{
# 860
enum Cyc_Absyn_Scope s=Cyc_Absyn_Public;
int istypedef=0;
if(ds.sc != 0){
enum Cyc_Parse_Storage_class _tmp11F=*ds.sc;enum Cyc_Parse_Storage_class _tmp120=_tmp11F;switch(_tmp120){case Cyc_Parse_Typedef_sc: _LL4: _LL5:
 istypedef=1;goto _LL3;case Cyc_Parse_Extern_sc: _LL6: _LL7:
 s=Cyc_Absyn_Extern;goto _LL3;case Cyc_Parse_ExternC_sc: _LL8: _LL9:
 s=Cyc_Absyn_ExternC;goto _LL3;case Cyc_Parse_Static_sc: _LLA: _LLB:
 s=Cyc_Absyn_Static;goto _LL3;case Cyc_Parse_Auto_sc: _LLC: _LLD:
 s=Cyc_Absyn_Public;goto _LL3;case Cyc_Parse_Register_sc: _LLE: _LLF:
 if(Cyc_Parse_no_register)s=Cyc_Absyn_Public;else{s=Cyc_Absyn_Register;}goto _LL3;default: _LL10: _LL11:
 s=Cyc_Absyn_Abstract;goto _LL3;}_LL3:;}{
# 876
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 880
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 888
void*_tmp121=Cyc_Parse_collapse_type_specifiers(_tmp17C,loc);
if(declarators == 0){
# 892
void*_tmp122=_tmp121;struct Cyc_List_List*_tmp162;struct _tuple0*_tmp161;struct _tuple0*_tmp160;int _tmp15F;struct Cyc_List_List*_tmp15E;struct Cyc_Absyn_Datatypedecl**_tmp15D;enum Cyc_Absyn_AggrKind _tmp15C;struct _tuple0*_tmp15B;struct Cyc_List_List*_tmp15A;struct Cyc_Absyn_Datatypedecl*_tmp159;struct Cyc_Absyn_Enumdecl*_tmp158;struct Cyc_Absyn_Aggrdecl*_tmp157;switch(*((int*)_tmp122)){case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp122)->f1)->r)){case 0U: _LL13: _tmp157=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp122)->f1)->r)->f1;_LL14:
# 894
({struct Cyc_List_List*_tmp7FF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp157->attributes,_tmp17B);_tmp157->attributes=_tmp7FF;});
_tmp157->sc=s;{
struct Cyc_List_List*_tmp125=({struct Cyc_List_List*_tmp124=_cycalloc(sizeof(*_tmp124));({struct Cyc_Absyn_Decl*_tmp801=({void*_tmp800=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=5U,_tmp123->f1=_tmp157;_tmp123;});Cyc_Absyn_new_decl(_tmp800,loc);});_tmp124->hd=_tmp801;}),_tmp124->tl=0;_tmp124;});_npop_handler(0U);return _tmp125;};case 1U: _LL15: _tmp158=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp122)->f1)->r)->f1;_LL16:
# 898
 if(_tmp17B != 0)({void*_tmp126=0U;({unsigned int _tmp803=loc;struct _dyneither_ptr _tmp802=({const char*_tmp127="attributes on enum not supported";_tag_dyneither(_tmp127,sizeof(char),33U);});Cyc_Warn_err(_tmp803,_tmp802,_tag_dyneither(_tmp126,sizeof(void*),0U));});});
_tmp158->sc=s;{
struct Cyc_List_List*_tmp12A=({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));({struct Cyc_Absyn_Decl*_tmp805=({void*_tmp804=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=7U,_tmp128->f1=_tmp158;_tmp128;});Cyc_Absyn_new_decl(_tmp804,loc);});_tmp129->hd=_tmp805;}),_tmp129->tl=0;_tmp129;});_npop_handler(0U);return _tmp12A;};default: _LL17: _tmp159=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp122)->f1)->r)->f1;_LL18:
# 902
 if(_tmp17B != 0)({void*_tmp12B=0U;({unsigned int _tmp807=loc;struct _dyneither_ptr _tmp806=({const char*_tmp12C="attributes on datatypes not supported";_tag_dyneither(_tmp12C,sizeof(char),38U);});Cyc_Warn_err(_tmp807,_tmp806,_tag_dyneither(_tmp12B,sizeof(void*),0U));});});
_tmp159->sc=s;{
struct Cyc_List_List*_tmp12F=({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct Cyc_Absyn_Decl*_tmp809=({void*_tmp808=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=6U,_tmp12D->f1=_tmp159;_tmp12D;});Cyc_Absyn_new_decl(_tmp808,loc);});_tmp12E->hd=_tmp809;}),_tmp12E->tl=0;_tmp12E;});_npop_handler(0U);return _tmp12F;};}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).aggr_info).UnknownAggr).tag == 1){_LL19: _tmp15C=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).aggr_info).UnknownAggr).val).f1;_tmp15B=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).aggr_info).UnknownAggr).val).f2;_tmp15A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).targs;_LL1A: {
# 906
struct Cyc_List_List*_tmp130=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15A);
struct Cyc_Absyn_Aggrdecl*_tmp131=({struct Cyc_Absyn_Aggrdecl*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->kind=_tmp15C,_tmp137->sc=s,_tmp137->name=_tmp15B,_tmp137->tvs=_tmp130,_tmp137->impl=0,_tmp137->attributes=0,_tmp137->expected_mem_kind=0;_tmp137;});
if(_tmp17B != 0)({void*_tmp132=0U;({unsigned int _tmp80B=loc;struct _dyneither_ptr _tmp80A=({const char*_tmp133="bad attributes on type declaration";_tag_dyneither(_tmp133,sizeof(char),35U);});Cyc_Warn_err(_tmp80B,_tmp80A,_tag_dyneither(_tmp132,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp136=({struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*_tmp135));({struct Cyc_Absyn_Decl*_tmp80D=({void*_tmp80C=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->tag=5U,_tmp134->f1=_tmp131;_tmp134;});Cyc_Absyn_new_decl(_tmp80C,loc);});_tmp135->hd=_tmp80D;}),_tmp135->tl=0;_tmp135;});_npop_handler(0U);return _tmp136;};}}else{goto _LL23;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).KnownDatatype).tag == 2){_LL1B: _tmp15D=(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).KnownDatatype).val;_LL1C:
# 911
 if(_tmp17B != 0)({void*_tmp138=0U;({unsigned int _tmp80F=loc;struct _dyneither_ptr _tmp80E=({const char*_tmp139="bad attributes on datatype";_tag_dyneither(_tmp139,sizeof(char),27U);});Cyc_Warn_err(_tmp80F,_tmp80E,_tag_dyneither(_tmp138,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13C=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));({struct Cyc_Absyn_Decl*_tmp811=({void*_tmp810=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->tag=6U,_tmp13A->f1=*_tmp15D;_tmp13A;});Cyc_Absyn_new_decl(_tmp810,loc);});_tmp13B->hd=_tmp811;}),_tmp13B->tl=0;_tmp13B;});_npop_handler(0U);return _tmp13C;};}else{_LL1D: _tmp160=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).UnknownDatatype).val).name;_tmp15F=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp15E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).targs;_LL1E: {
# 914
struct Cyc_List_List*_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15E);
struct Cyc_Absyn_Decl*_tmp13E=Cyc_Absyn_datatype_decl(s,_tmp160,_tmp13D,0,_tmp15F,loc);
if(_tmp17B != 0)({void*_tmp13F=0U;({unsigned int _tmp813=loc;struct _dyneither_ptr _tmp812=({const char*_tmp140="bad attributes on datatype";_tag_dyneither(_tmp140,sizeof(char),27U);});Cyc_Warn_err(_tmp813,_tmp812,_tag_dyneither(_tmp13F,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=_tmp13E,_tmp141->tl=0;_tmp141;});_npop_handler(0U);return _tmp142;};}}case 13U: _LL1F: _tmp161=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp122)->f1;_LL20: {
# 919
struct Cyc_Absyn_Enumdecl*_tmp143=({struct Cyc_Absyn_Enumdecl*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->sc=s,_tmp14A->name=_tmp161,_tmp14A->fields=0;_tmp14A;});
if(_tmp17B != 0)({void*_tmp144=0U;({unsigned int _tmp815=loc;struct _dyneither_ptr _tmp814=({const char*_tmp145="bad attributes on enum";_tag_dyneither(_tmp145,sizeof(char),23U);});Cyc_Warn_err(_tmp815,_tmp814,_tag_dyneither(_tmp144,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Decl*_tmp817=({struct Cyc_Absyn_Decl*_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp816=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->tag=7U,_tmp146->f1=_tmp143;_tmp146;});_tmp147->r=_tmp816;}),_tmp147->loc=loc;_tmp147;});_tmp148->hd=_tmp817;}),_tmp148->tl=0;_tmp148;});_npop_handler(0U);return _tmp149;};}case 14U: _LL21: _tmp162=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp122)->f1;_LL22: {
# 925
struct Cyc_Absyn_Enumdecl*_tmp14B=({struct Cyc_Absyn_Enumdecl*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->sc=s,({struct _tuple0*_tmp819=Cyc_Parse_gensym_enum();_tmp153->name=_tmp819;}),({struct Cyc_Core_Opt*_tmp818=({struct Cyc_Core_Opt*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->v=_tmp162;_tmp152;});_tmp153->fields=_tmp818;});_tmp153;});
if(_tmp17B != 0)({void*_tmp14C=0U;({unsigned int _tmp81B=loc;struct _dyneither_ptr _tmp81A=({const char*_tmp14D="bad attributes on enum";_tag_dyneither(_tmp14D,sizeof(char),23U);});Cyc_Warn_err(_tmp81B,_tmp81A,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp151=({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));({struct Cyc_Absyn_Decl*_tmp81D=({struct Cyc_Absyn_Decl*_tmp14F=_cycalloc(sizeof(*_tmp14F));({void*_tmp81C=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->tag=7U,_tmp14E->f1=_tmp14B;_tmp14E;});_tmp14F->r=_tmp81C;}),_tmp14F->loc=loc;_tmp14F;});_tmp150->hd=_tmp81D;}),_tmp150->tl=0;_tmp150;});_npop_handler(0U);return _tmp151;};}default: _LL23: _LL24:
({void*_tmp154=0U;({unsigned int _tmp81F=loc;struct _dyneither_ptr _tmp81E=({const char*_tmp155="missing declarator";_tag_dyneither(_tmp155,sizeof(char),19U);});Cyc_Warn_err(_tmp81F,_tmp81E,_tag_dyneither(_tmp154,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp156=0;_npop_handler(0U);return _tmp156;};}_LL12:;}else{
# 932
struct Cyc_List_List*_tmp163=Cyc_Parse_apply_tmss(mkrgn,_tmp17D,_tmp121,declarators,_tmp17B);
if(istypedef){
# 937
if(!exps_empty)
({void*_tmp164=0U;({unsigned int _tmp821=loc;struct _dyneither_ptr _tmp820=({const char*_tmp165="initializer in typedef declaration";_tag_dyneither(_tmp165,sizeof(char),35U);});Cyc_Warn_err(_tmp821,_tmp820,_tag_dyneither(_tmp164,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp163);
struct Cyc_List_List*_tmp166=decls;_npop_handler(0U);return _tmp166;};}else{
# 943
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp167=_tmp163;for(0;_tmp167 != 0;(_tmp167=_tmp167->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp168=(struct _tuple15*)_tmp167->hd;struct _tuple15*_tmp169=_tmp168;unsigned int _tmp179;struct _tuple0*_tmp178;struct Cyc_Absyn_Tqual _tmp177;void*_tmp176;struct Cyc_List_List*_tmp175;struct Cyc_List_List*_tmp174;_LL26: _tmp179=_tmp169->f1;_tmp178=_tmp169->f2;_tmp177=_tmp169->f3;_tmp176=_tmp169->f4;_tmp175=_tmp169->f5;_tmp174=_tmp169->f6;_LL27:;
if(_tmp175 != 0)
({void*_tmp16A=0U;({unsigned int _tmp823=loc;struct _dyneither_ptr _tmp822=({const char*_tmp16B="bad type params, ignoring";_tag_dyneither(_tmp16B,sizeof(char),26U);});Cyc_Warn_warn(_tmp823,_tmp822,_tag_dyneither(_tmp16A,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp16C=0U;({unsigned int _tmp825=loc;struct _dyneither_ptr _tmp824=({const char*_tmp16D="unexpected NULL in parse!";_tag_dyneither(_tmp16D,sizeof(char),26U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp825,_tmp824,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp16E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp16F=Cyc_Absyn_new_vardecl(_tmp179,_tmp178,_tmp176,_tmp16E);
_tmp16F->tq=_tmp177;
_tmp16F->sc=s;
_tmp16F->attributes=_tmp174;{
struct Cyc_Absyn_Decl*_tmp170=({struct Cyc_Absyn_Decl*_tmp173=_cycalloc(sizeof(*_tmp173));({void*_tmp826=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->tag=0U,_tmp172->f1=_tmp16F;_tmp172;});_tmp173->r=_tmp826;}),_tmp173->loc=loc;_tmp173;});
({struct Cyc_List_List*_tmp827=({struct Cyc_List_List*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->hd=_tmp170,_tmp171->tl=decls;_tmp171;});decls=_tmp827;});};};}}{
# 958
struct Cyc_List_List*_tmp17A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp17A;};}}};};};};}
# 855
;_pop_region(mkrgn);}
# 964
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp17E=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp17F=_tmp17E;switch(_tmp17F){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 974
{char _tmp180=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp181=_tmp180;switch(_tmp181){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 981
goto _LL0;case 84U: _LLF: _LL10:
# 983
{char _tmp182=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp183=_tmp182;switch(_tmp183){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 990
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 993
({struct Cyc_String_pa_PrintArg_struct _tmp186=({struct Cyc_String_pa_PrintArg_struct _tmp6E3;_tmp6E3.tag=0U,_tmp6E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp6E3;});struct Cyc_Int_pa_PrintArg_struct _tmp187=({struct Cyc_Int_pa_PrintArg_struct _tmp6E2;_tmp6E2.tag=1U,({unsigned long _tmp828=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp6E2.f1=_tmp828;});_tmp6E2;});void*_tmp184[2U];_tmp184[0]=& _tmp186,_tmp184[1]=& _tmp187;({unsigned int _tmp82A=loc;struct _dyneither_ptr _tmp829=({const char*_tmp185="bad kind: %s; strlen=%d";_tag_dyneither(_tmp185,sizeof(char),24U);});Cyc_Warn_err(_tmp82A,_tmp829,_tag_dyneither(_tmp184,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 998
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp188=e->r;void*_tmp189=_tmp188;int _tmp18C;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp189)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp189)->f1).Int_c).tag == 5){_LL1: _tmp18C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp189)->f1).Int_c).val).f2;_LL2:
 return _tmp18C;}else{goto _LL3;}}else{_LL3: _LL4:
# 1002
({void*_tmp18A=0U;({unsigned int _tmp82C=loc;struct _dyneither_ptr _tmp82B=({const char*_tmp18B="expecting integer constant";_tag_dyneither(_tmp18B,sizeof(char),27U);});Cyc_Warn_err(_tmp82C,_tmp82B,_tag_dyneither(_tmp18A,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 1008
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp18D=e->r;void*_tmp18E=_tmp18D;struct _dyneither_ptr _tmp191;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18E)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18E)->f1).String_c).tag == 8){_LL1: _tmp191=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18E)->f1).String_c).val;_LL2:
 return _tmp191;}else{goto _LL3;}}else{_LL3: _LL4:
# 1012
({void*_tmp18F=0U;({unsigned int _tmp82E=loc;struct _dyneither_ptr _tmp82D=({const char*_tmp190="expecting string constant";_tag_dyneither(_tmp190,sizeof(char),26U);});Cyc_Warn_err(_tmp82E,_tmp82D,_tag_dyneither(_tmp18F,sizeof(void*),0U));});});
return _tag_dyneither(0,0,0);}_LL0:;}
# 1018
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp192=x;long long _tmp19A;char _tmp199;int _tmp198;switch((_tmp192.LongLong_c).tag){case 5U: _LL1: _tmp198=((_tmp192.Int_c).val).f2;_LL2:
 return(unsigned int)_tmp198;case 2U: _LL3: _tmp199=((_tmp192.Char_c).val).f2;_LL4:
 return(unsigned int)_tmp199;case 6U: _LL5: _tmp19A=((_tmp192.LongLong_c).val).f2;_LL6: {
# 1023
unsigned long long y=(unsigned long long)_tmp19A;
if(y > -1)
({void*_tmp193=0U;({unsigned int _tmp830=loc;struct _dyneither_ptr _tmp82F=({const char*_tmp194="integer constant too large";_tag_dyneither(_tmp194,sizeof(char),27U);});Cyc_Warn_err(_tmp830,_tmp82F,_tag_dyneither(_tmp193,sizeof(void*),0U));});});
return(unsigned int)_tmp19A;}default: _LL7: _LL8:
# 1028
({struct Cyc_String_pa_PrintArg_struct _tmp197=({struct Cyc_String_pa_PrintArg_struct _tmp6E4;_tmp6E4.tag=0U,({struct _dyneither_ptr _tmp831=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp6E4.f1=_tmp831;});_tmp6E4;});void*_tmp195[1U];_tmp195[0]=& _tmp197;({unsigned int _tmp833=loc;struct _dyneither_ptr _tmp832=({const char*_tmp196="expected integer constant but found %s";_tag_dyneither(_tmp196,sizeof(char),39U);});Cyc_Warn_err(_tmp833,_tmp832,_tag_dyneither(_tmp195,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp19B=p->r;void*_tmp19C=_tmp19B;struct Cyc_Absyn_Exp*_tmp1A9;struct _tuple0*_tmp1A8;struct Cyc_List_List*_tmp1A7;struct _dyneither_ptr _tmp1A6;int _tmp1A5;char _tmp1A4;enum Cyc_Absyn_Sign _tmp1A3;int _tmp1A2;struct Cyc_Absyn_Pat*_tmp1A1;struct Cyc_Absyn_Vardecl*_tmp1A0;struct _tuple0*_tmp19F;switch(*((int*)_tmp19C)){case 15U: _LL1: _tmp19F=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LL2:
 return Cyc_Absyn_unknownid_exp(_tmp19F,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp19C)->f2)->r)->tag == 0U){_LL3: _tmp1A0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LL4:
# 1038
 return({struct Cyc_Absyn_Exp*_tmp834=Cyc_Absyn_unknownid_exp(_tmp1A0->name,p->loc);Cyc_Absyn_deref_exp(_tmp834,p->loc);});}else{goto _LL13;}case 6U: _LL5: _tmp1A1=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LL6:
 return({struct Cyc_Absyn_Exp*_tmp835=Cyc_Parse_pat2exp(_tmp1A1);Cyc_Absyn_address_exp(_tmp835,p->loc);});case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp1A3=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_tmp1A2=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp19C)->f2;_LLA:
 return Cyc_Absyn_int_exp(_tmp1A3,_tmp1A2,p->loc);case 11U: _LLB: _tmp1A4=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LLC:
 return Cyc_Absyn_char_exp(_tmp1A4,p->loc);case 12U: _LLD: _tmp1A6=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_tmp1A5=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp19C)->f2;_LLE:
 return Cyc_Absyn_float_exp(_tmp1A6,_tmp1A5,p->loc);case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp19C)->f3 == 0){_LLF: _tmp1A8=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_tmp1A7=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp19C)->f2;_LL10: {
# 1045
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1A8,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp1A7);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp1A9=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LL12:
 return _tmp1A9;default: _LL13: _LL14:
# 1050
({void*_tmp19D=0U;({unsigned int _tmp837=p->loc;struct _dyneither_ptr _tmp836=({const char*_tmp19E="cannot mix patterns and expressions in case";_tag_dyneither(_tmp19E,sizeof(char),44U);});Cyc_Warn_err(_tmp837,_tmp836,_tag_dyneither(_tmp19D,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1AA[7U]="cnst_t";
# 1107 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1AA,_tmp1AA,_tmp1AA + 7U}};
# 1109
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1AB=yy1;union Cyc_Absyn_Cnst _tmp1AC;if((_tmp1AB.Int_tok).tag == 1){_LL1: _tmp1AC=(_tmp1AB.Int_tok).val;_LL2:
# 1113
 yyzzz=_tmp1AC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL0:;}
# 1117
return yyzzz;}
# 1119
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.Int_tok).tag=1U,(_tmp6E5.Int_tok).val=yy1;_tmp6E5;});}static char _tmp1AD[5U]="char";
# 1108 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1AD,_tmp1AD,_tmp1AD + 5U}};
# 1110
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1AE=yy1;char _tmp1AF;if((_tmp1AE.Char_tok).tag == 2){_LL1: _tmp1AF=(_tmp1AE.Char_tok).val;_LL2:
# 1114
 yyzzz=_tmp1AF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL0:;}
# 1118
return yyzzz;}
# 1120
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.Char_tok).tag=2U,(_tmp6E6.Char_tok).val=yy1;_tmp6E6;});}static char _tmp1B0[13U]="string_t<`H>";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1B0,_tmp1B0,_tmp1B0 + 13U}};
# 1111
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1B1=yy1;struct _dyneither_ptr _tmp1B2;if((_tmp1B1.String_tok).tag == 3){_LL1: _tmp1B2=(_tmp1B1.String_tok).val;_LL2:
# 1115
 yyzzz=_tmp1B2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL0:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.String_tok).tag=3U,(_tmp6E7.String_tok).val=yy1;_tmp6E7;});}static char _tmp1B3[56U]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
# 1112 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1B3,_tmp1B3,_tmp1B3 + 56U}};
# 1114
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1B4=yy1;struct _tuple21*_tmp1B5;if((_tmp1B4.YY1).tag == 9){_LL1: _tmp1B5=(_tmp1B4.YY1).val;_LL2:
# 1118
 yyzzz=_tmp1B5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL0:;}
# 1122
return yyzzz;}
# 1124
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY1).tag=9U,(_tmp6E8.YY1).val=yy1;_tmp6E8;});}static char _tmp1B6[19U]="conref_t<bounds_t>";
# 1113 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1B6,_tmp1B6,_tmp1B6 + 19U}};
# 1115
union Cyc_Absyn_Constraint*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp1B7=yy1;union Cyc_Absyn_Constraint*_tmp1B8;if((_tmp1B7.YY2).tag == 10){_LL1: _tmp1B8=(_tmp1B7.YY2).val;_LL2:
# 1119
 yyzzz=_tmp1B8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL0:;}
# 1123
return yyzzz;}
# 1125
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY2).tag=10U,(_tmp6E9.YY2).val=yy1;_tmp6E9;});}static char _tmp1B9[28U]="list_t<offsetof_field_t,`H>";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1B9,_tmp1B9,_tmp1B9 + 28U}};
# 1116
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1BA=yy1;struct Cyc_List_List*_tmp1BB;if((_tmp1BA.YY3).tag == 11){_LL1: _tmp1BB=(_tmp1BA.YY3).val;_LL2:
# 1120
 yyzzz=_tmp1BB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL0:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY3).tag=11U,(_tmp6EA.YY3).val=yy1;_tmp6EA;});}static char _tmp1BC[6U]="exp_t";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1BC,_tmp1BC,_tmp1BC + 6U}};
# 1117
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1BD=yy1;struct Cyc_Absyn_Exp*_tmp1BE;if((_tmp1BD.Exp_tok).tag == 7){_LL1: _tmp1BE=(_tmp1BD.Exp_tok).val;_LL2:
# 1121
 yyzzz=_tmp1BE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL0:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.Exp_tok).tag=7U,(_tmp6EB.Exp_tok).val=yy1;_tmp6EB;});}static char _tmp1BF[17U]="list_t<exp_t,`H>";
# 1123 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1BF,_tmp1BF,_tmp1BF + 17U}};
# 1125
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1C0=yy1;struct Cyc_List_List*_tmp1C1;if((_tmp1C0.YY4).tag == 12){_LL1: _tmp1C1=(_tmp1C0.YY4).val;_LL2:
# 1129
 yyzzz=_tmp1C1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL0:;}
# 1133
return yyzzz;}
# 1135
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY4).tag=12U,(_tmp6EC.YY4).val=yy1;_tmp6EC;});}static char _tmp1C2[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1124 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1C2,_tmp1C2,_tmp1C2 + 47U}};
# 1126
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1C3=yy1;struct Cyc_List_List*_tmp1C4;if((_tmp1C3.YY5).tag == 13){_LL1: _tmp1C4=(_tmp1C3.YY5).val;_LL2:
# 1130
 yyzzz=_tmp1C4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL0:;}
# 1134
return yyzzz;}
# 1136
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY5).tag=13U,(_tmp6ED.YY5).val=yy1;_tmp6ED;});}static char _tmp1C5[9U]="primop_t";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1C5,_tmp1C5,_tmp1C5 + 9U}};
# 1127
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1C6=yy1;enum Cyc_Absyn_Primop _tmp1C7;if((_tmp1C6.YY6).tag == 14){_LL1: _tmp1C7=(_tmp1C6.YY6).val;_LL2:
# 1131
 yyzzz=_tmp1C7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL0:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY6).tag=14U,(_tmp6EE.YY6).val=yy1;_tmp6EE;});}static char _tmp1C8[19U]="opt_t<primop_t,`H>";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1C8,_tmp1C8,_tmp1C8 + 19U}};
# 1128
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1C9=yy1;struct Cyc_Core_Opt*_tmp1CA;if((_tmp1C9.YY7).tag == 15){_LL1: _tmp1CA=(_tmp1C9.YY7).val;_LL2:
# 1132
 yyzzz=_tmp1CA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL0:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY7).tag=15U,(_tmp6EF.YY7).val=yy1;_tmp6EF;});}static char _tmp1CB[7U]="qvar_t";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1CB,_tmp1CB,_tmp1CB + 7U}};
# 1129
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1CC=yy1;struct _tuple0*_tmp1CD;if((_tmp1CC.QualId_tok).tag == 5){_LL1: _tmp1CD=(_tmp1CC.QualId_tok).val;_LL2:
# 1133
 yyzzz=_tmp1CD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL0:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.QualId_tok).tag=5U,(_tmp6F0.QualId_tok).val=yy1;_tmp6F0;});}static char _tmp1CE[7U]="stmt_t";
# 1130 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp1CE,_tmp1CE,_tmp1CE + 7U}};
# 1132
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp1CF=yy1;struct Cyc_Absyn_Stmt*_tmp1D0;if((_tmp1CF.Stmt_tok).tag == 8){_LL1: _tmp1D0=(_tmp1CF.Stmt_tok).val;_LL2:
# 1136
 yyzzz=_tmp1D0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL0:;}
# 1140
return yyzzz;}
# 1142
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.Stmt_tok).tag=8U,(_tmp6F1.Stmt_tok).val=yy1;_tmp6F1;});}static char _tmp1D1[27U]="list_t<switch_clause_t,`H>";
# 1134 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1D1,_tmp1D1,_tmp1D1 + 27U}};
# 1136
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1D2=yy1;struct Cyc_List_List*_tmp1D3;if((_tmp1D2.YY8).tag == 16){_LL1: _tmp1D3=(_tmp1D2.YY8).val;_LL2:
# 1140
 yyzzz=_tmp1D3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL0:;}
# 1144
return yyzzz;}
# 1146
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY8).tag=16U,(_tmp6F2.YY8).val=yy1;_tmp6F2;});}static char _tmp1D4[6U]="pat_t";
# 1135 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1D4,_tmp1D4,_tmp1D4 + 6U}};
# 1137
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp1D5=yy1;struct Cyc_Absyn_Pat*_tmp1D6;if((_tmp1D5.YY9).tag == 17){_LL1: _tmp1D6=(_tmp1D5.YY9).val;_LL2:
# 1141
 yyzzz=_tmp1D6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL0:;}
# 1145
return yyzzz;}
# 1147
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY9).tag=17U,(_tmp6F3.YY9).val=yy1;_tmp6F3;});}static char _tmp1D7[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1140 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1D7,_tmp1D7,_tmp1D7 + 28U}};
# 1142
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1D8=yy1;struct _tuple22*_tmp1D9;if((_tmp1D8.YY10).tag == 18){_LL1: _tmp1D9=(_tmp1D8.YY10).val;_LL2:
# 1146
 yyzzz=_tmp1D9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL0:;}
# 1150
return yyzzz;}
# 1152
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY10).tag=18U,(_tmp6F4.YY10).val=yy1;_tmp6F4;});}static char _tmp1DA[17U]="list_t<pat_t,`H>";
# 1141 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1DA,_tmp1DA,_tmp1DA + 17U}};
# 1143
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1DB=yy1;struct Cyc_List_List*_tmp1DC;if((_tmp1DB.YY11).tag == 19){_LL1: _tmp1DC=(_tmp1DB.YY11).val;_LL2:
# 1147
 yyzzz=_tmp1DC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL0:;}
# 1151
return yyzzz;}
# 1153
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY11).tag=19U,(_tmp6F5.YY11).val=yy1;_tmp6F5;});}static char _tmp1DD[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1DD,_tmp1DD,_tmp1DD + 36U}};
# 1144
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp1DE=yy1;struct _tuple23*_tmp1DF;if((_tmp1DE.YY12).tag == 20){_LL1: _tmp1DF=(_tmp1DE.YY12).val;_LL2:
# 1148
 yyzzz=_tmp1DF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL0:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY12).tag=20U,(_tmp6F6.YY12).val=yy1;_tmp6F6;});}static char _tmp1E0[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1E0,_tmp1E0,_tmp1E0 + 47U}};
# 1145
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E1=yy1;struct Cyc_List_List*_tmp1E2;if((_tmp1E1.YY13).tag == 21){_LL1: _tmp1E2=(_tmp1E1.YY13).val;_LL2:
# 1149
 yyzzz=_tmp1E2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL0:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY13).tag=21U,(_tmp6F7.YY13).val=yy1;_tmp6F7;});}static char _tmp1E3[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1E3,_tmp1E3,_tmp1E3 + 58U}};
# 1146
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1E4=yy1;struct _tuple22*_tmp1E5;if((_tmp1E4.YY14).tag == 22){_LL1: _tmp1E5=(_tmp1E4.YY14).val;_LL2:
# 1150
 yyzzz=_tmp1E5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL0:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY14).tag=22U,(_tmp6F8.YY14).val=yy1;_tmp6F8;});}static char _tmp1E6[9U]="fndecl_t";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1E6,_tmp1E6,_tmp1E6 + 9U}};
# 1147
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp1E7=yy1;struct Cyc_Absyn_Fndecl*_tmp1E8;if((_tmp1E7.YY15).tag == 23){_LL1: _tmp1E8=(_tmp1E7.YY15).val;_LL2:
# 1151
 yyzzz=_tmp1E8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL0:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY15).tag=23U,(_tmp6F9.YY15).val=yy1;_tmp6F9;});}static char _tmp1E9[18U]="list_t<decl_t,`H>";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1E9,_tmp1E9,_tmp1E9 + 18U}};
# 1148
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1EA=yy1;struct Cyc_List_List*_tmp1EB;if((_tmp1EA.YY16).tag == 24){_LL1: _tmp1EB=(_tmp1EA.YY16).val;_LL2:
# 1152
 yyzzz=_tmp1EB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL0:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY16).tag=24U,(_tmp6FA.YY16).val=yy1;_tmp6FA;});}static char _tmp1EC[12U]="decl_spec_t";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1EC,_tmp1EC,_tmp1EC + 12U}};
# 1150
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp1ED=yy1;struct Cyc_Parse_Declaration_spec _tmp1EE;if((_tmp1ED.YY17).tag == 25){_LL1: _tmp1EE=(_tmp1ED.YY17).val;_LL2:
# 1154
 yyzzz=_tmp1EE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL0:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY17).tag=25U,(_tmp6FB.YY17).val=yy1;_tmp6FB;});}static char _tmp1EF[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1149 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1EF,_tmp1EF,_tmp1EF + 31U}};
# 1151
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp1F0=yy1;struct _tuple11 _tmp1F1;if((_tmp1F0.YY18).tag == 26){_LL1: _tmp1F1=(_tmp1F0.YY18).val;_LL2:
# 1155
 yyzzz=_tmp1F1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL0:;}
# 1159
return yyzzz;}
# 1161
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY18).tag=26U,(_tmp6FC.YY18).val=yy1;_tmp6FC;});}static char _tmp1F2[23U]="declarator_list_t<`yy>";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1F2,_tmp1F2,_tmp1F2 + 23U}};
# 1152
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp1F3=yy1;struct _tuple12*_tmp1F4;if((_tmp1F3.YY19).tag == 27){_LL1: _tmp1F4=(_tmp1F3.YY19).val;_LL2:
# 1156
 yyzzz=_tmp1F4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL0:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY19).tag=27U,(_tmp6FD.YY19).val=yy1;_tmp6FD;});}static char _tmp1F5[19U]="storage_class_t@`H";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1F5,_tmp1F5,_tmp1F5 + 19U}};
# 1153
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp1F6=yy1;enum Cyc_Parse_Storage_class*_tmp1F7;if((_tmp1F6.YY20).tag == 28){_LL1: _tmp1F7=(_tmp1F6.YY20).val;_LL2:
# 1157
 yyzzz=_tmp1F7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL0:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY20).tag=28U,(_tmp6FE.YY20).val=yy1;_tmp6FE;});}static char _tmp1F8[17U]="type_specifier_t";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1F8,_tmp1F8,_tmp1F8 + 17U}};
# 1154
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp1F9=yy1;struct Cyc_Parse_Type_specifier _tmp1FA;if((_tmp1F9.YY21).tag == 29){_LL1: _tmp1FA=(_tmp1F9.YY21).val;_LL2:
# 1158
 yyzzz=_tmp1FA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL0:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY21).tag=29U,(_tmp6FF.YY21).val=yy1;_tmp6FF;});}static char _tmp1FB[12U]="aggr_kind_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1FB,_tmp1FB,_tmp1FB + 12U}};
# 1156
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp1FC=yy1;enum Cyc_Absyn_AggrKind _tmp1FD;if((_tmp1FC.YY22).tag == 30){_LL1: _tmp1FD=(_tmp1FC.YY22).val;_LL2:
# 1160
 yyzzz=_tmp1FD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL0:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY22).tag=30U,(_tmp700.YY22).val=yy1;_tmp700;});}static char _tmp1FE[8U]="tqual_t";
# 1155 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1FE,_tmp1FE,_tmp1FE + 8U}};
# 1157
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp1FF=yy1;struct Cyc_Absyn_Tqual _tmp200;if((_tmp1FF.YY23).tag == 31){_LL1: _tmp200=(_tmp1FF.YY23).val;_LL2:
# 1161
 yyzzz=_tmp200;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL0:;}
# 1165
return yyzzz;}
# 1167
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY23).tag=31U,(_tmp701.YY23).val=yy1;_tmp701;});}static char _tmp201[23U]="list_t<aggrfield_t,`H>";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp201,_tmp201,_tmp201 + 23U}};
# 1158
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp202=yy1;struct Cyc_List_List*_tmp203;if((_tmp202.YY24).tag == 32){_LL1: _tmp203=(_tmp202.YY24).val;_LL2:
# 1162
 yyzzz=_tmp203;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL0:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY24).tag=32U,(_tmp702.YY24).val=yy1;_tmp702;});}static char _tmp204[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp204,_tmp204,_tmp204 + 34U}};
# 1159
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp205=yy1;struct Cyc_List_List*_tmp206;if((_tmp205.YY25).tag == 33){_LL1: _tmp206=(_tmp205.YY25).val;_LL2:
# 1163
 yyzzz=_tmp206;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL0:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY25).tag=33U,(_tmp703.YY25).val=yy1;_tmp703;});}static char _tmp207[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp207,_tmp207,_tmp207 + 33U}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp208=yy1;struct Cyc_List_List*_tmp209;if((_tmp208.YY26).tag == 34){_LL1: _tmp209=(_tmp208.YY26).val;_LL2:
# 1164
 yyzzz=_tmp209;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL0:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY26).tag=34U,(_tmp704.YY26).val=yy1;_tmp704;});}static char _tmp20A[18U]="declarator_t<`yy>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp20A,_tmp20A,_tmp20A + 18U}};
# 1161
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp20B=yy1;struct Cyc_Parse_Declarator _tmp20C;if((_tmp20B.YY27).tag == 35){_LL1: _tmp20C=(_tmp20B.YY27).val;_LL2:
# 1165
 yyzzz=_tmp20C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL0:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY27).tag=35U,(_tmp705.YY27).val=yy1;_tmp705;});}static char _tmp20D[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp20D,_tmp20D,_tmp20D + 45U}};
# 1162
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp20E=yy1;struct _tuple24*_tmp20F;if((_tmp20E.YY28).tag == 36){_LL1: _tmp20F=(_tmp20E.YY28).val;_LL2:
# 1166
 yyzzz=_tmp20F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL0:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY28).tag=36U,(_tmp706.YY28).val=yy1;_tmp706;});}static char _tmp210[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp210,_tmp210,_tmp210 + 57U}};
# 1163
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp211=yy1;struct Cyc_List_List*_tmp212;if((_tmp211.YY29).tag == 37){_LL1: _tmp212=(_tmp211.YY29).val;_LL2:
# 1167
 yyzzz=_tmp212;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL0:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY29).tag=37U,(_tmp707.YY29).val=yy1;_tmp707;});}static char _tmp213[26U]="abstractdeclarator_t<`yy>";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp213,_tmp213,_tmp213 + 26U}};
# 1164
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp214=yy1;struct Cyc_Parse_Abstractdeclarator _tmp215;if((_tmp214.YY30).tag == 38){_LL1: _tmp215=(_tmp214.YY30).val;_LL2:
# 1168
 yyzzz=_tmp215;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL0:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY30).tag=38U,(_tmp708.YY30).val=yy1;_tmp708;});}static char _tmp216[5U]="bool";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp216,_tmp216,_tmp216 + 5U}};
# 1165
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp217=yy1;int _tmp218;if((_tmp217.YY31).tag == 39){_LL1: _tmp218=(_tmp217.YY31).val;_LL2:
# 1169
 yyzzz=_tmp218;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL0:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY31).tag=39U,(_tmp709.YY31).val=yy1;_tmp709;});}static char _tmp219[8U]="scope_t";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp219,_tmp219,_tmp219 + 8U}};
# 1166
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp21A=yy1;enum Cyc_Absyn_Scope _tmp21B;if((_tmp21A.YY32).tag == 40){_LL1: _tmp21B=(_tmp21A.YY32).val;_LL2:
# 1170
 yyzzz=_tmp21B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL0:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY32).tag=40U,(_tmp70A.YY32).val=yy1;_tmp70A;});}static char _tmp21C[16U]="datatypefield_t";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp21C,_tmp21C,_tmp21C + 16U}};
# 1167
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp21D=yy1;struct Cyc_Absyn_Datatypefield*_tmp21E;if((_tmp21D.YY33).tag == 41){_LL1: _tmp21E=(_tmp21D.YY33).val;_LL2:
# 1171
 yyzzz=_tmp21E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL0:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY33).tag=41U,(_tmp70B.YY33).val=yy1;_tmp70B;});}static char _tmp21F[27U]="list_t<datatypefield_t,`H>";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp21F,_tmp21F,_tmp21F + 27U}};
# 1168
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp220=yy1;struct Cyc_List_List*_tmp221;if((_tmp220.YY34).tag == 42){_LL1: _tmp221=(_tmp220.YY34).val;_LL2:
# 1172
 yyzzz=_tmp221;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL0:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY34).tag=42U,(_tmp70C.YY34).val=yy1;_tmp70C;});}static char _tmp222[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp222,_tmp222,_tmp222 + 41U}};
# 1169
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp223=yy1;struct _tuple25 _tmp224;if((_tmp223.YY35).tag == 43){_LL1: _tmp224=(_tmp223.YY35).val;_LL2:
# 1173
 yyzzz=_tmp224;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL0:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY35).tag=43U,(_tmp70D.YY35).val=yy1;_tmp70D;});}static char _tmp225[17U]="list_t<var_t,`H>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp225,_tmp225,_tmp225 + 17U}};
# 1170
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp226=yy1;struct Cyc_List_List*_tmp227;if((_tmp226.YY36).tag == 44){_LL1: _tmp227=(_tmp226.YY36).val;_LL2:
# 1174
 yyzzz=_tmp227;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL0:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY36).tag=44U,(_tmp70E.YY36).val=yy1;_tmp70E;});}static char _tmp228[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp228,_tmp228,_tmp228 + 31U}};
# 1171
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp229=yy1;struct _tuple8*_tmp22A;if((_tmp229.YY37).tag == 45){_LL1: _tmp22A=(_tmp229.YY37).val;_LL2:
# 1175
 yyzzz=_tmp22A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL0:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY37).tag=45U,(_tmp70F.YY37).val=yy1;_tmp70F;});}static char _tmp22B[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp22B,_tmp22B,_tmp22B + 42U}};
# 1172
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22C=yy1;struct Cyc_List_List*_tmp22D;if((_tmp22C.YY38).tag == 46){_LL1: _tmp22D=(_tmp22C.YY38).val;_LL2:
# 1176
 yyzzz=_tmp22D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL0:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY38).tag=46U,(_tmp710.YY38).val=yy1;_tmp710;});}static char _tmp22E[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp22E,_tmp22E,_tmp22E + 115U}};
# 1173
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp22F=yy1;struct _tuple26*_tmp230;if((_tmp22F.YY39).tag == 47){_LL1: _tmp230=(_tmp22F.YY39).val;_LL2:
# 1177
 yyzzz=_tmp230;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL0:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY39).tag=47U,(_tmp711.YY39).val=yy1;_tmp711;});}static char _tmp231[18U]="list_t<type_t,`H>";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp231,_tmp231,_tmp231 + 18U}};
# 1174
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp232=yy1;struct Cyc_List_List*_tmp233;if((_tmp232.YY40).tag == 48){_LL1: _tmp233=(_tmp232.YY40).val;_LL2:
# 1178
 yyzzz=_tmp233;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL0:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY40).tag=48U,(_tmp712.YY40).val=yy1;_tmp712;});}static char _tmp234[24U]="list_t<designator_t,`H>";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp234,_tmp234,_tmp234 + 24U}};
# 1176
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp235=yy1;struct Cyc_List_List*_tmp236;if((_tmp235.YY41).tag == 49){_LL1: _tmp236=(_tmp235.YY41).val;_LL2:
# 1180
 yyzzz=_tmp236;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL0:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY41).tag=49U,(_tmp713.YY41).val=yy1;_tmp713;});}static char _tmp237[13U]="designator_t";
# 1175 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp237,_tmp237,_tmp237 + 13U}};
# 1177
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp238=yy1;void*_tmp239;if((_tmp238.YY42).tag == 50){_LL1: _tmp239=(_tmp238.YY42).val;_LL2:
# 1181
 yyzzz=_tmp239;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL0:;}
# 1185
return yyzzz;}
# 1187
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.YY42).tag=50U,(_tmp714.YY42).val=yy1;_tmp714;});}static char _tmp23A[7U]="kind_t";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp23A,_tmp23A,_tmp23A + 7U}};
# 1178
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp23B=yy1;struct Cyc_Absyn_Kind*_tmp23C;if((_tmp23B.YY43).tag == 51){_LL1: _tmp23C=(_tmp23B.YY43).val;_LL2:
# 1182
 yyzzz=_tmp23C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL0:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY43).tag=51U,(_tmp715.YY43).val=yy1;_tmp715;});}static char _tmp23D[7U]="type_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp23D,_tmp23D,_tmp23D + 7U}};
# 1179
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp23E=yy1;void*_tmp23F;if((_tmp23E.YY44).tag == 52){_LL1: _tmp23F=(_tmp23E.YY44).val;_LL2:
# 1183
 yyzzz=_tmp23F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL0:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp716;(_tmp716.YY44).tag=52U,(_tmp716.YY44).val=yy1;_tmp716;});}static char _tmp240[23U]="list_t<attribute_t,`H>";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp240,_tmp240,_tmp240 + 23U}};
# 1180
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp241=yy1;struct Cyc_List_List*_tmp242;if((_tmp241.YY45).tag == 53){_LL1: _tmp242=(_tmp241.YY45).val;_LL2:
# 1184
 yyzzz=_tmp242;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL0:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp717;(_tmp717.YY45).tag=53U,(_tmp717.YY45).val=yy1;_tmp717;});}static char _tmp243[12U]="attribute_t";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp243,_tmp243,_tmp243 + 12U}};
# 1181
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp244=yy1;void*_tmp245;if((_tmp244.YY46).tag == 54){_LL1: _tmp245=(_tmp244.YY46).val;_LL2:
# 1185
 yyzzz=_tmp245;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL0:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp718;(_tmp718.YY46).tag=54U,(_tmp718.YY46).val=yy1;_tmp718;});}static char _tmp246[12U]="enumfield_t";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp246,_tmp246,_tmp246 + 12U}};
# 1182
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp247=yy1;struct Cyc_Absyn_Enumfield*_tmp248;if((_tmp247.YY47).tag == 55){_LL1: _tmp248=(_tmp247.YY47).val;_LL2:
# 1186
 yyzzz=_tmp248;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL0:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp719;(_tmp719.YY47).tag=55U,(_tmp719.YY47).val=yy1;_tmp719;});}static char _tmp249[23U]="list_t<enumfield_t,`H>";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp249,_tmp249,_tmp249 + 23U}};
# 1183
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp24A=yy1;struct Cyc_List_List*_tmp24B;if((_tmp24A.YY48).tag == 56){_LL1: _tmp24B=(_tmp24A.YY48).val;_LL2:
# 1187
 yyzzz=_tmp24B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL0:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71A;(_tmp71A.YY48).tag=56U,(_tmp71A.YY48).val=yy1;_tmp71A;});}static char _tmp24C[11U]="type_opt_t";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp24C,_tmp24C,_tmp24C + 11U}};
# 1184
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp24D=yy1;void*_tmp24E;if((_tmp24D.YY49).tag == 57){_LL1: _tmp24E=(_tmp24D.YY49).val;_LL2:
# 1188
 yyzzz=_tmp24E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL0:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp71B;(_tmp71B.YY49).tag=57U,(_tmp71B.YY49).val=yy1;_tmp71B;});}static char _tmp24F[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp24F,_tmp24F,_tmp24F + 31U}};
# 1185
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp250=yy1;struct Cyc_List_List*_tmp251;if((_tmp250.YY50).tag == 58){_LL1: _tmp251=(_tmp250.YY50).val;_LL2:
# 1189
 yyzzz=_tmp251;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL0:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71C;(_tmp71C.YY50).tag=58U,(_tmp71C.YY50).val=yy1;_tmp71C;});}static char _tmp252[15U]="conref_t<bool>";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp252,_tmp252,_tmp252 + 15U}};
# 1186
union Cyc_Absyn_Constraint*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Constraint*yyzzz;
{union Cyc_YYSTYPE _tmp253=yy1;union Cyc_Absyn_Constraint*_tmp254;if((_tmp253.YY51).tag == 59){_LL1: _tmp254=(_tmp253.YY51).val;_LL2:
# 1190
 yyzzz=_tmp254;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL0:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp71D;(_tmp71D.YY51).tag=59U,(_tmp71D.YY51).val=yy1;_tmp71D;});}static char _tmp255[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp255,_tmp255,_tmp255 + 45U}};
# 1187
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp256=yy1;struct Cyc_List_List*_tmp257;if((_tmp256.YY52).tag == 60){_LL1: _tmp257=(_tmp256.YY52).val;_LL2:
# 1191
 yyzzz=_tmp257;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL0:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp71E;(_tmp71E.YY52).tag=60U,(_tmp71E.YY52).val=yy1;_tmp71E;});}static char _tmp258[20U]="pointer_qual_t<`yy>";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp258,_tmp258,_tmp258 + 20U}};
# 1188
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp259=yy1;void*_tmp25A;if((_tmp259.YY53).tag == 61){_LL1: _tmp25A=(_tmp259.YY53).val;_LL2:
# 1192
 yyzzz=_tmp25A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL0:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp71F;(_tmp71F.YY53).tag=61U,(_tmp71F.YY53).val=yy1;_tmp71F;});}static char _tmp25B[21U]="pointer_quals_t<`yy>";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp25B,_tmp25B,_tmp25B + 21U}};
# 1189
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp25C=yy1;struct Cyc_List_List*_tmp25D;if((_tmp25C.YY54).tag == 62){_LL1: _tmp25D=(_tmp25C.YY54).val;_LL2:
# 1193
 yyzzz=_tmp25D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL0:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp720;(_tmp720.YY54).tag=62U,(_tmp720.YY54).val=yy1;_tmp720;});}static char _tmp25E[21U]="$(bool,string_t<`H>)";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp25E,_tmp25E,_tmp25E + 21U}};
# 1190
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp25F=yy1;struct _tuple20 _tmp260;if((_tmp25F.Asm_tok).tag == 6){_LL1: _tmp260=(_tmp25F.Asm_tok).val;_LL2:
# 1194
 yyzzz=_tmp260;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL0:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp721;(_tmp721.Asm_tok).tag=6U,(_tmp721.Asm_tok).val=yy1;_tmp721;});}static char _tmp261[10U]="exp_opt_t";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp261,_tmp261,_tmp261 + 10U}};
# 1191
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp262=yy1;struct Cyc_Absyn_Exp*_tmp263;if((_tmp262.YY55).tag == 63){_LL1: _tmp263=(_tmp262.YY55).val;_LL2:
# 1195
 yyzzz=_tmp263;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL0:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp722;(_tmp722.YY55).tag=63U,(_tmp722.YY55).val=yy1;_tmp722;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1217
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp723;_tmp723.timestamp=0,_tmp723.first_line=0,_tmp723.first_column=0,_tmp723.last_line=0,_tmp723.last_column=0;_tmp723;});}
# 1220
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1231 "parse.y"
static short Cyc_yytranslate[376U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,147,2,2,131,145,142,2,128,129,136,139,124,143,133,144,2,2,2,2,2,2,2,2,2,2,132,121,126,125,127,138,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,134,2,135,141,130,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,122,140,123,146,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};static char _tmp264[2U]="$";static char _tmp265[6U]="error";static char _tmp266[12U]="$undefined.";static char _tmp267[5U]="AUTO";static char _tmp268[9U]="REGISTER";static char _tmp269[7U]="STATIC";static char _tmp26A[7U]="EXTERN";static char _tmp26B[8U]="TYPEDEF";static char _tmp26C[5U]="VOID";static char _tmp26D[5U]="CHAR";static char _tmp26E[6U]="SHORT";static char _tmp26F[4U]="INT";static char _tmp270[5U]="LONG";static char _tmp271[6U]="FLOAT";static char _tmp272[7U]="DOUBLE";static char _tmp273[7U]="SIGNED";static char _tmp274[9U]="UNSIGNED";static char _tmp275[6U]="CONST";static char _tmp276[9U]="VOLATILE";static char _tmp277[9U]="RESTRICT";static char _tmp278[7U]="STRUCT";static char _tmp279[6U]="UNION";static char _tmp27A[5U]="CASE";static char _tmp27B[8U]="DEFAULT";static char _tmp27C[7U]="INLINE";static char _tmp27D[7U]="SIZEOF";static char _tmp27E[9U]="OFFSETOF";static char _tmp27F[3U]="IF";static char _tmp280[5U]="ELSE";static char _tmp281[7U]="SWITCH";static char _tmp282[6U]="WHILE";static char _tmp283[3U]="DO";static char _tmp284[4U]="FOR";static char _tmp285[5U]="GOTO";static char _tmp286[9U]="CONTINUE";static char _tmp287[6U]="BREAK";static char _tmp288[7U]="RETURN";static char _tmp289[5U]="ENUM";static char _tmp28A[7U]="TYPEOF";static char _tmp28B[16U]="BUILTIN_VA_LIST";static char _tmp28C[10U]="EXTENSION";static char _tmp28D[8U]="NULL_kw";static char _tmp28E[4U]="LET";static char _tmp28F[6U]="THROW";static char _tmp290[4U]="TRY";static char _tmp291[6U]="CATCH";static char _tmp292[7U]="EXPORT";static char _tmp293[4U]="NEW";static char _tmp294[9U]="ABSTRACT";static char _tmp295[9U]="FALLTHRU";static char _tmp296[6U]="USING";static char _tmp297[10U]="NAMESPACE";static char _tmp298[9U]="DATATYPE";static char _tmp299[7U]="MALLOC";static char _tmp29A[8U]="RMALLOC";static char _tmp29B[15U]="RMALLOC_INLINE";static char _tmp29C[7U]="CALLOC";static char _tmp29D[8U]="RCALLOC";static char _tmp29E[5U]="SWAP";static char _tmp29F[9U]="REGION_T";static char _tmp2A0[6U]="TAG_T";static char _tmp2A1[7U]="REGION";static char _tmp2A2[5U]="RNEW";static char _tmp2A3[8U]="REGIONS";static char _tmp2A4[7U]="PORTON";static char _tmp2A5[8U]="PORTOFF";static char _tmp2A6[7U]="PRAGMA";static char _tmp2A7[12U]="DYNREGION_T";static char _tmp2A8[8U]="NUMELTS";static char _tmp2A9[8U]="VALUEOF";static char _tmp2AA[10U]="VALUEOF_T";static char _tmp2AB[9U]="TAGCHECK";static char _tmp2AC[13U]="NUMELTS_QUAL";static char _tmp2AD[10U]="THIN_QUAL";static char _tmp2AE[9U]="FAT_QUAL";static char _tmp2AF[13U]="NOTNULL_QUAL";static char _tmp2B0[14U]="NULLABLE_QUAL";static char _tmp2B1[14U]="REQUIRES_QUAL";static char _tmp2B2[13U]="ENSURES_QUAL";static char _tmp2B3[12U]="REGION_QUAL";static char _tmp2B4[16U]="NOZEROTERM_QUAL";static char _tmp2B5[14U]="ZEROTERM_QUAL";static char _tmp2B6[12U]="TAGGED_QUAL";static char _tmp2B7[16U]="EXTENSIBLE_QUAL";static char _tmp2B8[7U]="PTR_OP";static char _tmp2B9[7U]="INC_OP";static char _tmp2BA[7U]="DEC_OP";static char _tmp2BB[8U]="LEFT_OP";static char _tmp2BC[9U]="RIGHT_OP";static char _tmp2BD[6U]="LE_OP";static char _tmp2BE[6U]="GE_OP";static char _tmp2BF[6U]="EQ_OP";static char _tmp2C0[6U]="NE_OP";static char _tmp2C1[7U]="AND_OP";static char _tmp2C2[6U]="OR_OP";static char _tmp2C3[11U]="MUL_ASSIGN";static char _tmp2C4[11U]="DIV_ASSIGN";static char _tmp2C5[11U]="MOD_ASSIGN";static char _tmp2C6[11U]="ADD_ASSIGN";static char _tmp2C7[11U]="SUB_ASSIGN";static char _tmp2C8[12U]="LEFT_ASSIGN";static char _tmp2C9[13U]="RIGHT_ASSIGN";static char _tmp2CA[11U]="AND_ASSIGN";static char _tmp2CB[11U]="XOR_ASSIGN";static char _tmp2CC[10U]="OR_ASSIGN";static char _tmp2CD[9U]="ELLIPSIS";static char _tmp2CE[11U]="LEFT_RIGHT";static char _tmp2CF[12U]="COLON_COLON";static char _tmp2D0[11U]="IDENTIFIER";static char _tmp2D1[17U]="INTEGER_CONSTANT";static char _tmp2D2[7U]="STRING";static char _tmp2D3[8U]="WSTRING";static char _tmp2D4[19U]="CHARACTER_CONSTANT";static char _tmp2D5[20U]="WCHARACTER_CONSTANT";static char _tmp2D6[18U]="FLOATING_CONSTANT";static char _tmp2D7[9U]="TYPE_VAR";static char _tmp2D8[13U]="TYPEDEF_NAME";static char _tmp2D9[16U]="QUAL_IDENTIFIER";static char _tmp2DA[18U]="QUAL_TYPEDEF_NAME";static char _tmp2DB[10U]="ATTRIBUTE";static char _tmp2DC[4U]="ASM";static char _tmp2DD[4U]="';'";static char _tmp2DE[4U]="'{'";static char _tmp2DF[4U]="'}'";static char _tmp2E0[4U]="','";static char _tmp2E1[4U]="'='";static char _tmp2E2[4U]="'<'";static char _tmp2E3[4U]="'>'";static char _tmp2E4[4U]="'('";static char _tmp2E5[4U]="')'";static char _tmp2E6[4U]="'_'";static char _tmp2E7[4U]="'$'";static char _tmp2E8[4U]="':'";static char _tmp2E9[4U]="'.'";static char _tmp2EA[4U]="'['";static char _tmp2EB[4U]="']'";static char _tmp2EC[4U]="'*'";static char _tmp2ED[4U]="'@'";static char _tmp2EE[4U]="'?'";static char _tmp2EF[4U]="'+'";static char _tmp2F0[4U]="'|'";static char _tmp2F1[4U]="'^'";static char _tmp2F2[4U]="'&'";static char _tmp2F3[4U]="'-'";static char _tmp2F4[4U]="'/'";static char _tmp2F5[4U]="'%'";static char _tmp2F6[4U]="'~'";static char _tmp2F7[4U]="'!'";static char _tmp2F8[5U]="prog";static char _tmp2F9[17U]="translation_unit";static char _tmp2FA[16U]="extern_c_action";static char _tmp2FB[13U]="end_extern_c";static char _tmp2FC[12U]="export_list";static char _tmp2FD[19U]="export_list_values";static char _tmp2FE[21U]="external_declaration";static char _tmp2FF[15U]="optional_comma";static char _tmp300[20U]="function_definition";static char _tmp301[21U]="function_definition2";static char _tmp302[13U]="using_action";static char _tmp303[15U]="unusing_action";static char _tmp304[17U]="namespace_action";static char _tmp305[19U]="unnamespace_action";static char _tmp306[12U]="declaration";static char _tmp307[17U]="declaration_list";static char _tmp308[23U]="declaration_specifiers";static char _tmp309[24U]="storage_class_specifier";static char _tmp30A[15U]="attributes_opt";static char _tmp30B[11U]="attributes";static char _tmp30C[15U]="attribute_list";static char _tmp30D[10U]="attribute";static char _tmp30E[15U]="type_specifier";static char _tmp30F[25U]="type_specifier_notypedef";static char _tmp310[5U]="kind";static char _tmp311[15U]="type_qualifier";static char _tmp312[15U]="enum_specifier";static char _tmp313[11U]="enum_field";static char _tmp314[22U]="enum_declaration_list";static char _tmp315[26U]="struct_or_union_specifier";static char _tmp316[16U]="type_params_opt";static char _tmp317[16U]="struct_or_union";static char _tmp318[24U]="struct_declaration_list";static char _tmp319[25U]="struct_declaration_list0";static char _tmp31A[21U]="init_declarator_list";static char _tmp31B[22U]="init_declarator_list0";static char _tmp31C[16U]="init_declarator";static char _tmp31D[19U]="struct_declaration";static char _tmp31E[25U]="specifier_qualifier_list";static char _tmp31F[35U]="notypedef_specifier_qualifier_list";static char _tmp320[23U]="struct_declarator_list";static char _tmp321[24U]="struct_declarator_list0";static char _tmp322[18U]="struct_declarator";static char _tmp323[20U]="requires_clause_opt";static char _tmp324[19U]="ensures_clause_opt";static char _tmp325[19U]="datatype_specifier";static char _tmp326[14U]="qual_datatype";static char _tmp327[19U]="datatypefield_list";static char _tmp328[20U]="datatypefield_scope";static char _tmp329[14U]="datatypefield";static char _tmp32A[11U]="declarator";static char _tmp32B[23U]="declarator_withtypedef";static char _tmp32C[18U]="direct_declarator";static char _tmp32D[30U]="direct_declarator_withtypedef";static char _tmp32E[8U]="pointer";static char _tmp32F[12U]="one_pointer";static char _tmp330[14U]="pointer_quals";static char _tmp331[13U]="pointer_qual";static char _tmp332[23U]="pointer_null_and_bound";static char _tmp333[14U]="pointer_bound";static char _tmp334[18U]="zeroterm_qual_opt";static char _tmp335[8U]="rgn_opt";static char _tmp336[11U]="tqual_list";static char _tmp337[20U]="parameter_type_list";static char _tmp338[9U]="type_var";static char _tmp339[16U]="optional_effect";static char _tmp33A[19U]="optional_rgn_order";static char _tmp33B[10U]="rgn_order";static char _tmp33C[16U]="optional_inject";static char _tmp33D[11U]="effect_set";static char _tmp33E[14U]="atomic_effect";static char _tmp33F[11U]="region_set";static char _tmp340[15U]="parameter_list";static char _tmp341[22U]="parameter_declaration";static char _tmp342[16U]="identifier_list";static char _tmp343[17U]="identifier_list0";static char _tmp344[12U]="initializer";static char _tmp345[18U]="array_initializer";static char _tmp346[17U]="initializer_list";static char _tmp347[12U]="designation";static char _tmp348[16U]="designator_list";static char _tmp349[11U]="designator";static char _tmp34A[10U]="type_name";static char _tmp34B[14U]="any_type_name";static char _tmp34C[15U]="type_name_list";static char _tmp34D[20U]="abstract_declarator";static char _tmp34E[27U]="direct_abstract_declarator";static char _tmp34F[10U]="statement";static char _tmp350[18U]="labeled_statement";static char _tmp351[21U]="expression_statement";static char _tmp352[19U]="compound_statement";static char _tmp353[16U]="block_item_list";static char _tmp354[20U]="selection_statement";static char _tmp355[15U]="switch_clauses";static char _tmp356[20U]="iteration_statement";static char _tmp357[15U]="jump_statement";static char _tmp358[12U]="exp_pattern";static char _tmp359[20U]="conditional_pattern";static char _tmp35A[19U]="logical_or_pattern";static char _tmp35B[20U]="logical_and_pattern";static char _tmp35C[21U]="inclusive_or_pattern";static char _tmp35D[21U]="exclusive_or_pattern";static char _tmp35E[12U]="and_pattern";static char _tmp35F[17U]="equality_pattern";static char _tmp360[19U]="relational_pattern";static char _tmp361[14U]="shift_pattern";static char _tmp362[17U]="additive_pattern";static char _tmp363[23U]="multiplicative_pattern";static char _tmp364[13U]="cast_pattern";static char _tmp365[14U]="unary_pattern";static char _tmp366[16U]="postfix_pattern";static char _tmp367[16U]="primary_pattern";static char _tmp368[8U]="pattern";static char _tmp369[19U]="tuple_pattern_list";static char _tmp36A[20U]="tuple_pattern_list0";static char _tmp36B[14U]="field_pattern";static char _tmp36C[19U]="field_pattern_list";static char _tmp36D[20U]="field_pattern_list0";static char _tmp36E[11U]="expression";static char _tmp36F[22U]="assignment_expression";static char _tmp370[20U]="assignment_operator";static char _tmp371[23U]="conditional_expression";static char _tmp372[20U]="constant_expression";static char _tmp373[22U]="logical_or_expression";static char _tmp374[23U]="logical_and_expression";static char _tmp375[24U]="inclusive_or_expression";static char _tmp376[24U]="exclusive_or_expression";static char _tmp377[15U]="and_expression";static char _tmp378[20U]="equality_expression";static char _tmp379[22U]="relational_expression";static char _tmp37A[17U]="shift_expression";static char _tmp37B[20U]="additive_expression";static char _tmp37C[26U]="multiplicative_expression";static char _tmp37D[16U]="cast_expression";static char _tmp37E[17U]="unary_expression";static char _tmp37F[15U]="unary_operator";static char _tmp380[19U]="postfix_expression";static char _tmp381[17U]="field_expression";static char _tmp382[19U]="primary_expression";static char _tmp383[25U]="argument_expression_list";static char _tmp384[26U]="argument_expression_list0";static char _tmp385[9U]="constant";static char _tmp386[20U]="qual_opt_identifier";static char _tmp387[17U]="qual_opt_typedef";static char _tmp388[18U]="struct_union_name";static char _tmp389[11U]="field_name";static char _tmp38A[12U]="right_angle";
# 1586 "parse.y"
static struct _dyneither_ptr Cyc_yytname[295U]={{_tmp264,_tmp264,_tmp264 + 2U},{_tmp265,_tmp265,_tmp265 + 6U},{_tmp266,_tmp266,_tmp266 + 12U},{_tmp267,_tmp267,_tmp267 + 5U},{_tmp268,_tmp268,_tmp268 + 9U},{_tmp269,_tmp269,_tmp269 + 7U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 8U},{_tmp26C,_tmp26C,_tmp26C + 5U},{_tmp26D,_tmp26D,_tmp26D + 5U},{_tmp26E,_tmp26E,_tmp26E + 6U},{_tmp26F,_tmp26F,_tmp26F + 4U},{_tmp270,_tmp270,_tmp270 + 5U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 7U},{_tmp273,_tmp273,_tmp273 + 7U},{_tmp274,_tmp274,_tmp274 + 9U},{_tmp275,_tmp275,_tmp275 + 6U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 9U},{_tmp278,_tmp278,_tmp278 + 7U},{_tmp279,_tmp279,_tmp279 + 6U},{_tmp27A,_tmp27A,_tmp27A + 5U},{_tmp27B,_tmp27B,_tmp27B + 8U},{_tmp27C,_tmp27C,_tmp27C + 7U},{_tmp27D,_tmp27D,_tmp27D + 7U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 3U},{_tmp280,_tmp280,_tmp280 + 5U},{_tmp281,_tmp281,_tmp281 + 7U},{_tmp282,_tmp282,_tmp282 + 6U},{_tmp283,_tmp283,_tmp283 + 3U},{_tmp284,_tmp284,_tmp284 + 4U},{_tmp285,_tmp285,_tmp285 + 5U},{_tmp286,_tmp286,_tmp286 + 9U},{_tmp287,_tmp287,_tmp287 + 6U},{_tmp288,_tmp288,_tmp288 + 7U},{_tmp289,_tmp289,_tmp289 + 5U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 16U},{_tmp28C,_tmp28C,_tmp28C + 10U},{_tmp28D,_tmp28D,_tmp28D + 8U},{_tmp28E,_tmp28E,_tmp28E + 4U},{_tmp28F,_tmp28F,_tmp28F + 6U},{_tmp290,_tmp290,_tmp290 + 4U},{_tmp291,_tmp291,_tmp291 + 6U},{_tmp292,_tmp292,_tmp292 + 7U},{_tmp293,_tmp293,_tmp293 + 4U},{_tmp294,_tmp294,_tmp294 + 9U},{_tmp295,_tmp295,_tmp295 + 9U},{_tmp296,_tmp296,_tmp296 + 6U},{_tmp297,_tmp297,_tmp297 + 10U},{_tmp298,_tmp298,_tmp298 + 9U},{_tmp299,_tmp299,_tmp299 + 7U},{_tmp29A,_tmp29A,_tmp29A + 8U},{_tmp29B,_tmp29B,_tmp29B + 15U},{_tmp29C,_tmp29C,_tmp29C + 7U},{_tmp29D,_tmp29D,_tmp29D + 8U},{_tmp29E,_tmp29E,_tmp29E + 5U},{_tmp29F,_tmp29F,_tmp29F + 9U},{_tmp2A0,_tmp2A0,_tmp2A0 + 6U},{_tmp2A1,_tmp2A1,_tmp2A1 + 7U},{_tmp2A2,_tmp2A2,_tmp2A2 + 5U},{_tmp2A3,_tmp2A3,_tmp2A3 + 8U},{_tmp2A4,_tmp2A4,_tmp2A4 + 7U},{_tmp2A5,_tmp2A5,_tmp2A5 + 8U},{_tmp2A6,_tmp2A6,_tmp2A6 + 7U},{_tmp2A7,_tmp2A7,_tmp2A7 + 12U},{_tmp2A8,_tmp2A8,_tmp2A8 + 8U},{_tmp2A9,_tmp2A9,_tmp2A9 + 8U},{_tmp2AA,_tmp2AA,_tmp2AA + 10U},{_tmp2AB,_tmp2AB,_tmp2AB + 9U},{_tmp2AC,_tmp2AC,_tmp2AC + 13U},{_tmp2AD,_tmp2AD,_tmp2AD + 10U},{_tmp2AE,_tmp2AE,_tmp2AE + 9U},{_tmp2AF,_tmp2AF,_tmp2AF + 13U},{_tmp2B0,_tmp2B0,_tmp2B0 + 14U},{_tmp2B1,_tmp2B1,_tmp2B1 + 14U},{_tmp2B2,_tmp2B2,_tmp2B2 + 13U},{_tmp2B3,_tmp2B3,_tmp2B3 + 12U},{_tmp2B4,_tmp2B4,_tmp2B4 + 16U},{_tmp2B5,_tmp2B5,_tmp2B5 + 14U},{_tmp2B6,_tmp2B6,_tmp2B6 + 12U},{_tmp2B7,_tmp2B7,_tmp2B7 + 16U},{_tmp2B8,_tmp2B8,_tmp2B8 + 7U},{_tmp2B9,_tmp2B9,_tmp2B9 + 7U},{_tmp2BA,_tmp2BA,_tmp2BA + 7U},{_tmp2BB,_tmp2BB,_tmp2BB + 8U},{_tmp2BC,_tmp2BC,_tmp2BC + 9U},{_tmp2BD,_tmp2BD,_tmp2BD + 6U},{_tmp2BE,_tmp2BE,_tmp2BE + 6U},{_tmp2BF,_tmp2BF,_tmp2BF + 6U},{_tmp2C0,_tmp2C0,_tmp2C0 + 6U},{_tmp2C1,_tmp2C1,_tmp2C1 + 7U},{_tmp2C2,_tmp2C2,_tmp2C2 + 6U},{_tmp2C3,_tmp2C3,_tmp2C3 + 11U},{_tmp2C4,_tmp2C4,_tmp2C4 + 11U},{_tmp2C5,_tmp2C5,_tmp2C5 + 11U},{_tmp2C6,_tmp2C6,_tmp2C6 + 11U},{_tmp2C7,_tmp2C7,_tmp2C7 + 11U},{_tmp2C8,_tmp2C8,_tmp2C8 + 12U},{_tmp2C9,_tmp2C9,_tmp2C9 + 13U},{_tmp2CA,_tmp2CA,_tmp2CA + 11U},{_tmp2CB,_tmp2CB,_tmp2CB + 11U},{_tmp2CC,_tmp2CC,_tmp2CC + 10U},{_tmp2CD,_tmp2CD,_tmp2CD + 9U},{_tmp2CE,_tmp2CE,_tmp2CE + 11U},{_tmp2CF,_tmp2CF,_tmp2CF + 12U},{_tmp2D0,_tmp2D0,_tmp2D0 + 11U},{_tmp2D1,_tmp2D1,_tmp2D1 + 17U},{_tmp2D2,_tmp2D2,_tmp2D2 + 7U},{_tmp2D3,_tmp2D3,_tmp2D3 + 8U},{_tmp2D4,_tmp2D4,_tmp2D4 + 19U},{_tmp2D5,_tmp2D5,_tmp2D5 + 20U},{_tmp2D6,_tmp2D6,_tmp2D6 + 18U},{_tmp2D7,_tmp2D7,_tmp2D7 + 9U},{_tmp2D8,_tmp2D8,_tmp2D8 + 13U},{_tmp2D9,_tmp2D9,_tmp2D9 + 16U},{_tmp2DA,_tmp2DA,_tmp2DA + 18U},{_tmp2DB,_tmp2DB,_tmp2DB + 10U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 4U},{_tmp2F3,_tmp2F3,_tmp2F3 + 4U},{_tmp2F4,_tmp2F4,_tmp2F4 + 4U},{_tmp2F5,_tmp2F5,_tmp2F5 + 4U},{_tmp2F6,_tmp2F6,_tmp2F6 + 4U},{_tmp2F7,_tmp2F7,_tmp2F7 + 4U},{_tmp2F8,_tmp2F8,_tmp2F8 + 5U},{_tmp2F9,_tmp2F9,_tmp2F9 + 17U},{_tmp2FA,_tmp2FA,_tmp2FA + 16U},{_tmp2FB,_tmp2FB,_tmp2FB + 13U},{_tmp2FC,_tmp2FC,_tmp2FC + 12U},{_tmp2FD,_tmp2FD,_tmp2FD + 19U},{_tmp2FE,_tmp2FE,_tmp2FE + 21U},{_tmp2FF,_tmp2FF,_tmp2FF + 15U},{_tmp300,_tmp300,_tmp300 + 20U},{_tmp301,_tmp301,_tmp301 + 21U},{_tmp302,_tmp302,_tmp302 + 13U},{_tmp303,_tmp303,_tmp303 + 15U},{_tmp304,_tmp304,_tmp304 + 17U},{_tmp305,_tmp305,_tmp305 + 19U},{_tmp306,_tmp306,_tmp306 + 12U},{_tmp307,_tmp307,_tmp307 + 17U},{_tmp308,_tmp308,_tmp308 + 23U},{_tmp309,_tmp309,_tmp309 + 24U},{_tmp30A,_tmp30A,_tmp30A + 15U},{_tmp30B,_tmp30B,_tmp30B + 11U},{_tmp30C,_tmp30C,_tmp30C + 15U},{_tmp30D,_tmp30D,_tmp30D + 10U},{_tmp30E,_tmp30E,_tmp30E + 15U},{_tmp30F,_tmp30F,_tmp30F + 25U},{_tmp310,_tmp310,_tmp310 + 5U},{_tmp311,_tmp311,_tmp311 + 15U},{_tmp312,_tmp312,_tmp312 + 15U},{_tmp313,_tmp313,_tmp313 + 11U},{_tmp314,_tmp314,_tmp314 + 22U},{_tmp315,_tmp315,_tmp315 + 26U},{_tmp316,_tmp316,_tmp316 + 16U},{_tmp317,_tmp317,_tmp317 + 16U},{_tmp318,_tmp318,_tmp318 + 24U},{_tmp319,_tmp319,_tmp319 + 25U},{_tmp31A,_tmp31A,_tmp31A + 21U},{_tmp31B,_tmp31B,_tmp31B + 22U},{_tmp31C,_tmp31C,_tmp31C + 16U},{_tmp31D,_tmp31D,_tmp31D + 19U},{_tmp31E,_tmp31E,_tmp31E + 25U},{_tmp31F,_tmp31F,_tmp31F + 35U},{_tmp320,_tmp320,_tmp320 + 23U},{_tmp321,_tmp321,_tmp321 + 24U},{_tmp322,_tmp322,_tmp322 + 18U},{_tmp323,_tmp323,_tmp323 + 20U},{_tmp324,_tmp324,_tmp324 + 19U},{_tmp325,_tmp325,_tmp325 + 19U},{_tmp326,_tmp326,_tmp326 + 14U},{_tmp327,_tmp327,_tmp327 + 19U},{_tmp328,_tmp328,_tmp328 + 20U},{_tmp329,_tmp329,_tmp329 + 14U},{_tmp32A,_tmp32A,_tmp32A + 11U},{_tmp32B,_tmp32B,_tmp32B + 23U},{_tmp32C,_tmp32C,_tmp32C + 18U},{_tmp32D,_tmp32D,_tmp32D + 30U},{_tmp32E,_tmp32E,_tmp32E + 8U},{_tmp32F,_tmp32F,_tmp32F + 12U},{_tmp330,_tmp330,_tmp330 + 14U},{_tmp331,_tmp331,_tmp331 + 13U},{_tmp332,_tmp332,_tmp332 + 23U},{_tmp333,_tmp333,_tmp333 + 14U},{_tmp334,_tmp334,_tmp334 + 18U},{_tmp335,_tmp335,_tmp335 + 8U},{_tmp336,_tmp336,_tmp336 + 11U},{_tmp337,_tmp337,_tmp337 + 20U},{_tmp338,_tmp338,_tmp338 + 9U},{_tmp339,_tmp339,_tmp339 + 16U},{_tmp33A,_tmp33A,_tmp33A + 19U},{_tmp33B,_tmp33B,_tmp33B + 10U},{_tmp33C,_tmp33C,_tmp33C + 16U},{_tmp33D,_tmp33D,_tmp33D + 11U},{_tmp33E,_tmp33E,_tmp33E + 14U},{_tmp33F,_tmp33F,_tmp33F + 11U},{_tmp340,_tmp340,_tmp340 + 15U},{_tmp341,_tmp341,_tmp341 + 22U},{_tmp342,_tmp342,_tmp342 + 16U},{_tmp343,_tmp343,_tmp343 + 17U},{_tmp344,_tmp344,_tmp344 + 12U},{_tmp345,_tmp345,_tmp345 + 18U},{_tmp346,_tmp346,_tmp346 + 17U},{_tmp347,_tmp347,_tmp347 + 12U},{_tmp348,_tmp348,_tmp348 + 16U},{_tmp349,_tmp349,_tmp349 + 11U},{_tmp34A,_tmp34A,_tmp34A + 10U},{_tmp34B,_tmp34B,_tmp34B + 14U},{_tmp34C,_tmp34C,_tmp34C + 15U},{_tmp34D,_tmp34D,_tmp34D + 20U},{_tmp34E,_tmp34E,_tmp34E + 27U},{_tmp34F,_tmp34F,_tmp34F + 10U},{_tmp350,_tmp350,_tmp350 + 18U},{_tmp351,_tmp351,_tmp351 + 21U},{_tmp352,_tmp352,_tmp352 + 19U},{_tmp353,_tmp353,_tmp353 + 16U},{_tmp354,_tmp354,_tmp354 + 20U},{_tmp355,_tmp355,_tmp355 + 15U},{_tmp356,_tmp356,_tmp356 + 20U},{_tmp357,_tmp357,_tmp357 + 15U},{_tmp358,_tmp358,_tmp358 + 12U},{_tmp359,_tmp359,_tmp359 + 20U},{_tmp35A,_tmp35A,_tmp35A + 19U},{_tmp35B,_tmp35B,_tmp35B + 20U},{_tmp35C,_tmp35C,_tmp35C + 21U},{_tmp35D,_tmp35D,_tmp35D + 21U},{_tmp35E,_tmp35E,_tmp35E + 12U},{_tmp35F,_tmp35F,_tmp35F + 17U},{_tmp360,_tmp360,_tmp360 + 19U},{_tmp361,_tmp361,_tmp361 + 14U},{_tmp362,_tmp362,_tmp362 + 17U},{_tmp363,_tmp363,_tmp363 + 23U},{_tmp364,_tmp364,_tmp364 + 13U},{_tmp365,_tmp365,_tmp365 + 14U},{_tmp366,_tmp366,_tmp366 + 16U},{_tmp367,_tmp367,_tmp367 + 16U},{_tmp368,_tmp368,_tmp368 + 8U},{_tmp369,_tmp369,_tmp369 + 19U},{_tmp36A,_tmp36A,_tmp36A + 20U},{_tmp36B,_tmp36B,_tmp36B + 14U},{_tmp36C,_tmp36C,_tmp36C + 19U},{_tmp36D,_tmp36D,_tmp36D + 20U},{_tmp36E,_tmp36E,_tmp36E + 11U},{_tmp36F,_tmp36F,_tmp36F + 22U},{_tmp370,_tmp370,_tmp370 + 20U},{_tmp371,_tmp371,_tmp371 + 23U},{_tmp372,_tmp372,_tmp372 + 20U},{_tmp373,_tmp373,_tmp373 + 22U},{_tmp374,_tmp374,_tmp374 + 23U},{_tmp375,_tmp375,_tmp375 + 24U},{_tmp376,_tmp376,_tmp376 + 24U},{_tmp377,_tmp377,_tmp377 + 15U},{_tmp378,_tmp378,_tmp378 + 20U},{_tmp379,_tmp379,_tmp379 + 22U},{_tmp37A,_tmp37A,_tmp37A + 17U},{_tmp37B,_tmp37B,_tmp37B + 20U},{_tmp37C,_tmp37C,_tmp37C + 26U},{_tmp37D,_tmp37D,_tmp37D + 16U},{_tmp37E,_tmp37E,_tmp37E + 17U},{_tmp37F,_tmp37F,_tmp37F + 15U},{_tmp380,_tmp380,_tmp380 + 19U},{_tmp381,_tmp381,_tmp381 + 17U},{_tmp382,_tmp382,_tmp382 + 19U},{_tmp383,_tmp383,_tmp383 + 25U},{_tmp384,_tmp384,_tmp384 + 26U},{_tmp385,_tmp385,_tmp385 + 9U},{_tmp386,_tmp386,_tmp386 + 20U},{_tmp387,_tmp387,_tmp387 + 17U},{_tmp388,_tmp388,_tmp388 + 18U},{_tmp389,_tmp389,_tmp389 + 11U},{_tmp38A,_tmp38A,_tmp38A + 12U}};
# 1641
static short Cyc_yyr1[531U]={0,148,149,149,149,149,149,149,149,149,149,149,150,151,152,152,153,153,153,154,154,154,155,155,156,156,156,156,157,157,158,159,160,161,162,162,162,162,162,162,162,163,163,164,164,164,164,164,164,164,164,164,164,164,165,165,165,165,165,165,165,166,166,167,168,168,169,169,169,169,170,170,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,172,173,173,173,174,174,174,175,175,176,176,176,177,177,177,177,177,178,178,179,179,180,180,181,181,182,183,183,184,184,185,186,186,186,186,186,186,187,187,187,187,187,187,188,189,189,190,190,190,190,191,191,192,192,193,193,193,194,194,195,195,195,195,196,196,196,197,197,198,198,199,199,200,200,200,200,200,200,200,200,200,200,201,201,201,201,201,201,201,201,201,201,201,202,202,203,204,204,205,205,205,205,205,205,205,205,206,206,206,207,207,208,208,208,209,209,209,210,210,211,211,211,211,212,212,213,213,214,214,215,215,216,216,217,217,218,218,218,218,219,219,220,220,221,221,221,222,223,223,224,224,225,225,225,225,225,226,226,226,226,227,227,228,228,229,229,230,230,231,231,231,231,231,232,232,233,233,233,234,234,234,234,234,234,234,234,234,234,234,235,235,235,235,235,235,236,237,237,238,238,239,239,239,239,239,239,239,239,240,240,240,240,240,240,241,241,241,241,241,241,242,242,242,242,242,242,242,242,242,242,242,242,242,242,243,243,243,243,243,243,243,243,244,245,245,246,246,247,247,248,248,249,249,250,250,251,251,251,252,252,252,252,252,253,253,253,254,254,254,255,255,255,255,256,256,257,257,257,257,257,257,258,259,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,261,261,261,262,262,263,263,264,264,264,265,265,266,266,267,267,267,268,268,268,268,268,268,268,268,268,268,268,269,269,269,269,269,269,269,270,271,271,272,272,273,273,274,274,275,275,276,276,276,277,277,277,277,277,278,278,278,279,279,279,280,280,280,280,281,281,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,283,283,283,284,284,284,284,284,284,284,284,284,284,284,285,285,285,285,286,286,286,286,286,286,286,286,286,286,286,287,288,288,289,289,289,289,289,290,290,291,291,292,292,293,293,294,294};
# 1697
static short Cyc_yyr2[531U]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,2,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1753
static short Cyc_yydefact[1086U]={0,21,54,55,56,57,59,72,73,74,75,76,77,78,79,80,98,99,100,116,117,50,0,0,84,0,0,60,0,0,154,91,95,0,0,0,0,0,0,0,521,220,523,522,524,0,0,87,0,206,206,205,1,0,0,19,0,0,20,0,43,52,46,70,48,81,82,0,85,0,0,165,0,190,193,86,169,114,58,57,51,0,102,0,45,520,0,521,516,517,518,519,0,114,0,380,0,0,0,0,243,0,382,383,30,32,0,0,0,0,0,0,0,0,0,155,0,0,0,0,0,0,0,203,204,0,2,0,0,0,0,34,0,122,123,125,44,53,47,49,118,525,526,114,114,0,41,0,0,23,0,222,0,178,166,191,0,197,198,201,202,0,200,199,211,193,0,71,58,106,0,104,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,505,506,482,0,0,0,0,0,486,484,485,0,409,411,425,433,435,437,439,441,443,446,451,454,457,461,0,463,487,504,502,521,392,0,0,0,0,0,0,393,391,37,0,0,114,0,0,0,132,128,130,263,265,0,0,39,0,0,9,10,0,0,114,527,528,221,97,0,0,170,88,241,0,238,0,0,3,0,5,0,35,0,0,0,23,0,119,120,145,113,0,152,0,0,0,0,0,0,0,0,0,0,0,0,521,293,295,0,303,297,0,301,286,287,288,0,289,290,291,0,42,23,125,22,24,270,0,228,244,0,0,224,222,0,208,0,0,0,213,61,212,194,0,107,103,0,0,0,471,0,0,483,427,461,0,428,429,0,0,0,0,0,0,0,0,0,0,464,465,0,0,0,0,467,468,466,0,83,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,415,416,417,418,419,420,421,422,423,424,414,0,469,0,493,494,0,0,0,508,0,114,384,0,0,0,406,521,528,0,0,0,0,259,402,407,0,404,0,381,399,400,0,397,0,245,0,0,0,0,266,0,236,133,138,134,136,129,131,222,0,272,264,273,530,529,0,90,94,0,0,0,92,96,112,67,66,0,64,171,222,240,167,272,242,179,180,0,89,207,13,0,31,0,33,0,124,126,247,246,23,25,109,121,0,0,0,140,141,148,0,114,114,160,0,0,0,0,0,521,0,0,0,332,333,334,0,0,336,0,0,0,304,298,125,302,296,294,26,0,177,229,0,0,0,235,223,230,148,0,0,0,224,176,210,209,172,208,0,0,214,62,115,108,432,105,101,0,0,0,0,521,248,253,0,0,0,0,0,0,0,0,0,0,0,0,0,0,507,514,0,513,410,434,0,436,438,440,442,444,445,449,450,447,448,452,453,455,456,458,459,460,413,412,492,489,0,491,0,0,0,395,396,0,262,0,403,257,260,390,0,258,387,0,394,36,0,388,0,267,0,139,135,137,0,224,0,208,0,274,0,222,0,285,269,0,0,0,114,0,0,0,132,0,114,0,222,0,189,168,239,0,7,0,4,6,27,0,144,127,145,0,0,143,224,153,162,161,0,0,156,0,0,0,311,0,0,0,0,0,0,331,335,0,0,0,299,292,0,28,271,222,0,232,0,0,150,225,0,148,228,216,173,195,196,214,192,470,0,0,0,249,0,254,473,0,0,0,0,0,503,478,481,0,0,0,0,462,510,0,0,490,488,0,0,0,0,261,405,408,398,401,389,268,237,148,0,275,276,208,0,0,224,208,0,0,38,93,224,521,0,63,65,0,181,0,0,224,0,208,0,0,8,142,0,146,118,151,163,160,160,0,0,0,0,0,0,0,0,0,0,0,0,0,311,337,0,300,29,224,0,233,231,0,174,0,150,224,0,215,499,0,498,0,250,255,0,0,0,0,0,430,431,492,491,512,495,0,515,426,509,511,0,385,386,150,148,278,284,148,0,277,208,0,118,0,68,182,188,148,0,187,183,208,15,0,16,0,0,0,159,158,305,311,0,0,0,0,0,0,339,340,342,344,346,348,350,352,355,360,363,366,370,372,378,379,0,0,308,317,0,0,0,0,0,0,0,0,0,0,338,218,234,0,226,175,217,222,472,0,0,256,474,475,0,0,480,479,496,0,281,150,150,148,279,40,0,0,150,148,184,14,17,0,147,111,0,0,0,311,0,376,0,0,373,311,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,374,311,0,319,0,0,0,327,0,0,0,0,0,0,310,0,0,224,501,500,0,0,0,497,280,283,150,110,0,185,150,18,164,306,307,0,0,0,0,311,313,343,0,345,347,349,351,353,354,358,359,356,357,361,362,364,365,367,368,369,0,312,318,320,321,0,329,328,0,323,0,0,0,149,227,219,0,0,0,0,282,0,186,309,375,0,371,314,0,311,322,330,324,325,0,252,251,476,0,69,0,341,311,315,326,0,377,316,477,0,0,0};
# 1865
static short Cyc_yydefgoto[147U]={1083,52,53,479,661,863,54,314,55,298,56,481,57,483,58,147,59,60,548,238,465,466,239,63,255,240,65,169,170,66,167,67,275,276,132,133,134,277,241,445,494,495,496,671,812,68,69,676,677,678,70,497,71,470,72,73,164,165,74,123,544,329,714,632,75,633,538,705,530,534,535,439,322,263,99,100,561,486,562,419,420,421,242,315,316,634,451,301,302,303,304,305,306,794,307,308,877,878,879,880,881,882,883,884,885,886,887,888,889,890,891,892,422,430,431,423,424,425,309,202,399,203,553,204,205,206,207,208,209,210,211,212,213,214,215,216,217,819,218,578,579,219,220,77,864,426,455};
# 1883
static short Cyc_yypact[1086U]={2980,- -32768,- -32768,- -32768,- -32768,- 68,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3814,124,- 28,- -32768,3814,3426,- -32768,142,31,- -32768,58,70,38,- 4,88,160,93,216,254,- -32768,264,- -32768,- -32768,- -32768,226,322,285,273,291,291,- -32768,- -32768,314,2841,- -32768,239,255,- -32768,819,3814,3814,3814,- -32768,3814,- -32768,- -32768,938,- -32768,142,3728,132,80,623,993,- -32768,- -32768,298,347,384,- -32768,142,379,6872,- -32768,- -32768,3580,229,- -32768,- -32768,- -32768,- -32768,382,298,6872,- -32768,392,444,3580,452,458,470,- -32768,- 50,- -32768,- -32768,1557,1557,209,511,2841,2841,1557,6872,481,- -32768,279,505,851,516,279,4394,6872,- -32768,- -32768,3118,- -32768,2841,3118,2841,3118,- -32768,526,531,- -32768,3599,- -32768,- -32768,- -32768,- -32768,4394,- -32768,- -32768,298,50,1984,- -32768,3728,819,538,1557,3897,5212,- -32768,132,- -32768,535,- -32768,- -32768,- -32768,- -32768,547,- -32768,- -32768,- 17,993,1557,- -32768,- -32768,569,572,595,142,7266,583,7363,6872,7066,596,602,613,629,639,647,656,660,662,668,7363,7363,- -32768,- -32768,- -32768,2414,677,7460,7460,7460,- -32768,- -32768,- -32768,148,- -32768,- -32768,- 19,646,557,621,670,708,91,598,259,287,- -32768,604,7460,247,55,- -32768,685,86,- -32768,3580,104,725,983,187,3165,702,- -32768,- -32768,713,6872,298,3165,715,4145,4394,4477,4394,236,- -32768,- 15,- 15,- -32768,728,696,- -32768,- -32768,48,316,298,- -32768,- -32768,- -32768,- -32768,32,718,- -32768,- -32768,635,323,- -32768,743,748,- -32768,751,- -32768,753,- -32768,851,5309,3728,538,754,4394,- -32768,764,757,142,765,761,177,763,4544,770,782,772,773,5417,1644,4544,169,767,- -32768,- -32768,775,2129,2129,819,2129,- -32768,- -32768,- -32768,781,- -32768,- -32768,- -32768,199,- -32768,538,784,- -32768,- -32768,771,89,798,- -32768,52,783,785,412,787,665,778,6872,1557,- -32768,800,- -32768,- -32768,89,142,- -32768,6872,797,2414,- -32768,4394,2414,- -32768,- -32768,- -32768,4652,- -32768,839,6872,6872,6872,6872,6872,6872,826,6872,4394,742,- -32768,- -32768,2129,808,349,6872,- -32768,- -32768,- -32768,6872,- -32768,7460,6872,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,6872,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6872,- -32768,279,- -32768,- -32768,5514,279,6872,- -32768,812,298,- -32768,821,825,827,- -32768,78,382,279,6872,3580,413,- -32768,- -32768,- -32768,837,838,831,- -32768,- -32768,- -32768,835,841,3580,- -32768,416,983,840,1557,- -32768,843,848,- -32768,4477,4477,4477,- -32768,- -32768,3332,5611,9,- -32768,278,- -32768,- -32768,52,- -32768,- -32768,850,859,1557,- -32768,- -32768,860,- -32768,853,854,861,- -32768,3201,- -32768,327,369,- -32768,- -32768,- -32768,4394,- -32768,- -32768,- -32768,2702,- -32768,2841,- -32768,2841,- -32768,- -32768,- -32768,- -32768,538,- -32768,- -32768,- -32768,813,6872,870,868,- -32768,- 40,716,298,298,796,6872,6872,866,874,6872,875,970,1403,885,- -32768,- -32768,- -32768,430,964,- -32768,5708,2274,2554,- -32768,- -32768,3599,- -32768,- -32768,- -32768,- -32768,1557,- -32768,- -32768,4394,883,4228,- -32768,- -32768,873,937,52,886,4311,785,- -32768,- -32768,- -32768,- -32768,665,889,145,762,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,890,902,898,921,901,- -32768,- -32768,692,5309,906,912,913,914,915,431,916,918,928,664,919,7166,- -32768,- -32768,930,940,- -32768,646,350,557,621,670,708,91,91,598,598,598,598,259,259,287,287,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,932,- -32768,119,1557,5100,4394,- -32768,4394,- -32768,927,- -32768,- -32768,- -32768,- -32768,1174,- -32768,- -32768,3313,- -32768,- -32768,947,- -32768,150,- -32768,4394,- -32768,- -32768,- -32768,948,785,951,665,946,278,1557,4021,5805,- -32768,- -32768,6872,961,- 15,298,6969,954,32,3463,955,298,1557,3897,5902,- -32768,327,- -32768,965,- -32768,2841,- -32768,- -32768,- -32768,807,- -32768,- -32768,764,962,6872,- -32768,785,- -32768,- -32768,- -32768,966,142,448,434,455,6872,823,461,4544,973,5999,6096,462,- -32768,- -32768,971,977,975,2129,- -32768,3728,- -32768,771,987,1557,- -32768,986,52,1016,- -32768,985,937,61,- -32768,- -32768,- -32768,- -32768,762,- -32768,988,232,988,989,- -32768,4764,- -32768,- -32768,6872,6872,1093,6872,7066,- -32768,- -32768,- -32768,279,279,991,4876,- -32768,- -32768,6872,6872,- -32768,- -32768,89,695,1013,1015,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,937,995,- -32768,- -32768,665,89,997,785,665,992,483,- -32768,- -32768,785,1004,1001,- -32768,- -32768,1002,- -32768,89,1003,785,1005,665,998,302,- -32768,- -32768,6872,- -32768,4394,- -32768,1007,62,796,4544,1014,1008,1229,1006,1017,4544,6872,6193,503,6290,610,6387,823,- -32768,1018,- -32768,- -32768,785,164,- -32768,- -32768,1020,- -32768,1026,1016,785,4394,- -32768,- -32768,342,- -32768,6872,- -32768,- -32768,5309,1021,1022,1024,1019,- -32768,839,1025,1027,- -32768,- -32768,706,- -32768,- -32768,- -32768,- -32768,5100,- -32768,- -32768,1016,937,- -32768,- -32768,937,1028,- -32768,665,1023,4394,1033,- -32768,- -32768,- -32768,937,1030,- -32768,- -32768,665,- -32768,1032,611,1035,1039,4394,- -32768,- -32768,1117,823,1031,7557,1037,2554,7460,1034,- -32768,- 6,- -32768,1074,1029,721,766,220,777,283,486,- -32768,- -32768,- -32768,- -32768,1077,7460,2129,- -32768,- -32768,522,4544,525,6484,4544,549,6581,6678,634,1049,- -32768,- -32768,- -32768,6872,1050,- -32768,- -32768,987,- -32768,249,356,- -32768,- -32768,- -32768,4394,1150,- -32768,- -32768,- -32768,4988,- -32768,1016,1016,937,- -32768,- -32768,1053,1054,1016,937,- -32768,- -32768,- -32768,481,- -32768,- -32768,550,4544,1056,823,2414,- -32768,4394,1048,- -32768,1839,7460,6872,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,7460,6872,- -32768,823,1059,- -32768,4544,4544,563,- -32768,4544,4544,586,4544,594,6775,- -32768,1052,52,785,- -32768,- -32768,2554,1055,1057,- -32768,- -32768,- -32768,1016,- -32768,1079,- -32768,1016,- -32768,- -32768,- -32768,- -32768,1068,1065,1071,7460,823,- -32768,646,399,557,621,621,708,91,91,598,598,598,598,259,259,287,287,- -32768,- -32768,- -32768,425,- -32768,- -32768,- -32768,- -32768,4544,- -32768,- -32768,4544,- -32768,4544,4544,597,- -32768,- -32768,- -32768,1073,745,1069,4394,- -32768,1070,- -32768,- -32768,988,232,- -32768,- -32768,6872,1839,- -32768,- -32768,- -32768,- -32768,4544,- -32768,- -32768,- -32768,1072,- -32768,362,- -32768,823,- -32768,- -32768,1075,- -32768,- -32768,- -32768,1197,1200,- -32768};
# 1995
static short Cyc_yypgoto[147U]={- -32768,153,- -32768,- -32768,- -32768,261,- -32768,- 221,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 46,- 107,44,- -32768,- -32768,0,556,- -32768,95,- 173,1086,23,- -32768,- -32768,- 138,- -32768,182,1170,- 766,- -32768,- -32768,- -32768,941,933,746,351,- -32768,- -32768,543,- 9,- 567,- -32768,- -32768,97,- -32768,- -32768,85,- 140,1141,- 380,- 30,- -32768,1051,- -32768,- -32768,1168,- 512,- -32768,507,- 124,- 159,- 149,- 33,234,514,539,- 415,- 487,- 120,- 368,- 125,- -32768,- 263,- 162,- 568,- 306,- -32768,824,- 111,- 71,- 114,- 165,- 390,204,- -32768,- -32768,- 38,- 285,- -32768,- 564,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,116,1010,- -32768,632,816,- -32768,174,654,- -32768,- 157,- 355,- 160,- 360,- 350,- 365,880,- 351,- 345,- 321,- 301,- 280,- 185,855,464,891,194,- -32768,- 354,- -32768,293,542,- 63,- 20,- 113,68};
# 2017
static short Cyc_yytable[7705U]={61,262,321,256,142,330,584,256,581,485,363,364,365,520,521,345,523,346,866,342,583,61,586,64,146,61,323,320,273,587,588,400,149,710,336,243,244,669,563,642,742,250,78,155,64,702,118,143,64,463,604,142,332,489,61,589,590,591,592,637,61,61,61,613,61,80,443,674,675,84,61,153,234,452,574,368,450,64,235,593,594,637,360,64,64,64,934,64,954,146,526,657,670,64,252,62,472,274,41,299,83,310,595,596,136,137,138,658,139,311,61,61,453,328,148,531,62,110,657,369,62,469,706,756,454,61,440,61,61,61,61,119,955,64,64,61,452,447,666,105,464,752,101,448,135,61,108,61,64,62,64,64,64,64,153,62,62,62,64,62,533,407,699,693,109,62,835,41,64,529,64,658,459,540,532,453,166,452,552,148,376,377,319,280,106,- 157,223,454,40,300,- 222,148,408,- 222,223,551,107,43,474,597,598,599,222,258,224,62,62,126,46,111,- 527,449,224,527,230,474,453,378,379,411,62,113,62,62,62,62,555,310,556,557,62,471,40,312,412,488,19,20,907,442,62,43,62,366,572,845,81,913,498,849,40,45,299,299,740,299,547,201,150,43,151,552,444,248,249,860,152,664,227,443,443,443,366,61,712,226,928,367,265,751,266,267,268,269,454,40,112,251,602,454,516,61,605,910,43,533,64,517,61,61,721,61,563,454,612,503,115,946,504,962,963,366,456,299,64,783,427,148,460,102,525,64,64,366,64,279,281,791,651,549,245,401,402,403,246,84,552,223,932,410,253,818,300,300,429,300,964,965,254,938,- 244,429,651,- 244,117,224,312,253,992,61,127,128,998,999,447,254,626,361,62,1003,448,116,49,50,51,404,129,130,533,102,405,406,64,1009,528,522,62,253,645,1014,735,102,120,62,62,254,62,45,382,262,550,121,383,300,638,652,639,434,474,805,40,1035,640,122,824,696,435,449,42,43,44,440,968,384,166,862,969,865,563,474,40,385,386,1054,462,474,125,1056,471,43,366,45,442,442,442,461,45,475,915,1061,46,641,476,653,62,654,698,823,49,50,51,655,498,687,514,444,444,444,650,- 12,656,916,694,366,366,917,146,40,576,61,366,61,738,61,697,42,43,44,994,508,760,1080,665,741,168,917,515,468,743,1077,744,172,64,448,64,775,64,709,225,61,771,361,1081,552,361,759,102,440,61,102,228,102,61,366,758,771,569,704,102,776,774,1062,64,319,824,614,539,623,615,773,366,64,76,582,533,64,417,418,622,366,807,690,229,148,366,366,989,1063,366,815,727,919,148,789,82,829,148,830,103,787,104,713,788,231,62,706,62,823,62,366,606,837,232,801,790,366,366,652,76,40,795,608,1018,1019,1015,233,76,42,43,44,754,76,474,820,62,474,1017,366,1020,141,975,144,851,62,76,1021,1022,62,831,832,498,824,970,171,901,552,247,366,103,808,971,972,660,257,662,498,663,641,498,784,103,1023,1024,1025,1026,259,366,270,299,366,310,976,76,76,978,271,141,656,806,313,76,61,387,326,823,1027,1028,76,1013,76,76,76,76,366,475,327,679,680,982,1006,683,672,673,688,64,380,381,366,1029,1030,76,952,1039,333,61,334,61,371,814,388,389,390,391,392,393,394,395,396,397,974,366,339,102,765,171,1042,142,64,366,64,335,366,695,1044,347,102,1068,848,102,398,348,904,940,852,366,941,713,749,300,370,148,349,858,40,843,542,543,944,731,402,403,42,43,44,552,987,62,350,366,49,50,51,372,468,951,103,991,351,103,448,103,49,50,51,909,352,264,103,1076,16,17,18,914,85,353,1031,1032,1033,354,62,355,62,404,629,630,631,356,732,406,374,375,674,675,473,993,362,325,409,184,838,432,995,373,76,780,719,720,763,839,840,473,433,500,458,40,505,846,1060,766,926,927,533,42,43,44,929,457,1010,930,1011,413,856,76,437,492,792,793,467,936,299,40,88,190,191,89,90,91,960,961,43,798,800,958,959,966,967,477,261,1070,366,340,478,40,194,480,171,482,490,142,499,42,43,44,1050,868,869,278,501,695,502,510,506,492,511,512,61,493,261,509,518,49,50,51,519,524,1075,529,299,893,272,454,102,536,545,102,40,541,537,64,45,554,40,1000,42,43,44,487,40,1004,42,43,44,45,368,570,492,43,575,607,300,131,492,1072,49,50,51,820,46,609,49,50,51,610,61,611,49,50,51,1049,40,617,103,618,619,620,621,627,644,43,625,898,900,628,903,103,906,64,103,643,46,546,647,646,648,441,649,446,49,50,51,62,667,668,870,681,918,682,300,487,897,685,564,565,566,567,568,689,684,571,691,473,700,703,473,669,707,577,299,711,715,580,76,278,76,85,76,716,717,338,718,341,343,343,- 527,473,722,723,724,725,726,473,600,733,357,358,728,40,729,62,227,343,343,343,601,42,43,44,730,577,736,140,739,745,61,737,156,157,158,159,160,750,343,161,162,163,980,86,753,984,986,755,757,764,769,772,102,64,779,414,785,782,415,88,802,811,89,90,91,803,416,43,796,636,977,804,93,981,300,319,809,734,94,813,95,96,821,417,418,827,97,833,841,361,842,844,98,847,850,853,1016,854,855,857,861,859,867,871,872,895,908,896,912,935,923,933,945,341,1034,911,1007,920,921,922,947,924,939,925,931,62,937,103,1046,943,103,942,950,953,956,1051,957,973,577,988,577,990,996,1001,1012,1002,1008,1036,1047,1037,1038,1052,1053,1040,1041,1055,1043,343,1057,473,261,1058,1059,1069,1084,1071,1073,1085,1079,1005,76,1082,770,260,473,114,491,473,781,484,154,261,85,331,487,124,786,817,261,816,343,1048,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,810,1064,616,436,1065,573,1066,1067,747,624,585,1074,873,874,894,0,0,0,0,0,487,0,0,261,0,86,0,0,85,0,1078,343,0,0,261,0,0,746,0,0,415,88,0,261,89,90,91,0,416,43,0,0,762,0,93,0,0,0,0,768,94,0,95,96,0,417,418,778,97,0,0,0,0,0,98,0,0,0,0,141,86,0,0,0,0,0,0,0,0,0,0,0,103,577,0,221,88,0,0,89,90,91,0,92,43,0,343,0,0,93,0,0,0,0,0,875,0,95,96,0,0,0,0,97,0,0,876,0,0,98,198,0,487,199,200,825,826,0,828,0,0,0,0,261,0,0,487,0,0,836,0,0,0,0,441,0,0,0,261,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,343,0,0,0,0,0,0,0,0,0,22,23,24,291,85,26,176,0,0,0,177,27,0,0,0,30,178,179,180,181,182,0,31,32,33,183,0,0,0,184,36,185,186,37,187,0,0,0,487,0,0,0,0,141,0,38,39,0,188,189,0,0,0,0,487,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,41,42,43,44,45,192,686,343,0,0,0,0,278,193,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,261,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,487,343,0,0,0,0,0,0,0,0,0,0,343,22,23,24,0,278,0,0,0,0,0,0,0,0,0,0,30,0,0,0,261,0,0,31,32,0,0,236,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,343,0,38,39,0,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,0,41,42,0,44,45,0,0,237,0,22,23,24,291,85,0,47,48,0,0,0,27,0,0,0,30,178,179,180,181,182,0,31,32,0,0,0,0,0,184,36,185,186,37,187,0,0,0,0,0,0,0,0,0,0,38,39,949,188,189,343,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,343,0,0,40,88,190,191,89,90,91,41,42,43,44,45,192,0,343,0,0,0,0,0,340,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,343,0,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,792,793,21,173,174,282,343,283,284,285,286,287,288,289,290,22,23,24,291,85,26,176,292,0,0,177,27,293,0,0,30,178,179,180,181,182,0,31,32,33,183,0,0,0,184,36,185,186,37,187,0,0,0,0,0,0,343,0,0,0,38,39,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,294,88,190,191,89,90,91,41,42,43,44,45,192,295,145,0,0,0,0,0,193,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,282,0,283,284,285,286,287,288,289,290,22,23,24,291,85,26,176,292,0,0,177,27,293,0,0,30,178,179,180,181,182,0,31,32,33,183,0,0,0,184,36,185,186,37,187,0,0,0,0,0,0,0,0,0,0,38,39,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,294,88,190,191,89,90,91,41,42,43,44,45,192,295,145,296,0,0,0,0,193,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,282,0,283,284,285,286,287,288,289,290,22,23,24,291,85,26,176,292,0,0,177,27,293,0,0,30,178,179,180,181,182,0,31,32,33,183,0,0,0,184,36,185,186,37,187,0,0,0,0,0,0,0,0,0,0,38,39,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,294,88,190,191,89,90,91,41,42,43,44,45,192,295,145,0,0,0,0,0,193,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,173,174,282,0,283,284,285,286,287,288,289,290,22,23,24,291,85,26,176,292,0,0,177,27,293,0,0,30,178,179,180,181,182,0,31,32,33,183,0,0,0,184,36,185,186,37,187,0,0,0,0,0,0,0,0,0,0,38,39,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,507,88,190,191,89,90,91,41,42,43,44,45,192,295,145,0,0,0,0,0,193,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,23,24,175,85,0,176,0,0,0,177,0,0,0,0,30,178,179,180,181,182,0,31,32,0,183,0,0,0,184,36,185,186,37,187,0,0,0,0,0,0,0,0,0,0,38,39,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,41,42,43,44,45,192,0,359,0,0,0,0,0,193,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,173,174,0,0,0,0,0,0,0,0,0,0,22,23,24,175,85,0,176,0,0,0,177,0,0,0,0,30,178,179,180,181,182,0,31,32,0,183,0,0,0,184,36,185,186,37,187,0,0,0,0,0,0,0,0,0,0,38,39,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,41,42,43,44,45,192,0,0,0,0,0,0,0,193,0,47,297,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,659,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,0,0,0,41,42,43,44,45,0,0,0,- 11,0,0,0,0,46,0,47,48,0,0,0,0,49,50,51,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,0,0,0,41,42,43,44,45,0,0,0,- 11,0,0,0,0,46,0,47,48,0,0,0,0,49,50,51,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,0,0,0,0,0,0,41,42,43,44,45,0,0,0,0,0,0,0,0,46,0,47,48,0,0,0,0,49,50,51,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,85,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,40,0,0,0,0,0,0,41,42,43,44,45,22,23,24,- 11,0,0,0,0,46,0,47,48,0,0,0,30,49,50,51,0,86,0,31,32,0,0,0,0,0,0,36,0,428,37,0,221,88,0,0,89,90,91,0,92,43,38,39,0,0,93,0,0,0,0,0,94,0,95,96,0,0,0,0,97,0,0,0,0,317,98,0,40,0,0,0,0,0,0,41,42,43,44,45,0,319,0,0,0,0,0,0,468,0,47,48,0,0,448,0,49,50,51,7,8,9,10,11,12,13,14,15,16,17,18,19,20,85,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,86,0,0,0,0,0,0,0,38,39,0,0,748,0,0,221,88,0,0,89,90,91,0,92,43,0,0,0,0,93,0,317,0,0,0,94,0,95,96,0,0,41,42,97,44,45,0,319,0,98,0,0,0,0,447,0,47,48,0,0,448,85,49,50,51,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,86,0,0,31,32,0,0,0,0,0,0,36,0,0,37,87,88,0,0,89,90,91,0,92,43,0,38,39,0,93,0,0,0,0,0,94,0,95,96,0,0,0,0,97,0,0,0,0,0,98,0,0,40,0,0,0,0,0,0,41,42,43,44,45,0,0,0,0,0,0,0,0,492,0,47,48,0,0,0,0,49,50,51,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,85,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,36,0,0,37,0,0,0,86,0,0,0,0,0,0,0,38,39,0,0,0,0,0,221,88,0,0,89,90,91,0,92,43,0,0,0,0,93,0,0,0,0,0,94,0,95,96,0,0,41,42,97,44,45,0,0,145,98,0,272,0,0,0,0,47,48,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,2,3,4,79,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,41,42,0,44,45,0,0,145,22,23,24,25,0,0,0,47,48,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,41,42,0,44,45,22,23,24,0,0,0,0,0,0,0,47,48,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,317,0,0,318,0,0,0,0,0,0,41,42,0,44,45,0,319,0,0,0,0,0,0,0,0,47,48,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,317,0,0,0,0,0,0,0,0,0,41,42,0,44,45,0,319,0,0,0,0,0,0,0,0,47,48,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,41,42,0,44,45,22,23,24,438,0,0,0,0,0,0,47,48,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,41,42,0,44,45,22,23,24,701,0,0,0,0,0,0,47,48,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,708,0,0,0,0,0,0,0,0,0,41,42,0,44,45,22,23,24,0,0,0,0,0,0,0,47,48,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,41,42,0,44,45,22,23,24,0,0,0,0,0,0,0,47,48,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,36,0,0,37,0,0,0,0,0,0,0,0,0,0,0,38,39,0,0,0,0,0,0,0,0,173,174,282,0,283,284,285,286,287,288,289,290,0,0,0,175,85,0,176,292,0,0,177,41,293,0,0,45,178,179,180,181,182,0,0,0,0,183,47,48,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,507,88,190,191,89,90,91,0,0,43,0,0,192,295,145,0,0,0,0,0,193,0,0,194,0,173,174,0,195,0,0,196,558,0,197,198,0,0,199,200,175,85,0,176,0,0,0,177,0,0,0,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,559,88,190,191,89,90,91,0,254,43,0,0,192,0,344,560,0,0,0,0,193,0,0,194,0,417,418,0,195,173,174,196,0,0,197,198,0,0,199,200,0,0,0,0,175,85,0,176,0,0,0,177,0,0,0,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,559,88,190,191,89,90,91,0,254,43,0,0,192,0,344,822,0,0,0,0,193,0,0,194,0,417,418,0,195,173,174,196,0,0,197,198,0,0,199,200,0,0,0,0,175,85,0,176,0,0,0,177,0,0,0,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,559,88,190,191,89,90,91,0,254,43,0,0,192,0,344,834,0,0,0,0,193,0,0,194,0,417,418,0,195,173,174,196,0,0,197,198,0,0,199,200,0,0,0,0,175,85,0,176,0,0,0,177,0,0,0,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,559,88,190,191,89,90,91,0,254,43,0,0,192,0,344,997,0,0,0,0,193,0,0,194,0,417,418,0,195,173,174,196,0,0,197,198,0,0,199,200,0,0,0,0,175,85,0,176,0,0,0,177,0,0,0,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,559,88,190,191,89,90,91,0,254,43,0,0,192,0,344,0,0,0,0,0,193,0,0,194,0,417,418,0,195,173,174,196,0,0,197,198,0,0,199,200,0,0,0,0,175,85,0,176,0,0,0,177,0,0,0,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,0,0,194,0,0,0,324,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,344,0,0,0,0,0,193,0,0,194,0,173,174,0,195,0,0,196,0,0,197,198,0,0,199,200,175,85,0,176,0,0,0,177,0,0,0,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,513,173,174,0,0,0,0,193,0,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,603,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,0,0,194,0,0,0,635,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,692,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,0,0,194,0,0,0,761,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,0,0,194,0,0,0,777,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,797,173,174,0,0,0,0,193,0,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,799,173,174,0,0,0,0,193,0,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,899,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,902,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,905,173,174,0,0,0,0,193,0,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,979,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,983,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,985,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,1045,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,0,0,194,0,0,0,0,195,175,85,196,176,0,197,198,177,0,199,200,0,0,178,179,180,181,182,0,0,0,0,183,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,767,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,0,0,194,0,0,0,0,195,175,85,196,0,0,197,198,0,0,199,200,0,0,178,179,180,181,182,0,0,0,0,0,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,344,0,0,173,174,0,193,0,0,194,0,0,0,0,195,0,0,196,175,85,197,198,0,0,199,200,0,0,0,0,0,178,179,180,181,182,0,0,0,0,0,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,734,0,0,173,174,0,193,0,0,194,0,0,0,0,195,0,0,196,175,85,197,198,0,0,199,200,0,0,0,0,0,178,179,180,181,182,0,0,0,0,0,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,337,0,0,194,0,0,0,0,195,175,85,196,0,0,197,198,0,0,199,200,0,0,178,179,180,181,182,0,0,0,0,0,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,340,0,0,194,0,0,0,0,195,175,85,196,0,0,197,198,0,0,199,200,0,0,178,179,180,181,182,0,0,0,0,0,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,173,174,0,0,0,0,193,0,0,194,0,0,0,0,195,175,85,196,0,0,197,198,0,0,199,200,0,0,178,179,180,181,182,0,0,0,0,0,0,0,0,184,0,185,186,0,187,0,0,0,0,0,0,0,0,0,0,0,0,0,188,189,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,88,190,191,89,90,91,0,0,43,0,0,192,0,0,0,0,0,0,0,948,0,0,194,0,0,0,0,195,0,0,196,0,0,197,198,0,0,199,200};
# 2791
static short Cyc_yycheck[7705U]={0,121,151,116,67,164,371,120,368,272,195,196,197,298,299,177,301,177,784,176,370,21,373,0,70,25,151,151,135,374,375,216,70,545,172,106,107,77,344,454,608,112,110,73,21,532,46,67,25,17,404,114,166,274,54,376,377,378,379,449,60,61,62,418,64,21,239,5,6,25,70,71,122,88,359,94,241,54,128,380,381,471,193,60,61,62,852,64,94,135,311,471,132,70,114,0,261,135,115,145,128,147,382,383,60,61,62,475,64,147,110,111,127,130,70,63,21,121,498,138,25,261,537,635,139,125,237,127,128,129,130,46,138,110,111,135,88,128,493,108,108,628,26,134,59,145,108,147,125,54,127,128,129,130,154,60,61,62,135,64,319,106,530,517,126,70,734,115,145,108,147,539,124,322,122,127,126,88,335,135,89,90,121,133,126,123,108,139,108,145,129,147,137,132,108,333,126,117,261,384,385,386,86,118,126,110,111,54,128,121,132,241,126,124,98,278,127,126,127,115,125,128,127,128,129,130,337,273,339,340,135,261,108,148,130,273,20,21,802,239,145,117,147,124,355,757,122,814,278,761,108,119,298,299,135,301,327,83,126,117,128,418,239,110,111,777,134,488,94,442,443,444,124,273,129,93,843,129,125,129,127,128,129,130,139,108,126,113,401,139,121,291,405,129,117,454,273,128,298,299,563,301,608,139,417,128,52,871,131,89,90,124,244,359,291,670,129,273,250,26,121,298,299,124,301,143,144,681,468,329,121,84,85,86,125,291,493,108,850,223,108,109,298,299,228,301,126,127,116,861,121,235,492,124,128,126,271,108,109,359,121,122,929,930,128,116,437,193,273,936,134,107,136,137,138,128,121,122,537,86,133,134,359,947,316,300,291,108,459,953,575,98,107,298,299,116,301,119,139,519,332,128,143,359,126,468,128,233,471,694,108,975,134,122,720,522,234,447,116,117,118,532,139,136,126,123,143,782,734,492,108,144,145,1000,252,498,122,1004,468,117,124,119,442,443,444,129,119,124,816,1013,128,451,129,126,359,128,527,720,136,137,138,134,492,509,290,442,443,444,468,122,470,129,518,124,124,133,522,108,129,479,124,481,132,483,522,116,117,118,132,285,639,129,492,607,110,133,292,128,609,1063,611,122,479,134,481,654,483,540,126,509,650,337,1076,670,340,639,223,628,518,226,128,228,522,124,638,665,352,536,235,654,654,132,509,121,840,419,124,121,125,653,124,518,0,369,703,522,133,134,432,124,699,121,108,509,124,124,911,132,124,708,129,824,518,129,22,727,522,727,26,121,28,548,124,121,479,990,481,840,483,124,406,738,124,121,129,124,124,650,46,108,129,409,957,958,954,125,54,116,117,118,633,59,665,716,509,668,956,124,959,67,895,69,129,518,72,960,961,522,731,732,650,927,136,81,121,782,115,124,86,700,144,145,479,128,481,665,483,637,668,672,98,962,963,964,965,129,124,121,694,124,696,129,110,111,129,124,114,657,696,121,118,661,58,128,927,966,967,125,953,127,128,129,130,124,124,128,502,503,129,129,506,499,500,509,661,87,88,124,968,969,148,876,129,124,694,123,696,140,707,95,96,97,98,99,100,101,102,103,104,894,124,128,419,645,172,129,779,694,124,696,125,124,518,129,128,432,129,760,435,125,128,121,121,766,124,124,713,621,694,93,696,128,775,108,753,80,81,867,84,85,86,116,117,118,911,121,661,128,124,136,137,138,141,128,875,223,915,128,226,134,228,136,137,138,807,128,122,235,1063,17,18,19,815,41,128,970,971,972,128,694,128,696,128,442,443,444,128,133,134,91,92,5,6,261,917,128,152,122,66,741,108,922,142,271,661,123,124,643,123,124,278,108,280,127,108,283,758,1012,646,123,124,990,116,117,118,844,108,948,847,950,115,773,300,128,128,22,23,129,857,895,108,109,110,111,112,113,114,91,92,117,686,687,141,142,87,88,123,121,123,124,128,123,108,131,123,333,123,123,941,122,116,117,118,994,787,788,140,122,684,128,108,128,128,121,121,895,132,151,128,132,136,137,138,128,123,1062,108,953,792,125,139,618,129,135,621,108,129,132,895,119,123,108,931,116,117,118,272,108,937,116,117,118,119,94,108,128,117,129,126,895,121,128,1053,136,137,138,1059,128,127,136,137,138,127,953,127,136,137,138,991,108,123,419,124,132,129,124,123,108,117,129,796,797,124,799,432,801,953,435,128,128,326,128,122,129,238,124,240,136,137,138,895,121,124,789,128,821,122,953,344,795,30,347,348,349,350,351,121,132,354,45,468,128,139,471,77,129,362,1063,129,129,366,479,276,481,41,483,124,129,173,108,175,176,177,132,492,129,124,124,124,124,498,387,123,188,189,129,108,129,953,875,195,196,197,399,116,117,118,129,404,129,122,129,135,1063,124,72,73,74,75,76,123,216,79,80,81,901,93,129,904,905,129,135,121,129,129,792,1063,122,105,123,128,108,109,122,78,112,113,114,121,116,117,128,448,899,129,122,902,1063,121,123,122,128,127,130,131,126,133,134,25,136,129,108,948,108,129,142,129,135,124,955,129,129,129,135,129,128,122,129,132,121,123,115,109,124,121,28,291,973,128,945,129,129,128,122,129,123,129,129,1063,129,618,987,123,621,129,128,132,93,994,140,93,517,123,519,124,25,123,129,124,123,121,129,978,979,129,128,982,983,109,985,335,123,650,447,129,124,123,0,129,129,0,129,941,661,129,649,120,665,38,276,668,668,271,72,468,41,165,563,50,677,713,475,708,368,990,370,371,372,373,374,375,376,377,378,379,380,381,382,383,384,385,386,703,1039,420,235,1042,356,1044,1045,618,435,372,1059,25,26,792,- 1,- 1,- 1,- 1,- 1,608,- 1,- 1,519,- 1,93,- 1,- 1,41,- 1,1068,418,- 1,- 1,530,- 1,- 1,105,- 1,- 1,108,109,- 1,539,112,113,114,- 1,116,117,- 1,- 1,640,- 1,122,- 1,- 1,- 1,- 1,647,128,- 1,130,131,- 1,133,134,655,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,779,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,792,681,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,493,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,720,146,147,723,724,- 1,726,- 1,- 1,- 1,- 1,639,- 1,- 1,734,- 1,- 1,737,- 1,- 1,- 1,- 1,650,- 1,- 1,- 1,654,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,575,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,- 1,- 1,- 1,47,48,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,824,- 1,- 1,- 1,- 1,941,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,840,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,670,- 1,- 1,- 1,- 1,784,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,816,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,927,727,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,738,37,38,39,- 1,852,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,867,- 1,- 1,59,60,- 1,- 1,63,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,782,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,115,116,- 1,118,119,- 1,- 1,122,- 1,37,38,39,40,41,- 1,130,131,- 1,- 1,- 1,48,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,- 1,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,873,85,86,876,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,894,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,911,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,954,- 1,956,957,958,959,960,961,962,963,964,965,966,967,968,969,970,971,972,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1012,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,1062,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,67,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,41,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,37,38,39,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,52,136,137,138,- 1,93,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,105,70,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,82,83,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,105,142,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,134,- 1,136,137,138,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,105,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,- 1,- 1,- 1,122,- 1,105,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,115,116,136,118,119,- 1,121,- 1,142,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,134,41,136,137,138,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,93,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,82,83,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,115,116,136,118,119,- 1,- 1,122,142,- 1,125,- 1,- 1,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,37,38,39,40,- 1,- 1,- 1,130,131,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,123,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,123,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,67,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,47,115,49,- 1,- 1,119,53,54,55,56,57,- 1,- 1,- 1,- 1,62,130,131,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,25,26,- 1,136,- 1,- 1,139,32,- 1,142,143,- 1,- 1,146,147,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,25,26,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,25,26,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,40,41,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,25,26,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,40,41,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp429[8U]="stdcall";static char _tmp42A[6U]="cdecl";static char _tmp42B[9U]="fastcall";static char _tmp42C[9U]="noreturn";static char _tmp42D[6U]="const";static char _tmp42E[8U]="aligned";static char _tmp42F[7U]="packed";static char _tmp430[7U]="shared";static char _tmp431[7U]="unused";static char _tmp432[5U]="weak";static char _tmp433[10U]="dllimport";static char _tmp434[10U]="dllexport";static char _tmp435[23U]="no_instrument_function";static char _tmp436[12U]="constructor";static char _tmp437[11U]="destructor";static char _tmp438[22U]="no_check_memory_usage";static char _tmp439[5U]="pure";static char _tmp43A[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp38B=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp38B;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp768;(_tmp768.YYINITIALSVAL).tag=64U,(_tmp768.YYINITIALSVAL).val=0;_tmp768;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp6B9=200U;short*_tmp6BA=_region_calloc(yyregion,sizeof(short),_tmp6B9);_tag_dyneither(_tmp6BA,sizeof(short),_tmp6B9);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp6B8=(unsigned int)200;union Cyc_YYSTYPE*_tmp6B7=({struct _RegionHandle*_tmp838=yyregion;_region_malloc(_tmp838,_check_times(_tmp6B8,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp767=200U;unsigned int i;for(i=0;i < _tmp767;++ i){_tmp6B7[i]=yylval;}}0;});_tmp6B7;}),sizeof(union Cyc_YYSTYPE),200U);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp6B6=(unsigned int)200;struct Cyc_Yyltype*_tmp6B5=({struct _RegionHandle*_tmp839=yyregion;_region_malloc(_tmp839,_check_times(_tmp6B6,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp766=200U;unsigned int i;for(i=0;i < _tmp766;++ i){({struct Cyc_Yyltype _tmp83A=Cyc_yynewloc();_tmp6B5[i]=_tmp83A;});}}0;});_tmp6B5;}),sizeof(struct Cyc_Yyltype),200U);
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
({struct _dyneither_ptr _tmp83C=({const char*_tmp38C="parser stack overflow";_tag_dyneither(_tmp38C,sizeof(char),22U);});int _tmp83B=yystate;Cyc_yyerror(_tmp83C,_tmp83B,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp392=(unsigned int)yystacksize;short*_tmp391=({struct _RegionHandle*_tmp83D=yyregion;_region_malloc(_tmp83D,_check_times(_tmp392,sizeof(short)));});({{unsigned int _tmp726=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp726;++ i){
i <= yyssp_offset?_tmp391[i]=*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)):(_tmp391[i]=0);}}0;});_tag_dyneither(_tmp391,sizeof(short),_tmp392);});
struct _dyneither_ptr yyvs1=({unsigned int _tmp390=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp38F=({struct _RegionHandle*_tmp83E=yyregion;_region_malloc(_tmp83E,_check_times(_tmp390,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp725=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp725;++ i){
# 219
i <= yyssp_offset?_tmp38F[i]=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)):(_tmp38F[i]=yylval);}}0;});_tag_dyneither(_tmp38F,sizeof(union Cyc_YYSTYPE),_tmp390);});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp38E=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp38D=({struct _RegionHandle*_tmp83F=yyregion;_region_malloc(_tmp83F,_check_times(_tmp38E,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp724=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp724;++ i){
i <= yyssp_offset?_tmp38D[i]=*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):({
struct Cyc_Yyltype _tmp840=Cyc_yynewloc();_tmp38D[i]=_tmp840;});}}0;});_tag_dyneither(_tmp38D,sizeof(struct Cyc_Yyltype),_tmp38E);});
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1086U,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
({int _tmp841=Cyc_yylex(yylex_buf,& yylval,& yylloc);yychar=_tmp841;});
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 375?(int)Cyc_yytranslate[_check_known_subscript_notnull(376U,yychar)]: 295;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7704) || Cyc_yycheck[_check_known_subscript_notnull(7705U,yyn)]!= yychar1)goto yydefault;
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
if(yyn == 1085){
int _tmp393=0;_npop_handler(0U);return _tmp393;}
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
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(531U,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp394=yyn;switch(_tmp394){case 1U: _LL1: _LL2: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1196 "parse.y"
yyval=yyyyvsp[0];
({struct Cyc_List_List*_tmp842=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_parse_result=_tmp842;});
# 1199
goto _LL0;}case 2U: _LL3: _LL4: {
# 1201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1202 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp395;_push_handler(& _tmp395);{int _tmp397=0;if(setjmp(_tmp395.handler))_tmp397=1;if(!_tmp397){({struct Cyc_List_List*_tmp843=Cyc_yyget_YY16(yyyyvsp[0]);x=_tmp843;});;_pop_handler();}else{void*_tmp396=(void*)_exn_thrown;void*_tmp398=_tmp396;void*_tmp399;if(((struct Cyc_Core_Failure_exn_struct*)_tmp398)->tag == Cyc_Core_Failure){_LL424: _LL425:
 x=0;goto _LL423;}else{_LL426: _tmp399=_tmp398;_LL427:(int)_rethrow(_tmp399);}_LL423:;}};}
{struct _handler_cons _tmp39A;_push_handler(& _tmp39A);{int _tmp39C=0;if(setjmp(_tmp39A.handler))_tmp39C=1;if(!_tmp39C){({struct Cyc_List_List*_tmp844=Cyc_yyget_YY16(yyyyvsp[1]);y=_tmp844;});;_pop_handler();}else{void*_tmp39B=(void*)_exn_thrown;void*_tmp39D=_tmp39B;void*_tmp39E;if(((struct Cyc_Core_Failure_exn_struct*)_tmp39D)->tag == Cyc_Core_Failure){_LL429: _LL42A:
 y=0;goto _LL428;}else{_LL42B: _tmp39E=_tmp39D;_LL42C:(int)_rethrow(_tmp39E);}_LL428:;}};}
({union Cyc_YYSTYPE _tmp845=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));yyval=_tmp845;});
# 1209
goto _LL0;}case 3U: _LL5: _LL6: {
# 1211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1212 "parse.y"
({union Cyc_YYSTYPE _tmp84A=Cyc_YY16(({struct Cyc_List_List*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_Absyn_Decl*_tmp849=({struct Cyc_Absyn_Decl*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({void*_tmp848=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->tag=10U,({struct _tuple0*_tmp847=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp39F->f1=_tmp847;}),({struct Cyc_List_List*_tmp846=Cyc_yyget_YY16(yyyyvsp[2]);_tmp39F->f2=_tmp846;});_tmp39F;});_tmp3A0->r=_tmp848;}),_tmp3A0->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A0;});_tmp3A1->hd=_tmp849;}),_tmp3A1->tl=0;_tmp3A1;}));yyval=_tmp84A;});
Cyc_Lex_leave_using();
# 1215
goto _LL0;}case 4U: _LL7: _LL8: {
# 1217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1216 "parse.y"
({union Cyc_YYSTYPE _tmp850=Cyc_YY16(({struct Cyc_List_List*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct Cyc_Absyn_Decl*_tmp84F=({struct Cyc_Absyn_Decl*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({void*_tmp84E=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));_tmp3A2->tag=10U,({struct _tuple0*_tmp84D=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3A2->f1=_tmp84D;}),({struct Cyc_List_List*_tmp84C=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A2->f2=_tmp84C;});_tmp3A2;});_tmp3A3->r=_tmp84E;}),_tmp3A3->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A3;});_tmp3A4->hd=_tmp84F;}),({struct Cyc_List_List*_tmp84B=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3A4->tl=_tmp84B;});_tmp3A4;}));yyval=_tmp850;});
goto _LL0;}case 5U: _LL9: _LLA: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1219 "parse.y"
({union Cyc_YYSTYPE _tmp856=Cyc_YY16(({struct Cyc_List_List*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));({struct Cyc_Absyn_Decl*_tmp855=({struct Cyc_Absyn_Decl*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({void*_tmp854=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));_tmp3A6->tag=9U,({struct _dyneither_ptr*_tmp853=({struct _dyneither_ptr*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({struct _dyneither_ptr _tmp852=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp3A5=_tmp852;});_tmp3A5;});_tmp3A6->f1=_tmp853;}),({struct Cyc_List_List*_tmp851=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A6->f2=_tmp851;});_tmp3A6;});_tmp3A7->r=_tmp854;}),_tmp3A7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A7;});_tmp3A8->hd=_tmp855;}),_tmp3A8->tl=0;_tmp3A8;}));yyval=_tmp856;});
Cyc_Lex_leave_namespace();
# 1222
goto _LL0;}case 6U: _LLB: _LLC: {
# 1224
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1226
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1224 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp3A9;_push_handler(& _tmp3A9);{int _tmp3AB=0;if(setjmp(_tmp3A9.handler))_tmp3AB=1;if(!_tmp3AB){({struct _dyneither_ptr _tmp857=Cyc_yyget_String_tok(yyyyvsp[0]);nspace=_tmp857;});;_pop_handler();}else{void*_tmp3AA=(void*)_exn_thrown;void*_tmp3AC=_tmp3AA;void*_tmp3AE;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3AC)->tag == Cyc_Core_Failure){_LL42E: _LL42F:
({struct _dyneither_ptr _tmp858=({const char*_tmp3AD="";_tag_dyneither(_tmp3AD,sizeof(char),1U);});nspace=_tmp858;});goto _LL42D;}else{_LL430: _tmp3AE=_tmp3AC;_LL431:(int)_rethrow(_tmp3AE);}_LL42D:;}};}
# 1229
{struct _handler_cons _tmp3AF;_push_handler(& _tmp3AF);{int _tmp3B1=0;if(setjmp(_tmp3AF.handler))_tmp3B1=1;if(!_tmp3B1){({struct Cyc_List_List*_tmp859=Cyc_yyget_YY16(yyyyvsp[2]);x=_tmp859;});;_pop_handler();}else{void*_tmp3B0=(void*)_exn_thrown;void*_tmp3B2=_tmp3B0;void*_tmp3B3;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3B2)->tag == Cyc_Core_Failure){_LL433: _LL434:
 x=0;goto _LL432;}else{_LL435: _tmp3B3=_tmp3B2;_LL436:(int)_rethrow(_tmp3B3);}_LL432:;}};}
# 1232
{struct _handler_cons _tmp3B4;_push_handler(& _tmp3B4);{int _tmp3B6=0;if(setjmp(_tmp3B4.handler))_tmp3B6=1;if(!_tmp3B6){({struct Cyc_List_List*_tmp85A=Cyc_yyget_YY16(yyyyvsp[4]);y=_tmp85A;});;_pop_handler();}else{void*_tmp3B5=(void*)_exn_thrown;void*_tmp3B7=_tmp3B5;void*_tmp3B8;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3B7)->tag == Cyc_Core_Failure){_LL438: _LL439:
 y=0;goto _LL437;}else{_LL43A: _tmp3B8=_tmp3B7;_LL43B:(int)_rethrow(_tmp3B8);}_LL437:;}};}
# 1235
({union Cyc_YYSTYPE _tmp85E=Cyc_YY16(({struct Cyc_List_List*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct Cyc_Absyn_Decl*_tmp85D=({struct Cyc_Absyn_Decl*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({void*_tmp85C=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));_tmp3BA->tag=9U,({struct _dyneither_ptr*_tmp85B=({struct _dyneither_ptr*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));*_tmp3B9=nspace;_tmp3B9;});_tmp3BA->f1=_tmp85B;}),_tmp3BA->f2=x;_tmp3BA;});_tmp3BB->r=_tmp85C;}),_tmp3BB->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BB;});_tmp3BC->hd=_tmp85D;}),_tmp3BC->tl=y;_tmp3BC;}));yyval=_tmp85E;});
# 1237
goto _LL0;}case 7U: _LLD: _LLE: {
# 1239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1238 "parse.y"
int _tmp3BD=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp3BD)
({union Cyc_YYSTYPE _tmp863=Cyc_YY16(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct Cyc_Absyn_Decl*_tmp862=({struct Cyc_Absyn_Decl*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({void*_tmp861=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->tag=11U,({struct Cyc_List_List*_tmp860=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3BE->f1=_tmp860;});_tmp3BE;});_tmp3BF->r=_tmp861;}),_tmp3BF->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BF;});_tmp3C0->hd=_tmp862;}),({struct Cyc_List_List*_tmp85F=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3C0->tl=_tmp85F;});_tmp3C0;}));yyval=_tmp863;});else{
# 1242
({union Cyc_YYSTYPE _tmp868=Cyc_YY16(({struct Cyc_List_List*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));({struct Cyc_Absyn_Decl*_tmp867=({struct Cyc_Absyn_Decl*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({void*_tmp866=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->tag=12U,({struct Cyc_List_List*_tmp865=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C1->f1=_tmp865;}),_tmp3C1->f2=0;_tmp3C1;});_tmp3C2->r=_tmp866;}),_tmp3C2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C2;});_tmp3C3->hd=_tmp867;}),({struct Cyc_List_List*_tmp864=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3C3->tl=_tmp864;});_tmp3C3;}));yyval=_tmp868;});}
# 1244
goto _LL0;}case 8U: _LLF: _LL10: {
# 1246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1245 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp3C4=0U;({unsigned int _tmp86A=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp869=({const char*_tmp3C5="expecting \"C include\"";_tag_dyneither(_tmp3C5,sizeof(char),22U);});Cyc_Warn_err(_tmp86A,_tmp869,_tag_dyneither(_tmp3C4,sizeof(void*),0U));});});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
({union Cyc_YYSTYPE _tmp86F=Cyc_YY16(({struct Cyc_List_List*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));({struct Cyc_Absyn_Decl*_tmp86E=({struct Cyc_Absyn_Decl*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({void*_tmp86D=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->tag=12U,({struct Cyc_List_List*_tmp86C=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C6->f1=_tmp86C;}),_tmp3C6->f2=exs;_tmp3C6;});_tmp3C7->r=_tmp86D;}),_tmp3C7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C7;});_tmp3C8->hd=_tmp86E;}),({struct Cyc_List_List*_tmp86B=Cyc_yyget_YY16(yyyyvsp[5]);_tmp3C8->tl=_tmp86B;});_tmp3C8;}));yyval=_tmp86F;});
# 1250
goto _LL0;};}case 9U: _LL11: _LL12: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1251 "parse.y"
({union Cyc_YYSTYPE _tmp872=Cyc_YY16(({struct Cyc_List_List*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct Cyc_Absyn_Decl*_tmp871=({struct Cyc_Absyn_Decl*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C9->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C9;});_tmp3CA->hd=_tmp871;}),({struct Cyc_List_List*_tmp870=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3CA->tl=_tmp870;});_tmp3CA;}));yyval=_tmp872;});
goto _LL0;}case 10U: _LL13: _LL14: {
# 1254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1253 "parse.y"
({union Cyc_YYSTYPE _tmp875=Cyc_YY16(({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));({struct Cyc_Absyn_Decl*_tmp874=({struct Cyc_Absyn_Decl*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3CB->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3CB;});_tmp3CC->hd=_tmp874;}),({struct Cyc_List_List*_tmp873=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3CC->tl=_tmp873;});_tmp3CC;}));yyval=_tmp875;});
goto _LL0;}case 11U: _LL15: _LL16:
# 1256
({union Cyc_YYSTYPE _tmp876=Cyc_YY16(0);yyval=_tmp876;});
goto _LL0;case 12U: _LL17: _LL18: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1259 "parse.y"
struct _dyneither_ptr _tmp3CD=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({struct _dyneither_ptr _tmp877=(struct _dyneither_ptr)_tmp3CD;Cyc_strcmp(_tmp877,({const char*_tmp3CE="C";_tag_dyneither(_tmp3CE,sizeof(char),2U);}));})== 0)
({union Cyc_YYSTYPE _tmp878=Cyc_YY31(0);yyval=_tmp878;});else{
if(({struct _dyneither_ptr _tmp879=(struct _dyneither_ptr)_tmp3CD;Cyc_strcmp(_tmp879,({const char*_tmp3CF="C include";_tag_dyneither(_tmp3CF,sizeof(char),10U);}));})== 0)
({union Cyc_YYSTYPE _tmp87A=Cyc_YY31(1);yyval=_tmp87A;});else{
# 1266
({void*_tmp3D0=0U;({unsigned int _tmp87C=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp87B=({const char*_tmp3D1="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3D1,sizeof(char),29U);});Cyc_Warn_err(_tmp87C,_tmp87B,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp87D=Cyc_YY31(1);yyval=_tmp87D;});}}
# 1270
goto _LL0;}case 13U: _LL19: _LL1A: {
# 1272
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1274
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1273 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL0;}case 14U: _LL1B: _LL1C: {
# 1276
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1278
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1277 "parse.y"
yyval=yyyyvsp[2];
goto _LL0;}case 15U: _LL1D: _LL1E: {
# 1280
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1282
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1278 "parse.y"
({union Cyc_YYSTYPE _tmp87E=Cyc_YY52(0);yyval=_tmp87E;});
goto _LL0;}case 16U: _LL1F: _LL20: {
# 1281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1282 "parse.y"
({union Cyc_YYSTYPE _tmp881=Cyc_YY52(({struct Cyc_List_List*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct _tuple27*_tmp880=({struct _tuple27*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp87F=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D2->f2=_tmp87F;}),_tmp3D2->f3=0;_tmp3D2;});_tmp3D3->hd=_tmp880;}),_tmp3D3->tl=0;_tmp3D3;}));yyval=_tmp881;});
goto _LL0;}case 17U: _LL21: _LL22: {
# 1285
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1287
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1283 "parse.y"
({union Cyc_YYSTYPE _tmp884=Cyc_YY52(({struct Cyc_List_List*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct _tuple27*_tmp883=({struct _tuple27*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp882=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D4->f2=_tmp882;}),_tmp3D4->f3=0;_tmp3D4;});_tmp3D5->hd=_tmp883;}),_tmp3D5->tl=0;_tmp3D5;}));yyval=_tmp884;});
goto _LL0;}case 18U: _LL23: _LL24: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1285 "parse.y"
({union Cyc_YYSTYPE _tmp888=Cyc_YY52(({struct Cyc_List_List*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct _tuple27*_tmp887=({struct _tuple27*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp886=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D6->f2=_tmp886;}),_tmp3D6->f3=0;_tmp3D6;});_tmp3D7->hd=_tmp887;}),({struct Cyc_List_List*_tmp885=Cyc_yyget_YY52(yyyyvsp[2]);_tmp3D7->tl=_tmp885;});_tmp3D7;}));yyval=_tmp888;});
goto _LL0;}case 19U: _LL25: _LL26: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1289 "parse.y"
({union Cyc_YYSTYPE _tmp88C=Cyc_YY16(({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct Cyc_Absyn_Decl*_tmp88B=({void*_tmp88A=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp889=Cyc_yyget_YY15(yyyyvsp[0]);_tmp3D8->f1=_tmp889;});_tmp3D8;});Cyc_Absyn_new_decl(_tmp88A,(unsigned int)(yyyylsp[0]).first_line);});_tmp3D9->hd=_tmp88B;}),_tmp3D9->tl=0;_tmp3D9;}));yyval=_tmp88C;});
goto _LL0;}case 20U: _LL27: _LL28: {
# 1292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1290 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 21U: _LL29: _LL2A: {
# 1293
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1291 "parse.y"
({union Cyc_YYSTYPE _tmp88D=Cyc_YY16(0);yyval=_tmp88D;});
goto _LL0;}case 24U: _LL2B: _LL2C: {
# 1294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1300 "parse.y"
({union Cyc_YYSTYPE _tmp891=Cyc_YY15(({struct _RegionHandle*_tmp890=yyr;struct Cyc_Parse_Declarator _tmp88F=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp88E=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp890,0,_tmp88F,0,_tmp88E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp891;});
goto _LL0;}case 25U: _LL2D: _LL2E: {
# 1303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1302 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DA=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp895=Cyc_YY15(({struct _RegionHandle*_tmp894=yyr;struct Cyc_Parse_Declarator _tmp893=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp892=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp894,& _tmp3DA,_tmp893,0,_tmp892,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp895;});
goto _LL0;}case 26U: _LL2F: _LL30: {
# 1306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1315 "parse.y"
({union Cyc_YYSTYPE _tmp89A=Cyc_YY15(({struct _RegionHandle*_tmp899=yyr;struct Cyc_Parse_Declarator _tmp898=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp897=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp896=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp899,0,_tmp898,_tmp897,_tmp896,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp89A;});
goto _LL0;}case 27U: _LL31: _LL32: {
# 1318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1317 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DB=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp89F=Cyc_YY15(({struct _RegionHandle*_tmp89E=yyr;struct Cyc_Parse_Declarator _tmp89D=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp89C=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp89B=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp89E,& _tmp3DB,_tmp89D,_tmp89C,_tmp89B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp89F;});
goto _LL0;}case 28U: _LL33: _LL34: {
# 1321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1325 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DC=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp8A3=Cyc_YY15(({struct _RegionHandle*_tmp8A2=yyr;struct Cyc_Parse_Declarator _tmp8A1=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp8A0=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp8A2,& _tmp3DC,_tmp8A1,0,_tmp8A0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp8A3;});
goto _LL0;}case 29U: _LL35: _LL36: {
# 1329
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1328 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DD=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp8A8=Cyc_YY15(({struct _RegionHandle*_tmp8A7=yyr;struct Cyc_Parse_Declarator _tmp8A6=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp8A5=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp8A4=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp8A7,& _tmp3DD,_tmp8A6,_tmp8A5,_tmp8A4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp8A8;});
goto _LL0;}case 30U: _LL37: _LL38: {
# 1332
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1333 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL0;}case 31U: _LL39: _LL3A: {
# 1336
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1336 "parse.y"
Cyc_Lex_leave_using();
goto _LL0;}case 32U: _LL3B: _LL3C: {
# 1339
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1341
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1339 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));({struct _dyneither_ptr _tmp8A9=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp3DE=_tmp8A9;});_tmp3DE;}));yyval=yyyyvsp[1];
goto _LL0;}case 33U: _LL3D: _LL3E: {
# 1342
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1342 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL0;}case 34U: _LL3F: _LL40: {
# 1345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
int _tmp3DF=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp8AC=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp8AB=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp8AA=(unsigned int)_tmp3DF;Cyc_Parse_make_declarations(_tmp8AB,0,_tmp8AA,(unsigned int)_tmp3DF);}));yyval=_tmp8AC;});
goto _LL0;}case 35U: _LL41: _LL42: {
# 1352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1351 "parse.y"
int _tmp3E0=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp8B0=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp8AF=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp8AE=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp8AD=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp8AF,_tmp8AE,_tmp8AD,(unsigned int)_tmp3E0);}));yyval=_tmp8B0;});
goto _LL0;}case 36U: _LL43: _LL44: {
# 1355
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1355 "parse.y"
({union Cyc_YYSTYPE _tmp8B4=Cyc_YY16(({struct Cyc_List_List*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));({struct Cyc_Absyn_Decl*_tmp8B3=({struct Cyc_Absyn_Pat*_tmp8B2=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp8B1=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp8B2,_tmp8B1,(unsigned int)(yyyylsp[0]).first_line);});_tmp3E1->hd=_tmp8B3;}),_tmp3E1->tl=0;_tmp3E1;}));yyval=_tmp8B4;});
goto _LL0;}case 37U: _LL45: _LL46: {
# 1358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1357 "parse.y"
struct Cyc_List_List*_tmp3E2=0;
{struct Cyc_List_List*_tmp3E3=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp3E3 != 0;_tmp3E3=_tmp3E3->tl){
struct _dyneither_ptr*_tmp3E4=(struct _dyneither_ptr*)_tmp3E3->hd;
struct _tuple0*qv=({struct _tuple0*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));({union Cyc_Absyn_Nmspace _tmp8B5=Cyc_Absyn_Rel_n(0);_tmp3E7->f1=_tmp8B5;}),_tmp3E7->f2=_tmp3E4;_tmp3E7;});
struct Cyc_Absyn_Vardecl*_tmp3E5=({struct _tuple0*_tmp8B6=qv;Cyc_Absyn_new_vardecl(0U,_tmp8B6,Cyc_Absyn_wildtyp(0),0);});
({struct Cyc_List_List*_tmp8B7=({struct Cyc_List_List*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->hd=_tmp3E5,_tmp3E6->tl=_tmp3E2;_tmp3E6;});_tmp3E2=_tmp8B7;});}}
# 1364
({struct Cyc_List_List*_tmp8B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3E2);_tmp3E2=_tmp8B8;});
({union Cyc_YYSTYPE _tmp8BA=Cyc_YY16(({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));({struct Cyc_Absyn_Decl*_tmp8B9=Cyc_Absyn_letv_decl(_tmp3E2,(unsigned int)(yyyylsp[0]).first_line);_tmp3E8->hd=_tmp8B9;}),_tmp3E8->tl=0;_tmp3E8;}));yyval=_tmp8BA;});
# 1367
goto _LL0;}case 38U: _LL47: _LL48: {
# 1369
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1370 "parse.y"
struct _dyneither_ptr _tmp3E9=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1372
if(({struct _dyneither_ptr _tmp8BB=(struct _dyneither_ptr)_tmp3E9;Cyc_zstrcmp(_tmp8BB,({const char*_tmp3EA="`H";_tag_dyneither(_tmp3EA,sizeof(char),3U);}));})== 0)
({void*_tmp3EB=0U;({unsigned int _tmp8BD=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8BC=({const char*_tmp3EC="bad occurrence of heap region";_tag_dyneither(_tmp3EC,sizeof(char),30U);});Cyc_Warn_err(_tmp8BD,_tmp8BC,_tag_dyneither(_tmp3EB,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp8BE=(struct _dyneither_ptr)_tmp3E9;Cyc_zstrcmp(_tmp8BE,({const char*_tmp3ED="`U";_tag_dyneither(_tmp3ED,sizeof(char),3U);}));})== 0)
({void*_tmp3EE=0U;({unsigned int _tmp8C0=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8BF=({const char*_tmp3EF="bad occurrence of unique region";_tag_dyneither(_tmp3EF,sizeof(char),32U);});Cyc_Warn_err(_tmp8C0,_tmp8BF,_tag_dyneither(_tmp3EE,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));({struct _dyneither_ptr*_tmp8C2=({struct _dyneither_ptr*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));*_tmp3F5=_tmp3E9;_tmp3F5;});_tmp3F6->name=_tmp8C2;}),_tmp3F6->identity=- 1,({void*_tmp8C1=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3F6->kind=_tmp8C1;});_tmp3F6;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->tag=2U,_tmp3F4->f1=tv;_tmp3F4;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8C6=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp8C5=({struct _tuple0*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8C4=({struct _dyneither_ptr*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));({struct _dyneither_ptr _tmp8C3=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp3F1=_tmp8C3;});_tmp3F1;});_tmp3F2->f2=_tmp8C4;});_tmp3F2;});Cyc_Absyn_new_vardecl(_tmp8C6,_tmp8C5,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->tag=15U,_tmp3F3->f1=t;_tmp3F3;}),0);});
({union Cyc_YYSTYPE _tmp8C8=Cyc_YY16(({struct Cyc_List_List*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));({struct Cyc_Absyn_Decl*_tmp8C7=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3F0->hd=_tmp8C7;}),_tmp3F0->tl=0;_tmp3F0;}));yyval=_tmp8C8;});
# 1381
goto _LL0;};}case 39U: _LL49: _LL4A: {
# 1383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1383 "parse.y"
struct _dyneither_ptr _tmp3F7=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({struct _dyneither_ptr _tmp8C9=(struct _dyneither_ptr)_tmp3F7;Cyc_zstrcmp(_tmp8C9,({const char*_tmp3F8="H";_tag_dyneither(_tmp3F8,sizeof(char),2U);}));})== 0)
({void*_tmp3F9=0U;({unsigned int _tmp8CB=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp8CA=({const char*_tmp3FA="bad occurrence of heap region `H";_tag_dyneither(_tmp3FA,sizeof(char),33U);});Cyc_Warn_err(_tmp8CB,_tmp8CA,_tag_dyneither(_tmp3F9,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp8CC=(struct _dyneither_ptr)_tmp3F7;Cyc_zstrcmp(_tmp8CC,({const char*_tmp3FB="U";_tag_dyneither(_tmp3FB,sizeof(char),2U);}));})== 0)
({void*_tmp3FC=0U;({unsigned int _tmp8CE=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp8CD=({const char*_tmp3FD="bad occurrence of unique region `U";_tag_dyneither(_tmp3FD,sizeof(char),35U);});Cyc_Warn_err(_tmp8CE,_tmp8CD,_tag_dyneither(_tmp3FC,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp407=_cycalloc(sizeof(*_tmp407));({struct _dyneither_ptr*_tmp8D2=({struct _dyneither_ptr*_tmp406=_cycalloc(sizeof(*_tmp406));({struct _dyneither_ptr _tmp8D1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp405=({struct Cyc_String_pa_PrintArg_struct _tmp727;_tmp727.tag=0U,_tmp727.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F7);_tmp727;});void*_tmp403[1U];_tmp403[0]=& _tmp405;({struct _dyneither_ptr _tmp8D0=({const char*_tmp404="`%s";_tag_dyneither(_tmp404,sizeof(char),4U);});Cyc_aprintf(_tmp8D0,_tag_dyneither(_tmp403,sizeof(void*),1U));});});*_tmp406=_tmp8D1;});_tmp406;});_tmp407->name=_tmp8D2;}),_tmp407->identity=- 1,({
void*_tmp8CF=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp407->kind=_tmp8CF;});_tmp407;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->tag=2U,_tmp402->f1=tv;_tmp402;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8D5=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp8D4=({struct _tuple0*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8D3=({struct _dyneither_ptr*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));*_tmp3FF=_tmp3F7;_tmp3FF;});_tmp400->f2=_tmp8D3;});_tmp400;});Cyc_Absyn_new_vardecl(_tmp8D5,_tmp8D4,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->tag=15U,_tmp401->f1=t;_tmp401;}),0);});
({union Cyc_YYSTYPE _tmp8D7=Cyc_YY16(({struct Cyc_List_List*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));({struct Cyc_Absyn_Decl*_tmp8D6=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3FE->hd=_tmp8D6;}),_tmp3FE->tl=0;_tmp3FE;}));yyval=_tmp8D7;});
# 1394
goto _LL0;};}case 40U: _LL4B: _LL4C: {
# 1396
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1396 "parse.y"
struct _dyneither_ptr _tmp408=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp409=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp40A=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({struct _dyneither_ptr _tmp8D8=(struct _dyneither_ptr)_tmp409;Cyc_strcmp(_tmp8D8,({const char*_tmp40B="open";_tag_dyneither(_tmp40B,sizeof(char),5U);}));})!= 0)({void*_tmp40C=0U;({unsigned int _tmp8DA=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp8D9=({const char*_tmp40D="expecting `open'";_tag_dyneither(_tmp40D,sizeof(char),17U);});Cyc_Warn_err(_tmp8DA,_tmp8D9,_tag_dyneither(_tmp40C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp417=_cycalloc(sizeof(*_tmp417));({struct _dyneither_ptr*_tmp8DE=({struct _dyneither_ptr*_tmp416=_cycalloc(sizeof(*_tmp416));({struct _dyneither_ptr _tmp8DD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp415=({struct Cyc_String_pa_PrintArg_struct _tmp728;_tmp728.tag=0U,_tmp728.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp408);_tmp728;});void*_tmp413[1U];_tmp413[0]=& _tmp415;({struct _dyneither_ptr _tmp8DC=({const char*_tmp414="`%s";_tag_dyneither(_tmp414,sizeof(char),4U);});Cyc_aprintf(_tmp8DC,_tag_dyneither(_tmp413,sizeof(void*),1U));});});*_tmp416=_tmp8DD;});_tmp416;});_tmp417->name=_tmp8DE;}),_tmp417->identity=- 1,({
void*_tmp8DB=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp417->kind=_tmp8DB;});_tmp417;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->tag=2U,_tmp412->f1=tv;_tmp412;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8E1=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp8E0=({struct _tuple0*_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8DF=({struct _dyneither_ptr*_tmp40F=_cycalloc(sizeof(*_tmp40F));*_tmp40F=_tmp408;_tmp40F;});_tmp410->f2=_tmp8DF;});_tmp410;});Cyc_Absyn_new_vardecl(_tmp8E1,_tmp8E0,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->tag=15U,_tmp411->f1=t;_tmp411;}),0);});
({union Cyc_YYSTYPE _tmp8E3=Cyc_YY16(({struct Cyc_List_List*_tmp40E=_cycalloc(sizeof(*_tmp40E));({struct Cyc_Absyn_Decl*_tmp8E2=Cyc_Absyn_region_decl(tv,vd,_tmp40A,(unsigned int)(yyyylsp[0]).first_line);_tmp40E->hd=_tmp8E2;}),_tmp40E->tl=0;_tmp40E;}));yyval=_tmp8E3;});
# 1406
goto _LL0;};}case 41U: _LL4D: _LL4E: {
# 1408
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1410
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1410 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1413
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1415
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1412 "parse.y"
({union Cyc_YYSTYPE _tmp8E5=Cyc_YY16(({struct Cyc_List_List*_tmp8E4=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8E4,Cyc_yyget_YY16(yyyyvsp[1]));}));yyval=_tmp8E5;});
goto _LL0;}case 43U: _LL51: _LL52: {
# 1415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp8E9=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp729;({enum Cyc_Parse_Storage_class*_tmp8E8=Cyc_yyget_YY20(yyyyvsp[0]);_tmp729.sc=_tmp8E8;}),({struct Cyc_Absyn_Tqual _tmp8E7=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp729.tq=_tmp8E7;}),({
struct Cyc_Parse_Type_specifier _tmp8E6=Cyc_Parse_empty_spec(0U);_tmp729.type_specs=_tmp8E6;}),_tmp729.is_inline=0,_tmp729.attributes=0;_tmp729;}));
# 1418
yyval=_tmp8E9;});
# 1420
goto _LL0;}case 44U: _LL53: _LL54: {
# 1422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1421 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp418=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp418.sc != 0)
({void*_tmp419=0U;({unsigned int _tmp8EB=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8EA=({const char*_tmp41A="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp41A,sizeof(char),73U);});Cyc_Warn_warn(_tmp8EB,_tmp8EA,_tag_dyneither(_tmp419,sizeof(void*),0U));});});
# 1425
({union Cyc_YYSTYPE _tmp8ED=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72A;({enum Cyc_Parse_Storage_class*_tmp8EC=Cyc_yyget_YY20(yyyyvsp[0]);_tmp72A.sc=_tmp8EC;}),_tmp72A.tq=_tmp418.tq,_tmp72A.type_specs=_tmp418.type_specs,_tmp72A.is_inline=_tmp418.is_inline,_tmp72A.attributes=_tmp418.attributes;_tmp72A;}));yyval=_tmp8ED;});
# 1429
goto _LL0;}case 45U: _LL55: _LL56: {
# 1431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1430 "parse.y"
({void*_tmp41B=0U;({unsigned int _tmp8EF=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8EE=({const char*_tmp41C="__extension__ keyword ignored in declaration";_tag_dyneither(_tmp41C,sizeof(char),45U);});Cyc_Warn_warn(_tmp8EF,_tmp8EE,_tag_dyneither(_tmp41B,sizeof(void*),0U));});});
yyval=yyyyvsp[1];
# 1433
goto _LL0;}case 46U: _LL57: _LL58: {
# 1435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1434 "parse.y"
({union Cyc_YYSTYPE _tmp8F2=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72B;_tmp72B.sc=0,({struct Cyc_Absyn_Tqual _tmp8F1=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp72B.tq=_tmp8F1;}),({
struct Cyc_Parse_Type_specifier _tmp8F0=Cyc_yyget_YY21(yyyyvsp[0]);_tmp72B.type_specs=_tmp8F0;}),_tmp72B.is_inline=0,_tmp72B.attributes=0;_tmp72B;}));
# 1434
yyval=_tmp8F2;});
# 1436
goto _LL0;}case 47U: _LL59: _LL5A: {
# 1438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1437 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41D=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8F6=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72C;_tmp72C.sc=_tmp41D.sc,_tmp72C.tq=_tmp41D.tq,({
struct Cyc_Parse_Type_specifier _tmp8F5=({unsigned int _tmp8F4=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp8F3=_tmp41D.type_specs;Cyc_Parse_combine_specifiers(_tmp8F4,_tmp8F3,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp72C.type_specs=_tmp8F5;}),_tmp72C.is_inline=_tmp41D.is_inline,_tmp72C.attributes=_tmp41D.attributes;_tmp72C;}));
# 1438
yyval=_tmp8F6;});
# 1443
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1444 "parse.y"
({union Cyc_YYSTYPE _tmp8F9=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72D;_tmp72D.sc=0,({struct Cyc_Absyn_Tqual _tmp8F8=Cyc_yyget_YY23(yyyyvsp[0]);_tmp72D.tq=_tmp8F8;}),({struct Cyc_Parse_Type_specifier _tmp8F7=Cyc_Parse_empty_spec(0U);_tmp72D.type_specs=_tmp8F7;}),_tmp72D.is_inline=0,_tmp72D.attributes=0;_tmp72D;}));yyval=_tmp8F9;});
goto _LL0;}case 49U: _LL5D: _LL5E: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1446 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41E=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8FC=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72E;_tmp72E.sc=_tmp41E.sc,({struct Cyc_Absyn_Tqual _tmp8FB=({struct Cyc_Absyn_Tqual _tmp8FA=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp8FA,_tmp41E.tq);});_tmp72E.tq=_tmp8FB;}),_tmp72E.type_specs=_tmp41E.type_specs,_tmp72E.is_inline=_tmp41E.is_inline,_tmp72E.attributes=_tmp41E.attributes;_tmp72E;}));yyval=_tmp8FC;});
# 1451
goto _LL0;}case 50U: _LL5F: _LL60: {
# 1453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1452 "parse.y"
({union Cyc_YYSTYPE _tmp8FF=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp72F;_tmp72F.sc=0,({struct Cyc_Absyn_Tqual _tmp8FE=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp72F.tq=_tmp8FE;}),({
struct Cyc_Parse_Type_specifier _tmp8FD=Cyc_Parse_empty_spec(0U);_tmp72F.type_specs=_tmp8FD;}),_tmp72F.is_inline=1,_tmp72F.attributes=0;_tmp72F;}));
# 1452
yyval=_tmp8FF;});
# 1454
goto _LL0;}case 51U: _LL61: _LL62: {
# 1456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1455 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41F=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp900=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp730;_tmp730.sc=_tmp41F.sc,_tmp730.tq=_tmp41F.tq,_tmp730.type_specs=_tmp41F.type_specs,_tmp730.is_inline=1,_tmp730.attributes=_tmp41F.attributes;_tmp730;}));yyval=_tmp900;});
# 1459
goto _LL0;}case 52U: _LL63: _LL64: {
# 1461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1460 "parse.y"
({union Cyc_YYSTYPE _tmp904=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp731;_tmp731.sc=0,({struct Cyc_Absyn_Tqual _tmp903=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp731.tq=_tmp903;}),({
struct Cyc_Parse_Type_specifier _tmp902=Cyc_Parse_empty_spec(0U);_tmp731.type_specs=_tmp902;}),_tmp731.is_inline=0,({struct Cyc_List_List*_tmp901=Cyc_yyget_YY45(yyyyvsp[0]);_tmp731.attributes=_tmp901;});_tmp731;}));
# 1460
yyval=_tmp904;});
# 1462
goto _LL0;}case 53U: _LL65: _LL66: {
# 1464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1463 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp420=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp907=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp732;_tmp732.sc=_tmp420.sc,_tmp732.tq=_tmp420.tq,_tmp732.type_specs=_tmp420.type_specs,_tmp732.is_inline=_tmp420.is_inline,({
# 1466
struct Cyc_List_List*_tmp906=({struct Cyc_List_List*_tmp905=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp905,_tmp420.attributes);});_tmp732.attributes=_tmp906;});_tmp732;}));
# 1464
yyval=_tmp907;});
# 1467
goto _LL0;}case 54U: _LL67: _LL68: {
# 1469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1470 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
({union Cyc_YYSTYPE _tmp908=Cyc_YY20(& auto_sc);yyval=_tmp908;});
goto _LL0;}case 55U: _LL69: _LL6A: {
# 1474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1472 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
({union Cyc_YYSTYPE _tmp909=Cyc_YY20(& register_sc);yyval=_tmp909;});
goto _LL0;}case 56U: _LL6B: _LL6C: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1474 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
({union Cyc_YYSTYPE _tmp90A=Cyc_YY20(& static_sc);yyval=_tmp90A;});
goto _LL0;}case 57U: _LL6D: _LL6E: {
# 1478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1476 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
({union Cyc_YYSTYPE _tmp90B=Cyc_YY20(& extern_sc);yyval=_tmp90B;});
goto _LL0;}case 58U: _LL6F: _LL70: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1479 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _dyneither_ptr _tmp90C=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmp90C,({const char*_tmp421="C";_tag_dyneither(_tmp421,sizeof(char),2U);}));})!= 0)
({void*_tmp422=0U;({unsigned int _tmp90E=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp90D=({const char*_tmp423="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp423,sizeof(char),37U);});Cyc_Warn_err(_tmp90E,_tmp90D,_tag_dyneither(_tmp422,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp90F=Cyc_YY20(& externC_sc);yyval=_tmp90F;});
# 1484
goto _LL0;}case 59U: _LL71: _LL72: {
# 1486
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1488
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1484 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
({union Cyc_YYSTYPE _tmp910=Cyc_YY20(& typedef_sc);yyval=_tmp910;});
goto _LL0;}case 60U: _LL73: _LL74: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1487 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
({union Cyc_YYSTYPE _tmp911=Cyc_YY20(& abstract_sc);yyval=_tmp911;});
goto _LL0;}case 61U: _LL75: _LL76:
# 1491
({union Cyc_YYSTYPE _tmp912=Cyc_YY45(0);yyval=_tmp912;});
goto _LL0;case 62U: _LL77: _LL78: {
# 1494
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1496
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1494 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 63U: _LL79: _LL7A: {
# 1497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1499 "parse.y"
yyval=yyyyvsp[3];
goto _LL0;}case 64U: _LL7B: _LL7C: {
# 1502
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1504
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1503 "parse.y"
({union Cyc_YYSTYPE _tmp914=Cyc_YY45(({struct Cyc_List_List*_tmp424=_cycalloc(sizeof(*_tmp424));({void*_tmp913=Cyc_yyget_YY46(yyyyvsp[0]);_tmp424->hd=_tmp913;}),_tmp424->tl=0;_tmp424;}));yyval=_tmp914;});
goto _LL0;}case 65U: _LL7D: _LL7E: {
# 1506
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1508
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1504 "parse.y"
({union Cyc_YYSTYPE _tmp917=Cyc_YY45(({struct Cyc_List_List*_tmp425=_cycalloc(sizeof(*_tmp425));({void*_tmp916=Cyc_yyget_YY46(yyyyvsp[0]);_tmp425->hd=_tmp916;}),({struct Cyc_List_List*_tmp915=Cyc_yyget_YY45(yyyyvsp[2]);_tmp425->tl=_tmp915;});_tmp425;}));yyval=_tmp917;});
goto _LL0;}case 66U: _LL7F: _LL80: {
# 1507
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1509
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1509 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp429,_tmp429,_tmp429 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp42A,_tmp42A,_tmp42A + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp42B,_tmp42B,_tmp42B + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp42C,_tmp42C,_tmp42C + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp42D,_tmp42D,_tmp42D + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp42E,_tmp42E,_tmp42E + 8U},(void*)& att_aligned},{{_tmp42F,_tmp42F,_tmp42F + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp430,_tmp430,_tmp430 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp431,_tmp431,_tmp431 + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp432,_tmp432,_tmp432 + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp433,_tmp433,_tmp433 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp434,_tmp434,_tmp434 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp435,_tmp435,_tmp435 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp436,_tmp436,_tmp436 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp437,_tmp437,_tmp437 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp438,_tmp438,_tmp438 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp439,_tmp439,_tmp439 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp43A,_tmp43A,_tmp43A + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1530
struct _dyneither_ptr _tmp426=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1532
if((((_get_dyneither_size(_tmp426,sizeof(char))> 4  && ((const char*)_tmp426.curr)[0]== '_') && ((const char*)_tmp426.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp426,sizeof(char),(int)(_get_dyneither_size(_tmp426,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp426,sizeof(char),(int)(_get_dyneither_size(_tmp426,sizeof(char))- 3)))== '_')
# 1534
({struct _dyneither_ptr _tmp918=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp426,2,_get_dyneither_size(_tmp426,sizeof(char))- 5);_tmp426=_tmp918;});{
int i=0;
for(0;i < 18U;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp426,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18U,i)]).f1)== 0){
({union Cyc_YYSTYPE _tmp919=Cyc_YY46((att_map[i]).f2);yyval=_tmp919;});
break;}}
# 1541
if(i == 18U){
({void*_tmp427=0U;({unsigned int _tmp91B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp91A=({const char*_tmp428="unrecognized attribute";_tag_dyneither(_tmp428,sizeof(char),23U);});Cyc_Warn_err(_tmp91B,_tmp91A,_tag_dyneither(_tmp427,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp91C=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);yyval=_tmp91C;});}
# 1546
goto _LL0;};}case 67U: _LL81: _LL82: {
# 1548
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1546 "parse.y"
({union Cyc_YYSTYPE _tmp91D=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);yyval=_tmp91D;});
goto _LL0;}case 68U: _LL83: _LL84: {
# 1549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1548 "parse.y"
struct _dyneither_ptr _tmp43B=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp43C=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({struct _dyneither_ptr _tmp91F=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp91F,({const char*_tmp43D="aligned";_tag_dyneither(_tmp43D,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp91E=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp91E,({const char*_tmp43E="__aligned__";_tag_dyneither(_tmp43E,sizeof(char),12U);}));})== 0)
({void*_tmp920=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=6U,_tmp43F->f1=_tmp43C;_tmp43F;});a=_tmp920;});else{
if(({struct _dyneither_ptr _tmp922=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp922,({const char*_tmp440="section";_tag_dyneither(_tmp440,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp921=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp921,({const char*_tmp441="__section__";_tag_dyneither(_tmp441,sizeof(char),12U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp43C);
({void*_tmp923=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=8U,_tmp442->f1=str;_tmp442;});a=_tmp923;});}else{
if(({struct _dyneither_ptr _tmp924=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp924,({const char*_tmp443="__mode__";_tag_dyneither(_tmp443,sizeof(char),9U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp43C);
({void*_tmp925=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->tag=24U,_tmp444->f1=str;_tmp444;});a=_tmp925;});}else{
if(({struct _dyneither_ptr _tmp926=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp926,({const char*_tmp445="alias";_tag_dyneither(_tmp445,sizeof(char),6U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp43C);
({void*_tmp927=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446->tag=25U,_tmp446->f1=str;_tmp446;});a=_tmp927;});}else{
# 1564
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp43C);
if(({struct _dyneither_ptr _tmp929=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp929,({const char*_tmp447="regparm";_tag_dyneither(_tmp447,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp928=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp928,({const char*_tmp448="__regparm__";_tag_dyneither(_tmp448,sizeof(char),12U);}));})== 0){
if(n < 0  || n > 3)
({void*_tmp449=0U;({unsigned int _tmp92B=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp92A=({const char*_tmp44A="regparm requires value between 0 and 3";_tag_dyneither(_tmp44A,sizeof(char),39U);});Cyc_Warn_err(_tmp92B,_tmp92A,_tag_dyneither(_tmp449,sizeof(void*),0U));});});
({void*_tmp92C=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=0U,_tmp44B->f1=n;_tmp44B;});a=_tmp92C;});}else{
if(({struct _dyneither_ptr _tmp92E=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp92E,({const char*_tmp44C="initializes";_tag_dyneither(_tmp44C,sizeof(char),12U);}));})== 0  || ({struct _dyneither_ptr _tmp92D=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp92D,({const char*_tmp44D="__initializes__";_tag_dyneither(_tmp44D,sizeof(char),16U);}));})== 0)
({void*_tmp92F=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->tag=20U,_tmp44E->f1=n;_tmp44E;});a=_tmp92F;});else{
if(({struct _dyneither_ptr _tmp931=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp931,({const char*_tmp44F="noliveunique";_tag_dyneither(_tmp44F,sizeof(char),13U);}));})== 0  || ({struct _dyneither_ptr _tmp930=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp930,({const char*_tmp450="__noliveunique__";_tag_dyneither(_tmp450,sizeof(char),17U);}));})== 0)
({void*_tmp932=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=21U,_tmp451->f1=n;_tmp451;});a=_tmp932;});else{
if(({struct _dyneither_ptr _tmp934=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp934,({const char*_tmp452="noconsume";_tag_dyneither(_tmp452,sizeof(char),10U);}));})== 0  || ({struct _dyneither_ptr _tmp933=(struct _dyneither_ptr)_tmp43B;Cyc_zstrcmp(_tmp933,({const char*_tmp453="__noconsume__";_tag_dyneither(_tmp453,sizeof(char),14U);}));})== 0)
({void*_tmp935=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->tag=22U,_tmp454->f1=n;_tmp454;});a=_tmp935;});else{
# 1576
({void*_tmp455=0U;({unsigned int _tmp937=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp936=({const char*_tmp456="unrecognized attribute";_tag_dyneither(_tmp456,sizeof(char),23U);});Cyc_Warn_err(_tmp937,_tmp936,_tag_dyneither(_tmp455,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1580
({union Cyc_YYSTYPE _tmp938=Cyc_YY46(a);yyval=_tmp938;});
# 1582
goto _LL0;}case 69U: _LL85: _LL86: {
# 1584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1583 "parse.y"
struct _dyneither_ptr _tmp457=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp458=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp459=({unsigned int _tmp939=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmp939,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp45A=({unsigned int _tmp93A=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmp93A,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _dyneither_ptr _tmp93C=(struct _dyneither_ptr)_tmp457;Cyc_zstrcmp(_tmp93C,({const char*_tmp45B="format";_tag_dyneither(_tmp45B,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp93B=(struct _dyneither_ptr)_tmp457;Cyc_zstrcmp(_tmp93B,({const char*_tmp45C="__format__";_tag_dyneither(_tmp45C,sizeof(char),11U);}));})== 0){
if(({struct _dyneither_ptr _tmp93E=(struct _dyneither_ptr)_tmp458;Cyc_zstrcmp(_tmp93E,({const char*_tmp45D="printf";_tag_dyneither(_tmp45D,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp93D=(struct _dyneither_ptr)_tmp458;Cyc_zstrcmp(_tmp93D,({const char*_tmp45E="__printf__";_tag_dyneither(_tmp45E,sizeof(char),11U);}));})== 0)
({void*_tmp93F=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->tag=19U,_tmp45F->f1=Cyc_Absyn_Printf_ft,_tmp45F->f2=(int)_tmp459,_tmp45F->f3=(int)_tmp45A;_tmp45F;});a=_tmp93F;});else{
if(({struct _dyneither_ptr _tmp941=(struct _dyneither_ptr)_tmp458;Cyc_zstrcmp(_tmp941,({const char*_tmp460="scanf";_tag_dyneither(_tmp460,sizeof(char),6U);}));})== 0  || ({struct _dyneither_ptr _tmp940=(struct _dyneither_ptr)_tmp458;Cyc_zstrcmp(_tmp940,({const char*_tmp461="__scanf__";_tag_dyneither(_tmp461,sizeof(char),10U);}));})== 0)
({void*_tmp942=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp462=_cycalloc(sizeof(*_tmp462));_tmp462->tag=19U,_tmp462->f1=Cyc_Absyn_Scanf_ft,_tmp462->f2=(int)_tmp459,_tmp462->f3=(int)_tmp45A;_tmp462;});a=_tmp942;});else{
# 1594
({void*_tmp463=0U;({unsigned int _tmp944=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp943=({const char*_tmp464="unrecognized format type";_tag_dyneither(_tmp464,sizeof(char),25U);});Cyc_Warn_err(_tmp944,_tmp943,_tag_dyneither(_tmp463,sizeof(void*),0U));});});}}}else{
# 1596
({void*_tmp465=0U;({unsigned int _tmp946=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp945=({const char*_tmp466="unrecognized attribute";_tag_dyneither(_tmp466,sizeof(char),23U);});Cyc_Warn_err(_tmp946,_tmp945,_tag_dyneither(_tmp465,sizeof(void*),0U));});});}
({union Cyc_YYSTYPE _tmp947=Cyc_YY46(a);yyval=_tmp947;});
# 1599
goto _LL0;}case 70U: _LL87: _LL88: {
# 1601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1608 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 71U: _LL89: _LL8A: {
# 1611
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1613
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1610 "parse.y"
({union Cyc_YYSTYPE _tmp94B=Cyc_YY21(({void*_tmp94A=(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=17U,({struct _tuple0*_tmp949=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp467->f1=_tmp949;}),({struct Cyc_List_List*_tmp948=Cyc_yyget_YY40(yyyyvsp[1]);_tmp467->f2=_tmp948;}),_tmp467->f3=0,_tmp467->f4=0;_tmp467;});Cyc_Parse_type_spec(_tmp94A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp94B;});
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1614 "parse.y"
({union Cyc_YYSTYPE _tmp94C=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp94C;});
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1615 "parse.y"
({union Cyc_YYSTYPE _tmp94D=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp94D;});
goto _LL0;}case 74U: _LL8F: _LL90: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1616 "parse.y"
({union Cyc_YYSTYPE _tmp94E=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp94E;});
goto _LL0;}case 75U: _LL91: _LL92: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1617 "parse.y"
({union Cyc_YYSTYPE _tmp94F=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp94F;});
goto _LL0;}case 76U: _LL93: _LL94: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1618 "parse.y"
({union Cyc_YYSTYPE _tmp950=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp950;});
goto _LL0;}case 77U: _LL95: _LL96: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1619 "parse.y"
({union Cyc_YYSTYPE _tmp952=Cyc_YY21(({void*_tmp951=Cyc_Absyn_float_typ(0);Cyc_Parse_type_spec(_tmp951,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp952;});
goto _LL0;}case 78U: _LL97: _LL98: {
# 1622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1620 "parse.y"
({union Cyc_YYSTYPE _tmp954=Cyc_YY21(({void*_tmp953=Cyc_Absyn_float_typ(1);Cyc_Parse_type_spec(_tmp953,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp954;});
goto _LL0;}case 79U: _LL99: _LL9A: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1621 "parse.y"
({union Cyc_YYSTYPE _tmp955=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp955;});
goto _LL0;}case 80U: _LL9B: _LL9C: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1622 "parse.y"
({union Cyc_YYSTYPE _tmp956=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp956;});
goto _LL0;}case 81U: _LL9D: _LL9E: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1623 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1624 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1627 "parse.y"
({union Cyc_YYSTYPE _tmp959=Cyc_YY21(({void*_tmp958=(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->tag=27U,({struct Cyc_Absyn_Exp*_tmp957=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp468->f1=_tmp957;});_tmp468;});Cyc_Parse_type_spec(_tmp958,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp959;});
goto _LL0;}case 84U: _LLA3: _LLA4: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1629 "parse.y"
({union Cyc_YYSTYPE _tmp95C=Cyc_YY21(({void*_tmp95B=(void*)({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=28U,({struct _dyneither_ptr _tmp95A=({const char*_tmp469="__builtin_va_list";_tag_dyneither(_tmp469,sizeof(char),18U);});_tmp46A->f1=_tmp95A;}),_tmp46A->f2=& Cyc_Tcutil_bk;_tmp46A;});Cyc_Parse_type_spec(_tmp95B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp95C;});
goto _LL0;}case 85U: _LLA5: _LLA6: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1631 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 86U: _LLA7: _LLA8: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1633 "parse.y"
({union Cyc_YYSTYPE _tmp95E=Cyc_YY21(({void*_tmp95D=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmp95D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp95E;});
goto _LL0;}case 87U: _LLA9: _LLAA: {
# 1636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1635 "parse.y"
({union Cyc_YYSTYPE _tmp960=Cyc_YY21(({void*_tmp95F=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp95F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp960;});
goto _LL0;}case 88U: _LLAB: _LLAC: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1637 "parse.y"
({union Cyc_YYSTYPE _tmp962=Cyc_YY21(({void*_tmp961=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);Cyc_Parse_type_spec(_tmp961,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp962;});
goto _LL0;}case 89U: _LLAD: _LLAE: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1639 "parse.y"
({union Cyc_YYSTYPE _tmp966=Cyc_YY21(({void*_tmp965=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B->tag=10U,({struct Cyc_List_List*_tmp964=({unsigned int _tmp963=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp963,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});
# 1639
_tmp46B->f1=_tmp964;});_tmp46B;});Cyc_Parse_type_spec(_tmp965,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp966;});
# 1642
goto _LL0;}case 90U: _LLAF: _LLB0: {
# 1644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1643 "parse.y"
({union Cyc_YYSTYPE _tmp969=Cyc_YY21(({void*_tmp968=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->tag=15U,({void*_tmp967=Cyc_yyget_YY44(yyyyvsp[2]);_tmp46C->f1=_tmp967;});_tmp46C;});Cyc_Parse_type_spec(_tmp968,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp969;});
goto _LL0;}case 91U: _LLB1: _LLB2: {
# 1646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1645 "parse.y"
({union Cyc_YYSTYPE _tmp96C=Cyc_YY21(({void*_tmp96B=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->tag=15U,({void*_tmp96A=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);_tmp46D->f1=_tmp96A;});_tmp46D;});Cyc_Parse_type_spec(_tmp96B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp96C;});
# 1647
goto _LL0;}case 92U: _LLB3: _LLB4: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1648 "parse.y"
void*_tmp46E=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
({union Cyc_YYSTYPE _tmp96F=Cyc_YY21(({void*_tmp96E=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->tag=16U,({void*_tmp96D=Cyc_yyget_YY44(yyyyvsp[2]);_tmp46F->f1=_tmp96D;}),_tmp46F->f2=_tmp46E;_tmp46F;});Cyc_Parse_type_spec(_tmp96E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp96F;});
# 1651
goto _LL0;}case 93U: _LLB5: _LLB6: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1652 "parse.y"
({union Cyc_YYSTYPE _tmp973=Cyc_YY21(({void*_tmp972=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=16U,({void*_tmp971=Cyc_yyget_YY44(yyyyvsp[2]);_tmp470->f1=_tmp971;}),({void*_tmp970=Cyc_yyget_YY44(yyyyvsp[4]);_tmp470->f2=_tmp970;});_tmp470;});Cyc_Parse_type_spec(_tmp972,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp973;});
goto _LL0;}case 94U: _LLB7: _LLB8: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1654 "parse.y"
({union Cyc_YYSTYPE _tmp976=Cyc_YY21(({void*_tmp975=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=19U,({void*_tmp974=Cyc_yyget_YY44(yyyyvsp[2]);_tmp471->f1=_tmp974;});_tmp471;});Cyc_Parse_type_spec(_tmp975,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp976;});
goto _LL0;}case 95U: _LLB9: _LLBA: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1656 "parse.y"
({union Cyc_YYSTYPE _tmp979=Cyc_YY21(({void*_tmp978=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->tag=19U,({void*_tmp977=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0);_tmp472->f1=_tmp977;});_tmp472;});Cyc_Parse_type_spec(_tmp978,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp979;});
goto _LL0;}case 96U: _LLBB: _LLBC: {
# 1659
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1661
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1658 "parse.y"
({union Cyc_YYSTYPE _tmp97C=Cyc_YY21(({void*_tmp97B=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->tag=18U,({struct Cyc_Absyn_Exp*_tmp97A=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp473->f1=_tmp97A;});_tmp473;});Cyc_Parse_type_spec(_tmp97B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp97C;});
goto _LL0;}case 97U: _LLBD: _LLBE: {
# 1661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp97E=Cyc_YY43(({struct _dyneither_ptr _tmp97D=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmp97D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp97E;});
goto _LL0;}case 98U: _LLBF: _LLC0: {
# 1667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1668 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0U);
({union Cyc_YYSTYPE _tmp97F=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp733;_tmp733.print_const=1,_tmp733.q_volatile=0,_tmp733.q_restrict=0,_tmp733.real_const=1,_tmp733.loc=loc;_tmp733;}));yyval=_tmp97F;});
goto _LL0;}case 99U: _LLC1: _LLC2: {
# 1672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1670 "parse.y"
({union Cyc_YYSTYPE _tmp980=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp734;_tmp734.print_const=0,_tmp734.q_volatile=1,_tmp734.q_restrict=0,_tmp734.real_const=0,_tmp734.loc=0U;_tmp734;}));yyval=_tmp980;});
goto _LL0;}case 100U: _LLC3: _LLC4: {
# 1673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1671 "parse.y"
({union Cyc_YYSTYPE _tmp981=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp735;_tmp735.print_const=0,_tmp735.q_volatile=0,_tmp735.q_restrict=1,_tmp735.real_const=0,_tmp735.loc=0U;_tmp735;}));yyval=_tmp981;});
goto _LL0;}case 101U: _LLC5: _LLC6: {
# 1674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
struct Cyc_Absyn_TypeDecl*_tmp474=({struct Cyc_Absyn_TypeDecl*_tmp479=_cycalloc(sizeof(*_tmp479));({void*_tmp986=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp985=({struct Cyc_Absyn_Enumdecl*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp984=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp477->name=_tmp984;}),({struct Cyc_Core_Opt*_tmp983=({struct Cyc_Core_Opt*_tmp476=_cycalloc(sizeof(*_tmp476));({struct Cyc_List_List*_tmp982=Cyc_yyget_YY48(yyyyvsp[3]);_tmp476->v=_tmp982;});_tmp476;});_tmp477->fields=_tmp983;});_tmp477;});_tmp478->f1=_tmp985;});_tmp478;});_tmp479->r=_tmp986;}),_tmp479->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp479;});
# 1679
({union Cyc_YYSTYPE _tmp988=Cyc_YY21(({void*_tmp987=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->tag=26U,_tmp475->f1=_tmp474,_tmp475->f2=0;_tmp475;});Cyc_Parse_type_spec(_tmp987,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp988;});
# 1681
goto _LL0;}case 102U: _LLC7: _LLC8: {
# 1683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1682 "parse.y"
({union Cyc_YYSTYPE _tmp98B=Cyc_YY21(({void*_tmp98A=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->tag=13U,({struct _tuple0*_tmp989=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp47A->f1=_tmp989;}),_tmp47A->f2=0;_tmp47A;});Cyc_Parse_type_spec(_tmp98A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp98B;});
goto _LL0;}case 103U: _LLC9: _LLCA: {
# 1685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1684 "parse.y"
({union Cyc_YYSTYPE _tmp98E=Cyc_YY21(({void*_tmp98D=(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp47B=_cycalloc(sizeof(*_tmp47B));_tmp47B->tag=14U,({struct Cyc_List_List*_tmp98C=Cyc_yyget_YY48(yyyyvsp[2]);_tmp47B->f1=_tmp98C;});_tmp47B;});Cyc_Parse_type_spec(_tmp98D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp98E;});
goto _LL0;}case 104U: _LLCB: _LLCC: {
# 1687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp990=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct _tuple0*_tmp98F=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47C->name=_tmp98F;}),_tmp47C->tag=0,_tmp47C->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp47C;}));yyval=_tmp990;});
goto _LL0;}case 105U: _LLCD: _LLCE: {
# 1693
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1695
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1692 "parse.y"
({union Cyc_YYSTYPE _tmp993=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct _tuple0*_tmp992=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47D->name=_tmp992;}),({struct Cyc_Absyn_Exp*_tmp991=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp47D->tag=_tmp991;}),_tmp47D->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp47D;}));yyval=_tmp993;});
goto _LL0;}case 106U: _LLCF: _LLD0: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1696 "parse.y"
({union Cyc_YYSTYPE _tmp995=Cyc_YY48(({struct Cyc_List_List*_tmp47E=_cycalloc(sizeof(*_tmp47E));({struct Cyc_Absyn_Enumfield*_tmp994=Cyc_yyget_YY47(yyyyvsp[0]);_tmp47E->hd=_tmp994;}),_tmp47E->tl=0;_tmp47E;}));yyval=_tmp995;});
goto _LL0;}case 107U: _LLD1: _LLD2: {
# 1699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1697 "parse.y"
({union Cyc_YYSTYPE _tmp997=Cyc_YY48(({struct Cyc_List_List*_tmp47F=_cycalloc(sizeof(*_tmp47F));({struct Cyc_Absyn_Enumfield*_tmp996=Cyc_yyget_YY47(yyyyvsp[0]);_tmp47F->hd=_tmp996;}),_tmp47F->tl=0;_tmp47F;}));yyval=_tmp997;});
goto _LL0;}case 108U: _LLD3: _LLD4: {
# 1700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1698 "parse.y"
({union Cyc_YYSTYPE _tmp99A=Cyc_YY48(({struct Cyc_List_List*_tmp480=_cycalloc(sizeof(*_tmp480));({struct Cyc_Absyn_Enumfield*_tmp999=Cyc_yyget_YY47(yyyyvsp[0]);_tmp480->hd=_tmp999;}),({struct Cyc_List_List*_tmp998=Cyc_yyget_YY48(yyyyvsp[2]);_tmp480->tl=_tmp998;});_tmp480;}));yyval=_tmp99A;});
goto _LL0;}case 109U: _LLD5: _LLD6: {
# 1701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
({union Cyc_YYSTYPE _tmp99E=Cyc_YY21(({void*_tmp99D=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->tag=12U,({enum Cyc_Absyn_AggrKind _tmp99C=Cyc_yyget_YY22(yyyyvsp[0]);_tmp481->f1=_tmp99C;}),({struct Cyc_List_List*_tmp99B=Cyc_yyget_YY24(yyyyvsp[2]);_tmp481->f2=_tmp99B;});_tmp481;});Cyc_Parse_type_spec(_tmp99D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp99E;});
goto _LL0;}case 110U: _LLD7: _LLD8: {
# 1707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 1709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 1710 "parse.y"
struct Cyc_List_List*_tmp482=({unsigned int _tmp99F=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp99F,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp483=({unsigned int _tmp9A0=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9A0,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp484=({enum Cyc_Absyn_AggrKind _tmp9A6=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp9A5=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmp9A4=_tmp482;struct Cyc_Absyn_AggrdeclImpl*_tmp9A3=({
struct Cyc_List_List*_tmp9A2=_tmp483;struct Cyc_List_List*_tmp9A1=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmp9A2,_tmp9A1,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1712
Cyc_Absyn_aggr_tdecl(_tmp9A6,Cyc_Absyn_Public,_tmp9A5,_tmp9A4,_tmp9A3,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1715
({union Cyc_YYSTYPE _tmp9A8=Cyc_YY21(({void*_tmp9A7=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp485=_cycalloc(sizeof(*_tmp485));_tmp485->tag=26U,_tmp485->f1=_tmp484,_tmp485->f2=0;_tmp485;});Cyc_Parse_type_spec(_tmp9A7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9A8;});
# 1717
goto _LL0;}case 111U: _LLD9: _LLDA: {
# 1719
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1721
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1721 "parse.y"
struct Cyc_List_List*_tmp486=({unsigned int _tmp9A9=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9A9,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp487=({unsigned int _tmp9AA=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9AA,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp488=({enum Cyc_Absyn_AggrKind _tmp9B0=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmp9AF=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp9AE=_tmp486;struct Cyc_Absyn_AggrdeclImpl*_tmp9AD=({
struct Cyc_List_List*_tmp9AC=_tmp487;struct Cyc_List_List*_tmp9AB=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmp9AC,_tmp9AB,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1723
Cyc_Absyn_aggr_tdecl(_tmp9B0,Cyc_Absyn_Public,_tmp9AF,_tmp9AE,_tmp9AD,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1726
({union Cyc_YYSTYPE _tmp9B2=Cyc_YY21(({void*_tmp9B1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->tag=26U,_tmp489->f1=_tmp488,_tmp489->f2=0;_tmp489;});Cyc_Parse_type_spec(_tmp9B1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9B2;});
# 1728
goto _LL0;}case 112U: _LLDB: _LLDC: {
# 1730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1729 "parse.y"
({union Cyc_YYSTYPE _tmp9B8=Cyc_YY21(({void*_tmp9B7=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp9B6=({enum Cyc_Absyn_AggrKind _tmp9B5=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp9B4=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmp9B5,_tmp9B4,({struct Cyc_Core_Opt*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->v=(void*)1;_tmp48A;}));});(_tmp48B->f1).aggr_info=_tmp9B6;}),({struct Cyc_List_List*_tmp9B3=Cyc_yyget_YY40(yyyyvsp[3]);(_tmp48B->f1).targs=_tmp9B3;});_tmp48B;});Cyc_Parse_type_spec(_tmp9B7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9B8;});
# 1732
goto _LL0;}case 113U: _LLDD: _LLDE: {
# 1734
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1736
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1733 "parse.y"
({union Cyc_YYSTYPE _tmp9BD=Cyc_YY21(({void*_tmp9BC=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp9BB=({enum Cyc_Absyn_AggrKind _tmp9BA=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmp9BA,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});(_tmp48C->f1).aggr_info=_tmp9BB;}),({struct Cyc_List_List*_tmp9B9=Cyc_yyget_YY40(yyyyvsp[2]);(_tmp48C->f1).targs=_tmp9B9;});_tmp48C;});Cyc_Parse_type_spec(_tmp9BC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9BD;});
# 1736
goto _LL0;}case 114U: _LLDF: _LLE0:
# 1738
({union Cyc_YYSTYPE _tmp9BE=Cyc_YY40(0);yyval=_tmp9BE;});
goto _LL0;case 115U: _LLE1: _LLE2: {
# 1741
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1742 "parse.y"
({union Cyc_YYSTYPE _tmp9BF=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));yyval=_tmp9BF;});
goto _LL0;}case 116U: _LLE3: _LLE4: {
# 1745
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1746 "parse.y"
({union Cyc_YYSTYPE _tmp9C0=Cyc_YY22(Cyc_Absyn_StructA);yyval=_tmp9C0;});
goto _LL0;}case 117U: _LLE5: _LLE6: {
# 1749
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1747 "parse.y"
({union Cyc_YYSTYPE _tmp9C1=Cyc_YY22(Cyc_Absyn_UnionA);yyval=_tmp9C1;});
goto _LL0;}case 118U: _LLE7: _LLE8:
# 1750
({union Cyc_YYSTYPE _tmp9C2=Cyc_YY24(0);yyval=_tmp9C2;});
goto _LL0;case 119U: _LLE9: _LLEA: {
# 1753
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1755
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1754 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp48D=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp48D != 0;_tmp48D=_tmp48D->tl){
({struct Cyc_List_List*_tmp9C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp48D->hd,decls);decls=_tmp9C3;});}}{
# 1758
struct Cyc_List_List*_tmp48E=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp48E != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp48E,decls);
({union Cyc_YYSTYPE _tmp9C4=Cyc_YY24(decls);yyval=_tmp9C4;});
# 1763
goto _LL0;};}case 120U: _LLEB: _LLEC: {
# 1765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9C6=Cyc_YY25(({struct Cyc_List_List*_tmp48F=_cycalloc(sizeof(*_tmp48F));({struct Cyc_List_List*_tmp9C5=Cyc_yyget_YY24(yyyyvsp[0]);_tmp48F->hd=_tmp9C5;}),_tmp48F->tl=0;_tmp48F;}));yyval=_tmp9C6;});
goto _LL0;}case 121U: _LLED: _LLEE: {
# 1771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1770 "parse.y"
({union Cyc_YYSTYPE _tmp9C9=Cyc_YY25(({struct Cyc_List_List*_tmp490=_cycalloc(sizeof(*_tmp490));({struct Cyc_List_List*_tmp9C8=Cyc_yyget_YY24(yyyyvsp[1]);_tmp490->hd=_tmp9C8;}),({struct Cyc_List_List*_tmp9C7=Cyc_yyget_YY25(yyyyvsp[0]);_tmp490->tl=_tmp9C7;});_tmp490;}));yyval=_tmp9C9;});
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1774 "parse.y"
({union Cyc_YYSTYPE _tmp9CA=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));yyval=_tmp9CA;});
goto _LL0;}case 123U: _LLF1: _LLF2: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9CC=Cyc_YY19(({struct _tuple12*_tmp491=_region_malloc(yyr,sizeof(*_tmp491));_tmp491->tl=0,({struct _tuple11 _tmp9CB=Cyc_yyget_YY18(yyyyvsp[0]);_tmp491->hd=_tmp9CB;});_tmp491;}));yyval=_tmp9CC;});
goto _LL0;}case 124U: _LLF3: _LLF4: {
# 1783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1782 "parse.y"
({union Cyc_YYSTYPE _tmp9CF=Cyc_YY19(({struct _tuple12*_tmp492=_region_malloc(yyr,sizeof(*_tmp492));({struct _tuple12*_tmp9CE=Cyc_yyget_YY19(yyyyvsp[0]);_tmp492->tl=_tmp9CE;}),({struct _tuple11 _tmp9CD=Cyc_yyget_YY18(yyyyvsp[2]);_tmp492->hd=_tmp9CD;});_tmp492;}));yyval=_tmp9CF;});
goto _LL0;}case 125U: _LLF5: _LLF6: {
# 1785
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1787
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1787 "parse.y"
({union Cyc_YYSTYPE _tmp9D1=Cyc_YY18(({struct _tuple11 _tmp736;({struct Cyc_Parse_Declarator _tmp9D0=Cyc_yyget_YY27(yyyyvsp[0]);_tmp736.f1=_tmp9D0;}),_tmp736.f2=0;_tmp736;}));yyval=_tmp9D1;});
goto _LL0;}case 126U: _LLF7: _LLF8: {
# 1790
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1792
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1789 "parse.y"
({union Cyc_YYSTYPE _tmp9D4=Cyc_YY18(({struct _tuple11 _tmp737;({struct Cyc_Parse_Declarator _tmp9D3=Cyc_yyget_YY27(yyyyvsp[0]);_tmp737.f1=_tmp9D3;}),({struct Cyc_Absyn_Exp*_tmp9D2=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp737.f2=_tmp9D2;});_tmp737;}));yyval=_tmp9D4;});
goto _LL0;}case 127U: _LLF9: _LLFA: {
# 1792
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1794
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1795 "parse.y"
struct _RegionHandle _tmp493=_new_region("temp");struct _RegionHandle*temp=& _tmp493;_push_region(temp);
{struct _tuple25 _tmp494=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp495=_tmp494;struct Cyc_Absyn_Tqual _tmp4A3;struct Cyc_Parse_Type_specifier _tmp4A2;struct Cyc_List_List*_tmp4A1;_LL43D: _tmp4A3=_tmp495.f1;_tmp4A2=_tmp495.f2;_tmp4A1=_tmp495.f3;_LL43E:;
if(_tmp4A3.loc == 0)_tmp4A3.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp496=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp496 != 0;_tmp496=_tmp496->tl){
struct _tuple24*_tmp497=(struct _tuple24*)_tmp496->hd;struct _tuple24*_tmp498=_tmp497;struct Cyc_Parse_Declarator _tmp49E;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;_LL440: _tmp49E=_tmp498->f1;_tmp49D=_tmp498->f2;_tmp49C=_tmp498->f3;_LL441:;
({struct _tuple10*_tmp9D5=({struct _tuple10*_tmp499=_region_malloc(temp,sizeof(*_tmp499));_tmp499->tl=decls,_tmp499->hd=_tmp49E;_tmp499;});decls=_tmp9D5;});
({struct Cyc_List_List*_tmp9D7=({struct Cyc_List_List*_tmp49B=_region_malloc(temp,sizeof(*_tmp49B));
({struct _tuple16*_tmp9D6=({struct _tuple16*_tmp49A=_region_malloc(temp,sizeof(*_tmp49A));_tmp49A->f1=_tmp49D,_tmp49A->f2=_tmp49C;_tmp49A;});_tmp49B->hd=_tmp9D6;}),_tmp49B->tl=widths_and_reqs;_tmp49B;});
# 1803
widths_and_reqs=_tmp9D7;});}}
# 1806
({struct _tuple10*_tmp9D8=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);decls=_tmp9D8;});
({struct Cyc_List_List*_tmp9D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);widths_and_reqs=_tmp9D9;});{
void*_tmp49F=Cyc_Parse_speclist2typ(_tmp4A2,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp4A0=({struct _RegionHandle*_tmp9DC=temp;struct _RegionHandle*_tmp9DB=temp;struct Cyc_List_List*_tmp9DA=
Cyc_Parse_apply_tmss(temp,_tmp4A3,_tmp49F,decls,_tmp4A1);
# 1809
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp9DC,_tmp9DB,_tmp9DA,widths_and_reqs);});
# 1812
({union Cyc_YYSTYPE _tmp9DD=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp4A0));yyval=_tmp9DD;});
# 1814
_npop_handler(0U);goto _LL0;};};}
# 1796
;_pop_region(temp);}case 128U: _LLFB: _LLFC: {
# 1816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1822 "parse.y"
({union Cyc_YYSTYPE _tmp9E0=Cyc_YY35(({struct _tuple25 _tmp738;({struct Cyc_Absyn_Tqual _tmp9DF=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp738.f1=_tmp9DF;}),({struct Cyc_Parse_Type_specifier _tmp9DE=Cyc_yyget_YY21(yyyyvsp[0]);_tmp738.f2=_tmp9DE;}),_tmp738.f3=0;_tmp738;}));yyval=_tmp9E0;});
goto _LL0;}case 129U: _LLFD: _LLFE: {
# 1825
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1824 "parse.y"
struct _tuple25 _tmp4A4=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9E4=Cyc_YY35(({struct _tuple25 _tmp739;_tmp739.f1=_tmp4A4.f1,({struct Cyc_Parse_Type_specifier _tmp9E3=({unsigned int _tmp9E2=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp9E1=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp9E2,_tmp9E1,_tmp4A4.f2);});_tmp739.f2=_tmp9E3;}),_tmp739.f3=_tmp4A4.f3;_tmp739;}));yyval=_tmp9E4;});
goto _LL0;}case 130U: _LLFF: _LL100: {
# 1827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1826 "parse.y"
({union Cyc_YYSTYPE _tmp9E7=Cyc_YY35(({struct _tuple25 _tmp73A;({struct Cyc_Absyn_Tqual _tmp9E6=Cyc_yyget_YY23(yyyyvsp[0]);_tmp73A.f1=_tmp9E6;}),({struct Cyc_Parse_Type_specifier _tmp9E5=Cyc_Parse_empty_spec(0U);_tmp73A.f2=_tmp9E5;}),_tmp73A.f3=0;_tmp73A;}));yyval=_tmp9E7;});
goto _LL0;}case 131U: _LL101: _LL102: {
# 1829
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1831
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1828 "parse.y"
struct _tuple25 _tmp4A5=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp9EA=Cyc_YY35(({struct _tuple25 _tmp73B;({struct Cyc_Absyn_Tqual _tmp9E9=({struct Cyc_Absyn_Tqual _tmp9E8=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp9E8,_tmp4A5.f1);});_tmp73B.f1=_tmp9E9;}),_tmp73B.f2=_tmp4A5.f2,_tmp73B.f3=_tmp4A5.f3;_tmp73B;}));yyval=_tmp9EA;});
goto _LL0;}case 132U: _LL103: _LL104: {
# 1832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1831 "parse.y"
({union Cyc_YYSTYPE _tmp9EE=Cyc_YY35(({struct _tuple25 _tmp73C;({struct Cyc_Absyn_Tqual _tmp9ED=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp73C.f1=_tmp9ED;}),({struct Cyc_Parse_Type_specifier _tmp9EC=Cyc_Parse_empty_spec(0U);_tmp73C.f2=_tmp9EC;}),({struct Cyc_List_List*_tmp9EB=Cyc_yyget_YY45(yyyyvsp[0]);_tmp73C.f3=_tmp9EB;});_tmp73C;}));yyval=_tmp9EE;});
goto _LL0;}case 133U: _LL105: _LL106: {
# 1834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1833 "parse.y"
struct _tuple25 _tmp4A6=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9F1=Cyc_YY35(({struct _tuple25 _tmp73D;_tmp73D.f1=_tmp4A6.f1,_tmp73D.f2=_tmp4A6.f2,({struct Cyc_List_List*_tmp9F0=({struct Cyc_List_List*_tmp9EF=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9EF,_tmp4A6.f3);});_tmp73D.f3=_tmp9F0;});_tmp73D;}));yyval=_tmp9F1;});
goto _LL0;}case 134U: _LL107: _LL108: {
# 1836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9F4=Cyc_YY35(({struct _tuple25 _tmp73E;({struct Cyc_Absyn_Tqual _tmp9F3=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp73E.f1=_tmp9F3;}),({struct Cyc_Parse_Type_specifier _tmp9F2=Cyc_yyget_YY21(yyyyvsp[0]);_tmp73E.f2=_tmp9F2;}),_tmp73E.f3=0;_tmp73E;}));yyval=_tmp9F4;});
goto _LL0;}case 135U: _LL109: _LL10A: {
# 1842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1841 "parse.y"
struct _tuple25 _tmp4A7=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9F8=Cyc_YY35(({struct _tuple25 _tmp73F;_tmp73F.f1=_tmp4A7.f1,({struct Cyc_Parse_Type_specifier _tmp9F7=({unsigned int _tmp9F6=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp9F5=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp9F6,_tmp9F5,_tmp4A7.f2);});_tmp73F.f2=_tmp9F7;}),_tmp73F.f3=_tmp4A7.f3;_tmp73F;}));yyval=_tmp9F8;});
goto _LL0;}case 136U: _LL10B: _LL10C: {
# 1844
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1846
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1843 "parse.y"
({union Cyc_YYSTYPE _tmp9FB=Cyc_YY35(({struct _tuple25 _tmp740;({struct Cyc_Absyn_Tqual _tmp9FA=Cyc_yyget_YY23(yyyyvsp[0]);_tmp740.f1=_tmp9FA;}),({struct Cyc_Parse_Type_specifier _tmp9F9=Cyc_Parse_empty_spec(0U);_tmp740.f2=_tmp9F9;}),_tmp740.f3=0;_tmp740;}));yyval=_tmp9FB;});
goto _LL0;}case 137U: _LL10D: _LL10E: {
# 1846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1845 "parse.y"
struct _tuple25 _tmp4A8=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp9FE=Cyc_YY35(({struct _tuple25 _tmp741;({struct Cyc_Absyn_Tqual _tmp9FD=({struct Cyc_Absyn_Tqual _tmp9FC=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp9FC,_tmp4A8.f1);});_tmp741.f1=_tmp9FD;}),_tmp741.f2=_tmp4A8.f2,_tmp741.f3=_tmp4A8.f3;_tmp741;}));yyval=_tmp9FE;});
goto _LL0;}case 138U: _LL10F: _LL110: {
# 1849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1848 "parse.y"
({union Cyc_YYSTYPE _tmpA02=Cyc_YY35(({struct _tuple25 _tmp742;({struct Cyc_Absyn_Tqual _tmpA01=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp742.f1=_tmpA01;}),({struct Cyc_Parse_Type_specifier _tmpA00=Cyc_Parse_empty_spec(0U);_tmp742.f2=_tmpA00;}),({struct Cyc_List_List*_tmp9FF=Cyc_yyget_YY45(yyyyvsp[0]);_tmp742.f3=_tmp9FF;});_tmp742;}));yyval=_tmpA02;});
goto _LL0;}case 139U: _LL111: _LL112: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1850 "parse.y"
struct _tuple25 _tmp4A9=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmpA05=Cyc_YY35(({struct _tuple25 _tmp743;_tmp743.f1=_tmp4A9.f1,_tmp743.f2=_tmp4A9.f2,({struct Cyc_List_List*_tmpA04=({struct Cyc_List_List*_tmpA03=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpA03,_tmp4A9.f3);});_tmp743.f3=_tmpA04;});_tmp743;}));yyval=_tmpA05;});
goto _LL0;}case 140U: _LL113: _LL114: {
# 1853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1854 "parse.y"
({union Cyc_YYSTYPE _tmpA06=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));yyval=_tmpA06;});
goto _LL0;}case 141U: _LL115: _LL116: {
# 1857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpA08=Cyc_YY29(({struct Cyc_List_List*_tmp4AA=_region_malloc(yyr,sizeof(*_tmp4AA));({struct _tuple24*_tmpA07=Cyc_yyget_YY28(yyyyvsp[0]);_tmp4AA->hd=_tmpA07;}),_tmp4AA->tl=0;_tmp4AA;}));yyval=_tmpA08;});
goto _LL0;}case 142U: _LL117: _LL118: {
# 1863
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1865
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1862 "parse.y"
({union Cyc_YYSTYPE _tmpA0B=Cyc_YY29(({struct Cyc_List_List*_tmp4AB=_region_malloc(yyr,sizeof(*_tmp4AB));({struct _tuple24*_tmpA0A=Cyc_yyget_YY28(yyyyvsp[2]);_tmp4AB->hd=_tmpA0A;}),({struct Cyc_List_List*_tmpA09=Cyc_yyget_YY29(yyyyvsp[0]);_tmp4AB->tl=_tmpA09;});_tmp4AB;}));yyval=_tmpA0B;});
goto _LL0;}case 143U: _LL119: _LL11A: {
# 1865
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1867
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1867 "parse.y"
({union Cyc_YYSTYPE _tmpA0E=Cyc_YY28(({struct _tuple24*_tmp4AC=_region_malloc(yyr,sizeof(*_tmp4AC));({struct Cyc_Parse_Declarator _tmpA0D=Cyc_yyget_YY27(yyyyvsp[0]);_tmp4AC->f1=_tmpA0D;}),_tmp4AC->f2=0,({struct Cyc_Absyn_Exp*_tmpA0C=Cyc_yyget_YY55(yyyyvsp[1]);_tmp4AC->f3=_tmpA0C;});_tmp4AC;}));yyval=_tmpA0E;});
goto _LL0;}case 144U: _LL11B: _LL11C: {
# 1870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1871 "parse.y"
({union Cyc_YYSTYPE _tmpA14=Cyc_YY28(({struct _tuple24*_tmp4B0=_region_malloc(yyr,sizeof(*_tmp4B0));({struct _tuple0*_tmpA13=({struct _tuple0*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({union Cyc_Absyn_Nmspace _tmpA12=Cyc_Absyn_Rel_n(0);_tmp4AF->f1=_tmpA12;}),({struct _dyneither_ptr*_tmpA11=({struct _dyneither_ptr*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));({struct _dyneither_ptr _tmpA10=({const char*_tmp4AD="";_tag_dyneither(_tmp4AD,sizeof(char),1U);});*_tmp4AE=_tmpA10;});_tmp4AE;});_tmp4AF->f2=_tmpA11;});_tmp4AF;});(_tmp4B0->f1).id=_tmpA13;}),(_tmp4B0->f1).varloc=0U,(_tmp4B0->f1).tms=0,({struct Cyc_Absyn_Exp*_tmpA0F=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp4B0->f2=_tmpA0F;}),_tmp4B0->f3=0;_tmp4B0;}));yyval=_tmpA14;});
# 1873
goto _LL0;}case 145U: _LL11D: _LL11E:
# 1877
({union Cyc_YYSTYPE _tmpA19=Cyc_YY28(({struct _tuple24*_tmp4B4=_region_malloc(yyr,sizeof(*_tmp4B4));({struct _tuple0*_tmpA18=({struct _tuple0*_tmp4B3=_cycalloc(sizeof(*_tmp4B3));({union Cyc_Absyn_Nmspace _tmpA17=Cyc_Absyn_Rel_n(0);_tmp4B3->f1=_tmpA17;}),({struct _dyneither_ptr*_tmpA16=({struct _dyneither_ptr*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));({struct _dyneither_ptr _tmpA15=({const char*_tmp4B1="";_tag_dyneither(_tmp4B1,sizeof(char),1U);});*_tmp4B2=_tmpA15;});_tmp4B2;});_tmp4B3->f2=_tmpA16;});_tmp4B3;});(_tmp4B4->f1).id=_tmpA18;}),(_tmp4B4->f1).varloc=0U,(_tmp4B4->f1).tms=0,_tmp4B4->f2=0,_tmp4B4->f3=0;_tmp4B4;}));yyval=_tmpA19;});
# 1879
goto _LL0;case 146U: _LL11F: _LL120: {
# 1881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1879 "parse.y"
({union Cyc_YYSTYPE _tmpA1C=Cyc_YY28(({struct _tuple24*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));({struct Cyc_Parse_Declarator _tmpA1B=Cyc_yyget_YY27(yyyyvsp[0]);_tmp4B5->f1=_tmpA1B;}),({struct Cyc_Absyn_Exp*_tmpA1A=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4B5->f2=_tmpA1A;}),_tmp4B5->f3=0;_tmp4B5;}));yyval=_tmpA1C;});
goto _LL0;}case 147U: _LL121: _LL122: {
# 1882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1883 "parse.y"
({union Cyc_YYSTYPE _tmpA1D=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpA1D;});
goto _LL0;}case 148U: _LL123: _LL124:
# 1886
({union Cyc_YYSTYPE _tmpA1E=Cyc_YY55(0);yyval=_tmpA1E;});
goto _LL0;case 149U: _LL125: _LL126: {
# 1889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1888 "parse.y"
({union Cyc_YYSTYPE _tmpA1F=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpA1F;});
goto _LL0;}case 150U: _LL127: _LL128:
# 1891
({union Cyc_YYSTYPE _tmpA20=Cyc_YY55(0);yyval=_tmpA20;});
goto _LL0;case 151U: _LL129: _LL12A: {
# 1894
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1896
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1895 "parse.y"
int _tmp4B6=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp4B7=({unsigned int _tmpA21=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA21,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp4B8=({struct _tuple0*_tmpA26=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpA25=_tmp4B7;struct Cyc_Core_Opt*_tmpA24=({struct Cyc_Core_Opt*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));({struct Cyc_List_List*_tmpA22=Cyc_yyget_YY34(yyyyvsp[4]);_tmp4BA->v=_tmpA22;});_tmp4BA;});int _tmpA23=_tmp4B6;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmpA26,_tmpA25,_tmpA24,_tmpA23,(unsigned int)(yyyylsp[0]).first_line);});
# 1899
({union Cyc_YYSTYPE _tmpA28=Cyc_YY21(({void*_tmpA27=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->tag=26U,_tmp4B9->f1=_tmp4B8,_tmp4B9->f2=0;_tmp4B9;});Cyc_Parse_type_spec(_tmpA27,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA28;});
# 1901
goto _LL0;}case 152U: _LL12B: _LL12C: {
# 1903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1902 "parse.y"
int _tmp4BB=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA2D=Cyc_YY21(({void*_tmpA2C=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->tag=3U,({union Cyc_Absyn_DatatypeInfoU _tmpA2B=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp744;({struct _tuple0*_tmpA2A=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp744.name=_tmpA2A;}),_tmp744.is_extensible=_tmp4BB;_tmp744;}));(_tmp4BC->f1).datatype_info=_tmpA2B;}),({struct Cyc_List_List*_tmpA29=Cyc_yyget_YY40(yyyyvsp[2]);(_tmp4BC->f1).targs=_tmpA29;});_tmp4BC;});Cyc_Parse_type_spec(_tmpA2C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA2D;});
# 1905
goto _LL0;}case 153U: _LL12D: _LL12E: {
# 1907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1906 "parse.y"
int _tmp4BD=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA33=Cyc_YY21(({void*_tmpA32=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));_tmp4BE->tag=4U,({
union Cyc_Absyn_DatatypeFieldInfoU _tmpA31=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp745;({struct _tuple0*_tmpA30=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp745.datatype_name=_tmpA30;}),({struct _tuple0*_tmpA2F=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp745.field_name=_tmpA2F;}),_tmp745.is_extensible=_tmp4BD;_tmp745;}));(_tmp4BE->f1).field_info=_tmpA31;}),({struct Cyc_List_List*_tmpA2E=Cyc_yyget_YY40(yyyyvsp[4]);(_tmp4BE->f1).targs=_tmpA2E;});_tmp4BE;});
# 1907
Cyc_Parse_type_spec(_tmpA32,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA33;});
# 1911
goto _LL0;}case 154U: _LL12F: _LL130: {
# 1913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1914 "parse.y"
({union Cyc_YYSTYPE _tmpA34=Cyc_YY31(0);yyval=_tmpA34;});
goto _LL0;}case 155U: _LL131: _LL132: {
# 1917
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1919
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1915 "parse.y"
({union Cyc_YYSTYPE _tmpA35=Cyc_YY31(1);yyval=_tmpA35;});
goto _LL0;}case 156U: _LL133: _LL134: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1919 "parse.y"
({union Cyc_YYSTYPE _tmpA37=Cyc_YY34(({struct Cyc_List_List*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));({struct Cyc_Absyn_Datatypefield*_tmpA36=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4BF->hd=_tmpA36;}),_tmp4BF->tl=0;_tmp4BF;}));yyval=_tmpA37;});
goto _LL0;}case 157U: _LL135: _LL136: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1920 "parse.y"
({union Cyc_YYSTYPE _tmpA39=Cyc_YY34(({struct Cyc_List_List*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));({struct Cyc_Absyn_Datatypefield*_tmpA38=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4C0->hd=_tmpA38;}),_tmp4C0->tl=0;_tmp4C0;}));yyval=_tmpA39;});
goto _LL0;}case 158U: _LL137: _LL138: {
# 1923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1921 "parse.y"
({union Cyc_YYSTYPE _tmpA3C=Cyc_YY34(({struct Cyc_List_List*_tmp4C1=_cycalloc(sizeof(*_tmp4C1));({struct Cyc_Absyn_Datatypefield*_tmpA3B=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4C1->hd=_tmpA3B;}),({struct Cyc_List_List*_tmpA3A=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4C1->tl=_tmpA3A;});_tmp4C1;}));yyval=_tmpA3C;});
goto _LL0;}case 159U: _LL139: _LL13A: {
# 1924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1922 "parse.y"
({union Cyc_YYSTYPE _tmpA3F=Cyc_YY34(({struct Cyc_List_List*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));({struct Cyc_Absyn_Datatypefield*_tmpA3E=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4C2->hd=_tmpA3E;}),({struct Cyc_List_List*_tmpA3D=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4C2->tl=_tmpA3D;});_tmp4C2;}));yyval=_tmpA3F;});
goto _LL0;}case 160U: _LL13B: _LL13C:
# 1925
({union Cyc_YYSTYPE _tmpA40=Cyc_YY32(Cyc_Absyn_Public);yyval=_tmpA40;});
goto _LL0;case 161U: _LL13D: _LL13E: {
# 1928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1927 "parse.y"
({union Cyc_YYSTYPE _tmpA41=Cyc_YY32(Cyc_Absyn_Extern);yyval=_tmpA41;});
goto _LL0;}case 162U: _LL13F: _LL140: {
# 1930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1928 "parse.y"
({union Cyc_YYSTYPE _tmpA42=Cyc_YY32(Cyc_Absyn_Static);yyval=_tmpA42;});
goto _LL0;}case 163U: _LL141: _LL142: {
# 1931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1932 "parse.y"
({union Cyc_YYSTYPE _tmpA45=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));({struct _tuple0*_tmpA44=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4C3->name=_tmpA44;}),_tmp4C3->typs=0,_tmp4C3->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmpA43=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4C3->sc=_tmpA43;});_tmp4C3;}));yyval=_tmpA45;});
goto _LL0;}case 164U: _LL143: _LL144: {
# 1935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1934 "parse.y"
struct Cyc_List_List*_tmp4C4=({unsigned int _tmpA46=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmpA46,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
({union Cyc_YYSTYPE _tmpA49=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));({struct _tuple0*_tmpA48=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4C5->name=_tmpA48;}),_tmp4C5->typs=_tmp4C4,_tmp4C5->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmpA47=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4C5->sc=_tmpA47;});_tmp4C5;}));yyval=_tmpA49;});
goto _LL0;}case 165U: _LL145: _LL146: {
# 1938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1940 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 166U: _LL147: _LL148: {
# 1943
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1945
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1942 "parse.y"
struct Cyc_Parse_Declarator _tmp4C6=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA4C=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;_tmp746.id=_tmp4C6.id,_tmp746.varloc=_tmp4C6.varloc,({struct Cyc_List_List*_tmpA4B=({struct Cyc_List_List*_tmpA4A=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA4A,_tmp4C6.tms);});_tmp746.tms=_tmpA4B;});_tmp746;}));yyval=_tmpA4C;});
goto _LL0;}case 167U: _LL149: _LL14A: {
# 1946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 168U: _LL14B: _LL14C: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1951 "parse.y"
struct Cyc_Parse_Declarator _tmp4C7=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA4F=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;_tmp747.id=_tmp4C7.id,_tmp747.varloc=_tmp4C7.varloc,({struct Cyc_List_List*_tmpA4E=({struct Cyc_List_List*_tmpA4D=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA4D,_tmp4C7.tms);});_tmp747.tms=_tmpA4E;});_tmp747;}));yyval=_tmpA4F;});
goto _LL0;}case 169U: _LL14D: _LL14E: {
# 1955
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1957
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1957 "parse.y"
({union Cyc_YYSTYPE _tmpA51=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;({struct _tuple0*_tmpA50=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp748.id=_tmpA50;}),_tmp748.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp748.tms=0;_tmp748;}));yyval=_tmpA51;});
goto _LL0;}case 170U: _LL14F: _LL150: {
# 1960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1959 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 171U: _LL151: _LL152: {
# 1962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1963 "parse.y"
struct Cyc_Parse_Declarator _tmp4C8=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpA54=({struct Cyc_List_List*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));({void*_tmpA53=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));_tmp4C9->tag=5U,_tmp4C9->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA52=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4C9->f2=_tmpA52;});_tmp4C9;});_tmp4CA->hd=_tmpA53;}),_tmp4CA->tl=_tmp4C8.tms;_tmp4CA;});_tmp4C8.tms=_tmpA54;});
yyval=yyyyvsp[2];
# 1967
goto _LL0;}case 172U: _LL153: _LL154: {
# 1969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1968 "parse.y"
({union Cyc_YYSTYPE _tmpA5B=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;({struct _tuple0*_tmpA5A=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp749.id=_tmpA5A;}),({unsigned int _tmpA59=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp749.varloc=_tmpA59;}),({struct Cyc_List_List*_tmpA58=({struct Cyc_List_List*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));({void*_tmpA57=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));_tmp4CB->tag=0U,({union Cyc_Absyn_Constraint*_tmpA56=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4CB->f1=_tmpA56;}),_tmp4CB->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4CB;});_tmp4CC->hd=_tmpA57;}),({struct Cyc_List_List*_tmpA55=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4CC->tl=_tmpA55;});_tmp4CC;});_tmp749.tms=_tmpA58;});_tmp749;}));yyval=_tmpA5B;});
goto _LL0;}case 173U: _LL155: _LL156: {
# 1971
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1973
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1970 "parse.y"
({union Cyc_YYSTYPE _tmpA63=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;({struct _tuple0*_tmpA62=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp74A.id=_tmpA62;}),({unsigned int _tmpA61=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp74A.varloc=_tmpA61;}),({
struct Cyc_List_List*_tmpA60=({struct Cyc_List_List*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));({void*_tmpA5F=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4CD=_region_malloc(yyr,sizeof(*_tmp4CD));_tmp4CD->tag=1U,({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4CD->f1=_tmpA5E;}),({union Cyc_Absyn_Constraint*_tmpA5D=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4CD->f2=_tmpA5D;}),_tmp4CD->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4CD;});_tmp4CE->hd=_tmpA5F;}),({struct Cyc_List_List*_tmpA5C=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4CE->tl=_tmpA5C;});_tmp4CE;});_tmp74A.tms=_tmpA60;});_tmp74A;}));
# 1970
yyval=_tmpA63;});
# 1972
goto _LL0;}case 174U: _LL157: _LL158: {
# 1974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1973 "parse.y"
struct _tuple26*_tmp4CF=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4D0=_tmp4CF;struct Cyc_List_List*_tmp4DA;int _tmp4D9;struct Cyc_Absyn_VarargInfo*_tmp4D8;void*_tmp4D7;struct Cyc_List_List*_tmp4D6;_LL443: _tmp4DA=_tmp4D0->f1;_tmp4D9=_tmp4D0->f2;_tmp4D8=_tmp4D0->f3;_tmp4D7=_tmp4D0->f4;_tmp4D6=_tmp4D0->f5;_LL444:;{
struct Cyc_Absyn_Exp*_tmp4D1=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_yyget_YY55(yyyyvsp[5]);
({union Cyc_YYSTYPE _tmpA6A=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74B;({struct _tuple0*_tmpA69=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp74B.id=_tmpA69;}),({unsigned int _tmpA68=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp74B.varloc=_tmpA68;}),({struct Cyc_List_List*_tmpA67=({struct Cyc_List_List*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));({void*_tmpA66=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D4=_region_malloc(yyr,sizeof(*_tmp4D4));_tmp4D4->tag=3U,({void*_tmpA65=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4D3=_region_malloc(yyr,sizeof(*_tmp4D3));_tmp4D3->tag=1U,_tmp4D3->f1=_tmp4DA,_tmp4D3->f2=_tmp4D9,_tmp4D3->f3=_tmp4D8,_tmp4D3->f4=_tmp4D7,_tmp4D3->f5=_tmp4D6,_tmp4D3->f6=_tmp4D1,_tmp4D3->f7=_tmp4D2;_tmp4D3;});_tmp4D4->f1=_tmpA65;});_tmp4D4;});_tmp4D5->hd=_tmpA66;}),({struct Cyc_List_List*_tmpA64=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4D5->tl=_tmpA64;});_tmp4D5;});_tmp74B.tms=_tmpA67;});_tmp74B;}));yyval=_tmpA6A;});
# 1978
goto _LL0;};}case 175U: _LL159: _LL15A: {
# 1980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1979 "parse.y"
({union Cyc_YYSTYPE _tmpA75=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74C;({struct _tuple0*_tmpA74=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp74C.id=_tmpA74;}),({unsigned int _tmpA73=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp74C.varloc=_tmpA73;}),({
struct Cyc_List_List*_tmpA72=({struct Cyc_List_List*_tmp4DD=_region_malloc(yyr,sizeof(*_tmp4DD));({void*_tmpA71=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));_tmp4DC->tag=3U,({void*_tmpA70=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4DB=_region_malloc(yyr,sizeof(*_tmp4DB));_tmp4DB->tag=1U,_tmp4DB->f1=0,_tmp4DB->f2=0,_tmp4DB->f3=0,({
# 1982
void*_tmpA6F=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4DB->f4=_tmpA6F;}),({struct Cyc_List_List*_tmpA6E=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4DB->f5=_tmpA6E;}),({struct Cyc_Absyn_Exp*_tmpA6D=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4DB->f6=_tmpA6D;}),({struct Cyc_Absyn_Exp*_tmpA6C=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4DB->f7=_tmpA6C;});_tmp4DB;});
# 1980
_tmp4DC->f1=_tmpA70;});_tmp4DC;});_tmp4DD->hd=_tmpA71;}),({
# 1983
struct Cyc_List_List*_tmpA6B=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4DD->tl=_tmpA6B;});_tmp4DD;});
# 1980
_tmp74C.tms=_tmpA72;});_tmp74C;}));
# 1979
yyval=_tmpA75;});
# 1985
goto _LL0;}case 176U: _LL15B: _LL15C: {
# 1987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1986 "parse.y"
({union Cyc_YYSTYPE _tmpA7D=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74D;({struct _tuple0*_tmpA7C=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp74D.id=_tmpA7C;}),({unsigned int _tmpA7B=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp74D.varloc=_tmpA7B;}),({struct Cyc_List_List*_tmpA7A=({struct Cyc_List_List*_tmp4E0=_region_malloc(yyr,sizeof(*_tmp4E0));({void*_tmpA79=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4DF=_region_malloc(yyr,sizeof(*_tmp4DF));_tmp4DF->tag=3U,({void*_tmpA78=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4DE=_region_malloc(yyr,sizeof(*_tmp4DE));_tmp4DE->tag=0U,({struct Cyc_List_List*_tmpA77=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4DE->f1=_tmpA77;}),_tmp4DE->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4DE;});_tmp4DF->f1=_tmpA78;});_tmp4DF;});_tmp4E0->hd=_tmpA79;}),({struct Cyc_List_List*_tmpA76=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4E0->tl=_tmpA76;});_tmp4E0;});_tmp74D.tms=_tmpA7A;});_tmp74D;}));yyval=_tmpA7D;});
goto _LL0;}case 177U: _LL15D: _LL15E: {
# 1989
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1991
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1989 "parse.y"
struct Cyc_List_List*_tmp4E1=({unsigned int _tmpA7E=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA7E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpA84=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74E;({struct _tuple0*_tmpA83=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp74E.id=_tmpA83;}),({unsigned int _tmpA82=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp74E.varloc=_tmpA82;}),({struct Cyc_List_List*_tmpA81=({struct Cyc_List_List*_tmp4E3=_region_malloc(yyr,sizeof(*_tmp4E3));({void*_tmpA80=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4E2=_region_malloc(yyr,sizeof(*_tmp4E2));_tmp4E2->tag=4U,_tmp4E2->f1=_tmp4E1,_tmp4E2->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4E2->f3=0;_tmp4E2;});_tmp4E3->hd=_tmpA80;}),({struct Cyc_List_List*_tmpA7F=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4E3->tl=_tmpA7F;});_tmp4E3;});_tmp74E.tms=_tmpA81;});_tmp74E;}));yyval=_tmpA84;});
# 1992
goto _LL0;}case 178U: _LL15F: _LL160: {
# 1994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1993 "parse.y"
({union Cyc_YYSTYPE _tmpA8B=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74F;({struct _tuple0*_tmpA8A=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp74F.id=_tmpA8A;}),({unsigned int _tmpA89=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp74F.varloc=_tmpA89;}),({struct Cyc_List_List*_tmpA88=({struct Cyc_List_List*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));({void*_tmpA87=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));_tmp4E4->tag=5U,_tmp4E4->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA86=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4E4->f2=_tmpA86;});_tmp4E4;});_tmp4E5->hd=_tmpA87;}),({
struct Cyc_List_List*_tmpA85=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4E5->tl=_tmpA85;});_tmp4E5;});
# 1993
_tmp74F.tms=_tmpA88;});_tmp74F;}));yyval=_tmpA8B;});
# 1996
goto _LL0;}case 179U: _LL161: _LL162: {
# 1998
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpA8D=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp750;({struct _tuple0*_tmpA8C=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp750.id=_tmpA8C;}),_tmp750.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp750.tms=0;_tmp750;}));yyval=_tmpA8D;});
goto _LL0;}case 180U: _LL163: _LL164: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2003 "parse.y"
({union Cyc_YYSTYPE _tmpA8F=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp751;({struct _tuple0*_tmpA8E=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp751.id=_tmpA8E;}),_tmp751.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp751.tms=0;_tmp751;}));yyval=_tmpA8F;});
goto _LL0;}case 181U: _LL165: _LL166: {
# 2006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2005 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 182U: _LL167: _LL168: {
# 2008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2009 "parse.y"
struct Cyc_Parse_Declarator _tmp4E6=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpA92=({struct Cyc_List_List*_tmp4E8=_region_malloc(yyr,sizeof(*_tmp4E8));({void*_tmpA91=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));_tmp4E7->tag=5U,_tmp4E7->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA90=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4E7->f2=_tmpA90;});_tmp4E7;});_tmp4E8->hd=_tmpA91;}),_tmp4E8->tl=_tmp4E6.tms;_tmp4E8;});_tmp4E6.tms=_tmpA92;});
yyval=yyyyvsp[2];
# 2013
goto _LL0;}case 183U: _LL169: _LL16A: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2014 "parse.y"
struct Cyc_Parse_Declarator _tmp4E9=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA96=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp752;_tmp752.id=_tmp4E9.id,_tmp752.varloc=_tmp4E9.varloc,({
struct Cyc_List_List*_tmpA95=({struct Cyc_List_List*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));({void*_tmpA94=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));_tmp4EA->tag=0U,({union Cyc_Absyn_Constraint*_tmpA93=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4EA->f1=_tmpA93;}),_tmp4EA->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4EA;});_tmp4EB->hd=_tmpA94;}),_tmp4EB->tl=_tmp4E9.tms;_tmp4EB;});_tmp752.tms=_tmpA95;});_tmp752;}));
# 2015
yyval=_tmpA96;});
# 2017
goto _LL0;}case 184U: _LL16B: _LL16C: {
# 2019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2018 "parse.y"
struct Cyc_Parse_Declarator _tmp4EC=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA9B=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp753;_tmp753.id=_tmp4EC.id,_tmp753.varloc=_tmp4EC.varloc,({
struct Cyc_List_List*_tmpA9A=({struct Cyc_List_List*_tmp4EE=_region_malloc(yyr,sizeof(*_tmp4EE));({void*_tmpA99=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));_tmp4ED->tag=1U,({struct Cyc_Absyn_Exp*_tmpA98=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4ED->f1=_tmpA98;}),({union Cyc_Absyn_Constraint*_tmpA97=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4ED->f2=_tmpA97;}),_tmp4ED->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4ED;});_tmp4EE->hd=_tmpA99;}),_tmp4EE->tl=_tmp4EC.tms;_tmp4EE;});_tmp753.tms=_tmpA9A;});_tmp753;}));
# 2019
yyval=_tmpA9B;});
# 2022
goto _LL0;}case 185U: _LL16D: _LL16E: {
# 2024
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2026
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2023 "parse.y"
struct _tuple26*_tmp4EF=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4F0=_tmp4EF;struct Cyc_List_List*_tmp4FB;int _tmp4FA;struct Cyc_Absyn_VarargInfo*_tmp4F9;void*_tmp4F8;struct Cyc_List_List*_tmp4F7;_LL446: _tmp4FB=_tmp4F0->f1;_tmp4FA=_tmp4F0->f2;_tmp4F9=_tmp4F0->f3;_tmp4F8=_tmp4F0->f4;_tmp4F7=_tmp4F0->f5;_LL447:;{
struct Cyc_Absyn_Exp*_tmp4F1=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4F2=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp4F3=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA9F=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp754;_tmp754.id=_tmp4F3.id,_tmp754.varloc=_tmp4F3.varloc,({struct Cyc_List_List*_tmpA9E=({struct Cyc_List_List*_tmp4F6=_region_malloc(yyr,sizeof(*_tmp4F6));({void*_tmpA9D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4F5=_region_malloc(yyr,sizeof(*_tmp4F5));_tmp4F5->tag=3U,({void*_tmpA9C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));_tmp4F4->tag=1U,_tmp4F4->f1=_tmp4FB,_tmp4F4->f2=_tmp4FA,_tmp4F4->f3=_tmp4F9,_tmp4F4->f4=_tmp4F8,_tmp4F4->f5=_tmp4F7,_tmp4F4->f6=_tmp4F1,_tmp4F4->f7=_tmp4F2;_tmp4F4;});_tmp4F5->f1=_tmpA9C;});_tmp4F5;});_tmp4F6->hd=_tmpA9D;}),_tmp4F6->tl=_tmp4F3.tms;_tmp4F6;});_tmp754.tms=_tmpA9E;});_tmp754;}));yyval=_tmpA9F;});
# 2029
goto _LL0;};}case 186U: _LL16F: _LL170: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2030 "parse.y"
struct Cyc_Parse_Declarator _tmp4FC=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpAA7=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp755;_tmp755.id=_tmp4FC.id,_tmp755.varloc=_tmp4FC.varloc,({
struct Cyc_List_List*_tmpAA6=({struct Cyc_List_List*_tmp4FF=_region_malloc(yyr,sizeof(*_tmp4FF));({void*_tmpAA5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4FE=_region_malloc(yyr,sizeof(*_tmp4FE));_tmp4FE->tag=3U,({void*_tmpAA4=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4FD=_region_malloc(yyr,sizeof(*_tmp4FD));_tmp4FD->tag=1U,_tmp4FD->f1=0,_tmp4FD->f2=0,_tmp4FD->f3=0,({
# 2034
void*_tmpAA3=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4FD->f4=_tmpAA3;}),({struct Cyc_List_List*_tmpAA2=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4FD->f5=_tmpAA2;}),({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4FD->f6=_tmpAA1;}),({struct Cyc_Absyn_Exp*_tmpAA0=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4FD->f7=_tmpAA0;});_tmp4FD;});
# 2032
_tmp4FE->f1=_tmpAA4;});_tmp4FE;});_tmp4FF->hd=_tmpAA5;}),_tmp4FF->tl=_tmp4FC.tms;_tmp4FF;});_tmp755.tms=_tmpAA6;});_tmp755;}));
# 2031
yyval=_tmpAA7;});
# 2037
goto _LL0;}case 187U: _LL171: _LL172: {
# 2039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2038 "parse.y"
struct Cyc_Parse_Declarator _tmp500=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpAAC=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp756;_tmp756.id=_tmp500.id,_tmp756.varloc=_tmp500.varloc,({struct Cyc_List_List*_tmpAAB=({struct Cyc_List_List*_tmp503=_region_malloc(yyr,sizeof(*_tmp503));({void*_tmpAAA=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp502=_region_malloc(yyr,sizeof(*_tmp502));_tmp502->tag=3U,({void*_tmpAA9=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp501=_region_malloc(yyr,sizeof(*_tmp501));_tmp501->tag=0U,({struct Cyc_List_List*_tmpAA8=Cyc_yyget_YY36(yyyyvsp[2]);_tmp501->f1=_tmpAA8;}),_tmp501->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp501;});_tmp502->f1=_tmpAA9;});_tmp502;});_tmp503->hd=_tmpAAA;}),_tmp503->tl=_tmp500.tms;_tmp503;});_tmp756.tms=_tmpAAB;});_tmp756;}));yyval=_tmpAAC;});
goto _LL0;}case 188U: _LL173: _LL174: {
# 2042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2042 "parse.y"
struct Cyc_List_List*_tmp504=({unsigned int _tmpAAD=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpAAD,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp505=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpAB0=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp757;_tmp757.id=_tmp505.id,_tmp757.varloc=_tmp505.varloc,({struct Cyc_List_List*_tmpAAF=({struct Cyc_List_List*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));({void*_tmpAAE=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));_tmp506->tag=4U,_tmp506->f1=_tmp504,_tmp506->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp506->f3=0;_tmp506;});_tmp507->hd=_tmpAAE;}),_tmp507->tl=_tmp505.tms;_tmp507;});_tmp757.tms=_tmpAAF;});_tmp757;}));yyval=_tmpAB0;});
# 2046
goto _LL0;}case 189U: _LL175: _LL176: {
# 2048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2047 "parse.y"
struct Cyc_Parse_Declarator _tmp508=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpAB4=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp758;_tmp758.id=_tmp508.id,_tmp758.varloc=_tmp508.varloc,({struct Cyc_List_List*_tmpAB3=({struct Cyc_List_List*_tmp50A=_region_malloc(yyr,sizeof(*_tmp50A));({void*_tmpAB2=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));_tmp509->tag=5U,_tmp509->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpAB1=Cyc_yyget_YY45(yyyyvsp[1]);_tmp509->f2=_tmpAB1;});_tmp509;});_tmp50A->hd=_tmpAB2;}),_tmp50A->tl=_tmp508.tms;_tmp50A;});_tmp758.tms=_tmpAB3;});_tmp758;}));yyval=_tmpAB4;});
# 2050
goto _LL0;}case 190U: _LL177: _LL178: {
# 2052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2054 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 191U: _LL179: _LL17A: {
# 2057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2055 "parse.y"
({union Cyc_YYSTYPE _tmpAB6=Cyc_YY26(({struct Cyc_List_List*_tmpAB5=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpAB5,Cyc_yyget_YY26(yyyyvsp[1]));}));yyval=_tmpAB6;});
goto _LL0;}case 192U: _LL17B: _LL17C: {
# 2058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2059 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
({struct Cyc_List_List*_tmpAB9=({struct Cyc_List_List*_tmp50C=_region_malloc(yyr,sizeof(*_tmp50C));({void*_tmpAB8=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp50B=_region_malloc(yyr,sizeof(*_tmp50B));_tmp50B->tag=5U,_tmp50B->f1=(unsigned int)(yyyylsp[3]).first_line,({struct Cyc_List_List*_tmpAB7=Cyc_yyget_YY45(yyyyvsp[3]);_tmp50B->f2=_tmpAB7;});_tmp50B;});_tmp50C->hd=_tmpAB8;}),_tmp50C->tl=ans;_tmp50C;});ans=_tmpAB9;});{
# 2063
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp50D=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp50E=_tmp50D;unsigned int _tmp51A;union Cyc_Absyn_Constraint*_tmp519;union Cyc_Absyn_Constraint*_tmp518;_LL449: _tmp51A=_tmp50E.f1;_tmp519=_tmp50E.f2;_tmp518=_tmp50E.f3;_LL44A:;
if(Cyc_Absyn_porting_c_code)
({struct Cyc_Absyn_PtrLoc*_tmpABA=({struct Cyc_Absyn_PtrLoc*_tmp50F=_cycalloc(sizeof(*_tmp50F));_tmp50F->ptr_loc=_tmp51A,_tmp50F->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,_tmp50F->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp50F;});ptrloc=_tmpABA;});{
# 2068
struct _tuple14 _tmp510=({unsigned int _tmpABE=_tmp51A;union Cyc_Absyn_Constraint*_tmpABD=_tmp519;union Cyc_Absyn_Constraint*_tmpABC=_tmp518;void*_tmpABB=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmpABE,_tmpABD,_tmpABC,_tmpABB,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp511=_tmp510;union Cyc_Absyn_Constraint*_tmp517;union Cyc_Absyn_Constraint*_tmp516;union Cyc_Absyn_Constraint*_tmp515;void*_tmp514;_LL44C: _tmp517=_tmp511.f1;_tmp516=_tmp511.f2;_tmp515=_tmp511.f3;_tmp514=_tmp511.f4;_LL44D:;
({struct Cyc_List_List*_tmpAC1=({struct Cyc_List_List*_tmp513=_region_malloc(yyr,sizeof(*_tmp513));({void*_tmpAC0=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp512=_region_malloc(yyr,sizeof(*_tmp512));_tmp512->tag=2U,(_tmp512->f1).rgn=_tmp514,(_tmp512->f1).nullable=_tmp517,(_tmp512->f1).bounds=_tmp516,(_tmp512->f1).zero_term=_tmp515,(_tmp512->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmpABF=Cyc_yyget_YY23(yyyyvsp[4]);_tmp512->f2=_tmpABF;});_tmp512;});_tmp513->hd=_tmpAC0;}),_tmp513->tl=ans;_tmp513;});ans=_tmpAC1;});
({union Cyc_YYSTYPE _tmpAC2=Cyc_YY26(ans);yyval=_tmpAC2;});
# 2072
goto _LL0;};};}case 193U: _LL17D: _LL17E:
# 2074
({union Cyc_YYSTYPE _tmpAC3=Cyc_YY54(0);yyval=_tmpAC3;});
goto _LL0;case 194U: _LL17F: _LL180: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2075 "parse.y"
({union Cyc_YYSTYPE _tmpAC6=Cyc_YY54(({struct Cyc_List_List*_tmp51B=_region_malloc(yyr,sizeof(*_tmp51B));({void*_tmpAC5=Cyc_yyget_YY53(yyyyvsp[0]);_tmp51B->hd=_tmpAC5;}),({struct Cyc_List_List*_tmpAC4=Cyc_yyget_YY54(yyyyvsp[1]);_tmp51B->tl=_tmpAC4;});_tmp51B;}));yyval=_tmpAC6;});
goto _LL0;}case 195U: _LL181: _LL182: {
# 2078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2080 "parse.y"
({union Cyc_YYSTYPE _tmpAC8=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp51C=_region_malloc(yyr,sizeof(*_tmp51C));_tmp51C->tag=0U,({struct Cyc_Absyn_Exp*_tmpAC7=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp51C->f1=_tmpAC7;});_tmp51C;}));yyval=_tmpAC8;});
goto _LL0;}case 196U: _LL183: _LL184: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2082 "parse.y"
({union Cyc_YYSTYPE _tmpACA=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp51D=_region_malloc(yyr,sizeof(*_tmp51D));_tmp51D->tag=1U,({void*_tmpAC9=Cyc_yyget_YY44(yyyyvsp[2]);_tmp51D->f1=_tmpAC9;});_tmp51D;}));yyval=_tmpACA;});
goto _LL0;}case 197U: _LL185: _LL186: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2084 "parse.y"
({union Cyc_YYSTYPE _tmpACB=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp51E=_region_malloc(yyr,sizeof(*_tmp51E));_tmp51E->tag=2U;_tmp51E;}));yyval=_tmpACB;});
goto _LL0;}case 198U: _LL187: _LL188: {
# 2087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2086 "parse.y"
({union Cyc_YYSTYPE _tmpACC=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp51F=_region_malloc(yyr,sizeof(*_tmp51F));_tmp51F->tag=3U;_tmp51F;}));yyval=_tmpACC;});
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2088 "parse.y"
({union Cyc_YYSTYPE _tmpACD=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp520=_region_malloc(yyr,sizeof(*_tmp520));_tmp520->tag=4U;_tmp520;}));yyval=_tmpACD;});
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2090 "parse.y"
({union Cyc_YYSTYPE _tmpACE=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp521=_region_malloc(yyr,sizeof(*_tmp521));_tmp521->tag=5U;_tmp521;}));yyval=_tmpACE;});
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2092 "parse.y"
({union Cyc_YYSTYPE _tmpACF=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp522=_region_malloc(yyr,sizeof(*_tmp522));_tmp522->tag=6U;_tmp522;}));yyval=_tmpACF;});
goto _LL0;}case 202U: _LL18F: _LL190: {
# 2095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2094 "parse.y"
({union Cyc_YYSTYPE _tmpAD0=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp523=_region_malloc(yyr,sizeof(*_tmp523));_tmp523->tag=7U;_tmp523;}));yyval=_tmpAD0;});
goto _LL0;}case 203U: _LL191: _LL192: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2100 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpAD2=Cyc_YY1(({struct _tuple21*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->f1=loc,_tmp524->f2=Cyc_Absyn_true_conref,({union Cyc_Absyn_Constraint*_tmpAD1=Cyc_yyget_YY2(yyyyvsp[1]);_tmp524->f3=_tmpAD1;});_tmp524;}));yyval=_tmpAD2;});
# 2103
goto _LL0;}case 204U: _LL193: _LL194: {
# 2105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2104 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpAD4=Cyc_YY1(({struct _tuple21*_tmp525=_cycalloc(sizeof(*_tmp525));_tmp525->f1=loc,_tmp525->f2=Cyc_Absyn_false_conref,({union Cyc_Absyn_Constraint*_tmpAD3=Cyc_yyget_YY2(yyyyvsp[1]);_tmp525->f3=_tmpAD3;});_tmp525;}));yyval=_tmpAD4;});
# 2107
goto _LL0;}case 205U: _LL195: _LL196: {
# 2109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2108 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpAD5=Cyc_YY1(({struct _tuple21*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->f1=loc,_tmp526->f2=Cyc_Absyn_true_conref,_tmp526->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp526;}));yyval=_tmpAD5;});
# 2111
goto _LL0;}case 206U: _LL197: _LL198:
# 2113
({union Cyc_YYSTYPE _tmpAD6=Cyc_YY2(Cyc_Absyn_bounds_one_conref);yyval=_tmpAD6;});
goto _LL0;case 207U: _LL199: _LL19A: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2114 "parse.y"
({union Cyc_YYSTYPE _tmpAD8=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->tag=1U,({struct Cyc_Absyn_Exp*_tmpAD7=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp527->f1=_tmpAD7;});_tmp527;})));yyval=_tmpAD8;});
goto _LL0;}case 208U: _LL19B: _LL19C:
# 2117
({union Cyc_YYSTYPE _tmpAD9=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());yyval=_tmpAD9;});
goto _LL0;case 209U: _LL19D: _LL19E: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2118 "parse.y"
({union Cyc_YYSTYPE _tmpADA=Cyc_YY51(Cyc_Absyn_true_conref);yyval=_tmpADA;});
goto _LL0;}case 210U: _LL19F: _LL1A0: {
# 2121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2119 "parse.y"
({union Cyc_YYSTYPE _tmpADB=Cyc_YY51(Cyc_Absyn_false_conref);yyval=_tmpADB;});
goto _LL0;}case 211U: _LL1A1: _LL1A2:
# 2122
({union Cyc_YYSTYPE _tmpADC=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpADC;});
goto _LL0;case 212U: _LL1A3: _LL1A4: {
# 2125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2125 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL0;}case 213U: _LL1A5: _LL1A6: {
# 2128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2126 "parse.y"
({union Cyc_YYSTYPE _tmpADD=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpADD;});
goto _LL0;}case 214U: _LL1A7: _LL1A8:
# 2129
({union Cyc_YYSTYPE _tmpADE=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));yyval=_tmpADE;});
goto _LL0;case 215U: _LL1A9: _LL1AA: {
# 2132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2131 "parse.y"
({union Cyc_YYSTYPE _tmpAE0=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmpADF=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpADF,Cyc_yyget_YY23(yyyyvsp[1]));}));yyval=_tmpAE0;});
goto _LL0;}case 216U: _LL1AB: _LL1AC: {
# 2134
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2136
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2136 "parse.y"
({union Cyc_YYSTYPE _tmpAE4=Cyc_YY39(({struct _tuple26*_tmp528=_cycalloc(sizeof(*_tmp528));({struct Cyc_List_List*_tmpAE3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp528->f1=_tmpAE3;}),_tmp528->f2=0,_tmp528->f3=0,({void*_tmpAE2=Cyc_yyget_YY49(yyyyvsp[1]);_tmp528->f4=_tmpAE2;}),({struct Cyc_List_List*_tmpAE1=Cyc_yyget_YY50(yyyyvsp[2]);_tmp528->f5=_tmpAE1;});_tmp528;}));yyval=_tmpAE4;});
goto _LL0;}case 217U: _LL1AD: _LL1AE: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2138 "parse.y"
({union Cyc_YYSTYPE _tmpAE8=Cyc_YY39(({struct _tuple26*_tmp529=_cycalloc(sizeof(*_tmp529));({struct Cyc_List_List*_tmpAE7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp529->f1=_tmpAE7;}),_tmp529->f2=1,_tmp529->f3=0,({void*_tmpAE6=Cyc_yyget_YY49(yyyyvsp[3]);_tmp529->f4=_tmpAE6;}),({struct Cyc_List_List*_tmpAE5=Cyc_yyget_YY50(yyyyvsp[4]);_tmp529->f5=_tmpAE5;});_tmp529;}));yyval=_tmpAE8;});
goto _LL0;}case 218U: _LL1AF: _LL1B0: {
# 2141
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2143
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2141 "parse.y"
struct _tuple8*_tmp52A=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp52B=_tmp52A;struct _dyneither_ptr*_tmp531;struct Cyc_Absyn_Tqual _tmp530;void*_tmp52F;_LL44F: _tmp531=_tmp52B->f1;_tmp530=_tmp52B->f2;_tmp52F=_tmp52B->f3;_LL450:;{
struct Cyc_Absyn_VarargInfo*_tmp52C=({struct Cyc_Absyn_VarargInfo*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->name=_tmp531,_tmp52E->tq=_tmp530,_tmp52E->type=_tmp52F,({int _tmpAE9=Cyc_yyget_YY31(yyyyvsp[1]);_tmp52E->inject=_tmpAE9;});_tmp52E;});
({union Cyc_YYSTYPE _tmpAEC=Cyc_YY39(({struct _tuple26*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->f1=0,_tmp52D->f2=0,_tmp52D->f3=_tmp52C,({void*_tmpAEB=Cyc_yyget_YY49(yyyyvsp[3]);_tmp52D->f4=_tmpAEB;}),({struct Cyc_List_List*_tmpAEA=Cyc_yyget_YY50(yyyyvsp[4]);_tmp52D->f5=_tmpAEA;});_tmp52D;}));yyval=_tmpAEC;});
# 2145
goto _LL0;};}case 219U: _LL1B1: _LL1B2: {
# 2147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2147 "parse.y"
struct _tuple8*_tmp532=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp533=_tmp532;struct _dyneither_ptr*_tmp539;struct Cyc_Absyn_Tqual _tmp538;void*_tmp537;_LL452: _tmp539=_tmp533->f1;_tmp538=_tmp533->f2;_tmp537=_tmp533->f3;_LL453:;{
struct Cyc_Absyn_VarargInfo*_tmp534=({struct Cyc_Absyn_VarargInfo*_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536->name=_tmp539,_tmp536->tq=_tmp538,_tmp536->type=_tmp537,({int _tmpAED=Cyc_yyget_YY31(yyyyvsp[3]);_tmp536->inject=_tmpAED;});_tmp536;});
({union Cyc_YYSTYPE _tmpAF1=Cyc_YY39(({struct _tuple26*_tmp535=_cycalloc(sizeof(*_tmp535));({struct Cyc_List_List*_tmpAF0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp535->f1=_tmpAF0;}),_tmp535->f2=0,_tmp535->f3=_tmp534,({void*_tmpAEF=Cyc_yyget_YY49(yyyyvsp[5]);_tmp535->f4=_tmpAEF;}),({struct Cyc_List_List*_tmpAEE=Cyc_yyget_YY50(yyyyvsp[6]);_tmp535->f5=_tmpAEE;});_tmp535;}));yyval=_tmpAF1;});
# 2151
goto _LL0;};}case 220U: _LL1B3: _LL1B4: {
# 2153
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2155
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2155 "parse.y"
({union Cyc_YYSTYPE _tmpAF3=Cyc_YY44(({struct _dyneither_ptr _tmpAF2=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpAF2,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp53A=_cycalloc(sizeof(*_tmp53A));_tmp53A->tag=1U,_tmp53A->f1=0;_tmp53A;}));}));yyval=_tmpAF3;});
goto _LL0;}case 221U: _LL1B5: _LL1B6: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2156 "parse.y"
({union Cyc_YYSTYPE _tmpAF5=Cyc_YY44(({struct _dyneither_ptr _tmpAF4=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpAF4,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));yyval=_tmpAF5;});
goto _LL0;}case 222U: _LL1B7: _LL1B8:
# 2159
({union Cyc_YYSTYPE _tmpAF6=Cyc_YY49(0);yyval=_tmpAF6;});
goto _LL0;case 223U: _LL1B9: _LL1BA: {
# 2162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2160 "parse.y"
({union Cyc_YYSTYPE _tmpAF8=Cyc_YY49((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->tag=24U,({struct Cyc_List_List*_tmpAF7=Cyc_yyget_YY40(yyyyvsp[1]);_tmp53B->f1=_tmpAF7;});_tmp53B;}));yyval=_tmpAF8;});
goto _LL0;}case 224U: _LL1BB: _LL1BC:
# 2163
({union Cyc_YYSTYPE _tmpAF9=Cyc_YY50(0);yyval=_tmpAF9;});
goto _LL0;case 225U: _LL1BD: _LL1BE: {
# 2166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2165 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 226U: _LL1BF: _LL1C0: {
# 2168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2173 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp53C=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp542=_cycalloc(sizeof(*_tmp542));_tmp542->tag=2U,_tmp542->f1=0,_tmp542->f2=& Cyc_Tcutil_trk;_tmp542;});
struct _dyneither_ptr _tmp53D=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp53E=Cyc_Parse_id2type(_tmp53D,(void*)_tmp53C);
({union Cyc_YYSTYPE _tmpAFD=Cyc_YY50(({struct Cyc_List_List*_tmp541=_cycalloc(sizeof(*_tmp541));({struct _tuple29*_tmpAFC=({struct _tuple29*_tmp540=_cycalloc(sizeof(*_tmp540));({void*_tmpAFB=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->tag=24U,({struct Cyc_List_List*_tmpAFA=Cyc_yyget_YY40(yyyyvsp[0]);_tmp53F->f1=_tmpAFA;});_tmp53F;});_tmp540->f1=_tmpAFB;}),_tmp540->f2=_tmp53E;_tmp540;});_tmp541->hd=_tmpAFC;}),_tmp541->tl=0;_tmp541;}));yyval=_tmpAFD;});
# 2178
goto _LL0;}case 227U: _LL1C1: _LL1C2: {
# 2180
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2182
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2180 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp543=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->tag=2U,_tmp549->f1=0,_tmp549->f2=& Cyc_Tcutil_trk;_tmp549;});
struct _dyneither_ptr _tmp544=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp545=Cyc_Parse_id2type(_tmp544,(void*)_tmp543);
({union Cyc_YYSTYPE _tmpB02=Cyc_YY50(({struct Cyc_List_List*_tmp548=_cycalloc(sizeof(*_tmp548));({struct _tuple29*_tmpB01=({struct _tuple29*_tmp547=_cycalloc(sizeof(*_tmp547));({void*_tmpB00=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->tag=24U,({struct Cyc_List_List*_tmpAFF=Cyc_yyget_YY40(yyyyvsp[0]);_tmp546->f1=_tmpAFF;});_tmp546;});_tmp547->f1=_tmpB00;}),_tmp547->f2=_tmp545;_tmp547;});_tmp548->hd=_tmpB01;}),({struct Cyc_List_List*_tmpAFE=Cyc_yyget_YY50(yyyyvsp[4]);_tmp548->tl=_tmpAFE;});_tmp548;}));yyval=_tmpB02;});
# 2185
goto _LL0;}case 228U: _LL1C3: _LL1C4:
# 2187
({union Cyc_YYSTYPE _tmpB03=Cyc_YY31(0);yyval=_tmpB03;});
goto _LL0;case 229U: _LL1C5: _LL1C6: {
# 2190
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2192
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2191 "parse.y"
if(({struct _dyneither_ptr _tmpB04=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmpB04,({const char*_tmp54A="inject";_tag_dyneither(_tmp54A,sizeof(char),7U);}));})!= 0)
({void*_tmp54B=0U;({unsigned int _tmpB06=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB05=({const char*_tmp54C="missing type in function declaration";_tag_dyneither(_tmp54C,sizeof(char),37U);});Cyc_Warn_err(_tmpB06,_tmpB05,_tag_dyneither(_tmp54B,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB07=Cyc_YY31(1);yyval=_tmpB07;});
# 2195
goto _LL0;}case 230U: _LL1C7: _LL1C8: {
# 2197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2198 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 231U: _LL1C9: _LL1CA: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2199 "parse.y"
({union Cyc_YYSTYPE _tmpB09=Cyc_YY40(({struct Cyc_List_List*_tmpB08=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB08,Cyc_yyget_YY40(yyyyvsp[2]));}));yyval=_tmpB09;});
goto _LL0;}case 232U: _LL1CB: _LL1CC: {
# 2202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2203 "parse.y"
({union Cyc_YYSTYPE _tmpB0A=Cyc_YY40(0);yyval=_tmpB0A;});
goto _LL0;}case 233U: _LL1CD: _LL1CE: {
# 2206
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2208
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2204 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 234U: _LL1CF: _LL1D0: {
# 2207
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2209
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2206 "parse.y"
({union Cyc_YYSTYPE _tmpB0D=Cyc_YY40(({struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*_tmp54E));({void*_tmpB0C=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->tag=25U,({void*_tmpB0B=Cyc_yyget_YY44(yyyyvsp[2]);_tmp54D->f1=_tmpB0B;});_tmp54D;});_tmp54E->hd=_tmpB0C;}),_tmp54E->tl=0;_tmp54E;}));yyval=_tmpB0D;});
goto _LL0;}case 235U: _LL1D1: _LL1D2: {
# 2209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2208 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
({union Cyc_YYSTYPE _tmpB0F=Cyc_YY40(({struct Cyc_List_List*_tmp54F=_cycalloc(sizeof(*_tmp54F));({void*_tmpB0E=Cyc_yyget_YY44(yyyyvsp[0]);_tmp54F->hd=_tmpB0E;}),_tmp54F->tl=0;_tmp54F;}));yyval=_tmpB0F;});
# 2211
goto _LL0;}case 236U: _LL1D3: _LL1D4: {
# 2213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2228 "parse.y"
({union Cyc_YYSTYPE _tmpB13=Cyc_YY40(({struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));({void*_tmpB12=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->tag=23U,({void*_tmpB11=({struct _tuple8*_tmpB10=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpB10,(unsigned int)(yyyylsp[0]).first_line);});_tmp550->f1=_tmpB11;});_tmp550;});_tmp551->hd=_tmpB12;}),_tmp551->tl=0;_tmp551;}));yyval=_tmpB13;});
goto _LL0;}case 237U: _LL1D5: _LL1D6: {
# 2231
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2230 "parse.y"
({union Cyc_YYSTYPE _tmpB18=Cyc_YY40(({struct Cyc_List_List*_tmp553=_cycalloc(sizeof(*_tmp553));({void*_tmpB17=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->tag=23U,({void*_tmpB16=({struct _tuple8*_tmpB15=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpB15,(unsigned int)(yyyylsp[0]).first_line);});_tmp552->f1=_tmpB16;});_tmp552;});_tmp553->hd=_tmpB17;}),({struct Cyc_List_List*_tmpB14=Cyc_yyget_YY40(yyyyvsp[2]);_tmp553->tl=_tmpB14;});_tmp553;}));yyval=_tmpB18;});
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2233
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2235
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB1A=Cyc_YY38(({struct Cyc_List_List*_tmp554=_cycalloc(sizeof(*_tmp554));({struct _tuple8*_tmpB19=Cyc_yyget_YY37(yyyyvsp[0]);_tmp554->hd=_tmpB19;}),_tmp554->tl=0;_tmp554;}));yyval=_tmpB1A;});
goto _LL0;}case 239U: _LL1D9: _LL1DA: {
# 2239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2238 "parse.y"
({union Cyc_YYSTYPE _tmpB1D=Cyc_YY38(({struct Cyc_List_List*_tmp555=_cycalloc(sizeof(*_tmp555));({struct _tuple8*_tmpB1C=Cyc_yyget_YY37(yyyyvsp[2]);_tmp555->hd=_tmpB1C;}),({struct Cyc_List_List*_tmpB1B=Cyc_yyget_YY38(yyyyvsp[0]);_tmp555->tl=_tmpB1B;});_tmp555;}));yyval=_tmpB1D;});
goto _LL0;}case 240U: _LL1DB: _LL1DC: {
# 2241
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2243
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
struct _tuple25 _tmp556=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp557=_tmp556;struct Cyc_Absyn_Tqual _tmp56D;struct Cyc_Parse_Type_specifier _tmp56C;struct Cyc_List_List*_tmp56B;_LL455: _tmp56D=_tmp557.f1;_tmp56C=_tmp557.f2;_tmp56B=_tmp557.f3;_LL456:;
if(_tmp56D.loc == 0)_tmp56D.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp558=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp559=_tmp558;struct _tuple0*_tmp56A;unsigned int _tmp569;struct Cyc_List_List*_tmp568;_LL458: _tmp56A=_tmp559.id;_tmp569=_tmp559.varloc;_tmp568=_tmp559.tms;_LL459:;{
void*_tmp55A=Cyc_Parse_speclist2typ(_tmp56C,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp55B=Cyc_Parse_apply_tms(_tmp56D,_tmp55A,_tmp56B,_tmp568);struct _tuple13 _tmp55C=_tmp55B;struct Cyc_Absyn_Tqual _tmp567;void*_tmp566;struct Cyc_List_List*_tmp565;struct Cyc_List_List*_tmp564;_LL45B: _tmp567=_tmp55C.f1;_tmp566=_tmp55C.f2;_tmp565=_tmp55C.f3;_tmp564=_tmp55C.f4;_LL45C:;
if(_tmp565 != 0)
({void*_tmp55D=0U;({unsigned int _tmpB1F=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB1E=({const char*_tmp55E="parameter with bad type params";_tag_dyneither(_tmp55E,sizeof(char),31U);});Cyc_Warn_err(_tmpB1F,_tmpB1E,_tag_dyneither(_tmp55D,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp56A))
({void*_tmp55F=0U;({unsigned int _tmpB21=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB20=({const char*_tmp560="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp560,sizeof(char),47U);});Cyc_Warn_err(_tmpB21,_tmpB20,_tag_dyneither(_tmp55F,sizeof(void*),0U));});});{
struct _dyneither_ptr*idopt=(*_tmp56A).f2;
if(_tmp564 != 0)
({void*_tmp561=0U;({unsigned int _tmpB23=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB22=({const char*_tmp562="extra attributes on parameter, ignoring";_tag_dyneither(_tmp562,sizeof(char),40U);});Cyc_Warn_warn(_tmpB23,_tmpB22,_tag_dyneither(_tmp561,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB24=Cyc_YY37(({struct _tuple8*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->f1=idopt,_tmp563->f2=_tmp567,_tmp563->f3=_tmp566;_tmp563;}));yyval=_tmpB24;});
# 2258
goto _LL0;};};};}case 241U: _LL1DD: _LL1DE: {
# 2260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2259 "parse.y"
struct _tuple25 _tmp56E=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp56F=_tmp56E;struct Cyc_Absyn_Tqual _tmp576;struct Cyc_Parse_Type_specifier _tmp575;struct Cyc_List_List*_tmp574;_LL45E: _tmp576=_tmp56F.f1;_tmp575=_tmp56F.f2;_tmp574=_tmp56F.f3;_LL45F:;
if(_tmp576.loc == 0)_tmp576.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp570=Cyc_Parse_speclist2typ(_tmp575,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp574 != 0)
({void*_tmp571=0U;({unsigned int _tmpB26=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB25=({const char*_tmp572="bad attributes on parameter, ignoring";_tag_dyneither(_tmp572,sizeof(char),38U);});Cyc_Warn_warn(_tmpB26,_tmpB25,_tag_dyneither(_tmp571,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB27=Cyc_YY37(({struct _tuple8*_tmp573=_cycalloc(sizeof(*_tmp573));_tmp573->f1=0,_tmp573->f2=_tmp576,_tmp573->f3=_tmp570;_tmp573;}));yyval=_tmpB27;});
# 2266
goto _LL0;};}case 242U: _LL1DF: _LL1E0: {
# 2268
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2270
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2267 "parse.y"
struct _tuple25 _tmp577=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp578=_tmp577;struct Cyc_Absyn_Tqual _tmp588;struct Cyc_Parse_Type_specifier _tmp587;struct Cyc_List_List*_tmp586;_LL461: _tmp588=_tmp578.f1;_tmp587=_tmp578.f2;_tmp586=_tmp578.f3;_LL462:;
if(_tmp588.loc == 0)_tmp588.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp579=Cyc_Parse_speclist2typ(_tmp587,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp57A=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp57B=Cyc_Parse_apply_tms(_tmp588,_tmp579,_tmp586,_tmp57A);struct _tuple13 _tmp57C=_tmp57B;struct Cyc_Absyn_Tqual _tmp585;void*_tmp584;struct Cyc_List_List*_tmp583;struct Cyc_List_List*_tmp582;_LL464: _tmp585=_tmp57C.f1;_tmp584=_tmp57C.f2;_tmp583=_tmp57C.f3;_tmp582=_tmp57C.f4;_LL465:;
if(_tmp583 != 0)
({void*_tmp57D=0U;({unsigned int _tmpB29=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB28=({const char*_tmp57E="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp57E,sizeof(char),49U);});Cyc_Warn_warn(_tmpB29,_tmpB28,_tag_dyneither(_tmp57D,sizeof(void*),0U));});});
# 2275
if(_tmp582 != 0)
({void*_tmp57F=0U;({unsigned int _tmpB2B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB2A=({const char*_tmp580="bad attributes on parameter, ignoring";_tag_dyneither(_tmp580,sizeof(char),38U);});Cyc_Warn_warn(_tmpB2B,_tmpB2A,_tag_dyneither(_tmp57F,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB2C=Cyc_YY37(({struct _tuple8*_tmp581=_cycalloc(sizeof(*_tmp581));_tmp581->f1=0,_tmp581->f2=_tmp585,_tmp581->f3=_tmp584;_tmp581;}));yyval=_tmpB2C;});
# 2279
goto _LL0;};}case 243U: _LL1E1: _LL1E2: {
# 2281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2283 "parse.y"
({union Cyc_YYSTYPE _tmpB2D=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));yyval=_tmpB2D;});
goto _LL0;}case 244U: _LL1E3: _LL1E4: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2287 "parse.y"
({union Cyc_YYSTYPE _tmpB30=Cyc_YY36(({struct Cyc_List_List*_tmp58A=_cycalloc(sizeof(*_tmp58A));({struct _dyneither_ptr*_tmpB2F=({struct _dyneither_ptr*_tmp589=_cycalloc(sizeof(*_tmp589));({struct _dyneither_ptr _tmpB2E=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp589=_tmpB2E;});_tmp589;});_tmp58A->hd=_tmpB2F;}),_tmp58A->tl=0;_tmp58A;}));yyval=_tmpB30;});
goto _LL0;}case 245U: _LL1E5: _LL1E6: {
# 2290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2289 "parse.y"
({union Cyc_YYSTYPE _tmpB34=Cyc_YY36(({struct Cyc_List_List*_tmp58C=_cycalloc(sizeof(*_tmp58C));({struct _dyneither_ptr*_tmpB33=({struct _dyneither_ptr*_tmp58B=_cycalloc(sizeof(*_tmp58B));({struct _dyneither_ptr _tmpB32=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp58B=_tmpB32;});_tmp58B;});_tmp58C->hd=_tmpB33;}),({struct Cyc_List_List*_tmpB31=Cyc_yyget_YY36(yyyyvsp[0]);_tmp58C->tl=_tmpB31;});_tmp58C;}));yyval=_tmpB34;});
goto _LL0;}case 246U: _LL1E7: _LL1E8: {
# 2292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2293 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 247U: _LL1E9: _LL1EA: {
# 2296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2294 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 248U: _LL1EB: _LL1EC: {
# 2297
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2299
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2299 "parse.y"
({union Cyc_YYSTYPE _tmpB36=Cyc_Exp_tok(({void*_tmpB35=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->tag=36U,_tmp58D->f1=0,_tmp58D->f2=0;_tmp58D;});Cyc_Absyn_new_exp(_tmpB35,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB36;});
goto _LL0;}case 249U: _LL1ED: _LL1EE: {
# 2302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2301 "parse.y"
({union Cyc_YYSTYPE _tmpB39=Cyc_Exp_tok(({void*_tmpB38=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->tag=36U,_tmp58E->f1=0,({struct Cyc_List_List*_tmpB37=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp58E->f2=_tmpB37;});_tmp58E;});Cyc_Absyn_new_exp(_tmpB38,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB39;});
goto _LL0;}case 250U: _LL1EF: _LL1F0: {
# 2304
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2306
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2303 "parse.y"
({union Cyc_YYSTYPE _tmpB3C=Cyc_Exp_tok(({void*_tmpB3B=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->tag=36U,_tmp58F->f1=0,({struct Cyc_List_List*_tmpB3A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp58F->f2=_tmpB3A;});_tmp58F;});Cyc_Absyn_new_exp(_tmpB3B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB3C;});
goto _LL0;}case 251U: _LL1F1: _LL1F2: {
# 2306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2305 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp590=({unsigned int _tmpB41=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmpB40=({struct _tuple0*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB3E=({struct _dyneither_ptr*_tmp592=_cycalloc(sizeof(*_tmp592));({struct _dyneither_ptr _tmpB3D=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp592=_tmpB3D;});_tmp592;});_tmp593->f2=_tmpB3E;});_tmp593;});void*_tmpB3F=Cyc_Absyn_uint_typ;Cyc_Absyn_new_vardecl(_tmpB41,_tmpB40,_tmpB3F,
Cyc_Absyn_uint_exp(0U,(unsigned int)(yyyylsp[2]).first_line));});
# 2308
(_tmp590->tq).real_const=1;
({union Cyc_YYSTYPE _tmpB45=Cyc_Exp_tok(({void*_tmpB44=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->tag=27U,_tmp591->f1=_tmp590,({struct Cyc_Absyn_Exp*_tmpB43=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp591->f2=_tmpB43;}),({struct Cyc_Absyn_Exp*_tmpB42=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp591->f3=_tmpB42;}),_tmp591->f4=0;_tmp591;});Cyc_Absyn_new_exp(_tmpB44,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB45;});
# 2311
goto _LL0;}case 252U: _LL1F3: _LL1F4: {
# 2313
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2315
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2313 "parse.y"
void*_tmp594=({struct _tuple8*_tmpB46=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpB46,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpB49=Cyc_Exp_tok(({void*_tmpB48=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp595=_cycalloc(sizeof(*_tmp595));_tmp595->tag=28U,({struct Cyc_Absyn_Exp*_tmpB47=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp595->f1=_tmpB47;}),_tmp595->f2=_tmp594,_tmp595->f3=0;_tmp595;});Cyc_Absyn_new_exp(_tmpB48,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB49;});
# 2316
goto _LL0;}case 253U: _LL1F5: _LL1F6: {
# 2318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB4C=Cyc_YY5(({struct Cyc_List_List*_tmp597=_cycalloc(sizeof(*_tmp597));({struct _tuple30*_tmpB4B=({struct _tuple30*_tmp596=_cycalloc(sizeof(*_tmp596));_tmp596->f1=0,({struct Cyc_Absyn_Exp*_tmpB4A=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp596->f2=_tmpB4A;});_tmp596;});_tmp597->hd=_tmpB4B;}),_tmp597->tl=0;_tmp597;}));yyval=_tmpB4C;});
goto _LL0;}case 254U: _LL1F7: _LL1F8: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2323 "parse.y"
({union Cyc_YYSTYPE _tmpB50=Cyc_YY5(({struct Cyc_List_List*_tmp599=_cycalloc(sizeof(*_tmp599));({struct _tuple30*_tmpB4F=({struct _tuple30*_tmp598=_cycalloc(sizeof(*_tmp598));({struct Cyc_List_List*_tmpB4E=Cyc_yyget_YY41(yyyyvsp[0]);_tmp598->f1=_tmpB4E;}),({struct Cyc_Absyn_Exp*_tmpB4D=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp598->f2=_tmpB4D;});_tmp598;});_tmp599->hd=_tmpB4F;}),_tmp599->tl=0;_tmp599;}));yyval=_tmpB50;});
goto _LL0;}case 255U: _LL1F9: _LL1FA: {
# 2326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2325 "parse.y"
({union Cyc_YYSTYPE _tmpB54=Cyc_YY5(({struct Cyc_List_List*_tmp59B=_cycalloc(sizeof(*_tmp59B));({struct _tuple30*_tmpB53=({struct _tuple30*_tmp59A=_cycalloc(sizeof(*_tmp59A));_tmp59A->f1=0,({struct Cyc_Absyn_Exp*_tmpB52=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp59A->f2=_tmpB52;});_tmp59A;});_tmp59B->hd=_tmpB53;}),({struct Cyc_List_List*_tmpB51=Cyc_yyget_YY5(yyyyvsp[0]);_tmp59B->tl=_tmpB51;});_tmp59B;}));yyval=_tmpB54;});
goto _LL0;}case 256U: _LL1FB: _LL1FC: {
# 2328
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2330
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2327 "parse.y"
({union Cyc_YYSTYPE _tmpB59=Cyc_YY5(({struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*_tmp59D));({struct _tuple30*_tmpB58=({struct _tuple30*_tmp59C=_cycalloc(sizeof(*_tmp59C));({struct Cyc_List_List*_tmpB57=Cyc_yyget_YY41(yyyyvsp[2]);_tmp59C->f1=_tmpB57;}),({struct Cyc_Absyn_Exp*_tmpB56=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp59C->f2=_tmpB56;});_tmp59C;});_tmp59D->hd=_tmpB58;}),({struct Cyc_List_List*_tmpB55=Cyc_yyget_YY5(yyyyvsp[0]);_tmp59D->tl=_tmpB55;});_tmp59D;}));yyval=_tmpB59;});
goto _LL0;}case 257U: _LL1FD: _LL1FE: {
# 2330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2331 "parse.y"
({union Cyc_YYSTYPE _tmpB5A=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));yyval=_tmpB5A;});
goto _LL0;}case 258U: _LL1FF: _LL200: {
# 2334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2332 "parse.y"
({union Cyc_YYSTYPE _tmpB5E=Cyc_YY41(({struct Cyc_List_List*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));({void*_tmpB5D=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));_tmp59F->tag=1U,({struct _dyneither_ptr*_tmpB5C=({struct _dyneither_ptr*_tmp59E=_cycalloc(sizeof(*_tmp59E));({struct _dyneither_ptr _tmpB5B=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp59E=_tmpB5B;});_tmp59E;});_tmp59F->f1=_tmpB5C;});_tmp59F;});_tmp5A0->hd=_tmpB5D;}),_tmp5A0->tl=0;_tmp5A0;}));yyval=_tmpB5E;});
goto _LL0;}case 259U: _LL201: _LL202: {
# 2335
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2337
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2337 "parse.y"
({union Cyc_YYSTYPE _tmpB60=Cyc_YY41(({struct Cyc_List_List*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));({void*_tmpB5F=Cyc_yyget_YY42(yyyyvsp[0]);_tmp5A1->hd=_tmpB5F;}),_tmp5A1->tl=0;_tmp5A1;}));yyval=_tmpB60;});
goto _LL0;}case 260U: _LL203: _LL204: {
# 2340
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2342
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2338 "parse.y"
({union Cyc_YYSTYPE _tmpB63=Cyc_YY41(({struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));({void*_tmpB62=Cyc_yyget_YY42(yyyyvsp[1]);_tmp5A2->hd=_tmpB62;}),({struct Cyc_List_List*_tmpB61=Cyc_yyget_YY41(yyyyvsp[0]);_tmp5A2->tl=_tmpB61;});_tmp5A2;}));yyval=_tmpB63;});
goto _LL0;}case 261U: _LL205: _LL206: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2342 "parse.y"
({union Cyc_YYSTYPE _tmpB65=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->tag=0U,({struct Cyc_Absyn_Exp*_tmpB64=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp5A3->f1=_tmpB64;});_tmp5A3;}));yyval=_tmpB65;});
goto _LL0;}case 262U: _LL207: _LL208: {
# 2345
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2347
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2343 "parse.y"
({union Cyc_YYSTYPE _tmpB68=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));_tmp5A5->tag=1U,({struct _dyneither_ptr*_tmpB67=({struct _dyneither_ptr*_tmp5A4=_cycalloc(sizeof(*_tmp5A4));({struct _dyneither_ptr _tmpB66=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp5A4=_tmpB66;});_tmp5A4;});_tmp5A5->f1=_tmpB67;});_tmp5A5;}));yyval=_tmpB68;});
goto _LL0;}case 263U: _LL209: _LL20A: {
# 2346
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2348
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2348 "parse.y"
struct _tuple25 _tmp5A6=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp5A7=_tmp5A6;struct Cyc_Absyn_Tqual _tmp5AE;struct Cyc_Parse_Type_specifier _tmp5AD;struct Cyc_List_List*_tmp5AC;_LL467: _tmp5AE=_tmp5A7.f1;_tmp5AD=_tmp5A7.f2;_tmp5AC=_tmp5A7.f3;_LL468:;{
void*_tmp5A8=Cyc_Parse_speclist2typ(_tmp5AD,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp5AC != 0)
({void*_tmp5A9=0U;({unsigned int _tmpB6A=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB69=({const char*_tmp5AA="ignoring attributes in type";_tag_dyneither(_tmp5AA,sizeof(char),28U);});Cyc_Warn_warn(_tmpB6A,_tmpB69,_tag_dyneither(_tmp5A9,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB6B=Cyc_YY37(({struct _tuple8*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));_tmp5AB->f1=0,_tmp5AB->f2=_tmp5AE,_tmp5AB->f3=_tmp5A8;_tmp5AB;}));yyval=_tmpB6B;});
# 2354
goto _LL0;};}case 264U: _LL20B: _LL20C: {
# 2356
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2358
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2355 "parse.y"
struct _tuple25 _tmp5AF=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp5B0=_tmp5AF;struct Cyc_Absyn_Tqual _tmp5BB;struct Cyc_Parse_Type_specifier _tmp5BA;struct Cyc_List_List*_tmp5B9;_LL46A: _tmp5BB=_tmp5B0.f1;_tmp5BA=_tmp5B0.f2;_tmp5B9=_tmp5B0.f3;_LL46B:;{
void*_tmp5B1=Cyc_Parse_speclist2typ(_tmp5BA,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp5B2=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp5B3=Cyc_Parse_apply_tms(_tmp5BB,_tmp5B1,_tmp5B9,_tmp5B2);
if(_tmp5B3.f3 != 0)
# 2361
({void*_tmp5B4=0U;({unsigned int _tmpB6D=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB6C=({const char*_tmp5B5="bad type params, ignoring";_tag_dyneither(_tmp5B5,sizeof(char),26U);});Cyc_Warn_warn(_tmpB6D,_tmpB6C,_tag_dyneither(_tmp5B4,sizeof(void*),0U));});});
if(_tmp5B3.f4 != 0)
({void*_tmp5B6=0U;({unsigned int _tmpB6F=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB6E=({const char*_tmp5B7="bad specifiers, ignoring";_tag_dyneither(_tmp5B7,sizeof(char),25U);});Cyc_Warn_warn(_tmpB6F,_tmpB6E,_tag_dyneither(_tmp5B6,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB70=Cyc_YY37(({struct _tuple8*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->f1=0,_tmp5B8->f2=_tmp5B3.f1,_tmp5B8->f3=_tmp5B3.f2;_tmp5B8;}));yyval=_tmpB70;});
# 2366
goto _LL0;};}case 265U: _LL20D: _LL20E: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2369 "parse.y"
({union Cyc_YYSTYPE _tmpB72=Cyc_YY44(({struct _tuple8*_tmpB71=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpB71,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB72;});
goto _LL0;}case 266U: _LL20F: _LL210: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2370 "parse.y"
({union Cyc_YYSTYPE _tmpB73=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->tag=24U,_tmp5BC->f1=0;_tmp5BC;}));yyval=_tmpB73;});
goto _LL0;}case 267U: _LL211: _LL212: {
# 2373
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2375
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2371 "parse.y"
({union Cyc_YYSTYPE _tmpB75=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->tag=24U,({struct Cyc_List_List*_tmpB74=Cyc_yyget_YY40(yyyyvsp[1]);_tmp5BD->f1=_tmpB74;});_tmp5BD;}));yyval=_tmpB75;});
goto _LL0;}case 268U: _LL213: _LL214: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2372 "parse.y"
({union Cyc_YYSTYPE _tmpB77=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->tag=25U,({void*_tmpB76=Cyc_yyget_YY44(yyyyvsp[2]);_tmp5BE->f1=_tmpB76;});_tmp5BE;}));yyval=_tmpB77;});
goto _LL0;}case 269U: _LL215: _LL216: {
# 2375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2373 "parse.y"
({union Cyc_YYSTYPE _tmpB7B=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->tag=24U,({struct Cyc_List_List*_tmpB7A=({struct Cyc_List_List*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));({void*_tmpB79=Cyc_yyget_YY44(yyyyvsp[0]);_tmp5BF->hd=_tmpB79;}),({struct Cyc_List_List*_tmpB78=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5BF->tl=_tmpB78;});_tmp5BF;});_tmp5C0->f1=_tmpB7A;});_tmp5C0;}));yyval=_tmpB7B;});
goto _LL0;}case 270U: _LL217: _LL218: {
# 2376
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2378
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB7D=Cyc_YY40(({struct Cyc_List_List*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));({void*_tmpB7C=Cyc_yyget_YY44(yyyyvsp[0]);_tmp5C1->hd=_tmpB7C;}),_tmp5C1->tl=0;_tmp5C1;}));yyval=_tmpB7D;});
goto _LL0;}case 271U: _LL219: _LL21A: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2380 "parse.y"
({union Cyc_YYSTYPE _tmpB80=Cyc_YY40(({struct Cyc_List_List*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));({void*_tmpB7F=Cyc_yyget_YY44(yyyyvsp[2]);_tmp5C2->hd=_tmpB7F;}),({struct Cyc_List_List*_tmpB7E=Cyc_yyget_YY40(yyyyvsp[0]);_tmp5C2->tl=_tmpB7E;});_tmp5C2;}));yyval=_tmpB80;});
goto _LL0;}case 272U: _LL21B: _LL21C: {
# 2383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2385 "parse.y"
({union Cyc_YYSTYPE _tmpB82=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp759;({struct Cyc_List_List*_tmpB81=Cyc_yyget_YY26(yyyyvsp[0]);_tmp759.tms=_tmpB81;});_tmp759;}));yyval=_tmpB82;});
goto _LL0;}case 273U: _LL21D: _LL21E: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2387 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 274U: _LL21F: _LL220: {
# 2390
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2392
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2389 "parse.y"
({union Cyc_YYSTYPE _tmpB85=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75A;({struct Cyc_List_List*_tmpB84=({struct Cyc_List_List*_tmpB83=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB83,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp75A.tms=_tmpB84;});_tmp75A;}));yyval=_tmpB85;});
goto _LL0;}case 275U: _LL221: _LL222: {
# 2392
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2394
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2394 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 276U: _LL223: _LL224: {
# 2397
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2399
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2396 "parse.y"
({union Cyc_YYSTYPE _tmpB89=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75B;({struct Cyc_List_List*_tmpB88=({struct Cyc_List_List*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));({void*_tmpB87=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));_tmp5C3->tag=0U,({union Cyc_Absyn_Constraint*_tmpB86=Cyc_yyget_YY51(yyyyvsp[2]);_tmp5C3->f1=_tmpB86;}),_tmp5C3->f2=(unsigned int)(yyyylsp[2]).first_line;_tmp5C3;});_tmp5C4->hd=_tmpB87;}),_tmp5C4->tl=0;_tmp5C4;});_tmp75B.tms=_tmpB88;});_tmp75B;}));yyval=_tmpB89;});
goto _LL0;}case 277U: _LL225: _LL226: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2398 "parse.y"
({union Cyc_YYSTYPE _tmpB8E=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75C;({struct Cyc_List_List*_tmpB8D=({struct Cyc_List_List*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));({void*_tmpB8C=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));_tmp5C5->tag=0U,({union Cyc_Absyn_Constraint*_tmpB8B=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5C5->f1=_tmpB8B;}),_tmp5C5->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5C5;});_tmp5C6->hd=_tmpB8C;}),({struct Cyc_List_List*_tmpB8A=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5C6->tl=_tmpB8A;});_tmp5C6;});_tmp75C.tms=_tmpB8D;});_tmp75C;}));yyval=_tmpB8E;});
goto _LL0;}case 278U: _LL227: _LL228: {
# 2401
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2403
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2400 "parse.y"
({union Cyc_YYSTYPE _tmpB93=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75D;({struct Cyc_List_List*_tmpB92=({struct Cyc_List_List*_tmp5C8=_region_malloc(yyr,sizeof(*_tmp5C8));({void*_tmpB91=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5C7=_region_malloc(yyr,sizeof(*_tmp5C7));_tmp5C7->tag=1U,({struct Cyc_Absyn_Exp*_tmpB90=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp5C7->f1=_tmpB90;}),({union Cyc_Absyn_Constraint*_tmpB8F=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5C7->f2=_tmpB8F;}),_tmp5C7->f3=(unsigned int)(yyyylsp[3]).first_line;_tmp5C7;});_tmp5C8->hd=_tmpB91;}),_tmp5C8->tl=0;_tmp5C8;});_tmp75D.tms=_tmpB92;});_tmp75D;}));yyval=_tmpB93;});
goto _LL0;}case 279U: _LL229: _LL22A: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2402 "parse.y"
({union Cyc_YYSTYPE _tmpB99=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75E;({struct Cyc_List_List*_tmpB98=({struct Cyc_List_List*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));({void*_tmpB97=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5C9=_region_malloc(yyr,sizeof(*_tmp5C9));_tmp5C9->tag=1U,({struct Cyc_Absyn_Exp*_tmpB96=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5C9->f1=_tmpB96;}),({union Cyc_Absyn_Constraint*_tmpB95=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5C9->f2=_tmpB95;}),_tmp5C9->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5C9;});_tmp5CA->hd=_tmpB97;}),({
struct Cyc_List_List*_tmpB94=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5CA->tl=_tmpB94;});_tmp5CA;});
# 2402
_tmp75E.tms=_tmpB98;});_tmp75E;}));yyval=_tmpB99;});
# 2405
goto _LL0;}case 280U: _LL22B: _LL22C: {
# 2407
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2409
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2406 "parse.y"
({union Cyc_YYSTYPE _tmpBA1=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp75F;({struct Cyc_List_List*_tmpBA0=({struct Cyc_List_List*_tmp5CD=_region_malloc(yyr,sizeof(*_tmp5CD));({void*_tmpB9F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CC=_region_malloc(yyr,sizeof(*_tmp5CC));_tmp5CC->tag=3U,({void*_tmpB9E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5CB=_region_malloc(yyr,sizeof(*_tmp5CB));_tmp5CB->tag=1U,_tmp5CB->f1=0,_tmp5CB->f2=0,_tmp5CB->f3=0,({void*_tmpB9D=Cyc_yyget_YY49(yyyyvsp[1]);_tmp5CB->f4=_tmpB9D;}),({struct Cyc_List_List*_tmpB9C=Cyc_yyget_YY50(yyyyvsp[2]);_tmp5CB->f5=_tmpB9C;}),({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5CB->f6=_tmpB9B;}),({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5CB->f7=_tmpB9A;});_tmp5CB;});_tmp5CC->f1=_tmpB9E;});_tmp5CC;});_tmp5CD->hd=_tmpB9F;}),_tmp5CD->tl=0;_tmp5CD;});_tmp75F.tms=_tmpBA0;});_tmp75F;}));yyval=_tmpBA1;});
# 2408
goto _LL0;}case 281U: _LL22D: _LL22E: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2409 "parse.y"
struct _tuple26*_tmp5CE=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp5CF=_tmp5CE;struct Cyc_List_List*_tmp5D7;int _tmp5D6;struct Cyc_Absyn_VarargInfo*_tmp5D5;void*_tmp5D4;struct Cyc_List_List*_tmp5D3;_LL46D: _tmp5D7=_tmp5CF->f1;_tmp5D6=_tmp5CF->f2;_tmp5D5=_tmp5CF->f3;_tmp5D4=_tmp5CF->f4;_tmp5D3=_tmp5CF->f5;_LL46E:;
({union Cyc_YYSTYPE _tmpBA7=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp760;({struct Cyc_List_List*_tmpBA6=({struct Cyc_List_List*_tmp5D2=_region_malloc(yyr,sizeof(*_tmp5D2));({void*_tmpBA5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5D1=_region_malloc(yyr,sizeof(*_tmp5D1));_tmp5D1->tag=3U,({void*_tmpBA4=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5D0=_region_malloc(yyr,sizeof(*_tmp5D0));_tmp5D0->tag=1U,_tmp5D0->f1=_tmp5D7,_tmp5D0->f2=_tmp5D6,_tmp5D0->f3=_tmp5D5,_tmp5D0->f4=_tmp5D4,_tmp5D0->f5=_tmp5D3,({struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_YY55(yyyyvsp[3]);_tmp5D0->f6=_tmpBA3;}),({struct Cyc_Absyn_Exp*_tmpBA2=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5D0->f7=_tmpBA2;});_tmp5D0;});_tmp5D1->f1=_tmpBA4;});_tmp5D1;});_tmp5D2->hd=_tmpBA5;}),_tmp5D2->tl=0;_tmp5D2;});_tmp760.tms=_tmpBA6;});_tmp760;}));yyval=_tmpBA7;});
# 2412
goto _LL0;}case 282U: _LL22F: _LL230: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2413 "parse.y"
({union Cyc_YYSTYPE _tmpBB0=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp761;({struct Cyc_List_List*_tmpBAF=({struct Cyc_List_List*_tmp5DA=_region_malloc(yyr,sizeof(*_tmp5DA));({void*_tmpBAE=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5D9=_region_malloc(yyr,sizeof(*_tmp5D9));_tmp5D9->tag=3U,({void*_tmpBAD=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5D8=_region_malloc(yyr,sizeof(*_tmp5D8));_tmp5D8->tag=1U,_tmp5D8->f1=0,_tmp5D8->f2=0,_tmp5D8->f3=0,({void*_tmpBAC=Cyc_yyget_YY49(yyyyvsp[2]);_tmp5D8->f4=_tmpBAC;}),({struct Cyc_List_List*_tmpBAB=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5D8->f5=_tmpBAB;}),({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5D8->f6=_tmpBAA;}),({struct Cyc_Absyn_Exp*_tmpBA9=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5D8->f7=_tmpBA9;});_tmp5D8;});_tmp5D9->f1=_tmpBAD;});_tmp5D9;});_tmp5DA->hd=_tmpBAE;}),({
struct Cyc_List_List*_tmpBA8=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5DA->tl=_tmpBA8;});_tmp5DA;});
# 2413
_tmp761.tms=_tmpBAF;});_tmp761;}));yyval=_tmpBB0;});
# 2416
goto _LL0;}case 283U: _LL231: _LL232: {
# 2418
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2420
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2417 "parse.y"
struct _tuple26*_tmp5DB=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5DC=_tmp5DB;struct Cyc_List_List*_tmp5E4;int _tmp5E3;struct Cyc_Absyn_VarargInfo*_tmp5E2;void*_tmp5E1;struct Cyc_List_List*_tmp5E0;_LL470: _tmp5E4=_tmp5DC->f1;_tmp5E3=_tmp5DC->f2;_tmp5E2=_tmp5DC->f3;_tmp5E1=_tmp5DC->f4;_tmp5E0=_tmp5DC->f5;_LL471:;
({union Cyc_YYSTYPE _tmpBB7=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp762;({struct Cyc_List_List*_tmpBB6=({struct Cyc_List_List*_tmp5DF=_region_malloc(yyr,sizeof(*_tmp5DF));({void*_tmpBB5=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5DE=_region_malloc(yyr,sizeof(*_tmp5DE));_tmp5DE->tag=3U,({void*_tmpBB4=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5DD=_region_malloc(yyr,sizeof(*_tmp5DD));_tmp5DD->tag=1U,_tmp5DD->f1=_tmp5E4,_tmp5DD->f2=_tmp5E3,_tmp5DD->f3=_tmp5E2,_tmp5DD->f4=_tmp5E1,_tmp5DD->f5=_tmp5E0,({
struct Cyc_Absyn_Exp*_tmpBB3=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5DD->f6=_tmpBB3;}),({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5DD->f7=_tmpBB2;});_tmp5DD;});
# 2418
_tmp5DE->f1=_tmpBB4;});_tmp5DE;});_tmp5DF->hd=_tmpBB5;}),({
struct Cyc_List_List*_tmpBB1=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5DF->tl=_tmpBB1;});_tmp5DF;});
# 2418
_tmp762.tms=_tmpBB6;});_tmp762;}));yyval=_tmpBB7;});
# 2421
goto _LL0;}case 284U: _LL233: _LL234: {
# 2423
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2425
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2423 "parse.y"
struct Cyc_List_List*_tmp5E5=({unsigned int _tmpBB8=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpBB8,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpBBC=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp763;({struct Cyc_List_List*_tmpBBB=({struct Cyc_List_List*_tmp5E7=_region_malloc(yyr,sizeof(*_tmp5E7));({void*_tmpBBA=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5E6=_region_malloc(yyr,sizeof(*_tmp5E6));_tmp5E6->tag=4U,_tmp5E6->f1=_tmp5E5,_tmp5E6->f2=(unsigned int)(yyyylsp[1]).first_line,_tmp5E6->f3=0;_tmp5E6;});_tmp5E7->hd=_tmpBBA;}),({
struct Cyc_List_List*_tmpBB9=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5E7->tl=_tmpBB9;});_tmp5E7;});
# 2424
_tmp763.tms=_tmpBBB;});_tmp763;}));yyval=_tmpBBC;});
# 2427
goto _LL0;}case 285U: _LL235: _LL236: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2428 "parse.y"
({union Cyc_YYSTYPE _tmpBC1=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp764;({struct Cyc_List_List*_tmpBC0=({struct Cyc_List_List*_tmp5E9=_region_malloc(yyr,sizeof(*_tmp5E9));({void*_tmpBBF=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5E8=_region_malloc(yyr,sizeof(*_tmp5E8));_tmp5E8->tag=5U,_tmp5E8->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpBBE=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5E8->f2=_tmpBBE;});_tmp5E8;});_tmp5E9->hd=_tmpBBF;}),({struct Cyc_List_List*_tmpBBD=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5E9->tl=_tmpBBD;});_tmp5E9;});_tmp764.tms=_tmpBC0;});_tmp764;}));yyval=_tmpBC1;});
# 2430
goto _LL0;}case 286U: _LL237: _LL238: {
# 2432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2434 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 287U: _LL239: _LL23A: {
# 2437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2435 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 288U: _LL23B: _LL23C: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2436 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 289U: _LL23D: _LL23E: {
# 2439
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2441
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2437 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 290U: _LL23F: _LL240: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2438 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 291U: _LL241: _LL242: {
# 2441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2439 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 292U: _LL243: _LL244: {
# 2442
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2444
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpBC6=Cyc_Stmt_tok(({void*_tmpBC5=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));_tmp5EB->tag=13U,({struct _dyneither_ptr*_tmpBC4=({struct _dyneither_ptr*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));({struct _dyneither_ptr _tmpBC3=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5EA=_tmpBC3;});_tmp5EA;});_tmp5EB->f1=_tmpBC4;}),({struct Cyc_Absyn_Stmt*_tmpBC2=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5EB->f2=_tmpBC2;});_tmp5EB;});Cyc_Absyn_new_stmt(_tmpBC5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBC6;});
goto _LL0;}case 293U: _LL245: _LL246: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2449 "parse.y"
({union Cyc_YYSTYPE _tmpBC7=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpBC7;});
goto _LL0;}case 294U: _LL247: _LL248: {
# 2452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2450 "parse.y"
({union Cyc_YYSTYPE _tmpBC9=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBC8=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpBC8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBC9;});
goto _LL0;}case 295U: _LL249: _LL24A: {
# 2453
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2455
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2455 "parse.y"
({union Cyc_YYSTYPE _tmpBCA=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpBCA;});
goto _LL0;}case 296U: _LL24B: _LL24C: {
# 2458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2456 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 297U: _LL24D: _LL24E: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2461 "parse.y"
({union Cyc_YYSTYPE _tmpBCC=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpBCB=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpBCB,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));yyval=_tmpBCC;});
goto _LL0;}case 298U: _LL24F: _LL250: {
# 2464
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2466
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2462 "parse.y"
({union Cyc_YYSTYPE _tmpBCE=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpBCD=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpBCD,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpBCE;});
goto _LL0;}case 299U: _LL251: _LL252: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2463 "parse.y"
({union Cyc_YYSTYPE _tmpBD4=Cyc_Stmt_tok(({void*_tmpBD3=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->tag=13U,({struct _dyneither_ptr*_tmpBD2=({struct _dyneither_ptr*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));({struct _dyneither_ptr _tmpBD1=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5EC=_tmpBD1;});_tmp5EC;});_tmp5ED->f1=_tmpBD2;}),({struct Cyc_Absyn_Stmt*_tmpBD0=({struct Cyc_List_List*_tmpBCF=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpBCF,Cyc_Absyn_skip_stmt(0U));});_tmp5ED->f2=_tmpBD0;});_tmp5ED;});Cyc_Absyn_new_stmt(_tmpBD3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBD4;});
# 2465
goto _LL0;}case 300U: _LL253: _LL254: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2465 "parse.y"
({union Cyc_YYSTYPE _tmpBDA=Cyc_Stmt_tok(({void*_tmpBD9=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->tag=13U,({struct _dyneither_ptr*_tmpBD8=({struct _dyneither_ptr*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));({struct _dyneither_ptr _tmpBD7=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5EE=_tmpBD7;});_tmp5EE;});_tmp5EF->f1=_tmpBD8;}),({struct Cyc_Absyn_Stmt*_tmpBD6=({struct Cyc_List_List*_tmpBD5=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpBD5,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp5EF->f2=_tmpBD6;});_tmp5EF;});Cyc_Absyn_new_stmt(_tmpBD9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBDA;});
# 2467
goto _LL0;}case 301U: _LL255: _LL256: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2467 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 302U: _LL257: _LL258: {
# 2470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2468 "parse.y"
({union Cyc_YYSTYPE _tmpBDD=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpBDC=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpBDB=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpBDC,_tmpBDB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBDD;});
goto _LL0;}case 303U: _LL259: _LL25A: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2469 "parse.y"
({union Cyc_YYSTYPE _tmpBE1=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpBE0=({void*_tmpBDF=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpBDE=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5F0->f1=_tmpBDE;});_tmp5F0;});Cyc_Absyn_new_decl(_tmpBDF,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpBE0,
Cyc_Absyn_skip_stmt(0U));}));
# 2469
yyval=_tmpBE1;});
# 2471
goto _LL0;}case 304U: _LL25B: _LL25C: {
# 2473
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2475
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2472 "parse.y"
({union Cyc_YYSTYPE _tmpBE5=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpBE4=({void*_tmpBE3=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));_tmp5F1->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpBE2=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5F1->f1=_tmpBE2;});_tmp5F1;});Cyc_Absyn_new_decl(_tmpBE3,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpBE4,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpBE5;});
goto _LL0;}case 305U: _LL25D: _LL25E: {
# 2475
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2477
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2477 "parse.y"
({union Cyc_YYSTYPE _tmpBE9=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBE8=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpBE7=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpBE6=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpBE8,_tmpBE7,_tmpBE6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBE9;});
goto _LL0;}case 306U: _LL25F: _LL260: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2479 "parse.y"
({union Cyc_YYSTYPE _tmpBED=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBEC=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpBEB=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpBEA=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpBEC,_tmpBEB,_tmpBEA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBED;});
goto _LL0;}case 307U: _LL261: _LL262: {
# 2482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
({union Cyc_YYSTYPE _tmpBF0=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBEF=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpBEE=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpBEF,_tmpBEE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBF0;});
goto _LL0;}case 308U: _LL263: _LL264: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2488 "parse.y"
struct Cyc_Absyn_Exp*_tmp5F2=({struct _tuple0*_tmpBF1=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpBF1,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpBF4=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBF3=_tmp5F2;struct Cyc_List_List*_tmpBF2=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpBF3,_tmpBF2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBF4;});
goto _LL0;}case 309U: _LL265: _LL266: {
# 2492
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2494
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2492 "parse.y"
struct Cyc_Absyn_Exp*_tmp5F3=({struct Cyc_List_List*_tmpBF5=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpBF5,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpBF8=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBF7=_tmp5F3;struct Cyc_List_List*_tmpBF6=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpBF7,_tmpBF6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBF8;});
# 2495
goto _LL0;}case 310U: _LL267: _LL268: {
# 2497
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2499
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2498 "parse.y"
({union Cyc_YYSTYPE _tmpBFB=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpBFA=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpBF9=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpBFA,_tmpBF9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBFB;});
goto _LL0;}case 311U: _LL269: _LL26A:
# 2501
({union Cyc_YYSTYPE _tmpBFC=Cyc_YY8(0);yyval=_tmpBFC;});
goto _LL0;case 312U: _LL26B: _LL26C: {
# 2504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2516 "parse.y"
({union Cyc_YYSTYPE _tmpC01=Cyc_YY8(({struct Cyc_List_List*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));({struct Cyc_Absyn_Switch_clause*_tmpC00=({struct Cyc_Absyn_Switch_clause*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));({struct Cyc_Absyn_Pat*_tmpBFF=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp5F4->pattern=_tmpBFF;}),_tmp5F4->pat_vars=0,_tmp5F4->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpBFE=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5F4->body=_tmpBFE;}),_tmp5F4->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F4;});
# 2516
_tmp5F5->hd=_tmpC00;}),({
# 2518
struct Cyc_List_List*_tmpBFD=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5F5->tl=_tmpBFD;});_tmp5F5;}));
# 2516
yyval=_tmpC01;});
# 2519
goto _LL0;}case 313U: _LL26D: _LL26E: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2520 "parse.y"
({union Cyc_YYSTYPE _tmpC06=Cyc_YY8(({struct Cyc_List_List*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({struct Cyc_Absyn_Switch_clause*_tmpC05=({struct Cyc_Absyn_Switch_clause*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));({struct Cyc_Absyn_Pat*_tmpC04=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5F6->pattern=_tmpC04;}),_tmp5F6->pat_vars=0,_tmp5F6->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpC03=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp5F6->body=_tmpC03;}),_tmp5F6->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F6;});
# 2520
_tmp5F7->hd=_tmpC05;}),({
# 2522
struct Cyc_List_List*_tmpC02=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5F7->tl=_tmpC02;});_tmp5F7;}));
# 2520
yyval=_tmpC06;});
# 2523
goto _LL0;}case 314U: _LL26F: _LL270: {
# 2525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2524 "parse.y"
({union Cyc_YYSTYPE _tmpC0B=Cyc_YY8(({struct Cyc_List_List*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));({struct Cyc_Absyn_Switch_clause*_tmpC0A=({struct Cyc_Absyn_Switch_clause*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));({struct Cyc_Absyn_Pat*_tmpC09=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5F8->pattern=_tmpC09;}),_tmp5F8->pat_vars=0,_tmp5F8->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpC08=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp5F8->body=_tmpC08;}),_tmp5F8->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F8;});_tmp5F9->hd=_tmpC0A;}),({struct Cyc_List_List*_tmpC07=Cyc_yyget_YY8(yyyyvsp[4]);_tmp5F9->tl=_tmpC07;});_tmp5F9;}));yyval=_tmpC0B;});
goto _LL0;}case 315U: _LL271: _LL272: {
# 2527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2526 "parse.y"
({union Cyc_YYSTYPE _tmpC11=Cyc_YY8(({struct Cyc_List_List*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));({struct Cyc_Absyn_Switch_clause*_tmpC10=({struct Cyc_Absyn_Switch_clause*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({struct Cyc_Absyn_Pat*_tmpC0F=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5FA->pattern=_tmpC0F;}),_tmp5FA->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpC0E=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5FA->where_clause=_tmpC0E;}),({
struct Cyc_Absyn_Stmt*_tmpC0D=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp5FA->body=_tmpC0D;}),_tmp5FA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5FA;});
# 2526
_tmp5FB->hd=_tmpC10;}),({
# 2528
struct Cyc_List_List*_tmpC0C=Cyc_yyget_YY8(yyyyvsp[5]);_tmp5FB->tl=_tmpC0C;});_tmp5FB;}));
# 2526
yyval=_tmpC11;});
# 2529
goto _LL0;}case 316U: _LL273: _LL274: {
# 2531
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2533
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2530 "parse.y"
({union Cyc_YYSTYPE _tmpC17=Cyc_YY8(({struct Cyc_List_List*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));({struct Cyc_Absyn_Switch_clause*_tmpC16=({struct Cyc_Absyn_Switch_clause*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));({struct Cyc_Absyn_Pat*_tmpC15=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5FC->pattern=_tmpC15;}),_tmp5FC->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpC14=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5FC->where_clause=_tmpC14;}),({struct Cyc_Absyn_Stmt*_tmpC13=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp5FC->body=_tmpC13;}),_tmp5FC->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5FC;});_tmp5FD->hd=_tmpC16;}),({struct Cyc_List_List*_tmpC12=Cyc_yyget_YY8(yyyyvsp[6]);_tmp5FD->tl=_tmpC12;});_tmp5FD;}));yyval=_tmpC17;});
goto _LL0;}case 317U: _LL275: _LL276: {
# 2533
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2535
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2537 "parse.y"
({union Cyc_YYSTYPE _tmpC1A=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC19=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpC18=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpC19,_tmpC18,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC1A;});
goto _LL0;}case 318U: _LL277: _LL278: {
# 2540
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2541 "parse.y"
({union Cyc_YYSTYPE _tmpC1D=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpC1C=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpC1B=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpC1C,_tmpC1B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC1D;});
goto _LL0;}case 319U: _LL279: _LL27A: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2545 "parse.y"
({union Cyc_YYSTYPE _tmpC22=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC21=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC20=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC1F=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC1E=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2545
Cyc_Absyn_for_stmt(_tmpC21,_tmpC20,_tmpC1F,_tmpC1E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC22;});
# 2547
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2549
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2551
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2548 "parse.y"
({union Cyc_YYSTYPE _tmpC27=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC26=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC25=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC24=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpC23=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2548
Cyc_Absyn_for_stmt(_tmpC26,_tmpC25,_tmpC24,_tmpC23,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC27;});
# 2550
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2552
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2554
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2551 "parse.y"
({union Cyc_YYSTYPE _tmpC2C=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC2B=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC2A=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC29=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC28=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2551
Cyc_Absyn_for_stmt(_tmpC2B,_tmpC2A,_tmpC29,_tmpC28,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC2C;});
# 2553
goto _LL0;}case 322U: _LL27F: _LL280: {
# 2555
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2557
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2554 "parse.y"
({union Cyc_YYSTYPE _tmpC31=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC30=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC2F=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC2E=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpC2D=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2554
Cyc_Absyn_for_stmt(_tmpC30,_tmpC2F,_tmpC2E,_tmpC2D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC31;});
# 2556
goto _LL0;}case 323U: _LL281: _LL282: {
# 2558
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2560
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2557 "parse.y"
({union Cyc_YYSTYPE _tmpC36=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC35=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC34=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC33=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC32=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2557
Cyc_Absyn_for_stmt(_tmpC35,_tmpC34,_tmpC33,_tmpC32,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC36;});
# 2559
goto _LL0;}case 324U: _LL283: _LL284: {
# 2561
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2563
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2560 "parse.y"
({union Cyc_YYSTYPE _tmpC3B=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC3A=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC39=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC38=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpC37=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2560
Cyc_Absyn_for_stmt(_tmpC3A,_tmpC39,_tmpC38,_tmpC37,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC3B;});
# 2562
goto _LL0;}case 325U: _LL285: _LL286: {
# 2564
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2566
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2563 "parse.y"
({union Cyc_YYSTYPE _tmpC40=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC3F=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC3E=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpC3D=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC3C=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2563
Cyc_Absyn_for_stmt(_tmpC3F,_tmpC3E,_tmpC3D,_tmpC3C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC40;});
# 2565
goto _LL0;}case 326U: _LL287: _LL288: {
# 2567
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 2569
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 2566 "parse.y"
({union Cyc_YYSTYPE _tmpC45=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC44=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC43=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpC42=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpC41=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2566
Cyc_Absyn_for_stmt(_tmpC44,_tmpC43,_tmpC42,_tmpC41,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC45;});
# 2568
goto _LL0;}case 327U: _LL289: _LL28A: {
# 2570
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2572
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2569 "parse.y"
struct Cyc_List_List*_tmp5FE=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5FF=({struct Cyc_Absyn_Exp*_tmpC49=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC48=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC47=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC46=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2570
Cyc_Absyn_for_stmt(_tmpC49,_tmpC48,_tmpC47,_tmpC46,(unsigned int)(yyyylsp[0]).first_line);});
# 2572
({union Cyc_YYSTYPE _tmpC4A=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5FE,_tmp5FF));yyval=_tmpC4A;});
# 2574
goto _LL0;}case 328U: _LL28B: _LL28C: {
# 2576
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2578
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2575 "parse.y"
struct Cyc_List_List*_tmp600=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp601=({struct Cyc_Absyn_Exp*_tmpC4E=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC4D=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC4C=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC4B=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2576
Cyc_Absyn_for_stmt(_tmpC4E,_tmpC4D,_tmpC4C,_tmpC4B,(unsigned int)(yyyylsp[0]).first_line);});
# 2578
({union Cyc_YYSTYPE _tmpC4F=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp600,_tmp601));yyval=_tmpC4F;});
# 2580
goto _LL0;}case 329U: _LL28D: _LL28E: {
# 2582
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2584
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2581 "parse.y"
struct Cyc_List_List*_tmp602=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp603=({struct Cyc_Absyn_Exp*_tmpC53=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC52=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC51=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpC50=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2582
Cyc_Absyn_for_stmt(_tmpC53,_tmpC52,_tmpC51,_tmpC50,(unsigned int)(yyyylsp[0]).first_line);});
# 2584
({union Cyc_YYSTYPE _tmpC54=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp602,_tmp603));yyval=_tmpC54;});
# 2586
goto _LL0;}case 330U: _LL28F: _LL290: {
# 2588
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2590
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2587 "parse.y"
struct Cyc_List_List*_tmp604=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp605=({struct Cyc_Absyn_Exp*_tmpC58=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC57=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC56=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpC55=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2588
Cyc_Absyn_for_stmt(_tmpC58,_tmpC57,_tmpC56,_tmpC55,(unsigned int)(yyyylsp[0]).first_line);});
# 2590
({union Cyc_YYSTYPE _tmpC59=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp604,_tmp605));yyval=_tmpC59;});
# 2592
goto _LL0;}case 331U: _LL291: _LL292: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpC5C=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpC5B=({struct _dyneither_ptr*_tmp606=_cycalloc(sizeof(*_tmp606));({struct _dyneither_ptr _tmpC5A=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp606=_tmpC5A;});_tmp606;});Cyc_Absyn_goto_stmt(_tmpC5B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC5C;});
goto _LL0;}case 332U: _LL293: _LL294: {
# 2600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2598 "parse.y"
({union Cyc_YYSTYPE _tmpC5D=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC5D;});
goto _LL0;}case 333U: _LL295: _LL296: {
# 2601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2599 "parse.y"
({union Cyc_YYSTYPE _tmpC5E=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC5E;});
goto _LL0;}case 334U: _LL297: _LL298: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2600 "parse.y"
({union Cyc_YYSTYPE _tmpC5F=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC5F;});
goto _LL0;}case 335U: _LL299: _LL29A: {
# 2603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2601 "parse.y"
({union Cyc_YYSTYPE _tmpC61=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC60=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpC60,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC61;});
goto _LL0;}case 336U: _LL29B: _LL29C: {
# 2604
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2606
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2603 "parse.y"
({union Cyc_YYSTYPE _tmpC62=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC62;});
goto _LL0;}case 337U: _LL29D: _LL29E: {
# 2606
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2608
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2604 "parse.y"
({union Cyc_YYSTYPE _tmpC63=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC63;});
goto _LL0;}case 338U: _LL29F: _LL2A0: {
# 2607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2606 "parse.y"
({union Cyc_YYSTYPE _tmpC65=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpC64=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpC64,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC65;});
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2615 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2618 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2620 "parse.y"
({union Cyc_YYSTYPE _tmpC69=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC68=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC67=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC66=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpC68,_tmpC67,_tmpC66,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC69;});
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2623 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 343U: _LL2A9: _LL2AA: {
# 2626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2625 "parse.y"
({union Cyc_YYSTYPE _tmpC6C=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC6B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC6A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpC6B,_tmpC6A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC6C;});
goto _LL0;}case 344U: _LL2AB: _LL2AC: {
# 2628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2628 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 345U: _LL2AD: _LL2AE: {
# 2631
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2633
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2630 "parse.y"
({union Cyc_YYSTYPE _tmpC6F=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC6E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC6D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpC6E,_tmpC6D,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC6F;});
goto _LL0;}case 346U: _LL2AF: _LL2B0: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2633 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 347U: _LL2B1: _LL2B2: {
# 2636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2635 "parse.y"
({union Cyc_YYSTYPE _tmpC72=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC71=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC70=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpC71,_tmpC70,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC72;});
goto _LL0;}case 348U: _LL2B3: _LL2B4: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2638 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 349U: _LL2B5: _LL2B6: {
# 2641
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2643
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2640 "parse.y"
({union Cyc_YYSTYPE _tmpC75=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC74=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC73=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpC74,_tmpC73,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC75;});
goto _LL0;}case 350U: _LL2B7: _LL2B8: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2643 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 351U: _LL2B9: _LL2BA: {
# 2646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2645 "parse.y"
({union Cyc_YYSTYPE _tmpC78=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC77=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC76=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpC77,_tmpC76,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC78;});
goto _LL0;}case 352U: _LL2BB: _LL2BC: {
# 2648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2648 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 353U: _LL2BD: _LL2BE: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2650 "parse.y"
({union Cyc_YYSTYPE _tmpC7B=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC7A=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC79=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpC7A,_tmpC79,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC7B;});
goto _LL0;}case 354U: _LL2BF: _LL2C0: {
# 2653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2652 "parse.y"
({union Cyc_YYSTYPE _tmpC7E=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC7D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC7C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpC7D,_tmpC7C,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC7E;});
goto _LL0;}case 355U: _LL2C1: _LL2C2: {
# 2655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2655 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 356U: _LL2C3: _LL2C4: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2657 "parse.y"
({union Cyc_YYSTYPE _tmpC81=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC80=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC7F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpC80,_tmpC7F,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC81;});
goto _LL0;}case 357U: _LL2C5: _LL2C6: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2659 "parse.y"
({union Cyc_YYSTYPE _tmpC84=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC83=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC82=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpC83,_tmpC82,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC84;});
goto _LL0;}case 358U: _LL2C7: _LL2C8: {
# 2662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2661 "parse.y"
({union Cyc_YYSTYPE _tmpC87=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC86=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC85=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpC86,_tmpC85,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC87;});
goto _LL0;}case 359U: _LL2C9: _LL2CA: {
# 2664
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2666
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2663 "parse.y"
({union Cyc_YYSTYPE _tmpC8A=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC89=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC88=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpC89,_tmpC88,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC8A;});
goto _LL0;}case 360U: _LL2CB: _LL2CC: {
# 2666
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2668
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2666 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 361U: _LL2CD: _LL2CE: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2668 "parse.y"
({union Cyc_YYSTYPE _tmpC8D=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC8C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC8B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpC8C,_tmpC8B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC8D;});
goto _LL0;}case 362U: _LL2CF: _LL2D0: {
# 2671
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2673
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2670 "parse.y"
({union Cyc_YYSTYPE _tmpC90=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC8F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC8E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpC8F,_tmpC8E,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC90;});
goto _LL0;}case 363U: _LL2D1: _LL2D2: {
# 2673
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2675
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2673 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 364U: _LL2D3: _LL2D4: {
# 2676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2675 "parse.y"
({union Cyc_YYSTYPE _tmpC93=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC92=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC91=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpC92,_tmpC91,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC93;});
goto _LL0;}case 365U: _LL2D5: _LL2D6: {
# 2678
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2680
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2677 "parse.y"
({union Cyc_YYSTYPE _tmpC96=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC95=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC94=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpC95,_tmpC94,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC96;});
goto _LL0;}case 366U: _LL2D7: _LL2D8: {
# 2680
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2682
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2680 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 367U: _LL2D9: _LL2DA: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2682 "parse.y"
({union Cyc_YYSTYPE _tmpC99=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC98=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC97=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpC98,_tmpC97,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC99;});
goto _LL0;}case 368U: _LL2DB: _LL2DC: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2684 "parse.y"
({union Cyc_YYSTYPE _tmpC9C=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC9B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC9A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpC9B,_tmpC9A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC9C;});
goto _LL0;}case 369U: _LL2DD: _LL2DE: {
# 2687
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2689
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2686 "parse.y"
({union Cyc_YYSTYPE _tmpC9F=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC9E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC9D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpC9E,_tmpC9D,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC9F;});
goto _LL0;}case 370U: _LL2DF: _LL2E0: {
# 2689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2689 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 371U: _LL2E1: _LL2E2: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2691 "parse.y"
void*_tmp607=({struct _tuple8*_tmpCA0=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpCA0,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpCA3=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpCA2=_tmp607;struct Cyc_Absyn_Exp*_tmpCA1=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpCA2,_tmpCA1,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpCA3;});
# 2694
goto _LL0;}case 372U: _LL2E3: _LL2E4: {
# 2696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2697 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 373U: _LL2E5: _LL2E6: {
# 2700
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2702
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2700 "parse.y"
({union Cyc_YYSTYPE _tmpCA4=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpCA4;});
goto _LL0;}case 374U: _LL2E7: _LL2E8: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2702 "parse.y"
({union Cyc_YYSTYPE _tmpCA7=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpCA6=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpCA5=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpCA6,_tmpCA5,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpCA7;});
goto _LL0;}case 375U: _LL2E9: _LL2EA: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2704 "parse.y"
void*_tmp608=({struct _tuple8*_tmpCA8=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpCA8,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpCA9=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp608,(unsigned int)(yyyylsp[0]).first_line)));yyval=_tmpCA9;});
# 2707
goto _LL0;}case 376U: _LL2EB: _LL2EC: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2708 "parse.y"
({union Cyc_YYSTYPE _tmpCAB=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpCAA=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpCAA,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpCAB;});
goto _LL0;}case 377U: _LL2ED: _LL2EE: {
# 2711
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2713
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2710 "parse.y"
({union Cyc_YYSTYPE _tmpCAE=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpCAD=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpCAC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpCAD,_tmpCAC,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpCAE;});
goto _LL0;}case 378U: _LL2EF: _LL2F0: {
# 2713
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2715
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2715 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 379U: _LL2F1: _LL2F2: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2723 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 380U: _LL2F3: _LL2F4: {
# 2726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2728 "parse.y"
({union Cyc_YYSTYPE _tmpCAF=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpCAF;});
goto _LL0;}case 381U: _LL2F5: _LL2F6: {
# 2731
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2733
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2730 "parse.y"
({union Cyc_YYSTYPE _tmpCB0=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpCB0;});
goto _LL0;}case 382U: _LL2F7: _LL2F8: {
# 2733
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2735
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2732 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp609=e->r;void*_tmp60A=_tmp609;struct _dyneither_ptr _tmp61E;int _tmp61D;enum Cyc_Absyn_Sign _tmp61C;int _tmp61B;enum Cyc_Absyn_Sign _tmp61A;short _tmp619;enum Cyc_Absyn_Sign _tmp618;char _tmp617;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).LongLong_c).tag){case 2U: _LL473: _tmp618=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Char_c).val).f1;_tmp617=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Char_c).val).f2;_LL474:
# 2736
({union Cyc_YYSTYPE _tmpCB2=Cyc_YY9(({void*_tmpCB1=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->tag=11U,_tmp60B->f1=_tmp617;_tmp60B;});Cyc_Absyn_new_pat(_tmpCB1,e->loc);}));yyval=_tmpCB2;});goto _LL472;case 4U: _LL475: _tmp61A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Short_c).val).f1;_tmp619=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Short_c).val).f2;_LL476:
# 2738
({union Cyc_YYSTYPE _tmpCB4=Cyc_YY9(({void*_tmpCB3=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->tag=10U,_tmp60C->f1=_tmp61A,_tmp60C->f2=(int)_tmp619;_tmp60C;});Cyc_Absyn_new_pat(_tmpCB3,e->loc);}));yyval=_tmpCB4;});goto _LL472;case 5U: _LL477: _tmp61C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Int_c).val).f1;_tmp61B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Int_c).val).f2;_LL478:
# 2740
({union Cyc_YYSTYPE _tmpCB6=Cyc_YY9(({void*_tmpCB5=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));_tmp60D->tag=10U,_tmp60D->f1=_tmp61C,_tmp60D->f2=_tmp61B;_tmp60D;});Cyc_Absyn_new_pat(_tmpCB5,e->loc);}));yyval=_tmpCB6;});goto _LL472;case 7U: _LL479: _tmp61E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Float_c).val).f1;_tmp61D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp60A)->f1).Float_c).val).f2;_LL47A:
# 2742
({union Cyc_YYSTYPE _tmpCB8=Cyc_YY9(({void*_tmpCB7=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->tag=12U,_tmp60E->f1=_tmp61E,_tmp60E->f2=_tmp61D;_tmp60E;});Cyc_Absyn_new_pat(_tmpCB7,e->loc);}));yyval=_tmpCB8;});goto _LL472;case 1U: _LL47B: _LL47C:
# 2744
({union Cyc_YYSTYPE _tmpCB9=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));yyval=_tmpCB9;});goto _LL472;case 8U: _LL47D: _LL47E:
# 2746
({void*_tmp60F=0U;({unsigned int _tmpCBB=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCBA=({const char*_tmp610="strings cannot occur within patterns";_tag_dyneither(_tmp610,sizeof(char),37U);});Cyc_Warn_err(_tmpCBB,_tmpCBA,_tag_dyneither(_tmp60F,sizeof(void*),0U));});});goto _LL472;case 9U: _LL47F: _LL480:
# 2748
({void*_tmp611=0U;({unsigned int _tmpCBD=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCBC=({const char*_tmp612="strings cannot occur within patterns";_tag_dyneither(_tmp612,sizeof(char),37U);});Cyc_Warn_err(_tmpCBD,_tmpCBC,_tag_dyneither(_tmp611,sizeof(void*),0U));});});goto _LL472;case 6U: _LL481: _LL482:
# 2750
({void*_tmp613=0U;({unsigned int _tmpCBF=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCBE=({const char*_tmp614="long long's in patterns not yet implemented";_tag_dyneither(_tmp614,sizeof(char),44U);});Cyc_Warn_err(_tmpCBF,_tmpCBE,_tag_dyneither(_tmp613,sizeof(void*),0U));});});goto _LL472;default: goto _LL483;}else{_LL483: _LL484:
# 2752
({void*_tmp615=0U;({unsigned int _tmpCC1=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCC0=({const char*_tmp616="bad constant in case";_tag_dyneither(_tmp616,sizeof(char),21U);});Cyc_Warn_err(_tmpCC1,_tmpCC0,_tag_dyneither(_tmp615,sizeof(void*),0U));});});}_LL472:;}
# 2755
goto _LL0;}case 383U: _LL2F9: _LL2FA: {
# 2757
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2756 "parse.y"
({union Cyc_YYSTYPE _tmpCC4=Cyc_YY9(({void*_tmpCC3=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->tag=15U,({struct _tuple0*_tmpCC2=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp61F->f1=_tmpCC2;});_tmp61F;});Cyc_Absyn_new_pat(_tmpCC3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCC4;});
goto _LL0;}case 384U: _LL2FB: _LL2FC: {
# 2759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2758 "parse.y"
if(({struct _dyneither_ptr _tmpCC5=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpCC5,({const char*_tmp620="as";_tag_dyneither(_tmp620,sizeof(char),3U);}));})!= 0)
({void*_tmp621=0U;({unsigned int _tmpCC7=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCC6=({const char*_tmp622="expecting `as'";_tag_dyneither(_tmp622,sizeof(char),15U);});Cyc_Warn_err(_tmpCC7,_tmpCC6,_tag_dyneither(_tmp621,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpCCE=Cyc_YY9(({void*_tmpCCD=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp625=_cycalloc(sizeof(*_tmp625));_tmp625->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpCCC=({unsigned int _tmpCCB=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpCCB,({struct _tuple0*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCCA=({struct _dyneither_ptr*_tmp623=_cycalloc(sizeof(*_tmp623));({struct _dyneither_ptr _tmpCC9=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp623=_tmpCC9;});_tmp623;});_tmp624->f2=_tmpCCA;});_tmp624;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp625->f1=_tmpCCC;}),({
struct Cyc_Absyn_Pat*_tmpCC8=Cyc_yyget_YY9(yyyyvsp[2]);_tmp625->f2=_tmpCC8;});_tmp625;});
# 2760
Cyc_Absyn_new_pat(_tmpCCD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCCE;});
# 2763
goto _LL0;}case 385U: _LL2FD: _LL2FE: {
# 2765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2764 "parse.y"
if(({struct _dyneither_ptr _tmpCCF=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpCCF,({const char*_tmp626="alias";_tag_dyneither(_tmp626,sizeof(char),6U);}));})!= 0)
({void*_tmp627=0U;({unsigned int _tmpCD1=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCD0=({const char*_tmp628="expecting `alias'";_tag_dyneither(_tmp628,sizeof(char),18U);});Cyc_Warn_err(_tmpCD1,_tmpCD0,_tag_dyneither(_tmp627,sizeof(void*),0U));});});{
int _tmp629=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp62F=_cycalloc(sizeof(*_tmp62F));({struct _dyneither_ptr*_tmpCD4=({struct _dyneither_ptr*_tmp62D=_cycalloc(sizeof(*_tmp62D));({struct _dyneither_ptr _tmpCD3=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp62D=_tmpCD3;});_tmp62D;});_tmp62F->name=_tmpCD4;}),_tmp62F->identity=- 1,({void*_tmpCD2=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));_tmp62E->tag=0U,_tmp62E->f1=& Cyc_Tcutil_rk;_tmp62E;});_tmp62F->kind=_tmpCD2;});_tmp62F;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpCD9=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCD8=({struct _tuple0*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCD6=({struct _dyneither_ptr*_tmp62B=_cycalloc(sizeof(*_tmp62B));({struct _dyneither_ptr _tmpCD5=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp62B=_tmpCD5;});_tmp62B;});_tmp62C->f2=_tmpCD6;});_tmp62C;});Cyc_Absyn_new_vardecl(_tmpCD9,_tmpCD8,({
struct _tuple8*_tmpCD7=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpCD7,(unsigned int)(yyyylsp[4]).first_line);}),0);});
({union Cyc_YYSTYPE _tmpCDB=Cyc_YY9(({void*_tmpCDA=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp62A=_cycalloc(sizeof(*_tmp62A));_tmp62A->tag=2U,_tmp62A->f1=tv,_tmp62A->f2=vd;_tmp62A;});Cyc_Absyn_new_pat(_tmpCDA,(unsigned int)_tmp629);}));yyval=_tmpCDB;});
# 2772
goto _LL0;};}case 386U: _LL2FF: _LL300: {
# 2774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2773 "parse.y"
if(({struct _dyneither_ptr _tmpCDC=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpCDC,({const char*_tmp630="alias";_tag_dyneither(_tmp630,sizeof(char),6U);}));})!= 0)
({void*_tmp631=0U;({unsigned int _tmpCDE=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCDD=({const char*_tmp632="expecting `alias'";_tag_dyneither(_tmp632,sizeof(char),18U);});Cyc_Warn_err(_tmpCDE,_tmpCDD,_tag_dyneither(_tmp631,sizeof(void*),0U));});});{
int _tmp633=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp639=_cycalloc(sizeof(*_tmp639));({struct _dyneither_ptr*_tmpCE1=({struct _dyneither_ptr*_tmp637=_cycalloc(sizeof(*_tmp637));({struct _dyneither_ptr _tmpCE0=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp637=_tmpCE0;});_tmp637;});_tmp639->name=_tmpCE1;}),_tmp639->identity=- 1,({void*_tmpCDF=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp638=_cycalloc(sizeof(*_tmp638));_tmp638->tag=0U,_tmp638->f1=& Cyc_Tcutil_rk;_tmp638;});_tmp639->kind=_tmpCDF;});_tmp639;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpCE6=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCE5=({struct _tuple0*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCE3=({struct _dyneither_ptr*_tmp635=_cycalloc(sizeof(*_tmp635));({struct _dyneither_ptr _tmpCE2=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp635=_tmpCE2;});_tmp635;});_tmp636->f2=_tmpCE3;});_tmp636;});Cyc_Absyn_new_vardecl(_tmpCE6,_tmpCE5,({
struct _tuple8*_tmpCE4=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpCE4,(unsigned int)(yyyylsp[4]).first_line);}),0);});
({union Cyc_YYSTYPE _tmpCE8=Cyc_YY9(({void*_tmpCE7=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=2U,_tmp634->f1=tv,_tmp634->f2=vd;_tmp634;});Cyc_Absyn_new_pat(_tmpCE7,(unsigned int)_tmp633);}));yyval=_tmpCE8;});
# 2781
goto _LL0;};}case 387U: _LL301: _LL302: {
# 2783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2782 "parse.y"
struct _tuple22 _tmp63A=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp63B=_tmp63A;struct Cyc_List_List*_tmp63E;int _tmp63D;_LL486: _tmp63E=_tmp63B.f1;_tmp63D=_tmp63B.f2;_LL487:;
({union Cyc_YYSTYPE _tmpCEA=Cyc_YY9(({void*_tmpCE9=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C->tag=5U,_tmp63C->f1=_tmp63E,_tmp63C->f2=_tmp63D;_tmp63C;});Cyc_Absyn_new_pat(_tmpCE9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCEA;});
# 2785
goto _LL0;}case 388U: _LL303: _LL304: {
# 2787
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2789
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2786 "parse.y"
struct _tuple22 _tmp63F=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp640=_tmp63F;struct Cyc_List_List*_tmp643;int _tmp642;_LL489: _tmp643=_tmp640.f1;_tmp642=_tmp640.f2;_LL48A:;
({union Cyc_YYSTYPE _tmpCED=Cyc_YY9(({void*_tmpCEC=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->tag=16U,({struct _tuple0*_tmpCEB=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp641->f1=_tmpCEB;}),_tmp641->f2=_tmp643,_tmp641->f3=_tmp642;_tmp641;});Cyc_Absyn_new_pat(_tmpCEC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCED;});
# 2789
goto _LL0;}case 389U: _LL305: _LL306: {
# 2791
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2790 "parse.y"
struct _tuple22 _tmp644=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp645=_tmp644;struct Cyc_List_List*_tmp64A;int _tmp649;_LL48C: _tmp64A=_tmp645.f1;_tmp649=_tmp645.f2;_LL48D:;{
struct Cyc_List_List*_tmp646=({unsigned int _tmpCEE=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpCEE,Cyc_yyget_YY40(yyyyvsp[2]));});
({union Cyc_YYSTYPE _tmpCF2=Cyc_YY9(({void*_tmpCF1=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->tag=7U,({struct Cyc_Absyn_AggrInfo*_tmpCF0=({struct Cyc_Absyn_AggrInfo*_tmp647=_cycalloc(sizeof(*_tmp647));({union Cyc_Absyn_AggrInfoU _tmpCEF=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);_tmp647->aggr_info=_tmpCEF;}),_tmp647->targs=0;_tmp647;});_tmp648->f1=_tmpCF0;}),_tmp648->f2=_tmp646,_tmp648->f3=_tmp64A,_tmp648->f4=_tmp649;_tmp648;});Cyc_Absyn_new_pat(_tmpCF1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCF2;});
# 2795
goto _LL0;};}case 390U: _LL307: _LL308: {
# 2797
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2799
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2796 "parse.y"
struct _tuple22 _tmp64B=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp64C=_tmp64B;struct Cyc_List_List*_tmp650;int _tmp64F;_LL48F: _tmp650=_tmp64C.f1;_tmp64F=_tmp64C.f2;_LL490:;{
struct Cyc_List_List*_tmp64D=({unsigned int _tmpCF3=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpCF3,Cyc_yyget_YY40(yyyyvsp[1]));});
({union Cyc_YYSTYPE _tmpCF5=Cyc_YY9(({void*_tmpCF4=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->tag=7U,_tmp64E->f1=0,_tmp64E->f2=_tmp64D,_tmp64E->f3=_tmp650,_tmp64E->f4=_tmp64F;_tmp64E;});Cyc_Absyn_new_pat(_tmpCF4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCF5;});
# 2800
goto _LL0;};}case 391U: _LL309: _LL30A: {
# 2802
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2804
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2801 "parse.y"
({union Cyc_YYSTYPE _tmpCF8=Cyc_YY9(({void*_tmpCF7=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp651=_cycalloc(sizeof(*_tmp651));_tmp651->tag=6U,({struct Cyc_Absyn_Pat*_tmpCF6=Cyc_yyget_YY9(yyyyvsp[1]);_tmp651->f1=_tmpCF6;});_tmp651;});Cyc_Absyn_new_pat(_tmpCF7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCF8;});
goto _LL0;}case 392U: _LL30B: _LL30C: {
# 2804
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2806
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2803 "parse.y"
({union Cyc_YYSTYPE _tmpCFD=Cyc_YY9(({void*_tmpCFC=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp653=_cycalloc(sizeof(*_tmp653));_tmp653->tag=6U,({struct Cyc_Absyn_Pat*_tmpCFB=({void*_tmpCFA=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->tag=6U,({struct Cyc_Absyn_Pat*_tmpCF9=Cyc_yyget_YY9(yyyyvsp[1]);_tmp652->f1=_tmpCF9;});_tmp652;});Cyc_Absyn_new_pat(_tmpCFA,(unsigned int)(yyyylsp[0]).first_line);});_tmp653->f1=_tmpCFB;});_tmp653;});Cyc_Absyn_new_pat(_tmpCFC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCFD;});
goto _LL0;}case 393U: _LL30D: _LL30E: {
# 2806
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2808
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2805 "parse.y"
({union Cyc_YYSTYPE _tmpD04=Cyc_YY9(({void*_tmpD03=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpD02=({unsigned int _tmpD01=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpD01,({struct _tuple0*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpD00=({struct _dyneither_ptr*_tmp654=_cycalloc(sizeof(*_tmp654));({struct _dyneither_ptr _tmpCFF=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp654=_tmpCFF;});_tmp654;});_tmp655->f2=_tmpD00;});_tmp655;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp656->f1=_tmpD02;}),({
# 2807
struct Cyc_Absyn_Pat*_tmpCFE=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp656->f2=_tmpCFE;});_tmp656;});
# 2805
Cyc_Absyn_new_pat(_tmpD03,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD04;});
# 2809
goto _LL0;}case 394U: _LL30F: _LL310: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2810 "parse.y"
if(({struct _dyneither_ptr _tmpD05=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpD05,({const char*_tmp657="as";_tag_dyneither(_tmp657,sizeof(char),3U);}));})!= 0)
({void*_tmp658=0U;({unsigned int _tmpD07=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpD06=({const char*_tmp659="expecting `as'";_tag_dyneither(_tmp659,sizeof(char),15U);});Cyc_Warn_err(_tmpD07,_tmpD06,_tag_dyneither(_tmp658,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpD0E=Cyc_YY9(({void*_tmpD0D=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpD0C=({unsigned int _tmpD0B=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpD0B,({struct _tuple0*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpD0A=({struct _dyneither_ptr*_tmp65A=_cycalloc(sizeof(*_tmp65A));({struct _dyneither_ptr _tmpD09=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp65A=_tmpD09;});_tmp65A;});_tmp65B->f2=_tmpD0A;});_tmp65B;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp65C->f1=_tmpD0C;}),({
# 2814
struct Cyc_Absyn_Pat*_tmpD08=Cyc_yyget_YY9(yyyyvsp[3]);_tmp65C->f2=_tmpD08;});_tmp65C;});
# 2812
Cyc_Absyn_new_pat(_tmpD0D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD0E;});
# 2816
goto _LL0;}case 395U: _LL311: _LL312: {
# 2818
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2820
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2817 "parse.y"
void*_tmp65D=({struct _dyneither_ptr _tmpD0F=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpD0F,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
({union Cyc_YYSTYPE _tmpD17=Cyc_YY9(({void*_tmpD16=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=4U,({struct Cyc_Absyn_Tvar*_tmpD15=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp65D);_tmp661->f1=_tmpD15;}),({
struct Cyc_Absyn_Vardecl*_tmpD14=({unsigned int _tmpD13=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpD12=({struct _tuple0*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpD11=({struct _dyneither_ptr*_tmp65E=_cycalloc(sizeof(*_tmp65E));({struct _dyneither_ptr _tmpD10=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp65E=_tmpD10;});_tmp65E;});_tmp65F->f2=_tmpD11;});_tmp65F;});Cyc_Absyn_new_vardecl(_tmpD13,_tmpD12,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp660=_cycalloc(sizeof(*_tmp660));
_tmp660->tag=19U,_tmp660->f1=_tmp65D;_tmp660;}),0);});
# 2819
_tmp661->f2=_tmpD14;});_tmp661;});
# 2818
Cyc_Absyn_new_pat(_tmpD16,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD17;});
# 2822
goto _LL0;}case 396U: _LL313: _LL314: {
# 2824
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2826
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2823 "parse.y"
struct Cyc_Absyn_Tvar*_tmp662=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
({union Cyc_YYSTYPE _tmpD1F=Cyc_YY9(({void*_tmpD1E=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->tag=4U,_tmp667->f1=_tmp662,({
struct Cyc_Absyn_Vardecl*_tmpD1D=({unsigned int _tmpD1C=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpD1B=({struct _tuple0*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpD19=({struct _dyneither_ptr*_tmp663=_cycalloc(sizeof(*_tmp663));({struct _dyneither_ptr _tmpD18=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp663=_tmpD18;});_tmp663;});_tmp664->f2=_tmpD19;});_tmp664;});Cyc_Absyn_new_vardecl(_tmpD1C,_tmpD1B,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp666=_cycalloc(sizeof(*_tmp666));
_tmp666->tag=19U,({void*_tmpD1A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp665=_cycalloc(sizeof(*_tmp665));_tmp665->tag=2U,_tmp665->f1=_tmp662;_tmp665;});_tmp666->f1=_tmpD1A;});_tmp666;}),0);});
# 2825
_tmp667->f2=_tmpD1D;});_tmp667;});
# 2824
Cyc_Absyn_new_pat(_tmpD1E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD1F;});
# 2828
goto _LL0;}case 397U: _LL315: _LL316: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2831 "parse.y"
({union Cyc_YYSTYPE _tmpD21=Cyc_YY10(({struct _tuple22*_tmp668=_cycalloc(sizeof(*_tmp668));({struct Cyc_List_List*_tmpD20=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp668->f1=_tmpD20;}),_tmp668->f2=0;_tmp668;}));yyval=_tmpD21;});
goto _LL0;}case 398U: _LL317: _LL318: {
# 2834
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2836
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2832 "parse.y"
({union Cyc_YYSTYPE _tmpD23=Cyc_YY10(({struct _tuple22*_tmp669=_cycalloc(sizeof(*_tmp669));({struct Cyc_List_List*_tmpD22=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp669->f1=_tmpD22;}),_tmp669->f2=1;_tmp669;}));yyval=_tmpD23;});
goto _LL0;}case 399U: _LL319: _LL31A: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2833 "parse.y"
({union Cyc_YYSTYPE _tmpD24=Cyc_YY10(({struct _tuple22*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->f1=0,_tmp66A->f2=1;_tmp66A;}));yyval=_tmpD24;});
goto _LL0;}case 400U: _LL31B: _LL31C: {
# 2836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2838 "parse.y"
({union Cyc_YYSTYPE _tmpD26=Cyc_YY11(({struct Cyc_List_List*_tmp66B=_cycalloc(sizeof(*_tmp66B));({struct Cyc_Absyn_Pat*_tmpD25=Cyc_yyget_YY9(yyyyvsp[0]);_tmp66B->hd=_tmpD25;}),_tmp66B->tl=0;_tmp66B;}));yyval=_tmpD26;});
goto _LL0;}case 401U: _LL31D: _LL31E: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2840 "parse.y"
({union Cyc_YYSTYPE _tmpD29=Cyc_YY11(({struct Cyc_List_List*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct Cyc_Absyn_Pat*_tmpD28=Cyc_yyget_YY9(yyyyvsp[2]);_tmp66C->hd=_tmpD28;}),({struct Cyc_List_List*_tmpD27=Cyc_yyget_YY11(yyyyvsp[0]);_tmp66C->tl=_tmpD27;});_tmp66C;}));yyval=_tmpD29;});
goto _LL0;}case 402U: _LL31F: _LL320: {
# 2843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2845 "parse.y"
({union Cyc_YYSTYPE _tmpD2B=Cyc_YY12(({struct _tuple23*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->f1=0,({struct Cyc_Absyn_Pat*_tmpD2A=Cyc_yyget_YY9(yyyyvsp[0]);_tmp66D->f2=_tmpD2A;});_tmp66D;}));yyval=_tmpD2B;});
goto _LL0;}case 403U: _LL321: _LL322: {
# 2848
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2850
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2847 "parse.y"
({union Cyc_YYSTYPE _tmpD2E=Cyc_YY12(({struct _tuple23*_tmp66E=_cycalloc(sizeof(*_tmp66E));({struct Cyc_List_List*_tmpD2D=Cyc_yyget_YY41(yyyyvsp[0]);_tmp66E->f1=_tmpD2D;}),({struct Cyc_Absyn_Pat*_tmpD2C=Cyc_yyget_YY9(yyyyvsp[1]);_tmp66E->f2=_tmpD2C;});_tmp66E;}));yyval=_tmpD2E;});
goto _LL0;}case 404U: _LL323: _LL324: {
# 2850
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2852
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2850 "parse.y"
({union Cyc_YYSTYPE _tmpD30=Cyc_YY14(({struct _tuple22*_tmp66F=_cycalloc(sizeof(*_tmp66F));({struct Cyc_List_List*_tmpD2F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp66F->f1=_tmpD2F;}),_tmp66F->f2=0;_tmp66F;}));yyval=_tmpD30;});
goto _LL0;}case 405U: _LL325: _LL326: {
# 2853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2851 "parse.y"
({union Cyc_YYSTYPE _tmpD32=Cyc_YY14(({struct _tuple22*_tmp670=_cycalloc(sizeof(*_tmp670));({struct Cyc_List_List*_tmpD31=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp670->f1=_tmpD31;}),_tmp670->f2=1;_tmp670;}));yyval=_tmpD32;});
goto _LL0;}case 406U: _LL327: _LL328: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2852 "parse.y"
({union Cyc_YYSTYPE _tmpD33=Cyc_YY14(({struct _tuple22*_tmp671=_cycalloc(sizeof(*_tmp671));_tmp671->f1=0,_tmp671->f2=1;_tmp671;}));yyval=_tmpD33;});
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2857 "parse.y"
({union Cyc_YYSTYPE _tmpD35=Cyc_YY13(({struct Cyc_List_List*_tmp672=_cycalloc(sizeof(*_tmp672));({struct _tuple23*_tmpD34=Cyc_yyget_YY12(yyyyvsp[0]);_tmp672->hd=_tmpD34;}),_tmp672->tl=0;_tmp672;}));yyval=_tmpD35;});
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2859 "parse.y"
({union Cyc_YYSTYPE _tmpD38=Cyc_YY13(({struct Cyc_List_List*_tmp673=_cycalloc(sizeof(*_tmp673));({struct _tuple23*_tmpD37=Cyc_yyget_YY12(yyyyvsp[2]);_tmp673->hd=_tmpD37;}),({struct Cyc_List_List*_tmpD36=Cyc_yyget_YY13(yyyyvsp[0]);_tmp673->tl=_tmpD36;});_tmp673;}));yyval=_tmpD38;});
goto _LL0;}case 409U: _LL32D: _LL32E: {
# 2862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 410U: _LL32F: _LL330: {
# 2868
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2870
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2867 "parse.y"
({union Cyc_YYSTYPE _tmpD3B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD3A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD39=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpD3A,_tmpD39,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD3B;});
goto _LL0;}case 411U: _LL331: _LL332: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2872 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 412U: _LL333: _LL334: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2874 "parse.y"
({union Cyc_YYSTYPE _tmpD3F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD3E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpD3D=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpD3C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpD3E,_tmpD3D,_tmpD3C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD3F;});
goto _LL0;}case 413U: _LL335: _LL336: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2876 "parse.y"
({union Cyc_YYSTYPE _tmpD42=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD41=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD40=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpD41,_tmpD40,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD42;});
goto _LL0;}case 414U: _LL337: _LL338: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2880 "parse.y"
({union Cyc_YYSTYPE _tmpD43=Cyc_YY7(0);yyval=_tmpD43;});
goto _LL0;}case 415U: _LL339: _LL33A: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2881 "parse.y"
({union Cyc_YYSTYPE _tmpD44=Cyc_YY7(({struct Cyc_Core_Opt*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->v=(void*)Cyc_Absyn_Times;_tmp674;}));yyval=_tmpD44;});
goto _LL0;}case 416U: _LL33B: _LL33C: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2882 "parse.y"
({union Cyc_YYSTYPE _tmpD45=Cyc_YY7(({struct Cyc_Core_Opt*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->v=(void*)Cyc_Absyn_Div;_tmp675;}));yyval=_tmpD45;});
goto _LL0;}case 417U: _LL33D: _LL33E: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2883 "parse.y"
({union Cyc_YYSTYPE _tmpD46=Cyc_YY7(({struct Cyc_Core_Opt*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->v=(void*)Cyc_Absyn_Mod;_tmp676;}));yyval=_tmpD46;});
goto _LL0;}case 418U: _LL33F: _LL340: {
# 2886
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2888
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2884 "parse.y"
({union Cyc_YYSTYPE _tmpD47=Cyc_YY7(({struct Cyc_Core_Opt*_tmp677=_cycalloc(sizeof(*_tmp677));_tmp677->v=(void*)Cyc_Absyn_Plus;_tmp677;}));yyval=_tmpD47;});
goto _LL0;}case 419U: _LL341: _LL342: {
# 2887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2885 "parse.y"
({union Cyc_YYSTYPE _tmpD48=Cyc_YY7(({struct Cyc_Core_Opt*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678->v=(void*)Cyc_Absyn_Minus;_tmp678;}));yyval=_tmpD48;});
goto _LL0;}case 420U: _LL343: _LL344: {
# 2888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2886 "parse.y"
({union Cyc_YYSTYPE _tmpD49=Cyc_YY7(({struct Cyc_Core_Opt*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->v=(void*)Cyc_Absyn_Bitlshift;_tmp679;}));yyval=_tmpD49;});
goto _LL0;}case 421U: _LL345: _LL346: {
# 2889
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2891
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2887 "parse.y"
({union Cyc_YYSTYPE _tmpD4A=Cyc_YY7(({struct Cyc_Core_Opt*_tmp67A=_cycalloc(sizeof(*_tmp67A));_tmp67A->v=(void*)Cyc_Absyn_Bitlrshift;_tmp67A;}));yyval=_tmpD4A;});
goto _LL0;}case 422U: _LL347: _LL348: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2888 "parse.y"
({union Cyc_YYSTYPE _tmpD4B=Cyc_YY7(({struct Cyc_Core_Opt*_tmp67B=_cycalloc(sizeof(*_tmp67B));_tmp67B->v=(void*)Cyc_Absyn_Bitand;_tmp67B;}));yyval=_tmpD4B;});
goto _LL0;}case 423U: _LL349: _LL34A: {
# 2891
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2893
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2889 "parse.y"
({union Cyc_YYSTYPE _tmpD4C=Cyc_YY7(({struct Cyc_Core_Opt*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->v=(void*)Cyc_Absyn_Bitxor;_tmp67C;}));yyval=_tmpD4C;});
goto _LL0;}case 424U: _LL34B: _LL34C: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2890 "parse.y"
({union Cyc_YYSTYPE _tmpD4D=Cyc_YY7(({struct Cyc_Core_Opt*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->v=(void*)Cyc_Absyn_Bitor;_tmp67D;}));yyval=_tmpD4D;});
goto _LL0;}case 425U: _LL34D: _LL34E: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2895 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 426U: _LL34F: _LL350: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2897 "parse.y"
({union Cyc_YYSTYPE _tmpD51=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD50=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD4F=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD4E=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpD50,_tmpD4F,_tmpD4E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD51;});
goto _LL0;}case 427U: _LL351: _LL352: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2900 "parse.y"
({union Cyc_YYSTYPE _tmpD53=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD52=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpD52,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD53;});
goto _LL0;}case 428U: _LL353: _LL354: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2903 "parse.y"
({union Cyc_YYSTYPE _tmpD55=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD54=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpD54,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD55;});
goto _LL0;}case 429U: _LL355: _LL356: {
# 2906
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2908
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2905 "parse.y"
({union Cyc_YYSTYPE _tmpD57=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD56=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpD56,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD57;});
goto _LL0;}case 430U: _LL357: _LL358: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2907 "parse.y"
({union Cyc_YYSTYPE _tmpD5A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD59=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD58=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpD59,_tmpD58,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5A;});
goto _LL0;}case 431U: _LL359: _LL35A: {
# 2910
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2909 "parse.y"
({union Cyc_YYSTYPE _tmpD5D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD5C=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD5B=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpD5C,_tmpD5B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5D;});
goto _LL0;}case 432U: _LL35B: _LL35C: {
# 2912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2913 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 433U: _LL35D: _LL35E: {
# 2916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2917 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 434U: _LL35F: _LL360: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2919 "parse.y"
({union Cyc_YYSTYPE _tmpD60=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD5F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD5E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpD5F,_tmpD5E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD60;});
goto _LL0;}case 435U: _LL361: _LL362: {
# 2922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2923 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 436U: _LL363: _LL364: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2925 "parse.y"
({union Cyc_YYSTYPE _tmpD63=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD62=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD61=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpD62,_tmpD61,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD63;});
goto _LL0;}case 437U: _LL365: _LL366: {
# 2928
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2930
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2929 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 438U: _LL367: _LL368: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2931 "parse.y"
({union Cyc_YYSTYPE _tmpD66=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD65=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD64=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpD65,_tmpD64,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD66;});
goto _LL0;}case 439U: _LL369: _LL36A: {
# 2934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2935 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 440U: _LL36B: _LL36C: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2937 "parse.y"
({union Cyc_YYSTYPE _tmpD69=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD68=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD67=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpD68,_tmpD67,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD69;});
goto _LL0;}case 441U: _LL36D: _LL36E: {
# 2940
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2942
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2941 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 442U: _LL36F: _LL370: {
# 2944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2943 "parse.y"
({union Cyc_YYSTYPE _tmpD6C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD6B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD6A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpD6B,_tmpD6A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6C;});
goto _LL0;}case 443U: _LL371: _LL372: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2947 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 444U: _LL373: _LL374: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2949 "parse.y"
({union Cyc_YYSTYPE _tmpD6F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD6E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD6D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpD6E,_tmpD6D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6F;});
goto _LL0;}case 445U: _LL375: _LL376: {
# 2952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2951 "parse.y"
({union Cyc_YYSTYPE _tmpD72=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD71=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD70=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpD71,_tmpD70,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD72;});
goto _LL0;}case 446U: _LL377: _LL378: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2955 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 447U: _LL379: _LL37A: {
# 2958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2957 "parse.y"
({union Cyc_YYSTYPE _tmpD75=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD74=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD73=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpD74,_tmpD73,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD75;});
goto _LL0;}case 448U: _LL37B: _LL37C: {
# 2960
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2962
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2959 "parse.y"
({union Cyc_YYSTYPE _tmpD78=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD77=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD76=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpD77,_tmpD76,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD78;});
goto _LL0;}case 449U: _LL37D: _LL37E: {
# 2962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2961 "parse.y"
({union Cyc_YYSTYPE _tmpD7B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD7A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD79=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpD7A,_tmpD79,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7B;});
goto _LL0;}case 450U: _LL37F: _LL380: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2963 "parse.y"
({union Cyc_YYSTYPE _tmpD7E=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD7D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD7C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpD7D,_tmpD7C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7E;});
goto _LL0;}case 451U: _LL381: _LL382: {
# 2966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2967 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 452U: _LL383: _LL384: {
# 2970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2969 "parse.y"
({union Cyc_YYSTYPE _tmpD81=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD80=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD7F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpD80,_tmpD7F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD81;});
goto _LL0;}case 453U: _LL385: _LL386: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2971 "parse.y"
({union Cyc_YYSTYPE _tmpD84=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD83=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD82=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpD83,_tmpD82,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD84;});
goto _LL0;}case 454U: _LL387: _LL388: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2975 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 455U: _LL389: _LL38A: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2977 "parse.y"
({union Cyc_YYSTYPE _tmpD87=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD86=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD85=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpD86,_tmpD85,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD87;});
goto _LL0;}case 456U: _LL38B: _LL38C: {
# 2980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2979 "parse.y"
({union Cyc_YYSTYPE _tmpD8A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD89=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD88=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpD89,_tmpD88,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD8A;});
goto _LL0;}case 457U: _LL38D: _LL38E: {
# 2982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2983 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 458U: _LL38F: _LL390: {
# 2986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2985 "parse.y"
({union Cyc_YYSTYPE _tmpD8D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD8C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD8B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpD8C,_tmpD8B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD8D;});
goto _LL0;}case 459U: _LL391: _LL392: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2987 "parse.y"
({union Cyc_YYSTYPE _tmpD90=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD8F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD8E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpD8F,_tmpD8E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD90;});
goto _LL0;}case 460U: _LL393: _LL394: {
# 2990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2989 "parse.y"
({union Cyc_YYSTYPE _tmpD93=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD92=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD91=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpD92,_tmpD91,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD93;});
goto _LL0;}case 461U: _LL395: _LL396: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2993 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 462U: _LL397: _LL398: {
# 2996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2995 "parse.y"
void*_tmp67E=({struct _tuple8*_tmpD94=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpD94,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpD97=Cyc_Exp_tok(({void*_tmpD96=_tmp67E;struct Cyc_Absyn_Exp*_tmpD95=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpD96,_tmpD95,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD97;});
# 2998
goto _LL0;}case 463U: _LL399: _LL39A: {
# 3000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3001 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 464U: _LL39B: _LL39C: {
# 3004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3002 "parse.y"
({union Cyc_YYSTYPE _tmpD99=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD98=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpD98,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD99;});
goto _LL0;}case 465U: _LL39D: _LL39E: {
# 3005
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3007
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3003 "parse.y"
({union Cyc_YYSTYPE _tmpD9B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD9A=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpD9A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9B;});
goto _LL0;}case 466U: _LL39F: _LL3A0: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3004 "parse.y"
({union Cyc_YYSTYPE _tmpD9D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD9C=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpD9C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9D;});
goto _LL0;}case 467U: _LL3A1: _LL3A2: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3005 "parse.y"
({union Cyc_YYSTYPE _tmpD9F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD9E=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpD9E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9F;});
goto _LL0;}case 468U: _LL3A3: _LL3A4: {
# 3008
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3010
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3006 "parse.y"
({union Cyc_YYSTYPE _tmpDA1=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDA0=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpDA0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA1;});
goto _LL0;}case 469U: _LL3A5: _LL3A6: {
# 3009
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3011
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3007 "parse.y"
({union Cyc_YYSTYPE _tmpDA4=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpDA3=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpDA2=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpDA3,_tmpDA2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA4;});
goto _LL0;}case 470U: _LL3A7: _LL3A8: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3009 "parse.y"
void*_tmp67F=({struct _tuple8*_tmpDA5=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpDA5,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpDA6=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp67F,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDA6;});
# 3012
goto _LL0;}case 471U: _LL3A9: _LL3AA: {
# 3014
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3016
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3012 "parse.y"
({union Cyc_YYSTYPE _tmpDA8=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDA7=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpDA7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA8;});
goto _LL0;}case 472U: _LL3AB: _LL3AC: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3014 "parse.y"
void*_tmp680=({struct _tuple8*_tmpDA9=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpDA9,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpDAC=Cyc_Exp_tok(({void*_tmpDAB=_tmp680;struct Cyc_List_List*_tmpDAA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpDAB,_tmpDAA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDAC;});
# 3017
goto _LL0;}case 473U: _LL3AD: _LL3AE: {
# 3019
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3021
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3019 "parse.y"
({union Cyc_YYSTYPE _tmpDAF=Cyc_Exp_tok(({void*_tmpDAE=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681->tag=34U,(_tmp681->f1).is_calloc=0,(_tmp681->f1).rgn=0,(_tmp681->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpDAD=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp681->f1).num_elts=_tmpDAD;}),(_tmp681->f1).fat_result=0,(_tmp681->f1).inline_call=0;_tmp681;});Cyc_Absyn_new_exp(_tmpDAE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDAF;});
# 3021
goto _LL0;}case 474U: _LL3AF: _LL3B0: {
# 3023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3022 "parse.y"
({union Cyc_YYSTYPE _tmpDB3=Cyc_Exp_tok(({void*_tmpDB2=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp682=_cycalloc(sizeof(*_tmp682));_tmp682->tag=34U,(_tmp682->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpDB1=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp682->f1).rgn=_tmpDB1;}),(_tmp682->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpDB0=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp682->f1).num_elts=_tmpDB0;}),(_tmp682->f1).fat_result=0,(_tmp682->f1).inline_call=0;_tmp682;});Cyc_Absyn_new_exp(_tmpDB2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDB3;});
# 3024
goto _LL0;}case 475U: _LL3B1: _LL3B2: {
# 3026
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3028
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3025 "parse.y"
({union Cyc_YYSTYPE _tmpDB7=Cyc_Exp_tok(({void*_tmpDB6=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->tag=34U,(_tmp683->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpDB5=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp683->f1).rgn=_tmpDB5;}),(_tmp683->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpDB4=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp683->f1).num_elts=_tmpDB4;}),(_tmp683->f1).fat_result=0,(_tmp683->f1).inline_call=1;_tmp683;});Cyc_Absyn_new_exp(_tmpDB6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDB7;});
# 3027
goto _LL0;}case 476U: _LL3B3: _LL3B4: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 3028 "parse.y"
void*_tmp684=({struct _tuple8*_tmpDB8=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpDB8,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpDBC=Cyc_Exp_tok(({void*_tmpDBB=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp686=_cycalloc(sizeof(*_tmp686));_tmp686->tag=34U,(_tmp686->f1).is_calloc=1,(_tmp686->f1).rgn=0,({void**_tmpDBA=({void**_tmp685=_cycalloc(sizeof(*_tmp685));*_tmp685=_tmp684;_tmp685;});(_tmp686->f1).elt_type=_tmpDBA;}),({struct Cyc_Absyn_Exp*_tmpDB9=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp686->f1).num_elts=_tmpDB9;}),(_tmp686->f1).fat_result=0,(_tmp686->f1).inline_call=0;_tmp686;});Cyc_Absyn_new_exp(_tmpDBB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBC;});
# 3031
goto _LL0;}case 477U: _LL3B5: _LL3B6: {
# 3033
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11U));
# 3035
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11U));
# 3033 "parse.y"
void*_tmp687=({struct _tuple8*_tmpDBD=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpDBD,(unsigned int)(yyyylsp[8]).first_line);});
({union Cyc_YYSTYPE _tmpDC2=Cyc_Exp_tok(({void*_tmpDC1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->tag=34U,(_tmp689->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpDC0=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp689->f1).rgn=_tmpDC0;}),({void**_tmpDBF=({void**_tmp688=_cycalloc(sizeof(*_tmp688));*_tmp688=_tmp687;_tmp688;});(_tmp689->f1).elt_type=_tmpDBF;}),({struct Cyc_Absyn_Exp*_tmpDBE=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp689->f1).num_elts=_tmpDBE;}),(_tmp689->f1).fat_result=0,(_tmp689->f1).inline_call=0;_tmp689;});Cyc_Absyn_new_exp(_tmpDC1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC2;});
# 3036
goto _LL0;}case 478U: _LL3B7: _LL3B8: {
# 3038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3037 "parse.y"
({union Cyc_YYSTYPE _tmpDC5=Cyc_Exp_tok(({struct Cyc_List_List*_tmpDC4=({struct Cyc_Absyn_Exp*_tmp68A[1U];({struct Cyc_Absyn_Exp*_tmpDC3=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp68A[0]=_tmpDC3;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp68A,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpDC4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC5;});
goto _LL0;}case 479U: _LL3B9: _LL3BA: {
# 3040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3039 "parse.y"
({union Cyc_YYSTYPE _tmpDCA=Cyc_Exp_tok(({void*_tmpDC9=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C->tag=38U,({struct Cyc_Absyn_Exp*_tmpDC8=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp68C->f1=_tmpDC8;}),({struct _dyneither_ptr*_tmpDC7=({struct _dyneither_ptr*_tmp68B=_cycalloc(sizeof(*_tmp68B));({struct _dyneither_ptr _tmpDC6=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp68B=_tmpDC6;});_tmp68B;});_tmp68C->f2=_tmpDC7;});_tmp68C;});Cyc_Absyn_new_exp(_tmpDC9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDCA;});
goto _LL0;}case 480U: _LL3BB: _LL3BC: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3041 "parse.y"
({union Cyc_YYSTYPE _tmpDD0=Cyc_Exp_tok(({void*_tmpDCF=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->tag=38U,({struct Cyc_Absyn_Exp*_tmpDCE=({struct Cyc_Absyn_Exp*_tmpDCD=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpDCD,(unsigned int)(yyyylsp[2]).first_line);});_tmp68E->f1=_tmpDCE;}),({struct _dyneither_ptr*_tmpDCC=({struct _dyneither_ptr*_tmp68D=_cycalloc(sizeof(*_tmp68D));({struct _dyneither_ptr _tmpDCB=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp68D=_tmpDCB;});_tmp68D;});_tmp68E->f2=_tmpDCC;});_tmp68E;});Cyc_Absyn_new_exp(_tmpDCF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDD0;});
goto _LL0;}case 481U: _LL3BD: _LL3BE: {
# 3044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3043 "parse.y"
void*_tmp68F=({struct _tuple8*_tmpDD1=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpDD1,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpDD2=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp68F,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDD2;});
goto _LL0;}case 482U: _LL3BF: _LL3C0: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3046 "parse.y"
struct _tuple20 _tmp690=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp691=_tmp690;int _tmp693;struct _dyneither_ptr _tmp692;_LL492: _tmp693=_tmp691.f1;_tmp692=_tmp691.f2;_LL493:;
({union Cyc_YYSTYPE _tmpDD3=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp693,_tmp692,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDD3;});
goto _LL0;}case 483U: _LL3C1: _LL3C2: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3048 "parse.y"
({union Cyc_YYSTYPE _tmpDD5=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDD4=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_extension_exp(_tmpDD4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDD5;});
goto _LL0;}case 484U: _LL3C3: _LL3C4: {
# 3051
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3053
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3052 "parse.y"
({union Cyc_YYSTYPE _tmpDD6=Cyc_YY6(Cyc_Absyn_Bitnot);yyval=_tmpDD6;});
goto _LL0;}case 485U: _LL3C5: _LL3C6: {
# 3055
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3057
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3053 "parse.y"
({union Cyc_YYSTYPE _tmpDD7=Cyc_YY6(Cyc_Absyn_Not);yyval=_tmpDD7;});
goto _LL0;}case 486U: _LL3C7: _LL3C8: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3054 "parse.y"
({union Cyc_YYSTYPE _tmpDD8=Cyc_YY6(Cyc_Absyn_Minus);yyval=_tmpDD8;});
goto _LL0;}case 487U: _LL3C9: _LL3CA: {
# 3057
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3059
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3059 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3061 "parse.y"
({union Cyc_YYSTYPE _tmpDDB=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDDA=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpDD9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpDDA,_tmpDD9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDDB;});
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3063 "parse.y"
({union Cyc_YYSTYPE _tmpDDD=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDDC=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpDDC,0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDDD;});
goto _LL0;}case 490U: _LL3CF: _LL3D0: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3065 "parse.y"
({union Cyc_YYSTYPE _tmpDE0=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDDF=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpDDE=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpDDF,_tmpDDE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDE0;});
goto _LL0;}case 491U: _LL3D1: _LL3D2: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3067 "parse.y"
({union Cyc_YYSTYPE _tmpDE4=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDE3=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpDE2=({struct _dyneither_ptr*_tmp694=_cycalloc(sizeof(*_tmp694));({struct _dyneither_ptr _tmpDE1=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp694=_tmpDE1;});_tmp694;});Cyc_Absyn_aggrmember_exp(_tmpDE3,_tmpDE2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDE4;});
goto _LL0;}case 492U: _LL3D3: _LL3D4: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3069 "parse.y"
({union Cyc_YYSTYPE _tmpDE8=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDE7=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpDE6=({struct _dyneither_ptr*_tmp695=_cycalloc(sizeof(*_tmp695));({struct _dyneither_ptr _tmpDE5=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp695=_tmpDE5;});_tmp695;});Cyc_Absyn_aggrarrow_exp(_tmpDE7,_tmpDE6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDE8;});
goto _LL0;}case 493U: _LL3D5: _LL3D6: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3071 "parse.y"
({union Cyc_YYSTYPE _tmpDEA=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDE9=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpDE9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDEA;});
goto _LL0;}case 494U: _LL3D7: _LL3D8: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3073 "parse.y"
({union Cyc_YYSTYPE _tmpDEC=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDEB=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpDEB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDEC;});
goto _LL0;}case 495U: _LL3D9: _LL3DA: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3075 "parse.y"
({union Cyc_YYSTYPE _tmpDEF=Cyc_Exp_tok(({void*_tmpDEE=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->tag=25U,({struct _tuple8*_tmpDED=Cyc_yyget_YY37(yyyyvsp[1]);_tmp696->f1=_tmpDED;}),_tmp696->f2=0;_tmp696;});Cyc_Absyn_new_exp(_tmpDEE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDEF;});
goto _LL0;}case 496U: _LL3DB: _LL3DC: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3077 "parse.y"
({union Cyc_YYSTYPE _tmpDF3=Cyc_Exp_tok(({void*_tmpDF2=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->tag=25U,({struct _tuple8*_tmpDF1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp697->f1=_tmpDF1;}),({struct Cyc_List_List*_tmpDF0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp697->f2=_tmpDF0;});_tmp697;});Cyc_Absyn_new_exp(_tmpDF2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDF3;});
goto _LL0;}case 497U: _LL3DD: _LL3DE: {
# 3080
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 3082
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 3079 "parse.y"
({union Cyc_YYSTYPE _tmpDF7=Cyc_Exp_tok(({void*_tmpDF6=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698->tag=25U,({struct _tuple8*_tmpDF5=Cyc_yyget_YY37(yyyyvsp[1]);_tmp698->f1=_tmpDF5;}),({struct Cyc_List_List*_tmpDF4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp698->f2=_tmpDF4;});_tmp698;});Cyc_Absyn_new_exp(_tmpDF6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDF7;});
goto _LL0;}case 498U: _LL3DF: _LL3E0: {
# 3082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3084 "parse.y"
({union Cyc_YYSTYPE _tmpDFB=Cyc_YY3(({struct Cyc_List_List*_tmp69B=_cycalloc(sizeof(*_tmp69B));({void*_tmpDFA=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->tag=0U,({struct _dyneither_ptr*_tmpDF9=({struct _dyneither_ptr*_tmp699=_cycalloc(sizeof(*_tmp699));({struct _dyneither_ptr _tmpDF8=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp699=_tmpDF8;});_tmp699;});_tmp69A->f1=_tmpDF9;});_tmp69A;});_tmp69B->hd=_tmpDFA;}),_tmp69B->tl=0;_tmp69B;}));yyval=_tmpDFB;});
goto _LL0;}case 499U: _LL3E1: _LL3E2: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3087 "parse.y"
({union Cyc_YYSTYPE _tmpDFF=Cyc_YY3(({struct Cyc_List_List*_tmp69D=_cycalloc(sizeof(*_tmp69D));({void*_tmpDFE=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp69C=_cycalloc(sizeof(*_tmp69C));_tmp69C->tag=1U,({unsigned int _tmpDFD=({unsigned int _tmpDFC=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpDFC,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp69C->f1=_tmpDFD;});_tmp69C;});_tmp69D->hd=_tmpDFE;}),_tmp69D->tl=0;_tmp69D;}));yyval=_tmpDFF;});
goto _LL0;}case 500U: _LL3E3: _LL3E4: {
# 3090
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3092
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3089 "parse.y"
({union Cyc_YYSTYPE _tmpE04=Cyc_YY3(({struct Cyc_List_List*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));({void*_tmpE03=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp69F=_cycalloc(sizeof(*_tmp69F));_tmp69F->tag=0U,({struct _dyneither_ptr*_tmpE02=({struct _dyneither_ptr*_tmp69E=_cycalloc(sizeof(*_tmp69E));({struct _dyneither_ptr _tmpE01=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp69E=_tmpE01;});_tmp69E;});_tmp69F->f1=_tmpE02;});_tmp69F;});_tmp6A0->hd=_tmpE03;}),({struct Cyc_List_List*_tmpE00=Cyc_yyget_YY3(yyyyvsp[0]);_tmp6A0->tl=_tmpE00;});_tmp6A0;}));yyval=_tmpE04;});
goto _LL0;}case 501U: _LL3E5: _LL3E6: {
# 3092
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3094
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3092 "parse.y"
({union Cyc_YYSTYPE _tmpE09=Cyc_YY3(({struct Cyc_List_List*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));({void*_tmpE08=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1->tag=1U,({unsigned int _tmpE07=({unsigned int _tmpE06=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpE06,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp6A1->f1=_tmpE07;});_tmp6A1;});_tmp6A2->hd=_tmpE08;}),({struct Cyc_List_List*_tmpE05=Cyc_yyget_YY3(yyyyvsp[0]);_tmp6A2->tl=_tmpE05;});_tmp6A2;}));yyval=_tmpE09;});
goto _LL0;}case 502U: _LL3E7: _LL3E8: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpE0B=Cyc_Exp_tok(({struct _tuple0*_tmpE0A=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpE0A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE0B;});
goto _LL0;}case 503U: _LL3E9: _LL3EA: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3100 "parse.y"
({union Cyc_YYSTYPE _tmpE0D=Cyc_Exp_tok(({struct _dyneither_ptr _tmpE0C=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Absyn_pragma_exp(_tmpE0C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE0D;});
goto _LL0;}case 504U: _LL3EB: _LL3EC: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3102 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 505U: _LL3ED: _LL3EE: {
# 3105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3104 "parse.y"
({union Cyc_YYSTYPE _tmpE0F=Cyc_Exp_tok(({struct _dyneither_ptr _tmpE0E=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpE0E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE0F;});
goto _LL0;}case 506U: _LL3EF: _LL3F0: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3106 "parse.y"
({union Cyc_YYSTYPE _tmpE11=Cyc_Exp_tok(({struct _dyneither_ptr _tmpE10=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpE10,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE11;});
goto _LL0;}case 507U: _LL3F1: _LL3F2: {
# 3109
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3111
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3108 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 508U: _LL3F3: _LL3F4: {
# 3111
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3113
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3113 "parse.y"
({union Cyc_YYSTYPE _tmpE13=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpE12=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpE12,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE13;});
goto _LL0;}case 509U: _LL3F5: _LL3F6: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3115 "parse.y"
({union Cyc_YYSTYPE _tmpE16=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpE15=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpE14=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpE15,_tmpE14,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE16;});
goto _LL0;}case 510U: _LL3F7: _LL3F8: {
# 3118
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3120
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3118 "parse.y"
({union Cyc_YYSTYPE _tmpE18=Cyc_Exp_tok(({struct Cyc_List_List*_tmpE17=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpE17,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE18;});
goto _LL0;}case 511U: _LL3F9: _LL3FA: {
# 3121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3121 "parse.y"
({union Cyc_YYSTYPE _tmpE1D=Cyc_Exp_tok(({void*_tmpE1C=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));_tmp6A3->tag=29U,({struct _tuple0*_tmpE1B=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp6A3->f1=_tmpE1B;}),({struct Cyc_List_List*_tmpE1A=Cyc_yyget_YY40(yyyyvsp[2]);_tmp6A3->f2=_tmpE1A;}),({struct Cyc_List_List*_tmpE19=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp6A3->f3=_tmpE19;}),_tmp6A3->f4=0;_tmp6A3;});Cyc_Absyn_new_exp(_tmpE1C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE1D;});
goto _LL0;}case 512U: _LL3FB: _LL3FC: {
# 3124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3124 "parse.y"
({union Cyc_YYSTYPE _tmpE1F=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpE1E=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpE1E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE1F;});
goto _LL0;}case 513U: _LL3FD: _LL3FE: {
# 3127
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3129
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3128 "parse.y"
({union Cyc_YYSTYPE _tmpE20=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));yyval=_tmpE20;});
goto _LL0;}case 514U: _LL3FF: _LL400: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpE22=Cyc_YY4(({struct Cyc_List_List*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));({struct Cyc_Absyn_Exp*_tmpE21=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp6A4->hd=_tmpE21;}),_tmp6A4->tl=0;_tmp6A4;}));yyval=_tmpE22;});
goto _LL0;}case 515U: _LL401: _LL402: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3136 "parse.y"
({union Cyc_YYSTYPE _tmpE25=Cyc_YY4(({struct Cyc_List_List*_tmp6A5=_cycalloc(sizeof(*_tmp6A5));({struct Cyc_Absyn_Exp*_tmpE24=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp6A5->hd=_tmpE24;}),({struct Cyc_List_List*_tmpE23=Cyc_yyget_YY4(yyyyvsp[0]);_tmp6A5->tl=_tmpE23;});_tmp6A5;}));yyval=_tmpE25;});
goto _LL0;}case 516U: _LL403: _LL404: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpE27=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpE26=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpE26,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE27;});
goto _LL0;}case 517U: _LL405: _LL406: {
# 3145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3143 "parse.y"
({union Cyc_YYSTYPE _tmpE29=Cyc_Exp_tok(({char _tmpE28=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpE28,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE29;});
goto _LL0;}case 518U: _LL407: _LL408: {
# 3146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3144 "parse.y"
({union Cyc_YYSTYPE _tmpE2B=Cyc_Exp_tok(({struct _dyneither_ptr _tmpE2A=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpE2A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE2B;});
goto _LL0;}case 519U: _LL409: _LL40A: {
# 3147
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3149
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3146 "parse.y"
struct _dyneither_ptr _tmp6A6=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp6A6);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp6A6,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3154
({union Cyc_YYSTYPE _tmpE2C=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp6A6,i,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpE2C;});
# 3156
goto _LL0;}case 520U: _LL40B: _LL40C: {
# 3158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3157 "parse.y"
({union Cyc_YYSTYPE _tmpE2D=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpE2D;});
goto _LL0;}case 521U: _LL40D: _LL40E: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3161 "parse.y"
({union Cyc_YYSTYPE _tmpE31=Cyc_QualId_tok(({struct _tuple0*_tmp6A8=_cycalloc(sizeof(*_tmp6A8));({union Cyc_Absyn_Nmspace _tmpE30=Cyc_Absyn_Rel_n(0);_tmp6A8->f1=_tmpE30;}),({struct _dyneither_ptr*_tmpE2F=({struct _dyneither_ptr*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));({struct _dyneither_ptr _tmpE2E=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp6A7=_tmpE2E;});_tmp6A7;});_tmp6A8->f2=_tmpE2F;});_tmp6A8;}));yyval=_tmpE31;});
goto _LL0;}case 522U: _LL40F: _LL410: {
# 3164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3162 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 523U: _LL411: _LL412: {
# 3165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3165 "parse.y"
({union Cyc_YYSTYPE _tmpE35=Cyc_QualId_tok(({struct _tuple0*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));({union Cyc_Absyn_Nmspace _tmpE34=Cyc_Absyn_Rel_n(0);_tmp6AA->f1=_tmpE34;}),({struct _dyneither_ptr*_tmpE33=({struct _dyneither_ptr*_tmp6A9=_cycalloc(sizeof(*_tmp6A9));({struct _dyneither_ptr _tmpE32=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp6A9=_tmpE32;});_tmp6A9;});_tmp6AA->f2=_tmpE33;});_tmp6AA;}));yyval=_tmpE35;});
goto _LL0;}case 524U: _LL413: _LL414: {
# 3168
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3170
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3166 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 525U: _LL415: _LL416: {
# 3169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3171 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 526U: _LL417: _LL418: {
# 3174
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3176
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3172 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 527U: _LL419: _LL41A: {
# 3175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3175 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 528U: _LL41B: _LL41C: {
# 3178
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3180
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3176 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 529U: _LL41D: _LL41E: {
# 3179
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3181
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3181 "parse.y"
goto _LL0;}case 530U: _LL41F: _LL420: {
# 3183
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3185
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3181 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL0;}default: _LL421: _LL422:
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
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).first_column=yylloc.first_column;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_line=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_line;
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1))).last_column;}else{
# 405
({int _tmpE36=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpE36;});
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
({int _tmpE38=({short _tmpE37=Cyc_yypgoto[_check_known_subscript_notnull(147U,yyn - 148)];_tmpE37 + *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});yystate=_tmpE38;});
if((yystate >= 0  && yystate <= 7704) && Cyc_yycheck[_check_known_subscript_notnull(7705U,yystate)]== ((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 421
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(147U,yyn - 148)];}
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
if(yyn > - 32768  && yyn < 7704){
# 437
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 441
count=0;
# 443
for(x=yyn < 0?- yyn: 0;x < 295U / sizeof(char*);++ x){
# 445
if(Cyc_yycheck[_check_known_subscript_notnull(7705U,x + yyn)]== x)
({unsigned long _tmpE39=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(295U,x)])+ 15;sze +=_tmpE39;}),count ++;}
({struct _dyneither_ptr _tmpE3B=({unsigned int _tmp6AC=(unsigned int)(sze + 15)+ 1U;char*_tmp6AB=({struct _RegionHandle*_tmpE3A=yyregion;_region_malloc(_tmpE3A,_check_times(_tmp6AC,sizeof(char)));});({{unsigned int _tmp765=(unsigned int)(sze + 15);unsigned int i;for(i=0;i < _tmp765;++ i){_tmp6AB[i]='\000';}_tmp6AB[_tmp765]=0;}0;});_tag_dyneither(_tmp6AB,sizeof(char),_tmp6AC);});msg=_tmpE3B;});
({struct _dyneither_ptr _tmpE3C=msg;Cyc_strcpy(_tmpE3C,({const char*_tmp6AD="parse error";_tag_dyneither(_tmp6AD,sizeof(char),12U);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;x < 295U / sizeof(char*);++ x){
# 455
if(Cyc_yycheck[_check_known_subscript_notnull(7705U,x + yyn)]== x){
# 457
({struct _dyneither_ptr _tmpE3D=msg;Cyc_strcat(_tmpE3D,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp6AE=", expecting `";_tag_dyneither(_tmp6AE,sizeof(char),14U);}):(struct _dyneither_ptr)({const char*_tmp6AF=" or `";_tag_dyneither(_tmp6AF,sizeof(char),6U);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(295U,x)]);
({struct _dyneither_ptr _tmpE3E=msg;Cyc_strcat(_tmpE3E,({const char*_tmp6B0="'";_tag_dyneither(_tmp6B0,sizeof(char),2U);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpE40=({const char*_tmp6B1="parse error";_tag_dyneither(_tmp6B1,sizeof(char),12U);});int _tmpE3F=yystate;Cyc_yyerror(_tmpE40,_tmpE3F,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp6B2=1;_npop_handler(0U);return _tmp6B2;}
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
 if(yyssp_offset == 0){int _tmp6B3=1;_npop_handler(0U);return _tmp6B3;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1086U,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7704) || Cyc_yycheck[_check_known_subscript_notnull(7705U,yyn)]!= 1)goto yyerrdefault;
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
if(yyn == 1085){
int _tmp6B4=0;_npop_handler(0U);return _tmp6B4;}
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
union Cyc_YYSTYPE _tmp6BB=v;struct Cyc_Absyn_Stmt*_tmp6D5;struct Cyc_Absyn_Exp*_tmp6D4;struct _tuple0*_tmp6D3;struct _dyneither_ptr _tmp6D2;char _tmp6D1;union Cyc_Absyn_Cnst _tmp6D0;switch((_tmp6BB.Stmt_tok).tag){case 1U: _LL1: _tmp6D0=(_tmp6BB.Int_tok).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp6BE=({struct Cyc_String_pa_PrintArg_struct _tmp769;_tmp769.tag=0U,({struct _dyneither_ptr _tmpE41=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp6D0));_tmp769.f1=_tmpE41;});_tmp769;});void*_tmp6BC[1U];_tmp6BC[0]=& _tmp6BE;({struct Cyc___cycFILE*_tmpE43=Cyc_stderr;struct _dyneither_ptr _tmpE42=({const char*_tmp6BD="%s";_tag_dyneither(_tmp6BD,sizeof(char),3U);});Cyc_fprintf(_tmpE43,_tmpE42,_tag_dyneither(_tmp6BC,sizeof(void*),1U));});});goto _LL0;case 2U: _LL3: _tmp6D1=(_tmp6BB.Char_tok).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp6C1=({struct Cyc_Int_pa_PrintArg_struct _tmp76A;_tmp76A.tag=1U,_tmp76A.f1=(unsigned long)((int)_tmp6D1);_tmp76A;});void*_tmp6BF[1U];_tmp6BF[0]=& _tmp6C1;({struct Cyc___cycFILE*_tmpE45=Cyc_stderr;struct _dyneither_ptr _tmpE44=({const char*_tmp6C0="%c";_tag_dyneither(_tmp6C0,sizeof(char),3U);});Cyc_fprintf(_tmpE45,_tmpE44,_tag_dyneither(_tmp6BF,sizeof(void*),1U));});});goto _LL0;case 3U: _LL5: _tmp6D2=(_tmp6BB.String_tok).val;_LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp6C4=({struct Cyc_String_pa_PrintArg_struct _tmp76B;_tmp76B.tag=0U,_tmp76B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6D2);_tmp76B;});void*_tmp6C2[1U];_tmp6C2[0]=& _tmp6C4;({struct Cyc___cycFILE*_tmpE47=Cyc_stderr;struct _dyneither_ptr _tmpE46=({const char*_tmp6C3="\"%s\"";_tag_dyneither(_tmp6C3,sizeof(char),5U);});Cyc_fprintf(_tmpE47,_tmpE46,_tag_dyneither(_tmp6C2,sizeof(void*),1U));});});goto _LL0;case 5U: _LL7: _tmp6D3=(_tmp6BB.QualId_tok).val;_LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp6C7=({struct Cyc_String_pa_PrintArg_struct _tmp76C;_tmp76C.tag=0U,({struct _dyneither_ptr _tmpE48=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6D3));_tmp76C.f1=_tmpE48;});_tmp76C;});void*_tmp6C5[1U];_tmp6C5[0]=& _tmp6C7;({struct Cyc___cycFILE*_tmpE4A=Cyc_stderr;struct _dyneither_ptr _tmpE49=({const char*_tmp6C6="%s";_tag_dyneither(_tmp6C6,sizeof(char),3U);});Cyc_fprintf(_tmpE4A,_tmpE49,_tag_dyneither(_tmp6C5,sizeof(void*),1U));});});goto _LL0;case 7U: _LL9: _tmp6D4=(_tmp6BB.Exp_tok).val;_LLA:
({struct Cyc_String_pa_PrintArg_struct _tmp6CA=({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0U,({struct _dyneither_ptr _tmpE4B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp6D4));_tmp76D.f1=_tmpE4B;});_tmp76D;});void*_tmp6C8[1U];_tmp6C8[0]=& _tmp6CA;({struct Cyc___cycFILE*_tmpE4D=Cyc_stderr;struct _dyneither_ptr _tmpE4C=({const char*_tmp6C9="%s";_tag_dyneither(_tmp6C9,sizeof(char),3U);});Cyc_fprintf(_tmpE4D,_tmpE4C,_tag_dyneither(_tmp6C8,sizeof(void*),1U));});});goto _LL0;case 8U: _LLB: _tmp6D5=(_tmp6BB.Stmt_tok).val;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp6CD=({struct Cyc_String_pa_PrintArg_struct _tmp76E;_tmp76E.tag=0U,({struct _dyneither_ptr _tmpE4E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp6D5));_tmp76E.f1=_tmpE4E;});_tmp76E;});void*_tmp6CB[1U];_tmp6CB[0]=& _tmp6CD;({struct Cyc___cycFILE*_tmpE50=Cyc_stderr;struct _dyneither_ptr _tmpE4F=({const char*_tmp6CC="%s";_tag_dyneither(_tmp6CC,sizeof(char),3U);});Cyc_fprintf(_tmpE50,_tmpE4F,_tag_dyneither(_tmp6CB,sizeof(void*),1U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp6CE=0U;({struct Cyc___cycFILE*_tmpE52=Cyc_stderr;struct _dyneither_ptr _tmpE51=({const char*_tmp6CF="?";_tag_dyneither(_tmp6CF,sizeof(char),2U);});Cyc_fprintf(_tmpE52,_tmpE51,_tag_dyneither(_tmp6CE,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3196
struct _dyneither_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6D6="end-of-file";_tag_dyneither(_tmp6D6,sizeof(char),12U);});
# 3200
if(token == 363)
return Cyc_Lex_token_string;else{
if(token == 372)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0  && token <= 375?(int)Cyc_yytranslate[_check_known_subscript_notnull(376U,token)]: 295;
if((unsigned int)z < 295U)
return Cyc_yytname[z];else{
return _tag_dyneither(0,0,0);}};}
# 3211
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6D7=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6D7;_push_region(yyr);
({struct _RegionHandle*_tmpE53=yyr;Cyc_yyparse(_tmpE53,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6D8=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6D8;};
# 3214
;_pop_region(yyr);};}
