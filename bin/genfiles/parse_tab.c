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

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct
 _dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  unsigned delta = sz * numelts;
  /* Don't let someone decrease the size so much that it wraps around.
   * This is crucial to get NULL right. */
  if (x.last_plus_one - x.base >= delta)
    x.last_plus_one -= delta;
  else x.last_plus_one = x.base;
  return x; 
}

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
extern struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct Cyc_Absyn_Null_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};
# 877
extern struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct Cyc_Absyn_Porton_d_val;
extern struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct Cyc_Absyn_Portoff_d_val;struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 904
int Cyc_Absyn_is_qvar_qualified(struct _tuple0*);
# 908
struct Cyc_Absyn_Tqual Cyc_Absyn_combine_tqual(struct Cyc_Absyn_Tqual x,struct Cyc_Absyn_Tqual y);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 912
void*Cyc_Absyn_compress_kb(void*);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 925
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 928
extern void*Cyc_Absyn_char_type;extern void*Cyc_Absyn_uint_type;
# 930
extern void*Cyc_Absyn_sint_type;
# 932
extern void*Cyc_Absyn_float_type;extern void*Cyc_Absyn_double_type;extern void*Cyc_Absyn_long_double_type;
# 935
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 939
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_tag_type(void*);void*Cyc_Absyn_rgn_handle_type(void*);void*Cyc_Absyn_valueof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_typeof_type(struct Cyc_Absyn_Exp*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);void*Cyc_Absyn_anon_enum_type(struct Cyc_List_List*);void*Cyc_Absyn_builtin_type(struct _dyneither_ptr s,struct Cyc_Absyn_Kind*k);void*Cyc_Absyn_typedef_type(struct _tuple0*,struct Cyc_List_List*,struct Cyc_Absyn_Typedefdecl*,void*);
# 966
extern void*Cyc_Absyn_fat_bound_type;
void*Cyc_Absyn_thin_bounds_type(void*);
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 970
void*Cyc_Absyn_bounds_one();
# 972
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 998
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1001
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_datatype_field_type(union Cyc_Absyn_DatatypeFieldInfo,struct Cyc_List_List*args);
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
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
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
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
void*Cyc_Absyn_function_type(struct Cyc_List_List*tvs,void*eff_typ,struct Cyc_Absyn_Tqual ret_tqual,void*ret_type,struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,struct Cyc_List_List*rgn_po,struct Cyc_List_List*atts,struct Cyc_Absyn_Exp*requires_clause,struct Cyc_Absyn_Exp*ensures_clause);
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
({struct _tuple10*_tmp744=({struct _tuple10*_tmp2=_region_malloc(r,sizeof(*_tmp2));_tmp2->tl=declarators,_tmp2->hd=_tmp5;_tmp2;});declarators=_tmp744;});
({struct Cyc_List_List*_tmp745=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));_tmp3->hd=_tmp4,_tmp3->tl=exprs;_tmp3;});exprs=_tmp745;});}
# 177
({struct Cyc_List_List*_tmp746=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(exprs);*es=_tmp746;});
({struct _tuple10*_tmp747=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(declarators);*decls=_tmp747;});}struct Cyc_Parse_Abstractdeclarator{struct Cyc_List_List*tms;};
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
({void*_tmp8=0U;({unsigned int _tmp749=loc;struct _dyneither_ptr _tmp748=({const char*_tmp9="qualifier on type is ignored";_tag_dyneither(_tmp9,sizeof(char),29U);});Cyc_Warn_warn(_tmp749,_tmp748,_tag_dyneither(_tmp8,sizeof(void*),0U));});});}
# 209
return _tmpA;}struct _tuple14{void*f1;void*f2;void*f3;void*f4;};
# 212
static struct _tuple14 Cyc_Parse_collapse_pointer_quals(unsigned int loc,void*nullable,void*bound,void*rgn,struct Cyc_List_List*pqs){
# 218
void*zeroterm=Cyc_Tcutil_any_bool(0);
for(0;pqs != 0;pqs=pqs->tl){
void*_tmpC=(void*)pqs->hd;void*_tmpD=_tmpC;void*_tmpF;struct Cyc_Absyn_Exp*_tmpE;switch(*((int*)_tmpD)){case 4U: _LL1: _LL2:
 zeroterm=Cyc_Absyn_true_type;goto _LL0;case 5U: _LL3: _LL4:
 zeroterm=Cyc_Absyn_false_type;goto _LL0;case 7U: _LL5: _LL6:
 nullable=Cyc_Absyn_true_type;goto _LL0;case 6U: _LL7: _LL8:
 nullable=Cyc_Absyn_false_type;goto _LL0;case 3U: _LL9: _LLA:
 bound=Cyc_Absyn_fat_bound_type;goto _LL0;case 2U: _LLB: _LLC:
({void*_tmp74A=Cyc_Absyn_bounds_one();bound=_tmp74A;});goto _LL0;case 0U: _LLD: _tmpE=((struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LLE:
({void*_tmp74B=Cyc_Absyn_thin_bounds_exp(_tmpE);bound=_tmp74B;});goto _LL0;default: _LLF: _tmpF=(void*)((struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*)_tmpD)->f1;_LL10:
 rgn=_tmpF;goto _LL0;}_LL0:;}
# 230
return({struct _tuple14 _tmp6AC;_tmp6AC.f1=nullable,_tmp6AC.f2=bound,_tmp6AC.f3=zeroterm,_tmp6AC.f4=rgn;_tmp6AC;});}
# 236
struct _tuple0*Cyc_Parse_gensym_enum(){
# 238
static int enum_counter=0;
return({struct _tuple0*_tmp14=_cycalloc(sizeof(*_tmp14));({union Cyc_Absyn_Nmspace _tmp74F=Cyc_Absyn_Rel_n(0);_tmp14->f1=_tmp74F;}),({
struct _dyneither_ptr*_tmp74E=({struct _dyneither_ptr*_tmp13=_cycalloc(sizeof(*_tmp13));({struct _dyneither_ptr _tmp74D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp12=({struct Cyc_Int_pa_PrintArg_struct _tmp6AD;_tmp6AD.tag=1U,_tmp6AD.f1=(unsigned long)enum_counter ++;_tmp6AD;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _dyneither_ptr _tmp74C=({const char*_tmp11="__anonymous_enum_%d__";_tag_dyneither(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp74C,_tag_dyneither(_tmp10,sizeof(void*),1U));});});*_tmp13=_tmp74D;});_tmp13;});_tmp14->f2=_tmp74E;});_tmp14;});}struct _tuple15{unsigned int f1;struct _tuple0*f2;struct Cyc_Absyn_Tqual f3;void*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;};struct _tuple16{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct _tuple17{struct _tuple15*f1;struct _tuple16*f2;};
# 243
static struct Cyc_Absyn_Aggrfield*Cyc_Parse_make_aggr_field(unsigned int loc,struct _tuple17*field_info){
# 248
struct _tuple17*_tmp15=field_info;unsigned int _tmp22;struct _tuple0*_tmp21;struct Cyc_Absyn_Tqual _tmp20;void*_tmp1F;struct Cyc_List_List*_tmp1E;struct Cyc_List_List*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;_LL1: _tmp22=(_tmp15->f1)->f1;_tmp21=(_tmp15->f1)->f2;_tmp20=(_tmp15->f1)->f3;_tmp1F=(_tmp15->f1)->f4;_tmp1E=(_tmp15->f1)->f5;_tmp1D=(_tmp15->f1)->f6;_tmp1C=(_tmp15->f2)->f1;_tmp1B=(_tmp15->f2)->f2;_LL2:;
if(_tmp1E != 0)
({void*_tmp16=0U;({unsigned int _tmp751=loc;struct _dyneither_ptr _tmp750=({const char*_tmp17="bad type params in struct field";_tag_dyneither(_tmp17,sizeof(char),32U);});Cyc_Warn_err(_tmp751,_tmp750,_tag_dyneither(_tmp16,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp21))
({void*_tmp18=0U;({unsigned int _tmp753=loc;struct _dyneither_ptr _tmp752=({const char*_tmp19="struct or union field cannot be qualified with a namespace";_tag_dyneither(_tmp19,sizeof(char),59U);});Cyc_Warn_err(_tmp753,_tmp752,_tag_dyneither(_tmp18,sizeof(void*),0U));});});
return({struct Cyc_Absyn_Aggrfield*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->name=(*_tmp21).f2,_tmp1A->tq=_tmp20,_tmp1A->type=_tmp1F,_tmp1A->width=_tmp1C,_tmp1A->attributes=_tmp1D,_tmp1A->requires_clause=_tmp1B;_tmp1A;});}
# 258
static struct Cyc_Parse_Type_specifier Cyc_Parse_empty_spec(unsigned int loc){
return({struct Cyc_Parse_Type_specifier _tmp6AE;_tmp6AE.Signed_spec=0,_tmp6AE.Unsigned_spec=0,_tmp6AE.Short_spec=0,_tmp6AE.Long_spec=0,_tmp6AE.Long_Long_spec=0,_tmp6AE.Valid_type_spec=0,_tmp6AE.Type_spec=Cyc_Absyn_sint_type,_tmp6AE.loc=loc;_tmp6AE;});}
# 269
static struct Cyc_Parse_Type_specifier Cyc_Parse_type_spec(void*t,unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp23=Cyc_Parse_empty_spec(loc);
_tmp23.Type_spec=t;
_tmp23.Valid_type_spec=1;
return _tmp23;}
# 275
static struct Cyc_Parse_Type_specifier Cyc_Parse_signed_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp24=Cyc_Parse_empty_spec(loc);
_tmp24.Signed_spec=1;
return _tmp24;}
# 280
static struct Cyc_Parse_Type_specifier Cyc_Parse_unsigned_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp25=Cyc_Parse_empty_spec(loc);
_tmp25.Unsigned_spec=1;
return _tmp25;}
# 285
static struct Cyc_Parse_Type_specifier Cyc_Parse_short_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp26=Cyc_Parse_empty_spec(loc);
_tmp26.Short_spec=1;
return _tmp26;}
# 290
static struct Cyc_Parse_Type_specifier Cyc_Parse_long_spec(unsigned int loc){
struct Cyc_Parse_Type_specifier _tmp27=Cyc_Parse_empty_spec(loc);
_tmp27.Long_spec=1;
return _tmp27;}
# 297
static void*Cyc_Parse_array2ptr(void*t,int argposn){
# 299
return Cyc_Tcutil_is_array_type(t)?({
void*_tmp754=t;Cyc_Tcutil_promote_array(_tmp754,argposn?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0): Cyc_Absyn_heap_rgn_type,0);}): t;}struct _tuple18{struct _dyneither_ptr*f1;void*f2;};
# 312 "parse.y"
static struct Cyc_List_List*Cyc_Parse_get_arg_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp28=0;
for(0;x != 0;x=x->tl){
struct _tuple8*_tmp29=(struct _tuple8*)x->hd;struct _tuple8*_tmp2A=_tmp29;struct _dyneither_ptr _tmp3E;void**_tmp3D;struct _dyneither_ptr*_tmp3C;void*_tmp3B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL1: _tmp3C=_tmp2A->f1;_tmp3B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd;if(_tmp3C != 0){_LL2:
# 317
{void*_tmp2B=_tmp3B;void**_tmp32;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->tag == 1U){_LL8: _tmp32=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2;_LL9: {
# 321
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp31=_cycalloc(sizeof(*_tmp31));({struct _dyneither_ptr _tmp756=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30=({struct Cyc_String_pa_PrintArg_struct _tmp6AF;_tmp6AF.tag=0U,_tmp6AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3C);_tmp6AF;});void*_tmp2E[1U];_tmp2E[0]=& _tmp30;({struct _dyneither_ptr _tmp755=({const char*_tmp2F="`%s";_tag_dyneither(_tmp2F,sizeof(char),4U);});Cyc_aprintf(_tmp755,_tag_dyneither(_tmp2E,sizeof(void*),1U));});});*_tmp31=_tmp756;});_tmp31;});
({void*_tmp758=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->name=nm,_tmp2D->identity=- 1,({void*_tmp757=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=0U,_tmp2C->f1=& Cyc_Tcutil_ik;_tmp2C;});_tmp2D->kind=_tmp757;});_tmp2D;}));*_tmp32=_tmp758;});
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 326
({struct Cyc_List_List*_tmp75A=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));({struct _tuple18*_tmp759=({struct _tuple18*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->f1=_tmp3C,_tmp33->f2=_tmp3B;_tmp33;});_tmp34->hd=_tmp759;}),_tmp34->tl=_tmp28;_tmp34;});_tmp28=_tmp75A;});goto _LL0;}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2 != 0){if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->hd)->tag == 1U){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct _tuple8*)_tmp2A)->f3)->f2)->tl == 0){_LL3: _tmp3E=*_tmp2A->f1;_tmp3D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A->f3)->f2)->hd)->f2;_LL4: {
# 330
struct _dyneither_ptr*nm=({struct _dyneither_ptr*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct _dyneither_ptr _tmp75C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp6B0;_tmp6B0.tag=0U,_tmp6B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E);_tmp6B0;});void*_tmp37[1U];_tmp37[0]=& _tmp39;({struct _dyneither_ptr _tmp75B=({const char*_tmp38="`%s";_tag_dyneither(_tmp38,sizeof(char),4U);});Cyc_aprintf(_tmp75B,_tag_dyneither(_tmp37,sizeof(void*),1U));});});*_tmp3A=_tmp75C;});_tmp3A;});
({void*_tmp75E=Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->name=nm,_tmp36->identity=- 1,({void*_tmp75D=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35->tag=0U,_tmp35->f1=& Cyc_Tcutil_rk;_tmp35;});_tmp36->kind=_tmp75D;});_tmp36;}));*_tmp3D=_tmp75E;});
goto _LL0;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}else{goto _LL5;}}}else{if(((struct _tuple8*)_tmp2A)->f1 != 0)goto _LL5;else{_LL5: _LL6:
 goto _LL0;}}_LL0:;}
# 336
return _tmp28;}
# 340
static struct Cyc_List_List*Cyc_Parse_get_aggrfield_tags(struct Cyc_List_List*x){
struct Cyc_List_List*_tmp3F=0;
for(0;x != 0;x=x->tl){
void*_tmp40=((struct Cyc_Absyn_Aggrfield*)x->hd)->type;void*_tmp41=_tmp40;void*_tmp44;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->tl == 0){_LL1: _tmp44=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp41)->f2)->hd;_LL2:
# 345
({struct Cyc_List_List*_tmp760=({struct Cyc_List_List*_tmp43=_cycalloc(sizeof(*_tmp43));({struct _tuple18*_tmp75F=({struct _tuple18*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=((struct Cyc_Absyn_Aggrfield*)x->hd)->name,_tmp42->f2=_tmp44;_tmp42;});_tmp43->hd=_tmp75F;}),_tmp43->tl=_tmp3F;_tmp43;});_tmp3F=_tmp760;});goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 349
return _tmp3F;}
# 353
static struct Cyc_Absyn_Exp*Cyc_Parse_substitute_tags_exp(struct Cyc_List_List*tags,struct Cyc_Absyn_Exp*e){
{void*_tmp45=e->r;void*_tmp46=_tmp45;struct _dyneither_ptr*_tmp4D;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->tag == 1U){if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->tag == 0U){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).tag == 1){if(((((struct _tuple0*)((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f1).Rel_n).val == 0){_LL1: _tmp4D=(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp46)->f1)->f1)->f2;_LL2:
# 356
{struct Cyc_List_List*_tmp47=tags;for(0;_tmp47 != 0;_tmp47=_tmp47->tl){
struct _tuple18*_tmp48=(struct _tuple18*)_tmp47->hd;struct _tuple18*_tmp49=_tmp48;struct _dyneither_ptr*_tmp4C;void*_tmp4B;_LL6: _tmp4C=_tmp49->f1;_tmp4B=_tmp49->f2;_LL7:;
if(Cyc_strptrcmp(_tmp4C,_tmp4D)== 0)
return({void*_tmp762=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->tag=39U,({void*_tmp761=Cyc_Tcutil_copy_type(_tmp4B);_tmp4A->f1=_tmp761;});_tmp4A;});Cyc_Absyn_new_exp(_tmp762,e->loc);});}}
# 361
goto _LL0;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 364
return e;}
# 369
static void*Cyc_Parse_substitute_tags(struct Cyc_List_List*tags,void*t){
{void*_tmp4E=t;struct Cyc_Absyn_Exp*_tmp61;void*_tmp60;void*_tmp5F;struct Cyc_Absyn_Tqual _tmp5E;void*_tmp5D;void*_tmp5C;void*_tmp5B;void*_tmp5A;struct Cyc_Absyn_PtrLoc*_tmp59;void*_tmp58;struct Cyc_Absyn_Tqual _tmp57;struct Cyc_Absyn_Exp*_tmp56;void*_tmp55;unsigned int _tmp54;switch(*((int*)_tmp4E)){case 4U: _LL1: _tmp58=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp57=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).tq;_tmp56=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).num_elts;_tmp55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zero_term;_tmp54=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4E)->f1).zt_loc;_LL2: {
# 372
struct Cyc_Absyn_Exp*nelts2=_tmp56;
if(_tmp56 != 0)
({struct Cyc_Absyn_Exp*_tmp763=Cyc_Parse_substitute_tags_exp(tags,_tmp56);nelts2=_tmp763;});{
# 376
void*_tmp4F=Cyc_Parse_substitute_tags(tags,_tmp58);
if(_tmp56 != nelts2  || _tmp58 != _tmp4F)
return Cyc_Absyn_array_type(_tmp4F,_tmp57,nelts2,_tmp55,_tmp54);
goto _LL0;};}case 3U: _LL3: _tmp5F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_tmp5E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_tq;_tmp5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).rgn;_tmp5C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).nullable;_tmp5B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).bounds;_tmp5A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).zero_term;_tmp59=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).ptr_atts).ptrloc;_LL4: {
# 381
void*_tmp50=Cyc_Parse_substitute_tags(tags,_tmp5F);
void*_tmp51=Cyc_Parse_substitute_tags(tags,_tmp5B);
if(_tmp50 != _tmp5F  || _tmp51 != _tmp5B)
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6B1;_tmp6B1.elt_type=_tmp50,_tmp6B1.elt_tq=_tmp5E,(_tmp6B1.ptr_atts).rgn=_tmp5D,(_tmp6B1.ptr_atts).nullable=_tmp5C,(_tmp6B1.ptr_atts).bounds=_tmp51,(_tmp6B1.ptr_atts).zero_term=_tmp5A,(_tmp6B1.ptr_atts).ptrloc=_tmp59;_tmp6B1;}));
goto _LL0;}case 0U: if(((struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f1)->tag == 13U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->tl == 0){_LL5: _tmp60=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E)->f2)->hd;_LL6: {
# 387
void*_tmp52=Cyc_Parse_substitute_tags(tags,_tmp60);
if(_tmp60 != _tmp52)return Cyc_Absyn_thin_bounds_type(_tmp52);
goto _LL0;}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 9U: _LL7: _tmp61=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4E)->f1;_LL8: {
# 391
struct Cyc_Absyn_Exp*_tmp53=Cyc_Parse_substitute_tags_exp(tags,_tmp61);
if(_tmp53 != _tmp61)return Cyc_Absyn_valueof_type(_tmp53);
goto _LL0;}default: _LL9: _LLA:
# 397
 goto _LL0;}_LL0:;}
# 399
return t;}
# 404
static void Cyc_Parse_substitute_aggrfield_tags(struct Cyc_List_List*tags,struct Cyc_Absyn_Aggrfield*x){
({void*_tmp764=Cyc_Parse_substitute_tags(tags,x->type);x->type=_tmp764;});}struct _tuple19{struct Cyc_Absyn_Tqual f1;void*f2;};
# 411
static struct _tuple19*Cyc_Parse_get_tqual_typ(unsigned int loc,struct _tuple8*t){
# 413
return({struct _tuple19*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=(*t).f2,_tmp62->f2=(*t).f3;_tmp62;});}
# 416
static int Cyc_Parse_is_typeparam(void*tm){
void*_tmp63=tm;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp63)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 425
static void*Cyc_Parse_id2type(struct _dyneither_ptr s,void*k){
if(({struct _dyneither_ptr _tmp765=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp765,({const char*_tmp64="`H";_tag_dyneither(_tmp64,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_heap_rgn_type;else{
if(({struct _dyneither_ptr _tmp766=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp766,({const char*_tmp65="`U";_tag_dyneither(_tmp65,sizeof(char),3U);}));})== 0)
return Cyc_Absyn_unique_rgn_type;else{
if(({struct _dyneither_ptr _tmp767=(struct _dyneither_ptr)s;Cyc_zstrcmp(_tmp767,({const char*_tmp66="`RC";_tag_dyneither(_tmp66,sizeof(char),4U);}));})== 0)
return Cyc_Absyn_refcnt_rgn_type;else{
# 433
return Cyc_Absyn_var_type(({struct Cyc_Absyn_Tvar*_tmp68=_cycalloc(sizeof(*_tmp68));({struct _dyneither_ptr*_tmp768=({struct _dyneither_ptr*_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=s;_tmp67;});_tmp68->name=_tmp768;}),_tmp68->identity=- 1,_tmp68->kind=k;_tmp68;}));}}}}
# 440
static struct Cyc_Absyn_Tvar*Cyc_Parse_typ2tvar(unsigned int loc,void*t){
void*_tmp69=t;struct Cyc_Absyn_Tvar*_tmp6C;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->tag == 2U){_LL1: _tmp6C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp69)->f1;_LL2:
 return _tmp6C;}else{_LL3: _LL4:
({void*_tmp6A=0U;({unsigned int _tmp76A=loc;struct _dyneither_ptr _tmp769=({const char*_tmp6B="expecting a list of type variables, not types";_tag_dyneither(_tmp6B,sizeof(char),46U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp76A,_tmp769,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});}_LL0:;}
# 448
static void Cyc_Parse_set_vartyp_kind(void*t,struct Cyc_Absyn_Kind*k,int leq){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_tmp6E=_tmp6D;void**_tmp72;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->tag == 2U){_LL1: _tmp72=(void**)&(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1)->kind;_LL2: {
# 451
void*_tmp6F=Cyc_Absyn_compress_kb(*_tmp72);void*_tmp70=_tmp6F;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp70)->tag == 1U){_LL6: _LL7:
# 453
 if(!leq)({void*_tmp76B=Cyc_Tcutil_kind_to_bound(k);*_tmp72=_tmp76B;});else{
({void*_tmp76C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=2U,_tmp71->f1=0,_tmp71->f2=k;_tmp71;});*_tmp72=_tmp76C;});}
return;}else{_LL8: _LL9:
 return;}_LL5:;}}else{_LL3: _LL4:
# 458
 return;}_LL0:;}
# 463
static struct Cyc_List_List*Cyc_Parse_oldstyle2newstyle(struct _RegionHandle*yy,struct Cyc_List_List*tms,struct Cyc_List_List*tds,unsigned int loc){
# 469
if(tds == 0)return tms;
# 474
if(tms == 0)return 0;{
# 476
void*_tmp73=(void*)tms->hd;void*_tmp74=_tmp73;void*_tmp8F;if(((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->tag == 3U){_LL1: _tmp8F=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp74)->f1;_LL2:
# 479
 if(tms->tl == 0  || 
Cyc_Parse_is_typeparam((void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd) && ((struct Cyc_List_List*)_check_null(tms->tl))->tl == 0){
# 482
void*_tmp75=_tmp8F;struct Cyc_List_List*_tmp8D;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmp75)->tag == 1U){_LL6: _LL7:
# 484
({void*_tmp76=0U;({unsigned int _tmp76E=loc;struct _dyneither_ptr _tmp76D=({const char*_tmp77="function declaration with both new- and old-style parameter declarations; ignoring old-style";_tag_dyneither(_tmp77,sizeof(char),93U);});Cyc_Warn_warn(_tmp76E,_tmp76D,_tag_dyneither(_tmp76,sizeof(void*),0U));});});
# 486
return tms;}else{_LL8: _tmp8D=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmp75)->f1;_LL9:
# 488
 if(({int _tmp76F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);_tmp76F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tds);}))
({void*_tmp78=0U;({unsigned int _tmp771=loc;struct _dyneither_ptr _tmp770=({const char*_tmp79="wrong number of parameter declarations in old-style function declaration";_tag_dyneither(_tmp79,sizeof(char),73U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp771,_tmp770,_tag_dyneither(_tmp78,sizeof(void*),0U));});});{
# 492
struct Cyc_List_List*rev_new_params=0;
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
struct Cyc_List_List*_tmp7A=tds;
for(0;_tmp7A != 0;_tmp7A=_tmp7A->tl){
struct Cyc_Absyn_Decl*_tmp7B=(struct Cyc_Absyn_Decl*)_tmp7A->hd;
void*_tmp7C=_tmp7B->r;void*_tmp7D=_tmp7C;struct Cyc_Absyn_Vardecl*_tmp86;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->tag == 0U){_LLB: _tmp86=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7D)->f1;_LLC:
# 499
 if(Cyc_zstrptrcmp((*_tmp86->name).f2,(struct _dyneither_ptr*)_tmp8D->hd)!= 0)
continue;
if(_tmp86->initializer != 0)
({void*_tmp7E=0U;({unsigned int _tmp773=_tmp7B->loc;struct _dyneither_ptr _tmp772=({const char*_tmp7F="initializer found in parameter declaration";_tag_dyneither(_tmp7F,sizeof(char),43U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp773,_tmp772,_tag_dyneither(_tmp7E,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp86->name))
({void*_tmp80=0U;({unsigned int _tmp775=_tmp7B->loc;struct _dyneither_ptr _tmp774=({const char*_tmp81="namespaces forbidden in parameter declarations";_tag_dyneither(_tmp81,sizeof(char),47U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp775,_tmp774,_tag_dyneither(_tmp80,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp777=({struct Cyc_List_List*_tmp83=_cycalloc(sizeof(*_tmp83));
({struct _tuple8*_tmp776=({struct _tuple8*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=(*_tmp86->name).f2,_tmp82->f2=_tmp86->tq,_tmp82->f3=_tmp86->type;_tmp82;});_tmp83->hd=_tmp776;}),_tmp83->tl=rev_new_params;_tmp83;});
# 505
rev_new_params=_tmp777;});
# 508
goto L;}else{_LLD: _LLE:
({void*_tmp84=0U;({unsigned int _tmp779=_tmp7B->loc;struct _dyneither_ptr _tmp778=({const char*_tmp85="nonvariable declaration in parameter type";_tag_dyneither(_tmp85,sizeof(char),42U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp779,_tmp778,_tag_dyneither(_tmp84,sizeof(void*),0U));});});}_LLA:;}
# 512
L: if(_tmp7A == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp6B2;_tmp6B2.tag=0U,_tmp6B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp8D->hd));_tmp6B2;});void*_tmp87[1U];_tmp87[0]=& _tmp89;({unsigned int _tmp77B=loc;struct _dyneither_ptr _tmp77A=({const char*_tmp88="%s is not given a type";_tag_dyneither(_tmp88,sizeof(char),23U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp77B,_tmp77A,_tag_dyneither(_tmp87,sizeof(void*),1U));});});}
# 515
return({struct Cyc_List_List*_tmp8C=_region_malloc(yy,sizeof(*_tmp8C));
({void*_tmp77E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp8B=_region_malloc(yy,sizeof(*_tmp8B));_tmp8B->tag=3U,({void*_tmp77D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp8A=_region_malloc(yy,sizeof(*_tmp8A));_tmp8A->tag=1U,({struct Cyc_List_List*_tmp77C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(rev_new_params);_tmp8A->f1=_tmp77C;}),_tmp8A->f2=0,_tmp8A->f3=0,_tmp8A->f4=0,_tmp8A->f5=0,_tmp8A->f6=0,_tmp8A->f7=0;_tmp8A;});_tmp8B->f1=_tmp77D;});_tmp8B;});_tmp8C->hd=_tmp77E;}),_tmp8C->tl=0;_tmp8C;});};}_LL5:;}
# 522
goto _LL4;}else{_LL3: _LL4:
 return({struct Cyc_List_List*_tmp8E=_region_malloc(yy,sizeof(*_tmp8E));_tmp8E->hd=(void*)tms->hd,({struct Cyc_List_List*_tmp77F=Cyc_Parse_oldstyle2newstyle(yy,tms->tl,tds,loc);_tmp8E->tl=_tmp77F;});_tmp8E;});}_LL0:;};}
# 530
static struct Cyc_Absyn_Fndecl*Cyc_Parse_make_function(struct _RegionHandle*yy,struct Cyc_Parse_Declaration_spec*dso,struct Cyc_Parse_Declarator d,struct Cyc_List_List*tds,struct Cyc_Absyn_Stmt*body,unsigned int loc){
# 534
if(tds != 0)
({struct Cyc_Parse_Declarator _tmp781=({struct Cyc_Parse_Declarator _tmp6B3;_tmp6B3.id=d.id,_tmp6B3.varloc=d.varloc,({struct Cyc_List_List*_tmp780=Cyc_Parse_oldstyle2newstyle(yy,d.tms,tds,loc);_tmp6B3.tms=_tmp780;});_tmp6B3;});d=_tmp781;});{
# 537
enum Cyc_Absyn_Scope sc=2U;
struct Cyc_Parse_Type_specifier tss=Cyc_Parse_empty_spec(loc);
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
int is_inline=0;
struct Cyc_List_List*atts=0;
# 543
if(dso != 0){
tss=dso->type_specs;
tq=dso->tq;
is_inline=dso->is_inline;
atts=dso->attributes;
# 549
if(dso->sc != 0){
enum Cyc_Parse_Storage_class _tmp90=*((enum Cyc_Parse_Storage_class*)_check_null(dso->sc));enum Cyc_Parse_Storage_class _tmp91=_tmp90;switch(_tmp91){case Cyc_Parse_Extern_sc: _LL1: _LL2:
 sc=3U;goto _LL0;case Cyc_Parse_Static_sc: _LL3: _LL4:
 sc=0U;goto _LL0;default: _LL5: _LL6:
({void*_tmp92=0U;({unsigned int _tmp783=loc;struct _dyneither_ptr _tmp782=({const char*_tmp93="bad storage class on function";_tag_dyneither(_tmp93,sizeof(char),30U);});Cyc_Warn_err(_tmp783,_tmp782,_tag_dyneither(_tmp92,sizeof(void*),0U));});});goto _LL0;}_LL0:;}}{
# 556
void*_tmp94=Cyc_Parse_collapse_type_specifiers(tss,loc);
struct _tuple13 _tmp95=Cyc_Parse_apply_tms(tq,_tmp94,atts,d.tms);struct _tuple13 _tmp96=_tmp95;struct Cyc_Absyn_Tqual _tmpBC;void*_tmpBB;struct Cyc_List_List*_tmpBA;struct Cyc_List_List*_tmpB9;_LL8: _tmpBC=_tmp96.f1;_tmpBB=_tmp96.f2;_tmpBA=_tmp96.f3;_tmpB9=_tmp96.f4;_LL9:;
# 561
if(_tmpBA != 0)
# 564
({void*_tmp97=0U;({unsigned int _tmp785=loc;struct _dyneither_ptr _tmp784=({const char*_tmp98="bad type params, ignoring";_tag_dyneither(_tmp98,sizeof(char),26U);});Cyc_Warn_warn(_tmp785,_tmp784,_tag_dyneither(_tmp97,sizeof(void*),0U));});});{
# 566
void*_tmp99=_tmpBB;struct Cyc_List_List*_tmpB8;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;void*_tmpB5;struct Cyc_List_List*_tmpB4;int _tmpB3;struct Cyc_Absyn_VarargInfo*_tmpB2;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_List_List*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_List_List*_tmpAC;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->tag == 5U){_LLB: _tmpB8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).tvars;_tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).effect;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ret_tqual;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ret_type;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).args;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).c_varargs;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).cyc_varargs;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).rgn_po;_tmpB0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).attributes;_tmpAF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).requires_clause;_tmpAE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).requires_relns;_tmpAD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ensures_clause;_tmpAC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99)->f1).ensures_relns;_LLC: {
# 570
struct Cyc_List_List*_tmp9A=0;
{struct Cyc_List_List*_tmp9B=_tmpB4;for(0;_tmp9B != 0;_tmp9B=_tmp9B->tl){
struct _tuple8*_tmp9C=(struct _tuple8*)_tmp9B->hd;struct _tuple8*_tmp9D=_tmp9C;struct _dyneither_ptr*_tmpA8;struct Cyc_Absyn_Tqual _tmpA7;void*_tmpA6;_LL10: _tmpA8=_tmp9D->f1;_tmpA7=_tmp9D->f2;_tmpA6=_tmp9D->f3;_LL11:;
if(_tmpA8 == 0){
({void*_tmp9E=0U;({unsigned int _tmp787=loc;struct _dyneither_ptr _tmp786=({const char*_tmp9F="missing argument variable in function prototype";_tag_dyneither(_tmp9F,sizeof(char),48U);});Cyc_Warn_err(_tmp787,_tmp786,_tag_dyneither(_tmp9E,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp78B=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct _tuple8*_tmp78A=({struct _tuple8*_tmpA2=_cycalloc(sizeof(*_tmpA2));({struct _dyneither_ptr*_tmp789=({struct _dyneither_ptr*_tmpA1=_cycalloc(sizeof(*_tmpA1));({struct _dyneither_ptr _tmp788=({const char*_tmpA0="?";_tag_dyneither(_tmpA0,sizeof(char),2U);});*_tmpA1=_tmp788;});_tmpA1;});_tmpA2->f1=_tmp789;}),_tmpA2->f2=_tmpA7,_tmpA2->f3=_tmpA6;_tmpA2;});_tmpA3->hd=_tmp78A;}),_tmpA3->tl=_tmp9A;_tmpA3;});_tmp9A=_tmp78B;});}else{
# 577
({struct Cyc_List_List*_tmp78D=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));({struct _tuple8*_tmp78C=({struct _tuple8*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=_tmpA8,_tmpA4->f2=_tmpA7,_tmpA4->f3=_tmpA6;_tmpA4;});_tmpA5->hd=_tmp78C;}),_tmpA5->tl=_tmp9A;_tmpA5;});_tmp9A=_tmp78D;});}}}
# 581
return({struct Cyc_Absyn_Fndecl*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->sc=sc,_tmpA9->name=d.id,_tmpA9->tvs=_tmpB8,_tmpA9->is_inline=is_inline,_tmpA9->effect=_tmpB7,_tmpA9->ret_tqual=_tmpB6,_tmpA9->ret_type=_tmpB5,({
# 584
struct Cyc_List_List*_tmp78F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9A);_tmpA9->args=_tmp78F;}),_tmpA9->c_varargs=_tmpB3,_tmpA9->cyc_varargs=_tmpB2,_tmpA9->rgn_po=_tmpB1,_tmpA9->body=body,_tmpA9->cached_type=0,_tmpA9->param_vardecls=0,_tmpA9->fn_vardecl=0,({
# 591
struct Cyc_List_List*_tmp78E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB0,_tmpB9);_tmpA9->attributes=_tmp78E;}),_tmpA9->requires_clause=_tmpAF,_tmpA9->requires_relns=0,_tmpA9->ensures_clause=_tmpAD,_tmpA9->ensures_relns=0;_tmpA9;});}}else{_LLD: _LLE:
# 596
({void*_tmpAA=0U;({unsigned int _tmp791=loc;struct _dyneither_ptr _tmp790=({const char*_tmpAB="declarator is not a function prototype";_tag_dyneither(_tmpAB,sizeof(char),39U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp791,_tmp790,_tag_dyneither(_tmpAA,sizeof(void*),0U));});});}_LLA:;};};};}static char _tmpBD[76U]="at most one type may appear within a type specifier \n\t(missing ';' or ','?)";
# 600
static struct _dyneither_ptr Cyc_Parse_msg1={_tmpBD,_tmpBD,_tmpBD + 76U};static char _tmpBE[87U]="const or volatile may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 602
static struct _dyneither_ptr Cyc_Parse_msg2={_tmpBE,_tmpBE,_tmpBE + 87U};static char _tmpBF[74U]="type specifier includes more than one declaration \n\t(missing ';' or ','?)";
# 604
static struct _dyneither_ptr Cyc_Parse_msg3={_tmpBF,_tmpBF,_tmpBF + 74U};static char _tmpC0[84U]="sign specifier may appear only once within a type specifier \n\t(missing ';' or ','?)";
# 606
static struct _dyneither_ptr Cyc_Parse_msg4={_tmpC0,_tmpC0,_tmpC0 + 84U};
# 613
static struct Cyc_Parse_Type_specifier Cyc_Parse_combine_specifiers(unsigned int loc,struct Cyc_Parse_Type_specifier s1,struct Cyc_Parse_Type_specifier s2){
# 616
if(s1.Signed_spec  && s2.Signed_spec)
({void*_tmpC1=0U;({unsigned int _tmp793=loc;struct _dyneither_ptr _tmp792=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp793,_tmp792,_tag_dyneither(_tmpC1,sizeof(void*),0U));});});
s1.Signed_spec |=s2.Signed_spec;
if(s1.Unsigned_spec  && s2.Unsigned_spec)
({void*_tmpC2=0U;({unsigned int _tmp795=loc;struct _dyneither_ptr _tmp794=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp795,_tmp794,_tag_dyneither(_tmpC2,sizeof(void*),0U));});});
s1.Unsigned_spec |=s2.Unsigned_spec;
if(s1.Short_spec  && s2.Short_spec)
({void*_tmpC3=0U;({unsigned int _tmp797=loc;struct _dyneither_ptr _tmp796=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp797,_tmp796,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});
s1.Short_spec |=s2.Short_spec;
if((s1.Long_Long_spec  && s2.Long_Long_spec  || 
s1.Long_Long_spec  && s2.Long_spec) || 
s2.Long_Long_spec  && s1.Long_spec)
({void*_tmpC4=0U;({unsigned int _tmp799=loc;struct _dyneither_ptr _tmp798=Cyc_Parse_msg4;Cyc_Warn_warn(_tmp799,_tmp798,_tag_dyneither(_tmpC4,sizeof(void*),0U));});});
s1.Long_Long_spec=
(s1.Long_Long_spec  || s2.Long_Long_spec) || s1.Long_spec  && s2.Long_spec;
s1.Long_spec=!s1.Long_Long_spec  && (s1.Long_spec  || s2.Long_spec);
if(s1.Valid_type_spec  && s2.Valid_type_spec)
({void*_tmpC5=0U;({unsigned int _tmp79B=loc;struct _dyneither_ptr _tmp79A=Cyc_Parse_msg1;Cyc_Warn_err(_tmp79B,_tmp79A,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});else{
if(s2.Valid_type_spec){
s1.Type_spec=s2.Type_spec;
s1.Valid_type_spec=1;}}
# 638
return s1;}
# 644
static void*Cyc_Parse_collapse_type_specifiers(struct Cyc_Parse_Type_specifier ts,unsigned int loc){
# 647
int seen_type=ts.Valid_type_spec;
int seen_sign=ts.Signed_spec  || ts.Unsigned_spec;
int seen_size=(ts.Short_spec  || ts.Long_spec) || ts.Long_Long_spec;
void*t=seen_type?ts.Type_spec: Cyc_Absyn_void_type;
enum Cyc_Absyn_Size_of sz=2U;
enum Cyc_Absyn_Sign sgn=0U;
# 654
if(ts.Signed_spec  && ts.Unsigned_spec)
({void*_tmpC6=0U;({unsigned int _tmp79D=loc;struct _dyneither_ptr _tmp79C=Cyc_Parse_msg4;Cyc_Warn_err(_tmp79D,_tmp79C,_tag_dyneither(_tmpC6,sizeof(void*),0U));});});
if(ts.Unsigned_spec)sgn=1U;
if(ts.Short_spec  && (ts.Long_spec  || ts.Long_Long_spec) || 
ts.Long_spec  && ts.Long_Long_spec)
({void*_tmpC7=0U;({unsigned int _tmp79F=loc;struct _dyneither_ptr _tmp79E=Cyc_Parse_msg4;Cyc_Warn_err(_tmp79F,_tmp79E,_tag_dyneither(_tmpC7,sizeof(void*),0U));});});
if(ts.Short_spec)sz=1U;
if(ts.Long_spec)sz=3U;
if(ts.Long_Long_spec)sz=4U;
# 666
if(!seen_type){
if(!seen_sign  && !seen_size)
({void*_tmpC8=0U;({unsigned int _tmp7A1=loc;struct _dyneither_ptr _tmp7A0=({const char*_tmpC9="missing type within specifier";_tag_dyneither(_tmpC9,sizeof(char),30U);});Cyc_Warn_warn(_tmp7A1,_tmp7A0,_tag_dyneither(_tmpC8,sizeof(void*),0U));});});
({void*_tmp7A2=Cyc_Absyn_int_type(sgn,sz);t=_tmp7A2;});}else{
# 671
if(seen_sign){
void*_tmpCA=t;enum Cyc_Absyn_Sign _tmpCE;enum Cyc_Absyn_Size_of _tmpCD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->f1)->tag == 1U){_LL1: _tmpCE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->f1)->f1;_tmpCD=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCA)->f1)->f2;_LL2:
# 674
 if(_tmpCE != sgn)
({void*_tmp7A3=Cyc_Absyn_int_type(sgn,_tmpCD);t=_tmp7A3;});
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
({void*_tmpCB=0U;({unsigned int _tmp7A5=loc;struct _dyneither_ptr _tmp7A4=({const char*_tmpCC="sign specification on non-integral type";_tag_dyneither(_tmpCC,sizeof(char),40U);});Cyc_Warn_err(_tmp7A5,_tmp7A4,_tag_dyneither(_tmpCB,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 679
if(seen_size){
void*_tmpCF=t;enum Cyc_Absyn_Sign _tmpD3;enum Cyc_Absyn_Size_of _tmpD2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->f1)){case 1U: _LL6: _tmpD3=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->f1)->f1;_tmpD2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpCF)->f1)->f2;_LL7:
# 682
 if(_tmpD2 != sz)
({void*_tmp7A6=Cyc_Absyn_int_type(_tmpD3,sz);t=_tmp7A6;});
goto _LL5;case 2U: _LL8: _LL9:
# 686
 t=Cyc_Absyn_long_double_type;goto _LL5;default: goto _LLA;}else{_LLA: _LLB:
({void*_tmpD0=0U;({unsigned int _tmp7A8=loc;struct _dyneither_ptr _tmp7A7=({const char*_tmpD1="size qualifier on non-integral type";_tag_dyneither(_tmpD1,sizeof(char),36U);});Cyc_Warn_err(_tmp7A8,_tmp7A7,_tag_dyneither(_tmpD0,sizeof(void*),0U));});});goto _LL5;}_LL5:;}}
# 690
return t;}
# 693
static struct Cyc_List_List*Cyc_Parse_apply_tmss(struct _RegionHandle*r,struct Cyc_Absyn_Tqual tq,void*t,struct _tuple10*ds,struct Cyc_List_List*shared_atts){
# 697
if(ds == 0)return 0;{
struct Cyc_Parse_Declarator d=ds->hd;
struct _tuple0*_tmpD4=d.id;
unsigned int _tmpD5=d.varloc;
struct _tuple13 _tmpD6=Cyc_Parse_apply_tms(tq,t,shared_atts,d.tms);struct _tuple13 _tmpD7=_tmpD6;struct Cyc_Absyn_Tqual _tmpDF;void*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_List_List*_tmpDC;_LL1: _tmpDF=_tmpD7.f1;_tmpDE=_tmpD7.f2;_tmpDD=_tmpD7.f3;_tmpDC=_tmpD7.f4;_LL2:;
# 704
if(ds->tl == 0)
return({struct Cyc_List_List*_tmpD9=_region_malloc(r,sizeof(*_tmpD9));({struct _tuple15*_tmp7A9=({struct _tuple15*_tmpD8=_region_malloc(r,sizeof(*_tmpD8));_tmpD8->f1=_tmpD5,_tmpD8->f2=_tmpD4,_tmpD8->f3=_tmpDF,_tmpD8->f4=_tmpDE,_tmpD8->f5=_tmpDD,_tmpD8->f6=_tmpDC;_tmpD8;});_tmpD9->hd=_tmp7A9;}),_tmpD9->tl=0;_tmpD9;});else{
# 707
return({struct Cyc_List_List*_tmpDB=_region_malloc(r,sizeof(*_tmpDB));({struct _tuple15*_tmp7AF=({struct _tuple15*_tmpDA=_region_malloc(r,sizeof(*_tmpDA));_tmpDA->f1=_tmpD5,_tmpDA->f2=_tmpD4,_tmpDA->f3=_tmpDF,_tmpDA->f4=_tmpDE,_tmpDA->f5=_tmpDD,_tmpDA->f6=_tmpDC;_tmpDA;});_tmpDB->hd=_tmp7AF;}),({
struct Cyc_List_List*_tmp7AE=({struct _RegionHandle*_tmp7AD=r;struct Cyc_Absyn_Tqual _tmp7AC=tq;void*_tmp7AB=Cyc_Tcutil_copy_type(t);struct _tuple10*_tmp7AA=ds->tl;Cyc_Parse_apply_tmss(_tmp7AD,_tmp7AC,_tmp7AB,_tmp7AA,shared_atts);});_tmpDB->tl=_tmp7AE;});_tmpDB;});}};}
# 711
static struct _tuple13 Cyc_Parse_apply_tms(struct Cyc_Absyn_Tqual tq,void*t,struct Cyc_List_List*atts,struct Cyc_List_List*tms){
# 714
if(tms == 0)return({struct _tuple13 _tmp6B4;_tmp6B4.f1=tq,_tmp6B4.f2=t,_tmp6B4.f3=0,_tmp6B4.f4=atts;_tmp6B4;});{
void*_tmpE0=(void*)tms->hd;void*_tmpE1=_tmpE0;unsigned int _tmp106;struct Cyc_List_List*_tmp105;struct Cyc_Absyn_PtrAtts _tmp104;struct Cyc_Absyn_Tqual _tmp103;struct Cyc_List_List*_tmp102;unsigned int _tmp101;void*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;void*_tmpFE;unsigned int _tmpFD;void*_tmpFC;unsigned int _tmpFB;switch(*((int*)_tmpE1)){case 0U: _LL1: _tmpFC=(void*)((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmpFB=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LL2:
# 717
 return({struct Cyc_Absyn_Tqual _tmp7B2=Cyc_Absyn_empty_tqual(0U);void*_tmp7B1=
Cyc_Absyn_array_type(t,tq,0,_tmpFC,_tmpFB);
# 717
struct Cyc_List_List*_tmp7B0=atts;Cyc_Parse_apply_tms(_tmp7B2,_tmp7B1,_tmp7B0,tms->tl);});case 1U: _LL3: _tmpFF=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmpFE=(void*)((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_tmpFD=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmpE1)->f3;_LL4:
# 720
 return({struct Cyc_Absyn_Tqual _tmp7B5=Cyc_Absyn_empty_tqual(0U);void*_tmp7B4=
Cyc_Absyn_array_type(t,tq,_tmpFF,_tmpFE,_tmpFD);
# 720
struct Cyc_List_List*_tmp7B3=atts;Cyc_Parse_apply_tms(_tmp7B5,_tmp7B4,_tmp7B3,tms->tl);});case 3U: _LL5: _tmp100=(void*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_LL6: {
# 723
void*_tmpE2=_tmp100;unsigned int _tmpF8;struct Cyc_List_List*_tmpF7;int _tmpF6;struct Cyc_Absyn_VarargInfo*_tmpF5;void*_tmpF4;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->tag == 1U){_LLE: _tmpF7=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f1;_tmpF6=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f2;_tmpF5=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f3;_tmpF4=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f4;_tmpF3=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f5;_tmpF2=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f6;_tmpF1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)_tmpE2)->f7;_LLF: {
# 725
struct Cyc_List_List*typvars=0;
# 727
struct Cyc_List_List*fn_atts=0;struct Cyc_List_List*new_atts=0;
{struct Cyc_List_List*as=atts;for(0;as != 0;as=as->tl){
if(Cyc_Absyn_fntype_att((void*)as->hd))
({struct Cyc_List_List*_tmp7B6=({struct Cyc_List_List*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->hd=(void*)as->hd,_tmpE3->tl=fn_atts;_tmpE3;});fn_atts=_tmp7B6;});else{
# 732
({struct Cyc_List_List*_tmp7B7=({struct Cyc_List_List*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->hd=(void*)as->hd,_tmpE4->tl=new_atts;_tmpE4;});new_atts=_tmp7B7;});}}}
# 735
if(tms->tl != 0){
void*_tmpE5=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmpE6=_tmpE5;struct Cyc_List_List*_tmpE7;if(((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE6)->tag == 4U){_LL13: _tmpE7=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE6)->f1;_LL14:
# 738
 typvars=_tmpE7;
tms=tms->tl;
goto _LL12;}else{_LL15: _LL16:
# 742
 goto _LL12;}_LL12:;}
# 746
if(((((!_tmpF6  && _tmpF5 == 0) && _tmpF7 != 0) && _tmpF7->tl == 0) && (*((struct _tuple8*)_tmpF7->hd)).f1 == 0) && (*((struct _tuple8*)_tmpF7->hd)).f3 == Cyc_Absyn_void_type)
# 751
_tmpF7=0;{
# 754
struct Cyc_List_List*_tmpE8=Cyc_Parse_get_arg_tags(_tmpF7);
# 756
if(_tmpE8 != 0)
({void*_tmp7B8=Cyc_Parse_substitute_tags(_tmpE8,t);t=_tmp7B8;});
({void*_tmp7B9=Cyc_Parse_array2ptr(t,0);t=_tmp7B9;});
# 761
{struct Cyc_List_List*_tmpE9=_tmpF7;for(0;_tmpE9 != 0;_tmpE9=_tmpE9->tl){
struct _tuple8*_tmpEA=(struct _tuple8*)_tmpE9->hd;struct _tuple8*_tmpEB=_tmpEA;struct _dyneither_ptr*_tmpEE;struct Cyc_Absyn_Tqual _tmpED;void**_tmpEC;_LL18: _tmpEE=_tmpEB->f1;_tmpED=_tmpEB->f2;_tmpEC=(void**)& _tmpEB->f3;_LL19:;
if(_tmpE8 != 0)
({void*_tmp7BA=Cyc_Parse_substitute_tags(_tmpE8,*_tmpEC);*_tmpEC=_tmp7BA;});
({void*_tmp7BB=Cyc_Parse_array2ptr(*_tmpEC,1);*_tmpEC=_tmp7BB;});}}
# 773
return({struct Cyc_Absyn_Tqual _tmp7BE=Cyc_Absyn_empty_tqual(tq.loc);void*_tmp7BD=
Cyc_Absyn_function_type(typvars,_tmpF4,tq,t,_tmpF7,_tmpF6,_tmpF5,_tmpF3,fn_atts,_tmpF2,_tmpF1);
# 773
struct Cyc_List_List*_tmp7BC=new_atts;Cyc_Parse_apply_tms(_tmp7BE,_tmp7BD,_tmp7BC,((struct Cyc_List_List*)_check_null(tms))->tl);});};}}else{_LL10: _tmpF8=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)_tmpE2)->f2;_LL11:
# 780
({void*_tmpEF=0U;({unsigned int _tmp7C0=_tmpF8;struct _dyneither_ptr _tmp7BF=({const char*_tmpF0="function declaration without parameter types";_tag_dyneither(_tmpF0,sizeof(char),45U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7C0,_tmp7BF,_tag_dyneither(_tmpEF,sizeof(void*),0U));});});}_LLD:;}case 4U: _LL7: _tmp102=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmp101=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LL8:
# 787
 if(tms->tl == 0)
return({struct _tuple13 _tmp6B5;_tmp6B5.f1=tq,_tmp6B5.f2=t,_tmp6B5.f3=_tmp102,_tmp6B5.f4=atts;_tmp6B5;});
# 792
({void*_tmpF9=0U;({unsigned int _tmp7C2=_tmp101;struct _dyneither_ptr _tmp7C1=({const char*_tmpFA="type parameters must appear before function arguments in declarator";_tag_dyneither(_tmpFA,sizeof(char),68U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7C2,_tmp7C1,_tag_dyneither(_tmpF9,sizeof(void*),0U));});});case 2U: _LL9: _tmp104=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmp103=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LLA:
# 795
 return({struct Cyc_Absyn_Tqual _tmp7C5=_tmp103;void*_tmp7C4=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp6B6;_tmp6B6.elt_type=t,_tmp6B6.elt_tq=tq,_tmp6B6.ptr_atts=_tmp104;_tmp6B6;}));struct Cyc_List_List*_tmp7C3=atts;Cyc_Parse_apply_tms(_tmp7C5,_tmp7C4,_tmp7C3,tms->tl);});default: _LLB: _tmp106=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE1)->f1;_tmp105=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmpE1)->f2;_LLC:
# 800
 return({struct Cyc_Absyn_Tqual _tmp7C8=tq;void*_tmp7C7=t;struct Cyc_List_List*_tmp7C6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(atts,_tmp105);Cyc_Parse_apply_tms(_tmp7C8,_tmp7C7,_tmp7C6,tms->tl);});}_LL0:;};}
# 806
void*Cyc_Parse_speclist2typ(struct Cyc_Parse_Type_specifier tss,unsigned int loc){
return Cyc_Parse_collapse_type_specifiers(tss,loc);}
# 815
static struct Cyc_Absyn_Decl*Cyc_Parse_v_typ_to_typedef(unsigned int loc,struct _tuple15*t){
struct _tuple15*_tmp107=t;unsigned int _tmp111;struct _tuple0*_tmp110;struct Cyc_Absyn_Tqual _tmp10F;void*_tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_List_List*_tmp10C;_LL1: _tmp111=_tmp107->f1;_tmp110=_tmp107->f2;_tmp10F=_tmp107->f3;_tmp10E=_tmp107->f4;_tmp10D=_tmp107->f5;_tmp10C=_tmp107->f6;_LL2:;
# 818
Cyc_Lex_register_typedef(_tmp110);{
# 820
struct Cyc_Core_Opt*kind;
void*type;
{void*_tmp108=_tmp10E;struct Cyc_Core_Opt*_tmp109;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp108)->tag == 1U){_LL4: _tmp109=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp108)->f1;_LL5:
# 824
 type=0;
if(_tmp109 == 0)kind=& Cyc_Tcutil_bko;else{
kind=_tmp109;}
goto _LL3;}else{_LL6: _LL7:
 kind=0;type=_tmp10E;goto _LL3;}_LL3:;}
# 830
return({void*_tmp7CA=(void*)({struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->tag=8U,({struct Cyc_Absyn_Typedefdecl*_tmp7C9=({struct Cyc_Absyn_Typedefdecl*_tmp10A=_cycalloc(sizeof(*_tmp10A));_tmp10A->name=_tmp110,_tmp10A->tvs=_tmp10D,_tmp10A->kind=kind,_tmp10A->defn=type,_tmp10A->atts=_tmp10C,_tmp10A->tq=_tmp10F,_tmp10A->extern_c=0;_tmp10A;});_tmp10B->f1=_tmp7C9;});_tmp10B;});Cyc_Absyn_new_decl(_tmp7CA,loc);});};}
# 837
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s){
return({void*_tmp7CB=(void*)({struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=12U,_tmp112->f1=d,_tmp112->f2=s;_tmp112;});Cyc_Absyn_new_stmt(_tmp7CB,d->loc);});}
# 843
static struct Cyc_Absyn_Stmt*Cyc_Parse_flatten_declarations(struct Cyc_List_List*ds,struct Cyc_Absyn_Stmt*s){
return((struct Cyc_Absyn_Stmt*(*)(struct Cyc_Absyn_Stmt*(*f)(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*),struct Cyc_List_List*x,struct Cyc_Absyn_Stmt*accum))Cyc_List_fold_right)(Cyc_Parse_flatten_decl,ds,s);}
# 852
static struct Cyc_List_List*Cyc_Parse_make_declarations(struct Cyc_Parse_Declaration_spec ds,struct _tuple12*ids,unsigned int tqual_loc,unsigned int loc){
# 856
struct _RegionHandle _tmp113=_new_region("mkrgn");struct _RegionHandle*mkrgn=& _tmp113;_push_region(mkrgn);
{struct Cyc_Parse_Declaration_spec _tmp114=ds;struct Cyc_Absyn_Tqual _tmp175;struct Cyc_Parse_Type_specifier _tmp174;struct Cyc_List_List*_tmp173;_LL1: _tmp175=_tmp114.tq;_tmp174=_tmp114.type_specs;_tmp173=_tmp114.attributes;_LL2:;
if(_tmp175.loc == 0)_tmp175.loc=tqual_loc;
if(ds.is_inline)
({void*_tmp115=0U;({unsigned int _tmp7CD=loc;struct _dyneither_ptr _tmp7CC=({const char*_tmp116="inline qualifier on non-function definition";_tag_dyneither(_tmp116,sizeof(char),44U);});Cyc_Warn_warn(_tmp7CD,_tmp7CC,_tag_dyneither(_tmp115,sizeof(void*),0U));});});{
# 862
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
# 878
struct _tuple10*declarators=0;
struct Cyc_List_List*exprs=0;
Cyc_Parse_decl_split(mkrgn,ids,& declarators,& exprs);{
# 882
int exps_empty=1;
{struct Cyc_List_List*es=exprs;for(0;es != 0;es=es->tl){
if((struct Cyc_Absyn_Exp*)es->hd != 0){
exps_empty=0;
break;}}}{
# 890
void*_tmp119=Cyc_Parse_collapse_type_specifiers(_tmp174,loc);
if(declarators == 0){
# 894
void*_tmp11A=_tmp119;struct Cyc_List_List*_tmp15A;struct _tuple0*_tmp159;struct _tuple0*_tmp158;int _tmp157;struct Cyc_List_List*_tmp156;struct Cyc_Absyn_Datatypedecl**_tmp155;enum Cyc_Absyn_AggrKind _tmp154;struct _tuple0*_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Datatypedecl*_tmp151;struct Cyc_Absyn_Enumdecl*_tmp150;struct Cyc_Absyn_Aggrdecl*_tmp14F;switch(*((int*)_tmp11A)){case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)){case 0U: _LL13: _tmp14F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)->f1;_LL14:
# 896
({struct Cyc_List_List*_tmp7CE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp14F->attributes,_tmp173);_tmp14F->attributes=_tmp7CE;});
_tmp14F->sc=s;{
struct Cyc_List_List*_tmp11D=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct Cyc_Absyn_Decl*_tmp7D0=({void*_tmp7CF=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=5U,_tmp11B->f1=_tmp14F;_tmp11B;});Cyc_Absyn_new_decl(_tmp7CF,loc);});_tmp11C->hd=_tmp7D0;}),_tmp11C->tl=0;_tmp11C;});_npop_handler(0U);return _tmp11D;};case 1U: _LL15: _tmp150=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)->f1;_LL16:
# 900
 if(_tmp173 != 0)({void*_tmp11E=0U;({unsigned int _tmp7D2=loc;struct _dyneither_ptr _tmp7D1=({const char*_tmp11F="attributes on enum not supported";_tag_dyneither(_tmp11F,sizeof(char),33U);});Cyc_Warn_err(_tmp7D2,_tmp7D1,_tag_dyneither(_tmp11E,sizeof(void*),0U));});});
_tmp150->sc=s;{
struct Cyc_List_List*_tmp122=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));({struct Cyc_Absyn_Decl*_tmp7D4=({void*_tmp7D3=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->tag=7U,_tmp120->f1=_tmp150;_tmp120;});Cyc_Absyn_new_decl(_tmp7D3,loc);});_tmp121->hd=_tmp7D4;}),_tmp121->tl=0;_tmp121;});_npop_handler(0U);return _tmp122;};default: _LL17: _tmp151=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp11A)->f1)->r)->f1;_LL18:
# 904
 if(_tmp173 != 0)({void*_tmp123=0U;({unsigned int _tmp7D6=loc;struct _dyneither_ptr _tmp7D5=({const char*_tmp124="attributes on datatypes not supported";_tag_dyneither(_tmp124,sizeof(char),38U);});Cyc_Warn_err(_tmp7D6,_tmp7D5,_tag_dyneither(_tmp123,sizeof(void*),0U));});});
_tmp151->sc=s;{
struct Cyc_List_List*_tmp127=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));({struct Cyc_Absyn_Decl*_tmp7D8=({void*_tmp7D7=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=6U,_tmp125->f1=_tmp151;_tmp125;});Cyc_Absyn_new_decl(_tmp7D7,loc);});_tmp126->hd=_tmp7D8;}),_tmp126->tl=0;_tmp126;});_npop_handler(0U);return _tmp127;};}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownAggr).tag == 1){_LL19: _tmp154=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownAggr).val).f1;_tmp153=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownAggr).val).f2;_tmp152=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f2;_LL1A: {
# 908
struct Cyc_List_List*_tmp128=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp152);
struct Cyc_Absyn_Aggrdecl*_tmp129=({struct Cyc_Absyn_Aggrdecl*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->kind=_tmp154,_tmp12F->sc=s,_tmp12F->name=_tmp153,_tmp12F->tvs=_tmp128,_tmp12F->impl=0,_tmp12F->attributes=0,_tmp12F->expected_mem_kind=0;_tmp12F;});
if(_tmp173 != 0)({void*_tmp12A=0U;({unsigned int _tmp7DA=loc;struct _dyneither_ptr _tmp7D9=({const char*_tmp12B="bad attributes on type declaration";_tag_dyneither(_tmp12B,sizeof(char),35U);});Cyc_Warn_err(_tmp7DA,_tmp7D9,_tag_dyneither(_tmp12A,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp12E=({struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(*_tmp12D));({struct Cyc_Absyn_Decl*_tmp7DC=({void*_tmp7DB=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=5U,_tmp12C->f1=_tmp129;_tmp12C;});Cyc_Absyn_new_decl(_tmp7DB,loc);});_tmp12D->hd=_tmp7DC;}),_tmp12D->tl=0;_tmp12D;});_npop_handler(0U);return _tmp12E;};}}else{goto _LL23;}case 18U: if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).KnownDatatype).tag == 2){_LL1B: _tmp155=((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).KnownDatatype).val;_LL1C:
# 913
 if(_tmp173 != 0)({void*_tmp130=0U;({unsigned int _tmp7DE=loc;struct _dyneither_ptr _tmp7DD=({const char*_tmp131="bad attributes on datatype";_tag_dyneither(_tmp131,sizeof(char),27U);});Cyc_Warn_err(_tmp7DE,_tmp7DD,_tag_dyneither(_tmp130,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp134=({struct Cyc_List_List*_tmp133=_cycalloc(sizeof(*_tmp133));({struct Cyc_Absyn_Decl*_tmp7E0=({void*_tmp7DF=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=6U,_tmp132->f1=*_tmp155;_tmp132;});Cyc_Absyn_new_decl(_tmp7DF,loc);});_tmp133->hd=_tmp7E0;}),_tmp133->tl=0;_tmp133;});_npop_handler(0U);return _tmp134;};}else{_LL1D: _tmp158=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownDatatype).val).name;_tmp157=(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1).UnknownDatatype).val).is_extensible;_tmp156=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f2;_LL1E: {
# 916
struct Cyc_List_List*_tmp135=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,loc,_tmp156);
struct Cyc_Absyn_Decl*_tmp136=Cyc_Absyn_datatype_decl(s,_tmp158,_tmp135,0,_tmp157,loc);
if(_tmp173 != 0)({void*_tmp137=0U;({unsigned int _tmp7E2=loc;struct _dyneither_ptr _tmp7E1=({const char*_tmp138="bad attributes on datatype";_tag_dyneither(_tmp138,sizeof(char),27U);});Cyc_Warn_err(_tmp7E2,_tmp7E1,_tag_dyneither(_tmp137,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->hd=_tmp136,_tmp139->tl=0;_tmp139;});_npop_handler(0U);return _tmp13A;};}}case 15U: _LL1F: _tmp159=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1;_LL20: {
# 921
struct Cyc_Absyn_Enumdecl*_tmp13B=({struct Cyc_Absyn_Enumdecl*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->sc=s,_tmp142->name=_tmp159,_tmp142->fields=0;_tmp142;});
if(_tmp173 != 0)({void*_tmp13C=0U;({unsigned int _tmp7E4=loc;struct _dyneither_ptr _tmp7E3=({const char*_tmp13D="bad attributes on enum";_tag_dyneither(_tmp13D,sizeof(char),23U);});Cyc_Warn_err(_tmp7E4,_tmp7E3,_tag_dyneither(_tmp13C,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp141=({struct Cyc_List_List*_tmp140=_cycalloc(sizeof(*_tmp140));({struct Cyc_Absyn_Decl*_tmp7E6=({struct Cyc_Absyn_Decl*_tmp13F=_cycalloc(sizeof(*_tmp13F));({void*_tmp7E5=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->tag=7U,_tmp13E->f1=_tmp13B;_tmp13E;});_tmp13F->r=_tmp7E5;}),_tmp13F->loc=loc;_tmp13F;});_tmp140->hd=_tmp7E6;}),_tmp140->tl=0;_tmp140;});_npop_handler(0U);return _tmp141;};}case 16U: _LL21: _tmp15A=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp11A)->f1)->f1;_LL22: {
# 927
struct Cyc_Absyn_Enumdecl*_tmp143=({struct Cyc_Absyn_Enumdecl*_tmp14B=_cycalloc(sizeof(*_tmp14B));_tmp14B->sc=s,({struct _tuple0*_tmp7E8=Cyc_Parse_gensym_enum();_tmp14B->name=_tmp7E8;}),({struct Cyc_Core_Opt*_tmp7E7=({struct Cyc_Core_Opt*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->v=_tmp15A;_tmp14A;});_tmp14B->fields=_tmp7E7;});_tmp14B;});
if(_tmp173 != 0)({void*_tmp144=0U;({unsigned int _tmp7EA=loc;struct _dyneither_ptr _tmp7E9=({const char*_tmp145="bad attributes on enum";_tag_dyneither(_tmp145,sizeof(char),23U);});Cyc_Warn_err(_tmp7EA,_tmp7E9,_tag_dyneither(_tmp144,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp148=_cycalloc(sizeof(*_tmp148));({struct Cyc_Absyn_Decl*_tmp7EC=({struct Cyc_Absyn_Decl*_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp7EB=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->tag=7U,_tmp146->f1=_tmp143;_tmp146;});_tmp147->r=_tmp7EB;}),_tmp147->loc=loc;_tmp147;});_tmp148->hd=_tmp7EC;}),_tmp148->tl=0;_tmp148;});_npop_handler(0U);return _tmp149;};}default: goto _LL23;}default: _LL23: _LL24:
({void*_tmp14C=0U;({unsigned int _tmp7EE=loc;struct _dyneither_ptr _tmp7ED=({const char*_tmp14D="missing declarator";_tag_dyneither(_tmp14D,sizeof(char),19U);});Cyc_Warn_err(_tmp7EE,_tmp7ED,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});{struct Cyc_List_List*_tmp14E=0;_npop_handler(0U);return _tmp14E;};}_LL12:;}else{
# 934
struct Cyc_List_List*_tmp15B=Cyc_Parse_apply_tmss(mkrgn,_tmp175,_tmp119,declarators,_tmp173);
if(istypedef){
# 939
if(!exps_empty)
({void*_tmp15C=0U;({unsigned int _tmp7F0=loc;struct _dyneither_ptr _tmp7EF=({const char*_tmp15D="initializer in typedef declaration";_tag_dyneither(_tmp15D,sizeof(char),35U);});Cyc_Warn_err(_tmp7F0,_tmp7EF,_tag_dyneither(_tmp15C,sizeof(void*),0U));});});{
struct Cyc_List_List*decls=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Decl*(*f)(unsigned int,struct _tuple15*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_v_typ_to_typedef,loc,_tmp15B);
struct Cyc_List_List*_tmp15E=decls;_npop_handler(0U);return _tmp15E;};}else{
# 945
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp15F=_tmp15B;for(0;_tmp15F != 0;(_tmp15F=_tmp15F->tl,exprs=((struct Cyc_List_List*)_check_null(exprs))->tl)){
struct _tuple15*_tmp160=(struct _tuple15*)_tmp15F->hd;struct _tuple15*_tmp161=_tmp160;unsigned int _tmp171;struct _tuple0*_tmp170;struct Cyc_Absyn_Tqual _tmp16F;void*_tmp16E;struct Cyc_List_List*_tmp16D;struct Cyc_List_List*_tmp16C;_LL26: _tmp171=_tmp161->f1;_tmp170=_tmp161->f2;_tmp16F=_tmp161->f3;_tmp16E=_tmp161->f4;_tmp16D=_tmp161->f5;_tmp16C=_tmp161->f6;_LL27:;
if(_tmp16D != 0)
({void*_tmp162=0U;({unsigned int _tmp7F2=loc;struct _dyneither_ptr _tmp7F1=({const char*_tmp163="bad type params, ignoring";_tag_dyneither(_tmp163,sizeof(char),26U);});Cyc_Warn_warn(_tmp7F2,_tmp7F1,_tag_dyneither(_tmp162,sizeof(void*),0U));});});
if(exprs == 0)
({void*_tmp164=0U;({unsigned int _tmp7F4=loc;struct _dyneither_ptr _tmp7F3=({const char*_tmp165="unexpected NULL in parse!";_tag_dyneither(_tmp165,sizeof(char),26U);});((int(*)(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Parse_parse_abort)(_tmp7F4,_tmp7F3,_tag_dyneither(_tmp164,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp166=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(exprs))->hd;
struct Cyc_Absyn_Vardecl*_tmp167=Cyc_Absyn_new_vardecl(_tmp171,_tmp170,_tmp16E,_tmp166);
_tmp167->tq=_tmp16F;
_tmp167->sc=s;
_tmp167->attributes=_tmp16C;{
struct Cyc_Absyn_Decl*_tmp168=({struct Cyc_Absyn_Decl*_tmp16B=_cycalloc(sizeof(*_tmp16B));({void*_tmp7F5=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=0U,_tmp16A->f1=_tmp167;_tmp16A;});_tmp16B->r=_tmp7F5;}),_tmp16B->loc=loc;_tmp16B;});
({struct Cyc_List_List*_tmp7F6=({struct Cyc_List_List*_tmp169=_cycalloc(sizeof(*_tmp169));_tmp169->hd=_tmp168,_tmp169->tl=decls;_tmp169;});decls=_tmp7F6;});};};}}{
# 960
struct Cyc_List_List*_tmp172=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(decls);_npop_handler(0U);return _tmp172;};}}};};};};}
# 857
;_pop_region(mkrgn);}
# 966
static struct Cyc_Absyn_Kind*Cyc_Parse_id_to_kind(struct _dyneither_ptr s,unsigned int loc){
if(Cyc_strlen((struct _dyneither_ptr)s)== 1  || Cyc_strlen((struct _dyneither_ptr)s)== 2){
char _tmp176=*((const char*)_check_dyneither_subscript(s,sizeof(char),0));char _tmp177=_tmp176;switch(_tmp177){case 65U: _LL1: _LL2:
 return& Cyc_Tcutil_ak;case 77U: _LL3: _LL4:
 return& Cyc_Tcutil_mk;case 66U: _LL5: _LL6:
 return& Cyc_Tcutil_bk;case 82U: _LL7: _LL8:
 return& Cyc_Tcutil_rk;case 69U: _LL9: _LLA:
 return& Cyc_Tcutil_ek;case 73U: _LLB: _LLC:
 return& Cyc_Tcutil_ik;case 85U: _LLD: _LLE:
# 976
{char _tmp178=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp179=_tmp178;switch(_tmp179){case 82U: _LL14: _LL15:
 return& Cyc_Tcutil_urk;case 65U: _LL16: _LL17:
 return& Cyc_Tcutil_uak;case 77U: _LL18: _LL19:
 return& Cyc_Tcutil_umk;case 66U: _LL1A: _LL1B:
 return& Cyc_Tcutil_ubk;default: _LL1C: _LL1D:
 goto _LL13;}_LL13:;}
# 983
goto _LL0;case 84U: _LLF: _LL10:
# 985
{char _tmp17A=*((const char*)_check_dyneither_subscript(s,sizeof(char),1));char _tmp17B=_tmp17A;switch(_tmp17B){case 82U: _LL1F: _LL20:
 return& Cyc_Tcutil_trk;case 65U: _LL21: _LL22:
 return& Cyc_Tcutil_tak;case 77U: _LL23: _LL24:
 return& Cyc_Tcutil_tmk;case 66U: _LL25: _LL26:
 return& Cyc_Tcutil_tbk;default: _LL27: _LL28:
 goto _LL1E;}_LL1E:;}
# 992
goto _LL0;default: _LL11: _LL12:
 goto _LL0;}_LL0:;}
# 995
({struct Cyc_String_pa_PrintArg_struct _tmp17E=({struct Cyc_String_pa_PrintArg_struct _tmp6B8;_tmp6B8.tag=0U,_tmp6B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp6B8;});struct Cyc_Int_pa_PrintArg_struct _tmp17F=({struct Cyc_Int_pa_PrintArg_struct _tmp6B7;_tmp6B7.tag=1U,({unsigned long _tmp7F7=(unsigned long)((int)Cyc_strlen((struct _dyneither_ptr)s));_tmp6B7.f1=_tmp7F7;});_tmp6B7;});void*_tmp17C[2U];_tmp17C[0]=& _tmp17E,_tmp17C[1]=& _tmp17F;({unsigned int _tmp7F9=loc;struct _dyneither_ptr _tmp7F8=({const char*_tmp17D="bad kind: %s; strlen=%d";_tag_dyneither(_tmp17D,sizeof(char),24U);});Cyc_Warn_err(_tmp7F9,_tmp7F8,_tag_dyneither(_tmp17C,sizeof(void*),2U));});});
return& Cyc_Tcutil_bk;}
# 1000
static int Cyc_Parse_exp2int(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp180=e->r;void*_tmp181=_tmp180;int _tmp184;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181)->f1).Int_c).tag == 5){_LL1: _tmp184=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp181)->f1).Int_c).val).f2;_LL2:
 return _tmp184;}else{goto _LL3;}}else{_LL3: _LL4:
# 1004
({void*_tmp182=0U;({unsigned int _tmp7FB=loc;struct _dyneither_ptr _tmp7FA=({const char*_tmp183="expecting integer constant";_tag_dyneither(_tmp183,sizeof(char),27U);});Cyc_Warn_err(_tmp7FB,_tmp7FA,_tag_dyneither(_tmp182,sizeof(void*),0U));});});
return 0;}_LL0:;}
# 1010
static struct _dyneither_ptr Cyc_Parse_exp2string(unsigned int loc,struct Cyc_Absyn_Exp*e){
void*_tmp185=e->r;void*_tmp186=_tmp185;struct _dyneither_ptr _tmp189;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp186)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp186)->f1).String_c).tag == 8){_LL1: _tmp189=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp186)->f1).String_c).val;_LL2:
 return _tmp189;}else{goto _LL3;}}else{_LL3: _LL4:
# 1014
({void*_tmp187=0U;({unsigned int _tmp7FD=loc;struct _dyneither_ptr _tmp7FC=({const char*_tmp188="expecting string constant";_tag_dyneither(_tmp188,sizeof(char),26U);});Cyc_Warn_err(_tmp7FD,_tmp7FC,_tag_dyneither(_tmp187,sizeof(void*),0U));});});
return _tag_dyneither(0,0,0);}_LL0:;}
# 1020
static unsigned int Cyc_Parse_cnst2uint(unsigned int loc,union Cyc_Absyn_Cnst x){
union Cyc_Absyn_Cnst _tmp18A=x;long long _tmp192;char _tmp191;int _tmp190;switch((_tmp18A.LongLong_c).tag){case 5U: _LL1: _tmp190=((_tmp18A.Int_c).val).f2;_LL2:
 return(unsigned int)_tmp190;case 2U: _LL3: _tmp191=((_tmp18A.Char_c).val).f2;_LL4:
 return(unsigned int)_tmp191;case 6U: _LL5: _tmp192=((_tmp18A.LongLong_c).val).f2;_LL6: {
# 1025
unsigned long long y=(unsigned long long)_tmp192;
if(y > -1)
({void*_tmp18B=0U;({unsigned int _tmp7FF=loc;struct _dyneither_ptr _tmp7FE=({const char*_tmp18C="integer constant too large";_tag_dyneither(_tmp18C,sizeof(char),27U);});Cyc_Warn_err(_tmp7FF,_tmp7FE,_tag_dyneither(_tmp18B,sizeof(void*),0U));});});
return(unsigned int)_tmp192;}default: _LL7: _LL8:
# 1030
({struct Cyc_String_pa_PrintArg_struct _tmp18F=({struct Cyc_String_pa_PrintArg_struct _tmp6B9;_tmp6B9.tag=0U,({struct _dyneither_ptr _tmp800=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(x));_tmp6B9.f1=_tmp800;});_tmp6B9;});void*_tmp18D[1U];_tmp18D[0]=& _tmp18F;({unsigned int _tmp802=loc;struct _dyneither_ptr _tmp801=({const char*_tmp18E="expected integer constant but found %s";_tag_dyneither(_tmp18E,sizeof(char),39U);});Cyc_Warn_err(_tmp802,_tmp801,_tag_dyneither(_tmp18D,sizeof(void*),1U));});});
return 0U;}_LL0:;}
# 1036
static struct Cyc_Absyn_Exp*Cyc_Parse_pat2exp(struct Cyc_Absyn_Pat*p){
void*_tmp193=p->r;void*_tmp194=_tmp193;struct Cyc_Absyn_Exp*_tmp1A1;struct _tuple0*_tmp1A0;struct Cyc_List_List*_tmp19F;struct _dyneither_ptr _tmp19E;int _tmp19D;char _tmp19C;enum Cyc_Absyn_Sign _tmp19B;int _tmp19A;struct Cyc_Absyn_Pat*_tmp199;struct Cyc_Absyn_Vardecl*_tmp198;struct _tuple0*_tmp197;switch(*((int*)_tmp194)){case 15U: _LL1: _tmp197=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL2:
 return Cyc_Absyn_unknownid_exp(_tmp197,p->loc);case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp194)->f2)->r)->tag == 0U){_LL3: _tmp198=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL4:
# 1040
 return({struct Cyc_Absyn_Exp*_tmp803=Cyc_Absyn_unknownid_exp(_tmp198->name,p->loc);Cyc_Absyn_deref_exp(_tmp803,p->loc);});}else{goto _LL13;}case 6U: _LL5: _tmp199=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL6:
 return({struct Cyc_Absyn_Exp*_tmp804=Cyc_Parse_pat2exp(_tmp199);Cyc_Absyn_address_exp(_tmp804,p->loc);});case 9U: _LL7: _LL8:
 return Cyc_Absyn_null_exp(p->loc);case 10U: _LL9: _tmp19B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_tmp19A=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp194)->f2;_LLA:
 return Cyc_Absyn_int_exp(_tmp19B,_tmp19A,p->loc);case 11U: _LLB: _tmp19C=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LLC:
 return Cyc_Absyn_char_exp(_tmp19C,p->loc);case 12U: _LLD: _tmp19E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_tmp19D=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp194)->f2;_LLE:
 return Cyc_Absyn_float_exp(_tmp19E,_tmp19D,p->loc);case 16U: if(((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp194)->f3 == 0){_LLF: _tmp1A0=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_tmp19F=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp194)->f2;_LL10: {
# 1047
struct Cyc_Absyn_Exp*e1=Cyc_Absyn_unknownid_exp(_tmp1A0,p->loc);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Parse_pat2exp,_tmp19F);
return Cyc_Absyn_unknowncall_exp(e1,es,p->loc);}}else{goto _LL13;}case 17U: _LL11: _tmp1A1=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp194)->f1;_LL12:
 return _tmp1A1;default: _LL13: _LL14:
# 1052
({void*_tmp195=0U;({unsigned int _tmp806=p->loc;struct _dyneither_ptr _tmp805=({const char*_tmp196="cannot mix patterns and expressions in case";_tag_dyneither(_tmp196,sizeof(char),44U);});Cyc_Warn_err(_tmp806,_tmp805,_tag_dyneither(_tmp195,sizeof(void*),0U));});});
return Cyc_Absyn_null_exp(p->loc);}_LL0:;}struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple20{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple20 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple21{unsigned int f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple21*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple22{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Parse_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Parse_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Parse_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Parse_Declarator val;};struct _tuple24{struct Cyc_Parse_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple24*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Parse_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple25{struct Cyc_Absyn_Tqual f1;struct Cyc_Parse_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple25 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple26{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple26*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};static char _tmp1A2[7U]="cnst_t";
# 1109 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Int_tok={Cyc_Core_Failure,{_tmp1A2,_tmp1A2,_tmp1A2 + 7U}};
# 1111
union Cyc_Absyn_Cnst Cyc_yyget_Int_tok(union Cyc_YYSTYPE yy1){
union Cyc_Absyn_Cnst yyzzz;
{union Cyc_YYSTYPE _tmp1A3=yy1;union Cyc_Absyn_Cnst _tmp1A4;if((_tmp1A3.Int_tok).tag == 1){_LL1: _tmp1A4=(_tmp1A3.Int_tok).val;_LL2:
# 1115
 yyzzz=_tmp1A4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Int_tok);}_LL0:;}
# 1119
return yyzzz;}
# 1121
union Cyc_YYSTYPE Cyc_Int_tok(union Cyc_Absyn_Cnst yy1){return({union Cyc_YYSTYPE _tmp6BA;(_tmp6BA.Int_tok).tag=1U,(_tmp6BA.Int_tok).val=yy1;_tmp6BA;});}static char _tmp1A5[5U]="char";
# 1110 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Char_tok={Cyc_Core_Failure,{_tmp1A5,_tmp1A5,_tmp1A5 + 5U}};
# 1112
char Cyc_yyget_Char_tok(union Cyc_YYSTYPE yy1){
char yyzzz;
{union Cyc_YYSTYPE _tmp1A6=yy1;char _tmp1A7;if((_tmp1A6.Char_tok).tag == 2){_LL1: _tmp1A7=(_tmp1A6.Char_tok).val;_LL2:
# 1116
 yyzzz=_tmp1A7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Char_tok);}_LL0:;}
# 1120
return yyzzz;}
# 1122
union Cyc_YYSTYPE Cyc_Char_tok(char yy1){return({union Cyc_YYSTYPE _tmp6BB;(_tmp6BB.Char_tok).tag=2U,(_tmp6BB.Char_tok).val=yy1;_tmp6BB;});}static char _tmp1A8[13U]="string_t<`H>";
# 1111 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_String_tok={Cyc_Core_Failure,{_tmp1A8,_tmp1A8,_tmp1A8 + 13U}};
# 1113
struct _dyneither_ptr Cyc_yyget_String_tok(union Cyc_YYSTYPE yy1){
struct _dyneither_ptr yyzzz;
{union Cyc_YYSTYPE _tmp1A9=yy1;struct _dyneither_ptr _tmp1AA;if((_tmp1A9.String_tok).tag == 3){_LL1: _tmp1AA=(_tmp1A9.String_tok).val;_LL2:
# 1117
 yyzzz=_tmp1AA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_String_tok);}_LL0:;}
# 1121
return yyzzz;}
# 1123
union Cyc_YYSTYPE Cyc_String_tok(struct _dyneither_ptr yy1){return({union Cyc_YYSTYPE _tmp6BC;(_tmp6BC.String_tok).tag=3U,(_tmp6BC.String_tok).val=yy1;_tmp6BC;});}static char _tmp1AB[45U]="$(Position::seg_t,booltype_t, ptrbound_t)@`H";
# 1114 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY1={Cyc_Core_Failure,{_tmp1AB,_tmp1AB,_tmp1AB + 45U}};
# 1116
struct _tuple21*Cyc_yyget_YY1(union Cyc_YYSTYPE yy1){
struct _tuple21*yyzzz;
{union Cyc_YYSTYPE _tmp1AC=yy1;struct _tuple21*_tmp1AD;if((_tmp1AC.YY1).tag == 9){_LL1: _tmp1AD=(_tmp1AC.YY1).val;_LL2:
# 1120
 yyzzz=_tmp1AD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY1);}_LL0:;}
# 1124
return yyzzz;}
# 1126
union Cyc_YYSTYPE Cyc_YY1(struct _tuple21*yy1){return({union Cyc_YYSTYPE _tmp6BD;(_tmp6BD.YY1).tag=9U,(_tmp6BD.YY1).val=yy1;_tmp6BD;});}static char _tmp1AE[11U]="ptrbound_t";
# 1115 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY2={Cyc_Core_Failure,{_tmp1AE,_tmp1AE,_tmp1AE + 11U}};
# 1117
void*Cyc_yyget_YY2(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp1AF=yy1;void*_tmp1B0;if((_tmp1AF.YY2).tag == 10){_LL1: _tmp1B0=(_tmp1AF.YY2).val;_LL2:
# 1121
 yyzzz=_tmp1B0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY2);}_LL0:;}
# 1125
return yyzzz;}
# 1127
union Cyc_YYSTYPE Cyc_YY2(void*yy1){return({union Cyc_YYSTYPE _tmp6BE;(_tmp6BE.YY2).tag=10U,(_tmp6BE.YY2).val=yy1;_tmp6BE;});}static char _tmp1B1[28U]="list_t<offsetof_field_t,`H>";
# 1116 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY3={Cyc_Core_Failure,{_tmp1B1,_tmp1B1,_tmp1B1 + 28U}};
# 1118
struct Cyc_List_List*Cyc_yyget_YY3(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1B2=yy1;struct Cyc_List_List*_tmp1B3;if((_tmp1B2.YY3).tag == 11){_LL1: _tmp1B3=(_tmp1B2.YY3).val;_LL2:
# 1122
 yyzzz=_tmp1B3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY3);}_LL0:;}
# 1126
return yyzzz;}
# 1128
union Cyc_YYSTYPE Cyc_YY3(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6BF;(_tmp6BF.YY3).tag=11U,(_tmp6BF.YY3).val=yy1;_tmp6BF;});}static char _tmp1B4[6U]="exp_t";
# 1117 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Exp_tok={Cyc_Core_Failure,{_tmp1B4,_tmp1B4,_tmp1B4 + 6U}};
# 1119
struct Cyc_Absyn_Exp*Cyc_yyget_Exp_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp1B5=yy1;struct Cyc_Absyn_Exp*_tmp1B6;if((_tmp1B5.Exp_tok).tag == 7){_LL1: _tmp1B6=(_tmp1B5.Exp_tok).val;_LL2:
# 1123
 yyzzz=_tmp1B6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Exp_tok);}_LL0:;}
# 1127
return yyzzz;}
# 1129
union Cyc_YYSTYPE Cyc_Exp_tok(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6C0;(_tmp6C0.Exp_tok).tag=7U,(_tmp6C0.Exp_tok).val=yy1;_tmp6C0;});}static char _tmp1B7[17U]="list_t<exp_t,`H>";
# 1125 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY4={Cyc_Core_Failure,{_tmp1B7,_tmp1B7,_tmp1B7 + 17U}};
# 1127
struct Cyc_List_List*Cyc_yyget_YY4(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1B8=yy1;struct Cyc_List_List*_tmp1B9;if((_tmp1B8.YY4).tag == 12){_LL1: _tmp1B9=(_tmp1B8.YY4).val;_LL2:
# 1131
 yyzzz=_tmp1B9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY4);}_LL0:;}
# 1135
return yyzzz;}
# 1137
union Cyc_YYSTYPE Cyc_YY4(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C1;(_tmp6C1.YY4).tag=12U,(_tmp6C1.YY4).val=yy1;_tmp6C1;});}static char _tmp1BA[47U]="list_t<$(list_t<designator_t,`H>,exp_t)@`H,`H>";
# 1126 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY5={Cyc_Core_Failure,{_tmp1BA,_tmp1BA,_tmp1BA + 47U}};
# 1128
struct Cyc_List_List*Cyc_yyget_YY5(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1BB=yy1;struct Cyc_List_List*_tmp1BC;if((_tmp1BB.YY5).tag == 13){_LL1: _tmp1BC=(_tmp1BB.YY5).val;_LL2:
# 1132
 yyzzz=_tmp1BC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY5);}_LL0:;}
# 1136
return yyzzz;}
# 1138
union Cyc_YYSTYPE Cyc_YY5(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C2;(_tmp6C2.YY5).tag=13U,(_tmp6C2.YY5).val=yy1;_tmp6C2;});}static char _tmp1BD[9U]="primop_t";
# 1127 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY6={Cyc_Core_Failure,{_tmp1BD,_tmp1BD,_tmp1BD + 9U}};
# 1129
enum Cyc_Absyn_Primop Cyc_yyget_YY6(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Primop yyzzz;
{union Cyc_YYSTYPE _tmp1BE=yy1;enum Cyc_Absyn_Primop _tmp1BF;if((_tmp1BE.YY6).tag == 14){_LL1: _tmp1BF=(_tmp1BE.YY6).val;_LL2:
# 1133
 yyzzz=_tmp1BF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY6);}_LL0:;}
# 1137
return yyzzz;}
# 1139
union Cyc_YYSTYPE Cyc_YY6(enum Cyc_Absyn_Primop yy1){return({union Cyc_YYSTYPE _tmp6C3;(_tmp6C3.YY6).tag=14U,(_tmp6C3.YY6).val=yy1;_tmp6C3;});}static char _tmp1C0[19U]="opt_t<primop_t,`H>";
# 1128 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY7={Cyc_Core_Failure,{_tmp1C0,_tmp1C0,_tmp1C0 + 19U}};
# 1130
struct Cyc_Core_Opt*Cyc_yyget_YY7(union Cyc_YYSTYPE yy1){
struct Cyc_Core_Opt*yyzzz;
{union Cyc_YYSTYPE _tmp1C1=yy1;struct Cyc_Core_Opt*_tmp1C2;if((_tmp1C1.YY7).tag == 15){_LL1: _tmp1C2=(_tmp1C1.YY7).val;_LL2:
# 1134
 yyzzz=_tmp1C2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY7);}_LL0:;}
# 1138
return yyzzz;}
# 1140
union Cyc_YYSTYPE Cyc_YY7(struct Cyc_Core_Opt*yy1){return({union Cyc_YYSTYPE _tmp6C4;(_tmp6C4.YY7).tag=15U,(_tmp6C4.YY7).val=yy1;_tmp6C4;});}static char _tmp1C3[7U]="qvar_t";
# 1129 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_QualId_tok={Cyc_Core_Failure,{_tmp1C3,_tmp1C3,_tmp1C3 + 7U}};
# 1131
struct _tuple0*Cyc_yyget_QualId_tok(union Cyc_YYSTYPE yy1){
struct _tuple0*yyzzz;
{union Cyc_YYSTYPE _tmp1C4=yy1;struct _tuple0*_tmp1C5;if((_tmp1C4.QualId_tok).tag == 5){_LL1: _tmp1C5=(_tmp1C4.QualId_tok).val;_LL2:
# 1135
 yyzzz=_tmp1C5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_QualId_tok);}_LL0:;}
# 1139
return yyzzz;}
# 1141
union Cyc_YYSTYPE Cyc_QualId_tok(struct _tuple0*yy1){return({union Cyc_YYSTYPE _tmp6C5;(_tmp6C5.QualId_tok).tag=5U,(_tmp6C5.QualId_tok).val=yy1;_tmp6C5;});}static char _tmp1C6[7U]="stmt_t";
# 1132 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Stmt_tok={Cyc_Core_Failure,{_tmp1C6,_tmp1C6,_tmp1C6 + 7U}};
# 1134
struct Cyc_Absyn_Stmt*Cyc_yyget_Stmt_tok(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Stmt*yyzzz;
{union Cyc_YYSTYPE _tmp1C7=yy1;struct Cyc_Absyn_Stmt*_tmp1C8;if((_tmp1C7.Stmt_tok).tag == 8){_LL1: _tmp1C8=(_tmp1C7.Stmt_tok).val;_LL2:
# 1138
 yyzzz=_tmp1C8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Stmt_tok);}_LL0:;}
# 1142
return yyzzz;}
# 1144
union Cyc_YYSTYPE Cyc_Stmt_tok(struct Cyc_Absyn_Stmt*yy1){return({union Cyc_YYSTYPE _tmp6C6;(_tmp6C6.Stmt_tok).tag=8U,(_tmp6C6.Stmt_tok).val=yy1;_tmp6C6;});}static char _tmp1C9[27U]="list_t<switch_clause_t,`H>";
# 1136 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY8={Cyc_Core_Failure,{_tmp1C9,_tmp1C9,_tmp1C9 + 27U}};
# 1138
struct Cyc_List_List*Cyc_yyget_YY8(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1CA=yy1;struct Cyc_List_List*_tmp1CB;if((_tmp1CA.YY8).tag == 16){_LL1: _tmp1CB=(_tmp1CA.YY8).val;_LL2:
# 1142
 yyzzz=_tmp1CB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY8);}_LL0:;}
# 1146
return yyzzz;}
# 1148
union Cyc_YYSTYPE Cyc_YY8(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6C7;(_tmp6C7.YY8).tag=16U,(_tmp6C7.YY8).val=yy1;_tmp6C7;});}static char _tmp1CC[6U]="pat_t";
# 1137 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY9={Cyc_Core_Failure,{_tmp1CC,_tmp1CC,_tmp1CC + 6U}};
# 1139
struct Cyc_Absyn_Pat*Cyc_yyget_YY9(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Pat*yyzzz;
{union Cyc_YYSTYPE _tmp1CD=yy1;struct Cyc_Absyn_Pat*_tmp1CE;if((_tmp1CD.YY9).tag == 17){_LL1: _tmp1CE=(_tmp1CD.YY9).val;_LL2:
# 1143
 yyzzz=_tmp1CE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY9);}_LL0:;}
# 1147
return yyzzz;}
# 1149
union Cyc_YYSTYPE Cyc_YY9(struct Cyc_Absyn_Pat*yy1){return({union Cyc_YYSTYPE _tmp6C8;(_tmp6C8.YY9).tag=17U,(_tmp6C8.YY9).val=yy1;_tmp6C8;});}static char _tmp1CF[28U]="$(list_t<pat_t,`H>,bool)@`H";
# 1142 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY10={Cyc_Core_Failure,{_tmp1CF,_tmp1CF,_tmp1CF + 28U}};
# 1144
struct _tuple22*Cyc_yyget_YY10(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1D0=yy1;struct _tuple22*_tmp1D1;if((_tmp1D0.YY10).tag == 18){_LL1: _tmp1D1=(_tmp1D0.YY10).val;_LL2:
# 1148
 yyzzz=_tmp1D1;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY10);}_LL0:;}
# 1152
return yyzzz;}
# 1154
union Cyc_YYSTYPE Cyc_YY10(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6C9;(_tmp6C9.YY10).tag=18U,(_tmp6C9.YY10).val=yy1;_tmp6C9;});}static char _tmp1D2[17U]="list_t<pat_t,`H>";
# 1143 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY11={Cyc_Core_Failure,{_tmp1D2,_tmp1D2,_tmp1D2 + 17U}};
# 1145
struct Cyc_List_List*Cyc_yyget_YY11(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1D3=yy1;struct Cyc_List_List*_tmp1D4;if((_tmp1D3.YY11).tag == 19){_LL1: _tmp1D4=(_tmp1D3.YY11).val;_LL2:
# 1149
 yyzzz=_tmp1D4;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY11);}_LL0:;}
# 1153
return yyzzz;}
# 1155
union Cyc_YYSTYPE Cyc_YY11(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CA;(_tmp6CA.YY11).tag=19U,(_tmp6CA.YY11).val=yy1;_tmp6CA;});}static char _tmp1D5[36U]="$(list_t<designator_t,`H>,pat_t)@`H";
# 1144 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY12={Cyc_Core_Failure,{_tmp1D5,_tmp1D5,_tmp1D5 + 36U}};
# 1146
struct _tuple23*Cyc_yyget_YY12(union Cyc_YYSTYPE yy1){
struct _tuple23*yyzzz;
{union Cyc_YYSTYPE _tmp1D6=yy1;struct _tuple23*_tmp1D7;if((_tmp1D6.YY12).tag == 20){_LL1: _tmp1D7=(_tmp1D6.YY12).val;_LL2:
# 1150
 yyzzz=_tmp1D7;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY12);}_LL0:;}
# 1154
return yyzzz;}
# 1156
union Cyc_YYSTYPE Cyc_YY12(struct _tuple23*yy1){return({union Cyc_YYSTYPE _tmp6CB;(_tmp6CB.YY12).tag=20U,(_tmp6CB.YY12).val=yy1;_tmp6CB;});}static char _tmp1D8[47U]="list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>";
# 1145 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY13={Cyc_Core_Failure,{_tmp1D8,_tmp1D8,_tmp1D8 + 47U}};
# 1147
struct Cyc_List_List*Cyc_yyget_YY13(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1D9=yy1;struct Cyc_List_List*_tmp1DA;if((_tmp1D9.YY13).tag == 21){_LL1: _tmp1DA=(_tmp1D9.YY13).val;_LL2:
# 1151
 yyzzz=_tmp1DA;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY13);}_LL0:;}
# 1155
return yyzzz;}
# 1157
union Cyc_YYSTYPE Cyc_YY13(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CC;(_tmp6CC.YY13).tag=21U,(_tmp6CC.YY13).val=yy1;_tmp6CC;});}static char _tmp1DB[58U]="$(list_t<$(list_t<designator_t,`H>,pat_t)@`H,`H>,bool)@`H";
# 1146 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY14={Cyc_Core_Failure,{_tmp1DB,_tmp1DB,_tmp1DB + 58U}};
# 1148
struct _tuple22*Cyc_yyget_YY14(union Cyc_YYSTYPE yy1){
struct _tuple22*yyzzz;
{union Cyc_YYSTYPE _tmp1DC=yy1;struct _tuple22*_tmp1DD;if((_tmp1DC.YY14).tag == 22){_LL1: _tmp1DD=(_tmp1DC.YY14).val;_LL2:
# 1152
 yyzzz=_tmp1DD;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY14);}_LL0:;}
# 1156
return yyzzz;}
# 1158
union Cyc_YYSTYPE Cyc_YY14(struct _tuple22*yy1){return({union Cyc_YYSTYPE _tmp6CD;(_tmp6CD.YY14).tag=22U,(_tmp6CD.YY14).val=yy1;_tmp6CD;});}static char _tmp1DE[9U]="fndecl_t";
# 1147 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY15={Cyc_Core_Failure,{_tmp1DE,_tmp1DE,_tmp1DE + 9U}};
# 1149
struct Cyc_Absyn_Fndecl*Cyc_yyget_YY15(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Fndecl*yyzzz;
{union Cyc_YYSTYPE _tmp1DF=yy1;struct Cyc_Absyn_Fndecl*_tmp1E0;if((_tmp1DF.YY15).tag == 23){_LL1: _tmp1E0=(_tmp1DF.YY15).val;_LL2:
# 1153
 yyzzz=_tmp1E0;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY15);}_LL0:;}
# 1157
return yyzzz;}
# 1159
union Cyc_YYSTYPE Cyc_YY15(struct Cyc_Absyn_Fndecl*yy1){return({union Cyc_YYSTYPE _tmp6CE;(_tmp6CE.YY15).tag=23U,(_tmp6CE.YY15).val=yy1;_tmp6CE;});}static char _tmp1E1[18U]="list_t<decl_t,`H>";
# 1148 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY16={Cyc_Core_Failure,{_tmp1E1,_tmp1E1,_tmp1E1 + 18U}};
# 1150
struct Cyc_List_List*Cyc_yyget_YY16(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1E2=yy1;struct Cyc_List_List*_tmp1E3;if((_tmp1E2.YY16).tag == 24){_LL1: _tmp1E3=(_tmp1E2.YY16).val;_LL2:
# 1154
 yyzzz=_tmp1E3;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY16);}_LL0:;}
# 1158
return yyzzz;}
# 1160
union Cyc_YYSTYPE Cyc_YY16(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6CF;(_tmp6CF.YY16).tag=24U,(_tmp6CF.YY16).val=yy1;_tmp6CF;});}static char _tmp1E4[12U]="decl_spec_t";
# 1150 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY17={Cyc_Core_Failure,{_tmp1E4,_tmp1E4,_tmp1E4 + 12U}};
# 1152
struct Cyc_Parse_Declaration_spec Cyc_yyget_YY17(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declaration_spec yyzzz;
{union Cyc_YYSTYPE _tmp1E5=yy1;struct Cyc_Parse_Declaration_spec _tmp1E6;if((_tmp1E5.YY17).tag == 25){_LL1: _tmp1E6=(_tmp1E5.YY17).val;_LL2:
# 1156
 yyzzz=_tmp1E6;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY17);}_LL0:;}
# 1160
return yyzzz;}
# 1162
union Cyc_YYSTYPE Cyc_YY17(struct Cyc_Parse_Declaration_spec yy1){return({union Cyc_YYSTYPE _tmp6D0;(_tmp6D0.YY17).tag=25U,(_tmp6D0.YY17).val=yy1;_tmp6D0;});}static char _tmp1E7[31U]="$(declarator_t<`yy>,exp_opt_t)";
# 1151 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY18={Cyc_Core_Failure,{_tmp1E7,_tmp1E7,_tmp1E7 + 31U}};
# 1153
struct _tuple11 Cyc_yyget_YY18(union Cyc_YYSTYPE yy1){
struct _tuple11 yyzzz;
{union Cyc_YYSTYPE _tmp1E8=yy1;struct _tuple11 _tmp1E9;if((_tmp1E8.YY18).tag == 26){_LL1: _tmp1E9=(_tmp1E8.YY18).val;_LL2:
# 1157
 yyzzz=_tmp1E9;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY18);}_LL0:;}
# 1161
return yyzzz;}
# 1163
union Cyc_YYSTYPE Cyc_YY18(struct _tuple11 yy1){return({union Cyc_YYSTYPE _tmp6D1;(_tmp6D1.YY18).tag=26U,(_tmp6D1.YY18).val=yy1;_tmp6D1;});}static char _tmp1EA[23U]="declarator_list_t<`yy>";
# 1152 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY19={Cyc_Core_Failure,{_tmp1EA,_tmp1EA,_tmp1EA + 23U}};
# 1154
struct _tuple12*Cyc_yyget_YY19(union Cyc_YYSTYPE yy1){
struct _tuple12*yyzzz;
{union Cyc_YYSTYPE _tmp1EB=yy1;struct _tuple12*_tmp1EC;if((_tmp1EB.YY19).tag == 27){_LL1: _tmp1EC=(_tmp1EB.YY19).val;_LL2:
# 1158
 yyzzz=_tmp1EC;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY19);}_LL0:;}
# 1162
return yyzzz;}
# 1164
union Cyc_YYSTYPE Cyc_YY19(struct _tuple12*yy1){return({union Cyc_YYSTYPE _tmp6D2;(_tmp6D2.YY19).tag=27U,(_tmp6D2.YY19).val=yy1;_tmp6D2;});}static char _tmp1ED[19U]="storage_class_t@`H";
# 1153 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY20={Cyc_Core_Failure,{_tmp1ED,_tmp1ED,_tmp1ED + 19U}};
# 1155
enum Cyc_Parse_Storage_class*Cyc_yyget_YY20(union Cyc_YYSTYPE yy1){
enum Cyc_Parse_Storage_class*yyzzz;
{union Cyc_YYSTYPE _tmp1EE=yy1;enum Cyc_Parse_Storage_class*_tmp1EF;if((_tmp1EE.YY20).tag == 28){_LL1: _tmp1EF=(_tmp1EE.YY20).val;_LL2:
# 1159
 yyzzz=_tmp1EF;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY20);}_LL0:;}
# 1163
return yyzzz;}
# 1165
union Cyc_YYSTYPE Cyc_YY20(enum Cyc_Parse_Storage_class*yy1){return({union Cyc_YYSTYPE _tmp6D3;(_tmp6D3.YY20).tag=28U,(_tmp6D3.YY20).val=yy1;_tmp6D3;});}static char _tmp1F0[17U]="type_specifier_t";
# 1154 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY21={Cyc_Core_Failure,{_tmp1F0,_tmp1F0,_tmp1F0 + 17U}};
# 1156
struct Cyc_Parse_Type_specifier Cyc_yyget_YY21(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Type_specifier yyzzz;
{union Cyc_YYSTYPE _tmp1F1=yy1;struct Cyc_Parse_Type_specifier _tmp1F2;if((_tmp1F1.YY21).tag == 29){_LL1: _tmp1F2=(_tmp1F1.YY21).val;_LL2:
# 1160
 yyzzz=_tmp1F2;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY21);}_LL0:;}
# 1164
return yyzzz;}
# 1166
union Cyc_YYSTYPE Cyc_YY21(struct Cyc_Parse_Type_specifier yy1){return({union Cyc_YYSTYPE _tmp6D4;(_tmp6D4.YY21).tag=29U,(_tmp6D4.YY21).val=yy1;_tmp6D4;});}static char _tmp1F3[12U]="aggr_kind_t";
# 1156 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY22={Cyc_Core_Failure,{_tmp1F3,_tmp1F3,_tmp1F3 + 12U}};
# 1158
enum Cyc_Absyn_AggrKind Cyc_yyget_YY22(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_AggrKind yyzzz;
{union Cyc_YYSTYPE _tmp1F4=yy1;enum Cyc_Absyn_AggrKind _tmp1F5;if((_tmp1F4.YY22).tag == 30){_LL1: _tmp1F5=(_tmp1F4.YY22).val;_LL2:
# 1162
 yyzzz=_tmp1F5;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY22);}_LL0:;}
# 1166
return yyzzz;}
# 1168
union Cyc_YYSTYPE Cyc_YY22(enum Cyc_Absyn_AggrKind yy1){return({union Cyc_YYSTYPE _tmp6D5;(_tmp6D5.YY22).tag=30U,(_tmp6D5.YY22).val=yy1;_tmp6D5;});}static char _tmp1F6[8U]="tqual_t";
# 1157 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY23={Cyc_Core_Failure,{_tmp1F6,_tmp1F6,_tmp1F6 + 8U}};
# 1159
struct Cyc_Absyn_Tqual Cyc_yyget_YY23(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Tqual yyzzz;
{union Cyc_YYSTYPE _tmp1F7=yy1;struct Cyc_Absyn_Tqual _tmp1F8;if((_tmp1F7.YY23).tag == 31){_LL1: _tmp1F8=(_tmp1F7.YY23).val;_LL2:
# 1163
 yyzzz=_tmp1F8;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY23);}_LL0:;}
# 1167
return yyzzz;}
# 1169
union Cyc_YYSTYPE Cyc_YY23(struct Cyc_Absyn_Tqual yy1){return({union Cyc_YYSTYPE _tmp6D6;(_tmp6D6.YY23).tag=31U,(_tmp6D6.YY23).val=yy1;_tmp6D6;});}static char _tmp1F9[23U]="list_t<aggrfield_t,`H>";
# 1158 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY24={Cyc_Core_Failure,{_tmp1F9,_tmp1F9,_tmp1F9 + 23U}};
# 1160
struct Cyc_List_List*Cyc_yyget_YY24(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1FA=yy1;struct Cyc_List_List*_tmp1FB;if((_tmp1FA.YY24).tag == 32){_LL1: _tmp1FB=(_tmp1FA.YY24).val;_LL2:
# 1164
 yyzzz=_tmp1FB;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY24);}_LL0:;}
# 1168
return yyzzz;}
# 1170
union Cyc_YYSTYPE Cyc_YY24(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D7;(_tmp6D7.YY24).tag=32U,(_tmp6D7.YY24).val=yy1;_tmp6D7;});}static char _tmp1FC[34U]="list_t<list_t<aggrfield_t,`H>,`H>";
# 1159 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY25={Cyc_Core_Failure,{_tmp1FC,_tmp1FC,_tmp1FC + 34U}};
# 1161
struct Cyc_List_List*Cyc_yyget_YY25(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp1FD=yy1;struct Cyc_List_List*_tmp1FE;if((_tmp1FD.YY25).tag == 33){_LL1: _tmp1FE=(_tmp1FD.YY25).val;_LL2:
# 1165
 yyzzz=_tmp1FE;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY25);}_LL0:;}
# 1169
return yyzzz;}
# 1171
union Cyc_YYSTYPE Cyc_YY25(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D8;(_tmp6D8.YY25).tag=33U,(_tmp6D8.YY25).val=yy1;_tmp6D8;});}static char _tmp1FF[33U]="list_t<type_modifier_t<`yy>,`yy>";
# 1160 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY26={Cyc_Core_Failure,{_tmp1FF,_tmp1FF,_tmp1FF + 33U}};
# 1162
struct Cyc_List_List*Cyc_yyget_YY26(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp200=yy1;struct Cyc_List_List*_tmp201;if((_tmp200.YY26).tag == 34){_LL1: _tmp201=(_tmp200.YY26).val;_LL2:
# 1166
 yyzzz=_tmp201;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY26);}_LL0:;}
# 1170
return yyzzz;}
# 1172
union Cyc_YYSTYPE Cyc_YY26(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6D9;(_tmp6D9.YY26).tag=34U,(_tmp6D9.YY26).val=yy1;_tmp6D9;});}static char _tmp202[18U]="declarator_t<`yy>";
# 1161 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY27={Cyc_Core_Failure,{_tmp202,_tmp202,_tmp202 + 18U}};
# 1163
struct Cyc_Parse_Declarator Cyc_yyget_YY27(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Declarator yyzzz;
{union Cyc_YYSTYPE _tmp203=yy1;struct Cyc_Parse_Declarator _tmp204;if((_tmp203.YY27).tag == 35){_LL1: _tmp204=(_tmp203.YY27).val;_LL2:
# 1167
 yyzzz=_tmp204;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY27);}_LL0:;}
# 1171
return yyzzz;}
# 1173
union Cyc_YYSTYPE Cyc_YY27(struct Cyc_Parse_Declarator yy1){return({union Cyc_YYSTYPE _tmp6DA;(_tmp6DA.YY27).tag=35U,(_tmp6DA.YY27).val=yy1;_tmp6DA;});}static char _tmp205[45U]="$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy";
# 1162 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY28={Cyc_Core_Failure,{_tmp205,_tmp205,_tmp205 + 45U}};
# 1164
struct _tuple24*Cyc_yyget_YY28(union Cyc_YYSTYPE yy1){
struct _tuple24*yyzzz;
{union Cyc_YYSTYPE _tmp206=yy1;struct _tuple24*_tmp207;if((_tmp206.YY28).tag == 36){_LL1: _tmp207=(_tmp206.YY28).val;_LL2:
# 1168
 yyzzz=_tmp207;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY28);}_LL0:;}
# 1172
return yyzzz;}
# 1174
union Cyc_YYSTYPE Cyc_YY28(struct _tuple24*yy1){return({union Cyc_YYSTYPE _tmp6DB;(_tmp6DB.YY28).tag=36U,(_tmp6DB.YY28).val=yy1;_tmp6DB;});}static char _tmp208[57U]="list_t<$(declarator_t<`yy>,exp_opt_t,exp_opt_t)@`yy,`yy>";
# 1163 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY29={Cyc_Core_Failure,{_tmp208,_tmp208,_tmp208 + 57U}};
# 1165
struct Cyc_List_List*Cyc_yyget_YY29(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp209=yy1;struct Cyc_List_List*_tmp20A;if((_tmp209.YY29).tag == 37){_LL1: _tmp20A=(_tmp209.YY29).val;_LL2:
# 1169
 yyzzz=_tmp20A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY29);}_LL0:;}
# 1173
return yyzzz;}
# 1175
union Cyc_YYSTYPE Cyc_YY29(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6DC;(_tmp6DC.YY29).tag=37U,(_tmp6DC.YY29).val=yy1;_tmp6DC;});}static char _tmp20B[26U]="abstractdeclarator_t<`yy>";
# 1164 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY30={Cyc_Core_Failure,{_tmp20B,_tmp20B,_tmp20B + 26U}};
# 1166
struct Cyc_Parse_Abstractdeclarator Cyc_yyget_YY30(union Cyc_YYSTYPE yy1){
struct Cyc_Parse_Abstractdeclarator yyzzz;
{union Cyc_YYSTYPE _tmp20C=yy1;struct Cyc_Parse_Abstractdeclarator _tmp20D;if((_tmp20C.YY30).tag == 38){_LL1: _tmp20D=(_tmp20C.YY30).val;_LL2:
# 1170
 yyzzz=_tmp20D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY30);}_LL0:;}
# 1174
return yyzzz;}
# 1176
union Cyc_YYSTYPE Cyc_YY30(struct Cyc_Parse_Abstractdeclarator yy1){return({union Cyc_YYSTYPE _tmp6DD;(_tmp6DD.YY30).tag=38U,(_tmp6DD.YY30).val=yy1;_tmp6DD;});}static char _tmp20E[5U]="bool";
# 1165 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY31={Cyc_Core_Failure,{_tmp20E,_tmp20E,_tmp20E + 5U}};
# 1167
int Cyc_yyget_YY31(union Cyc_YYSTYPE yy1){
int yyzzz;
{union Cyc_YYSTYPE _tmp20F=yy1;int _tmp210;if((_tmp20F.YY31).tag == 39){_LL1: _tmp210=(_tmp20F.YY31).val;_LL2:
# 1171
 yyzzz=_tmp210;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY31);}_LL0:;}
# 1175
return yyzzz;}
# 1177
union Cyc_YYSTYPE Cyc_YY31(int yy1){return({union Cyc_YYSTYPE _tmp6DE;(_tmp6DE.YY31).tag=39U,(_tmp6DE.YY31).val=yy1;_tmp6DE;});}static char _tmp211[8U]="scope_t";
# 1166 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY32={Cyc_Core_Failure,{_tmp211,_tmp211,_tmp211 + 8U}};
# 1168
enum Cyc_Absyn_Scope Cyc_yyget_YY32(union Cyc_YYSTYPE yy1){
enum Cyc_Absyn_Scope yyzzz;
{union Cyc_YYSTYPE _tmp212=yy1;enum Cyc_Absyn_Scope _tmp213;if((_tmp212.YY32).tag == 40){_LL1: _tmp213=(_tmp212.YY32).val;_LL2:
# 1172
 yyzzz=_tmp213;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY32);}_LL0:;}
# 1176
return yyzzz;}
# 1178
union Cyc_YYSTYPE Cyc_YY32(enum Cyc_Absyn_Scope yy1){return({union Cyc_YYSTYPE _tmp6DF;(_tmp6DF.YY32).tag=40U,(_tmp6DF.YY32).val=yy1;_tmp6DF;});}static char _tmp214[16U]="datatypefield_t";
# 1167 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY33={Cyc_Core_Failure,{_tmp214,_tmp214,_tmp214 + 16U}};
# 1169
struct Cyc_Absyn_Datatypefield*Cyc_yyget_YY33(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Datatypefield*yyzzz;
{union Cyc_YYSTYPE _tmp215=yy1;struct Cyc_Absyn_Datatypefield*_tmp216;if((_tmp215.YY33).tag == 41){_LL1: _tmp216=(_tmp215.YY33).val;_LL2:
# 1173
 yyzzz=_tmp216;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY33);}_LL0:;}
# 1177
return yyzzz;}
# 1179
union Cyc_YYSTYPE Cyc_YY33(struct Cyc_Absyn_Datatypefield*yy1){return({union Cyc_YYSTYPE _tmp6E0;(_tmp6E0.YY33).tag=41U,(_tmp6E0.YY33).val=yy1;_tmp6E0;});}static char _tmp217[27U]="list_t<datatypefield_t,`H>";
# 1168 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY34={Cyc_Core_Failure,{_tmp217,_tmp217,_tmp217 + 27U}};
# 1170
struct Cyc_List_List*Cyc_yyget_YY34(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp218=yy1;struct Cyc_List_List*_tmp219;if((_tmp218.YY34).tag == 42){_LL1: _tmp219=(_tmp218.YY34).val;_LL2:
# 1174
 yyzzz=_tmp219;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY34);}_LL0:;}
# 1178
return yyzzz;}
# 1180
union Cyc_YYSTYPE Cyc_YY34(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E1;(_tmp6E1.YY34).tag=42U,(_tmp6E1.YY34).val=yy1;_tmp6E1;});}static char _tmp21A[41U]="$(tqual_t,type_specifier_t,attributes_t)";
# 1169 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY35={Cyc_Core_Failure,{_tmp21A,_tmp21A,_tmp21A + 41U}};
# 1171
struct _tuple25 Cyc_yyget_YY35(union Cyc_YYSTYPE yy1){
struct _tuple25 yyzzz;
{union Cyc_YYSTYPE _tmp21B=yy1;struct _tuple25 _tmp21C;if((_tmp21B.YY35).tag == 43){_LL1: _tmp21C=(_tmp21B.YY35).val;_LL2:
# 1175
 yyzzz=_tmp21C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY35);}_LL0:;}
# 1179
return yyzzz;}
# 1181
union Cyc_YYSTYPE Cyc_YY35(struct _tuple25 yy1){return({union Cyc_YYSTYPE _tmp6E2;(_tmp6E2.YY35).tag=43U,(_tmp6E2.YY35).val=yy1;_tmp6E2;});}static char _tmp21D[17U]="list_t<var_t,`H>";
# 1170 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY36={Cyc_Core_Failure,{_tmp21D,_tmp21D,_tmp21D + 17U}};
# 1172
struct Cyc_List_List*Cyc_yyget_YY36(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp21E=yy1;struct Cyc_List_List*_tmp21F;if((_tmp21E.YY36).tag == 44){_LL1: _tmp21F=(_tmp21E.YY36).val;_LL2:
# 1176
 yyzzz=_tmp21F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY36);}_LL0:;}
# 1180
return yyzzz;}
# 1182
union Cyc_YYSTYPE Cyc_YY36(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E3;(_tmp6E3.YY36).tag=44U,(_tmp6E3.YY36).val=yy1;_tmp6E3;});}static char _tmp220[31U]="$(var_opt_t,tqual_t,type_t)@`H";
# 1171 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY37={Cyc_Core_Failure,{_tmp220,_tmp220,_tmp220 + 31U}};
# 1173
struct _tuple8*Cyc_yyget_YY37(union Cyc_YYSTYPE yy1){
struct _tuple8*yyzzz;
{union Cyc_YYSTYPE _tmp221=yy1;struct _tuple8*_tmp222;if((_tmp221.YY37).tag == 45){_LL1: _tmp222=(_tmp221.YY37).val;_LL2:
# 1177
 yyzzz=_tmp222;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY37);}_LL0:;}
# 1181
return yyzzz;}
# 1183
union Cyc_YYSTYPE Cyc_YY37(struct _tuple8*yy1){return({union Cyc_YYSTYPE _tmp6E4;(_tmp6E4.YY37).tag=45U,(_tmp6E4.YY37).val=yy1;_tmp6E4;});}static char _tmp223[42U]="list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>";
# 1172 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY38={Cyc_Core_Failure,{_tmp223,_tmp223,_tmp223 + 42U}};
# 1174
struct Cyc_List_List*Cyc_yyget_YY38(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp224=yy1;struct Cyc_List_List*_tmp225;if((_tmp224.YY38).tag == 46){_LL1: _tmp225=(_tmp224.YY38).val;_LL2:
# 1178
 yyzzz=_tmp225;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY38);}_LL0:;}
# 1182
return yyzzz;}
# 1184
union Cyc_YYSTYPE Cyc_YY38(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E5;(_tmp6E5.YY38).tag=46U,(_tmp6E5.YY38).val=yy1;_tmp6E5;});}static char _tmp226[115U]="$(list_t<$(var_opt_t,tqual_t,type_t)@`H,`H>, bool,vararg_info_t *`H,type_opt_t, list_t<$(type_t,type_t)@`H,`H>)@`H";
# 1173 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY39={Cyc_Core_Failure,{_tmp226,_tmp226,_tmp226 + 115U}};
# 1175
struct _tuple26*Cyc_yyget_YY39(union Cyc_YYSTYPE yy1){
struct _tuple26*yyzzz;
{union Cyc_YYSTYPE _tmp227=yy1;struct _tuple26*_tmp228;if((_tmp227.YY39).tag == 47){_LL1: _tmp228=(_tmp227.YY39).val;_LL2:
# 1179
 yyzzz=_tmp228;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY39);}_LL0:;}
# 1183
return yyzzz;}
# 1185
union Cyc_YYSTYPE Cyc_YY39(struct _tuple26*yy1){return({union Cyc_YYSTYPE _tmp6E6;(_tmp6E6.YY39).tag=47U,(_tmp6E6.YY39).val=yy1;_tmp6E6;});}static char _tmp229[8U]="types_t";
# 1174 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY40={Cyc_Core_Failure,{_tmp229,_tmp229,_tmp229 + 8U}};
# 1176
struct Cyc_List_List*Cyc_yyget_YY40(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22A=yy1;struct Cyc_List_List*_tmp22B;if((_tmp22A.YY40).tag == 48){_LL1: _tmp22B=(_tmp22A.YY40).val;_LL2:
# 1180
 yyzzz=_tmp22B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY40);}_LL0:;}
# 1184
return yyzzz;}
# 1186
union Cyc_YYSTYPE Cyc_YY40(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E7;(_tmp6E7.YY40).tag=48U,(_tmp6E7.YY40).val=yy1;_tmp6E7;});}static char _tmp22C[24U]="list_t<designator_t,`H>";
# 1176 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY41={Cyc_Core_Failure,{_tmp22C,_tmp22C,_tmp22C + 24U}};
# 1178
struct Cyc_List_List*Cyc_yyget_YY41(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp22D=yy1;struct Cyc_List_List*_tmp22E;if((_tmp22D.YY41).tag == 49){_LL1: _tmp22E=(_tmp22D.YY41).val;_LL2:
# 1182
 yyzzz=_tmp22E;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY41);}_LL0:;}
# 1186
return yyzzz;}
# 1188
union Cyc_YYSTYPE Cyc_YY41(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6E8;(_tmp6E8.YY41).tag=49U,(_tmp6E8.YY41).val=yy1;_tmp6E8;});}static char _tmp22F[13U]="designator_t";
# 1177 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY42={Cyc_Core_Failure,{_tmp22F,_tmp22F,_tmp22F + 13U}};
# 1179
void*Cyc_yyget_YY42(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp230=yy1;void*_tmp231;if((_tmp230.YY42).tag == 50){_LL1: _tmp231=(_tmp230.YY42).val;_LL2:
# 1183
 yyzzz=_tmp231;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY42);}_LL0:;}
# 1187
return yyzzz;}
# 1189
union Cyc_YYSTYPE Cyc_YY42(void*yy1){return({union Cyc_YYSTYPE _tmp6E9;(_tmp6E9.YY42).tag=50U,(_tmp6E9.YY42).val=yy1;_tmp6E9;});}static char _tmp232[7U]="kind_t";
# 1178 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY43={Cyc_Core_Failure,{_tmp232,_tmp232,_tmp232 + 7U}};
# 1180
struct Cyc_Absyn_Kind*Cyc_yyget_YY43(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Kind*yyzzz;
{union Cyc_YYSTYPE _tmp233=yy1;struct Cyc_Absyn_Kind*_tmp234;if((_tmp233.YY43).tag == 51){_LL1: _tmp234=(_tmp233.YY43).val;_LL2:
# 1184
 yyzzz=_tmp234;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY43);}_LL0:;}
# 1188
return yyzzz;}
# 1190
union Cyc_YYSTYPE Cyc_YY43(struct Cyc_Absyn_Kind*yy1){return({union Cyc_YYSTYPE _tmp6EA;(_tmp6EA.YY43).tag=51U,(_tmp6EA.YY43).val=yy1;_tmp6EA;});}static char _tmp235[7U]="type_t";
# 1179 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY44={Cyc_Core_Failure,{_tmp235,_tmp235,_tmp235 + 7U}};
# 1181
void*Cyc_yyget_YY44(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp236=yy1;void*_tmp237;if((_tmp236.YY44).tag == 52){_LL1: _tmp237=(_tmp236.YY44).val;_LL2:
# 1185
 yyzzz=_tmp237;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY44);}_LL0:;}
# 1189
return yyzzz;}
# 1191
union Cyc_YYSTYPE Cyc_YY44(void*yy1){return({union Cyc_YYSTYPE _tmp6EB;(_tmp6EB.YY44).tag=52U,(_tmp6EB.YY44).val=yy1;_tmp6EB;});}static char _tmp238[23U]="list_t<attribute_t,`H>";
# 1180 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY45={Cyc_Core_Failure,{_tmp238,_tmp238,_tmp238 + 23U}};
# 1182
struct Cyc_List_List*Cyc_yyget_YY45(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp239=yy1;struct Cyc_List_List*_tmp23A;if((_tmp239.YY45).tag == 53){_LL1: _tmp23A=(_tmp239.YY45).val;_LL2:
# 1186
 yyzzz=_tmp23A;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY45);}_LL0:;}
# 1190
return yyzzz;}
# 1192
union Cyc_YYSTYPE Cyc_YY45(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EC;(_tmp6EC.YY45).tag=53U,(_tmp6EC.YY45).val=yy1;_tmp6EC;});}static char _tmp23B[12U]="attribute_t";
# 1181 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY46={Cyc_Core_Failure,{_tmp23B,_tmp23B,_tmp23B + 12U}};
# 1183
void*Cyc_yyget_YY46(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp23C=yy1;void*_tmp23D;if((_tmp23C.YY46).tag == 54){_LL1: _tmp23D=(_tmp23C.YY46).val;_LL2:
# 1187
 yyzzz=_tmp23D;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY46);}_LL0:;}
# 1191
return yyzzz;}
# 1193
union Cyc_YYSTYPE Cyc_YY46(void*yy1){return({union Cyc_YYSTYPE _tmp6ED;(_tmp6ED.YY46).tag=54U,(_tmp6ED.YY46).val=yy1;_tmp6ED;});}static char _tmp23E[12U]="enumfield_t";
# 1182 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY47={Cyc_Core_Failure,{_tmp23E,_tmp23E,_tmp23E + 12U}};
# 1184
struct Cyc_Absyn_Enumfield*Cyc_yyget_YY47(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Enumfield*yyzzz;
{union Cyc_YYSTYPE _tmp23F=yy1;struct Cyc_Absyn_Enumfield*_tmp240;if((_tmp23F.YY47).tag == 55){_LL1: _tmp240=(_tmp23F.YY47).val;_LL2:
# 1188
 yyzzz=_tmp240;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY47);}_LL0:;}
# 1192
return yyzzz;}
# 1194
union Cyc_YYSTYPE Cyc_YY47(struct Cyc_Absyn_Enumfield*yy1){return({union Cyc_YYSTYPE _tmp6EE;(_tmp6EE.YY47).tag=55U,(_tmp6EE.YY47).val=yy1;_tmp6EE;});}static char _tmp241[23U]="list_t<enumfield_t,`H>";
# 1183 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY48={Cyc_Core_Failure,{_tmp241,_tmp241,_tmp241 + 23U}};
# 1185
struct Cyc_List_List*Cyc_yyget_YY48(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp242=yy1;struct Cyc_List_List*_tmp243;if((_tmp242.YY48).tag == 56){_LL1: _tmp243=(_tmp242.YY48).val;_LL2:
# 1189
 yyzzz=_tmp243;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY48);}_LL0:;}
# 1193
return yyzzz;}
# 1195
union Cyc_YYSTYPE Cyc_YY48(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6EF;(_tmp6EF.YY48).tag=56U,(_tmp6EF.YY48).val=yy1;_tmp6EF;});}static char _tmp244[11U]="type_opt_t";
# 1184 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY49={Cyc_Core_Failure,{_tmp244,_tmp244,_tmp244 + 11U}};
# 1186
void*Cyc_yyget_YY49(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp245=yy1;void*_tmp246;if((_tmp245.YY49).tag == 57){_LL1: _tmp246=(_tmp245.YY49).val;_LL2:
# 1190
 yyzzz=_tmp246;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY49);}_LL0:;}
# 1194
return yyzzz;}
# 1196
union Cyc_YYSTYPE Cyc_YY49(void*yy1){return({union Cyc_YYSTYPE _tmp6F0;(_tmp6F0.YY49).tag=57U,(_tmp6F0.YY49).val=yy1;_tmp6F0;});}static char _tmp247[31U]="list_t<$(type_t,type_t)@`H,`H>";
# 1185 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY50={Cyc_Core_Failure,{_tmp247,_tmp247,_tmp247 + 31U}};
# 1187
struct Cyc_List_List*Cyc_yyget_YY50(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp248=yy1;struct Cyc_List_List*_tmp249;if((_tmp248.YY50).tag == 58){_LL1: _tmp249=(_tmp248.YY50).val;_LL2:
# 1191
 yyzzz=_tmp249;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY50);}_LL0:;}
# 1195
return yyzzz;}
# 1197
union Cyc_YYSTYPE Cyc_YY50(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F1;(_tmp6F1.YY50).tag=58U,(_tmp6F1.YY50).val=yy1;_tmp6F1;});}static char _tmp24A[11U]="booltype_t";
# 1186 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY51={Cyc_Core_Failure,{_tmp24A,_tmp24A,_tmp24A + 11U}};
# 1188
void*Cyc_yyget_YY51(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp24B=yy1;void*_tmp24C;if((_tmp24B.YY51).tag == 59){_LL1: _tmp24C=(_tmp24B.YY51).val;_LL2:
# 1192
 yyzzz=_tmp24C;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY51);}_LL0:;}
# 1196
return yyzzz;}
# 1198
union Cyc_YYSTYPE Cyc_YY51(void*yy1){return({union Cyc_YYSTYPE _tmp6F2;(_tmp6F2.YY51).tag=59U,(_tmp6F2.YY51).val=yy1;_tmp6F2;});}static char _tmp24D[45U]="list_t<$(Position::seg_t,qvar_t,bool)@`H,`H>";
# 1187 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY52={Cyc_Core_Failure,{_tmp24D,_tmp24D,_tmp24D + 45U}};
# 1189
struct Cyc_List_List*Cyc_yyget_YY52(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp24E=yy1;struct Cyc_List_List*_tmp24F;if((_tmp24E.YY52).tag == 60){_LL1: _tmp24F=(_tmp24E.YY52).val;_LL2:
# 1193
 yyzzz=_tmp24F;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY52);}_LL0:;}
# 1197
return yyzzz;}
# 1199
union Cyc_YYSTYPE Cyc_YY52(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F3;(_tmp6F3.YY52).tag=60U,(_tmp6F3.YY52).val=yy1;_tmp6F3;});}static char _tmp250[20U]="pointer_qual_t<`yy>";
# 1188 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY53={Cyc_Core_Failure,{_tmp250,_tmp250,_tmp250 + 20U}};
# 1190
void*Cyc_yyget_YY53(union Cyc_YYSTYPE yy1){
void*yyzzz;
{union Cyc_YYSTYPE _tmp251=yy1;void*_tmp252;if((_tmp251.YY53).tag == 61){_LL1: _tmp252=(_tmp251.YY53).val;_LL2:
# 1194
 yyzzz=_tmp252;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY53);}_LL0:;}
# 1198
return yyzzz;}
# 1200
union Cyc_YYSTYPE Cyc_YY53(void*yy1){return({union Cyc_YYSTYPE _tmp6F4;(_tmp6F4.YY53).tag=61U,(_tmp6F4.YY53).val=yy1;_tmp6F4;});}static char _tmp253[21U]="pointer_quals_t<`yy>";
# 1189 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY54={Cyc_Core_Failure,{_tmp253,_tmp253,_tmp253 + 21U}};
# 1191
struct Cyc_List_List*Cyc_yyget_YY54(union Cyc_YYSTYPE yy1){
struct Cyc_List_List*yyzzz;
{union Cyc_YYSTYPE _tmp254=yy1;struct Cyc_List_List*_tmp255;if((_tmp254.YY54).tag == 62){_LL1: _tmp255=(_tmp254.YY54).val;_LL2:
# 1195
 yyzzz=_tmp255;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY54);}_LL0:;}
# 1199
return yyzzz;}
# 1201
union Cyc_YYSTYPE Cyc_YY54(struct Cyc_List_List*yy1){return({union Cyc_YYSTYPE _tmp6F5;(_tmp6F5.YY54).tag=62U,(_tmp6F5.YY54).val=yy1;_tmp6F5;});}static char _tmp256[21U]="$(bool,string_t<`H>)";
# 1190 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_Asm_tok={Cyc_Core_Failure,{_tmp256,_tmp256,_tmp256 + 21U}};
# 1192
struct _tuple20 Cyc_yyget_Asm_tok(union Cyc_YYSTYPE yy1){
struct _tuple20 yyzzz;
{union Cyc_YYSTYPE _tmp257=yy1;struct _tuple20 _tmp258;if((_tmp257.Asm_tok).tag == 6){_LL1: _tmp258=(_tmp257.Asm_tok).val;_LL2:
# 1196
 yyzzz=_tmp258;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_Asm_tok);}_LL0:;}
# 1200
return yyzzz;}
# 1202
union Cyc_YYSTYPE Cyc_Asm_tok(struct _tuple20 yy1){return({union Cyc_YYSTYPE _tmp6F6;(_tmp6F6.Asm_tok).tag=6U,(_tmp6F6.Asm_tok).val=yy1;_tmp6F6;});}static char _tmp259[10U]="exp_opt_t";
# 1191 "parse.y"
static struct Cyc_Core_Failure_exn_struct Cyc_yyfail_YY55={Cyc_Core_Failure,{_tmp259,_tmp259,_tmp259 + 10U}};
# 1193
struct Cyc_Absyn_Exp*Cyc_yyget_YY55(union Cyc_YYSTYPE yy1){
struct Cyc_Absyn_Exp*yyzzz;
{union Cyc_YYSTYPE _tmp25A=yy1;struct Cyc_Absyn_Exp*_tmp25B;if((_tmp25A.YY55).tag == 63){_LL1: _tmp25B=(_tmp25A.YY55).val;_LL2:
# 1197
 yyzzz=_tmp25B;
goto _LL0;}else{_LL3: _LL4:
(int)_throw((void*)& Cyc_yyfail_YY55);}_LL0:;}
# 1201
return yyzzz;}
# 1203
union Cyc_YYSTYPE Cyc_YY55(struct Cyc_Absyn_Exp*yy1){return({union Cyc_YYSTYPE _tmp6F7;(_tmp6F7.YY55).tag=63U,(_tmp6F7.YY55).val=yy1;_tmp6F7;});}struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 1219
struct Cyc_Yyltype Cyc_yynewloc(){
return({struct Cyc_Yyltype _tmp6F8;_tmp6F8.timestamp=0,_tmp6F8.first_line=0,_tmp6F8.first_column=0,_tmp6F8.last_line=0,_tmp6F8.last_column=0;_tmp6F8;});}
# 1222
struct Cyc_Yyltype Cyc_yylloc={0,0,0,0,0};
# 1233 "parse.y"
static short Cyc_yytranslate[376U]={0,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,147,2,2,131,145,142,2,128,129,136,139,124,143,133,144,2,2,2,2,2,2,2,2,2,2,132,121,126,125,127,138,137,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,134,2,135,141,130,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,122,140,123,146,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,61,62,63,64,65,66,67,68,69,70,71,72,73,74,75,76,77,78,79,80,81,82,83,84,85,86,87,88,89,90,91,92,93,94,95,96,97,98,99,100,101,102,103,104,105,106,107,108,109,110,111,112,113,114,115,116,117,118,119,120};static char _tmp25C[2U]="$";static char _tmp25D[6U]="error";static char _tmp25E[12U]="$undefined.";static char _tmp25F[5U]="AUTO";static char _tmp260[9U]="REGISTER";static char _tmp261[7U]="STATIC";static char _tmp262[7U]="EXTERN";static char _tmp263[8U]="TYPEDEF";static char _tmp264[5U]="VOID";static char _tmp265[5U]="CHAR";static char _tmp266[6U]="SHORT";static char _tmp267[4U]="INT";static char _tmp268[5U]="LONG";static char _tmp269[6U]="FLOAT";static char _tmp26A[7U]="DOUBLE";static char _tmp26B[7U]="SIGNED";static char _tmp26C[9U]="UNSIGNED";static char _tmp26D[6U]="CONST";static char _tmp26E[9U]="VOLATILE";static char _tmp26F[9U]="RESTRICT";static char _tmp270[7U]="STRUCT";static char _tmp271[6U]="UNION";static char _tmp272[5U]="CASE";static char _tmp273[8U]="DEFAULT";static char _tmp274[7U]="INLINE";static char _tmp275[7U]="SIZEOF";static char _tmp276[9U]="OFFSETOF";static char _tmp277[3U]="IF";static char _tmp278[5U]="ELSE";static char _tmp279[7U]="SWITCH";static char _tmp27A[6U]="WHILE";static char _tmp27B[3U]="DO";static char _tmp27C[4U]="FOR";static char _tmp27D[5U]="GOTO";static char _tmp27E[9U]="CONTINUE";static char _tmp27F[6U]="BREAK";static char _tmp280[7U]="RETURN";static char _tmp281[5U]="ENUM";static char _tmp282[7U]="TYPEOF";static char _tmp283[16U]="BUILTIN_VA_LIST";static char _tmp284[10U]="EXTENSION";static char _tmp285[8U]="NULL_kw";static char _tmp286[4U]="LET";static char _tmp287[6U]="THROW";static char _tmp288[4U]="TRY";static char _tmp289[6U]="CATCH";static char _tmp28A[7U]="EXPORT";static char _tmp28B[4U]="NEW";static char _tmp28C[9U]="ABSTRACT";static char _tmp28D[9U]="FALLTHRU";static char _tmp28E[6U]="USING";static char _tmp28F[10U]="NAMESPACE";static char _tmp290[9U]="DATATYPE";static char _tmp291[7U]="MALLOC";static char _tmp292[8U]="RMALLOC";static char _tmp293[15U]="RMALLOC_INLINE";static char _tmp294[7U]="CALLOC";static char _tmp295[8U]="RCALLOC";static char _tmp296[5U]="SWAP";static char _tmp297[9U]="REGION_T";static char _tmp298[6U]="TAG_T";static char _tmp299[7U]="REGION";static char _tmp29A[5U]="RNEW";static char _tmp29B[8U]="REGIONS";static char _tmp29C[7U]="PORTON";static char _tmp29D[8U]="PORTOFF";static char _tmp29E[7U]="PRAGMA";static char _tmp29F[12U]="DYNREGION_T";static char _tmp2A0[8U]="NUMELTS";static char _tmp2A1[8U]="VALUEOF";static char _tmp2A2[10U]="VALUEOF_T";static char _tmp2A3[9U]="TAGCHECK";static char _tmp2A4[13U]="NUMELTS_QUAL";static char _tmp2A5[10U]="THIN_QUAL";static char _tmp2A6[9U]="FAT_QUAL";static char _tmp2A7[13U]="NOTNULL_QUAL";static char _tmp2A8[14U]="NULLABLE_QUAL";static char _tmp2A9[14U]="REQUIRES_QUAL";static char _tmp2AA[13U]="ENSURES_QUAL";static char _tmp2AB[12U]="REGION_QUAL";static char _tmp2AC[16U]="NOZEROTERM_QUAL";static char _tmp2AD[14U]="ZEROTERM_QUAL";static char _tmp2AE[12U]="TAGGED_QUAL";static char _tmp2AF[16U]="EXTENSIBLE_QUAL";static char _tmp2B0[7U]="PTR_OP";static char _tmp2B1[7U]="INC_OP";static char _tmp2B2[7U]="DEC_OP";static char _tmp2B3[8U]="LEFT_OP";static char _tmp2B4[9U]="RIGHT_OP";static char _tmp2B5[6U]="LE_OP";static char _tmp2B6[6U]="GE_OP";static char _tmp2B7[6U]="EQ_OP";static char _tmp2B8[6U]="NE_OP";static char _tmp2B9[7U]="AND_OP";static char _tmp2BA[6U]="OR_OP";static char _tmp2BB[11U]="MUL_ASSIGN";static char _tmp2BC[11U]="DIV_ASSIGN";static char _tmp2BD[11U]="MOD_ASSIGN";static char _tmp2BE[11U]="ADD_ASSIGN";static char _tmp2BF[11U]="SUB_ASSIGN";static char _tmp2C0[12U]="LEFT_ASSIGN";static char _tmp2C1[13U]="RIGHT_ASSIGN";static char _tmp2C2[11U]="AND_ASSIGN";static char _tmp2C3[11U]="XOR_ASSIGN";static char _tmp2C4[10U]="OR_ASSIGN";static char _tmp2C5[9U]="ELLIPSIS";static char _tmp2C6[11U]="LEFT_RIGHT";static char _tmp2C7[12U]="COLON_COLON";static char _tmp2C8[11U]="IDENTIFIER";static char _tmp2C9[17U]="INTEGER_CONSTANT";static char _tmp2CA[7U]="STRING";static char _tmp2CB[8U]="WSTRING";static char _tmp2CC[19U]="CHARACTER_CONSTANT";static char _tmp2CD[20U]="WCHARACTER_CONSTANT";static char _tmp2CE[18U]="FLOATING_CONSTANT";static char _tmp2CF[9U]="TYPE_VAR";static char _tmp2D0[13U]="TYPEDEF_NAME";static char _tmp2D1[16U]="QUAL_IDENTIFIER";static char _tmp2D2[18U]="QUAL_TYPEDEF_NAME";static char _tmp2D3[10U]="ATTRIBUTE";static char _tmp2D4[4U]="ASM";static char _tmp2D5[4U]="';'";static char _tmp2D6[4U]="'{'";static char _tmp2D7[4U]="'}'";static char _tmp2D8[4U]="','";static char _tmp2D9[4U]="'='";static char _tmp2DA[4U]="'<'";static char _tmp2DB[4U]="'>'";static char _tmp2DC[4U]="'('";static char _tmp2DD[4U]="')'";static char _tmp2DE[4U]="'_'";static char _tmp2DF[4U]="'$'";static char _tmp2E0[4U]="':'";static char _tmp2E1[4U]="'.'";static char _tmp2E2[4U]="'['";static char _tmp2E3[4U]="']'";static char _tmp2E4[4U]="'*'";static char _tmp2E5[4U]="'@'";static char _tmp2E6[4U]="'?'";static char _tmp2E7[4U]="'+'";static char _tmp2E8[4U]="'|'";static char _tmp2E9[4U]="'^'";static char _tmp2EA[4U]="'&'";static char _tmp2EB[4U]="'-'";static char _tmp2EC[4U]="'/'";static char _tmp2ED[4U]="'%'";static char _tmp2EE[4U]="'~'";static char _tmp2EF[4U]="'!'";static char _tmp2F0[5U]="prog";static char _tmp2F1[17U]="translation_unit";static char _tmp2F2[16U]="extern_c_action";static char _tmp2F3[13U]="end_extern_c";static char _tmp2F4[12U]="export_list";static char _tmp2F5[19U]="export_list_values";static char _tmp2F6[21U]="external_declaration";static char _tmp2F7[15U]="optional_comma";static char _tmp2F8[20U]="function_definition";static char _tmp2F9[21U]="function_definition2";static char _tmp2FA[13U]="using_action";static char _tmp2FB[15U]="unusing_action";static char _tmp2FC[17U]="namespace_action";static char _tmp2FD[19U]="unnamespace_action";static char _tmp2FE[12U]="declaration";static char _tmp2FF[17U]="declaration_list";static char _tmp300[23U]="declaration_specifiers";static char _tmp301[24U]="storage_class_specifier";static char _tmp302[15U]="attributes_opt";static char _tmp303[11U]="attributes";static char _tmp304[15U]="attribute_list";static char _tmp305[10U]="attribute";static char _tmp306[15U]="type_specifier";static char _tmp307[25U]="type_specifier_notypedef";static char _tmp308[5U]="kind";static char _tmp309[15U]="type_qualifier";static char _tmp30A[15U]="enum_specifier";static char _tmp30B[11U]="enum_field";static char _tmp30C[22U]="enum_declaration_list";static char _tmp30D[26U]="struct_or_union_specifier";static char _tmp30E[16U]="type_params_opt";static char _tmp30F[16U]="struct_or_union";static char _tmp310[24U]="struct_declaration_list";static char _tmp311[25U]="struct_declaration_list0";static char _tmp312[21U]="init_declarator_list";static char _tmp313[22U]="init_declarator_list0";static char _tmp314[16U]="init_declarator";static char _tmp315[19U]="struct_declaration";static char _tmp316[25U]="specifier_qualifier_list";static char _tmp317[35U]="notypedef_specifier_qualifier_list";static char _tmp318[23U]="struct_declarator_list";static char _tmp319[24U]="struct_declarator_list0";static char _tmp31A[18U]="struct_declarator";static char _tmp31B[20U]="requires_clause_opt";static char _tmp31C[19U]="ensures_clause_opt";static char _tmp31D[19U]="datatype_specifier";static char _tmp31E[14U]="qual_datatype";static char _tmp31F[19U]="datatypefield_list";static char _tmp320[20U]="datatypefield_scope";static char _tmp321[14U]="datatypefield";static char _tmp322[11U]="declarator";static char _tmp323[23U]="declarator_withtypedef";static char _tmp324[18U]="direct_declarator";static char _tmp325[30U]="direct_declarator_withtypedef";static char _tmp326[8U]="pointer";static char _tmp327[12U]="one_pointer";static char _tmp328[14U]="pointer_quals";static char _tmp329[13U]="pointer_qual";static char _tmp32A[23U]="pointer_null_and_bound";static char _tmp32B[14U]="pointer_bound";static char _tmp32C[18U]="zeroterm_qual_opt";static char _tmp32D[8U]="rgn_opt";static char _tmp32E[11U]="tqual_list";static char _tmp32F[20U]="parameter_type_list";static char _tmp330[9U]="type_var";static char _tmp331[16U]="optional_effect";static char _tmp332[19U]="optional_rgn_order";static char _tmp333[10U]="rgn_order";static char _tmp334[16U]="optional_inject";static char _tmp335[11U]="effect_set";static char _tmp336[14U]="atomic_effect";static char _tmp337[11U]="region_set";static char _tmp338[15U]="parameter_list";static char _tmp339[22U]="parameter_declaration";static char _tmp33A[16U]="identifier_list";static char _tmp33B[17U]="identifier_list0";static char _tmp33C[12U]="initializer";static char _tmp33D[18U]="array_initializer";static char _tmp33E[17U]="initializer_list";static char _tmp33F[12U]="designation";static char _tmp340[16U]="designator_list";static char _tmp341[11U]="designator";static char _tmp342[10U]="type_name";static char _tmp343[14U]="any_type_name";static char _tmp344[15U]="type_name_list";static char _tmp345[20U]="abstract_declarator";static char _tmp346[27U]="direct_abstract_declarator";static char _tmp347[10U]="statement";static char _tmp348[18U]="labeled_statement";static char _tmp349[21U]="expression_statement";static char _tmp34A[19U]="compound_statement";static char _tmp34B[16U]="block_item_list";static char _tmp34C[20U]="selection_statement";static char _tmp34D[15U]="switch_clauses";static char _tmp34E[20U]="iteration_statement";static char _tmp34F[15U]="jump_statement";static char _tmp350[12U]="exp_pattern";static char _tmp351[20U]="conditional_pattern";static char _tmp352[19U]="logical_or_pattern";static char _tmp353[20U]="logical_and_pattern";static char _tmp354[21U]="inclusive_or_pattern";static char _tmp355[21U]="exclusive_or_pattern";static char _tmp356[12U]="and_pattern";static char _tmp357[17U]="equality_pattern";static char _tmp358[19U]="relational_pattern";static char _tmp359[14U]="shift_pattern";static char _tmp35A[17U]="additive_pattern";static char _tmp35B[23U]="multiplicative_pattern";static char _tmp35C[13U]="cast_pattern";static char _tmp35D[14U]="unary_pattern";static char _tmp35E[16U]="postfix_pattern";static char _tmp35F[16U]="primary_pattern";static char _tmp360[8U]="pattern";static char _tmp361[19U]="tuple_pattern_list";static char _tmp362[20U]="tuple_pattern_list0";static char _tmp363[14U]="field_pattern";static char _tmp364[19U]="field_pattern_list";static char _tmp365[20U]="field_pattern_list0";static char _tmp366[11U]="expression";static char _tmp367[22U]="assignment_expression";static char _tmp368[20U]="assignment_operator";static char _tmp369[23U]="conditional_expression";static char _tmp36A[20U]="constant_expression";static char _tmp36B[22U]="logical_or_expression";static char _tmp36C[23U]="logical_and_expression";static char _tmp36D[24U]="inclusive_or_expression";static char _tmp36E[24U]="exclusive_or_expression";static char _tmp36F[15U]="and_expression";static char _tmp370[20U]="equality_expression";static char _tmp371[22U]="relational_expression";static char _tmp372[17U]="shift_expression";static char _tmp373[20U]="additive_expression";static char _tmp374[26U]="multiplicative_expression";static char _tmp375[16U]="cast_expression";static char _tmp376[17U]="unary_expression";static char _tmp377[15U]="unary_operator";static char _tmp378[19U]="postfix_expression";static char _tmp379[17U]="field_expression";static char _tmp37A[19U]="primary_expression";static char _tmp37B[25U]="argument_expression_list";static char _tmp37C[26U]="argument_expression_list0";static char _tmp37D[9U]="constant";static char _tmp37E[20U]="qual_opt_identifier";static char _tmp37F[17U]="qual_opt_typedef";static char _tmp380[18U]="struct_union_name";static char _tmp381[11U]="field_name";static char _tmp382[12U]="right_angle";
# 1587 "parse.y"
static struct _dyneither_ptr Cyc_yytname[295U]={{_tmp25C,_tmp25C,_tmp25C + 2U},{_tmp25D,_tmp25D,_tmp25D + 6U},{_tmp25E,_tmp25E,_tmp25E + 12U},{_tmp25F,_tmp25F,_tmp25F + 5U},{_tmp260,_tmp260,_tmp260 + 9U},{_tmp261,_tmp261,_tmp261 + 7U},{_tmp262,_tmp262,_tmp262 + 7U},{_tmp263,_tmp263,_tmp263 + 8U},{_tmp264,_tmp264,_tmp264 + 5U},{_tmp265,_tmp265,_tmp265 + 5U},{_tmp266,_tmp266,_tmp266 + 6U},{_tmp267,_tmp267,_tmp267 + 4U},{_tmp268,_tmp268,_tmp268 + 5U},{_tmp269,_tmp269,_tmp269 + 6U},{_tmp26A,_tmp26A,_tmp26A + 7U},{_tmp26B,_tmp26B,_tmp26B + 7U},{_tmp26C,_tmp26C,_tmp26C + 9U},{_tmp26D,_tmp26D,_tmp26D + 6U},{_tmp26E,_tmp26E,_tmp26E + 9U},{_tmp26F,_tmp26F,_tmp26F + 9U},{_tmp270,_tmp270,_tmp270 + 7U},{_tmp271,_tmp271,_tmp271 + 6U},{_tmp272,_tmp272,_tmp272 + 5U},{_tmp273,_tmp273,_tmp273 + 8U},{_tmp274,_tmp274,_tmp274 + 7U},{_tmp275,_tmp275,_tmp275 + 7U},{_tmp276,_tmp276,_tmp276 + 9U},{_tmp277,_tmp277,_tmp277 + 3U},{_tmp278,_tmp278,_tmp278 + 5U},{_tmp279,_tmp279,_tmp279 + 7U},{_tmp27A,_tmp27A,_tmp27A + 6U},{_tmp27B,_tmp27B,_tmp27B + 3U},{_tmp27C,_tmp27C,_tmp27C + 4U},{_tmp27D,_tmp27D,_tmp27D + 5U},{_tmp27E,_tmp27E,_tmp27E + 9U},{_tmp27F,_tmp27F,_tmp27F + 6U},{_tmp280,_tmp280,_tmp280 + 7U},{_tmp281,_tmp281,_tmp281 + 5U},{_tmp282,_tmp282,_tmp282 + 7U},{_tmp283,_tmp283,_tmp283 + 16U},{_tmp284,_tmp284,_tmp284 + 10U},{_tmp285,_tmp285,_tmp285 + 8U},{_tmp286,_tmp286,_tmp286 + 4U},{_tmp287,_tmp287,_tmp287 + 6U},{_tmp288,_tmp288,_tmp288 + 4U},{_tmp289,_tmp289,_tmp289 + 6U},{_tmp28A,_tmp28A,_tmp28A + 7U},{_tmp28B,_tmp28B,_tmp28B + 4U},{_tmp28C,_tmp28C,_tmp28C + 9U},{_tmp28D,_tmp28D,_tmp28D + 9U},{_tmp28E,_tmp28E,_tmp28E + 6U},{_tmp28F,_tmp28F,_tmp28F + 10U},{_tmp290,_tmp290,_tmp290 + 9U},{_tmp291,_tmp291,_tmp291 + 7U},{_tmp292,_tmp292,_tmp292 + 8U},{_tmp293,_tmp293,_tmp293 + 15U},{_tmp294,_tmp294,_tmp294 + 7U},{_tmp295,_tmp295,_tmp295 + 8U},{_tmp296,_tmp296,_tmp296 + 5U},{_tmp297,_tmp297,_tmp297 + 9U},{_tmp298,_tmp298,_tmp298 + 6U},{_tmp299,_tmp299,_tmp299 + 7U},{_tmp29A,_tmp29A,_tmp29A + 5U},{_tmp29B,_tmp29B,_tmp29B + 8U},{_tmp29C,_tmp29C,_tmp29C + 7U},{_tmp29D,_tmp29D,_tmp29D + 8U},{_tmp29E,_tmp29E,_tmp29E + 7U},{_tmp29F,_tmp29F,_tmp29F + 12U},{_tmp2A0,_tmp2A0,_tmp2A0 + 8U},{_tmp2A1,_tmp2A1,_tmp2A1 + 8U},{_tmp2A2,_tmp2A2,_tmp2A2 + 10U},{_tmp2A3,_tmp2A3,_tmp2A3 + 9U},{_tmp2A4,_tmp2A4,_tmp2A4 + 13U},{_tmp2A5,_tmp2A5,_tmp2A5 + 10U},{_tmp2A6,_tmp2A6,_tmp2A6 + 9U},{_tmp2A7,_tmp2A7,_tmp2A7 + 13U},{_tmp2A8,_tmp2A8,_tmp2A8 + 14U},{_tmp2A9,_tmp2A9,_tmp2A9 + 14U},{_tmp2AA,_tmp2AA,_tmp2AA + 13U},{_tmp2AB,_tmp2AB,_tmp2AB + 12U},{_tmp2AC,_tmp2AC,_tmp2AC + 16U},{_tmp2AD,_tmp2AD,_tmp2AD + 14U},{_tmp2AE,_tmp2AE,_tmp2AE + 12U},{_tmp2AF,_tmp2AF,_tmp2AF + 16U},{_tmp2B0,_tmp2B0,_tmp2B0 + 7U},{_tmp2B1,_tmp2B1,_tmp2B1 + 7U},{_tmp2B2,_tmp2B2,_tmp2B2 + 7U},{_tmp2B3,_tmp2B3,_tmp2B3 + 8U},{_tmp2B4,_tmp2B4,_tmp2B4 + 9U},{_tmp2B5,_tmp2B5,_tmp2B5 + 6U},{_tmp2B6,_tmp2B6,_tmp2B6 + 6U},{_tmp2B7,_tmp2B7,_tmp2B7 + 6U},{_tmp2B8,_tmp2B8,_tmp2B8 + 6U},{_tmp2B9,_tmp2B9,_tmp2B9 + 7U},{_tmp2BA,_tmp2BA,_tmp2BA + 6U},{_tmp2BB,_tmp2BB,_tmp2BB + 11U},{_tmp2BC,_tmp2BC,_tmp2BC + 11U},{_tmp2BD,_tmp2BD,_tmp2BD + 11U},{_tmp2BE,_tmp2BE,_tmp2BE + 11U},{_tmp2BF,_tmp2BF,_tmp2BF + 11U},{_tmp2C0,_tmp2C0,_tmp2C0 + 12U},{_tmp2C1,_tmp2C1,_tmp2C1 + 13U},{_tmp2C2,_tmp2C2,_tmp2C2 + 11U},{_tmp2C3,_tmp2C3,_tmp2C3 + 11U},{_tmp2C4,_tmp2C4,_tmp2C4 + 10U},{_tmp2C5,_tmp2C5,_tmp2C5 + 9U},{_tmp2C6,_tmp2C6,_tmp2C6 + 11U},{_tmp2C7,_tmp2C7,_tmp2C7 + 12U},{_tmp2C8,_tmp2C8,_tmp2C8 + 11U},{_tmp2C9,_tmp2C9,_tmp2C9 + 17U},{_tmp2CA,_tmp2CA,_tmp2CA + 7U},{_tmp2CB,_tmp2CB,_tmp2CB + 8U},{_tmp2CC,_tmp2CC,_tmp2CC + 19U},{_tmp2CD,_tmp2CD,_tmp2CD + 20U},{_tmp2CE,_tmp2CE,_tmp2CE + 18U},{_tmp2CF,_tmp2CF,_tmp2CF + 9U},{_tmp2D0,_tmp2D0,_tmp2D0 + 13U},{_tmp2D1,_tmp2D1,_tmp2D1 + 16U},{_tmp2D2,_tmp2D2,_tmp2D2 + 18U},{_tmp2D3,_tmp2D3,_tmp2D3 + 10U},{_tmp2D4,_tmp2D4,_tmp2D4 + 4U},{_tmp2D5,_tmp2D5,_tmp2D5 + 4U},{_tmp2D6,_tmp2D6,_tmp2D6 + 4U},{_tmp2D7,_tmp2D7,_tmp2D7 + 4U},{_tmp2D8,_tmp2D8,_tmp2D8 + 4U},{_tmp2D9,_tmp2D9,_tmp2D9 + 4U},{_tmp2DA,_tmp2DA,_tmp2DA + 4U},{_tmp2DB,_tmp2DB,_tmp2DB + 4U},{_tmp2DC,_tmp2DC,_tmp2DC + 4U},{_tmp2DD,_tmp2DD,_tmp2DD + 4U},{_tmp2DE,_tmp2DE,_tmp2DE + 4U},{_tmp2DF,_tmp2DF,_tmp2DF + 4U},{_tmp2E0,_tmp2E0,_tmp2E0 + 4U},{_tmp2E1,_tmp2E1,_tmp2E1 + 4U},{_tmp2E2,_tmp2E2,_tmp2E2 + 4U},{_tmp2E3,_tmp2E3,_tmp2E3 + 4U},{_tmp2E4,_tmp2E4,_tmp2E4 + 4U},{_tmp2E5,_tmp2E5,_tmp2E5 + 4U},{_tmp2E6,_tmp2E6,_tmp2E6 + 4U},{_tmp2E7,_tmp2E7,_tmp2E7 + 4U},{_tmp2E8,_tmp2E8,_tmp2E8 + 4U},{_tmp2E9,_tmp2E9,_tmp2E9 + 4U},{_tmp2EA,_tmp2EA,_tmp2EA + 4U},{_tmp2EB,_tmp2EB,_tmp2EB + 4U},{_tmp2EC,_tmp2EC,_tmp2EC + 4U},{_tmp2ED,_tmp2ED,_tmp2ED + 4U},{_tmp2EE,_tmp2EE,_tmp2EE + 4U},{_tmp2EF,_tmp2EF,_tmp2EF + 4U},{_tmp2F0,_tmp2F0,_tmp2F0 + 5U},{_tmp2F1,_tmp2F1,_tmp2F1 + 17U},{_tmp2F2,_tmp2F2,_tmp2F2 + 16U},{_tmp2F3,_tmp2F3,_tmp2F3 + 13U},{_tmp2F4,_tmp2F4,_tmp2F4 + 12U},{_tmp2F5,_tmp2F5,_tmp2F5 + 19U},{_tmp2F6,_tmp2F6,_tmp2F6 + 21U},{_tmp2F7,_tmp2F7,_tmp2F7 + 15U},{_tmp2F8,_tmp2F8,_tmp2F8 + 20U},{_tmp2F9,_tmp2F9,_tmp2F9 + 21U},{_tmp2FA,_tmp2FA,_tmp2FA + 13U},{_tmp2FB,_tmp2FB,_tmp2FB + 15U},{_tmp2FC,_tmp2FC,_tmp2FC + 17U},{_tmp2FD,_tmp2FD,_tmp2FD + 19U},{_tmp2FE,_tmp2FE,_tmp2FE + 12U},{_tmp2FF,_tmp2FF,_tmp2FF + 17U},{_tmp300,_tmp300,_tmp300 + 23U},{_tmp301,_tmp301,_tmp301 + 24U},{_tmp302,_tmp302,_tmp302 + 15U},{_tmp303,_tmp303,_tmp303 + 11U},{_tmp304,_tmp304,_tmp304 + 15U},{_tmp305,_tmp305,_tmp305 + 10U},{_tmp306,_tmp306,_tmp306 + 15U},{_tmp307,_tmp307,_tmp307 + 25U},{_tmp308,_tmp308,_tmp308 + 5U},{_tmp309,_tmp309,_tmp309 + 15U},{_tmp30A,_tmp30A,_tmp30A + 15U},{_tmp30B,_tmp30B,_tmp30B + 11U},{_tmp30C,_tmp30C,_tmp30C + 22U},{_tmp30D,_tmp30D,_tmp30D + 26U},{_tmp30E,_tmp30E,_tmp30E + 16U},{_tmp30F,_tmp30F,_tmp30F + 16U},{_tmp310,_tmp310,_tmp310 + 24U},{_tmp311,_tmp311,_tmp311 + 25U},{_tmp312,_tmp312,_tmp312 + 21U},{_tmp313,_tmp313,_tmp313 + 22U},{_tmp314,_tmp314,_tmp314 + 16U},{_tmp315,_tmp315,_tmp315 + 19U},{_tmp316,_tmp316,_tmp316 + 25U},{_tmp317,_tmp317,_tmp317 + 35U},{_tmp318,_tmp318,_tmp318 + 23U},{_tmp319,_tmp319,_tmp319 + 24U},{_tmp31A,_tmp31A,_tmp31A + 18U},{_tmp31B,_tmp31B,_tmp31B + 20U},{_tmp31C,_tmp31C,_tmp31C + 19U},{_tmp31D,_tmp31D,_tmp31D + 19U},{_tmp31E,_tmp31E,_tmp31E + 14U},{_tmp31F,_tmp31F,_tmp31F + 19U},{_tmp320,_tmp320,_tmp320 + 20U},{_tmp321,_tmp321,_tmp321 + 14U},{_tmp322,_tmp322,_tmp322 + 11U},{_tmp323,_tmp323,_tmp323 + 23U},{_tmp324,_tmp324,_tmp324 + 18U},{_tmp325,_tmp325,_tmp325 + 30U},{_tmp326,_tmp326,_tmp326 + 8U},{_tmp327,_tmp327,_tmp327 + 12U},{_tmp328,_tmp328,_tmp328 + 14U},{_tmp329,_tmp329,_tmp329 + 13U},{_tmp32A,_tmp32A,_tmp32A + 23U},{_tmp32B,_tmp32B,_tmp32B + 14U},{_tmp32C,_tmp32C,_tmp32C + 18U},{_tmp32D,_tmp32D,_tmp32D + 8U},{_tmp32E,_tmp32E,_tmp32E + 11U},{_tmp32F,_tmp32F,_tmp32F + 20U},{_tmp330,_tmp330,_tmp330 + 9U},{_tmp331,_tmp331,_tmp331 + 16U},{_tmp332,_tmp332,_tmp332 + 19U},{_tmp333,_tmp333,_tmp333 + 10U},{_tmp334,_tmp334,_tmp334 + 16U},{_tmp335,_tmp335,_tmp335 + 11U},{_tmp336,_tmp336,_tmp336 + 14U},{_tmp337,_tmp337,_tmp337 + 11U},{_tmp338,_tmp338,_tmp338 + 15U},{_tmp339,_tmp339,_tmp339 + 22U},{_tmp33A,_tmp33A,_tmp33A + 16U},{_tmp33B,_tmp33B,_tmp33B + 17U},{_tmp33C,_tmp33C,_tmp33C + 12U},{_tmp33D,_tmp33D,_tmp33D + 18U},{_tmp33E,_tmp33E,_tmp33E + 17U},{_tmp33F,_tmp33F,_tmp33F + 12U},{_tmp340,_tmp340,_tmp340 + 16U},{_tmp341,_tmp341,_tmp341 + 11U},{_tmp342,_tmp342,_tmp342 + 10U},{_tmp343,_tmp343,_tmp343 + 14U},{_tmp344,_tmp344,_tmp344 + 15U},{_tmp345,_tmp345,_tmp345 + 20U},{_tmp346,_tmp346,_tmp346 + 27U},{_tmp347,_tmp347,_tmp347 + 10U},{_tmp348,_tmp348,_tmp348 + 18U},{_tmp349,_tmp349,_tmp349 + 21U},{_tmp34A,_tmp34A,_tmp34A + 19U},{_tmp34B,_tmp34B,_tmp34B + 16U},{_tmp34C,_tmp34C,_tmp34C + 20U},{_tmp34D,_tmp34D,_tmp34D + 15U},{_tmp34E,_tmp34E,_tmp34E + 20U},{_tmp34F,_tmp34F,_tmp34F + 15U},{_tmp350,_tmp350,_tmp350 + 12U},{_tmp351,_tmp351,_tmp351 + 20U},{_tmp352,_tmp352,_tmp352 + 19U},{_tmp353,_tmp353,_tmp353 + 20U},{_tmp354,_tmp354,_tmp354 + 21U},{_tmp355,_tmp355,_tmp355 + 21U},{_tmp356,_tmp356,_tmp356 + 12U},{_tmp357,_tmp357,_tmp357 + 17U},{_tmp358,_tmp358,_tmp358 + 19U},{_tmp359,_tmp359,_tmp359 + 14U},{_tmp35A,_tmp35A,_tmp35A + 17U},{_tmp35B,_tmp35B,_tmp35B + 23U},{_tmp35C,_tmp35C,_tmp35C + 13U},{_tmp35D,_tmp35D,_tmp35D + 14U},{_tmp35E,_tmp35E,_tmp35E + 16U},{_tmp35F,_tmp35F,_tmp35F + 16U},{_tmp360,_tmp360,_tmp360 + 8U},{_tmp361,_tmp361,_tmp361 + 19U},{_tmp362,_tmp362,_tmp362 + 20U},{_tmp363,_tmp363,_tmp363 + 14U},{_tmp364,_tmp364,_tmp364 + 19U},{_tmp365,_tmp365,_tmp365 + 20U},{_tmp366,_tmp366,_tmp366 + 11U},{_tmp367,_tmp367,_tmp367 + 22U},{_tmp368,_tmp368,_tmp368 + 20U},{_tmp369,_tmp369,_tmp369 + 23U},{_tmp36A,_tmp36A,_tmp36A + 20U},{_tmp36B,_tmp36B,_tmp36B + 22U},{_tmp36C,_tmp36C,_tmp36C + 23U},{_tmp36D,_tmp36D,_tmp36D + 24U},{_tmp36E,_tmp36E,_tmp36E + 24U},{_tmp36F,_tmp36F,_tmp36F + 15U},{_tmp370,_tmp370,_tmp370 + 20U},{_tmp371,_tmp371,_tmp371 + 22U},{_tmp372,_tmp372,_tmp372 + 17U},{_tmp373,_tmp373,_tmp373 + 20U},{_tmp374,_tmp374,_tmp374 + 26U},{_tmp375,_tmp375,_tmp375 + 16U},{_tmp376,_tmp376,_tmp376 + 17U},{_tmp377,_tmp377,_tmp377 + 15U},{_tmp378,_tmp378,_tmp378 + 19U},{_tmp379,_tmp379,_tmp379 + 17U},{_tmp37A,_tmp37A,_tmp37A + 19U},{_tmp37B,_tmp37B,_tmp37B + 25U},{_tmp37C,_tmp37C,_tmp37C + 26U},{_tmp37D,_tmp37D,_tmp37D + 9U},{_tmp37E,_tmp37E,_tmp37E + 20U},{_tmp37F,_tmp37F,_tmp37F + 17U},{_tmp380,_tmp380,_tmp380 + 18U},{_tmp381,_tmp381,_tmp381 + 11U},{_tmp382,_tmp382,_tmp382 + 12U}};
# 1642
static short Cyc_yyr1[529U]={0,148,149,149,149,149,149,149,149,149,149,149,150,151,152,152,153,153,153,154,154,154,155,155,156,156,156,156,157,157,158,159,160,161,162,162,162,162,162,162,162,163,163,164,164,164,164,164,164,164,164,164,164,164,165,165,165,165,165,165,165,166,166,167,168,168,169,169,169,169,170,170,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,171,172,173,173,173,174,174,174,175,175,176,176,176,177,177,177,177,177,178,178,179,179,180,180,181,181,182,183,183,184,184,185,186,186,186,186,186,186,187,187,187,187,187,187,188,189,189,190,190,190,190,191,191,192,192,193,193,193,194,194,195,195,195,195,196,196,196,197,197,198,198,199,199,200,200,200,200,200,200,200,200,200,200,201,201,201,201,201,201,201,201,201,201,201,202,202,203,204,204,205,205,205,205,205,205,205,205,206,206,206,207,207,208,208,208,209,209,209,210,210,211,211,211,211,212,212,213,213,214,214,215,215,216,216,217,217,218,218,218,218,219,219,220,220,221,221,221,222,223,223,224,224,225,225,225,225,225,226,226,226,226,227,227,228,228,229,229,230,230,231,231,231,231,231,232,232,233,233,233,234,234,234,234,234,234,234,234,234,234,234,235,235,235,235,235,235,236,237,237,238,238,239,239,239,239,239,239,239,239,240,240,240,240,240,240,241,241,241,241,241,241,242,242,242,242,242,242,242,242,242,242,242,242,242,242,243,243,243,243,243,243,243,243,244,245,245,246,246,247,247,248,248,249,249,250,250,251,251,251,252,252,252,252,252,253,253,253,254,254,254,255,255,255,255,256,256,257,257,257,257,257,257,258,259,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,260,261,261,261,262,262,263,263,264,264,264,265,265,266,266,267,267,267,268,268,268,268,268,268,268,268,268,268,268,269,269,269,269,269,269,269,270,271,271,272,272,273,273,274,274,275,275,276,276,276,277,277,277,277,277,278,278,278,279,279,279,280,280,280,280,281,281,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,282,283,283,283,284,284,284,284,284,284,284,284,284,284,284,285,285,285,285,286,286,286,286,286,286,286,286,286,286,286,287,288,288,289,289,289,289,289,290,290,291,291,292,292,293,293,294,294};
# 1698
static short Cyc_yyr2[529U]={0,1,2,3,5,3,5,5,6,3,3,0,2,1,4,3,1,2,3,1,1,1,1,0,3,4,4,5,3,4,2,1,2,1,2,3,5,3,6,3,8,1,2,1,2,2,1,2,1,2,1,2,1,2,1,1,1,1,2,1,1,0,1,6,1,3,1,1,4,8,1,2,1,1,1,1,1,1,1,1,1,1,1,4,1,1,1,1,3,4,4,1,4,1,4,1,1,1,1,5,2,4,1,3,1,2,3,4,9,8,4,3,0,3,1,1,0,1,1,2,1,1,3,1,3,3,1,2,1,2,1,2,1,2,1,2,1,2,1,1,3,2,2,0,3,4,0,4,0,6,3,5,1,2,1,2,3,3,0,1,1,2,5,1,2,1,2,1,3,4,4,5,6,7,4,4,2,1,1,3,4,4,5,6,7,4,4,2,1,2,5,0,2,4,4,1,1,1,1,1,1,2,2,1,0,3,0,1,1,0,1,1,0,2,3,5,5,7,1,3,0,2,0,2,3,5,0,1,1,3,2,3,4,1,1,3,1,3,2,1,2,1,1,3,1,1,2,3,4,8,8,1,2,3,4,2,2,1,2,3,2,1,2,1,2,3,4,3,1,3,1,1,2,3,3,4,4,5,6,5,7,6,4,2,1,1,1,1,1,1,3,1,2,2,3,1,2,3,4,1,2,1,2,5,7,7,5,8,6,0,4,4,5,6,7,5,7,6,7,7,8,7,8,8,9,6,7,7,8,3,2,2,2,3,2,4,5,1,1,5,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,4,2,6,1,1,1,3,1,1,3,6,6,4,4,5,4,2,2,2,4,4,4,1,3,1,1,3,1,2,1,3,1,1,3,1,3,1,3,3,1,1,1,1,1,1,1,1,1,1,1,1,5,2,2,2,5,5,1,1,3,1,3,1,3,1,3,1,3,1,3,3,1,3,3,3,3,1,3,3,1,3,3,1,3,3,3,1,4,1,2,2,2,2,2,2,4,2,6,4,6,6,9,11,4,6,6,4,1,2,1,1,1,1,4,3,4,3,3,2,2,5,6,7,1,1,3,3,1,4,1,1,1,3,2,5,4,5,5,1,1,3,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1};
# 1754
static short Cyc_yydefact[1079U]={0,21,54,55,56,57,59,72,73,74,75,76,77,78,79,80,96,97,98,114,115,50,0,0,84,0,0,60,0,0,152,91,93,0,0,0,0,0,0,519,218,521,520,522,0,0,87,0,204,204,203,1,0,0,19,0,0,20,0,43,52,46,70,48,81,82,0,85,0,0,163,0,188,191,86,167,112,58,57,51,0,100,0,45,518,0,519,514,515,516,517,0,112,0,378,0,0,0,0,241,0,380,381,30,32,0,0,0,0,0,0,0,0,153,0,0,0,0,0,0,0,201,202,0,2,0,0,0,0,34,0,120,121,123,44,53,47,49,116,523,524,112,112,0,41,0,0,23,0,220,0,176,164,189,0,195,196,199,200,0,198,197,209,191,0,71,58,104,0,102,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,503,504,480,0,0,0,0,0,484,482,483,0,407,409,423,431,433,435,437,439,441,444,449,452,455,459,0,461,485,502,500,519,390,0,0,0,0,0,0,391,389,37,0,0,112,0,0,0,130,126,128,261,263,0,0,39,0,0,9,10,0,112,525,526,219,95,0,0,168,88,239,0,236,0,0,3,0,5,0,35,0,0,0,23,0,117,118,143,111,0,150,0,0,0,0,0,0,0,0,0,0,0,0,519,291,293,0,301,295,0,299,284,285,286,0,287,288,289,0,42,23,123,22,24,268,0,226,242,0,0,222,220,0,206,0,0,0,211,61,210,192,0,105,101,0,0,0,469,0,0,481,425,459,0,426,427,0,0,0,0,0,0,0,0,0,0,462,463,0,0,0,0,465,466,464,0,83,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,413,414,415,416,417,418,419,420,421,422,412,0,467,0,491,492,0,0,0,506,0,112,382,0,0,0,404,519,526,0,0,0,0,257,400,405,0,402,0,379,397,398,0,395,0,243,0,0,0,0,264,0,234,131,136,132,134,127,129,220,0,270,262,271,528,527,0,90,92,0,0,94,110,67,66,0,64,169,220,238,165,270,240,177,178,0,89,205,13,0,31,0,33,0,122,124,245,244,23,25,107,119,0,0,0,138,139,146,0,112,112,158,0,0,0,0,0,519,0,0,0,330,331,332,0,0,334,0,0,0,302,296,123,300,294,292,26,0,175,227,0,0,0,233,221,228,146,0,0,0,222,174,208,207,170,206,0,0,212,62,113,106,430,103,99,0,0,0,0,519,246,251,0,0,0,0,0,0,0,0,0,0,0,0,0,0,505,512,0,511,408,432,0,434,436,438,440,442,443,447,448,445,446,450,451,453,454,456,457,458,411,410,490,487,0,489,0,0,0,393,394,0,260,0,401,255,258,388,0,256,385,0,392,36,0,386,0,265,0,137,133,135,0,222,0,206,0,272,0,220,0,283,267,0,0,112,0,0,0,130,0,112,0,220,0,187,166,237,0,7,0,4,6,27,0,142,125,143,0,0,141,222,151,160,159,0,0,154,0,0,0,309,0,0,0,0,0,0,329,333,0,0,0,297,290,0,28,269,220,0,230,0,0,148,223,0,146,226,214,171,193,194,212,190,468,0,0,0,247,0,252,471,0,0,0,0,0,501,476,479,0,0,0,0,460,508,0,0,488,486,0,0,0,0,259,403,406,396,399,387,266,235,146,0,273,274,206,0,0,222,206,0,0,38,222,519,0,63,65,0,179,0,0,222,0,206,0,0,8,140,0,144,116,149,161,158,158,0,0,0,0,0,0,0,0,0,0,0,0,0,309,335,0,298,29,222,0,231,229,0,172,0,148,222,0,213,497,0,496,0,248,253,0,0,0,0,0,428,429,490,489,510,493,0,513,424,507,509,0,383,384,148,146,276,282,146,0,275,206,0,116,0,68,180,186,146,0,185,181,206,15,0,16,0,0,0,157,156,303,309,0,0,0,0,0,0,337,338,340,342,344,346,348,350,353,358,361,364,368,370,376,377,0,0,306,315,0,0,0,0,0,0,0,0,0,0,336,216,232,0,224,173,215,220,470,0,0,254,472,473,0,0,478,477,494,0,279,148,148,146,277,40,0,0,148,146,182,14,17,0,145,109,0,0,0,309,0,374,0,0,371,309,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,372,309,0,317,0,0,0,325,0,0,0,0,0,0,308,0,0,222,499,498,0,0,0,495,278,281,148,108,0,183,148,18,162,304,305,0,0,0,0,309,311,341,0,343,345,347,349,351,352,356,357,354,355,359,360,362,363,365,366,367,0,310,316,318,319,0,327,326,0,321,0,0,0,147,225,217,0,0,0,0,280,0,184,307,373,0,369,312,0,309,320,328,322,323,0,250,249,474,0,69,0,339,309,313,324,0,375,314,475,0,0,0};
# 1865
static short Cyc_yydefgoto[147U]={1076,51,52,474,655,856,53,311,54,295,55,476,56,478,57,145,58,59,543,236,460,461,237,62,252,238,64,167,168,65,165,66,272,273,130,131,132,274,239,442,489,490,491,665,805,67,68,670,671,672,69,492,70,465,71,72,162,163,73,121,539,326,708,627,74,628,533,699,525,529,530,436,319,260,98,99,556,481,557,416,417,418,240,312,313,629,448,298,299,300,301,302,303,787,304,305,870,871,872,873,874,875,876,877,878,879,880,881,882,883,884,885,419,427,428,420,421,422,306,200,396,201,548,202,203,204,205,206,207,208,209,210,211,212,213,214,215,812,216,573,574,217,218,76,857,423,452};
# 1883
static short Cyc_yypact[1079U]={2986,- -32768,- -32768,- -32768,- -32768,- 13,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,3689,270,- 4,- -32768,3689,1312,- -32768,83,36,- -32768,31,70,68,154,165,216,243,307,- -32768,263,- -32768,- -32768,- -32768,260,613,303,262,290,290,- -32768,- -32768,295,2847,- -32768,519,635,- -32768,536,3689,3689,3689,- -32768,3689,- -32768,- -32768,244,- -32768,83,3603,338,73,33,978,- -32768,- -32768,330,348,369,- -32768,83,359,6833,- -32768,- -32768,3721,102,- -32768,- -32768,- -32768,- -32768,368,330,6833,- -32768,375,402,3721,414,399,417,- -32768,241,- -32768,- -32768,3896,3896,220,445,2847,2847,6833,343,- -32768,145,462,200,474,145,4355,6833,- -32768,- -32768,3124,- -32768,2847,3124,2847,3124,- -32768,499,504,- -32768,3474,- -32768,- -32768,- -32768,- -32768,4355,- -32768,- -32768,330,- 62,1845,- -32768,3603,536,513,3896,3772,5173,- -32768,338,- -32768,533,- -32768,- -32768,- -32768,- -32768,551,- -32768,- -32768,- 8,978,3896,- -32768,- -32768,545,554,558,83,7227,557,7324,6833,7027,575,598,603,611,618,625,630,632,637,640,7324,7324,- -32768,- -32768,- -32768,2420,642,7421,7421,7421,- -32768,- -32768,- -32768,180,- -32768,- -32768,- 44,670,634,641,644,687,109,693,284,301,- -32768,749,7421,148,- 50,- -32768,650,85,- -32768,3721,190,669,1182,309,3301,684,- -32768,- -32768,690,6833,330,3301,667,4106,4355,4438,4355,288,- -32768,14,14,- -32768,697,685,- -32768,- -32768,387,330,- -32768,- -32768,- -32768,- -32768,29,706,- -32768,- -32768,848,396,- -32768,718,720,- -32768,732,- -32768,734,- -32768,200,5270,3603,513,737,4355,- -32768,1016,707,83,750,692,56,731,4505,748,769,757,758,5378,2280,4505,218,755,- -32768,- -32768,762,1990,1990,536,1990,- -32768,- -32768,- -32768,770,- -32768,- -32768,- -32768,450,- -32768,513,767,- -32768,- -32768,760,59,793,- -32768,- 14,776,781,506,778,709,773,6833,3896,- -32768,785,- -32768,- -32768,59,83,- -32768,6833,791,2420,- -32768,4355,2420,- -32768,- -32768,- -32768,4613,- -32768,821,6833,6833,6833,6833,6833,6833,812,6833,4355,772,- -32768,- -32768,1990,792,408,6833,- -32768,- -32768,- -32768,6833,- -32768,7421,6833,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,6833,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,6833,- -32768,145,- -32768,- -32768,5475,145,6833,- -32768,796,330,- -32768,797,799,800,- -32768,- 51,368,145,6833,3721,355,- -32768,- -32768,- -32768,807,809,802,- -32768,- -32768,- -32768,814,820,3721,- -32768,541,1182,816,3896,- -32768,808,823,- -32768,4438,4438,4438,- -32768,- -32768,1218,5572,261,- -32768,349,- -32768,- -32768,- 14,- -32768,- -32768,795,828,- -32768,826,- -32768,810,822,830,- -32768,3207,- -32768,424,268,- -32768,- -32768,- -32768,4355,- -32768,- -32768,- -32768,2708,- -32768,2847,- -32768,2847,- -32768,- -32768,- -32768,- -32768,513,- -32768,- -32768,- -32768,1058,6833,829,833,- -32768,- 36,140,330,330,805,6833,6833,824,836,6833,831,930,1454,847,- -32768,- -32768,- -32768,572,926,- -32768,5669,2135,2560,- -32768,- -32768,3474,- -32768,- -32768,- -32768,- -32768,3896,- -32768,- -32768,4355,849,4189,- -32768,- -32768,841,904,- 14,859,4272,781,- -32768,- -32768,- -32768,- -32768,709,861,74,306,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,862,868,871,894,872,- -32768,- -32768,691,5270,874,881,882,886,889,437,885,891,895,561,900,7127,- -32768,- -32768,897,911,- -32768,670,93,634,641,644,687,109,109,693,693,693,693,284,284,301,301,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,899,- -32768,11,3896,5061,4355,- -32768,4355,- -32768,901,- -32768,- -32768,- -32768,- -32768,3171,- -32768,- -32768,3455,- -32768,- -32768,915,- -32768,92,- -32768,4355,- -32768,- -32768,- -32768,912,781,914,709,909,349,3896,3982,5766,- -32768,- -32768,6833,925,330,6930,931,29,3338,933,330,3896,3772,5863,- -32768,424,- -32768,942,- -32768,2847,- -32768,- -32768,- -32768,879,- -32768,- -32768,1016,937,6833,- -32768,781,- -32768,- -32768,- -32768,943,83,604,439,446,6833,794,460,4505,939,5960,6057,612,- -32768,- -32768,947,950,944,1990,- -32768,3603,- -32768,760,951,3896,- -32768,952,- 14,998,- -32768,953,904,222,- -32768,- -32768,- -32768,- -32768,306,- -32768,955,198,955,958,- -32768,4725,- -32768,- -32768,6833,6833,1053,6833,7027,- -32768,- -32768,- -32768,145,145,957,4837,- -32768,- -32768,6833,6833,- -32768,- -32768,59,695,973,981,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,904,982,- -32768,- -32768,709,59,984,781,709,972,478,- -32768,781,986,985,- -32768,- -32768,987,- -32768,59,988,781,991,709,980,283,- -32768,- -32768,6833,- -32768,4355,- -32768,993,37,805,4505,1004,999,1506,995,1006,4505,6833,6154,614,6251,623,6348,794,- -32768,1009,- -32768,- -32768,781,100,- -32768,- -32768,1008,- -32768,1022,998,781,4355,- -32768,- -32768,321,- -32768,6833,- -32768,- -32768,5270,1010,1012,1014,1019,- -32768,821,1017,1018,- -32768,- -32768,700,- -32768,- -32768,- -32768,- -32768,5061,- -32768,- -32768,998,904,- -32768,- -32768,904,1020,- -32768,709,1024,4355,1041,- -32768,- -32768,- -32768,904,1027,- -32768,- -32768,709,- -32768,1034,624,1030,1037,4355,- -32768,- -32768,1133,794,1040,7518,1035,2560,7421,1032,- -32768,- 22,- -32768,1072,1028,698,736,119,775,390,363,- -32768,- -32768,- -32768,- -32768,1076,7421,1990,- -32768,- -32768,489,4505,525,6445,4505,547,6542,6639,631,1047,- -32768,- -32768,- -32768,6833,1048,- -32768,- -32768,951,- -32768,389,178,- -32768,- -32768,- -32768,4355,1146,- -32768,- -32768,- -32768,4949,- -32768,998,998,904,- -32768,- -32768,1050,1054,998,904,- -32768,- -32768,- -32768,343,- -32768,- -32768,562,4505,1057,794,2420,- -32768,4355,1052,- -32768,1700,7421,6833,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,7421,6833,- -32768,794,1066,- -32768,4505,4505,568,- -32768,4505,4505,580,4505,581,6736,- -32768,1059,- 14,781,- -32768,- -32768,2560,1060,1063,- -32768,- -32768,- -32768,998,- -32768,1083,- -32768,998,- -32768,- -32768,- -32768,- -32768,1078,1070,1079,7421,794,- -32768,670,203,634,641,641,687,109,109,693,693,693,693,284,284,301,301,- -32768,- -32768,- -32768,281,- -32768,- -32768,- -32768,- -32768,4505,- -32768,- -32768,4505,- -32768,4505,4505,594,- -32768,- -32768,- -32768,1081,741,1073,4355,- -32768,1077,- -32768,- -32768,955,198,- -32768,- -32768,6833,1700,- -32768,- -32768,- -32768,- -32768,4505,- -32768,- -32768,- -32768,1080,- -32768,444,- -32768,794,- -32768,- -32768,1084,- -32768,- -32768,- -32768,1205,1207,- -32768};
# 1994
static short Cyc_yypgoto[147U]={- -32768,206,- -32768,- -32768,- -32768,276,- -32768,- 209,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- 55,- 113,23,- -32768,- -32768,0,569,- -32768,79,- 161,1096,52,- -32768,- -32768,- 148,- -32768,302,1178,- 742,- -32768,- -32768,- -32768,948,945,549,275,- -32768,- -32768,555,- 457,- 565,- -32768,- -32768,86,- -32768,- -32768,114,- 225,1148,- 428,8,- -32768,1062,- -32768,- -32768,1171,- 501,- -32768,514,- 133,- 151,- 54,240,239,538,544,- 439,- 487,- 117,- 436,- 139,- -32768,- 245,- 174,- 577,- 183,- -32768,825,- 85,- 69,- 147,- 109,- 399,147,- -32768,- -32768,22,- 290,- -32768,- 137,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,- -32768,188,1013,- -32768,636,811,- -32768,81,686,- -32768,- 116,- 261,- 162,- 356,- 352,- 364,876,- 341,- 353,- 256,- 350,- 325,- 163,723,463,898,195,- -32768,- 378,- -32768,405,496,19,2,- 111,42};
# 2016
static short Cyc_yytable[7666U]={60,342,259,253,579,515,516,253,518,576,320,327,637,343,144,578,317,329,582,583,270,60,333,599,480,60,736,588,589,581,360,361,362,464,652,859,241,242,651,704,696,663,668,669,79,116,458,632,83,526,365,397,63,60,590,591,404,221,339,60,60,60,484,60,164,651,569,632,141,60,151,277,947,63,698,222,440,63,144,61,153,- 525,134,135,136,140,137,405,296,693,307,147,146,700,366,318,664,77,652,521,61,40,449,927,61,63,357,40,527,60,60,63,63,63,249,63,948,584,585,586,587,63,325,60,82,60,60,60,60,750,447,140,61,60,687,363,746,459,61,61,61,450,61,60,104,60,734,449,61,467,437,828,151,451,608,271,146,105,558,117,- 155,63,63,199,39,528,297,308,146,48,49,50,133,42,225,63,107,63,63,63,63,39,546,522,498,63,450,499,61,61,42,39,248,221,108,63,106,63,373,374,42,45,61,706,61,61,61,61,955,956,221,222,61,451,100,307,547,363,592,593,594,745,61,- 242,61,732,- 242,660,222,903,255,451,398,399,400,375,376,439,645,451,296,296,906,296,807,957,958,446,39,550,838,551,552,250,842,542,41,42,43,124,309,251,645,19,20,535,466,567,487,853,60,921,358,220,658,109,401,469,440,440,440,402,403,493,453,228,110,597,60,441,836,600,483,146,469,60,60,784,60,547,528,296,363,607,363,408,250,811,39,364,987,83,431,715,251,246,247,42,297,297,409,297,63,16,17,18,544,363,45,262,524,263,264,265,266,1055,48,49,50,511,63,243,925,316,111,244,512,63,63,61,63,- 220,39,931,- 220,523,60,991,992,113,41,42,43,232,996,621,138,61,509,233,114,545,547,908,61,61,39,61,39,297,922,528,309,923,41,42,43,42,115,444,119,39,80,929,224,445,463,259,798,41,42,43,445,776,690,363,855,729,63,407,118,517,120,1056,426,358,444,123,358,764,558,426,445,379,48,49,50,380,1047,503,564,101,1049,363,764,61,510,381,424,439,439,439,437,276,278,382,383,577,636,681,909,39,446,692,910,735,164,44,688,41,42,43,144,644,148,650,149,993,44,816,- 12,466,150,997,60,633,60,634,60,166,610,170,646,635,601,469,752,659,414,415,101,441,441,441,223,493,75,250,985,963,766,753,101,226,60,251,469,964,965,769,227,363,469,60,858,767,456,60,81,737,470,738,102,230,103,471,63,146,63,961,63,817,363,962,432,229,146,571,437,691,146,75,231,44,700,558,528,822,547,75,647,457,648,61,75,61,63,61,649,823,245,363,139,363,142,63,721,75,782,63,363,520,912,1073,363,783,169,910,673,674,754,102,677,61,363,682,1011,1012,816,788,254,1008,61,102,768,707,61,1010,968,813,1014,1015,363,256,609,75,75,844,139,1020,1021,1013,75,363,824,825,830,617,969,75,267,75,75,75,75,801,101,316,268,101,534,101,636,296,310,307,1022,1023,101,800,125,126,75,982,39,725,399,400,808,363,817,650,493,42,971,60,1006,129,900,547,689,323,618,646,45,363,169,493,258,330,493,363,48,49,50,816,975,331,469,324,654,469,656,332,657,336,470,275,60,401,60,999,363,684,726,403,363,1032,258,1016,1017,1018,1019,344,363,363,945,63,603,1035,1037,297,799,146,624,625,626,102,363,757,102,39,102,1061,967,780,345,939,781,102,42,346,44,794,61,894,363,817,363,347,63,45,63,937,897,933,348,363,934,48,49,50,980,349,468,363,127,128,350,707,351,791,793,367,75,352,1069,61,353,61,359,468,406,495,368,703,500,831,371,372,377,378,369,944,410,438,370,443,547,537,538,140,429,75,839,434,666,667,430,986,1024,1025,1026,1002,743,454,261,384,849,1007,668,669,455,84,713,714,785,786,832,833,497,101,275,919,920,689,169,953,954,494,988,1028,528,296,101,462,322,101,182,951,952,472,1053,473,385,386,387,388,389,390,391,392,393,394,984,475,1003,477,1004,501,485,773,959,960,1063,363,861,862,748,1054,891,893,496,896,395,899,504,505,506,507,39,87,188,189,88,89,90,513,60,42,514,296,269,519,335,911,338,340,340,451,337,524,1043,192,44,531,777,536,540,354,355,297,102,532,549,365,340,340,340,1070,565,570,602,638,604,102,605,606,102,863,612,622,1074,613,614,890,639,340,641,1068,63,813,759,615,616,620,60,623,640,225,661,642,675,140,643,482,39,662,676,468,679,1065,468,678,41,42,43,61,683,297,75,685,75,886,75,973,463,694,977,979,697,663,445,468,48,49,50,39,701,468,705,709,710,258,841,41,42,43,63,845,711,296,712,716,- 525,717,718,487,851,541,719,338,258,720,722,48,49,50,101,258,723,101,358,727,724,61,730,482,733,1009,559,560,561,562,563,731,739,566,744,970,902,747,974,749,751,572,758,1027,907,575,154,155,156,157,158,340,60,159,160,161,762,1039,765,258,772,775,778,789,1044,795,595,796,316,797,258,802,804,728,820,297,806,834,596,258,814,1000,826,572,340,835,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,843,63,102,846,837,102,840,847,854,848,850,1030,1031,852,860,1033,1034,39,1036,864,888,865,889,901,631,41,42,43,61,904,905,340,913,468,914,915,916,487,926,917,918,488,924,928,75,48,49,50,468,930,932,468,935,936,938,940,943,946,949,39,779,950,966,981,989,983,994,41,42,43,44,995,1057,1001,1005,1058,258,1059,1060,487,1029,1040,1045,101,1046,1048,438,48,49,50,258,572,1051,572,1050,1064,1052,1062,1077,1066,1078,1071,1072,998,340,763,1075,257,112,479,774,486,152,122,810,1041,84,1042,328,7,8,9,10,11,12,13,14,15,16,17,18,19,20,809,803,611,619,482,580,433,1067,887,741,0,568,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,139,0,30,0,0,0,0,85,0,31,32,0,0,102,0,0,0,0,0,411,36,482,412,87,0,340,88,89,90,0,413,42,37,38,0,0,92,0,0,0,0,0,93,0,94,95,0,414,415,0,96,0,0,756,0,314,97,0,275,761,0,0,0,0,0,40,41,771,43,44,0,316,0,0,0,0,0,0,444,0,46,47,0,0,445,84,48,49,50,0,258,0,0,572,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,340,0,0,0,0,0,0,275,0,0,0,0,0,482,0,0,818,819,85,821,0,0,258,0,0,0,0,482,0,0,829,0,0,86,87,0,0,88,89,90,0,91,42,139,0,0,0,92,0,0,0,0,0,93,0,94,95,340,0,0,0,96,0,0,0,0,0,97,340,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,0,0,0,0,0,0,0,0,0,0,22,23,24,288,84,26,174,340,0,0,175,27,482,0,0,30,176,177,178,179,180,0,31,32,33,181,0,0,482,182,0,183,184,36,185,0,0,0,0,0,866,867,0,0,0,37,38,0,186,187,0,0,0,0,0,0,84,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,40,41,42,43,44,190,680,0,0,0,0,0,0,191,0,46,294,0,0,0,942,193,0,340,194,0,0,195,196,0,85,197,198,0,0,0,0,482,0,0,0,340,0,0,0,219,87,0,0,88,89,90,0,91,42,0,0,0,340,92,0,0,0,0,0,868,0,94,95,0,0,0,0,96,0,0,869,0,0,97,196,0,0,197,198,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,340,0,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,340,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,785,786,21,171,172,279,340,280,281,282,283,284,285,286,287,22,23,24,288,84,26,174,289,0,0,175,27,290,0,0,30,176,177,178,179,180,0,31,32,33,181,0,0,0,182,0,183,184,36,185,0,0,0,0,0,0,340,0,0,0,37,38,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,291,87,188,189,88,89,90,40,41,42,43,44,190,292,143,0,0,0,0,0,191,0,46,294,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,279,0,280,281,282,283,284,285,286,287,22,23,24,288,84,26,174,289,0,0,175,27,290,0,0,30,176,177,178,179,180,0,31,32,33,181,0,0,0,182,0,183,184,36,185,0,0,0,0,0,0,0,0,0,0,37,38,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,291,87,188,189,88,89,90,40,41,42,43,44,190,292,143,293,0,0,0,0,191,0,46,294,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,279,0,280,281,282,283,284,285,286,287,22,23,24,288,84,26,174,289,0,0,175,27,290,0,0,30,176,177,178,179,180,0,31,32,33,181,0,0,0,182,0,183,184,36,185,0,0,0,0,0,0,0,0,0,0,37,38,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,291,87,188,189,88,89,90,40,41,42,43,44,190,292,143,0,0,0,0,0,191,0,46,294,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,279,0,280,281,282,283,284,285,286,287,22,23,24,288,84,26,174,289,0,0,175,27,290,0,0,30,176,177,178,179,180,0,31,32,33,181,0,0,0,182,0,183,184,36,185,0,0,0,0,0,0,0,0,0,0,37,38,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,502,87,188,189,88,89,90,40,41,42,43,44,190,292,143,0,0,0,0,0,191,0,46,294,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,171,172,0,0,0,0,0,0,0,0,0,0,22,23,24,288,84,0,0,0,0,0,0,27,0,0,0,30,176,177,178,179,180,0,31,32,0,0,0,0,0,182,0,183,184,36,185,0,0,0,0,0,0,0,0,0,0,37,38,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,40,41,42,43,44,190,0,0,0,0,0,0,0,337,0,46,294,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,23,24,173,84,0,174,0,0,0,175,0,0,0,0,30,176,177,178,179,180,0,31,32,0,181,0,0,0,182,0,183,184,36,185,0,0,0,0,0,0,0,0,0,0,37,38,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,40,41,42,43,44,190,0,356,0,0,0,0,0,191,0,46,294,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,171,172,0,0,0,0,0,0,0,0,0,0,22,23,24,173,84,0,174,0,0,0,175,0,0,0,0,30,176,177,178,179,180,0,31,32,0,181,0,0,0,182,0,183,184,36,185,0,0,0,0,0,0,0,0,0,0,37,38,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,40,41,42,43,44,190,0,0,0,0,0,0,0,191,0,46,294,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,653,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,- 11,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,- 11,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,45,0,46,47,0,0,0,0,48,49,50,1,0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,28,29,30,0,0,0,0,0,0,31,32,33,0,0,34,35,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,84,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,39,0,0,0,0,0,0,40,41,42,43,44,22,23,24,- 11,0,0,0,0,45,0,46,47,0,0,0,30,48,49,50,0,85,0,31,32,0,0,0,0,0,0,0,0,740,36,0,412,87,0,0,88,89,90,0,413,42,37,38,0,0,92,0,0,0,0,0,93,0,94,95,0,414,415,0,96,0,0,0,0,314,97,0,39,0,0,0,0,0,0,40,41,42,43,44,0,316,0,0,0,0,0,0,463,0,46,47,0,0,445,84,48,49,50,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,85,0,0,31,32,0,0,0,0,0,0,0,425,0,36,219,87,0,0,88,89,90,0,91,42,0,37,38,0,92,0,0,0,0,0,93,0,94,95,0,0,0,0,96,0,0,0,0,0,97,0,0,39,0,0,0,0,0,0,40,41,42,43,44,0,0,0,0,0,0,0,0,487,0,46,47,0,0,0,0,48,49,50,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,84,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,36,0,0,0,85,0,0,0,0,0,0,0,37,38,0,0,742,0,0,219,87,0,0,88,89,90,0,91,42,0,0,0,0,92,0,0,0,0,0,93,0,94,95,0,0,40,41,96,43,44,0,0,143,97,0,269,0,0,0,0,46,47,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,25,0,26,0,0,0,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,33,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,2,3,4,78,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,21,0,0,0,0,40,41,0,43,44,0,0,143,22,23,24,25,0,0,0,46,47,0,0,27,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,36,0,0,84,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,85,0,0,0,0,46,47,0,0,0,30,0,0,0,0,219,87,31,32,88,89,90,0,91,42,0,0,0,36,92,0,0,0,0,0,93,0,94,95,0,37,38,0,96,0,0,0,0,0,97,0,0,0,0,0,0,0,0,0,0,0,0,0,314,0,0,315,0,0,0,0,0,0,40,41,0,43,44,0,316,0,0,0,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,234,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,40,41,0,43,44,0,0,235,22,23,24,0,0,0,0,46,47,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,314,0,0,0,0,0,0,0,0,0,40,41,0,43,44,0,316,0,0,0,0,0,0,0,0,46,47,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,22,23,24,0,0,0,0,0,0,0,0,0,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,435,0,0,0,0,0,0,46,47,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,695,0,0,0,0,0,0,46,47,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,702,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,7,8,9,10,11,12,13,14,15,16,17,18,19,20,0,0,0,0,0,0,0,0,0,0,40,41,0,43,44,22,23,24,0,0,0,0,0,0,0,46,47,0,0,0,30,0,0,0,0,0,0,31,32,0,0,0,0,0,0,0,0,0,36,0,0,0,0,0,0,0,0,0,0,0,37,38,0,0,0,0,0,0,0,0,171,172,279,0,280,281,282,283,284,285,286,287,0,0,0,173,84,0,174,289,0,0,175,40,290,0,0,44,176,177,178,179,180,0,0,0,0,181,46,47,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,502,87,188,189,88,89,90,0,0,42,0,0,190,292,143,0,0,0,0,0,191,0,0,192,0,171,172,0,193,0,0,194,553,0,195,196,0,0,197,198,173,84,0,174,0,0,0,175,0,0,0,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,554,87,188,189,88,89,90,0,251,42,0,0,190,0,341,555,0,0,0,0,191,0,0,192,0,414,415,0,193,171,172,194,0,0,195,196,0,0,197,198,0,0,0,0,173,84,0,174,0,0,0,175,0,0,0,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,554,87,188,189,88,89,90,0,251,42,0,0,190,0,341,815,0,0,0,0,191,0,0,192,0,414,415,0,193,171,172,194,0,0,195,196,0,0,197,198,0,0,0,0,173,84,0,174,0,0,0,175,0,0,0,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,554,87,188,189,88,89,90,0,251,42,0,0,190,0,341,827,0,0,0,0,191,0,0,192,0,414,415,0,193,171,172,194,0,0,195,196,0,0,197,198,0,0,0,0,173,84,0,174,0,0,0,175,0,0,0,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,554,87,188,189,88,89,90,0,251,42,0,0,190,0,341,990,0,0,0,0,191,0,0,192,0,414,415,0,193,171,172,194,0,0,195,196,0,0,197,198,0,0,0,0,173,84,0,174,0,0,0,175,0,0,0,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,554,87,188,189,88,89,90,0,251,42,0,0,190,0,341,0,0,0,0,0,191,0,0,192,0,414,415,0,193,171,172,194,0,0,195,196,0,0,197,198,0,0,0,0,173,84,0,174,0,0,0,175,0,0,0,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,0,0,192,0,0,0,321,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,341,0,0,0,0,0,191,0,0,192,0,171,172,0,193,0,0,194,0,0,195,196,0,0,197,198,173,84,0,174,0,0,0,175,0,0,0,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,508,171,172,0,0,0,0,191,0,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,598,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,0,0,192,0,0,0,630,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,686,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,0,0,192,0,0,0,755,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,0,0,192,0,0,0,770,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,790,171,172,0,0,0,0,191,0,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,792,171,172,0,0,0,0,191,0,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,892,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,895,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,898,171,172,0,0,0,0,191,0,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,972,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,976,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,978,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,1038,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,0,0,192,0,0,0,0,193,173,84,194,174,0,195,196,175,0,197,198,0,0,176,177,178,179,180,0,0,0,0,181,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,760,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,0,0,192,0,0,0,0,193,173,84,194,0,0,195,196,0,0,197,198,0,0,176,177,178,179,180,0,0,0,0,0,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,341,0,0,171,172,0,191,0,0,192,0,0,0,0,193,0,0,194,173,84,195,196,0,0,197,198,0,0,0,0,0,176,177,178,179,180,0,0,0,0,0,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,728,0,0,171,172,0,191,0,0,192,0,0,0,0,193,0,0,194,173,84,195,196,0,0,197,198,0,0,0,0,0,176,177,178,179,180,0,0,0,0,0,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,334,0,0,192,0,0,0,0,193,173,84,194,0,0,195,196,0,0,197,198,0,0,176,177,178,179,180,0,0,0,0,0,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,337,0,0,192,0,0,0,0,193,173,84,194,0,0,195,196,0,0,197,198,0,0,176,177,178,179,180,0,0,0,0,0,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,171,172,0,0,0,0,191,0,0,192,0,0,0,0,193,173,84,194,0,0,195,196,0,0,197,198,0,0,176,177,178,179,180,0,0,0,0,0,0,0,0,182,0,183,184,0,185,0,0,0,0,0,0,0,0,0,0,0,0,0,186,187,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,39,87,188,189,88,89,90,0,0,42,0,0,190,0,0,0,0,0,0,0,941,0,0,192,0,0,0,0,193,0,0,194,0,0,195,196,0,0,197,198};
# 2786
static short Cyc_yycheck[7666U]={0,175,119,114,368,295,296,118,298,365,149,162,451,175,69,367,149,164,371,372,133,21,170,401,269,25,603,377,378,370,193,194,195,258,470,777,105,106,466,540,527,77,5,6,21,45,17,446,25,63,94,214,0,53,379,380,106,108,174,59,60,61,271,63,126,493,356,466,66,69,70,133,94,21,531,126,237,25,133,0,72,132,59,60,61,66,63,137,143,525,145,69,69,532,138,149,132,110,534,308,21,115,88,845,25,53,191,115,122,109,110,59,60,61,112,63,138,373,374,375,376,69,130,123,128,125,126,127,128,630,239,112,53,133,512,124,623,108,59,60,61,127,63,143,108,145,135,88,69,258,235,728,152,139,415,133,133,126,341,45,123,109,110,82,108,316,143,145,145,136,137,138,58,117,93,123,108,125,126,127,128,108,330,124,128,133,127,131,109,110,117,108,111,108,126,143,126,145,89,90,117,128,123,129,125,126,127,128,89,90,108,126,133,139,26,270,332,124,381,382,383,129,143,121,145,132,124,488,126,129,116,139,84,85,86,126,127,237,463,139,295,296,807,298,701,126,127,239,108,334,751,336,337,108,755,324,116,117,118,53,146,116,487,20,21,319,258,352,128,770,270,836,191,85,483,121,128,258,439,440,441,133,134,275,242,97,121,398,288,237,747,402,270,270,275,295,296,675,298,415,451,356,124,414,124,115,108,109,108,129,132,288,231,558,116,109,110,117,295,296,130,298,270,17,18,19,326,124,128,123,108,125,126,127,128,132,136,137,138,121,288,121,843,121,128,125,128,295,296,270,298,129,108,854,132,313,356,922,923,52,116,117,118,122,929,434,122,288,287,128,107,329,488,809,295,296,108,298,108,356,837,532,268,840,116,117,118,117,128,128,128,108,122,850,92,134,128,514,688,116,117,118,134,664,517,124,123,570,356,221,107,297,122,132,226,334,128,122,337,644,603,233,134,139,136,137,138,143,993,282,349,26,997,124,659,356,289,136,129,439,440,441,527,141,142,144,145,366,448,504,129,108,444,522,133,602,126,119,513,116,117,118,517,463,126,465,128,924,119,714,122,463,134,930,474,126,476,128,478,110,125,122,463,134,403,466,633,487,133,134,85,439,440,441,126,487,0,108,109,136,647,634,97,128,504,116,487,144,145,648,108,124,493,513,775,648,129,517,22,604,124,606,26,124,28,129,474,504,476,139,478,714,124,143,232,121,513,129,623,517,517,45,125,119,983,728,697,721,664,53,126,249,128,474,58,476,504,478,134,721,115,124,66,124,68,513,129,71,129,517,124,121,817,129,124,129,80,133,497,498,634,85,501,504,124,504,950,951,833,129,128,947,513,97,648,543,517,949,888,710,953,954,124,129,416,109,110,129,112,959,960,952,116,124,725,726,732,429,129,123,121,125,126,127,128,694,221,121,124,224,124,226,632,688,121,690,961,962,233,693,121,122,146,904,108,84,85,86,702,124,833,651,644,117,129,655,946,121,795,775,513,128,121,644,128,124,170,659,119,124,662,124,136,137,138,920,129,123,659,128,474,662,476,125,478,128,124,138,688,128,690,129,124,121,133,134,124,129,149,955,956,957,958,128,124,124,869,655,406,129,129,688,690,690,439,440,441,221,124,638,224,108,226,129,887,121,128,864,124,233,117,128,119,121,655,121,124,920,124,128,688,128,690,860,121,121,128,124,124,136,137,138,121,128,258,124,121,122,128,707,128,680,681,93,268,128,1056,688,128,690,128,275,122,277,140,535,280,735,91,92,87,88,141,868,115,236,142,238,904,80,81,772,108,297,752,128,494,495,108,910,963,964,965,940,616,108,120,58,766,946,5,6,127,41,123,124,22,23,123,124,128,416,273,123,124,678,330,91,92,122,915,968,983,888,429,129,150,432,66,141,142,123,1005,123,95,96,97,98,99,100,101,102,103,104,908,123,941,123,943,128,123,655,87,88,123,124,780,781,628,1006,789,790,122,792,125,794,128,108,121,121,108,109,110,111,112,113,114,132,888,117,128,946,125,123,171,814,173,174,175,139,128,108,987,131,119,129,666,129,135,186,187,888,416,132,123,94,193,194,195,1056,108,129,126,128,127,429,127,127,432,782,123,123,1069,124,132,788,108,214,128,1055,888,1052,640,129,124,129,946,124,122,868,121,129,128,934,124,269,108,124,122,463,30,1046,466,132,116,117,118,888,121,946,474,45,476,785,478,894,128,128,897,898,139,77,134,487,136,137,138,108,129,493,129,129,124,444,754,116,117,118,946,759,129,1056,108,129,132,124,124,128,768,323,124,288,463,124,129,136,137,138,613,470,129,616,941,123,129,946,129,341,129,948,344,345,346,347,348,124,135,351,123,892,800,129,895,129,135,359,121,966,808,363,72,73,74,75,76,332,1056,79,80,81,129,980,129,514,122,128,123,128,987,122,384,121,121,129,525,123,78,122,25,1056,127,108,396,534,126,938,129,401,365,108,367,368,369,370,371,372,373,374,375,376,377,378,379,380,381,382,383,135,1056,613,124,129,616,129,129,135,129,129,971,972,129,128,975,976,108,978,122,132,129,123,121,445,116,117,118,1056,128,115,415,129,644,129,128,124,128,121,129,129,132,129,109,655,136,137,138,659,129,123,662,129,123,28,122,128,132,93,108,671,140,93,123,25,124,123,116,117,118,119,124,1032,123,129,1035,634,1037,1038,128,121,129,129,785,128,109,644,136,137,138,648,512,129,514,123,129,124,123,0,129,0,1061,129,934,488,643,129,118,37,268,662,273,71,49,707,983,41,984,163,8,9,10,11,12,13,14,15,16,17,18,19,20,21,702,697,417,432,558,369,233,1052,785,613,- 1,353,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,772,- 1,52,- 1,- 1,- 1,- 1,93,- 1,59,60,- 1,- 1,785,- 1,- 1,- 1,- 1,- 1,105,70,603,108,109,- 1,570,112,113,114,- 1,116,117,82,83,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,635,- 1,105,142,- 1,777,641,- 1,- 1,- 1,- 1,- 1,115,116,649,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,134,41,136,137,138,- 1,809,- 1,- 1,675,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,664,- 1,- 1,- 1,- 1,- 1,- 1,845,- 1,- 1,- 1,- 1,- 1,714,- 1,- 1,717,718,93,720,- 1,- 1,860,- 1,- 1,- 1,- 1,728,- 1,- 1,731,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,934,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,721,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,732,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,42,43,775,- 1,- 1,47,48,817,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,833,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,25,26,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,866,136,- 1,869,139,- 1,- 1,142,143,- 1,93,146,147,- 1,- 1,- 1,- 1,920,- 1,- 1,- 1,887,- 1,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,- 1,- 1,904,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,947,- 1,949,950,951,952,953,954,955,956,957,958,959,960,961,962,963,964,965,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,1005,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,1055,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,27,- 1,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,- 1,- 1,47,48,49,- 1,- 1,52,53,54,55,56,57,- 1,59,60,61,62,- 1,- 1,- 1,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,25,26,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,52,53,54,55,56,57,- 1,59,60,- 1,62,- 1,- 1,- 1,66,- 1,68,69,70,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,115,116,117,118,119,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,46,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,0,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,50,51,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,64,65,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,41,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,37,38,39,123,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,52,136,137,138,- 1,93,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,70,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,82,83,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,133,134,- 1,136,- 1,- 1,- 1,- 1,105,142,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,134,41,136,137,138,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,93,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,70,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,82,83,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,117,118,119,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,- 1,- 1,136,137,138,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,41,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,105,- 1,- 1,108,109,- 1,- 1,112,113,114,- 1,116,117,- 1,- 1,- 1,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,- 1,115,116,136,118,119,- 1,- 1,122,142,- 1,125,- 1,- 1,- 1,- 1,130,131,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,40,- 1,42,- 1,- 1,- 1,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,61,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,24,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,37,38,39,40,- 1,- 1,- 1,130,131,- 1,- 1,48,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,41,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,- 1,- 1,93,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,108,109,59,60,112,113,114,- 1,116,117,- 1,- 1,- 1,70,122,- 1,- 1,- 1,- 1,- 1,128,- 1,130,131,- 1,82,83,- 1,136,- 1,- 1,- 1,- 1,- 1,142,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,108,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,63,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,- 1,122,37,38,39,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,- 1,121,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,123,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,123,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,105,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,8,9,10,11,12,13,14,15,16,17,18,19,20,21,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,115,116,- 1,118,119,37,38,39,- 1,- 1,- 1,- 1,- 1,- 1,- 1,130,131,- 1,- 1,- 1,52,- 1,- 1,- 1,- 1,- 1,- 1,59,60,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,70,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,82,83,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,25,26,27,- 1,29,30,31,32,33,34,35,36,- 1,- 1,- 1,40,41,- 1,43,44,- 1,- 1,47,115,49,- 1,- 1,119,53,54,55,56,57,- 1,- 1,- 1,- 1,62,130,131,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,25,26,- 1,136,- 1,- 1,139,32,- 1,142,143,- 1,- 1,146,147,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,123,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,116,117,- 1,- 1,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,133,134,- 1,136,25,26,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,25,26,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147,40,41,- 1,43,- 1,- 1,- 1,47,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,135,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,121,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,129,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,43,- 1,142,143,47,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,62,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,25,26,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,40,41,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,122,- 1,- 1,25,26,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,40,41,142,143,- 1,- 1,146,147,- 1,- 1,- 1,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,25,26,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,40,41,139,- 1,- 1,142,143,- 1,- 1,146,147,- 1,- 1,53,54,55,56,57,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,66,- 1,68,69,- 1,71,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,85,86,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,108,109,110,111,112,113,114,- 1,- 1,117,- 1,- 1,120,- 1,- 1,- 1,- 1,- 1,- 1,- 1,128,- 1,- 1,131,- 1,- 1,- 1,- 1,136,- 1,- 1,139,- 1,- 1,142,143,- 1,- 1,146,147};char Cyc_Yystack_overflow[17U]="Yystack_overflow";struct Cyc_Yystack_overflow_exn_struct{char*tag;int f1;};
# 45 "cycbison.simple"
struct Cyc_Yystack_overflow_exn_struct Cyc_Yystack_overflow_val={Cyc_Yystack_overflow,0};
# 72 "cycbison.simple"
void Cyc_yyerror(struct _dyneither_ptr,int state,int token);
# 82 "cycbison.simple"
int Cyc_yylex(struct Cyc_Lexing_lexbuf*,union Cyc_YYSTYPE*yylval_ptr,struct Cyc_Yyltype*yylloc);struct _tuple27{unsigned int f1;struct _tuple0*f2;int f3;};struct _tuple28{struct _dyneither_ptr f1;void*f2;};static char _tmp41B[8U]="stdcall";static char _tmp41C[6U]="cdecl";static char _tmp41D[9U]="fastcall";static char _tmp41E[9U]="noreturn";static char _tmp41F[6U]="const";static char _tmp420[8U]="aligned";static char _tmp421[7U]="packed";static char _tmp422[7U]="shared";static char _tmp423[7U]="unused";static char _tmp424[5U]="weak";static char _tmp425[10U]="dllimport";static char _tmp426[10U]="dllexport";static char _tmp427[23U]="no_instrument_function";static char _tmp428[12U]="constructor";static char _tmp429[11U]="destructor";static char _tmp42A[22U]="no_check_memory_usage";static char _tmp42B[5U]="pure";static char _tmp42C[14U]="always_inline";struct _tuple29{void*f1;void*f2;};struct _tuple30{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 137 "cycbison.simple"
int Cyc_yyparse(struct _RegionHandle*yyr,struct Cyc_Lexing_lexbuf*yylex_buf){
# 140
struct _RegionHandle _tmp383=_new_region("yyregion");struct _RegionHandle*yyregion=& _tmp383;_push_region(yyregion);
{int yystate;
int yyn=0;
int yyerrstatus;
int yychar1=0;
# 146
int yychar;
union Cyc_YYSTYPE yylval=({union Cyc_YYSTYPE _tmp73D;(_tmp73D.YYINITIALSVAL).tag=64U,(_tmp73D.YYINITIALSVAL).val=0;_tmp73D;});
int yynerrs;
# 150
struct Cyc_Yyltype yylloc;
# 154
int yyssp_offset;
# 156
struct _dyneither_ptr yyss=({unsigned int _tmp68C=200U;short*_tmp68D=_region_calloc(yyregion,sizeof(short),_tmp68C);_tag_dyneither(_tmp68D,sizeof(short),_tmp68C);});
# 158
int yyvsp_offset;
struct _dyneither_ptr yyvs=_tag_dyneither(({unsigned int _tmp68B=200U;union Cyc_YYSTYPE*_tmp68A=({struct _RegionHandle*_tmp807=yyregion;_region_malloc(_tmp807,_check_times(_tmp68B,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp73C=200U;unsigned int i;for(i=0;i < _tmp73C;++ i){_tmp68A[i]=yylval;}}0;});_tmp68A;}),sizeof(union Cyc_YYSTYPE),200U);
# 163
int yylsp_offset;
struct _dyneither_ptr yyls=_tag_dyneither(({unsigned int _tmp689=200U;struct Cyc_Yyltype*_tmp688=({struct _RegionHandle*_tmp808=yyregion;_region_malloc(_tmp808,_check_times(_tmp689,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp73B=200U;unsigned int i;for(i=0;i < _tmp73B;++ i){({struct Cyc_Yyltype _tmp809=Cyc_yynewloc();_tmp688[i]=_tmp809;});}}0;});_tmp688;}),sizeof(struct Cyc_Yyltype),200U);
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
({struct _dyneither_ptr _tmp80B=({const char*_tmp384="parser stack overflow";_tag_dyneither(_tmp384,sizeof(char),22U);});int _tmp80A=yystate;Cyc_yyerror(_tmp80B,_tmp80A,yychar);});
(int)_throw((void*)& Cyc_Yystack_overflow_val);}
# 212
yystacksize *=2;
if(yystacksize > 10000)
yystacksize=10000;{
struct _dyneither_ptr yyss1=({unsigned int _tmp38A=(unsigned int)yystacksize;short*_tmp389=({struct _RegionHandle*_tmp80C=yyregion;_region_malloc(_tmp80C,_check_times(_tmp38A,sizeof(short)));});({{unsigned int _tmp6FB=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6FB;++ i){
i <= yyssp_offset?_tmp389[i]=*((short*)_check_dyneither_subscript(yyss,sizeof(short),(int)i)):(_tmp389[i]=0);}}0;});_tag_dyneither(_tmp389,sizeof(short),_tmp38A);});
struct _dyneither_ptr yyvs1=({unsigned int _tmp388=(unsigned int)yystacksize;union Cyc_YYSTYPE*_tmp387=({struct _RegionHandle*_tmp80D=yyregion;_region_malloc(_tmp80D,_check_times(_tmp388,sizeof(union Cyc_YYSTYPE)));});({{unsigned int _tmp6FA=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6FA;++ i){
# 219
i <= yyssp_offset?_tmp387[i]=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(int)i)):(_tmp387[i]=yylval);}}0;});_tag_dyneither(_tmp387,sizeof(union Cyc_YYSTYPE),_tmp388);});
# 221
struct _dyneither_ptr yyls1=({unsigned int _tmp386=(unsigned int)yystacksize;struct Cyc_Yyltype*_tmp385=({struct _RegionHandle*_tmp80E=yyregion;_region_malloc(_tmp80E,_check_times(_tmp386,sizeof(struct Cyc_Yyltype)));});({{unsigned int _tmp6F9=(unsigned int)yystacksize;unsigned int i;for(i=0;i < _tmp6F9;++ i){
i <= yyssp_offset?_tmp385[i]=*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(int)i)):({
struct Cyc_Yyltype _tmp80F=Cyc_yynewloc();_tmp385[i]=_tmp80F;});}}0;});_tag_dyneither(_tmp385,sizeof(struct Cyc_Yyltype),_tmp386);});
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
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1079U,yystate)];
if(yyn == - 32768)goto yydefault;
# 259
if(yychar == - 2)
# 265
({int _tmp810=Cyc_yylex(yylex_buf,& yylval,& yylloc);yychar=_tmp810;});
# 269
if(yychar <= 0){
# 271
yychar1=0;
yychar=0;}else{
# 280
yychar1=yychar > 0  && yychar <= 375?(int)Cyc_yytranslate[_check_known_subscript_notnull(376U,yychar)]: 295;}
# 297 "cycbison.simple"
yyn +=yychar1;
if((yyn < 0  || yyn > 7665) || Cyc_yycheck[_check_known_subscript_notnull(7666U,yyn)]!= yychar1)goto yydefault;
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
if(yyn == 1078){
int _tmp38B=0;_npop_handler(0U);return _tmp38B;}
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
 yylen=(int)Cyc_yyr2[_check_known_subscript_notnull(529U,yyn)];
if(yylen > 0)
yyval=*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),(yyvsp_offset + 1)- yylen));
# 369 "cycbison.simple"
{int _tmp38C=yyn;switch(_tmp38C){case 1U: _LL1: _LL2: {
# 372
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 374
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1198 "parse.y"
yyval=yyyyvsp[0];
({struct Cyc_List_List*_tmp811=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_parse_result=_tmp811;});
# 1201
goto _LL0;}case 2U: _LL3: _LL4: {
# 1203
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1205
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1204 "parse.y"
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp38D;_push_handler(& _tmp38D);{int _tmp38F=0;if(setjmp(_tmp38D.handler))_tmp38F=1;if(!_tmp38F){({struct Cyc_List_List*_tmp812=Cyc_yyget_YY16(yyyyvsp[0]);x=_tmp812;});;_pop_handler();}else{void*_tmp38E=(void*)_exn_thrown;void*_tmp390=_tmp38E;void*_tmp391;if(((struct Cyc_Core_Failure_exn_struct*)_tmp390)->tag == Cyc_Core_Failure){_LL420: _LL421:
 x=0;goto _LL41F;}else{_LL422: _tmp391=_tmp390;_LL423:(int)_rethrow(_tmp391);}_LL41F:;}};}
{struct _handler_cons _tmp392;_push_handler(& _tmp392);{int _tmp394=0;if(setjmp(_tmp392.handler))_tmp394=1;if(!_tmp394){({struct Cyc_List_List*_tmp813=Cyc_yyget_YY16(yyyyvsp[1]);y=_tmp813;});;_pop_handler();}else{void*_tmp393=(void*)_exn_thrown;void*_tmp395=_tmp393;void*_tmp396;if(((struct Cyc_Core_Failure_exn_struct*)_tmp395)->tag == Cyc_Core_Failure){_LL425: _LL426:
 y=0;goto _LL424;}else{_LL427: _tmp396=_tmp395;_LL428:(int)_rethrow(_tmp396);}_LL424:;}};}
({union Cyc_YYSTYPE _tmp814=Cyc_YY16(((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(x,y));yyval=_tmp814;});
# 1211
goto _LL0;}case 3U: _LL5: _LL6: {
# 1213
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1215
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1214 "parse.y"
({union Cyc_YYSTYPE _tmp819=Cyc_YY16(({struct Cyc_List_List*_tmp399=_cycalloc(sizeof(*_tmp399));({struct Cyc_Absyn_Decl*_tmp818=({struct Cyc_Absyn_Decl*_tmp398=_cycalloc(sizeof(*_tmp398));({void*_tmp817=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp397=_cycalloc(sizeof(*_tmp397));_tmp397->tag=10U,({struct _tuple0*_tmp816=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp397->f1=_tmp816;}),({struct Cyc_List_List*_tmp815=Cyc_yyget_YY16(yyyyvsp[2]);_tmp397->f2=_tmp815;});_tmp397;});_tmp398->r=_tmp817;}),_tmp398->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp398;});_tmp399->hd=_tmp818;}),_tmp399->tl=0;_tmp399;}));yyval=_tmp819;});
Cyc_Lex_leave_using();
# 1217
goto _LL0;}case 4U: _LL7: _LL8: {
# 1219
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1221
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1218 "parse.y"
({union Cyc_YYSTYPE _tmp81F=Cyc_YY16(({struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));({struct Cyc_Absyn_Decl*_tmp81E=({struct Cyc_Absyn_Decl*_tmp39B=_cycalloc(sizeof(*_tmp39B));({void*_tmp81D=(void*)({struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_tmp39A=_cycalloc(sizeof(*_tmp39A));_tmp39A->tag=10U,({struct _tuple0*_tmp81C=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp39A->f1=_tmp81C;}),({struct Cyc_List_List*_tmp81B=Cyc_yyget_YY16(yyyyvsp[2]);_tmp39A->f2=_tmp81B;});_tmp39A;});_tmp39B->r=_tmp81D;}),_tmp39B->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp39B;});_tmp39C->hd=_tmp81E;}),({struct Cyc_List_List*_tmp81A=Cyc_yyget_YY16(yyyyvsp[4]);_tmp39C->tl=_tmp81A;});_tmp39C;}));yyval=_tmp81F;});
goto _LL0;}case 5U: _LL9: _LLA: {
# 1221
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1223
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1221 "parse.y"
({union Cyc_YYSTYPE _tmp825=Cyc_YY16(({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct Cyc_Absyn_Decl*_tmp824=({struct Cyc_Absyn_Decl*_tmp39F=_cycalloc(sizeof(*_tmp39F));({void*_tmp823=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->tag=9U,({struct _dyneither_ptr*_tmp822=({struct _dyneither_ptr*_tmp39D=_cycalloc(sizeof(*_tmp39D));({struct _dyneither_ptr _tmp821=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp39D=_tmp821;});_tmp39D;});_tmp39E->f1=_tmp822;}),({struct Cyc_List_List*_tmp820=Cyc_yyget_YY16(yyyyvsp[2]);_tmp39E->f2=_tmp820;});_tmp39E;});_tmp39F->r=_tmp823;}),_tmp39F->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp39F;});_tmp3A0->hd=_tmp824;}),_tmp3A0->tl=0;_tmp3A0;}));yyval=_tmp825;});
Cyc_Lex_leave_namespace();
# 1224
goto _LL0;}case 6U: _LLB: _LLC: {
# 1226
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1228
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1226 "parse.y"
struct _dyneither_ptr nspace;
struct Cyc_List_List*x;struct Cyc_List_List*y;
{struct _handler_cons _tmp3A1;_push_handler(& _tmp3A1);{int _tmp3A3=0;if(setjmp(_tmp3A1.handler))_tmp3A3=1;if(!_tmp3A3){({struct _dyneither_ptr _tmp826=Cyc_yyget_String_tok(yyyyvsp[0]);nspace=_tmp826;});;_pop_handler();}else{void*_tmp3A2=(void*)_exn_thrown;void*_tmp3A4=_tmp3A2;void*_tmp3A6;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3A4)->tag == Cyc_Core_Failure){_LL42A: _LL42B:
({struct _dyneither_ptr _tmp827=({const char*_tmp3A5="";_tag_dyneither(_tmp3A5,sizeof(char),1U);});nspace=_tmp827;});goto _LL429;}else{_LL42C: _tmp3A6=_tmp3A4;_LL42D:(int)_rethrow(_tmp3A6);}_LL429:;}};}
# 1231
{struct _handler_cons _tmp3A7;_push_handler(& _tmp3A7);{int _tmp3A9=0;if(setjmp(_tmp3A7.handler))_tmp3A9=1;if(!_tmp3A9){({struct Cyc_List_List*_tmp828=Cyc_yyget_YY16(yyyyvsp[2]);x=_tmp828;});;_pop_handler();}else{void*_tmp3A8=(void*)_exn_thrown;void*_tmp3AA=_tmp3A8;void*_tmp3AB;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3AA)->tag == Cyc_Core_Failure){_LL42F: _LL430:
 x=0;goto _LL42E;}else{_LL431: _tmp3AB=_tmp3AA;_LL432:(int)_rethrow(_tmp3AB);}_LL42E:;}};}
# 1234
{struct _handler_cons _tmp3AC;_push_handler(& _tmp3AC);{int _tmp3AE=0;if(setjmp(_tmp3AC.handler))_tmp3AE=1;if(!_tmp3AE){({struct Cyc_List_List*_tmp829=Cyc_yyget_YY16(yyyyvsp[4]);y=_tmp829;});;_pop_handler();}else{void*_tmp3AD=(void*)_exn_thrown;void*_tmp3AF=_tmp3AD;void*_tmp3B0;if(((struct Cyc_Core_Failure_exn_struct*)_tmp3AF)->tag == Cyc_Core_Failure){_LL434: _LL435:
 y=0;goto _LL433;}else{_LL436: _tmp3B0=_tmp3AF;_LL437:(int)_rethrow(_tmp3B0);}_LL433:;}};}
# 1237
({union Cyc_YYSTYPE _tmp82D=Cyc_YY16(({struct Cyc_List_List*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));({struct Cyc_Absyn_Decl*_tmp82C=({struct Cyc_Absyn_Decl*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));({void*_tmp82B=(void*)({struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));_tmp3B2->tag=9U,({struct _dyneither_ptr*_tmp82A=({struct _dyneither_ptr*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));*_tmp3B1=nspace;_tmp3B1;});_tmp3B2->f1=_tmp82A;}),_tmp3B2->f2=x;_tmp3B2;});_tmp3B3->r=_tmp82B;}),_tmp3B3->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B3;});_tmp3B4->hd=_tmp82C;}),_tmp3B4->tl=y;_tmp3B4;}));yyval=_tmp82D;});
# 1239
goto _LL0;}case 7U: _LLD: _LLE: {
# 1241
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1243
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1240 "parse.y"
int _tmp3B5=Cyc_yyget_YY31(yyyyvsp[0]);
if(!_tmp3B5)
({union Cyc_YYSTYPE _tmp832=Cyc_YY16(({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct Cyc_Absyn_Decl*_tmp831=({struct Cyc_Absyn_Decl*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));({void*_tmp830=(void*)({struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->tag=11U,({struct Cyc_List_List*_tmp82F=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B6->f1=_tmp82F;});_tmp3B6;});_tmp3B7->r=_tmp830;}),_tmp3B7->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3B7;});_tmp3B8->hd=_tmp831;}),({struct Cyc_List_List*_tmp82E=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3B8->tl=_tmp82E;});_tmp3B8;}));yyval=_tmp832;});else{
# 1244
({union Cyc_YYSTYPE _tmp837=Cyc_YY16(({struct Cyc_List_List*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct Cyc_Absyn_Decl*_tmp836=({struct Cyc_Absyn_Decl*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({void*_tmp835=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=12U,({struct Cyc_List_List*_tmp834=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3B9->f1=_tmp834;}),_tmp3B9->f2=0;_tmp3B9;});_tmp3BA->r=_tmp835;}),_tmp3BA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BA;});_tmp3BB->hd=_tmp836;}),({struct Cyc_List_List*_tmp833=Cyc_yyget_YY16(yyyyvsp[4]);_tmp3BB->tl=_tmp833;});_tmp3BB;}));yyval=_tmp837;});}
# 1246
goto _LL0;}case 8U: _LLF: _LL10: {
# 1248
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1250
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1247 "parse.y"
if(!Cyc_yyget_YY31(yyyyvsp[0]))
({void*_tmp3BC=0U;({unsigned int _tmp839=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp838=({const char*_tmp3BD="expecting \"C include\"";_tag_dyneither(_tmp3BD,sizeof(char),22U);});Cyc_Warn_err(_tmp839,_tmp838,_tag_dyneither(_tmp3BC,sizeof(void*),0U));});});{
struct Cyc_List_List*exs=Cyc_yyget_YY52(yyyyvsp[4]);
({union Cyc_YYSTYPE _tmp83E=Cyc_YY16(({struct Cyc_List_List*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct Cyc_Absyn_Decl*_tmp83D=({struct Cyc_Absyn_Decl*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));({void*_tmp83C=(void*)({struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->tag=12U,({struct Cyc_List_List*_tmp83B=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3BE->f1=_tmp83B;}),_tmp3BE->f2=exs;_tmp3BE;});_tmp3BF->r=_tmp83C;}),_tmp3BF->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3BF;});_tmp3C0->hd=_tmp83D;}),({struct Cyc_List_List*_tmp83A=Cyc_yyget_YY16(yyyyvsp[5]);_tmp3C0->tl=_tmp83A;});_tmp3C0;}));yyval=_tmp83E;});
# 1252
goto _LL0;};}case 9U: _LL11: _LL12: {
# 1254
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1256
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1253 "parse.y"
({union Cyc_YYSTYPE _tmp841=Cyc_YY16(({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));({struct Cyc_Absyn_Decl*_tmp840=({struct Cyc_Absyn_Decl*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->r=(void*)& Cyc_Absyn_Porton_d_val,_tmp3C1->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C1;});_tmp3C2->hd=_tmp840;}),({struct Cyc_List_List*_tmp83F=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C2->tl=_tmp83F;});_tmp3C2;}));yyval=_tmp841;});
goto _LL0;}case 10U: _LL13: _LL14: {
# 1256
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1258
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1255 "parse.y"
({union Cyc_YYSTYPE _tmp844=Cyc_YY16(({struct Cyc_List_List*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));({struct Cyc_Absyn_Decl*_tmp843=({struct Cyc_Absyn_Decl*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->r=(void*)& Cyc_Absyn_Portoff_d_val,_tmp3C3->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp3C3;});_tmp3C4->hd=_tmp843;}),({struct Cyc_List_List*_tmp842=Cyc_yyget_YY16(yyyyvsp[2]);_tmp3C4->tl=_tmp842;});_tmp3C4;}));yyval=_tmp844;});
goto _LL0;}case 11U: _LL15: _LL16:
# 1258
({union Cyc_YYSTYPE _tmp845=Cyc_YY16(0);yyval=_tmp845;});
goto _LL0;case 12U: _LL17: _LL18: {
# 1261
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1263
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1261 "parse.y"
struct _dyneither_ptr _tmp3C5=Cyc_yyget_String_tok(yyyyvsp[1]);
Cyc_Lex_enter_extern_c();
if(({struct _dyneither_ptr _tmp846=(struct _dyneither_ptr)_tmp3C5;Cyc_strcmp(_tmp846,({const char*_tmp3C6="C";_tag_dyneither(_tmp3C6,sizeof(char),2U);}));})== 0)
({union Cyc_YYSTYPE _tmp847=Cyc_YY31(0);yyval=_tmp847;});else{
if(({struct _dyneither_ptr _tmp848=(struct _dyneither_ptr)_tmp3C5;Cyc_strcmp(_tmp848,({const char*_tmp3C7="C include";_tag_dyneither(_tmp3C7,sizeof(char),10U);}));})== 0)
({union Cyc_YYSTYPE _tmp849=Cyc_YY31(1);yyval=_tmp849;});else{
# 1268
({void*_tmp3C8=0U;({unsigned int _tmp84B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp84A=({const char*_tmp3C9="expecting \"C\" or \"C include\"";_tag_dyneither(_tmp3C9,sizeof(char),29U);});Cyc_Warn_err(_tmp84B,_tmp84A,_tag_dyneither(_tmp3C8,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp84C=Cyc_YY31(1);yyval=_tmp84C;});}}
# 1272
goto _LL0;}case 13U: _LL19: _LL1A: {
# 1274
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1276
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1275 "parse.y"
Cyc_Lex_leave_extern_c();
goto _LL0;}case 14U: _LL1B: _LL1C: {
# 1278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1279 "parse.y"
yyval=yyyyvsp[2];
goto _LL0;}case 15U: _LL1D: _LL1E: {
# 1282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1280 "parse.y"
({union Cyc_YYSTYPE _tmp84D=Cyc_YY52(0);yyval=_tmp84D;});
goto _LL0;}case 16U: _LL1F: _LL20: {
# 1283
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1285
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1284 "parse.y"
({union Cyc_YYSTYPE _tmp850=Cyc_YY52(({struct Cyc_List_List*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));({struct _tuple27*_tmp84F=({struct _tuple27*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));_tmp3CA->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp84E=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3CA->f2=_tmp84E;}),_tmp3CA->f3=0;_tmp3CA;});_tmp3CB->hd=_tmp84F;}),_tmp3CB->tl=0;_tmp3CB;}));yyval=_tmp850;});
goto _LL0;}case 17U: _LL21: _LL22: {
# 1287
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1289
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1285 "parse.y"
({union Cyc_YYSTYPE _tmp853=Cyc_YY52(({struct Cyc_List_List*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));({struct _tuple27*_tmp852=({struct _tuple27*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp851=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3CC->f2=_tmp851;}),_tmp3CC->f3=0;_tmp3CC;});_tmp3CD->hd=_tmp852;}),_tmp3CD->tl=0;_tmp3CD;}));yyval=_tmp853;});
goto _LL0;}case 18U: _LL23: _LL24: {
# 1288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1287 "parse.y"
({union Cyc_YYSTYPE _tmp857=Cyc_YY52(({struct Cyc_List_List*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));({struct _tuple27*_tmp856=({struct _tuple27*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->f1=(unsigned int)(yyyylsp[0]).first_line,({struct _tuple0*_tmp855=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp3CE->f2=_tmp855;}),_tmp3CE->f3=0;_tmp3CE;});_tmp3CF->hd=_tmp856;}),({struct Cyc_List_List*_tmp854=Cyc_yyget_YY52(yyyyvsp[2]);_tmp3CF->tl=_tmp854;});_tmp3CF;}));yyval=_tmp857;});
goto _LL0;}case 19U: _LL25: _LL26: {
# 1290
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1292
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1291 "parse.y"
({union Cyc_YYSTYPE _tmp85B=Cyc_YY16(({struct Cyc_List_List*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct Cyc_Absyn_Decl*_tmp85A=({void*_tmp859=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp858=Cyc_yyget_YY15(yyyyvsp[0]);_tmp3D0->f1=_tmp858;});_tmp3D0;});Cyc_Absyn_new_decl(_tmp859,(unsigned int)(yyyylsp[0]).first_line);});_tmp3D1->hd=_tmp85A;}),_tmp3D1->tl=0;_tmp3D1;}));yyval=_tmp85B;});
goto _LL0;}case 20U: _LL27: _LL28: {
# 1294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1292 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 21U: _LL29: _LL2A: {
# 1295
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1297
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1293 "parse.y"
({union Cyc_YYSTYPE _tmp85C=Cyc_YY16(0);yyval=_tmp85C;});
goto _LL0;}case 24U: _LL2B: _LL2C: {
# 1296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1302 "parse.y"
({union Cyc_YYSTYPE _tmp860=Cyc_YY15(({struct _RegionHandle*_tmp85F=yyr;struct Cyc_Parse_Declarator _tmp85E=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmp85D=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Parse_make_function(_tmp85F,0,_tmp85E,0,_tmp85D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp860;});
goto _LL0;}case 25U: _LL2D: _LL2E: {
# 1305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1304 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D2=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp864=Cyc_YY15(({struct _RegionHandle*_tmp863=yyr;struct Cyc_Parse_Declarator _tmp862=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp861=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp863,& _tmp3D2,_tmp862,0,_tmp861,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp864;});
goto _LL0;}case 26U: _LL2F: _LL30: {
# 1308
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1310
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1317 "parse.y"
({union Cyc_YYSTYPE _tmp869=Cyc_YY15(({struct _RegionHandle*_tmp868=yyr;struct Cyc_Parse_Declarator _tmp867=Cyc_yyget_YY27(yyyyvsp[0]);struct Cyc_List_List*_tmp866=Cyc_yyget_YY16(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp865=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp868,0,_tmp867,_tmp866,_tmp865,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp869;});
goto _LL0;}case 27U: _LL31: _LL32: {
# 1320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1319 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D3=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp86E=Cyc_YY15(({struct _RegionHandle*_tmp86D=yyr;struct Cyc_Parse_Declarator _tmp86C=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp86B=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp86A=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp86D,& _tmp3D3,_tmp86C,_tmp86B,_tmp86A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp86E;});
goto _LL0;}case 28U: _LL33: _LL34: {
# 1323
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1325
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1327 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D4=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp872=Cyc_YY15(({struct _RegionHandle*_tmp871=yyr;struct Cyc_Parse_Declarator _tmp870=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Absyn_Stmt*_tmp86F=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Parse_make_function(_tmp871,& _tmp3D4,_tmp870,0,_tmp86F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp872;});
goto _LL0;}case 29U: _LL35: _LL36: {
# 1331
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1333
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1330 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp3D5=Cyc_yyget_YY17(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp877=Cyc_YY15(({struct _RegionHandle*_tmp876=yyr;struct Cyc_Parse_Declarator _tmp875=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_List_List*_tmp874=Cyc_yyget_YY16(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmp873=Cyc_yyget_Stmt_tok(yyyyvsp[3]);Cyc_Parse_make_function(_tmp876,& _tmp3D5,_tmp875,_tmp874,_tmp873,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp877;});
goto _LL0;}case 30U: _LL37: _LL38: {
# 1334
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1336
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1335 "parse.y"
Cyc_Lex_enter_using(Cyc_yyget_QualId_tok(yyyyvsp[1]));yyval=yyyyvsp[1];
goto _LL0;}case 31U: _LL39: _LL3A: {
# 1338
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1338 "parse.y"
Cyc_Lex_leave_using();
goto _LL0;}case 32U: _LL3B: _LL3C: {
# 1341
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1343
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1341 "parse.y"
Cyc_Lex_enter_namespace(({struct _dyneither_ptr*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _dyneither_ptr _tmp878=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp3D6=_tmp878;});_tmp3D6;}));yyval=yyyyvsp[1];
goto _LL0;}case 33U: _LL3D: _LL3E: {
# 1344
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1346
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1344 "parse.y"
Cyc_Lex_leave_namespace();
goto _LL0;}case 34U: _LL3F: _LL40: {
# 1347
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1349
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
int _tmp3D7=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp87B=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87A=Cyc_yyget_YY17(yyyyvsp[0]);unsigned int _tmp879=(unsigned int)_tmp3D7;Cyc_Parse_make_declarations(_tmp87A,0,_tmp879,(unsigned int)_tmp3D7);}));yyval=_tmp87B;});
goto _LL0;}case 35U: _LL41: _LL42: {
# 1354
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1356
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1353 "parse.y"
int _tmp3D8=(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmp87F=Cyc_YY16(({struct Cyc_Parse_Declaration_spec _tmp87E=Cyc_yyget_YY17(yyyyvsp[0]);struct _tuple12*_tmp87D=Cyc_yyget_YY19(yyyyvsp[1]);unsigned int _tmp87C=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_make_declarations(_tmp87E,_tmp87D,_tmp87C,(unsigned int)_tmp3D8);}));yyval=_tmp87F;});
goto _LL0;}case 36U: _LL43: _LL44: {
# 1357
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1359
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1357 "parse.y"
({union Cyc_YYSTYPE _tmp883=Cyc_YY16(({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));({struct Cyc_Absyn_Decl*_tmp882=({struct Cyc_Absyn_Pat*_tmp881=Cyc_yyget_YY9(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmp880=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_let_decl(_tmp881,_tmp880,(unsigned int)(yyyylsp[0]).first_line);});_tmp3D9->hd=_tmp882;}),_tmp3D9->tl=0;_tmp3D9;}));yyval=_tmp883;});
goto _LL0;}case 37U: _LL45: _LL46: {
# 1360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1359 "parse.y"
struct Cyc_List_List*_tmp3DA=0;
{struct Cyc_List_List*_tmp3DB=Cyc_yyget_YY36(yyyyvsp[1]);for(0;_tmp3DB != 0;_tmp3DB=_tmp3DB->tl){
struct _dyneither_ptr*_tmp3DC=(struct _dyneither_ptr*)_tmp3DB->hd;
struct _tuple0*qv=({struct _tuple0*_tmp3DF=_cycalloc(sizeof(*_tmp3DF));({union Cyc_Absyn_Nmspace _tmp884=Cyc_Absyn_Rel_n(0);_tmp3DF->f1=_tmp884;}),_tmp3DF->f2=_tmp3DC;_tmp3DF;});
struct Cyc_Absyn_Vardecl*_tmp3DD=({struct _tuple0*_tmp885=qv;Cyc_Absyn_new_vardecl(0U,_tmp885,Cyc_Absyn_wildtyp(0),0);});
({struct Cyc_List_List*_tmp886=({struct Cyc_List_List*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->hd=_tmp3DD,_tmp3DE->tl=_tmp3DA;_tmp3DE;});_tmp3DA=_tmp886;});}}
# 1366
({struct Cyc_List_List*_tmp887=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3DA);_tmp3DA=_tmp887;});
({union Cyc_YYSTYPE _tmp889=Cyc_YY16(({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct Cyc_Absyn_Decl*_tmp888=Cyc_Absyn_letv_decl(_tmp3DA,(unsigned int)(yyyylsp[0]).first_line);_tmp3E0->hd=_tmp888;}),_tmp3E0->tl=0;_tmp3E0;}));yyval=_tmp889;});
# 1369
goto _LL0;}case 38U: _LL47: _LL48: {
# 1371
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1373
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1372 "parse.y"
struct _dyneither_ptr _tmp3E1=Cyc_yyget_String_tok(yyyyvsp[2]);
# 1374
if(({struct _dyneither_ptr _tmp88A=(struct _dyneither_ptr)_tmp3E1;Cyc_zstrcmp(_tmp88A,({const char*_tmp3E2="`H";_tag_dyneither(_tmp3E2,sizeof(char),3U);}));})== 0)
({void*_tmp3E3=0U;({unsigned int _tmp88C=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp88B=({const char*_tmp3E4="bad occurrence of heap region";_tag_dyneither(_tmp3E4,sizeof(char),30U);});Cyc_Warn_err(_tmp88C,_tmp88B,_tag_dyneither(_tmp3E3,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp88D=(struct _dyneither_ptr)_tmp3E1;Cyc_zstrcmp(_tmp88D,({const char*_tmp3E5="`U";_tag_dyneither(_tmp3E5,sizeof(char),3U);}));})== 0)
({void*_tmp3E6=0U;({unsigned int _tmp88F=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp88E=({const char*_tmp3E7="bad occurrence of unique region";_tag_dyneither(_tmp3E7,sizeof(char),32U);});Cyc_Warn_err(_tmp88F,_tmp88E,_tag_dyneither(_tmp3E6,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct _dyneither_ptr*_tmp891=({struct _dyneither_ptr*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));*_tmp3EB=_tmp3E1;_tmp3EB;});_tmp3EC->name=_tmp891;}),_tmp3EC->identity=- 1,({void*_tmp890=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3EC->kind=_tmp890;});_tmp3EC;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp895=(unsigned int)(yyyylsp[4]).first_line;struct _tuple0*_tmp894=({struct _tuple0*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp893=({struct _dyneither_ptr*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({struct _dyneither_ptr _tmp892=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp3E9=_tmp892;});_tmp3E9;});_tmp3EA->f2=_tmp893;});_tmp3EA;});Cyc_Absyn_new_vardecl(_tmp895,_tmp894,Cyc_Absyn_rgn_handle_type(t),0);});
({union Cyc_YYSTYPE _tmp897=Cyc_YY16(({struct Cyc_List_List*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));({struct Cyc_Absyn_Decl*_tmp896=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3E8->hd=_tmp896;}),_tmp3E8->tl=0;_tmp3E8;}));yyval=_tmp897;});
# 1383
goto _LL0;};}case 39U: _LL49: _LL4A: {
# 1385
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1387
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1385 "parse.y"
struct _dyneither_ptr _tmp3ED=Cyc_yyget_String_tok(yyyyvsp[1]);
if(({struct _dyneither_ptr _tmp898=(struct _dyneither_ptr)_tmp3ED;Cyc_zstrcmp(_tmp898,({const char*_tmp3EE="H";_tag_dyneither(_tmp3EE,sizeof(char),2U);}));})== 0)
({void*_tmp3EF=0U;({unsigned int _tmp89A=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp899=({const char*_tmp3F0="bad occurrence of heap region `H";_tag_dyneither(_tmp3F0,sizeof(char),33U);});Cyc_Warn_err(_tmp89A,_tmp899,_tag_dyneither(_tmp3EF,sizeof(void*),0U));});});
if(({struct _dyneither_ptr _tmp89B=(struct _dyneither_ptr)_tmp3ED;Cyc_zstrcmp(_tmp89B,({const char*_tmp3F1="U";_tag_dyneither(_tmp3F1,sizeof(char),2U);}));})== 0)
({void*_tmp3F2=0U;({unsigned int _tmp89D=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmp89C=({const char*_tmp3F3="bad occurrence of unique region `U";_tag_dyneither(_tmp3F3,sizeof(char),35U);});Cyc_Warn_err(_tmp89D,_tmp89C,_tag_dyneither(_tmp3F2,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));({struct _dyneither_ptr*_tmp8A1=({struct _dyneither_ptr*_tmp3FA=_cycalloc(sizeof(*_tmp3FA));({struct _dyneither_ptr _tmp8A0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3F9=({struct Cyc_String_pa_PrintArg_struct _tmp6FC;_tmp6FC.tag=0U,_tmp6FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3ED);_tmp6FC;});void*_tmp3F7[1U];_tmp3F7[0]=& _tmp3F9;({struct _dyneither_ptr _tmp89F=({const char*_tmp3F8="`%s";_tag_dyneither(_tmp3F8,sizeof(char),4U);});Cyc_aprintf(_tmp89F,_tag_dyneither(_tmp3F7,sizeof(void*),1U));});});*_tmp3FA=_tmp8A0;});_tmp3FA;});_tmp3FB->name=_tmp8A1;}),_tmp3FB->identity=- 1,({
void*_tmp89E=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp3FB->kind=_tmp89E;});_tmp3FB;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8A4=(unsigned int)(yyyylsp[1]).first_line;struct _tuple0*_tmp8A3=({struct _tuple0*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));_tmp3F6->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8A2=({struct _dyneither_ptr*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));*_tmp3F5=_tmp3ED;_tmp3F5;});_tmp3F6->f2=_tmp8A2;});_tmp3F6;});Cyc_Absyn_new_vardecl(_tmp8A4,_tmp8A3,Cyc_Absyn_rgn_handle_type(t),0);});
({union Cyc_YYSTYPE _tmp8A6=Cyc_YY16(({struct Cyc_List_List*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));({struct Cyc_Absyn_Decl*_tmp8A5=Cyc_Absyn_region_decl(tv,vd,0,(unsigned int)(yyyylsp[0]).first_line);_tmp3F4->hd=_tmp8A5;}),_tmp3F4->tl=0;_tmp3F4;}));yyval=_tmp8A6;});
# 1396
goto _LL0;};}case 40U: _LL4B: _LL4C: {
# 1398
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1400
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1398 "parse.y"
struct _dyneither_ptr _tmp3FC=Cyc_yyget_String_tok(yyyyvsp[1]);
struct _dyneither_ptr _tmp3FD=Cyc_yyget_String_tok(yyyyvsp[3]);
struct Cyc_Absyn_Exp*_tmp3FE=Cyc_yyget_Exp_tok(yyyyvsp[5]);
if(({struct _dyneither_ptr _tmp8A7=(struct _dyneither_ptr)_tmp3FD;Cyc_strcmp(_tmp8A7,({const char*_tmp3FF="open";_tag_dyneither(_tmp3FF,sizeof(char),5U);}));})!= 0)({void*_tmp400=0U;({unsigned int _tmp8A9=(unsigned int)(yyyylsp[3]).first_line;struct _dyneither_ptr _tmp8A8=({const char*_tmp401="expecting `open'";_tag_dyneither(_tmp401,sizeof(char),17U);});Cyc_Warn_err(_tmp8A9,_tmp8A8,_tag_dyneither(_tmp400,sizeof(void*),0U));});});{
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp409=_cycalloc(sizeof(*_tmp409));({struct _dyneither_ptr*_tmp8AD=({struct _dyneither_ptr*_tmp408=_cycalloc(sizeof(*_tmp408));({struct _dyneither_ptr _tmp8AC=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp407=({struct Cyc_String_pa_PrintArg_struct _tmp6FD;_tmp6FD.tag=0U,_tmp6FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3FC);_tmp6FD;});void*_tmp405[1U];_tmp405[0]=& _tmp407;({struct _dyneither_ptr _tmp8AB=({const char*_tmp406="`%s";_tag_dyneither(_tmp406,sizeof(char),4U);});Cyc_aprintf(_tmp8AB,_tag_dyneither(_tmp405,sizeof(void*),1U));});});*_tmp408=_tmp8AC;});_tmp408;});_tmp409->name=_tmp8AD;}),_tmp409->identity=- 1,({
void*_tmp8AA=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp409->kind=_tmp8AA;});_tmp409;});
void*t=Cyc_Absyn_var_type(tv);
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmp8B0=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmp8AF=({struct _tuple0*_tmp404=_cycalloc(sizeof(*_tmp404));_tmp404->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp8AE=({struct _dyneither_ptr*_tmp403=_cycalloc(sizeof(*_tmp403));*_tmp403=_tmp3FC;_tmp403;});_tmp404->f2=_tmp8AE;});_tmp404;});Cyc_Absyn_new_vardecl(_tmp8B0,_tmp8AF,Cyc_Absyn_rgn_handle_type(t),0);});
({union Cyc_YYSTYPE _tmp8B2=Cyc_YY16(({struct Cyc_List_List*_tmp402=_cycalloc(sizeof(*_tmp402));({struct Cyc_Absyn_Decl*_tmp8B1=Cyc_Absyn_region_decl(tv,vd,_tmp3FE,(unsigned int)(yyyylsp[0]).first_line);_tmp402->hd=_tmp8B1;}),_tmp402->tl=0;_tmp402;}));yyval=_tmp8B2;});
# 1408
goto _LL0;};}case 41U: _LL4D: _LL4E: {
# 1410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1412 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 42U: _LL4F: _LL50: {
# 1415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1414 "parse.y"
({union Cyc_YYSTYPE _tmp8B4=Cyc_YY16(({struct Cyc_List_List*_tmp8B3=Cyc_yyget_YY16(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8B3,Cyc_yyget_YY16(yyyyvsp[1]));}));yyval=_tmp8B4;});
goto _LL0;}case 43U: _LL51: _LL52: {
# 1417
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1419
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp8B8=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6FE;({enum Cyc_Parse_Storage_class*_tmp8B7=Cyc_yyget_YY20(yyyyvsp[0]);_tmp6FE.sc=_tmp8B7;}),({struct Cyc_Absyn_Tqual _tmp8B6=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp6FE.tq=_tmp8B6;}),({
struct Cyc_Parse_Type_specifier _tmp8B5=Cyc_Parse_empty_spec(0U);_tmp6FE.type_specs=_tmp8B5;}),_tmp6FE.is_inline=0,_tmp6FE.attributes=0;_tmp6FE;}));
# 1420
yyval=_tmp8B8;});
# 1422
goto _LL0;}case 44U: _LL53: _LL54: {
# 1424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1423 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp40A=Cyc_yyget_YY17(yyyyvsp[1]);
if(_tmp40A.sc != 0)
({void*_tmp40B=0U;({unsigned int _tmp8BA=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8B9=({const char*_tmp40C="Only one storage class is allowed in a declaration (missing ';' or ','?)";_tag_dyneither(_tmp40C,sizeof(char),73U);});Cyc_Warn_warn(_tmp8BA,_tmp8B9,_tag_dyneither(_tmp40B,sizeof(void*),0U));});});
# 1427
({union Cyc_YYSTYPE _tmp8BC=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp6FF;({enum Cyc_Parse_Storage_class*_tmp8BB=Cyc_yyget_YY20(yyyyvsp[0]);_tmp6FF.sc=_tmp8BB;}),_tmp6FF.tq=_tmp40A.tq,_tmp6FF.type_specs=_tmp40A.type_specs,_tmp6FF.is_inline=_tmp40A.is_inline,_tmp6FF.attributes=_tmp40A.attributes;_tmp6FF;}));yyval=_tmp8BC;});
# 1431
goto _LL0;}case 45U: _LL55: _LL56: {
# 1433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1432 "parse.y"
({void*_tmp40D=0U;({unsigned int _tmp8BE=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8BD=({const char*_tmp40E="__extension__ keyword ignored in declaration";_tag_dyneither(_tmp40E,sizeof(char),45U);});Cyc_Warn_warn(_tmp8BE,_tmp8BD,_tag_dyneither(_tmp40D,sizeof(void*),0U));});});
yyval=yyyyvsp[1];
# 1435
goto _LL0;}case 46U: _LL57: _LL58: {
# 1437
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1439
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1436 "parse.y"
({union Cyc_YYSTYPE _tmp8C1=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp700;_tmp700.sc=0,({struct Cyc_Absyn_Tqual _tmp8C0=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp700.tq=_tmp8C0;}),({
struct Cyc_Parse_Type_specifier _tmp8BF=Cyc_yyget_YY21(yyyyvsp[0]);_tmp700.type_specs=_tmp8BF;}),_tmp700.is_inline=0,_tmp700.attributes=0;_tmp700;}));
# 1436
yyval=_tmp8C1;});
# 1438
goto _LL0;}case 47U: _LL59: _LL5A: {
# 1440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1439 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp40F=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8C5=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp701;_tmp701.sc=_tmp40F.sc,_tmp701.tq=_tmp40F.tq,({
struct Cyc_Parse_Type_specifier _tmp8C4=({unsigned int _tmp8C3=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp8C2=_tmp40F.type_specs;Cyc_Parse_combine_specifiers(_tmp8C3,_tmp8C2,Cyc_yyget_YY21(yyyyvsp[0]));});_tmp701.type_specs=_tmp8C4;}),_tmp701.is_inline=_tmp40F.is_inline,_tmp701.attributes=_tmp40F.attributes;_tmp701;}));
# 1440
yyval=_tmp8C5;});
# 1445
goto _LL0;}case 48U: _LL5B: _LL5C: {
# 1447
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1449
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1446 "parse.y"
({union Cyc_YYSTYPE _tmp8C8=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp702;_tmp702.sc=0,({struct Cyc_Absyn_Tqual _tmp8C7=Cyc_yyget_YY23(yyyyvsp[0]);_tmp702.tq=_tmp8C7;}),({struct Cyc_Parse_Type_specifier _tmp8C6=Cyc_Parse_empty_spec(0U);_tmp702.type_specs=_tmp8C6;}),_tmp702.is_inline=0,_tmp702.attributes=0;_tmp702;}));yyval=_tmp8C8;});
goto _LL0;}case 49U: _LL5D: _LL5E: {
# 1449
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1451
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1448 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp410=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8CB=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp703;_tmp703.sc=_tmp410.sc,({struct Cyc_Absyn_Tqual _tmp8CA=({struct Cyc_Absyn_Tqual _tmp8C9=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp8C9,_tmp410.tq);});_tmp703.tq=_tmp8CA;}),_tmp703.type_specs=_tmp410.type_specs,_tmp703.is_inline=_tmp410.is_inline,_tmp703.attributes=_tmp410.attributes;_tmp703;}));yyval=_tmp8CB;});
# 1453
goto _LL0;}case 50U: _LL5F: _LL60: {
# 1455
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1457
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1454 "parse.y"
({union Cyc_YYSTYPE _tmp8CE=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp704;_tmp704.sc=0,({struct Cyc_Absyn_Tqual _tmp8CD=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp704.tq=_tmp8CD;}),({
struct Cyc_Parse_Type_specifier _tmp8CC=Cyc_Parse_empty_spec(0U);_tmp704.type_specs=_tmp8CC;}),_tmp704.is_inline=1,_tmp704.attributes=0;_tmp704;}));
# 1454
yyval=_tmp8CE;});
# 1456
goto _LL0;}case 51U: _LL61: _LL62: {
# 1458
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1460
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1457 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp411=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8CF=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp705;_tmp705.sc=_tmp411.sc,_tmp705.tq=_tmp411.tq,_tmp705.type_specs=_tmp411.type_specs,_tmp705.is_inline=1,_tmp705.attributes=_tmp411.attributes;_tmp705;}));yyval=_tmp8CF;});
# 1461
goto _LL0;}case 52U: _LL63: _LL64: {
# 1463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1462 "parse.y"
({union Cyc_YYSTYPE _tmp8D3=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp706;_tmp706.sc=0,({struct Cyc_Absyn_Tqual _tmp8D2=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp706.tq=_tmp8D2;}),({
struct Cyc_Parse_Type_specifier _tmp8D1=Cyc_Parse_empty_spec(0U);_tmp706.type_specs=_tmp8D1;}),_tmp706.is_inline=0,({struct Cyc_List_List*_tmp8D0=Cyc_yyget_YY45(yyyyvsp[0]);_tmp706.attributes=_tmp8D0;});_tmp706;}));
# 1462
yyval=_tmp8D3;});
# 1464
goto _LL0;}case 53U: _LL65: _LL66: {
# 1466
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1468
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1465 "parse.y"
struct Cyc_Parse_Declaration_spec _tmp412=Cyc_yyget_YY17(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp8D6=Cyc_YY17(({struct Cyc_Parse_Declaration_spec _tmp707;_tmp707.sc=_tmp412.sc,_tmp707.tq=_tmp412.tq,_tmp707.type_specs=_tmp412.type_specs,_tmp707.is_inline=_tmp412.is_inline,({
# 1468
struct Cyc_List_List*_tmp8D5=({struct Cyc_List_List*_tmp8D4=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8D4,_tmp412.attributes);});_tmp707.attributes=_tmp8D5;});_tmp707;}));
# 1466
yyval=_tmp8D6;});
# 1469
goto _LL0;}case 54U: _LL67: _LL68: {
# 1471
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1473
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1472 "parse.y"
static enum Cyc_Parse_Storage_class auto_sc=Cyc_Parse_Auto_sc;
({union Cyc_YYSTYPE _tmp8D7=Cyc_YY20(& auto_sc);yyval=_tmp8D7;});
goto _LL0;}case 55U: _LL69: _LL6A: {
# 1476
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1478
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1474 "parse.y"
static enum Cyc_Parse_Storage_class register_sc=Cyc_Parse_Register_sc;
({union Cyc_YYSTYPE _tmp8D8=Cyc_YY20(& register_sc);yyval=_tmp8D8;});
goto _LL0;}case 56U: _LL6B: _LL6C: {
# 1478
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1480
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1476 "parse.y"
static enum Cyc_Parse_Storage_class static_sc=Cyc_Parse_Static_sc;
({union Cyc_YYSTYPE _tmp8D9=Cyc_YY20(& static_sc);yyval=_tmp8D9;});
goto _LL0;}case 57U: _LL6D: _LL6E: {
# 1480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1478 "parse.y"
static enum Cyc_Parse_Storage_class extern_sc=Cyc_Parse_Extern_sc;
({union Cyc_YYSTYPE _tmp8DA=Cyc_YY20(& extern_sc);yyval=_tmp8DA;});
goto _LL0;}case 58U: _LL6F: _LL70: {
# 1482
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1484
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1481 "parse.y"
static enum Cyc_Parse_Storage_class externC_sc=Cyc_Parse_ExternC_sc;
if(({struct _dyneither_ptr _tmp8DB=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmp8DB,({const char*_tmp413="C";_tag_dyneither(_tmp413,sizeof(char),2U);}));})!= 0)
({void*_tmp414=0U;({unsigned int _tmp8DD=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8DC=({const char*_tmp415="only extern or extern \"C\" is allowed";_tag_dyneither(_tmp415,sizeof(char),37U);});Cyc_Warn_err(_tmp8DD,_tmp8DC,_tag_dyneither(_tmp414,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp8DE=Cyc_YY20(& externC_sc);yyval=_tmp8DE;});
# 1486
goto _LL0;}case 59U: _LL71: _LL72: {
# 1488
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1490
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1486 "parse.y"
static enum Cyc_Parse_Storage_class typedef_sc=Cyc_Parse_Typedef_sc;
({union Cyc_YYSTYPE _tmp8DF=Cyc_YY20(& typedef_sc);yyval=_tmp8DF;});
goto _LL0;}case 60U: _LL73: _LL74: {
# 1490
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1492
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1489 "parse.y"
static enum Cyc_Parse_Storage_class abstract_sc=Cyc_Parse_Abstract_sc;
({union Cyc_YYSTYPE _tmp8E0=Cyc_YY20(& abstract_sc);yyval=_tmp8E0;});
goto _LL0;}case 61U: _LL75: _LL76:
# 1493
({union Cyc_YYSTYPE _tmp8E1=Cyc_YY45(0);yyval=_tmp8E1;});
goto _LL0;case 62U: _LL77: _LL78: {
# 1496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1496 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 63U: _LL79: _LL7A: {
# 1499
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1501
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1501 "parse.y"
yyval=yyyyvsp[3];
goto _LL0;}case 64U: _LL7B: _LL7C: {
# 1504
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1506
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1505 "parse.y"
({union Cyc_YYSTYPE _tmp8E3=Cyc_YY45(({struct Cyc_List_List*_tmp416=_cycalloc(sizeof(*_tmp416));({void*_tmp8E2=Cyc_yyget_YY46(yyyyvsp[0]);_tmp416->hd=_tmp8E2;}),_tmp416->tl=0;_tmp416;}));yyval=_tmp8E3;});
goto _LL0;}case 65U: _LL7D: _LL7E: {
# 1508
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1510
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1506 "parse.y"
({union Cyc_YYSTYPE _tmp8E6=Cyc_YY45(({struct Cyc_List_List*_tmp417=_cycalloc(sizeof(*_tmp417));({void*_tmp8E5=Cyc_yyget_YY46(yyyyvsp[0]);_tmp417->hd=_tmp8E5;}),({struct Cyc_List_List*_tmp8E4=Cyc_yyget_YY45(yyyyvsp[2]);_tmp417->tl=_tmp8E4;});_tmp417;}));yyval=_tmp8E6;});
goto _LL0;}case 66U: _LL7F: _LL80: {
# 1509
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1511
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1511 "parse.y"
static struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct att_aligned={6U,0};
static struct _tuple28 att_map[18U]={{{_tmp41B,_tmp41B,_tmp41B + 8U},(void*)& Cyc_Absyn_Stdcall_att_val},{{_tmp41C,_tmp41C,_tmp41C + 6U},(void*)& Cyc_Absyn_Cdecl_att_val},{{_tmp41D,_tmp41D,_tmp41D + 9U},(void*)& Cyc_Absyn_Fastcall_att_val},{{_tmp41E,_tmp41E,_tmp41E + 9U},(void*)& Cyc_Absyn_Noreturn_att_val},{{_tmp41F,_tmp41F,_tmp41F + 6U},(void*)& Cyc_Absyn_Const_att_val},{{_tmp420,_tmp420,_tmp420 + 8U},(void*)& att_aligned},{{_tmp421,_tmp421,_tmp421 + 7U},(void*)& Cyc_Absyn_Packed_att_val},{{_tmp422,_tmp422,_tmp422 + 7U},(void*)& Cyc_Absyn_Shared_att_val},{{_tmp423,_tmp423,_tmp423 + 7U},(void*)& Cyc_Absyn_Unused_att_val},{{_tmp424,_tmp424,_tmp424 + 5U},(void*)& Cyc_Absyn_Weak_att_val},{{_tmp425,_tmp425,_tmp425 + 10U},(void*)& Cyc_Absyn_Dllimport_att_val},{{_tmp426,_tmp426,_tmp426 + 10U},(void*)& Cyc_Absyn_Dllexport_att_val},{{_tmp427,_tmp427,_tmp427 + 23U},(void*)& Cyc_Absyn_No_instrument_function_att_val},{{_tmp428,_tmp428,_tmp428 + 12U},(void*)& Cyc_Absyn_Constructor_att_val},{{_tmp429,_tmp429,_tmp429 + 11U},(void*)& Cyc_Absyn_Destructor_att_val},{{_tmp42A,_tmp42A,_tmp42A + 22U},(void*)& Cyc_Absyn_No_check_memory_usage_att_val},{{_tmp42B,_tmp42B,_tmp42B + 5U},(void*)& Cyc_Absyn_Pure_att_val},{{_tmp42C,_tmp42C,_tmp42C + 14U},(void*)& Cyc_Absyn_Always_inline_att_val}};
# 1532
struct _dyneither_ptr _tmp418=Cyc_yyget_String_tok(yyyyvsp[0]);
# 1534
if((((_get_dyneither_size(_tmp418,sizeof(char))> 4  && ((const char*)_tmp418.curr)[0]== '_') && ((const char*)_tmp418.curr)[1]== '_') && *((const char*)_check_dyneither_subscript(_tmp418,sizeof(char),(int)(_get_dyneither_size(_tmp418,sizeof(char))- 2)))== '_') && *((const char*)_check_dyneither_subscript(_tmp418,sizeof(char),(int)(_get_dyneither_size(_tmp418,sizeof(char))- 3)))== '_')
# 1536
({struct _dyneither_ptr _tmp8E7=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp418,2,_get_dyneither_size(_tmp418,sizeof(char))- 5);_tmp418=_tmp8E7;});{
int i=0;
for(0;i < 18U;++ i){
if(Cyc_strcmp((struct _dyneither_ptr)_tmp418,(struct _dyneither_ptr)(att_map[_check_known_subscript_notnull(18U,i)]).f1)== 0){
({union Cyc_YYSTYPE _tmp8E8=Cyc_YY46((att_map[i]).f2);yyval=_tmp8E8;});
break;}}
# 1543
if(i == 18U){
({void*_tmp419=0U;({unsigned int _tmp8EA=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp8E9=({const char*_tmp41A="unrecognized attribute";_tag_dyneither(_tmp41A,sizeof(char),23U);});Cyc_Warn_err(_tmp8EA,_tmp8E9,_tag_dyneither(_tmp419,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmp8EB=Cyc_YY46((void*)& Cyc_Absyn_Cdecl_att_val);yyval=_tmp8EB;});}
# 1548
goto _LL0;};}case 67U: _LL81: _LL82: {
# 1550
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1552
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1548 "parse.y"
({union Cyc_YYSTYPE _tmp8EC=Cyc_YY46((void*)& Cyc_Absyn_Const_att_val);yyval=_tmp8EC;});
goto _LL0;}case 68U: _LL83: _LL84: {
# 1551
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1553
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1550 "parse.y"
struct _dyneither_ptr _tmp42D=Cyc_yyget_String_tok(yyyyvsp[0]);
struct Cyc_Absyn_Exp*_tmp42E=Cyc_yyget_Exp_tok(yyyyvsp[2]);
void*a;
if(({struct _dyneither_ptr _tmp8EE=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8EE,({const char*_tmp42F="aligned";_tag_dyneither(_tmp42F,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp8ED=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8ED,({const char*_tmp430="__aligned__";_tag_dyneither(_tmp430,sizeof(char),12U);}));})== 0)
({void*_tmp8EF=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->tag=6U,_tmp431->f1=_tmp42E;_tmp431;});a=_tmp8EF;});else{
if(({struct _dyneither_ptr _tmp8F1=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8F1,({const char*_tmp432="section";_tag_dyneither(_tmp432,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp8F0=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8F0,({const char*_tmp433="__section__";_tag_dyneither(_tmp433,sizeof(char),12U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp42E);
({void*_tmp8F2=(void*)({struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->tag=8U,_tmp434->f1=str;_tmp434;});a=_tmp8F2;});}else{
if(({struct _dyneither_ptr _tmp8F3=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8F3,({const char*_tmp435="__mode__";_tag_dyneither(_tmp435,sizeof(char),9U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp42E);
({void*_tmp8F4=(void*)({struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->tag=24U,_tmp436->f1=str;_tmp436;});a=_tmp8F4;});}else{
if(({struct _dyneither_ptr _tmp8F5=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8F5,({const char*_tmp437="alias";_tag_dyneither(_tmp437,sizeof(char),6U);}));})== 0){
struct _dyneither_ptr str=Cyc_Parse_exp2string((unsigned int)(yyyylsp[2]).first_line,_tmp42E);
({void*_tmp8F6=(void*)({struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=25U,_tmp438->f1=str;_tmp438;});a=_tmp8F6;});}else{
# 1566
int n=Cyc_Parse_exp2int((unsigned int)(yyyylsp[2]).first_line,_tmp42E);
if(({struct _dyneither_ptr _tmp8F8=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8F8,({const char*_tmp439="regparm";_tag_dyneither(_tmp439,sizeof(char),8U);}));})== 0  || ({struct _dyneither_ptr _tmp8F7=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8F7,({const char*_tmp43A="__regparm__";_tag_dyneither(_tmp43A,sizeof(char),12U);}));})== 0){
if(n < 0  || n > 3)
({void*_tmp43B=0U;({unsigned int _tmp8FA=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp8F9=({const char*_tmp43C="regparm requires value between 0 and 3";_tag_dyneither(_tmp43C,sizeof(char),39U);});Cyc_Warn_err(_tmp8FA,_tmp8F9,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});
({void*_tmp8FB=(void*)({struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->tag=0U,_tmp43D->f1=n;_tmp43D;});a=_tmp8FB;});}else{
if(({struct _dyneither_ptr _tmp8FD=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8FD,({const char*_tmp43E="initializes";_tag_dyneither(_tmp43E,sizeof(char),12U);}));})== 0  || ({struct _dyneither_ptr _tmp8FC=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8FC,({const char*_tmp43F="__initializes__";_tag_dyneither(_tmp43F,sizeof(char),16U);}));})== 0)
({void*_tmp8FE=(void*)({struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->tag=20U,_tmp440->f1=n;_tmp440;});a=_tmp8FE;});else{
if(({struct _dyneither_ptr _tmp900=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp900,({const char*_tmp441="noliveunique";_tag_dyneither(_tmp441,sizeof(char),13U);}));})== 0  || ({struct _dyneither_ptr _tmp8FF=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp8FF,({const char*_tmp442="__noliveunique__";_tag_dyneither(_tmp442,sizeof(char),17U);}));})== 0)
({void*_tmp901=(void*)({struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*_tmp443=_cycalloc(sizeof(*_tmp443));_tmp443->tag=21U,_tmp443->f1=n;_tmp443;});a=_tmp901;});else{
if(({struct _dyneither_ptr _tmp903=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp903,({const char*_tmp444="noconsume";_tag_dyneither(_tmp444,sizeof(char),10U);}));})== 0  || ({struct _dyneither_ptr _tmp902=(struct _dyneither_ptr)_tmp42D;Cyc_zstrcmp(_tmp902,({const char*_tmp445="__noconsume__";_tag_dyneither(_tmp445,sizeof(char),14U);}));})== 0)
({void*_tmp904=(void*)({struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446->tag=22U,_tmp446->f1=n;_tmp446;});a=_tmp904;});else{
# 1578
({void*_tmp447=0U;({unsigned int _tmp906=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp905=({const char*_tmp448="unrecognized attribute";_tag_dyneither(_tmp448,sizeof(char),23U);});Cyc_Warn_err(_tmp906,_tmp905,_tag_dyneither(_tmp447,sizeof(void*),0U));});});
a=(void*)& Cyc_Absyn_Cdecl_att_val;}}}}}}}}
# 1582
({union Cyc_YYSTYPE _tmp907=Cyc_YY46(a);yyval=_tmp907;});
# 1584
goto _LL0;}case 69U: _LL85: _LL86: {
# 1586
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1585 "parse.y"
struct _dyneither_ptr _tmp449=Cyc_yyget_String_tok(yyyyvsp[0]);
struct _dyneither_ptr _tmp44A=Cyc_yyget_String_tok(yyyyvsp[2]);
unsigned int _tmp44B=({unsigned int _tmp908=(unsigned int)(yyyylsp[4]).first_line;Cyc_Parse_cnst2uint(_tmp908,Cyc_yyget_Int_tok(yyyyvsp[4]));});
unsigned int _tmp44C=({unsigned int _tmp909=(unsigned int)(yyyylsp[6]).first_line;Cyc_Parse_cnst2uint(_tmp909,Cyc_yyget_Int_tok(yyyyvsp[6]));});
void*a=(void*)& Cyc_Absyn_Cdecl_att_val;
if(({struct _dyneither_ptr _tmp90B=(struct _dyneither_ptr)_tmp449;Cyc_zstrcmp(_tmp90B,({const char*_tmp44D="format";_tag_dyneither(_tmp44D,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp90A=(struct _dyneither_ptr)_tmp449;Cyc_zstrcmp(_tmp90A,({const char*_tmp44E="__format__";_tag_dyneither(_tmp44E,sizeof(char),11U);}));})== 0){
if(({struct _dyneither_ptr _tmp90D=(struct _dyneither_ptr)_tmp44A;Cyc_zstrcmp(_tmp90D,({const char*_tmp44F="printf";_tag_dyneither(_tmp44F,sizeof(char),7U);}));})== 0  || ({struct _dyneither_ptr _tmp90C=(struct _dyneither_ptr)_tmp44A;Cyc_zstrcmp(_tmp90C,({const char*_tmp450="__printf__";_tag_dyneither(_tmp450,sizeof(char),11U);}));})== 0)
({void*_tmp90E=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=19U,_tmp451->f1=Cyc_Absyn_Printf_ft,_tmp451->f2=(int)_tmp44B,_tmp451->f3=(int)_tmp44C;_tmp451;});a=_tmp90E;});else{
if(({struct _dyneither_ptr _tmp910=(struct _dyneither_ptr)_tmp44A;Cyc_zstrcmp(_tmp910,({const char*_tmp452="scanf";_tag_dyneither(_tmp452,sizeof(char),6U);}));})== 0  || ({struct _dyneither_ptr _tmp90F=(struct _dyneither_ptr)_tmp44A;Cyc_zstrcmp(_tmp90F,({const char*_tmp453="__scanf__";_tag_dyneither(_tmp453,sizeof(char),10U);}));})== 0)
({void*_tmp911=(void*)({struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*_tmp454=_cycalloc(sizeof(*_tmp454));_tmp454->tag=19U,_tmp454->f1=Cyc_Absyn_Scanf_ft,_tmp454->f2=(int)_tmp44B,_tmp454->f3=(int)_tmp44C;_tmp454;});a=_tmp911;});else{
# 1596
({void*_tmp455=0U;({unsigned int _tmp913=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmp912=({const char*_tmp456="unrecognized format type";_tag_dyneither(_tmp456,sizeof(char),25U);});Cyc_Warn_err(_tmp913,_tmp912,_tag_dyneither(_tmp455,sizeof(void*),0U));});});}}}else{
# 1598
({void*_tmp457=0U;({unsigned int _tmp915=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmp914=({const char*_tmp458="unrecognized attribute";_tag_dyneither(_tmp458,sizeof(char),23U);});Cyc_Warn_err(_tmp915,_tmp914,_tag_dyneither(_tmp457,sizeof(void*),0U));});});}
({union Cyc_YYSTYPE _tmp916=Cyc_YY46(a);yyval=_tmp916;});
# 1601
goto _LL0;}case 70U: _LL87: _LL88: {
# 1603
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1605
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1610 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 71U: _LL89: _LL8A: {
# 1613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1612 "parse.y"
({union Cyc_YYSTYPE _tmp919=Cyc_YY21(({void*_tmp918=({struct _tuple0*_tmp917=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_typedef_type(_tmp917,Cyc_yyget_YY40(yyyyvsp[1]),0,0);});Cyc_Parse_type_spec(_tmp918,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp919;});
goto _LL0;}case 72U: _LL8B: _LL8C: {
# 1615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1616 "parse.y"
({union Cyc_YYSTYPE _tmp91A=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_void_type,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp91A;});
goto _LL0;}case 73U: _LL8D: _LL8E: {
# 1619
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1621
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1617 "parse.y"
({union Cyc_YYSTYPE _tmp91B=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_char_type,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp91B;});
goto _LL0;}case 74U: _LL8F: _LL90: {
# 1620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1618 "parse.y"
({union Cyc_YYSTYPE _tmp91C=Cyc_YY21(Cyc_Parse_short_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp91C;});
goto _LL0;}case 75U: _LL91: _LL92: {
# 1621
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1623
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1619 "parse.y"
({union Cyc_YYSTYPE _tmp91D=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_sint_type,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp91D;});
goto _LL0;}case 76U: _LL93: _LL94: {
# 1622
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1624
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1620 "parse.y"
({union Cyc_YYSTYPE _tmp91E=Cyc_YY21(Cyc_Parse_long_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp91E;});
goto _LL0;}case 77U: _LL95: _LL96: {
# 1623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1621 "parse.y"
({union Cyc_YYSTYPE _tmp91F=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_float_type,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp91F;});
goto _LL0;}case 78U: _LL97: _LL98: {
# 1624
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1626
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1622 "parse.y"
({union Cyc_YYSTYPE _tmp920=Cyc_YY21(Cyc_Parse_type_spec(Cyc_Absyn_double_type,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmp920;});
goto _LL0;}case 79U: _LL99: _LL9A: {
# 1625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1623 "parse.y"
({union Cyc_YYSTYPE _tmp921=Cyc_YY21(Cyc_Parse_signed_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp921;});
goto _LL0;}case 80U: _LL9B: _LL9C: {
# 1626
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1628
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1624 "parse.y"
({union Cyc_YYSTYPE _tmp922=Cyc_YY21(Cyc_Parse_unsigned_spec((unsigned int)(yyyylsp[0]).first_line));yyval=_tmp922;});
goto _LL0;}case 81U: _LL9D: _LL9E: {
# 1627
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1629
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1625 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 82U: _LL9F: _LLA0: {
# 1628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1626 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 83U: _LLA1: _LLA2: {
# 1629
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1631
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1629 "parse.y"
({union Cyc_YYSTYPE _tmp924=Cyc_YY21(({void*_tmp923=Cyc_Absyn_typeof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp923,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp924;});
goto _LL0;}case 84U: _LLA3: _LLA4: {
# 1632
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1634
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1631 "parse.y"
({union Cyc_YYSTYPE _tmp926=Cyc_YY21(({void*_tmp925=Cyc_Absyn_builtin_type(({const char*_tmp459="__builtin_va_list";_tag_dyneither(_tmp459,sizeof(char),18U);}),& Cyc_Tcutil_bk);Cyc_Parse_type_spec(_tmp925,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp926;});
goto _LL0;}case 85U: _LLA5: _LLA6: {
# 1634
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1636
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1633 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 86U: _LLA7: _LLA8: {
# 1636
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1638
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1635 "parse.y"
({union Cyc_YYSTYPE _tmp928=Cyc_YY21(({void*_tmp927=Cyc_yyget_YY44(yyyyvsp[0]);Cyc_Parse_type_spec(_tmp927,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp928;});
goto _LL0;}case 87U: _LLA9: _LLAA: {
# 1638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1637 "parse.y"
({union Cyc_YYSTYPE _tmp92A=Cyc_YY21(({void*_tmp929=Cyc_Absyn_new_evar(0,0);Cyc_Parse_type_spec(_tmp929,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp92A;});
goto _LL0;}case 88U: _LLAB: _LLAC: {
# 1640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1639 "parse.y"
({union Cyc_YYSTYPE _tmp92C=Cyc_YY21(({void*_tmp92B=Cyc_Absyn_new_evar(Cyc_Tcutil_kind_to_opt(Cyc_yyget_YY43(yyyyvsp[2])),0);Cyc_Parse_type_spec(_tmp92B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp92C;});
goto _LL0;}case 89U: _LLAD: _LLAE: {
# 1642
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1644
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1641 "parse.y"
({union Cyc_YYSTYPE _tmp930=Cyc_YY21(({void*_tmp92F=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->tag=6U,({struct Cyc_List_List*_tmp92E=({unsigned int _tmp92D=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp92D,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[2])));});
# 1641
_tmp45A->f1=_tmp92E;});_tmp45A;});Cyc_Parse_type_spec(_tmp92F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp930;});
# 1644
goto _LL0;}case 90U: _LLAF: _LLB0: {
# 1646
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1648
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1645 "parse.y"
({union Cyc_YYSTYPE _tmp932=Cyc_YY21(({void*_tmp931=Cyc_Absyn_rgn_handle_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp931,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp932;});
goto _LL0;}case 91U: _LLB1: _LLB2: {
# 1648
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1650
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1647 "parse.y"
({union Cyc_YYSTYPE _tmp934=Cyc_YY21(({void*_tmp933=Cyc_Absyn_rgn_handle_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,0));Cyc_Parse_type_spec(_tmp933,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp934;});
# 1649
goto _LL0;}case 92U: _LLB3: _LLB4: {
# 1651
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1653
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1650 "parse.y"
({union Cyc_YYSTYPE _tmp936=Cyc_YY21(({void*_tmp935=Cyc_Absyn_tag_type(Cyc_yyget_YY44(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp935,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp936;});
goto _LL0;}case 93U: _LLB5: _LLB6: {
# 1653
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1655
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1652 "parse.y"
({union Cyc_YYSTYPE _tmp938=Cyc_YY21(({void*_tmp937=Cyc_Absyn_tag_type(Cyc_Absyn_new_evar(& Cyc_Tcutil_iko,0));Cyc_Parse_type_spec(_tmp937,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp938;});
goto _LL0;}case 94U: _LLB7: _LLB8: {
# 1655
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1657
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1654 "parse.y"
({union Cyc_YYSTYPE _tmp93A=Cyc_YY21(({void*_tmp939=Cyc_Absyn_valueof_type(Cyc_yyget_Exp_tok(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp939,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp93A;});
goto _LL0;}case 95U: _LLB9: _LLBA: {
# 1657
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1659
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp93C=Cyc_YY43(({struct _dyneither_ptr _tmp93B=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id_to_kind(_tmp93B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp93C;});
goto _LL0;}case 96U: _LLBB: _LLBC: {
# 1663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1664 "parse.y"
unsigned int loc=(unsigned int)(Cyc_Absyn_porting_c_code?(yyyylsp[0]).first_line:(int)0U);
({union Cyc_YYSTYPE _tmp93D=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp708;_tmp708.print_const=1,_tmp708.q_volatile=0,_tmp708.q_restrict=0,_tmp708.real_const=1,_tmp708.loc=loc;_tmp708;}));yyval=_tmp93D;});
goto _LL0;}case 97U: _LLBD: _LLBE: {
# 1668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1666 "parse.y"
({union Cyc_YYSTYPE _tmp93E=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp709;_tmp709.print_const=0,_tmp709.q_volatile=1,_tmp709.q_restrict=0,_tmp709.real_const=0,_tmp709.loc=0U;_tmp709;}));yyval=_tmp93E;});
goto _LL0;}case 98U: _LLBF: _LLC0: {
# 1669
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1671
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1667 "parse.y"
({union Cyc_YYSTYPE _tmp93F=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmp70A;_tmp70A.print_const=0,_tmp70A.q_volatile=0,_tmp70A.q_restrict=1,_tmp70A.real_const=0,_tmp70A.loc=0U;_tmp70A;}));yyval=_tmp93F;});
goto _LL0;}case 99U: _LLC1: _LLC2: {
# 1670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
struct Cyc_Absyn_TypeDecl*_tmp45B=({struct Cyc_Absyn_TypeDecl*_tmp460=_cycalloc(sizeof(*_tmp460));({void*_tmp944=(void*)({struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));_tmp45F->tag=1U,({struct Cyc_Absyn_Enumdecl*_tmp943=({struct Cyc_Absyn_Enumdecl*_tmp45E=_cycalloc(sizeof(*_tmp45E));_tmp45E->sc=Cyc_Absyn_Public,({struct _tuple0*_tmp942=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp45E->name=_tmp942;}),({struct Cyc_Core_Opt*_tmp941=({struct Cyc_Core_Opt*_tmp45D=_cycalloc(sizeof(*_tmp45D));({struct Cyc_List_List*_tmp940=Cyc_yyget_YY48(yyyyvsp[3]);_tmp45D->v=_tmp940;});_tmp45D;});_tmp45E->fields=_tmp941;});_tmp45E;});_tmp45F->f1=_tmp943;});_tmp45F;});_tmp460->r=_tmp944;}),_tmp460->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp460;});
# 1675
({union Cyc_YYSTYPE _tmp946=Cyc_YY21(({void*_tmp945=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp45C=_cycalloc(sizeof(*_tmp45C));_tmp45C->tag=10U,_tmp45C->f1=_tmp45B,_tmp45C->f2=0;_tmp45C;});Cyc_Parse_type_spec(_tmp945,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp946;});
# 1677
goto _LL0;}case 100U: _LLC3: _LLC4: {
# 1679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1678 "parse.y"
({union Cyc_YYSTYPE _tmp948=Cyc_YY21(({void*_tmp947=Cyc_Absyn_enum_type(Cyc_yyget_QualId_tok(yyyyvsp[1]),0);Cyc_Parse_type_spec(_tmp947,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp948;});
goto _LL0;}case 101U: _LLC5: _LLC6: {
# 1681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1680 "parse.y"
({union Cyc_YYSTYPE _tmp94A=Cyc_YY21(({void*_tmp949=Cyc_Absyn_anon_enum_type(Cyc_yyget_YY48(yyyyvsp[2]));Cyc_Parse_type_spec(_tmp949,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp94A;});
goto _LL0;}case 102U: _LLC7: _LLC8: {
# 1683
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1685
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp94C=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp461=_cycalloc(sizeof(*_tmp461));({struct _tuple0*_tmp94B=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp461->name=_tmp94B;}),_tmp461->tag=0,_tmp461->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp461;}));yyval=_tmp94C;});
goto _LL0;}case 103U: _LLC9: _LLCA: {
# 1689
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1691
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1688 "parse.y"
({union Cyc_YYSTYPE _tmp94F=Cyc_YY47(({struct Cyc_Absyn_Enumfield*_tmp462=_cycalloc(sizeof(*_tmp462));({struct _tuple0*_tmp94E=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp462->name=_tmp94E;}),({struct Cyc_Absyn_Exp*_tmp94D=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp462->tag=_tmp94D;}),_tmp462->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp462;}));yyval=_tmp94F;});
goto _LL0;}case 104U: _LLCB: _LLCC: {
# 1691
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1693
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1692 "parse.y"
({union Cyc_YYSTYPE _tmp951=Cyc_YY48(({struct Cyc_List_List*_tmp463=_cycalloc(sizeof(*_tmp463));({struct Cyc_Absyn_Enumfield*_tmp950=Cyc_yyget_YY47(yyyyvsp[0]);_tmp463->hd=_tmp950;}),_tmp463->tl=0;_tmp463;}));yyval=_tmp951;});
goto _LL0;}case 105U: _LLCD: _LLCE: {
# 1695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1693 "parse.y"
({union Cyc_YYSTYPE _tmp953=Cyc_YY48(({struct Cyc_List_List*_tmp464=_cycalloc(sizeof(*_tmp464));({struct Cyc_Absyn_Enumfield*_tmp952=Cyc_yyget_YY47(yyyyvsp[0]);_tmp464->hd=_tmp952;}),_tmp464->tl=0;_tmp464;}));yyval=_tmp953;});
goto _LL0;}case 106U: _LLCF: _LLD0: {
# 1696
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1698
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1694 "parse.y"
({union Cyc_YYSTYPE _tmp956=Cyc_YY48(({struct Cyc_List_List*_tmp465=_cycalloc(sizeof(*_tmp465));({struct Cyc_Absyn_Enumfield*_tmp955=Cyc_yyget_YY47(yyyyvsp[0]);_tmp465->hd=_tmp955;}),({struct Cyc_List_List*_tmp954=Cyc_yyget_YY48(yyyyvsp[2]);_tmp465->tl=_tmp954;});_tmp465;}));yyval=_tmp956;});
goto _LL0;}case 107U: _LLD1: _LLD2: {
# 1697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
({union Cyc_YYSTYPE _tmp95A=Cyc_YY21(({void*_tmp959=(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp466=_cycalloc(sizeof(*_tmp466));_tmp466->tag=7U,({enum Cyc_Absyn_AggrKind _tmp958=Cyc_yyget_YY22(yyyyvsp[0]);_tmp466->f1=_tmp958;}),({struct Cyc_List_List*_tmp957=Cyc_yyget_YY24(yyyyvsp[2]);_tmp466->f2=_tmp957;});_tmp466;});Cyc_Parse_type_spec(_tmp959,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp95A;});
goto _LL0;}case 108U: _LLD3: _LLD4: {
# 1703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 1705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 1706 "parse.y"
struct Cyc_List_List*_tmp467=({unsigned int _tmp95B=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp95B,Cyc_yyget_YY40(yyyyvsp[3]));});
struct Cyc_List_List*_tmp468=({unsigned int _tmp95C=(unsigned int)(yyyylsp[5]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp95C,Cyc_yyget_YY40(yyyyvsp[5]));});
struct Cyc_Absyn_TypeDecl*_tmp469=({enum Cyc_Absyn_AggrKind _tmp962=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp961=Cyc_yyget_QualId_tok(yyyyvsp[2]);struct Cyc_List_List*_tmp960=_tmp467;struct Cyc_Absyn_AggrdeclImpl*_tmp95F=({
struct Cyc_List_List*_tmp95E=_tmp468;struct Cyc_List_List*_tmp95D=Cyc_yyget_YY50(yyyyvsp[6]);Cyc_Absyn_aggrdecl_impl(_tmp95E,_tmp95D,Cyc_yyget_YY24(yyyyvsp[7]),1);});
# 1708
Cyc_Absyn_aggr_tdecl(_tmp962,Cyc_Absyn_Public,_tmp961,_tmp960,_tmp95F,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1711
({union Cyc_YYSTYPE _tmp964=Cyc_YY21(({void*_tmp963=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=10U,_tmp46A->f1=_tmp469,_tmp46A->f2=0;_tmp46A;});Cyc_Parse_type_spec(_tmp963,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp964;});
# 1713
goto _LL0;}case 109U: _LLD5: _LLD6: {
# 1715
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 1717
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 1717 "parse.y"
struct Cyc_List_List*_tmp46B=({unsigned int _tmp965=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp965,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_List_List*_tmp46C=({unsigned int _tmp966=(unsigned int)(yyyylsp[4]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp966,Cyc_yyget_YY40(yyyyvsp[4]));});
struct Cyc_Absyn_TypeDecl*_tmp46D=({enum Cyc_Absyn_AggrKind _tmp96C=Cyc_yyget_YY22(yyyyvsp[0]);struct _tuple0*_tmp96B=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp96A=_tmp46B;struct Cyc_Absyn_AggrdeclImpl*_tmp969=({
struct Cyc_List_List*_tmp968=_tmp46C;struct Cyc_List_List*_tmp967=Cyc_yyget_YY50(yyyyvsp[5]);Cyc_Absyn_aggrdecl_impl(_tmp968,_tmp967,Cyc_yyget_YY24(yyyyvsp[6]),0);});
# 1719
Cyc_Absyn_aggr_tdecl(_tmp96C,Cyc_Absyn_Public,_tmp96B,_tmp96A,_tmp969,0,(unsigned int)(yyyylsp[0]).first_line);});
# 1722
({union Cyc_YYSTYPE _tmp96E=Cyc_YY21(({void*_tmp96D=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=10U,_tmp46E->f1=_tmp46D,_tmp46E->f2=0;_tmp46E;});Cyc_Parse_type_spec(_tmp96D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp96E;});
# 1724
goto _LL0;}case 110U: _LLD7: _LLD8: {
# 1726
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1728
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1725 "parse.y"
({union Cyc_YYSTYPE _tmp973=Cyc_YY21(({void*_tmp972=({union Cyc_Absyn_AggrInfo _tmp971=({enum Cyc_Absyn_AggrKind _tmp970=Cyc_yyget_YY22(yyyyvsp[1]);struct _tuple0*_tmp96F=Cyc_yyget_QualId_tok(yyyyvsp[2]);Cyc_Absyn_UnknownAggr(_tmp970,_tmp96F,({struct Cyc_Core_Opt*_tmp46F=_cycalloc(sizeof(*_tmp46F));_tmp46F->v=(void*)1;_tmp46F;}));});Cyc_Absyn_aggr_type(_tmp971,Cyc_yyget_YY40(yyyyvsp[3]));});Cyc_Parse_type_spec(_tmp972,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp973;});
# 1728
goto _LL0;}case 111U: _LLD9: _LLDA: {
# 1730
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1732
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1729 "parse.y"
({union Cyc_YYSTYPE _tmp977=Cyc_YY21(({void*_tmp976=({union Cyc_Absyn_AggrInfo _tmp975=({enum Cyc_Absyn_AggrKind _tmp974=Cyc_yyget_YY22(yyyyvsp[0]);Cyc_Absyn_UnknownAggr(_tmp974,Cyc_yyget_QualId_tok(yyyyvsp[1]),0);});Cyc_Absyn_aggr_type(_tmp975,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp976,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp977;});
goto _LL0;}case 112U: _LLDB: _LLDC:
# 1732
({union Cyc_YYSTYPE _tmp978=Cyc_YY40(0);yyval=_tmp978;});
goto _LL0;case 113U: _LLDD: _LLDE: {
# 1735
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1737
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1736 "parse.y"
({union Cyc_YYSTYPE _tmp979=Cyc_YY40(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[1])));yyval=_tmp979;});
goto _LL0;}case 114U: _LLDF: _LLE0: {
# 1739
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1741
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1740 "parse.y"
({union Cyc_YYSTYPE _tmp97A=Cyc_YY22(Cyc_Absyn_StructA);yyval=_tmp97A;});
goto _LL0;}case 115U: _LLE1: _LLE2: {
# 1743
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1745
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1741 "parse.y"
({union Cyc_YYSTYPE _tmp97B=Cyc_YY22(Cyc_Absyn_UnionA);yyval=_tmp97B;});
goto _LL0;}case 116U: _LLE3: _LLE4:
# 1744
({union Cyc_YYSTYPE _tmp97C=Cyc_YY24(0);yyval=_tmp97C;});
goto _LL0;case 117U: _LLE5: _LLE6: {
# 1747
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1749
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1748 "parse.y"
struct Cyc_List_List*decls=0;
{struct Cyc_List_List*_tmp470=Cyc_yyget_YY25(yyyyvsp[0]);for(0;_tmp470 != 0;_tmp470=_tmp470->tl){
({struct Cyc_List_List*_tmp97D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)((struct Cyc_List_List*)_tmp470->hd,decls);decls=_tmp97D;});}}{
# 1752
struct Cyc_List_List*_tmp471=Cyc_Parse_get_aggrfield_tags(decls);
if(_tmp471 != 0)
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Parse_substitute_aggrfield_tags,_tmp471,decls);
({union Cyc_YYSTYPE _tmp97E=Cyc_YY24(decls);yyval=_tmp97E;});
# 1757
goto _LL0;};}case 118U: _LLE7: _LLE8: {
# 1759
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1761
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp980=Cyc_YY25(({struct Cyc_List_List*_tmp472=_cycalloc(sizeof(*_tmp472));({struct Cyc_List_List*_tmp97F=Cyc_yyget_YY24(yyyyvsp[0]);_tmp472->hd=_tmp97F;}),_tmp472->tl=0;_tmp472;}));yyval=_tmp980;});
goto _LL0;}case 119U: _LLE9: _LLEA: {
# 1765
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1767
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1764 "parse.y"
({union Cyc_YYSTYPE _tmp983=Cyc_YY25(({struct Cyc_List_List*_tmp473=_cycalloc(sizeof(*_tmp473));({struct Cyc_List_List*_tmp982=Cyc_yyget_YY24(yyyyvsp[1]);_tmp473->hd=_tmp982;}),({struct Cyc_List_List*_tmp981=Cyc_yyget_YY25(yyyyvsp[0]);_tmp473->tl=_tmp981;});_tmp473;}));yyval=_tmp983;});
goto _LL0;}case 120U: _LLEB: _LLEC: {
# 1767
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1769
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1768 "parse.y"
({union Cyc_YYSTYPE _tmp984=Cyc_YY19(((struct _tuple12*(*)(struct _tuple12*x))Cyc_Parse_flat_imp_rev)(Cyc_yyget_YY19(yyyyvsp[0])));yyval=_tmp984;});
goto _LL0;}case 121U: _LLED: _LLEE: {
# 1771
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1773
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp986=Cyc_YY19(({struct _tuple12*_tmp474=_region_malloc(yyr,sizeof(*_tmp474));_tmp474->tl=0,({struct _tuple11 _tmp985=Cyc_yyget_YY18(yyyyvsp[0]);_tmp474->hd=_tmp985;});_tmp474;}));yyval=_tmp986;});
goto _LL0;}case 122U: _LLEF: _LLF0: {
# 1777
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1779
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1776 "parse.y"
({union Cyc_YYSTYPE _tmp989=Cyc_YY19(({struct _tuple12*_tmp475=_region_malloc(yyr,sizeof(*_tmp475));({struct _tuple12*_tmp988=Cyc_yyget_YY19(yyyyvsp[0]);_tmp475->tl=_tmp988;}),({struct _tuple11 _tmp987=Cyc_yyget_YY18(yyyyvsp[2]);_tmp475->hd=_tmp987;});_tmp475;}));yyval=_tmp989;});
goto _LL0;}case 123U: _LLF1: _LLF2: {
# 1779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1781 "parse.y"
({union Cyc_YYSTYPE _tmp98B=Cyc_YY18(({struct _tuple11 _tmp70B;({struct Cyc_Parse_Declarator _tmp98A=Cyc_yyget_YY27(yyyyvsp[0]);_tmp70B.f1=_tmp98A;}),_tmp70B.f2=0;_tmp70B;}));yyval=_tmp98B;});
goto _LL0;}case 124U: _LLF3: _LLF4: {
# 1784
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1786
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1783 "parse.y"
({union Cyc_YYSTYPE _tmp98E=Cyc_YY18(({struct _tuple11 _tmp70C;({struct Cyc_Parse_Declarator _tmp98D=Cyc_yyget_YY27(yyyyvsp[0]);_tmp70C.f1=_tmp98D;}),({struct Cyc_Absyn_Exp*_tmp98C=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp70C.f2=_tmp98C;});_tmp70C;}));yyval=_tmp98E;});
goto _LL0;}case 125U: _LLF5: _LLF6: {
# 1786
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1788
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1789 "parse.y"
struct _RegionHandle _tmp476=_new_region("temp");struct _RegionHandle*temp=& _tmp476;_push_region(temp);
{struct _tuple25 _tmp477=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp478=_tmp477;struct Cyc_Absyn_Tqual _tmp486;struct Cyc_Parse_Type_specifier _tmp485;struct Cyc_List_List*_tmp484;_LL439: _tmp486=_tmp478.f1;_tmp485=_tmp478.f2;_tmp484=_tmp478.f3;_LL43A:;
if(_tmp486.loc == 0)_tmp486.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct _tuple10*decls=0;
struct Cyc_List_List*widths_and_reqs=0;
{struct Cyc_List_List*_tmp479=Cyc_yyget_YY29(yyyyvsp[1]);for(0;_tmp479 != 0;_tmp479=_tmp479->tl){
struct _tuple24*_tmp47A=(struct _tuple24*)_tmp479->hd;struct _tuple24*_tmp47B=_tmp47A;struct Cyc_Parse_Declarator _tmp481;struct Cyc_Absyn_Exp*_tmp480;struct Cyc_Absyn_Exp*_tmp47F;_LL43C: _tmp481=_tmp47B->f1;_tmp480=_tmp47B->f2;_tmp47F=_tmp47B->f3;_LL43D:;
({struct _tuple10*_tmp98F=({struct _tuple10*_tmp47C=_region_malloc(temp,sizeof(*_tmp47C));_tmp47C->tl=decls,_tmp47C->hd=_tmp481;_tmp47C;});decls=_tmp98F;});
({struct Cyc_List_List*_tmp991=({struct Cyc_List_List*_tmp47E=_region_malloc(temp,sizeof(*_tmp47E));
({struct _tuple16*_tmp990=({struct _tuple16*_tmp47D=_region_malloc(temp,sizeof(*_tmp47D));_tmp47D->f1=_tmp480,_tmp47D->f2=_tmp47F;_tmp47D;});_tmp47E->hd=_tmp990;}),_tmp47E->tl=widths_and_reqs;_tmp47E;});
# 1797
widths_and_reqs=_tmp991;});}}
# 1800
({struct _tuple10*_tmp992=((struct _tuple10*(*)(struct _tuple10*x))Cyc_Parse_flat_imp_rev)(decls);decls=_tmp992;});
({struct Cyc_List_List*_tmp993=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(widths_and_reqs);widths_and_reqs=_tmp993;});{
void*_tmp482=Cyc_Parse_speclist2typ(_tmp485,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp483=({struct _RegionHandle*_tmp996=temp;struct _RegionHandle*_tmp995=temp;struct Cyc_List_List*_tmp994=
Cyc_Parse_apply_tmss(temp,_tmp486,_tmp482,decls,_tmp484);
# 1803
((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp996,_tmp995,_tmp994,widths_and_reqs);});
# 1806
({union Cyc_YYSTYPE _tmp997=Cyc_YY24(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(unsigned int,struct _tuple17*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_make_aggr_field,(unsigned int)(yyyylsp[0]).first_line,_tmp483));yyval=_tmp997;});
# 1808
_npop_handler(0U);goto _LL0;};};}
# 1790
;_pop_region(temp);}case 126U: _LLF7: _LLF8: {
# 1810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1816 "parse.y"
({union Cyc_YYSTYPE _tmp99A=Cyc_YY35(({struct _tuple25 _tmp70D;({struct Cyc_Absyn_Tqual _tmp999=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp70D.f1=_tmp999;}),({struct Cyc_Parse_Type_specifier _tmp998=Cyc_yyget_YY21(yyyyvsp[0]);_tmp70D.f2=_tmp998;}),_tmp70D.f3=0;_tmp70D;}));yyval=_tmp99A;});
goto _LL0;}case 127U: _LLF9: _LLFA: {
# 1819
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1821
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1818 "parse.y"
struct _tuple25 _tmp487=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp99E=Cyc_YY35(({struct _tuple25 _tmp70E;_tmp70E.f1=_tmp487.f1,({struct Cyc_Parse_Type_specifier _tmp99D=({unsigned int _tmp99C=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp99B=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp99C,_tmp99B,_tmp487.f2);});_tmp70E.f2=_tmp99D;}),_tmp70E.f3=_tmp487.f3;_tmp70E;}));yyval=_tmp99E;});
goto _LL0;}case 128U: _LLFB: _LLFC: {
# 1821
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1823
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1820 "parse.y"
({union Cyc_YYSTYPE _tmp9A1=Cyc_YY35(({struct _tuple25 _tmp70F;({struct Cyc_Absyn_Tqual _tmp9A0=Cyc_yyget_YY23(yyyyvsp[0]);_tmp70F.f1=_tmp9A0;}),({struct Cyc_Parse_Type_specifier _tmp99F=Cyc_Parse_empty_spec(0U);_tmp70F.f2=_tmp99F;}),_tmp70F.f3=0;_tmp70F;}));yyval=_tmp9A1;});
goto _LL0;}case 129U: _LLFD: _LLFE: {
# 1823
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1825
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1822 "parse.y"
struct _tuple25 _tmp488=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp9A4=Cyc_YY35(({struct _tuple25 _tmp710;({struct Cyc_Absyn_Tqual _tmp9A3=({struct Cyc_Absyn_Tqual _tmp9A2=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp9A2,_tmp488.f1);});_tmp710.f1=_tmp9A3;}),_tmp710.f2=_tmp488.f2,_tmp710.f3=_tmp488.f3;_tmp710;}));yyval=_tmp9A4;});
goto _LL0;}case 130U: _LLFF: _LL100: {
# 1826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1825 "parse.y"
({union Cyc_YYSTYPE _tmp9A8=Cyc_YY35(({struct _tuple25 _tmp711;({struct Cyc_Absyn_Tqual _tmp9A7=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp711.f1=_tmp9A7;}),({struct Cyc_Parse_Type_specifier _tmp9A6=Cyc_Parse_empty_spec(0U);_tmp711.f2=_tmp9A6;}),({struct Cyc_List_List*_tmp9A5=Cyc_yyget_YY45(yyyyvsp[0]);_tmp711.f3=_tmp9A5;});_tmp711;}));yyval=_tmp9A8;});
goto _LL0;}case 131U: _LL101: _LL102: {
# 1828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1827 "parse.y"
struct _tuple25 _tmp489=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9AB=Cyc_YY35(({struct _tuple25 _tmp712;_tmp712.f1=_tmp489.f1,_tmp712.f2=_tmp489.f2,({struct Cyc_List_List*_tmp9AA=({struct Cyc_List_List*_tmp9A9=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9A9,_tmp489.f3);});_tmp712.f3=_tmp9AA;});_tmp712;}));yyval=_tmp9AB;});
goto _LL0;}case 132U: _LL103: _LL104: {
# 1830
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1832
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9AE=Cyc_YY35(({struct _tuple25 _tmp713;({struct Cyc_Absyn_Tqual _tmp9AD=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp713.f1=_tmp9AD;}),({struct Cyc_Parse_Type_specifier _tmp9AC=Cyc_yyget_YY21(yyyyvsp[0]);_tmp713.f2=_tmp9AC;}),_tmp713.f3=0;_tmp713;}));yyval=_tmp9AE;});
goto _LL0;}case 133U: _LL105: _LL106: {
# 1836
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1838
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1835 "parse.y"
struct _tuple25 _tmp48A=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9B2=Cyc_YY35(({struct _tuple25 _tmp714;_tmp714.f1=_tmp48A.f1,({struct Cyc_Parse_Type_specifier _tmp9B1=({unsigned int _tmp9B0=(unsigned int)(yyyylsp[0]).first_line;struct Cyc_Parse_Type_specifier _tmp9AF=Cyc_yyget_YY21(yyyyvsp[0]);Cyc_Parse_combine_specifiers(_tmp9B0,_tmp9AF,_tmp48A.f2);});_tmp714.f2=_tmp9B1;}),_tmp714.f3=_tmp48A.f3;_tmp714;}));yyval=_tmp9B2;});
goto _LL0;}case 134U: _LL107: _LL108: {
# 1838
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1840
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1837 "parse.y"
({union Cyc_YYSTYPE _tmp9B5=Cyc_YY35(({struct _tuple25 _tmp715;({struct Cyc_Absyn_Tqual _tmp9B4=Cyc_yyget_YY23(yyyyvsp[0]);_tmp715.f1=_tmp9B4;}),({struct Cyc_Parse_Type_specifier _tmp9B3=Cyc_Parse_empty_spec(0U);_tmp715.f2=_tmp9B3;}),_tmp715.f3=0;_tmp715;}));yyval=_tmp9B5;});
goto _LL0;}case 135U: _LL109: _LL10A: {
# 1840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1839 "parse.y"
struct _tuple25 _tmp48B=Cyc_yyget_YY35(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmp9B8=Cyc_YY35(({struct _tuple25 _tmp716;({struct Cyc_Absyn_Tqual _tmp9B7=({struct Cyc_Absyn_Tqual _tmp9B6=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmp9B6,_tmp48B.f1);});_tmp716.f1=_tmp9B7;}),_tmp716.f2=_tmp48B.f2,_tmp716.f3=_tmp48B.f3;_tmp716;}));yyval=_tmp9B8;});
goto _LL0;}case 136U: _LL10B: _LL10C: {
# 1843
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1845
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1842 "parse.y"
({union Cyc_YYSTYPE _tmp9BC=Cyc_YY35(({struct _tuple25 _tmp717;({struct Cyc_Absyn_Tqual _tmp9BB=Cyc_Absyn_empty_tqual((unsigned int)(yyyylsp[0]).first_line);_tmp717.f1=_tmp9BB;}),({struct Cyc_Parse_Type_specifier _tmp9BA=Cyc_Parse_empty_spec(0U);_tmp717.f2=_tmp9BA;}),({struct Cyc_List_List*_tmp9B9=Cyc_yyget_YY45(yyyyvsp[0]);_tmp717.f3=_tmp9B9;});_tmp717;}));yyval=_tmp9BC;});
goto _LL0;}case 137U: _LL10D: _LL10E: {
# 1845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1844 "parse.y"
struct _tuple25 _tmp48C=Cyc_yyget_YY35(yyyyvsp[1]);({union Cyc_YYSTYPE _tmp9BF=Cyc_YY35(({struct _tuple25 _tmp718;_tmp718.f1=_tmp48C.f1,_tmp718.f2=_tmp48C.f2,({struct Cyc_List_List*_tmp9BE=({struct Cyc_List_List*_tmp9BD=Cyc_yyget_YY45(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9BD,_tmp48C.f3);});_tmp718.f3=_tmp9BE;});_tmp718;}));yyval=_tmp9BF;});
goto _LL0;}case 138U: _LL10F: _LL110: {
# 1847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1848 "parse.y"
({union Cyc_YYSTYPE _tmp9C0=Cyc_YY29(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY29(yyyyvsp[0])));yyval=_tmp9C0;});
goto _LL0;}case 139U: _LL111: _LL112: {
# 1851
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1853
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmp9C2=Cyc_YY29(({struct Cyc_List_List*_tmp48D=_region_malloc(yyr,sizeof(*_tmp48D));({struct _tuple24*_tmp9C1=Cyc_yyget_YY28(yyyyvsp[0]);_tmp48D->hd=_tmp9C1;}),_tmp48D->tl=0;_tmp48D;}));yyval=_tmp9C2;});
goto _LL0;}case 140U: _LL113: _LL114: {
# 1857
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1859
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1856 "parse.y"
({union Cyc_YYSTYPE _tmp9C5=Cyc_YY29(({struct Cyc_List_List*_tmp48E=_region_malloc(yyr,sizeof(*_tmp48E));({struct _tuple24*_tmp9C4=Cyc_yyget_YY28(yyyyvsp[2]);_tmp48E->hd=_tmp9C4;}),({struct Cyc_List_List*_tmp9C3=Cyc_yyget_YY29(yyyyvsp[0]);_tmp48E->tl=_tmp9C3;});_tmp48E;}));yyval=_tmp9C5;});
goto _LL0;}case 141U: _LL115: _LL116: {
# 1859
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1861
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1861 "parse.y"
({union Cyc_YYSTYPE _tmp9C8=Cyc_YY28(({struct _tuple24*_tmp48F=_region_malloc(yyr,sizeof(*_tmp48F));({struct Cyc_Parse_Declarator _tmp9C7=Cyc_yyget_YY27(yyyyvsp[0]);_tmp48F->f1=_tmp9C7;}),_tmp48F->f2=0,({struct Cyc_Absyn_Exp*_tmp9C6=Cyc_yyget_YY55(yyyyvsp[1]);_tmp48F->f3=_tmp9C6;});_tmp48F;}));yyval=_tmp9C8;});
goto _LL0;}case 142U: _LL117: _LL118: {
# 1864
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1866
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1865 "parse.y"
({union Cyc_YYSTYPE _tmp9CE=Cyc_YY28(({struct _tuple24*_tmp493=_region_malloc(yyr,sizeof(*_tmp493));({struct _tuple0*_tmp9CD=({struct _tuple0*_tmp492=_cycalloc(sizeof(*_tmp492));({union Cyc_Absyn_Nmspace _tmp9CC=Cyc_Absyn_Rel_n(0);_tmp492->f1=_tmp9CC;}),({struct _dyneither_ptr*_tmp9CB=({struct _dyneither_ptr*_tmp491=_cycalloc(sizeof(*_tmp491));({struct _dyneither_ptr _tmp9CA=({const char*_tmp490="";_tag_dyneither(_tmp490,sizeof(char),1U);});*_tmp491=_tmp9CA;});_tmp491;});_tmp492->f2=_tmp9CB;});_tmp492;});(_tmp493->f1).id=_tmp9CD;}),(_tmp493->f1).varloc=0U,(_tmp493->f1).tms=0,({struct Cyc_Absyn_Exp*_tmp9C9=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp493->f2=_tmp9C9;}),_tmp493->f3=0;_tmp493;}));yyval=_tmp9CE;});
# 1867
goto _LL0;}case 143U: _LL119: _LL11A:
# 1871
({union Cyc_YYSTYPE _tmp9D3=Cyc_YY28(({struct _tuple24*_tmp497=_region_malloc(yyr,sizeof(*_tmp497));({struct _tuple0*_tmp9D2=({struct _tuple0*_tmp496=_cycalloc(sizeof(*_tmp496));({union Cyc_Absyn_Nmspace _tmp9D1=Cyc_Absyn_Rel_n(0);_tmp496->f1=_tmp9D1;}),({struct _dyneither_ptr*_tmp9D0=({struct _dyneither_ptr*_tmp495=_cycalloc(sizeof(*_tmp495));({struct _dyneither_ptr _tmp9CF=({const char*_tmp494="";_tag_dyneither(_tmp494,sizeof(char),1U);});*_tmp495=_tmp9CF;});_tmp495;});_tmp496->f2=_tmp9D0;});_tmp496;});(_tmp497->f1).id=_tmp9D2;}),(_tmp497->f1).varloc=0U,(_tmp497->f1).tms=0,_tmp497->f2=0,_tmp497->f3=0;_tmp497;}));yyval=_tmp9D3;});
# 1873
goto _LL0;case 144U: _LL11B: _LL11C: {
# 1875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1873 "parse.y"
({union Cyc_YYSTYPE _tmp9D6=Cyc_YY28(({struct _tuple24*_tmp498=_region_malloc(yyr,sizeof(*_tmp498));({struct Cyc_Parse_Declarator _tmp9D5=Cyc_yyget_YY27(yyyyvsp[0]);_tmp498->f1=_tmp9D5;}),({struct Cyc_Absyn_Exp*_tmp9D4=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp498->f2=_tmp9D4;}),_tmp498->f3=0;_tmp498;}));yyval=_tmp9D6;});
goto _LL0;}case 145U: _LL11D: _LL11E: {
# 1876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1877 "parse.y"
({union Cyc_YYSTYPE _tmp9D7=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmp9D7;});
goto _LL0;}case 146U: _LL11F: _LL120:
# 1880
({union Cyc_YYSTYPE _tmp9D8=Cyc_YY55(0);yyval=_tmp9D8;});
goto _LL0;case 147U: _LL121: _LL122: {
# 1883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1882 "parse.y"
({union Cyc_YYSTYPE _tmp9D9=Cyc_YY55(Cyc_yyget_Exp_tok(yyyyvsp[2]));yyval=_tmp9D9;});
goto _LL0;}case 148U: _LL123: _LL124:
# 1885
({union Cyc_YYSTYPE _tmp9DA=Cyc_YY55(0);yyval=_tmp9DA;});
goto _LL0;case 149U: _LL125: _LL126: {
# 1888
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1890
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1889 "parse.y"
int _tmp499=Cyc_yyget_YY31(yyyyvsp[0]);
struct Cyc_List_List*_tmp49A=({unsigned int _tmp9DB=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmp9DB,Cyc_yyget_YY40(yyyyvsp[2]));});
struct Cyc_Absyn_TypeDecl*_tmp49B=({struct _tuple0*_tmp9E0=Cyc_yyget_QualId_tok(yyyyvsp[1]);struct Cyc_List_List*_tmp9DF=_tmp49A;struct Cyc_Core_Opt*_tmp9DE=({struct Cyc_Core_Opt*_tmp49D=_cycalloc(sizeof(*_tmp49D));({struct Cyc_List_List*_tmp9DC=Cyc_yyget_YY34(yyyyvsp[4]);_tmp49D->v=_tmp9DC;});_tmp49D;});int _tmp9DD=_tmp499;Cyc_Absyn_datatype_tdecl(Cyc_Absyn_Public,_tmp9E0,_tmp9DF,_tmp9DE,_tmp9DD,(unsigned int)(yyyylsp[0]).first_line);});
# 1893
({union Cyc_YYSTYPE _tmp9E2=Cyc_YY21(({void*_tmp9E1=(void*)({struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C->tag=10U,_tmp49C->f1=_tmp49B,_tmp49C->f2=0;_tmp49C;});Cyc_Parse_type_spec(_tmp9E1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9E2;});
# 1895
goto _LL0;}case 150U: _LL127: _LL128: {
# 1897
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1899
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1896 "parse.y"
int _tmp49E=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9E6=Cyc_YY21(({void*_tmp9E5=({union Cyc_Absyn_DatatypeInfo _tmp9E4=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp719;({struct _tuple0*_tmp9E3=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp719.name=_tmp9E3;}),_tmp719.is_extensible=_tmp49E;_tmp719;}));Cyc_Absyn_datatype_type(_tmp9E4,Cyc_yyget_YY40(yyyyvsp[2]));});Cyc_Parse_type_spec(_tmp9E5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9E6;});
# 1899
goto _LL0;}case 151U: _LL129: _LL12A: {
# 1901
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1903
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1900 "parse.y"
int _tmp49F=Cyc_yyget_YY31(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmp9EB=Cyc_YY21(({void*_tmp9EA=({union Cyc_Absyn_DatatypeFieldInfo _tmp9E9=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp71A;({struct _tuple0*_tmp9E8=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp71A.datatype_name=_tmp9E8;}),({struct _tuple0*_tmp9E7=Cyc_yyget_QualId_tok(yyyyvsp[3]);_tmp71A.field_name=_tmp9E7;}),_tmp71A.is_extensible=_tmp49F;_tmp71A;}));Cyc_Absyn_datatype_field_type(_tmp9E9,Cyc_yyget_YY40(yyyyvsp[4]));});Cyc_Parse_type_spec(_tmp9EA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmp9EB;});
# 1903
goto _LL0;}case 152U: _LL12B: _LL12C: {
# 1905
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1907
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1906 "parse.y"
({union Cyc_YYSTYPE _tmp9EC=Cyc_YY31(0);yyval=_tmp9EC;});
goto _LL0;}case 153U: _LL12D: _LL12E: {
# 1909
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1911
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1907 "parse.y"
({union Cyc_YYSTYPE _tmp9ED=Cyc_YY31(1);yyval=_tmp9ED;});
goto _LL0;}case 154U: _LL12F: _LL130: {
# 1910
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1912
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1911 "parse.y"
({union Cyc_YYSTYPE _tmp9EF=Cyc_YY34(({struct Cyc_List_List*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));({struct Cyc_Absyn_Datatypefield*_tmp9EE=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4A0->hd=_tmp9EE;}),_tmp4A0->tl=0;_tmp4A0;}));yyval=_tmp9EF;});
goto _LL0;}case 155U: _LL131: _LL132: {
# 1914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1912 "parse.y"
({union Cyc_YYSTYPE _tmp9F1=Cyc_YY34(({struct Cyc_List_List*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));({struct Cyc_Absyn_Datatypefield*_tmp9F0=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4A1->hd=_tmp9F0;}),_tmp4A1->tl=0;_tmp4A1;}));yyval=_tmp9F1;});
goto _LL0;}case 156U: _LL133: _LL134: {
# 1915
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1917
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1913 "parse.y"
({union Cyc_YYSTYPE _tmp9F4=Cyc_YY34(({struct Cyc_List_List*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));({struct Cyc_Absyn_Datatypefield*_tmp9F3=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4A2->hd=_tmp9F3;}),({struct Cyc_List_List*_tmp9F2=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4A2->tl=_tmp9F2;});_tmp4A2;}));yyval=_tmp9F4;});
goto _LL0;}case 157U: _LL135: _LL136: {
# 1916
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1918
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1914 "parse.y"
({union Cyc_YYSTYPE _tmp9F7=Cyc_YY34(({struct Cyc_List_List*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));({struct Cyc_Absyn_Datatypefield*_tmp9F6=Cyc_yyget_YY33(yyyyvsp[0]);_tmp4A3->hd=_tmp9F6;}),({struct Cyc_List_List*_tmp9F5=Cyc_yyget_YY34(yyyyvsp[2]);_tmp4A3->tl=_tmp9F5;});_tmp4A3;}));yyval=_tmp9F7;});
goto _LL0;}case 158U: _LL137: _LL138:
# 1917
({union Cyc_YYSTYPE _tmp9F8=Cyc_YY32(Cyc_Absyn_Public);yyval=_tmp9F8;});
goto _LL0;case 159U: _LL139: _LL13A: {
# 1920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1919 "parse.y"
({union Cyc_YYSTYPE _tmp9F9=Cyc_YY32(Cyc_Absyn_Extern);yyval=_tmp9F9;});
goto _LL0;}case 160U: _LL13B: _LL13C: {
# 1922
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1924
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1920 "parse.y"
({union Cyc_YYSTYPE _tmp9FA=Cyc_YY32(Cyc_Absyn_Static);yyval=_tmp9FA;});
goto _LL0;}case 161U: _LL13D: _LL13E: {
# 1923
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1925
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1924 "parse.y"
({union Cyc_YYSTYPE _tmp9FD=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));({struct _tuple0*_tmp9FC=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4A4->name=_tmp9FC;}),_tmp4A4->typs=0,_tmp4A4->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp9FB=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4A4->sc=_tmp9FB;});_tmp4A4;}));yyval=_tmp9FD;});
goto _LL0;}case 162U: _LL13F: _LL140: {
# 1927
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1929
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1926 "parse.y"
struct Cyc_List_List*_tmp4A5=({unsigned int _tmp9FE=(unsigned int)(yyyylsp[3]).first_line;((struct Cyc_List_List*(*)(struct _tuple19*(*f)(unsigned int,struct _tuple8*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_get_tqual_typ,_tmp9FE,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[3])));});
({union Cyc_YYSTYPE _tmpA01=Cyc_YY33(({struct Cyc_Absyn_Datatypefield*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));({struct _tuple0*_tmpA00=Cyc_yyget_QualId_tok(yyyyvsp[1]);_tmp4A6->name=_tmpA00;}),_tmp4A6->typs=_tmp4A5,_tmp4A6->loc=(unsigned int)(yyyylsp[0]).first_line,({enum Cyc_Absyn_Scope _tmp9FF=Cyc_yyget_YY32(yyyyvsp[0]);_tmp4A6->sc=_tmp9FF;});_tmp4A6;}));yyval=_tmpA01;});
goto _LL0;}case 163U: _LL141: _LL142: {
# 1930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1932 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 164U: _LL143: _LL144: {
# 1935
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1937
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1934 "parse.y"
struct Cyc_Parse_Declarator _tmp4A7=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA04=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71B;_tmp71B.id=_tmp4A7.id,_tmp71B.varloc=_tmp4A7.varloc,({struct Cyc_List_List*_tmpA03=({struct Cyc_List_List*_tmpA02=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA02,_tmp4A7.tms);});_tmp71B.tms=_tmpA03;});_tmp71B;}));yyval=_tmpA04;});
goto _LL0;}case 165U: _LL145: _LL146: {
# 1938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 166U: _LL147: _LL148: {
# 1944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1943 "parse.y"
struct Cyc_Parse_Declarator _tmp4A8=Cyc_yyget_YY27(yyyyvsp[1]);
({union Cyc_YYSTYPE _tmpA07=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71C;_tmp71C.id=_tmp4A8.id,_tmp71C.varloc=_tmp4A8.varloc,({struct Cyc_List_List*_tmpA06=({struct Cyc_List_List*_tmpA05=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA05,_tmp4A8.tms);});_tmp71C.tms=_tmpA06;});_tmp71C;}));yyval=_tmpA07;});
goto _LL0;}case 167U: _LL149: _LL14A: {
# 1947
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1949
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1949 "parse.y"
({union Cyc_YYSTYPE _tmpA09=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71D;({struct _tuple0*_tmpA08=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp71D.id=_tmpA08;}),_tmp71D.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp71D.tms=0;_tmp71D;}));yyval=_tmpA09;});
goto _LL0;}case 168U: _LL14B: _LL14C: {
# 1952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 1954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1951 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 169U: _LL14D: _LL14E: {
# 1954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1955 "parse.y"
struct Cyc_Parse_Declarator _tmp4A9=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpA0C=({struct Cyc_List_List*_tmp4AB=_region_malloc(yyr,sizeof(*_tmp4AB));({void*_tmpA0B=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4AA=_region_malloc(yyr,sizeof(*_tmp4AA));_tmp4AA->tag=5U,_tmp4AA->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA0A=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4AA->f2=_tmpA0A;});_tmp4AA;});_tmp4AB->hd=_tmpA0B;}),_tmp4AB->tl=_tmp4A9.tms;_tmp4AB;});_tmp4A9.tms=_tmpA0C;});
yyval=yyyyvsp[2];
# 1959
goto _LL0;}case 170U: _LL14F: _LL150: {
# 1961
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1963
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1960 "parse.y"
({union Cyc_YYSTYPE _tmpA13=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71E;({struct _tuple0*_tmpA12=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp71E.id=_tmpA12;}),({unsigned int _tmpA11=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp71E.varloc=_tmpA11;}),({struct Cyc_List_List*_tmpA10=({struct Cyc_List_List*_tmp4AD=_region_malloc(yyr,sizeof(*_tmp4AD));({void*_tmpA0F=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4AC=_region_malloc(yyr,sizeof(*_tmp4AC));_tmp4AC->tag=0U,({void*_tmpA0E=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4AC->f1=_tmpA0E;}),_tmp4AC->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4AC;});_tmp4AD->hd=_tmpA0F;}),({struct Cyc_List_List*_tmpA0D=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4AD->tl=_tmpA0D;});_tmp4AD;});_tmp71E.tms=_tmpA10;});_tmp71E;}));yyval=_tmpA13;});
goto _LL0;}case 171U: _LL151: _LL152: {
# 1963
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 1965
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 1962 "parse.y"
({union Cyc_YYSTYPE _tmpA1B=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp71F;({struct _tuple0*_tmpA1A=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp71F.id=_tmpA1A;}),({unsigned int _tmpA19=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp71F.varloc=_tmpA19;}),({
struct Cyc_List_List*_tmpA18=({struct Cyc_List_List*_tmp4AF=_region_malloc(yyr,sizeof(*_tmp4AF));({void*_tmpA17=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4AE=_region_malloc(yyr,sizeof(*_tmp4AE));_tmp4AE->tag=1U,({struct Cyc_Absyn_Exp*_tmpA16=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4AE->f1=_tmpA16;}),({void*_tmpA15=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4AE->f2=_tmpA15;}),_tmp4AE->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4AE;});_tmp4AF->hd=_tmpA17;}),({struct Cyc_List_List*_tmpA14=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4AF->tl=_tmpA14;});_tmp4AF;});_tmp71F.tms=_tmpA18;});_tmp71F;}));
# 1962
yyval=_tmpA1B;});
# 1964
goto _LL0;}case 172U: _LL153: _LL154: {
# 1966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 1968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 1965 "parse.y"
struct _tuple26*_tmp4B0=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4B1=_tmp4B0;struct Cyc_List_List*_tmp4BB;int _tmp4BA;struct Cyc_Absyn_VarargInfo*_tmp4B9;void*_tmp4B8;struct Cyc_List_List*_tmp4B7;_LL43F: _tmp4BB=_tmp4B1->f1;_tmp4BA=_tmp4B1->f2;_tmp4B9=_tmp4B1->f3;_tmp4B8=_tmp4B1->f4;_tmp4B7=_tmp4B1->f5;_LL440:;{
struct Cyc_Absyn_Exp*_tmp4B2=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_yyget_YY55(yyyyvsp[5]);
({union Cyc_YYSTYPE _tmpA22=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp720;({struct _tuple0*_tmpA21=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp720.id=_tmpA21;}),({unsigned int _tmpA20=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp720.varloc=_tmpA20;}),({struct Cyc_List_List*_tmpA1F=({struct Cyc_List_List*_tmp4B6=_region_malloc(yyr,sizeof(*_tmp4B6));({void*_tmpA1E=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4B5=_region_malloc(yyr,sizeof(*_tmp4B5));_tmp4B5->tag=3U,({void*_tmpA1D=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4B4=_region_malloc(yyr,sizeof(*_tmp4B4));_tmp4B4->tag=1U,_tmp4B4->f1=_tmp4BB,_tmp4B4->f2=_tmp4BA,_tmp4B4->f3=_tmp4B9,_tmp4B4->f4=_tmp4B8,_tmp4B4->f5=_tmp4B7,_tmp4B4->f6=_tmp4B2,_tmp4B4->f7=_tmp4B3;_tmp4B4;});_tmp4B5->f1=_tmpA1D;});_tmp4B5;});_tmp4B6->hd=_tmpA1E;}),({struct Cyc_List_List*_tmpA1C=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4B6->tl=_tmpA1C;});_tmp4B6;});_tmp720.tms=_tmpA1F;});_tmp720;}));yyval=_tmpA22;});
# 1970
goto _LL0;};}case 173U: _LL155: _LL156: {
# 1972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 1974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 1971 "parse.y"
({union Cyc_YYSTYPE _tmpA2D=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp721;({struct _tuple0*_tmpA2C=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp721.id=_tmpA2C;}),({unsigned int _tmpA2B=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp721.varloc=_tmpA2B;}),({
struct Cyc_List_List*_tmpA2A=({struct Cyc_List_List*_tmp4BE=_region_malloc(yyr,sizeof(*_tmp4BE));({void*_tmpA29=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4BD=_region_malloc(yyr,sizeof(*_tmp4BD));_tmp4BD->tag=3U,({void*_tmpA28=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4BC=_region_malloc(yyr,sizeof(*_tmp4BC));_tmp4BC->tag=1U,_tmp4BC->f1=0,_tmp4BC->f2=0,_tmp4BC->f3=0,({
# 1974
void*_tmpA27=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4BC->f4=_tmpA27;}),({struct Cyc_List_List*_tmpA26=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4BC->f5=_tmpA26;}),({struct Cyc_Absyn_Exp*_tmpA25=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4BC->f6=_tmpA25;}),({struct Cyc_Absyn_Exp*_tmpA24=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4BC->f7=_tmpA24;});_tmp4BC;});
# 1972
_tmp4BD->f1=_tmpA28;});_tmp4BD;});_tmp4BE->hd=_tmpA29;}),({
# 1975
struct Cyc_List_List*_tmpA23=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4BE->tl=_tmpA23;});_tmp4BE;});
# 1972
_tmp721.tms=_tmpA2A;});_tmp721;}));
# 1971
yyval=_tmpA2D;});
# 1977
goto _LL0;}case 174U: _LL157: _LL158: {
# 1979
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1981
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1978 "parse.y"
({union Cyc_YYSTYPE _tmpA35=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp722;({struct _tuple0*_tmpA34=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp722.id=_tmpA34;}),({unsigned int _tmpA33=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp722.varloc=_tmpA33;}),({struct Cyc_List_List*_tmpA32=({struct Cyc_List_List*_tmp4C1=_region_malloc(yyr,sizeof(*_tmp4C1));({void*_tmpA31=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4C0=_region_malloc(yyr,sizeof(*_tmp4C0));_tmp4C0->tag=3U,({void*_tmpA30=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4BF=_region_malloc(yyr,sizeof(*_tmp4BF));_tmp4BF->tag=0U,({struct Cyc_List_List*_tmpA2F=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4BF->f1=_tmpA2F;}),_tmp4BF->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4BF;});_tmp4C0->f1=_tmpA30;});_tmp4C0;});_tmp4C1->hd=_tmpA31;}),({struct Cyc_List_List*_tmpA2E=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4C1->tl=_tmpA2E;});_tmp4C1;});_tmp722.tms=_tmpA32;});_tmp722;}));yyval=_tmpA35;});
goto _LL0;}case 175U: _LL159: _LL15A: {
# 1981
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 1983
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 1981 "parse.y"
struct Cyc_List_List*_tmp4C2=({unsigned int _tmpA36=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA36,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpA3C=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp723;({struct _tuple0*_tmpA3B=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp723.id=_tmpA3B;}),({unsigned int _tmpA3A=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp723.varloc=_tmpA3A;}),({struct Cyc_List_List*_tmpA39=({struct Cyc_List_List*_tmp4C4=_region_malloc(yyr,sizeof(*_tmp4C4));({void*_tmpA38=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4C3=_region_malloc(yyr,sizeof(*_tmp4C3));_tmp4C3->tag=4U,_tmp4C3->f1=_tmp4C2,_tmp4C3->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4C3->f3=0;_tmp4C3;});_tmp4C4->hd=_tmpA38;}),({struct Cyc_List_List*_tmpA37=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4C4->tl=_tmpA37;});_tmp4C4;});_tmp723.tms=_tmpA39;});_tmp723;}));yyval=_tmpA3C;});
# 1984
goto _LL0;}case 176U: _LL15B: _LL15C: {
# 1986
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 1988
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 1985 "parse.y"
({union Cyc_YYSTYPE _tmpA43=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp724;({struct _tuple0*_tmpA42=(Cyc_yyget_YY27(yyyyvsp[0])).id;_tmp724.id=_tmpA42;}),({unsigned int _tmpA41=(Cyc_yyget_YY27(yyyyvsp[0])).varloc;_tmp724.varloc=_tmpA41;}),({struct Cyc_List_List*_tmpA40=({struct Cyc_List_List*_tmp4C6=_region_malloc(yyr,sizeof(*_tmp4C6));({void*_tmpA3F=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C5=_region_malloc(yyr,sizeof(*_tmp4C5));_tmp4C5->tag=5U,_tmp4C5->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA3E=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4C5->f2=_tmpA3E;});_tmp4C5;});_tmp4C6->hd=_tmpA3F;}),({
struct Cyc_List_List*_tmpA3D=(Cyc_yyget_YY27(yyyyvsp[0])).tms;_tmp4C6->tl=_tmpA3D;});_tmp4C6;});
# 1985
_tmp724.tms=_tmpA40;});_tmp724;}));yyval=_tmpA43;});
# 1988
goto _LL0;}case 177U: _LL15D: _LL15E: {
# 1990
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1992
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpA45=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp725;({struct _tuple0*_tmpA44=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp725.id=_tmpA44;}),_tmp725.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp725.tms=0;_tmp725;}));yyval=_tmpA45;});
goto _LL0;}case 178U: _LL15F: _LL160: {
# 1996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 1998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 1995 "parse.y"
({union Cyc_YYSTYPE _tmpA47=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp726;({struct _tuple0*_tmpA46=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp726.id=_tmpA46;}),_tmp726.varloc=(unsigned int)(yyyylsp[0]).first_line,_tmp726.tms=0;_tmp726;}));yyval=_tmpA47;});
goto _LL0;}case 179U: _LL161: _LL162: {
# 1998
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 1997 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 180U: _LL163: _LL164: {
# 2000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2001 "parse.y"
struct Cyc_Parse_Declarator _tmp4C7=Cyc_yyget_YY27(yyyyvsp[2]);
({struct Cyc_List_List*_tmpA4A=({struct Cyc_List_List*_tmp4C9=_region_malloc(yyr,sizeof(*_tmp4C9));({void*_tmpA49=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4C8=_region_malloc(yyr,sizeof(*_tmp4C8));_tmp4C8->tag=5U,_tmp4C8->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA48=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4C8->f2=_tmpA48;});_tmp4C8;});_tmp4C9->hd=_tmpA49;}),_tmp4C9->tl=_tmp4C7.tms;_tmp4C9;});_tmp4C7.tms=_tmpA4A;});
yyval=yyyyvsp[2];
# 2005
goto _LL0;}case 181U: _LL165: _LL166: {
# 2007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2006 "parse.y"
struct Cyc_Parse_Declarator _tmp4CA=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA4E=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp727;_tmp727.id=_tmp4CA.id,_tmp727.varloc=_tmp4CA.varloc,({
struct Cyc_List_List*_tmpA4D=({struct Cyc_List_List*_tmp4CC=_region_malloc(yyr,sizeof(*_tmp4CC));({void*_tmpA4C=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp4CB=_region_malloc(yyr,sizeof(*_tmp4CB));_tmp4CB->tag=0U,({void*_tmpA4B=Cyc_yyget_YY51(yyyyvsp[3]);_tmp4CB->f1=_tmpA4B;}),_tmp4CB->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp4CB;});_tmp4CC->hd=_tmpA4C;}),_tmp4CC->tl=_tmp4CA.tms;_tmp4CC;});_tmp727.tms=_tmpA4D;});_tmp727;}));
# 2007
yyval=_tmpA4E;});
# 2009
goto _LL0;}case 182U: _LL167: _LL168: {
# 2011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2010 "parse.y"
struct Cyc_Parse_Declarator _tmp4CD=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA53=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp728;_tmp728.id=_tmp4CD.id,_tmp728.varloc=_tmp4CD.varloc,({
struct Cyc_List_List*_tmpA52=({struct Cyc_List_List*_tmp4CF=_region_malloc(yyr,sizeof(*_tmp4CF));({void*_tmpA51=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp4CE=_region_malloc(yyr,sizeof(*_tmp4CE));_tmp4CE->tag=1U,({struct Cyc_Absyn_Exp*_tmpA50=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4CE->f1=_tmpA50;}),({void*_tmpA4F=Cyc_yyget_YY51(yyyyvsp[4]);_tmp4CE->f2=_tmpA4F;}),_tmp4CE->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp4CE;});_tmp4CF->hd=_tmpA51;}),_tmp4CF->tl=_tmp4CD.tms;_tmp4CF;});_tmp728.tms=_tmpA52;});_tmp728;}));
# 2011
yyval=_tmpA53;});
# 2014
goto _LL0;}case 183U: _LL169: _LL16A: {
# 2016
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2018
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2015 "parse.y"
struct _tuple26*_tmp4D0=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp4D1=_tmp4D0;struct Cyc_List_List*_tmp4DC;int _tmp4DB;struct Cyc_Absyn_VarargInfo*_tmp4DA;void*_tmp4D9;struct Cyc_List_List*_tmp4D8;_LL442: _tmp4DC=_tmp4D1->f1;_tmp4DB=_tmp4D1->f2;_tmp4DA=_tmp4D1->f3;_tmp4D9=_tmp4D1->f4;_tmp4D8=_tmp4D1->f5;_LL443:;{
struct Cyc_Absyn_Exp*_tmp4D2=Cyc_yyget_YY55(yyyyvsp[4]);
struct Cyc_Absyn_Exp*_tmp4D3=Cyc_yyget_YY55(yyyyvsp[5]);
struct Cyc_Parse_Declarator _tmp4D4=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA57=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp729;_tmp729.id=_tmp4D4.id,_tmp729.varloc=_tmp4D4.varloc,({struct Cyc_List_List*_tmpA56=({struct Cyc_List_List*_tmp4D7=_region_malloc(yyr,sizeof(*_tmp4D7));({void*_tmpA55=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4D6=_region_malloc(yyr,sizeof(*_tmp4D6));_tmp4D6->tag=3U,({void*_tmpA54=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4D5=_region_malloc(yyr,sizeof(*_tmp4D5));_tmp4D5->tag=1U,_tmp4D5->f1=_tmp4DC,_tmp4D5->f2=_tmp4DB,_tmp4D5->f3=_tmp4DA,_tmp4D5->f4=_tmp4D9,_tmp4D5->f5=_tmp4D8,_tmp4D5->f6=_tmp4D2,_tmp4D5->f7=_tmp4D3;_tmp4D5;});_tmp4D6->f1=_tmpA54;});_tmp4D6;});_tmp4D7->hd=_tmpA55;}),_tmp4D7->tl=_tmp4D4.tms;_tmp4D7;});_tmp729.tms=_tmpA56;});_tmp729;}));yyval=_tmpA57;});
# 2021
goto _LL0;};}case 184U: _LL16B: _LL16C: {
# 2023
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2025
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2022 "parse.y"
struct Cyc_Parse_Declarator _tmp4DD=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA5F=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72A;_tmp72A.id=_tmp4DD.id,_tmp72A.varloc=_tmp4DD.varloc,({
struct Cyc_List_List*_tmpA5E=({struct Cyc_List_List*_tmp4E0=_region_malloc(yyr,sizeof(*_tmp4E0));({void*_tmpA5D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4DF=_region_malloc(yyr,sizeof(*_tmp4DF));_tmp4DF->tag=3U,({void*_tmpA5C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp4DE=_region_malloc(yyr,sizeof(*_tmp4DE));_tmp4DE->tag=1U,_tmp4DE->f1=0,_tmp4DE->f2=0,_tmp4DE->f3=0,({
# 2026
void*_tmpA5B=Cyc_yyget_YY49(yyyyvsp[2]);_tmp4DE->f4=_tmpA5B;}),({struct Cyc_List_List*_tmpA5A=Cyc_yyget_YY50(yyyyvsp[3]);_tmp4DE->f5=_tmpA5A;}),({struct Cyc_Absyn_Exp*_tmpA59=Cyc_yyget_YY55(yyyyvsp[5]);_tmp4DE->f6=_tmpA59;}),({struct Cyc_Absyn_Exp*_tmpA58=Cyc_yyget_YY55(yyyyvsp[6]);_tmp4DE->f7=_tmpA58;});_tmp4DE;});
# 2024
_tmp4DF->f1=_tmpA5C;});_tmp4DF;});_tmp4E0->hd=_tmpA5D;}),_tmp4E0->tl=_tmp4DD.tms;_tmp4E0;});_tmp72A.tms=_tmpA5E;});_tmp72A;}));
# 2023
yyval=_tmpA5F;});
# 2029
goto _LL0;}case 185U: _LL16D: _LL16E: {
# 2031
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2033
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2030 "parse.y"
struct Cyc_Parse_Declarator _tmp4E1=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA64=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72B;_tmp72B.id=_tmp4E1.id,_tmp72B.varloc=_tmp4E1.varloc,({struct Cyc_List_List*_tmpA63=({struct Cyc_List_List*_tmp4E4=_region_malloc(yyr,sizeof(*_tmp4E4));({void*_tmpA62=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp4E3=_region_malloc(yyr,sizeof(*_tmp4E3));_tmp4E3->tag=3U,({void*_tmpA61=(void*)({struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*_tmp4E2=_region_malloc(yyr,sizeof(*_tmp4E2));_tmp4E2->tag=0U,({struct Cyc_List_List*_tmpA60=Cyc_yyget_YY36(yyyyvsp[2]);_tmp4E2->f1=_tmpA60;}),_tmp4E2->f2=(unsigned int)(yyyylsp[0]).first_line;_tmp4E2;});_tmp4E3->f1=_tmpA61;});_tmp4E3;});_tmp4E4->hd=_tmpA62;}),_tmp4E4->tl=_tmp4E1.tms;_tmp4E4;});_tmp72B.tms=_tmpA63;});_tmp72B;}));yyval=_tmpA64;});
goto _LL0;}case 186U: _LL16F: _LL170: {
# 2034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2034 "parse.y"
struct Cyc_List_List*_tmp4E5=({unsigned int _tmpA65=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpA65,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
struct Cyc_Parse_Declarator _tmp4E6=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA68=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72C;_tmp72C.id=_tmp4E6.id,_tmp72C.varloc=_tmp4E6.varloc,({struct Cyc_List_List*_tmpA67=({struct Cyc_List_List*_tmp4E8=_region_malloc(yyr,sizeof(*_tmp4E8));({void*_tmpA66=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp4E7=_region_malloc(yyr,sizeof(*_tmp4E7));_tmp4E7->tag=4U,_tmp4E7->f1=_tmp4E5,_tmp4E7->f2=(unsigned int)(yyyylsp[0]).first_line,_tmp4E7->f3=0;_tmp4E7;});_tmp4E8->hd=_tmpA66;}),_tmp4E8->tl=_tmp4E6.tms;_tmp4E8;});_tmp72C.tms=_tmpA67;});_tmp72C;}));yyval=_tmpA68;});
# 2038
goto _LL0;}case 187U: _LL171: _LL172: {
# 2040
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2042
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2039 "parse.y"
struct Cyc_Parse_Declarator _tmp4E9=Cyc_yyget_YY27(yyyyvsp[0]);
({union Cyc_YYSTYPE _tmpA6C=Cyc_YY27(({struct Cyc_Parse_Declarator _tmp72D;_tmp72D.id=_tmp4E9.id,_tmp72D.varloc=_tmp4E9.varloc,({struct Cyc_List_List*_tmpA6B=({struct Cyc_List_List*_tmp4EB=_region_malloc(yyr,sizeof(*_tmp4EB));({void*_tmpA6A=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EA=_region_malloc(yyr,sizeof(*_tmp4EA));_tmp4EA->tag=5U,_tmp4EA->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpA69=Cyc_yyget_YY45(yyyyvsp[1]);_tmp4EA->f2=_tmpA69;});_tmp4EA;});_tmp4EB->hd=_tmpA6A;}),_tmp4EB->tl=_tmp4E9.tms;_tmp4EB;});_tmp72D.tms=_tmpA6B;});_tmp72D;}));yyval=_tmpA6C;});
# 2042
goto _LL0;}case 188U: _LL173: _LL174: {
# 2044
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2046
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2046 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 189U: _LL175: _LL176: {
# 2049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2047 "parse.y"
({union Cyc_YYSTYPE _tmpA6E=Cyc_YY26(({struct Cyc_List_List*_tmpA6D=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpA6D,Cyc_yyget_YY26(yyyyvsp[1]));}));yyval=_tmpA6E;});
goto _LL0;}case 190U: _LL177: _LL178: {
# 2050
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2052
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2051 "parse.y"
struct Cyc_List_List*ans=0;
if(Cyc_yyget_YY45(yyyyvsp[3])!= 0)
({struct Cyc_List_List*_tmpA71=({struct Cyc_List_List*_tmp4ED=_region_malloc(yyr,sizeof(*_tmp4ED));({void*_tmpA70=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp4EC=_region_malloc(yyr,sizeof(*_tmp4EC));_tmp4EC->tag=5U,_tmp4EC->f1=(unsigned int)(yyyylsp[3]).first_line,({struct Cyc_List_List*_tmpA6F=Cyc_yyget_YY45(yyyyvsp[3]);_tmp4EC->f2=_tmpA6F;});_tmp4EC;});_tmp4ED->hd=_tmpA70;}),_tmp4ED->tl=ans;_tmp4ED;});ans=_tmpA71;});{
# 2055
struct Cyc_Absyn_PtrLoc*ptrloc=0;
struct _tuple21 _tmp4EE=*Cyc_yyget_YY1(yyyyvsp[0]);struct _tuple21 _tmp4EF=_tmp4EE;unsigned int _tmp4FB;void*_tmp4FA;void*_tmp4F9;_LL445: _tmp4FB=_tmp4EF.f1;_tmp4FA=_tmp4EF.f2;_tmp4F9=_tmp4EF.f3;_LL446:;
if(Cyc_Absyn_porting_c_code)
({struct Cyc_Absyn_PtrLoc*_tmpA72=({struct Cyc_Absyn_PtrLoc*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->ptr_loc=_tmp4FB,_tmp4F0->rgn_loc=(unsigned int)(yyyylsp[2]).first_line,_tmp4F0->zt_loc=(unsigned int)(yyyylsp[1]).first_line;_tmp4F0;});ptrloc=_tmpA72;});{
# 2060
struct _tuple14 _tmp4F1=({unsigned int _tmpA76=_tmp4FB;void*_tmpA75=_tmp4FA;void*_tmpA74=_tmp4F9;void*_tmpA73=Cyc_yyget_YY44(yyyyvsp[2]);Cyc_Parse_collapse_pointer_quals(_tmpA76,_tmpA75,_tmpA74,_tmpA73,Cyc_yyget_YY54(yyyyvsp[1]));});struct _tuple14 _tmp4F2=_tmp4F1;void*_tmp4F8;void*_tmp4F7;void*_tmp4F6;void*_tmp4F5;_LL448: _tmp4F8=_tmp4F2.f1;_tmp4F7=_tmp4F2.f2;_tmp4F6=_tmp4F2.f3;_tmp4F5=_tmp4F2.f4;_LL449:;
({struct Cyc_List_List*_tmpA79=({struct Cyc_List_List*_tmp4F4=_region_malloc(yyr,sizeof(*_tmp4F4));({void*_tmpA78=(void*)({struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*_tmp4F3=_region_malloc(yyr,sizeof(*_tmp4F3));_tmp4F3->tag=2U,(_tmp4F3->f1).rgn=_tmp4F5,(_tmp4F3->f1).nullable=_tmp4F8,(_tmp4F3->f1).bounds=_tmp4F7,(_tmp4F3->f1).zero_term=_tmp4F6,(_tmp4F3->f1).ptrloc=ptrloc,({struct Cyc_Absyn_Tqual _tmpA77=Cyc_yyget_YY23(yyyyvsp[4]);_tmp4F3->f2=_tmpA77;});_tmp4F3;});_tmp4F4->hd=_tmpA78;}),_tmp4F4->tl=ans;_tmp4F4;});ans=_tmpA79;});
({union Cyc_YYSTYPE _tmpA7A=Cyc_YY26(ans);yyval=_tmpA7A;});
# 2064
goto _LL0;};};}case 191U: _LL179: _LL17A:
# 2066
({union Cyc_YYSTYPE _tmpA7B=Cyc_YY54(0);yyval=_tmpA7B;});
goto _LL0;case 192U: _LL17B: _LL17C: {
# 2069
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2071
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2067 "parse.y"
({union Cyc_YYSTYPE _tmpA7E=Cyc_YY54(({struct Cyc_List_List*_tmp4FC=_region_malloc(yyr,sizeof(*_tmp4FC));({void*_tmpA7D=Cyc_yyget_YY53(yyyyvsp[0]);_tmp4FC->hd=_tmpA7D;}),({struct Cyc_List_List*_tmpA7C=Cyc_yyget_YY54(yyyyvsp[1]);_tmp4FC->tl=_tmpA7C;});_tmp4FC;}));yyval=_tmpA7E;});
goto _LL0;}case 193U: _LL17D: _LL17E: {
# 2070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2072 "parse.y"
({union Cyc_YYSTYPE _tmpA80=Cyc_YY53((void*)({struct Cyc_Parse_Numelts_ptrqual_Parse_Pointer_qual_struct*_tmp4FD=_region_malloc(yyr,sizeof(*_tmp4FD));_tmp4FD->tag=0U,({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp4FD->f1=_tmpA7F;});_tmp4FD;}));yyval=_tmpA80;});
goto _LL0;}case 194U: _LL17F: _LL180: {
# 2075
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2077
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2074 "parse.y"
({union Cyc_YYSTYPE _tmpA82=Cyc_YY53((void*)({struct Cyc_Parse_Region_ptrqual_Parse_Pointer_qual_struct*_tmp4FE=_region_malloc(yyr,sizeof(*_tmp4FE));_tmp4FE->tag=1U,({void*_tmpA81=Cyc_yyget_YY44(yyyyvsp[2]);_tmp4FE->f1=_tmpA81;});_tmp4FE;}));yyval=_tmpA82;});
goto _LL0;}case 195U: _LL181: _LL182: {
# 2077
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2079
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2076 "parse.y"
({union Cyc_YYSTYPE _tmpA83=Cyc_YY53((void*)({struct Cyc_Parse_Thin_ptrqual_Parse_Pointer_qual_struct*_tmp4FF=_region_malloc(yyr,sizeof(*_tmp4FF));_tmp4FF->tag=2U;_tmp4FF;}));yyval=_tmpA83;});
goto _LL0;}case 196U: _LL183: _LL184: {
# 2079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2078 "parse.y"
({union Cyc_YYSTYPE _tmpA84=Cyc_YY53((void*)({struct Cyc_Parse_Fat_ptrqual_Parse_Pointer_qual_struct*_tmp500=_region_malloc(yyr,sizeof(*_tmp500));_tmp500->tag=3U;_tmp500;}));yyval=_tmpA84;});
goto _LL0;}case 197U: _LL185: _LL186: {
# 2081
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2083
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2080 "parse.y"
({union Cyc_YYSTYPE _tmpA85=Cyc_YY53((void*)({struct Cyc_Parse_Zeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp501=_region_malloc(yyr,sizeof(*_tmp501));_tmp501->tag=4U;_tmp501;}));yyval=_tmpA85;});
goto _LL0;}case 198U: _LL187: _LL188: {
# 2083
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2085
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2082 "parse.y"
({union Cyc_YYSTYPE _tmpA86=Cyc_YY53((void*)({struct Cyc_Parse_Nozeroterm_ptrqual_Parse_Pointer_qual_struct*_tmp502=_region_malloc(yyr,sizeof(*_tmp502));_tmp502->tag=5U;_tmp502;}));yyval=_tmpA86;});
goto _LL0;}case 199U: _LL189: _LL18A: {
# 2085
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2087
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2084 "parse.y"
({union Cyc_YYSTYPE _tmpA87=Cyc_YY53((void*)({struct Cyc_Parse_Notnull_ptrqual_Parse_Pointer_qual_struct*_tmp503=_region_malloc(yyr,sizeof(*_tmp503));_tmp503->tag=6U;_tmp503;}));yyval=_tmpA87;});
goto _LL0;}case 200U: _LL18B: _LL18C: {
# 2087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2086 "parse.y"
({union Cyc_YYSTYPE _tmpA88=Cyc_YY53((void*)({struct Cyc_Parse_Nullable_ptrqual_Parse_Pointer_qual_struct*_tmp504=_region_malloc(yyr,sizeof(*_tmp504));_tmp504->tag=7U;_tmp504;}));yyval=_tmpA88;});
goto _LL0;}case 201U: _LL18D: _LL18E: {
# 2089
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2091
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2092 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpA8A=Cyc_YY1(({struct _tuple21*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505->f1=loc,_tmp505->f2=Cyc_Absyn_true_type,({void*_tmpA89=Cyc_yyget_YY2(yyyyvsp[1]);_tmp505->f3=_tmpA89;});_tmp505;}));yyval=_tmpA8A;});
# 2095
goto _LL0;}case 202U: _LL18F: _LL190: {
# 2097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2096 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpA8C=Cyc_YY1(({struct _tuple21*_tmp506=_cycalloc(sizeof(*_tmp506));_tmp506->f1=loc,_tmp506->f2=Cyc_Absyn_false_type,({void*_tmpA8B=Cyc_yyget_YY2(yyyyvsp[1]);_tmp506->f3=_tmpA8B;});_tmp506;}));yyval=_tmpA8C;});
# 2099
goto _LL0;}case 203U: _LL191: _LL192: {
# 2101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2100 "parse.y"
unsigned int loc=(unsigned int)(yyyylsp[0]).first_line;
({union Cyc_YYSTYPE _tmpA8D=Cyc_YY1(({struct _tuple21*_tmp507=_cycalloc(sizeof(*_tmp507));_tmp507->f1=loc,_tmp507->f2=Cyc_Absyn_true_type,_tmp507->f3=Cyc_Absyn_fat_bound_type;_tmp507;}));yyval=_tmpA8D;});
# 2103
goto _LL0;}case 204U: _LL193: _LL194:
# 2105
({union Cyc_YYSTYPE _tmpA8E=Cyc_YY2(Cyc_Absyn_bounds_one());yyval=_tmpA8E;});
goto _LL0;case 205U: _LL195: _LL196: {
# 2108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2106 "parse.y"
({union Cyc_YYSTYPE _tmpA8F=Cyc_YY2(Cyc_Absyn_thin_bounds_exp(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpA8F;});
goto _LL0;}case 206U: _LL197: _LL198:
# 2109
({union Cyc_YYSTYPE _tmpA90=Cyc_YY51(Cyc_Tcutil_any_bool(0));yyval=_tmpA90;});
goto _LL0;case 207U: _LL199: _LL19A: {
# 2112
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2114
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2110 "parse.y"
({union Cyc_YYSTYPE _tmpA91=Cyc_YY51(Cyc_Absyn_true_type);yyval=_tmpA91;});
goto _LL0;}case 208U: _LL19B: _LL19C: {
# 2113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2111 "parse.y"
({union Cyc_YYSTYPE _tmpA92=Cyc_YY51(Cyc_Absyn_false_type);yyval=_tmpA92;});
goto _LL0;}case 209U: _LL19D: _LL19E:
# 2114
({union Cyc_YYSTYPE _tmpA93=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpA93;});
goto _LL0;case 210U: _LL19F: _LL1A0: {
# 2117
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2119
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2117 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_trk,1);yyval=yyyyvsp[0];
goto _LL0;}case 211U: _LL1A1: _LL1A2: {
# 2120
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2122
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2118 "parse.y"
({union Cyc_YYSTYPE _tmpA94=Cyc_YY44(Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,0));yyval=_tmpA94;});
goto _LL0;}case 212U: _LL1A3: _LL1A4:
# 2121
({union Cyc_YYSTYPE _tmpA95=Cyc_YY23(Cyc_Absyn_empty_tqual((unsigned int)(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset + 1))).first_line));yyval=_tmpA95;});
goto _LL0;case 213U: _LL1A5: _LL1A6: {
# 2124
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2126
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2123 "parse.y"
({union Cyc_YYSTYPE _tmpA97=Cyc_YY23(({struct Cyc_Absyn_Tqual _tmpA96=Cyc_yyget_YY23(yyyyvsp[0]);Cyc_Absyn_combine_tqual(_tmpA96,Cyc_yyget_YY23(yyyyvsp[1]));}));yyval=_tmpA97;});
goto _LL0;}case 214U: _LL1A7: _LL1A8: {
# 2126
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2128
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2128 "parse.y"
({union Cyc_YYSTYPE _tmpA9B=Cyc_YY39(({struct _tuple26*_tmp508=_cycalloc(sizeof(*_tmp508));({struct Cyc_List_List*_tmpA9A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp508->f1=_tmpA9A;}),_tmp508->f2=0,_tmp508->f3=0,({void*_tmpA99=Cyc_yyget_YY49(yyyyvsp[1]);_tmp508->f4=_tmpA99;}),({struct Cyc_List_List*_tmpA98=Cyc_yyget_YY50(yyyyvsp[2]);_tmp508->f5=_tmpA98;});_tmp508;}));yyval=_tmpA9B;});
goto _LL0;}case 215U: _LL1A9: _LL1AA: {
# 2131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2130 "parse.y"
({union Cyc_YYSTYPE _tmpA9F=Cyc_YY39(({struct _tuple26*_tmp509=_cycalloc(sizeof(*_tmp509));({struct Cyc_List_List*_tmpA9E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp509->f1=_tmpA9E;}),_tmp509->f2=1,_tmp509->f3=0,({void*_tmpA9D=Cyc_yyget_YY49(yyyyvsp[3]);_tmp509->f4=_tmpA9D;}),({struct Cyc_List_List*_tmpA9C=Cyc_yyget_YY50(yyyyvsp[4]);_tmp509->f5=_tmpA9C;});_tmp509;}));yyval=_tmpA9F;});
goto _LL0;}case 216U: _LL1AB: _LL1AC: {
# 2133
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2135
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2133 "parse.y"
struct _tuple8*_tmp50A=Cyc_yyget_YY37(yyyyvsp[2]);struct _tuple8*_tmp50B=_tmp50A;struct _dyneither_ptr*_tmp511;struct Cyc_Absyn_Tqual _tmp510;void*_tmp50F;_LL44B: _tmp511=_tmp50B->f1;_tmp510=_tmp50B->f2;_tmp50F=_tmp50B->f3;_LL44C:;{
struct Cyc_Absyn_VarargInfo*_tmp50C=({struct Cyc_Absyn_VarargInfo*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->name=_tmp511,_tmp50E->tq=_tmp510,_tmp50E->type=_tmp50F,({int _tmpAA0=Cyc_yyget_YY31(yyyyvsp[1]);_tmp50E->inject=_tmpAA0;});_tmp50E;});
({union Cyc_YYSTYPE _tmpAA3=Cyc_YY39(({struct _tuple26*_tmp50D=_cycalloc(sizeof(*_tmp50D));_tmp50D->f1=0,_tmp50D->f2=0,_tmp50D->f3=_tmp50C,({void*_tmpAA2=Cyc_yyget_YY49(yyyyvsp[3]);_tmp50D->f4=_tmpAA2;}),({struct Cyc_List_List*_tmpAA1=Cyc_yyget_YY50(yyyyvsp[4]);_tmp50D->f5=_tmpAA1;});_tmp50D;}));yyval=_tmpAA3;});
# 2137
goto _LL0;};}case 217U: _LL1AD: _LL1AE: {
# 2139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2139 "parse.y"
struct _tuple8*_tmp512=Cyc_yyget_YY37(yyyyvsp[4]);struct _tuple8*_tmp513=_tmp512;struct _dyneither_ptr*_tmp519;struct Cyc_Absyn_Tqual _tmp518;void*_tmp517;_LL44E: _tmp519=_tmp513->f1;_tmp518=_tmp513->f2;_tmp517=_tmp513->f3;_LL44F:;{
struct Cyc_Absyn_VarargInfo*_tmp514=({struct Cyc_Absyn_VarargInfo*_tmp516=_cycalloc(sizeof(*_tmp516));_tmp516->name=_tmp519,_tmp516->tq=_tmp518,_tmp516->type=_tmp517,({int _tmpAA4=Cyc_yyget_YY31(yyyyvsp[3]);_tmp516->inject=_tmpAA4;});_tmp516;});
({union Cyc_YYSTYPE _tmpAA8=Cyc_YY39(({struct _tuple26*_tmp515=_cycalloc(sizeof(*_tmp515));({struct Cyc_List_List*_tmpAA7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY38(yyyyvsp[0]));_tmp515->f1=_tmpAA7;}),_tmp515->f2=0,_tmp515->f3=_tmp514,({void*_tmpAA6=Cyc_yyget_YY49(yyyyvsp[5]);_tmp515->f4=_tmpAA6;}),({struct Cyc_List_List*_tmpAA5=Cyc_yyget_YY50(yyyyvsp[6]);_tmp515->f5=_tmpAA5;});_tmp515;}));yyval=_tmpAA8;});
# 2143
goto _LL0;};}case 218U: _LL1AF: _LL1B0: {
# 2145
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2147
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2147 "parse.y"
({union Cyc_YYSTYPE _tmpAAA=Cyc_YY44(({struct _dyneither_ptr _tmpAA9=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpAA9,(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->tag=1U,_tmp51A->f1=0;_tmp51A;}));}));yyval=_tmpAAA;});
goto _LL0;}case 219U: _LL1B1: _LL1B2: {
# 2150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2148 "parse.y"
({union Cyc_YYSTYPE _tmpAAC=Cyc_YY44(({struct _dyneither_ptr _tmpAAB=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Parse_id2type(_tmpAAB,Cyc_Tcutil_kind_to_bound(Cyc_yyget_YY43(yyyyvsp[2])));}));yyval=_tmpAAC;});
goto _LL0;}case 220U: _LL1B3: _LL1B4:
# 2151
({union Cyc_YYSTYPE _tmpAAD=Cyc_YY49(0);yyval=_tmpAAD;});
goto _LL0;case 221U: _LL1B5: _LL1B6: {
# 2154
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2156
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2152 "parse.y"
({union Cyc_YYSTYPE _tmpAAE=Cyc_YY49(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));yyval=_tmpAAE;});
goto _LL0;}case 222U: _LL1B7: _LL1B8:
# 2155
({union Cyc_YYSTYPE _tmpAAF=Cyc_YY50(0);yyval=_tmpAAF;});
goto _LL0;case 223U: _LL1B9: _LL1BA: {
# 2158
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2160
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2157 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 224U: _LL1BB: _LL1BC: {
# 2160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2165 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp51B=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520->tag=2U,_tmp520->f1=0,_tmp520->f2=& Cyc_Tcutil_trk;_tmp520;});
struct _dyneither_ptr _tmp51C=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp51D=Cyc_Parse_id2type(_tmp51C,(void*)_tmp51B);
({union Cyc_YYSTYPE _tmpAB2=Cyc_YY50(({struct Cyc_List_List*_tmp51F=_cycalloc(sizeof(*_tmp51F));({struct _tuple29*_tmpAB1=({struct _tuple29*_tmp51E=_cycalloc(sizeof(*_tmp51E));({void*_tmpAB0=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp51E->f1=_tmpAB0;}),_tmp51E->f2=_tmp51D;_tmp51E;});_tmp51F->hd=_tmpAB1;}),_tmp51F->tl=0;_tmp51F;}));yyval=_tmpAB2;});
# 2170
goto _LL0;}case 225U: _LL1BD: _LL1BE: {
# 2172
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2174
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2172 "parse.y"
struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp521=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp526=_cycalloc(sizeof(*_tmp526));_tmp526->tag=2U,_tmp526->f1=0,_tmp526->f2=& Cyc_Tcutil_trk;_tmp526;});
struct _dyneither_ptr _tmp522=Cyc_yyget_String_tok(yyyyvsp[2]);
void*_tmp523=Cyc_Parse_id2type(_tmp522,(void*)_tmp521);
({union Cyc_YYSTYPE _tmpAB6=Cyc_YY50(({struct Cyc_List_List*_tmp525=_cycalloc(sizeof(*_tmp525));({struct _tuple29*_tmpAB5=({struct _tuple29*_tmp524=_cycalloc(sizeof(*_tmp524));({void*_tmpAB4=Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[0]));_tmp524->f1=_tmpAB4;}),_tmp524->f2=_tmp523;_tmp524;});_tmp525->hd=_tmpAB5;}),({struct Cyc_List_List*_tmpAB3=Cyc_yyget_YY50(yyyyvsp[4]);_tmp525->tl=_tmpAB3;});_tmp525;}));yyval=_tmpAB6;});
# 2177
goto _LL0;}case 226U: _LL1BF: _LL1C0:
# 2179
({union Cyc_YYSTYPE _tmpAB7=Cyc_YY31(0);yyval=_tmpAB7;});
goto _LL0;case 227U: _LL1C1: _LL1C2: {
# 2182
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2184
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2183 "parse.y"
if(({struct _dyneither_ptr _tmpAB8=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_zstrcmp(_tmpAB8,({const char*_tmp527="inject";_tag_dyneither(_tmp527,sizeof(char),7U);}));})!= 0)
({void*_tmp528=0U;({unsigned int _tmpABA=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAB9=({const char*_tmp529="missing type in function declaration";_tag_dyneither(_tmp529,sizeof(char),37U);});Cyc_Warn_err(_tmpABA,_tmpAB9,_tag_dyneither(_tmp528,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpABB=Cyc_YY31(1);yyval=_tmpABB;});
# 2187
goto _LL0;}case 228U: _LL1C3: _LL1C4: {
# 2189
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2191
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2190 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 229U: _LL1C5: _LL1C6: {
# 2193
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2195
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2191 "parse.y"
({union Cyc_YYSTYPE _tmpABD=Cyc_YY40(({struct Cyc_List_List*_tmpABC=Cyc_yyget_YY40(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpABC,Cyc_yyget_YY40(yyyyvsp[2]));}));yyval=_tmpABD;});
goto _LL0;}case 230U: _LL1C7: _LL1C8: {
# 2194
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2196
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2195 "parse.y"
({union Cyc_YYSTYPE _tmpABE=Cyc_YY40(0);yyval=_tmpABE;});
goto _LL0;}case 231U: _LL1C9: _LL1CA: {
# 2198
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2200
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2196 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 232U: _LL1CB: _LL1CC: {
# 2199
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2201
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2198 "parse.y"
({union Cyc_YYSTYPE _tmpAC0=Cyc_YY40(({struct Cyc_List_List*_tmp52A=_cycalloc(sizeof(*_tmp52A));({void*_tmpABF=Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2]));_tmp52A->hd=_tmpABF;}),_tmp52A->tl=0;_tmp52A;}));yyval=_tmpAC0;});
goto _LL0;}case 233U: _LL1CD: _LL1CE: {
# 2201
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2203
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2200 "parse.y"
Cyc_Parse_set_vartyp_kind(Cyc_yyget_YY44(yyyyvsp[0]),& Cyc_Tcutil_ek,0);
({union Cyc_YYSTYPE _tmpAC2=Cyc_YY40(({struct Cyc_List_List*_tmp52B=_cycalloc(sizeof(*_tmp52B));({void*_tmpAC1=Cyc_yyget_YY44(yyyyvsp[0]);_tmp52B->hd=_tmpAC1;}),_tmp52B->tl=0;_tmp52B;}));yyval=_tmpAC2;});
# 2203
goto _LL0;}case 234U: _LL1CF: _LL1D0: {
# 2205
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2207
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2220 "parse.y"
({union Cyc_YYSTYPE _tmpAC5=Cyc_YY40(({struct Cyc_List_List*_tmp52C=_cycalloc(sizeof(*_tmp52C));({void*_tmpAC4=Cyc_Absyn_access_eff(({struct _tuple8*_tmpAC3=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpAC3,(unsigned int)(yyyylsp[0]).first_line);}));_tmp52C->hd=_tmpAC4;}),_tmp52C->tl=0;_tmp52C;}));yyval=_tmpAC5;});
goto _LL0;}case 235U: _LL1D1: _LL1D2: {
# 2223
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2225
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2222 "parse.y"
({union Cyc_YYSTYPE _tmpAC9=Cyc_YY40(({struct Cyc_List_List*_tmp52D=_cycalloc(sizeof(*_tmp52D));({void*_tmpAC8=Cyc_Absyn_access_eff(({struct _tuple8*_tmpAC7=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpAC7,(unsigned int)(yyyylsp[0]).first_line);}));_tmp52D->hd=_tmpAC8;}),({struct Cyc_List_List*_tmpAC6=Cyc_yyget_YY40(yyyyvsp[2]);_tmp52D->tl=_tmpAC6;});_tmp52D;}));yyval=_tmpAC9;});
goto _LL0;}case 236U: _LL1D3: _LL1D4: {
# 2225
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2227
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpACB=Cyc_YY38(({struct Cyc_List_List*_tmp52E=_cycalloc(sizeof(*_tmp52E));({struct _tuple8*_tmpACA=Cyc_yyget_YY37(yyyyvsp[0]);_tmp52E->hd=_tmpACA;}),_tmp52E->tl=0;_tmp52E;}));yyval=_tmpACB;});
goto _LL0;}case 237U: _LL1D5: _LL1D6: {
# 2231
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2233
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2230 "parse.y"
({union Cyc_YYSTYPE _tmpACE=Cyc_YY38(({struct Cyc_List_List*_tmp52F=_cycalloc(sizeof(*_tmp52F));({struct _tuple8*_tmpACD=Cyc_yyget_YY37(yyyyvsp[2]);_tmp52F->hd=_tmpACD;}),({struct Cyc_List_List*_tmpACC=Cyc_yyget_YY38(yyyyvsp[0]);_tmp52F->tl=_tmpACC;});_tmp52F;}));yyval=_tmpACE;});
goto _LL0;}case 238U: _LL1D7: _LL1D8: {
# 2233
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2235
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
struct _tuple25 _tmp530=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp531=_tmp530;struct Cyc_Absyn_Tqual _tmp547;struct Cyc_Parse_Type_specifier _tmp546;struct Cyc_List_List*_tmp545;_LL451: _tmp547=_tmp531.f1;_tmp546=_tmp531.f2;_tmp545=_tmp531.f3;_LL452:;
if(_tmp547.loc == 0)_tmp547.loc=(unsigned int)(yyyylsp[0]).first_line;{
struct Cyc_Parse_Declarator _tmp532=Cyc_yyget_YY27(yyyyvsp[1]);struct Cyc_Parse_Declarator _tmp533=_tmp532;struct _tuple0*_tmp544;unsigned int _tmp543;struct Cyc_List_List*_tmp542;_LL454: _tmp544=_tmp533.id;_tmp543=_tmp533.varloc;_tmp542=_tmp533.tms;_LL455:;{
void*_tmp534=Cyc_Parse_speclist2typ(_tmp546,(unsigned int)(yyyylsp[0]).first_line);
struct _tuple13 _tmp535=Cyc_Parse_apply_tms(_tmp547,_tmp534,_tmp545,_tmp542);struct _tuple13 _tmp536=_tmp535;struct Cyc_Absyn_Tqual _tmp541;void*_tmp540;struct Cyc_List_List*_tmp53F;struct Cyc_List_List*_tmp53E;_LL457: _tmp541=_tmp536.f1;_tmp540=_tmp536.f2;_tmp53F=_tmp536.f3;_tmp53E=_tmp536.f4;_LL458:;
if(_tmp53F != 0)
({void*_tmp537=0U;({unsigned int _tmpAD0=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpACF=({const char*_tmp538="parameter with bad type params";_tag_dyneither(_tmp538,sizeof(char),31U);});Cyc_Warn_err(_tmpAD0,_tmpACF,_tag_dyneither(_tmp537,sizeof(void*),0U));});});
if(Cyc_Absyn_is_qvar_qualified(_tmp544))
({void*_tmp539=0U;({unsigned int _tmpAD2=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAD1=({const char*_tmp53A="parameter cannot be qualified with a namespace";_tag_dyneither(_tmp53A,sizeof(char),47U);});Cyc_Warn_err(_tmpAD2,_tmpAD1,_tag_dyneither(_tmp539,sizeof(void*),0U));});});{
struct _dyneither_ptr*idopt=(*_tmp544).f2;
if(_tmp53E != 0)
({void*_tmp53B=0U;({unsigned int _tmpAD4=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAD3=({const char*_tmp53C="extra attributes on parameter, ignoring";_tag_dyneither(_tmp53C,sizeof(char),40U);});Cyc_Warn_warn(_tmpAD4,_tmpAD3,_tag_dyneither(_tmp53B,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpAD5=Cyc_YY37(({struct _tuple8*_tmp53D=_cycalloc(sizeof(*_tmp53D));_tmp53D->f1=idopt,_tmp53D->f2=_tmp541,_tmp53D->f3=_tmp540;_tmp53D;}));yyval=_tmpAD5;});
# 2250
goto _LL0;};};};}case 239U: _LL1D9: _LL1DA: {
# 2252
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2254
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2251 "parse.y"
struct _tuple25 _tmp548=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp549=_tmp548;struct Cyc_Absyn_Tqual _tmp550;struct Cyc_Parse_Type_specifier _tmp54F;struct Cyc_List_List*_tmp54E;_LL45A: _tmp550=_tmp549.f1;_tmp54F=_tmp549.f2;_tmp54E=_tmp549.f3;_LL45B:;
if(_tmp550.loc == 0)_tmp550.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp54A=Cyc_Parse_speclist2typ(_tmp54F,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp54E != 0)
({void*_tmp54B=0U;({unsigned int _tmpAD7=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAD6=({const char*_tmp54C="bad attributes on parameter, ignoring";_tag_dyneither(_tmp54C,sizeof(char),38U);});Cyc_Warn_warn(_tmpAD7,_tmpAD6,_tag_dyneither(_tmp54B,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpAD8=Cyc_YY37(({struct _tuple8*_tmp54D=_cycalloc(sizeof(*_tmp54D));_tmp54D->f1=0,_tmp54D->f2=_tmp550,_tmp54D->f3=_tmp54A;_tmp54D;}));yyval=_tmpAD8;});
# 2258
goto _LL0;};}case 240U: _LL1DB: _LL1DC: {
# 2260
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2262
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2259 "parse.y"
struct _tuple25 _tmp551=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp552=_tmp551;struct Cyc_Absyn_Tqual _tmp562;struct Cyc_Parse_Type_specifier _tmp561;struct Cyc_List_List*_tmp560;_LL45D: _tmp562=_tmp552.f1;_tmp561=_tmp552.f2;_tmp560=_tmp552.f3;_LL45E:;
if(_tmp562.loc == 0)_tmp562.loc=(unsigned int)(yyyylsp[0]).first_line;{
void*_tmp553=Cyc_Parse_speclist2typ(_tmp561,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp554=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp555=Cyc_Parse_apply_tms(_tmp562,_tmp553,_tmp560,_tmp554);struct _tuple13 _tmp556=_tmp555;struct Cyc_Absyn_Tqual _tmp55F;void*_tmp55E;struct Cyc_List_List*_tmp55D;struct Cyc_List_List*_tmp55C;_LL460: _tmp55F=_tmp556.f1;_tmp55E=_tmp556.f2;_tmp55D=_tmp556.f3;_tmp55C=_tmp556.f4;_LL461:;
if(_tmp55D != 0)
({void*_tmp557=0U;({unsigned int _tmpADA=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpAD9=({const char*_tmp558="bad type parameters on formal argument, ignoring";_tag_dyneither(_tmp558,sizeof(char),49U);});Cyc_Warn_warn(_tmpADA,_tmpAD9,_tag_dyneither(_tmp557,sizeof(void*),0U));});});
# 2267
if(_tmp55C != 0)
({void*_tmp559=0U;({unsigned int _tmpADC=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpADB=({const char*_tmp55A="bad attributes on parameter, ignoring";_tag_dyneither(_tmp55A,sizeof(char),38U);});Cyc_Warn_warn(_tmpADC,_tmpADB,_tag_dyneither(_tmp559,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpADD=Cyc_YY37(({struct _tuple8*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B->f1=0,_tmp55B->f2=_tmp55F,_tmp55B->f3=_tmp55E;_tmp55B;}));yyval=_tmpADD;});
# 2271
goto _LL0;};}case 241U: _LL1DD: _LL1DE: {
# 2273
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2275
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2275 "parse.y"
({union Cyc_YYSTYPE _tmpADE=Cyc_YY36(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY36(yyyyvsp[0])));yyval=_tmpADE;});
goto _LL0;}case 242U: _LL1DF: _LL1E0: {
# 2278
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2280
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2279 "parse.y"
({union Cyc_YYSTYPE _tmpAE1=Cyc_YY36(({struct Cyc_List_List*_tmp564=_cycalloc(sizeof(*_tmp564));({struct _dyneither_ptr*_tmpAE0=({struct _dyneither_ptr*_tmp563=_cycalloc(sizeof(*_tmp563));({struct _dyneither_ptr _tmpADF=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp563=_tmpADF;});_tmp563;});_tmp564->hd=_tmpAE0;}),_tmp564->tl=0;_tmp564;}));yyval=_tmpAE1;});
goto _LL0;}case 243U: _LL1E1: _LL1E2: {
# 2282
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2284
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2281 "parse.y"
({union Cyc_YYSTYPE _tmpAE5=Cyc_YY36(({struct Cyc_List_List*_tmp566=_cycalloc(sizeof(*_tmp566));({struct _dyneither_ptr*_tmpAE4=({struct _dyneither_ptr*_tmp565=_cycalloc(sizeof(*_tmp565));({struct _dyneither_ptr _tmpAE3=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp565=_tmpAE3;});_tmp565;});_tmp566->hd=_tmpAE4;}),({struct Cyc_List_List*_tmpAE2=Cyc_yyget_YY36(yyyyvsp[0]);_tmp566->tl=_tmpAE2;});_tmp566;}));yyval=_tmpAE5;});
goto _LL0;}case 244U: _LL1E3: _LL1E4: {
# 2284
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2286
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2285 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 245U: _LL1E5: _LL1E6: {
# 2288
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2290
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2286 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 246U: _LL1E7: _LL1E8: {
# 2289
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2291
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2291 "parse.y"
({union Cyc_YYSTYPE _tmpAE7=Cyc_Exp_tok(({void*_tmpAE6=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp567=_cycalloc(sizeof(*_tmp567));_tmp567->tag=36U,_tmp567->f1=0,_tmp567->f2=0;_tmp567;});Cyc_Absyn_new_exp(_tmpAE6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAE7;});
goto _LL0;}case 247U: _LL1E9: _LL1EA: {
# 2294
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2296
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2293 "parse.y"
({union Cyc_YYSTYPE _tmpAEA=Cyc_Exp_tok(({void*_tmpAE9=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=36U,_tmp568->f1=0,({struct Cyc_List_List*_tmpAE8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp568->f2=_tmpAE8;});_tmp568;});Cyc_Absyn_new_exp(_tmpAE9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAEA;});
goto _LL0;}case 248U: _LL1EB: _LL1EC: {
# 2296
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2298
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2295 "parse.y"
({union Cyc_YYSTYPE _tmpAED=Cyc_Exp_tok(({void*_tmpAEC=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->tag=36U,_tmp569->f1=0,({struct Cyc_List_List*_tmpAEB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[1]));_tmp569->f2=_tmpAEB;});_tmp569;});Cyc_Absyn_new_exp(_tmpAEC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAED;});
goto _LL0;}case 249U: _LL1ED: _LL1EE: {
# 2298
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2300
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2297 "parse.y"
struct Cyc_Absyn_Vardecl*_tmp56A=({unsigned int _tmpAF2=(unsigned int)(yyyylsp[2]).first_line;struct _tuple0*_tmpAF1=({struct _tuple0*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpAEF=({struct _dyneither_ptr*_tmp56C=_cycalloc(sizeof(*_tmp56C));({struct _dyneither_ptr _tmpAEE=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp56C=_tmpAEE;});_tmp56C;});_tmp56D->f2=_tmpAEF;});_tmp56D;});void*_tmpAF0=Cyc_Absyn_uint_type;Cyc_Absyn_new_vardecl(_tmpAF2,_tmpAF1,_tmpAF0,
Cyc_Absyn_uint_exp(0U,(unsigned int)(yyyylsp[2]).first_line));});
# 2300
(_tmp56A->tq).real_const=1;
({union Cyc_YYSTYPE _tmpAF6=Cyc_Exp_tok(({void*_tmpAF5=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp56B=_cycalloc(sizeof(*_tmp56B));_tmp56B->tag=27U,_tmp56B->f1=_tmp56A,({struct Cyc_Absyn_Exp*_tmpAF4=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp56B->f2=_tmpAF4;}),({struct Cyc_Absyn_Exp*_tmpAF3=Cyc_yyget_Exp_tok(yyyyvsp[6]);_tmp56B->f3=_tmpAF3;}),_tmp56B->f4=0;_tmp56B;});Cyc_Absyn_new_exp(_tmpAF5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAF6;});
# 2303
goto _LL0;}case 250U: _LL1EF: _LL1F0: {
# 2305
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2307
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2305 "parse.y"
void*_tmp56E=({struct _tuple8*_tmpAF7=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpAF7,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpAFA=Cyc_Exp_tok(({void*_tmpAF9=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp56F=_cycalloc(sizeof(*_tmp56F));_tmp56F->tag=28U,({struct Cyc_Absyn_Exp*_tmpAF8=Cyc_yyget_Exp_tok(yyyyvsp[4]);_tmp56F->f1=_tmpAF8;}),_tmp56F->f2=_tmp56E,_tmp56F->f3=0;_tmp56F;});Cyc_Absyn_new_exp(_tmpAF9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpAFA;});
# 2308
goto _LL0;}case 251U: _LL1F1: _LL1F2: {
# 2310
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2312
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpAFD=Cyc_YY5(({struct Cyc_List_List*_tmp571=_cycalloc(sizeof(*_tmp571));({struct _tuple30*_tmpAFC=({struct _tuple30*_tmp570=_cycalloc(sizeof(*_tmp570));_tmp570->f1=0,({struct Cyc_Absyn_Exp*_tmpAFB=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp570->f2=_tmpAFB;});_tmp570;});_tmp571->hd=_tmpAFC;}),_tmp571->tl=0;_tmp571;}));yyval=_tmpAFD;});
goto _LL0;}case 252U: _LL1F3: _LL1F4: {
# 2316
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2318
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2315 "parse.y"
({union Cyc_YYSTYPE _tmpB01=Cyc_YY5(({struct Cyc_List_List*_tmp573=_cycalloc(sizeof(*_tmp573));({struct _tuple30*_tmpB00=({struct _tuple30*_tmp572=_cycalloc(sizeof(*_tmp572));({struct Cyc_List_List*_tmpAFF=Cyc_yyget_YY41(yyyyvsp[0]);_tmp572->f1=_tmpAFF;}),({struct Cyc_Absyn_Exp*_tmpAFE=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp572->f2=_tmpAFE;});_tmp572;});_tmp573->hd=_tmpB00;}),_tmp573->tl=0;_tmp573;}));yyval=_tmpB01;});
goto _LL0;}case 253U: _LL1F5: _LL1F6: {
# 2318
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2320
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2317 "parse.y"
({union Cyc_YYSTYPE _tmpB05=Cyc_YY5(({struct Cyc_List_List*_tmp575=_cycalloc(sizeof(*_tmp575));({struct _tuple30*_tmpB04=({struct _tuple30*_tmp574=_cycalloc(sizeof(*_tmp574));_tmp574->f1=0,({struct Cyc_Absyn_Exp*_tmpB03=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp574->f2=_tmpB03;});_tmp574;});_tmp575->hd=_tmpB04;}),({struct Cyc_List_List*_tmpB02=Cyc_yyget_YY5(yyyyvsp[0]);_tmp575->tl=_tmpB02;});_tmp575;}));yyval=_tmpB05;});
goto _LL0;}case 254U: _LL1F7: _LL1F8: {
# 2320
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2322
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2319 "parse.y"
({union Cyc_YYSTYPE _tmpB0A=Cyc_YY5(({struct Cyc_List_List*_tmp577=_cycalloc(sizeof(*_tmp577));({struct _tuple30*_tmpB09=({struct _tuple30*_tmp576=_cycalloc(sizeof(*_tmp576));({struct Cyc_List_List*_tmpB08=Cyc_yyget_YY41(yyyyvsp[2]);_tmp576->f1=_tmpB08;}),({struct Cyc_Absyn_Exp*_tmpB07=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp576->f2=_tmpB07;});_tmp576;});_tmp577->hd=_tmpB09;}),({struct Cyc_List_List*_tmpB06=Cyc_yyget_YY5(yyyyvsp[0]);_tmp577->tl=_tmpB06;});_tmp577;}));yyval=_tmpB0A;});
goto _LL0;}case 255U: _LL1F9: _LL1FA: {
# 2322
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2324
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2323 "parse.y"
({union Cyc_YYSTYPE _tmpB0B=Cyc_YY41(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY41(yyyyvsp[0])));yyval=_tmpB0B;});
goto _LL0;}case 256U: _LL1FB: _LL1FC: {
# 2326
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2328
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2324 "parse.y"
({union Cyc_YYSTYPE _tmpB0F=Cyc_YY41(({struct Cyc_List_List*_tmp57A=_cycalloc(sizeof(*_tmp57A));({void*_tmpB0E=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp579=_cycalloc(sizeof(*_tmp579));_tmp579->tag=1U,({struct _dyneither_ptr*_tmpB0D=({struct _dyneither_ptr*_tmp578=_cycalloc(sizeof(*_tmp578));({struct _dyneither_ptr _tmpB0C=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp578=_tmpB0C;});_tmp578;});_tmp579->f1=_tmpB0D;});_tmp579;});_tmp57A->hd=_tmpB0E;}),_tmp57A->tl=0;_tmp57A;}));yyval=_tmpB0F;});
goto _LL0;}case 257U: _LL1FD: _LL1FE: {
# 2327
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2329
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2329 "parse.y"
({union Cyc_YYSTYPE _tmpB11=Cyc_YY41(({struct Cyc_List_List*_tmp57B=_cycalloc(sizeof(*_tmp57B));({void*_tmpB10=Cyc_yyget_YY42(yyyyvsp[0]);_tmp57B->hd=_tmpB10;}),_tmp57B->tl=0;_tmp57B;}));yyval=_tmpB11;});
goto _LL0;}case 258U: _LL1FF: _LL200: {
# 2332
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2334
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2330 "parse.y"
({union Cyc_YYSTYPE _tmpB14=Cyc_YY41(({struct Cyc_List_List*_tmp57C=_cycalloc(sizeof(*_tmp57C));({void*_tmpB13=Cyc_yyget_YY42(yyyyvsp[1]);_tmp57C->hd=_tmpB13;}),({struct Cyc_List_List*_tmpB12=Cyc_yyget_YY41(yyyyvsp[0]);_tmp57C->tl=_tmpB12;});_tmp57C;}));yyval=_tmpB14;});
goto _LL0;}case 259U: _LL201: _LL202: {
# 2333
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2335
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2334 "parse.y"
({union Cyc_YYSTYPE _tmpB16=Cyc_YY42((void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmp57D=_cycalloc(sizeof(*_tmp57D));_tmp57D->tag=0U,({struct Cyc_Absyn_Exp*_tmpB15=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp57D->f1=_tmpB15;});_tmp57D;}));yyval=_tmpB16;});
goto _LL0;}case 260U: _LL203: _LL204: {
# 2337
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2339
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2335 "parse.y"
({union Cyc_YYSTYPE _tmpB19=Cyc_YY42((void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp57F=_cycalloc(sizeof(*_tmp57F));_tmp57F->tag=1U,({struct _dyneither_ptr*_tmpB18=({struct _dyneither_ptr*_tmp57E=_cycalloc(sizeof(*_tmp57E));({struct _dyneither_ptr _tmpB17=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp57E=_tmpB17;});_tmp57E;});_tmp57F->f1=_tmpB18;});_tmp57F;}));yyval=_tmpB19;});
goto _LL0;}case 261U: _LL205: _LL206: {
# 2338
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2340
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2340 "parse.y"
struct _tuple25 _tmp580=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp581=_tmp580;struct Cyc_Absyn_Tqual _tmp588;struct Cyc_Parse_Type_specifier _tmp587;struct Cyc_List_List*_tmp586;_LL463: _tmp588=_tmp581.f1;_tmp587=_tmp581.f2;_tmp586=_tmp581.f3;_LL464:;{
void*_tmp582=Cyc_Parse_speclist2typ(_tmp587,(unsigned int)(yyyylsp[0]).first_line);
if(_tmp586 != 0)
({void*_tmp583=0U;({unsigned int _tmpB1B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpB1A=({const char*_tmp584="ignoring attributes in type";_tag_dyneither(_tmp584,sizeof(char),28U);});Cyc_Warn_warn(_tmpB1B,_tmpB1A,_tag_dyneither(_tmp583,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB1C=Cyc_YY37(({struct _tuple8*_tmp585=_cycalloc(sizeof(*_tmp585));_tmp585->f1=0,_tmp585->f2=_tmp588,_tmp585->f3=_tmp582;_tmp585;}));yyval=_tmpB1C;});
# 2346
goto _LL0;};}case 262U: _LL207: _LL208: {
# 2348
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2350
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2347 "parse.y"
struct _tuple25 _tmp589=Cyc_yyget_YY35(yyyyvsp[0]);struct _tuple25 _tmp58A=_tmp589;struct Cyc_Absyn_Tqual _tmp595;struct Cyc_Parse_Type_specifier _tmp594;struct Cyc_List_List*_tmp593;_LL466: _tmp595=_tmp58A.f1;_tmp594=_tmp58A.f2;_tmp593=_tmp58A.f3;_LL467:;{
void*_tmp58B=Cyc_Parse_speclist2typ(_tmp594,(unsigned int)(yyyylsp[0]).first_line);
struct Cyc_List_List*_tmp58C=(Cyc_yyget_YY30(yyyyvsp[1])).tms;
struct _tuple13 _tmp58D=Cyc_Parse_apply_tms(_tmp595,_tmp58B,_tmp593,_tmp58C);
if(_tmp58D.f3 != 0)
# 2353
({void*_tmp58E=0U;({unsigned int _tmpB1E=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB1D=({const char*_tmp58F="bad type params, ignoring";_tag_dyneither(_tmp58F,sizeof(char),26U);});Cyc_Warn_warn(_tmpB1E,_tmpB1D,_tag_dyneither(_tmp58E,sizeof(void*),0U));});});
if(_tmp58D.f4 != 0)
({void*_tmp590=0U;({unsigned int _tmpB20=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpB1F=({const char*_tmp591="bad specifiers, ignoring";_tag_dyneither(_tmp591,sizeof(char),25U);});Cyc_Warn_warn(_tmpB20,_tmpB1F,_tag_dyneither(_tmp590,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpB21=Cyc_YY37(({struct _tuple8*_tmp592=_cycalloc(sizeof(*_tmp592));_tmp592->f1=0,_tmp592->f2=_tmp58D.f1,_tmp592->f3=_tmp58D.f2;_tmp592;}));yyval=_tmpB21;});
# 2358
goto _LL0;};}case 263U: _LL209: _LL20A: {
# 2360
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2362
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2361 "parse.y"
({union Cyc_YYSTYPE _tmpB23=Cyc_YY44(({struct _tuple8*_tmpB22=Cyc_yyget_YY37(yyyyvsp[0]);Cyc_Parse_type_name_to_type(_tmpB22,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB23;});
goto _LL0;}case 264U: _LL20B: _LL20C: {
# 2364
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2366
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2362 "parse.y"
({union Cyc_YYSTYPE _tmpB24=Cyc_YY44(Cyc_Absyn_join_eff(0));yyval=_tmpB24;});
goto _LL0;}case 265U: _LL20D: _LL20E: {
# 2365
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2367
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2363 "parse.y"
({union Cyc_YYSTYPE _tmpB25=Cyc_YY44(Cyc_Absyn_join_eff(Cyc_yyget_YY40(yyyyvsp[1])));yyval=_tmpB25;});
goto _LL0;}case 266U: _LL20F: _LL210: {
# 2366
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2368
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2364 "parse.y"
({union Cyc_YYSTYPE _tmpB26=Cyc_YY44(Cyc_Absyn_regionsof_eff(Cyc_yyget_YY44(yyyyvsp[2])));yyval=_tmpB26;});
goto _LL0;}case 267U: _LL211: _LL212: {
# 2367
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2369
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2365 "parse.y"
({union Cyc_YYSTYPE _tmpB29=Cyc_YY44(Cyc_Absyn_join_eff(({struct Cyc_List_List*_tmp596=_cycalloc(sizeof(*_tmp596));({void*_tmpB28=Cyc_yyget_YY44(yyyyvsp[0]);_tmp596->hd=_tmpB28;}),({struct Cyc_List_List*_tmpB27=Cyc_yyget_YY40(yyyyvsp[2]);_tmp596->tl=_tmpB27;});_tmp596;})));yyval=_tmpB29;});
goto _LL0;}case 268U: _LL213: _LL214: {
# 2368
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2370
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpB2B=Cyc_YY40(({struct Cyc_List_List*_tmp597=_cycalloc(sizeof(*_tmp597));({void*_tmpB2A=Cyc_yyget_YY44(yyyyvsp[0]);_tmp597->hd=_tmpB2A;}),_tmp597->tl=0;_tmp597;}));yyval=_tmpB2B;});
goto _LL0;}case 269U: _LL215: _LL216: {
# 2374
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2376
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2372 "parse.y"
({union Cyc_YYSTYPE _tmpB2E=Cyc_YY40(({struct Cyc_List_List*_tmp598=_cycalloc(sizeof(*_tmp598));({void*_tmpB2D=Cyc_yyget_YY44(yyyyvsp[2]);_tmp598->hd=_tmpB2D;}),({struct Cyc_List_List*_tmpB2C=Cyc_yyget_YY40(yyyyvsp[0]);_tmp598->tl=_tmpB2C;});_tmp598;}));yyval=_tmpB2E;});
goto _LL0;}case 270U: _LL217: _LL218: {
# 2375
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2377
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2377 "parse.y"
({union Cyc_YYSTYPE _tmpB30=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72E;({struct Cyc_List_List*_tmpB2F=Cyc_yyget_YY26(yyyyvsp[0]);_tmp72E.tms=_tmpB2F;});_tmp72E;}));yyval=_tmpB30;});
goto _LL0;}case 271U: _LL219: _LL21A: {
# 2380
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2382
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2379 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 272U: _LL21B: _LL21C: {
# 2382
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2384
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2381 "parse.y"
({union Cyc_YYSTYPE _tmpB33=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp72F;({struct Cyc_List_List*_tmpB32=({struct Cyc_List_List*_tmpB31=Cyc_yyget_YY26(yyyyvsp[0]);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpB31,(Cyc_yyget_YY30(yyyyvsp[1])).tms);});_tmp72F.tms=_tmpB32;});_tmp72F;}));yyval=_tmpB33;});
goto _LL0;}case 273U: _LL21D: _LL21E: {
# 2384
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2386
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2386 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 274U: _LL21F: _LL220: {
# 2389
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2391
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2388 "parse.y"
({union Cyc_YYSTYPE _tmpB37=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp730;({struct Cyc_List_List*_tmpB36=({struct Cyc_List_List*_tmp59A=_region_malloc(yyr,sizeof(*_tmp59A));({void*_tmpB35=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp599=_region_malloc(yyr,sizeof(*_tmp599));_tmp599->tag=0U,({void*_tmpB34=Cyc_yyget_YY51(yyyyvsp[2]);_tmp599->f1=_tmpB34;}),_tmp599->f2=(unsigned int)(yyyylsp[2]).first_line;_tmp599;});_tmp59A->hd=_tmpB35;}),_tmp59A->tl=0;_tmp59A;});_tmp730.tms=_tmpB36;});_tmp730;}));yyval=_tmpB37;});
goto _LL0;}case 275U: _LL221: _LL222: {
# 2391
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2393
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2390 "parse.y"
({union Cyc_YYSTYPE _tmpB3C=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp731;({struct Cyc_List_List*_tmpB3B=({struct Cyc_List_List*_tmp59C=_region_malloc(yyr,sizeof(*_tmp59C));({void*_tmpB3A=(void*)({struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*_tmp59B=_region_malloc(yyr,sizeof(*_tmp59B));_tmp59B->tag=0U,({void*_tmpB39=Cyc_yyget_YY51(yyyyvsp[3]);_tmp59B->f1=_tmpB39;}),_tmp59B->f2=(unsigned int)(yyyylsp[3]).first_line;_tmp59B;});_tmp59C->hd=_tmpB3A;}),({struct Cyc_List_List*_tmpB38=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp59C->tl=_tmpB38;});_tmp59C;});_tmp731.tms=_tmpB3B;});_tmp731;}));yyval=_tmpB3C;});
goto _LL0;}case 276U: _LL223: _LL224: {
# 2393
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2395
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2392 "parse.y"
({union Cyc_YYSTYPE _tmpB41=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp732;({struct Cyc_List_List*_tmpB40=({struct Cyc_List_List*_tmp59E=_region_malloc(yyr,sizeof(*_tmp59E));({void*_tmpB3F=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp59D=_region_malloc(yyr,sizeof(*_tmp59D));_tmp59D->tag=1U,({struct Cyc_Absyn_Exp*_tmpB3E=Cyc_yyget_Exp_tok(yyyyvsp[1]);_tmp59D->f1=_tmpB3E;}),({void*_tmpB3D=Cyc_yyget_YY51(yyyyvsp[3]);_tmp59D->f2=_tmpB3D;}),_tmp59D->f3=(unsigned int)(yyyylsp[3]).first_line;_tmp59D;});_tmp59E->hd=_tmpB3F;}),_tmp59E->tl=0;_tmp59E;});_tmp732.tms=_tmpB40;});_tmp732;}));yyval=_tmpB41;});
goto _LL0;}case 277U: _LL225: _LL226: {
# 2395
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2397
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2394 "parse.y"
({union Cyc_YYSTYPE _tmpB47=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp733;({struct Cyc_List_List*_tmpB46=({struct Cyc_List_List*_tmp5A0=_region_malloc(yyr,sizeof(*_tmp5A0));({void*_tmpB45=(void*)({struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*_tmp59F=_region_malloc(yyr,sizeof(*_tmp59F));_tmp59F->tag=1U,({struct Cyc_Absyn_Exp*_tmpB44=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp59F->f1=_tmpB44;}),({void*_tmpB43=Cyc_yyget_YY51(yyyyvsp[4]);_tmp59F->f2=_tmpB43;}),_tmp59F->f3=(unsigned int)(yyyylsp[4]).first_line;_tmp59F;});_tmp5A0->hd=_tmpB45;}),({
struct Cyc_List_List*_tmpB42=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5A0->tl=_tmpB42;});_tmp5A0;});
# 2394
_tmp733.tms=_tmpB46;});_tmp733;}));yyval=_tmpB47;});
# 2397
goto _LL0;}case 278U: _LL227: _LL228: {
# 2399
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2401
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2398 "parse.y"
({union Cyc_YYSTYPE _tmpB4F=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp734;({struct Cyc_List_List*_tmpB4E=({struct Cyc_List_List*_tmp5A3=_region_malloc(yyr,sizeof(*_tmp5A3));({void*_tmpB4D=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5A2=_region_malloc(yyr,sizeof(*_tmp5A2));_tmp5A2->tag=3U,({void*_tmpB4C=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A1=_region_malloc(yyr,sizeof(*_tmp5A1));_tmp5A1->tag=1U,_tmp5A1->f1=0,_tmp5A1->f2=0,_tmp5A1->f3=0,({void*_tmpB4B=Cyc_yyget_YY49(yyyyvsp[1]);_tmp5A1->f4=_tmpB4B;}),({struct Cyc_List_List*_tmpB4A=Cyc_yyget_YY50(yyyyvsp[2]);_tmp5A1->f5=_tmpB4A;}),({struct Cyc_Absyn_Exp*_tmpB49=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5A1->f6=_tmpB49;}),({struct Cyc_Absyn_Exp*_tmpB48=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5A1->f7=_tmpB48;});_tmp5A1;});_tmp5A2->f1=_tmpB4C;});_tmp5A2;});_tmp5A3->hd=_tmpB4D;}),_tmp5A3->tl=0;_tmp5A3;});_tmp734.tms=_tmpB4E;});_tmp734;}));yyval=_tmpB4F;});
# 2400
goto _LL0;}case 279U: _LL229: _LL22A: {
# 2402
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2404
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2401 "parse.y"
struct _tuple26*_tmp5A4=Cyc_yyget_YY39(yyyyvsp[1]);struct _tuple26*_tmp5A5=_tmp5A4;struct Cyc_List_List*_tmp5AD;int _tmp5AC;struct Cyc_Absyn_VarargInfo*_tmp5AB;void*_tmp5AA;struct Cyc_List_List*_tmp5A9;_LL469: _tmp5AD=_tmp5A5->f1;_tmp5AC=_tmp5A5->f2;_tmp5AB=_tmp5A5->f3;_tmp5AA=_tmp5A5->f4;_tmp5A9=_tmp5A5->f5;_LL46A:;
({union Cyc_YYSTYPE _tmpB55=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp735;({struct Cyc_List_List*_tmpB54=({struct Cyc_List_List*_tmp5A8=_region_malloc(yyr,sizeof(*_tmp5A8));({void*_tmpB53=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5A7=_region_malloc(yyr,sizeof(*_tmp5A7));_tmp5A7->tag=3U,({void*_tmpB52=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5A6=_region_malloc(yyr,sizeof(*_tmp5A6));_tmp5A6->tag=1U,_tmp5A6->f1=_tmp5AD,_tmp5A6->f2=_tmp5AC,_tmp5A6->f3=_tmp5AB,_tmp5A6->f4=_tmp5AA,_tmp5A6->f5=_tmp5A9,({struct Cyc_Absyn_Exp*_tmpB51=Cyc_yyget_YY55(yyyyvsp[3]);_tmp5A6->f6=_tmpB51;}),({struct Cyc_Absyn_Exp*_tmpB50=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5A6->f7=_tmpB50;});_tmp5A6;});_tmp5A7->f1=_tmpB52;});_tmp5A7;});_tmp5A8->hd=_tmpB53;}),_tmp5A8->tl=0;_tmp5A8;});_tmp735.tms=_tmpB54;});_tmp735;}));yyval=_tmpB55;});
# 2404
goto _LL0;}case 280U: _LL22B: _LL22C: {
# 2406
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2408
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2405 "parse.y"
({union Cyc_YYSTYPE _tmpB5E=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp736;({struct Cyc_List_List*_tmpB5D=({struct Cyc_List_List*_tmp5B0=_region_malloc(yyr,sizeof(*_tmp5B0));({void*_tmpB5C=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5AF=_region_malloc(yyr,sizeof(*_tmp5AF));_tmp5AF->tag=3U,({void*_tmpB5B=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5AE=_region_malloc(yyr,sizeof(*_tmp5AE));_tmp5AE->tag=1U,_tmp5AE->f1=0,_tmp5AE->f2=0,_tmp5AE->f3=0,({void*_tmpB5A=Cyc_yyget_YY49(yyyyvsp[2]);_tmp5AE->f4=_tmpB5A;}),({struct Cyc_List_List*_tmpB59=Cyc_yyget_YY50(yyyyvsp[3]);_tmp5AE->f5=_tmpB59;}),({struct Cyc_Absyn_Exp*_tmpB58=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5AE->f6=_tmpB58;}),({struct Cyc_Absyn_Exp*_tmpB57=Cyc_yyget_YY55(yyyyvsp[6]);_tmp5AE->f7=_tmpB57;});_tmp5AE;});_tmp5AF->f1=_tmpB5B;});_tmp5AF;});_tmp5B0->hd=_tmpB5C;}),({
struct Cyc_List_List*_tmpB56=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5B0->tl=_tmpB56;});_tmp5B0;});
# 2405
_tmp736.tms=_tmpB5D;});_tmp736;}));yyval=_tmpB5E;});
# 2408
goto _LL0;}case 281U: _LL22D: _LL22E: {
# 2410
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2412
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2409 "parse.y"
struct _tuple26*_tmp5B1=Cyc_yyget_YY39(yyyyvsp[2]);struct _tuple26*_tmp5B2=_tmp5B1;struct Cyc_List_List*_tmp5BA;int _tmp5B9;struct Cyc_Absyn_VarargInfo*_tmp5B8;void*_tmp5B7;struct Cyc_List_List*_tmp5B6;_LL46C: _tmp5BA=_tmp5B2->f1;_tmp5B9=_tmp5B2->f2;_tmp5B8=_tmp5B2->f3;_tmp5B7=_tmp5B2->f4;_tmp5B6=_tmp5B2->f5;_LL46D:;
({union Cyc_YYSTYPE _tmpB65=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp737;({struct Cyc_List_List*_tmpB64=({struct Cyc_List_List*_tmp5B5=_region_malloc(yyr,sizeof(*_tmp5B5));({void*_tmpB63=(void*)({struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*_tmp5B4=_region_malloc(yyr,sizeof(*_tmp5B4));_tmp5B4->tag=3U,({void*_tmpB62=(void*)({struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*_tmp5B3=_region_malloc(yyr,sizeof(*_tmp5B3));_tmp5B3->tag=1U,_tmp5B3->f1=_tmp5BA,_tmp5B3->f2=_tmp5B9,_tmp5B3->f3=_tmp5B8,_tmp5B3->f4=_tmp5B7,_tmp5B3->f5=_tmp5B6,({
struct Cyc_Absyn_Exp*_tmpB61=Cyc_yyget_YY55(yyyyvsp[4]);_tmp5B3->f6=_tmpB61;}),({struct Cyc_Absyn_Exp*_tmpB60=Cyc_yyget_YY55(yyyyvsp[5]);_tmp5B3->f7=_tmpB60;});_tmp5B3;});
# 2410
_tmp5B4->f1=_tmpB62;});_tmp5B4;});_tmp5B5->hd=_tmpB63;}),({
struct Cyc_List_List*_tmpB5F=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5B5->tl=_tmpB5F;});_tmp5B5;});
# 2410
_tmp737.tms=_tmpB64;});_tmp737;}));yyval=_tmpB65;});
# 2413
goto _LL0;}case 282U: _LL22F: _LL230: {
# 2415
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2417
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2415 "parse.y"
struct Cyc_List_List*_tmp5BB=({unsigned int _tmpB66=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpB66,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[2])));});
({union Cyc_YYSTYPE _tmpB6A=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp738;({struct Cyc_List_List*_tmpB69=({struct Cyc_List_List*_tmp5BD=_region_malloc(yyr,sizeof(*_tmp5BD));({void*_tmpB68=(void*)({struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*_tmp5BC=_region_malloc(yyr,sizeof(*_tmp5BC));_tmp5BC->tag=4U,_tmp5BC->f1=_tmp5BB,_tmp5BC->f2=(unsigned int)(yyyylsp[1]).first_line,_tmp5BC->f3=0;_tmp5BC;});_tmp5BD->hd=_tmpB68;}),({
struct Cyc_List_List*_tmpB67=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5BD->tl=_tmpB67;});_tmp5BD;});
# 2416
_tmp738.tms=_tmpB69;});_tmp738;}));yyval=_tmpB6A;});
# 2419
goto _LL0;}case 283U: _LL231: _LL232: {
# 2421
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2423
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2420 "parse.y"
({union Cyc_YYSTYPE _tmpB6F=Cyc_YY30(({struct Cyc_Parse_Abstractdeclarator _tmp739;({struct Cyc_List_List*_tmpB6E=({struct Cyc_List_List*_tmp5BF=_region_malloc(yyr,sizeof(*_tmp5BF));({void*_tmpB6D=(void*)({struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*_tmp5BE=_region_malloc(yyr,sizeof(*_tmp5BE));_tmp5BE->tag=5U,_tmp5BE->f1=(unsigned int)(yyyylsp[1]).first_line,({struct Cyc_List_List*_tmpB6C=Cyc_yyget_YY45(yyyyvsp[1]);_tmp5BE->f2=_tmpB6C;});_tmp5BE;});_tmp5BF->hd=_tmpB6D;}),({struct Cyc_List_List*_tmpB6B=(Cyc_yyget_YY30(yyyyvsp[0])).tms;_tmp5BF->tl=_tmpB6B;});_tmp5BF;});_tmp739.tms=_tmpB6E;});_tmp739;}));yyval=_tmpB6F;});
# 2422
goto _LL0;}case 284U: _LL233: _LL234: {
# 2424
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2426
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2426 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 285U: _LL235: _LL236: {
# 2429
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2431
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2427 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 286U: _LL237: _LL238: {
# 2430
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2432
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2428 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 287U: _LL239: _LL23A: {
# 2431
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2433
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2429 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 288U: _LL23B: _LL23C: {
# 2432
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2434
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2430 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 289U: _LL23D: _LL23E: {
# 2433
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2435
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2431 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 290U: _LL23F: _LL240: {
# 2434
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2436
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpB74=Cyc_Stmt_tok(({void*_tmpB73=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->tag=13U,({struct _dyneither_ptr*_tmpB72=({struct _dyneither_ptr*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));({struct _dyneither_ptr _tmpB71=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5C0=_tmpB71;});_tmp5C0;});_tmp5C1->f1=_tmpB72;}),({struct Cyc_Absyn_Stmt*_tmpB70=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5C1->f2=_tmpB70;});_tmp5C1;});Cyc_Absyn_new_stmt(_tmpB73,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB74;});
goto _LL0;}case 291U: _LL241: _LL242: {
# 2440
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2442
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2441 "parse.y"
({union Cyc_YYSTYPE _tmpB75=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpB75;});
goto _LL0;}case 292U: _LL243: _LL244: {
# 2444
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2446
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2442 "parse.y"
({union Cyc_YYSTYPE _tmpB77=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpB76=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_exp_stmt(_tmpB76,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB77;});
goto _LL0;}case 293U: _LL245: _LL246: {
# 2445
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2447
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2447 "parse.y"
({union Cyc_YYSTYPE _tmpB78=Cyc_Stmt_tok(Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpB78;});
goto _LL0;}case 294U: _LL247: _LL248: {
# 2450
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2452
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2448 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 295U: _LL249: _LL24A: {
# 2451
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2453
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2453 "parse.y"
({union Cyc_YYSTYPE _tmpB7A=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpB79=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpB79,Cyc_Absyn_skip_stmt((unsigned int)(yyyylsp[0]).first_line));}));yyval=_tmpB7A;});
goto _LL0;}case 296U: _LL24B: _LL24C: {
# 2456
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2458
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2454 "parse.y"
({union Cyc_YYSTYPE _tmpB7C=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpB7B=Cyc_yyget_YY16(yyyyvsp[0]);Cyc_Parse_flatten_declarations(_tmpB7B,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpB7C;});
goto _LL0;}case 297U: _LL24D: _LL24E: {
# 2457
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2459
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2455 "parse.y"
({union Cyc_YYSTYPE _tmpB82=Cyc_Stmt_tok(({void*_tmpB81=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->tag=13U,({struct _dyneither_ptr*_tmpB80=({struct _dyneither_ptr*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));({struct _dyneither_ptr _tmpB7F=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5C2=_tmpB7F;});_tmp5C2;});_tmp5C3->f1=_tmpB80;}),({struct Cyc_Absyn_Stmt*_tmpB7E=({struct Cyc_List_List*_tmpB7D=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpB7D,Cyc_Absyn_skip_stmt(0U));});_tmp5C3->f2=_tmpB7E;});_tmp5C3;});Cyc_Absyn_new_stmt(_tmpB81,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB82;});
# 2457
goto _LL0;}case 298U: _LL24F: _LL250: {
# 2459
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2461
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2457 "parse.y"
({union Cyc_YYSTYPE _tmpB88=Cyc_Stmt_tok(({void*_tmpB87=(void*)({struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->tag=13U,({struct _dyneither_ptr*_tmpB86=({struct _dyneither_ptr*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));({struct _dyneither_ptr _tmpB85=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5C4=_tmpB85;});_tmp5C4;});_tmp5C5->f1=_tmpB86;}),({struct Cyc_Absyn_Stmt*_tmpB84=({struct Cyc_List_List*_tmpB83=Cyc_yyget_YY16(yyyyvsp[2]);Cyc_Parse_flatten_declarations(_tmpB83,Cyc_yyget_Stmt_tok(yyyyvsp[3]));});_tmp5C5->f2=_tmpB84;});_tmp5C5;});Cyc_Absyn_new_stmt(_tmpB87,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB88;});
# 2459
goto _LL0;}case 299U: _LL251: _LL252: {
# 2461
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2463
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2459 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 300U: _LL253: _LL254: {
# 2462
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2464
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2460 "parse.y"
({union Cyc_YYSTYPE _tmpB8B=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpB8A=Cyc_yyget_Stmt_tok(yyyyvsp[0]);struct Cyc_Absyn_Stmt*_tmpB89=Cyc_yyget_Stmt_tok(yyyyvsp[1]);Cyc_Absyn_seq_stmt(_tmpB8A,_tmpB89,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB8B;});
goto _LL0;}case 301U: _LL255: _LL256: {
# 2463
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2465
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2461 "parse.y"
({union Cyc_YYSTYPE _tmpB8F=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpB8E=({void*_tmpB8D=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5C6=_cycalloc(sizeof(*_tmp5C6));_tmp5C6->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpB8C=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5C6->f1=_tmpB8C;});_tmp5C6;});Cyc_Absyn_new_decl(_tmpB8D,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpB8E,
Cyc_Absyn_skip_stmt(0U));}));
# 2461
yyval=_tmpB8F;});
# 2463
goto _LL0;}case 302U: _LL257: _LL258: {
# 2465
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2467
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2464 "parse.y"
({union Cyc_YYSTYPE _tmpB93=Cyc_Stmt_tok(({struct Cyc_Absyn_Decl*_tmpB92=({void*_tmpB91=(void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp5C7=_cycalloc(sizeof(*_tmp5C7));_tmp5C7->tag=1U,({struct Cyc_Absyn_Fndecl*_tmpB90=Cyc_yyget_YY15(yyyyvsp[0]);_tmp5C7->f1=_tmpB90;});_tmp5C7;});Cyc_Absyn_new_decl(_tmpB91,(unsigned int)(yyyylsp[0]).first_line);});Cyc_Parse_flatten_decl(_tmpB92,Cyc_yyget_Stmt_tok(yyyyvsp[1]));}));yyval=_tmpB93;});
goto _LL0;}case 303U: _LL259: _LL25A: {
# 2467
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2469
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2469 "parse.y"
({union Cyc_YYSTYPE _tmpB97=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpB96=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpB95=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpB94=Cyc_Absyn_skip_stmt(0U);Cyc_Absyn_ifthenelse_stmt(_tmpB96,_tmpB95,_tmpB94,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB97;});
goto _LL0;}case 304U: _LL25B: _LL25C: {
# 2472
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2474
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2471 "parse.y"
({union Cyc_YYSTYPE _tmpB9B=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpB9A=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpB99=Cyc_yyget_Stmt_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpB98=Cyc_yyget_Stmt_tok(yyyyvsp[6]);Cyc_Absyn_ifthenelse_stmt(_tmpB9A,_tmpB99,_tmpB98,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB9B;});
goto _LL0;}case 305U: _LL25D: _LL25E: {
# 2474
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2476
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
({union Cyc_YYSTYPE _tmpB9E=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpB9D=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_List_List*_tmpB9C=Cyc_yyget_YY8(yyyyvsp[5]);Cyc_Absyn_switch_stmt(_tmpB9D,_tmpB9C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpB9E;});
goto _LL0;}case 306U: _LL25F: _LL260: {
# 2480
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2482
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2480 "parse.y"
struct Cyc_Absyn_Exp*_tmp5C8=({struct _tuple0*_tmpB9F=Cyc_yyget_QualId_tok(yyyyvsp[1]);Cyc_Absyn_unknownid_exp(_tmpB9F,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpBA2=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBA1=_tmp5C8;struct Cyc_List_List*_tmpBA0=Cyc_yyget_YY8(yyyyvsp[3]);Cyc_Absyn_switch_stmt(_tmpBA1,_tmpBA0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBA2;});
goto _LL0;}case 307U: _LL261: _LL262: {
# 2484
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2486
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2484 "parse.y"
struct Cyc_Absyn_Exp*_tmp5C9=({struct Cyc_List_List*_tmpBA3=Cyc_yyget_YY4(yyyyvsp[3]);Cyc_Absyn_tuple_exp(_tmpBA3,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpBA6=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBA5=_tmp5C9;struct Cyc_List_List*_tmpBA4=Cyc_yyget_YY8(yyyyvsp[6]);Cyc_Absyn_switch_stmt(_tmpBA5,_tmpBA4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBA6;});
# 2487
goto _LL0;}case 308U: _LL263: _LL264: {
# 2489
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2491
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2490 "parse.y"
({union Cyc_YYSTYPE _tmpBA9=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpBA8=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_List_List*_tmpBA7=Cyc_yyget_YY8(yyyyvsp[4]);Cyc_Absyn_trycatch_stmt(_tmpBA8,_tmpBA7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBA9;});
goto _LL0;}case 309U: _LL265: _LL266:
# 2493
({union Cyc_YYSTYPE _tmpBAA=Cyc_YY8(0);yyval=_tmpBAA;});
goto _LL0;case 310U: _LL267: _LL268: {
# 2496
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2498
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2508 "parse.y"
({union Cyc_YYSTYPE _tmpBAF=Cyc_YY8(({struct Cyc_List_List*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));({struct Cyc_Absyn_Switch_clause*_tmpBAE=({struct Cyc_Absyn_Switch_clause*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));({struct Cyc_Absyn_Pat*_tmpBAD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line);_tmp5CA->pattern=_tmpBAD;}),_tmp5CA->pat_vars=0,_tmp5CA->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpBAC=Cyc_yyget_Stmt_tok(yyyyvsp[2]);_tmp5CA->body=_tmpBAC;}),_tmp5CA->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5CA;});
# 2508
_tmp5CB->hd=_tmpBAE;}),({
# 2510
struct Cyc_List_List*_tmpBAB=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5CB->tl=_tmpBAB;});_tmp5CB;}));
# 2508
yyval=_tmpBAF;});
# 2511
goto _LL0;}case 311U: _LL269: _LL26A: {
# 2513
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2515
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2512 "parse.y"
({union Cyc_YYSTYPE _tmpBB4=Cyc_YY8(({struct Cyc_List_List*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));({struct Cyc_Absyn_Switch_clause*_tmpBB3=({struct Cyc_Absyn_Switch_clause*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));({struct Cyc_Absyn_Pat*_tmpBB2=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5CC->pattern=_tmpBB2;}),_tmp5CC->pat_vars=0,_tmp5CC->where_clause=0,({
struct Cyc_Absyn_Stmt*_tmpBB1=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[2]).first_line);_tmp5CC->body=_tmpBB1;}),_tmp5CC->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5CC;});
# 2512
_tmp5CD->hd=_tmpBB3;}),({
# 2514
struct Cyc_List_List*_tmpBB0=Cyc_yyget_YY8(yyyyvsp[3]);_tmp5CD->tl=_tmpBB0;});_tmp5CD;}));
# 2512
yyval=_tmpBB4;});
# 2515
goto _LL0;}case 312U: _LL26B: _LL26C: {
# 2517
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2519
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2516 "parse.y"
({union Cyc_YYSTYPE _tmpBB9=Cyc_YY8(({struct Cyc_List_List*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));({struct Cyc_Absyn_Switch_clause*_tmpBB8=({struct Cyc_Absyn_Switch_clause*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));({struct Cyc_Absyn_Pat*_tmpBB7=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5CE->pattern=_tmpBB7;}),_tmp5CE->pat_vars=0,_tmp5CE->where_clause=0,({struct Cyc_Absyn_Stmt*_tmpBB6=Cyc_yyget_Stmt_tok(yyyyvsp[3]);_tmp5CE->body=_tmpBB6;}),_tmp5CE->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5CE;});_tmp5CF->hd=_tmpBB8;}),({struct Cyc_List_List*_tmpBB5=Cyc_yyget_YY8(yyyyvsp[4]);_tmp5CF->tl=_tmpBB5;});_tmp5CF;}));yyval=_tmpBB9;});
goto _LL0;}case 313U: _LL26D: _LL26E: {
# 2519
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2521
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2518 "parse.y"
({union Cyc_YYSTYPE _tmpBBF=Cyc_YY8(({struct Cyc_List_List*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));({struct Cyc_Absyn_Switch_clause*_tmpBBE=({struct Cyc_Absyn_Switch_clause*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));({struct Cyc_Absyn_Pat*_tmpBBD=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5D0->pattern=_tmpBBD;}),_tmp5D0->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpBBC=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5D0->where_clause=_tmpBBC;}),({
struct Cyc_Absyn_Stmt*_tmpBBB=Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[4]).first_line);_tmp5D0->body=_tmpBBB;}),_tmp5D0->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5D0;});
# 2518
_tmp5D1->hd=_tmpBBE;}),({
# 2520
struct Cyc_List_List*_tmpBBA=Cyc_yyget_YY8(yyyyvsp[5]);_tmp5D1->tl=_tmpBBA;});_tmp5D1;}));
# 2518
yyval=_tmpBBF;});
# 2521
goto _LL0;}case 314U: _LL26F: _LL270: {
# 2523
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2525
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2522 "parse.y"
({union Cyc_YYSTYPE _tmpBC5=Cyc_YY8(({struct Cyc_List_List*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));({struct Cyc_Absyn_Switch_clause*_tmpBC4=({struct Cyc_Absyn_Switch_clause*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));({struct Cyc_Absyn_Pat*_tmpBC3=Cyc_yyget_YY9(yyyyvsp[1]);_tmp5D2->pattern=_tmpBC3;}),_tmp5D2->pat_vars=0,({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_yyget_Exp_tok(yyyyvsp[3]);_tmp5D2->where_clause=_tmpBC2;}),({struct Cyc_Absyn_Stmt*_tmpBC1=Cyc_yyget_Stmt_tok(yyyyvsp[5]);_tmp5D2->body=_tmpBC1;}),_tmp5D2->loc=(unsigned int)(yyyylsp[0]).first_line;_tmp5D2;});_tmp5D3->hd=_tmpBC4;}),({struct Cyc_List_List*_tmpBC0=Cyc_yyget_YY8(yyyyvsp[6]);_tmp5D3->tl=_tmpBC0;});_tmp5D3;}));yyval=_tmpBC5;});
goto _LL0;}case 315U: _LL271: _LL272: {
# 2525
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2527
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2529 "parse.y"
({union Cyc_YYSTYPE _tmpBC8=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBC7=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Stmt*_tmpBC6=Cyc_yyget_Stmt_tok(yyyyvsp[4]);Cyc_Absyn_while_stmt(_tmpBC7,_tmpBC6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBC8;});
goto _LL0;}case 316U: _LL273: _LL274: {
# 2532
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2534
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2533 "parse.y"
({union Cyc_YYSTYPE _tmpBCB=Cyc_Stmt_tok(({struct Cyc_Absyn_Stmt*_tmpBCA=Cyc_yyget_Stmt_tok(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpBC9=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_do_stmt(_tmpBCA,_tmpBC9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBCB;});
goto _LL0;}case 317U: _LL275: _LL276: {
# 2536
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2538
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2537 "parse.y"
({union Cyc_YYSTYPE _tmpBD0=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBCF=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpBCE=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpBCD=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpBCC=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2537
Cyc_Absyn_for_stmt(_tmpBCF,_tmpBCE,_tmpBCD,_tmpBCC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBD0;});
# 2539
goto _LL0;}case 318U: _LL277: _LL278: {
# 2541
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2543
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2540 "parse.y"
({union Cyc_YYSTYPE _tmpBD5=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBD4=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpBD3=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpBD2=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpBD1=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2540
Cyc_Absyn_for_stmt(_tmpBD4,_tmpBD3,_tmpBD2,_tmpBD1,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBD5;});
# 2542
goto _LL0;}case 319U: _LL279: _LL27A: {
# 2544
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2546
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2543 "parse.y"
({union Cyc_YYSTYPE _tmpBDA=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBD9=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpBD8=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpBD7=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpBD6=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2543
Cyc_Absyn_for_stmt(_tmpBD9,_tmpBD8,_tmpBD7,_tmpBD6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBDA;});
# 2545
goto _LL0;}case 320U: _LL27B: _LL27C: {
# 2547
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2549
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2546 "parse.y"
({union Cyc_YYSTYPE _tmpBDF=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBDE=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpBDD=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpBDC=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpBDB=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2546
Cyc_Absyn_for_stmt(_tmpBDE,_tmpBDD,_tmpBDC,_tmpBDB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBDF;});
# 2548
goto _LL0;}case 321U: _LL27D: _LL27E: {
# 2550
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2552
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2549 "parse.y"
({union Cyc_YYSTYPE _tmpBE4=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBE3=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpBE2=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpBE1=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpBE0=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2549
Cyc_Absyn_for_stmt(_tmpBE3,_tmpBE2,_tmpBE1,_tmpBE0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBE4;});
# 2551
goto _LL0;}case 322U: _LL27F: _LL280: {
# 2553
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2555
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2552 "parse.y"
({union Cyc_YYSTYPE _tmpBE9=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBE8=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpBE7=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpBE6=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpBE5=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2552
Cyc_Absyn_for_stmt(_tmpBE8,_tmpBE7,_tmpBE6,_tmpBE5,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBE9;});
# 2554
goto _LL0;}case 323U: _LL281: _LL282: {
# 2556
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2558
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2555 "parse.y"
({union Cyc_YYSTYPE _tmpBEE=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBED=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpBEC=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpBEB=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpBEA=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2555
Cyc_Absyn_for_stmt(_tmpBED,_tmpBEC,_tmpBEB,_tmpBEA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBEE;});
# 2557
goto _LL0;}case 324U: _LL283: _LL284: {
# 2559
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 2561
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 2558 "parse.y"
({union Cyc_YYSTYPE _tmpBF3=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpBF2=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpBF1=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Exp*_tmpBF0=Cyc_yyget_Exp_tok(yyyyvsp[6]);struct Cyc_Absyn_Stmt*_tmpBEF=
Cyc_yyget_Stmt_tok(yyyyvsp[8]);
# 2558
Cyc_Absyn_for_stmt(_tmpBF2,_tmpBF1,_tmpBF0,_tmpBEF,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpBF3;});
# 2560
goto _LL0;}case 325U: _LL285: _LL286: {
# 2562
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2564
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2561 "parse.y"
struct Cyc_List_List*_tmp5D4=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5D5=({struct Cyc_Absyn_Exp*_tmpBF7=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpBF6=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpBF5=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpBF4=
Cyc_yyget_Stmt_tok(yyyyvsp[5]);
# 2562
Cyc_Absyn_for_stmt(_tmpBF7,_tmpBF6,_tmpBF5,_tmpBF4,(unsigned int)(yyyylsp[0]).first_line);});
# 2564
({union Cyc_YYSTYPE _tmpBF8=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5D4,_tmp5D5));yyval=_tmpBF8;});
# 2566
goto _LL0;}case 326U: _LL287: _LL288: {
# 2568
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2570
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2567 "parse.y"
struct Cyc_List_List*_tmp5D6=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5D7=({struct Cyc_Absyn_Exp*_tmpBFC=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpBFB=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpBFA=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Stmt*_tmpBF9=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2568
Cyc_Absyn_for_stmt(_tmpBFC,_tmpBFB,_tmpBFA,_tmpBF9,(unsigned int)(yyyylsp[0]).first_line);});
# 2570
({union Cyc_YYSTYPE _tmpBFD=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5D6,_tmp5D7));yyval=_tmpBFD;});
# 2572
goto _LL0;}case 327U: _LL289: _LL28A: {
# 2574
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 2576
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 2573 "parse.y"
struct Cyc_List_List*_tmp5D8=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5D9=({struct Cyc_Absyn_Exp*_tmpC01=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC00=Cyc_Absyn_true_exp(0U);struct Cyc_Absyn_Exp*_tmpBFF=Cyc_yyget_Exp_tok(yyyyvsp[4]);struct Cyc_Absyn_Stmt*_tmpBFE=
Cyc_yyget_Stmt_tok(yyyyvsp[6]);
# 2574
Cyc_Absyn_for_stmt(_tmpC01,_tmpC00,_tmpBFF,_tmpBFE,(unsigned int)(yyyylsp[0]).first_line);});
# 2576
({union Cyc_YYSTYPE _tmpC02=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5D8,_tmp5D9));yyval=_tmpC02;});
# 2578
goto _LL0;}case 328U: _LL28B: _LL28C: {
# 2580
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 7),sizeof(union Cyc_YYSTYPE),8U));
# 2582
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 7),sizeof(struct Cyc_Yyltype),8U));
# 2579 "parse.y"
struct Cyc_List_List*_tmp5DA=Cyc_yyget_YY16(yyyyvsp[2]);
struct Cyc_Absyn_Stmt*_tmp5DB=({struct Cyc_Absyn_Exp*_tmpC06=Cyc_Absyn_false_exp(0U);struct Cyc_Absyn_Exp*_tmpC05=Cyc_yyget_Exp_tok(yyyyvsp[3]);struct Cyc_Absyn_Exp*_tmpC04=Cyc_yyget_Exp_tok(yyyyvsp[5]);struct Cyc_Absyn_Stmt*_tmpC03=
Cyc_yyget_Stmt_tok(yyyyvsp[7]);
# 2580
Cyc_Absyn_for_stmt(_tmpC06,_tmpC05,_tmpC04,_tmpC03,(unsigned int)(yyyylsp[0]).first_line);});
# 2582
({union Cyc_YYSTYPE _tmpC07=Cyc_Stmt_tok(Cyc_Parse_flatten_declarations(_tmp5DA,_tmp5DB));yyval=_tmpC07;});
# 2584
goto _LL0;}case 329U: _LL28D: _LL28E: {
# 2586
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2588
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
({union Cyc_YYSTYPE _tmpC0A=Cyc_Stmt_tok(({struct _dyneither_ptr*_tmpC09=({struct _dyneither_ptr*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct _dyneither_ptr _tmpC08=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp5DC=_tmpC08;});_tmp5DC;});Cyc_Absyn_goto_stmt(_tmpC09,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC0A;});
goto _LL0;}case 330U: _LL28F: _LL290: {
# 2592
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2594
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2590 "parse.y"
({union Cyc_YYSTYPE _tmpC0B=Cyc_Stmt_tok(Cyc_Absyn_continue_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC0B;});
goto _LL0;}case 331U: _LL291: _LL292: {
# 2593
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2595
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2591 "parse.y"
({union Cyc_YYSTYPE _tmpC0C=Cyc_Stmt_tok(Cyc_Absyn_break_stmt((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC0C;});
goto _LL0;}case 332U: _LL293: _LL294: {
# 2594
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2596
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2592 "parse.y"
({union Cyc_YYSTYPE _tmpC0D=Cyc_Stmt_tok(Cyc_Absyn_return_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC0D;});
goto _LL0;}case 333U: _LL295: _LL296: {
# 2595
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2597
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2593 "parse.y"
({union Cyc_YYSTYPE _tmpC0F=Cyc_Stmt_tok(({struct Cyc_Absyn_Exp*_tmpC0E=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_return_stmt(_tmpC0E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC0F;});
goto _LL0;}case 334U: _LL297: _LL298: {
# 2596
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2598
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2595 "parse.y"
({union Cyc_YYSTYPE _tmpC10=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC10;});
goto _LL0;}case 335U: _LL299: _LL29A: {
# 2598
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2600
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2596 "parse.y"
({union Cyc_YYSTYPE _tmpC11=Cyc_Stmt_tok(Cyc_Absyn_fallthru_stmt(0,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC11;});
goto _LL0;}case 336U: _LL29B: _LL29C: {
# 2599
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2601
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2598 "parse.y"
({union Cyc_YYSTYPE _tmpC13=Cyc_Stmt_tok(({struct Cyc_List_List*_tmpC12=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_fallthru_stmt(_tmpC12,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC13;});
goto _LL0;}case 337U: _LL29D: _LL29E: {
# 2601
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2603
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2607 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 338U: _LL29F: _LL2A0: {
# 2610
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2612
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2610 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 339U: _LL2A1: _LL2A2: {
# 2613
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2615
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2612 "parse.y"
({union Cyc_YYSTYPE _tmpC17=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC16=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC15=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpC14=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpC16,_tmpC15,_tmpC14,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC17;});
goto _LL0;}case 340U: _LL2A3: _LL2A4: {
# 2615
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2617
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2615 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 341U: _LL2A5: _LL2A6: {
# 2618
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2620
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2617 "parse.y"
({union Cyc_YYSTYPE _tmpC1A=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC19=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC18=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpC19,_tmpC18,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC1A;});
goto _LL0;}case 342U: _LL2A7: _LL2A8: {
# 2620
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2622
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2620 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 343U: _LL2A9: _LL2AA: {
# 2623
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2625
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2622 "parse.y"
({union Cyc_YYSTYPE _tmpC1D=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC1C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC1B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpC1C,_tmpC1B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC1D;});
goto _LL0;}case 344U: _LL2AB: _LL2AC: {
# 2625
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2627
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2625 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 345U: _LL2AD: _LL2AE: {
# 2628
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2630
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2627 "parse.y"
({union Cyc_YYSTYPE _tmpC20=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC1F=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC1E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpC1F,_tmpC1E,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC20;});
goto _LL0;}case 346U: _LL2AF: _LL2B0: {
# 2630
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2632
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2630 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 347U: _LL2B1: _LL2B2: {
# 2633
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2635
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2632 "parse.y"
({union Cyc_YYSTYPE _tmpC23=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC22=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC21=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpC22,_tmpC21,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC23;});
goto _LL0;}case 348U: _LL2B3: _LL2B4: {
# 2635
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2637
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2635 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 349U: _LL2B5: _LL2B6: {
# 2638
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2640
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2637 "parse.y"
({union Cyc_YYSTYPE _tmpC26=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC25=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC24=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpC25,_tmpC24,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC26;});
goto _LL0;}case 350U: _LL2B7: _LL2B8: {
# 2640
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2642
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2640 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 351U: _LL2B9: _LL2BA: {
# 2643
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2645
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2642 "parse.y"
({union Cyc_YYSTYPE _tmpC29=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC28=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC27=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpC28,_tmpC27,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC29;});
goto _LL0;}case 352U: _LL2BB: _LL2BC: {
# 2645
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2647
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2644 "parse.y"
({union Cyc_YYSTYPE _tmpC2C=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC2B=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC2A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpC2B,_tmpC2A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC2C;});
goto _LL0;}case 353U: _LL2BD: _LL2BE: {
# 2647
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2649
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2647 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 354U: _LL2BF: _LL2C0: {
# 2650
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2652
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2649 "parse.y"
({union Cyc_YYSTYPE _tmpC2F=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC2E=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC2D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpC2E,_tmpC2D,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC2F;});
goto _LL0;}case 355U: _LL2C1: _LL2C2: {
# 2652
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2654
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2651 "parse.y"
({union Cyc_YYSTYPE _tmpC32=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC31=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC30=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpC31,_tmpC30,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC32;});
goto _LL0;}case 356U: _LL2C3: _LL2C4: {
# 2654
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2656
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2653 "parse.y"
({union Cyc_YYSTYPE _tmpC35=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC34=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC33=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpC34,_tmpC33,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC35;});
goto _LL0;}case 357U: _LL2C5: _LL2C6: {
# 2656
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2658
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2655 "parse.y"
({union Cyc_YYSTYPE _tmpC38=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC37=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC36=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpC37,_tmpC36,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC38;});
goto _LL0;}case 358U: _LL2C7: _LL2C8: {
# 2658
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2660
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2658 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 359U: _LL2C9: _LL2CA: {
# 2661
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2663
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2660 "parse.y"
({union Cyc_YYSTYPE _tmpC3B=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC3A=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC39=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpC3A,_tmpC39,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC3B;});
goto _LL0;}case 360U: _LL2CB: _LL2CC: {
# 2663
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2665
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2662 "parse.y"
({union Cyc_YYSTYPE _tmpC3E=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC3D=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC3C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpC3D,_tmpC3C,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC3E;});
goto _LL0;}case 361U: _LL2CD: _LL2CE: {
# 2665
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2667
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2665 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 362U: _LL2CF: _LL2D0: {
# 2668
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2670
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2667 "parse.y"
({union Cyc_YYSTYPE _tmpC41=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC40=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC3F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpC40,_tmpC3F,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC41;});
goto _LL0;}case 363U: _LL2D1: _LL2D2: {
# 2670
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2672
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2669 "parse.y"
({union Cyc_YYSTYPE _tmpC44=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC43=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC42=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpC43,_tmpC42,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC44;});
goto _LL0;}case 364U: _LL2D3: _LL2D4: {
# 2672
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2674
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2672 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 365U: _LL2D5: _LL2D6: {
# 2675
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2677
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2674 "parse.y"
({union Cyc_YYSTYPE _tmpC47=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC46=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC45=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpC46,_tmpC45,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC47;});
goto _LL0;}case 366U: _LL2D7: _LL2D8: {
# 2677
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2679
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2676 "parse.y"
({union Cyc_YYSTYPE _tmpC4A=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC49=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC48=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpC49,_tmpC48,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC4A;});
goto _LL0;}case 367U: _LL2D9: _LL2DA: {
# 2679
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2681
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2678 "parse.y"
({union Cyc_YYSTYPE _tmpC4D=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC4C=Cyc_Parse_pat2exp(Cyc_yyget_YY9(yyyyvsp[0]));struct Cyc_Absyn_Exp*_tmpC4B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpC4C,_tmpC4B,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC4D;});
goto _LL0;}case 368U: _LL2DB: _LL2DC: {
# 2681
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2683
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2681 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 369U: _LL2DD: _LL2DE: {
# 2684
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2686
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2683 "parse.y"
void*_tmp5DD=({struct _tuple8*_tmpC4E=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpC4E,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpC51=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpC50=_tmp5DD;struct Cyc_Absyn_Exp*_tmpC4F=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpC50,_tmpC4F,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC51;});
# 2686
goto _LL0;}case 370U: _LL2DF: _LL2E0: {
# 2688
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2690
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2689 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 371U: _LL2E1: _LL2E2: {
# 2692
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2694
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2692 "parse.y"
({union Cyc_YYSTYPE _tmpC52=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpC52;});
goto _LL0;}case 372U: _LL2E3: _LL2E4: {
# 2695
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2697
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2694 "parse.y"
({union Cyc_YYSTYPE _tmpC55=Cyc_YY9(Cyc_Absyn_exp_pat(({enum Cyc_Absyn_Primop _tmpC54=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpC53=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpC54,_tmpC53,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC55;});
goto _LL0;}case 373U: _LL2E5: _LL2E6: {
# 2697
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2699
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2696 "parse.y"
void*_tmp5DE=({struct _tuple8*_tmpC56=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpC56,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpC57=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_Absyn_sizeoftype_exp(_tmp5DE,(unsigned int)(yyyylsp[0]).first_line)));yyval=_tmpC57;});
# 2699
goto _LL0;}case 374U: _LL2E7: _LL2E8: {
# 2701
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2703
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2700 "parse.y"
({union Cyc_YYSTYPE _tmpC59=Cyc_YY9(Cyc_Absyn_exp_pat(({struct Cyc_Absyn_Exp*_tmpC58=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpC58,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC59;});
goto _LL0;}case 375U: _LL2E9: _LL2EA: {
# 2703
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2705
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2702 "parse.y"
({union Cyc_YYSTYPE _tmpC5C=Cyc_YY9(Cyc_Absyn_exp_pat(({void*_tmpC5B=(*Cyc_yyget_YY37(yyyyvsp[2])).f3;struct Cyc_List_List*_tmpC5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpC5B,_tmpC5A,(unsigned int)(yyyylsp[0]).first_line);})));yyval=_tmpC5C;});
goto _LL0;}case 376U: _LL2EB: _LL2EC: {
# 2705
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2707
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2707 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 377U: _LL2ED: _LL2EE: {
# 2710
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2712
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2715 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 378U: _LL2EF: _LL2F0: {
# 2718
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2720
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2720 "parse.y"
({union Cyc_YYSTYPE _tmpC5D=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpC5D;});
goto _LL0;}case 379U: _LL2F1: _LL2F2: {
# 2723
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2725
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2722 "parse.y"
({union Cyc_YYSTYPE _tmpC5E=Cyc_YY9(Cyc_Absyn_exp_pat(Cyc_yyget_Exp_tok(yyyyvsp[1])));yyval=_tmpC5E;});
goto _LL0;}case 380U: _LL2F3: _LL2F4: {
# 2725
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2727
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2724 "parse.y"
struct Cyc_Absyn_Exp*e=Cyc_yyget_Exp_tok(yyyyvsp[0]);
{void*_tmp5DF=e->r;void*_tmp5E0=_tmp5DF;struct _dyneither_ptr _tmp5F4;int _tmp5F3;enum Cyc_Absyn_Sign _tmp5F2;int _tmp5F1;enum Cyc_Absyn_Sign _tmp5F0;short _tmp5EF;enum Cyc_Absyn_Sign _tmp5EE;char _tmp5ED;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).LongLong_c).tag){case 2U: _LL46F: _tmp5EE=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Char_c).val).f1;_tmp5ED=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Char_c).val).f2;_LL470:
# 2728
({union Cyc_YYSTYPE _tmpC60=Cyc_YY9(({void*_tmpC5F=(void*)({struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->tag=11U,_tmp5E1->f1=_tmp5ED;_tmp5E1;});Cyc_Absyn_new_pat(_tmpC5F,e->loc);}));yyval=_tmpC60;});goto _LL46E;case 4U: _LL471: _tmp5F0=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Short_c).val).f1;_tmp5EF=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Short_c).val).f2;_LL472:
# 2730
({union Cyc_YYSTYPE _tmpC62=Cyc_YY9(({void*_tmpC61=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));_tmp5E2->tag=10U,_tmp5E2->f1=_tmp5F0,_tmp5E2->f2=(int)_tmp5EF;_tmp5E2;});Cyc_Absyn_new_pat(_tmpC61,e->loc);}));yyval=_tmpC62;});goto _LL46E;case 5U: _LL473: _tmp5F2=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Int_c).val).f1;_tmp5F1=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Int_c).val).f2;_LL474:
# 2732
({union Cyc_YYSTYPE _tmpC64=Cyc_YY9(({void*_tmpC63=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->tag=10U,_tmp5E3->f1=_tmp5F2,_tmp5E3->f2=_tmp5F1;_tmp5E3;});Cyc_Absyn_new_pat(_tmpC63,e->loc);}));yyval=_tmpC64;});goto _LL46E;case 7U: _LL475: _tmp5F4=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Float_c).val).f1;_tmp5F3=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp5E0)->f1).Float_c).val).f2;_LL476:
# 2734
({union Cyc_YYSTYPE _tmpC66=Cyc_YY9(({void*_tmpC65=(void*)({struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->tag=12U,_tmp5E4->f1=_tmp5F4,_tmp5E4->f2=_tmp5F3;_tmp5E4;});Cyc_Absyn_new_pat(_tmpC65,e->loc);}));yyval=_tmpC66;});goto _LL46E;case 1U: _LL477: _LL478:
# 2736
({union Cyc_YYSTYPE _tmpC67=Cyc_YY9(Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Null_p_val,e->loc));yyval=_tmpC67;});goto _LL46E;case 8U: _LL479: _LL47A:
# 2738
({void*_tmp5E5=0U;({unsigned int _tmpC69=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC68=({const char*_tmp5E6="strings cannot occur within patterns";_tag_dyneither(_tmp5E6,sizeof(char),37U);});Cyc_Warn_err(_tmpC69,_tmpC68,_tag_dyneither(_tmp5E5,sizeof(void*),0U));});});goto _LL46E;case 9U: _LL47B: _LL47C:
# 2740
({void*_tmp5E7=0U;({unsigned int _tmpC6B=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC6A=({const char*_tmp5E8="strings cannot occur within patterns";_tag_dyneither(_tmp5E8,sizeof(char),37U);});Cyc_Warn_err(_tmpC6B,_tmpC6A,_tag_dyneither(_tmp5E7,sizeof(void*),0U));});});goto _LL46E;case 6U: _LL47D: _LL47E:
# 2742
({void*_tmp5E9=0U;({unsigned int _tmpC6D=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC6C=({const char*_tmp5EA="long long's in patterns not yet implemented";_tag_dyneither(_tmp5EA,sizeof(char),44U);});Cyc_Warn_err(_tmpC6D,_tmpC6C,_tag_dyneither(_tmp5E9,sizeof(void*),0U));});});goto _LL46E;default: goto _LL47F;}else{_LL47F: _LL480:
# 2744
({void*_tmp5EB=0U;({unsigned int _tmpC6F=(unsigned int)(yyyylsp[0]).first_line;struct _dyneither_ptr _tmpC6E=({const char*_tmp5EC="bad constant in case";_tag_dyneither(_tmp5EC,sizeof(char),21U);});Cyc_Warn_err(_tmpC6F,_tmpC6E,_tag_dyneither(_tmp5EB,sizeof(void*),0U));});});}_LL46E:;}
# 2747
goto _LL0;}case 381U: _LL2F5: _LL2F6: {
# 2749
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2751
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2748 "parse.y"
({union Cyc_YYSTYPE _tmpC72=Cyc_YY9(({void*_tmpC71=(void*)({struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->tag=15U,({struct _tuple0*_tmpC70=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp5F5->f1=_tmpC70;});_tmp5F5;});Cyc_Absyn_new_pat(_tmpC71,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC72;});
goto _LL0;}case 382U: _LL2F7: _LL2F8: {
# 2751
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2753
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2750 "parse.y"
if(({struct _dyneither_ptr _tmpC73=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[1]);Cyc_strcmp(_tmpC73,({const char*_tmp5F6="as";_tag_dyneither(_tmp5F6,sizeof(char),3U);}));})!= 0)
({void*_tmp5F7=0U;({unsigned int _tmpC75=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpC74=({const char*_tmp5F8="expecting `as'";_tag_dyneither(_tmp5F8,sizeof(char),15U);});Cyc_Warn_err(_tmpC75,_tmpC74,_tag_dyneither(_tmp5F7,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpC7D=Cyc_YY9(({void*_tmpC7C=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp5FB=_cycalloc(sizeof(*_tmp5FB));_tmp5FB->tag=1U,({struct Cyc_Absyn_Vardecl*_tmpC7B=({unsigned int _tmpC7A=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpC79=({struct _tuple0*_tmp5FA=_cycalloc(sizeof(*_tmp5FA));_tmp5FA->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpC78=({struct _dyneither_ptr*_tmp5F9=_cycalloc(sizeof(*_tmp5F9));({struct _dyneither_ptr _tmpC77=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp5F9=_tmpC77;});_tmp5F9;});_tmp5FA->f2=_tmpC78;});_tmp5FA;});Cyc_Absyn_new_vardecl(_tmpC7A,_tmpC79,Cyc_Absyn_void_type,0);});_tmp5FB->f1=_tmpC7B;}),({
struct Cyc_Absyn_Pat*_tmpC76=Cyc_yyget_YY9(yyyyvsp[2]);_tmp5FB->f2=_tmpC76;});_tmp5FB;});
# 2752
Cyc_Absyn_new_pat(_tmpC7C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC7D;});
# 2755
goto _LL0;}case 383U: _LL2F9: _LL2FA: {
# 2757
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2759
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2756 "parse.y"
if(({struct _dyneither_ptr _tmpC7E=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpC7E,({const char*_tmp5FC="alias";_tag_dyneither(_tmp5FC,sizeof(char),6U);}));})!= 0)
({void*_tmp5FD=0U;({unsigned int _tmpC80=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpC7F=({const char*_tmp5FE="expecting `alias'";_tag_dyneither(_tmp5FE,sizeof(char),18U);});Cyc_Warn_err(_tmpC80,_tmpC7F,_tag_dyneither(_tmp5FD,sizeof(void*),0U));});});{
int _tmp5FF=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp605=_cycalloc(sizeof(*_tmp605));({struct _dyneither_ptr*_tmpC83=({struct _dyneither_ptr*_tmp603=_cycalloc(sizeof(*_tmp603));({struct _dyneither_ptr _tmpC82=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp603=_tmpC82;});_tmp603;});_tmp605->name=_tmpC83;}),_tmp605->identity=- 1,({void*_tmpC81=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->tag=0U,_tmp604->f1=& Cyc_Tcutil_rk;_tmp604;});_tmp605->kind=_tmpC81;});_tmp605;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpC88=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpC87=({struct _tuple0*_tmp602=_cycalloc(sizeof(*_tmp602));_tmp602->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpC85=({struct _dyneither_ptr*_tmp601=_cycalloc(sizeof(*_tmp601));({struct _dyneither_ptr _tmpC84=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp601=_tmpC84;});_tmp601;});_tmp602->f2=_tmpC85;});_tmp602;});Cyc_Absyn_new_vardecl(_tmpC88,_tmpC87,({
struct _tuple8*_tmpC86=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpC86,(unsigned int)(yyyylsp[4]).first_line);}),0);});
({union Cyc_YYSTYPE _tmpC8A=Cyc_YY9(({void*_tmpC89=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->tag=2U,_tmp600->f1=tv,_tmp600->f2=vd;_tmp600;});Cyc_Absyn_new_pat(_tmpC89,(unsigned int)_tmp5FF);}));yyval=_tmpC8A;});
# 2764
goto _LL0;};}case 384U: _LL2FB: _LL2FC: {
# 2766
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 2768
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 2765 "parse.y"
if(({struct _dyneither_ptr _tmpC8B=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_strcmp(_tmpC8B,({const char*_tmp606="alias";_tag_dyneither(_tmp606,sizeof(char),6U);}));})!= 0)
({void*_tmp607=0U;({unsigned int _tmpC8D=(unsigned int)(yyyylsp[1]).first_line;struct _dyneither_ptr _tmpC8C=({const char*_tmp608="expecting `alias'";_tag_dyneither(_tmp608,sizeof(char),18U);});Cyc_Warn_err(_tmpC8D,_tmpC8C,_tag_dyneither(_tmp607,sizeof(void*),0U));});});{
int _tmp609=(yyyylsp[0]).first_line;
struct Cyc_Absyn_Tvar*tv=({struct Cyc_Absyn_Tvar*_tmp60F=_cycalloc(sizeof(*_tmp60F));({struct _dyneither_ptr*_tmpC90=({struct _dyneither_ptr*_tmp60D=_cycalloc(sizeof(*_tmp60D));({struct _dyneither_ptr _tmpC8F=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp60D=_tmpC8F;});_tmp60D;});_tmp60F->name=_tmpC90;}),_tmp60F->identity=- 1,({void*_tmpC8E=(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->tag=0U,_tmp60E->f1=& Cyc_Tcutil_rk;_tmp60E;});_tmp60F->kind=_tmpC8E;});_tmp60F;});
struct Cyc_Absyn_Vardecl*vd=({unsigned int _tmpC95=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpC94=({struct _tuple0*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpC92=({struct _dyneither_ptr*_tmp60B=_cycalloc(sizeof(*_tmp60B));({struct _dyneither_ptr _tmpC91=Cyc_yyget_String_tok(yyyyvsp[5]);*_tmp60B=_tmpC91;});_tmp60B;});_tmp60C->f2=_tmpC92;});_tmp60C;});Cyc_Absyn_new_vardecl(_tmpC95,_tmpC94,({
struct _tuple8*_tmpC93=Cyc_yyget_YY37(yyyyvsp[4]);Cyc_Parse_type_name_to_type(_tmpC93,(unsigned int)(yyyylsp[4]).first_line);}),0);});
({union Cyc_YYSTYPE _tmpC97=Cyc_YY9(({void*_tmpC96=(void*)({struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->tag=2U,_tmp60A->f1=tv,_tmp60A->f2=vd;_tmp60A;});Cyc_Absyn_new_pat(_tmpC96,(unsigned int)_tmp609);}));yyval=_tmpC97;});
# 2773
goto _LL0;};}case 385U: _LL2FD: _LL2FE: {
# 2775
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2777
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2774 "parse.y"
struct _tuple22 _tmp610=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp611=_tmp610;struct Cyc_List_List*_tmp614;int _tmp613;_LL482: _tmp614=_tmp611.f1;_tmp613=_tmp611.f2;_LL483:;
({union Cyc_YYSTYPE _tmpC99=Cyc_YY9(({void*_tmpC98=(void*)({struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*_tmp612=_cycalloc(sizeof(*_tmp612));_tmp612->tag=5U,_tmp612->f1=_tmp614,_tmp612->f2=_tmp613;_tmp612;});Cyc_Absyn_new_pat(_tmpC98,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC99;});
# 2777
goto _LL0;}case 386U: _LL2FF: _LL300: {
# 2779
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2781
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2778 "parse.y"
struct _tuple22 _tmp615=*Cyc_yyget_YY10(yyyyvsp[2]);struct _tuple22 _tmp616=_tmp615;struct Cyc_List_List*_tmp619;int _tmp618;_LL485: _tmp619=_tmp616.f1;_tmp618=_tmp616.f2;_LL486:;
({union Cyc_YYSTYPE _tmpC9C=Cyc_YY9(({void*_tmpC9B=(void*)({struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->tag=16U,({struct _tuple0*_tmpC9A=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp617->f1=_tmpC9A;}),_tmp617->f2=_tmp619,_tmp617->f3=_tmp618;_tmp617;});Cyc_Absyn_new_pat(_tmpC9B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpC9C;});
# 2781
goto _LL0;}case 387U: _LL301: _LL302: {
# 2783
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2785
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2782 "parse.y"
struct _tuple22 _tmp61A=*Cyc_yyget_YY14(yyyyvsp[3]);struct _tuple22 _tmp61B=_tmp61A;struct Cyc_List_List*_tmp620;int _tmp61F;_LL488: _tmp620=_tmp61B.f1;_tmp61F=_tmp61B.f2;_LL489:;{
struct Cyc_List_List*_tmp61C=({unsigned int _tmpC9D=(unsigned int)(yyyylsp[2]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpC9D,Cyc_yyget_YY40(yyyyvsp[2]));});
({union Cyc_YYSTYPE _tmpCA1=Cyc_YY9(({void*_tmpCA0=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp61E=_cycalloc(sizeof(*_tmp61E));_tmp61E->tag=7U,({union Cyc_Absyn_AggrInfo*_tmpC9F=({union Cyc_Absyn_AggrInfo*_tmp61D=_cycalloc(sizeof(*_tmp61D));({union Cyc_Absyn_AggrInfo _tmpC9E=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,Cyc_yyget_QualId_tok(yyyyvsp[0]),0);*_tmp61D=_tmpC9E;});_tmp61D;});_tmp61E->f1=_tmpC9F;}),_tmp61E->f2=_tmp61C,_tmp61E->f3=_tmp620,_tmp61E->f4=_tmp61F;_tmp61E;});Cyc_Absyn_new_pat(_tmpCA0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCA1;});
# 2787
goto _LL0;};}case 388U: _LL303: _LL304: {
# 2789
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2791
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2788 "parse.y"
struct _tuple22 _tmp621=*Cyc_yyget_YY14(yyyyvsp[2]);struct _tuple22 _tmp622=_tmp621;struct Cyc_List_List*_tmp626;int _tmp625;_LL48B: _tmp626=_tmp622.f1;_tmp625=_tmp622.f2;_LL48C:;{
struct Cyc_List_List*_tmp623=({unsigned int _tmpCA2=(unsigned int)(yyyylsp[1]).first_line;((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(unsigned int,void*),unsigned int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Parse_typ2tvar,_tmpCA2,Cyc_yyget_YY40(yyyyvsp[1]));});
({union Cyc_YYSTYPE _tmpCA4=Cyc_YY9(({void*_tmpCA3=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624->tag=7U,_tmp624->f1=0,_tmp624->f2=_tmp623,_tmp624->f3=_tmp626,_tmp624->f4=_tmp625;_tmp624;});Cyc_Absyn_new_pat(_tmpCA3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCA4;});
# 2792
goto _LL0;};}case 389U: _LL305: _LL306: {
# 2794
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2796
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2793 "parse.y"
({union Cyc_YYSTYPE _tmpCA7=Cyc_YY9(({void*_tmpCA6=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=6U,({struct Cyc_Absyn_Pat*_tmpCA5=Cyc_yyget_YY9(yyyyvsp[1]);_tmp627->f1=_tmpCA5;});_tmp627;});Cyc_Absyn_new_pat(_tmpCA6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCA7;});
goto _LL0;}case 390U: _LL307: _LL308: {
# 2796
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2798
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2795 "parse.y"
({union Cyc_YYSTYPE _tmpCAC=Cyc_YY9(({void*_tmpCAB=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629->tag=6U,({struct Cyc_Absyn_Pat*_tmpCAA=({void*_tmpCA9=(void*)({struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628->tag=6U,({struct Cyc_Absyn_Pat*_tmpCA8=Cyc_yyget_YY9(yyyyvsp[1]);_tmp628->f1=_tmpCA8;});_tmp628;});Cyc_Absyn_new_pat(_tmpCA9,(unsigned int)(yyyylsp[0]).first_line);});_tmp629->f1=_tmpCAA;});_tmp629;});Cyc_Absyn_new_pat(_tmpCAB,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCAC;});
goto _LL0;}case 391U: _LL309: _LL30A: {
# 2798
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2800
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2797 "parse.y"
({union Cyc_YYSTYPE _tmpCB4=Cyc_YY9(({void*_tmpCB3=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpCB2=({unsigned int _tmpCB1=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCB0=({struct _tuple0*_tmp62B=_cycalloc(sizeof(*_tmp62B));_tmp62B->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCAF=({struct _dyneither_ptr*_tmp62A=_cycalloc(sizeof(*_tmp62A));({struct _dyneither_ptr _tmpCAE=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp62A=_tmpCAE;});_tmp62A;});_tmp62B->f2=_tmpCAF;});_tmp62B;});Cyc_Absyn_new_vardecl(_tmpCB1,_tmpCB0,Cyc_Absyn_void_type,0);});_tmp62C->f1=_tmpCB2;}),({
# 2799
struct Cyc_Absyn_Pat*_tmpCAD=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,(unsigned int)(yyyylsp[1]).first_line);_tmp62C->f2=_tmpCAD;});_tmp62C;});
# 2797
Cyc_Absyn_new_pat(_tmpCB3,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCB4;});
# 2801
goto _LL0;}case 392U: _LL30B: _LL30C: {
# 2803
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2805
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2802 "parse.y"
if(({struct _dyneither_ptr _tmpCB5=(struct _dyneither_ptr)Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_strcmp(_tmpCB5,({const char*_tmp62D="as";_tag_dyneither(_tmp62D,sizeof(char),3U);}));})!= 0)
({void*_tmp62E=0U;({unsigned int _tmpCB7=(unsigned int)(yyyylsp[2]).first_line;struct _dyneither_ptr _tmpCB6=({const char*_tmp62F="expecting `as'";_tag_dyneither(_tmp62F,sizeof(char),15U);});Cyc_Warn_err(_tmpCB7,_tmpCB6,_tag_dyneither(_tmp62E,sizeof(void*),0U));});});
({union Cyc_YYSTYPE _tmpCBF=Cyc_YY9(({void*_tmpCBE=(void*)({struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*_tmp632=_cycalloc(sizeof(*_tmp632));_tmp632->tag=3U,({struct Cyc_Absyn_Vardecl*_tmpCBD=({unsigned int _tmpCBC=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCBB=({struct _tuple0*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCBA=({struct _dyneither_ptr*_tmp630=_cycalloc(sizeof(*_tmp630));({struct _dyneither_ptr _tmpCB9=Cyc_yyget_String_tok(yyyyvsp[1]);*_tmp630=_tmpCB9;});_tmp630;});_tmp631->f2=_tmpCBA;});_tmp631;});Cyc_Absyn_new_vardecl(_tmpCBC,_tmpCBB,Cyc_Absyn_void_type,0);});_tmp632->f1=_tmpCBD;}),({
# 2806
struct Cyc_Absyn_Pat*_tmpCB8=Cyc_yyget_YY9(yyyyvsp[3]);_tmp632->f2=_tmpCB8;});_tmp632;});
# 2804
Cyc_Absyn_new_pat(_tmpCBE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCBF;});
# 2808
goto _LL0;}case 393U: _LL30D: _LL30E: {
# 2810
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2812
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2809 "parse.y"
void*_tmp633=({struct _dyneither_ptr _tmpCC0=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Parse_id2type(_tmpCC0,Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));});
({union Cyc_YYSTYPE _tmpCC8=Cyc_YY9(({void*_tmpCC7=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636->tag=4U,({struct Cyc_Absyn_Tvar*_tmpCC6=Cyc_Parse_typ2tvar((unsigned int)(yyyylsp[2]).first_line,_tmp633);_tmp636->f1=_tmpCC6;}),({
struct Cyc_Absyn_Vardecl*_tmpCC5=({unsigned int _tmpCC4=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCC3=({struct _tuple0*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCC2=({struct _dyneither_ptr*_tmp634=_cycalloc(sizeof(*_tmp634));({struct _dyneither_ptr _tmpCC1=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp634=_tmpCC1;});_tmp634;});_tmp635->f2=_tmpCC2;});_tmp635;});Cyc_Absyn_new_vardecl(_tmpCC4,_tmpCC3,
Cyc_Absyn_tag_type(_tmp633),0);});
# 2811
_tmp636->f2=_tmpCC5;});_tmp636;});
# 2810
Cyc_Absyn_new_pat(_tmpCC7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCC8;});
# 2814
goto _LL0;}case 394U: _LL30F: _LL310: {
# 2816
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2818
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2815 "parse.y"
struct Cyc_Absyn_Tvar*_tmp637=Cyc_Tcutil_new_tvar(Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ik));
({union Cyc_YYSTYPE _tmpCCF=Cyc_YY9(({void*_tmpCCE=(void*)({struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*_tmp63A=_cycalloc(sizeof(*_tmp63A));_tmp63A->tag=4U,_tmp63A->f1=_tmp637,({
struct Cyc_Absyn_Vardecl*_tmpCCD=({unsigned int _tmpCCC=(unsigned int)(yyyylsp[0]).first_line;struct _tuple0*_tmpCCB=({struct _tuple0*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpCCA=({struct _dyneither_ptr*_tmp638=_cycalloc(sizeof(*_tmp638));({struct _dyneither_ptr _tmpCC9=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp638=_tmpCC9;});_tmp638;});_tmp639->f2=_tmpCCA;});_tmp639;});Cyc_Absyn_new_vardecl(_tmpCCC,_tmpCCB,
Cyc_Absyn_tag_type(Cyc_Absyn_var_type(_tmp637)),0);});
# 2817
_tmp63A->f2=_tmpCCD;});_tmp63A;});
# 2816
Cyc_Absyn_new_pat(_tmpCCE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCCF;});
# 2820
goto _LL0;}case 395U: _LL311: _LL312: {
# 2822
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2824
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2823 "parse.y"
({union Cyc_YYSTYPE _tmpCD1=Cyc_YY10(({struct _tuple22*_tmp63B=_cycalloc(sizeof(*_tmp63B));({struct Cyc_List_List*_tmpCD0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp63B->f1=_tmpCD0;}),_tmp63B->f2=0;_tmp63B;}));yyval=_tmpCD1;});
goto _LL0;}case 396U: _LL313: _LL314: {
# 2826
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2828
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2824 "parse.y"
({union Cyc_YYSTYPE _tmpCD3=Cyc_YY10(({struct _tuple22*_tmp63C=_cycalloc(sizeof(*_tmp63C));({struct Cyc_List_List*_tmpCD2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY11(yyyyvsp[0]));_tmp63C->f1=_tmpCD2;}),_tmp63C->f2=1;_tmp63C;}));yyval=_tmpCD3;});
goto _LL0;}case 397U: _LL315: _LL316: {
# 2827
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2829
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2825 "parse.y"
({union Cyc_YYSTYPE _tmpCD4=Cyc_YY10(({struct _tuple22*_tmp63D=_cycalloc(sizeof(*_tmp63D));_tmp63D->f1=0,_tmp63D->f2=1;_tmp63D;}));yyval=_tmpCD4;});
goto _LL0;}case 398U: _LL317: _LL318: {
# 2828
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2830
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2830 "parse.y"
({union Cyc_YYSTYPE _tmpCD6=Cyc_YY11(({struct Cyc_List_List*_tmp63E=_cycalloc(sizeof(*_tmp63E));({struct Cyc_Absyn_Pat*_tmpCD5=Cyc_yyget_YY9(yyyyvsp[0]);_tmp63E->hd=_tmpCD5;}),_tmp63E->tl=0;_tmp63E;}));yyval=_tmpCD6;});
goto _LL0;}case 399U: _LL319: _LL31A: {
# 2833
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2835
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2832 "parse.y"
({union Cyc_YYSTYPE _tmpCD9=Cyc_YY11(({struct Cyc_List_List*_tmp63F=_cycalloc(sizeof(*_tmp63F));({struct Cyc_Absyn_Pat*_tmpCD8=Cyc_yyget_YY9(yyyyvsp[2]);_tmp63F->hd=_tmpCD8;}),({struct Cyc_List_List*_tmpCD7=Cyc_yyget_YY11(yyyyvsp[0]);_tmp63F->tl=_tmpCD7;});_tmp63F;}));yyval=_tmpCD9;});
goto _LL0;}case 400U: _LL31B: _LL31C: {
# 2835
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2837
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2837 "parse.y"
({union Cyc_YYSTYPE _tmpCDB=Cyc_YY12(({struct _tuple23*_tmp640=_cycalloc(sizeof(*_tmp640));_tmp640->f1=0,({struct Cyc_Absyn_Pat*_tmpCDA=Cyc_yyget_YY9(yyyyvsp[0]);_tmp640->f2=_tmpCDA;});_tmp640;}));yyval=_tmpCDB;});
goto _LL0;}case 401U: _LL31D: _LL31E: {
# 2840
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2842
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2839 "parse.y"
({union Cyc_YYSTYPE _tmpCDE=Cyc_YY12(({struct _tuple23*_tmp641=_cycalloc(sizeof(*_tmp641));({struct Cyc_List_List*_tmpCDD=Cyc_yyget_YY41(yyyyvsp[0]);_tmp641->f1=_tmpCDD;}),({struct Cyc_Absyn_Pat*_tmpCDC=Cyc_yyget_YY9(yyyyvsp[1]);_tmp641->f2=_tmpCDC;});_tmp641;}));yyval=_tmpCDE;});
goto _LL0;}case 402U: _LL31F: _LL320: {
# 2842
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2844
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2842 "parse.y"
({union Cyc_YYSTYPE _tmpCE0=Cyc_YY14(({struct _tuple22*_tmp642=_cycalloc(sizeof(*_tmp642));({struct Cyc_List_List*_tmpCDF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp642->f1=_tmpCDF;}),_tmp642->f2=0;_tmp642;}));yyval=_tmpCE0;});
goto _LL0;}case 403U: _LL321: _LL322: {
# 2845
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2847
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2843 "parse.y"
({union Cyc_YYSTYPE _tmpCE2=Cyc_YY14(({struct _tuple22*_tmp643=_cycalloc(sizeof(*_tmp643));({struct Cyc_List_List*_tmpCE1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_yyget_YY13(yyyyvsp[0]));_tmp643->f1=_tmpCE1;}),_tmp643->f2=1;_tmp643;}));yyval=_tmpCE2;});
goto _LL0;}case 404U: _LL323: _LL324: {
# 2846
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2848
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2844 "parse.y"
({union Cyc_YYSTYPE _tmpCE3=Cyc_YY14(({struct _tuple22*_tmp644=_cycalloc(sizeof(*_tmp644));_tmp644->f1=0,_tmp644->f2=1;_tmp644;}));yyval=_tmpCE3;});
goto _LL0;}case 405U: _LL325: _LL326: {
# 2847
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2849
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2849 "parse.y"
({union Cyc_YYSTYPE _tmpCE5=Cyc_YY13(({struct Cyc_List_List*_tmp645=_cycalloc(sizeof(*_tmp645));({struct _tuple23*_tmpCE4=Cyc_yyget_YY12(yyyyvsp[0]);_tmp645->hd=_tmpCE4;}),_tmp645->tl=0;_tmp645;}));yyval=_tmpCE5;});
goto _LL0;}case 406U: _LL327: _LL328: {
# 2852
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2854
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2851 "parse.y"
({union Cyc_YYSTYPE _tmpCE8=Cyc_YY13(({struct Cyc_List_List*_tmp646=_cycalloc(sizeof(*_tmp646));({struct _tuple23*_tmpCE7=Cyc_yyget_YY12(yyyyvsp[2]);_tmp646->hd=_tmpCE7;}),({struct Cyc_List_List*_tmpCE6=Cyc_yyget_YY13(yyyyvsp[0]);_tmp646->tl=_tmpCE6;});_tmp646;}));yyval=_tmpCE8;});
goto _LL0;}case 407U: _LL329: _LL32A: {
# 2854
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2856
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
yyval=yyyyvsp[0];
goto _LL0;}case 408U: _LL32B: _LL32C: {
# 2860
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2862
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2859 "parse.y"
({union Cyc_YYSTYPE _tmpCEB=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpCEA=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpCE9=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_seq_exp(_tmpCEA,_tmpCE9,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCEB;});
goto _LL0;}case 409U: _LL32D: _LL32E: {
# 2862
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2864
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2864 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 410U: _LL32F: _LL330: {
# 2867
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2869
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2866 "parse.y"
({union Cyc_YYSTYPE _tmpCEF=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpCEE=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Core_Opt*_tmpCED=Cyc_yyget_YY7(yyyyvsp[1]);struct Cyc_Absyn_Exp*_tmpCEC=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_assignop_exp(_tmpCEE,_tmpCED,_tmpCEC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCEF;});
goto _LL0;}case 411U: _LL331: _LL332: {
# 2869
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2871
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2868 "parse.y"
({union Cyc_YYSTYPE _tmpCF2=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpCF1=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpCF0=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_swap_exp(_tmpCF1,_tmpCF0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpCF2;});
goto _LL0;}case 412U: _LL333: _LL334: {
# 2871
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2873
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2872 "parse.y"
({union Cyc_YYSTYPE _tmpCF3=Cyc_YY7(0);yyval=_tmpCF3;});
goto _LL0;}case 413U: _LL335: _LL336: {
# 2875
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2877
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2873 "parse.y"
({union Cyc_YYSTYPE _tmpCF4=Cyc_YY7(({struct Cyc_Core_Opt*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->v=(void*)Cyc_Absyn_Times;_tmp647;}));yyval=_tmpCF4;});
goto _LL0;}case 414U: _LL337: _LL338: {
# 2876
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2878
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2874 "parse.y"
({union Cyc_YYSTYPE _tmpCF5=Cyc_YY7(({struct Cyc_Core_Opt*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648->v=(void*)Cyc_Absyn_Div;_tmp648;}));yyval=_tmpCF5;});
goto _LL0;}case 415U: _LL339: _LL33A: {
# 2877
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2879
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2875 "parse.y"
({union Cyc_YYSTYPE _tmpCF6=Cyc_YY7(({struct Cyc_Core_Opt*_tmp649=_cycalloc(sizeof(*_tmp649));_tmp649->v=(void*)Cyc_Absyn_Mod;_tmp649;}));yyval=_tmpCF6;});
goto _LL0;}case 416U: _LL33B: _LL33C: {
# 2878
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2880
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2876 "parse.y"
({union Cyc_YYSTYPE _tmpCF7=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64A=_cycalloc(sizeof(*_tmp64A));_tmp64A->v=(void*)Cyc_Absyn_Plus;_tmp64A;}));yyval=_tmpCF7;});
goto _LL0;}case 417U: _LL33D: _LL33E: {
# 2879
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2881
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2877 "parse.y"
({union Cyc_YYSTYPE _tmpCF8=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64B=_cycalloc(sizeof(*_tmp64B));_tmp64B->v=(void*)Cyc_Absyn_Minus;_tmp64B;}));yyval=_tmpCF8;});
goto _LL0;}case 418U: _LL33F: _LL340: {
# 2880
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2882
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2878 "parse.y"
({union Cyc_YYSTYPE _tmpCF9=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64C=_cycalloc(sizeof(*_tmp64C));_tmp64C->v=(void*)Cyc_Absyn_Bitlshift;_tmp64C;}));yyval=_tmpCF9;});
goto _LL0;}case 419U: _LL341: _LL342: {
# 2881
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2883
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2879 "parse.y"
({union Cyc_YYSTYPE _tmpCFA=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->v=(void*)Cyc_Absyn_Bitlrshift;_tmp64D;}));yyval=_tmpCFA;});
goto _LL0;}case 420U: _LL343: _LL344: {
# 2882
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2884
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2880 "parse.y"
({union Cyc_YYSTYPE _tmpCFB=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64E=_cycalloc(sizeof(*_tmp64E));_tmp64E->v=(void*)Cyc_Absyn_Bitand;_tmp64E;}));yyval=_tmpCFB;});
goto _LL0;}case 421U: _LL345: _LL346: {
# 2883
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2885
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2881 "parse.y"
({union Cyc_YYSTYPE _tmpCFC=Cyc_YY7(({struct Cyc_Core_Opt*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->v=(void*)Cyc_Absyn_Bitxor;_tmp64F;}));yyval=_tmpCFC;});
goto _LL0;}case 422U: _LL347: _LL348: {
# 2884
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2886
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2882 "parse.y"
({union Cyc_YYSTYPE _tmpCFD=Cyc_YY7(({struct Cyc_Core_Opt*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->v=(void*)Cyc_Absyn_Bitor;_tmp650;}));yyval=_tmpCFD;});
goto _LL0;}case 423U: _LL349: _LL34A: {
# 2885
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2887
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2887 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 424U: _LL34B: _LL34C: {
# 2890
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2892
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2889 "parse.y"
({union Cyc_YYSTYPE _tmpD01=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD00=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpCFF=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpCFE=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_conditional_exp(_tmpD00,_tmpCFF,_tmpCFE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD01;});
goto _LL0;}case 425U: _LL34D: _LL34E: {
# 2892
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2894
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2892 "parse.y"
({union Cyc_YYSTYPE _tmpD03=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD02=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_throw_exp(_tmpD02,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD03;});
goto _LL0;}case 426U: _LL34F: _LL350: {
# 2895
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2897
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2895 "parse.y"
({union Cyc_YYSTYPE _tmpD05=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD04=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpD04,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD05;});
goto _LL0;}case 427U: _LL351: _LL352: {
# 2898
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2900
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2897 "parse.y"
({union Cyc_YYSTYPE _tmpD07=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD06=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_New_exp(0,_tmpD06,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD07;});
goto _LL0;}case 428U: _LL353: _LL354: {
# 2900
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2902
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2899 "parse.y"
({union Cyc_YYSTYPE _tmpD0A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD09=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD08=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpD09,_tmpD08,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD0A;});
goto _LL0;}case 429U: _LL355: _LL356: {
# 2902
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 2904
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 2901 "parse.y"
({union Cyc_YYSTYPE _tmpD0D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD0C=Cyc_yyget_Exp_tok(yyyyvsp[2]);struct Cyc_Absyn_Exp*_tmpD0B=Cyc_yyget_Exp_tok(yyyyvsp[4]);Cyc_Absyn_New_exp(_tmpD0C,_tmpD0B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD0D;});
goto _LL0;}case 430U: _LL357: _LL358: {
# 2904
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2906
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2905 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 431U: _LL359: _LL35A: {
# 2908
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2910
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2909 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 432U: _LL35B: _LL35C: {
# 2912
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2914
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2911 "parse.y"
({union Cyc_YYSTYPE _tmpD10=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD0F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD0E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_or_exp(_tmpD0F,_tmpD0E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD10;});
goto _LL0;}case 433U: _LL35D: _LL35E: {
# 2914
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2916
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2915 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 434U: _LL35F: _LL360: {
# 2918
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2920
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2917 "parse.y"
({union Cyc_YYSTYPE _tmpD13=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD12=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD11=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_and_exp(_tmpD12,_tmpD11,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD13;});
goto _LL0;}case 435U: _LL361: _LL362: {
# 2920
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2922
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2921 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 436U: _LL363: _LL364: {
# 2924
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2926
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2923 "parse.y"
({union Cyc_YYSTYPE _tmpD16=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD15=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD14=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitor,_tmpD15,_tmpD14,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD16;});
goto _LL0;}case 437U: _LL365: _LL366: {
# 2926
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2928
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2927 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 438U: _LL367: _LL368: {
# 2930
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2932
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2929 "parse.y"
({union Cyc_YYSTYPE _tmpD19=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD18=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD17=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitxor,_tmpD18,_tmpD17,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD19;});
goto _LL0;}case 439U: _LL369: _LL36A: {
# 2932
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2934
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2933 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 440U: _LL36B: _LL36C: {
# 2936
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2938
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2935 "parse.y"
({union Cyc_YYSTYPE _tmpD1C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD1B=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD1A=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitand,_tmpD1B,_tmpD1A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD1C;});
goto _LL0;}case 441U: _LL36D: _LL36E: {
# 2938
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2940
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2939 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 442U: _LL36F: _LL370: {
# 2942
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2944
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2941 "parse.y"
({union Cyc_YYSTYPE _tmpD1F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD1E=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD1D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_eq_exp(_tmpD1E,_tmpD1D,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD1F;});
goto _LL0;}case 443U: _LL371: _LL372: {
# 2944
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2946
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2943 "parse.y"
({union Cyc_YYSTYPE _tmpD22=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD21=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD20=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_neq_exp(_tmpD21,_tmpD20,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD22;});
goto _LL0;}case 444U: _LL373: _LL374: {
# 2946
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2948
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2947 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 445U: _LL375: _LL376: {
# 2950
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2952
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2949 "parse.y"
({union Cyc_YYSTYPE _tmpD25=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD24=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD23=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lt_exp(_tmpD24,_tmpD23,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD25;});
goto _LL0;}case 446U: _LL377: _LL378: {
# 2952
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2954
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2951 "parse.y"
({union Cyc_YYSTYPE _tmpD28=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD27=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD26=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gt_exp(_tmpD27,_tmpD26,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD28;});
goto _LL0;}case 447U: _LL379: _LL37A: {
# 2954
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2956
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2953 "parse.y"
({union Cyc_YYSTYPE _tmpD2B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD2A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD29=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_lte_exp(_tmpD2A,_tmpD29,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD2B;});
goto _LL0;}case 448U: _LL37B: _LL37C: {
# 2956
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2958
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2955 "parse.y"
({union Cyc_YYSTYPE _tmpD2E=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD2D=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD2C=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_gte_exp(_tmpD2D,_tmpD2C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD2E;});
goto _LL0;}case 449U: _LL37D: _LL37E: {
# 2958
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2960
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2959 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 450U: _LL37F: _LL380: {
# 2962
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2964
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2961 "parse.y"
({union Cyc_YYSTYPE _tmpD31=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD30=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD2F=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlshift,_tmpD30,_tmpD2F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD31;});
goto _LL0;}case 451U: _LL381: _LL382: {
# 2964
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2966
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2963 "parse.y"
({union Cyc_YYSTYPE _tmpD34=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD33=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD32=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Bitlrshift,_tmpD33,_tmpD32,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD34;});
goto _LL0;}case 452U: _LL383: _LL384: {
# 2966
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2968
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2967 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 453U: _LL385: _LL386: {
# 2970
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2972
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2969 "parse.y"
({union Cyc_YYSTYPE _tmpD37=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD36=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD35=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Plus,_tmpD36,_tmpD35,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD37;});
goto _LL0;}case 454U: _LL387: _LL388: {
# 2972
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2974
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2971 "parse.y"
({union Cyc_YYSTYPE _tmpD3A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD39=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD38=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmpD39,_tmpD38,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD3A;});
goto _LL0;}case 455U: _LL389: _LL38A: {
# 2974
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2976
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2975 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 456U: _LL38B: _LL38C: {
# 2978
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2980
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2977 "parse.y"
({union Cyc_YYSTYPE _tmpD3D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD3C=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD3B=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Times,_tmpD3C,_tmpD3B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD3D;});
goto _LL0;}case 457U: _LL38D: _LL38E: {
# 2980
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2982
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2979 "parse.y"
({union Cyc_YYSTYPE _tmpD40=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD3F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD3E=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Div,_tmpD3F,_tmpD3E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD40;});
goto _LL0;}case 458U: _LL38F: _LL390: {
# 2982
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 2984
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 2981 "parse.y"
({union Cyc_YYSTYPE _tmpD43=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD42=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD41=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_prim2_exp(Cyc_Absyn_Mod,_tmpD42,_tmpD41,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD43;});
goto _LL0;}case 459U: _LL391: _LL392: {
# 2984
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2986
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2985 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 460U: _LL393: _LL394: {
# 2988
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 2990
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 2987 "parse.y"
void*_tmp651=({struct _tuple8*_tmpD44=Cyc_yyget_YY37(yyyyvsp[1]);Cyc_Parse_type_name_to_type(_tmpD44,(unsigned int)(yyyylsp[1]).first_line);});
({union Cyc_YYSTYPE _tmpD47=Cyc_Exp_tok(({void*_tmpD46=_tmp651;struct Cyc_Absyn_Exp*_tmpD45=Cyc_yyget_Exp_tok(yyyyvsp[3]);Cyc_Absyn_cast_exp(_tmpD46,_tmpD45,1,Cyc_Absyn_Unknown_coercion,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD47;});
# 2990
goto _LL0;}case 461U: _LL395: _LL396: {
# 2992
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 2994
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 2993 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 462U: _LL397: _LL398: {
# 2996
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2998
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2994 "parse.y"
({union Cyc_YYSTYPE _tmpD49=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD48=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_inc_exp(_tmpD48,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD49;});
goto _LL0;}case 463U: _LL399: _LL39A: {
# 2997
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 2999
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2995 "parse.y"
({union Cyc_YYSTYPE _tmpD4B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD4A=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_pre_dec_exp(_tmpD4A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD4B;});
goto _LL0;}case 464U: _LL39B: _LL39C: {
# 2998
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3000
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2996 "parse.y"
({union Cyc_YYSTYPE _tmpD4D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD4C=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_address_exp(_tmpD4C,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD4D;});
goto _LL0;}case 465U: _LL39D: _LL39E: {
# 2999
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3001
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2997 "parse.y"
({union Cyc_YYSTYPE _tmpD4F=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD4E=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_deref_exp(_tmpD4E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD4F;});
goto _LL0;}case 466U: _LL39F: _LL3A0: {
# 3000
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3002
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2998 "parse.y"
({union Cyc_YYSTYPE _tmpD51=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD50=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(Cyc_Absyn_Plus,_tmpD50,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD51;});
goto _LL0;}case 467U: _LL3A1: _LL3A2: {
# 3001
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3003
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 2999 "parse.y"
({union Cyc_YYSTYPE _tmpD54=Cyc_Exp_tok(({enum Cyc_Absyn_Primop _tmpD53=Cyc_yyget_YY6(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD52=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_prim1_exp(_tmpD53,_tmpD52,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD54;});
goto _LL0;}case 468U: _LL3A3: _LL3A4: {
# 3002
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3004
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3001 "parse.y"
void*_tmp652=({struct _tuple8*_tmpD55=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpD55,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpD56=Cyc_Exp_tok(Cyc_Absyn_sizeoftype_exp(_tmp652,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD56;});
# 3004
goto _LL0;}case 469U: _LL3A5: _LL3A6: {
# 3006
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3008
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3004 "parse.y"
({union Cyc_YYSTYPE _tmpD58=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD57=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_sizeofexp_exp(_tmpD57,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD58;});
goto _LL0;}case 470U: _LL3A7: _LL3A8: {
# 3007
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3009
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3006 "parse.y"
void*_tmp653=({struct _tuple8*_tmpD59=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpD59,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpD5C=Cyc_Exp_tok(({void*_tmpD5B=_tmp653;struct Cyc_List_List*_tmpD5A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY3(yyyyvsp[4]));Cyc_Absyn_offsetof_exp(_tmpD5B,_tmpD5A,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5C;});
# 3009
goto _LL0;}case 471U: _LL3A9: _LL3AA: {
# 3011
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3013
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3011 "parse.y"
({union Cyc_YYSTYPE _tmpD5F=Cyc_Exp_tok(({void*_tmpD5E=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->tag=34U,(_tmp654->f1).is_calloc=0,(_tmp654->f1).rgn=0,(_tmp654->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpD5D=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp654->f1).num_elts=_tmpD5D;}),(_tmp654->f1).fat_result=0,(_tmp654->f1).inline_call=0;_tmp654;});Cyc_Absyn_new_exp(_tmpD5E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD5F;});
# 3013
goto _LL0;}case 472U: _LL3AB: _LL3AC: {
# 3015
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3017
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3014 "parse.y"
({union Cyc_YYSTYPE _tmpD63=Cyc_Exp_tok(({void*_tmpD62=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->tag=34U,(_tmp655->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpD61=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp655->f1).rgn=_tmpD61;}),(_tmp655->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpD60=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp655->f1).num_elts=_tmpD60;}),(_tmp655->f1).fat_result=0,(_tmp655->f1).inline_call=0;_tmp655;});Cyc_Absyn_new_exp(_tmpD62,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD63;});
# 3016
goto _LL0;}case 473U: _LL3AD: _LL3AE: {
# 3018
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3020
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3017 "parse.y"
({union Cyc_YYSTYPE _tmpD67=Cyc_Exp_tok(({void*_tmpD66=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->tag=34U,(_tmp656->f1).is_calloc=0,({struct Cyc_Absyn_Exp*_tmpD65=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp656->f1).rgn=_tmpD65;}),(_tmp656->f1).elt_type=0,({struct Cyc_Absyn_Exp*_tmpD64=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp656->f1).num_elts=_tmpD64;}),(_tmp656->f1).fat_result=0,(_tmp656->f1).inline_call=1;_tmp656;});Cyc_Absyn_new_exp(_tmpD66,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD67;});
# 3019
goto _LL0;}case 474U: _LL3AF: _LL3B0: {
# 3021
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 8),sizeof(union Cyc_YYSTYPE),9U));
# 3023
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 8),sizeof(struct Cyc_Yyltype),9U));
# 3020 "parse.y"
void*_tmp657=({struct _tuple8*_tmpD68=Cyc_yyget_YY37(yyyyvsp[6]);Cyc_Parse_type_name_to_type(_tmpD68,(unsigned int)(yyyylsp[6]).first_line);});
({union Cyc_YYSTYPE _tmpD6C=Cyc_Exp_tok(({void*_tmpD6B=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp659=_cycalloc(sizeof(*_tmp659));_tmp659->tag=34U,(_tmp659->f1).is_calloc=1,(_tmp659->f1).rgn=0,({void**_tmpD6A=({void**_tmp658=_cycalloc(sizeof(*_tmp658));*_tmp658=_tmp657;_tmp658;});(_tmp659->f1).elt_type=_tmpD6A;}),({struct Cyc_Absyn_Exp*_tmpD69=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp659->f1).num_elts=_tmpD69;}),(_tmp659->f1).fat_result=0,(_tmp659->f1).inline_call=0;_tmp659;});Cyc_Absyn_new_exp(_tmpD6B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD6C;});
# 3023
goto _LL0;}case 475U: _LL3B1: _LL3B2: {
# 3025
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 10),sizeof(union Cyc_YYSTYPE),11U));
# 3027
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 10),sizeof(struct Cyc_Yyltype),11U));
# 3025 "parse.y"
void*_tmp65A=({struct _tuple8*_tmpD6D=Cyc_yyget_YY37(yyyyvsp[8]);Cyc_Parse_type_name_to_type(_tmpD6D,(unsigned int)(yyyylsp[8]).first_line);});
({union Cyc_YYSTYPE _tmpD72=Cyc_Exp_tok(({void*_tmpD71=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp65C=_cycalloc(sizeof(*_tmp65C));_tmp65C->tag=34U,(_tmp65C->f1).is_calloc=1,({struct Cyc_Absyn_Exp*_tmpD70=Cyc_yyget_Exp_tok(yyyyvsp[2]);(_tmp65C->f1).rgn=_tmpD70;}),({void**_tmpD6F=({void**_tmp65B=_cycalloc(sizeof(*_tmp65B));*_tmp65B=_tmp65A;_tmp65B;});(_tmp65C->f1).elt_type=_tmpD6F;}),({struct Cyc_Absyn_Exp*_tmpD6E=Cyc_yyget_Exp_tok(yyyyvsp[4]);(_tmp65C->f1).num_elts=_tmpD6E;}),(_tmp65C->f1).fat_result=0,(_tmp65C->f1).inline_call=0;_tmp65C;});Cyc_Absyn_new_exp(_tmpD71,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD72;});
# 3028
goto _LL0;}case 476U: _LL3B3: _LL3B4: {
# 3030
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3032
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3029 "parse.y"
({union Cyc_YYSTYPE _tmpD75=Cyc_Exp_tok(({struct Cyc_List_List*_tmpD74=({struct Cyc_Absyn_Exp*_tmp65D[1U];({struct Cyc_Absyn_Exp*_tmpD73=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp65D[0]=_tmpD73;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp65D,sizeof(struct Cyc_Absyn_Exp*),1U));});Cyc_Absyn_primop_exp(Cyc_Absyn_Numelts,_tmpD74,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD75;});
goto _LL0;}case 477U: _LL3B5: _LL3B6: {
# 3032
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3034
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3031 "parse.y"
({union Cyc_YYSTYPE _tmpD7A=Cyc_Exp_tok(({void*_tmpD79=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp65F=_cycalloc(sizeof(*_tmp65F));_tmp65F->tag=38U,({struct Cyc_Absyn_Exp*_tmpD78=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp65F->f1=_tmpD78;}),({struct _dyneither_ptr*_tmpD77=({struct _dyneither_ptr*_tmp65E=_cycalloc(sizeof(*_tmp65E));({struct _dyneither_ptr _tmpD76=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp65E=_tmpD76;});_tmp65E;});_tmp65F->f2=_tmpD77;});_tmp65F;});Cyc_Absyn_new_exp(_tmpD79,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD7A;});
goto _LL0;}case 478U: _LL3B7: _LL3B8: {
# 3034
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3036
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3033 "parse.y"
({union Cyc_YYSTYPE _tmpD80=Cyc_Exp_tok(({void*_tmpD7F=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp661=_cycalloc(sizeof(*_tmp661));_tmp661->tag=38U,({struct Cyc_Absyn_Exp*_tmpD7E=({struct Cyc_Absyn_Exp*_tmpD7D=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_deref_exp(_tmpD7D,(unsigned int)(yyyylsp[2]).first_line);});_tmp661->f1=_tmpD7E;}),({struct _dyneither_ptr*_tmpD7C=({struct _dyneither_ptr*_tmp660=_cycalloc(sizeof(*_tmp660));({struct _dyneither_ptr _tmpD7B=Cyc_yyget_String_tok(yyyyvsp[4]);*_tmp660=_tmpD7B;});_tmp660;});_tmp661->f2=_tmpD7C;});_tmp661;});Cyc_Absyn_new_exp(_tmpD7F,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD80;});
goto _LL0;}case 479U: _LL3B9: _LL3BA: {
# 3036
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3038
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3035 "parse.y"
void*_tmp662=({struct _tuple8*_tmpD81=Cyc_yyget_YY37(yyyyvsp[2]);Cyc_Parse_type_name_to_type(_tmpD81,(unsigned int)(yyyylsp[2]).first_line);});
({union Cyc_YYSTYPE _tmpD82=Cyc_Exp_tok(Cyc_Absyn_valueof_exp(_tmp662,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD82;});
goto _LL0;}case 480U: _LL3BB: _LL3BC: {
# 3039
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3041
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3038 "parse.y"
struct _tuple20 _tmp663=Cyc_yyget_Asm_tok(yyyyvsp[0]);struct _tuple20 _tmp664=_tmp663;int _tmp666;struct _dyneither_ptr _tmp665;_LL48E: _tmp666=_tmp664.f1;_tmp665=_tmp664.f2;_LL48F:;
({union Cyc_YYSTYPE _tmpD83=Cyc_Exp_tok(Cyc_Absyn_asm_exp(_tmp666,_tmp665,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpD83;});
goto _LL0;}case 481U: _LL3BD: _LL3BE: {
# 3042
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3044
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3040 "parse.y"
({union Cyc_YYSTYPE _tmpD85=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD84=Cyc_yyget_Exp_tok(yyyyvsp[1]);Cyc_Absyn_extension_exp(_tmpD84,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD85;});
goto _LL0;}case 482U: _LL3BF: _LL3C0: {
# 3043
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3045
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3044 "parse.y"
({union Cyc_YYSTYPE _tmpD86=Cyc_YY6(Cyc_Absyn_Bitnot);yyval=_tmpD86;});
goto _LL0;}case 483U: _LL3C1: _LL3C2: {
# 3047
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3049
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3045 "parse.y"
({union Cyc_YYSTYPE _tmpD87=Cyc_YY6(Cyc_Absyn_Not);yyval=_tmpD87;});
goto _LL0;}case 484U: _LL3C3: _LL3C4: {
# 3048
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3050
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3046 "parse.y"
({union Cyc_YYSTYPE _tmpD88=Cyc_YY6(Cyc_Absyn_Minus);yyval=_tmpD88;});
goto _LL0;}case 485U: _LL3C5: _LL3C6: {
# 3049
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3051
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3051 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 486U: _LL3C7: _LL3C8: {
# 3054
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3056
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3053 "parse.y"
({union Cyc_YYSTYPE _tmpD8B=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD8A=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_Absyn_Exp*_tmpD89=Cyc_yyget_Exp_tok(yyyyvsp[2]);Cyc_Absyn_subscript_exp(_tmpD8A,_tmpD89,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD8B;});
goto _LL0;}case 487U: _LL3C9: _LL3CA: {
# 3056
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3058
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3055 "parse.y"
({union Cyc_YYSTYPE _tmpD8D=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD8C=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_unknowncall_exp(_tmpD8C,0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD8D;});
goto _LL0;}case 488U: _LL3CB: _LL3CC: {
# 3058
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3060
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3057 "parse.y"
({union Cyc_YYSTYPE _tmpD90=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD8F=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpD8E=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_unknowncall_exp(_tmpD8F,_tmpD8E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD90;});
goto _LL0;}case 489U: _LL3CD: _LL3CE: {
# 3060
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3062
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3059 "parse.y"
({union Cyc_YYSTYPE _tmpD94=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD93=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpD92=({struct _dyneither_ptr*_tmp667=_cycalloc(sizeof(*_tmp667));({struct _dyneither_ptr _tmpD91=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp667=_tmpD91;});_tmp667;});Cyc_Absyn_aggrmember_exp(_tmpD93,_tmpD92,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD94;});
goto _LL0;}case 490U: _LL3CF: _LL3D0: {
# 3062
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3064
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3061 "parse.y"
({union Cyc_YYSTYPE _tmpD98=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD97=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct _dyneither_ptr*_tmpD96=({struct _dyneither_ptr*_tmp668=_cycalloc(sizeof(*_tmp668));({struct _dyneither_ptr _tmpD95=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp668=_tmpD95;});_tmp668;});Cyc_Absyn_aggrarrow_exp(_tmpD97,_tmpD96,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD98;});
goto _LL0;}case 491U: _LL3D1: _LL3D2: {
# 3064
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3066
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3063 "parse.y"
({union Cyc_YYSTYPE _tmpD9A=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD99=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_inc_exp(_tmpD99,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9A;});
goto _LL0;}case 492U: _LL3D3: _LL3D4: {
# 3066
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3068
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3065 "parse.y"
({union Cyc_YYSTYPE _tmpD9C=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpD9B=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_post_dec_exp(_tmpD9B,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9C;});
goto _LL0;}case 493U: _LL3D5: _LL3D6: {
# 3068
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3070
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3067 "parse.y"
({union Cyc_YYSTYPE _tmpD9F=Cyc_Exp_tok(({void*_tmpD9E=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->tag=25U,({struct _tuple8*_tmpD9D=Cyc_yyget_YY37(yyyyvsp[1]);_tmp669->f1=_tmpD9D;}),_tmp669->f2=0;_tmp669;});Cyc_Absyn_new_exp(_tmpD9E,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpD9F;});
goto _LL0;}case 494U: _LL3D7: _LL3D8: {
# 3070
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 5),sizeof(union Cyc_YYSTYPE),6U));
# 3072
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 5),sizeof(struct Cyc_Yyltype),6U));
# 3069 "parse.y"
({union Cyc_YYSTYPE _tmpDA3=Cyc_Exp_tok(({void*_tmpDA2=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->tag=25U,({struct _tuple8*_tmpDA1=Cyc_yyget_YY37(yyyyvsp[1]);_tmp66A->f1=_tmpDA1;}),({struct Cyc_List_List*_tmpDA0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp66A->f2=_tmpDA0;});_tmp66A;});Cyc_Absyn_new_exp(_tmpDA2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA3;});
goto _LL0;}case 495U: _LL3D9: _LL3DA: {
# 3072
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 6),sizeof(union Cyc_YYSTYPE),7U));
# 3074
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 6),sizeof(struct Cyc_Yyltype),7U));
# 3071 "parse.y"
({union Cyc_YYSTYPE _tmpDA7=Cyc_Exp_tok(({void*_tmpDA6=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->tag=25U,({struct _tuple8*_tmpDA5=Cyc_yyget_YY37(yyyyvsp[1]);_tmp66B->f1=_tmpDA5;}),({struct Cyc_List_List*_tmpDA4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[4]));_tmp66B->f2=_tmpDA4;});_tmp66B;});Cyc_Absyn_new_exp(_tmpDA6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDA7;});
goto _LL0;}case 496U: _LL3DB: _LL3DC: {
# 3074
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3076
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3076 "parse.y"
({union Cyc_YYSTYPE _tmpDAB=Cyc_YY3(({struct Cyc_List_List*_tmp66E=_cycalloc(sizeof(*_tmp66E));({void*_tmpDAA=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->tag=0U,({struct _dyneither_ptr*_tmpDA9=({struct _dyneither_ptr*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct _dyneither_ptr _tmpDA8=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp66C=_tmpDA8;});_tmp66C;});_tmp66D->f1=_tmpDA9;});_tmp66D;});_tmp66E->hd=_tmpDAA;}),_tmp66E->tl=0;_tmp66E;}));yyval=_tmpDAB;});
goto _LL0;}case 497U: _LL3DD: _LL3DE: {
# 3079
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3081
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3079 "parse.y"
({union Cyc_YYSTYPE _tmpDAF=Cyc_YY3(({struct Cyc_List_List*_tmp670=_cycalloc(sizeof(*_tmp670));({void*_tmpDAE=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->tag=1U,({unsigned int _tmpDAD=({unsigned int _tmpDAC=(unsigned int)(yyyylsp[0]).first_line;Cyc_Parse_cnst2uint(_tmpDAC,Cyc_yyget_Int_tok(yyyyvsp[0]));});_tmp66F->f1=_tmpDAD;});_tmp66F;});_tmp670->hd=_tmpDAE;}),_tmp670->tl=0;_tmp670;}));yyval=_tmpDAF;});
goto _LL0;}case 498U: _LL3DF: _LL3E0: {
# 3082
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3084
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3081 "parse.y"
({union Cyc_YYSTYPE _tmpDB4=Cyc_YY3(({struct Cyc_List_List*_tmp673=_cycalloc(sizeof(*_tmp673));({void*_tmpDB3=(void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp672=_cycalloc(sizeof(*_tmp672));_tmp672->tag=0U,({struct _dyneither_ptr*_tmpDB2=({struct _dyneither_ptr*_tmp671=_cycalloc(sizeof(*_tmp671));({struct _dyneither_ptr _tmpDB1=Cyc_yyget_String_tok(yyyyvsp[2]);*_tmp671=_tmpDB1;});_tmp671;});_tmp672->f1=_tmpDB2;});_tmp672;});_tmp673->hd=_tmpDB3;}),({struct Cyc_List_List*_tmpDB0=Cyc_yyget_YY3(yyyyvsp[0]);_tmp673->tl=_tmpDB0;});_tmp673;}));yyval=_tmpDB4;});
goto _LL0;}case 499U: _LL3E1: _LL3E2: {
# 3084
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3086
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3084 "parse.y"
({union Cyc_YYSTYPE _tmpDB9=Cyc_YY3(({struct Cyc_List_List*_tmp675=_cycalloc(sizeof(*_tmp675));({void*_tmpDB8=(void*)({struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->tag=1U,({unsigned int _tmpDB7=({unsigned int _tmpDB6=(unsigned int)(yyyylsp[2]).first_line;Cyc_Parse_cnst2uint(_tmpDB6,Cyc_yyget_Int_tok(yyyyvsp[2]));});_tmp674->f1=_tmpDB7;});_tmp674;});_tmp675->hd=_tmpDB8;}),({struct Cyc_List_List*_tmpDB5=Cyc_yyget_YY3(yyyyvsp[0]);_tmp675->tl=_tmpDB5;});_tmp675;}));yyval=_tmpDB9;});
goto _LL0;}case 500U: _LL3E3: _LL3E4: {
# 3087
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3089
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpDBB=Cyc_Exp_tok(({struct _tuple0*_tmpDBA=Cyc_yyget_QualId_tok(yyyyvsp[0]);Cyc_Absyn_unknownid_exp(_tmpDBA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBB;});
goto _LL0;}case 501U: _LL3E5: _LL3E6: {
# 3093
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3095
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3092 "parse.y"
({union Cyc_YYSTYPE _tmpDBD=Cyc_Exp_tok(({struct _dyneither_ptr _tmpDBC=Cyc_yyget_String_tok(yyyyvsp[2]);Cyc_Absyn_pragma_exp(_tmpDBC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBD;});
goto _LL0;}case 502U: _LL3E7: _LL3E8: {
# 3095
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3097
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3094 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 503U: _LL3E9: _LL3EA: {
# 3097
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3099
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3096 "parse.y"
({union Cyc_YYSTYPE _tmpDBF=Cyc_Exp_tok(({struct _dyneither_ptr _tmpDBE=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_string_exp(_tmpDBE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDBF;});
goto _LL0;}case 504U: _LL3EB: _LL3EC: {
# 3099
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3101
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3098 "parse.y"
({union Cyc_YYSTYPE _tmpDC1=Cyc_Exp_tok(({struct _dyneither_ptr _tmpDC0=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wstring_exp(_tmpDC0,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC1;});
goto _LL0;}case 505U: _LL3ED: _LL3EE: {
# 3101
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3103
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3100 "parse.y"
yyval=yyyyvsp[1];
goto _LL0;}case 506U: _LL3EF: _LL3F0: {
# 3103
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 1),sizeof(union Cyc_YYSTYPE),2U));
# 3105
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 1),sizeof(struct Cyc_Yyltype),2U));
# 3105 "parse.y"
({union Cyc_YYSTYPE _tmpDC3=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDC2=Cyc_yyget_Exp_tok(yyyyvsp[0]);Cyc_Absyn_noinstantiate_exp(_tmpDC2,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC3;});
goto _LL0;}case 507U: _LL3F1: _LL3F2: {
# 3108
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3110
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3107 "parse.y"
({union Cyc_YYSTYPE _tmpDC6=Cyc_Exp_tok(({struct Cyc_Absyn_Exp*_tmpDC5=Cyc_yyget_Exp_tok(yyyyvsp[0]);struct Cyc_List_List*_tmpDC4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY40(yyyyvsp[3]));Cyc_Absyn_instantiate_exp(_tmpDC5,_tmpDC4,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC6;});
goto _LL0;}case 508U: _LL3F3: _LL3F4: {
# 3110
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 3),sizeof(union Cyc_YYSTYPE),4U));
# 3112
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 3),sizeof(struct Cyc_Yyltype),4U));
# 3110 "parse.y"
({union Cyc_YYSTYPE _tmpDC8=Cyc_Exp_tok(({struct Cyc_List_List*_tmpDC7=Cyc_yyget_YY4(yyyyvsp[2]);Cyc_Absyn_tuple_exp(_tmpDC7,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDC8;});
goto _LL0;}case 509U: _LL3F5: _LL3F6: {
# 3113
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3115
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3113 "parse.y"
({union Cyc_YYSTYPE _tmpDCD=Cyc_Exp_tok(({void*_tmpDCC=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp676=_cycalloc(sizeof(*_tmp676));_tmp676->tag=29U,({struct _tuple0*_tmpDCB=Cyc_yyget_QualId_tok(yyyyvsp[0]);_tmp676->f1=_tmpDCB;}),({struct Cyc_List_List*_tmpDCA=Cyc_yyget_YY40(yyyyvsp[2]);_tmp676->f2=_tmpDCA;}),({struct Cyc_List_List*_tmpDC9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY5(yyyyvsp[3]));_tmp676->f3=_tmpDC9;}),_tmp676->f4=0;_tmp676;});Cyc_Absyn_new_exp(_tmpDCC,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDCD;});
goto _LL0;}case 510U: _LL3F7: _LL3F8: {
# 3116
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 4),sizeof(union Cyc_YYSTYPE),5U));
# 3118
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 4),sizeof(struct Cyc_Yyltype),5U));
# 3116 "parse.y"
({union Cyc_YYSTYPE _tmpDCF=Cyc_Exp_tok(({struct Cyc_Absyn_Stmt*_tmpDCE=Cyc_yyget_Stmt_tok(yyyyvsp[2]);Cyc_Absyn_stmt_exp(_tmpDCE,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDCF;});
goto _LL0;}case 511U: _LL3F9: _LL3FA: {
# 3119
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3121
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3120 "parse.y"
({union Cyc_YYSTYPE _tmpDD0=Cyc_YY4(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_yyget_YY4(yyyyvsp[0])));yyval=_tmpDD0;});
goto _LL0;}case 512U: _LL3FB: _LL3FC: {
# 3123
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3125
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpDD2=Cyc_YY4(({struct Cyc_List_List*_tmp677=_cycalloc(sizeof(*_tmp677));({struct Cyc_Absyn_Exp*_tmpDD1=Cyc_yyget_Exp_tok(yyyyvsp[0]);_tmp677->hd=_tmpDD1;}),_tmp677->tl=0;_tmp677;}));yyval=_tmpDD2;});
goto _LL0;}case 513U: _LL3FD: _LL3FE: {
# 3129
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 2),sizeof(union Cyc_YYSTYPE),3U));
# 3131
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 2),sizeof(struct Cyc_Yyltype),3U));
# 3128 "parse.y"
({union Cyc_YYSTYPE _tmpDD5=Cyc_YY4(({struct Cyc_List_List*_tmp678=_cycalloc(sizeof(*_tmp678));({struct Cyc_Absyn_Exp*_tmpDD4=Cyc_yyget_Exp_tok(yyyyvsp[2]);_tmp678->hd=_tmpDD4;}),({struct Cyc_List_List*_tmpDD3=Cyc_yyget_YY4(yyyyvsp[0]);_tmp678->tl=_tmpDD3;});_tmp678;}));yyval=_tmpDD5;});
goto _LL0;}case 514U: _LL3FF: _LL400: {
# 3131
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3133
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
({union Cyc_YYSTYPE _tmpDD7=Cyc_Exp_tok(({union Cyc_Absyn_Cnst _tmpDD6=Cyc_yyget_Int_tok(yyyyvsp[0]);Cyc_Absyn_const_exp(_tmpDD6,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDD7;});
goto _LL0;}case 515U: _LL401: _LL402: {
# 3137
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3139
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3135 "parse.y"
({union Cyc_YYSTYPE _tmpDD9=Cyc_Exp_tok(({char _tmpDD8=Cyc_yyget_Char_tok(yyyyvsp[0]);Cyc_Absyn_char_exp(_tmpDD8,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDD9;});
goto _LL0;}case 516U: _LL403: _LL404: {
# 3138
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3140
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3136 "parse.y"
({union Cyc_YYSTYPE _tmpDDB=Cyc_Exp_tok(({struct _dyneither_ptr _tmpDDA=Cyc_yyget_String_tok(yyyyvsp[0]);Cyc_Absyn_wchar_exp(_tmpDDA,(unsigned int)(yyyylsp[0]).first_line);}));yyval=_tmpDDB;});
goto _LL0;}case 517U: _LL405: _LL406: {
# 3139
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3141
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3138 "parse.y"
struct _dyneither_ptr _tmp679=Cyc_yyget_String_tok(yyyyvsp[0]);
int l=(int)Cyc_strlen((struct _dyneither_ptr)_tmp679);
int i=1;
if(l > 0){
char c=*((const char*)_check_dyneither_subscript(_tmp679,sizeof(char),l - 1));
if(c == 'f'  || c == 'F')i=0;else{
if(c == 'l'  || c == 'L')i=2;}}
# 3146
({union Cyc_YYSTYPE _tmpDDC=Cyc_Exp_tok(Cyc_Absyn_float_exp(_tmp679,i,(unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDC;});
# 3148
goto _LL0;}case 518U: _LL407: _LL408: {
# 3150
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3152
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3149 "parse.y"
({union Cyc_YYSTYPE _tmpDDD=Cyc_Exp_tok(Cyc_Absyn_null_exp((unsigned int)(yyyylsp[0]).first_line));yyval=_tmpDDD;});
goto _LL0;}case 519U: _LL409: _LL40A: {
# 3152
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3154
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3153 "parse.y"
({union Cyc_YYSTYPE _tmpDE1=Cyc_QualId_tok(({struct _tuple0*_tmp67B=_cycalloc(sizeof(*_tmp67B));({union Cyc_Absyn_Nmspace _tmpDE0=Cyc_Absyn_Rel_n(0);_tmp67B->f1=_tmpDE0;}),({struct _dyneither_ptr*_tmpDDF=({struct _dyneither_ptr*_tmp67A=_cycalloc(sizeof(*_tmp67A));({struct _dyneither_ptr _tmpDDE=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp67A=_tmpDDE;});_tmp67A;});_tmp67B->f2=_tmpDDF;});_tmp67B;}));yyval=_tmpDE1;});
goto _LL0;}case 520U: _LL40B: _LL40C: {
# 3156
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3158
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3154 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 521U: _LL40D: _LL40E: {
# 3157
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3159
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3157 "parse.y"
({union Cyc_YYSTYPE _tmpDE5=Cyc_QualId_tok(({struct _tuple0*_tmp67D=_cycalloc(sizeof(*_tmp67D));({union Cyc_Absyn_Nmspace _tmpDE4=Cyc_Absyn_Rel_n(0);_tmp67D->f1=_tmpDE4;}),({struct _dyneither_ptr*_tmpDE3=({struct _dyneither_ptr*_tmp67C=_cycalloc(sizeof(*_tmp67C));({struct _dyneither_ptr _tmpDE2=Cyc_yyget_String_tok(yyyyvsp[0]);*_tmp67C=_tmpDE2;});_tmp67C;});_tmp67D->f2=_tmpDE3;});_tmp67D;}));yyval=_tmpDE5;});
goto _LL0;}case 522U: _LL40F: _LL410: {
# 3160
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3162
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3158 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 523U: _LL411: _LL412: {
# 3161
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3163
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3163 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 524U: _LL413: _LL414: {
# 3166
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3168
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3164 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 525U: _LL415: _LL416: {
# 3167
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3169
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3167 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 526U: _LL417: _LL418: {
# 3170
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3172
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3168 "parse.y"
yyval=yyyyvsp[0];
goto _LL0;}case 527U: _LL419: _LL41A: {
# 3171
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3173
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3173 "parse.y"
goto _LL0;}case 528U: _LL41B: _LL41C: {
# 3175
union Cyc_YYSTYPE*yyyyvsp=(union Cyc_YYSTYPE*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyvs,sizeof(union Cyc_YYSTYPE),yyvsp_offset - 0),sizeof(union Cyc_YYSTYPE),1U));
# 3177
struct Cyc_Yyltype*yyyylsp=(struct Cyc_Yyltype*)_check_null(_untag_dyneither_ptr(_dyneither_ptr_plus(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset - 0),sizeof(struct Cyc_Yyltype),1U));
# 3173 "parse.y"
yylex_buf->lex_curr_pos -=1;
goto _LL0;}default: _LL41D: _LL41E:
# 3178
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
({int _tmpDE6=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_line;(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),yylsp_offset))).last_line=_tmpDE6;});
(((struct Cyc_Yyltype*)yyls.curr)[yylsp_offset]).last_column=(*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),(yylsp_offset + yylen)- 1))).last_column;}
# 415
yyn=(int)Cyc_yyr1[yyn];
# 417
({int _tmpDE8=({short _tmpDE7=Cyc_yypgoto[_check_known_subscript_notnull(147U,yyn - 148)];_tmpDE7 + *((short*)_check_dyneither_subscript(yyss,sizeof(short),yyssp_offset));});yystate=_tmpDE8;});
if((yystate >= 0  && yystate <= 7665) && Cyc_yycheck[_check_known_subscript_notnull(7666U,yystate)]== ((short*)yyss.curr)[yyssp_offset])
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
if(yyn > - 32768  && yyn < 7665){
# 437
int sze=0;
struct _dyneither_ptr msg;
int x;int count;
# 441
count=0;
# 443
for(x=yyn < 0?- yyn: 0;x < 295U / sizeof(char*);++ x){
# 445
if(Cyc_yycheck[_check_known_subscript_notnull(7666U,x + yyn)]== x)
({unsigned long _tmpDE9=Cyc_strlen((struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(295U,x)])+ 15;sze +=_tmpDE9;}),count ++;}
({struct _dyneither_ptr _tmpDEB=({unsigned int _tmp67F=(unsigned int)(sze + 15)+ 1U;char*_tmp67E=({struct _RegionHandle*_tmpDEA=yyregion;_region_malloc(_tmpDEA,_check_times(_tmp67F,sizeof(char)));});({{unsigned int _tmp73A=(unsigned int)(sze + 15);unsigned int i;for(i=0;i < _tmp73A;++ i){_tmp67E[i]='\000';}_tmp67E[_tmp73A]=0;}0;});_tag_dyneither(_tmp67E,sizeof(char),_tmp67F);});msg=_tmpDEB;});
({struct _dyneither_ptr _tmpDEC=msg;Cyc_strcpy(_tmpDEC,({const char*_tmp680="parse error";_tag_dyneither(_tmp680,sizeof(char),12U);}));});
# 450
if(count < 5){
# 452
count=0;
for(x=yyn < 0?- yyn: 0;x < 295U / sizeof(char*);++ x){
# 455
if(Cyc_yycheck[_check_known_subscript_notnull(7666U,x + yyn)]== x){
# 457
({struct _dyneither_ptr _tmpDED=msg;Cyc_strcat(_tmpDED,(struct _dyneither_ptr)(count == 0?(struct _dyneither_ptr)({const char*_tmp681=", expecting `";_tag_dyneither(_tmp681,sizeof(char),14U);}):(struct _dyneither_ptr)({const char*_tmp682=" or `";_tag_dyneither(_tmp682,sizeof(char),6U);})));});
# 460
Cyc_strcat(msg,(struct _dyneither_ptr)Cyc_yytname[_check_known_subscript_notnull(295U,x)]);
({struct _dyneither_ptr _tmpDEE=msg;Cyc_strcat(_tmpDEE,({const char*_tmp683="'";_tag_dyneither(_tmp683,sizeof(char),2U);}));});
++ count;}}}
# 465
Cyc_yyerror((struct _dyneither_ptr)msg,yystate,yychar);}else{
# 469
({struct _dyneither_ptr _tmpDF0=({const char*_tmp684="parse error";_tag_dyneither(_tmp684,sizeof(char),12U);});int _tmpDEF=yystate;Cyc_yyerror(_tmpDF0,_tmpDEF,yychar);});}}
# 471
goto yyerrlab1;
# 473
yyerrlab1:
# 475
 if(yyerrstatus == 3){
# 480
if(yychar == 0){
int _tmp685=1;_npop_handler(0U);return _tmp685;}
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
 if(yyssp_offset == 0){int _tmp686=1;_npop_handler(0U);return _tmp686;}
-- yyvsp_offset;
yystate=(int)*((short*)_check_dyneither_subscript(yyss,sizeof(short),-- yyssp_offset));
# 515
-- yylsp_offset;
# 530 "cycbison.simple"
yyerrhandle:
 yyn=(int)Cyc_yypact[_check_known_subscript_notnull(1079U,yystate)];
if(yyn == - 32768)goto yyerrdefault;
# 534
yyn +=1;
if((yyn < 0  || yyn > 7665) || Cyc_yycheck[_check_known_subscript_notnull(7666U,yyn)]!= 1)goto yyerrdefault;
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
if(yyn == 1078){
int _tmp687=0;_npop_handler(0U);return _tmp687;}
# 554
*((union Cyc_YYSTYPE*)_check_dyneither_subscript(yyvs,sizeof(union Cyc_YYSTYPE),++ yyvsp_offset))=yylval;
# 556
*((struct Cyc_Yyltype*)_check_dyneither_subscript(yyls,sizeof(struct Cyc_Yyltype),++ yylsp_offset))=yylloc;
# 559
goto yynewstate;}
# 141 "cycbison.simple"
;_pop_region(yyregion);}
# 3176 "parse.y"
void Cyc_yyprint(int i,union Cyc_YYSTYPE v){
union Cyc_YYSTYPE _tmp68E=v;struct Cyc_Absyn_Stmt*_tmp6A8;struct Cyc_Absyn_Exp*_tmp6A7;struct _tuple0*_tmp6A6;struct _dyneither_ptr _tmp6A5;char _tmp6A4;union Cyc_Absyn_Cnst _tmp6A3;switch((_tmp68E.Stmt_tok).tag){case 1U: _LL1: _tmp6A3=(_tmp68E.Int_tok).val;_LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp691=({struct Cyc_String_pa_PrintArg_struct _tmp73E;_tmp73E.tag=0U,({struct _dyneither_ptr _tmpDF1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_cnst2string(_tmp6A3));_tmp73E.f1=_tmpDF1;});_tmp73E;});void*_tmp68F[1U];_tmp68F[0]=& _tmp691;({struct Cyc___cycFILE*_tmpDF3=Cyc_stderr;struct _dyneither_ptr _tmpDF2=({const char*_tmp690="%s";_tag_dyneither(_tmp690,sizeof(char),3U);});Cyc_fprintf(_tmpDF3,_tmpDF2,_tag_dyneither(_tmp68F,sizeof(void*),1U));});});goto _LL0;case 2U: _LL3: _tmp6A4=(_tmp68E.Char_tok).val;_LL4:
({struct Cyc_Int_pa_PrintArg_struct _tmp694=({struct Cyc_Int_pa_PrintArg_struct _tmp73F;_tmp73F.tag=1U,_tmp73F.f1=(unsigned long)((int)_tmp6A4);_tmp73F;});void*_tmp692[1U];_tmp692[0]=& _tmp694;({struct Cyc___cycFILE*_tmpDF5=Cyc_stderr;struct _dyneither_ptr _tmpDF4=({const char*_tmp693="%c";_tag_dyneither(_tmp693,sizeof(char),3U);});Cyc_fprintf(_tmpDF5,_tmpDF4,_tag_dyneither(_tmp692,sizeof(void*),1U));});});goto _LL0;case 3U: _LL5: _tmp6A5=(_tmp68E.String_tok).val;_LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp697=({struct Cyc_String_pa_PrintArg_struct _tmp740;_tmp740.tag=0U,_tmp740.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp6A5);_tmp740;});void*_tmp695[1U];_tmp695[0]=& _tmp697;({struct Cyc___cycFILE*_tmpDF7=Cyc_stderr;struct _dyneither_ptr _tmpDF6=({const char*_tmp696="\"%s\"";_tag_dyneither(_tmp696,sizeof(char),5U);});Cyc_fprintf(_tmpDF7,_tmpDF6,_tag_dyneither(_tmp695,sizeof(void*),1U));});});goto _LL0;case 5U: _LL7: _tmp6A6=(_tmp68E.QualId_tok).val;_LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp69A=({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0U,({struct _dyneither_ptr _tmpDF8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6A6));_tmp741.f1=_tmpDF8;});_tmp741;});void*_tmp698[1U];_tmp698[0]=& _tmp69A;({struct Cyc___cycFILE*_tmpDFA=Cyc_stderr;struct _dyneither_ptr _tmpDF9=({const char*_tmp699="%s";_tag_dyneither(_tmp699,sizeof(char),3U);});Cyc_fprintf(_tmpDFA,_tmpDF9,_tag_dyneither(_tmp698,sizeof(void*),1U));});});goto _LL0;case 7U: _LL9: _tmp6A7=(_tmp68E.Exp_tok).val;_LLA:
({struct Cyc_String_pa_PrintArg_struct _tmp69D=({struct Cyc_String_pa_PrintArg_struct _tmp742;_tmp742.tag=0U,({struct _dyneither_ptr _tmpDFB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp6A7));_tmp742.f1=_tmpDFB;});_tmp742;});void*_tmp69B[1U];_tmp69B[0]=& _tmp69D;({struct Cyc___cycFILE*_tmpDFD=Cyc_stderr;struct _dyneither_ptr _tmpDFC=({const char*_tmp69C="%s";_tag_dyneither(_tmp69C,sizeof(char),3U);});Cyc_fprintf(_tmpDFD,_tmpDFC,_tag_dyneither(_tmp69B,sizeof(void*),1U));});});goto _LL0;case 8U: _LLB: _tmp6A8=(_tmp68E.Stmt_tok).val;_LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp6A0=({struct Cyc_String_pa_PrintArg_struct _tmp743;_tmp743.tag=0U,({struct _dyneither_ptr _tmpDFE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(_tmp6A8));_tmp743.f1=_tmpDFE;});_tmp743;});void*_tmp69E[1U];_tmp69E[0]=& _tmp6A0;({struct Cyc___cycFILE*_tmpE00=Cyc_stderr;struct _dyneither_ptr _tmpDFF=({const char*_tmp69F="%s";_tag_dyneither(_tmp69F,sizeof(char),3U);});Cyc_fprintf(_tmpE00,_tmpDFF,_tag_dyneither(_tmp69E,sizeof(void*),1U));});});goto _LL0;default: _LLD: _LLE:
({void*_tmp6A1=0U;({struct Cyc___cycFILE*_tmpE02=Cyc_stderr;struct _dyneither_ptr _tmpE01=({const char*_tmp6A2="?";_tag_dyneither(_tmp6A2,sizeof(char),2U);});Cyc_fprintf(_tmpE02,_tmpE01,_tag_dyneither(_tmp6A1,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 3188
struct _dyneither_ptr Cyc_token2string(int token){
if(token <= 0)
return({const char*_tmp6A9="end-of-file";_tag_dyneither(_tmp6A9,sizeof(char),12U);});
# 3192
if(token == 363)
return Cyc_Lex_token_string;else{
if(token == 372)
return Cyc_Absynpp_qvar2string(Cyc_Lex_token_qvar);}{
int z=token > 0  && token <= 375?(int)Cyc_yytranslate[_check_known_subscript_notnull(376U,token)]: 295;
if((unsigned int)z < 295U)
return Cyc_yytname[z];else{
return _tag_dyneither(0,0,0);}};}
# 3203
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f){
Cyc_Parse_parse_result=0;{
struct _RegionHandle _tmp6AA=_new_region("yyr");struct _RegionHandle*yyr=& _tmp6AA;_push_region(yyr);
({struct _RegionHandle*_tmpE03=yyr;Cyc_yyparse(_tmpE03,Cyc_Lexing_from_file(f));});{
struct Cyc_List_List*_tmp6AB=Cyc_Parse_parse_result;_npop_handler(0U);return _tmp6AB;};
# 3206
;_pop_region(yyr);};}
