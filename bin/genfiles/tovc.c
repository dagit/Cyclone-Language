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
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 72 "list.h"
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 281 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict,void*);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 430 "absyn.h"
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 964
extern struct _tuple0*Cyc_Absyn_exn_name;
# 989
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_true_exp(unsigned int);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1057
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1079
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1092
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1107
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1178
struct _tuple0*Cyc_Absyn_binding2qvar(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 141
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
# 34 "tcutil.h"
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct Cyc_Hashtable_Table;
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple0*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 49
void Cyc_Toc_finish();
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);
# 71 "tovc.cyc"
int Cyc_Tovc_elim_array_initializers=1;struct Cyc_Tovc_BoxingEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_List_List*boundvars;struct Cyc_Set_Set**freevars;};struct Cyc_Tovc_ToExpEnv{struct _RegionHandle*rgn;struct Cyc_List_List*all_locals;struct Cyc_Dict_Dict varmap;struct Cyc_Absyn_Stmt*encloser;struct Cyc_List_List**gen_ds;};struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 99
static struct Cyc_Absyn_Exp*Cyc_Tovc_box_free_vars_exp(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Exp*e){
while(1){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_List_List*_tmp43;struct Cyc_Absyn_Exp**_tmp42;struct Cyc_Absyn_Exp**_tmp41;struct Cyc_Absyn_Exp**_tmp40;struct Cyc_Absyn_Exp**_tmp3F;struct Cyc_Absyn_Exp**_tmp3E;struct Cyc_Absyn_Exp**_tmp3D;struct Cyc_Absyn_Exp**_tmp3C;struct Cyc_Absyn_Exp**_tmp3B;struct Cyc_Absyn_Exp**_tmp3A;struct Cyc_Absyn_Exp**_tmp39;struct Cyc_Absyn_Exp**_tmp38;struct Cyc_Absyn_Exp**_tmp37;struct Cyc_Absyn_Exp**_tmp36;struct Cyc_Absyn_Exp**_tmp35;struct Cyc_Absyn_Exp**_tmp34;struct Cyc_Absyn_Exp**_tmp33;struct Cyc_List_List*_tmp32;struct Cyc_Absyn_Exp**_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_Absyn_Exp*_tmp2F;struct _dyneither_ptr*_tmp2E;int _tmp2D;int _tmp2C;struct Cyc_Absyn_Exp*_tmp2B;struct _dyneither_ptr*_tmp2A;int _tmp29;int _tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;void*_tmp23;switch(*((int*)_tmp1)){case 1U: _LL1: _tmp23=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2: {
# 103
struct _tuple0*_tmp2=Cyc_Absyn_binding2qvar(_tmp23);struct _tuple0*_tmp3=_tmp2;struct _dyneither_ptr*_tmpB;if(((((struct _tuple0*)_tmp3)->f1).Loc_n).tag == 4){_LL32: _tmpB=_tmp3->f2;_LL33:
# 107
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(env.varmap,_tmpB)){
({void*_tmp135=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(env.varmap,_tmpB))->r;e->r=_tmp135;});
continue;}
# 112
{int i=0;for(0;i < _get_dyneither_size(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*));++ i){
if(Cyc_strptrcmp(_tmpB,*((struct _dyneither_ptr**)_check_dyneither_subscript(Cyc_Toc_globals,sizeof(struct _dyneither_ptr*),i)))== 0)
return 0;}}
# 116
for(0;env.boundvars != 0;env.boundvars=(env.boundvars)->tl){
if(Cyc_strptrcmp(_tmpB,(*((struct Cyc_Absyn_Vardecl*)(env.boundvars)->hd)->name).f2)== 0)
return 0;}
# 120
for(0;env.all_locals != 0;env.all_locals=(env.all_locals)->tl){
struct Cyc_Absyn_Vardecl*_tmp4=(struct Cyc_Absyn_Vardecl*)(env.all_locals)->hd;
if(Cyc_strptrcmp(_tmpB,(*_tmp4->name).f2)== 0){
({struct Cyc_Set_Set*_tmp136=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct Cyc_Absyn_Vardecl*elt))Cyc_Set_rinsert)(env.rgn,*env.freevars,_tmp4);*env.freevars=_tmp136;});{
# 125
void*_tmp5=_tmp4->type;void*_tmp6=_tmp5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6)->tag == 8U){_LL37: _LL38:
 return 0;}else{_LL39: _LL3A:
 return Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp4->name,0U),0U);}_LL36:;};}}
# 134
for(0;env.all_locals != 0;env.all_locals=((struct Cyc_List_List*)_check_null(env.all_locals))->tl){
if(Cyc_strptrcmp(_tmpB,(*((struct _tuple0*)_check_null(((struct Cyc_Absyn_Vardecl*)_check_null((struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(env.all_locals))->hd))->name))).f2)== 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp138=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp12E;_tmp12E.tag=0U,_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct _dyneither_ptr*)_check_null(_tmpB)));_tmp12E;});void*_tmp7[1U];_tmp7[0]=& _tmp9;({struct _dyneither_ptr _tmp137=({const char*_tmp8="unbound variable %s found in box_free_vars_exp";_tag_dyneither(_tmp8,sizeof(char),47U);});Cyc_aprintf(_tmp137,_tag_dyneither(_tmp7,sizeof(void*),1U));});});_tmpA->f1=_tmp138;});_tmpA;}));}
# 141
return 0;}else{_LL34: _LL35:
 return 0;}_LL31:;}case 23U: _LL3: _tmp25=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp24=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL4: {
# 148
struct Cyc_Absyn_Exp*_tmpC=Cyc_Tovc_box_free_vars_exp(env,_tmp25);
struct Cyc_Absyn_Exp*_tmpD=Cyc_Tovc_box_free_vars_exp(env,_tmp24);
if(_tmpC != 0){
if(_tmpD != 0){
struct Cyc_Absyn_Exp*_tmpE=Cyc_Absyn_subscript_exp(_tmpC,_tmpD,e->loc);
_tmpE->topt=e->topt;
_tmpE->annot=e->annot;
return _tmpE;}else{
# 158
struct Cyc_Absyn_Exp*_tmpF=Cyc_Absyn_subscript_exp(_tmpC,_tmp24,e->loc);
_tmpF->topt=e->topt;
_tmpF->annot=e->annot;
return _tmpF;}}else{
# 164
if(_tmpD != 0){
struct Cyc_Absyn_Exp*_tmp10=Cyc_Absyn_subscript_exp(_tmp25,_tmpD,e->loc);
_tmp10->topt=e->topt;
_tmp10->annot=e->annot;
return _tmp10;}}
# 170
return 0;}case 15U: _LL5: _tmp26=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL6: {
# 173
struct Cyc_Absyn_Exp*_tmp11=Cyc_Tovc_box_free_vars_exp(env,_tmp26);
if(_tmp11 != 0){
struct Cyc_Absyn_Exp*_tmp12=Cyc_Absyn_address_exp(_tmp11,e->loc);
_tmp12->topt=e->topt;
_tmp12->annot=e->annot;
return _tmp12;}
# 180
return 0;}case 20U: _LL7: _tmp27=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL8: {
# 183
struct Cyc_Absyn_Exp*_tmp13=Cyc_Tovc_box_free_vars_exp(env,_tmp27);
if(_tmp13 != 0){
struct Cyc_Absyn_Exp*_tmp14=Cyc_Absyn_deref_exp(_tmp13,e->loc);
_tmp14->topt=e->topt;
_tmp14->annot=e->annot;
return _tmp14;}
# 190
return 0;}case 21U: _LL9: _tmp2B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp2A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp29=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp28=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1)->f4;_LLA: {
# 193
struct Cyc_Absyn_Exp*_tmp15=Cyc_Tovc_box_free_vars_exp(env,_tmp2B);
if(_tmp15 != 0){
struct Cyc_Absyn_Exp*_tmp16=({void*_tmp139=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=21U,_tmp17->f1=_tmp15,_tmp17->f2=_tmp2A,_tmp17->f3=_tmp29,_tmp17->f4=_tmp28;_tmp17;});Cyc_Absyn_new_exp(_tmp139,e->loc);});
_tmp16->topt=e->topt;
_tmp16->annot=e->annot;
return _tmp16;}
# 200
return 0;}case 22U: _LLB: _tmp2F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp2E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp2D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_tmp2C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1)->f4;_LLC: {
# 203
struct Cyc_Absyn_Exp*_tmp18=Cyc_Tovc_box_free_vars_exp(env,_tmp2F);
if(_tmp18 != 0){
struct Cyc_Absyn_Exp*_tmp19=({void*_tmp13A=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->tag=22U,_tmp1A->f1=_tmp18,_tmp1A->f2=_tmp2E,_tmp1A->f3=_tmp2D,_tmp1A->f4=_tmp2C;_tmp1A;});Cyc_Absyn_new_exp(_tmp13A,e->loc);});
_tmp19->topt=e->topt;
_tmp19->annot=e->annot;
return _tmp19;}
# 210
return 0;}case 0U: _LLD: _LLE:
# 213
 return 0;case 10U: _LLF: _tmp31=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp30=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL10:
# 215
{struct Cyc_Absyn_Exp*_tmp1B=Cyc_Tovc_box_free_vars_exp(env,*_tmp31);if(_tmp1B != 0)*_tmp31=_tmp1B;}
_tmp32=_tmp30;goto _LL12;case 3U: _LL11: _tmp32=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL12:
# 218
 for(0;_tmp32 != 0;_tmp32=_tmp32->tl){
struct Cyc_Absyn_Exp*_tmp1C=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_tmp32->hd);if(_tmp1C != 0)_tmp32->hd=(void*)_tmp1C;}
# 221
return 0;case 6U: _LL13: _tmp35=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp34=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_tmp33=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL14:
# 224
{struct Cyc_Absyn_Exp*_tmp1D=Cyc_Tovc_box_free_vars_exp(env,*_tmp33);if(_tmp1D != 0)*_tmp33=_tmp1D;}
_tmp37=_tmp35;_tmp36=_tmp34;goto _LL16;case 7U: _LL15: _tmp37=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp36=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL16:
 _tmp39=_tmp37;_tmp38=_tmp36;goto _LL18;case 8U: _LL17: _tmp39=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp38=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL18:
 _tmp3B=_tmp39;_tmp3A=_tmp38;goto _LL1A;case 9U: _LL19: _tmp3B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3A=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL1A:
 _tmp3D=_tmp3B;_tmp3C=_tmp3A;goto _LL1C;case 4U: _LL1B: _tmp3D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_tmp3C=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp1)->f3;_LL1C:
# 230
{struct Cyc_Absyn_Exp*_tmp1E=Cyc_Tovc_box_free_vars_exp(env,*_tmp3C);if(_tmp1E != 0)*_tmp3C=_tmp1E;}
_tmp3E=_tmp3D;goto _LL1E;case 12U: _LL1D: _tmp3E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL1E:
# 233
 _tmp3F=_tmp3E;goto _LL20;case 13U: _LL1F: _tmp3F=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL20:
 _tmp40=_tmp3F;goto _LL22;case 14U: _LL21: _tmp40=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL22:
 _tmp41=_tmp40;goto _LL24;case 18U: _LL23: _tmp41=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL24:
 _tmp42=_tmp41;goto _LL26;case 5U: _LL25: _tmp42=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL26:
# 238
{struct Cyc_Absyn_Exp*_tmp1F=Cyc_Tovc_box_free_vars_exp(env,*_tmp42);if(_tmp1F != 0)*_tmp42=_tmp1F;}
return 0;case 17U: _LL27: _LL28:
# 241
 goto _LL2A;case 19U: _LL29: _LL2A:
 goto _LL2C;case 32U: _LL2B: _LL2C:
 return 0;case 36U: _LL2D: _tmp43=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp1)->f2;_LL2E:
# 246
 for(0;_tmp43 != 0;_tmp43=_tmp43->tl){
struct Cyc_Absyn_Exp*_tmp20=Cyc_Tovc_box_free_vars_exp(env,(*((struct _tuple10*)_tmp43->hd)).f2);if(_tmp20 != 0)(*((struct _tuple10*)_tmp43->hd)).f2=_tmp20;}
# 249
return 0;default: _LL2F: _LL30:
# 251
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp13B=({const char*_tmp21="bad exp form in Tocv::box_free_vars_exp";_tag_dyneither(_tmp21,sizeof(char),40U);});_tmp22->f1=_tmp13B;});_tmp22;}));}_LL0:;}}
# 255
static void Cyc_Tovc_box_free_vars_stmt(struct Cyc_Tovc_BoxingEnv env,struct Cyc_Absyn_Stmt*s){
# 259
while(1){
void*_tmp44=s->r;void*_tmp45=_tmp44;struct Cyc_Absyn_Decl*_tmp71;struct Cyc_Absyn_Stmt*_tmp70;struct Cyc_Absyn_Exp**_tmp6F;struct Cyc_List_List*_tmp6E;struct Cyc_Absyn_Stmt*_tmp6D;struct Cyc_Absyn_Exp**_tmp6C;struct Cyc_Absyn_Exp**_tmp6B;struct Cyc_Absyn_Exp**_tmp6A;struct Cyc_Absyn_Exp**_tmp69;struct Cyc_Absyn_Stmt*_tmp68;struct Cyc_Absyn_Exp**_tmp67;struct Cyc_Absyn_Stmt*_tmp66;struct Cyc_Absyn_Exp**_tmp65;struct Cyc_Absyn_Stmt*_tmp64;struct Cyc_Absyn_Stmt*_tmp63;struct Cyc_Absyn_Stmt*_tmp62;struct Cyc_Absyn_Stmt*_tmp61;struct Cyc_Absyn_Exp**_tmp60;struct Cyc_Absyn_Exp**_tmp5F;struct Cyc_Absyn_Stmt*_tmp5E;switch(*((int*)_tmp45)){case 0U: _LL1: _LL2:
 return;case 6U: _LL3: _LL4:
# 263
({void*_tmp46=0U;({unsigned int _tmp13D=s->loc;struct _dyneither_ptr _tmp13C=({const char*_tmp47="break in eliminated statement expression";_tag_dyneither(_tmp47,sizeof(char),41U);});Cyc_Tcutil_warn(_tmp13D,_tmp13C,_tag_dyneither(_tmp46,sizeof(void*),0U));});});
return;case 7U: _LL5: _LL6:
# 266
({void*_tmp48=0U;({unsigned int _tmp13F=s->loc;struct _dyneither_ptr _tmp13E=({const char*_tmp49="continue in eliminated statement expression";_tag_dyneither(_tmp49,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp13F,_tmp13E,_tag_dyneither(_tmp48,sizeof(void*),0U));});});
# 269
return;case 8U: _LL7: _LL8:
# 271
({void*_tmp4A=0U;({unsigned int _tmp141=s->loc;struct _dyneither_ptr _tmp140=({const char*_tmp4B="goto in eliminated statement expression";_tag_dyneither(_tmp4B,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp141,_tmp140,_tag_dyneither(_tmp4A,sizeof(void*),0U));});});
return;case 13U: _LL9: _tmp5E=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LLA:
# 274
 s=_tmp5E;
continue;case 3U: _LLB: _tmp5F=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_LLC:
# 277
({void*_tmp4C=0U;({unsigned int _tmp143=s->loc;struct _dyneither_ptr _tmp142=({const char*_tmp4D="return in eliminated statement expression";_tag_dyneither(_tmp4D,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp143,_tmp142,_tag_dyneither(_tmp4C,sizeof(void*),0U));});});
# 279
if(*_tmp5F != 0){
struct Cyc_Absyn_Exp*_tmp4E=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(*_tmp5F));
if(_tmp4E != 0)*_tmp5F=_tmp4E;}
# 283
return;case 1U: _LLD: _tmp60=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_LLE:
# 285
{struct Cyc_Absyn_Exp*_tmp4F=Cyc_Tovc_box_free_vars_exp(env,*_tmp60);if(_tmp4F != 0)*_tmp60=_tmp4F;}
return;case 2U: _LLF: _tmp62=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp61=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL10:
# 288
 Cyc_Tovc_box_free_vars_stmt(env,_tmp62);
s=_tmp61;
continue;case 4U: _LL11: _tmp65=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp64=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_tmp63=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp45)->f3;_LL12:
# 292
{struct Cyc_Absyn_Exp*_tmp50=Cyc_Tovc_box_free_vars_exp(env,*_tmp65);if(_tmp50 != 0)*_tmp65=_tmp50;}
Cyc_Tovc_box_free_vars_stmt(env,_tmp64);
s=_tmp63;
continue;case 5U: _LL13: _tmp67=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f1).f1;_tmp66=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL14:
# 297
{struct Cyc_Absyn_Exp*_tmp51=Cyc_Tovc_box_free_vars_exp(env,*_tmp67);if(_tmp51 != 0)*_tmp67=_tmp51;}
s=_tmp66;
continue;case 9U: _LL15: _tmp6B=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp6A=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f1;_tmp69=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f3).f1;_tmp68=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp45)->f4;_LL16:
# 301
{struct Cyc_Absyn_Exp*_tmp52=Cyc_Tovc_box_free_vars_exp(env,*_tmp6B);if(_tmp52 != 0)*_tmp6B=_tmp52;}
{struct Cyc_Absyn_Exp*_tmp53=Cyc_Tovc_box_free_vars_exp(env,*_tmp6A);if(_tmp53 != 0)*_tmp6A=_tmp53;}
{struct Cyc_Absyn_Exp*_tmp54=Cyc_Tovc_box_free_vars_exp(env,*_tmp69);if(_tmp54 != 0)*_tmp69=_tmp54;}
s=_tmp68;
continue;case 14U: _LL17: _tmp6D=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp6C=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp45)->f2).f1;_LL18:
# 307
{struct Cyc_Absyn_Exp*_tmp55=Cyc_Tovc_box_free_vars_exp(env,*_tmp6C);if(_tmp55 != 0)*_tmp6C=_tmp55;}
s=_tmp6D;
continue;case 10U: _LL19: _tmp6F=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp6E=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL1A:
# 312
{struct Cyc_Absyn_Exp*_tmp56=Cyc_Tovc_box_free_vars_exp(env,*_tmp6F);if(_tmp56 != 0)*_tmp6F=_tmp56;}
for(0;_tmp6E != 0;_tmp6E=_tmp6E->tl){
Cyc_Tovc_box_free_vars_stmt(env,((struct Cyc_Absyn_Switch_clause*)_tmp6E->hd)->body);}
return;case 12U: _LL1B: _tmp71=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45)->f1;_tmp70=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp45)->f2;_LL1C:
# 317
{void*_tmp57=_tmp71->r;void*_tmp58=_tmp57;struct Cyc_Absyn_Vardecl*_tmp5B;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp58)->tag == 0U){_LL20: _tmp5B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp58)->f1;_LL21:
# 319
({struct Cyc_List_List*_tmp144=({struct Cyc_List_List*_tmp59=_region_malloc(env.rgn,sizeof(*_tmp59));_tmp59->hd=_tmp5B,_tmp59->tl=env.boundvars;_tmp59;});env.boundvars=_tmp144;});
({struct Cyc_Dict_Dict _tmp145=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _dyneither_ptr*))Cyc_Dict_rdelete_same)(env.varmap,(*_tmp5B->name).f2);env.varmap=_tmp145;});
if(_tmp5B->initializer != 0){
struct Cyc_Absyn_Exp*_tmp5A=Cyc_Tovc_box_free_vars_exp(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp5B->initializer));
if(_tmp5A != 0)_tmp5B->initializer=_tmp5A;}
# 325
s=_tmp70;
continue;}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 329
goto _LL1E;default: _LL1D: _LL1E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp146=({const char*_tmp5C="bad stmt after xlation to C";_tag_dyneither(_tmp5C,sizeof(char),28U);});_tmp5D->f1=_tmp146;});_tmp5D;}));}_LL0:;}}
# 340
static int Cyc_Tovc_stmt_to_fun_cmp(struct Cyc_Absyn_Vardecl*x,struct Cyc_Absyn_Vardecl*y){
return Cyc_Absyn_qvar_cmp(x->name,y->name);}
# 344
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_fun(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s,void*rettype){
# 347
struct Cyc_Set_Set*freevars=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Set_rempty)(env.rgn,Cyc_Tovc_stmt_to_fun_cmp);
struct Cyc_Tovc_BoxingEnv _tmp72=({struct Cyc_Tovc_BoxingEnv _tmp12F;_tmp12F.rgn=env.rgn,_tmp12F.all_locals=env.all_locals,_tmp12F.varmap=env.varmap,_tmp12F.boundvars=0,_tmp12F.freevars=& freevars;_tmp12F;});
Cyc_Tovc_box_free_vars_stmt(_tmp72,s);{
struct Cyc_List_List*params=0;
struct Cyc_List_List*args=0;
# 353
struct Cyc_Iter_Iter iter=((struct Cyc_Iter_Iter(*)(struct _RegionHandle*rgn,struct Cyc_Set_Set*s))Cyc_Set_make_iter)(Cyc_Core_heap_region,freevars);
# 355
struct Cyc_Absyn_Vardecl*_tmp73=({struct Cyc_Absyn_Vardecl*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->sc=Cyc_Absyn_Public,_tmp80->name=Cyc_Absyn_exn_name,_tmp80->varloc=0U,({struct Cyc_Absyn_Tqual _tmp147=Cyc_Absyn_empty_tqual(0U);_tmp80->tq=_tmp147;}),_tmp80->type=(void*)& Cyc_Absyn_VoidType_val,_tmp80->initializer=0,_tmp80->rgn=0,_tmp80->attributes=0,_tmp80->escapes=0;_tmp80;});
# 358
while(((int(*)(struct Cyc_Iter_Iter,struct Cyc_Absyn_Vardecl**))Cyc_Iter_next)(iter,& _tmp73)){
void*_tmp74=_tmp73->type;void*_tmp75=_tmp74;void*_tmp7C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp75)->tag == 8U){_LL1: _tmp7C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp75)->f1).elt_type;_LL2:
# 361
({struct Cyc_List_List*_tmp14B=({struct Cyc_List_List*_tmp77=_cycalloc(sizeof(*_tmp77));({struct _tuple8*_tmp14A=({struct _tuple8*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->f1=(*_tmp73->name).f2,_tmp76->f2=_tmp73->tq,({
void*_tmp149=({void*_tmp148=_tmp7C;Cyc_Absyn_cstar_typ(_tmp148,Cyc_Absyn_empty_tqual(0U));});_tmp76->f3=_tmp149;});_tmp76;});
# 361
_tmp77->hd=_tmp14A;}),_tmp77->tl=params;_tmp77;});params=_tmp14B;});
# 364
({struct Cyc_List_List*_tmp14D=({struct Cyc_List_List*_tmp78=_cycalloc(sizeof(*_tmp78));({struct Cyc_Absyn_Exp*_tmp14C=Cyc_Absyn_var_exp(_tmp73->name,0U);_tmp78->hd=_tmp14C;}),_tmp78->tl=args;_tmp78;});args=_tmp14D;});
goto _LL0;}else{_LL3: _LL4:
# 367
({struct Cyc_List_List*_tmp151=({struct Cyc_List_List*_tmp7A=_cycalloc(sizeof(*_tmp7A));({struct _tuple8*_tmp150=({struct _tuple8*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->f1=(*_tmp73->name).f2,_tmp79->f2=_tmp73->tq,({
void*_tmp14F=({void*_tmp14E=_tmp73->type;Cyc_Absyn_cstar_typ(_tmp14E,Cyc_Absyn_empty_tqual(0U));});_tmp79->f3=_tmp14F;});_tmp79;});
# 367
_tmp7A->hd=_tmp150;}),_tmp7A->tl=params;_tmp7A;});params=_tmp151;});
# 370
({struct Cyc_List_List*_tmp153=({struct Cyc_List_List*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct Cyc_Absyn_Exp*_tmp152=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(_tmp73->name,0U),0U);_tmp7B->hd=_tmp152;}),_tmp7B->tl=args;_tmp7B;});args=_tmp153;});
goto _LL0;}_LL0:;}{
# 373
struct _tuple0*funname=Cyc_Toc_temp_var();
({struct Cyc_List_List*_tmp157=({struct Cyc_List_List*_tmp7F=_cycalloc(sizeof(*_tmp7F));({struct Cyc_Absyn_Decl*_tmp156=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=1U,({struct Cyc_Absyn_Fndecl*_tmp155=({struct Cyc_Absyn_Fndecl*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->sc=Cyc_Absyn_Static,_tmp7D->is_inline=0,_tmp7D->name=funname,_tmp7D->tvs=0,_tmp7D->effect=0,({
struct Cyc_Absyn_Tqual _tmp154=Cyc_Absyn_empty_tqual(0U);_tmp7D->ret_tqual=_tmp154;}),_tmp7D->ret_type=rettype,_tmp7D->args=params,_tmp7D->c_varargs=0,_tmp7D->cyc_varargs=0,_tmp7D->rgn_po=0,_tmp7D->body=s,_tmp7D->cached_typ=0,_tmp7D->param_vardecls=0,_tmp7D->fn_vardecl=0,_tmp7D->attributes=0,_tmp7D->requires_clause=0,_tmp7D->requires_relns=0,_tmp7D->ensures_clause=0,_tmp7D->ensures_relns=0;_tmp7D;});
# 374
_tmp7E->f1=_tmp155;});_tmp7E;}),0U);_tmp7F->hd=_tmp156;}),_tmp7F->tl=*env.gen_ds;_tmp7F;});*env.gen_ds=_tmp157;});
# 380
return({struct Cyc_Absyn_Exp*_tmp158=Cyc_Absyn_var_exp(funname,0U);Cyc_Absyn_fncall_exp(_tmp158,args,0U);});};};}
# 383
static void Cyc_Tovc_apply_varmap(struct Cyc_Dict_Dict varmap,struct Cyc_Absyn_Exp*e){
void*_tmp81=e->r;void*_tmp82=_tmp81;struct Cyc_List_List*_tmpA2;struct Cyc_Absyn_Exp*_tmpA1;struct Cyc_Absyn_Exp*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_Absyn_Exp*_tmp99;struct Cyc_Absyn_Exp*_tmp98;struct Cyc_Absyn_Exp*_tmp97;struct Cyc_Absyn_Exp*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_Absyn_Exp*_tmp92;struct Cyc_Absyn_Exp*_tmp91;struct Cyc_Absyn_Exp*_tmp90;struct Cyc_Absyn_Exp*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_Absyn_Exp*_tmp8D;struct Cyc_Absyn_Exp*_tmp8C;struct Cyc_List_List*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_List_List*_tmp89;void*_tmp88;switch(*((int*)_tmp82)){case 1U: _LL1: _tmp88=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL2: {
# 386
struct _tuple0*_tmp83=Cyc_Absyn_binding2qvar(_tmp88);struct _tuple0*_tmp84=_tmp83;struct _dyneither_ptr*_tmp85;if(((((struct _tuple0*)_tmp84)->f1).Loc_n).tag == 4){_LL32: _tmp85=_tmp84->f2;_LL33:
# 388
 if(((int(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_member)(varmap,_tmp85))
({void*_tmp159=(((struct Cyc_Absyn_Exp*(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k))Cyc_Dict_lookup)(varmap,_tmp85))->r;e->r=_tmp159;});
return;}else{_LL34: _LL35:
 return;}_LL31:;}case 0U: _LL3: _LL4:
# 395
 return;case 10U: _LL5: _tmp8A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp89=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LL6:
# 397
 Cyc_Tovc_apply_varmap(varmap,_tmp8A);_tmp8B=_tmp89;goto _LL8;case 3U: _LL7: _tmp8B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LL8:
# 399
 for(0;_tmp8B != 0;_tmp8B=_tmp8B->tl){
Cyc_Tovc_apply_varmap(varmap,(struct Cyc_Absyn_Exp*)_tmp8B->hd);}
return;case 6U: _LL9: _tmp8E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp8D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_tmp8C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp82)->f3;_LLA:
# 404
 Cyc_Tovc_apply_varmap(varmap,_tmp8C);_tmp90=_tmp8E;_tmp8F=_tmp8D;goto _LLC;case 7U: _LLB: _tmp90=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp8F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LLC:
 _tmp92=_tmp90;_tmp91=_tmp8F;goto _LLE;case 8U: _LLD: _tmp92=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp91=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LLE:
 _tmp94=_tmp92;_tmp93=_tmp91;goto _LL10;case 23U: _LLF: _tmp94=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp93=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LL10:
 _tmp96=_tmp94;_tmp95=_tmp93;goto _LL12;case 9U: _LL11: _tmp96=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp95=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LL12:
 _tmp98=_tmp96;_tmp97=_tmp95;goto _LL14;case 4U: _LL13: _tmp98=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_tmp97=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp82)->f3;_LL14:
# 410
 Cyc_Tovc_apply_varmap(varmap,_tmp97);_tmp99=_tmp98;goto _LL16;case 12U: _LL15: _tmp99=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL16:
# 412
 _tmp9A=_tmp99;goto _LL18;case 13U: _LL17: _tmp9A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL18:
 _tmp9B=_tmp9A;goto _LL1A;case 14U: _LL19: _tmp9B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LL1A:
 _tmp9C=_tmp9B;goto _LL1C;case 15U: _LL1B: _tmp9C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL1C:
 _tmp9D=_tmp9C;goto _LL1E;case 18U: _LL1D: _tmp9D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL1E:
 _tmp9E=_tmp9D;goto _LL20;case 20U: _LL1F: _tmp9E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL20:
 _tmp9F=_tmp9E;goto _LL22;case 21U: _LL21: _tmp9F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL22:
 _tmpA0=_tmp9F;goto _LL24;case 22U: _LL23: _tmpA0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL24:
 _tmpA1=_tmpA0;goto _LL26;case 5U: _LL25: _tmpA1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp82)->f1;_LL26:
# 421
 Cyc_Tovc_apply_varmap(varmap,_tmpA1);
return;case 17U: _LL27: _LL28:
# 424
 goto _LL2A;case 19U: _LL29: _LL2A:
 goto _LL2C;case 32U: _LL2B: _LL2C:
 return;case 36U: _LL2D: _tmpA2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp82)->f2;_LL2E:
# 429
 for(0;_tmpA2 != 0;_tmpA2=_tmpA2->tl){
Cyc_Tovc_apply_varmap(varmap,(*((struct _tuple10*)_tmpA2->hd)).f2);}
return;default: _LL2F: _LL30:
# 433
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp15A=({const char*_tmp86="bad exp form in Tocv::apply_varmap";_tag_dyneither(_tmp86,sizeof(char),35U);});_tmp87->f1=_tmp15A;});_tmp87;}));}_LL0:;}
# 437
static struct Cyc_Absyn_Exp*Cyc_Tovc_stmt_to_exp(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
void*_tmpA3=s->r;void*_tmpA4=_tmpA3;struct Cyc_Absyn_Decl*_tmpBF;struct Cyc_Absyn_Stmt*_tmpBE;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_Absyn_Stmt*_tmpBC;struct Cyc_Absyn_Stmt*_tmpBB;struct Cyc_Absyn_Stmt*_tmpBA;struct Cyc_Absyn_Stmt*_tmpB9;struct Cyc_Absyn_Exp*_tmpB8;switch(*((int*)_tmpA4)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_true_exp(0U);case 1U: _LL3: _tmpB8=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA4)->f1;_LL4:
# 441
 Cyc_Tovc_apply_varmap(env.varmap,_tmpB8);
return _tmpB8;case 2U: _LL5: _tmpBA=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA4)->f1;_tmpB9=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA4)->f2;_LL6: {
# 445
struct Cyc_Absyn_Exp*_tmpA5=Cyc_Tovc_stmt_to_exp(env,_tmpBA);
struct Cyc_Absyn_Exp*_tmpA6=Cyc_Tovc_stmt_to_exp(env,_tmpB9);
return Cyc_Absyn_seq_exp(_tmpA5,_tmpA6,0U);}case 4U: _LL7: _tmpBD=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpA4)->f1;_tmpBC=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpA4)->f2;_tmpBB=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpA4)->f3;_LL8:
# 449
 Cyc_Tovc_apply_varmap(env.varmap,_tmpBD);{
# 451
struct Cyc_Absyn_Exp*_tmpA7=Cyc_Tovc_stmt_to_exp(env,_tmpBC);
struct Cyc_Absyn_Exp*_tmpA8=Cyc_Tovc_stmt_to_exp(env,_tmpBB);
return Cyc_Absyn_conditional_exp(_tmpBD,_tmpA7,_tmpA8,0U);};case 12U: _LL9: _tmpBF=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA4)->f1;_tmpBE=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA4)->f2;_LLA: {
# 455
void*_tmpA9=_tmpBF->r;void*_tmpAA=_tmpA9;struct Cyc_Absyn_Vardecl*_tmpB7;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpAA)->tag == 0U){_LLE: _tmpB7=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpAA)->f1;_LLF: {
# 458
struct _dyneither_ptr*_tmpAB=(*_tmpB7->name).f2;
struct _tuple0*_tmpAC=Cyc_Toc_temp_var();
_tmpB7->name=_tmpAC;
({struct Cyc_Dict_Dict _tmp15D=({struct Cyc_Dict_Dict _tmp15C=env.varmap;struct _dyneither_ptr*_tmp15B=_tmpAB;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _dyneither_ptr*k,struct Cyc_Absyn_Exp*v))Cyc_Dict_insert)(_tmp15C,_tmp15B,Cyc_Absyn_var_exp(_tmpAC,0U));});env.varmap=_tmp15D;});{
struct Cyc_List_List*_tmpAD=0;
if(_tmpB7->initializer != 0){
Cyc_Tovc_apply_varmap(env.varmap,(struct Cyc_Absyn_Exp*)_check_null(_tmpB7->initializer));{
void*_tmpAE=((struct Cyc_Absyn_Exp*)_check_null(_tmpB7->initializer))->r;void*_tmpAF=_tmpAE;struct Cyc_List_List*_tmpB2;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAF)->tag == 36U){_LL13: _tmpB2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAF)->f2;_LL14: {
# 477 "tovc.cyc"
struct Cyc_Absyn_Exp*_tmpB0=Cyc_Absyn_var_exp(_tmpB7->name,0U);
{unsigned int i=0U;for(0;_tmpB2 != 0;(_tmpB2=_tmpB2->tl,++ i)){
({struct Cyc_List_List*_tmp161=({struct Cyc_List_List*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct Cyc_Absyn_Exp*_tmp160=({struct Cyc_Absyn_Exp*_tmp15F=({struct Cyc_Absyn_Exp*_tmp15E=_tmpB0;Cyc_Absyn_subscript_exp(_tmp15E,Cyc_Absyn_uint_exp(i,0U),0U);});Cyc_Absyn_assign_exp(_tmp15F,(*((struct _tuple10*)_tmpB2->hd)).f2,0U);});_tmpB1->hd=_tmp160;}),_tmpB1->tl=_tmpAD;_tmpB1;});_tmpAD=_tmp161;});}}
# 483
_tmpB7->initializer=0;
goto _LL12;}}else{_LL15: _LL16:
 goto _LL12;}_LL12:;};}
