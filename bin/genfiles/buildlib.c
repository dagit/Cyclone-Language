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
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U,Cyc_Absyn_BoolKind  = 6U,Cyc_Absyn_PtrBndKind  = 7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 426 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 515
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 522
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 540
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1094 "absyn.h"
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1151
struct Cyc_Absyn_Decl*Cyc_Absyn_lookup_decl(struct Cyc_List_List*decls,struct _dyneither_ptr*name);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1165
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfo);
# 1173
struct _tuple1*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
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
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 32 "cifc.h"
void Cyc_Cifc_merge_sys_user_decl(unsigned int loc,int is_buildlib,struct Cyc_Absyn_Decl*user_decl,struct Cyc_Absyn_Decl*c_decl);
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
# 80 "buildlib.cyl"
void Cyc_Lex_lex_init(int use_cyclone_keywords);static char _tmp0[4U]="gcc";
# 90
static struct _dyneither_ptr Cyc_cyclone_cc={_tmp0,_tmp0,_tmp0 + 4U};static char _tmp1[1U]="";
static struct _dyneither_ptr Cyc_target_cflags={_tmp1,_tmp1,_tmp1 + 1U};
# 93
static int Cyc_do_setjmp=0;
static int Cyc_verbose=0;
int Cyc_noexpand_r=1;
# 97
struct Cyc___cycFILE*Cyc_log_file=0;
struct Cyc___cycFILE*Cyc_cstubs_file=0;
struct Cyc___cycFILE*Cyc_cycstubs_file=0;
# 101
int Cyc_log(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 104
if(Cyc_log_file == 0){
({void*_tmp2=0U;({struct Cyc___cycFILE*_tmp42E=Cyc_stderr;struct _dyneither_ptr _tmp42D=({const char*_tmp3="Internal error: log file is NULL\n";_tag_dyneither(_tmp3,sizeof(char),34U);});Cyc_fprintf(_tmp42E,_tmp42D,_tag_dyneither(_tmp2,sizeof(void*),0U));});});
 exit(1);}{
# 108
int _tmp4=Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_log_file),fmt,ap);
Cyc_fflush((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
return _tmp4;};}
# 113
static struct _dyneither_ptr*Cyc_current_source=0;
static struct Cyc_List_List*Cyc_current_args=0;
static struct Cyc_Set_Set**Cyc_current_targets=0;
static void Cyc_add_target(struct _dyneither_ptr*sptr){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Set_Set*_tmp42F=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),sptr);*_tmp5=_tmp42F;});_tmp5;});}struct _tuple21{struct _dyneither_ptr*f1;struct Cyc_Set_Set*f2;};
# 121
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*);
int Cyc_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_args(struct Cyc_Lexing_lexbuf*);
int Cyc_token(struct Cyc_Lexing_lexbuf*);
int Cyc_string(struct Cyc_Lexing_lexbuf*);
# 127
struct Cyc___cycFILE*Cyc_slurp_out=0;
int Cyc_slurp(struct Cyc_Lexing_lexbuf*);
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*);
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*);struct _tuple22{struct _dyneither_ptr f1;struct _dyneither_ptr*f2;};
# 135
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*);
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*);
struct _dyneither_ptr Cyc_current_line={(void*)0,(void*)0,(void*)(0 + 0)};struct _tuple23{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};struct _tuple24{struct _dyneither_ptr*f1;struct _dyneither_ptr*f2;};struct _tuple25{struct _dyneither_ptr f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;struct Cyc_List_List*f6;struct Cyc_List_List*f7;struct Cyc_List_List*f8;};
# 151
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
# 172
int Cyc_numdef=0;
# 174
static struct Cyc_List_List*Cyc_cppargs=0;static char _tmp6[14U]="BUILDLIB_sym_";
# 176
struct _dyneither_ptr Cyc_user_prefix={_tmp6,_tmp6,_tmp6 + 14U};
static struct _dyneither_ptr*Cyc_add_user_prefix(struct _dyneither_ptr*symbol){
struct _dyneither_ptr s=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_user_prefix,(struct _dyneither_ptr)*symbol);
return({struct _dyneither_ptr*_tmp7=_cycalloc(sizeof(*_tmp7));*_tmp7=s;_tmp7;});}
# 181
static struct _dyneither_ptr Cyc_remove_user_prefix(struct _dyneither_ptr symbol){
unsigned int prefix_len=Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix);
if(Cyc_strncmp((struct _dyneither_ptr)symbol,(struct _dyneither_ptr)Cyc_user_prefix,prefix_len)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp3D0;_tmp3D0.tag=0U,_tmp3D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)symbol);_tmp3D0;});void*_tmp8[1U];_tmp8[0]=& _tmpA;({struct Cyc___cycFILE*_tmp431=Cyc_stderr;struct _dyneither_ptr _tmp430=({const char*_tmp9="Internal error: bad user type name %s\n";_tag_dyneither(_tmp9,sizeof(char),39U);});Cyc_fprintf(_tmp431,_tmp430,_tag_dyneither(_tmp8,sizeof(void*),1U));});});
return symbol;}
# 187
return(struct _dyneither_ptr)({struct _dyneither_ptr _tmp434=(struct _dyneither_ptr)symbol;int _tmp433=(int)prefix_len;Cyc_substring(_tmp434,_tmp433,({unsigned long _tmp432=Cyc_strlen((struct _dyneither_ptr)symbol);_tmp432 - prefix_len;}));});}
# 190
static void Cyc_rename_decl(struct Cyc_Absyn_Decl*d){
void*_tmpB=d->r;void*_tmpC=_tmpB;struct Cyc_Absyn_Typedefdecl*_tmp14;struct Cyc_Absyn_Enumdecl*_tmp13;struct Cyc_Absyn_Aggrdecl*_tmp12;switch(*((int*)_tmpC)){case 5U: _LL1: _tmp12=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL2:
# 193
({struct _dyneither_ptr*_tmp436=({struct _dyneither_ptr*_tmpD=_cycalloc(sizeof(*_tmpD));({struct _dyneither_ptr _tmp435=Cyc_remove_user_prefix(*(*_tmp12->name).f2);*_tmpD=_tmp435;});_tmpD;});(*_tmp12->name).f2=_tmp436;});
goto _LL0;case 7U: _LL3: _tmp13=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL4:
# 196
({struct _dyneither_ptr*_tmp438=({struct _dyneither_ptr*_tmpE=_cycalloc(sizeof(*_tmpE));({struct _dyneither_ptr _tmp437=Cyc_remove_user_prefix(*(*_tmp13->name).f2);*_tmpE=_tmp437;});_tmpE;});(*_tmp13->name).f2=_tmp438;});
goto _LL0;case 8U: _LL5: _tmp14=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmpC)->f1;_LL6:
# 199
({struct _dyneither_ptr*_tmp43A=({struct _dyneither_ptr*_tmpF=_cycalloc(sizeof(*_tmpF));({struct _dyneither_ptr _tmp439=Cyc_remove_user_prefix(*(*_tmp14->name).f2);*_tmpF=_tmp439;});_tmpF;});(*_tmp14->name).f2=_tmp43A;});
goto _LL0;case 1U: _LL7: _LL8:
# 202
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
# 216
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmp43C=Cyc_stderr;struct _dyneither_ptr _tmp43B=({const char*_tmp11="Error in .cys file: bad user-defined type definition\n";_tag_dyneither(_tmp11,sizeof(char),54U);});Cyc_fprintf(_tmp43C,_tmp43B,_tag_dyneither(_tmp10,sizeof(void*),0U));});});
 exit(1);}_LL0:;}
# 223
const int Cyc_lex_base[431U]={0,0,75,192,305,310,311,166,312,91,27,384,28,523,637,715,829,325,92,- 3,0,- 1,- 2,- 8,- 3,1,- 2,323,- 4,2,166,- 5,605,907,312,- 6,- 8,- 4,16,945,- 3,1021,29,11,1135,- 4,13,1249,223,- 14,317,12,- 2,216,20,27,29,34,50,49,71,55,66,101,101,100,108,95,370,386,112,104,105,123,126,375,399,112,134,190,381,1363,1478,414,205,213,233,219,224,224,245,498,1592,1707,- 9,525,- 10,233,253,507,1821,1936,- 7,654,684,- 11,432,514,516,2013,2090,2167,2248,434,465,531,2323,253,253,253,251,247,257,0,13,4,2404,5,628,2412,2477,660,- 4,- 3,49,467,6,2438,7,701,2500,2538,793,- 23,1161,1166,273,621,254,264,271,266,277,288,277,277,284,295,302,290,- 20,302,306,304,314,321,324,- 15,305,324,316,313,324,319,366,415,417,403,399,399,423,424,- 17,416,420,415,437,433,450,429,452,455,443,447,438,438,- 19,447,442,445,455,464,447,449,481,488,489,4,6,21,490,491,478,503,515,515,522,524,24,554,556,24,20,557,570,53,625,627,- 13,645,592,594,582,581,595,597,604,605,1,661,622,632,1078,656,657,658,659,622,620,637,643,644,699,702,703,650,651,716,717,718,665,666,721,732,733,680,681,737,765,767,714,716,797,798,808,- 12,2601,1254,- 22,755,1365,- 21,1278,754,755,752,751,750,745,747,750,751,749,762,1370,754,756,754,767,1480,1504,770,771,761,762,760,773,1597,765,766,764,788,1599,- 7,- 8,8,956,2570,9,930,2603,2658,1103,1372,- 49,1257,- 2,827,- 4,828,930,1251,829,1104,925,1446,830,2685,2728,834,860,862,975,2798,864,1053,- 36,- 42,- 37,2873,- 28,892,- 40,- 25,906,- 27,- 45,- 39,- 48,2948,2977,1676,995,1005,1566,2987,3017,1692,1787,3050,3081,3119,998,1041,3189,3227,1077,1087,1079,1123,1191,1201,- 6,- 34,943,3159,- 47,- 30,- 32,- 46,- 29,- 31,- 33,953,3267,964,999,1806,1000,1002,1029,1040,1043,1045,1121,1127,1129,3340,3424,- 16,- 18,1908,1130,- 24,- 41,- 38,- 35,- 26,1377,3506,2,3589,1131,15,1073,1077,1078,1076,1072,1125,1195};
const int Cyc_lex_backtrk[431U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,- 1,- 1,- 1,2,- 1,8,- 1,3,5,- 1,- 1,6,5,- 1,- 1,- 1,7,6,- 1,6,5,2,0,- 1,- 1,0,2,- 1,12,13,- 1,13,13,13,13,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,11,12,2,4,4,- 1,0,0,0,2,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,2,8,3,5,- 1,6,5,- 1,- 1,6,5,2,8,3,5,- 1,6,5,- 1,22,22,22,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,15,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,17,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,6,1,9,2,4,- 1,5,4,- 1,- 1,2,- 1,48,- 1,48,48,48,48,48,48,48,48,5,7,48,48,48,48,0,48,48,- 1,- 1,- 1,0,- 1,43,- 1,- 1,42,- 1,- 1,- 1,- 1,9,7,- 1,7,7,- 1,8,9,- 1,- 1,9,5,6,5,5,- 1,4,4,4,6,6,5,5,- 1,- 1,- 1,9,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,2,- 1,1,2,1,1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_default[431U]={- 1,- 1,- 1,324,313,143,23,102,23,19,- 1,- 1,12,31,49,35,36,23,19,0,- 1,0,0,0,0,- 1,0,- 1,0,- 1,- 1,0,- 1,- 1,- 1,0,0,0,- 1,- 1,0,- 1,42,- 1,- 1,0,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,106,- 1,- 1,- 1,- 1,- 1,113,113,113,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,135,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,- 1,0,- 1,- 1,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,- 1,0,- 1,0,- 1,- 1,393,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,0,- 1,0,- 1,0,0,- 1,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,0,- 1,- 1,0,0,0,0,0,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
const int Cyc_lex_trans[3846U]={0,0,0,0,0,0,0,0,0,0,22,19,28,423,19,28,19,28,36,19,48,48,46,46,48,22,46,0,0,0,0,0,21,239,21,424,208,22,- 1,- 1,22,- 1,- 1,48,209,46,229,22,421,421,421,421,421,421,421,421,421,421,31,106,22,210,117,42,219,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,31,222,223,226,421,135,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,419,419,419,419,419,419,419,419,419,419,124,20,77,22,70,57,58,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,59,60,61,62,419,63,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,64,65,132,325,326,325,325,326,133,22,66,67,68,71,72,134,34,34,34,34,34,34,34,34,73,74,325,327,328,75,78,329,330,331,48,48,332,333,48,334,335,336,337,338,338,338,338,338,338,338,338,338,339,79,340,341,342,48,19,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,19,- 1,- 1,344,343,80,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,314,345,104,315,144,144,24,24,144,136,125,107,107,84,97,107,85,87,28,88,24,29,86,25,316,89,90,144,91,22,26,26,21,21,107,98,99,145,118,119,120,121,122,123,26,35,35,35,35,35,35,35,35,147,197,198,30,30,30,30,30,30,30,30,69,69,183,177,69,76,76,168,161,76,154,81,81,155,156,81,69,69,317,157,69,158,159,69,137,126,146,160,76,76,76,162,163,76,81,164,165,166,27,69,167,31,169,21,83,83,170,171,83,172,173,174,76,116,116,116,116,116,116,116,116,116,116,- 1,32,- 1,- 1,83,- 1,22,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,175,- 1,- 1,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,92,92,28,176,92,36,178,179,180,100,100,181,182,100,184,19,107,107,115,115,107,185,115,92,186,108,108,95,95,108,35,95,100,115,115,187,188,115,189,107,190,115,191,192,193,194,195,196,108,276,95,199,200,201,94,202,115,203,204,21,21,21,109,110,109,109,109,109,109,109,109,109,109,109,21,205,206,207,211,212,213,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,214,215,216,217,109,218,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,50,50,96,220,50,221,224,33,33,33,33,33,33,33,33,33,33,103,103,225,227,103,228,50,33,33,33,33,33,33,131,131,131,131,131,131,131,131,51,230,103,231,- 1,232,- 1,233,234,104,104,235,52,104,236,237,238,240,33,33,33,33,33,33,35,35,35,35,35,35,35,35,104,241,148,149,150,- 1,151,- 1,43,43,152,242,43,270,265,260,255,248,249,153,53,250,251,252,253,54,55,254,49,256,257,43,56,142,142,142,142,142,142,142,142,258,259,96,261,262,263,44,44,44,44,44,44,44,44,44,44,264,105,266,267,36,268,28,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,269,105,94,271,44,272,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,273,274,45,35,35,35,35,35,35,35,35,275,280,295,290,286,287,288,289,102,291,292,293,294,296,37,297,298,299,308,303,304,305,306,307,309,310,311,38,39,39,39,39,39,39,39,39,39,39,312,417,416,392,383,358,21,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,354,355,356,353,39,348,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,40,352,22,33,33,33,33,33,33,33,33,33,33,323,323,357,414,323,387,31,33,33,33,33,33,33,322,322,322,322,322,322,322,322,388,389,323,386,21,415,413,41,41,41,41,41,41,41,41,41,41,160,33,33,33,33,33,33,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,350,351,196,281,41,410,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,105,41,41,41,41,41,41,41,41,41,41,275,36,36,228,35,49,21,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,36,36,346,35,41,35,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,46,46,243,390,46,35,244,31,31,31,31,31,31,31,31,245,167,246,31,31,102,391,409,46,182,278,300,300,21,425,300,282,282,347,426,282,427,428,429,47,47,47,47,47,47,47,47,47,47,300,31,31,102,247,282,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,430,21,0,0,47,102,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,46,46,301,- 1,46,277,277,283,418,277,418,418,302,0,0,0,0,284,382,382,0,0,285,46,0,0,0,0,277,282,282,418,- 1,282,0,0,278,0,0,47,47,47,47,47,47,47,47,47,47,0,382,382,282,0,0,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,0,0,0,394,47,0,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,283,280,294,294,323,323,294,0,323,418,284,418,418,0,0,0,0,0,81,0,280,0,0,0,0,294,0,323,281,21,0,0,418,19,82,82,82,82,82,82,82,82,82,82,0,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,31,83,83,299,299,83,384,299,385,385,385,385,385,385,385,385,385,385,0,0,0,- 1,0,0,83,0,299,300,300,0,0,300,0,0,31,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,300,0,0,0,0,0,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,0,0,0,0,82,0,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,301,0,92,307,307,312,312,307,0,312,302,365,365,365,365,365,365,365,365,365,365,92,0,0,0,0,307,0,312,0,0,0,0,0,28,0,35,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,94,95,95,0,364,95,364,0,0,365,365,365,365,365,365,365,365,365,365,0,368,0,368,0,95,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,0,0,0,0,0,0,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,0,0,0,0,93,0,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,0,0,100,369,369,369,369,369,369,369,369,369,369,278,0,0,0,0,0,0,0,100,411,411,411,411,411,411,411,411,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,102,103,103,278,0,103,0,0,0,0,0,0,412,412,412,412,412,412,412,412,0,0,0,0,103,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,0,0,0,0,0,0,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,0,0,0,0,101,0,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,22,0,0,0,0,0,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,0,0,- 1,111,0,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,114,0,0,0,0,0,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,0,0,0,0,112,0,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,0,0,0,0,0,0,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,28,0,0,127,116,0,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,0,0,28,0,0,138,128,128,128,128,128,128,128,128,130,130,130,130,130,130,130,130,130,130,0,0,0,0,0,0,0,130,130,130,130,130,130,0,0,0,139,139,139,139,139,139,139,139,0,0,0,0,0,0,0,31,0,0,- 1,0,0,0,0,130,130,130,130,130,130,0,0,0,0,0,0,0,0,0,129,130,130,130,130,130,130,130,130,130,130,31,0,0,0,0,0,0,130,130,130,130,130,130,141,141,141,141,141,141,141,141,141,141,140,0,0,0,0,0,0,141,141,141,141,141,141,0,0,0,130,130,130,130,130,130,19,0,0,318,0,0,141,141,141,141,141,141,141,141,141,141,0,141,141,141,141,141,141,141,141,141,141,141,141,0,277,277,0,0,277,0,0,0,319,319,319,319,319,319,319,319,0,0,0,0,0,0,0,277,0,141,141,141,141,141,141,278,0,0,0,0,0,0,0,0,0,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,28,321,321,321,321,321,321,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,320,0,0,0,0,0,279,0,0,0,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,321,0,0,0,0,0,0,0,321,321,321,321,321,321,0,0,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,321,321,321,321,321,321,372,0,0,0,0,0,0,0,0,373,0,0,374,359,0,360,360,360,360,360,360,360,360,360,360,361,0,0,0,0,0,0,372,0,0,0,361,0,0,0,0,373,0,362,374,0,0,0,0,0,0,0,363,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,361,0,0,0,0,0,0,362,0,0,0,0,0,0,0,0,363,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,0,0,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,0,0,0,0,349,0,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,366,366,366,366,366,366,366,366,366,366,0,0,0,0,0,0,0,0,0,0,0,367,96,0,0,0,0,359,96,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,0,361,0,0,367,96,0,0,362,0,0,96,94,0,0,0,0,363,94,0,366,366,366,366,366,366,366,366,366,366,0,0,0,361,0,0,0,0,0,0,362,367,96,0,94,0,0,0,96,363,94,0,0,369,369,369,369,369,369,369,369,369,369,0,0,0,0,0,0,0,0,0,0,367,96,96,0,0,0,0,96,96,359,0,370,370,370,370,370,370,370,370,371,371,0,0,0,0,0,0,0,0,0,0,0,361,0,96,0,0,0,0,380,96,0,0,0,0,0,0,359,381,371,371,371,371,371,371,371,371,371,371,0,0,0,0,0,361,0,0,0,0,0,361,380,0,0,0,0,0,378,0,0,381,0,0,0,0,0,379,0,0,385,385,385,385,385,385,385,385,385,385,0,0,0,361,0,0,0,0,0,0,378,367,96,0,0,0,0,0,96,379,375,375,375,375,375,375,375,375,375,375,0,0,0,0,0,0,0,375,375,375,375,375,375,367,96,0,0,0,0,0,96,0,0,0,0,0,0,0,375,375,375,375,375,375,375,375,375,375,0,375,375,375,375,375,375,375,375,375,375,375,375,0,0,0,395,0,376,0,0,396,0,0,0,0,0,377,0,0,397,397,397,397,397,397,397,397,0,375,375,375,375,375,375,398,0,0,0,0,376,0,0,0,0,0,0,0,0,377,0,0,0,0,0,0,0,0,0,0,0,0,0,0,399,0,0,0,0,400,401,0,0,0,402,0,0,0,0,0,0,0,403,0,0,0,404,0,405,0,406,0,407,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,143,0,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,0,0,0,0,0,0,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,19,0,0,420,0,0,0,419,419,419,419,419,419,419,419,419,419,0,0,0,0,0,0,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,0,0,0,0,419,0,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,422,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,421,0,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
const int Cyc_lex_check[3846U]={- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,0,25,29,0,125,127,136,138,315,318,43,43,46,46,43,423,46,- 1,- 1,- 1,- 1,- 1,123,238,420,0,207,10,12,42,10,12,42,43,207,46,208,20,1,1,1,1,1,1,1,1,1,1,38,51,124,209,10,38,218,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,134,221,222,225,1,134,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,2,2,2,2,2,2,2,2,2,2,9,18,54,46,55,56,57,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,58,59,60,61,2,62,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,63,64,7,3,3,3,3,3,7,7,65,66,67,70,71,7,30,30,30,30,30,30,30,30,72,73,3,3,3,74,77,3,3,3,48,48,3,3,48,3,3,3,3,3,3,3,3,3,3,3,3,3,3,78,3,3,3,48,0,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,10,12,42,3,3,79,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,4,3,84,4,5,5,6,8,5,6,8,50,50,53,85,50,53,86,27,87,17,27,53,17,4,88,89,5,90,5,6,8,9,18,50,97,98,5,117,118,119,120,121,122,17,34,34,34,34,34,34,34,34,146,148,148,27,27,27,27,27,27,27,27,68,68,149,150,68,75,75,151,152,75,153,80,80,154,155,80,69,69,4,156,69,157,158,68,6,8,5,159,75,76,76,161,162,76,80,163,164,165,17,69,166,27,168,7,83,83,169,170,83,171,172,173,76,11,11,11,11,11,11,11,11,11,11,106,27,113,106,83,113,3,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,114,174,135,114,11,135,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,91,91,69,175,91,176,177,178,179,99,99,180,181,99,183,76,107,107,108,108,107,184,108,91,185,13,13,95,95,13,83,95,99,115,115,186,187,115,188,107,189,108,190,191,192,193,194,195,13,197,95,198,199,200,4,201,115,202,203,5,6,8,13,13,13,13,13,13,13,13,13,13,13,13,17,204,205,206,210,211,212,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,213,214,215,216,13,217,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,14,14,95,219,14,220,223,32,32,32,32,32,32,32,32,32,32,103,103,224,226,103,227,14,32,32,32,32,32,32,128,128,128,128,128,128,128,128,14,229,103,230,106,231,113,232,233,104,104,234,14,104,235,236,237,239,32,32,32,32,32,32,131,131,131,131,131,131,131,131,104,240,147,147,147,114,147,135,15,15,147,241,15,243,244,245,246,247,248,147,14,249,250,251,252,14,14,253,254,255,256,15,14,139,139,139,139,139,139,139,139,257,258,259,260,261,262,15,15,15,15,15,15,15,15,15,15,263,264,265,266,103,267,13,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,268,104,269,270,15,271,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,272,273,15,142,142,142,142,142,142,142,142,274,279,283,284,285,286,287,288,289,290,291,292,293,295,16,296,297,298,301,302,303,304,305,306,308,309,310,16,16,16,16,16,16,16,16,16,16,16,311,327,329,332,336,339,14,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,340,340,340,341,16,344,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,16,351,16,33,33,33,33,33,33,33,33,33,33,316,316,354,330,316,334,15,33,33,33,33,33,33,319,319,319,319,319,319,319,319,334,334,316,384,316,330,393,39,39,39,39,39,39,39,39,39,39,395,33,33,33,33,33,33,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,342,342,396,398,39,399,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,39,400,41,41,41,41,41,41,41,41,41,41,401,362,363,402,372,403,16,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,362,363,345,372,41,373,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,41,44,44,242,333,44,373,242,322,322,322,322,322,322,322,322,242,404,242,376,377,378,333,405,44,406,412,144,144,422,424,144,145,145,345,425,145,426,427,428,44,44,44,44,44,44,44,44,44,44,144,376,377,378,242,145,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,429,430,- 1,- 1,44,379,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,44,47,47,144,331,47,277,277,145,325,277,325,325,144,- 1,- 1,- 1,- 1,145,380,381,- 1,- 1,145,47,- 1,- 1,- 1,- 1,277,282,282,325,331,282,- 1,- 1,277,- 1,- 1,47,47,47,47,47,47,47,47,47,47,- 1,380,381,282,- 1,- 1,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,- 1,- 1,- 1,331,47,- 1,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,47,81,81,280,280,81,282,280,294,294,323,323,294,- 1,323,418,282,418,418,- 1,- 1,- 1,- 1,- 1,81,- 1,280,- 1,- 1,- 1,- 1,294,- 1,323,280,323,- 1,- 1,418,294,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,- 1,- 1,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,- 1,- 1,- 1,- 1,81,- 1,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,81,82,82,299,299,82,335,299,335,335,335,335,335,335,335,335,335,335,- 1,- 1,- 1,331,- 1,- 1,82,- 1,299,300,300,- 1,- 1,300,- 1,- 1,299,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,300,- 1,- 1,- 1,- 1,- 1,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,- 1,- 1,- 1,- 1,82,- 1,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,82,92,92,300,- 1,92,307,307,312,312,307,- 1,312,300,364,364,364,364,364,364,364,364,364,364,92,- 1,- 1,- 1,- 1,307,- 1,312,- 1,- 1,- 1,- 1,- 1,307,- 1,312,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,- 1,- 1,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,- 1,- 1,- 1,- 1,92,- 1,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,92,93,93,- 1,361,93,361,- 1,- 1,361,361,361,361,361,361,361,361,361,361,- 1,367,- 1,367,- 1,93,367,367,367,367,367,367,367,367,367,367,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,- 1,- 1,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,- 1,- 1,- 1,- 1,93,- 1,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,93,100,100,- 1,- 1,100,368,368,368,368,368,368,368,368,368,368,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,397,397,397,397,397,397,397,397,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,- 1,- 1,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,- 1,- 1,- 1,- 1,100,- 1,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,100,101,101,411,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,411,411,411,411,411,411,411,411,- 1,- 1,- 1,- 1,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,- 1,- 1,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,- 1,- 1,- 1,- 1,101,- 1,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,101,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,- 1,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,- 1,- 1,- 1,- 1,109,- 1,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,109,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,- 1,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,- 1,- 1,- 1,- 1,110,- 1,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,110,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,- 1,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,112,- 1,- 1,112,111,- 1,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,111,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,- 1,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,- 1,- 1,- 1,- 1,112,- 1,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,112,116,116,116,116,116,116,116,116,116,116,- 1,- 1,- 1,- 1,- 1,- 1,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,126,- 1,- 1,126,116,- 1,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,116,- 1,- 1,137,- 1,- 1,137,126,126,126,126,126,126,126,126,129,129,129,129,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,137,137,137,137,137,137,137,137,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,- 1,- 1,112,- 1,- 1,- 1,- 1,129,129,129,129,129,129,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,126,130,130,130,130,130,130,130,130,130,130,137,- 1,- 1,- 1,- 1,- 1,- 1,130,130,130,130,130,130,140,140,140,140,140,140,140,140,140,140,137,- 1,- 1,- 1,- 1,- 1,- 1,140,140,140,140,140,140,- 1,- 1,- 1,130,130,130,130,130,130,317,- 1,- 1,317,- 1,- 1,141,141,141,141,141,141,141,141,141,141,- 1,140,140,140,140,140,140,141,141,141,141,141,141,- 1,276,276,- 1,- 1,276,- 1,- 1,- 1,317,317,317,317,317,317,317,317,- 1,- 1,- 1,- 1,- 1,- 1,- 1,276,- 1,141,141,141,141,141,141,276,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,320,320,320,320,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,317,320,320,320,320,320,320,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,317,- 1,- 1,- 1,- 1,- 1,276,- 1,- 1,- 1,320,320,320,320,320,320,321,321,321,321,321,321,321,321,321,321,- 1,- 1,- 1,- 1,- 1,- 1,- 1,321,321,321,321,321,321,- 1,- 1,337,- 1,337,337,337,337,337,337,337,337,337,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,321,321,321,321,321,321,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,337,338,- 1,338,338,338,338,338,338,338,338,338,338,337,- 1,- 1,- 1,- 1,- 1,- 1,337,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,337,- 1,338,337,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,338,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,338,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,- 1,- 1,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,- 1,- 1,- 1,- 1,343,- 1,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,343,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,- 1,- 1,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,- 1,- 1,- 1,- 1,349,- 1,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,349,359,359,359,359,359,359,359,359,359,359,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,359,359,- 1,- 1,- 1,- 1,360,359,360,360,360,360,360,360,360,360,360,360,365,365,365,365,365,365,365,365,365,365,- 1,360,- 1,- 1,359,359,- 1,- 1,360,- 1,- 1,359,365,- 1,- 1,- 1,- 1,360,365,- 1,366,366,366,366,366,366,366,366,366,366,- 1,- 1,- 1,360,- 1,- 1,- 1,- 1,- 1,- 1,360,366,366,- 1,365,- 1,- 1,- 1,366,360,365,- 1,- 1,369,369,369,369,369,369,369,369,369,369,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,366,366,369,- 1,- 1,- 1,- 1,366,369,370,- 1,370,370,370,370,370,370,370,370,370,370,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,370,- 1,369,- 1,- 1,- 1,- 1,370,369,- 1,- 1,- 1,- 1,- 1,- 1,371,370,371,371,371,371,371,371,371,371,371,371,- 1,- 1,- 1,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,370,- 1,- 1,- 1,- 1,- 1,371,- 1,- 1,385,385,385,385,385,385,385,385,385,385,- 1,- 1,- 1,371,- 1,- 1,- 1,- 1,- 1,- 1,371,385,385,- 1,- 1,- 1,- 1,- 1,385,371,374,374,374,374,374,374,374,374,374,374,- 1,- 1,- 1,- 1,- 1,- 1,- 1,374,374,374,374,374,374,385,385,- 1,- 1,- 1,- 1,- 1,385,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,375,375,375,375,375,375,375,375,375,- 1,374,374,374,374,374,374,375,375,375,375,375,375,- 1,- 1,- 1,394,- 1,375,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,394,394,394,394,394,394,394,394,- 1,375,375,375,375,375,375,394,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,375,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,394,394,- 1,- 1,- 1,394,- 1,- 1,- 1,- 1,- 1,- 1,- 1,394,- 1,- 1,- 1,394,- 1,394,- 1,394,- 1,394,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,- 1,- 1,- 1,- 1,- 1,- 1,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,407,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,- 1,- 1,- 1,- 1,- 1,- 1,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,408,419,- 1,- 1,419,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,- 1,- 1,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,- 1,- 1,- 1,- 1,419,- 1,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,419,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,421,- 1,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,421,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1,- 1};
int Cyc_lex_engine(int start_state,struct Cyc_Lexing_lexbuf*lbuf){
# 230
int state;int base;int backtrk;
int c;
state=start_state;
# 234
if(state >= 0){
({int _tmp43D=lbuf->lex_start_pos=lbuf->lex_curr_pos;lbuf->lex_last_pos=_tmp43D;});
lbuf->lex_last_action=- 1;}else{
# 238
state=(- state)- 1;}
# 240
while(1){
base=*((const int*)_check_known_subscript_notnull(Cyc_lex_base,431U,sizeof(int),state));
if(base < 0)return(- base)- 1;
backtrk=Cyc_lex_backtrk[state];
if(backtrk >= 0){
lbuf->lex_last_pos=lbuf->lex_curr_pos;
lbuf->lex_last_action=backtrk;}
# 248
if(lbuf->lex_curr_pos >= lbuf->lex_buffer_len){
if(!lbuf->lex_eof_reached)
return(- state)- 1;else{
# 252
c=256;}}else{
# 254
c=(int)*((char*)_check_dyneither_subscript(lbuf->lex_buffer,sizeof(char),lbuf->lex_curr_pos ++));
if(c == -1)c=256;}
# 257
if(*((const int*)_check_known_subscript_notnull(Cyc_lex_check,3846U,sizeof(int),base + c))== state)
state=*((const int*)_check_known_subscript_notnull(Cyc_lex_trans,3846U,sizeof(int),base + c));else{
# 260
state=Cyc_lex_default[state];}
if(state < 0){
lbuf->lex_curr_pos=lbuf->lex_last_pos;
if(lbuf->lex_last_action == - 1)
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp43E=({const char*_tmp15="empty token";_tag_dyneither(_tmp15,sizeof(char),12U);});_tmp16->f1=_tmp43E;});_tmp16;}));else{
# 266
return lbuf->lex_last_action;}}else{
# 269
if(c == 256)lbuf->lex_eof_reached=0;}}}
# 273
struct _tuple21*Cyc_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp17=lexstate;switch(_tmp17){case 0U: _LL1: _LL2:
# 232 "buildlib.cyl"
 Cyc_macroname(lexbuf);
for(0;Cyc_current_args != 0;Cyc_current_args=((struct Cyc_List_List*)_check_null(Cyc_current_args))->tl){
Cyc_current_targets=({struct Cyc_Set_Set**_tmp18=_cycalloc(sizeof(*_tmp18));({struct Cyc_Set_Set*_tmp440=({struct Cyc_Set_Set*_tmp43F=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_delete)(_tmp43F,(struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_current_args))->hd);});*_tmp18=_tmp440;});_tmp18;});}
# 237
return({struct _tuple21*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->f1=(struct _dyneither_ptr*)_check_null(Cyc_current_source),_tmp19->f2=*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets));_tmp19;});case 1U: _LL3: _LL4:
# 240 "buildlib.cyl"
 return Cyc_line(lexbuf);case 2U: _LL5: _LL6:
# 242
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_line_rec(lexbuf,lexstate);}_LL0:;}
# 246
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp441=({const char*_tmp1A="some action didn't return!";_tag_dyneither(_tmp1A,sizeof(char),27U);});_tmp1B->f1=_tmp441;});_tmp1B;}));}
# 248
struct _tuple21*Cyc_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_line_rec(lexbuf,0);}
int Cyc_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp1C=lexstate;switch(_tmp1C){case 0U: _LL1: _LL2:
# 246 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmp1D=_cycalloc(sizeof(*_tmp1D));({struct _dyneither_ptr _tmp444=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp443=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp443,0,(unsigned long)(({
int _tmp442=Cyc_Lexing_lexeme_end(lexbuf);_tmp442 - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 246
*_tmp1D=_tmp444;});_tmp1D;});
# 248
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp1E=_cycalloc(sizeof(*_tmp1E));({struct Cyc_Set_Set*_tmp445=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp1E=_tmp445;});_tmp1E;});
Cyc_token(lexbuf);
return 0;case 1U: _LL3: _LL4:
# 254
 Cyc_current_source=({struct _dyneither_ptr*_tmp1F=_cycalloc(sizeof(*_tmp1F));({struct _dyneither_ptr _tmp448=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp447=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp447,0,(unsigned long)(({
int _tmp446=Cyc_Lexing_lexeme_end(lexbuf);_tmp446 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 254
*_tmp1F=_tmp448;});_tmp1F;});
# 256
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp20=_cycalloc(sizeof(*_tmp20));({struct Cyc_Set_Set*_tmp449=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp20=_tmp449;});_tmp20;});
Cyc_args(lexbuf);
return 0;case 2U: _LL5: _LL6:
# 262
 Cyc_current_source=({struct _dyneither_ptr*_tmp21=_cycalloc(sizeof(*_tmp21));({struct _dyneither_ptr _tmp44A=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp21=_tmp44A;});_tmp21;});
Cyc_current_args=0;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp22=_cycalloc(sizeof(*_tmp22));({struct Cyc_Set_Set*_tmp44B=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp22=_tmp44B;});_tmp22;});
Cyc_token(lexbuf);
return 0;default: _LL7: _LL8:
# 268
(lexbuf->refill_buff)(lexbuf);
return Cyc_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 271
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp44C=({const char*_tmp23="some action didn't return!";_tag_dyneither(_tmp23,sizeof(char),27U);});_tmp24->f1=_tmp44C;});_tmp24;}));}
# 273
int Cyc_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_macroname_rec(lexbuf,1);}
int Cyc_args_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp25=lexstate;switch(_tmp25){case 0U: _LL1: _LL2: {
# 271 "buildlib.cyl"
struct _dyneither_ptr*_tmp26=({struct _dyneither_ptr*_tmp28=_cycalloc(sizeof(*_tmp28));({struct _dyneither_ptr _tmp44F=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp44E=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp44E,0,(unsigned long)(({
int _tmp44D=Cyc_Lexing_lexeme_end(lexbuf);_tmp44D - Cyc_Lexing_lexeme_start(lexbuf);})- 2));});
# 271
*_tmp28=_tmp44F;});_tmp28;});
# 273
Cyc_current_args=({struct Cyc_List_List*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->hd=_tmp26,_tmp27->tl=Cyc_current_args;_tmp27;});
return Cyc_args(lexbuf);}case 1U: _LL3: _LL4: {
# 277
struct _dyneither_ptr*_tmp29=({struct _dyneither_ptr*_tmp2B=_cycalloc(sizeof(*_tmp2B));({struct _dyneither_ptr _tmp452=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp451=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp451,0,(unsigned long)(({
int _tmp450=Cyc_Lexing_lexeme_end(lexbuf);_tmp450 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 277
*_tmp2B=_tmp452;});_tmp2B;});
# 279
Cyc_current_args=({struct Cyc_List_List*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->hd=_tmp29,_tmp2A->tl=Cyc_current_args;_tmp2A;});
return Cyc_args(lexbuf);}case 2U: _LL5: _LL6: {
# 283
struct _dyneither_ptr*_tmp2C=({struct _dyneither_ptr*_tmp2E=_cycalloc(sizeof(*_tmp2E));({struct _dyneither_ptr _tmp455=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp454=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp454,0,(unsigned long)(({
int _tmp453=Cyc_Lexing_lexeme_end(lexbuf);_tmp453 - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
# 283
*_tmp2E=_tmp455;});_tmp2E;});
# 285
Cyc_current_args=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=_tmp2C,_tmp2D->tl=Cyc_current_args;_tmp2D;});
return Cyc_token(lexbuf);}default: _LL7: _LL8:
# 288
(lexbuf->refill_buff)(lexbuf);
return Cyc_args_rec(lexbuf,lexstate);}_LL0:;}
# 291
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp456=({const char*_tmp2F="some action didn't return!";_tag_dyneither(_tmp2F,sizeof(char),27U);});_tmp30->f1=_tmp456;});_tmp30;}));}
# 293
int Cyc_args(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_args_rec(lexbuf,2);}
int Cyc_token_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp31=lexstate;switch(_tmp31){case 0U: _LL1: _LL2:
# 292 "buildlib.cyl"
 Cyc_add_target(({struct _dyneither_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));({struct _dyneither_ptr _tmp457=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp32=_tmp457;});_tmp32;}));return Cyc_token(lexbuf);case 1U: _LL3: _LL4:
# 295 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 298 "buildlib.cyl"
 return Cyc_token(lexbuf);case 3U: _LL7: _LL8:
# 301 "buildlib.cyl"
 Cyc_string(lexbuf);return Cyc_token(lexbuf);case 4U: _LL9: _LLA:
# 304 "buildlib.cyl"
 return Cyc_token(lexbuf);case 5U: _LLB: _LLC:
# 306
 return Cyc_token(lexbuf);case 6U: _LLD: _LLE:
# 308
 return Cyc_token(lexbuf);case 7U: _LLF: _LL10:
# 310
 return Cyc_token(lexbuf);case 8U: _LL11: _LL12:
# 313 "buildlib.cyl"
 return Cyc_token(lexbuf);case 9U: _LL13: _LL14:
# 316 "buildlib.cyl"
 return Cyc_token(lexbuf);case 10U: _LL15: _LL16:
# 319 "buildlib.cyl"
 return Cyc_token(lexbuf);case 11U: _LL17: _LL18:
# 321
 return Cyc_token(lexbuf);case 12U: _LL19: _LL1A:
# 323
 return Cyc_token(lexbuf);case 13U: _LL1B: _LL1C:
# 325
 return Cyc_token(lexbuf);case 14U: _LL1D: _LL1E:
# 327
 return Cyc_token(lexbuf);case 15U: _LL1F: _LL20:
# 329
 return Cyc_token(lexbuf);case 16U: _LL21: _LL22:
# 331
 return Cyc_token(lexbuf);case 17U: _LL23: _LL24:
# 333
 return Cyc_token(lexbuf);case 18U: _LL25: _LL26:
# 335
 return Cyc_token(lexbuf);case 19U: _LL27: _LL28:
# 337
 return Cyc_token(lexbuf);case 20U: _LL29: _LL2A:
# 339
 return Cyc_token(lexbuf);case 21U: _LL2B: _LL2C:
# 341
 return Cyc_token(lexbuf);case 22U: _LL2D: _LL2E:
# 343
 return Cyc_token(lexbuf);case 23U: _LL2F: _LL30:
# 345
 return Cyc_token(lexbuf);case 24U: _LL31: _LL32:
# 348 "buildlib.cyl"
 return Cyc_token(lexbuf);case 25U: _LL33: _LL34:
# 350
 return Cyc_token(lexbuf);case 26U: _LL35: _LL36:
# 352
 return Cyc_token(lexbuf);case 27U: _LL37: _LL38:
# 354
 return Cyc_token(lexbuf);case 28U: _LL39: _LL3A:
# 356
 return Cyc_token(lexbuf);case 29U: _LL3B: _LL3C:
# 358
 return Cyc_token(lexbuf);case 30U: _LL3D: _LL3E:
# 360
 return Cyc_token(lexbuf);case 31U: _LL3F: _LL40:
# 362
 return Cyc_token(lexbuf);case 32U: _LL41: _LL42:
# 364
 return Cyc_token(lexbuf);case 33U: _LL43: _LL44:
# 366
 return Cyc_token(lexbuf);case 34U: _LL45: _LL46:
# 368
 return Cyc_token(lexbuf);case 35U: _LL47: _LL48:
# 370
 return Cyc_token(lexbuf);case 36U: _LL49: _LL4A:
# 372
 return Cyc_token(lexbuf);case 37U: _LL4B: _LL4C:
# 374
 return Cyc_token(lexbuf);case 38U: _LL4D: _LL4E:
# 376
 return Cyc_token(lexbuf);case 39U: _LL4F: _LL50:
# 378
 return Cyc_token(lexbuf);case 40U: _LL51: _LL52:
# 380
 return Cyc_token(lexbuf);case 41U: _LL53: _LL54:
# 382
 return Cyc_token(lexbuf);case 42U: _LL55: _LL56:
# 384
 return Cyc_token(lexbuf);case 43U: _LL57: _LL58:
# 386
 return Cyc_token(lexbuf);case 44U: _LL59: _LL5A:
# 388
 return Cyc_token(lexbuf);case 45U: _LL5B: _LL5C:
# 390
 return Cyc_token(lexbuf);case 46U: _LL5D: _LL5E:
# 392
 return Cyc_token(lexbuf);case 47U: _LL5F: _LL60:
# 394
 return Cyc_token(lexbuf);case 48U: _LL61: _LL62:
# 397 "buildlib.cyl"
 return Cyc_token(lexbuf);default: _LL63: _LL64:
(lexbuf->refill_buff)(lexbuf);
return Cyc_token_rec(lexbuf,lexstate);}_LL0:;}
# 401
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp458=({const char*_tmp33="some action didn't return!";_tag_dyneither(_tmp33,sizeof(char),27U);});_tmp34->f1=_tmp458;});_tmp34;}));}
# 403
int Cyc_token(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_token_rec(lexbuf,3);}
int Cyc_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp35=lexstate;switch(_tmp35){case 0U: _LL1: _LL2:
# 402 "buildlib.cyl"
 return Cyc_string(lexbuf);case 1U: _LL3: _LL4:
# 403 "buildlib.cyl"
 return 0;case 2U: _LL5: _LL6:
# 404 "buildlib.cyl"
 return Cyc_string(lexbuf);case 3U: _LL7: _LL8:
# 405 "buildlib.cyl"
 return Cyc_string(lexbuf);case 4U: _LL9: _LLA:
# 408 "buildlib.cyl"
 return Cyc_string(lexbuf);case 5U: _LLB: _LLC:
# 411 "buildlib.cyl"
 return Cyc_string(lexbuf);case 6U: _LLD: _LLE:
# 413
 return Cyc_string(lexbuf);case 7U: _LLF: _LL10:
# 414 "buildlib.cyl"
 return 0;case 8U: _LL11: _LL12:
# 415 "buildlib.cyl"
 return 0;case 9U: _LL13: _LL14:
# 416 "buildlib.cyl"
 return Cyc_string(lexbuf);default: _LL15: _LL16:
(lexbuf->refill_buff)(lexbuf);
return Cyc_string_rec(lexbuf,lexstate);}_LL0:;}
# 420
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp459=({const char*_tmp36="some action didn't return!";_tag_dyneither(_tmp36,sizeof(char),27U);});_tmp37->f1=_tmp459;});_tmp37;}));}
# 422
int Cyc_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_string_rec(lexbuf,4);}
int Cyc_slurp_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp38=lexstate;switch(_tmp38){case 0U: _LL1: _LL2:
# 425 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 427
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
while(Cyc_slurp_string(lexbuf)){;}
return 1;case 2U: _LL5: _LL6:
# 434 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp39=0U;({struct _dyneither_ptr _tmp45A=({const char*_tmp3A="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmp3A,sizeof(char),44U);});Cyc_log(_tmp45A,_tag_dyneither(_tmp39,sizeof(void*),0U));});});
return 1;case 3U: _LL7: _LL8:
# 440 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_MALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3B=0U;({struct _dyneither_ptr _tmp45B=({const char*_tmp3C="Warning: declaration of malloc sidestepped\n";_tag_dyneither(_tmp3C,sizeof(char),44U);});Cyc_log(_tmp45B,_tag_dyneither(_tmp3B,sizeof(void*),0U));});});
return 1;case 4U: _LL9: _LLA:
# 446 "buildlib.cyl"
 Cyc_fputs("*__IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3D=0U;({struct _dyneither_ptr _tmp45C=({const char*_tmp3E="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmp3E,sizeof(char),44U);});Cyc_log(_tmp45C,_tag_dyneither(_tmp3D,sizeof(void*),0U));});});
return 1;case 5U: _LLB: _LLC:
# 452 "buildlib.cyl"
 Cyc_fputs(" __IGNORE_FOR_CYCLONE_CALLOC(",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp3F=0U;({struct _dyneither_ptr _tmp45D=({const char*_tmp40="Warning: declaration of calloc sidestepped\n";_tag_dyneither(_tmp40,sizeof(char),44U);});Cyc_log(_tmp45D,_tag_dyneither(_tmp3F,sizeof(void*),0U));});});
return 1;case 6U: _LLD: _LLE:
# 456
 Cyc_fputs("__region",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp41=0U;({struct _dyneither_ptr _tmp45E=({const char*_tmp42="Warning: use of region sidestepped\n";_tag_dyneither(_tmp42,sizeof(char),36U);});Cyc_log(_tmp45E,_tag_dyneither(_tmp41,sizeof(void*),0U));});});
return 1;case 7U: _LLF: _LL10:
# 460
({void*_tmp43=0U;({struct _dyneither_ptr _tmp45F=({const char*_tmp44="Warning: use of __extension__ deleted\n";_tag_dyneither(_tmp44,sizeof(char),39U);});Cyc_log(_tmp45F,_tag_dyneither(_tmp43,sizeof(void*),0U));});});
return 1;case 8U: _LL11: _LL12:
# 465 "buildlib.cyl"
({void*_tmp45=0U;({struct _dyneither_ptr _tmp460=({const char*_tmp46="Warning: use of mode HI deleted\n";_tag_dyneither(_tmp46,sizeof(char),33U);});Cyc_log(_tmp460,_tag_dyneither(_tmp45,sizeof(void*),0U));});});
return 1;case 9U: _LL13: _LL14:
# 468
({void*_tmp47=0U;({struct _dyneither_ptr _tmp461=({const char*_tmp48="Warning: use of mode SI deleted\n";_tag_dyneither(_tmp48,sizeof(char),33U);});Cyc_log(_tmp461,_tag_dyneither(_tmp47,sizeof(void*),0U));});});
return 1;case 10U: _LL15: _LL16:
# 471
({void*_tmp49=0U;({struct _dyneither_ptr _tmp462=({const char*_tmp4A="Warning: use of mode QI deleted\n";_tag_dyneither(_tmp4A,sizeof(char),33U);});Cyc_log(_tmp462,_tag_dyneither(_tmp49,sizeof(void*),0U));});});
return 1;case 11U: _LL17: _LL18:
# 474
({void*_tmp4B=0U;({struct _dyneither_ptr _tmp463=({const char*_tmp4C="Warning: use of mode DI deleted\n";_tag_dyneither(_tmp4C,sizeof(char),33U);});Cyc_log(_tmp463,_tag_dyneither(_tmp4B,sizeof(void*),0U));});});
return 1;case 12U: _LL19: _LL1A:
# 477
({void*_tmp4D=0U;({struct _dyneither_ptr _tmp464=({const char*_tmp4E="Warning: use of mode DI deleted\n";_tag_dyneither(_tmp4E,sizeof(char),33U);});Cyc_log(_tmp464,_tag_dyneither(_tmp4D,sizeof(void*),0U));});});
return 1;case 13U: _LL1B: _LL1C:
# 480
({void*_tmp4F=0U;({struct _dyneither_ptr _tmp465=({const char*_tmp50="Warning: use of mode word deleted\n";_tag_dyneither(_tmp50,sizeof(char),35U);});Cyc_log(_tmp465,_tag_dyneither(_tmp4F,sizeof(void*),0U));});});
return 1;case 14U: _LL1D: _LL1E:
# 483
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 15U: _LL1F: _LL20:
# 485
 Cyc_fputs("inline",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 16U: _LL21: _LL22:
# 487
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 17U: _LL23: _LL24:
# 489
 Cyc_fputs("const",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 18U: _LL25: _LL26:
# 494 "buildlib.cyl"
 Cyc_fputs("int",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 1;case 19U: _LL27: _LL28:
# 496
 return 1;case 20U: _LL29: _LL2A:
# 498
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp51=0U;({struct _dyneither_ptr _tmp466=({const char*_tmp52="Warning: replacing use of __asm__ with 0\n";_tag_dyneither(_tmp52,sizeof(char),42U);});Cyc_log(_tmp466,_tag_dyneither(_tmp51,sizeof(void*),0U));});});
return 1;case 21U: _LL2B: _LL2C:
# 504
 Cyc_parens_to_match=1;
while(Cyc_asmtok(lexbuf)){;}
Cyc_fputs("0",(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
({void*_tmp53=0U;({struct _dyneither_ptr _tmp467=({const char*_tmp54="Warning: replacing use of __asm with 0\n";_tag_dyneither(_tmp54,sizeof(char),40U);});Cyc_log(_tmp467,_tag_dyneither(_tmp53,sizeof(void*),0U));});});
return 1;case 22U: _LL2D: _LL2E:
# 510
({int _tmp468=(int)Cyc_Lexing_lexeme_char(lexbuf,0);Cyc_fputc(_tmp468,(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));});return 1;default: _LL2F: _LL30:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_rec(lexbuf,lexstate);}_LL0:;}
# 514
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp469=({const char*_tmp55="some action didn't return!";_tag_dyneither(_tmp55,sizeof(char),27U);});_tmp56->f1=_tmp469;});_tmp56;}));}
# 516
int Cyc_slurp(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_rec(lexbuf,5);}
int Cyc_slurp_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp57=lexstate;switch(_tmp57){case 0U: _LL1: _LL2:
# 514 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 516
 Cyc_fputc((int)'"',(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));return 0;case 2U: _LL5: _LL6:
# 518
({void*_tmp58=0U;({struct _dyneither_ptr _tmp46A=({const char*_tmp59="Warning: unclosed string\n";_tag_dyneither(_tmp59,sizeof(char),26U);});Cyc_log(_tmp46A,_tag_dyneither(_tmp58,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0U,({struct _dyneither_ptr _tmp46B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D1.f1=_tmp46B;});_tmp3D1;});void*_tmp5A[1U];_tmp5A[0]=& _tmp5C;({struct Cyc___cycFILE*_tmp46D=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp46C=({const char*_tmp5B="%s";_tag_dyneither(_tmp5B,sizeof(char),3U);});Cyc_fprintf(_tmp46D,_tmp46C,_tag_dyneither(_tmp5A,sizeof(void*),1U));});});return 1;case 3U: _LL7: _LL8:
# 521
({struct Cyc_String_pa_PrintArg_struct _tmp5F=({struct Cyc_String_pa_PrintArg_struct _tmp3D2;_tmp3D2.tag=0U,({struct _dyneither_ptr _tmp46E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D2.f1=_tmp46E;});_tmp3D2;});void*_tmp5D[1U];_tmp5D[0]=& _tmp5F;({struct Cyc___cycFILE*_tmp470=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp46F=({const char*_tmp5E="%s";_tag_dyneither(_tmp5E,sizeof(char),3U);});Cyc_fprintf(_tmp470,_tmp46F,_tag_dyneither(_tmp5D,sizeof(void*),1U));});});return 1;case 4U: _LL9: _LLA:
# 523
({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0U,({struct _dyneither_ptr _tmp471=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D3.f1=_tmp471;});_tmp3D3;});void*_tmp60[1U];_tmp60[0]=& _tmp62;({struct Cyc___cycFILE*_tmp473=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp472=({const char*_tmp61="%s";_tag_dyneither(_tmp61,sizeof(char),3U);});Cyc_fprintf(_tmp473,_tmp472,_tag_dyneither(_tmp60,sizeof(void*),1U));});});return 1;case 5U: _LLB: _LLC:
# 525
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp3D4;_tmp3D4.tag=0U,({struct _dyneither_ptr _tmp474=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D4.f1=_tmp474;});_tmp3D4;});void*_tmp63[1U];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmp476=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp475=({const char*_tmp64="%s";_tag_dyneither(_tmp64,sizeof(char),3U);});Cyc_fprintf(_tmp476,_tmp475,_tag_dyneither(_tmp63,sizeof(void*),1U));});});return 1;case 6U: _LLD: _LLE:
# 527
({struct Cyc_String_pa_PrintArg_struct _tmp68=({struct Cyc_String_pa_PrintArg_struct _tmp3D5;_tmp3D5.tag=0U,({struct _dyneither_ptr _tmp477=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D5.f1=_tmp477;});_tmp3D5;});void*_tmp66[1U];_tmp66[0]=& _tmp68;({struct Cyc___cycFILE*_tmp479=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp478=({const char*_tmp67="%s";_tag_dyneither(_tmp67,sizeof(char),3U);});Cyc_fprintf(_tmp479,_tmp478,_tag_dyneither(_tmp66,sizeof(void*),1U));});});return 1;case 7U: _LLF: _LL10:
# 529
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0U,({struct _dyneither_ptr _tmp47A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D6.f1=_tmp47A;});_tmp3D6;});void*_tmp69[1U];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp47C=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp47B=({const char*_tmp6A="%s";_tag_dyneither(_tmp6A,sizeof(char),3U);});Cyc_fprintf(_tmp47C,_tmp47B,_tag_dyneither(_tmp69,sizeof(void*),1U));});});return 1;case 8U: _LL11: _LL12:
# 531
({struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp3D7;_tmp3D7.tag=0U,({struct _dyneither_ptr _tmp47D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));_tmp3D7.f1=_tmp47D;});_tmp3D7;});void*_tmp6C[1U];_tmp6C[0]=& _tmp6E;({struct Cyc___cycFILE*_tmp47F=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp47E=({const char*_tmp6D="%s";_tag_dyneither(_tmp6D,sizeof(char),3U);});Cyc_fprintf(_tmp47F,_tmp47E,_tag_dyneither(_tmp6C,sizeof(void*),1U));});});return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_slurp_string_rec(lexbuf,lexstate);}_LL0:;}
# 535
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp480=({const char*_tmp6F="some action didn't return!";_tag_dyneither(_tmp6F,sizeof(char),27U);});_tmp70->f1=_tmp480;});_tmp70;}));}
# 537
int Cyc_slurp_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_slurp_string_rec(lexbuf,6);}
int Cyc_asmtok_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp71=lexstate;switch(_tmp71){case 0U: _LL1: _LL2:
# 541 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 543
 if(Cyc_parens_to_match == 1)return 0;
-- Cyc_parens_to_match;
return 1;case 2U: _LL5: _LL6:
# 547
 ++ Cyc_parens_to_match;
return 1;case 3U: _LL7: _LL8:
# 550
 while(Cyc_asm_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 553
 while(Cyc_asm_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 556
 return 1;case 6U: _LLD: _LLE:
# 558
 return 1;default: _LLF: _LL10:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asmtok_rec(lexbuf,lexstate);}_LL0:;}
# 562
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp481=({const char*_tmp72="some action didn't return!";_tag_dyneither(_tmp72,sizeof(char),27U);});_tmp73->f1=_tmp481;});_tmp73;}));}
# 564
int Cyc_asmtok(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asmtok_rec(lexbuf,7);}
int Cyc_asm_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp74=lexstate;switch(_tmp74){case 0U: _LL1: _LL2:
# 562 "buildlib.cyl"
({void*_tmp75=0U;({struct _dyneither_ptr _tmp482=({const char*_tmp76="Warning: unclosed string\n";_tag_dyneither(_tmp76,sizeof(char),26U);});Cyc_log(_tmp482,_tag_dyneither(_tmp75,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 564
 return 0;case 2U: _LL5: _LL6:
# 566
({void*_tmp77=0U;({struct _dyneither_ptr _tmp483=({const char*_tmp78="Warning: unclosed string\n";_tag_dyneither(_tmp78,sizeof(char),26U);});Cyc_log(_tmp483,_tag_dyneither(_tmp77,sizeof(void*),0U));});});return 1;case 3U: _LL7: _LL8:
# 568
 return 1;case 4U: _LL9: _LLA:
# 570
 return 1;case 5U: _LLB: _LLC:
# 572
 return 1;case 6U: _LLD: _LLE:
# 574
 return 1;case 7U: _LLF: _LL10:
# 576
 return 1;case 8U: _LL11: _LL12:
# 578
 return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_string_rec(lexbuf,lexstate);}_LL0:;}
# 582
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp484=({const char*_tmp79="some action didn't return!";_tag_dyneither(_tmp79,sizeof(char),27U);});_tmp7A->f1=_tmp484;});_tmp7A;}));}
# 584
int Cyc_asm_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_string_rec(lexbuf,8);}
int Cyc_asm_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp7B=lexstate;switch(_tmp7B){case 0U: _LL1: _LL2:
# 582 "buildlib.cyl"
({void*_tmp7C=0U;({struct _dyneither_ptr _tmp485=({const char*_tmp7D="Warning: unclosed comment\n";_tag_dyneither(_tmp7D,sizeof(char),27U);});Cyc_log(_tmp485,_tag_dyneither(_tmp7C,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 584
 return 0;case 2U: _LL5: _LL6:
# 586
 return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_asm_comment_rec(lexbuf,lexstate);}_LL0:;}
# 590
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp486=({const char*_tmp7E="some action didn't return!";_tag_dyneither(_tmp7E,sizeof(char),27U);});_tmp7F->f1=_tmp486;});_tmp7F;}));}
# 592
int Cyc_asm_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_asm_comment_rec(lexbuf,9);}
struct _tuple22*Cyc_suck_line_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp80=lexstate;switch(_tmp80){case 0U: _LL1: _LL2:
# 594 "buildlib.cyl"
 Cyc_current_line=({const char*_tmp81="#define ";_tag_dyneither(_tmp81,sizeof(char),9U);});
Cyc_suck_macroname(lexbuf);
return({struct _tuple22*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->f1=Cyc_current_line,_tmp82->f2=(struct _dyneither_ptr*)_check_null(Cyc_current_source);_tmp82;});case 1U: _LL3: _LL4:
# 598
 return Cyc_suck_line(lexbuf);case 2U: _LL5: _LL6:
# 600
 return 0;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_line_rec(lexbuf,lexstate);}_LL0:;}
