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

# 69 "core.h"
 void exit(int);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 79
int Cyc_fclose(struct Cyc___cycFILE*);
# 84
int Cyc_feof(struct Cyc___cycFILE*);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 98
struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 108
unsigned long Cyc_fread(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 140 "cycboot.h"
unsigned long Cyc_fwrite(struct _dyneither_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 167
int remove(const char*);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 271 "cycboot.h"
struct Cyc___cycFILE*Cyc_file_open(struct _dyneither_ptr,struct _dyneither_ptr);
void Cyc_file_close(struct Cyc___cycFILE*);
# 313
char* getenv(const char*);
# 318
int system(const char*);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 277 "core.h"
void Cyc_Core_rethrow(void*);
# 281
const char*Cyc_Core_get_exn_name(void*);
# 283
const char*Cyc_Core_get_exn_filename();
# 290
int Cyc_Core_get_exn_lineno();struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)();};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr,struct _dyneither_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _dyneither_ptr);};
# 66 "arg.h"
void Cyc_Arg_usage(struct Cyc_List_List*,struct _dyneither_ptr);
# 69
extern int Cyc_Arg_current;
# 71
void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _dyneither_ptr),int(*anonflagfun)(struct _dyneither_ptr),struct _dyneither_ptr errmsg,struct _dyneither_ptr args);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
# 51
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
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
# 119
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);
# 30 "filename.h"
struct _dyneither_ptr Cyc_Filename_concat(struct _dyneither_ptr,struct _dyneither_ptr);
# 34
struct _dyneither_ptr Cyc_Filename_chop_extension(struct _dyneither_ptr);
# 40
struct _dyneither_ptr Cyc_Filename_dirname(struct _dyneither_ptr);
# 43
struct _dyneither_ptr Cyc_Filename_basename(struct _dyneither_ptr);
# 46
int Cyc_Filename_check_suffix(struct _dyneither_ptr,struct _dyneither_ptr);
# 28 "position.h"
void Cyc_Position_reset_position(struct _dyneither_ptr);struct Cyc_Position_Error;
# 46
extern int Cyc_Position_use_gcc_style_location;
# 48
extern int Cyc_Position_max_errors;
# 50
int Cyc_Position_error_p();struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1182 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1184
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _dyneither_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _dyneither_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _dyneither_ptr lex_base;struct _dyneither_ptr lex_backtrk;struct _dyneither_ptr lex_default;struct _dyneither_ptr lex_trans;struct _dyneither_ptr lex_check;};
# 28 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned int loc;};struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};struct _tuple11{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple10{struct _tuple10*tl;struct _tuple11 hd  __attribute__((aligned )) ;};
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc  = 0U,Cyc_Extern_sc  = 1U,Cyc_ExternC_sc  = 2U,Cyc_Static_sc  = 3U,Cyc_Auto_sc  = 4U,Cyc_Register_sc  = 5U,Cyc_Abstract_sc  = 6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _dyneither_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple12{int f1;struct _dyneither_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple12 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple13{unsigned int f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple13*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple14{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple11 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple10*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple16{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple17{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple17 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple18*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY53{int tag;void*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 57
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 26 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 29 "binding.h"
extern int Cyc_Binding_warn_override;
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _dyneither_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned int loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned int,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 135 "tcutil.h"
extern int Cyc_Tcutil_warn_alias_coerce;
# 138
extern int Cyc_Tcutil_warn_region_coerce;
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 33
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 37
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 34 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
void*Cyc_Toc_typ_to_c(void*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
void Cyc_Toc_finish();
# 26 "remove_aggregates.h"
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*decls);
# 27 "toseqc.h"
struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*decls);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple19{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple19 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 39 "insert_checks.h"
extern int Cyc_InsertChecks_warn_bounds_checks;
extern int Cyc_InsertChecks_warn_all_null_deref;
# 42
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*);struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty();
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final();
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple20{struct _dyneither_ptr f1;struct _dyneither_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple20*info);
# 72 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
void*Cyc_Warn_vimpos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 44
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
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
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 40 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 43
extern int Cyc_ParseErrors_print_state_and_token;
# 46
extern int Cyc_Lex_compile_for_boot_r;
void Cyc_Lex_pos_init();
void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 57
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
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
# 89
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _dyneither_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));({struct _dyneither_ptr*_tmp2B3=({struct _dyneither_ptr*_tmp0=_cycalloc(sizeof(*_tmp0));*_tmp0=s;_tmp0;});_tmp1->hd=_tmp2B3;}),_tmp1->tl=Cyc_ccargs;_tmp1;});}
# 97
void Cyc_set_c_compiler(struct _dyneither_ptr s){
if(({struct _dyneither_ptr _tmp2B4=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp2B4,({const char*_tmp2="vc";_tag_dyneither(_tmp2,sizeof(char),3U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;
Cyc_add_ccarg(({const char*_tmp3="-DVC_C";_tag_dyneither(_tmp3,sizeof(char),7U);}));}else{
# 102
if(({struct _dyneither_ptr _tmp2B5=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp2B5,({const char*_tmp4="gcc";_tag_dyneither(_tmp4,sizeof(char),4U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";_tag_dyneither(_tmp5,sizeof(char),8U);}));}}}
# 109
void Cyc_set_port_c_code(){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 116
static struct _dyneither_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _dyneither_ptr s){
Cyc_output_file=({struct _dyneither_ptr*_tmp6=_cycalloc(sizeof(*_tmp6));*_tmp6=s;_tmp6;});}
# 121
static struct _dyneither_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _dyneither_ptr s){
Cyc_specified_interface=s;}
# 131
extern char* Cdef_lib_path;
extern char* Carch;
extern char* Cversion;static char _tmp7[1U]="";
# 136
static struct _dyneither_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1U};
static void Cyc_set_cpp(struct _dyneither_ptr s){
Cyc_cpp=s;}
# 141
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _dyneither_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));({struct _dyneither_ptr*_tmp2B6=({struct _dyneither_ptr*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=s;_tmp8;});_tmp9->hd=_tmp2B6;}),_tmp9->tl=Cyc_cppargs;_tmp9;});}
# 146
static void Cyc_add_cpp_and_ccarg(struct _dyneither_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 151
static void Cyc_add_iprefix(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp280;_tmp280.tag=0U,_tmp280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp280;});void*_tmpA[1U];_tmpA[0]=& _tmpC;({struct _dyneither_ptr _tmp2B7=({const char*_tmpB="-iprefix %s";_tag_dyneither(_tmpB,sizeof(char),12U);});Cyc_aprintf(_tmp2B7,_tag_dyneither(_tmpA,sizeof(void*),1U));});}));}
# 154
static void Cyc_add_iwithprefix(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp281;_tmp281.tag=0U,_tmp281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp281;});void*_tmpD[1U];_tmpD[0]=& _tmpF;({struct _dyneither_ptr _tmp2B8=({const char*_tmpE="-iwithprefix %s";_tag_dyneither(_tmpE,sizeof(char),16U);});Cyc_aprintf(_tmp2B8,_tag_dyneither(_tmpD,sizeof(void*),1U));});}));}
# 157
static void Cyc_add_iwithprefixbefore(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp282;_tmp282.tag=0U,_tmp282.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp282;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _dyneither_ptr _tmp2B9=({const char*_tmp11="-iwithprefixbefore %s";_tag_dyneither(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp2B9,_tag_dyneither(_tmp10,sizeof(void*),1U));});}));}
# 160
static void Cyc_add_isystem(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp283;_tmp283.tag=0U,_tmp283.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp283;});void*_tmp13[1U];_tmp13[0]=& _tmp15;({struct _dyneither_ptr _tmp2BA=({const char*_tmp14="-isystem %s";_tag_dyneither(_tmp14,sizeof(char),12U);});Cyc_aprintf(_tmp2BA,_tag_dyneither(_tmp13,sizeof(void*),1U));});}));}
# 163
static void Cyc_add_idirafter(struct _dyneither_ptr s){
Cyc_add_cpparg((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp284;_tmp284.tag=0U,_tmp284.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp284;});void*_tmp16[1U];_tmp16[0]=& _tmp18;({struct _dyneither_ptr _tmp2BB=({const char*_tmp17="-idirafter %s";_tag_dyneither(_tmp17,sizeof(char),14U);});Cyc_aprintf(_tmp2BB,_tag_dyneither(_tmp16,sizeof(void*),1U));});}));}
# 167
static void Cyc_set_many_errors(){
Cyc_Position_max_errors=65535;}static char _tmp19[20U]="_see_cycspecs_file_";
# 171
static struct _dyneither_ptr Cyc_def_inc_path={_tmp19,_tmp19,_tmp19 + 20U};
# 174
static void Cyc_print_version(){
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp285;_tmp285.tag=0U,({struct _dyneither_ptr _tmp2BC=(struct _dyneither_ptr)({char*_tmp1D= Cversion;_tag_dyneither(_tmp1D,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D,1U));});_tmp285.f1=_tmp2BC;});_tmp285;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct _dyneither_ptr _tmp2BD=({const char*_tmp1B="The Cyclone compiler, version %s\n";_tag_dyneither(_tmp1B,sizeof(char),34U);});Cyc_printf(_tmp2BD,_tag_dyneither(_tmp1A,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp286;_tmp286.tag=0U,({struct _dyneither_ptr _tmp2BE=(struct _dyneither_ptr)({char*_tmp21= Carch;_tag_dyneither(_tmp21,sizeof(char),_get_zero_arr_size_char((void*)_tmp21,1U));});_tmp286.f1=_tmp2BE;});_tmp286;});void*_tmp1E[1U];_tmp1E[0]=& _tmp20;({struct _dyneither_ptr _tmp2BF=({const char*_tmp1F="Compiled for architecture: %s\n";_tag_dyneither(_tmp1F,sizeof(char),31U);});Cyc_printf(_tmp2BF,_tag_dyneither(_tmp1E,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp287;_tmp287.tag=0U,({struct _dyneither_ptr _tmp2C0=(struct _dyneither_ptr)({char*_tmp25= Cdef_lib_path;_tag_dyneither(_tmp25,sizeof(char),_get_zero_arr_size_char((void*)_tmp25,1U));});_tmp287.f1=_tmp2C0;});_tmp287;});void*_tmp22[1U];_tmp22[0]=& _tmp24;({struct _dyneither_ptr _tmp2C1=({const char*_tmp23="Standard library directory: %s\n";_tag_dyneither(_tmp23,sizeof(char),32U);});Cyc_printf(_tmp2C1,_tag_dyneither(_tmp22,sizeof(void*),1U));});});
# 179
({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp288;_tmp288.tag=0U,_tmp288.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_def_inc_path);_tmp288;});void*_tmp26[1U];_tmp26[0]=& _tmp28;({struct _dyneither_ptr _tmp2C2=({const char*_tmp27="Standard include directory: %s\n";_tag_dyneither(_tmp27,sizeof(char),32U);});Cyc_printf(_tmp2C2,_tag_dyneither(_tmp26,sizeof(void*),1U));});});
 exit(0);}
