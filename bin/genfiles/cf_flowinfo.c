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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 147 "dict.h"
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 214
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 224
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 287
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 892 "absyn.h"
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 941
extern void*Cyc_Absyn_void_type;
# 1170
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);
# 1172
int Cyc_Absyn_is_nontagged_nonrequire_union_type(void*);struct Cyc_RgnOrder_RgnPO;
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
# 62
int Cyc_Tcutil_is_bits_only_type(void*t);
# 112
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
# 115
void*Cyc_Tcutil_compress(void*t);
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 204
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 217
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 305 "tcutil.h"
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 388
int Cyc_Tcutil_force_type2bool(int desired,void*t);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
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
void*Cyc_Warn_impos_loc(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 113
int Cyc_Relations_relns_approx(struct Cyc_List_List*r2s,struct Cyc_List_List*r1s);
# 115
struct Cyc_List_List*Cyc_Relations_join_relns(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*);
# 117
void Cyc_Relations_print_relns(struct Cyc___cycFILE*,struct Cyc_List_List*);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};char Cyc_CfFlowInfo_IsZero[7U]="IsZero";struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};char Cyc_CfFlowInfo_NotZero[8U]="NotZero";struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};char Cyc_CfFlowInfo_UnknownZ[9U]="UnknownZ";struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};
# 87
extern struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val;struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 94
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*);
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL();struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple10{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple10 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 138 "cf_flowinfo.h"
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL();
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict,struct Cyc_List_List*);struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 153
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env();
# 155
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f);
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f);
# 158
int Cyc_CfFlowInfo_root_cmp(void*,void*);
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*);
# 161
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_List_List*,int no_init_bits_only,void*);
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*,void*t,int no_init_bits_only,void*leafval);
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*);
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume);
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r);struct _tuple11{void*f1;struct Cyc_List_List*f2;};
struct _tuple11 Cyc_CfFlowInfo_unname_rval(void*rv);
# 168
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*,struct Cyc_Dict_Dict d,void*r);
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place);
int Cyc_CfFlowInfo_is_init_pointer(void*r);
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2);
# 174
void Cyc_CfFlowInfo_print_absrval(void*rval);
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il);
void Cyc_CfFlowInfo_print_root(void*root);
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p);
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p);
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*p,void(*pr)(void*));
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d);
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f);
# 196 "cf_flowinfo.h"
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r);
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r);
# 201
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*,union Cyc_CfFlowInfo_FlowInfo,union Cyc_CfFlowInfo_FlowInfo);struct _tuple12{union Cyc_CfFlowInfo_FlowInfo f1;void*f2;};
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*,struct _tuple12 pr1,struct _tuple12 pr2);
# 206
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place);
# 38 "cf_flowinfo.cyc"
int Cyc_CfFlowInfo_anal_error=0;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 41
Cyc_CfFlowInfo_anal_error=1;
Cyc_Warn_verr(loc,fmt,ap);}
# 46
struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct Cyc_CfFlowInfo_IsZero_val={Cyc_CfFlowInfo_IsZero};
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_PlaceL(struct Cyc_CfFlowInfo_Place*x){
return({union Cyc_CfFlowInfo_AbsLVal _tmp220;(_tmp220.PlaceL).tag=1U,(_tmp220.PlaceL).val=x;_tmp220;});}
# 50
union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_UnknownL(){
return({union Cyc_CfFlowInfo_AbsLVal _tmp221;(_tmp221.UnknownL).tag=2U,(_tmp221.UnknownL).val=0;_tmp221;});}
# 53
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_BottomFL(){return({union Cyc_CfFlowInfo_FlowInfo _tmp222;(_tmp222.BottomFL).tag=1U,(_tmp222.BottomFL).val=0;_tmp222;});}
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_ReachableFL(struct Cyc_Dict_Dict fd,struct Cyc_List_List*r){
return({union Cyc_CfFlowInfo_FlowInfo _tmp223;(_tmp223.ReachableFL).tag=2U,((_tmp223.ReachableFL).val).f1=fd,((_tmp223.ReachableFL).val).f2=r;_tmp223;});}
# 58
struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_new_flow_env(){
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct dummy_rawexp={0U,{.Null_c={1,0}}};
static struct Cyc_Absyn_Exp dummy_exp={0,(void*)& dummy_rawexp,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp8=_cycalloc(sizeof(*_tmp8));
({void*_tmp25D=(void*)({struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->tag=0U;_tmp0;});_tmp8->zero=_tmp25D;}),({
void*_tmp25C=(void*)({struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=1U;_tmp1;});_tmp8->notzeroall=_tmp25C;}),({
void*_tmp25B=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=2U,_tmp2->f1=Cyc_CfFlowInfo_NoneIL;_tmp2;});_tmp8->unknown_none=_tmp25B;}),({
void*_tmp25A=(void*)({struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=2U,_tmp3->f1=Cyc_CfFlowInfo_AllIL;_tmp3;});_tmp8->unknown_all=_tmp25A;}),({
void*_tmp259=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=3U,_tmp4->f1=Cyc_CfFlowInfo_NoneIL;_tmp4;});_tmp8->esc_none=_tmp259;}),({
void*_tmp258=(void*)({struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=3U,_tmp5->f1=Cyc_CfFlowInfo_AllIL;_tmp5;});_tmp8->esc_all=_tmp258;}),({
struct Cyc_Dict_Dict _tmp257=((struct Cyc_Dict_Dict(*)(int(*cmp)(void*,void*)))Cyc_Dict_empty)(Cyc_CfFlowInfo_root_cmp);_tmp8->mt_flowdict=_tmp257;}),({
struct Cyc_CfFlowInfo_Place*_tmp256=({struct Cyc_CfFlowInfo_Place*_tmp7=_cycalloc(sizeof(*_tmp7));({void*_tmp255=(void*)({struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*_tmp6=_cycalloc(sizeof(*_tmp6));_tmp6->tag=1U,_tmp6->f1=& dummy_exp,_tmp6->f2=Cyc_Absyn_void_type;_tmp6;});_tmp7->root=_tmp255;}),_tmp7->path=0;_tmp7;});_tmp8->dummy_place=_tmp256;});_tmp8;});}
# 74
struct _dyneither_ptr Cyc_CfFlowInfo_place_err_string(struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmp9=*place;struct Cyc_CfFlowInfo_Place _tmpA=_tmp9;void*_tmp13;struct Cyc_List_List*_tmp12;_LL1: _tmp13=_tmpA.root;_tmp12=_tmpA.path;_LL2:;{
void*_tmpB=_tmp13;struct Cyc_Absyn_Vardecl*_tmp11;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->tag == 0U){_LL4: _tmp11=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmpB)->f1;_LL5:
# 78
 if(_tmp12 == 0)
return Cyc_Absynpp_qvar2string(_tmp11->name);else{
# 81
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp224;_tmp224.tag=0U,({struct _dyneither_ptr _tmp25E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp11->name));_tmp224.f1=_tmp25E;});_tmp224;});void*_tmpC[1U];_tmpC[0]=& _tmpE;({struct _dyneither_ptr _tmp25F=({const char*_tmpD="reachable from %s";_tag_dyneither(_tmpD,sizeof(char),18U);});Cyc_aprintf(_tmp25F,_tag_dyneither(_tmpC,sizeof(void*),1U));});});}}else{_LL6: _LL7:
({void*_tmpF=0U;({struct _dyneither_ptr _tmp260=({const char*_tmp10="error locations not for VarRoots";_tag_dyneither(_tmp10,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp260,_tag_dyneither(_tmpF,sizeof(void*),0U));});});}_LL3:;};}
# 87
int Cyc_CfFlowInfo_get_field_index_fs(struct Cyc_List_List*fs,struct _dyneither_ptr*f){
int n=0;
for(0;(unsigned int)fs;fs=fs->tl){
struct _dyneither_ptr*_tmp14=((struct Cyc_Absyn_Aggrfield*)fs->hd)->name;
if(Cyc_strptrcmp(_tmp14,f)== 0)return n;
n=n + 1;}
# 94
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp225;_tmp225.tag=0U,_tmp225.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp225;});void*_tmp15[1U];_tmp15[0]=& _tmp17;({struct _dyneither_ptr _tmp261=({const char*_tmp16="get_field_index_fs failed: %s";_tag_dyneither(_tmp16,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp261,_tag_dyneither(_tmp15,sizeof(void*),1U));});});}
# 97
int Cyc_CfFlowInfo_get_field_index(void*t,struct _dyneither_ptr*f){
void*_tmp18=Cyc_Tcutil_compress(t);void*_tmp19=_tmp18;struct Cyc_List_List*_tmp1F;union Cyc_Absyn_AggrInfo _tmp1E;switch(*((int*)_tmp19)){case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)->tag == 20U){_LL1: _tmp1E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19)->f1)->f1;_LL2: {
# 100
struct Cyc_List_List*_tmp1A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((Cyc_Absyn_get_known_aggrdecl(_tmp1E))->impl))->fields;
_tmp1F=_tmp1A;goto _LL4;}}else{goto _LL5;}case 7U: _LL3: _tmp1F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp19)->f2;_LL4:
# 103
 return Cyc_CfFlowInfo_get_field_index_fs(_tmp1F,f);default: _LL5: _LL6:
# 105
({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0U,({struct _dyneither_ptr _tmp262=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp226.f1=_tmp262;});_tmp226;});void*_tmp1B[1U];_tmp1B[0]=& _tmp1D;({struct _dyneither_ptr _tmp263=({const char*_tmp1C="get_field_index failed: %s";_tag_dyneither(_tmp1C,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp263,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});}_LL0:;}struct _tuple13{void*f1;void*f2;};
# 109
int Cyc_CfFlowInfo_root_cmp(void*r1,void*r2){
if((int)r1 == (int)r2)
return 0;{
struct _tuple13 _tmp20=({struct _tuple13 _tmp227;_tmp227.f1=r1,_tmp227.f2=r2;_tmp227;});struct _tuple13 _tmp21=_tmp20;int _tmp27;int _tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Vardecl*_tmp23;struct Cyc_Absyn_Vardecl*_tmp22;if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f1)->tag == 0U){if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 0U){_LL1: _tmp23=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp22=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;_LL2:
 return(int)_tmp23 - (int)_tmp22;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f1)->tag == 1U){if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 1U){_LL7: _tmp25=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp24=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;_LL8:
 return(int)_tmp25 - (int)_tmp24;}else{_LL9: _LLA:
 return - 1;}}else{if(((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp21.f2)->tag == 1U){_LLB: _LLC:
 return 1;}else{_LLD: _tmp27=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f1)->f1;_tmp26=((struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct*)_tmp21.f2)->f1;_LLE:
 return _tmp27 - _tmp26;}}}}_LL0:;};}
# 123
static int Cyc_CfFlowInfo_pathcon_cmp(void*p1,void*p2){
struct _tuple13 _tmp28=({struct _tuple13 _tmp228;_tmp228.f1=p1,_tmp228.f2=p2;_tmp228;});struct _tuple13 _tmp29=_tmp28;int _tmp2B;int _tmp2A;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f1)->tag == 0U){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f2)->tag == 0U){_LL1: _tmp2B=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f1)->f1;_tmp2A=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp29.f2)->f1;_LL2:
# 126
 if(_tmp2B == _tmp2A)return 0;
if(_tmp2B < _tmp2A)return - 1;else{
return 1;}}else{_LL7: _LL8:
# 131
 return 1;}}else{if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmp29.f2)->tag == 1U){_LL3: _LL4:
# 129
 return 0;}else{_LL5: _LL6:
 return - 1;}}_LL0:;}
