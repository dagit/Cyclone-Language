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
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);
# 104
int Cyc_fputc(int,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 278 "cycboot.h"
int Cyc_file_string_write(struct Cyc___cycFILE*,struct _dyneither_ptr src,int src_offset,int max_count);struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};
# 482
extern struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct Cyc_Absyn_Stdcall_att_val;
extern struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct Cyc_Absyn_Cdecl_att_val;
extern struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct Cyc_Absyn_Fastcall_att_val;
# 490
extern struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct Cyc_Absyn_Unused_att_val;struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 726 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 946
void*Cyc_Absyn_compress_kb(void*);
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 975
extern void*Cyc_Absyn_bounds_one;
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_times_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1164
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct _tuple10{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;};
# 1170
struct _tuple10 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1178
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 61
struct _dyneither_ptr Cyc_Absynpp_longlong2string(unsigned long long);
# 78
extern struct _dyneither_ptr*Cyc_Absynpp_cyc_stringptr;
int Cyc_Absynpp_exp_prec(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_char_escape(char);
struct _dyneither_ptr Cyc_Absynpp_string_escape(struct _dyneither_ptr);
struct _dyneither_ptr Cyc_Absynpp_prim2str(enum Cyc_Absyn_Primop p);
int Cyc_Absynpp_is_declaration(struct Cyc_Absyn_Stmt*s);
struct _tuple8*Cyc_Absynpp_arg_mk_opt(struct _tuple8*arg);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;struct Cyc_List_List*f3;};
# 86
struct _tuple11 Cyc_Absynpp_to_tms(struct _RegionHandle*,struct Cyc_Absyn_Tqual tq,void*t);struct _tuple12{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 26 "cyclone.h"
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;struct _tuple13{struct _dyneither_ptr f1;unsigned int f2;};
# 31 "absyndump.cyc"
struct _tuple13 Cyc_Lex_xlate_pos(unsigned int);
# 42
static int Cyc_Absyndump_expand_typedefs;
# 46
static int Cyc_Absyndump_qvar_to_Cids;
# 49
static int Cyc_Absyndump_add_cyc_prefix;
# 52
static int Cyc_Absyndump_generate_line_directives;
# 56
static int Cyc_Absyndump_to_VC;
# 58
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs){
Cyc_Absyndump_expand_typedefs=fs->expand_typedefs;
Cyc_Absyndump_qvar_to_Cids=fs->qvar_to_Cids;
Cyc_Absyndump_add_cyc_prefix=fs->add_cyc_prefix;
Cyc_Absyndump_to_VC=fs->to_VC;
Cyc_Absyndump_generate_line_directives=fs->generate_line_directives;
# 73 "absyndump.cyc"
Cyc_Absynpp_set_params(fs);}
# 76
void Cyc_Absyndump_dumptyp(void*);
void Cyc_Absyndump_dumpntyp(void*t);
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumpexp_prec(int,struct Cyc_Absyn_Exp*);
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*);
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*,int expstmt);
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*,unsigned int,int do_semi);
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*);
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a);
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual,void*,void(*f)(void*),void*);
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields);
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields);
# 92
void Cyc_Absyndump_dumploc(unsigned int);
# 95
struct Cyc___cycFILE**Cyc_Absyndump_dump_file=& Cyc_stdout;
# 97
void Cyc_Absyndump_ignore(void*x){return;}
# 99
static unsigned int Cyc_Absyndump_pos=0U;
static char Cyc_Absyndump_prev_char='x';
# 102
int Cyc_Absyndump_need_space_before(){
char _tmp0=Cyc_Absyndump_prev_char;switch(_tmp0){case 123U: _LL1: _LL2:
 goto _LL4;case 125U: _LL3: _LL4:
 goto _LL6;case 40U: _LL5: _LL6:
 goto _LL8;case 41U: _LL7: _LL8:
 goto _LLA;case 91U: _LL9: _LLA:
 goto _LLC;case 93U: _LLB: _LLC:
 goto _LLE;case 59U: _LLD: _LLE:
 goto _LL10;case 44U: _LLF: _LL10:
 goto _LL12;case 61U: _LL11: _LL12:
 goto _LL14;case 63U: _LL13: _LL14:
 goto _LL16;case 33U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 10U: _LL19: _LL1A:
 goto _LL1C;case 42U: _LL1B: _LL1C:
 return 0;default: _LL1D: _LL1E:
 return 1;}_LL0:;}
# 122
void Cyc_Absyndump_dump(struct _dyneither_ptr s){
unsigned int sz=Cyc_strlen((struct _dyneither_ptr)s);
# 129
if(Cyc_Absyndump_need_space_before())
Cyc_fputc((int)' ',*Cyc_Absyndump_dump_file);
if(sz >= 1){
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),(int)(sz - 1)));
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,(int)sz);}}
# 137
void Cyc_Absyndump_dump_nospace(struct _dyneither_ptr s){
int sz=(int)Cyc_strlen((struct _dyneither_ptr)s);
# 140
if(sz >= 1){
Cyc_file_string_write(*Cyc_Absyndump_dump_file,s,0,sz);
Cyc_Absyndump_prev_char=*((const char*)_check_dyneither_subscript(s,sizeof(char),sz - 1));}}
# 145
void Cyc_Absyndump_dump_char(int c){
# 147
Cyc_fputc(c,*Cyc_Absyndump_dump_file);
Cyc_Absyndump_prev_char=(char)c;}static char _tmpE[1U]="";
# 151
void Cyc_Absyndump_dumploc(unsigned int loc){
static struct _dyneither_ptr last_file={_tmpE,_tmpE,_tmpE + 1U};
static unsigned int last_line=0U;
if(loc == 0)return;
if(!Cyc_Absyndump_generate_line_directives)return;{
# 157
struct _tuple13 _tmp1=Cyc_Lex_xlate_pos(loc);struct _tuple13 _tmp2=_tmp1;struct _dyneither_ptr _tmpD;unsigned int _tmpC;_LL1: _tmpD=_tmp2.f1;_tmpC=_tmp2.f2;_LL2:;
if(({char*_tmp300=(char*)_tmpD.curr;_tmp300 == (char*)(_tag_dyneither(0,0,0)).curr;}) || (char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line)return;
if((char*)_tmpD.curr == (char*)last_file.curr  && _tmpC == last_line + 1)
({void*_tmp3=0U;({struct Cyc___cycFILE*_tmp302=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp301=({const char*_tmp4="\n";_tag_dyneither(_tmp4,sizeof(char),2U);});Cyc_fprintf(_tmp302,_tmp301,_tag_dyneither(_tmp3,sizeof(void*),0U));});});else{
if((char*)_tmpD.curr == (char*)last_file.curr)
({struct Cyc_Int_pa_PrintArg_struct _tmp7=({struct Cyc_Int_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=1U,_tmp2F3.f1=(unsigned long)((int)_tmpC);_tmp2F3;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct Cyc___cycFILE*_tmp304=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp303=({const char*_tmp6="\n# %d\n";_tag_dyneither(_tmp6,sizeof(char),7U);});Cyc_fprintf(_tmp304,_tmp303,_tag_dyneither(_tmp5,sizeof(void*),1U));});});else{
# 164
({struct Cyc_Int_pa_PrintArg_struct _tmpA=({struct Cyc_Int_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=1U,_tmp2F5.f1=(unsigned long)((int)_tmpC);_tmp2F5;});struct Cyc_String_pa_PrintArg_struct _tmpB=({struct Cyc_String_pa_PrintArg_struct _tmp2F4;_tmp2F4.tag=0U,_tmp2F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD);_tmp2F4;});void*_tmp8[2U];_tmp8[0]=& _tmpA,_tmp8[1]=& _tmpB;({struct Cyc___cycFILE*_tmp306=*Cyc_Absyndump_dump_file;struct _dyneither_ptr _tmp305=({const char*_tmp9="\n# %d %s\n";_tag_dyneither(_tmp9,sizeof(char),10U);});Cyc_fprintf(_tmp306,_tmp305,_tag_dyneither(_tmp8,sizeof(void*),2U));});});}}
# 166
last_file=_tmpD;
last_line=_tmpC;};}
# 172
void Cyc_Absyndump_dump_str(struct _dyneither_ptr*s){
Cyc_Absyndump_dump(*s);}
# 176
void Cyc_Absyndump_dump_semi(){
Cyc_Absyndump_dump_char((int)';');}
# 180
void Cyc_Absyndump_dump_sep(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 187
f(l->hd);}
# 189
void Cyc_Absyndump_dump_sep_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr sep){
if(l == 0)
return;
for(0;((struct Cyc_List_List*)_check_null(l))->tl != 0;l=l->tl){
f(env,l->hd);
Cyc_Absyndump_dump_nospace(sep);}
# 196
f(env,l->hd);}
# 198
void Cyc_Absyndump_group(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 200
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep(f,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 204
void Cyc_Absyndump_group_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 206
Cyc_Absyndump_dump_nospace(start);
Cyc_Absyndump_dump_sep_c(f,env,l,sep);
Cyc_Absyndump_dump_nospace(end);}
# 210
void Cyc_Absyndump_egroup(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep){
# 212
if(l != 0)
Cyc_Absyndump_group(f,l,start,end,sep);}
# 218
void Cyc_Absyndump_dumpqvar(struct _tuple0*v){
struct Cyc_List_List*_tmpF=0;
struct _dyneither_ptr**prefix=0;
{union Cyc_Absyn_Nmspace _tmp10=(*v).f1;union Cyc_Absyn_Nmspace _tmp11=_tmp10;struct Cyc_List_List*_tmp14;struct Cyc_List_List*_tmp13;struct Cyc_List_List*_tmp12;switch((_tmp11.C_n).tag){case 4U: _LL1: _LL2:
 _tmp12=0;goto _LL4;case 1U: _LL3: _tmp12=(_tmp11.Rel_n).val;_LL4:
# 224
 _tmpF=_tmp12;
goto _LL0;case 3U: _LL5: _tmp13=(_tmp11.C_n).val;_LL6:
# 230
 Cyc_Absyndump_dump_char((int)' ');
goto _LL0;default: _LL7: _tmp14=(_tmp11.Abs_n).val;_LL8:
# 233
 if(Cyc_Absyndump_qvar_to_Cids  || Cyc_Absyndump_add_cyc_prefix)
prefix=& Cyc_Absynpp_cyc_stringptr;
_tmpF=_tmp14;
goto _LL0;}_LL0:;}
# 238
if(prefix != 0){
Cyc_Absyndump_dump_str(*prefix);
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp15="::";_tag_dyneither(_tmp15,sizeof(char),3U);}));}}
# 243
if(_tmpF != 0){
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));
for(_tmpF=_tmpF->tl;_tmpF != 0;_tmpF=_tmpF->tl){
# 248
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_char((int)'_');else{
Cyc_Absyndump_dump_nospace(({const char*_tmp16="::";_tag_dyneither(_tmp16,sizeof(char),3U);}));}
Cyc_Absyndump_dump_nospace(*((struct _dyneither_ptr*)_tmpF->hd));}
# 252
if(Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump_nospace(({const char*_tmp17="_";_tag_dyneither(_tmp17,sizeof(char),2U);}));else{
Cyc_Absyndump_dump_nospace(({const char*_tmp18="::";_tag_dyneither(_tmp18,sizeof(char),3U);}));}
Cyc_Absyndump_dump_nospace(*(*v).f2);}else{
# 256
if(prefix != 0)
Cyc_Absyndump_dump_nospace(*(*v).f2);else{
# 259
Cyc_Absyndump_dump_str((*v).f2);}}}
# 263
void Cyc_Absyndump_dumptq(struct Cyc_Absyn_Tqual tq){
if(tq.q_restrict)Cyc_Absyndump_dump(({const char*_tmp19="restrict";_tag_dyneither(_tmp19,sizeof(char),9U);}));
if(tq.q_volatile)Cyc_Absyndump_dump(({const char*_tmp1A="volatile";_tag_dyneither(_tmp1A,sizeof(char),9U);}));
if(tq.print_const)Cyc_Absyndump_dump(({const char*_tmp1B="const";_tag_dyneither(_tmp1B,sizeof(char),6U);}));}
# 269
void Cyc_Absyndump_dumpscope(enum Cyc_Absyn_Scope sc){
enum Cyc_Absyn_Scope _tmp1C=sc;switch(_tmp1C){case Cyc_Absyn_Static: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp1D="static";_tag_dyneither(_tmp1D,sizeof(char),7U);}));return;case Cyc_Absyn_Public: _LL3: _LL4:
 return;case Cyc_Absyn_Extern: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp1E="extern";_tag_dyneither(_tmp1E,sizeof(char),7U);}));return;case Cyc_Absyn_ExternC: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp1F="extern \"C\"";_tag_dyneither(_tmp1F,sizeof(char),11U);}));return;case Cyc_Absyn_Abstract: _LL9: _LLA:
 Cyc_Absyndump_dump(({const char*_tmp20="abstract";_tag_dyneither(_tmp20,sizeof(char),9U);}));return;default: _LLB: _LLC:
 Cyc_Absyndump_dump(({const char*_tmp21="register";_tag_dyneither(_tmp21,sizeof(char),9U);}));return;}_LL0:;}
# 280
void Cyc_Absyndump_dumpkind(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp22=ka;enum Cyc_Absyn_KindQual _tmp37;enum Cyc_Absyn_AliasQual _tmp36;_LL1: _tmp37=_tmp22->kind;_tmp36=_tmp22->aliasqual;_LL2:;{
enum Cyc_Absyn_KindQual _tmp23=_tmp37;switch(_tmp23){case Cyc_Absyn_AnyKind: _LL4: _LL5: {
# 284
enum Cyc_Absyn_AliasQual _tmp24=_tmp36;switch(_tmp24){case Cyc_Absyn_Aliasable: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp25="A";_tag_dyneither(_tmp25,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL13: _LL14:
 Cyc_Absyndump_dump(({const char*_tmp26="UA";_tag_dyneither(_tmp26,sizeof(char),3U);}));return;default: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp27="TA";_tag_dyneither(_tmp27,sizeof(char),3U);}));return;}_LL10:;}case Cyc_Absyn_MemKind: _LL6: _LL7: {
# 290
enum Cyc_Absyn_AliasQual _tmp28=_tmp36;switch(_tmp28){case Cyc_Absyn_Aliasable: _LL18: _LL19:
 Cyc_Absyndump_dump(({const char*_tmp29="M";_tag_dyneither(_tmp29,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL1A: _LL1B:
 Cyc_Absyndump_dump(({const char*_tmp2A="UM";_tag_dyneither(_tmp2A,sizeof(char),3U);}));return;default: _LL1C: _LL1D:
 Cyc_Absyndump_dump(({const char*_tmp2B="TM";_tag_dyneither(_tmp2B,sizeof(char),3U);}));return;}_LL17:;}case Cyc_Absyn_BoxKind: _LL8: _LL9: {
# 296
enum Cyc_Absyn_AliasQual _tmp2C=_tmp36;switch(_tmp2C){case Cyc_Absyn_Aliasable: _LL1F: _LL20:
 Cyc_Absyndump_dump(({const char*_tmp2D="B";_tag_dyneither(_tmp2D,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL21: _LL22:
 Cyc_Absyndump_dump(({const char*_tmp2E="UB";_tag_dyneither(_tmp2E,sizeof(char),3U);}));return;default: _LL23: _LL24:
 Cyc_Absyndump_dump(({const char*_tmp2F="TB";_tag_dyneither(_tmp2F,sizeof(char),3U);}));return;}_LL1E:;}case Cyc_Absyn_RgnKind: _LLA: _LLB: {
# 302
enum Cyc_Absyn_AliasQual _tmp30=_tmp36;switch(_tmp30){case Cyc_Absyn_Aliasable: _LL26: _LL27:
 Cyc_Absyndump_dump(({const char*_tmp31="R";_tag_dyneither(_tmp31,sizeof(char),2U);}));return;case Cyc_Absyn_Unique: _LL28: _LL29:
 Cyc_Absyndump_dump(({const char*_tmp32="UR";_tag_dyneither(_tmp32,sizeof(char),3U);}));return;default: _LL2A: _LL2B:
 Cyc_Absyndump_dump(({const char*_tmp33="TR";_tag_dyneither(_tmp33,sizeof(char),3U);}));return;}_LL25:;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 307
 Cyc_Absyndump_dump(({const char*_tmp34="E";_tag_dyneither(_tmp34,sizeof(char),2U);}));return;default: _LLE: _LLF:
 Cyc_Absyndump_dump(({const char*_tmp35="I";_tag_dyneither(_tmp35,sizeof(char),2U);}));return;}_LL3:;};}
# 312
void Cyc_Absyndump_dumpaggr_kind(enum Cyc_Absyn_AggrKind k){
enum Cyc_Absyn_AggrKind _tmp38=k;if(_tmp38 == Cyc_Absyn_StructA){_LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp39="struct ";_tag_dyneither(_tmp39,sizeof(char),8U);}));return;}else{_LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp3A="union ";_tag_dyneither(_tmp3A,sizeof(char),7U);}));return;}_LL0:;}
# 319
void Cyc_Absyndump_dumptps(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp309=ts;struct _dyneither_ptr _tmp308=({const char*_tmp3B="<";_tag_dyneither(_tmp3B,sizeof(char),2U);});struct _dyneither_ptr _tmp307=({const char*_tmp3C=">";_tag_dyneither(_tmp3C,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptyp,_tmp309,_tmp308,_tmp307,({const char*_tmp3D=",";_tag_dyneither(_tmp3D,sizeof(char),2U);}));});}
# 322
void Cyc_Absyndump_dumptvar(struct Cyc_Absyn_Tvar*tv){
struct _dyneither_ptr n=*tv->name;
if(*((const char*)_check_dyneither_subscript(n,sizeof(char),0))== '#'){
Cyc_Absyndump_dump(({const char*_tmp3E="`G";_tag_dyneither(_tmp3E,sizeof(char),3U);}));
{void*_tmp3F=Cyc_Absyn_compress_kb(tv->kind);void*_tmp40=_tmp3F;struct Cyc_Absyn_Kind*_tmp43;struct Cyc_Absyn_Kind*_tmp42;switch(*((int*)_tmp40)){case 0U: _LL1: _tmp42=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp40)->f1;_LL2:
 _tmp43=_tmp42;goto _LL4;case 2U: _LL3: _tmp43=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp40)->f2;_LL4:
 Cyc_Absyndump_dumpkind(_tmp43);goto _LL0;default: _LL5: _LL6:
 Cyc_Absyndump_dump_nospace(({const char*_tmp41="K";_tag_dyneither(_tmp41,sizeof(char),2U);}));goto _LL0;}_LL0:;}
# 331
Cyc_Absyndump_dump_nospace(_dyneither_ptr_plus(n,sizeof(char),1));}else{
# 333
Cyc_Absyndump_dump(n);}}
# 335
void Cyc_Absyndump_dumpkindedtvar(struct Cyc_Absyn_Tvar*tv){
Cyc_Absyndump_dumptvar(tv);{
void*_tmp44=Cyc_Absyn_compress_kb(tv->kind);void*_tmp45=_tmp44;struct Cyc_Absyn_Kind*_tmp48;struct Cyc_Absyn_Kind*_tmp47;switch(*((int*)_tmp45)){case 1U: _LL1: _LL2:
 goto _LL0;case 2U: _LL3: _tmp47=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp45)->f2;_LL4:
 goto _LL0;default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->kind == Cyc_Absyn_BoxKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1)->aliasqual == Cyc_Absyn_Aliasable){_LL5: _LL6:
 goto _LL0;}else{goto _LL7;}}else{_LL7: _tmp48=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp45)->f1;_LL8:
 Cyc_Absyndump_dump(({const char*_tmp46="::";_tag_dyneither(_tmp46,sizeof(char),3U);}));Cyc_Absyndump_dumpkind(_tmp48);goto _LL0;}}_LL0:;};}
