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
# 41 "cycboot.h"
double modf(double,double*);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 161
int Cyc_putc(int,struct Cyc___cycFILE*);
# 174
struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 228
int Cyc_vprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
# 239
int Cyc_vsnprintf(struct _dyneither_ptr,unsigned long,struct _dyneither_ptr,struct _dyneither_ptr);
# 243
int Cyc_vsprintf(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 87 "printf.cyc"
static struct _dyneither_ptr Cyc_parg2string(void*x){
void*_tmp0=x;switch(*((int*)_tmp0)){case 0U: _LL1: _LL2:
 return({const char*_tmp1="string";_tag_dyneither(_tmp1,sizeof(char),7U);});case 1U: _LL3: _LL4:
 return({const char*_tmp2="int";_tag_dyneither(_tmp2,sizeof(char),4U);});case 2U: _LL5: _LL6:
# 92
 return({const char*_tmp3="double";_tag_dyneither(_tmp3,sizeof(char),7U);});case 3U: _LL7: _LL8:
 return({const char*_tmp4="long double";_tag_dyneither(_tmp4,sizeof(char),12U);});case 4U: _LL9: _LLA:
 return({const char*_tmp5="short *";_tag_dyneither(_tmp5,sizeof(char),8U);});default: _LLB: _LLC:
 return({const char*_tmp6="unsigned long *";_tag_dyneither(_tmp6,sizeof(char),16U);});}_LL0:;}
# 99
static void*Cyc_badarg(struct _dyneither_ptr s){
return(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=Cyc_Core_Invalid_argument,_tmp7->f1=s;_tmp7;}));}
# 104
static int Cyc_va_arg_int(struct _dyneither_ptr ap){
void*_tmp8=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmp9=_tmp8;unsigned long _tmpB;if(((struct Cyc_Int_pa_PrintArg_struct*)_tmp9)->tag == 1U){_LL1: _tmpB=((struct Cyc_Int_pa_PrintArg_struct*)_tmp9)->f1;_LL2:
 return(int)_tmpB;}else{_LL3: _LL4:
 return((int(*)(struct _dyneither_ptr s))Cyc_badarg)(({const char*_tmpA="printf expected int";_tag_dyneither(_tmpA,sizeof(char),20U);}));}_LL0:;}
# 111
static long Cyc_va_arg_long(struct _dyneither_ptr ap){
void*_tmpC=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmpD=_tmpC;unsigned long _tmpF;if(((struct Cyc_Int_pa_PrintArg_struct*)_tmpD)->tag == 1U){_LL1: _tmpF=((struct Cyc_Int_pa_PrintArg_struct*)_tmpD)->f1;_LL2:
 return(long)_tmpF;}else{_LL3: _LL4:
 return((long(*)(struct _dyneither_ptr s))Cyc_badarg)(({const char*_tmpE="printf expected int";_tag_dyneither(_tmpE,sizeof(char),20U);}));}_LL0:;}
# 118
static unsigned long Cyc_va_arg_ulong(struct _dyneither_ptr ap){
void*_tmp10=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmp11=_tmp10;unsigned long _tmp13;if(((struct Cyc_Int_pa_PrintArg_struct*)_tmp11)->tag == 1U){_LL1: _tmp13=((struct Cyc_Int_pa_PrintArg_struct*)_tmp11)->f1;_LL2:
 return _tmp13;}else{_LL3: _LL4:
 return((unsigned long(*)(struct _dyneither_ptr s))Cyc_badarg)(({const char*_tmp12="printf expected int";_tag_dyneither(_tmp12,sizeof(char),20U);}));}_LL0:;}
# 125
static unsigned long Cyc_va_arg_uint(struct _dyneither_ptr ap){
void*_tmp14=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmp15=_tmp14;unsigned long _tmp17;if(((struct Cyc_Int_pa_PrintArg_struct*)_tmp15)->tag == 1U){_LL1: _tmp17=((struct Cyc_Int_pa_PrintArg_struct*)_tmp15)->f1;_LL2:
 return _tmp17;}else{_LL3: _LL4:
 return((unsigned long(*)(struct _dyneither_ptr s))Cyc_badarg)(({const char*_tmp16="printf expected int";_tag_dyneither(_tmp16,sizeof(char),20U);}));}_LL0:;}