# 135
static int Cyc_CfFlowInfo_path_cmp(struct Cyc_List_List*path1,struct Cyc_List_List*path2){
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_CfFlowInfo_pathcon_cmp,path1,path2);}
# 139
int Cyc_CfFlowInfo_place_cmp(struct Cyc_CfFlowInfo_Place*p1,struct Cyc_CfFlowInfo_Place*p2){
if((int)p1 == (int)p2)
return 0;{
struct Cyc_CfFlowInfo_Place _tmp2C=*p1;struct Cyc_CfFlowInfo_Place _tmp2D=_tmp2C;void*_tmp33;struct Cyc_List_List*_tmp32;_LL1: _tmp33=_tmp2D.root;_tmp32=_tmp2D.path;_LL2:;{
struct Cyc_CfFlowInfo_Place _tmp2E=*p2;struct Cyc_CfFlowInfo_Place _tmp2F=_tmp2E;void*_tmp31;struct Cyc_List_List*_tmp30;_LL4: _tmp31=_tmp2F.root;_tmp30=_tmp2F.path;_LL5:;{
int i=Cyc_CfFlowInfo_root_cmp(_tmp33,_tmp31);
if(i != 0)
return i;
return Cyc_CfFlowInfo_path_cmp(_tmp32,_tmp30);};};};}
# 160 "cf_flowinfo.cyc"
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval);
# 165
struct _dyneither_ptr Cyc_CfFlowInfo_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 171
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fs);
struct _dyneither_ptr d=({unsigned int _tmp39=sz;void**_tmp38=_cycalloc(_check_times(_tmp39,sizeof(void*)));({{unsigned int _tmp229=sz;unsigned int i;for(i=0;i < _tmp229;++ i){_tmp38[i]=fenv->unknown_all;}}0;});_tag_dyneither(_tmp38,sizeof(void*),_tmp39);});
{int i=0;for(0;i < sz;(i ++,fs=fs->tl)){
struct Cyc_Absyn_Aggrfield*_tmp34=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(fs))->hd;struct Cyc_Absyn_Aggrfield*_tmp35=_tmp34;struct _dyneither_ptr*_tmp37;void*_tmp36;_LL1: _tmp37=_tmp35->name;_tmp36=_tmp35->type;_LL2:;
if(_get_dyneither_size(*_tmp37,sizeof(char))!= 1)
({void*_tmp264=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,_tmp36,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp264;});}}
# 179
return d;}struct _tuple14{struct _RegionHandle*f1;struct Cyc_List_List*f2;};
# 183
static struct Cyc_Absyn_Aggrfield*Cyc_CfFlowInfo_substitute_field(struct _tuple14*env,struct Cyc_Absyn_Aggrfield*f){
# 187
struct _tuple14*_tmp3A=env;struct _RegionHandle*_tmp40;struct Cyc_List_List*_tmp3F;_LL1: _tmp40=_tmp3A->f1;_tmp3F=_tmp3A->f2;_LL2:;{
void*_tmp3B=Cyc_Tcutil_rsubstitute(_tmp40,_tmp3F,f->type);
struct Cyc_Absyn_Exp*_tmp3C=f->requires_clause;
struct Cyc_Absyn_Exp*_tmp3D=_tmp3C == 0?0: Cyc_Tcutil_rsubsexp(_tmp40,_tmp3F,_tmp3C);
return({struct Cyc_Absyn_Aggrfield*_tmp3E=_region_malloc(_tmp40,sizeof(*_tmp3E));_tmp3E->name=f->name,_tmp3E->tq=f->tq,_tmp3E->type=_tmp3B,_tmp3E->width=f->width,_tmp3E->attributes=f->attributes,_tmp3E->requires_clause=_tmp3D;_tmp3E;});};}struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 198
static struct _dyneither_ptr Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_List_List*tvs,struct Cyc_List_List*targs,struct Cyc_List_List*fs,int no_init_bits_only,void*leafval){
# 204
struct _RegionHandle _tmp41=_new_region("temp");struct _RegionHandle*temp=& _tmp41;_push_region(temp);
# 208
{struct Cyc_List_List*inst=0;
for(0;tvs != 0;(tvs=tvs->tl,targs=targs->tl)){
struct Cyc_Absyn_Tvar*_tmp42=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp43=(void*)((struct Cyc_List_List*)_check_null(targs))->hd;
{struct Cyc_Absyn_Kind*_tmp44=Cyc_Tcutil_tvar_kind(_tmp42,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp45=_tmp44;switch(((struct Cyc_Absyn_Kind*)_tmp45)->kind){case Cyc_Absyn_RgnKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_EffKind: _LL3: _LL4:
 continue;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 217
({struct Cyc_List_List*_tmp266=({struct Cyc_List_List*_tmp47=_region_malloc(temp,sizeof(*_tmp47));({struct _tuple15*_tmp265=({struct _tuple15*_tmp46=_region_malloc(temp,sizeof(*_tmp46));_tmp46->f1=_tmp42,_tmp46->f2=_tmp43;_tmp46;});_tmp47->hd=_tmp265;}),_tmp47->tl=inst;_tmp47;});inst=_tmp266;});}
# 219
if(inst != 0){
struct _tuple14 _tmp48=({struct _tuple14 _tmp22A;_tmp22A.f1=temp,({struct Cyc_List_List*_tmp267=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);_tmp22A.f2=_tmp267;});_tmp22A;});
struct Cyc_List_List*subs_fs=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Aggrfield*(*f)(struct _tuple14*,struct Cyc_Absyn_Aggrfield*),struct _tuple14*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(temp,Cyc_CfFlowInfo_substitute_field,& _tmp48,fs);
struct _dyneither_ptr _tmp49=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,subs_fs,no_init_bits_only,leafval);_npop_handler(0U);return _tmp49;}else{
# 224
struct _dyneither_ptr _tmp4A=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,fs,no_init_bits_only,leafval);_npop_handler(0U);return _tmp4A;}}
# 208
;_pop_region(temp);}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 228
static void*Cyc_CfFlowInfo_i_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,int allow_zeroterm,int no_init_bits_only,void*t,void*leafval){
# 234
if(Cyc_Absyn_is_nontagged_nonrequire_union_type(t))return fenv->unknown_all;
# 236
{void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;void*_tmp5E;void*_tmp5D;void*_tmp5C;enum Cyc_Absyn_AggrKind _tmp5B;struct Cyc_List_List*_tmp5A;struct Cyc_List_List*_tmp59;void*_tmp58;union Cyc_Absyn_AggrInfo _tmp57;struct Cyc_List_List*_tmp56;struct Cyc_Absyn_Datatypefield*_tmp55;switch(*((int*)_tmp4C)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)){case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)->f1).KnownDatatypefield).tag == 2){_LL1: _tmp55=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)->f1).KnownDatatypefield).val).f2;_LL2:
# 238
 if(_tmp55->typs == 0)
return leafval;
_tmp59=_tmp55->typs;goto _LL4;}else{goto _LLF;}case 20U: _LL5: _tmp57=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f1)->f1;_tmp56=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f2;_LL6: {
# 250
struct Cyc_Absyn_Aggrdecl*_tmp50=Cyc_Absyn_get_known_aggrdecl(_tmp57);
if(_tmp50->impl == 0)
goto _LL0;{
struct Cyc_List_List*_tmp51=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp50->impl))->fields;
if(_tmp56 == 0){_tmp5B=_tmp50->kind;_tmp5A=_tmp51;goto _LL8;}
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->tag=6U,(_tmp52->f1).is_union=_tmp50->kind == Cyc_Absyn_UnionA,(_tmp52->f1).fieldnum=- 1,({
struct _dyneither_ptr _tmp268=Cyc_CfFlowInfo_substitute_aggrfields_to_aggrdict(fenv,_tmp50->tvs,_tmp56,_tmp51,_tmp50->kind == Cyc_Absyn_UnionA,leafval);_tmp52->f2=_tmp268;});_tmp52;});};}case 4U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f2 != 0){_LLB: _tmp58=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C)->f2)->hd;_LLC:
# 273
 return leafval;}else{goto _LLF;}default: goto _LLF;}case 6U: _LL3: _tmp59=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4C)->f1;_LL4: {
# 242
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp59);
struct _dyneither_ptr d=({unsigned int _tmp4F=sz;void**_tmp4E=_cycalloc(_check_times(_tmp4F,sizeof(void*)));({{unsigned int _tmp22B=sz;unsigned int i;for(i=0;i < _tmp22B;++ i){_tmp4E[i]=fenv->unknown_all;}}0;});_tag_dyneither(_tmp4E,sizeof(void*),_tmp4F);});
{int i=0;for(0;i < sz;++ i){
({void*_tmp269=Cyc_CfFlowInfo_i_typ_to_absrval(fenv,0,no_init_bits_only,(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp59))->hd)).f2,leafval);*((void**)_check_dyneither_subscript(d,sizeof(void*),i))=_tmp269;});
_tmp59=_tmp59->tl;}}
# 248
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->tag=6U,(_tmp4D->f1).is_union=0,(_tmp4D->f1).fieldnum=- 1,_tmp4D->f2=d;_tmp4D;});}case 7U: _LL7: _tmp5B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f1;_tmp5A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f2;_LL8:
# 265
 return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->tag=6U,(_tmp53->f1).is_union=_tmp5B == Cyc_Absyn_UnionA,(_tmp53->f1).fieldnum=- 1,({
struct _dyneither_ptr _tmp26A=Cyc_CfFlowInfo_aggrfields_to_aggrdict(fenv,_tmp5A,_tmp5B == Cyc_Absyn_UnionA,leafval);_tmp53->f2=_tmp26A;});_tmp53;});case 4U: _LL9: _tmp5D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_tmp5C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zero_term;_LLA:
# 268
 if(Cyc_Tcutil_force_type2bool(0,_tmp5C))
# 271
return(allow_zeroterm  && !no_init_bits_only) && Cyc_Tcutil_is_bits_only_type(_tmp5D)?fenv->unknown_all: leafval;
goto _LL0;case 3U: _LLD: _tmp5E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_LLE:
# 275
 if(!Cyc_Tcutil_force_type2bool(0,_tmp5E)){
void*_tmp54=leafval;if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54)->tag == 2U){if(((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp54)->f1 == Cyc_CfFlowInfo_AllIL){_LL12: _LL13:
 return fenv->notzeroall;}else{goto _LL14;}}else{_LL14: _LL15:
 goto _LL11;}_LL11:;}
# 280
goto _LL0;default: _LLF: _LL10:
 goto _LL0;}_LL0:;}
# 284
return !no_init_bits_only  && Cyc_Tcutil_is_bits_only_type(t)?fenv->unknown_all: leafval;}
# 287
void*Cyc_CfFlowInfo_typ_to_absrval(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,int no_init_bits_only,void*leafval){
return Cyc_CfFlowInfo_i_typ_to_absrval(fenv,1,no_init_bits_only,t,leafval);}
# 293
int Cyc_CfFlowInfo_is_unique_consumed(struct Cyc_Absyn_Exp*e,int env_iteration,void*r,int*needs_unconsume){
void*_tmp5F=r;void*_tmp67;int _tmp66;int _tmp65;struct _dyneither_ptr _tmp64;struct Cyc_Absyn_Exp*_tmp63;int _tmp62;void*_tmp61;switch(*((int*)_tmp5F)){case 7U: _LL1: _tmp63=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1;_tmp62=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;_tmp61=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f3;_LL2:
# 296
 if(_tmp63 == e  && _tmp62 == env_iteration){
*needs_unconsume=1;
return 0;}
# 300
return 1;case 6U: _LL3: _tmp66=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1).is_union;_tmp65=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f1).fieldnum;_tmp64=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;_LL4:
# 302
 if(!_tmp66  || _tmp65 == - 1){
unsigned int _tmp60=_get_dyneither_size(_tmp64,sizeof(void*));
{int i=0;for(0;i < _tmp60;++ i){
if(Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmp64,sizeof(void*),i)),needs_unconsume))
return 1;}}
# 308
return 0;}else{
# 311
return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,*((void**)_check_dyneither_subscript(_tmp64,sizeof(void*),_tmp65)),needs_unconsume);}case 8U: _LL5: _tmp67=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp5F)->f2;_LL6:
# 313
 return Cyc_CfFlowInfo_is_unique_consumed(e,env_iteration,_tmp67,needs_unconsume);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 320
void*Cyc_CfFlowInfo_make_unique_unconsumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*r){
void*_tmp68=r;struct Cyc_Absyn_Vardecl*_tmp75;void*_tmp74;struct Cyc_CfFlowInfo_UnionRInfo _tmp73;struct _dyneither_ptr _tmp72;struct Cyc_Absyn_Exp*_tmp71;int _tmp70;void*_tmp6F;switch(*((int*)_tmp68)){case 7U: _LL1: _tmp71=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp68)->f1;_tmp70=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp68)->f2;_tmp6F=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp68)->f3;_LL2:
# 323
 return _tmp6F;case 6U: _LL3: _tmp73=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp68)->f1;_tmp72=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp68)->f2;_LL4: {
# 325
unsigned int _tmp69=_get_dyneither_size(_tmp72,sizeof(void*));
int change=0;
struct _dyneither_ptr d2=({unsigned int _tmp6C=_tmp69;void**_tmp6B=_cycalloc(_check_times(_tmp6C,sizeof(void*)));({{unsigned int _tmp22C=_tmp69;unsigned int i;for(i=0;i < _tmp22C;++ i){_tmp6B[i]=*((void**)_check_dyneither_subscript(_tmp72,sizeof(void*),(int)i));}}0;});_tag_dyneither(_tmp6B,sizeof(void*),_tmp6C);});
{int i=0;for(0;i < _tmp69;++ i){
({void*_tmp26B=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,*((void**)_check_dyneither_subscript(_tmp72,sizeof(void*),i)));*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp26B;});
if(({void*_tmp26C=*((void**)_check_dyneither_subscript(d2,sizeof(void*),i));_tmp26C != *((void**)_check_dyneither_subscript(_tmp72,sizeof(void*),i));}))
change=1;}}
# 333
if(change)
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A->tag=6U,_tmp6A->f1=_tmp73,_tmp6A->f2=d2;_tmp6A;});else{
return r;}}case 8U: _LL5: _tmp75=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp68)->f1;_tmp74=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp68)->f2;_LL6: {
# 337
void*_tmp6D=Cyc_CfFlowInfo_make_unique_unconsumed(fenv,_tmp74);
if(_tmp6D != _tmp74)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=8U,_tmp6E->f1=_tmp75,_tmp6E->f2=_tmp6D;_tmp6E;});else{
return r;}}default: _LL7: _LL8:
 return r;}_LL0:;}
