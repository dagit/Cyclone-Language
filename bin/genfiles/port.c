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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 210
struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 38 "position.h"
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*);struct Cyc_Position_Error;
# 46
extern int Cyc_Position_use_gcc_style_location;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 925
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 930
extern void*Cyc_Absyn_sint_type;
# 941
void*Cyc_Absyn_enum_type(struct _tuple0*n,struct Cyc_Absyn_Enumdecl*d);
# 963
void*Cyc_Absyn_string_type(void*rgn);
# 966
extern void*Cyc_Absyn_fat_bound_type;
# 988
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*zeroterm);
# 1178
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
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
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 77
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 90
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 115
void*Cyc_Tcutil_compress(void*t);
# 388 "tcutil.h"
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 391
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*));
# 50
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val);
# 52
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 59
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data);struct Cyc_Port_Edit{unsigned int loc;struct _dyneither_ptr old_string;struct _dyneither_ptr new_string;};
# 88 "port.cyc"
int Cyc_Port_cmp_edit(struct Cyc_Port_Edit*e1,struct Cyc_Port_Edit*e2){
return(int)e1 - (int)e2;}
# 92
int Cyc_Port_cmp_seg_t(unsigned int loc1,unsigned int loc2){
return(int)(loc1 - loc2);}
# 95
int Cyc_Port_hash_seg_t(unsigned int loc){
return(int)loc;}struct Cyc_Port_VarUsage{int address_taken;struct Cyc_Absyn_Exp*init;struct Cyc_List_List*usage_locs;};struct Cyc_Port_Cvar{int id;void**eq;struct Cyc_List_List*uppers;struct Cyc_List_List*lowers;};struct Cyc_Port_Cfield{void*qual;struct _dyneither_ptr*f;void*type;};struct Cyc_Port_Const_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Notconst_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Thin_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Fat_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Void_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zero_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Arith_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Heap_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Zterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_Nozterm_ct_Port_Ctype_struct{int tag;};struct Cyc_Port_RgnVar_ct_Port_Ctype_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Port_Ptr_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;void*f4;void*f5;};struct Cyc_Port_Array_ct_Port_Ctype_struct{int tag;void*f1;void*f2;void*f3;};struct _tuple10{struct Cyc_Absyn_Aggrdecl*f1;struct Cyc_List_List*f2;};struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct{int tag;struct _tuple10*f1;};struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct{int tag;struct Cyc_List_List*f1;void**f2;};struct Cyc_Port_Fn_ct_Port_Ctype_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Port_Var_ct_Port_Ctype_struct{int tag;struct Cyc_Port_Cvar*f1;};
# 151
struct Cyc_Port_Const_ct_Port_Ctype_struct Cyc_Port_Const_ct_val={0U};
struct Cyc_Port_Notconst_ct_Port_Ctype_struct Cyc_Port_Notconst_ct_val={1U};
struct Cyc_Port_Thin_ct_Port_Ctype_struct Cyc_Port_Thin_ct_val={2U};
struct Cyc_Port_Fat_ct_Port_Ctype_struct Cyc_Port_Fat_ct_val={3U};
struct Cyc_Port_Void_ct_Port_Ctype_struct Cyc_Port_Void_ct_val={4U};
struct Cyc_Port_Zero_ct_Port_Ctype_struct Cyc_Port_Zero_ct_val={5U};
struct Cyc_Port_Arith_ct_Port_Ctype_struct Cyc_Port_Arith_ct_val={6U};
struct Cyc_Port_Heap_ct_Port_Ctype_struct Cyc_Port_Heap_ct_val={7U};
struct Cyc_Port_Zterm_ct_Port_Ctype_struct Cyc_Port_Zterm_ct_val={8U};
struct Cyc_Port_Nozterm_ct_Port_Ctype_struct Cyc_Port_Nozterm_ct_val={9U};
# 164
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts);
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*ts);
static struct _dyneither_ptr Cyc_Port_ctype2string(int deep,void*t){
void*_tmp0=t;struct Cyc_Port_Cvar*_tmp44;void*_tmp43;struct Cyc_List_List*_tmp42;struct Cyc_List_List*_tmp41;struct Cyc_List_List*_tmp40;void*_tmp3F;struct Cyc_Absyn_Aggrdecl*_tmp3E;struct Cyc_List_List*_tmp3D;void*_tmp3C;void*_tmp3B;void*_tmp3A;void*_tmp39;void*_tmp38;void*_tmp37;void*_tmp36;void*_tmp35;struct _dyneither_ptr*_tmp34;switch(*((int*)_tmp0)){case 0U: _LL1: _LL2:
 return({const char*_tmp1="const";_tag_dyneither(_tmp1,sizeof(char),6U);});case 1U: _LL3: _LL4:
 return({const char*_tmp2="notconst";_tag_dyneither(_tmp2,sizeof(char),9U);});case 2U: _LL5: _LL6:
 return({const char*_tmp3="thin";_tag_dyneither(_tmp3,sizeof(char),5U);});case 3U: _LL7: _LL8:
 return({const char*_tmp4="fat";_tag_dyneither(_tmp4,sizeof(char),4U);});case 4U: _LL9: _LLA:
 return({const char*_tmp5="void";_tag_dyneither(_tmp5,sizeof(char),5U);});case 5U: _LLB: _LLC:
 return({const char*_tmp6="zero";_tag_dyneither(_tmp6,sizeof(char),5U);});case 6U: _LLD: _LLE:
 return({const char*_tmp7="arith";_tag_dyneither(_tmp7,sizeof(char),6U);});case 7U: _LLF: _LL10:
 return({const char*_tmp8="heap";_tag_dyneither(_tmp8,sizeof(char),5U);});case 8U: _LL11: _LL12:
 return({const char*_tmp9="ZT";_tag_dyneither(_tmp9,sizeof(char),3U);});case 9U: _LL13: _LL14:
 return({const char*_tmpA="NZT";_tag_dyneither(_tmpA,sizeof(char),4U);});case 10U: _LL15: _tmp34=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp0)->f1;_LL16:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD=({struct Cyc_String_pa_PrintArg_struct _tmp384;_tmp384.tag=0U,_tmp384.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp34);_tmp384;});void*_tmpB[1U];_tmpB[0]=& _tmpD;({struct _dyneither_ptr _tmp3B4=({const char*_tmpC="%s";_tag_dyneither(_tmpC,sizeof(char),3U);});Cyc_aprintf(_tmp3B4,_tag_dyneither(_tmpB,sizeof(void*),1U));});});case 11U: _LL17: _tmp39=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp38=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f2;_tmp37=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f3;_tmp36=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f4;_tmp35=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp0)->f5;_LL18:
# 180
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp10=({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0U,({struct _dyneither_ptr _tmp3B5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp39));_tmp389.f1=_tmp3B5;});_tmp389;});struct Cyc_String_pa_PrintArg_struct _tmp11=({struct Cyc_String_pa_PrintArg_struct _tmp388;_tmp388.tag=0U,({
struct _dyneither_ptr _tmp3B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp38));_tmp388.f1=_tmp3B6;});_tmp388;});struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp387;_tmp387.tag=0U,({struct _dyneither_ptr _tmp3B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp37));_tmp387.f1=_tmp3B7;});_tmp387;});struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp386;_tmp386.tag=0U,({
struct _dyneither_ptr _tmp3B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp36));_tmp386.f1=_tmp3B8;});_tmp386;});struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0U,({struct _dyneither_ptr _tmp3B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp35));_tmp385.f1=_tmp3B9;});_tmp385;});void*_tmpE[5U];_tmpE[0]=& _tmp10,_tmpE[1]=& _tmp11,_tmpE[2]=& _tmp12,_tmpE[3]=& _tmp13,_tmpE[4]=& _tmp14;({struct _dyneither_ptr _tmp3BA=({const char*_tmpF="ptr(%s,%s,%s,%s,%s)";_tag_dyneither(_tmpF,sizeof(char),20U);});Cyc_aprintf(_tmp3BA,_tag_dyneither(_tmpE,sizeof(void*),5U));});});case 12U: _LL19: _tmp3C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp3B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f2;_tmp3A=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp0)->f3;_LL1A:
# 184
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp38C;_tmp38C.tag=0U,({struct _dyneither_ptr _tmp3BB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp3C));_tmp38C.f1=_tmp3BB;});_tmp38C;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp38B;_tmp38B.tag=0U,({
struct _dyneither_ptr _tmp3BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp3B));_tmp38B.f1=_tmp3BC;});_tmp38B;});struct Cyc_String_pa_PrintArg_struct _tmp19=({struct Cyc_String_pa_PrintArg_struct _tmp38A;_tmp38A.tag=0U,({struct _dyneither_ptr _tmp3BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp3A));_tmp38A.f1=_tmp3BD;});_tmp38A;});void*_tmp15[3U];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18,_tmp15[2]=& _tmp19;({struct _dyneither_ptr _tmp3BE=({const char*_tmp16="array(%s,%s,%s)";_tag_dyneither(_tmp16,sizeof(char),16U);});Cyc_aprintf(_tmp3BE,_tag_dyneither(_tmp15,sizeof(void*),3U));});});case 13U: _LL1B: _tmp3E=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f1;_tmp3D=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp0)->f1)->f2;_LL1C: {
# 187
struct _dyneither_ptr s=_tmp3E->kind == Cyc_Absyn_StructA?({const char*_tmp23="struct";_tag_dyneither(_tmp23,sizeof(char),7U);}):({const char*_tmp24="union";_tag_dyneither(_tmp24,sizeof(char),6U);});
if(!deep)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp38E;_tmp38E.tag=0U,_tmp38E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp38E;});struct Cyc_String_pa_PrintArg_struct _tmp1D=({struct Cyc_String_pa_PrintArg_struct _tmp38D;_tmp38D.tag=0U,({struct _dyneither_ptr _tmp3BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3E->name));_tmp38D.f1=_tmp3BF;});_tmp38D;});void*_tmp1A[2U];_tmp1A[0]=& _tmp1C,_tmp1A[1]=& _tmp1D;({struct _dyneither_ptr _tmp3C0=({const char*_tmp1B="%s %s";_tag_dyneither(_tmp1B,sizeof(char),6U);});Cyc_aprintf(_tmp3C0,_tag_dyneither(_tmp1A,sizeof(void*),2U));});});else{
# 191
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp391;_tmp391.tag=0U,_tmp391.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);_tmp391;});struct Cyc_String_pa_PrintArg_struct _tmp21=({struct Cyc_String_pa_PrintArg_struct _tmp390;_tmp390.tag=0U,({struct _dyneither_ptr _tmp3C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp3E->name));_tmp390.f1=_tmp3C1;});_tmp390;});struct Cyc_String_pa_PrintArg_struct _tmp22=({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0U,({
struct _dyneither_ptr _tmp3C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_cfields2string(0,_tmp3D));_tmp38F.f1=_tmp3C2;});_tmp38F;});void*_tmp1E[3U];_tmp1E[0]=& _tmp20,_tmp1E[1]=& _tmp21,_tmp1E[2]=& _tmp22;({struct _dyneither_ptr _tmp3C3=({const char*_tmp1F="%s %s {%s}";_tag_dyneither(_tmp1F,sizeof(char),11U);});Cyc_aprintf(_tmp3C3,_tag_dyneither(_tmp1E,sizeof(void*),3U));});});}}case 14U: if(((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2 != 0){_LL1D: _tmp40=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp3F=*((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f2;_LL1E:
 return Cyc_Port_ctype2string(deep,_tmp3F);}else{_LL1F: _tmp41=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp0)->f1;_LL20:
# 195
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp392;_tmp392.tag=0U,({struct _dyneither_ptr _tmp3C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_cfields2string(deep,_tmp41));_tmp392.f1=_tmp3C4;});_tmp392;});void*_tmp25[1U];_tmp25[0]=& _tmp27;({struct _dyneither_ptr _tmp3C5=({const char*_tmp26="aggr {%s}";_tag_dyneither(_tmp26,sizeof(char),10U);});Cyc_aprintf(_tmp3C5,_tag_dyneither(_tmp25,sizeof(void*),1U));});});}case 15U: _LL21: _tmp43=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f1;_tmp42=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp0)->f2;_LL22:
# 197
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2A=({struct Cyc_String_pa_PrintArg_struct _tmp394;_tmp394.tag=0U,({struct _dyneither_ptr _tmp3C6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(deep,_tmp42));_tmp394.f1=_tmp3C6;});_tmp394;});struct Cyc_String_pa_PrintArg_struct _tmp2B=({struct Cyc_String_pa_PrintArg_struct _tmp393;_tmp393.tag=0U,({struct _dyneither_ptr _tmp3C7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,_tmp43));_tmp393.f1=_tmp3C7;});_tmp393;});void*_tmp28[2U];_tmp28[0]=& _tmp2A,_tmp28[1]=& _tmp2B;({struct _dyneither_ptr _tmp3C8=({const char*_tmp29="fn(%s)->%s";_tag_dyneither(_tmp29,sizeof(char),11U);});Cyc_aprintf(_tmp3C8,_tag_dyneither(_tmp28,sizeof(void*),2U));});});default: _LL23: _tmp44=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp0)->f1;_LL24:
# 199
 if((unsigned int)_tmp44->eq)
return Cyc_Port_ctype2string(deep,*((void**)_check_null(_tmp44->eq)));else{
if(!deep  || _tmp44->uppers == 0  && _tmp44->lowers == 0)
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2E=({struct Cyc_Int_pa_PrintArg_struct _tmp395;_tmp395.tag=1U,_tmp395.f1=(unsigned long)_tmp44->id;_tmp395;});void*_tmp2C[1U];_tmp2C[0]=& _tmp2E;({struct _dyneither_ptr _tmp3C9=({const char*_tmp2D="var(%d)";_tag_dyneither(_tmp2D,sizeof(char),8U);});Cyc_aprintf(_tmp3C9,_tag_dyneither(_tmp2C,sizeof(void*),1U));});});else{
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0U,({
struct _dyneither_ptr _tmp3CA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(0,_tmp44->lowers));_tmp398.f1=_tmp3CA;});_tmp398;});struct Cyc_Int_pa_PrintArg_struct _tmp32=({struct Cyc_Int_pa_PrintArg_struct _tmp397;_tmp397.tag=1U,_tmp397.f1=(unsigned long)_tmp44->id;_tmp397;});struct Cyc_String_pa_PrintArg_struct _tmp33=({struct Cyc_String_pa_PrintArg_struct _tmp396;_tmp396.tag=0U,({
# 206
struct _dyneither_ptr _tmp3CB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctypes2string(0,_tmp44->uppers));_tmp396.f1=_tmp3CB;});_tmp396;});void*_tmp2F[3U];_tmp2F[0]=& _tmp31,_tmp2F[1]=& _tmp32,_tmp2F[2]=& _tmp33;({struct _dyneither_ptr _tmp3CC=({const char*_tmp30="var([%s]<=%d<=[%s])";_tag_dyneither(_tmp30,sizeof(char),20U);});Cyc_aprintf(_tmp3CC,_tag_dyneither(_tmp2F,sizeof(void*),3U));});});}}}_LL0:;}
# 209
static struct _dyneither_ptr*Cyc_Port_ctype2stringptr(int deep,void*t){return({struct _dyneither_ptr*_tmp45=_cycalloc(sizeof(*_tmp45));({struct _dyneither_ptr _tmp3CD=Cyc_Port_ctype2string(deep,t);*_tmp45=_tmp3CD;});_tmp45;});}
struct Cyc_List_List*Cyc_Port_sep(struct _dyneither_ptr s,struct Cyc_List_List*xs){
struct _dyneither_ptr*_tmp46=({struct _dyneither_ptr*_tmp4A=_cycalloc(sizeof(*_tmp4A));*_tmp4A=s;_tmp4A;});
if(xs == 0)return xs;{
struct Cyc_List_List*_tmp47=xs;
struct Cyc_List_List*_tmp48=xs->tl;
for(0;_tmp48 != 0;(_tmp47=_tmp48,_tmp48=_tmp48->tl)){
({struct Cyc_List_List*_tmp3CE=({struct Cyc_List_List*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->hd=_tmp46,_tmp49->tl=_tmp48;_tmp49;});_tmp47->tl=_tmp3CE;});}
# 218
return xs;};}
# 220
static struct _dyneither_ptr*Cyc_Port_cfield2stringptr(int deep,struct Cyc_Port_Cfield*f){
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4E=({struct Cyc_String_pa_PrintArg_struct _tmp39B;_tmp39B.tag=0U,({
struct _dyneither_ptr _tmp3CF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->qual));_tmp39B.f1=_tmp3CF;});_tmp39B;});struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp39A;_tmp39A.tag=0U,_tmp39A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f->f);_tmp39A;});struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp399;_tmp399.tag=0U,({struct _dyneither_ptr _tmp3D0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Port_ctype2string(deep,f->type));_tmp399.f1=_tmp3D0;});_tmp399;});void*_tmp4C[3U];_tmp4C[0]=& _tmp4E,_tmp4C[1]=& _tmp4F,_tmp4C[2]=& _tmp50;({struct _dyneither_ptr _tmp3D1=({const char*_tmp4D="%s %s: %s";_tag_dyneither(_tmp4D,sizeof(char),10U);});Cyc_aprintf(_tmp3D1,_tag_dyneither(_tmp4C,sizeof(void*),3U));});});
return({struct _dyneither_ptr*_tmp4B=_cycalloc(sizeof(*_tmp4B));*_tmp4B=s;_tmp4B;});}
# 225
static struct _dyneither_ptr Cyc_Port_ctypes2string(int deep,struct Cyc_List_List*ts){
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr _tmp3D2=({const char*_tmp51=",";_tag_dyneither(_tmp51,sizeof(char),2U);});Cyc_Port_sep(_tmp3D2,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_ctype2stringptr,deep,ts));}));}
# 228
static struct _dyneither_ptr Cyc_Port_cfields2string(int deep,struct Cyc_List_List*fs){
return(struct _dyneither_ptr)Cyc_strconcat_l(({struct _dyneither_ptr _tmp3D3=({const char*_tmp52=";";_tag_dyneither(_tmp52,sizeof(char),2U);});Cyc_Port_sep(_tmp3D3,((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(int,struct Cyc_Port_Cfield*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_cfield2stringptr,deep,fs));}));}
# 234
static void*Cyc_Port_notconst_ct(){return(void*)& Cyc_Port_Notconst_ct_val;}
static void*Cyc_Port_const_ct(){return(void*)& Cyc_Port_Const_ct_val;}
static void*Cyc_Port_thin_ct(){return(void*)& Cyc_Port_Thin_ct_val;}
static void*Cyc_Port_fat_ct(){return(void*)& Cyc_Port_Fat_ct_val;}
static void*Cyc_Port_void_ct(){return(void*)& Cyc_Port_Void_ct_val;}
static void*Cyc_Port_zero_ct(){return(void*)& Cyc_Port_Zero_ct_val;}
static void*Cyc_Port_arith_ct(){return(void*)& Cyc_Port_Arith_ct_val;}
static void*Cyc_Port_heap_ct(){return(void*)& Cyc_Port_Heap_ct_val;}
static void*Cyc_Port_zterm_ct(){return(void*)& Cyc_Port_Zterm_ct_val;}
static void*Cyc_Port_nozterm_ct(){return(void*)& Cyc_Port_Nozterm_ct_val;}
static void*Cyc_Port_rgnvar_ct(struct _dyneither_ptr*n){return(void*)({struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->tag=10U,_tmp53->f1=n;_tmp53;});}
static void*Cyc_Port_unknown_aggr_ct(struct Cyc_List_List*fs){
return(void*)({struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->tag=14U,_tmp54->f1=fs,_tmp54->f2=0;_tmp54;});}
# 248
static void*Cyc_Port_known_aggr_ct(struct _tuple10*p){
return(void*)({struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->tag=13U,_tmp55->f1=p;_tmp55;});}
# 251
static void*Cyc_Port_ptr_ct(void*elt,void*qual,void*ptr_kind,void*r,void*zt){
# 253
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->tag=11U,_tmp56->f1=elt,_tmp56->f2=qual,_tmp56->f3=ptr_kind,_tmp56->f4=r,_tmp56->f5=zt;_tmp56;});}
# 255
static void*Cyc_Port_array_ct(void*elt,void*qual,void*zt){
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=12U,_tmp57->f1=elt,_tmp57->f2=qual,_tmp57->f3=zt;_tmp57;});}
# 258
static void*Cyc_Port_fn_ct(void*return_type,struct Cyc_List_List*args){
return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->tag=15U,_tmp58->f1=return_type,_tmp58->f2=args;_tmp58;});}
# 261
static void*Cyc_Port_var(){
static int counter=0;
return(void*)({struct Cyc_Port_Var_ct_Port_Ctype_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=16U,({struct Cyc_Port_Cvar*_tmp3D4=({struct Cyc_Port_Cvar*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->id=counter ++,_tmp59->eq=0,_tmp59->uppers=0,_tmp59->lowers=0;_tmp59;});_tmp5A->f1=_tmp3D4;});_tmp5A;});}
# 265
static void*Cyc_Port_new_var(void*x){
return Cyc_Port_var();}
# 268
static struct _dyneither_ptr*Cyc_Port_new_region_var(){
static int counter=0;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5E=({struct Cyc_Int_pa_PrintArg_struct _tmp39C;_tmp39C.tag=1U,_tmp39C.f1=(unsigned long)counter ++;_tmp39C;});void*_tmp5C[1U];_tmp5C[0]=& _tmp5E;({struct _dyneither_ptr _tmp3D5=({const char*_tmp5D="`R%d";_tag_dyneither(_tmp5D,sizeof(char),5U);});Cyc_aprintf(_tmp3D5,_tag_dyneither(_tmp5C,sizeof(void*),1U));});});
return({struct _dyneither_ptr*_tmp5B=_cycalloc(sizeof(*_tmp5B));*_tmp5B=s;_tmp5B;});}
# 276
static int Cyc_Port_unifies(void*t1,void*t2);
# 278
static void*Cyc_Port_compress_ct(void*t){
void*_tmp5F=t;void**_tmp6B;void***_tmp6A;struct Cyc_List_List*_tmp69;struct Cyc_List_List*_tmp68;switch(*((int*)_tmp5F)){case 16U: _LL1: _tmp6A=(void***)&(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->eq;_tmp69=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->uppers;_tmp68=(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp5F)->f1)->lowers;_LL2: {
# 281
void**_tmp60=*_tmp6A;
if((unsigned int)_tmp60){
# 285
void*r=Cyc_Port_compress_ct(*_tmp60);
if(*_tmp60 != r)({void**_tmp3D6=({void**_tmp61=_cycalloc(sizeof(*_tmp61));*_tmp61=r;_tmp61;});*_tmp6A=_tmp3D6;});
return r;}
# 292
for(0;_tmp68 != 0;_tmp68=_tmp68->tl){
void*_tmp62=(void*)_tmp68->hd;void*_tmp63=_tmp62;switch(*((int*)_tmp63)){case 0U: _LL8: _LL9:
 goto _LLB;case 9U: _LLA: _LLB:
 goto _LLD;case 4U: _LLC: _LLD:
 goto _LLF;case 13U: _LLE: _LLF:
 goto _LL11;case 15U: _LL10: _LL11:
# 299
({void**_tmp3D7=({void**_tmp64=_cycalloc(sizeof(*_tmp64));*_tmp64=(void*)_tmp68->hd;_tmp64;});*_tmp6A=_tmp3D7;});
return(void*)_tmp68->hd;default: _LL12: _LL13:
# 302
 goto _LL7;}_LL7:;}
# 305
for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
void*_tmp65=(void*)_tmp69->hd;void*_tmp66=_tmp65;switch(*((int*)_tmp66)){case 1U: _LL15: _LL16:
 goto _LL18;case 8U: _LL17: _LL18:
 goto _LL1A;case 5U: _LL19: _LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;case 15U: _LL1D: _LL1E:
# 312
({void**_tmp3D8=({void**_tmp67=_cycalloc(sizeof(*_tmp67));*_tmp67=(void*)_tmp69->hd;_tmp67;});*_tmp6A=_tmp3D8;});
return(void*)_tmp69->hd;default: _LL1F: _LL20:
# 315
 goto _LL14;}_LL14:;}
# 318
return t;}case 14U: _LL3: _tmp6B=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp5F)->f2;_LL4:
# 321
 if((unsigned int)_tmp6B)return Cyc_Port_compress_ct(*_tmp6B);else{
return t;}default: _LL5: _LL6:
# 325
 return t;}_LL0:;}struct _tuple11{void*f1;void*f2;};
