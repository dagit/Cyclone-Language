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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 899 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 907
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 912
void*Cyc_Absyn_compress_kb(void*);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 963
void*Cyc_Absyn_string_type(void*rgn);
void*Cyc_Absyn_const_string_type(void*rgn);
# 988
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 998
void*Cyc_Absyn_array_type(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,void*zero_term,unsigned int ztloc);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1076
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1164
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83 "dict.h"
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 67 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned int,struct Cyc_Absyn_Fndecl*);
# 77
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 147
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 48
int Cyc_Tcutil_is_function_type(void*t);
# 50
int Cyc_Tcutil_is_array_type(void*t);
# 57
int Cyc_Tcutil_is_bound_one(void*b);
# 62
int Cyc_Tcutil_is_bits_only_type(void*t);
# 94
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 115
void*Cyc_Tcutil_compress(void*t);
# 118
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 146
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 151
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 181
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 195
void Cyc_Tcutil_explain_failure();
# 197
int Cyc_Tcutil_unify(void*,void*);
# 226
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 236
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 263 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 265
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 273
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 277
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 305
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 315
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 335
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 345
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 355
int Cyc_Tcutil_supports_default(void*);
# 363
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 367
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 388
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 391
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);struct _tuple10{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple10 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 33
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 37
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 40
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 59 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 62
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 64
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 72
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 81
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1U]="";
# 37 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1U};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 41
int Cyc_Tc_aggressive_warn=0;struct _tuple11{unsigned int f1;struct _tuple0*f2;int f3;};
# 43
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple11*_tmp1=(struct _tuple11*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 49
(*_tmp1).f3=1;
return 1;}}
# 53
return 0;}
# 56
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4=({struct Cyc_String_pa_PrintArg_struct _tmp1D0;_tmp1D0.tag=0U,({struct _dyneither_ptr _tmp20E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));_tmp1D0.f1=_tmp20E;});_tmp1D0;});void*_tmp2[1U];_tmp2[0]=& _tmp4;({unsigned int _tmp210=loc;struct _dyneither_ptr _tmp20F=({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp210,_tmp20F,_tag_dyneither(_tmp2,sizeof(void*),1U));});});
return 0;}
# 61
return 1;}struct _tuple12{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 63
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple12 lst1,struct _tuple12 lst2){
# 66
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp1D3;_tmp1D3.tag=0U,_tmp1D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);_tmp1D3;});struct Cyc_String_pa_PrintArg_struct _tmp8=({struct Cyc_String_pa_PrintArg_struct _tmp1D2;_tmp1D2.tag=0U,_tmp1D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);_tmp1D2;});struct Cyc_Int_pa_PrintArg_struct _tmp9=({struct Cyc_Int_pa_PrintArg_struct _tmp1D1;_tmp1D1.tag=1U,_tmp1D1.f1=(unsigned long)i;_tmp1D1;});void*_tmp5[3U];_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8,_tmp5[2]=& _tmp9;({unsigned int _tmp212=loc;struct _dyneither_ptr _tmp211=({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp212,_tmp211,_tag_dyneither(_tmp5,sizeof(void*),3U));});});}
# 70
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _tuple12 init_params=({struct _tuple12 _tmp1DA;_tmp1DA.f1=0,({struct _dyneither_ptr _tmp213=({const char*_tmp38="initializes";_tag_dyneither(_tmp38,sizeof(char),12U);});_tmp1DA.f2=_tmp213;});_tmp1DA;});
struct _tuple12 nolive_unique_params=({struct _tuple12 _tmp1D9;_tmp1D9.f1=0,({struct _dyneither_ptr _tmp214=({const char*_tmp37="noliveunique";_tag_dyneither(_tmp37,sizeof(char),13U);});_tmp1D9.f2=_tmp214;});_tmp1D9;});
struct _tuple12 noconsume_params=({struct _tuple12 _tmp1D8;_tmp1D8.f1=0,({struct _dyneither_ptr _tmp215=({const char*_tmp36="noconsume";_tag_dyneither(_tmp36,sizeof(char),10U);});_tmp1D8.f2=_tmp215;});_tmp1D8;});
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp34;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->tag == 5U){_LL1: _tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).attributes;_tmp34=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).args;_LL2: {
# 76
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp34);
for(0;_tmp35 != 0;_tmp35=_tmp35->tl){
void*_tmpC=(void*)_tmp35->hd;void*_tmpD=_tmpC;int _tmp31;int _tmp30;int _tmp2F;switch(*((int*)_tmpD)){case 20U: _LL6: _tmp2F=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL7:
# 80
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp2F,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp2F,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp2F,init_params,noconsume_params);{
struct _tuple8*_tmpE=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp2F - 1);struct _tuple8*_tmpF=_tmpE;void*_tmp24;_LLF: _tmp24=_tmpF->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp23="initializes attribute allowed only on";_tag_dyneither(_tmp23,sizeof(char),38U);});
{void*_tmp10=Cyc_Tcutil_compress(_tmp24);void*_tmp11=_tmp10;void*_tmp21;void*_tmp20;void*_tmp1F;void*_tmp1E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->tag == 3U){_LL12: _tmp21=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).elt_type;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).nullable;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).bounds;_tmp1E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).zero_term;_LL13:
# 87
 if(Cyc_Tcutil_force_type2bool(0,_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp1D4;_tmp1D4.tag=0U,_tmp1D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1D4;});void*_tmp12[1U];_tmp12[0]=& _tmp14;({unsigned int _tmp217=loc;struct _dyneither_ptr _tmp216=({const char*_tmp13="%s non-null pointers";_tag_dyneither(_tmp13,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp217,_tmp216,_tag_dyneither(_tmp12,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_bound_one(_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp1D5;_tmp1D5.tag=0U,_tmp1D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1D5;});void*_tmp15[1U];_tmp15[0]=& _tmp17;({unsigned int _tmp219=loc;struct _dyneither_ptr _tmp218=({const char*_tmp16="%s pointers of size 1";_tag_dyneither(_tmp16,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp219,_tmp218,_tag_dyneither(_tmp15,sizeof(void*),1U));});});
if(Cyc_Tcutil_force_type2bool(0,_tmp1E))
({struct Cyc_String_pa_PrintArg_struct _tmp1A=({struct Cyc_String_pa_PrintArg_struct _tmp1D6;_tmp1D6.tag=0U,_tmp1D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1D6;});void*_tmp18[1U];_tmp18[0]=& _tmp1A;({unsigned int _tmp21B=loc;struct _dyneither_ptr _tmp21A=({const char*_tmp19="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp19,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp21B,_tmp21A,_tag_dyneither(_tmp18,sizeof(void*),1U));});});
goto _LL11;}else{_LL14: _LL15:
# 95
({struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp1D7;_tmp1D7.tag=0U,_tmp1D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp1D7;});void*_tmp1B[1U];_tmp1B[0]=& _tmp1D;({unsigned int _tmp21D=loc;struct _dyneither_ptr _tmp21C=({const char*_tmp1C="%s pointers";_tag_dyneither(_tmp1C,sizeof(char),12U);});Cyc_Tcutil_terr(_tmp21D,_tmp21C,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});}_LL11:;}
# 97
({struct Cyc_List_List*_tmp21E=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=(void*)_tmp2F,_tmp22->tl=init_params.f1;_tmp22;});init_params.f1=_tmp21E;});
goto _LL5;};};case 21U: _LL8: _tmp30=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL9:
# 100
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp30,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp25=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp30 - 1);struct _tuple8*_tmp26=_tmp25;void*_tmp2A;_LL17: _tmp2A=_tmp26->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2A,0))
({void*_tmp27=0U;({unsigned int _tmp220=loc;struct _dyneither_ptr _tmp21F=({const char*_tmp28="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp28,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp220,_tmp21F,_tag_dyneither(_tmp27,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp221=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=(void*)_tmp30,_tmp29->tl=nolive_unique_params.f1;_tmp29;});nolive_unique_params.f1=_tmp221;});
goto _LL5;};case 22U: _LLA: _tmp31=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpD)->f1;_LLB:
# 109
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp2B=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp31 - 1);struct _tuple8*_tmp2C=_tmp2B;void*_tmp2E;_LL1A: _tmp2E=_tmp2C->f3;_LL1B:;
# 114
({struct Cyc_List_List*_tmp222=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=(void*)_tmp31,_tmp2D->tl=noconsume_params.f1;_tmp2D;});noconsume_params.f1=_tmp222;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 119
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp32=0U;({struct _dyneither_ptr _tmp223=({const char*_tmp33="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp33,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp223,_tag_dyneither(_tmp32,sizeof(void*),0U));});});}_LL0:;}struct _tuple13{void*f1;int f2;};
# 124
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 126
struct Cyc_Absyn_Vardecl*_tmp39=vd;enum Cyc_Absyn_Scope _tmp91;struct _tuple0*_tmp90;void*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_List_List*_tmp8D;_LL1: _tmp91=_tmp39->sc;_tmp90=_tmp39->name;_tmp8F=_tmp39->type;_tmp8E=_tmp39->initializer;_tmp8D=_tmp39->attributes;_LL2:;
# 133
{void*_tmp3A=Cyc_Tcutil_compress(_tmp8F);void*_tmp3B=_tmp3A;void*_tmp47;struct Cyc_Absyn_Tqual _tmp46;void*_tmp45;unsigned int _tmp44;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->tag == 4U){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).num_elts == 0){_LL4: _tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).elt_type;_tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).tq;_tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).zero_term;_tmp44=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).zt_loc;if(_tmp8E != 0){_LL5:
# 135
{void*_tmp3C=_tmp8E->r;void*_tmp3D=_tmp3C;struct Cyc_List_List*_tmp43;struct Cyc_List_List*_tmp42;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct _dyneither_ptr _tmp3F;struct _dyneither_ptr _tmp3E;switch(*((int*)_tmp3D)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D)->f1).Wstring_c).tag){case 8U: _LL9: _tmp3E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D)->f1).String_c).val;_LLA:
# 137
({void*_tmp229=({void*_tmp228=({void*_tmp227=_tmp47;struct Cyc_Absyn_Tqual _tmp226=_tmp46;struct Cyc_Absyn_Exp*_tmp225=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp3E,sizeof(char)),0U);void*_tmp224=_tmp45;Cyc_Absyn_array_type(_tmp227,_tmp226,_tmp225,_tmp224,_tmp44);});vd->type=_tmp228;});_tmp8F=_tmp229;});
goto _LL8;case 9U: _LLB: _tmp3F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D)->f1).Wstring_c).val;_LLC:
# 140
({void*_tmp22F=({void*_tmp22E=({void*_tmp22D=_tmp47;struct Cyc_Absyn_Tqual _tmp22C=_tmp46;struct Cyc_Absyn_Exp*_tmp22B=Cyc_Absyn_uint_exp(1U,0U);void*_tmp22A=_tmp45;Cyc_Absyn_array_type(_tmp22D,_tmp22C,_tmp22B,_tmp22A,_tmp44);});vd->type=_tmp22E;});_tmp8F=_tmp22F;});
goto _LL8;default: goto _LL15;}case 27U: _LLD: _tmp40=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D)->f2;_LLE:
 _tmp41=_tmp40;goto _LL10;case 28U: _LLF: _tmp41=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3D)->f1;_LL10:
# 145
({void*_tmp231=({void*_tmp230=Cyc_Absyn_array_type(_tmp47,_tmp46,_tmp41,_tmp45,_tmp44);vd->type=_tmp230;});_tmp8F=_tmp231;});
goto _LL8;case 36U: _LL11: _tmp42=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp3D)->f2;_LL12:
 _tmp43=_tmp42;goto _LL14;case 26U: _LL13: _tmp43=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3D)->f1;_LL14:
# 149
({void*_tmp237=({void*_tmp236=({void*_tmp235=_tmp47;struct Cyc_Absyn_Tqual _tmp234=_tmp46;struct Cyc_Absyn_Exp*_tmp233=Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp43),0U);void*_tmp232=_tmp45;Cyc_Absyn_array_type(_tmp235,_tmp234,_tmp233,_tmp232,_tmp44);});vd->type=_tmp236;});_tmp8F=_tmp237;});
goto _LL8;default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 153
goto _LL3;}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 157
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp8F);
# 159
({int _tmp238=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp8F);(vd->tq).real_const=_tmp238;});
# 162
({int _tmp239=!Cyc_Tcutil_is_array_type(_tmp8F);vd->escapes=_tmp239;});
# 164
if(Cyc_Tcutil_is_function_type(_tmp8F)){
({struct Cyc_List_List*_tmp23A=Cyc_Tcutil_transfer_fn_type_atts(_tmp8F,_tmp8D);_tmp8D=_tmp23A;});
Cyc_Tc_fnTypeAttsOK(te,_tmp8F,loc);}
# 169
if(_tmp91 == Cyc_Absyn_Extern  || _tmp91 == Cyc_Absyn_ExternC){
if(_tmp8E != 0  && !in_cinclude)
({void*_tmp48=0U;({unsigned int _tmp23C=loc;struct _dyneither_ptr _tmp23B=({const char*_tmp49="extern declaration should not have initializer";_tag_dyneither(_tmp49,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp23C,_tmp23B,_tag_dyneither(_tmp48,sizeof(void*),0U));});});}else{
if(!Cyc_Tcutil_is_function_type(_tmp8F)){
# 176
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
void*_tmp4A=(void*)_tmp8D->hd;void*_tmp4B=_tmp4A;switch(*((int*)_tmp4B)){case 6U: _LL18: _LL19:
 goto _LL1B;case 8U: _LL1A: _LL1B:
# 181
 goto _LL1D;case 9U: _LL1C: _LL1D:
 goto _LL1F;case 10U: _LL1E: _LL1F:
 goto _LL21;case 11U: _LL20: _LL21:
 goto _LL23;case 12U: _LL22: _LL23:
 goto _LL25;case 13U: _LL24: _LL25:
 goto _LL27;case 14U: _LL26: _LL27:
 continue;default: _LL28: _LL29:
# 189
({struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp1DC;_tmp1DC.tag=0U,({
struct _dyneither_ptr _tmp23D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8D->hd));_tmp1DC.f1=_tmp23D;});_tmp1DC;});struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp1DB;_tmp1DB.tag=0U,({struct _dyneither_ptr _tmp23E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1DB.f1=_tmp23E;});_tmp1DB;});void*_tmp4C[2U];_tmp4C[0]=& _tmp4E,_tmp4C[1]=& _tmp4F;({unsigned int _tmp240=loc;struct _dyneither_ptr _tmp23F=({const char*_tmp4D="bad attribute %s for variable %s";_tag_dyneither(_tmp4D,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp240,_tmp23F,_tag_dyneither(_tmp4C,sizeof(void*),2U));});});
goto _LL17;}_LL17:;}
# 194
if(_tmp8E == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp8F))
({struct Cyc_String_pa_PrintArg_struct _tmp52=({struct Cyc_String_pa_PrintArg_struct _tmp1DE;_tmp1DE.tag=0U,({
struct _dyneither_ptr _tmp241=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp1DE.f1=_tmp241;});_tmp1DE;});struct Cyc_String_pa_PrintArg_struct _tmp53=({struct Cyc_String_pa_PrintArg_struct _tmp1DD;_tmp1DD.tag=0U,({struct _dyneither_ptr _tmp242=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp8F));_tmp1DD.f1=_tmp242;});_tmp1DD;});void*_tmp50[2U];_tmp50[0]=& _tmp52,_tmp50[1]=& _tmp53;({unsigned int _tmp244=loc;struct _dyneither_ptr _tmp243=({const char*_tmp51="initializer required for variable %s of type %s";_tag_dyneither(_tmp51,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp244,_tmp243,_tag_dyneither(_tmp50,sizeof(void*),2U));});});}else{
# 199
struct Cyc_Absyn_Exp*_tmp54=_tmp8E;
# 203
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){
{void*_tmp58=Cyc_Tcexp_tcExpInitializer(te,& _tmp8F,_tmp54);
if(!Cyc_Tcutil_coerce_assign(te,_tmp54,_tmp8F)){
struct _dyneither_ptr _tmp59=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp5A=" declared with type ";
struct _dyneither_ptr _tmp5B=Cyc_Absynpp_typ2string(_tmp8F);
const char*_tmp5C=" but initialized with type ";
struct _dyneither_ptr _tmp5D=Cyc_Absynpp_typ2string(_tmp58);
if(({unsigned long _tmp248=({unsigned long _tmp247=({unsigned long _tmp246=({unsigned long _tmp245=Cyc_strlen((struct _dyneither_ptr)_tmp59);_tmp245 + Cyc_strlen(({const char*_tmp5E=_tmp5A;_tag_dyneither(_tmp5E,sizeof(char),21U);}));});_tmp246 + Cyc_strlen((struct _dyneither_ptr)_tmp5B);});_tmp247 + Cyc_strlen(({const char*_tmp5F=_tmp5C;_tag_dyneither(_tmp5F,sizeof(char),28U);}));});_tmp248 + Cyc_strlen((struct _dyneither_ptr)_tmp5D);})> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp1E3;_tmp1E3.tag=0U,_tmp1E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp59);_tmp1E3;});struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp1E2;_tmp1E2.tag=0U,({struct _dyneither_ptr _tmp249=(struct _dyneither_ptr)({const char*_tmp68=_tmp5A;_tag_dyneither(_tmp68,sizeof(char),21U);});_tmp1E2.f1=_tmp249;});_tmp1E2;});struct Cyc_String_pa_PrintArg_struct _tmp64=({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0U,_tmp1E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5B);_tmp1E1;});struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp1E0;_tmp1E0.tag=0U,({struct _dyneither_ptr _tmp24A=(struct _dyneither_ptr)({const char*_tmp67=_tmp5C;_tag_dyneither(_tmp67,sizeof(char),28U);});_tmp1E0.f1=_tmp24A;});_tmp1E0;});struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp1DF;_tmp1DF.tag=0U,_tmp1DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);_tmp1DF;});void*_tmp60[5U];_tmp60[0]=& _tmp62,_tmp60[1]=& _tmp63,_tmp60[2]=& _tmp64,_tmp60[3]=& _tmp65,_tmp60[4]=& _tmp66;({unsigned int _tmp24C=loc;struct _dyneither_ptr _tmp24B=({const char*_tmp61="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp61,sizeof(char),16U);});Cyc_Tcutil_terr(_tmp24C,_tmp24B,_tag_dyneither(_tmp60,sizeof(void*),5U));});});else{
# 214
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp1E8;_tmp1E8.tag=0U,_tmp1E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp59);_tmp1E8;});struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0U,({struct _dyneither_ptr _tmp24D=(struct _dyneither_ptr)({const char*_tmp71=_tmp5A;_tag_dyneither(_tmp71,sizeof(char),21U);});_tmp1E7.f1=_tmp24D;});_tmp1E7;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0U,_tmp1E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5B);_tmp1E6;});struct Cyc_String_pa_PrintArg_struct _tmp6E=({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0U,({struct _dyneither_ptr _tmp24E=(struct _dyneither_ptr)({const char*_tmp70=_tmp5C;_tag_dyneither(_tmp70,sizeof(char),28U);});_tmp1E5.f1=_tmp24E;});_tmp1E5;});struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp1E4;_tmp1E4.tag=0U,_tmp1E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);_tmp1E4;});void*_tmp69[5U];_tmp69[0]=& _tmp6B,_tmp69[1]=& _tmp6C,_tmp69[2]=& _tmp6D,_tmp69[3]=& _tmp6E,_tmp69[4]=& _tmp6F;({unsigned int _tmp250=loc;struct _dyneither_ptr _tmp24F=({const char*_tmp6A="%s%s%s%s%s";_tag_dyneither(_tmp6A,sizeof(char),11U);});Cyc_Tcutil_terr(_tmp250,_tmp24F,_tag_dyneither(_tmp69,sizeof(void*),5U));});});}
Cyc_Tcutil_explain_failure();
# 217
if(!Cyc_Tcutil_is_const_exp(_tmp54))
({void*_tmp72=0U;({unsigned int _tmp252=loc;struct _dyneither_ptr _tmp251=({const char*_tmp73="initializer is not a constant expression";_tag_dyneither(_tmp73,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp252,_tmp251,_tag_dyneither(_tmp72,sizeof(void*),0U));});});}}
# 204
;_pop_handler();}else{void*_tmp56=(void*)_exn_thrown;void*_tmp74=_tmp56;void*_tmp77;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp74)->tag == Cyc_Tcenv_Env_error){_LL2B: _LL2C:
# 222
({void*_tmp75=0U;({unsigned int _tmp254=loc;struct _dyneither_ptr _tmp253=({const char*_tmp76="initializer is not a constant expression";_tag_dyneither(_tmp76,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp254,_tmp253,_tag_dyneither(_tmp75,sizeof(void*),0U));});});
goto _LL2A;}else{_LL2D: _tmp77=_tmp74;_LL2E:(int)_rethrow(_tmp77);}_LL2A:;}};}}else{
# 228
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
void*_tmp78=(void*)_tmp8D->hd;void*_tmp79=_tmp78;switch(*((int*)_tmp79)){case 0U: _LL30: _LL31:
# 231
 goto _LL33;case 1U: _LL32: _LL33:
 goto _LL35;case 2U: _LL34: _LL35:
 goto _LL37;case 3U: _LL36: _LL37:
 goto _LL39;case 4U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 goto _LL3D;case 20U: _LL3C: _LL3D:
 goto _LL3F;case 23U: _LL3E: _LL3F:
 goto _LL41;case 26U: _LL40: _LL41:
 goto _LL43;case 5U: _LL42: _LL43:
({void*_tmp7A=0U;({struct _dyneither_ptr _tmp255=({const char*_tmp7B="tcVardecl: fn type atts in function var decl";_tag_dyneither(_tmp7B,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp255,_tag_dyneither(_tmp7A,sizeof(void*),0U));});});case 6U: _LL44: _LL45:
# 242
 goto _LL47;case 7U: _LL46: _LL47:
# 244
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp1E9;_tmp1E9.tag=0U,({
struct _dyneither_ptr _tmp256=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8D->hd));_tmp1E9.f1=_tmp256;});_tmp1E9;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp258=loc;struct _dyneither_ptr _tmp257=({const char*_tmp7D="bad attribute %s in function declaration";_tag_dyneither(_tmp7D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp258,_tmp257,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});
goto _LL2F;default: _LL48: _LL49:
 continue;}_LL2F:;}}}