# 344
void Cyc_Absyndump_dumptvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp30C=tvs;struct _dyneither_ptr _tmp30B=({const char*_tmp49="<";_tag_dyneither(_tmp49,sizeof(char),2U);});struct _dyneither_ptr _tmp30A=({const char*_tmp4A=">";_tag_dyneither(_tmp4A,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp30C,_tmp30B,_tmp30A,({const char*_tmp4B=",";_tag_dyneither(_tmp4B,sizeof(char),2U);}));});}
# 347
void Cyc_Absyndump_dumpkindedtvars(struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp30F=tvs;struct _dyneither_ptr _tmp30E=({const char*_tmp4C="<";_tag_dyneither(_tmp4C,sizeof(char),2U);});struct _dyneither_ptr _tmp30D=({const char*_tmp4D=">";_tag_dyneither(_tmp4D,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp30F,_tmp30E,_tmp30D,({const char*_tmp4E=",";_tag_dyneither(_tmp4E,sizeof(char),2U);}));});}struct _tuple14{struct Cyc_Absyn_Tqual f1;void*f2;};
# 351
void Cyc_Absyndump_dumparg(struct _tuple14*pr){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)((*pr).f1,(*pr).f2,(void(*)(int x))Cyc_Absyndump_ignore,0);}
# 354
void Cyc_Absyndump_dumpargs(struct Cyc_List_List*ts){
({struct Cyc_List_List*_tmp312=ts;struct _dyneither_ptr _tmp311=({const char*_tmp4F="(";_tag_dyneither(_tmp4F,sizeof(char),2U);});struct _dyneither_ptr _tmp310=({const char*_tmp50=")";_tag_dyneither(_tmp50,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple14*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumparg,_tmp312,_tmp311,_tmp310,({const char*_tmp51=",";_tag_dyneither(_tmp51,sizeof(char),2U);}));});}
# 358
void Cyc_Absyndump_dump_callconv(struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp52=(void*)atts->hd;void*_tmp53=_tmp52;switch(*((int*)_tmp53)){case 1U: _LL1: _LL2:
 Cyc_Absyndump_dump(({const char*_tmp54="_stdcall";_tag_dyneither(_tmp54,sizeof(char),9U);}));return;case 2U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp55="_cdecl";_tag_dyneither(_tmp55,sizeof(char),7U);}));return;case 3U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp56="_fastcall";_tag_dyneither(_tmp56,sizeof(char),10U);}));return;default: _LL7: _LL8:
 goto _LL0;}_LL0:;}}
# 368
void Cyc_Absyndump_dump_noncallconv(struct Cyc_List_List*atts){
# 370
int hasatt=0;
{struct Cyc_List_List*atts2=atts;for(0;atts2 != 0;atts2=atts2->tl){
void*_tmp57=(void*)((struct Cyc_List_List*)_check_null(atts))->hd;void*_tmp58=_tmp57;switch(*((int*)_tmp58)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 3U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
 hasatt=1;goto _LL0;}_LL0:;}}
# 378
if(!hasatt)
return;
Cyc_Absyndump_dump(({const char*_tmp59="__declspec(";_tag_dyneither(_tmp59,sizeof(char),12U);}));
for(0;atts != 0;atts=atts->tl){
void*_tmp5A=(void*)atts->hd;void*_tmp5B=_tmp5A;switch(*((int*)_tmp5B)){case 1U: _LLA: _LLB:
 goto _LLD;case 2U: _LLC: _LLD:
 goto _LLF;case 3U: _LLE: _LLF:
 goto _LL9;default: _LL10: _LL11:
 Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));goto _LL9;}_LL9:;}
# 388
Cyc_Absyndump_dump_char((int)')');}
# 391
void Cyc_Absyndump_dumpatts(struct Cyc_List_List*atts){
if(atts == 0)return;{
enum Cyc_Cyclone_C_Compilers _tmp5C=Cyc_Cyclone_c_compiler;if(_tmp5C == Cyc_Cyclone_Gcc_c){_LL1: _LL2:
# 395
 Cyc_Absyndump_dump(({const char*_tmp5D=" __attribute__((";_tag_dyneither(_tmp5D,sizeof(char),17U);}));
for(0;atts != 0;atts=atts->tl){
Cyc_Absyndump_dump(Cyc_Absyn_attribute2string((void*)atts->hd));
if(atts->tl != 0)Cyc_Absyndump_dump(({const char*_tmp5E=",";_tag_dyneither(_tmp5E,sizeof(char),2U);}));}
# 400
Cyc_Absyndump_dump(({const char*_tmp5F=")) ";_tag_dyneither(_tmp5F,sizeof(char),4U);}));
return;}else{_LL3: _LL4:
# 403
 Cyc_Absyndump_dump_noncallconv(atts);
return;}_LL0:;};}
# 408
static void Cyc_Absyndump_dumprgn(void*t){
Cyc_Absyndump_dumpntyp(t);}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 412
static struct _tuple15 Cyc_Absyndump_effects_split(void*t){
struct Cyc_List_List*rgions=0;
struct Cyc_List_List*effects=0;
# 416
{void*_tmp60=Cyc_Tcutil_compress(t);void*_tmp61=_tmp60;struct Cyc_List_List*_tmp69;void*_tmp68;switch(*((int*)_tmp61)){case 23U: _LL1: _tmp68=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp61)->f1;_LL2:
({struct Cyc_List_List*_tmp313=({struct Cyc_List_List*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->hd=_tmp68,_tmp62->tl=rgions;_tmp62;});rgions=_tmp313;});goto _LL0;case 24U: _LL3: _tmp69=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp61)->f1;_LL4:
# 419
 for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
struct _tuple15 _tmp63=Cyc_Absyndump_effects_split((void*)_tmp69->hd);struct _tuple15 _tmp64=_tmp63;struct Cyc_List_List*_tmp66;struct Cyc_List_List*_tmp65;_LL8: _tmp66=_tmp64.f1;_tmp65=_tmp64.f2;_LL9:;
({struct Cyc_List_List*_tmp314=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp66,rgions);rgions=_tmp314;});
({struct Cyc_List_List*_tmp315=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp65,effects);effects=_tmp315;});}
# 424
goto _LL0;default: _LL5: _LL6:
({struct Cyc_List_List*_tmp316=({struct Cyc_List_List*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->hd=t,_tmp67->tl=effects;_tmp67;});effects=_tmp316;});goto _LL0;}_LL0:;}
# 427
return({struct _tuple15 _tmp2F6;_tmp2F6.f1=rgions,_tmp2F6.f2=effects;_tmp2F6;});}
# 430
static void Cyc_Absyndump_dumpeff(void*t){
struct _tuple15 _tmp6A=Cyc_Absyndump_effects_split(t);struct _tuple15 _tmp6B=_tmp6A;struct Cyc_List_List*_tmp6E;struct Cyc_List_List*_tmp6D;_LL1: _tmp6E=_tmp6B.f1;_tmp6D=_tmp6B.f2;_LL2:;
({struct Cyc_List_List*_tmp317=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6E);_tmp6E=_tmp317;});
({struct Cyc_List_List*_tmp318=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp6D);_tmp6D=_tmp318;});
if(_tmp6D != 0){
{struct Cyc_List_List*_tmp6C=_tmp6D;for(0;_tmp6C != 0;_tmp6C=_tmp6C->tl){
Cyc_Absyndump_dumpntyp((void*)_tmp6C->hd);
if(_tmp6C->tl != 0)
Cyc_Absyndump_dump_char((int)'+');}}
# 440
if(_tmp6E != 0)Cyc_Absyndump_dump_char((int)'+');}
# 442
if(_tmp6E != 0  || _tmp6D == 0){
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp6E != 0;_tmp6E=_tmp6E->tl){
Cyc_Absyndump_dumprgn((void*)_tmp6E->hd);
if(_tmp6E->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 448
Cyc_Absyndump_dump_char((int)'}');}}
# 452
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*);
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*);
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*);
# 457
void Cyc_Absyndump_dumpntyp(void*t){
void*_tmp6F=t;struct Cyc_List_List*_tmpD3;void*_tmpD2;void*_tmpD1;struct Cyc_Absyn_Datatypedecl*_tmpD0;struct Cyc_Absyn_Enumdecl*_tmpCF;struct Cyc_Absyn_Aggrdecl*_tmpCE;void*_tmpCD;void*_tmpCC;void*_tmpCB;void*_tmpCA;struct _dyneither_ptr _tmpC9;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct _tuple0*_tmpC6;struct Cyc_List_List*_tmpC5;struct Cyc_List_List*_tmpC4;struct _tuple0*_tmpC3;enum Cyc_Absyn_AggrKind _tmpC2;struct Cyc_List_List*_tmpC1;union Cyc_Absyn_AggrInfoU _tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_List_List*_tmpBE;int _tmpBD;union Cyc_Absyn_DatatypeFieldInfoU _tmpBC;struct Cyc_List_List*_tmpBB;union Cyc_Absyn_DatatypeInfoU _tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_Core_Opt*_tmpB8;void*_tmpB7;int _tmpB6;struct Cyc_Core_Opt*_tmpB5;int _tmpB4;struct Cyc_Absyn_Tvar*_tmpB3;switch(*((int*)_tmp6F)){case 8U: _LL1: _LL2:
# 460
 goto _LL4;case 9U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return;case 0U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp70="void";_tag_dyneither(_tmp70,sizeof(char),5U);}));return;case 2U: _LL9: _tmpB3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6F)->f1;_LLA:
 Cyc_Absyndump_dumptvar(_tmpB3);return;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6F)->f2 == 0){_LLB: _tmpB5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6F)->f1;_tmpB4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6F)->f3;_LLC:
# 466
 Cyc_Absyndump_dump(({const char*_tmp71="`E";_tag_dyneither(_tmp71,sizeof(char),3U);}));
if(_tmpB5 == 0)Cyc_Absyndump_dump(({const char*_tmp72="K";_tag_dyneither(_tmp72,sizeof(char),2U);}));else{Cyc_Absyndump_dumpkind((struct Cyc_Absyn_Kind*)_tmpB5->v);}
Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp75=({struct Cyc_Int_pa_PrintArg_struct _tmp2F7;_tmp2F7.tag=1U,_tmp2F7.f1=(unsigned long)_tmpB4;_tmp2F7;});void*_tmp73[1U];_tmp73[0]=& _tmp75;({struct _dyneither_ptr _tmp319=({const char*_tmp74="%d";_tag_dyneither(_tmp74,sizeof(char),3U);});Cyc_aprintf(_tmp319,_tag_dyneither(_tmp73,sizeof(void*),1U));});}));return;}else{_LLD: _tmpB8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6F)->f1;_tmpB7=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6F)->f2;_tmpB6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6F)->f3;_LLE:
 Cyc_Absyndump_dumpntyp((void*)_check_null(_tmpB7));return;}case 3U: _LLF: _tmpBA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6F)->f1).datatype_info;_tmpB9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp6F)->f1).targs;_LL10:
# 471
{union Cyc_Absyn_DatatypeInfoU _tmp76=_tmpBA;struct _tuple0*_tmp7C;int _tmp7B;struct _tuple0*_tmp7A;int _tmp79;if((_tmp76.UnknownDatatype).tag == 1){_LL5E: _tmp7A=((_tmp76.UnknownDatatype).val).name;_tmp79=((_tmp76.UnknownDatatype).val).is_extensible;_LL5F:
 _tmp7C=_tmp7A;_tmp7B=_tmp79;goto _LL61;}else{_LL60: _tmp7C=(*(_tmp76.KnownDatatype).val)->name;_tmp7B=(*(_tmp76.KnownDatatype).val)->is_extensible;_LL61:
# 474
 if(_tmp7B)Cyc_Absyndump_dump(({const char*_tmp77="@extensible ";_tag_dyneither(_tmp77,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp78="datatype ";_tag_dyneither(_tmp78,sizeof(char),10U);}));
Cyc_Absyndump_dumpqvar(_tmp7C);
Cyc_Absyndump_dumptps(_tmpB9);
goto _LL5D;}_LL5D:;}
# 480
goto _LL0;case 4U: _LL11: _tmpBC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6F)->f1).field_info;_tmpBB=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp6F)->f1).targs;_LL12:
# 482
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7D=_tmpBC;struct _tuple0*_tmp86;int _tmp85;struct _tuple0*_tmp84;struct _tuple0*_tmp83;struct _tuple0*_tmp82;int _tmp81;if((_tmp7D.UnknownDatatypefield).tag == 1){_LL63: _tmp83=((_tmp7D.UnknownDatatypefield).val).datatype_name;_tmp82=((_tmp7D.UnknownDatatypefield).val).field_name;_tmp81=((_tmp7D.UnknownDatatypefield).val).is_extensible;_LL64:
# 484
 _tmp86=_tmp83;_tmp85=_tmp81;_tmp84=_tmp82;goto _LL66;}else{_LL65: _tmp86=(((_tmp7D.KnownDatatypefield).val).f1)->name;_tmp85=(((_tmp7D.KnownDatatypefield).val).f1)->is_extensible;_tmp84=(((_tmp7D.KnownDatatypefield).val).f2)->name;_LL66:
# 487
 if(_tmp85)Cyc_Absyndump_dump(({const char*_tmp7E="@extensible ";_tag_dyneither(_tmp7E,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp7F="datatype ";_tag_dyneither(_tmp7F,sizeof(char),10U);}));
Cyc_Absyndump_dumpqvar(_tmp86);
Cyc_Absyndump_dump(({const char*_tmp80=".";_tag_dyneither(_tmp80,sizeof(char),2U);}));
Cyc_Absyndump_dumpqvar(_tmp84);
Cyc_Absyndump_dumptps(_tmpBB);
goto _LL62;}_LL62:;}
# 495
goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F)->f1){case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F)->f2){case Cyc_Absyn_Int_sz: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 498
 goto _LL1A;case Cyc_Absyn_Char_sz: _LL1B: _LL1C:
# 500
 Cyc_Absyndump_dump(({const char*_tmp89="char";_tag_dyneither(_tmp89,sizeof(char),5U);}));return;case Cyc_Absyn_Short_sz: _LL21: _LL22:
# 503
 goto _LL24;default: _LL2B: _LL2C:
# 508
 goto _LL2E;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F)->f2){case Cyc_Absyn_Int_sz: _LL15: _LL16:
# 497
 Cyc_Absyndump_dump(({const char*_tmp87="int";_tag_dyneither(_tmp87,sizeof(char),4U);}));return;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 499
 Cyc_Absyndump_dump(({const char*_tmp88="long";_tag_dyneither(_tmp88,sizeof(char),5U);}));return;case Cyc_Absyn_Char_sz: _LL1D: _LL1E:
# 501
 Cyc_Absyndump_dump(({const char*_tmp8A="signed char";_tag_dyneither(_tmp8A,sizeof(char),12U);}));return;case Cyc_Absyn_Short_sz: _LL23: _LL24:
# 504
 Cyc_Absyndump_dump(({const char*_tmp8C="short";_tag_dyneither(_tmp8C,sizeof(char),6U);}));return;default: _LL2D: _LL2E: {
# 510
enum Cyc_Cyclone_C_Compilers _tmp90=Cyc_Cyclone_c_compiler;if(_tmp90 == Cyc_Cyclone_Gcc_c){_LL68: _LL69:
 Cyc_Absyndump_dump(({const char*_tmp91="long long";_tag_dyneither(_tmp91,sizeof(char),10U);}));return;}else{_LL6A: _LL6B:
 Cyc_Absyndump_dump(({const char*_tmp92="__int64";_tag_dyneither(_tmp92,sizeof(char),8U);}));return;}_LL67:;}}default: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp6F)->f2){case Cyc_Absyn_Char_sz: _LL1F: _LL20:
# 502
 Cyc_Absyndump_dump(({const char*_tmp8B="unsigned char";_tag_dyneither(_tmp8B,sizeof(char),14U);}));return;case Cyc_Absyn_Short_sz: _LL25: _LL26:
# 505
 Cyc_Absyndump_dump(({const char*_tmp8D="unsigned short";_tag_dyneither(_tmp8D,sizeof(char),15U);}));return;case Cyc_Absyn_Int_sz: _LL27: _LL28:
 Cyc_Absyndump_dump(({const char*_tmp8E="unsigned int";_tag_dyneither(_tmp8E,sizeof(char),13U);}));return;case Cyc_Absyn_Long_sz: _LL29: _LL2A:
 Cyc_Absyndump_dump(({const char*_tmp8F="unsigned long";_tag_dyneither(_tmp8F,sizeof(char),14U);}));return;default: _LL2F: _LL30: {
# 515
enum Cyc_Cyclone_C_Compilers _tmp93=Cyc_Cyclone_c_compiler;if(_tmp93 == Cyc_Cyclone_Vc_c){_LL6D: _LL6E:
 Cyc_Absyndump_dump(({const char*_tmp94="unsigned __int64";_tag_dyneither(_tmp94,sizeof(char),17U);}));return;}else{_LL6F: _LL70:
 Cyc_Absyndump_dump(({const char*_tmp95="unsigned long long";_tag_dyneither(_tmp95,sizeof(char),19U);}));return;}_LL6C:;}}}case 7U: _LL31: _tmpBD=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp6F)->f1;_LL32:
# 520
 if(_tmpBD == 0)
Cyc_Absyndump_dump(({const char*_tmp96="float";_tag_dyneither(_tmp96,sizeof(char),6U);}));else{
if(_tmpBD == 1)
Cyc_Absyndump_dump(({const char*_tmp97="double";_tag_dyneither(_tmp97,sizeof(char),7U);}));else{
# 525
Cyc_Absyndump_dump(({const char*_tmp98="long double";_tag_dyneither(_tmp98,sizeof(char),12U);}));}}
return;case 10U: _LL33: _tmpBE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6F)->f1;_LL34:
 Cyc_Absyndump_dump_char((int)'$');Cyc_Absyndump_dumpargs(_tmpBE);return;case 11U: _LL35: _tmpC0=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6F)->f1).aggr_info;_tmpBF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp6F)->f1).targs;_LL36: {
# 529
struct _tuple10 _tmp99=Cyc_Absyn_aggr_kinded_name(_tmpC0);struct _tuple10 _tmp9A=_tmp99;enum Cyc_Absyn_AggrKind _tmp9C;struct _tuple0*_tmp9B;_LL72: _tmp9C=_tmp9A.f1;_tmp9B=_tmp9A.f2;_LL73:;
Cyc_Absyndump_dumpaggr_kind(_tmp9C);Cyc_Absyndump_dumpqvar(_tmp9B);Cyc_Absyndump_dumptps(_tmpBF);
return;}case 12U: _LL37: _tmpC2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6F)->f1;_tmpC1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6F)->f2;_LL38:
# 533
 Cyc_Absyndump_dumpaggr_kind(_tmpC2);Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpaggrfields(_tmpC1);Cyc_Absyndump_dump_char((int)'}');return;case 13U: _LL39: _tmpC3=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp6F)->f1;_LL3A:
 Cyc_Absyndump_dump(({const char*_tmp9D="enum ";_tag_dyneither(_tmp9D,sizeof(char),6U);}));Cyc_Absyndump_dumpqvar(_tmpC3);return;case 14U: _LL3B: _tmpC4=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp6F)->f1;_LL3C:
 Cyc_Absyndump_dump(({const char*_tmp9E="enum {";_tag_dyneither(_tmp9E,sizeof(char),7U);}));Cyc_Absyndump_dumpenumfields(_tmpC4);Cyc_Absyndump_dump(({const char*_tmp9F="}";_tag_dyneither(_tmp9F,sizeof(char),2U);}));return;case 17U: _LL3D: _tmpC6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6F)->f1;_tmpC5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6F)->f2;_LL3E:
 Cyc_Absyndump_dumpqvar(_tmpC6),Cyc_Absyndump_dumptps(_tmpC5);return;case 18U: _LL3F: _tmpC7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6F)->f1;_LL40:
 Cyc_Absyndump_dump(({const char*_tmpA0="valueof_t(";_tag_dyneither(_tmpA0,sizeof(char),11U);}));Cyc_Absyndump_dumpexp(_tmpC7);Cyc_Absyndump_dump(({const char*_tmpA1=")";_tag_dyneither(_tmpA1,sizeof(char),2U);}));return;case 27U: _LL41: _tmpC8=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6F)->f1;_LL42:
 Cyc_Absyndump_dump(({const char*_tmpA2="typeof(";_tag_dyneither(_tmpA2,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmpC8);Cyc_Absyndump_dump(({const char*_tmpA3=")";_tag_dyneither(_tmpA3,sizeof(char),2U);}));return;case 28U: _LL43: _tmpC9=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp6F)->f1;_LL44:
 Cyc_Absyndump_dump(_tmpC9);return;case 15U: _LL45: _tmpCA=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp6F)->f1;_LL46:
# 541
 Cyc_Absyndump_dump(({const char*_tmpA4="region_t<";_tag_dyneither(_tmpA4,sizeof(char),10U);}));Cyc_Absyndump_dumprgn(_tmpCA);Cyc_Absyndump_dump(({const char*_tmpA5=">";_tag_dyneither(_tmpA5,sizeof(char),2U);}));return;case 16U: _LL47: _tmpCC=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6F)->f1;_tmpCB=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp6F)->f2;_LL48:
 Cyc_Absyndump_dump(({const char*_tmpA6="dynregion_t<";_tag_dyneither(_tmpA6,sizeof(char),13U);}));Cyc_Absyndump_dumprgn(_tmpCC);
Cyc_Absyndump_dump(({const char*_tmpA7=",";_tag_dyneither(_tmpA7,sizeof(char),2U);}));Cyc_Absyndump_dumprgn(_tmpCB);Cyc_Absyndump_dump(({const char*_tmpA8=">";_tag_dyneither(_tmpA8,sizeof(char),2U);}));return;case 19U: _LL49: _tmpCD=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6F)->f1;_LL4A:
 Cyc_Absyndump_dump(({const char*_tmpA9="tag_t<";_tag_dyneither(_tmpA9,sizeof(char),7U);}));Cyc_Absyndump_dumpntyp(_tmpCD);Cyc_Absyndump_dump(({const char*_tmpAA=">";_tag_dyneither(_tmpAA,sizeof(char),2U);}));return;case 21U: _LL4B: _LL4C:
 Cyc_Absyndump_dump(({const char*_tmpAB="`U";_tag_dyneither(_tmpAB,sizeof(char),3U);}));goto _LL0;case 22U: _LL4D: _LL4E:
 Cyc_Absyndump_dump(({const char*_tmpAC="`RC";_tag_dyneither(_tmpAC,sizeof(char),4U);}));goto _LL0;case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)){case 0U: _LL4F: _tmpCE=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)->f1;_LL50:
# 548
 Cyc_Absyndump_dump_aggrdecl(_tmpCE);return;case 1U: _LL51: _tmpCF=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)->f1;_LL52:
# 550
 Cyc_Absyndump_dump_enumdecl(_tmpCF);return;default: _LL53: _tmpD0=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)->f1;_LL54:
# 552
 Cyc_Absyndump_dump_datatypedecl(_tmpD0);return;}case 20U: _LL55: _LL56:
 Cyc_Absyndump_dump(({const char*_tmpAD="`H";_tag_dyneither(_tmpAD,sizeof(char),3U);}));goto _LL0;case 23U: _LL57: _tmpD1=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp6F)->f1;_LL58:
 Cyc_Absyndump_dump(({const char*_tmpAE="{";_tag_dyneither(_tmpAE,sizeof(char),2U);}));Cyc_Absyndump_dumptyp(_tmpD1);Cyc_Absyndump_dump(({const char*_tmpAF="}";_tag_dyneither(_tmpAF,sizeof(char),2U);}));goto _LL0;case 25U: _LL59: _tmpD2=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp6F)->f1;_LL5A:
 Cyc_Absyndump_dump(({const char*_tmpB0="regions(";_tag_dyneither(_tmpB0,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmpD2);Cyc_Absyndump_dump(({const char*_tmpB1=")";_tag_dyneither(_tmpB1,sizeof(char),2U);}));goto _LL0;default: _LL5B: _tmpD3=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp6F)->f1;_LL5C:
# 557
 for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
Cyc_Absyndump_dumptyp((void*)_tmpD3->hd);
if(_tmpD3->tl != 0)Cyc_Absyndump_dump(({const char*_tmpB2="+";_tag_dyneither(_tmpB2,sizeof(char),2U);}));}}_LL0:;}
# 564
void Cyc_Absyndump_dumpvaropt(struct _dyneither_ptr*vo){
if(vo != 0)Cyc_Absyndump_dump_str(vo);}
# 567
void Cyc_Absyndump_dumpfunarg(struct _tuple8*t){
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)((*t).f2,(*t).f3,Cyc_Absyndump_dumpvaropt,(*t).f1);}struct _tuple16{void*f1;void*f2;};
# 570
void Cyc_Absyndump_dump_rgncmp(struct _tuple16*cmp){
struct _tuple16*_tmpD4=cmp;void*_tmpD6;void*_tmpD5;_LL1: _tmpD6=_tmpD4->f1;_tmpD5=_tmpD4->f2;_LL2:;
Cyc_Absyndump_dumpeff(_tmpD6);Cyc_Absyndump_dump_char((int)'>');Cyc_Absyndump_dumprgn(_tmpD5);}
# 574
void Cyc_Absyndump_dump_rgnpo(struct Cyc_List_List*rgn_po){
({struct Cyc_List_List*_tmp31A=rgn_po;((void(*)(void(*f)(struct _tuple16*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dump_rgncmp,_tmp31A,({const char*_tmpD7=",";_tag_dyneither(_tmpD7,sizeof(char),2U);}));});}
# 577
void Cyc_Absyndump_dumpfunargs(struct Cyc_List_List*args,int c_varargs,struct Cyc_Absyn_VarargInfo*cyc_varargs,void*effopt,struct Cyc_List_List*rgn_po,struct Cyc_Absyn_Exp*req,struct Cyc_Absyn_Exp*ens){
# 581
Cyc_Absyndump_dump_char((int)'(');
for(0;args != 0;args=args->tl){
Cyc_Absyndump_dumpfunarg((struct _tuple8*)args->hd);
if((args->tl != 0  || c_varargs) || cyc_varargs != 0)Cyc_Absyndump_dump_char((int)',');}
# 586
if(c_varargs)
Cyc_Absyndump_dump(({const char*_tmpD8="...";_tag_dyneither(_tmpD8,sizeof(char),4U);}));else{
if(cyc_varargs != 0){
struct _tuple8*_tmpD9=({struct _tuple8*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->f1=cyc_varargs->name,_tmpDC->f2=cyc_varargs->tq,_tmpDC->f3=cyc_varargs->type;_tmpDC;});
Cyc_Absyndump_dump(({const char*_tmpDA="...";_tag_dyneither(_tmpDA,sizeof(char),4U);}));
if(cyc_varargs->inject)Cyc_Absyndump_dump(({const char*_tmpDB=" inject ";_tag_dyneither(_tmpDB,sizeof(char),9U);}));
Cyc_Absyndump_dumpfunarg(_tmpD9);}}
# 594
if(effopt != 0){
Cyc_Absyndump_dump_semi();
Cyc_Absyndump_dumpeff(effopt);}
# 598
if(rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(rgn_po);}
# 602
Cyc_Absyndump_dump_char((int)')');
if(req != 0){
Cyc_Absyndump_dump(({const char*_tmpDD=" @requires(";_tag_dyneither(_tmpDD,sizeof(char),12U);}));
Cyc_Absyndump_dumpexp(req);
Cyc_Absyndump_dump_char((int)')');}
# 608
if(ens != 0){
Cyc_Absyndump_dump(({const char*_tmpDE=" @ensures(";_tag_dyneither(_tmpDE,sizeof(char),11U);}));
Cyc_Absyndump_dumpexp(ens);
Cyc_Absyndump_dump_char((int)')');}}
# 615
void Cyc_Absyndump_dumptyp(void*t){
({struct Cyc_Absyn_Tqual _tmp31B=Cyc_Absyn_empty_tqual(0U);((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(int),int))Cyc_Absyndump_dumptqtd)(_tmp31B,t,(void(*)(int x))Cyc_Absyndump_ignore,0);});}
# 619
void Cyc_Absyndump_dumpdesignator(void*d){
void*_tmpDF=d;struct _dyneither_ptr*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDF)->tag == 0U){_LL1: _tmpE1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDF)->f1;_LL2:
 Cyc_Absyndump_dump(({const char*_tmpE0=".[";_tag_dyneither(_tmpE0,sizeof(char),3U);}));Cyc_Absyndump_dumpexp(_tmpE1);Cyc_Absyndump_dump_char((int)']');goto _LL0;}else{_LL3: _tmpE2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpDF)->f1;_LL4:
 Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmpE2);goto _LL0;}_LL0:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 626
void Cyc_Absyndump_dumpde(struct _tuple17*de){
({struct Cyc_List_List*_tmp31E=(*de).f1;struct _dyneither_ptr _tmp31D=({const char*_tmpE3="";_tag_dyneither(_tmpE3,sizeof(char),1U);});struct _dyneither_ptr _tmp31C=({const char*_tmpE4="=";_tag_dyneither(_tmpE4,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp31E,_tmp31D,_tmp31C,({const char*_tmpE5="=";_tag_dyneither(_tmpE5,sizeof(char),2U);}));});
Cyc_Absyndump_dumpexp((*de).f2);}
# 631
void Cyc_Absyndump_dumpexps_prec(int inprec,struct Cyc_List_List*es){
({int _tmp322=inprec;struct Cyc_List_List*_tmp321=es;struct _dyneither_ptr _tmp320=({const char*_tmpE6="";_tag_dyneither(_tmpE6,sizeof(char),1U);});struct _dyneither_ptr _tmp31F=({const char*_tmpE7="";_tag_dyneither(_tmpE7,sizeof(char),1U);});((void(*)(void(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group_c)(Cyc_Absyndump_dumpexp_prec,_tmp322,_tmp321,_tmp320,_tmp31F,({const char*_tmpE8=",";_tag_dyneither(_tmpE8,sizeof(char),2U);}));});}
# 635
void Cyc_Absyndump_dumpexp_prec(int inprec,struct Cyc_Absyn_Exp*e){
int myprec=Cyc_Absynpp_exp_prec(e);
if(inprec >= myprec)
Cyc_Absyndump_dump_nospace(({const char*_tmpE9="(";_tag_dyneither(_tmpE9,sizeof(char),2U);}));
{void*_tmpEA=e->r;void*_tmpEB=_tmpEA;struct Cyc_Absyn_Stmt*_tmp195;struct Cyc_Core_Opt*_tmp194;struct Cyc_List_List*_tmp193;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp191;int _tmp190;struct Cyc_Absyn_Exp*_tmp18F;void**_tmp18E;struct Cyc_Absyn_Exp*_tmp18D;int _tmp18C;struct Cyc_Absyn_Enumfield*_tmp18B;struct Cyc_Absyn_Enumfield*_tmp18A;struct Cyc_List_List*_tmp189;struct Cyc_Absyn_Datatypefield*_tmp188;struct Cyc_List_List*_tmp187;struct _tuple0*_tmp186;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp184;struct Cyc_Absyn_Exp*_tmp183;void*_tmp182;struct Cyc_Absyn_Vardecl*_tmp181;struct Cyc_Absyn_Exp*_tmp180;struct Cyc_Absyn_Exp*_tmp17F;struct Cyc_List_List*_tmp17E;struct _tuple8*_tmp17D;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp17B;struct Cyc_Absyn_Exp*_tmp17A;struct Cyc_Absyn_Exp*_tmp179;struct Cyc_Absyn_Exp*_tmp178;struct _dyneither_ptr*_tmp177;struct Cyc_Absyn_Exp*_tmp176;struct _dyneither_ptr*_tmp175;struct Cyc_Absyn_Exp*_tmp174;void*_tmp173;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_Exp*_tmp171;struct _dyneither_ptr*_tmp170;int _tmp16F;struct _dyneither_ptr _tmp16E;void*_tmp16D;struct Cyc_Absyn_Exp*_tmp16C;struct Cyc_Absyn_Exp*_tmp16B;void*_tmp16A;struct Cyc_Absyn_Exp*_tmp169;struct Cyc_Absyn_Exp*_tmp168;struct Cyc_Absyn_Exp*_tmp167;void*_tmp166;struct Cyc_Absyn_Exp*_tmp165;struct Cyc_Absyn_Exp*_tmp164;struct Cyc_Absyn_Exp*_tmp163;struct Cyc_Absyn_Exp*_tmp162;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_List_List*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_Exp*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_Exp*_tmp156;struct Cyc_Absyn_Exp*_tmp155;struct Cyc_Absyn_Exp*_tmp154;struct Cyc_Absyn_Exp*_tmp153;struct Cyc_Absyn_Exp*_tmp152;struct Cyc_Core_Opt*_tmp151;struct Cyc_Absyn_Exp*_tmp150;enum Cyc_Absyn_Primop _tmp14F;struct Cyc_List_List*_tmp14E;struct _dyneither_ptr _tmp14D;void*_tmp14C;struct _dyneither_ptr _tmp14B;struct _dyneither_ptr _tmp14A;struct _dyneither_ptr _tmp149;enum Cyc_Absyn_Sign _tmp148;long long _tmp147;int _tmp146;int _tmp145;int _tmp144;enum Cyc_Absyn_Sign _tmp143;short _tmp142;struct _dyneither_ptr _tmp141;enum Cyc_Absyn_Sign _tmp140;char _tmp13F;switch(*((int*)_tmpEB)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).String_c).tag){case 2U: _LL1: _tmp140=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Char_c).val).f1;_tmp13F=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Char_c).val).f2;_LL2:
# 641
 Cyc_Absyndump_dump_char((int)'\'');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp13F));Cyc_Absyndump_dump_char((int)'\'');