# 331
static void*Cyc_Port_inst(struct Cyc_Dict_Dict*instenv,void*t){
void*_tmp6C=Cyc_Port_compress_ct(t);void*_tmp6D=_tmp6C;void*_tmp80;struct Cyc_List_List*_tmp7F;void*_tmp7E;void*_tmp7D;void*_tmp7C;void*_tmp7B;void*_tmp7A;void*_tmp79;void*_tmp78;void*_tmp77;struct _dyneither_ptr*_tmp76;switch(*((int*)_tmp6D)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 8U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 14U: _LL13: _LL14:
 goto _LL16;case 13U: _LL15: _LL16:
 goto _LL18;case 16U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 return t;case 10U: _LL1B: _tmp76=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp6D)->f1;_LL1C:
# 347
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*instenv,_tmp76))
({struct Cyc_Dict_Dict _tmp3DB=({struct Cyc_Dict_Dict _tmp3DA=*instenv;struct _dyneither_ptr*_tmp3D9=_tmp76;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,void*v))Cyc_Dict_insert)(_tmp3DA,_tmp3D9,Cyc_Port_var());});*instenv=_tmp3DB;});
return((void*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*instenv,_tmp76);case 11U: _LL1D: _tmp7B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp7A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f2;_tmp79=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f3;_tmp78=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f4;_tmp77=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp6D)->f5;_LL1E: {
# 351
struct _tuple11 _tmp6E=({struct _tuple11 _tmp39D;({void*_tmp3DD=Cyc_Port_inst(instenv,_tmp7B);_tmp39D.f1=_tmp3DD;}),({void*_tmp3DC=Cyc_Port_inst(instenv,_tmp78);_tmp39D.f2=_tmp3DC;});_tmp39D;});struct _tuple11 _tmp6F=_tmp6E;void*_tmp72;void*_tmp71;_LL24: _tmp72=_tmp6F.f1;_tmp71=_tmp6F.f2;_LL25:;
if(_tmp72 == _tmp7B  && _tmp71 == _tmp78)return t;
return(void*)({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->tag=11U,_tmp70->f1=_tmp72,_tmp70->f2=_tmp7A,_tmp70->f3=_tmp79,_tmp70->f4=_tmp71,_tmp70->f5=_tmp77;_tmp70;});}case 12U: _LL1F: _tmp7E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp7D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f2;_tmp7C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp6D)->f3;_LL20: {
# 355
void*_tmp73=Cyc_Port_inst(instenv,_tmp7E);
if(_tmp73 == _tmp7E)return t;
return(void*)({struct Cyc_Port_Array_ct_Port_Ctype_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=12U,_tmp74->f1=_tmp73,_tmp74->f2=_tmp7D,_tmp74->f3=_tmp7C;_tmp74;});}default: _LL21: _tmp80=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp6D)->f1;_tmp7F=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp6D)->f2;_LL22:
# 359
 return(void*)({struct Cyc_Port_Fn_ct_Port_Ctype_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=15U,({void*_tmp3DF=Cyc_Port_inst(instenv,_tmp80);_tmp75->f1=_tmp3DF;}),({struct Cyc_List_List*_tmp3DE=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Dict_Dict*,void*),struct Cyc_Dict_Dict*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_inst,instenv,_tmp7F);_tmp75->f2=_tmp3DE;});_tmp75;});}_LL0:;}
# 363
void*Cyc_Port_instantiate(void*t){
return({struct Cyc_Dict_Dict*_tmp3E1=({struct Cyc_Dict_Dict*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Dict_Dict _tmp3E0=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp81=_tmp3E0;});_tmp81;});Cyc_Port_inst(_tmp3E1,t);});}
# 370
struct Cyc_List_List*Cyc_Port_filter_self(void*t,struct Cyc_List_List*ts){
int found=0;
{struct Cyc_List_List*_tmp82=ts;for(0;(unsigned int)_tmp82;_tmp82=_tmp82->tl){
void*_tmp83=Cyc_Port_compress_ct((void*)_tmp82->hd);
if(t == _tmp83)found=1;}}
# 376
if(!found)return ts;{
struct Cyc_List_List*res=0;
for(0;ts != 0;ts=ts->tl){
if(({void*_tmp3E2=t;_tmp3E2 == Cyc_Port_compress_ct((void*)ts->hd);}))continue;
({struct Cyc_List_List*_tmp3E3=({struct Cyc_List_List*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->hd=(void*)ts->hd,_tmp84->tl=res;_tmp84;});res=_tmp3E3;});}
# 382
return res;};}
# 387
void Cyc_Port_generalize(int is_rgn,void*t){
({void*_tmp3E4=Cyc_Port_compress_ct(t);t=_tmp3E4;});{
void*_tmp85=t;void*_tmp90;struct Cyc_List_List*_tmp8F;void*_tmp8E;void*_tmp8D;void*_tmp8C;void*_tmp8B;void*_tmp8A;void*_tmp89;void*_tmp88;void*_tmp87;struct Cyc_Port_Cvar*_tmp86;switch(*((int*)_tmp85)){case 16U: _LL1: _tmp86=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp85)->f1;_LL2:
# 392
({struct Cyc_List_List*_tmp3E5=Cyc_Port_filter_self(t,_tmp86->uppers);_tmp86->uppers=_tmp3E5;});
({struct Cyc_List_List*_tmp3E6=Cyc_Port_filter_self(t,_tmp86->lowers);_tmp86->lowers=_tmp3E6;});
if(is_rgn){
# 397
if(_tmp86->uppers == 0  && _tmp86->lowers == 0){
({void*_tmp3E7=t;Cyc_Port_unifies(_tmp3E7,Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var()));});
return;}
# 402
if((unsigned int)_tmp86->uppers){
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp86->uppers))->hd);
Cyc_Port_generalize(1,t);}else{
# 406
Cyc_Port_unifies(t,(void*)((struct Cyc_List_List*)_check_null(_tmp86->lowers))->hd);
Cyc_Port_generalize(1,t);}}
# 410
return;case 0U: _LL3: _LL4:
 goto _LL6;case 1U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 3U: _LL9: _LLA:
 goto _LLC;case 4U: _LLB: _LLC:
 goto _LLE;case 5U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 14U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
 goto _LL18;case 9U: _LL17: _LL18:
 goto _LL1A;case 8U: _LL19: _LL1A:
 goto _LL1C;case 7U: _LL1B: _LL1C:
 return;case 11U: _LL1D: _tmp8B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp8A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f2;_tmp89=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f3;_tmp88=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f4;_tmp87=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp85)->f5;_LL1E:
# 427
 Cyc_Port_generalize(0,_tmp8B);Cyc_Port_generalize(1,_tmp88);goto _LL0;case 12U: _LL1F: _tmp8E=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp8D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f2;_tmp8C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp85)->f3;_LL20:
# 429
 Cyc_Port_generalize(0,_tmp8E);Cyc_Port_generalize(0,_tmp8D);goto _LL0;default: _LL21: _tmp90=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp85)->f1;_tmp8F=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp85)->f2;_LL22:
# 431
 Cyc_Port_generalize(0,_tmp90);((void(*)(void(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Port_generalize,0,_tmp8F);goto _LL0;}_LL0:;};}
# 437
static int Cyc_Port_occurs(void*v,void*t){
({void*_tmp3E8=Cyc_Port_compress_ct(t);t=_tmp3E8;});
if(t == v)return 1;{
void*_tmp91=t;struct Cyc_List_List*_tmp9D;struct Cyc_List_List*_tmp9C;void*_tmp9B;struct Cyc_List_List*_tmp9A;void*_tmp99;void*_tmp98;void*_tmp97;void*_tmp96;void*_tmp95;void*_tmp94;void*_tmp93;void*_tmp92;switch(*((int*)_tmp91)){case 11U: _LL1: _tmp96=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp95=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f2;_tmp94=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f3;_tmp93=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f4;_tmp92=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp91)->f5;_LL2:
# 442
 return(((Cyc_Port_occurs(v,_tmp96) || Cyc_Port_occurs(v,_tmp95)) || Cyc_Port_occurs(v,_tmp94)) || Cyc_Port_occurs(v,_tmp93)) || 
Cyc_Port_occurs(v,_tmp92);case 12U: _LL3: _tmp99=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp98=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f2;_tmp97=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp91)->f3;_LL4:
# 445
 return(Cyc_Port_occurs(v,_tmp99) || Cyc_Port_occurs(v,_tmp98)) || Cyc_Port_occurs(v,_tmp97);case 15U: _LL5: _tmp9B=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp91)->f1;_tmp9A=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp91)->f2;_LL6:
# 447
 if(Cyc_Port_occurs(v,_tmp9B))return 1;
for(0;(unsigned int)_tmp9A;_tmp9A=_tmp9A->tl){
if(Cyc_Port_occurs(v,(void*)_tmp9A->hd))return 1;}
return 0;case 13U: _LL7: _tmp9C=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp91)->f1)->f2;_LL8:
 return 0;case 14U: _LL9: _tmp9D=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp91)->f1;_LLA:
# 453
 for(0;(unsigned int)_tmp9D;_tmp9D=_tmp9D->tl){
if(Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmp9D->hd)->qual) || Cyc_Port_occurs(v,((struct Cyc_Port_Cfield*)_tmp9D->hd)->type))return 1;}
return 0;default: _LLB: _LLC:
 return 0;}_LL0:;};}char Cyc_Port_Unify_ct[9U]="Unify_ct";struct Cyc_Port_Unify_ct_exn_struct{char*tag;};
# 465
struct Cyc_Port_Unify_ct_exn_struct Cyc_Port_Unify_ct_val={Cyc_Port_Unify_ct};
# 467
static int Cyc_Port_leq(void*t1,void*t2);
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2);
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_subset);
# 471
static void Cyc_Port_unify_ct(void*t1,void*t2){
({void*_tmp3E9=Cyc_Port_compress_ct(t1);t1=_tmp3E9;});
({void*_tmp3EA=Cyc_Port_compress_ct(t2);t2=_tmp3EA;});
if(t1 == t2)return;{
struct _tuple11 _tmp9E=({struct _tuple11 _tmp39E;_tmp39E.f1=t1,_tmp39E.f2=t2;_tmp39E;});struct _tuple11 _tmp9F=_tmp9E;struct Cyc_List_List*_tmpCC;void***_tmpCB;struct Cyc_Absyn_Aggrdecl*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpC8;void***_tmpC7;struct Cyc_List_List*_tmpC6;void***_tmpC5;struct Cyc_Absyn_Aggrdecl*_tmpC4;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC2;void***_tmpC1;struct _tuple10*_tmpC0;struct _tuple10*_tmpBF;void*_tmpBE;struct Cyc_List_List*_tmpBD;void*_tmpBC;struct Cyc_List_List*_tmpBB;void*_tmpBA;void*_tmpB9;void*_tmpB8;void*_tmpB7;void*_tmpB6;void*_tmpB5;struct _dyneither_ptr _tmpB4;struct _dyneither_ptr _tmpB3;void*_tmpB2;void*_tmpB1;void*_tmpB0;void*_tmpAF;void*_tmpAE;void*_tmpAD;void*_tmpAC;void*_tmpAB;void*_tmpAA;void*_tmpA9;struct Cyc_Port_Cvar*_tmpA8;struct Cyc_Port_Cvar*_tmpA7;if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f1)->tag == 16U){_LL1: _tmpA7=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_LL2:
# 477
 if(!Cyc_Port_occurs(t1,t2)){
# 480
{struct Cyc_List_List*_tmpA0=Cyc_Port_filter_self(t1,_tmpA7->uppers);for(0;(unsigned int)_tmpA0;_tmpA0=_tmpA0->tl){
if(!Cyc_Port_leq(t2,(void*)_tmpA0->hd))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
{struct Cyc_List_List*_tmpA1=Cyc_Port_filter_self(t1,_tmpA7->lowers);for(0;(unsigned int)_tmpA1;_tmpA1=_tmpA1->tl){
if(!Cyc_Port_leq((void*)_tmpA1->hd,t2))(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}
({void**_tmp3EB=({void**_tmpA2=_cycalloc(sizeof(*_tmpA2));*_tmpA2=t2;_tmpA2;});_tmpA7->eq=_tmp3EB;});
return;}else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}else{if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 16U){_LL3: _tmpA8=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LL4:
 Cyc_Port_unify_ct(t2,t1);return;}else{switch(*((int*)_tmp9F.f1)){case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 11U){_LL5: _tmpB2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpB0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f3;_tmpAF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f4;_tmpAE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f1)->f5;_tmpAD=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpAC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_tmpAB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f3;_tmpAA=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f4;_tmpA9=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp9F.f2)->f5;_LL6:
# 489
 Cyc_Port_unify_ct(_tmpB2,_tmpAD);Cyc_Port_unify_ct(_tmpB1,_tmpAC);Cyc_Port_unify_ct(_tmpB0,_tmpAB);Cyc_Port_unify_ct(_tmpAF,_tmpAA);
Cyc_Port_unify_ct(_tmpAE,_tmpA9);
return;}else{goto _LL15;}case 10U: if(((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 10U){_LL7: _tmpB4=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB3=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LL8:
# 493
 if(Cyc_strcmp((struct _dyneither_ptr)_tmpB4,(struct _dyneither_ptr)_tmpB3)!= 0)(int)_throw((void*)& Cyc_Port_Unify_ct_val);
return;}else{goto _LL15;}case 12U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 12U){_LL9: _tmpBA=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpB9=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpB8=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f1)->f3;_tmpB7=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpB6=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_tmpB5=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp9F.f2)->f3;_LLA:
# 496
 Cyc_Port_unify_ct(_tmpBA,_tmpB7);Cyc_Port_unify_ct(_tmpB9,_tmpB6);Cyc_Port_unify_ct(_tmpB8,_tmpB5);return;}else{goto _LL15;}case 15U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->tag == 15U){_LLB: _tmpBE=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpBD=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpBC=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpBB=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LLC:
# 498
 Cyc_Port_unify_ct(_tmpBE,_tmpBC);Cyc_Port_unify_cts(_tmpBD,_tmpBB);return;}else{goto _LL15;}case 13U: switch(*((int*)_tmp9F.f2)){case 13U: _LLD: _tmpC0=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpBF=((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_LLE:
# 500
 if(_tmpC0 == _tmpBF)return;else{(int)_throw((void*)& Cyc_Port_Unify_ct_val);}case 14U: _LL13: _tmpC4=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1)->f1;_tmpC3=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1)->f2;_tmpC2=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpC1=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LL14:
# 510
 Cyc_Port_merge_fields(_tmpC3,_tmpC2,0);
({void**_tmp3EC=({void**_tmpA6=_cycalloc(sizeof(*_tmpA6));*_tmpA6=t1;_tmpA6;});*_tmpC1=_tmp3EC;});
return;default: goto _LL15;}case 14U: switch(*((int*)_tmp9F.f2)){case 14U: _LLF: _tmpC8=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpC7=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpC6=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1;_tmpC5=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f2;_LL10: {
# 502
void*_tmpA3=Cyc_Port_unknown_aggr_ct(Cyc_Port_merge_fields(_tmpC8,_tmpC6,1));
({void**_tmp3EE=({void**_tmp3ED=({void**_tmpA4=_cycalloc(sizeof(*_tmpA4));*_tmpA4=_tmpA3;_tmpA4;});*_tmpC5=_tmp3ED;});*_tmpC7=_tmp3EE;});
return;}case 13U: _LL11: _tmpCC=((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f1;_tmpCB=(void***)&((struct Cyc_Port_UnknownAggr_ct_Port_Ctype_struct*)_tmp9F.f1)->f2;_tmpCA=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1)->f1;_tmpC9=(((struct Cyc_Port_KnownAggr_ct_Port_Ctype_struct*)_tmp9F.f2)->f1)->f2;_LL12:
# 506
 Cyc_Port_merge_fields(_tmpC9,_tmpCC,0);
({void**_tmp3EF=({void**_tmpA5=_cycalloc(sizeof(*_tmpA5));*_tmpA5=t2;_tmpA5;});*_tmpCB=_tmp3EF;});
return;default: goto _LL15;}default: _LL15: _LL16:
# 513
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}_LL0:;};}
# 517
static void Cyc_Port_unify_cts(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Port_unify_ct((void*)t1->hd,(void*)t2->hd);}
# 521
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}
# 527
static struct Cyc_List_List*Cyc_Port_merge_fields(struct Cyc_List_List*fs1,struct Cyc_List_List*fs2,int allow_f1_subset_f2){
# 529
struct Cyc_List_List*common=0;
{struct Cyc_List_List*_tmpCD=fs2;for(0;(unsigned int)_tmpCD;_tmpCD=_tmpCD->tl){
struct Cyc_Port_Cfield*_tmpCE=(struct Cyc_Port_Cfield*)_tmpCD->hd;
int found=0;
{struct Cyc_List_List*_tmpCF=fs1;for(0;(unsigned int)_tmpCF;_tmpCF=_tmpCF->tl){
struct Cyc_Port_Cfield*_tmpD0=(struct Cyc_Port_Cfield*)_tmpCF->hd;
if(Cyc_strptrcmp(_tmpD0->f,_tmpCE->f)== 0){
({struct Cyc_List_List*_tmp3F0=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->hd=_tmpD0,_tmpD1->tl=common;_tmpD1;});common=_tmp3F0;});
Cyc_Port_unify_ct(_tmpD0->qual,_tmpCE->qual);
Cyc_Port_unify_ct(_tmpD0->type,_tmpCE->type);
found=1;
break;}}}
# 543
if(!found){
if(allow_f1_subset_f2)
({struct Cyc_List_List*_tmp3F1=({struct Cyc_List_List*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->hd=_tmpCE,_tmpD2->tl=common;_tmpD2;});common=_tmp3F1;});else{
(int)_throw((void*)& Cyc_Port_Unify_ct_val);}}}}
# 549
{struct Cyc_List_List*_tmpD3=fs1;for(0;(unsigned int)_tmpD3;_tmpD3=_tmpD3->tl){
struct Cyc_Port_Cfield*_tmpD4=(struct Cyc_Port_Cfield*)_tmpD3->hd;
int found=0;
{struct Cyc_List_List*_tmpD5=fs2;for(0;(unsigned int)_tmpD5;_tmpD5=_tmpD5->tl){
struct Cyc_Port_Cfield*_tmpD6=(struct Cyc_Port_Cfield*)_tmpD5->hd;
if(Cyc_strptrcmp(_tmpD4->f,_tmpD6->f))found=1;}}
# 556
if(!found)
({struct Cyc_List_List*_tmp3F2=({struct Cyc_List_List*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->hd=_tmpD4,_tmpD7->tl=common;_tmpD7;});common=_tmp3F2;});}}
# 559
return common;}
# 562
static int Cyc_Port_unifies(void*t1,void*t2){
{struct _handler_cons _tmpD8;_push_handler(& _tmpD8);{int _tmpDA=0;if(setjmp(_tmpD8.handler))_tmpDA=1;if(!_tmpDA){
# 569
Cyc_Port_unify_ct(t1,t2);;_pop_handler();}else{void*_tmpD9=(void*)_exn_thrown;void*_tmpDB=_tmpD9;void*_tmpDC;if(((struct Cyc_Port_Unify_ct_exn_struct*)_tmpDB)->tag == Cyc_Port_Unify_ct){_LL1: _LL2:
# 576
 return 0;}else{_LL3: _tmpDC=_tmpDB;_LL4:(int)_rethrow(_tmpDC);}_LL0:;}};}
# 578
return 1;}struct _tuple12{void*f1;void*f2;void*f3;void*f4;void*f5;};
# 584
static struct Cyc_List_List*Cyc_Port_insert_upper(void*v,void*t,struct Cyc_List_List**uppers){
# 586
({void*_tmp3F3=Cyc_Port_compress_ct(t);t=_tmp3F3;});
{void*_tmpDD=t;switch(*((int*)_tmpDD)){case 1U: _LL1: _LL2:
# 590
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 5U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 3U: _LL9: _LLA:
 goto _LLC;case 12U: _LLB: _LLC:
 goto _LLE;case 13U: _LLD: _LLE:
 goto _LL10;case 15U: _LLF: _LL10:
 goto _LL12;case 7U: _LL11: _LL12:
# 601
*uppers=0;
Cyc_Port_unifies(v,t);
return*uppers;case 4U: _LL13: _LL14:
# 606
 goto _LL16;case 0U: _LL15: _LL16:
 goto _LL18;case 9U: _LL17: _LL18:
# 609
 return*uppers;default: _LL19: _LL1A:
 goto _LL0;}_LL0:;}
# 613
{struct Cyc_List_List*_tmpDE=*uppers;for(0;(unsigned int)_tmpDE;_tmpDE=_tmpDE->tl){
void*_tmpDF=Cyc_Port_compress_ct((void*)_tmpDE->hd);
# 616
if(t == _tmpDF)return*uppers;{
struct _tuple11 _tmpE0=({struct _tuple11 _tmp3A0;_tmp3A0.f1=t,_tmp3A0.f2=_tmpDF;_tmp3A0;});struct _tuple11 _tmpE1=_tmpE0;void*_tmpF4;void*_tmpF3;void*_tmpF2;void*_tmpF1;void*_tmpF0;void*_tmpEF;void*_tmpEE;void*_tmpED;void*_tmpEC;void*_tmpEB;switch(*((int*)_tmpE1.f1)){case 6U: switch(*((int*)_tmpE1.f2)){case 11U: _LL1C: _LL1D:
# 621
 goto _LL1F;case 5U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
# 624
 return*uppers;default: goto _LL24;}case 11U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->tag == 11U){_LL22: _tmpF4=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f1;_tmpF3=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f2;_tmpF2=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f3;_tmpF1=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f4;_tmpF0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f1)->f5;_tmpEF=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f1;_tmpEE=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f2;_tmpED=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f3;_tmpEC=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f4;_tmpEB=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpE1.f2)->f5;_LL23: {
# 629
struct _tuple12 _tmpE2=({struct _tuple12 _tmp39F;({void*_tmp3F8=Cyc_Port_var();_tmp39F.f1=_tmp3F8;}),({void*_tmp3F7=Cyc_Port_var();_tmp39F.f2=_tmp3F7;}),({void*_tmp3F6=Cyc_Port_var();_tmp39F.f3=_tmp3F6;}),({void*_tmp3F5=Cyc_Port_var();_tmp39F.f4=_tmp3F5;}),({void*_tmp3F4=Cyc_Port_var();_tmp39F.f5=_tmp3F4;});_tmp39F;});struct _tuple12 _tmpE3=_tmpE2;void*_tmpEA;void*_tmpE9;void*_tmpE8;void*_tmpE7;void*_tmpE6;_LL27: _tmpEA=_tmpE3.f1;_tmpE9=_tmpE3.f2;_tmpE8=_tmpE3.f3;_tmpE7=_tmpE3.f4;_tmpE6=_tmpE3.f5;_LL28:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpE4=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->tag=11U,_tmpE5->f1=_tmpEA,_tmpE5->f2=_tmpE9,_tmpE5->f3=_tmpE8,_tmpE5->f4=_tmpE7,_tmpE5->f5=_tmpE6;_tmpE5;});
Cyc_Port_leq(_tmpEA,_tmpF4);Cyc_Port_leq(_tmpEA,_tmpEF);
Cyc_Port_leq(_tmpE9,_tmpF3);Cyc_Port_leq(_tmpE9,_tmpEE);
Cyc_Port_leq(_tmpE8,_tmpF2);Cyc_Port_leq(_tmpE8,_tmpEE);
Cyc_Port_leq(_tmpE7,_tmpF1);Cyc_Port_leq(_tmpE7,_tmpEC);
Cyc_Port_leq(_tmpE6,_tmpF0);Cyc_Port_leq(_tmpE6,_tmpEB);
_tmpDE->hd=(void*)((void*)_tmpE4);
return*uppers;};}}else{goto _LL24;}default: _LL24: _LL25:
 goto _LL1B;}_LL1B:;};}}
