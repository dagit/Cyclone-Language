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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 86 "list.h"
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 966 "absyn.h"
extern void*Cyc_Absyn_fat_bound_type;
# 970
void*Cyc_Absyn_bounds_one();struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);union Cyc_Relations_RelnOp Cyc_Relations_RType(void*);union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct _tuple10{struct Cyc_Absyn_Exp*f1;enum Cyc_Relations_Relation f2;struct Cyc_Absyn_Exp*f3;};
# 63
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2);
# 67
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r);
# 69
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 94
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns);
# 99
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*);
# 102
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 105
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Vardecl*);
# 107
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 109
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 111
int Cyc_Relations_same_relns(struct Cyc_List_List*,struct Cyc_List_List*);
# 113
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 115
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 119
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r);
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r);
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r);
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*r);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
int Cyc_Tcutil_is_integral_type(void*);
# 77
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 90
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 115
void*Cyc_Tcutil_compress(void*t);
# 197
int Cyc_Tcutil_unify(void*,void*);
# 345 "tcutil.h"
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 374
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);struct _union_Node_NZero{int tag;int val;};struct _union_Node_NVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_Node_NType{int tag;void*val;};struct _union_Node_NParam{int tag;unsigned int val;};struct _union_Node_NParamNumelts{int tag;unsigned int val;};struct _union_Node_NReturn{int tag;int val;};union Cyc_Pratt_Node{struct _union_Node_NZero NZero;struct _union_Node_NVar NVar;struct _union_Node_NNumelts NNumelts;struct _union_Node_NType NType;struct _union_Node_NParam NParam;struct _union_Node_NParamNumelts NParamNumelts;struct _union_Node_NReturn NReturn;};
# 60 "pratt.h"
extern union Cyc_Pratt_Node Cyc_Pratt_zero_node;
# 62
union Cyc_Pratt_Node Cyc_Pratt_NVar(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NType(void*);
union Cyc_Pratt_Node Cyc_Pratt_NNumelts(struct Cyc_Absyn_Vardecl*);
union Cyc_Pratt_Node Cyc_Pratt_NParam(unsigned int);
union Cyc_Pratt_Node Cyc_Pratt_NParamNumelts(unsigned int);
union Cyc_Pratt_Node Cyc_Pratt_NReturn();struct Cyc_Pratt_Graph;
# 75
struct Cyc_Pratt_Graph*Cyc_Pratt_empty_graph();
struct Cyc_Pratt_Graph*Cyc_Pratt_copy_graph(struct Cyc_Pratt_Graph*);
# 80
struct Cyc_Pratt_Graph*Cyc_Pratt_add_edge(struct Cyc_Pratt_Graph*G,union Cyc_Pratt_Node i,union Cyc_Pratt_Node j,int a);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 64 "string.h"
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 31 "relations.cyc"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int i){
return({union Cyc_Relations_RelnOp _tmpDA;(_tmpDA.RConst).tag=1U,(_tmpDA.RConst).val=i;_tmpDA;});}
# 34
union Cyc_Relations_RelnOp Cyc_Relations_RVar(struct Cyc_Absyn_Vardecl*v){
return({union Cyc_Relations_RelnOp _tmpDB;(_tmpDB.RVar).tag=2U,(_tmpDB.RVar).val=v;_tmpDB;});}
# 37
union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*v){
return({union Cyc_Relations_RelnOp _tmpDC;(_tmpDC.RNumelts).tag=3U,(_tmpDC.RNumelts).val=v;_tmpDC;});}
# 40
union Cyc_Relations_RelnOp Cyc_Relations_RType(void*t){
return({union Cyc_Relations_RelnOp _tmpDD;(_tmpDD.RType).tag=4U,({void*_tmpF2=Cyc_Tcutil_compress(t);(_tmpDD.RType).val=_tmpF2;});_tmpDD;});}
# 43
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int i){
return({union Cyc_Relations_RelnOp _tmpDE;(_tmpDE.RParam).tag=5U,(_tmpDE.RParam).val=i;_tmpDE;});}
# 46
union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int i){
return({union Cyc_Relations_RelnOp _tmpDF;(_tmpDF.RParamNumelts).tag=6U,(_tmpDF.RParamNumelts).val=i;_tmpDF;});}
# 49
union Cyc_Relations_RelnOp Cyc_Relations_RReturn(){
return({union Cyc_Relations_RelnOp _tmpE0;(_tmpE0.RReturn).tag=7U,(_tmpE0.RReturn).val=0U;_tmpE0;});}struct _tuple12{union Cyc_Relations_RelnOp f1;union Cyc_Relations_RelnOp f2;};
# 53
static int Cyc_Relations_same_relop(union Cyc_Relations_RelnOp r1,union Cyc_Relations_RelnOp r2){
struct _tuple12 _tmp0=({struct _tuple12 _tmpE1;_tmpE1.f1=r1,_tmpE1.f2=r2;_tmpE1;});struct _tuple12 _tmp1=_tmp0;unsigned int _tmpD;unsigned int _tmpC;unsigned int _tmpB;unsigned int _tmpA;void*_tmp9;void*_tmp8;struct Cyc_Absyn_Vardecl*_tmp7;struct Cyc_Absyn_Vardecl*_tmp6;struct Cyc_Absyn_Vardecl*_tmp5;struct Cyc_Absyn_Vardecl*_tmp4;unsigned int _tmp3;unsigned int _tmp2;switch(((_tmp1.f1).RParamNumelts).tag){case 1U: if(((_tmp1.f2).RConst).tag == 1){_LL1: _tmp3=((_tmp1.f1).RConst).val;_tmp2=((_tmp1.f2).RConst).val;_LL2:
 return _tmp3 == _tmp2;}else{goto _LLF;}case 2U: if(((_tmp1.f2).RVar).tag == 2){_LL3: _tmp5=((_tmp1.f1).RVar).val;_tmp4=((_tmp1.f2).RVar).val;_LL4:
 return _tmp5 == _tmp4;}else{goto _LLF;}case 3U: if(((_tmp1.f2).RNumelts).tag == 3){_LL5: _tmp7=((_tmp1.f1).RNumelts).val;_tmp6=((_tmp1.f2).RNumelts).val;_LL6:
 return _tmp7 == _tmp6;}else{goto _LLF;}case 4U: if(((_tmp1.f2).RType).tag == 4){_LL7: _tmp9=((_tmp1.f1).RType).val;_tmp8=((_tmp1.f2).RType).val;_LL8:
 return Cyc_Tcutil_unify(_tmp9,_tmp8);}else{goto _LLF;}case 5U: if(((_tmp1.f2).RParam).tag == 5){_LL9: _tmpB=((_tmp1.f1).RParam).val;_tmpA=((_tmp1.f2).RParam).val;_LLA:
 return _tmpB == _tmpA;}else{goto _LLF;}case 6U: if(((_tmp1.f2).RParamNumelts).tag == 6){_LLB: _tmpD=((_tmp1.f1).RParamNumelts).val;_tmpC=((_tmp1.f2).RParamNumelts).val;_LLC:
 return _tmpD == _tmpC;}else{goto _LLF;}default: if(((_tmp1.f2).RReturn).tag == 7){_LLD: _LLE:
 return 1;}else{_LLF: _LL10:
 return 0;}}_LL0:;}