# 252
{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){
{struct _tuple13*_tmp82=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,_tmp90);
void*_tmp83=(*_tmp82).f1;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp84=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=1U,_tmp88->f1=vd;_tmp88;});
void**_tmp85=Cyc_Tcdecl_merge_binding(_tmp83,(void*)_tmp84,loc,Cyc_Tc_tc_msg);
if(_tmp85 == 0){_npop_handler(0U);return;}{
void*_tmp86=*_tmp85;
# 260
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp86 != _tmp83  || (*_tmp82).f2)
# 263
({struct Cyc_Dict_Dict _tmp25B=({struct Cyc_Dict_Dict _tmp25A=(te->ae)->ordinaries;struct _tuple0*_tmp259=_tmp90;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp25A,_tmp259,({struct _tuple13*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->f1=_tmp86,_tmp87->f2=1;_tmp87;}));});(te->ae)->ordinaries=_tmp25B;});}
_npop_handler(0U);return;};}
# 253
;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*_tmp89=_tmp80;void*_tmp8A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp89)->tag == Cyc_Dict_Absent){_LL4B: _LL4C:
# 265
 goto _LL4A;}else{_LL4D: _tmp8A=_tmp89;_LL4E:(int)_rethrow(_tmp8A);}_LL4A:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp25F=({struct Cyc_Dict_Dict _tmp25E=(te->ae)->ordinaries;struct _tuple0*_tmp25D=_tmp90;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp25E,_tmp25D,({struct _tuple13*_tmp8C=_cycalloc(sizeof(*_tmp8C));
({void*_tmp25C=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=1U,_tmp8B->f1=vd;_tmp8B;});_tmp8C->f1=_tmp25C;}),_tmp8C->f2=0;_tmp8C;}));});
# 267
(te->ae)->ordinaries=_tmp25F;});}
# 271
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp92=n;union Cyc_Absyn_Nmspace _tmp96;struct _dyneither_ptr*_tmp95;_LL1: _tmp96=_tmp92->f1;_tmp95=_tmp92->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp93=_tmp96;if((_tmp93.Abs_n).tag == 2){if((_tmp93.Abs_n).val == 0){_LL4: _LL5:
# 275
 return({struct _dyneither_ptr _tmp260=(struct _dyneither_ptr)*_tmp95;Cyc_strcmp(_tmp260,({const char*_tmp94="main";_tag_dyneither(_tmp94,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;};}
# 280
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 282
struct _tuple0*q=fd->name;
# 286
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmp97=0U;({unsigned int _tmp262=loc;struct _dyneither_ptr _tmp261=({const char*_tmp98="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmp98,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp262,_tmp261,_tag_dyneither(_tmp97,sizeof(void*),0U));});});
# 290
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 292
void*t=Cyc_Tcutil_fndecl2type(fd);
# 294
({struct Cyc_List_List*_tmp263=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);fd->attributes=_tmp263;});
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 297
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp99=(void*)atts->hd;void*_tmp9A=_tmp99;switch(*((int*)_tmp9A)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
# 301
({struct Cyc_String_pa_PrintArg_struct _tmp9D=({struct Cyc_String_pa_PrintArg_struct _tmp1EA;_tmp1EA.tag=0U,({struct _dyneither_ptr _tmp264=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1EA.f1=_tmp264;});_tmp1EA;});void*_tmp9B[1U];_tmp9B[0]=& _tmp9D;({unsigned int _tmp266=loc;struct _dyneither_ptr _tmp265=({const char*_tmp9C="bad attribute %s for function";_tag_dyneither(_tmp9C,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp266,_tmp265,_tag_dyneither(_tmp9B,sizeof(void*),1U));});});
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}}
# 307
{struct _handler_cons _tmp9E;_push_handler(& _tmp9E);{int _tmpA0=0;if(setjmp(_tmp9E.handler))_tmpA0=1;if(!_tmpA0){
{struct _tuple13*_tmpA1=((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
void*_tmpA2=(*_tmpA1).f1;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA3=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=2U,_tmpA7->f1=fd;_tmpA7;});
void**_tmpA4=Cyc_Tcdecl_merge_binding(_tmpA2,(void*)_tmpA3,loc,Cyc_Tc_tc_msg);
if(_tmpA4 != 0){
void*_tmpA5=*_tmpA4;
# 315
if(exports == 0  || Cyc_Tc_export_member(q,*exports)){
if(!(_tmpA5 == _tmpA2  && (*_tmpA1).f2))
({struct Cyc_Dict_Dict _tmp269=({struct Cyc_Dict_Dict _tmp268=(te->ae)->ordinaries;struct _tuple0*_tmp267=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp268,_tmp267,({struct _tuple13*_tmpA6=_cycalloc(sizeof(*_tmpA6));
_tmpA6->f1=_tmpA5,_tmpA6->f2=1;_tmpA6;}));});
# 317
(te->ae)->ordinaries=_tmp269;});}}}
# 308
;_pop_handler();}else{void*_tmp9F=(void*)_exn_thrown;void*_tmpA8=_tmp9F;void*_tmpAD;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA8)->tag == Cyc_Dict_Absent){_LL8: _LL9:
# 321
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA9=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=2U,_tmpAC->f1=fd;_tmpAC;});
({struct Cyc_Dict_Dict _tmp26D=({struct Cyc_Dict_Dict _tmp26C=(te->ae)->ordinaries;struct _tuple0*_tmp26B=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple13*v))Cyc_Dict_insert)(_tmp26C,_tmp26B,({struct _tuple13*_tmpAB=_cycalloc(sizeof(*_tmpAB));
({void*_tmp26A=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=2U,_tmpAA->f1=fd;_tmpAA;});_tmpAB->f1=_tmp26A;}),_tmpAB->f2=0;_tmpAB;}));});
# 323
(te->ae)->ordinaries=_tmp26D;});}
# 326
goto _LL7;}else{_LLA: _tmpAD=_tmpA8;_LLB:(int)_rethrow(_tmpAD);}_LL7:;}};}
# 330
if(te->in_extern_c_include)return;{
# 335
struct Cyc_Tcenv_Fenv*_tmpAE=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_tmpAF=({struct Cyc_Tcenv_Tenv*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->ns=te->ns,_tmpD3->ae=te->ae,_tmpD3->le=_tmpAE,_tmpD3->allow_valueof=0,_tmpD3->in_extern_c_include=0;_tmpD3;});
# 338
Cyc_Tcstmt_tcStmt(_tmpAF,fd->body,0);
# 341
Cyc_Tcenv_check_delayed_effects(_tmpAF);
Cyc_Tcenv_check_delayed_constraints(_tmpAF);
# 344
if(Cyc_Tc_is_main(q)){
# 346
{void*_tmpB0=Cyc_Tcutil_compress(fd->ret_type);void*_tmpB1=_tmpB0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB1)->f1)){case 0U: _LLD: _LLE:
({void*_tmpB2=0U;({unsigned int _tmp26F=loc;struct _dyneither_ptr _tmp26E=({const char*_tmpB3="main declared with return type void";_tag_dyneither(_tmpB3,sizeof(char),36U);});Cyc_Tcutil_warn(_tmp26F,_tmp26E,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});goto _LLC;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB1)->f1)->f2){case Cyc_Absyn_Int_sz: _LLF: _LL10:
 goto _LLC;case Cyc_Absyn_Long_sz: _LL11: _LL12:
 goto _LLC;default: goto _LL13;}default: goto _LL13;}else{_LL13: _LL14:
# 351
({struct Cyc_String_pa_PrintArg_struct _tmpB6=({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0U,({
struct _dyneither_ptr _tmp270=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(fd->ret_type));_tmp1EB.f1=_tmp270;});_tmp1EB;});void*_tmpB4[1U];_tmpB4[0]=& _tmpB6;({unsigned int _tmp272=loc;struct _dyneither_ptr _tmp271=({const char*_tmpB5="main declared with return type %s instead of int or void";_tag_dyneither(_tmpB5,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp272,_tmp271,_tag_dyneither(_tmpB4,sizeof(void*),1U));});});
goto _LLC;}_LLC:;}
# 355
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpB7=0U;({unsigned int _tmp274=loc;struct _dyneither_ptr _tmp273=({const char*_tmpB8="main declared with varargs";_tag_dyneither(_tmpB8,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp274,_tmp273,_tag_dyneither(_tmpB7,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpB9=fd->args;
if(_tmpB9 != 0){
struct _tuple8*_tmpBA=(struct _tuple8*)_tmpB9->hd;struct _tuple8*_tmpBB=_tmpBA;void*_tmpD2;_LL16: _tmpD2=_tmpBB->f3;_LL17:;
{void*_tmpBC=Cyc_Tcutil_compress(_tmpD2);void*_tmpBD=_tmpBC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBD)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBD)->f1)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpBD)->f1)->f2){case Cyc_Absyn_Int_sz: _LL19: _LL1A:
 goto _LL18;case Cyc_Absyn_Long_sz: _LL1B: _LL1C:
 goto _LL18;default: goto _LL1D;}else{goto _LL1D;}}else{_LL1D: _LL1E:
# 364
({struct Cyc_String_pa_PrintArg_struct _tmpC0=({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0U,({
struct _dyneither_ptr _tmp275=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD2));_tmp1EC.f1=_tmp275;});_tmp1EC;});void*_tmpBE[1U];_tmpBE[0]=& _tmpC0;({unsigned int _tmp277=loc;struct _dyneither_ptr _tmp276=({const char*_tmpBF="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpBF,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp277,_tmp276,_tag_dyneither(_tmpBE,sizeof(void*),1U));});});
goto _LL18;}_LL18:;}
# 368
_tmpB9=_tmpB9->tl;
if(_tmpB9 != 0){
struct _tuple8*_tmpC1=(struct _tuple8*)_tmpB9->hd;struct _tuple8*_tmpC2=_tmpC1;void*_tmpD1;_LL20: _tmpD1=_tmpC2->f3;_LL21:;
_tmpB9=_tmpB9->tl;
if(_tmpB9 != 0)
({void*_tmpC3=0U;({unsigned int _tmp279=loc;struct _dyneither_ptr _tmp278=({const char*_tmpC4="main declared with too many arguments";_tag_dyneither(_tmpC4,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp279,_tmp278,_tag_dyneither(_tmpC3,sizeof(void*),0U));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=fd->tvs;_tmpD0;});
if(((!({void*_tmp291=_tmpD1;Cyc_Tcutil_unify(_tmp291,({void*_tmp290=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp28C=({struct Cyc_Core_Opt*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->v=& Cyc_Tcutil_rk;_tmpC5;});Cyc_Absyn_new_evar(_tmp28C,tvs);}));void*_tmp28F=({
struct Cyc_Core_Opt*_tmp28D=({struct Cyc_Core_Opt*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->v=& Cyc_Tcutil_rk;_tmpC6;});Cyc_Absyn_new_evar(_tmp28D,tvs);});
# 375
struct Cyc_Absyn_Tqual _tmp28E=
# 377
Cyc_Absyn_empty_tqual(0U);
# 375
Cyc_Absyn_fatptr_type(_tmp290,_tmp28F,_tmp28E,
# 377
Cyc_Tcutil_any_bool(& _tmpAF));}));}) && !({
void*_tmp28B=_tmpD1;Cyc_Tcutil_unify(_tmp28B,({void*_tmp28A=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp286=({struct Cyc_Core_Opt*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->v=& Cyc_Tcutil_rk;_tmpC7;});Cyc_Absyn_new_evar(_tmp286,tvs);}));void*_tmp289=({
# 380
struct Cyc_Core_Opt*_tmp287=({struct Cyc_Core_Opt*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->v=& Cyc_Tcutil_rk;_tmpC8;});Cyc_Absyn_new_evar(_tmp287,tvs);});
# 378
struct Cyc_Absyn_Tqual _tmp288=
# 381
Cyc_Absyn_empty_tqual(0U);
# 378
Cyc_Absyn_fatptr_type(_tmp28A,_tmp289,_tmp288,
# 381
Cyc_Tcutil_any_bool(& _tmpAF));}));})) && !({
void*_tmp285=_tmpD1;Cyc_Tcutil_unify(_tmp285,({void*_tmp284=Cyc_Absyn_string_type(({struct Cyc_Core_Opt*_tmp280=({struct Cyc_Core_Opt*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->v=& Cyc_Tcutil_rk;_tmpC9;});Cyc_Absyn_new_evar(_tmp280,tvs);}));void*_tmp283=({
struct Cyc_Core_Opt*_tmp281=({struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));_tmpCA->v=& Cyc_Tcutil_rk;_tmpCA;});Cyc_Absyn_new_evar(_tmp281,tvs);});
# 382
struct Cyc_Absyn_Tqual _tmp282=
# 384
Cyc_Absyn_const_tqual(0U);
# 382
Cyc_Absyn_fatptr_type(_tmp284,_tmp283,_tmp282,
# 384
Cyc_Tcutil_any_bool(& _tmpAF));}));})) && !({
void*_tmp27F=_tmpD1;Cyc_Tcutil_unify(_tmp27F,({void*_tmp27E=Cyc_Absyn_const_string_type(({struct Cyc_Core_Opt*_tmp27A=({struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->v=& Cyc_Tcutil_rk;_tmpCB;});Cyc_Absyn_new_evar(_tmp27A,tvs);}));void*_tmp27D=({
# 387
struct Cyc_Core_Opt*_tmp27B=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=& Cyc_Tcutil_rk;_tmpCC;});Cyc_Absyn_new_evar(_tmp27B,tvs);});
# 385
struct Cyc_Absyn_Tqual _tmp27C=
# 388
Cyc_Absyn_const_tqual(0U);
# 385
Cyc_Absyn_fatptr_type(_tmp27E,_tmp27D,_tmp27C,
# 388
Cyc_Tcutil_any_bool(& _tmpAF));}));}))
({struct Cyc_String_pa_PrintArg_struct _tmpCF=({struct Cyc_String_pa_PrintArg_struct _tmp1ED;_tmp1ED.tag=0U,({
struct _dyneither_ptr _tmp292=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD1));_tmp1ED.f1=_tmp292;});_tmp1ED;});void*_tmpCD[1U];_tmpCD[0]=& _tmpCF;({unsigned int _tmp294=loc;struct _dyneither_ptr _tmp293=({const char*_tmpCE="second argument of main has type %s instead of char??";_tag_dyneither(_tmpCE,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp294,_tmp293,_tag_dyneither(_tmpCD,sizeof(void*),1U));});});};}}};}};};}
# 397
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 404
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp1EE;_tmp1EE.tag=0U,_tmp1EE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp1EE;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({unsigned int _tmp296=loc;struct _dyneither_ptr _tmp295=({const char*_tmpD5="redeclaration of typedef %s";_tag_dyneither(_tmpD5,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp296,_tmp295,_tag_dyneither(_tmpD4,sizeof(void*),1U));});});
return;}
# 409
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp297=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 413
(td->tq).real_const=_tmp297;});}
# 418
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpD7=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpD8=_tmpD7;struct Cyc_Core_Opt**_tmpE0;struct Cyc_Absyn_Kind*_tmpDF;struct Cyc_Core_Opt**_tmpDE;switch(*((int*)_tmpD8)){case 1U: _LL1: _tmpDE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpD8)->f1;_LL2:
# 421
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDB=({struct Cyc_String_pa_PrintArg_struct _tmp1EF;_tmp1EF.tag=0U,_tmp1EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);_tmp1EF;});void*_tmpD9[1U];_tmpD9[0]=& _tmpDB;({unsigned int _tmp299=loc;struct _dyneither_ptr _tmp298=({const char*_tmpDA="type variable %s is not used in typedef definition";_tag_dyneither(_tmpDA,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp299,_tmp298,_tag_dyneither(_tmpD9,sizeof(void*),1U));});});
# 424
({struct Cyc_Core_Opt*_tmp29B=({struct Cyc_Core_Opt*_tmpDC=_cycalloc(sizeof(*_tmpDC));({void*_tmp29A=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpDC->v=_tmp29A;});_tmpDC;});*_tmpDE=_tmp29B;});goto _LL0;case 2U: _LL3: _tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD8)->f1;_tmpDF=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpD8)->f2;_LL4:
# 431
({struct Cyc_Core_Opt*_tmp29D=({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));({void*_tmp29C=Cyc_Tcutil_kind_to_bound(_tmpDF);_tmpDD->v=_tmp29C;});_tmpDD;});*_tmpE0=_tmp29D;});
goto _LL0;default: _LL5: _LL6:
 continue;}_LL0:;}}