# 604
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp487=({const char*_tmp83="some action didn't return!";_tag_dyneither(_tmp83,sizeof(char),27U);});_tmp84->f1=_tmp487;});_tmp84;}));}
# 606
struct _tuple22*Cyc_suck_line(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_line_rec(lexbuf,10);}
int Cyc_suck_macroname_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp85=lexstate;if(_tmp85 == 0){_LL1: _LL2:
# 604 "buildlib.cyl"
 Cyc_current_source=({struct _dyneither_ptr*_tmp86=_cycalloc(sizeof(*_tmp86));({struct _dyneither_ptr _tmp488=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmp86=_tmp488;});_tmp86;});
Cyc_current_line=(struct _dyneither_ptr)Cyc_strconcat((struct _dyneither_ptr)Cyc_current_line,(struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_current_source)));
return Cyc_suck_restofline(lexbuf);}else{_LL3: _LL4:
# 608
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_macroname_rec(lexbuf,lexstate);}_LL0:;}
# 611
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp489=({const char*_tmp87="some action didn't return!";_tag_dyneither(_tmp87,sizeof(char),27U);});_tmp88->f1=_tmp489;});_tmp88;}));}
# 613
int Cyc_suck_macroname(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_macroname_rec(lexbuf,11);}
int Cyc_suck_restofline_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp89=lexstate;if(_tmp89 == 0){_LL1: _LL2:
# 611 "buildlib.cyl"
 Cyc_current_line=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp48A=(struct _dyneither_ptr)Cyc_current_line;Cyc_strconcat(_tmp48A,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});return 0;}else{_LL3: _LL4:
(lexbuf->refill_buff)(lexbuf);
return Cyc_suck_restofline_rec(lexbuf,lexstate);}_LL0:;}
# 615
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp48B=({const char*_tmp8A="some action didn't return!";_tag_dyneither(_tmp8A,sizeof(char),27U);});_tmp8B->f1=_tmp48B;});_tmp8B;}));}
# 617
int Cyc_suck_restofline(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_suck_restofline_rec(lexbuf,12);}
struct _tuple25*Cyc_spec_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp8C=lexstate;switch(_tmp8C){case 0U: _LL1: _LL2:
# 618 "buildlib.cyl"
 return Cyc_spec(lexbuf);case 1U: _LL3: _LL4:
# 620
 Cyc_current_headerfile=(struct _dyneither_ptr)({
struct _dyneither_ptr _tmp48D=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);Cyc_substring(_tmp48D,0,(unsigned long)(({
int _tmp48C=Cyc_Lexing_lexeme_end(lexbuf);_tmp48C - Cyc_Lexing_lexeme_start(lexbuf);})- 1));});
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
return({struct _tuple25*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->f1=Cyc_current_headerfile,_tmp8D->f2=Cyc_current_symbols,_tmp8D->f3=Cyc_current_user_defs,_tmp8D->f4=Cyc_current_omit_symbols,_tmp8D->f5=Cyc_current_hstubs,_tmp8D->f6=Cyc_current_cstubs,_tmp8D->f7=Cyc_current_cycstubs,_tmp8D->f8=Cyc_current_cpp;_tmp8D;});case 2U: _LL5: _LL6:
# 645
 return Cyc_spec(lexbuf);case 3U: _LL7: _LL8:
# 647
 return 0;case 4U: _LL9: _LLA:
# 649
({struct Cyc_Int_pa_PrintArg_struct _tmp90=({struct Cyc_Int_pa_PrintArg_struct _tmp3D8;_tmp3D8.tag=1U,({
# 651
unsigned long _tmp48E=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3D8.f1=_tmp48E;});_tmp3D8;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({struct Cyc___cycFILE*_tmp490=Cyc_stderr;struct _dyneither_ptr _tmp48F=({const char*_tmp8F="Error in .cys file: expected header file name, found '%c' instead\n";_tag_dyneither(_tmp8F,sizeof(char),67U);});Cyc_fprintf(_tmp490,_tmp48F,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});
return 0;default: _LLB: _LLC:
(lexbuf->refill_buff)(lexbuf);
return Cyc_spec_rec(lexbuf,lexstate);}_LL0:;}
# 656
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp491=({const char*_tmp91="some action didn't return!";_tag_dyneither(_tmp91,sizeof(char),27U);});_tmp92->f1=_tmp491;});_tmp92;}));}
# 658
struct _tuple25*Cyc_spec(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_spec_rec(lexbuf,13);}
int Cyc_commands_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmp93=lexstate;switch(_tmp93){case 0U: _LL1: _LL2:
# 656 "buildlib.cyl"
 return 0;case 1U: _LL3: _LL4:
# 658
 return 0;case 2U: _LL5: _LL6:
# 660
 Cyc_snarfed_symbols=0;
while(Cyc_snarfsymbols(lexbuf)){;}
Cyc_current_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_symbols);
return 1;case 3U: _LL7: _LL8:
# 665
 Cyc_snarfed_symbols=0;{
struct Cyc_List_List*_tmp94=Cyc_current_user_defs;
while(Cyc_snarfsymbols(lexbuf)){;}
if(_tmp94 != Cyc_current_user_defs){
({void*_tmp95=0U;({struct Cyc___cycFILE*_tmp493=Cyc_stderr;struct _dyneither_ptr _tmp492=({const char*_tmp96="Error in .cys file: got optional definition in omitsymbols\n";_tag_dyneither(_tmp96,sizeof(char),60U);});Cyc_fprintf(_tmp493,_tmp492,_tag_dyneither(_tmp95,sizeof(void*),0U));});});
# 671
return 0;}
# 673
Cyc_current_omit_symbols=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_snarfed_symbols,Cyc_current_omit_symbols);
return 1;};case 4U: _LL9: _LLA:
# 676
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp98=_cycalloc(sizeof(*_tmp98));({struct _dyneither_ptr _tmp495=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp98->f1=_tmp495;}),({
struct _dyneither_ptr _tmp494=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp98->f2=_tmp494;});_tmp98;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->hd=x,_tmp97->tl=Cyc_current_hstubs;_tmp97;});
return 1;};case 5U: _LLB: _LLC: {
# 684
struct _dyneither_ptr _tmp99=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmp99,sizeof(char),(int)Cyc_strlen(({const char*_tmp9A="hstub";_tag_dyneither(_tmp9A,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmp99,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmp99,sizeof(char),1);}{
struct _dyneither_ptr t=_tmp99;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmp9B=Cyc_substring((struct _dyneither_ptr)_tmp99,0,(unsigned long)((t.curr - _tmp99.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->f1=(struct _dyneither_ptr)_tmp9B,({
struct _dyneither_ptr _tmp496=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9D->f2=_tmp496;});_tmp9D;});
Cyc_current_hstubs=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->hd=x,_tmp9C->tl=Cyc_current_hstubs;_tmp9C;});
return 1;};};};}case 6U: _LLD: _LLE:
# 698
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmp9F=_cycalloc(sizeof(*_tmp9F));({struct _dyneither_ptr _tmp498=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmp9F->f1=_tmp498;}),({
struct _dyneither_ptr _tmp497=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmp9F->f2=_tmp497;});_tmp9F;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->hd=x,_tmp9E->tl=Cyc_current_cstubs;_tmp9E;});
return 1;};case 7U: _LLF: _LL10: {
# 706
struct _dyneither_ptr _tmpA0=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmpA0,sizeof(char),(int)Cyc_strlen(({const char*_tmpA1="cstub";_tag_dyneither(_tmpA1,sizeof(char),6U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmpA0,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmpA0,sizeof(char),1);}{
struct _dyneither_ptr t=_tmpA0;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmpA2=Cyc_substring((struct _dyneither_ptr)_tmpA0,0,(unsigned long)((t.curr - _tmpA0.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->f1=(struct _dyneither_ptr)_tmpA2,({
struct _dyneither_ptr _tmp499=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA4->f2=_tmp499;});_tmpA4;});
Cyc_current_cstubs=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->hd=x,_tmpA3->tl=Cyc_current_cstubs;_tmpA3;});
return 1;};};};}case 8U: _LL11: _LL12:
# 720
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _dyneither_ptr _tmp49B=(struct _dyneither_ptr)_tag_dyneither(0,0,0);_tmpA6->f1=_tmp49B;}),({
struct _dyneither_ptr _tmp49A=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpA6->f2=_tmp49A;});_tmpA6;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->hd=x,_tmpA5->tl=Cyc_current_cycstubs;_tmpA5;});
return 1;};case 9U: _LL13: _LL14: {
# 728
struct _dyneither_ptr _tmpA7=Cyc_Lexing_lexeme(lexbuf);
_dyneither_ptr_inplace_plus(& _tmpA7,sizeof(char),(int)Cyc_strlen(({const char*_tmpA8="cycstub";_tag_dyneither(_tmpA8,sizeof(char),8U);})));
while( isspace((int)*((char*)_check_dyneither_subscript(_tmpA7,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& _tmpA7,sizeof(char),1);}{
struct _dyneither_ptr t=_tmpA7;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}{
struct _dyneither_ptr _tmpA9=Cyc_substring((struct _dyneither_ptr)_tmpA7,0,(unsigned long)((t.curr - _tmpA7.curr)/ sizeof(char)));
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _tuple23*x=({struct _tuple23*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=(struct _dyneither_ptr)_tmpA9,({
struct _dyneither_ptr _tmp49C=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));_tmpAB->f2=_tmp49C;});_tmpAB;});
Cyc_current_cycstubs=({struct Cyc_List_List*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->hd=x,_tmpAA->tl=Cyc_current_cycstubs;_tmpAA;});
return 1;};};};}case 10U: _LL15: _LL16:
# 742
 Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}{
struct _dyneither_ptr*x=({struct _dyneither_ptr*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct _dyneither_ptr _tmp49D=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpAD=_tmp49D;});_tmpAD;});
Cyc_current_cpp=({struct Cyc_List_List*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->hd=x,_tmpAC->tl=Cyc_current_cpp;_tmpAC;});
return 1;};case 11U: _LL17: _LL18:
# 749
 return 1;case 12U: _LL19: _LL1A:
# 751
 return 1;case 13U: _LL1B: _LL1C:
# 753
({struct Cyc_Int_pa_PrintArg_struct _tmpB0=({struct Cyc_Int_pa_PrintArg_struct _tmp3D9;_tmp3D9.tag=1U,({
# 755
unsigned long _tmp49E=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3D9.f1=_tmp49E;});_tmp3D9;});void*_tmpAE[1U];_tmpAE[0]=& _tmpB0;({struct Cyc___cycFILE*_tmp4A0=Cyc_stderr;struct _dyneither_ptr _tmp49F=({const char*_tmpAF="Error in .cys file: expected command, found '%c' instead\n";_tag_dyneither(_tmpAF,sizeof(char),58U);});Cyc_fprintf(_tmp4A0,_tmp49F,_tag_dyneither(_tmpAE,sizeof(void*),1U));});});
return 0;default: _LL1D: _LL1E:
(lexbuf->refill_buff)(lexbuf);
return Cyc_commands_rec(lexbuf,lexstate);}_LL0:;}
# 760
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp4A1=({const char*_tmpB1="some action didn't return!";_tag_dyneither(_tmpB1,sizeof(char),27U);});_tmpB2->f1=_tmp4A1;});_tmpB2;}));}
# 762
int Cyc_commands(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_commands_rec(lexbuf,14);}
int Cyc_snarfsymbols_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpB3=lexstate;switch(_tmpB3){case 0U: _LL1: _LL2:
# 766 "buildlib.cyl"
 Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _dyneither_ptr*_tmp4A3=({struct _dyneither_ptr*_tmpB4=_cycalloc(sizeof(*_tmpB4));({struct _dyneither_ptr _tmp4A2=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);*_tmpB4=_tmp4A2;});_tmpB4;});_tmpB5->hd=_tmp4A3;}),_tmpB5->tl=Cyc_snarfed_symbols;_tmpB5;});
return 1;case 1U: _LL3: _LL4: {
# 769
struct _dyneither_ptr _tmpB6=Cyc_Lexing_lexeme(lexbuf);
struct _dyneither_ptr t=_tmpB6;
while(! isspace((int)*((char*)_check_dyneither_subscript(t,sizeof(char),0U)))){_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
Cyc_current_symbol=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)_tmpB6,0,(unsigned long)((t.curr - _tmpB6.curr)/ sizeof(char)));
Cyc_rename_current_symbol=1;
Cyc_braces_to_match=1;
Cyc_specbuf=Cyc_Buffer_create(255U);
while(Cyc_block(lexbuf)){;}
# 778
Cyc_rename_current_symbol=0;{
struct _tuple24*user_def=({struct _tuple24*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct _dyneither_ptr*_tmp4A6=({struct _dyneither_ptr*_tmpBA=_cycalloc(sizeof(*_tmpBA));*_tmpBA=Cyc_current_symbol;_tmpBA;});_tmpBC->f1=_tmp4A6;}),({
struct _dyneither_ptr*_tmp4A5=({struct _dyneither_ptr*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct _dyneither_ptr _tmp4A4=(struct _dyneither_ptr)Cyc_Buffer_contents((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf));*_tmpBB=_tmp4A4;});_tmpBB;});_tmpBC->f2=_tmp4A5;});_tmpBC;});
Cyc_snarfed_symbols=({struct Cyc_List_List*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct _dyneither_ptr*_tmp4A7=({struct _dyneither_ptr*_tmpB7=_cycalloc(sizeof(*_tmpB7));*_tmpB7=(struct _dyneither_ptr)Cyc_current_symbol;_tmpB7;});_tmpB8->hd=_tmp4A7;}),_tmpB8->tl=Cyc_snarfed_symbols;_tmpB8;});
Cyc_current_user_defs=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->hd=user_def,_tmpB9->tl=Cyc_current_user_defs;_tmpB9;});
return 1;};}case 2U: _LL5: _LL6:
# 785
 return 1;case 3U: _LL7: _LL8:
# 787
 return 0;case 4U: _LL9: _LLA:
# 789
({void*_tmpBD=0U;({struct Cyc___cycFILE*_tmp4A9=Cyc_stderr;struct _dyneither_ptr _tmp4A8=({const char*_tmpBE="Error in .cys file: unexpected end-of-file\n";_tag_dyneither(_tmpBE,sizeof(char),44U);});Cyc_fprintf(_tmp4A9,_tmp4A8,_tag_dyneither(_tmpBD,sizeof(void*),0U));});});
# 791
return 0;case 5U: _LLB: _LLC:
# 793
({struct Cyc_Int_pa_PrintArg_struct _tmpC1=({struct Cyc_Int_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=1U,({
# 795
unsigned long _tmp4AA=(unsigned long)((int)Cyc_Lexing_lexeme_char(lexbuf,0));_tmp3DA.f1=_tmp4AA;});_tmp3DA;});void*_tmpBF[1U];_tmpBF[0]=& _tmpC1;({struct Cyc___cycFILE*_tmp4AC=Cyc_stderr;struct _dyneither_ptr _tmp4AB=({const char*_tmpC0="Error in .cys file: expected symbol, found '%c' instead\n";_tag_dyneither(_tmpC0,sizeof(char),57U);});Cyc_fprintf(_tmp4AC,_tmp4AB,_tag_dyneither(_tmpBF,sizeof(void*),1U));});});
return 0;default: _LLD: _LLE:
(lexbuf->refill_buff)(lexbuf);
return Cyc_snarfsymbols_rec(lexbuf,lexstate);}_LL0:;}
# 800
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp4AD=({const char*_tmpC2="some action didn't return!";_tag_dyneither(_tmpC2,sizeof(char),27U);});_tmpC3->f1=_tmp4AD;});_tmpC3;}));}
# 802
int Cyc_snarfsymbols(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_snarfsymbols_rec(lexbuf,15);}
int Cyc_block_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpC4=lexstate;switch(_tmpC4){case 0U: _LL1: _LL2:
# 806 "buildlib.cyl"
({void*_tmpC5=0U;({struct _dyneither_ptr _tmp4AE=({const char*_tmpC6="Warning: unclosed brace\n";_tag_dyneither(_tmpC6,sizeof(char),25U);});Cyc_log(_tmp4AE,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 808
 if(Cyc_braces_to_match == 1)return 0;
-- Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'}');
return 1;case 2U: _LL5: _LL6:
# 813
 ++ Cyc_braces_to_match;
Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'{');
return 1;case 3U: _LL7: _LL8:
# 817
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');
while(Cyc_block_string(lexbuf)){;}
return 1;case 4U: _LL9: _LLA:
# 821
({struct Cyc_Buffer_t*_tmp4AF=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4AF,({const char*_tmpC7="/*";_tag_dyneither(_tmpC7,sizeof(char),3U);}));});
while(Cyc_block_comment(lexbuf)){;}
return 1;case 5U: _LLB: _LLC:
# 825
({struct Cyc_Buffer_t*_tmp4B0=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B0,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE: {
# 828
struct _dyneither_ptr _tmpC8=(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf);
if(Cyc_rename_current_symbol  && !Cyc_strcmp((struct _dyneither_ptr)_tmpC8,(struct _dyneither_ptr)Cyc_current_symbol))
({struct Cyc_Buffer_t*_tmp4B1=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B1,*Cyc_add_user_prefix(({struct _dyneither_ptr*_tmpC9=_cycalloc(sizeof(*_tmpC9));*_tmpC9=_tmpC8;_tmpC9;})));});else{
# 832
Cyc_Buffer_add_string((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),_tmpC8);}
return 1;}case 7U: _LLF: _LL10:
# 835
({struct Cyc_Buffer_t*_tmp4B2=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_char(_tmp4B2,Cyc_Lexing_lexeme_char(lexbuf,0));});
return 1;default: _LL11: _LL12:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_rec(lexbuf,lexstate);}_LL0:;}
# 840
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp4B3=({const char*_tmpCA="some action didn't return!";_tag_dyneither(_tmpCA,sizeof(char),27U);});_tmpCB->f1=_tmp4B3;});_tmpCB;}));}
# 842
int Cyc_block(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_rec(lexbuf,16);}
int Cyc_block_string_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpCC=lexstate;switch(_tmpCC){case 0U: _LL1: _LL2:
# 840 "buildlib.cyl"
({void*_tmpCD=0U;({struct _dyneither_ptr _tmp4B4=({const char*_tmpCE="Warning: unclosed string\n";_tag_dyneither(_tmpCE,sizeof(char),26U);});Cyc_log(_tmp4B4,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 842
 Cyc_Buffer_add_char((struct Cyc_Buffer_t*)_check_null(Cyc_specbuf),'"');return 0;case 2U: _LL5: _LL6:
# 844
({void*_tmpCF=0U;({struct _dyneither_ptr _tmp4B5=({const char*_tmpD0="Warning: unclosed string\n";_tag_dyneither(_tmpD0,sizeof(char),26U);});Cyc_log(_tmp4B5,_tag_dyneither(_tmpCF,sizeof(void*),0U));});});
({struct Cyc_Buffer_t*_tmp4B6=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B6,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 3U: _LL7: _LL8:
# 848
({struct Cyc_Buffer_t*_tmp4B7=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B7,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 4U: _LL9: _LLA:
# 851
({struct Cyc_Buffer_t*_tmp4B8=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B8,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 5U: _LLB: _LLC:
# 854
({struct Cyc_Buffer_t*_tmp4B9=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4B9,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 6U: _LLD: _LLE:
# 857
({struct Cyc_Buffer_t*_tmp4BA=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BA,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 7U: _LLF: _LL10:
# 860
({struct Cyc_Buffer_t*_tmp4BB=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BB,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;case 8U: _LL11: _LL12:
# 863
({struct Cyc_Buffer_t*_tmp4BC=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BC,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL13: _LL14:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_string_rec(lexbuf,lexstate);}_LL0:;}
# 868
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp4BD=({const char*_tmpD1="some action didn't return!";_tag_dyneither(_tmpD1,sizeof(char),27U);});_tmpD2->f1=_tmp4BD;});_tmpD2;}));}
# 870
int Cyc_block_string(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_string_rec(lexbuf,17);}
int Cyc_block_comment_rec(struct Cyc_Lexing_lexbuf*lexbuf,int lexstate){
lexstate=Cyc_lex_engine(lexstate,lexbuf);
{int _tmpD3=lexstate;switch(_tmpD3){case 0U: _LL1: _LL2:
# 868 "buildlib.cyl"
({void*_tmpD4=0U;({struct _dyneither_ptr _tmp4BE=({const char*_tmpD5="Warning: unclosed comment\n";_tag_dyneither(_tmpD5,sizeof(char),27U);});Cyc_log(_tmp4BE,_tag_dyneither(_tmpD4,sizeof(void*),0U));});});return 0;case 1U: _LL3: _LL4:
# 870
({struct Cyc_Buffer_t*_tmp4BF=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4BF,({const char*_tmpD6="*/";_tag_dyneither(_tmpD6,sizeof(char),3U);}));});return 0;case 2U: _LL5: _LL6:
# 872
({struct Cyc_Buffer_t*_tmp4C0=(struct Cyc_Buffer_t*)_check_null(Cyc_specbuf);Cyc_Buffer_add_string(_tmp4C0,(struct _dyneither_ptr)Cyc_Lexing_lexeme(lexbuf));});
return 1;default: _LL7: _LL8:
(lexbuf->refill_buff)(lexbuf);
return Cyc_block_comment_rec(lexbuf,lexstate);}_LL0:;}
# 877
(int)_throw((void*)({struct Cyc_Lexing_Error_exn_struct*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->tag=Cyc_Lexing_Error,({struct _dyneither_ptr _tmp4C1=({const char*_tmpD7="some action didn't return!";_tag_dyneither(_tmpD7,sizeof(char),27U);});_tmpD8->f1=_tmp4C1;});_tmpD8;}));}
# 879
int Cyc_block_comment(struct Cyc_Lexing_lexbuf*lexbuf){return Cyc_block_comment_rec(lexbuf,18);}
# 885 "buildlib.cyl"
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep);struct _tuple26{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
void Cyc_scan_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Hashtable_Table*dep){
void*_tmpD9=((struct Cyc_Absyn_Exp*)_check_null(e))->r;void*_tmpDA=_tmpD9;struct Cyc_List_List*_tmp129;void*_tmp128;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct _dyneither_ptr*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct _dyneither_ptr*_tmp123;void*_tmp122;void*_tmp121;struct Cyc_Absyn_Exp*_tmp120;int _tmp11F;struct Cyc_Absyn_Exp*_tmp11E;void**_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;void*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_List_List*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_List_List*_tmp105;void*_tmp104;switch(*((int*)_tmpDA)){case 1U: _LL1: _tmp104=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL2: {
# 889
struct _dyneither_ptr*_tmpDB=(*Cyc_Absyn_binding2qvar(_tmp104)).f2;
Cyc_add_target(_tmpDB);
return;}case 3U: _LL3: _tmp105=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL4:
# 893
 for(0;_tmp105 != 0;_tmp105=_tmp105->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp105->hd,dep);}
# 896
return;case 23U: _LL5: _tmp107=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp106=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL6:
# 898
 _tmp109=_tmp107;_tmp108=_tmp106;goto _LL8;case 9U: _LL7: _tmp109=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp108=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL8:
# 900
 _tmp10B=_tmp109;_tmp10A=_tmp108;goto _LLA;case 4U: _LL9: _tmp10B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp10A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;_LLA:
# 902
 Cyc_scan_exp(_tmp10B,dep);
Cyc_scan_exp(_tmp10A,dep);
return;case 41U: _LLB: _tmp10C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LLC:
 _tmp10D=_tmp10C;goto _LLE;case 20U: _LLD: _tmp10D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LLE:
# 907
 _tmp10E=_tmp10D;goto _LL10;case 18U: _LLF: _tmp10E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL10:
# 909
 _tmp10F=_tmp10E;goto _LL12;case 15U: _LL11: _tmp10F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL12:
# 911
 _tmp110=_tmp10F;goto _LL14;case 5U: _LL13: _tmp110=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL14:
# 913
 Cyc_scan_exp(_tmp110,dep);
return;case 6U: _LL15: _tmp113=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp112=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_tmp111=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDA)->f3;_LL16:
# 916
 Cyc_scan_exp(_tmp113,dep);
Cyc_scan_exp(_tmp112,dep);
Cyc_scan_exp(_tmp111,dep);
return;case 7U: _LL17: _tmp115=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp114=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL18:
 _tmp117=_tmp115;_tmp116=_tmp114;goto _LL1A;case 8U: _LL19: _tmp117=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp116=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1A:
# 922
 Cyc_scan_exp(_tmp117,dep);
Cyc_scan_exp(_tmp116,dep);
return;case 10U: _LL1B: _tmp119=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp118=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1C:
# 926
 Cyc_scan_exp(_tmp119,dep);
for(0;_tmp118 != 0;_tmp118=_tmp118->tl){
Cyc_scan_exp((struct Cyc_Absyn_Exp*)_tmp118->hd,dep);}
# 930
return;case 14U: _LL1D: _tmp11B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp11A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL1E:
# 932
 Cyc_scan_type(_tmp11B,dep);
Cyc_scan_exp(_tmp11A,dep);
return;case 34U: _LL1F: _tmp11F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).is_calloc;_tmp11E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).rgn;_tmp11D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).elt_type;_tmp11C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDA)->f1).num_elts;_LL20:
# 936
 if(_tmp11E != 0)Cyc_scan_exp(_tmp11E,dep);
if(_tmp11D != 0)Cyc_scan_type(*_tmp11D,dep);
Cyc_scan_exp(_tmp11C,dep);
return;case 38U: _LL21: _tmp120=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL22:
# 941
 Cyc_scan_exp(_tmp120,dep);return;case 39U: _LL23: _tmp121=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL24:
 _tmp122=_tmp121;goto _LL26;case 17U: _LL25: _tmp122=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_LL26:
# 944
 Cyc_scan_type(_tmp122,dep);
return;case 21U: _LL27: _tmp124=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp123=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL28:
# 947
 _tmp126=_tmp124;_tmp125=_tmp123;goto _LL2A;case 22U: _LL29: _tmp126=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp125=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL2A:
# 949
 Cyc_scan_exp(_tmp126,dep);
Cyc_add_target(_tmp125);
return;case 19U: _LL2B: _tmp128=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f1;_tmp127=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL2C:
# 953
 Cyc_scan_type(_tmp128,dep);
# 955
{void*_tmpDC=(void*)((struct Cyc_List_List*)_check_null(_tmp127))->hd;void*_tmpDD=_tmpDC;struct _dyneither_ptr*_tmpDE;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpDD)->tag == 0U){_LL56: _tmpDE=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmpDD)->f1;_LL57:
 Cyc_add_target(_tmpDE);goto _LL55;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}
# 959
return;case 0U: _LL2D: _LL2E:
# 961
 return;case 36U: _LL2F: _tmp129=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDA)->f2;_LL30:
# 963
 for(0;_tmp129 != 0;_tmp129=_tmp129->tl){
struct _tuple26*_tmpDF=(struct _tuple26*)_tmp129->hd;struct _tuple26*_tmpE0=_tmpDF;struct Cyc_Absyn_Exp*_tmpE1;_LL5B: _tmpE1=_tmpE0->f2;_LL5C:;
Cyc_scan_exp(_tmpE1,dep);}
# 967
return;case 40U: _LL31: _LL32:
 return;case 2U: _LL33: _LL34:
# 970
({void*_tmpE2=0U;({struct Cyc___cycFILE*_tmp4C3=Cyc_stderr;struct _dyneither_ptr _tmp4C2=({const char*_tmpE3="Error: unexpected Pragma_e\n";_tag_dyneither(_tmpE3,sizeof(char),28U);});Cyc_fprintf(_tmp4C3,_tmp4C2,_tag_dyneither(_tmpE2,sizeof(void*),0U));});});
 exit(1);return;case 35U: _LL35: _LL36:
# 973
({void*_tmpE4=0U;({struct Cyc___cycFILE*_tmp4C5=Cyc_stderr;struct _dyneither_ptr _tmp4C4=({const char*_tmpE5="Error: unexpected Swap_e\n";_tag_dyneither(_tmpE5,sizeof(char),26U);});Cyc_fprintf(_tmp4C5,_tmp4C4,_tag_dyneither(_tmpE4,sizeof(void*),0U));});});
 exit(1);return;case 37U: _LL37: _LL38:
# 976
({void*_tmpE6=0U;({struct Cyc___cycFILE*_tmp4C7=Cyc_stderr;struct _dyneither_ptr _tmp4C6=({const char*_tmpE7="Error: unexpected Stmt_e\n";_tag_dyneither(_tmpE7,sizeof(char),26U);});Cyc_fprintf(_tmp4C7,_tmp4C6,_tag_dyneither(_tmpE6,sizeof(void*),0U));});});
 exit(1);return;case 11U: _LL39: _LL3A:
# 979
({void*_tmpE8=0U;({struct Cyc___cycFILE*_tmp4C9=Cyc_stderr;struct _dyneither_ptr _tmp4C8=({const char*_tmpE9="Error: unexpected Throw_e\n";_tag_dyneither(_tmpE9,sizeof(char),27U);});Cyc_fprintf(_tmp4C9,_tmp4C8,_tag_dyneither(_tmpE8,sizeof(void*),0U));});});
 exit(1);return;case 12U: _LL3B: _LL3C:
# 982
({void*_tmpEA=0U;({struct Cyc___cycFILE*_tmp4CB=Cyc_stderr;struct _dyneither_ptr _tmp4CA=({const char*_tmpEB="Error: unexpected NoInstantiate_e\n";_tag_dyneither(_tmpEB,sizeof(char),35U);});Cyc_fprintf(_tmp4CB,_tmp4CA,_tag_dyneither(_tmpEA,sizeof(void*),0U));});});
 exit(1);return;case 13U: _LL3D: _LL3E:
# 985
({void*_tmpEC=0U;({struct Cyc___cycFILE*_tmp4CD=Cyc_stderr;struct _dyneither_ptr _tmp4CC=({const char*_tmpED="Error: unexpected Instantiate_e\n";_tag_dyneither(_tmpED,sizeof(char),33U);});Cyc_fprintf(_tmp4CD,_tmp4CC,_tag_dyneither(_tmpEC,sizeof(void*),0U));});});
 exit(1);return;case 16U: _LL3F: _LL40:
# 988
({void*_tmpEE=0U;({struct Cyc___cycFILE*_tmp4CF=Cyc_stderr;struct _dyneither_ptr _tmp4CE=({const char*_tmpEF="Error: unexpected New_e\n";_tag_dyneither(_tmpEF,sizeof(char),25U);});Cyc_fprintf(_tmp4CF,_tmp4CE,_tag_dyneither(_tmpEE,sizeof(void*),0U));});});
 exit(1);return;case 24U: _LL41: _LL42:
# 991
({void*_tmpF0=0U;({struct Cyc___cycFILE*_tmp4D1=Cyc_stderr;struct _dyneither_ptr _tmp4D0=({const char*_tmpF1="Error: unexpected Tuple_e\n";_tag_dyneither(_tmpF1,sizeof(char),27U);});Cyc_fprintf(_tmp4D1,_tmp4D0,_tag_dyneither(_tmpF0,sizeof(void*),0U));});});
 exit(1);return;case 25U: _LL43: _LL44:
# 994
({void*_tmpF2=0U;({struct Cyc___cycFILE*_tmp4D3=Cyc_stderr;struct _dyneither_ptr _tmp4D2=({const char*_tmpF3="Error: unexpected CompoundLit_e\n";_tag_dyneither(_tmpF3,sizeof(char),33U);});Cyc_fprintf(_tmp4D3,_tmp4D2,_tag_dyneither(_tmpF2,sizeof(void*),0U));});});
 exit(1);return;case 26U: _LL45: _LL46:
# 997
({void*_tmpF4=0U;({struct Cyc___cycFILE*_tmp4D5=Cyc_stderr;struct _dyneither_ptr _tmp4D4=({const char*_tmpF5="Error: unexpected Array_e\n";_tag_dyneither(_tmpF5,sizeof(char),27U);});Cyc_fprintf(_tmp4D5,_tmp4D4,_tag_dyneither(_tmpF4,sizeof(void*),0U));});});
 exit(1);return;case 27U: _LL47: _LL48:
# 1000
({void*_tmpF6=0U;({struct Cyc___cycFILE*_tmp4D7=Cyc_stderr;struct _dyneither_ptr _tmp4D6=({const char*_tmpF7="Error: unexpected Comprehension_e\n";_tag_dyneither(_tmpF7,sizeof(char),35U);});Cyc_fprintf(_tmp4D7,_tmp4D6,_tag_dyneither(_tmpF6,sizeof(void*),0U));});});
 exit(1);return;case 28U: _LL49: _LL4A:
# 1003
({void*_tmpF8=0U;({struct Cyc___cycFILE*_tmp4D9=Cyc_stderr;struct _dyneither_ptr _tmp4D8=({const char*_tmpF9="Error: unexpected ComprehensionNoinit_e\n";_tag_dyneither(_tmpF9,sizeof(char),41U);});Cyc_fprintf(_tmp4D9,_tmp4D8,_tag_dyneither(_tmpF8,sizeof(void*),0U));});});
 exit(1);return;case 29U: _LL4B: _LL4C:
# 1006
({void*_tmpFA=0U;({struct Cyc___cycFILE*_tmp4DB=Cyc_stderr;struct _dyneither_ptr _tmp4DA=({const char*_tmpFB="Error: unexpected Aggregate_e\n";_tag_dyneither(_tmpFB,sizeof(char),31U);});Cyc_fprintf(_tmp4DB,_tmp4DA,_tag_dyneither(_tmpFA,sizeof(void*),0U));});});
 exit(1);return;case 30U: _LL4D: _LL4E:
# 1009
({void*_tmpFC=0U;({struct Cyc___cycFILE*_tmp4DD=Cyc_stderr;struct _dyneither_ptr _tmp4DC=({const char*_tmpFD="Error: unexpected AnonStruct_e\n";_tag_dyneither(_tmpFD,sizeof(char),32U);});Cyc_fprintf(_tmp4DD,_tmp4DC,_tag_dyneither(_tmpFC,sizeof(void*),0U));});});
 exit(1);return;case 31U: _LL4F: _LL50:
# 1012
({void*_tmpFE=0U;({struct Cyc___cycFILE*_tmp4DF=Cyc_stderr;struct _dyneither_ptr _tmp4DE=({const char*_tmpFF="Error: unexpected Datatype_e\n";_tag_dyneither(_tmpFF,sizeof(char),30U);});Cyc_fprintf(_tmp4DF,_tmp4DE,_tag_dyneither(_tmpFE,sizeof(void*),0U));});});
 exit(1);return;case 32U: _LL51: _LL52:
# 1015
({void*_tmp100=0U;({struct Cyc___cycFILE*_tmp4E1=Cyc_stderr;struct _dyneither_ptr _tmp4E0=({const char*_tmp101="Error: unexpected Enum_e\n";_tag_dyneither(_tmp101,sizeof(char),26U);});Cyc_fprintf(_tmp4E1,_tmp4E0,_tag_dyneither(_tmp100,sizeof(void*),0U));});});
 exit(1);return;default: _LL53: _LL54:
# 1018
({void*_tmp102=0U;({struct Cyc___cycFILE*_tmp4E3=Cyc_stderr;struct _dyneither_ptr _tmp4E2=({const char*_tmp103="Error: unexpected AnonEnum_e\n";_tag_dyneither(_tmp103,sizeof(char),30U);});Cyc_fprintf(_tmp4E3,_tmp4E2,_tag_dyneither(_tmp102,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1023
void Cyc_scan_exp_opt(struct Cyc_Absyn_Exp*eo,struct Cyc_Hashtable_Table*dep){
if((unsigned int)eo)Cyc_scan_exp(eo,dep);
return;}
# 1028
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep);
void Cyc_scan_type(void*t,struct Cyc_Hashtable_Table*dep){
void*_tmp12A=t;struct Cyc_Absyn_Datatypedecl*_tmp156;struct Cyc_Absyn_Enumdecl*_tmp155;struct Cyc_Absyn_Aggrdecl*_tmp154;struct _dyneither_ptr*_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_FnInfo _tmp151;struct Cyc_Absyn_Exp*_tmp150;void*_tmp14F;struct Cyc_Absyn_Exp*_tmp14E;void*_tmp14D;struct Cyc_Absyn_PtrInfo _tmp14C;void*_tmp14B;struct Cyc_List_List*_tmp14A;switch(*((int*)_tmp12A)){case 0U: _LL1: _tmp14B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12A)->f1;_tmp14A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp12A)->f2;_LL2: {
# 1032
void*_tmp12B=_tmp14B;struct _dyneither_ptr*_tmp133;union Cyc_Absyn_AggrInfo _tmp132;switch(*((int*)_tmp12B)){case 0U: _LL1E: _LL1F:
 goto _LL21;case 1U: _LL20: _LL21:
 goto _LL23;case 17U: _LL22: _LL23:
 goto _LL25;case 2U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
# 1038
 return;case 20U: _LL28: _tmp132=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp12B)->f1;_LL29: {
# 1040
struct _tuple11 _tmp12C=Cyc_Absyn_aggr_kinded_name(_tmp132);struct _tuple11 _tmp12D=_tmp12C;struct _dyneither_ptr*_tmp12E;_LL49: _tmp12E=(_tmp12D.f2)->f2;_LL4A:;
_tmp133=_tmp12E;goto _LL2B;}case 15U: _LL2A: _tmp133=(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp12B)->f1)->f2;_LL2B:
 Cyc_add_target(_tmp133);return;case 18U: _LL2C: _LL2D:
 goto _LL2F;case 19U: _LL2E: _LL2F: goto _LL31;case 3U: _LL30: _LL31:
 goto _LL33;case 5U: _LL32: _LL33: goto _LL35;case 6U: _LL34: _LL35:
 goto _LL37;case 7U: _LL36: _LL37: goto _LL39;case 8U: _LL38: _LL39:
 goto _LL3B;case 9U: _LL3A: _LL3B: goto _LL3D;case 10U: _LL3C: _LL3D:
 goto _LL3F;case 4U: _LL3E: _LL3F: goto _LL41;case 11U: _LL40: _LL41:
 goto _LL43;case 12U: _LL42: _LL43: goto _LL45;case 13U: _LL44: _LL45:
 goto _LL47;default: _LL46: _LL47:
({struct Cyc_String_pa_PrintArg_struct _tmp131=({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0U,({struct _dyneither_ptr _tmp4E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp3DB.f1=_tmp4E4;});_tmp3DB;});void*_tmp12F[1U];_tmp12F[0]=& _tmp131;({struct Cyc___cycFILE*_tmp4E6=Cyc_stderr;struct _dyneither_ptr _tmp4E5=({const char*_tmp130="Error: unexpected %s\n";_tag_dyneither(_tmp130,sizeof(char),22U);});Cyc_fprintf(_tmp4E6,_tmp4E5,_tag_dyneither(_tmp12F,sizeof(void*),1U));});});
 exit(1);return;}_LL1D:;}case 3U: _LL3: _tmp14C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12A)->f1;_LL4:
# 1055
 Cyc_scan_type(_tmp14C.elt_type,dep);
return;case 4U: _LL5: _tmp14F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12A)->f1).elt_type;_tmp14E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12A)->f1).num_elts;_tmp14D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12A)->f1).zero_term;_LL6:
# 1058
 Cyc_scan_type(_tmp14F,dep);
Cyc_scan_exp_opt(_tmp14E,dep);
return;case 11U: _LL7: _tmp150=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp12A)->f1;_LL8:
# 1062
 Cyc_scan_exp(_tmp150,dep);
return;case 5U: _LL9: _tmp151=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp12A)->f1;_LLA:
# 1065
 Cyc_scan_type(_tmp151.ret_type,dep);
{struct Cyc_List_List*_tmp134=_tmp151.args;for(0;_tmp134 != 0;_tmp134=_tmp134->tl){
struct _tuple9*_tmp135=(struct _tuple9*)_tmp134->hd;struct _tuple9*_tmp136=_tmp135;void*_tmp137;_LL4C: _tmp137=_tmp136->f3;_LL4D:;
Cyc_scan_type(_tmp137,dep);}}
# 1070
if(_tmp151.cyc_varargs != 0)
Cyc_scan_type((_tmp151.cyc_varargs)->type,dep);
return;case 7U: _LLB: _tmp152=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12A)->f2;_LLC:
# 1074
 for(0;_tmp152 != 0;_tmp152=_tmp152->tl){
Cyc_scan_type(((struct Cyc_Absyn_Aggrfield*)_tmp152->hd)->type,dep);
Cyc_scan_exp_opt(((struct Cyc_Absyn_Aggrfield*)_tmp152->hd)->width,dep);}
# 1078
return;case 8U: _LLD: _tmp153=(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp12A)->f1)->f2;_LLE:
# 1080
 Cyc_add_target(_tmp153);
return;case 10U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12A)->f1)->r)){case 0U: _LLF: _tmp154=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12A)->f1)->r)->f1;_LL10:
# 1084
({struct Cyc_Absyn_Decl*_tmp4E7=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->tag=5U,_tmp138->f1=_tmp154;_tmp138;}),0U);Cyc_scan_decl(_tmp4E7,dep);});{
struct _tuple1*_tmp139=_tmp154->name;struct _tuple1*_tmp13A=_tmp139;struct _dyneither_ptr*_tmp13B;_LL4F: _tmp13B=_tmp13A->f2;_LL50:;
Cyc_add_target(_tmp13B);
return;};case 1U: _LL11: _tmp155=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12A)->f1)->r)->f1;_LL12:
# 1090
({struct Cyc_Absyn_Decl*_tmp4E8=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=7U,_tmp13C->f1=_tmp155;_tmp13C;}),0U);Cyc_scan_decl(_tmp4E8,dep);});{
struct _tuple1*_tmp13D=_tmp155->name;struct _tuple1*_tmp13E=_tmp13D;struct _dyneither_ptr*_tmp13F;_LL52: _tmp13F=_tmp13E->f2;_LL53:;
Cyc_add_target(_tmp13F);
return;};default: _LL13: _tmp156=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp12A)->f1)->r)->f1;_LL14:
# 1096
({void*_tmp140=0U;({struct Cyc___cycFILE*_tmp4EA=Cyc_stderr;struct _dyneither_ptr _tmp4E9=({const char*_tmp141="Error: unexpected Datatype declaration\n";_tag_dyneither(_tmp141,sizeof(char),40U);});Cyc_fprintf(_tmp4EA,_tmp4E9,_tag_dyneither(_tmp140,sizeof(void*),0U));});});
 exit(1);return;}case 1U: _LL15: _LL16:
# 1099
({void*_tmp142=0U;({struct Cyc___cycFILE*_tmp4EC=Cyc_stderr;struct _dyneither_ptr _tmp4EB=({const char*_tmp143="Error: unexpected Evar\n";_tag_dyneither(_tmp143,sizeof(char),24U);});Cyc_fprintf(_tmp4EC,_tmp4EB,_tag_dyneither(_tmp142,sizeof(void*),0U));});});
 exit(1);return;case 2U: _LL17: _LL18:
# 1102
({void*_tmp144=0U;({struct Cyc___cycFILE*_tmp4EE=Cyc_stderr;struct _dyneither_ptr _tmp4ED=({const char*_tmp145="Error: unexpected VarType\n";_tag_dyneither(_tmp145,sizeof(char),27U);});Cyc_fprintf(_tmp4EE,_tmp4ED,_tag_dyneither(_tmp144,sizeof(void*),0U));});});
 exit(1);return;case 6U: _LL19: _LL1A:
# 1105
({void*_tmp146=0U;({struct Cyc___cycFILE*_tmp4F0=Cyc_stderr;struct _dyneither_ptr _tmp4EF=({const char*_tmp147="Error: unexpected TupleType\n";_tag_dyneither(_tmp147,sizeof(char),29U);});Cyc_fprintf(_tmp4F0,_tmp4EF,_tag_dyneither(_tmp146,sizeof(void*),0U));});});
 exit(1);return;default: _LL1B: _LL1C:
# 1108
({void*_tmp148=0U;({struct Cyc___cycFILE*_tmp4F2=Cyc_stderr;struct _dyneither_ptr _tmp4F1=({const char*_tmp149="Error: unexpected valueof_t\n";_tag_dyneither(_tmp149,sizeof(char),29U);});Cyc_fprintf(_tmp4F2,_tmp4F1,_tag_dyneither(_tmp148,sizeof(void*),0U));});});
 exit(1);return;}_LL0:;}
# 1113
void Cyc_scan_decl(struct Cyc_Absyn_Decl*d,struct Cyc_Hashtable_Table*dep){
struct Cyc_Set_Set**_tmp157=Cyc_current_targets;
struct _dyneither_ptr*_tmp158=Cyc_current_source;
Cyc_current_targets=({struct Cyc_Set_Set**_tmp159=_cycalloc(sizeof(*_tmp159));({struct Cyc_Set_Set*_tmp4F3=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);*_tmp159=_tmp4F3;});_tmp159;});
{void*_tmp15A=d->r;void*_tmp15B=_tmp15A;struct Cyc_Absyn_Typedefdecl*_tmp193;struct Cyc_Absyn_Enumdecl*_tmp192;struct Cyc_Absyn_Aggrdecl*_tmp191;struct Cyc_Absyn_Fndecl*_tmp190;struct Cyc_Absyn_Vardecl*_tmp18F;switch(*((int*)_tmp15B)){case 0U: _LL1: _tmp18F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp15B)->f1;_LL2: {
# 1119
struct _tuple1*_tmp15C=_tmp18F->name;struct _tuple1*_tmp15D=_tmp15C;struct _dyneither_ptr*_tmp15E;_LL24: _tmp15E=_tmp15D->f2;_LL25:;
Cyc_current_source=_tmp15E;
Cyc_scan_type(_tmp18F->type,dep);
Cyc_scan_exp_opt(_tmp18F->initializer,dep);
goto _LL0;}case 1U: _LL3: _tmp190=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp15B)->f1;_LL4: {
# 1125
struct _tuple1*_tmp15F=_tmp190->name;struct _tuple1*_tmp160=_tmp15F;struct _dyneither_ptr*_tmp167;_LL27: _tmp167=_tmp160->f2;_LL28:;
Cyc_current_source=_tmp167;
Cyc_scan_type((_tmp190->i).ret_type,dep);
{struct Cyc_List_List*_tmp161=(_tmp190->i).args;for(0;_tmp161 != 0;_tmp161=_tmp161->tl){
struct _tuple9*_tmp162=(struct _tuple9*)_tmp161->hd;struct _tuple9*_tmp163=_tmp162;void*_tmp164;_LL2A: _tmp164=_tmp163->f3;_LL2B:;
Cyc_scan_type(_tmp164,dep);}}
# 1132
if((_tmp190->i).cyc_varargs != 0)
Cyc_scan_type(((struct Cyc_Absyn_VarargInfo*)_check_null((_tmp190->i).cyc_varargs))->type,dep);
if(_tmp190->is_inline)
({void*_tmp165=0U;({struct Cyc___cycFILE*_tmp4F5=Cyc_stderr;struct _dyneither_ptr _tmp4F4=({const char*_tmp166="Warning: ignoring inline function\n";_tag_dyneither(_tmp166,sizeof(char),35U);});Cyc_fprintf(_tmp4F5,_tmp4F4,_tag_dyneither(_tmp165,sizeof(void*),0U));});});
goto _LL0;}case 5U: _LL5: _tmp191=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp15B)->f1;_LL6: {
# 1138
struct _tuple1*_tmp168=_tmp191->name;struct _tuple1*_tmp169=_tmp168;struct _dyneither_ptr*_tmp16D;_LL2D: _tmp16D=_tmp169->f2;_LL2E:;
Cyc_current_source=_tmp16D;
if((unsigned int)_tmp191->impl){
{struct Cyc_List_List*_tmp16A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp191->impl))->fields;for(0;_tmp16A != 0;_tmp16A=_tmp16A->tl){
struct Cyc_Absyn_Aggrfield*_tmp16B=(struct Cyc_Absyn_Aggrfield*)_tmp16A->hd;
Cyc_scan_type(_tmp16B->type,dep);
Cyc_scan_exp_opt(_tmp16B->width,dep);}}{
# 1148
struct Cyc_List_List*_tmp16C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp191->impl))->fields;for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){;}};}
# 1152
goto _LL0;}case 7U: _LL7: _tmp192=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp15B)->f1;_LL8: {
# 1154
struct _tuple1*_tmp16E=_tmp192->name;struct _tuple1*_tmp16F=_tmp16E;struct _dyneither_ptr*_tmp173;_LL30: _tmp173=_tmp16F->f2;_LL31:;
Cyc_current_source=_tmp173;
if((unsigned int)_tmp192->fields){
{struct Cyc_List_List*_tmp170=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp192->fields))->v;for(0;_tmp170 != 0;_tmp170=_tmp170->tl){
struct Cyc_Absyn_Enumfield*_tmp171=(struct Cyc_Absyn_Enumfield*)_tmp170->hd;
Cyc_scan_exp_opt(_tmp171->tag,dep);}}{
# 1163
struct Cyc_List_List*_tmp172=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp192->fields))->v;for(0;_tmp172 != 0;_tmp172=_tmp172->tl){;}};}
# 1167
goto _LL0;}case 8U: _LL9: _tmp193=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp15B)->f1;_LLA: {
# 1169
struct _tuple1*_tmp174=_tmp193->name;struct _tuple1*_tmp175=_tmp174;struct _dyneither_ptr*_tmp176;_LL33: _tmp176=_tmp175->f2;_LL34:;
Cyc_current_source=_tmp176;
if((unsigned int)_tmp193->defn)
Cyc_scan_type((void*)_check_null(_tmp193->defn),dep);
goto _LL0;}case 4U: _LLB: _LLC:
# 1175
({void*_tmp177=0U;({struct Cyc___cycFILE*_tmp4F7=Cyc_stderr;struct _dyneither_ptr _tmp4F6=({const char*_tmp178="Error: unexpected region declaration";_tag_dyneither(_tmp178,sizeof(char),37U);});Cyc_fprintf(_tmp4F7,_tmp4F6,_tag_dyneither(_tmp177,sizeof(void*),0U));});});
 exit(1);case 13U: _LLD: _LLE:
# 1178
({void*_tmp179=0U;({struct Cyc___cycFILE*_tmp4F9=Cyc_stderr;struct _dyneither_ptr _tmp4F8=({const char*_tmp17A="Error: unexpected __cyclone_port_on__";_tag_dyneither(_tmp17A,sizeof(char),38U);});Cyc_fprintf(_tmp4F9,_tmp4F8,_tag_dyneither(_tmp179,sizeof(void*),0U));});});
 exit(1);case 14U: _LLF: _LL10:
# 1181
({void*_tmp17B=0U;({struct Cyc___cycFILE*_tmp4FB=Cyc_stderr;struct _dyneither_ptr _tmp4FA=({const char*_tmp17C="Error: unexpected __cyclone_port_off__";_tag_dyneither(_tmp17C,sizeof(char),39U);});Cyc_fprintf(_tmp4FB,_tmp4FA,_tag_dyneither(_tmp17B,sizeof(void*),0U));});});
 exit(1);case 15U: _LL11: _LL12:
# 1184
({void*_tmp17D=0U;({struct Cyc___cycFILE*_tmp4FD=Cyc_stderr;struct _dyneither_ptr _tmp4FC=({const char*_tmp17E="Error: unexpected __tempest_on__";_tag_dyneither(_tmp17E,sizeof(char),33U);});Cyc_fprintf(_tmp4FD,_tmp4FC,_tag_dyneither(_tmp17D,sizeof(void*),0U));});});
 exit(1);case 16U: _LL13: _LL14:
# 1187
({void*_tmp17F=0U;({struct Cyc___cycFILE*_tmp4FF=Cyc_stderr;struct _dyneither_ptr _tmp4FE=({const char*_tmp180="Error: unexpected __tempest_off__";_tag_dyneither(_tmp180,sizeof(char),34U);});Cyc_fprintf(_tmp4FF,_tmp4FE,_tag_dyneither(_tmp17F,sizeof(void*),0U));});});
 exit(1);case 2U: _LL15: _LL16:
# 1190
({void*_tmp181=0U;({struct Cyc___cycFILE*_tmp501=Cyc_stderr;struct _dyneither_ptr _tmp500=({const char*_tmp182="Error: unexpected let declaration\n";_tag_dyneither(_tmp182,sizeof(char),35U);});Cyc_fprintf(_tmp501,_tmp500,_tag_dyneither(_tmp181,sizeof(void*),0U));});});
 exit(1);case 6U: _LL17: _LL18:
# 1193
({void*_tmp183=0U;({struct Cyc___cycFILE*_tmp503=Cyc_stderr;struct _dyneither_ptr _tmp502=({const char*_tmp184="Error: unexpected datatype declaration\n";_tag_dyneither(_tmp184,sizeof(char),40U);});Cyc_fprintf(_tmp503,_tmp502,_tag_dyneither(_tmp183,sizeof(void*),0U));});});
 exit(1);case 3U: _LL19: _LL1A:
# 1196
({void*_tmp185=0U;({struct Cyc___cycFILE*_tmp505=Cyc_stderr;struct _dyneither_ptr _tmp504=({const char*_tmp186="Error: unexpected let declaration\n";_tag_dyneither(_tmp186,sizeof(char),35U);});Cyc_fprintf(_tmp505,_tmp504,_tag_dyneither(_tmp185,sizeof(void*),0U));});});
 exit(1);case 9U: _LL1B: _LL1C:
# 1199
({void*_tmp187=0U;({struct Cyc___cycFILE*_tmp507=Cyc_stderr;struct _dyneither_ptr _tmp506=({const char*_tmp188="Error: unexpected namespace declaration\n";_tag_dyneither(_tmp188,sizeof(char),41U);});Cyc_fprintf(_tmp507,_tmp506,_tag_dyneither(_tmp187,sizeof(void*),0U));});});
 exit(1);case 10U: _LL1D: _LL1E:
# 1202
({void*_tmp189=0U;({struct Cyc___cycFILE*_tmp509=Cyc_stderr;struct _dyneither_ptr _tmp508=({const char*_tmp18A="Error: unexpected using declaration\n";_tag_dyneither(_tmp18A,sizeof(char),37U);});Cyc_fprintf(_tmp509,_tmp508,_tag_dyneither(_tmp189,sizeof(void*),0U));});});
 exit(1);case 11U: _LL1F: _LL20:
# 1205
({void*_tmp18B=0U;({struct Cyc___cycFILE*_tmp50B=Cyc_stderr;struct _dyneither_ptr _tmp50A=({const char*_tmp18C="Error: unexpected extern \"C\" declaration\n";_tag_dyneither(_tmp18C,sizeof(char),42U);});Cyc_fprintf(_tmp50B,_tmp50A,_tag_dyneither(_tmp18B,sizeof(void*),0U));});});
 exit(1);default: _LL21: _LL22:
# 1208
({void*_tmp18D=0U;({struct Cyc___cycFILE*_tmp50D=Cyc_stderr;struct _dyneither_ptr _tmp50C=({const char*_tmp18E="Error: unexpected extern \"C include\" declaration\n";_tag_dyneither(_tmp18E,sizeof(char),50U);});Cyc_fprintf(_tmp50D,_tmp50C,_tag_dyneither(_tmp18D,sizeof(void*),0U));});});
 exit(1);}_LL0:;}{
# 1216
struct Cyc_Set_Set*old;
struct _dyneither_ptr*_tmp194=(struct _dyneither_ptr*)_check_null(Cyc_current_source);
{struct _handler_cons _tmp195;_push_handler(& _tmp195);{int _tmp197=0;if(setjmp(_tmp195.handler))_tmp197=1;if(!_tmp197){
old=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(dep,_tmp194);;_pop_handler();}else{void*_tmp196=(void*)_exn_thrown;void*_tmp198=_tmp196;void*_tmp199;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp198)->tag == Cyc_Core_Not_found){_LL36: _LL37:
# 1221
 old=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);goto _LL35;}else{_LL38: _tmp199=_tmp198;_LL39:(int)_rethrow(_tmp199);}_LL35:;}};}{
# 1223
struct Cyc_Set_Set*_tmp19A=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(*((struct Cyc_Set_Set**)_check_null(Cyc_current_targets)),old);
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(dep,_tmp194,_tmp19A);
# 1226
Cyc_current_targets=_tmp157;
Cyc_current_source=_tmp158;};};}
# 1230
struct Cyc_Hashtable_Table*Cyc_new_deps(){
return((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),int(*hash)(struct _dyneither_ptr*)))Cyc_Hashtable_create)(107,Cyc_strptrcmp,Cyc_Hashtable_hash_stringptr);}
# 1234
struct Cyc_Set_Set*Cyc_find(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*x){
struct _handler_cons _tmp19B;_push_handler(& _tmp19B);{int _tmp19D=0;if(setjmp(_tmp19B.handler))_tmp19D=1;if(!_tmp19D){{struct Cyc_Set_Set*_tmp19E=((struct Cyc_Set_Set*(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key))Cyc_Hashtable_lookup)(t,x);_npop_handler(0U);return _tmp19E;};_pop_handler();}else{void*_tmp19C=(void*)_exn_thrown;void*_tmp19F=_tmp19C;void*_tmp1A0;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp19F)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 1237
 return((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);}else{_LL3: _tmp1A0=_tmp19F;_LL4:(int)_rethrow(_tmp1A0);}_LL0:;}};}
# 1241
struct Cyc_Set_Set*Cyc_reachable(struct Cyc_List_List*init,struct Cyc_Hashtable_Table*t){
# 1251 "buildlib.cyl"
struct Cyc_Set_Set*emptyset=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
struct Cyc_Set_Set*curr;
for(curr=emptyset;init != 0;init=init->tl){
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(curr,(struct _dyneither_ptr*)init->hd);}{
# 1256
struct Cyc_Set_Set*_tmp1A1=curr;
# 1258
struct _dyneither_ptr*_tmp1A2=({struct _dyneither_ptr*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));({struct _dyneither_ptr _tmp50E=({const char*_tmp1A5="";_tag_dyneither(_tmp1A5,sizeof(char),1U);});*_tmp1A6=_tmp50E;});_tmp1A6;});
while(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp1A1)> 0){
struct Cyc_Set_Set*_tmp1A3=emptyset;
struct Cyc_Iter_Iter _tmp1A4=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,_tmp1A1);
while(((int(*)(struct Cyc_Iter_Iter,struct _dyneither_ptr**))Cyc_Iter_next)(_tmp1A4,& _tmp1A2)){
_tmp1A3=({struct Cyc_Set_Set*_tmp50F=_tmp1A3;((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(_tmp50F,Cyc_find(t,_tmp1A2));});}
_tmp1A1=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_diff)(_tmp1A3,curr);
curr=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2))Cyc_Set_union_two)(curr,_tmp1A1);}
# 1267
return curr;};}
# 1270
enum Cyc_buildlib_mode{Cyc_NORMAL  = 0U,Cyc_GATHER  = 1U,Cyc_GATHERSCRIPT  = 2U,Cyc_FINISH  = 3U};
static enum Cyc_buildlib_mode Cyc_mode=Cyc_NORMAL;
static int Cyc_gathering(){
return(int)Cyc_mode == (int)Cyc_GATHER  || (int)Cyc_mode == (int)Cyc_GATHERSCRIPT;}
# 1276
static struct Cyc___cycFILE*Cyc_script_file=0;
int Cyc_prscript(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1280
if(Cyc_script_file == 0){
({void*_tmp1A7=0U;({struct Cyc___cycFILE*_tmp511=Cyc_stderr;struct _dyneither_ptr _tmp510=({const char*_tmp1A8="Internal error: script file is NULL\n";_tag_dyneither(_tmp1A8,sizeof(char),37U);});Cyc_fprintf(_tmp511,_tmp510,_tag_dyneither(_tmp1A7,sizeof(void*),0U));});});
 exit(1);}
# 1284
return Cyc_vfprintf((struct Cyc___cycFILE*)_check_null(Cyc_script_file),fmt,ap);}
# 1287
int Cyc_force_directory(struct _dyneither_ptr d){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
({struct Cyc_String_pa_PrintArg_struct _tmp1AB=({struct Cyc_String_pa_PrintArg_struct _tmp3DD;_tmp3DD.tag=0U,_tmp3DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp3DD;});struct Cyc_String_pa_PrintArg_struct _tmp1AC=({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0U,_tmp3DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp3DC;});void*_tmp1A9[2U];_tmp1A9[0]=& _tmp1AB,_tmp1A9[1]=& _tmp1AC;({struct _dyneither_ptr _tmp512=({const char*_tmp1AA="if ! test -e %s; then mkdir %s; fi\n";_tag_dyneither(_tmp1AA,sizeof(char),36U);});Cyc_prscript(_tmp512,_tag_dyneither(_tmp1A9,sizeof(void*),2U));});});else{
# 1294
int _tmp1AD=({void*_tmp1B1=0U;({const char*_tmp513=(const char*)_untag_dyneither_ptr(d,sizeof(char),1U);Cyc_open(_tmp513,0,_tag_dyneither(_tmp1B1,sizeof(unsigned short),0U));});});
if(_tmp1AD == -1){
if( mkdir((const char*)_check_null(_untag_dyneither_ptr(d,sizeof(char),1U)),448)== - 1){
({struct Cyc_String_pa_PrintArg_struct _tmp1B0=({struct Cyc_String_pa_PrintArg_struct _tmp3DE;_tmp3DE.tag=0U,_tmp3DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)d);_tmp3DE;});void*_tmp1AE[1U];_tmp1AE[0]=& _tmp1B0;({struct Cyc___cycFILE*_tmp515=Cyc_stderr;struct _dyneither_ptr _tmp514=({const char*_tmp1AF="Error: could not create directory %s\n";_tag_dyneither(_tmp1AF,sizeof(char),38U);});Cyc_fprintf(_tmp515,_tmp514,_tag_dyneither(_tmp1AE,sizeof(void*),1U));});});
return 1;}}else{
# 1301
 close(_tmp1AD);}}
# 1303
return 0;}
# 1306
int Cyc_force_directory_prefixes(struct _dyneither_ptr file){
# 1310
struct _dyneither_ptr _tmp1B2=Cyc_strdup((struct _dyneither_ptr)file);
# 1312
struct Cyc_List_List*_tmp1B3=0;
while(1){
_tmp1B2=Cyc_Filename_dirname((struct _dyneither_ptr)_tmp1B2);
if(Cyc_strlen((struct _dyneither_ptr)_tmp1B2)== (unsigned long)0)break;
_tmp1B3=({struct Cyc_List_List*_tmp1B5=_cycalloc(sizeof(*_tmp1B5));({struct _dyneither_ptr*_tmp516=({struct _dyneither_ptr*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));*_tmp1B4=(struct _dyneither_ptr)_tmp1B2;_tmp1B4;});_tmp1B5->hd=_tmp516;}),_tmp1B5->tl=_tmp1B3;_tmp1B5;});}
# 1319
for(0;_tmp1B3 != 0;_tmp1B3=_tmp1B3->tl){
if(Cyc_force_directory(*((struct _dyneither_ptr*)_tmp1B3->hd)))return 1;}
# 1322
return 0;}char Cyc_NO_SUPPORT[11U]="NO_SUPPORT";struct Cyc_NO_SUPPORT_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 1329
static int Cyc_is_other_special(char c){
char _tmp1B6=c;switch(_tmp1B6){case 92U: _LL1: _LL2:
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
# 1344
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 1350
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmp1B7=Cyc_strlen((struct _dyneither_ptr)s);
# 1354
int _tmp1B8=0;
int _tmp1B9=0;
{int i=0;for(0;(unsigned long)i < _tmp1B7;++ i){
char _tmp1BA=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if((int)_tmp1BA == (int)'\'')++ _tmp1B8;else{
if(Cyc_is_other_special(_tmp1BA))++ _tmp1B9;}}}
# 1363
if(_tmp1B8 == 0  && _tmp1B9 == 0)
return s;
# 1367
if(_tmp1B8 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmp1BB[3U];({struct _dyneither_ptr*_tmp51B=({struct _dyneither_ptr*_tmp1BD=_cycalloc(sizeof(*_tmp1BD));({struct _dyneither_ptr _tmp51A=({const char*_tmp1BC="'";_tag_dyneither(_tmp1BC,sizeof(char),2U);});*_tmp1BD=_tmp51A;});_tmp1BD;});_tmp1BB[0]=_tmp51B;}),({struct _dyneither_ptr*_tmp519=({struct _dyneither_ptr*_tmp1BE=_cycalloc(sizeof(*_tmp1BE));*_tmp1BE=(struct _dyneither_ptr)s;_tmp1BE;});_tmp1BB[1]=_tmp519;}),({struct _dyneither_ptr*_tmp518=({struct _dyneither_ptr*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));({struct _dyneither_ptr _tmp517=({const char*_tmp1BF="'";_tag_dyneither(_tmp1BF,sizeof(char),2U);});*_tmp1C0=_tmp517;});_tmp1C0;});_tmp1BB[2]=_tmp518;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1BB,sizeof(struct _dyneither_ptr*),3U));}));{
# 1371
unsigned long _tmp1C1=(_tmp1B7 + (unsigned long)_tmp1B8)+ (unsigned long)_tmp1B9;
struct _dyneither_ptr s2=({unsigned int _tmp1CC=(_tmp1C1 + (unsigned long)1)+ 1U;char*_tmp1CB=_cycalloc_atomic(_check_times(_tmp1CC,sizeof(char)));({{unsigned int _tmp3DF=_tmp1C1 + (unsigned long)1;unsigned int i;for(i=0;i < _tmp3DF;++ i){_tmp1CB[i]='\000';}_tmp1CB[_tmp3DF]=0;}0;});_tag_dyneither(_tmp1CB,sizeof(char),_tmp1CC);});
int _tmp1C2=0;
int _tmp1C3=0;
for(0;(unsigned long)_tmp1C2 < _tmp1B7;++ _tmp1C2){
char _tmp1C4=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmp1C2));
if((int)_tmp1C4 == (int)'\''  || Cyc_is_other_special(_tmp1C4))
({struct _dyneither_ptr _tmp1C5=_dyneither_ptr_plus(s2,sizeof(char),_tmp1C3 ++);char _tmp1C6=*((char*)_check_dyneither_subscript(_tmp1C5,sizeof(char),0U));char _tmp1C7='\\';if(_get_dyneither_size(_tmp1C5,sizeof(char))== 1U  && (_tmp1C6 == 0  && _tmp1C7 != 0))_throw_arraybounds();*((char*)_tmp1C5.curr)=_tmp1C7;});
({struct _dyneither_ptr _tmp1C8=_dyneither_ptr_plus(s2,sizeof(char),_tmp1C3 ++);char _tmp1C9=*((char*)_check_dyneither_subscript(_tmp1C8,sizeof(char),0U));char _tmp1CA=_tmp1C4;if(_get_dyneither_size(_tmp1C8,sizeof(char))== 1U  && (_tmp1C9 == 0  && _tmp1CA != 0))_throw_arraybounds();*((char*)_tmp1C8.curr)=_tmp1CA;});}
# 1381
return(struct _dyneither_ptr)s2;};}
# 1383
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));({struct _dyneither_ptr _tmp51C=Cyc_sh_escape_string(*sp);*_tmp1CD=_tmp51C;});_tmp1CD;});}
# 1388
int Cyc_process_file(const char*filename,struct Cyc_List_List*start_symbols,struct Cyc_List_List*user_defs,struct Cyc_List_List*omit_symbols,struct Cyc_List_List*hstubs,struct Cyc_List_List*cstubs,struct Cyc_List_List*cycstubs,struct Cyc_List_List*cpp_insert){
# 1396
struct Cyc___cycFILE*maybe;
struct Cyc___cycFILE*in_file;
struct Cyc___cycFILE*out_file;
int errorcode=0;
# 1401
({struct Cyc_String_pa_PrintArg_struct _tmp1D0=({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0U,({
struct _dyneither_ptr _tmp51D=(struct _dyneither_ptr)({const char*_tmp1D1=filename;_tag_dyneither(_tmp1D1,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D1,1U));});_tmp3E0.f1=_tmp51D;});_tmp3E0;});void*_tmp1CE[1U];_tmp1CE[0]=& _tmp1D0;({struct Cyc___cycFILE*_tmp51F=Cyc_stderr;struct _dyneither_ptr _tmp51E=({const char*_tmp1CF="********************************* %s...\n";_tag_dyneither(_tmp1CF,sizeof(char),41U);});Cyc_fprintf(_tmp51F,_tmp51E,_tag_dyneither(_tmp1CE,sizeof(void*),1U));});});
# 1404
if(!Cyc_gathering())({struct Cyc_String_pa_PrintArg_struct _tmp1D4=({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0U,({struct _dyneither_ptr _tmp520=(struct _dyneither_ptr)({const char*_tmp1D5=filename;_tag_dyneither(_tmp1D5,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D5,1U));});_tmp3E1.f1=_tmp520;});_tmp3E1;});void*_tmp1D2[1U];_tmp1D2[0]=& _tmp1D4;({struct _dyneither_ptr _tmp521=({const char*_tmp1D3="\n%s:\n";_tag_dyneither(_tmp1D3,sizeof(char),6U);});Cyc_log(_tmp521,_tag_dyneither(_tmp1D2,sizeof(void*),1U));});});{
# 1416 "buildlib.cyl"
struct _dyneither_ptr _tmp1D6=Cyc_Filename_basename(({const char*_tmp329=filename;_tag_dyneither(_tmp329,sizeof(char),_get_zero_arr_size_char((void*)_tmp329,1U));}));
struct _dyneither_ptr _tmp1D7=Cyc_Filename_dirname(({const char*_tmp328=filename;_tag_dyneither(_tmp328,sizeof(char),_get_zero_arr_size_char((void*)_tmp328,1U));}));
struct _dyneither_ptr _tmp1D8=Cyc_Filename_chop_extension((struct _dyneither_ptr)_tmp1D6);
const char*_tmp1D9=(const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp522=(struct _dyneither_ptr)_tmp1D8;Cyc_strconcat(_tmp522,({const char*_tmp327=".iA";_tag_dyneither(_tmp327,sizeof(char),4U);}));}),sizeof(char),1U));
# 1421
const char*_tmp1DA=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp1D7,sizeof(char))== (unsigned int)0?({struct Cyc_String_pa_PrintArg_struct _tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp415;_tmp415.tag=0U,_tmp415.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D8);_tmp415;});void*_tmp321[1U];_tmp321[0]=& _tmp323;({struct _dyneither_ptr _tmp525=({const char*_tmp322="%s.iB";_tag_dyneither(_tmp322,sizeof(char),6U);});Cyc_aprintf(_tmp525,_tag_dyneither(_tmp321,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp524=(struct _dyneither_ptr)_tmp1D7;Cyc_Filename_concat(_tmp524,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp326=({struct Cyc_String_pa_PrintArg_struct _tmp416;_tmp416.tag=0U,_tmp416.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D8);_tmp416;});void*_tmp324[1U];_tmp324[0]=& _tmp326;({struct _dyneither_ptr _tmp523=({const char*_tmp325="%s.iB";_tag_dyneither(_tmp325,sizeof(char),6U);});Cyc_aprintf(_tmp523,_tag_dyneither(_tmp324,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1426
const char*_tmp1DB=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp1D7,sizeof(char))== (unsigned int)0?({struct Cyc_String_pa_PrintArg_struct _tmp31D=({struct Cyc_String_pa_PrintArg_struct _tmp413;_tmp413.tag=0U,_tmp413.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D8);_tmp413;});void*_tmp31B[1U];_tmp31B[0]=& _tmp31D;({struct _dyneither_ptr _tmp528=({const char*_tmp31C="%s.iC";_tag_dyneither(_tmp31C,sizeof(char),6U);});Cyc_aprintf(_tmp528,_tag_dyneither(_tmp31B,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp527=(struct _dyneither_ptr)_tmp1D7;Cyc_Filename_concat(_tmp527,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp320=({struct Cyc_String_pa_PrintArg_struct _tmp414;_tmp414.tag=0U,_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D8);_tmp414;});void*_tmp31E[1U];_tmp31E[0]=& _tmp320;({struct _dyneither_ptr _tmp526=({const char*_tmp31F="%s.iC";_tag_dyneither(_tmp31F,sizeof(char),6U);});Cyc_aprintf(_tmp526,_tag_dyneither(_tmp31E,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1431
const char*_tmp1DC=(const char*)_check_null(_untag_dyneither_ptr(_get_dyneither_size(_tmp1D7,sizeof(char))== (unsigned int)0?({struct Cyc_String_pa_PrintArg_struct _tmp317=({struct Cyc_String_pa_PrintArg_struct _tmp411;_tmp411.tag=0U,_tmp411.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D8);_tmp411;});void*_tmp315[1U];_tmp315[0]=& _tmp317;({struct _dyneither_ptr _tmp52B=({const char*_tmp316="%s.iD";_tag_dyneither(_tmp316,sizeof(char),6U);});Cyc_aprintf(_tmp52B,_tag_dyneither(_tmp315,sizeof(void*),1U));});}):({struct _dyneither_ptr _tmp52A=(struct _dyneither_ptr)_tmp1D7;Cyc_Filename_concat(_tmp52A,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31A=({struct Cyc_String_pa_PrintArg_struct _tmp412;_tmp412.tag=0U,_tmp412.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp1D8);_tmp412;});void*_tmp318[1U];_tmp318[0]=& _tmp31A;({struct _dyneither_ptr _tmp529=({const char*_tmp319="%s.iD";_tag_dyneither(_tmp319,sizeof(char),6U);});Cyc_aprintf(_tmp529,_tag_dyneither(_tmp318,sizeof(void*),1U));});}));}),sizeof(char),1U));
# 1436
{struct _handler_cons _tmp1DD;_push_handler(& _tmp1DD);{int _tmp1DF=0;if(setjmp(_tmp1DD.handler))_tmp1DF=1;if(!_tmp1DF){
# 1439
if(Cyc_force_directory_prefixes(({const char*_tmp1E0=filename;_tag_dyneither(_tmp1E0,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E0,1U));}))){
int _tmp1E1=1;_npop_handler(0U);return _tmp1E1;}
# 1444
if((int)Cyc_mode != (int)Cyc_FINISH){
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp1E4=({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0U,({struct _dyneither_ptr _tmp52C=(struct _dyneither_ptr)({const char*_tmp1E5=_tmp1D9;_tag_dyneither(_tmp1E5,sizeof(char),_get_zero_arr_size_char((void*)_tmp1E5,1U));});_tmp3E2.f1=_tmp52C;});_tmp3E2;});void*_tmp1E2[1U];_tmp1E2[0]=& _tmp1E4;({struct _dyneither_ptr _tmp52D=({const char*_tmp1E3="cat >%s <<XXX\n";_tag_dyneither(_tmp1E3,sizeof(char),15U);});Cyc_prscript(_tmp52D,_tag_dyneither(_tmp1E2,sizeof(void*),1U));});});
{struct Cyc_List_List*_tmp1E6=cpp_insert;for(0;_tmp1E6 != 0;_tmp1E6=_tmp1E6->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp1E9=({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0U,_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp1E6->hd));_tmp3E3;});void*_tmp1E7[1U];_tmp1E7[0]=& _tmp1E9;({struct _dyneither_ptr _tmp52E=({const char*_tmp1E8="%s";_tag_dyneither(_tmp1E8,sizeof(char),3U);});Cyc_prscript(_tmp52E,_tag_dyneither(_tmp1E7,sizeof(void*),1U));});});}}
({struct Cyc_String_pa_PrintArg_struct _tmp1EC=({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0U,({struct _dyneither_ptr _tmp52F=(struct _dyneither_ptr)({const char*_tmp1ED=filename;_tag_dyneither(_tmp1ED,sizeof(char),_get_zero_arr_size_char((void*)_tmp1ED,1U));});_tmp3E4.f1=_tmp52F;});_tmp3E4;});void*_tmp1EA[1U];_tmp1EA[0]=& _tmp1EC;({struct _dyneither_ptr _tmp530=({const char*_tmp1EB="#include <%s>\n";_tag_dyneither(_tmp1EB,sizeof(char),15U);});Cyc_prscript(_tmp530,_tag_dyneither(_tmp1EA,sizeof(void*),1U));});});
({void*_tmp1EE=0U;({struct _dyneither_ptr _tmp531=({const char*_tmp1EF="XXX\n";_tag_dyneither(_tmp1EF,sizeof(char),5U);});Cyc_prscript(_tmp531,_tag_dyneither(_tmp1EE,sizeof(void*),0U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1F2=({struct Cyc_String_pa_PrintArg_struct _tmp3E7;_tmp3E7.tag=0U,_tmp3E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp3E7;});struct Cyc_String_pa_PrintArg_struct _tmp1F3=({struct Cyc_String_pa_PrintArg_struct _tmp3E6;_tmp3E6.tag=0U,({struct _dyneither_ptr _tmp532=(struct _dyneither_ptr)({const char*_tmp1F6=_tmp1DA;_tag_dyneither(_tmp1F6,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F6,1U));});_tmp3E6.f1=_tmp532;});_tmp3E6;});struct Cyc_String_pa_PrintArg_struct _tmp1F4=({struct Cyc_String_pa_PrintArg_struct _tmp3E5;_tmp3E5.tag=0U,({struct _dyneither_ptr _tmp533=(struct _dyneither_ptr)({const char*_tmp1F5=_tmp1D9;_tag_dyneither(_tmp1F5,sizeof(char),_get_zero_arr_size_char((void*)_tmp1F5,1U));});_tmp3E5.f1=_tmp533;});_tmp3E5;});void*_tmp1F0[3U];_tmp1F0[0]=& _tmp1F2,_tmp1F0[1]=& _tmp1F3,_tmp1F0[2]=& _tmp1F4;({struct _dyneither_ptr _tmp534=({const char*_tmp1F1="$GCC %s -E -dM -o %s -x c %s && \\\n";_tag_dyneither(_tmp1F1,sizeof(char),35U);});Cyc_prscript(_tmp534,_tag_dyneither(_tmp1F0,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp1F9=({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0U,_tmp3EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp3EA;});struct Cyc_String_pa_PrintArg_struct _tmp1FA=({struct Cyc_String_pa_PrintArg_struct _tmp3E9;_tmp3E9.tag=0U,({struct _dyneither_ptr _tmp535=(struct _dyneither_ptr)({const char*_tmp1FD=_tmp1DB;_tag_dyneither(_tmp1FD,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FD,1U));});_tmp3E9.f1=_tmp535;});_tmp3E9;});struct Cyc_String_pa_PrintArg_struct _tmp1FB=({struct Cyc_String_pa_PrintArg_struct _tmp3E8;_tmp3E8.tag=0U,({struct _dyneither_ptr _tmp536=(struct _dyneither_ptr)({const char*_tmp1FC=_tmp1D9;_tag_dyneither(_tmp1FC,sizeof(char),_get_zero_arr_size_char((void*)_tmp1FC,1U));});_tmp3E8.f1=_tmp536;});_tmp3E8;});void*_tmp1F7[3U];_tmp1F7[0]=& _tmp1F9,_tmp1F7[1]=& _tmp1FA,_tmp1F7[2]=& _tmp1FB;({struct _dyneither_ptr _tmp537=({const char*_tmp1F8="$GCC %s -E     -o %s -x c %s;\n";_tag_dyneither(_tmp1F8,sizeof(char),31U);});Cyc_prscript(_tmp537,_tag_dyneither(_tmp1F7,sizeof(void*),3U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp200=({struct Cyc_String_pa_PrintArg_struct _tmp3EB;_tmp3EB.tag=0U,({struct _dyneither_ptr _tmp538=(struct _dyneither_ptr)({const char*_tmp201=_tmp1D9;_tag_dyneither(_tmp201,sizeof(char),_get_zero_arr_size_char((void*)_tmp201,1U));});_tmp3EB.f1=_tmp538;});_tmp3EB;});void*_tmp1FE[1U];_tmp1FE[0]=& _tmp200;({struct _dyneither_ptr _tmp539=({const char*_tmp1FF="rm %s\n";_tag_dyneither(_tmp1FF,sizeof(char),7U);});Cyc_prscript(_tmp539,_tag_dyneither(_tmp1FE,sizeof(void*),1U));});});}else{
# 1456
maybe=Cyc_fopen(_tmp1D9,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp204=({struct Cyc_String_pa_PrintArg_struct _tmp3EC;_tmp3EC.tag=0U,({struct _dyneither_ptr _tmp53A=(struct _dyneither_ptr)({const char*_tmp205=_tmp1D9;_tag_dyneither(_tmp205,sizeof(char),_get_zero_arr_size_char((void*)_tmp205,1U));});_tmp3EC.f1=_tmp53A;});_tmp3EC;});void*_tmp202[1U];_tmp202[0]=& _tmp204;({struct Cyc___cycFILE*_tmp53C=Cyc_stderr;struct _dyneither_ptr _tmp53B=({const char*_tmp203="Error: could not create file %s\n";_tag_dyneither(_tmp203,sizeof(char),33U);});Cyc_fprintf(_tmp53C,_tmp53B,_tag_dyneither(_tmp202,sizeof(void*),1U));});});{
int _tmp206=1;_npop_handler(0U);return _tmp206;};}
# 1461
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp209=({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0U,({struct _dyneither_ptr _tmp53D=(struct _dyneither_ptr)({const char*_tmp20A=_tmp1D9;_tag_dyneither(_tmp20A,sizeof(char),_get_zero_arr_size_char((void*)_tmp20A,1U));});_tmp3ED.f1=_tmp53D;});_tmp3ED;});void*_tmp207[1U];_tmp207[0]=& _tmp209;({struct Cyc___cycFILE*_tmp53F=Cyc_stderr;struct _dyneither_ptr _tmp53E=({const char*_tmp208="Creating %s\n";_tag_dyneither(_tmp208,sizeof(char),13U);});Cyc_fprintf(_tmp53F,_tmp53E,_tag_dyneither(_tmp207,sizeof(void*),1U));});});
out_file=maybe;
{struct Cyc_List_List*_tmp20B=cpp_insert;for(0;_tmp20B != 0;_tmp20B=_tmp20B->tl){
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(*((struct _dyneither_ptr*)_tmp20B->hd),sizeof(char),1U)),out_file);}}
# 1467
({struct Cyc_String_pa_PrintArg_struct _tmp20E=({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0U,({struct _dyneither_ptr _tmp540=(struct _dyneither_ptr)({const char*_tmp20F=filename;_tag_dyneither(_tmp20F,sizeof(char),_get_zero_arr_size_char((void*)_tmp20F,1U));});_tmp3EE.f1=_tmp540;});_tmp3EE;});void*_tmp20C[1U];_tmp20C[0]=& _tmp20E;({struct Cyc___cycFILE*_tmp542=out_file;struct _dyneither_ptr _tmp541=({const char*_tmp20D="#include <%s>\n";_tag_dyneither(_tmp20D,sizeof(char),15U);});Cyc_fprintf(_tmp542,_tmp541,_tag_dyneither(_tmp20C,sizeof(void*),1U));});});
Cyc_fclose(out_file);{
struct _dyneither_ptr _tmp210=({
struct Cyc_List_List*_tmp546=({struct Cyc_List_List*_tmp22D=_cycalloc(sizeof(*_tmp22D));({struct _dyneither_ptr*_tmp545=({struct _dyneither_ptr*_tmp22C=_cycalloc(sizeof(*_tmp22C));({struct _dyneither_ptr _tmp544=(struct _dyneither_ptr)({const char*_tmp22B="";_tag_dyneither(_tmp22B,sizeof(char),1U);});*_tmp22C=_tmp544;});_tmp22C;});_tmp22D->hd=_tmp545;}),({
struct Cyc_List_List*_tmp543=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp22D->tl=_tmp543;});_tmp22D;});
# 1470
Cyc_str_sepstr(_tmp546,({const char*_tmp22E=" ";_tag_dyneither(_tmp22E,sizeof(char),2U);}));});
# 1473
char*cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp224=({struct Cyc_String_pa_PrintArg_struct _tmp3FA;_tmp3FA.tag=0U,_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp3FA;});struct Cyc_String_pa_PrintArg_struct _tmp225=({struct Cyc_String_pa_PrintArg_struct _tmp3F9;_tmp3F9.tag=0U,_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp3F9;});struct Cyc_String_pa_PrintArg_struct _tmp226=({struct Cyc_String_pa_PrintArg_struct _tmp3F8;_tmp3F8.tag=0U,_tmp3F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210);_tmp3F8;});struct Cyc_String_pa_PrintArg_struct _tmp227=({struct Cyc_String_pa_PrintArg_struct _tmp3F7;_tmp3F7.tag=0U,({struct _dyneither_ptr _tmp547=(struct _dyneither_ptr)({const char*_tmp22A=_tmp1DA;_tag_dyneither(_tmp22A,sizeof(char),_get_zero_arr_size_char((void*)_tmp22A,1U));});_tmp3F7.f1=_tmp547;});_tmp3F7;});struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp3F6;_tmp3F6.tag=0U,({struct _dyneither_ptr _tmp548=(struct _dyneither_ptr)({const char*_tmp229=_tmp1D9;_tag_dyneither(_tmp229,sizeof(char),_get_zero_arr_size_char((void*)_tmp229,1U));});_tmp3F6.f1=_tmp548;});_tmp3F6;});void*_tmp222[5U];_tmp222[0]=& _tmp224,_tmp222[1]=& _tmp225,_tmp222[2]=& _tmp226,_tmp222[3]=& _tmp227,_tmp222[4]=& _tmp228;({struct _dyneither_ptr _tmp549=({const char*_tmp223="%s %s %s -E -dM -o %s -x c %s";_tag_dyneither(_tmp223,sizeof(char),30U);});Cyc_aprintf(_tmp549,_tag_dyneither(_tmp222,sizeof(void*),5U));});}),sizeof(char),1U));
# 1476
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp213=({struct Cyc_String_pa_PrintArg_struct _tmp3EF;_tmp3EF.tag=0U,({struct _dyneither_ptr _tmp54A=(struct _dyneither_ptr)({char*_tmp214=cmd;_tag_dyneither(_tmp214,sizeof(char),_get_zero_arr_size_char((void*)_tmp214,1U));});_tmp3EF.f1=_tmp54A;});_tmp3EF;});void*_tmp211[1U];_tmp211[0]=& _tmp213;({struct Cyc___cycFILE*_tmp54C=Cyc_stderr;struct _dyneither_ptr _tmp54B=({const char*_tmp212="%s\n";_tag_dyneither(_tmp212,sizeof(char),4U);});Cyc_fprintf(_tmp54C,_tmp54B,_tag_dyneither(_tmp211,sizeof(void*),1U));});});
if(! system((const char*)cmd)){
# 1481
cmd=(char*)_check_null(_untag_dyneither_ptr(({struct Cyc_String_pa_PrintArg_struct _tmp217=({struct Cyc_String_pa_PrintArg_struct _tmp3F4;_tmp3F4.tag=0U,_tmp3F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp3F4;});struct Cyc_String_pa_PrintArg_struct _tmp218=({struct Cyc_String_pa_PrintArg_struct _tmp3F3;_tmp3F3.tag=0U,_tmp3F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp3F3;});struct Cyc_String_pa_PrintArg_struct _tmp219=({struct Cyc_String_pa_PrintArg_struct _tmp3F2;_tmp3F2.tag=0U,_tmp3F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp210);_tmp3F2;});struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp3F1;_tmp3F1.tag=0U,({struct _dyneither_ptr _tmp54D=(struct _dyneither_ptr)({const char*_tmp21D=_tmp1DB;_tag_dyneither(_tmp21D,sizeof(char),_get_zero_arr_size_char((void*)_tmp21D,1U));});_tmp3F1.f1=_tmp54D;});_tmp3F1;});struct Cyc_String_pa_PrintArg_struct _tmp21B=({struct Cyc_String_pa_PrintArg_struct _tmp3F0;_tmp3F0.tag=0U,({struct _dyneither_ptr _tmp54E=(struct _dyneither_ptr)({const char*_tmp21C=_tmp1D9;_tag_dyneither(_tmp21C,sizeof(char),_get_zero_arr_size_char((void*)_tmp21C,1U));});_tmp3F0.f1=_tmp54E;});_tmp3F0;});void*_tmp215[5U];_tmp215[0]=& _tmp217,_tmp215[1]=& _tmp218,_tmp215[2]=& _tmp219,_tmp215[3]=& _tmp21A,_tmp215[4]=& _tmp21B;({struct _dyneither_ptr _tmp54F=({const char*_tmp216="%s %s %s -E -o %s -x c %s";_tag_dyneither(_tmp216,sizeof(char),26U);});Cyc_aprintf(_tmp54F,_tag_dyneither(_tmp215,sizeof(void*),5U));});}),sizeof(char),1U));
# 1484
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp220=({struct Cyc_String_pa_PrintArg_struct _tmp3F5;_tmp3F5.tag=0U,({struct _dyneither_ptr _tmp550=(struct _dyneither_ptr)({char*_tmp221=cmd;_tag_dyneither(_tmp221,sizeof(char),_get_zero_arr_size_char((void*)_tmp221,1U));});_tmp3F5.f1=_tmp550;});_tmp3F5;});void*_tmp21E[1U];_tmp21E[0]=& _tmp220;({struct Cyc___cycFILE*_tmp552=Cyc_stderr;struct _dyneither_ptr _tmp551=({const char*_tmp21F="%s\n";_tag_dyneither(_tmp21F,sizeof(char),4U);});Cyc_fprintf(_tmp552,_tmp551,_tag_dyneither(_tmp21E,sizeof(void*),1U));});});
 system((const char*)cmd);}};}}