# 66
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation relation,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns){
# 70
{struct Cyc_List_List*_tmpE=relns;for(0;_tmpE != 0;_tmpE=_tmpE->tl){
struct Cyc_Relations_Reln*_tmpF=(struct Cyc_Relations_Reln*)_tmpE->hd;
if((Cyc_Relations_same_relop(_tmpF->rop1,rop1) && _tmpF->relation == relation) && 
Cyc_Relations_same_relop(_tmpF->rop2,rop2))return relns;}}
# 75
return({struct Cyc_List_List*_tmp11=_region_malloc(rgn,sizeof(*_tmp11));({struct Cyc_Relations_Reln*_tmpF3=({struct Cyc_Relations_Reln*_tmp10=_region_malloc(rgn,sizeof(*_tmp10));_tmp10->rop1=rop1,_tmp10->relation=relation,_tmp10->rop2=rop2;_tmp10;});_tmp11->hd=_tmpF3;}),_tmp11->tl=relns;_tmp11;});}
# 78
struct Cyc_List_List*Cyc_Relations_duplicate_relation(struct _RegionHandle*rgn,struct Cyc_Absyn_Exp*e_old,struct Cyc_Absyn_Exp*e_new,struct Cyc_List_List*relns){
# 81
union Cyc_Relations_RelnOp rop_old=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop_new=Cyc_Relations_RConst(0U);
if(!Cyc_Relations_exp2relnop(e_old,& rop_old))return relns;
if(!Cyc_Relations_exp2relnop(e_new,& rop_new)){
({void*_tmp12=0U;({unsigned int _tmpF5=e_new->loc;struct _dyneither_ptr _tmpF4=({const char*_tmp13="Could not construct relation";_tag_dyneither(_tmp13,sizeof(char),29U);});Cyc_Tcutil_warn(_tmpF5,_tmpF4,_tag_dyneither(_tmp12,sizeof(void*),0U));});});
return relns;}
# 88
{struct Cyc_List_List*_tmp14=relns;for(0;_tmp14 != 0;_tmp14=_tmp14->tl){
struct Cyc_Relations_Reln*_tmp15=(struct Cyc_Relations_Reln*)_tmp14->hd;
union Cyc_Relations_RelnOp _tmp16=_tmp15->rop1;
union Cyc_Relations_RelnOp _tmp17=_tmp15->rop2;
int addIt=0;
if(Cyc_Relations_same_relop(_tmp16,rop_old)){
addIt=1;
_tmp16=rop_new;}
# 97
if(Cyc_Relations_same_relop(_tmp17,rop_old)){
addIt=1;
_tmp17=rop_new;}
# 101
if(addIt)
({struct Cyc_List_List*_tmpF7=({struct Cyc_List_List*_tmp19=_region_malloc(rgn,sizeof(*_tmp19));({struct Cyc_Relations_Reln*_tmpF6=({struct Cyc_Relations_Reln*_tmp18=_region_malloc(rgn,sizeof(*_tmp18));_tmp18->rop1=_tmp16,_tmp18->relation=_tmp15->relation,_tmp18->rop2=_tmp17;_tmp18;});_tmp19->hd=_tmpF6;}),_tmp19->tl=relns;_tmp19;});relns=_tmpF7;});}}
# 104
return relns;}
# 107
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s){
if(r1s == r2s)return 1;
# 111
for(0;r1s != 0;r1s=r1s->tl){
struct Cyc_Relations_Reln*_tmp1A=(struct Cyc_Relations_Reln*)r1s->hd;
int found=0;
{struct Cyc_List_List*_tmp1B=r2s;for(0;_tmp1B != 0;_tmp1B=_tmp1B->tl){
struct Cyc_Relations_Reln*_tmp1C=(struct Cyc_Relations_Reln*)_tmp1B->hd;
if(_tmp1A == _tmp1C  || (Cyc_Relations_same_relop(_tmp1A->rop1,_tmp1C->rop1) && _tmp1A->relation == _tmp1C->relation) && 
# 118
Cyc_Relations_same_relop(_tmp1A->rop2,_tmp1C->rop2)){
found=1;
break;}}}
# 123
if(!found)
return 0;}
# 126
return 1;}
# 129
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*r1s,struct Cyc_List_List*r2s){
if(r1s == r2s)return r1s;{
struct Cyc_List_List*res=0;
int diff=0;
{struct Cyc_List_List*_tmp1D=r1s;for(0;_tmp1D != 0;_tmp1D=_tmp1D->tl){
struct Cyc_Relations_Reln*_tmp1E=(struct Cyc_Relations_Reln*)_tmp1D->hd;
int found=0;
{struct Cyc_List_List*_tmp1F=r2s;for(0;_tmp1F != 0;_tmp1F=_tmp1F->tl){
struct Cyc_Relations_Reln*_tmp20=(struct Cyc_Relations_Reln*)_tmp1F->hd;
if(_tmp1E == _tmp20  || (Cyc_Relations_same_relop(_tmp1E->rop1,_tmp20->rop1) && _tmp1E->relation == _tmp20->relation) && 
# 140
Cyc_Relations_same_relop(_tmp1E->rop2,_tmp20->rop2)){
({struct Cyc_List_List*_tmpF8=({struct Cyc_List_List*_tmp21=_region_malloc(r,sizeof(*_tmp21));_tmp21->hd=_tmp1E,_tmp21->tl=res;_tmp21;});res=_tmpF8;});
found=1;
break;}}}
# 146
if(!found)diff=1;}}
# 148
if(!diff)res=r1s;
return res;};}
# 152
static int Cyc_Relations_rop_contains_var(union Cyc_Relations_RelnOp r,struct Cyc_Absyn_Vardecl*v){
union Cyc_Relations_RelnOp _tmp22=r;struct Cyc_Absyn_Vardecl*_tmp24;struct Cyc_Absyn_Vardecl*_tmp23;switch((_tmp22.RReturn).tag){case 2U: _LL1: _tmp23=(_tmp22.RVar).val;_LL2:
 return v == _tmp23;case 3U: _LL3: _tmp24=(_tmp22.RNumelts).val;_LL4:
 return v == _tmp24;case 4U: _LL5: _LL6:
 goto _LL8;case 5U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;default: _LLD: _LLE:
 return 0;}_LL0:;}