goto _LL0;case 3U: _LL3: _tmp141=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Wchar_c).val;_LL4:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpEE=({struct Cyc_String_pa_PrintArg_struct _tmp2F8;_tmp2F8.tag=0U,_tmp2F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp141);_tmp2F8;});void*_tmpEC[1U];_tmpEC[0]=& _tmpEE;({struct _dyneither_ptr _tmp323=({const char*_tmpED="L'%s'";_tag_dyneither(_tmpED,sizeof(char),6U);});Cyc_aprintf(_tmp323,_tag_dyneither(_tmpEC,sizeof(void*),1U));});}));goto _LL0;case 4U: _LL5: _tmp143=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Short_c).val).f1;_tmp142=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Short_c).val).f2;_LL6:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF1=({struct Cyc_Int_pa_PrintArg_struct _tmp2F9;_tmp2F9.tag=1U,_tmp2F9.f1=(unsigned long)((int)_tmp142);_tmp2F9;});void*_tmpEF[1U];_tmpEF[0]=& _tmpF1;({struct _dyneither_ptr _tmp324=({const char*_tmpF0="%d";_tag_dyneither(_tmpF0,sizeof(char),3U);});Cyc_aprintf(_tmp324,_tag_dyneither(_tmpEF,sizeof(void*),1U));});}));goto _LL0;case 5U: switch((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Int_c).val).f1){case Cyc_Absyn_None: _LL7: _tmp144=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Int_c).val).f2;_LL8:
 _tmp145=_tmp144;goto _LLA;case Cyc_Absyn_Signed: _LL9: _tmp145=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Int_c).val).f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF4=({struct Cyc_Int_pa_PrintArg_struct _tmp2FA;_tmp2FA.tag=1U,_tmp2FA.f1=(unsigned long)_tmp145;_tmp2FA;});void*_tmpF2[1U];_tmpF2[0]=& _tmpF4;({struct _dyneither_ptr _tmp325=({const char*_tmpF3="%d";_tag_dyneither(_tmpF3,sizeof(char),3U);});Cyc_aprintf(_tmp325,_tag_dyneither(_tmpF2,sizeof(void*),1U));});}));goto _LL0;default: _LLB: _tmp146=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Int_c).val).f2;_LLC:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpF7=({struct Cyc_Int_pa_PrintArg_struct _tmp2FB;_tmp2FB.tag=1U,_tmp2FB.f1=(unsigned int)_tmp146;_tmp2FB;});void*_tmpF5[1U];_tmpF5[0]=& _tmpF7;({struct _dyneither_ptr _tmp326=({const char*_tmpF6="%uU";_tag_dyneither(_tmpF6,sizeof(char),4U);});Cyc_aprintf(_tmp326,_tag_dyneither(_tmpF5,sizeof(void*),1U));});}));goto _LL0;}case 6U: _LLD: _tmp148=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).LongLong_c).val).f1;_tmp147=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).LongLong_c).val).f2;_LLE:
# 650
 Cyc_Absyndump_dump(Cyc_Absynpp_longlong2string((unsigned long long)_tmp147));goto _LL0;case 7U: _LLF: _tmp149=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Float_c).val).f1;_LL10:
 Cyc_Absyndump_dump(_tmp149);goto _LL0;case 1U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmpF8="NULL";_tag_dyneither(_tmpF8,sizeof(char),5U);}));goto _LL0;case 8U: _LL13: _tmp14A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).String_c).val;_LL14:
# 654
 Cyc_Absyndump_dump_char((int)'"');Cyc_Absyndump_dump_nospace(Cyc_Absynpp_string_escape(_tmp14A));Cyc_Absyndump_dump_char((int)'"');
goto _LL0;default: _LL15: _tmp14B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).Wstring_c).val;_LL16:
# 657
 Cyc_Absyndump_dump(({const char*_tmpF9="L\"";_tag_dyneither(_tmpF9,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(_tmp14B);Cyc_Absyndump_dump_char((int)'"');
goto _LL0;}case 1U: _LL17: _tmp14C=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL18:
# 660
 Cyc_Absyndump_dumpqvar(Cyc_Absyn_binding2qvar(_tmp14C));goto _LL0;case 2U: _LL19: _tmp14D=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL1A:
# 662
 Cyc_Absyndump_dump(({const char*_tmpFA="__cyclone_pragma__(";_tag_dyneither(_tmpFA,sizeof(char),20U);}));Cyc_Absyndump_dump_nospace(_tmp14D);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 3U: _LL1B: _tmp14F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp14E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL1C: {
# 665
struct _dyneither_ptr _tmpFB=Cyc_Absynpp_prim2str(_tmp14F);
{int _tmpFC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp14E);int _tmpFD=_tmpFC;switch(_tmpFD){case 1U: _LL70: _LL71:
# 668
 if(_tmp14F == Cyc_Absyn_Numelts){
Cyc_Absyndump_dump(({const char*_tmpFE="numelts(";_tag_dyneither(_tmpFE,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E))->hd);
Cyc_Absyndump_dump(({const char*_tmpFF=")";_tag_dyneither(_tmpFF,sizeof(char),2U);}));}else{
# 673
Cyc_Absyndump_dump(_tmpFB);
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E))->hd);}
# 676
goto _LL6F;case 2U: _LL72: _LL73:
# 678
 Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E))->hd);