# 347
void*Cyc_CfFlowInfo_make_unique_consumed(struct Cyc_CfFlowInfo_FlowEnv*fenv,void*t,struct Cyc_Absyn_Exp*consumer,int iteration,void*r){
struct _tuple13 _tmp76=({struct _tuple13 _tmp22F;({void*_tmp26D=Cyc_Tcutil_compress(t);_tmp22F.f1=_tmp26D;}),_tmp22F.f2=r;_tmp22F;});struct _tuple13 _tmp77=_tmp76;enum Cyc_Absyn_AggrKind _tmp92;struct Cyc_List_List*_tmp91;struct Cyc_CfFlowInfo_UnionRInfo _tmp90;struct _dyneither_ptr _tmp8F;union Cyc_Absyn_AggrInfo _tmp8E;struct Cyc_CfFlowInfo_UnionRInfo _tmp8D;struct _dyneither_ptr _tmp8C;struct Cyc_List_List*_tmp8B;struct Cyc_CfFlowInfo_UnionRInfo _tmp8A;struct _dyneither_ptr _tmp89;struct Cyc_Absyn_Vardecl*_tmp88;void*_tmp87;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 8U){_LL1: _tmp88=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp87=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL2: {
# 350
void*_tmp78=Cyc_CfFlowInfo_make_unique_consumed(fenv,t,consumer,iteration,_tmp87);
if(_tmp78 != _tmp87)return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=8U,_tmp79->f1=_tmp88,_tmp79->f2=_tmp78;_tmp79;});else{
return r;}}}else{switch(*((int*)_tmp77.f1)){case 6U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 6U){_LL3: _tmp8B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp77.f1)->f1;_tmp8A=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp89=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL4: {
# 354
unsigned int _tmp7A=_get_dyneither_size(_tmp89,sizeof(void*));
struct _dyneither_ptr d2=({unsigned int _tmp7D=_tmp7A;void**_tmp7C=_cycalloc(_check_times(_tmp7D,sizeof(void*)));({{unsigned int _tmp22D=_tmp7A;unsigned int i;for(i=0;i < _tmp22D;++ i){_tmp7C[i]=fenv->unknown_all;}}0;});_tag_dyneither(_tmp7C,sizeof(void*),_tmp7D);});
{int i=0;for(0;i < _tmp7A;++ i){
({void*_tmp272=({struct Cyc_CfFlowInfo_FlowEnv*_tmp271=fenv;void*_tmp270=(*((struct _tuple16*)((struct Cyc_List_List*)_check_null(_tmp8B))->hd)).f2;struct Cyc_Absyn_Exp*_tmp26F=consumer;int _tmp26E=iteration;Cyc_CfFlowInfo_make_unique_consumed(_tmp271,_tmp270,_tmp26F,_tmp26E,*((void**)_check_dyneither_subscript(_tmp89,sizeof(void*),i)));});*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp272;});
_tmp8B=_tmp8B->tl;}}
# 360
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=6U,_tmp7B->f1=_tmp8A,_tmp7B->f2=d2;_tmp7B;});}}else{goto _LL9;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp77.f1)->f1)->tag == 20U){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 6U){_LL5: _tmp8E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp77.f1)->f1)->f1;_tmp8D=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp8C=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL6: {
# 362
struct Cyc_Absyn_Aggrdecl*_tmp7E=Cyc_Absyn_get_known_aggrdecl(_tmp8E);
if(_tmp7E->impl == 0)return r;
_tmp92=_tmp7E->kind;_tmp91=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp7E->impl))->fields;_tmp90=_tmp8D;_tmp8F=_tmp8C;goto _LL8;}}else{goto _LL9;}}else{goto _LL9;}case 7U: if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->tag == 6U){_LL7: _tmp92=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp77.f1)->f1;_tmp91=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp77.f1)->f2;_tmp90=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f1;_tmp8F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp77.f2)->f2;_LL8: {
# 366
struct _dyneither_ptr d2=({unsigned int _tmp85=_get_dyneither_size(_tmp8F,sizeof(void*));void**_tmp84=_cycalloc(_check_times(_tmp85,sizeof(void*)));({{unsigned int _tmp22E=_get_dyneither_size(_tmp8F,sizeof(void*));unsigned int i;for(i=0;i < _tmp22E;++ i){_tmp84[i]=*((void**)_check_dyneither_subscript(_tmp8F,sizeof(void*),(int)i));}}0;});_tag_dyneither(_tmp84,sizeof(void*),_tmp85);});
unsigned int sz=(unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp91);
{int i=0;for(0;i < sz;(i ++,_tmp91=_tmp91->tl)){
struct Cyc_Absyn_Aggrfield*_tmp7F=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp91))->hd;struct Cyc_Absyn_Aggrfield*_tmp80=_tmp7F;struct _dyneither_ptr*_tmp82;void*_tmp81;_LLC: _tmp82=_tmp80->name;_tmp81=_tmp80->type;_LLD:;
if(_get_dyneither_size(*_tmp82,sizeof(char))!= 1)
({void*_tmp273=Cyc_CfFlowInfo_make_unique_consumed(fenv,_tmp81,consumer,iteration,*((void**)_check_dyneither_subscript(_tmp8F,sizeof(void*),i)));*((void**)_check_dyneither_subscript(d2,sizeof(void*),i))=_tmp273;});}}
# 373
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=6U,_tmp83->f1=_tmp90,_tmp83->f2=d2;_tmp83;});}}else{goto _LL9;}default: _LL9: _LLA:
# 375
 if(Cyc_Tcutil_is_noalias_pointer(t,0))
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=7U,_tmp86->f1=consumer,_tmp86->f2=iteration,_tmp86->f3=r;_tmp86;});else{
return r;}}}_LL0:;}struct _tuple17{struct Cyc_CfFlowInfo_Place*f1;unsigned int f2;};
# 381
static int Cyc_CfFlowInfo_prefix_of_member(struct Cyc_CfFlowInfo_Place*place,struct Cyc_Dict_Dict set){
# 383
struct _RegionHandle _tmp93=_new_region("r");struct _RegionHandle*r=& _tmp93;_push_region(r);
{struct _tuple17 elem=({struct _tuple17 _tmp230;_tmp230.f1=place,_tmp230.f2=0U;_tmp230;});
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d))Cyc_Dict_make_iter)(r,set);
while(((int(*)(struct Cyc_Iter_Iter,struct _tuple17*))Cyc_Iter_next)(iter,& elem)){
struct Cyc_CfFlowInfo_Place*_tmp94=elem.f1;
struct Cyc_CfFlowInfo_Place _tmp95=*place;struct Cyc_CfFlowInfo_Place _tmp96=_tmp95;void*_tmp9D;struct Cyc_List_List*_tmp9C;_LL1: _tmp9D=_tmp96.root;_tmp9C=_tmp96.path;_LL2:;{
struct Cyc_CfFlowInfo_Place _tmp97=*_tmp94;struct Cyc_CfFlowInfo_Place _tmp98=_tmp97;void*_tmp9B;struct Cyc_List_List*_tmp9A;_LL4: _tmp9B=_tmp98.root;_tmp9A=_tmp98.path;_LL5:;
if(Cyc_CfFlowInfo_root_cmp(_tmp9D,_tmp9B)!= 0)
continue;
for(0;_tmp9C != 0  && _tmp9A != 0;(_tmp9C=_tmp9C->tl,_tmp9A=_tmp9A->tl)){
if((void*)_tmp9C->hd != (void*)_tmp9A->hd)break;}
if(_tmp9C == 0){
int _tmp99=1;_npop_handler(0U);return _tmp99;}};}{
# 397
int _tmp9E=0;_npop_handler(0U);return _tmp9E;};}
# 384
;_pop_region(r);}struct Cyc_CfFlowInfo_EscPile{struct Cyc_List_List*places;};
# 407
static void Cyc_CfFlowInfo_add_place(struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_CfFlowInfo_Place*place){
# 410
if(!((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,pile->places,place))
({struct Cyc_List_List*_tmp274=({struct Cyc_List_List*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->hd=place,_tmp9F->tl=pile->places;_tmp9F;});pile->places=_tmp274;});}
# 413
static void Cyc_CfFlowInfo_add_places(struct Cyc_CfFlowInfo_EscPile*pile,void*r){
void*_tmpA0=r;struct Cyc_CfFlowInfo_UnionRInfo _tmpA5;struct _dyneither_ptr _tmpA4;struct Cyc_CfFlowInfo_Place*_tmpA3;void*_tmpA2;void*_tmpA1;switch(*((int*)_tmpA0)){case 7U: _LL1: _tmpA1=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f3;_LL2:
 Cyc_CfFlowInfo_add_places(pile,_tmpA1);return;case 8U: _LL3: _tmpA2=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f2;_LL4:
 Cyc_CfFlowInfo_add_places(pile,_tmpA2);return;case 4U: _LL5: _tmpA3=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_LL6:
 Cyc_CfFlowInfo_add_place(pile,_tmpA3);return;case 6U: _LL7: _tmpA5=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f1;_tmpA4=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA0)->f2;_LL8:
# 419
{int i=0;for(0;i < _get_dyneither_size(_tmpA4,sizeof(void*));++ i){
Cyc_CfFlowInfo_add_places(pile,*((void**)_check_dyneither_subscript(_tmpA4,sizeof(void*),i)));}}
return;default: _LL9: _LLA:
 return;}_LL0:;}
# 429
static void*Cyc_CfFlowInfo_insert_place_inner(void*new_val,void*old_val){
void*_tmpA6=old_val;struct Cyc_Absyn_Vardecl*_tmpB2;void*_tmpB1;struct Cyc_Absyn_Exp*_tmpB0;int _tmpAF;void*_tmpAE;int _tmpAD;struct _dyneither_ptr _tmpAC;switch(*((int*)_tmpA6)){case 6U: _LL1: _tmpAD=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f1).is_union;_tmpAC=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f2;_LL2: {
# 432
struct _dyneither_ptr d2=({unsigned int _tmpA9=_get_dyneither_size(_tmpAC,sizeof(void*));void**_tmpA8=_cycalloc(_check_times(_tmpA9,sizeof(void*)));({{unsigned int _tmp231=_get_dyneither_size(_tmpAC,sizeof(void*));unsigned int i;for(i=0;i < _tmp231;++ i){({
void*_tmp275=Cyc_CfFlowInfo_insert_place_inner(new_val,*((void**)_check_dyneither_subscript(_tmpAC,sizeof(void*),(int)i)));_tmpA8[i]=_tmp275;});}}0;});_tag_dyneither(_tmpA8,sizeof(void*),_tmpA9);});
# 436
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=6U,(_tmpA7->f1).is_union=_tmpAD,(_tmpA7->f1).fieldnum=- 1,_tmpA7->f2=d2;_tmpA7;});}case 7U: _LL3: _tmpB0=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f1;_tmpAF=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f2;_tmpAE=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f3;_LL4:
# 438
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=7U,_tmpAA->f1=_tmpB0,_tmpAA->f2=_tmpAF,({void*_tmp276=Cyc_CfFlowInfo_insert_place_inner(new_val,_tmpAE);_tmpAA->f3=_tmp276;});_tmpAA;});case 8U: _LL5: _tmpB2=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f1;_tmpB1=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpA6)->f2;_LL6:
# 440
 return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=8U,_tmpAB->f1=_tmpB2,({void*_tmp277=Cyc_CfFlowInfo_insert_place_inner(new_val,_tmpB1);_tmpAB->f2=_tmp277;});_tmpAB;});default: _LL7: _LL8:
 return new_val;}_LL0:;}
# 447
static struct _dyneither_ptr Cyc_CfFlowInfo_aggr_dict_insert(struct _dyneither_ptr d,int n,void*rval){
void*_tmpB3=*((void**)_check_dyneither_subscript(d,sizeof(void*),n));
if(_tmpB3 == rval)return d;{
struct _dyneither_ptr res=({unsigned int _tmpB5=_get_dyneither_size(d,sizeof(void*));void**_tmpB4=_cycalloc(_check_times(_tmpB5,sizeof(void*)));({{unsigned int _tmp232=_get_dyneither_size(d,sizeof(void*));unsigned int i;for(i=0;i < _tmp232;++ i){_tmpB4[i]=*((void**)_check_dyneither_subscript(d,sizeof(void*),(int)i));}}0;});_tag_dyneither(_tmpB4,sizeof(void*),_tmpB5);});
*((void**)_check_dyneither_subscript(res,sizeof(void*),n))=rval;
return res;};}struct _tuple18{struct Cyc_List_List*f1;void*f2;};
# 460
static void*Cyc_CfFlowInfo_insert_place_outer(struct Cyc_List_List*path,void*old_val,void*new_val){
# 462
if(path == 0)
return Cyc_CfFlowInfo_insert_place_inner(new_val,old_val);{
struct _tuple18 _tmpB6=({struct _tuple18 _tmp233;_tmp233.f1=path,_tmp233.f2=old_val;_tmp233;});struct _tuple18 _tmpB7=_tmpB6;struct Cyc_Absyn_Vardecl*_tmpC8;void*_tmpC7;struct Cyc_List_List*_tmpC6;void*_tmpC5;int _tmpC4;struct Cyc_List_List*_tmpC3;int _tmpC2;struct _dyneither_ptr _tmpC1;if(_tmpB7.f1 != 0){if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmpB7.f1)->hd)->tag == 0U)switch(*((int*)_tmpB7.f2)){case 6U: _LL1: _tmpC4=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmpB7.f1)->hd)->f1;_tmpC3=(_tmpB7.f1)->tl;_tmpC2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f1).is_union;_tmpC1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f2;_LL2: {
# 466
void*_tmpB8=Cyc_CfFlowInfo_insert_place_outer(_tmpC3,*((void**)_check_dyneither_subscript(_tmpC1,sizeof(void*),_tmpC4)),new_val);
struct _dyneither_ptr _tmpB9=Cyc_CfFlowInfo_aggr_dict_insert(_tmpC1,_tmpC4,_tmpB8);
if((void**)_tmpB9.curr == (void**)_tmpC1.curr)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->tag=6U,(_tmpBA->f1).is_union=_tmpC2,(_tmpBA->f1).fieldnum=- 1,_tmpBA->f2=_tmpB9;_tmpBA;});}}case 8U: goto _LL5;default: goto _LL7;}else{switch(*((int*)_tmpB7.f2)){case 5U: _LL3: _tmpC6=(_tmpB7.f1)->tl;_tmpC5=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f1;_LL4: {
# 471
void*_tmpBB=Cyc_CfFlowInfo_insert_place_outer(_tmpC6,_tmpC5,new_val);
if(_tmpBB == _tmpC5)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->tag=5U,_tmpBC->f1=_tmpBB;_tmpBC;});}}case 8U: goto _LL5;default: goto _LL7;}}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->tag == 8U){_LL5: _tmpC8=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f1;_tmpC7=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpB7.f2)->f2;_LL6: {
# 475
void*_tmpBD=Cyc_CfFlowInfo_insert_place_outer(path,_tmpC7,new_val);
if(_tmpBD == _tmpC7)return old_val;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=8U,_tmpBE->f1=_tmpC8,_tmpBE->f2=_tmpBD;_tmpBE;});}}}else{_LL7: _LL8:
({void*_tmpBF=0U;({struct _dyneither_ptr _tmp278=({const char*_tmpC0="bad insert place";_tag_dyneither(_tmpC0,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp278,_tag_dyneither(_tmpBF,sizeof(void*),0U));});});}}_LL0:;};}
# 485
static struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_these(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_CfFlowInfo_EscPile*pile,struct Cyc_Dict_Dict d){
while(pile->places != 0){
struct Cyc_CfFlowInfo_Place*_tmpC9=(struct Cyc_CfFlowInfo_Place*)((struct Cyc_List_List*)_check_null(pile->places))->hd;
pile->places=((struct Cyc_List_List*)_check_null(pile->places))->tl;{
void*oldval;void*newval;
{struct _handler_cons _tmpCA;_push_handler(& _tmpCA);{int _tmpCC=0;if(setjmp(_tmpCA.handler))_tmpCC=1;if(!_tmpCC){({void*_tmp279=Cyc_CfFlowInfo_lookup_place(d,_tmpC9);oldval=_tmp279;});;_pop_handler();}else{void*_tmpCB=(void*)_exn_thrown;void*_tmpCD=_tmpCB;void*_tmpCE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpCD)->tag == Cyc_Dict_Absent){_LL1: _LL2:
 continue;}else{_LL3: _tmpCE=_tmpCD;_LL4:(int)_rethrow(_tmpCE);}_LL0:;}};}
{enum Cyc_CfFlowInfo_InitLevel _tmpCF=Cyc_CfFlowInfo_initlevel(fenv,d,oldval);enum Cyc_CfFlowInfo_InitLevel _tmpD0=_tmpCF;if(_tmpD0 == Cyc_CfFlowInfo_AllIL){_LL6: _LL7:
 newval=fenv->esc_all;goto _LL5;}else{_LL8: _LL9:
 newval=fenv->esc_none;goto _LL5;}_LL5:;}
# 496
Cyc_CfFlowInfo_add_places(pile,oldval);{
struct Cyc_CfFlowInfo_Place _tmpD1=*_tmpC9;struct Cyc_CfFlowInfo_Place _tmpD2=_tmpD1;void*_tmpD4;struct Cyc_List_List*_tmpD3;_LLB: _tmpD4=_tmpD2.root;_tmpD3=_tmpD2.path;_LLC:;
({struct Cyc_Dict_Dict _tmp27E=({struct Cyc_Dict_Dict _tmp27D=d;void*_tmp27C=_tmpD4;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(_tmp27D,_tmp27C,({
struct Cyc_List_List*_tmp27B=_tmpD3;void*_tmp27A=
((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmpD4);
# 499
Cyc_CfFlowInfo_insert_place_outer(_tmp27B,_tmp27A,newval);}));});
# 498
d=_tmp27E;});};};}
# 503
return d;}struct Cyc_CfFlowInfo_InitlevelEnv{struct Cyc_Dict_Dict d;struct Cyc_List_List*seen;};
# 513
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_approx(void*r){
void*_tmpD5=r;void*_tmpDA;enum Cyc_CfFlowInfo_InitLevel _tmpD9;enum Cyc_CfFlowInfo_InitLevel _tmpD8;switch(*((int*)_tmpD5)){case 2U: _LL1: _tmpD8=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmpD5)->f1;_LL2:
 return _tmpD8;case 3U: _LL3: _tmpD9=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmpD5)->f1;_LL4:
 return _tmpD9;case 0U: _LL5: _LL6:
 goto _LL8;case 1U: _LL7: _LL8:
 return Cyc_CfFlowInfo_AllIL;case 7U: _LL9: _tmpDA=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpD5)->f3;_LLA:
 return Cyc_CfFlowInfo_NoneIL;default: _LLB: _LLC:
({void*_tmpD6=0U;({struct _dyneither_ptr _tmp27F=({const char*_tmpD7="initlevel_approx";_tag_dyneither(_tmpD7,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp27F,_tag_dyneither(_tmpD6,sizeof(void*),0U));});});}_LL0:;}
# 523
static enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_rec(struct Cyc_CfFlowInfo_InitlevelEnv*env,void*r,enum Cyc_CfFlowInfo_InitLevel acc){
# 525
enum Cyc_CfFlowInfo_InitLevel this_ans;
if(acc == Cyc_CfFlowInfo_NoneIL)return Cyc_CfFlowInfo_NoneIL;
{void*_tmpDB=r;void*_tmpE5;struct Cyc_CfFlowInfo_Place*_tmpE4;struct _dyneither_ptr _tmpE3;int _tmpE2;int _tmpE1;struct _dyneither_ptr _tmpE0;void*_tmpDF;switch(*((int*)_tmpDB)){case 8U: _LL1: _tmpDF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f2;_LL2:
 return Cyc_CfFlowInfo_initlevel_rec(env,_tmpDF,acc);case 6U: _LL3: _tmpE2=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1).is_union;_tmpE1=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1).fieldnum;_tmpE0=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f2;if(_tmpE2){_LL4: {
# 533
unsigned int _tmpDC=_get_dyneither_size(_tmpE0,sizeof(void*));
this_ans=0U;
if(_tmpE1 == - 1){
int i=0;for(0;i < _tmpDC;++ i){
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmpE0,sizeof(void*),i)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL){
this_ans=1U;
break;}}}else{
# 543
if(Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmpE0,sizeof(void*),_tmpE1)),Cyc_CfFlowInfo_AllIL)== Cyc_CfFlowInfo_AllIL)
this_ans=1U;}
goto _LL0;}}else{_LL5: _tmpE3=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f2;_LL6: {
# 547
unsigned int _tmpDD=_get_dyneither_size(_tmpE3,sizeof(void*));
this_ans=1U;
{int i=0;for(0;i < _tmpDD;++ i){
({enum Cyc_CfFlowInfo_InitLevel _tmp280=Cyc_CfFlowInfo_initlevel_rec(env,*((void**)_check_dyneither_subscript(_tmpE3,sizeof(void*),i)),this_ans);this_ans=_tmp280;});}}
goto _LL0;}}case 4U: _LL7: _tmpE4=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1;_LL8:
# 553
 if(((int(*)(int(*compare)(struct Cyc_CfFlowInfo_Place*,struct Cyc_CfFlowInfo_Place*),struct Cyc_List_List*l,struct Cyc_CfFlowInfo_Place*x))Cyc_List_mem)(Cyc_CfFlowInfo_place_cmp,env->seen,_tmpE4))
this_ans=1U;else{
# 556
({struct Cyc_List_List*_tmp281=({struct Cyc_List_List*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->hd=_tmpE4,_tmpDE->tl=env->seen;_tmpDE;});env->seen=_tmp281;});
({enum Cyc_CfFlowInfo_InitLevel _tmp283=({struct Cyc_CfFlowInfo_InitlevelEnv*_tmp282=env;Cyc_CfFlowInfo_initlevel_rec(_tmp282,Cyc_CfFlowInfo_lookup_place(env->d,_tmpE4),Cyc_CfFlowInfo_AllIL);});this_ans=_tmp283;});
env->seen=((struct Cyc_List_List*)_check_null(env->seen))->tl;}
# 560
goto _LL0;case 5U: _LL9: _tmpE5=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpDB)->f1;_LLA:
({enum Cyc_CfFlowInfo_InitLevel _tmp284=Cyc_CfFlowInfo_initlevel_rec(env,_tmpE5,Cyc_CfFlowInfo_AllIL);this_ans=_tmp284;});goto _LL0;default: _LLB: _LLC:
({enum Cyc_CfFlowInfo_InitLevel _tmp285=Cyc_CfFlowInfo_initlevel_approx(r);this_ans=_tmp285;});goto _LL0;}_LL0:;}
# 564
return this_ans;}
# 566
enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel(struct Cyc_CfFlowInfo_FlowEnv*env,struct Cyc_Dict_Dict d,void*r){
struct Cyc_CfFlowInfo_InitlevelEnv _tmpE6=({struct Cyc_CfFlowInfo_InitlevelEnv _tmp234;_tmp234.d=d,_tmp234.seen=0;_tmp234;});
return Cyc_CfFlowInfo_initlevel_rec(& _tmpE6,r,Cyc_CfFlowInfo_AllIL);}
# 571
void*Cyc_CfFlowInfo_lookup_place(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
struct Cyc_CfFlowInfo_Place _tmpE7=*place;struct Cyc_CfFlowInfo_Place _tmpE8=_tmpE7;void*_tmpF5;struct Cyc_List_List*_tmpF4;_LL1: _tmpF5=_tmpE8.root;_tmpF4=_tmpE8.path;_LL2:;{
void*_tmpE9=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmpF5);
for(0;_tmpF4 != 0;_tmpF4=_tmpF4->tl){
retry: {struct _tuple13 _tmpEA=({struct _tuple13 _tmp235;_tmp235.f1=_tmpE9,_tmp235.f2=(void*)_tmpF4->hd;_tmp235;});struct _tuple13 _tmpEB=_tmpEA;void*_tmpF3;struct Cyc_CfFlowInfo_UnionRInfo _tmpF2;struct _dyneither_ptr _tmpF1;int _tmpF0;void*_tmpEF;void*_tmpEE;switch(*((int*)_tmpEB.f1)){case 8U: _LL4: _tmpEE=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpEB.f1)->f2;_LL5:
# 577
 _tmpE9=_tmpEE;goto retry;case 7U: _LL6: _tmpEF=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpEB.f1)->f3;_LL7:
# 580
 _tmpE9=_tmpEF;goto retry;case 6U: if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmpEB.f2)->tag == 0U){_LL8: _tmpF2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpEB.f1)->f1;_tmpF1=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpEB.f1)->f2;_tmpF0=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmpEB.f2)->f1;_LL9:
# 582
 _tmpE9=*((void**)_check_dyneither_subscript(_tmpF1,sizeof(void*),_tmpF0));
goto _LL3;}else{goto _LLC;}case 5U: if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)_tmpEB.f2)->tag == 1U){_LLA: _tmpF3=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmpEB.f1)->f1;_LLB:
# 585
 _tmpE9=_tmpF3;
goto _LL3;}else{goto _LLC;}default: _LLC: _LLD:
# 593
({void*_tmpEC=0U;({struct _dyneither_ptr _tmp286=({const char*_tmpED="bad lookup_place";_tag_dyneither(_tmpED,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp286,_tag_dyneither(_tmpEC,sizeof(void*),0U));});});}_LL3:;}}
# 595
return _tmpE9;};}
# 598
static int Cyc_CfFlowInfo_is_rval_unescaped(void*rval){
void*_tmpF6=rval;int _tmpFB;int _tmpFA;struct _dyneither_ptr _tmpF9;void*_tmpF8;void*_tmpF7;switch(*((int*)_tmpF6)){case 3U: _LL1: _LL2:
 return 0;case 7U: _LL3: _tmpF7=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f3;_LL4:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmpF7);case 8U: _LL5: _tmpF8=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f2;_LL6:
 return Cyc_CfFlowInfo_is_rval_unescaped(_tmpF8);case 6U: _LL7: _tmpFB=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f1).is_union;_tmpFA=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f1).fieldnum;_tmpF9=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmpF6)->f2;_LL8:
# 604
 if(_tmpFB  && _tmpFA != - 1)
return !Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmpF9,sizeof(void*),_tmpFA)));else{
# 607
unsigned int sz=_get_dyneither_size(_tmpF9,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(!Cyc_CfFlowInfo_is_rval_unescaped(*((void**)_check_dyneither_subscript(_tmpF9,sizeof(void*),i))))return 0;}}
return 1;}default: _LL9: _LLA:
# 612
 return 1;}_LL0:;}
# 615
int Cyc_CfFlowInfo_is_unescaped(struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place){
return Cyc_CfFlowInfo_is_rval_unescaped(Cyc_CfFlowInfo_lookup_place(d,place));}
# 618
int Cyc_CfFlowInfo_is_init_pointer(void*rval){
void*_tmpFC=rval;void*_tmpFD;switch(*((int*)_tmpFC)){case 8U: _LL1: _tmpFD=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmpFC)->f2;_LL2:
 return Cyc_CfFlowInfo_is_init_pointer(_tmpFD);case 4U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 return 1;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 628
struct Cyc_Dict_Dict Cyc_CfFlowInfo_escape_deref(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct Cyc_Dict_Dict d,void*r){
struct _RegionHandle _tmpFE=_new_region("rgn");struct _RegionHandle*rgn=& _tmpFE;_push_region(rgn);
{struct Cyc_CfFlowInfo_EscPile*pile=({struct Cyc_CfFlowInfo_EscPile*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->places=0;_tmp100;});
Cyc_CfFlowInfo_add_places(pile,r);{
struct Cyc_Dict_Dict _tmpFF=Cyc_CfFlowInfo_escape_these(fenv,pile,d);_npop_handler(0U);return _tmpFF;};}
# 630
;_pop_region(rgn);}struct Cyc_CfFlowInfo_AssignEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d;unsigned int loc;struct Cyc_CfFlowInfo_Place*root_place;};
# 642
static void*Cyc_CfFlowInfo_assign_place_inner(struct Cyc_CfFlowInfo_AssignEnv*env,void*oldval,void*newval){
# 647
struct _tuple13 _tmp101=({struct _tuple13 _tmp237;_tmp237.f1=oldval,_tmp237.f2=newval;_tmp237;});struct _tuple13 _tmp102=_tmp101;enum Cyc_CfFlowInfo_InitLevel _tmp113;struct Cyc_CfFlowInfo_UnionRInfo _tmp112;struct _dyneither_ptr _tmp111;struct Cyc_CfFlowInfo_UnionRInfo _tmp110;struct _dyneither_ptr _tmp10F;struct Cyc_CfFlowInfo_Place*_tmp10E;struct Cyc_Absyn_Vardecl*_tmp10D;void*_tmp10C;void*_tmp10B;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->tag == 8U){_LL1: _tmp10B=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->f2;_LL2:
# 649
 return Cyc_CfFlowInfo_assign_place_inner(env,_tmp10B,newval);}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->tag == 8U){_LL3: _tmp10D=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_tmp10C=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f2;_LL4: {
# 651
void*_tmp103=Cyc_CfFlowInfo_assign_place_inner(env,oldval,_tmp10C);
if(_tmp103 == _tmp10C)return newval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=8U,_tmp104->f1=_tmp10D,_tmp104->f2=_tmp103;_tmp104;});}}}else{switch(*((int*)_tmp102.f1)){case 3U: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->tag == 4U){_LL5: _tmp10E=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_LL6:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp10E);goto _LL8;}else{_LL7: _LL8:
# 656
 if(Cyc_CfFlowInfo_initlevel(env->fenv,env->d,newval)!= Cyc_CfFlowInfo_AllIL)
({void*_tmp105=0U;({unsigned int _tmp288=env->loc;struct _dyneither_ptr _tmp287=({const char*_tmp106="assignment puts possibly-uninitialized data in an escaped location";_tag_dyneither(_tmp106,sizeof(char),67U);});Cyc_CfFlowInfo_aerr(_tmp288,_tmp287,_tag_dyneither(_tmp105,sizeof(void*),0U));});});
# 659
return(env->fenv)->esc_all;}case 6U: switch(*((int*)_tmp102.f2)){case 6U: _LL9: _tmp112=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->f1;_tmp111=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f1)->f2;_tmp110=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_tmp10F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f2;_LLA: {
# 661
struct _dyneither_ptr new_d=({unsigned int _tmp109=
_get_dyneither_size(_tmp111,sizeof(void*));void**_tmp108=_cycalloc(_check_times(_tmp109,sizeof(void*)));({{unsigned int _tmp236=_get_dyneither_size(_tmp111,sizeof(void*));unsigned int i;for(i=0;i < _tmp236;++ i){({void*_tmp28B=({struct Cyc_CfFlowInfo_AssignEnv*_tmp28A=env;void*_tmp289=*((void**)_check_dyneither_subscript(_tmp111,sizeof(void*),(int)i));Cyc_CfFlowInfo_assign_place_inner(_tmp28A,_tmp289,*((void**)_check_dyneither_subscript(_tmp10F,sizeof(void*),(int)i)));});_tmp108[i]=_tmp28B;});}}0;});_tag_dyneither(_tmp108,sizeof(void*),_tmp109);});
# 665
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp111,sizeof(void*));++ i){
if(({void*_tmp28C=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp28C != *((void**)_check_dyneither_subscript(_tmp111,sizeof(void*),i));})){
change=1;break;}}}
# 670
if(!change){
if(!_tmp112.is_union)return oldval;
new_d=_tmp111;}else{
# 675
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp111,sizeof(void*));++ i){
if(({void*_tmp28D=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp28D != *((void**)_check_dyneither_subscript(_tmp10F,sizeof(void*),i));})){
change=1;break;}}}
# 680
if(!change){
if(!_tmp112.is_union)return newval;
new_d=_tmp10F;}}
# 685
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->tag=6U,_tmp107->f1=_tmp110,_tmp107->f2=new_d;_tmp107;});}case 3U: goto _LLB;default: goto _LLD;}default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->tag == 3U){_LLB: _tmp113=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp102.f2)->f1;_LLC: {
# 687
enum Cyc_CfFlowInfo_InitLevel _tmp10A=_tmp113;if(_tmp10A == Cyc_CfFlowInfo_NoneIL){_LL10: _LL11:
 return(env->fenv)->unknown_none;}else{_LL12: _LL13:
 return(env->fenv)->unknown_all;}_LLF:;}}else{_LLD: _LLE:
# 691
 return newval;}}}}_LL0:;}