# 133
static double Cyc_va_arg_double(struct _dyneither_ptr ap){
void*_tmp18=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmp19=_tmp18;long double _tmp1E;double _tmp1D;switch(*((int*)_tmp19)){case 2U: _LL1: _tmp1D=((struct Cyc_Double_pa_PrintArg_struct*)_tmp19)->f1;_LL2:
 return _tmp1D;case 3U: _LL3: _tmp1E=((struct Cyc_LongDouble_pa_PrintArg_struct*)_tmp19)->f1;_LL4:
 return(double)_tmp1E;default: _LL5: _LL6:
# 138
(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmpC0;_tmpC0.tag=0U,({struct _dyneither_ptr _tmpC4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_parg2string(*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U))));_tmpC0.f1=_tmpC4;});_tmpC0;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct _dyneither_ptr _tmpC5=({const char*_tmp1B="printf expected double but found %s";_tag_dyneither(_tmp1B,sizeof(char),36U);});Cyc_aprintf(_tmpC5,_tag_dyneither(_tmp1A,sizeof(void*),1U));});})));}_LL0:;}
# 144
static short*Cyc_va_arg_short_ptr(struct _dyneither_ptr ap){
void*_tmp1F=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmp20=_tmp1F;short*_tmp22;if(((struct Cyc_ShortPtr_pa_PrintArg_struct*)_tmp20)->tag == 4U){_LL1: _tmp22=((struct Cyc_ShortPtr_pa_PrintArg_struct*)_tmp20)->f1;_LL2:
 return _tmp22;}else{_LL3: _LL4:
(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)(({const char*_tmp21="printf expected short pointer";_tag_dyneither(_tmp21,sizeof(char),30U);})));}_LL0:;}
# 152
static unsigned long*Cyc_va_arg_int_ptr(struct _dyneither_ptr ap){
void*_tmp23=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmp24=_tmp23;unsigned long*_tmp26;if(((struct Cyc_IntPtr_pa_PrintArg_struct*)_tmp24)->tag == 5U){_LL1: _tmp26=((struct Cyc_IntPtr_pa_PrintArg_struct*)_tmp24)->f1;_LL2:
 return _tmp26;}else{_LL3: _LL4:
(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)(({const char*_tmp25="printf expected int pointer";_tag_dyneither(_tmp25,sizeof(char),28U);})));}_LL0:;}
# 160
static const struct _dyneither_ptr Cyc_va_arg_string(struct _dyneither_ptr ap){
void*_tmp27=*((void**)_check_dyneither_subscript(ap,sizeof(void*),0U));void*_tmp28=_tmp27;struct _dyneither_ptr _tmp2A;if(((struct Cyc_String_pa_PrintArg_struct*)_tmp28)->tag == 0U){_LL1: _tmp2A=((struct Cyc_String_pa_PrintArg_struct*)_tmp28)->f1;_LL2:
 return _tmp2A;}else{_LL3: _LL4:
(int)_throw(((void*(*)(struct _dyneither_ptr s))Cyc_badarg)(({const char*_tmp29="printf expected string";_tag_dyneither(_tmp29,sizeof(char),23U);})));}_LL0:;}
# 177 "printf.cyc"
int Cyc___cvt_double(double number,int prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,struct _dyneither_ptr endp);
# 206 "printf.cyc"
enum Cyc_BASE{Cyc_OCT  = 0U,Cyc_DEC  = 1U,Cyc_HEX  = 2U};
# 212
inline static int Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr s,int howmany){
# 214
int i=0;
while(i < howmany){
if(ioputc((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),0U)),ioputc_env)== - 1)return i;
_dyneither_ptr_inplace_plus(& s,sizeof(char),1);
++ i;}
# 220
return i;}
# 223
static int Cyc__IO_nzsputn(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr s,int howmany){
# 225
int i=0;
while(i < howmany){
char c;
if((c=*((const char*)_check_dyneither_subscript(s,sizeof(char),0U)))== 0  || ioputc((int)c,ioputc_env)== - 1)return i;
_dyneither_ptr_inplace_plus(& s,sizeof(char),1);
++ i;}
# 232
return i;}
# 238
static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany){
# 240
int i=0;
while(i < howmany){
if(ioputc((int)c,ioputc_env)== - 1)return i;
++ i;}
# 245
return i;}
# 249
static struct _dyneither_ptr Cyc_my_memchr(struct _dyneither_ptr s,char c,int n){
int sz=(int)_get_dyneither_size(s,sizeof(char));
n=n < sz?n: sz;
for(0;n != 0;(n --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),1))){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0U))== c)return s;}
# 255
return _tag_dyneither(0,0,0);}
# 258
static struct _dyneither_ptr Cyc_my_nzmemchr(struct _dyneither_ptr s,char c,int n){
int sz=(int)_get_dyneither_size(s,sizeof(char));
n=n < sz?n: sz;
for(0;n != 0;(n --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),1))){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0U))== c)return s;}
# 264
return _tag_dyneither(0,0,0);}
# 267
inline static const unsigned long Cyc_my_strlen(struct _dyneither_ptr s){
unsigned int sz=_get_dyneither_size(s,sizeof(char));
unsigned int i=0U;
while(i < sz  && *((const char*)_check_dyneither_subscript(s,sizeof(char),(int)i))!= 0){++ i;}
return i;}
# 278
int Cyc__IO_vfprintf(int(*ioputc)(int,void*),void*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap){
# 282
struct _dyneither_ptr fmt;
register int ch;
register int n;
# 286
struct _dyneither_ptr cp=_tag_dyneither(0,0,0);
# 289
struct _dyneither_ptr cp2=_tag_dyneither(0,0,0);
# 292
struct _dyneither_ptr cp3=_tag_dyneither(0,0,0);
# 295
int which_cp;
struct _dyneither_ptr fmark;
register int flags;
int ret;
int width;
int prec;
char sign;
# 303
char sign_string[2U];sign_string[0]='\000',sign_string[1]='\000';{
int softsign=0;
double _double;
int fpprec;
unsigned long _ulong;
int dprec;
int dpad;
int fieldsz;
# 314
int size=0;
# 316
char buf[349U];({{unsigned int _tmpC1=348U;unsigned int i;for(i=0;i < _tmpC1;++ i){buf[i]='\000';}buf[_tmpC1]=0;}0;});{
char ox[2U];ox[0]='\000',ox[1]='\000';{
enum Cyc_BASE base;
# 342 "printf.cyc"
fmt=fmt0;
ret=0;
# 348
for(0;1;0){
# 351
fmark=fmt;{
unsigned int fmt_sz=_get_dyneither_size(fmt,sizeof(char));
for(n=0;(n < fmt_sz  && (ch=(int)*((const char*)_check_dyneither_subscript(fmt,sizeof(char),n)))!= '\000') && ch != '%';++ n){
;}
fmt=_dyneither_ptr_plus(fmt,sizeof(char),n);
# 357
if((n=(fmt.curr - fmark.curr)/ sizeof(char))!= 0){
do{if(({int _tmpC6=Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)fmark,n);_tmpC6 != n;}))goto error;}while(0);
ret +=n;}
# 361
if(ch == '\000')
goto done;
_dyneither_ptr_inplace_plus(& fmt,sizeof(char),1);
# 365
flags=0;
dprec=0;
fpprec=0;
width=0;
prec=-1;
sign='\000';
# 372
rflag: ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
reswitch: which_cp=0;
{int _tmp2B=ch;switch(_tmp2B){case 32U: _LL1: _LL2:
# 381
 if(!((int)sign))
sign=' ';
goto rflag;case 35U: _LL3: _LL4:
# 385
 flags |=8;
goto rflag;case 42U: _LL5: _LL6:
# 394
 width=Cyc_va_arg_int(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
if(width >= 0)
goto rflag;
width=- width;
goto _LL8;case 45U: _LL7: _LL8:
# 400
 flags |=16;
flags &=~ 32;
goto rflag;case 43U: _LL9: _LLA:
# 404
 sign='+';
goto rflag;case 46U: _LLB: _LLC:
# 407
 if((ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U)))== '*'){
n=Cyc_va_arg_int(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
prec=n < 0?- 1: n;
goto rflag;}
# 412
n=0;
while((unsigned int)(ch - '0')<= 9){
n=10 * n + (ch - '0');
ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}
# 417
prec=n < 0?- 1: n;
goto reswitch;case 48U: _LLD: _LLE:
# 425
 if(!(flags & 16))
flags |=32;
goto rflag;case 49U: _LLF: _LL10:
 goto _LL12;case 50U: _LL11: _LL12: goto _LL14;case 51U: _LL13: _LL14: goto _LL16;case 52U: _LL15: _LL16: goto _LL18;case 53U: _LL17: _LL18:
 goto _LL1A;case 54U: _LL19: _LL1A: goto _LL1C;case 55U: _LL1B: _LL1C: goto _LL1E;case 56U: _LL1D: _LL1E: goto _LL20;case 57U: _LL1F: _LL20:
 n=0;
do{
n=10 * n + (ch - '0');
ch=(int)*((const char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}while((unsigned int)(ch - '0')<= 9);
# 435
width=n;
goto reswitch;case 76U: _LL21: _LL22:
# 438
 flags |=2;
goto rflag;case 104U: _LL23: _LL24:
# 441
 flags |=4;
goto rflag;case 108U: _LL25: _LL26:
# 444
 flags |=1;
goto rflag;case 99U: _LL27: _LL28:
# 447
 cp=({char*_tmp2C=buf;_tag_dyneither(_tmp2C,sizeof(char),349U);});
({struct _dyneither_ptr _tmp2D=cp;char _tmp2E=*((char*)_check_dyneither_subscript(_tmp2D,sizeof(char),0U));char _tmp2F=(char)Cyc_va_arg_int(ap);if(_get_dyneither_size(_tmp2D,sizeof(char))== 1U  && (_tmp2E == 0  && _tmp2F != 0))_throw_arraybounds();*((char*)_tmp2D.curr)=_tmp2F;});_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
size=1;
sign='\000';
goto _LL0;case 68U: _LL29: _LL2A:
# 453
 flags |=1;
goto _LL2C;case 100U: _LL2B: _LL2C:
 goto _LL2E;case 105U: _LL2D: _LL2E:
 _ulong=(unsigned long)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(long)((short)Cyc_va_arg_int(ap)): Cyc_va_arg_int(ap)));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
if((long)_ulong < 0){
_ulong=- _ulong;
sign='-';}
# 461
base=1U;
goto number;case 101U: _LL2F: _LL30:
 goto _LL32;case 69U: _LL31: _LL32: goto _LL34;case 102U: _LL33: _LL34: goto _LL36;case 70U: _LL35: _LL36: goto _LL38;case 103U: _LL37: _LL38:
 goto _LL3A;case 71U: _LL39: _LL3A:
 _double=Cyc_va_arg_double(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
# 470
if(prec > 39){
if(ch != 'g'  && ch != 'G'  || flags & 8)
fpprec=prec - 39;
prec=39;}else{
if(prec == - 1)
prec=6;}
# 482
cp=({char*_tmp30=buf;_tag_dyneither(_tmp30,sizeof(char),349U);});
({struct _dyneither_ptr _tmp31=cp;char _tmp32=*((char*)_check_dyneither_subscript(_tmp31,sizeof(char),0U));char _tmp33='\000';if(_get_dyneither_size(_tmp31,sizeof(char))== 1U  && (_tmp32 == 0  && _tmp33 != 0))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});
size=({double _tmpCB=_double;int _tmpCA=prec;int _tmpC9=flags;int _tmpC8=ch;struct _dyneither_ptr _tmpC7=cp;Cyc___cvt_double(_tmpCB,_tmpCA,_tmpC9,& softsign,_tmpC8,_tmpC7,
# 486
_dyneither_ptr_plus(({char*_tmp34=buf;_tag_dyneither(_tmp34,sizeof(char),349U);}),sizeof(char),(int)(sizeof(buf)- 1)));});
if(softsign)
sign='-';
if(*((char*)_check_dyneither_subscript(cp,sizeof(char),0U))== '\000')
_dyneither_ptr_inplace_plus(& cp,sizeof(char),1);
goto _LL0;case 110U: _LL3B: _LL3C:
# 493
 if(flags & 1)
({unsigned long _tmpCC=(unsigned long)ret;*Cyc_va_arg_int_ptr(ap)=_tmpCC;});else{
if(flags & 4)
({short _tmpCD=(short)ret;*Cyc_va_arg_short_ptr(ap)=_tmpCD;});else{
# 498
({unsigned long _tmpCE=(unsigned long)ret;*Cyc_va_arg_int_ptr(ap)=_tmpCE;});}}
_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 79U: _LL3D: _LL3E:
# 502
 flags |=1;
goto _LL40;case 111U: _LL3F: _LL40:
# 505
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=0U;
goto nosign;case 112U: _LL41: _LL42:
# 517 "printf.cyc"
 _ulong=(unsigned long)Cyc_va_arg_long(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=2U;
flags |=64;
ch=(int)'x';
goto nosign;case 115U: _LL43: _LL44: {
# 523
struct _dyneither_ptr b=Cyc_va_arg_string(ap);_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
which_cp=3;cp3=b;
if(prec >= 0){
struct _dyneither_ptr p=Cyc_my_nzmemchr(cp3,'\000',prec);
if(({char*_tmpCF=(char*)p.curr;_tmpCF != (char*)(_tag_dyneither(0,0,0)).curr;})){
size=(p.curr - cp3.curr)/ sizeof(char);
if(size > prec)
size=prec;}else{
# 532
size=prec;}}else{
# 534
size=(int)Cyc_my_strlen(cp3);}
sign='\000';
goto _LL0;}case 85U: _LL45: _LL46:
# 538
 flags |=1;
goto _LL48;case 117U: _LL47: _LL48:
# 541
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=1U;
goto nosign;case 88U: _LL49: _LL4A:
 goto _LL4C;case 120U: _LL4B: _LL4C:
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_dyneither_ptr_inplace_plus(& ap,sizeof(void*),1);
base=2U;
# 548
if(flags & 8  && _ulong != 0)
flags |=64;
# 552
nosign: sign='\000';
# 558
number: if((dprec=prec)>= 0)
flags &=~ 32;
# 566
cp=_dyneither_ptr_plus(({char*_tmp35=buf;_tag_dyneither(_tmp35,sizeof(char),349U);}),sizeof(char),(308 + 39)+ 1);
if(_ulong != 0  || prec != 0){
struct _dyneither_ptr xdigs;
# 574
enum Cyc_BASE _tmp36=base;switch(_tmp36){case Cyc_OCT: _LL50: _LL51:
# 576
 do{
({struct _dyneither_ptr _tmp37=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp38=*((char*)_check_dyneither_subscript(_tmp37,sizeof(char),0U));char _tmp39=(char)((_ulong & 7)+ '0');if(_get_dyneither_size(_tmp37,sizeof(char))== 1U  && (_tmp38 == 0  && _tmp39 != 0))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});
_ulong >>=3U;}while((int)_ulong);
# 581
if(flags & 8  && *((char*)_check_dyneither_subscript(cp,sizeof(char),0U))!= '0')
({struct _dyneither_ptr _tmp3A=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp3B=*((char*)_check_dyneither_subscript(_tmp3A,sizeof(char),0U));char _tmp3C='0';if(_get_dyneither_size(_tmp3A,sizeof(char))== 1U  && (_tmp3B == 0  && _tmp3C != 0))_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});
goto _LL4F;case Cyc_DEC: _LL52: _LL53:
# 587
 while(_ulong >= 10){
({struct _dyneither_ptr _tmp3D=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp3E=*((char*)_check_dyneither_subscript(_tmp3D,sizeof(char),0U));char _tmp3F=(char)(_ulong % 10 + '0');if(_get_dyneither_size(_tmp3D,sizeof(char))== 1U  && (_tmp3E == 0  && _tmp3F != 0))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});
_ulong /=10U;}
# 591
({struct _dyneither_ptr _tmp40=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp41=*((char*)_check_dyneither_subscript(_tmp40,sizeof(char),0U));char _tmp42=(char)(_ulong + '0');if(_get_dyneither_size(_tmp40,sizeof(char))== 1U  && (_tmp41 == 0  && _tmp42 != 0))_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});
goto _LL4F;default: _LL54: _LL55:
# 595
 if(ch == 'X')
xdigs=({const char*_tmp43="0123456789ABCDEF";_tag_dyneither(_tmp43,sizeof(char),17U);});else{
# 598
xdigs=({const char*_tmp44="0123456789abcdef";_tag_dyneither(_tmp44,sizeof(char),17U);});}
do{
({struct _dyneither_ptr _tmp45=_dyneither_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp46=*((char*)_check_dyneither_subscript(_tmp45,sizeof(char),0U));char _tmp47=*((const char*)_check_dyneither_subscript(xdigs,sizeof(char),(int)(_ulong & 15)));if(_get_dyneither_size(_tmp45,sizeof(char))== 1U  && (_tmp46 == 0  && _tmp47 != 0))_throw_arraybounds();*((char*)_tmp45.curr)=_tmp47;});
_ulong >>=4U;}while((int)_ulong);
# 603
goto _LL4F;}_LL4F:;}
# 611
size=({unsigned char*_tmpD0=(_dyneither_ptr_plus(({char*_tmp48=buf;_tag_dyneither(_tmp48,sizeof(char),349U);}),sizeof(char),(308 + 39)+ 1)).curr;_tmpD0 - cp.curr;})/ sizeof(char);
skipsize:
 goto _LL0;default: _LL4D: _LL4E:
# 615
 if(ch == '\000')
goto done;
# 618
cp=({char*_tmp49=buf;_tag_dyneither(_tmp49,sizeof(char),349U);});
({struct _dyneither_ptr _tmp4A=cp;char _tmp4B=*((char*)_check_dyneither_subscript(_tmp4A,sizeof(char),0U));char _tmp4C=(char)ch;if(_get_dyneither_size(_tmp4A,sizeof(char))== 1U  && (_tmp4B == 0  && _tmp4C != 0))_throw_arraybounds();*((char*)_tmp4A.curr)=_tmp4C;});
size=1;
sign='\000';
goto _LL0;}_LL0:;}
# 647 "printf.cyc"
fieldsz=size + fpprec;
dpad=dprec - size;
if(dpad < 0)
dpad=0;
# 652
if((int)sign)
++ fieldsz;else{
if(flags & 64)
fieldsz +=2;}
fieldsz +=dpad;
# 659
if((flags & (16 | 32))== 0){
if(({int _tmpD1=Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz);_tmpD1 < width - fieldsz;}))goto error;}
# 663
if((int)sign){
({struct _dyneither_ptr _tmp4E=_dyneither_ptr_plus(({char*_tmp4D=sign_string;_tag_dyneither(_tmp4D,sizeof(char),2U);}),sizeof(char),0);char _tmp4F=*((char*)_check_dyneither_subscript(_tmp4E,sizeof(char),0U));char _tmp50=sign;if(_get_dyneither_size(_tmp4E,sizeof(char))== 1U  && (_tmp4F == 0  && _tmp50 != 0))_throw_arraybounds();*((char*)_tmp4E.curr)=_tmp50;});
do{if(({int(*_tmpD3)(int,void*)=ioputc;void*_tmpD2=ioputc_env;Cyc__IO_sputn(_tmpD3,_tmpD2,(struct _dyneither_ptr)({char*_tmp51=sign_string;_tag_dyneither(_tmp51,sizeof(char),2U);}),1);})!= 1)goto error;}while(0);}else{
if(flags & 64){
ox[0]='0';
ox[1]=(char)ch;
do{if(({int(*_tmpD5)(int,void*)=ioputc;void*_tmpD4=ioputc_env;Cyc__IO_nzsputn(_tmpD5,_tmpD4,_tag_dyneither(ox,sizeof(char),2U),2);})!= 2)goto error;}while(0);}}
# 673
if((flags & (16 | 32))== 32){
if(({int _tmpD6=Cyc__IO_padn(ioputc,ioputc_env,'0',width - fieldsz);_tmpD6 < width - fieldsz;}))goto error;}
# 677
if(({int _tmpD7=Cyc__IO_padn(ioputc,ioputc_env,'0',dpad);_tmpD7 < dpad;}))goto error;
# 680
if(which_cp == 0)
do{if(({int _tmpD8=Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)cp,size);_tmpD8 != size;}))goto error;}while(0);else{
if(which_cp == 2)
do{if(({int _tmpD9=Cyc__IO_sputn(ioputc,ioputc_env,(struct _dyneither_ptr)cp2,size);_tmpD9 != size;}))goto error;}while(0);else{
if(which_cp == 3)
do{if(({int _tmpDA=Cyc__IO_nzsputn(ioputc,ioputc_env,(struct _dyneither_ptr)cp3,size);_tmpDA != size;}))goto error;}while(0);}}
# 688
if(({int _tmpDB=Cyc__IO_padn(ioputc,ioputc_env,'0',fpprec);_tmpDB < fpprec;}))goto error;
# 691
if(flags & 16){
if(({int _tmpDC=Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz);_tmpDC < width - fieldsz;}))goto error;}
# 695
ret +=width > fieldsz?width: fieldsz;};}
# 698
done:
 return ret;
