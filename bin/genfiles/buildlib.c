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
# 165 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 275 "core.h"
void Cyc_Core_rethrow(void*);
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
void exit(int);
# 323
int mkdir(const char*pathname,unsigned short mode);
# 326
int close(int);
int chdir(const char*);
struct _dyneither_ptr Cyc_getcwd(struct _dyneither_ptr buf,unsigned long size);extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
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
# 50 "buffer.h"
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned int n);
# 58
struct _dyneither_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 81
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*,char);
# 92 "buffer.h"
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _dyneither_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1096 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1147
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1159
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1167
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
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0U,Cyc_Extern_sc  = 1U,Cyc_ExternC_sc  = 2U,Cyc_Static_sc  = 3U,Cyc_Auto_sc  = 4U,Cyc_Register_sc  = 5U,Cyc_Abstract_sc  = 6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple1*val;};struct _tuple14{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple14 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple15{unsigned int f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple16{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple13 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple12*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple18{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple19 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple9*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 68 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 33 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_finish();
# 194 "tcutil.h"
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
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp4DD=Cyc_stderr;struct _dyneither_ptr _tmp4DC=({const char*_tmp3="Internal error: log file is NULL\n";_tag_dyneither(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp4DD,_tmp4DC,_tag_dyneither(_tmp2,sizeof(void*),0U));});});
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
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp4DE=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);*_tmp5=_tmp4DE;});_tmp5;});}struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
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
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp454;_tmp454.tag=0U,_tmp454.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)symbol);_tmp454;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp4E0=Cyc_stderr;struct _dyneither_ptr _tmp4DF=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_dyneither(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp4E0,_tmp4DF,_tag_dyneither(_tmp8,sizeof(void*),1U));});});
return symbol;}
# 185
return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp4E3=(struct _dyneither_ptr)symbol;int _tmp4E2=(int)prefix_len;Cyc_substring(_tmp4E3,_tmp4E2,({unsigned long _tmp4E1=Cyc_strlen((struct _dyneither_ptr)symbol);_tmp4E1 - prefix_len;}));});}
# 188
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_tmpC=_tmpB;struct Cyc_Absyn_Typedefdecl*_tmp14;struct Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp12;switch(*((int*)_tmpC)){case 5U: _LL1: _tmp12=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2:
# 191
({struct _dyneither_ptr*_tmp4E5=({struct _dyneither_ptr*_tmpD=_cycalloc(sizeof(*_tmpD));({struct _dyneither_ptr _tmp4E4=Cyc_remove_user_prefix(*(*_tmp12->name).f2);*_tmpD=_tmp4E4;});_tmpD;});(*_tmp12->name).f2=_tmp4E5;});
goto _LL0;case 7U: _LL3: _tmp13=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4:
# 194
({struct _dyneither_ptr*_tmp4E7=({struct _dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _dyneither_ptr _tmp4E6=Cyc_remove_user_prefix(*(*_tmp13->name).f2);*_tmpE=_tmp4E6;});_tmpE;});(*_tmp13->name).f2=_tmp4E7;});
goto _LL0;case 8U: _LL5: _tmp14=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6:
# 197
({struct _dyneither_ptr*_tmp4E9=({struct _dyneither_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _dyneither_ptr _tmp4E8=Cyc_remove_user_prefix(*(*_tmp14->name).f2);*_tmpF=_tmp4E8;});_tmpF;});(*_tmp14->name).f2=_tmp4E9;});
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
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 214
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmp4EB=Cyc_stderr;struct _dyneither_ptr _tmp4EA=({const char*_tmp11="Error in .cys file: bad user-defined type definition\n";_tag_dyneither(_tmp11,sizeof(char),54U);});Cyc_fprintf(_tmp4EB,_tmp4EA,_tag_dyneither(_tmp10,sizeof(void*),0U));});});
 exit(1);}_LL0:;}
# 220
static struct Cyc_Absyn_Decl*Cyc_named_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*decl_name){
struct _dyneither_ptr*name;
for(0;decls != 0;decls=decls->tl){
{void*_tmp15=((struct Cyc_Absyn_Decl*)decls->hd)->r;void*_tmp16=_tmp15;struct Cyc_Absyn_Typedefdecl*_tmp22;struct Cyc_Absyn_Enumdecl*_tmp21;struct Cyc_Absyn_Aggrdecl*_tmp20;switch(*((int*)_tmp16)){case 5U: _LL1: _tmp20=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL2: {
# 225
struct _tuple1*_tmp17=_tmp20->name;struct _tuple1*_tmp18=_tmp17;struct _dyneither_ptr*_tmp19;_LL24: _tmp19=_tmp18->f2;_LL25:;
name=_tmp19;
goto _LL0;}case 7U: _LL3: _tmp21=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL4: {
# 229
struct _tuple1*_tmp1A=_tmp21->name;struct _tuple1*_tmp1B=_tmp1A;struct _dyneither_ptr*_tmp1C;_LL27: _tmp1C=_tmp1B->f2;_LL28:;
name=_tmp1C;
goto _LL0;}case 8U: _LL5: _tmp22=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp16)->f1;_LL6: {
# 233
struct _tuple1*_tmp1D=_tmp22->name;struct _tuple1*_tmp1E=_tmp1D;struct _dyneither_ptr*_tmp1F;_LL2A: _tmp1F=_tmp1E->f2;_LL2B:;
name=_tmp1F;
goto _LL0;}case 0U: _LL7: _LL8:
# 237
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LLC;case 13U: _LLB: _LLC:
 goto _LLE;case 14U: _LLD: _LLE:
 goto _LL10;case 15U: _LLF: _LL10:
 goto _LL12;case 16U: _LL11: _LL12:
 goto _LL14;case 2U: _LL13: _LL14:
 goto _LL16;case 6U: _LL15: _LL16:
 goto _LL18;case 3U: _LL17: _LL18:
 goto _LL1A;case 9U: _LL19: _LL1A:
 goto _LL1C;case 10U: _LL1B: _LL1C:
 goto _LL1E;case 11U: _LL1D: _LL1E:
 goto _LL20;case 12U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
# 251
 name=0;
goto _LL0;}_LL0:;}
# 254
if((unsigned int)name  && !Cyc_strptrcmp(name,decl_name))
return(struct Cyc_Absyn_Decl*)decls->hd;}
# 257
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp4EC=({const char*_tmp23="Internal Error: bad user-def name";_tag_dyneither(_tmp23,sizeof(char),34U);});_tmp24->f1=_tmp4EC;});_tmp24;}));}struct _tuple26{void*f1;void*f2;};struct _tuple27{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};
# 264
static void Cyc_merge_rep_decl(struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl){
{struct _tuple26 _tmp25=({struct _tuple26 _tmp47F;_tmp47F.f1=c_decl->r,_tmp47F.f2=user_decl->r;_tmp47F;});struct _tuple26 _tmp26=_tmp25;struct Cyc_Absyn_Typedefdecl*_tmp9E;struct Cyc_Absyn_Typedefdecl*_tmp9D;struct Cyc_Absyn_Enumdecl*_tmp9C;struct Cyc_Absyn_Enumdecl*_tmp9B;struct Cyc_Absyn_Aggrdecl*_tmp9A;struct Cyc_Absyn_Aggrdecl*_tmp99;switch(*((int*)_tmp26.f1)){case 5U: if(((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26.f2)->tag == 5U){_LL1: _tmp9A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26.f1)->f1;_tmp99=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26.f2)->f1;_LL2:
# 270
 if(_tmp99->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp29=({struct Cyc_String_pa_PrintArg_struct _tmp456;_tmp456.tag=0U,({struct _dyneither_ptr _tmp4ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp456.f1=_tmp4ED;});_tmp456;});struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp455;_tmp455.tag=0U,({struct _dyneither_ptr _tmp4EE=(struct _dyneither_ptr)({const char*_tmp2B=": universal quantification in user definition not allowed";_tag_dyneither(_tmp2B,sizeof(char),58U);});_tmp455.f1=_tmp4EE;});_tmp455;});void*_tmp27[2U];_tmp27[0]=& _tmp29,_tmp27[1]=& _tmp2A;({struct Cyc___cycFILE*_tmp4F0=Cyc_stderr;struct _dyneither_ptr _tmp4EF=({const char*_tmp28="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp28,sizeof(char),62U);});Cyc_fprintf(_tmp4F0,_tmp4EF,_tag_dyneither(_tmp27,sizeof(void*),2U));});});return;}
if((int)_tmp99->sc != (int)_tmp9A->sc)
({struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp458;_tmp458.tag=0U,({struct _dyneither_ptr _tmp4F1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp458.f1=_tmp4F1;});_tmp458;});struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp457;_tmp457.tag=0U,({struct _dyneither_ptr _tmp4F2=(struct _dyneither_ptr)({const char*_tmp30=": scopes don't match";_tag_dyneither(_tmp30,sizeof(char),21U);});_tmp457.f1=_tmp4F2;});_tmp457;});void*_tmp2C[2U];_tmp2C[0]=& _tmp2E,_tmp2C[1]=& _tmp2F;({struct Cyc___cycFILE*_tmp4F4=Cyc_stderr;struct _dyneither_ptr _tmp4F3=({const char*_tmp2D="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp2D,sizeof(char),71U);});Cyc_fprintf(_tmp4F4,_tmp4F3,_tag_dyneither(_tmp2C,sizeof(void*),2U));});});
# 275
if(_tmp99->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp45A;_tmp45A.tag=0U,({struct _dyneither_ptr _tmp4F5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp45A.f1=_tmp4F5;});_tmp45A;});struct Cyc_String_pa_PrintArg_struct _tmp34=({struct Cyc_String_pa_PrintArg_struct _tmp459;_tmp459.tag=0U,({struct _dyneither_ptr _tmp4F6=(struct _dyneither_ptr)({const char*_tmp35=": no user definition";_tag_dyneither(_tmp35,sizeof(char),21U);});_tmp459.f1=_tmp4F6;});_tmp459;});void*_tmp31[2U];_tmp31[0]=& _tmp33,_tmp31[1]=& _tmp34;({struct Cyc___cycFILE*_tmp4F8=Cyc_stderr;struct _dyneither_ptr _tmp4F7=({const char*_tmp32="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp32,sizeof(char),62U);});Cyc_fprintf(_tmp4F8,_tmp4F7,_tag_dyneither(_tmp31,sizeof(void*),2U));});});return;}
if(_tmp9A->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp38=({struct Cyc_String_pa_PrintArg_struct _tmp45C;_tmp45C.tag=0U,({struct _dyneither_ptr _tmp4F9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp45C.f1=_tmp4F9;});_tmp45C;});struct Cyc_String_pa_PrintArg_struct _tmp39=({struct Cyc_String_pa_PrintArg_struct _tmp45B;_tmp45B.tag=0U,({struct _dyneither_ptr _tmp4FA=(struct _dyneither_ptr)({const char*_tmp3A=": no definition for system version";_tag_dyneither(_tmp3A,sizeof(char),35U);});_tmp45B.f1=_tmp4FA;});_tmp45B;});void*_tmp36[2U];_tmp36[0]=& _tmp38,_tmp36[1]=& _tmp39;({struct Cyc___cycFILE*_tmp4FC=Cyc_stderr;struct _dyneither_ptr _tmp4FB=({const char*_tmp37="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp37,sizeof(char),71U);});Cyc_fprintf(_tmp4FC,_tmp4FB,_tag_dyneither(_tmp36,sizeof(void*),2U));});});
c_decl->r=user_decl->r;
return;}
# 282
{struct _tuple27 _tmp3B=({struct _tuple27 _tmp471;_tmp471.f1=_tmp9A->impl,_tmp471.f2=_tmp99->impl;_tmp471;});struct _tuple27 _tmp3C=_tmp3B;struct Cyc_List_List*_tmp76;struct Cyc_List_List*_tmp75;struct Cyc_List_List*_tmp74;struct Cyc_List_List*_tmp73;int _tmp72;if(_tmp3C.f1 != 0){if(_tmp3C.f2 != 0){_LLA: _tmp76=(_tmp3C.f1)->fields;_tmp75=(_tmp3C.f2)->exist_vars;_tmp74=(_tmp3C.f2)->rgn_po;_tmp73=(_tmp3C.f2)->fields;_tmp72=(_tmp3C.f2)->tagged;_LLB:
# 284
 if(_tmp72){
({struct Cyc_String_pa_PrintArg_struct _tmp3F=({struct Cyc_String_pa_PrintArg_struct _tmp45E;_tmp45E.tag=0U,({struct _dyneither_ptr _tmp4FD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp45E.f1=_tmp4FD;});_tmp45E;});struct Cyc_String_pa_PrintArg_struct _tmp40=({struct Cyc_String_pa_PrintArg_struct _tmp45D;_tmp45D.tag=0U,({struct _dyneither_ptr _tmp4FE=(struct _dyneither_ptr)({const char*_tmp41=": user @tagged annotation not allowed";_tag_dyneither(_tmp41,sizeof(char),38U);});_tmp45D.f1=_tmp4FE;});_tmp45D;});void*_tmp3D[2U];_tmp3D[0]=& _tmp3F,_tmp3D[1]=& _tmp40;({struct Cyc___cycFILE*_tmp500=Cyc_stderr;struct _dyneither_ptr _tmp4FF=({const char*_tmp3E="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp3E,sizeof(char),62U);});Cyc_fprintf(_tmp500,_tmp4FF,_tag_dyneither(_tmp3D,sizeof(void*),2U));});});return;}
if((unsigned int)_tmp75){
({struct Cyc_String_pa_PrintArg_struct _tmp44=({struct Cyc_String_pa_PrintArg_struct _tmp460;_tmp460.tag=0U,({struct _dyneither_ptr _tmp501=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp460.f1=_tmp501;});_tmp460;});struct Cyc_String_pa_PrintArg_struct _tmp45=({struct Cyc_String_pa_PrintArg_struct _tmp45F;_tmp45F.tag=0U,({struct _dyneither_ptr _tmp502=(struct _dyneither_ptr)({const char*_tmp46=": existential quantification in user definition not allowed";_tag_dyneither(_tmp46,sizeof(char),60U);});_tmp45F.f1=_tmp502;});_tmp45F;});void*_tmp42[2U];_tmp42[0]=& _tmp44,_tmp42[1]=& _tmp45;({struct Cyc___cycFILE*_tmp504=Cyc_stderr;struct _dyneither_ptr _tmp503=({const char*_tmp43="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp43,sizeof(char),62U);});Cyc_fprintf(_tmp504,_tmp503,_tag_dyneither(_tmp42,sizeof(void*),2U));});});return;}
if((unsigned int)_tmp74){
({struct Cyc_String_pa_PrintArg_struct _tmp49=({struct Cyc_String_pa_PrintArg_struct _tmp462;_tmp462.tag=0U,({struct _dyneither_ptr _tmp505=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp462.f1=_tmp505;});_tmp462;});struct Cyc_String_pa_PrintArg_struct _tmp4A=({struct Cyc_String_pa_PrintArg_struct _tmp461;_tmp461.tag=0U,({struct _dyneither_ptr _tmp506=(struct _dyneither_ptr)({const char*_tmp4B=": region partial order in user definition not allowed";_tag_dyneither(_tmp4B,sizeof(char),54U);});_tmp461.f1=_tmp506;});_tmp461;});void*_tmp47[2U];_tmp47[0]=& _tmp49,_tmp47[1]=& _tmp4A;({struct Cyc___cycFILE*_tmp508=Cyc_stderr;struct _dyneither_ptr _tmp507=({const char*_tmp48="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp48,sizeof(char),62U);});Cyc_fprintf(_tmp508,_tmp507,_tag_dyneither(_tmp47,sizeof(void*),2U));});});return;}{
struct Cyc_List_List*_tmp4C=_tmp76;
# 292
while(_tmp4C != 0){
struct Cyc_Absyn_Aggrfield*_tmp4D=(struct Cyc_Absyn_Aggrfield*)_tmp4C->hd;
struct Cyc_Absyn_Aggrfield*_tmp4E=Cyc_Absyn_lookup_field(_tmp73,_tmp4D->name);
# 296
if(_tmp4E != 0){
# 299
void*_tmp4F=Cyc_Toc_typ_to_c(_tmp4D->type);
void*_tmp50=Cyc_Toc_typ_to_c(_tmp4E->type);
if(!Cyc_Tcutil_unify(_tmp4F,_tmp50))
({struct Cyc_String_pa_PrintArg_struct _tmp53=({struct Cyc_String_pa_PrintArg_struct _tmp467;_tmp467.tag=0U,({struct _dyneither_ptr _tmp509=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp467.f1=_tmp509;});_tmp467;});struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp463;_tmp463.tag=0U,({struct _dyneither_ptr _tmp50D=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp57=({struct Cyc_String_pa_PrintArg_struct _tmp466;_tmp466.tag=0U,({struct _dyneither_ptr _tmp50A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp466.f1=_tmp50A;});_tmp466;});struct Cyc_String_pa_PrintArg_struct _tmp58=({struct Cyc_String_pa_PrintArg_struct _tmp465;_tmp465.tag=0U,_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp4D->name);_tmp465;});struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp464;_tmp464.tag=0U,({struct _dyneither_ptr _tmp50B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4D->type));_tmp464.f1=_tmp50B;});_tmp464;});void*_tmp55[3U];_tmp55[0]=& _tmp57,_tmp55[1]=& _tmp58,_tmp55[2]=& _tmp59;({struct _dyneither_ptr _tmp50C=({const char*_tmp56=": type %s of user definition of field %s != %s";_tag_dyneither(_tmp56,sizeof(char),47U);});Cyc_aprintf(_tmp50C,_tag_dyneither(_tmp55,sizeof(void*),3U));});}));_tmp463.f1=_tmp50D;});_tmp463;});void*_tmp51[2U];_tmp51[0]=& _tmp53,_tmp51[1]=& _tmp54;({struct Cyc___cycFILE*_tmp50F=Cyc_stderr;struct _dyneither_ptr _tmp50E=({const char*_tmp52="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp52,sizeof(char),71U);});Cyc_fprintf(_tmp50F,_tmp50E,_tag_dyneither(_tmp51,sizeof(void*),2U));});});else{
# 309
if(_tmp4E->width != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp46A;_tmp46A.tag=0U,({struct _dyneither_ptr _tmp510=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp46A.f1=_tmp510;});_tmp46A;});struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp468;_tmp468.tag=0U,({struct _dyneither_ptr _tmp513=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp60=({struct Cyc_String_pa_PrintArg_struct _tmp469;_tmp469.tag=0U,({struct _dyneither_ptr _tmp511=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp469.f1=_tmp511;});_tmp469;});void*_tmp5E[1U];_tmp5E[0]=& _tmp60;({struct _dyneither_ptr _tmp512=({const char*_tmp5F=": ignoring width of user definition of field %s";_tag_dyneither(_tmp5F,sizeof(char),48U);});Cyc_aprintf(_tmp512,_tag_dyneither(_tmp5E,sizeof(void*),1U));});}));_tmp468.f1=_tmp513;});_tmp468;});void*_tmp5A[2U];_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D;({struct Cyc___cycFILE*_tmp515=Cyc_stderr;struct _dyneither_ptr _tmp514=({const char*_tmp5B="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp5B,sizeof(char),71U);});Cyc_fprintf(_tmp515,_tmp514,_tag_dyneither(_tmp5A,sizeof(void*),2U));});});
# 313
if(_tmp4E->attributes != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp46D;_tmp46D.tag=0U,({struct _dyneither_ptr _tmp516=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp46D.f1=_tmp516;});_tmp46D;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp46B;_tmp46B.tag=0U,({struct _dyneither_ptr _tmp519=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp46C;_tmp46C.tag=0U,({struct _dyneither_ptr _tmp517=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp4E->type));_tmp46C.f1=_tmp517;});_tmp46C;});void*_tmp65[1U];_tmp65[0]=& _tmp67;({struct _dyneither_ptr _tmp518=({const char*_tmp66=": ignoring attributes of user definition of field %s";_tag_dyneither(_tmp66,sizeof(char),53U);});Cyc_aprintf(_tmp518,_tag_dyneither(_tmp65,sizeof(void*),1U));});}));_tmp46B.f1=_tmp519;});_tmp46B;});void*_tmp61[2U];_tmp61[0]=& _tmp63,_tmp61[1]=& _tmp64;({struct Cyc___cycFILE*_tmp51B=Cyc_stderr;struct _dyneither_ptr _tmp51A=({const char*_tmp62="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp62,sizeof(char),71U);});Cyc_fprintf(_tmp51B,_tmp51A,_tag_dyneither(_tmp61,sizeof(void*),2U));});});
# 319
_tmp4D->type=_tmp4E->type;
_tmp4D->tq=_tmp4E->tq;
_tmp4D->requires_clause=_tmp4E->requires_clause;}}
# 324
_tmp4C=_tmp4C->tl;}
# 327
_tmp4C=_tmp73;{
int missing_fields=0;
while(_tmp4C != 0){
struct Cyc_Absyn_Aggrfield*_tmp68=Cyc_Absyn_lookup_field(_tmp76,((struct Cyc_Absyn_Aggrfield*)_tmp4C->hd)->name);
if(_tmp68 == 0){
missing_fields=1;
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp470;_tmp470.tag=0U,({struct _dyneither_ptr _tmp51C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9A->name));_tmp470.f1=_tmp51C;});_tmp470;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp46E;_tmp46E.tag=0U,({struct _dyneither_ptr _tmp51E=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp46F;_tmp46F.tag=0U,_tmp46F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp4C->hd)->name);_tmp46F;});void*_tmp6D[1U];_tmp6D[0]=& _tmp6F;({struct _dyneither_ptr _tmp51D=({const char*_tmp6E=": no definition of field %s in system version";_tag_dyneither(_tmp6E,sizeof(char),46U);});Cyc_aprintf(_tmp51D,_tag_dyneither(_tmp6D,sizeof(void*),1U));});}));_tmp46E.f1=_tmp51E;});_tmp46E;});void*_tmp69[2U];_tmp69[0]=& _tmp6B,_tmp69[1]=& _tmp6C;({struct Cyc___cycFILE*_tmp520=Cyc_stderr;struct _dyneither_ptr _tmp51F=({const char*_tmp6A="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp6A,sizeof(char),71U);});Cyc_fprintf(_tmp520,_tmp51F,_tag_dyneither(_tmp69,sizeof(void*),2U));});});}
# 336
_tmp4C=_tmp4C->tl;}
# 338
goto _LL9;};};}else{goto _LLC;}}else{_LLC: _LLD:
# 340
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp521=({const char*_tmp70="Internal Error: NULL cases not possible";_tag_dyneither(_tmp70,sizeof(char),40U);});_tmp71->f1=_tmp521;});_tmp71;}));}_LL9:;}
# 342
return;}else{goto _LL7;}case 7U: if(((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26.f2)->tag == 7U){_LL3: _tmp9C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26.f1)->f1;_tmp9B=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26.f2)->f1;_LL4:
# 344
({struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmp473;_tmp473.tag=0U,({struct _dyneither_ptr _tmp522=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9C->name));_tmp473.f1=_tmp522;});_tmp473;});struct Cyc_String_pa_PrintArg_struct _tmp7A=({struct Cyc_String_pa_PrintArg_struct _tmp472;_tmp472.tag=0U,({struct _dyneither_ptr _tmp523=(struct _dyneither_ptr)({const char*_tmp7B=": enum merging not currently supported";_tag_dyneither(_tmp7B,sizeof(char),39U);});_tmp472.f1=_tmp523;});_tmp472;});void*_tmp77[2U];_tmp77[0]=& _tmp79,_tmp77[1]=& _tmp7A;({struct Cyc___cycFILE*_tmp525=Cyc_stderr;struct _dyneither_ptr _tmp524=({const char*_tmp78="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp78,sizeof(char),62U);});Cyc_fprintf(_tmp525,_tmp524,_tag_dyneither(_tmp77,sizeof(void*),2U));});});return;}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26.f2)->tag == 8U){_LL5: _tmp9E=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26.f1)->f1;_tmp9D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26.f2)->f1;_LL6:
# 346
 if(_tmp9D->tvs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp475;_tmp475.tag=0U,({struct _dyneither_ptr _tmp526=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp475.f1=_tmp526;});_tmp475;});struct Cyc_String_pa_PrintArg_struct _tmp7F=({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0U,({struct _dyneither_ptr _tmp527=(struct _dyneither_ptr)({const char*_tmp80=": universal quantification in user definition not supported";_tag_dyneither(_tmp80,sizeof(char),60U);});_tmp474.f1=_tmp527;});_tmp474;});void*_tmp7C[2U];_tmp7C[0]=& _tmp7E,_tmp7C[1]=& _tmp7F;({struct Cyc___cycFILE*_tmp529=Cyc_stderr;struct _dyneither_ptr _tmp528=({const char*_tmp7D="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp7D,sizeof(char),62U);});Cyc_fprintf(_tmp529,_tmp528,_tag_dyneither(_tmp7C,sizeof(void*),2U));});});return;}
# 349
if(_tmp9D->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp477;_tmp477.tag=0U,({struct _dyneither_ptr _tmp52A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp477.f1=_tmp52A;});_tmp477;});struct Cyc_String_pa_PrintArg_struct _tmp84=({struct Cyc_String_pa_PrintArg_struct _tmp476;_tmp476.tag=0U,({struct _dyneither_ptr _tmp52B=(struct _dyneither_ptr)({const char*_tmp85=": no user definition";_tag_dyneither(_tmp85,sizeof(char),21U);});_tmp476.f1=_tmp52B;});_tmp476;});void*_tmp81[2U];_tmp81[0]=& _tmp83,_tmp81[1]=& _tmp84;({struct Cyc___cycFILE*_tmp52D=Cyc_stderr;struct _dyneither_ptr _tmp52C=({const char*_tmp82="User-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp82,sizeof(char),62U);});Cyc_fprintf(_tmp52D,_tmp52C,_tag_dyneither(_tmp81,sizeof(void*),2U));});});return;}
if(_tmp9E->defn == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp88=({struct Cyc_String_pa_PrintArg_struct _tmp479;_tmp479.tag=0U,({struct _dyneither_ptr _tmp52E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp479.f1=_tmp52E;});_tmp479;});struct Cyc_String_pa_PrintArg_struct _tmp89=({struct Cyc_String_pa_PrintArg_struct _tmp478;_tmp478.tag=0U,({struct _dyneither_ptr _tmp52F=(struct _dyneither_ptr)({const char*_tmp8A=": no definition for system version";_tag_dyneither(_tmp8A,sizeof(char),35U);});_tmp478.f1=_tmp52F;});_tmp478;});void*_tmp86[2U];_tmp86[0]=& _tmp88,_tmp86[1]=& _tmp89;({struct Cyc___cycFILE*_tmp531=Cyc_stderr;struct _dyneither_ptr _tmp530=({const char*_tmp87="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp87,sizeof(char),71U);});Cyc_fprintf(_tmp531,_tmp530,_tag_dyneither(_tmp86,sizeof(void*),2U));});});
c_decl->r=user_decl->r;
return;}{
# 357
void*_tmp8B=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9E->defn));
void*_tmp8C=Cyc_Toc_typ_to_c((void*)_check_null(_tmp9D->defn));
if(!Cyc_Tcutil_unify(_tmp8B,_tmp8C))
({struct Cyc_String_pa_PrintArg_struct _tmp8F=({struct Cyc_String_pa_PrintArg_struct _tmp47D;_tmp47D.tag=0U,({struct _dyneither_ptr _tmp532=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp9E->name));_tmp47D.f1=_tmp532;});_tmp47D;});struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp47A;_tmp47A.tag=0U,({struct _dyneither_ptr _tmp536=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp93=({struct Cyc_String_pa_PrintArg_struct _tmp47C;_tmp47C.tag=0U,({struct _dyneither_ptr _tmp533=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp9D->defn)));_tmp47C.f1=_tmp533;});_tmp47C;});struct Cyc_String_pa_PrintArg_struct _tmp94=({struct Cyc_String_pa_PrintArg_struct _tmp47B;_tmp47B.tag=0U,({struct _dyneither_ptr _tmp534=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp9E->defn)));_tmp47B.f1=_tmp534;});_tmp47B;});void*_tmp91[2U];_tmp91[0]=& _tmp93,_tmp91[1]=& _tmp94;({struct _dyneither_ptr _tmp535=({const char*_tmp92=": type definition %s of user definition != %s";_tag_dyneither(_tmp92,sizeof(char),46U);});Cyc_aprintf(_tmp535,_tag_dyneither(_tmp91,sizeof(void*),2U));});}));_tmp47A.f1=_tmp536;});_tmp47A;});void*_tmp8D[2U];_tmp8D[0]=& _tmp8F,_tmp8D[1]=& _tmp90;({struct Cyc___cycFILE*_tmp538=Cyc_stderr;struct _dyneither_ptr _tmp537=({const char*_tmp8E="Warning: user-defined type for %s incompatible with system version %s\n";_tag_dyneither(_tmp8E,sizeof(char),71U);});Cyc_fprintf(_tmp538,_tmp537,_tag_dyneither(_tmp8D,sizeof(void*),2U));});});else{
# 366
_tmp9E->defn=_tmp9D->defn;
_tmp9E->tq=_tmp9D->tq;}
# 369
goto _LL0;};}else{goto _LL7;}default: _LL7: _LL8:
# 371
({struct Cyc_String_pa_PrintArg_struct _tmp97=({struct Cyc_String_pa_PrintArg_struct _tmp47E;_tmp47E.tag=0U,({
# 373
struct _dyneither_ptr _tmp539=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_decllist2string(({struct Cyc_Absyn_Decl*_tmp98[1U];_tmp98[0]=user_decl;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp98,sizeof(struct Cyc_Absyn_Decl*),1U));})));_tmp47E.f1=_tmp539;});_tmp47E;});void*_tmp95[1U];_tmp95[0]=& _tmp97;({struct Cyc___cycFILE*_tmp53B=Cyc_stderr;struct _dyneither_ptr _tmp53A=({const char*_tmp96="Error in .cys file: bad user-defined type %s\n";_tag_dyneither(_tmp96,sizeof(char),46U);});Cyc_fprintf(_tmp53B,_tmp53A,_tag_dyneither(_tmp95,sizeof(void*),1U));});});
return;}_LL0:;}
# 377
c_decl->r=user_decl->r;}
# 382
const int Cyc_lex_base[431U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431U]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 389
int state;int base;int backtrk;
int c;
state=start_state;
# 393
if(state >= 0){
({int _tmp53C=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp53C;});
lbuf->lex_last_action=- 1;}else{
# 397
state=(- state)- 1;}
# 399
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,431U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 407
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 411
c=256;}}else{
# 413
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 416
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,3846U,sizeof(int),base + c))== state)
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,3846U,sizeof(int),base + c));else{
# 419
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp53D=({const char*_tmp9F="empty token";_tag_dyneither(_tmp9F,sizeof(char),12U);});_tmpA0->f1=_tmp53D;});_tmpA0;}));else{
# 425
return lbuf->lex_last_action;}}else{
# 428
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 432
struct _tuple21*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpA1=lexstate;switch(_tmpA1){case 0U: _LL1: _LL2:
# 391 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmpA2=_cycalloc(sizeof(*_tmpA2));({struct Cyc_Set_Set*_tmp53F=({struct Cyc_Set_Set*_tmp53E=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(_tmp53E,(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});*_tmpA2=_tmp53F;});_tmpA2;});}
# 396
return({struct _tuple21*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmpA3->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmpA3;});case 1U: _LL3: _LL4:
# 399 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2U: _LL5: _LL6:
# 401
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 405
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp540=({const char*_tmpA4="some action didn't return!";_tag_dyneither(_tmpA4,sizeof(char),27U);});_tmpA5->f1=_tmp540;});_tmpA5;}));}
# 407
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpA6=lexstate;switch(_tmpA6){case 0U: _LL1: _LL2:
# 405 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct _dyneither_ptr _tmp543=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp542=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp542,0,(unsigned long)(({
int _tmp541=Cyc_Lexing_lexeme_end(lexbuf);_tmp541 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 405
*_tmpA7=_tmp543;});_tmpA7;});
# 407
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct Cyc_Set_Set*_tmp544=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmpA8=_tmp544;});_tmpA8;});
Cyc_token(lexbuf);
return 0;case 1U: _LL3: _LL4:
# 413
 Cyc_current_source=({struct _dyneither_ptr*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct _dyneither_ptr _tmp547=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp546=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp546,0,(unsigned long)(({
int _tmp545=Cyc_Lexing_lexeme_end(lexbuf);_tmp545 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 413
*_tmpA9=_tmp547;});_tmpA9;});
# 415
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct Cyc_Set_Set*_tmp548=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmpAA=_tmp548;});_tmpAA;});
Cyc_args(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 421
 Cyc_current_source=({struct _dyneither_ptr*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct _dyneither_ptr _tmp549=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpAB=_tmp549;});_tmpAB;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmpAC=_cycalloc(sizeof(*_tmpAC));({struct Cyc_Set_Set*_tmp54A=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmpAC=_tmp54A;});_tmpAC;});