Cyc_Absyndump_dump(_tmpFB);
Cyc_Absyndump_dump_char((int)' ');
Cyc_Absyndump_dumpexp_prec(myprec,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp14E->tl))->hd);
goto _LL6F;default: _LL74: _LL75:
# 684
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp327=({const char*_tmp100="Absyndump -- Bad number of arguments to primop";_tag_dyneither(_tmp100,sizeof(char),47U);});_tmp101->f1=_tmp327;});_tmp101;}));}_LL6F:;}
# 686
goto _LL0;}case 4U: _LL1D: _tmp152=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp151=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_tmp150=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpEB)->f3;_LL1E:
# 689
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp152);
if(_tmp151 != 0)
Cyc_Absyndump_dump(Cyc_Absynpp_prim2str((enum Cyc_Absyn_Primop)_tmp151->v));
Cyc_Absyndump_dump_nospace(({const char*_tmp102="=";_tag_dyneither(_tmp102,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp150);
goto _LL0;case 5U: switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEB)->f2){case Cyc_Absyn_PreInc: _LL1F: _tmp153=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL20:
# 696
 Cyc_Absyndump_dump(({const char*_tmp103="++";_tag_dyneither(_tmp103,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp153);goto _LL0;case Cyc_Absyn_PreDec: _LL21: _tmp154=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL22:
 Cyc_Absyndump_dump(({const char*_tmp104="--";_tag_dyneither(_tmp104,sizeof(char),3U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp154);goto _LL0;case Cyc_Absyn_PostInc: _LL23: _tmp155=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL24:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp155);Cyc_Absyndump_dump(({const char*_tmp105="++";_tag_dyneither(_tmp105,sizeof(char),3U);}));goto _LL0;default: _LL25: _tmp156=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL26:
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp156);Cyc_Absyndump_dump(({const char*_tmp106="--";_tag_dyneither(_tmp106,sizeof(char),3U);}));goto _LL0;}case 6U: _LL27: _tmp159=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp158=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_tmp157=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpEB)->f3;_LL28:
# 702
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp159);
Cyc_Absyndump_dump_char((int)'?');Cyc_Absyndump_dumpexp_prec(0,_tmp158);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp_prec(myprec,_tmp157);
goto _LL0;case 7U: _LL29: _tmp15B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp15A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL2A:
# 709
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15B);Cyc_Absyndump_dump(({const char*_tmp107=" && ";_tag_dyneither(_tmp107,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15A);goto _LL0;case 8U: _LL2B: _tmp15D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp15C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL2C:
# 713
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp15D);Cyc_Absyndump_dump(({const char*_tmp108=" || ";_tag_dyneither(_tmp108,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp15C);goto _LL0;case 9U: _LL2D: _tmp15F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp15E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL2E:
# 719
 Cyc_Absyndump_dumploc(e->loc);
# 722
Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp15F);Cyc_Absyndump_dump_char((int)',');Cyc_Absyndump_dumpexp_prec(myprec - 1,_tmp15E);
goto _LL0;case 10U: _LL2F: _tmp161=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp160=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL30:
# 726
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dumpexp_prec(myprec,_tmp161);
Cyc_Absyndump_dump_nospace(({const char*_tmp109="(";_tag_dyneither(_tmp109,sizeof(char),2U);}));{
# 732
int old_generate_line_directives=Cyc_Absyndump_generate_line_directives;
Cyc_Absyndump_generate_line_directives=
(old_generate_line_directives  && !(e->loc == 0)) && !(_tmp161->loc == 0);
Cyc_Absyndump_dumpexps_prec(20,_tmp160);
Cyc_Absyndump_dump_nospace(({const char*_tmp10A=")";_tag_dyneither(_tmp10A,sizeof(char),2U);}));
Cyc_Absyndump_generate_line_directives=old_generate_line_directives;
goto _LL0;};case 11U: _LL31: _tmp162=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL32:
# 741
 Cyc_Absyndump_dumploc(e->loc);
Cyc_Absyndump_dump(({const char*_tmp10B="throw";_tag_dyneither(_tmp10B,sizeof(char),6U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp162);goto _LL0;case 12U: _LL33: _tmp163=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL34:
# 744
 _tmp164=_tmp163;goto _LL36;case 13U: _LL35: _tmp164=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL36:
 Cyc_Absyndump_dumpexp_prec(inprec,_tmp164);goto _LL0;case 14U: _LL37: _tmp166=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp165=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL38:
# 748
 Cyc_Absyndump_dump_char((int)'(');Cyc_Absyndump_dumptyp(_tmp166);Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpexp_prec(myprec,_tmp165);
goto _LL0;case 15U: _LL39: _tmp167=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL3A:
# 752
 Cyc_Absyndump_dump_char((int)'&');Cyc_Absyndump_dumpexp_prec(myprec,_tmp167);goto _LL0;case 16U: _LL3B: _tmp169=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp168=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL3C:
# 755
 Cyc_Absyndump_dump(({const char*_tmp10C="new ";_tag_dyneither(_tmp10C,sizeof(char),5U);}));Cyc_Absyndump_dumpexp_prec(myprec,_tmp168);goto _LL0;case 17U: _LL3D: _tmp16A=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL3E:
# 758
 Cyc_Absyndump_dump(({const char*_tmp10D="sizeof(";_tag_dyneither(_tmp10D,sizeof(char),8U);}));Cyc_Absyndump_dumptyp(_tmp16A);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 18U: _LL3F: _tmp16B=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL40:
# 761
 Cyc_Absyndump_dump(({const char*_tmp10E="sizeof(";_tag_dyneither(_tmp10E,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp16B);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 41U: _LL41: _tmp16C=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL42:
# 764
 Cyc_Absyndump_dump(({const char*_tmp10F="__extension__(";_tag_dyneither(_tmp10F,sizeof(char),15U);}));Cyc_Absyndump_dumpexp(_tmp16C);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 39U: _LL43: _tmp16D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL44:
# 767
 Cyc_Absyndump_dump(({const char*_tmp110="valueof(";_tag_dyneither(_tmp110,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp16D);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 40U: _LL45: _tmp16F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp16E=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL46:
# 770
 Cyc_Absyndump_dump(({const char*_tmp111="__asm__";_tag_dyneither(_tmp111,sizeof(char),8U);}));
if(_tmp16F)Cyc_Absyndump_dump(({const char*_tmp112=" volatile ";_tag_dyneither(_tmp112,sizeof(char),11U);}));
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_nospace(_tmp16E);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 38U: _LL47: _tmp171=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp170=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL48:
# 778
 Cyc_Absyndump_dump(({const char*_tmp113="tagcheck(";_tag_dyneither(_tmp113,sizeof(char),10U);}));Cyc_Absyndump_dumpexp(_tmp171);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp170);
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 19U: _LL49: _tmp173=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp172=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL4A:
# 783
 Cyc_Absyndump_dump(({const char*_tmp114="offsetof(";_tag_dyneither(_tmp114,sizeof(char),10U);}));Cyc_Absyndump_dumptyp(_tmp173);Cyc_Absyndump_dump_char((int)',');
for(0;_tmp172 != 0;_tmp172=_tmp172->tl){
{void*_tmp115=(void*)_tmp172->hd;void*_tmp116=_tmp115;unsigned int _tmp11B;struct _dyneither_ptr*_tmp11A;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp116)->tag == 0U){_LL77: _tmp11A=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp116)->f1;_LL78:
# 787
 Cyc_Absyndump_dump_nospace(*_tmp11A);
goto _LL76;}else{_LL79: _tmp11B=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp116)->f1;_LL7A:
# 790
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp119=({struct Cyc_Int_pa_PrintArg_struct _tmp2FC;_tmp2FC.tag=1U,_tmp2FC.f1=(unsigned long)((int)_tmp11B);_tmp2FC;});void*_tmp117[1U];_tmp117[0]=& _tmp119;({struct _dyneither_ptr _tmp328=({const char*_tmp118="%d";_tag_dyneither(_tmp118,sizeof(char),3U);});Cyc_aprintf(_tmp328,_tag_dyneither(_tmp117,sizeof(void*),1U));});}));
goto _LL76;}_LL76:;}
# 793
if(_tmp172->tl != 0)Cyc_Absyndump_dump_char((int)'.');}
# 795
Cyc_Absyndump_dump_char((int)')');
goto _LL0;case 20U: _LL4B: _tmp174=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL4C:
# 799
 Cyc_Absyndump_dump_char((int)'*');Cyc_Absyndump_dumpexp_prec(myprec,_tmp174);goto _LL0;case 21U: _LL4D: _tmp176=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp175=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL4E:
# 802
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp176);Cyc_Absyndump_dump_char((int)'.');Cyc_Absyndump_dump_nospace(*_tmp175);
goto _LL0;case 22U: _LL4F: _tmp178=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp177=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL50:
# 806
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp178);Cyc_Absyndump_dump_nospace(({const char*_tmp11C="->";_tag_dyneither(_tmp11C,sizeof(char),3U);}));Cyc_Absyndump_dump_nospace(*_tmp177);
goto _LL0;case 23U: _LL51: _tmp17A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp179=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL52:
# 810
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp17A);
Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp179);Cyc_Absyndump_dump_char((int)']');goto _LL0;case 24U: _LL53: _tmp17B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL54:
# 814
 Cyc_Absyndump_dump(({const char*_tmp11D="$(";_tag_dyneither(_tmp11D,sizeof(char),3U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp17B);Cyc_Absyndump_dump_char((int)')');goto _LL0;case 25U: _LL55: _tmp17D=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp17C=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL56:
# 817
 Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptyp((*_tmp17D).f3);
Cyc_Absyndump_dump_char((int)')');
({struct Cyc_List_List*_tmp32B=_tmp17C;struct _dyneither_ptr _tmp32A=({const char*_tmp11E="{";_tag_dyneither(_tmp11E,sizeof(char),2U);});struct _dyneither_ptr _tmp329=({const char*_tmp11F="}";_tag_dyneither(_tmp11F,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp32B,_tmp32A,_tmp329,({const char*_tmp120=",";_tag_dyneither(_tmp120,sizeof(char),2U);}));});
goto _LL0;case 26U: _LL57: _tmp17E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL58:
# 824
({struct Cyc_List_List*_tmp32E=_tmp17E;struct _dyneither_ptr _tmp32D=({const char*_tmp121="{";_tag_dyneither(_tmp121,sizeof(char),2U);});struct _dyneither_ptr _tmp32C=({const char*_tmp122="}";_tag_dyneither(_tmp122,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp32E,_tmp32D,_tmp32C,({const char*_tmp123=",";_tag_dyneither(_tmp123,sizeof(char),2U);}));});
goto _LL0;case 27U: _LL59: _tmp181=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp180=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_tmp17F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpEB)->f3;_LL5A:
# 828
 Cyc_Absyndump_dump(({const char*_tmp124="{for";_tag_dyneither(_tmp124,sizeof(char),5U);}));Cyc_Absyndump_dump_str((*_tmp181->name).f2);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp180);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpexp(_tmp17F);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 28U: _LL5B: _tmp183=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp182=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL5C:
# 833
 Cyc_Absyndump_dump(({const char*_tmp125="{for x ";_tag_dyneither(_tmp125,sizeof(char),8U);}));Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumpexp(_tmp183);
Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumptyp(_tmp182);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 29U: _LL5D: _tmp186=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp185=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_tmp184=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpEB)->f3;_LL5E:
# 838
 Cyc_Absyndump_dumpqvar(_tmp186);
Cyc_Absyndump_dump_char((int)'{');
if(_tmp185 != 0)
Cyc_Absyndump_dumptps(_tmp185);
({struct Cyc_List_List*_tmp331=_tmp184;struct _dyneither_ptr _tmp330=({const char*_tmp126="";_tag_dyneither(_tmp126,sizeof(char),1U);});struct _dyneither_ptr _tmp32F=({const char*_tmp127="}";_tag_dyneither(_tmp127,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp331,_tmp330,_tmp32F,({const char*_tmp128=",";_tag_dyneither(_tmp128,sizeof(char),2U);}));});
goto _LL0;case 30U: _LL5F: _tmp187=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL60:
# 846
({struct Cyc_List_List*_tmp334=_tmp187;struct _dyneither_ptr _tmp333=({const char*_tmp129="{";_tag_dyneither(_tmp129,sizeof(char),2U);});struct _dyneither_ptr _tmp332=({const char*_tmp12A="}";_tag_dyneither(_tmp12A,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp334,_tmp333,_tmp332,({const char*_tmp12B=",";_tag_dyneither(_tmp12B,sizeof(char),2U);}));});
goto _LL0;case 31U: _LL61: _tmp189=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp188=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpEB)->f3;_LL62:
# 850
 Cyc_Absyndump_dumpqvar(_tmp188->name);
if(_tmp189 != 0)({struct Cyc_List_List*_tmp337=_tmp189;struct _dyneither_ptr _tmp336=({const char*_tmp12C="(";_tag_dyneither(_tmp12C,sizeof(char),2U);});struct _dyneither_ptr _tmp335=({const char*_tmp12D=")";_tag_dyneither(_tmp12D,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpexp,_tmp337,_tmp336,_tmp335,({const char*_tmp12E=",";_tag_dyneither(_tmp12E,sizeof(char),2U);}));});
goto _LL0;case 32U: _LL63: _tmp18A=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL64:
# 854
 Cyc_Absyndump_dumpqvar(_tmp18A->name);goto _LL0;case 33U: _LL65: _tmp18B=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL66:
 Cyc_Absyndump_dumpqvar(_tmp18B->name);goto _LL0;case 34U: _LL67: _tmp190=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).is_calloc;_tmp18F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).rgn;_tmp18E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).elt_type;_tmp18D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).num_elts;_tmp18C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpEB)->f1).inline_call;_LL68:
# 858
 Cyc_Absyndump_dumploc(_tmp18D->loc);
if(_tmp190){
if(_tmp18F != 0){
Cyc_Absyndump_dump(({const char*_tmp12F="rcalloc(";_tag_dyneither(_tmp12F,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(_tmp18F);Cyc_Absyndump_dump(({const char*_tmp130=",";_tag_dyneither(_tmp130,sizeof(char),2U);}));}else{
# 864
Cyc_Absyndump_dump(({const char*_tmp131="calloc";_tag_dyneither(_tmp131,sizeof(char),7U);}));}
# 866
Cyc_Absyndump_dumpexp(_tmp18D);
Cyc_Absyndump_dump(({const char*_tmp132=",";_tag_dyneither(_tmp132,sizeof(char),2U);}));
Cyc_Absyndump_dumpexp(Cyc_Absyn_sizeoftyp_exp(*((void**)_check_null(_tmp18E)),0U));
Cyc_Absyndump_dump(({const char*_tmp133=")";_tag_dyneither(_tmp133,sizeof(char),2U);}));}else{
# 871
if(_tmp18F != 0){
if(_tmp18C)
Cyc_Absyndump_dump(({const char*_tmp134="rmalloc_inline(";_tag_dyneither(_tmp134,sizeof(char),16U);}));else{
# 875
Cyc_Absyndump_dump(({const char*_tmp135="rmalloc(";_tag_dyneither(_tmp135,sizeof(char),9U);}));}
Cyc_Absyndump_dumpexp(_tmp18F);Cyc_Absyndump_dump(({const char*_tmp136=",";_tag_dyneither(_tmp136,sizeof(char),2U);}));}else{
# 878
Cyc_Absyndump_dump(({const char*_tmp137="malloc(";_tag_dyneither(_tmp137,sizeof(char),8U);}));}
# 881
if(_tmp18E != 0)
Cyc_Absyndump_dumpexp(({struct Cyc_Absyn_Exp*_tmp338=Cyc_Absyn_sizeoftyp_exp(*_tmp18E,0U);Cyc_Absyn_times_exp(_tmp338,_tmp18D,0U);}));else{
# 884
Cyc_Absyndump_dumpexp(_tmp18D);}
Cyc_Absyndump_dump(({const char*_tmp138=")";_tag_dyneither(_tmp138,sizeof(char),2U);}));}
# 887
goto _LL0;case 35U: _LL69: _tmp192=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp191=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL6A:
# 890
 Cyc_Absyndump_dumpexp_prec(myprec,_tmp192);
Cyc_Absyndump_dump_nospace(({const char*_tmp139=":=:";_tag_dyneither(_tmp139,sizeof(char),4U);}));
Cyc_Absyndump_dumpexp_prec(myprec,_tmp191);
goto _LL0;case 36U: _LL6B: _tmp194=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_tmp193=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpEB)->f2;_LL6C:
# 897
({struct Cyc_List_List*_tmp33B=_tmp193;struct _dyneither_ptr _tmp33A=({const char*_tmp13A="{";_tag_dyneither(_tmp13A,sizeof(char),2U);});struct _dyneither_ptr _tmp339=({const char*_tmp13B="}";_tag_dyneither(_tmp13B,sizeof(char),2U);});((void(*)(void(*f)(struct _tuple17*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpde,_tmp33B,_tmp33A,_tmp339,({const char*_tmp13C=",";_tag_dyneither(_tmp13C,sizeof(char),2U);}));});
goto _LL0;default: _LL6D: _tmp195=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpEB)->f1;_LL6E:
# 901
 Cyc_Absyndump_dump_nospace(({const char*_tmp13D="({";_tag_dyneither(_tmp13D,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(_tmp195,1);
Cyc_Absyndump_dump_nospace(({const char*_tmp13E="})";_tag_dyneither(_tmp13E,sizeof(char),3U);}));
goto _LL0;}_LL0:;}
# 906
if(inprec >= myprec)
Cyc_Absyndump_dump_char((int)')');}
# 910
void Cyc_Absyndump_dumpexp(struct Cyc_Absyn_Exp*e){
Cyc_Absyndump_dumpexp_prec(0,e);}
# 914
void Cyc_Absyndump_dumpswitchclauses(struct Cyc_List_List*scs){
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp196=(struct Cyc_Absyn_Switch_clause*)scs->hd;
if(_tmp196->where_clause == 0  && (_tmp196->pattern)->r == (void*)& Cyc_Absyn_Wild_p_val)
Cyc_Absyndump_dump(({const char*_tmp197="default:";_tag_dyneither(_tmp197,sizeof(char),9U);}));else{
# 920
Cyc_Absyndump_dump(({const char*_tmp198="case";_tag_dyneither(_tmp198,sizeof(char),5U);}));
Cyc_Absyndump_dumppat(_tmp196->pattern);
if(_tmp196->where_clause != 0){
Cyc_Absyndump_dump(({const char*_tmp199="&&";_tag_dyneither(_tmp199,sizeof(char),3U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp196->where_clause));}
# 926
Cyc_Absyndump_dump_nospace(({const char*_tmp19A=":";_tag_dyneither(_tmp19A,sizeof(char),2U);}));}
# 928
Cyc_Absyndump_dumpstmt(_tmp196->body,0);}}
# 932
void Cyc_Absyndump_dumpstmt(struct Cyc_Absyn_Stmt*s,int expstmt){
Cyc_Absyndump_dumploc(s->loc);{
void*_tmp19B=s->r;void*_tmp19C=_tmp19B;struct Cyc_Absyn_Stmt*_tmp1D2;struct Cyc_List_List*_tmp1D1;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_Stmt*_tmp1CF;struct Cyc_Absyn_Exp*_tmp1CE;struct _dyneither_ptr*_tmp1CD;struct Cyc_Absyn_Stmt*_tmp1CC;struct Cyc_Absyn_Decl*_tmp1CB;struct Cyc_Absyn_Stmt*_tmp1CA;struct Cyc_Absyn_Exp*_tmp1C9;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_Absyn_Stmt*_tmp1C4;struct _dyneither_ptr*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C2;struct Cyc_Absyn_Stmt*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Stmt*_tmp1BF;struct Cyc_Absyn_Stmt*_tmp1BE;struct Cyc_Absyn_Exp*_tmp1BD;struct Cyc_Absyn_Stmt*_tmp1BC;struct Cyc_Absyn_Stmt*_tmp1BB;struct Cyc_Absyn_Exp*_tmp1BA;switch(*((int*)_tmp19C)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_semi();goto _LL0;case 1U: _LL3: _tmp1BA=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_LL4:
 Cyc_Absyndump_dumpexp_prec(- 100,_tmp1BA);Cyc_Absyndump_dump_semi();goto _LL0;case 2U: _LL5: _tmp1BC=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1BB=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2;_LL6:
# 941
 if(Cyc_Absynpp_is_declaration(_tmp1BC)){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BC,0);Cyc_Absyndump_dump_char((int)'}');}else{
# 944
Cyc_Absyndump_dumpstmt(_tmp1BC,0);}
if(Cyc_Absynpp_is_declaration(_tmp1BB)){
if(expstmt)Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpstmt(_tmp1BB,expstmt);
Cyc_Absyndump_dump_char((int)'}');
if(expstmt)Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dump_semi();}else{
# 951
Cyc_Absyndump_dumpstmt(_tmp1BB,expstmt);}
goto _LL0;case 3U: if(((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1 == 0){_LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp19D="return;";_tag_dyneither(_tmp19D,sizeof(char),8U);}));goto _LL0;}else{_LL9: _tmp1BD=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_LLA:
 Cyc_Absyndump_dump(({const char*_tmp19E="return";_tag_dyneither(_tmp19E,sizeof(char),7U);}));Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(_tmp1BD));Cyc_Absyndump_dump_semi();goto _LL0;}case 4U: _LLB: _tmp1C0=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1BF=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2;_tmp1BE=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp19C)->f3;_LLC:
# 956
 Cyc_Absyndump_dump(({const char*_tmp19F="if(";_tag_dyneither(_tmp19F,sizeof(char),4U);}));Cyc_Absyndump_dumpexp(_tmp1C0);
{void*_tmp1A0=_tmp1BF->r;void*_tmp1A1=_tmp1A0;switch(*((int*)_tmp1A1)){case 2U: _LL26: _LL27:
 goto _LL29;case 12U: _LL28: _LL29:
 goto _LL2B;case 4U: _LL2A: _LL2B:
 goto _LL2D;case 13U: _LL2C: _LL2D:
 Cyc_Absyndump_dump_nospace(({const char*_tmp1A2="){";_tag_dyneither(_tmp1A2,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1BF,0);Cyc_Absyndump_dump_char((int)'}');goto _LL25;default: _LL2E: _LL2F:
 Cyc_Absyndump_dump_char((int)')');Cyc_Absyndump_dumpstmt(_tmp1BF,0);}_LL25:;}
# 964
{void*_tmp1A3=_tmp1BE->r;void*_tmp1A4=_tmp1A3;if(((struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*)_tmp1A4)->tag == 0U){_LL31: _LL32:
 goto _LL30;}else{_LL33: _LL34:
 Cyc_Absyndump_dump(({const char*_tmp1A5="else{";_tag_dyneither(_tmp1A5,sizeof(char),6U);}));Cyc_Absyndump_dumpstmt(_tmp1BE,0);Cyc_Absyndump_dump_char((int)'}');goto _LL30;}_LL30:;}
# 968
goto _LL0;case 5U: _LLD: _tmp1C2=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1).f1;_tmp1C1=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2;_LLE:
# 970
 Cyc_Absyndump_dump(({const char*_tmp1A6="while(";_tag_dyneither(_tmp1A6,sizeof(char),7U);}));Cyc_Absyndump_dumpexp(_tmp1C2);Cyc_Absyndump_dump_nospace(({const char*_tmp1A7="){";_tag_dyneither(_tmp1A7,sizeof(char),3U);}));
Cyc_Absyndump_dumpstmt(_tmp1C1,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 6U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp1A8="break;";_tag_dyneither(_tmp1A8,sizeof(char),7U);}));goto _LL0;case 7U: _LL11: _LL12:
 Cyc_Absyndump_dump(({const char*_tmp1A9="continue;";_tag_dyneither(_tmp1A9,sizeof(char),10U);}));goto _LL0;case 8U: _LL13: _tmp1C3=((struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_LL14:
 Cyc_Absyndump_dump(({const char*_tmp1AA="goto";_tag_dyneither(_tmp1AA,sizeof(char),5U);}));Cyc_Absyndump_dump_str(_tmp1C3);Cyc_Absyndump_dump_semi();goto _LL0;case 9U: _LL15: _tmp1C7=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1C6=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2).f1;_tmp1C5=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19C)->f3).f1;_tmp1C4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp19C)->f4;_LL16:
# 979
 Cyc_Absyndump_dump(({const char*_tmp1AB="for(";_tag_dyneither(_tmp1AB,sizeof(char),5U);}));Cyc_Absyndump_dumpexp(_tmp1C7);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C6);Cyc_Absyndump_dump_semi();Cyc_Absyndump_dumpexp(_tmp1C5);
Cyc_Absyndump_dump_nospace(({const char*_tmp1AC="){";_tag_dyneither(_tmp1AC,sizeof(char),3U);}));Cyc_Absyndump_dumpstmt(_tmp1C4,0);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 10U: _LL17: _tmp1C9=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1C8=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2;_LL18:
# 983
 Cyc_Absyndump_dump(({const char*_tmp1AD="switch(";_tag_dyneither(_tmp1AD,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1C9);Cyc_Absyndump_dump_nospace(({const char*_tmp1AE="){";_tag_dyneither(_tmp1AE,sizeof(char),3U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1C8);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 12U: _LL19: _tmp1CB=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1CA=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2;_LL1A:
# 988
 Cyc_Absyndump_dumpdecl(_tmp1CB);Cyc_Absyndump_dumpstmt(_tmp1CA,expstmt);goto _LL0;case 13U: _LL1B: _tmp1CD=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1CC=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2;_LL1C:
# 994
 if(Cyc_Absynpp_is_declaration(_tmp1CC)){
Cyc_Absyndump_dump_str(_tmp1CD);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1AF=": ({";_tag_dyneither(_tmp1AF,sizeof(char),5U);}));else{Cyc_Absyndump_dump_nospace(({const char*_tmp1B0=": {";_tag_dyneither(_tmp1B0,sizeof(char),4U);}));}
Cyc_Absyndump_dumpstmt(_tmp1CC,expstmt);
if(expstmt)Cyc_Absyndump_dump_nospace(({const char*_tmp1B1="});}";_tag_dyneither(_tmp1B1,sizeof(char),5U);}));else{Cyc_Absyndump_dump_char((int)'}');}}else{
# 1000
Cyc_Absyndump_dump_str(_tmp1CD);Cyc_Absyndump_dump_char((int)':');Cyc_Absyndump_dumpstmt(_tmp1CC,expstmt);}
# 1002
goto _LL0;case 14U: _LL1D: _tmp1CF=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1CE=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2).f1;_LL1E:
# 1005
 Cyc_Absyndump_dump(({const char*_tmp1B2="do{";_tag_dyneither(_tmp1B2,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1CF,0);
Cyc_Absyndump_dump_nospace(({const char*_tmp1B3="}while(";_tag_dyneither(_tmp1B3,sizeof(char),8U);}));Cyc_Absyndump_dumpexp(_tmp1CE);Cyc_Absyndump_dump_nospace(({const char*_tmp1B4=");";_tag_dyneither(_tmp1B4,sizeof(char),3U);}));
goto _LL0;case 11U: if(((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1 == 0){_LL1F: _LL20:
# 1009
 Cyc_Absyndump_dump(({const char*_tmp1B5="fallthru;";_tag_dyneither(_tmp1B5,sizeof(char),10U);}));goto _LL0;}else{_LL21: _tmp1D0=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_LL22:
# 1011
 Cyc_Absyndump_dump(({const char*_tmp1B6="fallthru(";_tag_dyneither(_tmp1B6,sizeof(char),10U);}));Cyc_Absyndump_dumpexps_prec(20,_tmp1D0);Cyc_Absyndump_dump_nospace(({const char*_tmp1B7=");";_tag_dyneither(_tmp1B7,sizeof(char),3U);}));goto _LL0;}default: _LL23: _tmp1D2=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19C)->f1;_tmp1D1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp19C)->f2;_LL24:
# 1014
 Cyc_Absyndump_dump(({const char*_tmp1B8="try";_tag_dyneither(_tmp1B8,sizeof(char),4U);}));Cyc_Absyndump_dumpstmt(_tmp1D2,0);
Cyc_Absyndump_dump(({const char*_tmp1B9="catch{";_tag_dyneither(_tmp1B9,sizeof(char),7U);}));
Cyc_Absyndump_dumpswitchclauses(_tmp1D1);Cyc_Absyndump_dump_char((int)'}');
goto _LL0;}_LL0:;};}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 1022
void Cyc_Absyndump_dumpdp(struct _tuple18*dp){
({struct Cyc_List_List*_tmp33E=(*dp).f1;struct _dyneither_ptr _tmp33D=({const char*_tmp1D3="";_tag_dyneither(_tmp1D3,sizeof(char),1U);});struct _dyneither_ptr _tmp33C=({const char*_tmp1D4="=";_tag_dyneither(_tmp1D4,sizeof(char),2U);});((void(*)(void(*f)(void*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpdesignator,_tmp33E,_tmp33D,_tmp33C,({const char*_tmp1D5="=";_tag_dyneither(_tmp1D5,sizeof(char),2U);}));});
Cyc_Absyndump_dumppat((*dp).f2);}
# 1027
void Cyc_Absyndump_dumppat(struct Cyc_Absyn_Pat*p){
void*_tmp1D6=p->r;void*_tmp1D7=_tmp1D6;struct Cyc_Absyn_Exp*_tmp228;struct Cyc_Absyn_Enumfield*_tmp227;struct Cyc_Absyn_Enumfield*_tmp226;struct Cyc_Absyn_Datatypefield*_tmp225;struct Cyc_List_List*_tmp224;int _tmp223;struct Cyc_List_List*_tmp222;struct Cyc_List_List*_tmp221;int _tmp220;union Cyc_Absyn_AggrInfoU _tmp21F;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp21D;int _tmp21C;struct _tuple0*_tmp21B;struct Cyc_List_List*_tmp21A;int _tmp219;struct _tuple0*_tmp218;struct Cyc_Absyn_Tvar*_tmp217;struct Cyc_Absyn_Vardecl*_tmp216;struct Cyc_Absyn_Vardecl*_tmp215;struct Cyc_Absyn_Pat*_tmp214;struct Cyc_Absyn_Vardecl*_tmp213;struct Cyc_Absyn_Pat*_tmp212;struct Cyc_List_List*_tmp211;int _tmp210;struct Cyc_Absyn_Tvar*_tmp20F;struct Cyc_Absyn_Vardecl*_tmp20E;struct Cyc_Absyn_Vardecl*_tmp20D;struct Cyc_Absyn_Pat*_tmp20C;struct Cyc_Absyn_Vardecl*_tmp20B;struct _dyneither_ptr _tmp20A;char _tmp209;int _tmp208;int _tmp207;int _tmp206;switch(*((int*)_tmp1D7)){case 0U: _LL1: _LL2:
 Cyc_Absyndump_dump_char((int)'_');goto _LL0;case 9U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp1D8="NULL";_tag_dyneither(_tmp1D8,sizeof(char),5U);}));goto _LL0;case 10U: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1){case Cyc_Absyn_None: _LL5: _tmp206=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL6:
 _tmp207=_tmp206;goto _LL8;case Cyc_Absyn_Signed: _LL7: _tmp207=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL8:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1DB=({struct Cyc_Int_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=1U,_tmp2FD.f1=(unsigned long)_tmp207;_tmp2FD;});void*_tmp1D9[1U];_tmp1D9[0]=& _tmp1DB;({struct _dyneither_ptr _tmp33F=({const char*_tmp1DA="%d";_tag_dyneither(_tmp1DA,sizeof(char),3U);});Cyc_aprintf(_tmp33F,_tag_dyneither(_tmp1D9,sizeof(void*),1U));});}));goto _LL0;default: _LL9: _tmp208=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LLA:
 Cyc_Absyndump_dump((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1DE=({struct Cyc_Int_pa_PrintArg_struct _tmp2FE;_tmp2FE.tag=1U,_tmp2FE.f1=(unsigned int)_tmp208;_tmp2FE;});void*_tmp1DC[1U];_tmp1DC[0]=& _tmp1DE;({struct _dyneither_ptr _tmp340=({const char*_tmp1DD="%u";_tag_dyneither(_tmp1DD,sizeof(char),3U);});Cyc_aprintf(_tmp340,_tag_dyneither(_tmp1DC,sizeof(void*),1U));});}));goto _LL0;}case 11U: _LLB: _tmp209=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_LLC:
# 1035
 Cyc_Absyndump_dump(({const char*_tmp1DF="'";_tag_dyneither(_tmp1DF,sizeof(char),2U);}));Cyc_Absyndump_dump_nospace(Cyc_Absynpp_char_escape(_tmp209));Cyc_Absyndump_dump_nospace(({const char*_tmp1E0="'";_tag_dyneither(_tmp1E0,sizeof(char),2U);}));goto _LL0;case 12U: _LLD: _tmp20A=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_LLE:
 Cyc_Absyndump_dump(_tmp20A);goto _LL0;case 1U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2)->r)->tag == 0U){_LLF: _tmp20B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_LL10:
 Cyc_Absyndump_dumpqvar(_tmp20B->name);goto _LL0;}else{_LL11: _tmp20D=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_tmp20C=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL12:
 Cyc_Absyndump_dumpqvar(_tmp20D->name);Cyc_Absyndump_dump(({const char*_tmp1E1=" as ";_tag_dyneither(_tmp1E1,sizeof(char),5U);}));Cyc_Absyndump_dumppat(_tmp20C);goto _LL0;}case 2U: _LL13: _tmp20F=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_tmp20E=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL14:
# 1040
 Cyc_Absyndump_dump(({const char*_tmp1E2="alias";_tag_dyneither(_tmp1E2,sizeof(char),6U);}));
Cyc_Absyndump_dump(({const char*_tmp1E3="<";_tag_dyneither(_tmp1E3,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp20F);Cyc_Absyndump_dump(({const char*_tmp1E4=">";_tag_dyneither(_tmp1E4,sizeof(char),2U);}));
Cyc_Absyndump_dumpvardecl(_tmp20E,p->loc,0);
goto _LL0;case 5U: _LL15: _tmp211=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_tmp210=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL16: {
# 1045
struct _dyneither_ptr term=_tmp210?({const char*_tmp1E7=", ...)";_tag_dyneither(_tmp1E7,sizeof(char),7U);}):({const char*_tmp1E8=")";_tag_dyneither(_tmp1E8,sizeof(char),2U);});
({struct Cyc_List_List*_tmp343=_tmp211;struct _dyneither_ptr _tmp342=({const char*_tmp1E5="$(";_tag_dyneither(_tmp1E5,sizeof(char),3U);});struct _dyneither_ptr _tmp341=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp343,_tmp342,_tmp341,({const char*_tmp1E6=",";_tag_dyneither(_tmp1E6,sizeof(char),2U);}));});goto _LL0;}case 6U: _LL17: _tmp212=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_LL18:
 Cyc_Absyndump_dump(({const char*_tmp1E9="&";_tag_dyneither(_tmp1E9,sizeof(char),2U);}));Cyc_Absyndump_dumppat(_tmp212);goto _LL0;case 3U: if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2)->r)->tag == 0U){_LL19: _tmp213=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_LL1A:
# 1049
 Cyc_Absyndump_dump(({const char*_tmp1EA="*";_tag_dyneither(_tmp1EA,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(_tmp213->name);goto _LL0;}else{_LL1B: _tmp215=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_tmp214=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL1C:
# 1051
 Cyc_Absyndump_dump(({const char*_tmp1EB="*";_tag_dyneither(_tmp1EB,sizeof(char),2U);}));Cyc_Absyndump_dumpqvar(_tmp215->name);Cyc_Absyndump_dump(({const char*_tmp1EC=" as ";_tag_dyneither(_tmp1EC,sizeof(char),5U);}));Cyc_Absyndump_dumppat(_tmp214);goto _LL0;}case 4U: _LL1D: _tmp217=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_tmp216=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL1E:
# 1053
 Cyc_Absyndump_dumpqvar(_tmp216->name);Cyc_Absyndump_dump_char((int)'<');Cyc_Absyndump_dumptvar(_tmp217);Cyc_Absyndump_dump_char((int)'>');goto _LL0;case 15U: _LL1F: _tmp218=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_LL20:
 Cyc_Absyndump_dumpqvar(_tmp218);goto _LL0;case 16U: _LL21: _tmp21B=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_tmp21A=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_tmp219=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1D7)->f3;_LL22: {
# 1056
struct _dyneither_ptr term=_tmp219?({const char*_tmp1EF=", ...)";_tag_dyneither(_tmp1EF,sizeof(char),7U);}):({const char*_tmp1F0=")";_tag_dyneither(_tmp1F0,sizeof(char),2U);});
Cyc_Absyndump_dumpqvar(_tmp21B);({struct Cyc_List_List*_tmp346=_tmp21A;struct _dyneither_ptr _tmp345=({const char*_tmp1ED="(";_tag_dyneither(_tmp1ED,sizeof(char),2U);});struct _dyneither_ptr _tmp344=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp346,_tmp345,_tmp344,({const char*_tmp1EE=",";_tag_dyneither(_tmp1EE,sizeof(char),2U);}));});goto _LL0;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1 != 0){_LL23: _tmp21F=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1)->aggr_info;_tmp21E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_tmp21D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f3;_tmp21C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f4;_LL24: {
# 1059
struct _tuple10 _tmp1F1=Cyc_Absyn_aggr_kinded_name(_tmp21F);struct _tuple10 _tmp1F2=_tmp1F1;struct _tuple0*_tmp1FA;_LL30: _tmp1FA=_tmp1F2.f2;_LL31:;{
struct _dyneither_ptr term=_tmp21C?({const char*_tmp1F8=", ...)";_tag_dyneither(_tmp1F8,sizeof(char),7U);}):({const char*_tmp1F9=")";_tag_dyneither(_tmp1F9,sizeof(char),2U);});
Cyc_Absyndump_dumpqvar(_tmp1FA);Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp349=_tmp21E;struct _dyneither_ptr _tmp348=({const char*_tmp1F3="[";_tag_dyneither(_tmp1F3,sizeof(char),2U);});struct _dyneither_ptr _tmp347=({const char*_tmp1F4="]";_tag_dyneither(_tmp1F4,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp349,_tmp348,_tmp347,({const char*_tmp1F5=",";_tag_dyneither(_tmp1F5,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp34C=_tmp21D;struct _dyneither_ptr _tmp34B=({const char*_tmp1F6="";_tag_dyneither(_tmp1F6,sizeof(char),1U);});struct _dyneither_ptr _tmp34A=term;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp34C,_tmp34B,_tmp34A,({const char*_tmp1F7=",";_tag_dyneither(_tmp1F7,sizeof(char),2U);}));});
goto _LL0;};}}else{_LL25: _tmp222=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_tmp221=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f3;_tmp220=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1D7)->f4;_LL26: {
# 1066
struct _dyneither_ptr term=_tmp220?({const char*_tmp200=", ...)";_tag_dyneither(_tmp200,sizeof(char),7U);}):({const char*_tmp201=")";_tag_dyneither(_tmp201,sizeof(char),2U);});
Cyc_Absyndump_dump_char((int)'{');
({struct Cyc_List_List*_tmp34F=_tmp222;struct _dyneither_ptr _tmp34E=({const char*_tmp1FB="[";_tag_dyneither(_tmp1FB,sizeof(char),2U);});struct _dyneither_ptr _tmp34D=({const char*_tmp1FC="]";_tag_dyneither(_tmp1FC,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumptvar,_tmp34F,_tmp34E,_tmp34D,({const char*_tmp1FD=",";_tag_dyneither(_tmp1FD,sizeof(char),2U);}));});
({struct Cyc_List_List*_tmp352=_tmp221;struct _dyneither_ptr _tmp351=({const char*_tmp1FE="";_tag_dyneither(_tmp1FE,sizeof(char),1U);});struct _dyneither_ptr _tmp350=term;((void(*)(void(*f)(struct _tuple18*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumpdp,_tmp352,_tmp351,_tmp350,({const char*_tmp1FF=",";_tag_dyneither(_tmp1FF,sizeof(char),2U);}));});
goto _LL0;}}case 8U: _LL27: _tmp225=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_tmp224=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D7)->f3;_tmp223=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1D7)->f4;_LL28: {
# 1072
struct _dyneither_ptr term=_tmp223?({const char*_tmp204=", ...)";_tag_dyneither(_tmp204,sizeof(char),7U);}):({const char*_tmp205=")";_tag_dyneither(_tmp205,sizeof(char),2U);});
Cyc_Absyndump_dumpqvar(_tmp225->name);
if(_tmp224 != 0)({struct Cyc_List_List*_tmp355=_tmp224;struct _dyneither_ptr _tmp354=({const char*_tmp202="(";_tag_dyneither(_tmp202,sizeof(char),2U);});struct _dyneither_ptr _tmp353=term;((void(*)(void(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dumppat,_tmp355,_tmp354,_tmp353,({const char*_tmp203=",";_tag_dyneither(_tmp203,sizeof(char),2U);}));});
goto _LL0;}case 13U: _LL29: _tmp226=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL2A:
 _tmp227=_tmp226;goto _LL2C;case 14U: _LL2B: _tmp227=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1D7)->f2;_LL2C:
 Cyc_Absyndump_dumpqvar(_tmp227->name);goto _LL0;default: _LL2D: _tmp228=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1D7)->f1;_LL2E:
 Cyc_Absyndump_dumpexp(_tmp228);goto _LL0;}_LL0:;}
# 1082
void Cyc_Absyndump_dumpdatatypefield(struct Cyc_Absyn_Datatypefield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->typs != 0)
Cyc_Absyndump_dumpargs(ef->typs);}
# 1087
void Cyc_Absyndump_dumpdatatypefields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp356=fields;((void(*)(void(*f)(struct Cyc_Absyn_Datatypefield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpdatatypefield,_tmp356,({const char*_tmp229=",";_tag_dyneither(_tmp229,sizeof(char),2U);}));});}
# 1090
void Cyc_Absyndump_dumpenumfield(struct Cyc_Absyn_Enumfield*ef){
Cyc_Absyndump_dumpqvar(ef->name);
if(ef->tag != 0){
Cyc_Absyndump_dump(({const char*_tmp22A=" = ";_tag_dyneither(_tmp22A,sizeof(char),4U);}));
Cyc_Absyndump_dumpexp((struct Cyc_Absyn_Exp*)_check_null(ef->tag));}}
# 1097
void Cyc_Absyndump_dumpenumfields(struct Cyc_List_List*fields){
({struct Cyc_List_List*_tmp357=fields;((void(*)(void(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l,struct _dyneither_ptr sep))Cyc_Absyndump_dump_sep)(Cyc_Absyndump_dumpenumfield,_tmp357,({const char*_tmp22B=",";_tag_dyneither(_tmp22B,sizeof(char),2U);}));});}
# 1101
void Cyc_Absyndump_dumpaggrfields(struct Cyc_List_List*fields){
for(0;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Aggrfield*_tmp22C=(struct Cyc_Absyn_Aggrfield*)fields->hd;struct Cyc_Absyn_Aggrfield*_tmp22D=_tmp22C;struct _dyneither_ptr*_tmp235;struct Cyc_Absyn_Tqual _tmp234;void*_tmp233;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_List_List*_tmp231;struct Cyc_Absyn_Exp*_tmp230;_LL1: _tmp235=_tmp22D->name;_tmp234=_tmp22D->tq;_tmp233=_tmp22D->type;_tmp232=_tmp22D->width;_tmp231=_tmp22D->attributes;_tmp230=_tmp22D->requires_clause;_LL2:;
# 1105
{enum Cyc_Cyclone_C_Compilers _tmp22E=Cyc_Cyclone_c_compiler;if(_tmp22E == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1107
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp234,_tmp233,Cyc_Absyndump_dump_str,_tmp235);
Cyc_Absyndump_dumpatts(_tmp231);
goto _LL3;}else{_LL6: _LL7:
# 1111
 Cyc_Absyndump_dumpatts(_tmp231);
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _dyneither_ptr*),struct _dyneither_ptr*))Cyc_Absyndump_dumptqtd)(_tmp234,_tmp233,Cyc_Absyndump_dump_str,_tmp235);
goto _LL3;}_LL3:;}
# 1115
if((unsigned int)_tmp230){
Cyc_Absyndump_dump(({const char*_tmp22F="@requires ";_tag_dyneither(_tmp22F,sizeof(char),11U);}));
Cyc_Absyndump_dumpexp(_tmp230);}
# 1119
if(_tmp232 != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dumpexp(_tmp232);}
# 1123
Cyc_Absyndump_dump_semi();}}
# 1127
void Cyc_Absyndump_dumptypedefname(struct Cyc_Absyn_Typedefdecl*td){
Cyc_Absyndump_dumpqvar(td->name);
Cyc_Absyndump_dumptvars(td->tvs);}
# 1132
static void Cyc_Absyndump_dump_atts_qvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dumpatts(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}struct _tuple19{void*f1;struct _tuple0*f2;};
# 1136
static void Cyc_Absyndump_dump_callconv_qvar(struct _tuple19*pr){
{void*_tmp236=(*pr).f1;void*_tmp237=_tmp236;switch(*((int*)_tmp237)){case 11U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _LL4:
 Cyc_Absyndump_dump(({const char*_tmp238="_stdcall";_tag_dyneither(_tmp238,sizeof(char),9U);}));goto _LL0;case 2U: _LL5: _LL6:
 Cyc_Absyndump_dump(({const char*_tmp239="_cdecl";_tag_dyneither(_tmp239,sizeof(char),7U);}));goto _LL0;case 3U: _LL7: _LL8:
 Cyc_Absyndump_dump(({const char*_tmp23A="_fastcall";_tag_dyneither(_tmp23A,sizeof(char),10U);}));goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 1144
Cyc_Absyndump_dumpqvar((*pr).f2);}
# 1146
static void Cyc_Absyndump_dump_callconv_fdqvar(struct Cyc_Absyn_Fndecl*fd){
Cyc_Absyndump_dump_callconv(fd->attributes);
Cyc_Absyndump_dumpqvar(fd->name);}
# 1151
static void Cyc_Absyndump_dumpids(struct Cyc_List_List*vds){
for(0;vds != 0;vds=vds->tl){
Cyc_Absyndump_dumpqvar(((struct Cyc_Absyn_Vardecl*)vds->hd)->name);
if(vds->tl != 0)Cyc_Absyndump_dump_char((int)',');}}
# 1158
static int Cyc_Absyndump_is_char_ptr(void*t){
# 1160
void*_tmp23B=t;void*_tmp240;void*_tmp23F;switch(*((int*)_tmp23B)){case 1U: _LL1: _tmp23F=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23B)->f2;if(_tmp23F != 0){_LL2:
 return Cyc_Absyndump_is_char_ptr(_tmp23F);}else{goto _LL5;}case 5U: _LL3: _tmp240=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23B)->f1).elt_typ;_LL4:
# 1163
 L: {
void*_tmp23C=_tmp240;void*_tmp23E;void*_tmp23D;switch(*((int*)_tmp23C)){case 1U: _LL8: _tmp23D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp23C)->f2;if(_tmp23D != 0){_LL9:
 _tmp240=_tmp23D;goto L;}else{goto _LLE;}case 17U: _LLA: _tmp23E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23C)->f4;if(_tmp23E != 0){_LLB:
 _tmp240=_tmp23E;goto L;}else{goto _LLE;}case 6U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23C)->f2 == Cyc_Absyn_Char_sz){_LLC: _LLD:
 return 1;}else{goto _LLE;}default: _LLE: _LLF:
 return 0;}_LL7:;}default: _LL5: _LL6:
# 1170
 return 0;}_LL0:;}