error:
 return - 1;};};};}
# 705
static struct _dyneither_ptr Cyc_exponent(struct _dyneither_ptr p,int exp,int fmtch){
# 707
struct _dyneither_ptr t;
char expbuffer[309U];({{unsigned int _tmpC2=308U;unsigned int i;for(i=0;i < _tmpC2;++ i){expbuffer[i]='0';}expbuffer[_tmpC2]=0;}0;});{
struct _dyneither_ptr expbuf=({char*_tmp6A=(char*)expbuffer;_tag_dyneither(_tmp6A,sizeof(char),309U);});
({struct _dyneither_ptr _tmp52=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp53=*((char*)_check_dyneither_subscript(_tmp52,sizeof(char),0U));char _tmp54=(char)fmtch;if(_get_dyneither_size(_tmp52,sizeof(char))== 1U  && (_tmp53 == 0  && _tmp54 != 0))_throw_arraybounds();*((char*)_tmp52.curr)=_tmp54;});
if(exp < 0){
exp=- exp;
({struct _dyneither_ptr _tmp55=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp56=*((char*)_check_dyneither_subscript(_tmp55,sizeof(char),0U));char _tmp57='-';if(_get_dyneither_size(_tmp55,sizeof(char))== 1U  && (_tmp56 == 0  && _tmp57 != 0))_throw_arraybounds();*((char*)_tmp55.curr)=_tmp57;});}else{
# 716
({struct _dyneither_ptr _tmp58=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp59=*((char*)_check_dyneither_subscript(_tmp58,sizeof(char),0U));char _tmp5A='+';if(_get_dyneither_size(_tmp58,sizeof(char))== 1U  && (_tmp59 == 0  && _tmp5A != 0))_throw_arraybounds();*((char*)_tmp58.curr)=_tmp5A;});}
t=_dyneither_ptr_plus(expbuf,sizeof(char),308);
if(exp > 9){
do{
({struct _dyneither_ptr _tmp5B=_dyneither_ptr_inplace_plus(& t,sizeof(char),-1);char _tmp5C=*((char*)_check_dyneither_subscript(_tmp5B,sizeof(char),0U));char _tmp5D=(char)(exp % 10 + '0');if(_get_dyneither_size(_tmp5B,sizeof(char))== 1U  && (_tmp5C == 0  && _tmp5D != 0))_throw_arraybounds();*((char*)_tmp5B.curr)=_tmp5D;});}while((exp /=10)> 9);
# 722
({struct _dyneither_ptr _tmp5E=_dyneither_ptr_inplace_plus(& t,sizeof(char),-1);char _tmp5F=*((char*)_check_dyneither_subscript(_tmp5E,sizeof(char),0U));char _tmp60=(char)(exp + '0');if(_get_dyneither_size(_tmp5E,sizeof(char))== 1U  && (_tmp5F == 0  && _tmp60 != 0))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});
for(0;({char*_tmpDD=(char*)t.curr;_tmpDD < (char*)(_dyneither_ptr_plus(expbuf,sizeof(char),308)).curr;});({struct _dyneither_ptr _tmp61=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp62=*((char*)_check_dyneither_subscript(_tmp61,sizeof(char),0U));char _tmp63=*((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1),sizeof(char),0U));if(_get_dyneither_size(_tmp61,sizeof(char))== 1U  && (_tmp62 == 0  && _tmp63 != 0))_throw_arraybounds();*((char*)_tmp61.curr)=_tmp63;})){;}}else{
# 726
({struct _dyneither_ptr _tmp64=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp65=*((char*)_check_dyneither_subscript(_tmp64,sizeof(char),0U));char _tmp66='0';if(_get_dyneither_size(_tmp64,sizeof(char))== 1U  && (_tmp65 == 0  && _tmp66 != 0))_throw_arraybounds();*((char*)_tmp64.curr)=_tmp66;});
({struct _dyneither_ptr _tmp67=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp68=*((char*)_check_dyneither_subscript(_tmp67,sizeof(char),0U));char _tmp69=(char)(exp + '0');if(_get_dyneither_size(_tmp67,sizeof(char))== 1U  && (_tmp68 == 0  && _tmp69 != 0))_throw_arraybounds();*((char*)_tmp67.curr)=_tmp69;});}
# 729
return p;};}
# 732
static struct _dyneither_ptr Cyc_sround(double fract,int*exp,struct _dyneither_ptr start,struct _dyneither_ptr end,char ch,int*signp){
# 736
double tmp=0.0;
# 738
if(fract != 0.0)
 modf(fract * 10,& tmp);else{
# 741
tmp=(double)(ch - '0');}
if(tmp > 4)
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){
if(*((char*)_check_dyneither_subscript(end,sizeof(char),0U))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);
if(({struct _dyneither_ptr _tmp6B=end;char _tmp6C=*((char*)_check_dyneither_subscript(_tmp6B,sizeof(char),0U));char _tmp6D=_tmp6C + '\001';if(_get_dyneither_size(_tmp6B,sizeof(char))== 1U  && (_tmp6C == 0  && _tmp6D != 0))_throw_arraybounds();*((char*)_tmp6B.curr)=_tmp6D;})<= '9')
break;
({struct _dyneither_ptr _tmp6E=end;char _tmp6F=*((char*)_check_dyneither_subscript(_tmp6E,sizeof(char),0U));char _tmp70='0';if(_get_dyneither_size(_tmp6E,sizeof(char))== 1U  && (_tmp6F == 0  && _tmp70 != 0))_throw_arraybounds();*((char*)_tmp6E.curr)=_tmp70;});
if((char*)end.curr == (char*)start.curr){
if((unsigned int)exp){
({struct _dyneither_ptr _tmp71=end;char _tmp72=*((char*)_check_dyneither_subscript(_tmp71,sizeof(char),0U));char _tmp73='1';if(_get_dyneither_size(_tmp71,sizeof(char))== 1U  && (_tmp72 == 0  && _tmp73 != 0))_throw_arraybounds();*((char*)_tmp71.curr)=_tmp73;});
++(*exp);}else{
# 755
({struct _dyneither_ptr _tmp74=_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);char _tmp75=*((char*)_check_dyneither_subscript(_tmp74,sizeof(char),0U));char _tmp76='1';if(_get_dyneither_size(_tmp74,sizeof(char))== 1U  && (_tmp75 == 0  && _tmp76 != 0))_throw_arraybounds();*((char*)_tmp74.curr)=_tmp76;});
_dyneither_ptr_inplace_plus(& start,sizeof(char),-1);}
# 758
break;}}else{
# 762
if(*signp == '-')
for(0;1;_dyneither_ptr_inplace_plus(& end,sizeof(char),-1)){
if(*((char*)_check_dyneither_subscript(end,sizeof(char),0U))== '.')
_dyneither_ptr_inplace_plus(& end,sizeof(char),-1);
if(*((char*)_check_dyneither_subscript(end,sizeof(char),0U))!= '0')
break;
if((char*)end.curr == (char*)start.curr)
*signp=0;}}
# 771
return start;}
# 774
int Cyc___cvt_double(double number,int prec,int flags,int*signp,int fmtch,struct _dyneither_ptr startp,struct _dyneither_ptr endp){
# 777
struct _dyneither_ptr p;struct _dyneither_ptr t;
register double fract;
int dotrim=0;int expcnt;int gformat=0;
double integer=0.0;double tmp=0.0;
# 782
expcnt=0;
if(number < 0){
number=- number;
*signp=(int)'-';}else{
# 787
*signp=0;}
# 789
fract= modf(number,& integer);
# 792
t=_dyneither_ptr_inplace_plus(& startp,sizeof(char),1);
# 798
for(p=_dyneither_ptr_plus(endp,sizeof(char),- 1);(char*)p.curr >= (char*)startp.curr  && integer != 0.0;++ expcnt){
tmp= modf(integer / 10,& integer);
({struct _dyneither_ptr _tmp77=_dyneither_ptr_inplace_plus_post(& p,sizeof(char),-1);char _tmp78=*((char*)_check_dyneither_subscript(_tmp77,sizeof(char),0U));char _tmp79=(char)((int)((tmp + .01)* 10)+ '0');if(_get_dyneither_size(_tmp77,sizeof(char))== 1U  && (_tmp78 == 0  && _tmp79 != 0))_throw_arraybounds();*((char*)_tmp77.curr)=_tmp79;});}
# 802
{int _tmp7A=fmtch;switch(_tmp7A){case 102U: _LL1: _LL2:
 goto _LL4;case 70U: _LL3: _LL4:
# 805
 if(expcnt)
for(0;({char*_tmpDE=(char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpDE < (char*)endp.curr;});({struct _dyneither_ptr _tmp7B=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp7C=*((char*)_check_dyneither_subscript(_tmp7B,sizeof(char),0U));char _tmp7D=*((char*)_check_dyneither_subscript(p,sizeof(char),0U));if(_get_dyneither_size(_tmp7B,sizeof(char))== 1U  && (_tmp7C == 0  && _tmp7D != 0))_throw_arraybounds();*((char*)_tmp7B.curr)=_tmp7D;})){;}else{
# 808
({struct _dyneither_ptr _tmp7E=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp7F=*((char*)_check_dyneither_subscript(_tmp7E,sizeof(char),0U));char _tmp80='0';if(_get_dyneither_size(_tmp7E,sizeof(char))== 1U  && (_tmp7F == 0  && _tmp80 != 0))_throw_arraybounds();*((char*)_tmp7E.curr)=_tmp80;});}
# 813
if(prec  || flags & 8)
({struct _dyneither_ptr _tmp81=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp82=*((char*)_check_dyneither_subscript(_tmp81,sizeof(char),0U));char _tmp83='.';if(_get_dyneither_size(_tmp81,sizeof(char))== 1U  && (_tmp82 == 0  && _tmp83 != 0))_throw_arraybounds();*((char*)_tmp81.curr)=_tmp83;});
# 816
if(fract != 0.0){
if(prec)
do{
fract= modf(fract * 10,& tmp);
({struct _dyneither_ptr _tmp84=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp85=*((char*)_check_dyneither_subscript(_tmp84,sizeof(char),0U));char _tmp86=(char)((int)tmp + '0');if(_get_dyneither_size(_tmp84,sizeof(char))== 1U  && (_tmp85 == 0  && _tmp86 != 0))_throw_arraybounds();*((char*)_tmp84.curr)=_tmp86;});}while(
-- prec  && fract != 0.0);
if(fract != 0.0)
startp=({double _tmpE1=fract;struct _dyneither_ptr _tmpE0=startp;struct _dyneither_ptr _tmpDF=
_dyneither_ptr_plus(t,sizeof(char),- 1);
# 823
Cyc_sround(_tmpE1,0,_tmpE0,_tmpDF,'\000',signp);});}
# 826
for(0;prec --;({struct _dyneither_ptr _tmp87=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp88=*((char*)_check_dyneither_subscript(_tmp87,sizeof(char),0U));char _tmp89='0';if(_get_dyneither_size(_tmp87,sizeof(char))== 1U  && (_tmp88 == 0  && _tmp89 != 0))_throw_arraybounds();*((char*)_tmp87.curr)=_tmp89;})){;}
goto _LL0;case 101U: _LL5: _LL6:
 goto _LL8;case 69U: _LL7: _LL8:
 eformat: if(expcnt){
({struct _dyneither_ptr _tmp8A=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp8B=*((char*)_check_dyneither_subscript(_tmp8A,sizeof(char),0U));char _tmp8C=*((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& p,sizeof(char),1),sizeof(char),0U));if(_get_dyneither_size(_tmp8A,sizeof(char))== 1U  && (_tmp8B == 0  && _tmp8C != 0))_throw_arraybounds();*((char*)_tmp8A.curr)=_tmp8C;});
if(prec  || flags & 8)
({struct _dyneither_ptr _tmp8D=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp8E=*((char*)_check_dyneither_subscript(_tmp8D,sizeof(char),0U));char _tmp8F='.';if(_get_dyneither_size(_tmp8D,sizeof(char))== 1U  && (_tmp8E == 0  && _tmp8F != 0))_throw_arraybounds();*((char*)_tmp8D.curr)=_tmp8F;});
# 834
for(0;prec  && ({char*_tmpE2=(char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpE2 < (char*)endp.curr;});-- prec){
({struct _dyneither_ptr _tmp90=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp91=*((char*)_check_dyneither_subscript(_tmp90,sizeof(char),0U));char _tmp92=*((char*)_check_dyneither_subscript(p,sizeof(char),0U));if(_get_dyneither_size(_tmp90,sizeof(char))== 1U  && (_tmp91 == 0  && _tmp92 != 0))_throw_arraybounds();*((char*)_tmp90.curr)=_tmp92;});}
# 841
if(!prec  && ({char*_tmpE3=(char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpE3 < (char*)endp.curr;})){
fract=(double)0;
startp=({struct _dyneither_ptr _tmpE6=startp;struct _dyneither_ptr _tmpE5=
_dyneither_ptr_plus(t,sizeof(char),- 1);
# 843
char _tmpE4=*((char*)_check_dyneither_subscript(p,sizeof(char),0U));Cyc_sround((double)0,& expcnt,_tmpE6,_tmpE5,_tmpE4,signp);});}
# 847
-- expcnt;}else{
# 850
if(fract != 0.0){
# 852
for(expcnt=- 1;1;-- expcnt){
fract= modf(fract * 10,& tmp);
if(tmp != 0.0)
break;}
# 857
({struct _dyneither_ptr _tmp93=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp94=*((char*)_check_dyneither_subscript(_tmp93,sizeof(char),0U));char _tmp95=(char)((int)tmp + '0');if(_get_dyneither_size(_tmp93,sizeof(char))== 1U  && (_tmp94 == 0  && _tmp95 != 0))_throw_arraybounds();*((char*)_tmp93.curr)=_tmp95;});
if(prec  || flags & 8)
({struct _dyneither_ptr _tmp96=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp97=*((char*)_check_dyneither_subscript(_tmp96,sizeof(char),0U));char _tmp98='.';if(_get_dyneither_size(_tmp96,sizeof(char))== 1U  && (_tmp97 == 0  && _tmp98 != 0))_throw_arraybounds();*((char*)_tmp96.curr)=_tmp98;});}else{
# 862
({struct _dyneither_ptr _tmp99=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp9A=*((char*)_check_dyneither_subscript(_tmp99,sizeof(char),0U));char _tmp9B='0';if(_get_dyneither_size(_tmp99,sizeof(char))== 1U  && (_tmp9A == 0  && _tmp9B != 0))_throw_arraybounds();*((char*)_tmp99.curr)=_tmp9B;});
if(prec  || flags & 8)
({struct _dyneither_ptr _tmp9C=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp9D=*((char*)_check_dyneither_subscript(_tmp9C,sizeof(char),0U));char _tmp9E='.';if(_get_dyneither_size(_tmp9C,sizeof(char))== 1U  && (_tmp9D == 0  && _tmp9E != 0))_throw_arraybounds();*((char*)_tmp9C.curr)=_tmp9E;});}}
# 867
if(fract != 0.0){
if(prec)
do{
fract= modf(fract * 10,& tmp);
({struct _dyneither_ptr _tmp9F=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpA0=*((char*)_check_dyneither_subscript(_tmp9F,sizeof(char),0U));char _tmpA1=(char)((int)tmp + '0');if(_get_dyneither_size(_tmp9F,sizeof(char))== 1U  && (_tmpA0 == 0  && _tmpA1 != 0))_throw_arraybounds();*((char*)_tmp9F.curr)=_tmpA1;});}while(
-- prec  && fract != 0.0);
if(fract != 0.0)
startp=({double _tmpE9=fract;struct _dyneither_ptr _tmpE8=startp;struct _dyneither_ptr _tmpE7=
_dyneither_ptr_plus(t,sizeof(char),- 1);
# 874
Cyc_sround(_tmpE9,& expcnt,_tmpE8,_tmpE7,'\000',signp);});}
# 878
for(0;prec --;({struct _dyneither_ptr _tmpA2=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpA3=*((char*)_check_dyneither_subscript(_tmpA2,sizeof(char),0U));char _tmpA4='0';if(_get_dyneither_size(_tmpA2,sizeof(char))== 1U  && (_tmpA3 == 0  && _tmpA4 != 0))_throw_arraybounds();*((char*)_tmpA2.curr)=_tmpA4;})){;}
# 881
if(gformat  && !(flags & 8)){
while((char*)t.curr > (char*)startp.curr  && *((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0U))== '0'){;}
if(*((char*)_check_dyneither_subscript(t,sizeof(char),0U))== '.')
_dyneither_ptr_inplace_plus(& t,sizeof(char),-1);
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}
# 887
t=Cyc_exponent(t,expcnt,fmtch);
goto _LL0;case 103U: _LL9: _LLA:
 goto _LLC;case 71U: _LLB: _LLC:
# 891
 if(!prec)
++ prec;
# 899
if(expcnt > prec  || (!expcnt  && fract != 0.0) && fract < .0001){
# 907
-- prec;
fmtch -=2;
gformat=1;
goto eformat;}
# 916
if(expcnt)
for(0;({char*_tmpEA=(char*)(_dyneither_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpEA < (char*)endp.curr;});(({struct _dyneither_ptr _tmpA5=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpA6=*((char*)_check_dyneither_subscript(_tmpA5,sizeof(char),0U));char _tmpA7=*((char*)_check_dyneither_subscript(p,sizeof(char),0U));if(_get_dyneither_size(_tmpA5,sizeof(char))== 1U  && (_tmpA6 == 0  && _tmpA7 != 0))_throw_arraybounds();*((char*)_tmpA5.curr)=_tmpA7;}),-- prec)){;}else{
# 919
({struct _dyneither_ptr _tmpA8=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpA9=*((char*)_check_dyneither_subscript(_tmpA8,sizeof(char),0U));char _tmpAA='0';if(_get_dyneither_size(_tmpA8,sizeof(char))== 1U  && (_tmpA9 == 0  && _tmpAA != 0))_throw_arraybounds();*((char*)_tmpA8.curr)=_tmpAA;});}
# 924
if(prec  || flags & 8){
dotrim=1;
({struct _dyneither_ptr _tmpAB=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpAC=*((char*)_check_dyneither_subscript(_tmpAB,sizeof(char),0U));char _tmpAD='.';if(_get_dyneither_size(_tmpAB,sizeof(char))== 1U  && (_tmpAC == 0  && _tmpAD != 0))_throw_arraybounds();*((char*)_tmpAB.curr)=_tmpAD;});}else{
# 929
dotrim=0;}
# 931
if(fract != 0.0){
if(prec){
# 935
do{
fract= modf(fract * 10,& tmp);
({struct _dyneither_ptr _tmpAE=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpAF=*((char*)_check_dyneither_subscript(_tmpAE,sizeof(char),0U));char _tmpB0=(char)((int)tmp + '0');if(_get_dyneither_size(_tmpAE,sizeof(char))== 1U  && (_tmpAF == 0  && _tmpB0 != 0))_throw_arraybounds();*((char*)_tmpAE.curr)=_tmpB0;});}while(
tmp == 0.0  && !expcnt);
while(-- prec  && fract != 0.0){
fract= modf(fract * 10,& tmp);
({struct _dyneither_ptr _tmpB1=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpB2=*((char*)_check_dyneither_subscript(_tmpB1,sizeof(char),0U));char _tmpB3=(char)((int)tmp + '0');if(_get_dyneither_size(_tmpB1,sizeof(char))== 1U  && (_tmpB2 == 0  && _tmpB3 != 0))_throw_arraybounds();*((char*)_tmpB1.curr)=_tmpB3;});}}
# 944
if(fract != 0.0)
startp=({double _tmpED=fract;struct _dyneither_ptr _tmpEC=startp;struct _dyneither_ptr _tmpEB=
_dyneither_ptr_plus(t,sizeof(char),- 1);
# 945
Cyc_sround(_tmpED,0,_tmpEC,_tmpEB,'\000',signp);});}
# 949
if(flags & 8)
for(0;prec --;({struct _dyneither_ptr _tmpB4=_dyneither_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpB5=*((char*)_check_dyneither_subscript(_tmpB4,sizeof(char),0U));char _tmpB6='0';if(_get_dyneither_size(_tmpB4,sizeof(char))== 1U  && (_tmpB5 == 0  && _tmpB6 != 0))_throw_arraybounds();*((char*)_tmpB4.curr)=_tmpB6;})){;}else{
if(dotrim){
while((char*)t.curr > (char*)startp.curr  && *((char*)_check_dyneither_subscript(_dyneither_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0U))== '0'){;}
if(*((char*)_check_dyneither_subscript(t,sizeof(char),0U))!= '.')
_dyneither_ptr_inplace_plus(& t,sizeof(char),1);}}
# 956
goto _LL0;default: _LLD: _LLE:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmpEE=({const char*_tmpB7="__cvt_double";_tag_dyneither(_tmpB7,sizeof(char),13U);});_tmpB8->f1=_tmpEE;});_tmpB8;}));}_LL0:;}
# 959
return(t.curr - startp.curr)/ sizeof(char);}
# 963
int Cyc_vfprintf(struct Cyc___cycFILE*f,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 966
int ans;
ans=((int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc,f,fmt,ap);
return ans;}
# 971
int Cyc_fprintf(struct Cyc___cycFILE*f,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 975
return Cyc_vfprintf(f,fmt,ap);}
# 978
int Cyc_vprintf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 981
int ans;
ans=((int(*)(int(*ioputc)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc,Cyc_stdout,fmt,ap);
return ans;}
# 986
int Cyc_printf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 989
int ans;
ans=Cyc_vprintf(fmt,ap);
return ans;}struct _tuple0{struct _dyneither_ptr*f1;unsigned long*f2;};
# 994
static int Cyc_putc_string(int c,struct _tuple0*sptr_n){
struct _tuple0*_tmpB9=sptr_n;struct _dyneither_ptr*_tmpBB;unsigned long*_tmpBA;_LL1: _tmpBB=_tmpB9->f1;_tmpBA=_tmpB9->f2;_LL2:;{
struct _dyneither_ptr s=*_tmpBB;
unsigned long n=*_tmpBA;
if(n == 0)return - 1;
*((char*)_check_dyneither_subscript(s,sizeof(char),0U))=(char)c;
_dyneither_ptr_inplace_plus(_tmpBB,sizeof(char),1);
*_tmpBA=n - 1;
return 1;};}
# 1005
int Cyc_vsnprintf(struct _dyneither_ptr s,unsigned long n,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1008
int ans;
struct _dyneither_ptr _tmpBC=s;
unsigned long _tmpBD=n;
struct _tuple0 _tmpBE=({struct _tuple0 _tmpC3;_tmpC3.f1=& _tmpBC,_tmpC3.f2=& _tmpBD;_tmpC3;});
ans=((int(*)(int(*ioputc)(int,struct _tuple0*),struct _tuple0*ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc_string,& _tmpBE,fmt,ap);
if(0 <= ans)
*((char*)_check_dyneither_subscript(s,sizeof(char),ans))='\000';
return ans;}
# 1018
int Cyc_snprintf(struct _dyneither_ptr s,unsigned long n,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1021
return Cyc_vsnprintf(s,n,fmt,ap);}
# 1024
int Cyc_vsprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1027
return Cyc_vsnprintf(s,_get_dyneither_size(s,sizeof(char)),fmt,ap);}
# 1030
int Cyc_sprintf(struct _dyneither_ptr s,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1033
return Cyc_vsnprintf(s,_get_dyneither_size(s,sizeof(char)),fmt,ap);}
# 1036
static int Cyc_putc_void(int c,int dummy){
return 1;}
# 1040
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*r1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1044
int size=((int(*)(int(*ioputc)(int,int),int ioputc_env,struct _dyneither_ptr fmt0,struct _dyneither_ptr ap))Cyc__IO_vfprintf)(Cyc_putc_void,0,fmt,ap)+ 1;
struct _dyneither_ptr s=(struct _dyneither_ptr)({unsigned int _tmpBF=size + 1;_tag_dyneither(_cyccalloc_atomic(sizeof(char),_tmpBF),sizeof(char),_tmpBF);});
({struct _dyneither_ptr _tmpF0=_dyneither_ptr_decrease_size(s,sizeof(char),1U);struct _dyneither_ptr _tmpEF=fmt;Cyc_vsprintf(_tmpF0,_tmpEF,ap);});
return s;}
# 1050
struct _dyneither_ptr Cyc_rprintf(struct _RegionHandle*r1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1053
return Cyc_vrprintf(r1,fmt,ap);}
# 1056
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 1059
return Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);}
