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
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 43
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 47
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);
int Cyc_Position_error_p();struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 302
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 317
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 324
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 427 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 517
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 524
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 542
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 899 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 901
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 909
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 912
void*Cyc_Absyn_compress_kb(void*);
# 918
int Cyc_Absyn_type2bool(int def,void*);
# 923
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 928
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 930
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 935
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 937
extern void*Cyc_Absyn_empty_effect;
# 939
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 941
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple2*n,struct Cyc_Absyn_Enumdecl*d);
# 960
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 966
extern void*Cyc_Absyn_fat_bound_type;
# 968
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 970
void*Cyc_Absyn_bounds_one();
# 972
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 977
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 1001
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 1003
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1052
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1111
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1158
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1160
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1162
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1164
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1166
int Cyc_Absyn_fntype_att(void*);
# 1172
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 74
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 75
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 78
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 141
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 41
int Cyc_Tcutil_is_void_type(void*);
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_array_type(void*t);
int Cyc_Tcutil_is_boxed(void*t);
# 53
int Cyc_Tcutil_is_dyneither_ptr(void*t);
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
int Cyc_Tcutil_is_bound_one(void*b);
# 58
int Cyc_Tcutil_is_tagged_pointer_type(void*);
# 61
int Cyc_Tcutil_is_bits_only_type(void*t);
# 63
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 68
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 70
void*Cyc_Tcutil_pointer_region(void*t);
# 73
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 76
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 79
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 81
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_dest);
# 83
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest);
# 85
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 89
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 93
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
# 101
void*Cyc_Tcutil_copy_type(void*t);
# 104
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 107
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 111
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t);
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 124
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 126
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 128
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple14{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 132
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 134
extern int Cyc_Tcutil_warn_alias_coerce;
# 137
extern int Cyc_Tcutil_warn_region_coerce;
# 141
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 150
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 155
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 160
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 169
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 174
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 179
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple15{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 183
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 188
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 190
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 194
void Cyc_Tcutil_explain_failure();
# 196
int Cyc_Tcutil_unify(void*,void*);
# 198
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 201
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 203
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 208
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 212
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 216
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 219
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 223
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 225
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 229
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple16{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*,struct Cyc_Absyn_Tvar*);
# 235
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 262 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 264
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 272
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 275
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 282
void Cyc_Tcutil_check_nonzero_bound(unsigned int,void*);
# 284
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,void*,int do_warn);
# 286
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 288
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 296
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 301
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 304
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 309
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 314
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple17{int f1;void*f2;};
# 318
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 326
void*Cyc_Tcutil_normalize_effect(void*e);
# 329
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 331
int Cyc_Tcutil_new_tvar_id();
# 333
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 336
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 338
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 341
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 344
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 347
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 350
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 354
int Cyc_Tcutil_supports_default(void*);
# 358
int Cyc_Tcutil_admits_zero(void*t);
# 362
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 366
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 369
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 373
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 376
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 380
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 383
int Cyc_Tcutil_zeroable_type(void*t);
# 387
int Cyc_Tcutil_force_type2bool(int desired,void*t);
# 390
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**te);
# 392
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**te);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 40 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 69
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 109
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 42 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_tmp1=_tmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 49
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 56
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_tmp5=_tmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 63
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_tmp7=_tmp6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 71
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_tmp9=_tmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 79
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 87
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_tmpD=_tmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 95
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_tmpF=_tmpE;void*_tmp11;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {
# 98
void*_tmp10=_tmp11;switch(*((int*)_tmp10)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 105
 return 0;}_LL0:;}
# 109
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 113
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t) || Cyc_Tcutil_is_any_float_type(t);}
# 117
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_tmp13=_tmp12;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp13)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 125
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_type_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 133
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp14=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp15=_tmp14;void*_tmp17;switch(*((int*)_tmp15)){case 0U: _LL1: _tmp17=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15)->f1;_LL2: {
# 136
void*_tmp16=_tmp17;switch(*((int*)_tmp16)){case 1U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 goto _LLF;case 4U: _LLE: _LLF:
 return 1;default: _LL10: _LL11:
 return 0;}_LL7:;}case 1U: _LL3: _LL4:
# 143
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);default: _LL5: _LL6:
 return 0;}_LL0:;}
# 149
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp18=Cyc_Tcutil_compress(t);void*_tmp19=_tmp18;void*_tmp1A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19)->tag == 3U){_LL1: _tmp1A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19)->f1).ptr_atts).zero_term;_LL2:
# 152
 return Cyc_Tcutil_force_type2bool(0,_tmp1A);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 158
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1B=Cyc_Tcutil_compress(t);void*_tmp1C=_tmp1B;void*_tmp1D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->tag == 3U){_LL1: _tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->f1).ptr_atts).nullable;_LL2:
# 161
 return Cyc_Tcutil_force_type2bool(0,_tmp1D);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 167
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_tmp1F=_tmp1E;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).bounds;_LL2:
# 170
 return Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp20);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 177
int Cyc_Tcutil_is_tagged_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp21=Cyc_Tcutil_compress(t);void*_tmp22=_tmp21;void*_tmp24;void*_tmp23;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->tag == 3U){_LL1: _tmp24=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).elt_type;_tmp23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).ptr_atts).bounds;_LL2:
# 180
 if(Cyc_Tcutil_unify(_tmp23,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp24;
return 1;}else{
# 184
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 191
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp25=Cyc_Tcutil_compress(t);void*_tmp26=_tmp25;void*_tmp28;void*_tmp27;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->tag == 3U){_LL1: _tmp28=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).elt_type;_tmp27=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26)->f1).ptr_atts).zero_term;_LL2:
# 194
*elt_type_dest=_tmp28;
return Cyc_Absyn_type2bool(0,_tmp27);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 203
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 205
void*_tmp29=Cyc_Tcutil_compress(t);void*_tmp2A=_tmp29;void*_tmp33;struct Cyc_Absyn_Tqual _tmp32;struct Cyc_Absyn_Exp*_tmp31;void*_tmp30;void*_tmp2F;void*_tmp2E;void*_tmp2D;switch(*((int*)_tmp2A)){case 3U: _LL1: _tmp2F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A)->f1).elt_type;_tmp2E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A)->f1).ptr_atts).bounds;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A)->f1).ptr_atts).zero_term;_LL2:
# 207
 if(Cyc_Absyn_type2bool(0,_tmp2D)){
*ptr_type=t;
*elt_type=_tmp2F;
{void*_tmp2B=Cyc_Tcutil_compress(_tmp2E);void*_tmp2C=_tmp2B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C)->f1)->tag == 14U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 214
return 1;}else{
return 0;}case 4U: _LL3: _tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A)->f1).elt_type;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A)->f1).tq;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A)->f1).num_elts;_tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A)->f1).zero_term;_LL4:
# 217
 if(Cyc_Absyn_type2bool(0,_tmp30)){
*elt_type=_tmp33;
*is_dyneither=0;
({void*_tmpA32=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmpA32;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 230
int Cyc_Tcutil_is_tagged_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_tagged_pointer_type_elt(t,& ignore);}
# 236
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp34=({void*_tmpA33=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpA33,b);});
if(_tmp34 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp35=_tmp34;
struct _tuple13 _tmp36=Cyc_Evexp_eval_const_uint_exp(_tmp35);struct _tuple13 _tmp37=_tmp36;unsigned int _tmp39;int _tmp38;_LL1: _tmp39=_tmp37.f1;_tmp38=_tmp37.f2;_LL2:;
return _tmp38  && _tmp39 == 1;};}
# 245
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3A=Cyc_Tcutil_compress(t);void*_tmp3B=_tmp3A;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp46;void*_tmp45;void*_tmp44;struct Cyc_Absyn_Aggrdecl*_tmp43;struct Cyc_List_List*_tmp42;void*_tmp41;switch(*((int*)_tmp3B)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2 == 0){_LL1: _tmp41=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f1;_LL2: {
# 248
void*_tmp3C=_tmp41;switch(*((int*)_tmp3C)){case 0U: _LL10: _LL11:
 goto _LL13;case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 return 1;case 15U: _LL16: _LL17:
 goto _LL19;case 16U: _LL18: _LL19:
 return 0;default: _LL1A: _LL1B:
 return 0;}_LLF:;}}else{if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f1)->f1).UnknownAggr).tag == 1){_LL7: _LL8:
# 264
 return 0;}else{_LL9: _tmp43=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f1)->f1).KnownAggr).val;_tmp42=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B)->f2;_LLA:
# 266
 if(_tmp43->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp3D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3D;_push_region(rgn);
{struct Cyc_List_List*_tmp3E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp43->tvs,_tmp42);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,_tmp3E,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp3F=0;_npop_handler(0U);return _tmp3F;}}}{
int _tmp40=1;_npop_handler(0U);return _tmp40;};}
# 273
;_pop_region(rgn);};};}}else{goto _LLD;}}case 4U: _LL3: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).elt_type;_tmp44=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).zero_term;_LL4:
# 259
 return !Cyc_Absyn_type2bool(0,_tmp44) && Cyc_Tcutil_is_bits_only_type(_tmp45);case 6U: _LL5: _tmp46=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3B)->f1;_LL6:
# 261
 for(0;_tmp46 != 0;_tmp46=_tmp46->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)_tmp46->hd)).f2))return 0;}
return 1;case 7U: _LLB: _tmp47=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B)->f2;_LLC:
# 278
 for(0;_tmp47 != 0;_tmp47=_tmp47->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp47->hd)->type))return 0;}
return 1;default: _LLD: _LLE:
 return 0;}_LL0:;}
# 287
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
{void*_tmp48=Cyc_Tcutil_compress(t);void*_tmp49=_tmp48;struct Cyc_List_List*_tmp4D;void*_tmp4C;switch(*((int*)_tmp49)){case 3U: _LL1: _tmp4C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4C);case 5U: _LL3: _tmp4D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp49)->f1).attributes;_LL4:
# 291
 for(0;_tmp4D != 0;_tmp4D=_tmp4D->tl){
void*_tmp4A=(void*)_tmp4D->hd;void*_tmp4B=_tmp4A;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4B)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 296
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 299
return 0;}char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 303
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 305
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 309
int Cyc_Tcutil_warn_region_coerce=0;
# 312
static void*Cyc_Tcutil_t1_failure=0;
static int Cyc_Tcutil_tq1_const=0;
static void*Cyc_Tcutil_t2_failure=0;
static int Cyc_Tcutil_tq2_const=0;
# 317
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 321
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 326
if(({struct _dyneither_ptr _tmpA34=({const char*_tmp4E="(qualifiers don't match)";_tag_dyneither(_tmp4E,sizeof(char),25U);});Cyc_strcmp(_tmpA34,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp99C;_tmp99C.tag=0U,_tmp99C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp99C;});void*_tmp4F[1U];_tmp4F[0]=& _tmp51;({struct Cyc___cycFILE*_tmpA36=Cyc_stderr;struct _dyneither_ptr _tmpA35=({const char*_tmp50="  %s\n";_tag_dyneither(_tmp50,sizeof(char),6U);});Cyc_fprintf(_tmpA36,_tmpA35,_tag_dyneither(_tmp4F,sizeof(void*),1U));});});
return;}
# 331
if(({struct _dyneither_ptr _tmpA37=({const char*_tmp52="(function effects do not match)";_tag_dyneither(_tmp52,sizeof(char),32U);});Cyc_strcmp(_tmpA37,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp53=Cyc_Absynpp_tc_params_r;
_tmp53.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp53);}{
# 336
void*_tmp54=Cyc_Tcutil_t1_failure;
void*_tmp55=Cyc_Tcutil_t2_failure;
struct _dyneither_ptr s1=(unsigned int)_tmp54?Cyc_Absynpp_typ2string(_tmp54):({const char*_tmp70="<?>";_tag_dyneither(_tmp70,sizeof(char),4U);});
struct _dyneither_ptr s2=(unsigned int)_tmp55?Cyc_Absynpp_typ2string(_tmp55):({const char*_tmp6F="<?>";_tag_dyneither(_tmp6F,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp58=({struct Cyc_String_pa_PrintArg_struct _tmp99D;_tmp99D.tag=0U,_tmp99D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp99D;});void*_tmp56[1U];_tmp56[0]=& _tmp58;({struct Cyc___cycFILE*_tmpA39=Cyc_stderr;struct _dyneither_ptr _tmpA38=({const char*_tmp57="  %s";_tag_dyneither(_tmp57,sizeof(char),5U);});Cyc_fprintf(_tmpA39,_tmpA38,_tag_dyneither(_tmp56,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp59=0U;({struct Cyc___cycFILE*_tmpA3B=Cyc_stderr;struct _dyneither_ptr _tmpA3A=({const char*_tmp5A="\n\t";_tag_dyneither(_tmp5A,sizeof(char),3U);});Cyc_fprintf(_tmpA3B,_tmpA3A,_tag_dyneither(_tmp59,sizeof(void*),0U));});});
pos=8;}else{
# 347
({void*_tmp5B=0U;({struct Cyc___cycFILE*_tmpA3D=Cyc_stderr;struct _dyneither_ptr _tmpA3C=({const char*_tmp5C=" ";_tag_dyneither(_tmp5C,sizeof(char),2U);});Cyc_fprintf(_tmpA3D,_tmpA3C,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});
++ pos;}
# 350
({void*_tmp5D=0U;({struct Cyc___cycFILE*_tmpA3F=Cyc_stderr;struct _dyneither_ptr _tmpA3E=({const char*_tmp5E="and ";_tag_dyneither(_tmp5E,sizeof(char),5U);});Cyc_fprintf(_tmpA3F,_tmpA3E,_tag_dyneither(_tmp5D,sizeof(void*),0U));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp5F=0U;({struct Cyc___cycFILE*_tmpA41=Cyc_stderr;struct _dyneither_ptr _tmpA40=({const char*_tmp60="\n\t";_tag_dyneither(_tmp60,sizeof(char),3U);});Cyc_fprintf(_tmpA41,_tmpA40,_tag_dyneither(_tmp5F,sizeof(void*),0U));});});
pos=8;}
# 356
({struct Cyc_String_pa_PrintArg_struct _tmp63=({struct Cyc_String_pa_PrintArg_struct _tmp99E;_tmp99E.tag=0U,_tmp99E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp99E;});void*_tmp61[1U];_tmp61[0]=& _tmp63;({struct Cyc___cycFILE*_tmpA43=Cyc_stderr;struct _dyneither_ptr _tmpA42=({const char*_tmp62="%s ";_tag_dyneither(_tmp62,sizeof(char),4U);});Cyc_fprintf(_tmpA43,_tmpA42,_tag_dyneither(_tmp61,sizeof(void*),1U));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp64=0U;({struct Cyc___cycFILE*_tmpA45=Cyc_stderr;struct _dyneither_ptr _tmpA44=({const char*_tmp65="\n\t";_tag_dyneither(_tmp65,sizeof(char),3U);});Cyc_fprintf(_tmpA45,_tmpA44,_tag_dyneither(_tmp64,sizeof(void*),0U));});});
pos=8;}
# 362
({void*_tmp66=0U;({struct Cyc___cycFILE*_tmpA47=Cyc_stderr;struct _dyneither_ptr _tmpA46=({const char*_tmp67="are not compatible. ";_tag_dyneither(_tmp67,sizeof(char),21U);});Cyc_fprintf(_tmpA47,_tmpA46,_tag_dyneither(_tmp66,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpA48=(char*)Cyc_Tcutil_failure_reason.curr;_tmpA48 != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({int _tmpA49=pos;_tmpA49 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80)
({void*_tmp68=0U;({struct Cyc___cycFILE*_tmpA4B=Cyc_stderr;struct _dyneither_ptr _tmpA4A=({const char*_tmp69="\n\t";_tag_dyneither(_tmp69,sizeof(char),3U);});Cyc_fprintf(_tmpA4B,_tmpA4A,_tag_dyneither(_tmp68,sizeof(void*),0U));});});
# 368
({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp99F;_tmp99F.tag=0U,_tmp99F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);_tmp99F;});void*_tmp6A[1U];_tmp6A[0]=& _tmp6C;({struct Cyc___cycFILE*_tmpA4D=Cyc_stderr;struct _dyneither_ptr _tmpA4C=({const char*_tmp6B="%s";_tag_dyneither(_tmp6B,sizeof(char),3U);});Cyc_fprintf(_tmpA4D,_tmpA4C,_tag_dyneither(_tmp6A,sizeof(void*),1U));});});}
# 370
({void*_tmp6D=0U;({struct Cyc___cycFILE*_tmpA4F=Cyc_stderr;struct _dyneither_ptr _tmpA4E=({const char*_tmp6E="\n";_tag_dyneither(_tmp6E,sizeof(char),2U);});Cyc_fprintf(_tmpA4F,_tmpA4E,_tag_dyneither(_tmp6D,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 374
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 376
Cyc_Position_post_error(({unsigned int _tmpA50=loc;Cyc_Position_mk_err(_tmpA50,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 378
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 381
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
if(!Cyc_Position_error_p()){
({struct Cyc_String_pa_PrintArg_struct _tmp73=({struct Cyc_String_pa_PrintArg_struct _tmp9A0;_tmp9A0.tag=0U,_tmp9A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmp9A0;});void*_tmp71[1U];_tmp71[0]=& _tmp73;({struct Cyc___cycFILE*_tmpA52=Cyc_stderr;struct _dyneither_ptr _tmpA51=({const char*_tmp72="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp72,sizeof(char),36U);});Cyc_fprintf(_tmpA52,_tmpA51,_tag_dyneither(_tmp71,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);}
# 386
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->tag=Cyc_Core_Impossible,_tmp74->f1=msg;_tmp74;}));}
# 388
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 390
Cyc_Warn_vwarn(sg,fmt,ap);}
# 393
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp75=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp76=_tmp75;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp76)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp76)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 403
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 408
void*Cyc_Tcutil_compress(void*t){
void*_tmp77=t;void*_tmp80;struct Cyc_Absyn_Exp*_tmp7F;struct Cyc_Absyn_Exp*_tmp7E;void**_tmp7D;void**_tmp7C;switch(*((int*)_tmp77)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp77)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp7C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp77)->f2;_LL8: {
# 419
void*ta=(void*)_check_null(*_tmp7C);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7C=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp77)->f4 == 0){_LL3: _LL4:
# 411
 return t;}else{_LL5: _tmp7D=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp77)->f4;_LL6: {
# 413
void*ta=(void*)_check_null(*_tmp7D);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7D=t2;
return t2;}}case 9U: _LL9: _tmp7E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp77)->f1;_LLA:
# 425
 Cyc_Evexp_eval_const_uint_exp(_tmp7E);{
void*_tmp78=_tmp7E->r;void*_tmp79=_tmp78;void*_tmp7A;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp79)->tag == 39U){_LL12: _tmp7A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp79)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp7A);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp7F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp77)->f1;_LLC: {
# 431
void*_tmp7B=_tmp7F->topt;
if(_tmp7B != 0)return _tmp7B;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp77)->f2 != 0){_LLD: _tmp80=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp77)->f2;_LLE:
# 435
 return Cyc_Tcutil_compress(_tmp80);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 444
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 448
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp81=Cyc_Absyn_compress_kb(kb);void*_tmp82=_tmp81;struct Cyc_Absyn_Kind*_tmp85;switch(*((int*)_tmp82)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=1U,_tmp83->f1=0;_tmp83;});case 2U: _LL3: _tmp85=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp82)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=2U,_tmp84->f1=0,_tmp84->f2=_tmp85;_tmp84;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 455
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 457
return({struct Cyc_Absyn_Tvar*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->name=tv->name,_tmp86->identity=- 1,({void*_tmpA53=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp86->kind=_tmpA53;});_tmp86;});}
# 459
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 461
struct _tuple10*_tmp87=arg;struct _dyneither_ptr*_tmp8B;struct Cyc_Absyn_Tqual _tmp8A;void*_tmp89;_LL1: _tmp8B=_tmp87->f1;_tmp8A=_tmp87->f2;_tmp89=_tmp87->f3;_LL2:;
return({struct _tuple10*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->f1=_tmp8B,_tmp88->f2=_tmp8A,({void*_tmpA54=Cyc_Tcutil_copy_type(_tmp89);_tmp88->f3=_tmpA54;});_tmp88;});}
# 464
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp8C=arg;struct Cyc_Absyn_Tqual _tmp8F;void*_tmp8E;_LL1: _tmp8F=_tmp8C->f1;_tmp8E=_tmp8C->f2;_LL2:;
return({struct _tuple12*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D->f1=_tmp8F,({void*_tmpA55=Cyc_Tcutil_copy_type(_tmp8E);_tmp8D->f2=_tmpA55;});_tmp8D;});}
# 468
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 470
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->name=f->name,_tmp90->tq=f->tq,({void*_tmpA57=Cyc_Tcutil_copy_type(f->type);_tmp90->type=_tmpA57;}),_tmp90->width=f->width,_tmp90->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmpA56=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp90->requires_clause=_tmpA56;});_tmp90;});}
# 474
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp91=x;void*_tmp94;void*_tmp93;_LL1: _tmp94=_tmp91->f1;_tmp93=_tmp91->f2;_LL2:;
return({struct _tuple0*_tmp92=_cycalloc(sizeof(*_tmp92));({void*_tmpA59=Cyc_Tcutil_copy_type(_tmp94);_tmp92->f1=_tmpA59;}),({void*_tmpA58=Cyc_Tcutil_copy_type(_tmp93);_tmp92->f2=_tmpA58;});_tmp92;});}
# 478
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95->name=f->name,_tmp95->tag=f->tag,_tmp95->loc=f->loc;_tmp95;});}
# 481
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 485
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp96=Cyc_Tcutil_compress(t);void*_tmp97=_tmp96;struct Cyc_Absyn_Datatypedecl*_tmpDB;struct Cyc_Absyn_Enumdecl*_tmpDA;struct Cyc_Absyn_Aggrdecl*_tmpD9;struct _tuple2*_tmpD8;struct Cyc_List_List*_tmpD7;struct Cyc_Absyn_Typedefdecl*_tmpD6;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD4;enum Cyc_Absyn_AggrKind _tmpD3;struct Cyc_List_List*_tmpD2;struct Cyc_List_List*_tmpD1;struct Cyc_List_List*_tmpD0;void*_tmpCF;struct Cyc_Absyn_Tqual _tmpCE;void*_tmpCD;struct Cyc_List_List*_tmpCC;int _tmpCB;struct Cyc_Absyn_VarargInfo*_tmpCA;struct Cyc_List_List*_tmpC9;struct Cyc_List_List*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_List_List*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_List_List*_tmpC4;void*_tmpC3;struct Cyc_Absyn_Tqual _tmpC2;struct Cyc_Absyn_Exp*_tmpC1;void*_tmpC0;unsigned int _tmpBF;void*_tmpBE;struct Cyc_Absyn_Tqual _tmpBD;void*_tmpBC;void*_tmpBB;void*_tmpBA;void*_tmpB9;struct Cyc_Absyn_PtrLoc*_tmpB8;struct Cyc_Absyn_Tvar*_tmpB7;void*_tmpB6;struct Cyc_List_List*_tmpB5;void*_tmpB4;switch(*((int*)_tmp97)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp97)->f2 == 0){_LL1: _tmpB4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp97)->f1;_LL2:
 return t;}else{_LL3: _tmpB6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp97)->f1;_tmpB5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp97)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=0U,_tmp98->f1=_tmpB6,({struct Cyc_List_List*_tmpA5A=Cyc_Tcutil_copy_types(_tmpB5);_tmp98->f2=_tmpA5A;});_tmp98;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmpB7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp97)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmpB7));case 3U: _LL9: _tmpBE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).elt_type;_tmpBD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).elt_tq;_tmpBC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).ptr_atts).rgn;_tmpBB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).ptr_atts).nullable;_tmpBA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).ptr_atts).bounds;_tmpB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).ptr_atts).zero_term;_tmpB8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp97)->f1).ptr_atts).ptrloc;_LLA: {
# 492
void*_tmp99=Cyc_Tcutil_copy_type(_tmpBE);
void*_tmp9A=Cyc_Tcutil_copy_type(_tmpBC);
void*_tmp9B=Cyc_Tcutil_copy_type(_tmpBB);
struct Cyc_Absyn_Tqual _tmp9C=_tmpBD;
# 497
void*_tmp9D=Cyc_Tcutil_copy_type(_tmpBA);
void*_tmp9E=Cyc_Tcutil_copy_type(_tmpB9);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=3U,(_tmp9F->f1).elt_type=_tmp99,(_tmp9F->f1).elt_tq=_tmp9C,((_tmp9F->f1).ptr_atts).rgn=_tmp9A,((_tmp9F->f1).ptr_atts).nullable=_tmp9B,((_tmp9F->f1).ptr_atts).bounds=_tmp9D,((_tmp9F->f1).ptr_atts).zero_term=_tmp9E,((_tmp9F->f1).ptr_atts).ptrloc=_tmpB8;_tmp9F;});}case 4U: _LLB: _tmpC3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97)->f1).elt_type;_tmpC2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97)->f1).tq;_tmpC1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97)->f1).num_elts;_tmpC0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97)->f1).zero_term;_tmpBF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97)->f1).zt_loc;_LLC: {
# 501
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC1);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=4U,({void*_tmpA5C=Cyc_Tcutil_copy_type(_tmpC3);(_tmpA0->f1).elt_type=_tmpA5C;}),(_tmpA0->f1).tq=_tmpC2,(_tmpA0->f1).num_elts=eopt2,({
void*_tmpA5B=Cyc_Tcutil_copy_type(_tmpC0);(_tmpA0->f1).zero_term=_tmpA5B;}),(_tmpA0->f1).zt_loc=_tmpBF;_tmpA0;});}case 5U: _LLD: _tmpD0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).tvars;_tmpCF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).effect;_tmpCE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).ret_tqual;_tmpCD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).ret_type;_tmpCC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).args;_tmpCB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).c_varargs;_tmpCA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).cyc_varargs;_tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).rgn_po;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).attributes;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).requires_clause;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).requires_relns;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).ensures_clause;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp97)->f1).ensures_relns;_LLE: {
# 505
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpD0);
void*effopt2=_tmpCF == 0?0: Cyc_Tcutil_copy_type(_tmpCF);
void*_tmpA2=Cyc_Tcutil_copy_type(_tmpCD);
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpCC);
int _tmpA4=_tmpCB;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpCA != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpCA;
({struct Cyc_Absyn_VarargInfo*_tmpA5E=({struct Cyc_Absyn_VarargInfo*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->name=cv->name,_tmpA5->tq=cv->tq,({void*_tmpA5D=Cyc_Tcutil_copy_type(cv->type);_tmpA5->type=_tmpA5D;}),_tmpA5->inject=cv->inject;_tmpA5;});cyc_varargs2=_tmpA5E;});}{
# 516
struct Cyc_List_List*_tmpA6=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpC9);
struct Cyc_List_List*_tmpA7=_tmpC8;
struct Cyc_Absyn_Exp*_tmpA8=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC7);
struct Cyc_List_List*_tmpA9=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC6);
struct Cyc_Absyn_Exp*_tmpAA=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC5);
struct Cyc_List_List*_tmpAB=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpC4);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=5U,(_tmpAC->f1).tvars=_tmpA1,(_tmpAC->f1).effect=effopt2,(_tmpAC->f1).ret_tqual=_tmpCE,(_tmpAC->f1).ret_type=_tmpA2,(_tmpAC->f1).args=_tmpA3,(_tmpAC->f1).c_varargs=_tmpA4,(_tmpAC->f1).cyc_varargs=cyc_varargs2,(_tmpAC->f1).rgn_po=_tmpA6,(_tmpAC->f1).attributes=_tmpA7,(_tmpAC->f1).requires_clause=_tmpA8,(_tmpAC->f1).requires_relns=_tmpA9,(_tmpAC->f1).ensures_clause=_tmpAA,(_tmpAC->f1).ensures_relns=_tmpAB;_tmpAC;});};}case 6U: _LLF: _tmpD1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp97)->f1;_LL10:
# 525
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=6U,({struct Cyc_List_List*_tmpA5F=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpD1);_tmpAD->f1=_tmpA5F;});_tmpAD;});case 7U: _LL11: _tmpD3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp97)->f1;_tmpD2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp97)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=7U,_tmpAE->f1=_tmpD3,({struct Cyc_List_List*_tmpA60=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpD2);_tmpAE->f2=_tmpA60;});_tmpAE;});case 9U: _LL13: _tmpD4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp97)->f1;_LL14:
# 529
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=9U,_tmpAF->f1=_tmpD4;_tmpAF;});case 11U: _LL15: _tmpD5=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp97)->f1;_LL16:
# 532
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=11U,_tmpB0->f1=_tmpD5;_tmpB0;});case 8U: _LL17: _tmpD8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp97)->f1;_tmpD7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp97)->f2;_tmpD6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp97)->f3;_LL18:
# 534
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=8U,_tmpB1->f1=_tmpD8,({struct Cyc_List_List*_tmpA61=Cyc_Tcutil_copy_types(_tmpD7);_tmpB1->f2=_tmpA61;}),_tmpB1->f3=_tmpD6,_tmpB1->f4=0;_tmpB1;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp97)->f1)->r)){case 0U: _LL19: _tmpD9=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp97)->f1)->r)->f1;_LL1A: {
# 537
struct Cyc_List_List*_tmpB2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD9->tvs);
return({union Cyc_Absyn_AggrInfo _tmpA62=Cyc_Absyn_UnknownAggr(_tmpD9->kind,_tmpD9->name,0);Cyc_Absyn_aggr_type(_tmpA62,_tmpB2);});}case 1U: _LL1B: _tmpDA=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp97)->f1)->r)->f1;_LL1C:
# 540
 return Cyc_Absyn_enum_type(_tmpDA->name,0);default: _LL1D: _tmpDB=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp97)->f1)->r)->f1;_LL1E: {
# 542
struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpDB->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmpA63=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp9A1;_tmp9A1.name=_tmpDB->name,_tmp9A1.is_extensible=0;_tmp9A1;}));Cyc_Absyn_datatype_type(_tmpA63,_tmpB3);});}}}_LL0:;}
# 557 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpDC=d;struct _dyneither_ptr*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDC)->tag == 0U){_LL1: _tmpDE=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpDC)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->tag=0U,({struct Cyc_Absyn_Exp*_tmpA64=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpDE);_tmpDD->f1=_tmpA64;});_tmpDD;});}else{_LL3: _tmpDF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpDC)->f1;_LL4:
 return d;}_LL0:;}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 563
static struct _tuple18*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple18*e){
# 565
return({struct _tuple18*_tmpE0=_cycalloc(sizeof(*_tmpE0));({struct Cyc_List_List*_tmpA66=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpE0->f1=_tmpA66;}),({struct Cyc_Absyn_Exp*_tmpA65=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpE0->f2=_tmpA65;});_tmpE0;});}
# 568
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpE1=preserve_types;
{void*_tmpE2=e->r;void*_tmpE3=_tmpE2;int _tmp160;struct _dyneither_ptr _tmp15F;void*_tmp15E;struct Cyc_Absyn_Exp*_tmp15D;struct _dyneither_ptr*_tmp15C;struct Cyc_Core_Opt*_tmp15B;struct Cyc_List_List*_tmp15A;struct Cyc_Absyn_Exp*_tmp159;struct Cyc_Absyn_Exp*_tmp158;int _tmp157;struct Cyc_Absyn_Exp*_tmp156;void**_tmp155;struct Cyc_Absyn_Exp*_tmp154;int _tmp153;int _tmp152;void*_tmp151;struct Cyc_Absyn_Enumfield*_tmp150;struct Cyc_Absyn_Enumdecl*_tmp14F;struct Cyc_Absyn_Enumfield*_tmp14E;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Datatypedecl*_tmp14C;struct Cyc_Absyn_Datatypefield*_tmp14B;void*_tmp14A;struct Cyc_List_List*_tmp149;struct _tuple2*_tmp148;struct Cyc_List_List*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_Absyn_Aggrdecl*_tmp145;struct Cyc_Absyn_Exp*_tmp144;void*_tmp143;int _tmp142;struct Cyc_Absyn_Vardecl*_tmp141;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;int _tmp13E;struct Cyc_List_List*_tmp13D;struct _dyneither_ptr*_tmp13C;struct Cyc_Absyn_Tqual _tmp13B;void*_tmp13A;struct Cyc_List_List*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Exp*_tmp135;struct _dyneither_ptr*_tmp134;int _tmp133;int _tmp132;struct Cyc_Absyn_Exp*_tmp131;struct _dyneither_ptr*_tmp130;int _tmp12F;int _tmp12E;struct Cyc_Absyn_Exp*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;void*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;void*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;void*_tmp124;struct Cyc_Absyn_Exp*_tmp123;int _tmp122;enum Cyc_Absyn_Coercion _tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_List_List*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;int _tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_Absyn_VarargCallInfo*_tmp119;int _tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;enum Cyc_Absyn_Incrementor _tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Core_Opt*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;enum Cyc_Absyn_Primop _tmp109;struct Cyc_List_List*_tmp108;struct _dyneither_ptr _tmp107;void*_tmp106;union Cyc_Absyn_Cnst _tmp105;switch(*((int*)_tmpE3)){case 0U: _LL1: _tmp105=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL2:
({struct Cyc_Absyn_Exp*_tmpA67=Cyc_Absyn_const_exp(_tmp105,e->loc);new_e=_tmpA67;});goto _LL0;case 1U: _LL3: _tmp106=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpA68=Cyc_Absyn_varb_exp(_tmp106,e->loc);new_e=_tmpA68;});goto _LL0;case 2U: _LL5: _tmp107=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpA69=Cyc_Absyn_pragma_exp(_tmp107,e->loc);new_e=_tmpA69;});goto _LL0;case 3U: _LL7: _tmp109=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp108=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL8:
({struct Cyc_Absyn_Exp*_tmpA6C=({enum Cyc_Absyn_Primop _tmpA6B=_tmp109;struct Cyc_List_List*_tmpA6A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE1,_tmp108);Cyc_Absyn_primop_exp(_tmpA6B,_tmpA6A,e->loc);});new_e=_tmpA6C;});goto _LL0;case 4U: _LL9: _tmp10C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp10B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp10A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_LLA:
# 577
({struct Cyc_Absyn_Exp*_tmpA70=({struct Cyc_Absyn_Exp*_tmpA6F=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp10C);struct Cyc_Core_Opt*_tmpA6E=(unsigned int)_tmp10B?({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->v=(void*)_tmp10B->v;_tmpE4;}): 0;struct Cyc_Absyn_Exp*_tmpA6D=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp10A);Cyc_Absyn_assignop_exp(_tmpA6F,_tmpA6E,_tmpA6D,e->loc);});new_e=_tmpA70;});
goto _LL0;case 5U: _LLB: _tmp10E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp10D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LLC:
({struct Cyc_Absyn_Exp*_tmpA73=({struct Cyc_Absyn_Exp*_tmpA72=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp10E);enum Cyc_Absyn_Incrementor _tmpA71=_tmp10D;Cyc_Absyn_increment_exp(_tmpA72,_tmpA71,e->loc);});new_e=_tmpA73;});goto _LL0;case 6U: _LLD: _tmp111=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp110=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp10F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_LLE:
# 581
({struct Cyc_Absyn_Exp*_tmpA77=({struct Cyc_Absyn_Exp*_tmpA76=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp111);struct Cyc_Absyn_Exp*_tmpA75=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp110);struct Cyc_Absyn_Exp*_tmpA74=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp10F);Cyc_Absyn_conditional_exp(_tmpA76,_tmpA75,_tmpA74,e->loc);});new_e=_tmpA77;});goto _LL0;case 7U: _LLF: _tmp113=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp112=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL10:
({struct Cyc_Absyn_Exp*_tmpA7A=({struct Cyc_Absyn_Exp*_tmpA79=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp113);struct Cyc_Absyn_Exp*_tmpA78=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp112);Cyc_Absyn_and_exp(_tmpA79,_tmpA78,e->loc);});new_e=_tmpA7A;});goto _LL0;case 8U: _LL11: _tmp115=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp114=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL12:
({struct Cyc_Absyn_Exp*_tmpA7D=({struct Cyc_Absyn_Exp*_tmpA7C=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp115);struct Cyc_Absyn_Exp*_tmpA7B=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp114);Cyc_Absyn_or_exp(_tmpA7C,_tmpA7B,e->loc);});new_e=_tmpA7D;});goto _LL0;case 9U: _LL13: _tmp117=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp116=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL14:
({struct Cyc_Absyn_Exp*_tmpA80=({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp117);struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp116);Cyc_Absyn_seq_exp(_tmpA7F,_tmpA7E,e->loc);});new_e=_tmpA80;});goto _LL0;case 10U: _LL15: _tmp11B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp11A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp119=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_tmp118=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE3)->f4;_LL16:
# 586
{struct Cyc_Absyn_VarargCallInfo*_tmpE5=_tmp119;int _tmpF1;struct Cyc_List_List*_tmpF0;struct Cyc_Absyn_VarargInfo*_tmpEF;if(_tmpE5 != 0){_LL56: _tmpF1=_tmpE5->num_varargs;_tmpF0=_tmpE5->injectors;_tmpEF=_tmpE5->vai;_LL57: {
# 588
struct Cyc_Absyn_VarargInfo*_tmpE6=_tmpEF;struct _dyneither_ptr*_tmpED;struct Cyc_Absyn_Tqual _tmpEC;void*_tmpEB;int _tmpEA;_LL5B: _tmpED=_tmpE6->name;_tmpEC=_tmpE6->tq;_tmpEB=_tmpE6->type;_tmpEA=_tmpE6->inject;_LL5C:;
({struct Cyc_Absyn_Exp*_tmpA87=({void*_tmpA86=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->tag=10U,({
struct Cyc_Absyn_Exp*_tmpA85=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp11B);_tmpE9->f1=_tmpA85;}),({struct Cyc_List_List*_tmpA84=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE1,_tmp11A);_tmpE9->f2=_tmpA84;}),({
struct Cyc_Absyn_VarargCallInfo*_tmpA83=({struct Cyc_Absyn_VarargCallInfo*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->num_varargs=_tmpF1,_tmpE8->injectors=_tmpF0,({
struct Cyc_Absyn_VarargInfo*_tmpA82=({struct Cyc_Absyn_VarargInfo*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->name=_tmpED,_tmpE7->tq=_tmpEC,({void*_tmpA81=Cyc_Tcutil_copy_type(_tmpEB);_tmpE7->type=_tmpA81;}),_tmpE7->inject=_tmpEA;_tmpE7;});_tmpE8->vai=_tmpA82;});_tmpE8;});
# 591
_tmpE9->f3=_tmpA83;}),_tmpE9->f4=_tmp118;_tmpE9;});
# 589
Cyc_Absyn_new_exp(_tmpA86,e->loc);});new_e=_tmpA87;});
# 594
goto _LL55;}}else{_LL58: _LL59:
# 596
({struct Cyc_Absyn_Exp*_tmpA8B=({void*_tmpA8A=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=10U,({struct Cyc_Absyn_Exp*_tmpA89=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp11B);_tmpEE->f1=_tmpA89;}),({struct Cyc_List_List*_tmpA88=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE1,_tmp11A);_tmpEE->f2=_tmpA88;}),_tmpEE->f3=_tmp119,_tmpEE->f4=_tmp118;_tmpEE;});Cyc_Absyn_new_exp(_tmpA8A,e->loc);});new_e=_tmpA8B;});
goto _LL55;}_LL55:;}
# 599
goto _LL0;case 11U: _LL17: _tmp11D=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp11C=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL18:
# 601
({struct Cyc_Absyn_Exp*_tmpA8E=_tmp11C?({struct Cyc_Absyn_Exp*_tmpA8D=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp11D);Cyc_Absyn_rethrow_exp(_tmpA8D,e->loc);}):({struct Cyc_Absyn_Exp*_tmpA8C=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp11D);Cyc_Absyn_throw_exp(_tmpA8C,e->loc);});new_e=_tmpA8E;});
goto _LL0;case 12U: _LL19: _tmp11E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL1A:
({struct Cyc_Absyn_Exp*_tmpA90=({struct Cyc_Absyn_Exp*_tmpA8F=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp11E);Cyc_Absyn_noinstantiate_exp(_tmpA8F,e->loc);});new_e=_tmpA90;});
goto _LL0;case 13U: _LL1B: _tmp120=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp11F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL1C:
# 606
({struct Cyc_Absyn_Exp*_tmpA93=({struct Cyc_Absyn_Exp*_tmpA92=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp120);struct Cyc_List_List*_tmpA91=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp11F);Cyc_Absyn_instantiate_exp(_tmpA92,_tmpA91,e->loc);});new_e=_tmpA93;});
goto _LL0;case 14U: _LL1D: _tmp124=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp123=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp122=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_tmp121=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE3)->f4;_LL1E:
# 609
({struct Cyc_Absyn_Exp*_tmpA98=({void*_tmpA97=Cyc_Tcutil_copy_type(_tmp124);struct Cyc_Absyn_Exp*_tmpA96=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp123);int _tmpA95=_tmp122;enum Cyc_Absyn_Coercion _tmpA94=_tmp121;Cyc_Absyn_cast_exp(_tmpA97,_tmpA96,_tmpA95,_tmpA94,e->loc);});new_e=_tmpA98;});goto _LL0;case 15U: _LL1F: _tmp125=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL20:
({struct Cyc_Absyn_Exp*_tmpA9A=({struct Cyc_Absyn_Exp*_tmpA99=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp125);Cyc_Absyn_address_exp(_tmpA99,e->loc);});new_e=_tmpA9A;});goto _LL0;case 16U: _LL21: _tmp127=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp126=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL22: {
# 612
struct Cyc_Absyn_Exp*eo1=_tmp127;if(_tmp127 != 0)({struct Cyc_Absyn_Exp*_tmpA9B=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp127);eo1=_tmpA9B;});
({struct Cyc_Absyn_Exp*_tmpA9E=({struct Cyc_Absyn_Exp*_tmpA9D=eo1;struct Cyc_Absyn_Exp*_tmpA9C=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp126);Cyc_Absyn_New_exp(_tmpA9D,_tmpA9C,e->loc);});new_e=_tmpA9E;});
goto _LL0;}case 17U: _LL23: _tmp128=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL24:
({struct Cyc_Absyn_Exp*_tmpAA0=({void*_tmpA9F=Cyc_Tcutil_copy_type(_tmp128);Cyc_Absyn_sizeoftype_exp(_tmpA9F,e->loc);});new_e=_tmpAA0;});
goto _LL0;case 18U: _LL25: _tmp129=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL26:
({struct Cyc_Absyn_Exp*_tmpAA2=({struct Cyc_Absyn_Exp*_tmpAA1=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp129);Cyc_Absyn_sizeofexp_exp(_tmpAA1,e->loc);});new_e=_tmpAA2;});goto _LL0;case 19U: _LL27: _tmp12B=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp12A=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL28:
# 619
({struct Cyc_Absyn_Exp*_tmpAA5=({void*_tmpAA4=Cyc_Tcutil_copy_type(_tmp12B);struct Cyc_List_List*_tmpAA3=_tmp12A;Cyc_Absyn_offsetof_exp(_tmpAA4,_tmpAA3,e->loc);});new_e=_tmpAA5;});goto _LL0;case 20U: _LL29: _tmp12C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL2A:
({struct Cyc_Absyn_Exp*_tmpAA7=({struct Cyc_Absyn_Exp*_tmpAA6=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp12C);Cyc_Absyn_deref_exp(_tmpAA6,e->loc);});new_e=_tmpAA7;});goto _LL0;case 41U: _LL2B: _tmp12D=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL2C:
({struct Cyc_Absyn_Exp*_tmpAA9=({struct Cyc_Absyn_Exp*_tmpAA8=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp12D);Cyc_Absyn_extension_exp(_tmpAA8,e->loc);});new_e=_tmpAA9;});goto _LL0;case 21U: _LL2D: _tmp131=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp130=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp12F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_tmp12E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE3)->f4;_LL2E:
# 623
({struct Cyc_Absyn_Exp*_tmpAAC=({void*_tmpAAB=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->tag=21U,({struct Cyc_Absyn_Exp*_tmpAAA=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp131);_tmpF2->f1=_tmpAAA;}),_tmpF2->f2=_tmp130,_tmpF2->f3=_tmp12F,_tmpF2->f4=_tmp12E;_tmpF2;});Cyc_Absyn_new_exp(_tmpAAB,e->loc);});new_e=_tmpAAC;});goto _LL0;case 22U: _LL2F: _tmp135=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp134=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp133=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_tmp132=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE3)->f4;_LL30:
# 625
({struct Cyc_Absyn_Exp*_tmpAAF=({void*_tmpAAE=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=22U,({struct Cyc_Absyn_Exp*_tmpAAD=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp135);_tmpF3->f1=_tmpAAD;}),_tmpF3->f2=_tmp134,_tmpF3->f3=_tmp133,_tmpF3->f4=_tmp132;_tmpF3;});Cyc_Absyn_new_exp(_tmpAAE,e->loc);});new_e=_tmpAAF;});goto _LL0;case 23U: _LL31: _tmp137=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp136=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL32:
({struct Cyc_Absyn_Exp*_tmpAB2=({struct Cyc_Absyn_Exp*_tmpAB1=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp137);struct Cyc_Absyn_Exp*_tmpAB0=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp136);Cyc_Absyn_subscript_exp(_tmpAB1,_tmpAB0,e->loc);});new_e=_tmpAB2;});
goto _LL0;case 24U: _LL33: _tmp138=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL34:
({struct Cyc_Absyn_Exp*_tmpAB4=({struct Cyc_List_List*_tmpAB3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE1,_tmp138);Cyc_Absyn_tuple_exp(_tmpAB3,e->loc);});new_e=_tmpAB4;});goto _LL0;case 25U: _LL35: _tmp13C=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE3)->f1)->f1;_tmp13B=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE3)->f1)->f2;_tmp13A=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE3)->f1)->f3;_tmp139=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL36: {
# 630
struct _dyneither_ptr*vopt1=_tmp13C;
if(_tmp13C != 0)vopt1=_tmp13C;
({struct Cyc_Absyn_Exp*_tmpAB9=({void*_tmpAB8=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->tag=25U,({struct _tuple10*_tmpAB7=({struct _tuple10*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->f1=vopt1,_tmpF4->f2=_tmp13B,({void*_tmpAB6=Cyc_Tcutil_copy_type(_tmp13A);_tmpF4->f3=_tmpAB6;});_tmpF4;});_tmpF5->f1=_tmpAB7;}),({
struct Cyc_List_List*_tmpAB5=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE1,_tmp139);_tmpF5->f2=_tmpAB5;});_tmpF5;});
# 632
Cyc_Absyn_new_exp(_tmpAB8,e->loc);});new_e=_tmpAB9;});
# 634
goto _LL0;}case 26U: _LL37: _tmp13D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL38:
({struct Cyc_Absyn_Exp*_tmpABC=({void*_tmpABB=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6->tag=26U,({struct Cyc_List_List*_tmpABA=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE1,_tmp13D);_tmpF6->f1=_tmpABA;});_tmpF6;});Cyc_Absyn_new_exp(_tmpABB,e->loc);});new_e=_tmpABC;});
goto _LL0;case 27U: _LL39: _tmp141=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp140=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp13F=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_tmp13E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE3)->f4;_LL3A:
# 638
({struct Cyc_Absyn_Exp*_tmpAC0=({void*_tmpABF=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->tag=27U,_tmpF7->f1=_tmp141,({struct Cyc_Absyn_Exp*_tmpABE=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp140);_tmpF7->f2=_tmpABE;}),({struct Cyc_Absyn_Exp*_tmpABD=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp13F);_tmpF7->f3=_tmpABD;}),_tmpF7->f4=_tmp13E;_tmpF7;});Cyc_Absyn_new_exp(_tmpABF,e->loc);});new_e=_tmpAC0;});
goto _LL0;case 28U: _LL3B: _tmp144=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp143=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp142=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_LL3C:
# 641
({struct Cyc_Absyn_Exp*_tmpAC4=({void*_tmpAC3=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->tag=28U,({struct Cyc_Absyn_Exp*_tmpAC2=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp144);_tmpF8->f1=_tmpAC2;}),({void*_tmpAC1=Cyc_Tcutil_copy_type(_tmp143);_tmpF8->f2=_tmpAC1;}),_tmpF8->f3=_tmp142;_tmpF8;});Cyc_Absyn_new_exp(_tmpAC3,_tmp144->loc);});new_e=_tmpAC4;});
# 643
goto _LL0;case 29U: _LL3D: _tmp148=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp147=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp146=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_tmp145=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE3)->f4;_LL3E:
# 645
({struct Cyc_Absyn_Exp*_tmpAC8=({void*_tmpAC7=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9->tag=29U,_tmpF9->f1=_tmp148,({struct Cyc_List_List*_tmpAC6=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp147);_tmpF9->f2=_tmpAC6;}),({struct Cyc_List_List*_tmpAC5=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE1,_tmp146);_tmpF9->f3=_tmpAC5;}),_tmpF9->f4=_tmp145;_tmpF9;});Cyc_Absyn_new_exp(_tmpAC7,e->loc);});new_e=_tmpAC8;});
# 647
goto _LL0;case 30U: _LL3F: _tmp14A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp149=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL40:
# 649
({struct Cyc_Absyn_Exp*_tmpACC=({void*_tmpACB=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->tag=30U,({void*_tmpACA=Cyc_Tcutil_copy_type(_tmp14A);_tmpFA->f1=_tmpACA;}),({struct Cyc_List_List*_tmpAC9=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE1,_tmp149);_tmpFA->f2=_tmpAC9;});_tmpFA;});Cyc_Absyn_new_exp(_tmpACB,e->loc);});new_e=_tmpACC;});
goto _LL0;case 31U: _LL41: _tmp14D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp14C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_tmp14B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE3)->f3;_LL42:
# 652
({struct Cyc_Absyn_Exp*_tmpACF=({void*_tmpACE=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->tag=31U,({struct Cyc_List_List*_tmpACD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpE1,_tmp14D);_tmpFB->f1=_tmpACD;}),_tmpFB->f2=_tmp14C,_tmpFB->f3=_tmp14B;_tmpFB;});Cyc_Absyn_new_exp(_tmpACE,e->loc);});new_e=_tmpACF;});
goto _LL0;case 32U: _LL43: _tmp14F=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp14E=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL44:
 new_e=e;goto _LL0;case 33U: _LL45: _tmp151=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp150=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL46:
# 656
({struct Cyc_Absyn_Exp*_tmpAD2=({void*_tmpAD1=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=33U,({void*_tmpAD0=Cyc_Tcutil_copy_type(_tmp151);_tmpFC->f1=_tmpAD0;}),_tmpFC->f2=_tmp150;_tmpFC;});Cyc_Absyn_new_exp(_tmpAD1,e->loc);});new_e=_tmpAD2;});
goto _LL0;case 34U: _LL47: _tmp157=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).is_calloc;_tmp156=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).rgn;_tmp155=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).elt_type;_tmp154=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).num_elts;_tmp153=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).fat_result;_tmp152=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE3)->f1).inline_call;_LL48: {
# 659
struct Cyc_Absyn_Exp*_tmpFD=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp156;if(_tmp156 != 0)({struct Cyc_Absyn_Exp*_tmpAD3=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp156);r1=_tmpAD3;});{
void**t1=_tmp155;if(_tmp155 != 0)({void**_tmpAD5=({void**_tmpFE=_cycalloc(sizeof(*_tmpFE));({void*_tmpAD4=Cyc_Tcutil_copy_type(*_tmp155);*_tmpFE=_tmpAD4;});_tmpFE;});t1=_tmpAD5;});
({void*_tmpAD6=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->tag=34U,(_tmpFF->f1).is_calloc=_tmp157,(_tmpFF->f1).rgn=r1,(_tmpFF->f1).elt_type=t1,(_tmpFF->f1).num_elts=_tmp154,(_tmpFF->f1).fat_result=_tmp153,(_tmpFF->f1).inline_call=_tmp152;_tmpFF;});_tmpFD->r=_tmpAD6;});
new_e=_tmpFD;
goto _LL0;};}case 35U: _LL49: _tmp159=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp158=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL4A:
({struct Cyc_Absyn_Exp*_tmpAD9=({struct Cyc_Absyn_Exp*_tmpAD8=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp159);struct Cyc_Absyn_Exp*_tmpAD7=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp158);Cyc_Absyn_swap_exp(_tmpAD8,_tmpAD7,e->loc);});new_e=_tmpAD9;});goto _LL0;case 36U: _LL4B: _tmp15B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp15A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL4C: {
# 667
struct Cyc_Core_Opt*nopt1=_tmp15B;
if(_tmp15B != 0)({struct Cyc_Core_Opt*_tmpADA=({struct Cyc_Core_Opt*_tmp100=_cycalloc(sizeof(*_tmp100));_tmp100->v=(struct _tuple2*)_tmp15B->v;_tmp100;});nopt1=_tmpADA;});
({struct Cyc_Absyn_Exp*_tmpADD=({void*_tmpADC=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101->tag=36U,_tmp101->f1=nopt1,({struct Cyc_List_List*_tmpADB=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(int,struct _tuple18*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpE1,_tmp15A);_tmp101->f2=_tmpADB;});_tmp101;});Cyc_Absyn_new_exp(_tmpADC,e->loc);});new_e=_tmpADD;});
goto _LL0;}case 37U: _LL4D: _LL4E:
# 672
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103->tag=Cyc_Core_Failure,({struct _dyneither_ptr _tmpADE=({const char*_tmp102="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp102,sizeof(char),45U);});_tmp103->f1=_tmpADE;});_tmp103;}));case 38U: _LL4F: _tmp15D=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp15C=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL50:
({struct Cyc_Absyn_Exp*_tmpAE1=({void*_tmpAE0=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=38U,({struct Cyc_Absyn_Exp*_tmpADF=Cyc_Tcutil_deep_copy_exp(_tmpE1,_tmp15D);_tmp104->f1=_tmpADF;}),_tmp104->f2=_tmp15C;_tmp104;});Cyc_Absyn_new_exp(_tmpAE0,e->loc);});new_e=_tmpAE1;});
goto _LL0;case 39U: _LL51: _tmp15E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_LL52:
({struct Cyc_Absyn_Exp*_tmpAE3=({void*_tmpAE2=Cyc_Tcutil_copy_type(_tmp15E);Cyc_Absyn_valueof_exp(_tmpAE2,e->loc);});new_e=_tmpAE3;});
goto _LL0;default: _LL53: _tmp160=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE3)->f1;_tmp15F=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE3)->f2;_LL54:
({struct Cyc_Absyn_Exp*_tmpAE4=Cyc_Absyn_asm_exp(_tmp160,_tmp15F,e->loc);new_e=_tmpAE4;});goto _LL0;}_LL0:;}
# 680
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 684
return new_e;}
# 687
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple19{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple20{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 700 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp161=ka1;enum Cyc_Absyn_KindQual _tmp16A;enum Cyc_Absyn_AliasQual _tmp169;_LL1: _tmp16A=_tmp161->kind;_tmp169=_tmp161->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp162=ka2;enum Cyc_Absyn_KindQual _tmp168;enum Cyc_Absyn_AliasQual _tmp167;_LL4: _tmp168=_tmp162->kind;_tmp167=_tmp162->aliasqual;_LL5:;
# 704
if(_tmp16A != _tmp168){
struct _tuple19 _tmp163=({struct _tuple19 _tmp9A2;_tmp9A2.f1=_tmp16A,_tmp9A2.f2=_tmp168;_tmp9A2;});struct _tuple19 _tmp164=_tmp163;switch(_tmp164.f1){case Cyc_Absyn_BoxKind: switch(_tmp164.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp164.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 713
if(_tmp169 != _tmp167){
struct _tuple20 _tmp165=({struct _tuple20 _tmp9A3;_tmp9A3.f1=_tmp169,_tmp9A3.f2=_tmp167;_tmp9A3;});struct _tuple20 _tmp166=_tmp165;switch(_tmp166.f1){case Cyc_Absyn_Aliasable: if(_tmp166.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp166.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 720
return 1;};}
# 723
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp16B=Cyc_Absyn_compress_kb(tv->kind);void*_tmp16C=_tmp16B;struct Cyc_Absyn_Kind*_tmp16F;struct Cyc_Absyn_Kind*_tmp16E;switch(*((int*)_tmp16C)){case 0U: _LL1: _tmp16E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp16C)->f1;_LL2:
 return _tmp16E;case 2U: _LL3: _tmp16F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp16C)->f2;_LL4:
 return _tmp16F;default: _LL5: _LL6:
# 728
({void*_tmpAE5=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->tag=2U,_tmp16D->f1=0,_tmp16D->f2=def;_tmp16D;});tv->kind=_tmpAE5;});
return def;}_LL0:;}
# 733
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp170=({struct _tuple0 _tmp9A4;({void*_tmpAE7=Cyc_Absyn_compress_kb(kb1);_tmp9A4.f1=_tmpAE7;}),({void*_tmpAE6=Cyc_Absyn_compress_kb(kb2);_tmp9A4.f2=_tmpAE6;});_tmp9A4;});struct _tuple0 _tmp171=_tmp170;struct Cyc_Core_Opt**_tmp186;void*_tmp185;void*_tmp184;struct Cyc_Core_Opt**_tmp183;struct Cyc_Core_Opt**_tmp182;struct Cyc_Absyn_Kind*_tmp181;struct Cyc_Core_Opt**_tmp180;struct Cyc_Absyn_Kind*_tmp17F;struct Cyc_Core_Opt**_tmp17E;struct Cyc_Absyn_Kind*_tmp17D;struct Cyc_Absyn_Kind*_tmp17C;struct Cyc_Absyn_Kind*_tmp17B;struct Cyc_Core_Opt**_tmp17A;struct Cyc_Absyn_Kind*_tmp179;struct Cyc_Absyn_Kind*_tmp178;struct Cyc_Absyn_Kind*_tmp177;switch(*((int*)_tmp171.f1)){case 0U: switch(*((int*)_tmp171.f2)){case 0U: _LL1: _tmp178=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp171.f1)->f1;_tmp177=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp171.f2)->f1;_LL2:
 return _tmp178 == _tmp177;case 2U: _LL5: _tmp17B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp171.f1)->f1;_tmp17A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f2)->f1;_tmp179=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f2)->f2;_LL6:
# 742
 if(Cyc_Tcutil_kind_leq(_tmp17B,_tmp179)){
({struct Cyc_Core_Opt*_tmpAE8=({struct Cyc_Core_Opt*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->v=kb1;_tmp173;});*_tmp17A=_tmpAE8;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp171.f2)){case 0U: _LL3: _tmp17E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f1)->f1;_tmp17D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f1)->f2;_tmp17C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp171.f2)->f1;_LL4:
# 737
 if(Cyc_Tcutil_kind_leq(_tmp17C,_tmp17D)){
({struct Cyc_Core_Opt*_tmpAE9=({struct Cyc_Core_Opt*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->v=kb2;_tmp172;});*_tmp17E=_tmpAE9;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp182=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f1)->f1;_tmp181=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f1)->f2;_tmp180=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f2)->f1;_tmp17F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp171.f2)->f2;_LL8:
# 747
 if(Cyc_Tcutil_kind_leq(_tmp181,_tmp17F)){
({struct Cyc_Core_Opt*_tmpAEA=({struct Cyc_Core_Opt*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->v=kb1;_tmp174;});*_tmp180=_tmpAEA;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp17F,_tmp181)){
({struct Cyc_Core_Opt*_tmpAEB=({struct Cyc_Core_Opt*_tmp175=_cycalloc(sizeof(*_tmp175));_tmp175->v=kb2;_tmp175;});*_tmp182=_tmpAEB;});
return 1;}else{
return 0;}}default: _LLB: _tmp184=_tmp171.f1;_tmp183=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp171.f2)->f1;_LLC:
# 756
({struct Cyc_Core_Opt*_tmpAEC=({struct Cyc_Core_Opt*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->v=_tmp184;_tmp176;});*_tmp183=_tmpAEC;});
return 1;}default: _LL9: _tmp186=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp171.f1)->f1;_tmp185=_tmp171.f2;_LLA:
# 754
 _tmp184=_tmp185;_tmp183=_tmp186;goto _LLC;}_LL0:;}
# 761
struct _tuple15 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp187=Cyc_Tcutil_compress(t);void*_tmp188=_tmp187;struct Cyc_Absyn_Tvar*_tmp18D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp188)->tag == 2U){_LL1: _tmp18D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp188)->f1;_LL2: {
# 764
void*_tmp189=_tmp18D->kind;
_tmp18D->kind=kb;
return({struct _tuple15 _tmp9A5;_tmp9A5.f1=_tmp18D,_tmp9A5.f2=_tmp189;_tmp9A5;});}}else{_LL3: _LL4:
# 768
({struct Cyc_String_pa_PrintArg_struct _tmp18C=({struct Cyc_String_pa_PrintArg_struct _tmp9A6;_tmp9A6.tag=0U,({struct _dyneither_ptr _tmpAED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9A6.f1=_tmpAED;});_tmp9A6;});void*_tmp18A[1U];_tmp18A[0]=& _tmp18C;({struct _dyneither_ptr _tmpAEE=({const char*_tmp18B="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp18B,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAEE,_tag_dyneither(_tmp18A,sizeof(void*),1U));});});}_LL0:;}
# 774
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 776
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 780
struct _RegionHandle _tmp18E=_new_region("temp");struct _RegionHandle*temp=& _tmp18E;_push_region(temp);
{struct Cyc_List_List*_tmp18F=0;
# 783
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp190=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp191=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp192=Cyc_Tcutil_tvar_kind(_tmp190,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp193=_tmp192;switch(((struct Cyc_Absyn_Kind*)_tmp193)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 789
({struct Cyc_List_List*_tmpAF0=({struct Cyc_List_List*_tmp195=_region_malloc(temp,sizeof(*_tmp195));({struct _tuple15*_tmpAEF=({struct _tuple15*_tmp194=_region_malloc(temp,sizeof(*_tmp194));_tmp194->f1=_tmp190,_tmp194->f2=_tmp191;_tmp194;});_tmp195->hd=_tmpAEF;}),_tmp195->tl=_tmp18F;_tmp195;});_tmp18F=_tmpAF0;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 793
if(_tmp18F != 0){
({void*_tmpAF3=({struct _RegionHandle*_tmpAF2=temp;struct Cyc_List_List*_tmpAF1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp18F);Cyc_Tcutil_rsubstitute(_tmpAF2,_tmpAF1,field_type);});field_type=_tmpAF3;});
({struct Cyc_Absyn_Kind*_tmpAF4=Cyc_Tcutil_type_kind(field_type);k=_tmpAF4;});}}
# 781
;_pop_region(temp);}
# 798
return k;}
# 805
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 807
void*_tmp196=Cyc_Tcutil_compress(t);void*_tmp197=_tmp196;struct Cyc_Absyn_Typedefdecl*_tmp1B5;struct Cyc_Absyn_Exp*_tmp1B4;struct Cyc_Absyn_PtrInfo _tmp1B3;void*_tmp1B2;struct Cyc_List_List*_tmp1B1;struct Cyc_Absyn_Tvar*_tmp1B0;struct Cyc_Core_Opt*_tmp1AF;switch(*((int*)_tmp197)){case 1U: _LL1: _tmp1AF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp197)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1AF))->v;case 2U: _LL3: _tmp1B0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp197)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp1B0,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp1B2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f1;_tmp1B1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2;_LL6: {
# 811
void*_tmp198=_tmp1B2;enum Cyc_Absyn_AggrKind _tmp1A5;struct Cyc_List_List*_tmp1A4;struct Cyc_Absyn_AggrdeclImpl*_tmp1A3;int _tmp1A2;struct Cyc_Absyn_Kind*_tmp1A1;enum Cyc_Absyn_Size_of _tmp1A0;switch(*((int*)_tmp198)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp1A0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp198)->f2;_LL21:
 return(_tmp1A0 == Cyc_Absyn_Int_sz  || _tmp1A0 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp1A1=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp198)->f2;_LL31:
 return _tmp1A1;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 832
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 834
({void*_tmp199=0U;({struct _dyneither_ptr _tmpAF5=({const char*_tmp19A="type_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp19A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF5,_tag_dyneither(_tmp199,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp198)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 838
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp1A5=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownAggr).val)->kind;_tmp1A4=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownAggr).val)->tvs;_tmp1A3=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownAggr).val)->impl;_tmp1A2=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp198)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 840
 if(_tmp1A3 == 0){
if(_tmp1A2)
return& Cyc_Tcutil_mk;else{
# 844
return& Cyc_Tcutil_ak;}}{
# 846
struct Cyc_List_List*_tmp19B=_tmp1A3->fields;
if(_tmp19B == 0)return& Cyc_Tcutil_mk;
# 849
if(_tmp1A5 == Cyc_Absyn_StructA){
for(0;((struct Cyc_List_List*)_check_null(_tmp19B))->tl != 0;_tmp19B=_tmp19B->tl){;}{
void*_tmp19C=((struct Cyc_Absyn_Aggrfield*)_tmp19B->hd)->type;
struct Cyc_Absyn_Kind*_tmp19D=Cyc_Tcutil_field_kind(_tmp19C,_tmp1B1,_tmp1A4);
if(_tmp19D == & Cyc_Tcutil_ak  || _tmp19D == & Cyc_Tcutil_tak)return _tmp19D;};}else{
# 857
for(0;_tmp19B != 0;_tmp19B=_tmp19B->tl){
void*_tmp19E=((struct Cyc_Absyn_Aggrfield*)_tmp19B->hd)->type;
struct Cyc_Absyn_Kind*_tmp19F=Cyc_Tcutil_field_kind(_tmp19E,_tmp1B1,_tmp1A4);
if(_tmp19F == & Cyc_Tcutil_ak  || _tmp19F == & Cyc_Tcutil_tak)return _tmp19F;}}
# 863
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 865
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp1B3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp197)->f1;_LLC: {
# 868
void*_tmp1A6=Cyc_Tcutil_compress((_tmp1B3.ptr_atts).bounds);void*_tmp1A7=_tmp1A6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f1)){case 13U: _LL4D: _LL4E: {
# 870
enum Cyc_Absyn_AliasQual _tmp1A8=(Cyc_Tcutil_type_kind((_tmp1B3.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A9=_tmp1A8;switch(_tmp1A9){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;default: _LL58: _LL59:
 return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 876
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 878
enum Cyc_Absyn_AliasQual _tmp1AA=(Cyc_Tcutil_type_kind((_tmp1B3.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1AB=_tmp1AA;switch(_tmp1AB){case Cyc_Absyn_Aliasable: _LL5B: _LL5C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5D: _LL5E:
 return& Cyc_Tcutil_umk;default: _LL5F: _LL60:
 return& Cyc_Tcutil_tmk;}_LL5A:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 884
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 888
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp1B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).num_elts;_LL12:
# 890
 if(_tmp1B4 == 0  || Cyc_Tcutil_is_const_exp(_tmp1B4))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp1B5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp197)->f3;_LL16:
# 894
 if(_tmp1B5 == 0  || _tmp1B5->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1AE=({struct Cyc_String_pa_PrintArg_struct _tmp9A7;_tmp9A7.tag=0U,({struct _dyneither_ptr _tmpAF6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9A7.f1=_tmpAF6;});_tmp9A7;});void*_tmp1AC[1U];_tmp1AC[0]=& _tmp1AE;({struct _dyneither_ptr _tmpAF7=({const char*_tmp1AD="type_kind: typedef found: %s";_tag_dyneither(_tmp1AD,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAF7,_tag_dyneither(_tmp1AC,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B5->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp197)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 903
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 908
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1B6;_push_handler(& _tmp1B6);{int _tmp1B8=0;if(setjmp(_tmp1B6.handler))_tmp1B8=1;if(!_tmp1B8){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1B9=1;_npop_handler(0U);return _tmp1B9;};
# 910
;_pop_handler();}else{void*_tmp1B7=(void*)_exn_thrown;void*_tmp1BA=_tmp1B7;void*_tmp1BB;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1BA)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 912
 return 0;}else{_LL3: _tmp1BB=_tmp1BA;_LL4:(int)_rethrow(_tmp1BB);}_LL0:;}};}
# 917
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
({void*_tmpAF8=Cyc_Tcutil_compress(t);t=_tmpAF8;});{
void*_tmp1BC=t;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1D8;void*_tmp1D7;struct Cyc_Absyn_Tqual _tmp1D6;void*_tmp1D5;struct Cyc_List_List*_tmp1D4;int _tmp1D3;struct Cyc_Absyn_VarargInfo*_tmp1D2;struct Cyc_List_List*_tmp1D1;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_Exp*_tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Exp*_tmp1CD;struct Cyc_List_List*_tmp1CC;void*_tmp1CB;void*_tmp1CA;struct Cyc_Absyn_PtrInfo _tmp1C9;void*_tmp1C8;struct Cyc_Core_Opt**_tmp1C7;struct Cyc_Absyn_Tvar*_tmp1C6;switch(*((int*)_tmp1BC)){case 2U: _LL1: _tmp1C6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1BC)->f1;_LL2:
# 922
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1C6)){
({struct _dyneither_ptr _tmpAF9=({const char*_tmp1BD="(type variable would escape scope)";_tag_dyneither(_tmp1BD,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpAF9;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 926
goto _LL0;case 1U: _LL3: _tmp1C8=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f2;_tmp1C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f4;_LL4:
# 928
 if(t == evar){
({struct _dyneither_ptr _tmpAFA=({const char*_tmp1BE="(occurs check)";_tag_dyneither(_tmp1BE,sizeof(char),15U);});Cyc_Tcutil_failure_reason=_tmpAFA;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 932
if(_tmp1C8 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1C8);else{
# 935
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C7))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 942
if(problem){
struct Cyc_List_List*_tmp1BF=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1C7))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
({struct Cyc_List_List*_tmpAFB=({struct Cyc_List_List*_tmp1C0=_cycalloc(sizeof(*_tmp1C0));_tmp1C0->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp1C0->tl=_tmp1BF;_tmp1C0;});_tmp1BF=_tmpAFB;});}}
# 948
({struct Cyc_Core_Opt*_tmpAFC=({struct Cyc_Core_Opt*_tmp1C1=_cycalloc(sizeof(*_tmp1C1));_tmp1C1->v=_tmp1BF;_tmp1C1;});*_tmp1C7=_tmpAFC;});}}}
# 951
goto _LL0;case 3U: _LL5: _tmp1C9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BC)->f1;_LL6:
# 953
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C9.elt_type);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C9.ptr_atts).rgn);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C9.ptr_atts).nullable);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C9.ptr_atts).bounds);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C9.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp1CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BC)->f1).elt_type;_tmp1CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1BC)->f1).zero_term;_LL8:
# 961
 Cyc_Tcutil_occurs(evar,r,env,_tmp1CB);
Cyc_Tcutil_occurs(evar,r,env,_tmp1CA);
goto _LL0;case 5U: _LL9: _tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).tvars;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).effect;_tmp1D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).ret_tqual;_tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).ret_type;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).args;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).c_varargs;_tmp1D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).cyc_varargs;_tmp1D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).rgn_po;_tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).attributes;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).requires_clause;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).requires_relns;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).ensures_clause;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1BC)->f1).ensures_relns;_LLA:
# 966
({struct Cyc_List_List*_tmpAFD=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1D8,env);env=_tmpAFD;});
if(_tmp1D7 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D7);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D5);
for(0;_tmp1D4 != 0;_tmp1D4=_tmp1D4->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1D4->hd)).f3);}
if(_tmp1D2 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1D2->type);
for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){
struct _tuple0*_tmp1C2=(struct _tuple0*)_tmp1D1->hd;struct _tuple0*_tmp1C3=_tmp1C2;void*_tmp1C5;void*_tmp1C4;_LL16: _tmp1C5=_tmp1C3->f1;_tmp1C4=_tmp1C3->f2;_LL17:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1C5);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C4);}
# 978
goto _LL0;case 6U: _LLB: _tmp1D9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1BC)->f1;_LLC:
# 980
 for(0;_tmp1D9 != 0;_tmp1D9=_tmp1D9->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1D9->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp1DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1BC)->f2;_LLE:
# 985
 for(0;_tmp1DA != 0;_tmp1DA=_tmp1DA->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1DA->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp1DB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1BC)->f2;_LL10:
 _tmp1DC=_tmp1DB;goto _LL12;case 0U: _LL11: _tmp1DC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2;_LL12:
# 990
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1DC);goto _LL0;default: _LL13: _LL14:
# 993
 goto _LL0;}_LL0:;};}
# 996
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 998
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 1003
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1011
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1DD=0U;({struct _dyneither_ptr _tmpAFE=({const char*_tmp1DE="tq1 real_const not set.";_tag_dyneither(_tmp1DE,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAFE,_tag_dyneither(_tmp1DD,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1DF=0U;({struct _dyneither_ptr _tmpAFF=({const char*_tmp1E0="tq2 real_const not set.";_tag_dyneither(_tmp1E0,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAFF,_tag_dyneither(_tmp1DF,sizeof(void*),0U));});});
# 1017
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 1020
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpB00=({const char*_tmp1E1="(qualifiers don't match)";_tag_dyneither(_tmp1E1,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB00;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1028
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 1032
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 1038
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp1E2=({struct _tuple0 _tmp9A8;_tmp9A8.f1=e1->r,_tmp9A8.f2=e2->r;_tmp9A8;});struct _tuple0 _tmp1E3=_tmp1E2;void*_tmp1ED;void*_tmp1EC;struct Cyc_Absyn_Exp*_tmp1EB;struct Cyc_Absyn_Exp*_tmp1EA;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E3.f1)->tag == 14U){_LL1: _tmp1EA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E3.f1)->f2;_LL2:
# 1043
 Cyc_Tcutil_unify_cmp_exp(_tmp1EA,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E3.f2)->tag == 14U){_LL3: _tmp1EB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1E3.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1EB);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E3.f1)->tag == 39U){_LL5: _tmp1EC=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E3.f1)->f1;_LL6: {
# 1046
void*_tmp1E4=Cyc_Tcutil_compress(_tmp1EC);void*_tmp1E5=_tmp1E4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E5)->tag == 1U){_LLC: _LLD:
({void*_tmpB01=_tmp1EC;Cyc_Tcutil_unify_it(_tmpB01,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));_tmp1E6->tag=9U,_tmp1E6->f1=e2;_tmp1E6;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E3.f2)->tag == 39U){_LL7: _tmp1ED=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1E3.f2)->f1;_LL8: {
# 1051
void*_tmp1E7=Cyc_Tcutil_compress(_tmp1ED);void*_tmp1E8=_tmp1E7;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E8)->tag == 1U){_LL11: _LL12:
({void*_tmpB02=_tmp1ED;Cyc_Tcutil_unify_it(_tmpB02,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->tag=9U,_tmp1E9->f1=e1;_tmp1E9;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 1055
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 1059
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp1EE=att;switch(*((int*)_tmp1EE)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 9U: _LL13: _LL14:
 return 9;case 10U: _LL15: _LL16:
 return 10;case 11U: _LL17: _LL18:
 return 11;case 12U: _LL19: _LL1A:
 return 12;case 13U: _LL1B: _LL1C:
 return 13;case 14U: _LL1D: _LL1E:
 return 14;case 15U: _LL1F: _LL20:
 return 15;case 16U: _LL21: _LL22:
 return 16;case 17U: _LL23: _LL24:
 return 17;case 18U: _LL25: _LL26:
 return 18;case 19U: _LL27: _LL28:
 return 19;case 20U: _LL29: _LL2A:
 return 20;default: _LL2B: _LL2C:
 return 21;}_LL0:;}
# 1086
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp1EF=({struct _tuple0 _tmp9A9;_tmp9A9.f1=att1,_tmp9A9.f2=att2;_tmp9A9;});struct _tuple0 _tmp1F0=_tmp1EF;enum Cyc_Absyn_Format_Type _tmp200;int _tmp1FF;int _tmp1FE;enum Cyc_Absyn_Format_Type _tmp1FD;int _tmp1FC;int _tmp1FB;struct _dyneither_ptr _tmp1FA;struct _dyneither_ptr _tmp1F9;struct Cyc_Absyn_Exp*_tmp1F8;struct Cyc_Absyn_Exp*_tmp1F7;int _tmp1F6;int _tmp1F5;int _tmp1F4;int _tmp1F3;switch(*((int*)_tmp1F0.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1F0.f2)->tag == 0U){_LL1: _tmp1F4=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1F0.f1)->f1;_tmp1F3=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp1F0.f2)->f1;_LL2:
 _tmp1F6=_tmp1F4;_tmp1F5=_tmp1F3;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1F0.f2)->tag == 20U){_LL3: _tmp1F6=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1F0.f1)->f1;_tmp1F5=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp1F0.f2)->f1;_LL4:
# 1090
 return Cyc_Core_intcmp(_tmp1F6,_tmp1F5);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1F0.f2)->tag == 6U){_LL5: _tmp1F8=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1F0.f1)->f1;_tmp1F7=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp1F0.f2)->f1;_LL6:
# 1092
 if(_tmp1F8 == _tmp1F7)return 0;
if(_tmp1F8 == 0)return - 1;
if(_tmp1F7 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp1F8,_tmp1F7);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1F0.f2)->tag == 8U){_LL7: _tmp1FA=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1F0.f1)->f1;_tmp1F9=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp1F0.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp1FA,(struct _dyneither_ptr)_tmp1F9);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F0.f2)->tag == 19U){_LL9: _tmp200=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F0.f1)->f1;_tmp1FF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F0.f1)->f2;_tmp1FE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F0.f1)->f3;_tmp1FD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F0.f2)->f1;_tmp1FC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F0.f2)->f2;_tmp1FB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1F0.f2)->f3;_LLA: {
# 1098
int _tmp1F1=Cyc_Core_intcmp((int)((unsigned int)_tmp200),(int)((unsigned int)_tmp1FD));
if(_tmp1F1 != 0)return _tmp1F1;{
int _tmp1F2=Cyc_Core_intcmp(_tmp1FF,_tmp1FC);
if(_tmp1F2 != 0)return _tmp1F2;
return Cyc_Core_intcmp(_tmp1FE,_tmp1FB);};}}else{goto _LLB;}default: _LLB: _LLC:
# 1104
 return({int _tmpB03=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpB03,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
# 1108
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 1112
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 1121
static void*Cyc_Tcutil_rgns_of(void*t);
# 1123
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 1127
static struct _tuple15*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp201=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp202=_tmp201;switch(((struct Cyc_Absyn_Kind*)_tmp202)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp202)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
({void*_tmpB05=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp203=_cycalloc(sizeof(*_tmp203));_tmp203->tag=9U,({struct Cyc_Absyn_Exp*_tmpB04=Cyc_Absyn_uint_exp(0U,0U);_tmp203->f1=_tmpB04;});_tmp203;});t=_tmpB05;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 1138
return({struct _tuple15*_tmp204=_cycalloc(sizeof(*_tmp204));_tmp204->f1=tv,_tmp204->f2=t;_tmp204;});}
# 1145
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp205=Cyc_Tcutil_compress(t);void*_tmp206=_tmp205;struct Cyc_List_List*_tmp21C;struct Cyc_List_List*_tmp21B;struct Cyc_List_List*_tmp21A;void*_tmp219;struct Cyc_Absyn_Tqual _tmp218;void*_tmp217;struct Cyc_List_List*_tmp216;struct Cyc_Absyn_VarargInfo*_tmp215;struct Cyc_List_List*_tmp214;struct Cyc_List_List*_tmp213;void*_tmp212;void*_tmp211;void*_tmp210;struct Cyc_List_List*_tmp20F;switch(*((int*)_tmp206)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp206)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp206)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp20F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp206)->f2;_LL6: {
# 1150
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp20F);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 1154
struct Cyc_Absyn_Kind*_tmp207=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp208=_tmp207;switch(((struct Cyc_Absyn_Kind*)_tmp208)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp206)->f1).elt_type;_tmp210=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp206)->f1).ptr_atts).rgn;_LLC:
# 1162
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp209[2U];({void*_tmpB07=Cyc_Absyn_access_eff(_tmp210);_tmp209[0]=_tmpB07;}),({void*_tmpB06=Cyc_Tcutil_rgns_of(_tmp211);_tmp209[1]=_tmpB06;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp209,sizeof(void*),2U));})));case 4U: _LLD: _tmp212=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp206)->f1).elt_type;_LLE:
# 1164
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp212));case 7U: _LLF: _tmp213=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp206)->f2;_LL10:
# 1166
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp213)));case 5U: _LL11: _tmp21A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1).tvars;_tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1).effect;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1).ret_tqual;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1).ret_type;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1).args;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1).cyc_varargs;_tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp206)->f1).rgn_po;_LL12: {
# 1175
void*_tmp20A=({struct Cyc_List_List*_tmpB08=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp21A);Cyc_Tcutil_substitute(_tmpB08,(void*)_check_null(_tmp219));});
return Cyc_Tcutil_normalize_effect(_tmp20A);}case 6U: _LL13: _tmp21B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp206)->f1;_LL14: {
# 1178
struct Cyc_List_List*_tmp20B=0;
for(0;_tmp21B != 0;_tmp21B=_tmp21B->tl){
({struct Cyc_List_List*_tmpB09=({struct Cyc_List_List*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C->hd=(*((struct _tuple12*)_tmp21B->hd)).f2,_tmp20C->tl=_tmp20B;_tmp20C;});_tmp20B=_tmpB09;});}
_tmp21C=_tmp20B;goto _LL16;}case 8U: _LL15: _tmp21C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp206)->f2;_LL16:
# 1183
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp21C)));case 10U: _LL17: _LL18:
({void*_tmp20D=0U;({struct _dyneither_ptr _tmpB0A=({const char*_tmp20E="typedecl in rgns_of";_tag_dyneither(_tmp20E,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB0A,_tag_dyneither(_tmp20D,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 1193
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpB0B=Cyc_Tcutil_compress(e);e=_tmpB0B;});{
void*_tmp21D=e;void*_tmp229;struct Cyc_List_List**_tmp228;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f1)){case 9U: _LL1: _tmp228=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2;_LL2: {
# 1197
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp228;for(0;effs != 0;effs=effs->tl){
void*_tmp21E=(void*)effs->hd;
({void*_tmpB0C=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp21E));effs->hd=_tmpB0C;});{
void*_tmp21F=(void*)effs->hd;void*_tmp220=_tmp21F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp220)->f2)->tl == 0){_LLE: _LLF:
# 1206
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1210
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp228;for(0;effs != 0;effs=effs->tl){
void*_tmp221=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp222=_tmp221;void*_tmp225;struct Cyc_List_List*_tmp224;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f1)){case 9U: _LL13: _tmp224=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2;_LL14:
# 1215
({struct Cyc_List_List*_tmpB0D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp224,effects);effects=_tmpB0D;});
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp222)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp225=_tmp222;_LL1C:
({struct Cyc_List_List*_tmpB0E=({struct Cyc_List_List*_tmp223=_cycalloc(sizeof(*_tmp223));_tmp223->hd=_tmp225,_tmp223->tl=effects;_tmp223;});effects=_tmpB0E;});goto _LL12;}_LL12:;}}
# 1223
({struct Cyc_List_List*_tmpB0F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp228=_tmpB0F;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2 != 0){_LL3: _tmp229=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp21D)->f2)->hd;_LL4: {
# 1226
void*_tmp226=Cyc_Tcutil_compress(_tmp229);void*_tmp227=_tmp226;switch(*((int*)_tmp227)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp229);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 1231
 return e;}_LL0:;};}
# 1236
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22A=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp22B=_cycalloc(sizeof(*_tmp22B));_tmp22B->tag=5U,(_tmp22B->f1).tvars=0,(_tmp22B->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmpB10=Cyc_Absyn_empty_tqual(0U);(_tmp22B->f1).ret_tqual=_tmpB10;}),(_tmp22B->f1).ret_type=Cyc_Absyn_void_type,(_tmp22B->f1).args=0,(_tmp22B->f1).c_varargs=0,(_tmp22B->f1).cyc_varargs=0,(_tmp22B->f1).rgn_po=0,(_tmp22B->f1).attributes=0,(_tmp22B->f1).requires_clause=0,(_tmp22B->f1).requires_relns=0,(_tmp22B->f1).ensures_clause=0,(_tmp22B->f1).ensures_relns=0;_tmp22B;});
# 1246
return({void*_tmpB14=(void*)_tmp22A;void*_tmpB13=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmpB12=Cyc_Absyn_empty_tqual(0U);void*_tmpB11=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmpB14,_tmpB13,_tmpB12,_tmpB11,Cyc_Absyn_false_type);});}
# 1253
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpB15=Cyc_Tcutil_compress(r);r=_tmpB15;});
if((r == Cyc_Absyn_heap_rgn_type  || r == Cyc_Absyn_unique_rgn_type) || r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp22C=Cyc_Tcutil_compress(e);void*_tmp22D=_tmp22C;struct Cyc_Core_Opt*_tmp248;void**_tmp247;struct Cyc_Core_Opt*_tmp246;void*_tmp245;struct Cyc_List_List*_tmp244;void*_tmp243;switch(*((int*)_tmp22D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f2 != 0){_LL1: _tmp243=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f2)->hd;_LL2:
# 1260
 if(constrain)return Cyc_Tcutil_unify(r,_tmp243);
({void*_tmpB16=Cyc_Tcutil_compress(_tmp243);_tmp243=_tmpB16;});
if(r == _tmp243)return 1;{
struct _tuple0 _tmp22E=({struct _tuple0 _tmp9AA;_tmp9AA.f1=r,_tmp9AA.f2=_tmp243;_tmp9AA;});struct _tuple0 _tmp22F=_tmp22E;struct Cyc_Absyn_Tvar*_tmp231;struct Cyc_Absyn_Tvar*_tmp230;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22F.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22F.f2)->tag == 2U){_LLC: _tmp231=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22F.f1)->f1;_tmp230=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp22F.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp231,_tmp230)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp244=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f2;_LL4:
# 1268
 for(0;_tmp244 != 0;_tmp244=_tmp244->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp244->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f2 != 0){_LL5: _tmp245=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22D)->f2)->hd;_LL6: {
# 1272
void*_tmp232=Cyc_Tcutil_rgns_of(_tmp245);void*_tmp233=_tmp232;void*_tmp23D;void*_tmp23C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp233)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp233)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp233)->f2 != 0){_LL11: _tmp23C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp233)->f2)->hd;_LL12:
# 1274
 if(!constrain)return 0;{
void*_tmp234=Cyc_Tcutil_compress(_tmp23C);void*_tmp235=_tmp234;struct Cyc_Core_Opt*_tmp23B;void**_tmp23A;struct Cyc_Core_Opt*_tmp239;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp235)->tag == 1U){_LL16: _tmp23B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp235)->f1;_tmp23A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp235)->f2;_tmp239=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp235)->f4;_LL17: {
# 1279
void*_tmp236=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp239);
# 1282
Cyc_Tcutil_occurs(_tmp236,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp239))->v,r);{
void*_tmp237=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp238[2U];_tmp238[0]=_tmp236,({void*_tmpB17=Cyc_Absyn_access_eff(r);_tmp238[1]=_tmpB17;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp238,sizeof(void*),2U));})));
*_tmp23A=_tmp237;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp23D=_tmp233;_LL14:
# 1288
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp23D);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp248=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22D)->f1;_tmp247=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22D)->f2;_tmp246=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22D)->f4;_LL8:
# 1291
 if(_tmp248 == 0  || ((struct Cyc_Absyn_Kind*)_tmp248->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp23E=0U;({struct _dyneither_ptr _tmpB18=({const char*_tmp23F="effect evar has wrong kind";_tag_dyneither(_tmp23F,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB18,_tag_dyneither(_tmp23E,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1296
void*_tmp240=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp246);
# 1299
Cyc_Tcutil_occurs(_tmp240,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp246))->v,r);{
void*_tmp241=Cyc_Absyn_join_eff(({void*_tmp242[2U];_tmp242[0]=_tmp240,({void*_tmpB19=Cyc_Absyn_access_eff(r);_tmp242[1]=_tmpB19;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp242,sizeof(void*),2U));}));
*_tmp247=_tmp241;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1310
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpB1A=Cyc_Tcutil_compress(t);t=_tmpB1A;});{
void*_tmp249=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp24A=_tmp249;struct Cyc_Core_Opt*_tmp25C;void**_tmp25B;struct Cyc_Core_Opt*_tmp25A;void*_tmp259;struct Cyc_List_List*_tmp258;switch(*((int*)_tmp24A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp258=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f2;_LL4:
# 1315
 for(0;_tmp258 != 0;_tmp258=_tmp258->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp258->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f2 != 0){_LL5: _tmp259=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A)->f2)->hd;_LL6:
# 1320
({void*_tmpB1B=Cyc_Tcutil_compress(_tmp259);_tmp259=_tmpB1B;});
if(t == _tmp259)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp259);{
void*_tmp24B=Cyc_Tcutil_rgns_of(t);void*_tmp24C=_tmp24B;void*_tmp252;void*_tmp251;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f2 != 0){_LLC: _tmp251=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C)->f2)->hd;_LLD: {
# 1325
struct _tuple0 _tmp24D=({struct _tuple0 _tmp9AB;({void*_tmpB1C=Cyc_Tcutil_compress(_tmp251);_tmp9AB.f1=_tmpB1C;}),_tmp9AB.f2=_tmp259;_tmp9AB;});struct _tuple0 _tmp24E=_tmp24D;struct Cyc_Absyn_Tvar*_tmp250;struct Cyc_Absyn_Tvar*_tmp24F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24E.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24E.f2)->tag == 2U){_LL11: _tmp250=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp24F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24E.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp259);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp251 == _tmp259;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp252=_tmp24C;_LLF:
# 1329
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp252);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp25C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24A)->f1;_tmp25B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24A)->f2;_tmp25A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24A)->f4;_LL8:
# 1332
 if(_tmp25C == 0  || ((struct Cyc_Absyn_Kind*)_tmp25C->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp253=0U;({struct _dyneither_ptr _tmpB1D=({const char*_tmp254="effect evar has wrong kind";_tag_dyneither(_tmp254,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB1D,_tag_dyneither(_tmp253,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1337
void*_tmp255=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp25A);
# 1340
Cyc_Tcutil_occurs(_tmp255,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp25A))->v,t);{
void*_tmp256=Cyc_Absyn_join_eff(({void*_tmp257[2U];_tmp257[0]=_tmp255,({void*_tmpB1E=Cyc_Absyn_regionsof_eff(t);_tmp257[1]=_tmpB1E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp257,sizeof(void*),2U));}));
*_tmp25B=_tmp256;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1351
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpB1F=Cyc_Tcutil_compress(e);e=_tmpB1F;});{
void*_tmp25D=e;struct Cyc_Core_Opt*_tmp274;void**_tmp273;struct Cyc_Core_Opt*_tmp272;void*_tmp271;struct Cyc_List_List*_tmp270;struct Cyc_Absyn_Tvar*_tmp26F;switch(*((int*)_tmp25D)){case 2U: _LL1: _tmp26F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp25D)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp26F)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f1)){case 9U: _LL3: _tmp270=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f2;_LL4:
# 1356
 for(0;_tmp270 != 0;_tmp270=_tmp270->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp270->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f2 != 0){_LL5: _tmp271=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25D)->f2)->hd;_LL6: {
# 1361
void*_tmp25E=Cyc_Tcutil_rgns_of(_tmp271);void*_tmp25F=_tmp25E;void*_tmp269;void*_tmp268;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2 != 0){_LLC: _tmp268=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25F)->f2)->hd;_LLD:
# 1363
 if(!may_constrain_evars)return 0;{
void*_tmp260=Cyc_Tcutil_compress(_tmp268);void*_tmp261=_tmp260;struct Cyc_Core_Opt*_tmp267;void**_tmp266;struct Cyc_Core_Opt*_tmp265;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp261)->tag == 1U){_LL11: _tmp267=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp261)->f1;_tmp266=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp261)->f2;_tmp265=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp261)->f4;_LL12: {
# 1369
void*_tmp262=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp265);
# 1371
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp265))->v,v))return 0;{
void*_tmp263=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp264[2U];_tmp264[0]=_tmp262,({void*_tmpB20=Cyc_Absyn_var_type(v);_tmp264[1]=_tmpB20;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp264,sizeof(void*),2U));})));
*_tmp266=_tmp263;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp269=_tmp25F;_LLF:
# 1377
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp269);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp274=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25D)->f1;_tmp273=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25D)->f2;_tmp272=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25D)->f4;_LL8:
# 1380
 if(_tmp274 == 0  || ((struct Cyc_Absyn_Kind*)_tmp274->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp26A=0U;({struct _dyneither_ptr _tmpB21=({const char*_tmp26B="effect evar has wrong kind";_tag_dyneither(_tmp26B,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB21,_tag_dyneither(_tmp26A,sizeof(void*),0U));});});{
# 1384
void*_tmp26C=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp272);
# 1386
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp272))->v,v))
return 0;{
void*_tmp26D=Cyc_Absyn_join_eff(({void*_tmp26E[2U];_tmp26E[0]=_tmp26C,({void*_tmpB22=Cyc_Absyn_var_type(v);_tmp26E[1]=_tmpB22;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp26E,sizeof(void*),2U));}));
*_tmp273=_tmp26D;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1396
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpB23=Cyc_Tcutil_compress(e);e=_tmpB23;});{
void*_tmp275=e;void*_tmp27B;struct Cyc_List_List*_tmp27A;switch(*((int*)_tmp275)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f1)){case 9U: _LL1: _tmp27A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f2;_LL2:
# 1400
 for(0;_tmp27A != 0;_tmp27A=_tmp27A->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp27A->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f2 != 0){_LL3: _tmp27B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp275)->f2)->hd;_LL4: {
# 1405
void*_tmp276=Cyc_Tcutil_rgns_of(_tmp27B);void*_tmp277=_tmp276;void*_tmp279;void*_tmp278;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->f2 != 0){_LLA: _tmp278=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp277)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp279=_tmp277;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp279);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1409
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1423 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1428
void*_tmp27C=Cyc_Tcutil_compress(e1);void*_tmp27D=_tmp27C;void**_tmp28A;struct Cyc_Core_Opt*_tmp289;struct Cyc_Absyn_Tvar*_tmp288;void*_tmp287;void*_tmp286;struct Cyc_List_List*_tmp285;switch(*((int*)_tmp27D)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f1)){case 9U: _LL1: _tmp285=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2;_LL2:
# 1430
 for(0;_tmp285 != 0;_tmp285=_tmp285->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp285->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2 != 0){_LL3: _tmp286=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2)->hd;_LL4:
# 1441
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp286,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp286,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2 != 0){_LL7: _tmp287=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27D)->f2)->hd;_LL8: {
# 1445
void*_tmp27E=Cyc_Tcutil_rgns_of(_tmp287);void*_tmp27F=_tmp27E;void*_tmp281;void*_tmp280;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f2 != 0){_LLE: _tmp280=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27F)->f2)->hd;_LLF:
# 1450
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp280,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp280,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp281=_tmp27F;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp281,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp288=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp27D)->f1;_LL6:
# 1443
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp288,e2);case 1U: _LL9: _tmp28A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27D)->f2;_tmp289=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27D)->f4;_LLA:
# 1455
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1459
*_tmp28A=Cyc_Absyn_empty_effect;
# 1462
return 1;}else{
# 1464
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp284=({struct Cyc_String_pa_PrintArg_struct _tmp9AC;_tmp9AC.tag=0U,({struct _dyneither_ptr _tmpB24=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp9AC.f1=_tmpB24;});_tmp9AC;});void*_tmp282[1U];_tmp282[0]=& _tmp284;({struct _dyneither_ptr _tmpB25=({const char*_tmp283="subset_effect: bad effect: %s";_tag_dyneither(_tmp283,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB25,_tag_dyneither(_tmp282,sizeof(void*),1U));});});}_LL0:;}
# 1480 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpB26=Cyc_Tcutil_normalize_effect(e1);e1=_tmpB26;});
({void*_tmpB27=Cyc_Tcutil_normalize_effect(e2);e2=_tmpB27;});
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1496
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1498
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp28B=(struct _tuple0*)r1->hd;struct _tuple0*_tmp28C=_tmp28B;void*_tmp292;void*_tmp291;_LL1: _tmp292=_tmp28C->f1;_tmp291=_tmp28C->f2;_LL2:;{
int found=_tmp292 == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp28D=(struct _tuple0*)r2->hd;struct _tuple0*_tmp28E=_tmp28D;void*_tmp290;void*_tmp28F;_LL4: _tmp290=_tmp28E->f1;_tmp28F=_tmp28E->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp292,_tmp290) && Cyc_Tcutil_unify(_tmp291,_tmp28F)){
found=1;
break;}}}
# 1508
if(!found)return 0;};}}
# 1510
return 1;}
# 1517
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp293=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp294=({struct Cyc_List_List*_tmp295=_cycalloc(sizeof(*_tmp295));_tmp295->hd=_tmp293,_tmp295->tl=r1;_tmp295;});
if(Cyc_Relations_consistent_relations(_tmp294))return 0;}
# 1523
return 1;}
# 1528
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1534
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1536
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1539
int Cyc_Tcutil_tycon2int(void*t){
void*_tmp296=t;switch(*((int*)_tmp296)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1545
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1548
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1551
 return 7;default: _LL1B: _LL1C:
# 1554
 return 13;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1543
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1546
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1549
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1552
 return 8;default: _LL1D: _LL1E:
# 1555
 return 14;}default: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp296)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1544
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1547
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1550
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1553
 return 9;default: _LL1F: _LL20:
# 1556
 return 15;}}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp296)->f1){case 0U: _LL21: _LL22:
 return 16;case 1U: _LL23: _LL24:
 return 17;default: _LL25: _LL26:
 return 18;}case 3U: _LL27: _LL28:
 return 19;case 4U: _LL29: _LL2A:
 return 20;case 5U: _LL2B: _LL2C:
 return 21;case 6U: _LL2D: _LL2E:
 return 22;case 7U: _LL2F: _LL30:
 return 23;case 8U: _LL31: _LL32:
 return 24;case 9U: _LL33: _LL34:
 return 25;case 10U: _LL35: _LL36:
 return 26;case 11U: _LL37: _LL38:
 return 27;case 12U: _LL39: _LL3A:
 return 28;case 14U: _LL3B: _LL3C:
 return 29;case 13U: _LL3D: _LL3E:
 return 30;case 15U: _LL3F: _LL40:
 return 31;case 16U: _LL41: _LL42:
 return 32;case 17U: _LL43: _LL44:
 return 33;case 18U: _LL45: _LL46:
 return 34;case 19U: _LL47: _LL48:
 return 35;default: _LL49: _LL4A:
 return 36;}_LL0:;}
# 1581
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);
# 1583
struct _tuple2*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp297=i;struct _tuple2*_tmp299;struct Cyc_Absyn_Datatypedecl*_tmp298;if((_tmp297.KnownDatatype).tag == 2){_LL1: _tmp298=*(_tmp297.KnownDatatype).val;_LL2:
 return _tmp298->name;}else{_LL3: _tmp299=((_tmp297.UnknownDatatype).val).name;_LL4:
 return _tmp299;}_LL0:;}struct _tuple21{struct _tuple2*f1;struct _tuple2*f2;};
# 1590
struct _tuple21 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp29A=i;struct _tuple2*_tmp29E;struct _tuple2*_tmp29D;struct Cyc_Absyn_Datatypedecl*_tmp29C;struct Cyc_Absyn_Datatypefield*_tmp29B;if((_tmp29A.KnownDatatypefield).tag == 2){_LL1: _tmp29C=((_tmp29A.KnownDatatypefield).val).f1;_tmp29B=((_tmp29A.KnownDatatypefield).val).f2;_LL2:
# 1593
 return({struct _tuple21 _tmp9AD;_tmp9AD.f1=_tmp29C->name,_tmp9AD.f2=_tmp29B->name;_tmp9AD;});}else{_LL3: _tmp29E=((_tmp29A.UnknownDatatypefield).val).datatype_name;_tmp29D=((_tmp29A.UnknownDatatypefield).val).field_name;_LL4:
# 1595
 return({struct _tuple21 _tmp9AE;_tmp9AE.f1=_tmp29E,_tmp9AE.f2=_tmp29D;_tmp9AE;});}_LL0:;}struct _tuple22{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1599
struct _tuple22 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp29F=i;struct Cyc_Absyn_Aggrdecl*_tmp2A2;enum Cyc_Absyn_AggrKind _tmp2A1;struct _tuple2*_tmp2A0;if((_tmp29F.UnknownAggr).tag == 1){_LL1: _tmp2A1=((_tmp29F.UnknownAggr).val).f1;_tmp2A0=((_tmp29F.UnknownAggr).val).f2;_LL2:
 return({struct _tuple22 _tmp9AF;_tmp9AF.f1=_tmp2A1,_tmp9AF.f2=_tmp2A0;_tmp9AF;});}else{_LL3: _tmp2A2=*(_tmp29F.KnownAggr).val;_LL4:
 return({struct _tuple22 _tmp9B0;_tmp9B0.f1=_tmp2A2->kind,_tmp9B0.f2=_tmp2A2->name;_tmp9B0;});}_LL0:;}
# 1606
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1612
struct _tuple0 _tmp2A3=({struct _tuple0 _tmp9B1;_tmp9B1.f1=t1,_tmp9B1.f2=t2;_tmp9B1;});struct _tuple0 _tmp2A4=_tmp2A3;union Cyc_Absyn_AggrInfo _tmp2C2;union Cyc_Absyn_AggrInfo _tmp2C1;union Cyc_Absyn_DatatypeFieldInfo _tmp2C0;union Cyc_Absyn_DatatypeFieldInfo _tmp2BF;union Cyc_Absyn_DatatypeInfo _tmp2BE;union Cyc_Absyn_DatatypeInfo _tmp2BD;struct Cyc_List_List*_tmp2BC;struct Cyc_List_List*_tmp2BB;struct _dyneither_ptr _tmp2BA;struct _dyneither_ptr _tmp2B9;struct _tuple2*_tmp2B8;struct _tuple2*_tmp2B7;switch(*((int*)_tmp2A4.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 15U){_LL1: _tmp2B8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2B7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp2B8,_tmp2B7);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 17U){_LL3: _tmp2BA=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2B9=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL4:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp2BA,(struct _dyneither_ptr)_tmp2B9);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 16U){_LL5: _tmp2BC=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2BB=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL6:
# 1616
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp2BC,_tmp2BB);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 18U){_LL7: _tmp2BE=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2BD=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL8: {
# 1618
struct _tuple2*q1=Cyc_Tcutil_get_datatype_qvar(_tmp2BE);
struct _tuple2*q2=Cyc_Tcutil_get_datatype_qvar(_tmp2BD);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 19U){_LL9: _tmp2C0=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2BF=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LLA: {
# 1622
struct _tuple21 _tmp2A5=Cyc_Tcutil_get_datatype_field_qvars(_tmp2C0);struct _tuple21 _tmp2A6=_tmp2A5;struct _tuple2*_tmp2AD;struct _tuple2*_tmp2AC;_LL10: _tmp2AD=_tmp2A6.f1;_tmp2AC=_tmp2A6.f2;_LL11:;{
struct _tuple21 _tmp2A7=Cyc_Tcutil_get_datatype_field_qvars(_tmp2BF);struct _tuple21 _tmp2A8=_tmp2A7;struct _tuple2*_tmp2AB;struct _tuple2*_tmp2AA;_LL13: _tmp2AB=_tmp2A8.f1;_tmp2AA=_tmp2A8.f2;_LL14:;{
int _tmp2A9=Cyc_Absyn_qvar_cmp(_tmp2AD,_tmp2AB);
if(_tmp2A9 != 0)return _tmp2A9;
return Cyc_Absyn_qvar_cmp(_tmp2AC,_tmp2AA);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 20U){_LLB: _tmp2C2=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2C1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LLC: {
# 1628
struct _tuple22 _tmp2AE=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C2);struct _tuple22 _tmp2AF=_tmp2AE;enum Cyc_Absyn_AggrKind _tmp2B6;struct _tuple2*_tmp2B5;_LL16: _tmp2B6=_tmp2AF.f1;_tmp2B5=_tmp2AF.f2;_LL17:;{
struct _tuple22 _tmp2B0=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2C1);struct _tuple22 _tmp2B1=_tmp2B0;enum Cyc_Absyn_AggrKind _tmp2B4;struct _tuple2*_tmp2B3;_LL19: _tmp2B4=_tmp2B1.f1;_tmp2B3=_tmp2B1.f2;_LL1A:;{
int _tmp2B2=Cyc_Absyn_qvar_cmp(_tmp2B5,_tmp2B3);
if(_tmp2B2 != 0)return _tmp2B2;
return(int)_tmp2B6 - (int)_tmp2B4;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}struct _tuple23{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1638
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB28=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpB28;});
({void*_tmpB29=Cyc_Tcutil_compress(t1);t1=_tmpB29;});
({void*_tmpB2A=Cyc_Tcutil_compress(t2);t2=_tmpB2A;});
if(t1 == t2)return;
{void*_tmp2C3=t1;struct Cyc_Core_Opt*_tmp2D1;void**_tmp2D0;struct Cyc_Core_Opt*_tmp2CF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->tag == 1U){_LL1: _tmp2D1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f1;_tmp2D0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f2;_tmp2CF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f4;_LL2:
# 1649
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CF))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2C4=Cyc_Tcutil_type_kind(t2);
# 1654
if(Cyc_Tcutil_kind_leq(_tmp2C4,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2D1))->v)){
*_tmp2D0=t2;
return;}else{
# 1658
{void*_tmp2C5=t2;struct Cyc_Absyn_PtrInfo _tmp2CD;void**_tmp2CC;struct Cyc_Core_Opt*_tmp2CB;switch(*((int*)_tmp2C5)){case 1U: _LL6: _tmp2CC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f2;_tmp2CB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f4;_LL7: {
# 1661
struct Cyc_List_List*_tmp2C6=(struct Cyc_List_List*)_tmp2CF->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CB))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2C6,(struct Cyc_Absyn_Tvar*)s2->hd)){
({struct _dyneither_ptr _tmpB2B=({const char*_tmp2C7="(type variable would escape scope)";_tag_dyneither(_tmp2C7,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpB2B;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1668
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2D1->v,_tmp2C4)){
*_tmp2CC=t1;return;}
# 1671
({struct _dyneither_ptr _tmpB2C=({const char*_tmp2C8="(kinds are incompatible)";_tag_dyneither(_tmp2C8,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB2C;});
goto _LL5;}case 3U: _LL8: _tmp2CD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5)->f1;if(((struct Cyc_Absyn_Kind*)_tmp2D1->v)->kind == Cyc_Absyn_BoxKind){_LL9: {
# 1677
void*_tmp2C9=Cyc_Tcutil_compress((_tmp2CD.ptr_atts).bounds);
{void*_tmp2CA=_tmp2C9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2CA)->tag == 1U){_LLD: _LLE:
# 1680
({void*_tmpB2D=_tmp2C9;Cyc_Tcutil_unify(_tmpB2D,Cyc_Absyn_bounds_one());});
*_tmp2D0=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1685
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1688
({struct _dyneither_ptr _tmpB2E=({const char*_tmp2CE="(kinds are incompatible)";_tag_dyneither(_tmp2CE,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB2E;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1691
 goto _LL0;}_LL0:;}
# 1696
{struct _tuple0 _tmp2D2=({struct _tuple0 _tmp9B7;_tmp9B7.f1=t2,_tmp9B7.f2=t1;_tmp9B7;});struct _tuple0 _tmp2D3=_tmp2D2;struct Cyc_List_List*_tmp361;struct Cyc_Absyn_Typedefdecl*_tmp360;struct Cyc_List_List*_tmp35F;struct Cyc_Absyn_Typedefdecl*_tmp35E;enum Cyc_Absyn_AggrKind _tmp35D;struct Cyc_List_List*_tmp35C;enum Cyc_Absyn_AggrKind _tmp35B;struct Cyc_List_List*_tmp35A;struct Cyc_List_List*_tmp359;struct Cyc_List_List*_tmp358;struct Cyc_List_List*_tmp357;void*_tmp356;struct Cyc_Absyn_Tqual _tmp355;void*_tmp354;struct Cyc_List_List*_tmp353;int _tmp352;struct Cyc_Absyn_VarargInfo*_tmp351;struct Cyc_List_List*_tmp350;struct Cyc_List_List*_tmp34F;struct Cyc_Absyn_Exp*_tmp34E;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Exp*_tmp34C;struct Cyc_List_List*_tmp34B;struct Cyc_List_List*_tmp34A;void*_tmp349;struct Cyc_Absyn_Tqual _tmp348;void*_tmp347;struct Cyc_List_List*_tmp346;int _tmp345;struct Cyc_Absyn_VarargInfo*_tmp344;struct Cyc_List_List*_tmp343;struct Cyc_List_List*_tmp342;struct Cyc_Absyn_Exp*_tmp341;struct Cyc_List_List*_tmp340;struct Cyc_Absyn_Exp*_tmp33F;struct Cyc_List_List*_tmp33E;void*_tmp33D;struct Cyc_Absyn_Tqual _tmp33C;struct Cyc_Absyn_Exp*_tmp33B;void*_tmp33A;void*_tmp339;struct Cyc_Absyn_Tqual _tmp338;struct Cyc_Absyn_Exp*_tmp337;void*_tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_Absyn_Exp*_tmp334;void*_tmp333;struct Cyc_Absyn_Tqual _tmp332;void*_tmp331;void*_tmp330;void*_tmp32F;void*_tmp32E;void*_tmp32D;struct Cyc_Absyn_Tqual _tmp32C;void*_tmp32B;void*_tmp32A;void*_tmp329;void*_tmp328;struct Cyc_Absyn_Tvar*_tmp327;struct Cyc_Absyn_Tvar*_tmp326;void*_tmp325;struct Cyc_List_List*_tmp324;void*_tmp323;struct Cyc_List_List*_tmp322;switch(*((int*)_tmp2D3.f1)){case 1U: _LL12: _LL13:
# 1699
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)->tag == 9U){_LL14: _LL15:
# 1702
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmp325=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp324=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f2;_tmp323=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_tmp322=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f2;_LL21:
# 1713
 if(Cyc_Tcutil_tycon_cmp(_tmp325,_tmp323)== 0){
Cyc_Tcutil_unify_list(_tmp324,_tmp322);
return;}else{
# 1717
({struct _dyneither_ptr _tmpB2F=({const char*_tmp2D5="(different type constructors)";_tag_dyneither(_tmp2D5,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpB2F;});}
goto _LL11;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f1)->f1)){case 8U: _LL18: _LL19:
# 1704
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 1706
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D3.f2)->f1)){case 9U: _LL16: _LL17:
# 1703
 goto _LL19;case 8U: _LL1A: _LL1B:
# 1705
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 1708
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
({struct _dyneither_ptr _tmpB30=({const char*_tmp2D4="(effects don't unify)";_tag_dyneither(_tmp2D4,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpB30;});
goto _LL11;default: switch(*((int*)_tmp2D3.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp2D3.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 2U){_LL22: _tmp327=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp326=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_LL23: {
# 1721
struct _dyneither_ptr*_tmp2D6=_tmp327->name;
struct _dyneither_ptr*_tmp2D7=_tmp326->name;
# 1724
int _tmp2D8=_tmp327->identity;
int _tmp2D9=_tmp326->identity;
if(_tmp2D9 == _tmp2D8)return;
({struct _dyneither_ptr _tmpB31=({const char*_tmp2DA="(variable types are not the same)";_tag_dyneither(_tmp2DA,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpB31;});
goto _LL11;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 3U){_LL24: _tmp333=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).elt_type;_tmp332=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).elt_tq;_tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).rgn;_tmp330=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).nullable;_tmp32F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).bounds;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ptr_atts).zero_term;_tmp32D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).elt_type;_tmp32C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).elt_tq;_tmp32B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).rgn;_tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).nullable;_tmp329=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).bounds;_tmp328=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ptr_atts).zero_term;_LL25:
# 1732
 Cyc_Tcutil_unify_it(_tmp32D,_tmp333);
Cyc_Tcutil_unify_it(_tmp331,_tmp32B);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
struct _dyneither_ptr _tmp2DB=Cyc_Tcutil_failure_reason;
({struct _dyneither_ptr _tmpB32=({const char*_tmp2DC="(not both zero terminated)";_tag_dyneither(_tmp2DC,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpB32;});
Cyc_Tcutil_unify_it(_tmp328,_tmp32E);
Cyc_Tcutil_unify_tqual(_tmp32C,_tmp32D,_tmp332,_tmp333);
({struct _dyneither_ptr _tmpB33=({const char*_tmp2DD="(different pointer bounds)";_tag_dyneither(_tmp2DD,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpB33;});
Cyc_Tcutil_unify_it(_tmp329,_tmp32F);{
# 1743
void*_tmp2DE=Cyc_Tcutil_compress(_tmp329);void*_tmp2DF=_tmp2DE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DF)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2DF)->f1)->tag == 14U){_LL35: _LL36:
# 1745
 Cyc_Tcutil_failure_reason=_tmp2DB;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 1748
({struct _dyneither_ptr _tmpB34=({const char*_tmp2E0="(incompatible pointer types)";_tag_dyneither(_tmp2E0,sizeof(char),29U);});Cyc_Tcutil_failure_reason=_tmpB34;});
Cyc_Tcutil_unify_it(_tmp32A,_tmp330);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 9U){_LL26: _tmp335=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp334=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_LL27:
# 1754
 if(!Cyc_Evexp_same_const_exp(_tmp335,_tmp334)){
({struct _dyneither_ptr _tmpB35=({const char*_tmp2E1="(cannot prove expressions are the same)";_tag_dyneither(_tmp2E1,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpB35;});
goto _LL11;}
# 1758
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 4U){_LL28: _tmp33D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).elt_type;_tmp33C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).tq;_tmp33B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).num_elts;_tmp33A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f1)->f1).zero_term;_tmp339=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).elt_type;_tmp338=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).tq;_tmp337=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).num_elts;_tmp336=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D3.f2)->f1).zero_term;_LL29:
# 1762
 Cyc_Tcutil_unify_it(_tmp339,_tmp33D);
Cyc_Tcutil_unify_tqual(_tmp338,_tmp339,_tmp33C,_tmp33D);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB36=({const char*_tmp2E2="(not both zero terminated)";_tag_dyneither(_tmp2E2,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpB36;});
Cyc_Tcutil_unify_it(_tmp33A,_tmp336);
if(_tmp33B == _tmp337)return;
if(_tmp33B == 0  || _tmp337 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp33B,_tmp337))
return;
({struct _dyneither_ptr _tmpB37=({const char*_tmp2E3="(different array sizes)";_tag_dyneither(_tmp2E3,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpB37;});
goto _LL11;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 5U){_LL2A: _tmp357=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).tvars;_tmp356=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).effect;_tmp355=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ret_tqual;_tmp354=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ret_type;_tmp353=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).args;_tmp352=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).c_varargs;_tmp351=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).cyc_varargs;_tmp350=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).rgn_po;_tmp34F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).attributes;_tmp34E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).requires_clause;_tmp34D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).requires_relns;_tmp34C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ensures_clause;_tmp34B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f1)->f1).ensures_relns;_tmp34A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).tvars;_tmp349=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).effect;_tmp348=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ret_tqual;_tmp347=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ret_type;_tmp346=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).args;_tmp345=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).c_varargs;_tmp344=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).cyc_varargs;_tmp343=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).rgn_po;_tmp342=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).attributes;_tmp341=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).requires_clause;_tmp340=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).requires_relns;_tmp33F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ensures_clause;_tmp33E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2D3.f2)->f1).ensures_relns;_LL2B: {
# 1777
int done=0;
{struct _RegionHandle _tmp2E4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2E4;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp34A != 0){
if(_tmp357 == 0){
({struct _dyneither_ptr _tmpB38=({const char*_tmp2E5="(second function type has too few type variables)";_tag_dyneither(_tmp2E5,sizeof(char),50U);});Cyc_Tcutil_failure_reason=_tmpB38;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1785
void*_tmp2E6=((struct Cyc_Absyn_Tvar*)_tmp34A->hd)->kind;
void*_tmp2E7=((struct Cyc_Absyn_Tvar*)_tmp357->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2E6,_tmp2E7)){
({struct _dyneither_ptr _tmpB3D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2EA=({struct Cyc_String_pa_PrintArg_struct _tmp9B4;_tmp9B4.tag=0U,({
struct _dyneither_ptr _tmpB39=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp34A->hd));_tmp9B4.f1=_tmpB39;});_tmp9B4;});struct Cyc_String_pa_PrintArg_struct _tmp2EB=({struct Cyc_String_pa_PrintArg_struct _tmp9B3;_tmp9B3.tag=0U,({
struct _dyneither_ptr _tmpB3A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp34A->hd,& Cyc_Tcutil_bk)));_tmp9B3.f1=_tmpB3A;});_tmp9B3;});struct Cyc_String_pa_PrintArg_struct _tmp2EC=({struct Cyc_String_pa_PrintArg_struct _tmp9B2;_tmp9B2.tag=0U,({
struct _dyneither_ptr _tmpB3B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp357->hd,& Cyc_Tcutil_bk)));_tmp9B2.f1=_tmpB3B;});_tmp9B2;});void*_tmp2E8[3U];_tmp2E8[0]=& _tmp2EA,_tmp2E8[1]=& _tmp2EB,_tmp2E8[2]=& _tmp2EC;({struct _dyneither_ptr _tmpB3C=({const char*_tmp2E9="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp2E9,sizeof(char),44U);});Cyc_aprintf(_tmpB3C,_tag_dyneither(_tmp2E8,sizeof(void*),3U));});});
# 1788
Cyc_Tcutil_failure_reason=_tmpB3D;});
# 1792
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1794
({struct Cyc_List_List*_tmpB40=({struct Cyc_List_List*_tmp2EE=_region_malloc(rgn,sizeof(*_tmp2EE));({struct _tuple15*_tmpB3F=({struct _tuple15*_tmp2ED=_region_malloc(rgn,sizeof(*_tmp2ED));_tmp2ED->f1=(struct Cyc_Absyn_Tvar*)_tmp357->hd,({void*_tmpB3E=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp34A->hd);_tmp2ED->f2=_tmpB3E;});_tmp2ED;});_tmp2EE->hd=_tmpB3F;}),_tmp2EE->tl=inst;_tmp2EE;});inst=_tmpB40;});
_tmp34A=_tmp34A->tl;
_tmp357=_tmp357->tl;};}
# 1798
if(_tmp357 != 0){
({struct _dyneither_ptr _tmpB41=({const char*_tmp2EF="(second function type has too many type variables)";_tag_dyneither(_tmp2EF,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpB41;});
_npop_handler(0U);goto _LL11;}
# 1802
if(inst != 0){
({void*_tmpB44=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F0=_cycalloc(sizeof(*_tmp2F0));_tmp2F0->tag=5U,(_tmp2F0->f1).tvars=0,(_tmp2F0->f1).effect=_tmp349,(_tmp2F0->f1).ret_tqual=_tmp348,(_tmp2F0->f1).ret_type=_tmp347,(_tmp2F0->f1).args=_tmp346,(_tmp2F0->f1).c_varargs=_tmp345,(_tmp2F0->f1).cyc_varargs=_tmp344,(_tmp2F0->f1).rgn_po=_tmp343,(_tmp2F0->f1).attributes=_tmp342,(_tmp2F0->f1).requires_clause=_tmp34E,(_tmp2F0->f1).requires_relns=_tmp34D,(_tmp2F0->f1).ensures_clause=_tmp34C,(_tmp2F0->f1).ensures_relns=_tmp34B;_tmp2F0;});Cyc_Tcutil_unify_it(_tmpB44,({
# 1806
struct _RegionHandle*_tmpB43=rgn;struct Cyc_List_List*_tmpB42=inst;Cyc_Tcutil_rsubstitute(_tmpB43,_tmpB42,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2F1=_cycalloc(sizeof(*_tmp2F1));
_tmp2F1->tag=5U,(_tmp2F1->f1).tvars=0,(_tmp2F1->f1).effect=_tmp356,(_tmp2F1->f1).ret_tqual=_tmp355,(_tmp2F1->f1).ret_type=_tmp354,(_tmp2F1->f1).args=_tmp353,(_tmp2F1->f1).c_varargs=_tmp352,(_tmp2F1->f1).cyc_varargs=_tmp351,(_tmp2F1->f1).rgn_po=_tmp350,(_tmp2F1->f1).attributes=_tmp34F,(_tmp2F1->f1).requires_clause=_tmp341,(_tmp2F1->f1).requires_relns=_tmp340,(_tmp2F1->f1).ensures_clause=_tmp33F,(_tmp2F1->f1).ensures_relns=_tmp33E;_tmp2F1;}));}));});
# 1811
done=1;}}
# 1779
;_pop_region(rgn);}
# 1814
if(done)
return;
Cyc_Tcutil_unify_it(_tmp347,_tmp354);
Cyc_Tcutil_unify_tqual(_tmp348,_tmp347,_tmp355,_tmp354);
for(0;_tmp346 != 0  && _tmp353 != 0;(_tmp346=_tmp346->tl,_tmp353=_tmp353->tl)){
struct _tuple10 _tmp2F2=*((struct _tuple10*)_tmp346->hd);struct _tuple10 _tmp2F3=_tmp2F2;struct Cyc_Absyn_Tqual _tmp2F9;void*_tmp2F8;_LL3A: _tmp2F9=_tmp2F3.f2;_tmp2F8=_tmp2F3.f3;_LL3B:;{
struct _tuple10 _tmp2F4=*((struct _tuple10*)_tmp353->hd);struct _tuple10 _tmp2F5=_tmp2F4;struct Cyc_Absyn_Tqual _tmp2F7;void*_tmp2F6;_LL3D: _tmp2F7=_tmp2F5.f2;_tmp2F6=_tmp2F5.f3;_LL3E:;
Cyc_Tcutil_unify_it(_tmp2F8,_tmp2F6);
Cyc_Tcutil_unify_tqual(_tmp2F9,_tmp2F8,_tmp2F7,_tmp2F6);};}
# 1824
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp346 != 0  || _tmp353 != 0){
({struct _dyneither_ptr _tmpB45=({const char*_tmp2FA="(function types have different number of arguments)";_tag_dyneither(_tmp2FA,sizeof(char),52U);});Cyc_Tcutil_failure_reason=_tmpB45;});
goto _LL11;}
# 1830
if(_tmp345 != _tmp352){
({struct _dyneither_ptr _tmpB46=({const char*_tmp2FB="(only one function type takes C varargs)";_tag_dyneither(_tmp2FB,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpB46;});
goto _LL11;}{
# 1835
int bad_cyc_vararg=0;
{struct _tuple23 _tmp2FC=({struct _tuple23 _tmp9B5;_tmp9B5.f1=_tmp344,_tmp9B5.f2=_tmp351;_tmp9B5;});struct _tuple23 _tmp2FD=_tmp2FC;struct _dyneither_ptr*_tmp307;struct Cyc_Absyn_Tqual _tmp306;void*_tmp305;int _tmp304;struct _dyneither_ptr*_tmp303;struct Cyc_Absyn_Tqual _tmp302;void*_tmp301;int _tmp300;if(_tmp2FD.f1 == 0){if(_tmp2FD.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmp2FD.f2 == 0){_LL44: _LL45:
# 1840
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpB47=({const char*_tmp2FE="(only one function type takes varargs)";_tag_dyneither(_tmp2FE,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpB47;});
goto _LL3F;}else{_LL46: _tmp307=(_tmp2FD.f1)->name;_tmp306=(_tmp2FD.f1)->tq;_tmp305=(_tmp2FD.f1)->type;_tmp304=(_tmp2FD.f1)->inject;_tmp303=(_tmp2FD.f2)->name;_tmp302=(_tmp2FD.f2)->tq;_tmp301=(_tmp2FD.f2)->type;_tmp300=(_tmp2FD.f2)->inject;_LL47:
# 1844
 Cyc_Tcutil_unify_it(_tmp305,_tmp301);
Cyc_Tcutil_unify_tqual(_tmp306,_tmp305,_tmp302,_tmp301);
if(_tmp304 != _tmp300){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpB48=({const char*_tmp2FF="(only one function type injects varargs)";_tag_dyneither(_tmp2FF,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpB48;});}
# 1850
goto _LL3F;}}_LL3F:;}
# 1852
if(bad_cyc_vararg)goto _LL11;{
# 1855
int bad_effect=0;
{struct _tuple0 _tmp308=({struct _tuple0 _tmp9B6;_tmp9B6.f1=_tmp349,_tmp9B6.f2=_tmp356;_tmp9B6;});struct _tuple0 _tmp309=_tmp308;if(_tmp309.f1 == 0){if(_tmp309.f2 == 0){_LL49: _LL4A:
 goto _LL48;}else{_LL4B: _LL4C:
 goto _LL4E;}}else{if(_tmp309.f2 == 0){_LL4D: _LL4E:
 bad_effect=1;goto _LL48;}else{_LL4F: _LL50:
({int _tmpB4A=!({void*_tmpB49=(void*)_check_null(_tmp349);Cyc_Tcutil_unify_effect(_tmpB49,(void*)_check_null(_tmp356));});bad_effect=_tmpB4A;});goto _LL48;}}_LL48:;}
# 1862
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpB4B=({const char*_tmp30A="(function effects do not match)";_tag_dyneither(_tmp30A,sizeof(char),32U);});Cyc_Tcutil_failure_reason=_tmpB4B;});
goto _LL11;}
# 1868
if(!Cyc_Tcutil_same_atts(_tmp34F,_tmp342)){
({struct _dyneither_ptr _tmpB4C=({const char*_tmp30B="(function types have different attributes)";_tag_dyneither(_tmp30B,sizeof(char),43U);});Cyc_Tcutil_failure_reason=_tmpB4C;});
goto _LL11;}
# 1872
if(!Cyc_Tcutil_same_rgn_po(_tmp350,_tmp343)){
({struct _dyneither_ptr _tmpB4D=({const char*_tmp30C="(function types have different region lifetime orderings)";_tag_dyneither(_tmp30C,sizeof(char),58U);});Cyc_Tcutil_failure_reason=_tmpB4D;});
goto _LL11;}
# 1876
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34D,_tmp340)){
({struct _dyneither_ptr _tmpB4E=({const char*_tmp30D="(@requires clauses not equivalent)";_tag_dyneither(_tmp30D,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpB4E;});
goto _LL11;}
# 1880
if(!Cyc_Tcutil_check_logical_equivalence(_tmp34B,_tmp33E)){
({struct _dyneither_ptr _tmpB4F=({const char*_tmp30E="(@ensures clauses not equivalent)";_tag_dyneither(_tmp30E,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpB4F;});
goto _LL11;}
# 1884
return;};};}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 6U){_LL2C: _tmp359=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp358=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_LL2D:
# 1887
 for(0;_tmp358 != 0  && _tmp359 != 0;(_tmp358=_tmp358->tl,_tmp359=_tmp359->tl)){
struct _tuple12 _tmp30F=*((struct _tuple12*)_tmp358->hd);struct _tuple12 _tmp310=_tmp30F;struct Cyc_Absyn_Tqual _tmp316;void*_tmp315;_LL52: _tmp316=_tmp310.f1;_tmp315=_tmp310.f2;_LL53:;{
struct _tuple12 _tmp311=*((struct _tuple12*)_tmp359->hd);struct _tuple12 _tmp312=_tmp311;struct Cyc_Absyn_Tqual _tmp314;void*_tmp313;_LL55: _tmp314=_tmp312.f1;_tmp313=_tmp312.f2;_LL56:;
Cyc_Tcutil_unify_it(_tmp315,_tmp313);
Cyc_Tcutil_unify_tqual(_tmp316,_tmp315,_tmp314,_tmp313);};}
# 1893
if(_tmp358 == 0  && _tmp359 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB50=({const char*_tmp317="(tuple types have different numbers of components)";_tag_dyneither(_tmp317,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpB50;});
goto _LL11;}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 7U){_LL2E: _tmp35D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f1)->f1;_tmp35C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f1)->f2;_tmp35B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f2)->f1;_tmp35A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2D3.f2)->f2;_LL2F:
# 1900
 if(_tmp35B != _tmp35D){({struct _dyneither_ptr _tmpB51=({const char*_tmp318="(struct and union type)";_tag_dyneither(_tmp318,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpB51;});goto _LL11;}
for(0;_tmp35A != 0  && _tmp35C != 0;(_tmp35A=_tmp35A->tl,_tmp35C=_tmp35C->tl)){
struct Cyc_Absyn_Aggrfield*_tmp319=(struct Cyc_Absyn_Aggrfield*)_tmp35A->hd;
struct Cyc_Absyn_Aggrfield*_tmp31A=(struct Cyc_Absyn_Aggrfield*)_tmp35C->hd;
if(Cyc_strptrcmp(_tmp319->name,_tmp31A->name)!= 0){
({struct _dyneither_ptr _tmpB52=({const char*_tmp31B="(different member names)";_tag_dyneither(_tmp31B,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpB52;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1908
Cyc_Tcutil_unify_it(_tmp319->type,_tmp31A->type);
Cyc_Tcutil_unify_tqual(_tmp319->tq,_tmp319->type,_tmp31A->tq,_tmp31A->type);
if(!Cyc_Tcutil_same_atts(_tmp319->attributes,_tmp31A->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB53=({const char*_tmp31C="(different attributes on member)";_tag_dyneither(_tmp31C,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpB53;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1916
if((_tmp319->width != 0  && _tmp31A->width == 0  || 
_tmp31A->width != 0  && _tmp319->width == 0) || 
(_tmp319->width != 0  && _tmp31A->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpB54=(struct Cyc_Absyn_Exp*)_check_null(_tmp319->width);Cyc_Evexp_same_const_exp(_tmpB54,(struct Cyc_Absyn_Exp*)_check_null(_tmp31A->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB55=({const char*_tmp31D="(different bitfield widths on member)";_tag_dyneither(_tmp31D,sizeof(char),38U);});Cyc_Tcutil_failure_reason=_tmpB55;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1925
if((_tmp319->requires_clause != 0  && _tmp31A->requires_clause == 0  || 
_tmp319->requires_clause == 0  && _tmp31A->requires_clause != 0) || 
(_tmp319->requires_clause == 0  && _tmp31A->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpB56=(struct Cyc_Absyn_Exp*)_check_null(_tmp319->requires_clause);Cyc_Evexp_same_const_exp(_tmpB56,(struct Cyc_Absyn_Exp*)_check_null(_tmp31A->requires_clause));})){
# 1930
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB57=({const char*_tmp31E="(different @requires clauses on member)";_tag_dyneither(_tmp31E,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpB57;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1936
if(_tmp35A == 0  && _tmp35C == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpB58=({const char*_tmp31F="(different number of members)";_tag_dyneither(_tmp31F,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpB58;});
goto _LL11;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f2)->tag == 8U){_LL30: _tmp361=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f1)->f2;_tmp360=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f1)->f3;_tmp35F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f2)->f2;_tmp35E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2D3.f2)->f3;_LL31:
# 1942
 if(_tmp360 != _tmp35E){
({struct _dyneither_ptr _tmpB59=({const char*_tmp320="(different abstract typedefs)";_tag_dyneither(_tmp320,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpB59;});
goto _LL11;}
# 1946
({struct _dyneither_ptr _tmpB5A=({const char*_tmp321="(type parameters to typedef differ)";_tag_dyneither(_tmp321,sizeof(char),36U);});Cyc_Tcutil_failure_reason=_tmpB5A;});
Cyc_Tcutil_unify_list(_tmp361,_tmp35F);
return;}else{goto _LL32;}default: _LL32: _LL33:
 goto _LL11;}}}_LL11:;}
# 1951
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1954
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpB5C)(void*,void*)=cmp;void*_tmpB5B=(void*)_check_null(a1);_tmpB5C(_tmpB5B,(void*)_check_null(a2));});}
# 1961
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp362=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp363=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp362,_tmp363);}
# 1967
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp364=tqt1;struct Cyc_Absyn_Tqual _tmp36A;void*_tmp369;_LL1: _tmp36A=_tmp364->f1;_tmp369=_tmp364->f2;_LL2:;{
struct _tuple12*_tmp365=tqt2;struct Cyc_Absyn_Tqual _tmp368;void*_tmp367;_LL4: _tmp368=_tmp365->f1;_tmp367=_tmp365->f2;_LL5:;{
int _tmp366=Cyc_Tcutil_tqual_cmp(_tmp36A,_tmp368);
if(_tmp366 != 0)return _tmp366;
return Cyc_Tcutil_typecmp(_tmp369,_tmp367);};};}
# 1975
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp36B=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp36B != 0)return _tmp36B;{
int _tmp36C=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp36C != 0)return _tmp36C;{
int _tmp36D=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp36D != 0)return _tmp36D;{
int _tmp36E=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp36E != 0)return _tmp36E;
({int _tmpB5D=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp36E=_tmpB5D;});
if(_tmp36E != 0)return _tmp36E;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1989
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp36F=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp36F != 0)return _tmp36F;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1995
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp370=t;void*_tmp371;switch(*((int*)_tmp370)){case 1U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 2;case 3U: _LL5: _LL6:
 return 3;case 4U: _LL7: _LL8:
 return 4;case 5U: _LL9: _LLA:
 return 5;case 6U: _LLB: _LLC:
 return 6;case 7U: _LLD: _LLE:
 return 7;case 8U: _LLF: _LL10:
 return 8;case 9U: _LL11: _LL12:
 return 9;case 10U: _LL13: _LL14:
 return 10;case 11U: _LL15: _LL16:
 return 11;default: _LL17: _tmp371=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp370)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp371);}_LL0:;}
# 2014
int Cyc_Tcutil_typecmp(void*t1,void*t2){
({void*_tmpB5E=Cyc_Tcutil_compress(t1);t1=_tmpB5E;});
({void*_tmpB5F=Cyc_Tcutil_compress(t2);t2=_tmpB5F;});
if(t1 == t2)return 0;{
int _tmp372=({int _tmpB60=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpB60,Cyc_Tcutil_type_case_number(t2));});
if(_tmp372 != 0)
return _tmp372;{
# 2023
struct _tuple0 _tmp373=({struct _tuple0 _tmp9B8;_tmp9B8.f1=t2,_tmp9B8.f2=t1;_tmp9B8;});struct _tuple0 _tmp374=_tmp373;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_Absyn_Exp*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;enum Cyc_Absyn_AggrKind _tmp3BC;struct Cyc_List_List*_tmp3BB;enum Cyc_Absyn_AggrKind _tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_List_List*_tmp3B8;struct Cyc_List_List*_tmp3B7;struct Cyc_Absyn_FnInfo _tmp3B6;struct Cyc_Absyn_FnInfo _tmp3B5;void*_tmp3B4;struct Cyc_Absyn_Tqual _tmp3B3;struct Cyc_Absyn_Exp*_tmp3B2;void*_tmp3B1;void*_tmp3B0;struct Cyc_Absyn_Tqual _tmp3AF;struct Cyc_Absyn_Exp*_tmp3AE;void*_tmp3AD;void*_tmp3AC;struct Cyc_Absyn_Tqual _tmp3AB;void*_tmp3AA;void*_tmp3A9;void*_tmp3A8;void*_tmp3A7;void*_tmp3A6;struct Cyc_Absyn_Tqual _tmp3A5;void*_tmp3A4;void*_tmp3A3;void*_tmp3A2;void*_tmp3A1;struct Cyc_Absyn_Tvar*_tmp3A0;struct Cyc_Absyn_Tvar*_tmp39F;void*_tmp39E;struct Cyc_List_List*_tmp39D;void*_tmp39C;struct Cyc_List_List*_tmp39B;switch(*((int*)_tmp374.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->tag == 0U){_LL1: _tmp39E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp39D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f1)->f2;_tmp39C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->f1;_tmp39B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->f2;_LL2: {
# 2025
int _tmp375=Cyc_Tcutil_tycon_cmp(_tmp39E,_tmp39C);
if(_tmp375 != 0)return _tmp375;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp39D,_tmp39B);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp374.f2)->tag == 1U){_LL3: _LL4:
# 2029
({void*_tmp376=0U;({struct _dyneither_ptr _tmpB61=({const char*_tmp377="typecmp: can only compare closed types";_tag_dyneither(_tmp377,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB61,_tag_dyneither(_tmp376,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp374.f2)->tag == 2U){_LL5: _tmp3A0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp39F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp374.f2)->f1;_LL6:
# 2033
 return Cyc_Core_intcmp(_tmp39F->identity,_tmp3A0->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->tag == 3U){_LL7: _tmp3AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).elt_type;_tmp3AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).elt_tq;_tmp3AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).rgn;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).nullable;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).bounds;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f1)->f1).ptr_atts).zero_term;_tmp3A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).elt_type;_tmp3A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).elt_tq;_tmp3A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).rgn;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).nullable;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).bounds;_tmp3A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp374.f2)->f1).ptr_atts).zero_term;_LL8: {
# 2037
int _tmp378=Cyc_Tcutil_typecmp(_tmp3A6,_tmp3AC);
if(_tmp378 != 0)return _tmp378;{
int _tmp379=Cyc_Tcutil_typecmp(_tmp3A4,_tmp3AA);
if(_tmp379 != 0)return _tmp379;{
int _tmp37A=Cyc_Tcutil_tqual_cmp(_tmp3A5,_tmp3AB);
if(_tmp37A != 0)return _tmp37A;{
int _tmp37B=Cyc_Tcutil_typecmp(_tmp3A2,_tmp3A8);
if(_tmp37B != 0)return _tmp37B;{
int _tmp37C=Cyc_Tcutil_typecmp(_tmp3A1,_tmp3A7);
if(_tmp37C != 0)return _tmp37C;{
int _tmp37D=Cyc_Tcutil_typecmp(_tmp3A2,_tmp3A8);
if(_tmp37D != 0)return _tmp37D;
return Cyc_Tcutil_typecmp(_tmp3A3,_tmp3A9);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->tag == 4U){_LL9: _tmp3B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).elt_type;_tmp3B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).tq;_tmp3B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).num_elts;_tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f1)->f1).zero_term;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).elt_type;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).tq;_tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).num_elts;_tmp3AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp374.f2)->f1).zero_term;_LLA: {
# 2053
int _tmp37E=Cyc_Tcutil_tqual_cmp(_tmp3AF,_tmp3B3);
if(_tmp37E != 0)return _tmp37E;{
int _tmp37F=Cyc_Tcutil_typecmp(_tmp3B0,_tmp3B4);
if(_tmp37F != 0)return _tmp37F;{
int _tmp380=Cyc_Tcutil_typecmp(_tmp3B1,_tmp3AD);
if(_tmp380 != 0)return _tmp380;
if(_tmp3B2 == _tmp3AE)return 0;
if(_tmp3B2 == 0  || _tmp3AE == 0)
({void*_tmp381=0U;({struct _dyneither_ptr _tmpB62=({const char*_tmp382="missing expression in array index";_tag_dyneither(_tmp382,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB62,_tag_dyneither(_tmp381,sizeof(void*),0U));});});
# 2063
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B2,_tmp3AE);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp374.f2)->tag == 5U){_LLB: _tmp3B6=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3B5=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp374.f2)->f1;_LLC:
# 2066
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp3B6.ret_type,_tmp3B5.ret_type);
if(r != 0)return r;
({int _tmpB63=Cyc_Tcutil_tqual_cmp(_tmp3B6.ret_tqual,_tmp3B5.ret_tqual);r=_tmpB63;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp383=_tmp3B6.args;
struct Cyc_List_List*_tmp384=_tmp3B5.args;
for(0;_tmp383 != 0  && _tmp384 != 0;(_tmp383=_tmp383->tl,_tmp384=_tmp384->tl)){
struct _tuple10 _tmp385=*((struct _tuple10*)_tmp383->hd);struct _tuple10 _tmp386=_tmp385;struct Cyc_Absyn_Tqual _tmp38C;void*_tmp38B;_LL18: _tmp38C=_tmp386.f2;_tmp38B=_tmp386.f3;_LL19:;{
struct _tuple10 _tmp387=*((struct _tuple10*)_tmp384->hd);struct _tuple10 _tmp388=_tmp387;struct Cyc_Absyn_Tqual _tmp38A;void*_tmp389;_LL1B: _tmp38A=_tmp388.f2;_tmp389=_tmp388.f3;_LL1C:;
({int _tmpB64=Cyc_Tcutil_tqual_cmp(_tmp38C,_tmp38A);r=_tmpB64;});
if(r != 0)return r;
({int _tmpB65=Cyc_Tcutil_typecmp(_tmp38B,_tmp389);r=_tmpB65;});
if(r != 0)return r;};}
# 2081
if(_tmp383 != 0)return 1;
if(_tmp384 != 0)return - 1;
if(_tmp3B6.c_varargs  && !_tmp3B5.c_varargs)return 1;
if(!_tmp3B6.c_varargs  && _tmp3B5.c_varargs)return - 1;
if(_tmp3B6.cyc_varargs != 0 & _tmp3B5.cyc_varargs == 0)return 1;
if(_tmp3B6.cyc_varargs == 0 & _tmp3B5.cyc_varargs != 0)return - 1;
if(_tmp3B6.cyc_varargs != 0 & _tmp3B5.cyc_varargs != 0){
({int _tmpB67=({struct Cyc_Absyn_Tqual _tmpB66=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B6.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmpB66,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp3B5.cyc_varargs))->tq);});r=_tmpB67;});
if(r != 0)return r;
({int _tmpB68=Cyc_Tcutil_typecmp((_tmp3B6.cyc_varargs)->type,(_tmp3B5.cyc_varargs)->type);r=_tmpB68;});
if(r != 0)return r;
if((_tmp3B6.cyc_varargs)->inject  && !(_tmp3B5.cyc_varargs)->inject)return 1;
if(!(_tmp3B6.cyc_varargs)->inject  && (_tmp3B5.cyc_varargs)->inject)return - 1;}
# 2095
({int _tmpB69=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp3B6.effect,_tmp3B5.effect);r=_tmpB69;});
if(r != 0)return r;{
struct Cyc_List_List*_tmp38D=_tmp3B6.rgn_po;
struct Cyc_List_List*_tmp38E=_tmp3B5.rgn_po;
for(0;_tmp38D != 0  && _tmp38E != 0;(_tmp38D=_tmp38D->tl,_tmp38E=_tmp38E->tl)){
struct _tuple0 _tmp38F=*((struct _tuple0*)_tmp38D->hd);struct _tuple0 _tmp390=_tmp38F;void*_tmp396;void*_tmp395;_LL1E: _tmp396=_tmp390.f1;_tmp395=_tmp390.f2;_LL1F:;{
struct _tuple0 _tmp391=*((struct _tuple0*)_tmp38E->hd);struct _tuple0 _tmp392=_tmp391;void*_tmp394;void*_tmp393;_LL21: _tmp394=_tmp392.f1;_tmp393=_tmp392.f2;_LL22:;
({int _tmpB6A=Cyc_Tcutil_typecmp(_tmp396,_tmp394);r=_tmpB6A;});if(r != 0)return r;
({int _tmpB6B=Cyc_Tcutil_typecmp(_tmp395,_tmp393);r=_tmpB6B;});if(r != 0)return r;};}
# 2105
if(_tmp38D != 0)return 1;
if(_tmp38E != 0)return - 1;
({int _tmpB6C=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B6.requires_clause,_tmp3B5.requires_clause);r=_tmpB6C;});
if(r != 0)return r;
({int _tmpB6D=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp3B6.ensures_clause,_tmp3B5.ensures_clause);r=_tmpB6D;});
if(r != 0)return r;
# 2113
({void*_tmp397=0U;({struct _dyneither_ptr _tmpB6E=({const char*_tmp398="typecmp: function type comparison should never get here!";_tag_dyneither(_tmp398,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB6E,_tag_dyneither(_tmp397,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp374.f2)->tag == 6U){_LLD: _tmp3B8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3B7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp374.f2)->f1;_LLE:
# 2116
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp3B7,_tmp3B8);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f2)->tag == 7U){_LLF: _tmp3BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f1)->f2;_tmp3BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f2)->f1;_tmp3B9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp374.f2)->f2;_LL10:
# 2119
 if(_tmp3BA != _tmp3BC){
if(_tmp3BA == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp3B9,_tmp3BB);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp374.f2)->tag == 9U){_LL11: _tmp3BE=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3BD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp374.f2)->f1;_LL12:
# 2124
 _tmp3C0=_tmp3BE;_tmp3BF=_tmp3BD;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp374.f2)->tag == 11U){_LL13: _tmp3C0=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp374.f1)->f1;_tmp3BF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp374.f2)->f1;_LL14:
# 2126
 return Cyc_Evexp_const_exp_cmp(_tmp3C0,_tmp3BF);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp399=0U;({struct _dyneither_ptr _tmpB6F=({const char*_tmp39A="Unmatched case in typecmp";_tag_dyneither(_tmp39A,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB6F,_tag_dyneither(_tmp399,sizeof(void*),0U));});});}_LL0:;};};}
# 2133
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
({void*_tmpB70=Cyc_Tcutil_compress(t1);t1=_tmpB70;});
({void*_tmpB71=Cyc_Tcutil_compress(t2);t2=_tmpB71;});{
struct _tuple0 _tmp3C1=({struct _tuple0 _tmp9BA;_tmp9BA.f1=t1,_tmp9BA.f2=t2;_tmp9BA;});struct _tuple0 _tmp3C2=_tmp3C1;void*_tmp3C8;void*_tmp3C7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f2)->tag == 0U){_LL1: _tmp3C8=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f1)->f1;_tmp3C7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2.f2)->f1;_LL2: {
# 2138
struct _tuple0 _tmp3C3=({struct _tuple0 _tmp9B9;_tmp9B9.f1=_tmp3C8,_tmp9B9.f2=_tmp3C7;_tmp9B9;});struct _tuple0 _tmp3C4=_tmp3C3;int _tmp3C6;int _tmp3C5;switch(*((int*)_tmp3C4.f1)){case 2U: switch(*((int*)_tmp3C4.f2)){case 2U: _LL6: _tmp3C6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f1)->f1;_tmp3C5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f1;_LL7:
# 2140
 return _tmp3C5 < _tmp3C6;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp3C4.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 2147
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 2152
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 2155
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f1 == 0){_LL14: _LL15:
# 2150
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp3C4.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 2148
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 2153
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 2156
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f1 == 0){_LL16: _LL17:
# 2151
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 2157
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3C4.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 2154
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 2158
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 2160
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2162
 return 0;}_LL0:;};}
# 2168
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2171
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
({struct Cyc_Core_Opt*_tmpB72=({struct Cyc_Core_Opt*_tmp3C9=_cycalloc(sizeof(*_tmp3C9));_tmp3C9->v=t1;_tmp3C9;});max_arith_type=_tmpB72;});}}}
# 2180
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2184
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CC=({struct Cyc_String_pa_PrintArg_struct _tmp9BC;_tmp9BC.tag=0U,({
struct _dyneither_ptr _tmpB73=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9BC.f1=_tmpB73;});_tmp9BC;});struct Cyc_String_pa_PrintArg_struct _tmp3CD=({struct Cyc_String_pa_PrintArg_struct _tmp9BB;_tmp9BB.tag=0U,({struct _dyneither_ptr _tmpB74=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp9BB.f1=_tmpB74;});_tmp9BB;});void*_tmp3CA[2U];_tmp3CA[0]=& _tmp3CC,_tmp3CA[1]=& _tmp3CD;({unsigned int _tmpB76=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpB75=({const char*_tmp3CB="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp3CB,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpB76,_tmpB75,_tag_dyneither(_tmp3CA,sizeof(void*),2U));});});
return 0;}}}
# 2190
return 1;}
# 2195
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(te,e)){
void*_tmp3CE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3CF=_tmp3CE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CF)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2201
return 1;}
# 2205
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 2209
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp3D0=0U;({unsigned int _tmpB78=e->loc;struct _dyneither_ptr _tmpB77=({const char*_tmp3D1="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D1,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpB78,_tmpB77,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2215
return 0;}
# 2219
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 2223
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp3D2=0U;({unsigned int _tmpB7A=e->loc;struct _dyneither_ptr _tmpB79=({const char*_tmp3D3="integral size mismatch; conversion supplied";_tag_dyneither(_tmp3D3,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpB7A,_tmpB79,_tag_dyneither(_tmp3D2,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2229
return 0;}
# 2234
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Tcutil_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 2240
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmpB7B=t;Cyc_Tcutil_unify(_tmpB7B,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 2245
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp3D4=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp3D5=_tmp3D4;struct Cyc_Absyn_Exp*_tmp3D7;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D5)->tag == 9U){_LL1: _tmp3D7=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D5)->f1;_LL2:
 return _tmp3D7;}else{_LL3: _LL4: {
# 2250
struct Cyc_Absyn_Exp*_tmp3D6=Cyc_Absyn_valueof_exp(_tmp3D4,0U);
_tmp3D6->topt=Cyc_Absyn_uint_type;
return _tmp3D6;}}_LL0:;}
# 2259
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Tcutil_unify(def,b);{
void*_tmp3D8=Cyc_Tcutil_compress(b);void*_tmp3D9=_tmp3D8;struct Cyc_List_List*_tmp3DD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp3DD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp3DD);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp3DC=({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0U,({struct _dyneither_ptr _tmpB7C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(b));_tmp9BD.f1=_tmpB7C;});_tmp9BD;});void*_tmp3DA[1U];_tmp3DA[0]=& _tmp3DC;({struct _dyneither_ptr _tmpB7D=({const char*_tmp3DB="get_bounds_exp: %s";_tag_dyneither(_tmp3DB,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB7D,_tag_dyneither(_tmp3DA,sizeof(void*),1U));});});}_LL0:;};}
# 2268
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp3DE=Cyc_Tcutil_compress(t);void*_tmp3DF=_tmp3DE;void*_tmp3E3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->tag == 3U){_LL1: _tmp3E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->f1).ptr_atts).bounds;_LL2:
# 2271
 return Cyc_Tcutil_get_bounds_exp(def,_tmp3E3);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp3E2=({struct Cyc_String_pa_PrintArg_struct _tmp9BE;_tmp9BE.tag=0U,({struct _dyneither_ptr _tmpB7E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9BE.f1=_tmpB7E;});_tmp9BE;});void*_tmp3E0[1U];_tmp3E0[0]=& _tmp3E2;({struct _dyneither_ptr _tmpB7F=({const char*_tmp3E1="get_ptr_bounds_exp not pointer: %s";_tag_dyneither(_tmp3E1,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB7F,_tag_dyneither(_tmp3E0,sizeof(void*),1U));});});}_LL0:;}
# 2277
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));({struct Cyc_List_List*_tmpB80=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E4->v=_tmpB80;});_tmp3E4;}));else{
# 2281
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->v=0;_tmp3E5;}));}}
# 2284
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));({struct Cyc_List_List*_tmpB81=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3E6->v=_tmpB81;});_tmp3E6;}));else{
# 2288
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->v=0;_tmp3E7;}));}}
# 2292
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple24{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 2300
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2302
({void*_tmpB82=Cyc_Tcutil_compress(t1);t1=_tmpB82;});
({void*_tmpB83=Cyc_Tcutil_compress(t2);t2=_tmpB83;});{
struct _tuple0 _tmp3E8=({struct _tuple0 _tmp9C2;_tmp9C2.f1=t1,_tmp9C2.f2=t2;_tmp9C2;});struct _tuple0 _tmp3E9=_tmp3E8;void*_tmp3FE;struct Cyc_Absyn_Tqual _tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;void*_tmp3FB;void*_tmp3FA;struct Cyc_Absyn_Tqual _tmp3F9;struct Cyc_Absyn_Exp*_tmp3F8;void*_tmp3F7;struct Cyc_Absyn_PtrInfo _tmp3F6;struct Cyc_Absyn_PtrInfo _tmp3F5;switch(*((int*)_tmp3E9.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9.f2)->tag == 3U){_LL1: _tmp3F6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9.f1)->f1;_tmp3F5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9.f2)->f1;_LL2: {
# 2306
int okay=1;
# 2308
if(!Cyc_Tcutil_unify((_tmp3F6.ptr_atts).nullable,(_tmp3F5.ptr_atts).nullable))
# 2310
({int _tmpB84=!Cyc_Tcutil_force_type2bool(0,(_tmp3F6.ptr_atts).nullable);okay=_tmpB84;});
# 2312
if(!Cyc_Tcutil_unify((_tmp3F6.ptr_atts).bounds,(_tmp3F5.ptr_atts).bounds)){
struct _tuple24 _tmp3EA=({struct _tuple24 _tmp9BF;({struct Cyc_Absyn_Exp*_tmpB88=({void*_tmpB87=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB87,(_tmp3F6.ptr_atts).bounds);});_tmp9BF.f1=_tmpB88;}),({
struct Cyc_Absyn_Exp*_tmpB86=({void*_tmpB85=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpB85,(_tmp3F5.ptr_atts).bounds);});_tmp9BF.f2=_tmpB86;});_tmp9BF;});
# 2313
struct _tuple24 _tmp3EB=_tmp3EA;struct Cyc_Absyn_Exp*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3EE;if(_tmp3EB.f2 == 0){_LLA: _LLB:
# 2316
 okay=1;goto _LL9;}else{if(_tmp3EB.f1 == 0){_LLC: _LLD:
# 2319
 if(Cyc_Tcutil_force_type2bool(0,(_tmp3F6.ptr_atts).zero_term) && ({
void*_tmpB89=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpB89,(_tmp3F5.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp3EF=_tmp3EB.f1;_tmp3EE=_tmp3EB.f2;_LLF:
# 2325
({int _tmpB8B=okay  && ({struct Cyc_Absyn_Exp*_tmpB8A=(struct Cyc_Absyn_Exp*)_check_null(_tmp3EE);Cyc_Evexp_lte_const_exp(_tmpB8A,(struct Cyc_Absyn_Exp*)_check_null(_tmp3EF));});okay=_tmpB8B;});
# 2329
if(!Cyc_Tcutil_force_type2bool(0,(_tmp3F5.ptr_atts).zero_term))
({void*_tmp3EC=0U;({unsigned int _tmpB8D=loc;struct _dyneither_ptr _tmpB8C=({const char*_tmp3ED="implicit cast to shorter array";_tag_dyneither(_tmp3ED,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpB8D,_tmpB8C,_tag_dyneither(_tmp3EC,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 2336
okay=okay  && (!(_tmp3F6.elt_tq).real_const  || (_tmp3F5.elt_tq).real_const);
# 2339
if(!Cyc_Tcutil_unify((_tmp3F6.ptr_atts).rgn,(_tmp3F5.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp3F6.ptr_atts).rgn,(_tmp3F5.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp3F2=({struct Cyc_String_pa_PrintArg_struct _tmp9C1;_tmp9C1.tag=0U,({
struct _dyneither_ptr _tmpB8E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F6.ptr_atts).rgn));_tmp9C1.f1=_tmpB8E;});_tmp9C1;});struct Cyc_String_pa_PrintArg_struct _tmp3F3=({struct Cyc_String_pa_PrintArg_struct _tmp9C0;_tmp9C0.tag=0U,({
struct _dyneither_ptr _tmpB8F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((_tmp3F5.ptr_atts).rgn));_tmp9C0.f1=_tmpB8F;});_tmp9C0;});void*_tmp3F0[2U];_tmp3F0[0]=& _tmp3F2,_tmp3F0[1]=& _tmp3F3;({unsigned int _tmpB91=loc;struct _dyneither_ptr _tmpB90=({const char*_tmp3F1="implicit cast from region %s to region %s";_tag_dyneither(_tmp3F1,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpB91,_tmpB90,_tag_dyneither(_tmp3F0,sizeof(void*),2U));});});}else{
okay=0;}}
# 2349
({int _tmpB92=okay  && (Cyc_Tcutil_unify((_tmp3F6.ptr_atts).zero_term,(_tmp3F5.ptr_atts).zero_term) || 
# 2351
Cyc_Tcutil_force_type2bool(1,(_tmp3F6.ptr_atts).zero_term) && (_tmp3F5.elt_tq).real_const);
# 2349
okay=_tmpB92;});{
# 2359
int _tmp3F4=
({void*_tmpB93=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpB93,(_tmp3F5.ptr_atts).bounds);}) && !
Cyc_Tcutil_force_type2bool(0,(_tmp3F5.ptr_atts).zero_term);
# 2365
({int _tmpB94=okay  && (Cyc_Tcutil_unify(_tmp3F6.elt_type,_tmp3F5.elt_type) || 
(_tmp3F4  && ((_tmp3F5.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3F5.elt_type)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp3F6.elt_type,_tmp3F5.elt_type));
# 2365
okay=_tmpB94;});
# 2368
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->tag == 4U){_LL3: _tmp3FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).elt_type;_tmp3FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).tq;_tmp3FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).num_elts;_tmp3FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f1)->f1).zero_term;_tmp3FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).elt_type;_tmp3F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).tq;_tmp3F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).num_elts;_tmp3F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E9.f2)->f1).zero_term;_LL4: {
# 2372
int okay;
# 2375
({int _tmpB95=Cyc_Tcutil_unify(_tmp3FB,_tmp3F7) && (
(_tmp3FC != 0  && _tmp3F8 != 0) && Cyc_Evexp_same_const_exp(_tmp3FC,_tmp3F8));
# 2375
okay=_tmpB95;});
# 2378
return(okay  && Cyc_Tcutil_unify(_tmp3FE,_tmp3FA)) && (!_tmp3FD.real_const  || _tmp3F9.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E9.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E9.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E9.f2)->f1)->tag == 1U){_LL5: _LL6:
# 2380
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 2382
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2386
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp3FF=Cyc_Tcutil_compress(t);void*_tmp400=_tmp3FF;void*_tmp403;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp400)->tag == 3U){_LL1: _tmp403=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp400)->f1).elt_type;_LL2:
 return _tmp403;}else{_LL3: _LL4:
({void*_tmp401=0U;({struct _dyneither_ptr _tmpB96=({const char*_tmp402="pointer_elt_type";_tag_dyneither(_tmp402,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB96,_tag_dyneither(_tmp401,sizeof(void*),0U));});});}_LL0:;}
# 2392
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp404=Cyc_Tcutil_compress(t);void*_tmp405=_tmp404;struct Cyc_Absyn_PtrAtts*_tmp408;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405)->tag == 3U){_LL1: _tmp408=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp405)->f1).ptr_atts;_LL2:
 return _tmp408->rgn;}else{_LL3: _LL4:
({void*_tmp406=0U;({struct _dyneither_ptr _tmpB97=({const char*_tmp407="pointer_elt_type";_tag_dyneither(_tmp407,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB97,_tag_dyneither(_tmp406,sizeof(void*),0U));});});}_LL0:;}
# 2399
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp409=Cyc_Tcutil_compress(t);void*_tmp40A=_tmp409;void*_tmp40B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40A)->tag == 3U){_LL1: _tmp40B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40A)->f1).ptr_atts).rgn;_LL2:
# 2402
*rgn=_tmp40B;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2409
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp40C=e->r;void*_tmp40D=_tmp40C;void*_tmp411;struct Cyc_Absyn_Exp*_tmp410;struct _dyneither_ptr _tmp40F;switch(*((int*)_tmp40D)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40D)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40D)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40D)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40D)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40D)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp40F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40D)->f1).Wchar_c).val;_LLC: {
# 2418
unsigned long _tmp40E=Cyc_strlen((struct _dyneither_ptr)_tmp40F);
int i=0;
if(_tmp40E >= 2  && *((const char*)_check_dyneither_subscript(_tmp40F,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp40F,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp40F.curr)[1]== 'x'  && _tmp40E >= 3) && *((const char*)_check_dyneither_subscript(_tmp40F,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp40E;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp40F,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2428
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2416
 return 1;case 14U: _LLD: _tmp411=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40D)->f1;_tmp410=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40D)->f2;_LLE:
# 2429
 return Cyc_Tcutil_is_zero(_tmp410) && Cyc_Tcutil_admits_zero(_tmp411);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2434
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 2443
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2448
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2453
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 2462
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2467
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2472
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp412=ka;enum Cyc_Absyn_KindQual _tmp41B;enum Cyc_Absyn_AliasQual _tmp41A;_LL1: _tmp41B=_tmp412->kind;_tmp41A=_tmp412->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp413=_tmp41A;switch(_tmp413){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2476
enum Cyc_Absyn_KindQual _tmp414=_tmp41B;switch(_tmp414){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL15: _LL16:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL17: _LL18:
 return& Cyc_Tcutil_bko;default: _LL19: _LL1A:
 return& Cyc_Tcutil_ptrbko;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2487
{enum Cyc_Absyn_KindQual _tmp415=_tmp41B;switch(_tmp415){case Cyc_Absyn_AnyKind: _LL1C: _LL1D:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL1E: _LL1F:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL20: _LL21:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL22: _LL23:
 return& Cyc_Tcutil_urko;default: _LL24: _LL25:
 goto _LL1B;}_LL1B:;}
# 2494
goto _LL3;default: _LL8: _LL9:
# 2496
{enum Cyc_Absyn_KindQual _tmp416=_tmp41B;switch(_tmp416){case Cyc_Absyn_AnyKind: _LL27: _LL28:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL29: _LL2A:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_trko;default: _LL2F: _LL30:
 goto _LL26;}_LL26:;}
# 2503
goto _LL3;}_LL3:;}
# 2505
({struct Cyc_String_pa_PrintArg_struct _tmp419=({struct Cyc_String_pa_PrintArg_struct _tmp9C3;_tmp9C3.tag=0U,({struct _dyneither_ptr _tmpB98=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp9C3.f1=_tmpB98;});_tmp9C3;});void*_tmp417[1U];_tmp417[0]=& _tmp419;({struct _dyneither_ptr _tmpB99=({const char*_tmp418="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp418,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB99,_tag_dyneither(_tmp417,sizeof(void*),1U));});});}
# 2508
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->tag=0U,_tmp41C->f1=k;_tmp41C;});}
# 2511
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp41D=_cycalloc(sizeof(*_tmp41D));({void*_tmpB9A=Cyc_Tcutil_kind_to_bound(k);_tmp41D->v=_tmpB9A;});_tmp41D;});}
# 2517
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpB9B=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->tag=0U,_tmp41E->f1=Cyc_Absyn_Null_c;_tmp41E;});e1->r=_tmpB9B;});{
struct Cyc_Core_Opt*_tmp41F=Cyc_Tcenv_lookup_opt_type_vars(te);
void*_tmp420=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9C6;({void*_tmpBA0=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp41F);_tmp9C6.elt_type=_tmpBA0;}),({
struct Cyc_Absyn_Tqual _tmpB9F=Cyc_Absyn_empty_tqual(0U);_tmp9C6.elt_tq=_tmpB9F;}),
({void*_tmpB9E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp41F);(_tmp9C6.ptr_atts).rgn=_tmpB9E;}),(_tmp9C6.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 2525
void*_tmpB9D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,_tmp41F);(_tmp9C6.ptr_atts).bounds=_tmpB9D;}),({
void*_tmpB9C=Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,_tmp41F);(_tmp9C6.ptr_atts).zero_term=_tmpB9C;}),(_tmp9C6.ptr_atts).ptrloc=0;_tmp9C6;}));
e1->topt=_tmp420;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp423=({struct Cyc_String_pa_PrintArg_struct _tmp9C5;_tmp9C5.tag=0U,({
struct _dyneither_ptr _tmpBA1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9C5.f1=_tmpBA1;});_tmp9C5;});struct Cyc_String_pa_PrintArg_struct _tmp424=({struct Cyc_String_pa_PrintArg_struct _tmp9C4;_tmp9C4.tag=0U,({struct _dyneither_ptr _tmpBA2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Position_string_of_segment(e1->loc));_tmp9C4.f1=_tmpBA2;});_tmp9C4;});void*_tmp421[2U];_tmp421[0]=& _tmp423,_tmp421[1]=& _tmp424;({struct _dyneither_ptr _tmpBA3=({const char*_tmp422="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp422,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBA3,_tag_dyneither(_tmp421,sizeof(void*),2U));});});
return retv;};};}
# 2535
return 0;}
# 2538
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp425=c;switch(_tmp425){case Cyc_Absyn_Unknown_coercion: _LL1: _LL2:
 return({const char*_tmp426="unknown";_tag_dyneither(_tmp426,sizeof(char),8U);});case Cyc_Absyn_No_coercion: _LL3: _LL4:
 return({const char*_tmp427="no coercion";_tag_dyneither(_tmp427,sizeof(char),12U);});case Cyc_Absyn_Null_to_NonNull: _LL5: _LL6:
 return({const char*_tmp428="null check";_tag_dyneither(_tmp428,sizeof(char),11U);});default: _LL7: _LL8:
 return({const char*_tmp429="other coercion";_tag_dyneither(_tmp429,sizeof(char),15U);});}_LL0:;}
# 2547
int Cyc_Tcutil_warn_alias_coerce=0;
# 2553
struct _tuple14 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2557
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmpBA6=({struct _dyneither_ptr*_tmp43C=_cycalloc(sizeof(*_tmp43C));({struct _dyneither_ptr _tmpBA5=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp43B=({struct Cyc_Int_pa_PrintArg_struct _tmp9C8;_tmp9C8.tag=1U,_tmp9C8.f1=(unsigned long)counter ++;_tmp9C8;});void*_tmp439[1U];_tmp439[0]=& _tmp43B;({struct _dyneither_ptr _tmpBA4=({const char*_tmp43A="__aliasvar%d";_tag_dyneither(_tmp43A,sizeof(char),13U);});Cyc_aprintf(_tmpBA4,_tag_dyneither(_tmp439,sizeof(void*),1U));});});*_tmp43C=_tmpBA5;});_tmp43C;});_tmp43D->f2=_tmpBA6;});_tmp43D;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpBA7=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=4U,_tmp438->f1=vd;_tmp438;});Cyc_Absyn_varb_exp(_tmpBA7,e->loc);});
# 2566
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2568
{void*_tmp42A=Cyc_Tcutil_compress(e_type);void*_tmp42B=_tmp42A;void*_tmp437;struct Cyc_Absyn_Tqual _tmp436;void*_tmp435;void*_tmp434;void*_tmp433;void*_tmp432;struct Cyc_Absyn_PtrLoc*_tmp431;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->tag == 3U){_LL1: _tmp437=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).elt_type;_tmp436=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).elt_tq;_tmp435=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).rgn;_tmp434=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).nullable;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).bounds;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).zero_term;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).ptrloc;_LL2:
# 2570
{void*_tmp42C=Cyc_Tcutil_compress(_tmp435);void*_tmp42D=_tmp42C;void**_tmp430;struct Cyc_Core_Opt*_tmp42F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42D)->tag == 1U){_LL6: _tmp430=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42D)->f2;_tmp42F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42D)->f4;_LL7: {
# 2572
void*_tmp42E=Cyc_Absyn_var_type(tv);
*_tmp430=_tmp42E;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2577
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2581
e->topt=0;
vd->initializer=0;{
# 2585
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2587
return({struct _tuple14 _tmp9C7;_tmp9C7.f1=d,_tmp9C7.f2=ve;_tmp9C7;});};}
# 2592
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned int loc){
# 2595
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_type,0)) && 
Cyc_Tcutil_is_pointer_type(e_type)){
# 2600
void*_tmp43E=Cyc_Tcutil_compress(wants_type);void*_tmp43F=_tmp43E;void*_tmp441;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->tag == 3U){_LL1: _tmp441=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).rgn;_LL2:
# 2602
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp441))return 0;{
struct Cyc_Absyn_Kind*_tmp440=Cyc_Tcutil_type_kind(_tmp441);
return _tmp440->kind == Cyc_Absyn_RgnKind  && _tmp440->aliasqual == Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2608
return 0;}
# 2612
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2617
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2619
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2621
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp444=({struct Cyc_String_pa_PrintArg_struct _tmp9CA;_tmp9CA.tag=0U,({
struct _dyneither_ptr _tmpBA8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9CA.f1=_tmpBA8;});_tmp9CA;});struct Cyc_String_pa_PrintArg_struct _tmp445=({struct Cyc_String_pa_PrintArg_struct _tmp9C9;_tmp9C9.tag=0U,({struct _dyneither_ptr _tmpBA9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9C9.f1=_tmpBA9;});_tmp9C9;});void*_tmp442[2U];_tmp442[0]=& _tmp444,_tmp442[1]=& _tmp445;({unsigned int _tmpBAB=e->loc;struct _dyneither_ptr _tmpBAA=({const char*_tmp443="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp443,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpBAB,_tmpBAA,_tag_dyneither(_tmp442,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2629
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp448=({struct Cyc_String_pa_PrintArg_struct _tmp9CD;_tmp9CD.tag=0U,({
struct _dyneither_ptr _tmpBAC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp9CD.f1=_tmpBAC;});_tmp9CD;});struct Cyc_String_pa_PrintArg_struct _tmp449=({struct Cyc_String_pa_PrintArg_struct _tmp9CC;_tmp9CC.tag=0U,({struct _dyneither_ptr _tmpBAD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9CC.f1=_tmpBAD;});_tmp9CC;});struct Cyc_String_pa_PrintArg_struct _tmp44A=({struct Cyc_String_pa_PrintArg_struct _tmp9CB;_tmp9CB.tag=0U,({struct _dyneither_ptr _tmpBAE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9CB.f1=_tmpBAE;});_tmp9CB;});void*_tmp446[3U];_tmp446[0]=& _tmp448,_tmp446[1]=& _tmp449,_tmp446[2]=& _tmp44A;({unsigned int _tmpBB0=e->loc;struct _dyneither_ptr _tmpBAF=({const char*_tmp447="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp447,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpBB0,_tmpBAF,_tag_dyneither(_tmp446,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2636
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({enum Cyc_Absyn_Coercion _tmpBB1=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpBB1;})!= Cyc_Absyn_Unknown_coercion){
# 2644
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp44D=({struct Cyc_String_pa_PrintArg_struct _tmp9CF;_tmp9CF.tag=0U,({
struct _dyneither_ptr _tmpBB2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp9CF.f1=_tmpBB2;});_tmp9CF;});struct Cyc_String_pa_PrintArg_struct _tmp44E=({struct Cyc_String_pa_PrintArg_struct _tmp9CE;_tmp9CE.tag=0U,({struct _dyneither_ptr _tmpBB3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp9CE.f1=_tmpBB3;});_tmp9CE;});void*_tmp44B[2U];_tmp44B[0]=& _tmp44D,_tmp44B[1]=& _tmp44E;({unsigned int _tmpBB5=e->loc;struct _dyneither_ptr _tmpBB4=({const char*_tmp44C="implicit cast from %s to %s";_tag_dyneither(_tmp44C,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpBB5,_tmpBB4,_tag_dyneither(_tmp44B,sizeof(void*),2U));});});
return 1;}else{
# 2650
return 0;}}}}}
# 2657
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2660
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2674 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple25{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2678
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple25*env,struct Cyc_Absyn_Aggrfield*x){
# 2681
struct _tuple25 _tmp44F=*env;struct _tuple25 _tmp450=_tmp44F;struct Cyc_List_List*_tmp458;struct _RegionHandle*_tmp457;struct Cyc_Tcenv_Tenv*_tmp456;int _tmp455;_LL1: _tmp458=_tmp450.f1;_tmp457=_tmp450.f2;_tmp456=_tmp450.f3;_tmp455=_tmp450.f4;_LL2:;{
# 2683
void*_tmp451=_tmp458 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp457,_tmp458,x->type);
struct Cyc_List_List*_tmp452=Cyc_Tcutil_flatten_type(_tmp457,_tmp455,_tmp456,_tmp451);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp452)== 1)
return({struct Cyc_List_List*_tmp454=_region_malloc(_tmp457,sizeof(*_tmp454));({struct _tuple12*_tmpBB6=({struct _tuple12*_tmp453=_region_malloc(_tmp457,sizeof(*_tmp453));_tmp453->f1=x->tq,_tmp453->f2=_tmp451;_tmp453;});_tmp454->hd=_tmpBB6;}),_tmp454->tl=0;_tmp454;});else{
return _tmp452;}};}struct _tuple26{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2689
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple26*env,struct _tuple12*x){
# 2691
struct _tuple26 _tmp459=*env;struct _tuple26 _tmp45A=_tmp459;struct _RegionHandle*_tmp464;struct Cyc_Tcenv_Tenv*_tmp463;int _tmp462;_LL1: _tmp464=_tmp45A.f1;_tmp463=_tmp45A.f2;_tmp462=_tmp45A.f3;_LL2:;{
struct _tuple12 _tmp45B=*x;struct _tuple12 _tmp45C=_tmp45B;struct Cyc_Absyn_Tqual _tmp461;void*_tmp460;_LL4: _tmp461=_tmp45C.f1;_tmp460=_tmp45C.f2;_LL5:;{
struct Cyc_List_List*_tmp45D=Cyc_Tcutil_flatten_type(_tmp464,_tmp462,_tmp463,_tmp460);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp45D)== 1)
return({struct Cyc_List_List*_tmp45F=_region_malloc(_tmp464,sizeof(*_tmp45F));({struct _tuple12*_tmpBB7=({struct _tuple12*_tmp45E=_region_malloc(_tmp464,sizeof(*_tmp45E));_tmp45E->f1=_tmp461,_tmp45E->f2=_tmp460;_tmp45E;});_tmp45F->hd=_tmpBB7;}),_tmp45F->tl=0;_tmp45F;});else{
return _tmp45D;}};};}
# 2698
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2702
if(flatten){
({void*_tmpBB8=Cyc_Tcutil_compress(t1);t1=_tmpBB8;});{
void*_tmp465=t1;struct Cyc_List_List*_tmp484;struct Cyc_List_List*_tmp483;struct Cyc_Absyn_Aggrdecl*_tmp482;struct Cyc_List_List*_tmp481;switch(*((int*)_tmp465)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp465)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp465)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp482=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp465)->f1)->f1).KnownAggr).val;_tmp481=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp465)->f2;_LL6:
# 2720
 if(((_tmp482->kind == Cyc_Absyn_UnionA  || _tmp482->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->rgn_po != 0)
# 2722
return({struct Cyc_List_List*_tmp470=_region_malloc(r,sizeof(*_tmp470));({struct _tuple12*_tmpBBA=({struct _tuple12*_tmp46F=_region_malloc(r,sizeof(*_tmp46F));({struct Cyc_Absyn_Tqual _tmpBB9=Cyc_Absyn_empty_tqual(0U);_tmp46F->f1=_tmpBB9;}),_tmp46F->f2=t1;_tmp46F;});_tmp470->hd=_tmpBBA;}),_tmp470->tl=0;_tmp470;});{
struct Cyc_List_List*_tmp471=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp482->tvs,_tmp481);
struct _tuple25 env=({struct _tuple25 _tmp9D0;_tmp9D0.f1=_tmp471,_tmp9D0.f2=r,_tmp9D0.f3=te,_tmp9D0.f4=flatten;_tmp9D0;});
struct Cyc_List_List*_tmp472=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp482->impl))->fields;struct Cyc_List_List*_tmp473=_tmp472;struct Cyc_Absyn_Aggrfield*_tmp479;struct Cyc_List_List*_tmp478;if(_tmp473 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp479=(struct Cyc_Absyn_Aggrfield*)_tmp473->hd;_tmp478=_tmp473->tl;_LL14: {
# 2728
struct Cyc_List_List*_tmp474=Cyc_Tcutil_flatten_type_f(& env,_tmp479);
env.f4=0;{
struct Cyc_List_List*_tmp475=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp478);
struct Cyc_List_List*_tmp476=({struct Cyc_List_List*_tmp477=_region_malloc(r,sizeof(*_tmp477));_tmp477->hd=_tmp474,_tmp477->tl=_tmp475;_tmp477;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp476);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp483=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp465)->f1;_LL4: {
# 2707
struct _tuple26 _tmp466=({struct _tuple26 _tmp9D1;_tmp9D1.f1=r,_tmp9D1.f2=te,_tmp9D1.f3=flatten;_tmp9D1;});
# 2709
struct Cyc_List_List*_tmp467=_tmp483;struct _tuple12*_tmp46E;struct Cyc_List_List*_tmp46D;if(_tmp467 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp46E=(struct _tuple12*)_tmp467->hd;_tmp46D=_tmp467->tl;_LLF: {
# 2712
struct Cyc_List_List*_tmp468=Cyc_Tcutil_rcopy_tqt(& _tmp466,_tmp46E);
_tmp466.f3=0;{
struct Cyc_List_List*_tmp469=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple26*,struct _tuple12*),struct _tuple26*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp466,_tmp483);
struct Cyc_List_List*_tmp46A=({struct Cyc_List_List*_tmp46C=_region_malloc(r,sizeof(*_tmp46C));_tmp46C->hd=_tmp468,_tmp46C->tl=_tmp469;_tmp46C;});
return({struct _RegionHandle*_tmpBBB=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpBBB,({struct Cyc_List_List*_tmp46B=_region_malloc(r,sizeof(*_tmp46B));_tmp46B->hd=_tmp468,_tmp46B->tl=_tmp469;_tmp46B;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp465)->f1 == Cyc_Absyn_StructA){_LL7: _tmp484=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp465)->f2;_LL8: {
# 2735
struct _tuple25 env=({struct _tuple25 _tmp9D2;_tmp9D2.f1=0,_tmp9D2.f2=r,_tmp9D2.f3=te,_tmp9D2.f4=flatten;_tmp9D2;});
struct Cyc_List_List*_tmp47A=_tmp484;struct Cyc_Absyn_Aggrfield*_tmp480;struct Cyc_List_List*_tmp47F;if(_tmp47A == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp480=(struct Cyc_Absyn_Aggrfield*)_tmp47A->hd;_tmp47F=_tmp47A->tl;_LL19: {
# 2739
struct Cyc_List_List*_tmp47B=Cyc_Tcutil_flatten_type_f(& env,_tmp480);
env.f4=0;{
struct Cyc_List_List*_tmp47C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct Cyc_Absyn_Aggrfield*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp47F);
struct Cyc_List_List*_tmp47D=({struct Cyc_List_List*_tmp47E=_region_malloc(r,sizeof(*_tmp47E));_tmp47E->hd=_tmp47B,_tmp47E->tl=_tmp47C;_tmp47E;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp47D);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2745
 goto _LL0;}_LL0:;};}
# 2748
return({struct Cyc_List_List*_tmp486=_region_malloc(r,sizeof(*_tmp486));({struct _tuple12*_tmpBBD=({struct _tuple12*_tmp485=_region_malloc(r,sizeof(*_tmp485));({struct Cyc_Absyn_Tqual _tmpBBC=Cyc_Absyn_empty_tqual(0U);_tmp485->f1=_tmpBBC;}),_tmp485->f2=t1;_tmp485;});_tmp486->hd=_tmpBBD;}),_tmp486->tl=0;_tmp486;});}
# 2752
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp487=(void*)t->hd;void*_tmp488=_tmp487;switch(*((int*)_tmp488)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2758
 continue;default: _LL7: _LL8:
# 2760
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2763
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2766
return 1;}
# 2769
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp489=({struct _tuple0 _tmp9D3;({void*_tmpBBF=Cyc_Tcutil_compress(t1);_tmp9D3.f1=_tmpBBF;}),({void*_tmpBBE=Cyc_Tcutil_compress(t2);_tmp9D3.f2=_tmpBBE;});_tmp9D3;});struct _tuple0 _tmp48A=_tmp489;enum Cyc_Absyn_Size_of _tmp48C;enum Cyc_Absyn_Size_of _tmp48B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48A.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48A.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48A.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48A.f2)->f1)->tag == 1U){_LL1: _tmp48C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48A.f1)->f1)->f2;_tmp48B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48A.f2)->f1)->f2;_LL2:
# 2772
 return(_tmp48C == _tmp48B  || _tmp48C == Cyc_Absyn_Int_sz  && _tmp48B == Cyc_Absyn_Long_sz) || 
_tmp48C == Cyc_Absyn_Long_sz  && _tmp48B == Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2780
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2783
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
({void*_tmpBC0=Cyc_Tcutil_compress(t1);t1=_tmpBC0;});
({void*_tmpBC1=Cyc_Tcutil_compress(t2);t2=_tmpBC1;});{
struct _tuple0 _tmp48D=({struct _tuple0 _tmp9D4;_tmp9D4.f1=t1,_tmp9D4.f2=t2;_tmp9D4;});struct _tuple0 _tmp48E=_tmp48D;struct Cyc_Absyn_FnInfo _tmp4B8;struct Cyc_Absyn_FnInfo _tmp4B7;struct Cyc_Absyn_Datatypedecl*_tmp4B6;struct Cyc_Absyn_Datatypefield*_tmp4B5;struct Cyc_List_List*_tmp4B4;struct Cyc_Absyn_Datatypedecl*_tmp4B3;struct Cyc_List_List*_tmp4B2;void*_tmp4B1;struct Cyc_Absyn_Tqual _tmp4B0;void*_tmp4AF;void*_tmp4AE;void*_tmp4AD;void*_tmp4AC;void*_tmp4AB;struct Cyc_Absyn_Tqual _tmp4AA;void*_tmp4A9;void*_tmp4A8;void*_tmp4A7;void*_tmp4A6;switch(*((int*)_tmp48E.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f2)->tag == 3U){_LL1: _tmp4B1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f1)->f1).elt_type;_tmp4B0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f1)->f1).elt_tq;_tmp4AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f1)->f1).ptr_atts).rgn;_tmp4AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f1)->f1).ptr_atts).nullable;_tmp4AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f1)->f1).ptr_atts).bounds;_tmp4AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f1)->f1).ptr_atts).zero_term;_tmp4AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f2)->f1).elt_type;_tmp4AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f2)->f1).elt_tq;_tmp4A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f2)->f1).ptr_atts).rgn;_tmp4A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f2)->f1).ptr_atts).nullable;_tmp4A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f2)->f1).ptr_atts).bounds;_tmp4A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E.f2)->f1).ptr_atts).zero_term;_LL2:
# 2795
 if(_tmp4B0.real_const  && !_tmp4AA.real_const)
return 0;
# 2798
if((!Cyc_Tcutil_unify(_tmp4AE,_tmp4A8) && 
Cyc_Absyn_type2bool(0,_tmp4AE)) && !Cyc_Absyn_type2bool(0,_tmp4A8))
return 0;
# 2802
if((Cyc_Tcutil_unify(_tmp4AC,_tmp4A6) && !
Cyc_Absyn_type2bool(0,_tmp4AC)) && Cyc_Absyn_type2bool(0,_tmp4A6))
return 0;
# 2806
if((!Cyc_Tcutil_unify(_tmp4AF,_tmp4A9) && !Cyc_Tcenv_region_outlives(te,_tmp4AF,_tmp4A9)) && !
Cyc_Tcutil_subtype(te,assume,_tmp4AF,_tmp4A9))
return 0;
# 2810
if(!Cyc_Tcutil_unify(_tmp4AD,_tmp4A7)){
struct Cyc_Absyn_Exp*_tmp48F=({void*_tmpBC2=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBC2,_tmp4AD);});
struct Cyc_Absyn_Exp*_tmp490=({void*_tmpBC3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBC3,_tmp4A7);});
if(_tmp48F != _tmp490){
if((_tmp48F == 0  || _tmp490 == 0) || !Cyc_Evexp_lte_const_exp(_tmp490,_tmp490))
return 0;}}
# 2820
if(!_tmp4AA.real_const  && _tmp4B0.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4AB)))
return 0;}{
# 2826
int _tmp491=
({void*_tmpBC4=_tmp4A7;Cyc_Tcutil_unify(_tmpBC4,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4A6);
# 2831
return(_tmp491  && ({struct Cyc_Tcenv_Tenv*_tmpBC8=te;struct Cyc_List_List*_tmpBC7=({struct Cyc_List_List*_tmp493=_cycalloc(sizeof(*_tmp493));({struct _tuple0*_tmpBC5=({struct _tuple0*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->f1=t1,_tmp492->f2=t2;_tmp492;});_tmp493->hd=_tmpBC5;}),_tmp493->tl=assume;_tmp493;});void*_tmpBC6=_tmp4B1;Cyc_Tcutil_ptrsubtype(_tmpBC8,_tmpBC7,_tmpBC6,_tmp4AB);}) || Cyc_Tcutil_unify(_tmp4B1,_tmp4AB)) || Cyc_Tcutil_isomorphic(_tmp4B1,_tmp4AB);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp4B6=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp4B5=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp4B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f1)->f2;_tmp4B3=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f2)->f1)->f1).KnownDatatype).val;_tmp4B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp48E.f2)->f2;_LL4:
# 2838
 if(_tmp4B6 != _tmp4B3  && Cyc_Absyn_qvar_cmp(_tmp4B6->name,_tmp4B3->name)!= 0)return 0;
# 2840
if(({int _tmpBC9=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B4);_tmpBC9 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4B2);}))return 0;
for(0;_tmp4B4 != 0;(_tmp4B4=_tmp4B4->tl,_tmp4B2=_tmp4B2->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp4B4->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp4B2))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48E.f2)->tag == 5U){_LL5: _tmp4B8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48E.f1)->f1;_tmp4B7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp48E.f2)->f1;_LL6:
# 2847
 if(_tmp4B8.tvars != 0  || _tmp4B7.tvars != 0){
struct Cyc_List_List*_tmp494=_tmp4B8.tvars;
struct Cyc_List_List*_tmp495=_tmp4B7.tvars;
if(({int _tmpBCA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp494);_tmpBCA != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp495);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp494 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp494->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp495))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpBCD=({struct Cyc_List_List*_tmp497=_cycalloc(sizeof(*_tmp497));({struct _tuple15*_tmpBCC=({struct _tuple15*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->f1=(struct Cyc_Absyn_Tvar*)_tmp495->hd,({void*_tmpBCB=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp494->hd);_tmp496->f2=_tmpBCB;});_tmp496;});_tmp497->hd=_tmpBCC;}),_tmp497->tl=inst;_tmp497;});inst=_tmpBCD;});
_tmp494=_tmp494->tl;
_tmp495=_tmp495->tl;}
# 2858
if(inst != 0){
_tmp4B8.tvars=0;
_tmp4B7.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpBD0=te;struct Cyc_List_List*_tmpBCF=assume;void*_tmpBCE=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->tag=5U,_tmp498->f1=_tmp4B8;_tmp498;});Cyc_Tcutil_subtype(_tmpBD0,_tmpBCF,_tmpBCE,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->tag=5U,_tmp499->f1=_tmp4B7;_tmp499;}));});}};}
# 2865
if(!Cyc_Tcutil_subtype(te,assume,_tmp4B8.ret_type,_tmp4B7.ret_type))return 0;{
struct Cyc_List_List*_tmp49A=_tmp4B8.args;
struct Cyc_List_List*_tmp49B=_tmp4B7.args;
# 2870
if(({int _tmpBD1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49A);_tmpBD1 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49B);}))return 0;
# 2872
for(0;_tmp49A != 0;(_tmp49A=_tmp49A->tl,_tmp49B=_tmp49B->tl)){
struct _tuple10 _tmp49C=*((struct _tuple10*)_tmp49A->hd);struct _tuple10 _tmp49D=_tmp49C;struct Cyc_Absyn_Tqual _tmp4A3;void*_tmp4A2;_LLA: _tmp4A3=_tmp49D.f2;_tmp4A2=_tmp49D.f3;_LLB:;{
struct _tuple10 _tmp49E=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp49B))->hd);struct _tuple10 _tmp49F=_tmp49E;struct Cyc_Absyn_Tqual _tmp4A1;void*_tmp4A0;_LLD: _tmp4A1=_tmp49F.f2;_tmp4A0=_tmp49F.f3;_LLE:;
# 2876
if(_tmp4A1.real_const  && !_tmp4A3.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp4A0,_tmp4A2))
return 0;};}
# 2880
if(_tmp4B8.c_varargs != _tmp4B7.c_varargs)return 0;
if(_tmp4B8.cyc_varargs != 0  && _tmp4B7.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp4A4=*_tmp4B8.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp4A5=*_tmp4B7.cyc_varargs;
# 2885
if((_tmp4A5.tq).real_const  && !(_tmp4A4.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp4A5.type,_tmp4A4.type))
return 0;}else{
if(_tmp4B8.cyc_varargs != 0  || _tmp4B7.cyc_varargs != 0)return 0;}
# 2890
if(!({void*_tmpBD2=(void*)_check_null(_tmp4B8.effect);Cyc_Tcutil_subset_effect(1,_tmpBD2,(void*)_check_null(_tmp4B7.effect));}))return 0;
# 2892
if(!Cyc_Tcutil_sub_rgnpo(_tmp4B8.rgn_po,_tmp4B7.rgn_po))return 0;
# 2894
if(!Cyc_Tcutil_sub_attributes(_tmp4B8.attributes,_tmp4B7.attributes))return 0;
# 2896
if(!Cyc_Tcutil_check_logical_implication(_tmp4B7.requires_relns,_tmp4B8.requires_relns))
return 0;
# 2899
if(!Cyc_Tcutil_check_logical_implication(_tmp4B8.ensures_relns,_tmp4B7.ensures_relns))
return 0;
# 2902
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2914 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2916
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp4B9=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp4BA=_tmp4B9;struct Cyc_Absyn_Tqual _tmp4C0;void*_tmp4BF;_LL1: _tmp4C0=_tmp4BA->f1;_tmp4BF=_tmp4BA->f2;_LL2:;{
struct _tuple12*_tmp4BB=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp4BC=_tmp4BB;struct Cyc_Absyn_Tqual _tmp4BE;void*_tmp4BD;_LL4: _tmp4BE=_tmp4BC->f1;_tmp4BD=_tmp4BC->f2;_LL5:;
# 2923
if(_tmp4C0.real_const  && !_tmp4BE.real_const)return 0;
# 2925
if((_tmp4BE.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4BD))) && 
Cyc_Tcutil_subtype(te,assume,_tmp4BF,_tmp4BD))
# 2928
continue;
# 2930
if(Cyc_Tcutil_unify(_tmp4BF,_tmp4BD))
# 2932
continue;
# 2934
if(Cyc_Tcutil_isomorphic(_tmp4BF,_tmp4BD))
# 2936
continue;
# 2939
return 0;};};}
# 2941
return 1;}
# 2946
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpBD3=Cyc_Tcutil_compress(t1);t1=_tmpBD3;});
({void*_tmpBD4=Cyc_Tcutil_compress(t2);t2=_tmpBD4;});
# 2952
{void*_tmp4C1=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2955
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2957
 if((Cyc_Tcutil_type_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2961
void*_tmp4C2=t1;void*_tmp4E7;struct Cyc_Absyn_Enumdecl*_tmp4E6;void*_tmp4E5;struct Cyc_Absyn_Tqual _tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;void*_tmp4E2;void*_tmp4E1;struct Cyc_Absyn_Tqual _tmp4E0;void*_tmp4DF;void*_tmp4DE;void*_tmp4DD;void*_tmp4DC;switch(*((int*)_tmp4C2)){case 3U: _LLA: _tmp4E1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).elt_type;_tmp4E0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).elt_tq;_tmp4DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).rgn;_tmp4DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).nullable;_tmp4DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).bounds;_tmp4DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C2)->f1).ptr_atts).zero_term;_LLB:
# 2970
{void*_tmp4C3=t2;void*_tmp4D2;struct Cyc_Absyn_Tqual _tmp4D1;void*_tmp4D0;void*_tmp4CF;void*_tmp4CE;void*_tmp4CD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C3)->tag == 3U){_LL19: _tmp4D2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C3)->f1).elt_type;_tmp4D1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C3)->f1).elt_tq;_tmp4D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C3)->f1).ptr_atts).rgn;_tmp4CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C3)->f1).ptr_atts).nullable;_tmp4CE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C3)->f1).ptr_atts).bounds;_tmp4CD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C3)->f1).ptr_atts).zero_term;_LL1A: {
# 2974
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp4C4=({struct Cyc_List_List*_tmp4CC=_cycalloc(sizeof(*_tmp4CC));({struct _tuple0*_tmpBD5=({struct _tuple0*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));_tmp4CB->f1=t1,_tmp4CB->f2=t2;_tmp4CB;});_tmp4CC->hd=_tmpBD5;}),_tmp4CC->tl=0;_tmp4CC;});
int _tmp4C5=_tmp4D1.real_const  || !_tmp4E0.real_const;
# 2988 "tcutil.cyc"
int _tmp4C6=
({void*_tmpBD6=_tmp4CE;Cyc_Tcutil_unify(_tmpBD6,Cyc_Absyn_bounds_one());}) && !Cyc_Tcutil_force_type2bool(0,_tmp4CD);
# 2991
int _tmp4C7=_tmp4C5  && (
((_tmp4C6  && Cyc_Tcutil_ptrsubtype(te,_tmp4C4,_tmp4E1,_tmp4D2) || 
Cyc_Tcutil_unify(_tmp4E1,_tmp4D2)) || Cyc_Tcutil_isomorphic(_tmp4E1,_tmp4D2)) || Cyc_Tcutil_unify(_tmp4D2,Cyc_Absyn_void_type));
# 2995
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=Cyc_Tcutil_unify(_tmp4DC,_tmp4CD) || !Cyc_Absyn_type2bool(0,_tmp4CD);
# 2999
int _tmp4C8=_tmp4C7?0:((Cyc_Tcutil_is_bits_only_type(_tmp4E1) && Cyc_Tcutil_is_char_type(_tmp4D2)) && !
Cyc_Tcutil_force_type2bool(0,_tmp4CD)) && (
_tmp4D1.real_const  || !_tmp4E0.real_const);
int bounds_ok=Cyc_Tcutil_unify(_tmp4DD,_tmp4CE);
if(!bounds_ok  && !_tmp4C8){
struct Cyc_Absyn_Exp*_tmp4C9=({void*_tmpBD7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBD7,_tmp4DD);});
struct Cyc_Absyn_Exp*_tmp4CA=({void*_tmpBD8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpBD8,_tmp4CE);});
if((_tmp4C9 != 0  && _tmp4CA != 0) && Cyc_Evexp_lte_const_exp(_tmp4CA,_tmp4C9))
bounds_ok=1;else{
if(_tmp4C9 == 0  || _tmp4CA == 0)
bounds_ok=1;}}{
# 3011
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4DE);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4CF);
if(t1_nullable  && !t2_nullable)
coercion=2U;
# 3018
if(((bounds_ok  && zeroterm_ok) && (_tmp4C7  || _tmp4C8)) && (
Cyc_Tcutil_unify(_tmp4DF,_tmp4D0) || Cyc_Tcenv_region_outlives(te,_tmp4DF,_tmp4D0)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 3024
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp4E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C2)->f1).elt_type;_tmp4E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C2)->f1).tq;_tmp4E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C2)->f1).num_elts;_tmp4E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C2)->f1).zero_term;_LLD:
# 3026
{void*_tmp4D3=t2;void*_tmp4D7;struct Cyc_Absyn_Tqual _tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;void*_tmp4D4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D3)->tag == 4U){_LL1E: _tmp4D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D3)->f1).elt_type;_tmp4D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D3)->f1).tq;_tmp4D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D3)->f1).num_elts;_tmp4D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4D3)->f1).zero_term;_LL1F: {
# 3028
int okay;
({int _tmpBD9=
(((_tmp4E3 != 0  && _tmp4D5 != 0) && Cyc_Tcutil_unify(_tmp4E2,_tmp4D4)) && 
Cyc_Evexp_lte_const_exp(_tmp4D5,_tmp4E3)) && 
Cyc_Evexp_lte_const_exp(_tmp4E3,_tmp4D5);
# 3029
okay=_tmpBD9;});
# 3033
return
# 3035
(okay  && Cyc_Tcutil_unify(_tmp4E5,_tmp4D7)) && (!_tmp4E4.real_const  || _tmp4D6.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 3037
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 3039
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->f1)){case 15U: _LLE: _tmp4E6=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->f1)->f2;_LLF:
# 3043
{void*_tmp4D8=t2;struct Cyc_Absyn_Enumdecl*_tmp4D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->f1)->tag == 15U){_LL23: _tmp4D9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->f1)->f2;_LL24:
# 3045
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4E6))->fields != 0  && ((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4D9))->fields != 0) && ({
int _tmpBDA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4E6->fields))->v);_tmpBDA >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4D9->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 3051
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 3054
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->f2 != 0){_LL14: _tmp4E7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C2)->f2)->hd;_LL15:
# 3057
{void*_tmp4DA=t2;void*_tmp4DB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DA)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DA)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DA)->f2 != 0){_LL28: _tmp4DB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4DA)->f2)->hd;_LL29:
# 3059
 if(Cyc_Tcenv_region_outlives(te,_tmp4E7,_tmp4DB))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 3063
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 3069
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp4E8=Cyc_Absyn_copy_exp(e);
({void*_tmpBDB=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->tag=14U,_tmp4E9->f1=t,_tmp4E9->f2=_tmp4E8,_tmp4E9->f3=0,_tmp4E9->f4=c;_tmp4E9;});e->r=_tmpBDB;});
e->topt=t;}}
# 3077
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple0 _tmp4EA=({struct _tuple0 _tmp9D6;({void*_tmpBDD=Cyc_Tcutil_compress(t1);_tmp9D6.f1=_tmpBDD;}),({void*_tmpBDC=Cyc_Tcutil_compress(t2);_tmp9D6.f2=_tmpBDC;});_tmp9D6;});struct _tuple0 _tmp4EB=_tmp4EA;void*_tmp4F1;void*_tmp4F0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EB.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EB.f2)->tag == 0U){_LL1: _tmp4F1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EB.f1)->f1;_tmp4F0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4EB.f2)->f1;_LL2:
# 3080
{struct _tuple0 _tmp4EC=({struct _tuple0 _tmp9D5;_tmp9D5.f1=_tmp4F1,_tmp9D5.f2=_tmp4F0;_tmp9D5;});struct _tuple0 _tmp4ED=_tmp4EC;int _tmp4EF;int _tmp4EE;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4ED.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4ED.f2)->tag == 2U){_LL6: _tmp4EF=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f1;_tmp4EE=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f1;_LL7:
# 3082
 if(_tmp4EF != 0  && _tmp4EF != 1)return t1;else{
if(_tmp4EE != 0  && _tmp4EE != 1)return t2;else{
if(_tmp4EF >= _tmp4EE)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp4ED.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1A;else{goto _LL20;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL18;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL1E;else{goto _LL20;}}}}}else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2){case Cyc_Absyn_Long_sz: _LL14: _LL15:
# 3092
 goto _LL17;case Cyc_Absyn_Int_sz: _LL18: _LL19:
# 3095
 goto _LL1B;default: goto _LL20;}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1A;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL1C;else{goto _LL20;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL1E;else{goto _LL20;}}}}}}else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL10: _LL11:
# 3090
 goto _LL13;case Cyc_Absyn_Long_sz: _LL1C: _LL1D:
# 3097
 goto _LL1F;default: goto _LL20;}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 3089
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 3093
 return Cyc_Absyn_ulong_type;case Cyc_Absyn_Int_sz: _LL1A: _LL1B:
# 3096
 return Cyc_Absyn_uint_type;default: goto _LL20;}else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp4ED.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL12: _LL13:
# 3091
 return Cyc_Absyn_slonglong_type;case Cyc_Absyn_Long_sz: _LL1E: _LL1F:
# 3098
 return Cyc_Absyn_slong_type;default: goto _LL20;}}}else{_LL20: _LL21:
 goto _LL5;}}}}_LL5:;}
# 3101
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3104
return Cyc_Absyn_sint_type;}
# 3114 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpBDE=Cyc_Absyn_compress_kb(c1);c1=_tmpBDE;});
({void*_tmpBDF=Cyc_Absyn_compress_kb(c2);c2=_tmpBDF;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp4F2=({struct _tuple0 _tmp9D7;_tmp9D7.f1=c1,_tmp9D7.f2=c2;_tmp9D7;});struct _tuple0 _tmp4F3=_tmp4F2;struct Cyc_Core_Opt**_tmp507;struct Cyc_Absyn_Kind*_tmp506;struct Cyc_Core_Opt**_tmp505;struct Cyc_Absyn_Kind*_tmp504;struct Cyc_Core_Opt**_tmp503;struct Cyc_Absyn_Kind*_tmp502;struct Cyc_Absyn_Kind*_tmp501;struct Cyc_Core_Opt**_tmp500;struct Cyc_Core_Opt**_tmp4FF;struct Cyc_Absyn_Kind*_tmp4FE;struct Cyc_Core_Opt**_tmp4FD;struct Cyc_Absyn_Kind*_tmp4FC;struct Cyc_Absyn_Kind*_tmp4FB;struct Cyc_Absyn_Kind*_tmp4FA;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->tag == 0U)switch(*((int*)_tmp4F3.f2)){case 0U: _LL1: _tmp4FB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->f1;_tmp4FA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->f1;_LL2:
 return _tmp4FB == _tmp4FA;case 1U: goto _LL3;default: _LL9: _tmp4FE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->f1;_tmp4FD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->f1;_tmp4FC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->f2;_LLA:
# 3127
 if(Cyc_Tcutil_kind_leq(_tmp4FE,_tmp4FC)){
({struct Cyc_Core_Opt*_tmpBE0=({struct Cyc_Core_Opt*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));_tmp4F7->v=c1;_tmp4F7;});*_tmp4FD=_tmpBE0;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->tag == 1U){_LL3: _tmp4FF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->f1;_LL4:
# 3120
({struct Cyc_Core_Opt*_tmpBE1=({struct Cyc_Core_Opt*_tmp4F4=_cycalloc(sizeof(*_tmp4F4));_tmp4F4->v=c1;_tmp4F4;});*_tmp4FF=_tmpBE1;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->tag == 1U){_LL5: _tmp500=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpBE2=({struct Cyc_Core_Opt*_tmp4F5=_cycalloc(sizeof(*_tmp4F5));_tmp4F5->v=c2;_tmp4F5;});*_tmp500=_tmpBE2;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->tag == 0U){_LL7: _tmp503=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->f1;_tmp502=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->f2;_tmp501=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->f1;_LL8:
# 3123
 if(Cyc_Tcutil_kind_leq(_tmp501,_tmp502)){
({struct Cyc_Core_Opt*_tmpBE3=({struct Cyc_Core_Opt*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->v=c2;_tmp4F6;});*_tmp503=_tmpBE3;});return 1;}else{
return 0;}}else{_LLB: _tmp507=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->f1;_tmp506=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f1)->f2;_tmp505=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->f1;_tmp504=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4F3.f2)->f2;_LLC:
# 3131
 if(Cyc_Tcutil_kind_leq(_tmp506,_tmp504)){
({struct Cyc_Core_Opt*_tmpBE4=({struct Cyc_Core_Opt*_tmp4F8=_cycalloc(sizeof(*_tmp4F8));_tmp4F8->v=c1;_tmp4F8;});*_tmp505=_tmpBE4;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp504,_tmp506)){
({struct Cyc_Core_Opt*_tmpBE5=({struct Cyc_Core_Opt*_tmp4F9=_cycalloc(sizeof(*_tmp4F9));_tmp4F9->v=c2;_tmp4F9;});*_tmp507=_tmpBE5;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3140
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3145
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp50D=({struct Cyc_Int_pa_PrintArg_struct _tmp9D8;_tmp9D8.tag=1U,_tmp9D8.f1=(unsigned long)i;_tmp9D8;});void*_tmp50B[1U];_tmp50B[0]=& _tmp50D;({struct _dyneither_ptr _tmpBE6=({const char*_tmp50C="#%d";_tag_dyneither(_tmp50C,sizeof(char),4U);});Cyc_aprintf(_tmpBE6,_tag_dyneither(_tmp50B,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp50A=_cycalloc(sizeof(*_tmp50A));({struct _dyneither_ptr*_tmpBE7=({unsigned int _tmp509=1;struct _dyneither_ptr*_tmp508=_cycalloc(_check_times(_tmp509,sizeof(struct _dyneither_ptr)));_tmp508[0]=s;_tmp508;});_tmp50A->name=_tmpBE7;}),_tmp50A->identity=- 1,_tmp50A->kind=k;_tmp50A;});}
# 3152
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp50E=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp50E,sizeof(char),0))== '#';}
# 3157
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp511=({struct Cyc_String_pa_PrintArg_struct _tmp9D9;_tmp9D9.tag=0U,_tmp9D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);_tmp9D9;});void*_tmp50F[1U];_tmp50F[0]=& _tmp511;({struct _dyneither_ptr _tmpBE8=({const char*_tmp510="%s";_tag_dyneither(_tmp510,sizeof(char),3U);});Cyc_printf(_tmpBE8,_tag_dyneither(_tmp50F,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp512=({struct _dyneither_ptr _tmpBE9=({const char*_tmp518="`";_tag_dyneither(_tmp518,sizeof(char),2U);});Cyc_strconcat(_tmpBE9,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp513=_dyneither_ptr_plus(_tmp512,sizeof(char),1);char _tmp514=*((char*)_check_dyneither_subscript(_tmp513,sizeof(char),0U));char _tmp515='t';if(_get_dyneither_size(_tmp513,sizeof(char))== 1U  && (_tmp514 == '\000'  && _tmp515 != '\000'))_throw_arraybounds();*((char*)_tmp513.curr)=_tmp515;});
({struct _dyneither_ptr*_tmpBEA=({unsigned int _tmp517=1;struct _dyneither_ptr*_tmp516=_cycalloc(_check_times(_tmp517,sizeof(struct _dyneither_ptr)));_tmp516[0]=(struct _dyneither_ptr)_tmp512;_tmp516;});t->name=_tmpBEA;});};}
# 3166
static struct _tuple10*Cyc_Tcutil_fndecl2type_f(struct _tuple10*x){
# 3168
return({struct _tuple10*_tmp519=_cycalloc(sizeof(*_tmp519));_tmp519->f1=(*x).f1,_tmp519->f2=(*x).f2,_tmp519->f3=(*x).f3;_tmp519;});}
# 3171
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 3178
struct Cyc_List_List*_tmp51A=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpBEB=({struct Cyc_List_List*_tmp51B=_cycalloc(sizeof(*_tmp51B));_tmp51B->hd=(void*)atts->hd,_tmp51B->tl=_tmp51A;_tmp51B;});_tmp51A=_tmpBEB;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp51C=_cycalloc(sizeof(*_tmp51C));_tmp51C->tag=5U,(_tmp51C->f1).tvars=fd->tvs,(_tmp51C->f1).effect=fd->effect,(_tmp51C->f1).ret_tqual=fd->ret_tqual,(_tmp51C->f1).ret_type=fd->ret_type,({
struct Cyc_List_List*_tmpBEC=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2type_f,fd->args);(_tmp51C->f1).args=_tmpBEC;}),(_tmp51C->f1).c_varargs=fd->c_varargs,(_tmp51C->f1).cyc_varargs=fd->cyc_varargs,(_tmp51C->f1).rgn_po=fd->rgn_po,(_tmp51C->f1).attributes=_tmp51A,(_tmp51C->f1).requires_clause=fd->requires_clause,(_tmp51C->f1).requires_relns=fd->requires_relns,(_tmp51C->f1).ensures_clause=fd->ensures_clause,(_tmp51C->f1).ensures_relns=fd->ensures_relns;_tmp51C;});}
# 3191
return(void*)_check_null(fd->cached_type);}
# 3197
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3199
union Cyc_Relations_RelnOp _tmp51D=*rop;union Cyc_Relations_RelnOp _tmp51E=_tmp51D;struct Cyc_Absyn_Vardecl*_tmp531;struct Cyc_Absyn_Vardecl*_tmp530;switch((_tmp51E.RNumelts).tag){case 2U: _LL1: _tmp530=(_tmp51E.RVar).val;_LL2: {
# 3201
struct _tuple2 _tmp51F=*_tmp530->name;struct _tuple2 _tmp520=_tmp51F;union Cyc_Absyn_Nmspace _tmp527;struct _dyneither_ptr*_tmp526;_LL8: _tmp527=_tmp520.f1;_tmp526=_tmp520.f2;_LL9:;
if(!((int)((_tmp527.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpBED=(struct _dyneither_ptr)*_tmp526;Cyc_strcmp(_tmpBED,({const char*_tmp521="return_value";_tag_dyneither(_tmp521,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpBEE=Cyc_Relations_RReturn();*rop=_tmpBEE;});
goto _LL0;}{
# 3207
unsigned int c=0U;
{struct Cyc_List_List*_tmp522=args;for(0;_tmp522 != 0;(_tmp522=_tmp522->tl,c ++)){
struct _tuple10*_tmp523=(struct _tuple10*)_tmp522->hd;struct _tuple10*_tmp524=_tmp523;struct _dyneither_ptr*_tmp525;_LLB: _tmp525=_tmp524->f1;_LLC:;
if(_tmp525 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp526,(struct _dyneither_ptr)*_tmp525)== 0){
({union Cyc_Relations_RelnOp _tmpBEF=Cyc_Relations_RParam(c);*rop=_tmpBEF;});
break;}}}}
# 3217
goto _LL0;};}case 3U: _LL3: _tmp531=(_tmp51E.RNumelts).val;_LL4: {
# 3219
struct _tuple2 _tmp528=*_tmp531->name;struct _tuple2 _tmp529=_tmp528;union Cyc_Absyn_Nmspace _tmp52F;struct _dyneither_ptr*_tmp52E;_LLE: _tmp52F=_tmp529.f1;_tmp52E=_tmp529.f2;_LLF:;
if(!((int)((_tmp52F.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp52A=args;for(0;_tmp52A != 0;(_tmp52A=_tmp52A->tl,c ++)){
struct _tuple10*_tmp52B=(struct _tuple10*)_tmp52A->hd;struct _tuple10*_tmp52C=_tmp52B;struct _dyneither_ptr*_tmp52D;_LL11: _tmp52D=_tmp52C->f1;_LL12:;
if(_tmp52D != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp52E,(struct _dyneither_ptr)*_tmp52D)== 0){
({union Cyc_Relations_RelnOp _tmpBF0=Cyc_Relations_RParamNumelts(c);*rop=_tmpBF0;});
break;}}}}
# 3231
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3236
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3238
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3242
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3244
if(e == 0)return 0;{
struct Cyc_List_List*_tmp532=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp532);
return _tmp532;};}struct _tuple27{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3251
static void*Cyc_Tcutil_fst_fdarg(struct _tuple27*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp533=pr;struct Cyc_Absyn_Tqual _tmp536;void*_tmp535;_LL1: _tmp536=_tmp533->f1;_tmp535=_tmp533->f2;_LL2:;
if(_tmp535 == t)return pr;else{
return({struct _tuple12*_tmp534=_cycalloc(sizeof(*_tmp534));_tmp534->f1=_tmp536,_tmp534->f2=t;_tmp534;});}}struct _tuple28{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple29{struct _tuple28*f1;void*f2;};
# 3258
static struct _tuple29*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3260
return({struct _tuple29*_tmp538=_region_malloc(rgn,sizeof(*_tmp538));({struct _tuple28*_tmpBF1=({struct _tuple28*_tmp537=_region_malloc(rgn,sizeof(*_tmp537));_tmp537->f1=(*y).f1,_tmp537->f2=(*y).f2;_tmp537;});_tmp538->f1=_tmpBF1;}),_tmp538->f2=(*y).f3;_tmp538;});}
# 3262
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3264
struct _tuple10 _tmp539=*orig_arg;struct _tuple10 _tmp53A=_tmp539;struct _dyneither_ptr*_tmp53E;struct Cyc_Absyn_Tqual _tmp53D;void*_tmp53C;_LL1: _tmp53E=_tmp53A.f1;_tmp53D=_tmp53A.f2;_tmp53C=_tmp53A.f3;_LL2:;
if(t == _tmp53C)return orig_arg;
return({struct _tuple10*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->f1=_tmp53E,_tmp53B->f2=_tmp53D,_tmp53B->f3=t;_tmp53B;});}
# 3268
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3271
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3276
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3278
return({struct Cyc_Absyn_Exp*_tmp53F=_cycalloc(sizeof(*_tmp53F));_tmp53F->topt=old->topt,_tmp53F->r=r,_tmp53F->loc=old->loc,_tmp53F->annot=old->annot;_tmp53F;});}
# 3283
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp540=e->r;void*_tmp541=_tmp540;void*_tmp581;void*_tmp580;struct Cyc_List_List*_tmp57F;struct Cyc_Absyn_Exp*_tmp57E;struct Cyc_Absyn_Exp*_tmp57D;void*_tmp57C;void*_tmp57B;struct Cyc_Absyn_Exp*_tmp57A;int _tmp579;enum Cyc_Absyn_Coercion _tmp578;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp575;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Exp*_tmp573;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Exp*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;enum Cyc_Absyn_Primop _tmp56E;struct Cyc_List_List*_tmp56D;switch(*((int*)_tmp541)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp56E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_tmp56D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_LLC:
# 3292
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56D)== 1){
struct Cyc_Absyn_Exp*_tmp542=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56D))->hd;
struct Cyc_Absyn_Exp*_tmp543=Cyc_Tcutil_rsubsexp(r,inst,_tmp542);
if(_tmp543 == _tmp542)return e;
return({struct Cyc_Absyn_Exp*_tmpBF3=e;Cyc_Tcutil_copye(_tmpBF3,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->tag=3U,_tmp545->f1=_tmp56E,({struct Cyc_List_List*_tmpBF2=({struct Cyc_Absyn_Exp*_tmp544[1U];_tmp544[0]=_tmp543;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp544,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp545->f2=_tmpBF2;});_tmp545;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56D)== 2){
struct Cyc_Absyn_Exp*_tmp546=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56D))->hd;
struct Cyc_Absyn_Exp*_tmp547=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp56D->tl))->hd;
struct Cyc_Absyn_Exp*_tmp548=Cyc_Tcutil_rsubsexp(r,inst,_tmp546);
struct Cyc_Absyn_Exp*_tmp549=Cyc_Tcutil_rsubsexp(r,inst,_tmp547);
if(_tmp548 == _tmp546  && _tmp549 == _tmp547)return e;
return({struct Cyc_Absyn_Exp*_tmpBF5=e;Cyc_Tcutil_copye(_tmpBF5,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->tag=3U,_tmp54B->f1=_tmp56E,({struct Cyc_List_List*_tmpBF4=({struct Cyc_Absyn_Exp*_tmp54A[2U];_tmp54A[0]=_tmp548,_tmp54A[1]=_tmp549;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp54A,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp54B->f2=_tmpBF4;});_tmp54B;}));});}else{
return({void*_tmp54C=0U;({struct _dyneither_ptr _tmpBF6=({const char*_tmp54D="primop does not have 1 or 2 args!";_tag_dyneither(_tmp54D,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBF6,_tag_dyneither(_tmp54C,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp571=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_tmp570=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_tmp56F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp541)->f3;_LLE: {
# 3306
struct Cyc_Absyn_Exp*_tmp54E=Cyc_Tcutil_rsubsexp(r,inst,_tmp571);
struct Cyc_Absyn_Exp*_tmp54F=Cyc_Tcutil_rsubsexp(r,inst,_tmp570);
struct Cyc_Absyn_Exp*_tmp550=Cyc_Tcutil_rsubsexp(r,inst,_tmp56F);
if((_tmp54E == _tmp571  && _tmp54F == _tmp570) && _tmp550 == _tmp56F)return e;
return({struct Cyc_Absyn_Exp*_tmpBF7=e;Cyc_Tcutil_copye(_tmpBF7,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->tag=6U,_tmp551->f1=_tmp54E,_tmp551->f2=_tmp54F,_tmp551->f3=_tmp550;_tmp551;}));});}case 7U: _LLF: _tmp573=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_tmp572=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_LL10: {
# 3312
struct Cyc_Absyn_Exp*_tmp552=Cyc_Tcutil_rsubsexp(r,inst,_tmp573);
struct Cyc_Absyn_Exp*_tmp553=Cyc_Tcutil_rsubsexp(r,inst,_tmp572);
if(_tmp552 == _tmp573  && _tmp553 == _tmp572)return e;
return({struct Cyc_Absyn_Exp*_tmpBF8=e;Cyc_Tcutil_copye(_tmpBF8,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->tag=7U,_tmp554->f1=_tmp552,_tmp554->f2=_tmp553;_tmp554;}));});}case 8U: _LL11: _tmp575=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_tmp574=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_LL12: {
# 3317
struct Cyc_Absyn_Exp*_tmp555=Cyc_Tcutil_rsubsexp(r,inst,_tmp575);
struct Cyc_Absyn_Exp*_tmp556=Cyc_Tcutil_rsubsexp(r,inst,_tmp574);
if(_tmp555 == _tmp575  && _tmp556 == _tmp574)return e;
return({struct Cyc_Absyn_Exp*_tmpBF9=e;Cyc_Tcutil_copye(_tmpBF9,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->tag=8U,_tmp557->f1=_tmp555,_tmp557->f2=_tmp556;_tmp557;}));});}case 9U: _LL13: _tmp577=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_tmp576=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_LL14: {
# 3322
struct Cyc_Absyn_Exp*_tmp558=Cyc_Tcutil_rsubsexp(r,inst,_tmp577);
struct Cyc_Absyn_Exp*_tmp559=Cyc_Tcutil_rsubsexp(r,inst,_tmp576);
if(_tmp558 == _tmp577  && _tmp559 == _tmp576)return e;
return({struct Cyc_Absyn_Exp*_tmpBFA=e;Cyc_Tcutil_copye(_tmpBFA,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp55A=_cycalloc(sizeof(*_tmp55A));_tmp55A->tag=9U,_tmp55A->f1=_tmp558,_tmp55A->f2=_tmp559;_tmp55A;}));});}case 14U: _LL15: _tmp57B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_tmp57A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_tmp579=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp541)->f3;_tmp578=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp541)->f4;_LL16: {
# 3327
struct Cyc_Absyn_Exp*_tmp55B=Cyc_Tcutil_rsubsexp(r,inst,_tmp57A);
void*_tmp55C=Cyc_Tcutil_rsubstitute(r,inst,_tmp57B);
if(_tmp55B == _tmp57A  && _tmp55C == _tmp57B)return e;
return({struct Cyc_Absyn_Exp*_tmpBFB=e;Cyc_Tcutil_copye(_tmpBFB,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->tag=14U,_tmp55D->f1=_tmp55C,_tmp55D->f2=_tmp55B,_tmp55D->f3=_tmp579,_tmp55D->f4=_tmp578;_tmp55D;}));});}case 17U: _LL17: _tmp57C=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_LL18: {
# 3332
void*_tmp55E=Cyc_Tcutil_rsubstitute(r,inst,_tmp57C);
if(_tmp55E == _tmp57C)return e;
return({struct Cyc_Absyn_Exp*_tmpBFC=e;Cyc_Tcutil_copye(_tmpBFC,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F->tag=17U,_tmp55F->f1=_tmp55E;_tmp55F;}));});}case 18U: _LL19: _tmp57D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_LL1A: {
# 3336
struct Cyc_Absyn_Exp*_tmp560=Cyc_Tcutil_rsubsexp(r,inst,_tmp57D);
if(_tmp560 == _tmp57D)return e;
return({struct Cyc_Absyn_Exp*_tmpBFD=e;Cyc_Tcutil_copye(_tmpBFD,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561->tag=18U,_tmp561->f1=_tmp560;_tmp561;}));});}case 41U: _LL1B: _tmp57E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_LL1C: {
# 3340
struct Cyc_Absyn_Exp*_tmp562=Cyc_Tcutil_rsubsexp(r,inst,_tmp57E);
if(_tmp562 == _tmp57E)return e;
return({struct Cyc_Absyn_Exp*_tmpBFE=e;Cyc_Tcutil_copye(_tmpBFE,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563->tag=41U,_tmp563->f1=_tmp562;_tmp563;}));});}case 19U: _LL1D: _tmp580=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_tmp57F=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp541)->f2;_LL1E: {
# 3344
void*_tmp564=Cyc_Tcutil_rsubstitute(r,inst,_tmp580);
if(_tmp564 == _tmp580)return e;
return({struct Cyc_Absyn_Exp*_tmpBFF=e;Cyc_Tcutil_copye(_tmpBFF,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp565=_cycalloc(sizeof(*_tmp565));_tmp565->tag=19U,_tmp565->f1=_tmp564,_tmp565->f2=_tmp57F;_tmp565;}));});}case 39U: _LL1F: _tmp581=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp541)->f1;_LL20: {
# 3348
void*_tmp566=Cyc_Tcutil_rsubstitute(r,inst,_tmp581);
if(_tmp566 == _tmp581)return e;{
# 3351
void*_tmp567=Cyc_Tcutil_compress(_tmp566);void*_tmp568=_tmp567;struct Cyc_Absyn_Exp*_tmp56A;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp568)->tag == 9U){_LL24: _tmp56A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp568)->f1;_LL25:
 return _tmp56A;}else{_LL26: _LL27:
# 3354
 return({struct Cyc_Absyn_Exp*_tmpC00=e;Cyc_Tcutil_copye(_tmpC00,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp569=_cycalloc(sizeof(*_tmp569));_tmp569->tag=39U,_tmp569->f1=_tmp566;_tmp569;}));});}_LL23:;};}default: _LL21: _LL22:
# 3357
 return({void*_tmp56B=0U;({struct _dyneither_ptr _tmpC01=({const char*_tmp56C="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp56C,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC01,_tag_dyneither(_tmp56B,sizeof(void*),0U));});});}_LL0:;}
# 3361
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3364
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3368
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3371
void*_tmp582=f->type;
struct Cyc_Absyn_Exp*_tmp583=f->requires_clause;
void*_tmp584=Cyc_Tcutil_rsubstitute(r,inst,_tmp582);
struct Cyc_Absyn_Exp*_tmp585=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp583);
if(_tmp582 == _tmp584  && _tmp583 == _tmp585)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp586=_cycalloc(sizeof(*_tmp586));_tmp586->name=f->name,_tmp586->tq=f->tq,_tmp586->type=_tmp584,_tmp586->width=f->width,_tmp586->attributes=f->attributes,_tmp586->requires_clause=_tmp585;_tmp586;});}}
# 3381
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3384
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp587=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp588=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp589=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp587);
struct Cyc_List_List*_tmp58A=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp588);
if(_tmp589 == _tmp587  && _tmp58A == _tmp588)return fs;
return({struct Cyc_List_List*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->hd=_tmp589,_tmp58B->tl=_tmp58A;_tmp58B;});};}
# 3393
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3396
struct _tuple1 _tmp58C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp58D=_tmp58C;struct Cyc_List_List*_tmp591;struct Cyc_List_List*_tmp590;_LL1: _tmp591=_tmp58D.f1;_tmp590=_tmp58D.f2;_LL2:;{
struct Cyc_List_List*_tmp58E=Cyc_Tcutil_substs(rgn,inst,_tmp591);
struct Cyc_List_List*_tmp58F=Cyc_Tcutil_substs(rgn,inst,_tmp590);
if(_tmp58E == _tmp591  && _tmp58F == _tmp590)
return rgn_po;else{
# 3402
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp58E,_tmp58F);}};}
# 3405
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3408
void*_tmp592=Cyc_Tcutil_compress(t);void*_tmp593=_tmp592;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_Exp*_tmp5EA;void*_tmp5E9;struct Cyc_List_List*_tmp5E8;void*_tmp5E7;void*_tmp5E6;enum Cyc_Absyn_AggrKind _tmp5E5;struct Cyc_List_List*_tmp5E4;struct Cyc_List_List*_tmp5E3;struct Cyc_List_List*_tmp5E2;void*_tmp5E1;struct Cyc_Absyn_Tqual _tmp5E0;void*_tmp5DF;struct Cyc_List_List*_tmp5DE;int _tmp5DD;struct Cyc_Absyn_VarargInfo*_tmp5DC;struct Cyc_List_List*_tmp5DB;struct Cyc_List_List*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5D8;void*_tmp5D7;struct Cyc_Absyn_Tqual _tmp5D6;void*_tmp5D5;void*_tmp5D4;void*_tmp5D3;void*_tmp5D2;void*_tmp5D1;struct Cyc_Absyn_Tqual _tmp5D0;struct Cyc_Absyn_Exp*_tmp5CF;void*_tmp5CE;unsigned int _tmp5CD;struct _tuple2*_tmp5CC;struct Cyc_List_List*_tmp5CB;struct Cyc_Absyn_Typedefdecl*_tmp5CA;void*_tmp5C9;struct Cyc_Absyn_Tvar*_tmp5C8;switch(*((int*)_tmp593)){case 2U: _LL1: _tmp5C8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp593)->f1;_LL2: {
# 3411
struct _handler_cons _tmp594;_push_handler(& _tmp594);{int _tmp596=0;if(setjmp(_tmp594.handler))_tmp596=1;if(!_tmp596){{void*_tmp597=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp5C8);_npop_handler(0U);return _tmp597;};_pop_handler();}else{void*_tmp595=(void*)_exn_thrown;void*_tmp598=_tmp595;void*_tmp599;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp598)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp599=_tmp598;_LL1F:(int)_rethrow(_tmp599);}_LL1B:;}};}case 8U: _LL3: _tmp5CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp593)->f1;_tmp5CB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp593)->f2;_tmp5CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp593)->f3;_tmp5C9=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp593)->f4;_LL4: {
# 3414
struct Cyc_List_List*_tmp59A=Cyc_Tcutil_substs(rgn,inst,_tmp5CB);
return _tmp59A == _tmp5CB?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp59B=_cycalloc(sizeof(*_tmp59B));_tmp59B->tag=8U,_tmp59B->f1=_tmp5CC,_tmp59B->f2=_tmp59A,_tmp59B->f3=_tmp5CA,_tmp59B->f4=_tmp5C9;_tmp59B;});}case 4U: _LL5: _tmp5D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp593)->f1).elt_type;_tmp5D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp593)->f1).tq;_tmp5CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp593)->f1).num_elts;_tmp5CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp593)->f1).zero_term;_tmp5CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp593)->f1).zt_loc;_LL6: {
# 3417
void*_tmp59C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D1);
struct Cyc_Absyn_Exp*_tmp59D=_tmp5CF == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5CF);
void*_tmp59E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5CE);
return(_tmp59C == _tmp5D1  && _tmp59D == _tmp5CF) && _tmp59E == _tmp5CE?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp59F=_cycalloc(sizeof(*_tmp59F));
_tmp59F->tag=4U,(_tmp59F->f1).elt_type=_tmp59C,(_tmp59F->f1).tq=_tmp5D0,(_tmp59F->f1).num_elts=_tmp59D,(_tmp59F->f1).zero_term=_tmp59E,(_tmp59F->f1).zt_loc=_tmp5CD;_tmp59F;});}case 3U: _LL7: _tmp5D7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp593)->f1).elt_type;_tmp5D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp593)->f1).elt_tq;_tmp5D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp593)->f1).ptr_atts).rgn;_tmp5D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp593)->f1).ptr_atts).nullable;_tmp5D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp593)->f1).ptr_atts).bounds;_tmp5D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp593)->f1).ptr_atts).zero_term;_LL8: {
# 3423
void*_tmp5A0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D7);
void*_tmp5A1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D5);
void*_tmp5A2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D3);
void*_tmp5A3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5D2);
if(((_tmp5A0 == _tmp5D7  && _tmp5A1 == _tmp5D5) && _tmp5A2 == _tmp5D3) && _tmp5A3 == _tmp5D2)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp9DA;_tmp9DA.elt_type=_tmp5A0,_tmp9DA.elt_tq=_tmp5D6,(_tmp9DA.ptr_atts).rgn=_tmp5A1,(_tmp9DA.ptr_atts).nullable=_tmp5D4,(_tmp9DA.ptr_atts).bounds=_tmp5A2,(_tmp9DA.ptr_atts).zero_term=_tmp5A3,(_tmp9DA.ptr_atts).ptrloc=0;_tmp9DA;}));}case 5U: _LL9: _tmp5E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).tvars;_tmp5E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).effect;_tmp5E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).ret_tqual;_tmp5DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).ret_type;_tmp5DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).args;_tmp5DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).c_varargs;_tmp5DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).cyc_varargs;_tmp5DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).rgn_po;_tmp5DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).attributes;_tmp5D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).requires_clause;_tmp5D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp593)->f1).ensures_clause;_LLA:
# 3433
{struct Cyc_List_List*_tmp5A4=_tmp5E2;for(0;_tmp5A4 != 0;_tmp5A4=_tmp5A4->tl){
({struct Cyc_List_List*_tmpC04=({struct Cyc_List_List*_tmp5A6=_region_malloc(rgn,sizeof(*_tmp5A6));({struct _tuple15*_tmpC03=({struct _tuple15*_tmp5A5=_region_malloc(rgn,sizeof(*_tmp5A5));_tmp5A5->f1=(struct Cyc_Absyn_Tvar*)_tmp5A4->hd,({void*_tmpC02=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp5A4->hd);_tmp5A5->f2=_tmpC02;});_tmp5A5;});_tmp5A6->hd=_tmpC03;}),_tmp5A6->tl=inst;_tmp5A6;});inst=_tmpC04;});}}{
struct _tuple1 _tmp5A7=({struct _RegionHandle*_tmpC06=rgn;struct _RegionHandle*_tmpC05=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpC06,_tmpC05,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple29*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp5DE));});
# 3435
struct _tuple1 _tmp5A8=_tmp5A7;struct Cyc_List_List*_tmp5B7;struct Cyc_List_List*_tmp5B6;_LL21: _tmp5B7=_tmp5A8.f1;_tmp5B6=_tmp5A8.f2;_LL22:;{
# 3437
struct Cyc_List_List*_tmp5A9=_tmp5DE;
struct Cyc_List_List*_tmp5AA=Cyc_Tcutil_substs(rgn,inst,_tmp5B6);
if(_tmp5AA != _tmp5B6)
({struct Cyc_List_List*_tmpC07=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp5DE,_tmp5AA);_tmp5A9=_tmpC07;});{
void*eff2;
if(_tmp5E1 == 0)
eff2=0;else{
# 3445
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E1);
if(new_eff == _tmp5E1)
eff2=_tmp5E1;else{
# 3449
eff2=new_eff;}}{
# 3451
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp5DC == 0)
cyc_varargs2=0;else{
# 3455
struct Cyc_Absyn_VarargInfo _tmp5AB=*_tmp5DC;struct Cyc_Absyn_VarargInfo _tmp5AC=_tmp5AB;struct _dyneither_ptr*_tmp5B2;struct Cyc_Absyn_Tqual _tmp5B1;void*_tmp5B0;int _tmp5AF;_LL24: _tmp5B2=_tmp5AC.name;_tmp5B1=_tmp5AC.tq;_tmp5B0=_tmp5AC.type;_tmp5AF=_tmp5AC.inject;_LL25:;{
void*_tmp5AD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B0);
if(_tmp5AD == _tmp5B0)cyc_varargs2=_tmp5DC;else{
# 3459
({struct Cyc_Absyn_VarargInfo*_tmpC08=({struct Cyc_Absyn_VarargInfo*_tmp5AE=_cycalloc(sizeof(*_tmp5AE));_tmp5AE->name=_tmp5B2,_tmp5AE->tq=_tmp5B1,_tmp5AE->type=_tmp5AD,_tmp5AE->inject=_tmp5AF;_tmp5AE;});cyc_varargs2=_tmpC08;});}};}{
# 3461
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5DB);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5D9);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5D8);
struct Cyc_List_List*_tmp5B3=Cyc_Tcutil_extract_relns(_tmp5A9,req2);
struct Cyc_List_List*_tmp5B4=Cyc_Tcutil_extract_relns(_tmp5A9,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));
_tmp5B5->tag=5U,(_tmp5B5->f1).tvars=_tmp5E2,(_tmp5B5->f1).effect=eff2,(_tmp5B5->f1).ret_tqual=_tmp5E0,({void*_tmpC09=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5DF);(_tmp5B5->f1).ret_type=_tmpC09;}),(_tmp5B5->f1).args=_tmp5A9,(_tmp5B5->f1).c_varargs=_tmp5DD,(_tmp5B5->f1).cyc_varargs=cyc_varargs2,(_tmp5B5->f1).rgn_po=rgn_po2,(_tmp5B5->f1).attributes=_tmp5DA,(_tmp5B5->f1).requires_clause=req2,(_tmp5B5->f1).requires_relns=_tmp5B3,(_tmp5B5->f1).ensures_clause=ens2,(_tmp5B5->f1).ensures_relns=_tmp5B4;_tmp5B5;});};};};};};case 6U: _LLB: _tmp5E3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp593)->f1;_LLC: {
# 3471
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp5B8=_tmp5E3;for(0;_tmp5B8 != 0;_tmp5B8=_tmp5B8->tl){
void*_tmp5B9=(*((struct _tuple12*)_tmp5B8->hd)).f2;
void*_tmp5BA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B9);
if(_tmp5B9 != _tmp5BA)
change=1;
# 3479
({struct Cyc_List_List*_tmpC0A=({struct Cyc_List_List*_tmp5BB=_region_malloc(rgn,sizeof(*_tmp5BB));_tmp5BB->hd=_tmp5BA,_tmp5BB->tl=ts2;_tmp5BB;});ts2=_tmpC0A;});}}
# 3481
if(!change)
return t;{
struct Cyc_List_List*_tmp5BC=({struct Cyc_List_List*_tmpC0B=_tmp5E3;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpC0B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->tag=6U,_tmp5BD->f1=_tmp5BC;_tmp5BD;});};}case 7U: _LLD: _tmp5E5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp593)->f1;_tmp5E4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp593)->f2;_LLE: {
# 3486
struct Cyc_List_List*_tmp5BE=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp5E4);
if(_tmp5E4 == _tmp5BE)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->tag=7U,_tmp5BF->f1=_tmp5E5,_tmp5BF->f2=_tmp5BE;_tmp5BF;});}case 1U: _LLF: _tmp5E6=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp593)->f2;_LL10:
# 3490
 if(_tmp5E6 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5E6);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp593)->f2 == 0){_LL11: _tmp5E7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp593)->f1;_LL12:
 return t;}else{_LL13: _tmp5E9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp593)->f1;_tmp5E8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp593)->f2;_LL14: {
# 3494
struct Cyc_List_List*_tmp5C0=Cyc_Tcutil_substs(rgn,inst,_tmp5E8);
if(_tmp5E8 == _tmp5C0)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->tag=0U,_tmp5C1->f1=_tmp5E9,_tmp5C1->f2=_tmp5C0;_tmp5C1;});}}}case 9U: _LL15: _tmp5EA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp593)->f1;_LL16: {
# 3498
struct Cyc_Absyn_Exp*_tmp5C2=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5EA);
return _tmp5C2 == _tmp5EA?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->tag=9U,_tmp5C3->f1=_tmp5C2;_tmp5C3;});}case 11U: _LL17: _tmp5EB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp593)->f1;_LL18: {
# 3501
struct Cyc_Absyn_Exp*_tmp5C4=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5EB);
return _tmp5C4 == _tmp5EB?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->tag=11U,_tmp5C5->f1=_tmp5C4;_tmp5C5;});}default: _LL19: _LL1A:
({void*_tmp5C6=0U;({struct _dyneither_ptr _tmpC0C=({const char*_tmp5C7="found typedecltype in rsubs";_tag_dyneither(_tmp5C7,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0C,_tag_dyneither(_tmp5C6,sizeof(void*),0U));});});}_LL0:;}
# 3507
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3510
if(ts == 0)
return 0;{
void*_tmp5EC=(void*)ts->hd;
struct Cyc_List_List*_tmp5ED=ts->tl;
void*_tmp5EE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5EC);
struct Cyc_List_List*_tmp5EF=Cyc_Tcutil_substs(rgn,inst,_tmp5ED);
if(_tmp5EC == _tmp5EE  && _tmp5ED == _tmp5EF)
return ts;
return({struct Cyc_List_List*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0->hd=_tmp5EE,_tmp5F0->tl=_tmp5EF;_tmp5F0;});};}
# 3521
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3528
struct _tuple15*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp5F1=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->f1=tv,({void*_tmpC0E=({struct Cyc_Core_Opt*_tmpC0D=_tmp5F1;Cyc_Absyn_new_evar(_tmpC0D,({struct Cyc_Core_Opt*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->v=s;_tmp5F2;}));});_tmp5F3->f2=_tmpC0E;});_tmp5F3;});}
# 3533
struct _tuple15*Cyc_Tcutil_r_make_inst_var(struct _tuple16*env,struct Cyc_Absyn_Tvar*tv){
# 3535
struct _tuple16*_tmp5F4=env;struct Cyc_List_List*_tmp5F9;struct _RegionHandle*_tmp5F8;_LL1: _tmp5F9=_tmp5F4->f1;_tmp5F8=_tmp5F4->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp5F5=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple15*_tmp5F7=_region_malloc(_tmp5F8,sizeof(*_tmp5F7));_tmp5F7->f1=tv,({void*_tmpC10=({struct Cyc_Core_Opt*_tmpC0F=_tmp5F5;Cyc_Absyn_new_evar(_tmpC0F,({struct Cyc_Core_Opt*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->v=_tmp5F9;_tmp5F6;}));});_tmp5F7->f2=_tmpC10;});_tmp5F7;});};}
# 3545
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3549
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp5FC=({struct Cyc_String_pa_PrintArg_struct _tmp9DD;_tmp9DD.tag=0U,_tmp9DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);_tmp9DD;});struct Cyc_String_pa_PrintArg_struct _tmp5FD=({struct Cyc_String_pa_PrintArg_struct _tmp9DC;_tmp9DC.tag=0U,({
struct _dyneither_ptr _tmpC11=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp9DC.f1=_tmpC11;});_tmp9DC;});struct Cyc_String_pa_PrintArg_struct _tmp5FE=({struct Cyc_String_pa_PrintArg_struct _tmp9DB;_tmp9DB.tag=0U,({struct _dyneither_ptr _tmpC12=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k2));_tmp9DB.f1=_tmpC12;});_tmp9DB;});void*_tmp5FA[3U];_tmp5FA[0]=& _tmp5FC,_tmp5FA[1]=& _tmp5FD,_tmp5FA[2]=& _tmp5FE;({unsigned int _tmpC14=loc;struct _dyneither_ptr _tmpC13=({const char*_tmp5FB="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp5FB,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpC14,_tmpC13,_tag_dyneither(_tmp5FA,sizeof(void*),3U));});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp5FF=0U;({struct _dyneither_ptr _tmpC15=({const char*_tmp600="same type variable has different identity!";_tag_dyneither(_tmp600,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC15,_tag_dyneither(_tmp5FF,sizeof(void*),0U));});});}
return tvs;}}}
# 3563
({int _tmpC16=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpC16;});
return({struct Cyc_List_List*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->hd=tv,_tmp601->tl=tvs;_tmp601;});}
# 3569
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3571
if(tv->identity == - 1)
({void*_tmp602=0U;({struct _dyneither_ptr _tmpC17=({const char*_tmp603="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp603,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC17,_tag_dyneither(_tmp602,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3575
struct Cyc_Absyn_Tvar*_tmp604=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp604->identity == - 1)
({void*_tmp605=0U;({struct _dyneither_ptr _tmpC18=({const char*_tmp606="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp606,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC18,_tag_dyneither(_tmp605,sizeof(void*),0U));});});
if(_tmp604->identity == tv->identity)
return tvs;}}
# 3582
return({struct Cyc_List_List*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->hd=tv,_tmp607->tl=tvs;_tmp607;});}struct _tuple30{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3588
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3593
if(tv->identity == - 1)
({void*_tmp608=0U;({struct _dyneither_ptr _tmpC19=({const char*_tmp609="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp609,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC19,_tag_dyneither(_tmp608,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3597
struct _tuple30*_tmp60A=(struct _tuple30*)tvs2->hd;struct _tuple30*_tmp60B=_tmp60A;struct Cyc_Absyn_Tvar*_tmp60F;int*_tmp60E;_LL1: _tmp60F=_tmp60B->f1;_tmp60E=(int*)& _tmp60B->f2;_LL2:;
if(_tmp60F->identity == - 1)
({void*_tmp60C=0U;({struct _dyneither_ptr _tmpC1A=({const char*_tmp60D="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp60D,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC1A,_tag_dyneither(_tmp60C,sizeof(void*),0U));});});
if(_tmp60F->identity == tv->identity){
*_tmp60E=*_tmp60E  || b;
return tvs;}}}
# 3605
return({struct Cyc_List_List*_tmp611=_region_malloc(r,sizeof(*_tmp611));({struct _tuple30*_tmpC1B=({struct _tuple30*_tmp610=_region_malloc(r,sizeof(*_tmp610));_tmp610->f1=tv,_tmp610->f2=b;_tmp610;});_tmp611->hd=_tmpC1B;}),_tmp611->tl=tvs;_tmp611;});}
# 3609
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3611
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp614=({struct Cyc_String_pa_PrintArg_struct _tmp9DE;_tmp9DE.tag=0U,({struct _dyneither_ptr _tmpC1C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp9DE.f1=_tmpC1C;});_tmp9DE;});void*_tmp612[1U];_tmp612[0]=& _tmp614;({struct _dyneither_ptr _tmpC1D=({const char*_tmp613="bound tvar id for %s is NULL";_tag_dyneither(_tmp613,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC1D,_tag_dyneither(_tmp612,sizeof(void*),1U));});});
return({struct Cyc_List_List*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->hd=tv,_tmp615->tl=tvs;_tmp615;});}struct _tuple31{void*f1;int f2;};
# 3620
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3623
void*_tmp616=Cyc_Tcutil_compress(e);void*_tmp617=_tmp616;int _tmp621;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp617)->tag == 1U){_LL1: _tmp621=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp617)->f3;_LL2:
# 3625
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple31*_tmp618=(struct _tuple31*)es2->hd;struct _tuple31*_tmp619=_tmp618;void*_tmp61E;int*_tmp61D;_LL6: _tmp61E=_tmp619->f1;_tmp61D=(int*)& _tmp619->f2;_LL7:;{
void*_tmp61A=Cyc_Tcutil_compress(_tmp61E);void*_tmp61B=_tmp61A;int _tmp61C;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61B)->tag == 1U){_LL9: _tmp61C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp61B)->f3;_LLA:
# 3629
 if(_tmp621 == _tmp61C){
if(b != *_tmp61D)*_tmp61D=1;
return es;}
# 3633
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3637
return({struct Cyc_List_List*_tmp620=_region_malloc(r,sizeof(*_tmp620));({struct _tuple31*_tmpC1E=({struct _tuple31*_tmp61F=_region_malloc(r,sizeof(*_tmp61F));_tmp61F->f1=e,_tmp61F->f2=b;_tmp61F;});_tmp620->hd=_tmpC1E;}),_tmp620->tl=es;_tmp620;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3642
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3645
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3654
if(!present)({struct Cyc_List_List*_tmpC1F=({struct Cyc_List_List*_tmp622=_region_malloc(rgn,sizeof(*_tmp622));_tmp622->hd=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp622->tl=r;_tmp622;});r=_tmpC1F;});}
# 3656
({struct Cyc_List_List*_tmpC20=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpC20;});
return r;}
# 3661
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3665
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple30 _tmp623=*((struct _tuple30*)tvs->hd);struct _tuple30 _tmp624=_tmp623;struct Cyc_Absyn_Tvar*_tmp627;int _tmp626;_LL1: _tmp627=_tmp624.f1;_tmp626=_tmp624.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp627->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3675
if(!present)({struct Cyc_List_List*_tmpC21=({struct Cyc_List_List*_tmp625=_region_malloc(r,sizeof(*_tmp625));_tmp625->hd=(struct _tuple30*)tvs->hd,_tmp625->tl=res;_tmp625;});res=_tmpC21;});};}
# 3677
({struct Cyc_List_List*_tmpC22=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpC22;});
return res;}
# 3681
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_type,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3683
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp62A=({struct Cyc_String_pa_PrintArg_struct _tmp9DF;_tmp9DF.tag=0U,_tmp9DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9DF;});void*_tmp628[1U];_tmp628[0]=& _tmp62A;({unsigned int _tmpC24=loc;struct _dyneither_ptr _tmpC23=({const char*_tmp629="bitfield %s does not have constant width";_tag_dyneither(_tmp629,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpC24,_tmpC23,_tag_dyneither(_tmp628,sizeof(void*),1U));});});else{
# 3688
struct _tuple13 _tmp62B=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _tmp62C=_tmp62B;unsigned int _tmp632;int _tmp631;_LL1: _tmp632=_tmp62C.f1;_tmp631=_tmp62C.f2;_LL2:;
if(!_tmp631)
({void*_tmp62D=0U;({unsigned int _tmpC26=loc;struct _dyneither_ptr _tmpC25=({const char*_tmp62E="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp62E,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpC26,_tmpC25,_tag_dyneither(_tmp62D,sizeof(void*),0U));});});
if((int)_tmp632 < 0)
({void*_tmp62F=0U;({unsigned int _tmpC28=loc;struct _dyneither_ptr _tmpC27=({const char*_tmp630="bitfield has negative width";_tag_dyneither(_tmp630,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpC28,_tmpC27,_tag_dyneither(_tmp62F,sizeof(void*),0U));});});
w=_tmp632;}{
# 3695
void*_tmp633=Cyc_Tcutil_compress(field_type);void*_tmp634=_tmp633;enum Cyc_Absyn_Size_of _tmp642;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp634)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp634)->f1)->tag == 1U){_LL4: _tmp642=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp634)->f1)->f2;_LL5:
# 3698
{enum Cyc_Absyn_Size_of _tmp635=_tmp642;switch(_tmp635){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > 8)({void*_tmp636=0U;({unsigned int _tmpC2A=loc;struct _dyneither_ptr _tmpC29=({const char*_tmp637="bitfield larger than type";_tag_dyneither(_tmp637,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC2A,_tmpC29,_tag_dyneither(_tmp636,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > 16)({void*_tmp638=0U;({unsigned int _tmpC2C=loc;struct _dyneither_ptr _tmpC2B=({const char*_tmp639="bitfield larger than type";_tag_dyneither(_tmp639,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC2C,_tmpC2B,_tag_dyneither(_tmp638,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3703
 if(w > 32)({void*_tmp63A=0U;({unsigned int _tmpC2E=loc;struct _dyneither_ptr _tmpC2D=({const char*_tmp63B="bitfield larger than type";_tag_dyneither(_tmp63B,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC2E,_tmpC2D,_tag_dyneither(_tmp63A,sizeof(void*),0U));});});goto _LL8;default: _LL11: _LL12:
# 3705
 if(w > 64)({void*_tmp63C=0U;({unsigned int _tmpC30=loc;struct _dyneither_ptr _tmpC2F=({const char*_tmp63D="bitfield larger than type";_tag_dyneither(_tmp63D,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpC30,_tmpC2F,_tag_dyneither(_tmp63C,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3707
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 3709
({struct Cyc_String_pa_PrintArg_struct _tmp640=({struct Cyc_String_pa_PrintArg_struct _tmp9E1;_tmp9E1.tag=0U,_tmp9E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9E1;});struct Cyc_String_pa_PrintArg_struct _tmp641=({struct Cyc_String_pa_PrintArg_struct _tmp9E0;_tmp9E0.tag=0U,({
struct _dyneither_ptr _tmpC31=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(field_type));_tmp9E0.f1=_tmpC31;});_tmp9E0;});void*_tmp63E[2U];_tmp63E[0]=& _tmp640,_tmp63E[1]=& _tmp641;({unsigned int _tmpC33=loc;struct _dyneither_ptr _tmpC32=({const char*_tmp63F="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp63F,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpC33,_tmpC32,_tag_dyneither(_tmp63E,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3716
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp643=(void*)atts->hd;void*_tmp644=_tmp643;switch(*((int*)_tmp644)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp647=({struct Cyc_String_pa_PrintArg_struct _tmp9E3;_tmp9E3.tag=0U,({
struct _dyneither_ptr _tmpC34=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp9E3.f1=_tmpC34;});_tmp9E3;});struct Cyc_String_pa_PrintArg_struct _tmp648=({struct Cyc_String_pa_PrintArg_struct _tmp9E2;_tmp9E2.tag=0U,_tmp9E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);_tmp9E2;});void*_tmp645[2U];_tmp645[0]=& _tmp647,_tmp645[1]=& _tmp648;({unsigned int _tmpC36=loc;struct _dyneither_ptr _tmpC35=({const char*_tmp646="bad attribute %s on member %s";_tag_dyneither(_tmp646,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpC36,_tmpC35,_tag_dyneither(_tmp645,sizeof(void*),2U));});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3744
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp649=t;struct Cyc_Absyn_Typedefdecl*_tmp64D;void*_tmp64C;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp649)->tag == 8U){_LL1: _tmp64D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp649)->f3;_tmp64C=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp649)->f4;_LL2:
# 3747
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp64D))->tq).real_const  || (_tmp64D->tq).print_const){
if(declared_const)({void*_tmp64A=0U;({unsigned int _tmpC38=loc;struct _dyneither_ptr _tmpC37=({const char*_tmp64B="extra const";_tag_dyneither(_tmp64B,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpC38,_tmpC37,_tag_dyneither(_tmp64A,sizeof(void*),0U));});});
return 1;}
# 3752
if((unsigned int)_tmp64C)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp64C);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3759
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp64E=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp64F=_tmp64E;void*_tmp653;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64F)->tag == 3U){_LL1: _tmp653=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp64F)->f1).ptr_atts).rgn;_LL2:
# 3764
{void*_tmp650=Cyc_Tcutil_compress(_tmp653);void*_tmp651=_tmp650;struct Cyc_Absyn_Tvar*_tmp652;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp651)->tag == 2U){_LL6: _tmp652=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp651)->f1;_LL7:
# 3766
 return Cyc_Absyn_tvar_cmp(tvar,_tmp652)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3769
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3775
return 1;}
return 0;}
# 3779
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3782
void*_tmp654=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp655=_tmp654;switch(*((int*)_tmp655)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp655)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp655)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp655)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp655)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3791
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3794
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3800
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3806
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple32{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3810
static struct _tuple32 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3813
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp658=({struct Cyc_String_pa_PrintArg_struct _tmp9E5;_tmp9E5.tag=0U,_tmp9E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9E5;});struct Cyc_String_pa_PrintArg_struct _tmp659=({struct Cyc_String_pa_PrintArg_struct _tmp9E4;_tmp9E4.tag=0U,({
struct _dyneither_ptr _tmpC39=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp9E4.f1=_tmpC39;});_tmp9E4;});void*_tmp656[2U];_tmp656[0]=& _tmp658,_tmp656[1]=& _tmp659;({unsigned int _tmpC3B=loc;struct _dyneither_ptr _tmpC3A=({const char*_tmp657="%s clause has type %s instead of integral type";_tag_dyneither(_tmp657,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpC3B,_tmpC3A,_tag_dyneither(_tmp656,sizeof(void*),2U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC3C=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpC3C;});
({struct Cyc_List_List*_tmpC3D=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpC3D;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp65C=({struct Cyc_String_pa_PrintArg_struct _tmp9E7;_tmp9E7.tag=0U,_tmp9E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);_tmp9E7;});struct Cyc_String_pa_PrintArg_struct _tmp65D=({struct Cyc_String_pa_PrintArg_struct _tmp9E6;_tmp9E6.tag=0U,({
struct _dyneither_ptr _tmpC3E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(clause));_tmp9E6.f1=_tmpC3E;});_tmp9E6;});void*_tmp65A[2U];_tmp65A[0]=& _tmp65C,_tmp65A[1]=& _tmp65D;({unsigned int _tmpC40=clause->loc;struct _dyneither_ptr _tmpC3F=({const char*_tmp65B="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp65B,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpC40,_tmpC3F,_tag_dyneither(_tmp65A,sizeof(void*),2U));});});}
# 3825
return({struct _tuple32 _tmp9E8;_tmp9E8.f1=cvtenv,_tmp9E8.f2=relns;_tmp9E8;});}
# 3855 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr);
# 3862
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_aggr(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_AggrInfo*info,struct Cyc_List_List**targs,int allow_abs_aggr){
# 3868
{union Cyc_Absyn_AggrInfo _tmp65E=*info;union Cyc_Absyn_AggrInfo _tmp65F=_tmp65E;struct Cyc_Absyn_Aggrdecl*_tmp682;enum Cyc_Absyn_AggrKind _tmp681;struct _tuple2*_tmp680;struct Cyc_Core_Opt*_tmp67F;if((_tmp65F.UnknownAggr).tag == 1){_LL1: _tmp681=((_tmp65F.UnknownAggr).val).f1;_tmp680=((_tmp65F.UnknownAggr).val).f2;_tmp67F=((_tmp65F.UnknownAggr).val).f3;_LL2: {
# 3870
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp660;_push_handler(& _tmp660);{int _tmp662=0;if(setjmp(_tmp660.handler))_tmp662=1;if(!_tmp662){
({struct Cyc_Absyn_Aggrdecl**_tmpC41=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp680);adp=_tmpC41;});{
struct Cyc_Absyn_Aggrdecl*_tmp663=*adp;
if(_tmp663->kind != _tmp681){
if(_tmp663->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp666=({struct Cyc_String_pa_PrintArg_struct _tmp9EA;_tmp9EA.tag=0U,({
struct _dyneither_ptr _tmpC42=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp680));_tmp9EA.f1=_tmpC42;});_tmp9EA;});struct Cyc_String_pa_PrintArg_struct _tmp667=({struct Cyc_String_pa_PrintArg_struct _tmp9E9;_tmp9E9.tag=0U,({struct _dyneither_ptr _tmpC43=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp680));_tmp9E9.f1=_tmpC43;});_tmp9E9;});void*_tmp664[2U];_tmp664[0]=& _tmp666,_tmp664[1]=& _tmp667;({unsigned int _tmpC45=loc;struct _dyneither_ptr _tmpC44=({const char*_tmp665="expecting struct %s instead of union %s";_tag_dyneither(_tmp665,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC45,_tmpC44,_tag_dyneither(_tmp664,sizeof(void*),2U));});});else{
# 3879
({struct Cyc_String_pa_PrintArg_struct _tmp66A=({struct Cyc_String_pa_PrintArg_struct _tmp9EC;_tmp9EC.tag=0U,({
struct _dyneither_ptr _tmpC46=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp680));_tmp9EC.f1=_tmpC46;});_tmp9EC;});struct Cyc_String_pa_PrintArg_struct _tmp66B=({struct Cyc_String_pa_PrintArg_struct _tmp9EB;_tmp9EB.tag=0U,({struct _dyneither_ptr _tmpC47=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp680));_tmp9EB.f1=_tmpC47;});_tmp9EB;});void*_tmp668[2U];_tmp668[0]=& _tmp66A,_tmp668[1]=& _tmp66B;({unsigned int _tmpC49=loc;struct _dyneither_ptr _tmpC48=({const char*_tmp669="expecting union %s instead of struct %s";_tag_dyneither(_tmp669,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC49,_tmpC48,_tag_dyneither(_tmp668,sizeof(void*),2U));});});}}
# 3882
if((unsigned int)_tmp67F  && (int)_tmp67F->v){
if(!((unsigned int)_tmp663->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp663->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp66E=({struct Cyc_String_pa_PrintArg_struct _tmp9ED;_tmp9ED.tag=0U,({
struct _dyneither_ptr _tmpC4A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp680));_tmp9ED.f1=_tmpC4A;});_tmp9ED;});void*_tmp66C[1U];_tmp66C[0]=& _tmp66E;({unsigned int _tmpC4C=loc;struct _dyneither_ptr _tmpC4B=({const char*_tmp66D="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp66D,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpC4C,_tmpC4B,_tag_dyneither(_tmp66C,sizeof(void*),1U));});});}
# 3888
({union Cyc_Absyn_AggrInfo _tmpC4D=Cyc_Absyn_KnownAggr(adp);*info=_tmpC4D;});};
# 3872
;_pop_handler();}else{void*_tmp661=(void*)_exn_thrown;void*_tmp66F=_tmp661;void*_tmp675;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp66F)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3892
struct Cyc_Absyn_Aggrdecl*_tmp670=({struct Cyc_Absyn_Aggrdecl*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674->kind=_tmp681,_tmp674->sc=Cyc_Absyn_Extern,_tmp674->name=_tmp680,_tmp674->tvs=0,_tmp674->impl=0,_tmp674->attributes=0,_tmp674->expected_mem_kind=0;_tmp674;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp670);
({struct Cyc_Absyn_Aggrdecl**_tmpC4E=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp680);adp=_tmpC4E;});
({union Cyc_Absyn_AggrInfo _tmpC4F=Cyc_Absyn_KnownAggr(adp);*info=_tmpC4F;});
# 3897
if(*targs != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp673=({struct Cyc_String_pa_PrintArg_struct _tmp9EE;_tmp9EE.tag=0U,({struct _dyneither_ptr _tmpC50=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp680));_tmp9EE.f1=_tmpC50;});_tmp9EE;});void*_tmp671[1U];_tmp671[0]=& _tmp673;({unsigned int _tmpC52=loc;struct _dyneither_ptr _tmpC51=({const char*_tmp672="declare parameterized type %s before using";_tag_dyneither(_tmp672,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpC52,_tmpC51,_tag_dyneither(_tmp671,sizeof(void*),1U));});});
return cvtenv;}
# 3901
goto _LL5;}}else{_LL8: _tmp675=_tmp66F;_LL9:(int)_rethrow(_tmp675);}_LL5:;}};}
# 3903
_tmp682=*adp;goto _LL4;}}else{_LL3: _tmp682=*(_tmp65F.KnownAggr).val;_LL4: {
# 3905
struct Cyc_List_List*tvs=_tmp682->tvs;
struct Cyc_List_List*ts=*targs;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp676=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp677=(void*)ts->hd;
# 3913
{struct _tuple0 _tmp678=({struct _tuple0 _tmp9EF;({void*_tmpC53=Cyc_Absyn_compress_kb(_tmp676->kind);_tmp9EF.f1=_tmpC53;}),_tmp9EF.f2=_tmp677;_tmp9EF;});struct _tuple0 _tmp679=_tmp678;struct Cyc_Absyn_Tvar*_tmp67A;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp679.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp679.f2)->tag == 2U){_LLB: _tmp67A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp679.f2)->f1;_LLC:
# 3915
({struct Cyc_List_List*_tmpC54=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp67A);cvtenv.kind_env=_tmpC54;});
({struct Cyc_List_List*_tmpC55=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp67A,1);cvtenv.free_vars=_tmpC55;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3920
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpC56=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpC56;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 3924
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp67D=({struct Cyc_String_pa_PrintArg_struct _tmp9F0;_tmp9F0.tag=0U,({struct _dyneither_ptr _tmpC57=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp682->name));_tmp9F0.f1=_tmpC57;});_tmp9F0;});void*_tmp67B[1U];_tmp67B[0]=& _tmp67D;({unsigned int _tmpC59=loc;struct _dyneither_ptr _tmpC58=({const char*_tmp67C="too many parameters for type %s";_tag_dyneither(_tmp67C,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpC59,_tmpC58,_tag_dyneither(_tmp67B,sizeof(void*),1U));});});
if(tvs != 0){
# 3928
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpC5A=({struct Cyc_List_List*_tmp67E=_cycalloc(sizeof(*_tmp67E));_tmp67E->hd=e,_tmp67E->tl=hidden_ts;_tmp67E;});hidden_ts=_tmpC5A;});
({struct Cyc_Tcutil_CVTEnv _tmpC5B=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpC5B;});}
# 3935
({struct Cyc_List_List*_tmpC5D=({struct Cyc_List_List*_tmpC5C=*targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC5C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targs=_tmpC5D;});}
# 3937
if((allow_abs_aggr  && _tmp682->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 3942
_tmp682->expected_mem_kind=1;}}_LL0:;}
# 3945
return cvtenv;}
# 3949
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeInfo*info,struct Cyc_List_List**targsp,int allow_abs_aggr){
# 3955
struct Cyc_List_List*_tmp683=*targsp;
{union Cyc_Absyn_DatatypeInfo _tmp684=*info;union Cyc_Absyn_DatatypeInfo _tmp685=_tmp684;struct Cyc_Absyn_Datatypedecl*_tmp69C;struct _tuple2*_tmp69B;int _tmp69A;if((_tmp685.UnknownDatatype).tag == 1){_LL1: _tmp69B=((_tmp685.UnknownDatatype).val).name;_tmp69A=((_tmp685.UnknownDatatype).val).is_extensible;_LL2: {
# 3958
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp686;_push_handler(& _tmp686);{int _tmp688=0;if(setjmp(_tmp686.handler))_tmp688=1;if(!_tmp688){({struct Cyc_Absyn_Datatypedecl**_tmpC5E=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp69B);tudp=_tmpC5E;});;_pop_handler();}else{void*_tmp687=(void*)_exn_thrown;void*_tmp689=_tmp687;void*_tmp68F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp689)->tag == Cyc_Dict_Absent){_LL6: _LL7: {
# 3962
struct Cyc_Absyn_Datatypedecl*_tmp68A=({struct Cyc_Absyn_Datatypedecl*_tmp68E=_cycalloc(sizeof(*_tmp68E));_tmp68E->sc=Cyc_Absyn_Extern,_tmp68E->name=_tmp69B,_tmp68E->tvs=0,_tmp68E->fields=0,_tmp68E->is_extensible=_tmp69A;_tmp68E;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp68A);
({struct Cyc_Absyn_Datatypedecl**_tmpC5F=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp69B);tudp=_tmpC5F;});
# 3966
if(_tmp683 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp68D=({struct Cyc_String_pa_PrintArg_struct _tmp9F1;_tmp9F1.tag=0U,({
struct _dyneither_ptr _tmpC60=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69B));_tmp9F1.f1=_tmpC60;});_tmp9F1;});void*_tmp68B[1U];_tmp68B[0]=& _tmp68D;({unsigned int _tmpC62=loc;struct _dyneither_ptr _tmpC61=({const char*_tmp68C="declare parameterized datatype %s before using";_tag_dyneither(_tmp68C,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpC62,_tmpC61,_tag_dyneither(_tmp68B,sizeof(void*),1U));});});
return cvtenv;}
# 3971
goto _LL5;}}else{_LL8: _tmp68F=_tmp689;_LL9:(int)_rethrow(_tmp68F);}_LL5:;}};}
# 3975
if(_tmp69A  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp692=({struct Cyc_String_pa_PrintArg_struct _tmp9F2;_tmp9F2.tag=0U,({struct _dyneither_ptr _tmpC63=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69B));_tmp9F2.f1=_tmpC63;});_tmp9F2;});void*_tmp690[1U];_tmp690[0]=& _tmp692;({unsigned int _tmpC65=loc;struct _dyneither_ptr _tmpC64=({const char*_tmp691="datatype %s was not declared @extensible";_tag_dyneither(_tmp691,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpC65,_tmpC64,_tag_dyneither(_tmp690,sizeof(void*),1U));});});
({union Cyc_Absyn_DatatypeInfo _tmpC66=Cyc_Absyn_KnownDatatype(tudp);*info=_tmpC66;});
_tmp69C=*tudp;goto _LL4;}}else{_LL3: _tmp69C=*(_tmp685.KnownDatatype).val;_LL4: {
# 3981
struct Cyc_List_List*tvs=_tmp69C->tvs;
for(0;_tmp683 != 0  && tvs != 0;(_tmp683=_tmp683->tl,tvs=tvs->tl)){
void*t=(void*)_tmp683->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3987
{struct _tuple0 _tmp693=({struct _tuple0 _tmp9F3;({void*_tmpC67=Cyc_Absyn_compress_kb(tv->kind);_tmp9F3.f1=_tmpC67;}),_tmp9F3.f2=t;_tmp9F3;});struct _tuple0 _tmp694=_tmp693;struct Cyc_Absyn_Tvar*_tmp695;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp694.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp694.f2)->tag == 2U){_LLB: _tmp695=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp694.f2)->f1;_LLC:
# 3989
({struct Cyc_List_List*_tmpC68=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp695);cvtenv.kind_env=_tmpC68;});
({struct Cyc_List_List*_tmpC69=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp695,1);cvtenv.free_vars=_tmpC69;});
continue;}else{goto _LLD;}}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}{
# 3994
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpC6A=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpC6A;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3998
if(_tmp683 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp698=({struct Cyc_String_pa_PrintArg_struct _tmp9F4;_tmp9F4.tag=0U,({
struct _dyneither_ptr _tmpC6B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp69C->name));_tmp9F4.f1=_tmpC6B;});_tmp9F4;});void*_tmp696[1U];_tmp696[0]=& _tmp698;({unsigned int _tmpC6D=loc;struct _dyneither_ptr _tmpC6C=({const char*_tmp697="too many type arguments for datatype %s";_tag_dyneither(_tmp697,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpC6D,_tmpC6C,_tag_dyneither(_tmp696,sizeof(void*),1U));});});
if(tvs != 0){
# 4003
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpC6E=({struct Cyc_List_List*_tmp699=_cycalloc(sizeof(*_tmp699));_tmp699->hd=e,_tmp699->tl=hidden_ts;_tmp699;});hidden_ts=_tmpC6E;});
({struct Cyc_Tcutil_CVTEnv _tmpC6F=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpC6F;});}
# 4010
({struct Cyc_List_List*_tmpC71=({struct Cyc_List_List*_tmpC70=*targsp;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpC70,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*targsp=_tmpC71;});}
# 4012
goto _LL0;}}_LL0:;}
# 4014
return cvtenv;}
# 4018
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_datatype_field(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,union Cyc_Absyn_DatatypeFieldInfo*info,struct Cyc_List_List*targs,int allow_abs_aggr){
# 4024
{union Cyc_Absyn_DatatypeFieldInfo _tmp69D=*info;union Cyc_Absyn_DatatypeFieldInfo _tmp69E=_tmp69D;struct Cyc_Absyn_Datatypedecl*_tmp6B1;struct Cyc_Absyn_Datatypefield*_tmp6B0;struct _tuple2*_tmp6AF;struct _tuple2*_tmp6AE;int _tmp6AD;if((_tmp69E.UnknownDatatypefield).tag == 1){_LL1: _tmp6AF=((_tmp69E.UnknownDatatypefield).val).datatype_name;_tmp6AE=((_tmp69E.UnknownDatatypefield).val).field_name;_tmp6AD=((_tmp69E.UnknownDatatypefield).val).is_extensible;_LL2: {
# 4027
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp6AF);
struct Cyc_Absyn_Datatypefield*tuf;
# 4032
{struct Cyc_List_List*_tmp69F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp69F=_tmp69F->tl){
if(_tmp69F == 0)({void*_tmp6A0=0U;({struct _dyneither_ptr _tmpC72=({const char*_tmp6A1="Tcutil found a bad datatypefield";_tag_dyneither(_tmp6A1,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC72,_tag_dyneither(_tmp6A0,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp69F->hd)->name,_tmp6AE)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp69F->hd;
break;}}}
# 4039
({union Cyc_Absyn_DatatypeFieldInfo _tmpC73=Cyc_Absyn_KnownDatatypefield(tud,tuf);*info=_tmpC73;});
_tmp6B1=tud;_tmp6B0=tuf;goto _LL4;}}else{_LL3: _tmp6B1=((_tmp69E.KnownDatatypefield).val).f1;_tmp6B0=((_tmp69E.KnownDatatypefield).val).f2;_LL4: {
# 4043
struct Cyc_List_List*tvs=_tmp6B1->tvs;
for(0;targs != 0  && tvs != 0;(targs=targs->tl,tvs=tvs->tl)){
void*t=(void*)targs->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4049
{struct _tuple0 _tmp6A2=({struct _tuple0 _tmp9F5;({void*_tmpC74=Cyc_Absyn_compress_kb(tv->kind);_tmp9F5.f1=_tmpC74;}),_tmp9F5.f2=t;_tmp9F5;});struct _tuple0 _tmp6A3=_tmp6A2;struct Cyc_Absyn_Tvar*_tmp6A4;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6A3.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A3.f2)->tag == 2U){_LL6: _tmp6A4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6A3.f2)->f1;_LL7:
# 4051
({struct Cyc_List_List*_tmpC75=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp6A4);cvtenv.kind_env=_tmpC75;});
({struct Cyc_List_List*_tmpC76=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp6A4,1);cvtenv.free_vars=_tmpC76;});
continue;}else{goto _LL8;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}{
# 4056
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpC77=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpC77;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4060
if(targs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6A7=({struct Cyc_String_pa_PrintArg_struct _tmp9F7;_tmp9F7.tag=0U,({
struct _dyneither_ptr _tmpC78=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B1->name));_tmp9F7.f1=_tmpC78;});_tmp9F7;});struct Cyc_String_pa_PrintArg_struct _tmp6A8=({struct Cyc_String_pa_PrintArg_struct _tmp9F6;_tmp9F6.tag=0U,({struct _dyneither_ptr _tmpC79=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B0->name));_tmp9F6.f1=_tmpC79;});_tmp9F6;});void*_tmp6A5[2U];_tmp6A5[0]=& _tmp6A7,_tmp6A5[1]=& _tmp6A8;({unsigned int _tmpC7B=loc;struct _dyneither_ptr _tmpC7A=({const char*_tmp6A6="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp6A6,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpC7B,_tmpC7A,_tag_dyneither(_tmp6A5,sizeof(void*),2U));});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp6AB=({struct Cyc_String_pa_PrintArg_struct _tmp9F9;_tmp9F9.tag=0U,({
struct _dyneither_ptr _tmpC7C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B1->name));_tmp9F9.f1=_tmpC7C;});_tmp9F9;});struct Cyc_String_pa_PrintArg_struct _tmp6AC=({struct Cyc_String_pa_PrintArg_struct _tmp9F8;_tmp9F8.tag=0U,({struct _dyneither_ptr _tmpC7D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp6B0->name));_tmp9F8.f1=_tmpC7D;});_tmp9F8;});void*_tmp6A9[2U];_tmp6A9[0]=& _tmp6AB,_tmp6A9[1]=& _tmp6AC;({unsigned int _tmpC7F=loc;struct _dyneither_ptr _tmpC7E=({const char*_tmp6AA="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp6AA,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpC7F,_tmpC7E,_tag_dyneither(_tmp6A9,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 4068
return cvtenv;}
# 4071
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_app(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*c,struct Cyc_List_List**targsp,int put_in_effect,int allow_abs_aggr){
# 4076
struct Cyc_List_List*_tmp6B2=*targsp;
{void*_tmp6B3=c;union Cyc_Absyn_DatatypeFieldInfo*_tmp6DA;union Cyc_Absyn_DatatypeInfo*_tmp6D9;union Cyc_Absyn_AggrInfo*_tmp6D8;struct Cyc_List_List*_tmp6D7;struct _tuple2*_tmp6D6;struct Cyc_Absyn_Enumdecl**_tmp6D5;switch(*((int*)_tmp6B3)){case 1U: _LL1: _LL2:
# 4079
 goto _LL4;case 2U: _LL3: _LL4: goto _LL6;case 0U: _LL5: _LL6: goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 6U: _LL9: _LLA: goto _LLC;case 5U: _LLB: _LLC: goto _LLE;case 12U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10: goto _LL12;case 14U: _LL11: _LL12: goto _LL14;case 17U: _LL13: _LL14:
# 4083
 if(_tmp6B2 != 0)({struct Cyc_String_pa_PrintArg_struct _tmp6B6=({struct Cyc_String_pa_PrintArg_struct _tmp9FA;_tmp9FA.tag=0U,({
struct _dyneither_ptr _tmpC80=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));_tmp6B7->tag=0U,_tmp6B7->f1=c,_tmp6B7->f2=_tmp6B2;_tmp6B7;})));_tmp9FA.f1=_tmpC80;});_tmp9FA;});void*_tmp6B4[1U];_tmp6B4[0]=& _tmp6B6;({struct _dyneither_ptr _tmpC81=({const char*_tmp6B5="%s applied to argument(s)";_tag_dyneither(_tmp6B5,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC81,_tag_dyneither(_tmp6B4,sizeof(void*),1U));});});
goto _LL0;case 9U: _LL15: _LL16:
# 4087
 for(0;_tmp6B2 != 0;_tmp6B2=_tmp6B2->tl){
({struct Cyc_Tcutil_CVTEnv _tmpC82=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp6B2->hd,1,1);cvtenv=_tmpC82;});}
goto _LL0;case 4U: _LL17: _LL18:
# 4091
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B2)!= 1)({void*_tmp6B8=0U;({struct _dyneither_ptr _tmpC83=({const char*_tmp6B9="tag_t applied to wrong number of arguments";_tag_dyneither(_tmp6B9,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC83,_tag_dyneither(_tmp6B8,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC84=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B2))->hd,0,1);cvtenv=_tmpC84;});goto _LL0;case 15U: _LL19: _tmp6D6=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B3)->f1;_tmp6D5=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6B3)->f2;_LL1A:
# 4094
 if(_tmp6B2 != 0)({void*_tmp6BA=0U;({struct _dyneither_ptr _tmpC85=({const char*_tmp6BB="enum applied to argument(s)";_tag_dyneither(_tmp6BB,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC85,_tag_dyneither(_tmp6BA,sizeof(void*),0U));});});
if(*_tmp6D5 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp6D5))->fields == 0){
struct _handler_cons _tmp6BC;_push_handler(& _tmp6BC);{int _tmp6BE=0;if(setjmp(_tmp6BC.handler))_tmp6BE=1;if(!_tmp6BE){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D6);
*_tmp6D5=*ed;}
# 4097
;_pop_handler();}else{void*_tmp6BD=(void*)_exn_thrown;void*_tmp6BF=_tmp6BD;void*_tmp6C2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp6BF)->tag == Cyc_Dict_Absent){_LL2C: _LL2D: {
# 4101
struct Cyc_Absyn_Enumdecl*_tmp6C0=({struct Cyc_Absyn_Enumdecl*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1->sc=Cyc_Absyn_Extern,_tmp6C1->name=_tmp6D6,_tmp6C1->fields=0;_tmp6C1;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp6C0);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp6D6);
*_tmp6D5=*ed;
goto _LL2B;};}}else{_LL2E: _tmp6C2=_tmp6BF;_LL2F:(int)_rethrow(_tmp6C2);}_LL2B:;}};}
# 4107
goto _LL0;case 16U: _LL1B: _tmp6D7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp6B3)->f1;_LL1C:
# 4109
 if(_tmp6B2 != 0)({void*_tmp6C3=0U;({struct _dyneither_ptr _tmpC86=({const char*_tmp6C4="enum applied to argument(s)";_tag_dyneither(_tmp6C4,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC86,_tag_dyneither(_tmp6C3,sizeof(void*),0U));});});{
# 4111
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp6D7 != 0;_tmp6D7=_tmp6D7->tl){
struct Cyc_Absyn_Enumfield*_tmp6C5=(struct Cyc_Absyn_Enumfield*)_tmp6D7->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp6C5->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp6C8=({struct Cyc_String_pa_PrintArg_struct _tmp9FB;_tmp9FB.tag=0U,_tmp9FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C5->name).f2);_tmp9FB;});void*_tmp6C6[1U];_tmp6C6[0]=& _tmp6C8;({unsigned int _tmpC88=_tmp6C5->loc;struct _dyneither_ptr _tmpC87=({const char*_tmp6C7="duplicate enum field name %s";_tag_dyneither(_tmp6C7,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpC88,_tmpC87,_tag_dyneither(_tmp6C6,sizeof(void*),1U));});});else{
# 4118
({struct Cyc_List_List*_tmpC89=({struct Cyc_List_List*_tmp6C9=_cycalloc(sizeof(*_tmp6C9));_tmp6C9->hd=(*_tmp6C5->name).f2,_tmp6C9->tl=prev_fields;_tmp6C9;});prev_fields=_tmpC89;});}
if(_tmp6C5->tag == 0)
({struct Cyc_Absyn_Exp*_tmpC8A=Cyc_Absyn_uint_exp(tag_count,_tmp6C5->loc);_tmp6C5->tag=_tmpC8A;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C5->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp6CC=({struct Cyc_String_pa_PrintArg_struct _tmp9FC;_tmp9FC.tag=0U,_tmp9FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp6C5->name).f2);_tmp9FC;});void*_tmp6CA[1U];_tmp6CA[0]=& _tmp6CC;({unsigned int _tmpC8C=loc;struct _dyneither_ptr _tmpC8B=({const char*_tmp6CB="enum field %s: expression is not constant";_tag_dyneither(_tmp6CB,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpC8C,_tmpC8B,_tag_dyneither(_tmp6CA,sizeof(void*),1U));});});}{
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp6C5->tag))).f1;
tag_count=t1 + 1;};}
# 4126
goto _LL0;};case 10U: _LL1D: _LL1E:
# 4128
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B2)!= 1)({void*_tmp6CD=0U;({struct _dyneither_ptr _tmpC8D=({const char*_tmp6CE="regions has wrong number of arguments";_tag_dyneither(_tmp6CE,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC8D,_tag_dyneither(_tmp6CD,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC8E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,(void*)((struct Cyc_List_List*)_check_null(_tmp6B2))->hd,1,1);cvtenv=_tmpC8E;});goto _LL0;case 3U: _LL1F: _LL20:
# 4131
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B2)!= 1)({void*_tmp6CF=0U;({struct _dyneither_ptr _tmpC8F=({const char*_tmp6D0="region_t has wrong number of arguments";_tag_dyneither(_tmp6D0,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC8F,_tag_dyneither(_tmp6CF,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC90=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B2))->hd,1,1);cvtenv=_tmpC90;});
goto _LL0;case 13U: _LL21: _LL22:
# 4135
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B2)!= 1)({void*_tmp6D1=0U;({struct _dyneither_ptr _tmpC91=({const char*_tmp6D2="@thin has wrong number of arguments";_tag_dyneither(_tmp6D2,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC91,_tag_dyneither(_tmp6D1,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC92=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,(void*)((struct Cyc_List_List*)_check_null(_tmp6B2))->hd,0,1);cvtenv=_tmpC92;});
goto _LL0;case 8U: _LL23: _LL24:
# 4139
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp6B2)!= 1)({void*_tmp6D3=0U;({struct _dyneither_ptr _tmpC93=({const char*_tmp6D4="access has wrong number of arguments";_tag_dyneither(_tmp6D4,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC93,_tag_dyneither(_tmp6D3,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpC94=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,(void*)((struct Cyc_List_List*)_check_null(_tmp6B2))->hd,1,1);cvtenv=_tmpC94;});goto _LL0;case 20U: _LL25: _tmp6D8=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp6B3)->f1;_LL26:
# 4142
({struct Cyc_Tcutil_CVTEnv _tmpC95=Cyc_Tcutil_i_check_valid_aggr(loc,te,cvtenv,expected_kind,_tmp6D8,targsp,allow_abs_aggr);cvtenv=_tmpC95;});
# 4144
goto _LL0;case 18U: _LL27: _tmp6D9=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp6B3)->f1;_LL28:
# 4146
({struct Cyc_Tcutil_CVTEnv _tmpC96=Cyc_Tcutil_i_check_valid_datatype(loc,te,cvtenv,expected_kind,_tmp6D9,targsp,allow_abs_aggr);cvtenv=_tmpC96;});
# 4148
goto _LL0;default: _LL29: _tmp6DA=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp6B3)->f1;_LL2A:
# 4150
({struct Cyc_Tcutil_CVTEnv _tmpC97=Cyc_Tcutil_i_check_valid_datatype_field(loc,te,cvtenv,expected_kind,_tmp6DA,_tmp6B2,allow_abs_aggr);cvtenv=_tmpC97;});
# 4152
goto _LL0;}_LL0:;}
# 4154
return cvtenv;}struct _tuple33{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 4158
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 4166
{void*_tmp6DB=Cyc_Tcutil_compress(t);void*_tmp6DC=_tmp6DB;struct _tuple2*_tmp7E0;struct Cyc_List_List**_tmp7DF;struct Cyc_Absyn_Typedefdecl**_tmp7DE;void**_tmp7DD;enum Cyc_Absyn_AggrKind _tmp7DC;struct Cyc_List_List*_tmp7DB;struct Cyc_List_List*_tmp7DA;struct Cyc_List_List**_tmp7D9;void**_tmp7D8;struct Cyc_Absyn_Tqual*_tmp7D7;void*_tmp7D6;struct Cyc_List_List*_tmp7D5;int _tmp7D4;struct Cyc_Absyn_VarargInfo*_tmp7D3;struct Cyc_List_List*_tmp7D2;struct Cyc_List_List*_tmp7D1;struct Cyc_Absyn_Exp*_tmp7D0;struct Cyc_List_List**_tmp7CF;struct Cyc_Absyn_Exp*_tmp7CE;struct Cyc_List_List**_tmp7CD;void*_tmp7CC;struct Cyc_Absyn_Tqual*_tmp7CB;struct Cyc_Absyn_Exp**_tmp7CA;void*_tmp7C9;unsigned int _tmp7C8;struct Cyc_Absyn_Exp*_tmp7C7;struct Cyc_Absyn_Exp*_tmp7C6;void*_tmp7C5;struct Cyc_Absyn_Tqual*_tmp7C4;void*_tmp7C3;void*_tmp7C2;void*_tmp7C1;void*_tmp7C0;void*_tmp7BF;void***_tmp7BE;struct Cyc_Absyn_Tvar*_tmp7BD;struct Cyc_Core_Opt**_tmp7BC;void**_tmp7BB;void*_tmp7BA;struct Cyc_List_List**_tmp7B9;switch(*((int*)_tmp6DC)){case 0U: _LL1: _tmp7BA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DC)->f1;_tmp7B9=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6DC)->f2;_LL2:
# 4168
({struct Cyc_Tcutil_CVTEnv _tmpC98=Cyc_Tcutil_i_check_valid_type_app(loc,te,cvtenv,expected_kind,_tmp7BA,_tmp7B9,put_in_effect,allow_abs_aggr);cvtenv=_tmpC98;});
# 4170
goto _LL0;case 1U: _LL3: _tmp7BC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DC)->f1;_tmp7BB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp6DC)->f2;_LL4:
# 4173
 if(*_tmp7BC == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BC))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp7BC))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpC99=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp7BC=_tmpC99;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 4178
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp7BB=Cyc_Absyn_unique_rgn_type;else{
# 4181
*_tmp7BB=Cyc_Absyn_heap_rgn_type;}}else{
if((cvtenv.generalize_evars  && expected_kind->kind != Cyc_Absyn_BoolKind) && expected_kind->kind != Cyc_Absyn_PtrBndKind){
# 4185
struct Cyc_Absyn_Tvar*_tmp6DD=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6DE=_cycalloc(sizeof(*_tmp6DE));_tmp6DE->tag=2U,_tmp6DE->f1=0,_tmp6DE->f2=expected_kind;_tmp6DE;}));
({void*_tmpC9A=Cyc_Absyn_var_type(_tmp6DD);*_tmp7BB=_tmpC9A;});
_tmp7BD=_tmp6DD;goto _LL6;}else{
# 4189
({struct Cyc_List_List*_tmpC9B=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpC9B;});}}
# 4191
goto _LL0;case 2U: _LL5: _tmp7BD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6DC)->f1;_LL6:
# 4193
{void*_tmp6DF=Cyc_Absyn_compress_kb(_tmp7BD->kind);void*_tmp6E0=_tmp6DF;struct Cyc_Core_Opt**_tmp6E3;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6E0)->tag == 1U){_LL1A: _tmp6E3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp6E0)->f1;_LL1B:
# 4195
({struct Cyc_Core_Opt*_tmpC9D=({struct Cyc_Core_Opt*_tmp6E2=_cycalloc(sizeof(*_tmp6E2));({void*_tmpC9C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E1=_cycalloc(sizeof(*_tmp6E1));_tmp6E1->tag=2U,_tmp6E1->f1=0,_tmp6E1->f2=expected_kind;_tmp6E1;});_tmp6E2->v=_tmpC9C;});_tmp6E2;});*_tmp6E3=_tmpC9D;});goto _LL19;}else{_LL1C: _LL1D:
 goto _LL19;}_LL19:;}
# 4200
({struct Cyc_List_List*_tmpC9E=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7BD);cvtenv.kind_env=_tmpC9E;});
# 4203
({struct Cyc_List_List*_tmpC9F=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7BD,put_in_effect);cvtenv.free_vars=_tmpC9F;});
# 4205
{void*_tmp6E4=Cyc_Absyn_compress_kb(_tmp7BD->kind);void*_tmp6E5=_tmp6E4;struct Cyc_Core_Opt**_tmp6E9;struct Cyc_Absyn_Kind*_tmp6E8;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E5)->tag == 2U){_LL1F: _tmp6E9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E5)->f1;_tmp6E8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp6E5)->f2;_LL20:
# 4207
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp6E8))
({struct Cyc_Core_Opt*_tmpCA1=({struct Cyc_Core_Opt*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));({void*_tmpCA0=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp6E6=_cycalloc(sizeof(*_tmp6E6));_tmp6E6->tag=2U,_tmp6E6->f1=0,_tmp6E6->f2=expected_kind;_tmp6E6;});_tmp6E7->v=_tmpCA0;});_tmp6E7;});*_tmp6E9=_tmpCA1;});
goto _LL1E;}else{_LL21: _LL22:
 goto _LL1E;}_LL1E:;}
# 4212
goto _LL0;case 10U: _LL7: _tmp7BF=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DC)->f1)->r;_tmp7BE=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6DC)->f2;_LL8: {
# 4218
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp6EA=_tmp7BF;struct Cyc_Absyn_Datatypedecl*_tmp6ED;struct Cyc_Absyn_Enumdecl*_tmp6EC;struct Cyc_Absyn_Aggrdecl*_tmp6EB;switch(*((int*)_tmp6EA)){case 0U: _LL24: _tmp6EB=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp6EA)->f1;_LL25:
# 4221
 if(te->in_extern_c_include)
_tmp6EB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp6EB);goto _LL23;case 1U: _LL26: _tmp6EC=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp6EA)->f1;_LL27:
# 4225
 if(te->in_extern_c_include)
_tmp6EC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp6EC);goto _LL23;default: _LL28: _tmp6ED=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp6EA)->f1;_LL29:
# 4229
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp6ED);goto _LL23;}_LL23:;}
# 4231
({void**_tmpCA2=({void**_tmp6EE=_cycalloc(sizeof(*_tmp6EE));*_tmp6EE=new_t;_tmp6EE;});*_tmp7BE=_tmpCA2;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 3U: _LL9: _tmp7C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DC)->f1).elt_type;_tmp7C4=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DC)->f1).elt_tq;_tmp7C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DC)->f1).ptr_atts).rgn;_tmp7C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DC)->f1).ptr_atts).nullable;_tmp7C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DC)->f1).ptr_atts).bounds;_tmp7C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6DC)->f1).ptr_atts).zero_term;_LLA: {
# 4237
int is_zero_terminated;
# 4239
({struct Cyc_Tcutil_CVTEnv _tmpCA3=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7C5,1,1);cvtenv=_tmpCA3;});
({int _tmpCA4=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7C4->print_const,_tmp7C5);_tmp7C4->real_const=_tmpCA4;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp6EF=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp6F0=_tmp6EF;switch(_tmp6F0){case Cyc_Absyn_Aliasable: _LL2B: _LL2C:
 k=& Cyc_Tcutil_rk;goto _LL2A;case Cyc_Absyn_Unique: _LL2D: _LL2E:
 k=& Cyc_Tcutil_urk;goto _LL2A;default: _LL2F: _LL30:
 k=& Cyc_Tcutil_trk;goto _LL2A;}_LL2A:;}
# 4247
({struct Cyc_Tcutil_CVTEnv _tmpCA5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp7C3,1,1);cvtenv=_tmpCA5;});
# 4250
({struct Cyc_Tcutil_CVTEnv _tmpCA6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7C0,0,1);cvtenv=_tmpCA6;});
({struct Cyc_Tcutil_CVTEnv _tmpCA7=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_boolk,_tmp7C2,0,1);cvtenv=_tmpCA7;});
({void*_tmpCA8=_tmp7C0;Cyc_Tcutil_unify(_tmpCA8,Cyc_Tcutil_is_char_type(_tmp7C5)?Cyc_Absyn_true_type: Cyc_Absyn_false_type);});
({int _tmpCA9=Cyc_Absyn_type2bool(0,_tmp7C0);is_zero_terminated=_tmpCA9;});
if(is_zero_terminated){
# 4256
if(!Cyc_Tcutil_admits_zero(_tmp7C5))
({struct Cyc_String_pa_PrintArg_struct _tmp6F3=({struct Cyc_String_pa_PrintArg_struct _tmp9FD;_tmp9FD.tag=0U,({
struct _dyneither_ptr _tmpCAA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7C5));_tmp9FD.f1=_tmpCAA;});_tmp9FD;});void*_tmp6F1[1U];_tmp6F1[0]=& _tmp6F3;({unsigned int _tmpCAC=loc;struct _dyneither_ptr _tmpCAB=({const char*_tmp6F2="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp6F2,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpCAC,_tmpCAB,_tag_dyneither(_tmp6F1,sizeof(void*),1U));});});}
# 4261
({struct Cyc_Tcutil_CVTEnv _tmpCAD=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ptrbk,_tmp7C1,0,allow_abs_aggr);cvtenv=_tmpCAD;});{
struct Cyc_Absyn_Exp*_tmp6F4=({void*_tmpCAE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCAE,_tmp7C1);});
if(_tmp6F4 != 0){
struct _tuple13 _tmp6F5=Cyc_Evexp_eval_const_uint_exp(_tmp6F4);struct _tuple13 _tmp6F6=_tmp6F5;unsigned int _tmp6FA;int _tmp6F9;_LL32: _tmp6FA=_tmp6F6.f1;_tmp6F9=_tmp6F6.f2;_LL33:;
if(is_zero_terminated  && (!_tmp6F9  || _tmp6FA < 1))
({void*_tmp6F7=0U;({unsigned int _tmpCB0=loc;struct _dyneither_ptr _tmpCAF=({const char*_tmp6F8="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp6F8,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpCB0,_tmpCAF,_tag_dyneither(_tmp6F7,sizeof(void*),0U));});});}
# 4268
goto _LL0;};};}case 9U: _LLB: _tmp7C6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6DC)->f1;_LLC:
# 4273
({struct Cyc_Tcenv_Tenv*_tmpCB1=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpCB1,0,_tmp7C6);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp7C6))
({void*_tmp6FB=0U;({unsigned int _tmpCB3=loc;struct _dyneither_ptr _tmpCB2=({const char*_tmp6FC="valueof_t requires an int expression";_tag_dyneither(_tmp6FC,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpCB3,_tmpCB2,_tag_dyneither(_tmp6FB,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpCB4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7C6,te,cvtenv);cvtenv=_tmpCB4;});
goto _LL0;case 11U: _LLD: _tmp7C7=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6DC)->f1;_LLE:
# 4282
({struct Cyc_Tcenv_Tenv*_tmpCB5=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpCB5,0,_tmp7C7);});
goto _LL0;case 4U: _LLF: _tmp7CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DC)->f1).elt_type;_tmp7CB=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DC)->f1).tq;_tmp7CA=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DC)->f1).num_elts;_tmp7C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DC)->f1).zero_term;_tmp7C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6DC)->f1).zt_loc;_LL10: {
# 4287
struct Cyc_Absyn_Exp*_tmp6FD=*_tmp7CA;
({struct Cyc_Tcutil_CVTEnv _tmpCB6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7CC,1,allow_abs_aggr);cvtenv=_tmpCB6;});
({int _tmpCB7=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7CB->print_const,_tmp7CC);_tmp7CB->real_const=_tmpCB7;});{
# 4291
int is_zero_terminated=Cyc_Tcutil_force_type2bool(0,_tmp7C9);
if(is_zero_terminated){
# 4294
if(!Cyc_Tcutil_admits_zero(_tmp7CC))
({struct Cyc_String_pa_PrintArg_struct _tmp700=({struct Cyc_String_pa_PrintArg_struct _tmp9FE;_tmp9FE.tag=0U,({
struct _dyneither_ptr _tmpCB8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7CC));_tmp9FE.f1=_tmpCB8;});_tmp9FE;});void*_tmp6FE[1U];_tmp6FE[0]=& _tmp700;({unsigned int _tmpCBA=loc;struct _dyneither_ptr _tmpCB9=({const char*_tmp6FF="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp6FF,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpCBA,_tmpCB9,_tag_dyneither(_tmp6FE,sizeof(void*),1U));});});}
# 4300
if(_tmp6FD == 0){
# 4302
if(is_zero_terminated)
# 4304
({struct Cyc_Absyn_Exp*_tmpCBC=({struct Cyc_Absyn_Exp*_tmpCBB=Cyc_Absyn_uint_exp(1U,0U);_tmp6FD=_tmpCBB;});*_tmp7CA=_tmpCBC;});else{
# 4307
({void*_tmp701=0U;({unsigned int _tmpCBE=loc;struct _dyneither_ptr _tmpCBD=({const char*_tmp702="array bound defaults to 1 here";_tag_dyneither(_tmp702,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpCBE,_tmpCBD,_tag_dyneither(_tmp701,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpCC0=({struct Cyc_Absyn_Exp*_tmpCBF=Cyc_Absyn_uint_exp(1U,0U);_tmp6FD=_tmpCBF;});*_tmp7CA=_tmpCC0;});}}
# 4311
({struct Cyc_Tcenv_Tenv*_tmpCC1=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpCC1,0,_tmp6FD);});
if(!Cyc_Tcutil_coerce_uint_type(te,_tmp6FD))
({void*_tmp703=0U;({unsigned int _tmpCC3=loc;struct _dyneither_ptr _tmpCC2=({const char*_tmp704="array bounds expression is not an unsigned int";_tag_dyneither(_tmp704,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCC3,_tmpCC2,_tag_dyneither(_tmp703,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpCC4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp6FD,te,cvtenv);cvtenv=_tmpCC4;});{
# 4319
struct _tuple13 _tmp705=Cyc_Evexp_eval_const_uint_exp(_tmp6FD);struct _tuple13 _tmp706=_tmp705;unsigned int _tmp70C;int _tmp70B;_LL35: _tmp70C=_tmp706.f1;_tmp70B=_tmp706.f2;_LL36:;
# 4321
if((is_zero_terminated  && _tmp70B) && _tmp70C < 1)
({void*_tmp707=0U;({unsigned int _tmpCC6=loc;struct _dyneither_ptr _tmpCC5=({const char*_tmp708="zero terminated array cannot have zero elements";_tag_dyneither(_tmp708,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpCC6,_tmpCC5,_tag_dyneither(_tmp707,sizeof(void*),0U));});});
# 4324
if((_tmp70B  && _tmp70C < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp709=0U;({unsigned int _tmpCC8=loc;struct _dyneither_ptr _tmpCC7=({const char*_tmp70A="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp70A,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpCC8,_tmpCC7,_tag_dyneither(_tmp709,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpCC9=Cyc_Absyn_uint_exp(1U,0U);*_tmp7CA=_tmpCC9;});}
# 4328
goto _LL0;};};}case 5U: _LL11: _tmp7D9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).tvars;_tmp7D8=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).effect;_tmp7D7=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).ret_tqual;_tmp7D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).ret_type;_tmp7D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).args;_tmp7D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).c_varargs;_tmp7D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).cyc_varargs;_tmp7D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).rgn_po;_tmp7D1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).attributes;_tmp7D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).requires_clause;_tmp7CF=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).requires_relns;_tmp7CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).ensures_clause;_tmp7CD=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6DC)->f1).ensures_relns;_LL12: {
# 4335
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=0U;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp7D1 != 0;_tmp7D1=_tmp7D1->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp7D1->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp70F=({struct Cyc_String_pa_PrintArg_struct _tmp9FF;_tmp9FF.tag=0U,({struct _dyneither_ptr _tmpCCA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp7D1->hd));_tmp9FF.f1=_tmpCCA;});_tmp9FF;});void*_tmp70D[1U];_tmp70D[0]=& _tmp70F;({unsigned int _tmpCCC=loc;struct _dyneither_ptr _tmpCCB=({const char*_tmp70E="bad function type attribute %s";_tag_dyneither(_tmp70E,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpCCC,_tmpCCB,_tag_dyneither(_tmp70D,sizeof(void*),1U));});});{
void*_tmp710=(void*)_tmp7D1->hd;void*_tmp711=_tmp710;enum Cyc_Absyn_Format_Type _tmp716;int _tmp715;int _tmp714;switch(*((int*)_tmp711)){case 1U: _LL38: _LL39:
# 4348
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL37;case 2U: _LL3A: _LL3B:
# 4350
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL37;case 3U: _LL3C: _LL3D:
# 4352
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL37;case 19U: _LL3E: _tmp716=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp711)->f1;_tmp715=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp711)->f2;_tmp714=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp711)->f3;_LL3F:
# 4354
 if(!seen_format){
seen_format=1;
ft=_tmp716;
fmt_desc_arg=_tmp715;
fmt_arg_start=_tmp714;}else{
# 4360
({void*_tmp712=0U;({unsigned int _tmpCCE=loc;struct _dyneither_ptr _tmpCCD=({const char*_tmp713="function can't have multiple format attributes";_tag_dyneither(_tmp713,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpCCE,_tmpCCD,_tag_dyneither(_tmp712,sizeof(void*),0U));});});}
goto _LL37;default: _LL40: _LL41:
 goto _LL37;}_LL37:;};}
# 4365
if(num_convs > 1)
({void*_tmp717=0U;({unsigned int _tmpCD0=loc;struct _dyneither_ptr _tmpCCF=({const char*_tmp718="function can't have multiple calling conventions";_tag_dyneither(_tmp718,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpCD0,_tmpCCF,_tag_dyneither(_tmp717,sizeof(void*),0U));});});
# 4370
Cyc_Tcutil_check_unique_tvars(loc,*_tmp7D9);
{struct Cyc_List_List*b=*_tmp7D9;for(0;b != 0;b=b->tl){
({int _tmpCD1=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpCD1;});
({struct Cyc_List_List*_tmpCD2=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpCD2;});{
void*_tmp719=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp71A=_tmp719;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp71A)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp71A)->f1)->kind == Cyc_Absyn_MemKind){_LL43: _LL44:
# 4376
({struct Cyc_String_pa_PrintArg_struct _tmp71D=({struct Cyc_String_pa_PrintArg_struct _tmpA00;_tmpA00.tag=0U,_tmpA00.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);_tmpA00;});void*_tmp71B[1U];_tmp71B[0]=& _tmp71D;({unsigned int _tmpCD4=loc;struct _dyneither_ptr _tmpCD3=({const char*_tmp71C="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp71C,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpCD4,_tmpCD3,_tag_dyneither(_tmp71B,sizeof(void*),1U));});});
goto _LL42;}else{goto _LL45;}}else{_LL45: _LL46:
 goto _LL42;}_LL42:;};}}{
# 4384
struct Cyc_Tcutil_CVTEnv _tmp71E=({struct Cyc_Tcutil_CVTEnv _tmpA03;_tmpA03.r=Cyc_Core_heap_region,_tmpA03.kind_env=cvtenv.kind_env,_tmpA03.free_vars=0,_tmpA03.free_evars=0,_tmpA03.generalize_evars=cvtenv.generalize_evars,_tmpA03.fn_result=1;_tmpA03;});
# 4389
({struct Cyc_Tcutil_CVTEnv _tmpCD5=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71E,& Cyc_Tcutil_tmk,_tmp7D6,1,1);_tmp71E=_tmpCD5;});
({int _tmpCD6=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7D7->print_const,_tmp7D6);_tmp7D7->real_const=_tmpCD6;});
_tmp71E.fn_result=0;
{struct Cyc_List_List*a=_tmp7D5;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp71F=(struct _tuple10*)a->hd;
void*_tmp720=(*_tmp71F).f3;
({struct Cyc_Tcutil_CVTEnv _tmpCD7=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71E,& Cyc_Tcutil_tmk,_tmp720,1,1);_tmp71E=_tmpCD7;});{
int _tmp721=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp71F).f2).print_const,_tmp720);
((*_tmp71F).f2).real_const=_tmp721;
# 4400
if(Cyc_Tcutil_is_array_type(_tmp720)){
# 4402
void*_tmp722=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpCD8=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71E,& Cyc_Tcutil_rk,_tmp722,1,1);_tmp71E=_tmpCD8;});
({void*_tmpCD9=Cyc_Tcutil_promote_array(_tmp720,_tmp722,0);(*_tmp71F).f3=_tmpCD9;});}};}}
# 4409
if(_tmp7D3 != 0){
if(_tmp7D4)({void*_tmp723=0U;({struct _dyneither_ptr _tmpCDA=({const char*_tmp724="both c_vararg and cyc_vararg";_tag_dyneither(_tmp724,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDA,_tag_dyneither(_tmp723,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp725=*_tmp7D3;struct Cyc_Absyn_VarargInfo _tmp726=_tmp725;void*_tmp737;int _tmp736;_LL48: _tmp737=_tmp726.type;_tmp736=_tmp726.inject;_LL49:;
({struct Cyc_Tcutil_CVTEnv _tmpCDB=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71E,& Cyc_Tcutil_tmk,_tmp737,1,1);_tmp71E=_tmpCDB;});
({int _tmpCDC=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp7D3->tq).print_const,_tmp737);(_tmp7D3->tq).real_const=_tmpCDC;});
# 4415
if(_tmp736){
void*_tmp727=Cyc_Tcutil_compress(_tmp737);void*_tmp728=_tmp727;void*_tmp735;void*_tmp734;void*_tmp733;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->tag == 3U){_LL4B: _tmp735=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->f1).elt_type;_tmp734=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->f1).ptr_atts).bounds;_tmp733=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp728)->f1).ptr_atts).zero_term;_LL4C:
# 4418
{void*_tmp729=Cyc_Tcutil_compress(_tmp735);void*_tmp72A=_tmp729;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72A)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp72A)->f1)->tag == 18U){_LL50: _LL51:
# 4420
 if(Cyc_Tcutil_force_type2bool(0,_tmp733))
({void*_tmp72B=0U;({unsigned int _tmpCDE=loc;struct _dyneither_ptr _tmpCDD=({const char*_tmp72C="can't inject into a zeroterm pointer";_tag_dyneither(_tmp72C,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpCDE,_tmpCDD,_tag_dyneither(_tmp72B,sizeof(void*),0U));});});
if(!({void*_tmpCDF=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmpCDF,_tmp734);}))
({void*_tmp72D=0U;({unsigned int _tmpCE1=loc;struct _dyneither_ptr _tmpCE0=({const char*_tmp72E="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp72E,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpCE1,_tmpCE0,_tag_dyneither(_tmp72D,sizeof(void*),0U));});});
goto _LL4F;}else{goto _LL52;}}else{_LL52: _LL53:
({void*_tmp72F=0U;({unsigned int _tmpCE3=loc;struct _dyneither_ptr _tmpCE2=({const char*_tmp730="can't inject a non-datatype type";_tag_dyneither(_tmp730,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpCE3,_tmpCE2,_tag_dyneither(_tmp72F,sizeof(void*),0U));});});goto _LL4F;}_LL4F:;}
# 4427
goto _LL4A;}else{_LL4D: _LL4E:
({void*_tmp731=0U;({unsigned int _tmpCE5=loc;struct _dyneither_ptr _tmpCE4=({const char*_tmp732="expecting a datatype pointer type";_tag_dyneither(_tmp732,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpCE5,_tmpCE4,_tag_dyneither(_tmp731,sizeof(void*),0U));});});goto _LL4A;}_LL4A:;}};}
# 4433
if(seen_format){
int _tmp738=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp7D5);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp738) || fmt_arg_start < 0) || 
# 4437
(_tmp7D3 == 0  && !_tmp7D4) && fmt_arg_start != 0) || 
(_tmp7D3 != 0  || _tmp7D4) && fmt_arg_start != _tmp738 + 1)
# 4440
({void*_tmp739=0U;({unsigned int _tmpCE7=loc;struct _dyneither_ptr _tmpCE6=({const char*_tmp73A="bad format descriptor";_tag_dyneither(_tmp73A,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpCE7,_tmpCE6,_tag_dyneither(_tmp739,sizeof(void*),0U));});});else{
# 4443
struct _tuple10 _tmp73B=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp7D5,fmt_desc_arg - 1);struct _tuple10 _tmp73C=_tmp73B;void*_tmp751;_LL55: _tmp751=_tmp73C.f3;_LL56:;
# 4445
{void*_tmp73D=Cyc_Tcutil_compress(_tmp751);void*_tmp73E=_tmp73D;void*_tmp74A;void*_tmp749;void*_tmp748;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73E)->tag == 3U){_LL58: _tmp74A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73E)->f1).elt_type;_tmp749=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73E)->f1).ptr_atts).bounds;_tmp748=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp73E)->f1).ptr_atts).zero_term;_LL59:
# 4448
 if(!Cyc_Tcutil_is_char_type(_tmp74A))
({void*_tmp73F=0U;({unsigned int _tmpCE9=loc;struct _dyneither_ptr _tmpCE8=({const char*_tmp740="format descriptor is not a string";_tag_dyneither(_tmp740,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpCE9,_tmpCE8,_tag_dyneither(_tmp73F,sizeof(void*),0U));});});else{
# 4451
struct Cyc_Absyn_Exp*_tmp741=({void*_tmpCEA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpCEA,_tmp749);});
if(_tmp741 == 0  && _tmp7D4)
({void*_tmp742=0U;({unsigned int _tmpCEC=loc;struct _dyneither_ptr _tmpCEB=({const char*_tmp743="format descriptor is not a char * type";_tag_dyneither(_tmp743,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCEC,_tmpCEB,_tag_dyneither(_tmp742,sizeof(void*),0U));});});else{
if(_tmp741 != 0  && !_tmp7D4)
({void*_tmp744=0U;({unsigned int _tmpCEE=loc;struct _dyneither_ptr _tmpCED=({const char*_tmp745="format descriptor is not a char ? type";_tag_dyneither(_tmp745,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCEE,_tmpCED,_tag_dyneither(_tmp744,sizeof(void*),0U));});});}}
# 4457
goto _LL57;}else{_LL5A: _LL5B:
({void*_tmp746=0U;({unsigned int _tmpCF0=loc;struct _dyneither_ptr _tmpCEF=({const char*_tmp747="format descriptor is not a string type";_tag_dyneither(_tmp747,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpCF0,_tmpCEF,_tag_dyneither(_tmp746,sizeof(void*),0U));});});goto _LL57;}_LL57:;}
# 4460
if(fmt_arg_start != 0  && !_tmp7D4){
# 4464
int problem;
{struct _tuple33 _tmp74B=({struct _tuple33 _tmpA01;_tmpA01.f1=ft,({void*_tmpCF1=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp7D3))->type));_tmpA01.f2=_tmpCF1;});_tmpA01;});struct _tuple33 _tmp74C=_tmp74B;struct Cyc_Absyn_Datatypedecl*_tmp74E;struct Cyc_Absyn_Datatypedecl*_tmp74D;if(_tmp74C.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5D: _tmp74D=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->f1)->f1).KnownDatatype).val;_LL5E:
# 4467
({int _tmpCF2=Cyc_Absyn_qvar_cmp(_tmp74D->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;problem=_tmpCF2;});goto _LL5C;}else{goto _LL61;}}else{goto _LL61;}}else{goto _LL61;}}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->f1)->f1).KnownDatatype).tag == 2){_LL5F: _tmp74E=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp74C.f2)->f1)->f1).KnownDatatype).val;_LL60:
# 4469
({int _tmpCF3=Cyc_Absyn_qvar_cmp(_tmp74E->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;problem=_tmpCF3;});goto _LL5C;}else{goto _LL61;}}else{goto _LL61;}}else{_LL61: _LL62:
# 4471
 problem=1;goto _LL5C;}}_LL5C:;}
# 4473
if(problem)
({void*_tmp74F=0U;({unsigned int _tmpCF5=loc;struct _dyneither_ptr _tmpCF4=({const char*_tmp750="format attribute and vararg types don't match";_tag_dyneither(_tmp750,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpCF5,_tmpCF4,_tag_dyneither(_tmp74F,sizeof(void*),0U));});});}}}
# 4481
{struct Cyc_List_List*rpo=_tmp7D2;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp752=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp753=_tmp752;void*_tmp755;void*_tmp754;_LL64: _tmp755=_tmp753->f1;_tmp754=_tmp753->f2;_LL65:;
({struct Cyc_Tcutil_CVTEnv _tmpCF6=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71E,& Cyc_Tcutil_ek,_tmp755,1,1);_tmp71E=_tmpCF6;});
({struct Cyc_Tcutil_CVTEnv _tmpCF7=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71E,& Cyc_Tcutil_trk,_tmp754,1,1);_tmp71E=_tmpCF7;});}}{
# 4489
struct Cyc_Tcenv_Fenv*_tmp756=Cyc_Tcenv_bogus_fenv(_tmp7D6,_tmp7D5);
struct Cyc_Tcenv_Tenv*_tmp757=({struct Cyc_Tcenv_Tenv*_tmp795=_cycalloc(sizeof(*_tmp795));_tmp795->ns=te->ns,_tmp795->ae=te->ae,_tmp795->le=_tmp756,_tmp795->allow_valueof=1,_tmp795->in_extern_c_include=te->in_extern_c_include;_tmp795;});
struct _tuple32 _tmp758=({unsigned int _tmpCFB=loc;struct Cyc_Tcenv_Tenv*_tmpCFA=_tmp757;struct Cyc_Tcutil_CVTEnv _tmpCF9=_tmp71E;struct _dyneither_ptr _tmpCF8=({const char*_tmp794="@requires";_tag_dyneither(_tmp794,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpCFB,_tmpCFA,_tmpCF9,_tmpCF8,_tmp7D0);});struct _tuple32 _tmp759=_tmp758;struct Cyc_Tcutil_CVTEnv _tmp793;struct Cyc_List_List*_tmp792;_LL67: _tmp793=_tmp759.f1;_tmp792=_tmp759.f2;_LL68:;
_tmp71E=_tmp793;
*_tmp7CF=_tmp792;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D5,_tmp792);{
# 4502
struct _tuple32 _tmp75A=({unsigned int _tmpCFF=loc;struct Cyc_Tcenv_Tenv*_tmpCFE=_tmp757;struct Cyc_Tcutil_CVTEnv _tmpCFD=_tmp71E;struct _dyneither_ptr _tmpCFC=({const char*_tmp791="@ensures";_tag_dyneither(_tmp791,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpCFF,_tmpCFE,_tmpCFD,_tmpCFC,_tmp7CE);});struct _tuple32 _tmp75B=_tmp75A;struct Cyc_Tcutil_CVTEnv _tmp790;struct Cyc_List_List*_tmp78F;_LL6A: _tmp790=_tmp75B.f1;_tmp78F=_tmp75B.f2;_LL6B:;
_tmp71E=_tmp790;
*_tmp7CD=_tmp78F;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp7D5,_tmp78F);
# 4507
if(*_tmp7D8 != 0)
({struct Cyc_Tcutil_CVTEnv _tmpD00=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp71E,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp7D8),1,1);_tmp71E=_tmpD00;});else{
# 4510
struct Cyc_List_List*effect=0;
# 4515
{struct Cyc_List_List*tvs=_tmp71E.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple30 _tmp75C=*((struct _tuple30*)tvs->hd);struct _tuple30 _tmp75D=_tmp75C;struct Cyc_Absyn_Tvar*_tmp76B;int _tmp76A;_LL6D: _tmp76B=_tmp75D.f1;_tmp76A=_tmp75D.f2;_LL6E:;
if(!_tmp76A)continue;{
void*_tmp75E=Cyc_Absyn_compress_kb(_tmp76B->kind);void*_tmp75F=_tmp75E;struct Cyc_Core_Opt**_tmp769;struct Cyc_Absyn_Kind*_tmp768;struct Cyc_Core_Opt**_tmp767;struct Cyc_Core_Opt**_tmp766;struct Cyc_Absyn_Kind*_tmp765;switch(*((int*)_tmp75F)){case 2U: _LL70: _tmp766=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75F)->f1;_tmp765=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75F)->f2;if(_tmp765->kind == Cyc_Absyn_RgnKind){_LL71:
# 4521
 if(_tmp765->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpD01=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp766=_tmpD01;});_tmp768=_tmp765;goto _LL73;}
# 4524
({struct Cyc_Core_Opt*_tmpD02=Cyc_Tcutil_kind_to_bound_opt(_tmp765);*_tmp766=_tmpD02;});_tmp768=_tmp765;goto _LL73;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75F)->f2)->kind){case Cyc_Absyn_BoolKind: _LL74: _LL75:
# 4527
 goto _LL77;case Cyc_Absyn_PtrBndKind: _LL76: _LL77:
 goto _LL79;case Cyc_Absyn_IntKind: _LL78: _LL79:
 goto _LL7B;case Cyc_Absyn_EffKind: _LL80: _tmp767=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75F)->f1;_LL81:
# 4534
({struct Cyc_Core_Opt*_tmpD03=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp767=_tmpD03;});goto _LL83;default: goto _LL86;}}case 0U: _LL72: _tmp768=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75F)->f1;if(_tmp768->kind == Cyc_Absyn_RgnKind){_LL73:
# 4526
({struct Cyc_List_List*_tmpD05=({struct Cyc_List_List*_tmp760=_cycalloc(sizeof(*_tmp760));({void*_tmpD04=Cyc_Absyn_access_eff(Cyc_Absyn_var_type(_tmp76B));_tmp760->hd=_tmpD04;}),_tmp760->tl=effect;_tmp760;});effect=_tmpD05;});goto _LL6F;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75F)->f1)->kind){case Cyc_Absyn_BoolKind: _LL7A: _LL7B:
# 4530
 goto _LL7D;case Cyc_Absyn_PtrBndKind: _LL7C: _LL7D:
 goto _LL7F;case Cyc_Absyn_IntKind: _LL7E: _LL7F:
 goto _LL6F;case Cyc_Absyn_EffKind: _LL82: _LL83:
# 4536
({struct Cyc_List_List*_tmpD07=({struct Cyc_List_List*_tmp761=_cycalloc(sizeof(*_tmp761));({void*_tmpD06=Cyc_Absyn_var_type(_tmp76B);_tmp761->hd=_tmpD06;}),_tmp761->tl=effect;_tmp761;});effect=_tmpD07;});goto _LL6F;default: _LL86: _LL87:
# 4541
({struct Cyc_List_List*_tmpD09=({struct Cyc_List_List*_tmp764=_cycalloc(sizeof(*_tmp764));({void*_tmpD08=Cyc_Absyn_regionsof_eff(Cyc_Absyn_var_type(_tmp76B));_tmp764->hd=_tmpD08;}),_tmp764->tl=effect;_tmp764;});effect=_tmpD09;});goto _LL6F;}}default: _LL84: _tmp769=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp75F)->f1;_LL85:
# 4538
({struct Cyc_Core_Opt*_tmpD0B=({struct Cyc_Core_Opt*_tmp763=_cycalloc(sizeof(*_tmp763));({void*_tmpD0A=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp762=_cycalloc(sizeof(*_tmp762));_tmp762->tag=2U,_tmp762->f1=0,_tmp762->f2=& Cyc_Tcutil_ak;_tmp762;});_tmp763->v=_tmpD0A;});_tmp763;});*_tmp769=_tmpD0B;});goto _LL87;}_LL6F:;};}}
# 4545
{struct Cyc_List_List*ts=_tmp71E.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple31 _tmp76C=*((struct _tuple31*)ts->hd);struct _tuple31 _tmp76D=_tmp76C;void*_tmp774;int _tmp773;_LL89: _tmp774=_tmp76D.f1;_tmp773=_tmp76D.f2;_LL8A:;
if(!_tmp773)continue;{
struct Cyc_Absyn_Kind*_tmp76E=Cyc_Tcutil_type_kind(_tmp774);struct Cyc_Absyn_Kind*_tmp76F=_tmp76E;switch(((struct Cyc_Absyn_Kind*)_tmp76F)->kind){case Cyc_Absyn_RgnKind: _LL8C: _LL8D:
# 4550
({struct Cyc_List_List*_tmpD0D=({struct Cyc_List_List*_tmp770=_cycalloc(sizeof(*_tmp770));({void*_tmpD0C=Cyc_Absyn_access_eff(_tmp774);_tmp770->hd=_tmpD0C;}),_tmp770->tl=effect;_tmp770;});effect=_tmpD0D;});goto _LL8B;case Cyc_Absyn_EffKind: _LL8E: _LL8F:
# 4552
({struct Cyc_List_List*_tmpD0E=({struct Cyc_List_List*_tmp771=_cycalloc(sizeof(*_tmp771));_tmp771->hd=_tmp774,_tmp771->tl=effect;_tmp771;});effect=_tmpD0E;});goto _LL8B;case Cyc_Absyn_IntKind: _LL90: _LL91:
 goto _LL8B;default: _LL92: _LL93:
# 4555
({struct Cyc_List_List*_tmpD10=({struct Cyc_List_List*_tmp772=_cycalloc(sizeof(*_tmp772));({void*_tmpD0F=Cyc_Absyn_regionsof_eff(_tmp774);_tmp772->hd=_tmpD0F;}),_tmp772->tl=effect;_tmp772;});effect=_tmpD10;});goto _LL8B;}_LL8B:;};}}
# 4558
({void*_tmpD11=Cyc_Absyn_join_eff(effect);*_tmp7D8=_tmpD11;});}
# 4564
if(*_tmp7D9 != 0){
struct Cyc_List_List*bs=*_tmp7D9;for(0;bs != 0;bs=bs->tl){
void*_tmp775=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp776=_tmp775;struct Cyc_Core_Opt**_tmp786;struct Cyc_Absyn_Kind*_tmp785;struct Cyc_Core_Opt**_tmp784;struct Cyc_Core_Opt**_tmp783;struct Cyc_Core_Opt**_tmp782;struct Cyc_Core_Opt**_tmp781;struct Cyc_Core_Opt**_tmp780;struct Cyc_Core_Opt**_tmp77F;struct Cyc_Core_Opt**_tmp77E;struct Cyc_Core_Opt**_tmp77D;struct Cyc_Core_Opt**_tmp77C;switch(*((int*)_tmp776)){case 1U: _LL95: _tmp77C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LL96:
# 4568
({struct Cyc_String_pa_PrintArg_struct _tmp779=({struct Cyc_String_pa_PrintArg_struct _tmpA02;_tmpA02.tag=0U,_tmpA02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);_tmpA02;});void*_tmp777[1U];_tmp777[0]=& _tmp779;({unsigned int _tmpD13=loc;struct _dyneither_ptr _tmpD12=({const char*_tmp778="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp778,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpD13,_tmpD12,_tag_dyneither(_tmp777,sizeof(void*),1U));});});
# 4570
_tmp77D=_tmp77C;goto _LL98;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f2)->aliasqual == Cyc_Absyn_Top){_LL97: _tmp77D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LL98:
 _tmp77E=_tmp77D;goto _LL9A;}else{goto _LLA7;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f2)->aliasqual){case Cyc_Absyn_Top: _LL99: _tmp77E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LL9A:
 _tmp77F=_tmp77E;goto _LL9C;case Cyc_Absyn_Aliasable: _LL9B: _tmp77F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LL9C:
 _tmp781=_tmp77F;goto _LL9E;default: _LLA1: _tmp780=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LLA2:
# 4577
 _tmp783=_tmp780;goto _LLA4;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f2)->aliasqual){case Cyc_Absyn_Top: _LL9D: _tmp781=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LL9E:
# 4574
 _tmp782=_tmp781;goto _LLA0;case Cyc_Absyn_Aliasable: _LL9F: _tmp782=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LLA0:
# 4576
({struct Cyc_Core_Opt*_tmpD14=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp782=_tmpD14;});goto _LL94;default: _LLA3: _tmp783=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LLA4:
# 4579
({struct Cyc_Core_Opt*_tmpD15=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp783=_tmpD15;});goto _LL94;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f2)->aliasqual == Cyc_Absyn_Top){_LLA5: _tmp784=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_LLA6:
# 4581
({struct Cyc_Core_Opt*_tmpD16=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp784=_tmpD16;});goto _LL94;}else{goto _LLA7;}default: _LLA7: _tmp786=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f1;_tmp785=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp776)->f2;_LLA8:
# 4583
({struct Cyc_Core_Opt*_tmpD17=Cyc_Tcutil_kind_to_bound_opt(_tmp785);*_tmp786=_tmpD17;});goto _LL94;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp776)->f1)->kind == Cyc_Absyn_MemKind){_LLA9: _LLAA:
# 4585
({void*_tmp77A=0U;({unsigned int _tmpD19=loc;struct _dyneither_ptr _tmpD18=({const char*_tmp77B="functions can't abstract M types";_tag_dyneither(_tmp77B,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpD19,_tmpD18,_tag_dyneither(_tmp77A,sizeof(void*),0U));});});goto _LL94;}else{_LLAB: _LLAC:
 goto _LL94;}}_LL94:;}}
# 4590
({struct Cyc_List_List*_tmpD1A=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp71E.kind_env,*_tmp7D9);cvtenv.kind_env=_tmpD1A;});
({struct Cyc_List_List*_tmpD1B=Cyc_Tcutil_remove_bound_tvars_bool(_tmp71E.r,_tmp71E.free_vars,*_tmp7D9);_tmp71E.free_vars=_tmpD1B;});
# 4593
{struct Cyc_List_List*tvs=_tmp71E.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple30 _tmp787=*((struct _tuple30*)tvs->hd);struct _tuple30 _tmp788=_tmp787;struct Cyc_Absyn_Tvar*_tmp78A;int _tmp789;_LLAE: _tmp78A=_tmp788.f1;_tmp789=_tmp788.f2;_LLAF:;
({struct Cyc_List_List*_tmpD1C=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp78A,_tmp789);cvtenv.free_vars=_tmpD1C;});}}
# 4598
{struct Cyc_List_List*evs=_tmp71E.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple31 _tmp78B=*((struct _tuple31*)evs->hd);struct _tuple31 _tmp78C=_tmp78B;void*_tmp78E;int _tmp78D;_LLB1: _tmp78E=_tmp78C.f1;_tmp78D=_tmp78C.f2;_LLB2:;
({struct Cyc_List_List*_tmpD1D=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp78E,_tmp78D);cvtenv.free_evars=_tmpD1D;});}}
# 4602
goto _LL0;};};};}case 6U: _LL13: _tmp7DA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6DC)->f1;_LL14:
# 4605
 for(0;_tmp7DA != 0;_tmp7DA=_tmp7DA->tl){
struct _tuple12*_tmp796=(struct _tuple12*)_tmp7DA->hd;
({struct Cyc_Tcutil_CVTEnv _tmpD1E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp796).f2,1,0);cvtenv=_tmpD1E;});
({int _tmpD1F=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp796).f1).print_const,(*_tmp796).f2);
# 4608
((*_tmp796).f1).real_const=_tmpD1F;});}
# 4611
goto _LL0;case 7U: _LL15: _tmp7DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DC)->f1;_tmp7DB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6DC)->f2;_LL16: {
# 4615
struct Cyc_List_List*prev_fields=0;
for(0;_tmp7DB != 0;_tmp7DB=_tmp7DB->tl){
struct Cyc_Absyn_Aggrfield*_tmp797=(struct Cyc_Absyn_Aggrfield*)_tmp7DB->hd;struct Cyc_Absyn_Aggrfield*_tmp798=_tmp797;struct _dyneither_ptr*_tmp7A8;struct Cyc_Absyn_Tqual*_tmp7A7;void*_tmp7A6;struct Cyc_Absyn_Exp*_tmp7A5;struct Cyc_List_List*_tmp7A4;struct Cyc_Absyn_Exp*_tmp7A3;_LLB4: _tmp7A8=_tmp798->name;_tmp7A7=(struct Cyc_Absyn_Tqual*)& _tmp798->tq;_tmp7A6=_tmp798->type;_tmp7A5=_tmp798->width;_tmp7A4=_tmp798->attributes;_tmp7A3=_tmp798->requires_clause;_LLB5:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp7A8))
({struct Cyc_String_pa_PrintArg_struct _tmp79B=({struct Cyc_String_pa_PrintArg_struct _tmpA04;_tmpA04.tag=0U,_tmpA04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp7A8);_tmpA04;});void*_tmp799[1U];_tmp799[0]=& _tmp79B;({unsigned int _tmpD21=loc;struct _dyneither_ptr _tmpD20=({const char*_tmp79A="duplicate field %s";_tag_dyneither(_tmp79A,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpD21,_tmpD20,_tag_dyneither(_tmp799,sizeof(void*),1U));});});
if(({struct _dyneither_ptr _tmpD22=(struct _dyneither_ptr)*_tmp7A8;Cyc_strcmp(_tmpD22,({const char*_tmp79C="";_tag_dyneither(_tmp79C,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpD23=({struct Cyc_List_List*_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D->hd=_tmp7A8,_tmp79D->tl=prev_fields;_tmp79D;});prev_fields=_tmpD23;});
({struct Cyc_Tcutil_CVTEnv _tmpD24=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp7A6,1,0);cvtenv=_tmpD24;});
({int _tmpD25=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp7A7->print_const,_tmp7A6);_tmp7A7->real_const=_tmpD25;});
Cyc_Tcutil_check_bitfield(loc,te,_tmp7A6,_tmp7A5,_tmp7A8);
Cyc_Tcutil_check_field_atts(loc,_tmp7A8,_tmp7A4);
if(_tmp7A3 != 0){
# 4628
if(_tmp7DC != Cyc_Absyn_UnionA)
({void*_tmp79E=0U;({unsigned int _tmpD27=loc;struct _dyneither_ptr _tmpD26=({const char*_tmp79F="@requires clause is only allowed on union members";_tag_dyneither(_tmp79F,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpD27,_tmpD26,_tag_dyneither(_tmp79E,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpD28=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpD28,0,_tmp7A3);});
if(!Cyc_Tcutil_is_integral(_tmp7A3))
({struct Cyc_String_pa_PrintArg_struct _tmp7A2=({struct Cyc_String_pa_PrintArg_struct _tmpA05;_tmpA05.tag=0U,({
struct _dyneither_ptr _tmpD29=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp7A3->topt)));_tmpA05.f1=_tmpD29;});_tmpA05;});void*_tmp7A0[1U];_tmp7A0[0]=& _tmp7A2;({unsigned int _tmpD2B=loc;struct _dyneither_ptr _tmpD2A=({const char*_tmp7A1="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp7A1,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpD2B,_tmpD2A,_tag_dyneither(_tmp7A0,sizeof(void*),1U));});});
({struct Cyc_Tcutil_CVTEnv _tmpD2C=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7A3,te,cvtenv);cvtenv=_tmpD2C;});}}
# 4637
goto _LL0;}default: _LL17: _tmp7E0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DC)->f1;_tmp7DF=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DC)->f2;_tmp7DE=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DC)->f3;_tmp7DD=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6DC)->f4;_LL18: {
# 4640
struct Cyc_List_List*targs=*_tmp7DF;
# 4642
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp7A9;_push_handler(& _tmp7A9);{int _tmp7AB=0;if(setjmp(_tmp7A9.handler))_tmp7AB=1;if(!_tmp7AB){({struct Cyc_Absyn_Typedefdecl*_tmpD2D=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp7E0);td=_tmpD2D;});;_pop_handler();}else{void*_tmp7AA=(void*)_exn_thrown;void*_tmp7AC=_tmp7AA;void*_tmp7B0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7AC)->tag == Cyc_Dict_Absent){_LLB7: _LLB8:
# 4645
({struct Cyc_String_pa_PrintArg_struct _tmp7AF=({struct Cyc_String_pa_PrintArg_struct _tmpA06;_tmpA06.tag=0U,({struct _dyneither_ptr _tmpD2E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E0));_tmpA06.f1=_tmpD2E;});_tmpA06;});void*_tmp7AD[1U];_tmp7AD[0]=& _tmp7AF;({unsigned int _tmpD30=loc;struct _dyneither_ptr _tmpD2F=({const char*_tmp7AE="unbound typedef name %s";_tag_dyneither(_tmp7AE,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpD30,_tmpD2F,_tag_dyneither(_tmp7AD,sizeof(void*),1U));});});
return cvtenv;}else{_LLB9: _tmp7B0=_tmp7AC;_LLBA:(int)_rethrow(_tmp7B0);}_LLB6:;}};}
# 4648
*_tmp7DE=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4653
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4658
({struct Cyc_Tcutil_CVTEnv _tmpD31=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpD31;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpD33=({struct Cyc_List_List*_tmp7B2=_cycalloc(sizeof(*_tmp7B2));({struct _tuple15*_tmpD32=({struct _tuple15*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7B1->f2=(void*)ts->hd;_tmp7B1;});_tmp7B2->hd=_tmpD32;}),_tmp7B2->tl=inst;_tmp7B2;});inst=_tmpD33;});}
# 4662
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B5=({struct Cyc_String_pa_PrintArg_struct _tmpA07;_tmpA07.tag=0U,({struct _dyneither_ptr _tmpD34=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E0));_tmpA07.f1=_tmpD34;});_tmpA07;});void*_tmp7B3[1U];_tmp7B3[0]=& _tmp7B5;({unsigned int _tmpD36=loc;struct _dyneither_ptr _tmpD35=({const char*_tmp7B4="too many parameters for typedef %s";_tag_dyneither(_tmp7B4,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpD36,_tmpD35,_tag_dyneither(_tmp7B3,sizeof(void*),1U));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4667
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpD37=({struct Cyc_List_List*_tmp7B6=_cycalloc(sizeof(*_tmp7B6));_tmp7B6->hd=e,_tmp7B6->tl=hidden_ts;_tmp7B6;});hidden_ts=_tmpD37;});
({struct Cyc_Tcutil_CVTEnv _tmpD38=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpD38;});
({struct Cyc_List_List*_tmpD3A=({struct Cyc_List_List*_tmp7B8=_cycalloc(sizeof(*_tmp7B8));({struct _tuple15*_tmpD39=({struct _tuple15*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->f1=(struct Cyc_Absyn_Tvar*)tvs->hd,_tmp7B7->f2=e;_tmp7B7;});_tmp7B8->hd=_tmpD39;}),_tmp7B8->tl=inst;_tmp7B8;});inst=_tmpD3A;});}
# 4675
({struct Cyc_List_List*_tmpD3C=({struct Cyc_List_List*_tmpD3B=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpD3B,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp7DF=_tmpD3C;});}
# 4677
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp7DD=new_typ;}
# 4683
goto _LL0;};}}_LL0:;}
# 4685
if(!({struct Cyc_Absyn_Kind*_tmpD3D=Cyc_Tcutil_type_kind(t);Cyc_Tcutil_kind_leq(_tmpD3D,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp7E3=({struct Cyc_String_pa_PrintArg_struct _tmpA0A;_tmpA0A.tag=0U,({
struct _dyneither_ptr _tmpD3E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA0A.f1=_tmpD3E;});_tmpA0A;});struct Cyc_String_pa_PrintArg_struct _tmp7E4=({struct Cyc_String_pa_PrintArg_struct _tmpA09;_tmpA09.tag=0U,({struct _dyneither_ptr _tmpD3F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_type_kind(t)));_tmpA09.f1=_tmpD3F;});_tmpA09;});struct Cyc_String_pa_PrintArg_struct _tmp7E5=({struct Cyc_String_pa_PrintArg_struct _tmpA08;_tmpA08.tag=0U,({struct _dyneither_ptr _tmpD40=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(expected_kind));_tmpA08.f1=_tmpD40;});_tmpA08;});void*_tmp7E1[3U];_tmp7E1[0]=& _tmp7E3,_tmp7E1[1]=& _tmp7E4,_tmp7E1[2]=& _tmp7E5;({unsigned int _tmpD42=loc;struct _dyneither_ptr _tmpD41=({const char*_tmp7E2="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp7E2,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpD42,_tmpD41,_tag_dyneither(_tmp7E1,sizeof(void*),3U));});});
# 4689
return cvtenv;}
# 4697
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4699
{void*_tmp7E6=e->r;void*_tmp7E7=_tmp7E6;struct Cyc_Absyn_Exp*_tmp7FA;struct Cyc_Absyn_Exp*_tmp7F9;void*_tmp7F8;void*_tmp7F7;void*_tmp7F6;void*_tmp7F5;struct Cyc_Absyn_Exp*_tmp7F4;struct Cyc_Absyn_Exp*_tmp7F3;struct Cyc_Absyn_Exp*_tmp7F2;struct Cyc_Absyn_Exp*_tmp7F1;struct Cyc_Absyn_Exp*_tmp7F0;struct Cyc_Absyn_Exp*_tmp7EF;struct Cyc_Absyn_Exp*_tmp7EE;struct Cyc_Absyn_Exp*_tmp7ED;struct Cyc_Absyn_Exp*_tmp7EC;struct Cyc_Absyn_Exp*_tmp7EB;struct Cyc_List_List*_tmp7EA;switch(*((int*)_tmp7E7)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp7EA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7E7)->f2;_LLC:
# 4706
 for(0;_tmp7EA != 0;_tmp7EA=_tmp7EA->tl){
({struct Cyc_Tcutil_CVTEnv _tmpD43=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp7EA->hd,te,cvtenv);cvtenv=_tmpD43;});}
goto _LL0;case 6U: _LLD: _tmp7ED=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_tmp7EC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E7)->f2;_tmp7EB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7E7)->f3;_LLE:
# 4710
({struct Cyc_Tcutil_CVTEnv _tmpD44=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7ED,te,cvtenv);cvtenv=_tmpD44;});
({struct Cyc_Tcutil_CVTEnv _tmpD45=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EC,te,cvtenv);cvtenv=_tmpD45;});
({struct Cyc_Tcutil_CVTEnv _tmpD46=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7EB,te,cvtenv);cvtenv=_tmpD46;});
goto _LL0;case 7U: _LLF: _tmp7EF=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_tmp7EE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7E7)->f2;_LL10:
 _tmp7F1=_tmp7EF;_tmp7F0=_tmp7EE;goto _LL12;case 8U: _LL11: _tmp7F1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_tmp7F0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7E7)->f2;_LL12:
 _tmp7F3=_tmp7F1;_tmp7F2=_tmp7F0;goto _LL14;case 9U: _LL13: _tmp7F3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_tmp7F2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7E7)->f2;_LL14:
# 4717
({struct Cyc_Tcutil_CVTEnv _tmpD47=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F3,te,cvtenv);cvtenv=_tmpD47;});
({struct Cyc_Tcutil_CVTEnv _tmpD48=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F2,te,cvtenv);cvtenv=_tmpD48;});
goto _LL0;case 14U: _LL15: _tmp7F5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_tmp7F4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7E7)->f2;_LL16:
# 4721
({struct Cyc_Tcutil_CVTEnv _tmpD49=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F4,te,cvtenv);cvtenv=_tmpD49;});
({struct Cyc_Tcutil_CVTEnv _tmpD4A=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F5,1,0);cvtenv=_tmpD4A;});
goto _LL0;case 19U: _LL17: _tmp7F6=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_LL18:
 _tmp7F7=_tmp7F6;goto _LL1A;case 17U: _LL19: _tmp7F7=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_LL1A:
# 4726
({struct Cyc_Tcutil_CVTEnv _tmpD4B=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp7F7,1,0);cvtenv=_tmpD4B;});
goto _LL0;case 39U: _LL1B: _tmp7F8=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_LL1C:
# 4729
({struct Cyc_Tcutil_CVTEnv _tmpD4C=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp7F8,1,0);cvtenv=_tmpD4C;});
goto _LL0;case 18U: _LL1D: _tmp7F9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_LL1E:
# 4732
({struct Cyc_Tcutil_CVTEnv _tmpD4D=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7F9,te,cvtenv);cvtenv=_tmpD4D;});
goto _LL0;case 41U: _LL1F: _tmp7FA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp7E7)->f1;_LL20:
# 4735
({struct Cyc_Tcutil_CVTEnv _tmpD4E=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7FA,te,cvtenv);cvtenv=_tmpD4E;});
goto _LL0;default: _LL21: _LL22:
# 4738
({void*_tmp7E8=0U;({struct _dyneither_ptr _tmpD4F=({const char*_tmp7E9="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp7E9,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD4F,_tag_dyneither(_tmp7E8,sizeof(void*),0U));});});}_LL0:;}
# 4740
return cvtenv;}
# 4743
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4748
struct Cyc_List_List*_tmp7FB=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpD50=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpD50;});
# 4751
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple30 _tmp7FC=*((struct _tuple30*)vs->hd);struct _tuple30 _tmp7FD=_tmp7FC;struct Cyc_Absyn_Tvar*_tmp7FE;_LL1: _tmp7FE=_tmp7FD.f1;_LL2:;
({struct Cyc_List_List*_tmpD51=Cyc_Tcutil_fast_add_free_tvar(_tmp7FB,_tmp7FE);cvt.kind_env=_tmpD51;});}}
# 4759
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple31 _tmp7FF=*((struct _tuple31*)evs->hd);struct _tuple31 _tmp800=_tmp7FF;void*_tmp809;_LL4: _tmp809=_tmp800.f1;_LL5:;{
void*_tmp801=Cyc_Tcutil_compress(_tmp809);void*_tmp802=_tmp801;struct Cyc_Core_Opt**_tmp808;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp802)->tag == 1U){_LL7: _tmp808=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp802)->f4;_LL8:
# 4763
 if(*_tmp808 == 0)
({struct Cyc_Core_Opt*_tmpD52=({struct Cyc_Core_Opt*_tmp803=_cycalloc(sizeof(*_tmp803));_tmp803->v=_tmp7FB;_tmp803;});*_tmp808=_tmpD52;});else{
# 4767
struct Cyc_List_List*_tmp804=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp808))->v;
struct Cyc_List_List*_tmp805=0;
for(0;_tmp804 != 0;_tmp804=_tmp804->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp7FB,(struct Cyc_Absyn_Tvar*)_tmp804->hd))
({struct Cyc_List_List*_tmpD53=({struct Cyc_List_List*_tmp806=_cycalloc(sizeof(*_tmp806));_tmp806->hd=(struct Cyc_Absyn_Tvar*)_tmp804->hd,_tmp806->tl=_tmp805;_tmp806;});_tmp805=_tmpD53;});}
({struct Cyc_Core_Opt*_tmpD54=({struct Cyc_Core_Opt*_tmp807=_cycalloc(sizeof(*_tmp807));_tmp807->v=_tmp805;_tmp807;});*_tmp808=_tmpD54;});}
# 4774
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4778
return cvt;}
# 4785
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
{void*_tmp80A=Cyc_Tcutil_compress(e);void*_tmp80B=_tmp80A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp80B)->tag == 1U){_LL1: _LL2:
 goto _LL0;}else{_LL3: _LL4:
# 4791
 continue;}_LL0:;}{
# 4793
struct Cyc_Absyn_Kind*_tmp80C=Cyc_Tcutil_type_kind(e);struct Cyc_Absyn_Kind*_tmp80D=_tmp80C;switch(((struct Cyc_Absyn_Kind*)_tmp80D)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp80D)->aliasqual){case Cyc_Absyn_Unique: _LL6: _LL7:
# 4795
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_unique_rgn_type))
({void*_tmp80E=0U;({struct _dyneither_ptr _tmpD55=({const char*_tmp80F="can't unify evar with unique region!";_tag_dyneither(_tmp80F,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD55,_tag_dyneither(_tmp80E,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_Aliasable: _LL8: _LL9:
 goto _LLB;default: _LLA: _LLB:
# 4800
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_heap_rgn_type))({void*_tmp810=0U;({struct _dyneither_ptr _tmpD56=({const char*_tmp811="can't unify evar with heap!";_tag_dyneither(_tmp811,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD56,_tag_dyneither(_tmp810,sizeof(void*),0U));});});
goto _LL5;}case Cyc_Absyn_EffKind: _LLC: _LLD:
# 4803
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp812=0U;({struct _dyneither_ptr _tmpD57=({const char*_tmp813="can't unify evar with {}!";_tag_dyneither(_tmp813,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD57,_tag_dyneither(_tmp812,sizeof(void*),0U));});});
goto _LL5;case Cyc_Absyn_BoolKind: _LLE: _LLF:
# 4806
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_false_type))({struct Cyc_String_pa_PrintArg_struct _tmp816=({struct Cyc_String_pa_PrintArg_struct _tmpA0B;_tmpA0B.tag=0U,({
struct _dyneither_ptr _tmpD58=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmpA0B.f1=_tmpD58;});_tmpA0B;});void*_tmp814[1U];_tmp814[0]=& _tmp816;({struct _dyneither_ptr _tmpD59=({const char*_tmp815="can't unify evar %s with @false!";_tag_dyneither(_tmp815,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD59,_tag_dyneither(_tmp814,sizeof(void*),1U));});});
goto _LL5;case Cyc_Absyn_PtrBndKind: _LL10: _LL11:
# 4810
 if(!({void*_tmpD5A=e;Cyc_Tcutil_unify(_tmpD5A,Cyc_Absyn_bounds_one());}))({void*_tmp817=0U;({struct _dyneither_ptr _tmpD5B=({const char*_tmp818="can't unify evar with bounds_one!";_tag_dyneither(_tmp818,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD5B,_tag_dyneither(_tmp817,sizeof(void*),0U));});});
goto _LL5;default: _LL12: _LL13:
# 4813
({struct Cyc_String_pa_PrintArg_struct _tmp81B=({struct Cyc_String_pa_PrintArg_struct _tmpA0D;_tmpA0D.tag=0U,({
struct _dyneither_ptr _tmpD5C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmpA0D.f1=_tmpD5C;});_tmpA0D;});struct Cyc_String_pa_PrintArg_struct _tmp81C=({struct Cyc_String_pa_PrintArg_struct _tmpA0C;_tmpA0C.tag=0U,({struct _dyneither_ptr _tmpD5D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(in_t));_tmpA0C.f1=_tmpD5D;});_tmpA0C;});void*_tmp819[2U];_tmp819[0]=& _tmp81B,_tmp819[1]=& _tmp81C;({unsigned int _tmpD5F=loc;struct _dyneither_ptr _tmpD5E=({const char*_tmp81A="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp81A,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpD5F,_tmpD5E,_tag_dyneither(_tmp819,sizeof(void*),2U));});});
goto _LL5;}_LL5:;};}}
# 4825
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp81D=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp81E=({unsigned int _tmpD63=loc;struct Cyc_Tcenv_Tenv*_tmpD62=te;struct Cyc_Tcutil_CVTEnv _tmpD61=({struct Cyc_Tcutil_CVTEnv _tmpA11;_tmpA11.r=Cyc_Core_heap_region,_tmpA11.kind_env=_tmp81D,_tmpA11.free_vars=0,_tmpA11.free_evars=0,_tmpA11.generalize_evars=generalize_evars,_tmpA11.fn_result=0;_tmpA11;});struct Cyc_Absyn_Kind*_tmpD60=expected_kind;Cyc_Tcutil_check_valid_type(_tmpD63,_tmpD62,_tmpD61,_tmpD60,1,t);});
# 4833
struct Cyc_List_List*_tmp81F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple30*))Cyc_Core_fst,_tmp81E.free_vars);
struct Cyc_List_List*_tmp820=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple31*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple31*))Cyc_Core_fst,_tmp81E.free_evars);
# 4837
if(_tmp81D != 0){
struct Cyc_List_List*_tmp821=0;
{struct Cyc_List_List*_tmp822=_tmp81F;for(0;(unsigned int)_tmp822;_tmp822=_tmp822->tl){
struct Cyc_Absyn_Tvar*_tmp823=(struct Cyc_Absyn_Tvar*)_tmp822->hd;
int found=0;
{struct Cyc_List_List*_tmp824=_tmp81D;for(0;(unsigned int)_tmp824;_tmp824=_tmp824->tl){
if(Cyc_Absyn_tvar_cmp(_tmp823,(struct Cyc_Absyn_Tvar*)_tmp824->hd)== 0){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpD64=({struct Cyc_List_List*_tmp825=_cycalloc(sizeof(*_tmp825));_tmp825->hd=(struct Cyc_Absyn_Tvar*)_tmp822->hd,_tmp825->tl=_tmp821;_tmp825;});_tmp821=_tmpD64;});}}
# 4847
({struct Cyc_List_List*_tmpD65=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp821);_tmp81F=_tmpD65;});}
# 4853
{struct Cyc_List_List*x=_tmp81F;for(0;x != 0;x=x->tl){
void*_tmp826=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp827=_tmp826;enum Cyc_Absyn_AliasQual _tmp835;struct Cyc_Core_Opt**_tmp834;struct Cyc_Absyn_Kind*_tmp833;struct Cyc_Core_Opt**_tmp832;struct Cyc_Core_Opt**_tmp831;struct Cyc_Core_Opt**_tmp830;struct Cyc_Core_Opt**_tmp82F;struct Cyc_Core_Opt**_tmp82E;struct Cyc_Core_Opt**_tmp82D;switch(*((int*)_tmp827)){case 1U: _LL1: _tmp82D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp827)->f1;_LL2:
 _tmp82E=_tmp82D;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp82E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f1;_LL4:
 _tmp82F=_tmp82E;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp82F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f1;_LL6:
 _tmp830=_tmp82F;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp830=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f1;_LL8:
# 4859
({struct Cyc_Core_Opt*_tmpD66=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp830=_tmpD66;});goto _LL0;default: _LL9: _tmp831=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f1;_LLA:
# 4861
({struct Cyc_Core_Opt*_tmpD67=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp831=_tmpD67;});goto _LL0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp832=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f1;_LLC:
# 4863
({struct Cyc_Core_Opt*_tmpD68=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp832=_tmpD68;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp834=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f1;_tmp833=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp827)->f2;_LLE:
# 4865
({struct Cyc_Core_Opt*_tmpD69=Cyc_Tcutil_kind_to_bound_opt(_tmp833);*_tmp834=_tmpD69;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp827)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp835=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp827)->f1)->aliasqual;_LL10:
# 4867
({struct Cyc_String_pa_PrintArg_struct _tmp82A=({struct Cyc_String_pa_PrintArg_struct _tmpA0F;_tmpA0F.tag=0U,({
struct _dyneither_ptr _tmpD6A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmpA0F.f1=_tmpD6A;});_tmpA0F;});struct Cyc_String_pa_PrintArg_struct _tmp82B=({struct Cyc_String_pa_PrintArg_struct _tmpA0E;_tmpA0E.tag=0U,({struct _dyneither_ptr _tmpD6B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp82C=_cycalloc(sizeof(*_tmp82C));_tmp82C->kind=Cyc_Absyn_MemKind,_tmp82C->aliasqual=_tmp835;_tmp82C;})));_tmpA0E.f1=_tmpD6B;});_tmpA0E;});void*_tmp828[2U];_tmp828[0]=& _tmp82A,_tmp828[1]=& _tmp82B;({unsigned int _tmpD6D=loc;struct _dyneither_ptr _tmpD6C=({const char*_tmp829="type variable %s cannot have kind %s";_tag_dyneither(_tmp829,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpD6D,_tmpD6C,_tag_dyneither(_tmp828,sizeof(void*),2U));});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4874
if(_tmp81F != 0  || _tmp820 != 0){
{void*_tmp836=Cyc_Tcutil_compress(t);void*_tmp837=_tmp836;struct Cyc_List_List**_tmp838;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp837)->tag == 5U){_LL14: _tmp838=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp837)->f1).tvars;_LL15:
# 4877
 if(*_tmp838 == 0){
# 4879
({struct Cyc_List_List*_tmpD6E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp81F);*_tmp838=_tmpD6E;});
_tmp81F=0;}
# 4882
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4885
if(_tmp81F != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp83B=({struct Cyc_String_pa_PrintArg_struct _tmpA10;_tmpA10.tag=0U,_tmpA10.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp81F->hd)->name);_tmpA10;});void*_tmp839[1U];_tmp839[0]=& _tmp83B;({unsigned int _tmpD70=loc;struct _dyneither_ptr _tmpD6F=({const char*_tmp83A="unbound type variable %s";_tag_dyneither(_tmp83A,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpD70,_tmpD6F,_tag_dyneither(_tmp839,sizeof(void*),1U));});});
Cyc_Tcutil_check_free_evars(_tmp820,t,loc);}}
# 4895
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2type(fd);
# 4898
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp83C=Cyc_Tcutil_compress(t);void*_tmp83D=_tmp83C;struct Cyc_List_List*_tmp849;void*_tmp848;struct Cyc_Absyn_Tqual _tmp847;void*_tmp846;struct Cyc_List_List*_tmp845;struct Cyc_Absyn_Exp*_tmp844;struct Cyc_List_List*_tmp843;struct Cyc_Absyn_Exp*_tmp842;struct Cyc_List_List*_tmp841;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->tag == 5U){_LL1: _tmp849=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).tvars;_tmp848=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).effect;_tmp847=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).ret_tqual;_tmp846=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).ret_type;_tmp845=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).args;_tmp844=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).requires_clause;_tmp843=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).requires_relns;_tmp842=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).ensures_clause;_tmp841=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp83D)->f1).ensures_relns;_LL2:
# 4901
 fd->tvs=_tmp849;
fd->effect=_tmp848;
{struct Cyc_List_List*_tmp83E=fd->args;for(0;_tmp83E != 0;(_tmp83E=_tmp83E->tl,_tmp845=_tmp845->tl)){
# 4905
(*((struct _tuple10*)_tmp83E->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp845))->hd)).f2;
(*((struct _tuple10*)_tmp83E->hd)).f3=(*((struct _tuple10*)_tmp845->hd)).f3;}}
# 4908
fd->ret_tqual=_tmp847;
fd->ret_type=_tmp846;
({int _tmpD71=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp846);(fd->ret_tqual).real_const=_tmpD71;});
fd->requires_clause=_tmp844;
fd->requires_relns=_tmp843;
fd->ensures_clause=_tmp842;
fd->ensures_relns=_tmp841;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp83F=0U;({struct _dyneither_ptr _tmpD72=({const char*_tmp840="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp840,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD72,_tag_dyneither(_tmp83F,sizeof(void*),0U));});});}_LL0:;}
# 4918
({struct Cyc_List_List*_tmpD74=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpD73=loc;Cyc_Tcutil_check_unique_vars(_tmpD74,_tmpD73,({const char*_tmp84A="function declaration has repeated parameter";_tag_dyneither(_tmp84A,sizeof(char),44U);}));});
# 4921
fd->cached_type=t;}
# 4926
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4932
struct Cyc_Tcutil_CVTEnv _tmp84B=({unsigned int _tmpD79=loc;struct Cyc_Tcenv_Tenv*_tmpD78=te;struct Cyc_Tcutil_CVTEnv _tmpD77=({struct Cyc_Tcutil_CVTEnv _tmpA14;_tmpA14.r=Cyc_Core_heap_region,_tmpA14.kind_env=bound_tvars,_tmpA14.free_vars=0,_tmpA14.free_evars=0,_tmpA14.generalize_evars=0,_tmpA14.fn_result=0;_tmpA14;});struct Cyc_Absyn_Kind*_tmpD76=expected_kind;int _tmpD75=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpD79,_tmpD78,_tmpD77,_tmpD76,_tmpD75,t);});
# 4936
struct Cyc_List_List*_tmp84C=({struct _RegionHandle*_tmpD7B=Cyc_Core_heap_region;struct Cyc_List_List*_tmpD7A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple30*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple30*))Cyc_Core_fst,_tmp84B.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpD7B,_tmpD7A,bound_tvars);});
# 4938
struct Cyc_List_List*_tmp84D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple31*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple31*))Cyc_Core_fst,_tmp84B.free_evars);
{struct Cyc_List_List*fs=_tmp84C;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp84E=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp851=({struct Cyc_String_pa_PrintArg_struct _tmpA13;_tmpA13.tag=0U,_tmpA13.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp84E);_tmpA13;});struct Cyc_String_pa_PrintArg_struct _tmp852=({struct Cyc_String_pa_PrintArg_struct _tmpA12;_tmpA12.tag=0U,({struct _dyneither_ptr _tmpD7C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA12.f1=_tmpD7C;});_tmpA12;});void*_tmp84F[2U];_tmp84F[0]=& _tmp851,_tmp84F[1]=& _tmp852;({unsigned int _tmpD7E=loc;struct _dyneither_ptr _tmpD7D=({const char*_tmp850="unbound type variable %s in type %s";_tag_dyneither(_tmp850,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpD7E,_tmpD7D,_tag_dyneither(_tmp84F,sizeof(void*),2U));});});}}
# 4943
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp84D,t,loc);}
# 4947
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpD7F=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpD7F;});}
# 4952
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4958
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4961
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp855=({struct Cyc_String_pa_PrintArg_struct _tmpA16;_tmpA16.tag=0U,_tmpA16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);_tmpA16;});struct Cyc_String_pa_PrintArg_struct _tmp856=({struct Cyc_String_pa_PrintArg_struct _tmpA15;_tmpA15.tag=0U,({struct _dyneither_ptr _tmpD80=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmpA15.f1=_tmpD80;});_tmpA15;});void*_tmp853[2U];_tmp853[0]=& _tmp855,_tmp853[1]=& _tmp856;({unsigned int _tmpD82=loc;struct _dyneither_ptr _tmpD81=({const char*_tmp854="%s: %s";_tag_dyneither(_tmp854,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpD82,_tmpD81,_tag_dyneither(_tmp853,sizeof(void*),2U));});});}}}
# 4967
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4971
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4975
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpD84=tvs;unsigned int _tmpD83=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpD84,_tmpD83,Cyc_Absynpp_tvar2string,({const char*_tmp857="duplicate type variable";_tag_dyneither(_tmp857,sizeof(char),24U);}));});}struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple35{struct Cyc_List_List*f1;void*f2;};struct _tuple36{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4989 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4994
struct _RegionHandle _tmp858=_new_region("temp");struct _RegionHandle*temp=& _tmp858;_push_region(temp);
# 4998
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpD85=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpD85,({const char*_tmp859="";_tag_dyneither(_tmp859,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpD87=({struct Cyc_List_List*_tmp85B=_region_malloc(temp,sizeof(*_tmp85B));({struct _tuple34*_tmpD86=({struct _tuple34*_tmp85A=_region_malloc(temp,sizeof(*_tmp85A));_tmp85A->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp85A->f2=0;_tmp85A;});_tmp85B->hd=_tmpD86;}),_tmp85B->tl=fields;_tmp85B;});fields=_tmpD87;});}}
# 5003
({struct Cyc_List_List*_tmpD88=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpD88;});{
# 5005
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp880="struct";_tag_dyneither(_tmp880,sizeof(char),7U);}):({const char*_tmp881="union";_tag_dyneither(_tmp881,sizeof(char),6U);});
# 5008
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple35*_tmp85C=(struct _tuple35*)des->hd;struct _tuple35*_tmp85D=_tmp85C;struct Cyc_List_List*_tmp878;void*_tmp877;_LL1: _tmp878=_tmp85D->f1;_tmp877=_tmp85D->f2;_LL2:;
if(_tmp878 == 0){
# 5013
struct Cyc_List_List*_tmp85E=fields;
for(0;_tmp85E != 0;_tmp85E=_tmp85E->tl){
if(!(*((struct _tuple34*)_tmp85E->hd)).f2){
(*((struct _tuple34*)_tmp85E->hd)).f2=1;
({struct Cyc_List_List*_tmpD8A=({struct Cyc_List_List*_tmp860=_cycalloc(sizeof(*_tmp860));({void*_tmpD89=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->tag=1U,_tmp85F->f1=((*((struct _tuple34*)_tmp85E->hd)).f1)->name;_tmp85F;});_tmp860->hd=_tmpD89;}),_tmp860->tl=0;_tmp860;});(*((struct _tuple35*)des->hd)).f1=_tmpD8A;});
({struct Cyc_List_List*_tmpD8C=({struct Cyc_List_List*_tmp862=_region_malloc(rgn,sizeof(*_tmp862));({struct _tuple36*_tmpD8B=({struct _tuple36*_tmp861=_region_malloc(rgn,sizeof(*_tmp861));_tmp861->f1=(*((struct _tuple34*)_tmp85E->hd)).f1,_tmp861->f2=_tmp877;_tmp861;});_tmp862->hd=_tmpD8B;}),_tmp862->tl=ans;_tmp862;});ans=_tmpD8C;});
break;}}
# 5021
if(_tmp85E == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp865=({struct Cyc_String_pa_PrintArg_struct _tmpA17;_tmpA17.tag=0U,_tmpA17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpA17;});void*_tmp863[1U];_tmp863[0]=& _tmp865;({unsigned int _tmpD8E=loc;struct _dyneither_ptr _tmpD8D=({const char*_tmp864="too many arguments to %s";_tag_dyneither(_tmp864,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpD8E,_tmpD8D,_tag_dyneither(_tmp863,sizeof(void*),1U));});});}else{
if(_tmp878->tl != 0)
# 5025
({void*_tmp866=0U;({unsigned int _tmpD90=loc;struct _dyneither_ptr _tmpD8F=({const char*_tmp867="multiple designators are not yet supported";_tag_dyneither(_tmp867,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpD90,_tmpD8F,_tag_dyneither(_tmp866,sizeof(void*),0U));});});else{
# 5028
void*_tmp868=(void*)_tmp878->hd;void*_tmp869=_tmp868;struct _dyneither_ptr*_tmp876;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp869)->tag == 0U){_LL4: _LL5:
# 5030
({struct Cyc_String_pa_PrintArg_struct _tmp86C=({struct Cyc_String_pa_PrintArg_struct _tmpA18;_tmpA18.tag=0U,_tmpA18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);_tmpA18;});void*_tmp86A[1U];_tmp86A[0]=& _tmp86C;({unsigned int _tmpD92=loc;struct _dyneither_ptr _tmpD91=({const char*_tmp86B="array designator used in argument to %s";_tag_dyneither(_tmp86B,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpD92,_tmpD91,_tag_dyneither(_tmp86A,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp876=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp869)->f1;_LL7: {
# 5033
struct Cyc_List_List*_tmp86D=fields;
for(0;_tmp86D != 0;_tmp86D=_tmp86D->tl){
if(Cyc_strptrcmp(_tmp876,((*((struct _tuple34*)_tmp86D->hd)).f1)->name)== 0){
if((*((struct _tuple34*)_tmp86D->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp870=({struct Cyc_String_pa_PrintArg_struct _tmpA19;_tmpA19.tag=0U,_tmpA19.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp876);_tmpA19;});void*_tmp86E[1U];_tmp86E[0]=& _tmp870;({unsigned int _tmpD94=loc;struct _dyneither_ptr _tmpD93=({const char*_tmp86F="member %s has already been used as an argument";_tag_dyneither(_tmp86F,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpD94,_tmpD93,_tag_dyneither(_tmp86E,sizeof(void*),1U));});});
(*((struct _tuple34*)_tmp86D->hd)).f2=1;
({struct Cyc_List_List*_tmpD96=({struct Cyc_List_List*_tmp872=_region_malloc(rgn,sizeof(*_tmp872));({struct _tuple36*_tmpD95=({struct _tuple36*_tmp871=_region_malloc(rgn,sizeof(*_tmp871));_tmp871->f1=(*((struct _tuple34*)_tmp86D->hd)).f1,_tmp871->f2=_tmp877;_tmp871;});_tmp872->hd=_tmpD95;}),_tmp872->tl=ans;_tmp872;});ans=_tmpD96;});
break;}}
# 5042
if(_tmp86D == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp875=({struct Cyc_String_pa_PrintArg_struct _tmpA1A;_tmpA1A.tag=0U,_tmpA1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp876);_tmpA1A;});void*_tmp873[1U];_tmp873[0]=& _tmp875;({unsigned int _tmpD98=loc;struct _dyneither_ptr _tmpD97=({const char*_tmp874="bad field designator %s";_tag_dyneither(_tmp874,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpD98,_tmpD97,_tag_dyneither(_tmp873,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 5047
if(aggr_kind == Cyc_Absyn_StructA)
# 5049
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple34*)fields->hd)).f2){
({void*_tmp879=0U;({unsigned int _tmpD9A=loc;struct _dyneither_ptr _tmpD99=({const char*_tmp87A="too few arguments to struct";_tag_dyneither(_tmp87A,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpD9A,_tmpD99,_tag_dyneither(_tmp879,sizeof(void*),0U));});});
break;}}else{
# 5056
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple34*)fields->hd)).f2){
if(found)({void*_tmp87B=0U;({unsigned int _tmpD9C=loc;struct _dyneither_ptr _tmpD9B=({const char*_tmp87C="only one member of a union is allowed";_tag_dyneither(_tmp87C,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpD9C,_tmpD9B,_tag_dyneither(_tmp87B,sizeof(void*),0U));});});
found=1;}}
# 5063
if(!found)({void*_tmp87D=0U;({unsigned int _tmpD9E=loc;struct _dyneither_ptr _tmpD9D=({const char*_tmp87E="missing member for union";_tag_dyneither(_tmp87E,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpD9E,_tmpD9D,_tag_dyneither(_tmp87D,sizeof(void*),0U));});});}{
# 5066
struct Cyc_List_List*_tmp87F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp87F;};};}
# 4998
;_pop_region(temp);}
# 5072
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5074
void*_tmp882=e1->r;void*_tmp883=_tmp882;struct Cyc_Absyn_Exp*_tmp895;struct Cyc_Absyn_Exp*_tmp894;struct Cyc_Absyn_Exp*_tmp893;struct Cyc_Absyn_Exp*_tmp892;struct Cyc_Absyn_Exp*_tmp891;struct Cyc_Absyn_Exp*_tmp890;switch(*((int*)_tmp883)){case 14U: _LL1: _LL2:
# 5076
({struct Cyc_String_pa_PrintArg_struct _tmp886=({struct Cyc_String_pa_PrintArg_struct _tmpA1B;_tmpA1B.tag=0U,({struct _dyneither_ptr _tmpD9F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA1B.f1=_tmpD9F;});_tmpA1B;});void*_tmp884[1U];_tmp884[0]=& _tmp886;({struct _dyneither_ptr _tmpDA0=({const char*_tmp885="we have a cast in a lhs:  %s";_tag_dyneither(_tmp885,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA0,_tag_dyneither(_tmp884,sizeof(void*),1U));});});case 20U: _LL3: _tmp890=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp883)->f1;_LL4:
 _tmp891=_tmp890;goto _LL6;case 23U: _LL5: _tmp891=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp883)->f1;_LL6:
# 5079
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp891->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp892=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp883)->f1;_LL8:
 _tmp893=_tmp892;goto _LLA;case 21U: _LL9: _tmp893=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp883)->f1;_LLA:
# 5083
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp893->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp889=({struct Cyc_String_pa_PrintArg_struct _tmpA1C;_tmpA1C.tag=0U,({
struct _dyneither_ptr _tmpDA1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA1C.f1=_tmpDA1;});_tmpA1C;});void*_tmp887[1U];_tmp887[0]=& _tmp889;({struct _dyneither_ptr _tmpDA2=({const char*_tmp888="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp888,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA2,_tag_dyneither(_tmp887,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp894=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp883)->f1;_LLC:
 _tmp895=_tmp894;goto _LLE;case 12U: _LLD: _tmp895=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp883)->f1;_LLE:
# 5089
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp895->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp88C=({struct Cyc_String_pa_PrintArg_struct _tmpA1D;_tmpA1D.tag=0U,({
struct _dyneither_ptr _tmpDA3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA1D.f1=_tmpDA3;});_tmpA1D;});void*_tmp88A[1U];_tmp88A[0]=& _tmp88C;({struct _dyneither_ptr _tmpDA4=({const char*_tmp88B="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp88B,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA4,_tag_dyneither(_tmp88A,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp88F=({struct Cyc_String_pa_PrintArg_struct _tmpA1E;_tmpA1E.tag=0U,({struct _dyneither_ptr _tmpDA5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmpA1E.f1=_tmpDA5;});_tmpA1E;});void*_tmp88D[1U];_tmp88D[0]=& _tmp88F;({struct _dyneither_ptr _tmpDA6=({const char*_tmp88E="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp88E,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDA6,_tag_dyneither(_tmp88D,sizeof(void*),1U));});});}_LL0:;}
# 5104
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5107
void*_tmp896=Cyc_Tcutil_compress(r);void*_tmp897=_tmp896;struct Cyc_Absyn_Tvar*_tmp8A3;enum Cyc_Absyn_KindQual _tmp8A2;enum Cyc_Absyn_AliasQual _tmp8A1;switch(*((int*)_tmp897)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp897)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp897)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp897)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp897)->f4 == 0){_LL5: _tmp8A2=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp897)->f3)->kind)->v)->kind;_tmp8A1=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp897)->f3)->kind)->v)->aliasqual;_LL6:
# 5111
 return _tmp8A2 == Cyc_Absyn_RgnKind  && (_tmp8A1 == Cyc_Absyn_Unique  || _tmp8A1 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp8A3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp897)->f1;_LL8: {
# 5115
struct Cyc_Absyn_Kind*_tmp898=Cyc_Tcutil_tvar_kind(_tmp8A3,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp899=_tmp898;enum Cyc_Absyn_KindQual _tmp8A0;enum Cyc_Absyn_AliasQual _tmp89F;_LLC: _tmp8A0=_tmp899->kind;_tmp89F=_tmp899->aliasqual;_LLD:;
if(_tmp8A0 == Cyc_Absyn_RgnKind  && (_tmp89F == Cyc_Absyn_Unique  || _tmp89F == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp89A=Cyc_Absyn_compress_kb(_tmp8A3->kind);void*_tmp89B=_tmp89A;struct Cyc_Core_Opt**_tmp89E;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp89E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89B)->f1;_LL10:
# 5119
({struct Cyc_Core_Opt*_tmpDA8=({struct Cyc_Core_Opt*_tmp89D=_cycalloc(sizeof(*_tmp89D));({void*_tmpDA7=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C->tag=2U,_tmp89C->f1=0,_tmp89C->f2=& Cyc_Tcutil_rk;_tmp89C;});_tmp89D->v=_tmpDA7;});_tmp89D;});*_tmp89E=_tmpDA8;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5124
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5131
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp8A4=Cyc_Tcutil_compress(t);void*_tmp8A5=_tmp8A4;struct Cyc_Absyn_Tvar*_tmp8B3;void*_tmp8B2;switch(*((int*)_tmp8A5)){case 3U: _LL1: _tmp8B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8A5)->f1).ptr_atts).rgn;_LL2:
# 5134
 return Cyc_Tcutil_is_noalias_region(_tmp8B2,must_be_unique);case 2U: _LL3: _tmp8B3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8A5)->f1;_LL4: {
# 5136
struct Cyc_Absyn_Kind*_tmp8A6=Cyc_Tcutil_tvar_kind(_tmp8B3,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp8A7=_tmp8A6;enum Cyc_Absyn_KindQual _tmp8B1;enum Cyc_Absyn_AliasQual _tmp8B0;_LL8: _tmp8B1=_tmp8A7->kind;_tmp8B0=_tmp8A7->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp8A8=_tmp8B1;switch(_tmp8A8){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if(_tmp8B0 == Cyc_Absyn_Unique  || _tmp8B0 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp8A9=Cyc_Absyn_compress_kb(_tmp8B3->kind);void*_tmp8AA=_tmp8A9;struct Cyc_Core_Opt**_tmp8AF;enum Cyc_Absyn_KindQual _tmp8AE;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AA)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AA)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp8AF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AA)->f1;_tmp8AE=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8AA)->f2)->kind;_LL15:
# 5142
({struct Cyc_Core_Opt*_tmpDAB=({struct Cyc_Core_Opt*_tmp8AD=_cycalloc(sizeof(*_tmp8AD));({void*_tmpDAA=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC->tag=2U,_tmp8AC->f1=0,({struct Cyc_Absyn_Kind*_tmpDA9=({struct Cyc_Absyn_Kind*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB->kind=_tmp8AE,_tmp8AB->aliasqual=Cyc_Absyn_Aliasable;_tmp8AB;});_tmp8AC->f2=_tmpDA9;});_tmp8AC;});_tmp8AD->v=_tmpDAA;});_tmp8AD;});*_tmp8AF=_tmpDAB;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5146
 return 1;}_LL13:;}
# 5149
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5152
 return 0;}_LL0:;}
# 5155
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp8B4=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp8B4,0))return 1;{
void*_tmp8B5=_tmp8B4;struct Cyc_List_List*_tmp8C9;union Cyc_Absyn_DatatypeFieldInfo _tmp8C8;struct Cyc_List_List*_tmp8C7;union Cyc_Absyn_DatatypeInfo _tmp8C6;struct Cyc_List_List*_tmp8C5;struct Cyc_Absyn_Aggrdecl**_tmp8C4;struct Cyc_List_List*_tmp8C3;struct Cyc_List_List*_tmp8C2;switch(*((int*)_tmp8B5)){case 6U: _LL1: _tmp8C2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8B5)->f1;_LL2:
# 5160
 while(_tmp8C2 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp8C2->hd)).f2))return 1;
_tmp8C2=_tmp8C2->tl;}
# 5164
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp8C4=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f1)->f1).KnownAggr).val;_tmp8C3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f2;_LL4:
# 5166
 if((*_tmp8C4)->impl == 0)return 0;else{
# 5168
struct Cyc_List_List*_tmp8B6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp8C4)->tvs,_tmp8C3);
struct Cyc_List_List*_tmp8B7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp8C4)->impl))->fields;
void*t;
while(_tmp8B7 != 0){
({void*_tmpDAC=_tmp8B6 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp8B7->hd)->type: Cyc_Tcutil_substitute(_tmp8B6,((struct Cyc_Absyn_Aggrfield*)_tmp8B7->hd)->type);t=_tmpDAC;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp8B7=_tmp8B7->tl;}
# 5176
return 0;}}else{_LL7: _LL8:
# 5186
 return 0;}case 18U: _LL9: _tmp8C6=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f1)->f1;_tmp8C5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f2;_LLA: {
# 5188
union Cyc_Absyn_DatatypeInfo _tmp8B8=_tmp8C6;struct Cyc_List_List*_tmp8BC;struct Cyc_Core_Opt*_tmp8BB;struct _tuple2*_tmp8BA;int _tmp8B9;if((_tmp8B8.UnknownDatatype).tag == 1){_LL10: _tmp8BA=((_tmp8B8.UnknownDatatype).val).name;_tmp8B9=((_tmp8B8.UnknownDatatype).val).is_extensible;_LL11:
# 5191
 return 0;}else{_LL12: _tmp8BC=(*(_tmp8B8.KnownDatatype).val)->tvs;_tmp8BB=(*(_tmp8B8.KnownDatatype).val)->fields;_LL13:
# 5194
 return 0;}_LLF:;}case 19U: _LLB: _tmp8C8=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f1)->f1;_tmp8C7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8B5)->f2;_LLC: {
# 5197
union Cyc_Absyn_DatatypeFieldInfo _tmp8BD=_tmp8C8;struct Cyc_Absyn_Datatypedecl*_tmp8C1;struct Cyc_Absyn_Datatypefield*_tmp8C0;if((_tmp8BD.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5200
 return 0;}else{_LL17: _tmp8C1=((_tmp8BD.KnownDatatypefield).val).f1;_tmp8C0=((_tmp8BD.KnownDatatypefield).val).f2;_LL18: {
# 5202
struct Cyc_List_List*_tmp8BE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp8C1->tvs,_tmp8C7);
struct Cyc_List_List*_tmp8BF=_tmp8C0->typs;
while(_tmp8BF != 0){
({void*_tmpDAD=_tmp8BE == 0?(*((struct _tuple12*)_tmp8BF->hd)).f2: Cyc_Tcutil_substitute(_tmp8BE,(*((struct _tuple12*)_tmp8BF->hd)).f2);_tmp8B4=_tmpDAD;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp8B4))return 1;
_tmp8BF=_tmp8BF->tl;}
# 5209
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp8C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8B5)->f2;_LL6:
# 5179
 while(_tmp8C9 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp8C9->hd)->type))return 1;
_tmp8C9=_tmp8C9->tl;}
# 5183
return 0;default: _LLD: _LLE:
# 5211
 return 0;}_LL0:;};}
# 5218
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp8CA=e->r;void*_tmp8CB=_tmp8CA;struct Cyc_Absyn_Stmt*_tmp8E1;struct Cyc_Absyn_Exp*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8DE;struct Cyc_Absyn_Exp*_tmp8DD;struct Cyc_Absyn_Exp*_tmp8DC;struct Cyc_Absyn_Exp*_tmp8DB;struct Cyc_Absyn_Exp*_tmp8DA;struct _dyneither_ptr*_tmp8D9;struct Cyc_Absyn_Exp*_tmp8D8;struct Cyc_Absyn_Exp*_tmp8D7;switch(*((int*)_tmp8CB)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8CB)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp8D7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8CB)->f1;_LL4:
 _tmp8D8=_tmp8D7;goto _LL6;case 20U: _LL5: _tmp8D8=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8CB)->f1;_LL6:
# 5223
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp8D8->topt),1) && Cyc_Tcutil_is_noalias_path(_tmp8D8);case 21U: _LL7: _tmp8DA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8CB)->f1;_tmp8D9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8CB)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp8DA);case 23U: _LL9: _tmp8DC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8CB)->f1;_tmp8DB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8CB)->f2;_LLA: {
# 5226
void*_tmp8CC=Cyc_Tcutil_compress((void*)_check_null(_tmp8DC->topt));void*_tmp8CD=_tmp8CC;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8CD)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp8DC);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp8DE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8CB)->f2;_tmp8DD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp8CB)->f3;_LLC:
# 5231
 return Cyc_Tcutil_is_noalias_path(_tmp8DE) && Cyc_Tcutil_is_noalias_path(_tmp8DD);case 9U: _LLD: _tmp8DF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp8CB)->f2;_LLE:
 _tmp8E0=_tmp8DF;goto _LL10;case 14U: _LLF: _tmp8E0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp8CB)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp8E0);case 37U: _LL11: _tmp8E1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp8CB)->f1;_LL12:
# 5235
 while(1){
void*_tmp8CE=_tmp8E1->r;void*_tmp8CF=_tmp8CE;struct Cyc_Absyn_Exp*_tmp8D6;struct Cyc_Absyn_Decl*_tmp8D5;struct Cyc_Absyn_Stmt*_tmp8D4;struct Cyc_Absyn_Stmt*_tmp8D3;struct Cyc_Absyn_Stmt*_tmp8D2;switch(*((int*)_tmp8CF)){case 2U: _LL1B: _tmp8D3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8CF)->f1;_tmp8D2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp8CF)->f2;_LL1C:
 _tmp8E1=_tmp8D2;goto _LL1A;case 12U: _LL1D: _tmp8D5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8CF)->f1;_tmp8D4=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp8CF)->f2;_LL1E:
 _tmp8E1=_tmp8D4;goto _LL1A;case 1U: _LL1F: _tmp8D6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp8CF)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp8D6);default: _LL21: _LL22:
({void*_tmp8D0=0U;({struct _dyneither_ptr _tmpDAE=({const char*_tmp8D1="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmp8D1,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDAE,_tag_dyneither(_tmp8D0,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5243
 return 1;}_LL0:;}
# 5260 "tcutil.cyc"
struct _tuple17 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5262
struct _tuple17 bogus_ans=({struct _tuple17 _tmpA2B;_tmpA2B.f1=0,_tmpA2B.f2=Cyc_Absyn_heap_rgn_type;_tmpA2B;});
void*_tmp8E2=e->r;void*_tmp8E3=_tmp8E2;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91A;struct Cyc_Absyn_Exp*_tmp919;struct _dyneither_ptr*_tmp918;int _tmp917;struct Cyc_Absyn_Exp*_tmp916;struct _dyneither_ptr*_tmp915;int _tmp914;void*_tmp913;switch(*((int*)_tmp8E3)){case 1U: _LL1: _tmp913=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp8E3)->f1;_LL2: {
# 5266
void*_tmp8E4=_tmp913;struct Cyc_Absyn_Vardecl*_tmp8EC;struct Cyc_Absyn_Vardecl*_tmp8EB;struct Cyc_Absyn_Vardecl*_tmp8EA;struct Cyc_Absyn_Vardecl*_tmp8E9;switch(*((int*)_tmp8E4)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp8E9=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp8E4)->f1;_LL13: {
# 5270
void*_tmp8E5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8E6=_tmp8E5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E6)->tag == 4U){_LL1B: _LL1C:
# 5272
 return({struct _tuple17 _tmpA1F;_tmpA1F.f1=1,_tmpA1F.f2=Cyc_Absyn_heap_rgn_type;_tmpA1F;});}else{_LL1D: _LL1E:
 return({struct _tuple17 _tmpA20;_tmpA20.f1=(_tmp8E9->tq).real_const,_tmpA20.f2=Cyc_Absyn_heap_rgn_type;_tmpA20;});}_LL1A:;}case 4U: _LL14: _tmp8EA=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp8E4)->f1;_LL15: {
# 5276
void*_tmp8E7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp8E8=_tmp8E7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp8E8)->tag == 4U){_LL20: _LL21:
 return({struct _tuple17 _tmpA21;_tmpA21.f1=1,_tmpA21.f2=(void*)_check_null(_tmp8EA->rgn);_tmpA21;});}else{_LL22: _LL23:
# 5279
 _tmp8EA->escapes=1;
return({struct _tuple17 _tmpA22;_tmpA22.f1=(_tmp8EA->tq).real_const,_tmpA22.f2=(void*)_check_null(_tmp8EA->rgn);_tmpA22;});}_LL1F:;}case 5U: _LL16: _tmp8EB=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp8E4)->f1;_LL17:
# 5282
 _tmp8EC=_tmp8EB;goto _LL19;default: _LL18: _tmp8EC=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp8E4)->f1;_LL19:
# 5284
 _tmp8EC->escapes=1;
return({struct _tuple17 _tmpA23;_tmpA23.f1=(_tmp8EC->tq).real_const,_tmpA23.f2=(void*)_check_null(_tmp8EC->rgn);_tmpA23;});}_LLD:;}case 21U: _LL3: _tmp916=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8E3)->f1;_tmp915=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8E3)->f2;_tmp914=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp8E3)->f3;_LL4:
# 5289
 if(_tmp914)return bogus_ans;{
# 5292
void*_tmp8ED=Cyc_Tcutil_compress((void*)_check_null(_tmp916->topt));void*_tmp8EE=_tmp8ED;struct Cyc_Absyn_Aggrdecl*_tmp8FA;struct Cyc_List_List*_tmp8F9;switch(*((int*)_tmp8EE)){case 7U: _LL25: _tmp8F9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8EE)->f2;_LL26: {
# 5294
struct Cyc_Absyn_Aggrfield*_tmp8EF=Cyc_Absyn_lookup_field(_tmp8F9,_tmp915);
if(_tmp8EF != 0  && _tmp8EF->width == 0){
struct _tuple17 _tmp8F0=Cyc_Tcutil_addressof_props(te,_tmp916);struct _tuple17 _tmp8F1=_tmp8F0;int _tmp8F3;void*_tmp8F2;_LL2C: _tmp8F3=_tmp8F1.f1;_tmp8F2=_tmp8F1.f2;_LL2D:;
return({struct _tuple17 _tmpA24;_tmpA24.f1=(_tmp8EF->tq).real_const  || _tmp8F3,_tmpA24.f2=_tmp8F2;_tmpA24;});}
# 5299
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8EE)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8EE)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp8FA=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8EE)->f1)->f1).KnownAggr).val;_LL28: {
# 5301
struct Cyc_Absyn_Aggrfield*_tmp8F4=Cyc_Absyn_lookup_decl_field(_tmp8FA,_tmp915);
if(_tmp8F4 != 0  && _tmp8F4->width == 0){
struct _tuple17 _tmp8F5=Cyc_Tcutil_addressof_props(te,_tmp916);struct _tuple17 _tmp8F6=_tmp8F5;int _tmp8F8;void*_tmp8F7;_LL2F: _tmp8F8=_tmp8F6.f1;_tmp8F7=_tmp8F6.f2;_LL30:;
return({struct _tuple17 _tmpA25;_tmpA25.f1=(_tmp8F4->tq).real_const  || _tmp8F8,_tmpA25.f2=_tmp8F7;_tmpA25;});}
# 5306
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp919=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8E3)->f1;_tmp918=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8E3)->f2;_tmp917=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp8E3)->f3;_LL6:
# 5311
 if(_tmp917)return bogus_ans;{
# 5314
void*_tmp8FB=Cyc_Tcutil_compress((void*)_check_null(_tmp919->topt));void*_tmp8FC=_tmp8FB;void*_tmp902;void*_tmp901;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FC)->tag == 3U){_LL32: _tmp902=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FC)->f1).elt_type;_tmp901=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8FC)->f1).ptr_atts).rgn;_LL33: {
# 5316
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp8FD=Cyc_Tcutil_compress(_tmp902);void*_tmp8FE=_tmp8FD;struct Cyc_Absyn_Aggrdecl*_tmp900;struct Cyc_List_List*_tmp8FF;switch(*((int*)_tmp8FE)){case 7U: _LL37: _tmp8FF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp8FE)->f2;_LL38:
# 5319
({struct Cyc_Absyn_Aggrfield*_tmpDAF=Cyc_Absyn_lookup_field(_tmp8FF,_tmp918);finfo=_tmpDAF;});goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8FE)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8FE)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp900=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp8FE)->f1)->f1).KnownAggr).val;_LL3A:
# 5321
({struct Cyc_Absyn_Aggrfield*_tmpDB0=Cyc_Absyn_lookup_decl_field(_tmp900,_tmp918);finfo=_tmpDB0;});goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5324
if(finfo != 0  && finfo->width == 0)
return({struct _tuple17 _tmpA26;_tmpA26.f1=(finfo->tq).real_const,_tmpA26.f2=_tmp901;_tmpA26;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp91A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp8E3)->f1;_LL8: {
# 5331
void*_tmp903=Cyc_Tcutil_compress((void*)_check_null(_tmp91A->topt));void*_tmp904=_tmp903;struct Cyc_Absyn_Tqual _tmp906;void*_tmp905;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp904)->tag == 3U){_LL3E: _tmp906=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp904)->f1).elt_tq;_tmp905=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp904)->f1).ptr_atts).rgn;_LL3F:
# 5333
 return({struct _tuple17 _tmpA27;_tmpA27.f1=_tmp906.real_const,_tmpA27.f2=_tmp905;_tmpA27;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp91C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8E3)->f1;_tmp91B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp8E3)->f2;_LLA: {
# 5339
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp91C->topt));
void*_tmp907=t;struct Cyc_Absyn_Tqual _tmp910;struct Cyc_Absyn_Tqual _tmp90F;void*_tmp90E;struct Cyc_List_List*_tmp90D;switch(*((int*)_tmp907)){case 6U: _LL43: _tmp90D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp907)->f1;_LL44: {
# 5343
struct _tuple13 _tmp908=Cyc_Evexp_eval_const_uint_exp(_tmp91B);struct _tuple13 _tmp909=_tmp908;unsigned int _tmp90C;int _tmp90B;_LL4C: _tmp90C=_tmp909.f1;_tmp90B=_tmp909.f2;_LL4D:;
if(!_tmp90B)
return bogus_ans;{
struct _tuple12*_tmp90A=Cyc_Absyn_lookup_tuple_field(_tmp90D,(int)_tmp90C);
if(_tmp90A != 0)
return({struct _tuple17 _tmpA28;_tmpA28.f1=((*_tmp90A).f1).real_const,({void*_tmpDB1=(Cyc_Tcutil_addressof_props(te,_tmp91C)).f2;_tmpA28.f2=_tmpDB1;});_tmpA28;});
return bogus_ans;};}case 3U: _LL45: _tmp90F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp907)->f1).elt_tq;_tmp90E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp907)->f1).ptr_atts).rgn;_LL46:
# 5351
 return({struct _tuple17 _tmpA29;_tmpA29.f1=_tmp90F.real_const,_tmpA29.f2=_tmp90E;_tmpA29;});case 4U: _LL47: _tmp910=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp907)->f1).tq;_LL48:
# 5357
 return({struct _tuple17 _tmpA2A;_tmpA2A.f1=_tmp910.real_const,({void*_tmpDB2=(Cyc_Tcutil_addressof_props(te,_tmp91C)).f2;_tmpA2A.f2=_tmpDB2;});_tmpA2A;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5361
({void*_tmp911=0U;({unsigned int _tmpDB4=e->loc;struct _dyneither_ptr _tmpDB3=({const char*_tmp912="unary & applied to non-lvalue";_tag_dyneither(_tmp912,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpDB4,_tmpDB3,_tag_dyneither(_tmp911,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5368
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp91D=({void*_tmpDB5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpDB5,b);});
if(_tmp91D == 0)return;{
struct Cyc_Absyn_Exp*_tmp91E=_tmp91D;
struct _tuple13 _tmp91F=Cyc_Evexp_eval_const_uint_exp(_tmp91E);struct _tuple13 _tmp920=_tmp91F;unsigned int _tmp92A;int _tmp929;_LL1: _tmp92A=_tmp920.f1;_tmp929=_tmp920.f2;_LL2:;
if(_tmp929  && _tmp92A <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp923=({struct Cyc_Int_pa_PrintArg_struct _tmpA2D;_tmpA2D.tag=1U,_tmpA2D.f1=(unsigned long)((int)_tmp92A);_tmpA2D;});struct Cyc_Int_pa_PrintArg_struct _tmp924=({struct Cyc_Int_pa_PrintArg_struct _tmpA2C;_tmpA2C.tag=1U,_tmpA2C.f1=(unsigned long)((int)i);_tmpA2C;});void*_tmp921[2U];_tmp921[0]=& _tmp923,_tmp921[1]=& _tmp924;({unsigned int _tmpDB7=loc;struct _dyneither_ptr _tmpDB6=({const char*_tmp922="a dereference will be out of bounds: %d <= %d";_tag_dyneither(_tmp922,sizeof(char),46U);});Cyc_Tcutil_warn(_tmpDB7,_tmpDB6,_tag_dyneither(_tmp921,sizeof(void*),2U));});});else{
# 5377
({struct Cyc_Int_pa_PrintArg_struct _tmp927=({struct Cyc_Int_pa_PrintArg_struct _tmpA2F;_tmpA2F.tag=1U,_tmpA2F.f1=(unsigned long)((int)_tmp92A);_tmpA2F;});struct Cyc_Int_pa_PrintArg_struct _tmp928=({struct Cyc_Int_pa_PrintArg_struct _tmpA2E;_tmpA2E.tag=1U,_tmpA2E.f1=(unsigned long)((int)i);_tmpA2E;});void*_tmp925[2U];_tmp925[0]=& _tmp927,_tmp925[1]=& _tmp928;({unsigned int _tmpDB9=loc;struct _dyneither_ptr _tmpDB8=({const char*_tmp926="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmp926,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpDB9,_tmpDB8,_tag_dyneither(_tmp925,sizeof(void*),2U));});});}}
return;};}
# 5381
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 5386
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp92B=e->r;void*_tmp92C=_tmp92B;struct Cyc_Absyn_Exp*_tmp92E;struct Cyc_Absyn_Exp*_tmp92D;switch(*((int*)_tmp92C)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmp92D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp92C)->f1;_LL4:
 _tmp92E=_tmp92D;goto _LL6;case 13U: _LL5: _tmp92E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp92C)->f1;_LL6:
 e=_tmp92E;continue;default: _LL7: _LL8:
# 5393
 return 0;}_LL0:;}}
# 5403
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp92F=e->r;void*_tmp930=_tmp92F;struct Cyc_List_List*_tmp94F;struct Cyc_List_List*_tmp94E;enum Cyc_Absyn_Primop _tmp94D;struct Cyc_List_List*_tmp94C;struct Cyc_List_List*_tmp94B;struct Cyc_List_List*_tmp94A;struct Cyc_List_List*_tmp949;struct Cyc_Absyn_Exp*_tmp948;struct Cyc_Absyn_Exp*_tmp947;struct Cyc_Absyn_Exp*_tmp946;struct Cyc_Absyn_Exp*_tmp945;void*_tmp944;struct Cyc_Absyn_Exp*_tmp943;void*_tmp942;struct Cyc_Absyn_Exp*_tmp941;struct Cyc_Absyn_Exp*_tmp940;struct Cyc_Absyn_Exp*_tmp93F;struct Cyc_Absyn_Exp*_tmp93E;struct Cyc_Absyn_Exp*_tmp93D;struct Cyc_Absyn_Exp*_tmp93C;struct Cyc_Absyn_Exp*_tmp93B;struct Cyc_Absyn_Exp*_tmp93A;struct Cyc_Absyn_Exp*_tmp939;void*_tmp938;switch(*((int*)_tmp930)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp938=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL10: {
# 5415
void*_tmp931=_tmp938;struct Cyc_Absyn_Vardecl*_tmp937;struct Cyc_Absyn_Vardecl*_tmp936;switch(*((int*)_tmp931)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp936=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp931)->f1;_LL37: {
# 5418
void*_tmp932=Cyc_Tcutil_compress(_tmp936->type);void*_tmp933=_tmp932;switch(*((int*)_tmp933)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp937=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp931)->f1;_LL39:
# 5425
 if(_tmp937->sc == Cyc_Absyn_Static){
void*_tmp934=Cyc_Tcutil_compress(_tmp937->type);void*_tmp935=_tmp934;switch(*((int*)_tmp935)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 5432
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp93B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_tmp93A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp930)->f2;_tmp939=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp930)->f3;_LL12:
# 5438
 return(Cyc_Tcutil_cnst_exp(0,_tmp93B) && 
Cyc_Tcutil_cnst_exp(0,_tmp93A)) && 
Cyc_Tcutil_cnst_exp(0,_tmp939);case 9U: _LL13: _tmp93D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_tmp93C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp930)->f2;_LL14:
# 5442
 return Cyc_Tcutil_cnst_exp(0,_tmp93D) && Cyc_Tcutil_cnst_exp(0,_tmp93C);case 41U: _LL15: _tmp93E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL16:
 _tmp93F=_tmp93E;goto _LL18;case 12U: _LL17: _tmp93F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL18:
 _tmp940=_tmp93F;goto _LL1A;case 13U: _LL19: _tmp940=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL1A:
# 5446
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp940);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp930)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp942=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_tmp941=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp930)->f2;_LL1C:
# 5448
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp941);}else{_LL1D: _tmp944=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_tmp943=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp930)->f2;_LL1E:
# 5451
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp943);}case 15U: _LL1F: _tmp945=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL20:
# 5453
 return Cyc_Tcutil_cnst_exp(1,_tmp945);case 27U: _LL21: _tmp947=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp930)->f2;_tmp946=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp930)->f3;_LL22:
# 5455
 return Cyc_Tcutil_cnst_exp(0,_tmp947) && Cyc_Tcutil_cnst_exp(0,_tmp946);case 28U: _LL23: _tmp948=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL24:
# 5457
 return Cyc_Tcutil_cnst_exp(0,_tmp948);case 26U: _LL25: _tmp949=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL26:
 _tmp94A=_tmp949;goto _LL28;case 30U: _LL27: _tmp94A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp930)->f2;_LL28:
 _tmp94B=_tmp94A;goto _LL2A;case 29U: _LL29: _tmp94B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp930)->f3;_LL2A:
# 5461
 for(0;_tmp94B != 0;_tmp94B=_tmp94B->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple18*)_tmp94B->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp94D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_tmp94C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp930)->f2;_LL2C:
# 5466
 _tmp94E=_tmp94C;goto _LL2E;case 24U: _LL2D: _tmp94E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL2E:
 _tmp94F=_tmp94E;goto _LL30;case 31U: _LL2F: _tmp94F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp930)->f1;_LL30:
# 5469
 for(0;_tmp94F != 0;_tmp94F=_tmp94F->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp94F->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 5477
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5481
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5483
int Cyc_Tcutil_supports_default(void*t){
void*_tmp950=Cyc_Tcutil_compress(t);void*_tmp951=_tmp950;struct Cyc_List_List*_tmp95B;struct Cyc_List_List*_tmp95A;void*_tmp959;void*_tmp958;void*_tmp957;void*_tmp956;struct Cyc_List_List*_tmp955;switch(*((int*)_tmp951)){case 0U: _LL1: _tmp956=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp951)->f1;_tmp955=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp951)->f2;_LL2: {
# 5486
void*_tmp952=_tmp956;union Cyc_Absyn_AggrInfo _tmp954;switch(*((int*)_tmp952)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 16U: _LL14: _LL15:
 goto _LL17;case 15U: _LL16: _LL17:
 return 1;case 20U: _LL18: _tmp954=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp952)->f1;_LL19: {
# 5493
struct Cyc_Absyn_Aggrdecl*_tmp953=Cyc_Absyn_get_known_aggrdecl(_tmp954);
if(_tmp953->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp953->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmp953->tvs,_tmp955,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp953->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp958=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp951)->f1).ptr_atts).nullable;_tmp957=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp951)->f1).ptr_atts).bounds;_LL4:
# 5500
 return Cyc_Tcutil_unify(_tmp957,Cyc_Absyn_fat_bound_type) || Cyc_Tcutil_force_type2bool(1,_tmp958);case 4U: _LL5: _tmp959=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp951)->f1).elt_type;_LL6:
 return Cyc_Tcutil_supports_default(_tmp959);case 6U: _LL7: _tmp95A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp951)->f1;_LL8:
# 5503
 for(0;_tmp95A != 0;_tmp95A=_tmp95A->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmp95A->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp95B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp951)->f2;_LLA:
 return Cyc_Tcutil_fields_support_default(0,0,_tmp95B);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 5512
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmp95C=t;struct Cyc_Absyn_Typedefdecl*_tmp961;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95C)->tag == 8U){_LL1: _tmp961=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp95C)->f3;_LL2:
# 5515
 if(_tmp961 != 0){
struct Cyc_Absyn_Tqual _tmp95D=_tmp961->tq;
if(((_tmp95D.print_const  || _tmp95D.q_volatile) || _tmp95D.q_restrict) || _tmp95D.real_const)
# 5520
({struct Cyc_String_pa_PrintArg_struct _tmp960=({struct Cyc_String_pa_PrintArg_struct _tmpA30;_tmpA30.tag=0U,({struct _dyneither_ptr _tmpDBA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA30.f1=_tmpDBA;});_tmpA30;});void*_tmp95E[1U];_tmp95E[0]=& _tmp960;({unsigned int _tmpDBC=loc;struct _dyneither_ptr _tmpDBB=({const char*_tmp95F="qualifier within typedef type %s is ignored";_tag_dyneither(_tmp95F,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpDBC,_tmpDBB,_tag_dyneither(_tmp95E,sizeof(void*),1U));});});}
# 5523
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5528
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5530
struct _RegionHandle _tmp962=_new_region("rgn");struct _RegionHandle*rgn=& _tmp962;_push_region(rgn);
{struct Cyc_List_List*_tmp963=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmp964=1;_npop_handler(0U);return _tmp964;}
({void*_tmpDBD=Cyc_Tcutil_rsubstitute(rgn,_tmp963,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpDBD;});
if(!Cyc_Tcutil_supports_default(t)){int _tmp965=0;_npop_handler(0U);return _tmp965;}}{
# 5538
int _tmp966=1;_npop_handler(0U);return _tmp966;};}
# 5531
;_pop_region(rgn);}
# 5544
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp967=Cyc_Tcutil_compress(t);void*_tmp968=_tmp967;void*_tmp96A;void*_tmp969;switch(*((int*)_tmp968)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp968)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp96A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp968)->f1).ptr_atts).nullable;_tmp969=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp968)->f1).ptr_atts).bounds;_LL6:
# 5552
 return !Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp969) && Cyc_Tcutil_force_type2bool(0,_tmp96A);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 5559
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp96B=Cyc_Tcutil_compress(t);void*_tmp96C=_tmp96B;struct Cyc_List_List**_tmp972;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp96C)->tag == 5U){_LL1: _tmp972=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp96C)->f1).attributes;_LL2: {
# 5562
struct Cyc_List_List*_tmp96D=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmp972,(void*)atts->hd))
({struct Cyc_List_List*_tmpDBE=({struct Cyc_List_List*_tmp96E=_cycalloc(sizeof(*_tmp96E));_tmp96E->hd=(void*)atts->hd,_tmp96E->tl=*_tmp972;_tmp96E;});*_tmp972=_tmpDBE;});}else{
# 5569
({struct Cyc_List_List*_tmpDBF=({struct Cyc_List_List*_tmp96F=_cycalloc(sizeof(*_tmp96F));_tmp96F->hd=(void*)atts->hd,_tmp96F->tl=_tmp96D;_tmp96F;});_tmp96D=_tmpDBF;});}}
return _tmp96D;}}else{_LL3: _LL4:
({void*_tmp970=0U;({struct _dyneither_ptr _tmpDC0=({const char*_tmp971="transfer_fn_type_atts";_tag_dyneither(_tmp971,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDC0,_tag_dyneither(_tmp970,sizeof(void*),0U));});});}_LL0:;}
# 5576
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp973=Cyc_Tcutil_compress(t);void*_tmp974=_tmp973;struct Cyc_Absyn_Exp*_tmp976;struct Cyc_Absyn_PtrInfo*_tmp975;switch(*((int*)_tmp974)){case 3U: _LL1: _tmp975=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp974)->f1;_LL2:
# 5579
 return({void*_tmpDC1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmpDC1,(_tmp975->ptr_atts).bounds);});case 4U: _LL3: _tmp976=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp974)->f1).num_elts;_LL4:
# 5581
 return _tmp976;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5588
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmp977=b;struct Cyc_Absyn_Vardecl*_tmp97B;struct Cyc_Absyn_Vardecl*_tmp97A;struct Cyc_Absyn_Vardecl*_tmp979;struct Cyc_Absyn_Vardecl*_tmp978;switch(*((int*)_tmp977)){case 5U: _LL1: _tmp978=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp977)->f1;_LL2:
 _tmp979=_tmp978;goto _LL4;case 4U: _LL3: _tmp979=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp977)->f1;_LL4:
 _tmp97A=_tmp979;goto _LL6;case 3U: _LL5: _tmp97A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp977)->f1;_LL6:
 _tmp97B=_tmp97A;goto _LL8;case 1U: _LL7: _tmp97B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp977)->f1;_LL8:
# 5594
 if(!_tmp97B->escapes)return _tmp97B;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5598
return 0;}
# 5602
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp97C=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpDC2=({struct Cyc_List_List*_tmp97D=_cycalloc(sizeof(*_tmp97D));_tmp97D->hd=*((void**)_check_null((void**)x->hd)),_tmp97D->tl=_tmp97C;_tmp97D;});_tmp97C=_tmpDC2;});}}
return _tmp97C;}
# 5609
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp97E=Cyc_Tcutil_compress(t);void*_tmp97F=_tmp97E;void*_tmp989;struct Cyc_Absyn_Tqual _tmp988;struct Cyc_Absyn_Exp*_tmp987;void*_tmp986;unsigned int _tmp985;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97F)->tag == 4U){_LL1: _tmp989=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97F)->f1).elt_type;_tmp988=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97F)->f1).tq;_tmp987=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97F)->f1).num_elts;_tmp986=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97F)->f1).zero_term;_tmp985=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp97F)->f1).zt_loc;_LL2: {
# 5612
void*b;
if(_tmp987 == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 5616
struct Cyc_Absyn_Exp*bnd=_tmp987;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmp980=0U;({struct _dyneither_ptr _tmpDC3=({const char*_tmp981="cannot convert tag without type!";_tag_dyneither(_tmp981,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDC3,_tag_dyneither(_tmp980,sizeof(void*),0U));});});{
void*_tmp982=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmp983=_tmp982;void*_tmp984;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp983)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp983)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp983)->f2 != 0){_LL6: _tmp984=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp983)->f2)->hd;_LL7:
# 5622
({void*_tmpDC5=Cyc_Absyn_thin_bounds_exp(({void*_tmpDC4=Cyc_Absyn_uint_type;Cyc_Absyn_cast_exp(_tmpDC4,Cyc_Absyn_valueof_exp(_tmp984,0U),0,Cyc_Absyn_No_coercion,0U);}));b=_tmpDC5;});
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 5625
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpDC6=Cyc_Absyn_thin_bounds_exp(bnd);b=_tmpDC6;});else{
# 5628
b=Cyc_Absyn_fat_bound_type;}}_LL5:;};}else{
# 5632
({void*_tmpDC7=Cyc_Absyn_thin_bounds_exp(bnd);b=_tmpDC7;});}}
# 5634
return Cyc_Absyn_atb_type(_tmp989,rgn,_tmp988,b,_tmp986);}}else{_LL3: _LL4:
 return t;}_LL0:;}
# 5640
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp98A=Cyc_Tcutil_compress(t);void*_tmp98B=_tmp98A;struct Cyc_List_List*_tmp99B;struct Cyc_List_List*_tmp99A;void*_tmp999;void*_tmp998;void*_tmp997;struct Cyc_List_List*_tmp996;switch(*((int*)_tmp98B)){case 0U: _LL1: _tmp997=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp98B)->f1;_tmp996=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp98B)->f2;_LL2: {
# 5643
void*_tmp98C=_tmp997;union Cyc_Absyn_AggrInfo _tmp995;switch(*((int*)_tmp98C)){case 0U: _LL1A: _LL1B:
 goto _LL1D;case 1U: _LL1C: _LL1D:
 goto _LL1F;case 2U: _LL1E: _LL1F:
 return 1;case 15U: _LL20: _LL21:
 goto _LL23;case 4U: _LL22: _LL23:
 return 1;case 16U: _LL24: _LL25:
 return 1;case 3U: _LL26: _LL27:
 goto _LL29;case 18U: _LL28: _LL29:
 goto _LL2B;case 19U: _LL2A: _LL2B:
 goto _LL2D;case 17U: _LL2C: _LL2D:
 return 0;case 5U: _LL2E: _LL2F:
 goto _LL31;case 6U: _LL30: _LL31:
 goto _LL33;case 7U: _LL32: _LL33:
 goto _LL35;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 11U: _LL38: _LL39:
 goto _LL3B;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 13U: _LL3C: _LL3D:
 goto _LL3F;case 14U: _LL3E: _LL3F:
 goto _LL41;case 10U: _LL40: _LL41:
({struct Cyc_String_pa_PrintArg_struct _tmp98F=({struct Cyc_String_pa_PrintArg_struct _tmpA31;_tmpA31.tag=0U,({struct _dyneither_ptr _tmpDC8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA31.f1=_tmpDC8;});_tmpA31;});void*_tmp98D[1U];_tmp98D[0]=& _tmp98F;({struct _dyneither_ptr _tmpDC9=({const char*_tmp98E="bad type `%s' in zeroable type";_tag_dyneither(_tmp98E,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDC9,_tag_dyneither(_tmp98D,sizeof(void*),1U));});});default: _LL42: _tmp995=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp98C)->f1;_LL43: {
# 5665
struct Cyc_Absyn_Aggrdecl*_tmp990=Cyc_Absyn_get_known_aggrdecl(_tmp995);
if(_tmp990->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp990->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmp991=_new_region("r");struct _RegionHandle*r=& _tmp991;_push_region(r);
{struct Cyc_List_List*_tmp992=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp990->tvs,_tmp996);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp990->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmp992,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp993=0;_npop_handler(0U);return _tmp993;}}}{
int _tmp994=1;_npop_handler(0U);return _tmp994;};}
# 5669
;_pop_region(r);};}}_LL19:;}case 1U: _LL3: _LL4:
# 5674
 goto _LL6;case 2U: _LL5: _LL6:
 return 0;case 3U: _LL7: _tmp998=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp98B)->f1).ptr_atts).nullable;_LL8:
# 5677
 return Cyc_Absyn_type2bool(1,_tmp998);case 4U: _LL9: _tmp999=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp98B)->f1).elt_type;_LLA:
 return Cyc_Tcutil_zeroable_type(_tmp999);case 5U: _LLB: _LLC:
 return 0;case 6U: _LLD: _tmp99A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp98B)->f1;_LLE:
# 5681
 for(0;(unsigned int)_tmp99A;_tmp99A=_tmp99A->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmp99A->hd)).f2))return 0;}
return 1;case 7U: _LLF: _tmp99B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp98B)->f2;_LL10:
# 5685
 for(0;_tmp99B != 0;_tmp99B=_tmp99B->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmp99B->hd)->type))return 0;}
return 1;case 8U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;default: _LL17: _LL18:
 return 0;}_LL0:;}