# 488
({struct Cyc_List_List*_tmp162=({struct Cyc_List_List*_tmpB3=_region_malloc(env.rgn,sizeof(*_tmpB3));_tmpB3->hd=_tmpB7,_tmpB3->tl=env.all_locals;_tmpB3;});env.all_locals=_tmp162;});
# 490
({void*_tmp164=({struct Cyc_Absyn_Decl*_tmp163=_tmpBF;Cyc_Absyn_decl_stmt(_tmp163,Cyc_Absyn_new_stmt((env.encloser)->r,0U),0U);})->r;(env.encloser)->r=_tmp164;});{
struct Cyc_Absyn_Exp*_tmpB4=Cyc_Tovc_stmt_to_exp(env,_tmpBE);
if(_tmpB7->initializer != 0){
({struct Cyc_Absyn_Exp*_tmp167=({struct Cyc_Absyn_Exp*_tmp166=({struct Cyc_Absyn_Exp*_tmp165=Cyc_Absyn_var_exp(_tmpB7->name,0U);Cyc_Absyn_assign_exp(_tmp165,(struct Cyc_Absyn_Exp*)_check_null(_tmpB7->initializer),0U);});Cyc_Absyn_seq_exp(_tmp166,_tmpB4,0U);});_tmpB4=_tmp167;});
# 495
_tmpB7->initializer=0;}
# 497
for(0;_tmpAD != 0;_tmpAD=_tmpAD->tl){
({struct Cyc_Absyn_Exp*_tmp168=Cyc_Absyn_seq_exp((struct Cyc_Absyn_Exp*)_tmpAD->hd,_tmpB4,0U);_tmpB4=_tmp168;});}
return _tmpB4;};};}}else{_LL10: _LL11:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp169=({const char*_tmpB5="bad local decl in Tovc::stmt_to_exp";_tag_dyneither(_tmpB5,sizeof(char),36U);});_tmpB6->f1=_tmp169;});_tmpB6;}));}_LLD:;}default: _LLB: _LLC:
# 503
 return Cyc_Tovc_stmt_to_fun(env,s,(void*)& Cyc_Absyn_VoidType_val);}_LL0:;}