# 641
return({struct Cyc_List_List*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->hd=t,_tmpF5->tl=*uppers;_tmpF5;});}
# 646
static struct Cyc_List_List*Cyc_Port_insert_lower(void*v,void*t,struct Cyc_List_List**lowers){
# 648
({void*_tmp3F9=Cyc_Port_compress_ct(t);t=_tmp3F9;});
{void*_tmpF6=t;switch(*((int*)_tmpF6)){case 0U: _LL1: _LL2:
 goto _LL4;case 8U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 13U: _LLB: _LLC:
 goto _LLE;case 15U: _LLD: _LLE:
 goto _LL10;case 10U: _LLF: _LL10:
# 658
*lowers=0;
Cyc_Port_unifies(v,t);
return*lowers;case 7U: _LL11: _LL12:
 goto _LL14;case 1U: _LL13: _LL14:
 goto _LL16;case 9U: _LL15: _LL16:
# 664
 return*lowers;default: _LL17: _LL18:
# 666
 goto _LL0;}_LL0:;}
# 668
{struct Cyc_List_List*_tmpF7=*lowers;for(0;(unsigned int)_tmpF7;_tmpF7=_tmpF7->tl){
void*_tmpF8=Cyc_Port_compress_ct((void*)_tmpF7->hd);
if(t == _tmpF8)return*lowers;{
struct _tuple11 _tmpF9=({struct _tuple11 _tmp3A2;_tmp3A2.f1=t,_tmp3A2.f2=_tmpF8;_tmp3A2;});struct _tuple11 _tmpFA=_tmpF9;void*_tmp10D;void*_tmp10C;void*_tmp10B;void*_tmp10A;void*_tmp109;void*_tmp108;void*_tmp107;void*_tmp106;void*_tmp105;void*_tmp104;if(((struct Cyc_Port_Void_ct_Port_Ctype_struct*)_tmpFA.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{switch(*((int*)_tmpFA.f1)){case 5U: switch(*((int*)_tmpFA.f2)){case 6U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;default: goto _LL26;}case 11U: switch(*((int*)_tmpFA.f2)){case 6U: _LL20: _LL21:
 goto _LL23;case 11U: _LL24: _tmp10D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f1;_tmp10C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f2;_tmp10B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f3;_tmp10A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f4;_tmp109=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f1)->f5;_tmp108=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f1;_tmp107=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f2;_tmp106=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f3;_tmp105=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f4;_tmp104=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmpFA.f2)->f5;_LL25: {
# 682
struct _tuple12 _tmpFB=({struct _tuple12 _tmp3A1;({void*_tmp3FE=Cyc_Port_var();_tmp3A1.f1=_tmp3FE;}),({void*_tmp3FD=Cyc_Port_var();_tmp3A1.f2=_tmp3FD;}),({void*_tmp3FC=Cyc_Port_var();_tmp3A1.f3=_tmp3FC;}),({void*_tmp3FB=Cyc_Port_var();_tmp3A1.f4=_tmp3FB;}),({void*_tmp3FA=Cyc_Port_var();_tmp3A1.f5=_tmp3FA;});_tmp3A1;});struct _tuple12 _tmpFC=_tmpFB;void*_tmp103;void*_tmp102;void*_tmp101;void*_tmp100;void*_tmpFF;_LL29: _tmp103=_tmpFC.f1;_tmp102=_tmpFC.f2;_tmp101=_tmpFC.f3;_tmp100=_tmpFC.f4;_tmpFF=_tmpFC.f5;_LL2A:;{
struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFD=({struct Cyc_Port_Ptr_ct_Port_Ctype_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->tag=11U,_tmpFE->f1=_tmp103,_tmpFE->f2=_tmp102,_tmpFE->f3=_tmp101,_tmpFE->f4=_tmp100,_tmpFE->f5=_tmpFF;_tmpFE;});
Cyc_Port_leq(_tmp10D,_tmp103);Cyc_Port_leq(_tmp108,_tmp103);
Cyc_Port_leq(_tmp10C,_tmp102);Cyc_Port_leq(_tmp107,_tmp102);
Cyc_Port_leq(_tmp10B,_tmp101);Cyc_Port_leq(_tmp107,_tmp101);
Cyc_Port_leq(_tmp10A,_tmp100);Cyc_Port_leq(_tmp105,_tmp100);
Cyc_Port_leq(_tmp109,_tmpFF);Cyc_Port_leq(_tmp104,_tmpFF);
_tmpF7->hd=(void*)((void*)_tmpFD);
return*lowers;};}default: goto _LL26;}case 12U: if(((struct Cyc_Port_Arith_ct_Port_Ctype_struct*)_tmpFA.f2)->tag == 6U){_LL22: _LL23:
# 677
 return*lowers;}else{goto _LL26;}default: _LL26: _LL27:
# 691
 goto _LL19;}}_LL19:;};}}
# 694
return({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));_tmp10E->hd=t,_tmp10E->tl=*lowers;_tmp10E;});}
# 701
static int Cyc_Port_leq(void*t1,void*t2){
# 707
if(t1 == t2)return 1;
({void*_tmp3FF=Cyc_Port_compress_ct(t1);t1=_tmp3FF;});
({void*_tmp400=Cyc_Port_compress_ct(t2);t2=_tmp400;});{
struct _tuple11 _tmp10F=({struct _tuple11 _tmp3A3;_tmp3A3.f1=t1,_tmp3A3.f2=t2;_tmp3A3;});struct _tuple11 _tmp110=_tmp10F;struct Cyc_Port_Cvar*_tmp12E;void*_tmp12D;void*_tmp12C;void*_tmp12B;void*_tmp12A;void*_tmp129;void*_tmp128;void*_tmp127;void*_tmp126;void*_tmp125;void*_tmp124;void*_tmp123;void*_tmp122;void*_tmp121;void*_tmp120;void*_tmp11F;void*_tmp11E;void*_tmp11D;void*_tmp11C;void*_tmp11B;void*_tmp11A;void*_tmp119;void*_tmp118;void*_tmp117;struct Cyc_Port_Cvar*_tmp116;struct Cyc_Port_Cvar*_tmp115;struct Cyc_Port_Cvar*_tmp114;struct _dyneither_ptr _tmp113;struct _dyneither_ptr _tmp112;struct _dyneither_ptr _tmp111;switch(*((int*)_tmp110.f1)){case 7U: _LL1: _LL2:
 return 1;case 10U: switch(*((int*)_tmp110.f2)){case 10U: _LL3: _tmp112=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp111=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp112,(struct _dyneither_ptr)_tmp111)== 0;case 7U: _LL5: _tmp113=*((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_LL6:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 1U: switch(*((int*)_tmp110.f2)){case 0U: _LL7: _LL8:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 0U: switch(*((int*)_tmp110.f2)){case 1U: _LL9: _LLA:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 9U: switch(*((int*)_tmp110.f2)){case 8U: _LLB: _LLC:
 return 0;case 16U: goto _LL2D;default: goto _LL2F;}case 8U: switch(*((int*)_tmp110.f2)){case 9U: _LLD: _LLE:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 16U: switch(*((int*)_tmp110.f2)){case 0U: _LLF: _LL10:
 return 1;case 4U: _LL11: _LL12:
 return 1;case 16U: _LL29: _tmp115=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp114=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL2A:
# 737
({struct Cyc_List_List*_tmp401=Cyc_Port_filter_self(t1,_tmp115->uppers);_tmp115->uppers=_tmp401;});
({struct Cyc_List_List*_tmp402=Cyc_Port_filter_self(t2,_tmp114->lowers);_tmp114->lowers=_tmp402;});
({struct Cyc_List_List*_tmp403=Cyc_Port_insert_upper(t1,t2,& _tmp115->uppers);_tmp115->uppers=_tmp403;});
({struct Cyc_List_List*_tmp404=Cyc_Port_insert_lower(t2,t1,& _tmp114->lowers);_tmp114->lowers=_tmp404;});
return 1;default: _LL2B: _tmp116=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_LL2C:
# 743
({struct Cyc_List_List*_tmp405=Cyc_Port_filter_self(t1,_tmp116->uppers);_tmp116->uppers=_tmp405;});
({struct Cyc_List_List*_tmp406=Cyc_Port_insert_upper(t1,t2,& _tmp116->uppers);_tmp116->uppers=_tmp406;});
return 1;}case 4U: _LL13: _LL14:
# 720
 return 0;case 5U: switch(*((int*)_tmp110.f2)){case 6U: _LL15: _LL16:
 return 1;case 11U: _LL17: _LL18:
 return 1;case 4U: _LL19: _LL1A:
 return 1;case 16U: goto _LL2D;default: goto _LL2F;}case 11U: switch(*((int*)_tmp110.f2)){case 6U: _LL1B: _LL1C:
 return 1;case 4U: _LL1D: _LL1E:
 return 1;case 11U: _LL23: _tmp120=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp11F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp11E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp11D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f4;_tmp11C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f1)->f5;_tmp11B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp11A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp119=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_tmp118=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f4;_tmp117=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f5;_LL24:
# 729
 return(((Cyc_Port_leq(_tmp120,_tmp11B) && Cyc_Port_leq(_tmp11F,_tmp11A)) && Cyc_Port_unifies(_tmp11E,_tmp119)) && Cyc_Port_leq(_tmp11D,_tmp118)) && 
Cyc_Port_leq(_tmp11C,_tmp117);case 16U: goto _LL2D;default: goto _LL2F;}case 12U: switch(*((int*)_tmp110.f2)){case 6U: _LL1F: _LL20:
# 726
 return 1;case 4U: _LL21: _LL22:
 return 1;case 12U: _LL25: _tmp126=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp125=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp124=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp123=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp122=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp121=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_LL26:
# 732
 return(Cyc_Port_leq(_tmp126,_tmp123) && Cyc_Port_leq(_tmp125,_tmp122)) && Cyc_Port_leq(_tmp124,_tmp121);case 11U: _LL27: _tmp12D=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f1;_tmp12C=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f2;_tmp12B=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp110.f1)->f3;_tmp12A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_tmp129=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f2;_tmp128=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f3;_tmp127=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp110.f2)->f5;_LL28:
# 734
 return((Cyc_Port_leq(_tmp12D,_tmp12A) && Cyc_Port_leq(_tmp12C,_tmp129)) && Cyc_Port_unifies((void*)& Cyc_Port_Fat_ct_val,_tmp128)) && 
Cyc_Port_leq(_tmp12B,_tmp127);case 16U: goto _LL2D;default: goto _LL2F;}default: if(((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->tag == 16U){_LL2D: _tmp12E=((struct Cyc_Port_Var_ct_Port_Ctype_struct*)_tmp110.f2)->f1;_LL2E:
# 747
({struct Cyc_List_List*_tmp407=Cyc_Port_filter_self(t2,_tmp12E->lowers);_tmp12E->lowers=_tmp407;});
({struct Cyc_List_List*_tmp408=Cyc_Port_insert_lower(t2,t1,& _tmp12E->lowers);_tmp12E->lowers=_tmp408;});
return 1;}else{_LL2F: _LL30:
 return Cyc_Port_unifies(t1,t2);}}_LL0:;};}struct Cyc_Port_GlobalCenv{struct Cyc_Dict_Dict typedef_dict;struct Cyc_Dict_Dict struct_dict;struct Cyc_Dict_Dict union_dict;void*return_type;struct Cyc_List_List*qualifier_edits;struct Cyc_List_List*pointer_edits;struct Cyc_List_List*zeroterm_edits;struct Cyc_List_List*vardecl_locs;struct Cyc_Hashtable_Table*varusage_tab;struct Cyc_List_List*edits;int porting;};
# 781
enum Cyc_Port_CPos{Cyc_Port_FnRes_pos  = 0U,Cyc_Port_FnArg_pos  = 1U,Cyc_Port_FnBody_pos  = 2U,Cyc_Port_Toplevel_pos  = 3U};struct Cyc_Port_Cenv{struct Cyc_Port_GlobalCenv*gcenv;struct Cyc_Dict_Dict var_dict;enum Cyc_Port_CPos cpos;};
# 794
static struct Cyc_Port_Cenv*Cyc_Port_empty_cenv(){
struct Cyc_Port_GlobalCenv*g=({struct Cyc_Port_GlobalCenv*_tmp130=_cycalloc(sizeof(*_tmp130));({struct Cyc_Dict_Dict _tmp40D=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->typedef_dict=_tmp40D;}),({
struct Cyc_Dict_Dict _tmp40C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->struct_dict=_tmp40C;}),({
struct Cyc_Dict_Dict _tmp40B=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp130->union_dict=_tmp40B;}),_tmp130->qualifier_edits=0,_tmp130->pointer_edits=0,_tmp130->zeroterm_edits=0,_tmp130->vardecl_locs=0,({
# 802
struct Cyc_Hashtable_Table*_tmp40A=((struct Cyc_Hashtable_Table*(*)(int sz,int(*cmp)(unsigned int,unsigned int),int(*hash)(unsigned int)))Cyc_Hashtable_create)(128,Cyc_Port_cmp_seg_t,Cyc_Port_hash_seg_t);_tmp130->varusage_tab=_tmp40A;}),_tmp130->edits=0,_tmp130->porting=0,({
# 805
void*_tmp409=Cyc_Port_void_ct();_tmp130->return_type=_tmp409;});_tmp130;});
return({struct Cyc_Port_Cenv*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->gcenv=g,_tmp12F->cpos=Cyc_Port_Toplevel_pos,({
# 808
struct Cyc_Dict_Dict _tmp40E=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp12F->var_dict=_tmp40E;});_tmp12F;});}
# 814
static int Cyc_Port_in_fn_arg(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnArg_pos;}
# 817
static int Cyc_Port_in_fn_res(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_FnRes_pos;}
# 820
static int Cyc_Port_in_toplevel(struct Cyc_Port_Cenv*env){
return env->cpos == Cyc_Port_Toplevel_pos;}
# 823
static void*Cyc_Port_lookup_return_type(struct Cyc_Port_Cenv*env){
return(env->gcenv)->return_type;}
# 826
static void*Cyc_Port_lookup_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp131;_push_handler(& _tmp131);{int _tmp133=0;if(setjmp(_tmp131.handler))_tmp133=1;if(!_tmp133){
{struct _tuple11 _tmp134=*((struct _tuple11*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple11 _tmp135=_tmp134;void*_tmp137;_LL1: _tmp137=_tmp135.f1;_LL2:;{
void*_tmp136=_tmp137;_npop_handler(0U);return _tmp136;};}
# 828
;_pop_handler();}else{void*_tmp132=(void*)_exn_thrown;void*_tmp138=_tmp132;void*_tmp139;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp138)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 835
 return Cyc_Absyn_sint_type;}else{_LL6: _tmp139=_tmp138;_LL7:(int)_rethrow(_tmp139);}_LL3:;}};}
# 840
static void*Cyc_Port_lookup_typedef_ctype(struct Cyc_Port_Cenv*env,struct _tuple0*n){
struct _handler_cons _tmp13A;_push_handler(& _tmp13A);{int _tmp13C=0;if(setjmp(_tmp13A.handler))_tmp13C=1;if(!_tmp13C){
{struct _tuple11 _tmp13D=*((struct _tuple11*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((env->gcenv)->typedef_dict,n);struct _tuple11 _tmp13E=_tmp13D;void*_tmp140;_LL1: _tmp140=_tmp13E.f2;_LL2:;{
void*_tmp13F=_tmp140;_npop_handler(0U);return _tmp13F;};}
# 842
;_pop_handler();}else{void*_tmp13B=(void*)_exn_thrown;void*_tmp141=_tmp13B;void*_tmp142;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp141)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 849
 return Cyc_Port_var();}else{_LL6: _tmp142=_tmp141;_LL7:(int)_rethrow(_tmp142);}_LL3:;}};}
# 855
static struct _tuple10*Cyc_Port_lookup_struct_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 857
struct _tuple10**_tmp143=((struct _tuple10**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->struct_dict,n);
if((unsigned int)_tmp143)
return*_tmp143;else{
# 861
struct Cyc_Absyn_Aggrdecl*_tmp144=({struct Cyc_Absyn_Aggrdecl*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->kind=Cyc_Absyn_StructA,_tmp146->sc=Cyc_Absyn_Public,_tmp146->name=n,_tmp146->tvs=0,_tmp146->impl=0,_tmp146->attributes=0,_tmp146->expected_mem_kind=0;_tmp146;});
# 864
struct _tuple10*p=({struct _tuple10*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->f1=_tmp144,_tmp145->f2=0;_tmp145;});
({struct Cyc_Dict_Dict _tmp40F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->struct_dict,n,p);(env->gcenv)->struct_dict=_tmp40F;});
return p;}}
# 872
static struct _tuple10*Cyc_Port_lookup_union_decl(struct Cyc_Port_Cenv*env,struct _tuple0*n){
# 874
struct _tuple10**_tmp147=((struct _tuple10**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((env->gcenv)->union_dict,n);
if((unsigned int)_tmp147)
return*_tmp147;else{
# 878
struct Cyc_Absyn_Aggrdecl*_tmp148=({struct Cyc_Absyn_Aggrdecl*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->kind=Cyc_Absyn_UnionA,_tmp14A->sc=Cyc_Absyn_Public,_tmp14A->name=n,_tmp14A->tvs=0,_tmp14A->impl=0,_tmp14A->attributes=0,_tmp14A->expected_mem_kind=0;_tmp14A;});
# 881
struct _tuple10*p=({struct _tuple10*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->f1=_tmp148,_tmp149->f2=0;_tmp149;});
({struct Cyc_Dict_Dict _tmp410=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple10*v))Cyc_Dict_insert)((env->gcenv)->union_dict,n,p);(env->gcenv)->union_dict=_tmp410;});
return p;}}struct _tuple13{struct _tuple11 f1;unsigned int f2;};struct _tuple14{void*f1;struct _tuple11*f2;unsigned int f3;};
# 888
static struct _tuple13 Cyc_Port_lookup_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp14B;_push_handler(& _tmp14B);{int _tmp14D=0;if(setjmp(_tmp14B.handler))_tmp14D=1;if(!_tmp14D){
{struct _tuple14 _tmp14E=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp14F=_tmp14E;struct _tuple11*_tmp152;unsigned int _tmp151;_LL1: _tmp152=_tmp14F.f2;_tmp151=_tmp14F.f3;_LL2:;{
struct _tuple13 _tmp150=({struct _tuple13 _tmp3A4;_tmp3A4.f1=*_tmp152,_tmp3A4.f2=_tmp151;_tmp3A4;});_npop_handler(0U);return _tmp150;};}
# 890
;_pop_handler();}else{void*_tmp14C=(void*)_exn_thrown;void*_tmp153=_tmp14C;void*_tmp154;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp153)->tag == Cyc_Dict_Absent){_LL4: _LL5:
# 897
 return({struct _tuple13 _tmp3A5;({void*_tmp412=Cyc_Port_var();(_tmp3A5.f1).f1=_tmp412;}),({void*_tmp411=Cyc_Port_var();(_tmp3A5.f1).f2=_tmp411;}),_tmp3A5.f2=0U;_tmp3A5;});}else{_LL6: _tmp154=_tmp153;_LL7:(int)_rethrow(_tmp154);}_LL3:;}};}
