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

# 69 "core.h"
 void exit(int);
# 38 "cycboot.h"
int Cyc_open(const char*,int,struct _dyneither_ptr);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);
# 106
int Cyc_fputs(const char*,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 310
int toupper(int);
# 318
int system(const char*);
# 321
int mkdir(const char*pathname,unsigned short mode);
# 324
int close(int);
int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 277 "core.h"
void Cyc_Core_rethrow(void*);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 80 "lexing.h"
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*);
# 84
struct _dyneither_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*);
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*,int);
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*);
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 51 "set.h"
struct Cyc_Set_Set*Cyc_Set_empty(int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 75
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 82
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);
# 85
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
# 103 "string.h"
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
# 108
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 78
int Cyc_Hashtable_hash_stringptr(struct _dyneither_ptr*p);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 66 "arg.h"
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);struct Cyc_Buffer_t;
# 49 "buffer.h"
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 57
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 70
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 81 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1107 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1158
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1170
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1178
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
struct _dyneither_ptr Cyc_Absynpp_decllist2string(struct Cyc_List_List*tdl);
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};struct Cyc_Declarator{struct _tuple1*id;struct Cyc_List_List*tms;};struct _tuple13{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple12{struct _tuple12*tl;struct _tuple13 hd  __attribute__((aligned )) ;};
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0U,Cyc_Extern_sc  = 1U,Cyc_ExternC_sc  = 2U,Cyc_Static_sc  = 3U,Cyc_Auto_sc  = 4U,Cyc_Register_sc  = 5U,Cyc_Abstract_sc  = 6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple14{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple14 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple15{unsigned int f1;union Cyc_Absyn_Constraint*f2;union Cyc_Absyn_Constraint*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY2{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple16{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple13 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple18{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple19 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple9*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;union Cyc_Absyn_Constraint*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 66 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 33 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 49
void Cyc_Toc_finish();
# 147 "tcutil.h"
int Cyc_Tcutil_unify(void*,void*);
# 30 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple1*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 29 "specsfile.h"
struct _dyneither_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _dyneither_ptr s);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _dyneither_ptr file);
struct _dyneither_ptr Cyc_Specsfile_split_specs(struct _dyneither_ptr cmdline);
struct _dyneither_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _dyneither_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _dyneither_ptr Cyc_Specsfile_def_lib_path;
struct _dyneither_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr argv);
# 44
struct _dyneither_ptr Cyc_Specsfile_find_in_arch_path(struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_Specsfile_find_in_exec_path(struct _dyneither_ptr s);
# 78 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);static char _tmp0[4U]="gcc";
# 88
static struct _dyneither_ptr Cyc_cyclone_cc={_tmp0,_tmp0,_tmp0 + 4U};static char _tmp1[1U]="";
static struct _dyneither_ptr Cyc_target_cflags={_tmp1,_tmp1,_tmp1 + 1U};
# 91
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
int Cyc_noexpand_r=1;
# 95
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 99
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 102
if(Cyc_log_file == 0){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp4E8=Cyc_stderr;struct _dyneither_ptr _tmp4E7=({const char*_tmp3="Internal error: log file is NULL\n";_tag_dyneither(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp4E8,_tmp4E7,_tag_dyneither(_tmp2,sizeof(void*),0U));});});
 exit(1);}{
# 106
int _tmp4=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return _tmp4;};}
# 111
static struct _dyneither_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _dyneither_ptr*sptr){
({struct Cyc_Set_Set**_tmp4EA=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp4E9=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);*_tmp5=_tmp4E9;});_tmp5;});Cyc_current_targets=_tmp4EA;});}struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 119
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 125
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple22{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 133
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple23{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct _tuple24{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct _tuple25{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;struct Cyc_List_List*f8;};
# 149
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*);
int Cyc_commands(struct Cyc_Lexing_lexbuf*);
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*);
int Cyc_block(struct Cyc_Lexing_lexbuf*);
int Cyc_block_string(struct Cyc_Lexing_lexbuf*);
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_headerfile={(void*)0,(void*)0,(void*)(0 + 0)};
struct Cyc_List_List*Cyc_snarfed_symbols=0;
struct Cyc_List_List*Cyc_current_symbols=0;
struct Cyc_List_List*Cyc_current_user_defs=0;
struct Cyc_List_List*Cyc_current_cstubs=0;
struct Cyc_List_List*Cyc_current_cycstubs=0;
struct Cyc_List_List*Cyc_current_hstubs=0;
struct Cyc_List_List*Cyc_current_omit_symbols=0;
struct Cyc_List_List*Cyc_current_cpp=0;
struct Cyc_Buffer_t*Cyc_specbuf=0;
struct _dyneither_ptr Cyc_current_symbol={(void*)0,(void*)0,(void*)(0 + 0)};
int Cyc_rename_current_symbol=0;
int Cyc_braces_to_match=0;
int Cyc_parens_to_match=0;
# 170
int Cyc_numdef=0;
# 172
static struct Cyc_List_List*Cyc_cppargs=0;static char _tmp6[14U]="BUILDLIB_sym_";
# 174
struct _dyneither_ptr Cyc_user_prefix={_tmp6,_tmp6,_tmp6 + 14U};
static struct _dyneither_ptr*Cyc_add_user_prefix(struct _dyneither_ptr*symbol){
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_user_prefix,(struct _dyneither_ptr)*symbol);
return({struct _dyneither_ptr*_tmp7=_cycalloc(sizeof(*_tmp7));*_tmp7=s;_tmp7;});}
# 179
static struct _dyneither_ptr Cyc_remove_user_prefix(struct _dyneither_ptr symbol){
unsigned int prefix_len=Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix);
if(Cyc_strncmp((struct _dyneither_ptr)symbol,(struct _dyneither_ptr)Cyc_user_prefix,prefix_len)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp460;_tmp460.tag=0U,_tmp460.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)symbol);_tmp460;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp4EC=Cyc_stderr;struct _dyneither_ptr _tmp4EB=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_dyneither(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp4EC,_tmp4EB,_tag_dyneither(_tmp8,sizeof(void*),1U));});});
return symbol;}
# 185
return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp4EF=(struct _dyneither_ptr)symbol;int _tmp4EE=(int)prefix_len;Cyc_substring(_tmp4EF,_tmp4EE,({unsigned long _tmp4ED=Cyc_strlen((struct _dyneither_ptr)symbol);_tmp4ED - prefix_len;}));});}
# 188
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_tmpC=_tmpB;struct Cyc_Absyn_Typedefdecl*_tmp14;struct Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp12;switch(*((int*)_tmpC)){case 5U: _LL1: _tmp12=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2:
# 191
({struct _dyneither_ptr*_tmp4F1=({struct _dyneither_ptr*_tmpD=_cycalloc(sizeof(*_tmpD));({struct _dyneither_ptr _tmp4F0=Cyc_remove_user_prefix(*(*_tmp12->name).f2);*_tmpD=_tmp4F0;});_tmpD;});(*_tmp12->name).f2=_tmp4F1;});
goto _LL0;case 7U: _LL3: _tmp13=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4:
# 194
({struct _dyneither_ptr*_tmp4F3=({struct _dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _dyneither_ptr _tmp4F2=Cyc_remove_user_prefix(*(*_tmp13->name).f2);*_tmpE=_tmp4F2;});_tmpE;});(*_tmp13->name).f2=_tmp4F3;});
goto _LL0;case 8U: _LL5: _tmp14=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6:
# 197
({struct _dyneither_ptr*_tmp4F5=({struct _dyneither_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _dyneither_ptr _tmp4F4=Cyc_remove_user_prefix(*(*_tmp14->name).f2);*_tmpF=_tmp4F4;});_tmpF;});(*_tmp14->name).f2=_tmp4F5;});
goto _LL0;case 1U: _LL7: _LL8:
# 200
 goto _LLA;case 0U: _LL9: _LLA:
 goto _LLC;case 4U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 2U: _LLF: _LL10:
 goto _LL12;case 3U: _LL11: _LL12:
 goto _LL14;case 9U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
 goto _LL18;case 11U: _LL17: _LL18:
 goto _LL1A;case 12U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 212
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmp4F7=Cyc_stderr;struct _dyneither_ptr _tmp4F6=({const char*_tmp11="Error in .cys file: bad user-defined type definition\n";_tag_dyneither(_tmp11,sizeof(char),54U);});Cyc_fprintf(_tmp4F7,_tmp4F6,_tag_dyneither(_tmp10,sizeof(void*),0U));});});
 exit(1);}_LL0:;}