# 164
struct Cyc_List_List*Cyc_Relations_reln_kill_var(struct _RegionHandle*rgn,struct Cyc_List_List*rs,struct Cyc_Absyn_Vardecl*v){
struct Cyc_List_List*p;
int found=0;
for(p=rs;!found  && p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp25=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp25->rop1,v) || Cyc_Relations_rop_contains_var(_tmp25->rop2,v)){
found=1;
break;}}
# 174
if(!found)return rs;{
# 176
struct Cyc_List_List*_tmp26=0;
for(p=rs;p != 0;p=p->tl){
struct Cyc_Relations_Reln*_tmp27=(struct Cyc_Relations_Reln*)p->hd;
if(Cyc_Relations_rop_contains_var(_tmp27->rop1,v) || Cyc_Relations_rop_contains_var(_tmp27->rop2,v))continue;
({struct Cyc_List_List*_tmpF9=({struct Cyc_List_List*_tmp28=_region_malloc(rgn,sizeof(*_tmp28));_tmp28->hd=_tmp27,_tmp28->tl=_tmp26;_tmp28;});_tmp26=_tmpF9;});}
# 182
return _tmp26;};}
# 188
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p){
# 191
RETRY:
 if(e->topt != 0){
void*_tmp29=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A=_tmp29;struct Cyc_Absyn_Exp*_tmp2C;void*_tmp2B;switch(*((int*)_tmp2A)){case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f2 != 0){_LL1: _tmp2B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A)->f2)->hd;_LL2:
# 195
({union Cyc_Relations_RelnOp _tmpFA=Cyc_Relations_RType(_tmp2B);*p=_tmpFA;});return 1;
goto _LL0;}else{goto _LL5;}}else{goto _LL5;}case 9U: _LL3: _tmp2C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2A)->f1;_LL4:
# 198
 e=_tmp2C;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 203
{void*_tmp2D=e->r;void*_tmp2E=_tmp2D;void*_tmp38;void*_tmp37;int _tmp36;void*_tmp35;struct Cyc_Absyn_Exp*_tmp34;switch(*((int*)_tmp2E)){case 14U: _LL8: _tmp34=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E)->f2;_LL9:
 return Cyc_Relations_exp2relnop(_tmp34,p);case 1U: _LLA: _tmp35=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp2E)->f1;_LLB: {
# 206
struct Cyc_Absyn_Vardecl*_tmp2F=Cyc_Tcutil_nonesc_vardecl(_tmp35);
if(_tmp2F != 0){
({union Cyc_Relations_RelnOp _tmpFB=Cyc_Relations_RVar(_tmp2F);*p=_tmpFB;});
return 1;}
# 211
goto _LL7;}case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E)->f1).Int_c).tag == 5){_LLC: _tmp36=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E)->f1).Int_c).val).f2;_LLD:
# 213
({union Cyc_Relations_RelnOp _tmpFC=Cyc_Relations_RConst((unsigned int)_tmp36);*p=_tmpFC;});
return 1;}else{goto _LL12;}case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E)->f1 == Cyc_Absyn_Numelts){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E)->f2 != 0){if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E)->f2)->hd)->r)->tag == 1U){_LLE: _tmp37=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp2E)->f2)->hd)->r)->f1;_LLF: {
# 216
struct Cyc_Absyn_Vardecl*_tmp30=Cyc_Tcutil_nonesc_vardecl(_tmp37);
if(_tmp30 != 0){
({union Cyc_Relations_RelnOp _tmpFD=Cyc_Relations_RNumelts(_tmp30);*p=_tmpFD;});
return 1;}
# 221
goto _LL7;}}else{goto _LL12;}}else{goto _LL12;}}else{goto _LL12;}case 39U: _LL10: _tmp38=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2E)->f1;_LL11:
# 223
{void*_tmp31=Cyc_Tcutil_compress(_tmp38);void*_tmp32=_tmp31;struct Cyc_Absyn_Exp*_tmp33;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp32)->tag == 9U){_LL15: _tmp33=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp32)->f1;_LL16:
# 225
 e=_tmp33;
goto RETRY;}else{_LL17: _LL18:
# 228
({union Cyc_Relations_RelnOp _tmpFE=Cyc_Relations_RType(_tmp38);*p=_tmpFE;});
return 1;}_LL14:;}
# 231
goto _LL7;default: _LL12: _LL13:
 goto _LL7;}_LL7:;}
# 235
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmp39=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmp3A=_tmp39;unsigned int _tmp3C;int _tmp3B;_LL1A: _tmp3C=_tmp3A.f1;_tmp3B=_tmp3A.f2;_LL1B:;
if(_tmp3B){
({union Cyc_Relations_RelnOp _tmpFF=Cyc_Relations_RConst(_tmp3C);*p=_tmpFF;});
return 1;}}
# 242
return 0;}
# 245
struct Cyc_List_List*Cyc_Relations_reln_kill_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e){
void*_tmp3D=e->r;void*_tmp3E=_tmp3D;void*_tmp40;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3E)->tag == 1U){_LL1: _tmp40=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp3E)->f1;_LL2: {
# 248
struct Cyc_Absyn_Vardecl*_tmp3F=Cyc_Tcutil_nonesc_vardecl(_tmp40);
if(_tmp3F != 0)
return Cyc_Relations_reln_kill_var(rgn,r,_tmp3F);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 256
struct Cyc_Relations_Reln*Cyc_Relations_copy_reln(struct _RegionHandle*r2,struct Cyc_Relations_Reln*r){
return({struct Cyc_Relations_Reln*_tmp41=_region_malloc(r2,sizeof(*_tmp41));*_tmp41=*r;_tmp41;});}
# 260
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*r2,struct Cyc_List_List*relns){
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Relations_Reln*(*f)(struct _RegionHandle*,struct Cyc_Relations_Reln*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r2,Cyc_Relations_copy_reln,r2,relns);}
# 264
int Cyc_Relations_same_relns(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r1 != 0  && r2 != 0;(r1=r1->tl,r2=r2->tl)){
struct Cyc_Relations_Reln*_tmp42=(struct Cyc_Relations_Reln*)r1->hd;
struct Cyc_Relations_Reln*_tmp43=(struct Cyc_Relations_Reln*)r2->hd;
if((!Cyc_Relations_same_relop(_tmp42->rop1,_tmp43->rop1) || _tmp42->relation != _tmp43->relation) || !
# 270
Cyc_Relations_same_relop(_tmp42->rop2,_tmp43->rop2))return 0;}
# 272
if(r1 != 0  || r2 != 0)return 0;else{
return 1;}}
# 276
struct Cyc_List_List*Cyc_Relations_reln_assign_var(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Vardecl*v,struct Cyc_Absyn_Exp*e){
# 280
if(v->escapes)return r;
# 283
({struct Cyc_List_List*_tmp100=Cyc_Relations_reln_kill_var(rgn,r,v);r=_tmp100;});
# 288
{void*_tmp44=e->r;void*_tmp45=_tmp44;struct Cyc_Absyn_Exp*_tmp4B;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp45)->tag == 34U){if((((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp45)->f1).fat_result == 1){_LL1: _tmp4B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp45)->f1).num_elts;_LL2:
# 291
 malloc_loop: {
void*_tmp46=_tmp4B->r;void*_tmp47=_tmp46;void*_tmp4A;struct Cyc_Absyn_Exp*_tmp49;switch(*((int*)_tmp47)){case 14U: _LL6: _tmp49=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp47)->f2;_LL7:
 _tmp4B=_tmp49;goto malloc_loop;case 1U: _LL8: _tmp4A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp47)->f1;_LL9: {
# 295
struct Cyc_Absyn_Vardecl*_tmp48=Cyc_Tcutil_nonesc_vardecl(_tmp4A);
if(_tmp48 != 0)
return({struct _RegionHandle*_tmp103=rgn;union Cyc_Relations_RelnOp _tmp102=Cyc_Relations_RVar(_tmp48);union Cyc_Relations_RelnOp _tmp101=Cyc_Relations_RNumelts(v);Cyc_Relations_add_relation(_tmp103,_tmp102,Cyc_Relations_Req,_tmp101,r);});else{
return r;}}default: _LLA: _LLB:
 return r;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 302
 goto _LL0;}_LL0:;}