# 437
({struct Cyc_Dict_Dict _tmp29E=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp29E;});}struct _tuple14{void*f1;void*f2;};
# 440
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 447
struct _RegionHandle _tmpE1=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpE1;_push_region(uprev_rgn);
# 449
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple14*_tmpE2=(struct _tuple14*)rpo->hd;struct _tuple14*_tmpE3=_tmpE2;void*_tmpE5;void*_tmpE4;_LL1: _tmpE5=_tmpE3->f1;_tmpE4=_tmpE3->f2;_LL2:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmpE5);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmpE4);}{
# 455
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 458
struct Cyc_List_List*_tmpE6=fields;for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
struct Cyc_Absyn_Aggrfield*_tmpE7=(struct Cyc_Absyn_Aggrfield*)_tmpE6->hd;struct Cyc_Absyn_Aggrfield*_tmpE8=_tmpE7;struct _dyneither_ptr*_tmp103;struct Cyc_Absyn_Tqual _tmp102;void*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;_LL4: _tmp103=_tmpE8->name;_tmp102=_tmpE8->tq;_tmp101=_tmpE8->type;_tmp100=_tmpE8->width;_tmpFF=_tmpE8->attributes;_tmpFE=_tmpE8->requires_clause;_LL5:;
# 461
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp103))
({struct Cyc_String_pa_PrintArg_struct _tmpEB=({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0U,_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp103);_tmp1F0;});void*_tmpE9[1U];_tmpE9[0]=& _tmpEB;({unsigned int _tmp2A0=loc;struct _dyneither_ptr _tmp29F=({const char*_tmpEA="duplicate member %s";_tag_dyneither(_tmpEA,sizeof(char),20U);});Cyc_Tcutil_terr(_tmp2A0,_tmp29F,_tag_dyneither(_tmpE9,sizeof(void*),1U));});});
# 465
if(({struct _dyneither_ptr _tmp2A1=(struct _dyneither_ptr)*_tmp103;Cyc_strcmp(_tmp2A1,({const char*_tmpEC="";_tag_dyneither(_tmpEC,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmp2A2=({struct Cyc_List_List*_tmpED=_region_malloc(uprev_rgn,sizeof(*_tmpED));_tmpED->hd=_tmp103,_tmpED->tl=prev_fields;_tmpED;});prev_fields=_tmp2A2;});{
# 468
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 472
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmpE6->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp101);
# 477
({int _tmp2A3=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpE6->hd)->tq).print_const,_tmp101);(((struct Cyc_Absyn_Aggrfield*)_tmpE6->hd)->tq).real_const=_tmp2A3;});
# 480
Cyc_Tcutil_check_bitfield(loc,te,_tmp101,_tmp100,_tmp103);
# 482
if((unsigned int)_tmpFE){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmpEE=0U;({unsigned int _tmp2A5=loc;struct _dyneither_ptr _tmp2A4=({const char*_tmpEF="@requires clauses are only allowed on union members";_tag_dyneither(_tmpEF,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2A5,_tmp2A4,_tag_dyneither(_tmpEE,sizeof(void*),0U));});});{
struct Cyc_Tcenv_Tenv*_tmpF0=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(_tmpF0,0,_tmpFE);
if(!Cyc_Tcutil_is_integral(_tmpFE))
({struct Cyc_String_pa_PrintArg_struct _tmpF3=({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0U,({
# 490
struct _dyneither_ptr _tmp2A6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpFE->topt)));_tmp1F1.f1=_tmp2A6;});_tmp1F1;});void*_tmpF1[1U];_tmpF1[0]=& _tmpF3;({unsigned int _tmp2A8=_tmpFE->loc;struct _dyneither_ptr _tmp2A7=({const char*_tmpF2="@requires clause has type %s instead of integral type";_tag_dyneither(_tmpF2,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp2A8,_tmp2A7,_tag_dyneither(_tmpF1,sizeof(void*),1U));});});else{
# 492
({unsigned int _tmp2AB=_tmpFE->loc;struct Cyc_Tcenv_Tenv*_tmp2AA=te;struct Cyc_List_List*_tmp2A9=tvs;Cyc_Tcutil_check_type(_tmp2AB,_tmp2AA,_tmp2A9,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->tag=9U,_tmpF4->f1=_tmpFE;_tmpF4;}));});{
# 494
struct Cyc_List_List*_tmpF5=Cyc_Relations_exp2relns(uprev_rgn,_tmpFE);
# 501
if(!Cyc_Relations_consistent_relations(_tmpF5))
({void*_tmpF6=0U;({unsigned int _tmp2AD=_tmpFE->loc;struct _dyneither_ptr _tmp2AC=({const char*_tmpF7="@requires clause may be unsatisfiable";_tag_dyneither(_tmpF7,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp2AD,_tmp2AC,_tag_dyneither(_tmpF6,sizeof(void*),0U));});});
# 507
{struct Cyc_List_List*_tmpF8=prev_relations;for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmpF5,(struct Cyc_List_List*)_tmpF8->hd)))
({void*_tmpF9=0U;({unsigned int _tmp2AF=_tmpFE->loc;struct _dyneither_ptr _tmp2AE=({const char*_tmpFA="@requires clause may overlap with previous clauses";_tag_dyneither(_tmpFA,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2AF,_tmp2AE,_tag_dyneither(_tmpF9,sizeof(void*),0U));});});}}
# 512
({struct Cyc_List_List*_tmp2B0=({struct Cyc_List_List*_tmpFB=_region_malloc(uprev_rgn,sizeof(*_tmpFB));_tmpFB->hd=_tmpF5,_tmpFB->tl=prev_relations;_tmpFB;});prev_relations=_tmp2B0;});};}};}else{
# 515
if(prev_relations != 0)
({void*_tmpFC=0U;({unsigned int _tmp2B2=loc;struct _dyneither_ptr _tmp2B1=({const char*_tmpFD="if one field has a @requires clause, they all must";_tag_dyneither(_tmpFD,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2B2,_tmp2B1,_tag_dyneither(_tmpFC,sizeof(void*),0U));});});}};}};
# 449
;_pop_region(uprev_rgn);}
# 521
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 523
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp104=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp105=_tmp104;enum Cyc_Absyn_AliasQual _tmp11A;struct Cyc_Core_Opt**_tmp119;struct Cyc_Absyn_Kind*_tmp118;struct Cyc_Core_Opt**_tmp117;struct Cyc_Core_Opt**_tmp116;enum Cyc_Absyn_AliasQual _tmp115;struct Cyc_Core_Opt**_tmp114;switch(*((int*)_tmp105)){case 1U: _LL1: _tmp114=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp105)->f1;_LL2:
# 526
({struct Cyc_Core_Opt*_tmp2B4=({struct Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));({void*_tmp2B3=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp106->v=_tmp2B3;});_tmp106;});*_tmp114=_tmp2B4;});continue;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmp116=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f1;_tmp115=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2)->aliasqual;_LL4:
# 528
 if(constrain_top_kind  && _tmp115 == Cyc_Absyn_Top)