# 900
static struct _tuple14*Cyc_Port_lookup_full_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);}
# 904
static int Cyc_Port_declared_var(struct Cyc_Port_Cenv*env,struct _tuple0*x){
return((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)(env->var_dict,x);}
# 908
static int Cyc_Port_isfn(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp155;_push_handler(& _tmp155);{int _tmp157=0;if(setjmp(_tmp155.handler))_tmp157=1;if(!_tmp157){
{struct _tuple14 _tmp158=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp159=_tmp158;void*_tmp15E;_LL1: _tmp15E=_tmp159.f1;_LL2:;
LOOP: {void*_tmp15A=_tmp15E;struct _tuple0*_tmp15D;switch(*((int*)_tmp15A)){case 8U: _LL4: _tmp15D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15A)->f1;_LL5:
({void*_tmp413=Cyc_Port_lookup_typedef(env,_tmp15D);_tmp15E=_tmp413;});goto LOOP;case 5U: _LL6: _LL7: {
int _tmp15B=1;_npop_handler(0U);return _tmp15B;}default: _LL8: _LL9: {
int _tmp15C=0;_npop_handler(0U);return _tmp15C;}}_LL3:;}}
# 910
;_pop_handler();}else{void*_tmp156=(void*)_exn_thrown;void*_tmp15F=_tmp156;void*_tmp160;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp15F)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 921
 return 0;}else{_LLD: _tmp160=_tmp15F;_LLE:(int)_rethrow(_tmp160);}_LLA:;}};}
# 925
static int Cyc_Port_isarray(struct Cyc_Port_Cenv*env,struct _tuple0*x){
struct _handler_cons _tmp161;_push_handler(& _tmp161);{int _tmp163=0;if(setjmp(_tmp161.handler))_tmp163=1;if(!_tmp163){
{struct _tuple14 _tmp164=*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->var_dict,x);struct _tuple14 _tmp165=_tmp164;void*_tmp16A;_LL1: _tmp16A=_tmp165.f1;_LL2:;
LOOP: {void*_tmp166=_tmp16A;struct _tuple0*_tmp169;switch(*((int*)_tmp166)){case 8U: _LL4: _tmp169=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp166)->f1;_LL5:
({void*_tmp414=Cyc_Port_lookup_typedef(env,_tmp169);_tmp16A=_tmp414;});goto LOOP;case 4U: _LL6: _LL7: {
int _tmp167=1;_npop_handler(0U);return _tmp167;}default: _LL8: _LL9: {
int _tmp168=0;_npop_handler(0U);return _tmp168;}}_LL3:;}}
# 927
;_pop_handler();}else{void*_tmp162=(void*)_exn_thrown;void*_tmp16B=_tmp162;void*_tmp16C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp16B)->tag == Cyc_Dict_Absent){_LLB: _LLC:
# 938
 return 0;}else{_LLD: _tmp16C=_tmp16B;_LLE:(int)_rethrow(_tmp16C);}_LLA:;}};}
# 944
static struct Cyc_Port_Cenv*Cyc_Port_set_cpos(struct Cyc_Port_Cenv*env,enum Cyc_Port_CPos cpos){
return({struct Cyc_Port_Cenv*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->gcenv=env->gcenv,_tmp16D->var_dict=env->var_dict,_tmp16D->cpos=cpos;_tmp16D;});}
# 949
static void Cyc_Port_add_return_type(struct Cyc_Port_Cenv*env,void*t){
(env->gcenv)->return_type=t;}
# 954
static struct Cyc_Port_Cenv*Cyc_Port_add_var(struct Cyc_Port_Cenv*env,struct _tuple0*x,void*t,void*qual,void*ctype,unsigned int loc){
# 956
return({struct Cyc_Port_Cenv*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->gcenv=env->gcenv,({
struct Cyc_Dict_Dict _tmp418=({struct Cyc_Dict_Dict _tmp417=env->var_dict;struct _tuple0*_tmp416=x;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp417,_tmp416,({struct _tuple14*_tmp16F=_cycalloc(sizeof(*_tmp16F));_tmp16F->f1=t,({struct _tuple11*_tmp415=({struct _tuple11*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->f1=qual,_tmp16E->f2=ctype;_tmp16E;});_tmp16F->f2=_tmp415;}),_tmp16F->f3=loc;_tmp16F;}));});_tmp170->var_dict=_tmp418;}),_tmp170->cpos=env->cpos;_tmp170;});}
# 962
static void Cyc_Port_add_typedef(struct Cyc_Port_Cenv*env,struct _tuple0*n,void*t,void*ct){
({struct Cyc_Dict_Dict _tmp41B=({struct Cyc_Dict_Dict _tmp41A=(env->gcenv)->typedef_dict;struct _tuple0*_tmp419=n;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple11*v))Cyc_Dict_insert)(_tmp41A,_tmp419,({struct _tuple11*_tmp171=_cycalloc(sizeof(*_tmp171));
_tmp171->f1=t,_tmp171->f2=ct;_tmp171;}));});
# 963
(env->gcenv)->typedef_dict=_tmp41B;});}struct _tuple15{struct _tuple0*f1;unsigned int f2;void*f3;};
# 966
static void Cyc_Port_register_localvar_decl(struct Cyc_Port_Cenv*env,struct _tuple0*x,unsigned int loc,void*type,struct Cyc_Absyn_Exp*init){
({struct Cyc_List_List*_tmp41D=({struct Cyc_List_List*_tmp173=_cycalloc(sizeof(*_tmp173));({struct _tuple15*_tmp41C=({struct _tuple15*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->f1=x,_tmp172->f2=loc,_tmp172->f3=type;_tmp172;});_tmp173->hd=_tmp41C;}),_tmp173->tl=(env->gcenv)->vardecl_locs;_tmp173;});(env->gcenv)->vardecl_locs=_tmp41D;});
({struct Cyc_Hashtable_Table*_tmp41F=(env->gcenv)->varusage_tab;unsigned int _tmp41E=loc;((void(*)(struct Cyc_Hashtable_Table*t,unsigned int key,struct Cyc_Port_VarUsage*val))Cyc_Hashtable_insert)(_tmp41F,_tmp41E,({struct Cyc_Port_VarUsage*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->address_taken=0,_tmp174->init=init,_tmp174->usage_locs=0;_tmp174;}));});}
# 970
static void Cyc_Port_register_localvar_usage(struct Cyc_Port_Cenv*env,unsigned int declloc,unsigned int useloc,int takeaddress){
struct Cyc_Port_VarUsage*varusage=0;
if(((int(*)(struct Cyc_Hashtable_Table*t,unsigned int key,struct Cyc_Port_VarUsage**data))Cyc_Hashtable_try_lookup)((env->gcenv)->varusage_tab,declloc,& varusage)){
if(takeaddress)((struct Cyc_Port_VarUsage*)_check_null(varusage))->address_taken=1;{
struct Cyc_List_List*_tmp175=((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs;
({struct Cyc_List_List*_tmp420=({struct Cyc_List_List*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->hd=(void*)useloc,_tmp176->tl=_tmp175;_tmp176;});((struct Cyc_Port_VarUsage*)_check_null(varusage))->usage_locs=_tmp420;});};}}struct _tuple16{void*f1;void*f2;unsigned int f3;};
# 981
static void Cyc_Port_register_const_cvar(struct Cyc_Port_Cenv*env,void*new_qual,void*orig_qual,unsigned int loc){
# 983
({struct Cyc_List_List*_tmp422=({struct Cyc_List_List*_tmp178=_cycalloc(sizeof(*_tmp178));({struct _tuple16*_tmp421=({struct _tuple16*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=new_qual,_tmp177->f2=orig_qual,_tmp177->f3=loc;_tmp177;});_tmp178->hd=_tmp421;}),_tmp178->tl=(env->gcenv)->qualifier_edits;_tmp178;});(env->gcenv)->qualifier_edits=_tmp422;});}
# 989
static void Cyc_Port_register_ptr_cvars(struct Cyc_Port_Cenv*env,void*new_ptr,void*orig_ptr,unsigned int loc){
# 991
({struct Cyc_List_List*_tmp424=({struct Cyc_List_List*_tmp17A=_cycalloc(sizeof(*_tmp17A));({struct _tuple16*_tmp423=({struct _tuple16*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->f1=new_ptr,_tmp179->f2=orig_ptr,_tmp179->f3=loc;_tmp179;});_tmp17A->hd=_tmp423;}),_tmp17A->tl=(env->gcenv)->pointer_edits;_tmp17A;});(env->gcenv)->pointer_edits=_tmp424;});}
# 995
static void Cyc_Port_register_zeroterm_cvars(struct Cyc_Port_Cenv*env,void*new_zt,void*orig_zt,unsigned int loc){
# 997
({struct Cyc_List_List*_tmp426=({struct Cyc_List_List*_tmp17C=_cycalloc(sizeof(*_tmp17C));({struct _tuple16*_tmp425=({struct _tuple16*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=new_zt,_tmp17B->f2=orig_zt,_tmp17B->f3=loc;_tmp17B;});_tmp17C->hd=_tmp425;}),_tmp17C->tl=(env->gcenv)->zeroterm_edits;_tmp17C;});(env->gcenv)->zeroterm_edits=_tmp426;});}
# 1003
static void*Cyc_Port_main_type(){
struct _tuple8*arg1=({struct _tuple8*_tmp180=_cycalloc(sizeof(*_tmp180));
_tmp180->f1=0,({struct Cyc_Absyn_Tqual _tmp427=Cyc_Absyn_empty_tqual(0U);_tmp180->f2=_tmp427;}),_tmp180->f3=Cyc_Absyn_sint_type;_tmp180;});
struct _tuple8*arg2=({struct _tuple8*_tmp17F=_cycalloc(sizeof(*_tmp17F));
_tmp17F->f1=0,({struct Cyc_Absyn_Tqual _tmp42C=Cyc_Absyn_empty_tqual(0U);_tmp17F->f2=_tmp42C;}),({
void*_tmp42B=({void*_tmp42A=Cyc_Absyn_string_type(Cyc_Absyn_wildtyp(0));void*_tmp429=Cyc_Absyn_wildtyp(0);struct Cyc_Absyn_Tqual _tmp428=
Cyc_Absyn_empty_tqual(0U);
# 1008
Cyc_Absyn_fatptr_type(_tmp42A,_tmp429,_tmp428,
Cyc_Tcutil_any_bool(0));});
# 1008
_tmp17F->f3=_tmp42B;});_tmp17F;});
# 1010
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->tag=5U,(_tmp17E->f1).tvars=0,(_tmp17E->f1).effect=0,({
struct Cyc_Absyn_Tqual _tmp42E=Cyc_Absyn_empty_tqual(0U);(_tmp17E->f1).ret_tqual=_tmp42E;}),(_tmp17E->f1).ret_type=Cyc_Absyn_sint_type,({
# 1013
struct Cyc_List_List*_tmp42D=({struct _tuple8*_tmp17D[2U];_tmp17D[0]=arg1,_tmp17D[1]=arg2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp17D,sizeof(struct _tuple8*),2U));});(_tmp17E->f1).args=_tmp42D;}),(_tmp17E->f1).c_varargs=0,(_tmp17E->f1).cyc_varargs=0,(_tmp17E->f1).rgn_po=0,(_tmp17E->f1).attributes=0,(_tmp17E->f1).requires_clause=0,(_tmp17E->f1).requires_relns=0,(_tmp17E->f1).ensures_clause=0,(_tmp17E->f1).ensures_relns=0;_tmp17E;});}
# 1022
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t);
# 1025
static struct Cyc_Port_Cenv*Cyc_Port_initial_cenv(){
struct Cyc_Port_Cenv*_tmp181=Cyc_Port_empty_cenv();
# 1028
({struct Cyc_Port_Cenv*_tmp436=({struct Cyc_Port_Cenv*_tmp435=_tmp181;struct _tuple0*_tmp434=({struct _tuple0*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp430=({struct _dyneither_ptr*_tmp183=_cycalloc(sizeof(*_tmp183));({struct _dyneither_ptr _tmp42F=({const char*_tmp182="main";_tag_dyneither(_tmp182,sizeof(char),5U);});*_tmp183=_tmp42F;});_tmp183;});_tmp184->f2=_tmp430;});_tmp184;});void*_tmp433=Cyc_Port_main_type();void*_tmp432=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp435,_tmp434,_tmp433,_tmp432,({
struct Cyc_Port_Cenv*_tmp431=_tmp181;Cyc_Port_type_to_ctype(_tmp431,Cyc_Port_main_type());}),0U);});
# 1028
_tmp181=_tmp436;});
# 1030
return _tmp181;}
# 1036
static void Cyc_Port_region_counts(struct Cyc_Dict_Dict*cts,void*t){
void*_tmp185=Cyc_Port_compress_ct(t);void*_tmp186=_tmp185;void*_tmp193;struct Cyc_List_List*_tmp192;void*_tmp191;void*_tmp190;void*_tmp18F;void*_tmp18E;void*_tmp18D;void*_tmp18C;void*_tmp18B;void*_tmp18A;struct _dyneither_ptr*_tmp189;switch(*((int*)_tmp186)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 14U: _LLF: _LL10:
 goto _LL12;case 13U: _LL11: _LL12:
 goto _LL14;case 16U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 goto _LL18;case 9U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 return;case 10U: _LL1B: _tmp189=((struct Cyc_Port_RgnVar_ct_Port_Ctype_struct*)_tmp186)->f1;_LL1C:
# 1052
 if(!((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(*cts,_tmp189))
({struct Cyc_Dict_Dict _tmp439=({struct Cyc_Dict_Dict _tmp438=*cts;struct _dyneither_ptr*_tmp437=_tmp189;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,int*v))Cyc_Dict_insert)(_tmp438,_tmp437,({int*_tmp187=_cycalloc_atomic(sizeof(*_tmp187));*_tmp187=0;_tmp187;}));});*cts=_tmp439;});{
int*_tmp188=((int*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(*cts,_tmp189);
*_tmp188=*_tmp188 + 1;
return;};case 11U: _LL1D: _tmp18E=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp18D=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f2;_tmp18C=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f3;_tmp18B=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f4;_tmp18A=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp186)->f5;_LL1E:
# 1058
 Cyc_Port_region_counts(cts,_tmp18E);
Cyc_Port_region_counts(cts,_tmp18B);
return;case 12U: _LL1F: _tmp191=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp190=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f2;_tmp18F=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp186)->f3;_LL20:
# 1062
 Cyc_Port_region_counts(cts,_tmp191);
return;default: _LL21: _tmp193=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp186)->f1;_tmp192=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp186)->f2;_LL22:
# 1065
 Cyc_Port_region_counts(cts,_tmp193);
for(0;(unsigned int)_tmp192;_tmp192=_tmp192->tl){Cyc_Port_region_counts(cts,(void*)_tmp192->hd);}
return;}_LL0:;}
# 1076
static void Cyc_Port_register_rgns(struct Cyc_Port_Cenv*env,struct Cyc_Dict_Dict counts,int fn_res,void*t,void*c){
# 1078
({void*_tmp43A=Cyc_Port_compress_ct(c);c=_tmp43A;});{
struct _tuple11 _tmp194=({struct _tuple11 _tmp3A6;_tmp3A6.f1=t,_tmp3A6.f2=c;_tmp3A6;});struct _tuple11 _tmp195=_tmp194;void*_tmp1A9;struct Cyc_List_List*_tmp1A8;void*_tmp1A7;struct Cyc_List_List*_tmp1A6;void*_tmp1A5;void*_tmp1A4;void*_tmp1A3;void*_tmp1A2;struct Cyc_Absyn_PtrLoc*_tmp1A1;void*_tmp1A0;void*_tmp19F;switch(*((int*)_tmp195.f1)){case 3U: if(((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 11U){_LL1: _tmp1A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).elt_type;_tmp1A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).ptr_atts).rgn;_tmp1A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp195.f1)->f1).ptr_atts).ptrloc;_tmp1A0=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_tmp19F=(void*)((struct Cyc_Port_Ptr_ct_Port_Ctype_struct*)_tmp195.f2)->f4;_LL2:
# 1081
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1A3,_tmp1A0);{
unsigned int _tmp196=(unsigned int)_tmp1A1?_tmp1A1->rgn_loc:(unsigned int)0;
({void*_tmp43B=Cyc_Port_compress_ct(_tmp19F);_tmp19F=_tmp43B;});
# 1098 "port.cyc"
({struct Cyc_List_List*_tmp43F=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));
({struct Cyc_Port_Edit*_tmp43E=({struct Cyc_Port_Edit*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->loc=_tmp196,({struct _dyneither_ptr _tmp43D=({const char*_tmp197="`H ";_tag_dyneither(_tmp197,sizeof(char),4U);});_tmp199->old_string=_tmp43D;}),({struct _dyneither_ptr _tmp43C=({const char*_tmp198="";_tag_dyneither(_tmp198,sizeof(char),1U);});_tmp199->new_string=_tmp43C;});_tmp199;});_tmp19A->hd=_tmp43E;}),_tmp19A->tl=(env->gcenv)->edits;_tmp19A;});
# 1098
(env->gcenv)->edits=_tmp43F;});
# 1101
goto _LL0;};}else{goto _LL7;}case 4U: if(((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 12U){_LL3: _tmp1A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp195.f1)->f1).elt_type;_tmp1A4=(void*)((struct Cyc_Port_Array_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_LL4:
# 1103
 Cyc_Port_register_rgns(env,counts,fn_res,_tmp1A5,_tmp1A4);
goto _LL0;}else{goto _LL7;}case 5U: if(((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->tag == 15U){_LL5: _tmp1A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp195.f1)->f1).ret_type;_tmp1A8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp195.f1)->f1).args;_tmp1A7=(void*)((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->f1;_tmp1A6=((struct Cyc_Port_Fn_ct_Port_Ctype_struct*)_tmp195.f2)->f2;_LL6:
# 1106
 Cyc_Port_register_rgns(env,counts,1,_tmp1A9,_tmp1A7);
for(0;_tmp1A8 != 0  && _tmp1A6 != 0;(_tmp1A8=_tmp1A8->tl,_tmp1A6=_tmp1A6->tl)){
struct _tuple8 _tmp19B=*((struct _tuple8*)_tmp1A8->hd);struct _tuple8 _tmp19C=_tmp19B;void*_tmp19E;_LLA: _tmp19E=_tmp19C.f3;_LLB:;{
void*_tmp19D=(void*)_tmp1A6->hd;
Cyc_Port_register_rgns(env,counts,0,_tmp19E,_tmp19D);};}
# 1112
goto _LL0;}else{goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;};}
# 1119
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e);
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s);
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int takeaddress);
# 1124
static int Cyc_Port_is_char(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1AA=t;struct _tuple0*_tmp1AB;switch(*((int*)_tmp1AA)){case 8U: _LL1: _tmp1AB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1AA)->f1;_LL2:
# 1127
(*_tmp1AB).f1=Cyc_Absyn_Loc_n;
return({struct Cyc_Port_Cenv*_tmp440=env;Cyc_Port_is_char(_tmp440,Cyc_Port_lookup_typedef(env,_tmp1AB));});case 0U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->f2 == Cyc_Absyn_Char_sz){_LL3: _LL4:
 return 1;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 1135
static void*Cyc_Port_type_to_ctype(struct Cyc_Port_Cenv*env,void*t){
void*_tmp1AC=t;void*_tmp1E3;struct Cyc_List_List*_tmp1E2;void*_tmp1E1;struct Cyc_Absyn_Tqual _tmp1E0;void*_tmp1DF;unsigned int _tmp1DE;void*_tmp1DD;struct Cyc_Absyn_Tqual _tmp1DC;void*_tmp1DB;void*_tmp1DA;void*_tmp1D9;void*_tmp1D8;struct Cyc_Absyn_PtrLoc*_tmp1D7;struct Cyc_List_List*_tmp1D6;union Cyc_Absyn_AggrInfo _tmp1D5;struct _tuple0*_tmp1D4;switch(*((int*)_tmp1AC)){case 8U: _LL1: _tmp1D4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1AC)->f1;_LL2:
# 1138
(*_tmp1D4).f1=Cyc_Absyn_Loc_n;
return Cyc_Port_lookup_typedef_ctype(env,_tmp1D4);case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)){case 0U: _LL3: _LL4:
 return Cyc_Port_void_ct();case 1U: _LL7: _LL8:
# 1217
 goto _LLA;case 2U: _LL9: _LLA:
 return Cyc_Port_arith_ct();case 20U: _LLF: _tmp1D5=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL10: {
# 1260
union Cyc_Absyn_AggrInfo _tmp1C9=_tmp1D5;struct Cyc_Absyn_Aggrdecl**_tmp1D3;struct _tuple0*_tmp1D2;struct _tuple0*_tmp1D1;if((_tmp1C9.UnknownAggr).tag == 1){if(((_tmp1C9.UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL38: _tmp1D1=((_tmp1C9.UnknownAggr).val).f2;_LL39:
# 1262
(*_tmp1D1).f1=Cyc_Absyn_Loc_n;{
struct _tuple10*_tmp1CA=Cyc_Port_lookup_struct_decl(env,_tmp1D1);
return Cyc_Port_known_aggr_ct(_tmp1CA);};}else{_LL3A: _tmp1D2=((_tmp1C9.UnknownAggr).val).f2;_LL3B:
# 1266
(*_tmp1D2).f1=Cyc_Absyn_Loc_n;{
struct _tuple10*_tmp1CB=Cyc_Port_lookup_union_decl(env,_tmp1D2);
return Cyc_Port_known_aggr_ct(_tmp1CB);};}}else{_LL3C: _tmp1D3=(_tmp1C9.KnownAggr).val;_LL3D: {
# 1270
struct Cyc_Absyn_Aggrdecl*_tmp1CC=*_tmp1D3;
enum Cyc_Absyn_AggrKind _tmp1CD=_tmp1CC->kind;enum Cyc_Absyn_AggrKind _tmp1CE=_tmp1CD;if(_tmp1CE == Cyc_Absyn_StructA){_LL3F: _LL40: {
# 1273
struct _tuple10*_tmp1CF=Cyc_Port_lookup_struct_decl(env,_tmp1CC->name);
return Cyc_Port_known_aggr_ct(_tmp1CF);}}else{_LL41: _LL42: {
# 1276
struct _tuple10*_tmp1D0=Cyc_Port_lookup_union_decl(env,_tmp1CC->name);
return Cyc_Port_known_aggr_ct(_tmp1D0);}}_LL3E:;}}_LL37:;}case 15U: _LL11: _LL12:
# 1280
 return Cyc_Port_arith_ct();case 16U: _LL13: _tmp1D6=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->f1;_LL14:
# 1283
 for(0;(unsigned int)_tmp1D6;_tmp1D6=_tmp1D6->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp1D6->hd)->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp445=({struct Cyc_Port_Cenv*_tmp444=env;struct _tuple0*_tmp443=((struct Cyc_Absyn_Enumfield*)_tmp1D6->hd)->name;void*_tmp442=Cyc_Absyn_sint_type;void*_tmp441=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp444,_tmp443,_tmp442,_tmp441,Cyc_Port_arith_ct(),0U);});env=_tmp445;});}
