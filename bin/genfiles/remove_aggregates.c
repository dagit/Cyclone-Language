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
struct _RegionPage; // abstract -- defined in runtime_memory.c

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
#define _cycalloc(n)            (GC_malloc(n)          ? : _throw_badalloc())
#define _cycalloc_atomic(n)     (GC_malloc_atomic(n)   ? : _throw_badalloc())
#define _cyccalloc(n,s)         (GC_calloc(n,s)        ? : _throw_badalloc())
#define _cyccalloc_atomic(n,s)  (GC_calloc_atomic(n,s) ? : _throw_badalloc())

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
#endif
#endif
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 954 "absyn.h"
extern void*Cyc_Absyn_uint_typ;
# 995
void*Cyc_Absyn_strctq(struct _tuple0*name);
void*Cyc_Absyn_unionq_typ(struct _tuple0*name);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple0*,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_lt_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1086
struct Cyc_Absyn_Stmt*Cyc_Absyn_for_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple0*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1178
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 67 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);struct Cyc_Hashtable_Table;
# 38 "toc.h"
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple0*Cyc_Toc_temp_var();extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 49
void Cyc_Toc_finish();
# 26 "remove_aggregates.h"
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*decls);
# 49 "remove_aggregates.cyc"
static void Cyc_RemoveAggrs_massage_toplevel_aggr(struct Cyc_Absyn_Exp*e){
if(e == 0)
return;}
# 57
static struct Cyc_Absyn_Exp*Cyc_RemoveAggrs_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp2;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 20U){_LL1: _tmp2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp2,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}
# 66
enum Cyc_RemoveAggrs_ExpContext{Cyc_RemoveAggrs_Initializer  = 0U,Cyc_RemoveAggrs_NewDest  = 1U,Cyc_RemoveAggrs_AggrField  = 2U,Cyc_RemoveAggrs_Other  = 3U};struct Cyc_RemoveAggrs_Env{enum Cyc_RemoveAggrs_ExpContext ctxt;struct Cyc_Absyn_Exp*dest;};
# 77
static struct Cyc_RemoveAggrs_Env Cyc_RemoveAggrs_other_env={Cyc_RemoveAggrs_Other,0};
# 79
enum Cyc_RemoveAggrs_ExpResult{Cyc_RemoveAggrs_WasArray  = 0U,Cyc_RemoveAggrs_OtherRes  = 1U};struct Cyc_RemoveAggrs_Result{enum Cyc_RemoveAggrs_ExpResult res;};
# 88
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env,struct Cyc_Absyn_Exp*);
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s);
# 91
static void Cyc_RemoveAggrs_noarray_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
struct Cyc_RemoveAggrs_Result _tmp3=Cyc_RemoveAggrs_remove_aggrs_exp(env,e);struct Cyc_RemoveAggrs_Result _tmp4=_tmp3;enum Cyc_RemoveAggrs_ExpResult _tmp8;_LL1: _tmp8=_tmp4.res;_LL2:;
if(_tmp8 != Cyc_RemoveAggrs_OtherRes)
({struct Cyc_String_pa_PrintArg_struct _tmp7=({struct Cyc_String_pa_PrintArg_struct _tmp83;_tmp83.tag=0U,({
struct _dyneither_ptr _tmp8C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp83.f1=_tmp8C;});_tmp83;});void*_tmp5[1U];_tmp5[0]=& _tmp7;({struct _dyneither_ptr _tmp8D=({const char*_tmp6="remove_aggrs_exp -- unexpected array or comprehension: %s";_tag_dyneither(_tmp6,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp8D,_tag_dyneither(_tmp5,sizeof(void*),1U));});});}struct _tuple10{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 98
static struct Cyc_RemoveAggrs_Result Cyc_RemoveAggrs_remove_aggrs_exp(struct Cyc_RemoveAggrs_Env env,struct Cyc_Absyn_Exp*e){
enum Cyc_RemoveAggrs_ExpResult res=Cyc_RemoveAggrs_OtherRes;
int did_assign=0;
# 102
{void*_tmp9=e->r;void*_tmpA=_tmp9;struct Cyc_Absyn_Stmt*_tmp53;struct Cyc_Absyn_Exp*_tmp52;struct Cyc_Absyn_Exp*_tmp51;struct Cyc_Absyn_Exp*_tmp50;struct Cyc_Absyn_Exp*_tmp4F;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_Absyn_Exp*_tmp4D;struct Cyc_Absyn_Exp*_tmp4C;struct Cyc_Absyn_Exp*_tmp4B;struct Cyc_Absyn_Exp*_tmp4A;struct Cyc_Absyn_Exp*_tmp49;struct Cyc_Absyn_Exp*_tmp48;struct Cyc_Absyn_Exp*_tmp47;struct Cyc_Absyn_Exp*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_List_List*_tmp43;struct Cyc_Absyn_Exp*_tmp42;struct Cyc_List_List*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct Cyc_Absyn_Exp*_tmp3F;struct Cyc_Absyn_Exp*_tmp3E;struct Cyc_Absyn_Exp*_tmp3D;struct Cyc_Absyn_Exp*_tmp3C;struct Cyc_Absyn_Exp*_tmp3B;struct Cyc_Absyn_Exp*_tmp3A;struct Cyc_Absyn_Exp*_tmp39;struct Cyc_Absyn_Vardecl*_tmp38;struct Cyc_Absyn_Exp*_tmp37;struct Cyc_Absyn_Exp*_tmp36;int _tmp35;struct Cyc_List_List*_tmp34;struct _tuple0*_tmp33;struct Cyc_List_List*_tmp32;struct Cyc_Absyn_Aggrdecl*_tmp31;struct Cyc_Absyn_Exp*_tmp30;switch(*((int*)_tmpA)){case 0U: _LL1: _LL2:
 goto _LL4;case 17U: _LL3: _LL4:
 goto _LL6;case 19U: _LL5: _LL6:
 goto _LL8;case 33U: _LL7: _LL8:
 goto _LLA;case 32U: _LL9: _LLA:
 goto _LLC;case 40U: _LLB: _LLC:
 goto _LLE;case 1U: _LLD: _LLE:
 goto _LL0;case 16U: _LLF: _tmp30=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL10:
# 112
{void*_tmpB=e->annot;void*_tmpC=_tmpB;struct Cyc_Absyn_Exp*_tmpF;if(((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmpC)->tag == Cyc_Toc_Dest){_LL40: _tmpF=((struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*)_tmpC)->f1;_LL41:
# 114
({struct Cyc_RemoveAggrs_Env _tmp8E=({struct Cyc_RemoveAggrs_Env _tmp84;_tmp84.ctxt=Cyc_RemoveAggrs_NewDest,_tmp84.dest=_tmpF;_tmp84;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp8E,_tmp30);});
*e=*_tmp30;
goto _LL3F;}else{_LL42: _LL43:
({void*_tmpD=0U;({struct _dyneither_ptr _tmp8F=({const char*_tmpE="removeAggrs: toc did not set a new-destination";_tag_dyneither(_tmpE,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp8F,_tag_dyneither(_tmpD,sizeof(void*),0U));});});}_LL3F:;}
# 119
goto _LL0;case 29U: _LL11: _tmp33=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp32=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_tmp31=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA)->f4;_LL12:
# 122
 did_assign=1;{
int do_stmt_exp=1;
if(env.ctxt == Cyc_RemoveAggrs_AggrField  || env.ctxt == Cyc_RemoveAggrs_NewDest)
do_stmt_exp=0;{
struct Cyc_Absyn_Exp*dest;
struct _tuple0**v;
if(do_stmt_exp){
({struct _tuple0**_tmp91=({struct _tuple0**_tmp10=_cycalloc(sizeof(*_tmp10));({struct _tuple0*_tmp90=Cyc_Toc_temp_var();*_tmp10=_tmp90;});_tmp10;});v=_tmp91;});
({struct Cyc_Absyn_Exp*_tmp92=Cyc_Absyn_var_exp(*v,0U);dest=_tmp92;});}else{
# 132
v=0;
dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
if(env.ctxt == Cyc_RemoveAggrs_NewDest)
({struct Cyc_Absyn_Exp*_tmp93=Cyc_Absyn_deref_exp(dest,0U);dest=_tmp93;});}
# 138
{struct Cyc_List_List*_tmp11=_tmp32;for(0;_tmp11 != 0;_tmp11=_tmp11->tl){
struct _tuple10*_tmp12=(struct _tuple10*)_tmp11->hd;struct _tuple10*_tmp13=_tmp12;struct Cyc_List_List*_tmp17;struct Cyc_Absyn_Exp*_tmp16;_LL45: _tmp17=_tmp13->f1;_tmp16=_tmp13->f2;_LL46:;{
struct _dyneither_ptr*_tmp14=Cyc_Absyn_designatorlist_to_fieldname(_tmp17);
struct Cyc_Absyn_Exp*_tmp15=({struct Cyc_Absyn_Exp*_tmp94=Cyc_Absyn_copy_exp(dest);Cyc_RemoveAggrs_member_exp(_tmp94,_tmp14,0U);});
({struct Cyc_RemoveAggrs_Env _tmp95=({struct Cyc_RemoveAggrs_Env _tmp85;_tmp85.ctxt=Cyc_RemoveAggrs_AggrField,_tmp85.dest=_tmp15;_tmp85;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp95,_tmp16);});};}}
# 145
if(_tmp32 == 0)
({void*_tmp18=0U;({struct _dyneither_ptr _tmp96=({const char*_tmp19="zero-field aggregate";_tag_dyneither(_tmp19,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp96,_tag_dyneither(_tmp18,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple10*)_tmp32->hd)).f2;
for(_tmp32=_tmp32->tl;_tmp32 != 0;_tmp32=_tmp32->tl){
({struct Cyc_Absyn_Exp*_tmp97=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple10*)_tmp32->hd)).f2,0U);init_e=_tmp97;});}
if(do_stmt_exp){
void*(*_tmp1A)(struct _tuple0*name)=(unsigned int)_tmp31  && _tmp31->kind == Cyc_Absyn_UnionA?Cyc_Absyn_unionq_typ: Cyc_Absyn_strctq;
({void*_tmp9B=(Cyc_Absyn_stmt_exp(({struct _tuple0*_tmp9A=*((struct _tuple0**)_check_null(v));void*_tmp99=_tmp1A(_tmp33);Cyc_Absyn_declare_stmt(_tmp9A,_tmp99,0,({
struct Cyc_Absyn_Stmt*_tmp98=Cyc_Absyn_exp_stmt(init_e,0U);Cyc_Absyn_seq_stmt(_tmp98,
Cyc_Absyn_exp_stmt(dest,0U),0U);}),0U);}),0U))->r;
# 152
e->r=_tmp9B;});}else{
# 156
e->r=init_e->r;
e->topt=0;}
# 159
goto _LL0;};};};case 26U: _LL13: _tmp34=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL14:
# 164
 res=Cyc_RemoveAggrs_WasArray;
if(_tmp34 == 0){
({struct Cyc_Absyn_Exp _tmp9C=*Cyc_Absyn_signed_int_exp(0,0U);*e=_tmp9C;});
goto _LL0;}
# 169
{enum Cyc_RemoveAggrs_ExpContext _tmp1B=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _tmp1C=_tmp1B;if(_tmp1C == Cyc_RemoveAggrs_Other){_LL48: _LL49:
({void*_tmp1D=0U;({struct _dyneither_ptr _tmp9D=({const char*_tmp1E="remove-aggrs: Array_e in bad position";_tag_dyneither(_tmp1E,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmp9D,_tag_dyneither(_tmp1D,sizeof(void*),0U));});});}else{_LL4A: _LL4B:
 goto _LL47;}_LL47:;}
# 173
did_assign=1;{
struct Cyc_Absyn_Exp*dest=(struct Cyc_Absyn_Exp*)_check_null(env.dest);
# 176
int i=0;
{struct Cyc_List_List*_tmp1F=_tmp34;for(0;_tmp1F != 0;(_tmp1F=_tmp1F->tl,++ i)){
# 180
struct _tuple10*_tmp20=(struct _tuple10*)_tmp1F->hd;struct _tuple10*_tmp21=_tmp20;struct Cyc_Absyn_Exp*_tmp23;_LL4D: _tmp23=_tmp21->f2;_LL4E:;{
struct Cyc_Absyn_Exp*_tmp22=({struct Cyc_Absyn_Exp*_tmp9E=Cyc_Absyn_copy_exp(dest);Cyc_Absyn_subscript_exp(_tmp9E,Cyc_Absyn_signed_int_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _tmp9F=({struct Cyc_RemoveAggrs_Env _tmp86;_tmp86.ctxt=Cyc_RemoveAggrs_AggrField,_tmp86.dest=_tmp22;_tmp86;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmp9F,_tmp23);});};}}{
# 184
struct Cyc_Absyn_Exp*init_e=(*((struct _tuple10*)_tmp34->hd)).f2;
for(_tmp34=_tmp34->tl;_tmp34 != 0;_tmp34=_tmp34->tl){
({struct Cyc_Absyn_Exp*_tmpA0=Cyc_Absyn_seq_exp(init_e,(*((struct _tuple10*)_tmp34->hd)).f2,0U);init_e=_tmpA0;});}
e->r=init_e->r;
e->topt=0;
goto _LL0;};};case 27U: _LL15: _tmp38=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp37=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_tmp36=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_tmp35=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA)->f4;_LL16:
# 193
 did_assign=1;
res=Cyc_RemoveAggrs_WasArray;
{enum Cyc_RemoveAggrs_ExpContext _tmp24=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _tmp25=_tmp24;if(_tmp25 == Cyc_RemoveAggrs_Other){_LL50: _LL51:
({void*_tmp26=0U;({struct _dyneither_ptr _tmpA1=({const char*_tmp27="remove-aggrs: comprehension in bad position";_tag_dyneither(_tmp27,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpA1,_tag_dyneither(_tmp26,sizeof(void*),0U));});});}else{_LL52: _LL53:
 goto _LL4F;}_LL4F:;}{
# 199
struct _tuple0*max=Cyc_Toc_temp_var();
struct _tuple0*i=_tmp38->name;
struct Cyc_Absyn_Exp*ea=({struct Cyc_Absyn_Exp*_tmpA2=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_assign_exp(_tmpA2,Cyc_Absyn_signed_int_exp(0,0U),0U);});
struct Cyc_Absyn_Exp*eb=({struct Cyc_Absyn_Exp*_tmpA3=Cyc_Absyn_var_exp(i,0U);Cyc_Absyn_lt_exp(_tmpA3,Cyc_Absyn_var_exp(max,0U),0U);});
struct Cyc_Absyn_Exp*ec=Cyc_Absyn_pre_inc_exp(Cyc_Absyn_var_exp(i,0U),0U);
struct Cyc_Absyn_Exp*lval=({struct Cyc_Absyn_Exp*_tmpA4=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_subscript_exp(_tmpA4,Cyc_Absyn_var_exp(i,0U),0U);});
({struct Cyc_RemoveAggrs_Env _tmpA6=({struct Cyc_RemoveAggrs_Env _tmp87;_tmp87.ctxt=Cyc_RemoveAggrs_AggrField,({struct Cyc_Absyn_Exp*_tmpA5=Cyc_Absyn_copy_exp(lval);_tmp87.dest=_tmpA5;});_tmp87;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpA6,_tmp36);});{
struct Cyc_Absyn_Stmt*s=({struct Cyc_Absyn_Exp*_tmpA9=ea;struct Cyc_Absyn_Exp*_tmpA8=eb;struct Cyc_Absyn_Exp*_tmpA7=ec;Cyc_Absyn_for_stmt(_tmpA9,_tmpA8,_tmpA7,Cyc_Absyn_exp_stmt(_tmp36,0U),0U);});
if(_tmp35){
# 214
struct Cyc_Absyn_Exp*ex=({struct Cyc_Absyn_Exp*_tmpAB=({struct Cyc_Absyn_Exp*_tmpAA=Cyc_Absyn_copy_exp(env.dest);Cyc_Absyn_subscript_exp(_tmpAA,
Cyc_Absyn_var_exp(max,0U),0U);});
# 214
Cyc_Absyn_assign_exp(_tmpAB,
# 216
Cyc_Absyn_signed_int_exp(0,0U),0U);});
({struct Cyc_Absyn_Stmt*_tmpAD=({struct Cyc_Absyn_Stmt*_tmpAC=s;Cyc_Absyn_seq_stmt(_tmpAC,Cyc_Absyn_exp_stmt(ex,0U),0U);});s=_tmpAD;});}
# 220
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp37);
({void*_tmpB2=(Cyc_Absyn_stmt_exp(({struct Cyc_Absyn_Stmt*_tmpB1=({struct _tuple0*_tmpB0=max;void*_tmpAF=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpAE=_tmp37;Cyc_Absyn_declare_stmt(_tmpB0,_tmpAF,_tmpAE,
Cyc_Absyn_declare_stmt(i,Cyc_Absyn_uint_typ,0,s,0U),0U);});
# 221
Cyc_Absyn_seq_stmt(_tmpB1,
# 223
Cyc_Absyn_exp_stmt(Cyc_Absyn_signed_int_exp(0,0U),0U),0U);}),0U))->r;
# 221
e->r=_tmpB2;});
# 224
e->topt=0;
goto _LL0;};};case 28U: _LL17: _tmp39=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL18:
# 228
 did_assign=1;
res=Cyc_RemoveAggrs_WasArray;
{enum Cyc_RemoveAggrs_ExpContext _tmp28=env.ctxt;enum Cyc_RemoveAggrs_ExpContext _tmp29=_tmp28;if(_tmp29 == Cyc_RemoveAggrs_Other){_LL55: _LL56:
({void*_tmp2A=0U;({struct _dyneither_ptr _tmpB3=({const char*_tmp2B="remove-aggrs: no-init-comp in bad position";_tag_dyneither(_tmp2B,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB3,_tag_dyneither(_tmp2A,sizeof(void*),0U));});});}else{_LL57: _LL58:
 goto _LL54;}_LL54:;}
# 235
*e=*_tmp39;
goto _LL0;case 4U: _LL19: _tmp3B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp3A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_LL1A:
# 239
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp3B);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp3A);
goto _LL0;case 9U: _LL1B: _tmp3D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp3C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL1C:
# 243
 did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp3D);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,_tmp3C);
goto _LL0;case 6U: _LL1D: _tmp40=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp3F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_tmp3E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA)->f3;_LL1E:
# 248
 did_assign=1;
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp40);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(env,_tmp3F);{
struct Cyc_RemoveAggrs_Env _tmp2C=({struct Cyc_RemoveAggrs_Env _tmp88;_tmp88.ctxt=env.ctxt,env.dest == 0?_tmp88.dest=0:({struct Cyc_Absyn_Exp*_tmpB4=Cyc_Absyn_copy_exp(env.dest);_tmp88.dest=_tmpB4;});_tmp88;});
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(_tmp2C,_tmp3E);
goto _LL0;};case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA)->f3 == 0){_LL1F: _tmp42=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp41=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL20:
# 256
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp42);
_tmp43=_tmp41;goto _LL22;}else{goto _LL3D;}case 3U: _LL21: _tmp43=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL22:
# 259
 for(0;_tmp43 != 0;_tmp43=_tmp43->tl){
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,(struct Cyc_Absyn_Exp*)_tmp43->hd);}
goto _LL0;case 23U: _LL23: _tmp45=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp44=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL24:
# 263
 _tmp47=_tmp45;_tmp46=_tmp44;goto _LL26;case 7U: _LL25: _tmp47=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp46=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL26:
 _tmp49=_tmp47;_tmp48=_tmp46;goto _LL28;case 8U: _LL27: _tmp49=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_tmp48=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL28:
# 266
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp49);
Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp48);
goto _LL0;case 13U: _LL29: _tmp4A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL2A:
# 271
 _tmp4B=_tmp4A;goto _LL2C;case 12U: _LL2B: _tmp4B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL2C:
 _tmp4C=_tmp4B;goto _LL2E;case 14U: _LL2D: _tmp4C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA)->f2;_LL2E:
 _tmp4D=_tmp4C;goto _LL30;case 15U: _LL2F: _tmp4D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL30:
 _tmp4E=_tmp4D;goto _LL32;case 20U: _LL31: _tmp4E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL32:
 _tmp4F=_tmp4E;goto _LL34;case 21U: _LL33: _tmp4F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL34:
 _tmp50=_tmp4F;goto _LL36;case 22U: _LL35: _tmp50=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL36:
 _tmp51=_tmp50;goto _LL38;case 18U: _LL37: _tmp51=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL38:
 _tmp52=_tmp51;goto _LL3A;case 5U: _LL39: _tmp52=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL3A:
# 280
 Cyc_RemoveAggrs_noarray_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp52);
goto _LL0;case 37U: _LL3B: _tmp53=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA)->f1;_LL3C:
# 284
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp53);
goto _LL0;default: _LL3D: _LL3E:
# 287
({struct Cyc_String_pa_PrintArg_struct _tmp2F=({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0U,({
struct _dyneither_ptr _tmpB5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp89.f1=_tmpB5;});_tmp89;});void*_tmp2D[1U];_tmp2D[0]=& _tmp2F;({struct _dyneither_ptr _tmpB6=({const char*_tmp2E="bad exp after translation to C: %s";_tag_dyneither(_tmp2E,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpB6,_tag_dyneither(_tmp2D,sizeof(void*),1U));});});}_LL0:;}
# 290
if(env.ctxt == Cyc_RemoveAggrs_AggrField  && !did_assign)
({void*_tmpB8=({struct Cyc_Absyn_Exp*_tmpB7=(struct Cyc_Absyn_Exp*)_check_null(env.dest);Cyc_Absyn_assign_exp(_tmpB7,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmpB8;});
if(env.ctxt == Cyc_RemoveAggrs_NewDest  && !did_assign)
({void*_tmpBA=({struct Cyc_Absyn_Exp*_tmpB9=Cyc_Absyn_deref_exp((struct Cyc_Absyn_Exp*)_check_null(env.dest),0U);Cyc_Absyn_assign_exp(_tmpB9,Cyc_Absyn_copy_exp(e),0U);})->r;e->r=_tmpBA;});
return({struct Cyc_RemoveAggrs_Result _tmp8A;_tmp8A.res=res;_tmp8A;});}
# 301
static void Cyc_RemoveAggrs_remove_aggrs_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp54=s->r;void*_tmp55=_tmp54;struct Cyc_Absyn_Decl*_tmp74;struct Cyc_Absyn_Stmt*_tmp73;struct Cyc_Absyn_Exp*_tmp72;struct Cyc_List_List*_tmp71;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_Absyn_Stmt*_tmp6D;struct Cyc_Absyn_Exp*_tmp6C;struct Cyc_Absyn_Stmt*_tmp6B;struct Cyc_Absyn_Stmt*_tmp6A;struct Cyc_Absyn_Exp*_tmp69;struct Cyc_Absyn_Exp*_tmp68;struct Cyc_Absyn_Stmt*_tmp67;struct Cyc_Absyn_Stmt*_tmp66;struct Cyc_Absyn_Stmt*_tmp65;struct Cyc_Absyn_Stmt*_tmp64;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_Absyn_Exp*_tmp62;struct Cyc_Absyn_Stmt*_tmp61;switch(*((int*)_tmp55)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 goto _LL8;case 8U: _LL7: _LL8:
 return;case 13U: _LL9: _tmp61=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_LLA:
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp61);return;case 3U: _LLB: _tmp62=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_LLC:
# 309
 if(_tmp62 == 0)
return;
_tmp63=_tmp62;goto _LLE;case 1U: _LLD: _tmp63=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_LLE:
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp63);return;case 2U: _LLF: _tmp65=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp64=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_LL10:
# 314
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp65);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp64);
return;case 4U: _LL11: _tmp68=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp67=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_tmp66=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp55)->f3;_LL12:
# 318
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp68);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp67);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp66);
return;case 14U: _LL13: _tmp6A=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp69=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp55)->f2).f1;_LL14:
 _tmp6C=_tmp69;_tmp6B=_tmp6A;goto _LL16;case 5U: _LL15: _tmp6C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp55)->f1).f1;_tmp6B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_LL16:
# 324
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp6C);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp6B);
return;case 9U: _LL17: _tmp70=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp6F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f2).f1;_tmp6E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f3).f1;_tmp6D=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp55)->f4;_LL18:
# 328
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp70);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp6F);
Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp6E);
Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp6D);
return;case 10U: _LL19: _tmp72=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp71=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_LL1A:
# 336
 Cyc_RemoveAggrs_remove_aggrs_exp(Cyc_RemoveAggrs_other_env,_tmp72);
for(0;_tmp71 != 0;_tmp71=_tmp71->tl){
Cyc_RemoveAggrs_remove_aggrs_stmt(((struct Cyc_Absyn_Switch_clause*)_tmp71->hd)->body);}
return;case 12U: _LL1B: _tmp74=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp55)->f1;_tmp73=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp55)->f2;_LL1C:
# 341
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp73);
{void*_tmp56=_tmp74->r;void*_tmp57=_tmp56;struct Cyc_Absyn_Fndecl*_tmp5E;struct Cyc_Absyn_Vardecl*_tmp5D;switch(*((int*)_tmp57)){case 0U: _LL20: _tmp5D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp57)->f1;_LL21:
# 344
 if(_tmp5D->sc == Cyc_Absyn_Static){
Cyc_RemoveAggrs_massage_toplevel_aggr(_tmp5D->initializer);
goto _LL1F;}
# 355
if(_tmp5D->initializer != 0){
struct Cyc_RemoveAggrs_Result _tmp58=({struct Cyc_RemoveAggrs_Env _tmpBC=({struct Cyc_RemoveAggrs_Env _tmp8B;_tmp8B.ctxt=Cyc_RemoveAggrs_Initializer,({struct Cyc_Absyn_Exp*_tmpBB=Cyc_Absyn_var_exp(_tmp5D->name,0U);_tmp8B.dest=_tmpBB;});_tmp8B;});Cyc_RemoveAggrs_remove_aggrs_exp(_tmpBC,(struct Cyc_Absyn_Exp*)_check_null(_tmp5D->initializer));});struct Cyc_RemoveAggrs_Result _tmp59=_tmp58;enum Cyc_RemoveAggrs_ExpResult _tmp5A;_LL27: _tmp5A=_tmp59.res;_LL28:;
# 358
if(_tmp5A == Cyc_RemoveAggrs_WasArray){
({void*_tmpBF=({struct Cyc_Absyn_Decl*_tmpBE=_tmp74;Cyc_Absyn_decl_stmt(_tmpBE,({struct Cyc_Absyn_Stmt*_tmpBD=Cyc_Absyn_exp_stmt((struct Cyc_Absyn_Exp*)_check_null(_tmp5D->initializer),0U);Cyc_Absyn_seq_stmt(_tmpBD,_tmp73,0U);}),0U);})->r;s->r=_tmpBF;});
# 361
_tmp5D->initializer=0;}}
# 364
goto _LL1F;case 1U: _LL22: _tmp5E=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp57)->f1;_LL23:
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp5E->body);goto _LL1F;default: _LL24: _LL25:
({void*_tmp5B=0U;({struct _dyneither_ptr _tmpC0=({const char*_tmp5C="bad local declaration after xlation to C";_tag_dyneither(_tmp5C,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC0,_tag_dyneither(_tmp5B,sizeof(void*),0U));});});}_LL1F:;}
# 368
return;default: _LL1D: _LL1E:
({void*_tmp5F=0U;({struct _dyneither_ptr _tmpC1=({const char*_tmp60="bad stmt after xlation to C";_tag_dyneither(_tmp60,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC1,_tag_dyneither(_tmp5F,sizeof(void*),0U));});});}_LL0:;}
# 373
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*ds){
{struct Cyc_List_List*_tmp75=ds;for(0;_tmp75 != 0;_tmp75=_tmp75->tl){
void*_tmp76=((struct Cyc_Absyn_Decl*)_tmp75->hd)->r;void*_tmp77=_tmp76;struct Cyc_List_List*_tmp82;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp80;struct Cyc_List_List*_tmp7F;struct Cyc_Absyn_Aggrdecl*_tmp7E;struct Cyc_Absyn_Vardecl*_tmp7D;struct Cyc_Absyn_Fndecl*_tmp7C;switch(*((int*)_tmp77)){case 1U: _LL1: _tmp7C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp77)->f1;_LL2:
 Cyc_RemoveAggrs_remove_aggrs_stmt(_tmp7C->body);goto _LL0;case 0U: _LL3: _tmp7D=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp77)->f1;_LL4:
 Cyc_RemoveAggrs_massage_toplevel_aggr(_tmp7D->initializer);goto _LL0;case 5U: _LL5: _tmp7E=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp77)->f1;_LL6:
 goto _LL8;case 7U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 goto _LL0;case 4U: _LLB: _LLC:
 goto _LLE;case 6U: _LLD: _LLE:
 goto _LL10;case 2U: _LLF: _LL10:
 goto _LL12;case 3U: _LL11: _LL12:
({void*_tmp78=0U;({struct _dyneither_ptr _tmpC2=({const char*_tmp79="Cyclone decl after xlation to C";_tag_dyneither(_tmp79,sizeof(char),32U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC2,_tag_dyneither(_tmp78,sizeof(void*),0U));});});case 9U: _LL13: _tmp7F=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp77)->f2;_LL14:
 _tmp80=_tmp7F;goto _LL16;case 10U: _LL15: _tmp80=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp77)->f2;_LL16:
 _tmp81=_tmp80;goto _LL18;case 11U: _LL17: _tmp81=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp77)->f1;_LL18:
 _tmp82=_tmp81;goto _LL1A;case 12U: _LL19: _tmp82=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp77)->f1;_LL1A:
 goto _LL1C;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
({void*_tmp7A=0U;({struct _dyneither_ptr _tmpC3=({const char*_tmp7B="nested translation unit after xlation to C";_tag_dyneither(_tmp7B,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Warn_impos)(_tmpC3,_tag_dyneither(_tmp7A,sizeof(void*),0U));});});}_LL0:;}}
# 392
return ds;}