# 305
if(!Cyc_Tcutil_is_integral_type(v->type))return r;{
# 307
union Cyc_Relations_RelnOp eop=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(e,& eop))
return({struct _RegionHandle*_tmp106=rgn;union Cyc_Relations_RelnOp _tmp105=Cyc_Relations_RVar(v);union Cyc_Relations_RelnOp _tmp104=eop;Cyc_Relations_add_relation(_tmp106,_tmp105,Cyc_Relations_Req,_tmp104,r);});
# 311
return r;};}
# 314
struct Cyc_List_List*Cyc_Relations_reln_assign_exp(struct _RegionHandle*rgn,struct Cyc_List_List*r,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*_tmp4C=e1->r;void*_tmp4D=_tmp4C;void*_tmp4F;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4D)->tag == 1U){_LL1: _tmp4F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4D)->f1;_LL2: {
# 317
struct Cyc_Absyn_Vardecl*_tmp4E=Cyc_Tcutil_nonesc_vardecl(_tmp4F);
if(_tmp4E != 0)
return Cyc_Relations_reln_assign_var(rgn,r,_tmp4E,e2);else{
return r;}}}else{_LL3: _LL4:
 return r;}_LL0:;}
# 326
static union Cyc_Pratt_Node Cyc_Relations_rop2node(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp50=r;unsigned int _tmp57;unsigned int _tmp56;void*_tmp55;struct Cyc_Absyn_Vardecl*_tmp54;struct Cyc_Absyn_Vardecl*_tmp53;switch((_tmp50.RReturn).tag){case 2U: _LL1: _tmp53=(_tmp50.RVar).val;_LL2:
 return Cyc_Pratt_NVar(_tmp53);case 3U: _LL3: _tmp54=(_tmp50.RNumelts).val;_LL4:
 return Cyc_Pratt_NNumelts(_tmp54);case 4U: _LL5: _tmp55=(_tmp50.RType).val;_LL6:
 return Cyc_Pratt_NType(_tmp55);case 5U: _LL7: _tmp56=(_tmp50.RParam).val;_LL8:
 return Cyc_Pratt_NParam(_tmp56);case 6U: _LL9: _tmp57=(_tmp50.RParamNumelts).val;_LLA:
 return Cyc_Pratt_NParamNumelts(_tmp57);case 7U: _LLB: _LLC:
 return Cyc_Pratt_NReturn();default: _LLD: _LLE:
({void*_tmp51=0U;({struct _dyneither_ptr _tmp107=({const char*_tmp52="rop2node";_tag_dyneither(_tmp52,sizeof(char),9U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp107,_tag_dyneither(_tmp51,sizeof(void*),0U));});});}_LL0:;}
# 338
struct _dyneither_ptr Cyc_Relations_relation2string(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmp58=r;switch(_tmp58){case Cyc_Relations_Req: _LL1: _LL2:
 return({const char*_tmp59="==";_tag_dyneither(_tmp59,sizeof(char),3U);});case Cyc_Relations_Rneq: _LL3: _LL4:
 return({const char*_tmp5A="!=";_tag_dyneither(_tmp5A,sizeof(char),3U);});case Cyc_Relations_Rlt: _LL5: _LL6:
 return({const char*_tmp5B="<";_tag_dyneither(_tmp5B,sizeof(char),2U);});default: _LL7: _LL8:
 return({const char*_tmp5C="<=";_tag_dyneither(_tmp5C,sizeof(char),3U);});}_LL0:;}
# 347
struct _dyneither_ptr Cyc_Relations_rop2string(union Cyc_Relations_RelnOp r){
union Cyc_Relations_RelnOp _tmp5D=r;unsigned int _tmp74;unsigned int _tmp73;struct Cyc_Absyn_Vardecl*_tmp72;void*_tmp71;struct Cyc_Absyn_Vardecl*_tmp70;unsigned int _tmp6F;switch((_tmp5D.RParamNumelts).tag){case 1U: _LL1: _tmp6F=(_tmp5D.RConst).val;_LL2:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp60=({struct Cyc_Int_pa_PrintArg_struct _tmpE2;_tmpE2.tag=1U,_tmpE2.f1=(unsigned long)((int)_tmp6F);_tmpE2;});void*_tmp5E[1U];_tmp5E[0]=& _tmp60;({struct _dyneither_ptr _tmp108=({const char*_tmp5F="%d";_tag_dyneither(_tmp5F,sizeof(char),3U);});Cyc_aprintf(_tmp108,_tag_dyneither(_tmp5E,sizeof(void*),1U));});});case 2U: _LL3: _tmp70=(_tmp5D.RVar).val;_LL4:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmpE3;_tmpE3.tag=0U,({struct _dyneither_ptr _tmp109=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp70->name));_tmpE3.f1=_tmp109;});_tmpE3;});void*_tmp61[1U];_tmp61[0]=& _tmp63;({struct _dyneither_ptr _tmp10A=({const char*_tmp62="%s";_tag_dyneither(_tmp62,sizeof(char),3U);});Cyc_aprintf(_tmp10A,_tag_dyneither(_tmp61,sizeof(void*),1U));});});case 4U: _LL5: _tmp71=(_tmp5D.RType).val;_LL6:
 return Cyc_Absynpp_typ2string(_tmp71);case 3U: _LL7: _tmp72=(_tmp5D.RNumelts).val;_LL8:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmpE4;_tmpE4.tag=0U,({struct _dyneither_ptr _tmp10B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp72->name));_tmpE4.f1=_tmp10B;});_tmpE4;});void*_tmp64[1U];_tmp64[0]=& _tmp66;({struct _dyneither_ptr _tmp10C=({const char*_tmp65="numelts(%s)";_tag_dyneither(_tmp65,sizeof(char),12U);});Cyc_aprintf(_tmp10C,_tag_dyneither(_tmp64,sizeof(void*),1U));});});case 5U: _LL9: _tmp73=(_tmp5D.RParam).val;_LLA:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp69=({struct Cyc_Int_pa_PrintArg_struct _tmpE5;_tmpE5.tag=1U,_tmpE5.f1=(unsigned long)((int)_tmp73);_tmpE5;});void*_tmp67[1U];_tmp67[0]=& _tmp69;({struct _dyneither_ptr _tmp10D=({const char*_tmp68="param(%d)";_tag_dyneither(_tmp68,sizeof(char),10U);});Cyc_aprintf(_tmp10D,_tag_dyneither(_tmp67,sizeof(void*),1U));});});case 6U: _LLB: _tmp74=(_tmp5D.RParamNumelts).val;_LLC:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6C=({struct Cyc_Int_pa_PrintArg_struct _tmpE6;_tmpE6.tag=1U,_tmpE6.f1=(unsigned long)((int)_tmp74);_tmpE6;});void*_tmp6A[1U];_tmp6A[0]=& _tmp6C;({struct _dyneither_ptr _tmp10E=({const char*_tmp6B="numelts(param(%d))";_tag_dyneither(_tmp6B,sizeof(char),19U);});Cyc_aprintf(_tmp10E,_tag_dyneither(_tmp6A,sizeof(void*),1U));});});default: _LLD: _LLE:
 return(struct _dyneither_ptr)({void*_tmp6D=0U;({struct _dyneither_ptr _tmp10F=({const char*_tmp6E="return_value";_tag_dyneither(_tmp6E,sizeof(char),13U);});Cyc_aprintf(_tmp10F,_tag_dyneither(_tmp6D,sizeof(void*),0U));});});}_LL0:;}