# 1287
return Cyc_Port_arith_ct();default: goto _LL15;}case 3U: _LL5: _tmp1DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).elt_type;_tmp1DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).elt_tq;_tmp1DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).rgn;_tmp1DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).nullable;_tmp1D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).bounds;_tmp1D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).zero_term;_tmp1D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1AC)->f1).ptr_atts).ptrloc;_LL6: {
# 1142
unsigned int _tmp1AD=(unsigned int)_tmp1D7?_tmp1D7->ptr_loc:(unsigned int)0;
unsigned int _tmp1AE=(unsigned int)_tmp1D7?_tmp1D7->rgn_loc:(unsigned int)0;
unsigned int _tmp1AF=(unsigned int)_tmp1D7?_tmp1D7->zt_loc:(unsigned int)0;
# 1148
void*_tmp1B0=Cyc_Port_type_to_ctype(env,_tmp1DD);
void*new_rgn;
# 1151
{void*_tmp1B1=_tmp1DB;struct _dyneither_ptr*_tmp1B2;switch(*((int*)_tmp1B1)){case 0U: if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f1)->tag == 5U){_LL18: _LL19:
# 1153
({void*_tmp446=Cyc_Port_heap_ct();new_rgn=_tmp446;});goto _LL17;}else{goto _LL1E;}case 2U: _LL1A: _tmp1B2=(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1)->f1)->name;_LL1B:
# 1155
({void*_tmp447=Cyc_Port_rgnvar_ct(_tmp1B2);new_rgn=_tmp447;});goto _LL17;case 1U: _LL1C: _LL1D:
# 1159
 if(Cyc_Port_in_fn_arg(env))
({void*_tmp448=Cyc_Port_rgnvar_ct(Cyc_Port_new_region_var());new_rgn=_tmp448;});else{
# 1162
if(Cyc_Port_in_fn_res(env) || Cyc_Port_in_toplevel(env))
({void*_tmp449=Cyc_Port_heap_ct();new_rgn=_tmp449;});else{
({void*_tmp44A=Cyc_Port_var();new_rgn=_tmp44A;});}}
goto _LL17;default: _LL1E: _LL1F:
# 1167
({void*_tmp44B=Cyc_Port_heap_ct();new_rgn=_tmp44B;});goto _LL17;}_LL17:;}{
# 1169
int _tmp1B3=Cyc_Tcutil_get_bounds_exp(Cyc_Absyn_fat_bound_type,_tmp1D9)== 0;
int orig_zt;
{void*_tmp1B4=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1B5=_tmp1B4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B5)->tag == 1U){_LL21: _LL22:
({int _tmp44C=Cyc_Port_is_char(env,_tmp1DD);orig_zt=_tmp44C;});goto _LL20;}else{_LL23: _LL24:
({int _tmp44D=Cyc_Tcutil_force_type2bool(0,_tmp1D8);orig_zt=_tmp44D;});goto _LL20;}_LL20:;}
# 1175
if((env->gcenv)->porting){
void*_tmp1B6=Cyc_Port_var();
# 1180
({struct Cyc_Port_Cenv*_tmp450=env;void*_tmp44F=_tmp1B6;void*_tmp44E=_tmp1DC.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp450,_tmp44F,_tmp44E,_tmp1DC.loc);});
# 1183
if(_tmp1DC.print_const)({void*_tmp451=_tmp1B6;Cyc_Port_unify_ct(_tmp451,Cyc_Port_const_ct());});{
# 1189
void*_tmp1B7=Cyc_Port_var();
# 1192
({struct Cyc_Port_Cenv*_tmp454=env;void*_tmp453=_tmp1B7;void*_tmp452=_tmp1B3?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();Cyc_Port_register_ptr_cvars(_tmp454,_tmp453,_tmp452,_tmp1AD);});
# 1194
if(_tmp1B3)({void*_tmp455=_tmp1B7;Cyc_Port_unify_ct(_tmp455,Cyc_Port_fat_ct());});{
void*_tmp1B8=Cyc_Port_var();
# 1198
({struct Cyc_Port_Cenv*_tmp458=env;void*_tmp457=_tmp1B8;void*_tmp456=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp458,_tmp457,_tmp456,_tmp1AF);});
# 1200
{void*_tmp1B9=Cyc_Tcutil_compress(_tmp1D8);void*_tmp1BA=_tmp1B9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BA)->tag == 1U){_LL26: _LL27:
 goto _LL25;}else{_LL28: _LL29:
# 1203
({void*_tmp459=_tmp1B8;Cyc_Port_unify_ct(_tmp459,Cyc_Tcutil_force_type2bool(0,_tmp1D8)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL25;}_LL25:;}
# 1209
return Cyc_Port_ptr_ct(_tmp1B0,_tmp1B6,_tmp1B7,new_rgn,_tmp1B8);};};}else{
# 1213
return({void*_tmp45D=_tmp1B0;void*_tmp45C=_tmp1DC.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();void*_tmp45B=
_tmp1B3?Cyc_Port_fat_ct(): Cyc_Port_thin_ct();
# 1213
void*_tmp45A=new_rgn;Cyc_Port_ptr_ct(_tmp45D,_tmp45C,_tmp45B,_tmp45A,
# 1215
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}};}case 4U: _LLB: _tmp1E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).elt_type;_tmp1E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).tq;_tmp1DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).zero_term;_tmp1DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1AC)->f1).zt_loc;_LLC: {
# 1221
void*_tmp1BB=Cyc_Port_type_to_ctype(env,_tmp1E1);
int orig_zt;
{void*_tmp1BC=Cyc_Tcutil_compress(_tmp1DF);void*_tmp1BD=_tmp1BC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->tag == 1U){_LL2B: _LL2C:
 orig_zt=0;goto _LL2A;}else{_LL2D: _LL2E:
({int _tmp45E=Cyc_Tcutil_force_type2bool(0,_tmp1DF);orig_zt=_tmp45E;});goto _LL2A;}_LL2A:;}
# 1227
if((env->gcenv)->porting){
void*_tmp1BE=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp461=env;void*_tmp460=_tmp1BE;void*_tmp45F=_tmp1E0.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp461,_tmp460,_tmp45F,_tmp1E0.loc);});{
# 1236
void*_tmp1BF=Cyc_Port_var();
({struct Cyc_Port_Cenv*_tmp464=env;void*_tmp463=_tmp1BF;void*_tmp462=orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct();Cyc_Port_register_zeroterm_cvars(_tmp464,_tmp463,_tmp462,_tmp1DE);});
# 1239
{void*_tmp1C0=Cyc_Tcutil_compress(_tmp1DF);void*_tmp1C1=_tmp1C0;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C1)->tag == 1U){_LL30: _LL31:
 goto _LL2F;}else{_LL32: _LL33:
# 1242
({void*_tmp465=_tmp1BF;Cyc_Port_unify_ct(_tmp465,Cyc_Tcutil_force_type2bool(0,_tmp1DF)?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});
goto _LL2F;}_LL2F:;}
# 1245
return Cyc_Port_array_ct(_tmp1BB,_tmp1BE,_tmp1BF);};}else{
# 1247
return({void*_tmp467=_tmp1BB;void*_tmp466=_tmp1E0.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_array_ct(_tmp467,_tmp466,
orig_zt?Cyc_Port_zterm_ct(): Cyc_Port_nozterm_ct());});}}case 5U: _LLD: _tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).ret_type;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1AC)->f1).args;_LLE: {
# 1251
void*_tmp1C2=({struct Cyc_Port_Cenv*_tmp468=Cyc_Port_set_cpos(env,Cyc_Port_FnRes_pos);Cyc_Port_type_to_ctype(_tmp468,_tmp1E3);});
struct Cyc_Port_Cenv*_tmp1C3=Cyc_Port_set_cpos(env,Cyc_Port_FnArg_pos);
struct Cyc_List_List*cargs=0;
for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
struct _tuple8 _tmp1C4=*((struct _tuple8*)_tmp1E2->hd);struct _tuple8 _tmp1C5=_tmp1C4;struct Cyc_Absyn_Tqual _tmp1C8;void*_tmp1C7;_LL35: _tmp1C8=_tmp1C5.f2;_tmp1C7=_tmp1C5.f3;_LL36:;
({struct Cyc_List_List*_tmp46A=({struct Cyc_List_List*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));({void*_tmp469=Cyc_Port_type_to_ctype(_tmp1C3,_tmp1C7);_tmp1C6->hd=_tmp469;}),_tmp1C6->tl=cargs;_tmp1C6;});cargs=_tmp46A;});}
# 1258
return({void*_tmp46B=_tmp1C2;Cyc_Port_fn_ct(_tmp46B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(cargs));});}default: _LL15: _LL16:
# 1288
 return Cyc_Port_arith_ct();}_LL0:;}
# 1293
static void*Cyc_Port_gen_primop(struct Cyc_Port_Cenv*env,enum Cyc_Absyn_Primop p,struct Cyc_List_List*args){
void*_tmp1E4=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(args))->hd);
struct Cyc_List_List*_tmp1E5=args->tl;
enum Cyc_Absyn_Primop _tmp1E6=p;enum Cyc_Absyn_Primop _tmp1EB;switch(_tmp1E6){case Cyc_Absyn_Plus: _LL1: _LL2: {
# 1301
void*_tmp1E7=Cyc_Port_compress_ct((void*)((struct Cyc_List_List*)_check_null(_tmp1E5))->hd);
if(({void*_tmp470=_tmp1E4;Cyc_Port_leq(_tmp470,({void*_tmp46F=Cyc_Port_var();void*_tmp46E=Cyc_Port_var();void*_tmp46D=Cyc_Port_fat_ct();void*_tmp46C=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp46F,_tmp46E,_tmp46D,_tmp46C,Cyc_Port_var());}));})){
({void*_tmp471=_tmp1E7;Cyc_Port_leq(_tmp471,Cyc_Port_arith_ct());});
return _tmp1E4;}else{
if(({void*_tmp476=_tmp1E7;Cyc_Port_leq(_tmp476,({void*_tmp475=Cyc_Port_var();void*_tmp474=Cyc_Port_var();void*_tmp473=Cyc_Port_fat_ct();void*_tmp472=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp475,_tmp474,_tmp473,_tmp472,Cyc_Port_var());}));})){
({void*_tmp477=_tmp1E4;Cyc_Port_leq(_tmp477,Cyc_Port_arith_ct());});
return _tmp1E7;}else{
# 1309
({void*_tmp478=_tmp1E4;Cyc_Port_leq(_tmp478,Cyc_Port_arith_ct());});
({void*_tmp479=_tmp1E7;Cyc_Port_leq(_tmp479,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}}case Cyc_Absyn_Minus: _LL3: _LL4:
# 1318
 if(_tmp1E5 == 0){
# 1320
({void*_tmp47A=_tmp1E4;Cyc_Port_leq(_tmp47A,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}else{
# 1323
void*_tmp1E8=Cyc_Port_compress_ct((void*)_tmp1E5->hd);
if(({void*_tmp47F=_tmp1E4;Cyc_Port_leq(_tmp47F,({void*_tmp47E=Cyc_Port_var();void*_tmp47D=Cyc_Port_var();void*_tmp47C=Cyc_Port_fat_ct();void*_tmp47B=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp47E,_tmp47D,_tmp47C,_tmp47B,Cyc_Port_var());}));})){
if(({void*_tmp484=_tmp1E8;Cyc_Port_leq(_tmp484,({void*_tmp483=Cyc_Port_var();void*_tmp482=Cyc_Port_var();void*_tmp481=Cyc_Port_fat_ct();void*_tmp480=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp483,_tmp482,_tmp481,_tmp480,Cyc_Port_var());}));}))
return Cyc_Port_arith_ct();else{
# 1328
({void*_tmp485=_tmp1E8;Cyc_Port_leq(_tmp485,Cyc_Port_arith_ct());});
return _tmp1E4;}}else{
# 1332
({void*_tmp486=_tmp1E4;Cyc_Port_leq(_tmp486,Cyc_Port_arith_ct());});
({void*_tmp487=_tmp1E4;Cyc_Port_leq(_tmp487,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();}}case Cyc_Absyn_Times: _LL5: _LL6:
# 1337
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8: goto _LLA;case Cyc_Absyn_Mod: _LL9: _LLA: goto _LLC;case Cyc_Absyn_Eq: _LLB: _LLC: goto _LLE;case Cyc_Absyn_Neq: _LLD: _LLE: goto _LL10;case Cyc_Absyn_Lt: _LLF: _LL10: goto _LL12;case Cyc_Absyn_Gt: _LL11: _LL12: goto _LL14;case Cyc_Absyn_Lte: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Gte: _LL15: _LL16: goto _LL18;case Cyc_Absyn_Bitand: _LL17: _LL18: goto _LL1A;case Cyc_Absyn_Bitor: _LL19: _LL1A: goto _LL1C;case Cyc_Absyn_Bitxor: _LL1B: _LL1C: goto _LL1E;case Cyc_Absyn_Bitlshift: _LL1D: _LL1E: goto _LL20;case Cyc_Absyn_Bitlrshift: _LL1F: _LL20:
 goto _LL22;case Cyc_Absyn_Bitarshift: _LL21: _LL22:
({void*_tmp488=_tmp1E4;Cyc_Port_leq(_tmp488,Cyc_Port_arith_ct());});
({void*_tmp489=(void*)((struct Cyc_List_List*)_check_null(_tmp1E5))->hd;Cyc_Port_leq(_tmp489,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case Cyc_Absyn_Not: _LL23: _LL24:
 goto _LL26;case Cyc_Absyn_Bitnot: _LL25: _LL26:
# 1345
({void*_tmp48A=_tmp1E4;Cyc_Port_leq(_tmp48A,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();default: _LL27: _tmp1EB=_tmp1E6;_LL28:
({void*_tmp1E9=0U;({struct _dyneither_ptr _tmp48B=({const char*_tmp1EA=".size primop";_tag_dyneither(_tmp1EA,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp48B,_tag_dyneither(_tmp1E9,sizeof(void*),0U));});});}_LL0:;}
# 1352
static struct _tuple11 Cyc_Port_gen_lhs(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e,int takeaddress){
void*_tmp1EC=e->r;void*_tmp1ED=_tmp1EC;struct Cyc_Absyn_Exp*_tmp20F;struct _dyneither_ptr*_tmp20E;struct Cyc_Absyn_Exp*_tmp20D;struct _dyneither_ptr*_tmp20C;struct Cyc_Absyn_Exp*_tmp20B;struct Cyc_Absyn_Exp*_tmp20A;struct Cyc_Absyn_Exp*_tmp209;void*_tmp208;switch(*((int*)_tmp1ED)){case 1U: _LL1: _tmp208=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1ED)->f1;_LL2: {
# 1355
struct _tuple0*_tmp1EE=Cyc_Absyn_binding2qvar(_tmp208);
(*_tmp1EE).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp1EF=Cyc_Port_lookup_var(env,_tmp1EE);struct _tuple13 _tmp1F0=_tmp1EF;struct _tuple11 _tmp1F2;unsigned int _tmp1F1;_LLE: _tmp1F2=_tmp1F0.f1;_tmp1F1=_tmp1F0.f2;_LLF:;
Cyc_Port_register_localvar_usage(env,_tmp1F1,e->loc,takeaddress);
return _tmp1F2;};}case 23U: _LL3: _tmp20A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1ED)->f1;_tmp209=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1ED)->f2;_LL4: {
# 1361
void*_tmp1F3=Cyc_Port_var();
void*_tmp1F4=Cyc_Port_var();
void*_tmp1F5=Cyc_Port_gen_exp(0,env,_tmp20A);
({void*_tmp48C=Cyc_Port_gen_exp(0,env,_tmp209);Cyc_Port_leq(_tmp48C,Cyc_Port_arith_ct());});{
void*_tmp1F6=({void*_tmp490=_tmp1F3;void*_tmp48F=_tmp1F4;void*_tmp48E=Cyc_Port_fat_ct();void*_tmp48D=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp490,_tmp48F,_tmp48E,_tmp48D,Cyc_Port_var());});
Cyc_Port_leq(_tmp1F5,_tmp1F6);
return({struct _tuple11 _tmp3A7;_tmp3A7.f1=_tmp1F4,_tmp3A7.f2=_tmp1F3;_tmp3A7;});};}case 20U: _LL5: _tmp20B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1ED)->f1;_LL6: {
# 1369
void*_tmp1F7=Cyc_Port_var();
void*_tmp1F8=Cyc_Port_var();
void*_tmp1F9=({void*_tmp494=_tmp1F7;void*_tmp493=_tmp1F8;void*_tmp492=Cyc_Port_var();void*_tmp491=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp494,_tmp493,_tmp492,_tmp491,Cyc_Port_var());});
({void*_tmp495=Cyc_Port_gen_exp(0,env,_tmp20B);Cyc_Port_leq(_tmp495,_tmp1F9);});
return({struct _tuple11 _tmp3A8;_tmp3A8.f1=_tmp1F8,_tmp3A8.f2=_tmp1F7;_tmp3A8;});}case 21U: _LL7: _tmp20D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1ED)->f1;_tmp20C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1ED)->f2;_LL8: {
# 1375
void*_tmp1FA=Cyc_Port_var();
void*_tmp1FB=Cyc_Port_var();
void*_tmp1FC=Cyc_Port_gen_exp(takeaddress,env,_tmp20D);
({void*_tmp497=_tmp1FC;Cyc_Port_leq(_tmp497,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp1FD[1U];({struct Cyc_Port_Cfield*_tmp496=({struct Cyc_Port_Cfield*_tmp1FE=_cycalloc(sizeof(*_tmp1FE));_tmp1FE->qual=_tmp1FB,_tmp1FE->f=_tmp20C,_tmp1FE->type=_tmp1FA;_tmp1FE;});_tmp1FD[0]=_tmp496;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1FD,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return({struct _tuple11 _tmp3A9;_tmp3A9.f1=_tmp1FB,_tmp3A9.f2=_tmp1FA;_tmp3A9;});}case 22U: _LL9: _tmp20F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1ED)->f1;_tmp20E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1ED)->f2;_LLA: {
# 1381
void*_tmp1FF=Cyc_Port_var();
void*_tmp200=Cyc_Port_var();
void*_tmp201=Cyc_Port_gen_exp(0,env,_tmp20F);
({void*_tmp49D=_tmp201;Cyc_Port_leq(_tmp49D,({void*_tmp49C=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp202[1U];({struct Cyc_Port_Cfield*_tmp498=({struct Cyc_Port_Cfield*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->qual=_tmp200,_tmp203->f=_tmp20E,_tmp203->type=_tmp1FF;_tmp203;});_tmp202[0]=_tmp498;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp202,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp49B=
Cyc_Port_var();
# 1384
void*_tmp49A=
Cyc_Port_var();
# 1384
void*_tmp499=
Cyc_Port_var();
# 1384
Cyc_Port_ptr_ct(_tmp49C,_tmp49B,_tmp49A,_tmp499,
Cyc_Port_var());}));});
return({struct _tuple11 _tmp3AA;_tmp3AA.f1=_tmp200,_tmp3AA.f2=_tmp1FF;_tmp3AA;});}default: _LLB: _LLC:
({void*_tmp204=0U;({struct _dyneither_ptr _tmp4A0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp207=({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0U,({struct _dyneither_ptr _tmp49E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp3AB.f1=_tmp49E;});_tmp3AB;});void*_tmp205[1U];_tmp205[0]=& _tmp207;({struct _dyneither_ptr _tmp49F=({const char*_tmp206="gen_lhs: %s";_tag_dyneither(_tmp206,sizeof(char),12U);});Cyc_aprintf(_tmp49F,_tag_dyneither(_tmp205,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4A0,_tag_dyneither(_tmp204,sizeof(void*),0U));});});}_LL0:;}
# 1392
static void*Cyc_Port_gen_exp_false(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
return Cyc_Port_gen_exp(0,env,e);}
# 1395
static void*Cyc_Port_gen_exp(int takeaddress,struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Exp*e){
void*_tmp210=e->r;void*_tmp211=_tmp210;struct Cyc_Absyn_Stmt*_tmp286;struct Cyc_Absyn_Exp*_tmp285;struct Cyc_Absyn_Exp*_tmp284;void**_tmp283;struct Cyc_Absyn_Exp*_tmp282;struct Cyc_Absyn_Exp*_tmp281;struct _dyneither_ptr*_tmp280;struct Cyc_Absyn_Exp*_tmp27F;struct _dyneither_ptr*_tmp27E;struct Cyc_Absyn_Exp*_tmp27D;struct Cyc_Absyn_Exp*_tmp27C;struct Cyc_Absyn_Exp*_tmp27B;struct Cyc_Absyn_Exp*_tmp27A;void*_tmp279;struct Cyc_Absyn_Exp*_tmp278;struct Cyc_Absyn_Exp*_tmp277;struct Cyc_Absyn_Exp*_tmp276;struct Cyc_Absyn_Exp*_tmp275;struct Cyc_List_List*_tmp274;struct Cyc_Absyn_Exp*_tmp273;struct Cyc_Absyn_Exp*_tmp272;struct Cyc_Absyn_Exp*_tmp271;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Exp*_tmp26F;struct Cyc_Absyn_Exp*_tmp26E;struct Cyc_Absyn_Exp*_tmp26D;struct Cyc_Absyn_Exp*_tmp26C;struct Cyc_Absyn_Exp*_tmp26B;struct Cyc_Absyn_Exp*_tmp26A;enum Cyc_Absyn_Incrementor _tmp269;struct Cyc_Absyn_Exp*_tmp268;struct Cyc_Core_Opt*_tmp267;struct Cyc_Absyn_Exp*_tmp266;enum Cyc_Absyn_Primop _tmp265;struct Cyc_List_List*_tmp264;void*_tmp263;switch(*((int*)_tmp211)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp211)->f1).Wstring_c).tag){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
 goto _LL6;case 4U: _LL5: _LL6:
 goto _LL8;case 6U: _LL7: _LL8:
 goto _LLA;case 7U: _LL13: _LL14:
# 1406
 return Cyc_Port_arith_ct();case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp211)->f1).Int_c).val).f2 == 0){_LL15: _LL16:
 return Cyc_Port_zero_ct();}else{_LL17: _LL18:
 return Cyc_Port_arith_ct();}case 8U: _LL19: _LL1A:
# 1410
 return({void*_tmp4A4=Cyc_Port_arith_ct();void*_tmp4A3=Cyc_Port_const_ct();void*_tmp4A2=Cyc_Port_fat_ct();void*_tmp4A1=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4A4,_tmp4A3,_tmp4A2,_tmp4A1,Cyc_Port_zterm_ct());});case 9U: _LL1B: _LL1C:
# 1412
 return({void*_tmp4A8=Cyc_Port_arith_ct();void*_tmp4A7=Cyc_Port_const_ct();void*_tmp4A6=Cyc_Port_fat_ct();void*_tmp4A5=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4A8,_tmp4A7,_tmp4A6,_tmp4A5,Cyc_Port_zterm_ct());});default: _LL1D: _LL1E:
 return Cyc_Port_zero_ct();}case 17U: _LL9: _LLA:
# 1401
 goto _LLC;case 18U: _LLB: _LLC:
 goto _LLE;case 19U: _LLD: _LLE:
 goto _LL10;case 32U: _LLF: _LL10:
 goto _LL12;case 33U: _LL11: _LL12:
 goto _LL14;case 1U: _LL1F: _tmp263=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL20: {
# 1415
struct _tuple0*_tmp212=Cyc_Absyn_binding2qvar(_tmp263);
(*_tmp212).f1=Cyc_Absyn_Loc_n;{
struct _tuple13 _tmp213=Cyc_Port_lookup_var(env,_tmp212);struct _tuple13 _tmp214=_tmp213;struct _tuple11 _tmp21D;unsigned int _tmp21C;_LL68: _tmp21D=_tmp214.f1;_tmp21C=_tmp214.f2;_LL69:;{
struct _tuple11 _tmp215=_tmp21D;void*_tmp21B;_LL6B: _tmp21B=_tmp215.f2;_LL6C:;
if(Cyc_Port_isfn(env,_tmp212)){
({void*_tmp4A9=Cyc_Port_instantiate(_tmp21B);_tmp21B=_tmp4A9;});
return({void*_tmp4AD=_tmp21B;void*_tmp4AC=Cyc_Port_var();void*_tmp4AB=Cyc_Port_var();void*_tmp4AA=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4AD,_tmp4AC,_tmp4AB,_tmp4AA,Cyc_Port_nozterm_ct());});}else{
# 1423
if(Cyc_Port_isarray(env,_tmp212)){
void*_tmp216=Cyc_Port_var();
void*_tmp217=Cyc_Port_var();
void*_tmp218=Cyc_Port_var();
void*_tmp219=Cyc_Port_array_ct(_tmp216,_tmp217,_tmp218);
Cyc_Port_unifies(_tmp21B,_tmp219);{
void*_tmp21A=({void*_tmp4B1=_tmp216;void*_tmp4B0=_tmp217;void*_tmp4AF=Cyc_Port_fat_ct();void*_tmp4AE=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4B1,_tmp4B0,_tmp4AF,_tmp4AE,_tmp218);});
return _tmp21A;};}else{
# 1432
Cyc_Port_register_localvar_usage(env,_tmp21C,e->loc,takeaddress);
return _tmp21B;}}};};}case 3U: _LL21: _tmp265=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp264=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL22:
# 1435
 return({struct Cyc_Port_Cenv*_tmp4B3=env;enum Cyc_Absyn_Primop _tmp4B2=_tmp265;Cyc_Port_gen_primop(_tmp4B3,_tmp4B2,((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp264));});case 4U: _LL23: _tmp268=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp267=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_tmp266=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp211)->f3;_LL24: {
# 1437
struct _tuple11 _tmp21E=Cyc_Port_gen_lhs(env,_tmp268,0);struct _tuple11 _tmp21F=_tmp21E;void*_tmp223;void*_tmp222;_LL6E: _tmp223=_tmp21F.f1;_tmp222=_tmp21F.f2;_LL6F:;
({void*_tmp4B4=_tmp223;Cyc_Port_unifies(_tmp4B4,Cyc_Port_notconst_ct());});{
void*_tmp220=Cyc_Port_gen_exp(0,env,_tmp266);
if((unsigned int)_tmp267){
struct Cyc_List_List x2=({struct Cyc_List_List _tmp3AD;_tmp3AD.hd=_tmp220,_tmp3AD.tl=0;_tmp3AD;});
struct Cyc_List_List x1=({struct Cyc_List_List _tmp3AC;_tmp3AC.hd=_tmp222,_tmp3AC.tl=& x2;_tmp3AC;});
void*_tmp221=Cyc_Port_gen_primop(env,(enum Cyc_Absyn_Primop)_tmp267->v,& x1);
Cyc_Port_leq(_tmp221,_tmp222);
return _tmp222;}else{
# 1447
Cyc_Port_leq(_tmp220,_tmp222);
return _tmp222;}};}case 5U: _LL25: _tmp26A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp269=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL26: {
# 1451
struct _tuple11 _tmp224=Cyc_Port_gen_lhs(env,_tmp26A,0);struct _tuple11 _tmp225=_tmp224;void*_tmp227;void*_tmp226;_LL71: _tmp227=_tmp225.f1;_tmp226=_tmp225.f2;_LL72:;
({void*_tmp4B5=_tmp227;Cyc_Port_unifies(_tmp4B5,Cyc_Port_notconst_ct());});
# 1454
({void*_tmp4BA=_tmp226;Cyc_Port_leq(_tmp4BA,({void*_tmp4B9=Cyc_Port_var();void*_tmp4B8=Cyc_Port_var();void*_tmp4B7=Cyc_Port_fat_ct();void*_tmp4B6=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4B9,_tmp4B8,_tmp4B7,_tmp4B6,Cyc_Port_var());}));});
({void*_tmp4BB=_tmp226;Cyc_Port_leq(_tmp4BB,Cyc_Port_arith_ct());});
return _tmp226;}case 6U: _LL27: _tmp26D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp26C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_tmp26B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp211)->f3;_LL28: {
# 1458
void*_tmp228=Cyc_Port_var();
({void*_tmp4BC=Cyc_Port_gen_exp(0,env,_tmp26D);Cyc_Port_leq(_tmp4BC,Cyc_Port_arith_ct());});
({void*_tmp4BD=Cyc_Port_gen_exp(0,env,_tmp26C);Cyc_Port_leq(_tmp4BD,_tmp228);});
({void*_tmp4BE=Cyc_Port_gen_exp(0,env,_tmp26B);Cyc_Port_leq(_tmp4BE,_tmp228);});
return _tmp228;}case 7U: _LL29: _tmp26F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp26E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL2A:
 _tmp271=_tmp26F;_tmp270=_tmp26E;goto _LL2C;case 8U: _LL2B: _tmp271=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp270=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL2C:
# 1465
({void*_tmp4BF=Cyc_Port_gen_exp(0,env,_tmp271);Cyc_Port_leq(_tmp4BF,Cyc_Port_arith_ct());});
({void*_tmp4C0=Cyc_Port_gen_exp(0,env,_tmp270);Cyc_Port_leq(_tmp4C0,Cyc_Port_arith_ct());});
return Cyc_Port_arith_ct();case 9U: _LL2D: _tmp273=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp272=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL2E:
# 1469
 Cyc_Port_gen_exp(0,env,_tmp273);
return Cyc_Port_gen_exp(0,env,_tmp272);case 10U: _LL2F: _tmp275=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp274=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL30: {
# 1472
void*_tmp229=Cyc_Port_var();
void*_tmp22A=Cyc_Port_gen_exp(0,env,_tmp275);
struct Cyc_List_List*_tmp22B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Port_Cenv*,struct Cyc_Absyn_Exp*),struct Cyc_Port_Cenv*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Port_gen_exp_false,env,_tmp274);
struct Cyc_List_List*_tmp22C=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(void*x))Cyc_Port_new_var,_tmp22B);
({void*_tmp4C5=_tmp22A;Cyc_Port_unifies(_tmp4C5,({void*_tmp4C4=Cyc_Port_fn_ct(_tmp229,_tmp22C);void*_tmp4C3=Cyc_Port_var();void*_tmp4C2=Cyc_Port_var();void*_tmp4C1=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4C4,_tmp4C3,_tmp4C2,_tmp4C1,Cyc_Port_nozterm_ct());}));});
for(0;_tmp22B != 0;(_tmp22B=_tmp22B->tl,_tmp22C=_tmp22C->tl)){
Cyc_Port_leq((void*)_tmp22B->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp22C))->hd);}
# 1480
return _tmp229;}case 11U: _LL31: _LL32:
({void*_tmp22D=0U;({struct _dyneither_ptr _tmp4C6=({const char*_tmp22E="throw";_tag_dyneither(_tmp22E,sizeof(char),6U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C6,_tag_dyneither(_tmp22D,sizeof(void*),0U));});});case 41U: _LL33: _tmp276=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL34:
 return Cyc_Port_gen_exp(0,env,_tmp276);case 12U: _LL35: _tmp277=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL36:
 return Cyc_Port_gen_exp(0,env,_tmp277);case 13U: _LL37: _LL38:
({void*_tmp22F=0U;({struct _dyneither_ptr _tmp4C7=({const char*_tmp230="instantiate";_tag_dyneither(_tmp230,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4C7,_tag_dyneither(_tmp22F,sizeof(void*),0U));});});case 14U: _LL39: _tmp279=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp278=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL3A: {
# 1486
void*_tmp231=Cyc_Port_type_to_ctype(env,_tmp279);
void*_tmp232=Cyc_Port_gen_exp(0,env,_tmp278);
Cyc_Port_leq(_tmp231,_tmp232);
return _tmp232;}case 15U: _LL3B: _tmp27A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL3C: {
# 1491
struct _tuple11 _tmp233=Cyc_Port_gen_lhs(env,_tmp27A,1);struct _tuple11 _tmp234=_tmp233;void*_tmp236;void*_tmp235;_LL74: _tmp236=_tmp234.f1;_tmp235=_tmp234.f2;_LL75:;
return({void*_tmp4CB=_tmp235;void*_tmp4CA=_tmp236;void*_tmp4C9=Cyc_Port_var();void*_tmp4C8=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4CB,_tmp4CA,_tmp4C9,_tmp4C8,Cyc_Port_var());});}case 23U: _LL3D: _tmp27C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp27B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL3E: {
# 1494
void*_tmp237=Cyc_Port_var();
({void*_tmp4CC=Cyc_Port_gen_exp(0,env,_tmp27B);Cyc_Port_leq(_tmp4CC,Cyc_Port_arith_ct());});{
void*_tmp238=Cyc_Port_gen_exp(0,env,_tmp27C);
({void*_tmp4D1=_tmp238;Cyc_Port_leq(_tmp4D1,({void*_tmp4D0=_tmp237;void*_tmp4CF=Cyc_Port_var();void*_tmp4CE=Cyc_Port_fat_ct();void*_tmp4CD=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4D0,_tmp4CF,_tmp4CE,_tmp4CD,Cyc_Port_var());}));});
return _tmp237;};}case 20U: _LL3F: _tmp27D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL40: {
# 1500
void*_tmp239=Cyc_Port_var();
({void*_tmp4D6=Cyc_Port_gen_exp(0,env,_tmp27D);Cyc_Port_leq(_tmp4D6,({void*_tmp4D5=_tmp239;void*_tmp4D4=Cyc_Port_var();void*_tmp4D3=Cyc_Port_var();void*_tmp4D2=Cyc_Port_var();Cyc_Port_ptr_ct(_tmp4D5,_tmp4D4,_tmp4D3,_tmp4D2,Cyc_Port_var());}));});
return _tmp239;}case 21U: _LL41: _tmp27F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp27E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL42: {
# 1504
void*_tmp23A=Cyc_Port_var();
void*_tmp23B=Cyc_Port_gen_exp(takeaddress,env,_tmp27F);
({void*_tmp4D9=_tmp23B;Cyc_Port_leq(_tmp4D9,Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp23C[1U];({struct Cyc_Port_Cfield*_tmp4D8=({struct Cyc_Port_Cfield*_tmp23D=_cycalloc(sizeof(*_tmp23D));({void*_tmp4D7=Cyc_Port_var();_tmp23D->qual=_tmp4D7;}),_tmp23D->f=_tmp27E,_tmp23D->type=_tmp23A;_tmp23D;});_tmp23C[0]=_tmp4D8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp23C,sizeof(struct Cyc_Port_Cfield*),1U));})));});
return _tmp23A;}case 22U: _LL43: _tmp281=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp280=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL44: {
# 1509
void*_tmp23E=Cyc_Port_var();
void*_tmp23F=Cyc_Port_gen_exp(0,env,_tmp281);
({void*_tmp4E0=Cyc_Port_gen_exp(0,env,_tmp281);Cyc_Port_leq(_tmp4E0,({void*_tmp4DF=Cyc_Port_unknown_aggr_ct(({struct Cyc_Port_Cfield*_tmp240[1U];({struct Cyc_Port_Cfield*_tmp4DB=({struct Cyc_Port_Cfield*_tmp241=_cycalloc(sizeof(*_tmp241));({void*_tmp4DA=Cyc_Port_var();_tmp241->qual=_tmp4DA;}),_tmp241->f=_tmp280,_tmp241->type=_tmp23E;_tmp241;});_tmp240[0]=_tmp4DB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp240,sizeof(struct Cyc_Port_Cfield*),1U));}));void*_tmp4DE=
Cyc_Port_var();
# 1511
void*_tmp4DD=
Cyc_Port_var();
# 1511
void*_tmp4DC=
Cyc_Port_var();
# 1511
Cyc_Port_ptr_ct(_tmp4DF,_tmp4DE,_tmp4DD,_tmp4DC,
Cyc_Port_var());}));});
return _tmp23E;}case 34U: _LL45: _tmp283=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp211)->f1).elt_type;_tmp282=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp211)->f1).num_elts;_LL46:
# 1515
({void*_tmp4E1=Cyc_Port_gen_exp(0,env,_tmp282);Cyc_Port_leq(_tmp4E1,Cyc_Port_arith_ct());});{
void*_tmp242=(unsigned int)_tmp283?Cyc_Port_type_to_ctype(env,*_tmp283): Cyc_Port_var();
return({void*_tmp4E5=_tmp242;void*_tmp4E4=Cyc_Port_var();void*_tmp4E3=Cyc_Port_fat_ct();void*_tmp4E2=Cyc_Port_heap_ct();Cyc_Port_ptr_ct(_tmp4E5,_tmp4E4,_tmp4E3,_tmp4E2,Cyc_Port_var());});};case 2U: _LL47: _LL48:
({void*_tmp243=0U;({struct _dyneither_ptr _tmp4E6=({const char*_tmp244="Pragma_e";_tag_dyneither(_tmp244,sizeof(char),9U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4E6,_tag_dyneither(_tmp243,sizeof(void*),0U));});});case 35U: _LL49: _tmp285=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_tmp284=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp211)->f2;_LL4A:
({void*_tmp245=0U;({struct _dyneither_ptr _tmp4E7=({const char*_tmp246="Swap_e";_tag_dyneither(_tmp246,sizeof(char),7U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4E7,_tag_dyneither(_tmp245,sizeof(void*),0U));});});case 16U: _LL4B: _LL4C:
({void*_tmp247=0U;({struct _dyneither_ptr _tmp4E8=({const char*_tmp248="New_e";_tag_dyneither(_tmp248,sizeof(char),6U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4E8,_tag_dyneither(_tmp247,sizeof(void*),0U));});});case 24U: _LL4D: _LL4E:
({void*_tmp249=0U;({struct _dyneither_ptr _tmp4E9=({const char*_tmp24A="Tuple_e";_tag_dyneither(_tmp24A,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4E9,_tag_dyneither(_tmp249,sizeof(void*),0U));});});case 25U: _LL4F: _LL50:
({void*_tmp24B=0U;({struct _dyneither_ptr _tmp4EA=({const char*_tmp24C="CompoundLit_e";_tag_dyneither(_tmp24C,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EA,_tag_dyneither(_tmp24B,sizeof(void*),0U));});});case 26U: _LL51: _LL52:
({void*_tmp24D=0U;({struct _dyneither_ptr _tmp4EB=({const char*_tmp24E="Array_e";_tag_dyneither(_tmp24E,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EB,_tag_dyneither(_tmp24D,sizeof(void*),0U));});});case 27U: _LL53: _LL54:
({void*_tmp24F=0U;({struct _dyneither_ptr _tmp4EC=({const char*_tmp250="Comprehension_e";_tag_dyneither(_tmp250,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EC,_tag_dyneither(_tmp24F,sizeof(void*),0U));});});case 28U: _LL55: _LL56:
({void*_tmp251=0U;({struct _dyneither_ptr _tmp4ED=({const char*_tmp252="ComprehensionNoinit_e";_tag_dyneither(_tmp252,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4ED,_tag_dyneither(_tmp251,sizeof(void*),0U));});});case 29U: _LL57: _LL58:
({void*_tmp253=0U;({struct _dyneither_ptr _tmp4EE=({const char*_tmp254="Aggregate_e";_tag_dyneither(_tmp254,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EE,_tag_dyneither(_tmp253,sizeof(void*),0U));});});case 30U: _LL59: _LL5A:
({void*_tmp255=0U;({struct _dyneither_ptr _tmp4EF=({const char*_tmp256="AnonStruct_e";_tag_dyneither(_tmp256,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EF,_tag_dyneither(_tmp255,sizeof(void*),0U));});});case 31U: _LL5B: _LL5C:
({void*_tmp257=0U;({struct _dyneither_ptr _tmp4F0=({const char*_tmp258="Datatype_e";_tag_dyneither(_tmp258,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F0,_tag_dyneither(_tmp257,sizeof(void*),0U));});});case 36U: _LL5D: _LL5E:
({void*_tmp259=0U;({struct _dyneither_ptr _tmp4F1=({const char*_tmp25A="UnresolvedMem_e";_tag_dyneither(_tmp25A,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F1,_tag_dyneither(_tmp259,sizeof(void*),0U));});});case 37U: _LL5F: _tmp286=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp211)->f1;_LL60:
({void*_tmp25B=0U;({struct _dyneither_ptr _tmp4F2=({const char*_tmp25C="StmtExp_e";_tag_dyneither(_tmp25C,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F2,_tag_dyneither(_tmp25B,sizeof(void*),0U));});});case 39U: _LL61: _LL62:
({void*_tmp25D=0U;({struct _dyneither_ptr _tmp4F3=({const char*_tmp25E="Valueof_e";_tag_dyneither(_tmp25E,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F3,_tag_dyneither(_tmp25D,sizeof(void*),0U));});});case 40U: _LL63: _LL64:
({void*_tmp25F=0U;({struct _dyneither_ptr _tmp4F4=({const char*_tmp260="Asm_e";_tag_dyneither(_tmp260,sizeof(char),6U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F4,_tag_dyneither(_tmp25F,sizeof(void*),0U));});});default: _LL65: _LL66:
({void*_tmp261=0U;({struct _dyneither_ptr _tmp4F5=({const char*_tmp262="Tagcheck_e";_tag_dyneither(_tmp262,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F5,_tag_dyneither(_tmp261,sizeof(void*),0U));});});}_LL0:;}
# 1538
static void Cyc_Port_gen_stmt(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Stmt*s){
void*_tmp287=s->r;void*_tmp288=_tmp287;struct Cyc_Absyn_Stmt*_tmp2A2;struct Cyc_Absyn_Exp*_tmp2A1;struct Cyc_Absyn_Stmt*_tmp2A0;struct Cyc_Absyn_Decl*_tmp29F;struct Cyc_Absyn_Stmt*_tmp29E;struct Cyc_Absyn_Exp*_tmp29D;struct Cyc_List_List*_tmp29C;struct Cyc_Absyn_Exp*_tmp29B;struct Cyc_Absyn_Exp*_tmp29A;struct Cyc_Absyn_Exp*_tmp299;struct Cyc_Absyn_Stmt*_tmp298;struct Cyc_Absyn_Exp*_tmp297;struct Cyc_Absyn_Stmt*_tmp296;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Stmt*_tmp294;struct Cyc_Absyn_Stmt*_tmp293;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Stmt*_tmp291;struct Cyc_Absyn_Stmt*_tmp290;struct Cyc_Absyn_Exp*_tmp28F;switch(*((int*)_tmp288)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmp28F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_LL4:
 Cyc_Port_gen_exp(0,env,_tmp28F);return;case 2U: _LL5: _tmp291=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_tmp290=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL6:
 Cyc_Port_gen_stmt(env,_tmp291);Cyc_Port_gen_stmt(env,_tmp290);return;case 3U: _LL7: _tmp292=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_LL8: {
# 1544
void*_tmp289=Cyc_Port_lookup_return_type(env);
void*_tmp28A=(unsigned int)_tmp292?Cyc_Port_gen_exp(0,env,_tmp292): Cyc_Port_void_ct();
Cyc_Port_leq(_tmp28A,_tmp289);
return;}case 4U: _LL9: _tmp295=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_tmp294=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_tmp293=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp288)->f3;_LLA:
# 1549
({void*_tmp4F6=Cyc_Port_gen_exp(0,env,_tmp295);Cyc_Port_leq(_tmp4F6,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp294);
Cyc_Port_gen_stmt(env,_tmp293);
return;case 5U: _LLB: _tmp297=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp288)->f1).f1;_tmp296=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LLC:
# 1554
({void*_tmp4F7=Cyc_Port_gen_exp(0,env,_tmp297);Cyc_Port_leq(_tmp4F7,Cyc_Port_arith_ct());});
Cyc_Port_gen_stmt(env,_tmp296);
return;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 8U: _LL11: _LL12:
 return;case 9U: _LL13: _tmp29B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_tmp29A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp288)->f2).f1;_tmp299=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp288)->f3).f1;_tmp298=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp288)->f4;_LL14:
# 1561
 Cyc_Port_gen_exp(0,env,_tmp29B);
({void*_tmp4F8=Cyc_Port_gen_exp(0,env,_tmp29A);Cyc_Port_leq(_tmp4F8,Cyc_Port_arith_ct());});
Cyc_Port_gen_exp(0,env,_tmp299);
Cyc_Port_gen_stmt(env,_tmp298);
return;case 10U: _LL15: _tmp29D=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_tmp29C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL16:
# 1567
({void*_tmp4F9=Cyc_Port_gen_exp(0,env,_tmp29D);Cyc_Port_leq(_tmp4F9,Cyc_Port_arith_ct());});
for(0;(unsigned int)_tmp29C;_tmp29C=_tmp29C->tl){
# 1570
Cyc_Port_gen_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp29C->hd)->body);}
# 1572
return;case 11U: _LL17: _LL18:
({void*_tmp28B=0U;({struct _dyneither_ptr _tmp4FA=({const char*_tmp28C="fallthru";_tag_dyneither(_tmp28C,sizeof(char),9U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FA,_tag_dyneither(_tmp28B,sizeof(void*),0U));});});case 12U: _LL19: _tmp29F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_tmp29E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL1A:
# 1575
({struct Cyc_Port_Cenv*_tmp4FB=Cyc_Port_gen_localdecl(env,_tmp29F,0);env=_tmp4FB;});Cyc_Port_gen_stmt(env,_tmp29E);return;case 13U: _LL1B: _tmp2A0=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp288)->f2;_LL1C:
 Cyc_Port_gen_stmt(env,_tmp2A0);return;case 14U: _LL1D: _tmp2A2=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp288)->f1;_tmp2A1=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp288)->f2).f1;_LL1E:
# 1578
 Cyc_Port_gen_stmt(env,_tmp2A2);
({void*_tmp4FC=Cyc_Port_gen_exp(0,env,_tmp2A1);Cyc_Port_leq(_tmp4FC,Cyc_Port_arith_ct());});
return;default: _LL1F: _LL20:
({void*_tmp28D=0U;({struct _dyneither_ptr _tmp4FD=({const char*_tmp28E="try/catch";_tag_dyneither(_tmp28E,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FD,_tag_dyneither(_tmp28D,sizeof(void*),0U));});});}_LL0:;}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1586
static void*Cyc_Port_gen_initializer(struct Cyc_Port_Cenv*env,void*t,struct Cyc_Absyn_Exp*e){
void*_tmp2A3=e->r;void*_tmp2A4=_tmp2A3;struct Cyc_List_List*_tmp2CB;struct Cyc_List_List*_tmp2CA;struct Cyc_List_List*_tmp2C9;switch(*((int*)_tmp2A4)){case 36U: _LL1: _tmp2C9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp2A4)->f2;_LL2:
 _tmp2CA=_tmp2C9;goto _LL4;case 26U: _LL3: _tmp2CA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp2A4)->f1;_LL4:
 _tmp2CB=_tmp2CA;goto _LL6;case 25U: _LL5: _tmp2CB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp2A4)->f2;_LL6:
# 1591
 LOOP: {
void*_tmp2A5=t;struct Cyc_Absyn_Aggrdecl*_tmp2C6;struct _tuple0*_tmp2C5;void*_tmp2C4;void*_tmp2C3;unsigned int _tmp2C2;struct _tuple0*_tmp2C1;switch(*((int*)_tmp2A5)){case 8U: _LLE: _tmp2C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2A5)->f1;_LLF:
# 1594
(*_tmp2C1).f1=Cyc_Absyn_Loc_n;
({void*_tmp4FE=Cyc_Port_lookup_typedef(env,_tmp2C1);t=_tmp4FE;});goto LOOP;case 4U: _LL10: _tmp2C4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A5)->f1).elt_type;_tmp2C3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A5)->f1).zero_term;_tmp2C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A5)->f1).zt_loc;_LL11: {
# 1597
void*_tmp2A6=Cyc_Port_var();
void*_tmp2A7=Cyc_Port_var();
void*_tmp2A8=Cyc_Port_var();
void*_tmp2A9=Cyc_Port_type_to_ctype(env,_tmp2C4);
for(0;_tmp2CB != 0;_tmp2CB=_tmp2CB->tl){
struct _tuple17 _tmp2AA=*((struct _tuple17*)_tmp2CB->hd);struct _tuple17 _tmp2AB=_tmp2AA;struct Cyc_List_List*_tmp2B0;struct Cyc_Absyn_Exp*_tmp2AF;_LL19: _tmp2B0=_tmp2AB.f1;_tmp2AF=_tmp2AB.f2;_LL1A:;
if((unsigned int)_tmp2B0)({void*_tmp2AC=0U;({struct _dyneither_ptr _tmp4FF=({const char*_tmp2AD="designators in initializers";_tag_dyneither(_tmp2AD,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4FF,_tag_dyneither(_tmp2AC,sizeof(void*),0U));});});{
void*_tmp2AE=Cyc_Port_gen_initializer(env,_tmp2C4,_tmp2AF);
Cyc_Port_leq(_tmp2AE,_tmp2A6);};}
# 1607
return Cyc_Port_array_ct(_tmp2A6,_tmp2A7,_tmp2A8);}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)->f1).UnknownAggr).tag == 1){if((((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)->f1).UnknownAggr).val).f1 == Cyc_Absyn_StructA){_LL12: _tmp2C5=(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)->f1).UnknownAggr).val).f2;_LL13:
# 1609
(*_tmp2C5).f1=Cyc_Absyn_Loc_n;{
struct _tuple10 _tmp2B1=*Cyc_Port_lookup_struct_decl(env,_tmp2C5);struct _tuple10 _tmp2B2=_tmp2B1;struct Cyc_Absyn_Aggrdecl*_tmp2B5;_LL1C: _tmp2B5=_tmp2B2.f1;_LL1D:;
if(_tmp2B5 == 0)({void*_tmp2B3=0U;({struct _dyneither_ptr _tmp500=({const char*_tmp2B4="struct is not yet defined";_tag_dyneither(_tmp2B4,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp500,_tag_dyneither(_tmp2B3,sizeof(void*),0U));});});
_tmp2C6=_tmp2B5;goto _LL15;};}else{goto _LL16;}}else{_LL14: _tmp2C6=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A5)->f1)->f1).KnownAggr).val;_LL15: {
# 1614
struct Cyc_List_List*_tmp2B6=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2C6->impl))->fields;
for(0;_tmp2CB != 0;_tmp2CB=_tmp2CB->tl){
struct _tuple17 _tmp2B7=*((struct _tuple17*)_tmp2CB->hd);struct _tuple17 _tmp2B8=_tmp2B7;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Exp*_tmp2BD;_LL1F: _tmp2BE=_tmp2B8.f1;_tmp2BD=_tmp2B8.f2;_LL20:;
if((unsigned int)_tmp2BE)({void*_tmp2B9=0U;({struct _dyneither_ptr _tmp501=({const char*_tmp2BA="designators in initializers";_tag_dyneither(_tmp2BA,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp501,_tag_dyneither(_tmp2B9,sizeof(void*),0U));});});{
struct Cyc_Absyn_Aggrfield*_tmp2BB=(struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp2B6))->hd;
_tmp2B6=_tmp2B6->tl;{
void*_tmp2BC=Cyc_Port_gen_initializer(env,_tmp2BB->type,_tmp2BD);;};};}
# 1622
return Cyc_Port_type_to_ctype(env,t);}}}else{goto _LL16;}default: _LL16: _LL17:
({void*_tmp2BF=0U;({struct _dyneither_ptr _tmp502=({const char*_tmp2C0="bad type for aggregate initializer";_tag_dyneither(_tmp2C0,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp502,_tag_dyneither(_tmp2BF,sizeof(void*),0U));});});}_LLD:;}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2A4)->f1).Wstring_c).tag){case 8U: _LL7: _LL8:
# 1625
 goto _LLA;case 9U: _LL9: _LLA:
# 1627
 LOOP2: {
void*_tmp2C7=t;struct _tuple0*_tmp2C8;switch(*((int*)_tmp2C7)){case 8U: _LL22: _tmp2C8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2C7)->f1;_LL23:
# 1630
(*_tmp2C8).f1=Cyc_Absyn_Loc_n;
({void*_tmp503=Cyc_Port_lookup_typedef(env,_tmp2C8);t=_tmp503;});goto LOOP2;case 4U: _LL24: _LL25:
 return({void*_tmp505=Cyc_Port_arith_ct();void*_tmp504=Cyc_Port_const_ct();Cyc_Port_array_ct(_tmp505,_tmp504,Cyc_Port_zterm_ct());});default: _LL26: _LL27:
 return Cyc_Port_gen_exp(0,env,e);}_LL21:;}default: goto _LLB;}default: _LLB: _LLC:
# 1635
 return Cyc_Port_gen_exp(0,env,e);}_LL0:;}
# 1640
static struct Cyc_Port_Cenv*Cyc_Port_gen_localdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d,int fromglobal){
void*_tmp2CC=d->r;void*_tmp2CD=_tmp2CC;struct Cyc_Absyn_Vardecl*_tmp2D1;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CD)->tag == 0U){_LL1: _tmp2D1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2CD)->f1;_LL2:
# 1643
 if(!fromglobal)Cyc_Port_register_localvar_decl(env,_tmp2D1->name,_tmp2D1->varloc,_tmp2D1->type,_tmp2D1->initializer);{
void*_tmp2CE=Cyc_Port_var();
void*q;
if((env->gcenv)->porting){
({void*_tmp506=Cyc_Port_var();q=_tmp506;});
({struct Cyc_Port_Cenv*_tmp509=env;void*_tmp508=q;void*_tmp507=
(_tmp2D1->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1648
Cyc_Port_register_const_cvar(_tmp509,_tmp508,_tmp507,(_tmp2D1->tq).loc);});}else{
# 1657
({void*_tmp50A=(_tmp2D1->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();q=_tmp50A;});}
# 1659
(*_tmp2D1->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp50B=Cyc_Port_add_var(env,_tmp2D1->name,_tmp2D1->type,q,_tmp2CE,_tmp2D1->varloc);env=_tmp50B;});
({void*_tmp50C=_tmp2CE;Cyc_Port_unifies(_tmp50C,Cyc_Port_type_to_ctype(env,_tmp2D1->type));});
if((unsigned int)_tmp2D1->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp2D1->initializer);
void*t2=Cyc_Port_gen_initializer(env,_tmp2D1->type,e);
Cyc_Port_leq(t2,_tmp2CE);}
# 1667
return env;};}else{_LL3: _LL4:
({void*_tmp2CF=0U;({struct _dyneither_ptr _tmp50D=({const char*_tmp2D0="non-local decl that isn't a variable";_tag_dyneither(_tmp2D0,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp50D,_tag_dyneither(_tmp2CF,sizeof(void*),0U));});});}_LL0:;}
# 1672
static struct _tuple8*Cyc_Port_make_targ(struct _tuple8*arg){
struct _tuple8 _tmp2D2=*arg;struct _tuple8 _tmp2D3=_tmp2D2;struct _dyneither_ptr*_tmp2D7;struct Cyc_Absyn_Tqual _tmp2D6;void*_tmp2D5;_LL1: _tmp2D7=_tmp2D3.f1;_tmp2D6=_tmp2D3.f2;_tmp2D5=_tmp2D3.f3;_LL2:;
return({struct _tuple8*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->f1=0,_tmp2D4->f2=_tmp2D6,_tmp2D4->f3=_tmp2D5;_tmp2D4;});}
# 1677
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds);struct _tuple18{struct _dyneither_ptr*f1;void*f2;void*f3;void*f4;};
# 1679
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecl(struct Cyc_Port_Cenv*env,struct Cyc_Absyn_Decl*d){
void*_tmp2D8=d->r;void*_tmp2D9=_tmp2D8;struct Cyc_Absyn_Enumdecl*_tmp315;struct Cyc_Absyn_Aggrdecl*_tmp314;struct Cyc_Absyn_Typedefdecl*_tmp313;struct Cyc_Absyn_Fndecl*_tmp312;struct Cyc_Absyn_Vardecl*_tmp311;switch(*((int*)_tmp2D9)){case 13U: _LL1: _LL2:
# 1682
(env->gcenv)->porting=1;
return env;case 14U: _LL3: _LL4:
# 1685
(env->gcenv)->porting=0;
return env;case 0U: _LL5: _tmp311=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp2D9)->f1;_LL6:
# 1688
(*_tmp311->name).f1=Cyc_Absyn_Loc_n;
# 1692
if(Cyc_Port_declared_var(env,_tmp311->name)){
struct _tuple13 _tmp2DA=Cyc_Port_lookup_var(env,_tmp311->name);struct _tuple13 _tmp2DB=_tmp2DA;struct _tuple11 _tmp2E0;unsigned int _tmp2DF;_LL12: _tmp2E0=_tmp2DB.f1;_tmp2DF=_tmp2DB.f2;_LL13:;{
struct _tuple11 _tmp2DC=_tmp2E0;void*_tmp2DE;void*_tmp2DD;_LL15: _tmp2DE=_tmp2DC.f1;_tmp2DD=_tmp2DC.f2;_LL16:;{
void*q2;
if((env->gcenv)->porting  && !Cyc_Port_isfn(env,_tmp311->name)){
({void*_tmp50E=Cyc_Port_var();q2=_tmp50E;});
({struct Cyc_Port_Cenv*_tmp511=env;void*_tmp510=q2;void*_tmp50F=
(_tmp311->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1698
Cyc_Port_register_const_cvar(_tmp511,_tmp510,_tmp50F,(_tmp311->tq).loc);});}else{
# 1707
({void*_tmp512=(_tmp311->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();q2=_tmp512;});}
# 1709
Cyc_Port_unifies(_tmp2DE,q2);
({void*_tmp513=_tmp2DD;Cyc_Port_unifies(_tmp513,Cyc_Port_type_to_ctype(env,_tmp311->type));});
if((unsigned int)_tmp311->initializer){
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_check_null(_tmp311->initializer);
({void*_tmp514=Cyc_Port_gen_initializer(env,_tmp311->type,e);Cyc_Port_leq(_tmp514,_tmp2DD);});}
# 1715
return env;};};}else{
# 1717
return Cyc_Port_gen_localdecl(env,d,1);}case 1U: _LL7: _tmp312=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp2D9)->f1;_LL8:
# 1723
(*_tmp312->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple14*predeclared=0;
# 1726
if(Cyc_Port_declared_var(env,_tmp312->name))
({struct _tuple14*_tmp515=Cyc_Port_lookup_full_var(env,_tmp312->name);predeclared=_tmp515;});{
# 1729
void*_tmp2E1=_tmp312->ret_type;
struct Cyc_List_List*_tmp2E2=_tmp312->args;
struct Cyc_List_List*_tmp2E3=((struct Cyc_List_List*(*)(struct _tuple8*(*f)(struct _tuple8*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_make_targ,_tmp2E2);
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2E4=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));
_tmp2FF->tag=5U,(_tmp2FF->f1).tvars=0,(_tmp2FF->f1).effect=0,({struct Cyc_Absyn_Tqual _tmp516=Cyc_Absyn_empty_tqual(0U);(_tmp2FF->f1).ret_tqual=_tmp516;}),(_tmp2FF->f1).ret_type=_tmp2E1,(_tmp2FF->f1).args=_tmp2E3,(_tmp2FF->f1).c_varargs=0,(_tmp2FF->f1).cyc_varargs=0,(_tmp2FF->f1).rgn_po=0,(_tmp2FF->f1).attributes=0,(_tmp2FF->f1).requires_clause=0,(_tmp2FF->f1).requires_relns=0,(_tmp2FF->f1).ensures_clause=0,(_tmp2FF->f1).ensures_relns=0;_tmp2FF;});
# 1736
struct Cyc_Port_Cenv*_tmp2E5=Cyc_Port_set_cpos(env,Cyc_Port_FnBody_pos);
void*_tmp2E6=Cyc_Port_type_to_ctype(_tmp2E5,_tmp2E1);
struct Cyc_List_List*c_args=0;
struct Cyc_List_List*c_arg_types=0;
{struct Cyc_List_List*_tmp2E7=_tmp2E2;for(0;(unsigned int)_tmp2E7;_tmp2E7=_tmp2E7->tl){
struct _tuple8 _tmp2E8=*((struct _tuple8*)_tmp2E7->hd);struct _tuple8 _tmp2E9=_tmp2E8;struct _dyneither_ptr*_tmp2F0;struct Cyc_Absyn_Tqual _tmp2EF;void*_tmp2EE;_LL18: _tmp2F0=_tmp2E9.f1;_tmp2EF=_tmp2E9.f2;_tmp2EE=_tmp2E9.f3;_LL19:;{
# 1744
void*_tmp2EA=Cyc_Port_type_to_ctype(_tmp2E5,_tmp2EE);
void*tqv;
if((env->gcenv)->porting){
({void*_tmp517=Cyc_Port_var();tqv=_tmp517;});
({struct Cyc_Port_Cenv*_tmp51A=env;void*_tmp519=tqv;void*_tmp518=_tmp2EF.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();Cyc_Port_register_const_cvar(_tmp51A,_tmp519,_tmp518,_tmp2EF.loc);});}else{
# 1756
({void*_tmp51B=_tmp2EF.print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();tqv=_tmp51B;});}
# 1758
({struct Cyc_List_List*_tmp51D=({struct Cyc_List_List*_tmp2EC=_cycalloc(sizeof(*_tmp2EC));({struct _tuple18*_tmp51C=({struct _tuple18*_tmp2EB=_cycalloc(sizeof(*_tmp2EB));_tmp2EB->f1=_tmp2F0,_tmp2EB->f2=_tmp2EE,_tmp2EB->f3=tqv,_tmp2EB->f4=_tmp2EA;_tmp2EB;});_tmp2EC->hd=_tmp51C;}),_tmp2EC->tl=c_args;_tmp2EC;});c_args=_tmp51D;});
({struct Cyc_List_List*_tmp51E=({struct Cyc_List_List*_tmp2ED=_cycalloc(sizeof(*_tmp2ED));_tmp2ED->hd=_tmp2EA,_tmp2ED->tl=c_arg_types;_tmp2ED;});c_arg_types=_tmp51E;});};}}
# 1761
({struct Cyc_List_List*_tmp51F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_args);c_args=_tmp51F;});
({struct Cyc_List_List*_tmp520=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(c_arg_types);c_arg_types=_tmp520;});{
void*_tmp2F1=Cyc_Port_fn_ct(_tmp2E6,c_arg_types);
# 1767
(*_tmp312->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp525=({struct Cyc_Port_Cenv*_tmp524=_tmp2E5;struct _tuple0*_tmp523=_tmp312->name;void*_tmp522=(void*)_tmp2E4;void*_tmp521=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp524,_tmp523,_tmp522,_tmp521,_tmp2F1,0U);});_tmp2E5=_tmp525;});
Cyc_Port_add_return_type(_tmp2E5,_tmp2E6);
{struct Cyc_List_List*_tmp2F2=c_args;for(0;(unsigned int)_tmp2F2;_tmp2F2=_tmp2F2->tl){
struct _tuple18 _tmp2F3=*((struct _tuple18*)_tmp2F2->hd);struct _tuple18 _tmp2F4=_tmp2F3;struct _dyneither_ptr*_tmp2F9;void*_tmp2F8;void*_tmp2F7;void*_tmp2F6;_LL1B: _tmp2F9=_tmp2F4.f1;_tmp2F8=_tmp2F4.f2;_tmp2F7=_tmp2F4.f3;_tmp2F6=_tmp2F4.f4;_LL1C:;
({struct Cyc_Port_Cenv*_tmp52A=({struct Cyc_Port_Cenv*_tmp529=_tmp2E5;struct _tuple0*_tmp528=({struct _tuple0*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));_tmp2F5->f1=Cyc_Absyn_Loc_n,_tmp2F5->f2=_tmp2F9;_tmp2F5;});void*_tmp527=_tmp2F8;void*_tmp526=_tmp2F7;Cyc_Port_add_var(_tmp529,_tmp528,_tmp527,_tmp526,_tmp2F6,0U);});_tmp2E5=_tmp52A;});}}
# 1774
Cyc_Port_gen_stmt(_tmp2E5,_tmp312->body);
# 1779
Cyc_Port_generalize(0,_tmp2F1);{
# 1786
struct Cyc_Dict_Dict counts=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);
Cyc_Port_region_counts(& counts,_tmp2F1);
# 1790
Cyc_Port_register_rgns(env,counts,1,(void*)_tmp2E4,_tmp2F1);
({struct Cyc_Port_Cenv*_tmp52F=({struct Cyc_Port_Cenv*_tmp52E=_tmp2E5;struct _tuple0*_tmp52D=_tmp312->name;void*_tmp52C=(void*)_tmp2E4;void*_tmp52B=Cyc_Port_const_ct();Cyc_Port_add_var(_tmp52E,_tmp52D,_tmp52C,_tmp52B,_tmp2F1,0U);});env=_tmp52F;});
if((unsigned int)predeclared){
# 1799
struct _tuple14 _tmp2FA=*predeclared;struct _tuple14 _tmp2FB=_tmp2FA;void*_tmp2FE;void*_tmp2FD;void*_tmp2FC;_LL1E: _tmp2FE=_tmp2FB.f1;_tmp2FD=(_tmp2FB.f2)->f1;_tmp2FC=(_tmp2FB.f2)->f2;_LL1F:;
({void*_tmp530=_tmp2FD;Cyc_Port_unifies(_tmp530,Cyc_Port_const_ct());});
({void*_tmp531=_tmp2FC;Cyc_Port_unifies(_tmp531,Cyc_Port_instantiate(_tmp2F1));});
# 1803
Cyc_Port_register_rgns(env,counts,1,_tmp2FE,_tmp2F1);}
# 1805
return env;};};};};case 8U: _LL9: _tmp313=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp2D9)->f1;_LLA: {
# 1811
void*_tmp300=(void*)_check_null(_tmp313->defn);
void*_tmp301=Cyc_Port_type_to_ctype(env,_tmp300);
(*_tmp313->name).f1=Cyc_Absyn_Loc_n;
Cyc_Port_add_typedef(env,_tmp313->name,_tmp300,_tmp301);
return env;}case 5U: _LLB: _tmp314=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp2D9)->f1;_LLC: {
# 1821
struct _tuple0*_tmp302=_tmp314->name;
(*_tmp314->name).f1=Cyc_Absyn_Loc_n;{
struct _tuple10*previous;
struct Cyc_List_List*curr=0;
{enum Cyc_Absyn_AggrKind _tmp303=_tmp314->kind;enum Cyc_Absyn_AggrKind _tmp304=_tmp303;if(_tmp304 == Cyc_Absyn_StructA){_LL21: _LL22:
# 1827
({struct _tuple10*_tmp532=Cyc_Port_lookup_struct_decl(env,_tmp302);previous=_tmp532;});
goto _LL20;}else{_LL23: _LL24:
# 1830
({struct _tuple10*_tmp533=Cyc_Port_lookup_union_decl(env,_tmp302);previous=_tmp533;});
goto _LL20;}_LL20:;}
# 1833
if((unsigned int)_tmp314->impl){
struct Cyc_List_List*cf=(*previous).f2;
{struct Cyc_List_List*_tmp305=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp314->impl))->fields;for(0;(unsigned int)_tmp305;_tmp305=_tmp305->tl){
struct Cyc_Absyn_Aggrfield*_tmp306=(struct Cyc_Absyn_Aggrfield*)_tmp305->hd;
void*qv;
if((env->gcenv)->porting){
({void*_tmp534=Cyc_Port_var();qv=_tmp534;});
({struct Cyc_Port_Cenv*_tmp537=env;void*_tmp536=qv;void*_tmp535=
(_tmp306->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();
# 1840
Cyc_Port_register_const_cvar(_tmp537,_tmp536,_tmp535,(_tmp306->tq).loc);});}else{
# 1849
({void*_tmp538=(_tmp306->tq).print_const?Cyc_Port_const_ct(): Cyc_Port_notconst_ct();qv=_tmp538;});}{
# 1851
void*_tmp307=Cyc_Port_type_to_ctype(env,_tmp306->type);
if(cf != 0){
struct Cyc_Port_Cfield _tmp308=*((struct Cyc_Port_Cfield*)cf->hd);struct Cyc_Port_Cfield _tmp309=_tmp308;void*_tmp30B;void*_tmp30A;_LL26: _tmp30B=_tmp309.qual;_tmp30A=_tmp309.type;_LL27:;
cf=cf->tl;
Cyc_Port_unifies(qv,_tmp30B);
Cyc_Port_unifies(_tmp307,_tmp30A);}
# 1858
({struct Cyc_List_List*_tmp53A=({struct Cyc_List_List*_tmp30D=_cycalloc(sizeof(*_tmp30D));({struct Cyc_Port_Cfield*_tmp539=({struct Cyc_Port_Cfield*_tmp30C=_cycalloc(sizeof(*_tmp30C));_tmp30C->qual=qv,_tmp30C->f=_tmp306->name,_tmp30C->type=_tmp307;_tmp30C;});_tmp30D->hd=_tmp539;}),_tmp30D->tl=curr;_tmp30D;});curr=_tmp53A;});};}}
# 1860
({struct Cyc_List_List*_tmp53B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(curr);curr=_tmp53B;});
if((*previous).f2 == 0)
(*previous).f2=curr;}
# 1865
return env;};}case 7U: _LLD: _tmp315=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp2D9)->f1;_LLE:
# 1870
(*_tmp315->name).f1=Cyc_Absyn_Loc_n;
# 1872
if((unsigned int)_tmp315->fields){
struct Cyc_List_List*_tmp30E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp315->fields))->v;for(0;(unsigned int)_tmp30E;_tmp30E=_tmp30E->tl){
(*((struct Cyc_Absyn_Enumfield*)_tmp30E->hd)->name).f1=Cyc_Absyn_Loc_n;
({struct Cyc_Port_Cenv*_tmp540=({struct Cyc_Port_Cenv*_tmp53F=env;struct _tuple0*_tmp53E=((struct Cyc_Absyn_Enumfield*)_tmp30E->hd)->name;void*_tmp53D=Cyc_Absyn_enum_type(_tmp315->name,_tmp315);void*_tmp53C=
Cyc_Port_const_ct();
# 1875
Cyc_Port_add_var(_tmp53F,_tmp53E,_tmp53D,_tmp53C,
Cyc_Port_arith_ct(),0U);});
# 1875
env=_tmp540;});}}
# 1878
return env;default: _LLF: _LL10:
# 1880
 if((env->gcenv)->porting)