# 697
static int Cyc_CfFlowInfo_nprefix(int*n,void*unused){
if(*n > 0){*n=*n - 1;return 1;}else{
return 0;}}
# 701
static void*Cyc_CfFlowInfo_assign_place_outer(struct Cyc_CfFlowInfo_AssignEnv*env,struct Cyc_List_List*path,int path_prefix,void*oldval,void*newval){
# 712
if(path == 0)return Cyc_CfFlowInfo_assign_place_inner(env,oldval,newval);{
struct _tuple18 _tmp114=({struct _tuple18 _tmp238;_tmp238.f1=path,_tmp238.f2=oldval;_tmp238;});struct _tuple18 _tmp115=_tmp114;int _tmp135;struct Cyc_List_List*_tmp134;int _tmp133;int _tmp132;struct _dyneither_ptr _tmp131;struct Cyc_List_List*_tmp130;void*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;int _tmp12D;void*_tmp12C;struct Cyc_Absyn_Vardecl*_tmp12B;void*_tmp12A;switch(*((int*)_tmp115.f2)){case 8U: _LL1: _tmp12B=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1;_tmp12A=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f2;_LL2: {
# 715
void*_tmp116=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,_tmp12A,newval);
if(_tmp116 == _tmp12A)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->tag=8U,_tmp117->f1=_tmp12B,_tmp117->f2=_tmp116;_tmp117;});}}case 7U: _LL3: _tmp12E=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1;_tmp12D=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f2;_tmp12C=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f3;_LL4: {
# 719
void*_tmp118=Cyc_CfFlowInfo_assign_place_outer(env,path,path_prefix,_tmp12C,newval);
if(_tmp118 == _tmp12C)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=7U,_tmp119->f1=_tmp12E,_tmp119->f2=_tmp12D,_tmp119->f3=_tmp118;_tmp119;});}}default: if(_tmp115.f1 != 0){if(((struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct*)((struct Cyc_List_List*)_tmp115.f1)->hd)->tag == 1U){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->tag == 5U){_LL5: _tmp130=(_tmp115.f1)->tl;_tmp12F=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1;_LL6: {
# 723
void*_tmp11A=Cyc_CfFlowInfo_assign_place_outer(env,_tmp130,path_prefix + 1,_tmp12F,newval);
if(_tmp11A == _tmp12F)return oldval;else{
return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=5U,_tmp11B->f1=_tmp11A;_tmp11B;});}}}else{goto _LL9;}}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->tag == 6U){_LL7: _tmp135=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)(_tmp115.f1)->hd)->f1;_tmp134=(_tmp115.f1)->tl;_tmp133=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1).is_union;_tmp132=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f1).fieldnum;_tmp131=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp115.f2)->f2;_LL8: {
# 727
void*_tmp11C=Cyc_CfFlowInfo_assign_place_outer(env,_tmp134,path_prefix + 1,*((void**)_check_dyneither_subscript(_tmp131,sizeof(void*),_tmp135)),newval);
# 729
struct _dyneither_ptr _tmp11D=Cyc_CfFlowInfo_aggr_dict_insert(_tmp131,_tmp135,_tmp11C);
if((void**)_tmp11D.curr == (void**)_tmp131.curr  && (!_tmp133  || _tmp132 == _tmp135))return oldval;
_tmp131=_tmp11D;
# 734
if(_tmp133){
int changed=0;
int sz=(int)_get_dyneither_size(_tmp131,sizeof(void*));
{int i=0;for(0;i < sz;++ i){
if(i != _tmp135){
struct _dyneither_ptr _tmp11E=({
struct _dyneither_ptr _tmp28F=_tmp131;int _tmp28E=i;Cyc_CfFlowInfo_aggr_dict_insert(_tmp28F,_tmp28E,
Cyc_CfFlowInfo_insert_place_inner((env->fenv)->unknown_all,*((void**)_check_dyneither_subscript(_tmp131,sizeof(void*),i))));});
if((void**)_tmp11E.curr != (void**)_tmp131.curr){
_tmp131=_tmp11E;
changed=1;}}}}
# 750
if(changed){
struct Cyc_CfFlowInfo_Place*_tmp11F=env->root_place;struct Cyc_CfFlowInfo_Place*_tmp120=_tmp11F;void*_tmp126;struct Cyc_List_List*_tmp125;_LLC: _tmp126=_tmp120->root;_tmp125=_tmp120->path;_LLD:;{
struct Cyc_List_List*_tmp121=({int*_tmp290=({int*_tmp124=_cycalloc_atomic(sizeof(*_tmp124));*_tmp124=path_prefix;_tmp124;});((struct Cyc_List_List*(*)(int(*f)(int*,void*),int*env,struct Cyc_List_List*x))Cyc_List_filter_c)((int(*)(int*n,void*unused))Cyc_CfFlowInfo_nprefix,_tmp290,_tmp125);});
struct Cyc_CfFlowInfo_Place*_tmp122=({struct Cyc_CfFlowInfo_Place*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->root=_tmp126,_tmp123->path=_tmp121;_tmp123;});;};}}
# 756
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=6U,(_tmp127->f1).is_union=_tmp133,(_tmp127->f1).fieldnum=_tmp135,_tmp127->f2=_tmp131;_tmp127;});}}else{goto _LL9;}}}else{_LL9: _LLA:
({void*_tmp128=0U;({struct _dyneither_ptr _tmp291=({const char*_tmp129="bad assign place";_tag_dyneither(_tmp129,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp291,_tag_dyneither(_tmp128,sizeof(void*),0U));});});}}_LL0:;};}
# 760
struct Cyc_Dict_Dict Cyc_CfFlowInfo_assign_place(struct Cyc_CfFlowInfo_FlowEnv*fenv,unsigned int loc,struct Cyc_Dict_Dict d,struct Cyc_CfFlowInfo_Place*place,void*r){
# 768
struct Cyc_CfFlowInfo_Place*_tmp136=place;void*_tmp139;struct Cyc_List_List*_tmp138;_LL1: _tmp139=_tmp136->root;_tmp138=_tmp136->path;_LL2:;{
struct Cyc_CfFlowInfo_AssignEnv env=({struct Cyc_CfFlowInfo_AssignEnv _tmp239;_tmp239.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp292=({struct Cyc_CfFlowInfo_EscPile*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->places=0;_tmp137;});_tmp239.pile=_tmp292;}),_tmp239.d=d,_tmp239.loc=loc,_tmp239.root_place=place;_tmp239;});
void*newval=({struct Cyc_List_List*_tmp294=_tmp138;void*_tmp293=((void*(*)(struct Cyc_Dict_Dict d,void*k))Cyc_Dict_lookup)(d,_tmp139);Cyc_CfFlowInfo_assign_place_outer(& env,_tmp294,0,_tmp293,r);});
return({struct Cyc_CfFlowInfo_FlowEnv*_tmp296=fenv;struct Cyc_CfFlowInfo_EscPile*_tmp295=env.pile;Cyc_CfFlowInfo_escape_these(_tmp296,_tmp295,((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,void*k,void*v))Cyc_Dict_insert)(d,_tmp139,newval));});};}struct Cyc_CfFlowInfo_JoinEnv{struct Cyc_CfFlowInfo_FlowEnv*fenv;struct Cyc_CfFlowInfo_EscPile*pile;struct Cyc_Dict_Dict d1;struct Cyc_Dict_Dict d2;};
# 783
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2);struct _tuple19{enum Cyc_CfFlowInfo_InitLevel f1;enum Cyc_CfFlowInfo_InitLevel f2;};
# 791
static void*Cyc_CfFlowInfo_join_absRval(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2){
if(r1 == r2)return r1;
# 794
{struct _tuple13 _tmp13A=({struct _tuple13 _tmp23B;_tmp23B.f1=r1,_tmp23B.f2=r2;_tmp23B;});struct _tuple13 _tmp13B=_tmp13A;int _tmp173;int _tmp172;struct _dyneither_ptr _tmp171;int _tmp170;int _tmp16F;struct _dyneither_ptr _tmp16E;struct Cyc_CfFlowInfo_Place*_tmp16D;void*_tmp16C;void*_tmp16B;void*_tmp16A;void*_tmp169;struct Cyc_CfFlowInfo_Place*_tmp168;struct Cyc_CfFlowInfo_Place*_tmp167;struct Cyc_CfFlowInfo_Place*_tmp166;struct Cyc_CfFlowInfo_Place*_tmp165;struct Cyc_CfFlowInfo_Place*_tmp164;struct Cyc_Absyn_Exp*_tmp163;int _tmp162;void*_tmp161;struct Cyc_Absyn_Exp*_tmp160;int _tmp15F;void*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;int _tmp15C;void*_tmp15B;struct Cyc_Absyn_Exp*_tmp15A;int _tmp159;void*_tmp158;struct Cyc_Absyn_Vardecl*_tmp157;void*_tmp156;struct Cyc_Absyn_Vardecl*_tmp155;void*_tmp154;struct Cyc_Absyn_Vardecl*_tmp153;void*_tmp152;struct Cyc_Absyn_Vardecl*_tmp151;void*_tmp150;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->tag == 8U){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 8U){_LL1: _tmp153=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp152=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp151=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp150=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_LL2:
# 797
 if(_tmp153 == _tmp151)
return(void*)({struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=8U,_tmp13C->f1=_tmp153,({void*_tmp297=Cyc_CfFlowInfo_join_absRval(env,a,_tmp152,_tmp150);_tmp13C->f2=_tmp297;});_tmp13C;});else{
# 800
return Cyc_CfFlowInfo_join_absRval(env,a,_tmp152,_tmp150);}}else{_LL3: _tmp155=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp154=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_LL4:
 return Cyc_CfFlowInfo_join_absRval(env,a,_tmp154,r2);}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 8U){_LL5: _tmp157=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp156=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_LL6:
 return Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp156);}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->tag == 7U){if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 7U){_LL7: _tmp15D=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp15C=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp15B=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f3;_tmp15A=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp159=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_tmp158=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f3;_LL8:
# 805
 if(_tmp15D == _tmp15A)
return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->tag=7U,_tmp13D->f1=_tmp15D,_tmp15C > _tmp159?_tmp13D->f2=_tmp15C:(_tmp13D->f2=_tmp159),({void*_tmp298=Cyc_CfFlowInfo_join_absRval(env,a,_tmp15B,_tmp158);_tmp13D->f3=_tmp298;});_tmp13D;});
{void*_tmp13E=_tmp15B;struct Cyc_CfFlowInfo_Place*_tmp13F;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13E)->tag == 4U){_LL22: _tmp13F=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13E)->f1;_LL23:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp13F);goto _LL21;}else{_LL24: _LL25:
 goto _LL21;}_LL21:;}
# 811
{void*_tmp140=_tmp158;struct Cyc_CfFlowInfo_Place*_tmp141;if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp140)->tag == 4U){_LL27: _tmp141=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp140)->f1;_LL28:
 Cyc_CfFlowInfo_add_place(env->pile,_tmp141);goto _LL26;}else{_LL29: _LL2A:
 goto _LL26;}_LL26:;}