# 1491
if(Cyc_gathering()){int _tmp22F=0;_npop_handler(0U);return _tmp22F;}{
# 1494
struct Cyc_Hashtable_Table*t=Cyc_new_deps();
maybe=Cyc_fopen(_tmp1DA,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp234=_cycalloc(sizeof(*_tmp234));_tmp234->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp555=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp232=({struct Cyc_String_pa_PrintArg_struct _tmp3FB;_tmp3FB.tag=0U,({struct _dyneither_ptr _tmp553=(struct _dyneither_ptr)({const char*_tmp233=_tmp1DA;_tag_dyneither(_tmp233,sizeof(char),_get_zero_arr_size_char((void*)_tmp233,1U));});_tmp3FB.f1=_tmp553;});_tmp3FB;});void*_tmp230[1U];_tmp230[0]=& _tmp232;({struct _dyneither_ptr _tmp554=({const char*_tmp231="can't open macrosfile %s";_tag_dyneither(_tmp231,sizeof(char),25U);});Cyc_aprintf(_tmp554,_tag_dyneither(_tmp230,sizeof(void*),1U));});});_tmp234->f1=_tmp555;});_tmp234;}));
# 1499
in_file=maybe;{
struct Cyc_Lexing_lexbuf*_tmp235=Cyc_Lexing_from_file(in_file);
struct _tuple21*entry;
while((entry=((struct _tuple21*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_line)(_tmp235))!= 0){
struct _tuple21*_tmp236=(struct _tuple21*)_check_null(entry);struct _tuple21*_tmp237=_tmp236;struct _dyneither_ptr*_tmp239;struct Cyc_Set_Set*_tmp238;_LL1: _tmp239=_tmp237->f1;_tmp238=_tmp237->f2;_LL2:;
((void(*)(struct Cyc_Hashtable_Table*t,struct _dyneither_ptr*key,struct Cyc_Set_Set*val))Cyc_Hashtable_insert)(t,_tmp239,_tmp238);}
# 1508
Cyc_fclose(in_file);
# 1511
maybe=Cyc_fopen(_tmp1DB,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp558=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp23C=({struct Cyc_String_pa_PrintArg_struct _tmp3FC;_tmp3FC.tag=0U,({struct _dyneither_ptr _tmp556=(struct _dyneither_ptr)({const char*_tmp23D=_tmp1DB;_tag_dyneither(_tmp23D,sizeof(char),_get_zero_arr_size_char((void*)_tmp23D,1U));});_tmp3FC.f1=_tmp556;});_tmp3FC;});void*_tmp23A[1U];_tmp23A[0]=& _tmp23C;({struct _dyneither_ptr _tmp557=({const char*_tmp23B="can't open declsfile %s";_tag_dyneither(_tmp23B,sizeof(char),24U);});Cyc_aprintf(_tmp557,_tag_dyneither(_tmp23A,sizeof(void*),1U));});});_tmp23E->f1=_tmp558;});_tmp23E;}));
# 1515
in_file=maybe;
_tmp235=Cyc_Lexing_from_file(in_file);
Cyc_slurp_out=Cyc_fopen(_tmp1DC,"w");
if(!((unsigned int)Cyc_slurp_out)){int _tmp23F=1;_npop_handler(0U);return _tmp23F;}
while(((int(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_slurp)(_tmp235)){;}{
# 1521
struct Cyc_List_List*_tmp240=user_defs;
while(_tmp240 != 0){
struct _tuple24*_tmp241=(struct _tuple24*)_tmp240->hd;struct _tuple24*_tmp242=_tmp241;struct _dyneither_ptr*_tmp246;_LL4: _tmp246=_tmp242->f2;_LL5:;
({struct Cyc_String_pa_PrintArg_struct _tmp245=({struct Cyc_String_pa_PrintArg_struct _tmp3FD;_tmp3FD.tag=0U,_tmp3FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp246);_tmp3FD;});void*_tmp243[1U];_tmp243[0]=& _tmp245;({struct Cyc___cycFILE*_tmp55A=(struct Cyc___cycFILE*)_check_null(Cyc_slurp_out);struct _dyneither_ptr _tmp559=({const char*_tmp244="%s";_tag_dyneither(_tmp244,sizeof(char),3U);});Cyc_fprintf(_tmp55A,_tmp559,_tag_dyneither(_tmp243,sizeof(void*),1U));});});
_tmp240=_tmp240->tl;}
# 1527
Cyc_fclose(in_file);
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_slurp_out));
if((int)Cyc_mode != (int)Cyc_FINISH)
;
# 1533
maybe=Cyc_fopen(_tmp1DC,"r");
if(!((unsigned int)maybe)){int _tmp247=1;_npop_handler(0U);return _tmp247;}
in_file=maybe;
Cyc_Position_reset_position(({const char*_tmp248=_tmp1DC;_tag_dyneither(_tmp248,sizeof(char),_get_zero_arr_size_char((void*)_tmp248,1U));}));
Cyc_Lex_lex_init(0);{
struct Cyc_List_List*_tmp249=Cyc_Parse_parse_file(in_file);
Cyc_Lex_lex_init(0);
Cyc_fclose(in_file);
# 1543
{struct Cyc_List_List*_tmp24A=_tmp249;for(0;_tmp24A != 0;_tmp24A=_tmp24A->tl){
Cyc_scan_decl((struct Cyc_Absyn_Decl*)_tmp24A->hd,t);}}{
# 1547
struct Cyc_List_List*_tmp24B=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_add_user_prefix,(((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)(user_defs)).f1);
struct Cyc_Set_Set*_tmp24C=({struct Cyc_List_List*_tmp55B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(start_symbols,_tmp24B);Cyc_reachable(_tmp55B,t);});
# 1551
struct Cyc_List_List*_tmp24D=0;
struct Cyc_List_List*_tmp24E=0;
struct Cyc_Set_Set*defined_symbols=((struct Cyc_Set_Set*(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Set_empty)(Cyc_strptrcmp);
{struct Cyc_List_List*_tmp24F=_tmp249;for(0;_tmp24F != 0;_tmp24F=_tmp24F->tl){
struct Cyc_Absyn_Decl*_tmp250=(struct Cyc_Absyn_Decl*)_tmp24F->hd;
struct _dyneither_ptr*name;
{void*_tmp251=_tmp250->r;void*_tmp252=_tmp251;struct Cyc_Absyn_Typedefdecl*_tmp26D;struct Cyc_Absyn_Enumdecl*_tmp26C;struct Cyc_Absyn_Aggrdecl*_tmp26B;struct Cyc_Absyn_Fndecl*_tmp26A;struct Cyc_Absyn_Vardecl*_tmp269;switch(*((int*)_tmp252)){case 0U: _LL7: _tmp269=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL8: {
# 1559
struct _tuple1*_tmp253=_tmp269->name;struct _tuple1*_tmp254=_tmp253;struct _dyneither_ptr*_tmp255;_LL2A: _tmp255=_tmp254->f2;_LL2B:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp255);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp255))name=0;else{
name=_tmp255;}
goto _LL6;}case 1U: _LL9: _tmp26A=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LLA: {
# 1565
struct _tuple1*_tmp256=_tmp26A->name;struct _tuple1*_tmp257=_tmp256;struct _dyneither_ptr*_tmp258;_LL2D: _tmp258=_tmp257->f2;_LL2E:;
defined_symbols=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_insert)(defined_symbols,_tmp258);
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,omit_symbols,_tmp258))name=0;else{
name=_tmp258;}
goto _LL6;}case 5U: _LLB: _tmp26B=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LLC: {
# 1571
struct _tuple1*_tmp259=_tmp26B->name;struct _tuple1*_tmp25A=_tmp259;struct _dyneither_ptr*_tmp25B;_LL30: _tmp25B=_tmp25A->f2;_LL31:;
name=_tmp25B;
goto _LL6;}case 7U: _LLD: _tmp26C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LLE: {
# 1575
struct _tuple1*_tmp25C=_tmp26C->name;struct _tuple1*_tmp25D=_tmp25C;struct _dyneither_ptr*_tmp265;_LL33: _tmp265=_tmp25D->f2;_LL34:;
name=_tmp265;
# 1579
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp24C,name))
_tmp24D=({struct Cyc_List_List*_tmp25E=_cycalloc(sizeof(*_tmp25E));_tmp25E->hd=_tmp250,_tmp25E->tl=_tmp24D;_tmp25E;});else{
# 1582
if((unsigned int)_tmp26C->fields){
struct Cyc_List_List*_tmp25F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp26C->fields))->v;for(0;_tmp25F != 0;_tmp25F=_tmp25F->tl){
struct Cyc_Absyn_Enumfield*_tmp260=(struct Cyc_Absyn_Enumfield*)_tmp25F->hd;
struct _tuple1*_tmp261=_tmp260->name;struct _tuple1*_tmp262=_tmp261;struct _dyneither_ptr*_tmp264;_LL36: _tmp264=_tmp262->f2;_LL37:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp24C,_tmp264)){
_tmp24D=({struct Cyc_List_List*_tmp263=_cycalloc(sizeof(*_tmp263));_tmp263->hd=_tmp250,_tmp263->tl=_tmp24D;_tmp263;});
break;}}}}
# 1592
name=0;
goto _LL6;}case 8U: _LLF: _tmp26D=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL10: {
# 1595
struct _tuple1*_tmp266=_tmp26D->name;struct _tuple1*_tmp267=_tmp266;struct _dyneither_ptr*_tmp268;_LL39: _tmp268=_tmp267->f2;_LL3A:;
name=_tmp268;
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
# 1610
 name=0;
goto _LL6;}_LL6:;}
# 1614
if(name != 0  && ((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp24C,name)){
if(({struct _dyneither_ptr _tmp55D=(struct _dyneither_ptr)*name;struct _dyneither_ptr _tmp55C=(struct _dyneither_ptr)Cyc_user_prefix;Cyc_strncmp(_tmp55D,_tmp55C,Cyc_strlen((struct _dyneither_ptr)Cyc_user_prefix));})!= 0)
_tmp24D=({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->hd=_tmp250,_tmp26E->tl=_tmp24D;_tmp26E;});else{
# 1619
Cyc_rename_decl(_tmp250);
_tmp24E=({struct Cyc_List_List*_tmp26F=_cycalloc(sizeof(*_tmp26F));_tmp26F->hd=_tmp250,_tmp26F->tl=_tmp24E;_tmp26F;});}}}}
# 1626
if(!Cyc_do_setjmp){
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){int _tmp270=1;_npop_handler(0U);return _tmp270;}
out_file=maybe;}else{
out_file=Cyc_stdout;}{
struct _dyneither_ptr ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2ED=({struct Cyc_String_pa_PrintArg_struct _tmp409;_tmp409.tag=0U,({struct _dyneither_ptr _tmp55E=(struct _dyneither_ptr)({const char*_tmp2EE=filename;_tag_dyneither(_tmp2EE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2EE,1U));});_tmp409.f1=_tmp55E;});_tmp409;});void*_tmp2EB[1U];_tmp2EB[0]=& _tmp2ED;({struct _dyneither_ptr _tmp55F=({const char*_tmp2EC="_%s_";_tag_dyneither(_tmp2EC,sizeof(char),5U);});Cyc_aprintf(_tmp55F,_tag_dyneither(_tmp2EB,sizeof(void*),1U));});});
{int _tmp271=0;for(0;(unsigned int)_tmp271 < _get_dyneither_size(ifdefmacro,sizeof(char));++ _tmp271){
if((int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp271))== (int)'.'  || (int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp271))== (int)'/')
({struct _dyneither_ptr _tmp272=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp271);char _tmp273=*((char*)_check_dyneither_subscript(_tmp272,sizeof(char),0U));char _tmp274='_';if(_get_dyneither_size(_tmp272,sizeof(char))== 1U  && (_tmp273 == 0  && _tmp274 != 0))_throw_arraybounds();*((char*)_tmp272.curr)=_tmp274;});else{
if((int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp271))!= (int)'_'  && (int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp271))!= (int)'/')
({struct _dyneither_ptr _tmp275=_dyneither_ptr_plus(ifdefmacro,sizeof(char),_tmp271);char _tmp276=*((char*)_check_dyneither_subscript(_tmp275,sizeof(char),0U));char _tmp277=(char) toupper((int)*((char*)_check_dyneither_subscript(ifdefmacro,sizeof(char),_tmp271)));if(_get_dyneither_size(_tmp275,sizeof(char))== 1U  && (_tmp276 == 0  && _tmp277 != 0))_throw_arraybounds();*((char*)_tmp275.curr)=_tmp277;});}}}
# 1638
({struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp3FF;_tmp3FF.tag=0U,_tmp3FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp3FF;});struct Cyc_String_pa_PrintArg_struct _tmp27B=({struct Cyc_String_pa_PrintArg_struct _tmp3FE;_tmp3FE.tag=0U,_tmp3FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp3FE;});void*_tmp278[2U];_tmp278[0]=& _tmp27A,_tmp278[1]=& _tmp27B;({struct Cyc___cycFILE*_tmp561=out_file;struct _dyneither_ptr _tmp560=({const char*_tmp279="#ifndef %s\n#define %s\n";_tag_dyneither(_tmp279,sizeof(char),23U);});Cyc_fprintf(_tmp561,_tmp560,_tag_dyneither(_tmp278,sizeof(void*),2U));});});{
# 1645
struct Cyc_List_List*_tmp27C=0;
struct Cyc_List_List*_tmp27D=0;
{struct Cyc_List_List*_tmp27E=_tmp24D;for(0;_tmp27E != 0;_tmp27E=_tmp27E->tl){
struct Cyc_Absyn_Decl*_tmp27F=(struct Cyc_Absyn_Decl*)_tmp27E->hd;
int _tmp280=0;
struct _dyneither_ptr*name;
{void*_tmp281=_tmp27F->r;void*_tmp282=_tmp281;struct Cyc_Absyn_Typedefdecl*_tmp296;struct Cyc_Absyn_Enumdecl*_tmp295;struct Cyc_Absyn_Aggrdecl*_tmp294;struct Cyc_Absyn_Fndecl*_tmp293;struct Cyc_Absyn_Vardecl*_tmp292;switch(*((int*)_tmp282)){case 0U: _LL3C: _tmp292=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp282)->f1;_LL3D: {
# 1653
struct _tuple1*_tmp283=_tmp292->name;struct _tuple1*_tmp284=_tmp283;struct _dyneither_ptr*_tmp285;_LL5F: _tmp285=_tmp284->f2;_LL60:;
name=_tmp285;
goto _LL3B;}case 1U: _LL3E: _tmp293=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp282)->f1;_LL3F:
# 1657
 if(_tmp293->is_inline){name=0;goto _LL3B;}{
struct _tuple1*_tmp286=_tmp293->name;struct _tuple1*_tmp287=_tmp286;struct _dyneither_ptr*_tmp288;_LL62: _tmp288=_tmp287->f2;_LL63:;
name=_tmp288;
goto _LL3B;};case 5U: _LL40: _tmp294=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp282)->f1;_LL41: {
# 1662
struct _tuple1*_tmp289=_tmp294->name;struct _tuple1*_tmp28A=_tmp289;struct _dyneither_ptr*_tmp28B;_LL65: _tmp28B=_tmp28A->f2;_LL66:;
name=_tmp28B;
goto _LL3B;}case 7U: _LL42: _tmp295=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp282)->f1;_LL43: {
# 1666
struct _tuple1*_tmp28C=_tmp295->name;struct _tuple1*_tmp28D=_tmp28C;struct _dyneither_ptr*_tmp28E;_LL68: _tmp28E=_tmp28D->f2;_LL69:;
name=_tmp28E;
goto _LL3B;}case 8U: _LL44: _tmp296=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp282)->f1;_LL45: {
# 1670
struct _tuple1*_tmp28F=_tmp296->name;struct _tuple1*_tmp290=_tmp28F;struct _dyneither_ptr*_tmp291;_LL6B: _tmp291=_tmp290->f2;_LL6C:;
name=_tmp291;
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
# 1685
 name=0;
goto _LL3B;}_LL3B:;}
# 1688
if(!((unsigned int)name) && !_tmp280)continue;
# 1693
if(({struct Cyc_Set_Set*_tmp562=_tmp24C;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp562,Cyc_add_user_prefix(name));})){
struct Cyc_Absyn_Decl*_tmp297=Cyc_Absyn_lookup_decl(_tmp24E,name);
if(_tmp297 == 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp563=({const char*_tmp298="Internal Error: bad user-def name";_tag_dyneither(_tmp298,sizeof(char),34U);});_tmp299->f1=_tmp563;});_tmp299;}));else{
# 1699
void*_tmp29A=_tmp297->r;void*_tmp29B=_tmp29A;switch(*((int*)_tmp29B)){case 0U: _LL6E: _LL6F:
 goto _LL71;case 1U: _LL70: _LL71:
# 1702
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp564=({const char*_tmp29C="user defintions for function or variable decls";_tag_dyneither(_tmp29C,sizeof(char),47U);});_tmp29D->f1=_tmp564;});_tmp29D;}));default: _LL72: _LL73:
 goto _LL6D;}_LL6D:;}