({void*_tmp30F=0U;({struct Cyc___cycFILE*_tmp542=Cyc_stderr;struct _dyneither_ptr _tmp541=({const char*_tmp310="Warning: Cyclone declaration found in code to be ported -- skipping.";_tag_dyneither(_tmp310,sizeof(char),69U);});Cyc_fprintf(_tmp542,_tmp541,_tag_dyneither(_tmp30F,sizeof(void*),0U));});});
return env;}_LL0:;}
# 1887
static struct Cyc_Port_Cenv*Cyc_Port_gen_topdecls(struct Cyc_Port_Cenv*env,struct Cyc_List_List*ds){
for(0;(unsigned int)ds;ds=ds->tl){
({struct Cyc_Port_Cenv*_tmp543=Cyc_Port_gen_topdecl(env,(struct Cyc_Absyn_Decl*)ds->hd);env=_tmp543;});}
return env;}
# 1894
static struct Cyc_List_List*Cyc_Port_gen_edits(struct Cyc_List_List*ds){
# 1896
struct Cyc_Port_Cenv*env=({struct Cyc_Port_Cenv*_tmp544=Cyc_Port_initial_cenv();Cyc_Port_gen_topdecls(_tmp544,ds);});
# 1898
struct Cyc_List_List*_tmp316=(env->gcenv)->pointer_edits;
struct Cyc_List_List*_tmp317=(env->gcenv)->qualifier_edits;
struct Cyc_List_List*_tmp318=(env->gcenv)->zeroterm_edits;
struct Cyc_List_List*_tmp319=(env->gcenv)->edits;
struct Cyc_List_List*_tmp31A=(env->gcenv)->vardecl_locs;
# 1904
for(0;(unsigned int)_tmp31A;_tmp31A=_tmp31A->tl){
struct _tuple15 _tmp31B=*((struct _tuple15*)_tmp31A->hd);struct _tuple15 _tmp31C=_tmp31B;struct _tuple0*_tmp337;unsigned int _tmp336;void*_tmp335;_LL1: _tmp337=_tmp31C.f1;_tmp336=_tmp31C.f2;_tmp335=_tmp31C.f3;_LL2:;{
struct _tuple0 _tmp31D=*_tmp337;struct _tuple0 _tmp31E=_tmp31D;struct _dyneither_ptr*_tmp334;_LL4: _tmp334=_tmp31E.f2;_LL5:;{
struct Cyc_Port_VarUsage*_tmp31F=((struct Cyc_Port_VarUsage*(*)(struct Cyc_Hashtable_Table*t,unsigned int key))Cyc_Hashtable_lookup)((env->gcenv)->varusage_tab,_tmp336);
if(((struct Cyc_Port_VarUsage*)_check_null(_tmp31F))->address_taken){
if((unsigned int)_tmp31F->init){
# 1911
({struct Cyc_List_List*_tmp548=({struct Cyc_List_List*_tmp323=_cycalloc(sizeof(*_tmp323));({struct Cyc_Port_Edit*_tmp547=({struct Cyc_Port_Edit*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->loc=_tmp336,({struct _dyneither_ptr _tmp546=({const char*_tmp320="@";_tag_dyneither(_tmp320,sizeof(char),2U);});_tmp322->old_string=_tmp546;}),({struct _dyneither_ptr _tmp545=({const char*_tmp321="";_tag_dyneither(_tmp321,sizeof(char),1U);});_tmp322->new_string=_tmp545;});_tmp322;});_tmp323->hd=_tmp547;}),_tmp323->tl=_tmp319;_tmp323;});_tmp319=_tmp548;});
({struct Cyc_List_List*_tmp54C=({struct Cyc_List_List*_tmp327=_cycalloc(sizeof(*_tmp327));({struct Cyc_Port_Edit*_tmp54B=({struct Cyc_Port_Edit*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->loc=((struct Cyc_Absyn_Exp*)_check_null(_tmp31F->init))->loc,({struct _dyneither_ptr _tmp54A=({const char*_tmp324="new ";_tag_dyneither(_tmp324,sizeof(char),5U);});_tmp326->old_string=_tmp54A;}),({struct _dyneither_ptr _tmp549=({const char*_tmp325="";_tag_dyneither(_tmp325,sizeof(char),1U);});_tmp326->new_string=_tmp549;});_tmp326;});_tmp327->hd=_tmp54B;}),_tmp327->tl=_tmp319;_tmp327;});_tmp319=_tmp54C;});}else{
# 1916
({struct Cyc_List_List*_tmp551=({struct Cyc_List_List*_tmp32D=_cycalloc(sizeof(*_tmp32D));({struct Cyc_Port_Edit*_tmp550=({struct Cyc_Port_Edit*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->loc=_tmp336,({struct _dyneither_ptr _tmp54F=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp32A=({struct Cyc_String_pa_PrintArg_struct _tmp3AF;_tmp3AF.tag=0U,_tmp3AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp334);_tmp3AF;});struct Cyc_String_pa_PrintArg_struct _tmp32B=({struct Cyc_String_pa_PrintArg_struct _tmp3AE;_tmp3AE.tag=0U,({struct _dyneither_ptr _tmp54D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp335));_tmp3AE.f1=_tmp54D;});_tmp3AE;});void*_tmp328[2U];_tmp328[0]=& _tmp32A,_tmp328[1]=& _tmp32B;({struct _dyneither_ptr _tmp54E=({const char*_tmp329="?%s = malloc(sizeof(%s))";_tag_dyneither(_tmp329,sizeof(char),25U);});Cyc_aprintf(_tmp54E,_tag_dyneither(_tmp328,sizeof(void*),2U));});});_tmp32C->old_string=_tmp54F;}),_tmp32C->new_string=*_tmp334;_tmp32C;});_tmp32D->hd=_tmp550;}),_tmp32D->tl=_tmp319;_tmp32D;});_tmp319=_tmp551;});}{
# 1918
struct Cyc_List_List*_tmp32E=_tmp31F->usage_locs;
for(0;(unsigned int)_tmp32E;_tmp32E=_tmp32E->tl){
({struct Cyc_List_List*_tmp555=({struct Cyc_List_List*_tmp333=_cycalloc(sizeof(*_tmp333));({struct Cyc_Port_Edit*_tmp554=({struct Cyc_Port_Edit*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332->loc=(unsigned int)_tmp32E->hd,({struct _dyneither_ptr _tmp553=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp331=({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0U,_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp334);_tmp3B0;});void*_tmp32F[1U];_tmp32F[0]=& _tmp331;({struct _dyneither_ptr _tmp552=({const char*_tmp330="(*%s)";_tag_dyneither(_tmp330,sizeof(char),6U);});Cyc_aprintf(_tmp552,_tag_dyneither(_tmp32F,sizeof(void*),1U));});});_tmp332->old_string=_tmp553;}),_tmp332->new_string=*_tmp334;_tmp332;});_tmp333->hd=_tmp554;}),_tmp333->tl=_tmp319;_tmp333;});_tmp319=_tmp555;});}};}};};}
# 1926
{struct Cyc_List_List*_tmp338=_tmp316;for(0;(unsigned int)_tmp338;_tmp338=_tmp338->tl){
struct _tuple16 _tmp339=*((struct _tuple16*)_tmp338->hd);struct _tuple16 _tmp33A=_tmp339;void*_tmp33D;void*_tmp33C;unsigned int _tmp33B;_LL7: _tmp33D=_tmp33A.f1;_tmp33C=_tmp33A.f2;_tmp33B=_tmp33A.f3;_LL8:;
Cyc_Port_unifies(_tmp33D,_tmp33C);}}
# 1930
{struct Cyc_List_List*_tmp33E=_tmp317;for(0;(unsigned int)_tmp33E;_tmp33E=_tmp33E->tl){
struct _tuple16 _tmp33F=*((struct _tuple16*)_tmp33E->hd);struct _tuple16 _tmp340=_tmp33F;void*_tmp343;void*_tmp342;unsigned int _tmp341;_LLA: _tmp343=_tmp340.f1;_tmp342=_tmp340.f2;_tmp341=_tmp340.f3;_LLB:;
Cyc_Port_unifies(_tmp343,_tmp342);}}
# 1934
{struct Cyc_List_List*_tmp344=_tmp318;for(0;(unsigned int)_tmp344;_tmp344=_tmp344->tl){
struct _tuple16 _tmp345=*((struct _tuple16*)_tmp344->hd);struct _tuple16 _tmp346=_tmp345;void*_tmp349;void*_tmp348;unsigned int _tmp347;_LLD: _tmp349=_tmp346.f1;_tmp348=_tmp346.f2;_tmp347=_tmp346.f3;_LLE:;
Cyc_Port_unifies(_tmp349,_tmp348);}}
# 1940
for(0;(unsigned int)_tmp316;_tmp316=_tmp316->tl){
struct _tuple16 _tmp34A=*((struct _tuple16*)_tmp316->hd);struct _tuple16 _tmp34B=_tmp34A;void*_tmp358;void*_tmp357;unsigned int _tmp356;_LL10: _tmp358=_tmp34B.f1;_tmp357=_tmp34B.f2;_tmp356=_tmp34B.f3;_LL11:;
if(!Cyc_Port_unifies(_tmp358,_tmp357) && (int)_tmp356){
void*_tmp34C=Cyc_Port_compress_ct(_tmp357);void*_tmp34D=_tmp34C;switch(*((int*)_tmp34D)){case 2U: _LL13: _LL14:
# 1945
({struct Cyc_List_List*_tmp559=({struct Cyc_List_List*_tmp351=_cycalloc(sizeof(*_tmp351));({struct Cyc_Port_Edit*_tmp558=({struct Cyc_Port_Edit*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350->loc=_tmp356,({struct _dyneither_ptr _tmp557=({const char*_tmp34E="?";_tag_dyneither(_tmp34E,sizeof(char),2U);});_tmp350->old_string=_tmp557;}),({struct _dyneither_ptr _tmp556=({const char*_tmp34F="*";_tag_dyneither(_tmp34F,sizeof(char),2U);});_tmp350->new_string=_tmp556;});_tmp350;});_tmp351->hd=_tmp558;}),_tmp351->tl=_tmp319;_tmp351;});_tmp319=_tmp559;});
goto _LL12;case 3U: _LL15: _LL16:
# 1948
({struct Cyc_List_List*_tmp55D=({struct Cyc_List_List*_tmp355=_cycalloc(sizeof(*_tmp355));({struct Cyc_Port_Edit*_tmp55C=({struct Cyc_Port_Edit*_tmp354=_cycalloc(sizeof(*_tmp354));_tmp354->loc=_tmp356,({struct _dyneither_ptr _tmp55B=({const char*_tmp352="*";_tag_dyneither(_tmp352,sizeof(char),2U);});_tmp354->old_string=_tmp55B;}),({struct _dyneither_ptr _tmp55A=({const char*_tmp353="?";_tag_dyneither(_tmp353,sizeof(char),2U);});_tmp354->new_string=_tmp55A;});_tmp354;});_tmp355->hd=_tmp55C;}),_tmp355->tl=_tmp319;_tmp355;});_tmp319=_tmp55D;});
goto _LL12;default: _LL17: _LL18:
 goto _LL12;}_LL12:;}}
# 1955
for(0;(unsigned int)_tmp317;_tmp317=_tmp317->tl){
struct _tuple16 _tmp359=*((struct _tuple16*)_tmp317->hd);struct _tuple16 _tmp35A=_tmp359;void*_tmp367;void*_tmp366;unsigned int _tmp365;_LL1A: _tmp367=_tmp35A.f1;_tmp366=_tmp35A.f2;_tmp365=_tmp35A.f3;_LL1B:;
if(!Cyc_Port_unifies(_tmp367,_tmp366) && (int)_tmp365){
void*_tmp35B=Cyc_Port_compress_ct(_tmp366);void*_tmp35C=_tmp35B;switch(*((int*)_tmp35C)){case 1U: _LL1D: _LL1E:
# 1960
({struct Cyc_List_List*_tmp561=({struct Cyc_List_List*_tmp360=_cycalloc(sizeof(*_tmp360));({struct Cyc_Port_Edit*_tmp560=({struct Cyc_Port_Edit*_tmp35F=_cycalloc(sizeof(*_tmp35F));_tmp35F->loc=_tmp365,({struct _dyneither_ptr _tmp55F=({const char*_tmp35D="const ";_tag_dyneither(_tmp35D,sizeof(char),7U);});_tmp35F->old_string=_tmp55F;}),({struct _dyneither_ptr _tmp55E=({const char*_tmp35E="";_tag_dyneither(_tmp35E,sizeof(char),1U);});_tmp35F->new_string=_tmp55E;});_tmp35F;});_tmp360->hd=_tmp560;}),_tmp360->tl=_tmp319;_tmp360;});_tmp319=_tmp561;});goto _LL1C;case 0U: _LL1F: _LL20:
# 1962
({struct Cyc_List_List*_tmp565=({struct Cyc_List_List*_tmp364=_cycalloc(sizeof(*_tmp364));({struct Cyc_Port_Edit*_tmp564=({struct Cyc_Port_Edit*_tmp363=_cycalloc(sizeof(*_tmp363));_tmp363->loc=_tmp365,({struct _dyneither_ptr _tmp563=({const char*_tmp361="";_tag_dyneither(_tmp361,sizeof(char),1U);});_tmp363->old_string=_tmp563;}),({struct _dyneither_ptr _tmp562=({const char*_tmp362="const ";_tag_dyneither(_tmp362,sizeof(char),7U);});_tmp363->new_string=_tmp562;});_tmp363;});_tmp364->hd=_tmp564;}),_tmp364->tl=_tmp319;_tmp364;});_tmp319=_tmp565;});goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}}
# 1968
for(0;(unsigned int)_tmp318;_tmp318=_tmp318->tl){
struct _tuple16 _tmp368=*((struct _tuple16*)_tmp318->hd);struct _tuple16 _tmp369=_tmp368;void*_tmp376;void*_tmp375;unsigned int _tmp374;_LL24: _tmp376=_tmp369.f1;_tmp375=_tmp369.f2;_tmp374=_tmp369.f3;_LL25:;
if(!Cyc_Port_unifies(_tmp376,_tmp375) && (int)_tmp374){
void*_tmp36A=Cyc_Port_compress_ct(_tmp375);void*_tmp36B=_tmp36A;switch(*((int*)_tmp36B)){case 8U: _LL27: _LL28:
# 1973
({struct Cyc_List_List*_tmp569=({struct Cyc_List_List*_tmp36F=_cycalloc(sizeof(*_tmp36F));({struct Cyc_Port_Edit*_tmp568=({struct Cyc_Port_Edit*_tmp36E=_cycalloc(sizeof(*_tmp36E));_tmp36E->loc=_tmp374,({struct _dyneither_ptr _tmp567=({const char*_tmp36C="@nozeroterm ";_tag_dyneither(_tmp36C,sizeof(char),13U);});_tmp36E->old_string=_tmp567;}),({struct _dyneither_ptr _tmp566=({const char*_tmp36D="";_tag_dyneither(_tmp36D,sizeof(char),1U);});_tmp36E->new_string=_tmp566;});_tmp36E;});_tmp36F->hd=_tmp568;}),_tmp36F->tl=_tmp319;_tmp36F;});_tmp319=_tmp569;});goto _LL26;case 9U: _LL29: _LL2A:
# 1975
({struct Cyc_List_List*_tmp56D=({struct Cyc_List_List*_tmp373=_cycalloc(sizeof(*_tmp373));({struct Cyc_Port_Edit*_tmp56C=({struct Cyc_Port_Edit*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->loc=_tmp374,({struct _dyneither_ptr _tmp56B=({const char*_tmp370="@zeroterm ";_tag_dyneither(_tmp370,sizeof(char),11U);});_tmp372->old_string=_tmp56B;}),({struct _dyneither_ptr _tmp56A=({const char*_tmp371="";_tag_dyneither(_tmp371,sizeof(char),1U);});_tmp372->new_string=_tmp56A;});_tmp372;});_tmp373->hd=_tmp56C;}),_tmp373->tl=_tmp319;_tmp373;});_tmp319=_tmp56D;});goto _LL26;default: _LL2B: _LL2C:
 goto _LL26;}_LL26:;}}
# 1982
({struct Cyc_List_List*_tmp56E=((struct Cyc_List_List*(*)(int(*cmp)(struct Cyc_Port_Edit*,struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_merge_sort)(Cyc_Port_cmp_edit,_tmp319);_tmp319=_tmp56E;});
# 1984
while((unsigned int)_tmp319  && ((struct Cyc_Port_Edit*)_tmp319->hd)->loc == 0){
# 1988
_tmp319=_tmp319->tl;}
# 1990
return _tmp319;}
# 1993
static unsigned int Cyc_Port_get_seg(struct Cyc_Port_Edit*e){
return e->loc;}
# 1999
void Cyc_Port_port(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp377=Cyc_Port_gen_edits(ds);
struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(unsigned int(*f)(struct Cyc_Port_Edit*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Port_get_seg,_tmp377);
Cyc_Position_use_gcc_style_location=0;{
struct Cyc_List_List*_tmp379=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Position_strings_of_segments(_tmp378));
for(0;(unsigned int)_tmp377;(_tmp377=_tmp377->tl,_tmp379=_tmp379->tl)){
struct Cyc_Port_Edit _tmp37A=*((struct Cyc_Port_Edit*)_tmp377->hd);struct Cyc_Port_Edit _tmp37B=_tmp37A;unsigned int _tmp383;struct _dyneither_ptr _tmp382;struct _dyneither_ptr _tmp381;_LL1: _tmp383=_tmp37B.loc;_tmp382=_tmp37B.old_string;_tmp381=_tmp37B.new_string;_LL2:;{
struct _dyneither_ptr sloc=(struct _dyneither_ptr)*((struct _dyneither_ptr*)((struct Cyc_List_List*)_check_null(_tmp379))->hd);
({struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0U,_tmp3B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)sloc);_tmp3B3;});struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0U,_tmp3B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp382);_tmp3B2;});struct Cyc_String_pa_PrintArg_struct _tmp380=({struct Cyc_String_pa_PrintArg_struct _tmp3B1;_tmp3B1.tag=0U,_tmp3B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp381);_tmp3B1;});void*_tmp37C[3U];_tmp37C[0]=& _tmp37E,_tmp37C[1]=& _tmp37F,_tmp37C[2]=& _tmp380;({struct _dyneither_ptr _tmp56F=({const char*_tmp37D="%s: insert `%s' for `%s'\n";_tag_dyneither(_tmp37D,sizeof(char),26U);});Cyc_printf(_tmp56F,_tag_dyneither(_tmp37C,sizeof(void*),3U));});});};}};}