# 183
static int Cyc_is_cyclone_sourcefile(struct _dyneither_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _dyneither_ptr)s);
if(_tmp29 <= 4)return 0;else{
return({struct _dyneither_ptr _tmp2C3=(struct _dyneither_ptr)_dyneither_ptr_plus(s,sizeof(char),(int)(_tmp29 - 4));Cyc_strcmp(_tmp2C3,({const char*_tmp2A=".cyc";_tag_dyneither(_tmp2A,sizeof(char),5U);}));})== 0;}}
# 199 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _dyneither_ptr s){
if(({struct _dyneither_ptr _tmp2C4=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp2C4,({const char*_tmp2B="-lxml";_tag_dyneither(_tmp2B,sizeof(char),6U);}));})== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 205
Cyc_add_ccarg(({const char*_tmp2C="-lxml_a";_tag_dyneither(_tmp2C,sizeof(char),8U);}));}}else{
if(({struct _dyneither_ptr _tmp2C5=(struct _dyneither_ptr)s;Cyc_strncmp(_tmp2C5,({const char*_tmp2D="-lcyc";_tag_dyneither(_tmp2D,sizeof(char),6U);}),5U);})== 0)
Cyc_add_ccarg(s);else{
# 209
Cyc_libargs=({struct Cyc_List_List*_tmp2F=_cycalloc(sizeof(*_tmp2F));({struct _dyneither_ptr*_tmp2C6=({struct _dyneither_ptr*_tmp2E=_cycalloc(sizeof(*_tmp2E));*_tmp2E=s;_tmp2E;});_tmp2F->hd=_tmp2C6;}),_tmp2F->tl=Cyc_libargs;_tmp2F;});}}}
# 212
static void Cyc_add_marg(struct _dyneither_ptr s){
Cyc_add_ccarg(s);}
# 216
static void Cyc_set_save_temps(){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_tmp30="-save-temps";_tag_dyneither(_tmp30,sizeof(char),12U);}));}
# 221
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies(){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_tmp31="-M";_tag_dyneither(_tmp31,sizeof(char),3U);}));}
# 228
static struct _dyneither_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _dyneither_ptr s){
Cyc_dependencies_target=({struct _dyneither_ptr*_tmp32=_cycalloc(sizeof(*_tmp32));*_tmp32=s;_tmp32;});}
# 233
static void Cyc_set_stop_after_objectfile(){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_tmp33="-c";_tag_dyneither(_tmp33,sizeof(char),3U);}));}
# 238
static void Cyc_set_nocppprecomp(){
Cyc_add_cpp_and_ccarg(({const char*_tmp34="-no-cpp-precomp";_tag_dyneither(_tmp34,sizeof(char),16U);}));}
# 242
static void Cyc_clear_lineno(){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 246
static void Cyc_set_inline_functions(){
Cyc_inline_functions_r=1;}
# 249
static void Cyc_set_elim_se(){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 253
static void Cyc_set_tovc(){
Cyc_Cyclone_tovc_r=1;
Cyc_add_ccarg(({const char*_tmp35="-DCYC_ANSI_OUTPUT";_tag_dyneither(_tmp35,sizeof(char),18U);}));
Cyc_set_elim_se();}
# 258
static void Cyc_set_notoseqc(){
Cyc_toseqc_r=0;}
# 261
static void Cyc_set_noboundschecks(){
Cyc_add_ccarg(({const char*_tmp36="-DNO_CYC_BOUNDS_CHECKS";_tag_dyneither(_tmp36,sizeof(char),23U);}));}
# 264
static void Cyc_set_nonullchecks(){
Cyc_add_ccarg(({const char*_tmp37="-DNO_CYC_NULL_CHECKS";_tag_dyneither(_tmp37,sizeof(char),21U);}));}
# 267
static void Cyc_set_nochecks(){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 273
static void Cyc_set_nocyc(){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_tmp38="-DNO_CYC_PREFIX";_tag_dyneither(_tmp38,sizeof(char),16U);}));}
# 278
static void Cyc_set_pa(){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_tmp39="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp39,sizeof(char),21U);}));
Cyc_add_cpparg(({const char*_tmp3A="-DCYC_REGION_PROFILE";_tag_dyneither(_tmp3A,sizeof(char),21U);}));}
# 284
static void Cyc_set_pg(){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_tmp3B="-pg";_tag_dyneither(_tmp3B,sizeof(char),4U);}));}
# 289
static void Cyc_set_stop_after_asmfile(){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_tmp3C="-S";_tag_dyneither(_tmp3C,sizeof(char),3U);}));}
# 294
static void Cyc_set_all_warnings(){
Cyc_InsertChecks_warn_bounds_checks=1;
Cyc_InsertChecks_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;
Cyc_Tc_aggressive_warn=1;
Cyc_Binding_warn_override=1;}
# 305
enum Cyc_inputtype{Cyc_DEFAULTINPUT  = 0U,Cyc_CYCLONEFILE  = 1U};
# 310
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _dyneither_ptr s){
if(({struct _dyneither_ptr _tmp2C7=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp2C7,({const char*_tmp3D="cyc";_tag_dyneither(_tmp3D,sizeof(char),4U);}));})== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
if(({struct _dyneither_ptr _tmp2C8=(struct _dyneither_ptr)s;Cyc_strcmp(_tmp2C8,({const char*_tmp3E="none";_tag_dyneither(_tmp3E,sizeof(char),5U);}));})== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 317
({struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp289;_tmp289.tag=0U,_tmp289.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp289;});void*_tmp3F[1U];_tmp3F[0]=& _tmp41;({struct Cyc___cycFILE*_tmp2CA=Cyc_stderr;struct _dyneither_ptr _tmp2C9=({const char*_tmp40="Input type '%s' not supported\n";_tag_dyneither(_tmp40,sizeof(char),31U);});Cyc_fprintf(_tmp2CA,_tmp2C9,_tag_dyneither(_tmp3F,sizeof(void*),1U));});});}}}
# 320
struct _dyneither_ptr Cyc_make_base_filename(struct _dyneither_ptr s,struct _dyneither_ptr*output_file){
struct _dyneither_ptr _tmp42=Cyc_Filename_dirname(output_file == 0?s:*output_file);
struct _dyneither_ptr _tmp43=Cyc_Filename_chop_extension((struct _dyneither_ptr)Cyc_Filename_basename(s));
# 326
struct _dyneither_ptr _tmp44=Cyc_strlen((struct _dyneither_ptr)_tmp42)> 0?Cyc_Filename_concat((struct _dyneither_ptr)_tmp42,(struct _dyneither_ptr)_tmp43): _tmp43;
return(struct _dyneither_ptr)_tmp44;}
# 332
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 334
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _dyneither_ptr s){
# 337
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
# 341
if(Cyc_intype == Cyc_CYCLONEFILE  || Cyc_is_cyclone_sourcefile(s)){
# 343
Cyc_cyclone_files=({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));({struct _dyneither_ptr*_tmp2CB=({struct _dyneither_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));*_tmp45=s;_tmp45;});_tmp46->hd=_tmp2CB;}),_tmp46->tl=Cyc_cyclone_files;_tmp46;});{
# 349
struct _dyneither_ptr _tmp47=Cyc_make_base_filename(s,0);
struct _dyneither_ptr _tmp48=({struct _dyneither_ptr _tmp2CC=(struct _dyneither_ptr)_tmp47;Cyc_strconcat(_tmp2CC,({const char*_tmp49=".c";_tag_dyneither(_tmp49,sizeof(char),3U);}));});
Cyc_add_ccarg((struct _dyneither_ptr)_tmp48);};}else{
# 353
Cyc_add_ccarg(s);}}}
# 358
static int Cyc_assume_gcc_flag=1;struct _tuple21{struct _dyneither_ptr flag;int optargs;};static char _tmp4A[3U]="-s";static char _tmp4B[3U]="-O";static char _tmp4C[4U]="-O0";static char _tmp4D[4U]="-O2";static char _tmp4E[4U]="-O3";static char _tmp4F[21U]="-fomit-frame-pointer";static char _tmp50[13U]="-fno-builtin";static char _tmp51[3U]="-g";static char _tmp52[3U]="-p";static char _tmp53[8U]="-static";
static int Cyc_add_other_flag(struct _dyneither_ptr s){
static struct _tuple21 known_gcc_flags[10U]={{.flag={_tmp4A,_tmp4A,_tmp4A + 3U},.optargs=0},{.flag={_tmp4B,_tmp4B,_tmp4B + 3U},.optargs=0},{.flag={_tmp4C,_tmp4C,_tmp4C + 4U},.optargs=0},{.flag={_tmp4D,_tmp4D,_tmp4D + 4U},.optargs=0},{.flag={_tmp4E,_tmp4E,_tmp4E + 4U},.optargs=0},{.flag={_tmp4F,_tmp4F,_tmp4F + 21U},.optargs=0},{.flag={_tmp50,_tmp50,_tmp50 + 13U},.optargs=0},{.flag={_tmp51,_tmp51,_tmp51 + 3U},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 3U},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 8U},.optargs=0}};
# 372
if(Cyc_assume_gcc_flag)
Cyc_add_ccarg(s);else{
# 375
{int i=0;for(0;i < 10U;++ i){
if(!Cyc_strcmp((struct _dyneither_ptr)(*((struct _tuple21*)_check_known_subscript_notnull(known_gcc_flags,10U,sizeof(struct _tuple21),i))).flag,(struct _dyneither_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(known_gcc_flags[i]).optargs;
break;}}}
# 382
return 0;}
# 384
return 1;}
# 387
static void Cyc_noassume_gcc_flag(){
Cyc_assume_gcc_flag=0;}
# 391
static void Cyc_remove_file(struct _dyneither_ptr s){
if(Cyc_save_temps_r)return;else{
 remove((const char*)_check_null(_untag_dyneither_ptr(s,sizeof(char),1U)));}}
# 398
int Cyc_compile_failure=0;
# 400
struct Cyc___cycFILE*Cyc_try_file_open(struct _dyneither_ptr filename,struct _dyneither_ptr mode,struct _dyneither_ptr msg_part){
struct _handler_cons _tmp54;_push_handler(& _tmp54);{int _tmp56=0;if(setjmp(_tmp54.handler))_tmp56=1;if(!_tmp56){{struct Cyc___cycFILE*_tmp57=Cyc_file_open(filename,mode);_npop_handler(0U);return _tmp57;};_pop_handler();}else{void*_tmp55=(void*)_exn_thrown;void*_tmp58=_tmp55;_LL1: _LL2:
# 404
 Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp5B=({struct Cyc_String_pa_PrintArg_struct _tmp28B;_tmp28B.tag=0U,_tmp28B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);_tmp28B;});struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp28A;_tmp28A.tag=0U,_tmp28A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename);_tmp28A;});void*_tmp59[2U];_tmp59[0]=& _tmp5B,_tmp59[1]=& _tmp5C;({struct Cyc___cycFILE*_tmp2CE=Cyc_stderr;struct _dyneither_ptr _tmp2CD=({const char*_tmp5A="\nError: couldn't open %s %s\n";_tag_dyneither(_tmp5A,sizeof(char),29U);});Cyc_fprintf(_tmp2CE,_tmp2CD,_tag_dyneither(_tmp59,sizeof(void*),2U));});});
Cyc_fflush(Cyc_stderr);
return 0;_LL0:;}};}
# 411
void CYCALLOCPROFILE_mark(const char*s);
# 413
void*Cyc_do_stage(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void(*on_fail)(void*),void*failenv){
# 416
 CYCALLOCPROFILE_mark((const char*)_untag_dyneither_ptr(stage_name,sizeof(char),1U));{
# 418
void*ans;
{struct _handler_cons _tmp5D;_push_handler(& _tmp5D);{int _tmp5F=0;if(setjmp(_tmp5D.handler))_tmp5F=1;if(!_tmp5F){ans=f(env,tds);;_pop_handler();}else{void*_tmp5E=(void*)_exn_thrown;void*_tmp60=_tmp5E;void*_tmp66;_LL1: _tmp66=_tmp60;_LL2:
# 423
 if(Cyc_Position_error_p()){
Cyc_compile_failure=1;
on_fail(failenv);
({void*_tmp61=0U;({struct Cyc___cycFILE*_tmp2D0=Cyc_stderr;struct _dyneither_ptr _tmp2CF=({const char*_tmp62="\nCOMPILATION FAILED!\n";_tag_dyneither(_tmp62,sizeof(char),22U);});Cyc_fprintf(_tmp2D0,_tmp2CF,_tag_dyneither(_tmp61,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);}else{
# 430
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp28C;_tmp28C.tag=0U,_tmp28C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);_tmp28C;});void*_tmp63[1U];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmp2D2=Cyc_stderr;struct _dyneither_ptr _tmp2D1=({const char*_tmp64="COMPILER STAGE %s\n";_tag_dyneither(_tmp64,sizeof(char),19U);});Cyc_fprintf(_tmp2D2,_tmp2D1,_tag_dyneither(_tmp63,sizeof(void*),1U));});});
on_fail(failenv);}
# 433
Cyc_Core_rethrow(_tmp66);
goto _LL0;_LL0:;}};}
# 436
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
on_fail(failenv);
({void*_tmp67=0U;({struct Cyc___cycFILE*_tmp2D4=Cyc_stderr;struct _dyneither_ptr _tmp2D3=({const char*_tmp68="\nCOMPILATION FAILED!\n";_tag_dyneither(_tmp68,sizeof(char),22U);});Cyc_fprintf(_tmp2D4,_tmp2D3,_tag_dyneither(_tmp67,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);
return ans;}else{
# 444
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp28D;_tmp28D.tag=0U,_tmp28D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stage_name);_tmp28D;});void*_tmp69[1U];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp2D6=Cyc_stderr;struct _dyneither_ptr _tmp2D5=({const char*_tmp6A="%s completed.\n";_tag_dyneither(_tmp6A,sizeof(char),15U);});Cyc_fprintf(_tmp2D6,_tmp2D5,_tag_dyneither(_tmp69,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return ans;}}
# 449
return ans;};}
# 452
static void Cyc_ignore(void*x){;}
static void Cyc_remove_fileptr(struct _dyneither_ptr*s){Cyc_remove_file((struct _dyneither_ptr)*s);}
# 455
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp6C;_push_handler(& _tmp6C);{int _tmp6E=0;if(setjmp(_tmp6C.handler))_tmp6E=1;if(!_tmp6E){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 460
;_pop_handler();}else{void*_tmp6D=(void*)_exn_thrown;void*_tmp6F=_tmp6D;void*_tmp70;if(((struct Cyc_Parse_Exit_exn_struct*)_tmp6F)->tag == Cyc_Parse_Exit){_LL1: _LL2:
# 463
 Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_LL3: _tmp70=_tmp6F;_LL4:
 Cyc_file_close(f);Cyc_Core_rethrow(_tmp70);}_LL0:;}};}