# 359
struct _dyneither_ptr Cyc_Relations_reln2string(struct Cyc_Relations_Reln*r){
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp77=({struct Cyc_String_pa_PrintArg_struct _tmpE9;_tmpE9.tag=0U,({struct _dyneither_ptr _tmp110=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_rop2string(r->rop1));_tmpE9.f1=_tmp110;});_tmpE9;});struct Cyc_String_pa_PrintArg_struct _tmp78=({struct Cyc_String_pa_PrintArg_struct _tmpE8;_tmpE8.tag=0U,({struct _dyneither_ptr _tmp111=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_relation2string(r->relation));_tmpE8.f1=_tmp111;});_tmpE8;});struct Cyc_String_pa_PrintArg_struct _tmp79=({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0U,({
struct _dyneither_ptr _tmp112=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_rop2string(r->rop2));_tmpE7.f1=_tmp112;});_tmpE7;});void*_tmp75[3U];_tmp75[0]=& _tmp77,_tmp75[1]=& _tmp78,_tmp75[2]=& _tmp79;({struct _dyneither_ptr _tmp113=({const char*_tmp76="%s %s %s";_tag_dyneither(_tmp76,sizeof(char),9U);});Cyc_aprintf(_tmp113,_tag_dyneither(_tmp75,sizeof(void*),3U));});});}
# 364
struct _dyneither_ptr Cyc_Relations_relns2string(struct Cyc_List_List*rs){
if(rs == 0)return({const char*_tmp7A="true";_tag_dyneither(_tmp7A,sizeof(char),5U);});{
struct Cyc_List_List*ss=0;
for(0;rs != 0;rs=rs->tl){
({struct Cyc_List_List*_tmp116=({struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*_tmp7C));({struct _dyneither_ptr*_tmp115=({struct _dyneither_ptr*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct _dyneither_ptr _tmp114=Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)rs->hd);*_tmp7B=_tmp114;});_tmp7B;});_tmp7C->hd=_tmp115;}),_tmp7C->tl=ss;_tmp7C;});ss=_tmp116;});
if(rs->tl != 0)({struct Cyc_List_List*_tmp119=({struct Cyc_List_List*_tmp7F=_cycalloc(sizeof(*_tmp7F));({struct _dyneither_ptr*_tmp118=({struct _dyneither_ptr*_tmp7E=_cycalloc(sizeof(*_tmp7E));({struct _dyneither_ptr _tmp117=({const char*_tmp7D=" && ";_tag_dyneither(_tmp7D,sizeof(char),5U);});*_tmp7E=_tmp117;});_tmp7E;});_tmp7F->hd=_tmp118;}),_tmp7F->tl=ss;_tmp7F;});ss=_tmp119;});}
# 371
return(struct _dyneither_ptr)Cyc_strconcat_l(ss);};}
# 374
void Cyc_Relations_print_relns(struct Cyc___cycFILE*f,struct Cyc_List_List*r){
for(0;r != 0;r=r->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp82=({struct Cyc_String_pa_PrintArg_struct _tmpEA;_tmpEA.tag=0U,({struct _dyneither_ptr _tmp11A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Relations_reln2string((struct Cyc_Relations_Reln*)r->hd));_tmpEA.f1=_tmp11A;});_tmpEA;});void*_tmp80[1U];_tmp80[0]=& _tmp82;({struct Cyc___cycFILE*_tmp11C=f;struct _dyneither_ptr _tmp11B=({const char*_tmp81="%s";_tag_dyneither(_tmp81,sizeof(char),3U);});Cyc_fprintf(_tmp11C,_tmp11B,_tag_dyneither(_tmp80,sizeof(void*),1U));});});
if(r->tl != 0)({void*_tmp83=0U;({struct Cyc___cycFILE*_tmp11E=f;struct _dyneither_ptr _tmp11D=({const char*_tmp84=",";_tag_dyneither(_tmp84,sizeof(char),2U);});Cyc_fprintf(_tmp11E,_tmp11D,_tag_dyneither(_tmp83,sizeof(void*),0U));});});}}
# 381
static union Cyc_Relations_RelnOp Cyc_Relations_reduce_relnop(union Cyc_Relations_RelnOp rop){
{union Cyc_Relations_RelnOp _tmp85=rop;struct Cyc_Absyn_Vardecl*_tmp8F;void*_tmp8E;switch((_tmp85.RNumelts).tag){case 4U: _LL1: _tmp8E=(_tmp85.RType).val;_LL2:
# 384
{void*_tmp86=Cyc_Tcutil_compress(_tmp8E);void*_tmp87=_tmp86;struct Cyc_Absyn_Exp*_tmp88;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp87)->tag == 9U){_LL8: _tmp88=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp87)->f1;_LL9: {
# 386
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp88,& rop2))return rop2;
goto _LL7;}}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 391
goto _LL0;case 3U: _LL3: _tmp8F=(_tmp85.RNumelts).val;_LL4:
# 393
{void*_tmp89=Cyc_Tcutil_compress(_tmp8F->type);void*_tmp8A=_tmp89;void*_tmp8D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A)->tag == 3U){_LLD: _tmp8D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A)->f1).ptr_atts).bounds;_LLE:
# 397
 if(({void*_tmp11F=_tmp8D;_tmp11F == Cyc_Absyn_bounds_one();}))goto _LLC;{
struct Cyc_Absyn_Exp*_tmp8B=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp8D);
if(_tmp8B != 0){
struct Cyc_Absyn_Exp*_tmp8C=_tmp8B;
union Cyc_Relations_RelnOp rop2=rop;
if(Cyc_Relations_exp2relnop(_tmp8C,& rop2))return rop2;}
# 404
goto _LLC;};}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 407
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 410
return rop;}
# 415
static int Cyc_Relations_consist_relations(struct Cyc_Pratt_Graph*G,struct Cyc_List_List*rlns){
for(0;(unsigned int)G  && rlns != 0;rlns=rlns->tl){
struct Cyc_Relations_Reln*_tmp90=(struct Cyc_Relations_Reln*)rlns->hd;
({union Cyc_Relations_RelnOp _tmp120=Cyc_Relations_reduce_relnop(_tmp90->rop1);_tmp90->rop1=_tmp120;});
({union Cyc_Relations_RelnOp _tmp121=Cyc_Relations_reduce_relnop(_tmp90->rop2);_tmp90->rop2=_tmp121;});{
# 425
struct Cyc_Relations_Reln*_tmp91=_tmp90;union Cyc_Relations_RelnOp _tmpAB;enum Cyc_Relations_Relation _tmpAA;union Cyc_Relations_RelnOp _tmpA9;union Cyc_Relations_RelnOp _tmpA8;enum Cyc_Relations_Relation _tmpA7;unsigned int _tmpA6;union Cyc_Relations_RelnOp _tmpA5;union Cyc_Relations_RelnOp _tmpA4;union Cyc_Relations_RelnOp _tmpA3;unsigned int _tmpA2;unsigned int _tmpA1;enum Cyc_Relations_Relation _tmpA0;union Cyc_Relations_RelnOp _tmp9F;unsigned int _tmp9E;union Cyc_Relations_RelnOp _tmp9D;unsigned int _tmp9C;enum Cyc_Relations_Relation _tmp9B;unsigned int _tmp9A;if(((((struct Cyc_Relations_Reln*)_tmp91)->rop1).RConst).tag == 1){if(((((struct Cyc_Relations_Reln*)_tmp91)->rop2).RConst).tag == 1){_LL1: _tmp9C=((_tmp91->rop1).RConst).val;_tmp9B=_tmp91->relation;_tmp9A=((_tmp91->rop2).RConst).val;_LL2:
# 427
{enum Cyc_Relations_Relation _tmp92=_tmp9B;switch(_tmp92){case Cyc_Relations_Req: _LL10: _LL11:
 if(_tmp9C != _tmp9A)return 0;goto _LLF;case Cyc_Relations_Rneq: _LL12: _LL13:
 if(_tmp9C == _tmp9A)return 0;goto _LLF;case Cyc_Relations_Rlt: _LL14: _LL15:
 if(_tmp9C >= _tmp9A)return 0;goto _LLF;default: _LL16: _LL17:
 if(_tmp9C > _tmp9A)return 0;goto _LLF;}_LLF:;}
# 433
goto _LL0;}else{if(((struct Cyc_Relations_Reln*)_tmp91)->relation == Cyc_Relations_Rneq){_LL5: _tmp9E=((_tmp91->rop1).RConst).val;_tmp9D=_tmp91->rop2;_LL6: {
# 440
union Cyc_Pratt_Node _tmp93=Cyc_Relations_rop2node(_tmp9D);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
({struct Cyc_Pratt_Graph*_tmp122=Cyc_Pratt_add_edge(G2,_tmp93,Cyc_Pratt_zero_node,(int)(_tmp9E - 1));G2=_tmp122;});
({struct Cyc_Pratt_Graph*_tmp123=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp93,(int)(-(_tmp9E + 1)));G=_tmp123;});
# 450
if(G != 0  && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl) || 
Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}else{_LLB: _tmpA1=((_tmp91->rop1).RConst).val;_tmpA0=_tmp91->relation;_tmp9F=_tmp91->rop2;_LLC: {
# 484
union Cyc_Pratt_Node _tmp97=Cyc_Relations_rop2node(_tmp9F);
if(_tmpA0 == Cyc_Relations_Rlt)_tmpA1=_tmpA1 + 1;
({struct Cyc_Pratt_Graph*_tmp124=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp97,(int)(- _tmpA1));G=_tmp124;});
# 489
if((unsigned int)G  && _tmpA0 == Cyc_Relations_Req)
# 494
({struct Cyc_Pratt_Graph*_tmp125=Cyc_Pratt_add_edge(G,_tmp97,Cyc_Pratt_zero_node,(int)_tmpA1);G=_tmp125;});
# 501
goto _LL0;}}}}else{if(((struct Cyc_Relations_Reln*)_tmp91)->relation == Cyc_Relations_Rneq){if(((((struct Cyc_Relations_Reln*)_tmp91)->rop2).RConst).tag == 1){_LL3: _tmpA3=_tmp91->rop1;_tmpA2=((_tmp91->rop2).RConst).val;_LL4:
# 438
 _tmp9E=_tmpA2;_tmp9D=_tmpA3;goto _LL6;}else{_LL7: _tmpA5=_tmp91->rop1;_tmpA4=_tmp91->rop2;_LL8: {
# 457
union Cyc_Pratt_Node _tmp94=Cyc_Relations_rop2node(_tmpA5);
union Cyc_Pratt_Node _tmp95=Cyc_Relations_rop2node(_tmpA4);
struct Cyc_Pratt_Graph*G2=Cyc_Pratt_copy_graph(G);
({struct Cyc_Pratt_Graph*_tmp126=Cyc_Pratt_add_edge(G2,_tmp94,_tmp95,- 1);G2=_tmp126;});
({struct Cyc_Pratt_Graph*_tmp127=Cyc_Pratt_add_edge(G,_tmp95,_tmp94,- 1);G=_tmp127;});
if(G != 0  && G2 != 0)
return Cyc_Relations_consist_relations(G,rlns->tl) || 
Cyc_Relations_consist_relations(G2,rlns->tl);else{
if(G == 0)
G=G2;}
goto _LL0;}}}else{if(((((struct Cyc_Relations_Reln*)_tmp91)->rop2).RConst).tag == 1){_LL9: _tmpA8=_tmp91->rop1;_tmpA7=_tmp91->relation;_tmpA6=((_tmp91->rop2).RConst).val;_LLA: {
# 471
union Cyc_Pratt_Node _tmp96=Cyc_Relations_rop2node(_tmpA8);
if(_tmpA7 == Cyc_Relations_Rlt)_tmpA6=_tmpA6 - 1;
({struct Cyc_Pratt_Graph*_tmp128=Cyc_Pratt_add_edge(G,_tmp96,Cyc_Pratt_zero_node,(int)_tmpA6);G=_tmp128;});
# 475
if((unsigned int)G  && _tmpA7 == Cyc_Relations_Req)
({struct Cyc_Pratt_Graph*_tmp129=Cyc_Pratt_add_edge(G,Cyc_Pratt_zero_node,_tmp96,(int)(- _tmpA6));G=_tmp129;});
goto _LL0;}}else{_LLD: _tmpAB=_tmp91->rop1;_tmpAA=_tmp91->relation;_tmpA9=_tmp91->rop2;_LLE: {
# 504
union Cyc_Pratt_Node _tmp98=Cyc_Relations_rop2node(_tmpAB);
union Cyc_Pratt_Node _tmp99=Cyc_Relations_rop2node(_tmpA9);
int i=_tmpAA == Cyc_Relations_Rlt?- 1: 0;
# 509
({struct Cyc_Pratt_Graph*_tmp12A=Cyc_Pratt_add_edge(G,_tmp98,_tmp99,i);G=_tmp12A;});
# 511
if((unsigned int)G  && _tmpAA == Cyc_Relations_Req)
({struct Cyc_Pratt_Graph*_tmp12B=Cyc_Pratt_add_edge(G,_tmp99,_tmp98,i);G=_tmp12B;});
goto _LL0;}}}}_LL0:;};}
# 516
if(G != 0)return 1;else{return 0;}}
# 524
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns){
# 530
struct Cyc_Pratt_Graph*G=Cyc_Pratt_empty_graph();
return Cyc_Relations_consist_relations(G,rlns);}
# 538
struct _tuple10 Cyc_Relations_primop2relation(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e2){
# 540
enum Cyc_Absyn_Primop _tmpAC=p;switch(_tmpAC){case Cyc_Absyn_Eq: _LL1: _LL2:
 return({struct _tuple10 _tmpEB;_tmpEB.f1=e1,_tmpEB.f2=Cyc_Relations_Req,_tmpEB.f3=e2;_tmpEB;});case Cyc_Absyn_Neq: _LL3: _LL4:
 return({struct _tuple10 _tmpEC;_tmpEC.f1=e1,_tmpEC.f2=Cyc_Relations_Rneq,_tmpEC.f3=e2;_tmpEC;});case Cyc_Absyn_Lt: _LL5: _LL6:
 return({struct _tuple10 _tmpED;_tmpED.f1=e1,_tmpED.f2=Cyc_Relations_Rlt,_tmpED.f3=e2;_tmpED;});case Cyc_Absyn_Lte: _LL7: _LL8:
 return({struct _tuple10 _tmpEE;_tmpEE.f1=e1,_tmpEE.f2=Cyc_Relations_Rlte,_tmpEE.f3=e2;_tmpEE;});case Cyc_Absyn_Gt: _LL9: _LLA:
 return({struct _tuple10 _tmpEF;_tmpEF.f1=e2,_tmpEF.f2=Cyc_Relations_Rlt,_tmpEF.f3=e1;_tmpEF;});case Cyc_Absyn_Gte: _LLB: _LLC:
 return({struct _tuple10 _tmpF0;_tmpF0.f1=e2,_tmpF0.f2=Cyc_Relations_Rlte,_tmpF0.f3=e1;_tmpF0;});default: _LLD: _LLE:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp12C=({const char*_tmpAD="primop2relation";_tag_dyneither(_tmpAD,sizeof(char),16U);});_tmpAE->f1=_tmp12C;});_tmpAE;}));}_LL0:;}