# 1174
void Cyc_Absyndump_dumpvardecl(struct Cyc_Absyn_Vardecl*vd,unsigned int loc,int do_semi){
struct Cyc_Absyn_Vardecl*_tmp241=vd;enum Cyc_Absyn_Scope _tmp257;struct _tuple0*_tmp256;unsigned int _tmp255;struct Cyc_Absyn_Tqual _tmp254;void*_tmp253;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_List_List*_tmp251;_LL1: _tmp257=_tmp241->sc;_tmp256=_tmp241->name;_tmp255=_tmp241->varloc;_tmp254=_tmp241->tq;_tmp253=_tmp241->type;_tmp252=_tmp241->initializer;_tmp251=_tmp241->attributes;_LL2:;
{enum Cyc_Cyclone_C_Compilers _tmp242=Cyc_Cyclone_c_compiler;if(_tmp242 == Cyc_Cyclone_Gcc_c){_LL4: _LL5:
# 1179
 if(_tmp257 == Cyc_Absyn_Extern  && Cyc_Absyndump_qvar_to_Cids){
void*_tmp243=Cyc_Tcutil_compress(_tmp253);void*_tmp244=_tmp243;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp244)->tag == 9U){_LL9: _LLA:
 goto _LL8;}else{_LLB: _LLC:
 Cyc_Absyndump_dumpscope(_tmp257);}_LL8:;}else{
# 1185
Cyc_Absyndump_dumpscope(_tmp257);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct _tuple0*),struct _tuple0*))Cyc_Absyndump_dumptqtd)(_tmp254,_tmp253,Cyc_Absyndump_dumpqvar,_tmp256);
Cyc_Absyndump_dumpatts(_tmp251);
goto _LL3;}else{_LL6: _LL7:
# 1190
 Cyc_Absyndump_dumpatts(_tmp251);
Cyc_Absyndump_dumpscope(_tmp257);{
struct _RegionHandle _tmp245=_new_region("temp");struct _RegionHandle*temp=& _tmp245;_push_region(temp);{
int is_cp=Cyc_Absyndump_is_char_ptr(_tmp253);
struct _tuple11 _tmp246=Cyc_Absynpp_to_tms(temp,_tmp254,_tmp253);struct _tuple11 _tmp247=_tmp246;struct Cyc_Absyn_Tqual _tmp250;void*_tmp24F;struct Cyc_List_List*_tmp24E;_LLE: _tmp250=_tmp247.f1;_tmp24F=_tmp247.f2;_tmp24E=_tmp247.f3;_LLF:;{
# 1196
void*call_conv=(void*)& Cyc_Absyn_Unused_att_val;
{struct Cyc_List_List*tms2=_tmp24E;for(0;tms2 != 0;tms2=tms2->tl){
void*_tmp248=(void*)tms2->hd;void*_tmp249=_tmp248;struct Cyc_List_List*_tmp24C;if(((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp249)->tag == 5U){_LL11: _tmp24C=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp249)->f2;_LL12:
# 1200
 for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){
void*_tmp24A=(void*)_tmp24C->hd;void*_tmp24B=_tmp24A;switch(*((int*)_tmp24B)){case 1U: _LL16: _LL17:
 call_conv=(void*)& Cyc_Absyn_Stdcall_att_val;goto _LL15;case 2U: _LL18: _LL19:
 call_conv=(void*)& Cyc_Absyn_Cdecl_att_val;goto _LL15;case 3U: _LL1A: _LL1B:
 call_conv=(void*)& Cyc_Absyn_Fastcall_att_val;goto _LL15;default: _LL1C: _LL1D:
 goto _LL15;}_LL15:;}
# 1207
goto _LL10;}else{_LL13: _LL14:
 goto _LL10;}_LL10:;}}
# 1210
Cyc_Absyndump_dumptq(_tmp250);
Cyc_Absyndump_dumpntyp(_tmp24F);{
struct _tuple19 _tmp24D=({struct _tuple19 _tmp2FF;_tmp2FF.f1=call_conv,_tmp2FF.f2=_tmp256;_tmp2FF;});
({int _tmp358=is_cp;((void(*)(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(struct _tuple19*),struct _tuple19*a))Cyc_Absyndump_dumptms)(_tmp358,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp24E),Cyc_Absyndump_dump_callconv_qvar,& _tmp24D);});};};}
# 1215
_npop_handler(0U);goto _LL3;
# 1192
;_pop_region(temp);};}_LL3:;}
# 1218
if(_tmp252 != 0){
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp252);}
# 1222
if(do_semi)Cyc_Absyndump_dump_semi();}
# 1225
static void Cyc_Absyndump_dump_aggrdecl(struct Cyc_Absyn_Aggrdecl*ad){
Cyc_Absyndump_dumpscope(ad->sc);
if(ad->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)
Cyc_Absyndump_dump(({const char*_tmp258="@tagged ";_tag_dyneither(_tmp258,sizeof(char),9U);}));
Cyc_Absyndump_dumpaggr_kind(ad->kind);
Cyc_Absyndump_dumpqvar(ad->name);
Cyc_Absyndump_dumpkindedtvars(ad->tvs);
if(ad->impl == 0)return;else{
# 1234
Cyc_Absyndump_dump_char((int)'{');
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)
({struct Cyc_List_List*_tmp35B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars;struct _dyneither_ptr _tmp35A=({const char*_tmp259="<";_tag_dyneither(_tmp259,sizeof(char),2U);});struct _dyneither_ptr _tmp359=({const char*_tmp25A=">";_tag_dyneither(_tmp25A,sizeof(char),2U);});((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_egroup)(Cyc_Absyndump_dumpkindedtvar,_tmp35B,_tmp35A,_tmp359,({const char*_tmp25B=",";_tag_dyneither(_tmp25B,sizeof(char),2U);}));});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0){
Cyc_Absyndump_dump_char((int)':');
Cyc_Absyndump_dump_rgnpo(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po);}
# 1241
Cyc_Absyndump_dumpaggrfields(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
Cyc_Absyndump_dump(({const char*_tmp25C="}";_tag_dyneither(_tmp25C,sizeof(char),2U);}));
Cyc_Absyndump_dumpatts(ad->attributes);}}
# 1246
static void Cyc_Absyndump_dump_enumdecl(struct Cyc_Absyn_Enumdecl*ed){
struct Cyc_Absyn_Enumdecl*_tmp25D=ed;enum Cyc_Absyn_Scope _tmp262;struct _tuple0*_tmp261;struct Cyc_Core_Opt*_tmp260;_LL1: _tmp262=_tmp25D->sc;_tmp261=_tmp25D->name;_tmp260=_tmp25D->fields;_LL2:;
Cyc_Absyndump_dumpscope(_tmp262);
Cyc_Absyndump_dump(({const char*_tmp25E="enum ";_tag_dyneither(_tmp25E,sizeof(char),6U);}));
Cyc_Absyndump_dumpqvar(_tmp261);
if(_tmp260 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpenumfields((struct Cyc_List_List*)_tmp260->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp25F="}";_tag_dyneither(_tmp25F,sizeof(char),2U);}));}}
# 1257
static void Cyc_Absyndump_dump_datatypedecl(struct Cyc_Absyn_Datatypedecl*dd){
struct Cyc_Absyn_Datatypedecl*_tmp263=dd;enum Cyc_Absyn_Scope _tmp26B;struct _tuple0*_tmp26A;struct Cyc_List_List*_tmp269;struct Cyc_Core_Opt*_tmp268;int _tmp267;_LL1: _tmp26B=_tmp263->sc;_tmp26A=_tmp263->name;_tmp269=_tmp263->tvs;_tmp268=_tmp263->fields;_tmp267=_tmp263->is_extensible;_LL2:;
Cyc_Absyndump_dumpscope(_tmp26B);
if(_tmp267)Cyc_Absyndump_dump(({const char*_tmp264="@extensible ";_tag_dyneither(_tmp264,sizeof(char),13U);}));
Cyc_Absyndump_dump(({const char*_tmp265="datatype ";_tag_dyneither(_tmp265,sizeof(char),10U);}));
Cyc_Absyndump_dumpqvar(_tmp26A);
Cyc_Absyndump_dumptvars(_tmp269);
if(_tmp268 != 0){
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpdatatypefields((struct Cyc_List_List*)_tmp268->v);
Cyc_Absyndump_dump_nospace(({const char*_tmp266="}";_tag_dyneither(_tmp266,sizeof(char),2U);}));}}struct _tuple20{unsigned int f1;struct _tuple0*f2;int f3;};
# 1271
void Cyc_Absyndump_dumpdecl(struct Cyc_Absyn_Decl*d){
Cyc_Absyndump_dumploc(d->loc);{
void*_tmp26C=d->r;void*_tmp26D=_tmp26C;struct Cyc_List_List*_tmp29A;struct Cyc_List_List*_tmp299;struct Cyc_List_List*_tmp298;struct _tuple0*_tmp297;struct Cyc_List_List*_tmp296;struct _dyneither_ptr*_tmp295;struct Cyc_List_List*_tmp294;struct Cyc_Absyn_Typedefdecl*_tmp293;struct Cyc_Absyn_Tvar*_tmp292;struct Cyc_Absyn_Vardecl*_tmp291;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_List_List*_tmp28F;struct Cyc_Absyn_Pat*_tmp28E;struct Cyc_Absyn_Exp*_tmp28D;struct Cyc_Absyn_Enumdecl*_tmp28C;struct Cyc_Absyn_Datatypedecl*_tmp28B;struct Cyc_Absyn_Aggrdecl*_tmp28A;struct Cyc_Absyn_Fndecl*_tmp289;struct Cyc_Absyn_Vardecl*_tmp288;switch(*((int*)_tmp26D)){case 0U: _LL1: _tmp288=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LL2:
# 1275
 Cyc_Absyndump_dumpvardecl(_tmp288,d->loc,1);
goto _LL0;case 1U: _LL3: _tmp289=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LL4:
# 1279
{enum Cyc_Cyclone_C_Compilers _tmp26E=Cyc_Cyclone_c_compiler;if(_tmp26E == Cyc_Cyclone_Vc_c){_LL20: _LL21:
 Cyc_Absyndump_dumpatts(_tmp289->attributes);goto _LL1F;}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 1283
if(_tmp289->is_inline){
enum Cyc_Cyclone_C_Compilers _tmp26F=Cyc_Cyclone_c_compiler;enum Cyc_Cyclone_C_Compilers _tmp272;if(_tmp26F == Cyc_Cyclone_Vc_c){_LL25: _LL26:
 Cyc_Absyndump_dump(({const char*_tmp270="__inline";_tag_dyneither(_tmp270,sizeof(char),9U);}));goto _LL24;}else{_LL27: _tmp272=_tmp26F;_LL28:
 Cyc_Absyndump_dump(({const char*_tmp271="inline";_tag_dyneither(_tmp271,sizeof(char),7U);}));goto _LL24;}_LL24:;}
# 1288
Cyc_Absyndump_dumpscope(_tmp289->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->tag=9U,(_tmp274->f1).tvars=_tmp289->tvs,(_tmp274->f1).effect=_tmp289->effect,(_tmp274->f1).ret_tqual=_tmp289->ret_tqual,(_tmp274->f1).ret_typ=_tmp289->ret_type,({
# 1291
struct Cyc_List_List*_tmp35C=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp289->args);(_tmp274->f1).args=_tmp35C;}),(_tmp274->f1).c_varargs=_tmp289->c_varargs,(_tmp274->f1).cyc_varargs=_tmp289->cyc_varargs,(_tmp274->f1).rgn_po=_tmp289->rgn_po,(_tmp274->f1).attributes=0,(_tmp274->f1).requires_clause=_tmp289->requires_clause,(_tmp274->f1).requires_relns=_tmp289->requires_relns,(_tmp274->f1).ensures_clause=_tmp289->ensures_clause,(_tmp274->f1).ensures_relns=_tmp289->ensures_relns;_tmp274;});
# 1296
{enum Cyc_Cyclone_C_Compilers _tmp273=Cyc_Cyclone_c_compiler;if(_tmp273 == Cyc_Cyclone_Gcc_c){_LL2A: _LL2B:
# 1298
({struct Cyc_Absyn_Tqual _tmp35E=Cyc_Absyn_empty_tqual(0U);void*_tmp35D=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp35E,_tmp35D,Cyc_Absyndump_dump_atts_qvar,_tmp289);});
goto _LL29;}else{_LL2C: _LL2D:
# 1301
({struct Cyc_Absyn_Tqual _tmp360=Cyc_Absyn_empty_tqual(0U);void*_tmp35F=t;((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*))Cyc_Absyndump_dumptqtd)(_tmp360,_tmp35F,Cyc_Absyndump_dump_callconv_fdqvar,_tmp289);});
goto _LL29;}_LL29:;}
# 1304
Cyc_Absyndump_dump_char((int)'{');
Cyc_Absyndump_dumpstmt(_tmp289->body,0);
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;};case 5U: _LL5: _tmp28A=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LL6:
# 1309
 Cyc_Absyndump_dump_aggrdecl(_tmp28A);Cyc_Absyndump_dump_semi();