# 507
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s);
# 509
static void Cyc_Tovc_exp_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Exp*e){
struct _dyneither_ptr bad_form=({const char*_tmpF2="";_tag_dyneither(_tmpF2,sizeof(char),1U);});
{void*_tmpC0=e->r;void*_tmpC1=_tmpC0;struct Cyc_Absyn_Stmt*_tmpEC;struct Cyc_List_List*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_List_List*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_List_List*_tmpD1;switch(*((int*)_tmpC1)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _LL4:
 return;case 10U: _LL5: _tmpD2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_tmpD1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL6:
# 516
 Cyc_Tovc_exp_to_vc(env,_tmpD2);_tmpD3=_tmpD1;goto _LL8;case 3U: _LL7: _tmpD3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL8:
# 518
 for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_tmpD3->hd);}
return;case 6U: _LL9: _tmpD6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_tmpD5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_tmpD4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpC1)->f3;_LLA:
# 523
 Cyc_Tovc_exp_to_vc(env,_tmpD4);_tmpD8=_tmpD6;_tmpD7=_tmpD5;goto _LLC;case 7U: _LLB: _tmpD8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_tmpD7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LLC:
 _tmpDA=_tmpD8;_tmpD9=_tmpD7;goto _LLE;case 8U: _LLD: _tmpDA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_tmpD9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LLE:
 _tmpDC=_tmpDA;_tmpDB=_tmpD9;goto _LL10;case 23U: _LLF: _tmpDC=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_tmpDB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL10:
 _tmpDE=_tmpDC;_tmpDD=_tmpDB;goto _LL12;case 9U: _LL11: _tmpDE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_tmpDD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL12:
 _tmpE0=_tmpDE;_tmpDF=_tmpDD;goto _LL14;case 4U: _LL13: _tmpE0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_tmpDF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpC1)->f3;_LL14:
# 529
 Cyc_Tovc_exp_to_vc(env,_tmpDF);_tmpE1=_tmpE0;goto _LL16;case 41U: _LL15: _tmpE1=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL16:
# 531
 _tmpE2=_tmpE1;goto _LL18;case 12U: _LL17: _tmpE2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL18:
 _tmpE3=_tmpE2;goto _LL1A;case 13U: _LL19: _tmpE3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL1A:
 _tmpE4=_tmpE3;goto _LL1C;case 14U: _LL1B: _tmpE4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL1C:
 _tmpE5=_tmpE4;goto _LL1E;case 15U: _LL1D: _tmpE5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL1E:
 _tmpE6=_tmpE5;goto _LL20;case 18U: _LL1F: _tmpE6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL20:
 _tmpE7=_tmpE6;goto _LL22;case 20U: _LL21: _tmpE7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL22:
 _tmpE8=_tmpE7;goto _LL24;case 21U: _LL23: _tmpE8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL24:
 _tmpE9=_tmpE8;goto _LL26;case 22U: _LL25: _tmpE9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL26:
 _tmpEA=_tmpE9;goto _LL28;case 5U: _LL27: _tmpEA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL28:
# 541
 Cyc_Tovc_exp_to_vc(env,_tmpEA);
return;case 17U: _LL29: _LL2A:
# 544
 goto _LL2C;case 19U: _LL2B: _LL2C:
 goto _LL2E;case 33U: _LL2D: _LL2E:
 goto _LL30;case 32U: _LL2F: _LL30:
 return;case 36U: _LL31: _tmpEB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpC1)->f2;_LL32:
# 553
 for(0;_tmpEB != 0;_tmpEB=_tmpEB->tl){
Cyc_Tovc_exp_to_vc(env,(*((struct _tuple10*)_tmpEB->hd)).f2);}
return;case 37U: _LL33: _tmpEC=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpC1)->f1;_LL34:
# 561
 Cyc_Tovc_stmt_to_vc(env,_tmpEC);
# 563
({void*_tmp16A=(Cyc_Tovc_stmt_to_exp(env,_tmpEC))->r;e->r=_tmp16A;});
return;case 40U: _LL35: _LL36:
# 566
 return;case 2U: _LL37: _LL38:
# 568
({struct _dyneither_ptr _tmp16B=({const char*_tmpC2="Pragma_e";_tag_dyneither(_tmpC2,sizeof(char),9U);});bad_form=_tmp16B;});goto _LL0;case 11U: _LL39: _LL3A:
({struct _dyneither_ptr _tmp16C=({const char*_tmpC3="Throw_e";_tag_dyneither(_tmpC3,sizeof(char),8U);});bad_form=_tmp16C;});goto _LL0;case 16U: _LL3B: _LL3C:
({struct _dyneither_ptr _tmp16D=({const char*_tmpC4="New_e";_tag_dyneither(_tmpC4,sizeof(char),6U);});bad_form=_tmp16D;});goto _LL0;case 24U: _LL3D: _LL3E:
({struct _dyneither_ptr _tmp16E=({const char*_tmpC5="Tuple_e";_tag_dyneither(_tmpC5,sizeof(char),8U);});bad_form=_tmp16E;});goto _LL0;case 25U: _LL3F: _LL40:
({struct _dyneither_ptr _tmp16F=({const char*_tmpC6="CompoundLit_e";_tag_dyneither(_tmpC6,sizeof(char),14U);});bad_form=_tmp16F;});goto _LL0;case 26U: _LL41: _LL42:
({struct _dyneither_ptr _tmp170=({const char*_tmpC7="Array_e";_tag_dyneither(_tmpC7,sizeof(char),8U);});bad_form=_tmp170;});goto _LL0;case 27U: _LL43: _LL44:
({struct _dyneither_ptr _tmp171=({const char*_tmpC8="Comprehension_e";_tag_dyneither(_tmpC8,sizeof(char),16U);});bad_form=_tmp171;});goto _LL0;case 28U: _LL45: _LL46:
({struct _dyneither_ptr _tmp172=({const char*_tmpC9="ComprehensionNoinit_e";_tag_dyneither(_tmpC9,sizeof(char),22U);});bad_form=_tmp172;});goto _LL0;case 29U: _LL47: _LL48:
({struct _dyneither_ptr _tmp173=({const char*_tmpCA="Aggregate_e";_tag_dyneither(_tmpCA,sizeof(char),12U);});bad_form=_tmp173;});goto _LL0;case 30U: _LL49: _LL4A:
({struct _dyneither_ptr _tmp174=({const char*_tmpCB="AnonStruct_e";_tag_dyneither(_tmpCB,sizeof(char),13U);});bad_form=_tmp174;});goto _LL0;case 31U: _LL4B: _LL4C:
# 579
({struct _dyneither_ptr _tmp175=({const char*_tmpCC="Datatype_e";_tag_dyneither(_tmpCC,sizeof(char),11U);});bad_form=_tmp175;});goto _LL0;case 34U: _LL4D: _LL4E:
({struct _dyneither_ptr _tmp176=({const char*_tmpCD="Malloc_e";_tag_dyneither(_tmpCD,sizeof(char),9U);});bad_form=_tmp176;});goto _LL0;case 35U: _LL4F: _LL50:
({struct _dyneither_ptr _tmp177=({const char*_tmpCE="Swap_e";_tag_dyneither(_tmpCE,sizeof(char),7U);});bad_form=_tmp177;});goto _LL0;case 38U: _LL51: _LL52:
({struct _dyneither_ptr _tmp178=({const char*_tmpCF="Tagcheck_e";_tag_dyneither(_tmpCF,sizeof(char),11U);});bad_form=_tmp178;});goto _LL0;default: _LL53: _LL54:
({struct _dyneither_ptr _tmp179=({const char*_tmpD0="Valueof_e";_tag_dyneither(_tmpD0,sizeof(char),10U);});bad_form=_tmp179;});goto _LL0;}_LL0:;}
# 585
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp17C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpEF=({struct Cyc_String_pa_PrintArg_struct _tmp131;_tmp131.tag=0U,_tmp131.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)bad_form);_tmp131;});struct Cyc_String_pa_PrintArg_struct _tmpF0=({struct Cyc_String_pa_PrintArg_struct _tmp130;_tmp130.tag=0U,({struct _dyneither_ptr _tmp17A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp130.f1=_tmp17A;});_tmp130;});void*_tmpED[2U];_tmpED[0]=& _tmpEF,_tmpED[1]=& _tmpF0;({struct _dyneither_ptr _tmp17B=({const char*_tmpEE="bad exp form %s (exp=|%s|) after xlation to C";_tag_dyneither(_tmpEE,sizeof(char),46U);});Cyc_aprintf(_tmp17B,_tag_dyneither(_tmpED,sizeof(void*),2U));});});_tmpF1->f1=_tmp17C;});_tmpF1;}));}
# 589
static void Cyc_Tovc_stmt_to_vc(struct Cyc_Tovc_ToExpEnv env,struct Cyc_Absyn_Stmt*s){
# 591
while(1){
env.encloser=s;{
void*_tmpF3=s->r;void*_tmpF4=_tmpF3;struct Cyc_Absyn_Decl*_tmp118;struct Cyc_Absyn_Stmt*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_List_List*_tmp115;struct Cyc_Absyn_Stmt*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Stmt*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Stmt*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Stmt*_tmp10B;struct Cyc_Absyn_Stmt*_tmp10A;struct Cyc_Absyn_Stmt*_tmp109;struct Cyc_Absyn_Stmt*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Stmt*_tmp105;switch(*((int*)_tmpF4)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 goto _LL8;case 8U: _LL7: _LL8:
 return;case 13U: _LL9: _tmp105=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2;_LLA:
# 599
 s=_tmp105;
continue;case 3U: _LLB: _tmp106=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_LLC:
# 602
 if(_tmp106 == 0)
return;
_tmp107=_tmp106;goto _LLE;case 1U: _LLD: _tmp107=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_LLE:
# 606
 Cyc_Tovc_exp_to_vc(env,_tmp107);
return;case 2U: _LLF: _tmp109=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_tmp108=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2;_LL10:
# 609
 Cyc_Tovc_stmt_to_vc(env,_tmp109);
s=_tmp108;
continue;case 4U: _LL11: _tmp10C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_tmp10B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2;_tmp10A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmpF4)->f3;_LL12:
# 613
 Cyc_Tovc_exp_to_vc(env,_tmp10C);
Cyc_Tovc_stmt_to_vc(env,_tmp10B);
s=_tmp10A;
continue;case 5U: _LL13: _tmp10E=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1).f1;_tmp10D=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2;_LL14:
# 618
 Cyc_Tovc_exp_to_vc(env,_tmp10E);
s=_tmp10D;
continue;case 9U: _LL15: _tmp112=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_tmp111=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2).f1;_tmp110=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpF4)->f3).f1;_tmp10F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmpF4)->f4;_LL16:
# 622
 Cyc_Tovc_exp_to_vc(env,_tmp112);
Cyc_Tovc_exp_to_vc(env,_tmp111);
Cyc_Tovc_exp_to_vc(env,_tmp110);
s=_tmp10F;
continue;case 14U: _LL17: _tmp114=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_tmp113=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2).f1;_LL18:
# 628
 Cyc_Tovc_exp_to_vc(env,_tmp113);
s=_tmp114;
continue;case 10U: _LL19: _tmp116=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_tmp115=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2;_LL1A:
# 634
 Cyc_Tovc_exp_to_vc(env,_tmp116);