# 218
static struct Cyc_Absyn_Decl*Cyc_named_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*decl_name){
struct _dyneither_ptr*name;
for(0;decls != 0;decls=decls->tl){
{void*_tmp15=((struct Cyc_Absyn_Decl*)decls->hd)->r;void*_tmp16=_tmp15;struct Cyc_Absyn_Typedefdecl*_tmp22;struct Cyc_Absyn_Enumdecl*_tmp21;struct Cyc_Absyn_Aggrdecl*_tmp20;switch(*((int*)_tmp16)){case 5U: _LL1: _tmp20=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL2: {
# 223
struct _tuple1*_tmp17=_tmp20->name;struct _tuple1*_tmp18=_tmp17;struct _dyneither_ptr*_tmp19;_LL20: _tmp19=_tmp18->f2;_LL21:;
name=_tmp19;
goto _LL0;}case 7U: _LL3: _tmp21=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL4: {
# 227
struct _tuple1*_tmp1A=_tmp21->name;struct _tuple1*_tmp1B=_tmp1A;struct _dyneither_ptr*_tmp1C;_LL23: _tmp1C=_tmp1B->f2;_LL24:;
name=_tmp1C;
goto _LL0;}case 8U: _LL5: _tmp22=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL6: {
# 231
struct _tuple1*_tmp1D=_tmp22->name;struct _tuple1*_tmp1E=_tmp1D;struct _dyneither_ptr*_tmp1F;_LL26: _tmp1F=_tmp1E->f2;_LL27:;
name=_tmp1F;
goto _LL0;}case 0U: _LL7: _LL8:
# 235
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LLC;case 13U: _LLB: _LLC:
 goto _LLE;case 14U: _LLD: _LLE:
 goto _LL10;case 2U: _LLF: _LL10:
 goto _LL12;case 6U: _LL11: _LL12:
 goto _LL14;case 3U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
 goto _LL18;case 10U: _LL17: _LL18:
 goto _LL1A;case 11U: _LL19: _LL1A:
 goto _LL1C;case 12U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 247
 name=0;
goto _LL0;}_LL0:;}
# 250
if((unsigned int)name  && !Cyc_strptrcmp(name,decl_name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 253
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp4F8=({const char*_tmp23="Internal Error: bad user-def name";_tag_dyneither(_tmp23,sizeof(char),34U);});_tmp24->f1=_tmp4F8;});_tmp24;}));}struct _tuple26{void*f1;void*f2;};struct _tuple27{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 260
static void Cyc_merge_rep_decl(struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
{struct _tuple26 _tmp25=({struct _tuple26 _tmp48B;_tmp48B.f1=c_decl->r,_tmp48B.f2=user_decl->r;_tmp48B;});struct _tuple26 _tmp26=_tmp25;struct Cyc_Absyn_Typedefdecl*_tmp9E;struct Cyc_Absyn_Typedefdecl*_tmp9D;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_Absyn_Enumdecl*_tmp9B;struct Cyc_Absyn_Aggrdecl*_tmp9A;struct Cyc_Absyn_Aggrdecl*_tmp99;switch(*((int*)_tmp26.f1)){case 5U: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26.f2)->tag == 5U){_LL1: _tmp9A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26.f1)->f1;_tmp99=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26.f2)->f1;_LL2:
# 266
 if(_tmp99->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp29=({struct Cyc_String_pa_PrintArg_struct _tmp462;_tmp462.tag=0U,({struct _dyneither_ptr _tmp4F9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp462.f1=_tmp4F9;});_tmp462;});struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp461;_tmp461.tag=0U,({struct _dyneither_ptr _tmp4FA=(struct _dyneither_ptr)({const char*_tmp2B=": universal quantification in user definition not allowed";_tag_dyneither(_tmp2B,sizeof(char),58U);});_tmp461.f1=_tmp4FA;});_tmp461;});void*_tmp27[2U];_tmp27[0]=& _tmp29,_tmp27[1]=& _tmp2A;({struct Cyc___cycFILE*_tmp4FC=Cyc_stderr;struct _dyneither_ptr _tmp4FB=({const char*_tmp28="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp28,sizeof(char),62U);});Cyc_fprintf(_tmp4FC,_tmp4FB,_tag_dyneither(_tmp27,sizeof(void*),2U));});});return;}
if(_tmp99->sc != _tmp9A->sc)
({struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp464;_tmp464.tag=0U,({struct _dyneither_ptr _tmp4FD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp464.f1=_tmp4FD;});_tmp464;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp463;_tmp463.tag=0U,({struct _dyneither_ptr _tmp4FE=(struct _dyneither_ptr)({const char*_tmp30=": scopes don't match";_tag_dyneither(_tmp30,sizeof(char),21U);});_tmp463.f1=_tmp4FE;});_tmp463;});void*_tmp2C[2U];_tmp2C[0]=& _tmp2E,_tmp2C[1]=& _tmp2F;({struct Cyc___cycFILE*_tmp500=Cyc_stderr;struct _dyneither_ptr _tmp4FF=({const char*_tmp2D="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp2D,sizeof(char),71U);});Cyc_fprintf(_tmp500,_tmp4FF,_tag_dyneither(_tmp2C,sizeof(void*),2U));});});
# 271
if(_tmp99->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp466;_tmp466.tag=0U,({struct _dyneither_ptr _tmp501=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp466.f1=_tmp501;});_tmp466;});struct Cyc_String_pa_PrintArg_struct _tmp34=({struct Cyc_String_pa_PrintArg_struct _tmp465;_tmp465.tag=0U,({struct _dyneither_ptr _tmp502=(struct _dyneither_ptr)({const char*_tmp35=": no user definition";_tag_dyneither(_tmp35,sizeof(char),21U);});_tmp465.f1=_tmp502;});_tmp465;});void*_tmp31[2U];_tmp31[0]=& _tmp33,_tmp31[1]=& _tmp34;({struct Cyc___cycFILE*_tmp504=Cyc_stderr;struct _dyneither_ptr _tmp503=({const char*_tmp32="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp32,sizeof(char),62U);});Cyc_fprintf(_tmp504,_tmp503,_tag_dyneither(_tmp31,sizeof(void*),2U));});});return;}
if(_tmp9A->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp468;_tmp468.tag=0U,({struct _dyneither_ptr _tmp505=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp468.f1=_tmp505;});_tmp468;});struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp467;_tmp467.tag=0U,({struct _dyneither_ptr _tmp506=(struct _dyneither_ptr)({const char*_tmp3A=": no definition for system version";_tag_dyneither(_tmp3A,sizeof(char),35U);});_tmp467.f1=_tmp506;});_tmp467;});void*_tmp36[2U];_tmp36[0]=& _tmp38,_tmp36[1]=& _tmp39;({struct Cyc___cycFILE*_tmp508=Cyc_stderr;struct _dyneither_ptr _tmp507=({const char*_tmp37="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp37,sizeof(char),71U);});Cyc_fprintf(_tmp508,_tmp507,_tag_dyneither(_tmp36,sizeof(void*),2U));});});
c_decl->r=user_decl->r;
return;}
# 278
{struct _tuple27 _tmp3B=({struct _tuple27 _tmp47D;_tmp47D.f1=_tmp9A->impl,_tmp47D.f2=_tmp99->impl;_tmp47D;});struct _tuple27 _tmp3C=_tmp3B;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp75;struct Cyc_List_List*_tmp74;struct Cyc_List_List*_tmp73;int _tmp72;if(_tmp3C.f1 != 0){if(_tmp3C.f2 != 0){_LLA: _tmp76=(_tmp3C.f1)->fields;_tmp75=(_tmp3C.f2)->exist_vars;_tmp74=(_tmp3C.f2)->rgn_po;_tmp73=(_tmp3C.f2)->fields;_tmp72=(_tmp3C.f2)->tagged;_LLB:
# 280
 if(_tmp72){
({struct Cyc_String_pa_PrintArg_struct _tmp3F=({struct Cyc_String_pa_PrintArg_struct _tmp46A;_tmp46A.tag=0U,({struct _dyneither_ptr _tmp509=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp46A.f1=_tmp509;});_tmp46A;});struct Cyc_String_pa_PrintArg_struct _tmp40=({struct Cyc_String_pa_PrintArg_struct _tmp469;_tmp469.tag=0U,({struct _dyneither_ptr _tmp50A=(struct _dyneither_ptr)({const char*_tmp41=": user @tagged annotation not allowed";_tag_dyneither(_tmp41,sizeof(char),38U);});_tmp469.f1=_tmp50A;});_tmp469;});void*_tmp3D[2U];_tmp3D[0]=& _tmp3F,_tmp3D[1]=& _tmp40;({struct Cyc___cycFILE*_tmp50C=Cyc_stderr;struct _dyneither_ptr _tmp50B=({const char*_tmp3E="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp3E,sizeof(char),62U);});Cyc_fprintf(_tmp50C,_tmp50B,_tag_dyneither(_tmp3D,sizeof(void*),2U));});});return;}
if((unsigned int)_tmp75){
({struct Cyc_String_pa_PrintArg_struct _tmp44=({struct Cyc_String_pa_PrintArg_struct _tmp46C;_tmp46C.tag=0U,({struct _dyneither_ptr _tmp50D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp46C.f1=_tmp50D;});_tmp46C;});struct Cyc_String_pa_PrintArg_struct _tmp45=({struct Cyc_String_pa_PrintArg_struct _tmp46B;_tmp46B.tag=0U,({struct _dyneither_ptr _tmp50E=(struct _dyneither_ptr)({const char*_tmp46=": existential quantification in user definition not allowed";_tag_dyneither(_tmp46,sizeof(char),60U);});_tmp46B.f1=_tmp50E;});_tmp46B;});void*_tmp42[2U];_tmp42[0]=& _tmp44,_tmp42[1]=& _tmp45;({struct Cyc___cycFILE*_tmp510=Cyc_stderr;struct _dyneither_ptr _tmp50F=({const char*_tmp43="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp43,sizeof(char),62U);});Cyc_fprintf(_tmp510,_tmp50F,_tag_dyneither(_tmp42,sizeof(void*),2U));});});return;}
if((unsigned int)_tmp74){
({struct Cyc_String_pa_PrintArg_struct _tmp49=({struct Cyc_String_pa_PrintArg_struct _tmp46E;_tmp46E.tag=0U,({struct _dyneither_ptr _tmp511=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp46E.f1=_tmp511;});_tmp46E;});struct Cyc_String_pa_PrintArg_struct _tmp4A=({struct Cyc_String_pa_PrintArg_struct _tmp46D;_tmp46D.tag=0U,({struct _dyneither_ptr _tmp512=(struct _dyneither_ptr)({const char*_tmp4B=": region partial order in user definition not allowed";_tag_dyneither(_tmp4B,sizeof(char),54U);});_tmp46D.f1=_tmp512;});_tmp46D;});void*_tmp47[2U];_tmp47[0]=& _tmp49,_tmp47[1]=& _tmp4A;({struct Cyc___cycFILE*_tmp514=Cyc_stderr;struct _dyneither_ptr _tmp513=({const char*_tmp48="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp48,sizeof(char),62U);});Cyc_fprintf(_tmp514,_tmp513,_tag_dyneither(_tmp47,sizeof(void*),2U));});});return;}{
struct Cyc_List_List*_tmp4C=_tmp76;
# 288
while(_tmp4C != 0){
struct Cyc_Absyn_Aggrfield*_tmp4D=(struct Cyc_Absyn_Aggrfield*)_tmp4C->hd;
struct Cyc_Absyn_Aggrfield*_tmp4E=Cyc_Absyn_lookup_field(_tmp73,_tmp4D->name);
# 292
if(_tmp4E != 0){
# 295
void*_tmp4F=Cyc_Toc_typ_to_c(_tmp4D->type);
void*_tmp50=Cyc_Toc_typ_to_c(_tmp4E->type);
if(!Cyc_Tcutil_unify(_tmp4F,_tmp50))
({struct Cyc_String_pa_PrintArg_struct _tmp53=({struct Cyc_String_pa_PrintArg_struct _tmp473;_tmp473.tag=0U,({struct _dyneither_ptr _tmp515=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp473.f1=_tmp515;});_tmp473;});struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp46F;_tmp46F.tag=0U,({struct _dyneither_ptr _tmp519=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp57=({struct Cyc_String_pa_PrintArg_struct _tmp472;_tmp472.tag=0U,({struct _dyneither_ptr _tmp516=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp472.f1=_tmp516;});_tmp472;});struct Cyc_String_pa_PrintArg_struct _tmp58=({struct Cyc_String_pa_PrintArg_struct _tmp471;_tmp471.tag=0U,_tmp471.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D->name);_tmp471;});struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp470;_tmp470.tag=0U,({struct _dyneither_ptr _tmp517=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4D->type));_tmp470.f1=_tmp517;});_tmp470;});void*_tmp55[3U];_tmp55[0]=& _tmp57,_tmp55[1]=& _tmp58,_tmp55[2]=& _tmp59;({struct _dyneither_ptr _tmp518=({const char*_tmp56=": type %s of user definition of field %s != %s";_tag_dyneither(_tmp56,sizeof(char),47U);});Cyc_aprintf(_tmp518,_tag_dyneither(_tmp55,sizeof(void*),3U));});}));_tmp46F.f1=_tmp519;});_tmp46F;});void*_tmp51[2U];_tmp51[0]=& _tmp53,_tmp51[1]=& _tmp54;({struct Cyc___cycFILE*_tmp51B=Cyc_stderr;struct _dyneither_ptr _tmp51A=({const char*_tmp52="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp52,sizeof(char),71U);});Cyc_fprintf(_tmp51B,_tmp51A,_tag_dyneither(_tmp51,sizeof(void*),2U));});});else{
# 305
if(_tmp4E->width != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp476;_tmp476.tag=0U,({struct _dyneither_ptr _tmp51C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp476.f1=_tmp51C;});_tmp476;});struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0U,({struct _dyneither_ptr _tmp51F=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp60=({struct Cyc_String_pa_PrintArg_struct _tmp475;_tmp475.tag=0U,({struct _dyneither_ptr _tmp51D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp475.f1=_tmp51D;});_tmp475;});void*_tmp5E[1U];_tmp5E[0]=& _tmp60;({struct _dyneither_ptr _tmp51E=({const char*_tmp5F=": ignoring width of user definition of field %s";_tag_dyneither(_tmp5F,sizeof(char),48U);});Cyc_aprintf(_tmp51E,_tag_dyneither(_tmp5E,sizeof(void*),1U));});}));_tmp474.f1=_tmp51F;});_tmp474;});void*_tmp5A[2U];_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D;({struct Cyc___cycFILE*_tmp521=Cyc_stderr;struct _dyneither_ptr _tmp520=({const char*_tmp5B="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp5B,sizeof(char),71U);});Cyc_fprintf(_tmp521,_tmp520,_tag_dyneither(_tmp5A,sizeof(void*),2U));});});
# 309
if(_tmp4E->attributes != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp479;_tmp479.tag=0U,({struct _dyneither_ptr _tmp522=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp479.f1=_tmp522;});_tmp479;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp477;_tmp477.tag=0U,({struct _dyneither_ptr _tmp525=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp478;_tmp478.tag=0U,({struct _dyneither_ptr _tmp523=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp478.f1=_tmp523;});_tmp478;});void*_tmp65[1U];_tmp65[0]=& _tmp67;({struct _dyneither_ptr _tmp524=({const char*_tmp66=": ignoring attributes of user definition of field %s";_tag_dyneither(_tmp66,sizeof(char),53U);});Cyc_aprintf(_tmp524,_tag_dyneither(_tmp65,sizeof(void*),1U));});}));_tmp477.f1=_tmp525;});_tmp477;});void*_tmp61[2U];_tmp61[0]=& _tmp63,_tmp61[1]=& _tmp64;({struct Cyc___cycFILE*_tmp527=Cyc_stderr;struct _dyneither_ptr _tmp526=({const char*_tmp62="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp62,sizeof(char),71U);});Cyc_fprintf(_tmp527,_tmp526,_tag_dyneither(_tmp61,sizeof(void*),2U));});});
# 315
_tmp4D->type=_tmp4E->type;
_tmp4D->tq=_tmp4E->tq;
_tmp4D->requires_clause=_tmp4E->requires_clause;}}
# 320
_tmp4C=_tmp4C->tl;}
# 323
_tmp4C=_tmp73;{
int missing_fields=0;
while(_tmp4C != 0){
struct Cyc_Absyn_Aggrfield*_tmp68=Cyc_Absyn_lookup_field(_tmp76,((struct Cyc_Absyn_Aggrfield*)_tmp4C->hd)->name);
if(_tmp68 == 0){
missing_fields=1;
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp47C;_tmp47C.tag=0U,({struct _dyneither_ptr _tmp528=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp47C.f1=_tmp528;});_tmp47C;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp47A;_tmp47A.tag=0U,({struct _dyneither_ptr _tmp52A=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp47B;_tmp47B.tag=0U,_tmp47B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp4C->hd)->name);_tmp47B;});void*_tmp6D[1U];_tmp6D[0]=& _tmp6F;({struct _dyneither_ptr _tmp529=({const char*_tmp6E=": no definition of field %s in system version";_tag_dyneither(_tmp6E,sizeof(char),46U);});Cyc_aprintf(_tmp529,_tag_dyneither(_tmp6D,sizeof(void*),1U));});}));_tmp47A.f1=_tmp52A;});_tmp47A;});void*_tmp69[2U];_tmp69[0]=& _tmp6B,_tmp69[1]=& _tmp6C;({struct Cyc___cycFILE*_tmp52C=Cyc_stderr;struct _dyneither_ptr _tmp52B=({const char*_tmp6A="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp6A,sizeof(char),71U);});Cyc_fprintf(_tmp52C,_tmp52B,_tag_dyneither(_tmp69,sizeof(void*),2U));});});}
# 332
_tmp4C=_tmp4C->tl;}
# 334
goto _LL9;};};}else{goto _LLC;}}else{_LLC: _LLD:
# 336
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp52D=({const char*_tmp70="Internal Error: NULL cases not possible";_tag_dyneither(_tmp70,sizeof(char),40U);});_tmp71->f1=_tmp52D;});_tmp71;}));}_LL9:;}
# 338
return;}else{goto _LL7;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26.f2)->tag == 7U){_LL3: _tmp9C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26.f1)->f1;_tmp9B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26.f2)->f1;_LL4:
# 340
({struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp47F;_tmp47F.tag=0U,({struct _dyneither_ptr _tmp52E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9C->name));_tmp47F.f1=_tmp52E;});_tmp47F;});struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp47E;_tmp47E.tag=0U,({struct _dyneither_ptr _tmp52F=(struct _dyneither_ptr)({const char*_tmp7B=": enum merging not currently supported";_tag_dyneither(_tmp7B,sizeof(char),39U);});_tmp47E.f1=_tmp52F;});_tmp47E;});void*_tmp77[2U];_tmp77[0]=& _tmp79,_tmp77[1]=& _tmp7A;({struct Cyc___cycFILE*_tmp531=Cyc_stderr;struct _dyneither_ptr _tmp530=({const char*_tmp78="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp78,sizeof(char),62U);});Cyc_fprintf(_tmp531,_tmp530,_tag_dyneither(_tmp77,sizeof(void*),2U));});});return;}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26.f2)->tag == 8U){_LL5: _tmp9E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26.f1)->f1;_tmp9D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26.f2)->f1;_LL6:
# 342
 if(_tmp9D->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp481;_tmp481.tag=0U,({struct _dyneither_ptr _tmp532=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp481.f1=_tmp532;});_tmp481;});struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmp480;_tmp480.tag=0U,({struct _dyneither_ptr _tmp533=(struct _dyneither_ptr)({const char*_tmp80=": universal quantification in user definition not supported";_tag_dyneither(_tmp80,sizeof(char),60U);});_tmp480.f1=_tmp533;});_tmp480;});void*_tmp7C[2U];_tmp7C[0]=& _tmp7E,_tmp7C[1]=& _tmp7F;({struct Cyc___cycFILE*_tmp535=Cyc_stderr;struct _dyneither_ptr _tmp534=({const char*_tmp7D="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp7D,sizeof(char),62U);});Cyc_fprintf(_tmp535,_tmp534,_tag_dyneither(_tmp7C,sizeof(void*),2U));});});return;}
# 345
if(_tmp9D->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp483;_tmp483.tag=0U,({struct _dyneither_ptr _tmp536=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp483.f1=_tmp536;});_tmp483;});struct Cyc_String_pa_PrintArg_struct _tmp84=({struct Cyc_String_pa_PrintArg_struct _tmp482;_tmp482.tag=0U,({struct _dyneither_ptr _tmp537=(struct _dyneither_ptr)({const char*_tmp85=": no user definition";_tag_dyneither(_tmp85,sizeof(char),21U);});_tmp482.f1=_tmp537;});_tmp482;});void*_tmp81[2U];_tmp81[0]=& _tmp83,_tmp81[1]=& _tmp84;({struct Cyc___cycFILE*_tmp539=Cyc_stderr;struct _dyneither_ptr _tmp538=({const char*_tmp82="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp82,sizeof(char),62U);});Cyc_fprintf(_tmp539,_tmp538,_tag_dyneither(_tmp81,sizeof(void*),2U));});});return;}
if(_tmp9E->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp88=({struct Cyc_String_pa_PrintArg_struct _tmp485;_tmp485.tag=0U,({struct _dyneither_ptr _tmp53A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp485.f1=_tmp53A;});_tmp485;});struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp484;_tmp484.tag=0U,({struct _dyneither_ptr _tmp53B=(struct _dyneither_ptr)({const char*_tmp8A=": no definition for system version";_tag_dyneither(_tmp8A,sizeof(char),35U);});_tmp484.f1=_tmp53B;});_tmp484;});void*_tmp86[2U];_tmp86[0]=& _tmp88,_tmp86[1]=& _tmp89;({struct Cyc___cycFILE*_tmp53D=Cyc_stderr;struct _dyneither_ptr _tmp53C=({const char*_tmp87="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp87,sizeof(char),71U);});Cyc_fprintf(_tmp53D,_tmp53C,_tag_dyneither(_tmp86,sizeof(void*),2U));});});
c_decl->r=user_decl->r;
return;}{
# 353
void*_tmp8B=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9E->defn));
void*_tmp8C=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9D->defn));
if(!Cyc_Tcutil_unify(_tmp8B,_tmp8C))
({struct Cyc_String_pa_PrintArg_struct _tmp8F=({struct Cyc_String_pa_PrintArg_struct _tmp489;_tmp489.tag=0U,({struct _dyneither_ptr _tmp53E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp489.f1=_tmp53E;});_tmp489;});struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp486;_tmp486.tag=0U,({struct _dyneither_ptr _tmp542=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmp488;_tmp488.tag=0U,({struct _dyneither_ptr _tmp53F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp9D->defn)));_tmp488.f1=_tmp53F;});_tmp488;});struct Cyc_String_pa_PrintArg_struct _tmp94=({struct Cyc_String_pa_PrintArg_struct _tmp487;_tmp487.tag=0U,({struct _dyneither_ptr _tmp540=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp9E->defn)));_tmp487.f1=_tmp540;});_tmp487;});void*_tmp91[2U];_tmp91[0]=& _tmp93,_tmp91[1]=& _tmp94;({struct _dyneither_ptr _tmp541=({const char*_tmp92=": type definition %s of user definition != %s";_tag_dyneither(_tmp92,sizeof(char),46U);});Cyc_aprintf(_tmp541,_tag_dyneither(_tmp91,sizeof(void*),2U));});}));_tmp486.f1=_tmp542;});_tmp486;});void*_tmp8D[2U];_tmp8D[0]=& _tmp8F,_tmp8D[1]=& _tmp90;({struct Cyc___cycFILE*_tmp544=Cyc_stderr;struct _dyneither_ptr _tmp543=({const char*_tmp8E="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp8E,sizeof(char),71U);});Cyc_fprintf(_tmp544,_tmp543,_tag_dyneither(_tmp8D,sizeof(void*),2U));});});else{
# 362
_tmp9E->defn=_tmp9D->defn;
_tmp9E->tq=_tmp9D->tq;}
# 365
goto _LL0;};}else{goto _LL7;}default: _LL7: _LL8:
# 367
({struct Cyc_String_pa_PrintArg_struct _tmp97=({struct Cyc_String_pa_PrintArg_struct _tmp48A;_tmp48A.tag=0U,({
# 369
struct _dyneither_ptr _tmp545=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp98[1U];_tmp98[0]=user_decl;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp98,sizeof(struct Cyc_Absyn_Decl*),1U));})));_tmp48A.f1=_tmp545;});_tmp48A;});void*_tmp95[1U];_tmp95[0]=& _tmp97;({struct Cyc___cycFILE*_tmp547=Cyc_stderr;struct _dyneither_ptr _tmp546=({const char*_tmp96="Error in .cys file: bad user-defined type %s\n";_tag_dyneither(_tmp96,sizeof(char),46U);});Cyc_fprintf(_tmp547,_tmp546,_tag_dyneither(_tmp95,sizeof(void*),1U));});});
return;}_LL0:;}
# 373
c_decl->r=user_decl->r;}
# 378
const int Cyc_lex_base[431U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431U]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 385
int state;int base;int backtrk;
int c;
state=start_state;
# 389
if(state >= 0){
({int _tmp548=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp548;});
lbuf->lex_last_action=- 1;}else{
# 393
state=(- state)- 1;}
# 395
while(1){
base=Cyc_lex_base[_check_known_subscript_notnull(431U,state)];
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 403
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 407
c=256;}}else{
# 409
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == - 1)c=256;}
# 412
if(Cyc_lex_check[_check_known_subscript_notnull(3846U,base + c)]== state)
state=Cyc_lex_trans[_check_known_subscript_notnull(3846U,base + c)];else{
# 415
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp549=({const char*_tmp9F="empty token";_tag_dyneither(_tmp9F,sizeof(char),12U);});_tmpA0->f1=_tmp549;});_tmpA0;}));else{
# 421
return lbuf->lex_last_action;}}else{
# 424
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 428
struct _tuple21*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp54A=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp54A;});
{int _tmpA1=lexstate;switch(_tmpA1){case 0U: _LL1: _LL2:
# 387 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
({struct Cyc_Set_Set**_tmp54D=({struct Cyc_Set_Set**_tmpA2=_cycalloc(sizeof(*_tmpA2));({struct Cyc_Set_Set*_tmp54C=({struct Cyc_Set_Set*_tmp54B=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(_tmp54B,(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});*_tmpA2=_tmp54C;});_tmpA2;});Cyc_current_targets=_tmp54D;});}
# 392
return({struct _tuple21*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmpA3->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmpA3;});case 1U: _LL3: _LL4:
# 395 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2U: _LL5: _LL6:
# 397
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 401
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp54E=({const char*_tmpA4="some action didn't return!";_tag_dyneither(_tmpA4,sizeof(char),27U);});_tmpA5->f1=_tmp54E;});_tmpA5;}));}
# 403
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp54F=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp54F;});
{int _tmpA6=lexstate;switch(_tmpA6){case 0U: _LL1: _LL2:
# 401 "buildlib.cyl"
({struct _dyneither_ptr*_tmp553=({struct _dyneither_ptr*_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct _dyneither_ptr _tmp552=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp551=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp551,0,(unsigned long)(({
int _tmp550=Cyc_Lexing_lexeme_end(lexbuf);_tmp550 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 401
*_tmpA7=_tmp552;});_tmpA7;});Cyc_current_source=_tmp553;});
# 403
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp555=({struct Cyc_Set_Set**_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct Cyc_Set_Set*_tmp554=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmpA8=_tmp554;});_tmpA8;});Cyc_current_targets=_tmp555;});
Cyc_token(lexbuf);
return 0;case 1U: _LL3: _LL4:
# 409
({struct _dyneither_ptr*_tmp559=({struct _dyneither_ptr*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct _dyneither_ptr _tmp558=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp557=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp557,0,(unsigned long)(({
int _tmp556=Cyc_Lexing_lexeme_end(lexbuf);_tmp556 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 409
*_tmpA9=_tmp558;});_tmpA9;});Cyc_current_source=_tmp559;});
# 411
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp55B=({struct Cyc_Set_Set**_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct Cyc_Set_Set*_tmp55A=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmpAA=_tmp55A;});_tmpAA;});Cyc_current_targets=_tmp55B;});
Cyc_args(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 417
({struct _dyneither_ptr*_tmp55D=({struct _dyneither_ptr*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct _dyneither_ptr _tmp55C=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpAB=_tmp55C;});_tmpAB;});Cyc_current_source=_tmp55D;});
Cyc_current_args=0;
({struct Cyc_Set_Set**_tmp55F=({struct Cyc_Set_Set**_tmpAC=_cycalloc(sizeof(*_tmpAC));({struct Cyc_Set_Set*_tmp55E=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmpAC=_tmp55E;});_tmpAC;});Cyc_current_targets=_tmp55F;});
Cyc_token(lexbuf);
return 0;default: _LL7: _LL8:
# 423
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 426
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp560=({const char*_tmpAD="some action didn't return!";_tag_dyneither(_tmpAD,sizeof(char),27U);});_tmpAE->f1=_tmp560;});_tmpAE;}));}
# 428
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp561=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp561;});
{int _tmpAF=lexstate;switch(_tmpAF){case 0U: _LL1: _LL2: {
# 426 "buildlib.cyl"
struct _dyneither_ptr*_tmpB0=({struct _dyneither_ptr*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct _dyneither_ptr _tmp564=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp563=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp563,0,(unsigned long)(({
int _tmp562=Cyc_Lexing_lexeme_end(lexbuf);_tmp562 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 426
*_tmpB2=_tmp564;});_tmpB2;});
# 428
({struct Cyc_List_List*_tmp565=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->hd=_tmpB0,_tmpB1->tl=Cyc_current_args;_tmpB1;});Cyc_current_args=_tmp565;});
return Cyc_args(lexbuf);}case 1U: _LL3: _LL4: {
# 432
struct _dyneither_ptr*_tmpB3=({struct _dyneither_ptr*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _dyneither_ptr _tmp568=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp567=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp567,0,(unsigned long)(({
int _tmp566=Cyc_Lexing_lexeme_end(lexbuf);_tmp566 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 432
*_tmpB5=_tmp568;});_tmpB5;});
# 434
({struct Cyc_List_List*_tmp569=({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->hd=_tmpB3,_tmpB4->tl=Cyc_current_args;_tmpB4;});Cyc_current_args=_tmp569;});
return Cyc_args(lexbuf);}case 2U: _LL5: _LL6: {
# 438
struct _dyneither_ptr*_tmpB6=({struct _dyneither_ptr*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct _dyneither_ptr _tmp56C=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp56B=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp56B,0,(unsigned long)(({
int _tmp56A=Cyc_Lexing_lexeme_end(lexbuf);_tmp56A - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 438
*_tmpB8=_tmp56C;});_tmpB8;});
# 440
({struct Cyc_List_List*_tmp56D=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->hd=_tmpB6,_tmpB7->tl=Cyc_current_args;_tmpB7;});Cyc_current_args=_tmp56D;});
return Cyc_token(lexbuf);}default: _LL7: _LL8:
# 443
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL0:;}
# 446
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp56E=({const char*_tmpB9="some action didn't return!";_tag_dyneither(_tmpB9,sizeof(char),27U);});_tmpBA->f1=_tmp56E;});_tmpBA;}));}
# 448
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp56F=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp56F;});
{int _tmpBB=lexstate;switch(_tmpBB){case 0U: _LL1: _LL2:
# 447 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _dyneither_ptr _tmp570=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpBC=_tmp570;});_tmpBC;}));return Cyc_token(lexbuf);case 1U: _LL3: _LL4:
# 450 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 453 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3U: _LL7: _LL8:
# 456 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4U: _LL9: _LLA:
# 459 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5U: _LLB: _LLC:
# 461
 return Cyc_token(lexbuf);case 6U: _LLD: _LLE:
# 463
 return Cyc_token(lexbuf);case 7U: _LLF: _LL10:
# 465
 return Cyc_token(lexbuf);case 8U: _LL11: _LL12:
# 468 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9U: _LL13: _LL14:
# 471 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10U: _LL15: _LL16:
# 474 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11U: _LL17: _LL18:
# 476
 return Cyc_token(lexbuf);case 12U: _LL19: _LL1A:
# 478
 return Cyc_token(lexbuf);case 13U: _LL1B: _LL1C:
# 480
 return Cyc_token(lexbuf);case 14U: _LL1D: _LL1E:
# 482
 return Cyc_token(lexbuf);case 15U: _LL1F: _LL20:
# 484
 return Cyc_token(lexbuf);case 16U: _LL21: _LL22:
# 486
 return Cyc_token(lexbuf);case 17U: _LL23: _LL24:
# 488
 return Cyc_token(lexbuf);case 18U: _LL25: _LL26:
# 490
 return Cyc_token(lexbuf);case 19U: _LL27: _LL28:
# 492
 return Cyc_token(lexbuf);case 20U: _LL29: _LL2A:
# 494
 return Cyc_token(lexbuf);case 21U: _LL2B: _LL2C:
# 496
 return Cyc_token(lexbuf);case 22U: _LL2D: _LL2E:
# 498
 return Cyc_token(lexbuf);case 23U: _LL2F: _LL30:
# 500
 return Cyc_token(lexbuf);case 24U: _LL31: _LL32:
# 503 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25U: _LL33: _LL34:
# 505
 return Cyc_token(lexbuf);case 26U: _LL35: _LL36:
# 507
 return Cyc_token(lexbuf);case 27U: _LL37: _LL38:
# 509
 return Cyc_token(lexbuf);case 28U: _LL39: _LL3A:
# 511
 return Cyc_token(lexbuf);case 29U: _LL3B: _LL3C:
# 513
 return Cyc_token(lexbuf);case 30U: _LL3D: _LL3E:
# 515
 return Cyc_token(lexbuf);case 31U: _LL3F: _LL40:
# 517
 return Cyc_token(lexbuf);case 32U: _LL41: _LL42:
# 519
 return Cyc_token(lexbuf);case 33U: _LL43: _LL44:
# 521
 return Cyc_token(lexbuf);case 34U: _LL45: _LL46:
# 523
 return Cyc_token(lexbuf);case 35U: _LL47: _LL48:
# 525
 return Cyc_token(lexbuf);case 36U: _LL49: _LL4A:
# 527
 return Cyc_token(lexbuf);case 37U: _LL4B: _LL4C:
# 529
 return Cyc_token(lexbuf);case 38U: _LL4D: _LL4E:
# 531
 return Cyc_token(lexbuf);case 39U: _LL4F: _LL50:
# 533
 return Cyc_token(lexbuf);case 40U: _LL51: _LL52:
# 535
 return Cyc_token(lexbuf);case 41U: _LL53: _LL54:
# 537
 return Cyc_token(lexbuf);case 42U: _LL55: _LL56:
# 539
 return Cyc_token(lexbuf);case 43U: _LL57: _LL58:
# 541
 return Cyc_token(lexbuf);case 44U: _LL59: _LL5A:
# 543
 return Cyc_token(lexbuf);case 45U: _LL5B: _LL5C:
# 545
 return Cyc_token(lexbuf);case 46U: _LL5D: _LL5E:
# 547
 return Cyc_token(lexbuf);case 47U: _LL5F: _LL60:
# 549
 return Cyc_token(lexbuf);case 48U: _LL61: _LL62:
# 552 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL63: _LL64:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL0:;}
# 556
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp571=({const char*_tmpBD="some action didn't return!";_tag_dyneither(_tmpBD,sizeof(char),27U);});_tmpBE->f1=_tmp571;});_tmpBE;}));}
# 558
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp572=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp572;});
{int _tmpBF=lexstate;switch(_tmpBF){case 0U: _LL1: _LL2:
# 557 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1U: _LL3: _LL4:
# 558 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 559 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3U: _LL7: _LL8:
# 560 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4U: _LL9: _LLA:
# 563 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5U: _LLB: _LLC:
# 566 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6U: _LLD: _LLE:
# 568
 return Cyc_string(lexbuf);case 7U: _LLF: _LL10:
# 569 "buildlib.cyl"
 return 0;case 8U: _LL11: _LL12:
# 570 "buildlib.cyl"
 return 0;case 9U: _LL13: _LL14:
# 571 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL15: _LL16:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL0:;}
# 575
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp573=({const char*_tmpC0="some action didn't return!";_tag_dyneither(_tmpC0,sizeof(char),27U);});_tmpC1->f1=_tmp573;});_tmpC1;}));}
# 577
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp574=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp574;});
{int _tmpC2=lexstate;switch(_tmpC2){case 0U: _LL1: _LL2:
# 580 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 582
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2U: _LL5: _LL6:
# 589 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC3=0U;({struct _dyneither_ptr _tmp575=({const char*_tmpC4="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpC4,sizeof(char),44U);});Cyc_log(_tmp575,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});
return 1;case 3U: _LL7: _LL8:
# 595 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC5=0U;({struct _dyneither_ptr _tmp576=({const char*_tmpC6="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpC6,sizeof(char),44U);});Cyc_log(_tmp576,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});
return 1;case 4U: _LL9: _LLA:
# 601 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC7=0U;({struct _dyneither_ptr _tmp577=({const char*_tmpC8="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpC8,sizeof(char),44U);});Cyc_log(_tmp577,_tag_dyneither(_tmpC7,sizeof(void*),0U));});});
return 1;case 5U: _LLB: _LLC:
# 607 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC9=0U;({struct _dyneither_ptr _tmp578=({const char*_tmpCA="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpCA,sizeof(char),44U);});Cyc_log(_tmp578,_tag_dyneither(_tmpC9,sizeof(void*),0U));});});
return 1;case 6U: _LLD: _LLE:
# 611
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCB=0U;({struct _dyneither_ptr _tmp579=({const char*_tmpCC="Warning: use of region sidestepped\n";_tag_dyneither(_tmpCC,sizeof(char),36U);});Cyc_log(_tmp579,_tag_dyneither(_tmpCB,sizeof(void*),0U));});});
return 1;case 7U: _LLF: _LL10:
# 615
({void*_tmpCD=0U;({struct _dyneither_ptr _tmp57A=({const char*_tmpCE="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmpCE,sizeof(char),39U);});Cyc_log(_tmp57A,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});
return 1;case 8U: _LL11: _LL12:
# 620 "buildlib.cyl"
({void*_tmpCF=0U;({struct _dyneither_ptr _tmp57B=({const char*_tmpD0="Warning: use of mode HI deleted\n";_tag_dyneither(_tmpD0,sizeof(char),33U);});Cyc_log(_tmp57B,_tag_dyneither(_tmpCF,sizeof(void*),0U));});});
return 1;case 9U: _LL13: _LL14:
# 623
({void*_tmpD1=0U;({struct _dyneither_ptr _tmp57C=({const char*_tmpD2="Warning: use of mode SI deleted\n";_tag_dyneither(_tmpD2,sizeof(char),33U);});Cyc_log(_tmp57C,_tag_dyneither(_tmpD1,sizeof(void*),0U));});});
return 1;case 10U: _LL15: _LL16:
# 626
({void*_tmpD3=0U;({struct _dyneither_ptr _tmp57D=({const char*_tmpD4="Warning: use of mode QI deleted\n";_tag_dyneither(_tmpD4,sizeof(char),33U);});Cyc_log(_tmp57D,_tag_dyneither(_tmpD3,sizeof(void*),0U));});});
return 1;case 11U: _LL17: _LL18:
# 629
({void*_tmpD5=0U;({struct _dyneither_ptr _tmp57E=({const char*_tmpD6="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpD6,sizeof(char),33U);});Cyc_log(_tmp57E,_tag_dyneither(_tmpD5,sizeof(void*),0U));});});
return 1;case 12U: _LL19: _LL1A:
# 632
({void*_tmpD7=0U;({struct _dyneither_ptr _tmp57F=({const char*_tmpD8="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpD8,sizeof(char),33U);});Cyc_log(_tmp57F,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});
return 1;case 13U: _LL1B: _LL1C:
# 635
({void*_tmpD9=0U;({struct _dyneither_ptr _tmp580=({const char*_tmpDA="Warning: use of mode word deleted\n";_tag_dyneither(_tmpDA,sizeof(char),35U);});Cyc_log(_tmp580,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});
return 1;case 14U: _LL1D: _LL1E:
# 638
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15U: _LL1F: _LL20:
# 640
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16U: _LL21: _LL22:
# 642
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17U: _LL23: _LL24:
# 644
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18U: _LL25: _LL26:
# 649 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19U: _LL27: _LL28:
# 651
 return 1;case 20U: _LL29: _LL2A:
# 653
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpDB=0U;({struct _dyneither_ptr _tmp581=({const char*_tmpDC="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmpDC,sizeof(char),42U);});Cyc_log(_tmp581,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
return 1;case 21U: _LL2B: _LL2C:
# 659
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpDD=0U;({struct _dyneither_ptr _tmp582=({const char*_tmpDE="Warning: replacing use of __asm with 0\n";_tag_dyneither(_tmpDE,sizeof(char),40U);});Cyc_log(_tmp582,_tag_dyneither(_tmpDD,sizeof(void*),0U));});});
return 1;case 22U: _LL2D: _LL2E:
# 665
({int _tmp583=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp583,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default: _LL2F: _LL30:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL0:;}
# 669
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp584=({const char*_tmpDF="some action didn't return!";_tag_dyneither(_tmpDF,sizeof(char),27U);});_tmpE0->f1=_tmp584;});_tmpE0;}));}
# 671
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp585=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp585;});
{int _tmpE1=lexstate;switch(_tmpE1){case 0U: _LL1: _LL2:
# 669 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 671
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2U: _LL5: _LL6:
# 673
({void*_tmpE2=0U;({struct _dyneither_ptr _tmp586=({const char*_tmpE3="Warning: unclosed string\n";_tag_dyneither(_tmpE3,sizeof(char),26U);});Cyc_log(_tmp586,_tag_dyneither(_tmpE2,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmpE6=({struct Cyc_String_pa_PrintArg_struct _tmp48C;_tmp48C.tag=0U,({struct _dyneither_ptr _tmp587=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp48C.f1=_tmp587;});_tmp48C;});void*_tmpE4[1U];_tmpE4[0]=& _tmpE6;({struct Cyc___cycFILE*_tmp589=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp588=({const char*_tmpE5="%s";_tag_dyneither(_tmpE5,sizeof(char),3U);});Cyc_fprintf(_tmp589,_tmp588,_tag_dyneither(_tmpE4,sizeof(void*),1U));});});return 1;case 3U: _LL7: _LL8:
# 676
({struct Cyc_String_pa_PrintArg_struct _tmpE9=({struct Cyc_String_pa_PrintArg_struct _tmp48D;_tmp48D.tag=0U,({struct _dyneither_ptr _tmp58A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp48D.f1=_tmp58A;});_tmp48D;});void*_tmpE7[1U];_tmpE7[0]=& _tmpE9;({struct Cyc___cycFILE*_tmp58C=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp58B=({const char*_tmpE8="%s";_tag_dyneither(_tmpE8,sizeof(char),3U);});Cyc_fprintf(_tmp58C,_tmp58B,_tag_dyneither(_tmpE7,sizeof(void*),1U));});});return 1;case 4U: _LL9: _LLA:
# 678
({struct Cyc_String_pa_PrintArg_struct _tmpEC=({struct Cyc_String_pa_PrintArg_struct _tmp48E;_tmp48E.tag=0U,({struct _dyneither_ptr _tmp58D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp48E.f1=_tmp58D;});_tmp48E;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEC;({struct Cyc___cycFILE*_tmp58F=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp58E=({const char*_tmpEB="%s";_tag_dyneither(_tmpEB,sizeof(char),3U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_dyneither(_tmpEA,sizeof(void*),1U));});});return 1;case 5U: _LLB: _LLC:
# 680
({struct Cyc_String_pa_PrintArg_struct _tmpEF=({struct Cyc_String_pa_PrintArg_struct _tmp48F;_tmp48F.tag=0U,({struct _dyneither_ptr _tmp590=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp48F.f1=_tmp590;});_tmp48F;});void*_tmpED[1U];_tmpED[0]=& _tmpEF;({struct Cyc___cycFILE*_tmp592=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp591=({const char*_tmpEE="%s";_tag_dyneither(_tmpEE,sizeof(char),3U);});Cyc_fprintf(_tmp592,_tmp591,_tag_dyneither(_tmpED,sizeof(void*),1U));});});return 1;case 6U: _LLD: _LLE:
# 682
({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp490;_tmp490.tag=0U,({struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp490.f1=_tmp593;});_tmp490;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF2;({struct Cyc___cycFILE*_tmp595=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp594=({const char*_tmpF1="%s";_tag_dyneither(_tmpF1,sizeof(char),3U);});Cyc_fprintf(_tmp595,_tmp594,_tag_dyneither(_tmpF0,sizeof(void*),1U));});});return 1;case 7U: _LLF: _LL10:
# 684
({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp491;_tmp491.tag=0U,({struct _dyneither_ptr _tmp596=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp491.f1=_tmp596;});_tmp491;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({struct Cyc___cycFILE*_tmp598=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp597=({const char*_tmpF4="%s";_tag_dyneither(_tmpF4,sizeof(char),3U);});Cyc_fprintf(_tmp598,_tmp597,_tag_dyneither(_tmpF3,sizeof(void*),1U));});});return 1;case 8U: _LL11: _LL12:
# 686
({struct Cyc_String_pa_PrintArg_struct _tmpF8=({struct Cyc_String_pa_PrintArg_struct _tmp492;_tmp492.tag=0U,({struct _dyneither_ptr _tmp599=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp492.f1=_tmp599;});_tmp492;});void*_tmpF6[1U];_tmpF6[0]=& _tmpF8;({struct Cyc___cycFILE*_tmp59B=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp59A=({const char*_tmpF7="%s";_tag_dyneither(_tmpF7,sizeof(char),3U);});Cyc_fprintf(_tmp59B,_tmp59A,_tag_dyneither(_tmpF6,sizeof(void*),1U));});});return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL0:;}
# 690
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp59C=({const char*_tmpF9="some action didn't return!";_tag_dyneither(_tmpF9,sizeof(char),27U);});_tmpFA->f1=_tmp59C;});_tmpFA;}));}
# 692
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp59D=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp59D;});
{int _tmpFB=lexstate;switch(_tmpFB){case 0U: _LL1: _LL2:
# 696 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 698
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2U: _LL5: _LL6:
# 702
 ++ Cyc_parens_to_match;
return 1;case 3U: _LL7: _LL8:
# 705
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 708
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 711
 return 1;case 6U: _LLD: _LLE:
# 713
 return 1;default: _LLF: _LL10:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL0:;}
# 717
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp59E=({const char*_tmpFC="some action didn't return!";_tag_dyneither(_tmpFC,sizeof(char),27U);});_tmpFD->f1=_tmp59E;});_tmpFD;}));}
# 719
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp59F=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp59F;});
{int _tmpFE=lexstate;switch(_tmpFE){case 0U: _LL1: _LL2:
# 717 "buildlib.cyl"
({void*_tmpFF=0U;({struct _dyneither_ptr _tmp5A0=({const char*_tmp100="Warning: unclosed string\n";_tag_dyneither(_tmp100,sizeof(char),26U);});Cyc_log(_tmp5A0,_tag_dyneither(_tmpFF,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 719
 return 0;case 2U: _LL5: _LL6:
# 721
({void*_tmp101=0U;({struct _dyneither_ptr _tmp5A1=({const char*_tmp102="Warning: unclosed string\n";_tag_dyneither(_tmp102,sizeof(char),26U);});Cyc_log(_tmp5A1,_tag_dyneither(_tmp101,sizeof(void*),0U));});});return 1;case 3U: _LL7: _LL8:
# 723
 return 1;case 4U: _LL9: _LLA:
# 725
 return 1;case 5U: _LLB: _LLC:
# 727
 return 1;case 6U: _LLD: _LLE:
# 729
 return 1;case 7U: _LLF: _LL10:
# 731
 return 1;case 8U: _LL11: _LL12:
# 733
 return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL0:;}
# 737
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5A2=({const char*_tmp103="some action didn't return!";_tag_dyneither(_tmp103,sizeof(char),27U);});_tmp104->f1=_tmp5A2;});_tmp104;}));}
# 739
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5A3=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5A3;});
{int _tmp105=lexstate;switch(_tmp105){case 0U: _LL1: _LL2:
# 737 "buildlib.cyl"
({void*_tmp106=0U;({struct _dyneither_ptr _tmp5A4=({const char*_tmp107="Warning: unclosed comment\n";_tag_dyneither(_tmp107,sizeof(char),27U);});Cyc_log(_tmp5A4,_tag_dyneither(_tmp106,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 739
 return 0;case 2U: _LL5: _LL6:
# 741
 return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL0:;}
# 745
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5A5=({const char*_tmp108="some action didn't return!";_tag_dyneither(_tmp108,sizeof(char),27U);});_tmp109->f1=_tmp5A5;});_tmp109;}));}
# 747
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple22*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5A6=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5A6;});
{int _tmp10A=lexstate;switch(_tmp10A){case 0U: _LL1: _LL2:
# 749 "buildlib.cyl"
({struct _dyneither_ptr _tmp5A7=({const char*_tmp10B="#define ";_tag_dyneither(_tmp10B,sizeof(char),9U);});Cyc_current_line=_tmp5A7;});
Cyc_suck_macroname(lexbuf);
return({struct _tuple22*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->f1=Cyc_current_line,_tmp10C->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp10C;});case 1U: _LL3: _LL4:
# 753
 return Cyc_suck_line(lexbuf);case 2U: _LL5: _LL6:
# 755
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL0:;}
# 759
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5A8=({const char*_tmp10D="some action didn't return!";_tag_dyneither(_tmp10D,sizeof(char),27U);});_tmp10E->f1=_tmp5A8;});_tmp10E;}));}
# 761
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5A9=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5A9;});
{int _tmp10F=lexstate;if(_tmp10F == 0){_LL1: _LL2:
# 759 "buildlib.cyl"
({struct _dyneither_ptr*_tmp5AB=({struct _dyneither_ptr*_tmp110=_cycalloc(sizeof(*_tmp110));({struct _dyneither_ptr _tmp5AA=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp110=_tmp5AA;});_tmp110;});Cyc_current_source=_tmp5AB;});
({struct _dyneither_ptr _tmp5AC=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));Cyc_current_line=_tmp5AC;});
return Cyc_suck_restofline(lexbuf);}else{_LL3: _LL4:
# 763
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 766
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5AD=({const char*_tmp111="some action didn't return!";_tag_dyneither(_tmp111,sizeof(char),27U);});_tmp112->f1=_tmp5AD;});_tmp112;}));}
# 768
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5AE=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5AE;});
{int _tmp113=lexstate;if(_tmp113 == 0){_LL1: _LL2:
# 766 "buildlib.cyl"
({struct _dyneither_ptr _tmp5B0=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp5AF=(struct _dyneither_ptr)Cyc_current_line;Cyc_strconcat(_tmp5AF,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});Cyc_current_line=_tmp5B0;});return 0;}else{_LL3: _LL4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL0:;}
# 770
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5B1=({const char*_tmp114="some action didn't return!";_tag_dyneither(_tmp114,sizeof(char),27U);});_tmp115->f1=_tmp5B1;});_tmp115;}));}
# 772
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple25*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5B2=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5B2;});
{int _tmp116=lexstate;switch(_tmp116){case 0U: _LL1: _LL2:
# 773 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1U: _LL3: _LL4:
# 775
({struct _dyneither_ptr _tmp5B5=(struct _dyneither_ptr)({
struct _dyneither_ptr _tmp5B4=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp5B4,0,(unsigned long)(({
int _tmp5B3=Cyc_Lexing_lexeme_end(lexbuf);_tmp5B3 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 775
Cyc_current_headerfile=_tmp5B5;});
# 778
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
Cyc_current_cpp=0;
while(Cyc_commands(lexbuf)){;}
({struct Cyc_List_List*_tmp5B6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);Cyc_current_hstubs=_tmp5B6;});
({struct Cyc_List_List*_tmp5B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);Cyc_current_cstubs=_tmp5B7;});
({struct Cyc_List_List*_tmp5B8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);Cyc_current_cycstubs=_tmp5B8;});
({struct Cyc_List_List*_tmp5B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);Cyc_current_cpp=_tmp5B9;});
return({struct _tuple25*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->f1=Cyc_current_headerfile,_tmp117->f2=Cyc_current_symbols,_tmp117->f3=Cyc_current_user_defs,_tmp117->f4=Cyc_current_omit_symbols,_tmp117->f5=Cyc_current_hstubs,_tmp117->f6=Cyc_current_cstubs,_tmp117->f7=Cyc_current_cycstubs,_tmp117->f8=Cyc_current_cpp;_tmp117;});case 2U: _LL5: _LL6:
# 800
 return Cyc_spec(lexbuf);case 3U: _LL7: _LL8:
# 802
 return 0;case 4U: _LL9: _LLA:
# 804
({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp493;_tmp493.tag=1U,({
# 806
unsigned long _tmp5BA=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp493.f1=_tmp5BA;});_tmp493;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct Cyc___cycFILE*_tmp5BC=Cyc_stderr;struct _dyneither_ptr _tmp5BB=({const char*_tmp119="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp119,sizeof(char),67U);});Cyc_fprintf(_tmp5BC,_tmp5BB,_tag_dyneither(_tmp118,sizeof(void*),1U));});});
return 0;default: _LLB: _LLC:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL0:;}
# 811
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5BD=({const char*_tmp11B="some action didn't return!";_tag_dyneither(_tmp11B,sizeof(char),27U);});_tmp11C->f1=_tmp5BD;});_tmp11C;}));}
# 813
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5BE=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5BE;});
{int _tmp11D=lexstate;switch(_tmp11D){case 0U: _LL1: _LL2:
# 811 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 813
 return 0;case 2U: _LL5: _LL6:
# 815
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
({struct Cyc_List_List*_tmp5BF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);Cyc_current_symbols=_tmp5BF;});
return 1;case 3U: _LL7: _LL8:
# 820
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp11E=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){;}
if(_tmp11E != Cyc_current_user_defs){
({void*_tmp11F=0U;({struct Cyc___cycFILE*_tmp5C1=Cyc_stderr;struct _dyneither_ptr _tmp5C0=({const char*_tmp120="Error in .cys file: got optional definition in omitsymbols\n";_tag_dyneither(_tmp120,sizeof(char),60U);});Cyc_fprintf(_tmp5C1,_tmp5C0,_tag_dyneither(_tmp11F,sizeof(void*),0U));});});
# 826
return 0;}
# 828
({struct Cyc_List_List*_tmp5C2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);Cyc_current_omit_symbols=_tmp5C2;});
return 1;};case 4U: _LL9: _LLA:
# 831
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5C3=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5C3;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp122=_cycalloc(sizeof(*_tmp122));({struct _dyneither_ptr _tmp5C5=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp122->f1=_tmp5C5;}),({
struct _dyneither_ptr _tmp5C4=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp122->f2=_tmp5C4;});_tmp122;});
({struct Cyc_List_List*_tmp5C6=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=x,_tmp121->tl=Cyc_current_hstubs;_tmp121;});Cyc_current_hstubs=_tmp5C6;});
return 1;};case 5U: _LLB: _LLC: {
# 839
struct _dyneither_ptr _tmp123=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp123,sizeof(char),(int)Cyc_strlen(({const char*_tmp124="hstub";_tag_dyneither(_tmp124,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp123,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp123,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp123;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp125=Cyc_substring((struct _dyneither_ptr)_tmp123,0,(unsigned long)((t.curr - _tmp123.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5C7=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5C7;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->f1=(struct _dyneither_ptr)_tmp125,({
struct _dyneither_ptr _tmp5C8=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp127->f2=_tmp5C8;});_tmp127;});
({struct Cyc_List_List*_tmp5C9=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->hd=x,_tmp126->tl=Cyc_current_hstubs;_tmp126;});Cyc_current_hstubs=_tmp5C9;});
return 1;};};};}case 6U: _LLD: _LLE:
# 853
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5CA=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5CA;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp129=_cycalloc(sizeof(*_tmp129));({struct _dyneither_ptr _tmp5CC=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp129->f1=_tmp5CC;}),({
struct _dyneither_ptr _tmp5CB=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp129->f2=_tmp5CB;});_tmp129;});
({struct Cyc_List_List*_tmp5CD=({struct Cyc_List_List*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=x,_tmp128->tl=Cyc_current_cstubs;_tmp128;});Cyc_current_cstubs=_tmp5CD;});
return 1;};case 7U: _LLF: _LL10: {
# 861
struct _dyneither_ptr _tmp12A=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp12A,sizeof(char),(int)Cyc_strlen(({const char*_tmp12B="cstub";_tag_dyneither(_tmp12B,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp12A,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp12A,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp12A;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp12C=Cyc_substring((struct _dyneither_ptr)_tmp12A,0,(unsigned long)((t.curr - _tmp12A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5CE=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5CE;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->f1=(struct _dyneither_ptr)_tmp12C,({
struct _dyneither_ptr _tmp5CF=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp12E->f2=_tmp5CF;});_tmp12E;});
({struct Cyc_List_List*_tmp5D0=({struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->hd=x,_tmp12D->tl=Cyc_current_cstubs;_tmp12D;});Cyc_current_cstubs=_tmp5D0;});
return 1;};};};}case 8U: _LL11: _LL12:
# 875
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5D1=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5D1;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp130=_cycalloc(sizeof(*_tmp130));({struct _dyneither_ptr _tmp5D3=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp130->f1=_tmp5D3;}),({
struct _dyneither_ptr _tmp5D2=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp130->f2=_tmp5D2;});_tmp130;});
({struct Cyc_List_List*_tmp5D4=({struct Cyc_List_List*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->hd=x,_tmp12F->tl=Cyc_current_cycstubs;_tmp12F;});Cyc_current_cycstubs=_tmp5D4;});
return 1;};case 9U: _LL13: _LL14: {
# 883
struct _dyneither_ptr _tmp131=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp131,sizeof(char),(int)Cyc_strlen(({const char*_tmp132="cycstub";_tag_dyneither(_tmp132,sizeof(char),8U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp131,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp131,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp131;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp133=Cyc_substring((struct _dyneither_ptr)_tmp131,0,(unsigned long)((t.curr - _tmp131.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5D5=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5D5;});
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->f1=(struct _dyneither_ptr)_tmp133,({
struct _dyneither_ptr _tmp5D6=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp135->f2=_tmp5D6;});_tmp135;});
({struct Cyc_List_List*_tmp5D7=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=x,_tmp134->tl=Cyc_current_cycstubs;_tmp134;});Cyc_current_cycstubs=_tmp5D7;});
return 1;};};};}case 10U: _LL15: _LL16:
# 897
 Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5D8=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5D8;});
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmp137=_cycalloc(sizeof(*_tmp137));({struct _dyneither_ptr _tmp5D9=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmp137=_tmp5D9;});_tmp137;});
({struct Cyc_List_List*_tmp5DA=({struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=x,_tmp136->tl=Cyc_current_cpp;_tmp136;});Cyc_current_cpp=_tmp5DA;});
return 1;};case 11U: _LL17: _LL18:
# 904
 return 1;case 12U: _LL19: _LL1A:
# 906
 return 1;case 13U: _LL1B: _LL1C:
# 908
({struct Cyc_Int_pa_PrintArg_struct _tmp13A=({struct Cyc_Int_pa_PrintArg_struct _tmp494;_tmp494.tag=1U,({
# 910
unsigned long _tmp5DB=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp494.f1=_tmp5DB;});_tmp494;});void*_tmp138[1U];_tmp138[0]=& _tmp13A;({struct Cyc___cycFILE*_tmp5DD=Cyc_stderr;struct _dyneither_ptr _tmp5DC=({const char*_tmp139="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmp139,sizeof(char),58U);});Cyc_fprintf(_tmp5DD,_tmp5DC,_tag_dyneither(_tmp138,sizeof(void*),1U));});});
return 0;default: _LL1D: _LL1E:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL0:;}
# 915
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5DE=({const char*_tmp13B="some action didn't return!";_tag_dyneither(_tmp13B,sizeof(char),27U);});_tmp13C->f1=_tmp5DE;});_tmp13C;}));}
# 917
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5DF=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5DF;});
{int _tmp13D=lexstate;switch(_tmp13D){case 0U: _LL1: _LL2:
# 921 "buildlib.cyl"
({struct Cyc_List_List*_tmp5E2=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));({struct _dyneither_ptr*_tmp5E1=({struct _dyneither_ptr*_tmp13E=_cycalloc(sizeof(*_tmp13E));({struct _dyneither_ptr _tmp5E0=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp13E=_tmp5E0;});_tmp13E;});_tmp13F->hd=_tmp5E1;}),_tmp13F->tl=Cyc_snarfed_symbols;_tmp13F;});Cyc_snarfed_symbols=_tmp5E2;});
return 1;case 1U: _LL3: _LL4: {
# 924
struct _dyneither_ptr _tmp140=Cyc_Lexing_lexeme(lexbuf);
struct _dyneither_ptr t=_tmp140;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
({struct _dyneither_ptr _tmp5E3=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp140,0,(unsigned long)((t.curr - _tmp140.curr)/ sizeof(char)));Cyc_current_symbol=_tmp5E3;});
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
({struct Cyc_Buffer_t*_tmp5E4=Cyc_Buffer_create(255U);Cyc_specbuf=_tmp5E4;});
while(Cyc_block(lexbuf)){;}
# 933
Cyc_rename_current_symbol=0;{
struct _tuple24*user_def=({struct _tuple24*_tmp146=_cycalloc(sizeof(*_tmp146));({struct _dyneither_ptr*_tmp5E7=({struct _dyneither_ptr*_tmp144=_cycalloc(sizeof(*_tmp144));*_tmp144=Cyc_current_symbol;_tmp144;});_tmp146->f1=_tmp5E7;}),({
struct _dyneither_ptr*_tmp5E6=({struct _dyneither_ptr*_tmp145=_cycalloc(sizeof(*_tmp145));({struct _dyneither_ptr _tmp5E5=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmp145=_tmp5E5;});_tmp145;});_tmp146->f2=_tmp5E6;});_tmp146;});
({struct Cyc_List_List*_tmp5E9=({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));({struct _dyneither_ptr*_tmp5E8=({struct _dyneither_ptr*_tmp141=_cycalloc(sizeof(*_tmp141));*_tmp141=(struct _dyneither_ptr)Cyc_current_symbol;_tmp141;});_tmp142->hd=_tmp5E8;}),_tmp142->tl=Cyc_snarfed_symbols;_tmp142;});Cyc_snarfed_symbols=_tmp5E9;});
({struct Cyc_List_List*_tmp5EA=({struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=user_def,_tmp143->tl=Cyc_current_user_defs;_tmp143;});Cyc_current_user_defs=_tmp5EA;});
return 1;};}case 2U: _LL5: _LL6:
# 940
 return 1;case 3U: _LL7: _LL8:
# 942
 return 0;case 4U: _LL9: _LLA:
# 944
({void*_tmp147=0U;({struct Cyc___cycFILE*_tmp5EC=Cyc_stderr;struct _dyneither_ptr _tmp5EB=({const char*_tmp148="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmp148,sizeof(char),44U);});Cyc_fprintf(_tmp5EC,_tmp5EB,_tag_dyneither(_tmp147,sizeof(void*),0U));});});
# 946
return 0;case 5U: _LLB: _LLC:
# 948
({struct Cyc_Int_pa_PrintArg_struct _tmp14B=({struct Cyc_Int_pa_PrintArg_struct _tmp495;_tmp495.tag=1U,({
# 950
unsigned long _tmp5ED=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp495.f1=_tmp5ED;});_tmp495;});void*_tmp149[1U];_tmp149[0]=& _tmp14B;({struct Cyc___cycFILE*_tmp5EF=Cyc_stderr;struct _dyneither_ptr _tmp5EE=({const char*_tmp14A="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmp14A,sizeof(char),57U);});Cyc_fprintf(_tmp5EF,_tmp5EE,_tag_dyneither(_tmp149,sizeof(void*),1U));});});
return 0;default: _LLD: _LLE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL0:;}
# 955
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5F0=({const char*_tmp14C="some action didn't return!";_tag_dyneither(_tmp14C,sizeof(char),27U);});_tmp14D->f1=_tmp5F0;});_tmp14D;}));}
# 957
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5F1=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5F1;});
{int _tmp14E=lexstate;switch(_tmp14E){case 0U: _LL1: _LL2:
# 961 "buildlib.cyl"
({void*_tmp14F=0U;({struct _dyneither_ptr _tmp5F2=({const char*_tmp150="Warning: unclosed brace\n";_tag_dyneither(_tmp150,sizeof(char),25U);});Cyc_log(_tmp5F2,_tag_dyneither(_tmp14F,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 963
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2U: _LL5: _LL6:
# 968
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3U: _LL7: _LL8:
# 972
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 976
({struct Cyc_Buffer_t*_tmp5F3=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5F3,({const char*_tmp151="/*";_tag_dyneither(_tmp151,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 980
({struct Cyc_Buffer_t*_tmp5F4=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5F4,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE: {
# 983
struct _dyneither_ptr _tmp152=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol  && !Cyc_strcmp((struct _dyneither_ptr)_tmp152,(struct _dyneither_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp5F5=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5F5,*Cyc_add_user_prefix(({struct _dyneither_ptr*_tmp153=_cycalloc(sizeof(*_tmp153));*_tmp153=_tmp152;_tmp153;})));});else{
# 987
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tmp152);}
return 1;}case 7U: _LLF: _LL10:
# 990
({struct Cyc_Buffer_t*_tmp5F6=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp5F6,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default: _LL11: _LL12:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL0:;}
# 995
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5F7=({const char*_tmp154="some action didn't return!";_tag_dyneither(_tmp154,sizeof(char),27U);});_tmp155->f1=_tmp5F7;});_tmp155;}));}
# 997
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp5F8=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp5F8;});
{int _tmp156=lexstate;switch(_tmp156){case 0U: _LL1: _LL2:
# 995 "buildlib.cyl"
({void*_tmp157=0U;({struct _dyneither_ptr _tmp5F9=({const char*_tmp158="Warning: unclosed string\n";_tag_dyneither(_tmp158,sizeof(char),26U);});Cyc_log(_tmp5F9,_tag_dyneither(_tmp157,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 997
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2U: _LL5: _LL6:
# 999
({void*_tmp159=0U;({struct _dyneither_ptr _tmp5FA=({const char*_tmp15A="Warning: unclosed string\n";_tag_dyneither(_tmp15A,sizeof(char),26U);});Cyc_log(_tmp5FA,_tag_dyneither(_tmp159,sizeof(void*),0U));});});
({struct Cyc_Buffer_t*_tmp5FB=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5FB,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3U: _LL7: _LL8:
# 1003
({struct Cyc_Buffer_t*_tmp5FC=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5FC,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4U: _LL9: _LLA:
# 1006
({struct Cyc_Buffer_t*_tmp5FD=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5FD,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5U: _LLB: _LLC:
# 1009
({struct Cyc_Buffer_t*_tmp5FE=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5FE,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE:
# 1012
({struct Cyc_Buffer_t*_tmp5FF=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5FF,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7U: _LLF: _LL10:
# 1015
({struct Cyc_Buffer_t*_tmp600=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp600,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8U: _LL11: _LL12:
# 1018
({struct Cyc_Buffer_t*_tmp601=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp601,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL0:;}
# 1023
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp602=({const char*_tmp15B="some action didn't return!";_tag_dyneither(_tmp15B,sizeof(char),27U);});_tmp15C->f1=_tmp602;});_tmp15C;}));}
# 1025
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
({int _tmp603=Cyc_lex_engine(lexstate,lexbuf);lexstate=_tmp603;});
{int _tmp15D=lexstate;switch(_tmp15D){case 0U: _LL1: _LL2:
# 1023 "buildlib.cyl"
({void*_tmp15E=0U;({struct _dyneither_ptr _tmp604=({const char*_tmp15F="Warning: unclosed comment\n";_tag_dyneither(_tmp15F,sizeof(char),27U);});Cyc_log(_tmp604,_tag_dyneither(_tmp15E,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 1025
({struct Cyc_Buffer_t*_tmp605=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp605,({const char*_tmp160="*/";_tag_dyneither(_tmp160,sizeof(char),3U);}));});return 0;case 2U: _LL5: _LL6:
# 1027
({struct Cyc_Buffer_t*_tmp606=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp606,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL0:;}
# 1032
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp607=({const char*_tmp161="some action didn't return!";_tag_dyneither(_tmp161,sizeof(char),27U);});_tmp162->f1=_tmp607;});_tmp162;}));}
# 1034
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 1040 "buildlib.cyl"
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmp163=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmp164=_tmp163;struct Cyc_List_List*_tmp1B3;void*_tmp1B2;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B0;struct _dyneither_ptr*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1AE;struct _dyneither_ptr*_tmp1AD;void*_tmp1AC;void*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AA;int _tmp1A9;struct Cyc_Absyn_Exp*_tmp1A8;void**_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A6;void*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_List_List*_tmp18F;void*_tmp18E;switch(*((int*)_tmp164)){case 1U: _LL1: _tmp18E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL2: {
# 1044
struct _dyneither_ptr*_tmp165=(*Cyc_Absyn_binding2qvar(_tmp18E)).f2;
Cyc_add_target(_tmp165);
return;}case 3U: _LL3: _tmp18F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL4:
# 1048
 for(0;_tmp18F != 0;_tmp18F=_tmp18F->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp18F->hd,dep);}
# 1051
return;case 23U: _LL5: _tmp191=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp190=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL6:
# 1053
 _tmp193=_tmp191;_tmp192=_tmp190;goto _LL8;case 9U: _LL7: _tmp193=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp192=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL8:
# 1055
 _tmp195=_tmp193;_tmp194=_tmp192;goto _LLA;case 4U: _LL9: _tmp195=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp194=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LLA:
# 1057
 Cyc_scan_exp(_tmp195,dep);
Cyc_scan_exp(_tmp194,dep);
return;case 41U: _LLB: _tmp196=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LLC:
 _tmp197=_tmp196;goto _LLE;case 20U: _LLD: _tmp197=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LLE:
# 1062
 _tmp198=_tmp197;goto _LL10;case 18U: _LLF: _tmp198=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL10:
# 1064
 _tmp199=_tmp198;goto _LL12;case 15U: _LL11: _tmp199=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL12:
# 1066
 _tmp19A=_tmp199;goto _LL14;case 5U: _LL13: _tmp19A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL14:
# 1068
 Cyc_scan_exp(_tmp19A,dep);
return;case 6U: _LL15: _tmp19D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp19C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp19B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL16:
# 1071
 Cyc_scan_exp(_tmp19D,dep);
Cyc_scan_exp(_tmp19C,dep);
Cyc_scan_exp(_tmp19B,dep);
return;case 7U: _LL17: _tmp19F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp19E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL18:
 _tmp1A1=_tmp19F;_tmp1A0=_tmp19E;goto _LL1A;case 8U: _LL19: _tmp1A1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1A0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL1A:
# 1077
 Cyc_scan_exp(_tmp1A1,dep);
Cyc_scan_exp(_tmp1A0,dep);
return;case 10U: _LL1B: _tmp1A3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1A2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL1C:
# 1081
 Cyc_scan_exp(_tmp1A3,dep);
for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1A2->hd,dep);}
# 1085
return;case 14U: _LL1D: _tmp1A5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1A4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL1E:
# 1087
 Cyc_scan_type(_tmp1A5,dep);
Cyc_scan_exp(_tmp1A4,dep);
return;case 34U: _LL1F: _tmp1A9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).is_calloc;_tmp1A8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).rgn;_tmp1A7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).elt_type;_tmp1A6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).num_elts;_LL20:
# 1091
 if(_tmp1A8 != 0)Cyc_scan_exp(_tmp1A8,dep);
if(_tmp1A7 != 0)Cyc_scan_type(*_tmp1A7,dep);
Cyc_scan_exp(_tmp1A6,dep);
return;case 38U: _LL21: _tmp1AA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL22:
# 1096
 Cyc_scan_exp(_tmp1AA,dep);return;case 39U: _LL23: _tmp1AB=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL24:
 _tmp1AC=_tmp1AB;goto _LL26;case 17U: _LL25: _tmp1AC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL26:
# 1099
 Cyc_scan_type(_tmp1AC,dep);
return;case 21U: _LL27: _tmp1AE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1AD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL28:
# 1102
 _tmp1B0=_tmp1AE;_tmp1AF=_tmp1AD;goto _LL2A;case 22U: _LL29: _tmp1B0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1AF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL2A:
# 1104
 Cyc_scan_exp(_tmp1B0,dep);
Cyc_add_target(_tmp1AF);
return;case 19U: _LL2B: _tmp1B2=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1B1=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL2C:
# 1108
 Cyc_scan_type(_tmp1B2,dep);
# 1110
{void*_tmp166=(void*)((struct Cyc_List_List*)_check_null(_tmp1B1))->hd;void*_tmp167=_tmp166;struct _dyneither_ptr*_tmp168;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp167)->tag == 0U){_LL56: _tmp168=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp167)->f1;_LL57:
 Cyc_add_target(_tmp168);goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 1114
return;case 0U: _LL2D: _LL2E:
# 1116
 return;case 36U: _LL2F: _tmp1B3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL30:
# 1118
 for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){
struct _tuple28*_tmp169=(struct _tuple28*)_tmp1B3->hd;struct _tuple28*_tmp16A=_tmp169;struct Cyc_Absyn_Exp*_tmp16B;_LL5B: _tmp16B=_tmp16A->f2;_LL5C:;
Cyc_scan_exp(_tmp16B,dep);}
# 1122
return;case 40U: _LL31: _LL32:
 return;case 2U: _LL33: _LL34:
# 1125
({void*_tmp16C=0U;({struct Cyc___cycFILE*_tmp609=Cyc_stderr;struct _dyneither_ptr _tmp608=({const char*_tmp16D="Error: unexpected Pragma_e\n";_tag_dyneither(_tmp16D,sizeof(char),28U);});Cyc_fprintf(_tmp609,_tmp608,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});
 exit(1);return;case 35U: _LL35: _LL36:
# 1128
({void*_tmp16E=0U;({struct Cyc___cycFILE*_tmp60B=Cyc_stderr;struct _dyneither_ptr _tmp60A=({const char*_tmp16F="Error: unexpected Swap_e\n";_tag_dyneither(_tmp16F,sizeof(char),26U);});Cyc_fprintf(_tmp60B,_tmp60A,_tag_dyneither(_tmp16E,sizeof(void*),0U));});});
 exit(1);return;case 37U: _LL37: _LL38:
# 1131
({void*_tmp170=0U;({struct Cyc___cycFILE*_tmp60D=Cyc_stderr;struct _dyneither_ptr _tmp60C=({const char*_tmp171="Error: unexpected Stmt_e\n";_tag_dyneither(_tmp171,sizeof(char),26U);});Cyc_fprintf(_tmp60D,_tmp60C,_tag_dyneither(_tmp170,sizeof(void*),0U));});});
 exit(1);return;case 11U: _LL39: _LL3A:
# 1134
({void*_tmp172=0U;({struct Cyc___cycFILE*_tmp60F=Cyc_stderr;struct _dyneither_ptr _tmp60E=({const char*_tmp173="Error: unexpected Throw_e\n";_tag_dyneither(_tmp173,sizeof(char),27U);});Cyc_fprintf(_tmp60F,_tmp60E,_tag_dyneither(_tmp172,sizeof(void*),0U));});});
 exit(1);return;case 12U: _LL3B: _LL3C:
# 1137
({void*_tmp174=0U;({struct Cyc___cycFILE*_tmp611=Cyc_stderr;struct _dyneither_ptr _tmp610=({const char*_tmp175="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmp175,sizeof(char),35U);});Cyc_fprintf(_tmp611,_tmp610,_tag_dyneither(_tmp174,sizeof(void*),0U));});});
 exit(1);return;case 13U: _LL3D: _LL3E:
# 1140
({void*_tmp176=0U;({struct Cyc___cycFILE*_tmp613=Cyc_stderr;struct _dyneither_ptr _tmp612=({const char*_tmp177="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmp177,sizeof(char),33U);});Cyc_fprintf(_tmp613,_tmp612,_tag_dyneither(_tmp176,sizeof(void*),0U));});});
 exit(1);return;case 16U: _LL3F: _LL40:
# 1143
({void*_tmp178=0U;({struct Cyc___cycFILE*_tmp615=Cyc_stderr;struct _dyneither_ptr _tmp614=({const char*_tmp179="Error: unexpected New_e\n";_tag_dyneither(_tmp179,sizeof(char),25U);});Cyc_fprintf(_tmp615,_tmp614,_tag_dyneither(_tmp178,sizeof(void*),0U));});});
 exit(1);return;case 24U: _LL41: _LL42:
# 1146
({void*_tmp17A=0U;({struct Cyc___cycFILE*_tmp617=Cyc_stderr;struct _dyneither_ptr _tmp616=({const char*_tmp17B="Error: unexpected Tuple_e\n";_tag_dyneither(_tmp17B,sizeof(char),27U);});Cyc_fprintf(_tmp617,_tmp616,_tag_dyneither(_tmp17A,sizeof(void*),0U));});});
 exit(1);return;case 25U: _LL43: _LL44:
# 1149
({void*_tmp17C=0U;({struct Cyc___cycFILE*_tmp619=Cyc_stderr;struct _dyneither_ptr _tmp618=({const char*_tmp17D="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmp17D,sizeof(char),33U);});Cyc_fprintf(_tmp619,_tmp618,_tag_dyneither(_tmp17C,sizeof(void*),0U));});});
 exit(1);return;case 26U: _LL45: _LL46:
# 1152
({void*_tmp17E=0U;({struct Cyc___cycFILE*_tmp61B=Cyc_stderr;struct _dyneither_ptr _tmp61A=({const char*_tmp17F="Error: unexpected Array_e\n";_tag_dyneither(_tmp17F,sizeof(char),27U);});Cyc_fprintf(_tmp61B,_tmp61A,_tag_dyneither(_tmp17E,sizeof(void*),0U));});});
 exit(1);return;case 27U: _LL47: _LL48:
# 1155
({void*_tmp180=0U;({struct Cyc___cycFILE*_tmp61D=Cyc_stderr;struct _dyneither_ptr _tmp61C=({const char*_tmp181="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmp181,sizeof(char),35U);});Cyc_fprintf(_tmp61D,_tmp61C,_tag_dyneither(_tmp180,sizeof(void*),0U));});});
 exit(1);return;case 28U: _LL49: _LL4A:
# 1158
({void*_tmp182=0U;({struct Cyc___cycFILE*_tmp61F=Cyc_stderr;struct _dyneither_ptr _tmp61E=({const char*_tmp183="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmp183,sizeof(char),41U);});Cyc_fprintf(_tmp61F,_tmp61E,_tag_dyneither(_tmp182,sizeof(void*),0U));});});
 exit(1);return;case 29U: _LL4B: _LL4C:
# 1161
({void*_tmp184=0U;({struct Cyc___cycFILE*_tmp621=Cyc_stderr;struct _dyneither_ptr _tmp620=({const char*_tmp185="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmp185,sizeof(char),31U);});Cyc_fprintf(_tmp621,_tmp620,_tag_dyneither(_tmp184,sizeof(void*),0U));});});
 exit(1);return;case 30U: _LL4D: _LL4E:
# 1164
({void*_tmp186=0U;({struct Cyc___cycFILE*_tmp623=Cyc_stderr;struct _dyneither_ptr _tmp622=({const char*_tmp187="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmp187,sizeof(char),32U);});Cyc_fprintf(_tmp623,_tmp622,_tag_dyneither(_tmp186,sizeof(void*),0U));});});
 exit(1);return;case 31U: _LL4F: _LL50:
# 1167
({void*_tmp188=0U;({struct Cyc___cycFILE*_tmp625=Cyc_stderr;struct _dyneither_ptr _tmp624=({const char*_tmp189="Error: unexpected Datatype_e\n";_tag_dyneither(_tmp189,sizeof(char),30U);});Cyc_fprintf(_tmp625,_tmp624,_tag_dyneither(_tmp188,sizeof(void*),0U));});});
 exit(1);return;case 32U: _LL51: _LL52:
# 1170
({void*_tmp18A=0U;({struct Cyc___cycFILE*_tmp627=Cyc_stderr;struct _dyneither_ptr _tmp626=({const char*_tmp18B="Error: unexpected Enum_e\n";_tag_dyneither(_tmp18B,sizeof(char),26U);});Cyc_fprintf(_tmp627,_tmp626,_tag_dyneither(_tmp18A,sizeof(void*),0U));});});
 exit(1);return;default: _LL53: _LL54:
# 1173
({void*_tmp18C=0U;({struct Cyc___cycFILE*_tmp629=Cyc_stderr;struct _dyneither_ptr _tmp628=({const char*_tmp18D="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp18D,sizeof(char),30U);});Cyc_fprintf(_tmp629,_tmp628,_tag_dyneither(_tmp18C,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1178
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 1183
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp1B4=t;struct Cyc_Absyn_Datatypedecl*_tmp1F0;struct Cyc_Absyn_Enumdecl*_tmp1EF;struct Cyc_Absyn_Aggrdecl*_tmp1EE;struct _dyneither_ptr*_tmp1ED;struct _dyneither_ptr*_tmp1EC;union Cyc_Absyn_AggrInfoU _tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_Absyn_FnInfo _tmp1E9;struct Cyc_Absyn_Exp*_tmp1E8;void*_tmp1E7;struct Cyc_Absyn_Exp*_tmp1E6;union Cyc_Absyn_Constraint*_tmp1E5;struct Cyc_Absyn_PtrInfo _tmp1E4;switch(*((int*)_tmp1B4)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 7U: _LL7: _LL8:
# 1190
 return;case 5U: _LL9: _tmp1E4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B4)->f1;_LLA:
# 1193
 Cyc_scan_type(_tmp1E4.elt_typ,dep);
return;case 8U: _LLB: _tmp1E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).elt_type;_tmp1E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).num_elts;_tmp1E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).zero_term;_LLC:
# 1196
 Cyc_scan_type(_tmp1E7,dep);
Cyc_scan_exp_opt(_tmp1E6,dep);
return;case 27U: _LLD: _tmp1E8=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1B4)->f1;_LLE:
# 1200
 Cyc_scan_exp(_tmp1E8,dep);
return;case 9U: _LLF: _tmp1E9=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1;_LL10:
# 1203
 Cyc_scan_type(_tmp1E9.ret_typ,dep);
{struct Cyc_List_List*_tmp1B5=_tmp1E9.args;for(0;_tmp1B5 != 0;_tmp1B5=_tmp1B5->tl){
struct _tuple9*_tmp1B6=(struct _tuple9*)_tmp1B5->hd;struct _tuple9*_tmp1B7=_tmp1B6;void*_tmp1B8;_LL40: _tmp1B8=_tmp1B7->f3;_LL41:;
Cyc_scan_type(_tmp1B8,dep);}}
# 1208
if(_tmp1E9.cyc_varargs != 0)
Cyc_scan_type((_tmp1E9.cyc_varargs)->type,dep);
return;case 12U: _LL11: _tmp1EA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B4)->f2;_LL12:
# 1212
 for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp1EA->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp1EA->hd)->width,dep);}
# 1216
return;case 14U: _LL13: _LL14:
# 1218
 return;case 11U: _LL15: _tmp1EB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1B4)->f1).aggr_info;_LL16: {
# 1220
struct _tuple11 _tmp1B9=Cyc_Absyn_aggr_kinded_name(_tmp1EB);struct _tuple11 _tmp1BA=_tmp1B9;struct _dyneither_ptr*_tmp1BB;_LL43: _tmp1BB=(_tmp1BA.f2)->f2;_LL44:;
_tmp1EC=_tmp1BB;goto _LL18;}case 13U: _LL17: _tmp1EC=(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp1B4)->f1)->f2;_LL18:
# 1223
 _tmp1ED=_tmp1EC;goto _LL1A;case 17U: _LL19: _tmp1ED=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B4)->f1)->f2;_LL1A:
# 1225
 Cyc_add_target(_tmp1ED);
return;case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)){case 0U: _LL1B: _tmp1EE=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)->f1;_LL1C:
# 1229
({struct Cyc_Absyn_Decl*_tmp62A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->tag=5U,_tmp1BC->f1=_tmp1EE;_tmp1BC;}),0U);Cyc_scan_decl(_tmp62A,dep);});{
struct _tuple1*_tmp1BD=_tmp1EE->name;struct _tuple1*_tmp1BE=_tmp1BD;struct _dyneither_ptr*_tmp1BF;_LL46: _tmp1BF=_tmp1BE->f2;_LL47:;
Cyc_add_target(_tmp1BF);
return;};case 1U: _LL1D: _tmp1EF=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)->f1;_LL1E:
# 1235
({struct Cyc_Absyn_Decl*_tmp62B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->tag=7U,_tmp1C0->f1=_tmp1EF;_tmp1C0;}),0U);Cyc_scan_decl(_tmp62B,dep);});{
struct _tuple1*_tmp1C1=_tmp1EF->name;struct _tuple1*_tmp1C2=_tmp1C1;struct _dyneither_ptr*_tmp1C3;_LL49: _tmp1C3=_tmp1C2->f2;_LL4A:;
Cyc_add_target(_tmp1C3);
return;};default: _LL1F: _tmp1F0=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)->f1;_LL20:
# 1241
({void*_tmp1C4=0U;({struct Cyc___cycFILE*_tmp62D=Cyc_stderr;struct _dyneither_ptr _tmp62C=({const char*_tmp1C5="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp1C5,sizeof(char),40U);});Cyc_fprintf(_tmp62D,_tmp62C,_tag_dyneither(_tmp1C4,sizeof(void*),0U));});});
 exit(1);return;}case 1U: _LL21: _LL22:
# 1244
({void*_tmp1C6=0U;({struct Cyc___cycFILE*_tmp62F=Cyc_stderr;struct _dyneither_ptr _tmp62E=({const char*_tmp1C7="Error: unexpected Evar\n";_tag_dyneither(_tmp1C7,sizeof(char),24U);});Cyc_fprintf(_tmp62F,_tmp62E,_tag_dyneither(_tmp1C6,sizeof(void*),0U));});});
 exit(1);return;case 2U: _LL23: _LL24:
# 1247
({void*_tmp1C8=0U;({struct Cyc___cycFILE*_tmp631=Cyc_stderr;struct _dyneither_ptr _tmp630=({const char*_tmp1C9="Error: unexpected VarType\n";_tag_dyneither(_tmp1C9,sizeof(char),27U);});Cyc_fprintf(_tmp631,_tmp630,_tag_dyneither(_tmp1C8,sizeof(void*),0U));});});
 exit(1);return;case 3U: _LL25: _LL26:
# 1250
({void*_tmp1CA=0U;({struct Cyc___cycFILE*_tmp633=Cyc_stderr;struct _dyneither_ptr _tmp632=({const char*_tmp1CB="Error: unexpected DatatypeType\n";_tag_dyneither(_tmp1CB,sizeof(char),32U);});Cyc_fprintf(_tmp633,_tmp632,_tag_dyneither(_tmp1CA,sizeof(void*),0U));});});
 exit(1);return;case 4U: _LL27: _LL28:
# 1253
({void*_tmp1CC=0U;({struct Cyc___cycFILE*_tmp635=Cyc_stderr;struct _dyneither_ptr _tmp634=({const char*_tmp1CD="Error: unexpected DatatypeFieldType\n";_tag_dyneither(_tmp1CD,sizeof(char),37U);});Cyc_fprintf(_tmp635,_tmp634,_tag_dyneither(_tmp1CC,sizeof(void*),0U));});});
 exit(1);return;case 10U: _LL29: _LL2A:
# 1256
({void*_tmp1CE=0U;({struct Cyc___cycFILE*_tmp637=Cyc_stderr;struct _dyneither_ptr _tmp636=({const char*_tmp1CF="Error: unexpected TupleType\n";_tag_dyneither(_tmp1CF,sizeof(char),29U);});Cyc_fprintf(_tmp637,_tmp636,_tag_dyneither(_tmp1CE,sizeof(void*),0U));});});
 exit(1);return;case 15U: _LL2B: _LL2C:
# 1259
({void*_tmp1D0=0U;({struct Cyc___cycFILE*_tmp639=Cyc_stderr;struct _dyneither_ptr _tmp638=({const char*_tmp1D1="Error: unexpected RgnHandleType\n";_tag_dyneither(_tmp1D1,sizeof(char),33U);});Cyc_fprintf(_tmp639,_tmp638,_tag_dyneither(_tmp1D0,sizeof(void*),0U));});});
 exit(1);return;case 16U: _LL2D: _LL2E:
# 1262
({void*_tmp1D2=0U;({struct Cyc___cycFILE*_tmp63B=Cyc_stderr;struct _dyneither_ptr _tmp63A=({const char*_tmp1D3="Error: unexpected DynRgnType\n";_tag_dyneither(_tmp1D3,sizeof(char),30U);});Cyc_fprintf(_tmp63B,_tmp63A,_tag_dyneither(_tmp1D2,sizeof(void*),0U));});});
 exit(1);return;case 20U: _LL2F: _LL30:
# 1265
({void*_tmp1D4=0U;({struct Cyc___cycFILE*_tmp63D=Cyc_stderr;struct _dyneither_ptr _tmp63C=({const char*_tmp1D5="Error: unexpected HeapRgn\n";_tag_dyneither(_tmp1D5,sizeof(char),27U);});Cyc_fprintf(_tmp63D,_tmp63C,_tag_dyneither(_tmp1D4,sizeof(void*),0U));});});
 exit(1);return;case 21U: _LL31: _LL32:
# 1268
({void*_tmp1D6=0U;({struct Cyc___cycFILE*_tmp63F=Cyc_stderr;struct _dyneither_ptr _tmp63E=({const char*_tmp1D7="Error: unexpected UniqueRgn\n";_tag_dyneither(_tmp1D7,sizeof(char),29U);});Cyc_fprintf(_tmp63F,_tmp63E,_tag_dyneither(_tmp1D6,sizeof(void*),0U));});});
 exit(1);return;case 22U: _LL33: _LL34:
# 1271
({void*_tmp1D8=0U;({struct Cyc___cycFILE*_tmp641=Cyc_stderr;struct _dyneither_ptr _tmp640=({const char*_tmp1D9="Error: unexpected RefCntRgn\n";_tag_dyneither(_tmp1D9,sizeof(char),29U);});Cyc_fprintf(_tmp641,_tmp640,_tag_dyneither(_tmp1D8,sizeof(void*),0U));});});
 exit(1);return;case 23U: _LL35: _LL36:
# 1274
({void*_tmp1DA=0U;({struct Cyc___cycFILE*_tmp643=Cyc_stderr;struct _dyneither_ptr _tmp642=({const char*_tmp1DB="Error: unexpected AccessEff\n";_tag_dyneither(_tmp1DB,sizeof(char),29U);});Cyc_fprintf(_tmp643,_tmp642,_tag_dyneither(_tmp1DA,sizeof(void*),0U));});});
 exit(1);return;case 24U: _LL37: _LL38:
# 1277
({void*_tmp1DC=0U;({struct Cyc___cycFILE*_tmp645=Cyc_stderr;struct _dyneither_ptr _tmp644=({const char*_tmp1DD="Error: unexpected JoinEff\n";_tag_dyneither(_tmp1DD,sizeof(char),27U);});Cyc_fprintf(_tmp645,_tmp644,_tag_dyneither(_tmp1DC,sizeof(void*),0U));});});
 exit(1);return;case 25U: _LL39: _LL3A:
# 1280
({void*_tmp1DE=0U;({struct Cyc___cycFILE*_tmp647=Cyc_stderr;struct _dyneither_ptr _tmp646=({const char*_tmp1DF="Error: unexpected RgnsEff\n";_tag_dyneither(_tmp1DF,sizeof(char),27U);});Cyc_fprintf(_tmp647,_tmp646,_tag_dyneither(_tmp1DE,sizeof(void*),0U));});});
 exit(1);return;case 19U: _LL3B: _LL3C:
# 1283
({void*_tmp1E0=0U;({struct Cyc___cycFILE*_tmp649=Cyc_stderr;struct _dyneither_ptr _tmp648=({const char*_tmp1E1="Error: unexpected tag_t\n";_tag_dyneither(_tmp1E1,sizeof(char),25U);});Cyc_fprintf(_tmp649,_tmp648,_tag_dyneither(_tmp1E0,sizeof(void*),0U));});});
 exit(1);return;default: _LL3D: _LL3E:
# 1286
({void*_tmp1E2=0U;({struct Cyc___cycFILE*_tmp64B=Cyc_stderr;struct _dyneither_ptr _tmp64A=({const char*_tmp1E3="Error: unexpected valueof_t\n";_tag_dyneither(_tmp1E3,sizeof(char),29U);});Cyc_fprintf(_tmp64B,_tmp64A,_tag_dyneither(_tmp1E2,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1291
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp1F1=Cyc_current_targets;
struct _dyneither_ptr*_tmp1F2=Cyc_current_source;
({struct Cyc_Set_Set**_tmp64D=({struct Cyc_Set_Set**_tmp1F3=_cycalloc(sizeof(*_tmp1F3));({struct Cyc_Set_Set*_tmp64C=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp1F3=_tmp64C;});_tmp1F3;});Cyc_current_targets=_tmp64D;});
{void*_tmp1F4=d->r;void*_tmp1F5=_tmp1F4;struct Cyc_Absyn_Typedefdecl*_tmp229;struct Cyc_Absyn_Enumdecl*_tmp228;struct Cyc_Absyn_Aggrdecl*_tmp227;struct Cyc_Absyn_Fndecl*_tmp226;struct Cyc_Absyn_Vardecl*_tmp225;switch(*((int*)_tmp1F5)){case 0U: _LL1: _tmp225=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LL2: {
# 1297
struct _tuple1*_tmp1F6=_tmp225->name;struct _tuple1*_tmp1F7=_tmp1F6;struct _dyneither_ptr*_tmp1F8;_LL20: _tmp1F8=_tmp1F7->f2;_LL21:;
Cyc_current_source=_tmp1F8;
Cyc_scan_type(_tmp225->type,dep);
Cyc_scan_exp_opt(_tmp225->initializer,dep);
goto _LL0;}case 1U: _LL3: _tmp226=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LL4: {
# 1303
struct _tuple1*_tmp1F9=_tmp226->name;struct _tuple1*_tmp1FA=_tmp1F9;struct _dyneither_ptr*_tmp201;_LL23: _tmp201=_tmp1FA->f2;_LL24:;
Cyc_current_source=_tmp201;
Cyc_scan_type(_tmp226->ret_type,dep);
{struct Cyc_List_List*_tmp1FB=_tmp226->args;for(0;_tmp1FB != 0;_tmp1FB=_tmp1FB->tl){
struct _tuple9*_tmp1FC=(struct _tuple9*)_tmp1FB->hd;struct _tuple9*_tmp1FD=_tmp1FC;void*_tmp1FE;_LL26: _tmp1FE=_tmp1FD->f3;_LL27:;
Cyc_scan_type(_tmp1FE,dep);}}
# 1310
if(_tmp226->cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp226->cyc_varargs))->type,dep);
if(_tmp226->is_inline)
({void*_tmp1FF=0U;({struct Cyc___cycFILE*_tmp64F=Cyc_stderr;struct _dyneither_ptr _tmp64E=({const char*_tmp200="Warning: ignoring inline function\n";_tag_dyneither(_tmp200,sizeof(char),35U);});Cyc_fprintf(_tmp64F,_tmp64E,_tag_dyneither(_tmp1FF,sizeof(void*),0U));});});
# 1315
goto _LL0;}case 5U: _LL5: _tmp227=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LL6: {
# 1317
struct _tuple1*_tmp202=_tmp227->name;struct _tuple1*_tmp203=_tmp202;struct _dyneither_ptr*_tmp207;_LL29: _tmp207=_tmp203->f2;_LL2A:;
Cyc_current_source=_tmp207;
if((unsigned int)_tmp227->impl){
{struct Cyc_List_List*_tmp204=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp227->impl))->fields;for(0;_tmp204 != 0;_tmp204=_tmp204->tl){
struct Cyc_Absyn_Aggrfield*_tmp205=(struct Cyc_Absyn_Aggrfield*)_tmp204->hd;
Cyc_scan_type(_tmp205->type,dep);
Cyc_scan_exp_opt(_tmp205->width,dep);}}{
# 1327
struct Cyc_List_List*_tmp206=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp227->impl))->fields;for(0;_tmp206 != 0;_tmp206=_tmp206->tl){;}};}
# 1331
goto _LL0;}case 7U: _LL7: _tmp228=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LL8: {
# 1333
struct _tuple1*_tmp208=_tmp228->name;struct _tuple1*_tmp209=_tmp208;struct _dyneither_ptr*_tmp20D;_LL2C: _tmp20D=_tmp209->f2;_LL2D:;
Cyc_current_source=_tmp20D;
if((unsigned int)_tmp228->fields){
{struct Cyc_List_List*_tmp20A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp228->fields))->v;for(0;_tmp20A != 0;_tmp20A=_tmp20A->tl){
struct Cyc_Absyn_Enumfield*_tmp20B=(struct Cyc_Absyn_Enumfield*)_tmp20A->hd;
Cyc_scan_exp_opt(_tmp20B->tag,dep);}}{
# 1342
struct Cyc_List_List*_tmp20C=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp228->fields))->v;for(0;_tmp20C != 0;_tmp20C=_tmp20C->tl){;}};}
# 1346
goto _LL0;}case 8U: _LL9: _tmp229=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LLA: {
# 1348
struct _tuple1*_tmp20E=_tmp229->name;struct _tuple1*_tmp20F=_tmp20E;struct _dyneither_ptr*_tmp210;_LL2F: _tmp210=_tmp20F->f2;_LL30:;
Cyc_current_source=_tmp210;
if((unsigned int)_tmp229->defn)
Cyc_scan_type((void*)_check_null(_tmp229->defn),dep);
goto _LL0;}case 4U: _LLB: _LLC:
# 1354
({void*_tmp211=0U;({struct Cyc___cycFILE*_tmp651=Cyc_stderr;struct _dyneither_ptr _tmp650=({const char*_tmp212="Error: unexpected region declaration";_tag_dyneither(_tmp212,sizeof(char),37U);});Cyc_fprintf(_tmp651,_tmp650,_tag_dyneither(_tmp211,sizeof(void*),0U));});});
 exit(1);case 13U: _LLD: _LLE:
# 1357
({void*_tmp213=0U;({struct Cyc___cycFILE*_tmp653=Cyc_stderr;struct _dyneither_ptr _tmp652=({const char*_tmp214="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp214,sizeof(char),38U);});Cyc_fprintf(_tmp653,_tmp652,_tag_dyneither(_tmp213,sizeof(void*),0U));});});
 exit(1);case 14U: _LLF: _LL10:
# 1360
({void*_tmp215=0U;({struct Cyc___cycFILE*_tmp655=Cyc_stderr;struct _dyneither_ptr _tmp654=({const char*_tmp216="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp216,sizeof(char),39U);});Cyc_fprintf(_tmp655,_tmp654,_tag_dyneither(_tmp215,sizeof(void*),0U));});});
 exit(1);case 2U: _LL11: _LL12:
# 1363
({void*_tmp217=0U;({struct Cyc___cycFILE*_tmp657=Cyc_stderr;struct _dyneither_ptr _tmp656=({const char*_tmp218="Error: unexpected let declaration\n";_tag_dyneither(_tmp218,sizeof(char),35U);});Cyc_fprintf(_tmp657,_tmp656,_tag_dyneither(_tmp217,sizeof(void*),0U));});});
 exit(1);case 6U: _LL13: _LL14:
# 1366
({void*_tmp219=0U;({struct Cyc___cycFILE*_tmp659=Cyc_stderr;struct _dyneither_ptr _tmp658=({const char*_tmp21A="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp21A,sizeof(char),40U);});Cyc_fprintf(_tmp659,_tmp658,_tag_dyneither(_tmp219,sizeof(void*),0U));});});
 exit(1);case 3U: _LL15: _LL16:
# 1369
({void*_tmp21B=0U;({struct Cyc___cycFILE*_tmp65B=Cyc_stderr;struct _dyneither_ptr _tmp65A=({const char*_tmp21C="Error: unexpected let declaration\n";_tag_dyneither(_tmp21C,sizeof(char),35U);});Cyc_fprintf(_tmp65B,_tmp65A,_tag_dyneither(_tmp21B,sizeof(void*),0U));});});
 exit(1);case 9U: _LL17: _LL18:
# 1372
({void*_tmp21D=0U;({struct Cyc___cycFILE*_tmp65D=Cyc_stderr;struct _dyneither_ptr _tmp65C=({const char*_tmp21E="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp21E,sizeof(char),41U);});Cyc_fprintf(_tmp65D,_tmp65C,_tag_dyneither(_tmp21D,sizeof(void*),0U));});});
 exit(1);case 10U: _LL19: _LL1A:
# 1375
({void*_tmp21F=0U;({struct Cyc___cycFILE*_tmp65F=Cyc_stderr;struct _dyneither_ptr _tmp65E=({const char*_tmp220="Error: unexpected using declaration\n";_tag_dyneither(_tmp220,sizeof(char),37U);});Cyc_fprintf(_tmp65F,_tmp65E,_tag_dyneither(_tmp21F,sizeof(void*),0U));});});
 exit(1);case 11U: _LL1B: _LL1C:
# 1378
({void*_tmp221=0U;({struct Cyc___cycFILE*_tmp661=Cyc_stderr;struct _dyneither_ptr _tmp660=({const char*_tmp222="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp222,sizeof(char),42U);});Cyc_fprintf(_tmp661,_tmp660,_tag_dyneither(_tmp221,sizeof(void*),0U));});});
 exit(1);default: _LL1D: _LL1E:
# 1381
({void*_tmp223=0U;({struct Cyc___cycFILE*_tmp663=Cyc_stderr;struct _dyneither_ptr _tmp662=({const char*_tmp224="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp224,sizeof(char),50U);});Cyc_fprintf(_tmp663,_tmp662,_tag_dyneither(_tmp223,sizeof(void*),0U));});});
 exit(1);}_LL0:;}{
# 1389
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp22A=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp22B;_push_handler(& _tmp22B);{int _tmp22D=0;if(setjmp(_tmp22B.handler))_tmp22D=1;if(!_tmp22D){
({struct Cyc_Set_Set*_tmp664=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp22A);old=_tmp664;});;_pop_handler();}else{void*_tmp22C=(void*)_exn_thrown;void*_tmp22E=_tmp22C;void*_tmp22F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp22E)->tag == Cyc_Core_Not_found){_LL32: _LL33:
# 1394
({struct Cyc_Set_Set*_tmp665=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);old=_tmp665;});goto _LL31;}else{_LL34: _tmp22F=_tmp22E;_LL35:(int)_rethrow(_tmp22F);}_LL31:;}};}{
# 1396
struct Cyc_Set_Set*_tmp230=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp22A,_tmp230);
# 1399
Cyc_current_targets=_tmp1F1;
Cyc_current_source=_tmp1F2;};};}
# 1403
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1407
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp231;_push_handler(& _tmp231);{int _tmp233=0;if(setjmp(_tmp231.handler))_tmp233=1;if(!_tmp233){{struct Cyc_Set_Set*_tmp234=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0U);return _tmp234;};_pop_handler();}else{void*_tmp232=(void*)_exn_thrown;void*_tmp235=_tmp232;void*_tmp236;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp235)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 1410
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL3: _tmp236=_tmp235;_LL4:(int)_rethrow(_tmp236);}_LL0:;}};}
# 1414
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1424 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
({struct Cyc_Set_Set*_tmp666=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);curr=_tmp666;});}{
# 1429
struct Cyc_Set_Set*_tmp237=curr;
# 1431
struct _dyneither_ptr*_tmp238=({struct _dyneither_ptr*_tmp23C=_cycalloc(sizeof(*_tmp23C));({struct _dyneither_ptr _tmp667=({const char*_tmp23B="";_tag_dyneither(_tmp23B,sizeof(char),1U);});*_tmp23C=_tmp667;});_tmp23C;});
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp237)> 0){
struct Cyc_Set_Set*_tmp239=emptyset;
struct Cyc_Iter_Iter _tmp23A=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp237);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp23A,& _tmp238)){
({struct Cyc_Set_Set*_tmp669=({struct Cyc_Set_Set*_tmp668=_tmp239;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp668,Cyc_find(t,_tmp238));});_tmp239=_tmp669;});}
({struct Cyc_Set_Set*_tmp66A=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp239,curr);_tmp237=_tmp66A;});
({struct Cyc_Set_Set*_tmp66B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp237);curr=_tmp66B;});}
# 1440
return curr;};}
# 1443
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0U,Cyc_GATHER  = 1U,Cyc_GATHERSCRIPT  = 2U,Cyc_FINISH  = 3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return Cyc_mode == Cyc_GATHER  || Cyc_mode == Cyc_GATHERSCRIPT;}
# 1449
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1453
if(Cyc_script_file == 0){
({void*_tmp23D=0U;({struct Cyc___cycFILE*_tmp66D=Cyc_stderr;struct _dyneither_ptr _tmp66C=({const char*_tmp23E="Internal error: script file is NULL\n";_tag_dyneither(_tmp23E,sizeof(char),37U);});Cyc_fprintf(_tmp66D,_tmp66C,_tag_dyneither(_tmp23D,sizeof(void*),0U));});});
 exit(1);}
# 1457
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1460
int Cyc_force_directory(struct _dyneither_ptr d){
if(Cyc_mode == Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp241=({struct Cyc_String_pa_PrintArg_struct _tmp497;_tmp497.tag=0U,_tmp497.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp497;});struct Cyc_String_pa_PrintArg_struct _tmp242=({struct Cyc_String_pa_PrintArg_struct _tmp496;_tmp496.tag=0U,_tmp496.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp496;});void*_tmp23F[2U];_tmp23F[0]=& _tmp241,_tmp23F[1]=& _tmp242;({struct _dyneither_ptr _tmp66E=({const char*_tmp240="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp240,sizeof(char),36U);});Cyc_prscript(_tmp66E,_tag_dyneither(_tmp23F,sizeof(void*),2U));});});else{
# 1467
int _tmp243=({void*_tmp247=0U;({const char*_tmp66F=(const char*)_untag_dyneither_ptr(d,sizeof(char),1U);Cyc_open(_tmp66F,0,_tag_dyneither(_tmp247,sizeof(unsigned short),0U));});});
if(_tmp243 == - 1){
if( mkdir((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1U)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp246=({struct Cyc_String_pa_PrintArg_struct _tmp498;_tmp498.tag=0U,_tmp498.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp498;});void*_tmp244[1U];_tmp244[0]=& _tmp246;({struct Cyc___cycFILE*_tmp671=Cyc_stderr;struct _dyneither_ptr _tmp670=({const char*_tmp245="Error: could not create directory %s\n";_tag_dyneither(_tmp245,sizeof(char),38U);});Cyc_fprintf(_tmp671,_tmp670,_tag_dyneither(_tmp244,sizeof(void*),1U));});});
return 1;}}else{
# 1474
 close(_tmp243);}}
# 1476
return 0;}
# 1479
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1483
struct _dyneither_ptr _tmp248=Cyc_strdup((struct _dyneither_ptr)file);
# 1485
struct Cyc_List_List*_tmp249=0;
while(1){
({struct _dyneither_ptr _tmp672=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp248);_tmp248=_tmp672;});
if(Cyc_strlen((struct _dyneither_ptr)_tmp248)== 0)break;
({struct Cyc_List_List*_tmp674=({struct Cyc_List_List*_tmp24B=_cycalloc(sizeof(*_tmp24B));({struct _dyneither_ptr*_tmp673=({struct _dyneither_ptr*_tmp24A=_cycalloc(sizeof(*_tmp24A));*_tmp24A=(struct _dyneither_ptr)_tmp248;_tmp24A;});_tmp24B->hd=_tmp673;}),_tmp24B->tl=_tmp249;_tmp24B;});_tmp249=_tmp674;});}
# 1492
for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp249->hd)))return 1;}
# 1495
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1502
static int Cyc_is_other_special(char c){
char _tmp24C=c;switch(_tmp24C){case 92U: _LL1: _LL2:
 goto _LL4;case 34U: _LL3: _LL4:
 goto _LL6;case 59U: _LL5: _LL6:
 goto _LL8;case 38U: _LL7: _LL8:
 goto _LLA;case 40U: _LL9: _LLA:
 goto _LLC;case 41U: _LLB: _LLC:
 goto _LLE;case 124U: _LLD: _LLE:
 goto _LL10;case 94U: _LLF: _LL10:
 goto _LL12;case 60U: _LL11: _LL12:
 goto _LL14;case 62U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
# 1517
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 1523
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp24D=Cyc_strlen((struct _dyneither_ptr)s);
# 1527
int _tmp24E=0;
int _tmp24F=0;
{int i=0;for(0;i < _tmp24D;++ i){
char _tmp250=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmp250 == '\'')++ _tmp24E;else{
if(Cyc_is_other_special(_tmp250))++ _tmp24F;}}}
# 1536
if(_tmp24E == 0  && _tmp24F == 0)
return s;
# 1540
if(_tmp24E == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp251[3U];({struct _dyneither_ptr*_tmp679=({struct _dyneither_ptr*_tmp253=_cycalloc(sizeof(*_tmp253));({struct _dyneither_ptr _tmp678=({const char*_tmp252="'";_tag_dyneither(_tmp252,sizeof(char),2U);});*_tmp253=_tmp678;});_tmp253;});_tmp251[0]=_tmp679;}),({struct _dyneither_ptr*_tmp677=({struct _dyneither_ptr*_tmp254=_cycalloc(sizeof(*_tmp254));*_tmp254=(struct _dyneither_ptr)s;_tmp254;});_tmp251[1]=_tmp677;}),({struct _dyneither_ptr*_tmp676=({struct _dyneither_ptr*_tmp256=_cycalloc(sizeof(*_tmp256));({struct _dyneither_ptr _tmp675=({const char*_tmp255="'";_tag_dyneither(_tmp255,sizeof(char),2U);});*_tmp256=_tmp675;});_tmp256;});_tmp251[2]=_tmp676;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp251,sizeof(struct _dyneither_ptr*),3U));}));{
# 1544
unsigned long _tmp257=(_tmp24D + _tmp24E)+ _tmp24F;
struct _dyneither_ptr s2=({unsigned int _tmp262=(_tmp257 + 1)+ 1U;char*_tmp261=_cycalloc_atomic(_check_times(_tmp262,sizeof(char)));({{unsigned int _tmp499=_tmp257 + 1;unsigned int i;for(i=0;i < _tmp499;++ i){_tmp261[i]='\000';}_tmp261[_tmp499]=0;}0;});_tag_dyneither(_tmp261,sizeof(char),_tmp262);});
int _tmp258=0;
int _tmp259=0;
for(0;_tmp258 < _tmp24D;++ _tmp258){
char _tmp25A=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp258));
if(_tmp25A == '\''  || Cyc_is_other_special(_tmp25A))
({struct _dyneither_ptr _tmp25B=_dyneither_ptr_plus(s2,sizeof(char),_tmp259 ++);char _tmp25C=*((char*)_check_dyneither_subscript(_tmp25B,sizeof(char),0U));char _tmp25D='\\';if(_get_dyneither_size(_tmp25B,sizeof(char))== 1U  && (_tmp25C == '\000'  && _tmp25D != '\000'))_throw_arraybounds();*((char*)_tmp25B.curr)=_tmp25D;});
({struct _dyneither_ptr _tmp25E=_dyneither_ptr_plus(s2,sizeof(char),_tmp259 ++);char _tmp25F=*((char*)_check_dyneither_subscript(_tmp25E,sizeof(char),0U));char _tmp260=_tmp25A;if(_get_dyneither_size(_tmp25E,sizeof(char))== 1U  && (_tmp25F == '\000'  && _tmp260 != '\000'))_throw_arraybounds();*((char*)_tmp25E.curr)=_tmp260;});}
# 1554
return(struct _dyneither_ptr)s2;};}
# 1556
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp263=_cycalloc(sizeof(*_tmp263));({struct _dyneither_ptr _tmp67A=Cyc_sh_escape_string(*sp);*_tmp263=_tmp67A;});_tmp263;});}
# 1561
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1569
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1574
({struct Cyc_String_pa_PrintArg_struct _tmp266=({struct Cyc_String_pa_PrintArg_struct _tmp49A;_tmp49A.tag=0U,({
struct _dyneither_ptr _tmp67B=(struct _dyneither_ptr)({const char*_tmp267=filename;_tag_dyneither(_tmp267,sizeof(char),_get_zero_arr_size_char((void*)_tmp267,1U));});_tmp49A.f1=_tmp67B;});_tmp49A;});void*_tmp264[1U];_tmp264[0]=& _tmp266;({struct Cyc___cycFILE*_tmp67D=Cyc_stderr;struct _dyneither_ptr _tmp67C=({const char*_tmp265="********************************* %s...\n";_tag_dyneither(_tmp265,sizeof(char),41U);});Cyc_fprintf(_tmp67D,_tmp67C,_tag_dyneither(_tmp264,sizeof(void*),1U));});});
# 1577
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp26A=({struct Cyc_String_pa_PrintArg_struct _tmp49B;_tmp49B.tag=0U,({struct _dyneither_ptr _tmp67E=(struct _dyneither_ptr)({const char*_tmp26B=filename;_tag_dyneither(_tmp26B,sizeof(char),_get_zero_arr_size_char((void*)_tmp26B,1U));});_tmp49B.f1=_tmp67E;});_tmp49B;});void*_tmp268[1U];_tmp268[0]=& _tmp26A;({struct _dyneither_ptr _tmp67F=({const char*_tmp269="\n%s:\n";_tag_dyneither(_tmp269,sizeof(char),6U);});Cyc_log(_tmp67F,_tag_dyneither(_tmp268,sizeof(void*),1U));});});{
# 1589 "buildlib.cyl"
struct _dyneither_ptr _tmp26C=Cyc_Filename_basename(({const char*_tmp3B9=filename;_tag_dyneither(_tmp3B9,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B9,1U));}));
struct _dyneither_ptr _tmp26D=Cyc_Filename_dirname(({const char*_tmp3B8=filename;_tag_dyneither(_tmp3B8,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B8,1U));}));
struct _dyneither_ptr _tmp26E=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp26C);
const char*_tmp26F=(const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp680=(struct _dyneither_ptr)_tmp26E;Cyc_strconcat(_tmp680,({const char*_tmp3B7=".iA";_tag_dyneither(_tmp3B7,sizeof(char),4U);}));}),sizeof(char),1U));
# 1594
const char*_tmp270=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp26D,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3B3=({struct Cyc_String_pa_PrintArg_struct _tmp4CF;_tmp4CF.tag=0U,_tmp4CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26E);_tmp4CF;});void*_tmp3B1[1U];_tmp3B1[0]=& _tmp3B3;({struct _dyneither_ptr _tmp683=({const char*_tmp3B2="%s.iB";_tag_dyneither(_tmp3B2,sizeof(char),6U);});Cyc_aprintf(_tmp683,_tag_dyneither(_tmp3B1,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp682=(struct _dyneither_ptr)_tmp26D;Cyc_Filename_concat(_tmp682,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B6=({struct Cyc_String_pa_PrintArg_struct _tmp4D0;_tmp4D0.tag=0U,_tmp4D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26E);_tmp4D0;});void*_tmp3B4[1U];_tmp3B4[0]=& _tmp3B6;({struct _dyneither_ptr _tmp681=({const char*_tmp3B5="%s.iB";_tag_dyneither(_tmp3B5,sizeof(char),6U);});Cyc_aprintf(_tmp681,_tag_dyneither(_tmp3B4,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1599
const char*_tmp271=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp26D,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3AD=({struct Cyc_String_pa_PrintArg_struct _tmp4CD;_tmp4CD.tag=0U,_tmp4CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26E);_tmp4CD;});void*_tmp3AB[1U];_tmp3AB[0]=& _tmp3AD;({struct _dyneither_ptr _tmp686=({const char*_tmp3AC="%s.iC";_tag_dyneither(_tmp3AC,sizeof(char),6U);});Cyc_aprintf(_tmp686,_tag_dyneither(_tmp3AB,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp685=(struct _dyneither_ptr)_tmp26D;Cyc_Filename_concat(_tmp685,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3B0=({struct Cyc_String_pa_PrintArg_struct _tmp4CE;_tmp4CE.tag=0U,_tmp4CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26E);_tmp4CE;});void*_tmp3AE[1U];_tmp3AE[0]=& _tmp3B0;({struct _dyneither_ptr _tmp684=({const char*_tmp3AF="%s.iC";_tag_dyneither(_tmp3AF,sizeof(char),6U);});Cyc_aprintf(_tmp684,_tag_dyneither(_tmp3AE,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1604
const char*_tmp272=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp26D,sizeof(char))== 0?({struct Cyc_String_pa_PrintArg_struct _tmp3A7=({struct Cyc_String_pa_PrintArg_struct _tmp4CB;_tmp4CB.tag=0U,_tmp4CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26E);_tmp4CB;});void*_tmp3A5[1U];_tmp3A5[0]=& _tmp3A7;({struct _dyneither_ptr _tmp689=({const char*_tmp3A6="%s.iD";_tag_dyneither(_tmp3A6,sizeof(char),6U);});Cyc_aprintf(_tmp689,_tag_dyneither(_tmp3A5,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp688=(struct _dyneither_ptr)_tmp26D;Cyc_Filename_concat(_tmp688,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AA=({struct Cyc_String_pa_PrintArg_struct _tmp4CC;_tmp4CC.tag=0U,_tmp4CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26E);_tmp4CC;});void*_tmp3A8[1U];_tmp3A8[0]=& _tmp3AA;({struct _dyneither_ptr _tmp687=({const char*_tmp3A9="%s.iD";_tag_dyneither(_tmp3A9,sizeof(char),6U);});Cyc_aprintf(_tmp687,_tag_dyneither(_tmp3A8,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1609
{struct _handler_cons _tmp273;_push_handler(& _tmp273);{int _tmp275=0;if(setjmp(_tmp273.handler))_tmp275=1;if(!_tmp275){
# 1612
if(Cyc_force_directory_prefixes(({const char*_tmp276=filename;_tag_dyneither(_tmp276,sizeof(char),_get_zero_arr_size_char((void*)_tmp276,1U));}))){
int _tmp277=1;_npop_handler(0U);return _tmp277;}
# 1617
if(Cyc_mode != Cyc_FINISH){
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp49C;_tmp49C.tag=0U,({struct _dyneither_ptr _tmp68A=(struct _dyneither_ptr)({const char*_tmp27B=_tmp26F;_tag_dyneither(_tmp27B,sizeof(char),_get_zero_arr_size_char((void*)_tmp27B,1U));});_tmp49C.f1=_tmp68A;});_tmp49C;});void*_tmp278[1U];_tmp278[0]=& _tmp27A;({struct _dyneither_ptr _tmp68B=({const char*_tmp279="cat >%s <<XXX\n";_tag_dyneither(_tmp279,sizeof(char),15U);});Cyc_prscript(_tmp68B,_tag_dyneither(_tmp278,sizeof(void*),1U));});});
{struct Cyc_List_List*_tmp27C=cpp_insert;for(0;_tmp27C != 0;_tmp27C=_tmp27C->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp49D;_tmp49D.tag=0U,_tmp49D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp27C->hd));_tmp49D;});void*_tmp27D[1U];_tmp27D[0]=& _tmp27F;({struct _dyneither_ptr _tmp68C=({const char*_tmp27E="%s";_tag_dyneither(_tmp27E,sizeof(char),3U);});Cyc_prscript(_tmp68C,_tag_dyneither(_tmp27D,sizeof(void*),1U));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp49E;_tmp49E.tag=0U,({struct _dyneither_ptr _tmp68D=(struct _dyneither_ptr)({const char*_tmp283=filename;_tag_dyneither(_tmp283,sizeof(char),_get_zero_arr_size_char((void*)_tmp283,1U));});_tmp49E.f1=_tmp68D;});_tmp49E;});void*_tmp280[1U];_tmp280[0]=& _tmp282;({struct _dyneither_ptr _tmp68E=({const char*_tmp281="#include <%s>\n";_tag_dyneither(_tmp281,sizeof(char),15U);});Cyc_prscript(_tmp68E,_tag_dyneither(_tmp280,sizeof(void*),1U));});});
({void*_tmp284=0U;({struct _dyneither_ptr _tmp68F=({const char*_tmp285="XXX\n";_tag_dyneither(_tmp285,sizeof(char),5U);});Cyc_prscript(_tmp68F,_tag_dyneither(_tmp284,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp4A1;_tmp4A1.tag=0U,_tmp4A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4A1;});struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp4A0;_tmp4A0.tag=0U,({struct _dyneither_ptr _tmp690=(struct _dyneither_ptr)({const char*_tmp28C=_tmp270;_tag_dyneither(_tmp28C,sizeof(char),_get_zero_arr_size_char((void*)_tmp28C,1U));});_tmp4A0.f1=_tmp690;});_tmp4A0;});struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp49F;_tmp49F.tag=0U,({struct _dyneither_ptr _tmp691=(struct _dyneither_ptr)({const char*_tmp28B=_tmp26F;_tag_dyneither(_tmp28B,sizeof(char),_get_zero_arr_size_char((void*)_tmp28B,1U));});_tmp49F.f1=_tmp691;});_tmp49F;});void*_tmp286[3U];_tmp286[0]=& _tmp288,_tmp286[1]=& _tmp289,_tmp286[2]=& _tmp28A;({struct _dyneither_ptr _tmp692=({const char*_tmp287="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp287,sizeof(char),35U);});Cyc_prscript(_tmp692,_tag_dyneither(_tmp286,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp28F=({struct Cyc_String_pa_PrintArg_struct _tmp4A4;_tmp4A4.tag=0U,_tmp4A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4A4;});struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp4A3;_tmp4A3.tag=0U,({struct _dyneither_ptr _tmp693=(struct _dyneither_ptr)({const char*_tmp293=_tmp271;_tag_dyneither(_tmp293,sizeof(char),_get_zero_arr_size_char((void*)_tmp293,1U));});_tmp4A3.f1=_tmp693;});_tmp4A3;});struct Cyc_String_pa_PrintArg_struct _tmp291=({struct Cyc_String_pa_PrintArg_struct _tmp4A2;_tmp4A2.tag=0U,({struct _dyneither_ptr _tmp694=(struct _dyneither_ptr)({const char*_tmp292=_tmp26F;_tag_dyneither(_tmp292,sizeof(char),_get_zero_arr_size_char((void*)_tmp292,1U));});_tmp4A2.f1=_tmp694;});_tmp4A2;});void*_tmp28D[3U];_tmp28D[0]=& _tmp28F,_tmp28D[1]=& _tmp290,_tmp28D[2]=& _tmp291;({struct _dyneither_ptr _tmp695=({const char*_tmp28E="$GCC %s -E     -o %s -x c %s;\n";_tag_dyneither(_tmp28E,sizeof(char),31U);});Cyc_prscript(_tmp695,_tag_dyneither(_tmp28D,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp296=({struct Cyc_String_pa_PrintArg_struct _tmp4A5;_tmp4A5.tag=0U,({struct _dyneither_ptr _tmp696=(struct _dyneither_ptr)({const char*_tmp297=_tmp26F;_tag_dyneither(_tmp297,sizeof(char),_get_zero_arr_size_char((void*)_tmp297,1U));});_tmp4A5.f1=_tmp696;});_tmp4A5;});void*_tmp294[1U];_tmp294[0]=& _tmp296;({struct _dyneither_ptr _tmp697=({const char*_tmp295="rm %s\n";_tag_dyneither(_tmp295,sizeof(char),7U);});Cyc_prscript(_tmp697,_tag_dyneither(_tmp294,sizeof(void*),1U));});});}else{
# 1629
({struct Cyc___cycFILE*_tmp698=Cyc_fopen(_tmp26F,"w");maybe=_tmp698;});
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp29A=({struct Cyc_String_pa_PrintArg_struct _tmp4A6;_tmp4A6.tag=0U,({struct _dyneither_ptr _tmp699=(struct _dyneither_ptr)({const char*_tmp29B=_tmp26F;_tag_dyneither(_tmp29B,sizeof(char),_get_zero_arr_size_char((void*)_tmp29B,1U));});_tmp4A6.f1=_tmp699;});_tmp4A6;});void*_tmp298[1U];_tmp298[0]=& _tmp29A;({struct Cyc___cycFILE*_tmp69B=Cyc_stderr;struct _dyneither_ptr _tmp69A=({const char*_tmp299="Error: could not create file %s\n";_tag_dyneither(_tmp299,sizeof(char),33U);});Cyc_fprintf(_tmp69B,_tmp69A,_tag_dyneither(_tmp298,sizeof(void*),1U));});});{
int _tmp29C=1;_npop_handler(0U);return _tmp29C;};}
# 1634
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp29F=({struct Cyc_String_pa_PrintArg_struct _tmp4A7;_tmp4A7.tag=0U,({struct _dyneither_ptr _tmp69C=(struct _dyneither_ptr)({const char*_tmp2A0=_tmp26F;_tag_dyneither(_tmp2A0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A0,1U));});_tmp4A7.f1=_tmp69C;});_tmp4A7;});void*_tmp29D[1U];_tmp29D[0]=& _tmp29F;({struct Cyc___cycFILE*_tmp69E=Cyc_stderr;struct _dyneither_ptr _tmp69D=({const char*_tmp29E="Creating %s\n";_tag_dyneither(_tmp29E,sizeof(char),13U);});Cyc_fprintf(_tmp69E,_tmp69D,_tag_dyneither(_tmp29D,sizeof(void*),1U));});});
out_file=maybe;
{struct Cyc_List_List*_tmp2A1=cpp_insert;for(0;_tmp2A1 != 0;_tmp2A1=_tmp2A1->tl){
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp2A1->hd),sizeof(char),1U)),out_file);}}
# 1640
({struct Cyc_String_pa_PrintArg_struct _tmp2A4=({struct Cyc_String_pa_PrintArg_struct _tmp4A8;_tmp4A8.tag=0U,({struct _dyneither_ptr _tmp69F=(struct _dyneither_ptr)({const char*_tmp2A5=filename;_tag_dyneither(_tmp2A5,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A5,1U));});_tmp4A8.f1=_tmp69F;});_tmp4A8;});void*_tmp2A2[1U];_tmp2A2[0]=& _tmp2A4;({struct Cyc___cycFILE*_tmp6A1=out_file;struct _dyneither_ptr _tmp6A0=({const char*_tmp2A3="#include <%s>\n";_tag_dyneither(_tmp2A3,sizeof(char),15U);});Cyc_fprintf(_tmp6A1,_tmp6A0,_tag_dyneither(_tmp2A2,sizeof(void*),1U));});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp2A6=({
struct Cyc_List_List*_tmp6A5=({struct Cyc_List_List*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));({struct _dyneither_ptr*_tmp6A4=({struct _dyneither_ptr*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));({struct _dyneither_ptr _tmp6A3=(struct _dyneither_ptr)({const char*_tmp2C1="";_tag_dyneither(_tmp2C1,sizeof(char),1U);});*_tmp2C2=_tmp6A3;});_tmp2C2;});_tmp2C3->hd=_tmp6A4;}),({
struct Cyc_List_List*_tmp6A2=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp2C3->tl=_tmp6A2;});_tmp2C3;});
# 1643
Cyc_str_sepstr(_tmp6A5,({const char*_tmp2C4=" ";_tag_dyneither(_tmp2C4,sizeof(char),2U);}));});
# 1646
char*cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2BA=({struct Cyc_String_pa_PrintArg_struct _tmp4B4;_tmp4B4.tag=0U,_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4B4;});struct Cyc_String_pa_PrintArg_struct _tmp2BB=({struct Cyc_String_pa_PrintArg_struct _tmp4B3;_tmp4B3.tag=0U,_tmp4B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4B3;});struct Cyc_String_pa_PrintArg_struct _tmp2BC=({struct Cyc_String_pa_PrintArg_struct _tmp4B2;_tmp4B2.tag=0U,_tmp4B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A6);_tmp4B2;});struct Cyc_String_pa_PrintArg_struct _tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp4B1;_tmp4B1.tag=0U,({struct _dyneither_ptr _tmp6A6=(struct _dyneither_ptr)({const char*_tmp2C0=_tmp270;_tag_dyneither(_tmp2C0,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C0,1U));});_tmp4B1.f1=_tmp6A6;});_tmp4B1;});struct Cyc_String_pa_PrintArg_struct _tmp2BE=({struct Cyc_String_pa_PrintArg_struct _tmp4B0;_tmp4B0.tag=0U,({struct _dyneither_ptr _tmp6A7=(struct _dyneither_ptr)({const char*_tmp2BF=_tmp26F;_tag_dyneither(_tmp2BF,sizeof(char),_get_zero_arr_size_char((void*)_tmp2BF,1U));});_tmp4B0.f1=_tmp6A7;});_tmp4B0;});void*_tmp2B8[5U];_tmp2B8[0]=& _tmp2BA,_tmp2B8[1]=& _tmp2BB,_tmp2B8[2]=& _tmp2BC,_tmp2B8[3]=& _tmp2BD,_tmp2B8[4]=& _tmp2BE;({struct _dyneither_ptr _tmp6A8=({const char*_tmp2B9="%s %s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp2B9,sizeof(char),30U);});Cyc_aprintf(_tmp6A8,_tag_dyneither(_tmp2B8,sizeof(void*),5U));});}),sizeof(char),1U));
# 1649
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2A9=({struct Cyc_String_pa_PrintArg_struct _tmp4A9;_tmp4A9.tag=0U,({struct _dyneither_ptr _tmp6A9=(struct _dyneither_ptr)({char*_tmp2AA=cmd;_tag_dyneither(_tmp2AA,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AA,1U));});_tmp4A9.f1=_tmp6A9;});_tmp4A9;});void*_tmp2A7[1U];_tmp2A7[0]=& _tmp2A9;({struct Cyc___cycFILE*_tmp6AB=Cyc_stderr;struct _dyneither_ptr _tmp6AA=({const char*_tmp2A8="%s\n";_tag_dyneither(_tmp2A8,sizeof(char),4U);});Cyc_fprintf(_tmp6AB,_tmp6AA,_tag_dyneither(_tmp2A7,sizeof(void*),1U));});});
if(! system((const char*)cmd)){
# 1654
cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2AD=({struct Cyc_String_pa_PrintArg_struct _tmp4AE;_tmp4AE.tag=0U,_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4AE;});struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp4AD;_tmp4AD.tag=0U,_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4AD;});struct Cyc_String_pa_PrintArg_struct _tmp2AF=({struct Cyc_String_pa_PrintArg_struct _tmp4AC;_tmp4AC.tag=0U,_tmp4AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A6);_tmp4AC;});struct Cyc_String_pa_PrintArg_struct _tmp2B0=({struct Cyc_String_pa_PrintArg_struct _tmp4AB;_tmp4AB.tag=0U,({struct _dyneither_ptr _tmp6AC=(struct _dyneither_ptr)({const char*_tmp2B3=_tmp271;_tag_dyneither(_tmp2B3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B3,1U));});_tmp4AB.f1=_tmp6AC;});_tmp4AB;});struct Cyc_String_pa_PrintArg_struct _tmp2B1=({struct Cyc_String_pa_PrintArg_struct _tmp4AA;_tmp4AA.tag=0U,({struct _dyneither_ptr _tmp6AD=(struct _dyneither_ptr)({const char*_tmp2B2=_tmp26F;_tag_dyneither(_tmp2B2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B2,1U));});_tmp4AA.f1=_tmp6AD;});_tmp4AA;});void*_tmp2AB[5U];_tmp2AB[0]=& _tmp2AD,_tmp2AB[1]=& _tmp2AE,_tmp2AB[2]=& _tmp2AF,_tmp2AB[3]=& _tmp2B0,_tmp2AB[4]=& _tmp2B1;({struct _dyneither_ptr _tmp6AE=({const char*_tmp2AC="%s %s %s -E -o %s -x c %s";_tag_dyneither(_tmp2AC,sizeof(char),26U);});Cyc_aprintf(_tmp6AE,_tag_dyneither(_tmp2AB,sizeof(void*),5U));});}),sizeof(char),1U));
# 1657
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2B6=({struct Cyc_String_pa_PrintArg_struct _tmp4AF;_tmp4AF.tag=0U,({struct _dyneither_ptr _tmp6AF=(struct _dyneither_ptr)({char*_tmp2B7=cmd;_tag_dyneither(_tmp2B7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B7,1U));});_tmp4AF.f1=_tmp6AF;});_tmp4AF;});void*_tmp2B4[1U];_tmp2B4[0]=& _tmp2B6;({struct Cyc___cycFILE*_tmp6B1=Cyc_stderr;struct _dyneither_ptr _tmp6B0=({const char*_tmp2B5="%s\n";_tag_dyneither(_tmp2B5,sizeof(char),4U);});Cyc_fprintf(_tmp6B1,_tmp6B0,_tag_dyneither(_tmp2B4,sizeof(void*),1U));});});
 system((const char*)cmd);}};}}
# 1664
if(Cyc_gathering()){int _tmp2C5=0;_npop_handler(0U);return _tmp2C5;}{
# 1667
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
({struct Cyc___cycFILE*_tmp6B2=Cyc_fopen(_tmp270,"r");maybe=_tmp6B2;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp6B5=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C8=({struct Cyc_String_pa_PrintArg_struct _tmp4B5;_tmp4B5.tag=0U,({struct _dyneither_ptr _tmp6B3=(struct _dyneither_ptr)({const char*_tmp2C9=_tmp270;_tag_dyneither(_tmp2C9,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C9,1U));});_tmp4B5.f1=_tmp6B3;});_tmp4B5;});void*_tmp2C6[1U];_tmp2C6[0]=& _tmp2C8;({struct _dyneither_ptr _tmp6B4=({const char*_tmp2C7="can't open macrosfile %s";_tag_dyneither(_tmp2C7,sizeof(char),25U);});Cyc_aprintf(_tmp6B4,_tag_dyneither(_tmp2C6,sizeof(void*),1U));});});_tmp2CA->f1=_tmp6B5;});_tmp2CA;}));
# 1672
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp2CB=Cyc_Lexing_from_file(in_file);
struct _tuple21*entry;
while(({struct _tuple21*_tmp6B6=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp2CB);entry=_tmp6B6;})!= 0){
struct _tuple21*_tmp2CC=(struct _tuple21*)_check_null(entry);struct _tuple21*_tmp2CD=_tmp2CC;struct _dyneither_ptr*_tmp2CF;struct Cyc_Set_Set*_tmp2CE;_LL1: _tmp2CF=_tmp2CD->f1;_tmp2CE=_tmp2CD->f2;_LL2:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp2CF,_tmp2CE);}
# 1681
Cyc_fclose(in_file);
# 1684
({struct Cyc___cycFILE*_tmp6B7=Cyc_fopen(_tmp271,"r");maybe=_tmp6B7;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp6BA=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D2=({struct Cyc_String_pa_PrintArg_struct _tmp4B6;_tmp4B6.tag=0U,({struct _dyneither_ptr _tmp6B8=(struct _dyneither_ptr)({const char*_tmp2D3=_tmp271;_tag_dyneither(_tmp2D3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D3,1U));});_tmp4B6.f1=_tmp6B8;});_tmp4B6;});void*_tmp2D0[1U];_tmp2D0[0]=& _tmp2D2;({struct _dyneither_ptr _tmp6B9=({const char*_tmp2D1="can't open declsfile %s";_tag_dyneither(_tmp2D1,sizeof(char),24U);});Cyc_aprintf(_tmp6B9,_tag_dyneither(_tmp2D0,sizeof(void*),1U));});});_tmp2D4->f1=_tmp6BA;});_tmp2D4;}));
# 1688
in_file=maybe;
({struct Cyc_Lexing_lexbuf*_tmp6BB=Cyc_Lexing_from_file(in_file);_tmp2CB=_tmp6BB;});
({struct Cyc___cycFILE*_tmp6BC=Cyc_fopen(_tmp272,"w");Cyc_slurp_out=_tmp6BC;});
if(!((unsigned int)Cyc_slurp_out)){int _tmp2D5=1;_npop_handler(0U);return _tmp2D5;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp2CB)){;}{
# 1694
struct Cyc_List_List*_tmp2D6=user_defs;
while(_tmp2D6 != 0){
struct _tuple24*_tmp2D7=(struct _tuple24*)_tmp2D6->hd;struct _tuple24*_tmp2D8=_tmp2D7;struct _dyneither_ptr*_tmp2DC;_LL4: _tmp2DC=_tmp2D8->f2;_LL5:;
({struct Cyc_String_pa_PrintArg_struct _tmp2DB=({struct Cyc_String_pa_PrintArg_struct _tmp4B7;_tmp4B7.tag=0U,_tmp4B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2DC);_tmp4B7;});void*_tmp2D9[1U];_tmp2D9[0]=& _tmp2DB;({struct Cyc___cycFILE*_tmp6BE=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp6BD=({const char*_tmp2DA="%s";_tag_dyneither(_tmp2DA,sizeof(char),3U);});Cyc_fprintf(_tmp6BE,_tmp6BD,_tag_dyneither(_tmp2D9,sizeof(void*),1U));});});
_tmp2D6=_tmp2D6->tl;}
# 1700
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if(Cyc_mode != Cyc_FINISH)
;
# 1706
({struct Cyc___cycFILE*_tmp6BF=Cyc_fopen(_tmp272,"r");maybe=_tmp6BF;});
if(!((unsigned int)maybe)){int _tmp2DD=1;_npop_handler(0U);return _tmp2DD;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp2DE=_tmp272;_tag_dyneither(_tmp2DE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2DE,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp2DF=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1716
{struct Cyc_List_List*_tmp2E0=_tmp2DF;for(0;_tmp2E0 != 0;_tmp2E0=_tmp2E0->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp2E0->hd,t);}}{
# 1720
struct Cyc_List_List*_tmp2E1=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);
struct Cyc_Set_Set*_tmp2E2=({struct Cyc_List_List*_tmp6C0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,_tmp2E1);Cyc_reachable(_tmp6C0,t);});
# 1724
struct Cyc_List_List*_tmp2E3=0;
struct Cyc_List_List*_tmp2E4=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp2E5=_tmp2DF;for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){
struct Cyc_Absyn_Decl*_tmp2E6=(struct Cyc_Absyn_Decl*)_tmp2E5->hd;
struct _dyneither_ptr*name;
{void*_tmp2E7=_tmp2E6->r;void*_tmp2E8=_tmp2E7;struct Cyc_Absyn_Typedefdecl*_tmp303;struct Cyc_Absyn_Enumdecl*_tmp302;struct Cyc_Absyn_Aggrdecl*_tmp301;struct Cyc_Absyn_Fndecl*_tmp300;struct Cyc_Absyn_Vardecl*_tmp2FF;switch(*((int*)_tmp2E8)){case 0U: _LL7: _tmp2FF=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2E8)->f1;_LL8: {
# 1732
struct _tuple1*_tmp2E9=_tmp2FF->name;struct _tuple1*_tmp2EA=_tmp2E9;struct _dyneither_ptr*_tmp2EB;_LL26: _tmp2EB=_tmp2EA->f2;_LL27:;
({struct Cyc_Set_Set*_tmp6C1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2EB);defined_symbols=_tmp6C1;});
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp2EB))name=0;else{
name=_tmp2EB;}
goto _LL6;}case 1U: _LL9: _tmp300=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2E8)->f1;_LLA: {
# 1738
struct _tuple1*_tmp2EC=_tmp300->name;struct _tuple1*_tmp2ED=_tmp2EC;struct _dyneither_ptr*_tmp2EE;_LL29: _tmp2EE=_tmp2ED->f2;_LL2A:;
({struct Cyc_Set_Set*_tmp6C2=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2EE);defined_symbols=_tmp6C2;});
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp2EE))name=0;else{
name=_tmp2EE;}
goto _LL6;}case 5U: _LLB: _tmp301=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2E8)->f1;_LLC: {
# 1744
struct _tuple1*_tmp2EF=_tmp301->name;struct _tuple1*_tmp2F0=_tmp2EF;struct _dyneither_ptr*_tmp2F1;_LL2C: _tmp2F1=_tmp2F0->f2;_LL2D:;
name=_tmp2F1;
goto _LL6;}case 7U: _LLD: _tmp302=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2E8)->f1;_LLE: {
# 1748
struct _tuple1*_tmp2F2=_tmp302->name;struct _tuple1*_tmp2F3=_tmp2F2;struct _dyneither_ptr*_tmp2FB;_LL2F: _tmp2FB=_tmp2F3->f2;_LL30:;
name=_tmp2FB;
# 1752
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2E2,name))
({struct Cyc_List_List*_tmp6C3=({struct Cyc_List_List*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->hd=_tmp2E6,_tmp2F4->tl=_tmp2E3;_tmp2F4;});_tmp2E3=_tmp6C3;});else{
# 1755
if((unsigned int)_tmp302->fields){
struct Cyc_List_List*_tmp2F5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp302->fields))->v;for(0;_tmp2F5 != 0;_tmp2F5=_tmp2F5->tl){
struct Cyc_Absyn_Enumfield*_tmp2F6=(struct Cyc_Absyn_Enumfield*)_tmp2F5->hd;
struct _tuple1*_tmp2F7=_tmp2F6->name;struct _tuple1*_tmp2F8=_tmp2F7;struct _dyneither_ptr*_tmp2FA;_LL32: _tmp2FA=_tmp2F8->f2;_LL33:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2E2,_tmp2FA)){
({struct Cyc_List_List*_tmp6C4=({struct Cyc_List_List*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->hd=_tmp2E6,_tmp2F9->tl=_tmp2E3;_tmp2F9;});_tmp2E3=_tmp6C4;});
break;}}}}
# 1765
name=0;
goto _LL6;}case 8U: _LLF: _tmp303=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2E8)->f1;_LL10: {
# 1768
struct _tuple1*_tmp2FC=_tmp303->name;struct _tuple1*_tmp2FD=_tmp2FC;struct _dyneither_ptr*_tmp2FE;_LL35: _tmp2FE=_tmp2FD->f2;_LL36:;
name=_tmp2FE;
goto _LL6;}case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 2U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 3U: _LL19: _LL1A:
 goto _LL1C;case 9U: _LL1B: _LL1C:
 goto _LL1E;case 10U: _LL1D: _LL1E:
 goto _LL20;case 11U: _LL1F: _LL20:
 goto _LL22;case 12U: _LL21: _LL22:
 goto _LL24;default: _LL23: _LL24:
# 1781
 name=0;
goto _LL6;}_LL6:;}
# 1785
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2E2,name)){
if(({struct _dyneither_ptr _tmp6C6=(struct _dyneither_ptr)*name;struct _dyneither_ptr _tmp6C5=(struct _dyneither_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp6C6,_tmp6C5,Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix));})!= 0)
({struct Cyc_List_List*_tmp6C7=({struct Cyc_List_List*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->hd=_tmp2E6,_tmp304->tl=_tmp2E3;_tmp304;});_tmp2E3=_tmp6C7;});else{
# 1790
Cyc_rename_decl(_tmp2E6);
({struct Cyc_List_List*_tmp6C8=({struct Cyc_List_List*_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305->hd=_tmp2E6,_tmp305->tl=_tmp2E4;_tmp305;});_tmp2E4=_tmp6C8;});}}}}
# 1797
if(!Cyc_do_setjmp){
({struct Cyc___cycFILE*_tmp6C9=Cyc_fopen(filename,"w");maybe=_tmp6C9;});
if(!((unsigned int)maybe)){int _tmp306=1;_npop_handler(0U);return _tmp306;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp37D=({struct Cyc_String_pa_PrintArg_struct _tmp4C3;_tmp4C3.tag=0U,({struct _dyneither_ptr _tmp6CA=(struct _dyneither_ptr)({const char*_tmp37E=filename;_tag_dyneither(_tmp37E,sizeof(char),_get_zero_arr_size_char((void*)_tmp37E,1U));});_tmp4C3.f1=_tmp6CA;});_tmp4C3;});void*_tmp37B[1U];_tmp37B[0]=& _tmp37D;({struct _dyneither_ptr _tmp6CB=({const char*_tmp37C="_%s_";_tag_dyneither(_tmp37C,sizeof(char),5U);});Cyc_aprintf(_tmp6CB,_tag_dyneither(_tmp37B,sizeof(void*),1U));});});
{int _tmp307=0;for(0;_tmp307 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp307){
if(((char*)ifdefmacro.curr)[_tmp307]== '.'  || ((char*)ifdefmacro.curr)[_tmp307]== '/')
({struct _dyneither_ptr _tmp308=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp307);char _tmp309=*((char*)_check_dyneither_subscript(_tmp308,sizeof(char),0U));char _tmp30A='_';if(_get_dyneither_size(_tmp308,sizeof(char))== 1U  && (_tmp309 == '\000'  && _tmp30A != '\000'))_throw_arraybounds();*((char*)_tmp308.curr)=_tmp30A;});else{
if(((char*)ifdefmacro.curr)[_tmp307]!= '_'  && ((char*)ifdefmacro.curr)[_tmp307]!= '/')
({struct _dyneither_ptr _tmp30B=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp307);char _tmp30C=*((char*)_check_dyneither_subscript(_tmp30B,sizeof(char),0U));char _tmp30D=(char) toupper((int)((char*)ifdefmacro.curr)[_tmp307]);if(_get_dyneither_size(_tmp30B,sizeof(char))== 1U  && (_tmp30C == '\000'  && _tmp30D != '\000'))_throw_arraybounds();*((char*)_tmp30B.curr)=_tmp30D;});}}}
# 1809
({struct Cyc_String_pa_PrintArg_struct _tmp310=({struct Cyc_String_pa_PrintArg_struct _tmp4B9;_tmp4B9.tag=0U,_tmp4B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4B9;});struct Cyc_String_pa_PrintArg_struct _tmp311=({struct Cyc_String_pa_PrintArg_struct _tmp4B8;_tmp4B8.tag=0U,_tmp4B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4B8;});void*_tmp30E[2U];_tmp30E[0]=& _tmp310,_tmp30E[1]=& _tmp311;({struct Cyc___cycFILE*_tmp6CD=out_file;struct _dyneither_ptr _tmp6CC=({const char*_tmp30F="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp30F,sizeof(char),23U);});Cyc_fprintf(_tmp6CD,_tmp6CC,_tag_dyneither(_tmp30E,sizeof(void*),2U));});});{
# 1816
struct Cyc_List_List*_tmp312=0;
struct Cyc_List_List*_tmp313=0;
{struct Cyc_List_List*_tmp314=_tmp2E3;for(0;_tmp314 != 0;_tmp314=_tmp314->tl){
struct Cyc_Absyn_Decl*_tmp315=(struct Cyc_Absyn_Decl*)_tmp314->hd;
int _tmp316=0;
struct _dyneither_ptr*name;
{void*_tmp317=_tmp315->r;void*_tmp318=_tmp317;struct Cyc_Absyn_Typedefdecl*_tmp32C;struct Cyc_Absyn_Enumdecl*_tmp32B;struct Cyc_Absyn_Aggrdecl*_tmp32A;struct Cyc_Absyn_Fndecl*_tmp329;struct Cyc_Absyn_Vardecl*_tmp328;switch(*((int*)_tmp318)){case 0U: _LL38: _tmp328=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp318)->f1;_LL39: {
# 1824
struct _tuple1*_tmp319=_tmp328->name;struct _tuple1*_tmp31A=_tmp319;struct _dyneither_ptr*_tmp31B;_LL57: _tmp31B=_tmp31A->f2;_LL58:;
name=_tmp31B;
goto _LL37;}case 1U: _LL3A: _tmp329=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp318)->f1;_LL3B:
# 1828
 if(_tmp329->is_inline){name=0;goto _LL37;}{
struct _tuple1*_tmp31C=_tmp329->name;struct _tuple1*_tmp31D=_tmp31C;struct _dyneither_ptr*_tmp31E;_LL5A: _tmp31E=_tmp31D->f2;_LL5B:;
name=_tmp31E;
goto _LL37;};case 5U: _LL3C: _tmp32A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp318)->f1;_LL3D: {
# 1833
struct _tuple1*_tmp31F=_tmp32A->name;struct _tuple1*_tmp320=_tmp31F;struct _dyneither_ptr*_tmp321;_LL5D: _tmp321=_tmp320->f2;_LL5E:;
name=_tmp321;
goto _LL37;}case 7U: _LL3E: _tmp32B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp318)->f1;_LL3F: {
# 1837
struct _tuple1*_tmp322=_tmp32B->name;struct _tuple1*_tmp323=_tmp322;struct _dyneither_ptr*_tmp324;_LL60: _tmp324=_tmp323->f2;_LL61:;
name=_tmp324;
goto _LL37;}case 8U: _LL40: _tmp32C=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp318)->f1;_LL41: {
# 1841
struct _tuple1*_tmp325=_tmp32C->name;struct _tuple1*_tmp326=_tmp325;struct _dyneither_ptr*_tmp327;_LL63: _tmp327=_tmp326->f2;_LL64:;
name=_tmp327;
goto _LL37;}case 4U: _LL42: _LL43:
 goto _LL45;case 13U: _LL44: _LL45:
 goto _LL47;case 14U: _LL46: _LL47:
 goto _LL49;case 2U: _LL48: _LL49:
 goto _LL4B;case 6U: _LL4A: _LL4B:
 goto _LL4D;case 3U: _LL4C: _LL4D:
 goto _LL4F;case 9U: _LL4E: _LL4F:
 goto _LL51;case 10U: _LL50: _LL51:
 goto _LL53;case 11U: _LL52: _LL53:
 goto _LL55;default: _LL54: _LL55:
# 1854
 name=0;
goto _LL37;}_LL37:;}
# 1857
if(!((unsigned int)name) && !_tmp316)continue;
# 1862
if(({struct Cyc_Set_Set*_tmp6CE=_tmp2E2;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6CE,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp32D=Cyc_named_decl(_tmp2E4,name);
Cyc_merge_rep_decl(_tmp32D,_tmp315);
({struct Cyc_List_List*_tmp6CF=({struct Cyc_List_List*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->hd=_tmp315,_tmp32E->tl=_tmp312;_tmp32E;});_tmp312=_tmp6CF;});}else{
# 1868
({struct Cyc_List_List*_tmp6D0=({struct Cyc_List_List*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F->hd=_tmp315,_tmp32F->tl=_tmp312;_tmp32F;});_tmp312=_tmp6D0;});}
({struct Cyc_List_List*_tmp6D1=({struct Cyc_List_List*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->hd=name,_tmp330->tl=_tmp313;_tmp330;});_tmp313=_tmp6D1;});}}
# 1873
{struct _handler_cons _tmp331;_push_handler(& _tmp331);{int _tmp333=0;if(setjmp(_tmp331.handler))_tmp333=1;if(!_tmp333){
Cyc_Binding_resolve_all(_tmp312);
({struct Cyc_Tcenv_Tenv*_tmp6D2=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp6D2,1,_tmp312);});
# 1874
;_pop_handler();}else{void*_tmp332=(void*)_exn_thrown;void*_tmp334=_tmp332;_LL66: _LL67:
# 1877
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp336=_cycalloc(sizeof(*_tmp336));_tmp336->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp6D3=({const char*_tmp335="can't typecheck acquired declarations";_tag_dyneither(_tmp335,sizeof(char),38U);});_tmp336->f1=_tmp6D3;});_tmp336;}));_LL65:;}};}
# 1881
{struct _tuple0 _tmp337=({struct _tuple0 _tmp4BD;_tmp4BD.f1=_tmp312,_tmp4BD.f2=_tmp313;_tmp4BD;});struct _tuple0 _tmp338=_tmp337;struct Cyc_List_List*_tmp34A;struct Cyc_List_List*_tmp349;_LL6B: _tmp34A=_tmp338.f1;_tmp349=_tmp338.f2;_LL6C:;for(0;
_tmp34A != 0  && _tmp349 != 0;(_tmp34A=_tmp34A->tl,_tmp349=_tmp349->tl)){
struct Cyc_Absyn_Decl*_tmp339=(struct Cyc_Absyn_Decl*)_tmp34A->hd;
struct _dyneither_ptr*_tmp33A=(struct _dyneither_ptr*)_tmp349->hd;
int _tmp33B=0;
if(!((unsigned int)_tmp33A))
_tmp33B=1;
# 1890
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp33A){
({struct _dyneither_ptr _tmp6D5=({struct Cyc_String_pa_PrintArg_struct _tmp33E=({struct Cyc_String_pa_PrintArg_struct _tmp4BA;_tmp4BA.tag=0U,_tmp4BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp33A);_tmp4BA;});void*_tmp33C[1U];_tmp33C[0]=& _tmp33E;({struct _dyneither_ptr _tmp6D4=({const char*_tmp33D="_%s_def_";_tag_dyneither(_tmp33D,sizeof(char),9U);});Cyc_aprintf(_tmp6D4,_tag_dyneither(_tmp33C,sizeof(void*),1U));});});ifdefmacro=_tmp6D5;});
({struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp4BB;_tmp4BB.tag=0U,_tmp4BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4BB;});void*_tmp33F[1U];_tmp33F[0]=& _tmp341;({struct Cyc___cycFILE*_tmp6D7=out_file;struct _dyneither_ptr _tmp6D6=({const char*_tmp340="#ifndef %s\n";_tag_dyneither(_tmp340,sizeof(char),12U);});Cyc_fprintf(_tmp6D7,_tmp6D6,_tag_dyneither(_tmp33F,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp344=({struct Cyc_String_pa_PrintArg_struct _tmp4BC;_tmp4BC.tag=0U,_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4BC;});void*_tmp342[1U];_tmp342[0]=& _tmp344;({struct Cyc___cycFILE*_tmp6D9=out_file;struct _dyneither_ptr _tmp6D8=({const char*_tmp343="#define %s\n";_tag_dyneither(_tmp343,sizeof(char),12U);});Cyc_fprintf(_tmp6D9,_tmp6D8,_tag_dyneither(_tmp342,sizeof(void*),1U));});});
# 1896
({struct Cyc_List_List*_tmp6DA=({struct Cyc_Absyn_Decl*_tmp345[1U];_tmp345[0]=_tmp339;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp345,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp6DA,out_file);});
({void*_tmp346=0U;({struct Cyc___cycFILE*_tmp6DC=out_file;struct _dyneither_ptr _tmp6DB=({const char*_tmp347="#endif\n";_tag_dyneither(_tmp347,sizeof(char),8U);});Cyc_fprintf(_tmp6DC,_tmp6DB,_tag_dyneither(_tmp346,sizeof(void*),0U));});});}else{
# 1901
({struct Cyc_List_List*_tmp6DD=({struct Cyc_Absyn_Decl*_tmp348[1U];_tmp348[0]=_tmp339;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp348,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp6DD,out_file);});}}}
# 1906
({struct Cyc___cycFILE*_tmp6DE=Cyc_fopen(_tmp270,"r");maybe=_tmp6DE;});
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp6E1=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34D=({struct Cyc_String_pa_PrintArg_struct _tmp4BE;_tmp4BE.tag=0U,({struct _dyneither_ptr _tmp6DF=(struct _dyneither_ptr)({const char*_tmp34E=_tmp270;_tag_dyneither(_tmp34E,sizeof(char),_get_zero_arr_size_char((void*)_tmp34E,1U));});_tmp4BE.f1=_tmp6DF;});_tmp4BE;});void*_tmp34B[1U];_tmp34B[0]=& _tmp34D;({struct _dyneither_ptr _tmp6E0=({const char*_tmp34C="can't open macrosfile %s";_tag_dyneither(_tmp34C,sizeof(char),25U);});Cyc_aprintf(_tmp6E0,_tag_dyneither(_tmp34B,sizeof(void*),1U));});});_tmp34F->f1=_tmp6E1;});_tmp34F;}));
# 1909
in_file=maybe;
({struct Cyc_Lexing_lexbuf*_tmp6E2=Cyc_Lexing_from_file(in_file);_tmp2CB=_tmp6E2;});{
struct _tuple22*entry2;
while(({struct _tuple22*_tmp6E3=((struct _tuple22*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp2CB);entry2=_tmp6E3;})!= 0){
struct _tuple22*_tmp350=(struct _tuple22*)_check_null(entry2);struct _tuple22*_tmp351=_tmp350;struct _dyneither_ptr _tmp35B;struct _dyneither_ptr*_tmp35A;_LL6E: _tmp35B=_tmp351->f1;_tmp35A=_tmp351->f2;_LL6F:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2E2,_tmp35A)){
({struct Cyc_String_pa_PrintArg_struct _tmp354=({struct Cyc_String_pa_PrintArg_struct _tmp4BF;_tmp4BF.tag=0U,_tmp4BF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35A);_tmp4BF;});void*_tmp352[1U];_tmp352[0]=& _tmp354;({struct Cyc___cycFILE*_tmp6E5=out_file;struct _dyneither_ptr _tmp6E4=({const char*_tmp353="#ifndef %s\n";_tag_dyneither(_tmp353,sizeof(char),12U);});Cyc_fprintf(_tmp6E5,_tmp6E4,_tag_dyneither(_tmp352,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp357=({struct Cyc_String_pa_PrintArg_struct _tmp4C0;_tmp4C0.tag=0U,_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp35B);_tmp4C0;});void*_tmp355[1U];_tmp355[0]=& _tmp357;({struct Cyc___cycFILE*_tmp6E7=out_file;struct _dyneither_ptr _tmp6E6=({const char*_tmp356="%s\n";_tag_dyneither(_tmp356,sizeof(char),4U);});Cyc_fprintf(_tmp6E7,_tmp6E6,_tag_dyneither(_tmp355,sizeof(void*),1U));});});
({void*_tmp358=0U;({struct Cyc___cycFILE*_tmp6E9=out_file;struct _dyneither_ptr _tmp6E8=({const char*_tmp359="#endif\n";_tag_dyneither(_tmp359,sizeof(char),8U);});Cyc_fprintf(_tmp6E9,_tmp6E8,_tag_dyneither(_tmp358,sizeof(void*),0U));});});}}
# 1920
Cyc_fclose(in_file);
if(Cyc_mode != Cyc_FINISH);
# 1923
if(hstubs != 0){
struct Cyc_List_List*_tmp35C=hstubs;for(0;_tmp35C != 0;_tmp35C=_tmp35C->tl){
struct _tuple23*_tmp35D=(struct _tuple23*)_tmp35C->hd;struct _tuple23*_tmp35E=_tmp35D;struct _dyneither_ptr _tmp364;struct _dyneither_ptr _tmp363;_LL71: _tmp364=_tmp35E->f1;_tmp363=_tmp35E->f2;_LL72:;
if(({char*_tmp6EC=(char*)_tmp363.curr;_tmp6EC != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6EB=(char*)_tmp364.curr;_tmp6EB == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6EA=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6EA,({struct _dyneither_ptr*_tmp35F=_cycalloc(sizeof(*_tmp35F));*_tmp35F=_tmp364;_tmp35F;}));})))
# 1929
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp363,sizeof(char),1U)),out_file);else{
# 1931
({struct Cyc_String_pa_PrintArg_struct _tmp362=({struct Cyc_String_pa_PrintArg_struct _tmp4C1;_tmp4C1.tag=0U,_tmp4C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp364);_tmp4C1;});void*_tmp360[1U];_tmp360[0]=& _tmp362;({struct _dyneither_ptr _tmp6ED=({const char*_tmp361="%s is not supported on this platform\n";_tag_dyneither(_tmp361,sizeof(char),38U);});Cyc_log(_tmp6ED,_tag_dyneither(_tmp360,sizeof(void*),1U));});});}}}
# 1934
({void*_tmp365=0U;({struct Cyc___cycFILE*_tmp6EF=out_file;struct _dyneither_ptr _tmp6EE=({const char*_tmp366="#endif\n";_tag_dyneither(_tmp366,sizeof(char),8U);});Cyc_fprintf(_tmp6EF,_tmp6EE,_tag_dyneither(_tmp365,sizeof(void*),0U));});});
if(Cyc_do_setjmp){int _tmp367=0;_npop_handler(0U);return _tmp367;}else{
Cyc_fclose(out_file);}
# 1939
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp368=cstubs;for(0;_tmp368 != 0;_tmp368=_tmp368->tl){
struct _tuple23*_tmp369=(struct _tuple23*)_tmp368->hd;struct _tuple23*_tmp36A=_tmp369;struct _dyneither_ptr _tmp36D;struct _dyneither_ptr _tmp36C;_LL74: _tmp36D=_tmp36A->f1;_tmp36C=_tmp36A->f2;_LL75:;
if(({char*_tmp6F2=(char*)_tmp36C.curr;_tmp6F2 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6F1=(char*)_tmp36D.curr;_tmp6F1 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6F0=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6F0,({struct _dyneither_ptr*_tmp36B=_cycalloc(sizeof(*_tmp36B));*_tmp36B=_tmp36D;_tmp36B;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp36C,sizeof(char),1U)),out_file);}};}
# 1950
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1952
({struct Cyc_String_pa_PrintArg_struct _tmp370=({struct Cyc_String_pa_PrintArg_struct _tmp4C2;_tmp4C2.tag=0U,({struct _dyneither_ptr _tmp6F3=(struct _dyneither_ptr)({const char*_tmp371=filename;_tag_dyneither(_tmp371,sizeof(char),_get_zero_arr_size_char((void*)_tmp371,1U));});_tmp4C2.f1=_tmp6F3;});_tmp4C2;});void*_tmp36E[1U];_tmp36E[0]=& _tmp370;({struct Cyc___cycFILE*_tmp6F5=out_file;struct _dyneither_ptr _tmp6F4=({const char*_tmp36F="#include <%s>\n\n";_tag_dyneither(_tmp36F,sizeof(char),16U);});Cyc_fprintf(_tmp6F5,_tmp6F4,_tag_dyneither(_tmp36E,sizeof(void*),1U));});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp372=cycstubs;for(0;_tmp372 != 0;_tmp372=_tmp372->tl){
struct _tuple23*_tmp373=(struct _tuple23*)_tmp372->hd;struct _tuple23*_tmp374=_tmp373;struct _dyneither_ptr _tmp377;struct _dyneither_ptr _tmp376;_LL77: _tmp377=_tmp374->f1;_tmp376=_tmp374->f2;_LL78:;
if(({char*_tmp6F8=(char*)_tmp376.curr;_tmp6F8 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp6F7=(char*)_tmp377.curr;_tmp6F7 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp6F6=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp6F6,({struct _dyneither_ptr*_tmp375=_cycalloc(sizeof(*_tmp375));*_tmp375=_tmp377;_tmp375;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp376,sizeof(char),1U)),out_file);}}
# 1961
({void*_tmp378=0U;({struct Cyc___cycFILE*_tmp6FA=out_file;struct _dyneither_ptr _tmp6F9=({const char*_tmp379="\n";_tag_dyneither(_tmp379,sizeof(char),2U);});Cyc_fprintf(_tmp6FA,_tmp6F9,_tag_dyneither(_tmp378,sizeof(void*),0U));});});}{
# 1964
int _tmp37A=0;_npop_handler(0U);return _tmp37A;};};};};};};};};};
# 1612
;_pop_handler();}else{void*_tmp274=(void*)_exn_thrown;void*_tmp37F=_tmp274;void*_tmp396;struct _dyneither_ptr _tmp395;struct _dyneither_ptr _tmp394;struct _dyneither_ptr _tmp393;struct _dyneither_ptr _tmp392;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp37F)->tag == Cyc_Core_Impossible){_LL7A: _tmp392=((struct Cyc_Core_Impossible_exn_struct*)_tmp37F)->f1;_LL7B:
# 1968
({struct Cyc_String_pa_PrintArg_struct _tmp382=({struct Cyc_String_pa_PrintArg_struct _tmp4C4;_tmp4C4.tag=0U,_tmp4C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp392);_tmp4C4;});void*_tmp380[1U];_tmp380[0]=& _tmp382;({struct Cyc___cycFILE*_tmp6FC=Cyc_stderr;struct _dyneither_ptr _tmp6FB=({const char*_tmp381="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp381,sizeof(char),26U);});Cyc_fprintf(_tmp6FC,_tmp6FB,_tag_dyneither(_tmp380,sizeof(void*),1U));});});goto _LL79;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp37F)->tag == Cyc_Dict_Absent){_LL7C: _LL7D:
# 1970
({void*_tmp383=0U;({struct Cyc___cycFILE*_tmp6FE=Cyc_stderr;struct _dyneither_ptr _tmp6FD=({const char*_tmp384="Got Dict::Absent\n";_tag_dyneither(_tmp384,sizeof(char),18U);});Cyc_fprintf(_tmp6FE,_tmp6FD,_tag_dyneither(_tmp383,sizeof(void*),0U));});});goto _LL79;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp37F)->tag == Cyc_Core_Failure){_LL7E: _tmp393=((struct Cyc_Core_Failure_exn_struct*)_tmp37F)->f1;_LL7F:
# 1972
({struct Cyc_String_pa_PrintArg_struct _tmp387=({struct Cyc_String_pa_PrintArg_struct _tmp4C5;_tmp4C5.tag=0U,_tmp4C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp393);_tmp4C5;});void*_tmp385[1U];_tmp385[0]=& _tmp387;({struct Cyc___cycFILE*_tmp700=Cyc_stderr;struct _dyneither_ptr _tmp6FF=({const char*_tmp386="Got Core::Failure(%s)\n";_tag_dyneither(_tmp386,sizeof(char),23U);});Cyc_fprintf(_tmp700,_tmp6FF,_tag_dyneither(_tmp385,sizeof(void*),1U));});});goto _LL79;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp37F)->tag == Cyc_Core_Invalid_argument){_LL80: _tmp394=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp37F)->f1;_LL81:
# 1974
({struct Cyc_String_pa_PrintArg_struct _tmp38A=({struct Cyc_String_pa_PrintArg_struct _tmp4C6;_tmp4C6.tag=0U,_tmp4C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp394);_tmp4C6;});void*_tmp388[1U];_tmp388[0]=& _tmp38A;({struct Cyc___cycFILE*_tmp702=Cyc_stderr;struct _dyneither_ptr _tmp701=({const char*_tmp389="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp389,sizeof(char),26U);});Cyc_fprintf(_tmp702,_tmp701,_tag_dyneither(_tmp388,sizeof(void*),1U));});});goto _LL79;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp37F)->tag == Cyc_Core_Not_found){_LL82: _LL83:
# 1976
({void*_tmp38B=0U;({struct Cyc___cycFILE*_tmp704=Cyc_stderr;struct _dyneither_ptr _tmp703=({const char*_tmp38C="Got Not_found\n";_tag_dyneither(_tmp38C,sizeof(char),15U);});Cyc_fprintf(_tmp704,_tmp703,_tag_dyneither(_tmp38B,sizeof(void*),0U));});});goto _LL79;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp37F)->tag == Cyc_NO_SUPPORT){_LL84: _tmp395=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp37F)->f1;_LL85:
# 1978
({struct Cyc_String_pa_PrintArg_struct _tmp38F=({struct Cyc_String_pa_PrintArg_struct _tmp4C7;_tmp4C7.tag=0U,_tmp4C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp395);_tmp4C7;});void*_tmp38D[1U];_tmp38D[0]=& _tmp38F;({struct Cyc___cycFILE*_tmp706=Cyc_stderr;struct _dyneither_ptr _tmp705=({const char*_tmp38E="No support because %s\n";_tag_dyneither(_tmp38E,sizeof(char),23U);});Cyc_fprintf(_tmp706,_tmp705,_tag_dyneither(_tmp38D,sizeof(void*),1U));});});goto _LL79;}else{_LL86: _tmp396=_tmp37F;_LL87:
# 1980
({void*_tmp390=0U;({struct Cyc___cycFILE*_tmp708=Cyc_stderr;struct _dyneither_ptr _tmp707=({const char*_tmp391="Got unknown exception\n";_tag_dyneither(_tmp391,sizeof(char),23U);});Cyc_fprintf(_tmp708,_tmp707,_tag_dyneither(_tmp390,sizeof(void*),0U));});});
Cyc_Core_rethrow(_tmp396);}}}}}}_LL79:;}};}
# 1986
({struct Cyc___cycFILE*_tmp709=Cyc_fopen(filename,"w");maybe=_tmp709;});
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp399=({struct Cyc_String_pa_PrintArg_struct _tmp4C8;_tmp4C8.tag=0U,({struct _dyneither_ptr _tmp70A=(struct _dyneither_ptr)({const char*_tmp39A=filename;_tag_dyneither(_tmp39A,sizeof(char),_get_zero_arr_size_char((void*)_tmp39A,1U));});_tmp4C8.f1=_tmp70A;});_tmp4C8;});void*_tmp397[1U];_tmp397[0]=& _tmp399;({struct Cyc___cycFILE*_tmp70C=Cyc_stderr;struct _dyneither_ptr _tmp70B=({const char*_tmp398="Error: could not create file %s\n";_tag_dyneither(_tmp398,sizeof(char),33U);});Cyc_fprintf(_tmp70C,_tmp70B,_tag_dyneither(_tmp397,sizeof(void*),1U));});});
return 1;}
# 1991
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp39D=({struct Cyc_String_pa_PrintArg_struct _tmp4C9;_tmp4C9.tag=0U,({
# 1994
struct _dyneither_ptr _tmp70D=(struct _dyneither_ptr)({const char*_tmp39E=filename;_tag_dyneither(_tmp39E,sizeof(char),_get_zero_arr_size_char((void*)_tmp39E,1U));});_tmp4C9.f1=_tmp70D;});_tmp4C9;});void*_tmp39B[1U];_tmp39B[0]=& _tmp39D;({struct Cyc___cycFILE*_tmp70F=out_file;struct _dyneither_ptr _tmp70E=({const char*_tmp39C="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp39C,sizeof(char),48U);});Cyc_fprintf(_tmp70F,_tmp70E,_tag_dyneither(_tmp39B,sizeof(void*),1U));});});
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp3A1=({struct Cyc_String_pa_PrintArg_struct _tmp4CA;_tmp4CA.tag=0U,({
struct _dyneither_ptr _tmp710=(struct _dyneither_ptr)({const char*_tmp3A2=filename;_tag_dyneither(_tmp3A2,sizeof(char),_get_zero_arr_size_char((void*)_tmp3A2,1U));});_tmp4CA.f1=_tmp710;});_tmp4CA;});void*_tmp39F[1U];_tmp39F[0]=& _tmp3A1;({struct Cyc___cycFILE*_tmp712=Cyc_stderr;struct _dyneither_ptr _tmp711=({const char*_tmp3A0="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp3A0,sizeof(char),52U);});Cyc_fprintf(_tmp712,_tmp711,_tag_dyneither(_tmp39F,sizeof(void*),1U));});});
({void*_tmp3A3=0U;({struct _dyneither_ptr _tmp713=({const char*_tmp3A4="Not supported on this platform\n";_tag_dyneither(_tmp3A4,sizeof(char),32U);});Cyc_log(_tmp713,_tag_dyneither(_tmp3A3,sizeof(void*),0U));});});
# 2005
return 0;};}
# 2009
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3BC=({struct Cyc_String_pa_PrintArg_struct _tmp4D1;_tmp4D1.tag=0U,({struct _dyneither_ptr _tmp714=(struct _dyneither_ptr)({const char*_tmp3BD=file;_tag_dyneither(_tmp3BD,sizeof(char),_get_zero_arr_size_char((void*)_tmp3BD,1U));});_tmp4D1.f1=_tmp714;});_tmp4D1;});void*_tmp3BA[1U];_tmp3BA[0]=& _tmp3BC;({struct Cyc___cycFILE*_tmp716=Cyc_stderr;struct _dyneither_ptr _tmp715=({const char*_tmp3BB="Processing %s\n";_tag_dyneither(_tmp3BB,sizeof(char),15U);});Cyc_fprintf(_tmp716,_tmp715,_tag_dyneither(_tmp3BA,sizeof(void*),1U));});});{
struct Cyc___cycFILE*_tmp3BE=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp3BE)){
({struct Cyc_String_pa_PrintArg_struct _tmp3C1=({struct Cyc_String_pa_PrintArg_struct _tmp4D2;_tmp4D2.tag=0U,({struct _dyneither_ptr _tmp717=(struct _dyneither_ptr)({const char*_tmp3C2=file;_tag_dyneither(_tmp3C2,sizeof(char),_get_zero_arr_size_char((void*)_tmp3C2,1U));});_tmp4D2.f1=_tmp717;});_tmp4D2;});void*_tmp3BF[1U];_tmp3BF[0]=& _tmp3C1;({struct Cyc___cycFILE*_tmp719=Cyc_stderr;struct _dyneither_ptr _tmp718=({const char*_tmp3C0="Error: could not open %s\n";_tag_dyneither(_tmp3C0,sizeof(char),26U);});Cyc_fprintf(_tmp719,_tmp718,_tag_dyneither(_tmp3BF,sizeof(void*),1U));});});
return 1;}{
# 2017
struct Cyc___cycFILE*_tmp3C3=_tmp3BE;
# 2021
struct _dyneither_ptr buf=({char*_tmp3E1=({unsigned int _tmp3E0=(unsigned int)1024 + 1U;char*_tmp3DF=_cycalloc_atomic(_check_times(_tmp3E0,sizeof(char)));({{unsigned int _tmp4D8=1024U;unsigned int i;for(i=0;i < _tmp4D8;++ i){_tmp3DF[i]='\000';}_tmp3DF[_tmp4D8]=0;}0;});_tmp3DF;});_tag_dyneither(_tmp3E1,sizeof(char),1025U);});
struct _dyneither_ptr _tmp3C4=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp3C7=({struct Cyc_String_pa_PrintArg_struct _tmp4D3;_tmp4D3.tag=0U,({struct _dyneither_ptr _tmp71A=(struct _dyneither_ptr)({const char*_tmp3C8=dir;_tag_dyneither(_tmp3C8,sizeof(char),_get_zero_arr_size_char((void*)_tmp3C8,1U));});_tmp4D3.f1=_tmp71A;});_tmp4D3;});void*_tmp3C5[1U];_tmp3C5[0]=& _tmp3C7;({struct Cyc___cycFILE*_tmp71C=Cyc_stderr;struct _dyneither_ptr _tmp71B=({const char*_tmp3C6="Error: can't change directory to %s\n";_tag_dyneither(_tmp3C6,sizeof(char),37U);});Cyc_fprintf(_tmp71C,_tmp71B,_tag_dyneither(_tmp3C5,sizeof(void*),1U));});});
return 1;}}
# 2029
if(Cyc_mode == Cyc_GATHER){
# 2031
struct _dyneither_ptr _tmp3C9=({struct Cyc_String_pa_PrintArg_struct _tmp3CF=({struct Cyc_String_pa_PrintArg_struct _tmp4D6;_tmp4D6.tag=0U,_tmp4D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4D6;});struct Cyc_String_pa_PrintArg_struct _tmp3D0=({struct Cyc_String_pa_PrintArg_struct _tmp4D5;_tmp4D5.tag=0U,_tmp4D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4D5;});void*_tmp3CD[2U];_tmp3CD[0]=& _tmp3CF,_tmp3CD[1]=& _tmp3D0;({struct _dyneither_ptr _tmp71D=({const char*_tmp3CE="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp3CE,sizeof(char),39U);});Cyc_aprintf(_tmp71D,_tag_dyneither(_tmp3CD,sizeof(void*),2U));});});
# 2033
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3CC=({struct Cyc_String_pa_PrintArg_struct _tmp4D4;_tmp4D4.tag=0U,_tmp4D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C9);_tmp4D4;});void*_tmp3CA[1U];_tmp3CA[0]=& _tmp3CC;({struct Cyc___cycFILE*_tmp71F=Cyc_stderr;struct _dyneither_ptr _tmp71E=({const char*_tmp3CB="%s\n";_tag_dyneither(_tmp3CB,sizeof(char),4U);});Cyc_fprintf(_tmp71F,_tmp71E,_tag_dyneither(_tmp3CA,sizeof(void*),1U));});});
 system((const char*)_check_null(_untag_dyneither_ptr(_tmp3C9,sizeof(char),1U)));}{
# 2038
struct Cyc_Lexing_lexbuf*_tmp3D1=Cyc_Lexing_from_file(_tmp3C3);
struct _tuple25*entry;
while(({struct _tuple25*_tmp720=((struct _tuple25*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp3D1);entry=_tmp720;})!= 0){
struct _tuple25*_tmp3D2=(struct _tuple25*)_check_null(entry);struct _tuple25*_tmp3D3=_tmp3D2;struct _dyneither_ptr _tmp3DB;struct Cyc_List_List*_tmp3DA;struct Cyc_List_List*_tmp3D9;struct Cyc_List_List*_tmp3D8;struct Cyc_List_List*_tmp3D7;struct Cyc_List_List*_tmp3D6;struct Cyc_List_List*_tmp3D5;struct Cyc_List_List*_tmp3D4;_LL1: _tmp3DB=_tmp3D3->f1;_tmp3DA=_tmp3D3->f2;_tmp3D9=_tmp3D3->f3;_tmp3D8=_tmp3D3->f4;_tmp3D7=_tmp3D3->f5;_tmp3D6=_tmp3D3->f6;_tmp3D5=_tmp3D3->f7;_tmp3D4=_tmp3D3->f8;_LL2:;
# 2043
if(Cyc_process_file((const char*)_check_null(_untag_dyneither_ptr(_tmp3DB,sizeof(char),1U)),_tmp3DA,_tmp3D9,_tmp3D8,_tmp3D7,_tmp3D6,_tmp3D5,_tmp3D4))
# 2045
return 1;}
# 2047
Cyc_fclose(_tmp3C3);
# 2049
if(Cyc_mode != Cyc_GATHERSCRIPT){
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3C4,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp3DE=({struct Cyc_String_pa_PrintArg_struct _tmp4D7;_tmp4D7.tag=0U,_tmp4D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C4);_tmp4D7;});void*_tmp3DC[1U];_tmp3DC[0]=& _tmp3DE;({struct Cyc___cycFILE*_tmp722=Cyc_stderr;struct _dyneither_ptr _tmp721=({const char*_tmp3DD="Error: could not change directory to %s\n";_tag_dyneither(_tmp3DD,sizeof(char),41U);});Cyc_fprintf(_tmp722,_tmp721,_tag_dyneither(_tmp3DC,sizeof(void*),1U));});});
return 1;}}
# 2055
return 0;};};};}
# 2059
int Cyc_process_setjmp(const char*dir){
# 2062
struct _dyneither_ptr buf=({char*_tmp3F3=({unsigned int _tmp3F2=(unsigned int)1024 + 1U;char*_tmp3F1=_cycalloc_atomic(_check_times(_tmp3F2,sizeof(char)));({{unsigned int _tmp4DB=1024U;unsigned int i;for(i=0;i < _tmp4DB;++ i){_tmp3F1[i]='\000';}_tmp3F1[_tmp4DB]=0;}0;});_tmp3F1;});_tag_dyneither(_tmp3F3,sizeof(char),1025U);});
struct _dyneither_ptr _tmp3E2=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp3E5=({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0U,({struct _dyneither_ptr _tmp723=(struct _dyneither_ptr)({const char*_tmp3E6=dir;_tag_dyneither(_tmp3E6,sizeof(char),_get_zero_arr_size_char((void*)_tmp3E6,1U));});_tmp4D9.f1=_tmp723;});_tmp4D9;});void*_tmp3E3[1U];_tmp3E3[0]=& _tmp3E5;({struct Cyc___cycFILE*_tmp725=Cyc_stderr;struct _dyneither_ptr _tmp724=({const char*_tmp3E4="Error: can't change directory to %s\n";_tag_dyneither(_tmp3E4,sizeof(char),37U);});Cyc_fprintf(_tmp725,_tmp724,_tag_dyneither(_tmp3E3,sizeof(void*),1U));});});
return 1;}
# 2068
if(({struct Cyc_List_List*_tmp72B=({struct _dyneither_ptr*_tmp3E7[1U];({struct _dyneither_ptr*_tmp727=({struct _dyneither_ptr*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({struct _dyneither_ptr _tmp726=({const char*_tmp3E8="jmp_buf";_tag_dyneither(_tmp3E8,sizeof(char),8U);});*_tmp3E9=_tmp726;});_tmp3E9;});_tmp3E7[0]=_tmp727;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3E7,sizeof(struct _dyneither_ptr*),1U));});Cyc_process_file("setjmp.h",_tmp72B,0,0,({struct _tuple23*_tmp3EA[1U];({struct _tuple23*_tmp72A=({struct _tuple23*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));
({struct _dyneither_ptr _tmp729=({const char*_tmp3EB="setjmp";_tag_dyneither(_tmp3EB,sizeof(char),7U);});_tmp3ED->f1=_tmp729;}),({struct _dyneither_ptr _tmp728=({const char*_tmp3EC="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp3EC,sizeof(char),29U);});_tmp3ED->f2=_tmp728;});_tmp3ED;});_tmp3EA[0]=_tmp72A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3EA,sizeof(struct _tuple23*),1U));}),0,0,0);}))
# 2071
return 1;
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3E2,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp3F0=({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0U,_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3E2);_tmp4DA;});void*_tmp3EE[1U];_tmp3EE[0]=& _tmp3F0;({struct Cyc___cycFILE*_tmp72D=Cyc_stderr;struct _dyneither_ptr _tmp72C=({const char*_tmp3EF="Error: could not change directory to %s\n";_tag_dyneither(_tmp3EF,sizeof(char),41U);});Cyc_fprintf(_tmp72D,_tmp72C,_tag_dyneither(_tmp3EE,sizeof(void*),1U));});});
return 1;}
# 2076
return 0;}static char _tmp3F4[13U]="BUILDLIB.OUT";
# 2080
static struct _dyneither_ptr Cyc_output_dir={_tmp3F4,_tmp3F4,_tmp3F4 + 13U};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 2084
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp72E=({struct Cyc_List_List*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->hd=(const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1U)),_tmp3F5->tl=Cyc_spec_files;_tmp3F5;});Cyc_spec_files=_tmp72E;});}
# 2088
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 2092
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 2095
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 2098
static void Cyc_add_cpparg(struct _dyneither_ptr s){
({struct Cyc_List_List*_tmp730=({struct Cyc_List_List*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));({struct _dyneither_ptr*_tmp72F=({struct _dyneither_ptr*_tmp3F6=_cycalloc(sizeof(*_tmp3F6));*_tmp3F6=s;_tmp3F6;});_tmp3F7->hd=_tmp72F;}),_tmp3F7->tl=Cyc_cppargs;_tmp3F7;});Cyc_cppargs=_tmp730;});}
# 2101
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp3FA=({struct Cyc_String_pa_PrintArg_struct _tmp4DC;_tmp4DC.tag=0U,_tmp4DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp4DC;});void*_tmp3F8[1U];_tmp3F8[0]=& _tmp3FA;({struct Cyc___cycFILE*_tmp732=Cyc_stderr;struct _dyneither_ptr _tmp731=({const char*_tmp3F9="Unsupported option %s\n";_tag_dyneither(_tmp3F9,sizeof(char),23U);});Cyc_fprintf(_tmp732,_tmp731,_tag_dyneither(_tmp3F8,sizeof(void*),1U));});});
Cyc_badparse=1;}
# 2110
void GC_blacklist_warn_clear();struct _tuple29{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
 GC_blacklist_warn_clear();{
# 2114
struct Cyc_List_List*options=({struct _tuple29*_tmp432[9U];({
struct _tuple29*_tmp75F=({struct _tuple29*_tmp437=_cycalloc(sizeof(*_tmp437));({struct _dyneither_ptr _tmp75E=({const char*_tmp433="-d";_tag_dyneither(_tmp433,sizeof(char),3U);});_tmp437->f1=_tmp75E;}),_tmp437->f2=0,({struct _dyneither_ptr _tmp75D=({const char*_tmp434=" <file>";_tag_dyneither(_tmp434,sizeof(char),8U);});_tmp437->f3=_tmp75D;}),({
void*_tmp75C=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->tag=5U,_tmp435->f1=Cyc_set_output_dir;_tmp435;});_tmp437->f4=_tmp75C;}),({
struct _dyneither_ptr _tmp75B=({const char*_tmp436="Set the output directory to <file>";_tag_dyneither(_tmp436,sizeof(char),35U);});_tmp437->f5=_tmp75B;});_tmp437;});
# 2115
_tmp432[0]=_tmp75F;}),({
# 2118
struct _tuple29*_tmp75A=({struct _tuple29*_tmp43C=_cycalloc(sizeof(*_tmp43C));({struct _dyneither_ptr _tmp759=({const char*_tmp438="-gather";_tag_dyneither(_tmp438,sizeof(char),8U);});_tmp43C->f1=_tmp759;}),_tmp43C->f2=0,({struct _dyneither_ptr _tmp758=({const char*_tmp439="";_tag_dyneither(_tmp439,sizeof(char),1U);});_tmp43C->f3=_tmp758;}),({
void*_tmp757=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=0U,_tmp43A->f1=Cyc_set_GATHER;_tmp43A;});_tmp43C->f4=_tmp757;}),({
struct _dyneither_ptr _tmp756=({const char*_tmp43B="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp43B,sizeof(char),56U);});_tmp43C->f5=_tmp756;});_tmp43C;});
# 2118
_tmp432[1]=_tmp75A;}),({
# 2121
struct _tuple29*_tmp755=({struct _tuple29*_tmp441=_cycalloc(sizeof(*_tmp441));({struct _dyneither_ptr _tmp754=({const char*_tmp43D="-gatherscript";_tag_dyneither(_tmp43D,sizeof(char),14U);});_tmp441->f1=_tmp754;}),_tmp441->f2=0,({struct _dyneither_ptr _tmp753=({const char*_tmp43E="";_tag_dyneither(_tmp43E,sizeof(char),1U);});_tmp441->f3=_tmp753;}),({
void*_tmp752=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=0U,_tmp43F->f1=Cyc_set_GATHERSCRIPT;_tmp43F;});_tmp441->f4=_tmp752;}),({
struct _dyneither_ptr _tmp751=({const char*_tmp440="Produce a script to gather C library info";_tag_dyneither(_tmp440,sizeof(char),42U);});_tmp441->f5=_tmp751;});_tmp441;});
# 2121
_tmp432[2]=_tmp755;}),({
# 2124
struct _tuple29*_tmp750=({struct _tuple29*_tmp446=_cycalloc(sizeof(*_tmp446));({struct _dyneither_ptr _tmp74F=({const char*_tmp442="-finish";_tag_dyneither(_tmp442,sizeof(char),8U);});_tmp446->f1=_tmp74F;}),_tmp446->f2=0,({struct _dyneither_ptr _tmp74E=({const char*_tmp443="";_tag_dyneither(_tmp443,sizeof(char),1U);});_tmp446->f3=_tmp74E;}),({
void*_tmp74D=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->tag=0U,_tmp444->f1=Cyc_set_FINISH;_tmp444;});_tmp446->f4=_tmp74D;}),({
struct _dyneither_ptr _tmp74C=({const char*_tmp445="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp445,sizeof(char),57U);});_tmp446->f5=_tmp74C;});_tmp446;});
# 2124
_tmp432[3]=_tmp750;}),({
# 2127
struct _tuple29*_tmp74B=({struct _tuple29*_tmp44B=_cycalloc(sizeof(*_tmp44B));({struct _dyneither_ptr _tmp74A=({const char*_tmp447="-setjmp";_tag_dyneither(_tmp447,sizeof(char),8U);});_tmp44B->f1=_tmp74A;}),_tmp44B->f2=0,({struct _dyneither_ptr _tmp749=({const char*_tmp448="";_tag_dyneither(_tmp448,sizeof(char),1U);});_tmp44B->f3=_tmp749;}),({
void*_tmp748=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->tag=3U,_tmp449->f1=& Cyc_do_setjmp;_tmp449;});_tmp44B->f4=_tmp748;}),({
# 2132
struct _dyneither_ptr _tmp747=({const char*_tmp44A="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp44A,sizeof(char),186U);});_tmp44B->f5=_tmp747;});_tmp44B;});
# 2127
_tmp432[4]=_tmp74B;}),({
# 2133
struct _tuple29*_tmp746=({struct _tuple29*_tmp450=_cycalloc(sizeof(*_tmp450));({struct _dyneither_ptr _tmp745=({const char*_tmp44C="-b";_tag_dyneither(_tmp44C,sizeof(char),3U);});_tmp450->f1=_tmp745;}),_tmp450->f2=0,({struct _dyneither_ptr _tmp744=({const char*_tmp44D=" <machine>";_tag_dyneither(_tmp44D,sizeof(char),11U);});_tmp450->f3=_tmp744;}),({
void*_tmp743=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp44E=_cycalloc(sizeof(*_tmp44E));_tmp44E->tag=5U,_tmp44E->f1=Cyc_Specsfile_set_target_arch;_tmp44E;});_tmp450->f4=_tmp743;}),({
struct _dyneither_ptr _tmp742=({const char*_tmp44F="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp44F,sizeof(char),52U);});_tmp450->f5=_tmp742;});_tmp450;});
# 2133
_tmp432[5]=_tmp746;}),({
# 2136
struct _tuple29*_tmp741=({struct _tuple29*_tmp455=_cycalloc(sizeof(*_tmp455));({struct _dyneither_ptr _tmp740=({const char*_tmp451="-B";_tag_dyneither(_tmp451,sizeof(char),3U);});_tmp455->f1=_tmp740;}),_tmp455->f2=1,({struct _dyneither_ptr _tmp73F=({const char*_tmp452="<file>";_tag_dyneither(_tmp452,sizeof(char),7U);});_tmp455->f3=_tmp73F;}),({
void*_tmp73E=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp453=_cycalloc(sizeof(*_tmp453));_tmp453->tag=1U,_tmp453->f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp453;});_tmp455->f4=_tmp73E;}),({
struct _dyneither_ptr _tmp73D=({const char*_tmp454="Add to the list of directories to search for compiler files";_tag_dyneither(_tmp454,sizeof(char),60U);});_tmp455->f5=_tmp73D;});_tmp455;});
# 2136
_tmp432[6]=_tmp741;}),({
# 2139
struct _tuple29*_tmp73C=({struct _tuple29*_tmp45A=_cycalloc(sizeof(*_tmp45A));({struct _dyneither_ptr _tmp73B=({const char*_tmp456="-v";_tag_dyneither(_tmp456,sizeof(char),3U);});_tmp45A->f1=_tmp73B;}),_tmp45A->f2=0,({struct _dyneither_ptr _tmp73A=({const char*_tmp457="";_tag_dyneither(_tmp457,sizeof(char),1U);});_tmp45A->f3=_tmp73A;}),({
void*_tmp739=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp458=_cycalloc(sizeof(*_tmp458));_tmp458->tag=3U,_tmp458->f1=& Cyc_verbose;_tmp458;});_tmp45A->f4=_tmp739;}),({
struct _dyneither_ptr _tmp738=({const char*_tmp459="Verbose operation";_tag_dyneither(_tmp459,sizeof(char),18U);});_tmp45A->f5=_tmp738;});_tmp45A;});
# 2139
_tmp432[7]=_tmp73C;}),({
# 2142
struct _tuple29*_tmp737=({struct _tuple29*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct _dyneither_ptr _tmp736=({const char*_tmp45B="-";_tag_dyneither(_tmp45B,sizeof(char),2U);});_tmp45F->f1=_tmp736;}),_tmp45F->f2=1,({struct _dyneither_ptr _tmp735=({const char*_tmp45C="";_tag_dyneither(_tmp45C,sizeof(char),1U);});_tmp45F->f3=_tmp735;}),({
void*_tmp734=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp45D=_cycalloc(sizeof(*_tmp45D));_tmp45D->tag=1U,_tmp45D->f1=Cyc_add_cpparg;_tmp45D;});_tmp45F->f4=_tmp734;}),({
struct _dyneither_ptr _tmp733=({const char*_tmp45E="";_tag_dyneither(_tmp45E,sizeof(char),1U);});_tmp45F->f5=_tmp733;});_tmp45F;});
# 2142
_tmp432[8]=_tmp737;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp432,sizeof(struct _tuple29*),9U));});
# 2147
struct _dyneither_ptr _tmp3FB=({struct Cyc_List_List*_tmp761=options;struct _dyneither_ptr _tmp760=({const char*_tmp431="Options:";_tag_dyneither(_tmp431,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp761,Cyc_add_spec_file,Cyc_no_other,_tmp760,argv);});
# 2149
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp763=options;struct _dyneither_ptr _tmp762=({const char*_tmp3FC="Options:";_tag_dyneither(_tmp3FC,sizeof(char),9U);});Cyc_Arg_parse(_tmp763,Cyc_add_spec_file,Cyc_no_other,_tmp762,_tmp3FB);});
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHER) || 
Cyc_do_setjmp  && Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_List_List*_tmp764=options;Cyc_Arg_usage(_tmp764,({const char*_tmp3FD="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp3FD,sizeof(char),59U);}));});
# 2159
return 1;}{
# 2165
struct _dyneither_ptr _tmp3FE=Cyc_Specsfile_find_in_arch_path(({const char*_tmp430="cycspecs";_tag_dyneither(_tmp430,sizeof(char),9U);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp401=({struct Cyc_String_pa_PrintArg_struct _tmp4DD;_tmp4DD.tag=0U,_tmp4DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3FE);_tmp4DD;});void*_tmp3FF[1U];_tmp3FF[0]=& _tmp401;({struct Cyc___cycFILE*_tmp766=Cyc_stderr;struct _dyneither_ptr _tmp765=({const char*_tmp400="Reading from specs file %s\n";_tag_dyneither(_tmp400,sizeof(char),28U);});Cyc_fprintf(_tmp766,_tmp765,_tag_dyneither(_tmp3FF,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp402=Cyc_Specsfile_read_specs(_tmp3FE);
({struct _dyneither_ptr _tmp768=({struct Cyc_List_List*_tmp767=_tmp402;Cyc_Specsfile_get_spec(_tmp767,({const char*_tmp403="cyclone_target_cflags";_tag_dyneither(_tmp403,sizeof(char),22U);}));});Cyc_target_cflags=_tmp768;});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp406=({struct Cyc_String_pa_PrintArg_struct _tmp4DE;_tmp4DE.tag=0U,_tmp4DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4DE;});void*_tmp404[1U];_tmp404[0]=& _tmp406;({struct Cyc___cycFILE*_tmp76A=Cyc_stderr;struct _dyneither_ptr _tmp769=({const char*_tmp405="Target cflags are %s\n";_tag_dyneither(_tmp405,sizeof(char),22U);});Cyc_fprintf(_tmp76A,_tmp769,_tag_dyneither(_tmp404,sizeof(void*),1U));});});
({struct _dyneither_ptr _tmp76C=({struct Cyc_List_List*_tmp76B=_tmp402;Cyc_Specsfile_get_spec(_tmp76B,({const char*_tmp407="cyclone_cc";_tag_dyneither(_tmp407,sizeof(char),11U);}));});Cyc_cyclone_cc=_tmp76C;});
if(!((unsigned int)Cyc_cyclone_cc.curr))({struct _dyneither_ptr _tmp76D=({const char*_tmp408="gcc";_tag_dyneither(_tmp408,sizeof(char),4U);});Cyc_cyclone_cc=_tmp76D;});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp40B=({struct Cyc_String_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=0U,_tmp4DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4DF;});void*_tmp409[1U];_tmp409[0]=& _tmp40B;({struct Cyc___cycFILE*_tmp76F=Cyc_stderr;struct _dyneither_ptr _tmp76E=({const char*_tmp40A="C compiler is %s\n";_tag_dyneither(_tmp40A,sizeof(char),18U);});Cyc_fprintf(_tmp76F,_tmp76E,_tag_dyneither(_tmp409,sizeof(void*),1U));});});
# 2174
if(Cyc_mode == Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp40C=0U;({struct Cyc___cycFILE*_tmp771=Cyc_stderr;struct _dyneither_ptr _tmp770=({const char*_tmp40D="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp40D,sizeof(char),22U);});Cyc_fprintf(_tmp771,_tmp770,_tag_dyneither(_tmp40C,sizeof(void*),0U));});});
({struct Cyc___cycFILE*_tmp772=Cyc_fopen("BUILDLIB.sh","w");Cyc_script_file=_tmp772;});
if(!((unsigned int)Cyc_script_file)){
({void*_tmp40E=0U;({struct Cyc___cycFILE*_tmp774=Cyc_stderr;struct _dyneither_ptr _tmp773=({const char*_tmp40F="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp40F,sizeof(char),35U);});Cyc_fprintf(_tmp774,_tmp773,_tag_dyneither(_tmp40E,sizeof(void*),0U));});});
 exit(1);}
# 2182
({void*_tmp410=0U;({struct _dyneither_ptr _tmp775=({const char*_tmp411="#!/bin/sh\n";_tag_dyneither(_tmp411,sizeof(char),11U);});Cyc_prscript(_tmp775,_tag_dyneither(_tmp410,sizeof(void*),0U));});});
({void*_tmp412=0U;({struct _dyneither_ptr _tmp776=({const char*_tmp413="GCC=\"gcc\"\n";_tag_dyneither(_tmp413,sizeof(char),11U);});Cyc_prscript(_tmp776,_tag_dyneither(_tmp412,sizeof(void*),0U));});});}
# 2187
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp416=({struct Cyc_String_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=0U,_tmp4E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4E0;});void*_tmp414[1U];_tmp414[0]=& _tmp416;({struct Cyc___cycFILE*_tmp778=Cyc_stderr;struct _dyneither_ptr _tmp777=({const char*_tmp415="Error: could not create directory %s\n";_tag_dyneither(_tmp415,sizeof(char),38U);});Cyc_fprintf(_tmp778,_tmp777,_tag_dyneither(_tmp414,sizeof(void*),1U));});});
return 1;}
# 2191
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp419=({struct Cyc_String_pa_PrintArg_struct _tmp4E1;_tmp4E1.tag=0U,_tmp4E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4E1;});void*_tmp417[1U];_tmp417[0]=& _tmp419;({struct Cyc___cycFILE*_tmp77A=Cyc_stderr;struct _dyneither_ptr _tmp779=({const char*_tmp418="Output directory is %s\n";_tag_dyneither(_tmp418,sizeof(char),24U);});Cyc_fprintf(_tmp77A,_tmp779,_tag_dyneither(_tmp417,sizeof(void*),1U));});});
# 2194
if(Cyc_mode == Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp41C=({struct Cyc_String_pa_PrintArg_struct _tmp4E2;_tmp4E2.tag=0U,_tmp4E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4E2;});void*_tmp41A[1U];_tmp41A[0]=& _tmp41C;({struct _dyneither_ptr _tmp77B=({const char*_tmp41B="cd %s\n";_tag_dyneither(_tmp41B,sizeof(char),7U);});Cyc_prscript(_tmp77B,_tag_dyneither(_tmp41A,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp41F=({struct Cyc_String_pa_PrintArg_struct _tmp4E3;_tmp4E3.tag=0U,_tmp4E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4E3;});void*_tmp41D[1U];_tmp41D[0]=& _tmp41F;({struct _dyneither_ptr _tmp77C=({const char*_tmp41E="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp41E,sizeof(char),41U);});Cyc_prscript(_tmp77C,_tag_dyneither(_tmp41D,sizeof(void*),1U));});});}
# 2200
if(!Cyc_gathering()){
# 2203
({struct Cyc___cycFILE*_tmp77E=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp77D=Cyc_output_dir;Cyc_Filename_concat(_tmp77D,({const char*_tmp420="BUILDLIB.LOG";_tag_dyneither(_tmp420,sizeof(char),13U);}));}),sizeof(char),1U)),"w");Cyc_log_file=_tmp77E;});
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp423=({struct Cyc_String_pa_PrintArg_struct _tmp4E4;_tmp4E4.tag=0U,_tmp4E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4E4;});void*_tmp421[1U];_tmp421[0]=& _tmp423;({struct Cyc___cycFILE*_tmp780=Cyc_stderr;struct _dyneither_ptr _tmp77F=({const char*_tmp422="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp422,sizeof(char),50U);});Cyc_fprintf(_tmp780,_tmp77F,_tag_dyneither(_tmp421,sizeof(void*),1U));});});
return 1;}
# 2209
if(!Cyc_do_setjmp){
# 2211
({struct Cyc___cycFILE*_tmp782=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp781=Cyc_output_dir;Cyc_Filename_concat(_tmp781,({const char*_tmp424="cstubs.c";_tag_dyneither(_tmp424,sizeof(char),9U);}));}),sizeof(char),1U)),"w");Cyc_cstubs_file=_tmp782;});
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp427=({struct Cyc_String_pa_PrintArg_struct _tmp4E5;_tmp4E5.tag=0U,_tmp4E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4E5;});void*_tmp425[1U];_tmp425[0]=& _tmp427;({struct Cyc___cycFILE*_tmp784=Cyc_stderr;struct _dyneither_ptr _tmp783=({const char*_tmp426="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp426,sizeof(char),50U);});Cyc_fprintf(_tmp784,_tmp783,_tag_dyneither(_tmp425,sizeof(void*),1U));});});
return 1;}
# 2218
({struct Cyc___cycFILE*_tmp786=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp785=Cyc_output_dir;Cyc_Filename_concat(_tmp785,({const char*_tmp428="cycstubs.cyc";_tag_dyneither(_tmp428,sizeof(char),13U);}));}),sizeof(char),1U)),"w");Cyc_cycstubs_file=_tmp786;});
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp42B=({struct Cyc_String_pa_PrintArg_struct _tmp4E6;_tmp4E6.tag=0U,_tmp4E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4E6;});void*_tmp429[1U];_tmp429[0]=& _tmp42B;({struct Cyc___cycFILE*_tmp788=Cyc_stderr;struct _dyneither_ptr _tmp787=({const char*_tmp42A="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp42A,sizeof(char),52U);});Cyc_fprintf(_tmp788,_tmp787,_tag_dyneither(_tmp429,sizeof(void*),1U));});});
# 2223
return 1;}
# 2225
({void*_tmp42C=0U;({struct Cyc___cycFILE*_tmp78A=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _dyneither_ptr _tmp789=({const char*_tmp42D="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp42D,sizeof(char),32U);});Cyc_fprintf(_tmp78A,_tmp789,_tag_dyneither(_tmp42C,sizeof(void*),0U));});});}}{
# 2232
const char*outdir=(const char*)_check_null(_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 2238
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp42E=0U;({struct Cyc___cycFILE*_tmp78C=Cyc_stderr;struct _dyneither_ptr _tmp78B=({const char*_tmp42F="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp42F,sizeof(char),22U);});Cyc_fprintf(_tmp78C,_tmp78B,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});
 exit(1);}}}
# 2246
if(Cyc_mode == Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2249
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2257
return 0;};};};};}