# 1706
Cyc_Cifc_merge_sys_user_decl(0U,1,_tmp297,_tmp27F);
_tmp27C=({struct Cyc_List_List*_tmp29E=_cycalloc(sizeof(*_tmp29E));_tmp29E->hd=_tmp27F,_tmp29E->tl=_tmp27C;_tmp29E;});}else{
# 1710
_tmp27C=({struct Cyc_List_List*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F->hd=_tmp27F,_tmp29F->tl=_tmp27C;_tmp29F;});}
_tmp27D=({struct Cyc_List_List*_tmp2A0=_cycalloc(sizeof(*_tmp2A0));_tmp2A0->hd=name,_tmp2A0->tl=_tmp27D;_tmp2A0;});}}
# 1715
{struct _handler_cons _tmp2A1;_push_handler(& _tmp2A1);{int _tmp2A3=0;if(setjmp(_tmp2A1.handler))_tmp2A3=1;if(!_tmp2A3){
Cyc_Binding_resolve_all(_tmp27C);
({struct Cyc_Tcenv_Tenv*_tmp565=Cyc_Tcenv_tc_init();Cyc_Tc_tc(_tmp565,1,_tmp27C);});
# 1716
;_pop_handler();}else{void*_tmp2A2=(void*)_exn_thrown;void*_tmp2A4=_tmp2A2;_LL75: _LL76:
# 1719
(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp566=({const char*_tmp2A5="can't typecheck acquired declarations";_tag_dyneither(_tmp2A5,sizeof(char),38U);});_tmp2A6->f1=_tmp566;});_tmp2A6;}));_LL74:;}};}
# 1723
{struct _tuple0 _tmp2A7=({struct _tuple0 _tmp403;_tmp403.f1=_tmp27C,_tmp403.f2=_tmp27D;_tmp403;});struct _tuple0 _tmp2A8=_tmp2A7;struct Cyc_List_List*_tmp2BA;struct Cyc_List_List*_tmp2B9;_LL7A: _tmp2BA=_tmp2A8.f1;_tmp2B9=_tmp2A8.f2;_LL7B:;for(0;
_tmp2BA != 0  && _tmp2B9 != 0;(_tmp2BA=_tmp2BA->tl,_tmp2B9=_tmp2B9->tl)){
struct Cyc_Absyn_Decl*_tmp2A9=(struct Cyc_Absyn_Decl*)_tmp2BA->hd;
struct _dyneither_ptr*_tmp2AA=(struct _dyneither_ptr*)_tmp2B9->hd;
int _tmp2AB=0;
if(!((unsigned int)_tmp2AA))
_tmp2AB=1;
# 1732
Cyc_Absynpp_set_params(& Cyc_Absynpp_cyc_params_r);
if((unsigned int)_tmp2AA){
ifdefmacro=({struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp400;_tmp400.tag=0U,_tmp400.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2AA);_tmp400;});void*_tmp2AC[1U];_tmp2AC[0]=& _tmp2AE;({struct _dyneither_ptr _tmp567=({const char*_tmp2AD="_%s_def_";_tag_dyneither(_tmp2AD,sizeof(char),9U);});Cyc_aprintf(_tmp567,_tag_dyneither(_tmp2AC,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2B1=({struct Cyc_String_pa_PrintArg_struct _tmp401;_tmp401.tag=0U,_tmp401.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp401;});void*_tmp2AF[1U];_tmp2AF[0]=& _tmp2B1;({struct Cyc___cycFILE*_tmp569=out_file;struct _dyneither_ptr _tmp568=({const char*_tmp2B0="#ifndef %s\n";_tag_dyneither(_tmp2B0,sizeof(char),12U);});Cyc_fprintf(_tmp569,_tmp568,_tag_dyneither(_tmp2AF,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2B4=({struct Cyc_String_pa_PrintArg_struct _tmp402;_tmp402.tag=0U,_tmp402.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ifdefmacro);_tmp402;});void*_tmp2B2[1U];_tmp2B2[0]=& _tmp2B4;({struct Cyc___cycFILE*_tmp56B=out_file;struct _dyneither_ptr _tmp56A=({const char*_tmp2B3="#define %s\n";_tag_dyneither(_tmp2B3,sizeof(char),12U);});Cyc_fprintf(_tmp56B,_tmp56A,_tag_dyneither(_tmp2B2,sizeof(void*),1U));});});
# 1738
({struct Cyc_List_List*_tmp56C=({struct Cyc_Absyn_Decl*_tmp2B5[1U];_tmp2B5[0]=_tmp2A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2B5,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp56C,out_file);});
({void*_tmp2B6=0U;({struct Cyc___cycFILE*_tmp56E=out_file;struct _dyneither_ptr _tmp56D=({const char*_tmp2B7="#endif\n";_tag_dyneither(_tmp2B7,sizeof(char),8U);});Cyc_fprintf(_tmp56E,_tmp56D,_tag_dyneither(_tmp2B6,sizeof(void*),0U));});});}else{
# 1743
({struct Cyc_List_List*_tmp56F=({struct Cyc_Absyn_Decl*_tmp2B8[1U];_tmp2B8[0]=_tmp2A9;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2B8,sizeof(struct Cyc_Absyn_Decl*),1U));});Cyc_Absynpp_decllist2file(_tmp56F,out_file);});}}}
# 1748
maybe=Cyc_fopen(_tmp1DA,"r");
if(!((unsigned int)maybe))(int)_throw((void*)({struct Cyc_NO_SUPPORT_exn_struct*_tmp2BF=_cycalloc(sizeof(*_tmp2BF));_tmp2BF->tag=Cyc_NO_SUPPORT,({struct _dyneither_ptr _tmp572=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp404;_tmp404.tag=0U,({struct _dyneither_ptr _tmp570=(struct _dyneither_ptr)({const char*_tmp2BE=_tmp1DA;_tag_dyneither(_tmp2BE,sizeof(char),_get_zero_arr_size_char((void*)_tmp2BE,1U));});_tmp404.f1=_tmp570;});_tmp404;});void*_tmp2BB[1U];_tmp2BB[0]=& _tmp2BD;({struct _dyneither_ptr _tmp571=({const char*_tmp2BC="can't open macrosfile %s";_tag_dyneither(_tmp2BC,sizeof(char),25U);});Cyc_aprintf(_tmp571,_tag_dyneither(_tmp2BB,sizeof(void*),1U));});});_tmp2BF->f1=_tmp572;});_tmp2BF;}));
# 1751
in_file=maybe;
_tmp235=Cyc_Lexing_from_file(in_file);{
struct _tuple22*entry2;
while((entry2=((struct _tuple22*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_suck_line)(_tmp235))!= 0){
struct _tuple22*_tmp2C0=(struct _tuple22*)_check_null(entry2);struct _tuple22*_tmp2C1=_tmp2C0;struct _dyneither_ptr _tmp2CB;struct _dyneither_ptr*_tmp2CA;_LL7D: _tmp2CB=_tmp2C1->f1;_tmp2CA=_tmp2C1->f2;_LL7E:;
if(((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp24C,_tmp2CA)){
({struct Cyc_String_pa_PrintArg_struct _tmp2C4=({struct Cyc_String_pa_PrintArg_struct _tmp405;_tmp405.tag=0U,_tmp405.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp2CA);_tmp405;});void*_tmp2C2[1U];_tmp2C2[0]=& _tmp2C4;({struct Cyc___cycFILE*_tmp574=out_file;struct _dyneither_ptr _tmp573=({const char*_tmp2C3="#ifndef %s\n";_tag_dyneither(_tmp2C3,sizeof(char),12U);});Cyc_fprintf(_tmp574,_tmp573,_tag_dyneither(_tmp2C2,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp2C7=({struct Cyc_String_pa_PrintArg_struct _tmp406;_tmp406.tag=0U,_tmp406.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2CB);_tmp406;});void*_tmp2C5[1U];_tmp2C5[0]=& _tmp2C7;({struct Cyc___cycFILE*_tmp576=out_file;struct _dyneither_ptr _tmp575=({const char*_tmp2C6="%s\n";_tag_dyneither(_tmp2C6,sizeof(char),4U);});Cyc_fprintf(_tmp576,_tmp575,_tag_dyneither(_tmp2C5,sizeof(void*),1U));});});
({void*_tmp2C8=0U;({struct Cyc___cycFILE*_tmp578=out_file;struct _dyneither_ptr _tmp577=({const char*_tmp2C9="#endif\n";_tag_dyneither(_tmp2C9,sizeof(char),8U);});Cyc_fprintf(_tmp578,_tmp577,_tag_dyneither(_tmp2C8,sizeof(void*),0U));});});}}
# 1762
Cyc_fclose(in_file);
if((int)Cyc_mode != (int)Cyc_FINISH);
# 1765
if(hstubs != 0){
struct Cyc_List_List*_tmp2CC=hstubs;for(0;_tmp2CC != 0;_tmp2CC=_tmp2CC->tl){
struct _tuple23*_tmp2CD=(struct _tuple23*)_tmp2CC->hd;struct _tuple23*_tmp2CE=_tmp2CD;struct _dyneither_ptr _tmp2D4;struct _dyneither_ptr _tmp2D3;_LL80: _tmp2D4=_tmp2CE->f1;_tmp2D3=_tmp2CE->f2;_LL81:;
if(({char*_tmp57B=(char*)_tmp2D3.curr;_tmp57B != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp57A=(char*)_tmp2D4.curr;_tmp57A == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp579=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp579,({struct _dyneither_ptr*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));*_tmp2CF=_tmp2D4;_tmp2CF;}));})))
# 1771
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp2D3,sizeof(char),1U)),out_file);else{
# 1773
({struct Cyc_String_pa_PrintArg_struct _tmp2D2=({struct Cyc_String_pa_PrintArg_struct _tmp407;_tmp407.tag=0U,_tmp407.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2D4);_tmp407;});void*_tmp2D0[1U];_tmp2D0[0]=& _tmp2D2;({struct _dyneither_ptr _tmp57C=({const char*_tmp2D1="%s is not supported on this platform\n";_tag_dyneither(_tmp2D1,sizeof(char),38U);});Cyc_log(_tmp57C,_tag_dyneither(_tmp2D0,sizeof(void*),1U));});});}}}
# 1776
({void*_tmp2D5=0U;({struct Cyc___cycFILE*_tmp57E=out_file;struct _dyneither_ptr _tmp57D=({const char*_tmp2D6="#endif\n";_tag_dyneither(_tmp2D6,sizeof(char),8U);});Cyc_fprintf(_tmp57E,_tmp57D,_tag_dyneither(_tmp2D5,sizeof(void*),0U));});});
if(Cyc_do_setjmp){int _tmp2D7=0;_npop_handler(0U);return _tmp2D7;}else{
Cyc_fclose(out_file);}
# 1781
if(cstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file);{
struct Cyc_List_List*_tmp2D8=cstubs;for(0;_tmp2D8 != 0;_tmp2D8=_tmp2D8->tl){
struct _tuple23*_tmp2D9=(struct _tuple23*)_tmp2D8->hd;struct _tuple23*_tmp2DA=_tmp2D9;struct _dyneither_ptr _tmp2DD;struct _dyneither_ptr _tmp2DC;_LL83: _tmp2DD=_tmp2DA->f1;_tmp2DC=_tmp2DA->f2;_LL84:;
if(({char*_tmp581=(char*)_tmp2DC.curr;_tmp581 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp580=(char*)_tmp2DD.curr;_tmp580 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp57F=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp57F,({struct _dyneither_ptr*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));*_tmp2DB=_tmp2DD;_tmp2DB;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp2DC,sizeof(char),1U)),out_file);}};}
# 1792
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
# 1794
({struct Cyc_String_pa_PrintArg_struct _tmp2E0=({struct Cyc_String_pa_PrintArg_struct _tmp408;_tmp408.tag=0U,({struct _dyneither_ptr _tmp582=(struct _dyneither_ptr)({const char*_tmp2E1=filename;_tag_dyneither(_tmp2E1,sizeof(char),_get_zero_arr_size_char((void*)_tmp2E1,1U));});_tmp408.f1=_tmp582;});_tmp408;});void*_tmp2DE[1U];_tmp2DE[0]=& _tmp2E0;({struct Cyc___cycFILE*_tmp584=out_file;struct _dyneither_ptr _tmp583=({const char*_tmp2DF="#include <%s>\n\n";_tag_dyneither(_tmp2DF,sizeof(char),16U);});Cyc_fprintf(_tmp584,_tmp583,_tag_dyneither(_tmp2DE,sizeof(void*),1U));});});
if(cycstubs != 0){
out_file=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);
{struct Cyc_List_List*_tmp2E2=cycstubs;for(0;_tmp2E2 != 0;_tmp2E2=_tmp2E2->tl){
struct _tuple23*_tmp2E3=(struct _tuple23*)_tmp2E2->hd;struct _tuple23*_tmp2E4=_tmp2E3;struct _dyneither_ptr _tmp2E7;struct _dyneither_ptr _tmp2E6;_LL86: _tmp2E7=_tmp2E4->f1;_tmp2E6=_tmp2E4->f2;_LL87:;
if(({char*_tmp587=(char*)_tmp2E6.curr;_tmp587 != (char*)(_tag_dyneither(0,0,0)).curr;}) && (
({char*_tmp586=(char*)_tmp2E7.curr;_tmp586 == (char*)(_tag_dyneither(0,0,0)).curr;}) || ({struct Cyc_Set_Set*_tmp585=defined_symbols;((int(*)(struct Cyc_Set_Set*s,struct _dyneither_ptr*elt))Cyc_Set_member)(_tmp585,({struct _dyneither_ptr*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));*_tmp2E5=_tmp2E7;_tmp2E5;}));})))
Cyc_fputs((const char*)_check_null(_untag_dyneither_ptr(_tmp2E6,sizeof(char),1U)),out_file);}}
# 1803
({void*_tmp2E8=0U;({struct Cyc___cycFILE*_tmp589=out_file;struct _dyneither_ptr _tmp588=({const char*_tmp2E9="\n";_tag_dyneither(_tmp2E9,sizeof(char),2U);});Cyc_fprintf(_tmp589,_tmp588,_tag_dyneither(_tmp2E8,sizeof(void*),0U));});});}{
# 1806
int _tmp2EA=0;_npop_handler(0U);return _tmp2EA;};};};};};};};};};
# 1439
;_pop_handler();}else{void*_tmp1DE=(void*)_exn_thrown;void*_tmp2EF=_tmp1DE;void*_tmp306;struct _dyneither_ptr _tmp305;struct _dyneither_ptr _tmp304;struct _dyneither_ptr _tmp303;struct _dyneither_ptr _tmp302;if(((struct Cyc_Core_Impossible_exn_struct*)_tmp2EF)->tag == Cyc_Core_Impossible){_LL89: _tmp302=((struct Cyc_Core_Impossible_exn_struct*)_tmp2EF)->f1;_LL8A:
# 1810
({struct Cyc_String_pa_PrintArg_struct _tmp2F2=({struct Cyc_String_pa_PrintArg_struct _tmp40A;_tmp40A.tag=0U,_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp302);_tmp40A;});void*_tmp2F0[1U];_tmp2F0[0]=& _tmp2F2;({struct Cyc___cycFILE*_tmp58B=Cyc_stderr;struct _dyneither_ptr _tmp58A=({const char*_tmp2F1="Got Core::Impossible(%s)\n";_tag_dyneither(_tmp2F1,sizeof(char),26U);});Cyc_fprintf(_tmp58B,_tmp58A,_tag_dyneither(_tmp2F0,sizeof(void*),1U));});});goto _LL88;}else{if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2EF)->tag == Cyc_Dict_Absent){_LL8B: _LL8C:
# 1812
({void*_tmp2F3=0U;({struct Cyc___cycFILE*_tmp58D=Cyc_stderr;struct _dyneither_ptr _tmp58C=({const char*_tmp2F4="Got Dict::Absent\n";_tag_dyneither(_tmp2F4,sizeof(char),18U);});Cyc_fprintf(_tmp58D,_tmp58C,_tag_dyneither(_tmp2F3,sizeof(void*),0U));});});goto _LL88;}else{if(((struct Cyc_Core_Failure_exn_struct*)_tmp2EF)->tag == Cyc_Core_Failure){_LL8D: _tmp303=((struct Cyc_Core_Failure_exn_struct*)_tmp2EF)->f1;_LL8E:
# 1814
({struct Cyc_String_pa_PrintArg_struct _tmp2F7=({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0U,_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp303);_tmp40B;});void*_tmp2F5[1U];_tmp2F5[0]=& _tmp2F7;({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _dyneither_ptr _tmp58E=({const char*_tmp2F6="Got Core::Failure(%s)\n";_tag_dyneither(_tmp2F6,sizeof(char),23U);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_dyneither(_tmp2F5,sizeof(void*),1U));});});goto _LL88;}else{if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2EF)->tag == Cyc_Core_Invalid_argument){_LL8F: _tmp304=((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp2EF)->f1;_LL90:
# 1816
({struct Cyc_String_pa_PrintArg_struct _tmp2FA=({struct Cyc_String_pa_PrintArg_struct _tmp40C;_tmp40C.tag=0U,_tmp40C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp304);_tmp40C;});void*_tmp2F8[1U];_tmp2F8[0]=& _tmp2FA;({struct Cyc___cycFILE*_tmp591=Cyc_stderr;struct _dyneither_ptr _tmp590=({const char*_tmp2F9="Got Invalid_argument(%s)\n";_tag_dyneither(_tmp2F9,sizeof(char),26U);});Cyc_fprintf(_tmp591,_tmp590,_tag_dyneither(_tmp2F8,sizeof(void*),1U));});});goto _LL88;}else{if(((struct Cyc_Core_Not_found_exn_struct*)_tmp2EF)->tag == Cyc_Core_Not_found){_LL91: _LL92:
# 1818
({void*_tmp2FB=0U;({struct Cyc___cycFILE*_tmp593=Cyc_stderr;struct _dyneither_ptr _tmp592=({const char*_tmp2FC="Got Not_found\n";_tag_dyneither(_tmp2FC,sizeof(char),15U);});Cyc_fprintf(_tmp593,_tmp592,_tag_dyneither(_tmp2FB,sizeof(void*),0U));});});goto _LL88;}else{if(((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2EF)->tag == Cyc_NO_SUPPORT){_LL93: _tmp305=((struct Cyc_NO_SUPPORT_exn_struct*)_tmp2EF)->f1;_LL94:
# 1820
({struct Cyc_String_pa_PrintArg_struct _tmp2FF=({struct Cyc_String_pa_PrintArg_struct _tmp40D;_tmp40D.tag=0U,_tmp40D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp305);_tmp40D;});void*_tmp2FD[1U];_tmp2FD[0]=& _tmp2FF;({struct Cyc___cycFILE*_tmp595=Cyc_stderr;struct _dyneither_ptr _tmp594=({const char*_tmp2FE="No support because %s\n";_tag_dyneither(_tmp2FE,sizeof(char),23U);});Cyc_fprintf(_tmp595,_tmp594,_tag_dyneither(_tmp2FD,sizeof(void*),1U));});});goto _LL88;}else{_LL95: _tmp306=_tmp2EF;_LL96:
# 1822
({void*_tmp300=0U;({struct Cyc___cycFILE*_tmp597=Cyc_stderr;struct _dyneither_ptr _tmp596=({const char*_tmp301="Got unknown exception\n";_tag_dyneither(_tmp301,sizeof(char),23U);});Cyc_fprintf(_tmp597,_tmp596,_tag_dyneither(_tmp300,sizeof(void*),0U));});});
Cyc_Core_rethrow(_tmp306);}}}}}}_LL88:;}};}
# 1828
maybe=Cyc_fopen(filename,"w");
if(!((unsigned int)maybe)){
({struct Cyc_String_pa_PrintArg_struct _tmp309=({struct Cyc_String_pa_PrintArg_struct _tmp40E;_tmp40E.tag=0U,({struct _dyneither_ptr _tmp598=(struct _dyneither_ptr)({const char*_tmp30A=filename;_tag_dyneither(_tmp30A,sizeof(char),_get_zero_arr_size_char((void*)_tmp30A,1U));});_tmp40E.f1=_tmp598;});_tmp40E;});void*_tmp307[1U];_tmp307[0]=& _tmp309;({struct Cyc___cycFILE*_tmp59A=Cyc_stderr;struct _dyneither_ptr _tmp599=({const char*_tmp308="Error: could not create file %s\n";_tag_dyneither(_tmp308,sizeof(char),33U);});Cyc_fprintf(_tmp59A,_tmp599,_tag_dyneither(_tmp307,sizeof(void*),1U));});});
return 1;}
# 1833
out_file=maybe;
({struct Cyc_String_pa_PrintArg_struct _tmp30D=({struct Cyc_String_pa_PrintArg_struct _tmp40F;_tmp40F.tag=0U,({
# 1836
struct _dyneither_ptr _tmp59B=(struct _dyneither_ptr)({const char*_tmp30E=filename;_tag_dyneither(_tmp30E,sizeof(char),_get_zero_arr_size_char((void*)_tmp30E,1U));});_tmp40F.f1=_tmp59B;});_tmp40F;});void*_tmp30B[1U];_tmp30B[0]=& _tmp30D;({struct Cyc___cycFILE*_tmp59D=out_file;struct _dyneither_ptr _tmp59C=({const char*_tmp30C="#error -- %s is not supported on this platform\n";_tag_dyneither(_tmp30C,sizeof(char),48U);});Cyc_fprintf(_tmp59D,_tmp59C,_tag_dyneither(_tmp30B,sizeof(void*),1U));});});
Cyc_fclose(out_file);
({struct Cyc_String_pa_PrintArg_struct _tmp311=({struct Cyc_String_pa_PrintArg_struct _tmp410;_tmp410.tag=0U,({
struct _dyneither_ptr _tmp59E=(struct _dyneither_ptr)({const char*_tmp312=filename;_tag_dyneither(_tmp312,sizeof(char),_get_zero_arr_size_char((void*)_tmp312,1U));});_tmp410.f1=_tmp59E;});_tmp410;});void*_tmp30F[1U];_tmp30F[0]=& _tmp311;({struct Cyc___cycFILE*_tmp5A0=Cyc_stderr;struct _dyneither_ptr _tmp59F=({const char*_tmp310="Warning: %s will not be supported on this platform\n";_tag_dyneither(_tmp310,sizeof(char),52U);});Cyc_fprintf(_tmp5A0,_tmp59F,_tag_dyneither(_tmp30F,sizeof(void*),1U));});});
({void*_tmp313=0U;({struct _dyneither_ptr _tmp5A1=({const char*_tmp314="Not supported on this platform\n";_tag_dyneither(_tmp314,sizeof(char),32U);});Cyc_log(_tmp5A1,_tag_dyneither(_tmp313,sizeof(void*),0U));});});
# 1847
return 0;};}
# 1851
int Cyc_process_specfile(const char*file,const char*dir){
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp32C=({struct Cyc_String_pa_PrintArg_struct _tmp417;_tmp417.tag=0U,({struct _dyneither_ptr _tmp5A2=(struct _dyneither_ptr)({const char*_tmp32D=file;_tag_dyneither(_tmp32D,sizeof(char),_get_zero_arr_size_char((void*)_tmp32D,1U));});_tmp417.f1=_tmp5A2;});_tmp417;});void*_tmp32A[1U];_tmp32A[0]=& _tmp32C;({struct Cyc___cycFILE*_tmp5A4=Cyc_stderr;struct _dyneither_ptr _tmp5A3=({const char*_tmp32B="Processing %s\n";_tag_dyneither(_tmp32B,sizeof(char),15U);});Cyc_fprintf(_tmp5A4,_tmp5A3,_tag_dyneither(_tmp32A,sizeof(void*),1U));});});{
struct Cyc___cycFILE*_tmp32E=Cyc_fopen(file,"r");
if(!((unsigned int)_tmp32E)){
({struct Cyc_String_pa_PrintArg_struct _tmp331=({struct Cyc_String_pa_PrintArg_struct _tmp418;_tmp418.tag=0U,({struct _dyneither_ptr _tmp5A5=(struct _dyneither_ptr)({const char*_tmp332=file;_tag_dyneither(_tmp332,sizeof(char),_get_zero_arr_size_char((void*)_tmp332,1U));});_tmp418.f1=_tmp5A5;});_tmp418;});void*_tmp32F[1U];_tmp32F[0]=& _tmp331;({struct Cyc___cycFILE*_tmp5A7=Cyc_stderr;struct _dyneither_ptr _tmp5A6=({const char*_tmp330="Error: could not open %s\n";_tag_dyneither(_tmp330,sizeof(char),26U);});Cyc_fprintf(_tmp5A7,_tmp5A6,_tag_dyneither(_tmp32F,sizeof(void*),1U));});});
return 1;}{
# 1859
struct Cyc___cycFILE*_tmp333=_tmp32E;
# 1863
struct _dyneither_ptr buf=({char*_tmp351=({unsigned int _tmp350=1024U + 1U;char*_tmp34F=_cycalloc_atomic(_check_times(_tmp350,sizeof(char)));({{unsigned int _tmp41E=1024U;unsigned int i;for(i=0;i < _tmp41E;++ i){_tmp34F[i]='\000';}_tmp34F[_tmp41E]=0;}0;});_tmp34F;});_tag_dyneither(_tmp351,sizeof(char),1025U);});
struct _dyneither_ptr _tmp334=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp337=({struct Cyc_String_pa_PrintArg_struct _tmp419;_tmp419.tag=0U,({struct _dyneither_ptr _tmp5A8=(struct _dyneither_ptr)({const char*_tmp338=dir;_tag_dyneither(_tmp338,sizeof(char),_get_zero_arr_size_char((void*)_tmp338,1U));});_tmp419.f1=_tmp5A8;});_tmp419;});void*_tmp335[1U];_tmp335[0]=& _tmp337;({struct Cyc___cycFILE*_tmp5AA=Cyc_stderr;struct _dyneither_ptr _tmp5A9=({const char*_tmp336="Error: can't change directory to %s\n";_tag_dyneither(_tmp336,sizeof(char),37U);});Cyc_fprintf(_tmp5AA,_tmp5A9,_tag_dyneither(_tmp335,sizeof(void*),1U));});});
return 1;}}
# 1871
if((int)Cyc_mode == (int)Cyc_GATHER){
# 1873
struct _dyneither_ptr _tmp339=({struct Cyc_String_pa_PrintArg_struct _tmp33F=({struct Cyc_String_pa_PrintArg_struct _tmp41C;_tmp41C.tag=0U,_tmp41C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp41C;});struct Cyc_String_pa_PrintArg_struct _tmp340=({struct Cyc_String_pa_PrintArg_struct _tmp41B;_tmp41B.tag=0U,_tmp41B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp41B;});void*_tmp33D[2U];_tmp33D[0]=& _tmp33F,_tmp33D[1]=& _tmp340;({struct _dyneither_ptr _tmp5AB=({const char*_tmp33E="echo | %s %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp33E,sizeof(char),39U);});Cyc_aprintf(_tmp5AB,_tag_dyneither(_tmp33D,sizeof(void*),2U));});});
# 1875
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp33C=({struct Cyc_String_pa_PrintArg_struct _tmp41A;_tmp41A.tag=0U,_tmp41A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp339);_tmp41A;});void*_tmp33A[1U];_tmp33A[0]=& _tmp33C;({struct Cyc___cycFILE*_tmp5AD=Cyc_stderr;struct _dyneither_ptr _tmp5AC=({const char*_tmp33B="%s\n";_tag_dyneither(_tmp33B,sizeof(char),4U);});Cyc_fprintf(_tmp5AD,_tmp5AC,_tag_dyneither(_tmp33A,sizeof(void*),1U));});});
 system((const char*)_check_null(_untag_dyneither_ptr(_tmp339,sizeof(char),1U)));}{
# 1880
struct Cyc_Lexing_lexbuf*_tmp341=Cyc_Lexing_from_file(_tmp333);
struct _tuple25*entry;
while((entry=((struct _tuple25*(*)(struct Cyc_Lexing_lexbuf*lexbuf))Cyc_spec)(_tmp341))!= 0){
struct _tuple25*_tmp342=(struct _tuple25*)_check_null(entry);struct _tuple25*_tmp343=_tmp342;struct _dyneither_ptr _tmp34B;struct Cyc_List_List*_tmp34A;struct Cyc_List_List*_tmp349;struct Cyc_List_List*_tmp348;struct Cyc_List_List*_tmp347;struct Cyc_List_List*_tmp346;struct Cyc_List_List*_tmp345;struct Cyc_List_List*_tmp344;_LL1: _tmp34B=_tmp343->f1;_tmp34A=_tmp343->f2;_tmp349=_tmp343->f3;_tmp348=_tmp343->f4;_tmp347=_tmp343->f5;_tmp346=_tmp343->f6;_tmp345=_tmp343->f7;_tmp344=_tmp343->f8;_LL2:;
# 1885
if(Cyc_process_file((const char*)_check_null(_untag_dyneither_ptr(_tmp34B,sizeof(char),1U)),_tmp34A,_tmp349,_tmp348,_tmp347,_tmp346,_tmp345,_tmp344))
# 1887
return 1;}
# 1889
Cyc_fclose(_tmp333);
# 1891
if((int)Cyc_mode != (int)Cyc_GATHERSCRIPT){
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp334,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp34E=({struct Cyc_String_pa_PrintArg_struct _tmp41D;_tmp41D.tag=0U,_tmp41D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp334);_tmp41D;});void*_tmp34C[1U];_tmp34C[0]=& _tmp34E;({struct Cyc___cycFILE*_tmp5AF=Cyc_stderr;struct _dyneither_ptr _tmp5AE=({const char*_tmp34D="Error: could not change directory to %s\n";_tag_dyneither(_tmp34D,sizeof(char),41U);});Cyc_fprintf(_tmp5AF,_tmp5AE,_tag_dyneither(_tmp34C,sizeof(void*),1U));});});
return 1;}}
# 1897
return 0;};};};}
# 1901
int Cyc_process_setjmp(const char*dir){
# 1904
struct _dyneither_ptr buf=({char*_tmp363=({unsigned int _tmp362=1024U + 1U;char*_tmp361=_cycalloc_atomic(_check_times(_tmp362,sizeof(char)));({{unsigned int _tmp421=1024U;unsigned int i;for(i=0;i < _tmp421;++ i){_tmp361[i]='\000';}_tmp361[_tmp421]=0;}0;});_tmp361;});_tag_dyneither(_tmp363,sizeof(char),1025U);});
struct _dyneither_ptr _tmp352=Cyc_getcwd(buf,_get_dyneither_size(buf,sizeof(char)));
if( chdir(dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp355=({struct Cyc_String_pa_PrintArg_struct _tmp41F;_tmp41F.tag=0U,({struct _dyneither_ptr _tmp5B0=(struct _dyneither_ptr)({const char*_tmp356=dir;_tag_dyneither(_tmp356,sizeof(char),_get_zero_arr_size_char((void*)_tmp356,1U));});_tmp41F.f1=_tmp5B0;});_tmp41F;});void*_tmp353[1U];_tmp353[0]=& _tmp355;({struct Cyc___cycFILE*_tmp5B2=Cyc_stderr;struct _dyneither_ptr _tmp5B1=({const char*_tmp354="Error: can't change directory to %s\n";_tag_dyneither(_tmp354,sizeof(char),37U);});Cyc_fprintf(_tmp5B2,_tmp5B1,_tag_dyneither(_tmp353,sizeof(void*),1U));});});
return 1;}
# 1910
if(({struct Cyc_List_List*_tmp5B8=({struct _dyneither_ptr*_tmp357[1U];({struct _dyneither_ptr*_tmp5B4=({struct _dyneither_ptr*_tmp359=_cycalloc(sizeof(*_tmp359));({struct _dyneither_ptr _tmp5B3=({const char*_tmp358="jmp_buf";_tag_dyneither(_tmp358,sizeof(char),8U);});*_tmp359=_tmp5B3;});_tmp359;});_tmp357[0]=_tmp5B4;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp357,sizeof(struct _dyneither_ptr*),1U));});Cyc_process_file("setjmp.h",_tmp5B8,0,0,({struct _tuple23*_tmp35A[1U];({struct _tuple23*_tmp5B7=({struct _tuple23*_tmp35D=_cycalloc(sizeof(*_tmp35D));
({struct _dyneither_ptr _tmp5B6=({const char*_tmp35B="setjmp";_tag_dyneither(_tmp35B,sizeof(char),7U);});_tmp35D->f1=_tmp5B6;}),({struct _dyneither_ptr _tmp5B5=({const char*_tmp35C="extern int setjmp(jmp_buf);\n";_tag_dyneither(_tmp35C,sizeof(char),29U);});_tmp35D->f2=_tmp5B5;});_tmp35D;});_tmp35A[0]=_tmp5B7;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35A,sizeof(struct _tuple23*),1U));}),0,0,0);}))
# 1913
return 1;
if( chdir((const char*)((char*)_check_null(_untag_dyneither_ptr(_tmp352,sizeof(char),1U))))){
({struct Cyc_String_pa_PrintArg_struct _tmp360=({struct Cyc_String_pa_PrintArg_struct _tmp420;_tmp420.tag=0U,_tmp420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp352);_tmp420;});void*_tmp35E[1U];_tmp35E[0]=& _tmp360;({struct Cyc___cycFILE*_tmp5BA=Cyc_stderr;struct _dyneither_ptr _tmp5B9=({const char*_tmp35F="Error: could not change directory to %s\n";_tag_dyneither(_tmp35F,sizeof(char),41U);});Cyc_fprintf(_tmp5BA,_tmp5B9,_tag_dyneither(_tmp35E,sizeof(void*),1U));});});
return 1;}
# 1918
return 0;}static char _tmp364[13U]="BUILDLIB.OUT";
# 1922
static struct _dyneither_ptr Cyc_output_dir={_tmp364,_tmp364,_tmp364 + 13U};
static void Cyc_set_output_dir(struct _dyneither_ptr s){
Cyc_output_dir=s;}
# 1926
static struct Cyc_List_List*Cyc_spec_files=0;
static void Cyc_add_spec_file(struct _dyneither_ptr s){
Cyc_spec_files=({struct Cyc_List_List*_tmp365=_cycalloc(sizeof(*_tmp365));_tmp365->hd=(const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1U)),_tmp365->tl=Cyc_spec_files;_tmp365;});}
# 1930
static int Cyc_no_other(struct _dyneither_ptr s){return 0;}
static void Cyc_set_GATHER(){
Cyc_mode=Cyc_GATHER;}
# 1934
static void Cyc_set_GATHERSCRIPT(){
Cyc_mode=Cyc_GATHERSCRIPT;}
# 1937
static void Cyc_set_FINISH(){
Cyc_mode=Cyc_FINISH;}
# 1940
static void Cyc_add_cpparg(struct _dyneither_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp367=_cycalloc(sizeof(*_tmp367));({struct _dyneither_ptr*_tmp5BB=({struct _dyneither_ptr*_tmp366=_cycalloc(sizeof(*_tmp366));*_tmp366=s;_tmp366;});_tmp367->hd=_tmp5BB;}),_tmp367->tl=Cyc_cppargs;_tmp367;});}
# 1943
static int Cyc_badparse=0;
static void Cyc_unsupported_option(struct _dyneither_ptr s){
({struct Cyc_String_pa_PrintArg_struct _tmp36A=({struct Cyc_String_pa_PrintArg_struct _tmp422;_tmp422.tag=0U,_tmp422.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp422;});void*_tmp368[1U];_tmp368[0]=& _tmp36A;({struct Cyc___cycFILE*_tmp5BD=Cyc_stderr;struct _dyneither_ptr _tmp5BC=({const char*_tmp369="Unsupported option %s\n";_tag_dyneither(_tmp369,sizeof(char),23U);});Cyc_fprintf(_tmp5BD,_tmp5BC,_tag_dyneither(_tmp368,sizeof(void*),1U));});});
Cyc_badparse=1;}
# 1952
void GC_blacklist_warn_clear();struct _tuple27{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
int Cyc_main(int argc,struct _dyneither_ptr argv){
 GC_blacklist_warn_clear();{
# 1956
struct Cyc_List_List*options=({struct _tuple27*_tmp3A2[9U];({
struct _tuple27*_tmp5EA=({struct _tuple27*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));({struct _dyneither_ptr _tmp5E9=({const char*_tmp3A3="-d";_tag_dyneither(_tmp3A3,sizeof(char),3U);});_tmp3A7->f1=_tmp5E9;}),_tmp3A7->f2=0,({struct _dyneither_ptr _tmp5E8=({const char*_tmp3A4=" <file>";_tag_dyneither(_tmp3A4,sizeof(char),8U);});_tmp3A7->f3=_tmp5E8;}),({
void*_tmp5E7=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->tag=5U,_tmp3A5->f1=Cyc_set_output_dir;_tmp3A5;});_tmp3A7->f4=_tmp5E7;}),({
struct _dyneither_ptr _tmp5E6=({const char*_tmp3A6="Set the output directory to <file>";_tag_dyneither(_tmp3A6,sizeof(char),35U);});_tmp3A7->f5=_tmp5E6;});_tmp3A7;});
# 1957
_tmp3A2[0]=_tmp5EA;}),({
# 1960
struct _tuple27*_tmp5E5=({struct _tuple27*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({struct _dyneither_ptr _tmp5E4=({const char*_tmp3A8="-gather";_tag_dyneither(_tmp3A8,sizeof(char),8U);});_tmp3AC->f1=_tmp5E4;}),_tmp3AC->f2=0,({struct _dyneither_ptr _tmp5E3=({const char*_tmp3A9="";_tag_dyneither(_tmp3A9,sizeof(char),1U);});_tmp3AC->f3=_tmp5E3;}),({
void*_tmp5E2=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3AA=_cycalloc(sizeof(*_tmp3AA));_tmp3AA->tag=0U,_tmp3AA->f1=Cyc_set_GATHER;_tmp3AA;});_tmp3AC->f4=_tmp5E2;}),({
struct _dyneither_ptr _tmp5E1=({const char*_tmp3AB="Gather C library info but don't produce Cyclone headers";_tag_dyneither(_tmp3AB,sizeof(char),56U);});_tmp3AC->f5=_tmp5E1;});_tmp3AC;});
# 1960
_tmp3A2[1]=_tmp5E5;}),({
# 1963
struct _tuple27*_tmp5E0=({struct _tuple27*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));({struct _dyneither_ptr _tmp5DF=({const char*_tmp3AD="-gatherscript";_tag_dyneither(_tmp3AD,sizeof(char),14U);});_tmp3B1->f1=_tmp5DF;}),_tmp3B1->f2=0,({struct _dyneither_ptr _tmp5DE=({const char*_tmp3AE="";_tag_dyneither(_tmp3AE,sizeof(char),1U);});_tmp3B1->f3=_tmp5DE;}),({
void*_tmp5DD=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->tag=0U,_tmp3AF->f1=Cyc_set_GATHERSCRIPT;_tmp3AF;});_tmp3B1->f4=_tmp5DD;}),({
struct _dyneither_ptr _tmp5DC=({const char*_tmp3B0="Produce a script to gather C library info";_tag_dyneither(_tmp3B0,sizeof(char),42U);});_tmp3B1->f5=_tmp5DC;});_tmp3B1;});
# 1963
_tmp3A2[2]=_tmp5E0;}),({
# 1966
struct _tuple27*_tmp5DB=({struct _tuple27*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));({struct _dyneither_ptr _tmp5DA=({const char*_tmp3B2="-finish";_tag_dyneither(_tmp3B2,sizeof(char),8U);});_tmp3B6->f1=_tmp5DA;}),_tmp3B6->f2=0,({struct _dyneither_ptr _tmp5D9=({const char*_tmp3B3="";_tag_dyneither(_tmp3B3,sizeof(char),1U);});_tmp3B6->f3=_tmp5D9;}),({
void*_tmp5D8=(void*)({struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_tmp3B4=_cycalloc(sizeof(*_tmp3B4));_tmp3B4->tag=0U,_tmp3B4->f1=Cyc_set_FINISH;_tmp3B4;});_tmp3B6->f4=_tmp5D8;}),({
struct _dyneither_ptr _tmp5D7=({const char*_tmp3B5="Produce Cyclone headers from pre-gathered C library info";_tag_dyneither(_tmp3B5,sizeof(char),57U);});_tmp3B6->f5=_tmp5D7;});_tmp3B6;});
# 1966
_tmp3A2[3]=_tmp5DB;}),({
# 1969
struct _tuple27*_tmp5D6=({struct _tuple27*_tmp3BB=_cycalloc(sizeof(*_tmp3BB));({struct _dyneither_ptr _tmp5D5=({const char*_tmp3B7="-setjmp";_tag_dyneither(_tmp3B7,sizeof(char),8U);});_tmp3BB->f1=_tmp5D5;}),_tmp3BB->f2=0,({struct _dyneither_ptr _tmp5D4=({const char*_tmp3B8="";_tag_dyneither(_tmp3B8,sizeof(char),1U);});_tmp3BB->f3=_tmp5D4;}),({
void*_tmp5D3=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=3U,_tmp3B9->f1=& Cyc_do_setjmp;_tmp3B9;});_tmp3BB->f4=_tmp5D3;}),({
# 1974
struct _dyneither_ptr _tmp5D2=({const char*_tmp3BA="Produce the jmp_buf and setjmp declarations on the standard output, for use by the Cyclone compiler special file cyc_setjmp.h.  Cannot be used with -gather, -gatherscript, or specfiles.";_tag_dyneither(_tmp3BA,sizeof(char),186U);});_tmp3BB->f5=_tmp5D2;});_tmp3BB;});
# 1969
_tmp3A2[4]=_tmp5D6;}),({
# 1975
struct _tuple27*_tmp5D1=({struct _tuple27*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct _dyneither_ptr _tmp5D0=({const char*_tmp3BC="-b";_tag_dyneither(_tmp3BC,sizeof(char),3U);});_tmp3C0->f1=_tmp5D0;}),_tmp3C0->f2=0,({struct _dyneither_ptr _tmp5CF=({const char*_tmp3BD=" <machine>";_tag_dyneither(_tmp3BD,sizeof(char),11U);});_tmp3C0->f3=_tmp5CF;}),({
void*_tmp5CE=(void*)({struct Cyc_Arg_String_spec_Arg_Spec_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->tag=5U,_tmp3BE->f1=Cyc_Specsfile_set_target_arch;_tmp3BE;});_tmp3C0->f4=_tmp5CE;}),({
struct _dyneither_ptr _tmp5CD=({const char*_tmp3BF="Set the target machine for compilation to <machine>";_tag_dyneither(_tmp3BF,sizeof(char),52U);});_tmp3C0->f5=_tmp5CD;});_tmp3C0;});
# 1975
_tmp3A2[5]=_tmp5D1;}),({
# 1978
struct _tuple27*_tmp5CC=({struct _tuple27*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));({struct _dyneither_ptr _tmp5CB=({const char*_tmp3C1="-B";_tag_dyneither(_tmp3C1,sizeof(char),3U);});_tmp3C5->f1=_tmp5CB;}),_tmp3C5->f2=1,({struct _dyneither_ptr _tmp5CA=({const char*_tmp3C2="<file>";_tag_dyneither(_tmp3C2,sizeof(char),7U);});_tmp3C5->f3=_tmp5CA;}),({
void*_tmp5C9=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->tag=1U,_tmp3C3->f1=Cyc_Specsfile_add_cyclone_exec_path;_tmp3C3;});_tmp3C5->f4=_tmp5C9;}),({
struct _dyneither_ptr _tmp5C8=({const char*_tmp3C4="Add to the list of directories to search for compiler files";_tag_dyneither(_tmp3C4,sizeof(char),60U);});_tmp3C5->f5=_tmp5C8;});_tmp3C5;});
# 1978
_tmp3A2[6]=_tmp5CC;}),({
# 1981
struct _tuple27*_tmp5C7=({struct _tuple27*_tmp3CA=_cycalloc(sizeof(*_tmp3CA));({struct _dyneither_ptr _tmp5C6=({const char*_tmp3C6="-v";_tag_dyneither(_tmp3C6,sizeof(char),3U);});_tmp3CA->f1=_tmp5C6;}),_tmp3CA->f2=0,({struct _dyneither_ptr _tmp5C5=({const char*_tmp3C7="";_tag_dyneither(_tmp3C7,sizeof(char),1U);});_tmp3CA->f3=_tmp5C5;}),({
void*_tmp5C4=(void*)({struct Cyc_Arg_Set_spec_Arg_Spec_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=3U,_tmp3C8->f1=& Cyc_verbose;_tmp3C8;});_tmp3CA->f4=_tmp5C4;}),({
struct _dyneither_ptr _tmp5C3=({const char*_tmp3C9="Verbose operation";_tag_dyneither(_tmp3C9,sizeof(char),18U);});_tmp3CA->f5=_tmp5C3;});_tmp3CA;});
# 1981
_tmp3A2[7]=_tmp5C7;}),({
# 1984
struct _tuple27*_tmp5C2=({struct _tuple27*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));({struct _dyneither_ptr _tmp5C1=({const char*_tmp3CB="-";_tag_dyneither(_tmp3CB,sizeof(char),2U);});_tmp3CF->f1=_tmp5C1;}),_tmp3CF->f2=1,({struct _dyneither_ptr _tmp5C0=({const char*_tmp3CC="";_tag_dyneither(_tmp3CC,sizeof(char),1U);});_tmp3CF->f3=_tmp5C0;}),({
void*_tmp5BF=(void*)({struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->tag=1U,_tmp3CD->f1=Cyc_add_cpparg;_tmp3CD;});_tmp3CF->f4=_tmp5BF;}),({
struct _dyneither_ptr _tmp5BE=({const char*_tmp3CE="";_tag_dyneither(_tmp3CE,sizeof(char),1U);});_tmp3CF->f5=_tmp5BE;});_tmp3CF;});
# 1984
_tmp3A2[8]=_tmp5C2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A2,sizeof(struct _tuple27*),9U));});
# 1989
struct _dyneither_ptr _tmp36B=({struct Cyc_List_List*_tmp5EC=options;struct _dyneither_ptr _tmp5EB=({const char*_tmp3A1="Options:";_tag_dyneither(_tmp3A1,sizeof(char),9U);});Cyc_Specsfile_parse_b(_tmp5EC,Cyc_add_spec_file,Cyc_no_other,_tmp5EB,argv);});
# 1991
Cyc_Arg_current=0;
({struct Cyc_List_List*_tmp5EE=options;struct _dyneither_ptr _tmp5ED=({const char*_tmp36C="Options:";_tag_dyneither(_tmp36C,sizeof(char),9U);});Cyc_Arg_parse(_tmp5EE,Cyc_add_spec_file,Cyc_no_other,_tmp5ED,_tmp36B);});
if((((Cyc_badparse  || 
!Cyc_do_setjmp  && Cyc_spec_files == 0) || 
Cyc_do_setjmp  && Cyc_spec_files != 0) || 
Cyc_do_setjmp  && (int)Cyc_mode == (int)Cyc_GATHER) || 
Cyc_do_setjmp  && (int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_List_List*_tmp5EF=options;Cyc_Arg_usage(_tmp5EF,({const char*_tmp36D="Usage: buildlib [options] specfile1 specfile2 ...\nOptions:";_tag_dyneither(_tmp36D,sizeof(char),59U);}));});
# 2001
return 1;}{
# 2007
struct _dyneither_ptr _tmp36E=Cyc_Specsfile_find_in_arch_path(({const char*_tmp3A0="cycspecs";_tag_dyneither(_tmp3A0,sizeof(char),9U);}));
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp371=({struct Cyc_String_pa_PrintArg_struct _tmp423;_tmp423.tag=0U,_tmp423.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp36E);_tmp423;});void*_tmp36F[1U];_tmp36F[0]=& _tmp371;({struct Cyc___cycFILE*_tmp5F1=Cyc_stderr;struct _dyneither_ptr _tmp5F0=({const char*_tmp370="Reading from specs file %s\n";_tag_dyneither(_tmp370,sizeof(char),28U);});Cyc_fprintf(_tmp5F1,_tmp5F0,_tag_dyneither(_tmp36F,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp372=Cyc_Specsfile_read_specs(_tmp36E);
Cyc_target_cflags=({struct Cyc_List_List*_tmp5F2=_tmp372;Cyc_Specsfile_get_spec(_tmp5F2,({const char*_tmp373="cyclone_target_cflags";_tag_dyneither(_tmp373,sizeof(char),22U);}));});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp376=({struct Cyc_String_pa_PrintArg_struct _tmp424;_tmp424.tag=0U,_tmp424.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp424;});void*_tmp374[1U];_tmp374[0]=& _tmp376;({struct Cyc___cycFILE*_tmp5F4=Cyc_stderr;struct _dyneither_ptr _tmp5F3=({const char*_tmp375="Target cflags are %s\n";_tag_dyneither(_tmp375,sizeof(char),22U);});Cyc_fprintf(_tmp5F4,_tmp5F3,_tag_dyneither(_tmp374,sizeof(void*),1U));});});
Cyc_cyclone_cc=({struct Cyc_List_List*_tmp5F5=_tmp372;Cyc_Specsfile_get_spec(_tmp5F5,({const char*_tmp377="cyclone_cc";_tag_dyneither(_tmp377,sizeof(char),11U);}));});
if(!((unsigned int)Cyc_cyclone_cc.curr))Cyc_cyclone_cc=({const char*_tmp378="gcc";_tag_dyneither(_tmp378,sizeof(char),4U);});
if(Cyc_verbose)({struct Cyc_String_pa_PrintArg_struct _tmp37B=({struct Cyc_String_pa_PrintArg_struct _tmp425;_tmp425.tag=0U,_tmp425.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cyclone_cc);_tmp425;});void*_tmp379[1U];_tmp379[0]=& _tmp37B;({struct Cyc___cycFILE*_tmp5F7=Cyc_stderr;struct _dyneither_ptr _tmp5F6=({const char*_tmp37A="C compiler is %s\n";_tag_dyneither(_tmp37A,sizeof(char),18U);});Cyc_fprintf(_tmp5F7,_tmp5F6,_tag_dyneither(_tmp379,sizeof(void*),1U));});});
# 2016
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
if(Cyc_verbose)
({void*_tmp37C=0U;({struct Cyc___cycFILE*_tmp5F9=Cyc_stderr;struct _dyneither_ptr _tmp5F8=({const char*_tmp37D="Creating BUILDLIB.sh\n";_tag_dyneither(_tmp37D,sizeof(char),22U);});Cyc_fprintf(_tmp5F9,_tmp5F8,_tag_dyneither(_tmp37C,sizeof(void*),0U));});});
Cyc_script_file=Cyc_fopen("BUILDLIB.sh","w");
if(!((unsigned int)Cyc_script_file)){
({void*_tmp37E=0U;({struct Cyc___cycFILE*_tmp5FB=Cyc_stderr;struct _dyneither_ptr _tmp5FA=({const char*_tmp37F="Could not create file BUILDLIB.sh\n";_tag_dyneither(_tmp37F,sizeof(char),35U);});Cyc_fprintf(_tmp5FB,_tmp5FA,_tag_dyneither(_tmp37E,sizeof(void*),0U));});});
 exit(1);}
# 2024
({void*_tmp380=0U;({struct _dyneither_ptr _tmp5FC=({const char*_tmp381="#!/bin/sh\n";_tag_dyneither(_tmp381,sizeof(char),11U);});Cyc_prscript(_tmp5FC,_tag_dyneither(_tmp380,sizeof(void*),0U));});});
({void*_tmp382=0U;({struct _dyneither_ptr _tmp5FD=({const char*_tmp383="GCC=\"gcc\"\n";_tag_dyneither(_tmp383,sizeof(char),11U);});Cyc_prscript(_tmp5FD,_tag_dyneither(_tmp382,sizeof(void*),0U));});});}
# 2029
if(Cyc_force_directory_prefixes(Cyc_output_dir) || Cyc_force_directory(Cyc_output_dir)){
({struct Cyc_String_pa_PrintArg_struct _tmp386=({struct Cyc_String_pa_PrintArg_struct _tmp426;_tmp426.tag=0U,_tmp426.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp426;});void*_tmp384[1U];_tmp384[0]=& _tmp386;({struct Cyc___cycFILE*_tmp5FF=Cyc_stderr;struct _dyneither_ptr _tmp5FE=({const char*_tmp385="Error: could not create directory %s\n";_tag_dyneither(_tmp385,sizeof(char),38U);});Cyc_fprintf(_tmp5FF,_tmp5FE,_tag_dyneither(_tmp384,sizeof(void*),1U));});});
return 1;}
# 2033
if(Cyc_verbose)
({struct Cyc_String_pa_PrintArg_struct _tmp389=({struct Cyc_String_pa_PrintArg_struct _tmp427;_tmp427.tag=0U,_tmp427.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp427;});void*_tmp387[1U];_tmp387[0]=& _tmp389;({struct Cyc___cycFILE*_tmp601=Cyc_stderr;struct _dyneither_ptr _tmp600=({const char*_tmp388="Output directory is %s\n";_tag_dyneither(_tmp388,sizeof(char),24U);});Cyc_fprintf(_tmp601,_tmp600,_tag_dyneither(_tmp387,sizeof(void*),1U));});});
# 2036
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT){
({struct Cyc_String_pa_PrintArg_struct _tmp38C=({struct Cyc_String_pa_PrintArg_struct _tmp428;_tmp428.tag=0U,_tmp428.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp428;});void*_tmp38A[1U];_tmp38A[0]=& _tmp38C;({struct _dyneither_ptr _tmp602=({const char*_tmp38B="cd %s\n";_tag_dyneither(_tmp38B,sizeof(char),7U);});Cyc_prscript(_tmp602,_tag_dyneither(_tmp38A,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp38F=({struct Cyc_String_pa_PrintArg_struct _tmp429;_tmp429.tag=0U,_tmp429.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_target_cflags);_tmp429;});void*_tmp38D[1U];_tmp38D[0]=& _tmp38F;({struct _dyneither_ptr _tmp603=({const char*_tmp38E="echo | $GCC %s -E -dM - -o INITMACROS.h\n";_tag_dyneither(_tmp38E,sizeof(char),41U);});Cyc_prscript(_tmp603,_tag_dyneither(_tmp38D,sizeof(void*),1U));});});}
# 2042
if(!Cyc_gathering()){
# 2045
Cyc_log_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp604=Cyc_output_dir;Cyc_Filename_concat(_tmp604,({const char*_tmp390="BUILDLIB.LOG";_tag_dyneither(_tmp390,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned int)Cyc_log_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp42A;_tmp42A.tag=0U,_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp42A;});void*_tmp391[1U];_tmp391[0]=& _tmp393;({struct Cyc___cycFILE*_tmp606=Cyc_stderr;struct _dyneither_ptr _tmp605=({const char*_tmp392="Error: could not create log file in directory %s\n";_tag_dyneither(_tmp392,sizeof(char),50U);});Cyc_fprintf(_tmp606,_tmp605,_tag_dyneither(_tmp391,sizeof(void*),1U));});});
return 1;}
# 2051
if(!Cyc_do_setjmp){
# 2053
Cyc_cstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp607=Cyc_output_dir;Cyc_Filename_concat(_tmp607,({const char*_tmp394="cstubs.c";_tag_dyneither(_tmp394,sizeof(char),9U);}));}),sizeof(char),1U)),"w");
if(!((unsigned int)Cyc_cstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp397=({struct Cyc_String_pa_PrintArg_struct _tmp42B;_tmp42B.tag=0U,_tmp42B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp42B;});void*_tmp395[1U];_tmp395[0]=& _tmp397;({struct Cyc___cycFILE*_tmp609=Cyc_stderr;struct _dyneither_ptr _tmp608=({const char*_tmp396="Error: could not create cstubs.c in directory %s\n";_tag_dyneither(_tmp396,sizeof(char),50U);});Cyc_fprintf(_tmp609,_tmp608,_tag_dyneither(_tmp395,sizeof(void*),1U));});});
return 1;}
# 2060
Cyc_cycstubs_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(({struct _dyneither_ptr _tmp60A=Cyc_output_dir;Cyc_Filename_concat(_tmp60A,({const char*_tmp398="cycstubs.cyc";_tag_dyneither(_tmp398,sizeof(char),13U);}));}),sizeof(char),1U)),"w");
if(!((unsigned int)Cyc_cycstubs_file)){
({struct Cyc_String_pa_PrintArg_struct _tmp39B=({struct Cyc_String_pa_PrintArg_struct _tmp42C;_tmp42C.tag=0U,_tmp42C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_output_dir);_tmp42C;});void*_tmp399[1U];_tmp399[0]=& _tmp39B;({struct Cyc___cycFILE*_tmp60C=Cyc_stderr;struct _dyneither_ptr _tmp60B=({const char*_tmp39A="Error: could not create cycstubs.c in directory %s\n";_tag_dyneither(_tmp39A,sizeof(char),52U);});Cyc_fprintf(_tmp60C,_tmp60B,_tag_dyneither(_tmp399,sizeof(void*),1U));});});
# 2065
return 1;}
# 2067
({void*_tmp39C=0U;({struct Cyc___cycFILE*_tmp60E=(struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file);struct _dyneither_ptr _tmp60D=({const char*_tmp39D="#include <core.h>\nusing Core;\n\n";_tag_dyneither(_tmp39D,sizeof(char),32U);});Cyc_fprintf(_tmp60E,_tmp60D,_tag_dyneither(_tmp39C,sizeof(void*),0U));});});}}{
# 2074
const char*outdir=(const char*)_check_null(_untag_dyneither_ptr(Cyc_output_dir,sizeof(char),1U));
if(Cyc_do_setjmp  && Cyc_process_setjmp(outdir))
return 1;else{
# 2080
for(0;Cyc_spec_files != 0;Cyc_spec_files=((struct Cyc_List_List*)_check_null(Cyc_spec_files))->tl){
if(Cyc_process_specfile((const char*)((struct Cyc_List_List*)_check_null(Cyc_spec_files))->hd,outdir)){
({void*_tmp39E=0U;({struct Cyc___cycFILE*_tmp610=Cyc_stderr;struct _dyneither_ptr _tmp60F=({const char*_tmp39F="FATAL ERROR -- QUIT!\n";_tag_dyneither(_tmp39F,sizeof(char),22U);});Cyc_fprintf(_tmp610,_tmp60F,_tag_dyneither(_tmp39E,sizeof(void*),0U));});});
 exit(1);}}}
# 2088
if((int)Cyc_mode == (int)Cyc_GATHERSCRIPT)
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_script_file));else{
# 2091
if(!Cyc_gathering()){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_log_file));
if(!Cyc_do_setjmp){
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cstubs_file));
Cyc_fclose((struct Cyc___cycFILE*)_check_null(Cyc_cycstubs_file));}}}
# 2099
return 0;};};};};}