# 815
goto _LL0;}else{_LL9: _tmp160=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp15F=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp15E=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f3;_LLA:
# 817
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->tag=7U,_tmp142->f1=_tmp160,_tmp142->f2=_tmp15F,({void*_tmp299=Cyc_CfFlowInfo_join_absRval(env,a,_tmp15E,r2);_tmp142->f3=_tmp299;});_tmp142;});}}else{if(((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 7U){_LLB: _tmp163=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_tmp162=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_tmp161=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f3;_LLC:
# 819
 return(void*)({struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143->tag=7U,_tmp143->f1=_tmp163,_tmp143->f2=_tmp162,({void*_tmp29A=Cyc_CfFlowInfo_join_absRval(env,a,r1,_tmp161);_tmp143->f3=_tmp29A;});_tmp143;});}else{switch(*((int*)_tmp13B.f1)){case 4U: switch(*((int*)_tmp13B.f2)){case 4U: _LLD: _tmp165=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp164=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LLE:
# 822
 if(Cyc_CfFlowInfo_place_cmp(_tmp165,_tmp164)== 0)return r1;
Cyc_CfFlowInfo_add_place(env->pile,_tmp165);
Cyc_CfFlowInfo_add_place(env->pile,_tmp164);
goto _LL0;case 1U: _LLF: _tmp166=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_LL10:
# 828
 Cyc_CfFlowInfo_add_place(env->pile,_tmp166);{
enum Cyc_CfFlowInfo_InitLevel _tmp144=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);enum Cyc_CfFlowInfo_InitLevel _tmp145=_tmp144;if(_tmp145 == Cyc_CfFlowInfo_AllIL){_LL2C: _LL2D:
 return(env->fenv)->notzeroall;}else{_LL2E: _LL2F:
 return(env->fenv)->unknown_none;}_LL2B:;};default: _LL19: _tmp167=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_LL1A:
# 854
 Cyc_CfFlowInfo_add_place(env->pile,_tmp167);goto _LL0;}case 1U: switch(*((int*)_tmp13B.f2)){case 4U: _LL11: _tmp168=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL12:
# 834
 Cyc_CfFlowInfo_add_place(env->pile,_tmp168);{
enum Cyc_CfFlowInfo_InitLevel _tmp146=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);enum Cyc_CfFlowInfo_InitLevel _tmp147=_tmp146;if(_tmp147 == Cyc_CfFlowInfo_AllIL){_LL31: _LL32:
 return(env->fenv)->notzeroall;}else{_LL33: _LL34:
 return(env->fenv)->unknown_none;}_LL30:;};case 5U: _LL17: _tmp169=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL18: {
# 849
enum Cyc_CfFlowInfo_InitLevel _tmp14B=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,_tmp169);enum Cyc_CfFlowInfo_InitLevel _tmp14C=_tmp14B;if(_tmp14C == Cyc_CfFlowInfo_AllIL){_LL3B: _LL3C:
 return(env->fenv)->notzeroall;}else{_LL3D: _LL3E:
 return(env->fenv)->unknown_none;}_LL3A:;}default: goto _LL1F;}case 5U: switch(*((int*)_tmp13B.f2)){case 5U: _LL13: _tmp16B=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_tmp16A=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL14:
# 841
 return(void*)({struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->tag=5U,({void*_tmp29B=Cyc_CfFlowInfo_join_absRval(env,a,_tmp16B,_tmp16A);_tmp148->f1=_tmp29B;});_tmp148;});case 1U: _LL15: _tmp16C=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1;_LL16: {
# 844
enum Cyc_CfFlowInfo_InitLevel _tmp149=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,_tmp16C);enum Cyc_CfFlowInfo_InitLevel _tmp14A=_tmp149;if(_tmp14A == Cyc_CfFlowInfo_AllIL){_LL36: _LL37:
 return(env->fenv)->notzeroall;}else{_LL38: _LL39:
 return(env->fenv)->unknown_none;}_LL35:;}case 4U: goto _LL1B;default: goto _LL1F;}default: if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 4U){_LL1B: _tmp16D=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1;_LL1C:
# 855
 Cyc_CfFlowInfo_add_place(env->pile,_tmp16D);goto _LL0;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->tag == 6U){if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->tag == 6U){_LL1D: _tmp173=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1).is_union;_tmp172=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f1).fieldnum;_tmp171=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f1)->f2;_tmp170=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1).is_union;_tmp16F=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f1).fieldnum;_tmp16E=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp13B.f2)->f2;_LL1E: {
# 858
struct _dyneither_ptr new_d=({unsigned int _tmp14F=
_get_dyneither_size(_tmp171,sizeof(void*));void**_tmp14E=_cycalloc(_check_times(_tmp14F,sizeof(void*)));({{unsigned int _tmp23A=_get_dyneither_size(_tmp171,sizeof(void*));unsigned int i;for(i=0;i < _tmp23A;++ i){({void*_tmp29E=({struct Cyc_CfFlowInfo_JoinEnv*_tmp29D=env;void*_tmp29C=*((void**)_check_dyneither_subscript(_tmp171,sizeof(void*),(int)i));((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(_tmp29D,0,_tmp29C,*((void**)_check_dyneither_subscript(_tmp16E,sizeof(void*),(int)i)));});_tmp14E[i]=_tmp29E;});}}0;});_tag_dyneither(_tmp14E,sizeof(void*),_tmp14F);});
# 863
int change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp171,sizeof(void*));++ i){
if(({void*_tmp29F=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp29F != *((void**)_check_dyneither_subscript(_tmp171,sizeof(void*),i));})){
change=1;break;}}}
# 868
if(!change){
if(!_tmp173)return r1;
new_d=_tmp171;}else{
# 873
change=0;
{int i=0;for(0;i < _get_dyneither_size(_tmp171,sizeof(void*));++ i){
if(({void*_tmp2A0=*((void**)_check_dyneither_subscript(new_d,sizeof(void*),i));_tmp2A0 != *((void**)_check_dyneither_subscript(_tmp16E,sizeof(void*),i));})){
change=1;break;}}}
# 878
if(!change){
if(!_tmp173)return r2;
new_d=_tmp16E;}}
# 883
return(void*)({struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->tag=6U,(_tmp14D->f1).is_union=_tmp173,_tmp172 == _tmp16F?(_tmp14D->f1).fieldnum=_tmp172:((_tmp14D->f1).fieldnum=- 1),_tmp14D->f2=new_d;_tmp14D;});}}else{goto _LL1F;}}else{_LL1F: _LL20:
# 885
 goto _LL0;}}}}}}}_LL0:;}{
# 887
enum Cyc_CfFlowInfo_InitLevel il1=Cyc_CfFlowInfo_initlevel(env->fenv,env->d1,r1);
enum Cyc_CfFlowInfo_InitLevel il2=Cyc_CfFlowInfo_initlevel(env->fenv,env->d2,r2);
struct _tuple13 _tmp174=({struct _tuple13 _tmp23E;_tmp23E.f1=r1,_tmp23E.f2=r2;_tmp23E;});struct _tuple13 _tmp175=_tmp174;if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp175.f1)->tag == 3U){_LL40: _LL41:
 goto _LL43;}else{if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp175.f2)->tag == 3U){_LL42: _LL43: {
# 892
struct _tuple19 _tmp176=({struct _tuple19 _tmp23C;_tmp23C.f1=il1,_tmp23C.f2=il2;_tmp23C;});struct _tuple19 _tmp177=_tmp176;if(_tmp177.f2 == Cyc_CfFlowInfo_NoneIL){_LL47: _LL48:
 goto _LL4A;}else{if(_tmp177.f1 == Cyc_CfFlowInfo_NoneIL){_LL49: _LL4A: return(env->fenv)->esc_none;}else{_LL4B: _LL4C:
 return(env->fenv)->esc_all;}}_LL46:;}}else{_LL44: _LL45: {
# 897
struct _tuple19 _tmp178=({struct _tuple19 _tmp23D;_tmp23D.f1=il1,_tmp23D.f2=il2;_tmp23D;});struct _tuple19 _tmp179=_tmp178;if(_tmp179.f2 == Cyc_CfFlowInfo_NoneIL){_LL4E: _LL4F:
 goto _LL51;}else{if(_tmp179.f1 == Cyc_CfFlowInfo_NoneIL){_LL50: _LL51: return(env->fenv)->unknown_none;}else{_LL52: _LL53:
 return(env->fenv)->unknown_all;}}_LL4D:;}}}_LL3F:;};}struct _tuple20{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;};
# 904
union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_join_flow(struct Cyc_CfFlowInfo_FlowEnv*fenv,union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 906
struct _tuple20 _tmp17A=({struct _tuple20 _tmp240;_tmp240.f1=f1,_tmp240.f2=f2;_tmp240;});struct _tuple20 _tmp17B=_tmp17A;struct Cyc_Dict_Dict _tmp183;struct Cyc_List_List*_tmp182;struct Cyc_Dict_Dict _tmp181;struct Cyc_List_List*_tmp180;if(((_tmp17B.f1).BottomFL).tag == 1){_LL1: _LL2:
 return f2;}else{if(((_tmp17B.f2).BottomFL).tag == 1){_LL3: _LL4:
 return f1;}else{_LL5: _tmp183=(((_tmp17B.f1).ReachableFL).val).f1;_tmp182=(((_tmp17B.f1).ReachableFL).val).f2;_tmp181=(((_tmp17B.f2).ReachableFL).val).f1;_tmp180=(((_tmp17B.f2).ReachableFL).val).f2;_LL6:
# 912
 if(_tmp183.t == _tmp181.t  && _tmp182 == _tmp180)return f1;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f1,f2))return f2;
if(Cyc_CfFlowInfo_flow_lessthan_approx(f2,f1))return f1;{
struct Cyc_CfFlowInfo_JoinEnv _tmp17C=({struct Cyc_CfFlowInfo_JoinEnv _tmp23F;_tmp23F.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp2A1=({struct Cyc_CfFlowInfo_EscPile*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->places=0;_tmp17F;});_tmp23F.pile=_tmp2A1;}),_tmp23F.d1=_tmp183,_tmp23F.d2=_tmp181;_tmp23F;});
struct Cyc_Dict_Dict _tmp17D=((struct Cyc_Dict_Dict(*)(void*(*f)(struct Cyc_CfFlowInfo_JoinEnv*,void*,void*,void*),struct Cyc_CfFlowInfo_JoinEnv*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,void*a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval,& _tmp17C,_tmp183,_tmp181);
struct Cyc_List_List*_tmp17E=Cyc_Relations_join_relns(Cyc_Core_heap_region,_tmp182,_tmp180);
return({struct Cyc_Dict_Dict _tmp2A2=Cyc_CfFlowInfo_escape_these(fenv,_tmp17C.pile,_tmp17D);Cyc_CfFlowInfo_ReachableFL(_tmp2A2,_tmp17E);});};}}_LL0:;}struct _tuple21{union Cyc_CfFlowInfo_FlowInfo f1;union Cyc_CfFlowInfo_FlowInfo f2;union Cyc_CfFlowInfo_FlowInfo f3;};
# 922
struct _tuple12 Cyc_CfFlowInfo_join_flow_and_rval(struct Cyc_CfFlowInfo_FlowEnv*fenv,struct _tuple12 pr1,struct _tuple12 pr2){
# 927
struct _tuple12 _tmp184=pr1;union Cyc_CfFlowInfo_FlowInfo _tmp194;void*_tmp193;_LL1: _tmp194=_tmp184.f1;_tmp193=_tmp184.f2;_LL2:;{
struct _tuple12 _tmp185=pr2;union Cyc_CfFlowInfo_FlowInfo _tmp192;void*_tmp191;_LL4: _tmp192=_tmp185.f1;_tmp191=_tmp185.f2;_LL5:;{
union Cyc_CfFlowInfo_FlowInfo outflow=Cyc_CfFlowInfo_join_flow(fenv,_tmp194,_tmp192);
struct _tuple21 _tmp186=({struct _tuple21 _tmp247;_tmp247.f1=_tmp194,_tmp247.f2=_tmp192,_tmp247.f3=outflow;_tmp247;});struct _tuple21 _tmp187=_tmp186;struct Cyc_Dict_Dict _tmp190;struct Cyc_Dict_Dict _tmp18F;struct Cyc_Dict_Dict _tmp18E;struct Cyc_List_List*_tmp18D;if(((_tmp187.f1).BottomFL).tag == 1){_LL7: _LL8:
 return({struct _tuple12 _tmp241;_tmp241.f1=outflow,_tmp241.f2=_tmp191;_tmp241;});}else{if(((_tmp187.f2).BottomFL).tag == 1){_LL9: _LLA:
 return({struct _tuple12 _tmp242;_tmp242.f1=outflow,_tmp242.f2=_tmp193;_tmp242;});}else{if(((_tmp187.f3).ReachableFL).tag == 2){_LLB: _tmp190=(((_tmp187.f1).ReachableFL).val).f1;_tmp18F=(((_tmp187.f2).ReachableFL).val).f1;_tmp18E=(((_tmp187.f3).ReachableFL).val).f1;_tmp18D=(((_tmp187.f3).ReachableFL).val).f2;_LLC:
# 935
 if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp193,_tmp191))return({struct _tuple12 _tmp243;_tmp243.f1=outflow,_tmp243.f2=_tmp191;_tmp243;});
if(((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp191,_tmp193))return({struct _tuple12 _tmp244;_tmp244.f1=outflow,_tmp244.f2=_tmp193;_tmp244;});{
struct Cyc_CfFlowInfo_JoinEnv _tmp188=({struct Cyc_CfFlowInfo_JoinEnv _tmp246;_tmp246.fenv=fenv,({struct Cyc_CfFlowInfo_EscPile*_tmp2A3=({struct Cyc_CfFlowInfo_EscPile*_tmp18A=_cycalloc(sizeof(*_tmp18A));_tmp18A->places=0;_tmp18A;});_tmp246.pile=_tmp2A3;}),_tmp246.d1=_tmp190,_tmp246.d2=_tmp18F;_tmp246;});
void*_tmp189=((void*(*)(struct Cyc_CfFlowInfo_JoinEnv*env,int a,void*r1,void*r2))Cyc_CfFlowInfo_join_absRval)(& _tmp188,0,_tmp193,_tmp191);
return({struct _tuple12 _tmp245;({union Cyc_CfFlowInfo_FlowInfo _tmp2A5=({struct Cyc_Dict_Dict _tmp2A4=Cyc_CfFlowInfo_escape_these(fenv,_tmp188.pile,_tmp18E);Cyc_CfFlowInfo_ReachableFL(_tmp2A4,_tmp18D);});_tmp245.f1=_tmp2A5;}),_tmp245.f2=_tmp189;_tmp245;});};}else{_LLD: _LLE:
# 941
({void*_tmp18B=0U;({struct _dyneither_ptr _tmp2A6=({const char*_tmp18C="join_flow_and_rval: BottomFL outflow";_tag_dyneither(_tmp18C,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp2A6,_tag_dyneither(_tmp18B,sizeof(void*),0U));});});}}}_LL6:;};};}
# 946
static int Cyc_CfFlowInfo_absRval_lessthan_approx(void*ignore,void*r1,void*r2){
if(r1 == r2)return 1;
# 949
{struct _tuple13 _tmp195=({struct _tuple13 _tmp248;_tmp248.f1=r1,_tmp248.f2=r2;_tmp248;});struct _tuple13 _tmp196=_tmp195;int _tmp1A4;int _tmp1A3;struct _dyneither_ptr _tmp1A2;int _tmp1A1;int _tmp1A0;struct _dyneither_ptr _tmp19F;void*_tmp19E;void*_tmp19D;struct Cyc_CfFlowInfo_Place*_tmp19C;struct Cyc_CfFlowInfo_Place*_tmp19B;struct Cyc_Absyn_Vardecl*_tmp19A;void*_tmp199;struct Cyc_Absyn_Vardecl*_tmp198;void*_tmp197;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 8U){if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 8U){_LL1: _tmp19A=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1;_tmp199=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f2;_tmp198=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1;_tmp197=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f2;_LL2:
# 951
 return _tmp19A == _tmp198  && Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp199,_tmp197);}else{_LL3: _LL4:
 goto _LL6;}}else{if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 8U){_LL5: _LL6:
 return 0;}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 4U){if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 4U){_LL7: _tmp19C=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1;_tmp19B=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1;_LL8:
 return Cyc_CfFlowInfo_place_cmp(_tmp19C,_tmp19B)== 0;}else{_LL9: _LLA:
 goto _LLC;}}else{if(((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 4U){_LLB: _LLC:
 return 0;}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 5U){if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 5U){_LLD: _tmp19E=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1;_tmp19D=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1;_LLE:
 return Cyc_CfFlowInfo_absRval_lessthan_approx(ignore,_tmp19E,_tmp19D);}else{_LLF: _LL10:
 goto _LL12;}}else{if(((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 5U){_LL11: _LL12:
 return 0;}else{if(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 6U)switch(*((int*)_tmp196.f2)){case 6U: _LL13: _tmp1A4=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1).is_union;_tmp1A3=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f1).fieldnum;_tmp1A2=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->f2;_tmp1A1=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1).is_union;_tmp1A0=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f1).fieldnum;_tmp19F=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->f2;_LL14:
# 962
 if(_tmp1A4  && _tmp1A3 != _tmp1A0)return 0;
if((void**)_tmp1A2.curr == (void**)_tmp19F.curr)return 1;
{int i=0;for(0;i < _get_dyneither_size(_tmp1A2,sizeof(void*));++ i){
if(!({void*_tmp2A7=*((void**)_check_dyneither_subscript(_tmp1A2,sizeof(void*),i));((int(*)(int ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx)(0,_tmp2A7,*((void**)_check_dyneither_subscript(_tmp19F,sizeof(void*),i)));}))return 0;}}
return 1;case 0U: goto _LL15;case 1U: goto _LL17;default: goto _LL1D;}else{switch(*((int*)_tmp196.f2)){case 0U: _LL15: _LL16:
 goto _LL18;case 1U: _LL17: _LL18:
 return 0;default: if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp196.f1)->tag == 3U){if(((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp196.f2)->tag == 3U){_LL19: _LL1A:
 goto _LL0;}else{_LL1B: _LL1C:
 return 0;}}else{_LL1D: _LL1E:
# 977
 goto _LL0;}}}}}}}}}_LL0:;}{
# 979
struct _tuple19 _tmp1A5=({struct _tuple19 _tmp249;({enum Cyc_CfFlowInfo_InitLevel _tmp2A9=Cyc_CfFlowInfo_initlevel_approx(r1);_tmp249.f1=_tmp2A9;}),({enum Cyc_CfFlowInfo_InitLevel _tmp2A8=Cyc_CfFlowInfo_initlevel_approx(r2);_tmp249.f2=_tmp2A8;});_tmp249;});struct _tuple19 _tmp1A6=_tmp1A5;if(_tmp1A6.f1 == Cyc_CfFlowInfo_AllIL){_LL20: _LL21:
 goto _LL23;}else{if(_tmp1A6.f2 == Cyc_CfFlowInfo_NoneIL){_LL22: _LL23:
 return 1;}else{_LL24: _LL25:
 return 0;}}_LL1F:;};}
