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

#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#define _check_known_subscript_nullX(ptr,bound,elt_sz,index) ({ \
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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 252 "cycboot.h"
int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 72
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 77
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d);
# 80
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d);
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 92
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*k,void*v);
# 96
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*l);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 104
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*,int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);
# 126
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*k,void**ans);
# 131
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum);
# 135
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum);
# 139
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 143
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 147
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 149
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 153
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 158
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 162
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum);
# 170
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 173
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict);
# 176
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d);
# 181
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_Dict_Dict d);
# 186
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);
# 189
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d);
# 195
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 205
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 214
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 220
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 224
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct _tuple0{void*f1;void*f2;};
# 231
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 235
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d);
# 238
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*,struct Cyc_Dict_Dict d);
# 242
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 246
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*,int(*f)(void*,void*),struct Cyc_Dict_Dict d);
# 250
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 254
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 261
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 267
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);
# 274
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);
# 278
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);
# 281
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 287
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d);
# 294
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict);
# 301
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp);char Cyc_Dict_Absent[7U]="Absent";char Cyc_Dict_Present[8U]="Present";
# 27 "dict.cyc"
struct Cyc_Dict_Absent_exn_struct Cyc_Dict_Absent_val={Cyc_Dict_Absent};
struct Cyc_Dict_Present_exn_struct Cyc_Dict_Present_val={Cyc_Dict_Present};
# 30
enum Cyc_Dict_Color{Cyc_Dict_R  = 0U,Cyc_Dict_B  = 1U};struct Cyc_Dict_T{enum Cyc_Dict_Color color;const struct Cyc_Dict_T*left;const struct Cyc_Dict_T*right;struct _tuple0 key_val;};
# 41
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){
return({struct Cyc_Dict_Dict _tmpD3;_tmpD3.rel=comp,_tmpD3.r=r,_tmpD3.t=0;_tmpD3;});}
# 44
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){
return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
# 47
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){
# 49
return({struct Cyc_Dict_Dict _tmpD4;_tmpD4.rel=comp,_tmpD4.r=r,({const struct Cyc_Dict_T*_tmpEE=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->color=Cyc_Dict_B,_tmp0->left=0,_tmp0->right=0,(_tmp0->key_val).f1=key,(_tmp0->key_val).f2=data;_tmp0;});_tmpD4.t=_tmpEE;});_tmpD4;});}
# 51
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){
return Cyc_Dict_rsingleton(Cyc_Core_heap_region,comp,key,data);}
# 55
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
const struct Cyc_Dict_T*t2=t;
return({struct Cyc_Dict_Dict _tmpD5;_tmpD5.rel=d.rel,_tmpD5.r=r,_tmpD5.t=t2;_tmpD5;});}
# 61
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d){
return d.t == (const struct Cyc_Dict_T*)0;}
# 65
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){
int(*_tmp1)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmp2=d.t;
while(_tmp2 != (const struct Cyc_Dict_T*)0){
int _tmp3=_tmp1(key,(_tmp2->key_val).f1);
if(_tmp3 < 0)_tmp2=_tmp2->left;else{
if(_tmp3 > 0)_tmp2=_tmp2->right;else{
return 1;}}}
# 74
return 0;}
# 78
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){
int(*_tmp4)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmp5=d.t;
while(_tmp5 != (const struct Cyc_Dict_T*)0){
int _tmp6=_tmp4(key,(_tmp5->key_val).f1);
if(_tmp6 < 0)_tmp5=_tmp5->left;else{
if(_tmp6 > 0)_tmp5=_tmp5->right;else{
return(_tmp5->key_val).f2;}}}
# 87
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 90
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*key){
const struct Cyc_Dict_T*_tmp7=d.t;
while(_tmp7 != (const struct Cyc_Dict_T*)0){
int _tmp8=cmp(key,(_tmp7->key_val).f1);
if(_tmp8 < 0)_tmp7=_tmp7->left;else{
if(_tmp8 > 0)_tmp7=_tmp7->right;else{
return(_tmp7->key_val).f2;}}}
# 98
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 101
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*key){
int(*_tmp9)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmpA=d.t;
while(_tmpA != (const struct Cyc_Dict_T*)0){
int _tmpB=_tmp9(key,(_tmpA->key_val).f1);
if(_tmpB < 0)_tmpA=_tmpA->left;else{
if(_tmpB > 0)_tmpA=_tmpA->right;else{
return&(_tmpA->key_val).f2;}}}
# 110
return 0;}
# 113
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*key,void**ans_place){
int(*_tmpC)(void*,void*)=d.rel;
const struct Cyc_Dict_T*_tmpD=d.t;
while(_tmpD != (const struct Cyc_Dict_T*)0){
int _tmpE=_tmpC(key,(_tmpD->key_val).f1);
if(_tmpE < 0)_tmpD=_tmpD->left;else{
if(_tmpE > 0)_tmpD=_tmpD->right;else{
# 121
*ans_place=(_tmpD->key_val).f2;
return 1;}}}
# 125
return 0;}struct _tuple1{enum Cyc_Dict_Color f1;const struct Cyc_Dict_T*f2;const struct Cyc_Dict_T*f3;struct _tuple0 f4;};
# 128
static struct Cyc_Dict_T*Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){
# 131
struct _tuple1 _tmpF=quad;enum Cyc_Dict_Color _tmp3C;const struct Cyc_Dict_T*_tmp3B;const struct Cyc_Dict_T*_tmp3A;struct _tuple0 _tmp39;const struct Cyc_Dict_T*_tmp38;const struct Cyc_Dict_T*_tmp37;const struct Cyc_Dict_T*_tmp36;const struct Cyc_Dict_T*_tmp35;struct _tuple0 _tmp34;struct _tuple0 _tmp33;struct _tuple0 _tmp32;const struct Cyc_Dict_T*_tmp31;const struct Cyc_Dict_T*_tmp30;const struct Cyc_Dict_T*_tmp2F;struct _tuple0 _tmp2E;const struct Cyc_Dict_T*_tmp2D;struct _tuple0 _tmp2C;struct _tuple0 _tmp2B;const struct Cyc_Dict_T*_tmp2A;const struct Cyc_Dict_T*_tmp29;const struct Cyc_Dict_T*_tmp28;struct _tuple0 _tmp27;struct _tuple0 _tmp26;const struct Cyc_Dict_T*_tmp25;struct _tuple0 _tmp24;const struct Cyc_Dict_T*_tmp23;const struct Cyc_Dict_T*_tmp22;struct _tuple0 _tmp21;const struct Cyc_Dict_T*_tmp20;struct _tuple0 _tmp1F;const struct Cyc_Dict_T*_tmp1E;struct _tuple0 _tmp1D;if(_tmpF.f1 == Cyc_Dict_B){if(_tmpF.f2 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f2)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f2)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->left)->color == Cyc_Dict_R){_LL1: _tmp23=((_tmpF.f2)->left)->left;_tmp22=((_tmpF.f2)->left)->right;_tmp21=((_tmpF.f2)->left)->key_val;_tmp20=(_tmpF.f2)->right;_tmp1F=(_tmpF.f2)->key_val;_tmp1E=_tmpF.f3;_tmp1D=_tmpF.f4;_LL2:
# 133
 return({struct Cyc_Dict_T*_tmp12=_region_malloc(r,sizeof(*_tmp12));_tmp12->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpF0=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp10=_region_malloc(r,sizeof(*_tmp10));_tmp10->color=Cyc_Dict_B,_tmp10->left=_tmp23,_tmp10->right=_tmp22,_tmp10->key_val=_tmp21;_tmp10;});_tmp12->left=_tmpF0;}),({const struct Cyc_Dict_T*_tmpEF=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp11=_region_malloc(r,sizeof(*_tmp11));_tmp11->color=Cyc_Dict_B,_tmp11->left=_tmp20,_tmp11->right=_tmp1E,_tmp11->key_val=_tmp1D;_tmp11;});_tmp12->right=_tmpEF;}),_tmp12->key_val=_tmp1F;_tmp12;});}else{if(((const struct Cyc_Dict_T*)_tmpF.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->right)->color == Cyc_Dict_R)goto _LL3;else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->right)->color == Cyc_Dict_R){_LL3: _tmp2A=(_tmpF.f2)->left;_tmp29=((_tmpF.f2)->right)->left;_tmp28=((_tmpF.f2)->right)->right;_tmp27=((_tmpF.f2)->right)->key_val;_tmp26=(_tmpF.f2)->key_val;_tmp25=_tmpF.f3;_tmp24=_tmpF.f4;_LL4:
# 135
 return({struct Cyc_Dict_T*_tmp15=_region_malloc(r,sizeof(*_tmp15));_tmp15->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpF2=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp13=_region_malloc(r,sizeof(*_tmp13));_tmp13->color=Cyc_Dict_B,_tmp13->left=_tmp2A,_tmp13->right=_tmp29,_tmp13->key_val=_tmp26;_tmp13;});_tmp15->left=_tmpF2;}),({const struct Cyc_Dict_T*_tmpF1=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp14=_region_malloc(r,sizeof(*_tmp14));_tmp14->color=Cyc_Dict_B,_tmp14->left=_tmp28,_tmp14->right=_tmp25,_tmp14->key_val=_tmp24;_tmp14;});_tmp15->right=_tmpF1;}),_tmp15->key_val=_tmp27;_tmp15;});}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R){_LL5: _tmp31=_tmpF.f2;_tmp30=((_tmpF.f3)->left)->left;_tmp2F=((_tmpF.f3)->left)->right;_tmp2E=((_tmpF.f3)->left)->key_val;_tmp2D=(_tmpF.f3)->right;_tmp2C=(_tmpF.f3)->key_val;_tmp2B=_tmpF.f4;_LL6:
# 137
 return({struct Cyc_Dict_T*_tmp18=_region_malloc(r,sizeof(*_tmp18));_tmp18->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpF4=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp16=_region_malloc(r,sizeof(*_tmp16));_tmp16->color=Cyc_Dict_B,_tmp16->left=_tmp31,_tmp16->right=_tmp30,_tmp16->key_val=_tmp2B;_tmp16;});_tmp18->left=_tmpF4;}),({const struct Cyc_Dict_T*_tmpF3=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp17=_region_malloc(r,sizeof(*_tmp17));_tmp17->color=Cyc_Dict_B,_tmp17->left=_tmp2F,_tmp17->right=_tmp2D,_tmp17->key_val=_tmp2C;_tmp17;});_tmp18->right=_tmpF3;}),_tmp18->key_val=_tmp2E;_tmp18;});}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R){_LL7: _tmp38=_tmpF.f2;_tmp37=(_tmpF.f3)->left;_tmp36=((_tmpF.f3)->right)->left;_tmp35=((_tmpF.f3)->right)->right;_tmp34=((_tmpF.f3)->right)->key_val;_tmp33=(_tmpF.f3)->key_val;_tmp32=_tmpF.f4;_LL8:
# 139
 return({struct Cyc_Dict_T*_tmp1B=_region_malloc(r,sizeof(*_tmp1B));_tmp1B->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_tmpF6=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp19=_region_malloc(r,sizeof(*_tmp19));_tmp19->color=Cyc_Dict_B,_tmp19->left=_tmp38,_tmp19->right=_tmp37,_tmp19->key_val=_tmp32;_tmp19;});_tmp1B->left=_tmpF6;}),({const struct Cyc_Dict_T*_tmpF5=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1A=_region_malloc(r,sizeof(*_tmp1A));_tmp1A->color=Cyc_Dict_B,_tmp1A->left=_tmp36,_tmp1A->right=_tmp35,_tmp1A->key_val=_tmp34;_tmp1A;});_tmp1B->right=_tmpF5;}),_tmp1B->key_val=_tmp33;_tmp1B;});}else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{_LL9: _tmp3C=_tmpF.f1;_tmp3B=_tmpF.f2;_tmp3A=_tmpF.f3;_tmp39=_tmpF.f4;_LLA:
# 141
 return({struct Cyc_Dict_T*_tmp1C=_region_malloc(r,sizeof(*_tmp1C));_tmp1C->color=_tmp3C,_tmp1C->left=_tmp3B,_tmp1C->right=_tmp3A,_tmp1C->key_val=_tmp39;_tmp1C;});}_LL0:;}