# 552
enum Cyc_Relations_Relation Cyc_Relations_flip_relation(enum Cyc_Relations_Relation r){
enum Cyc_Relations_Relation _tmpAF=r;switch(_tmpAF){case Cyc_Relations_Req: _LL1: _LL2:
 return Cyc_Relations_Rneq;case Cyc_Relations_Rneq: _LL3: _LL4:
 return Cyc_Relations_Req;case Cyc_Relations_Rlt: _LL5: _LL6:
 return Cyc_Relations_Rlte;default: _LL7: _LL8:
 return Cyc_Relations_Rlt;}_LL0:;}
# 561
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*r,struct Cyc_Relations_Reln*rln){
return({struct Cyc_Relations_Reln*_tmpB0=_region_malloc(r,sizeof(*_tmpB0));_tmpB0->rop1=rln->rop2,({enum Cyc_Relations_Relation _tmp12D=Cyc_Relations_flip_relation(rln->relation);_tmpB0->relation=_tmp12D;}),_tmpB0->rop2=rln->rop1;_tmpB0;});}
# 572
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
# 574
{void*_tmpB1=e->r;void*_tmpB2=_tmpB1;enum Cyc_Absyn_Primop _tmpCE;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;enum Cyc_Absyn_Primop _tmpCB;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;void*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;switch(*((int*)_tmpB2)){case 14U: _LL1: _tmpC6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmpC5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LL2:
# 577
{void*_tmpB3=Cyc_Tcutil_compress(_tmpC6);void*_tmpB4=_tmpB3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB4)->f1)->f2 == Cyc_Absyn_Int_sz){_LLC: _LLD:
 return Cyc_Relations_exp2relns(r,_tmpC5);}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _LLF:
 goto _LLB;}_LLB:;}