goto _LL0;case 6U: _LL7: _tmp28B=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LL8:
# 1312
 Cyc_Absyndump_dump_datatypedecl(_tmp28B);Cyc_Absyndump_dump_semi();
goto _LL0;case 7U: _LL9: _tmp28C=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LLA:
# 1315
 Cyc_Absyndump_dump_enumdecl(_tmp28C);Cyc_Absyndump_dump_semi();
goto _LL0;case 2U: _LLB: _tmp28E=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_tmp28D=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp26D)->f3;_LLC:
# 1318
 Cyc_Absyndump_dump(({const char*_tmp275="let";_tag_dyneither(_tmp275,sizeof(char),4U);}));
Cyc_Absyndump_dumppat(_tmp28E);
Cyc_Absyndump_dump_char((int)'=');
Cyc_Absyndump_dumpexp(_tmp28D);
Cyc_Absyndump_dump_semi();
goto _LL0;case 3U: _LLD: _tmp28F=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LLE:
# 1325
 Cyc_Absyndump_dump(({const char*_tmp276="let ";_tag_dyneither(_tmp276,sizeof(char),5U);}));
Cyc_Absyndump_dumpids(_tmp28F);
Cyc_Absyndump_dump_semi();
goto _LL0;case 4U: _LLF: _tmp292=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_tmp291=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp26D)->f2;_tmp290=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp26D)->f3;_LL10:
# 1330
 Cyc_Absyndump_dump(({const char*_tmp277="region";_tag_dyneither(_tmp277,sizeof(char),7U);}));
Cyc_Absyndump_dump(({const char*_tmp278="<";_tag_dyneither(_tmp278,sizeof(char),2U);}));Cyc_Absyndump_dumptvar(_tmp292);Cyc_Absyndump_dump(({const char*_tmp279="> ";_tag_dyneither(_tmp279,sizeof(char),3U);}));Cyc_Absyndump_dumpqvar(_tmp291->name);
if((unsigned int)_tmp290){
Cyc_Absyndump_dump(({const char*_tmp27A=" = open(";_tag_dyneither(_tmp27A,sizeof(char),9U);}));
Cyc_Absyndump_dumpexp(_tmp290);
Cyc_Absyndump_dump(({const char*_tmp27B=")";_tag_dyneither(_tmp27B,sizeof(char),2U);}));}
# 1337
Cyc_Absyndump_dump_semi();
goto _LL0;case 8U: _LL11: _tmp293=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LL12:
# 1344
 Cyc_Absyndump_dump(({const char*_tmp27C="typedef";_tag_dyneither(_tmp27C,sizeof(char),8U);}));{
void*t;
if(_tmp293->defn == 0)
({void*_tmp361=Cyc_Absyn_new_evar(_tmp293->kind,0);t=_tmp361;});else{
# 1349
t=(void*)_check_null(_tmp293->defn);}
((void(*)(struct Cyc_Absyn_Tqual,void*,void(*f)(struct Cyc_Absyn_Typedefdecl*),struct Cyc_Absyn_Typedefdecl*))Cyc_Absyndump_dumptqtd)(_tmp293->tq,t,Cyc_Absyndump_dumptypedefname,_tmp293);
Cyc_Absyndump_dumpatts(_tmp293->atts);
Cyc_Absyndump_dump_semi();
# 1354
goto _LL0;};case 9U: _LL13: _tmp295=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_tmp294=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp26D)->f2;_LL14:
# 1356
 Cyc_Absyndump_dump(({const char*_tmp27D="namespace ";_tag_dyneither(_tmp27D,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(_tmp295);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp294 != 0;_tmp294=_tmp294->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp294->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 10U: _LL15: _tmp297=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_tmp296=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp26D)->f2;_LL16:
# 1364
 Cyc_Absyndump_dump(({const char*_tmp27E="using ";_tag_dyneither(_tmp27E,sizeof(char),7U);}));
Cyc_Absyndump_dumpqvar(_tmp297);
Cyc_Absyndump_dump_char((int)'{');
for(0;_tmp296 != 0;_tmp296=_tmp296->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp296->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 11U: _LL17: _tmp298=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_LL18:
# 1372
 Cyc_Absyndump_dump(({const char*_tmp27F="extern \"C\" {";_tag_dyneither(_tmp27F,sizeof(char),13U);}));
for(0;_tmp298 != 0;_tmp298=_tmp298->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp298->hd);}
Cyc_Absyndump_dump_char((int)'}');
goto _LL0;case 12U: _LL19: _tmp29A=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp26D)->f1;_tmp299=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp26D)->f2;_LL1A:
# 1378
 Cyc_Absyndump_dump(({const char*_tmp280="extern \"C include\" {";_tag_dyneither(_tmp280,sizeof(char),21U);}));
for(0;_tmp29A != 0;_tmp29A=_tmp29A->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)_tmp29A->hd);}
Cyc_Absyndump_dump_char((int)'}');
if(_tmp299 != 0){
Cyc_Absyndump_dump(({const char*_tmp281=" export {";_tag_dyneither(_tmp281,sizeof(char),10U);}));
for(0;_tmp299 != 0;_tmp299=_tmp299->tl){
struct _tuple20 _tmp282=*((struct _tuple20*)_tmp299->hd);struct _tuple20 _tmp283=_tmp282;struct _tuple0*_tmp284;_LL2F: _tmp284=_tmp283.f2;_LL30:;
Cyc_Absyndump_dumpqvar(_tmp284);
if(_tmp299->tl != 0)Cyc_Absyndump_dump_char((int)',');}
# 1389
Cyc_Absyndump_dump(({const char*_tmp285="}";_tag_dyneither(_tmp285,sizeof(char),2U);}));}
# 1391
goto _LL0;case 13U: _LL1B: _LL1C:
# 1393
 Cyc_Absyndump_dump(({const char*_tmp286=" __cyclone_port_on__; ";_tag_dyneither(_tmp286,sizeof(char),23U);}));
goto _LL0;default: _LL1D: _LL1E:
# 1396
 Cyc_Absyndump_dump(({const char*_tmp287=" __cyclone_port_off__; ";_tag_dyneither(_tmp287,sizeof(char),24U);}));