for(0;_tmp115 != 0;_tmp115=_tmp115->tl){
Cyc_Tovc_stmt_to_vc(env,((struct Cyc_Absyn_Switch_clause*)_tmp115->hd)->body);}
return;case 12U: _LL1B: _tmp118=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpF4)->f1;_tmp117=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpF4)->f2;_LL1C:
# 639
{void*_tmpF5=_tmp118->r;void*_tmpF6=_tmpF5;struct Cyc_Absyn_Vardecl*_tmp102;if(((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpF6)->tag == 0U){_LL20: _tmp102=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpF6)->f1;_LL21:
# 641
 if(Cyc_Tovc_elim_array_initializers){
# 646
void*_tmpF7=Cyc_Tcutil_compress(_tmp102->type);void*_tmpF8=_tmpF7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF8)->tag == 8U){_LL25: _LL26:
# 648
 if(_tmp102->initializer != 0){
void*_tmpF9=((struct Cyc_Absyn_Exp*)_check_null(_tmp102->initializer))->r;void*_tmpFA=_tmpF9;struct Cyc_List_List*_tmp100;if(((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpFA)->tag == 36U){_LL2A: _tmp100=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpFA)->f2;_LL2B:
# 651
 _tmp102->initializer=0;{
# 653
struct Cyc_Absyn_Exp*arr=Cyc_Absyn_var_exp(_tmp102->name,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_new_stmt(_tmp117->r,0U);
{unsigned int i=0U;for(0;_tmp100 != 0;(_tmp100=_tmp100->tl,++ i)){
({struct Cyc_Absyn_Stmt*_tmp180=({struct Cyc_Absyn_Stmt*_tmp17F=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp17E=({struct Cyc_Absyn_Exp*_tmp17D=arr;Cyc_Absyn_subscript_exp(_tmp17D,
Cyc_Absyn_uint_exp(i,0U),0U);});
# 656
Cyc_Absyn_assign_exp(_tmp17E,(*((struct _tuple10*)_tmp100->hd)).f2,0U);}),0U);Cyc_Absyn_seq_stmt(_tmp17F,s3,0U);});s3=_tmp180;});}}
# 661
_tmp117->r=s3->r;
goto _LL29;};}else{_LL2C: _LL2D: {
# 664
struct _dyneither_ptr _tmpFB=({struct Cyc_String_pa_PrintArg_struct _tmpFF=({struct Cyc_String_pa_PrintArg_struct _tmp132;_tmp132.tag=0U,({
struct _dyneither_ptr _tmp181=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp132.f1=_tmp181;});_tmp132;});void*_tmpFD[1U];_tmpFD[0]=& _tmpFF;({struct _dyneither_ptr _tmp182=({const char*_tmpFE="|%s|: array type bad initializer";_tag_dyneither(_tmpFE,sizeof(char),33U);});Cyc_aprintf(_tmp182,_tag_dyneither(_tmpFD,sizeof(void*),1U));});});
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->tag=Cyc_Core_Impossible,_tmpFC->f1=(struct _dyneither_ptr)_tmpFB;_tmpFC;}));}}_LL29:;}
# 668
goto _LL24;}else{_LL27: _LL28:
 goto _LL24;}_LL24:;}
# 672
if(_tmp102->initializer != 0)
Cyc_Tovc_exp_to_vc(env,(struct Cyc_Absyn_Exp*)_check_null(_tmp102->initializer));
({struct Cyc_List_List*_tmp183=({struct Cyc_List_List*_tmp101=_region_malloc(env.rgn,sizeof(*_tmp101));_tmp101->hd=_tmp102,_tmp101->tl=env.all_locals;_tmp101;});env.all_locals=_tmp183;});
s=_tmp117;
continue;}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 679
goto _LL1E;default: _LL1D: _LL1E:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp184=({const char*_tmp103="bad stmt after xlation to C";_tag_dyneither(_tmp103,sizeof(char),28U);});_tmp104->f1=_tmp184;});_tmp104;}));}_LL0:;};}}
# 685
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*old_ds){
struct Cyc_List_List*_tmp119=0;
for(0;old_ds != 0;old_ds=old_ds->tl){
struct Cyc_Absyn_Decl*_tmp11A=(struct Cyc_Absyn_Decl*)old_ds->hd;
void*_tmp11B=_tmp11A->r;void*_tmp11C=_tmp11B;struct Cyc_List_List*_tmp12D;struct Cyc_List_List*_tmp12C;struct Cyc_List_List*_tmp12B;struct Cyc_List_List*_tmp12A;struct Cyc_Absyn_Fndecl*_tmp129;switch(*((int*)_tmp11C)){case 1U: _LL1: _tmp129=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp11C)->f1;_LL2:
# 694
 _tmp129->cached_typ=0;
# 701
if(_tmp129->param_vardecls != 0){
struct _RegionHandle _tmp11D=_new_region("rgn");struct _RegionHandle*rgn=& _tmp11D;_push_region(rgn);
{struct Cyc_List_List**_tmp11E=({struct Cyc_List_List**_tmp125=_region_malloc(rgn,sizeof(*_tmp125));*_tmp125=0;_tmp125;});
if(_tmp129->param_vardecls == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp121=({struct Cyc_String_pa_PrintArg_struct _tmp133;_tmp133.tag=0U,_tmp133.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp129->name).f2);_tmp133;});void*_tmp11F[1U];_tmp11F[0]=& _tmp121;({struct Cyc___cycFILE*_tmp186=Cyc_stderr;struct _dyneither_ptr _tmp185=({const char*_tmp120="No param vardecls for function %s\n";_tag_dyneither(_tmp120,sizeof(char),35U);});Cyc_fprintf(_tmp186,_tmp185,_tag_dyneither(_tmp11F,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);}
# 708
({struct Cyc_Tovc_ToExpEnv _tmp189=({struct Cyc_Tovc_ToExpEnv _tmp134;_tmp134.rgn=rgn,({
struct Cyc_List_List*_tmp188=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rcopy)(rgn,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp129->param_vardecls))->v);_tmp134.all_locals=_tmp188;}),({
struct Cyc_Dict_Dict _tmp187=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*)))Cyc_Dict_rempty)(rgn,Cyc_strptrcmp);_tmp134.varmap=_tmp187;}),_tmp134.encloser=_tmp129->body,_tmp134.gen_ds=_tmp11E;_tmp134;});
# 708
Cyc_Tovc_stmt_to_vc(_tmp189,_tmp129->body);});
# 714
if(*_tmp11E != 0){
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->sc=_tmp129->sc,_tmp124->name=_tmp129->name,_tmp124->varloc=0U,({struct Cyc_Absyn_Tqual _tmp18B=Cyc_Absyn_const_tqual(0U);_tmp124->tq=_tmp18B;}),({
void*_tmp18A=Cyc_Tcutil_fndecl2typ(_tmp129);_tmp124->type=_tmp18A;}),_tmp124->initializer=0,_tmp124->rgn=0,_tmp124->attributes=_tmp129->attributes,_tmp124->escapes=0;_tmp124;});
({struct Cyc_List_List*_tmp18D=({struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));({struct Cyc_Absyn_Decl*_tmp18C=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=0U,_tmp122->f1=vd;_tmp122;}),0U);_tmp123->hd=_tmp18C;}),_tmp123->tl=_tmp119;_tmp123;});_tmp119=_tmp18D;});
# 719
({struct Cyc_List_List*_tmp18E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(*_tmp11E,_tmp119);_tmp119=_tmp18E;});}}
# 703
;_pop_region(rgn);}
# 723
goto _LL4;case 0U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 goto _LL12;case 8U: _LL11: _LL12:
# 732
({struct Cyc_List_List*_tmp18F=({struct Cyc_List_List*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->hd=_tmp11A,_tmp126->tl=_tmp119;_tmp126;});_tmp119=_tmp18F;});
goto _LL0;case 9U: _LL13: _tmp12A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp11C)->f2;_LL14:
 _tmp12B=_tmp12A;goto _LL16;case 10U: _LL15: _tmp12B=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp11C)->f2;_LL16:
 _tmp12C=_tmp12B;goto _LL18;case 11U: _LL17: _tmp12C=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp11C)->f1;_LL18:
 _tmp12D=_tmp12C;goto _LL1A;case 12U: _LL19: _tmp12D=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp11C)->f1;_LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 740
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp190=({const char*_tmp127="nested translation unit after translation to C";_tag_dyneither(_tmp127,sizeof(char),47U);});_tmp128->f1=_tmp190;});_tmp128;}));}_LL0:;}
# 743
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp119);}