# 581
goto _LL0;case 7U: _LL3: _tmpC8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmpC7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LL4:
 return({struct _RegionHandle*_tmp12F=r;struct Cyc_List_List*_tmp12E=Cyc_Relations_exp2relns(r,_tmpC8);((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp12F,_tmp12E,Cyc_Relations_exp2relns(r,_tmpC7));});case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f1 == Cyc_Absyn_Not){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2 != 0){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd)->r)->tag == 3U){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd)->r)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd)->r)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd)->r)->f2)->tl)->tl == 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl == 0){_LL5: _tmpCB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd)->r)->f1;_tmpCA=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd)->r)->f2)->hd;_tmpC9=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd)->r)->f2)->tl)->hd;_LL6:
# 584
{enum Cyc_Absyn_Primop _tmpB5=_tmpCB;switch(_tmpB5){case Cyc_Absyn_Eq: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Neq: _LL13: _LL14: goto _LL16;case Cyc_Absyn_Lt: _LL15: _LL16: goto _LL18;case Cyc_Absyn_Lte: _LL17: _LL18: goto _LL1A;case Cyc_Absyn_Gt: _LL19: _LL1A: goto _LL1C;case Cyc_Absyn_Gte: _LL1B: _LL1C: {
struct _tuple10 _tmpB6=Cyc_Relations_primop2relation(_tmpCA,_tmpCB,_tmpC9);struct _tuple10 _tmpB7=_tmpB6;struct Cyc_Absyn_Exp*_tmpBC;enum Cyc_Relations_Relation _tmpBB;struct Cyc_Absyn_Exp*_tmpBA;_LL20: _tmpBC=_tmpB7.f1;_tmpBB=_tmpB7.f2;_tmpBA=_tmpB7.f3;_LL21:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpBC,& rop1) && Cyc_Relations_exp2relnop(_tmpBA,& rop2))
return({struct Cyc_List_List*_tmpB9=_region_malloc(r,sizeof(*_tmpB9));({struct Cyc_Relations_Reln*_tmp131=({struct Cyc_Relations_Reln*_tmpB8=_region_malloc(r,sizeof(*_tmpB8));_tmpB8->rop1=rop2,({enum Cyc_Relations_Relation _tmp130=Cyc_Relations_flip_relation(_tmpBB);_tmpB8->relation=_tmp130;}),_tmpB8->rop2=rop1;_tmpB8;});_tmpB9->hd=_tmp131;}),_tmpB9->tl=0;_tmpB9;});
goto _LL10;};}default: _LL1D: _LL1E:
 goto _LL10;}_LL10:;}