goto _LL0;}_LL0:;};}
# 1401
static void Cyc_Absyndump_dump_upperbound(struct Cyc_Absyn_Exp*e){
struct _tuple12 pr=Cyc_Evexp_eval_const_uint_exp(e);
if(pr.f1 != 1  || !pr.f2){
Cyc_Absyndump_dump_char((int)'{');Cyc_Absyndump_dumpexp(e);Cyc_Absyndump_dump_char((int)'}');}}
# 1408
void Cyc_Absyndump_dumptms(int is_char_ptr,struct Cyc_List_List*tms,void(*f)(void*),void*a){
if(tms == 0){
f(a);
return;}{
# 1413
void*_tmp29B=(void*)tms->hd;void*_tmp29C=_tmp29B;void*_tmp2CC;union Cyc_Absyn_Constraint*_tmp2CB;union Cyc_Absyn_Constraint*_tmp2CA;union Cyc_Absyn_Constraint*_tmp2C9;struct Cyc_Absyn_Tqual _tmp2C8;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29C)->tag == 2U){_LL1: _tmp2CC=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29C)->f1).rgn;_tmp2CB=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29C)->f1).nullable;_tmp2CA=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29C)->f1).bounds;_tmp2C9=(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29C)->f1).zero_term;_tmp2C8=((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp29C)->f2;_LL2:
# 1417
{void*_tmp29D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp2CA);void*_tmp29E=_tmp29D;struct Cyc_Absyn_Exp*_tmp29F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp29E)->tag == 0U){_LL6: _LL7:
 Cyc_Absyndump_dump_char((int)'?');goto _LL5;}else{_LL8: _tmp29F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp29E)->f1;_LL9:
# 1420
 Cyc_Absyndump_dump_char((int)(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmp2CB)?'*':'@'));
Cyc_Absyndump_dump_upperbound(_tmp29F);
goto _LL5;}_LL5:;}
# 1424
if((!Cyc_Absyndump_qvar_to_Cids  && !is_char_ptr) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2C9))Cyc_Absyndump_dump(({const char*_tmp2A0="@zeroterm";_tag_dyneither(_tmp2A0,sizeof(char),10U);}));
if((!Cyc_Absyndump_qvar_to_Cids  && is_char_ptr) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2C9))Cyc_Absyndump_dump(({const char*_tmp2A1="@nozeroterm";_tag_dyneither(_tmp2A1,sizeof(char),12U);}));
{void*_tmp2A2=Cyc_Tcutil_compress(_tmp2CC);void*_tmp2A3=_tmp2A2;struct Cyc_Absyn_Tvar*_tmp2AB;switch(*((int*)_tmp2A3)){case 20U: _LLB: _LLC:
 if(!Cyc_Absyndump_qvar_to_Cids)Cyc_Absyndump_dump(({const char*_tmp2A4="`H";_tag_dyneither(_tmp2A4,sizeof(char),3U);}));goto _LLA;case 21U: _LLD: _LLE:
 Cyc_Absyndump_dump(({const char*_tmp2A5="`U";_tag_dyneither(_tmp2A5,sizeof(char),3U);}));goto _LLA;case 22U: _LLF: _LL10:
 Cyc_Absyndump_dump(({const char*_tmp2A6="`RC";_tag_dyneither(_tmp2A6,sizeof(char),4U);}));goto _LLA;case 2U: _LL11: _tmp2AB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2A3)->f1;_LL12:
 Cyc_Absyndump_dumptvar(_tmp2AB);goto _LLA;case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A3)->f2 == 0){_LL13: _LL14:
 Cyc_Absyndump_dumpntyp(Cyc_Tcutil_compress(_tmp2CC));goto _LLA;}else{goto _LL17;}case 17U: _LL15: _LL16:
 Cyc_Absyndump_dump(({const char*_tmp2A7="@region(";_tag_dyneither(_tmp2A7,sizeof(char),9U);}));Cyc_Absyndump_dumptyp(_tmp2CC);Cyc_Absyndump_dump(({const char*_tmp2A8=")";_tag_dyneither(_tmp2A8,sizeof(char),2U);}));goto _LLA;default: _LL17: _LL18:
({void*_tmp2A9=0U;({struct _dyneither_ptr _tmp362=({const char*_tmp2AA="dumptms: bad rgn type in Pointer_mod";_tag_dyneither(_tmp2AA,sizeof(char),37U);});Cyc_Tcutil_impos(_tmp362,_tag_dyneither(_tmp2A9,sizeof(void*),0U));});});}_LLA:;}
# 1435
Cyc_Absyndump_dumptq(_tmp2C8);
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
return;}else{_LL3: _LL4: {
# 1440
int next_is_pointer=0;
if(tms->tl != 0){
void*_tmp2AC=(void*)((struct Cyc_List_List*)_check_null(tms->tl))->hd;void*_tmp2AD=_tmp2AC;if(((struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct*)_tmp2AD)->tag == 2U){_LL1A: _LL1B:
 next_is_pointer=1;goto _LL19;}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 1446
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)'(');
Cyc_Absyndump_dumptms(0,tms->tl,f,a);
if(next_is_pointer)
Cyc_Absyndump_dump_char((int)')');
{void*_tmp2AE=(void*)tms->hd;void*_tmp2AF=_tmp2AE;struct Cyc_List_List*_tmp2C7;struct Cyc_List_List*_tmp2C6;unsigned int _tmp2C5;int _tmp2C4;struct Cyc_List_List*_tmp2C3;unsigned int _tmp2C2;struct Cyc_List_List*_tmp2C1;int _tmp2C0;struct Cyc_Absyn_VarargInfo*_tmp2BF;void*_tmp2BE;struct Cyc_List_List*_tmp2BD;struct Cyc_Absyn_Exp*_tmp2BC;struct Cyc_Absyn_Exp*_tmp2BB;struct Cyc_Absyn_Exp*_tmp2BA;union Cyc_Absyn_Constraint*_tmp2B9;union Cyc_Absyn_Constraint*_tmp2B8;switch(*((int*)_tmp2AF)){case 0U: _LL1F: _tmp2B8=((struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1;_LL20:
# 1453
 Cyc_Absyndump_dump(({const char*_tmp2B0="[]";_tag_dyneither(_tmp2B0,sizeof(char),3U);}));
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2B8))Cyc_Absyndump_dump(({const char*_tmp2B1="@zeroterm";_tag_dyneither(_tmp2B1,sizeof(char),10U);}));
goto _LL1E;case 1U: _LL21: _tmp2BA=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1;_tmp2B9=((struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f2;_LL22:
# 1457
 Cyc_Absyndump_dump_char((int)'[');Cyc_Absyndump_dumpexp(_tmp2BA);Cyc_Absyndump_dump_char((int)']');
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2B9))Cyc_Absyndump_dump(({const char*_tmp2B2="@zeroterm";_tag_dyneither(_tmp2B2,sizeof(char),10U);}));
goto _LL1E;case 3U: if(((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->tag == 1U){_LL23: _tmp2C1=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f1;_tmp2C0=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f2;_tmp2BF=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f3;_tmp2BE=(void*)((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f4;_tmp2BD=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f5;_tmp2BC=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f6;_tmp2BB=((struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f7;_LL24:
# 1461
 Cyc_Absyndump_dumpfunargs(_tmp2C1,_tmp2C0,_tmp2BF,_tmp2BE,_tmp2BD,_tmp2BC,_tmp2BB);goto _LL1E;}else{_LL25: _tmp2C3=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f1;_tmp2C2=((struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct*)((struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1)->f2;_LL26:
# 1463
({struct Cyc_List_List*_tmp365=_tmp2C3;struct _dyneither_ptr _tmp364=({const char*_tmp2B3="(";_tag_dyneither(_tmp2B3,sizeof(char),2U);});struct _dyneither_ptr _tmp363=({const char*_tmp2B4=")";_tag_dyneither(_tmp2B4,sizeof(char),2U);});((void(*)(void(*f)(struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr start,struct _dyneither_ptr end,struct _dyneither_ptr sep))Cyc_Absyndump_group)(Cyc_Absyndump_dump_str,_tmp365,_tmp364,_tmp363,({const char*_tmp2B5=",";_tag_dyneither(_tmp2B5,sizeof(char),2U);}));});goto _LL1E;}case 4U: _LL27: _tmp2C6=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f1;_tmp2C5=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f2;_tmp2C4=((struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f3;_LL28:
# 1465
 if(_tmp2C4)Cyc_Absyndump_dumpkindedtvars(_tmp2C6);else{Cyc_Absyndump_dumptvars(_tmp2C6);}goto _LL1E;case 5U: _LL29: _tmp2C7=((struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct*)_tmp2AF)->f2;_LL2A:
 Cyc_Absyndump_dumpatts(_tmp2C7);goto _LL1E;default: _LL2B: _LL2C:
({void*_tmp2B6=0U;({struct _dyneither_ptr _tmp366=({const char*_tmp2B7="dumptms";_tag_dyneither(_tmp2B7,sizeof(char),8U);});Cyc_Tcutil_impos(_tmp366,_tag_dyneither(_tmp2B6,sizeof(void*),0U));});});}_LL1E:;}
# 1469
return;}}_LL0:;};}
# 1473
void Cyc_Absyndump_dumptqtd(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(void*),void*a){
int _tmp2CD=Cyc_Absyndump_is_char_ptr(t);
struct _RegionHandle _tmp2CE=_new_region("temp");struct _RegionHandle*temp=& _tmp2CE;_push_region(temp);
{struct _tuple11 _tmp2CF=Cyc_Absynpp_to_tms(temp,tq,t);struct _tuple11 _tmp2D0=_tmp2CF;struct Cyc_Absyn_Tqual _tmp2D3;void*_tmp2D2;struct Cyc_List_List*_tmp2D1;_LL1: _tmp2D3=_tmp2D0.f1;_tmp2D2=_tmp2D0.f2;_tmp2D1=_tmp2D0.f3;_LL2:;
Cyc_Absyndump_dumptq(_tmp2D3);
Cyc_Absyndump_dumpntyp(_tmp2D2);
({int _tmp369=_tmp2CD;struct Cyc_List_List*_tmp368=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp2D1);void(*_tmp367)(void*)=f;Cyc_Absyndump_dumptms(_tmp369,_tmp368,_tmp367,a);});}
# 1476
;_pop_region(temp);}
# 1483
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f){
# 1485
*Cyc_Absyndump_dump_file=f;
for(0;tdl != 0;tdl=tdl->tl){
Cyc_Absyndump_dumpdecl((struct Cyc_Absyn_Decl*)tdl->hd);}
({void*_tmp2D4=0U;({struct Cyc___cycFILE*_tmp36B=f;struct _dyneither_ptr _tmp36A=({const char*_tmp2D5="\n";_tag_dyneither(_tmp2D5,sizeof(char),2U);});Cyc_fprintf(_tmp36B,_tmp36A,_tag_dyneither(_tmp2D4,sizeof(void*),0U));});});}
# 1491
static void Cyc_Absyndump_dump_decl_interface(struct Cyc_Absyn_Decl*d){
void*_tmp2D6=d->r;void*_tmp2D7=_tmp2D6;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F1;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*_tmp2EF;struct _dyneither_ptr*_tmp2EE;struct Cyc_List_List*_tmp2ED;struct Cyc_Absyn_Typedefdecl*_tmp2EC;struct Cyc_Absyn_Enumdecl*_tmp2EB;struct Cyc_Absyn_Datatypedecl*_tmp2EA;struct Cyc_Absyn_Aggrdecl*_tmp2E9;struct Cyc_Absyn_Fndecl*_tmp2E8;struct Cyc_Absyn_Vardecl*_tmp2E7;switch(*((int*)_tmp2D7)){case 0U: _LL1: _tmp2E7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_LL2:
# 1494
 if(_tmp2E7->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_Exp*init=_tmp2E7->initializer;
_tmp2E7->initializer=0;
if(_tmp2E7->sc == Cyc_Absyn_Public)
Cyc_Absyndump_dump(({const char*_tmp2D8="extern ";_tag_dyneither(_tmp2D8,sizeof(char),8U);}));
Cyc_Absyndump_dumpvardecl(_tmp2E7,d->loc,1);
Cyc_Absyndump_dump(({const char*_tmp2D9="\n";_tag_dyneither(_tmp2D9,sizeof(char),2U);}));
_tmp2E7->initializer=init;
goto _LL0;};case 1U: _LL3: _tmp2E8=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_LL4:
# 1504
 if(_tmp2E8->sc == Cyc_Absyn_Static)return;
Cyc_Absyndump_dumpscope(_tmp2E8->sc);{
void*t=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB->tag=9U,(_tmp2DB->f1).tvars=_tmp2E8->tvs,(_tmp2DB->f1).effect=_tmp2E8->effect,(_tmp2DB->f1).ret_tqual=_tmp2E8->ret_tqual,(_tmp2DB->f1).ret_typ=_tmp2E8->ret_type,({
# 1508
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Absynpp_arg_mk_opt,_tmp2E8->args);(_tmp2DB->f1).args=_tmp36C;}),(_tmp2DB->f1).c_varargs=_tmp2E8->c_varargs,(_tmp2DB->f1).cyc_varargs=_tmp2E8->cyc_varargs,(_tmp2DB->f1).rgn_po=_tmp2E8->rgn_po,(_tmp2DB->f1).attributes=0,(_tmp2DB->f1).requires_clause=_tmp2E8->requires_clause,(_tmp2DB->f1).requires_relns=_tmp2E8->requires_relns,(_tmp2DB->f1).ensures_clause=_tmp2E8->ensures_clause,(_tmp2DB->f1).ensures_relns=_tmp2E8->ensures_relns;_tmp2DB;});
# 1513
({struct Cyc_Absyn_Tqual _tmp36E=Cyc_Absyn_empty_tqual(0U);void*_tmp36D=t;((void(*)(struct Cyc_Absyn_Tqual tq,void*t,void(*f)(struct Cyc_Absyn_Fndecl*),struct Cyc_Absyn_Fndecl*a))Cyc_Absyndump_dumptqtd)(_tmp36E,_tmp36D,Cyc_Absyndump_dump_atts_qvar,_tmp2E8);});
Cyc_Absyndump_dump(({const char*_tmp2DA=";\n";_tag_dyneither(_tmp2DA,sizeof(char),3U);}));
goto _LL0;};case 5U: _LL5: _tmp2E9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_LL6:
# 1517
 if(_tmp2E9->sc == Cyc_Absyn_Static)return;{
struct Cyc_Absyn_AggrdeclImpl*impl=_tmp2E9->impl;
if(_tmp2E9->sc == Cyc_Absyn_Abstract)_tmp2E9->impl=0;
Cyc_Absyndump_dump_aggrdecl(_tmp2E9);
_tmp2E9->impl=impl;
Cyc_Absyndump_dump(({const char*_tmp2DC=";\n";_tag_dyneither(_tmp2DC,sizeof(char),3U);}));
goto _LL0;};case 6U: _LL7: _tmp2EA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_LL8:
# 1525
 Cyc_Absyndump_dump_datatypedecl(_tmp2EA);
Cyc_Absyndump_dump(({const char*_tmp2DD=";\n";_tag_dyneither(_tmp2DD,sizeof(char),3U);}));
goto _LL0;case 7U: _LL9: _tmp2EB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_LLA:
# 1529
 Cyc_Absyndump_dump_enumdecl(_tmp2EB);
Cyc_Absyndump_dump(({const char*_tmp2DE=";\n";_tag_dyneither(_tmp2DE,sizeof(char),3U);}));
goto _LL0;case 8U: _LLB: _tmp2EC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_LLC:
# 1533
 if(_tmp2EC->defn == 0){
Cyc_Absyndump_dumpdecl(d);
Cyc_Absyndump_dump(({const char*_tmp2DF="\n";_tag_dyneither(_tmp2DF,sizeof(char),2U);}));}
# 1537
goto _LL0;case 9U: _LLD: _tmp2EE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_tmp2ED=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp2D7)->f2;_LLE:
# 1539
 Cyc_Absyndump_dump(({const char*_tmp2E0="namespace ";_tag_dyneither(_tmp2E0,sizeof(char),11U);}));
Cyc_Absyndump_dump_str(_tmp2EE);
Cyc_Absyndump_dump(({const char*_tmp2E1="{\n";_tag_dyneither(_tmp2E1,sizeof(char),3U);}));
for(0;_tmp2ED != 0;_tmp2ED=_tmp2ED->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2ED->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E2="}\n";_tag_dyneither(_tmp2E2,sizeof(char),3U);}));
goto _LL0;case 10U: _LLF: _tmp2EF=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp2D7)->f2;_LL10:
# 1547
 for(0;_tmp2EF != 0;_tmp2EF=_tmp2EF->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2EF->hd);}
goto _LL0;case 11U: _LL11: _tmp2F0=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_LL12:
# 1551
 Cyc_Absyndump_dump(({const char*_tmp2E3="extern \"C\" {";_tag_dyneither(_tmp2E3,sizeof(char),13U);}));
for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)_tmp2F0->hd);}
Cyc_Absyndump_dump(({const char*_tmp2E4="}\n";_tag_dyneither(_tmp2E4,sizeof(char),3U);}));
goto _LL0;case 12U: _LL13: _tmp2F2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D7)->f1;_tmp2F1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp2D7)->f2;_LL14:
# 1572 "absyndump.cyc"
 goto _LL0;default: _LL15: _LL16:
({void*_tmp2E5=0U;({struct _dyneither_ptr _tmp36F=({const char*_tmp2E6="bad top-level declaration";_tag_dyneither(_tmp2E6,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp36F,_tag_dyneither(_tmp2E5,sizeof(void*),0U));});});}_LL0:;}
# 1577
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f){
*Cyc_Absyndump_dump_file=f;
for(0;ds != 0;ds=ds->tl){
Cyc_Absyndump_dump_decl_interface((struct Cyc_Absyn_Decl*)ds->hd);}}