# 466
Cyc_Lex_lex_init(1);
return ans;};}
# 470
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 475
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 477
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);
return tds;}
# 483
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 485
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 488
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 490
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}
# 494
int Cyc_do_insert_checks(int ignore,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds);
return 1;}struct _tuple22{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 501
struct Cyc_List_List*Cyc_do_interface(struct _tuple22*params,struct Cyc_List_List*tds){
# 503
struct _tuple22*_tmp71=params;struct Cyc_Tcenv_Tenv*_tmp79;struct Cyc___cycFILE*_tmp78;struct Cyc___cycFILE*_tmp77;_LL1: _tmp79=_tmp71->f1;_tmp78=_tmp71->f2;_tmp77=_tmp71->f3;_LL2:;{
struct Cyc_Interface_I*_tmp72=Cyc_Interface_extract(_tmp79->ae,tds);
if(_tmp78 == 0)
Cyc_Interface_save(_tmp72,_tmp77);else{
# 508
struct Cyc_Interface_I*_tmp73=Cyc_Interface_parse(_tmp78);
if(!({struct Cyc_Interface_I*_tmp2DA=_tmp73;struct Cyc_Interface_I*_tmp2D9=_tmp72;Cyc_Interface_is_subinterface(_tmp2DA,_tmp2D9,({struct _tuple20*_tmp76=_cycalloc(sizeof(*_tmp76));({struct _dyneither_ptr _tmp2D8=({const char*_tmp74="written interface";_tag_dyneither(_tmp74,sizeof(char),18U);});_tmp76->f1=_tmp2D8;}),({struct _dyneither_ptr _tmp2D7=({const char*_tmp75="maximal interface";_tag_dyneither(_tmp75,sizeof(char),18U);});_tmp76->f2=_tmp2D7;});_tmp76;}));}))
Cyc_compile_failure=1;else{
# 512
Cyc_Interface_save(_tmp73,_tmp77);}}
# 514
return tds;};}
# 516
void Cyc_interface_fail(struct _tuple22*params){
struct _tuple22*_tmp7A=params;struct Cyc___cycFILE*_tmp7C;struct Cyc___cycFILE*_tmp7B;_LL1: _tmp7C=_tmp7A->f2;_tmp7B=_tmp7A->f3;_LL2:;
if(_tmp7C != 0)
Cyc_file_close(_tmp7C);
Cyc_file_close(_tmp7B);}
# 523
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 526
return Cyc_Toc_toc(pops,tds);}
# 528
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 532
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 535
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 540
static struct _dyneither_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 542
static struct _dyneither_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 544
static void Cyc_set_cyc_include(struct _dyneither_ptr f){
Cyc_cyc_include=f;}
# 549
int Cyc_copy_internal_file(struct _dyneither_ptr file,struct Cyc___cycFILE*out_file){
# 552
if(({char*_tmp2DB=(char*)file.curr;_tmp2DB == (char*)(_tag_dyneither(0,0,0)).curr;})){
({void*_tmp7D=0U;({struct Cyc___cycFILE*_tmp2DD=Cyc_stderr;struct _dyneither_ptr _tmp2DC=({const char*_tmp7E="Internal error: copy_internal_file called with NULL\n";_tag_dyneither(_tmp7E,sizeof(char),53U);});Cyc_fprintf(_tmp2DD,_tmp2DC,_tag_dyneither(_tmp7D,sizeof(void*),0U));});});
return 1;}{
# 556
struct Cyc___cycFILE*_tmp7F=({struct _dyneither_ptr _tmp2DF=file;struct _dyneither_ptr _tmp2DE=({const char*_tmp86="r";_tag_dyneither(_tmp86,sizeof(char),2U);});Cyc_try_file_open(_tmp2DF,_tmp2DE,({const char*_tmp87="internal compiler file";_tag_dyneither(_tmp87,sizeof(char),23U);}));});
if(_tmp7F == 0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned int _tmp290=1024U;unsigned int i;for(i=0;i < _tmp290;++ i){buf[i]='\000';}}0;});
while(n_read == 1024){
n_read=({struct _dyneither_ptr _tmp2E0=_tag_dyneither(buf,sizeof(char),1024U);Cyc_fread(_tmp2E0,1U,1024U,_tmp7F);});
if(n_read != 1024  && !Cyc_feof(_tmp7F)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp82=({struct Cyc_String_pa_PrintArg_struct _tmp28E;_tmp28E.tag=0U,_tmp28E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp28E;});void*_tmp80[1U];_tmp80[0]=& _tmp82;({struct Cyc___cycFILE*_tmp2E2=Cyc_stderr;struct _dyneither_ptr _tmp2E1=({const char*_tmp81="\nError: problem copying %s\n";_tag_dyneither(_tmp81,sizeof(char),28U);});Cyc_fprintf(_tmp2E2,_tmp2E1,_tag_dyneither(_tmp80,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 1;}
# 569
n_written=({struct _dyneither_ptr _tmp2E4=_tag_dyneither(buf,sizeof(char),1024U);unsigned long _tmp2E3=n_read;Cyc_fwrite(_tmp2E4,1U,_tmp2E3,out_file);});
if(n_read != n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp85=({struct Cyc_String_pa_PrintArg_struct _tmp28F;_tmp28F.tag=0U,_tmp28F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp28F;});void*_tmp83[1U];_tmp83[0]=& _tmp85;({struct Cyc___cycFILE*_tmp2E6=Cyc_stderr;struct _dyneither_ptr _tmp2E5=({const char*_tmp84="\nError: problem copying %s\n";_tag_dyneither(_tmp84,sizeof(char),28U);});Cyc_fprintf(_tmp2E6,_tmp2E5,_tag_dyneither(_tmp83,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 577
Cyc_fclose(_tmp7F);
return 0;};};}
# 583
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles(){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file((struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 589
static void Cyc_find_fail(struct _dyneither_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp2E8=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp291;_tmp291.tag=0U,_tmp291.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)file);_tmp291;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({struct _dyneither_ptr _tmp2E7=({const char*_tmp89="Error: can't find internal compiler file %s\n";_tag_dyneither(_tmp89,sizeof(char),45U);});Cyc_aprintf(_tmp2E7,_tag_dyneither(_tmp88,sizeof(void*),1U));});});_tmp8B->f1=_tmp2E8;});_tmp8B;}));}
# 594
static struct _dyneither_ptr Cyc_find_in_arch_path(struct _dyneither_ptr s){
struct _dyneither_ptr _tmp8C=Cyc_Specsfile_find_in_arch_path(s);
if(!((unsigned int)_tmp8C.curr))Cyc_find_fail(s);
return _tmp8C;}
# 599
static struct _dyneither_ptr Cyc_find_in_exec_path(struct _dyneither_ptr s){
struct _dyneither_ptr _tmp8D=Cyc_Specsfile_find_in_exec_path(s);
if(!((unsigned int)_tmp8D.curr))Cyc_find_fail(s);
return _tmp8D;}struct _tuple23{struct Cyc___cycFILE*f1;struct _dyneither_ptr f2;};
# 605
struct Cyc_List_List*Cyc_do_print(struct _tuple23*env,struct Cyc_List_List*tds){
# 607
struct _tuple23*_tmp8E=env;struct Cyc___cycFILE*_tmp97;struct _dyneither_ptr _tmp96;_LL1: _tmp97=_tmp8E->f1;_tmp96=_tmp8E->f2;_LL2:;{
struct Cyc_Absynpp_Params _tmp8F=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;
_tmp8F.expand_typedefs=!Cyc_noexpand_r;
_tmp8F.to_VC=Cyc_Cyclone_tovc_r;
_tmp8F.add_cyc_prefix=Cyc_add_cyc_namespace_r;
_tmp8F.generate_line_directives=Cyc_generate_line_directives_r;
_tmp8F.print_full_evars=Cyc_print_full_evars_r;
_tmp8F.print_all_tvars=Cyc_print_all_tvars_r;
_tmp8F.print_all_kinds=Cyc_print_all_kinds_r;
_tmp8F.print_all_effects=Cyc_print_all_effects_r;
# 618
if(Cyc_inline_functions_r)
({void*_tmp90=0U;({struct Cyc___cycFILE*_tmp2EA=_tmp97;struct _dyneither_ptr _tmp2E9=({const char*_tmp91="#define _INLINE_FUNCTIONS\n";_tag_dyneither(_tmp91,sizeof(char),27U);});Cyc_fprintf(_tmp2EA,_tmp2E9,_tag_dyneither(_tmp90,sizeof(void*),0U));});});
# 622
if((!Cyc_parseonly_r  && !Cyc_tc_r) && !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r)
({void*_tmp92=0U;({struct Cyc___cycFILE*_tmp2EC=_tmp97;struct _dyneither_ptr _tmp2EB=({const char*_tmp93="#include <setjmp.h>\n";_tag_dyneither(_tmp93,sizeof(char),21U);});Cyc_fprintf(_tmp2EC,_tmp2EB,_tag_dyneither(_tmp92,sizeof(void*),0U));});});else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,_tmp97))return tds;}}
# 628
if(Cyc_copy_internal_file(Cyc_cyc_include,_tmp97))return tds;}
# 630
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& _tmp8F);
Cyc_Absynpp_decllist2file(tds,_tmp97);}else{
# 634
Cyc_Absyndump_set_params(& _tmp8F);
Cyc_Absyndump_dumpdecllist2file(tds,_tmp97);}
# 637
Cyc_fflush(_tmp97);
Cyc_file_close(_tmp97);
Cyc_cfiles=({struct Cyc_List_List*_tmp95=_cycalloc(sizeof(*_tmp95));({struct _dyneither_ptr*_tmp2ED=({struct _dyneither_ptr*_tmp94=_cycalloc(sizeof(*_tmp94));*_tmp94=_tmp96;_tmp94;});_tmp95->hd=_tmp2ED;}),_tmp95->tl=Cyc_cfiles;_tmp95;});
return tds;};}
# 642
void Cyc_print_fail(struct _tuple23*env){
struct _tuple23*_tmp98=env;struct Cyc___cycFILE*_tmp9C;struct _dyneither_ptr _tmp9B;_LL1: _tmp9C=_tmp98->f1;_tmp9B=_tmp98->f2;_LL2:;
Cyc_file_close(_tmp9C);
Cyc_cfiles=({struct Cyc_List_List*_tmp9A=_cycalloc(sizeof(*_tmp9A));({struct _dyneither_ptr*_tmp2EF=({struct _dyneither_ptr*_tmp99=_cycalloc(sizeof(*_tmp99));({struct _dyneither_ptr _tmp2EE=Cyc_strdup((struct _dyneither_ptr)_tmp9B);*_tmp99=_tmp2EE;});_tmp99;});_tmp9A->hd=_tmp2EF;}),_tmp9A->tl=Cyc_cfiles;_tmp9A;});}
# 649
static struct Cyc_List_List*Cyc_split_by_char(struct _dyneither_ptr s,char c){
if(({char*_tmp2F0=(char*)s.curr;_tmp2F0 == (char*)(_tag_dyneither(0,0,0)).curr;}))return 0;{
struct Cyc_List_List*_tmp9D=0;
unsigned long _tmp9E=Cyc_strlen((struct _dyneither_ptr)s);
# 654
while(_tmp9E > 0){
struct _dyneither_ptr _tmp9F=Cyc_strchr(s,c);
if(({char*_tmp2F1=(char*)_tmp9F.curr;_tmp2F1 == (char*)(_tag_dyneither(0,0,0)).curr;})){
_tmp9D=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*_tmpA1));({struct _dyneither_ptr*_tmp2F2=({struct _dyneither_ptr*_tmpA0=_cycalloc(sizeof(*_tmpA0));*_tmpA0=s;_tmpA0;});_tmpA1->hd=_tmp2F2;}),_tmpA1->tl=_tmp9D;_tmpA1;});
break;}else{
# 661
_tmp9D=({struct Cyc_List_List*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct _dyneither_ptr*_tmp2F4=({struct _dyneither_ptr*_tmpA2=_cycalloc(sizeof(*_tmpA2));({struct _dyneither_ptr _tmp2F3=(struct _dyneither_ptr)Cyc_substring((struct _dyneither_ptr)s,0,(unsigned long)((((struct _dyneither_ptr)_tmp9F).curr - s.curr)/ sizeof(char)));*_tmpA2=_tmp2F3;});_tmpA2;});_tmpA3->hd=_tmp2F4;}),_tmpA3->tl=_tmp9D;_tmpA3;});
_tmp9E -=(((struct _dyneither_ptr)_tmp9F).curr - s.curr)/ sizeof(char);
s=_dyneither_ptr_plus(_tmp9F,sizeof(char),1);}}
# 666
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp9D);};}
# 671
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _dyneither_ptr subdir){
# 673
struct Cyc_List_List*_tmpA4=0;
for(0;dirs != 0;dirs=dirs->tl){
_tmpA4=({struct Cyc_List_List*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _dyneither_ptr*_tmp2F6=({struct _dyneither_ptr*_tmpA5=_cycalloc(sizeof(*_tmpA5));({struct _dyneither_ptr _tmp2F5=(struct _dyneither_ptr)Cyc_Filename_concat(*((struct _dyneither_ptr*)dirs->hd),subdir);*_tmpA5=_tmp2F5;});_tmpA5;});_tmpA6->hd=_tmp2F6;}),_tmpA6->tl=_tmpA4;_tmpA6;});}
# 677
_tmpA4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA4);
return _tmpA4;}
# 682
static int Cyc_is_other_special(char c){
char _tmpA7=c;switch(_tmpA7){case 92U: _LL1: _LL2:
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
# 697
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 704
static struct _dyneither_ptr Cyc_sh_escape_string(struct _dyneither_ptr s){
unsigned long _tmpA8=Cyc_strlen((struct _dyneither_ptr)s);
# 708
int _tmpA9=0;
int _tmpAA=0;
{int i=0;for(0;i < _tmpA8;++ i){
char _tmpAB=*((const char*)_check_dyneither_subscript(s,sizeof(char),i));
if(_tmpAB == '\'')++ _tmpA9;else{
if(Cyc_is_other_special(_tmpAB))++ _tmpAA;}}}
# 717
if(_tmpA9 == 0  && _tmpAA == 0)
return s;
# 721
if(_tmpA9 == 0)
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr*_tmpAC[3U];({struct _dyneither_ptr*_tmp2FB=({struct _dyneither_ptr*_tmpAE=_cycalloc(sizeof(*_tmpAE));({struct _dyneither_ptr _tmp2FA=({const char*_tmpAD="'";_tag_dyneither(_tmpAD,sizeof(char),2U);});*_tmpAE=_tmp2FA;});_tmpAE;});_tmpAC[0]=_tmp2FB;}),({struct _dyneither_ptr*_tmp2F9=({struct _dyneither_ptr*_tmpAF=_cycalloc(sizeof(*_tmpAF));*_tmpAF=(struct _dyneither_ptr)s;_tmpAF;});_tmpAC[1]=_tmp2F9;}),({struct _dyneither_ptr*_tmp2F8=({struct _dyneither_ptr*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct _dyneither_ptr _tmp2F7=({const char*_tmpB0="'";_tag_dyneither(_tmpB0,sizeof(char),2U);});*_tmpB1=_tmp2F7;});_tmpB1;});_tmpAC[2]=_tmp2F8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpAC,sizeof(struct _dyneither_ptr*),3U));}));{
# 725
unsigned long _tmpB2=(_tmpA8 + _tmpA9)+ _tmpAA;
struct _dyneither_ptr s2=({unsigned int _tmpBD=(_tmpB2 + 1)+ 1U;char*_tmpBC=_cycalloc_atomic(_check_times(_tmpBD,sizeof(char)));({{unsigned int _tmp292=_tmpB2 + 1;unsigned int i;for(i=0;i < _tmp292;++ i){_tmpBC[i]='\000';}_tmpBC[_tmp292]=0;}0;});_tag_dyneither(_tmpBC,sizeof(char),_tmpBD);});
int _tmpB3=0;
int _tmpB4=0;
for(0;_tmpB3 < _tmpA8;++ _tmpB3){
char _tmpB5=*((const char*)_check_dyneither_subscript(s,sizeof(char),_tmpB3));
if(_tmpB5 == '\''  || Cyc_is_other_special(_tmpB5))
({struct _dyneither_ptr _tmpB6=_dyneither_ptr_plus(s2,sizeof(char),_tmpB4 ++);char _tmpB7=*((char*)_check_dyneither_subscript(_tmpB6,sizeof(char),0U));char _tmpB8='\\';if(_get_dyneither_size(_tmpB6,sizeof(char))== 1U  && (_tmpB7 == 0  && _tmpB8 != 0))_throw_arraybounds();*((char*)_tmpB6.curr)=_tmpB8;});
({struct _dyneither_ptr _tmpB9=_dyneither_ptr_plus(s2,sizeof(char),_tmpB4 ++);char _tmpBA=*((char*)_check_dyneither_subscript(_tmpB9,sizeof(char),0U));char _tmpBB=_tmpB5;if(_get_dyneither_size(_tmpB9,sizeof(char))== 1U  && (_tmpBA == 0  && _tmpBB != 0))_throw_arraybounds();*((char*)_tmpB9.curr)=_tmpBB;});}
# 735
return(struct _dyneither_ptr)s2;};}
# 737
static struct _dyneither_ptr*Cyc_sh_escape_stringptr(struct _dyneither_ptr*sp){
return({struct _dyneither_ptr*_tmpBE=_cycalloc(sizeof(*_tmpBE));({struct _dyneither_ptr _tmp2FC=Cyc_sh_escape_string(*sp);*_tmpBE=_tmp2FC;});_tmpBE;});}
# 741
static void Cyc_process_file(struct _dyneither_ptr filename){
# 743
struct _dyneither_ptr _tmpBF=Cyc_make_base_filename(filename,Cyc_output_file);
struct _dyneither_ptr _tmpC0=({struct _dyneither_ptr _tmp2FD=(struct _dyneither_ptr)_tmpBF;Cyc_strconcat(_tmp2FD,({const char*_tmp114=".cyp";_tag_dyneither(_tmp114,sizeof(char),5U);}));});
struct _dyneither_ptr _tmpC1=({char*_tmp2FF=(char*)Cyc_specified_interface.curr;_tmp2FF != (char*)(_tag_dyneither(0,0,0)).curr;})?Cyc_specified_interface:(struct _dyneither_ptr)({
struct _dyneither_ptr _tmp2FE=(struct _dyneither_ptr)_tmpBF;Cyc_strconcat(_tmp2FE,({const char*_tmp113=".cyci";_tag_dyneither(_tmp113,sizeof(char),6U);}));});
struct _dyneither_ptr _tmpC2=({struct _dyneither_ptr _tmp300=(struct _dyneither_ptr)_tmpBF;Cyc_strconcat(_tmp300,({const char*_tmp112=".cycio";_tag_dyneither(_tmp112,sizeof(char),7U);}));});
# 751
struct _dyneither_ptr _tmpC3=({struct _dyneither_ptr _tmp301=(struct _dyneither_ptr)Cyc_make_base_filename(filename,0);Cyc_strconcat(_tmp301,({const char*_tmp111=".c";_tag_dyneither(_tmp111,sizeof(char),3U);}));});
struct _dyneither_ptr*_tmpC4=({struct _dyneither_ptr*_tmp110=_cycalloc(sizeof(*_tmp110));*_tmp110=_tmpC0;_tmp110;});
# 754
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp293;_tmp293.tag=0U,_tmp293.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)filename);_tmp293;});void*_tmpC5[1U];_tmpC5[0]=& _tmpC7;({struct Cyc___cycFILE*_tmp303=Cyc_stderr;struct _dyneither_ptr _tmp302=({const char*_tmpC6="Compiling %s\n";_tag_dyneither(_tmpC6,sizeof(char),14U);});Cyc_fprintf(_tmp303,_tmp302,_tag_dyneither(_tmpC5,sizeof(void*),1U));});});{
# 757
struct Cyc___cycFILE*f0=({struct _dyneither_ptr _tmp305=filename;struct _dyneither_ptr _tmp304=({const char*_tmp10E="r";_tag_dyneither(_tmp10E,sizeof(char),2U);});Cyc_try_file_open(_tmp305,_tmp304,({const char*_tmp10F="input file";_tag_dyneither(_tmp10F,sizeof(char),11U);}));});
if(Cyc_compile_failure  || !((unsigned int)f0))
return;
Cyc_fclose(f0);{
# 764
struct _dyneither_ptr _tmpC8=({
struct Cyc_List_List*_tmp309=({struct Cyc_List_List*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct _dyneither_ptr*_tmp308=({struct _dyneither_ptr*_tmp10B=_cycalloc(sizeof(*_tmp10B));({struct _dyneither_ptr _tmp307=(struct _dyneither_ptr)({const char*_tmp10A="";_tag_dyneither(_tmp10A,sizeof(char),1U);});*_tmp10B=_tmp307;});_tmp10B;});_tmp10C->hd=_tmp308;}),({
struct Cyc_List_List*_tmp306=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp10C->tl=_tmp306;});_tmp10C;});
# 765
Cyc_str_sepstr(_tmp309,({const char*_tmp10D=" ";_tag_dyneither(_tmp10D,sizeof(char),2U);}));});
# 774
struct Cyc_List_List*_tmpC9=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);
_tmpC9=({struct Cyc_List_List*_tmp30A=_tmpC9;Cyc_add_subdir(_tmp30A,({const char*_tmpCA="include";_tag_dyneither(_tmpCA,sizeof(char),8U);}));});
if(Cyc_strlen((struct _dyneither_ptr)Cyc_def_inc_path)> 0)
_tmpC9=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));({struct _dyneither_ptr*_tmp30B=({struct _dyneither_ptr*_tmpCB=_cycalloc(sizeof(*_tmpCB));*_tmpCB=Cyc_def_inc_path;_tmpCB;});_tmpCC->hd=_tmp30B;}),_tmpCC->tl=_tmpC9;_tmpCC;});{
# 779
char*_tmpCD= getenv("CYCLONE_INCLUDE_PATH");
if(_tmpCD != 0)
_tmpC9=({struct Cyc_List_List*_tmp30C=Cyc_split_by_char(({char*_tmpCE=_tmpCD;_tag_dyneither(_tmpCE,sizeof(char),_get_zero_arr_size_char((void*)_tmpCE,1U));}),':');((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp30C,_tmpC9);});{
# 784
struct _dyneither_ptr stdinc_string=(struct _dyneither_ptr)({
struct Cyc_List_List*_tmp310=({struct Cyc_List_List*_tmp108=_cycalloc(sizeof(*_tmp108));({struct _dyneither_ptr*_tmp30F=({struct _dyneither_ptr*_tmp107=_cycalloc(sizeof(*_tmp107));({struct _dyneither_ptr _tmp30E=(struct _dyneither_ptr)({const char*_tmp106="";_tag_dyneither(_tmp106,sizeof(char),1U);});*_tmp107=_tmp30E;});_tmp107;});_tmp108->hd=_tmp30F;}),({
struct Cyc_List_List*_tmp30D=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,_tmpC9);_tmp108->tl=_tmp30D;});_tmp108;});
# 785
Cyc_str_sepstr(_tmp310,({const char*_tmp109=" -I";_tag_dyneither(_tmp109,sizeof(char),4U);}));});
# 788
struct _dyneither_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0)
ofile_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp294;_tmp294.tag=0U,_tmp294.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_output_file)));_tmp294;});void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({struct _dyneither_ptr _tmp311=({const char*_tmpD0=" > %s";_tag_dyneither(_tmpD0,sizeof(char),6U);});Cyc_aprintf(_tmp311,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});else{
# 793
ofile_string=({const char*_tmpD2="";_tag_dyneither(_tmpD2,sizeof(char),1U);});}}else{
# 795
ofile_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD5=({struct Cyc_String_pa_PrintArg_struct _tmp295;_tmp295.tag=0U,({struct _dyneither_ptr _tmp312=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string((struct _dyneither_ptr)_tmpC0));_tmp295.f1=_tmp312;});_tmp295;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({struct _dyneither_ptr _tmp313=({const char*_tmpD4=" > %s";_tag_dyneither(_tmpD4,sizeof(char),6U);});Cyc_aprintf(_tmp313,_tag_dyneither(_tmpD3,sizeof(void*),1U));});});}{
# 797
struct _dyneither_ptr fixup_string;
if(Cyc_produce_dependencies){
# 801
if(Cyc_dependencies_target == 0)
# 805
fixup_string=({const char*_tmpD6=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_dyneither(_tmpD6,sizeof(char),35U);});else{
# 810
fixup_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD9=({struct Cyc_String_pa_PrintArg_struct _tmp296;_tmp296.tag=0U,_tmp296.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(Cyc_dependencies_target)));_tmp296;});void*_tmpD7[1U];_tmpD7[0]=& _tmpD9;({struct _dyneither_ptr _tmp314=({const char*_tmpD8=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_dyneither(_tmpD8,sizeof(char),29U);});Cyc_aprintf(_tmp314,_tag_dyneither(_tmpD7,sizeof(void*),1U));});});}}else{
# 814
fixup_string=({const char*_tmpDA="";_tag_dyneither(_tmpDA,sizeof(char),1U);});}{
# 816
struct _dyneither_ptr _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp100=({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0U,_tmp29E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_cpp);_tmp29E;});struct Cyc_String_pa_PrintArg_struct _tmp101=({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0U,_tmp29D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC8);_tmp29D;});struct Cyc_String_pa_PrintArg_struct _tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp29C;_tmp29C.tag=0U,_tmp29C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdinc_string);_tmp29C;});struct Cyc_String_pa_PrintArg_struct _tmp103=({struct Cyc_String_pa_PrintArg_struct _tmp29B;_tmp29B.tag=0U,({
# 819
struct _dyneither_ptr _tmp315=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(filename));_tmp29B.f1=_tmp315;});_tmp29B;});struct Cyc_String_pa_PrintArg_struct _tmp104=({struct Cyc_String_pa_PrintArg_struct _tmp29A;_tmp29A.tag=0U,_tmp29A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)fixup_string);_tmp29A;});struct Cyc_String_pa_PrintArg_struct _tmp105=({struct Cyc_String_pa_PrintArg_struct _tmp299;_tmp299.tag=0U,_tmp299.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)ofile_string);_tmp299;});void*_tmpFE[6U];_tmpFE[0]=& _tmp100,_tmpFE[1]=& _tmp101,_tmpFE[2]=& _tmp102,_tmpFE[3]=& _tmp103,_tmpFE[4]=& _tmp104,_tmpFE[5]=& _tmp105;({struct _dyneither_ptr _tmp316=({const char*_tmpFF="%s %s%s %s%s%s";_tag_dyneither(_tmpFF,sizeof(char),15U);});Cyc_aprintf(_tmp316,_tag_dyneither(_tmpFE,sizeof(void*),6U));});});
# 822
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0U,_tmp297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpDB);_tmp297;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({struct Cyc___cycFILE*_tmp318=Cyc_stderr;struct _dyneither_ptr _tmp317=({const char*_tmpDD="%s\n";_tag_dyneither(_tmpDD,sizeof(char),4U);});Cyc_fprintf(_tmp318,_tmp317,_tag_dyneither(_tmpDC,sizeof(void*),1U));});});
if( system((const char*)_check_null(_untag_dyneither_ptr(_tmpDB,sizeof(char),1U)))!= 0){
Cyc_compile_failure=1;
({void*_tmpDF=0U;({struct Cyc___cycFILE*_tmp31A=Cyc_stderr;struct _dyneither_ptr _tmp319=({const char*_tmpE0="\nError: preprocessing\n";_tag_dyneither(_tmpE0,sizeof(char),23U);});Cyc_fprintf(_tmp31A,_tmp319,_tag_dyneither(_tmpDF,sizeof(void*),0U));});});
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _dyneither_ptr)_tmpC0);
return;}
# 829
if(Cyc_stop_after_cpp_r)return;
# 832
Cyc_Position_reset_position((struct _dyneither_ptr)_tmpC0);{
struct Cyc___cycFILE*in_file=({struct _dyneither_ptr _tmp31C=(struct _dyneither_ptr)_tmpC0;struct _dyneither_ptr _tmp31B=({const char*_tmpFC="r";_tag_dyneither(_tmpFC,sizeof(char),2U);});Cyc_try_file_open(_tmp31C,_tmp31B,({const char*_tmpFD="file";_tag_dyneither(_tmpFD,sizeof(char),5U);}));});
if(Cyc_compile_failure)return;{
# 836
struct Cyc_List_List*tds=0;
# 839
tds=({struct _dyneither_ptr _tmp31F=({const char*_tmpE1="parsing";_tag_dyneither(_tmpE1,sizeof(char),8U);});struct Cyc_List_List*_tmp31E=tds;struct Cyc___cycFILE*_tmp31D=(struct Cyc___cycFILE*)_check_null(in_file);((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp31F,_tmp31E,Cyc_do_parse,_tmp31D,Cyc_remove_fileptr,_tmpC4);});
# 841
if(Cyc_compile_failure)return;
# 843
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}
# 849
({struct _dyneither_ptr _tmp321=({const char*_tmpE2="binding";_tag_dyneither(_tmpE2,sizeof(char),8U);});struct Cyc_List_List*_tmp320=tds;((int(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp321,_tmp320,Cyc_do_binding,1,Cyc_remove_fileptr,_tmpC4);});
if(Cyc_compile_failure)return;{
# 852
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables;
{struct Cyc_Tcenv_Tenv*_tmpE3=Cyc_Tcenv_tc_init();
if(Cyc_parseonly_r)goto PRINTC;
# 857
tds=({struct _dyneither_ptr _tmp324=({const char*_tmpE4="type checking";_tag_dyneither(_tmpE4,sizeof(char),14U);});struct Cyc_List_List*_tmp323=tds;struct Cyc_Tcenv_Tenv*_tmp322=_tmpE3;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp324,_tmp323,Cyc_do_typecheck,_tmp322,Cyc_remove_fileptr,_tmpC4);});
# 859
if(Cyc_compile_failure)return;
# 862
jump_tables=({struct _dyneither_ptr _tmp326=({const char*_tmpE5="jump checking";_tag_dyneither(_tmpE5,sizeof(char),14U);});struct Cyc_List_List*_tmp325=tds;((struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp326,_tmp325,Cyc_do_jumpanalysis,1,Cyc_remove_fileptr,_tmpC4);});
# 864
if(Cyc_compile_failure)return;
# 867
if(Cyc_tc_r)goto PRINTC;
({struct _dyneither_ptr _tmp329=({const char*_tmpE6="control-flow checking";_tag_dyneither(_tmpE6,sizeof(char),22U);});struct Cyc_List_List*_tmp328=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp327=jump_tables;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp329,_tmp328,Cyc_do_cfcheck,_tmp327,Cyc_remove_fileptr,_tmpC4);});
# 870
if(Cyc_compile_failure)return;
({struct _dyneither_ptr _tmp32B=({const char*_tmpE7="check insertion";_tag_dyneither(_tmpE7,sizeof(char),16U);});struct Cyc_List_List*_tmp32A=tds;((int(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp32B,_tmp32A,Cyc_do_insert_checks,1,Cyc_remove_fileptr,_tmpC4);});
# 873
if(Cyc_compile_failure)return;
# 876
if(Cyc_generate_interface_r){
struct Cyc___cycFILE*inter_file=({struct _dyneither_ptr _tmp32D=({const char*_tmpE8=(const char*)_check_null(_untag_dyneither_ptr(_tmpC1,sizeof(char),1U));_tag_dyneither(_tmpE8,sizeof(char),_get_zero_arr_size_char((void*)_tmpE8,1U));});struct _dyneither_ptr _tmp32C=({const char*_tmpE9="w";_tag_dyneither(_tmpE9,sizeof(char),2U);});Cyc_try_file_open(_tmp32D,_tmp32C,({const char*_tmpEA="interface file";_tag_dyneither(_tmpEA,sizeof(char),15U);}));});
if(inter_file == 0){
Cyc_compile_failure=1;
return;}
# 882
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 889
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_check_null(_untag_dyneither_ptr(_tmpC1,sizeof(char),1U)),"r");
struct Cyc___cycFILE*inter_objfile=({struct _dyneither_ptr _tmp32F=(struct _dyneither_ptr)_tmpC2;struct _dyneither_ptr _tmp32E=({const char*_tmpED="w";_tag_dyneither(_tmpED,sizeof(char),2U);});Cyc_try_file_open(_tmp32F,_tmp32E,({const char*_tmpEE="interface object file";_tag_dyneither(_tmpEE,sizeof(char),22U);}));});
if(inter_objfile == 0){
Cyc_compile_failure=1;
return;}
# 896
Cyc_Position_reset_position(_tmpC1);{
struct _tuple22 _tmpEB=({struct _tuple22 _tmp298;_tmp298.f1=_tmpE3,_tmp298.f2=inter_file,_tmp298.f3=inter_objfile;_tmp298;});
tds=({struct _dyneither_ptr _tmp330=({const char*_tmpEC="interface checking";_tag_dyneither(_tmpEC,sizeof(char),19U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_List_List*),struct _tuple22*env,void(*on_fail)(struct _tuple22*),struct _tuple22*failenv))Cyc_do_stage)(_tmp330,tds,Cyc_do_interface,& _tmpEB,Cyc_interface_fail,& _tmpEB);});};}}
# 902
if(Cyc_cf_r)goto PRINTC;
# 905
tds=({struct _dyneither_ptr _tmp333=({const char*_tmpEF="translation to C";_tag_dyneither(_tmpEF,sizeof(char),17U);});struct Cyc_List_List*_tmp332=tds;struct Cyc_Hashtable_Table*_tmp331=jump_tables->pop_tables;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*env,void(*on_fail)(struct _dyneither_ptr*),struct _dyneither_ptr*failenv))Cyc_do_stage)(_tmp333,_tmp332,Cyc_do_translate,_tmp331,Cyc_remove_fileptr,_tmpC4);});
# 908
tds=({struct _dyneither_ptr _tmp334=({const char*_tmpF0="aggregate removal";_tag_dyneither(_tmpF0,sizeof(char),18U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp334,tds,Cyc_do_removeaggrs,1,(void(*)(int x))Cyc_ignore,1);});
if(Cyc_compile_failure)return;
# 912
if(Cyc_toseqc_r)
tds=({struct _dyneither_ptr _tmp335=({const char*_tmpF1="post-pass to L-to-R evaluation order";_tag_dyneither(_tmpF1,sizeof(char),37U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp335,tds,Cyc_do_toseqc,1,(void(*)(int x))Cyc_ignore,1);});
# 916
Cyc_Toc_finish();
# 918
if(Cyc_compile_failure)return;
# 921
if(Cyc_Cyclone_tovc_r  || Cyc_elim_se_r)
tds=({struct _dyneither_ptr _tmp336=({const char*_tmpF2="post-pass to VC";_tag_dyneither(_tmpF2,sizeof(char),16U);});((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp336,tds,Cyc_do_tovc,1,(void(*)(int x))Cyc_ignore,1);});
if(Cyc_compile_failure)return;
# 925
Cyc_Warn_flush_warnings();
Cyc_remove_file((struct _dyneither_ptr)_tmpC0);
# 929
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r  || Cyc_tc_r) || Cyc_cf_r){
if(Cyc_output_file != 0)
out_file=({struct _dyneither_ptr _tmp338=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));struct _dyneither_ptr _tmp337=({const char*_tmpF3="w";_tag_dyneither(_tmpF3,sizeof(char),2U);});Cyc_try_file_open(_tmp338,_tmp337,({const char*_tmpF4="output file";_tag_dyneither(_tmpF4,sizeof(char),12U);}));});else{
# 935
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r  && Cyc_output_file != 0)
out_file=({struct _dyneither_ptr _tmp33A=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));struct _dyneither_ptr _tmp339=({const char*_tmpF5="w";_tag_dyneither(_tmpF5,sizeof(char),2U);});Cyc_try_file_open(_tmp33A,_tmp339,({const char*_tmpF6="output file";_tag_dyneither(_tmpF6,sizeof(char),12U);}));});else{
# 939
out_file=({struct _dyneither_ptr _tmp33C=(struct _dyneither_ptr)_tmpC3;struct _dyneither_ptr _tmp33B=({const char*_tmpF7="w";_tag_dyneither(_tmpF7,sizeof(char),2U);});Cyc_try_file_open(_tmp33C,_tmp33B,({const char*_tmpF8="output file";_tag_dyneither(_tmpF8,sizeof(char),12U);}));});}}
# 941
if(Cyc_compile_failure  || !((unsigned int)out_file))return;
# 943
if(!Cyc_noprint_r){
struct _tuple23*_tmpF9=({struct _tuple23*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->f1=out_file,_tmpFB->f2=_tmpC3;_tmpFB;});
tds=({struct _dyneither_ptr _tmp33F=({const char*_tmpFA="printing";_tag_dyneither(_tmpFA,sizeof(char),9U);});struct Cyc_List_List*_tmp33E=tds;struct _tuple23*_tmp33D=_tmpF9;((struct Cyc_List_List*(*)(struct _dyneither_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_List_List*),struct _tuple23*env,void(*on_fail)(struct _tuple23*),struct _tuple23*failenv))Cyc_do_stage)(_tmp33F,_tmp33E,Cyc_do_print,_tmp33D,Cyc_print_fail,_tmpF9);});}}};};};};};};};};};}static char _tmp115[8U]="<final>";
# 950
static struct _dyneither_ptr Cyc_final_str={_tmp115,_tmp115,_tmp115 + 8U};
static struct _dyneither_ptr*Cyc_final_strptr=& Cyc_final_str;
# 953
static struct Cyc_Interface_I*Cyc_read_cycio(struct _dyneither_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 957
struct _dyneither_ptr basename;
{struct _handler_cons _tmp116;_push_handler(& _tmp116);{int _tmp118=0;if(setjmp(_tmp116.handler))_tmp118=1;if(!_tmp118){basename=(struct _dyneither_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp117=(void*)_exn_thrown;void*_tmp119=_tmp117;void*_tmp11A;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp119)->tag == Cyc_Core_Invalid_argument){_LL1: _LL2:
 basename=*n;goto _LL0;}else{_LL3: _tmp11A=_tmp119;_LL4:(int)_rethrow(_tmp11A);}_LL0:;}};}{
# 961
struct _dyneither_ptr _tmp11B=({struct _dyneither_ptr _tmp340=(struct _dyneither_ptr)basename;Cyc_strconcat(_tmp340,({const char*_tmp120=".cycio";_tag_dyneither(_tmp120,sizeof(char),7U);}));});
struct Cyc___cycFILE*_tmp11C=({struct _dyneither_ptr _tmp342=(struct _dyneither_ptr)_tmp11B;struct _dyneither_ptr _tmp341=({const char*_tmp11E="rb";_tag_dyneither(_tmp11E,sizeof(char),3U);});Cyc_try_file_open(_tmp342,_tmp341,({const char*_tmp11F="interface object file";_tag_dyneither(_tmp11F,sizeof(char),22U);}));});
if(_tmp11C == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
 exit(1);}
# 968
Cyc_Position_reset_position((struct _dyneither_ptr)_tmp11B);{
# 970
struct Cyc_Interface_I*_tmp11D=Cyc_Interface_load(_tmp11C);
Cyc_file_close(_tmp11C);
return _tmp11D;};};};}
# 975
static int Cyc_is_cfile(struct _dyneither_ptr*n){
return*((const char*)_check_dyneither_subscript(*n,sizeof(char),0))!= '-';}
# 979
void GC_blacklist_warn_clear();struct _tuple24{struct _dyneither_ptr f1;int f2;struct _dyneither_ptr f3;void*f4;struct _dyneither_ptr f5;};
# 989
void Cyc_print_options();
# 996
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3U,& Cyc_v_r};static char _tmp121[3U]="-v";static char _tmp122[1U]="";static char _tmp123[35U]="print compilation stages verbosely";struct _tuple24 Cyc_opt1_tuple={{_tmp121,_tmp121,_tmp121 + 3U},0,{_tmp122,_tmp122,_tmp122 + 1U},(void*)& Cyc_opt1_spec,{_tmp123,_tmp123,_tmp123 + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 998
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0U,Cyc_print_version};static char _tmp124[10U]="--version";static char _tmp125[1U]="";static char _tmp126[35U]="Print version information and exit";struct _tuple24 Cyc_opt2_tuple={{_tmp124,_tmp124,_tmp124 + 10U},0,{_tmp125,_tmp125,_tmp125 + 1U},(void*)& Cyc_opt2_spec,{_tmp126,_tmp126,_tmp126 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 1001
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5U,Cyc_set_output_file};static char _tmp127[3U]="-o";static char _tmp128[8U]=" <file>";static char _tmp129[35U]="Set the output file name to <file>";struct _tuple24 Cyc_opt3_tuple={{_tmp127,_tmp127,_tmp127 + 3U},0,{_tmp128,_tmp128,_tmp128 + 8U},(void*)& Cyc_opt3_spec,{_tmp129,_tmp129,_tmp129 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 1004
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1U,Cyc_add_cpparg};static char _tmp12A[3U]="-D";static char _tmp12B[17U]="<name>[=<value>]";static char _tmp12C[32U]="Pass definition to preprocessor";struct _tuple24 Cyc_opt4_tuple={{_tmp12A,_tmp12A,_tmp12A + 3U},1,{_tmp12B,_tmp12B,_tmp12B + 17U},(void*)& Cyc_opt4_spec,{_tmp12C,_tmp12C,_tmp12C + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 1007
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1U,Cyc_Specsfile_add_cyclone_exec_path};static char _tmp12D[3U]="-B";static char _tmp12E[7U]="<file>";static char _tmp12F[60U]="Add to the list of directories to search for compiler files";struct _tuple24 Cyc_opt5_tuple={{_tmp12D,_tmp12D,_tmp12D + 3U},1,{_tmp12E,_tmp12E,_tmp12E + 7U},(void*)& Cyc_opt5_spec,{_tmp12F,_tmp12F,_tmp12F + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 1010
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1U,Cyc_add_cpparg};static char _tmp130[3U]="-I";static char _tmp131[6U]="<dir>";static char _tmp132[59U]="Add to the list of directories to search for include files";struct _tuple24 Cyc_opt6_tuple={{_tmp130,_tmp130,_tmp130 + 3U},1,{_tmp131,_tmp131,_tmp131 + 6U},(void*)& Cyc_opt6_spec,{_tmp132,_tmp132,_tmp132 + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 1013
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1U,Cyc_add_ccarg};static char _tmp133[3U]="-L";static char _tmp134[6U]="<dir>";static char _tmp135[38U]="Add to the list of directories for -l";struct _tuple24 Cyc_opt7_tuple={{_tmp133,_tmp133,_tmp133 + 3U},1,{_tmp134,_tmp134,_tmp134 + 6U},(void*)& Cyc_opt7_spec,{_tmp135,_tmp135,_tmp135 + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 1016
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1U,Cyc_add_libarg};static char _tmp136[3U]="-l";static char _tmp137[10U]="<libname>";static char _tmp138[13U]="Library file";struct _tuple24 Cyc_opt8_tuple={{_tmp136,_tmp136,_tmp136 + 3U},1,{_tmp137,_tmp137,_tmp137 + 10U},(void*)& Cyc_opt8_spec,{_tmp138,_tmp138,_tmp138 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 1019
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1U,Cyc_add_marg};static char _tmp139[3U]="-m";static char _tmp13A[9U]="<option>";static char _tmp13B[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple24 Cyc_opt9_tuple={{_tmp139,_tmp139,_tmp139 + 3U},1,{_tmp13A,_tmp13A,_tmp13A + 9U},(void*)& Cyc_opt9_spec,{_tmp13B,_tmp13B,_tmp13B + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 1022
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0U,Cyc_set_stop_after_objectfile};static char _tmp13C[3U]="-c";static char _tmp13D[1U]="";static char _tmp13E[61U]="Produce an object file instead of an executable; do not link";struct _tuple24 Cyc_opt10_tuple={{_tmp13C,_tmp13C,_tmp13C + 3U},0,{_tmp13D,_tmp13D,_tmp13D + 1U},(void*)& Cyc_opt10_spec,{_tmp13E,_tmp13E,_tmp13E + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 1025
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5U,Cyc_set_inputtype};static char _tmp13F[3U]="-x";static char _tmp140[12U]=" <language>";static char _tmp141[49U]="Specify <language> for the following input files";struct _tuple24 Cyc_opt11_tuple={{_tmp13F,_tmp13F,_tmp13F + 3U},0,{_tmp140,_tmp140,_tmp140 + 12U},(void*)& Cyc_opt11_spec,{_tmp141,_tmp141,_tmp141 + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1028
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0U,Cyc_set_pa};static char _tmp142[4U]="-pa";static char _tmp143[1U]="";static char _tmp144[33U]="Compile for profiling with aprof";struct _tuple24 Cyc_opt12_tuple={{_tmp142,_tmp142,_tmp142 + 4U},0,{_tmp143,_tmp143,_tmp143 + 1U},(void*)& Cyc_opt12_spec,{_tmp144,_tmp144,_tmp144 + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1031
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0U,Cyc_set_stop_after_asmfile};static char _tmp145[3U]="-S";static char _tmp146[1U]="";static char _tmp147[35U]="Stop after producing assembly code";struct _tuple24 Cyc_opt13_tuple={{_tmp145,_tmp145,_tmp145 + 3U},0,{_tmp146,_tmp146,_tmp146 + 1U},(void*)& Cyc_opt13_spec,{_tmp147,_tmp147,_tmp147 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1034
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0U,Cyc_set_produce_dependencies};static char _tmp148[3U]="-M";static char _tmp149[1U]="";static char _tmp14A[21U]="Produce dependencies";struct _tuple24 Cyc_opt14_tuple={{_tmp148,_tmp148,_tmp148 + 3U},0,{_tmp149,_tmp149,_tmp149 + 1U},(void*)& Cyc_opt14_spec,{_tmp14A,_tmp14A,_tmp14A + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1037
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1U,Cyc_add_cpparg};static char _tmp14B[4U]="-MG";static char _tmp14C[1U]="";static char _tmp14D[68U]="When producing dependencies assume that missing files are generated";struct _tuple24 Cyc_opt15_tuple={{_tmp14B,_tmp14B,_tmp14B + 4U},0,{_tmp14C,_tmp14C,_tmp14C + 1U},(void*)& Cyc_opt15_spec,{_tmp14D,_tmp14D,_tmp14D + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1041
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5U,Cyc_set_dependencies_target};static char _tmp14E[4U]="-MT";static char _tmp14F[10U]=" <target>";static char _tmp150[29U]="Give target for dependencies";struct _tuple24 Cyc_opt16_tuple={{_tmp14E,_tmp14E,_tmp14E + 4U},0,{_tmp14F,_tmp14F,_tmp14F + 10U},(void*)& Cyc_opt16_spec,{_tmp150,_tmp150,_tmp150 + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1044
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5U,Cyc_Specsfile_set_target_arch};static char _tmp151[3U]="-b";static char _tmp152[10U]="<machine>";static char _tmp153[19U]="Set target machine";struct _tuple24 Cyc_opt17_tuple={{_tmp151,_tmp151,_tmp151 + 3U},0,{_tmp152,_tmp152,_tmp152 + 10U},(void*)& Cyc_opt17_spec,{_tmp153,_tmp153,_tmp153 + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1047
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3U,& Cyc_NewControlFlow_warn_lose_unique};static char _tmp154[14U]="-Wlose-unique";static char _tmp155[1U]="";static char _tmp156[49U]="Try to warn when a unique pointer might get lost";struct _tuple24 Cyc_opt18_tuple={{_tmp154,_tmp154,_tmp154 + 14U},0,{_tmp155,_tmp155,_tmp155 + 1U},(void*)& Cyc_opt18_spec,{_tmp156,_tmp156,_tmp156 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1050
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3U,& Cyc_Binding_warn_override};static char _tmp157[11U]="-Woverride";static char _tmp158[1U]="";static char _tmp159[58U]="Warn when a local variable overrides an existing variable";struct _tuple24 Cyc_opt19_tuple={{_tmp157,_tmp157,_tmp157 + 11U},0,{_tmp158,_tmp158,_tmp158 + 1U},(void*)& Cyc_opt19_spec,{_tmp159,_tmp159,_tmp159 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1053
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0U,Cyc_set_all_warnings};static char _tmp15A[6U]="-Wall";static char _tmp15B[1U]="";static char _tmp15C[21U]="Turn on all warnings";struct _tuple24 Cyc_opt20_tuple={{_tmp15A,_tmp15A,_tmp15A + 6U},0,{_tmp15B,_tmp15B,_tmp15B + 1U},(void*)& Cyc_opt20_spec,{_tmp15C,_tmp15C,_tmp15C + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1056
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt21_spec={3U,& Cyc_stop_after_cpp_r};static char _tmp15D[3U]="-E";static char _tmp15E[1U]="";static char _tmp15F[25U]="Stop after preprocessing";struct _tuple24 Cyc_opt21_tuple={{_tmp15D,_tmp15D,_tmp15D + 3U},0,{_tmp15E,_tmp15E,_tmp15E + 1U},(void*)& Cyc_opt21_spec,{_tmp15F,_tmp15F,_tmp15F + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1059
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt22_spec={3U,& Cyc_parseonly_r};static char _tmp160[17U]="-stopafter-parse";static char _tmp161[1U]="";static char _tmp162[19U]="Stop after parsing";struct _tuple24 Cyc_opt22_tuple={{_tmp160,_tmp160,_tmp160 + 17U},0,{_tmp161,_tmp161,_tmp161 + 1U},(void*)& Cyc_opt22_spec,{_tmp162,_tmp162,_tmp162 + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1062
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt23_spec={3U,& Cyc_tc_r};static char _tmp163[14U]="-stopafter-tc";static char _tmp164[1U]="";static char _tmp165[25U]="Stop after type checking";struct _tuple24 Cyc_opt23_tuple={{_tmp163,_tmp163,_tmp163 + 14U},0,{_tmp164,_tmp164,_tmp164 + 1U},(void*)& Cyc_opt23_spec,{_tmp165,_tmp165,_tmp165 + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1065
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3U,& Cyc_noprint_r};static char _tmp166[9U]="-noprint";static char _tmp167[1U]="";static char _tmp168[42U]="Do not print output (when stopping early)";struct _tuple24 Cyc_opt24_tuple={{_tmp166,_tmp166,_tmp166 + 9U},0,{_tmp167,_tmp167,_tmp167 + 1U},(void*)& Cyc_opt24_spec,{_tmp168,_tmp168,_tmp168 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1068
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt25_spec={3U,& Cyc_cf_r};static char _tmp169[14U]="-stopafter-cf";static char _tmp16A[1U]="";static char _tmp16B[33U]="Stop after control-flow checking";struct _tuple24 Cyc_opt25_tuple={{_tmp169,_tmp169,_tmp169 + 14U},0,{_tmp16A,_tmp16A,_tmp16A + 1U},(void*)& Cyc_opt25_spec,{_tmp16B,_tmp16B,_tmp16B + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1071
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt26_spec={3U,& Cyc_toc_r};static char _tmp16C[15U]="-stopafter-toc";static char _tmp16D[1U]="";static char _tmp16E[28U]="Stop after translation to C";struct _tuple24 Cyc_opt26_tuple={{_tmp16C,_tmp16C,_tmp16C + 15U},0,{_tmp16D,_tmp16D,_tmp16D + 1U},(void*)& Cyc_opt26_spec,{_tmp16E,_tmp16E,_tmp16E + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1074
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3U,& Cyc_ic_r};static char _tmp16F[4U]="-ic";static char _tmp170[1U]="";static char _tmp171[26U]="Activate the link-checker";struct _tuple24 Cyc_opt27_tuple={{_tmp16F,_tmp16F,_tmp16F + 4U},0,{_tmp170,_tmp170,_tmp170 + 1U},(void*)& Cyc_opt27_spec,{_tmp171,_tmp171,_tmp171 + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1077
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3U,& Cyc_pp_r};static char _tmp172[4U]="-pp";static char _tmp173[1U]="";static char _tmp174[47U]="Pretty print the C code that Cyclone generates";struct _tuple24 Cyc_opt28_tuple={{_tmp172,_tmp172,_tmp172 + 4U},0,{_tmp173,_tmp173,_tmp173 + 1U},(void*)& Cyc_opt28_spec,{_tmp174,_tmp174,_tmp174 + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1080
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4U,& Cyc_pp_r};static char _tmp175[4U]="-up";static char _tmp176[1U]="";static char _tmp177[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple24 Cyc_opt29_tuple={{_tmp175,_tmp175,_tmp175 + 4U},0,{_tmp176,_tmp176,_tmp176 + 1U},(void*)& Cyc_opt29_spec,{_tmp177,_tmp177,_tmp177 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1083
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3U,& Cyc_elim_se_r};static char _tmp178[28U]="-elim-statement-expressions";static char _tmp179[1U]="";static char _tmp17A[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple24 Cyc_opt30_tuple={{_tmp178,_tmp178,_tmp178 + 28U},0,{_tmp179,_tmp179,_tmp179 + 1U},(void*)& Cyc_opt30_spec,{_tmp17A,_tmp17A,_tmp17A + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1087
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4U,& Cyc_elim_se_r};static char _tmp17B[31U]="-no-elim-statement-expressions";static char _tmp17C[1U]="";static char _tmp17D[47U]="Do not avoid statement expressions in C output";struct _tuple24 Cyc_opt31_tuple={{_tmp17B,_tmp17B,_tmp17B + 31U},0,{_tmp17C,_tmp17C,_tmp17C + 1U},(void*)& Cyc_opt31_spec,{_tmp17D,_tmp17D,_tmp17D + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1090
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0U,Cyc_set_tovc};static char _tmp17E[8U]="-un-gcc";static char _tmp17F[1U]="";static char _tmp180[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple24 Cyc_opt32_tuple={{_tmp17E,_tmp17E,_tmp17E + 8U},0,{_tmp17F,_tmp17F,_tmp17F + 1U},(void*)& Cyc_opt32_spec,{_tmp180,_tmp180,_tmp180 + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1093
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5U,Cyc_set_c_compiler};static char _tmp181[8U]="-c-comp";static char _tmp182[12U]=" <compiler>";static char _tmp183[40U]="Produce C output for the given compiler";struct _tuple24 Cyc_opt33_tuple={{_tmp181,_tmp181,_tmp181 + 8U},0,{_tmp182,_tmp182,_tmp182 + 12U},(void*)& Cyc_opt33_spec,{_tmp183,_tmp183,_tmp183 + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1096
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0U,Cyc_set_save_temps};static char _tmp184[12U]="-save-temps";static char _tmp185[1U]="";static char _tmp186[29U]="Don't delete temporary files";struct _tuple24 Cyc_opt34_tuple={{_tmp184,_tmp184,_tmp184 + 12U},0,{_tmp185,_tmp185,_tmp185 + 1U},(void*)& Cyc_opt34_spec,{_tmp186,_tmp186,_tmp186 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1099
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3U,& Cyc_save_c_r};static char _tmp187[8U]="-save-c";static char _tmp188[1U]="";static char _tmp189[31U]="Don't delete temporary C files";struct _tuple24 Cyc_opt35_tuple={{_tmp187,_tmp187,_tmp187 + 8U},0,{_tmp188,_tmp188,_tmp188 + 1U},(void*)& Cyc_opt35_spec,{_tmp189,_tmp189,_tmp189 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1102
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0U,Cyc_clear_lineno};static char _tmp18A[11U]="--nolineno";static char _tmp18B[1U]="";static char _tmp18C[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple24 Cyc_opt36_tuple={{_tmp18A,_tmp18A,_tmp18A + 11U},0,{_tmp18B,_tmp18B,_tmp18B + 1U},(void*)& Cyc_opt36_spec,{_tmp18C,_tmp18C,_tmp18C + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1105
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0U,Cyc_set_nochecks};static char _tmp18D[11U]="--nochecks";static char _tmp18E[1U]="";static char _tmp18F[27U]="Disable bounds/null checks";struct _tuple24 Cyc_opt37_tuple={{_tmp18D,_tmp18D,_tmp18D + 11U},0,{_tmp18E,_tmp18E,_tmp18E + 1U},(void*)& Cyc_opt37_spec,{_tmp18F,_tmp18F,_tmp18F + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1108
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0U,Cyc_set_nonullchecks};static char _tmp190[15U]="--nonullchecks";static char _tmp191[1U]="";static char _tmp192[20U]="Disable null checks";struct _tuple24 Cyc_opt38_tuple={{_tmp190,_tmp190,_tmp190 + 15U},0,{_tmp191,_tmp191,_tmp191 + 1U},(void*)& Cyc_opt38_spec,{_tmp192,_tmp192,_tmp192 + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1111
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0U,Cyc_set_noboundschecks};static char _tmp193[17U]="--noboundschecks";static char _tmp194[1U]="";static char _tmp195[22U]="Disable bounds checks";struct _tuple24 Cyc_opt39_tuple={{_tmp193,_tmp193,_tmp193 + 17U},0,{_tmp194,_tmp194,_tmp194 + 1U},(void*)& Cyc_opt39_spec,{_tmp195,_tmp195,_tmp195 + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1114
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt40_spec={0U,Cyc_set_inline_functions};static char _tmp196[16U]="--inline-checks";static char _tmp197[1U]="";static char _tmp198[40U]="Inline bounds checks instead of #define";struct _tuple24 Cyc_opt40_tuple={{_tmp196,_tmp196,_tmp196 + 16U},0,{_tmp197,_tmp197,_tmp197 + 1U},(void*)& Cyc_opt40_spec,{_tmp198,_tmp198,_tmp198 + 40U}};struct Cyc_List_List Cyc_opt40={(void*)& Cyc_opt40_tuple,0};
# 1117
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5U,Cyc_set_cpp};static char _tmp199[9U]="-use-cpp";static char _tmp19A[7U]="<path>";static char _tmp19B[35U]="Indicate which preprocessor to use";struct _tuple24 Cyc_opt41_tuple={{_tmp199,_tmp199,_tmp199 + 9U},0,{_tmp19A,_tmp19A,_tmp19A + 7U},(void*)& Cyc_opt41_spec,{_tmp19B,_tmp19B,_tmp19B + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1120
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0U,Cyc_set_nocppprecomp};static char _tmp19C[16U]="-no-cpp-precomp";static char _tmp19D[1U]="";static char _tmp19E[40U]="Don't do smart preprocessing (mac only)";struct _tuple24 Cyc_opt42_tuple={{_tmp19C,_tmp19C,_tmp19C + 16U},0,{_tmp19D,_tmp19D,_tmp19D + 1U},(void*)& Cyc_opt42_spec,{_tmp19E,_tmp19E,_tmp19E + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1123
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0U,Cyc_set_nocyc};static char _tmp19F[7U]="-nocyc";static char _tmp1A0[1U]="";static char _tmp1A1[33U]="Don't add implicit namespace Cyc";struct _tuple24 Cyc_opt43_tuple={{_tmp19F,_tmp19F,_tmp19F + 7U},0,{_tmp1A0,_tmp1A0,_tmp1A0 + 1U},(void*)& Cyc_opt43_spec,{_tmp1A1,_tmp1A1,_tmp1A1 + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1126
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3U,& Cyc_nogc_r};static char _tmp1A2[6U]="-nogc";static char _tmp1A3[1U]="";static char _tmp1A4[36U]="Don't link in the garbage collector";struct _tuple24 Cyc_opt44_tuple={{_tmp1A2,_tmp1A2,_tmp1A2 + 6U},0,{_tmp1A3,_tmp1A3,_tmp1A3 + 1U},(void*)& Cyc_opt44_spec,{_tmp1A4,_tmp1A4,_tmp1A4 + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1129
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3U,& Cyc_noshake_r};static char _tmp1A5[16U]="-noremoveunused";static char _tmp1A6[1U]="";static char _tmp1A7[49U]="Don't remove externed variables that aren't used";struct _tuple24 Cyc_opt45_tuple={{_tmp1A5,_tmp1A5,_tmp1A5 + 16U},0,{_tmp1A6,_tmp1A6,_tmp1A6 + 1U},(void*)& Cyc_opt45_spec,{_tmp1A7,_tmp1A7,_tmp1A7 + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1132
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3U,& Cyc_noexpand_r};static char _tmp1A8[18U]="-noexpandtypedefs";static char _tmp1A9[1U]="";static char _tmp1AA[41U]="Don't expand typedefs in pretty printing";struct _tuple24 Cyc_opt46_tuple={{_tmp1A8,_tmp1A8,_tmp1A8 + 18U},0,{_tmp1A9,_tmp1A9,_tmp1A9 + 1U},(void*)& Cyc_opt46_spec,{_tmp1AA,_tmp1AA,_tmp1AA + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1135
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3U,& Cyc_print_all_tvars_r};static char _tmp1AB[15U]="-printalltvars";static char _tmp1AC[1U]="";static char _tmp1AD[57U]="Print all type variables (even implicit default effects)";struct _tuple24 Cyc_opt47_tuple={{_tmp1AB,_tmp1AB,_tmp1AB + 15U},0,{_tmp1AC,_tmp1AC,_tmp1AC + 1U},(void*)& Cyc_opt47_spec,{_tmp1AD,_tmp1AD,_tmp1AD + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1138
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3U,& Cyc_print_all_kinds_r};static char _tmp1AE[15U]="-printallkinds";static char _tmp1AF[1U]="";static char _tmp1B0[37U]="Always print kinds of type variables";struct _tuple24 Cyc_opt48_tuple={{_tmp1AE,_tmp1AE,_tmp1AE + 15U},0,{_tmp1AF,_tmp1AF,_tmp1AF + 1U},(void*)& Cyc_opt48_spec,{_tmp1B0,_tmp1B0,_tmp1B0 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1141
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3U,& Cyc_print_full_evars_r};static char _tmp1B1[16U]="-printfullevars";static char _tmp1B2[1U]="";static char _tmp1B3[50U]="Print full information for evars (type debugging)";struct _tuple24 Cyc_opt49_tuple={{_tmp1B1,_tmp1B1,_tmp1B1 + 16U},0,{_tmp1B2,_tmp1B2,_tmp1B2 + 1U},(void*)& Cyc_opt49_spec,{_tmp1B3,_tmp1B3,_tmp1B3 + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1144
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3U,& Cyc_print_all_effects_r};static char _tmp1B4[17U]="-printalleffects";static char _tmp1B5[1U]="";static char _tmp1B6[45U]="Print effects for functions (type debugging)";struct _tuple24 Cyc_opt50_tuple={{_tmp1B4,_tmp1B4,_tmp1B4 + 17U},0,{_tmp1B5,_tmp1B5,_tmp1B5 + 1U},(void*)& Cyc_opt50_spec,{_tmp1B6,_tmp1B6,_tmp1B6 + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1147
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3U,& Cyc_nocyc_setjmp_r};static char _tmp1B7[14U]="-nocyc_setjmp";static char _tmp1B8[1U]="";static char _tmp1B9[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple24 Cyc_opt51_tuple={{_tmp1B7,_tmp1B7,_tmp1B7 + 14U},0,{_tmp1B8,_tmp1B8,_tmp1B8 + 1U},(void*)& Cyc_opt51_spec,{_tmp1B9,_tmp1B9,_tmp1B9 + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1150
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3U,& Cyc_Lex_compile_for_boot_r};static char _tmp1BA[18U]="-compile-for-boot";static char _tmp1BB[1U]="";static char _tmp1BC[71U]="Compile using the special boot library instead of the standard library";struct _tuple24 Cyc_opt52_tuple={{_tmp1BA,_tmp1BA,_tmp1BA + 18U},0,{_tmp1BB,_tmp1BB,_tmp1BB + 1U},(void*)& Cyc_opt52_spec,{_tmp1BC,_tmp1BC,_tmp1BC + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1153
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5U,Cyc_set_cyc_include};static char _tmp1BD[4U]="-CI";static char _tmp1BE[8U]=" <file>";static char _tmp1BF[31U]="Set cyc_include.h to be <file>";struct _tuple24 Cyc_opt53_tuple={{_tmp1BD,_tmp1BD,_tmp1BD + 4U},0,{_tmp1BE,_tmp1BE,_tmp1BE + 8U},(void*)& Cyc_opt53_spec,{_tmp1BF,_tmp1BF,_tmp1BF + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1156
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3U,& Cyc_InsertChecks_warn_bounds_checks};static char _tmp1C0[18U]="-warnboundschecks";static char _tmp1C1[1U]="";static char _tmp1C2[44U]="Warn when bounds checks can't be eliminated";struct _tuple24 Cyc_opt54_tuple={{_tmp1C0,_tmp1C0,_tmp1C0 + 18U},0,{_tmp1C1,_tmp1C1,_tmp1C1 + 1U},(void*)& Cyc_opt54_spec,{_tmp1C2,_tmp1C2,_tmp1C2 + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1159
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3U,& Cyc_InsertChecks_warn_all_null_deref};static char _tmp1C3[16U]="-warnnullchecks";static char _tmp1C4[1U]="";static char _tmp1C5[45U]="Warn when any null check can't be eliminated";struct _tuple24 Cyc_opt55_tuple={{_tmp1C3,_tmp1C3,_tmp1C3 + 16U},0,{_tmp1C4,_tmp1C4,_tmp1C4 + 1U},(void*)& Cyc_opt55_spec,{_tmp1C5,_tmp1C5,_tmp1C5 + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1162
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3U,& Cyc_Tcutil_warn_alias_coerce};static char _tmp1C6[19U]="-warnaliascoercion";static char _tmp1C7[1U]="";static char _tmp1C8[41U]="Warn when any alias coercion is inserted";struct _tuple24 Cyc_opt56_tuple={{_tmp1C6,_tmp1C6,_tmp1C6 + 19U},0,{_tmp1C7,_tmp1C7,_tmp1C7 + 1U},(void*)& Cyc_opt56_spec,{_tmp1C8,_tmp1C8,_tmp1C8 + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1165
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3U,& Cyc_Tcutil_warn_region_coerce};static char _tmp1C9[18U]="-warnregioncoerce";static char _tmp1CA[1U]="";static char _tmp1CB[42U]="Warn when any region coercion is inserted";struct _tuple24 Cyc_opt57_tuple={{_tmp1C9,_tmp1C9,_tmp1C9 + 18U},0,{_tmp1CA,_tmp1CA,_tmp1CA + 1U},(void*)& Cyc_opt57_spec,{_tmp1CB,_tmp1CB,_tmp1CB + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1168
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3U,& Cyc_Parse_no_register};static char _tmp1CC[12U]="-noregister";static char _tmp1CD[1U]="";static char _tmp1CE[39U]="Treat register storage class as public";struct _tuple24 Cyc_opt58_tuple={{_tmp1CC,_tmp1CC,_tmp1CC + 12U},0,{_tmp1CD,_tmp1CD,_tmp1CD + 1U},(void*)& Cyc_opt58_spec,{_tmp1CE,_tmp1CE,_tmp1CE + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1171
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4U,& Cyc_Position_use_gcc_style_location};static char _tmp1CF[18U]="-detailedlocation";static char _tmp1D0[1U]="";static char _tmp1D1[58U]="Try to give more detailed location information for errors";struct _tuple24 Cyc_opt59_tuple={{_tmp1CF,_tmp1CF,_tmp1CF + 18U},0,{_tmp1D0,_tmp1D0,_tmp1D0 + 1U},(void*)& Cyc_opt59_spec,{_tmp1D1,_tmp1D1,_tmp1D1 + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1174
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0U,Cyc_set_port_c_code};static char _tmp1D2[6U]="-port";static char _tmp1D3[1U]="";static char _tmp1D4[38U]="Suggest how to port C code to Cyclone";struct _tuple24 Cyc_opt60_tuple={{_tmp1D2,_tmp1D2,_tmp1D2 + 6U},0,{_tmp1D3,_tmp1D3,_tmp1D3 + 1U},(void*)& Cyc_opt60_spec,{_tmp1D4,_tmp1D4,_tmp1D4 + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1177
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3U,& Cyc_Absyn_no_regions};static char _tmp1D5[11U]="-noregions";static char _tmp1D6[1U]="";static char _tmp1D7[39U]="Generate code that doesn't use regions";struct _tuple24 Cyc_opt61_tuple={{_tmp1D5,_tmp1D5,_tmp1D5 + 11U},0,{_tmp1D6,_tmp1D6,_tmp1D6 + 1U},(void*)& Cyc_opt61_spec,{_tmp1D7,_tmp1D7,_tmp1D7 + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1180
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5U,Cyc_add_iprefix};static char _tmp1D8[9U]="-iprefix";static char _tmp1D9[9U]="<prefix>";static char _tmp1DA[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple24 Cyc_opt62_tuple={{_tmp1D8,_tmp1D8,_tmp1D8 + 9U},0,{_tmp1D9,_tmp1D9,_tmp1D9 + 9U},(void*)& Cyc_opt62_spec,{_tmp1DA,_tmp1DA,_tmp1DA + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1183
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5U,Cyc_add_iwithprefix};static char _tmp1DB[13U]="-iwithprefix";static char _tmp1DC[6U]="<dir>";static char _tmp1DD[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple24 Cyc_opt63_tuple={{_tmp1DB,_tmp1DB,_tmp1DB + 13U},0,{_tmp1DC,_tmp1DC,_tmp1DC + 6U},(void*)& Cyc_opt63_spec,{_tmp1DD,_tmp1DD,_tmp1DD + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1186
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5U,Cyc_add_iwithprefixbefore};static char _tmp1DE[19U]="-iwithprefixbefore";static char _tmp1DF[6U]="<dir>";static char _tmp1E0[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple24 Cyc_opt64_tuple={{_tmp1DE,_tmp1DE,_tmp1DE + 19U},0,{_tmp1DF,_tmp1DF,_tmp1DF + 6U},(void*)& Cyc_opt64_spec,{_tmp1E0,_tmp1E0,_tmp1E0 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1189
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5U,Cyc_add_isystem};static char _tmp1E1[9U]="-isystem";static char _tmp1E2[6U]="<dir>";static char _tmp1E3[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple24 Cyc_opt65_tuple={{_tmp1E1,_tmp1E1,_tmp1E1 + 9U},0,{_tmp1E2,_tmp1E2,_tmp1E2 + 6U},(void*)& Cyc_opt65_spec,{_tmp1E3,_tmp1E3,_tmp1E3 + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1192
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5U,Cyc_add_idirafter};static char _tmp1E4[11U]="-idirafter";static char _tmp1E5[6U]="<dir>";static char _tmp1E6[46U]="Add the directory to the second include path.";struct _tuple24 Cyc_opt66_tuple={{_tmp1E4,_tmp1E4,_tmp1E4 + 11U},0,{_tmp1E5,_tmp1E5,_tmp1E5 + 6U},(void*)& Cyc_opt66_spec,{_tmp1E6,_tmp1E6,_tmp1E6 + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1195
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3U,& Cyc_generate_interface_r};static char _tmp1E7[15U]="--geninterface";static char _tmp1E8[1U]="";static char _tmp1E9[25U]="Generate interface files";struct _tuple24 Cyc_opt67_tuple={{_tmp1E7,_tmp1E7,_tmp1E7 + 15U},0,{_tmp1E8,_tmp1E8,_tmp1E8 + 1U},(void*)& Cyc_opt67_spec,{_tmp1E9,_tmp1E9,_tmp1E9 + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1198
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5U,Cyc_set_specified_interface};static char _tmp1EA[12U]="--interface";static char _tmp1EB[8U]=" <file>";static char _tmp1EC[37U]="Set the interface file to be <file>.";struct _tuple24 Cyc_opt68_tuple={{_tmp1EA,_tmp1EA,_tmp1EA + 12U},0,{_tmp1EB,_tmp1EB,_tmp1EB + 8U},(void*)& Cyc_opt68_spec,{_tmp1EC,_tmp1EC,_tmp1EC + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1201
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0U,Cyc_set_many_errors};static char _tmp1ED[13U]="--manyerrors";static char _tmp1EE[1U]="";static char _tmp1EF[36U]="don't stop after only a few errors.";struct _tuple24 Cyc_opt69_tuple={{_tmp1ED,_tmp1ED,_tmp1ED + 13U},0,{_tmp1EE,_tmp1EE,_tmp1EE + 1U},(void*)& Cyc_opt69_spec,{_tmp1EF,_tmp1EF,_tmp1EF + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1204
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3U,& Cyc_ParseErrors_print_state_and_token};static char _tmp1F0[13U]="--parsestate";static char _tmp1F1[1U]="";static char _tmp1F2[50U]="print the parse state and token on a syntax error";struct _tuple24 Cyc_opt70_tuple={{_tmp1F0,_tmp1F0,_tmp1F0 + 13U},0,{_tmp1F1,_tmp1F1,_tmp1F1 + 1U},(void*)& Cyc_opt70_spec,{_tmp1F2,_tmp1F2,_tmp1F2 + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1207
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0U,Cyc_noassume_gcc_flag};static char _tmp1F3[22U]="-known-gcc-flags-only";static char _tmp1F4[1U]="";static char _tmp1F5[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple24 Cyc_opt71_tuple={{_tmp1F3,_tmp1F3,_tmp1F3 + 22U},0,{_tmp1F4,_tmp1F4,_tmp1F4 + 1U},(void*)& Cyc_opt71_spec,{_tmp1F5,_tmp1F5,_tmp1F5 + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1210
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0U,Cyc_print_options};static char _tmp1F6[6U]="-help";static char _tmp1F7[1U]="";static char _tmp1F8[32U]="print out the available options";struct _tuple24 Cyc_opt72_tuple={{_tmp1F6,_tmp1F6,_tmp1F6 + 6U},0,{_tmp1F7,_tmp1F7,_tmp1F7 + 1U},(void*)& Cyc_opt72_spec,{_tmp1F8,_tmp1F8,_tmp1F8 + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1213
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0U,Cyc_print_options};static char _tmp1F9[7U]="-usage";static char _tmp1FA[1U]="";static char _tmp1FB[32U]="print out the available options";struct _tuple24 Cyc_opt73_tuple={{_tmp1F9,_tmp1F9,_tmp1F9 + 7U},0,{_tmp1FA,_tmp1FA,_tmp1FA + 1U},(void*)& Cyc_opt73_spec,{_tmp1FB,_tmp1FB,_tmp1FB + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1216
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0U,Cyc_set_notoseqc};static char _tmp1FC[10U]="-no-seq-c";static char _tmp1FD[1U]="";static char _tmp1FE[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple24 Cyc_opt74_tuple={{_tmp1FC,_tmp1FC,_tmp1FC + 10U},0,{_tmp1FD,_tmp1FD,_tmp1FD + 1U},(void*)& Cyc_opt74_spec,{_tmp1FE,_tmp1FE,_tmp1FE + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1219
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0U,Cyc_set_pg};static char _tmp1FF[4U]="-pg";static char _tmp200[1U]="";static char _tmp201[33U]="Compile for profiling with gprof";struct _tuple24 Cyc_opt75_tuple={{_tmp1FF,_tmp1FF,_tmp1FF + 4U},0,{_tmp200,_tmp200,_tmp200 + 1U},(void*)& Cyc_opt75_spec,{_tmp201,_tmp201,_tmp201 + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1223
struct Cyc_List_List*Cyc_global_options[75U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt74,& Cyc_opt75,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt40,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73};
# 1234
void Cyc_print_options(){
({struct Cyc_List_List*_tmp343=Cyc_global_options[0];Cyc_Arg_usage(_tmp343,({const char*_tmp202="<program.cyc>";_tag_dyneither(_tmp202,sizeof(char),14U);}));});}
# 1239
int Cyc_main(int argc,struct _dyneither_ptr argv){
# 1243
 GC_blacklist_warn_clear();{
# 1246
struct _dyneither_ptr optstring=({const char*_tmp27F="Options:";_tag_dyneither(_tmp27F,sizeof(char),9U);});
# 1248
{int i=1;for(0;i < 75U;++ i){
(*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,75U,sizeof(struct Cyc_List_List*),i - 1)))->tl=Cyc_global_options[i];}}{
# 1251
struct Cyc_List_List*options=Cyc_global_options[0];
# 1253
struct _dyneither_ptr _tmp203=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);
# 1259
struct _dyneither_ptr _tmp204=Cyc_find_in_arch_path(({const char*_tmp27E="cycspecs";_tag_dyneither(_tmp27E,sizeof(char),9U);}));
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp207=({struct Cyc_String_pa_PrintArg_struct _tmp29F;_tmp29F.tag=0U,_tmp29F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp204);_tmp29F;});void*_tmp205[1U];_tmp205[0]=& _tmp207;({struct Cyc___cycFILE*_tmp345=Cyc_stderr;struct _dyneither_ptr _tmp344=({const char*_tmp206="Reading from specs file %s\n";_tag_dyneither(_tmp206,sizeof(char),28U);});Cyc_fprintf(_tmp345,_tmp344,_tag_dyneither(_tmp205,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp208=Cyc_Specsfile_read_specs(_tmp204);
# 1263
struct _dyneither_ptr _tmp209=Cyc_Specsfile_split_specs(({struct Cyc_List_List*_tmp346=_tmp208;Cyc_Specsfile_get_spec(_tmp346,({const char*_tmp27D="cyclone";_tag_dyneither(_tmp27D,sizeof(char),8U);}));}));
if(({struct _dyneither_ptr*_tmp347=(struct _dyneither_ptr*)_tmp209.curr;_tmp347 != (struct _dyneither_ptr*)(_tag_dyneither(0,0,0)).curr;})){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp209);}{
# 1268
struct _dyneither_ptr _tmp20A=({struct Cyc_List_List*_tmp348=_tmp208;Cyc_Specsfile_get_spec(_tmp348,({const char*_tmp27C="cyclone_target_cflags";_tag_dyneither(_tmp27C,sizeof(char),22U);}));});
struct _dyneither_ptr _tmp20B=({struct Cyc_List_List*_tmp349=_tmp208;Cyc_Specsfile_get_spec(_tmp349,({const char*_tmp27B="cyclone_cc";_tag_dyneither(_tmp27B,sizeof(char),11U);}));});
if(!((unsigned int)_tmp20B.curr))_tmp20B=({const char*_tmp20C="gcc";_tag_dyneither(_tmp20C,sizeof(char),4U);});
Cyc_def_inc_path=({struct Cyc_List_List*_tmp34A=_tmp208;Cyc_Specsfile_get_spec(_tmp34A,({const char*_tmp20D="cyclone_inc_path";_tag_dyneither(_tmp20D,sizeof(char),17U);}));});
# 1274
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,_tmp203);
if(({struct _dyneither_ptr _tmp34B=(struct _dyneither_ptr)Cyc_cpp;Cyc_strcmp(_tmp34B,({const char*_tmp20E="";_tag_dyneither(_tmp20E,sizeof(char),1U);}));})== 0){
# 1292 "cyclone.cyc"
const char*_tmp20F=Cyc_produce_dependencies?"":" -E";
Cyc_set_cpp((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp212=({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0U,_tmp2A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20B);_tmp2A3;});struct Cyc_String_pa_PrintArg_struct _tmp213=({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0U,_tmp2A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20A);_tmp2A2;});struct Cyc_String_pa_PrintArg_struct _tmp214=({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0U,({
# 1295
struct _dyneither_ptr _tmp34C=(struct _dyneither_ptr)({const char*_tmp216=_tmp20F;_tag_dyneither(_tmp216,sizeof(char),_get_zero_arr_size_char((void*)_tmp216,1U));});_tmp2A1.f1=_tmp34C;});_tmp2A1;});struct Cyc_String_pa_PrintArg_struct _tmp215=({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0U,_tmp2A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp204);_tmp2A0;});void*_tmp210[4U];_tmp210[0]=& _tmp212,_tmp210[1]=& _tmp213,_tmp210[2]=& _tmp214,_tmp210[3]=& _tmp215;({struct _dyneither_ptr _tmp34D=({const char*_tmp211="%s %s -w -x c%s -specs %s";_tag_dyneither(_tmp211,sizeof(char),26U);});Cyc_aprintf(_tmp34D,_tag_dyneither(_tmp210,sizeof(void*),4U));});}));}
# 1297
if(Cyc_cyclone_files == 0  && Cyc_ccargs == 0){
({void*_tmp217=0U;({struct Cyc___cycFILE*_tmp34F=Cyc_stderr;struct _dyneither_ptr _tmp34E=({const char*_tmp218="missing file\n";_tag_dyneither(_tmp218,sizeof(char),14U);});Cyc_fprintf(_tmp34F,_tmp34E,_tag_dyneither(_tmp217,sizeof(void*),0U));});});
 exit(1);}
# 1306
if(!Cyc_stop_after_cpp_r  && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_find_in_arch_path(({const char*_tmp219="cyc_setjmp.h";_tag_dyneither(_tmp219,sizeof(char),13U);}));
if(!Cyc_stop_after_cpp_r  && Cyc_strlen((struct _dyneither_ptr)Cyc_cyc_include)== 0)
Cyc_cyc_include=Cyc_find_in_exec_path(({const char*_tmp21A="cyc_include.h";_tag_dyneither(_tmp21A,sizeof(char),14U);}));
# 1312
{struct _handler_cons _tmp21B;_push_handler(& _tmp21B);{int _tmp21D=0;if(setjmp(_tmp21B.handler))_tmp21D=1;if(!_tmp21D){
{struct Cyc_List_List*_tmp21E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);for(0;_tmp21E != 0;_tmp21E=_tmp21E->tl){
Cyc_process_file(*((struct _dyneither_ptr*)_tmp21E->hd));
if(Cyc_compile_failure){int _tmp21F=1;_npop_handler(0U);return _tmp21F;}}}
# 1313
;_pop_handler();}else{void*_tmp21C=(void*)_exn_thrown;void*_tmp220=_tmp21C;void*_tmp221;_LL1: _tmp221=_tmp220;_LL2:
# 1319
 if(Cyc_compile_failure)return 1;else{
Cyc_Core_rethrow(_tmp221);}_LL0:;}};}
# 1323
if(((Cyc_stop_after_cpp_r  || Cyc_parseonly_r) || Cyc_tc_r) || Cyc_toc_r)return 0;
# 1329
if(Cyc_ccargs == 0)return 0;
# 1332
if((unsigned int)Cyc_Specsfile_target_arch.curr  && (unsigned int)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg((struct _dyneither_ptr)({struct Cyc_List_List*_tmp353=({struct Cyc_List_List*_tmp224=_cycalloc(sizeof(*_tmp224));({struct _dyneither_ptr*_tmp352=({struct _dyneither_ptr*_tmp223=_cycalloc(sizeof(*_tmp223));({struct _dyneither_ptr _tmp351=({const char*_tmp222="";_tag_dyneither(_tmp222,sizeof(char),1U);});*_tmp223=_tmp351;});_tmp223;});_tmp224->hd=_tmp352;}),({
struct Cyc_List_List*_tmp350=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_tmp224->tl=_tmp350;});_tmp224;});
# 1333
Cyc_str_sepstr(_tmp353,({const char*_tmp225=" -L";_tag_dyneither(_tmp225,sizeof(char),4U);}));}));
# 1336
Cyc_add_ccarg((struct _dyneither_ptr)({struct _dyneither_ptr _tmp354=({const char*_tmp226="-L";_tag_dyneither(_tmp226,sizeof(char),3U);});Cyc_strconcat(_tmp354,(struct _dyneither_ptr)Cyc_Specsfile_def_lib_path);}));
# 1338
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
struct _dyneither_ptr _tmp227=({struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_tmp355,({const char*_tmp27A=" ";_tag_dyneither(_tmp27A,sizeof(char),2U);}));});
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
struct _dyneither_ptr _tmp228=({struct Cyc_List_List*_tmp359=({struct Cyc_List_List*_tmp278=_cycalloc(sizeof(*_tmp278));({struct _dyneither_ptr*_tmp358=({struct _dyneither_ptr*_tmp277=_cycalloc(sizeof(*_tmp277));({struct _dyneither_ptr _tmp357=({const char*_tmp276="";_tag_dyneither(_tmp276,sizeof(char),1U);});*_tmp277=_tmp357;});_tmp277;});_tmp278->hd=_tmp358;}),({
struct Cyc_List_List*_tmp356=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp278->tl=_tmp356;});_tmp278;});
# 1341
Cyc_str_sepstr(_tmp359,({const char*_tmp279=" ";_tag_dyneither(_tmp279,sizeof(char),2U);}));});
# 1344
struct Cyc_List_List*stdlib;
struct _dyneither_ptr stdlib_string;
int _tmp229=
((Cyc_stop_after_asmfile_r  || Cyc_stop_after_objectfile_r) || 
# 1349
Cyc_output_file != 0  && ({struct _dyneither_ptr _tmp35B=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp35B,({const char*_tmp274=".a";_tag_dyneither(_tmp274,sizeof(char),3U);}));})) || 
Cyc_output_file != 0  && ({struct _dyneither_ptr _tmp35A=*((struct _dyneither_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp35A,({const char*_tmp275=".lib";_tag_dyneither(_tmp275,sizeof(char),5U);}));});
if(_tmp229){
stdlib=0;
stdlib_string=(struct _dyneither_ptr)({const char*_tmp22A="";_tag_dyneither(_tmp22A,sizeof(char),1U);});}else{
# 1356
struct _dyneither_ptr libcyc_flag;
struct _dyneither_ptr nogc_filename;
struct _dyneither_ptr runtime_filename;
struct _dyneither_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=({const char*_tmp22B="-lcyc_a";_tag_dyneither(_tmp22B,sizeof(char),8U);});
nogc_filename=({const char*_tmp22C="nogc_a.a";_tag_dyneither(_tmp22C,sizeof(char),9U);});
runtime_filename=({const char*_tmp22D="runtime_cyc_a.a";_tag_dyneither(_tmp22D,sizeof(char),16U);});}else{
if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp22E="-lcyc_nocheck";_tag_dyneither(_tmp22E,sizeof(char),14U);});
nogc_filename=({const char*_tmp22F="nogc.a";_tag_dyneither(_tmp22F,sizeof(char),7U);});
runtime_filename=({const char*_tmp230="runtime_cyc.a";_tag_dyneither(_tmp230,sizeof(char),14U);});}else{
if(Cyc_pg_r){
libcyc_flag=({const char*_tmp231="-lcyc_pg";_tag_dyneither(_tmp231,sizeof(char),9U);});
runtime_filename=({const char*_tmp232="runtime_cyc_pg.a";_tag_dyneither(_tmp232,sizeof(char),17U);});
nogc_filename=({const char*_tmp233="nogc_pg.a";_tag_dyneither(_tmp233,sizeof(char),10U);});}else{
if(Cyc_Lex_compile_for_boot_r){
# 1374
libcyc_flag=({const char*_tmp234="-lcycboot";_tag_dyneither(_tmp234,sizeof(char),10U);});
nogc_filename=({const char*_tmp235="nogc.a";_tag_dyneither(_tmp235,sizeof(char),7U);});
runtime_filename=({const char*_tmp236="runtime_cyc.a";_tag_dyneither(_tmp236,sizeof(char),14U);});}else{
# 1378
libcyc_flag=({const char*_tmp237="-lcyc";_tag_dyneither(_tmp237,sizeof(char),6U);});
nogc_filename=({const char*_tmp238="nogc.a";_tag_dyneither(_tmp238,sizeof(char),7U);});
runtime_filename=({const char*_tmp239="runtime_cyc.a";_tag_dyneither(_tmp239,sizeof(char),14U);});}}}}
# 1382
gc_filename=({const char*_tmp23A="gc.a";_tag_dyneither(_tmp23A,sizeof(char),5U);});{
# 1384
struct _dyneither_ptr _tmp23B=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);
struct _dyneither_ptr _tmp23C=Cyc_find_in_arch_path(runtime_filename);
# 1388
stdlib=0;
stdlib_string=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp23F=({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0U,_tmp2A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)libcyc_flag);_tmp2A6;});struct Cyc_String_pa_PrintArg_struct _tmp240=({struct Cyc_String_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=0U,_tmp2A5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp23C);_tmp2A5;});struct Cyc_String_pa_PrintArg_struct _tmp241=({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0U,_tmp2A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp23B);_tmp2A4;});void*_tmp23D[3U];_tmp23D[0]=& _tmp23F,_tmp23D[1]=& _tmp240,_tmp23D[2]=& _tmp241;({struct _dyneither_ptr _tmp35C=({const char*_tmp23E=" %s %s %s";_tag_dyneither(_tmp23E,sizeof(char),10U);});Cyc_aprintf(_tmp35C,_tag_dyneither(_tmp23D,sizeof(void*),3U));});});};}
# 1392
if(Cyc_ic_r){struct _handler_cons _tmp242;_push_handler(& _tmp242);{int _tmp244=0;if(setjmp(_tmp242.handler))_tmp244=1;if(!_tmp244){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _dyneither_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp245=({struct Cyc_List_List*_tmp35D=stdlib;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp35D,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));});
if(!_tmp229)
_tmp245=({struct Cyc_List_List*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246->hd=Cyc_final_strptr,_tmp246->tl=_tmp245;_tmp246;});{
# 1400
struct Cyc_Interface_I*_tmp247=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _dyneither_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,_tmp245,_tmp245);
if(_tmp247 == 0){
({void*_tmp248=0U;({struct Cyc___cycFILE*_tmp35F=Cyc_stderr;struct _dyneither_ptr _tmp35E=({const char*_tmp249="Error: interfaces incompatible\n";_tag_dyneither(_tmp249,sizeof(char),32U);});Cyc_fprintf(_tmp35F,_tmp35E,_tag_dyneither(_tmp248,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp24A=1;_npop_handler(0U);return _tmp24A;};}
# 1407
if(_tmp229){
if(Cyc_output_file != 0){
struct _dyneither_ptr _tmp24B=({struct Cyc_String_pa_PrintArg_struct _tmp252=({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0U,({struct _dyneither_ptr _tmp360=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Filename_chop_extension(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));_tmp2A7.f1=_tmp360;});_tmp2A7;});void*_tmp250[1U];_tmp250[0]=& _tmp252;({struct _dyneither_ptr _tmp361=({const char*_tmp251="%s.cycio";_tag_dyneither(_tmp251,sizeof(char),9U);});Cyc_aprintf(_tmp361,_tag_dyneither(_tmp250,sizeof(void*),1U));});});
struct Cyc___cycFILE*_tmp24C=({struct _dyneither_ptr _tmp363=(struct _dyneither_ptr)_tmp24B;struct _dyneither_ptr _tmp362=({const char*_tmp24E="wb";_tag_dyneither(_tmp24E,sizeof(char),3U);});Cyc_try_file_open(_tmp363,_tmp362,({const char*_tmp24F="interface object file";_tag_dyneither(_tmp24F,sizeof(char),22U);}));});
if(_tmp24C == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp24D=1;_npop_handler(0U);return _tmp24D;};}
# 1416
Cyc_Interface_save(_tmp247,_tmp24C);
Cyc_file_close(_tmp24C);}}else{
# 1420
if(!({struct Cyc_Interface_I*_tmp367=Cyc_Interface_empty();struct Cyc_Interface_I*_tmp366=_tmp247;Cyc_Interface_is_subinterface(_tmp367,_tmp366,({struct _tuple20*_tmp255=_cycalloc(sizeof(*_tmp255));
({struct _dyneither_ptr _tmp365=({const char*_tmp253="empty interface";_tag_dyneither(_tmp253,sizeof(char),16U);});_tmp255->f1=_tmp365;}),({struct _dyneither_ptr _tmp364=({const char*_tmp254="global interface";_tag_dyneither(_tmp254,sizeof(char),17U);});_tmp255->f2=_tmp364;});_tmp255;}));})){
({void*_tmp256=0U;({struct Cyc___cycFILE*_tmp369=Cyc_stderr;struct _dyneither_ptr _tmp368=({const char*_tmp257="Error: some objects are still undefined\n";_tag_dyneither(_tmp257,sizeof(char),41U);});Cyc_fprintf(_tmp369,_tmp368,_tag_dyneither(_tmp256,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp258=1;_npop_handler(0U);return _tmp258;};}}};};
# 1393
;_pop_handler();}else{void*_tmp243=(void*)_exn_thrown;void*_tmp259=_tmp243;void*_tmp261;_LL6: _tmp261=_tmp259;_LL7:
# 1430
({struct Cyc_String_pa_PrintArg_struct _tmp25C=({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0U,({struct _dyneither_ptr _tmp36A=(struct _dyneither_ptr)({const char*_tmp260=Cyc_Core_get_exn_name(_tmp261);_tag_dyneither(_tmp260,sizeof(char),_get_zero_arr_size_char((void*)_tmp260,1U));});_tmp2AA.f1=_tmp36A;});_tmp2AA;});struct Cyc_String_pa_PrintArg_struct _tmp25D=({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0U,({struct _dyneither_ptr _tmp36B=(struct _dyneither_ptr)({const char*_tmp25F=Cyc_Core_get_exn_filename();_tag_dyneither(_tmp25F,sizeof(char),_get_zero_arr_size_char((void*)_tmp25F,1U));});_tmp2A9.f1=_tmp36B;});_tmp2A9;});struct Cyc_Int_pa_PrintArg_struct _tmp25E=({struct Cyc_Int_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=1U,({unsigned long _tmp36C=(unsigned long)Cyc_Core_get_exn_lineno();_tmp2A8.f1=_tmp36C;});_tmp2A8;});void*_tmp25A[3U];_tmp25A[0]=& _tmp25C,_tmp25A[1]=& _tmp25D,_tmp25A[2]=& _tmp25E;({struct Cyc___cycFILE*_tmp36E=Cyc_stderr;struct _dyneither_ptr _tmp36D=({const char*_tmp25B="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_dyneither(_tmp25B,sizeof(char),131U);});Cyc_fprintf(_tmp36E,_tmp36D,_tag_dyneither(_tmp25A,sizeof(void*),3U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;_LL5:;}};}{
# 1436
struct _dyneither_ptr outfile_str=({const char*_tmp273="";_tag_dyneither(_tmp273,sizeof(char),1U);});
if(Cyc_output_file != 0)
outfile_str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp264=({struct Cyc_String_pa_PrintArg_struct _tmp2AB;_tmp2AB.tag=0U,({struct _dyneither_ptr _tmp36F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_sh_escape_string(*((struct _dyneither_ptr*)_check_null(Cyc_output_file))));_tmp2AB.f1=_tmp36F;});_tmp2AB;});void*_tmp262[1U];_tmp262[0]=& _tmp264;({struct _dyneither_ptr _tmp370=({const char*_tmp263=" -o %s";_tag_dyneither(_tmp263,sizeof(char),7U);});Cyc_aprintf(_tmp370,_tag_dyneither(_tmp262,sizeof(void*),1U));});});{
# 1440
struct _dyneither_ptr _tmp265=({struct Cyc_String_pa_PrintArg_struct _tmp26D=({struct Cyc_String_pa_PrintArg_struct _tmp2B2;_tmp2B2.tag=0U,_tmp2B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20B);_tmp2B2;});struct Cyc_String_pa_PrintArg_struct _tmp26E=({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0U,_tmp2B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp20A);_tmp2B1;});struct Cyc_String_pa_PrintArg_struct _tmp26F=({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0U,_tmp2B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp227);_tmp2B0;});struct Cyc_String_pa_PrintArg_struct _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0U,_tmp2AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)outfile_str);_tmp2AF;});struct Cyc_String_pa_PrintArg_struct _tmp271=({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0U,_tmp2AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)stdlib_string);_tmp2AE;});struct Cyc_String_pa_PrintArg_struct _tmp272=({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0U,_tmp2AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp228);_tmp2AD;});void*_tmp26B[6U];_tmp26B[0]=& _tmp26D,_tmp26B[1]=& _tmp26E,_tmp26B[2]=& _tmp26F,_tmp26B[3]=& _tmp270,_tmp26B[4]=& _tmp271,_tmp26B[5]=& _tmp272;({struct _dyneither_ptr _tmp371=({const char*_tmp26C="%s %s %s%s %s%s";_tag_dyneither(_tmp26C,sizeof(char),16U);});Cyc_aprintf(_tmp371,_tag_dyneither(_tmp26B,sizeof(void*),6U));});});
# 1445
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _tmp268=({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0U,_tmp2AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp265);_tmp2AC;});void*_tmp266[1U];_tmp266[0]=& _tmp268;({struct Cyc___cycFILE*_tmp373=Cyc_stderr;struct _dyneither_ptr _tmp372=({const char*_tmp267="%s\n";_tag_dyneither(_tmp267,sizeof(char),4U);});Cyc_fprintf(_tmp373,_tmp372,_tag_dyneither(_tmp266,sizeof(void*),1U));});});Cyc_fflush(Cyc_stderr);}
if( system((const char*)_check_null(_untag_dyneither_ptr(_tmp265,sizeof(char),1U)))!= 0){
({void*_tmp269=0U;({struct Cyc___cycFILE*_tmp375=Cyc_stderr;struct _dyneither_ptr _tmp374=({const char*_tmp26A="Error: C compiler failed\n";_tag_dyneither(_tmp26A,sizeof(char),26U);});Cyc_fprintf(_tmp375,_tmp374,_tag_dyneither(_tmp269,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1452
Cyc_remove_cfiles();
# 1454
return Cyc_compile_failure?1: 0;};};};};};};};};}