# 992
int Cyc_CfFlowInfo_flow_lessthan_approx(union Cyc_CfFlowInfo_FlowInfo f1,union Cyc_CfFlowInfo_FlowInfo f2){
# 994
struct _tuple20 _tmp1A7=({struct _tuple20 _tmp24A;_tmp24A.f1=f1,_tmp24A.f2=f2;_tmp24A;});struct _tuple20 _tmp1A8=_tmp1A7;struct Cyc_Dict_Dict _tmp1AC;struct Cyc_List_List*_tmp1AB;struct Cyc_Dict_Dict _tmp1AA;struct Cyc_List_List*_tmp1A9;if(((_tmp1A8.f1).BottomFL).tag == 1){_LL1: _LL2:
 return 1;}else{if(((_tmp1A8.f2).BottomFL).tag == 1){_LL3: _LL4:
 return 0;}else{_LL5: _tmp1AC=(((_tmp1A8.f1).ReachableFL).val).f1;_tmp1AB=(((_tmp1A8.f1).ReachableFL).val).f2;_tmp1AA=(((_tmp1A8.f2).ReachableFL).val).f1;_tmp1A9=(((_tmp1A8.f2).ReachableFL).val).f2;_LL6:
# 998
 if(_tmp1AC.t == _tmp1AA.t  && _tmp1AB == _tmp1A9)return 1;
return((int(*)(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_forall_intersect)((int(*)(void*ignore,void*r1,void*r2))Cyc_CfFlowInfo_absRval_lessthan_approx,_tmp1AC,_tmp1AA) && 
Cyc_Relations_relns_approx(_tmp1AB,_tmp1A9);}}_LL0:;}
# 1004
struct _tuple11 Cyc_CfFlowInfo_unname_rval(void*rv){
struct Cyc_List_List*names=0;
int done=0;
while(!done){
void*_tmp1AD=rv;struct Cyc_Absyn_Vardecl*_tmp1B0;void*_tmp1AF;if(((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->tag == 8U){_LL1: _tmp1B0=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->f1;_tmp1AF=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1AD)->f2;_LL2:
# 1010
({struct Cyc_List_List*_tmp2AA=({struct Cyc_List_List*_tmp1AE=_cycalloc(sizeof(*_tmp1AE));_tmp1AE->hd=_tmp1B0,_tmp1AE->tl=names;_tmp1AE;});names=_tmp2AA;});rv=_tmp1AF;goto _LL0;}else{_LL3: _LL4:
# 1012
 done=1;goto _LL0;}_LL0:;}
# 1015
return({struct _tuple11 _tmp24B;_tmp24B.f1=rv,_tmp24B.f2=names;_tmp24B;});}
# 1018
void Cyc_CfFlowInfo_print_initlevel(enum Cyc_CfFlowInfo_InitLevel il){
enum Cyc_CfFlowInfo_InitLevel _tmp1B1=il;if(_tmp1B1 == Cyc_CfFlowInfo_NoneIL){_LL1: _LL2:
({void*_tmp1B2=0U;({struct Cyc___cycFILE*_tmp2AC=Cyc_stderr;struct _dyneither_ptr _tmp2AB=({const char*_tmp1B3="uninitialized";_tag_dyneither(_tmp1B3,sizeof(char),14U);});Cyc_fprintf(_tmp2AC,_tmp2AB,_tag_dyneither(_tmp1B2,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
({void*_tmp1B4=0U;({struct Cyc___cycFILE*_tmp2AE=Cyc_stderr;struct _dyneither_ptr _tmp2AD=({const char*_tmp1B5="all-initialized";_tag_dyneither(_tmp1B5,sizeof(char),16U);});Cyc_fprintf(_tmp2AE,_tmp2AD,_tag_dyneither(_tmp1B4,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 1025
void Cyc_CfFlowInfo_print_root(void*root){
void*_tmp1B6=root;struct Cyc_Absyn_Exp*_tmp1C2;void*_tmp1C1;struct Cyc_Absyn_Vardecl*_tmp1C0;switch(*((int*)_tmp1B6)){case 0U: _LL1: _tmp1C0=((struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct*)_tmp1B6)->f1;_LL2:
# 1028
({struct Cyc_String_pa_PrintArg_struct _tmp1B9=({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0U,({struct _dyneither_ptr _tmp2AF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp1C0->name));_tmp24C.f1=_tmp2AF;});_tmp24C;});void*_tmp1B7[1U];_tmp1B7[0]=& _tmp1B9;({struct Cyc___cycFILE*_tmp2B1=Cyc_stderr;struct _dyneither_ptr _tmp2B0=({const char*_tmp1B8="Root(%s)";_tag_dyneither(_tmp1B8,sizeof(char),9U);});Cyc_fprintf(_tmp2B1,_tmp2B0,_tag_dyneither(_tmp1B7,sizeof(void*),1U));});});goto _LL0;case 1U: _LL3: _tmp1C2=((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp1B6)->f1;_tmp1C1=(void*)((struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct*)_tmp1B6)->f2;_LL4:
# 1030
({struct Cyc_String_pa_PrintArg_struct _tmp1BC=({struct Cyc_String_pa_PrintArg_struct _tmp24E;_tmp24E.tag=0U,({
struct _dyneither_ptr _tmp2B2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp1C2));_tmp24E.f1=_tmp2B2;});_tmp24E;});struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp24D;_tmp24D.tag=0U,({struct _dyneither_ptr _tmp2B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1C1));_tmp24D.f1=_tmp2B3;});_tmp24D;});void*_tmp1BA[2U];_tmp1BA[0]=& _tmp1BC,_tmp1BA[1]=& _tmp1BD;({struct Cyc___cycFILE*_tmp2B5=Cyc_stderr;struct _dyneither_ptr _tmp2B4=({const char*_tmp1BB="MallocPt(%s,%s)";_tag_dyneither(_tmp1BB,sizeof(char),16U);});Cyc_fprintf(_tmp2B5,_tmp2B4,_tag_dyneither(_tmp1BA,sizeof(void*),2U));});});goto _LL0;default: _LL5: _LL6:
# 1033
({void*_tmp1BE=0U;({struct Cyc___cycFILE*_tmp2B7=Cyc_stderr;struct _dyneither_ptr _tmp2B6=({const char*_tmp1BF="InitParam(_,_)";_tag_dyneither(_tmp1BF,sizeof(char),15U);});Cyc_fprintf(_tmp2B7,_tmp2B6,_tag_dyneither(_tmp1BE,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 1037
void Cyc_CfFlowInfo_print_path(struct Cyc_List_List*p){
{struct Cyc_List_List*x=p;for(0;x != 0;x=((struct Cyc_List_List*)_check_null(x))->tl){
void*_tmp1C3=(void*)x->hd;void*_tmp1C4=_tmp1C3;int _tmp1D0;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1C4)->tag == 0U){_LL1: _tmp1D0=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1C4)->f1;_LL2:
# 1041
({struct Cyc_Int_pa_PrintArg_struct _tmp1C7=({struct Cyc_Int_pa_PrintArg_struct _tmp24F;_tmp24F.tag=1U,_tmp24F.f1=(unsigned long)_tmp1D0;_tmp24F;});void*_tmp1C5[1U];_tmp1C5[0]=& _tmp1C7;({struct Cyc___cycFILE*_tmp2B9=Cyc_stderr;struct _dyneither_ptr _tmp2B8=({const char*_tmp1C6=".%d";_tag_dyneither(_tmp1C6,sizeof(char),4U);});Cyc_fprintf(_tmp2B9,_tmp2B8,_tag_dyneither(_tmp1C5,sizeof(void*),1U));});});
goto _LL0;}else{_LL3: _LL4:
# 1044
 if(x->tl != 0){
void*_tmp1C8=(void*)((struct Cyc_List_List*)_check_null(x->tl))->hd;void*_tmp1C9=_tmp1C8;int _tmp1CD;if(((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1C9)->tag == 0U){_LL6: _tmp1CD=((struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct*)_tmp1C9)->f1;_LL7:
# 1047
 x=x->tl;
({struct Cyc_Int_pa_PrintArg_struct _tmp1CC=({struct Cyc_Int_pa_PrintArg_struct _tmp250;_tmp250.tag=1U,_tmp250.f1=(unsigned long)_tmp1CD;_tmp250;});void*_tmp1CA[1U];_tmp1CA[0]=& _tmp1CC;({struct Cyc___cycFILE*_tmp2BB=Cyc_stderr;struct _dyneither_ptr _tmp2BA=({const char*_tmp1CB="->%d";_tag_dyneither(_tmp1CB,sizeof(char),5U);});Cyc_fprintf(_tmp2BB,_tmp2BA,_tag_dyneither(_tmp1CA,sizeof(void*),1U));});});
continue;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1053
({void*_tmp1CE=0U;({struct Cyc___cycFILE*_tmp2BD=Cyc_stderr;struct _dyneither_ptr _tmp2BC=({const char*_tmp1CF="*";_tag_dyneither(_tmp1CF,sizeof(char),2U);});Cyc_fprintf(_tmp2BD,_tmp2BC,_tag_dyneither(_tmp1CE,sizeof(void*),0U));});});}_LL0:;}}
# 1056
({void*_tmp1D1=0U;({struct Cyc___cycFILE*_tmp2BF=Cyc_stderr;struct _dyneither_ptr _tmp2BE=({const char*_tmp1D2=" ";_tag_dyneither(_tmp1D2,sizeof(char),2U);});Cyc_fprintf(_tmp2BF,_tmp2BE,_tag_dyneither(_tmp1D1,sizeof(void*),0U));});});}
# 1059
void Cyc_CfFlowInfo_print_place(struct Cyc_CfFlowInfo_Place*p){
Cyc_CfFlowInfo_print_root(p->root);
Cyc_CfFlowInfo_print_path(p->path);}
# 1064
void Cyc_CfFlowInfo_print_list(struct Cyc_List_List*x,void(*pr)(void*)){
int _tmp1D3=1;
while(x != 0){
if(!_tmp1D3){({void*_tmp1D4=0U;({struct Cyc___cycFILE*_tmp2C1=Cyc_stderr;struct _dyneither_ptr _tmp2C0=({const char*_tmp1D5=", ";_tag_dyneither(_tmp1D5,sizeof(char),3U);});Cyc_fprintf(_tmp2C1,_tmp2C0,_tag_dyneither(_tmp1D4,sizeof(void*),0U));});});_tmp1D3=0;}
pr(x->hd);
x=x->tl;}
# 1071
({void*_tmp1D6=0U;({struct Cyc___cycFILE*_tmp2C3=Cyc_stderr;struct _dyneither_ptr _tmp2C2=({const char*_tmp1D7="\n";_tag_dyneither(_tmp1D7,sizeof(char),2U);});Cyc_fprintf(_tmp2C3,_tmp2C2,_tag_dyneither(_tmp1D6,sizeof(void*),0U));});});}
# 1074
void Cyc_CfFlowInfo_print_absrval(void*rval){
void*_tmp1D8=rval;struct Cyc_Absyn_Vardecl*_tmp20E;void*_tmp20D;struct Cyc_Absyn_Exp*_tmp20C;int _tmp20B;void*_tmp20A;int _tmp209;int _tmp208;struct _dyneither_ptr _tmp207;void*_tmp206;struct Cyc_CfFlowInfo_Place*_tmp205;enum Cyc_CfFlowInfo_InitLevel _tmp204;enum Cyc_CfFlowInfo_InitLevel _tmp203;switch(*((int*)_tmp1D8)){case 0U: _LL1: _LL2:
({void*_tmp1D9=0U;({struct Cyc___cycFILE*_tmp2C5=Cyc_stderr;struct _dyneither_ptr _tmp2C4=({const char*_tmp1DA="Zero";_tag_dyneither(_tmp1DA,sizeof(char),5U);});Cyc_fprintf(_tmp2C5,_tmp2C4,_tag_dyneither(_tmp1D9,sizeof(void*),0U));});});goto _LL0;case 1U: _LL3: _LL4:
({void*_tmp1DB=0U;({struct Cyc___cycFILE*_tmp2C7=Cyc_stderr;struct _dyneither_ptr _tmp2C6=({const char*_tmp1DC="NotZeroAll";_tag_dyneither(_tmp1DC,sizeof(char),11U);});Cyc_fprintf(_tmp2C7,_tmp2C6,_tag_dyneither(_tmp1DB,sizeof(void*),0U));});});goto _LL0;case 2U: _LL5: _tmp203=((struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LL6:
({void*_tmp1DD=0U;({struct Cyc___cycFILE*_tmp2C9=Cyc_stderr;struct _dyneither_ptr _tmp2C8=({const char*_tmp1DE="Unknown(";_tag_dyneither(_tmp1DE,sizeof(char),9U);});Cyc_fprintf(_tmp2C9,_tmp2C8,_tag_dyneither(_tmp1DD,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_initlevel(_tmp203);
({void*_tmp1DF=0U;({struct Cyc___cycFILE*_tmp2CB=Cyc_stderr;struct _dyneither_ptr _tmp2CA=({const char*_tmp1E0=")";_tag_dyneither(_tmp1E0,sizeof(char),2U);});Cyc_fprintf(_tmp2CB,_tmp2CA,_tag_dyneither(_tmp1DF,sizeof(void*),0U));});});goto _LL0;case 3U: _LL7: _tmp204=((struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LL8:
({void*_tmp1E1=0U;({struct Cyc___cycFILE*_tmp2CD=Cyc_stderr;struct _dyneither_ptr _tmp2CC=({const char*_tmp1E2="Esc(";_tag_dyneither(_tmp1E2,sizeof(char),5U);});Cyc_fprintf(_tmp2CD,_tmp2CC,_tag_dyneither(_tmp1E1,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_initlevel(_tmp204);
({void*_tmp1E3=0U;({struct Cyc___cycFILE*_tmp2CF=Cyc_stderr;struct _dyneither_ptr _tmp2CE=({const char*_tmp1E4=")";_tag_dyneither(_tmp1E4,sizeof(char),2U);});Cyc_fprintf(_tmp2CF,_tmp2CE,_tag_dyneither(_tmp1E3,sizeof(void*),0U));});});goto _LL0;case 4U: _LL9: _tmp205=((struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LLA:
({void*_tmp1E5=0U;({struct Cyc___cycFILE*_tmp2D1=Cyc_stderr;struct _dyneither_ptr _tmp2D0=({const char*_tmp1E6="AddrOf(";_tag_dyneither(_tmp1E6,sizeof(char),8U);});Cyc_fprintf(_tmp2D1,_tmp2D0,_tag_dyneither(_tmp1E5,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_place(_tmp205);
({void*_tmp1E7=0U;({struct Cyc___cycFILE*_tmp2D3=Cyc_stderr;struct _dyneither_ptr _tmp2D2=({const char*_tmp1E8=")";_tag_dyneither(_tmp1E8,sizeof(char),2U);});Cyc_fprintf(_tmp2D3,_tmp2D2,_tag_dyneither(_tmp1E7,sizeof(void*),0U));});});goto _LL0;case 5U: _LLB: _tmp206=(void*)((struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_LLC:
({void*_tmp1E9=0U;({struct Cyc___cycFILE*_tmp2D5=Cyc_stderr;struct _dyneither_ptr _tmp2D4=({const char*_tmp1EA="UniquePtr(";_tag_dyneither(_tmp1EA,sizeof(char),11U);});Cyc_fprintf(_tmp2D5,_tmp2D4,_tag_dyneither(_tmp1E9,sizeof(void*),0U));});});Cyc_CfFlowInfo_print_absrval(_tmp206);
({void*_tmp1EB=0U;({struct Cyc___cycFILE*_tmp2D7=Cyc_stderr;struct _dyneither_ptr _tmp2D6=({const char*_tmp1EC=")";_tag_dyneither(_tmp1EC,sizeof(char),2U);});Cyc_fprintf(_tmp2D7,_tmp2D6,_tag_dyneither(_tmp1EB,sizeof(void*),0U));});});goto _LL0;case 6U: _LLD: _tmp209=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1).is_union;_tmp208=(((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1).fieldnum;_tmp207=((struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f2;_LLE:
# 1087
 if(_tmp209){
({void*_tmp1ED=0U;({struct Cyc___cycFILE*_tmp2D9=Cyc_stderr;struct _dyneither_ptr _tmp2D8=({const char*_tmp1EE="AggrUnion{";_tag_dyneither(_tmp1EE,sizeof(char),11U);});Cyc_fprintf(_tmp2D9,_tmp2D8,_tag_dyneither(_tmp1ED,sizeof(void*),0U));});});
if(_tmp208 != - 1)
({struct Cyc_Int_pa_PrintArg_struct _tmp1F1=({struct Cyc_Int_pa_PrintArg_struct _tmp251;_tmp251.tag=1U,_tmp251.f1=(unsigned long)_tmp208;_tmp251;});void*_tmp1EF[1U];_tmp1EF[0]=& _tmp1F1;({struct Cyc___cycFILE*_tmp2DB=Cyc_stderr;struct _dyneither_ptr _tmp2DA=({const char*_tmp1F0="tag == %d;";_tag_dyneither(_tmp1F0,sizeof(char),11U);});Cyc_fprintf(_tmp2DB,_tmp2DA,_tag_dyneither(_tmp1EF,sizeof(void*),1U));});});}else{
# 1092
({void*_tmp1F2=0U;({struct Cyc___cycFILE*_tmp2DD=Cyc_stderr;struct _dyneither_ptr _tmp2DC=({const char*_tmp1F3="AggrStruct{";_tag_dyneither(_tmp1F3,sizeof(char),12U);});Cyc_fprintf(_tmp2DD,_tmp2DC,_tag_dyneither(_tmp1F2,sizeof(void*),0U));});});}
{int i=0;for(0;i < _get_dyneither_size(_tmp207,sizeof(void*));++ i){
Cyc_CfFlowInfo_print_absrval(*((void**)_check_dyneither_subscript(_tmp207,sizeof(void*),i)));
if(i + 1 < _get_dyneither_size(_tmp207,sizeof(void*)))({void*_tmp1F4=0U;({struct Cyc___cycFILE*_tmp2DF=Cyc_stderr;struct _dyneither_ptr _tmp2DE=({const char*_tmp1F5=",";_tag_dyneither(_tmp1F5,sizeof(char),2U);});Cyc_fprintf(_tmp2DF,_tmp2DE,_tag_dyneither(_tmp1F4,sizeof(void*),0U));});});}}
# 1097
({void*_tmp1F6=0U;({struct Cyc___cycFILE*_tmp2E1=Cyc_stderr;struct _dyneither_ptr _tmp2E0=({const char*_tmp1F7="}";_tag_dyneither(_tmp1F7,sizeof(char),2U);});Cyc_fprintf(_tmp2E1,_tmp2E0,_tag_dyneither(_tmp1F6,sizeof(void*),0U));});});
goto _LL0;case 7U: _LLF: _tmp20C=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_tmp20B=((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f2;_tmp20A=(void*)((struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f3;_LL10:
# 1100
({struct Cyc_String_pa_PrintArg_struct _tmp1FA=({struct Cyc_String_pa_PrintArg_struct _tmp253;_tmp253.tag=0U,({struct _dyneither_ptr _tmp2E2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp20C));_tmp253.f1=_tmp2E2;});_tmp253;});struct Cyc_Int_pa_PrintArg_struct _tmp1FB=({struct Cyc_Int_pa_PrintArg_struct _tmp252;_tmp252.tag=1U,_tmp252.f1=(unsigned long)_tmp20B;_tmp252;});void*_tmp1F8[2U];_tmp1F8[0]=& _tmp1FA,_tmp1F8[1]=& _tmp1FB;({struct Cyc___cycFILE*_tmp2E4=Cyc_stderr;struct _dyneither_ptr _tmp2E3=({const char*_tmp1F9="[Consumed(%s,%d,";_tag_dyneither(_tmp1F9,sizeof(char),17U);});Cyc_fprintf(_tmp2E4,_tmp2E3,_tag_dyneither(_tmp1F8,sizeof(void*),2U));});});
Cyc_CfFlowInfo_print_absrval(_tmp20A);({void*_tmp1FC=0U;({struct Cyc___cycFILE*_tmp2E6=Cyc_stderr;struct _dyneither_ptr _tmp2E5=({const char*_tmp1FD=")]";_tag_dyneither(_tmp1FD,sizeof(char),3U);});Cyc_fprintf(_tmp2E6,_tmp2E5,_tag_dyneither(_tmp1FC,sizeof(void*),0U));});});
goto _LL0;default: _LL11: _tmp20E=((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f1;_tmp20D=(void*)((struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct*)_tmp1D8)->f2;_LL12:
# 1104
({struct Cyc_String_pa_PrintArg_struct _tmp200=({struct Cyc_String_pa_PrintArg_struct _tmp254;_tmp254.tag=0U,({struct _dyneither_ptr _tmp2E7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp20E->name));_tmp254.f1=_tmp2E7;});_tmp254;});void*_tmp1FE[1U];_tmp1FE[0]=& _tmp200;({struct Cyc___cycFILE*_tmp2E9=Cyc_stderr;struct _dyneither_ptr _tmp2E8=({const char*_tmp1FF="[NamedLocation(%s,";_tag_dyneither(_tmp1FF,sizeof(char),19U);});Cyc_fprintf(_tmp2E9,_tmp2E8,_tag_dyneither(_tmp1FE,sizeof(void*),1U));});});
Cyc_CfFlowInfo_print_absrval(_tmp20D);({void*_tmp201=0U;({struct Cyc___cycFILE*_tmp2EB=Cyc_stderr;struct _dyneither_ptr _tmp2EA=({const char*_tmp202=")]";_tag_dyneither(_tmp202,sizeof(char),3U);});Cyc_fprintf(_tmp2EB,_tmp2EA,_tag_dyneither(_tmp201,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 1110
static void Cyc_CfFlowInfo_print_flow_mapping(void*root,void*rval){
({void*_tmp20F=0U;({struct Cyc___cycFILE*_tmp2ED=Cyc_stderr;struct _dyneither_ptr _tmp2EC=({const char*_tmp210="    ";_tag_dyneither(_tmp210,sizeof(char),5U);});Cyc_fprintf(_tmp2ED,_tmp2EC,_tag_dyneither(_tmp20F,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_root(root);
({void*_tmp211=0U;({struct Cyc___cycFILE*_tmp2EF=Cyc_stderr;struct _dyneither_ptr _tmp2EE=({const char*_tmp212=" --> ";_tag_dyneither(_tmp212,sizeof(char),6U);});Cyc_fprintf(_tmp2EF,_tmp2EE,_tag_dyneither(_tmp211,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_absrval(rval);
({void*_tmp213=0U;({struct Cyc___cycFILE*_tmp2F1=Cyc_stderr;struct _dyneither_ptr _tmp2F0=({const char*_tmp214="\n";_tag_dyneither(_tmp214,sizeof(char),2U);});Cyc_fprintf(_tmp2F1,_tmp2F0,_tag_dyneither(_tmp213,sizeof(void*),0U));});});}
# 1118
void Cyc_CfFlowInfo_print_flowdict(struct Cyc_Dict_Dict d){
((void(*)(void(*f)(void*,void*),struct Cyc_Dict_Dict d))Cyc_Dict_iter)(Cyc_CfFlowInfo_print_flow_mapping,d);}
# 1122
void Cyc_CfFlowInfo_print_flow(union Cyc_CfFlowInfo_FlowInfo f){
union Cyc_CfFlowInfo_FlowInfo _tmp215=f;struct Cyc_Dict_Dict _tmp21F;struct Cyc_List_List*_tmp21E;if((_tmp215.BottomFL).tag == 1){_LL1: _LL2:
({void*_tmp216=0U;({struct Cyc___cycFILE*_tmp2F3=Cyc_stderr;struct _dyneither_ptr _tmp2F2=({const char*_tmp217="  BottomFL\n";_tag_dyneither(_tmp217,sizeof(char),12U);});Cyc_fprintf(_tmp2F3,_tmp2F2,_tag_dyneither(_tmp216,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _tmp21F=((_tmp215.ReachableFL).val).f1;_tmp21E=((_tmp215.ReachableFL).val).f2;_LL4:
# 1126
({void*_tmp218=0U;({struct Cyc___cycFILE*_tmp2F5=Cyc_stderr;struct _dyneither_ptr _tmp2F4=({const char*_tmp219="  ReachableFL:\n";_tag_dyneither(_tmp219,sizeof(char),16U);});Cyc_fprintf(_tmp2F5,_tmp2F4,_tag_dyneither(_tmp218,sizeof(void*),0U));});});
Cyc_CfFlowInfo_print_flowdict(_tmp21F);
({void*_tmp21A=0U;({struct Cyc___cycFILE*_tmp2F7=Cyc_stderr;struct _dyneither_ptr _tmp2F6=({const char*_tmp21B="\n  Relations: ";_tag_dyneither(_tmp21B,sizeof(char),15U);});Cyc_fprintf(_tmp2F7,_tmp2F6,_tag_dyneither(_tmp21A,sizeof(void*),0U));});});
Cyc_Relations_print_relns(Cyc_stderr,_tmp21E);
({void*_tmp21C=0U;({struct Cyc___cycFILE*_tmp2F9=Cyc_stderr;struct _dyneither_ptr _tmp2F8=({const char*_tmp21D="\n";_tag_dyneither(_tmp21D,sizeof(char),2U);});Cyc_fprintf(_tmp2F9,_tmp2F8,_tag_dyneither(_tmp21C,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