({struct Cyc_Core_Opt*_tmp2B6=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));({void*_tmp2B5=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->kind=Cyc_Absyn_BoxKind,_tmp107->aliasqual=Cyc_Absyn_Aliasable;_tmp107;}));_tmp108->v=_tmp2B5;});_tmp108;});*_tmp116=_tmp2B6;});else{
# 531
({struct Cyc_Core_Opt*_tmp2B8=({struct Cyc_Core_Opt*_tmp10A=_cycalloc(sizeof(*_tmp10A));({void*_tmp2B7=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109->kind=Cyc_Absyn_BoxKind,_tmp109->aliasqual=_tmp115;_tmp109;}));_tmp10A->v=_tmp2B7;});_tmp10A;});*_tmp116=_tmp2B8;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmp117=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f1;if(constrain_top_kind){_LL6:
# 534
({struct Cyc_Core_Opt*_tmp2BA=({struct Cyc_Core_Opt*_tmp10C=_cycalloc(sizeof(*_tmp10C));({void*_tmp2B9=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->kind=Cyc_Absyn_BoxKind,_tmp10B->aliasqual=Cyc_Absyn_Aliasable;_tmp10B;}));_tmp10C->v=_tmp2B9;});_tmp10C;});*_tmp117=_tmp2BA;});
continue;}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmp119=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f1;_tmp118=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp105)->f2;_LL8:
# 537
({struct Cyc_Core_Opt*_tmp2BC=({struct Cyc_Core_Opt*_tmp10D=_cycalloc(sizeof(*_tmp10D));({void*_tmp2BB=Cyc_Tcutil_kind_to_bound(_tmp118);_tmp10D->v=_tmp2BB;});_tmp10D;});*_tmp119=_tmp2BC;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp105)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmp11A=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp105)->f1)->aliasqual;_LLA:
# 539
({struct Cyc_String_pa_PrintArg_struct _tmp110=({struct Cyc_String_pa_PrintArg_struct _tmp1F4;_tmp1F4.tag=0U,_tmp1F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*n).f2);_tmp1F4;});struct Cyc_String_pa_PrintArg_struct _tmp111=({struct Cyc_String_pa_PrintArg_struct _tmp1F3;_tmp1F3.tag=0U,_tmp1F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp1F3;});struct Cyc_String_pa_PrintArg_struct _tmp112=({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0U,({
struct _dyneither_ptr _tmp2BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->kind=Cyc_Absyn_MemKind,_tmp113->aliasqual=_tmp11A;_tmp113;})));_tmp1F2.f1=_tmp2BD;});_tmp1F2;});void*_tmp10E[3U];_tmp10E[0]=& _tmp110,_tmp10E[1]=& _tmp111,_tmp10E[2]=& _tmp112;({unsigned int _tmp2BF=loc;struct _dyneither_ptr _tmp2BE=({const char*_tmp10F="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp10F,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2BF,_tmp2BE,_tag_dyneither(_tmp10E,sizeof(void*),3U));});});
continue;}else{_LLB: _LLC:
 continue;}}_LL0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 547
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp11B=ad->name;
# 554
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp11C=(void*)atts->hd;void*_tmp11D=_tmp11C;switch(*((int*)_tmp11D)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
# 559
({struct Cyc_String_pa_PrintArg_struct _tmp120=({struct Cyc_String_pa_PrintArg_struct _tmp1F6;_tmp1F6.tag=0U,({
struct _dyneither_ptr _tmp2C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp1F6.f1=_tmp2C0;});_tmp1F6;});struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp1F5;_tmp1F5.tag=0U,_tmp1F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp11B).f2);_tmp1F5;});void*_tmp11E[2U];_tmp11E[0]=& _tmp120,_tmp11E[1]=& _tmp121;({unsigned int _tmp2C2=loc;struct _dyneither_ptr _tmp2C1=({const char*_tmp11F="bad attribute %s in %s definition";_tag_dyneither(_tmp11F,sizeof(char),34U);});Cyc_Tcutil_terr(_tmp2C2,_tmp2C1,_tag_dyneither(_tmp11E,sizeof(void*),2U));});});
goto _LL0;}_LL0:;}}{
# 564
struct Cyc_List_List*_tmp122=ad->tvs;
# 567
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 572
struct _tuple15 _tmp123=({struct _tuple15 _tmp1FA;_tmp1FA.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp2C3=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,_tmp11B);_tmp1FA.f2=_tmp2C3;});_tmp1FA;});struct _tuple15 _tmp124=_tmp123;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp13F;struct Cyc_List_List*_tmp13E;int _tmp13D;struct Cyc_Absyn_Aggrdecl**_tmp13C;struct Cyc_List_List*_tmp13B;struct Cyc_List_List*_tmp13A;struct Cyc_List_List*_tmp139;int _tmp138;struct Cyc_Absyn_Aggrdecl**_tmp137;if(_tmp124.f1 == 0){if(_tmp124.f2 == 0){_LL8: _LL9:
# 575
 Cyc_Tc_rule_out_memkind(loc,_tmp11B,_tmp122,0);
# 577
({struct Cyc_Dict_Dict _tmp2C6=({struct Cyc_Dict_Dict _tmp2C5=(te->ae)->aggrdecls;struct _tuple0*_tmp2C4=_tmp11B;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp2C5,_tmp2C4,({struct Cyc_Absyn_Aggrdecl**_tmp125=_cycalloc(sizeof(*_tmp125));*_tmp125=ad;_tmp125;}));});(te->ae)->aggrdecls=_tmp2C6;});
goto _LL7;}else{_LLE: _tmp137=*_tmp124.f2;_LLF: {
# 637
struct Cyc_Absyn_Aggrdecl*_tmp136=Cyc_Tcdecl_merge_aggrdecl(*_tmp137,ad,loc,Cyc_Tc_tc_msg);
if(_tmp136 == 0)
return;else{
# 641
Cyc_Tc_rule_out_memkind(loc,_tmp11B,_tmp122,0);
# 644
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp11B,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 647
*_tmp137=_tmp136;
ad=_tmp136;
goto _LL7;}}}}else{if(_tmp124.f2 == 0){_LLA: _tmp13B=(_tmp124.f1)->exist_vars;_tmp13A=(_tmp124.f1)->rgn_po;_tmp139=(_tmp124.f1)->fields;_tmp138=(_tmp124.f1)->tagged;_LLB: {
# 582
struct Cyc_Absyn_Aggrdecl**_tmp126=({struct Cyc_Absyn_Aggrdecl**_tmp12F=_cycalloc(sizeof(*_tmp12F));({struct Cyc_Absyn_Aggrdecl*_tmp2C7=({struct Cyc_Absyn_Aggrdecl*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->kind=ad->kind,_tmp12E->sc=Cyc_Absyn_Extern,_tmp12E->name=ad->name,_tmp12E->tvs=_tmp122,_tmp12E->impl=0,_tmp12E->attributes=ad->attributes,_tmp12E->expected_mem_kind=0;_tmp12E;});*_tmp12F=_tmp2C7;});_tmp12F;});
# 584
({struct Cyc_Dict_Dict _tmp2C8=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp11B,_tmp126);(te->ae)->aggrdecls=_tmp2C8;});
# 589
Cyc_Tcutil_check_unique_tvars(loc,_tmp13B);
Cyc_Tcutil_add_tvar_identities(_tmp13B);
# 593
if(_tmp138  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp127=0U;({unsigned int _tmp2CA=loc;struct _dyneither_ptr _tmp2C9=({const char*_tmp128="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp128,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2CA,_tmp2C9,_tag_dyneither(_tmp127,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2CF=te;unsigned int _tmp2CE=loc;enum Cyc_Absyn_AggrKind _tmp2CD=ad->kind;struct Cyc_List_List*_tmp2CC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp122,_tmp13B);struct Cyc_List_List*_tmp2CB=_tmp13A;Cyc_Tc_tcAggrImpl(_tmp2CF,_tmp2CE,_tmp2CD,_tmp2CC,_tmp2CB,_tmp139);});
# 597
Cyc_Tc_rule_out_memkind(loc,_tmp11B,_tmp122,0);
# 600
Cyc_Tc_rule_out_memkind(loc,_tmp11B,_tmp13B,1);
# 602
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp138){
# 605
struct Cyc_List_List*f=_tmp139;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp12B=({struct Cyc_String_pa_PrintArg_struct _tmp1F9;_tmp1F9.tag=0U,_tmp1F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);_tmp1F9;});struct Cyc_String_pa_PrintArg_struct _tmp12C=({struct Cyc_String_pa_PrintArg_struct _tmp1F8;_tmp1F8.tag=0U,_tmp1F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp11B).f2);_tmp1F8;});struct Cyc_String_pa_PrintArg_struct _tmp12D=({struct Cyc_String_pa_PrintArg_struct _tmp1F7;_tmp1F7.tag=0U,({
struct _dyneither_ptr _tmp2D0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));_tmp1F7.f1=_tmp2D0;});_tmp1F7;});void*_tmp129[3U];_tmp129[0]=& _tmp12B,_tmp129[1]=& _tmp12C,_tmp129[2]=& _tmp12D;({unsigned int _tmp2D2=loc;struct _dyneither_ptr _tmp2D1=({const char*_tmp12A="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp12A,sizeof(char),98U);});Cyc_Tcutil_warn(_tmp2D2,_tmp2D1,_tag_dyneither(_tmp129,sizeof(void*),3U));});});}}
# 611
*_tmp126=ad;
goto _LL7;}}else{_LLC: _tmp140=(_tmp124.f1)->exist_vars;_tmp13F=(_tmp124.f1)->rgn_po;_tmp13E=(_tmp124.f1)->fields;_tmp13D=(_tmp124.f1)->tagged;_tmp13C=*_tmp124.f2;_LLD:
# 615
 if(ad->kind != (*_tmp13C)->kind)
({void*_tmp130=0U;({unsigned int _tmp2D4=loc;struct _dyneither_ptr _tmp2D3=({const char*_tmp131="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp131,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp2D4,_tmp2D3,_tag_dyneither(_tmp130,sizeof(void*),0U));});});{
# 618
struct Cyc_Absyn_Aggrdecl*_tmp132=*_tmp13C;
# 620
({struct Cyc_Absyn_Aggrdecl*_tmp2D5=({struct Cyc_Absyn_Aggrdecl*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->kind=ad->kind,_tmp133->sc=Cyc_Absyn_Extern,_tmp133->name=ad->name,_tmp133->tvs=_tmp122,_tmp133->impl=0,_tmp133->attributes=ad->attributes,_tmp133->expected_mem_kind=0;_tmp133;});*_tmp13C=_tmp2D5;});
# 626
Cyc_Tcutil_check_unique_tvars(loc,_tmp140);
Cyc_Tcutil_add_tvar_identities(_tmp140);
# 629
if(_tmp13D  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp134=0U;({unsigned int _tmp2D7=loc;struct _dyneither_ptr _tmp2D6=({const char*_tmp135="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp135,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2D7,_tmp2D6,_tag_dyneither(_tmp134,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2DC=te;unsigned int _tmp2DB=loc;enum Cyc_Absyn_AggrKind _tmp2DA=ad->kind;struct Cyc_List_List*_tmp2D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp122,_tmp140);struct Cyc_List_List*_tmp2D8=_tmp13F;Cyc_Tc_tcAggrImpl(_tmp2DC,_tmp2DB,_tmp2DA,_tmp2D9,_tmp2D8,_tmp13E);});
# 633
*_tmp13C=_tmp132;
_tmp137=_tmp13C;goto _LLF;};}}_LL7:;};};}
# 654
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp141=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp142=_tmp141;enum Cyc_Absyn_AliasQual _tmp15F;enum Cyc_Absyn_KindQual _tmp15E;struct Cyc_Core_Opt**_tmp15D;enum Cyc_Absyn_KindQual _tmp15C;struct Cyc_Core_Opt**_tmp15B;struct Cyc_Core_Opt**_tmp15A;struct Cyc_Core_Opt**_tmp159;struct Cyc_Core_Opt**_tmp158;struct Cyc_Core_Opt**_tmp157;struct Cyc_Core_Opt**_tmp156;switch(*((int*)_tmp142)){case 1U: _LL1: _tmp156=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL2:
 _tmp157=_tmp156;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmp157=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL4:
# 659
 _tmp158=_tmp157;goto _LL6;case Cyc_Absyn_Aliasable: _LL5: _tmp158=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL6:
# 661
({struct Cyc_Core_Opt*_tmp2DE=({struct Cyc_Core_Opt*_tmp143=_cycalloc(sizeof(*_tmp143));({void*_tmp2DD=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp143->v=_tmp2DD;});_tmp143;});*_tmp158=_tmp2DE;});goto _LL0;default: goto _LLF;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmp159=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LL8:
 _tmp15A=_tmp159;goto _LLA;case Cyc_Absyn_Aliasable: _LL9: _tmp15A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LLA:
# 664
({struct Cyc_Core_Opt*_tmp2E0=({struct Cyc_Core_Opt*_tmp144=_cycalloc(sizeof(*_tmp144));({void*_tmp2DF=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp144->v=_tmp2DF;});_tmp144;});*_tmp15A=_tmp2E0;});goto _LL0;default: goto _LLF;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmp15B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_LLC:
# 666
({struct Cyc_Core_Opt*_tmp2E2=({struct Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));({void*_tmp2E1=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp145->v=_tmp2E1;});_tmp145;});*_tmp15B=_tmp2E2;});goto _LL0;case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmp15D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f1;_tmp15C=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp142)->f2)->kind;_LL10:
# 670
 _tmp15E=_tmp15C;goto _LL12;}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 668
({struct Cyc_String_pa_PrintArg_struct _tmp148=({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0U,_tmp1FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp1FC;});struct Cyc_String_pa_PrintArg_struct _tmp149=({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0U,_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp1FB;});void*_tmp146[2U];_tmp146[0]=& _tmp148,_tmp146[1]=& _tmp149;({unsigned int _tmp2E4=loc;struct _dyneither_ptr _tmp2E3=({const char*_tmp147="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp147,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2E4,_tmp2E3,_tag_dyneither(_tmp146,sizeof(void*),2U));});});
goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmp15E=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->kind;_LL12:
# 672
({struct Cyc_String_pa_PrintArg_struct _tmp14C=({struct Cyc_String_pa_PrintArg_struct _tmp1FF;_tmp1FF.tag=0U,_tmp1FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp1FF;});struct Cyc_String_pa_PrintArg_struct _tmp14D=({struct Cyc_String_pa_PrintArg_struct _tmp1FE;_tmp1FE.tag=0U,_tmp1FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp1FE;});struct Cyc_String_pa_PrintArg_struct _tmp14E=({struct Cyc_String_pa_PrintArg_struct _tmp1FD;_tmp1FD.tag=0U,({
# 674
struct _dyneither_ptr _tmp2E5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp14F=_cycalloc(sizeof(*_tmp14F));_tmp14F->kind=_tmp15E,_tmp14F->aliasqual=Cyc_Absyn_Unique;_tmp14F;})));_tmp1FD.f1=_tmp2E5;});_tmp1FD;});void*_tmp14A[3U];_tmp14A[0]=& _tmp14C,_tmp14A[1]=& _tmp14D,_tmp14A[2]=& _tmp14E;({unsigned int _tmp2E7=loc;struct _dyneither_ptr _tmp2E6=({const char*_tmp14B="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp14B,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2E7,_tmp2E6,_tag_dyneither(_tmp14A,sizeof(void*),3U));});});goto _LL0;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmp15F=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp142)->f1)->aliasqual;_LL14:
# 676
({struct Cyc_String_pa_PrintArg_struct _tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp202;_tmp202.tag=0U,_tmp202.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp202;});struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp201;_tmp201.tag=0U,_tmp201.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);_tmp201;});struct Cyc_String_pa_PrintArg_struct _tmp154=({struct Cyc_String_pa_PrintArg_struct _tmp200;_tmp200.tag=0U,({
# 678
struct _dyneither_ptr _tmp2E8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->kind=Cyc_Absyn_MemKind,_tmp155->aliasqual=_tmp15F;_tmp155;})));_tmp200.f1=_tmp2E8;});_tmp200;});void*_tmp150[3U];_tmp150[0]=& _tmp152,_tmp150[1]=& _tmp153,_tmp150[2]=& _tmp154;({unsigned int _tmp2EA=loc;struct _dyneither_ptr _tmp2E9=({const char*_tmp151="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp151,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2EA,_tmp2E9,_tag_dyneither(_tmp150,sizeof(void*),3U));});});goto _LL0;}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 684
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 692
{struct Cyc_List_List*_tmp160=fields;for(0;_tmp160 != 0;_tmp160=_tmp160->tl){
struct Cyc_Absyn_Datatypefield*_tmp161=(struct Cyc_Absyn_Datatypefield*)_tmp160->hd;
struct Cyc_List_List*typs=_tmp161->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp161->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 697
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp203;_tmp203.tag=0U,({
# 700
struct _dyneither_ptr _tmp2EB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp161->name));_tmp203.f1=_tmp2EB;});_tmp203;});void*_tmp162[1U];_tmp162[0]=& _tmp164;({unsigned int _tmp2ED=_tmp161->loc;struct _dyneither_ptr _tmp2EC=({const char*_tmp163="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp163,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2ED,_tmp2EC,_tag_dyneither(_tmp162,sizeof(void*),1U));});});
({int _tmp2EE=
Cyc_Tcutil_extract_const_from_typedef(_tmp161->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 701
((*((struct _tuple16*)typs->hd)).f1).real_const=_tmp2EE;});}}}{
# 707
struct Cyc_List_List*fields2;
if(is_extensible){
# 710
int _tmp165=1;
struct Cyc_List_List*_tmp166=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp165,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp165)
fields2=_tmp166;else{
# 715
fields2=0;}}else{
# 717
struct _RegionHandle _tmp167=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp167;_push_region(uprev_rgn);
# 719
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp168=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp168->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp16B=({struct Cyc_String_pa_PrintArg_struct _tmp205;_tmp205.tag=0U,_tmp205.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp168->name).f2);_tmp205;});struct Cyc_String_pa_PrintArg_struct _tmp16C=({struct Cyc_String_pa_PrintArg_struct _tmp204;_tmp204.tag=0U,_tmp204.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);_tmp204;});void*_tmp169[2U];_tmp169[0]=& _tmp16B,_tmp169[1]=& _tmp16C;({unsigned int _tmp2F0=_tmp168->loc;struct _dyneither_ptr _tmp2EF=({const char*_tmp16A="duplicate field name %s in %s";_tag_dyneither(_tmp16A,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp2F0,_tmp2EF,_tag_dyneither(_tmp169,sizeof(void*),2U));});});else{
# 725
({struct Cyc_List_List*_tmp2F1=({struct Cyc_List_List*_tmp16D=_region_malloc(uprev_rgn,sizeof(*_tmp16D));_tmp16D->hd=(*_tmp168->name).f2,_tmp16D->tl=prev_fields;_tmp16D;});prev_fields=_tmp2F1;});}
# 727
if(_tmp168->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp170=({struct Cyc_String_pa_PrintArg_struct _tmp206;_tmp206.tag=0U,_tmp206.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp168->name).f2);_tmp206;});void*_tmp16E[1U];_tmp16E[0]=& _tmp170;({unsigned int _tmp2F3=loc;struct _dyneither_ptr _tmp2F2=({const char*_tmp16F="ignoring scope of field %s";_tag_dyneither(_tmp16F,sizeof(char),27U);});Cyc_Tcutil_warn(_tmp2F3,_tmp2F2,_tag_dyneither(_tmp16E,sizeof(void*),1U));});});
_tmp168->sc=Cyc_Absyn_Public;}}}
# 732
fields2=fields;}
# 719
;_pop_region(uprev_rgn);}
# 734
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 737
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp186="@extensible datatype";_tag_dyneither(_tmp186,sizeof(char),21U);}):({const char*_tmp187="datatype";_tag_dyneither(_tmp187,sizeof(char),9U);});
# 744
struct Cyc_List_List*tvs=tud->tvs;
# 747
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 753
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 763 "tc.cyc"
{struct _handler_cons _tmp171;_push_handler(& _tmp171);{int _tmp173=0;if(setjmp(_tmp171.handler))_tmp173=1;if(!_tmp173){
({struct Cyc_Absyn_Datatypedecl***_tmp2F4=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);tud_opt=_tmp2F4;});
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->tag=Cyc_Dict_Absent;_tmp174;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 769
({union Cyc_Absyn_Nmspace _tmp2F5=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp2F5;});}
# 764
;_pop_handler();}else{void*_tmp172=(void*)_exn_thrown;void*_tmp175=_tmp172;void*_tmp178;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp175)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 773
struct Cyc_Absyn_Datatypedecl***_tmp176=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);
({struct Cyc_Absyn_Datatypedecl***_tmp2F6=(unsigned int)_tmp176?({struct Cyc_Absyn_Datatypedecl***_tmp177=_cycalloc(sizeof(*_tmp177));*_tmp177=*_tmp176;_tmp177;}): 0;tud_opt=_tmp2F6;});
goto _LL0;}}else{_LL3: _tmp178=_tmp175;_LL4:(int)_rethrow(_tmp178);}_LL0:;}};}{
# 780
struct _tuple17 _tmp179=({struct _tuple17 _tmp207;_tmp207.f1=tud->fields,_tmp207.f2=tud_opt;_tmp207;});struct _tuple17 _tmp17A=_tmp179;struct Cyc_List_List**_tmp185;struct Cyc_Absyn_Datatypedecl**_tmp184;struct Cyc_List_List**_tmp183;struct Cyc_Absyn_Datatypedecl**_tmp182;if(_tmp17A.f1 == 0){if(_tmp17A.f2 == 0){_LL6: _LL7:
# 783
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp2F9=({struct Cyc_Dict_Dict _tmp2F8=(te->ae)->datatypedecls;struct _tuple0*_tmp2F7=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp2F8,_tmp2F7,({struct Cyc_Absyn_Datatypedecl**_tmp17B=_cycalloc(sizeof(*_tmp17B));*_tmp17B=tud;_tmp17B;}));});(te->ae)->datatypedecls=_tmp2F9;});
goto _LL5;}else{_LLC: _tmp182=*_tmp17A.f2;_LLD: {
# 816
struct Cyc_Absyn_Datatypedecl*_tmp181=Cyc_Tcdecl_merge_datatypedecl(*_tmp182,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(_tmp181 == 0)
return;else{
# 821
*_tmp182=_tmp181;
goto _LL5;}}}}else{if(_tmp17A.f2 == 0){_LL8: _tmp183=(struct Cyc_List_List**)&(_tmp17A.f1)->v;_LL9: {
# 788
struct Cyc_Absyn_Datatypedecl**_tmp17C=({struct Cyc_Absyn_Datatypedecl**_tmp17E=_cycalloc(sizeof(*_tmp17E));({struct Cyc_Absyn_Datatypedecl*_tmp2FA=({struct Cyc_Absyn_Datatypedecl*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->sc=Cyc_Absyn_Extern,_tmp17D->name=tud->name,_tmp17D->tvs=tvs,_tmp17D->fields=0,_tmp17D->is_extensible=tud->is_extensible;_tmp17D;});*_tmp17E=_tmp2FA;});_tmp17E;});
# 790
({struct Cyc_Dict_Dict _tmp2FB=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,_tmp17C);(te->ae)->datatypedecls=_tmp2FB;});
# 793
({struct Cyc_List_List*_tmp2FC=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp183,tvs,tud);*_tmp183=_tmp2FC;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*_tmp17C=tud;
goto _LL5;}}else{_LLA: _tmp185=(struct Cyc_List_List**)&(_tmp17A.f1)->v;_tmp184=*_tmp17A.f2;_LLB: {
# 798
struct Cyc_Absyn_Datatypedecl*_tmp17F=*_tmp184;
# 801
if((!tud->is_extensible  && (unsigned int)_tmp17F) && _tmp17F->is_extensible)
tud->is_extensible=1;
# 804
({struct Cyc_Absyn_Datatypedecl*_tmp2FD=({struct Cyc_Absyn_Datatypedecl*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->sc=Cyc_Absyn_Extern,_tmp180->name=tud->name,_tmp180->tvs=tvs,_tmp180->fields=0,_tmp180->is_extensible=tud->is_extensible;_tmp180;});*_tmp184=_tmp2FD;});
# 808
({struct Cyc_List_List*_tmp2FE=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp185,tvs,tud);*_tmp185=_tmp2FE;});
# 811
*_tmp184=_tmp17F;
_tmp182=_tmp184;goto _LLD;}}}_LL5:;};};}
# 827
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 834
if(ed->fields != 0){struct _RegionHandle _tmp188=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp188;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp189=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 840
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp189->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp18C=({struct Cyc_String_pa_PrintArg_struct _tmp208;_tmp208.tag=0U,_tmp208.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp189->name).f2);_tmp208;});void*_tmp18A[1U];_tmp18A[0]=& _tmp18C;({unsigned int _tmp300=_tmp189->loc;struct _dyneither_ptr _tmp2FF=({const char*_tmp18B="duplicate enum constructor %s";_tag_dyneither(_tmp18B,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp300,_tmp2FF,_tag_dyneither(_tmp18A,sizeof(void*),1U));});});else{
# 843
({struct Cyc_List_List*_tmp301=({struct Cyc_List_List*_tmp18D=_region_malloc(uprev_rgn,sizeof(*_tmp18D));_tmp18D->hd=(*_tmp189->name).f2,_tmp18D->tl=prev_fields;_tmp18D;});prev_fields=_tmp301;});}
# 845
if(((struct _tuple13**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,_tmp189->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp190=({struct Cyc_String_pa_PrintArg_struct _tmp209;_tmp209.tag=0U,_tmp209.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp189->name).f2);_tmp209;});void*_tmp18E[1U];_tmp18E[0]=& _tmp190;({unsigned int _tmp303=_tmp189->loc;struct _dyneither_ptr _tmp302=({const char*_tmp18F="enum field name %s shadows global name";_tag_dyneither(_tmp18F,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp303,_tmp302,_tag_dyneither(_tmp18E,sizeof(void*),1U));});});
# 848
if(_tmp189->tag == 0){
({struct Cyc_Absyn_Exp*_tmp304=Cyc_Absyn_uint_exp(tag_count,_tmp189->loc);_tmp189->tag=_tmp304;});
++ tag_count;}else{
# 853
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp189->tag))){
struct _tuple10 _tmp191=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp189->tag));struct _tuple10 _tmp192=_tmp191;unsigned int _tmp194;int _tmp193;_LL1: _tmp194=_tmp192.f1;_tmp193=_tmp192.f2;_LL2:;
if(_tmp193)tag_count=_tmp194 + 1;}}}}
# 835
;_pop_region(uprev_rgn);}
# 861
{struct _handler_cons _tmp195;_push_handler(& _tmp195);{int _tmp197=0;if(setjmp(_tmp195.handler))_tmp197=1;if(!_tmp197){
{struct Cyc_Absyn_Enumdecl**_tmp198=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);
struct Cyc_Absyn_Enumdecl*_tmp199=Cyc_Tcdecl_merge_enumdecl(*_tmp198,ed,loc,Cyc_Tc_tc_msg);
if(_tmp199 == 0){_npop_handler(0U);return;}
*_tmp198=_tmp199;}
# 862
;_pop_handler();}else{void*_tmp196=(void*)_exn_thrown;void*_tmp19A=_tmp196;void*_tmp19D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp19A)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 867
struct Cyc_Absyn_Enumdecl**_tmp19B=({struct Cyc_Absyn_Enumdecl**_tmp19C=_cycalloc(sizeof(*_tmp19C));*_tmp19C=ed;_tmp19C;});
({struct Cyc_Dict_Dict _tmp305=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,_tmp19B);(te->ae)->enumdecls=_tmp305;});
goto _LL3;}}else{_LL6: _tmp19D=_tmp19A;_LL7:(int)_rethrow(_tmp19D);}_LL3:;}};}
# 873
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp19E=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp19E->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp19E->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1A1=({struct Cyc_String_pa_PrintArg_struct _tmp20B;_tmp20B.tag=0U,_tmp20B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);_tmp20B;});struct Cyc_String_pa_PrintArg_struct _tmp1A2=({struct Cyc_String_pa_PrintArg_struct _tmp20A;_tmp20A.tag=0U,_tmp20A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19E->name).f2);_tmp20A;});void*_tmp19F[2U];_tmp19F[0]=& _tmp1A1,_tmp19F[1]=& _tmp1A2;({unsigned int _tmp307=loc;struct _dyneither_ptr _tmp306=({const char*_tmp1A0="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1A0,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp307,_tmp306,_tag_dyneither(_tmp19F,sizeof(void*),2U));});});}}}
# 883
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1A3=sc;switch(_tmp1A3){case Cyc_Absyn_Static: _LL1: _LL2:
# 886
 if(!in_include)
({void*_tmp1A4=0U;({unsigned int _tmp309=loc;struct _dyneither_ptr _tmp308=({const char*_tmp1A5="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1A5,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp309,_tmp308,_tag_dyneither(_tmp1A4,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 890
({void*_tmp1A6=0U;({unsigned int _tmp30B=loc;struct _dyneither_ptr _tmp30A=({const char*_tmp1A7="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1A7,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp30B,_tmp30A,_tag_dyneither(_tmp1A6,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;default: _LLB: _LLC:
# 896
({void*_tmp1A8=0U;({unsigned int _tmp30D=loc;struct _dyneither_ptr _tmp30C=({const char*_tmp1A9="nested extern \"C\" declaration";_tag_dyneither(_tmp1A9,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp30D,_tmp30C,_tag_dyneither(_tmp1A8,sizeof(void*),0U));});});
return 1;}_LL0:;}
# 906
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 911
struct Cyc_List_List*last=0;
# 913
struct Cyc_List_List*_tmp1AA=ds0;for(0;_tmp1AA != 0;(last=_tmp1AA,_tmp1AA=_tmp1AA->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1AA->hd;
unsigned int loc=d->loc;
# 917
void*_tmp1AB=d->r;void*_tmp1AC=_tmp1AB;struct Cyc_List_List*_tmp1C7;struct Cyc_List_List*_tmp1C6;struct Cyc_List_List*_tmp1C5;union Cyc_Absyn_Nmspace _tmp1C4;struct _dyneither_ptr*_tmp1C3;struct Cyc_List_List*_tmp1C2;struct _dyneither_ptr*_tmp1C1;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Enumdecl*_tmp1BF;struct Cyc_Absyn_Datatypedecl*_tmp1BE;struct Cyc_Absyn_Aggrdecl*_tmp1BD;struct Cyc_Absyn_Typedefdecl*_tmp1BC;struct Cyc_Absyn_Fndecl*_tmp1BB;struct Cyc_Absyn_Vardecl*_tmp1BA;switch(*((int*)_tmp1AC)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
# 920
({void*_tmp1AD=0U;({unsigned int _tmp30F=loc;struct _dyneither_ptr _tmp30E=({const char*_tmp1AE="top level let-declarations are not implemented";_tag_dyneither(_tmp1AE,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp30F,_tmp30E,_tag_dyneither(_tmp1AD,sizeof(void*),0U));});});
goto _LL0;case 4U: _LL5: _LL6:
# 923
({void*_tmp1AF=0U;({unsigned int _tmp311=loc;struct _dyneither_ptr _tmp310=({const char*_tmp1B0="top level region declarations are not implemented";_tag_dyneither(_tmp1B0,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp311,_tmp310,_tag_dyneither(_tmp1AF,sizeof(void*),0U));});});
goto _LL0;case 0U: _LL7: _tmp1BA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL8:
# 926
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BA->sc,te->in_extern_c_include))
_tmp1BA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,_tmp1BA,check_var_init,te->in_extern_c_include,exports);
goto _LL0;case 1U: _LL9: _tmp1BB=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LLA:
# 931
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BB->sc,te->in_extern_c_include))
_tmp1BB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,_tmp1BB,exports);
goto _LL0;case 8U: _LLB: _tmp1BC=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LLC:
# 936
 _tmp1BC->extern_c=te->in_extern_c_include;
# 940
Cyc_Tc_tcTypedefdecl(te,loc,_tmp1BC);
goto _LL0;case 5U: _LLD: _tmp1BD=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LLE:
# 943
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BD->sc,te->in_extern_c_include))
_tmp1BD->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp1BD);
goto _LL0;case 6U: _LLF: _tmp1BE=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL10:
# 948
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BE->sc,te->in_extern_c_include))
_tmp1BE->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,_tmp1BE);
goto _LL0;case 7U: _LL11: _tmp1BF=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL12:
# 953
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BF->sc,te->in_extern_c_include))
_tmp1BF->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp1BF);
goto _LL0;case 13U: _LL13: _LL14:
# 958
({void*_tmp1B1=0U;({unsigned int _tmp313=d->loc;struct _dyneither_ptr _tmp312=({const char*_tmp1B2="spurious __cyclone_port_on__";_tag_dyneither(_tmp1B2,sizeof(char),29U);});Cyc_Tcutil_warn(_tmp313,_tmp312,_tag_dyneither(_tmp1B1,sizeof(void*),0U));});});
goto _LL0;case 14U: _LL15: _LL16:
# 961
 goto _LL0;case 9U: _LL17: _tmp1C1=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_tmp1C0=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AC)->f2;_LL18: {
# 964
struct Cyc_List_List*_tmp1B3=te->ns;
({struct Cyc_List_List*_tmp315=({struct Cyc_List_List*_tmp314=_tmp1B3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp314,({struct Cyc_List_List*_tmp1B4=_cycalloc(sizeof(*_tmp1B4));_tmp1B4->hd=_tmp1C1,_tmp1B4->tl=0;_tmp1B4;}));});te->ns=_tmp315;});
Cyc_Tc_tc_decls(te,_tmp1C0,in_externC,check_var_init,exports);
te->ns=_tmp1B3;
goto _LL0;}case 10U: _LL19: _tmp1C4=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1)->f1;_tmp1C3=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1)->f2;_tmp1C2=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AC)->f2;_LL1A:
# 972
 Cyc_Tc_tc_decls(te,_tmp1C2,in_externC,check_var_init,exports);
goto _LL0;case 11U: _LL1B: _tmp1C5=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_LL1C:
# 976
 Cyc_Tc_tc_decls(te,_tmp1C5,1,check_var_init,exports);
goto _LL0;default: _LL1D: _tmp1C7=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AC)->f1;_tmp1C6=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AC)->f2;_LL1E: {
# 982
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C6,(unsigned int)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1B5=Cyc_Tcenv_enter_extern_c_include(te);
Cyc_Tc_tc_decls(_tmp1B5,_tmp1C7,1,check_var_init,& newexs);
# 987
for(0;_tmp1C6 != 0;_tmp1C6=_tmp1C6->tl){
struct _tuple11*_tmp1B6=(struct _tuple11*)_tmp1C6->hd;
if(!(*_tmp1B6).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1B9=({struct Cyc_String_pa_PrintArg_struct _tmp20C;_tmp20C.tag=0U,({
struct _dyneither_ptr _tmp316=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string((*_tmp1B6).f2));_tmp20C.f1=_tmp316;});_tmp20C;});void*_tmp1B7[1U];_tmp1B7[0]=& _tmp1B9;({unsigned int _tmp318=(*_tmp1B6).f1;struct _dyneither_ptr _tmp317=({const char*_tmp1B8="%s is exported but not defined";_tag_dyneither(_tmp1B8,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp318,_tmp317,_tag_dyneither(_tmp1B7,sizeof(void*),1U));});});}
# 993
goto _LL0;}}_LL0:;}}
# 998
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1018 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1020
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1024
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1C8=d->r;void*_tmp1C9=_tmp1C8;struct Cyc_List_List**_tmp1CE;struct Cyc_List_List**_tmp1CD;struct Cyc_List_List**_tmp1CC;struct Cyc_List_List**_tmp1CB;struct Cyc_Absyn_Vardecl*_tmp1CA;switch(*((int*)_tmp1C9)){case 0U: _LL1: _tmp1CA=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1C9)->f1;_LL2:
# 1029
 if((env->in_cinclude  || _tmp1CA->sc != Cyc_Absyn_Extern  && _tmp1CA->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp1CA->name,Cyc_Absyn_uniquergn_qvar))
# 1032
return 1;
# 1034
return(*((struct _tuple13*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,_tmp1CA->name)).f2;case 11U: _LL3: _tmp1CB=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1C9)->f1;_LL4:
 _tmp1CC=_tmp1CB;goto _LL6;case 10U: _LL5: _tmp1CC=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1C9)->f2;_LL6:
 _tmp1CD=_tmp1CC;goto _LL8;case 9U: _LL7: _tmp1CD=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1C9)->f2;_LL8:
# 1038
({struct Cyc_List_List*_tmp319=Cyc_Tc_treeshake_f(env,*_tmp1CD);*_tmp1CD=_tmp319;});
return 1;case 12U: _LL9: _tmp1CE=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1C9)->f1;_LLA: {
# 1041
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp31A=Cyc_Tc_treeshake_f(env,*_tmp1CE);*_tmp1CE=_tmp31A;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 1050
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1CF=({struct Cyc_Tc_TreeshakeEnv _tmp20D;_tmp20D.in_cinclude=0,_tmp20D.ordinaries=(te->ae)->ordinaries;_tmp20D;});
return Cyc_Tc_treeshake_f(& _tmp1CF,ds);}
