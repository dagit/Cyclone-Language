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
struct _RegionPage; // abstract -- defined in runtime_memory.c

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
#endif

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
#define _cycalloc(n)            (GC_malloc(n)          ? : _throw_badalloc())
#define _cycalloc_atomic(n)     (GC_malloc_atomic(n)   ? : _throw_badalloc())
#define _cyccalloc(n,s)         (GC_calloc(n,s)        ? : _throw_badalloc())
#define _cyccalloc_atomic(n,s)  (GC_calloc_atomic(n,s) ? : _throw_badalloc())

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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 894
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 921
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 944
void*Cyc_Absyn_compress_kb(void*);
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 951
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_typ(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 956
extern void*Cyc_Absyn_sint_typ;
# 958
void*Cyc_Absyn_float_typ(int);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_false_exp(unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_char_exp(char c,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wchar_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_wstring_exp(struct _dyneither_ptr s,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_unknownid_exp(struct _tuple0*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_gte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_lte_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_post_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_post_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_dec_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_unknowncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1052
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
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_while_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_break_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_continue_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_switch_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_fallthru_stmt(struct Cyc_List_List*el,unsigned int loc);
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_do_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
# 1097
struct Cyc_Absyn_Stmt*Cyc_Absyn_trycatch_stmt(struct Cyc_Absyn_Stmt*,struct Cyc_List_List*,unsigned int);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
struct Cyc_Absyn_Pat*Cyc_Absyn_exp_pat(struct Cyc_Absyn_Exp*);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_letv_decl(struct Cyc_List_List*,unsigned int loc);
struct Cyc_Absyn_Decl*Cyc_Absyn_region_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*open_exp,unsigned int);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple0*x,void*t,struct Cyc_Absyn_Exp*init);
# 1111
struct Cyc_Absyn_AggrdeclImpl*Cyc_Absyn_aggrdecl_impl(struct Cyc_List_List*exists,struct Cyc_List_List*po,struct Cyc_List_List*fs,int tagged);
# 1118
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_aggr_tdecl(enum Cyc_Absyn_AggrKind k,enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Absyn_AggrdeclImpl*i,struct Cyc_List_List*atts,unsigned int loc);
# 1127
struct Cyc_Absyn_Decl*Cyc_Absyn_datatype_decl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1132
struct Cyc_Absyn_TypeDecl*Cyc_Absyn_datatype_tdecl(enum Cyc_Absyn_Scope s,struct _tuple0*n,struct Cyc_List_List*ts,struct Cyc_Core_Opt*fs,int is_extensible,unsigned int loc);
# 1138
void*Cyc_Absyn_function_typ(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_typ,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
# 1163
int Cyc_Absyn_fntype_att(void*);
# 1181
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
({struct _tuple10*_tmp762=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators,_tmp2->hd=_tmp5;_tmp2;});declarators=_tmp762;});
({struct Cyc_List_List*_tmp763=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4,_tmp3->tl=exprs;_tmp3;});exprs=_tmp763;});}
# 177
({struct Cyc_List_List*_tmp764=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp764;});
({struct _tuple10*_tmp765=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp765;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
({void*_tmp8=0U;({unsigned int _tmp767=loc;struct _dyneither_ptr _tmp766=({const char*_tmp9="qualifier on type is ignored";_tag_dyneither(_tmp9,sizeof(char),29U);});Cyc_Warn_warn(_tmp767,_tmp766,_tag_dyneither(_tmp8,sizeof(void*),0U));});});}
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
({union Cyc_Absyn_Constraint*_tmp768=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->tag=1U,_tmpE->f1=_tmpF;_tmpE;}));bound=_tmp768;});goto _LL0;default: _LLF: _tmp10=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10:
 rgn=_tmp10;goto _LL0;}_LL0:;}
# 230
return({struct _tuple14 _tmp6CC;_tmp6CC.f1=nullable,_tmp6CC.f2=bound,_tmp6CC.f3=zeroterm,_tmp6CC.f4=rgn;_tmp6CC;});}
# 236
struct _tuple0*Cyc_Parse_gensym_enum(){
# 238
static int enum_counter=0;
return({struct _tuple0*_tmp15=_cycalloc(sizeof(*_tmp15));({union Cyc_Absyn_Nmspace _tmp76C=Cyc_Absyn_Rel_n(0);_tmp15->f1=_tmp76C;}),({
struct _dyneither_ptr*_tmp76B=({struct _dyneither_ptr*_tmp14=_cycalloc(sizeof(*_tmp14));({struct _dyneither_ptr _tmp76A=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp13=({struct Cyc_Int_pa_PrintArg_struct _tmp6CD;_tmp6CD.tag=1U,_tmp6CD.f1=(unsigned long)enum_counter ++;_tmp6CD;});void*_tmp11[1U];_tmp11[0]=& _tmp13;({struct _dyneither_ptr _tmp769=({const char*_tmp12="__anonymous_enum_%d__";_tag_dyneither(_tmp12,sizeof(char),22U);});Cyc_aprintf(_tmp769,_tag_dyneither(_tmp11,sizeof(void*),1U));});});*_tmp14=_tmp76A;});_tmp14;});_tmp15->f2=_tmp76B;});_tmp15;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 243
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 248
struct _tuple17*_tmp16=field_info;unsigned int _tmp23;struct _tuple0*_tmp22;struct Cyc_Absyn_Tqual _tmp21;void*_tmp20;struct Cyc_List_List*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;_LL1: _tmp23=(_tmp16->f1)->f1;_tmp22=(_tmp16->f1)->f2;_tmp21=(_tmp16->f1)->f3;_tmp20=(_tmp16->f1)->f4;_tmp1F=(_tmp16->f1)->f5;_tmp1E=(_tmp16->f1)->f6;_tmp1D=(_tmp16->f2)->f1;_tmp1C=(_tmp16->f2)->f2;_LL2:;
if(_tmp1F != 0)
({void*_tmp17=0U;({unsigned int _tmp76E=loc;struct _dyneither_ptr _tmp76D=({const char*_tmp18="bad type params in struct field";_tag_dyneither(_tmp18,sizeof(char),32U);});Cyc_Warn_err(_tmp76E,_tmp76D,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp22))
({void*_tmp19=0U;({unsigned int _tmp770=loc;struct _dyneither_ptr _tmp76F=({const char*_tmp1A="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp1A,sizeof(char),59U);});Cyc_Warn_err(_tmp770,_tmp76F,_tag_dyneither(_tmp19,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->name=(*_tmp22).f2,_tmp1B->tq=_tmp21,_tmp1B->type=_tmp20,_tmp1B->width=_tmp1D,_tmp1B->attributes=_tmp1E,_tmp1B->requires_clause=_tmp1C;_tmp1B;});}
# 258
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp6CE;_tmp6CE.Signed_spec=0,_tmp6CE.Unsigned_spec=0,_tmp6CE.Short_spec=0,_tmp6CE.Long_spec=0,_tmp6CE.Long_Long_spec=0,_tmp6CE.Valid_type_spec=0,_tmp6CE.Type_spec=Cyc_Absyn_sint_typ,_tmp6CE.loc=loc;_tmp6CE;});}
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
void*_tmp771=t;Cyc_Tcutil_promote_array(_tmp771,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0):(void*)& Cyc_Absyn_HeapRgn_val,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 312 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp29=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp2A=(struct _tuple8*)x->hd;struct _tuple8*_tmp2B=_tmp2A;struct _dyneither_ptr _tmp41;void**_tmp40;struct _dyneither_ptr*_tmp3F;void*_tmp3E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)((struct _tuple8*)_tmp2B)->f3)->tag == 19U){_LL1: _tmp3F=_tmp2B->f1;_tmp3E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B->f3)->f1;if(_tmp3F != 0){_LL2:
# 317
{void*_tmp2C=_tmp3E;void**_tmp34;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C)->tag == 1U){_LL8: _tmp34=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C)->f2;_LL9: {
# 321
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp33=_cycalloc(sizeof(*_tmp33));({struct _dyneither_ptr _tmp773=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp6CF;_tmp6CF.tag=0U,_tmp6CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3F);_tmp6CF;});void*_tmp30[1U];_tmp30[0]=& _tmp32;({struct _dyneither_ptr _tmp772=({const char*_tmp31="`%s";_tag_dyneither(_tmp31,sizeof(char),4U);});Cyc_aprintf(_tmp772,_tag_dyneither(_tmp30,sizeof(void*),1U));});});*_tmp33=_tmp773;});_tmp33;});
({void*_tmp776=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->tag=2U,({struct Cyc_Absyn_Tvar*_tmp775=({struct Cyc_Absyn_Tvar*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E->name=nm,_tmp2E->identity=- 1,({void*_tmp774=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->tag=0U,_tmp2D->f1=& Cyc_Tcutil_ik;_tmp2D;});_tmp2E->kind=_tmp774;});_tmp2E;});_tmp2F->f1=_tmp775;});_tmp2F;});*_tmp34=_tmp776;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 326
({struct Cyc_List_List*_tmp778=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _tuple18*_tmp777=({struct _tuple18*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->f1=_tmp3F,_tmp35->f2=_tmp3E;_tmp35;});_tmp36->hd=_tmp777;}),_tmp36->tl=_tmp29;_tmp36;});_tmp29=_tmp778;});goto _LL0;}else{if(((struct _tuple8*)_tmp2B)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2B)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2B)->f3)->tag == 15U){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)((struct _tuple8*)_tmp2B)->f3)->f1)->tag == 1U){_LL3: _tmp41=*_tmp2B->f1;_tmp40=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp2B->f3)->f1)->f2;_LL4: {
# 330
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct _dyneither_ptr _tmp77A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3C=({struct Cyc_String_pa_PrintArg_struct _tmp6D0;_tmp6D0.tag=0U,_tmp6D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp41);_tmp6D0;});void*_tmp3A[1U];_tmp3A[0]=& _tmp3C;({struct _dyneither_ptr _tmp779=({const char*_tmp3B="`%s";_tag_dyneither(_tmp3B,sizeof(char),4U);});Cyc_aprintf(_tmp779,_tag_dyneither(_tmp3A,sizeof(void*),1U));});});*_tmp3D=_tmp77A;});_tmp3D;});
({void*_tmp77D=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=2U,({struct Cyc_Absyn_Tvar*_tmp77C=({struct Cyc_Absyn_Tvar*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->name=nm,_tmp38->identity=- 1,({void*_tmp77B=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->tag=0U,_tmp37->f1=& Cyc_Tcutil_rk;_tmp37;});_tmp38->kind=_tmp77B;});_tmp38;});_tmp39->f1=_tmp77C;});_tmp39;});*_tmp40=_tmp77D;});
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
({struct Cyc_List_List*_tmp77F=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));({struct _tuple18*_tmp77E=({struct _tuple18*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp45->f2=_tmp47;_tmp45;});_tmp46->hd=_tmp77E;}),_tmp46->tl=_tmp42;_tmp46;});_tmp42=_tmp77F;});goto _LL0;}else{_LL3: _LL4:
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
return({void*_tmp781=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=39U,({void*_tmp780=Cyc_Tcutil_copy_type(_tmp4E);_tmp4D->f1=_tmp780;});_tmp4D;});Cyc_Absyn_new_exp(_tmp781,e->loc);});}}
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
({struct Cyc_Absyn_Exp*_tmp782=Cyc_Parse_substitute_tags_exp(tags,_tmp5E);nelts2=_tmp782;});{
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
({struct Cyc_Absyn_Exp*_tmp783=Cyc_Parse_substitute_tags_exp(tags,_tmp5A);_tmp5A=_tmp783;});
({union Cyc_Absyn_Constraint*_tmp785=({union Cyc_Absyn_Constraint*_tmp59=_cycalloc(sizeof(*_tmp59));(_tmp59->Eq_constr).tag=1U,({void*_tmp784=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=1U,_tmp58->f1=_tmp5A;_tmp58;});(_tmp59->Eq_constr).val=_tmp784;});_tmp59;});_tmp55=_tmp785;});
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
({void*_tmp786=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp786;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
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
if(({struct _dyneither_ptr _tmp787=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp787,({const char*_tmp6A="`H";_tag_dyneither(_tmp6A,sizeof(char),3U);}));})== 0)
return(void*)& Cyc_Absyn_HeapRgn_val;else{
if(({struct _dyneither_ptr _tmp788=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp788,({const char*_tmp6B="`U";_tag_dyneither(_tmp6B,sizeof(char),3U);}));})== 0)
return(void*)& Cyc_Absyn_UniqueRgn_val;else{
if(({struct _dyneither_ptr _tmp789=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp789,({const char*_tmp6C="`RC";_tag_dyneither(_tmp6C,sizeof(char),4U);}));})== 0)
return(void*)& Cyc_Absyn_RefCntRgn_val;else{
# 431
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F->tag=2U,({struct Cyc_Absyn_Tvar*_tmp78B=({struct Cyc_Absyn_Tvar*_tmp6E=_cycalloc(sizeof(*_tmp6E));({struct _dyneither_ptr*_tmp78A=({struct _dyneither_ptr*_tmp6D=_cycalloc(sizeof(*_tmp6D));*_tmp6D=s;_tmp6D;});_tmp6E->name=_tmp78A;}),_tmp6E->identity=- 1,_tmp6E->kind=k;_tmp6E;});_tmp6F->f1=_tmp78B;});_tmp6F;});}}}}
# 438
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp70=t;struct Cyc_Absyn_Tvar*_tmp73;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->tag == 2U){_LL1: _tmp73=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp70)->f1;_LL2:
 return _tmp73;}else{_LL3: _LL4:
({void*_tmp71=0U;({unsigned int _tmp78D=loc;struct _dyneither_ptr _tmp78C=({const char*_tmp72="expecting a list of type variables, not types";_tag_dyneither(_tmp72,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp78D,_tmp78C,_tag_dyneither(_tmp71,sizeof(void*),0U));});});}_LL0:;}
# 446
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp74=Cyc_Tcutil_compress(t);void*_tmp75=_tmp74;void**_tmp79;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->tag == 2U){_LL1: _tmp79=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75)->f1)->kind;_LL2: {
# 449
void*_tmp76=Cyc_Absyn_compress_kb(*_tmp79);void*_tmp77=_tmp76;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp77)->tag == 1U){_LL6: _LL7:
# 451
 if(!leq)({void*_tmp78E=Cyc_Tcutil_kind_to_bound(k);*_tmp79=_tmp78E;});else{
({void*_tmp78F=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=2U,_tmp78->f1=0,_tmp78->f2=k;_tmp78;});*_tmp79=_tmp78F;});}
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
({void*_tmp7D=0U;({unsigned int _tmp791=loc;struct _dyneither_ptr _tmp790=({const char*_tmp7E="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp7E,sizeof(char),93U);});Cyc_Warn_warn(_tmp791,_tmp790,_tag_dyneither(_tmp7D,sizeof(void*),0U));});});
# 484
return tms;}else{_LL8: _tmp94=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp7C)->f1;_LL9:
# 486
 if(({int _tmp792=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp94);_tmp792 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp7F=0U;({unsigned int _tmp794=loc;struct _dyneither_ptr _tmp793=({const char*_tmp80="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp80,sizeof(char),73U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp794,_tmp793,_tag_dyneither(_tmp7F,sizeof(void*),0U));});});{
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
({void*_tmp85=0U;({unsigned int _tmp796=_tmp82->loc;struct _dyneither_ptr _tmp795=({const char*_tmp86="initializer found in parameter declaration";_tag_dyneither(_tmp86,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp796,_tmp795,_tag_dyneither(_tmp85,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp8D->name))
({void*_tmp87=0U;({unsigned int _tmp798=_tmp82->loc;struct _dyneither_ptr _tmp797=({const char*_tmp88="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp88,sizeof(char),47U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp798,_tmp797,_tag_dyneither(_tmp87,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp79A=({struct Cyc_List_List*_tmp8A=_cycalloc(sizeof(*_tmp8A));
({struct _tuple8*_tmp799=({struct _tuple8*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->f1=(*_tmp8D->name).f2,_tmp89->f2=_tmp8D->tq,_tmp89->f3=_tmp8D->type;_tmp89;});_tmp8A->hd=_tmp799;}),_tmp8A->tl=rev_new_params;_tmp8A;});
# 503
rev_new_params=_tmp79A;});
# 506
goto L;}else{_LLD: _LLE:
({void*_tmp8B=0U;({unsigned int _tmp79C=_tmp82->loc;struct _dyneither_ptr _tmp79B=({const char*_tmp8C="nonvariable declaration in parameter type";_tag_dyneither(_tmp8C,sizeof(char),42U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp79C,_tmp79B,_tag_dyneither(_tmp8B,sizeof(void*),0U));});});}_LLA:;}
# 510
L: if(_tmp81 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp6D1;_tmp6D1.tag=0U,_tmp6D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp94->hd));_tmp6D1;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({unsigned int _tmp79E=loc;struct _dyneither_ptr _tmp79D=({const char*_tmp8F="%s is not given a type";_tag_dyneither(_tmp8F,sizeof(char),23U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp79E,_tmp79D,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});}
# 513
return({struct Cyc_List_List*_tmp93=_region_malloc(yy,sizeof(*_tmp93));
({void*_tmp7A1=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp92=_region_malloc(yy,sizeof(*_tmp92));_tmp92->tag=3U,({void*_tmp7A0=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp91=_region_malloc(yy,sizeof(*_tmp91));_tmp91->tag=1U,({struct Cyc_List_List*_tmp79F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp91->f1=_tmp79F;}),_tmp91->f2=0,_tmp91->f3=0,_tmp91->f4=0,_tmp91->f5=0,_tmp91->f6=0,_tmp91->f7=0;_tmp91;});_tmp92->f1=_tmp7A0;});_tmp92;});_tmp93->hd=_tmp7A1;}),_tmp93->tl=0;_tmp93;});};}_LL5:;}
# 520
goto _LL4;}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp95=_region_malloc(yy,sizeof(*_tmp95));_tmp95->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp7A2=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp95->tl=_tmp7A2;});_tmp95;});}_LL0:;};}
# 528
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 532
if(tds != 0)
({struct Cyc_Parse_Declarator _tmp7A4=({struct Cyc_Parse_Declarator _tmp6D2;_tmp6D2.id=d.id,_tmp6D2.varloc=d.varloc,({struct Cyc_List_List*_tmp7A3=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6D2.tms=_tmp7A3;});_tmp6D2;});d=_tmp7A4;});{
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
({void*_tmp99=0U;({unsigned int _tmp7A6=loc;struct _dyneither_ptr _tmp7A5=({const char*_tmp9A="bad storage class on function";_tag_dyneither(_tmp9A,sizeof(char),30U);});Cyc_Warn_err(_tmp7A6,_tmp7A5,_tag_dyneither(_tmp99,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 554
void*_tmp9B=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmp9C=Cyc_Parse_apply_tms(tq,_tmp9B,atts,d.tms);struct _tuple13 _tmp9D=_tmp9C;struct Cyc_Absyn_Tqual _tmpC3;void*_tmpC2;struct Cyc_List_List*_tmpC1;struct Cyc_List_List*_tmpC0;_LL8: _tmpC3=_tmp9D.f1;_tmpC2=_tmp9D.f2;_tmpC1=_tmp9D.f3;_tmpC0=_tmp9D.f4;_LL9:;
# 559
if(_tmpC1 != 0)
# 562
({void*_tmp9E=0U;({unsigned int _tmp7A8=loc;struct _dyneither_ptr _tmp7A7=({const char*_tmp9F="bad type params, ignoring";_tag_dyneither(_tmp9F,sizeof(char),26U);});Cyc_Warn_warn(_tmp7A8,_tmp7A7,_tag_dyneither(_tmp9E,sizeof(void*),0U));});});{
# 564
void*_tmpA0=_tmpC2;struct Cyc_List_List*_tmpBF;void*_tmpBE;struct Cyc_Absyn_Tqual _tmpBD;void*_tmpBC;struct Cyc_List_List*_tmpBB;int _tmpBA;struct Cyc_Absyn_VarargInfo*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_List_List*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_List_List*_tmpB3;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->tag == 9U){_LLB: _tmpBF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).tvars;_tmpBE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).effect;_tmpBD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ret_tqual;_tmpBC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ret_typ;_tmpBB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).args;_tmpBA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).c_varargs;_tmpB9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).cyc_varargs;_tmpB8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).rgn_po;_tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).attributes;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).requires_clause;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).requires_relns;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ensures_clause;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpA0)->f1).ensures_relns;_LLC: {
# 568
struct Cyc_List_List*_tmpA1=0;
{struct Cyc_List_List*_tmpA2=_tmpBB;for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
struct _tuple8*_tmpA3=(struct _tuple8*)_tmpA2->hd;struct _tuple8*_tmpA4=_tmpA3;struct _dyneither_ptr*_tmpAF;struct Cyc_Absyn_Tqual _tmpAE;void*_tmpAD;_LL10: _tmpAF=_tmpA4->f1;_tmpAE=_tmpA4->f2;_tmpAD=_tmpA4->f3;_LL11:;
if(_tmpAF == 0){
({void*_tmpA5=0U;({unsigned int _tmp7AA=loc;struct _dyneither_ptr _tmp7A9=({const char*_tmpA6="missing argument variable in function prototype";_tag_dyneither(_tmpA6,sizeof(char),48U);});Cyc_Warn_err(_tmp7AA,_tmp7A9,_tag_dyneither(_tmpA5,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp7AE=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct _tuple8*_tmp7AD=({struct _tuple8*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct _dyneither_ptr*_tmp7AC=({struct _dyneither_ptr*_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct _dyneither_ptr _tmp7AB=({const char*_tmpA7="?";_tag_dyneither(_tmpA7,sizeof(char),2U);});*_tmpA8=_tmp7AB;});_tmpA8;});_tmpA9->f1=_tmp7AC;}),_tmpA9->f2=_tmpAE,_tmpA9->f3=_tmpAD;_tmpA9;});_tmpAA->hd=_tmp7AD;}),_tmpAA->tl=_tmpA1;_tmpAA;});_tmpA1=_tmp7AE;});}else{
# 575
({struct Cyc_List_List*_tmp7B0=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));({struct _tuple8*_tmp7AF=({struct _tuple8*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=_tmpAF,_tmpAB->f2=_tmpAE,_tmpAB->f3=_tmpAD;_tmpAB;});_tmpAC->hd=_tmp7AF;}),_tmpAC->tl=_tmpA1;_tmpAC;});_tmpA1=_tmp7B0;});}}}
# 579
return({struct Cyc_Absyn_Fndecl*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->sc=sc,_tmpB0->name=d.id,_tmpB0->tvs=_tmpBF,_tmpB0->is_inline=is_inline,_tmpB0->effect=_tmpBE,_tmpB0->ret_tqual=_tmpBD,_tmpB0->ret_type=_tmpBC,({
# 582
struct Cyc_List_List*_tmp7B2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA1);_tmpB0->args=_tmp7B2;}),_tmpB0->c_varargs=_tmpBA,_tmpB0->cyc_varargs=_tmpB9,_tmpB0->rgn_po=_tmpB8,_tmpB0->body=body,_tmpB0->cached_typ=0,_tmpB0->param_vardecls=0,_tmpB0->fn_vardecl=0,({
# 589
struct Cyc_List_List*_tmp7B1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB7,_tmpC0);_tmpB0->attributes=_tmp7B1;}),_tmpB0->requires_clause=_tmpB6,_tmpB0->requires_relns=0,_tmpB0->ensures_clause=_tmpB4,_tmpB0->ensures_relns=0;_tmpB0;});}}else{_LLD: _LLE:
# 594
({void*_tmpB1=0U;({unsigned int _tmp7B4=loc;struct _dyneither_ptr _tmp7B3=({const char*_tmpB2="declarator is not a function prototype";_tag_dyneither(_tmpB2,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7B4,_tmp7B3,_tag_dyneither(_tmpB1,sizeof(void*),0U));});});}_LLA:;};};};}static char _tmpC4[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
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
({void*_tmpC8=0U;({unsigned int _tmp7B6=loc;struct _dyneither_ptr _tmp7B5=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B6,_tmp7B5,_tag_dyneither(_tmpC8,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpC9=0U;({unsigned int _tmp7B8=loc;struct _dyneither_ptr _tmp7B7=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7B8,_tmp7B7,_tag_dyneither(_tmpC9,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpCA=0U;({unsigned int _tmp7BA=loc;struct _dyneither_ptr _tmp7B9=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7BA,_tmp7B9,_tag_dyneither(_tmpCA,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpCB=0U;({unsigned int _tmp7BC=loc;struct _dyneither_ptr _tmp7BB=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp7BC,_tmp7BB,_tag_dyneither(_tmpCB,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpCC=0U;({unsigned int _tmp7BE=loc;struct _dyneither_ptr _tmp7BD=Cyc_Parse_msg1;Cyc_Warn_err(_tmp7BE,_tmp7BD,_tag_dyneither(_tmpCC,sizeof(void*),0U));});});else{
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
({void*_tmpCD=0U;({unsigned int _tmp7C0=loc;struct _dyneither_ptr _tmp7BF=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7C0,_tmp7BF,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=Cyc_Absyn_Unsigned;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpCE=0U;({unsigned int _tmp7C2=loc;struct _dyneither_ptr _tmp7C1=Cyc_Parse_msg4;Cyc_Warn_err(_tmp7C2,_tmp7C1,_tag_dyneither(_tmpCE,sizeof(void*),0U));});});
if(ts.Short_spec)sz=Cyc_Absyn_Short_sz;
if(ts.Long_spec)sz=Cyc_Absyn_Long_sz;
if(ts.Long_Long_spec)sz=Cyc_Absyn_LongLong_sz;
# 664
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpCF=0U;({unsigned int _tmp7C4=loc;struct _dyneither_ptr _tmp7C3=({const char*_tmpD0="missing type within specifier";_tag_dyneither(_tmpD0,sizeof(char),30U);});Cyc_Warn_warn(_tmp7C4,_tmp7C3,_tag_dyneither(_tmpCF,sizeof(void*),0U));});});
({void*_tmp7C5=Cyc_Absyn_int_typ(sgn,sz);t=_tmp7C5;});}else{
# 669
if(seen_sign){
void*_tmpD1=t;enum Cyc_Absyn_Sign _tmpD5;enum Cyc_Absyn_Size_of _tmpD4;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1)->tag == 6U){_LL1: _tmpD5=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1)->f1;_tmpD4=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD1)->f2;_LL2:
# 672
 if(_tmpD5 != sgn)
({void*_tmp7C6=Cyc_Absyn_int_typ(sgn,_tmpD4);t=_tmp7C6;});
goto _LL0;}else{_LL3: _LL4:
({void*_tmpD2=0U;({unsigned int _tmp7C8=loc;struct _dyneither_ptr _tmp7C7=({const char*_tmpD3="sign specification on non-integral type";_tag_dyneither(_tmpD3,sizeof(char),40U);});Cyc_Warn_err(_tmp7C8,_tmp7C7,_tag_dyneither(_tmpD2,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 677
if(seen_size){
void*_tmpD6=t;enum Cyc_Absyn_Sign _tmpDA;enum Cyc_Absyn_Size_of _tmpD9;switch(*((int*)_tmpD6)){case 6U: _LL6: _tmpDA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD6)->f1;_tmpD9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD6)->f2;_LL7:
# 680
 if(_tmpD9 != sz)
({void*_tmp7C9=Cyc_Absyn_int_typ(_tmpDA,sz);t=_tmp7C9;});
goto _LL5;case 7U: _LL8: _LL9:
# 684
({void*_tmp7CA=Cyc_Absyn_float_typ(2);t=_tmp7CA;});goto _LL5;default: _LLA: _LLB:
({void*_tmpD7=0U;({unsigned int _tmp7CC=loc;struct _dyneither_ptr _tmp7CB=({const char*_tmpD8="size qualifier on non-integral type";_tag_dyneither(_tmpD8,sizeof(char),36U);});Cyc_Warn_err(_tmp7CC,_tmp7CB,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
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
return({struct Cyc_List_List*_tmpE0=_region_malloc(r,sizeof(*_tmpE0));({struct _tuple15*_tmp7CD=({struct _tuple15*_tmpDF=_region_malloc(r,sizeof(*_tmpDF));_tmpDF->f1=_tmpDC,_tmpDF->f2=_tmpDB,_tmpDF->f3=_tmpE6,_tmpDF->f4=_tmpE5,_tmpDF->f5=_tmpE4,_tmpDF->f6=_tmpE3;_tmpDF;});_tmpE0->hd=_tmp7CD;}),_tmpE0->tl=0;_tmpE0;});else{
# 705
return({struct Cyc_List_List*_tmpE2=_region_malloc(r,sizeof(*_tmpE2));({struct _tuple15*_tmp7D3=({struct _tuple15*_tmpE1=_region_malloc(r,sizeof(*_tmpE1));_tmpE1->f1=_tmpDC,_tmpE1->f2=_tmpDB,_tmpE1->f3=_tmpE6,_tmpE1->f4=_tmpE5,_tmpE1->f5=_tmpE4,_tmpE1->f6=_tmpE3;_tmpE1;});_tmpE2->hd=_tmp7D3;}),({
struct Cyc_List_List*_tmp7D2=({struct _RegionHandle*_tmp7D1=r;struct Cyc_Absyn_Tqual _tmp7D0=tq;void*_tmp7CF=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp7CE=ds->tl;Cyc_Parse_apply_tmss(_tmp7D1,_tmp7D0,_tmp7CF,_tmp7CE,shared_atts);});_tmpE2->tl=_tmp7D2;});_tmpE2;});}};}
# 709
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 712
if(tms == 0)return({struct _tuple13 _tmp6D3;_tmp6D3.f1=tq,_tmp6D3.f2=t,_tmp6D3.f3=0,_tmp6D3.f4=atts;_tmp6D3;});{
void*_tmpE7=(void*)tms->hd;void*_tmpE8=_tmpE7;unsigned int _tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_PtrAtts _tmp10C;struct Cyc_Absyn_Tqual _tmp10B;struct Cyc_List_List*_tmp10A;unsigned int _tmp109;void*_tmp108;struct Cyc_Absyn_Exp*_tmp107;union Cyc_Absyn_Constraint*_tmp106;unsigned int _tmp105;union Cyc_Absyn_Constraint*_tmp104;unsigned int _tmp103;switch(*((int*)_tmpE8)){case 0U: _LL1: _tmp104=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp103=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LL2:
# 715
 return({struct Cyc_Absyn_Tqual _tmp7D6=Cyc_Absyn_empty_tqual(0U);void*_tmp7D5=
Cyc_Absyn_array_typ(t,tq,0,_tmp104,_tmp103);
# 715
struct Cyc_List_List*_tmp7D4=atts;Cyc_Parse_apply_tms(_tmp7D6,_tmp7D5,_tmp7D4,tms->tl);});case 1U: _LL3: _tmp107=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp106=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_tmp105=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE8)->f3;_LL4:
# 718
 return({struct Cyc_Absyn_Tqual _tmp7D9=Cyc_Absyn_empty_tqual(0U);void*_tmp7D8=
Cyc_Absyn_array_typ(t,tq,_tmp107,_tmp106,_tmp105);
# 718
struct Cyc_List_List*_tmp7D7=atts;Cyc_Parse_apply_tms(_tmp7D9,_tmp7D8,_tmp7D7,tms->tl);});case 3U: _LL5: _tmp108=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_LL6: {
# 721
void*_tmpE9=_tmp108;unsigned int _tmpFF;struct Cyc_List_List*_tmpFE;int _tmpFD;struct Cyc_Absyn_VarargInfo*_tmpFC;void*_tmpFB;struct Cyc_List_List*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->tag == 1U){_LLE: _tmpFE=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f1;_tmpFD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f2;_tmpFC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f3;_tmpFB=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f4;_tmpFA=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f5;_tmpF9=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f6;_tmpF8=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE9)->f7;_LLF: {
# 723
struct Cyc_List_List*typvars=0;
# 725
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
({struct Cyc_List_List*_tmp7DA=({struct Cyc_List_List*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->hd=(void*)as->hd,_tmpEA->tl=fn_atts;_tmpEA;});fn_atts=_tmp7DA;});else{
# 730
({struct Cyc_List_List*_tmp7DB=({struct Cyc_List_List*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->hd=(void*)as->hd,_tmpEB->tl=new_atts;_tmpEB;});new_atts=_tmp7DB;});}}}
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
({void*_tmp7DC=Cyc_Parse_substitute_tags(_tmpEF,t);t=_tmp7DC;});
({void*_tmp7DD=Cyc_Parse_array2ptr(t,0);t=_tmp7DD;});
# 759
{struct Cyc_List_List*_tmpF0=_tmpFE;for(0;_tmpF0 != 0;_tmpF0=_tmpF0->tl){
struct _tuple8*_tmpF1=(struct _tuple8*)_tmpF0->hd;struct _tuple8*_tmpF2=_tmpF1;struct _dyneither_ptr*_tmpF5;struct Cyc_Absyn_Tqual _tmpF4;void**_tmpF3;_LL18: _tmpF5=_tmpF2->f1;_tmpF4=_tmpF2->f2;_tmpF3=(void**)& _tmpF2->f3;_LL19:;
if(_tmpEF != 0)
({void*_tmp7DE=Cyc_Parse_substitute_tags(_tmpEF,*_tmpF3);*_tmpF3=_tmp7DE;});
({void*_tmp7DF=Cyc_Parse_array2ptr(*_tmpF3,1);*_tmpF3=_tmp7DF;});}}
# 771
return({struct Cyc_Absyn_Tqual _tmp7E2=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7E1=
Cyc_Absyn_function_typ(typvars,_tmpFB,tq,t,_tmpFE,_tmpFD,_tmpFC,_tmpFA,fn_atts,_tmpF9,_tmpF8);
# 771
struct Cyc_List_List*_tmp7E0=new_atts;Cyc_Parse_apply_tms(_tmp7E2,_tmp7E1,_tmp7E0,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL10: _tmpFF=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpE9)->f2;_LL11:
# 778
({void*_tmpF6=0U;({unsigned int _tmp7E4=_tmpFF;struct _dyneither_ptr _tmp7E3=({const char*_tmpF7="function declaration without parameter types";_tag_dyneither(_tmpF7,sizeof(char),45U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7E4,_tmp7E3,_tag_dyneither(_tmpF6,sizeof(void*),0U));});});}_LLD:;}case 4U: _LL7: _tmp10A=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp109=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LL8:
# 785
 if(tms->tl == 0)
return({struct _tuple13 _tmp6D4;_tmp6D4.f1=tq,_tmp6D4.f2=t,_tmp6D4.f3=_tmp10A,_tmp6D4.f4=atts;_tmp6D4;});
# 790
({void*_tmp100=0U;({unsigned int _tmp7E6=_tmp109;struct _dyneither_ptr _tmp7E5=({const char*_tmp101="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmp101,sizeof(char),68U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7E6,_tmp7E5,_tag_dyneither(_tmp100,sizeof(void*),0U));});});case 2U: _LL9: _tmp10C=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp10B=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LLA:
# 793
 return({struct Cyc_Absyn_Tqual _tmp7E9=_tmp10B;void*_tmp7E8=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102->tag=5U,(_tmp102->f1).elt_typ=t,(_tmp102->f1).elt_tq=tq,(_tmp102->f1).ptr_atts=_tmp10C;_tmp102;});struct Cyc_List_List*_tmp7E7=atts;Cyc_Parse_apply_tms(_tmp7E9,_tmp7E8,_tmp7E7,tms->tl);});default: _LLB: _tmp10E=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE8)->f1;_tmp10D=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE8)->f2;_LLC:
# 798
 return({struct Cyc_Absyn_Tqual _tmp7EC=tq;void*_tmp7EB=t;struct Cyc_List_List*_tmp7EA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp10D);Cyc_Parse_apply_tms(_tmp7EC,_tmp7EB,_tmp7EA,tms->tl);});}_LL0:;};}
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
return({void*_tmp7EE=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7ED=({struct Cyc_Absyn_Typedefdecl*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=_tmp118,_tmp112->tvs=_tmp115,_tmp112->kind=kind,_tmp112->defn=type,_tmp112->atts=_tmp114,_tmp112->tq=_tmp117,_tmp112->extern_c=0;_tmp112;});_tmp113->f1=_tmp7ED;});_tmp113;});Cyc_Absyn_new_decl(_tmp7EE,loc);});};}
# 835
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7EF=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=12U,_tmp11A->f1=d,_tmp11A->f2=s;_tmp11A;});Cyc_Absyn_new_stmt(_tmp7EF,d->loc);});}
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
({void*_tmp11D=0U;({unsigned int _tmp7F1=loc;struct _dyneither_ptr _tmp7F0=({const char*_tmp11E="inline qualifier on non-function definition";_tag_dyneither(_tmp11E,sizeof(char),44U);});Cyc_Warn_warn(_tmp7F1,_tmp7F0,_tag_dyneither(_tmp11D,sizeof(void*),0U));});});{
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
({struct Cyc_List_List*_tmp7F2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp157->attributes,_tmp17B);_tmp157->attributes=_tmp7F2;});
_tmp157->sc=s;{
struct Cyc_List_List*_tmp125=({struct Cyc_List_List*_tmp124=_cycalloc(sizeof(*_tmp124));({struct Cyc_Absyn_Decl*_tmp7F4=({void*_tmp7F3=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=5U,_tmp123->f1=_tmp157;_tmp123;});Cyc_Absyn_new_decl(_tmp7F3,loc);});_tmp124->hd=_tmp7F4;}),_tmp124->tl=0;_tmp124;});_npop_handler(0U);return _tmp125;};case 1U: _LL15: _tmp158=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp122)->f1)->r)->f1;_LL16:
# 898
 if(_tmp17B != 0)({void*_tmp126=0U;({unsigned int _tmp7F6=loc;struct _dyneither_ptr _tmp7F5=({const char*_tmp127="attributes on enum not supported";_tag_dyneither(_tmp127,sizeof(char),33U);});Cyc_Warn_err(_tmp7F6,_tmp7F5,_tag_dyneither(_tmp126,sizeof(void*),0U));});});
_tmp158->sc=s;{
struct Cyc_List_List*_tmp12A=({struct Cyc_List_List*_tmp129=_cycalloc(sizeof(*_tmp129));({struct Cyc_Absyn_Decl*_tmp7F8=({void*_tmp7F7=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=7U,_tmp128->f1=_tmp158;_tmp128;});Cyc_Absyn_new_decl(_tmp7F7,loc);});_tmp129->hd=_tmp7F8;}),_tmp129->tl=0;_tmp129;});_npop_handler(0U);return _tmp12A;};default: _LL17: _tmp159=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp122)->f1)->r)->f1;_LL18:
# 902
 if(_tmp17B != 0)({void*_tmp12B=0U;({unsigned int _tmp7FA=loc;struct _dyneither_ptr _tmp7F9=({const char*_tmp12C="attributes on datatypes not supported";_tag_dyneither(_tmp12C,sizeof(char),38U);});Cyc_Warn_err(_tmp7FA,_tmp7F9,_tag_dyneither(_tmp12B,sizeof(void*),0U));});});
_tmp159->sc=s;{
struct Cyc_List_List*_tmp12F=({struct Cyc_List_List*_tmp12E=_cycalloc(sizeof(*_tmp12E));({struct Cyc_Absyn_Decl*_tmp7FC=({void*_tmp7FB=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=6U,_tmp12D->f1=_tmp159;_tmp12D;});Cyc_Absyn_new_decl(_tmp7FB,loc);});_tmp12E->hd=_tmp7FC;}),_tmp12E->tl=0;_tmp12E;});_npop_handler(0U);return _tmp12F;};}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).aggr_info).UnknownAggr).tag == 1){_LL19: _tmp15C=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).aggr_info).UnknownAggr).val).f1;_tmp15B=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).aggr_info).UnknownAggr).val).f2;_tmp15A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp122)->f1).targs;_LL1A: {
# 906
struct Cyc_List_List*_tmp130=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15A);
struct Cyc_Absyn_Aggrdecl*_tmp131=({struct Cyc_Absyn_Aggrdecl*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->kind=_tmp15C,_tmp137->sc=s,_tmp137->name=_tmp15B,_tmp137->tvs=_tmp130,_tmp137->impl=0,_tmp137->attributes=0,_tmp137->expected_mem_kind=0;_tmp137;});
if(_tmp17B != 0)({void*_tmp132=0U;({unsigned int _tmp7FE=loc;struct _dyneither_ptr _tmp7FD=({const char*_tmp133="bad attributes on type declaration";_tag_dyneither(_tmp133,sizeof(char),35U);});Cyc_Warn_err(_tmp7FE,_tmp7FD,_tag_dyneither(_tmp132,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp136=({struct Cyc_List_List*_tmp135=_cycalloc(sizeof(*_tmp135));({struct Cyc_Absyn_Decl*_tmp800=({void*_tmp7FF=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->tag=5U,_tmp134->f1=_tmp131;_tmp134;});Cyc_Absyn_new_decl(_tmp7FF,loc);});_tmp135->hd=_tmp800;}),_tmp135->tl=0;_tmp135;});_npop_handler(0U);return _tmp136;};}}else{goto _LL23;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).KnownDatatype).tag == 2){_LL1B: _tmp15D=(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).KnownDatatype).val;_LL1C:
# 911
 if(_tmp17B != 0)({void*_tmp138=0U;({unsigned int _tmp802=loc;struct _dyneither_ptr _tmp801=({const char*_tmp139="bad attributes on datatype";_tag_dyneither(_tmp139,sizeof(char),27U);});Cyc_Warn_err(_tmp802,_tmp801,_tag_dyneither(_tmp138,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13C=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));({struct Cyc_Absyn_Decl*_tmp804=({void*_tmp803=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->tag=6U,_tmp13A->f1=*_tmp15D;_tmp13A;});Cyc_Absyn_new_decl(_tmp803,loc);});_tmp13B->hd=_tmp804;}),_tmp13B->tl=0;_tmp13B;});_npop_handler(0U);return _tmp13C;};}else{_LL1D: _tmp160=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).UnknownDatatype).val).name;_tmp15F=((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).datatype_info).UnknownDatatype).val).is_extensible;_tmp15E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp122)->f1).targs;_LL1E: {
# 914
struct Cyc_List_List*_tmp13D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp15E);
struct Cyc_Absyn_Decl*_tmp13E=Cyc_Absyn_datatype_decl(s,_tmp160,_tmp13D,0,_tmp15F,loc);
if(_tmp17B != 0)({void*_tmp13F=0U;({unsigned int _tmp806=loc;struct _dyneither_ptr _tmp805=({const char*_tmp140="bad attributes on datatype";_tag_dyneither(_tmp140,sizeof(char),27U);});Cyc_Warn_err(_tmp806,_tmp805,_tag_dyneither(_tmp13F,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp142=({struct Cyc_List_List*_tmp141=_cycalloc(sizeof(*_tmp141));_tmp141->hd=_tmp13E,_tmp141->tl=0;_tmp141;});_npop_handler(0U);return _tmp142;};}}case 13U: _LL1F: _tmp161=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp122)->f1;_LL20: {
# 919
struct Cyc_Absyn_Enumdecl*_tmp143=({struct Cyc_Absyn_Enumdecl*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->sc=s,_tmp14A->name=_tmp161,_tmp14A->fields=0;_tmp14A;});
if(_tmp17B != 0)({void*_tmp144=0U;({unsigned int _tmp808=loc;struct _dyneither_ptr _tmp807=({const char*_tmp145="bad attributes on enum";_tag_dyneither(_tmp145,sizeof(char),23U);});Cyc_Warn_err(_tmp808,_tmp807,_tag_dyneither(_tmp144,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Decl*_tmp80A=({struct Cyc_Absyn_Decl*_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp809=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->tag=7U,_tmp146->f1=_tmp143;_tmp146;});_tmp147->r=_tmp809;}),_tmp147->loc=loc;_tmp147;});_tmp148->hd=_tmp80A;}),_tmp148->tl=0;_tmp148;});_npop_handler(0U);return _tmp149;};}case 14U: _LL21: _tmp162=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp122)->f1;_LL22: {
# 925
struct Cyc_Absyn_Enumdecl*_tmp14B=({struct Cyc_Absyn_Enumdecl*_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153->sc=s,({struct _tuple0*_tmp80C=Cyc_Parse_gensym_enum();_tmp153->name=_tmp80C;}),({struct Cyc_Core_Opt*_tmp80B=({struct Cyc_Core_Opt*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->v=_tmp162;_tmp152;});_tmp153->fields=_tmp80B;});_tmp153;});
if(_tmp17B != 0)({void*_tmp14C=0U;({unsigned int _tmp80E=loc;struct _dyneither_ptr _tmp80D=({const char*_tmp14D="bad attributes on enum";_tag_dyneither(_tmp14D,sizeof(char),23U);});Cyc_Warn_err(_tmp80E,_tmp80D,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp151=({struct Cyc_List_List*_tmp150=_cycalloc(sizeof(*_tmp150));({struct Cyc_Absyn_Decl*_tmp810=({struct Cyc_Absyn_Decl*_tmp14F=_cycalloc(sizeof(*_tmp14F));({void*_tmp80F=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp14E=_cycalloc(sizeof(*_tmp14E));_tmp14E->tag=7U,_tmp14E->f1=_tmp14B;_tmp14E;});_tmp14F->r=_tmp80F;}),_tmp14F->loc=loc;_tmp14F;});_tmp150->hd=_tmp810;}),_tmp150->tl=0;_tmp150;});_npop_handler(0U);return _tmp151;};}default: _LL23: _LL24:
({void*_tmp154=0U;({unsigned int _tmp812=loc;struct _dyneither_ptr _tmp811=({const char*_tmp155="missing declarator";_tag_dyneither(_tmp155,sizeof(char),19U);});Cyc_Warn_err(_tmp812,_tmp811,_tag_dyneither(_tmp154,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp156=0;_npop_handler(0U);return _tmp156;};}_LL12:;}else{
# 932
struct Cyc_List_List*_tmp163=Cyc_Parse_apply_tmss(mkrgn,_tmp17D,_tmp121,declarators,_tmp17B);
if(istypedef){
# 937
if(!exps_empty)
({void*_tmp164=0U;({unsigned int _tmp814=loc;struct _dyneither_ptr _tmp813=({const char*_tmp165="initializer in typedef declaration";_tag_dyneither(_tmp165,sizeof(char),35U);});Cyc_Warn_err(_tmp814,_tmp813,_tag_dyneither(_tmp164,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp163);
struct Cyc_List_List*_tmp166=decls;_npop_handler(0U);return _tmp166;};}else{
# 943
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp167=_tmp163;for(0;_tmp167 != 0;(_tmp167=_tmp167->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp168=(struct _tuple15*)_tmp167->hd;struct _tuple15*_tmp169=_tmp168;unsigned int _tmp179;struct _tuple0*_tmp178;struct Cyc_Absyn_Tqual _tmp177;void*_tmp176;struct Cyc_List_List*_tmp175;struct Cyc_List_List*_tmp174;_LL26: _tmp179=_tmp169->f1;_tmp178=_tmp169->f2;_tmp177=_tmp169->f3;_tmp176=_tmp169->f4;_tmp175=_tmp169->f5;_tmp174=_tmp169->f6;_LL27:;
if(_tmp175 != 0)
({void*_tmp16A=0U;({unsigned int _tmp816=loc;struct _dyneither_ptr _tmp815=({const char*_tmp16B="bad type params, ignoring";_tag_dyneither(_tmp16B,sizeof(char),26U);});Cyc_Warn_warn(_tmp816,_tmp815,_tag_dyneither(_tmp16A,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp16C=0U;({unsigned int _tmp818=loc;struct _dyneither_ptr _tmp817=({const char*_tmp16D="unexpected NULL in parse!";_tag_dyneither(_tmp16D,sizeof(char),26U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp818,_tmp817,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp16E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp16F=Cyc_Absyn_new_vardecl(_tmp179,_tmp178,_tmp176,_tmp16E);
_tmp16F->tq=_tmp177;
_tmp16F->sc=s;
_tmp16F->attributes=_tmp174;{
struct Cyc_Absyn_Decl*_tmp170=({struct Cyc_Absyn_Decl*_tmp173=_cycalloc(sizeof(*_tmp173));({void*_tmp819=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->tag=0U,_tmp172->f1=_tmp16F;_tmp172;});_tmp173->r=_tmp819;}),_tmp173->loc=loc;_tmp173;});
({struct Cyc_List_List*_tmp81A=({struct Cyc_List_List*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->hd=_tmp170,_tmp171->tl=decls;_tmp171;});decls=_tmp81A;});};};}}{
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
({struct Cyc_String_pa_PrintArg_struct _tmp186=({struct Cyc_String_pa_PrintArg_struct _tmp6D6;_tmp6D6.tag=0U,_tmp6D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp6D6;});struct Cyc_Int_pa_PrintArg_struct _tmp187=({struct Cyc_Int_pa_PrintArg_struct _tmp6D5;_tmp6D5.tag=1U,({unsigned long _tmp81B=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp6D5.f1=_tmp81B;});_tmp6D5;});void*_tmp184[2U];_tmp184[0]=& _tmp186,_tmp184[1]=& _tmp187;({unsigned int _tmp81D=loc;struct _dyneither_ptr _tmp81C=({const char*_tmp185="bad kind: %s; strlen=%d";_tag_dyneither(_tmp185,sizeof(char),24U);});Cyc_Warn_err(_tmp81D,_tmp81C,_tag_dyneither(_tmp184,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 998
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp188=e->r;void*_tmp189=_tmp188;int _tmp18C;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp189)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp189)->f1).Int_c).tag == 5){_LL1: _tmp18C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp189)->f1).Int_c).val).f2;_LL2:
 return _tmp18C;}else{goto _LL3;}}else{_LL3: _LL4:
# 1002
({void*_tmp18A=0U;({unsigned int _tmp81F=loc;struct _dyneither_ptr _tmp81E=({const char*_tmp18B="expecting integer constant";_tag_dyneither(_tmp18B,sizeof(char),27U);});Cyc_Warn_err(_tmp81F,_tmp81E,_tag_dyneither(_tmp18A,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 1008
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp18D=e->r;void*_tmp18E=_tmp18D;struct _dyneither_ptr _tmp191;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18E)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18E)->f1).String_c).tag == 8){_LL1: _tmp191=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18E)->f1).String_c).val;_LL2:
 return _tmp191;}else{goto _LL3;}}else{_LL3: _LL4:
# 1012
({void*_tmp18F=0U;({unsigned int _tmp821=loc;struct _dyneither_ptr _tmp820=({const char*_tmp190="expecting string constant";_tag_dyneither(_tmp190,sizeof(char),26U);});Cyc_Warn_err(_tmp821,_tmp820,_tag_dyneither(_tmp18F,sizeof(void*),0U));});});
return _tag_dyneither(0,0,0);}_LL0:;}
# 1018
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp192=x;long long _tmp19A;char _tmp199;int _tmp198;switch((_tmp192.LongLong_c).tag){case 5U: _LL1: _tmp198=((_tmp192.Int_c).val).f2;_LL2:
 return(unsigned int)_tmp198;case 2U: _LL3: _tmp199=((_tmp192.Char_c).val).f2;_LL4:
 return(unsigned int)_tmp199;case 6U: _LL5: _tmp19A=((_tmp192.LongLong_c).val).f2;_LL6: {
# 1023
unsigned long long y=(unsigned long long)_tmp19A;
if(y > -1)
({void*_tmp193=0U;({unsigned int _tmp823=loc;struct _dyneither_ptr _tmp822=({const char*_tmp194="integer constant too large";_tag_dyneither(_tmp194,sizeof(char),27U);});Cyc_Warn_err(_tmp823,_tmp822,_tag_dyneither(_tmp193,sizeof(void*),0U));});});
return(unsigned int)_tmp19A;}default: _LL7: _LL8:
# 1028
({struct Cyc_String_pa_PrintArg_struct _tmp197=({struct Cyc_String_pa_PrintArg_struct _tmp6D7;_tmp6D7.tag=0U,({struct _dyneither_ptr _tmp824=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp6D7.f1=_tmp824;});_tmp6D7;});void*_tmp195[1U];_tmp195[0]=& _tmp197;({unsigned int _tmp826=loc;struct _dyneither_ptr _tmp825=({const char*_tmp196="expected integer constant but found %s";_tag_dyneither(_tmp196,sizeof(char),39U);});Cyc_Warn_err(_tmp826,_tmp825,_tag_dyneither(_tmp195,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1034
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp19B=p->r;void*_tmp19C=_tmp19B;struct Cyc_Absyn_Exp*_tmp1A9;struct _tuple0*_tmp1A8;struct Cyc_List_List*_tmp1A7;struct _dyneither_ptr _tmp1A6;int _tmp1A5;char _tmp1A4;enum Cyc_Absyn_Sign _tmp1A3;int _tmp1A2;struct Cyc_Absyn_Pat*_tmp1A1;struct Cyc_Absyn_Vardecl*_tmp1A0;struct _tuple0*_tmp19F;switch(*((int*)_tmp19C)){case 15U: _LL1: _tmp19F=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LL2:
 return Cyc_Absyn_unknownid_exp(_tmp19F,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp19C)->f2)->r)->tag == 0U){_LL3: _tmp1A0=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LL4:
# 1038
 return({struct Cyc_Absyn_Exp*_tmp827=Cyc_Absyn_unknownid_exp(_tmp1A0->name,p->loc);Cyc_Absyn_deref_exp(_tmp827,p->loc);});}else{goto _LL13;}case 6U: _LL5: _tmp1A1=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp19C)->f1;_LL6:
 return({struct Cyc_Absyn_Exp*_tmp828=Cyc_Parse_pat2exp(_tmp1A1);Cyc_Absyn_address_exp(_tmp828,p->loc);});case 9U: _LL7: _LL8:
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
({void*_tmp19D=0U;({unsigned int _tmp82A=p->loc;struct _dyneither_ptr _tmp829=({const char*_tmp19E="cannot mix patterns and expressions in case";_tag_dyneither(_tmp19E,sizeof(char),44U);});Cyc_Warn_err(_tmp82A,_tmp829,_tag_dyneither(_tmp19D,sizeof(void*),0U));});});
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
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.Int_tok).tag=1U,(_tmp6D8.Int_tok).val=yy1;_tmp6D8;});}static char _tmp1AD[5U]="char";
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
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.Char_tok).tag=2U,(_tmp6D9.Char_tok).val=yy1;_tmp6D9;});}static char _tmp1B0[13U]="string_t<`H>";
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
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.String_tok).tag=3U,(_tmp6DA.String_tok).val=yy1;_tmp6DA;});}static char _tmp1B3[56U]="$(Position::seg_t,conref_t<bool>,conref_t<bounds_t>)@`H";
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
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.YY1).tag=9U,(_tmp6DB.YY1).val=yy1;_tmp6DB;});}static char _tmp1B6[19U]="conref_t<bounds_t>";
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
union Cyc_YYSTYPE Cyc_YY2(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.YY2).tag=10U,(_tmp6DC.YY2).val=yy1;_tmp6DC;});}static char _tmp1B9[28U]="list_t<offsetof_field_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.YY3).tag=11U,(_tmp6DD.YY3).val=yy1;_tmp6DD;});}static char _tmp1BC[6U]="exp_t";
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
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.Exp_tok).tag=7U,(_tmp6DE.Exp_tok).val=yy1;_tmp6DE;});}static char _tmp1BF[17U]="list_t<exp_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY4).tag=12U,(_tmp6DF.YY4).val=yy1;_tmp6DF;});}static char _tmp1C2[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY5).tag=13U,(_tmp6E0.YY5).val=yy1;_tmp6E0;});}static char _tmp1C5[9U]="primop_t";
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
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY6).tag=14U,(_tmp6E1.YY6).val=yy1;_tmp6E1;});}static char _tmp1C8[19U]="opt_t<primop_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY7).tag=15U,(_tmp6E2.YY7).val=yy1;_tmp6E2;});}static char _tmp1CB[7U]="qvar_t";
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
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.QualId_tok).tag=5U,(_tmp6E3.QualId_tok).val=yy1;_tmp6E3;});}static char _tmp1CE[7U]="stmt_t";
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
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.Stmt_tok).tag=8U,(_tmp6E4.Stmt_tok).val=yy1;_tmp6E4;});}static char _tmp1D1[27U]="list_t<switch_clause_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY8).tag=16U,(_tmp6E5.YY8).val=yy1;_tmp6E5;});}static char _tmp1D4[6U]="pat_t";
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
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY9).tag=17U,(_tmp6E6.YY9).val=yy1;_tmp6E6;});}static char _tmp1D7[28U]="$(list_t<pat_t,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY10).tag=18U,(_tmp6E7.YY10).val=yy1;_tmp6E7;});}static char _tmp1DA[17U]="list_t<pat_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY11).tag=19U,(_tmp6E8.YY11).val=yy1;_tmp6E8;});}static char _tmp1DD[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY12).tag=20U,(_tmp6E9.YY12).val=yy1;_tmp6E9;});}static char _tmp1E0[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY13).tag=21U,(_tmp6EA.YY13).val=yy1;_tmp6EA;});}static char _tmp1E3[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
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
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY14).tag=22U,(_tmp6EB.YY14).val=yy1;_tmp6EB;});}static char _tmp1E6[9U]="fndecl_t";
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
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY15).tag=23U,(_tmp6EC.YY15).val=yy1;_tmp6EC;});}static char _tmp1E9[18U]="list_t<decl_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY16).tag=24U,(_tmp6ED.YY16).val=yy1;_tmp6ED;});}static char _tmp1EC[12U]="decl_spec_t";
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
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY17).tag=25U,(_tmp6EE.YY17).val=yy1;_tmp6EE;});}static char _tmp1EF[31U]="$(declarator_t<`yy>,exp_opt_t)";
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
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY18).tag=26U,(_tmp6EF.YY18).val=yy1;_tmp6EF;});}static char _tmp1F2[23U]="declarator_list_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY19).tag=27U,(_tmp6F0.YY19).val=yy1;_tmp6F0;});}static char _tmp1F5[19U]="storage_class_t@`H";
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
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY20).tag=28U,(_tmp6F1.YY20).val=yy1;_tmp6F1;});}static char _tmp1F8[17U]="type_specifier_t";
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
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY21).tag=29U,(_tmp6F2.YY21).val=yy1;_tmp6F2;});}static char _tmp1FB[12U]="aggr_kind_t";
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
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY22).tag=30U,(_tmp6F3.YY22).val=yy1;_tmp6F3;});}static char _tmp1FE[8U]="tqual_t";
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
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY23).tag=31U,(_tmp6F4.YY23).val=yy1;_tmp6F4;});}static char _tmp201[23U]="list_t<aggrfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY24).tag=32U,(_tmp6F5.YY24).val=yy1;_tmp6F5;});}static char _tmp204[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
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
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.YY25).tag=33U,(_tmp6F6.YY25).val=yy1;_tmp6F6;});}static char _tmp207[33U]="list_t<type_modifier_t<`yy>,`yy>";
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
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY26).tag=34U,(_tmp6F7.YY26).val=yy1;_tmp6F7;});}static char _tmp20A[18U]="declarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6F8;(_tmp6F8.YY27).tag=35U,(_tmp6F8.YY27).val=yy1;_tmp6F8;});}static char _tmp20D[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
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
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6F9;(_tmp6F9.YY28).tag=36U,(_tmp6F9.YY28).val=yy1;_tmp6F9;});}static char _tmp210[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
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
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FA;(_tmp6FA.YY29).tag=37U,(_tmp6FA.YY29).val=yy1;_tmp6FA;});}static char _tmp213[26U]="abstractdeclarator_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6FB;(_tmp6FB.YY30).tag=38U,(_tmp6FB.YY30).val=yy1;_tmp6FB;});}static char _tmp216[5U]="bool";
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
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6FC;(_tmp6FC.YY31).tag=39U,(_tmp6FC.YY31).val=yy1;_tmp6FC;});}static char _tmp219[8U]="scope_t";
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
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6FD;(_tmp6FD.YY32).tag=40U,(_tmp6FD.YY32).val=yy1;_tmp6FD;});}static char _tmp21C[16U]="datatypefield_t";
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
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6FE;(_tmp6FE.YY33).tag=41U,(_tmp6FE.YY33).val=yy1;_tmp6FE;});}static char _tmp21F[27U]="list_t<datatypefield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6FF;(_tmp6FF.YY34).tag=42U,(_tmp6FF.YY34).val=yy1;_tmp6FF;});}static char _tmp222[41U]="$(tqual_t,type_specifier_t,attributes_t)";
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
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp700;(_tmp700.YY35).tag=43U,(_tmp700.YY35).val=yy1;_tmp700;});}static char _tmp225[17U]="list_t<var_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp701;(_tmp701.YY36).tag=44U,(_tmp701.YY36).val=yy1;_tmp701;});}static char _tmp228[31U]="$(var_opt_t,tqual_t,type_t)@`H";
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
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp702;(_tmp702.YY37).tag=45U,(_tmp702.YY37).val=yy1;_tmp702;});}static char _tmp22B[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp703;(_tmp703.YY38).tag=46U,(_tmp703.YY38).val=yy1;_tmp703;});}static char _tmp22E[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
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
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp704;(_tmp704.YY39).tag=47U,(_tmp704.YY39).val=yy1;_tmp704;});}static char _tmp231[18U]="list_t<type_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp705;(_tmp705.YY40).tag=48U,(_tmp705.YY40).val=yy1;_tmp705;});}static char _tmp234[24U]="list_t<designator_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp706;(_tmp706.YY41).tag=49U,(_tmp706.YY41).val=yy1;_tmp706;});}static char _tmp237[13U]="designator_t";
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
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp707;(_tmp707.YY42).tag=50U,(_tmp707.YY42).val=yy1;_tmp707;});}static char _tmp23A[7U]="kind_t";
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
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp708;(_tmp708.YY43).tag=51U,(_tmp708.YY43).val=yy1;_tmp708;});}static char _tmp23D[7U]="type_t";
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
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp709;(_tmp709.YY44).tag=52U,(_tmp709.YY44).val=yy1;_tmp709;});}static char _tmp240[23U]="list_t<attribute_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70A;(_tmp70A.YY45).tag=53U,(_tmp70A.YY45).val=yy1;_tmp70A;});}static char _tmp243[12U]="attribute_t";
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
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp70B;(_tmp70B.YY46).tag=54U,(_tmp70B.YY46).val=yy1;_tmp70B;});}static char _tmp246[12U]="enumfield_t";
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
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp70C;(_tmp70C.YY47).tag=55U,(_tmp70C.YY47).val=yy1;_tmp70C;});}static char _tmp249[23U]="list_t<enumfield_t,`H>";
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
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70D;(_tmp70D.YY48).tag=56U,(_tmp70D.YY48).val=yy1;_tmp70D;});}static char _tmp24C[11U]="type_opt_t";
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
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp70E;(_tmp70E.YY49).tag=57U,(_tmp70E.YY49).val=yy1;_tmp70E;});}static char _tmp24F[31U]="list_t<$(type_t,type_t)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp70F;(_tmp70F.YY50).tag=58U,(_tmp70F.YY50).val=yy1;_tmp70F;});}static char _tmp252[15U]="conref_t<bool>";
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
union Cyc_YYSTYPE Cyc_YY51(union Cyc_Absyn_Constraint*yy1){return({union Cyc_YYSTYPE _tmp710;(_tmp710.YY51).tag=59U,(_tmp710.YY51).val=yy1;_tmp710;});}static char _tmp255[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
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
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp711;(_tmp711.YY52).tag=60U,(_tmp711.YY52).val=yy1;_tmp711;});}static char _tmp258[20U]="pointer_qual_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp712;(_tmp712.YY53).tag=61U,(_tmp712.YY53).val=yy1;_tmp712;});}static char _tmp25B[21U]="pointer_quals_t<`yy>";
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
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp713;(_tmp713.YY54).tag=62U,(_tmp713.YY54).val=yy1;_tmp713;});}static char _tmp25E[21U]="$(bool,string_t<`H>)";
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
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp714;(_tmp714.Asm_tok).tag=6U,(_tmp714.Asm_tok).val=yy1;_tmp714;});}static char _tmp261[10U]="exp_opt_t";
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
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp715;(_tmp715.YY55).tag=63U,(_tmp715.YY55).val=yy1;_tmp715;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1217
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp716;_tmp716.timestamp=0,_tmp716.first_line=0,_tmp716.first_column=0,_tmp716.last_line=0,_tmp716.last_column=0;_tmp716;});}
# 1220
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1231 "parse.y"
static short Cyc_yytranslate[375U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,146,2,2,130,144,141,2,127,128,135,138,123,142,132,143,2,2,2,2,2,2,2,2,2,2,131,120,125,124,126,137,136,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,133,2,134,140,129,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,121,139,122,145,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119};static char _tmp264[2U]="$";static char _tmp265[6U]="error";static char _tmp266[12U]="$undefined.";static char _tmp267[5U]="AUTO";static char _tmp268[9U]="REGISTER";static char _tmp269[7U]="STATIC";static char _tmp26A[7U]="EXTERN";static char _tmp26B[8U]="TYPEDEF";static char _tmp26C[5U]="VOID";static char _tmp26D[5U]="CHAR";static char _tmp26E[6U]="SHORT";static char _tmp26F[4U]="INT";static char _tmp270[5U]="LONG";static char _tmp271[6U]="FLOAT";static char _tmp272[7U]="DOUBLE";static char _tmp273[7U]="SIGNED";static char _tmp274[9U]="UNSIGNED";static char _tmp275[6U]="CONST";static char _tmp276[9U]="VOLATILE";static char _tmp277[9U]="RESTRICT";static char _tmp278[7U]="STRUCT";static char _tmp279[6U]="UNION";static char _tmp27A[5U]="CASE";static char _tmp27B[8U]="DEFAULT";static char _tmp27C[7U]="INLINE";static char _tmp27D[7U]="SIZEOF";static char _tmp27E[9U]="OFFSETOF";static char _tmp27F[3U]="IF";static char _tmp280[5U]="ELSE";static char _tmp281[7U]="SWITCH";static char _tmp282[6U]="WHILE";static char _tmp283[3U]="DO";static char _tmp284[4U]="FOR";static char _tmp285[5U]="GOTO";static char _tmp286[9U]="CONTINUE";static char _tmp287[6U]="BREAK";static char _tmp288[7U]="RETURN";static char _tmp289[5U]="ENUM";static char _tmp28A[7U]="TYPEOF";static char _tmp28B[16U]="BUILTIN_VA_LIST";static char _tmp28C[8U]="NULL_kw";static char _tmp28D[4U]="LET";static char _tmp28E[6U]="THROW";static char _tmp28F[4U]="TRY";static char _tmp290[6U]="CATCH";static char _tmp291[7U]="EXPORT";static char _tmp292[4U]="NEW";static char _tmp293[9U]="ABSTRACT";static char _tmp294[9U]="FALLTHRU";static char _tmp295[6U]="USING";static char _tmp296[10U]="NAMESPACE";static char _tmp297[9U]="DATATYPE";static char _tmp298[7U]="MALLOC";static char _tmp299[8U]="RMALLOC";static char _tmp29A[15U]="RMALLOC_INLINE";static char _tmp29B[7U]="CALLOC";static char _tmp29C[8U]="RCALLOC";static char _tmp29D[5U]="SWAP";static char _tmp29E[9U]="REGION_T";static char _tmp29F[6U]="TAG_T";static char _tmp2A0[7U]="REGION";static char _tmp2A1[5U]="RNEW";static char _tmp2A2[8U]="REGIONS";static char _tmp2A3[7U]="PORTON";static char _tmp2A4[8U]="PORTOFF";static char _tmp2A5[7U]="PRAGMA";static char _tmp2A6[12U]="DYNREGION_T";static char _tmp2A7[8U]="NUMELTS";static char _tmp2A8[8U]="VALUEOF";static char _tmp2A9[10U]="VALUEOF_T";static char _tmp2AA[9U]="TAGCHECK";static char _tmp2AB[13U]="NUMELTS_QUAL";static char _tmp2AC[10U]="THIN_QUAL";static char _tmp2AD[9U]="FAT_QUAL";static char _tmp2AE[13U]="NOTNULL_QUAL";static char _tmp2AF[14U]="NULLABLE_QUAL";static char _tmp2B0[14U]="REQUIRES_QUAL";static char _tmp2B1[13U]="ENSURES_QUAL";static char _tmp2B2[12U]="REGION_QUAL";static char _tmp2B3[16U]="NOZEROTERM_QUAL";static char _tmp2B4[14U]="ZEROTERM_QUAL";static char _tmp2B5[12U]="TAGGED_QUAL";static char _tmp2B6[16U]="EXTENSIBLE_QUAL";static char _tmp2B7[7U]="PTR_OP";static char _tmp2B8[7U]="INC_OP";static char _tmp2B9[7U]="DEC_OP";static char _tmp2BA[8U]="LEFT_OP";static char _tmp2BB[9U]="RIGHT_OP";static char _tmp2BC[6U]="LE_OP";static char _tmp2BD[6U]="GE_OP";static char _tmp2BE[6U]="EQ_OP";static char _tmp2BF[6U]="NE_OP";static char _tmp2C0[7U]="AND_OP";static char _tmp2C1[6U]="OR_OP";static char _tmp2C2[11U]="MUL_ASSIGN";static char _tmp2C3[11U]="DIV_ASSIGN";static char _tmp2C4[11U]="MOD_ASSIGN";static char _tmp2C5[11U]="ADD_ASSIGN";static char _tmp2C6[11U]="SUB_ASSIGN";static char _tmp2C7[12U]="LEFT_ASSIGN";static char _tmp2C8[13U]="RIGHT_ASSIGN";static char _tmp2C9[11U]="AND_ASSIGN";static char _tmp2CA[11U]="XOR_ASSIGN";static char _tmp2CB[10U]="OR_ASSIGN";static char _tmp2CC[9U]="ELLIPSIS";static char _tmp2CD[11U]="LEFT_RIGHT";static char _tmp2CE[12U]="COLON_COLON";static char _tmp2CF[11U]="IDENTIFIER";static char _tmp2D0[17U]="INTEGER_CONSTANT";static char _tmp2D1[7U]="STRING";static char _tmp2D2[8U]="WSTRING";static char _tmp2D3[19U]="CHARACTER_CONSTANT";static char _tmp2D4[20U]="WCHARACTER_CONSTANT";static char _tmp2D5[18U]="FLOATING_CONSTANT";static char _tmp2D6[9U]="TYPE_VAR";static char _tmp2D7[13U]="TYPEDEF_NAME";static char _tmp2D8[16U]="QUAL_IDENTIFIER";static char _tmp2D9[18U]="QUAL_TYPEDEF_NAME";static char _tmp2DA[10U]="ATTRIBUTE";static char _tmp2DB[4U]="ASM";static char _tmp2DC[4U]="';'";static char _tmp2DD[4U]="'{'";static char _tmp2DE[4U]="'}'";static char _tmp2DF[4U]="','";static char _tmp2E0[4U]="'='";static char _tmp2E1[4U]="'<'";static char _tmp2E2[4U]="'>'";static char _tmp2E3[4U]="'('";static char _tmp2E4[4U]="')'";static char _tmp2E5[4U]="'_'";static char _tmp2E6[4U]="'$'";static char _tmp2E7[4U]="':'";static char _tmp2E8[4U]="'.'";static char _tmp2E9[4U]="'['";static char _tmp2EA[4U]="']'";static char _tmp2EB[4U]="'*'";static char _tmp2EC[4U]="'@'";static char _tmp2ED[4U]="'?'";static char _tmp2EE[4U]="'+'";static char _tmp2EF[4U]="'|'";static char _tmp2F0[4U]="'^'";static char _tmp2F1[4U]="'&'";static char _tmp2F2[4U]="'-'";static char _tmp2F3[4U]="'/'";static char _tmp2F4[4U]="'%'";static char _tmp2F5[4U]="'~'";static char _tmp2F6[4U]="'!'";static char _tmp2F7[5U]="prog";static char _tmp2F8[17U]="translation_unit";static char _tmp2F9[16U]="extern_c_action";static char _tmp2FA[13U]="end_extern_c";static char _tmp2FB[12U]="export_list";static char _tmp2FC[19U]="export_list_values";static char _tmp2FD[21U]="external_declaration";static char _tmp2FE[15U]="optional_comma";static char _tmp2FF[20U]="function_definition";static char _tmp300[21U]="function_definition2";static char _tmp301[13U]="using_action";static char _tmp302[15U]="unusing_action";static char _tmp303[17U]="namespace_action";static char _tmp304[19U]="unnamespace_action";static char _tmp305[12U]="declaration";static char _tmp306[17U]="declaration_list";static char _tmp307[23U]="declaration_specifiers";static char _tmp308[24U]="storage_class_specifier";static char _tmp309[15U]="attributes_opt";static char _tmp30A[11U]="attributes";static char _tmp30B[15U]="attribute_list";static char _tmp30C[10U]="attribute";static char _tmp30D[15U]="type_specifier";static char _tmp30E[25U]="type_specifier_notypedef";static char _tmp30F[5U]="kind";static char _tmp310[15U]="type_qualifier";static char _tmp311[15U]="enum_specifier";static char _tmp312[11U]="enum_field";static char _tmp313[22U]="enum_declaration_list";static char _tmp314[26U]="struct_or_union_specifier";static char _tmp315[16U]="type_params_opt";static char _tmp316[16U]="struct_or_union";static char _tmp317[24U]="struct_declaration_list";static char _tmp318[25U]="struct_declaration_list0";static char _tmp319[21U]="init_declarator_list";static char _tmp31A[22U]="init_declarator_list0";static char _tmp31B[16U]="init_declarator";static char _tmp31C[19U]="struct_declaration";static char _tmp31D[25U]="specifier_qualifier_list";static char _tmp31E[35U]="notypedef_specifier_qualifier_list";static char _tmp31F[23U]="struct_declarator_list";static char _tmp320[24U]="struct_declarator_list0";static char _tmp321[18U]="struct_declarator";static char _tmp322[20U]="requires_clause_opt";static char _tmp323[19U]="ensures_clause_opt";static char _tmp324[19U]="datatype_specifier";static char _tmp325[14U]="qual_datatype";static char _tmp326[19U]="datatypefield_list";static char _tmp327[20U]="datatypefield_scope";static char _tmp328[14U]="datatypefield";static char _tmp329[11U]="declarator";static char _tmp32A[23U]="declarator_withtypedef";static char _tmp32B[18U]="direct_declarator";static char _tmp32C[30U]="direct_declarator_withtypedef";static char _tmp32D[8U]="pointer";static char _tmp32E[12U]="one_pointer";static char _tmp32F[14U]="pointer_quals";static char _tmp330[13U]="pointer_qual";static char _tmp331[23U]="pointer_null_and_bound";static char _tmp332[14U]="pointer_bound";static char _tmp333[18U]="zeroterm_qual_opt";static char _tmp334[8U]="rgn_opt";static char _tmp335[11U]="tqual_list";static char _tmp336[20U]="parameter_type_list";static char _tmp337[9U]="type_var";static char _tmp338[16U]="optional_effect";static char _tmp339[19U]="optional_rgn_order";static char _tmp33A[10U]="rgn_order";static char _tmp33B[16U]="optional_inject";static char _tmp33C[11U]="effect_set";static char _tmp33D[14U]="atomic_effect";static char _tmp33E[11U]="region_set";static char _tmp33F[15U]="parameter_list";static char _tmp340[22U]="parameter_declaration";static char _tmp341[16U]="identifier_list";static char _tmp342[17U]="identifier_list0";static char _tmp343[12U]="initializer";static char _tmp344[18U]="array_initializer";static char _tmp345[17U]="initializer_list";static char _tmp346[12U]="designation";static char _tmp347[16U]="designator_list";static char _tmp348[11U]="designator";static char _tmp349[10U]="type_name";static char _tmp34A[14U]="any_type_name";static char _tmp34B[15U]="type_name_list";static char _tmp34C[20U]="abstract_declarator";static char _tmp34D[27U]="direct_abstract_declarator";static char _tmp34E[10U]="statement";static char _tmp34F[18U]="labeled_statement";static char _tmp350[21U]="expression_statement";static char _tmp351[19U]="compound_statement";static char _tmp352[16U]="block_item_list";static char _tmp353[20U]="selection_statement";static char _tmp354[15U]="switch_clauses";static char _tmp355[20U]="iteration_statement";static char _tmp356[15U]="jump_statement";static char _tmp357[12U]="exp_pattern";static char _tmp358[20U]="conditional_pattern";static char _tmp359[19U]="logical_or_pattern";static char _tmp35A[20U]="logical_and_pattern";static char _tmp35B[21U]="inclusive_or_pattern";static char _tmp35C[21U]="exclusive_or_pattern";static char _tmp35D[12U]="and_pattern";static char _tmp35E[17U]="equality_pattern";static char _tmp35F[19U]="relational_pattern";static char _tmp360[14U]="shift_pattern";static char _tmp361[17U]="additive_pattern";static char _tmp362[23U]="multiplicative_pattern";static char _tmp363[13U]="cast_pattern";static char _tmp364[14U]="unary_pattern";static char _tmp365[16U]="postfix_pattern";static char _tmp366[16U]="primary_pattern";static char _tmp367[8U]="pattern";static char _tmp368[19U]="tuple_pattern_list";static char _tmp369[20U]="tuple_pattern_list0";static char _tmp36A[14U]="field_pattern";static char _tmp36B[19U]="field_pattern_list";static char _tmp36C[20U]="field_pattern_list0";static char _tmp36D[11U]="expression";static char _tmp36E[22U]="assignment_expression";static char _tmp36F[20U]="assignment_operator";static char _tmp370[23U]="conditional_expression";static char _tmp371[20U]="constant_expression";static char _tmp372[22U]="logical_or_expression";static char _tmp373[23U]="logical_and_expression";static char _tmp374[24U]="inclusive_or_expression";static char _tmp375[24U]="exclusive_or_expression";static char _tmp376[15U]="and_expression";static char _tmp377[20U]="equality_expression";static char _tmp378[22U]="relational_expression";static char _tmp379[17U]="shift_expression";static char _tmp37A[20U]="additive_expression";static char _tmp37B[26U]="multiplicative_expression";static char _tmp37C[16U]="cast_expression";static char _tmp37D[17U]="unary_expression";static char _tmp37E[15U]="unary_operator";static char _tmp37F[19U]="postfix_expression";static char _tmp380[17U]="field_expression";static char _tmp381[19U]="primary_expression";static char _tmp382[25U]="argument_expression_list";static char _tmp383[26U]="argument_expression_list0";static char _tmp384[9U]="constant";static char _tmp385[20U]="qual_opt_identifier";static char _tmp386[17U]="qual_opt_typedef";static char _tmp387[18U]="struct_union_name";static char _tmp388[11U]="field_name";static char _tmp389[12U]="right_angle";
# 1585 "parse.y"
static struct _dyneither_ptr Cyc_yytname[294U]={{_tmp264,_tmp264,_tmp264 + 2U},{_tmp265,_tmp265,_tmp265 + 6U},{_tmp266,_tmp266,_tmp266 + 12U},{_tmp267,_tmp267,_tmp267 + 5U},{_tmp268,_tmp268,_tmp268 + 9U},{_tmp269,_tmp269,_tmp269 + 7U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 8U},{_tmp26C,_tmp26C,_tmp26C + 5U},{_tmp26D,_tmp26D,_tmp26D + 5U},{_tmp26E,_tmp26E,_tmp26E + 6U},{_tmp26F,_tmp26F,_tmp26F + 4U},{_tmp270,_tmp270,_tmp270 + 5U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 7U},{_tmp273,_tmp273,_tmp273 + 7U},{_tmp274,_tmp274,_tmp274 + 9U},{_tmp275,_tmp275,_tmp275 + 6U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 9U},{_tmp278,_tmp278,_tmp278 + 7U},{_tmp279,_tmp279,_tmp279 + 6U},{_tmp27A,_tmp27A,_tmp27A + 5U},{_tmp27B,_tmp27B,_tmp27B + 8U},{_tmp27C,_tmp27C,_tmp27C + 7U},{_tmp27D,_tmp27D,_tmp27D + 7U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 3U},{_tmp280,_tmp280,_tmp280 + 5U},{_tmp281,_tmp281,_tmp281 + 7U},{_tmp282,_tmp282,_tmp282 + 6U},{_tmp283,_tmp283,_tmp283 + 3U},{_tmp284,_tmp284,_tmp284 + 4U},{_tmp285,_tmp285,_tmp285 + 5U},{_tmp286,_tmp286,_tmp286 + 9U},{_tmp287,_tmp287,_tmp287 + 6U},{_tmp288,_tmp288,_tmp288 + 7U},{_tmp289,_tmp289,_tmp289 + 5U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 16U},{_tmp28C,_tmp28C,_tmp28C + 8U},{_tmp28D,_tmp28D,_tmp28D + 4U},{_tmp28E,_tmp28E,_tmp28E + 6U},{_tmp28F,_tmp28F,_tmp28F + 4U},{_tmp290,_tmp290,_tmp290 + 6U},{_tmp291,_tmp291,_tmp291 + 7U},{_tmp292,_tmp292,_tmp292 + 4U},{_tmp293,_tmp293,_tmp293 + 9U},{_tmp294,_tmp294,_tmp294 + 9U},{_tmp295,_tmp295,_tmp295 + 6U},{_tmp296,_tmp296,_tmp296 + 10U},{_tmp297,_tmp297,_tmp297 + 9U},{_tmp298,_tmp298,_tmp298 + 7U},{_tmp299,_tmp299,_tmp299 + 8U},{_tmp29A,_tmp29A,_tmp29A + 15U},{_tmp29B,_tmp29B,_tmp29B + 7U},{_tmp29C,_tmp29C,_tmp29C + 8U},{_tmp29D,_tmp29D,_tmp29D + 5U},{_tmp29E,_tmp29E,_tmp29E + 9U},{_tmp29F,_tmp29F,_tmp29F + 6U},{_tmp2A0,_tmp2A0,_tmp2A0 + 7U},{_tmp2A1,_tmp2A1,_tmp2A1 + 5U},{_tmp2A2,_tmp2A2,_tmp2A2 + 8U},{_tmp2A3,_tmp2A3,_tmp2A3 + 7U},{_tmp2A4,_tmp2A4,_tmp2A4 + 8U},{_tmp2A5,_tmp2A5,_tmp2A5 + 7U},{_tmp2A6,_tmp2A6,_tmp2A6 + 12U},{_tmp2A7,_tmp2A7,_tmp2A7 + 8U},{_tmp2A8,_tmp2A8,_tmp2A8 + 8U},{_tmp2A9,_tmp2A9,_tmp2A9 + 10U},{_tmp2AA,_tmp2AA,_tmp2AA + 9U},{_tmp2AB,_tmp2AB,_tmp2AB + 13U},{_tmp2AC,_tmp2AC,_tmp2AC + 10U},{_tmp2AD,_tmp2AD,_tmp2AD + 9U},{_tmp2AE,_tmp2AE,_tmp2AE + 13U},{_tmp2AF,_tmp2AF,_tmp2AF + 14U},{_tmp2B0,_tmp2B0,_tmp2B0 + 14U},{_tmp2B1,_tmp2B1,_tmp2B1 + 13U},{_tmp2B2,_tmp2B2,_tmp2B2 + 12U},{_tmp2B3,_tmp2B3,_tmp2B3 + 16U},{_tmp2B4,_tmp2B4,_tmp2B4 + 14U},{_tmp2B5,_tmp2B5,_tmp2B5 + 12U},{_tmp2B6,_tmp2B6,_tmp2B6 + 16U},{_tmp2B7,_tmp2B7,_tmp2B7 + 7U},{_tmp2B8,_tmp2B8,_tmp2B8 + 7U},{_tmp2B9,_tmp2B9,_tmp2B9 + 7U},{_tmp2BA,_tmp2BA,_tmp2BA + 8U},{_tmp2BB,_tmp2BB,_tmp2BB + 9U},{_tmp2BC,_tmp2BC,_tmp2BC + 6U},{_tmp2BD,_tmp2BD,_tmp2BD + 6U},{_tmp2BE,_tmp2BE,_tmp2BE + 6U},{_tmp2BF,_tmp2BF,_tmp2BF + 6U},{_tmp2C0,_tmp2C0,_tmp2C0 + 7U},{_tmp2C1,_tmp2C1,_tmp2C1 + 6U},{_tmp2C2,_tmp2C2,_tmp2C2 + 11U},{_tmp2C3,_tmp2C3,_tmp2C3 + 11U},{_tmp2C4,_tmp2C4,_tmp2C4 + 11U},{_tmp2C5,_tmp2C5,_tmp2C5 + 11U},{_tmp2C6,_tmp2C6,_tmp2C6 + 11U},{_tmp2C7,_tmp2C7,_tmp2C7 + 12U},{_tmp2C8,_tmp2C8,_tmp2C8 + 13U},{_tmp2C9,_tmp2C9,_tmp2C9 + 11U},{_tmp2CA,_tmp2CA,_tmp2CA + 11U},{_tmp2CB,_tmp2CB,_tmp2CB + 10U},{_tmp2CC,_tmp2CC,_tmp2CC + 9U},{_tmp2CD,_tmp2CD,_tmp2CD + 11U},{_tmp2CE,_tmp2CE,_tmp2CE + 12U},{_tmp2CF,_tmp2CF,_tmp2CF + 11U},{_tmp2D0,_tmp2D0,_tmp2D0 + 17U},{_tmp2D1,_tmp2D1,_tmp2D1 + 7U},{_tmp2D2,_tmp2D2,_tmp2D2 + 8U},{_tmp2D3,_tmp2D3,_tmp2D3 + 19U},{_tmp2D4,_tmp2D4,_tmp2D4 + 20U},{_tmp2D5,_tmp2D5,_tmp2D5 + 18U},{_tmp2D6,_tmp2D6,_tmp2D6 + 9U},{_tmp2D7,_tmp2D7,_tmp2D7 + 13U},{_tmp2D8,_tmp2D8,_tmp2D8 + 16U},{_tmp2D9,_tmp2D9,_tmp2D9 + 18U},{_tmp2DA,_tmp2DA,_tmp2DA + 10U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 4U},{_tmp2F1,_tmp2F1,_tmp2F1 + 4U},{_tmp2F2,_tmp2F2,_tmp2F2 + 4U},{_tmp2F3,_tmp2F3,_tmp2F3 + 4U},{_tmp2F4,_tmp2F4,_tmp2F4 + 4U},{_tmp2F5,_tmp2F5,_tmp2F5 + 4U},{_tmp2F6,_tmp2F6,_tmp2F6 + 4U},{_tmp2F7,_tmp2F7,_tmp2F7 + 5U},{_tmp2F8,_tmp2F8,_tmp2F8 + 17U},{_tmp2F9,_tmp2F9,_tmp2F9 + 16U},{_tmp2FA,_tmp2FA,_tmp2FA + 13U},{_tmp2FB,_tmp2FB,_tmp2FB + 12U},{_tmp2FC,_tmp2FC,_tmp2FC + 19U},{_tmp2FD,_tmp2FD,_tmp2FD + 21U},{_tmp2FE,_tmp2FE,_tmp2FE + 15U},{_tmp2FF,_tmp2FF,_tmp2FF + 20U},{_tmp300,_tmp300,_tmp300 + 21U},{_tmp301,_tmp301,_tmp301 + 13U},{_tmp302,_tmp302,_tmp302 + 15U},{_tmp303,_tmp303,_tmp303 + 17U},{_tmp304,_tmp304,_tmp304 + 19U},{_tmp305,_tmp305,_tmp305 + 12U},{_tmp306,_tmp306,_tmp306 + 17U},{_tmp307,_tmp307,_tmp307 + 23U},{_tmp308,_tmp308,_tmp308 + 24U},{_tmp309,_tmp309,_tmp309 + 15U},{_tmp30A,_tmp30A,_tmp30A + 11U},{_tmp30B,_tmp30B,_tmp30B + 15U},{_tmp30C,_tmp30C,_tmp30C + 10U},{_tmp30D,_tmp30D,_tmp30D + 15U},{_tmp30E,_tmp30E,_tmp30E + 25U},{_tmp30F,_tmp30F,_tmp30F + 5U},{_tmp310,_tmp310,_tmp310 + 15U},{_tmp311,_tmp311,_tmp311 + 15U},{_tmp312,_tmp312,_tmp312 + 11U},{_tmp313,_tmp313,_tmp313 + 22U},{_tmp314,_tmp314,_tmp314 + 26U},{_tmp315,_tmp315,_tmp315 + 16U},{_tmp316,_tmp316,_tmp316 + 16U},{_tmp317,_tmp317,_tmp317 + 24U},{_tmp318,_tmp318,_tmp318 + 25U},{_tmp319,_tmp319,_tmp319 + 21U},{_tmp31A,_tmp31A,_tmp31A + 22U},{_tmp31B,_tmp31B,_tmp31B + 16U},{_tmp31C,_tmp31C,_tmp31C + 19U},{_tmp31D,_tmp31D,_tmp31D + 25U},{_tmp31E,_tmp31E,_tmp31E + 35U},{_tmp31F,_tmp31F,_tmp31F + 23U},{_tmp320,_tmp320,_tmp320 + 24U},{_tmp321,_tmp321,_tmp321 + 18U},{_tmp322,_tmp322,_tmp322 + 20U},{_tmp323,_tmp323,_tmp323 + 19U},{_tmp324,_tmp324,_tmp324 + 19U},{_tmp325,_tmp325,_tmp325 + 14U},{_tmp326,_tmp326,_tmp326 + 19U},{_tmp327,_tmp327,_tmp327 + 20U},{_tmp328,_tmp328,_tmp328 + 14U},{_tmp329,_tmp329,_tmp329 + 11U},{_tmp32A,_tmp32A,_tmp32A + 23U},{_tmp32B,_tmp32B,_tmp32B + 18U},{_tmp32C,_tmp32C,_tmp32C + 30U},{_tmp32D,_tmp32D,_tmp32D + 8U},{_tmp32E,_tmp32E,_tmp32E + 12U},{_tmp32F,_tmp32F,_tmp32F + 14U},{_tmp330,_tmp330,_tmp330 + 13U},{_tmp331,_tmp331,_tmp331 + 23U},{_tmp332,_tmp332,_tmp332 + 14U},{_tmp333,_tmp333,_tmp333 + 18U},{_tmp334,_tmp334,_tmp334 + 8U},{_tmp335,_tmp335,_tmp335 + 11U},{_tmp336,_tmp336,_tmp336 + 20U},{_tmp337,_tmp337,_tmp337 + 9U},{_tmp338,_tmp338,_tmp338 + 16U},{_tmp339,_tmp339,_tmp339 + 19U},{_tmp33A,_tmp33A,_tmp33A + 10U},{_tmp33B,_tmp33B,_tmp33B + 16U},{_tmp33C,_tmp33C,_tmp33C + 11U},{_tmp33D,_tmp33D,_tmp33D + 14U},{_tmp33E,_tmp33E,_tmp33E + 11U},{_tmp33F,_tmp33F,_tmp33F + 15U},{_tmp340,_tmp340,_tmp340 + 22U},{_tmp341,_tmp341,_tmp341 + 16U},{_tmp342,_tmp342,_tmp342 + 17U},{_tmp343,_tmp343,_tmp343 + 12U},{_tmp344,_tmp344,_tmp344 + 18U},{_tmp345,_tmp345,_tmp345 + 17U},{_tmp346,_tmp346,_tmp346 + 12U},{_tmp347,_tmp347,_tmp347 + 16U},{_tmp348,_tmp348,_tmp348 + 11U},{_tmp349,_tmp349,_tmp349 + 10U},{_tmp34A,_tmp34A,_tmp34A + 14U},{_tmp34B,_tmp34B,_tmp34B + 15U},{_tmp34C,_tmp34C,_tmp34C + 20U},{_tmp34D,_tmp34D,_tmp34D + 27U},{_tmp34E,_tmp34E,_tmp34E + 10U},{_tmp34F,_tmp34F,_tmp34F + 18U},{_tmp350,_tmp350,_tmp350 + 21U},{_tmp351,_tmp351,_tmp351 + 19U},{_tmp352,_tmp352,_tmp352 + 16U},{_tmp353,_tmp353,_tmp353 + 20U},{_tmp354,_tmp354,_tmp354 + 15U},{_tmp355,_tmp355,_tmp355 + 20U},{_tmp356,_tmp356,_tmp356 + 15U},{_tmp357,_tmp357,_tmp357 + 12U},{_tmp358,_tmp358,_tmp358 + 20U},{_tmp359,_tmp359,_tmp359 + 19U},{_tmp35A,_tmp35A,_tmp35A + 20U},{_tmp35B,_tmp35B,_tmp35B + 21U},{_tmp35C,_tmp35C,_tmp35C + 21U},{_tmp35D,_tmp35D,_tmp35D + 12U},{_tmp35E,_tmp35E,_tmp35E + 17U},{_tmp35F,_tmp35F,_tmp35F + 19U},{_tmp360,_tmp360,_tmp360 + 14U},{_tmp361,_tmp361,_tmp361 + 17U},{_tmp362,_tmp362,_tmp362 + 23U},{_tmp363,_tmp363,_tmp363 + 13U},{_tmp364,_tmp364,_tmp364 + 14U},{_tmp365,_tmp365,_tmp365 + 16U},{_tmp366,_tmp366,_tmp366 + 16U},{_tmp367,_tmp367,_tmp367 + 8U},{_tmp368,_tmp368,_tmp368 + 19U},{_tmp369,_tmp369,_tmp369 + 20U},{_tmp36A,_tmp36A,_tmp36A + 14U},{_tmp36B,_tmp36B,_tmp36B + 19U},{_tmp36C,_tmp36C,_tmp36C + 20U},{_tmp36D,_tmp36D,_tmp36D + 11U},{_tmp36E,_tmp36E,_tmp36E + 22U},{_tmp36F,_tmp36F,_tmp36F + 20U},{_tmp370,_tmp370,_tmp370 + 23U},{_tmp371,_tmp371,_tmp371 + 20U},{_tmp372,_tmp372,_tmp372 + 22U},{_tmp373,_tmp373,_tmp373 + 23U},{_tmp374,_tmp374,_tmp374 + 24U},{_tmp375,_tmp375,_tmp375 + 24U},{_tmp376,_tmp376,_tmp376 + 15U},{_tmp377,_tmp377,_tmp377 + 20U},{_tmp378,_tmp378,_tmp378 + 22U},{_tmp379,_tmp379,_tmp379 + 17U},{_tmp37A,_tmp37A,_tmp37A + 20U},{_tmp37B,_tmp37B,_tmp37B + 26U},{_tmp37C,_tmp37C,_tmp37C + 16U},{_tmp37D,_tmp37D,_tmp37D + 17U},{_tmp37E,_tmp37E,_tmp37E + 15U},{_tmp37F,_tmp37F,_tmp37F + 19U},{_tmp380,_tmp380,_tmp380 + 17U},{_tmp381,_tmp381,_tmp381 + 19U},{_tmp382,_tmp382,_tmp382 + 25U},{_tmp383,_tmp383,_tmp383 + 26U},{_tmp384,_tmp384,_tmp384 + 9U},{_tmp385,_tmp385,_tmp385 + 20U},{_tmp386,_tmp386,_tmp386 + 17U},{_tmp387,_tmp387,_tmp387 + 18U},{_tmp388,_tmp388,_tmp388 + 11U},{_tmp389,_tmp389,_tmp389 + 12U}};
# 1640
static short Cyc_yyr1[528U]={0,147,148,148,148,148,148,148,148,148,148,148,149,150,151,151,152,152,152,153,153,153,154,154,155,155,155,155,156,156,157,158,159,160,161,161,161,161,161,161,161,162,162,163,163,163,163,163,163,163,163,163,163,164,164,164,164,164,164,164,165,165,166,167,167,168,168,168,168,169,169,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,170,171,172,172,172,173,173,173,174,174,175,175,175,176,176,176,176,176,177,177,178,178,179,179,180,180,181,182,182,183,183,184,185,185,185,185,185,185,186,186,186,186,186,186,187,188,188,189,189,189,189,190,190,191,191,192,192,192,193,193,194,194,194,194,195,195,195,196,196,197,197,198,198,199,199,199,199,199,199,199,199,199,199,200,200,200,200,200,200,200,200,200,200,200,201,201,202,203,203,204,204,204,204,204,204,204,204,205,205,205,206,206,207,207,207,208,208,208,209,209,210,210,210,210,211,211,212,212,213,213,214,214,215,215,216,216,217,217,217,217,218,218,219,219,220,220,220,221,222,222,223,223,224,224,224,224,224,225,225,225,225,226,226,227,227,228,228,229,229,230,230,230,230,230,231,231,232,232,232,233,233,233,233,233,233,233,233,233,233,233,234,234,234,234,234,234,235,236,236,237,237,238,238,238,238,238,238,238,238,239,239,239,239,239,239,240,240,240,240,240,240,241,241,241,241,241,241,241,241,241,241,241,241,241,241,242,242,242,242,242,242,242,242,243,244,244,245,245,246,246,247,247,248,248,249,249,250,250,250,251,251,251,251,251,252,252,252,253,253,253,254,254,254,254,255,255,256,256,256,256,256,256,257,258,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,259,260,260,260,261,261,262,262,263,263,263,264,264,265,265,266,266,266,267,267,267,267,267,267,267,267,267,267,267,268,268,268,268,268,268,268,269,270,270,271,271,272,272,273,273,274,274,275,275,275,276,276,276,276,276,277,277,277,278,278,278,279,279,279,279,280,280,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,281,282,282,282,283,283,283,283,283,283,283,283,283,283,283,284,284,284,284,285,285,285,285,285,285,285,285,285,285,285,286,287,287,288,288,288,288,288,289,289,290,290,291,291,292,292,293,293};
# 1696
static short Cyc_yyr2[528U]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,6,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1752
static short Cyc_yydefact[1074U]={0,21,53,54,55,56,58,71,72,73,74,75,76,77,78,79,97,98,99,115,116,49,0,0,83,0,59,0,0,153,90,94,0,0,0,0,0,0,0,518,219,520,519,521,0,0,86,0,205,205,204,1,0,0,19,0,0,20,0,43,51,45,69,47,80,81,0,84,0,0,164,0,189,192,85,168,113,57,56,50,0,101,0,517,0,518,513,514,515,516,113,0,379,0,0,0,0,242,0,381,382,30,32,0,0,0,0,0,0,0,0,0,154,0,0,0,0,0,0,0,202,203,0,2,0,0,0,0,34,0,121,122,124,44,52,46,48,117,522,523,113,113,0,41,0,0,23,0,221,0,177,165,190,0,196,197,200,201,0,199,198,210,192,0,70,57,105,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,502,503,480,0,0,0,0,0,483,481,482,0,407,409,423,431,433,435,437,439,441,444,449,452,455,459,0,461,484,501,499,518,390,0,0,0,0,0,391,389,37,0,0,113,0,0,0,131,127,129,262,264,0,0,39,0,0,9,10,0,0,113,524,525,220,96,0,0,169,87,240,0,237,0,0,3,0,5,0,35,0,0,0,23,0,118,119,144,112,0,151,0,0,0,0,0,0,0,0,0,0,0,518,292,294,0,302,296,0,300,285,286,287,0,288,289,290,0,42,23,124,22,24,269,0,227,243,0,0,223,221,0,207,0,0,0,212,60,211,193,0,106,102,0,0,0,469,0,425,459,0,426,427,0,0,0,0,0,0,0,0,0,0,0,462,463,0,0,0,0,465,466,464,0,82,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,413,414,415,416,417,418,419,420,421,422,412,0,467,0,490,491,0,0,0,505,0,113,383,0,0,404,518,0,0,0,0,258,400,405,0,402,0,380,397,398,0,395,0,244,0,0,0,0,265,0,235,132,137,133,135,128,130,221,0,271,263,272,527,526,0,89,93,0,0,0,91,95,111,66,65,0,63,170,221,239,166,271,241,178,179,0,88,206,13,0,31,0,33,0,123,125,246,245,23,25,108,120,0,0,0,139,140,147,0,113,113,159,0,0,0,0,0,518,0,0,0,331,332,333,0,0,335,0,0,0,303,297,124,301,295,293,26,0,176,228,0,0,0,234,222,229,147,0,0,0,223,175,209,208,171,207,0,0,213,61,114,107,430,104,100,0,0,0,518,247,252,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,504,511,0,510,408,432,0,434,436,438,440,442,443,447,448,445,446,450,451,453,454,456,457,458,411,410,489,486,0,488,0,0,0,393,394,261,0,401,256,259,388,0,257,385,0,392,36,0,386,0,266,0,138,134,136,0,223,0,207,0,273,0,221,0,284,268,0,0,0,113,0,0,0,131,0,113,0,221,0,188,167,238,0,7,0,4,6,27,0,143,126,144,0,0,142,223,152,161,160,0,0,155,0,0,0,310,0,0,0,0,0,0,330,334,0,0,0,298,291,0,28,270,221,0,231,0,0,149,224,0,147,227,215,172,194,195,213,191,468,0,0,248,0,253,471,0,0,0,0,0,500,476,479,0,0,0,0,0,460,507,0,0,487,485,0,0,0,260,403,406,396,399,387,267,236,147,0,274,275,207,0,0,223,207,0,0,38,92,223,518,0,62,64,0,180,0,0,223,0,207,0,0,8,141,0,145,117,150,162,159,159,0,0,0,0,0,0,0,0,0,0,0,0,0,310,336,0,299,29,223,0,232,230,0,173,0,149,223,0,214,496,0,495,0,249,254,0,0,0,0,0,428,429,489,488,509,492,0,512,424,506,508,0,384,149,147,277,283,147,0,276,207,0,117,0,67,181,187,147,0,186,182,207,15,0,16,0,0,0,158,157,304,310,0,0,0,0,0,0,338,339,341,343,345,347,349,351,354,359,362,365,369,371,377,378,0,0,307,316,0,0,0,0,0,0,0,0,0,0,337,217,233,0,225,174,216,221,470,0,0,255,472,473,0,0,478,477,493,0,280,149,149,147,278,40,0,0,149,147,183,14,17,0,146,110,0,0,0,310,0,375,0,0,372,310,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,373,310,0,318,0,0,0,326,0,0,0,0,0,0,309,0,0,223,498,497,0,0,0,494,279,282,149,109,0,184,149,18,163,305,306,0,0,0,0,310,312,342,0,344,346,348,350,352,353,357,358,355,356,360,361,363,364,366,367,368,0,311,317,319,320,0,328,327,0,322,0,0,0,148,226,218,0,0,0,0,281,0,185,308,374,0,370,313,0,310,321,329,323,324,0,251,250,474,0,68,0,340,310,314,325,0,376,315,475,0,0,0};
# 1863
static short Cyc_yydefgoto[147U]={1071,51,52,470,651,851,53,308,54,292,55,472,56,474,57,144,58,59,539,233,456,457,234,62,250,235,64,166,167,65,164,66,270,271,129,130,131,272,236,436,485,486,487,661,801,67,68,666,667,668,69,488,70,461,71,72,161,162,73,120,535,323,704,622,74,623,529,695,521,525,526,430,316,258,96,97,551,477,552,410,411,412,237,309,310,624,442,295,296,297,298,299,300,783,301,302,865,866,867,868,869,870,871,872,873,874,875,876,877,878,879,880,413,421,422,414,415,416,303,198,392,199,544,200,201,202,203,204,205,206,207,208,209,210,211,212,213,808,214,569,570,215,216,76,852,417,446};
# 1881
static short Cyc_yypact[1074U]={2628,- -32768,- -32768,- -32768,- -32768,- 45,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3455,103,- 50,- -32768,3040,- -32768,85,11,- -32768,4,44,36,51,88,107,86,251,202,- -32768,152,- -32768,- -32768,- -32768,192,553,222,213,221,221,- -32768,- -32768,303,2490,- -32768,248,459,- -32768,546,3455,3455,3455,- -32768,3455,- -32768,- -32768,584,- -32768,85,3370,240,119,697,1048,- -32768,- -32768,316,329,355,- -32768,85,346,6356,- -32768,3071,182,- -32768,- -32768,- -32768,- -32768,316,6356,- -32768,375,403,3071,392,428,442,- -32768,257,- -32768,- -32768,3660,3660,166,510,2490,2490,3660,6356,275,- -32768,196,476,607,505,196,4114,6356,- -32768,- -32768,2765,- -32768,2490,2765,2490,2765,- -32768,537,555,- -32768,3242,- -32768,- -32768,- -32768,- -32768,4114,- -32768,- -32768,316,238,1777,- -32768,3370,546,574,3660,3537,3783,- -32768,240,- -32768,575,- -32768,- -32768,- -32768,- -32768,579,- -32768,- -32768,- 40,1048,3660,- -32768,- -32768,606,615,622,85,6743,604,6356,6549,632,636,643,652,664,686,688,694,696,710,6839,6839,- -32768,- -32768,- -32768,2204,716,6935,6935,6935,- -32768,- -32768,- -32768,113,- -32768,- -32768,- 52,656,713,649,718,552,92,714,201,400,- -32768,831,6935,229,- 8,- -32768,750,47,- -32768,3071,73,1224,286,2811,777,- -32768,- -32768,782,6356,316,2811,778,3868,4114,4196,4114,513,- -32768,83,83,- -32768,801,788,- -32768,- -32768,124,300,316,- -32768,- -32768,- -32768,- -32768,33,791,- -32768,- -32768,560,357,- -32768,794,799,- -32768,815,- -32768,816,- -32768,607,4804,3370,574,817,4114,- -32768,930,819,85,822,827,281,829,4263,846,837,826,838,4901,4263,333,818,- -32768,- -32768,848,1921,1921,546,1921,- -32768,- -32768,- -32768,855,- -32768,- -32768,- -32768,367,- -32768,574,856,- -32768,- -32768,841,104,874,- -32768,101,861,852,381,862,771,850,6356,3660,- -32768,868,- -32768,- -32768,104,85,- -32768,6356,869,2204,- -32768,4114,- -32768,- -32768,4360,- -32768,899,6356,6356,6356,6356,6356,6356,907,6356,4114,858,2204,- -32768,- -32768,1921,887,368,6356,- -32768,- -32768,- -32768,6356,- -32768,6935,6356,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6356,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6356,- -32768,196,- -32768,- -32768,4998,196,6356,- -32768,892,316,- -32768,893,894,- -32768,130,196,6356,3071,416,- -32768,- -32768,- -32768,896,900,891,- -32768,- -32768,- -32768,898,904,3071,- -32768,444,1224,902,3660,- -32768,906,908,- -32768,4196,4196,4196,- -32768,- -32768,2977,5095,292,- -32768,319,- -32768,- -32768,101,- -32768,- -32768,911,933,3660,- -32768,- -32768,920,- -32768,916,921,931,- -32768,2847,- -32768,344,412,- -32768,- -32768,- -32768,4114,- -32768,- -32768,- -32768,1432,- -32768,2490,- -32768,2490,- -32768,- -32768,- -32768,- -32768,574,- -32768,- -32768,- -32768,1018,6356,924,932,- -32768,- 33,634,316,316,859,6356,6356,926,935,6356,927,1030,1167,942,- -32768,- -32768,- -32768,482,1019,- -32768,5192,2065,2343,- -32768,- -32768,3242,- -32768,- -32768,- -32768,- -32768,3660,- -32768,- -32768,4114,937,3950,- -32768,- -32768,934,993,101,943,4032,852,- -32768,- -32768,- -32768,- -32768,771,948,146,811,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,949,947,971,950,- -32768,- -32768,745,4804,951,959,960,961,963,533,966,967,968,247,970,969,6646,- -32768,- -32768,973,965,- -32768,656,223,713,649,718,552,92,92,714,714,714,714,201,201,400,400,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,974,- -32768,- 35,3660,3575,4114,- -32768,- -32768,955,- -32768,- -32768,- -32768,- -32768,1266,- -32768,- -32768,2926,- -32768,- -32768,977,- -32768,189,- -32768,4114,- -32768,- -32768,- -32768,976,852,978,771,975,319,3660,3745,5289,- -32768,- -32768,6356,985,83,316,6453,980,33,3107,982,316,3660,3537,5386,- -32768,344,- -32768,990,- -32768,2490,- -32768,- -32768,- -32768,1183,- -32768,- -32768,930,986,6356,- -32768,852,- -32768,- -32768,- -32768,994,85,521,590,592,6356,853,624,4263,988,5483,5580,545,- -32768,- -32768,991,998,996,1921,- -32768,3370,- -32768,841,1010,3660,- -32768,1015,101,1061,- -32768,1013,993,147,- -32768,- -32768,- -32768,- -32768,811,- -32768,1020,162,1021,- -32768,4471,- -32768,- -32768,6356,6356,1118,6356,6549,- -32768,- -32768,- -32768,196,196,1020,1022,4582,- -32768,- -32768,6356,6356,- -32768,- -32768,104,756,1041,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,993,1024,- -32768,- -32768,771,104,1029,852,771,1025,648,- -32768,- -32768,852,1026,1032,- -32768,- -32768,1033,- -32768,104,1034,852,1035,771,1031,305,- -32768,- -32768,6356,- -32768,4114,- -32768,1037,66,859,4263,1045,1039,728,1027,1046,4263,6356,5677,610,5774,612,5871,853,- -32768,1049,- -32768,- -32768,852,197,- -32768,- -32768,1062,- -32768,1076,1061,852,4114,- -32768,- -32768,393,- -32768,6356,- -32768,- -32768,4804,1066,1067,1069,1074,- -32768,899,1070,1072,- -32768,- -32768,759,- -32768,- -32768,- -32768,- -32768,3575,- -32768,1061,993,- -32768,- -32768,993,1082,- -32768,771,1091,4114,1094,- -32768,- -32768,- -32768,993,1084,- -32768,- -32768,771,- -32768,1093,637,1088,1095,4114,- -32768,- -32768,1196,853,1108,7031,1103,2343,6935,1100,- -32768,- 46,- -32768,1146,1101,754,806,212,814,395,433,- -32768,- -32768,- -32768,- -32768,1147,6935,1921,- -32768,- -32768,650,4263,658,5968,4263,659,6065,6162,691,1119,- -32768,- -32768,- -32768,6356,1120,- -32768,- -32768,1010,- -32768,609,307,- -32768,- -32768,- -32768,4114,1217,- -32768,- -32768,- -32768,4693,- -32768,1061,1061,993,- -32768,- -32768,1122,1124,1061,993,- -32768,- -32768,- -32768,275,- -32768,- -32768,662,4263,1123,853,2204,- -32768,4114,1125,- -32768,1633,6935,6356,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6935,6356,- -32768,853,1134,- -32768,4263,4263,665,- -32768,4263,4263,674,4263,676,6259,- -32768,1127,101,852,- -32768,- -32768,2343,1128,1131,- -32768,- -32768,- -32768,1061,- -32768,1151,- -32768,1061,- -32768,- -32768,- -32768,- -32768,1138,1135,1139,6935,853,- -32768,656,320,713,649,649,552,92,92,714,714,714,714,201,201,400,400,- -32768,- -32768,- -32768,324,- -32768,- -32768,- -32768,- -32768,4263,- -32768,- -32768,4263,- -32768,4263,4263,699,- -32768,- -32768,- -32768,1144,781,1140,4114,- -32768,1141,- -32768,- -32768,1020,162,- -32768,- -32768,6356,1633,- -32768,- -32768,- -32768,- -32768,4263,- -32768,- -32768,- -32768,1143,- -32768,527,- -32768,853,- -32768,- -32768,1145,- -32768,- -32768,- -32768,1267,1272,- -32768};
# 1992
static short Cyc_yypgoto[147U]={- -32768,12,- -32768,- -32768,- -32768,359,- -32768,- 153,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 64,- 113,41,- -32768,- -32768,0,668,- -32768,52,- 195,1172,23,- -32768,- -32768,- 134,- -32768,16,1254,- 731,- -32768,- -32768,- -32768,1038,1040,621,413,- -32768,- -32768,645,- 446,- 196,- -32768,- -32768,133,- -32768,- -32768,137,- 239,1230,- 372,140,- -32768,1133,- -32768,- -32768,1265,- 490,- -32768,614,- 114,- 147,- 126,- 398,337,623,629,- 425,- 472,- 115,- 434,- 136,- -32768,- 243,- 160,- 566,- 270,- -32768,913,- 25,- 78,- 123,29,- 265,- 80,- -32768,- -32768,- 65,- 277,- -32768,- 586,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,269,1096,- -32768,717,901,- -32768,132,693,- -32768,- 143,- 361,- 145,- 334,- 362,- 315,964,- 328,- 360,- 314,- 363,- 339,- 98,633,561,995,287,- -32768,- 367,- -32768,164,489,- 57,87,- 111,- 77};
# 2014
static short Cyc_yytable[7178U]={60,574,251,257,146,143,251,578,579,139,584,585,317,337,324,511,512,460,514,268,632,60,315,63,476,238,239,572,338,334,595,245,648,732,314,330,586,587,577,434,326,361,854,659,63,115,700,942,603,575,454,692,61,60,139,580,581,582,583,60,60,60,79,60,77,123,553,269,143,60,150,664,665,61,40,565,63,82,293,305,304,694,63,63,63,362,63,689,359,322,647,943,63,356,357,358,648,400,660,730,133,134,135,696,136,61,221,60,60,922,145,61,61,61,393,61,480,647,102,243,244,61,60,656,60,60,60,60,401,103,63,63,60,699,260,745,261,262,263,264,455,683,60,105,60,63,741,63,63,63,63,150,517,140,219,63,274,276,824,61,61,106,447,522,353,63,524,63,451,104,443,107,220,145,61,627,61,61,61,61,369,370,116,294,61,145,543,404,- 156,99,531,443,39,542,61,132,61,627,247,465,499,42,405,479,304,895,506,431,108,444,39,443,152,110,197,40,465,371,372,42,641,445,523,222,80,743,39,518,293,293,444,293,109,519,433,42,359,219,434,434,434,360,246,538,641,426,45,450,99,541,444,803,253,112,520,220,833,435,113,99,837,- 524,445,453,773,441,543,313,60,248,807,19,20,934,702,- 221,848,249,- 221,588,589,590,306,593,445,463,240,596,293,219,241,63,60,60,98,60,831,602,524,772,950,951,- 243,248,780,- 243,546,220,547,145,710,249,394,395,396,63,63,740,63,114,61,354,562,540,564,898,654,445,117,553,720,395,396,294,294,445,294,952,953,375,118,543,119,376,61,61,359,61,920,997,616,836,60,218,728,1002,397,840,44,926,425,398,399,163,225,147,846,148,124,125,275,903,635,149,397,63,440,1023,229,721,399,524,39,99,230,99,917,99,39,918,41,42,43,294,99,257,462,42,897,924,686,760,642,306,61,465,902,794,494,359,853,495,39,489,418,1049,760,505,599,438,41,42,43,359,122,439,465,850,452,685,359,513,465,433,433,433,677,44,982,813,688,163,631,359,628,684,629,359,687,143,- 12,1050,630,507,553,1051,435,435,435,640,508,646,44,354,165,1065,812,169,725,643,60,644,60,988,60,731,559,645,1069,992,466,354,650,655,652,467,653,516,403,75,359,359,420,63,573,63,567,63,431,420,60,313,223,749,530,747,662,663,765,60,224,81,226,60,100,748,101,543,764,39,762,904,61,63,61,905,61,41,42,43,763,597,63,956,75,377,63,957,977,459,605,145,75,378,379,439,524,75,408,409,145,227,61,696,145,138,818,141,754,813,75,61,703,796,613,61,228,359,958,168,907,819,804,100,99,733,959,960,440,126,127,1037,1005,642,100,826,812,99,1009,1010,99,1015,1016,431,685,809,75,75,465,462,138,465,680,252,75,359,963,901,1003,820,821,75,797,75,75,75,75,1017,1018,1008,293,795,304,489,242,669,670,631,543,673,1006,1007,678,254,75,916,1011,1012,1013,1014,438,776,367,368,777,813,439,646,48,49,50,60,755,39,827,1068,359,265,168,905,39,716,42,769,1001,790,128,39,359,42,834,44,812,45,63,41,42,43,266,604,45,48,49,50,60,844,60,459,48,49,50,39,612,439,307,48,49,50,858,41,42,43,320,61,885,137,321,63,100,63,100,139,100,359,39,359,248,980,778,100,779,986,987,42,249,294,703,145,991,327,889,333,892,359,45,359,61,328,61,256,932,39,48,49,50,464,329,359,363,41,42,43,784,861,862,75,543,928,273,339,929,483,464,340,491,752,940,496,83,256,341,359,99,359,1064,99,839,979,964,342,489,359,359,75,962,466,966,970,359,365,994,343,1042,1027,981,489,1044,359,489,359,373,374,1030,332,1032,335,335,965,787,789,969,975,259,344,359,345,168,350,351,293,84,346,359,347,335,335,335,1056,16,17,18,524,48,49,50,217,86,348,939,87,88,89,319,355,42,335,619,620,621,90,533,534,364,995,432,863,437,92,93,366,1019,1020,1021,94,664,665,864,708,709,95,194,402,139,195,196,781,782,293,828,829,738,914,915,60,423,983,1025,1026,380,424,1028,1029,273,1031,946,947,948,949,83,100,954,955,1048,1058,359,428,63,1063,448,856,857,998,100,999,449,100,468,886,888,458,891,469,894,180,294,381,382,383,384,385,386,387,388,389,390,61,809,471,473,481,490,60,906,492,501,99,502,1052,464,509,1053,464,1054,1055,493,391,497,1038,503,75,478,75,335,75,63,39,86,186,187,87,88,89,464,500,42,510,1066,515,464,445,267,520,294,528,536,349,44,293,190,527,532,545,361,61,335,222,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,537,560,566,1060,598,607,600,601,968,609,608,972,974,610,611,617,478,615,618,554,555,556,557,558,39,633,561,634,636,335,637,657,41,42,43,568,638,881,60,571,671,639,658,672,483,674,256,675,484,679,681,690,48,49,50,354,659,706,697,693,591,63,1004,701,705,707,711,256,- 524,712,713,714,592,715,256,727,734,568,723,294,1022,717,718,719,100,722,739,100,726,729,61,742,753,744,1034,758,746,761,768,791,771,1039,785,774,335,792,153,154,155,156,157,793,39,158,159,160,464,313,256,626,41,42,43,44,798,800,802,75,724,256,816,464,483,810,464,830,841,822,256,832,48,49,50,775,835,883,838,842,843,845,847,855,849,859,860,884,896,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,899,900,21,170,171,908,909,910,911,912,335,913,568,923,568,22,23,24,83,25,172,919,921,925,173,26,927,930,931,29,174,175,176,177,178,933,30,31,32,179,935,938,941,180,35,181,182,36,183,944,961,945,976,984,978,989,996,478,990,37,38,256,184,185,1000,1024,1035,1040,138,1041,1043,1045,432,1047,1046,83,256,1057,1072,1059,1061,100,1067,1073,1070,39,86,186,187,87,88,89,40,41,42,43,44,188,676,993,255,39,111,478,335,189,325,46,291,41,42,43,151,191,770,475,192,83,759,193,194,483,482,195,196,121,1036,84,806,48,49,50,805,799,751,606,736,427,614,406,576,757,407,86,0,1062,87,88,89,767,249,42,0,882,563,0,90,0,0,0,335,0,91,0,92,93,0,408,409,84,94,0,335,0,0,568,95,0,0,0,0,735,0,0,407,86,0,0,87,88,89,0,249,42,0,0,0,0,90,0,0,0,0,0,91,273,92,93,0,408,409,0,94,478,0,335,814,815,95,817,0,0,0,0,0,0,0,0,478,138,0,825,0,0,0,0,0,256,0,0,0,0,0,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,273,0,0,0,0,0,0,0,22,23,24,0,25,0,0,256,649,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,937,33,34,335,35,0,0,36,0,0,0,0,478,0,0,0,0,0,0,37,38,335,0,0,0,0,0,0,478,0,0,0,0,0,0,0,0,0,335,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,0,0,0,0,0,335,0,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,335,0,0,0,0,0,0,0,0,0,0,0,0,0,0,478,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,335,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,781,782,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,335,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,86,186,187,87,88,89,40,41,42,43,44,188,289,142,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,86,186,187,87,88,89,40,41,42,43,44,188,289,142,290,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,288,86,186,187,87,88,89,40,41,42,43,44,188,289,142,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,170,171,277,0,278,279,280,281,282,283,284,285,22,23,24,83,25,172,286,0,0,173,26,287,0,0,29,174,175,176,177,178,0,30,31,32,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,498,86,186,187,87,88,89,40,41,42,43,44,188,289,142,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,83,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,40,41,42,43,44,188,0,352,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,170,171,0,0,0,0,0,0,0,0,0,0,22,23,24,83,0,172,0,0,0,173,0,0,0,0,29,174,175,176,177,178,0,30,31,0,179,0,0,0,180,35,181,182,36,183,0,0,0,0,0,0,0,0,0,0,37,38,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,40,41,42,43,44,188,0,0,0,0,0,0,0,189,0,46,291,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,27,28,29,0,0,0,0,0,0,30,31,32,0,0,33,34,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,83,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,22,23,24,- 11,0,0,0,0,45,0,46,47,0,0,29,0,48,49,50,84,0,30,31,0,0,0,0,0,0,35,0,419,36,0,217,86,0,0,87,88,89,0,0,42,37,38,0,0,90,0,0,0,0,0,91,0,92,93,0,0,0,0,94,0,0,0,0,311,95,0,39,0,0,0,0,0,0,40,41,42,43,44,83,313,0,0,0,0,0,0,459,0,46,47,0,0,439,0,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,84,0,0,0,0,0,0,0,0,0,29,0,737,0,0,217,86,30,31,87,88,89,0,0,42,35,0,0,36,90,0,0,0,0,0,91,0,92,93,0,37,38,0,94,0,0,0,0,0,95,0,0,0,0,0,0,0,0,0,0,0,0,83,311,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,313,0,0,0,0,0,0,438,0,46,47,0,0,439,83,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,84,0,0,0,0,0,0,0,0,0,0,0,22,23,24,85,86,0,0,87,88,89,0,0,42,0,29,0,0,90,0,84,0,30,31,91,0,92,93,0,0,35,0,94,36,0,217,86,0,95,87,88,89,0,0,42,37,38,0,0,90,0,0,0,0,0,91,0,92,93,0,0,0,0,94,0,0,0,0,0,95,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,483,0,46,47,0,0,0,0,48,49,50,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,32,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,0,142,0,0,267,0,0,0,0,46,47,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,25,0,0,0,0,0,26,0,0,0,29,0,0,0,0,0,0,30,31,32,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,40,41,0,43,44,0,0,142,22,23,24,0,0,0,0,46,47,0,26,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,170,171,0,35,0,0,36,0,0,0,0,0,0,0,0,83,0,172,37,38,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,311,181,182,312,183,0,0,0,0,0,40,41,0,43,44,0,313,0,184,185,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,22,23,24,0,0,189,0,0,190,0,408,409,0,191,29,0,192,0,0,193,194,30,31,195,196,231,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,40,41,0,43,44,0,0,232,22,23,24,0,0,0,0,46,47,0,0,0,0,0,29,0,0,0,0,0,0,30,31,0,0,0,170,171,0,35,0,0,36,0,0,0,0,0,0,0,0,83,0,172,37,38,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,311,181,182,0,183,0,0,0,0,0,40,41,0,43,44,0,313,0,184,185,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,22,23,24,0,0,189,0,0,190,0,0,0,318,191,29,0,192,0,0,193,194,30,31,195,196,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,429,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,691,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,698,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,29,0,0,0,0,0,0,30,31,0,0,0,0,0,0,35,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,170,171,277,0,278,279,280,281,282,283,284,285,0,0,0,83,0,172,286,0,0,173,40,287,0,0,44,174,175,176,177,178,0,0,0,0,179,46,47,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,498,86,186,187,87,88,89,0,0,42,0,0,188,289,142,170,171,0,0,0,189,0,548,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,550,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,811,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,823,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,549,86,186,187,87,88,89,0,249,42,0,0,188,0,336,985,0,0,0,0,189,0,0,190,0,408,409,0,191,170,171,192,0,0,193,194,0,0,195,196,0,0,0,0,83,0,172,0,0,0,173,0,0,0,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,336,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,504,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,594,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,625,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,682,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,750,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,766,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,786,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,788,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,887,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,890,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,893,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,967,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,971,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,973,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,1033,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,172,0,193,194,173,0,195,196,0,0,174,175,176,177,178,0,0,0,0,179,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,756,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,189,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,336,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,0,0,193,194,0,0,195,196,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,724,170,171,0,0,0,189,0,0,190,0,0,0,0,191,0,83,192,0,0,193,194,0,0,195,196,0,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,331,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,349,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,170,171,0,0,0,0,189,0,0,190,0,0,0,0,191,83,0,192,0,0,193,194,0,0,195,196,0,174,175,176,177,178,0,0,0,0,0,0,0,0,180,0,181,182,0,183,0,0,0,0,0,0,0,0,0,0,0,0,0,184,185,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,86,186,187,87,88,89,0,0,42,0,0,188,0,0,0,0,0,0,0,936,0,0,190,0,0,0,0,191,0,0,192,0,0,193,194,0,0,195,196};
# 2735
static short Cyc_yycheck[7178U]={0,363,113,118,69,69,117,367,368,66,373,374,148,173,161,292,293,256,295,132,445,21,148,0,267,103,104,361,173,172,397,109,466,599,148,169,375,376,366,234,163,93,773,76,21,45,536,93,409,364,17,523,0,53,111,369,370,371,372,59,60,61,21,63,109,53,336,132,132,69,70,5,6,21,114,352,53,127,142,144,144,527,59,60,61,137,63,521,123,129,462,137,69,191,192,193,530,105,131,134,59,60,61,528,63,53,90,107,108,840,69,59,60,61,212,63,269,489,107,107,108,69,122,484,124,125,126,127,136,125,107,108,132,531,122,625,124,125,126,127,107,508,142,107,144,122,618,124,125,126,127,151,305,66,107,132,140,141,724,107,108,125,239,62,189,142,313,144,245,125,87,120,125,132,122,440,124,125,126,127,88,89,45,142,132,144,329,114,122,25,316,87,107,327,142,58,144,462,111,256,280,116,129,268,268,791,286,232,120,126,107,87,72,127,82,114,273,125,126,116,459,138,121,91,121,623,107,123,292,293,126,295,125,310,234,116,123,107,433,434,435,128,110,321,483,229,127,123,84,326,126,697,115,51,107,125,746,234,106,95,750,131,138,247,662,236,409,120,268,107,108,20,21,859,128,128,766,115,131,377,378,379,145,394,138,256,120,398,352,107,124,268,292,293,25,295,742,408,445,660,88,89,120,107,671,123,331,125,333,268,553,115,83,84,85,292,293,128,295,127,268,189,347,323,349,128,479,138,106,599,83,84,85,292,293,138,295,125,126,138,127,484,121,142,292,293,123,295,838,935,428,749,352,84,131,941,127,755,118,849,228,132,133,125,95,125,764,127,120,121,132,805,450,133,127,352,236,963,121,132,133,528,107,219,127,221,832,223,107,835,115,116,117,352,230,510,256,116,796,845,513,640,459,266,352,462,804,684,127,123,771,130,107,273,128,1001,655,285,402,127,115,116,117,123,121,133,483,122,128,509,123,294,489,433,434,435,500,118,131,709,518,125,442,123,125,509,127,123,513,513,121,131,133,120,724,131,433,434,435,459,127,461,118,331,109,1051,709,121,566,125,470,127,472,919,474,598,344,133,1064,925,123,349,470,483,472,128,474,120,219,0,123,123,223,470,362,472,128,474,523,230,500,120,127,629,123,628,490,491,644,509,107,22,120,513,25,629,27,660,644,107,643,128,470,500,472,132,474,115,116,117,644,399,509,138,45,135,513,142,899,127,124,500,53,143,144,133,693,58,132,133,509,123,500,978,513,66,716,68,635,829,71,509,539,689,120,513,124,123,135,80,813,716,698,84,410,600,143,144,438,120,121,979,944,640,95,728,829,423,948,949,426,954,955,618,674,706,107,108,655,459,111,658,120,127,115,123,883,803,942,720,721,122,690,124,125,126,127,956,957,947,684,686,686,483,114,493,494,627,771,497,945,946,500,128,145,831,950,951,952,953,127,120,90,91,123,915,133,647,135,136,137,651,636,107,731,128,123,120,169,132,107,128,116,651,941,120,120,107,123,116,747,118,915,127,651,115,116,117,123,410,127,135,136,137,684,762,686,127,135,136,137,107,423,133,120,135,136,137,778,115,116,117,127,651,784,121,127,684,219,686,221,768,223,123,107,123,107,108,128,230,128,917,918,116,115,684,703,686,924,123,120,127,120,123,127,123,684,122,686,118,855,107,135,136,137,256,124,123,92,115,116,117,128,25,26,266,899,120,137,127,123,127,273,127,275,633,864,278,40,148,127,123,608,123,1051,611,128,903,128,127,640,123,123,294,882,123,128,128,123,140,128,127,988,128,905,655,992,123,658,123,86,87,128,170,128,172,173,887,676,677,890,120,119,127,123,127,327,184,185,883,92,127,123,127,191,192,193,128,17,18,19,978,135,136,137,107,108,127,863,111,112,113,149,127,116,212,433,434,435,121,79,80,139,933,233,127,235,129,130,141,958,959,960,135,5,6,138,122,123,141,142,121,929,145,146,22,23,941,122,123,611,122,123,883,107,910,966,967,57,107,970,971,271,973,140,141,90,91,40,410,86,87,1000,122,123,127,883,1050,107,776,777,936,423,938,126,426,122,785,786,128,788,122,790,65,883,94,95,96,97,98,99,100,101,102,103,883,1047,122,122,122,121,941,810,121,107,781,120,1027,459,131,1030,462,1032,1033,127,124,127,982,120,470,267,472,329,474,941,107,108,109,110,111,112,113,483,127,116,127,1056,122,489,138,124,107,941,131,134,127,118,1051,130,128,128,122,93,941,361,863,363,364,365,366,367,368,369,370,371,372,373,374,375,376,377,378,379,320,107,128,1041,125,122,126,126,889,131,123,892,893,128,123,122,336,128,123,339,340,341,342,343,107,127,346,107,121,409,127,120,115,116,117,355,128,781,1051,359,127,123,123,121,127,131,438,30,131,120,44,127,135,136,137,936,76,123,128,138,380,1051,943,128,128,107,128,459,131,123,123,123,392,123,466,123,134,397,122,1051,961,128,128,128,608,128,122,611,128,128,1051,128,120,128,975,128,134,128,121,121,127,982,127,122,484,120,71,72,73,74,75,128,107,78,79,80,640,120,510,439,115,116,117,118,122,77,126,651,121,521,25,655,127,125,658,107,123,128,530,128,135,136,137,667,128,131,134,128,128,128,128,127,134,121,128,122,120,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,127,114,24,25,26,128,128,127,123,128,566,128,508,108,510,37,38,39,40,41,42,128,120,128,46,47,122,128,122,51,52,53,54,55,56,28,58,59,60,61,121,127,131,65,66,67,68,69,70,92,92,139,122,25,123,122,122,553,123,81,82,629,84,85,128,120,128,128,768,127,108,122,640,123,128,40,644,122,0,128,128,781,128,0,128,107,108,109,110,111,112,113,114,115,116,117,118,119,120,929,117,107,37,599,660,127,162,129,130,115,116,117,71,135,658,266,138,40,639,141,142,127,271,145,146,49,978,92,703,135,136,137,698,693,630,411,608,230,426,104,365,637,107,108,- 1,1047,111,112,113,645,115,116,- 1,781,348,- 1,121,- 1,- 1,- 1,716,- 1,127,- 1,129,130,- 1,132,133,92,135,- 1,728,- 1,- 1,671,141,- 1,- 1,- 1,- 1,104,- 1,- 1,107,108,- 1,- 1,111,112,113,- 1,115,116,- 1,- 1,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,773,129,130,- 1,132,133,- 1,135,709,- 1,771,712,713,141,715,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,724,929,- 1,727,- 1,- 1,- 1,- 1,- 1,805,- 1,- 1,- 1,- 1,- 1,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,840,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,855,45,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,861,63,64,864,66,- 1,- 1,69,- 1,- 1,- 1,- 1,813,- 1,- 1,- 1,- 1,- 1,- 1,81,82,882,- 1,- 1,- 1,- 1,- 1,- 1,829,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,899,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,- 1,- 1,- 1,- 1,- 1,942,- 1,944,945,946,947,948,949,950,951,952,953,954,955,956,957,958,959,960,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,915,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,1000,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,1050,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,- 1,- 1,46,47,48,- 1,- 1,51,52,53,54,55,56,- 1,58,59,60,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,51,52,53,54,55,56,- 1,58,59,- 1,61,- 1,- 1,- 1,65,66,67,68,69,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,114,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,49,50,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,63,64,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,40,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,37,38,39,122,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,51,- 1,135,136,137,92,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,104,69,- 1,107,108,- 1,- 1,111,112,113,- 1,- 1,116,81,82,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,- 1,- 1,104,141,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,40,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,133,- 1,135,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,51,- 1,104,- 1,- 1,107,108,58,59,111,112,113,- 1,- 1,116,66,- 1,- 1,69,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,81,82,- 1,135,- 1,- 1,- 1,- 1,- 1,141,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,133,40,135,136,137,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,107,108,- 1,- 1,111,112,113,- 1,- 1,116,- 1,51,- 1,- 1,121,- 1,92,- 1,58,59,127,- 1,129,130,- 1,- 1,66,- 1,135,69,- 1,107,108,- 1,141,111,112,113,- 1,- 1,116,81,82,- 1,- 1,121,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,- 1,- 1,- 1,- 1,- 1,141,- 1,107,- 1,- 1,- 1,- 1,- 1,- 1,114,115,116,117,118,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,129,130,- 1,- 1,- 1,- 1,135,136,137,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,- 1,- 1,124,- 1,- 1,- 1,- 1,129,130,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,41,- 1,- 1,- 1,- 1,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,37,38,39,- 1,- 1,- 1,- 1,129,130,- 1,47,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,25,26,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,81,82,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,104,67,68,107,70,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,84,85,- 1,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,37,38,39,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,51,- 1,138,- 1,- 1,141,142,58,59,145,146,62,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,- 1,121,37,38,39,- 1,- 1,- 1,- 1,129,130,- 1,- 1,- 1,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,25,26,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,40,- 1,42,81,82,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,104,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,- 1,120,- 1,84,85,- 1,- 1,- 1,- 1,- 1,129,130,8,9,10,11,12,13,14,15,16,17,18,19,20,21,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,37,38,39,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,51,- 1,138,- 1,- 1,141,142,58,59,145,146,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,122,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,122,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,104,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,114,115,- 1,117,118,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,130,- 1,- 1,51,- 1,- 1,- 1,- 1,- 1,- 1,58,59,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,- 1,69,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,82,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,- 1,42,43,- 1,- 1,46,114,48,- 1,- 1,118,52,53,54,55,56,- 1,- 1,- 1,- 1,61,129,130,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,121,25,26,- 1,- 1,- 1,127,- 1,32,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,115,116,- 1,- 1,119,- 1,121,122,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,132,133,- 1,135,25,26,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,- 1,- 1,40,- 1,42,- 1,- 1,- 1,46,- 1,- 1,- 1,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,134,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,120,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,128,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,42,- 1,141,142,46,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,61,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,121,25,26,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,40,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,25,26,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,40,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146,- 1,52,53,54,55,56,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,65,- 1,67,68,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,84,85,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,107,108,109,110,111,112,113,- 1,- 1,116,- 1,- 1,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,127,- 1,- 1,130,- 1,- 1,- 1,- 1,135,- 1,- 1,138,- 1,- 1,141,142,- 1,- 1,145,146};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp426[8U]="stdcall";static char _tmp427[6U]="cdecl";static char _tmp428[9U]="fastcall";static char _tmp429[9U]="noreturn";static char _tmp42A[6U]="const";static char _tmp42B[8U]="aligned";static char _tmp42C[7U]="packed";static char _tmp42D[7U]="shared";static char _tmp42E[7U]="unused";static char _tmp42F[5U]="weak";static char _tmp430[10U]="dllimport";static char _tmp431[10U]="dllexport";static char _tmp432[23U]="no_instrument_function";static char _tmp433[12U]="constructor";static char _tmp434[11U]="destructor";static char _tmp435[22U]="no_check_memory_usage";static char _tmp436[5U]="pure";static char _tmp437[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp38A=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp38A;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp75B;(_tmp75B.YYINITIALSVAL).tag=64U,(_tmp75B.YYINITIALSVAL).val=0;_tmp75B;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp6AC=200U;short*_tmp6AD=_region_calloc(yyregion,sizeof(short),_tmp6AC);_tag_dyneither(_tmp6AD,sizeof(short),_tmp6AC);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp6AB=(unsigned int)200;union Cyc_YYSTYPE*_tmp6AA=({struct _RegionHandle*_tmp82B=yyregion;_region_malloc(_tmp82B,_check_times(_tmp6AB,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp75A=200U;unsigned int i;for(i=0;i < _tmp75A;++ i){_tmp6AA[i]=yylval;}}0;});_tmp6AA;}),sizeof(union Cyc_YYSTYPE),200U);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp6A9=(unsigned int)200;struct Cyc_Yyltype*_tmp6A8=({struct _RegionHandle*_tmp82C=yyregion;_region_malloc(_tmp82C,_check_times(_tmp6A9,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp759=200U;unsigned int i;for(i=0;i < _tmp759;++ i){({struct Cyc_Yyltype _tmp82D=Cyc_yynewloc();_tmp6A8[i]=_tmp82D;});}}0;});_tmp6A8;}),sizeof(struct Cyc_Yyltype),200U);
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
({struct _dyneither_ptr _tmp82F=({const char*_tmp38B="parser stack overflow";_tag_dyneither(_tmp38B,sizeof(char),22U);});int _tmp82E=yystate;Cyc_yyerror(_tmp82F,_tmp82E,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp391=(unsigned int)yystacksize;short*_tmp390=({struct _RegionHandle*_tmp830=yyregion;_region_malloc(_tmp830,_check_times(_tmp391,sizeof(short)));});({{unsigned int _tmp719=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp719;++ i){
i <= yyssp_offset?_tmp390[i]=*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)):(_tmp390[i]=0);}}0;});_tag_dyneither(_tmp390,sizeof(short),_tmp391);});
struct _dyneither_ptr yyvs1=({unsigned int _tmp38F=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp38E=({struct _RegionHandle*_tmp831=yyregion;_region_malloc(_tmp831,_check_times(_tmp38F,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp718=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp718;++ i){
# 219
i <= yyssp_offset?_tmp38E[i]=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)):(_tmp38E[i]=yylval);}}0;});_tag_dyneither(_tmp38E,sizeof(union Cyc_YYSTYPE),_tmp38F);});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp38D=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp38C=({struct _RegionHandle*_tmp832=yyregion;_region_malloc(_tmp832,_check_times(_tmp38D,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp717=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp717;++ i){
i <= yyssp_offset?_tmp38C[i]=*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):({
struct Cyc_Yyltype _tmp833=Cyc_yynewloc();_tmp38C[i]=_tmp833;});}}0;});_tag_dyneither(_tmp38C,sizeof(struct Cyc_Yyltype),_tmp38D);});
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1074U,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
({int _tmp834=Cyc_yylex(yylex_buf,& yylval,& yylloc);yychar=_tmp834;});
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 374?(int)Cyc_yytranslate[_check_known_subscript_notnull(375U,yychar)]: 294;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7177) || Cyc_yycheck[_check_known_subscript_notnull(7178U,yyn)]!= yychar1)goto yydefault;
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
if(yyn == 1073){
int _tmp392=0;_npop_handler(0U);return _tmp392;}
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
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(528U,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp393=yyn;switch(_tmp393){case 1U: _LL1: _LL2: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1196 "parse.y"
yyval=yyyyvsp[0];
({struct Cyc_List_List*_tmp835=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_parse_result=_tmp835;});
# 1199
goto _LL0;}case 2U: _LL3: _LL4: {
# 1201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1202 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp394;_push_handler(& _tmp394);{int _tmp396=0;if(setjmp(_tmp394.handler))_tmp396=1;if(!_tmp396){({struct Cyc_List_List*_tmp836=Cyc_yyget_YY16(yyyyvsp[0]);x=_tmp836;});;_pop_handler();}else{void*_tmp395=(void*)_exn_thrown;void*_tmp397=_tmp395;void*_tmp398;if(((struct Cyc_Core_Failure_exn_struct*)_tmp397)->tag == Cyc_Core_Failure){_LL41E: _LL41F:
 x=0;goto _LL41D;}else{_LL420: _tmp398=_tmp397;_LL421:(int)_rethrow(_tmp398);}_LL41D:;}};}
{struct _handler_cons _tmp399;_push_handler(& _tmp399);{int _tmp39B=0;if(setjmp(_tmp399.handler))_tmp39B=1;if(!_tmp39B){({struct Cyc_List_List*_tmp837=Cyc_yyget_YY16(yyyyvsp[1]);y=_tmp837;});;_pop_handler();}else{void*_tmp39A=(void*)_exn_thrown;void*_tmp39C=_tmp39A;void*_tmp39D;if(((struct Cyc_Core_Failure_exn_struct*)_tmp39C)->tag == Cyc_Core_Failure){_LL423: _LL424:
 y=0;goto _LL422;}else{_LL425: _tmp39D=_tmp39C;_LL426:(int)_rethrow(_tmp39D);}_LL422:;}};}
({union Cyc_YYSTYPE _tmp838=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));yyval=_tmp838;});
# 1209
goto _LL0;}case 3U: _LL5: _LL6: {
# 1211
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1213
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1212 "parse.y"
({union Cyc_YYSTYPE _tmp83D=Cyc_YY16(({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct Cyc_Absyn_Decl*_tmp83C=({struct Cyc_Absyn_Decl*_tmp39F=_cycalloc(sizeof(*_tmp39F));({void*_tmp83B=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->tag=10U,({struct _tuple0*_tmp83A=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp39E->f1=_tmp83A;}),({struct Cyc_List_List*_tmp839=Cyc_yyget_YY16(yyyyvsp[2]);_tmp39E->f2=_tmp839;});_tmp39E;});_tmp39F->r=_tmp83B;}),_tmp39F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp39F;});_tmp3A0->hd=_tmp83C;}),_tmp3A0->tl=0;_tmp3A0;}));yyval=_tmp83D;});
Cyc_Lex_leave_using();
# 1215
goto _LL0;}case 4U: _LL7: _LL8: {
# 1217
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1219
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1216 "parse.y"
({union Cyc_YYSTYPE _tmp843=Cyc_YY16(({struct Cyc_List_List*_tmp3A3=_cycalloc(sizeof(*_tmp3A3));({struct Cyc_Absyn_Decl*_tmp842=({struct Cyc_Absyn_Decl*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({void*_tmp841=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->tag=10U,({struct _tuple0*_tmp840=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3A1->f1=_tmp840;}),({struct Cyc_List_List*_tmp83F=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A1->f2=_tmp83F;});_tmp3A1;});_tmp3A2->r=_tmp841;}),_tmp3A2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A2;});_tmp3A3->hd=_tmp842;}),({struct Cyc_List_List*_tmp83E=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3A3->tl=_tmp83E;});_tmp3A3;}));yyval=_tmp843;});
goto _LL0;}case 5U: _LL9: _LLA: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1219 "parse.y"
({union Cyc_YYSTYPE _tmp849=Cyc_YY16(({struct Cyc_List_List*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct Cyc_Absyn_Decl*_tmp848=({struct Cyc_Absyn_Decl*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({void*_tmp847=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=9U,({struct _dyneither_ptr*_tmp846=({struct _dyneither_ptr*_tmp3A4=_cycalloc(sizeof(*_tmp3A4));({struct _dyneither_ptr _tmp845=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp3A4=_tmp845;});_tmp3A4;});_tmp3A5->f1=_tmp846;}),({struct Cyc_List_List*_tmp844=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3A5->f2=_tmp844;});_tmp3A5;});_tmp3A6->r=_tmp847;}),_tmp3A6->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3A6;});_tmp3A7->hd=_tmp848;}),_tmp3A7->tl=0;_tmp3A7;}));yyval=_tmp849;});
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
{struct _handler_cons _tmp3A8;_push_handler(& _tmp3A8);{int _tmp3AA=0;if(setjmp(_tmp3A8.handler))_tmp3AA=1;if(!_tmp3AA){({struct _dyneither_ptr _tmp84A=Cyc_yyget_String_tok(yyyyvsp[0]);nspace=_tmp84A;});;_pop_handler();}else{void*_tmp3A9=(void*)_exn_thrown;void*_tmp3AB=_tmp3A9;void*_tmp3AD;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3AB)->tag == Cyc_Core_Failure){_LL428: _LL429:
({struct _dyneither_ptr _tmp84B=({const char*_tmp3AC="";_tag_dyneither(_tmp3AC,sizeof(char),1U);});nspace=_tmp84B;});goto _LL427;}else{_LL42A: _tmp3AD=_tmp3AB;_LL42B:(int)_rethrow(_tmp3AD);}_LL427:;}};}
# 1229
{struct _handler_cons _tmp3AE;_push_handler(& _tmp3AE);{int _tmp3B0=0;if(setjmp(_tmp3AE.handler))_tmp3B0=1;if(!_tmp3B0){({struct Cyc_List_List*_tmp84C=Cyc_yyget_YY16(yyyyvsp[2]);x=_tmp84C;});;_pop_handler();}else{void*_tmp3AF=(void*)_exn_thrown;void*_tmp3B1=_tmp3AF;void*_tmp3B2;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3B1)->tag == Cyc_Core_Failure){_LL42D: _LL42E:
 x=0;goto _LL42C;}else{_LL42F: _tmp3B2=_tmp3B1;_LL430:(int)_rethrow(_tmp3B2);}_LL42C:;}};}
# 1232
{struct _handler_cons _tmp3B3;_push_handler(& _tmp3B3);{int _tmp3B5=0;if(setjmp(_tmp3B3.handler))_tmp3B5=1;if(!_tmp3B5){({struct Cyc_List_List*_tmp84D=Cyc_yyget_YY16(yyyyvsp[4]);y=_tmp84D;});;_pop_handler();}else{void*_tmp3B4=(void*)_exn_thrown;void*_tmp3B6=_tmp3B4;void*_tmp3B7;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3B6)->tag == Cyc_Core_Failure){_LL432: _LL433:
 y=0;goto _LL431;}else{_LL434: _tmp3B7=_tmp3B6;_LL435:(int)_rethrow(_tmp3B7);}_LL431:;}};}
# 1235
({union Cyc_YYSTYPE _tmp851=Cyc_YY16(({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct Cyc_Absyn_Decl*_tmp850=({struct Cyc_Absyn_Decl*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({void*_tmp84F=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=9U,({struct _dyneither_ptr*_tmp84E=({struct _dyneither_ptr*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));*_tmp3B8=nspace;_tmp3B8;});_tmp3B9->f1=_tmp84E;}),_tmp3B9->f2=x;_tmp3B9;});_tmp3BA->r=_tmp84F;}),_tmp3BA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BA;});_tmp3BB->hd=_tmp850;}),_tmp3BB->tl=y;_tmp3BB;}));yyval=_tmp851;});
# 1237
goto _LL0;}case 7U: _LLD: _LLE: {
# 1239
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1241
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1238 "parse.y"
int _tmp3BC=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp3BC)
({union Cyc_YYSTYPE _tmp856=Cyc_YY16(({struct Cyc_List_List*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({struct Cyc_Absyn_Decl*_tmp855=({struct Cyc_Absyn_Decl*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({void*_tmp854=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=11U,({struct Cyc_List_List*_tmp853=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3BD->f1=_tmp853;});_tmp3BD;});_tmp3BE->r=_tmp854;}),_tmp3BE->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BE;});_tmp3BF->hd=_tmp855;}),({struct Cyc_List_List*_tmp852=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3BF->tl=_tmp852;});_tmp3BF;}));yyval=_tmp856;});else{
# 1242
({union Cyc_YYSTYPE _tmp85B=Cyc_YY16(({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Decl*_tmp85A=({struct Cyc_Absyn_Decl*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));({void*_tmp859=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));_tmp3C0->tag=12U,({struct Cyc_List_List*_tmp858=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C0->f1=_tmp858;}),_tmp3C0->f2=0;_tmp3C0;});_tmp3C1->r=_tmp859;}),_tmp3C1->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C1;});_tmp3C2->hd=_tmp85A;}),({struct Cyc_List_List*_tmp857=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3C2->tl=_tmp857;});_tmp3C2;}));yyval=_tmp85B;});}
# 1244
goto _LL0;}case 8U: _LLF: _LL10: {
# 1246
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1248
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1245 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp3C3=0U;({unsigned int _tmp85D=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp85C=({const char*_tmp3C4="expecting \"C include\"";_tag_dyneither(_tmp3C4,sizeof(char),22U);});Cyc_Warn_err(_tmp85D,_tmp85C,_tag_dyneither(_tmp3C3,sizeof(void*),0U));});});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
({union Cyc_YYSTYPE _tmp862=Cyc_YY16(({struct Cyc_List_List*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct Cyc_Absyn_Decl*_tmp861=({struct Cyc_Absyn_Decl*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({void*_tmp860=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->tag=12U,({struct Cyc_List_List*_tmp85F=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C5->f1=_tmp85F;}),_tmp3C5->f2=exs;_tmp3C5;});_tmp3C6->r=_tmp860;}),_tmp3C6->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C6;});_tmp3C7->hd=_tmp861;}),({struct Cyc_List_List*_tmp85E=Cyc_yyget_YY16(yyyyvsp[5]);_tmp3C7->tl=_tmp85E;});_tmp3C7;}));yyval=_tmp862;});
# 1250
goto _LL0;};}case 9U: _LL11: _LL12: {
# 1252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1251 "parse.y"
({union Cyc_YYSTYPE _tmp865=Cyc_YY16(({struct Cyc_List_List*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));({struct Cyc_Absyn_Decl*_tmp864=({struct Cyc_Absyn_Decl*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C8->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C8;});_tmp3C9->hd=_tmp864;}),({struct Cyc_List_List*_tmp863=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C9->tl=_tmp863;});_tmp3C9;}));yyval=_tmp865;});
goto _LL0;}case 10U: _LL13: _LL14: {
# 1254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1253 "parse.y"
({union Cyc_YYSTYPE _tmp868=Cyc_YY16(({struct Cyc_List_List*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));({struct Cyc_Absyn_Decl*_tmp867=({struct Cyc_Absyn_Decl*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3CA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3CA;});_tmp3CB->hd=_tmp867;}),({struct Cyc_List_List*_tmp866=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3CB->tl=_tmp866;});_tmp3CB;}));yyval=_tmp868;});
goto _LL0;}case 11U: _LL15: _LL16:
# 1256
({union Cyc_YYSTYPE _tmp869=Cyc_YY16(0);yyval=_tmp869;});
goto _LL0;case 12U: _LL17: _LL18: {
# 1259
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1261
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1259 "parse.y"
struct _dyneither_ptr _tmp3CC=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({struct _dyneither_ptr _tmp86A=(struct _dyneither_ptr)_tmp3CC;Cyc_strcmp(_tmp86A,({const char*_tmp3CD="C";_tag_dyneither(_tmp3CD,sizeof(char),2U);}));})== 0)
({union Cyc_YYSTYPE _tmp86B=Cyc_YY31(0);yyval=_tmp86B;});else{
if(({struct _dyneither_ptr _tmp86C=(struct _dyneither_ptr)_tmp3CC;Cyc_strcmp(_tmp86C,({const char*_tmp3CE="C include";_tag_dyneither(_tmp3CE,sizeof(char),10U);}));})== 0)
({union Cyc_YYSTYPE _tmp86D=Cyc_YY31(1);yyval=_tmp86D;});else{
# 1266
({void*_tmp3CF=0U;({unsigned int _tmp86F=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp86E=({const char*_tmp3D0="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3D0,sizeof(char),29U);});Cyc_Warn_err(_tmp86F,_tmp86E,_tag_dyneither(_tmp3CF,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp870=Cyc_YY31(1);yyval=_tmp870;});}}
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
({union Cyc_YYSTYPE _tmp871=Cyc_YY52(0);yyval=_tmp871;});
goto _LL0;}case 16U: _LL1F: _LL20: {
# 1281
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1283
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1282 "parse.y"
({union Cyc_YYSTYPE _tmp874=Cyc_YY52(({struct Cyc_List_List*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct _tuple27*_tmp873=({struct _tuple27*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp872=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D1->f2=_tmp872;}),_tmp3D1->f3=0;_tmp3D1;});_tmp3D2->hd=_tmp873;}),_tmp3D2->tl=0;_tmp3D2;}));yyval=_tmp874;});
goto _LL0;}case 17U: _LL21: _LL22: {
# 1285
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1287
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1283 "parse.y"
({union Cyc_YYSTYPE _tmp877=Cyc_YY52(({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct _tuple27*_tmp876=({struct _tuple27*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp875=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D3->f2=_tmp875;}),_tmp3D3->f3=0;_tmp3D3;});_tmp3D4->hd=_tmp876;}),_tmp3D4->tl=0;_tmp3D4;}));yyval=_tmp877;});
goto _LL0;}case 18U: _LL23: _LL24: {
# 1286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1285 "parse.y"
({union Cyc_YYSTYPE _tmp87B=Cyc_YY52(({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _tuple27*_tmp87A=({struct _tuple27*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));_tmp3D5->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp879=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3D5->f2=_tmp879;}),_tmp3D5->f3=0;_tmp3D5;});_tmp3D6->hd=_tmp87A;}),({struct Cyc_List_List*_tmp878=Cyc_yyget_YY52(yyyyvsp[2]);_tmp3D6->tl=_tmp878;});_tmp3D6;}));yyval=_tmp87B;});
goto _LL0;}case 19U: _LL25: _LL26: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1289 "parse.y"
({union Cyc_YYSTYPE _tmp87F=Cyc_YY16(({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));({struct Cyc_Absyn_Decl*_tmp87E=({void*_tmp87D=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp87C=Cyc_yyget_YY15(yyyyvsp[0]);_tmp3D7->f1=_tmp87C;});_tmp3D7;});Cyc_Absyn_new_decl(_tmp87D,(unsigned int)(yyyylsp[0]).first_line);});_tmp3D8->hd=_tmp87E;}),_tmp3D8->tl=0;_tmp3D8;}));yyval=_tmp87F;});
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
({union Cyc_YYSTYPE _tmp880=Cyc_YY16(0);yyval=_tmp880;});
goto _LL0;}case 24U: _LL2B: _LL2C: {
# 1294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1300 "parse.y"
({union Cyc_YYSTYPE _tmp884=Cyc_YY15(({struct _RegionHandle*_tmp883=yyr;struct Cyc_Parse_Declarator _tmp882=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp881=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp883,0,_tmp882,0,_tmp881,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp884;});
goto _LL0;}case 25U: _LL2D: _LL2E: {
# 1303
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1305
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1302 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D9=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp888=Cyc_YY15(({struct _RegionHandle*_tmp887=yyr;struct Cyc_Parse_Declarator _tmp886=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp885=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp887,& _tmp3D9,_tmp886,0,_tmp885,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp888;});
goto _LL0;}case 26U: _LL2F: _LL30: {
# 1306
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1308
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1315 "parse.y"
({union Cyc_YYSTYPE _tmp88D=Cyc_YY15(({struct _RegionHandle*_tmp88C=yyr;struct Cyc_Parse_Declarator _tmp88B=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp88A=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp889=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp88C,0,_tmp88B,_tmp88A,_tmp889,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp88D;});
goto _LL0;}case 27U: _LL31: _LL32: {
# 1318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1317 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DA=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp892=Cyc_YY15(({struct _RegionHandle*_tmp891=yyr;struct Cyc_Parse_Declarator _tmp890=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp88F=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp88E=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp891,& _tmp3DA,_tmp890,_tmp88F,_tmp88E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp892;});
goto _LL0;}case 28U: _LL33: _LL34: {
# 1321
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1323
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1325 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DB=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp896=Cyc_YY15(({struct _RegionHandle*_tmp895=yyr;struct Cyc_Parse_Declarator _tmp894=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp893=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp895,& _tmp3DB,_tmp894,0,_tmp893,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp896;});
goto _LL0;}case 29U: _LL35: _LL36: {
# 1329
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1331
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1328 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3DC=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp89B=Cyc_YY15(({struct _RegionHandle*_tmp89A=yyr;struct Cyc_Parse_Declarator _tmp899=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp898=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp897=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp89A,& _tmp3DC,_tmp899,_tmp898,_tmp897,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp89B;});
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
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct _dyneither_ptr _tmp89C=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp3DD=_tmp89C;});_tmp3DD;}));yyval=yyyyvsp[1];
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
int _tmp3DE=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp89F=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp89E=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp89D=(unsigned int)_tmp3DE;Cyc_Parse_make_declarations(_tmp89E,0,_tmp89D,(unsigned int)_tmp3DE);}));yyval=_tmp89F;});
goto _LL0;}case 35U: _LL41: _LL42: {
# 1352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1351 "parse.y"
int _tmp3DF=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp8A3=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp8A2=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp8A1=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp8A0=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp8A2,_tmp8A1,_tmp8A0,(unsigned int)_tmp3DF);}));yyval=_tmp8A3;});
goto _LL0;}case 36U: _LL43: _LL44: {
# 1355
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1357
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1355 "parse.y"
({union Cyc_YYSTYPE _tmp8A7=Cyc_YY16(({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct Cyc_Absyn_Decl*_tmp8A6=({struct Cyc_Absyn_Pat*_tmp8A5=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp8A4=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp8A5,_tmp8A4,(unsigned int)(yyyylsp[0]).first_line);});_tmp3E0->hd=_tmp8A6;}),_tmp3E0->tl=0;_tmp3E0;}));yyval=_tmp8A7;});
goto _LL0;}case 37U: _LL45: _LL46: {
# 1358
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1360
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1357 "parse.y"
struct Cyc_List_List*_tmp3E1=0;
{struct Cyc_List_List*_tmp3E2=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp3E2 != 0;_tmp3E2=_tmp3E2->tl){
struct _dyneither_ptr*_tmp3E3=(struct _dyneither_ptr*)_tmp3E2->hd;
struct _tuple0*qv=({struct _tuple0*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({union Cyc_Absyn_Nmspace _tmp8A8=Cyc_Absyn_Rel_n(0);_tmp3E6->f1=_tmp8A8;}),_tmp3E6->f2=_tmp3E3;_tmp3E6;});
struct Cyc_Absyn_Vardecl*_tmp3E4=({struct _tuple0*_tmp8A9=qv;Cyc_Absyn_new_vardecl(0U,_tmp8A9,Cyc_Absyn_wildtyp(0),0);});
({struct Cyc_List_List*_tmp8AA=({struct Cyc_List_List*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->hd=_tmp3E4,_tmp3E5->tl=_tmp3E1;_tmp3E5;});_tmp3E1=_tmp8AA;});}}
# 1364
({struct Cyc_List_List*_tmp8AB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3E1);_tmp3E1=_tmp8AB;});
({union Cyc_YYSTYPE _tmp8AD=Cyc_YY16(({struct Cyc_List_List*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));({struct Cyc_Absyn_Decl*_tmp8AC=Cyc_Absyn_letv_decl(_tmp3E1,(unsigned int)(yyyylsp[0]).first_line);_tmp3E7->hd=_tmp8AC;}),_tmp3E7->tl=0;_tmp3E7;}));yyval=_tmp8AD;});
# 1367
goto _LL0;}case 38U: _LL47: _LL48: {
# 1369
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1370 "parse.y"
struct _dyneither_ptr _tmp3E8=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1372
if(({struct _dyneither_ptr _tmp8AE=(struct _dyneither_ptr)_tmp3E8;Cyc_zstrcmp(_tmp8AE,({const char*_tmp3E9="`H";_tag_dyneither(_tmp3E9,sizeof(char),3U);}));})== 0)
({void*_tmp3EA=0U;({unsigned int _tmp8B0=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8AF=({const char*_tmp3EB="bad occurrence of heap region";_tag_dyneither(_tmp3EB,sizeof(char),30U);});Cyc_Warn_err(_tmp8B0,_tmp8AF,_tag_dyneither(_tmp3EA,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp8B1=(struct _dyneither_ptr)_tmp3E8;Cyc_zstrcmp(_tmp8B1,({const char*_tmp3EC="`U";_tag_dyneither(_tmp3EC,sizeof(char),3U);}));})== 0)
({void*_tmp3ED=0U;({unsigned int _tmp8B3=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8B2=({const char*_tmp3EE="bad occurrence of unique region";_tag_dyneither(_tmp3EE,sizeof(char),32U);});Cyc_Warn_err(_tmp8B3,_tmp8B2,_tag_dyneither(_tmp3ED,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));({struct _dyneither_ptr*_tmp8B5=({struct _dyneither_ptr*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));*_tmp3F4=_tmp3E8;_tmp3F4;});_tmp3F5->name=_tmp8B5;}),_tmp3F5->identity=- 1,({void*_tmp8B4=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3F5->kind=_tmp8B4;});_tmp3F5;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->tag=2U,_tmp3F3->f1=tv;_tmp3F3;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8B9=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp8B8=({struct _tuple0*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8B7=({struct _dyneither_ptr*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));({struct _dyneither_ptr _tmp8B6=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp3F0=_tmp8B6;});_tmp3F0;});_tmp3F1->f2=_tmp8B7;});_tmp3F1;});Cyc_Absyn_new_vardecl(_tmp8B9,_tmp8B8,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->tag=15U,_tmp3F2->f1=t;_tmp3F2;}),0);});
({union Cyc_YYSTYPE _tmp8BB=Cyc_YY16(({struct Cyc_List_List*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));({struct Cyc_Absyn_Decl*_tmp8BA=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3EF->hd=_tmp8BA;}),_tmp3EF->tl=0;_tmp3EF;}));yyval=_tmp8BB;});
# 1381
goto _LL0;};}case 39U: _LL49: _LL4A: {
# 1383
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1385
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1383 "parse.y"
struct _dyneither_ptr _tmp3F6=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({struct _dyneither_ptr _tmp8BC=(struct _dyneither_ptr)_tmp3F6;Cyc_zstrcmp(_tmp8BC,({const char*_tmp3F7="H";_tag_dyneither(_tmp3F7,sizeof(char),2U);}));})== 0)
({void*_tmp3F8=0U;({unsigned int _tmp8BE=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp8BD=({const char*_tmp3F9="bad occurrence of heap region `H";_tag_dyneither(_tmp3F9,sizeof(char),33U);});Cyc_Warn_err(_tmp8BE,_tmp8BD,_tag_dyneither(_tmp3F8,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp8BF=(struct _dyneither_ptr)_tmp3F6;Cyc_zstrcmp(_tmp8BF,({const char*_tmp3FA="U";_tag_dyneither(_tmp3FA,sizeof(char),2U);}));})== 0)
({void*_tmp3FB=0U;({unsigned int _tmp8C1=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp8C0=({const char*_tmp3FC="bad occurrence of unique region `U";_tag_dyneither(_tmp3FC,sizeof(char),35U);});Cyc_Warn_err(_tmp8C1,_tmp8C0,_tag_dyneither(_tmp3FB,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp406=_cycalloc(sizeof(*_tmp406));({struct _dyneither_ptr*_tmp8C5=({struct _dyneither_ptr*_tmp405=_cycalloc(sizeof(*_tmp405));({struct _dyneither_ptr _tmp8C4=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp404=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F6);_tmp71A;});void*_tmp402[1U];_tmp402[0]=& _tmp404;({struct _dyneither_ptr _tmp8C3=({const char*_tmp403="`%s";_tag_dyneither(_tmp403,sizeof(char),4U);});Cyc_aprintf(_tmp8C3,_tag_dyneither(_tmp402,sizeof(void*),1U));});});*_tmp405=_tmp8C4;});_tmp405;});_tmp406->name=_tmp8C5;}),_tmp406->identity=- 1,({
void*_tmp8C2=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp406->kind=_tmp8C2;});_tmp406;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->tag=2U,_tmp401->f1=tv;_tmp401;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8C8=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp8C7=({struct _tuple0*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8C6=({struct _dyneither_ptr*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));*_tmp3FE=_tmp3F6;_tmp3FE;});_tmp3FF->f2=_tmp8C6;});_tmp3FF;});Cyc_Absyn_new_vardecl(_tmp8C8,_tmp8C7,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->tag=15U,_tmp400->f1=t;_tmp400;}),0);});
({union Cyc_YYSTYPE _tmp8CA=Cyc_YY16(({struct Cyc_List_List*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));({struct Cyc_Absyn_Decl*_tmp8C9=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3FD->hd=_tmp8C9;}),_tmp3FD->tl=0;_tmp3FD;}));yyval=_tmp8CA;});
# 1394
goto _LL0;};}case 40U: _LL4B: _LL4C: {
# 1396
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1398
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1396 "parse.y"
struct _dyneither_ptr _tmp407=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp408=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp409=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({struct _dyneither_ptr _tmp8CB=(struct _dyneither_ptr)_tmp408;Cyc_strcmp(_tmp8CB,({const char*_tmp40A="open";_tag_dyneither(_tmp40A,sizeof(char),5U);}));})!= 0)({void*_tmp40B=0U;({unsigned int _tmp8CD=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp8CC=({const char*_tmp40C="expecting `open'";_tag_dyneither(_tmp40C,sizeof(char),17U);});Cyc_Warn_err(_tmp8CD,_tmp8CC,_tag_dyneither(_tmp40B,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp416=_cycalloc(sizeof(*_tmp416));({struct _dyneither_ptr*_tmp8D1=({struct _dyneither_ptr*_tmp415=_cycalloc(sizeof(*_tmp415));({struct _dyneither_ptr _tmp8D0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp414=({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0U,_tmp71B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp407);_tmp71B;});void*_tmp412[1U];_tmp412[0]=& _tmp414;({struct _dyneither_ptr _tmp8CF=({const char*_tmp413="`%s";_tag_dyneither(_tmp413,sizeof(char),4U);});Cyc_aprintf(_tmp8CF,_tag_dyneither(_tmp412,sizeof(void*),1U));});});*_tmp415=_tmp8D0;});_tmp415;});_tmp416->name=_tmp8D1;}),_tmp416->identity=- 1,({
void*_tmp8CE=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp416->kind=_tmp8CE;});_tmp416;});
void*t=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->tag=2U,_tmp411->f1=tv;_tmp411;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8D4=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp8D3=({struct _tuple0*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8D2=({struct _dyneither_ptr*_tmp40E=_cycalloc(sizeof(*_tmp40E));*_tmp40E=_tmp407;_tmp40E;});_tmp40F->f2=_tmp8D2;});_tmp40F;});Cyc_Absyn_new_vardecl(_tmp8D4,_tmp8D3,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->tag=15U,_tmp410->f1=t;_tmp410;}),0);});
({union Cyc_YYSTYPE _tmp8D6=Cyc_YY16(({struct Cyc_List_List*_tmp40D=_cycalloc(sizeof(*_tmp40D));({struct Cyc_Absyn_Decl*_tmp8D5=Cyc_Absyn_region_decl(tv,vd,_tmp409,(unsigned int)(yyyylsp[0]).first_line);_tmp40D->hd=_tmp8D5;}),_tmp40D->tl=0;_tmp40D;}));yyval=_tmp8D6;});
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
({union Cyc_YYSTYPE _tmp8D8=Cyc_YY16(({struct Cyc_List_List*_tmp8D7=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8D7,Cyc_yyget_YY16(yyyyvsp[1]));}));yyval=_tmp8D8;});
goto _LL0;}case 43U: _LL51: _LL52: {
# 1415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp8DC=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71C;({enum Cyc_Parse_Storage_class*_tmp8DB=Cyc_yyget_YY20(yyyyvsp[0]);_tmp71C.sc=_tmp8DB;}),({struct Cyc_Absyn_Tqual _tmp8DA=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp71C.tq=_tmp8DA;}),({
struct Cyc_Parse_Type_specifier _tmp8D9=Cyc_Parse_empty_spec(0U);_tmp71C.type_specs=_tmp8D9;}),_tmp71C.is_inline=0,_tmp71C.attributes=0;_tmp71C;}));
# 1418
yyval=_tmp8DC;});
# 1420
goto _LL0;}case 44U: _LL53: _LL54: {
# 1422
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1424
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1421 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp417=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp417.sc != 0)
({void*_tmp418=0U;({unsigned int _tmp8DE=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8DD=({const char*_tmp419="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp419,sizeof(char),73U);});Cyc_Warn_warn(_tmp8DE,_tmp8DD,_tag_dyneither(_tmp418,sizeof(void*),0U));});});
# 1425
({union Cyc_YYSTYPE _tmp8E0=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71D;({enum Cyc_Parse_Storage_class*_tmp8DF=Cyc_yyget_YY20(yyyyvsp[0]);_tmp71D.sc=_tmp8DF;}),_tmp71D.tq=_tmp417.tq,_tmp71D.type_specs=_tmp417.type_specs,_tmp71D.is_inline=_tmp417.is_inline,_tmp71D.attributes=_tmp417.attributes;_tmp71D;}));yyval=_tmp8E0;});
# 1429
goto _LL0;}case 45U: _LL55: _LL56: {
# 1431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1430 "parse.y"
({union Cyc_YYSTYPE _tmp8E3=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71E;_tmp71E.sc=0,({struct Cyc_Absyn_Tqual _tmp8E2=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp71E.tq=_tmp8E2;}),({
struct Cyc_Parse_Type_specifier _tmp8E1=Cyc_yyget_YY21(yyyyvsp[0]);_tmp71E.type_specs=_tmp8E1;}),_tmp71E.is_inline=0,_tmp71E.attributes=0;_tmp71E;}));
# 1430
yyval=_tmp8E3;});
# 1432
goto _LL0;}case 46U: _LL57: _LL58: {
# 1434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1433 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41A=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8E7=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp71F;_tmp71F.sc=_tmp41A.sc,_tmp71F.tq=_tmp41A.tq,({
struct Cyc_Parse_Type_specifier _tmp8E6=({unsigned int _tmp8E5=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp8E4=_tmp41A.type_specs;Cyc_Parse_combine_specifiers(_tmp8E5,_tmp8E4,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp71F.type_specs=_tmp8E6;}),_tmp71F.is_inline=_tmp41A.is_inline,_tmp71F.attributes=_tmp41A.attributes;_tmp71F;}));
# 1434
yyval=_tmp8E7;});
# 1439
goto _LL0;}case 47U: _LL59: _LL5A: {
# 1441
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1443
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1440 "parse.y"
({union Cyc_YYSTYPE _tmp8EA=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp720;_tmp720.sc=0,({struct Cyc_Absyn_Tqual _tmp8E9=Cyc_yyget_YY23(yyyyvsp[0]);_tmp720.tq=_tmp8E9;}),({struct Cyc_Parse_Type_specifier _tmp8E8=Cyc_Parse_empty_spec(0U);_tmp720.type_specs=_tmp8E8;}),_tmp720.is_inline=0,_tmp720.attributes=0;_tmp720;}));yyval=_tmp8EA;});
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1443
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1445
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1442 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41B=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8ED=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp721;_tmp721.sc=_tmp41B.sc,({struct Cyc_Absyn_Tqual _tmp8EC=({struct Cyc_Absyn_Tqual _tmp8EB=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp8EB,_tmp41B.tq);});_tmp721.tq=_tmp8EC;}),_tmp721.type_specs=_tmp41B.type_specs,_tmp721.is_inline=_tmp41B.is_inline,_tmp721.attributes=_tmp41B.attributes;_tmp721;}));yyval=_tmp8ED;});
# 1447
goto _LL0;}case 49U: _LL5D: _LL5E: {
# 1449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1448 "parse.y"
({union Cyc_YYSTYPE _tmp8F0=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp722;_tmp722.sc=0,({struct Cyc_Absyn_Tqual _tmp8EF=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp722.tq=_tmp8EF;}),({
struct Cyc_Parse_Type_specifier _tmp8EE=Cyc_Parse_empty_spec(0U);_tmp722.type_specs=_tmp8EE;}),_tmp722.is_inline=1,_tmp722.attributes=0;_tmp722;}));
# 1448
yyval=_tmp8F0;});
# 1450
goto _LL0;}case 50U: _LL5F: _LL60: {
# 1452
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1454
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1451 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41C=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8F1=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp723;_tmp723.sc=_tmp41C.sc,_tmp723.tq=_tmp41C.tq,_tmp723.type_specs=_tmp41C.type_specs,_tmp723.is_inline=1,_tmp723.attributes=_tmp41C.attributes;_tmp723;}));yyval=_tmp8F1;});
# 1455
goto _LL0;}case 51U: _LL61: _LL62: {
# 1457
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1456 "parse.y"
({union Cyc_YYSTYPE _tmp8F5=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp724;_tmp724.sc=0,({struct Cyc_Absyn_Tqual _tmp8F4=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp724.tq=_tmp8F4;}),({
struct Cyc_Parse_Type_specifier _tmp8F3=Cyc_Parse_empty_spec(0U);_tmp724.type_specs=_tmp8F3;}),_tmp724.is_inline=0,({struct Cyc_List_List*_tmp8F2=Cyc_yyget_YY45(yyyyvsp[0]);_tmp724.attributes=_tmp8F2;});_tmp724;}));
# 1456
yyval=_tmp8F5;});
# 1458
goto _LL0;}case 52U: _LL63: _LL64: {
# 1460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1459 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp41D=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8F8=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp725;_tmp725.sc=_tmp41D.sc,_tmp725.tq=_tmp41D.tq,_tmp725.type_specs=_tmp41D.type_specs,_tmp725.is_inline=_tmp41D.is_inline,({
# 1462
struct Cyc_List_List*_tmp8F7=({struct Cyc_List_List*_tmp8F6=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8F6,_tmp41D.attributes);});_tmp725.attributes=_tmp8F7;});_tmp725;}));
# 1460
yyval=_tmp8F8;});
# 1463
goto _LL0;}case 53U: _LL65: _LL66: {
# 1465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1466 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
({union Cyc_YYSTYPE _tmp8F9=Cyc_YY20(& auto_sc);yyval=_tmp8F9;});
goto _LL0;}case 54U: _LL67: _LL68: {
# 1470
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1472
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1468 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
({union Cyc_YYSTYPE _tmp8FA=Cyc_YY20(& register_sc);yyval=_tmp8FA;});
goto _LL0;}case 55U: _LL69: _LL6A: {
# 1472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1470 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
({union Cyc_YYSTYPE _tmp8FB=Cyc_YY20(& static_sc);yyval=_tmp8FB;});
goto _LL0;}case 56U: _LL6B: _LL6C: {
# 1474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1472 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
({union Cyc_YYSTYPE _tmp8FC=Cyc_YY20(& extern_sc);yyval=_tmp8FC;});
goto _LL0;}case 57U: _LL6D: _LL6E: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1475 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _dyneither_ptr _tmp8FD=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmp8FD,({const char*_tmp41E="C";_tag_dyneither(_tmp41E,sizeof(char),2U);}));})!= 0)
({void*_tmp41F=0U;({unsigned int _tmp8FF=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8FE=({const char*_tmp420="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp420,sizeof(char),37U);});Cyc_Warn_err(_tmp8FF,_tmp8FE,_tag_dyneither(_tmp41F,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp900=Cyc_YY20(& externC_sc);yyval=_tmp900;});
# 1480
goto _LL0;}case 58U: _LL6F: _LL70: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1480 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
({union Cyc_YYSTYPE _tmp901=Cyc_YY20(& typedef_sc);yyval=_tmp901;});
goto _LL0;}case 59U: _LL71: _LL72: {
# 1484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1483 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
({union Cyc_YYSTYPE _tmp902=Cyc_YY20(& abstract_sc);yyval=_tmp902;});
goto _LL0;}case 60U: _LL73: _LL74:
# 1487
({union Cyc_YYSTYPE _tmp903=Cyc_YY45(0);yyval=_tmp903;});
goto _LL0;case 61U: _LL75: _LL76: {
# 1490
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1492
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1490 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 62U: _LL77: _LL78: {
# 1493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1495 "parse.y"
yyval=yyyyvsp[3];
goto _LL0;}case 63U: _LL79: _LL7A: {
# 1498
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1500
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1499 "parse.y"
({union Cyc_YYSTYPE _tmp905=Cyc_YY45(({struct Cyc_List_List*_tmp421=_cycalloc(sizeof(*_tmp421));({void*_tmp904=Cyc_yyget_YY46(yyyyvsp[0]);_tmp421->hd=_tmp904;}),_tmp421->tl=0;_tmp421;}));yyval=_tmp905;});
goto _LL0;}case 64U: _LL7B: _LL7C: {
# 1502
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1504
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1500 "parse.y"
({union Cyc_YYSTYPE _tmp908=Cyc_YY45(({struct Cyc_List_List*_tmp422=_cycalloc(sizeof(*_tmp422));({void*_tmp907=Cyc_yyget_YY46(yyyyvsp[0]);_tmp422->hd=_tmp907;}),({struct Cyc_List_List*_tmp906=Cyc_yyget_YY45(yyyyvsp[2]);_tmp422->tl=_tmp906;});_tmp422;}));yyval=_tmp908;});
goto _LL0;}case 65U: _LL7D: _LL7E: {
# 1503
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1505
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1505 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp426,_tmp426,_tmp426 + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp427,_tmp427,_tmp427 + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp428,_tmp428,_tmp428 + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp429,_tmp429,_tmp429 + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp42A,_tmp42A,_tmp42A + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp42B,_tmp42B,_tmp42B + 8U},(void*)& att_aligned},{{_tmp42C,_tmp42C,_tmp42C + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp42D,_tmp42D,_tmp42D + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp42E,_tmp42E,_tmp42E + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp42F,_tmp42F,_tmp42F + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp430,_tmp430,_tmp430 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp431,_tmp431,_tmp431 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp432,_tmp432,_tmp432 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp433,_tmp433,_tmp433 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp434,_tmp434,_tmp434 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp435,_tmp435,_tmp435 + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp436,_tmp436,_tmp436 + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp437,_tmp437,_tmp437 + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1526
struct _dyneither_ptr _tmp423=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1528
if((((_get_dyneither_size(_tmp423,sizeof(char))> 4  && ((const char*)_tmp423.curr)[0]== '_') && ((const char*)_tmp423.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp423,sizeof(char),(int)(_get_dyneither_size(_tmp423,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp423,sizeof(char),(int)(_get_dyneither_size(_tmp423,sizeof(char))- 3)))== '_')
# 1530
({struct _dyneither_ptr _tmp909=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp423,2,_get_dyneither_size(_tmp423,sizeof(char))- 5);_tmp423=_tmp909;});{
int i=0;
for(0;i < 18U;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp423,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18U,i)]).f1)== 0){
({union Cyc_YYSTYPE _tmp90A=Cyc_YY46((att_map[i]).f2);yyval=_tmp90A;});
break;}}
# 1537
if(i == 18U){
({void*_tmp424=0U;({unsigned int _tmp90C=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp90B=({const char*_tmp425="unrecognized attribute";_tag_dyneither(_tmp425,sizeof(char),23U);});Cyc_Warn_err(_tmp90C,_tmp90B,_tag_dyneither(_tmp424,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp90D=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);yyval=_tmp90D;});}
# 1542
goto _LL0;};}case 66U: _LL7F: _LL80: {
# 1544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1542 "parse.y"
({union Cyc_YYSTYPE _tmp90E=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);yyval=_tmp90E;});
goto _LL0;}case 67U: _LL81: _LL82: {
# 1545
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1544 "parse.y"
struct _dyneither_ptr _tmp438=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp439=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({struct _dyneither_ptr _tmp910=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp910,({const char*_tmp43A="aligned";_tag_dyneither(_tmp43A,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp90F=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp90F,({const char*_tmp43B="__aligned__";_tag_dyneither(_tmp43B,sizeof(char),12U);}));})== 0)
({void*_tmp911=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=6U,_tmp43C->f1=_tmp439;_tmp43C;});a=_tmp911;});else{
if(({struct _dyneither_ptr _tmp913=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp913,({const char*_tmp43D="section";_tag_dyneither(_tmp43D,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp912=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp912,({const char*_tmp43E="__section__";_tag_dyneither(_tmp43E,sizeof(char),12U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp439);
({void*_tmp914=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=8U,_tmp43F->f1=str;_tmp43F;});a=_tmp914;});}else{
if(({struct _dyneither_ptr _tmp915=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp915,({const char*_tmp440="__mode__";_tag_dyneither(_tmp440,sizeof(char),9U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp439);
({void*_tmp916=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp441=_cycalloc(sizeof(*_tmp441));_tmp441->tag=24U,_tmp441->f1=str;_tmp441;});a=_tmp916;});}else{
if(({struct _dyneither_ptr _tmp917=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp917,({const char*_tmp442="alias";_tag_dyneither(_tmp442,sizeof(char),6U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp439);
({void*_tmp918=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->tag=25U,_tmp443->f1=str;_tmp443;});a=_tmp918;});}else{
# 1560
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp439);
if(({struct _dyneither_ptr _tmp91A=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp91A,({const char*_tmp444="regparm";_tag_dyneither(_tmp444,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp919=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp919,({const char*_tmp445="__regparm__";_tag_dyneither(_tmp445,sizeof(char),12U);}));})== 0){
if(n < 0  || n > 3)
({void*_tmp446=0U;({unsigned int _tmp91C=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp91B=({const char*_tmp447="regparm requires value between 0 and 3";_tag_dyneither(_tmp447,sizeof(char),39U);});Cyc_Warn_err(_tmp91C,_tmp91B,_tag_dyneither(_tmp446,sizeof(void*),0U));});});
({void*_tmp91D=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp448=_cycalloc(sizeof(*_tmp448));_tmp448->tag=0U,_tmp448->f1=n;_tmp448;});a=_tmp91D;});}else{
if(({struct _dyneither_ptr _tmp91F=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp91F,({const char*_tmp449="initializes";_tag_dyneither(_tmp449,sizeof(char),12U);}));})== 0  || ({struct _dyneither_ptr _tmp91E=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp91E,({const char*_tmp44A="__initializes__";_tag_dyneither(_tmp44A,sizeof(char),16U);}));})== 0)
({void*_tmp920=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=20U,_tmp44B->f1=n;_tmp44B;});a=_tmp920;});else{
if(({struct _dyneither_ptr _tmp922=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp922,({const char*_tmp44C="noliveunique";_tag_dyneither(_tmp44C,sizeof(char),13U);}));})== 0  || ({struct _dyneither_ptr _tmp921=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp921,({const char*_tmp44D="__noliveunique__";_tag_dyneither(_tmp44D,sizeof(char),17U);}));})== 0)
({void*_tmp923=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->tag=21U,_tmp44E->f1=n;_tmp44E;});a=_tmp923;});else{
if(({struct _dyneither_ptr _tmp925=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp925,({const char*_tmp44F="noconsume";_tag_dyneither(_tmp44F,sizeof(char),10U);}));})== 0  || ({struct _dyneither_ptr _tmp924=(struct _dyneither_ptr)_tmp438;Cyc_zstrcmp(_tmp924,({const char*_tmp450="__noconsume__";_tag_dyneither(_tmp450,sizeof(char),14U);}));})== 0)
({void*_tmp926=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=22U,_tmp451->f1=n;_tmp451;});a=_tmp926;});else{
# 1572
({void*_tmp452=0U;({unsigned int _tmp928=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp927=({const char*_tmp453="unrecognized attribute";_tag_dyneither(_tmp453,sizeof(char),23U);});Cyc_Warn_err(_tmp928,_tmp927,_tag_dyneither(_tmp452,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1576
({union Cyc_YYSTYPE _tmp929=Cyc_YY46(a);yyval=_tmp929;});
# 1578
goto _LL0;}case 68U: _LL83: _LL84: {
# 1580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1579 "parse.y"
struct _dyneither_ptr _tmp454=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp455=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp456=({unsigned int _tmp92A=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmp92A,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp457=({unsigned int _tmp92B=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmp92B,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _dyneither_ptr _tmp92D=(struct _dyneither_ptr)_tmp454;Cyc_zstrcmp(_tmp92D,({const char*_tmp458="format";_tag_dyneither(_tmp458,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp92C=(struct _dyneither_ptr)_tmp454;Cyc_zstrcmp(_tmp92C,({const char*_tmp459="__format__";_tag_dyneither(_tmp459,sizeof(char),11U);}));})== 0){
if(({struct _dyneither_ptr _tmp92F=(struct _dyneither_ptr)_tmp455;Cyc_zstrcmp(_tmp92F,({const char*_tmp45A="printf";_tag_dyneither(_tmp45A,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp92E=(struct _dyneither_ptr)_tmp455;Cyc_zstrcmp(_tmp92E,({const char*_tmp45B="__printf__";_tag_dyneither(_tmp45B,sizeof(char),11U);}));})== 0)
({void*_tmp930=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=19U,_tmp45C->f1=Cyc_Absyn_Printf_ft,_tmp45C->f2=(int)_tmp456,_tmp45C->f3=(int)_tmp457;_tmp45C;});a=_tmp930;});else{
if(({struct _dyneither_ptr _tmp932=(struct _dyneither_ptr)_tmp455;Cyc_zstrcmp(_tmp932,({const char*_tmp45D="scanf";_tag_dyneither(_tmp45D,sizeof(char),6U);}));})== 0  || ({struct _dyneither_ptr _tmp931=(struct _dyneither_ptr)_tmp455;Cyc_zstrcmp(_tmp931,({const char*_tmp45E="__scanf__";_tag_dyneither(_tmp45E,sizeof(char),10U);}));})== 0)
({void*_tmp933=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->tag=19U,_tmp45F->f1=Cyc_Absyn_Scanf_ft,_tmp45F->f2=(int)_tmp456,_tmp45F->f3=(int)_tmp457;_tmp45F;});a=_tmp933;});else{
# 1590
({void*_tmp460=0U;({unsigned int _tmp935=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp934=({const char*_tmp461="unrecognized format type";_tag_dyneither(_tmp461,sizeof(char),25U);});Cyc_Warn_err(_tmp935,_tmp934,_tag_dyneither(_tmp460,sizeof(void*),0U));});});}}}else{
# 1592
({void*_tmp462=0U;({unsigned int _tmp937=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp936=({const char*_tmp463="unrecognized attribute";_tag_dyneither(_tmp463,sizeof(char),23U);});Cyc_Warn_err(_tmp937,_tmp936,_tag_dyneither(_tmp462,sizeof(void*),0U));});});}
({union Cyc_YYSTYPE _tmp938=Cyc_YY46(a);yyval=_tmp938;});
# 1595
goto _LL0;}case 69U: _LL85: _LL86: {
# 1597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1604 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 70U: _LL87: _LL88: {
# 1607
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1609
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1606 "parse.y"
({union Cyc_YYSTYPE _tmp93C=Cyc_YY21(({void*_tmp93B=(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp464=_cycalloc(sizeof(*_tmp464));_tmp464->tag=17U,({struct _tuple0*_tmp93A=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp464->f1=_tmp93A;}),({struct Cyc_List_List*_tmp939=Cyc_yyget_YY40(yyyyvsp[1]);_tmp464->f2=_tmp939;}),_tmp464->f3=0,_tmp464->f4=0;_tmp464;});Cyc_Parse_type_spec(_tmp93B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp93C;});
goto _LL0;}case 71U: _LL89: _LL8A: {
# 1609
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1611
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1610 "parse.y"
({union Cyc_YYSTYPE _tmp93D=Cyc_YY21(Cyc_Parse_type_spec((void*)& Cyc_Absyn_VoidType_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp93D;});
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1611 "parse.y"
({union Cyc_YYSTYPE _tmp93E=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp93E;});
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1612 "parse.y"
({union Cyc_YYSTYPE _tmp93F=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp93F;});
goto _LL0;}case 74U: _LL8F: _LL90: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1613 "parse.y"
({union Cyc_YYSTYPE _tmp940=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_typ,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp940;});
goto _LL0;}case 75U: _LL91: _LL92: {
# 1616
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1618
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1614 "parse.y"
({union Cyc_YYSTYPE _tmp941=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp941;});
goto _LL0;}case 76U: _LL93: _LL94: {
# 1617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1615 "parse.y"
({union Cyc_YYSTYPE _tmp943=Cyc_YY21(({void*_tmp942=Cyc_Absyn_float_typ(0);Cyc_Parse_type_spec(_tmp942,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp943;});
goto _LL0;}case 77U: _LL95: _LL96: {
# 1618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1616 "parse.y"
({union Cyc_YYSTYPE _tmp945=Cyc_YY21(({void*_tmp944=Cyc_Absyn_float_typ(1);Cyc_Parse_type_spec(_tmp944,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp945;});
goto _LL0;}case 78U: _LL97: _LL98: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1617 "parse.y"
({union Cyc_YYSTYPE _tmp946=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp946;});
goto _LL0;}case 79U: _LL99: _LL9A: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1618 "parse.y"
({union Cyc_YYSTYPE _tmp947=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp947;});
goto _LL0;}case 80U: _LL9B: _LL9C: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1619 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 81U: _LL9D: _LL9E: {
# 1622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1620 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1623 "parse.y"
({union Cyc_YYSTYPE _tmp94A=Cyc_YY21(({void*_tmp949=(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp465=_cycalloc(sizeof(*_tmp465));_tmp465->tag=27U,({struct Cyc_Absyn_Exp*_tmp948=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp465->f1=_tmp948;});_tmp465;});Cyc_Parse_type_spec(_tmp949,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp94A;});
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1625 "parse.y"
({union Cyc_YYSTYPE _tmp94D=Cyc_YY21(({void*_tmp94C=(void*)({struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=28U,({struct _dyneither_ptr _tmp94B=({const char*_tmp466="__builtin_va_list";_tag_dyneither(_tmp466,sizeof(char),18U);});_tmp467->f1=_tmp94B;}),_tmp467->f2=& Cyc_Tcutil_bk;_tmp467;});Cyc_Parse_type_spec(_tmp94C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp94D;});
goto _LL0;}case 84U: _LLA3: _LLA4: {
# 1628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1627 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 85U: _LLA5: _LLA6: {
# 1630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1629 "parse.y"
({union Cyc_YYSTYPE _tmp94F=Cyc_YY21(({void*_tmp94E=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmp94E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp94F;});
goto _LL0;}case 86U: _LLA7: _LLA8: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1631 "parse.y"
({union Cyc_YYSTYPE _tmp951=Cyc_YY21(({void*_tmp950=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp950,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp951;});
goto _LL0;}case 87U: _LLA9: _LLAA: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1633 "parse.y"
({union Cyc_YYSTYPE _tmp953=Cyc_YY21(({void*_tmp952=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);Cyc_Parse_type_spec(_tmp952,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp953;});
goto _LL0;}case 88U: _LLAB: _LLAC: {
# 1636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1635 "parse.y"
({union Cyc_YYSTYPE _tmp957=Cyc_YY21(({void*_tmp956=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->tag=10U,({struct Cyc_List_List*_tmp955=({unsigned int _tmp954=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp954,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});
# 1635
_tmp468->f1=_tmp955;});_tmp468;});Cyc_Parse_type_spec(_tmp956,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp957;});
# 1638
goto _LL0;}case 89U: _LLAD: _LLAE: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1639 "parse.y"
({union Cyc_YYSTYPE _tmp95A=Cyc_YY21(({void*_tmp959=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp469=_cycalloc(sizeof(*_tmp469));_tmp469->tag=15U,({void*_tmp958=Cyc_yyget_YY44(yyyyvsp[2]);_tmp469->f1=_tmp958;});_tmp469;});Cyc_Parse_type_spec(_tmp959,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp95A;});
goto _LL0;}case 90U: _LLAF: _LLB0: {
# 1642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1641 "parse.y"
({union Cyc_YYSTYPE _tmp95D=Cyc_YY21(({void*_tmp95C=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=15U,({void*_tmp95B=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);_tmp46A->f1=_tmp95B;});_tmp46A;});Cyc_Parse_type_spec(_tmp95C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp95D;});
# 1643
goto _LL0;}case 91U: _LLB1: _LLB2: {
# 1645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1644 "parse.y"
void*_tmp46B=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0);
({union Cyc_YYSTYPE _tmp960=Cyc_YY21(({void*_tmp95F=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->tag=16U,({void*_tmp95E=Cyc_yyget_YY44(yyyyvsp[2]);_tmp46C->f1=_tmp95E;}),_tmp46C->f2=_tmp46B;_tmp46C;});Cyc_Parse_type_spec(_tmp95F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp960;});
# 1647
goto _LL0;}case 92U: _LLB3: _LLB4: {
# 1649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1648 "parse.y"
({union Cyc_YYSTYPE _tmp964=Cyc_YY21(({void*_tmp963=(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));_tmp46D->tag=16U,({void*_tmp962=Cyc_yyget_YY44(yyyyvsp[2]);_tmp46D->f1=_tmp962;}),({void*_tmp961=Cyc_yyget_YY44(yyyyvsp[4]);_tmp46D->f2=_tmp961;});_tmp46D;});Cyc_Parse_type_spec(_tmp963,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp964;});
goto _LL0;}case 93U: _LLB5: _LLB6: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1650 "parse.y"
({union Cyc_YYSTYPE _tmp967=Cyc_YY21(({void*_tmp966=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=19U,({void*_tmp965=Cyc_yyget_YY44(yyyyvsp[2]);_tmp46E->f1=_tmp965;});_tmp46E;});Cyc_Parse_type_spec(_tmp966,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp967;});
goto _LL0;}case 94U: _LLB7: _LLB8: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1652 "parse.y"
({union Cyc_YYSTYPE _tmp96A=Cyc_YY21(({void*_tmp969=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->tag=19U,({void*_tmp968=Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0);_tmp46F->f1=_tmp968;});_tmp46F;});Cyc_Parse_type_spec(_tmp969,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp96A;});
goto _LL0;}case 95U: _LLB9: _LLBA: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1654 "parse.y"
({union Cyc_YYSTYPE _tmp96D=Cyc_YY21(({void*_tmp96C=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=18U,({struct Cyc_Absyn_Exp*_tmp96B=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp470->f1=_tmp96B;});_tmp470;});Cyc_Parse_type_spec(_tmp96C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp96D;});
goto _LL0;}case 96U: _LLBB: _LLBC: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp96F=Cyc_YY43(({struct _dyneither_ptr _tmp96E=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmp96E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp96F;});
goto _LL0;}case 97U: _LLBD: _LLBE: {
# 1663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1664 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0U);
({union Cyc_YYSTYPE _tmp970=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp726;_tmp726.print_const=1,_tmp726.q_volatile=0,_tmp726.q_restrict=0,_tmp726.real_const=1,_tmp726.loc=loc;_tmp726;}));yyval=_tmp970;});
goto _LL0;}case 98U: _LLBF: _LLC0: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1666 "parse.y"
({union Cyc_YYSTYPE _tmp971=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp727;_tmp727.print_const=0,_tmp727.q_volatile=1,_tmp727.q_restrict=0,_tmp727.real_const=0,_tmp727.loc=0U;_tmp727;}));yyval=_tmp971;});
goto _LL0;}case 99U: _LLC1: _LLC2: {
# 1669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1667 "parse.y"
({union Cyc_YYSTYPE _tmp972=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp728;_tmp728.print_const=0,_tmp728.q_volatile=0,_tmp728.q_restrict=1,_tmp728.real_const=0,_tmp728.loc=0U;_tmp728;}));yyval=_tmp972;});
goto _LL0;}case 100U: _LLC3: _LLC4: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
struct Cyc_Absyn_TypeDecl*_tmp471=({struct Cyc_Absyn_TypeDecl*_tmp476=_cycalloc(sizeof(*_tmp476));({void*_tmp977=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp475=_cycalloc(sizeof(*_tmp475));_tmp475->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp976=({struct Cyc_Absyn_Enumdecl*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp975=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp474->name=_tmp975;}),({struct Cyc_Core_Opt*_tmp974=({struct Cyc_Core_Opt*_tmp473=_cycalloc(sizeof(*_tmp473));({struct Cyc_List_List*_tmp973=Cyc_yyget_YY48(yyyyvsp[3]);_tmp473->v=_tmp973;});_tmp473;});_tmp474->fields=_tmp974;});_tmp474;});_tmp475->f1=_tmp976;});_tmp475;});_tmp476->r=_tmp977;}),_tmp476->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp476;});
# 1675
({union Cyc_YYSTYPE _tmp979=Cyc_YY21(({void*_tmp978=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->tag=26U,_tmp472->f1=_tmp471,_tmp472->f2=0;_tmp472;});Cyc_Parse_type_spec(_tmp978,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp979;});
# 1677
goto _LL0;}case 101U: _LLC5: _LLC6: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1678 "parse.y"
({union Cyc_YYSTYPE _tmp97C=Cyc_YY21(({void*_tmp97B=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=13U,({struct _tuple0*_tmp97A=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp477->f1=_tmp97A;}),_tmp477->f2=0;_tmp477;});Cyc_Parse_type_spec(_tmp97B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp97C;});
goto _LL0;}case 102U: _LLC7: _LLC8: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1680 "parse.y"
({union Cyc_YYSTYPE _tmp97F=Cyc_YY21(({void*_tmp97E=(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->tag=14U,({struct Cyc_List_List*_tmp97D=Cyc_yyget_YY48(yyyyvsp[2]);_tmp478->f1=_tmp97D;});_tmp478;});Cyc_Parse_type_spec(_tmp97E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp97F;});
goto _LL0;}case 103U: _LLC9: _LLCA: {
# 1683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp981=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp479=_cycalloc(sizeof(*_tmp479));({struct _tuple0*_tmp980=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp479->name=_tmp980;}),_tmp479->tag=0,_tmp479->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp479;}));yyval=_tmp981;});
goto _LL0;}case 104U: _LLCB: _LLCC: {
# 1689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1688 "parse.y"
({union Cyc_YYSTYPE _tmp984=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp47A=_cycalloc(sizeof(*_tmp47A));({struct _tuple0*_tmp983=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp47A->name=_tmp983;}),({struct Cyc_Absyn_Exp*_tmp982=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp47A->tag=_tmp982;}),_tmp47A->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp47A;}));yyval=_tmp984;});
goto _LL0;}case 105U: _LLCD: _LLCE: {
# 1691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1692 "parse.y"
({union Cyc_YYSTYPE _tmp986=Cyc_YY48(({struct Cyc_List_List*_tmp47B=_cycalloc(sizeof(*_tmp47B));({struct Cyc_Absyn_Enumfield*_tmp985=Cyc_yyget_YY47(yyyyvsp[0]);_tmp47B->hd=_tmp985;}),_tmp47B->tl=0;_tmp47B;}));yyval=_tmp986;});
goto _LL0;}case 106U: _LLCF: _LLD0: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1693 "parse.y"
({union Cyc_YYSTYPE _tmp988=Cyc_YY48(({struct Cyc_List_List*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct Cyc_Absyn_Enumfield*_tmp987=Cyc_yyget_YY47(yyyyvsp[0]);_tmp47C->hd=_tmp987;}),_tmp47C->tl=0;_tmp47C;}));yyval=_tmp988;});
goto _LL0;}case 107U: _LLD1: _LLD2: {
# 1696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1694 "parse.y"
({union Cyc_YYSTYPE _tmp98B=Cyc_YY48(({struct Cyc_List_List*_tmp47D=_cycalloc(sizeof(*_tmp47D));({struct Cyc_Absyn_Enumfield*_tmp98A=Cyc_yyget_YY47(yyyyvsp[0]);_tmp47D->hd=_tmp98A;}),({struct Cyc_List_List*_tmp989=Cyc_yyget_YY48(yyyyvsp[2]);_tmp47D->tl=_tmp989;});_tmp47D;}));yyval=_tmp98B;});
goto _LL0;}case 108U: _LLD3: _LLD4: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
({union Cyc_YYSTYPE _tmp98F=Cyc_YY21(({void*_tmp98E=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->tag=12U,({enum Cyc_Absyn_AggrKind _tmp98D=Cyc_yyget_YY22(yyyyvsp[0]);_tmp47E->f1=_tmp98D;}),({struct Cyc_List_List*_tmp98C=Cyc_yyget_YY24(yyyyvsp[2]);_tmp47E->f2=_tmp98C;});_tmp47E;});Cyc_Parse_type_spec(_tmp98E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp98F;});
goto _LL0;}case 109U: _LLD5: _LLD6: {
# 1703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 1705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 1706 "parse.y"
struct Cyc_List_List*_tmp47F=({unsigned int _tmp990=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp990,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp480=({unsigned int _tmp991=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp991,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp481=({enum Cyc_Absyn_AggrKind _tmp997=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp996=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmp995=_tmp47F;struct Cyc_Absyn_AggrdeclImpl*_tmp994=({
struct Cyc_List_List*_tmp993=_tmp480;struct Cyc_List_List*_tmp992=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmp993,_tmp992,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1708
Cyc_Absyn_aggr_tdecl(_tmp997,Cyc_Absyn_Public,_tmp996,_tmp995,_tmp994,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1711
({union Cyc_YYSTYPE _tmp999=Cyc_YY21(({void*_tmp998=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->tag=26U,_tmp482->f1=_tmp481,_tmp482->f2=0;_tmp482;});Cyc_Parse_type_spec(_tmp998,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp999;});
# 1713
goto _LL0;}case 110U: _LLD7: _LLD8: {
# 1715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1717 "parse.y"
struct Cyc_List_List*_tmp483=({unsigned int _tmp99A=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp99A,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp484=({unsigned int _tmp99B=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp99B,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp485=({enum Cyc_Absyn_AggrKind _tmp9A1=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmp9A0=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp99F=_tmp483;struct Cyc_Absyn_AggrdeclImpl*_tmp99E=({
struct Cyc_List_List*_tmp99D=_tmp484;struct Cyc_List_List*_tmp99C=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmp99D,_tmp99C,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1719
Cyc_Absyn_aggr_tdecl(_tmp9A1,Cyc_Absyn_Public,_tmp9A0,_tmp99F,_tmp99E,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1722
({union Cyc_YYSTYPE _tmp9A3=Cyc_YY21(({void*_tmp9A2=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp486=_cycalloc(sizeof(*_tmp486));_tmp486->tag=26U,_tmp486->f1=_tmp485,_tmp486->f2=0;_tmp486;});Cyc_Parse_type_spec(_tmp9A2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9A3;});
# 1724
goto _LL0;}case 111U: _LLD9: _LLDA: {
# 1726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1725 "parse.y"
({union Cyc_YYSTYPE _tmp9A9=Cyc_YY21(({void*_tmp9A8=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp9A7=({enum Cyc_Absyn_AggrKind _tmp9A6=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp9A5=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmp9A6,_tmp9A5,({struct Cyc_Core_Opt*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->v=(void*)1;_tmp487;}));});(_tmp488->f1).aggr_info=_tmp9A7;}),({struct Cyc_List_List*_tmp9A4=Cyc_yyget_YY40(yyyyvsp[3]);(_tmp488->f1).targs=_tmp9A4;});_tmp488;});Cyc_Parse_type_spec(_tmp9A8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9A9;});
# 1728
goto _LL0;}case 112U: _LLDB: _LLDC: {
# 1730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1729 "parse.y"
({union Cyc_YYSTYPE _tmp9AE=Cyc_YY21(({void*_tmp9AD=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp9AC=({enum Cyc_Absyn_AggrKind _tmp9AB=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmp9AB,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});(_tmp489->f1).aggr_info=_tmp9AC;}),({struct Cyc_List_List*_tmp9AA=Cyc_yyget_YY40(yyyyvsp[2]);(_tmp489->f1).targs=_tmp9AA;});_tmp489;});Cyc_Parse_type_spec(_tmp9AD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9AE;});
# 1732
goto _LL0;}case 113U: _LLDD: _LLDE:
# 1734
({union Cyc_YYSTYPE _tmp9AF=Cyc_YY40(0);yyval=_tmp9AF;});
goto _LL0;case 114U: _LLDF: _LLE0: {
# 1737
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1739
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1738 "parse.y"
({union Cyc_YYSTYPE _tmp9B0=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));yyval=_tmp9B0;});
goto _LL0;}case 115U: _LLE1: _LLE2: {
# 1741
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1743
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1742 "parse.y"
({union Cyc_YYSTYPE _tmp9B1=Cyc_YY22(Cyc_Absyn_StructA);yyval=_tmp9B1;});
goto _LL0;}case 116U: _LLE3: _LLE4: {
# 1745
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1747
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1743 "parse.y"
({union Cyc_YYSTYPE _tmp9B2=Cyc_YY22(Cyc_Absyn_UnionA);yyval=_tmp9B2;});
goto _LL0;}case 117U: _LLE5: _LLE6:
# 1746
({union Cyc_YYSTYPE _tmp9B3=Cyc_YY24(0);yyval=_tmp9B3;});
goto _LL0;case 118U: _LLE7: _LLE8: {
# 1749
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1750 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp48A=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp48A != 0;_tmp48A=_tmp48A->tl){
({struct Cyc_List_List*_tmp9B4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp48A->hd,decls);decls=_tmp9B4;});}}{
# 1754
struct Cyc_List_List*_tmp48B=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp48B != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp48B,decls);
({union Cyc_YYSTYPE _tmp9B5=Cyc_YY24(decls);yyval=_tmp9B5;});
# 1759
goto _LL0;};}case 119U: _LLE9: _LLEA: {
# 1761
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9B7=Cyc_YY25(({struct Cyc_List_List*_tmp48C=_cycalloc(sizeof(*_tmp48C));({struct Cyc_List_List*_tmp9B6=Cyc_yyget_YY24(yyyyvsp[0]);_tmp48C->hd=_tmp9B6;}),_tmp48C->tl=0;_tmp48C;}));yyval=_tmp9B7;});
goto _LL0;}case 120U: _LLEB: _LLEC: {
# 1767
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1769
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1766 "parse.y"
({union Cyc_YYSTYPE _tmp9BA=Cyc_YY25(({struct Cyc_List_List*_tmp48D=_cycalloc(sizeof(*_tmp48D));({struct Cyc_List_List*_tmp9B9=Cyc_yyget_YY24(yyyyvsp[1]);_tmp48D->hd=_tmp9B9;}),({struct Cyc_List_List*_tmp9B8=Cyc_yyget_YY25(yyyyvsp[0]);_tmp48D->tl=_tmp9B8;});_tmp48D;}));yyval=_tmp9BA;});
goto _LL0;}case 121U: _LLED: _LLEE: {
# 1769
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1771
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1770 "parse.y"
({union Cyc_YYSTYPE _tmp9BB=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));yyval=_tmp9BB;});
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1773
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1775
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9BD=Cyc_YY19(({struct _tuple12*_tmp48E=_region_malloc(yyr,sizeof(*_tmp48E));_tmp48E->tl=0,({struct _tuple11 _tmp9BC=Cyc_yyget_YY18(yyyyvsp[0]);_tmp48E->hd=_tmp9BC;});_tmp48E;}));yyval=_tmp9BD;});
goto _LL0;}case 123U: _LLF1: _LLF2: {
# 1779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1778 "parse.y"
({union Cyc_YYSTYPE _tmp9C0=Cyc_YY19(({struct _tuple12*_tmp48F=_region_malloc(yyr,sizeof(*_tmp48F));({struct _tuple12*_tmp9BF=Cyc_yyget_YY19(yyyyvsp[0]);_tmp48F->tl=_tmp9BF;}),({struct _tuple11 _tmp9BE=Cyc_yyget_YY18(yyyyvsp[2]);_tmp48F->hd=_tmp9BE;});_tmp48F;}));yyval=_tmp9C0;});
goto _LL0;}case 124U: _LLF3: _LLF4: {
# 1781
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1783
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1783 "parse.y"
({union Cyc_YYSTYPE _tmp9C2=Cyc_YY18(({struct _tuple11 _tmp729;({struct Cyc_Parse_Declarator _tmp9C1=Cyc_yyget_YY27(yyyyvsp[0]);_tmp729.f1=_tmp9C1;}),_tmp729.f2=0;_tmp729;}));yyval=_tmp9C2;});
goto _LL0;}case 125U: _LLF5: _LLF6: {
# 1786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1785 "parse.y"
({union Cyc_YYSTYPE _tmp9C5=Cyc_YY18(({struct _tuple11 _tmp72A;({struct Cyc_Parse_Declarator _tmp9C4=Cyc_yyget_YY27(yyyyvsp[0]);_tmp72A.f1=_tmp9C4;}),({struct Cyc_Absyn_Exp*_tmp9C3=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp72A.f2=_tmp9C3;});_tmp72A;}));yyval=_tmp9C5;});
goto _LL0;}case 126U: _LLF7: _LLF8: {
# 1788
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1790
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1791 "parse.y"
struct _RegionHandle _tmp490=_new_region("temp");struct _RegionHandle*temp=& _tmp490;_push_region(temp);
{struct _tuple25 _tmp491=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp492=_tmp491;struct Cyc_Absyn_Tqual _tmp4A0;struct Cyc_Parse_Type_specifier _tmp49F;struct Cyc_List_List*_tmp49E;_LL437: _tmp4A0=_tmp492.f1;_tmp49F=_tmp492.f2;_tmp49E=_tmp492.f3;_LL438:;
if(_tmp4A0.loc == 0)_tmp4A0.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp493=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp493 != 0;_tmp493=_tmp493->tl){
struct _tuple24*_tmp494=(struct _tuple24*)_tmp493->hd;struct _tuple24*_tmp495=_tmp494;struct Cyc_Parse_Declarator _tmp49B;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Absyn_Exp*_tmp499;_LL43A: _tmp49B=_tmp495->f1;_tmp49A=_tmp495->f2;_tmp499=_tmp495->f3;_LL43B:;
({struct _tuple10*_tmp9C6=({struct _tuple10*_tmp496=_region_malloc(temp,sizeof(*_tmp496));_tmp496->tl=decls,_tmp496->hd=_tmp49B;_tmp496;});decls=_tmp9C6;});
({struct Cyc_List_List*_tmp9C8=({struct Cyc_List_List*_tmp498=_region_malloc(temp,sizeof(*_tmp498));
({struct _tuple16*_tmp9C7=({struct _tuple16*_tmp497=_region_malloc(temp,sizeof(*_tmp497));_tmp497->f1=_tmp49A,_tmp497->f2=_tmp499;_tmp497;});_tmp498->hd=_tmp9C7;}),_tmp498->tl=widths_and_reqs;_tmp498;});
# 1799
widths_and_reqs=_tmp9C8;});}}
# 1802
({struct _tuple10*_tmp9C9=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);decls=_tmp9C9;});
({struct Cyc_List_List*_tmp9CA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);widths_and_reqs=_tmp9CA;});{
void*_tmp49C=Cyc_Parse_speclist2typ(_tmp49F,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp49D=({struct _RegionHandle*_tmp9CD=temp;struct _RegionHandle*_tmp9CC=temp;struct Cyc_List_List*_tmp9CB=
Cyc_Parse_apply_tmss(temp,_tmp4A0,_tmp49C,decls,_tmp49E);
# 1805
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp9CD,_tmp9CC,_tmp9CB,widths_and_reqs);});
# 1808
({union Cyc_YYSTYPE _tmp9CE=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp49D));yyval=_tmp9CE;});
# 1810
_npop_handler(0U);goto _LL0;};};}
# 1792
;_pop_region(temp);}case 127U: _LLF9: _LLFA: {
# 1812
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1814
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1818 "parse.y"
({union Cyc_YYSTYPE _tmp9D1=Cyc_YY35(({struct _tuple25 _tmp72B;({struct Cyc_Absyn_Tqual _tmp9D0=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp72B.f1=_tmp9D0;}),({struct Cyc_Parse_Type_specifier _tmp9CF=Cyc_yyget_YY21(yyyyvsp[0]);_tmp72B.f2=_tmp9CF;}),_tmp72B.f3=0;_tmp72B;}));yyval=_tmp9D1;});
goto _LL0;}case 128U: _LLFB: _LLFC: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1820 "parse.y"
struct _tuple25 _tmp4A1=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9D5=Cyc_YY35(({struct _tuple25 _tmp72C;_tmp72C.f1=_tmp4A1.f1,({struct Cyc_Parse_Type_specifier _tmp9D4=({unsigned int _tmp9D3=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp9D2=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp9D3,_tmp9D2,_tmp4A1.f2);});_tmp72C.f2=_tmp9D4;}),_tmp72C.f3=_tmp4A1.f3;_tmp72C;}));yyval=_tmp9D5;});
goto _LL0;}case 129U: _LLFD: _LLFE: {
# 1823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1822 "parse.y"
({union Cyc_YYSTYPE _tmp9D8=Cyc_YY35(({struct _tuple25 _tmp72D;({struct Cyc_Absyn_Tqual _tmp9D7=Cyc_yyget_YY23(yyyyvsp[0]);_tmp72D.f1=_tmp9D7;}),({struct Cyc_Parse_Type_specifier _tmp9D6=Cyc_Parse_empty_spec(0U);_tmp72D.f2=_tmp9D6;}),_tmp72D.f3=0;_tmp72D;}));yyval=_tmp9D8;});
goto _LL0;}case 130U: _LLFF: _LL100: {
# 1825
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1827
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1824 "parse.y"
struct _tuple25 _tmp4A2=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp9DB=Cyc_YY35(({struct _tuple25 _tmp72E;({struct Cyc_Absyn_Tqual _tmp9DA=({struct Cyc_Absyn_Tqual _tmp9D9=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp9D9,_tmp4A2.f1);});_tmp72E.f1=_tmp9DA;}),_tmp72E.f2=_tmp4A2.f2,_tmp72E.f3=_tmp4A2.f3;_tmp72E;}));yyval=_tmp9DB;});
goto _LL0;}case 131U: _LL101: _LL102: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1827 "parse.y"
({union Cyc_YYSTYPE _tmp9DF=Cyc_YY35(({struct _tuple25 _tmp72F;({struct Cyc_Absyn_Tqual _tmp9DE=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp72F.f1=_tmp9DE;}),({struct Cyc_Parse_Type_specifier _tmp9DD=Cyc_Parse_empty_spec(0U);_tmp72F.f2=_tmp9DD;}),({struct Cyc_List_List*_tmp9DC=Cyc_yyget_YY45(yyyyvsp[0]);_tmp72F.f3=_tmp9DC;});_tmp72F;}));yyval=_tmp9DF;});
goto _LL0;}case 132U: _LL103: _LL104: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1829 "parse.y"
struct _tuple25 _tmp4A3=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9E2=Cyc_YY35(({struct _tuple25 _tmp730;_tmp730.f1=_tmp4A3.f1,_tmp730.f2=_tmp4A3.f2,({struct Cyc_List_List*_tmp9E1=({struct Cyc_List_List*_tmp9E0=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9E0,_tmp4A3.f3);});_tmp730.f3=_tmp9E1;});_tmp730;}));yyval=_tmp9E2;});
goto _LL0;}case 133U: _LL105: _LL106: {
# 1832
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1834
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9E5=Cyc_YY35(({struct _tuple25 _tmp731;({struct Cyc_Absyn_Tqual _tmp9E4=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp731.f1=_tmp9E4;}),({struct Cyc_Parse_Type_specifier _tmp9E3=Cyc_yyget_YY21(yyyyvsp[0]);_tmp731.f2=_tmp9E3;}),_tmp731.f3=0;_tmp731;}));yyval=_tmp9E5;});
goto _LL0;}case 134U: _LL107: _LL108: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1837 "parse.y"
struct _tuple25 _tmp4A4=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9E9=Cyc_YY35(({struct _tuple25 _tmp732;_tmp732.f1=_tmp4A4.f1,({struct Cyc_Parse_Type_specifier _tmp9E8=({unsigned int _tmp9E7=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp9E6=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp9E7,_tmp9E6,_tmp4A4.f2);});_tmp732.f2=_tmp9E8;}),_tmp732.f3=_tmp4A4.f3;_tmp732;}));yyval=_tmp9E9;});
goto _LL0;}case 135U: _LL109: _LL10A: {
# 1840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1839 "parse.y"
({union Cyc_YYSTYPE _tmp9EC=Cyc_YY35(({struct _tuple25 _tmp733;({struct Cyc_Absyn_Tqual _tmp9EB=Cyc_yyget_YY23(yyyyvsp[0]);_tmp733.f1=_tmp9EB;}),({struct Cyc_Parse_Type_specifier _tmp9EA=Cyc_Parse_empty_spec(0U);_tmp733.f2=_tmp9EA;}),_tmp733.f3=0;_tmp733;}));yyval=_tmp9EC;});
goto _LL0;}case 136U: _LL10B: _LL10C: {
# 1842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1841 "parse.y"
struct _tuple25 _tmp4A5=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp9EF=Cyc_YY35(({struct _tuple25 _tmp734;({struct Cyc_Absyn_Tqual _tmp9EE=({struct Cyc_Absyn_Tqual _tmp9ED=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp9ED,_tmp4A5.f1);});_tmp734.f1=_tmp9EE;}),_tmp734.f2=_tmp4A5.f2,_tmp734.f3=_tmp4A5.f3;_tmp734;}));yyval=_tmp9EF;});
goto _LL0;}case 137U: _LL10D: _LL10E: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1844 "parse.y"
({union Cyc_YYSTYPE _tmp9F3=Cyc_YY35(({struct _tuple25 _tmp735;({struct Cyc_Absyn_Tqual _tmp9F2=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp735.f1=_tmp9F2;}),({struct Cyc_Parse_Type_specifier _tmp9F1=Cyc_Parse_empty_spec(0U);_tmp735.f2=_tmp9F1;}),({struct Cyc_List_List*_tmp9F0=Cyc_yyget_YY45(yyyyvsp[0]);_tmp735.f3=_tmp9F0;});_tmp735;}));yyval=_tmp9F3;});
goto _LL0;}case 138U: _LL10F: _LL110: {
# 1847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1846 "parse.y"
struct _tuple25 _tmp4A6=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9F6=Cyc_YY35(({struct _tuple25 _tmp736;_tmp736.f1=_tmp4A6.f1,_tmp736.f2=_tmp4A6.f2,({struct Cyc_List_List*_tmp9F5=({struct Cyc_List_List*_tmp9F4=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9F4,_tmp4A6.f3);});_tmp736.f3=_tmp9F5;});_tmp736;}));yyval=_tmp9F6;});
goto _LL0;}case 139U: _LL111: _LL112: {
# 1849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1850 "parse.y"
({union Cyc_YYSTYPE _tmp9F7=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));yyval=_tmp9F7;});
goto _LL0;}case 140U: _LL113: _LL114: {
# 1853
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1855
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9F9=Cyc_YY29(({struct Cyc_List_List*_tmp4A7=_region_malloc(yyr,sizeof(*_tmp4A7));({struct _tuple24*_tmp9F8=Cyc_yyget_YY28(yyyyvsp[0]);_tmp4A7->hd=_tmp9F8;}),_tmp4A7->tl=0;_tmp4A7;}));yyval=_tmp9F9;});
goto _LL0;}case 141U: _LL115: _LL116: {
# 1859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1858 "parse.y"
({union Cyc_YYSTYPE _tmp9FC=Cyc_YY29(({struct Cyc_List_List*_tmp4A8=_region_malloc(yyr,sizeof(*_tmp4A8));({struct _tuple24*_tmp9FB=Cyc_yyget_YY28(yyyyvsp[2]);_tmp4A8->hd=_tmp9FB;}),({struct Cyc_List_List*_tmp9FA=Cyc_yyget_YY29(yyyyvsp[0]);_tmp4A8->tl=_tmp9FA;});_tmp4A8;}));yyval=_tmp9FC;});
goto _LL0;}case 142U: _LL117: _LL118: {
# 1861
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1863
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1863 "parse.y"
({union Cyc_YYSTYPE _tmp9FF=Cyc_YY28(({struct _tuple24*_tmp4A9=_region_malloc(yyr,sizeof(*_tmp4A9));({struct Cyc_Parse_Declarator _tmp9FE=Cyc_yyget_YY27(yyyyvsp[0]);_tmp4A9->f1=_tmp9FE;}),_tmp4A9->f2=0,({struct Cyc_Absyn_Exp*_tmp9FD=Cyc_yyget_YY55(yyyyvsp[1]);_tmp4A9->f3=_tmp9FD;});_tmp4A9;}));yyval=_tmp9FF;});
goto _LL0;}case 143U: _LL119: _LL11A: {
# 1866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1867 "parse.y"
({union Cyc_YYSTYPE _tmpA05=Cyc_YY28(({struct _tuple24*_tmp4AD=_region_malloc(yyr,sizeof(*_tmp4AD));({struct _tuple0*_tmpA04=({struct _tuple0*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));({union Cyc_Absyn_Nmspace _tmpA03=Cyc_Absyn_Rel_n(0);_tmp4AC->f1=_tmpA03;}),({struct _dyneither_ptr*_tmpA02=({struct _dyneither_ptr*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));({struct _dyneither_ptr _tmpA01=({const char*_tmp4AA="";_tag_dyneither(_tmp4AA,sizeof(char),1U);});*_tmp4AB=_tmpA01;});_tmp4AB;});_tmp4AC->f2=_tmpA02;});_tmp4AC;});(_tmp4AD->f1).id=_tmpA04;}),(_tmp4AD->f1).varloc=0U,(_tmp4AD->f1).tms=0,({struct Cyc_Absyn_Exp*_tmpA00=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp4AD->f2=_tmpA00;}),_tmp4AD->f3=0;_tmp4AD;}));yyval=_tmpA05;});
# 1869
goto _LL0;}case 144U: _LL11B: _LL11C:
# 1873
({union Cyc_YYSTYPE _tmpA0A=Cyc_YY28(({struct _tuple24*_tmp4B1=_region_malloc(yyr,sizeof(*_tmp4B1));({struct _tuple0*_tmpA09=({struct _tuple0*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));({union Cyc_Absyn_Nmspace _tmpA08=Cyc_Absyn_Rel_n(0);_tmp4B0->f1=_tmpA08;}),({struct _dyneither_ptr*_tmpA07=({struct _dyneither_ptr*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({struct _dyneither_ptr _tmpA06=({const char*_tmp4AE="";_tag_dyneither(_tmp4AE,sizeof(char),1U);});*_tmp4AF=_tmpA06;});_tmp4AF;});_tmp4B0->f2=_tmpA07;});_tmp4B0;});(_tmp4B1->f1).id=_tmpA09;}),(_tmp4B1->f1).varloc=0U,(_tmp4B1->f1).tms=0,_tmp4B1->f2=0,_tmp4B1->f3=0;_tmp4B1;}));yyval=_tmpA0A;});
# 1875
goto _LL0;case 145U: _LL11D: _LL11E: {
# 1877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1875 "parse.y"
({union Cyc_YYSTYPE _tmpA0D=Cyc_YY28(({struct _tuple24*_tmp4B2=_region_malloc(yyr,sizeof(*_tmp4B2));({struct Cyc_Parse_Declarator _tmpA0C=Cyc_yyget_YY27(yyyyvsp[0]);_tmp4B2->f1=_tmpA0C;}),({struct Cyc_Absyn_Exp*_tmpA0B=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4B2->f2=_tmpA0B;}),_tmp4B2->f3=0;_tmp4B2;}));yyval=_tmpA0D;});
goto _LL0;}case 146U: _LL11F: _LL120: {
# 1878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1879 "parse.y"
({union Cyc_YYSTYPE _tmpA0E=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpA0E;});
goto _LL0;}case 147U: _LL121: _LL122:
# 1882
({union Cyc_YYSTYPE _tmpA0F=Cyc_YY55(0);yyval=_tmpA0F;});
goto _LL0;case 148U: _LL123: _LL124: {
# 1885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1884 "parse.y"
({union Cyc_YYSTYPE _tmpA10=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmpA10;});
goto _LL0;}case 149U: _LL125: _LL126:
# 1887
({union Cyc_YYSTYPE _tmpA11=Cyc_YY55(0);yyval=_tmpA11;});
goto _LL0;case 150U: _LL127: _LL128: {
# 1890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1891 "parse.y"
int _tmp4B3=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp4B4=({unsigned int _tmpA12=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA12,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp4B5=({struct _tuple0*_tmpA17=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpA16=_tmp4B4;struct Cyc_Core_Opt*_tmpA15=({struct Cyc_Core_Opt*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));({struct Cyc_List_List*_tmpA13=Cyc_yyget_YY34(yyyyvsp[4]);_tmp4B7->v=_tmpA13;});_tmp4B7;});int _tmpA14=_tmp4B3;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmpA17,_tmpA16,_tmpA15,_tmpA14,(unsigned int)(yyyylsp[0]).first_line);});
# 1895
({union Cyc_YYSTYPE _tmpA19=Cyc_YY21(({void*_tmpA18=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->tag=26U,_tmp4B6->f1=_tmp4B5,_tmp4B6->f2=0;_tmp4B6;});Cyc_Parse_type_spec(_tmpA18,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA19;});
# 1897
goto _LL0;}case 151U: _LL129: _LL12A: {
# 1899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1898 "parse.y"
int _tmp4B8=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA1E=Cyc_YY21(({void*_tmpA1D=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->tag=3U,({union Cyc_Absyn_DatatypeInfoU _tmpA1C=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp737;({struct _tuple0*_tmpA1B=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp737.name=_tmpA1B;}),_tmp737.is_extensible=_tmp4B8;_tmp737;}));(_tmp4B9->f1).datatype_info=_tmpA1C;}),({struct Cyc_List_List*_tmpA1A=Cyc_yyget_YY40(yyyyvsp[2]);(_tmp4B9->f1).targs=_tmpA1A;});_tmp4B9;});Cyc_Parse_type_spec(_tmpA1D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA1E;});
# 1901
goto _LL0;}case 152U: _LL12B: _LL12C: {
# 1903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1902 "parse.y"
int _tmp4BA=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA24=Cyc_YY21(({void*_tmpA23=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->tag=4U,({
union Cyc_Absyn_DatatypeFieldInfoU _tmpA22=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp738;({struct _tuple0*_tmpA21=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp738.datatype_name=_tmpA21;}),({struct _tuple0*_tmpA20=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp738.field_name=_tmpA20;}),_tmp738.is_extensible=_tmp4BA;_tmp738;}));(_tmp4BB->f1).field_info=_tmpA22;}),({struct Cyc_List_List*_tmpA1F=Cyc_yyget_YY40(yyyyvsp[4]);(_tmp4BB->f1).targs=_tmpA1F;});_tmp4BB;});
# 1903
Cyc_Parse_type_spec(_tmpA23,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpA24;});
# 1907
goto _LL0;}case 153U: _LL12D: _LL12E: {
# 1909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1910 "parse.y"
({union Cyc_YYSTYPE _tmpA25=Cyc_YY31(0);yyval=_tmpA25;});
goto _LL0;}case 154U: _LL12F: _LL130: {
# 1913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1911 "parse.y"
({union Cyc_YYSTYPE _tmpA26=Cyc_YY31(1);yyval=_tmpA26;});
goto _LL0;}case 155U: _LL131: _LL132: {
# 1914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1915 "parse.y"
({union Cyc_YYSTYPE _tmpA28=Cyc_YY34(({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));({struct Cyc_Absyn_Datatypefield*_tmpA27=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4BC->hd=_tmpA27;}),_tmp4BC->tl=0;_tmp4BC;}));yyval=_tmpA28;});
goto _LL0;}case 156U: _LL133: _LL134: {
# 1918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1916 "parse.y"
({union Cyc_YYSTYPE _tmpA2A=Cyc_YY34(({struct Cyc_List_List*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));({struct Cyc_Absyn_Datatypefield*_tmpA29=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4BD->hd=_tmpA29;}),_tmp4BD->tl=0;_tmp4BD;}));yyval=_tmpA2A;});
goto _LL0;}case 157U: _LL135: _LL136: {
# 1919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1917 "parse.y"
({union Cyc_YYSTYPE _tmpA2D=Cyc_YY34(({struct Cyc_List_List*_tmp4BE=_cycalloc(sizeof(*_tmp4BE));({struct Cyc_Absyn_Datatypefield*_tmpA2C=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4BE->hd=_tmpA2C;}),({struct Cyc_List_List*_tmpA2B=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4BE->tl=_tmpA2B;});_tmp4BE;}));yyval=_tmpA2D;});
goto _LL0;}case 158U: _LL137: _LL138: {
# 1920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1918 "parse.y"
({union Cyc_YYSTYPE _tmpA30=Cyc_YY34(({struct Cyc_List_List*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));({struct Cyc_Absyn_Datatypefield*_tmpA2F=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4BF->hd=_tmpA2F;}),({struct Cyc_List_List*_tmpA2E=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4BF->tl=_tmpA2E;});_tmp4BF;}));yyval=_tmpA30;});
goto _LL0;}case 159U: _LL139: _LL13A:
# 1921
({union Cyc_YYSTYPE _tmpA31=Cyc_YY32(Cyc_Absyn_Public);yyval=_tmpA31;});
goto _LL0;case 160U: _LL13B: _LL13C: {
# 1924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1923 "parse.y"
({union Cyc_YYSTYPE _tmpA32=Cyc_YY32(Cyc_Absyn_Extern);yyval=_tmpA32;});
goto _LL0;}case 161U: _LL13D: _LL13E: {
# 1926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1924 "parse.y"
({union Cyc_YYSTYPE _tmpA33=Cyc_YY32(Cyc_Absyn_Static);yyval=_tmpA33;});
goto _LL0;}case 162U: _LL13F: _LL140: {
# 1927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1928 "parse.y"
({union Cyc_YYSTYPE _tmpA36=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));({struct _tuple0*_tmpA35=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4C0->name=_tmpA35;}),_tmp4C0->typs=0,_tmp4C0->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmpA34=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4C0->sc=_tmpA34;});_tmp4C0;}));yyval=_tmpA36;});
goto _LL0;}case 163U: _LL141: _LL142: {
# 1931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1930 "parse.y"
struct Cyc_List_List*_tmp4C1=({unsigned int _tmpA37=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmpA37,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
({union Cyc_YYSTYPE _tmpA3A=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));({struct _tuple0*_tmpA39=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4C2->name=_tmpA39;}),_tmp4C2->typs=_tmp4C1,_tmp4C2->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmpA38=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4C2->sc=_tmpA38;});_tmp4C2;}));yyval=_tmpA3A;});
goto _LL0;}case 164U: _LL143: _LL144: {
# 1934
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1936
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1936 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 165U: _LL145: _LL146: {
# 1939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1938 "parse.y"
struct Cyc_Parse_Declarator _tmp4C3=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA3D=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp739;_tmp739.id=_tmp4C3.id,_tmp739.varloc=_tmp4C3.varloc,({struct Cyc_List_List*_tmpA3C=({struct Cyc_List_List*_tmpA3B=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA3B,_tmp4C3.tms);});_tmp739.tms=_tmpA3C;});_tmp739;}));yyval=_tmpA3D;});
goto _LL0;}case 166U: _LL147: _LL148: {
# 1942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 167U: _LL149: _LL14A: {
# 1948
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1950
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1947 "parse.y"
struct Cyc_Parse_Declarator _tmp4C4=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA40=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73A;_tmp73A.id=_tmp4C4.id,_tmp73A.varloc=_tmp4C4.varloc,({struct Cyc_List_List*_tmpA3F=({struct Cyc_List_List*_tmpA3E=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA3E,_tmp4C4.tms);});_tmp73A.tms=_tmpA3F;});_tmp73A;}));yyval=_tmpA40;});
goto _LL0;}case 168U: _LL14B: _LL14C: {
# 1951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1953 "parse.y"
({union Cyc_YYSTYPE _tmpA42=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73B;({struct _tuple0*_tmpA41=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp73B.id=_tmpA41;}),_tmp73B.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp73B.tms=0;_tmp73B;}));yyval=_tmpA42;});
goto _LL0;}case 169U: _LL14D: _LL14E: {
# 1956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1955 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 170U: _LL14F: _LL150: {
# 1958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1959 "parse.y"
struct Cyc_Parse_Declarator _tmp4C5=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpA45=({struct Cyc_List_List*_tmp4C7=_region_malloc(yyr,sizeof(*_tmp4C7));({void*_tmpA44=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));_tmp4C6->tag=5U,_tmp4C6->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA43=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4C6->f2=_tmpA43;});_tmp4C6;});_tmp4C7->hd=_tmpA44;}),_tmp4C7->tl=_tmp4C5.tms;_tmp4C7;});_tmp4C5.tms=_tmpA45;});
yyval=yyyyvsp[2];
# 1963
goto _LL0;}case 171U: _LL151: _LL152: {
# 1965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1964 "parse.y"
({union Cyc_YYSTYPE _tmpA4C=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73C;({struct _tuple0*_tmpA4B=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp73C.id=_tmpA4B;}),({unsigned int _tmpA4A=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp73C.varloc=_tmpA4A;}),({struct Cyc_List_List*_tmpA49=({struct Cyc_List_List*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));({void*_tmpA48=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));_tmp4C8->tag=0U,({union Cyc_Absyn_Constraint*_tmpA47=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4C8->f1=_tmpA47;}),_tmp4C8->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4C8;});_tmp4C9->hd=_tmpA48;}),({struct Cyc_List_List*_tmpA46=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4C9->tl=_tmpA46;});_tmp4C9;});_tmp73C.tms=_tmpA49;});_tmp73C;}));yyval=_tmpA4C;});
goto _LL0;}case 172U: _LL153: _LL154: {
# 1967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1966 "parse.y"
({union Cyc_YYSTYPE _tmpA54=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73D;({struct _tuple0*_tmpA53=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp73D.id=_tmpA53;}),({unsigned int _tmpA52=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp73D.varloc=_tmpA52;}),({
struct Cyc_List_List*_tmpA51=({struct Cyc_List_List*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));({void*_tmpA50=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4CA=_region_malloc(yyr,sizeof(*_tmp4CA));_tmp4CA->tag=1U,({struct Cyc_Absyn_Exp*_tmpA4F=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4CA->f1=_tmpA4F;}),({union Cyc_Absyn_Constraint*_tmpA4E=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4CA->f2=_tmpA4E;}),_tmp4CA->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4CA;});_tmp4CB->hd=_tmpA50;}),({struct Cyc_List_List*_tmpA4D=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4CB->tl=_tmpA4D;});_tmp4CB;});_tmp73D.tms=_tmpA51;});_tmp73D;}));
# 1966
yyval=_tmpA54;});
# 1968
goto _LL0;}case 173U: _LL155: _LL156: {
# 1970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1969 "parse.y"
struct _tuple26*_tmp4CC=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4CD=_tmp4CC;struct Cyc_List_List*_tmp4D7;int _tmp4D6;struct Cyc_Absyn_VarargInfo*_tmp4D5;void*_tmp4D4;struct Cyc_List_List*_tmp4D3;_LL43D: _tmp4D7=_tmp4CD->f1;_tmp4D6=_tmp4CD->f2;_tmp4D5=_tmp4CD->f3;_tmp4D4=_tmp4CD->f4;_tmp4D3=_tmp4CD->f5;_LL43E:;{
struct Cyc_Absyn_Exp*_tmp4CE=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4CF=Cyc_yyget_YY55(yyyyvsp[5]);
({union Cyc_YYSTYPE _tmpA5B=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73E;({struct _tuple0*_tmpA5A=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp73E.id=_tmpA5A;}),({unsigned int _tmpA59=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp73E.varloc=_tmpA59;}),({struct Cyc_List_List*_tmpA58=({struct Cyc_List_List*_tmp4D2=_region_malloc(yyr,sizeof(*_tmp4D2));({void*_tmpA57=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D1=_region_malloc(yyr,sizeof(*_tmp4D1));_tmp4D1->tag=3U,({void*_tmpA56=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4D0=_region_malloc(yyr,sizeof(*_tmp4D0));_tmp4D0->tag=1U,_tmp4D0->f1=_tmp4D7,_tmp4D0->f2=_tmp4D6,_tmp4D0->f3=_tmp4D5,_tmp4D0->f4=_tmp4D4,_tmp4D0->f5=_tmp4D3,_tmp4D0->f6=_tmp4CE,_tmp4D0->f7=_tmp4CF;_tmp4D0;});_tmp4D1->f1=_tmpA56;});_tmp4D1;});_tmp4D2->hd=_tmpA57;}),({struct Cyc_List_List*_tmpA55=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4D2->tl=_tmpA55;});_tmp4D2;});_tmp73E.tms=_tmpA58;});_tmp73E;}));yyval=_tmpA5B;});
# 1974
goto _LL0;};}case 174U: _LL157: _LL158: {
# 1976
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1978
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1975 "parse.y"
({union Cyc_YYSTYPE _tmpA66=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp73F;({struct _tuple0*_tmpA65=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp73F.id=_tmpA65;}),({unsigned int _tmpA64=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp73F.varloc=_tmpA64;}),({
struct Cyc_List_List*_tmpA63=({struct Cyc_List_List*_tmp4DA=_region_malloc(yyr,sizeof(*_tmp4DA));({void*_tmpA62=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D9=_region_malloc(yyr,sizeof(*_tmp4D9));_tmp4D9->tag=3U,({void*_tmpA61=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4D8=_region_malloc(yyr,sizeof(*_tmp4D8));_tmp4D8->tag=1U,_tmp4D8->f1=0,_tmp4D8->f2=0,_tmp4D8->f3=0,({
# 1978
void*_tmpA60=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4D8->f4=_tmpA60;}),({struct Cyc_List_List*_tmpA5F=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4D8->f5=_tmpA5F;}),({struct Cyc_Absyn_Exp*_tmpA5E=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4D8->f6=_tmpA5E;}),({struct Cyc_Absyn_Exp*_tmpA5D=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4D8->f7=_tmpA5D;});_tmp4D8;});
# 1976
_tmp4D9->f1=_tmpA61;});_tmp4D9;});_tmp4DA->hd=_tmpA62;}),({
# 1979
struct Cyc_List_List*_tmpA5C=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4DA->tl=_tmpA5C;});_tmp4DA;});
# 1976
_tmp73F.tms=_tmpA63;});_tmp73F;}));
# 1975
yyval=_tmpA66;});
# 1981
goto _LL0;}case 175U: _LL159: _LL15A: {
# 1983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1982 "parse.y"
({union Cyc_YYSTYPE _tmpA6E=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp740;({struct _tuple0*_tmpA6D=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp740.id=_tmpA6D;}),({unsigned int _tmpA6C=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp740.varloc=_tmpA6C;}),({struct Cyc_List_List*_tmpA6B=({struct Cyc_List_List*_tmp4DD=_region_malloc(yyr,sizeof(*_tmp4DD));({void*_tmpA6A=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4DC=_region_malloc(yyr,sizeof(*_tmp4DC));_tmp4DC->tag=3U,({void*_tmpA69=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4DB=_region_malloc(yyr,sizeof(*_tmp4DB));_tmp4DB->tag=0U,({struct Cyc_List_List*_tmpA68=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4DB->f1=_tmpA68;}),_tmp4DB->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4DB;});_tmp4DC->f1=_tmpA69;});_tmp4DC;});_tmp4DD->hd=_tmpA6A;}),({struct Cyc_List_List*_tmpA67=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4DD->tl=_tmpA67;});_tmp4DD;});_tmp740.tms=_tmpA6B;});_tmp740;}));yyval=_tmpA6E;});
goto _LL0;}case 176U: _LL15B: _LL15C: {
# 1985
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1987
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1985 "parse.y"
struct Cyc_List_List*_tmp4DE=({unsigned int _tmpA6F=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA6F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpA75=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp741;({struct _tuple0*_tmpA74=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp741.id=_tmpA74;}),({unsigned int _tmpA73=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp741.varloc=_tmpA73;}),({struct Cyc_List_List*_tmpA72=({struct Cyc_List_List*_tmp4E0=_region_malloc(yyr,sizeof(*_tmp4E0));({void*_tmpA71=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4DF=_region_malloc(yyr,sizeof(*_tmp4DF));_tmp4DF->tag=4U,_tmp4DF->f1=_tmp4DE,_tmp4DF->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4DF->f3=0;_tmp4DF;});_tmp4E0->hd=_tmpA71;}),({struct Cyc_List_List*_tmpA70=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4E0->tl=_tmpA70;});_tmp4E0;});_tmp741.tms=_tmpA72;});_tmp741;}));yyval=_tmpA75;});
# 1988
goto _LL0;}case 177U: _LL15D: _LL15E: {
# 1990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1989 "parse.y"
({union Cyc_YYSTYPE _tmpA7C=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp742;({struct _tuple0*_tmpA7B=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp742.id=_tmpA7B;}),({unsigned int _tmpA7A=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp742.varloc=_tmpA7A;}),({struct Cyc_List_List*_tmpA79=({struct Cyc_List_List*_tmp4E2=_region_malloc(yyr,sizeof(*_tmp4E2));({void*_tmpA78=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E1=_region_malloc(yyr,sizeof(*_tmp4E1));_tmp4E1->tag=5U,_tmp4E1->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA77=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4E1->f2=_tmpA77;});_tmp4E1;});_tmp4E2->hd=_tmpA78;}),({
struct Cyc_List_List*_tmpA76=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4E2->tl=_tmpA76;});_tmp4E2;});
# 1989
_tmp742.tms=_tmpA79;});_tmp742;}));yyval=_tmpA7C;});
# 1992
goto _LL0;}case 178U: _LL15F: _LL160: {
# 1994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpA7E=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp743;({struct _tuple0*_tmpA7D=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp743.id=_tmpA7D;}),_tmp743.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp743.tms=0;_tmp743;}));yyval=_tmpA7E;});
goto _LL0;}case 179U: _LL161: _LL162: {
# 2000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1999 "parse.y"
({union Cyc_YYSTYPE _tmpA80=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp744;({struct _tuple0*_tmpA7F=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp744.id=_tmpA7F;}),_tmp744.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp744.tms=0;_tmp744;}));yyval=_tmpA80;});
goto _LL0;}case 180U: _LL163: _LL164: {
# 2002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2001 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 181U: _LL165: _LL166: {
# 2004
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2006
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2005 "parse.y"
struct Cyc_Parse_Declarator _tmp4E3=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpA83=({struct Cyc_List_List*_tmp4E5=_region_malloc(yyr,sizeof(*_tmp4E5));({void*_tmpA82=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));_tmp4E4->tag=5U,_tmp4E4->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA81=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4E4->f2=_tmpA81;});_tmp4E4;});_tmp4E5->hd=_tmpA82;}),_tmp4E5->tl=_tmp4E3.tms;_tmp4E5;});_tmp4E3.tms=_tmpA83;});
yyval=yyyyvsp[2];
# 2009
goto _LL0;}case 182U: _LL167: _LL168: {
# 2011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2010 "parse.y"
struct Cyc_Parse_Declarator _tmp4E6=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA87=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp745;_tmp745.id=_tmp4E6.id,_tmp745.varloc=_tmp4E6.varloc,({
struct Cyc_List_List*_tmpA86=({struct Cyc_List_List*_tmp4E8=_region_malloc(yyr,sizeof(*_tmp4E8));({void*_tmpA85=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));_tmp4E7->tag=0U,({union Cyc_Absyn_Constraint*_tmpA84=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4E7->f1=_tmpA84;}),_tmp4E7->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4E7;});_tmp4E8->hd=_tmpA85;}),_tmp4E8->tl=_tmp4E6.tms;_tmp4E8;});_tmp745.tms=_tmpA86;});_tmp745;}));
# 2011
yyval=_tmpA87;});
# 2013
goto _LL0;}case 183U: _LL169: _LL16A: {
# 2015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2014 "parse.y"
struct Cyc_Parse_Declarator _tmp4E9=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA8C=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp746;_tmp746.id=_tmp4E9.id,_tmp746.varloc=_tmp4E9.varloc,({
struct Cyc_List_List*_tmpA8B=({struct Cyc_List_List*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));({void*_tmpA8A=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));_tmp4EA->tag=1U,({struct Cyc_Absyn_Exp*_tmpA89=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4EA->f1=_tmpA89;}),({union Cyc_Absyn_Constraint*_tmpA88=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4EA->f2=_tmpA88;}),_tmp4EA->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4EA;});_tmp4EB->hd=_tmpA8A;}),_tmp4EB->tl=_tmp4E9.tms;_tmp4EB;});_tmp746.tms=_tmpA8B;});_tmp746;}));
# 2015
yyval=_tmpA8C;});
# 2018
goto _LL0;}case 184U: _LL16B: _LL16C: {
# 2020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2019 "parse.y"
struct _tuple26*_tmp4EC=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4ED=_tmp4EC;struct Cyc_List_List*_tmp4F8;int _tmp4F7;struct Cyc_Absyn_VarargInfo*_tmp4F6;void*_tmp4F5;struct Cyc_List_List*_tmp4F4;_LL440: _tmp4F8=_tmp4ED->f1;_tmp4F7=_tmp4ED->f2;_tmp4F6=_tmp4ED->f3;_tmp4F5=_tmp4ED->f4;_tmp4F4=_tmp4ED->f5;_LL441:;{
struct Cyc_Absyn_Exp*_tmp4EE=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4EF=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp4F0=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA90=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp747;_tmp747.id=_tmp4F0.id,_tmp747.varloc=_tmp4F0.varloc,({struct Cyc_List_List*_tmpA8F=({struct Cyc_List_List*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));({void*_tmpA8E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4F2=_region_malloc(yyr,sizeof(*_tmp4F2));_tmp4F2->tag=3U,({void*_tmpA8D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4F1=_region_malloc(yyr,sizeof(*_tmp4F1));_tmp4F1->tag=1U,_tmp4F1->f1=_tmp4F8,_tmp4F1->f2=_tmp4F7,_tmp4F1->f3=_tmp4F6,_tmp4F1->f4=_tmp4F5,_tmp4F1->f5=_tmp4F4,_tmp4F1->f6=_tmp4EE,_tmp4F1->f7=_tmp4EF;_tmp4F1;});_tmp4F2->f1=_tmpA8D;});_tmp4F2;});_tmp4F3->hd=_tmpA8E;}),_tmp4F3->tl=_tmp4F0.tms;_tmp4F3;});_tmp747.tms=_tmpA8F;});_tmp747;}));yyval=_tmpA90;});
# 2025
goto _LL0;};}case 185U: _LL16D: _LL16E: {
# 2027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2026 "parse.y"
struct Cyc_Parse_Declarator _tmp4F9=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA98=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp748;_tmp748.id=_tmp4F9.id,_tmp748.varloc=_tmp4F9.varloc,({
struct Cyc_List_List*_tmpA97=({struct Cyc_List_List*_tmp4FC=_region_malloc(yyr,sizeof(*_tmp4FC));({void*_tmpA96=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4FB=_region_malloc(yyr,sizeof(*_tmp4FB));_tmp4FB->tag=3U,({void*_tmpA95=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4FA=_region_malloc(yyr,sizeof(*_tmp4FA));_tmp4FA->tag=1U,_tmp4FA->f1=0,_tmp4FA->f2=0,_tmp4FA->f3=0,({
# 2030
void*_tmpA94=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4FA->f4=_tmpA94;}),({struct Cyc_List_List*_tmpA93=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4FA->f5=_tmpA93;}),({struct Cyc_Absyn_Exp*_tmpA92=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4FA->f6=_tmpA92;}),({struct Cyc_Absyn_Exp*_tmpA91=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4FA->f7=_tmpA91;});_tmp4FA;});
# 2028
_tmp4FB->f1=_tmpA95;});_tmp4FB;});_tmp4FC->hd=_tmpA96;}),_tmp4FC->tl=_tmp4F9.tms;_tmp4FC;});_tmp748.tms=_tmpA97;});_tmp748;}));
# 2027
yyval=_tmpA98;});
# 2033
goto _LL0;}case 186U: _LL16F: _LL170: {
# 2035
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2037
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2034 "parse.y"
struct Cyc_Parse_Declarator _tmp4FD=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA9D=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp749;_tmp749.id=_tmp4FD.id,_tmp749.varloc=_tmp4FD.varloc,({struct Cyc_List_List*_tmpA9C=({struct Cyc_List_List*_tmp500=_region_malloc(yyr,sizeof(*_tmp500));({void*_tmpA9B=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4FF=_region_malloc(yyr,sizeof(*_tmp4FF));_tmp4FF->tag=3U,({void*_tmpA9A=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4FE=_region_malloc(yyr,sizeof(*_tmp4FE));_tmp4FE->tag=0U,({struct Cyc_List_List*_tmpA99=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4FE->f1=_tmpA99;}),_tmp4FE->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4FE;});_tmp4FF->f1=_tmpA9A;});_tmp4FF;});_tmp500->hd=_tmpA9B;}),_tmp500->tl=_tmp4FD.tms;_tmp500;});_tmp749.tms=_tmpA9C;});_tmp749;}));yyval=_tmpA9D;});
goto _LL0;}case 187U: _LL171: _LL172: {
# 2038
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2040
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2038 "parse.y"
struct Cyc_List_List*_tmp501=({unsigned int _tmpA9E=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA9E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp502=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpAA1=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74A;_tmp74A.id=_tmp502.id,_tmp74A.varloc=_tmp502.varloc,({struct Cyc_List_List*_tmpAA0=({struct Cyc_List_List*_tmp504=_region_malloc(yyr,sizeof(*_tmp504));({void*_tmpA9F=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp503=_region_malloc(yyr,sizeof(*_tmp503));_tmp503->tag=4U,_tmp503->f1=_tmp501,_tmp503->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp503->f3=0;_tmp503;});_tmp504->hd=_tmpA9F;}),_tmp504->tl=_tmp502.tms;_tmp504;});_tmp74A.tms=_tmpAA0;});_tmp74A;}));yyval=_tmpAA1;});
# 2042
goto _LL0;}case 188U: _LL173: _LL174: {
# 2044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2043 "parse.y"
struct Cyc_Parse_Declarator _tmp505=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpAA5=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp74B;_tmp74B.id=_tmp505.id,_tmp74B.varloc=_tmp505.varloc,({struct Cyc_List_List*_tmpAA4=({struct Cyc_List_List*_tmp507=_region_malloc(yyr,sizeof(*_tmp507));({void*_tmpAA3=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp506=_region_malloc(yyr,sizeof(*_tmp506));_tmp506->tag=5U,_tmp506->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpAA2=Cyc_yyget_YY45(yyyyvsp[1]);_tmp506->f2=_tmpAA2;});_tmp506;});_tmp507->hd=_tmpAA3;}),_tmp507->tl=_tmp505.tms;_tmp507;});_tmp74B.tms=_tmpAA4;});_tmp74B;}));yyval=_tmpAA5;});
# 2046
goto _LL0;}case 189U: _LL175: _LL176: {
# 2048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2050 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 190U: _LL177: _LL178: {
# 2053
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2055
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2051 "parse.y"
({union Cyc_YYSTYPE _tmpAA7=Cyc_YY26(({struct Cyc_List_List*_tmpAA6=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpAA6,Cyc_yyget_YY26(yyyyvsp[1]));}));yyval=_tmpAA7;});
goto _LL0;}case 191U: _LL179: _LL17A: {
# 2054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2055 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
({struct Cyc_List_List*_tmpAAA=({struct Cyc_List_List*_tmp509=_region_malloc(yyr,sizeof(*_tmp509));({void*_tmpAA9=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp508=_region_malloc(yyr,sizeof(*_tmp508));_tmp508->tag=5U,_tmp508->f1=(unsigned int)(yyyylsp[3]).first_line,({struct Cyc_List_List*_tmpAA8=Cyc_yyget_YY45(yyyyvsp[3]);_tmp508->f2=_tmpAA8;});_tmp508;});_tmp509->hd=_tmpAA9;}),_tmp509->tl=ans;_tmp509;});ans=_tmpAAA;});{
# 2059
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp50A=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp50B=_tmp50A;unsigned int _tmp517;union Cyc_Absyn_Constraint*_tmp516;union Cyc_Absyn_Constraint*_tmp515;_LL443: _tmp517=_tmp50B.f1;_tmp516=_tmp50B.f2;_tmp515=_tmp50B.f3;_LL444:;
if(Cyc_Absyn_porting_c_code)
({struct Cyc_Absyn_PtrLoc*_tmpAAB=({struct Cyc_Absyn_PtrLoc*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->ptr_loc=_tmp517,_tmp50C->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,_tmp50C->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp50C;});ptrloc=_tmpAAB;});{
# 2064
struct _tuple14 _tmp50D=({unsigned int _tmpAAF=_tmp517;union Cyc_Absyn_Constraint*_tmpAAE=_tmp516;union Cyc_Absyn_Constraint*_tmpAAD=_tmp515;void*_tmpAAC=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmpAAF,_tmpAAE,_tmpAAD,_tmpAAC,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp50E=_tmp50D;union Cyc_Absyn_Constraint*_tmp514;union Cyc_Absyn_Constraint*_tmp513;union Cyc_Absyn_Constraint*_tmp512;void*_tmp511;_LL446: _tmp514=_tmp50E.f1;_tmp513=_tmp50E.f2;_tmp512=_tmp50E.f3;_tmp511=_tmp50E.f4;_LL447:;
({struct Cyc_List_List*_tmpAB2=({struct Cyc_List_List*_tmp510=_region_malloc(yyr,sizeof(*_tmp510));({void*_tmpAB1=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp50F=_region_malloc(yyr,sizeof(*_tmp50F));_tmp50F->tag=2U,(_tmp50F->f1).rgn=_tmp511,(_tmp50F->f1).nullable=_tmp514,(_tmp50F->f1).bounds=_tmp513,(_tmp50F->f1).zero_term=_tmp512,(_tmp50F->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmpAB0=Cyc_yyget_YY23(yyyyvsp[4]);_tmp50F->f2=_tmpAB0;});_tmp50F;});_tmp510->hd=_tmpAB1;}),_tmp510->tl=ans;_tmp510;});ans=_tmpAB2;});
({union Cyc_YYSTYPE _tmpAB3=Cyc_YY26(ans);yyval=_tmpAB3;});
# 2068
goto _LL0;};};}case 192U: _LL17B: _LL17C:
# 2070
({union Cyc_YYSTYPE _tmpAB4=Cyc_YY54(0);yyval=_tmpAB4;});
goto _LL0;case 193U: _LL17D: _LL17E: {
# 2073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2071 "parse.y"
({union Cyc_YYSTYPE _tmpAB7=Cyc_YY54(({struct Cyc_List_List*_tmp518=_region_malloc(yyr,sizeof(*_tmp518));({void*_tmpAB6=Cyc_yyget_YY53(yyyyvsp[0]);_tmp518->hd=_tmpAB6;}),({struct Cyc_List_List*_tmpAB5=Cyc_yyget_YY54(yyyyvsp[1]);_tmp518->tl=_tmpAB5;});_tmp518;}));yyval=_tmpAB7;});
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2076 "parse.y"
({union Cyc_YYSTYPE _tmpAB9=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp519=_region_malloc(yyr,sizeof(*_tmp519));_tmp519->tag=0U,({struct Cyc_Absyn_Exp*_tmpAB8=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp519->f1=_tmpAB8;});_tmp519;}));yyval=_tmpAB9;});
goto _LL0;}case 195U: _LL181: _LL182: {
# 2079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2078 "parse.y"
({union Cyc_YYSTYPE _tmpABB=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp51A=_region_malloc(yyr,sizeof(*_tmp51A));_tmp51A->tag=1U,({void*_tmpABA=Cyc_yyget_YY44(yyyyvsp[2]);_tmp51A->f1=_tmpABA;});_tmp51A;}));yyval=_tmpABB;});
goto _LL0;}case 196U: _LL183: _LL184: {
# 2081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2080 "parse.y"
({union Cyc_YYSTYPE _tmpABC=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp51B=_region_malloc(yyr,sizeof(*_tmp51B));_tmp51B->tag=2U;_tmp51B;}));yyval=_tmpABC;});
goto _LL0;}case 197U: _LL185: _LL186: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2082 "parse.y"
({union Cyc_YYSTYPE _tmpABD=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp51C=_region_malloc(yyr,sizeof(*_tmp51C));_tmp51C->tag=3U;_tmp51C;}));yyval=_tmpABD;});
goto _LL0;}case 198U: _LL187: _LL188: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2084 "parse.y"
({union Cyc_YYSTYPE _tmpABE=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp51D=_region_malloc(yyr,sizeof(*_tmp51D));_tmp51D->tag=4U;_tmp51D;}));yyval=_tmpABE;});
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2086 "parse.y"
({union Cyc_YYSTYPE _tmpABF=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp51E=_region_malloc(yyr,sizeof(*_tmp51E));_tmp51E->tag=5U;_tmp51E;}));yyval=_tmpABF;});
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2088 "parse.y"
({union Cyc_YYSTYPE _tmpAC0=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp51F=_region_malloc(yyr,sizeof(*_tmp51F));_tmp51F->tag=6U;_tmp51F;}));yyval=_tmpAC0;});
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2090 "parse.y"
({union Cyc_YYSTYPE _tmpAC1=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp520=_region_malloc(yyr,sizeof(*_tmp520));_tmp520->tag=7U;_tmp520;}));yyval=_tmpAC1;});
goto _LL0;}case 202U: _LL18F: _LL190: {
# 2093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2096 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpAC3=Cyc_YY1(({struct _tuple21*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->f1=loc,_tmp521->f2=Cyc_Absyn_true_conref,({union Cyc_Absyn_Constraint*_tmpAC2=Cyc_yyget_YY2(yyyyvsp[1]);_tmp521->f3=_tmpAC2;});_tmp521;}));yyval=_tmpAC3;});
# 2099
goto _LL0;}case 203U: _LL191: _LL192: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2100 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpAC5=Cyc_YY1(({struct _tuple21*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->f1=loc,_tmp522->f2=Cyc_Absyn_false_conref,({union Cyc_Absyn_Constraint*_tmpAC4=Cyc_yyget_YY2(yyyyvsp[1]);_tmp522->f3=_tmpAC4;});_tmp522;}));yyval=_tmpAC5;});
# 2103
goto _LL0;}case 204U: _LL193: _LL194: {
# 2105
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2107
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2104 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpAC6=Cyc_YY1(({struct _tuple21*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->f1=loc,_tmp523->f2=Cyc_Absyn_true_conref,_tmp523->f3=Cyc_Absyn_bounds_dyneither_conref;_tmp523;}));yyval=_tmpAC6;});
# 2107
goto _LL0;}case 205U: _LL195: _LL196:
# 2109
({union Cyc_YYSTYPE _tmpAC7=Cyc_YY2(Cyc_Absyn_bounds_one_conref);yyval=_tmpAC7;});
goto _LL0;case 206U: _LL197: _LL198: {
# 2112
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2110 "parse.y"
({union Cyc_YYSTYPE _tmpAC9=Cyc_YY2(((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->tag=1U,({struct Cyc_Absyn_Exp*_tmpAC8=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp524->f1=_tmpAC8;});_tmp524;})));yyval=_tmpAC9;});
goto _LL0;}case 207U: _LL199: _LL19A:
# 2113
({union Cyc_YYSTYPE _tmpACA=Cyc_YY51(((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());yyval=_tmpACA;});
goto _LL0;case 208U: _LL19B: _LL19C: {
# 2116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2114 "parse.y"
({union Cyc_YYSTYPE _tmpACB=Cyc_YY51(Cyc_Absyn_true_conref);yyval=_tmpACB;});
goto _LL0;}case 209U: _LL19D: _LL19E: {
# 2117
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2115 "parse.y"
({union Cyc_YYSTYPE _tmpACC=Cyc_YY51(Cyc_Absyn_false_conref);yyval=_tmpACC;});
goto _LL0;}case 210U: _LL19F: _LL1A0:
# 2118
({union Cyc_YYSTYPE _tmpACD=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpACD;});
goto _LL0;case 211U: _LL1A1: _LL1A2: {
# 2121
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2123
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2121 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL0;}case 212U: _LL1A3: _LL1A4: {
# 2124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2122 "parse.y"
({union Cyc_YYSTYPE _tmpACE=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpACE;});
goto _LL0;}case 213U: _LL1A5: _LL1A6:
# 2125
({union Cyc_YYSTYPE _tmpACF=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));yyval=_tmpACF;});
goto _LL0;case 214U: _LL1A7: _LL1A8: {
# 2128
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2130
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2127 "parse.y"
({union Cyc_YYSTYPE _tmpAD1=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmpAD0=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpAD0,Cyc_yyget_YY23(yyyyvsp[1]));}));yyval=_tmpAD1;});
goto _LL0;}case 215U: _LL1A9: _LL1AA: {
# 2130
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2132
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2132 "parse.y"
({union Cyc_YYSTYPE _tmpAD5=Cyc_YY39(({struct _tuple26*_tmp525=_cycalloc(sizeof(*_tmp525));({struct Cyc_List_List*_tmpAD4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp525->f1=_tmpAD4;}),_tmp525->f2=0,_tmp525->f3=0,({void*_tmpAD3=Cyc_yyget_YY49(yyyyvsp[1]);_tmp525->f4=_tmpAD3;}),({struct Cyc_List_List*_tmpAD2=Cyc_yyget_YY50(yyyyvsp[2]);_tmp525->f5=_tmpAD2;});_tmp525;}));yyval=_tmpAD5;});
goto _LL0;}case 216U: _LL1AB: _LL1AC: {
# 2135
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2137
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2134 "parse.y"
({union Cyc_YYSTYPE _tmpAD9=Cyc_YY39(({struct _tuple26*_tmp526=_cycalloc(sizeof(*_tmp526));({struct Cyc_List_List*_tmpAD8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp526->f1=_tmpAD8;}),_tmp526->f2=1,_tmp526->f3=0,({void*_tmpAD7=Cyc_yyget_YY49(yyyyvsp[3]);_tmp526->f4=_tmpAD7;}),({struct Cyc_List_List*_tmpAD6=Cyc_yyget_YY50(yyyyvsp[4]);_tmp526->f5=_tmpAD6;});_tmp526;}));yyval=_tmpAD9;});
goto _LL0;}case 217U: _LL1AD: _LL1AE: {
# 2137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2137 "parse.y"
struct _tuple8*_tmp527=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp528=_tmp527;struct _dyneither_ptr*_tmp52E;struct Cyc_Absyn_Tqual _tmp52D;void*_tmp52C;_LL449: _tmp52E=_tmp528->f1;_tmp52D=_tmp528->f2;_tmp52C=_tmp528->f3;_LL44A:;{
struct Cyc_Absyn_VarargInfo*_tmp529=({struct Cyc_Absyn_VarargInfo*_tmp52B=_cycalloc(sizeof(*_tmp52B));_tmp52B->name=_tmp52E,_tmp52B->tq=_tmp52D,_tmp52B->type=_tmp52C,({int _tmpADA=Cyc_yyget_YY31(yyyyvsp[1]);_tmp52B->inject=_tmpADA;});_tmp52B;});
({union Cyc_YYSTYPE _tmpADD=Cyc_YY39(({struct _tuple26*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->f1=0,_tmp52A->f2=0,_tmp52A->f3=_tmp529,({void*_tmpADC=Cyc_yyget_YY49(yyyyvsp[3]);_tmp52A->f4=_tmpADC;}),({struct Cyc_List_List*_tmpADB=Cyc_yyget_YY50(yyyyvsp[4]);_tmp52A->f5=_tmpADB;});_tmp52A;}));yyval=_tmpADD;});
# 2141
goto _LL0;};}case 218U: _LL1AF: _LL1B0: {
# 2143
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2145
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2143 "parse.y"
struct _tuple8*_tmp52F=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp530=_tmp52F;struct _dyneither_ptr*_tmp536;struct Cyc_Absyn_Tqual _tmp535;void*_tmp534;_LL44C: _tmp536=_tmp530->f1;_tmp535=_tmp530->f2;_tmp534=_tmp530->f3;_LL44D:;{
struct Cyc_Absyn_VarargInfo*_tmp531=({struct Cyc_Absyn_VarargInfo*_tmp533=_cycalloc(sizeof(*_tmp533));_tmp533->name=_tmp536,_tmp533->tq=_tmp535,_tmp533->type=_tmp534,({int _tmpADE=Cyc_yyget_YY31(yyyyvsp[3]);_tmp533->inject=_tmpADE;});_tmp533;});
({union Cyc_YYSTYPE _tmpAE2=Cyc_YY39(({struct _tuple26*_tmp532=_cycalloc(sizeof(*_tmp532));({struct Cyc_List_List*_tmpAE1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp532->f1=_tmpAE1;}),_tmp532->f2=0,_tmp532->f3=_tmp531,({void*_tmpAE0=Cyc_yyget_YY49(yyyyvsp[5]);_tmp532->f4=_tmpAE0;}),({struct Cyc_List_List*_tmpADF=Cyc_yyget_YY50(yyyyvsp[6]);_tmp532->f5=_tmpADF;});_tmp532;}));yyval=_tmpAE2;});
# 2147
goto _LL0;};}case 219U: _LL1B1: _LL1B2: {
# 2149
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2151
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2151 "parse.y"
({union Cyc_YYSTYPE _tmpAE4=Cyc_YY44(({struct _dyneither_ptr _tmpAE3=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpAE3,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp537=_cycalloc(sizeof(*_tmp537));_tmp537->tag=1U,_tmp537->f1=0;_tmp537;}));}));yyval=_tmpAE4;});
goto _LL0;}case 220U: _LL1B3: _LL1B4: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2152 "parse.y"
({union Cyc_YYSTYPE _tmpAE6=Cyc_YY44(({struct _dyneither_ptr _tmpAE5=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpAE5,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));yyval=_tmpAE6;});
goto _LL0;}case 221U: _LL1B5: _LL1B6:
# 2155
({union Cyc_YYSTYPE _tmpAE7=Cyc_YY49(0);yyval=_tmpAE7;});
goto _LL0;case 222U: _LL1B7: _LL1B8: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2156 "parse.y"
({union Cyc_YYSTYPE _tmpAE9=Cyc_YY49((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp538=_cycalloc(sizeof(*_tmp538));_tmp538->tag=24U,({struct Cyc_List_List*_tmpAE8=Cyc_yyget_YY40(yyyyvsp[1]);_tmp538->f1=_tmpAE8;});_tmp538;}));yyval=_tmpAE9;});
goto _LL0;}case 223U: _LL1B9: _LL1BA:
# 2159
({union Cyc_YYSTYPE _tmpAEA=Cyc_YY50(0);yyval=_tmpAEA;});
goto _LL0;case 224U: _LL1BB: _LL1BC: {
# 2162
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2164
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2161 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 225U: _LL1BD: _LL1BE: {
# 2164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2169 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp539=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->tag=2U,_tmp53F->f1=0,_tmp53F->f2=& Cyc_Tcutil_trk;_tmp53F;});
struct _dyneither_ptr _tmp53A=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp53B=Cyc_Parse_id2type(_tmp53A,(void*)_tmp539);
({union Cyc_YYSTYPE _tmpAEE=Cyc_YY50(({struct Cyc_List_List*_tmp53E=_cycalloc(sizeof(*_tmp53E));({struct _tuple29*_tmpAED=({struct _tuple29*_tmp53D=_cycalloc(sizeof(*_tmp53D));({void*_tmpAEC=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->tag=24U,({struct Cyc_List_List*_tmpAEB=Cyc_yyget_YY40(yyyyvsp[0]);_tmp53C->f1=_tmpAEB;});_tmp53C;});_tmp53D->f1=_tmpAEC;}),_tmp53D->f2=_tmp53B;_tmp53D;});_tmp53E->hd=_tmpAED;}),_tmp53E->tl=0;_tmp53E;}));yyval=_tmpAEE;});
# 2174
goto _LL0;}case 226U: _LL1BF: _LL1C0: {
# 2176
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2178
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2176 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp540=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->tag=2U,_tmp546->f1=0,_tmp546->f2=& Cyc_Tcutil_trk;_tmp546;});
struct _dyneither_ptr _tmp541=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp542=Cyc_Parse_id2type(_tmp541,(void*)_tmp540);
({union Cyc_YYSTYPE _tmpAF3=Cyc_YY50(({struct Cyc_List_List*_tmp545=_cycalloc(sizeof(*_tmp545));({struct _tuple29*_tmpAF2=({struct _tuple29*_tmp544=_cycalloc(sizeof(*_tmp544));({void*_tmpAF1=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp543=_cycalloc(sizeof(*_tmp543));_tmp543->tag=24U,({struct Cyc_List_List*_tmpAF0=Cyc_yyget_YY40(yyyyvsp[0]);_tmp543->f1=_tmpAF0;});_tmp543;});_tmp544->f1=_tmpAF1;}),_tmp544->f2=_tmp542;_tmp544;});_tmp545->hd=_tmpAF2;}),({struct Cyc_List_List*_tmpAEF=Cyc_yyget_YY50(yyyyvsp[4]);_tmp545->tl=_tmpAEF;});_tmp545;}));yyval=_tmpAF3;});
# 2181
goto _LL0;}case 227U: _LL1C1: _LL1C2:
# 2183
({union Cyc_YYSTYPE _tmpAF4=Cyc_YY31(0);yyval=_tmpAF4;});
goto _LL0;case 228U: _LL1C3: _LL1C4: {
# 2186
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2188
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2187 "parse.y"
if(({struct _dyneither_ptr _tmpAF5=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmpAF5,({const char*_tmp547="inject";_tag_dyneither(_tmp547,sizeof(char),7U);}));})!= 0)
({void*_tmp548=0U;({unsigned int _tmpAF7=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAF6=({const char*_tmp549="missing type in function declaration";_tag_dyneither(_tmp549,sizeof(char),37U);});Cyc_Warn_err(_tmpAF7,_tmpAF6,_tag_dyneither(_tmp548,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpAF8=Cyc_YY31(1);yyval=_tmpAF8;});
# 2191
goto _LL0;}case 229U: _LL1C5: _LL1C6: {
# 2193
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2195
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2194 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 230U: _LL1C7: _LL1C8: {
# 2197
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2199
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2195 "parse.y"
({union Cyc_YYSTYPE _tmpAFA=Cyc_YY40(({struct Cyc_List_List*_tmpAF9=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpAF9,Cyc_yyget_YY40(yyyyvsp[2]));}));yyval=_tmpAFA;});
goto _LL0;}case 231U: _LL1C9: _LL1CA: {
# 2198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2199 "parse.y"
({union Cyc_YYSTYPE _tmpAFB=Cyc_YY40(0);yyval=_tmpAFB;});
goto _LL0;}case 232U: _LL1CB: _LL1CC: {
# 2202
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2204
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2200 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 233U: _LL1CD: _LL1CE: {
# 2203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2202 "parse.y"
({union Cyc_YYSTYPE _tmpAFE=Cyc_YY40(({struct Cyc_List_List*_tmp54B=_cycalloc(sizeof(*_tmp54B));({void*_tmpAFD=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->tag=25U,({void*_tmpAFC=Cyc_yyget_YY44(yyyyvsp[2]);_tmp54A->f1=_tmpAFC;});_tmp54A;});_tmp54B->hd=_tmpAFD;}),_tmp54B->tl=0;_tmp54B;}));yyval=_tmpAFE;});
goto _LL0;}case 234U: _LL1CF: _LL1D0: {
# 2205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2204 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
({union Cyc_YYSTYPE _tmpB00=Cyc_YY40(({struct Cyc_List_List*_tmp54C=_cycalloc(sizeof(*_tmp54C));({void*_tmpAFF=Cyc_yyget_YY44(yyyyvsp[0]);_tmp54C->hd=_tmpAFF;}),_tmp54C->tl=0;_tmp54C;}));yyval=_tmpB00;});
# 2207
goto _LL0;}case 235U: _LL1D1: _LL1D2: {
# 2209
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2211
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2224 "parse.y"
({union Cyc_YYSTYPE _tmpB04=Cyc_YY40(({struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*_tmp54E));({void*_tmpB03=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->tag=23U,({void*_tmpB02=({struct _tuple8*_tmpB01=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpB01,(unsigned int)(yyyylsp[0]).first_line);});_tmp54D->f1=_tmpB02;});_tmp54D;});_tmp54E->hd=_tmpB03;}),_tmp54E->tl=0;_tmp54E;}));yyval=_tmpB04;});
goto _LL0;}case 236U: _LL1D3: _LL1D4: {
# 2227
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2229
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2226 "parse.y"
({union Cyc_YYSTYPE _tmpB09=Cyc_YY40(({struct Cyc_List_List*_tmp550=_cycalloc(sizeof(*_tmp550));({void*_tmpB08=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->tag=23U,({void*_tmpB07=({struct _tuple8*_tmpB06=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpB06,(unsigned int)(yyyylsp[0]).first_line);});_tmp54F->f1=_tmpB07;});_tmp54F;});_tmp550->hd=_tmpB08;}),({struct Cyc_List_List*_tmpB05=Cyc_yyget_YY40(yyyyvsp[2]);_tmp550->tl=_tmpB05;});_tmp550;}));yyval=_tmpB09;});
goto _LL0;}case 237U: _LL1D5: _LL1D6: {
# 2229
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2231
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB0B=Cyc_YY38(({struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));({struct _tuple8*_tmpB0A=Cyc_yyget_YY37(yyyyvsp[0]);_tmp551->hd=_tmpB0A;}),_tmp551->tl=0;_tmp551;}));yyval=_tmpB0B;});
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2235
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2237
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2234 "parse.y"
({union Cyc_YYSTYPE _tmpB0E=Cyc_YY38(({struct Cyc_List_List*_tmp552=_cycalloc(sizeof(*_tmp552));({struct _tuple8*_tmpB0D=Cyc_yyget_YY37(yyyyvsp[2]);_tmp552->hd=_tmpB0D;}),({struct Cyc_List_List*_tmpB0C=Cyc_yyget_YY38(yyyyvsp[0]);_tmp552->tl=_tmpB0C;});_tmp552;}));yyval=_tmpB0E;});
goto _LL0;}case 239U: _LL1D9: _LL1DA: {
# 2237
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2239
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
struct _tuple25 _tmp553=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp554=_tmp553;struct Cyc_Absyn_Tqual _tmp56A;struct Cyc_Parse_Type_specifier _tmp569;struct Cyc_List_List*_tmp568;_LL44F: _tmp56A=_tmp554.f1;_tmp569=_tmp554.f2;_tmp568=_tmp554.f3;_LL450:;
if(_tmp56A.loc == 0)_tmp56A.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp555=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp556=_tmp555;struct _tuple0*_tmp567;unsigned int _tmp566;struct Cyc_List_List*_tmp565;_LL452: _tmp567=_tmp556.id;_tmp566=_tmp556.varloc;_tmp565=_tmp556.tms;_LL453:;{
void*_tmp557=Cyc_Parse_speclist2typ(_tmp569,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp558=Cyc_Parse_apply_tms(_tmp56A,_tmp557,_tmp568,_tmp565);struct _tuple13 _tmp559=_tmp558;struct Cyc_Absyn_Tqual _tmp564;void*_tmp563;struct Cyc_List_List*_tmp562;struct Cyc_List_List*_tmp561;_LL455: _tmp564=_tmp559.f1;_tmp563=_tmp559.f2;_tmp562=_tmp559.f3;_tmp561=_tmp559.f4;_LL456:;
if(_tmp562 != 0)
({void*_tmp55A=0U;({unsigned int _tmpB10=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB0F=({const char*_tmp55B="parameter with bad type params";_tag_dyneither(_tmp55B,sizeof(char),31U);});Cyc_Warn_err(_tmpB10,_tmpB0F,_tag_dyneither(_tmp55A,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp567))
({void*_tmp55C=0U;({unsigned int _tmpB12=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB11=({const char*_tmp55D="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp55D,sizeof(char),47U);});Cyc_Warn_err(_tmpB12,_tmpB11,_tag_dyneither(_tmp55C,sizeof(void*),0U));});});{
struct _dyneither_ptr*idopt=(*_tmp567).f2;
if(_tmp561 != 0)
({void*_tmp55E=0U;({unsigned int _tmpB14=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB13=({const char*_tmp55F="extra attributes on parameter, ignoring";_tag_dyneither(_tmp55F,sizeof(char),40U);});Cyc_Warn_warn(_tmpB14,_tmpB13,_tag_dyneither(_tmp55E,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB15=Cyc_YY37(({struct _tuple8*_tmp560=_cycalloc(sizeof(*_tmp560));_tmp560->f1=idopt,_tmp560->f2=_tmp564,_tmp560->f3=_tmp563;_tmp560;}));yyval=_tmpB15;});
# 2254
goto _LL0;};};};}case 240U: _LL1DB: _LL1DC: {
# 2256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2255 "parse.y"
struct _tuple25 _tmp56B=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp56C=_tmp56B;struct Cyc_Absyn_Tqual _tmp573;struct Cyc_Parse_Type_specifier _tmp572;struct Cyc_List_List*_tmp571;_LL458: _tmp573=_tmp56C.f1;_tmp572=_tmp56C.f2;_tmp571=_tmp56C.f3;_LL459:;
if(_tmp573.loc == 0)_tmp573.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp56D=Cyc_Parse_speclist2typ(_tmp572,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp571 != 0)
({void*_tmp56E=0U;({unsigned int _tmpB17=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB16=({const char*_tmp56F="bad attributes on parameter, ignoring";_tag_dyneither(_tmp56F,sizeof(char),38U);});Cyc_Warn_warn(_tmpB17,_tmpB16,_tag_dyneither(_tmp56E,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB18=Cyc_YY37(({struct _tuple8*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->f1=0,_tmp570->f2=_tmp573,_tmp570->f3=_tmp56D;_tmp570;}));yyval=_tmpB18;});
# 2262
goto _LL0;};}case 241U: _LL1DD: _LL1DE: {
# 2264
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2266
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2263 "parse.y"
struct _tuple25 _tmp574=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp575=_tmp574;struct Cyc_Absyn_Tqual _tmp585;struct Cyc_Parse_Type_specifier _tmp584;struct Cyc_List_List*_tmp583;_LL45B: _tmp585=_tmp575.f1;_tmp584=_tmp575.f2;_tmp583=_tmp575.f3;_LL45C:;
if(_tmp585.loc == 0)_tmp585.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp576=Cyc_Parse_speclist2typ(_tmp584,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp577=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp578=Cyc_Parse_apply_tms(_tmp585,_tmp576,_tmp583,_tmp577);struct _tuple13 _tmp579=_tmp578;struct Cyc_Absyn_Tqual _tmp582;void*_tmp581;struct Cyc_List_List*_tmp580;struct Cyc_List_List*_tmp57F;_LL45E: _tmp582=_tmp579.f1;_tmp581=_tmp579.f2;_tmp580=_tmp579.f3;_tmp57F=_tmp579.f4;_LL45F:;
if(_tmp580 != 0)
({void*_tmp57A=0U;({unsigned int _tmpB1A=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB19=({const char*_tmp57B="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp57B,sizeof(char),49U);});Cyc_Warn_warn(_tmpB1A,_tmpB19,_tag_dyneither(_tmp57A,sizeof(void*),0U));});});
# 2271
if(_tmp57F != 0)
({void*_tmp57C=0U;({unsigned int _tmpB1C=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB1B=({const char*_tmp57D="bad attributes on parameter, ignoring";_tag_dyneither(_tmp57D,sizeof(char),38U);});Cyc_Warn_warn(_tmpB1C,_tmpB1B,_tag_dyneither(_tmp57C,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB1D=Cyc_YY37(({struct _tuple8*_tmp57E=_cycalloc(sizeof(*_tmp57E));_tmp57E->f1=0,_tmp57E->f2=_tmp582,_tmp57E->f3=_tmp581;_tmp57E;}));yyval=_tmpB1D;});
# 2275
goto _LL0;};}case 242U: _LL1DF: _LL1E0: {
# 2277
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2279
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2279 "parse.y"
({union Cyc_YYSTYPE _tmpB1E=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));yyval=_tmpB1E;});
goto _LL0;}case 243U: _LL1E1: _LL1E2: {
# 2282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2283 "parse.y"
({union Cyc_YYSTYPE _tmpB21=Cyc_YY36(({struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));({struct _dyneither_ptr*_tmpB20=({struct _dyneither_ptr*_tmp586=_cycalloc(sizeof(*_tmp586));({struct _dyneither_ptr _tmpB1F=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp586=_tmpB1F;});_tmp586;});_tmp587->hd=_tmpB20;}),_tmp587->tl=0;_tmp587;}));yyval=_tmpB21;});
goto _LL0;}case 244U: _LL1E3: _LL1E4: {
# 2286
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2288
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2285 "parse.y"
({union Cyc_YYSTYPE _tmpB25=Cyc_YY36(({struct Cyc_List_List*_tmp589=_cycalloc(sizeof(*_tmp589));({struct _dyneither_ptr*_tmpB24=({struct _dyneither_ptr*_tmp588=_cycalloc(sizeof(*_tmp588));({struct _dyneither_ptr _tmpB23=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp588=_tmpB23;});_tmp588;});_tmp589->hd=_tmpB24;}),({struct Cyc_List_List*_tmpB22=Cyc_yyget_YY36(yyyyvsp[0]);_tmp589->tl=_tmpB22;});_tmp589;}));yyval=_tmpB25;});
goto _LL0;}case 245U: _LL1E5: _LL1E6: {
# 2288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2289 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 246U: _LL1E7: _LL1E8: {
# 2292
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2294
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2290 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 247U: _LL1E9: _LL1EA: {
# 2293
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2295
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2295 "parse.y"
({union Cyc_YYSTYPE _tmpB27=Cyc_Exp_tok(({void*_tmpB26=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58A=_cycalloc(sizeof(*_tmp58A));_tmp58A->tag=36U,_tmp58A->f1=0,_tmp58A->f2=0;_tmp58A;});Cyc_Absyn_new_exp(_tmpB26,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB27;});
goto _LL0;}case 248U: _LL1EB: _LL1EC: {
# 2298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2297 "parse.y"
({union Cyc_YYSTYPE _tmpB2A=Cyc_Exp_tok(({void*_tmpB29=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->tag=36U,_tmp58B->f1=0,({struct Cyc_List_List*_tmpB28=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp58B->f2=_tmpB28;});_tmp58B;});Cyc_Absyn_new_exp(_tmpB29,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB2A;});
goto _LL0;}case 249U: _LL1ED: _LL1EE: {
# 2300
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2302
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2299 "parse.y"
({union Cyc_YYSTYPE _tmpB2D=Cyc_Exp_tok(({void*_tmpB2C=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp58C=_cycalloc(sizeof(*_tmp58C));_tmp58C->tag=36U,_tmp58C->f1=0,({struct Cyc_List_List*_tmpB2B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp58C->f2=_tmpB2B;});_tmp58C;});Cyc_Absyn_new_exp(_tmpB2C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB2D;});
goto _LL0;}case 250U: _LL1EF: _LL1F0: {
# 2302
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2304
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2301 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp58D=({unsigned int _tmpB32=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmpB31=({struct _tuple0*_tmp590=_cycalloc(sizeof(*_tmp590));_tmp590->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpB2F=({struct _dyneither_ptr*_tmp58F=_cycalloc(sizeof(*_tmp58F));({struct _dyneither_ptr _tmpB2E=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp58F=_tmpB2E;});_tmp58F;});_tmp590->f2=_tmpB2F;});_tmp590;});void*_tmpB30=Cyc_Absyn_uint_typ;Cyc_Absyn_new_vardecl(_tmpB32,_tmpB31,_tmpB30,
Cyc_Absyn_uint_exp(0U,(unsigned int)(yyyylsp[2]).first_line));});
# 2304
(_tmp58D->tq).real_const=1;
({union Cyc_YYSTYPE _tmpB36=Cyc_Exp_tok(({void*_tmpB35=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp58E=_cycalloc(sizeof(*_tmp58E));_tmp58E->tag=27U,_tmp58E->f1=_tmp58D,({struct Cyc_Absyn_Exp*_tmpB34=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp58E->f2=_tmpB34;}),({struct Cyc_Absyn_Exp*_tmpB33=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp58E->f3=_tmpB33;}),_tmp58E->f4=0;_tmp58E;});Cyc_Absyn_new_exp(_tmpB35,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB36;});
# 2307
goto _LL0;}case 251U: _LL1F1: _LL1F2: {
# 2309
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2311
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2309 "parse.y"
void*_tmp591=({struct _tuple8*_tmpB37=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpB37,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpB3A=Cyc_Exp_tok(({void*_tmpB39=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->tag=28U,({struct Cyc_Absyn_Exp*_tmpB38=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp592->f1=_tmpB38;}),_tmp592->f2=_tmp591,_tmp592->f3=0;_tmp592;});Cyc_Absyn_new_exp(_tmpB39,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB3A;});
# 2312
goto _LL0;}case 252U: _LL1F3: _LL1F4: {
# 2314
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2316
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB3D=Cyc_YY5(({struct Cyc_List_List*_tmp594=_cycalloc(sizeof(*_tmp594));({struct _tuple30*_tmpB3C=({struct _tuple30*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->f1=0,({struct Cyc_Absyn_Exp*_tmpB3B=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp593->f2=_tmpB3B;});_tmp593;});_tmp594->hd=_tmpB3C;}),_tmp594->tl=0;_tmp594;}));yyval=_tmpB3D;});
goto _LL0;}case 253U: _LL1F5: _LL1F6: {
# 2320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2319 "parse.y"
({union Cyc_YYSTYPE _tmpB41=Cyc_YY5(({struct Cyc_List_List*_tmp596=_cycalloc(sizeof(*_tmp596));({struct _tuple30*_tmpB40=({struct _tuple30*_tmp595=_cycalloc(sizeof(*_tmp595));({struct Cyc_List_List*_tmpB3F=Cyc_yyget_YY41(yyyyvsp[0]);_tmp595->f1=_tmpB3F;}),({struct Cyc_Absyn_Exp*_tmpB3E=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp595->f2=_tmpB3E;});_tmp595;});_tmp596->hd=_tmpB40;}),_tmp596->tl=0;_tmp596;}));yyval=_tmpB41;});
goto _LL0;}case 254U: _LL1F7: _LL1F8: {
# 2322
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2321 "parse.y"
({union Cyc_YYSTYPE _tmpB45=Cyc_YY5(({struct Cyc_List_List*_tmp598=_cycalloc(sizeof(*_tmp598));({struct _tuple30*_tmpB44=({struct _tuple30*_tmp597=_cycalloc(sizeof(*_tmp597));_tmp597->f1=0,({struct Cyc_Absyn_Exp*_tmpB43=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp597->f2=_tmpB43;});_tmp597;});_tmp598->hd=_tmpB44;}),({struct Cyc_List_List*_tmpB42=Cyc_yyget_YY5(yyyyvsp[0]);_tmp598->tl=_tmpB42;});_tmp598;}));yyval=_tmpB45;});
goto _LL0;}case 255U: _LL1F9: _LL1FA: {
# 2324
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2326
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2323 "parse.y"
({union Cyc_YYSTYPE _tmpB4A=Cyc_YY5(({struct Cyc_List_List*_tmp59A=_cycalloc(sizeof(*_tmp59A));({struct _tuple30*_tmpB49=({struct _tuple30*_tmp599=_cycalloc(sizeof(*_tmp599));({struct Cyc_List_List*_tmpB48=Cyc_yyget_YY41(yyyyvsp[2]);_tmp599->f1=_tmpB48;}),({struct Cyc_Absyn_Exp*_tmpB47=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp599->f2=_tmpB47;});_tmp599;});_tmp59A->hd=_tmpB49;}),({struct Cyc_List_List*_tmpB46=Cyc_yyget_YY5(yyyyvsp[0]);_tmp59A->tl=_tmpB46;});_tmp59A;}));yyval=_tmpB4A;});
goto _LL0;}case 256U: _LL1FB: _LL1FC: {
# 2326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2327 "parse.y"
({union Cyc_YYSTYPE _tmpB4B=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));yyval=_tmpB4B;});
goto _LL0;}case 257U: _LL1FD: _LL1FE: {
# 2330
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2332
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2328 "parse.y"
({union Cyc_YYSTYPE _tmpB4F=Cyc_YY41(({struct Cyc_List_List*_tmp59D=_cycalloc(sizeof(*_tmp59D));({void*_tmpB4E=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->tag=1U,({struct _dyneither_ptr*_tmpB4D=({struct _dyneither_ptr*_tmp59B=_cycalloc(sizeof(*_tmp59B));({struct _dyneither_ptr _tmpB4C=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp59B=_tmpB4C;});_tmp59B;});_tmp59C->f1=_tmpB4D;});_tmp59C;});_tmp59D->hd=_tmpB4E;}),_tmp59D->tl=0;_tmp59D;}));yyval=_tmpB4F;});
goto _LL0;}case 258U: _LL1FF: _LL200: {
# 2331
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2333
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2333 "parse.y"
({union Cyc_YYSTYPE _tmpB51=Cyc_YY41(({struct Cyc_List_List*_tmp59E=_cycalloc(sizeof(*_tmp59E));({void*_tmpB50=Cyc_yyget_YY42(yyyyvsp[0]);_tmp59E->hd=_tmpB50;}),_tmp59E->tl=0;_tmp59E;}));yyval=_tmpB51;});
goto _LL0;}case 259U: _LL201: _LL202: {
# 2336
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2338
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2334 "parse.y"
({union Cyc_YYSTYPE _tmpB54=Cyc_YY41(({struct Cyc_List_List*_tmp59F=_cycalloc(sizeof(*_tmp59F));({void*_tmpB53=Cyc_yyget_YY42(yyyyvsp[1]);_tmp59F->hd=_tmpB53;}),({struct Cyc_List_List*_tmpB52=Cyc_yyget_YY41(yyyyvsp[0]);_tmp59F->tl=_tmpB52;});_tmp59F;}));yyval=_tmpB54;});
goto _LL0;}case 260U: _LL203: _LL204: {
# 2337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2338 "parse.y"
({union Cyc_YYSTYPE _tmpB56=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp5A0=_cycalloc(sizeof(*_tmp5A0));_tmp5A0->tag=0U,({struct Cyc_Absyn_Exp*_tmpB55=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp5A0->f1=_tmpB55;});_tmp5A0;}));yyval=_tmpB56;});
goto _LL0;}case 261U: _LL205: _LL206: {
# 2341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2339 "parse.y"
({union Cyc_YYSTYPE _tmpB59=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->tag=1U,({struct _dyneither_ptr*_tmpB58=({struct _dyneither_ptr*_tmp5A1=_cycalloc(sizeof(*_tmp5A1));({struct _dyneither_ptr _tmpB57=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp5A1=_tmpB57;});_tmp5A1;});_tmp5A2->f1=_tmpB58;});_tmp5A2;}));yyval=_tmpB59;});
goto _LL0;}case 262U: _LL207: _LL208: {
# 2342
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2344
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2344 "parse.y"
struct _tuple25 _tmp5A3=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp5A4=_tmp5A3;struct Cyc_Absyn_Tqual _tmp5AB;struct Cyc_Parse_Type_specifier _tmp5AA;struct Cyc_List_List*_tmp5A9;_LL461: _tmp5AB=_tmp5A4.f1;_tmp5AA=_tmp5A4.f2;_tmp5A9=_tmp5A4.f3;_LL462:;{
void*_tmp5A5=Cyc_Parse_speclist2typ(_tmp5AA,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp5A9 != 0)
({void*_tmp5A6=0U;({unsigned int _tmpB5B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB5A=({const char*_tmp5A7="ignoring attributes in type";_tag_dyneither(_tmp5A7,sizeof(char),28U);});Cyc_Warn_warn(_tmpB5B,_tmpB5A,_tag_dyneither(_tmp5A6,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB5C=Cyc_YY37(({struct _tuple8*_tmp5A8=_cycalloc(sizeof(*_tmp5A8));_tmp5A8->f1=0,_tmp5A8->f2=_tmp5AB,_tmp5A8->f3=_tmp5A5;_tmp5A8;}));yyval=_tmpB5C;});
# 2350
goto _LL0;};}case 263U: _LL209: _LL20A: {
# 2352
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2354
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2351 "parse.y"
struct _tuple25 _tmp5AC=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp5AD=_tmp5AC;struct Cyc_Absyn_Tqual _tmp5B8;struct Cyc_Parse_Type_specifier _tmp5B7;struct Cyc_List_List*_tmp5B6;_LL464: _tmp5B8=_tmp5AD.f1;_tmp5B7=_tmp5AD.f2;_tmp5B6=_tmp5AD.f3;_LL465:;{
void*_tmp5AE=Cyc_Parse_speclist2typ(_tmp5B7,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp5AF=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp5B0=Cyc_Parse_apply_tms(_tmp5B8,_tmp5AE,_tmp5B6,_tmp5AF);
if(_tmp5B0.f3 != 0)
# 2357
({void*_tmp5B1=0U;({unsigned int _tmpB5E=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB5D=({const char*_tmp5B2="bad type params, ignoring";_tag_dyneither(_tmp5B2,sizeof(char),26U);});Cyc_Warn_warn(_tmpB5E,_tmpB5D,_tag_dyneither(_tmp5B1,sizeof(void*),0U));});});
if(_tmp5B0.f4 != 0)
({void*_tmp5B3=0U;({unsigned int _tmpB60=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB5F=({const char*_tmp5B4="bad specifiers, ignoring";_tag_dyneither(_tmp5B4,sizeof(char),25U);});Cyc_Warn_warn(_tmpB60,_tmpB5F,_tag_dyneither(_tmp5B3,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB61=Cyc_YY37(({struct _tuple8*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->f1=0,_tmp5B5->f2=_tmp5B0.f1,_tmp5B5->f3=_tmp5B0.f2;_tmp5B5;}));yyval=_tmpB61;});
# 2362
goto _LL0;};}case 264U: _LL20B: _LL20C: {
# 2364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2365 "parse.y"
({union Cyc_YYSTYPE _tmpB63=Cyc_YY44(({struct _tuple8*_tmpB62=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpB62,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB63;});
goto _LL0;}case 265U: _LL20D: _LL20E: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2366 "parse.y"
({union Cyc_YYSTYPE _tmpB64=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->tag=24U,_tmp5B9->f1=0;_tmp5B9;}));yyval=_tmpB64;});
goto _LL0;}case 266U: _LL20F: _LL210: {
# 2369
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2371
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2367 "parse.y"
({union Cyc_YYSTYPE _tmpB66=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->tag=24U,({struct Cyc_List_List*_tmpB65=Cyc_yyget_YY40(yyyyvsp[1]);_tmp5BA->f1=_tmpB65;});_tmp5BA;}));yyval=_tmpB66;});
goto _LL0;}case 267U: _LL211: _LL212: {
# 2370
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2372
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2368 "parse.y"
({union Cyc_YYSTYPE _tmpB68=Cyc_YY44((void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->tag=25U,({void*_tmpB67=Cyc_yyget_YY44(yyyyvsp[2]);_tmp5BB->f1=_tmpB67;});_tmp5BB;}));yyval=_tmpB68;});
goto _LL0;}case 268U: _LL213: _LL214: {
# 2371
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2373
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2369 "parse.y"
({union Cyc_YYSTYPE _tmpB6C=Cyc_YY44((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->tag=24U,({struct Cyc_List_List*_tmpB6B=({struct Cyc_List_List*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));({void*_tmpB6A=Cyc_yyget_YY44(yyyyvsp[0]);_tmp5BC->hd=_tmpB6A;}),({struct Cyc_List_List*_tmpB69=Cyc_yyget_YY40(yyyyvsp[2]);_tmp5BC->tl=_tmpB69;});_tmp5BC;});_tmp5BD->f1=_tmpB6B;});_tmp5BD;}));yyval=_tmpB6C;});
goto _LL0;}case 269U: _LL215: _LL216: {
# 2372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB6E=Cyc_YY40(({struct Cyc_List_List*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));({void*_tmpB6D=Cyc_yyget_YY44(yyyyvsp[0]);_tmp5BE->hd=_tmpB6D;}),_tmp5BE->tl=0;_tmp5BE;}));yyval=_tmpB6E;});
goto _LL0;}case 270U: _LL217: _LL218: {
# 2378
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2380
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2376 "parse.y"
({union Cyc_YYSTYPE _tmpB71=Cyc_YY40(({struct Cyc_List_List*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));({void*_tmpB70=Cyc_yyget_YY44(yyyyvsp[2]);_tmp5BF->hd=_tmpB70;}),({struct Cyc_List_List*_tmpB6F=Cyc_yyget_YY40(yyyyvsp[0]);_tmp5BF->tl=_tmpB6F;});_tmp5BF;}));yyval=_tmpB71;});
goto _LL0;}case 271U: _LL219: _LL21A: {
# 2379
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2381
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2381 "parse.y"
({union Cyc_YYSTYPE _tmpB73=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp74C;({struct Cyc_List_List*_tmpB72=Cyc_yyget_YY26(yyyyvsp[0]);_tmp74C.tms=_tmpB72;});_tmp74C;}));yyval=_tmpB73;});
goto _LL0;}case 272U: _LL21B: _LL21C: {
# 2384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2383 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 273U: _LL21D: _LL21E: {
# 2386
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2388
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2385 "parse.y"
({union Cyc_YYSTYPE _tmpB76=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp74D;({struct Cyc_List_List*_tmpB75=({struct Cyc_List_List*_tmpB74=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB74,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp74D.tms=_tmpB75;});_tmp74D;}));yyval=_tmpB76;});
goto _LL0;}case 274U: _LL21F: _LL220: {
# 2388
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2390
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2390 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 275U: _LL221: _LL222: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2392 "parse.y"
({union Cyc_YYSTYPE _tmpB7A=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp74E;({struct Cyc_List_List*_tmpB79=({struct Cyc_List_List*_tmp5C1=_region_malloc(yyr,sizeof(*_tmp5C1));({void*_tmpB78=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5C0=_region_malloc(yyr,sizeof(*_tmp5C0));_tmp5C0->tag=0U,({union Cyc_Absyn_Constraint*_tmpB77=Cyc_yyget_YY51(yyyyvsp[2]);_tmp5C0->f1=_tmpB77;}),_tmp5C0->f2=(unsigned int)(yyyylsp[2]).first_line;_tmp5C0;});_tmp5C1->hd=_tmpB78;}),_tmp5C1->tl=0;_tmp5C1;});_tmp74E.tms=_tmpB79;});_tmp74E;}));yyval=_tmpB7A;});
goto _LL0;}case 276U: _LL223: _LL224: {
# 2395
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2394 "parse.y"
({union Cyc_YYSTYPE _tmpB7F=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp74F;({struct Cyc_List_List*_tmpB7E=({struct Cyc_List_List*_tmp5C3=_region_malloc(yyr,sizeof(*_tmp5C3));({void*_tmpB7D=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp5C2=_region_malloc(yyr,sizeof(*_tmp5C2));_tmp5C2->tag=0U,({union Cyc_Absyn_Constraint*_tmpB7C=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5C2->f1=_tmpB7C;}),_tmp5C2->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp5C2;});_tmp5C3->hd=_tmpB7D;}),({struct Cyc_List_List*_tmpB7B=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5C3->tl=_tmpB7B;});_tmp5C3;});_tmp74F.tms=_tmpB7E;});_tmp74F;}));yyval=_tmpB7F;});
goto _LL0;}case 277U: _LL225: _LL226: {
# 2397
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2399
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2396 "parse.y"
({union Cyc_YYSTYPE _tmpB84=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp750;({struct Cyc_List_List*_tmpB83=({struct Cyc_List_List*_tmp5C5=_region_malloc(yyr,sizeof(*_tmp5C5));({void*_tmpB82=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5C4=_region_malloc(yyr,sizeof(*_tmp5C4));_tmp5C4->tag=1U,({struct Cyc_Absyn_Exp*_tmpB81=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp5C4->f1=_tmpB81;}),({union Cyc_Absyn_Constraint*_tmpB80=Cyc_yyget_YY51(yyyyvsp[3]);_tmp5C4->f2=_tmpB80;}),_tmp5C4->f3=(unsigned int)(yyyylsp[3]).first_line;_tmp5C4;});_tmp5C5->hd=_tmpB82;}),_tmp5C5->tl=0;_tmp5C5;});_tmp750.tms=_tmpB83;});_tmp750;}));yyval=_tmpB84;});
goto _LL0;}case 278U: _LL227: _LL228: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2398 "parse.y"
({union Cyc_YYSTYPE _tmpB8A=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp751;({struct Cyc_List_List*_tmpB89=({struct Cyc_List_List*_tmp5C7=_region_malloc(yyr,sizeof(*_tmp5C7));({void*_tmpB88=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp5C6=_region_malloc(yyr,sizeof(*_tmp5C6));_tmp5C6->tag=1U,({struct Cyc_Absyn_Exp*_tmpB87=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp5C6->f1=_tmpB87;}),({union Cyc_Absyn_Constraint*_tmpB86=Cyc_yyget_YY51(yyyyvsp[4]);_tmp5C6->f2=_tmpB86;}),_tmp5C6->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp5C6;});_tmp5C7->hd=_tmpB88;}),({
struct Cyc_List_List*_tmpB85=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5C7->tl=_tmpB85;});_tmp5C7;});
# 2398
_tmp751.tms=_tmpB89;});_tmp751;}));yyval=_tmpB8A;});
# 2401
goto _LL0;}case 279U: _LL229: _LL22A: {
# 2403
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2405
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2402 "parse.y"
({union Cyc_YYSTYPE _tmpB92=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp752;({struct Cyc_List_List*_tmpB91=({struct Cyc_List_List*_tmp5CA=_region_malloc(yyr,sizeof(*_tmp5CA));({void*_tmpB90=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5C9=_region_malloc(yyr,sizeof(*_tmp5C9));_tmp5C9->tag=3U,({void*_tmpB8F=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5C8=_region_malloc(yyr,sizeof(*_tmp5C8));_tmp5C8->tag=1U,_tmp5C8->f1=0,_tmp5C8->f2=0,_tmp5C8->f3=0,({void*_tmpB8E=Cyc_yyget_YY49(yyyyvsp[1]);_tmp5C8->f4=_tmpB8E;}),({struct Cyc_List_List*_tmpB8D=Cyc_yyget_YY50(yyyyvsp[2]);_tmp5C8->f5=_tmpB8D;}),({struct Cyc_Absyn_Exp*_tmpB8C=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5C8->f6=_tmpB8C;}),({struct Cyc_Absyn_Exp*_tmpB8B=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5C8->f7=_tmpB8B;});_tmp5C8;});_tmp5C9->f1=_tmpB8F;});_tmp5C9;});_tmp5CA->hd=_tmpB90;}),_tmp5CA->tl=0;_tmp5CA;});_tmp752.tms=_tmpB91;});_tmp752;}));yyval=_tmpB92;});
# 2404
goto _LL0;}case 280U: _LL22B: _LL22C: {
# 2406
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2408
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2405 "parse.y"
struct _tuple26*_tmp5CB=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp5CC=_tmp5CB;struct Cyc_List_List*_tmp5D4;int _tmp5D3;struct Cyc_Absyn_VarargInfo*_tmp5D2;void*_tmp5D1;struct Cyc_List_List*_tmp5D0;_LL467: _tmp5D4=_tmp5CC->f1;_tmp5D3=_tmp5CC->f2;_tmp5D2=_tmp5CC->f3;_tmp5D1=_tmp5CC->f4;_tmp5D0=_tmp5CC->f5;_LL468:;
({union Cyc_YYSTYPE _tmpB98=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp753;({struct Cyc_List_List*_tmpB97=({struct Cyc_List_List*_tmp5CF=_region_malloc(yyr,sizeof(*_tmp5CF));({void*_tmpB96=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5CE=_region_malloc(yyr,sizeof(*_tmp5CE));_tmp5CE->tag=3U,({void*_tmpB95=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5CD=_region_malloc(yyr,sizeof(*_tmp5CD));_tmp5CD->tag=1U,_tmp5CD->f1=_tmp5D4,_tmp5CD->f2=_tmp5D3,_tmp5CD->f3=_tmp5D2,_tmp5CD->f4=_tmp5D1,_tmp5CD->f5=_tmp5D0,({struct Cyc_Absyn_Exp*_tmpB94=Cyc_yyget_YY55(yyyyvsp[3]);_tmp5CD->f6=_tmpB94;}),({struct Cyc_Absyn_Exp*_tmpB93=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5CD->f7=_tmpB93;});_tmp5CD;});_tmp5CE->f1=_tmpB95;});_tmp5CE;});_tmp5CF->hd=_tmpB96;}),_tmp5CF->tl=0;_tmp5CF;});_tmp753.tms=_tmpB97;});_tmp753;}));yyval=_tmpB98;});
# 2408
goto _LL0;}case 281U: _LL22D: _LL22E: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2409 "parse.y"
({union Cyc_YYSTYPE _tmpBA1=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp754;({struct Cyc_List_List*_tmpBA0=({struct Cyc_List_List*_tmp5D7=_region_malloc(yyr,sizeof(*_tmp5D7));({void*_tmpB9F=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5D6=_region_malloc(yyr,sizeof(*_tmp5D6));_tmp5D6->tag=3U,({void*_tmpB9E=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5D5=_region_malloc(yyr,sizeof(*_tmp5D5));_tmp5D5->tag=1U,_tmp5D5->f1=0,_tmp5D5->f2=0,_tmp5D5->f3=0,({void*_tmpB9D=Cyc_yyget_YY49(yyyyvsp[2]);_tmp5D5->f4=_tmpB9D;}),({struct Cyc_List_List*_tmpB9C=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5D5->f5=_tmpB9C;}),({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5D5->f6=_tmpB9B;}),({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5D5->f7=_tmpB9A;});_tmp5D5;});_tmp5D6->f1=_tmpB9E;});_tmp5D6;});_tmp5D7->hd=_tmpB9F;}),({
struct Cyc_List_List*_tmpB99=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5D7->tl=_tmpB99;});_tmp5D7;});
# 2409
_tmp754.tms=_tmpBA0;});_tmp754;}));yyval=_tmpBA1;});
# 2412
goto _LL0;}case 282U: _LL22F: _LL230: {
# 2414
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2416
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2413 "parse.y"
struct _tuple26*_tmp5D8=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5D9=_tmp5D8;struct Cyc_List_List*_tmp5E1;int _tmp5E0;struct Cyc_Absyn_VarargInfo*_tmp5DF;void*_tmp5DE;struct Cyc_List_List*_tmp5DD;_LL46A: _tmp5E1=_tmp5D9->f1;_tmp5E0=_tmp5D9->f2;_tmp5DF=_tmp5D9->f3;_tmp5DE=_tmp5D9->f4;_tmp5DD=_tmp5D9->f5;_LL46B:;
({union Cyc_YYSTYPE _tmpBA8=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp755;({struct Cyc_List_List*_tmpBA7=({struct Cyc_List_List*_tmp5DC=_region_malloc(yyr,sizeof(*_tmp5DC));({void*_tmpBA6=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5DB=_region_malloc(yyr,sizeof(*_tmp5DB));_tmp5DB->tag=3U,({void*_tmpBA5=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5DA=_region_malloc(yyr,sizeof(*_tmp5DA));_tmp5DA->tag=1U,_tmp5DA->f1=_tmp5E1,_tmp5DA->f2=_tmp5E0,_tmp5DA->f3=_tmp5DF,_tmp5DA->f4=_tmp5DE,_tmp5DA->f5=_tmp5DD,({
struct Cyc_Absyn_Exp*_tmpBA4=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5DA->f6=_tmpBA4;}),({struct Cyc_Absyn_Exp*_tmpBA3=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5DA->f7=_tmpBA3;});_tmp5DA;});
# 2414
_tmp5DB->f1=_tmpBA5;});_tmp5DB;});_tmp5DC->hd=_tmpBA6;}),({
struct Cyc_List_List*_tmpBA2=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5DC->tl=_tmpBA2;});_tmp5DC;});
# 2414
_tmp755.tms=_tmpBA7;});_tmp755;}));yyval=_tmpBA8;});
# 2417
goto _LL0;}case 283U: _LL231: _LL232: {
# 2419
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2421
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2419 "parse.y"
struct Cyc_List_List*_tmp5E2=({unsigned int _tmpBA9=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpBA9,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpBAD=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp756;({struct Cyc_List_List*_tmpBAC=({struct Cyc_List_List*_tmp5E4=_region_malloc(yyr,sizeof(*_tmp5E4));({void*_tmpBAB=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5E3=_region_malloc(yyr,sizeof(*_tmp5E3));_tmp5E3->tag=4U,_tmp5E3->f1=_tmp5E2,_tmp5E3->f2=(unsigned int)(yyyylsp[1]).first_line,_tmp5E3->f3=0;_tmp5E3;});_tmp5E4->hd=_tmpBAB;}),({
struct Cyc_List_List*_tmpBAA=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5E4->tl=_tmpBAA;});_tmp5E4;});
# 2420
_tmp756.tms=_tmpBAC;});_tmp756;}));yyval=_tmpBAD;});
# 2423
goto _LL0;}case 284U: _LL233: _LL234: {
# 2425
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2427
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2424 "parse.y"
({union Cyc_YYSTYPE _tmpBB2=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp757;({struct Cyc_List_List*_tmpBB1=({struct Cyc_List_List*_tmp5E6=_region_malloc(yyr,sizeof(*_tmp5E6));({void*_tmpBB0=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5E5=_region_malloc(yyr,sizeof(*_tmp5E5));_tmp5E5->tag=5U,_tmp5E5->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpBAF=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5E5->f2=_tmpBAF;});_tmp5E5;});_tmp5E6->hd=_tmpBB0;}),({struct Cyc_List_List*_tmpBAE=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5E6->tl=_tmpBAE;});_tmp5E6;});_tmp757.tms=_tmpBB1;});_tmp757;}));yyval=_tmpBB2;});
# 2426
goto _LL0;}case 285U: _LL235: _LL236: {
# 2428
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2430
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2430 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 286U: _LL237: _LL238: {
# 2433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2431 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 287U: _LL239: _LL23A: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2432 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 288U: _LL23B: _LL23C: {
# 2435
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2437
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2433 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 289U: _LL23D: _LL23E: {
# 2436
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2438
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2434 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 290U: _LL23F: _LL240: {
# 2437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2435 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 291U: _LL241: _LL242: {
# 2438
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2440
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpBB7=Cyc_Stmt_tok(({void*_tmpBB6=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->tag=13U,({struct _dyneither_ptr*_tmpBB5=({struct _dyneither_ptr*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));({struct _dyneither_ptr _tmpBB4=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5E7=_tmpBB4;});_tmp5E7;});_tmp5E8->f1=_tmpBB5;}),({struct Cyc_Absyn_Stmt*_tmpBB3=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5E8->f2=_tmpBB3;});_tmp5E8;});Cyc_Absyn_new_stmt(_tmpBB6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBB7;});
goto _LL0;}case 292U: _LL243: _LL244: {
# 2444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2445 "parse.y"
({union Cyc_YYSTYPE _tmpBB8=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpBB8;});
goto _LL0;}case 293U: _LL245: _LL246: {
# 2448
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2450
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2446 "parse.y"
({union Cyc_YYSTYPE _tmpBBA=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBB9=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpBB9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBBA;});
goto _LL0;}case 294U: _LL247: _LL248: {
# 2449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2451 "parse.y"
({union Cyc_YYSTYPE _tmpBBB=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpBBB;});
goto _LL0;}case 295U: _LL249: _LL24A: {
# 2454
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2456
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2452 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 296U: _LL24B: _LL24C: {
# 2455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2457 "parse.y"
({union Cyc_YYSTYPE _tmpBBD=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpBBC=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpBBC,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));yyval=_tmpBBD;});
goto _LL0;}case 297U: _LL24D: _LL24E: {
# 2460
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2462
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2458 "parse.y"
({union Cyc_YYSTYPE _tmpBBF=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpBBE=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpBBE,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpBBF;});
goto _LL0;}case 298U: _LL24F: _LL250: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2459 "parse.y"
({union Cyc_YYSTYPE _tmpBC5=Cyc_Stmt_tok(({void*_tmpBC4=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5EA=_cycalloc(sizeof(*_tmp5EA));_tmp5EA->tag=13U,({struct _dyneither_ptr*_tmpBC3=({struct _dyneither_ptr*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));({struct _dyneither_ptr _tmpBC2=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5E9=_tmpBC2;});_tmp5E9;});_tmp5EA->f1=_tmpBC3;}),({struct Cyc_Absyn_Stmt*_tmpBC1=({struct Cyc_List_List*_tmpBC0=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpBC0,Cyc_Absyn_skip_stmt(0U));});_tmp5EA->f2=_tmpBC1;});_tmp5EA;});Cyc_Absyn_new_stmt(_tmpBC4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBC5;});
# 2461
goto _LL0;}case 299U: _LL251: _LL252: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2461 "parse.y"
({union Cyc_YYSTYPE _tmpBCB=Cyc_Stmt_tok(({void*_tmpBCA=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->tag=13U,({struct _dyneither_ptr*_tmpBC9=({struct _dyneither_ptr*_tmp5EB=_cycalloc(sizeof(*_tmp5EB));({struct _dyneither_ptr _tmpBC8=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5EB=_tmpBC8;});_tmp5EB;});_tmp5EC->f1=_tmpBC9;}),({struct Cyc_Absyn_Stmt*_tmpBC7=({struct Cyc_List_List*_tmpBC6=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpBC6,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp5EC->f2=_tmpBC7;});_tmp5EC;});Cyc_Absyn_new_stmt(_tmpBCA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBCB;});
# 2463
goto _LL0;}case 300U: _LL253: _LL254: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2463 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 301U: _LL255: _LL256: {
# 2466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2464 "parse.y"
({union Cyc_YYSTYPE _tmpBCE=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpBCD=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpBCC=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpBCD,_tmpBCC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBCE;});
goto _LL0;}case 302U: _LL257: _LL258: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2465 "parse.y"
({union Cyc_YYSTYPE _tmpBD2=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpBD1=({void*_tmpBD0=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpBCF=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5ED->f1=_tmpBCF;});_tmp5ED;});Cyc_Absyn_new_decl(_tmpBD0,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpBD1,
Cyc_Absyn_skip_stmt(0U));}));
# 2465
yyval=_tmpBD2;});
# 2467
goto _LL0;}case 303U: _LL259: _LL25A: {
# 2469
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2471
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2468 "parse.y"
({union Cyc_YYSTYPE _tmpBD6=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpBD5=({void*_tmpBD4=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpBD3=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5EE->f1=_tmpBD3;});_tmp5EE;});Cyc_Absyn_new_decl(_tmpBD4,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpBD5,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpBD6;});
goto _LL0;}case 304U: _LL25B: _LL25C: {
# 2471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2473 "parse.y"
({union Cyc_YYSTYPE _tmpBDA=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBD9=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpBD8=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpBD7=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpBD9,_tmpBD8,_tmpBD7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBDA;});
goto _LL0;}case 305U: _LL25D: _LL25E: {
# 2476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2475 "parse.y"
({union Cyc_YYSTYPE _tmpBDE=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBDD=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpBDC=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpBDB=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpBDD,_tmpBDC,_tmpBDB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBDE;});
goto _LL0;}case 306U: _LL25F: _LL260: {
# 2478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
({union Cyc_YYSTYPE _tmpBE1=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBE0=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpBDF=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpBE0,_tmpBDF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBE1;});
goto _LL0;}case 307U: _LL261: _LL262: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2484 "parse.y"
struct Cyc_Absyn_Exp*_tmp5EF=({struct _tuple0*_tmpBE2=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpBE2,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpBE5=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBE4=_tmp5EF;struct Cyc_List_List*_tmpBE3=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpBE4,_tmpBE3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBE5;});
goto _LL0;}case 308U: _LL263: _LL264: {
# 2488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2488 "parse.y"
struct Cyc_Absyn_Exp*_tmp5F0=({struct Cyc_List_List*_tmpBE6=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpBE6,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpBE9=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBE8=_tmp5F0;struct Cyc_List_List*_tmpBE7=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpBE8,_tmpBE7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBE9;});
# 2491
goto _LL0;}case 309U: _LL265: _LL266: {
# 2493
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2495
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2494 "parse.y"
({union Cyc_YYSTYPE _tmpBEC=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpBEB=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpBEA=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpBEB,_tmpBEA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBEC;});
goto _LL0;}case 310U: _LL267: _LL268:
# 2497
({union Cyc_YYSTYPE _tmpBED=Cyc_YY8(0);yyval=_tmpBED;});
goto _LL0;case 311U: _LL269: _LL26A: {
# 2500
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2502
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2512 "parse.y"
({union Cyc_YYSTYPE _tmpBF2=Cyc_YY8(({struct Cyc_List_List*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));({struct Cyc_Absyn_Switch_clause*_tmpBF1=({struct Cyc_Absyn_Switch_clause*_tmp5F1=_cycalloc(sizeof(*_tmp5F1));({struct Cyc_Absyn_Pat*_tmpBF0=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp5F1->pattern=_tmpBF0;}),_tmp5F1->pat_vars=0,_tmp5F1->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpBEF=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5F1->body=_tmpBEF;}),_tmp5F1->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F1;});
# 2512
_tmp5F2->hd=_tmpBF1;}),({
# 2514
struct Cyc_List_List*_tmpBEE=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5F2->tl=_tmpBEE;});_tmp5F2;}));
# 2512
yyval=_tmpBF2;});
# 2515
goto _LL0;}case 312U: _LL26B: _LL26C: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2516 "parse.y"
({union Cyc_YYSTYPE _tmpBF7=Cyc_YY8(({struct Cyc_List_List*_tmp5F4=_cycalloc(sizeof(*_tmp5F4));({struct Cyc_Absyn_Switch_clause*_tmpBF6=({struct Cyc_Absyn_Switch_clause*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));({struct Cyc_Absyn_Pat*_tmpBF5=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5F3->pattern=_tmpBF5;}),_tmp5F3->pat_vars=0,_tmp5F3->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpBF4=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp5F3->body=_tmpBF4;}),_tmp5F3->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F3;});
# 2516
_tmp5F4->hd=_tmpBF6;}),({
# 2518
struct Cyc_List_List*_tmpBF3=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5F4->tl=_tmpBF3;});_tmp5F4;}));
# 2516
yyval=_tmpBF7;});
# 2519
goto _LL0;}case 313U: _LL26D: _LL26E: {
# 2521
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2523
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2520 "parse.y"
({union Cyc_YYSTYPE _tmpBFC=Cyc_YY8(({struct Cyc_List_List*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));({struct Cyc_Absyn_Switch_clause*_tmpBFB=({struct Cyc_Absyn_Switch_clause*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));({struct Cyc_Absyn_Pat*_tmpBFA=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5F5->pattern=_tmpBFA;}),_tmp5F5->pat_vars=0,_tmp5F5->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpBF9=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp5F5->body=_tmpBF9;}),_tmp5F5->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F5;});_tmp5F6->hd=_tmpBFB;}),({struct Cyc_List_List*_tmpBF8=Cyc_yyget_YY8(yyyyvsp[4]);_tmp5F6->tl=_tmpBF8;});_tmp5F6;}));yyval=_tmpBFC;});
goto _LL0;}case 314U: _LL26F: _LL270: {
# 2523
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2522 "parse.y"
({union Cyc_YYSTYPE _tmpC02=Cyc_YY8(({struct Cyc_List_List*_tmp5F8=_cycalloc(sizeof(*_tmp5F8));({struct Cyc_Absyn_Switch_clause*_tmpC01=({struct Cyc_Absyn_Switch_clause*_tmp5F7=_cycalloc(sizeof(*_tmp5F7));({struct Cyc_Absyn_Pat*_tmpC00=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5F7->pattern=_tmpC00;}),_tmp5F7->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpBFF=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5F7->where_clause=_tmpBFF;}),({
struct Cyc_Absyn_Stmt*_tmpBFE=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp5F7->body=_tmpBFE;}),_tmp5F7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F7;});
# 2522
_tmp5F8->hd=_tmpC01;}),({
# 2524
struct Cyc_List_List*_tmpBFD=Cyc_yyget_YY8(yyyyvsp[5]);_tmp5F8->tl=_tmpBFD;});_tmp5F8;}));
# 2522
yyval=_tmpC02;});
# 2525
goto _LL0;}case 315U: _LL271: _LL272: {
# 2527
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2529
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2526 "parse.y"
({union Cyc_YYSTYPE _tmpC08=Cyc_YY8(({struct Cyc_List_List*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));({struct Cyc_Absyn_Switch_clause*_tmpC07=({struct Cyc_Absyn_Switch_clause*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));({struct Cyc_Absyn_Pat*_tmpC06=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5F9->pattern=_tmpC06;}),_tmp5F9->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpC05=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5F9->where_clause=_tmpC05;}),({struct Cyc_Absyn_Stmt*_tmpC04=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp5F9->body=_tmpC04;}),_tmp5F9->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5F9;});_tmp5FA->hd=_tmpC07;}),({struct Cyc_List_List*_tmpC03=Cyc_yyget_YY8(yyyyvsp[6]);_tmp5FA->tl=_tmpC03;});_tmp5FA;}));yyval=_tmpC08;});
goto _LL0;}case 316U: _LL273: _LL274: {
# 2529
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2531
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2533 "parse.y"
({union Cyc_YYSTYPE _tmpC0B=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC0A=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpC09=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpC0A,_tmpC09,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC0B;});
goto _LL0;}case 317U: _LL275: _LL276: {
# 2536
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2537 "parse.y"
({union Cyc_YYSTYPE _tmpC0E=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpC0D=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpC0C=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpC0D,_tmpC0C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC0E;});
goto _LL0;}case 318U: _LL277: _LL278: {
# 2540
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2542
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2541 "parse.y"
({union Cyc_YYSTYPE _tmpC13=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC12=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC11=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC10=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC0F=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2541
Cyc_Absyn_for_stmt(_tmpC12,_tmpC11,_tmpC10,_tmpC0F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC13;});
# 2543
goto _LL0;}case 319U: _LL279: _LL27A: {
# 2545
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2547
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2544 "parse.y"
({union Cyc_YYSTYPE _tmpC18=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC17=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC16=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC15=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpC14=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2544
Cyc_Absyn_for_stmt(_tmpC17,_tmpC16,_tmpC15,_tmpC14,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC18;});
# 2546
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2548
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2550
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2547 "parse.y"
({union Cyc_YYSTYPE _tmpC1D=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC1C=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC1B=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC1A=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC19=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2547
Cyc_Absyn_for_stmt(_tmpC1C,_tmpC1B,_tmpC1A,_tmpC19,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC1D;});
# 2549
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2551
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2550 "parse.y"
({union Cyc_YYSTYPE _tmpC22=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC21=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC20=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC1F=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpC1E=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2550
Cyc_Absyn_for_stmt(_tmpC21,_tmpC20,_tmpC1F,_tmpC1E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC22;});
# 2552
goto _LL0;}case 322U: _LL27F: _LL280: {
# 2554
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2556
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2553 "parse.y"
({union Cyc_YYSTYPE _tmpC27=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC26=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC25=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC24=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC23=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2553
Cyc_Absyn_for_stmt(_tmpC26,_tmpC25,_tmpC24,_tmpC23,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC27;});
# 2555
goto _LL0;}case 323U: _LL281: _LL282: {
# 2557
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2559
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2556 "parse.y"
({union Cyc_YYSTYPE _tmpC2C=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC2B=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC2A=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC29=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpC28=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2556
Cyc_Absyn_for_stmt(_tmpC2B,_tmpC2A,_tmpC29,_tmpC28,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC2C;});
# 2558
goto _LL0;}case 324U: _LL283: _LL284: {
# 2560
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2562
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2559 "parse.y"
({union Cyc_YYSTYPE _tmpC31=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC30=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC2F=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpC2E=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC2D=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2559
Cyc_Absyn_for_stmt(_tmpC30,_tmpC2F,_tmpC2E,_tmpC2D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC31;});
# 2561
goto _LL0;}case 325U: _LL285: _LL286: {
# 2563
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 2565
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 2562 "parse.y"
({union Cyc_YYSTYPE _tmpC36=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC35=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC34=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpC33=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpC32=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2562
Cyc_Absyn_for_stmt(_tmpC35,_tmpC34,_tmpC33,_tmpC32,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC36;});
# 2564
goto _LL0;}case 326U: _LL287: _LL288: {
# 2566
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2568
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2565 "parse.y"
struct Cyc_List_List*_tmp5FB=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5FC=({struct Cyc_Absyn_Exp*_tmpC3A=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC39=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC38=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC37=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2566
Cyc_Absyn_for_stmt(_tmpC3A,_tmpC39,_tmpC38,_tmpC37,(unsigned int)(yyyylsp[0]).first_line);});
# 2568
({union Cyc_YYSTYPE _tmpC3B=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5FB,_tmp5FC));yyval=_tmpC3B;});
# 2570
goto _LL0;}case 327U: _LL289: _LL28A: {
# 2572
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2574
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2571 "parse.y"
struct Cyc_List_List*_tmp5FD=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5FE=({struct Cyc_Absyn_Exp*_tmpC3F=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC3E=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC3D=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpC3C=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2572
Cyc_Absyn_for_stmt(_tmpC3F,_tmpC3E,_tmpC3D,_tmpC3C,(unsigned int)(yyyylsp[0]).first_line);});
# 2574
({union Cyc_YYSTYPE _tmpC40=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5FD,_tmp5FE));yyval=_tmpC40;});
# 2576
goto _LL0;}case 328U: _LL28B: _LL28C: {
# 2578
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2580
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2577 "parse.y"
struct Cyc_List_List*_tmp5FF=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp600=({struct Cyc_Absyn_Exp*_tmpC44=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC43=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpC42=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpC41=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2578
Cyc_Absyn_for_stmt(_tmpC44,_tmpC43,_tmpC42,_tmpC41,(unsigned int)(yyyylsp[0]).first_line);});
# 2580
({union Cyc_YYSTYPE _tmpC45=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5FF,_tmp600));yyval=_tmpC45;});
# 2582
goto _LL0;}case 329U: _LL28D: _LL28E: {
# 2584
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2586
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2583 "parse.y"
struct Cyc_List_List*_tmp601=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp602=({struct Cyc_Absyn_Exp*_tmpC49=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC48=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC47=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpC46=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2584
Cyc_Absyn_for_stmt(_tmpC49,_tmpC48,_tmpC47,_tmpC46,(unsigned int)(yyyylsp[0]).first_line);});
# 2586
({union Cyc_YYSTYPE _tmpC4A=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp601,_tmp602));yyval=_tmpC4A;});
# 2588
goto _LL0;}case 330U: _LL28F: _LL290: {
# 2590
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2592
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpC4D=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpC4C=({struct _dyneither_ptr*_tmp603=_cycalloc(sizeof(*_tmp603));({struct _dyneither_ptr _tmpC4B=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp603=_tmpC4B;});_tmp603;});Cyc_Absyn_goto_stmt(_tmpC4C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC4D;});
goto _LL0;}case 331U: _LL291: _LL292: {
# 2596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2594 "parse.y"
({union Cyc_YYSTYPE _tmpC4E=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC4E;});
goto _LL0;}case 332U: _LL293: _LL294: {
# 2597
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2599
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2595 "parse.y"
({union Cyc_YYSTYPE _tmpC4F=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC4F;});
goto _LL0;}case 333U: _LL295: _LL296: {
# 2598
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2596 "parse.y"
({union Cyc_YYSTYPE _tmpC50=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC50;});
goto _LL0;}case 334U: _LL297: _LL298: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2597 "parse.y"
({union Cyc_YYSTYPE _tmpC52=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC51=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpC51,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC52;});
goto _LL0;}case 335U: _LL299: _LL29A: {
# 2600
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2602
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2599 "parse.y"
({union Cyc_YYSTYPE _tmpC53=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC53;});
goto _LL0;}case 336U: _LL29B: _LL29C: {
# 2602
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2604
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2600 "parse.y"
({union Cyc_YYSTYPE _tmpC54=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC54;});
goto _LL0;}case 337U: _LL29D: _LL29E: {
# 2603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2602 "parse.y"
({union Cyc_YYSTYPE _tmpC56=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpC55=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpC55,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC56;});
goto _LL0;}case 338U: _LL29F: _LL2A0: {
# 2605
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2607
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2611 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2614
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2616
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2614 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2617
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2619
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2616 "parse.y"
({union Cyc_YYSTYPE _tmpC5A=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC59=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC58=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC57=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpC59,_tmpC58,_tmpC57,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC5A;});
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2619 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2621 "parse.y"
({union Cyc_YYSTYPE _tmpC5D=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC5C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC5B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpC5C,_tmpC5B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC5D;});
goto _LL0;}case 343U: _LL2A9: _LL2AA: {
# 2624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2624 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 344U: _LL2AB: _LL2AC: {
# 2627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2626 "parse.y"
({union Cyc_YYSTYPE _tmpC60=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC5F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC5E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpC5F,_tmpC5E,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC60;});
goto _LL0;}case 345U: _LL2AD: _LL2AE: {
# 2629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2629 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 346U: _LL2AF: _LL2B0: {
# 2632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2631 "parse.y"
({union Cyc_YYSTYPE _tmpC63=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC62=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC61=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpC62,_tmpC61,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC63;});
goto _LL0;}case 347U: _LL2B1: _LL2B2: {
# 2634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2634 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 348U: _LL2B3: _LL2B4: {
# 2637
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2639
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2636 "parse.y"
({union Cyc_YYSTYPE _tmpC66=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC65=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC64=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpC65,_tmpC64,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC66;});
goto _LL0;}case 349U: _LL2B5: _LL2B6: {
# 2639
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2641
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2639 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 350U: _LL2B7: _LL2B8: {
# 2642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2641 "parse.y"
({union Cyc_YYSTYPE _tmpC69=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC68=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC67=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpC68,_tmpC67,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC69;});
goto _LL0;}case 351U: _LL2B9: _LL2BA: {
# 2644
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2646
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2644 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 352U: _LL2BB: _LL2BC: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2646 "parse.y"
({union Cyc_YYSTYPE _tmpC6C=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC6B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC6A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpC6B,_tmpC6A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC6C;});
goto _LL0;}case 353U: _LL2BD: _LL2BE: {
# 2649
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2651
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2648 "parse.y"
({union Cyc_YYSTYPE _tmpC6F=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC6E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC6D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpC6E,_tmpC6D,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC6F;});
goto _LL0;}case 354U: _LL2BF: _LL2C0: {
# 2651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2651 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 355U: _LL2C1: _LL2C2: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2653 "parse.y"
({union Cyc_YYSTYPE _tmpC72=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC71=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC70=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpC71,_tmpC70,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC72;});
goto _LL0;}case 356U: _LL2C3: _LL2C4: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2655 "parse.y"
({union Cyc_YYSTYPE _tmpC75=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC74=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC73=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpC74,_tmpC73,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC75;});
goto _LL0;}case 357U: _LL2C5: _LL2C6: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2657 "parse.y"
({union Cyc_YYSTYPE _tmpC78=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC77=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC76=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpC77,_tmpC76,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC78;});
goto _LL0;}case 358U: _LL2C7: _LL2C8: {
# 2660
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2662
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2659 "parse.y"
({union Cyc_YYSTYPE _tmpC7B=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC7A=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC79=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpC7A,_tmpC79,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC7B;});
goto _LL0;}case 359U: _LL2C9: _LL2CA: {
# 2662
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2664
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2662 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 360U: _LL2CB: _LL2CC: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2664 "parse.y"
({union Cyc_YYSTYPE _tmpC7E=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC7D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC7C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpC7D,_tmpC7C,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC7E;});
goto _LL0;}case 361U: _LL2CD: _LL2CE: {
# 2667
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2669
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2666 "parse.y"
({union Cyc_YYSTYPE _tmpC81=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC80=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC7F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpC80,_tmpC7F,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC81;});
goto _LL0;}case 362U: _LL2CF: _LL2D0: {
# 2669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2669 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 363U: _LL2D1: _LL2D2: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2671 "parse.y"
({union Cyc_YYSTYPE _tmpC84=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC83=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC82=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpC83,_tmpC82,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC84;});
goto _LL0;}case 364U: _LL2D3: _LL2D4: {
# 2674
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2676
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2673 "parse.y"
({union Cyc_YYSTYPE _tmpC87=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC86=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC85=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpC86,_tmpC85,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC87;});
goto _LL0;}case 365U: _LL2D5: _LL2D6: {
# 2676
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2678
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2676 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 366U: _LL2D7: _LL2D8: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2678 "parse.y"
({union Cyc_YYSTYPE _tmpC8A=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC89=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC88=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpC89,_tmpC88,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC8A;});
goto _LL0;}case 367U: _LL2D9: _LL2DA: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2680 "parse.y"
({union Cyc_YYSTYPE _tmpC8D=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC8C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC8B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpC8C,_tmpC8B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC8D;});
goto _LL0;}case 368U: _LL2DB: _LL2DC: {
# 2683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2682 "parse.y"
({union Cyc_YYSTYPE _tmpC90=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC8F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC8E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpC8F,_tmpC8E,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC90;});
goto _LL0;}case 369U: _LL2DD: _LL2DE: {
# 2685
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2687
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2685 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 370U: _LL2DF: _LL2E0: {
# 2688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2687 "parse.y"
void*_tmp604=({struct _tuple8*_tmpC91=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpC91,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpC94=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpC93=_tmp604;struct Cyc_Absyn_Exp*_tmpC92=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpC93,_tmpC92,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC94;});
# 2690
goto _LL0;}case 371U: _LL2E1: _LL2E2: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2693 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 372U: _LL2E3: _LL2E4: {
# 2696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2696 "parse.y"
({union Cyc_YYSTYPE _tmpC95=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpC95;});
goto _LL0;}case 373U: _LL2E5: _LL2E6: {
# 2699
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2701
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2698 "parse.y"
({union Cyc_YYSTYPE _tmpC98=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpC97=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpC96=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpC97,_tmpC96,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC98;});
goto _LL0;}case 374U: _LL2E7: _LL2E8: {
# 2701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2700 "parse.y"
void*_tmp605=({struct _tuple8*_tmpC99=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpC99,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpC9A=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftyp_exp(_tmp605,(unsigned int)(yyyylsp[0]).first_line)));yyval=_tmpC9A;});
# 2703
goto _LL0;}case 375U: _LL2E9: _LL2EA: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2704 "parse.y"
({union Cyc_YYSTYPE _tmpC9C=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC9B=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpC9B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC9C;});
goto _LL0;}case 376U: _LL2EB: _LL2EC: {
# 2707
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2709
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2706 "parse.y"
({union Cyc_YYSTYPE _tmpC9F=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpC9E=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpC9D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpC9E,_tmpC9D,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC9F;});
goto _LL0;}case 377U: _LL2ED: _LL2EE: {
# 2709
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2711
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2711 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 378U: _LL2EF: _LL2F0: {
# 2714
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2716
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2719 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 379U: _LL2F1: _LL2F2: {
# 2722
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2724
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2724 "parse.y"
({union Cyc_YYSTYPE _tmpCA0=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpCA0;});
goto _LL0;}case 380U: _LL2F3: _LL2F4: {
# 2727
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2729
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2726 "parse.y"
({union Cyc_YYSTYPE _tmpCA1=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpCA1;});
goto _LL0;}case 381U: _LL2F5: _LL2F6: {
# 2729
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2731
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2728 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp606=e->r;void*_tmp607=_tmp606;struct _dyneither_ptr _tmp61B;int _tmp61A;enum Cyc_Absyn_Sign _tmp619;int _tmp618;enum Cyc_Absyn_Sign _tmp617;short _tmp616;enum Cyc_Absyn_Sign _tmp615;char _tmp614;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).LongLong_c).tag){case 2U: _LL46D: _tmp615=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Char_c).val).f1;_tmp614=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Char_c).val).f2;_LL46E:
# 2732
({union Cyc_YYSTYPE _tmpCA3=Cyc_YY9(({void*_tmpCA2=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->tag=11U,_tmp608->f1=_tmp614;_tmp608;});Cyc_Absyn_new_pat(_tmpCA2,e->loc);}));yyval=_tmpCA3;});goto _LL46C;case 4U: _LL46F: _tmp617=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Short_c).val).f1;_tmp616=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Short_c).val).f2;_LL470:
# 2734
({union Cyc_YYSTYPE _tmpCA5=Cyc_YY9(({void*_tmpCA4=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->tag=10U,_tmp609->f1=_tmp617,_tmp609->f2=(int)_tmp616;_tmp609;});Cyc_Absyn_new_pat(_tmpCA4,e->loc);}));yyval=_tmpCA5;});goto _LL46C;case 5U: _LL471: _tmp619=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Int_c).val).f1;_tmp618=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Int_c).val).f2;_LL472:
# 2736
({union Cyc_YYSTYPE _tmpCA7=Cyc_YY9(({void*_tmpCA6=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->tag=10U,_tmp60A->f1=_tmp619,_tmp60A->f2=_tmp618;_tmp60A;});Cyc_Absyn_new_pat(_tmpCA6,e->loc);}));yyval=_tmpCA7;});goto _LL46C;case 7U: _LL473: _tmp61B=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Float_c).val).f1;_tmp61A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp607)->f1).Float_c).val).f2;_LL474:
# 2738
({union Cyc_YYSTYPE _tmpCA9=Cyc_YY9(({void*_tmpCA8=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->tag=12U,_tmp60B->f1=_tmp61B,_tmp60B->f2=_tmp61A;_tmp60B;});Cyc_Absyn_new_pat(_tmpCA8,e->loc);}));yyval=_tmpCA9;});goto _LL46C;case 1U: _LL475: _LL476:
# 2740
({union Cyc_YYSTYPE _tmpCAA=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));yyval=_tmpCAA;});goto _LL46C;case 8U: _LL477: _LL478:
# 2742
({void*_tmp60C=0U;({unsigned int _tmpCAC=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCAB=({const char*_tmp60D="strings cannot occur within patterns";_tag_dyneither(_tmp60D,sizeof(char),37U);});Cyc_Warn_err(_tmpCAC,_tmpCAB,_tag_dyneither(_tmp60C,sizeof(void*),0U));});});goto _LL46C;case 9U: _LL479: _LL47A:
# 2744
({void*_tmp60E=0U;({unsigned int _tmpCAE=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCAD=({const char*_tmp60F="strings cannot occur within patterns";_tag_dyneither(_tmp60F,sizeof(char),37U);});Cyc_Warn_err(_tmpCAE,_tmpCAD,_tag_dyneither(_tmp60E,sizeof(void*),0U));});});goto _LL46C;case 6U: _LL47B: _LL47C:
# 2746
({void*_tmp610=0U;({unsigned int _tmpCB0=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCAF=({const char*_tmp611="long long's in patterns not yet implemented";_tag_dyneither(_tmp611,sizeof(char),44U);});Cyc_Warn_err(_tmpCB0,_tmpCAF,_tag_dyneither(_tmp610,sizeof(void*),0U));});});goto _LL46C;default: goto _LL47D;}else{_LL47D: _LL47E:
# 2748
({void*_tmp612=0U;({unsigned int _tmpCB2=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpCB1=({const char*_tmp613="bad constant in case";_tag_dyneither(_tmp613,sizeof(char),21U);});Cyc_Warn_err(_tmpCB2,_tmpCB1,_tag_dyneither(_tmp612,sizeof(void*),0U));});});}_LL46C:;}
# 2751
goto _LL0;}case 382U: _LL2F7: _LL2F8: {
# 2753
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2755
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2752 "parse.y"
({union Cyc_YYSTYPE _tmpCB5=Cyc_YY9(({void*_tmpCB4=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));_tmp61C->tag=15U,({struct _tuple0*_tmpCB3=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp61C->f1=_tmpCB3;});_tmp61C;});Cyc_Absyn_new_pat(_tmpCB4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCB5;});
goto _LL0;}case 383U: _LL2F9: _LL2FA: {
# 2755
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2757
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2754 "parse.y"
if(({struct _dyneither_ptr _tmpCB6=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpCB6,({const char*_tmp61D="as";_tag_dyneither(_tmp61D,sizeof(char),3U);}));})!= 0)
({void*_tmp61E=0U;({unsigned int _tmpCB8=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCB7=({const char*_tmp61F="expecting `as'";_tag_dyneither(_tmp61F,sizeof(char),15U);});Cyc_Warn_err(_tmpCB8,_tmpCB7,_tag_dyneither(_tmp61E,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpCBF=Cyc_YY9(({void*_tmpCBE=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp622=_cycalloc(sizeof(*_tmp622));_tmp622->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpCBD=({unsigned int _tmpCBC=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpCBC,({struct _tuple0*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCBB=({struct _dyneither_ptr*_tmp620=_cycalloc(sizeof(*_tmp620));({struct _dyneither_ptr _tmpCBA=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp620=_tmpCBA;});_tmp620;});_tmp621->f2=_tmpCBB;});_tmp621;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp622->f1=_tmpCBD;}),({
struct Cyc_Absyn_Pat*_tmpCB9=Cyc_yyget_YY9(yyyyvsp[2]);_tmp622->f2=_tmpCB9;});_tmp622;});
# 2756
Cyc_Absyn_new_pat(_tmpCBE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCBF;});
# 2759
goto _LL0;}case 384U: _LL2FB: _LL2FC: {
# 2761
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2763
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2760 "parse.y"
if(({struct _dyneither_ptr _tmpCC0=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpCC0,({const char*_tmp623="alias";_tag_dyneither(_tmp623,sizeof(char),6U);}));})!= 0)
({void*_tmp624=0U;({unsigned int _tmpCC2=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpCC1=({const char*_tmp625="expecting `alias'";_tag_dyneither(_tmp625,sizeof(char),18U);});Cyc_Warn_err(_tmpCC2,_tmpCC1,_tag_dyneither(_tmp624,sizeof(void*),0U));});});{
int _tmp626=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp62C=_cycalloc(sizeof(*_tmp62C));({struct _dyneither_ptr*_tmpCC5=({struct _dyneither_ptr*_tmp62A=_cycalloc(sizeof(*_tmp62A));({struct _dyneither_ptr _tmpCC4=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp62A=_tmpCC4;});_tmp62A;});_tmp62C->name=_tmpCC5;}),_tmp62C->identity=- 1,({void*_tmpCC3=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->tag=0U,_tmp62B->f1=& Cyc_Tcutil_rk;_tmp62B;});_tmp62C->kind=_tmpCC3;});_tmp62C;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpCCA=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCC9=({struct _tuple0*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCC7=({struct _dyneither_ptr*_tmp628=_cycalloc(sizeof(*_tmp628));({struct _dyneither_ptr _tmpCC6=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp628=_tmpCC6;});_tmp628;});_tmp629->f2=_tmpCC7;});_tmp629;});Cyc_Absyn_new_vardecl(_tmpCCA,_tmpCC9,({
struct _tuple8*_tmpCC8=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpCC8,(unsigned int)(yyyylsp[4]).first_line);}),0);});
({union Cyc_YYSTYPE _tmpCCC=Cyc_YY9(({void*_tmpCCB=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=2U,_tmp627->f1=tv,_tmp627->f2=vd;_tmp627;});Cyc_Absyn_new_pat(_tmpCCB,(unsigned int)_tmp626);}));yyval=_tmpCCC;});
# 2768
goto _LL0;};}case 385U: _LL2FD: _LL2FE: {
# 2770
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2772
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2769 "parse.y"
struct _tuple22 _tmp62D=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp62E=_tmp62D;struct Cyc_List_List*_tmp631;int _tmp630;_LL480: _tmp631=_tmp62E.f1;_tmp630=_tmp62E.f2;_LL481:;
({union Cyc_YYSTYPE _tmpCCE=Cyc_YY9(({void*_tmpCCD=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=5U,_tmp62F->f1=_tmp631,_tmp62F->f2=_tmp630;_tmp62F;});Cyc_Absyn_new_pat(_tmpCCD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCCE;});
# 2772
goto _LL0;}case 386U: _LL2FF: _LL300: {
# 2774
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2776
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2773 "parse.y"
struct _tuple22 _tmp632=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp633=_tmp632;struct Cyc_List_List*_tmp636;int _tmp635;_LL483: _tmp636=_tmp633.f1;_tmp635=_tmp633.f2;_LL484:;
({union Cyc_YYSTYPE _tmpCD1=Cyc_YY9(({void*_tmpCD0=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp634=_cycalloc(sizeof(*_tmp634));_tmp634->tag=16U,({struct _tuple0*_tmpCCF=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp634->f1=_tmpCCF;}),_tmp634->f2=_tmp636,_tmp634->f3=_tmp635;_tmp634;});Cyc_Absyn_new_pat(_tmpCD0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCD1;});
# 2776
goto _LL0;}case 387U: _LL301: _LL302: {
# 2778
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2780
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2777 "parse.y"
struct _tuple22 _tmp637=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp638=_tmp637;struct Cyc_List_List*_tmp63D;int _tmp63C;_LL486: _tmp63D=_tmp638.f1;_tmp63C=_tmp638.f2;_LL487:;{
struct Cyc_List_List*_tmp639=({unsigned int _tmpCD2=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpCD2,Cyc_yyget_YY40(yyyyvsp[2]));});
({union Cyc_YYSTYPE _tmpCD6=Cyc_YY9(({void*_tmpCD5=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B->tag=7U,({struct Cyc_Absyn_AggrInfo*_tmpCD4=({struct Cyc_Absyn_AggrInfo*_tmp63A=_cycalloc(sizeof(*_tmp63A));({union Cyc_Absyn_AggrInfoU _tmpCD3=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);_tmp63A->aggr_info=_tmpCD3;}),_tmp63A->targs=0;_tmp63A;});_tmp63B->f1=_tmpCD4;}),_tmp63B->f2=_tmp639,_tmp63B->f3=_tmp63D,_tmp63B->f4=_tmp63C;_tmp63B;});Cyc_Absyn_new_pat(_tmpCD5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCD6;});
# 2782
goto _LL0;};}case 388U: _LL303: _LL304: {
# 2784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2783 "parse.y"
struct _tuple22 _tmp63E=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp63F=_tmp63E;struct Cyc_List_List*_tmp643;int _tmp642;_LL489: _tmp643=_tmp63F.f1;_tmp642=_tmp63F.f2;_LL48A:;{
struct Cyc_List_List*_tmp640=({unsigned int _tmpCD7=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpCD7,Cyc_yyget_YY40(yyyyvsp[1]));});
({union Cyc_YYSTYPE _tmpCD9=Cyc_YY9(({void*_tmpCD8=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp641=_cycalloc(sizeof(*_tmp641));_tmp641->tag=7U,_tmp641->f1=0,_tmp641->f2=_tmp640,_tmp641->f3=_tmp643,_tmp641->f4=_tmp642;_tmp641;});Cyc_Absyn_new_pat(_tmpCD8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCD9;});
# 2787
goto _LL0;};}case 389U: _LL305: _LL306: {
# 2789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2788 "parse.y"
({union Cyc_YYSTYPE _tmpCDC=Cyc_YY9(({void*_tmpCDB=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->tag=6U,({struct Cyc_Absyn_Pat*_tmpCDA=Cyc_yyget_YY9(yyyyvsp[1]);_tmp644->f1=_tmpCDA;});_tmp644;});Cyc_Absyn_new_pat(_tmpCDB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCDC;});
goto _LL0;}case 390U: _LL307: _LL308: {
# 2791
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2793
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2790 "parse.y"
({union Cyc_YYSTYPE _tmpCE1=Cyc_YY9(({void*_tmpCE0=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp646=_cycalloc(sizeof(*_tmp646));_tmp646->tag=6U,({struct Cyc_Absyn_Pat*_tmpCDF=({void*_tmpCDE=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645->tag=6U,({struct Cyc_Absyn_Pat*_tmpCDD=Cyc_yyget_YY9(yyyyvsp[1]);_tmp645->f1=_tmpCDD;});_tmp645;});Cyc_Absyn_new_pat(_tmpCDE,(unsigned int)(yyyylsp[0]).first_line);});_tmp646->f1=_tmpCDF;});_tmp646;});Cyc_Absyn_new_pat(_tmpCE0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCE1;});
goto _LL0;}case 391U: _LL309: _LL30A: {
# 2793
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2795
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2792 "parse.y"
({union Cyc_YYSTYPE _tmpCE8=Cyc_YY9(({void*_tmpCE7=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpCE6=({unsigned int _tmpCE5=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpCE5,({struct _tuple0*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCE4=({struct _dyneither_ptr*_tmp647=_cycalloc(sizeof(*_tmp647));({struct _dyneither_ptr _tmpCE3=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp647=_tmpCE3;});_tmp647;});_tmp648->f2=_tmpCE4;});_tmp648;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp649->f1=_tmpCE6;}),({
# 2794
struct Cyc_Absyn_Pat*_tmpCE2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp649->f2=_tmpCE2;});_tmp649;});
# 2792
Cyc_Absyn_new_pat(_tmpCE7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCE8;});
# 2796
goto _LL0;}case 392U: _LL30B: _LL30C: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2797 "parse.y"
if(({struct _dyneither_ptr _tmpCE9=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpCE9,({const char*_tmp64A="as";_tag_dyneither(_tmp64A,sizeof(char),3U);}));})!= 0)
({void*_tmp64B=0U;({unsigned int _tmpCEB=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpCEA=({const char*_tmp64C="expecting `as'";_tag_dyneither(_tmp64C,sizeof(char),15U);});Cyc_Warn_err(_tmpCEB,_tmpCEA,_tag_dyneither(_tmp64B,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpCF2=Cyc_YY9(({void*_tmpCF1=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpCF0=({unsigned int _tmpCEF=(unsigned int)(yyyylsp[0]).first_line;Cyc_Absyn_new_vardecl(_tmpCEF,({struct _tuple0*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCEE=({struct _dyneither_ptr*_tmp64D=_cycalloc(sizeof(*_tmp64D));({struct _dyneither_ptr _tmpCED=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp64D=_tmpCED;});_tmp64D;});_tmp64E->f2=_tmpCEE;});_tmp64E;}),(void*)& Cyc_Absyn_VoidType_val,0);});_tmp64F->f1=_tmpCF0;}),({
# 2801
struct Cyc_Absyn_Pat*_tmpCEC=Cyc_yyget_YY9(yyyyvsp[3]);_tmp64F->f2=_tmpCEC;});_tmp64F;});
# 2799
Cyc_Absyn_new_pat(_tmpCF1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCF2;});
# 2803
goto _LL0;}case 393U: _LL30D: _LL30E: {
# 2805
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2807
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2804 "parse.y"
void*_tmp650=({struct _dyneither_ptr _tmpCF3=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpCF3,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
({union Cyc_YYSTYPE _tmpCFB=Cyc_YY9(({void*_tmpCFA=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->tag=4U,({struct Cyc_Absyn_Tvar*_tmpCF9=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp650);_tmp654->f1=_tmpCF9;}),({
struct Cyc_Absyn_Vardecl*_tmpCF8=({unsigned int _tmpCF7=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCF6=({struct _tuple0*_tmp652=_cycalloc(sizeof(*_tmp652));_tmp652->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCF5=({struct _dyneither_ptr*_tmp651=_cycalloc(sizeof(*_tmp651));({struct _dyneither_ptr _tmpCF4=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp651=_tmpCF4;});_tmp651;});_tmp652->f2=_tmpCF5;});_tmp652;});Cyc_Absyn_new_vardecl(_tmpCF7,_tmpCF6,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp653=_cycalloc(sizeof(*_tmp653));
_tmp653->tag=19U,_tmp653->f1=_tmp650;_tmp653;}),0);});
# 2806
_tmp654->f2=_tmpCF8;});_tmp654;});
# 2805
Cyc_Absyn_new_pat(_tmpCFA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCFB;});
# 2809
goto _LL0;}case 394U: _LL30F: _LL310: {
# 2811
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2813
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2810 "parse.y"
struct Cyc_Absyn_Tvar*_tmp655=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
({union Cyc_YYSTYPE _tmpD03=Cyc_YY9(({void*_tmpD02=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp65A=_cycalloc(sizeof(*_tmp65A));_tmp65A->tag=4U,_tmp65A->f1=_tmp655,({
struct Cyc_Absyn_Vardecl*_tmpD01=({unsigned int _tmpD00=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCFF=({struct _tuple0*_tmp657=_cycalloc(sizeof(*_tmp657));_tmp657->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCFD=({struct _dyneither_ptr*_tmp656=_cycalloc(sizeof(*_tmp656));({struct _dyneither_ptr _tmpCFC=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp656=_tmpCFC;});_tmp656;});_tmp657->f2=_tmpCFD;});_tmp657;});Cyc_Absyn_new_vardecl(_tmpD00,_tmpCFF,(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp659=_cycalloc(sizeof(*_tmp659));
_tmp659->tag=19U,({void*_tmpCFE=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp658=_cycalloc(sizeof(*_tmp658));_tmp658->tag=2U,_tmp658->f1=_tmp655;_tmp658;});_tmp659->f1=_tmpCFE;});_tmp659;}),0);});
# 2812
_tmp65A->f2=_tmpD01;});_tmp65A;});
# 2811
Cyc_Absyn_new_pat(_tmpD02,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD03;});
# 2815
goto _LL0;}case 395U: _LL311: _LL312: {
# 2817
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2819
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2818 "parse.y"
({union Cyc_YYSTYPE _tmpD05=Cyc_YY10(({struct _tuple22*_tmp65B=_cycalloc(sizeof(*_tmp65B));({struct Cyc_List_List*_tmpD04=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp65B->f1=_tmpD04;}),_tmp65B->f2=0;_tmp65B;}));yyval=_tmpD05;});
goto _LL0;}case 396U: _LL313: _LL314: {
# 2821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2819 "parse.y"
({union Cyc_YYSTYPE _tmpD07=Cyc_YY10(({struct _tuple22*_tmp65C=_cycalloc(sizeof(*_tmp65C));({struct Cyc_List_List*_tmpD06=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp65C->f1=_tmpD06;}),_tmp65C->f2=1;_tmp65C;}));yyval=_tmpD07;});
goto _LL0;}case 397U: _LL315: _LL316: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2820 "parse.y"
({union Cyc_YYSTYPE _tmpD08=Cyc_YY10(({struct _tuple22*_tmp65D=_cycalloc(sizeof(*_tmp65D));_tmp65D->f1=0,_tmp65D->f2=1;_tmp65D;}));yyval=_tmpD08;});
goto _LL0;}case 398U: _LL317: _LL318: {
# 2823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2825 "parse.y"
({union Cyc_YYSTYPE _tmpD0A=Cyc_YY11(({struct Cyc_List_List*_tmp65E=_cycalloc(sizeof(*_tmp65E));({struct Cyc_Absyn_Pat*_tmpD09=Cyc_yyget_YY9(yyyyvsp[0]);_tmp65E->hd=_tmpD09;}),_tmp65E->tl=0;_tmp65E;}));yyval=_tmpD0A;});
goto _LL0;}case 399U: _LL319: _LL31A: {
# 2828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2827 "parse.y"
({union Cyc_YYSTYPE _tmpD0D=Cyc_YY11(({struct Cyc_List_List*_tmp65F=_cycalloc(sizeof(*_tmp65F));({struct Cyc_Absyn_Pat*_tmpD0C=Cyc_yyget_YY9(yyyyvsp[2]);_tmp65F->hd=_tmpD0C;}),({struct Cyc_List_List*_tmpD0B=Cyc_yyget_YY11(yyyyvsp[0]);_tmp65F->tl=_tmpD0B;});_tmp65F;}));yyval=_tmpD0D;});
goto _LL0;}case 400U: _LL31B: _LL31C: {
# 2830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2832 "parse.y"
({union Cyc_YYSTYPE _tmpD0F=Cyc_YY12(({struct _tuple23*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->f1=0,({struct Cyc_Absyn_Pat*_tmpD0E=Cyc_yyget_YY9(yyyyvsp[0]);_tmp660->f2=_tmpD0E;});_tmp660;}));yyval=_tmpD0F;});
goto _LL0;}case 401U: _LL31D: _LL31E: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2834 "parse.y"
({union Cyc_YYSTYPE _tmpD12=Cyc_YY12(({struct _tuple23*_tmp661=_cycalloc(sizeof(*_tmp661));({struct Cyc_List_List*_tmpD11=Cyc_yyget_YY41(yyyyvsp[0]);_tmp661->f1=_tmpD11;}),({struct Cyc_Absyn_Pat*_tmpD10=Cyc_yyget_YY9(yyyyvsp[1]);_tmp661->f2=_tmpD10;});_tmp661;}));yyval=_tmpD12;});
goto _LL0;}case 402U: _LL31F: _LL320: {
# 2837
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2839
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2837 "parse.y"
({union Cyc_YYSTYPE _tmpD14=Cyc_YY14(({struct _tuple22*_tmp662=_cycalloc(sizeof(*_tmp662));({struct Cyc_List_List*_tmpD13=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp662->f1=_tmpD13;}),_tmp662->f2=0;_tmp662;}));yyval=_tmpD14;});
goto _LL0;}case 403U: _LL321: _LL322: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2838 "parse.y"
({union Cyc_YYSTYPE _tmpD16=Cyc_YY14(({struct _tuple22*_tmp663=_cycalloc(sizeof(*_tmp663));({struct Cyc_List_List*_tmpD15=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp663->f1=_tmpD15;}),_tmp663->f2=1;_tmp663;}));yyval=_tmpD16;});
goto _LL0;}case 404U: _LL323: _LL324: {
# 2841
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2843
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2839 "parse.y"
({union Cyc_YYSTYPE _tmpD17=Cyc_YY14(({struct _tuple22*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->f1=0,_tmp664->f2=1;_tmp664;}));yyval=_tmpD17;});
goto _LL0;}case 405U: _LL325: _LL326: {
# 2842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2844 "parse.y"
({union Cyc_YYSTYPE _tmpD19=Cyc_YY13(({struct Cyc_List_List*_tmp665=_cycalloc(sizeof(*_tmp665));({struct _tuple23*_tmpD18=Cyc_yyget_YY12(yyyyvsp[0]);_tmp665->hd=_tmpD18;}),_tmp665->tl=0;_tmp665;}));yyval=_tmpD19;});
goto _LL0;}case 406U: _LL327: _LL328: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2846 "parse.y"
({union Cyc_YYSTYPE _tmpD1C=Cyc_YY13(({struct Cyc_List_List*_tmp666=_cycalloc(sizeof(*_tmp666));({struct _tuple23*_tmpD1B=Cyc_yyget_YY12(yyyyvsp[2]);_tmp666->hd=_tmpD1B;}),({struct Cyc_List_List*_tmpD1A=Cyc_yyget_YY13(yyyyvsp[0]);_tmp666->tl=_tmpD1A;});_tmp666;}));yyval=_tmpD1C;});
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2849
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2851
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2855
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2857
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2854 "parse.y"
({union Cyc_YYSTYPE _tmpD1F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD1E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD1D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpD1E,_tmpD1D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD1F;});
goto _LL0;}case 409U: _LL32D: _LL32E: {
# 2857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2859 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 410U: _LL32F: _LL330: {
# 2862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2861 "parse.y"
({union Cyc_YYSTYPE _tmpD23=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD22=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpD21=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpD20=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpD22,_tmpD21,_tmpD20,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD23;});
goto _LL0;}case 411U: _LL331: _LL332: {
# 2864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2863 "parse.y"
({union Cyc_YYSTYPE _tmpD26=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD25=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD24=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpD25,_tmpD24,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD26;});
goto _LL0;}case 412U: _LL333: _LL334: {
# 2866
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2868
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2867 "parse.y"
({union Cyc_YYSTYPE _tmpD27=Cyc_YY7(0);yyval=_tmpD27;});
goto _LL0;}case 413U: _LL335: _LL336: {
# 2870
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2872
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2868 "parse.y"
({union Cyc_YYSTYPE _tmpD28=Cyc_YY7(({struct Cyc_Core_Opt*_tmp667=_cycalloc(sizeof(*_tmp667));_tmp667->v=(void*)Cyc_Absyn_Times;_tmp667;}));yyval=_tmpD28;});
goto _LL0;}case 414U: _LL337: _LL338: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2869 "parse.y"
({union Cyc_YYSTYPE _tmpD29=Cyc_YY7(({struct Cyc_Core_Opt*_tmp668=_cycalloc(sizeof(*_tmp668));_tmp668->v=(void*)Cyc_Absyn_Div;_tmp668;}));yyval=_tmpD29;});
goto _LL0;}case 415U: _LL339: _LL33A: {
# 2872
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2874
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2870 "parse.y"
({union Cyc_YYSTYPE _tmpD2A=Cyc_YY7(({struct Cyc_Core_Opt*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->v=(void*)Cyc_Absyn_Mod;_tmp669;}));yyval=_tmpD2A;});
goto _LL0;}case 416U: _LL33B: _LL33C: {
# 2873
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2875
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2871 "parse.y"
({union Cyc_YYSTYPE _tmpD2B=Cyc_YY7(({struct Cyc_Core_Opt*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->v=(void*)Cyc_Absyn_Plus;_tmp66A;}));yyval=_tmpD2B;});
goto _LL0;}case 417U: _LL33D: _LL33E: {
# 2874
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2876
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2872 "parse.y"
({union Cyc_YYSTYPE _tmpD2C=Cyc_YY7(({struct Cyc_Core_Opt*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->v=(void*)Cyc_Absyn_Minus;_tmp66B;}));yyval=_tmpD2C;});
goto _LL0;}case 418U: _LL33F: _LL340: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2873 "parse.y"
({union Cyc_YYSTYPE _tmpD2D=Cyc_YY7(({struct Cyc_Core_Opt*_tmp66C=_cycalloc(sizeof(*_tmp66C));_tmp66C->v=(void*)Cyc_Absyn_Bitlshift;_tmp66C;}));yyval=_tmpD2D;});
goto _LL0;}case 419U: _LL341: _LL342: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2874 "parse.y"
({union Cyc_YYSTYPE _tmpD2E=Cyc_YY7(({struct Cyc_Core_Opt*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->v=(void*)Cyc_Absyn_Bitlrshift;_tmp66D;}));yyval=_tmpD2E;});
goto _LL0;}case 420U: _LL343: _LL344: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2875 "parse.y"
({union Cyc_YYSTYPE _tmpD2F=Cyc_YY7(({struct Cyc_Core_Opt*_tmp66E=_cycalloc(sizeof(*_tmp66E));_tmp66E->v=(void*)Cyc_Absyn_Bitand;_tmp66E;}));yyval=_tmpD2F;});
goto _LL0;}case 421U: _LL345: _LL346: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2876 "parse.y"
({union Cyc_YYSTYPE _tmpD30=Cyc_YY7(({struct Cyc_Core_Opt*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->v=(void*)Cyc_Absyn_Bitxor;_tmp66F;}));yyval=_tmpD30;});
goto _LL0;}case 422U: _LL347: _LL348: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2877 "parse.y"
({union Cyc_YYSTYPE _tmpD31=Cyc_YY7(({struct Cyc_Core_Opt*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670->v=(void*)Cyc_Absyn_Bitor;_tmp670;}));yyval=_tmpD31;});
goto _LL0;}case 423U: _LL349: _LL34A: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2882 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 424U: _LL34B: _LL34C: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2884 "parse.y"
({union Cyc_YYSTYPE _tmpD35=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD34=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD33=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD32=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpD34,_tmpD33,_tmpD32,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD35;});
goto _LL0;}case 425U: _LL34D: _LL34E: {
# 2887
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2889
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2887 "parse.y"
({union Cyc_YYSTYPE _tmpD37=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD36=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpD36,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD37;});
goto _LL0;}case 426U: _LL34F: _LL350: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2890 "parse.y"
({union Cyc_YYSTYPE _tmpD39=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD38=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpD38,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD39;});
goto _LL0;}case 427U: _LL351: _LL352: {
# 2893
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2895
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2892 "parse.y"
({union Cyc_YYSTYPE _tmpD3B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD3A=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpD3A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD3B;});
goto _LL0;}case 428U: _LL353: _LL354: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2894 "parse.y"
({union Cyc_YYSTYPE _tmpD3E=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD3D=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD3C=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpD3D,_tmpD3C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD3E;});
goto _LL0;}case 429U: _LL355: _LL356: {
# 2897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2896 "parse.y"
({union Cyc_YYSTYPE _tmpD41=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD40=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD3F=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpD40,_tmpD3F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD41;});
goto _LL0;}case 430U: _LL357: _LL358: {
# 2899
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2901
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2900 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 431U: _LL359: _LL35A: {
# 2903
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2905
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2904 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 432U: _LL35B: _LL35C: {
# 2907
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2909
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2906 "parse.y"
({union Cyc_YYSTYPE _tmpD44=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD43=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD42=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpD43,_tmpD42,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD44;});
goto _LL0;}case 433U: _LL35D: _LL35E: {
# 2909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2910 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 434U: _LL35F: _LL360: {
# 2913
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2915
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2912 "parse.y"
({union Cyc_YYSTYPE _tmpD47=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD46=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD45=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpD46,_tmpD45,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD47;});
goto _LL0;}case 435U: _LL361: _LL362: {
# 2915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2916 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 436U: _LL363: _LL364: {
# 2919
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2921
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2918 "parse.y"
({union Cyc_YYSTYPE _tmpD4A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD49=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD48=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpD49,_tmpD48,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD4A;});
goto _LL0;}case 437U: _LL365: _LL366: {
# 2921
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2923
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2922 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 438U: _LL367: _LL368: {
# 2925
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2927
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2924 "parse.y"
({union Cyc_YYSTYPE _tmpD4D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD4C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD4B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpD4C,_tmpD4B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD4D;});
goto _LL0;}case 439U: _LL369: _LL36A: {
# 2927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2928 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 440U: _LL36B: _LL36C: {
# 2931
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2933
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2930 "parse.y"
({union Cyc_YYSTYPE _tmpD50=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD4F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD4E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpD4F,_tmpD4E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD50;});
goto _LL0;}case 441U: _LL36D: _LL36E: {
# 2933
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2935
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2934 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 442U: _LL36F: _LL370: {
# 2937
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2939
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2936 "parse.y"
({union Cyc_YYSTYPE _tmpD53=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD52=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD51=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpD52,_tmpD51,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD53;});
goto _LL0;}case 443U: _LL371: _LL372: {
# 2939
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2941
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2938 "parse.y"
({union Cyc_YYSTYPE _tmpD56=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD55=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD54=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpD55,_tmpD54,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD56;});
goto _LL0;}case 444U: _LL373: _LL374: {
# 2941
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2943
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2942 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 445U: _LL375: _LL376: {
# 2945
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2947
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2944 "parse.y"
({union Cyc_YYSTYPE _tmpD59=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD58=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD57=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpD58,_tmpD57,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD59;});
goto _LL0;}case 446U: _LL377: _LL378: {
# 2947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2946 "parse.y"
({union Cyc_YYSTYPE _tmpD5C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD5B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD5A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpD5B,_tmpD5A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5C;});
goto _LL0;}case 447U: _LL379: _LL37A: {
# 2949
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2951
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2948 "parse.y"
({union Cyc_YYSTYPE _tmpD5F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD5E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD5D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpD5E,_tmpD5D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5F;});
goto _LL0;}case 448U: _LL37B: _LL37C: {
# 2951
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2953
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2950 "parse.y"
({union Cyc_YYSTYPE _tmpD62=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD61=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD60=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpD61,_tmpD60,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD62;});
goto _LL0;}case 449U: _LL37D: _LL37E: {
# 2953
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2955
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2954 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 450U: _LL37F: _LL380: {
# 2957
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2959
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2956 "parse.y"
({union Cyc_YYSTYPE _tmpD65=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD64=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD63=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpD64,_tmpD63,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD65;});
goto _LL0;}case 451U: _LL381: _LL382: {
# 2959
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2961
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2958 "parse.y"
({union Cyc_YYSTYPE _tmpD68=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD67=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD66=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpD67,_tmpD66,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD68;});
goto _LL0;}case 452U: _LL383: _LL384: {
# 2961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2962 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 453U: _LL385: _LL386: {
# 2965
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2967
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2964 "parse.y"
({union Cyc_YYSTYPE _tmpD6B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD6A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD69=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpD6A,_tmpD69,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6B;});
goto _LL0;}case 454U: _LL387: _LL388: {
# 2967
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2969
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2966 "parse.y"
({union Cyc_YYSTYPE _tmpD6E=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD6D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD6C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpD6D,_tmpD6C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6E;});
goto _LL0;}case 455U: _LL389: _LL38A: {
# 2969
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2971
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2970 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 456U: _LL38B: _LL38C: {
# 2973
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2975
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2972 "parse.y"
({union Cyc_YYSTYPE _tmpD71=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD70=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD6F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpD70,_tmpD6F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD71;});
goto _LL0;}case 457U: _LL38D: _LL38E: {
# 2975
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2977
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2974 "parse.y"
({union Cyc_YYSTYPE _tmpD74=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD73=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD72=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpD73,_tmpD72,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD74;});
goto _LL0;}case 458U: _LL38F: _LL390: {
# 2977
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2979
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2976 "parse.y"
({union Cyc_YYSTYPE _tmpD77=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD76=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD75=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpD76,_tmpD75,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD77;});
goto _LL0;}case 459U: _LL391: _LL392: {
# 2979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2980 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 460U: _LL393: _LL394: {
# 2983
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2985
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2982 "parse.y"
void*_tmp671=({struct _tuple8*_tmpD78=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpD78,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpD7B=Cyc_Exp_tok(({void*_tmpD7A=_tmp671;struct Cyc_Absyn_Exp*_tmpD79=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpD7A,_tmpD79,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7B;});
# 2985
goto _LL0;}case 461U: _LL395: _LL396: {
# 2987
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2989
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2988 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 462U: _LL397: _LL398: {
# 2991
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2993
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2989 "parse.y"
({union Cyc_YYSTYPE _tmpD7D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD7C=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpD7C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7D;});
goto _LL0;}case 463U: _LL399: _LL39A: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2990 "parse.y"
({union Cyc_YYSTYPE _tmpD7F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD7E=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpD7E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7F;});
goto _LL0;}case 464U: _LL39B: _LL39C: {
# 2993
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2995
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2991 "parse.y"
({union Cyc_YYSTYPE _tmpD81=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD80=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpD80,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD81;});
goto _LL0;}case 465U: _LL39D: _LL39E: {
# 2994
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2996
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2992 "parse.y"
({union Cyc_YYSTYPE _tmpD83=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD82=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpD82,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD83;});
goto _LL0;}case 466U: _LL39F: _LL3A0: {
# 2995
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2997
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2993 "parse.y"
({union Cyc_YYSTYPE _tmpD85=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD84=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpD84,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD85;});
goto _LL0;}case 467U: _LL3A1: _LL3A2: {
# 2996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2994 "parse.y"
({union Cyc_YYSTYPE _tmpD88=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpD87=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD86=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpD87,_tmpD86,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD88;});
goto _LL0;}case 468U: _LL3A3: _LL3A4: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2996 "parse.y"
void*_tmp672=({struct _tuple8*_tmpD89=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpD89,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpD8A=Cyc_Exp_tok(Cyc_Absyn_sizeoftyp_exp(_tmp672,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD8A;});
# 2999
goto _LL0;}case 469U: _LL3A5: _LL3A6: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2999 "parse.y"
({union Cyc_YYSTYPE _tmpD8C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD8B=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpD8B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD8C;});
goto _LL0;}case 470U: _LL3A7: _LL3A8: {
# 3002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3001 "parse.y"
void*_tmp673=({struct _tuple8*_tmpD8D=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpD8D,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpD90=Cyc_Exp_tok(({void*_tmpD8F=_tmp673;struct Cyc_List_List*_tmpD8E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpD8F,_tmpD8E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD90;});
# 3004
goto _LL0;}case 471U: _LL3A9: _LL3AA: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3006 "parse.y"
({union Cyc_YYSTYPE _tmpD93=Cyc_Exp_tok(({void*_tmpD92=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->tag=34U,(_tmp674->f1).is_calloc=0,(_tmp674->f1).rgn=0,(_tmp674->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpD91=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp674->f1).num_elts=_tmpD91;}),(_tmp674->f1).fat_result=0,(_tmp674->f1).inline_call=0;_tmp674;});Cyc_Absyn_new_exp(_tmpD92,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD93;});
# 3008
goto _LL0;}case 472U: _LL3AB: _LL3AC: {
# 3010
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3012
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3009 "parse.y"
({union Cyc_YYSTYPE _tmpD97=Cyc_Exp_tok(({void*_tmpD96=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp675=_cycalloc(sizeof(*_tmp675));_tmp675->tag=34U,(_tmp675->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpD95=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp675->f1).rgn=_tmpD95;}),(_tmp675->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpD94=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp675->f1).num_elts=_tmpD94;}),(_tmp675->f1).fat_result=0,(_tmp675->f1).inline_call=0;_tmp675;});Cyc_Absyn_new_exp(_tmpD96,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD97;});
# 3011
goto _LL0;}case 473U: _LL3AD: _LL3AE: {
# 3013
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3015
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3012 "parse.y"
({union Cyc_YYSTYPE _tmpD9B=Cyc_Exp_tok(({void*_tmpD9A=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->tag=34U,(_tmp676->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpD99=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp676->f1).rgn=_tmpD99;}),(_tmp676->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpD98=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp676->f1).num_elts=_tmpD98;}),(_tmp676->f1).fat_result=0,(_tmp676->f1).inline_call=1;_tmp676;});Cyc_Absyn_new_exp(_tmpD9A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9B;});
# 3014
goto _LL0;}case 474U: _LL3AF: _LL3B0: {
# 3016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 3018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 3015 "parse.y"
void*_tmp677=({struct _tuple8*_tmpD9C=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpD9C,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpDA0=Cyc_Exp_tok(({void*_tmpD9F=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->tag=34U,(_tmp679->f1).is_calloc=1,(_tmp679->f1).rgn=0,({void**_tmpD9E=({void**_tmp678=_cycalloc(sizeof(*_tmp678));*_tmp678=_tmp677;_tmp678;});(_tmp679->f1).elt_type=_tmpD9E;}),({struct Cyc_Absyn_Exp*_tmpD9D=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp679->f1).num_elts=_tmpD9D;}),(_tmp679->f1).fat_result=0,(_tmp679->f1).inline_call=0;_tmp679;});Cyc_Absyn_new_exp(_tmpD9F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA0;});
# 3018
goto _LL0;}case 475U: _LL3B1: _LL3B2: {
# 3020
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11U));
# 3022
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11U));
# 3020 "parse.y"
void*_tmp67A=({struct _tuple8*_tmpDA1=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpDA1,(unsigned int)(yyyylsp[8]).first_line);});
({union Cyc_YYSTYPE _tmpDA6=Cyc_Exp_tok(({void*_tmpDA5=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->tag=34U,(_tmp67C->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpDA4=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp67C->f1).rgn=_tmpDA4;}),({void**_tmpDA3=({void**_tmp67B=_cycalloc(sizeof(*_tmp67B));*_tmp67B=_tmp67A;_tmp67B;});(_tmp67C->f1).elt_type=_tmpDA3;}),({struct Cyc_Absyn_Exp*_tmpDA2=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp67C->f1).num_elts=_tmpDA2;}),(_tmp67C->f1).fat_result=0,(_tmp67C->f1).inline_call=0;_tmp67C;});Cyc_Absyn_new_exp(_tmpDA5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA6;});
# 3023
goto _LL0;}case 476U: _LL3B3: _LL3B4: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3024 "parse.y"
({union Cyc_YYSTYPE _tmpDA9=Cyc_Exp_tok(({struct Cyc_List_List*_tmpDA8=({struct Cyc_Absyn_Exp*_tmp67D[1U];({struct Cyc_Absyn_Exp*_tmpDA7=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp67D[0]=_tmpDA7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp67D,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpDA8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA9;});
goto _LL0;}case 477U: _LL3B5: _LL3B6: {
# 3027
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3029
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3026 "parse.y"
({union Cyc_YYSTYPE _tmpDAE=Cyc_Exp_tok(({void*_tmpDAD=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F->tag=38U,({struct Cyc_Absyn_Exp*_tmpDAC=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp67F->f1=_tmpDAC;}),({struct _dyneither_ptr*_tmpDAB=({struct _dyneither_ptr*_tmp67E=_cycalloc(sizeof(*_tmp67E));({struct _dyneither_ptr _tmpDAA=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp67E=_tmpDAA;});_tmp67E;});_tmp67F->f2=_tmpDAB;});_tmp67F;});Cyc_Absyn_new_exp(_tmpDAD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDAE;});
goto _LL0;}case 478U: _LL3B7: _LL3B8: {
# 3029
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3031
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3028 "parse.y"
({union Cyc_YYSTYPE _tmpDB4=Cyc_Exp_tok(({void*_tmpDB3=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681->tag=38U,({struct Cyc_Absyn_Exp*_tmpDB2=({struct Cyc_Absyn_Exp*_tmpDB1=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpDB1,(unsigned int)(yyyylsp[2]).first_line);});_tmp681->f1=_tmpDB2;}),({struct _dyneither_ptr*_tmpDB0=({struct _dyneither_ptr*_tmp680=_cycalloc(sizeof(*_tmp680));({struct _dyneither_ptr _tmpDAF=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp680=_tmpDAF;});_tmp680;});_tmp681->f2=_tmpDB0;});_tmp681;});Cyc_Absyn_new_exp(_tmpDB3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDB4;});
goto _LL0;}case 479U: _LL3B9: _LL3BA: {
# 3031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3030 "parse.y"
void*_tmp682=({struct _tuple8*_tmpDB5=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpDB5,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpDB6=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp682,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDB6;});
goto _LL0;}case 480U: _LL3BB: _LL3BC: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3033 "parse.y"
struct _tuple20 _tmp683=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp684=_tmp683;int _tmp686;struct _dyneither_ptr _tmp685;_LL48C: _tmp686=_tmp684.f1;_tmp685=_tmp684.f2;_LL48D:;
({union Cyc_YYSTYPE _tmpDB7=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp686,_tmp685,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDB7;});
goto _LL0;}case 481U: _LL3BD: _LL3BE: {
# 3037
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3039
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3038 "parse.y"
({union Cyc_YYSTYPE _tmpDB8=Cyc_YY6(Cyc_Absyn_Bitnot);yyval=_tmpDB8;});
goto _LL0;}case 482U: _LL3BF: _LL3C0: {
# 3041
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3043
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3039 "parse.y"
({union Cyc_YYSTYPE _tmpDB9=Cyc_YY6(Cyc_Absyn_Not);yyval=_tmpDB9;});
goto _LL0;}case 483U: _LL3C1: _LL3C2: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3040 "parse.y"
({union Cyc_YYSTYPE _tmpDBA=Cyc_YY6(Cyc_Absyn_Minus);yyval=_tmpDBA;});
goto _LL0;}case 484U: _LL3C3: _LL3C4: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3045 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 485U: _LL3C5: _LL3C6: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3047 "parse.y"
({union Cyc_YYSTYPE _tmpDBD=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDBC=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpDBB=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpDBC,_tmpDBB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBD;});
goto _LL0;}case 486U: _LL3C7: _LL3C8: {
# 3050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3049 "parse.y"
({union Cyc_YYSTYPE _tmpDBF=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDBE=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpDBE,0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBF;});
goto _LL0;}case 487U: _LL3C9: _LL3CA: {
# 3052
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3054
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3051 "parse.y"
({union Cyc_YYSTYPE _tmpDC2=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDC1=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpDC0=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpDC1,_tmpDC0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC2;});
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3053 "parse.y"
({union Cyc_YYSTYPE _tmpDC6=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDC5=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpDC4=({struct _dyneither_ptr*_tmp687=_cycalloc(sizeof(*_tmp687));({struct _dyneither_ptr _tmpDC3=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp687=_tmpDC3;});_tmp687;});Cyc_Absyn_aggrmember_exp(_tmpDC5,_tmpDC4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC6;});
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3055 "parse.y"
({union Cyc_YYSTYPE _tmpDCA=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDC9=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpDC8=({struct _dyneither_ptr*_tmp688=_cycalloc(sizeof(*_tmp688));({struct _dyneither_ptr _tmpDC7=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp688=_tmpDC7;});_tmp688;});Cyc_Absyn_aggrarrow_exp(_tmpDC9,_tmpDC8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDCA;});
goto _LL0;}case 490U: _LL3CF: _LL3D0: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3057 "parse.y"
({union Cyc_YYSTYPE _tmpDCC=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDCB=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpDCB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDCC;});
goto _LL0;}case 491U: _LL3D1: _LL3D2: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3059 "parse.y"
({union Cyc_YYSTYPE _tmpDCE=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDCD=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpDCD,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDCE;});
goto _LL0;}case 492U: _LL3D3: _LL3D4: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3061 "parse.y"
({union Cyc_YYSTYPE _tmpDD1=Cyc_Exp_tok(({void*_tmpDD0=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689->tag=25U,({struct _tuple8*_tmpDCF=Cyc_yyget_YY37(yyyyvsp[1]);_tmp689->f1=_tmpDCF;}),_tmp689->f2=0;_tmp689;});Cyc_Absyn_new_exp(_tmpDD0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDD1;});
goto _LL0;}case 493U: _LL3D5: _LL3D6: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3063 "parse.y"
({union Cyc_YYSTYPE _tmpDD5=Cyc_Exp_tok(({void*_tmpDD4=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68A=_cycalloc(sizeof(*_tmp68A));_tmp68A->tag=25U,({struct _tuple8*_tmpDD3=Cyc_yyget_YY37(yyyyvsp[1]);_tmp68A->f1=_tmpDD3;}),({struct Cyc_List_List*_tmpDD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp68A->f2=_tmpDD2;});_tmp68A;});Cyc_Absyn_new_exp(_tmpDD4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDD5;});
goto _LL0;}case 494U: _LL3D7: _LL3D8: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 3065 "parse.y"
({union Cyc_YYSTYPE _tmpDD9=Cyc_Exp_tok(({void*_tmpDD8=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B->tag=25U,({struct _tuple8*_tmpDD7=Cyc_yyget_YY37(yyyyvsp[1]);_tmp68B->f1=_tmpDD7;}),({struct Cyc_List_List*_tmpDD6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp68B->f2=_tmpDD6;});_tmp68B;});Cyc_Absyn_new_exp(_tmpDD8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDD9;});
goto _LL0;}case 495U: _LL3D9: _LL3DA: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3070 "parse.y"
({union Cyc_YYSTYPE _tmpDDD=Cyc_YY3(({struct Cyc_List_List*_tmp68E=_cycalloc(sizeof(*_tmp68E));({void*_tmpDDC=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));_tmp68D->tag=0U,({struct _dyneither_ptr*_tmpDDB=({struct _dyneither_ptr*_tmp68C=_cycalloc(sizeof(*_tmp68C));({struct _dyneither_ptr _tmpDDA=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp68C=_tmpDDA;});_tmp68C;});_tmp68D->f1=_tmpDDB;});_tmp68D;});_tmp68E->hd=_tmpDDC;}),_tmp68E->tl=0;_tmp68E;}));yyval=_tmpDDD;});
goto _LL0;}case 496U: _LL3DB: _LL3DC: {
# 3073
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3075
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3073 "parse.y"
({union Cyc_YYSTYPE _tmpDE1=Cyc_YY3(({struct Cyc_List_List*_tmp690=_cycalloc(sizeof(*_tmp690));({void*_tmpDE0=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->tag=1U,({unsigned int _tmpDDF=({unsigned int _tmpDDE=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpDDE,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp68F->f1=_tmpDDF;});_tmp68F;});_tmp690->hd=_tmpDE0;}),_tmp690->tl=0;_tmp690;}));yyval=_tmpDE1;});
goto _LL0;}case 497U: _LL3DD: _LL3DE: {
# 3076
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3078
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3075 "parse.y"
({union Cyc_YYSTYPE _tmpDE6=Cyc_YY3(({struct Cyc_List_List*_tmp693=_cycalloc(sizeof(*_tmp693));({void*_tmpDE5=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692->tag=0U,({struct _dyneither_ptr*_tmpDE4=({struct _dyneither_ptr*_tmp691=_cycalloc(sizeof(*_tmp691));({struct _dyneither_ptr _tmpDE3=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp691=_tmpDE3;});_tmp691;});_tmp692->f1=_tmpDE4;});_tmp692;});_tmp693->hd=_tmpDE5;}),({struct Cyc_List_List*_tmpDE2=Cyc_yyget_YY3(yyyyvsp[0]);_tmp693->tl=_tmpDE2;});_tmp693;}));yyval=_tmpDE6;});
goto _LL0;}case 498U: _LL3DF: _LL3E0: {
# 3078
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3080
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3078 "parse.y"
({union Cyc_YYSTYPE _tmpDEB=Cyc_YY3(({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));({void*_tmpDEA=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp694=_cycalloc(sizeof(*_tmp694));_tmp694->tag=1U,({unsigned int _tmpDE9=({unsigned int _tmpDE8=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpDE8,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp694->f1=_tmpDE9;});_tmp694;});_tmp695->hd=_tmpDEA;}),({struct Cyc_List_List*_tmpDE7=Cyc_yyget_YY3(yyyyvsp[0]);_tmp695->tl=_tmpDE7;});_tmp695;}));yyval=_tmpDEB;});
goto _LL0;}case 499U: _LL3E1: _LL3E2: {
# 3081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpDED=Cyc_Exp_tok(({struct _tuple0*_tmpDEC=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpDEC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDED;});
goto _LL0;}case 500U: _LL3E3: _LL3E4: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3086 "parse.y"
({union Cyc_YYSTYPE _tmpDEF=Cyc_Exp_tok(({struct _dyneither_ptr _tmpDEE=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Absyn_pragma_exp(_tmpDEE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDEF;});
goto _LL0;}case 501U: _LL3E5: _LL3E6: {
# 3089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3088 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 502U: _LL3E7: _LL3E8: {
# 3091
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3093
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3090 "parse.y"
({union Cyc_YYSTYPE _tmpDF1=Cyc_Exp_tok(({struct _dyneither_ptr _tmpDF0=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpDF0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDF1;});
goto _LL0;}case 503U: _LL3E9: _LL3EA: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3092 "parse.y"
({union Cyc_YYSTYPE _tmpDF3=Cyc_Exp_tok(({struct _dyneither_ptr _tmpDF2=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpDF2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDF3;});
goto _LL0;}case 504U: _LL3EB: _LL3EC: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3094 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 505U: _LL3ED: _LL3EE: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3099 "parse.y"
({union Cyc_YYSTYPE _tmpDF5=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDF4=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpDF4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDF5;});
goto _LL0;}case 506U: _LL3EF: _LL3F0: {
# 3102
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3104
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3101 "parse.y"
({union Cyc_YYSTYPE _tmpDF8=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDF7=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpDF6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpDF7,_tmpDF6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDF8;});
goto _LL0;}case 507U: _LL3F1: _LL3F2: {
# 3104
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3106
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3104 "parse.y"
({union Cyc_YYSTYPE _tmpDFA=Cyc_Exp_tok(({struct Cyc_List_List*_tmpDF9=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpDF9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDFA;});
goto _LL0;}case 508U: _LL3F3: _LL3F4: {
# 3107
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3109
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3107 "parse.y"
({union Cyc_YYSTYPE _tmpDFF=Cyc_Exp_tok(({void*_tmpDFE=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->tag=29U,({struct _tuple0*_tmpDFD=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp696->f1=_tmpDFD;}),({struct Cyc_List_List*_tmpDFC=Cyc_yyget_YY40(yyyyvsp[2]);_tmp696->f2=_tmpDFC;}),({struct Cyc_List_List*_tmpDFB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp696->f3=_tmpDFB;}),_tmp696->f4=0;_tmp696;});Cyc_Absyn_new_exp(_tmpDFE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDFF;});
goto _LL0;}case 509U: _LL3F5: _LL3F6: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3110 "parse.y"
({union Cyc_YYSTYPE _tmpE01=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpE00=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpE00,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE01;});
goto _LL0;}case 510U: _LL3F7: _LL3F8: {
# 3113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3114 "parse.y"
({union Cyc_YYSTYPE _tmpE02=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));yyval=_tmpE02;});
goto _LL0;}case 511U: _LL3F9: _LL3FA: {
# 3117
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpE04=Cyc_YY4(({struct Cyc_List_List*_tmp697=_cycalloc(sizeof(*_tmp697));({struct Cyc_Absyn_Exp*_tmpE03=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp697->hd=_tmpE03;}),_tmp697->tl=0;_tmp697;}));yyval=_tmpE04;});
goto _LL0;}case 512U: _LL3FB: _LL3FC: {
# 3123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3122 "parse.y"
({union Cyc_YYSTYPE _tmpE07=Cyc_YY4(({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));({struct Cyc_Absyn_Exp*_tmpE06=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp698->hd=_tmpE06;}),({struct Cyc_List_List*_tmpE05=Cyc_yyget_YY4(yyyyvsp[0]);_tmp698->tl=_tmpE05;});_tmp698;}));yyval=_tmpE07;});
goto _LL0;}case 513U: _LL3FD: _LL3FE: {
# 3125
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3127
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpE09=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpE08=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpE08,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE09;});
goto _LL0;}case 514U: _LL3FF: _LL400: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3129 "parse.y"
({union Cyc_YYSTYPE _tmpE0B=Cyc_Exp_tok(({char _tmpE0A=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpE0A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE0B;});
goto _LL0;}case 515U: _LL401: _LL402: {
# 3132
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3134
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3130 "parse.y"
({union Cyc_YYSTYPE _tmpE0D=Cyc_Exp_tok(({struct _dyneither_ptr _tmpE0C=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpE0C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpE0D;});
goto _LL0;}case 516U: _LL403: _LL404: {
# 3133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3132 "parse.y"
struct _dyneither_ptr _tmp699=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp699);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp699,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3140
({union Cyc_YYSTYPE _tmpE0E=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp699,i,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpE0E;});
# 3142
goto _LL0;}case 517U: _LL405: _LL406: {
# 3144
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3146
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3143 "parse.y"
({union Cyc_YYSTYPE _tmpE0F=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpE0F;});
goto _LL0;}case 518U: _LL407: _LL408: {
# 3146
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3148
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3147 "parse.y"
({union Cyc_YYSTYPE _tmpE13=Cyc_QualId_tok(({struct _tuple0*_tmp69B=_cycalloc(sizeof(*_tmp69B));({union Cyc_Absyn_Nmspace _tmpE12=Cyc_Absyn_Rel_n(0);_tmp69B->f1=_tmpE12;}),({struct _dyneither_ptr*_tmpE11=({struct _dyneither_ptr*_tmp69A=_cycalloc(sizeof(*_tmp69A));({struct _dyneither_ptr _tmpE10=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp69A=_tmpE10;});_tmp69A;});_tmp69B->f2=_tmpE11;});_tmp69B;}));yyval=_tmpE13;});
goto _LL0;}case 519U: _LL409: _LL40A: {
# 3150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3148 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 520U: _LL40B: _LL40C: {
# 3151
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3153
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3151 "parse.y"
({union Cyc_YYSTYPE _tmpE17=Cyc_QualId_tok(({struct _tuple0*_tmp69D=_cycalloc(sizeof(*_tmp69D));({union Cyc_Absyn_Nmspace _tmpE16=Cyc_Absyn_Rel_n(0);_tmp69D->f1=_tmpE16;}),({struct _dyneither_ptr*_tmpE15=({struct _dyneither_ptr*_tmp69C=_cycalloc(sizeof(*_tmp69C));({struct _dyneither_ptr _tmpE14=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp69C=_tmpE14;});_tmp69C;});_tmp69D->f2=_tmpE15;});_tmp69D;}));yyval=_tmpE17;});
goto _LL0;}case 521U: _LL40D: _LL40E: {
# 3154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3152 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 522U: _LL40F: _LL410: {
# 3155
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3157
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3157 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 523U: _LL411: _LL412: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3158 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 524U: _LL413: _LL414: {
# 3161
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3161 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 525U: _LL415: _LL416: {
# 3164
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3166
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3162 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 526U: _LL417: _LL418: {
# 3165
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3167
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3167 "parse.y"
goto _LL0;}case 527U: _LL419: _LL41A: {
# 3169
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3171
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3167 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL0;}default: _LL41B: _LL41C:
# 3172
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
({int _tmpE18=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpE18;});
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
({int _tmpE1A=({short _tmpE19=Cyc_yypgoto[_check_known_subscript_notnull(147U,yyn - 147)];_tmpE19 + *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});yystate=_tmpE1A;});
if((yystate >= 0  && yystate <= 7177) && Cyc_yycheck[_check_known_subscript_notnull(7178U,yystate)]== ((short*)yyss.curr)[yyssp_offset])
yystate=(int)Cyc_yytable[yystate];else{
# 421
yystate=(int)Cyc_yydefgoto[_check_known_subscript_notnull(147U,yyn - 147)];}
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
if(yyn > - 32768  && yyn < 7177){
# 437
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 441
count=0;
# 443
for(x=yyn < 0?- yyn: 0;x < 294U / sizeof(char*);++ x){
# 445
if(Cyc_yycheck[_check_known_subscript_notnull(7178U,x + yyn)]== x)
({unsigned long _tmpE1B=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(294U,x)])+ 15;sze +=_tmpE1B;}),count ++;}
({struct _dyneither_ptr _tmpE1D=({unsigned int _tmp69F=(unsigned int)(sze + 15)+ 1U;char*_tmp69E=({struct _RegionHandle*_tmpE1C=yyregion;_region_malloc(_tmpE1C,_check_times(_tmp69F,sizeof(char)));});({{unsigned int _tmp758=(unsigned int)(sze + 15);unsigned int i;for(i=0;i < _tmp758;++ i){_tmp69E[i]='\000';}_tmp69E[_tmp758]=0;}0;});_tag_dyneither(_tmp69E,sizeof(char),_tmp69F);});msg=_tmpE1D;});
({struct _dyneither_ptr _tmpE1E=msg;Cyc_strcpy(_tmpE1E,({const char*_tmp6A0="parse error";_tag_dyneither(_tmp6A0,sizeof(char),12U);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;x < 294U / sizeof(char*);++ x){
# 455
if(Cyc_yycheck[_check_known_subscript_notnull(7178U,x + yyn)]== x){
# 457
({struct _dyneither_ptr _tmpE1F=msg;Cyc_strcat(_tmpE1F,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp6A1=", expecting `";_tag_dyneither(_tmp6A1,sizeof(char),14U);}):(struct _dyneither_ptr)({const char*_tmp6A2=" or `";_tag_dyneither(_tmp6A2,sizeof(char),6U);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(294U,x)]);
({struct _dyneither_ptr _tmpE20=msg;Cyc_strcat(_tmpE20,({const char*_tmp6A3="'";_tag_dyneither(_tmp6A3,sizeof(char),2U);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpE22=({const char*_tmp6A4="parse error";_tag_dyneither(_tmp6A4,sizeof(char),12U);});int _tmpE21=yystate;Cyc_yyerror(_tmpE22,_tmpE21,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp6A5=1;_npop_handler(0U);return _tmp6A5;}
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
 if(yyssp_offset == 0){int _tmp6A6=1;_npop_handler(0U);return _tmp6A6;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1074U,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7177) || Cyc_yycheck[_check_known_subscript_notnull(7178U,yyn)]!= 1)goto yyerrdefault;
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
if(yyn == 1073){
int _tmp6A7=0;_npop_handler(0U);return _tmp6A7;}
# 554
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 556
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 559
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3170 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp6AE=v;struct Cyc_Absyn_Stmt*_tmp6C8;struct Cyc_Absyn_Exp*_tmp6C7;struct _tuple0*_tmp6C6;struct _dyneither_ptr _tmp6C5;char _tmp6C4;union Cyc_Absyn_Cnst _tmp6C3;switch((_tmp6AE.Stmt_tok).tag){case 1U: _LL1: _tmp6C3=(_tmp6AE.Int_tok).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp6B1=({struct Cyc_String_pa_PrintArg_struct _tmp75C;_tmp75C.tag=0U,({struct _dyneither_ptr _tmpE23=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp6C3));_tmp75C.f1=_tmpE23;});_tmp75C;});void*_tmp6AF[1U];_tmp6AF[0]=& _tmp6B1;({struct Cyc___cycFILE*_tmpE25=Cyc_stderr;struct _dyneither_ptr _tmpE24=({const char*_tmp6B0="%s";_tag_dyneither(_tmp6B0,sizeof(char),3U);});Cyc_fprintf(_tmpE25,_tmpE24,_tag_dyneither(_tmp6AF,sizeof(void*),1U));});});goto _LL0;case 2U: _LL3: _tmp6C4=(_tmp6AE.Char_tok).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp6B4=({struct Cyc_Int_pa_PrintArg_struct _tmp75D;_tmp75D.tag=1U,_tmp75D.f1=(unsigned long)((int)_tmp6C4);_tmp75D;});void*_tmp6B2[1U];_tmp6B2[0]=& _tmp6B4;({struct Cyc___cycFILE*_tmpE27=Cyc_stderr;struct _dyneither_ptr _tmpE26=({const char*_tmp6B3="%c";_tag_dyneither(_tmp6B3,sizeof(char),3U);});Cyc_fprintf(_tmpE27,_tmpE26,_tag_dyneither(_tmp6B2,sizeof(void*),1U));});});goto _LL0;case 3U: _LL5: _tmp6C5=(_tmp6AE.String_tok).val;_LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp6B7=({struct Cyc_String_pa_PrintArg_struct _tmp75E;_tmp75E.tag=0U,_tmp75E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6C5);_tmp75E;});void*_tmp6B5[1U];_tmp6B5[0]=& _tmp6B7;({struct Cyc___cycFILE*_tmpE29=Cyc_stderr;struct _dyneither_ptr _tmpE28=({const char*_tmp6B6="\"%s\"";_tag_dyneither(_tmp6B6,sizeof(char),5U);});Cyc_fprintf(_tmpE29,_tmpE28,_tag_dyneither(_tmp6B5,sizeof(void*),1U));});});goto _LL0;case 5U: _LL7: _tmp6C6=(_tmp6AE.QualId_tok).val;_LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp6BA=({struct Cyc_String_pa_PrintArg_struct _tmp75F;_tmp75F.tag=0U,({struct _dyneither_ptr _tmpE2A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6C6));_tmp75F.f1=_tmpE2A;});_tmp75F;});void*_tmp6B8[1U];_tmp6B8[0]=& _tmp6BA;({struct Cyc___cycFILE*_tmpE2C=Cyc_stderr;struct _dyneither_ptr _tmpE2B=({const char*_tmp6B9="%s";_tag_dyneither(_tmp6B9,sizeof(char),3U);});Cyc_fprintf(_tmpE2C,_tmpE2B,_tag_dyneither(_tmp6B8,sizeof(void*),1U));});});goto _LL0;case 7U: _LL9: _tmp6C7=(_tmp6AE.Exp_tok).val;_LLA:
({struct Cyc_String_pa_PrintArg_struct _tmp6BD=({struct Cyc_String_pa_PrintArg_struct _tmp760;_tmp760.tag=0U,({struct _dyneither_ptr _tmpE2D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp6C7));_tmp760.f1=_tmpE2D;});_tmp760;});void*_tmp6BB[1U];_tmp6BB[0]=& _tmp6BD;({struct Cyc___cycFILE*_tmpE2F=Cyc_stderr;struct _dyneither_ptr _tmpE2E=({const char*_tmp6BC="%s";_tag_dyneither(_tmp6BC,sizeof(char),3U);});Cyc_fprintf(_tmpE2F,_tmpE2E,_tag_dyneither(_tmp6BB,sizeof(void*),1U));});});goto _LL0;case 8U: _LLB: _tmp6C8=(_tmp6AE.Stmt_tok).val;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp6C0=({struct Cyc_String_pa_PrintArg_struct _tmp761;_tmp761.tag=0U,({struct _dyneither_ptr _tmpE30=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp6C8));_tmp761.f1=_tmpE30;});_tmp761;});void*_tmp6BE[1U];_tmp6BE[0]=& _tmp6C0;({struct Cyc___cycFILE*_tmpE32=Cyc_stderr;struct _dyneither_ptr _tmpE31=({const char*_tmp6BF="%s";_tag_dyneither(_tmp6BF,sizeof(char),3U);});Cyc_fprintf(_tmpE32,_tmpE31,_tag_dyneither(_tmp6BE,sizeof(void*),1U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp6C1=0U;({struct Cyc___cycFILE*_tmpE34=Cyc_stderr;struct _dyneither_ptr _tmpE33=({const char*_tmp6C2="?";_tag_dyneither(_tmp6C2,sizeof(char),2U);});Cyc_fprintf(_tmpE34,_tmpE33,_tag_dyneither(_tmp6C1,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3182
struct _dyneither_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6C9="end-of-file";_tag_dyneither(_tmp6C9,sizeof(char),12U);});
# 3186
if(token == 362)
return Cyc_Lex_token_string;else{
if(token == 371)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0  && token <= 374?(int)Cyc_yytranslate[_check_known_subscript_notnull(375U,token)]: 294;
if((unsigned int)z < 294U)
return Cyc_yytname[z];else{
return _tag_dyneither(0,0,0);}};}
# 3197
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6CA=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6CA;_push_region(yyr);
({struct _RegionHandle*_tmpE35=yyr;Cyc_yyparse(_tmpE35,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6CB=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6CB;};
# 3200
;_pop_region(yyr);};}