# 145
static struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){
# 147
const struct Cyc_Dict_T*_tmp3D=t;enum Cyc_Dict_Color _tmp44;const struct Cyc_Dict_T*_tmp43;const struct Cyc_Dict_T*_tmp42;struct _tuple0 _tmp41;if(_tmp3D == 0){_LL1: _LL2:
 return({struct Cyc_Dict_T*_tmp3E=_region_malloc(r,sizeof(*_tmp3E));_tmp3E->color=Cyc_Dict_R,_tmp3E->left=0,_tmp3E->right=0,_tmp3E->key_val=key_val;_tmp3E;});}else{_LL3: _tmp44=_tmp3D->color;_tmp43=_tmp3D->left;_tmp42=_tmp3D->right;_tmp41=_tmp3D->key_val;_LL4: {
# 150
int _tmp3F=rel(key_val.f1,_tmp41.f1);
if(_tmp3F < 0)return({struct _RegionHandle*_tmpF8=r;Cyc_Dict_balance(_tmpF8,({struct _tuple1 _tmpD6;_tmpD6.f1=_tmp44,({const struct Cyc_Dict_T*_tmpF7=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,_tmp43);_tmpD6.f2=_tmpF7;}),_tmpD6.f3=_tmp42,_tmpD6.f4=_tmp41;_tmpD6;}));});else{
if(_tmp3F > 0)return({struct _RegionHandle*_tmpFA=r;Cyc_Dict_balance(_tmpFA,({struct _tuple1 _tmpD7;_tmpD7.f1=_tmp44,_tmpD7.f2=_tmp43,({const struct Cyc_Dict_T*_tmpF9=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,_tmp42);_tmpD7.f3=_tmpF9;}),_tmpD7.f4=_tmp41;_tmpD7;}));});else{
return({struct Cyc_Dict_T*_tmp40=_region_malloc(r,sizeof(*_tmp40));_tmp40->color=_tmp44,_tmp40->left=_tmp43,_tmp40->right=_tmp42,_tmp40->key_val=key_val;_tmp40;});}}}}_LL0:;}
# 157
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){
struct Cyc_Dict_T*_tmp45=({struct _RegionHandle*_tmpFD=d.r;int(*_tmpFC)(void*,void*)=d.rel;struct _tuple0 _tmpFB=({struct _tuple0 _tmpD9;_tmpD9.f1=key,_tmpD9.f2=data;_tmpD9;});Cyc_Dict_ins(_tmpFD,_tmpFC,_tmpFB,d.t);});
((struct Cyc_Dict_T*)_check_null(_tmp45))->color=Cyc_Dict_B;{
struct Cyc_Dict_Dict _tmp46=({struct Cyc_Dict_Dict _tmpD8;_tmpD8.rel=d.rel,_tmpD8.r=d.r,_tmpD8.t=(const struct Cyc_Dict_T*)_tmp45;_tmpD8;});
return _tmp46;};}
# 165
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,void*data){
if(Cyc_Dict_member(d,key))
(int)_throw((void*)& Cyc_Dict_Absent_val);
return Cyc_Dict_insert(d,key,data);}
# 171
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*kds){
# 173
for(0;kds != 0;kds=kds->tl){
({struct Cyc_Dict_Dict _tmpFE=Cyc_Dict_insert(d,(((struct _tuple0*)kds->hd)[0]).f1,(((struct _tuple0*)kds->hd)[0]).f2);d=_tmpFE;});}
return d;}
# 178
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*t,void*accum){
const struct Cyc_Dict_T*_tmp47=t;const struct Cyc_Dict_T*_tmp4B;const struct Cyc_Dict_T*_tmp4A;void*_tmp49;void*_tmp48;_LL1: _tmp4B=_tmp47->left;_tmp4A=_tmp47->right;_tmp49=(_tmp47->key_val).f1;_tmp48=(_tmp47->key_val).f2;_LL2:;
if(_tmp4B != (const struct Cyc_Dict_T*)0)({void*_tmpFF=Cyc_Dict_fold_tree(f,_tmp4B,accum);accum=_tmpFF;});
({void*_tmp100=f(_tmp49,_tmp48,accum);accum=_tmp100;});
if(_tmp4A != (const struct Cyc_Dict_T*)0)({void*_tmp101=Cyc_Dict_fold_tree(f,_tmp4A,accum);accum=_tmp101;});
return accum;}
# 186
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp4C=d.t;
if(_tmp4C == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree(f,_tmp4C,accum);}
# 193
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){
# 195
const struct Cyc_Dict_T*_tmp4D=t;const struct Cyc_Dict_T*_tmp51;const struct Cyc_Dict_T*_tmp50;void*_tmp4F;void*_tmp4E;_LL1: _tmp51=_tmp4D->left;_tmp50=_tmp4D->right;_tmp4F=(_tmp4D->key_val).f1;_tmp4E=(_tmp4D->key_val).f2;_LL2:;
if(_tmp51 != (const struct Cyc_Dict_T*)0)
({void*_tmp102=Cyc_Dict_fold_tree_c(f,env,_tmp51,accum);accum=_tmp102;});
({void*_tmp103=f(env,_tmp4F,_tmp4E,accum);accum=_tmp103;});
if(_tmp50 != (const struct Cyc_Dict_T*)0)
({void*_tmp104=Cyc_Dict_fold_tree_c(f,env,_tmp50,accum);accum=_tmp104;});
return accum;}
# 204
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp52=d.t;
if(_tmp52 == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree_c(f,env,_tmp52,accum);}
# 211
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp53=t;const struct Cyc_Dict_T*_tmp57;const struct Cyc_Dict_T*_tmp56;void*_tmp55;void*_tmp54;_LL1: _tmp57=_tmp53->left;_tmp56=_tmp53->right;_tmp55=(_tmp53->key_val).f1;_tmp54=(_tmp53->key_val).f2;_LL2:;
if(_tmp57 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,_tmp57);
f(_tmp55,_tmp54);
if(_tmp56 != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,_tmp56);}
# 218
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp58=d.t;
if(_tmp58 != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree(f,_tmp58);}
# 224
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp59=t;const struct Cyc_Dict_T*_tmp5D;const struct Cyc_Dict_T*_tmp5C;void*_tmp5B;void*_tmp5A;_LL1: _tmp5D=_tmp59->left;_tmp5C=_tmp59->right;_tmp5B=(_tmp59->key_val).f1;_tmp5A=(_tmp59->key_val).f2;_LL2:;
if(_tmp5D != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,_tmp5D);
f(env,_tmp5B,_tmp5A);
if(_tmp5C != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,_tmp5C);}
# 231
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp5E=d.t;
if(_tmp5E != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree_c(f,env,_tmp5E);}
# 237
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp5F=t;const struct Cyc_Dict_T*_tmp63;const struct Cyc_Dict_T*_tmp62;void*_tmp61;void*_tmp60;_LL1: _tmp63=_tmp5F->left;_tmp62=_tmp5F->right;_tmp61=(_tmp5F->key_val).f1;_tmp60=(_tmp5F->key_val).f2;_LL2:;
if(_tmp63 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,_tmp63);
f(_tmp61,_tmp60);
if(_tmp62 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,_tmp62);}
# 244
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp64=d.t;
if(_tmp64 != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree(f,_tmp64);}
# 250
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp65=t;const struct Cyc_Dict_T*_tmp69;const struct Cyc_Dict_T*_tmp68;void*_tmp67;void*_tmp66;_LL1: _tmp69=_tmp65->left;_tmp68=_tmp65->right;_tmp67=(_tmp65->key_val).f1;_tmp66=(_tmp65->key_val).f2;_LL2:;
if(_tmp69 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,_tmp69);
f(env,_tmp67,_tmp66);
if(_tmp68 != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,_tmp68);}
# 257
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp6A=d.t;
if(_tmp6A != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree_c(f,env,_tmp6A);}
# 263
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){
*cnt=*cnt + 1;}
# 266
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){
int num=0;
((void(*)(void(*f)(int*,void*,void*),int*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_count_elem,& num,d);
return num;}struct _tuple2{void(*f1)(void*,void*);struct Cyc_Dict_Dict f2;};
# 272
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){
# 274
struct _tuple2*_tmp6B=env;void(*_tmp6D)(void*,void*);struct Cyc_Dict_Dict _tmp6C;_LL1: _tmp6D=_tmp6B->f1;_tmp6C=_tmp6B->f2;_LL2:;
({void(*_tmp106)(void*,void*)=_tmp6D;void*_tmp105=b1;_tmp106(_tmp105,Cyc_Dict_lookup(_tmp6C,a));});}
# 278
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 281
struct _tuple2 _tmp6E=({struct _tuple2 _tmpDA;_tmpDA.f1=f,_tmpDA.f2=d2;_tmpDA;});
((void(*)(void(*f)(struct _tuple2*,void*,void*),struct _tuple2*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_f,& _tmp6E,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 285
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){
# 287
struct _tuple3*_tmp6F=env;void(*_tmp72)(void*,void*,void*);struct Cyc_Dict_Dict _tmp71;void*_tmp70;_LL1: _tmp72=_tmp6F->f1;_tmp71=_tmp6F->f2;_tmp70=_tmp6F->f3;_LL2:;
({void(*_tmp109)(void*,void*,void*)=_tmp72;void*_tmp108=_tmp70;void*_tmp107=b1;_tmp109(_tmp108,_tmp107,Cyc_Dict_lookup(_tmp71,a));});}
# 291
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 294
struct _tuple3 _tmp73=({struct _tuple3 _tmpDB;_tmpDB.f1=f,_tmpDB.f2=d2,_tmpDB.f3=inner_env;_tmpDB;});
((void(*)(void(*f)(struct _tuple3*,void*,void*),struct _tuple3*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Dict_iter2_c_f,& _tmp73,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 298
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){
# 302
struct _tuple4*_tmp74=env;void*(*_tmp77)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict _tmp76;void*_tmp75;_LL1: _tmp77=_tmp74->f1;_tmp76=_tmp74->f2;_tmp75=_tmp74->f3;_LL2:;
return({void*(*_tmp10E)(void*,void*,void*,void*,void*)=_tmp77;void*_tmp10D=_tmp75;void*_tmp10C=a;void*_tmp10B=b1;void*_tmp10A=Cyc_Dict_lookup(_tmp76,a);_tmp10E(_tmp10D,_tmp10C,_tmp10B,_tmp10A,accum);});}
# 306
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){
# 310
struct _tuple4 _tmp78=({struct _tuple4 _tmpDC;_tmpDC.f1=f,_tmpDC.f2=d2,_tmpDC.f3=inner_env;_tmpDC;});
return((void*(*)(void*(*f)(struct _tuple4*,void*,void*,void*),struct _tuple4*env,struct Cyc_Dict_Dict d,void*accum))Cyc_Dict_fold_c)(Cyc_Dict_fold2_c_f,& _tmp78,d1,accum);}
# 314
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const struct Cyc_Dict_T*t){
if(t == (const struct Cyc_Dict_T*)0)return 0;else{
# 317
struct Cyc_Dict_T _tmp79=*t;struct Cyc_Dict_T _tmp7A=_tmp79;enum Cyc_Dict_Color _tmp81;const struct Cyc_Dict_T*_tmp80;const struct Cyc_Dict_T*_tmp7F;struct _tuple0 _tmp7E;_LL1: _tmp81=_tmp7A.color;_tmp80=_tmp7A.left;_tmp7F=_tmp7A.right;_tmp7E=_tmp7A.key_val;_LL2:;{
const struct Cyc_Dict_T*_tmp7B=Cyc_Dict_copy_tree(r2,_tmp80);
const struct Cyc_Dict_T*_tmp7C=Cyc_Dict_copy_tree(r2,_tmp7F);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp7D=_region_malloc(r2,sizeof(*_tmp7D));_tmp7D->color=_tmp81,_tmp7D->left=_tmp7B,_tmp7D->right=_tmp7C,_tmp7D->key_val=_tmp7E;_tmp7D;});};}}
# 324
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
return({struct Cyc_Dict_Dict _tmpDD;_tmpDD.rel=d.rel,_tmpDD.r=r2,({const struct Cyc_Dict_T*_tmp10F=Cyc_Dict_copy_tree(r2,d.t);_tmpDD.t=_tmp10F;});_tmpDD;});}
# 328
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}
# 332
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct Cyc_Dict_T*t){
# 334
const struct Cyc_Dict_T*_tmp82=t;enum Cyc_Dict_Color _tmp8B;const struct Cyc_Dict_T*_tmp8A;const struct Cyc_Dict_T*_tmp89;void*_tmp88;void*_tmp87;_LL1: _tmp8B=_tmp82->color;_tmp8A=_tmp82->left;_tmp89=_tmp82->right;_tmp88=(_tmp82->key_val).f1;_tmp87=(_tmp82->key_val).f2;_LL2:;{
const struct Cyc_Dict_T*_tmp83=_tmp8A == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,_tmp8A);
void*_tmp84=f(_tmp87);
const struct Cyc_Dict_T*_tmp85=_tmp89 == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,_tmp89);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp86=_region_malloc(r,sizeof(*_tmp86));_tmp86->color=_tmp8B,_tmp86->left=_tmp83,_tmp86->right=_tmp85,(_tmp86->key_val).f1=_tmp88,(_tmp86->key_val).f2=_tmp84;_tmp86;});};}
# 341
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp8C=d.t;
if(_tmp8C == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmpDE;_tmpDE.rel=d.rel,_tmpDE.r=r,_tmpDE.t=0;_tmpDE;});
return({struct Cyc_Dict_Dict _tmpDF;_tmpDF.rel=d.rel,_tmpDF.r=r,({const struct Cyc_Dict_T*_tmp110=Cyc_Dict_map_tree(r,f,_tmp8C);_tmpDF.t=_tmp110;});_tmpDF;});}
# 348
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
# 352
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Dict_T*t){
# 354
const struct Cyc_Dict_T*_tmp8D=t;enum Cyc_Dict_Color _tmp96;const struct Cyc_Dict_T*_tmp95;const struct Cyc_Dict_T*_tmp94;void*_tmp93;void*_tmp92;_LL1: _tmp96=_tmp8D->color;_tmp95=_tmp8D->left;_tmp94=_tmp8D->right;_tmp93=(_tmp8D->key_val).f1;_tmp92=(_tmp8D->key_val).f2;_LL2:;{
const struct Cyc_Dict_T*_tmp8E=
_tmp95 == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,_tmp95);
void*_tmp8F=f(env,_tmp92);
const struct Cyc_Dict_T*_tmp90=
_tmp94 == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,_tmp94);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp91=_region_malloc(r,sizeof(*_tmp91));_tmp91->color=_tmp96,_tmp91->left=_tmp8E,_tmp91->right=_tmp90,(_tmp91->key_val).f1=_tmp93,(_tmp91->key_val).f2=_tmp8F;_tmp91;});};}
# 363
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
# 365
const struct Cyc_Dict_T*_tmp97=d.t;
if(_tmp97 == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmpE0;_tmpE0.rel=d.rel,_tmpE0.r=r,_tmpE0.t=0;_tmpE0;});
return({struct Cyc_Dict_Dict _tmpE1;_tmpE1.rel=d.rel,_tmpE1.r=r,({const struct Cyc_Dict_T*_tmp111=Cyc_Dict_map_tree_c(r,f,env,_tmp97);_tmpE1.t=_tmp111;});_tmpE1;});}
# 371
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}
# 375
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
if(d.t == (const struct Cyc_Dict_T*)0)
(int)_throw((void*)& Cyc_Dict_Absent_val);
return({struct _tuple0*_tmp98=_region_malloc(r,sizeof(*_tmp98));_tmp98->f1=((d.t)->key_val).f1,_tmp98->f2=((d.t)->key_val).f2;_tmp98;});}
# 381
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp99=t;const struct Cyc_Dict_T*_tmp9D;const struct Cyc_Dict_T*_tmp9C;void*_tmp9B;void*_tmp9A;_LL1: _tmp9D=_tmp99->left;_tmp9C=_tmp99->right;_tmp9B=(_tmp99->key_val).f1;_tmp9A=(_tmp99->key_val).f2;_LL2:;
return
((_tmp9D == (const struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,_tmp9D)) && 
f(env,_tmp9B,_tmp9A)) && (
_tmp9C == (const struct Cyc_Dict_T*)0  || Cyc_Dict_forall_tree_c(f,env,_tmp9C));}
# 389
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp9E=d.t;
if(_tmp9E == (const struct Cyc_Dict_T*)0)
return 1;
return Cyc_Dict_forall_tree_c(f,env,_tmp9E);}struct _tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;};
# 396
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
# 398
struct _tuple5*_tmp9F=env;int(*_tmpA1)(void*,void*,void*);struct Cyc_Dict_Dict _tmpA0;_LL1: _tmpA1=_tmp9F->f1;_tmpA0=_tmp9F->f2;_LL2:;
if(Cyc_Dict_member(_tmpA0,a))
return({int(*_tmp114)(void*,void*,void*)=_tmpA1;void*_tmp113=a;void*_tmp112=b;_tmp114(_tmp113,_tmp112,Cyc_Dict_lookup(_tmpA0,a));});
return 1;}
# 405
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
struct _tuple5 _tmpA2=({struct _tuple5 _tmpE2;_tmpE2.f1=f,_tmpE2.f2=d2;_tmpE2;});
return((int(*)(int(*f)(struct _tuple5*,void*,void*),struct _tuple5*env,struct Cyc_Dict_Dict d))Cyc_Dict_forall_c)(Cyc_Dict_forall_intersect_f,& _tmpA2,d1);}struct _tuple6{void*(*f1)(void*,void*,void*,void*);void*f2;};
# 411
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){
# 415
if(Cyc_Dict_member(*d1,a)){
# 417
void*_tmpA3=Cyc_Dict_lookup(*d1,a);
void*_tmpA4=((*env).f1)((*env).f2,a,_tmpA3,b);
if(_tmpA4 != _tmpA3)
({struct Cyc_Dict_Dict _tmp115=Cyc_Dict_insert(*d1,a,_tmpA4);*d1=_tmp115;});
return d1;}
# 423
({struct Cyc_Dict_Dict _tmp116=Cyc_Dict_insert(*d1,a,b);*d1=_tmp116;});
return d1;}
# 427
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 430
if((int)d1.t == (int)d2.t)return d1;
if(d1.t == (const struct Cyc_Dict_T*)0)return d2;
if(d2.t == (const struct Cyc_Dict_T*)0)return d1;{
struct _tuple6 _tmpA5=({struct _tuple6 _tmpE3;_tmpE3.f1=f,_tmpE3.f2=env;_tmpE3;});
((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_union_f,& _tmpA5,d2,& d1);
return d1;};}
# 438
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 441
const struct Cyc_Dict_T*_tmpA6=d2.t;
if(_tmpA6 == (const struct Cyc_Dict_T*)0)return d2;
if((int)d1.t == (int)_tmpA6)return d2;{
const struct Cyc_Dict_T*ans_tree=0;
struct _RegionHandle _tmpA7=_new_region("temp");struct _RegionHandle*temp=& _tmpA7;_push_region(temp);{
# 447
struct _dyneither_ptr queue=_tag_dyneither(({unsigned int _tmpB1=16U;const struct Cyc_Dict_T**_tmpB0=({struct _RegionHandle*_tmp117=temp;_region_malloc(_tmp117,_check_times(_tmpB1,sizeof(const struct Cyc_Dict_T*)));});({{unsigned int _tmpE6=16U;unsigned int i;for(i=0;i < _tmpE6;++ i){_tmpB0[i]=_tmpA6;}}0;});_tmpB0;}),sizeof(const struct Cyc_Dict_T*),16U);
int ind=0;
while(ind != - 1){
const struct Cyc_Dict_T*_tmpA8=*((const struct Cyc_Dict_T**)_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),ind --));const struct Cyc_Dict_T*_tmpA9=_tmpA8;const struct Cyc_Dict_T*_tmpAF;const struct Cyc_Dict_T*_tmpAE;void*_tmpAD;void*_tmpAC;_LL1: _tmpAF=_tmpA9->left;_tmpAE=_tmpA9->right;_tmpAD=(_tmpA9->key_val).f1;_tmpAC=(_tmpA9->key_val).f2;_LL2:;
if(ind + 2 >= _get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*)))
({struct _dyneither_ptr _tmp119=({unsigned int _tmpAB=_get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*))* 2;const struct Cyc_Dict_T**_tmpAA=({struct _RegionHandle*_tmp118=temp;_region_malloc(_tmp118,_check_times(_tmpAB,sizeof(const struct Cyc_Dict_T*)));});({{unsigned int _tmpE4=_get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*))* 2;unsigned int i;for(i=0;i < _tmpE4;++ i){
i < _get_dyneither_size(queue,sizeof(const struct Cyc_Dict_T*))?_tmpAA[i]=((const struct Cyc_Dict_T**)queue.curr)[(int)i]:(_tmpAA[i]=_tmpA6);}}0;});_tag_dyneither(_tmpAA,sizeof(const struct Cyc_Dict_T*),_tmpAB);});
# 452
queue=_tmp119;});
# 454
if(_tmpAF != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=_tmpAF;
if(_tmpAE != (const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_dyneither_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=_tmpAE;
if(Cyc_Dict_member(d1,_tmpAD))
({const struct Cyc_Dict_T*_tmp122=(const struct Cyc_Dict_T*)({struct _RegionHandle*_tmp121=d2.r;int(*_tmp120)(void*,void*)=d2.rel;struct _tuple0 _tmp11F=({struct _tuple0 _tmpE5;_tmpE5.f1=_tmpAD,({void*_tmp11E=({void*(*_tmp11D)(void*,void*,void*,void*)=f;void*_tmp11C=env;void*_tmp11B=_tmpAD;void*_tmp11A=Cyc_Dict_lookup(d1,_tmpAD);_tmp11D(_tmp11C,_tmp11B,_tmp11A,_tmpAC);});_tmpE5.f2=_tmp11E;});_tmpE5;});Cyc_Dict_ins(_tmp121,_tmp120,_tmp11F,ans_tree);});ans_tree=_tmp122;});}}{
# 460
struct Cyc_Dict_Dict _tmpB2=({struct Cyc_Dict_Dict _tmpE7;_tmpE7.rel=d2.rel,_tmpE7.r=d2.r,_tmpE7.t=ans_tree;_tmpE7;});_npop_handler(0U);return _tmpB2;};
# 445
;_pop_region(temp);};}
# 463
static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,void*b1,void*b2){
return f(a,b1,b2);}
# 467
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 470
return((struct Cyc_Dict_Dict(*)(void*(*f)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*env)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_intersect_c)(Cyc_Dict_intersect_f,f,d1,d2);}
# 473
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
# 475
return({struct Cyc_List_List*_tmpB4=_region_malloc(r,sizeof(*_tmpB4));({struct _tuple0*_tmp123=({struct _tuple0*_tmpB3=_region_malloc(r,sizeof(*_tmpB3));_tmpB3->f1=k,_tmpB3->f2=v;_tmpB3;});_tmpB4->hd=_tmp123;}),_tmpB4->tl=accum;_tmpB4;});}
# 478
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*f)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*env,struct Cyc_Dict_Dict d,struct Cyc_List_List*accum))Cyc_Dict_fold_c)(Cyc_Dict_to_list_f,r,d,0);}
# 481
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){
return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,void*);struct _RegionHandle*f2;};
# 485
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 488
struct _tuple7*_tmpB5=env;int(*_tmpB7)(void*,void*);struct _RegionHandle*_tmpB6;_LL1: _tmpB7=_tmpB5->f1;_tmpB6=_tmpB5->f2;_LL2:;
if(_tmpB7(x,y))
({struct Cyc_Dict_Dict _tmp124=Cyc_Dict_insert(*acc,x,y);*acc=_tmp124;});
# 492
return acc;}
# 495
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict d){
# 497
struct _tuple7 _tmpB8=({struct _tuple7 _tmpE8;_tmpE8.f1=f,_tmpE8.f2=r2;_tmpE8;});
struct Cyc_Dict_Dict _tmpB9=Cyc_Dict_rempty(r2,d.rel);
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_f,& _tmpB8,d,& _tmpB9);}
# 507
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,void*,void*);void*f2;struct _RegionHandle*f3;};
# 512
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 516
struct _tuple8*_tmpBA=env;int(*_tmpBD)(void*,void*,void*);void*_tmpBC;struct _RegionHandle*_tmpBB;_LL1: _tmpBD=_tmpBA->f1;_tmpBC=_tmpBA->f2;_tmpBB=_tmpBA->f3;_LL2:;
if(_tmpBD(_tmpBC,x,y))
({struct Cyc_Dict_Dict _tmp125=Cyc_Dict_insert(*acc,x,y);*acc=_tmp125;});
return acc;}
# 522
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 524
struct _tuple8 _tmpBE=({struct _tuple8 _tmpE9;_tmpE9.f1=f,_tmpE9.f2=f_env,_tmpE9.f3=r2;_tmpE9;});
struct Cyc_Dict_Dict _tmpBF=Cyc_Dict_rempty(r2,d.rel);
return*((struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*f)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*env,struct Cyc_Dict_Dict d,struct Cyc_Dict_Dict*accum))Cyc_Dict_fold_c)(Cyc_Dict_filter_c_f,& _tmpBE,d,& _tmpBF);}
# 528
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 530
return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,f_env,d);}
# 534
static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return !Cyc_Dict_member(*d,x);}
# 538
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*r2,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 540
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_difference_f,& d2,d1);}
# 542
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 544
return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*f1)(void*,void*);void*f2;};
# 547
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){
struct _tuple9*_tmpC0=env;int(*_tmpC2)(void*,void*);void*_tmpC1;_LL1: _tmpC2=_tmpC0->f1;_tmpC1=_tmpC0->f2;_LL2:;
return _tmpC2(_tmpC1,x)!= 0;}
# 552
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(r2,d);{
struct _tuple9 _tmpC3=({struct _tuple9 _tmpEA;_tmpEA.f1=d.rel,_tmpEA.f2=x;_tmpEA;});
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(r2,Cyc_Dict_delete_f,& _tmpC3,d);};}
# 558
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return d;{
struct _tuple9 _tmpC4=({struct _tuple9 _tmpEB;_tmpEB.f1=d.rel,_tmpEB.f2=x;_tmpEB;});
return((struct Cyc_Dict_Dict(*)(struct _RegionHandle*r2,int(*f)(struct _tuple9*,void*,void*),struct _tuple9*f_env,struct Cyc_Dict_Dict d))Cyc_Dict_rfilter_c)(d.r,Cyc_Dict_delete_f,& _tmpC4,d);};}
# 564
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct _dyneither_ptr f1;int f2;};
# 568
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
# 571
struct _tuple10*_tmpC5=stk;struct _dyneither_ptr _tmpC9;int*_tmpC8;_LL1: _tmpC9=_tmpC5->f1;_tmpC8=(int*)& _tmpC5->f2;_LL2:;{
int _tmpC6=*_tmpC8;
if(_tmpC6 == - 1)
return 0;{
const struct Cyc_Dict_T*_tmpC7=*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmpC9,sizeof(const struct Cyc_Dict_T*),_tmpC6));
*dest=((const struct Cyc_Dict_T*)_check_null(_tmpC7))->key_val;
-- _tmpC6;
if((unsigned int)_tmpC7->left)
*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmpC9,sizeof(const struct Cyc_Dict_T*),++ _tmpC6))=_tmpC7->left;
if((unsigned int)_tmpC7->right)
*((const struct Cyc_Dict_T**)_check_dyneither_subscript(_tmpC9,sizeof(const struct Cyc_Dict_T*),++ _tmpC6))=_tmpC7->right;
*_tmpC8=_tmpC6;
return 1;};};}
# 586
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){
# 591
int half_max_size=1;
const struct Cyc_Dict_T*_tmpCA=d.t;
while(_tmpCA != (const struct Cyc_Dict_T*)0){
_tmpCA=_tmpCA->left;
++ half_max_size;}
# 597
_tmpCA=d.t;{
struct _dyneither_ptr _tmpCB=({unsigned int _tmpCE=(unsigned int)(2 * half_max_size);const struct Cyc_Dict_T**_tmpCD=({struct _RegionHandle*_tmp126=rgn;_region_malloc(_tmp126,_check_times(_tmpCE,sizeof(const struct Cyc_Dict_T*)));});({{unsigned int _tmpED=(unsigned int)(2 * half_max_size);unsigned int i;for(i=0;i < _tmpED;++ i){_tmpCD[i]=_tmpCA;}}0;});_tag_dyneither(_tmpCD,sizeof(const struct Cyc_Dict_T*),_tmpCE);});
return({struct Cyc_Iter_Iter _tmpEC;({struct _tuple10*_tmp127=({struct _tuple10*_tmpCC=_region_malloc(rgn,sizeof(*_tmpCC));_tmpCC->f1=_tmpCB,(unsigned int)_tmpCA?_tmpCC->f2=0:(_tmpCC->f2=- 1);_tmpCC;});_tmpEC.env=_tmp127;}),_tmpEC.next=Cyc_Dict_iter_f;_tmpEC;});};}
# 607
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
# 614
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
int len=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(dict_list);
# 618
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp128=({const char*_tmpCF="Dict::marshal: Write failure";_tag_dyneither(_tmpCF,sizeof(char),29U);});_tmpD0->f1=_tmp128;});_tmpD0;}));
# 620
while(dict_list != 0){
({void*_tmp129=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_key)(env,fp,(struct _tuple0*)dict_list->hd);env=_tmp129;});
({void*_tmp12A=((void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_val)(env,fp,(struct _tuple0*)dict_list->hd);env=_tmp12A;});
dict_list=dict_list->tl;}
# 625
return env;}
# 628
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){
# 635
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);
int len=Cyc_getw(fp);
if(len == - 1)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmp12B=({const char*_tmpD1="Dict::unmarshal: list length is -1";_tag_dyneither(_tmpD1,sizeof(char),35U);});_tmpD2->f1=_tmp12B;});_tmpD2;}));
# 640
{int i=0;for(0;i < len;++ i){
void*key=read_key(env,fp);
void*val=read_val(env,fp);
({struct Cyc_Dict_Dict _tmp12C=Cyc_Dict_insert(dict,key,val);dict=_tmp12C;});}}
# 645
return dict;}