Cyc_token(lexbuf);
return 0;default: _LL7: _LL8:
# 427
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 430
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp54B=({const char*_tmpAD="some action didn't return!";_tag_dyneither(_tmpAD,sizeof(char),27U);});_tmpAE->f1=_tmp54B;});_tmpAE;}));}
# 432
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpAF=lexstate;switch(_tmpAF){case 0U: _LL1: _LL2: {
# 430 "buildlib.cyl"
struct _dyneither_ptr*_tmpB0=({struct _dyneither_ptr*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct _dyneither_ptr _tmp54E=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp54D=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp54D,0,(unsigned long)(({
int _tmp54C=Cyc_Lexing_lexeme_end(lexbuf);_tmp54C - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 430
*_tmpB2=_tmp54E;});_tmpB2;});
# 432
Cyc_current_args=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->hd=_tmpB0,_tmpB1->tl=Cyc_current_args;_tmpB1;});
return Cyc_args(lexbuf);}case 1U: _LL3: _LL4: {
# 436
struct _dyneither_ptr*_tmpB3=({struct _dyneither_ptr*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _dyneither_ptr _tmp551=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp550=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp550,0,(unsigned long)(({
int _tmp54F=Cyc_Lexing_lexeme_end(lexbuf);_tmp54F - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 436
*_tmpB5=_tmp551;});_tmpB5;});
# 438
Cyc_current_args=({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->hd=_tmpB3,_tmpB4->tl=Cyc_current_args;_tmpB4;});
return Cyc_args(lexbuf);}case 2U: _LL5: _LL6: {
# 442
struct _dyneither_ptr*_tmpB6=({struct _dyneither_ptr*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct _dyneither_ptr _tmp554=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp553=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp553,0,(unsigned long)(({
int _tmp552=Cyc_Lexing_lexeme_end(lexbuf);_tmp552 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 442
*_tmpB8=_tmp554;});_tmpB8;});
# 444
Cyc_current_args=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->hd=_tmpB6,_tmpB7->tl=Cyc_current_args;_tmpB7;});
return Cyc_token(lexbuf);}default: _LL7: _LL8:
# 447
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL0:;}
# 450
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp555=({const char*_tmpB9="some action didn't return!";_tag_dyneither(_tmpB9,sizeof(char),27U);});_tmpBA->f1=_tmp555;});_tmpBA;}));}
# 452
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpBB=lexstate;switch(_tmpBB){case 0U: _LL1: _LL2:
# 451 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _dyneither_ptr _tmp556=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpBC=_tmp556;});_tmpBC;}));return Cyc_token(lexbuf);case 1U: _LL3: _LL4:
# 454 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 457 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3U: _LL7: _LL8:
# 460 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4U: _LL9: _LLA:
# 463 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5U: _LLB: _LLC:
# 465
 return Cyc_token(lexbuf);case 6U: _LLD: _LLE:
# 467
 return Cyc_token(lexbuf);case 7U: _LLF: _LL10:
# 469
 return Cyc_token(lexbuf);case 8U: _LL11: _LL12:
# 472 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9U: _LL13: _LL14:
# 475 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10U: _LL15: _LL16:
# 478 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11U: _LL17: _LL18:
# 480
 return Cyc_token(lexbuf);case 12U: _LL19: _LL1A:
# 482
 return Cyc_token(lexbuf);case 13U: _LL1B: _LL1C:
# 484
 return Cyc_token(lexbuf);case 14U: _LL1D: _LL1E:
# 486
 return Cyc_token(lexbuf);case 15U: _LL1F: _LL20:
# 488
 return Cyc_token(lexbuf);case 16U: _LL21: _LL22:
# 490
 return Cyc_token(lexbuf);case 17U: _LL23: _LL24:
# 492
 return Cyc_token(lexbuf);case 18U: _LL25: _LL26:
# 494
 return Cyc_token(lexbuf);case 19U: _LL27: _LL28:
# 496
 return Cyc_token(lexbuf);case 20U: _LL29: _LL2A:
# 498
 return Cyc_token(lexbuf);case 21U: _LL2B: _LL2C:
# 500
 return Cyc_token(lexbuf);case 22U: _LL2D: _LL2E:
# 502
 return Cyc_token(lexbuf);case 23U: _LL2F: _LL30:
# 504
 return Cyc_token(lexbuf);case 24U: _LL31: _LL32:
# 507 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25U: _LL33: _LL34:
# 509
 return Cyc_token(lexbuf);case 26U: _LL35: _LL36:
# 511
 return Cyc_token(lexbuf);case 27U: _LL37: _LL38:
# 513
 return Cyc_token(lexbuf);case 28U: _LL39: _LL3A:
# 515
 return Cyc_token(lexbuf);case 29U: _LL3B: _LL3C:
# 517
 return Cyc_token(lexbuf);case 30U: _LL3D: _LL3E:
# 519
 return Cyc_token(lexbuf);case 31U: _LL3F: _LL40:
# 521
 return Cyc_token(lexbuf);case 32U: _LL41: _LL42:
# 523
 return Cyc_token(lexbuf);case 33U: _LL43: _LL44:
# 525
 return Cyc_token(lexbuf);case 34U: _LL45: _LL46:
# 527
 return Cyc_token(lexbuf);case 35U: _LL47: _LL48:
# 529
 return Cyc_token(lexbuf);case 36U: _LL49: _LL4A:
# 531
 return Cyc_token(lexbuf);case 37U: _LL4B: _LL4C:
# 533
 return Cyc_token(lexbuf);case 38U: _LL4D: _LL4E:
# 535
 return Cyc_token(lexbuf);case 39U: _LL4F: _LL50:
# 537
 return Cyc_token(lexbuf);case 40U: _LL51: _LL52:
# 539
 return Cyc_token(lexbuf);case 41U: _LL53: _LL54:
# 541
 return Cyc_token(lexbuf);case 42U: _LL55: _LL56:
# 543
 return Cyc_token(lexbuf);case 43U: _LL57: _LL58:
# 545
 return Cyc_token(lexbuf);case 44U: _LL59: _LL5A:
# 547
 return Cyc_token(lexbuf);case 45U: _LL5B: _LL5C:
# 549
 return Cyc_token(lexbuf);case 46U: _LL5D: _LL5E:
# 551
 return Cyc_token(lexbuf);case 47U: _LL5F: _LL60:
# 553
 return Cyc_token(lexbuf);case 48U: _LL61: _LL62:
# 556 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL63: _LL64:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL0:;}
# 560
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp557=({const char*_tmpBD="some action didn't return!";_tag_dyneither(_tmpBD,sizeof(char),27U);});_tmpBE->f1=_tmp557;});_tmpBE;}));}
# 562
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpBF=lexstate;switch(_tmpBF){case 0U: _LL1: _LL2:
# 561 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1U: _LL3: _LL4:
# 562 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 563 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3U: _LL7: _LL8:
# 564 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4U: _LL9: _LLA:
# 567 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5U: _LLB: _LLC:
# 570 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6U: _LLD: _LLE:
# 572
 return Cyc_string(lexbuf);case 7U: _LLF: _LL10:
# 573 "buildlib.cyl"
 return 0;case 8U: _LL11: _LL12:
# 574 "buildlib.cyl"
 return 0;case 9U: _LL13: _LL14:
# 575 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL15: _LL16:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL0:;}
# 579
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp558=({const char*_tmpC0="some action didn't return!";_tag_dyneither(_tmpC0,sizeof(char),27U);});_tmpC1->f1=_tmp558;});_tmpC1;}));}
# 581
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC2=lexstate;switch(_tmpC2){case 0U: _LL1: _LL2:
# 584 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 586
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2U: _LL5: _LL6:
# 593 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC3=0U;({struct _dyneither_ptr _tmp559=({const char*_tmpC4="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpC4,sizeof(char),44U);});Cyc_log(_tmp559,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});
return 1;case 3U: _LL7: _LL8:
# 599 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC5=0U;({struct _dyneither_ptr _tmp55A=({const char*_tmpC6="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmpC6,sizeof(char),44U);});Cyc_log(_tmp55A,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});
return 1;case 4U: _LL9: _LLA:
# 605 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC7=0U;({struct _dyneither_ptr _tmp55B=({const char*_tmpC8="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpC8,sizeof(char),44U);});Cyc_log(_tmp55B,_tag_dyneither(_tmpC7,sizeof(void*),0U));});});
return 1;case 5U: _LLB: _LLC:
# 611 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpC9=0U;({struct _dyneither_ptr _tmp55C=({const char*_tmpCA="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmpCA,sizeof(char),44U);});Cyc_log(_tmp55C,_tag_dyneither(_tmpC9,sizeof(void*),0U));});});
return 1;case 6U: _LLD: _LLE:
# 615
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpCB=0U;({struct _dyneither_ptr _tmp55D=({const char*_tmpCC="Warning: use of region sidestepped\n";_tag_dyneither(_tmpCC,sizeof(char),36U);});Cyc_log(_tmp55D,_tag_dyneither(_tmpCB,sizeof(void*),0U));});});
return 1;case 7U: _LLF: _LL10:
# 619
({void*_tmpCD=0U;({struct _dyneither_ptr _tmp55E=({const char*_tmpCE="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmpCE,sizeof(char),39U);});Cyc_log(_tmp55E,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});
return 1;case 8U: _LL11: _LL12:
# 624 "buildlib.cyl"
({void*_tmpCF=0U;({struct _dyneither_ptr _tmp55F=({const char*_tmpD0="Warning: use of mode HI deleted\n";_tag_dyneither(_tmpD0,sizeof(char),33U);});Cyc_log(_tmp55F,_tag_dyneither(_tmpCF,sizeof(void*),0U));});});
return 1;case 9U: _LL13: _LL14:
# 627
({void*_tmpD1=0U;({struct _dyneither_ptr _tmp560=({const char*_tmpD2="Warning: use of mode SI deleted\n";_tag_dyneither(_tmpD2,sizeof(char),33U);});Cyc_log(_tmp560,_tag_dyneither(_tmpD1,sizeof(void*),0U));});});
return 1;case 10U: _LL15: _LL16:
# 630
({void*_tmpD3=0U;({struct _dyneither_ptr _tmp561=({const char*_tmpD4="Warning: use of mode QI deleted\n";_tag_dyneither(_tmpD4,sizeof(char),33U);});Cyc_log(_tmp561,_tag_dyneither(_tmpD3,sizeof(void*),0U));});});
return 1;case 11U: _LL17: _LL18:
# 633
({void*_tmpD5=0U;({struct _dyneither_ptr _tmp562=({const char*_tmpD6="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpD6,sizeof(char),33U);});Cyc_log(_tmp562,_tag_dyneither(_tmpD5,sizeof(void*),0U));});});
return 1;case 12U: _LL19: _LL1A:
# 636
({void*_tmpD7=0U;({struct _dyneither_ptr _tmp563=({const char*_tmpD8="Warning: use of mode DI deleted\n";_tag_dyneither(_tmpD8,sizeof(char),33U);});Cyc_log(_tmp563,_tag_dyneither(_tmpD7,sizeof(void*),0U));});});
return 1;case 13U: _LL1B: _LL1C:
# 639
({void*_tmpD9=0U;({struct _dyneither_ptr _tmp564=({const char*_tmpDA="Warning: use of mode word deleted\n";_tag_dyneither(_tmpDA,sizeof(char),35U);});Cyc_log(_tmp564,_tag_dyneither(_tmpD9,sizeof(void*),0U));});});
return 1;case 14U: _LL1D: _LL1E:
# 642
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15U: _LL1F: _LL20:
# 644
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16U: _LL21: _LL22:
# 646
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17U: _LL23: _LL24:
# 648
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18U: _LL25: _LL26:
# 653 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19U: _LL27: _LL28:
# 655
 return 1;case 20U: _LL29: _LL2A:
# 657
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpDB=0U;({struct _dyneither_ptr _tmp565=({const char*_tmpDC="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmpDC,sizeof(char),42U);});Cyc_log(_tmp565,_tag_dyneither(_tmpDB,sizeof(void*),0U));});});
return 1;case 21U: _LL2B: _LL2C:
# 663
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmpDD=0U;({struct _dyneither_ptr _tmp566=({const char*_tmpDE="Warning: replacing use of __asm with 0\n";_tag_dyneither(_tmpDE,sizeof(char),40U);});Cyc_log(_tmp566,_tag_dyneither(_tmpDD,sizeof(void*),0U));});});
return 1;case 22U: _LL2D: _LL2E:
# 669
({int _tmp567=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp567,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default: _LL2F: _LL30:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL0:;}
# 673
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp568=({const char*_tmpDF="some action didn't return!";_tag_dyneither(_tmpDF,sizeof(char),27U);});_tmpE0->f1=_tmp568;});_tmpE0;}));}
# 675
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpE1=lexstate;switch(_tmpE1){case 0U: _LL1: _LL2:
# 673 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 675
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2U: _LL5: _LL6:
# 677
({void*_tmpE2=0U;({struct _dyneither_ptr _tmp569=({const char*_tmpE3="Warning: unclosed string\n";_tag_dyneither(_tmpE3,sizeof(char),26U);});Cyc_log(_tmp569,_tag_dyneither(_tmpE2,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmpE6=({struct Cyc_String_pa_PrintArg_struct _tmp480;_tmp480.tag=0U,({struct _dyneither_ptr _tmp56A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp480.f1=_tmp56A;});_tmp480;});void*_tmpE4[1U];_tmpE4[0]=& _tmpE6;({struct Cyc___cycFILE*_tmp56C=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp56B=({const char*_tmpE5="%s";_tag_dyneither(_tmpE5,sizeof(char),3U);});Cyc_fprintf(_tmp56C,_tmp56B,_tag_dyneither(_tmpE4,sizeof(void*),1U));});});return 1;case 3U: _LL7: _LL8:
# 680
({struct Cyc_String_pa_PrintArg_struct _tmpE9=({struct Cyc_String_pa_PrintArg_struct _tmp481;_tmp481.tag=0U,({struct _dyneither_ptr _tmp56D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp481.f1=_tmp56D;});_tmp481;});void*_tmpE7[1U];_tmpE7[0]=& _tmpE9;({struct Cyc___cycFILE*_tmp56F=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp56E=({const char*_tmpE8="%s";_tag_dyneither(_tmpE8,sizeof(char),3U);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_dyneither(_tmpE7,sizeof(void*),1U));});});return 1;case 4U: _LL9: _LLA:
# 682
({struct Cyc_String_pa_PrintArg_struct _tmpEC=({struct Cyc_String_pa_PrintArg_struct _tmp482;_tmp482.tag=0U,({struct _dyneither_ptr _tmp570=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp482.f1=_tmp570;});_tmp482;});void*_tmpEA[1U];_tmpEA[0]=& _tmpEC;({struct Cyc___cycFILE*_tmp572=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp571=({const char*_tmpEB="%s";_tag_dyneither(_tmpEB,sizeof(char),3U);});Cyc_fprintf(_tmp572,_tmp571,_tag_dyneither(_tmpEA,sizeof(void*),1U));});});return 1;case 5U: _LLB: _LLC:
# 684
({struct Cyc_String_pa_PrintArg_struct _tmpEF=({struct Cyc_String_pa_PrintArg_struct _tmp483;_tmp483.tag=0U,({struct _dyneither_ptr _tmp573=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp483.f1=_tmp573;});_tmp483;});void*_tmpED[1U];_tmpED[0]=& _tmpEF;({struct Cyc___cycFILE*_tmp575=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp574=({const char*_tmpEE="%s";_tag_dyneither(_tmpEE,sizeof(char),3U);});Cyc_fprintf(_tmp575,_tmp574,_tag_dyneither(_tmpED,sizeof(void*),1U));});});return 1;case 6U: _LLD: _LLE:
# 686
({struct Cyc_String_pa_PrintArg_struct _tmpF2=({struct Cyc_String_pa_PrintArg_struct _tmp484;_tmp484.tag=0U,({struct _dyneither_ptr _tmp576=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp484.f1=_tmp576;});_tmp484;});void*_tmpF0[1U];_tmpF0[0]=& _tmpF2;({struct Cyc___cycFILE*_tmp578=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp577=({const char*_tmpF1="%s";_tag_dyneither(_tmpF1,sizeof(char),3U);});Cyc_fprintf(_tmp578,_tmp577,_tag_dyneither(_tmpF0,sizeof(void*),1U));});});return 1;case 7U: _LLF: _LL10:
# 688
({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp485;_tmp485.tag=0U,({struct _dyneither_ptr _tmp579=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp485.f1=_tmp579;});_tmp485;});void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({struct Cyc___cycFILE*_tmp57B=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp57A=({const char*_tmpF4="%s";_tag_dyneither(_tmpF4,sizeof(char),3U);});Cyc_fprintf(_tmp57B,_tmp57A,_tag_dyneither(_tmpF3,sizeof(void*),1U));});});return 1;case 8U: _LL11: _LL12:
# 690
({struct Cyc_String_pa_PrintArg_struct _tmpF8=({struct Cyc_String_pa_PrintArg_struct _tmp486;_tmp486.tag=0U,({struct _dyneither_ptr _tmp57C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp486.f1=_tmp57C;});_tmp486;});void*_tmpF6[1U];_tmpF6[0]=& _tmpF8;({struct Cyc___cycFILE*_tmp57E=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp57D=({const char*_tmpF7="%s";_tag_dyneither(_tmpF7,sizeof(char),3U);});Cyc_fprintf(_tmp57E,_tmp57D,_tag_dyneither(_tmpF6,sizeof(void*),1U));});});return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL0:;}
# 694
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp57F=({const char*_tmpF9="some action didn't return!";_tag_dyneither(_tmpF9,sizeof(char),27U);});_tmpFA->f1=_tmp57F;});_tmpFA;}));}
# 696
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpFB=lexstate;switch(_tmpFB){case 0U: _LL1: _LL2:
# 700 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 702
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2U: _LL5: _LL6:
# 706
 ++ Cyc_parens_to_match;
return 1;case 3U: _LL7: _LL8:
# 709
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 712
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 715
 return 1;case 6U: _LLD: _LLE:
# 717
 return 1;default: _LLF: _LL10:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL0:;}
# 721
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp580=({const char*_tmpFC="some action didn't return!";_tag_dyneither(_tmpFC,sizeof(char),27U);});_tmpFD->f1=_tmp580;});_tmpFD;}));}
# 723
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpFE=lexstate;switch(_tmpFE){case 0U: _LL1: _LL2:
# 721 "buildlib.cyl"
({void*_tmpFF=0U;({struct _dyneither_ptr _tmp581=({const char*_tmp100="Warning: unclosed string\n";_tag_dyneither(_tmp100,sizeof(char),26U);});Cyc_log(_tmp581,_tag_dyneither(_tmpFF,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 723
 return 0;case 2U: _LL5: _LL6:
# 725
({void*_tmp101=0U;({struct _dyneither_ptr _tmp582=({const char*_tmp102="Warning: unclosed string\n";_tag_dyneither(_tmp102,sizeof(char),26U);});Cyc_log(_tmp582,_tag_dyneither(_tmp101,sizeof(void*),0U));});});return 1;case 3U: _LL7: _LL8:
# 727
 return 1;case 4U: _LL9: _LLA:
# 729
 return 1;case 5U: _LLB: _LLC:
# 731
 return 1;case 6U: _LLD: _LLE:
# 733
 return 1;case 7U: _LLF: _LL10:
# 735
 return 1;case 8U: _LL11: _LL12:
# 737
 return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL0:;}
# 741
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp583=({const char*_tmp103="some action didn't return!";_tag_dyneither(_tmp103,sizeof(char),27U);});_tmp104->f1=_tmp583;});_tmp104;}));}
# 743
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp105=lexstate;switch(_tmp105){case 0U: _LL1: _LL2:
# 741 "buildlib.cyl"
({void*_tmp106=0U;({struct _dyneither_ptr _tmp584=({const char*_tmp107="Warning: unclosed comment\n";_tag_dyneither(_tmp107,sizeof(char),27U);});Cyc_log(_tmp584,_tag_dyneither(_tmp106,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 743
 return 0;case 2U: _LL5: _LL6:
# 745
 return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL0:;}
# 749
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp585=({const char*_tmp108="some action didn't return!";_tag_dyneither(_tmp108,sizeof(char),27U);});_tmp109->f1=_tmp585;});_tmp109;}));}
# 751
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple22*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp10A=lexstate;switch(_tmp10A){case 0U: _LL1: _LL2:
# 753 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp10B="#define ";_tag_dyneither(_tmp10B,sizeof(char),9U);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple22*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->f1=Cyc_current_line,_tmp10C->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp10C;});case 1U: _LL3: _LL4:
# 757
 return Cyc_suck_line(lexbuf);case 2U: _LL5: _LL6:
# 759
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL0:;}
# 763
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp586=({const char*_tmp10D="some action didn't return!";_tag_dyneither(_tmp10D,sizeof(char),27U);});_tmp10E->f1=_tmp586;});_tmp10E;}));}
# 765
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp10F=lexstate;if(_tmp10F == 0){_LL1: _LL2:
# 763 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmp110=_cycalloc(sizeof(*_tmp110));({struct _dyneither_ptr _tmp587=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp110=_tmp587;});_tmp110;});
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL3: _LL4:
# 767
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 770
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp588=({const char*_tmp111="some action didn't return!";_tag_dyneither(_tmp111,sizeof(char),27U);});_tmp112->f1=_tmp588;});_tmp112;}));}
# 772
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp113=lexstate;if(_tmp113 == 0){_LL1: _LL2:
# 770 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp589=(struct _dyneither_ptr)Cyc_current_line;Cyc_strconcat(_tmp589,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});return 0;}else{_LL3: _LL4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL0:;}
# 774
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp58A=({const char*_tmp114="some action didn't return!";_tag_dyneither(_tmp114,sizeof(char),27U);});_tmp115->f1=_tmp58A;});_tmp115;}));}
# 776
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple25*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp116=lexstate;switch(_tmp116){case 0U: _LL1: _LL2:
# 777 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1U: _LL3: _LL4:
# 779
 Cyc_current_headerfile=(struct _dyneither_ptr)({
struct _dyneither_ptr _tmp58C=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp58C,0,(unsigned long)(({
int _tmp58B=Cyc_Lexing_lexeme_end(lexbuf);_tmp58B - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
Cyc_current_symbols=0;
Cyc_current_user_defs=0;
Cyc_current_omit_symbols=0;
Cyc_current_cstubs=0;
Cyc_current_cycstubs=0;
Cyc_current_hstubs=0;
Cyc_current_cpp=0;
while(Cyc_commands(lexbuf)){;}
Cyc_current_hstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_hstubs);
Cyc_current_cstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cstubs);
Cyc_current_cycstubs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cycstubs);
Cyc_current_cpp=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_current_cpp);
return({struct _tuple25*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->f1=Cyc_current_headerfile,_tmp117->f2=Cyc_current_symbols,_tmp117->f3=Cyc_current_user_defs,_tmp117->f4=Cyc_current_omit_symbols,_tmp117->f5=Cyc_current_hstubs,_tmp117->f6=Cyc_current_cstubs,_tmp117->f7=Cyc_current_cycstubs,_tmp117->f8=Cyc_current_cpp;_tmp117;});case 2U: _LL5: _LL6:
# 804
 return Cyc_spec(lexbuf);case 3U: _LL7: _LL8:
# 806
 return 0;case 4U: _LL9: _LLA:
# 808
({struct Cyc_Int_pa_PrintArg_struct _tmp11A=({struct Cyc_Int_pa_PrintArg_struct _tmp487;_tmp487.tag=1U,({
# 810
unsigned long _tmp58D=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp487.f1=_tmp58D;});_tmp487;});void*_tmp118[1U];_tmp118[0]=& _tmp11A;({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _dyneither_ptr _tmp58E=({const char*_tmp119="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp119,sizeof(char),67U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_dyneither(_tmp118,sizeof(void*),1U));});});
return 0;default: _LLB: _LLC:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL0:;}
# 815
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp590=({const char*_tmp11B="some action didn't return!";_tag_dyneither(_tmp11B,sizeof(char),27U);});_tmp11C->f1=_tmp590;});_tmp11C;}));}
# 817
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp11D=lexstate;switch(_tmp11D){case 0U: _LL1: _LL2:
# 815 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 817
 return 0;case 2U: _LL5: _LL6:
# 819
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3U: _LL7: _LL8:
# 824
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp11E=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){;}
if(_tmp11E != Cyc_current_user_defs){
({void*_tmp11F=0U;({struct Cyc___cycFILE*_tmp592=Cyc_stderr;struct _dyneither_ptr _tmp591=({const char*_tmp120="Error in .cys file: got optional definition in omitsymbols\n";_tag_dyneither(_tmp120,sizeof(char),60U);});Cyc_fprintf(_tmp592,_tmp591,_tag_dyneither(_tmp11F,sizeof(void*),0U));});});
# 830
return 0;}
# 832
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;};case 4U: _LL9: _LLA:
# 835
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp122=_cycalloc(sizeof(*_tmp122));({struct _dyneither_ptr _tmp594=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp122->f1=_tmp594;}),({
struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp122->f2=_tmp593;});_tmp122;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->hd=x,_tmp121->tl=Cyc_current_hstubs;_tmp121;});
return 1;};case 5U: _LLB: _LLC: {
# 843
struct _dyneither_ptr _tmp123=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp123,sizeof(char),(int)Cyc_strlen(({const char*_tmp124="hstub";_tag_dyneither(_tmp124,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp123,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp123,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp123;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp125=Cyc_substring((struct _dyneither_ptr)_tmp123,0,(unsigned long)((t.curr - _tmp123.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->f1=(struct _dyneither_ptr)_tmp125,({
struct _dyneither_ptr _tmp595=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp127->f2=_tmp595;});_tmp127;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->hd=x,_tmp126->tl=Cyc_current_hstubs;_tmp126;});
return 1;};};};}case 6U: _LLD: _LLE:
# 857
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp129=_cycalloc(sizeof(*_tmp129));({struct _dyneither_ptr _tmp597=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp129->f1=_tmp597;}),({
struct _dyneither_ptr _tmp596=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp129->f2=_tmp596;});_tmp129;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->hd=x,_tmp128->tl=Cyc_current_cstubs;_tmp128;});
return 1;};case 7U: _LLF: _LL10: {
# 865
struct _dyneither_ptr _tmp12A=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp12A,sizeof(char),(int)Cyc_strlen(({const char*_tmp12B="cstub";_tag_dyneither(_tmp12B,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp12A,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp12A,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp12A;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp12C=Cyc_substring((struct _dyneither_ptr)_tmp12A,0,(unsigned long)((t.curr - _tmp12A.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->f1=(struct _dyneither_ptr)_tmp12C,({
struct _dyneither_ptr _tmp598=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp12E->f2=_tmp598;});_tmp12E;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->hd=x,_tmp12D->tl=Cyc_current_cstubs;_tmp12D;});
return 1;};};};}case 8U: _LL11: _LL12:
# 879
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp130=_cycalloc(sizeof(*_tmp130));({struct _dyneither_ptr _tmp59A=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp130->f1=_tmp59A;}),({
struct _dyneither_ptr _tmp599=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp130->f2=_tmp599;});_tmp130;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->hd=x,_tmp12F->tl=Cyc_current_cycstubs;_tmp12F;});
return 1;};case 9U: _LL13: _LL14: {
# 887
struct _dyneither_ptr _tmp131=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp131,sizeof(char),(int)Cyc_strlen(({const char*_tmp132="cycstub";_tag_dyneither(_tmp132,sizeof(char),8U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp131,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp131,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp131;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp133=Cyc_substring((struct _dyneither_ptr)_tmp131,0,(unsigned long)((t.curr - _tmp131.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->f1=(struct _dyneither_ptr)_tmp133,({
struct _dyneither_ptr _tmp59B=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp135->f2=_tmp59B;});_tmp135;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->hd=x,_tmp134->tl=Cyc_current_cycstubs;_tmp134;});
return 1;};};};}case 10U: _LL15: _LL16:
# 901
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmp137=_cycalloc(sizeof(*_tmp137));({struct _dyneither_ptr _tmp59C=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmp137=_tmp59C;});_tmp137;});
Cyc_current_cpp=({struct Cyc_List_List*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->hd=x,_tmp136->tl=Cyc_current_cpp;_tmp136;});
return 1;};case 11U: _LL17: _LL18:
# 908
 return 1;case 12U: _LL19: _LL1A:
# 910
 return 1;case 13U: _LL1B: _LL1C:
# 912
({struct Cyc_Int_pa_PrintArg_struct _tmp13A=({struct Cyc_Int_pa_PrintArg_struct _tmp488;_tmp488.tag=1U,({
# 914
unsigned long _tmp59D=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp488.f1=_tmp59D;});_tmp488;});void*_tmp138[1U];_tmp138[0]=& _tmp13A;({struct Cyc___cycFILE*_tmp59F=Cyc_stderr;struct _dyneither_ptr _tmp59E=({const char*_tmp139="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmp139,sizeof(char),58U);});Cyc_fprintf(_tmp59F,_tmp59E,_tag_dyneither(_tmp138,sizeof(void*),1U));});});
return 0;default: _LL1D: _LL1E:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL0:;}
# 919
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5A0=({const char*_tmp13B="some action didn't return!";_tag_dyneither(_tmp13B,sizeof(char),27U);});_tmp13C->f1=_tmp5A0;});_tmp13C;}));}
# 921
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp13D=lexstate;switch(_tmp13D){case 0U: _LL1: _LL2:
# 925 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmp13F=_cycalloc(sizeof(*_tmp13F));({struct _dyneither_ptr*_tmp5A2=({struct _dyneither_ptr*_tmp13E=_cycalloc(sizeof(*_tmp13E));({struct _dyneither_ptr _tmp5A1=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp13E=_tmp5A1;});_tmp13E;});_tmp13F->hd=_tmp5A2;}),_tmp13F->tl=Cyc_snarfed_symbols;_tmp13F;});
return 1;case 1U: _LL3: _LL4: {
# 928
struct _dyneither_ptr _tmp140=Cyc_Lexing_lexeme(lexbuf);
struct _dyneither_ptr t=_tmp140;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmp140,0,(unsigned long)((t.curr - _tmp140.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}
# 937
Cyc_rename_current_symbol=0;{
struct _tuple24*user_def=({struct _tuple24*_tmp146=_cycalloc(sizeof(*_tmp146));({struct _dyneither_ptr*_tmp5A5=({struct _dyneither_ptr*_tmp144=_cycalloc(sizeof(*_tmp144));*_tmp144=Cyc_current_symbol;_tmp144;});_tmp146->f1=_tmp5A5;}),({
struct _dyneither_ptr*_tmp5A4=({struct _dyneither_ptr*_tmp145=_cycalloc(sizeof(*_tmp145));({struct _dyneither_ptr _tmp5A3=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmp145=_tmp5A3;});_tmp145;});_tmp146->f2=_tmp5A4;});_tmp146;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_tmp142=_cycalloc(sizeof(*_tmp142));({struct _dyneither_ptr*_tmp5A6=({struct _dyneither_ptr*_tmp141=_cycalloc(sizeof(*_tmp141));*_tmp141=(struct _dyneither_ptr)Cyc_current_symbol;_tmp141;});_tmp142->hd=_tmp5A6;}),_tmp142->tl=Cyc_snarfed_symbols;_tmp142;});
Cyc_current_user_defs=({struct Cyc_List_List*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->hd=user_def,_tmp143->tl=Cyc_current_user_defs;_tmp143;});
return 1;};}case 2U: _LL5: _LL6:
# 944
 return 1;case 3U: _LL7: _LL8:
# 946
 return 0;case 4U: _LL9: _LLA:
# 948
({void*_tmp147=0U;({struct Cyc___cycFILE*_tmp5A8=Cyc_stderr;struct _dyneither_ptr _tmp5A7=({const char*_tmp148="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmp148,sizeof(char),44U);});Cyc_fprintf(_tmp5A8,_tmp5A7,_tag_dyneither(_tmp147,sizeof(void*),0U));});});
# 950
return 0;case 5U: _LLB: _LLC:
# 952
({struct Cyc_Int_pa_PrintArg_struct _tmp14B=({struct Cyc_Int_pa_PrintArg_struct _tmp489;_tmp489.tag=1U,({
# 954
unsigned long _tmp5A9=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp489.f1=_tmp5A9;});_tmp489;});void*_tmp149[1U];_tmp149[0]=& _tmp14B;({struct Cyc___cycFILE*_tmp5AB=Cyc_stderr;struct _dyneither_ptr _tmp5AA=({const char*_tmp14A="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmp14A,sizeof(char),57U);});Cyc_fprintf(_tmp5AB,_tmp5AA,_tag_dyneither(_tmp149,sizeof(void*),1U));});});
return 0;default: _LLD: _LLE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL0:;}
# 959
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5AC=({const char*_tmp14C="some action didn't return!";_tag_dyneither(_tmp14C,sizeof(char),27U);});_tmp14D->f1=_tmp5AC;});_tmp14D;}));}
# 961
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp14E=lexstate;switch(_tmp14E){case 0U: _LL1: _LL2:
# 965 "buildlib.cyl"
({void*_tmp14F=0U;({struct _dyneither_ptr _tmp5AD=({const char*_tmp150="Warning: unclosed brace\n";_tag_dyneither(_tmp150,sizeof(char),25U);});Cyc_log(_tmp5AD,_tag_dyneither(_tmp14F,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 967
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2U: _LL5: _LL6:
# 972
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3U: _LL7: _LL8:
# 976
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 980
({struct Cyc_Buffer_t*_tmp5AE=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5AE,({const char*_tmp151="/*";_tag_dyneither(_tmp151,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 984
({struct Cyc_Buffer_t*_tmp5AF=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5AF,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE: {
# 987
struct _dyneither_ptr _tmp152=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol  && !Cyc_strcmp((struct _dyneither_ptr)_tmp152,(struct _dyneither_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp5B0=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B0,*Cyc_add_user_prefix(({struct _dyneither_ptr*_tmp153=_cycalloc(sizeof(*_tmp153));*_tmp153=_tmp152;_tmp153;})));});else{
# 991
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tmp152);}
return 1;}case 7U: _LLF: _LL10:
# 994
({struct Cyc_Buffer_t*_tmp5B1=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp5B1,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default: _LL11: _LL12:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL0:;}
# 999
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5B2=({const char*_tmp154="some action didn't return!";_tag_dyneither(_tmp154,sizeof(char),27U);});_tmp155->f1=_tmp5B2;});_tmp155;}));}
# 1001
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp156=lexstate;switch(_tmp156){case 0U: _LL1: _LL2:
# 999 "buildlib.cyl"
({void*_tmp157=0U;({struct _dyneither_ptr _tmp5B3=({const char*_tmp158="Warning: unclosed string\n";_tag_dyneither(_tmp158,sizeof(char),26U);});Cyc_log(_tmp5B3,_tag_dyneither(_tmp157,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 1001
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2U: _LL5: _LL6:
# 1003
({void*_tmp159=0U;({struct _dyneither_ptr _tmp5B4=({const char*_tmp15A="Warning: unclosed string\n";_tag_dyneither(_tmp15A,sizeof(char),26U);});Cyc_log(_tmp5B4,_tag_dyneither(_tmp159,sizeof(void*),0U));});});
({struct Cyc_Buffer_t*_tmp5B5=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B5,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3U: _LL7: _LL8:
# 1007
({struct Cyc_Buffer_t*_tmp5B6=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B6,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4U: _LL9: _LLA:
# 1010
({struct Cyc_Buffer_t*_tmp5B7=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B7,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5U: _LLB: _LLC:
# 1013
({struct Cyc_Buffer_t*_tmp5B8=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B8,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE:
# 1016
({struct Cyc_Buffer_t*_tmp5B9=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5B9,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7U: _LLF: _LL10:
# 1019
({struct Cyc_Buffer_t*_tmp5BA=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5BA,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8U: _LL11: _LL12:
# 1022
({struct Cyc_Buffer_t*_tmp5BB=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5BB,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL0:;}
# 1027
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5BC=({const char*_tmp15B="some action didn't return!";_tag_dyneither(_tmp15B,sizeof(char),27U);});_tmp15C->f1=_tmp5BC;});_tmp15C;}));}
# 1029
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp15D=lexstate;switch(_tmp15D){case 0U: _LL1: _LL2:
# 1027 "buildlib.cyl"
({void*_tmp15E=0U;({struct _dyneither_ptr _tmp5BD=({const char*_tmp15F="Warning: unclosed comment\n";_tag_dyneither(_tmp15F,sizeof(char),27U);});Cyc_log(_tmp5BD,_tag_dyneither(_tmp15E,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 1029
({struct Cyc_Buffer_t*_tmp5BE=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5BE,({const char*_tmp160="*/";_tag_dyneither(_tmp160,sizeof(char),3U);}));});return 0;case 2U: _LL5: _LL6:
# 1031
({struct Cyc_Buffer_t*_tmp5BF=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp5BF,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL0:;}
# 1036
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp5C0=({const char*_tmp161="some action didn't return!";_tag_dyneither(_tmp161,sizeof(char),27U);});_tmp162->f1=_tmp5C0;});_tmp162;}));}
# 1038
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 1044 "buildlib.cyl"
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple28{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmp163=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmp164=_tmp163;struct Cyc_List_List*_tmp1B3;void*_tmp1B2;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Exp*_tmp1B0;struct _dyneither_ptr*_tmp1AF;struct Cyc_Absyn_Exp*_tmp1AE;struct _dyneither_ptr*_tmp1AD;void*_tmp1AC;void*_tmp1AB;struct Cyc_Absyn_Exp*_tmp1AA;int _tmp1A9;struct Cyc_Absyn_Exp*_tmp1A8;void**_tmp1A7;struct Cyc_Absyn_Exp*_tmp1A6;void*_tmp1A5;struct Cyc_Absyn_Exp*_tmp1A4;struct Cyc_Absyn_Exp*_tmp1A3;struct Cyc_List_List*_tmp1A2;struct Cyc_Absyn_Exp*_tmp1A1;struct Cyc_Absyn_Exp*_tmp1A0;struct Cyc_Absyn_Exp*_tmp19F;struct Cyc_Absyn_Exp*_tmp19E;struct Cyc_Absyn_Exp*_tmp19D;struct Cyc_Absyn_Exp*_tmp19C;struct Cyc_Absyn_Exp*_tmp19B;struct Cyc_Absyn_Exp*_tmp19A;struct Cyc_Absyn_Exp*_tmp199;struct Cyc_Absyn_Exp*_tmp198;struct Cyc_Absyn_Exp*_tmp197;struct Cyc_Absyn_Exp*_tmp196;struct Cyc_Absyn_Exp*_tmp195;struct Cyc_Absyn_Exp*_tmp194;struct Cyc_Absyn_Exp*_tmp193;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp191;struct Cyc_Absyn_Exp*_tmp190;struct Cyc_List_List*_tmp18F;void*_tmp18E;switch(*((int*)_tmp164)){case 1U: _LL1: _tmp18E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL2: {
# 1048
struct _dyneither_ptr*_tmp165=(*Cyc_Absyn_binding2qvar(_tmp18E)).f2;
Cyc_add_target(_tmp165);
return;}case 3U: _LL3: _tmp18F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL4:
# 1052
 for(0;_tmp18F != 0;_tmp18F=_tmp18F->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp18F->hd,dep);}
# 1055
return;case 23U: _LL5: _tmp191=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp190=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL6:
# 1057
 _tmp193=_tmp191;_tmp192=_tmp190;goto _LL8;case 9U: _LL7: _tmp193=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp192=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL8:
# 1059
 _tmp195=_tmp193;_tmp194=_tmp192;goto _LLA;case 4U: _LL9: _tmp195=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp194=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LLA:
# 1061
 Cyc_scan_exp(_tmp195,dep);
Cyc_scan_exp(_tmp194,dep);
return;case 41U: _LLB: _tmp196=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LLC:
 _tmp197=_tmp196;goto _LLE;case 20U: _LLD: _tmp197=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LLE:
# 1066
 _tmp198=_tmp197;goto _LL10;case 18U: _LLF: _tmp198=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL10:
# 1068
 _tmp199=_tmp198;goto _LL12;case 15U: _LL11: _tmp199=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL12:
# 1070
 _tmp19A=_tmp199;goto _LL14;case 5U: _LL13: _tmp19A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL14:
# 1072
 Cyc_scan_exp(_tmp19A,dep);
return;case 6U: _LL15: _tmp19D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp19C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_tmp19B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp164)->f3;_LL16:
# 1075
 Cyc_scan_exp(_tmp19D,dep);
Cyc_scan_exp(_tmp19C,dep);
Cyc_scan_exp(_tmp19B,dep);
return;case 7U: _LL17: _tmp19F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp19E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL18:
 _tmp1A1=_tmp19F;_tmp1A0=_tmp19E;goto _LL1A;case 8U: _LL19: _tmp1A1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1A0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL1A:
# 1081
 Cyc_scan_exp(_tmp1A1,dep);
Cyc_scan_exp(_tmp1A0,dep);
return;case 10U: _LL1B: _tmp1A3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1A2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL1C:
# 1085
 Cyc_scan_exp(_tmp1A3,dep);
for(0;_tmp1A2 != 0;_tmp1A2=_tmp1A2->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp1A2->hd,dep);}
# 1089
return;case 14U: _LL1D: _tmp1A5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1A4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL1E:
# 1091
 Cyc_scan_type(_tmp1A5,dep);
Cyc_scan_exp(_tmp1A4,dep);
return;case 34U: _LL1F: _tmp1A9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).is_calloc;_tmp1A8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).rgn;_tmp1A7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).elt_type;_tmp1A6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp164)->f1).num_elts;_LL20:
# 1095
 if(_tmp1A8 != 0)Cyc_scan_exp(_tmp1A8,dep);
if(_tmp1A7 != 0)Cyc_scan_type(*_tmp1A7,dep);
Cyc_scan_exp(_tmp1A6,dep);
return;case 38U: _LL21: _tmp1AA=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL22:
# 1100
 Cyc_scan_exp(_tmp1AA,dep);return;case 39U: _LL23: _tmp1AB=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL24:
 _tmp1AC=_tmp1AB;goto _LL26;case 17U: _LL25: _tmp1AC=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_LL26:
# 1103
 Cyc_scan_type(_tmp1AC,dep);
return;case 21U: _LL27: _tmp1AE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1AD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL28:
# 1106
 _tmp1B0=_tmp1AE;_tmp1AF=_tmp1AD;goto _LL2A;case 22U: _LL29: _tmp1B0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1AF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL2A:
# 1108
 Cyc_scan_exp(_tmp1B0,dep);
Cyc_add_target(_tmp1AF);
return;case 19U: _LL2B: _tmp1B2=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp164)->f1;_tmp1B1=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL2C:
# 1112
 Cyc_scan_type(_tmp1B2,dep);
# 1114
{void*_tmp166=(void*)((struct Cyc_List_List*)_check_null(_tmp1B1))->hd;void*_tmp167=_tmp166;struct _dyneither_ptr*_tmp168;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp167)->tag == 0U){_LL56: _tmp168=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp167)->f1;_LL57:
 Cyc_add_target(_tmp168);goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 1118
return;case 0U: _LL2D: _LL2E:
# 1120
 return;case 36U: _LL2F: _tmp1B3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp164)->f2;_LL30:
# 1122
 for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){
struct _tuple28*_tmp169=(struct _tuple28*)_tmp1B3->hd;struct _tuple28*_tmp16A=_tmp169;struct Cyc_Absyn_Exp*_tmp16B;_LL5B: _tmp16B=_tmp16A->f2;_LL5C:;
Cyc_scan_exp(_tmp16B,dep);}
# 1126
return;case 40U: _LL31: _LL32:
 return;case 2U: _LL33: _LL34:
# 1129
({void*_tmp16C=0U;({struct Cyc___cycFILE*_tmp5C2=Cyc_stderr;struct _dyneither_ptr _tmp5C1=({const char*_tmp16D="Error: unexpected Pragma_e\n";_tag_dyneither(_tmp16D,sizeof(char),28U);});Cyc_fprintf(_tmp5C2,_tmp5C1,_tag_dyneither(_tmp16C,sizeof(void*),0U));});});
 exit(1);return;case 35U: _LL35: _LL36:
# 1132
({void*_tmp16E=0U;({struct Cyc___cycFILE*_tmp5C4=Cyc_stderr;struct _dyneither_ptr _tmp5C3=({const char*_tmp16F="Error: unexpected Swap_e\n";_tag_dyneither(_tmp16F,sizeof(char),26U);});Cyc_fprintf(_tmp5C4,_tmp5C3,_tag_dyneither(_tmp16E,sizeof(void*),0U));});});
 exit(1);return;case 37U: _LL37: _LL38:
# 1135
({void*_tmp170=0U;({struct Cyc___cycFILE*_tmp5C6=Cyc_stderr;struct _dyneither_ptr _tmp5C5=({const char*_tmp171="Error: unexpected Stmt_e\n";_tag_dyneither(_tmp171,sizeof(char),26U);});Cyc_fprintf(_tmp5C6,_tmp5C5,_tag_dyneither(_tmp170,sizeof(void*),0U));});});
 exit(1);return;case 11U: _LL39: _LL3A:
# 1138
({void*_tmp172=0U;({struct Cyc___cycFILE*_tmp5C8=Cyc_stderr;struct _dyneither_ptr _tmp5C7=({const char*_tmp173="Error: unexpected Throw_e\n";_tag_dyneither(_tmp173,sizeof(char),27U);});Cyc_fprintf(_tmp5C8,_tmp5C7,_tag_dyneither(_tmp172,sizeof(void*),0U));});});
 exit(1);return;case 12U: _LL3B: _LL3C:
# 1141
({void*_tmp174=0U;({struct Cyc___cycFILE*_tmp5CA=Cyc_stderr;struct _dyneither_ptr _tmp5C9=({const char*_tmp175="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmp175,sizeof(char),35U);});Cyc_fprintf(_tmp5CA,_tmp5C9,_tag_dyneither(_tmp174,sizeof(void*),0U));});});
 exit(1);return;case 13U: _LL3D: _LL3E:
# 1144
({void*_tmp176=0U;({struct Cyc___cycFILE*_tmp5CC=Cyc_stderr;struct _dyneither_ptr _tmp5CB=({const char*_tmp177="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmp177,sizeof(char),33U);});Cyc_fprintf(_tmp5CC,_tmp5CB,_tag_dyneither(_tmp176,sizeof(void*),0U));});});
 exit(1);return;case 16U: _LL3F: _LL40:
# 1147
({void*_tmp178=0U;({struct Cyc___cycFILE*_tmp5CE=Cyc_stderr;struct _dyneither_ptr _tmp5CD=({const char*_tmp179="Error: unexpected New_e\n";_tag_dyneither(_tmp179,sizeof(char),25U);});Cyc_fprintf(_tmp5CE,_tmp5CD,_tag_dyneither(_tmp178,sizeof(void*),0U));});});
 exit(1);return;case 24U: _LL41: _LL42:
# 1150
({void*_tmp17A=0U;({struct Cyc___cycFILE*_tmp5D0=Cyc_stderr;struct _dyneither_ptr _tmp5CF=({const char*_tmp17B="Error: unexpected Tuple_e\n";_tag_dyneither(_tmp17B,sizeof(char),27U);});Cyc_fprintf(_tmp5D0,_tmp5CF,_tag_dyneither(_tmp17A,sizeof(void*),0U));});});
 exit(1);return;case 25U: _LL43: _LL44:
# 1153
({void*_tmp17C=0U;({struct Cyc___cycFILE*_tmp5D2=Cyc_stderr;struct _dyneither_ptr _tmp5D1=({const char*_tmp17D="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmp17D,sizeof(char),33U);});Cyc_fprintf(_tmp5D2,_tmp5D1,_tag_dyneither(_tmp17C,sizeof(void*),0U));});});
 exit(1);return;case 26U: _LL45: _LL46:
# 1156
({void*_tmp17E=0U;({struct Cyc___cycFILE*_tmp5D4=Cyc_stderr;struct _dyneither_ptr _tmp5D3=({const char*_tmp17F="Error: unexpected Array_e\n";_tag_dyneither(_tmp17F,sizeof(char),27U);});Cyc_fprintf(_tmp5D4,_tmp5D3,_tag_dyneither(_tmp17E,sizeof(void*),0U));});});
 exit(1);return;case 27U: _LL47: _LL48:
# 1159
({void*_tmp180=0U;({struct Cyc___cycFILE*_tmp5D6=Cyc_stderr;struct _dyneither_ptr _tmp5D5=({const char*_tmp181="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmp181,sizeof(char),35U);});Cyc_fprintf(_tmp5D6,_tmp5D5,_tag_dyneither(_tmp180,sizeof(void*),0U));});});
 exit(1);return;case 28U: _LL49: _LL4A:
# 1162
({void*_tmp182=0U;({struct Cyc___cycFILE*_tmp5D8=Cyc_stderr;struct _dyneither_ptr _tmp5D7=({const char*_tmp183="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmp183,sizeof(char),41U);});Cyc_fprintf(_tmp5D8,_tmp5D7,_tag_dyneither(_tmp182,sizeof(void*),0U));});});
 exit(1);return;case 29U: _LL4B: _LL4C:
# 1165
({void*_tmp184=0U;({struct Cyc___cycFILE*_tmp5DA=Cyc_stderr;struct _dyneither_ptr _tmp5D9=({const char*_tmp185="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmp185,sizeof(char),31U);});Cyc_fprintf(_tmp5DA,_tmp5D9,_tag_dyneither(_tmp184,sizeof(void*),0U));});});
 exit(1);return;case 30U: _LL4D: _LL4E:
# 1168
({void*_tmp186=0U;({struct Cyc___cycFILE*_tmp5DC=Cyc_stderr;struct _dyneither_ptr _tmp5DB=({const char*_tmp187="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmp187,sizeof(char),32U);});Cyc_fprintf(_tmp5DC,_tmp5DB,_tag_dyneither(_tmp186,sizeof(void*),0U));});});
 exit(1);return;case 31U: _LL4F: _LL50:
# 1171
({void*_tmp188=0U;({struct Cyc___cycFILE*_tmp5DE=Cyc_stderr;struct _dyneither_ptr _tmp5DD=({const char*_tmp189="Error: unexpected Datatype_e\n";_tag_dyneither(_tmp189,sizeof(char),30U);});Cyc_fprintf(_tmp5DE,_tmp5DD,_tag_dyneither(_tmp188,sizeof(void*),0U));});});
 exit(1);return;case 32U: _LL51: _LL52:
# 1174
({void*_tmp18A=0U;({struct Cyc___cycFILE*_tmp5E0=Cyc_stderr;struct _dyneither_ptr _tmp5DF=({const char*_tmp18B="Error: unexpected Enum_e\n";_tag_dyneither(_tmp18B,sizeof(char),26U);});Cyc_fprintf(_tmp5E0,_tmp5DF,_tag_dyneither(_tmp18A,sizeof(void*),0U));});});
 exit(1);return;default: _LL53: _LL54:
# 1177
({void*_tmp18C=0U;({struct Cyc___cycFILE*_tmp5E2=Cyc_stderr;struct _dyneither_ptr _tmp5E1=({const char*_tmp18D="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp18D,sizeof(char),30U);});Cyc_fprintf(_tmp5E2,_tmp5E1,_tag_dyneither(_tmp18C,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1182
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 1187
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp1B4=t;struct Cyc_Absyn_Datatypedecl*_tmp1E0;struct Cyc_Absyn_Enumdecl*_tmp1DF;struct Cyc_Absyn_Aggrdecl*_tmp1DE;struct _dyneither_ptr*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_FnInfo _tmp1DB;struct Cyc_Absyn_Exp*_tmp1DA;void*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1D8;void*_tmp1D7;struct Cyc_Absyn_PtrInfo _tmp1D6;void*_tmp1D5;struct Cyc_List_List*_tmp1D4;switch(*((int*)_tmp1B4)){case 0U: _LL1: _tmp1D5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B4)->f1;_tmp1D4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B4)->f2;_LL2: {
# 1191
void*_tmp1B5=_tmp1D5;struct _dyneither_ptr*_tmp1BD;union Cyc_Absyn_AggrInfo _tmp1BC;switch(*((int*)_tmp1B5)){case 0U: _LL1E: _LL1F:
 goto _LL21;case 1U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 goto _LL25;case 2U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
# 1197
 return;case 20U: _LL28: _tmp1BC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp1B5)->f1;_LL29: {
# 1199
struct _tuple11 _tmp1B6=Cyc_Absyn_aggr_kinded_name(_tmp1BC);struct _tuple11 _tmp1B7=_tmp1B6;struct _dyneither_ptr*_tmp1B8;_LL49: _tmp1B8=(_tmp1B7.f2)->f2;_LL4A:;
_tmp1BD=_tmp1B8;goto _LL2B;}case 15U: _LL2A: _tmp1BD=(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp1B5)->f1)->f2;_LL2B:
 Cyc_add_target(_tmp1BD);return;case 18U: _LL2C: _LL2D:
 goto _LL2F;case 19U: _LL2E: _LL2F: goto _LL31;case 3U: _LL30: _LL31:
 goto _LL33;case 5U: _LL32: _LL33: goto _LL35;case 6U: _LL34: _LL35:
 goto _LL37;case 7U: _LL36: _LL37: goto _LL39;case 8U: _LL38: _LL39:
 goto _LL3B;case 9U: _LL3A: _LL3B: goto _LL3D;case 10U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F: goto _LL41;case 11U: _LL40: _LL41:
 goto _LL43;case 12U: _LL42: _LL43: goto _LL45;case 13U: _LL44: _LL45:
 goto _LL47;default: _LL46: _LL47:
({struct Cyc_String_pa_PrintArg_struct _tmp1BB=({struct Cyc_String_pa_PrintArg_struct _tmp48A;_tmp48A.tag=0U,({struct _dyneither_ptr _tmp5E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp48A.f1=_tmp5E3;});_tmp48A;});void*_tmp1B9[1U];_tmp1B9[0]=& _tmp1BB;({struct Cyc___cycFILE*_tmp5E5=Cyc_stderr;struct _dyneither_ptr _tmp5E4=({const char*_tmp1BA="Error: unexpected %s\n";_tag_dyneither(_tmp1BA,sizeof(char),22U);});Cyc_fprintf(_tmp5E5,_tmp5E4,_tag_dyneither(_tmp1B9,sizeof(void*),1U));});});
 exit(1);return;}_LL1D:;}case 3U: _LL3: _tmp1D6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B4)->f1;_LL4:
# 1214
 Cyc_scan_type(_tmp1D6.elt_type,dep);
return;case 4U: _LL5: _tmp1D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).elt_type;_tmp1D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).num_elts;_tmp1D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).zero_term;_LL6:
# 1217
 Cyc_scan_type(_tmp1D9,dep);
Cyc_scan_exp_opt(_tmp1D8,dep);
return;case 11U: _LL7: _tmp1DA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp1B4)->f1;_LL8:
# 1221
 Cyc_scan_exp(_tmp1DA,dep);
return;case 5U: _LL9: _tmp1DB=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1;_LLA:
# 1224
 Cyc_scan_type(_tmp1DB.ret_type,dep);
{struct Cyc_List_List*_tmp1BE=_tmp1DB.args;for(0;_tmp1BE != 0;_tmp1BE=_tmp1BE->tl){
struct _tuple9*_tmp1BF=(struct _tuple9*)_tmp1BE->hd;struct _tuple9*_tmp1C0=_tmp1BF;void*_tmp1C1;_LL4C: _tmp1C1=_tmp1C0->f3;_LL4D:;
Cyc_scan_type(_tmp1C1,dep);}}
# 1229
if(_tmp1DB.cyc_varargs != 0)
Cyc_scan_type((_tmp1DB.cyc_varargs)->type,dep);
return;case 7U: _LLB: _tmp1DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B4)->f2;_LLC:
# 1233
 for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp1DC->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp1DC->hd)->width,dep);}
# 1237
return;case 8U: _LLD: _tmp1DD=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B4)->f1)->f2;_LLE:
# 1239
 Cyc_add_target(_tmp1DD);
return;case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)){case 0U: _LLF: _tmp1DE=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)->f1;_LL10:
# 1243
({struct Cyc_Absyn_Decl*_tmp5E6=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->tag=5U,_tmp1C2->f1=_tmp1DE;_tmp1C2;}),0U);Cyc_scan_decl(_tmp5E6,dep);});{
struct _tuple1*_tmp1C3=_tmp1DE->name;struct _tuple1*_tmp1C4=_tmp1C3;struct _dyneither_ptr*_tmp1C5;_LL4F: _tmp1C5=_tmp1C4->f2;_LL50:;
Cyc_add_target(_tmp1C5);
return;};case 1U: _LL11: _tmp1DF=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)->f1;_LL12:
# 1249
({struct Cyc_Absyn_Decl*_tmp5E7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->tag=7U,_tmp1C6->f1=_tmp1DF;_tmp1C6;}),0U);Cyc_scan_decl(_tmp5E7,dep);});{
struct _tuple1*_tmp1C7=_tmp1DF->name;struct _tuple1*_tmp1C8=_tmp1C7;struct _dyneither_ptr*_tmp1C9;_LL52: _tmp1C9=_tmp1C8->f2;_LL53:;
Cyc_add_target(_tmp1C9);
return;};default: _LL13: _tmp1E0=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1B4)->f1)->r)->f1;_LL14:
# 1255
({void*_tmp1CA=0U;({struct Cyc___cycFILE*_tmp5E9=Cyc_stderr;struct _dyneither_ptr _tmp5E8=({const char*_tmp1CB="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp1CB,sizeof(char),40U);});Cyc_fprintf(_tmp5E9,_tmp5E8,_tag_dyneither(_tmp1CA,sizeof(void*),0U));});});
 exit(1);return;}case 1U: _LL15: _LL16:
# 1258
({void*_tmp1CC=0U;({struct Cyc___cycFILE*_tmp5EB=Cyc_stderr;struct _dyneither_ptr _tmp5EA=({const char*_tmp1CD="Error: unexpected Evar\n";_tag_dyneither(_tmp1CD,sizeof(char),24U);});Cyc_fprintf(_tmp5EB,_tmp5EA,_tag_dyneither(_tmp1CC,sizeof(void*),0U));});});
 exit(1);return;case 2U: _LL17: _LL18:
# 1261
({void*_tmp1CE=0U;({struct Cyc___cycFILE*_tmp5ED=Cyc_stderr;struct _dyneither_ptr _tmp5EC=({const char*_tmp1CF="Error: unexpected VarType\n";_tag_dyneither(_tmp1CF,sizeof(char),27U);});Cyc_fprintf(_tmp5ED,_tmp5EC,_tag_dyneither(_tmp1CE,sizeof(void*),0U));});});
 exit(1);return;case 6U: _LL19: _LL1A:
# 1264
({void*_tmp1D0=0U;({struct Cyc___cycFILE*_tmp5EF=Cyc_stderr;struct _dyneither_ptr _tmp5EE=({const char*_tmp1D1="Error: unexpected TupleType\n";_tag_dyneither(_tmp1D1,sizeof(char),29U);});Cyc_fprintf(_tmp5EF,_tmp5EE,_tag_dyneither(_tmp1D0,sizeof(void*),0U));});});
 exit(1);return;default: _LL1B: _LL1C:
# 1267
({void*_tmp1D2=0U;({struct Cyc___cycFILE*_tmp5F1=Cyc_stderr;struct _dyneither_ptr _tmp5F0=({const char*_tmp1D3="Error: unexpected valueof_t\n";_tag_dyneither(_tmp1D3,sizeof(char),29U);});Cyc_fprintf(_tmp5F1,_tmp5F0,_tag_dyneither(_tmp1D2,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1272
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp1E1=Cyc_current_targets;
struct _dyneither_ptr*_tmp1E2=Cyc_current_source;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1E3=_cycalloc(sizeof(*_tmp1E3));({struct Cyc_Set_Set*_tmp5F2=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp1E3=_tmp5F2;});_tmp1E3;});
{void*_tmp1E4=d->r;void*_tmp1E5=_tmp1E4;struct Cyc_Absyn_Typedefdecl*_tmp21D;struct Cyc_Absyn_Enumdecl*_tmp21C;struct Cyc_Absyn_Aggrdecl*_tmp21B;struct Cyc_Absyn_Fndecl*_tmp21A;struct Cyc_Absyn_Vardecl*_tmp219;switch(*((int*)_tmp1E5)){case 0U: _LL1: _tmp219=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1E5)->f1;_LL2: {
# 1278
struct _tuple1*_tmp1E6=_tmp219->name;struct _tuple1*_tmp1E7=_tmp1E6;struct _dyneither_ptr*_tmp1E8;_LL24: _tmp1E8=_tmp1E7->f2;_LL25:;
Cyc_current_source=_tmp1E8;
Cyc_scan_type(_tmp219->type,dep);
Cyc_scan_exp_opt(_tmp219->initializer,dep);
goto _LL0;}case 1U: _LL3: _tmp21A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1E5)->f1;_LL4: {
# 1284
struct _tuple1*_tmp1E9=_tmp21A->name;struct _tuple1*_tmp1EA=_tmp1E9;struct _dyneither_ptr*_tmp1F1;_LL27: _tmp1F1=_tmp1EA->f2;_LL28:;
Cyc_current_source=_tmp1F1;
Cyc_scan_type((_tmp21A->i).ret_type,dep);
{struct Cyc_List_List*_tmp1EB=(_tmp21A->i).args;for(0;_tmp1EB != 0;_tmp1EB=_tmp1EB->tl){
struct _tuple9*_tmp1EC=(struct _tuple9*)_tmp1EB->hd;struct _tuple9*_tmp1ED=_tmp1EC;void*_tmp1EE;_LL2A: _tmp1EE=_tmp1ED->f3;_LL2B:;
Cyc_scan_type(_tmp1EE,dep);}}
# 1291
if((_tmp21A->i).cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null((_tmp21A->i).cyc_varargs))->type,dep);
if(_tmp21A->is_inline)
({void*_tmp1EF=0U;({struct Cyc___cycFILE*_tmp5F4=Cyc_stderr;struct _dyneither_ptr _tmp5F3=({const char*_tmp1F0="Warning: ignoring inline function\n";_tag_dyneither(_tmp1F0,sizeof(char),35U);});Cyc_fprintf(_tmp5F4,_tmp5F3,_tag_dyneither(_tmp1EF,sizeof(void*),0U));});});
goto _LL0;}case 5U: _LL5: _tmp21B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1E5)->f1;_LL6: {
# 1297
struct _tuple1*_tmp1F2=_tmp21B->name;struct _tuple1*_tmp1F3=_tmp1F2;struct _dyneither_ptr*_tmp1F7;_LL2D: _tmp1F7=_tmp1F3->f2;_LL2E:;
Cyc_current_source=_tmp1F7;
if((unsigned int)_tmp21B->impl){
{struct Cyc_List_List*_tmp1F4=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21B->impl))->fields;for(0;_tmp1F4 != 0;_tmp1F4=_tmp1F4->tl){
struct Cyc_Absyn_Aggrfield*_tmp1F5=(struct Cyc_Absyn_Aggrfield*)_tmp1F4->hd;
Cyc_scan_type(_tmp1F5->type,dep);
Cyc_scan_exp_opt(_tmp1F5->width,dep);}}{
# 1307
struct Cyc_List_List*_tmp1F6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp21B->impl))->fields;for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){;}};}
# 1311
goto _LL0;}case 7U: _LL7: _tmp21C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1E5)->f1;_LL8: {
# 1313
struct _tuple1*_tmp1F8=_tmp21C->name;struct _tuple1*_tmp1F9=_tmp1F8;struct _dyneither_ptr*_tmp1FD;_LL30: _tmp1FD=_tmp1F9->f2;_LL31:;
Cyc_current_source=_tmp1FD;
if((unsigned int)_tmp21C->fields){
{struct Cyc_List_List*_tmp1FA=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp21C->fields))->v;for(0;_tmp1FA != 0;_tmp1FA=_tmp1FA->tl){
struct Cyc_Absyn_Enumfield*_tmp1FB=(struct Cyc_Absyn_Enumfield*)_tmp1FA->hd;
Cyc_scan_exp_opt(_tmp1FB->tag,dep);}}{
# 1322
struct Cyc_List_List*_tmp1FC=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp21C->fields))->v;for(0;_tmp1FC != 0;_tmp1FC=_tmp1FC->tl){;}};}
# 1326
goto _LL0;}case 8U: _LL9: _tmp21D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1E5)->f1;_LLA: {
# 1328
struct _tuple1*_tmp1FE=_tmp21D->name;struct _tuple1*_tmp1FF=_tmp1FE;struct _dyneither_ptr*_tmp200;_LL33: _tmp200=_tmp1FF->f2;_LL34:;
Cyc_current_source=_tmp200;
if((unsigned int)_tmp21D->defn)
Cyc_scan_type((void*)_check_null(_tmp21D->defn),dep);
goto _LL0;}case 4U: _LLB: _LLC:
# 1334
({void*_tmp201=0U;({struct Cyc___cycFILE*_tmp5F6=Cyc_stderr;struct _dyneither_ptr _tmp5F5=({const char*_tmp202="Error: unexpected region declaration";_tag_dyneither(_tmp202,sizeof(char),37U);});Cyc_fprintf(_tmp5F6,_tmp5F5,_tag_dyneither(_tmp201,sizeof(void*),0U));});});
 exit(1);case 13U: _LLD: _LLE:
# 1337
({void*_tmp203=0U;({struct Cyc___cycFILE*_tmp5F8=Cyc_stderr;struct _dyneither_ptr _tmp5F7=({const char*_tmp204="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp204,sizeof(char),38U);});Cyc_fprintf(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp203,sizeof(void*),0U));});});
 exit(1);case 14U: _LLF: _LL10:
# 1340
({void*_tmp205=0U;({struct Cyc___cycFILE*_tmp5FA=Cyc_stderr;struct _dyneither_ptr _tmp5F9=({const char*_tmp206="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp206,sizeof(char),39U);});Cyc_fprintf(_tmp5FA,_tmp5F9,_tag_dyneither(_tmp205,sizeof(void*),0U));});});
 exit(1);case 15U: _LL11: _LL12:
# 1343
({void*_tmp207=0U;({struct Cyc___cycFILE*_tmp5FC=Cyc_stderr;struct _dyneither_ptr _tmp5FB=({const char*_tmp208="Error: unexpected __tempest_on__";_tag_dyneither(_tmp208,sizeof(char),33U);});Cyc_fprintf(_tmp5FC,_tmp5FB,_tag_dyneither(_tmp207,sizeof(void*),0U));});});
 exit(1);case 16U: _LL13: _LL14:
# 1346
({void*_tmp209=0U;({struct Cyc___cycFILE*_tmp5FE=Cyc_stderr;struct _dyneither_ptr _tmp5FD=({const char*_tmp20A="Error: unexpected __tempest_off__";_tag_dyneither(_tmp20A,sizeof(char),34U);});Cyc_fprintf(_tmp5FE,_tmp5FD,_tag_dyneither(_tmp209,sizeof(void*),0U));});});
 exit(1);case 2U: _LL15: _LL16:
# 1349
({void*_tmp20B=0U;({struct Cyc___cycFILE*_tmp600=Cyc_stderr;struct _dyneither_ptr _tmp5FF=({const char*_tmp20C="Error: unexpected let declaration\n";_tag_dyneither(_tmp20C,sizeof(char),35U);});Cyc_fprintf(_tmp600,_tmp5FF,_tag_dyneither(_tmp20B,sizeof(void*),0U));});});
 exit(1);case 6U: _LL17: _LL18:
# 1352
({void*_tmp20D=0U;({struct Cyc___cycFILE*_tmp602=Cyc_stderr;struct _dyneither_ptr _tmp601=({const char*_tmp20E="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp20E,sizeof(char),40U);});Cyc_fprintf(_tmp602,_tmp601,_tag_dyneither(_tmp20D,sizeof(void*),0U));});});
 exit(1);case 3U: _LL19: _LL1A:
# 1355
({void*_tmp20F=0U;({struct Cyc___cycFILE*_tmp604=Cyc_stderr;struct _dyneither_ptr _tmp603=({const char*_tmp210="Error: unexpected let declaration\n";_tag_dyneither(_tmp210,sizeof(char),35U);});Cyc_fprintf(_tmp604,_tmp603,_tag_dyneither(_tmp20F,sizeof(void*),0U));});});
 exit(1);case 9U: _LL1B: _LL1C:
# 1358
({void*_tmp211=0U;({struct Cyc___cycFILE*_tmp606=Cyc_stderr;struct _dyneither_ptr _tmp605=({const char*_tmp212="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp212,sizeof(char),41U);});Cyc_fprintf(_tmp606,_tmp605,_tag_dyneither(_tmp211,sizeof(void*),0U));});});
 exit(1);case 10U: _LL1D: _LL1E:
# 1361
({void*_tmp213=0U;({struct Cyc___cycFILE*_tmp608=Cyc_stderr;struct _dyneither_ptr _tmp607=({const char*_tmp214="Error: unexpected using declaration\n";_tag_dyneither(_tmp214,sizeof(char),37U);});Cyc_fprintf(_tmp608,_tmp607,_tag_dyneither(_tmp213,sizeof(void*),0U));});});
 exit(1);case 11U: _LL1F: _LL20:
# 1364
({void*_tmp215=0U;({struct Cyc___cycFILE*_tmp60A=Cyc_stderr;struct _dyneither_ptr _tmp609=({const char*_tmp216="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp216,sizeof(char),42U);});Cyc_fprintf(_tmp60A,_tmp609,_tag_dyneither(_tmp215,sizeof(void*),0U));});});
 exit(1);default: _LL21: _LL22:
# 1367
({void*_tmp217=0U;({struct Cyc___cycFILE*_tmp60C=Cyc_stderr;struct _dyneither_ptr _tmp60B=({const char*_tmp218="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp218,sizeof(char),50U);});Cyc_fprintf(_tmp60C,_tmp60B,_tag_dyneither(_tmp217,sizeof(void*),0U));});});
 exit(1);}_LL0:;}{
# 1375
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp21E=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp21F;_push_handler(& _tmp21F);{int _tmp221=0;if(setjmp(_tmp21F.handler))_tmp221=1;if(!_tmp221){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp21E);;_pop_handler();}else{void*_tmp220=(void*)_exn_thrown;void*_tmp222=_tmp220;void*_tmp223;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp222)->tag == Cyc_Core_Not_found){_LL36: _LL37:
# 1380
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL35;}else{_LL38: _tmp223=_tmp222;_LL39:(int)_rethrow(_tmp223);}_LL35:;}};}{
# 1382
struct Cyc_Set_Set*_tmp224=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp21E,_tmp224);
# 1385
Cyc_current_targets=_tmp1E1;
Cyc_current_source=_tmp1E2;};};}
# 1389
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1393
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp225;_push_handler(& _tmp225);{int _tmp227=0;if(setjmp(_tmp225.handler))_tmp227=1;if(!_tmp227){{struct Cyc_Set_Set*_tmp228=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0U);return _tmp228;};_pop_handler();}else{void*_tmp226=(void*)_exn_thrown;void*_tmp229=_tmp226;void*_tmp22A;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp229)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 1396
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL3: _tmp22A=_tmp229;_LL4:(int)_rethrow(_tmp22A);}_LL0:;}};}
# 1400
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1410 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
# 1415
struct Cyc_Set_Set*_tmp22B=curr;
# 1417
struct _dyneither_ptr*_tmp22C=({struct _dyneither_ptr*_tmp230=_cycalloc(sizeof(*_tmp230));({struct _dyneither_ptr _tmp60D=({const char*_tmp22F="";_tag_dyneither(_tmp22F,sizeof(char),1U);});*_tmp230=_tmp60D;});_tmp230;});
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp22B)> 0){
struct Cyc_Set_Set*_tmp22D=emptyset;
struct Cyc_Iter_Iter _tmp22E=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp22B);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp22E,& _tmp22C)){
_tmp22D=({struct Cyc_Set_Set*_tmp60E=_tmp22D;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp60E,Cyc_find(t,_tmp22C));});}
_tmp22B=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp22D,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp22B);}
# 1426
return curr;};}
# 1429
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0U,Cyc_GATHER  = 1U,Cyc_GATHERSCRIPT  = 2U,Cyc_FINISH  = 3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return(int)Cyc_mode == (int)Cyc_GATHER  || (int)Cyc_mode == (int)Cyc_GATHERSCRIPT;}
# 1435
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1439
if(Cyc_script_file == 0){
({void*_tmp231=0U;({struct Cyc___cycFILE*_tmp610=Cyc_stderr;struct _dyneither_ptr _tmp60F=({const char*_tmp232="Internal error: script file is NULL\n";_tag_dyneither(_tmp232,sizeof(char),37U);});Cyc_fprintf(_tmp610,_tmp60F,_tag_dyneither(_tmp231,sizeof(void*),0U));});});
 exit(1);}
# 1443
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1446
int Cyc_force_directory(struct _dyneither_ptr d){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp235=({struct Cyc_String_pa_PrintArg_struct _tmp48C;_tmp48C.tag=0U,_tmp48C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp48C;});struct Cyc_String_pa_PrintArg_struct _tmp236=({struct Cyc_String_pa_PrintArg_struct _tmp48B;_tmp48B.tag=0U,_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp48B;});void*_tmp233[2U];_tmp233[0]=& _tmp235,_tmp233[1]=& _tmp236;({struct _dyneither_ptr _tmp611=({const char*_tmp234="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp234,sizeof(char),36U);});Cyc_prscript(_tmp611,_tag_dyneither(_tmp233,sizeof(void*),2U));});});else{
# 1453
int _tmp237=({void*_tmp23B=0U;({const char*_tmp612=(const char*)_untag_dyneither_ptr(d,sizeof(char),1U);Cyc_open(_tmp612,0,_tag_dyneither(_tmp23B,sizeof(unsigned short),0U));});});
if(_tmp237 == -1){
if( mkdir((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1U)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp23A=({struct Cyc_String_pa_PrintArg_struct _tmp48D;_tmp48D.tag=0U,_tmp48D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp48D;});void*_tmp238[1U];_tmp238[0]=& _tmp23A;({struct Cyc___cycFILE*_tmp614=Cyc_stderr;struct _dyneither_ptr _tmp613=({const char*_tmp239="Error: could not create directory %s\n";_tag_dyneither(_tmp239,sizeof(char),38U);});Cyc_fprintf(_tmp614,_tmp613,_tag_dyneither(_tmp238,sizeof(void*),1U));});});
return 1;}}else{
# 1460
 close(_tmp237);}}
# 1462
return 0;}
# 1465
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1469
struct _dyneither_ptr _tmp23C=Cyc_strdup((struct _dyneither_ptr)file);
# 1471
struct Cyc_List_List*_tmp23D=0;
while(1){
_tmp23C=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp23C);
if(Cyc_strlen((struct _dyneither_ptr)_tmp23C)== (unsigned long)0)break;
_tmp23D=({struct Cyc_List_List*_tmp23F=_cycalloc(sizeof(*_tmp23F));({struct _dyneither_ptr*_tmp615=({struct _dyneither_ptr*_tmp23E=_cycalloc(sizeof(*_tmp23E));*_tmp23E=(struct _dyneither_ptr)_tmp23C;_tmp23E;});_tmp23F->hd=_tmp615;}),_tmp23F->tl=_tmp23D;_tmp23F;});}
# 1478
for(0;_tmp23D != 0;_tmp23D=_tmp23D->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp23D->hd)))return 1;}
# 1481
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1488
static int Cyc_is_other_special(char c){
char _tmp240=c;switch(_tmp240){case 92U: _LL1: _LL2:
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
# 1503
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 1509
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp241=Cyc_strlen((struct _dyneither_ptr)s);
# 1513
int _tmp242=0;
int _tmp243=0;
{int i=0;for(0;(unsigned long)i < _tmp241;++ i){
char _tmp244=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if((int)_tmp244 == (int)'\'')++ _tmp242;else{
if(Cyc_is_other_special(_tmp244))++ _tmp243;}}}
# 1522
if(_tmp242 == 0  && _tmp243 == 0)
return s;
# 1526
if(_tmp242 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp245[3U];({struct _dyneither_ptr*_tmp61A=({struct _dyneither_ptr*_tmp247=_cycalloc(sizeof(*_tmp247));({struct _dyneither_ptr _tmp619=({const char*_tmp246="'";_tag_dyneither(_tmp246,sizeof(char),2U);});*_tmp247=_tmp619;});_tmp247;});_tmp245[0]=_tmp61A;}),({struct _dyneither_ptr*_tmp618=({struct _dyneither_ptr*_tmp248=_cycalloc(sizeof(*_tmp248));*_tmp248=(struct _dyneither_ptr)s;_tmp248;});_tmp245[1]=_tmp618;}),({struct _dyneither_ptr*_tmp617=({struct _dyneither_ptr*_tmp24A=_cycalloc(sizeof(*_tmp24A));({struct _dyneither_ptr _tmp616=({const char*_tmp249="'";_tag_dyneither(_tmp249,sizeof(char),2U);});*_tmp24A=_tmp616;});_tmp24A;});_tmp245[2]=_tmp617;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp245,sizeof(struct _dyneither_ptr*),3U));}));{
# 1530
unsigned long _tmp24B=(_tmp241 + (unsigned long)_tmp242)+ (unsigned long)_tmp243;
struct _dyneither_ptr s2=({unsigned int _tmp256=(_tmp24B + (unsigned long)1)+ 1U;char*_tmp255=_cycalloc_atomic(_check_times(_tmp256,sizeof(char)));({{unsigned int _tmp48E=_tmp24B + (unsigned long)1;unsigned int i;for(i=0;i < _tmp48E;++ i){_tmp255[i]='\000';}_tmp255[_tmp48E]=0;}0;});_tag_dyneither(_tmp255,sizeof(char),_tmp256);});
int _tmp24C=0;
int _tmp24D=0;
for(0;(unsigned long)_tmp24C < _tmp241;++ _tmp24C){
char _tmp24E=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp24C));
if((int)_tmp24E == (int)'\''  || Cyc_is_other_special(_tmp24E))
({struct _dyneither_ptr _tmp24F=_dyneither_ptr_plus(s2,sizeof(char),_tmp24D ++);char _tmp250=*((char*)_check_dyneither_subscript(_tmp24F,sizeof(char),0U));char _tmp251='\\';if(_get_dyneither_size(_tmp24F,sizeof(char))== 1U  && (_tmp250 == 0  && _tmp251 != 0))_throw_arraybounds();*((char*)_tmp24F.curr)=_tmp251;});
({struct _dyneither_ptr _tmp252=_dyneither_ptr_plus(s2,sizeof(char),_tmp24D ++);char _tmp253=*((char*)_check_dyneither_subscript(_tmp252,sizeof(char),0U));char _tmp254=_tmp24E;if(_get_dyneither_size(_tmp252,sizeof(char))== 1U  && (_tmp253 == 0  && _tmp254 != 0))_throw_arraybounds();*((char*)_tmp252.curr)=_tmp254;});}
# 1540
return(struct _dyneither_ptr)s2;};}
# 1542
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp257=_cycalloc(sizeof(*_tmp257));({struct _dyneither_ptr _tmp61B=Cyc_sh_escape_string(*sp);*_tmp257=_tmp61B;});_tmp257;});}
# 1547
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1555
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1560
({struct Cyc_String_pa_PrintArg_struct _tmp25A=({struct Cyc_String_pa_PrintArg_struct _tmp48F;_tmp48F.tag=0U,({
struct _dyneither_ptr _tmp61C=(struct _dyneither_ptr)({const char*_tmp25B=filename;_tag_dyneither(_tmp25B,sizeof(char),_get_zero_arr_size_char((void*)_tmp25B,1U));});_tmp48F.f1=_tmp61C;});_tmp48F;});void*_tmp258[1U];_tmp258[0]=& _tmp25A;({struct Cyc___cycFILE*_tmp61E=Cyc_stderr;struct _dyneither_ptr _tmp61D=({const char*_tmp259="********************************* %s...\n";_tag_dyneither(_tmp259,sizeof(char),41U);});Cyc_fprintf(_tmp61E,_tmp61D,_tag_dyneither(_tmp258,sizeof(void*),1U));});});
# 1563
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp25E=({struct Cyc_String_pa_PrintArg_struct _tmp490;_tmp490.tag=0U,({struct _dyneither_ptr _tmp61F=(struct _dyneither_ptr)({const char*_tmp25F=filename;_tag_dyneither(_tmp25F,sizeof(char),_get_zero_arr_size_char((void*)_tmp25F,1U));});_tmp490.f1=_tmp61F;});_tmp490;});void*_tmp25C[1U];_tmp25C[0]=& _tmp25E;({struct _dyneither_ptr _tmp620=({const char*_tmp25D="\n%s:\n";_tag_dyneither(_tmp25D,sizeof(char),6U);});Cyc_log(_tmp620,_tag_dyneither(_tmp25C,sizeof(void*),1U));});});{
# 1575 "buildlib.cyl"
struct _dyneither_ptr _tmp260=Cyc_Filename_basename(({const char*_tmp3AD=filename;_tag_dyneither(_tmp3AD,sizeof(char),_get_zero_arr_size_char((void*)_tmp3AD,1U));}));
struct _dyneither_ptr _tmp261=Cyc_Filename_dirname(({const char*_tmp3AC=filename;_tag_dyneither(_tmp3AC,sizeof(char),_get_zero_arr_size_char((void*)_tmp3AC,1U));}));
struct _dyneither_ptr _tmp262=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp260);
const char*_tmp263=(const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp621=(struct _dyneither_ptr)_tmp262;Cyc_strconcat(_tmp621,({const char*_tmp3AB=".iA";_tag_dyneither(_tmp3AB,sizeof(char),4U);}));}),sizeof(char),1U));
# 1580
const char*_tmp264=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp261,sizeof(char))== (unsigned int)0?({struct Cyc_String_pa_PrintArg_struct _tmp3A7=({struct Cyc_String_pa_PrintArg_struct _tmp4C4;_tmp4C4.tag=0U,_tmp4C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp262);_tmp4C4;});void*_tmp3A5[1U];_tmp3A5[0]=& _tmp3A7;({struct _dyneither_ptr _tmp624=({const char*_tmp3A6="%s.iB";_tag_dyneither(_tmp3A6,sizeof(char),6U);});Cyc_aprintf(_tmp624,_tag_dyneither(_tmp3A5,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp623=(struct _dyneither_ptr)_tmp261;Cyc_Filename_concat(_tmp623,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3AA=({struct Cyc_String_pa_PrintArg_struct _tmp4C5;_tmp4C5.tag=0U,_tmp4C5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp262);_tmp4C5;});void*_tmp3A8[1U];_tmp3A8[0]=& _tmp3AA;({struct _dyneither_ptr _tmp622=({const char*_tmp3A9="%s.iB";_tag_dyneither(_tmp3A9,sizeof(char),6U);});Cyc_aprintf(_tmp622,_tag_dyneither(_tmp3A8,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1585
const char*_tmp265=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp261,sizeof(char))== (unsigned int)0?({struct Cyc_String_pa_PrintArg_struct _tmp3A1=({struct Cyc_String_pa_PrintArg_struct _tmp4C2;_tmp4C2.tag=0U,_tmp4C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp262);_tmp4C2;});void*_tmp39F[1U];_tmp39F[0]=& _tmp3A1;({struct _dyneither_ptr _tmp627=({const char*_tmp3A0="%s.iC";_tag_dyneither(_tmp3A0,sizeof(char),6U);});Cyc_aprintf(_tmp627,_tag_dyneither(_tmp39F,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp626=(struct _dyneither_ptr)_tmp261;Cyc_Filename_concat(_tmp626,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A4=({struct Cyc_String_pa_PrintArg_struct _tmp4C3;_tmp4C3.tag=0U,_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp262);_tmp4C3;});void*_tmp3A2[1U];_tmp3A2[0]=& _tmp3A4;({struct _dyneither_ptr _tmp625=({const char*_tmp3A3="%s.iC";_tag_dyneither(_tmp3A3,sizeof(char),6U);});Cyc_aprintf(_tmp625,_tag_dyneither(_tmp3A2,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1590
const char*_tmp266=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp261,sizeof(char))== (unsigned int)0?({struct Cyc_String_pa_PrintArg_struct _tmp39B=({struct Cyc_String_pa_PrintArg_struct _tmp4C0;_tmp4C0.tag=0U,_tmp4C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp262);_tmp4C0;});void*_tmp399[1U];_tmp399[0]=& _tmp39B;({struct _dyneither_ptr _tmp62A=({const char*_tmp39A="%s.iD";_tag_dyneither(_tmp39A,sizeof(char),6U);});Cyc_aprintf(_tmp62A,_tag_dyneither(_tmp399,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp629=(struct _dyneither_ptr)_tmp261;Cyc_Filename_concat(_tmp629,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39E=({struct Cyc_String_pa_PrintArg_struct _tmp4C1;_tmp4C1.tag=0U,_tmp4C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp262);_tmp4C1;});void*_tmp39C[1U];_tmp39C[0]=& _tmp39E;({struct _dyneither_ptr _tmp628=({const char*_tmp39D="%s.iD";_tag_dyneither(_tmp39D,sizeof(char),6U);});Cyc_aprintf(_tmp628,_tag_dyneither(_tmp39C,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1595
{struct _handler_cons _tmp267;_push_handler(& _tmp267);{int _tmp269=0;if(setjmp(_tmp267.handler))_tmp269=1;if(!_tmp269){
# 1598
if(Cyc_force_directory_prefixes(({const char*_tmp26A=filename;_tag_dyneither(_tmp26A,sizeof(char),_get_zero_arr_size_char((void*)_tmp26A,1U));}))){
int _tmp26B=1;_npop_handler(0U);return _tmp26B;}
# 1603
if((int)Cyc_mode != (int)Cyc_FINISH){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp26E=({struct Cyc_String_pa_PrintArg_struct _tmp491;_tmp491.tag=0U,({struct _dyneither_ptr _tmp62B=(struct _dyneither_ptr)({const char*_tmp26F=_tmp263;_tag_dyneither(_tmp26F,sizeof(char),_get_zero_arr_size_char((void*)_tmp26F,1U));});_tmp491.f1=_tmp62B;});_tmp491;});void*_tmp26C[1U];_tmp26C[0]=& _tmp26E;({struct _dyneither_ptr _tmp62C=({const char*_tmp26D="cat >%s <<XXX\n";_tag_dyneither(_tmp26D,sizeof(char),15U);});Cyc_prscript(_tmp62C,_tag_dyneither(_tmp26C,sizeof(void*),1U));});});
{struct Cyc_List_List*_tmp270=cpp_insert;for(0;_tmp270 != 0;_tmp270=_tmp270->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp273=({struct Cyc_String_pa_PrintArg_struct _tmp492;_tmp492.tag=0U,_tmp492.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp270->hd));_tmp492;});void*_tmp271[1U];_tmp271[0]=& _tmp273;({struct _dyneither_ptr _tmp62D=({const char*_tmp272="%s";_tag_dyneither(_tmp272,sizeof(char),3U);});Cyc_prscript(_tmp62D,_tag_dyneither(_tmp271,sizeof(void*),1U));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp276=({struct Cyc_String_pa_PrintArg_struct _tmp493;_tmp493.tag=0U,({struct _dyneither_ptr _tmp62E=(struct _dyneither_ptr)({const char*_tmp277=filename;_tag_dyneither(_tmp277,sizeof(char),_get_zero_arr_size_char((void*)_tmp277,1U));});_tmp493.f1=_tmp62E;});_tmp493;});void*_tmp274[1U];_tmp274[0]=& _tmp276;({struct _dyneither_ptr _tmp62F=({const char*_tmp275="#include <%s>\n";_tag_dyneither(_tmp275,sizeof(char),15U);});Cyc_prscript(_tmp62F,_tag_dyneither(_tmp274,sizeof(void*),1U));});});
({void*_tmp278=0U;({struct _dyneither_ptr _tmp630=({const char*_tmp279="XXX\n";_tag_dyneither(_tmp279,sizeof(char),5U);});Cyc_prscript(_tmp630,_tag_dyneither(_tmp278,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp27C=({struct Cyc_String_pa_PrintArg_struct _tmp496;_tmp496.tag=0U,_tmp496.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp496;});struct Cyc_String_pa_PrintArg_struct _tmp27D=({struct Cyc_String_pa_PrintArg_struct _tmp495;_tmp495.tag=0U,({struct _dyneither_ptr _tmp631=(struct _dyneither_ptr)({const char*_tmp280=_tmp264;_tag_dyneither(_tmp280,sizeof(char),_get_zero_arr_size_char((void*)_tmp280,1U));});_tmp495.f1=_tmp631;});_tmp495;});struct Cyc_String_pa_PrintArg_struct _tmp27E=({struct Cyc_String_pa_PrintArg_struct _tmp494;_tmp494.tag=0U,({struct _dyneither_ptr _tmp632=(struct _dyneither_ptr)({const char*_tmp27F=_tmp263;_tag_dyneither(_tmp27F,sizeof(char),_get_zero_arr_size_char((void*)_tmp27F,1U));});_tmp494.f1=_tmp632;});_tmp494;});void*_tmp27A[3U];_tmp27A[0]=& _tmp27C,_tmp27A[1]=& _tmp27D,_tmp27A[2]=& _tmp27E;({struct _dyneither_ptr _tmp633=({const char*_tmp27B="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp27B,sizeof(char),35U);});Cyc_prscript(_tmp633,_tag_dyneither(_tmp27A,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp499;_tmp499.tag=0U,_tmp499.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp499;});struct Cyc_String_pa_PrintArg_struct _tmp284=({struct Cyc_String_pa_PrintArg_struct _tmp498;_tmp498.tag=0U,({struct _dyneither_ptr _tmp634=(struct _dyneither_ptr)({const char*_tmp287=_tmp265;_tag_dyneither(_tmp287,sizeof(char),_get_zero_arr_size_char((void*)_tmp287,1U));});_tmp498.f1=_tmp634;});_tmp498;});struct Cyc_String_pa_PrintArg_struct _tmp285=({struct Cyc_String_pa_PrintArg_struct _tmp497;_tmp497.tag=0U,({struct _dyneither_ptr _tmp635=(struct _dyneither_ptr)({const char*_tmp286=_tmp263;_tag_dyneither(_tmp286,sizeof(char),_get_zero_arr_size_char((void*)_tmp286,1U));});_tmp497.f1=_tmp635;});_tmp497;});void*_tmp281[3U];_tmp281[0]=& _tmp283,_tmp281[1]=& _tmp284,_tmp281[2]=& _tmp285;({struct _dyneither_ptr _tmp636=({const char*_tmp282="$GCC %s -E     -o %s -x c %s;\n";_tag_dyneither(_tmp282,sizeof(char),31U);});Cyc_prscript(_tmp636,_tag_dyneither(_tmp281,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp28A=({struct Cyc_String_pa_PrintArg_struct _tmp49A;_tmp49A.tag=0U,({struct _dyneither_ptr _tmp637=(struct _dyneither_ptr)({const char*_tmp28B=_tmp263;_tag_dyneither(_tmp28B,sizeof(char),_get_zero_arr_size_char((void*)_tmp28B,1U));});_tmp49A.f1=_tmp637;});_tmp49A;});void*_tmp288[1U];_tmp288[0]=& _tmp28A;({struct _dyneither_ptr _tmp638=({const char*_tmp289="rm %s\n";_tag_dyneither(_tmp289,sizeof(char),7U);});Cyc_prscript(_tmp638,_tag_dyneither(_tmp288,sizeof(void*),1U));});});}else{
# 1615
maybe=Cyc_fopen(_tmp263,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp28E=({struct Cyc_String_pa_PrintArg_struct _tmp49B;_tmp49B.tag=0U,({struct _dyneither_ptr _tmp639=(struct _dyneither_ptr)({const char*_tmp28F=_tmp263;_tag_dyneither(_tmp28F,sizeof(char),_get_zero_arr_size_char((void*)_tmp28F,1U));});_tmp49B.f1=_tmp639;});_tmp49B;});void*_tmp28C[1U];_tmp28C[0]=& _tmp28E;({struct Cyc___cycFILE*_tmp63B=Cyc_stderr;struct _dyneither_ptr _tmp63A=({const char*_tmp28D="Error: could not create file %s\n";_tag_dyneither(_tmp28D,sizeof(char),33U);});Cyc_fprintf(_tmp63B,_tmp63A,_tag_dyneither(_tmp28C,sizeof(void*),1U));});});{
int _tmp290=1;_npop_handler(0U);return _tmp290;};}
# 1620
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp293=({struct Cyc_String_pa_PrintArg_struct _tmp49C;_tmp49C.tag=0U,({struct _dyneither_ptr _tmp63C=(struct _dyneither_ptr)({const char*_tmp294=_tmp263;_tag_dyneither(_tmp294,sizeof(char),_get_zero_arr_size_char((void*)_tmp294,1U));});_tmp49C.f1=_tmp63C;});_tmp49C;});void*_tmp291[1U];_tmp291[0]=& _tmp293;({struct Cyc___cycFILE*_tmp63E=Cyc_stderr;struct _dyneither_ptr _tmp63D=({const char*_tmp292="Creating %s\n";_tag_dyneither(_tmp292,sizeof(char),13U);});Cyc_fprintf(_tmp63E,_tmp63D,_tag_dyneither(_tmp291,sizeof(void*),1U));});});
out_file=maybe;
{struct Cyc_List_List*_tmp295=cpp_insert;for(0;_tmp295 != 0;_tmp295=_tmp295->tl){
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp295->hd),sizeof(char),1U)),out_file);}}
# 1626
({struct Cyc_String_pa_PrintArg_struct _tmp298=({struct Cyc_String_pa_PrintArg_struct _tmp49D;_tmp49D.tag=0U,({struct _dyneither_ptr _tmp63F=(struct _dyneither_ptr)({const char*_tmp299=filename;_tag_dyneither(_tmp299,sizeof(char),_get_zero_arr_size_char((void*)_tmp299,1U));});_tmp49D.f1=_tmp63F;});_tmp49D;});void*_tmp296[1U];_tmp296[0]=& _tmp298;({struct Cyc___cycFILE*_tmp641=out_file;struct _dyneither_ptr _tmp640=({const char*_tmp297="#include <%s>\n";_tag_dyneither(_tmp297,sizeof(char),15U);});Cyc_fprintf(_tmp641,_tmp640,_tag_dyneither(_tmp296,sizeof(void*),1U));});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp29A=({
struct Cyc_List_List*_tmp645=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));({struct _dyneither_ptr*_tmp644=({struct _dyneither_ptr*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));({struct _dyneither_ptr _tmp643=(struct _dyneither_ptr)({const char*_tmp2B5="";_tag_dyneither(_tmp2B5,sizeof(char),1U);});*_tmp2B6=_tmp643;});_tmp2B6;});_tmp2B7->hd=_tmp644;}),({
struct Cyc_List_List*_tmp642=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp2B7->tl=_tmp642;});_tmp2B7;});
# 1629
Cyc_str_sepstr(_tmp645,({const char*_tmp2B8=" ";_tag_dyneither(_tmp2B8,sizeof(char),2U);}));});
# 1632
char*cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp4A9;_tmp4A9.tag=0U,_tmp4A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4A9;});struct Cyc_String_pa_PrintArg_struct _tmp2AF=({struct Cyc_String_pa_PrintArg_struct _tmp4A8;_tmp4A8.tag=0U,_tmp4A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4A8;});struct Cyc_String_pa_PrintArg_struct _tmp2B0=({struct Cyc_String_pa_PrintArg_struct _tmp4A7;_tmp4A7.tag=0U,_tmp4A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp29A);_tmp4A7;});struct Cyc_String_pa_PrintArg_struct _tmp2B1=({struct Cyc_String_pa_PrintArg_struct _tmp4A6;_tmp4A6.tag=0U,({struct _dyneither_ptr _tmp646=(struct _dyneither_ptr)({const char*_tmp2B4=_tmp264;_tag_dyneither(_tmp2B4,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B4,1U));});_tmp4A6.f1=_tmp646;});_tmp4A6;});struct Cyc_String_pa_PrintArg_struct _tmp2B2=({struct Cyc_String_pa_PrintArg_struct _tmp4A5;_tmp4A5.tag=0U,({struct _dyneither_ptr _tmp647=(struct _dyneither_ptr)({const char*_tmp2B3=_tmp263;_tag_dyneither(_tmp2B3,sizeof(char),_get_zero_arr_size_char((void*)_tmp2B3,1U));});_tmp4A5.f1=_tmp647;});_tmp4A5;});void*_tmp2AC[5U];_tmp2AC[0]=& _tmp2AE,_tmp2AC[1]=& _tmp2AF,_tmp2AC[2]=& _tmp2B0,_tmp2AC[3]=& _tmp2B1,_tmp2AC[4]=& _tmp2B2;({struct _dyneither_ptr _tmp648=({const char*_tmp2AD="%s %s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp2AD,sizeof(char),30U);});Cyc_aprintf(_tmp648,_tag_dyneither(_tmp2AC,sizeof(void*),5U));});}),sizeof(char),1U));
# 1635
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp29D=({struct Cyc_String_pa_PrintArg_struct _tmp49E;_tmp49E.tag=0U,({struct _dyneither_ptr _tmp649=(struct _dyneither_ptr)({char*_tmp29E=cmd;_tag_dyneither(_tmp29E,sizeof(char),_get_zero_arr_size_char((void*)_tmp29E,1U));});_tmp49E.f1=_tmp649;});_tmp49E;});void*_tmp29B[1U];_tmp29B[0]=& _tmp29D;({struct Cyc___cycFILE*_tmp64B=Cyc_stderr;struct _dyneither_ptr _tmp64A=({const char*_tmp29C="%s\n";_tag_dyneither(_tmp29C,sizeof(char),4U);});Cyc_fprintf(_tmp64B,_tmp64A,_tag_dyneither(_tmp29B,sizeof(void*),1U));});});
if(! system((const char*)cmd)){
# 1640
cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp2A1=({struct Cyc_String_pa_PrintArg_struct _tmp4A3;_tmp4A3.tag=0U,_tmp4A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4A3;});struct Cyc_String_pa_PrintArg_struct _tmp2A2=({struct Cyc_String_pa_PrintArg_struct _tmp4A2;_tmp4A2.tag=0U,_tmp4A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4A2;});struct Cyc_String_pa_PrintArg_struct _tmp2A3=({struct Cyc_String_pa_PrintArg_struct _tmp4A1;_tmp4A1.tag=0U,_tmp4A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp29A);_tmp4A1;});struct Cyc_String_pa_PrintArg_struct _tmp2A4=({struct Cyc_String_pa_PrintArg_struct _tmp4A0;_tmp4A0.tag=0U,({struct _dyneither_ptr _tmp64C=(struct _dyneither_ptr)({const char*_tmp2A7=_tmp265;_tag_dyneither(_tmp2A7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A7,1U));});_tmp4A0.f1=_tmp64C;});_tmp4A0;});struct Cyc_String_pa_PrintArg_struct _tmp2A5=({struct Cyc_String_pa_PrintArg_struct _tmp49F;_tmp49F.tag=0U,({struct _dyneither_ptr _tmp64D=(struct _dyneither_ptr)({const char*_tmp2A6=_tmp263;_tag_dyneither(_tmp2A6,sizeof(char),_get_zero_arr_size_char((void*)_tmp2A6,1U));});_tmp49F.f1=_tmp64D;});_tmp49F;});void*_tmp29F[5U];_tmp29F[0]=& _tmp2A1,_tmp29F[1]=& _tmp2A2,_tmp29F[2]=& _tmp2A3,_tmp29F[3]=& _tmp2A4,_tmp29F[4]=& _tmp2A5;({struct _dyneither_ptr _tmp64E=({const char*_tmp2A0="%s %s %s -E -o %s -x c %s";_tag_dyneither(_tmp2A0,sizeof(char),26U);});Cyc_aprintf(_tmp64E,_tag_dyneither(_tmp29F,sizeof(void*),5U));});}),sizeof(char),1U));
# 1643
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp4A4;_tmp4A4.tag=0U,({struct _dyneither_ptr _tmp64F=(struct _dyneither_ptr)({char*_tmp2AB=cmd;_tag_dyneither(_tmp2AB,sizeof(char),_get_zero_arr_size_char((void*)_tmp2AB,1U));});_tmp4A4.f1=_tmp64F;});_tmp4A4;});void*_tmp2A8[1U];_tmp2A8[0]=& _tmp2AA;({struct Cyc___cycFILE*_tmp651=Cyc_stderr;struct _dyneither_ptr _tmp650=({const char*_tmp2A9="%s\n";_tag_dyneither(_tmp2A9,sizeof(char),4U);});Cyc_fprintf(_tmp651,_tmp650,_tag_dyneither(_tmp2A8,sizeof(void*),1U));});});
 system((const char*)cmd);}};}}
# 1650
if(Cyc_gathering()){int _tmp2B9=0;_npop_handler(0U);return _tmp2B9;}{
# 1653
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp264,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp654=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BC=({struct Cyc_String_pa_PrintArg_struct _tmp4AA;_tmp4AA.tag=0U,({struct _dyneither_ptr _tmp652=(struct _dyneither_ptr)({const char*_tmp2BD=_tmp264;_tag_dyneither(_tmp2BD,sizeof(char),_get_zero_arr_size_char((void*)_tmp2BD,1U));});_tmp4AA.f1=_tmp652;});_tmp4AA;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BC;({struct _dyneither_ptr _tmp653=({const char*_tmp2BB="can't open macrosfile %s";_tag_dyneither(_tmp2BB,sizeof(char),25U);});Cyc_aprintf(_tmp653,_tag_dyneither(_tmp2BA,sizeof(void*),1U));});});_tmp2BE->f1=_tmp654;});_tmp2BE;}));
# 1658
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp2BF=Cyc_Lexing_from_file(in_file);
struct _tuple21*entry;
while((entry=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp2BF))!= 0){
struct _tuple21*_tmp2C0=(struct _tuple21*)_check_null(entry);struct _tuple21*_tmp2C1=_tmp2C0;struct _dyneither_ptr*_tmp2C3;struct Cyc_Set_Set*_tmp2C2;_LL1: _tmp2C3=_tmp2C1->f1;_tmp2C2=_tmp2C1->f2;_LL2:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp2C3,_tmp2C2);}
# 1667
Cyc_fclose(in_file);
# 1670
maybe=Cyc_fopen(_tmp265,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));_tmp2C8->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp657=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2C6=({struct Cyc_String_pa_PrintArg_struct _tmp4AB;_tmp4AB.tag=0U,({struct _dyneither_ptr _tmp655=(struct _dyneither_ptr)({const char*_tmp2C7=_tmp265;_tag_dyneither(_tmp2C7,sizeof(char),_get_zero_arr_size_char((void*)_tmp2C7,1U));});_tmp4AB.f1=_tmp655;});_tmp4AB;});void*_tmp2C4[1U];_tmp2C4[0]=& _tmp2C6;({struct _dyneither_ptr _tmp656=({const char*_tmp2C5="can't open declsfile %s";_tag_dyneither(_tmp2C5,sizeof(char),24U);});Cyc_aprintf(_tmp656,_tag_dyneither(_tmp2C4,sizeof(void*),1U));});});_tmp2C8->f1=_tmp657;});_tmp2C8;}));
# 1674
in_file=maybe;
_tmp2BF=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp266,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp2C9=1;_npop_handler(0U);return _tmp2C9;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp2BF)){;}{
# 1680
struct Cyc_List_List*_tmp2CA=user_defs;
while(_tmp2CA != 0){
struct _tuple24*_tmp2CB=(struct _tuple24*)_tmp2CA->hd;struct _tuple24*_tmp2CC=_tmp2CB;struct _dyneither_ptr*_tmp2D0;_LL4: _tmp2D0=_tmp2CC->f2;_LL5:;
({struct Cyc_String_pa_PrintArg_struct _tmp2CF=({struct Cyc_String_pa_PrintArg_struct _tmp4AC;_tmp4AC.tag=0U,_tmp4AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2D0);_tmp4AC;});void*_tmp2CD[1U];_tmp2CD[0]=& _tmp2CF;({struct Cyc___cycFILE*_tmp659=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp658=({const char*_tmp2CE="%s";_tag_dyneither(_tmp2CE,sizeof(char),3U);});Cyc_fprintf(_tmp659,_tmp658,_tag_dyneither(_tmp2CD,sizeof(void*),1U));});});
_tmp2CA=_tmp2CA->tl;}
# 1686
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if((int)Cyc_mode != (int)Cyc_FINISH)
;
# 1692
maybe=Cyc_fopen(_tmp266,"r");
if(!((unsigned int)maybe)){int _tmp2D1=1;_npop_handler(0U);return _tmp2D1;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp2D2=_tmp266;_tag_dyneither(_tmp2D2,sizeof(char),_get_zero_arr_size_char((void*)_tmp2D2,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp2D3=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1702
{struct Cyc_List_List*_tmp2D4=_tmp2D3;for(0;_tmp2D4 != 0;_tmp2D4=_tmp2D4->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp2D4->hd,t);}}{
# 1706
struct Cyc_List_List*_tmp2D5=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);
struct Cyc_Set_Set*_tmp2D6=({struct Cyc_List_List*_tmp65A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,_tmp2D5);Cyc_reachable(_tmp65A,t);});
# 1710
struct Cyc_List_List*_tmp2D7=0;
struct Cyc_List_List*_tmp2D8=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp2D9=_tmp2D3;for(0;_tmp2D9 != 0;_tmp2D9=_tmp2D9->tl){
struct Cyc_Absyn_Decl*_tmp2DA=(struct Cyc_Absyn_Decl*)_tmp2D9->hd;
struct _dyneither_ptr*name;
{void*_tmp2DB=_tmp2DA->r;void*_tmp2DC=_tmp2DB;struct Cyc_Absyn_Typedefdecl*_tmp2F7;struct Cyc_Absyn_Enumdecl*_tmp2F6;struct Cyc_Absyn_Aggrdecl*_tmp2F5;struct Cyc_Absyn_Fndecl*_tmp2F4;struct Cyc_Absyn_Vardecl*_tmp2F3;switch(*((int*)_tmp2DC)){case 0U: _LL7: _tmp2F3=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2DC)->f1;_LL8: {
# 1718
struct _tuple1*_tmp2DD=_tmp2F3->name;struct _tuple1*_tmp2DE=_tmp2DD;struct _dyneither_ptr*_tmp2DF;_LL2A: _tmp2DF=_tmp2DE->f2;_LL2B:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2DF);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp2DF))name=0;else{
name=_tmp2DF;}
goto _LL6;}case 1U: _LL9: _tmp2F4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2DC)->f1;_LLA: {
# 1724
struct _tuple1*_tmp2E0=_tmp2F4->name;struct _tuple1*_tmp2E1=_tmp2E0;struct _dyneither_ptr*_tmp2E2;_LL2D: _tmp2E2=_tmp2E1->f2;_LL2E:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp2E2);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp2E2))name=0;else{
name=_tmp2E2;}
goto _LL6;}case 5U: _LLB: _tmp2F5=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2DC)->f1;_LLC: {
# 1730
struct _tuple1*_tmp2E3=_tmp2F5->name;struct _tuple1*_tmp2E4=_tmp2E3;struct _dyneither_ptr*_tmp2E5;_LL30: _tmp2E5=_tmp2E4->f2;_LL31:;
name=_tmp2E5;
goto _LL6;}case 7U: _LLD: _tmp2F6=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2DC)->f1;_LLE: {
# 1734
struct _tuple1*_tmp2E6=_tmp2F6->name;struct _tuple1*_tmp2E7=_tmp2E6;struct _dyneither_ptr*_tmp2EF;_LL33: _tmp2EF=_tmp2E7->f2;_LL34:;
name=_tmp2EF;
# 1738
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2D6,name))
_tmp2D7=({struct Cyc_List_List*_tmp2E8=_cycalloc(sizeof(*_tmp2E8));_tmp2E8->hd=_tmp2DA,_tmp2E8->tl=_tmp2D7;_tmp2E8;});else{
# 1741
if((unsigned int)_tmp2F6->fields){
struct Cyc_List_List*_tmp2E9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2F6->fields))->v;for(0;_tmp2E9 != 0;_tmp2E9=_tmp2E9->tl){
struct Cyc_Absyn_Enumfield*_tmp2EA=(struct Cyc_Absyn_Enumfield*)_tmp2E9->hd;
struct _tuple1*_tmp2EB=_tmp2EA->name;struct _tuple1*_tmp2EC=_tmp2EB;struct _dyneither_ptr*_tmp2EE;_LL36: _tmp2EE=_tmp2EC->f2;_LL37:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2D6,_tmp2EE)){
_tmp2D7=({struct Cyc_List_List*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->hd=_tmp2DA,_tmp2ED->tl=_tmp2D7;_tmp2ED;});
break;}}}}
# 1751
name=0;
goto _LL6;}case 8U: _LLF: _tmp2F7=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2DC)->f1;_LL10: {
# 1754
struct _tuple1*_tmp2F0=_tmp2F7->name;struct _tuple1*_tmp2F1=_tmp2F0;struct _dyneither_ptr*_tmp2F2;_LL39: _tmp2F2=_tmp2F1->f2;_LL3A:;
name=_tmp2F2;
goto _LL6;}case 13U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 15U: _LL15: _LL16:
 goto _LL18;case 16U: _LL17: _LL18:
 goto _LL1A;case 2U: _LL19: _LL1A:
 goto _LL1C;case 6U: _LL1B: _LL1C:
 goto _LL1E;case 3U: _LL1D: _LL1E:
 goto _LL20;case 9U: _LL1F: _LL20:
 goto _LL22;case 10U: _LL21: _LL22:
 goto _LL24;case 11U: _LL23: _LL24:
 goto _LL26;case 12U: _LL25: _LL26:
 goto _LL28;default: _LL27: _LL28:
# 1769
 name=0;
goto _LL6;}_LL6:;}
# 1773
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2D6,name)){
if(({struct _dyneither_ptr _tmp65C=(struct _dyneither_ptr)*name;struct _dyneither_ptr _tmp65B=(struct _dyneither_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp65C,_tmp65B,Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix));})!= 0)
_tmp2D7=({struct Cyc_List_List*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->hd=_tmp2DA,_tmp2F8->tl=_tmp2D7;_tmp2F8;});else{
# 1778
Cyc_rename_decl(_tmp2DA);
_tmp2D8=({struct Cyc_List_List*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->hd=_tmp2DA,_tmp2F9->tl=_tmp2D8;_tmp2F9;});}}}}
# 1785
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp2FA=1;_npop_handler(0U);return _tmp2FA;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp371=({struct Cyc_String_pa_PrintArg_struct _tmp4B8;_tmp4B8.tag=0U,({struct _dyneither_ptr _tmp65D=(struct _dyneither_ptr)({const char*_tmp372=filename;_tag_dyneither(_tmp372,sizeof(char),_get_zero_arr_size_char((void*)_tmp372,1U));});_tmp4B8.f1=_tmp65D;});_tmp4B8;});void*_tmp36F[1U];_tmp36F[0]=& _tmp371;({struct _dyneither_ptr _tmp65E=({const char*_tmp370="_%s_";_tag_dyneither(_tmp370,sizeof(char),5U);});Cyc_aprintf(_tmp65E,_tag_dyneither(_tmp36F,sizeof(void*),1U));});});
{int _tmp2FB=0;for(0;(unsigned int)_tmp2FB < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp2FB){
if((int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp2FB))== (int)'.'  || (int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp2FB))== (int)'/')
({struct _dyneither_ptr _tmp2FC=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp2FB);char _tmp2FD=*((char*)_check_dyneither_subscript(_tmp2FC,sizeof(char),0U));char _tmp2FE='_';if(_get_dyneither_size(_tmp2FC,sizeof(char))== 1U  && (_tmp2FD == 0  && _tmp2FE != 0))_throw_arraybounds();*((char*)_tmp2FC.curr)=_tmp2FE;});else{
if((int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp2FB))!= (int)'_'  && (int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp2FB))!= (int)'/')
({struct _dyneither_ptr _tmp2FF=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp2FB);char _tmp300=*((char*)_check_dyneither_subscript(_tmp2FF,sizeof(char),0U));char _tmp301=(char) toupper((int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp2FB)));if(_get_dyneither_size(_tmp2FF,sizeof(char))== 1U  && (_tmp300 == 0  && _tmp301 != 0))_throw_arraybounds();*((char*)_tmp2FF.curr)=_tmp301;});}}}
# 1797
({struct Cyc_String_pa_PrintArg_struct _tmp304=({struct Cyc_String_pa_PrintArg_struct _tmp4AE;_tmp4AE.tag=0U,_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4AE;});struct Cyc_String_pa_PrintArg_struct _tmp305=({struct Cyc_String_pa_PrintArg_struct _tmp4AD;_tmp4AD.tag=0U,_tmp4AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4AD;});void*_tmp302[2U];_tmp302[0]=& _tmp304,_tmp302[1]=& _tmp305;({struct Cyc___cycFILE*_tmp660=out_file;struct _dyneither_ptr _tmp65F=({const char*_tmp303="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp303,sizeof(char),23U);});Cyc_fprintf(_tmp660,_tmp65F,_tag_dyneither(_tmp302,sizeof(void*),2U));});});{
# 1804
struct Cyc_List_List*_tmp306=0;
struct Cyc_List_List*_tmp307=0;
{struct Cyc_List_List*_tmp308=_tmp2D7;for(0;_tmp308 != 0;_tmp308=_tmp308->tl){
struct Cyc_Absyn_Decl*_tmp309=(struct Cyc_Absyn_Decl*)_tmp308->hd;
int _tmp30A=0;
struct _dyneither_ptr*name;
{void*_tmp30B=_tmp309->r;void*_tmp30C=_tmp30B;struct Cyc_Absyn_Typedefdecl*_tmp320;struct Cyc_Absyn_Enumdecl*_tmp31F;struct Cyc_Absyn_Aggrdecl*_tmp31E;struct Cyc_Absyn_Fndecl*_tmp31D;struct Cyc_Absyn_Vardecl*_tmp31C;switch(*((int*)_tmp30C)){case 0U: _LL3C: _tmp31C=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp30C)->f1;_LL3D: {
# 1812
struct _tuple1*_tmp30D=_tmp31C->name;struct _tuple1*_tmp30E=_tmp30D;struct _dyneither_ptr*_tmp30F;_LL5F: _tmp30F=_tmp30E->f2;_LL60:;
name=_tmp30F;
goto _LL3B;}case 1U: _LL3E: _tmp31D=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp30C)->f1;_LL3F:
# 1816
 if(_tmp31D->is_inline){name=0;goto _LL3B;}{
struct _tuple1*_tmp310=_tmp31D->name;struct _tuple1*_tmp311=_tmp310;struct _dyneither_ptr*_tmp312;_LL62: _tmp312=_tmp311->f2;_LL63:;
name=_tmp312;
goto _LL3B;};case 5U: _LL40: _tmp31E=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp30C)->f1;_LL41: {
# 1821
struct _tuple1*_tmp313=_tmp31E->name;struct _tuple1*_tmp314=_tmp313;struct _dyneither_ptr*_tmp315;_LL65: _tmp315=_tmp314->f2;_LL66:;
name=_tmp315;
goto _LL3B;}case 7U: _LL42: _tmp31F=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp30C)->f1;_LL43: {
# 1825
struct _tuple1*_tmp316=_tmp31F->name;struct _tuple1*_tmp317=_tmp316;struct _dyneither_ptr*_tmp318;_LL68: _tmp318=_tmp317->f2;_LL69:;
name=_tmp318;
goto _LL3B;}case 8U: _LL44: _tmp320=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp30C)->f1;_LL45: {
# 1829
struct _tuple1*_tmp319=_tmp320->name;struct _tuple1*_tmp31A=_tmp319;struct _dyneither_ptr*_tmp31B;_LL6B: _tmp31B=_tmp31A->f2;_LL6C:;
name=_tmp31B;
goto _LL3B;}case 4U: _LL46: _LL47:
 goto _LL49;case 13U: _LL48: _LL49:
 goto _LL4B;case 14U: _LL4A: _LL4B:
 goto _LL4D;case 15U: _LL4C: _LL4D:
 goto _LL4F;case 16U: _LL4E: _LL4F:
 goto _LL51;case 2U: _LL50: _LL51:
 goto _LL53;case 6U: _LL52: _LL53:
 goto _LL55;case 3U: _LL54: _LL55:
 goto _LL57;case 9U: _LL56: _LL57:
 goto _LL59;case 10U: _LL58: _LL59:
 goto _LL5B;case 11U: _LL5A: _LL5B:
 goto _LL5D;default: _LL5C: _LL5D:
# 1844
 name=0;
goto _LL3B;}_LL3B:;}
# 1847
if(!((unsigned int)name) && !_tmp30A)continue;
# 1852
if(({struct Cyc_Set_Set*_tmp661=_tmp2D6;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp661,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp321=Cyc_named_decl(_tmp2D8,name);
Cyc_merge_rep_decl(_tmp321,_tmp309);
_tmp306=({struct Cyc_List_List*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->hd=_tmp309,_tmp322->tl=_tmp306;_tmp322;});}else{
# 1858
_tmp306=({struct Cyc_List_List*_tmp323=_cycalloc(sizeof(*_tmp323));_tmp323->hd=_tmp309,_tmp323->tl=_tmp306;_tmp323;});}
_tmp307=({struct Cyc_List_List*_tmp324=_cycalloc(sizeof(*_tmp324));_tmp324->hd=name,_tmp324->tl=_tmp307;_tmp324;});}}
# 1863
{struct _handler_cons _tmp325;_push_handler(& _tmp325);{int _tmp327=0;if(setjmp(_tmp325.handler))_tmp327=1;if(!_tmp327){
Cyc_Binding_resolve_all(_tmp306);
({struct Cyc_Tcenv_Tenv*_tmp662=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp662,1,_tmp306);});
# 1864
;_pop_handler();}else{void*_tmp326=(void*)_exn_thrown;void*_tmp328=_tmp326;_LL6E: _LL6F:
# 1867
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp663=({const char*_tmp329="can't typecheck acquired declarations";_tag_dyneither(_tmp329,sizeof(char),38U);});_tmp32A->f1=_tmp663;});_tmp32A;}));_LL6D:;}};}
# 1871
{struct _tuple0 _tmp32B=({struct _tuple0 _tmp4B2;_tmp4B2.f1=_tmp306,_tmp4B2.f2=_tmp307;_tmp4B2;});struct _tuple0 _tmp32C=_tmp32B;struct Cyc_List_List*_tmp33E;struct Cyc_List_List*_tmp33D;_LL73: _tmp33E=_tmp32C.f1;_tmp33D=_tmp32C.f2;_LL74:;for(0;
_tmp33E != 0  && _tmp33D != 0;(_tmp33E=_tmp33E->tl,_tmp33D=_tmp33D->tl)){
struct Cyc_Absyn_Decl*_tmp32D=(struct Cyc_Absyn_Decl*)_tmp33E->hd;
struct _dyneither_ptr*_tmp32E=(struct _dyneither_ptr*)_tmp33D->hd;
int _tmp32F=0;
if(!((unsigned int)_tmp32E))
_tmp32F=1;
# 1880
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp32E){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp332=({struct Cyc_String_pa_PrintArg_struct _tmp4AF;_tmp4AF.tag=0U,_tmp4AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp32E);_tmp4AF;});void*_tmp330[1U];_tmp330[0]=& _tmp332;({struct _dyneither_ptr _tmp664=({const char*_tmp331="_%s_def_";_tag_dyneither(_tmp331,sizeof(char),9U);});Cyc_aprintf(_tmp664,_tag_dyneither(_tmp330,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp335=({struct Cyc_String_pa_PrintArg_struct _tmp4B0;_tmp4B0.tag=0U,_tmp4B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4B0;});void*_tmp333[1U];_tmp333[0]=& _tmp335;({struct Cyc___cycFILE*_tmp666=out_file;struct _dyneither_ptr _tmp665=({const char*_tmp334="#ifndef %s\n";_tag_dyneither(_tmp334,sizeof(char),12U);});Cyc_fprintf(_tmp666,_tmp665,_tag_dyneither(_tmp333,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp338=({struct Cyc_String_pa_PrintArg_struct _tmp4B1;_tmp4B1.tag=0U,_tmp4B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp4B1;});void*_tmp336[1U];_tmp336[0]=& _tmp338;({struct Cyc___cycFILE*_tmp668=out_file;struct _dyneither_ptr _tmp667=({const char*_tmp337="#define %s\n";_tag_dyneither(_tmp337,sizeof(char),12U);});Cyc_fprintf(_tmp668,_tmp667,_tag_dyneither(_tmp336,sizeof(void*),1U));});});
# 1886
({struct Cyc_List_List*_tmp669=({struct Cyc_Absyn_Decl*_tmp339[1U];_tmp339[0]=_tmp32D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp339,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp669,out_file);});
({void*_tmp33A=0U;({struct Cyc___cycFILE*_tmp66B=out_file;struct _dyneither_ptr _tmp66A=({const char*_tmp33B="#endif\n";_tag_dyneither(_tmp33B,sizeof(char),8U);});Cyc_fprintf(_tmp66B,_tmp66A,_tag_dyneither(_tmp33A,sizeof(void*),0U));});});}else{
# 1891
({struct Cyc_List_List*_tmp66C=({struct Cyc_Absyn_Decl*_tmp33C[1U];_tmp33C[0]=_tmp32D;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33C,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp66C,out_file);});}}}
# 1896
maybe=Cyc_fopen(_tmp264,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp343=_cycalloc(sizeof(*_tmp343));_tmp343->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp66F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp4B3;_tmp4B3.tag=0U,({struct _dyneither_ptr _tmp66D=(struct _dyneither_ptr)({const char*_tmp342=_tmp264;_tag_dyneither(_tmp342,sizeof(char),_get_zero_arr_size_char((void*)_tmp342,1U));});_tmp4B3.f1=_tmp66D;});_tmp4B3;});void*_tmp33F[1U];_tmp33F[0]=& _tmp341;({struct _dyneither_ptr _tmp66E=({const char*_tmp340="can't open macrosfile %s";_tag_dyneither(_tmp340,sizeof(char),25U);});Cyc_aprintf(_tmp66E,_tag_dyneither(_tmp33F,sizeof(void*),1U));});});_tmp343->f1=_tmp66F;});_tmp343;}));
# 1899
in_file=maybe;
_tmp2BF=Cyc_Lexing_from_file(in_file);{
struct _tuple22*entry2;
while((entry2=((struct _tuple22*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp2BF))!= 0){
struct _tuple22*_tmp344=(struct _tuple22*)_check_null(entry2);struct _tuple22*_tmp345=_tmp344;struct _dyneither_ptr _tmp34F;struct _dyneither_ptr*_tmp34E;_LL76: _tmp34F=_tmp345->f1;_tmp34E=_tmp345->f2;_LL77:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp2D6,_tmp34E)){
({struct Cyc_String_pa_PrintArg_struct _tmp348=({struct Cyc_String_pa_PrintArg_struct _tmp4B4;_tmp4B4.tag=0U,_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp34E);_tmp4B4;});void*_tmp346[1U];_tmp346[0]=& _tmp348;({struct Cyc___cycFILE*_tmp671=out_file;struct _dyneither_ptr _tmp670=({const char*_tmp347="#ifndef %s\n";_tag_dyneither(_tmp347,sizeof(char),12U);});Cyc_fprintf(_tmp671,_tmp670,_tag_dyneither(_tmp346,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp34B=({struct Cyc_String_pa_PrintArg_struct _tmp4B5;_tmp4B5.tag=0U,_tmp4B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34F);_tmp4B5;});void*_tmp349[1U];_tmp349[0]=& _tmp34B;({struct Cyc___cycFILE*_tmp673=out_file;struct _dyneither_ptr _tmp672=({const char*_tmp34A="%s\n";_tag_dyneither(_tmp34A,sizeof(char),4U);});Cyc_fprintf(_tmp673,_tmp672,_tag_dyneither(_tmp349,sizeof(void*),1U));});});
({void*_tmp34C=0U;({struct Cyc___cycFILE*_tmp675=out_file;struct _dyneither_ptr _tmp674=({const char*_tmp34D="#endif\n";_tag_dyneither(_tmp34D,sizeof(char),8U);});Cyc_fprintf(_tmp675,_tmp674,_tag_dyneither(_tmp34C,sizeof(void*),0U));});});}}
# 1910
Cyc_fclose(in_file);
if((int)Cyc_mode != (int)Cyc_FINISH);
# 1913
if(hstubs != 0){
struct Cyc_List_List*_tmp350=hstubs;for(0;_tmp350 != 0;_tmp350=_tmp350->tl){
struct _tuple23*_tmp351=(struct _tuple23*)_tmp350->hd;struct _tuple23*_tmp352=_tmp351;struct _dyneither_ptr _tmp358;struct _dyneither_ptr _tmp357;_LL79: _tmp358=_tmp352->f1;_tmp357=_tmp352->f2;_LL7A:;
if(({char*_tmp678=(char*)_tmp357.curr;_tmp678 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp677=(char*)_tmp358.curr;_tmp677 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp676=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp676,({struct _dyneither_ptr*_tmp353=_cycalloc(sizeof(*_tmp353));*_tmp353=_tmp358;_tmp353;}));})))
# 1919
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp357,sizeof(char),1U)),out_file);else{
# 1921
({struct Cyc_String_pa_PrintArg_struct _tmp356=({struct Cyc_String_pa_PrintArg_struct _tmp4B6;_tmp4B6.tag=0U,_tmp4B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp358);_tmp4B6;});void*_tmp354[1U];_tmp354[0]=& _tmp356;({struct _dyneither_ptr _tmp679=({const char*_tmp355="%s is not supported on this platform\n";_tag_dyneither(_tmp355,sizeof(char),38U);});Cyc_log(_tmp679,_tag_dyneither(_tmp354,sizeof(void*),1U));});});}}}
# 1924
({void*_tmp359=0U;({struct Cyc___cycFILE*_tmp67B=out_file;struct _dyneither_ptr _tmp67A=({const char*_tmp35A="#endif\n";_tag_dyneither(_tmp35A,sizeof(char),8U);});Cyc_fprintf(_tmp67B,_tmp67A,_tag_dyneither(_tmp359,sizeof(void*),0U));});});
if(Cyc_do_setjmp){int _tmp35B=0;_npop_handler(0U);return _tmp35B;}else{
Cyc_fclose(out_file);}
# 1929
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp35C=cstubs;for(0;_tmp35C != 0;_tmp35C=_tmp35C->tl){
struct _tuple23*_tmp35D=(struct _tuple23*)_tmp35C->hd;struct _tuple23*_tmp35E=_tmp35D;struct _dyneither_ptr _tmp361;struct _dyneither_ptr _tmp360;_LL7C: _tmp361=_tmp35E->f1;_tmp360=_tmp35E->f2;_LL7D:;
if(({char*_tmp67E=(char*)_tmp360.curr;_tmp67E != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp67D=(char*)_tmp361.curr;_tmp67D == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp67C=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp67C,({struct _dyneither_ptr*_tmp35F=_cycalloc(sizeof(*_tmp35F));*_tmp35F=_tmp361;_tmp35F;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp360,sizeof(char),1U)),out_file);}};}
# 1940
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1942
({struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp4B7;_tmp4B7.tag=0U,({struct _dyneither_ptr _tmp67F=(struct _dyneither_ptr)({const char*_tmp365=filename;_tag_dyneither(_tmp365,sizeof(char),_get_zero_arr_size_char((void*)_tmp365,1U));});_tmp4B7.f1=_tmp67F;});_tmp4B7;});void*_tmp362[1U];_tmp362[0]=& _tmp364;({struct Cyc___cycFILE*_tmp681=out_file;struct _dyneither_ptr _tmp680=({const char*_tmp363="#include <%s>\n\n";_tag_dyneither(_tmp363,sizeof(char),16U);});Cyc_fprintf(_tmp681,_tmp680,_tag_dyneither(_tmp362,sizeof(void*),1U));});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp366=cycstubs;for(0;_tmp366 != 0;_tmp366=_tmp366->tl){
struct _tuple23*_tmp367=(struct _tuple23*)_tmp366->hd;struct _tuple23*_tmp368=_tmp367;struct _dyneither_ptr _tmp36B;struct _dyneither_ptr _tmp36A;_LL7F: _tmp36B=_tmp368->f1;_tmp36A=_tmp368->f2;_LL80:;
if(({char*_tmp684=(char*)_tmp36A.curr;_tmp684 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp683=(char*)_tmp36B.curr;_tmp683 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp682=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp682,({struct _dyneither_ptr*_tmp369=_cycalloc(sizeof(*_tmp369));*_tmp369=_tmp36B;_tmp369;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp36A,sizeof(char),1U)),out_file);}}
# 1951
({void*_tmp36C=0U;({struct Cyc___cycFILE*_tmp686=out_file;struct _dyneither_ptr _tmp685=({const char*_tmp36D="\n";_tag_dyneither(_tmp36D,sizeof(char),2U);});Cyc_fprintf(_tmp686,_tmp685,_tag_dyneither(_tmp36C,sizeof(void*),0U));});});}{
# 1954
int _tmp36E=0;_npop_handler(0U);return _tmp36E;};};};};};};};};};
# 1598
;_pop_handler();}else{void*_tmp268=(void*)_exn_thrown;void*_tmp373=_tmp268;void*_tmp38A;struct _dyneither_ptr _tmp389;struct _dyneither_ptr _tmp388;struct _dyneither_ptr _tmp387;struct _dyneither_ptr _tmp386;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp373)->tag == Cyc_Core_Impossible){_LL82: _tmp386=((struct Cyc_Core_Impossible_exn_struct*)_tmp373)->f1;_LL83:
# 1958
({struct Cyc_String_pa_PrintArg_struct _tmp376=({struct Cyc_String_pa_PrintArg_struct _tmp4B9;_tmp4B9.tag=0U,_tmp4B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp386);_tmp4B9;});void*_tmp374[1U];_tmp374[0]=& _tmp376;({struct Cyc___cycFILE*_tmp688=Cyc_stderr;struct _dyneither_ptr _tmp687=({const char*_tmp375="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp375,sizeof(char),26U);});Cyc_fprintf(_tmp688,_tmp687,_tag_dyneither(_tmp374,sizeof(void*),1U));});});goto _LL81;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp373)->tag == Cyc_Dict_Absent){_LL84: _LL85:
# 1960
({void*_tmp377=0U;({struct Cyc___cycFILE*_tmp68A=Cyc_stderr;struct _dyneither_ptr _tmp689=({const char*_tmp378="Got Dict::Absent\n";_tag_dyneither(_tmp378,sizeof(char),18U);});Cyc_fprintf(_tmp68A,_tmp689,_tag_dyneither(_tmp377,sizeof(void*),0U));});});goto _LL81;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp373)->tag == Cyc_Core_Failure){_LL86: _tmp387=((struct Cyc_Core_Failure_exn_struct*)_tmp373)->f1;_LL87:
# 1962
({struct Cyc_String_pa_PrintArg_struct _tmp37B=({struct Cyc_String_pa_PrintArg_struct _tmp4BA;_tmp4BA.tag=0U,_tmp4BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp387);_tmp4BA;});void*_tmp379[1U];_tmp379[0]=& _tmp37B;({struct Cyc___cycFILE*_tmp68C=Cyc_stderr;struct _dyneither_ptr _tmp68B=({const char*_tmp37A="Got Core::Failure(%s)\n";_tag_dyneither(_tmp37A,sizeof(char),23U);});Cyc_fprintf(_tmp68C,_tmp68B,_tag_dyneither(_tmp379,sizeof(void*),1U));});});goto _LL81;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp373)->tag == Cyc_Core_Invalid_argument){_LL88: _tmp388=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp373)->f1;_LL89:
# 1964
({struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp4BB;_tmp4BB.tag=0U,_tmp4BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp388);_tmp4BB;});void*_tmp37C[1U];_tmp37C[0]=& _tmp37E;({struct Cyc___cycFILE*_tmp68E=Cyc_stderr;struct _dyneither_ptr _tmp68D=({const char*_tmp37D="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp37D,sizeof(char),26U);});Cyc_fprintf(_tmp68E,_tmp68D,_tag_dyneither(_tmp37C,sizeof(void*),1U));});});goto _LL81;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp373)->tag == Cyc_Core_Not_found){_LL8A: _LL8B:
# 1966
({void*_tmp37F=0U;({struct Cyc___cycFILE*_tmp690=Cyc_stderr;struct _dyneither_ptr _tmp68F=({const char*_tmp380="Got Not_found\n";_tag_dyneither(_tmp380,sizeof(char),15U);});Cyc_fprintf(_tmp690,_tmp68F,_tag_dyneither(_tmp37F,sizeof(void*),0U));});});goto _LL81;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp373)->tag == Cyc_NO_SUPPORT){_LL8C: _tmp389=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp373)->f1;_LL8D:
# 1968
({struct Cyc_String_pa_PrintArg_struct _tmp383=({struct Cyc_String_pa_PrintArg_struct _tmp4BC;_tmp4BC.tag=0U,_tmp4BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp389);_tmp4BC;});void*_tmp381[1U];_tmp381[0]=& _tmp383;({struct Cyc___cycFILE*_tmp692=Cyc_stderr;struct _dyneither_ptr _tmp691=({const char*_tmp382="No support because %s\n";_tag_dyneither(_tmp382,sizeof(char),23U);});Cyc_fprintf(_tmp692,_tmp691,_tag_dyneither(_tmp381,sizeof(void*),1U));});});goto _LL81;}else{_LL8E: _tmp38A=_tmp373;_LL8F:
# 1970
({void*_tmp384=0U;({struct Cyc___cycFILE*_tmp694=Cyc_stderr;struct _dyneither_ptr _tmp693=({const char*_tmp385="Got unknown exception\n";_tag_dyneither(_tmp385,sizeof(char),23U);});Cyc_fprintf(_tmp694,_tmp693,_tag_dyneither(_tmp384,sizeof(void*),0U));});});
Cyc_Core_rethrow(_tmp38A);}}}}}}_LL81:;}};}
# 1976
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp38D=({struct Cyc_String_pa_PrintArg_struct _tmp4BD;_tmp4BD.tag=0U,({struct _dyneither_ptr _tmp695=(struct _dyneither_ptr)({const char*_tmp38E=filename;_tag_dyneither(_tmp38E,sizeof(char),_get_zero_arr_size_char((void*)_tmp38E,1U));});_tmp4BD.f1=_tmp695;});_tmp4BD;});void*_tmp38B[1U];_tmp38B[0]=& _tmp38D;({struct Cyc___cycFILE*_tmp697=Cyc_stderr;struct _dyneither_ptr _tmp696=({const char*_tmp38C="Error: could not create file %s\n";_tag_dyneither(_tmp38C,sizeof(char),33U);});Cyc_fprintf(_tmp697,_tmp696,_tag_dyneither(_tmp38B,sizeof(void*),1U));});});
return 1;}
# 1981
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp391=({struct Cyc_String_pa_PrintArg_struct _tmp4BE;_tmp4BE.tag=0U,({
# 1984
struct _dyneither_ptr _tmp698=(struct _dyneither_ptr)({const char*_tmp392=filename;_tag_dyneither(_tmp392,sizeof(char),_get_zero_arr_size_char((void*)_tmp392,1U));});_tmp4BE.f1=_tmp698;});_tmp4BE;});void*_tmp38F[1U];_tmp38F[0]=& _tmp391;({struct Cyc___cycFILE*_tmp69A=out_file;struct _dyneither_ptr _tmp699=({const char*_tmp390="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp390,sizeof(char),48U);});Cyc_fprintf(_tmp69A,_tmp699,_tag_dyneither(_tmp38F,sizeof(void*),1U));});});
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp4BF;_tmp4BF.tag=0U,({
struct _dyneither_ptr _tmp69B=(struct _dyneither_ptr)({const char*_tmp396=filename;_tag_dyneither(_tmp396,sizeof(char),_get_zero_arr_size_char((void*)_tmp396,1U));});_tmp4BF.f1=_tmp69B;});_tmp4BF;});void*_tmp393[1U];_tmp393[0]=& _tmp395;({struct Cyc___cycFILE*_tmp69D=Cyc_stderr;struct _dyneither_ptr _tmp69C=({const char*_tmp394="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp394,sizeof(char),52U);});Cyc_fprintf(_tmp69D,_tmp69C,_tag_dyneither(_tmp393,sizeof(void*),1U));});});
({void*_tmp397=0U;({struct _dyneither_ptr _tmp69E=({const char*_tmp398="Not supported on this platform\n";_tag_dyneither(_tmp398,sizeof(char),32U);});Cyc_log(_tmp69E,_tag_dyneither(_tmp397,sizeof(void*),0U));});});
# 1995
return 0;};}
# 1999
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3B0=({struct Cyc_String_pa_PrintArg_struct _tmp4C6;_tmp4C6.tag=0U,({struct _dyneither_ptr _tmp69F=(struct _dyneither_ptr)({const char*_tmp3B1=file;_tag_dyneither(_tmp3B1,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B1,1U));});_tmp4C6.f1=_tmp69F;});_tmp4C6;});void*_tmp3AE[1U];_tmp3AE[0]=& _tmp3B0;({struct Cyc___cycFILE*_tmp6A1=Cyc_stderr;struct _dyneither_ptr _tmp6A0=({const char*_tmp3AF="Processing %s\n";_tag_dyneither(_tmp3AF,sizeof(char),15U);});Cyc_fprintf(_tmp6A1,_tmp6A0,_tag_dyneither(_tmp3AE,sizeof(void*),1U));});});{
struct Cyc___cycFILE*_tmp3B2=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp3B2)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B5=({struct Cyc_String_pa_PrintArg_struct _tmp4C7;_tmp4C7.tag=0U,({struct _dyneither_ptr _tmp6A2=(struct _dyneither_ptr)({const char*_tmp3B6=file;_tag_dyneither(_tmp3B6,sizeof(char),_get_zero_arr_size_char((void*)_tmp3B6,1U));});_tmp4C7.f1=_tmp6A2;});_tmp4C7;});void*_tmp3B3[1U];_tmp3B3[0]=& _tmp3B5;({struct Cyc___cycFILE*_tmp6A4=Cyc_stderr;struct _dyneither_ptr _tmp6A3=({const char*_tmp3B4="Error: could not open %s\n";_tag_dyneither(_tmp3B4,sizeof(char),26U);});Cyc_fprintf(_tmp6A4,_tmp6A3,_tag_dyneither(_tmp3B3,sizeof(void*),1U));});});
return 1;}{
# 2007
struct Cyc___cycFILE*_tmp3B7=_tmp3B2;
# 2011
struct _dyneither_ptr buf=({char*_tmp3D5=({unsigned int _tmp3D4=1024U + 1U;char*_tmp3D3=_cycalloc_atomic(_check_times(_tmp3D4,sizeof(char)));({{unsigned int _tmp4CD=1024U;unsigned int i;for(i=0;i < _tmp4CD;++ i){_tmp3D3[i]='\000';}_tmp3D3[_tmp4CD]=0;}0;});_tmp3D3;});_tag_dyneither(_tmp3D5,sizeof(char),1025U);});
struct _dyneither_ptr _tmp3B8=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp3BB=({struct Cyc_String_pa_PrintArg_struct _tmp4C8;_tmp4C8.tag=0U,({struct _dyneither_ptr _tmp6A5=(struct _dyneither_ptr)({const char*_tmp3BC=dir;_tag_dyneither(_tmp3BC,sizeof(char),_get_zero_arr_size_char((void*)_tmp3BC,1U));});_tmp4C8.f1=_tmp6A5;});_tmp4C8;});void*_tmp3B9[1U];_tmp3B9[0]=& _tmp3BB;({struct Cyc___cycFILE*_tmp6A7=Cyc_stderr;struct _dyneither_ptr _tmp6A6=({const char*_tmp3BA="Error: can't change directory to %s\n";_tag_dyneither(_tmp3BA,sizeof(char),37U);});Cyc_fprintf(_tmp6A7,_tmp6A6,_tag_dyneither(_tmp3B9,sizeof(void*),1U));});});
return 1;}}
# 2019
if((int)Cyc_mode == (int)Cyc_GATHER){
# 2021
struct _dyneither_ptr _tmp3BD=({struct Cyc_String_pa_PrintArg_struct _tmp3C3=({struct Cyc_String_pa_PrintArg_struct _tmp4CB;_tmp4CB.tag=0U,_tmp4CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4CB;});struct Cyc_String_pa_PrintArg_struct _tmp3C4=({struct Cyc_String_pa_PrintArg_struct _tmp4CA;_tmp4CA.tag=0U,_tmp4CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4CA;});void*_tmp3C1[2U];_tmp3C1[0]=& _tmp3C3,_tmp3C1[1]=& _tmp3C4;({struct _dyneither_ptr _tmp6A8=({const char*_tmp3C2="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp3C2,sizeof(char),39U);});Cyc_aprintf(_tmp6A8,_tag_dyneither(_tmp3C1,sizeof(void*),2U));});});
# 2023
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp3C0=({struct Cyc_String_pa_PrintArg_struct _tmp4C9;_tmp4C9.tag=0U,_tmp4C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3BD);_tmp4C9;});void*_tmp3BE[1U];_tmp3BE[0]=& _tmp3C0;({struct Cyc___cycFILE*_tmp6AA=Cyc_stderr;struct _dyneither_ptr _tmp6A9=({const char*_tmp3BF="%s\n";_tag_dyneither(_tmp3BF,sizeof(char),4U);});Cyc_fprintf(_tmp6AA,_tmp6A9,_tag_dyneither(_tmp3BE,sizeof(void*),1U));});});
 system((const char*)_check_null(_untag_dyneither_ptr(_tmp3BD,sizeof(char),1U)));}{
# 2028
struct Cyc_Lexing_lexbuf*_tmp3C5=Cyc_Lexing_from_file(_tmp3B7);
struct _tuple25*entry;
while((entry=((struct _tuple25*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp3C5))!= 0){
struct _tuple25*_tmp3C6=(struct _tuple25*)_check_null(entry);struct _tuple25*_tmp3C7=_tmp3C6;struct _dyneither_ptr _tmp3CF;struct Cyc_List_List*_tmp3CE;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List*_tmp3CC;struct Cyc_List_List*_tmp3CB;struct Cyc_List_List*_tmp3CA;struct Cyc_List_List*_tmp3C9;struct Cyc_List_List*_tmp3C8;_LL1: _tmp3CF=_tmp3C7->f1;_tmp3CE=_tmp3C7->f2;_tmp3CD=_tmp3C7->f3;_tmp3CC=_tmp3C7->f4;_tmp3CB=_tmp3C7->f5;_tmp3CA=_tmp3C7->f6;_tmp3C9=_tmp3C7->f7;_tmp3C8=_tmp3C7->f8;_LL2:;
# 2033
if(Cyc_process_file((const char*)_check_null(_untag_dyneither_ptr(_tmp3CF,sizeof(char),1U)),_tmp3CE,_tmp3CD,_tmp3CC,_tmp3CB,_tmp3CA,_tmp3C9,_tmp3C8))
# 2035
return 1;}
# 2037
Cyc_fclose(_tmp3B7);
# 2039
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3B8,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp3D2=({struct Cyc_String_pa_PrintArg_struct _tmp4CC;_tmp4CC.tag=0U,_tmp4CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B8);_tmp4CC;});void*_tmp3D0[1U];_tmp3D0[0]=& _tmp3D2;({struct Cyc___cycFILE*_tmp6AC=Cyc_stderr;struct _dyneither_ptr _tmp6AB=({const char*_tmp3D1="Error: could not change directory to %s\n";_tag_dyneither(_tmp3D1,sizeof(char),41U);});Cyc_fprintf(_tmp6AC,_tmp6AB,_tag_dyneither(_tmp3D0,sizeof(void*),1U));});});
return 1;}}
# 2045
return 0;};};};}
# 2049
int Cyc_process_setjmp(const char*dir){
# 2052
struct _dyneither_ptr buf=({char*_tmp3E7=({unsigned int _tmp3E6=1024U + 1U;char*_tmp3E5=_cycalloc_atomic(_check_times(_tmp3E6,sizeof(char)));({{unsigned int _tmp4D0=1024U;unsigned int i;for(i=0;i < _tmp4D0;++ i){_tmp3E5[i]='\000';}_tmp3E5[_tmp4D0]=0;}0;});_tmp3E5;});_tag_dyneither(_tmp3E7,sizeof(char),1025U);});
struct _dyneither_ptr _tmp3D6=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp3D9=({struct Cyc_String_pa_PrintArg_struct _tmp4CE;_tmp4CE.tag=0U,({struct _dyneither_ptr _tmp6AD=(struct _dyneither_ptr)({const char*_tmp3DA=dir;_tag_dyneither(_tmp3DA,sizeof(char),_get_zero_arr_size_char((void*)_tmp3DA,1U));});_tmp4CE.f1=_tmp6AD;});_tmp4CE;});void*_tmp3D7[1U];_tmp3D7[0]=& _tmp3D9;({struct Cyc___cycFILE*_tmp6AF=Cyc_stderr;struct _dyneither_ptr _tmp6AE=({const char*_tmp3D8="Error: can't change directory to %s\n";_tag_dyneither(_tmp3D8,sizeof(char),37U);});Cyc_fprintf(_tmp6AF,_tmp6AE,_tag_dyneither(_tmp3D7,sizeof(void*),1U));});});
return 1;}
# 2058
if(({struct Cyc_List_List*_tmp6B5=({struct _dyneither_ptr*_tmp3DB[1U];({struct _dyneither_ptr*_tmp6B1=({struct _dyneither_ptr*_tmp3DD=_cycalloc(sizeof(*_tmp3DD));({struct _dyneither_ptr _tmp6B0=({const char*_tmp3DC="jmp_buf";_tag_dyneither(_tmp3DC,sizeof(char),8U);});*_tmp3DD=_tmp6B0;});_tmp3DD;});_tmp3DB[0]=_tmp6B1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3DB,sizeof(struct _dyneither_ptr*),1U));});Cyc_process_file("setjmp.h",_tmp6B5,0,0,({struct _tuple23*_tmp3DE[1U];({struct _tuple23*_tmp6B4=({struct _tuple23*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));
({struct _dyneither_ptr _tmp6B3=({const char*_tmp3DF="setjmp";_tag_dyneither(_tmp3DF,sizeof(char),7U);});_tmp3E1->f1=_tmp6B3;}),({struct _dyneither_ptr _tmp6B2=({const char*_tmp3E0="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp3E0,sizeof(char),29U);});_tmp3E1->f2=_tmp6B2;});_tmp3E1;});_tmp3DE[0]=_tmp6B4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3DE,sizeof(struct _tuple23*),1U));}),0,0,0);}))
# 2061
return 1;
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp3D6,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp3E4=({struct Cyc_String_pa_PrintArg_struct _tmp4CF;_tmp4CF.tag=0U,_tmp4CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3D6);_tmp4CF;});void*_tmp3E2[1U];_tmp3E2[0]=& _tmp3E4;({struct Cyc___cycFILE*_tmp6B7=Cyc_stderr;struct _dyneither_ptr _tmp6B6=({const char*_tmp3E3="Error: could not change directory to %s\n";_tag_dyneither(_tmp3E3,sizeof(char),41U);});Cyc_fprintf(_tmp6B7,_tmp6B6,_tag_dyneither(_tmp3E2,sizeof(void*),1U));});});
return 1;}
# 2066
return 0;}static char _tmp3E8[13U]="BUILDLIB.OUT";
# 2070
static struct _dyneither_ptr Cyc_output_dir={_tmp3E8,_tmp3E8,_tmp3E8 + 13U};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 2074
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));_tmp3E9->hd=(const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1U)),_tmp3E9->tl=Cyc_spec_files;_tmp3E9;});}
# 2078
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 2082
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 2085
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 2088
static void Cyc_add_cpparg(struct _dyneither_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));({struct _dyneither_ptr*_tmp6B8=({struct _dyneither_ptr*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));*_tmp3EA=s;_tmp3EA;});_tmp3EB->hd=_tmp6B8;}),_tmp3EB->tl=Cyc_cppargs;_tmp3EB;});}
# 2091
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp3EE=({struct Cyc_String_pa_PrintArg_struct _tmp4D1;_tmp4D1.tag=0U,_tmp4D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp4D1;});void*_tmp3EC[1U];_tmp3EC[0]=& _tmp3EE;({struct Cyc___cycFILE*_tmp6BA=Cyc_stderr;struct _dyneither_ptr _tmp6B9=({const char*_tmp3ED="Unsupported option %s\n";_tag_dyneither(_tmp3ED,sizeof(char),23U);});Cyc_fprintf(_tmp6BA,_tmp6B9,_tag_dyneither(_tmp3EC,sizeof(void*),1U));});});
Cyc_badparse=1;}
# 2100
void GC_blacklist_warn_clear();struct _tuple29{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
 GC_blacklist_warn_clear();{
# 2104
struct Cyc_List_List*options=({struct _tuple29*_tmp426[9U];({
struct _tuple29*_tmp6E7=({struct _tuple29*_tmp42B=_cycalloc(sizeof(*_tmp42B));({struct _dyneither_ptr _tmp6E6=({const char*_tmp427="-d";_tag_dyneither(_tmp427,sizeof(char),3U);});_tmp42B->f1=_tmp6E6;}),_tmp42B->f2=0,({struct _dyneither_ptr _tmp6E5=({const char*_tmp428=" <file>";_tag_dyneither(_tmp428,sizeof(char),8U);});_tmp42B->f3=_tmp6E5;}),({
void*_tmp6E4=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->tag=5U,_tmp429->f1=Cyc_set_output_dir;_tmp429;});_tmp42B->f4=_tmp6E4;}),({
struct _dyneither_ptr _tmp6E3=({const char*_tmp42A="Set the output directory to <file>";_tag_dyneither(_tmp42A,sizeof(char),35U);});_tmp42B->f5=_tmp6E3;});_tmp42B;});
# 2105
_tmp426[0]=_tmp6E7;}),({
# 2108
struct _tuple29*_tmp6E2=({struct _tuple29*_tmp430=_cycalloc(sizeof(*_tmp430));({struct _dyneither_ptr _tmp6E1=({const char*_tmp42C="-gather";_tag_dyneither(_tmp42C,sizeof(char),8U);});_tmp430->f1=_tmp6E1;}),_tmp430->f2=0,({struct _dyneither_ptr _tmp6E0=({const char*_tmp42D="";_tag_dyneither(_tmp42D,sizeof(char),1U);});_tmp430->f3=_tmp6E0;}),({
void*_tmp6DF=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->tag=0U,_tmp42E->f1=Cyc_set_GATHER;_tmp42E;});_tmp430->f4=_tmp6DF;}),({
struct _dyneither_ptr _tmp6DE=({const char*_tmp42F="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp42F,sizeof(char),56U);});_tmp430->f5=_tmp6DE;});_tmp430;});
# 2108
_tmp426[1]=_tmp6E2;}),({
# 2111
struct _tuple29*_tmp6DD=({struct _tuple29*_tmp435=_cycalloc(sizeof(*_tmp435));({struct _dyneither_ptr _tmp6DC=({const char*_tmp431="-gatherscript";_tag_dyneither(_tmp431,sizeof(char),14U);});_tmp435->f1=_tmp6DC;}),_tmp435->f2=0,({struct _dyneither_ptr _tmp6DB=({const char*_tmp432="";_tag_dyneither(_tmp432,sizeof(char),1U);});_tmp435->f3=_tmp6DB;}),({
void*_tmp6DA=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->tag=0U,_tmp433->f1=Cyc_set_GATHERSCRIPT;_tmp433;});_tmp435->f4=_tmp6DA;}),({
struct _dyneither_ptr _tmp6D9=({const char*_tmp434="Produce a script to gather C library info";_tag_dyneither(_tmp434,sizeof(char),42U);});_tmp435->f5=_tmp6D9;});_tmp435;});
# 2111
_tmp426[2]=_tmp6DD;}),({
# 2114
struct _tuple29*_tmp6D8=({struct _tuple29*_tmp43A=_cycalloc(sizeof(*_tmp43A));({struct _dyneither_ptr _tmp6D7=({const char*_tmp436="-finish";_tag_dyneither(_tmp436,sizeof(char),8U);});_tmp43A->f1=_tmp6D7;}),_tmp43A->f2=0,({struct _dyneither_ptr _tmp6D6=({const char*_tmp437="";_tag_dyneither(_tmp437,sizeof(char),1U);});_tmp43A->f3=_tmp6D6;}),({
void*_tmp6D5=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=0U,_tmp438->f1=Cyc_set_FINISH;_tmp438;});_tmp43A->f4=_tmp6D5;}),({
struct _dyneither_ptr _tmp6D4=({const char*_tmp439="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp439,sizeof(char),57U);});_tmp43A->f5=_tmp6D4;});_tmp43A;});
# 2114
_tmp426[3]=_tmp6D8;}),({
# 2117
struct _tuple29*_tmp6D3=({struct _tuple29*_tmp43F=_cycalloc(sizeof(*_tmp43F));({struct _dyneither_ptr _tmp6D2=({const char*_tmp43B="-setjmp";_tag_dyneither(_tmp43B,sizeof(char),8U);});_tmp43F->f1=_tmp6D2;}),_tmp43F->f2=0,({struct _dyneither_ptr _tmp6D1=({const char*_tmp43C="";_tag_dyneither(_tmp43C,sizeof(char),1U);});_tmp43F->f3=_tmp6D1;}),({
void*_tmp6D0=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->tag=3U,_tmp43D->f1=& Cyc_do_setjmp;_tmp43D;});_tmp43F->f4=_tmp6D0;}),({
# 2122
struct _dyneither_ptr _tmp6CF=({const char*_tmp43E="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp43E,sizeof(char),186U);});_tmp43F->f5=_tmp6CF;});_tmp43F;});
# 2117
_tmp426[4]=_tmp6D3;}),({
# 2123
struct _tuple29*_tmp6CE=({struct _tuple29*_tmp444=_cycalloc(sizeof(*_tmp444));({struct _dyneither_ptr _tmp6CD=({const char*_tmp440="-b";_tag_dyneither(_tmp440,sizeof(char),3U);});_tmp444->f1=_tmp6CD;}),_tmp444->f2=0,({struct _dyneither_ptr _tmp6CC=({const char*_tmp441=" <machine>";_tag_dyneither(_tmp441,sizeof(char),11U);});_tmp444->f3=_tmp6CC;}),({
void*_tmp6CB=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=5U,_tmp442->f1=Cyc_Specsfile_set_target_arch;_tmp442;});_tmp444->f4=_tmp6CB;}),({
struct _dyneither_ptr _tmp6CA=({const char*_tmp443="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp443,sizeof(char),52U);});_tmp444->f5=_tmp6CA;});_tmp444;});
# 2123
_tmp426[5]=_tmp6CE;}),({
# 2126
struct _tuple29*_tmp6C9=({struct _tuple29*_tmp449=_cycalloc(sizeof(*_tmp449));({struct _dyneither_ptr _tmp6C8=({const char*_tmp445="-B";_tag_dyneither(_tmp445,sizeof(char),3U);});_tmp449->f1=_tmp6C8;}),_tmp449->f2=1,({struct _dyneither_ptr _tmp6C7=({const char*_tmp446="<file>";_tag_dyneither(_tmp446,sizeof(char),7U);});_tmp449->f3=_tmp6C7;}),({
void*_tmp6C6=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=1U,_tmp447->f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp447;});_tmp449->f4=_tmp6C6;}),({
struct _dyneither_ptr _tmp6C5=({const char*_tmp448="Add to the list of directories to search for compiler files";_tag_dyneither(_tmp448,sizeof(char),60U);});_tmp449->f5=_tmp6C5;});_tmp449;});
# 2126
_tmp426[6]=_tmp6C9;}),({
# 2129
struct _tuple29*_tmp6C4=({struct _tuple29*_tmp44E=_cycalloc(sizeof(*_tmp44E));({struct _dyneither_ptr _tmp6C3=({const char*_tmp44A="-v";_tag_dyneither(_tmp44A,sizeof(char),3U);});_tmp44E->f1=_tmp6C3;}),_tmp44E->f2=0,({struct _dyneither_ptr _tmp6C2=({const char*_tmp44B="";_tag_dyneither(_tmp44B,sizeof(char),1U);});_tmp44E->f3=_tmp6C2;}),({
void*_tmp6C1=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->tag=3U,_tmp44C->f1=& Cyc_verbose;_tmp44C;});_tmp44E->f4=_tmp6C1;}),({
struct _dyneither_ptr _tmp6C0=({const char*_tmp44D="Verbose operation";_tag_dyneither(_tmp44D,sizeof(char),18U);});_tmp44E->f5=_tmp6C0;});_tmp44E;});
# 2129
_tmp426[7]=_tmp6C4;}),({
# 2132
struct _tuple29*_tmp6BF=({struct _tuple29*_tmp453=_cycalloc(sizeof(*_tmp453));({struct _dyneither_ptr _tmp6BE=({const char*_tmp44F="-";_tag_dyneither(_tmp44F,sizeof(char),2U);});_tmp453->f1=_tmp6BE;}),_tmp453->f2=1,({struct _dyneither_ptr _tmp6BD=({const char*_tmp450="";_tag_dyneither(_tmp450,sizeof(char),1U);});_tmp453->f3=_tmp6BD;}),({
void*_tmp6BC=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=1U,_tmp451->f1=Cyc_add_cpparg;_tmp451;});_tmp453->f4=_tmp6BC;}),({
struct _dyneither_ptr _tmp6BB=({const char*_tmp452="";_tag_dyneither(_tmp452,sizeof(char),1U);});_tmp453->f5=_tmp6BB;});_tmp453;});
# 2132
_tmp426[8]=_tmp6BF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp426,sizeof(struct _tuple29*),9U));});
# 2137
struct _dyneither_ptr _tmp3EF=({struct Cyc_List_List*_tmp6E9=options;struct _dyneither_ptr _tmp6E8=({const char*_tmp425="Options:";_tag_dyneither(_tmp425,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp6E9,Cyc_add_spec_file,Cyc_no_other,_tmp6E8,argv);});
# 2139
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp6EB=options;struct _dyneither_ptr _tmp6EA=({const char*_tmp3F0="Options:";_tag_dyneither(_tmp3F0,sizeof(char),9U);});Cyc_Arg_parse(_tmp6EB,Cyc_add_spec_file,Cyc_no_other,_tmp6EA,_tmp3EF);});
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && (int)Cyc_mode == (int)Cyc_GATHER) || 
Cyc_do_setjmp  && (int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_List_List*_tmp6EC=options;Cyc_Arg_usage(_tmp6EC,({const char*_tmp3F1="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp3F1,sizeof(char),59U);}));});
# 2149
return 1;}{
# 2155
struct _dyneither_ptr _tmp3F2=Cyc_Specsfile_find_in_arch_path(({const char*_tmp424="cycspecs";_tag_dyneither(_tmp424,sizeof(char),9U);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp3F5=({struct Cyc_String_pa_PrintArg_struct _tmp4D2;_tmp4D2.tag=0U,_tmp4D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3F2);_tmp4D2;});void*_tmp3F3[1U];_tmp3F3[0]=& _tmp3F5;({struct Cyc___cycFILE*_tmp6EE=Cyc_stderr;struct _dyneither_ptr _tmp6ED=({const char*_tmp3F4="Reading from specs file %s\n";_tag_dyneither(_tmp3F4,sizeof(char),28U);});Cyc_fprintf(_tmp6EE,_tmp6ED,_tag_dyneither(_tmp3F3,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp3F6=Cyc_Specsfile_read_specs(_tmp3F2);
Cyc_target_cflags=({struct Cyc_List_List*_tmp6EF=_tmp3F6;Cyc_Specsfile_get_spec(_tmp6EF,({const char*_tmp3F7="cyclone_target_cflags";_tag_dyneither(_tmp3F7,sizeof(char),22U);}));});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp3FA=({struct Cyc_String_pa_PrintArg_struct _tmp4D3;_tmp4D3.tag=0U,_tmp4D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4D3;});void*_tmp3F8[1U];_tmp3F8[0]=& _tmp3FA;({struct Cyc___cycFILE*_tmp6F1=Cyc_stderr;struct _dyneither_ptr _tmp6F0=({const char*_tmp3F9="Target cflags are %s\n";_tag_dyneither(_tmp3F9,sizeof(char),22U);});Cyc_fprintf(_tmp6F1,_tmp6F0,_tag_dyneither(_tmp3F8,sizeof(void*),1U));});});
Cyc_cyclone_cc=({struct Cyc_List_List*_tmp6F2=_tmp3F6;Cyc_Specsfile_get_spec(_tmp6F2,({const char*_tmp3FB="cyclone_cc";_tag_dyneither(_tmp3FB,sizeof(char),11U);}));});
if(!((unsigned int)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp3FC="gcc";_tag_dyneither(_tmp3FC,sizeof(char),4U);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp3FF=({struct Cyc_String_pa_PrintArg_struct _tmp4D4;_tmp4D4.tag=0U,_tmp4D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp4D4;});void*_tmp3FD[1U];_tmp3FD[0]=& _tmp3FF;({struct Cyc___cycFILE*_tmp6F4=Cyc_stderr;struct _dyneither_ptr _tmp6F3=({const char*_tmp3FE="C compiler is %s\n";_tag_dyneither(_tmp3FE,sizeof(char),18U);});Cyc_fprintf(_tmp6F4,_tmp6F3,_tag_dyneither(_tmp3FD,sizeof(void*),1U));});});
# 2164
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp400=0U;({struct Cyc___cycFILE*_tmp6F6=Cyc_stderr;struct _dyneither_ptr _tmp6F5=({const char*_tmp401="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp401,sizeof(char),22U);});Cyc_fprintf(_tmp6F6,_tmp6F5,_tag_dyneither(_tmp400,sizeof(void*),0U));});});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
({void*_tmp402=0U;({struct Cyc___cycFILE*_tmp6F8=Cyc_stderr;struct _dyneither_ptr _tmp6F7=({const char*_tmp403="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp403,sizeof(char),35U);});Cyc_fprintf(_tmp6F8,_tmp6F7,_tag_dyneither(_tmp402,sizeof(void*),0U));});});
 exit(1);}
# 2172
({void*_tmp404=0U;({struct _dyneither_ptr _tmp6F9=({const char*_tmp405="#!/bin/sh\n";_tag_dyneither(_tmp405,sizeof(char),11U);});Cyc_prscript(_tmp6F9,_tag_dyneither(_tmp404,sizeof(void*),0U));});});
({void*_tmp406=0U;({struct _dyneither_ptr _tmp6FA=({const char*_tmp407="GCC=\"gcc\"\n";_tag_dyneither(_tmp407,sizeof(char),11U);});Cyc_prscript(_tmp6FA,_tag_dyneither(_tmp406,sizeof(void*),0U));});});}
# 2177
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp40A=({struct Cyc_String_pa_PrintArg_struct _tmp4D5;_tmp4D5.tag=0U,_tmp4D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4D5;});void*_tmp408[1U];_tmp408[0]=& _tmp40A;({struct Cyc___cycFILE*_tmp6FC=Cyc_stderr;struct _dyneither_ptr _tmp6FB=({const char*_tmp409="Error: could not create directory %s\n";_tag_dyneither(_tmp409,sizeof(char),38U);});Cyc_fprintf(_tmp6FC,_tmp6FB,_tag_dyneither(_tmp408,sizeof(void*),1U));});});
return 1;}
# 2181
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp40D=({struct Cyc_String_pa_PrintArg_struct _tmp4D6;_tmp4D6.tag=0U,_tmp4D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4D6;});void*_tmp40B[1U];_tmp40B[0]=& _tmp40D;({struct Cyc___cycFILE*_tmp6FE=Cyc_stderr;struct _dyneither_ptr _tmp6FD=({const char*_tmp40C="Output directory is %s\n";_tag_dyneither(_tmp40C,sizeof(char),24U);});Cyc_fprintf(_tmp6FE,_tmp6FD,_tag_dyneither(_tmp40B,sizeof(void*),1U));});});
# 2184
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp410=({struct Cyc_String_pa_PrintArg_struct _tmp4D7;_tmp4D7.tag=0U,_tmp4D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4D7;});void*_tmp40E[1U];_tmp40E[0]=& _tmp410;({struct _dyneither_ptr _tmp6FF=({const char*_tmp40F="cd %s\n";_tag_dyneither(_tmp40F,sizeof(char),7U);});Cyc_prscript(_tmp6FF,_tag_dyneither(_tmp40E,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp413=({struct Cyc_String_pa_PrintArg_struct _tmp4D8;_tmp4D8.tag=0U,_tmp4D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp4D8;});void*_tmp411[1U];_tmp411[0]=& _tmp413;({struct _dyneither_ptr _tmp700=({const char*_tmp412="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp412,sizeof(char),41U);});Cyc_prscript(_tmp700,_tag_dyneither(_tmp411,sizeof(void*),1U));});});}
# 2190
if(!Cyc_gathering()){
# 2193
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp701=Cyc_output_dir;Cyc_Filename_concat(_tmp701,({const char*_tmp414="BUILDLIB.LOG";_tag_dyneither(_tmp414,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp417=({struct Cyc_String_pa_PrintArg_struct _tmp4D9;_tmp4D9.tag=0U,_tmp4D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4D9;});void*_tmp415[1U];_tmp415[0]=& _tmp417;({struct Cyc___cycFILE*_tmp703=Cyc_stderr;struct _dyneither_ptr _tmp702=({const char*_tmp416="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp416,sizeof(char),50U);});Cyc_fprintf(_tmp703,_tmp702,_tag_dyneither(_tmp415,sizeof(void*),1U));});});
return 1;}
# 2199
if(!Cyc_do_setjmp){
# 2201
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp704=Cyc_output_dir;Cyc_Filename_concat(_tmp704,({const char*_tmp418="cstubs.c";_tag_dyneither(_tmp418,sizeof(char),9U);}));}),sizeof(char),1U)),"w");
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp41B=({struct Cyc_String_pa_PrintArg_struct _tmp4DA;_tmp4DA.tag=0U,_tmp4DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4DA;});void*_tmp419[1U];_tmp419[0]=& _tmp41B;({struct Cyc___cycFILE*_tmp706=Cyc_stderr;struct _dyneither_ptr _tmp705=({const char*_tmp41A="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp41A,sizeof(char),50U);});Cyc_fprintf(_tmp706,_tmp705,_tag_dyneither(_tmp419,sizeof(void*),1U));});});
return 1;}
# 2208
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp707=Cyc_output_dir;Cyc_Filename_concat(_tmp707,({const char*_tmp41C="cycstubs.cyc";_tag_dyneither(_tmp41C,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp41F=({struct Cyc_String_pa_PrintArg_struct _tmp4DB;_tmp4DB.tag=0U,_tmp4DB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp4DB;});void*_tmp41D[1U];_tmp41D[0]=& _tmp41F;({struct Cyc___cycFILE*_tmp709=Cyc_stderr;struct _dyneither_ptr _tmp708=({const char*_tmp41E="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp41E,sizeof(char),52U);});Cyc_fprintf(_tmp709,_tmp708,_tag_dyneither(_tmp41D,sizeof(void*),1U));});});
# 2213
return 1;}
# 2215
({void*_tmp420=0U;({struct Cyc___cycFILE*_tmp70B=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _dyneither_ptr _tmp70A=({const char*_tmp421="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp421,sizeof(char),32U);});Cyc_fprintf(_tmp70B,_tmp70A,_tag_dyneither(_tmp420,sizeof(void*),0U));});});}}{
# 2222
const char*outdir=(const char*)_check_null(_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 2228
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp422=0U;({struct Cyc___cycFILE*_tmp70D=Cyc_stderr;struct _dyneither_ptr _tmp70C=({const char*_tmp423="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp423,sizeof(char),22U);});Cyc_fprintf(_tmp70D,_tmp70C,_tag_dyneither(_tmp422,sizeof(void*),0U));});});
 exit(1);}}}
# 2236
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2239
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2247
return 0;};};};};}