# 594
goto _LL0;}else{if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl)->tl == 0)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl)->tl == 0){_LL7: _tmpCE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_tmpCD=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->hd;_tmpCC=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB2)->f2)->tl)->hd;_LL8:
# 596
{enum Cyc_Absyn_Primop _tmpBD=_tmpCE;switch(_tmpBD){case Cyc_Absyn_Eq: _LL23: _LL24:
 goto _LL26;case Cyc_Absyn_Neq: _LL25: _LL26: goto _LL28;case Cyc_Absyn_Lt: _LL27: _LL28: goto _LL2A;case Cyc_Absyn_Lte: _LL29: _LL2A: goto _LL2C;case Cyc_Absyn_Gt: _LL2B: _LL2C: goto _LL2E;case Cyc_Absyn_Gte: _LL2D: _LL2E: {
struct _tuple10 _tmpBE=Cyc_Relations_primop2relation(_tmpCD,_tmpCE,_tmpCC);struct _tuple10 _tmpBF=_tmpBE;struct Cyc_Absyn_Exp*_tmpC4;enum Cyc_Relations_Relation _tmpC3;struct Cyc_Absyn_Exp*_tmpC2;_LL32: _tmpC4=_tmpBF.f1;_tmpC3=_tmpBF.f2;_tmpC2=_tmpBF.f3;_LL33:;{
union Cyc_Relations_RelnOp rop1=Cyc_Relations_RConst(0U);
union Cyc_Relations_RelnOp rop2=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmpC4,& rop1) && Cyc_Relations_exp2relnop(_tmpC2,& rop2))
return({struct Cyc_List_List*_tmpC1=_region_malloc(r,sizeof(*_tmpC1));({struct Cyc_Relations_Reln*_tmp132=({struct Cyc_Relations_Reln*_tmpC0=_region_malloc(r,sizeof(*_tmpC0));_tmpC0->rop1=rop1,_tmpC0->relation=_tmpC3,_tmpC0->rop2=rop2;_tmpC0;});_tmpC1->hd=_tmp132;}),_tmpC1->tl=0;_tmpC1;});
goto _LL22;};}default: _LL2F: _LL30:
 goto _LL22;}_LL22:;}
# 606
goto _LL0;}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}}default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 610
if(Cyc_Tcutil_is_const_exp(e)){
struct _tuple11 _tmpCF=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple11 _tmpD0=_tmpCF;unsigned int _tmpD7;int _tmpD6;_LL35: _tmpD7=_tmpD0.f1;_tmpD6=_tmpD0.f2;_LL36:;
if(_tmpD6){
if((int)_tmpD7)return 0;else{
# 615
return({struct Cyc_List_List*_tmpD2=_region_malloc(r,sizeof(*_tmpD2));({struct Cyc_Relations_Reln*_tmp135=({struct Cyc_Relations_Reln*_tmpD1=_region_malloc(r,sizeof(*_tmpD1));({union Cyc_Relations_RelnOp _tmp134=Cyc_Relations_RConst(0U);_tmpD1->rop1=_tmp134;}),_tmpD1->relation=Cyc_Relations_Rlt,({union Cyc_Relations_RelnOp _tmp133=Cyc_Relations_RConst(0U);_tmpD1->rop2=_tmp133;});_tmpD1;});_tmpD2->hd=_tmp135;}),_tmpD2->tl=0;_tmpD2;});}}
# 617
({struct Cyc_String_pa_PrintArg_struct _tmpD5=({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0U,({
struct _dyneither_ptr _tmp136=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmpF1.f1=_tmp136;});_tmpF1;});void*_tmpD3[1U];_tmpD3[0]=& _tmpD5;({unsigned int _tmp138=e->loc;struct _dyneither_ptr _tmp137=({const char*_tmpD4="unable to convert `%s' to static relation";_tag_dyneither(_tmpD4,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp138,_tmp137,_tag_dyneither(_tmpD3,sizeof(void*),1U));});});}
# 621
return({struct Cyc_List_List*_tmpD9=_region_malloc(r,sizeof(*_tmpD9));({struct Cyc_Relations_Reln*_tmp13B=({struct Cyc_Relations_Reln*_tmpD8=_region_malloc(r,sizeof(*_tmpD8));({union Cyc_Relations_RelnOp _tmp13A=Cyc_Relations_RConst(0U);_tmpD8->rop1=_tmp13A;}),_tmpD8->relation=Cyc_Relations_Rlt,({union Cyc_Relations_RelnOp _tmp139=Cyc_Relations_RConst(0U);_tmpD8->rop2=_tmp139;});_tmpD8;});_tmpD9->hd=_tmp13B;}),_tmpD9->tl=0;_tmpD9;});}
